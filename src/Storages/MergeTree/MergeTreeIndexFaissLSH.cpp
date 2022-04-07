#include <cstddef>
#include <exception>
#include <memory>
#include <Storages/MergeTree/MergeTreeIndexFaissLSH.h>

#include <Parsers/ASTFunction.h>

#include <faiss/IndexFlat.h>
#include <faiss/IndexIVF.h>
#include <faiss/IndexIVFFlat.h>
#include <faiss/IndexLSH.h>
#include <faiss/MetricType.h>
#include <faiss/impl/io.h>
#include <faiss/index_io.h>
#include "Common/typeid_cast.h"
#include "Storages/MergeTree/KeyCondition.h"

#include "Core/Field.h"
#include "Interpreters/Context_fwd.h"
#include "MergeTreeIndices.h"
#include "KeyCondition.h"
#include "Parsers/ASTIdentifier.h"
#include "Parsers/ASTSelectQuery.h"
#include "Parsers/IAST_fwd.h"
#include "Storages/SelectQueryInfo.h"
#include "base/types.h"

namespace DB {

/// Serialization helpers //////////////////////////////////////////////////////////////

namespace Detail {
  class WriteBufferFaissWrapper : public faiss::IOWriter {
    public:
        explicit WriteBufferFaissWrapper(WriteBuffer & ostr_)
        : ostr(ostr_) {}

        size_t operator()(const void* ptr, size_t size, size_t nitems) override 
        {
            ostr.write(reinterpret_cast<const char*>(ptr), size * nitems);

            // WriteBuffer guarantees to write all items
            return nitems;
        }

    private:
        WriteBuffer & ostr;
    };

  class ReadBufferFaissWrapper : public faiss::IOReader {
    public:        
        explicit ReadBufferFaissWrapper(ReadBuffer & istr_)
        : istr(istr_) {}

        size_t operator()(void* ptr, size_t size, size_t nitems) override 
        {
            return istr.read(reinterpret_cast<char*>(ptr), size * nitems) / size;
        }

    private:
        ReadBuffer & istr;
    };
}

/// Granule ////////////////////////////////////////////////////////////////////////////

    MergeTreeIndexGranuleFaissLSH::MergeTreeIndexGranuleFaissLSH(
        const String & index_name_, 
        const Block & sample_block)
        : index_name(index_name_), 
          index_sample_block(sample_block), 
          index_impl(nullptr) {}

    MergeTreeIndexGranuleFaissLSH::MergeTreeIndexGranuleFaissLSH(
        const String & index_name_,
        const Block & sample_block,
        std::shared_ptr<faiss::IndexLSH> index) 
        : index_name(index_name_), 
          index_sample_block(sample_block), 
          index_impl(std::move(index)) {}

    void MergeTreeIndexGranuleFaissLSH::serializeBinary(WriteBuffer &ostr) const {
        Detail::WriteBufferFaissWrapper ostr_wrapped(ostr);
        faiss::write_index(index_impl.get(), &ostr_wrapped);
    }

    void MergeTreeIndexGranuleFaissLSH::deserializeBinary(ReadBuffer &istr, MergeTreeIndexVersion version) {

        if (version != 1)
            throw Exception(ErrorCodes::LOGICAL_ERROR, "Unknown index version {}.", version);

        Detail::ReadBufferFaissWrapper istr_wrapped(istr);
        index_impl = std::shared_ptr<faiss::IndexLSH>(dynamic_cast<faiss::IndexLSH*>(faiss::read_index(&istr_wrapped)));

    }

    bool MergeTreeIndexGranuleFaissLSH::empty() const {
        return index_impl == nullptr;
    }

/// Aggregator /////////////////////////////////////////////////////////////////////////

    MergeTreeIndexAggregatorFaissLSH::MergeTreeIndexAggregatorFaissLSH(
        const String & index_name_,
        const Block & sample_block)
        : index_name(index_name_),
          index_sample_block(sample_block),
          index_impl(nullptr) {}

    MergeTreeIndexAggregatorFaissLSH::MergeTreeIndexAggregatorFaissLSH(
        const String & index_name_, 
        const Block & sample_block,
        const std::shared_ptr<faiss::IndexLSH> & index)
        : index_name(index_name_),
          index_sample_block(sample_block),
          index_impl(index) {}    

    bool MergeTreeIndexAggregatorFaissLSH::empty() const {
        // should think about it
        return index_impl == nullptr;
    }

    MergeTreeIndexGranulePtr MergeTreeIndexAggregatorFaissLSH::getGranuleAndReset() {
        return std::make_shared<MergeTreeIndexGranuleFaissLSH>(index_name, index_sample_block, std::move(index_impl));
    }

