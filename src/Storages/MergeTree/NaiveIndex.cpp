#include <memory>
#include <Storages/MergeTree/NaiveIndex.h>

#include <Interpreters/ExpressionActions.h>
#include <Interpreters/ExpressionAnalyzer.h>
#include <Interpreters/TreeRewriter.h>

#include <Parsers/ASTFunction.h>

#include <Poco/Logger.h>
#include <Common/FieldVisitorsAccurateComparison.h>
#include "Storages/MergeTree/MergeTreeIndices.h"

#include <chrono>
#include <random>

namespace DB {

namespace ErrorCodes {

    extern const int LOGICAL_ERROR;

}

    MergeTreeIndexGranuleNaive::MergeTreeIndexGranuleNaive(const String& index_name_, int64_t num_)
     : index_name(index_name_), random_number(num_) {
         if (num_ == 0) {
            auto seed = std::chrono::system_clock::now().time_since_epoch().count();
            std::mt19937 generator(seed);
            std::uniform_int_distribution<int> distribution(1, 10);
            random_number = distribution(generator);
         }
     }


    MergeTreeIndexAggregatorNaive::MergeTreeIndexAggregatorNaive(const String& index_name_, int64_t num_) :
        index_name(index_name_), random_number(num_) {
            if (num_ == 0) {
            auto seed = std::chrono::system_clock::now().time_since_epoch().count();
            std::mt19937 generator(seed);
            std::uniform_int_distribution<int> distribution(1, 10);
            random_number = distribution(generator);
         }
        }

    MergeTreeIndexGranulePtr MergeTreeIndexAggregatorNaive::getGranuleAndReset() {
        return std::make_shared<MergeTreeIndexGranuleNaive>(index_name, random_number);
    }

    void MergeTreeIndexAggregatorNaive::update(const Block &block, size_t *pos, size_t limit) {
        if (*pos >= block.rows()) {
        throw Exception(
                "The provided position is not less than the number of block rows. Position: "
                + toString(*pos) + ", Block rows: " + toString(block.rows()) + ".", ErrorCodes::LOGICAL_ERROR);
        }

         size_t rows_read = std::min(limit, block.rows() - *pos);

            /// Index Metadata update

         *pos += rows_read;
    }

    MergeTreeIndexConditionNaive::MergeTreeIndexConditionNaive(
        const IndexDescription & index,
        const SelectQueryInfo & query,
        ContextPtr context)
    : index_data_types(index.data_types)
    , condition(query, context, index.column_names, index.expression) {}

    bool MergeTreeIndexConditionNaive::alwaysUnknownOrTrue() const {
        return false;
    }

    bool MergeTreeIndexConditionNaive::mayBeTrueOnGranule(MergeTreeIndexGranulePtr idx_granule) const {
        std::shared_ptr<MergeTreeIndexGranuleNaive> granule
            = std::dynamic_pointer_cast<MergeTreeIndexGranuleNaive>(idx_granule);
        if (!granule)
            throw Exception(
                "Minmax index condition got a granule with the wrong type.", ErrorCodes::LOGICAL_ERROR);

        return granule->random_number % 3 == 1;
    }

    MergeTreeIndexGranulePtr MergeTreeIndexNaive::createIndexGranule() const {
        return std::make_shared<MergeTreeIndexGranuleNaive>(index.name);
    }

    MergeTreeIndexAggregatorPtr MergeTreeIndexNaive::createIndexAggregator() const {
        return std::make_shared<MergeTreeIndexAggregatorNaive>(index.name);
    }

    MergeTreeIndexConditionPtr MergeTreeIndexNaive::createIndexCondition(
        const SelectQueryInfo & query, ContextPtr context) const {
        return std::make_shared<MergeTreeIndexConditionNaive>(index, query, context);
    }

    bool MergeTreeIndexNaive::mayBenefitFromIndexForIn(const ASTPtr & node) const {
        const String column_name = node->getColumnName();

        for (const auto & cname : index.column_names)
            if (column_name == cname)
                return true;

        if (const auto * func = typeid_cast<const ASTFunction *>(node.get()))
            if (func->arguments->children.size() == 1)
                return mayBenefitFromIndexForIn(func->arguments->children.front());

        return false;
    }

    MergeTreeIndexFormat MergeTreeIndexNaive::getDeserializedFormat(const DiskPtr disk, const std::string & relative_path_prefix) const {
        if (disk->exists(relative_path_prefix + ".idx2"))
            return {2, ".idx2"};
        else if (disk->exists(relative_path_prefix + ".idx"))
            return {1, ".idx"};
        return {0 /* unknown */, ""};
    }

    MergeTreeIndexPtr naiveIndexCreator(
        const IndexDescription & index) {
        return std::make_shared<MergeTreeIndexNaive>(index);
    }

    void naiveIndexValidator(const IndexDescription & /* index */, bool /* attach */) {
    }

    

}

