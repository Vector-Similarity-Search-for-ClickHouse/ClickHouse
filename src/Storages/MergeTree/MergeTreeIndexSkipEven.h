#pragma once

#include <Storages/MergeTree/MergeTreeIndices.h>
#include <Storages/MergeTree/KeyCondition.h>

namespace DB
{

struct MergeTreeIndexGranuleSkipEven final : public IMergeTreeIndexGranule
{
    MergeTreeIndexGranuleSkipEven(const String & index_name_, const Block & index_sample_block_);

    ~MergeTreeIndexGranuleSkipEven() override = default;

    void serializeBinary(WriteBuffer & ostr) const override;

    void deserializeBinary(ReadBuffer & istr, MergeTreeIndexVersion version) override;

    bool empty() const override;

    String index_name;
    Block index_sample_block;
};


struct MergeTreeIndexAggregatorSkipEven final : IMergeTreeIndexAggregator
{
    MergeTreeIndexAggregatorSkipEven(const String & index_name_, const Block & index_sample_block_);
    ~MergeTreeIndexAggregatorSkipEven() override = default;

    bool empty() const override;
    MergeTreeIndexGranulePtr getGranuleAndReset() override;
    void update(const Block & block, size_t * pos, size_t limit) override;

    String index_name;
    Block index_sample_block;
};


class MergeTreeIndexConditionSkipEven final : public IMergeTreeIndexCondition
{
public:
    MergeTreeIndexConditionSkipEven(
        const IndexDescription & index,
        const SelectQueryInfo & query,
        ContextPtr context);
    ~MergeTreeIndexConditionSkipEven() override = default;

    bool alwaysUnknownOrTrue() const override;

    bool mayBeTrueOnGranule(MergeTreeIndexGranulePtr idx_granule) const override;

private:
    DataTypes index_data_types;
};


class MergeTreeIndexSkipEven : public IMergeTreeIndex
{
public:
    explicit MergeTreeIndexSkipEven(const IndexDescription & index_);

    ~MergeTreeIndexSkipEven() override = default;

    MergeTreeIndexGranulePtr createIndexGranule() const override;
    MergeTreeIndexAggregatorPtr createIndexAggregator() const override;

    MergeTreeIndexConditionPtr createIndexCondition(
        const SelectQueryInfo & query, ContextPtr context) const override;

    bool mayBenefitFromIndexForIn(const ASTPtr & node) const override;

    const char* getSerializedFileExtension() const override { return ".idx2"; }
    MergeTreeIndexFormat getDeserializedFormat(const DiskPtr disk, const std::string & path_prefix) const override;
};


}