    void MergeTreeIndexAggregatorFaissLSH::update(const Block &block, size_t *pos, size_t limit) {
        if (*pos >= block.rows())
        throw Exception(
                "The provided position is not less than the number of block rows. Position: "
                + toString(*pos) + ", Block rows: " + toString(block.rows()) + ".", ErrorCodes::LOGICAL_ERROR);

        size_t rows_read = std::min(limit, block.rows() - *pos);

        const Names index_columns = index_sample_block.getNames();
        for (const auto & column_name : index_columns) {
            const auto & column = block.getByName(column_name).column->cut(*pos, rows_read);
            std::vector<Float32> points;

            // Get data from column
            for (size_t i = 0; i < rows_read; ++i) {
                Field field;
                column->get(i, field);
                auto field_array = field.safeGet<Tuple>();

                for (const auto& value : field_array) {
                    points.push_back(value.safeGet<Float32>());
                }
            }

            // Update by training
            int64_t n = points.size() / index_impl->rrot.d_in;
            try {
                index_impl->train(n, points.data());
            } catch(...) {
                throw Exception(
                "Something went worng while training the index"
                , ErrorCodes::LOGICAL_ERROR);
            }
        }

        *pos += rows_read;
    }

/// Condition //////////////////////////////////////////////////////////////////////////


bool MergeTreeIndexConditionFaissLSH::alwaysUnknownOrTrue() const {
    /// ???
    return true;
}

bool MergeTreeIndexConditionFaissLSH::mayBeTrueOnGranule(MergeTreeIndexGranulePtr /*idx_granule*/) const {
    /// ???
    return true;
}

// Common Condition ////////////////////////////////////////////////////////////////////

CommonCondition::CommonCondition(const SelectQueryInfo & query_info,
                                 ContextPtr context,
                                 const Names & /*key_column_names*/,
                                 const ExpressionActionsPtr & /*key_expr*/) {
    buildRPN(query_info, context);
    matchAllRPNS();
}

void CommonCondition::buildRPN(const SelectQueryInfo & query, ContextPtr context) {

    block_with_constants = KeyCondition::getBlockWithConstants(query.query, query.syntax_analyzer_result, context);

    const auto & select = query.query->as<ASTSelectQuery &>();

    if (select.prewhere()) {
        traverseAST(select.prewhere(), rpn_prewhere_clause);
    }

    if (select.where()) {
        traverseAST(select.where(), rpn_where_clause); 
    }

    std::reverse(rpn_prewhere_clause.begin(), rpn_prewhere_clause.end());
    std::reverse(rpn_where_clause.begin(), rpn_where_clause.end());

}

void CommonCondition::traverseAST(const ASTPtr & node, RPN & rpn) {
    if (const auto * func = node->as<ASTFunction>()) {
        const ASTs & args = func->arguments->children;

        for (const auto& arg : args) {
            traverseAST(arg, rpn);  
        }
    }

    RPNElement element;

    if(!traverseAtomAST(node, element)) {
        element.function = RPNElement::FUNCTION_UNKNOWN;
    }

    rpn.emplace_back(std::move(element)); 
}

 bool CommonCondition::traverseAtomAST(const ASTPtr & node, RPNElement & out) {
     // Firstly check if we have constants behind the node
    {
        Field const_value;
        DataTypePtr const_type;


        if (KeyCondition::getConstant(node, block_with_constants, const_value, const_type))
        {
            /// Check constant type (use Float64 because all Fields implementation contains Float64 (for Float32 too))
            if (const_value.getType() == Field::Types::Float64)
            {
                out.function = RPNElement::FUNCTION_FLOAT_LITERAL;
                out.float_literal.emplace(const_value.get<Float32>());
                out.func_name = "Float literal";

                return true;
            }
        }
    }

     if (const auto * function = node->as<ASTFunction>())
    {
        // Set the name
        out.func_name = function->name;


        // TODO: Add support for other metrics 
        if (function->name == "L2Distance") 
        {
            out.function = RPNElement::FUNCTION_DISTANCE;
        } 
        else if (function->name == "tuple") 
        {
            out.function = RPNElement::FUNCTION_TUPLE;
        } 
        else if (function->name == "less") 
        {
            out.function = RPNElement::FUNCTION_LESS;
        } 
        else 
        {
            return false;
        }

        return true;
    }
    // Match identifier 
    else if (const auto * identifier = node->as<ASTIdentifier>()) 
    {
        out.function = RPNElement::FUNCTION_IDENTIFIER;
        out.identifier.emplace(identifier->name());
        out.func_name = "column identifier";

        return true;
    } 

    return false;
 }

