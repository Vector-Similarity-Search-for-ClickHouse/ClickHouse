#pragma once

#include <Storages/MergeTree/MergeTreeIndices.h>
#include "Core/Types.h"
#include "Interpreters/Context_fwd.h"
#include "base/types.h"

// #include <scann/dataset.hpp>
// #include <scann/scann_searcher.hpp>

#include <Interpreters/scann.h>

#include <vector>

namespace DB
{


struct MergeTreeIndexGranuleScaNN final : public IMergeTreeIndexGranule
{
    MergeTreeIndexGranuleScaNN(const String & index_name_, const Block & index_sample_block_, ContextPtr context);
    MergeTreeIndexGranuleScaNN(
        const String & index_name_, const Block & index_sample_block_, ContextPtr context, std::vector<Float32> && data);

    ~MergeTreeIndexGranuleScaNN() override = default;

    void serializeBinary(WriteBuffer & ostr) const override;
    void deserializeBinary(ReadBuffer & istr, MergeTreeIndexVersion version) override;

    bool empty() const override;

    String index_name;
    Block index_sample_block;

    std::vector<Float32> data;
    std::optional<scann::ScannSearcher> searcher;
};

struct MergeTreeIndexAggregatorScaNN final : IMergeTreeIndexAggregator
{
    MergeTreeIndexAggregatorScaNN(const String & index_name_, const Block & index_sample_block_, ContextPtr context);
    ~MergeTreeIndexAggregatorScaNN() override = default;

    bool empty() const override;
    MergeTreeIndexGranulePtr getGranuleAndReset() override;
    void update(const Block & block, size_t * pos, size_t limit) override;

    String index_name;
    Block index_sample_block;

    // row_major_array
    std::vector<Float32> data;

    ContextPtr context {nullptr};
};

class MergeTreeIndexConditionScaNN final : public IMergeTreeIndexCondition
{
public:
    MergeTreeIndexConditionScaNN(const IndexDescription & index, const SelectQueryInfo & query, ContextPtr context);

    bool alwaysUnknownOrTrue() const override;

    bool mayBeTrueOnGranule(MergeTreeIndexGranulePtr idx_granule) const override;

    ~MergeTreeIndexConditionScaNN() override = default;

private:
    DataTypes index_data_types;
};

class MergeTreeIndexScaNN : public IMergeTreeIndex
{
public:
    MergeTreeIndexScaNN(const IndexDescription & index_) : IMergeTreeIndex(index_) { }

    ~MergeTreeIndexScaNN() override = default;

    MergeTreeIndexGranulePtr createIndexGranule() const override;
    MergeTreeIndexAggregatorPtr createIndexAggregator() const override;

    MergeTreeIndexConditionPtr createIndexCondition(const SelectQueryInfo & query, ContextPtr context) const override;

    bool mayBenefitFromIndexForIn(const ASTPtr & node) const override;

    const char * getSerializedFileExtension() const override { return ".idx2"; }
    MergeTreeIndexFormat getDeserializedFormat(const DiskPtr disk, const std::string & path_prefix) const override;
private:
    mutable ContextPtr context;
};


}
