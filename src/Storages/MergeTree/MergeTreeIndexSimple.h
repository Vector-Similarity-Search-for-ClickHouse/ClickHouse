#pragma once

#include <memory>
#include <random>

#include <Storages/MergeTree/MergeTreeIndices.h>
#include <Storages/MergeTree/MergeTreeData.h>
#include <Storages/IndicesDescription.h>

namespace DB
{

struct MergeTreeIndexGranuleSimple final : public IMergeTreeIndexGranule
{
    MergeTreeIndexGranuleSimple() = default;
    ~MergeTreeIndexGranuleSimple() override = default;

    void serializeBinary(WriteBuffer & /*ostr*/) const override {}
    void deserializeBinary(ReadBuffer & /*istr*/, MergeTreeIndexVersion /*version*/) override {}

    bool empty() const override { return true; }
};


struct MergeTreeIndexAggregatorSimple final : public IMergeTreeIndexAggregator
{
    MergeTreeIndexAggregatorSimple() = default;
    ~MergeTreeIndexAggregatorSimple() override = default;

    bool empty() const override { return false; }
    MergeTreeIndexGranulePtr getGranuleAndReset() override;
    void update(const Block & /*block*/, size_t * /*pos*/, size_t /*limit*/) override {}
};


struct MergeTreeIndexConditionSimple final : public IMergeTreeIndexCondition {
    MergeTreeIndexConditionSimple() = default;

    ~MergeTreeIndexConditionSimple() override = default;

    bool alwaysUnknownOrTrue() const override { return false; }
    bool mayBeTrueOnGranule(MergeTreeIndexGranulePtr idx_granule) const override;
    
};


struct MergeTreeIndexSimple : public IMergeTreeIndex
{
    explicit MergeTreeIndexSimple(const IndexDescription & index_)
        : IMergeTreeIndex(index_)
    {}

    ~MergeTreeIndexSimple() override = default;

    MergeTreeIndexGranulePtr createIndexGranule() const override;
    MergeTreeIndexAggregatorPtr createIndexAggregator() const override;

    MergeTreeIndexConditionPtr createIndexCondition(
        const SelectQueryInfo & query, ContextPtr context) const override;

    bool mayBenefitFromIndexForIn(const ASTPtr & /*node*/) const override { return true; }

    const char* getSerializedFileExtension() const override { return ".idx2"; }
    MergeTreeIndexFormat getDeserializedFormat(DiskPtr disk, const std::string & path_prefix) const override;
};


}