 namespace ErrorCodes
{
    extern const int CANNOT_PARSE_INPUT_ASSERTION_FAILED;
}

bool CommonCondition::matchAllRPNS() {
    ANNExpression expr_prewhere;
    ANNExpression expr_where;
    bool prewhere_is_valid = matchRPNWhere(rpn_prewhere_clause, expr_prewhere);
    bool where_is_valid = matchRPNWhere(rpn_where_clause, expr_where);

    // Not valid querry for the index
    if (!prewhere_is_valid && !where_is_valid) {
        return false;
    }

    // Unxpected situation
    if (prewhere_is_valid && where_is_valid) {
        throw Exception(
            "Expected to have only `where` or only `prewhere` querry part", ErrorCodes::CANNOT_PARSE_INPUT_ASSERTION_FAILED);
    }

    expression = std::move(where_is_valid ? expr_where : expr_prewhere);
    return true;
 }

bool CommonCondition::matchRPNWhere(RPN & rpn, ANNExpression & expr) {
    const size_t minimal_elemets_count = 6;// At least 6 AST nodes in querry
    if (rpn.size() < minimal_elemets_count) {
        return false;
    }

    auto iter = rpn.begin();

    // Query starts from operator less
    if (iter->function != RPNElement::FUNCTION_LESS) {
        return false;
    }

    ++iter;
    bool less_literal_found = false;

    // After less operator can be the second child - literal for comparison
    if (iter->function == RPNElement::FUNCTION_FLOAT_LITERAL) {
        expr.distance = getFloatLiteralOrPanic(iter);
        less_literal_found = true;
        ++iter;
    }

    // Expected Distance fucntion after less function
    if (iter->function != RPNElement::FUNCTION_DISTANCE) {
        return false;
    }
    expr.meteic_name = iter->func_name;

    ++iter;
    bool identifier_found = false;

    // Can be identifier after DistanceFunction. Also it can be later
    if (iter->function == RPNElement::FUNCTION_IDENTIFIER) {
        expr.column_name = getIdentifierOrPanic(iter);
        identifier_found = true;
        ++iter;
    }

    // Expected tuple construction after DistanceFuction
    if (iter->function != RPNElement::FUNCTION_TUPLE) {
        return false;
    }

    ++iter;

    // Getting query's rest part, there should be floats and maybe identifier
    while(iter != rpn.end()) {
        if(iter->function == RPNElement::FUNCTION_FLOAT_LITERAL) 
        {
            expr.target.emplace_back(getFloatLiteralOrPanic(iter));
        } 
        else if(iter->function == RPNElement::FUNCTION_IDENTIFIER)
        {
            if(identifier_found) {
                return false;
            }
            expr.column_name = getIdentifierOrPanic(iter);
            identifier_found = true;
        } 
        else 
        {
            return false;
        }

        ++iter;
    }

    // Final checks of correctness
    if (!identifier_found || expr.target.empty()) {
        return false;
    }

    // If we hadn't found float literal for less operator, than
    // it was the last element in RPN
    if (!less_literal_found) {
        // In this case needs at least 2 float literals
        if (expr.target.size() >= 2) {
            expr.distance = expr.target.back();
            expr.target.pop_back();
            less_literal_found = true;
        } else {
            return false;
        }
    }

    // Querry is ok
    return true;
}

String CommonCondition::getIdentifierOrPanic(RPN::iterator& iter) {
    String identifier;
    try {
        identifier = std::move(iter->identifier.value());
    } catch(...) {
        CommonCondition::panicIfWrongBuiltRPN();
    }
    return identifier;
}

float CommonCondition::getFloatLiteralOrPanic(RPN::iterator& iter) {
    float literal = 0.0;
    try {
        literal = iter->float_literal.value();
    } catch(...) {
        CommonCondition::panicIfWrongBuiltRPN();
    }
    return literal;
}

void CommonCondition::panicIfWrongBuiltRPN() {
    throw Exception(
                "Wrong parsed AST in buildRPN\n", ErrorCodes::LOGICAL_ERROR);
}


/// Index //////////////////////////////////////////////////////////////////////////////

MergeTreeIndexFaissLSH::MergeTreeIndexFaissLSH(const IndexDescription & index_)
        : IMergeTreeIndex(index_) {}

MergeTreeIndexGranulePtr MergeTreeIndexFaissLSH::createIndexGranule() const {
    /// need index_impl constructed here
    return std::make_shared<MergeTreeIndexGranuleFaissLSH>(index.name, index.sample_block);
}

MergeTreeIndexAggregatorPtr MergeTreeIndexFaissLSH::createIndexAggregator() const {
    /// need index_impl constructed here
    return std::make_shared<MergeTreeIndexAggregatorFaissLSH>(index.name, index.sample_block);
}

const char* MergeTreeIndexFaissLSH::getSerializedFileExtension() const {
     return ".idxFaissLSH"; 
}


/// Registration ///////////////////////////////////////////////////////////////////////



    MergeTreeIndexPtr faissLSHIndexCreator(const IndexDescription & index) {
        return std::make_shared<MergeTreeIndexFaissLSH>(index);
    }

    void faissLSHIndexValidator(const IndexDescription & /*index*/, bool /*attach*/) {
        /// ???
    }
}
