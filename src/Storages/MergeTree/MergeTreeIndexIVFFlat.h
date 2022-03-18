#pragma once

#include <Storages/MergeTree/MergeTreeIndices.h>

namespace DB
{

struct MergeTreeIndexGranuleIVFFlat final : public IMergeTreeIndexGranule
{
    MergeTreeIndexGranuleIVFFlat(const String & index_name_, const Block & index_sample_block_);

    ~MergeTreeIndexGranuleIVFFlat() override = default;

    void serializeBinary(WriteBuffer & ostr) const override;

    void deserializeBinary(ReadBuffer & istr, MergeTreeIndexVersion version) override;

    bool empty() const override;

    String index_name;
    Block index_sample_block;
};


struct MergeTreeIndexAggregatorIVFFlat final : IMergeTreeIndexAggregator
{
    MergeTreeIndexAggregatorIVFFlat(const String & index_name_, const Block & index_sample_block_);
    ~MergeTreeIndexAggregatorIVFFlat() override = default;

    bool empty() const override;
    MergeTreeIndexGranulePtr getGranuleAndReset() override;
    void update(const Block & block, size_t * pos, size_t limit) override;

    String index_name;
    Block index_sample_block;
};


class MergeTreeIndexConditionIVFFlat final : public IMergeTreeIndexCondition
{
public:
    MergeTreeIndexConditionIVFFlat(
        const IndexDescription & index,
        const SelectQueryInfo & query,
        ContextPtr context);
    ~MergeTreeIndexConditionIVFFlat() override = default;

    bool alwaysUnknownOrTrue() const override;

    bool mayBeTrueOnGranule(MergeTreeIndexGranulePtr idx_granule) const override;

private:
    DataTypes index_data_types;
};


class MergeTreeIndexIVFFlat : public IMergeTreeIndex
{
public:
    explicit MergeTreeIndexIVFFlat(const IndexDescription & index_);

    ~MergeTreeIndexIVFFlat() override = default;

    MergeTreeIndexGranulePtr createIndexGranule() const override;
    MergeTreeIndexAggregatorPtr createIndexAggregator() const override;

    MergeTreeIndexConditionPtr createIndexCondition(
        const SelectQueryInfo & query, ContextPtr context) const override;

    bool mayBenefitFromIndexForIn(const ASTPtr & node) const override;

    const char* getSerializedFileExtension() const override { return ".idx2"; }
    MergeTreeIndexFormat getDeserializedFormat(const DiskPtr disk, const std::string & path_prefix) const override;
};

}
