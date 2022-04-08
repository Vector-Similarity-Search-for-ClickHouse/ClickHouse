#pragma once

#include <Storages/MergeTree/MergeTreeIndices.h>
#include <Storages/MergeTree/MergeTreeData.h>
#include <Storages/MergeTree/KeyCondition.h>

#include <faiss/IndexLSH.h>
#include "Storages/MergeTree/MergeTreeIndexSet.h"
#include "Storages/SelectQueryInfo.h"
#include "base/types.h"
#include "Parsers/IAST_fwd.h"
#include "Interpreters/Context_fwd.h"

#include <memory>
#include <optional>
#include <vector>

namespace DB {

struct MergeTreeIndexGranuleFaissLSH final : public IMergeTreeIndexGranule
{
    /// Should think about arguments
    MergeTreeIndexGranuleFaissLSH(
        const String & index_name_, 
        const Block & sample_block);

    MergeTreeIndexGranuleFaissLSH(
        const String & index_name_,
        const Block & sample_block,
        std::shared_ptr<faiss::IndexLSH> index);

    ~MergeTreeIndexGranuleFaissLSH() override = default;

    void serializeBinary(WriteBuffer & ostr) const override;
    void deserializeBinary(ReadBuffer & istr, MergeTreeIndexVersion version) override;

    bool empty() const override;
    String getOnlyColoumnName() const;

    String index_name;
    Block index_sample_block;
    std::shared_ptr<faiss::IndexLSH> index_impl;
};

struct MergeTreeIndexAggregatorFaissLSH final : IMergeTreeIndexAggregator
{
    MergeTreeIndexAggregatorFaissLSH(
        const String & index_name_,
        const Block & sample_block);

    MergeTreeIndexAggregatorFaissLSH(
        const String & index_name_, 
        const Block & sample_block,
        const std::shared_ptr<faiss::IndexLSH> & index);

    ~MergeTreeIndexAggregatorFaissLSH() override = default;

    bool empty() const override;
    MergeTreeIndexGranulePtr getGranuleAndReset() override;
    void update(const Block & block, size_t * pos, size_t limit) override;

    String index_name;
    Block index_sample_block;
    std::shared_ptr<faiss::IndexLSH> index_impl;
};

class CommonCondition 
{
public:

    CommonCondition(const SelectQueryInfo & query_info,
                    ContextPtr context);

    bool alwaysUnknownOrTrue() const;

    std::optional<float> getComparisonDistance() const;

    std::optional<std::vector<float>> getTargetVector() const;

    std::optional<String> getColumnName() const;

    std::optional<String> getMetric() const;

private:
    // Type of the vector to use as a target in the distance function
    using Target = std::vector<float>;

    // Extracted data from the query like WHERE L2Distance(column_name, target) < distance
    struct ANNExpression {
        Target target;
        float distance;     
        String meteic_name; // Metic name, maybe some Enum for all indices
        String column_name; // Coloumn name stored in IndexGranule
    };

    using ANNExpressionOpt = std::optional<ANNExpression>;
    struct RPNElement {
        enum Function {
            // l2 dist
            FUNCTION_DISTANCE,

            //tuple(10, 15)
            FUNCTION_TUPLE,

            // Operator <
            FUNCTION_LESS,

            // Numeric float value
            FUNCTION_FLOAT_LITERAL,

            // Column identifier
            FUNCTION_IDENTIFIER,

            // Unknown, can be any value
            FUNCTION_UNKNOWN,
        };

        explicit RPNElement(Function function_ = FUNCTION_UNKNOWN) 
        : function(function_), func_name("Unknown"), float_literal(std::nullopt), identifier(std::nullopt) {}

        Function function;
        String func_name;

        std::optional<float> float_literal;
        std::optional<String> identifier;

        UInt32 dim{0};
    };

    using RPN = std::vector<RPNElement>;

    void buildRPN(const SelectQueryInfo & query, ContextPtr context);

    // Util functions for the traversal of AST
    void traverseAST(const ASTPtr & node, RPN & rpn);
    // Return true if we can identify our node type
    bool traverseAtomAST(const ASTPtr & node, RPNElement & out);

    // Checks that at least one rpn is matching for index
    // New RPNs for other query types can be added here
    bool matchAllRPNS();

    /* Returns true and stores ANNExpr if the querry matches the template:
     * WHERE DistFunc(column_name, tuple(float_1, float_2, ..., float_dim)) < float_literal */
    static bool matchRPNWhere(RPN & rpn, ANNExpression & expr);

    // Util methods
    static void panicIfWrongBuiltRPN [[noreturn]] (); 

    static String getIdentifierOrPanic(RPN::iterator& iter);

    static float getFloatLiteralOrPanic(RPN::iterator& iter);

    
    // One of them can be empty
    RPN rpn_prewhere_clause;
    RPN rpn_where_clause;

    Block block_with_constants;

    ANNExpressionOpt expression{std::nullopt};

    // true if we had extracted ANNExpression from querry
    bool index_is_useful{false};

};

class MergeTreeIndexConditionFaissLSH final : public IMergeTreeIndexCondition
{
public:
    explicit MergeTreeIndexConditionFaissLSH(
       const IndexDescription & index,
        const SelectQueryInfo & query,
        ContextPtr context);

    bool alwaysUnknownOrTrue() const override;

    bool mayBeTrueOnGranule(MergeTreeIndexGranulePtr idx_granule) const override;

    ~MergeTreeIndexConditionFaissLSH() override = default;
private:
    void traverseAST(ASTPtr & node) const;
    bool atomFromAST(ASTPtr & node) const;
    static bool operatorFromAST(ASTPtr & node);

    bool checkASTUseless(const ASTPtr & node, bool atomic = false) const;


    String index_name;
    CommonCondition condition;
};

class MergeTreeIndexFaissLSH : public IMergeTreeIndex
{
public:
    explicit MergeTreeIndexFaissLSH(const IndexDescription & index_);
    ~MergeTreeIndexFaissLSH() override = default;

    MergeTreeIndexGranulePtr createIndexGranule() const override;
    MergeTreeIndexAggregatorPtr createIndexAggregator() const override;

    MergeTreeIndexConditionPtr createIndexCondition(
        const SelectQueryInfo & query, ContextPtr context) const override;

    bool mayBenefitFromIndexForIn(const ASTPtr & node) const override;

    /// Questions, questions
    const char* getSerializedFileExtension() const override;
    MergeTreeIndexFormat getDeserializedFormat(const DiskPtr disk, const std::string & path_prefix) const override;
};

}
