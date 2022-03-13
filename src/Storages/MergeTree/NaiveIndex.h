#pragma once

#include <Storages/MergeTree/MergeTreeIndices.h>
#include <Storages/MergeTree/MergeTreeData.h>
#include <Storages/MergeTree/KeyCondition.h>
#include "Storages/IndicesDescription.h"
#include "Storages/SelectQueryInfo.h"

#include <cstddef>
#include <memory>

namespace DB {

struct MergeTreeIndexGranuleNaive final : public IMergeTreeIndexGranule {
  public:
     explicit MergeTreeIndexGranuleNaive(const String& index_name_, int64_t num_ = 0);
    ~MergeTreeIndexGranuleNaive() override = default;

    void serializeBinary(WriteBuffer& /*ostr*/) const override {}
    void deserializeBinary(ReadBuffer& /*istr*/, MergeTreeIndexVersion /*version*/) override {}
    bool empty() const override { return false; }

    String index_name;
    int64_t random_number;
};

struct MergeTreeIndexAggregatorNaive final : IMergeTreeIndexAggregator {
  public:
    explicit MergeTreeIndexAggregatorNaive(const String& index_name_, int64_t num_ = 0);
    ~MergeTreeIndexAggregatorNaive() override = default;

    bool empty() const override { return false; } 
    MergeTreeIndexGranulePtr getGranuleAndReset() override;
    void update(const Block& block, size_t* pos, size_t limit) override;

    String index_name;
    int64_t random_number;
};

class MergeTreeIndexConditionNaive final : public IMergeTreeIndexCondition     {
  public:
    MergeTreeIndexConditionNaive(const IndexDescription& index,
                                   const SelectQueryInfo& query,
                                   ContextPtr context);
    
    ~MergeTreeIndexConditionNaive() override = default;

    bool alwaysUnknownOrTrue() const override;
    bool mayBeTrueOnGranule(MergeTreeIndexGranulePtr idx_granule) const override;
  
  private:
    DataTypes index_data_types;
    KeyCondition condition;

};

class MergeTreeIndexNaive : public IMergeTreeIndex
{
  public:
    explicit MergeTreeIndexNaive(const IndexDescription & index_)
        : IMergeTreeIndex(index_)
    {}

    ~MergeTreeIndexNaive() override = default;

    MergeTreeIndexGranulePtr createIndexGranule() const override;
    MergeTreeIndexAggregatorPtr createIndexAggregator() const override;

    MergeTreeIndexConditionPtr createIndexCondition(
        const SelectQueryInfo & query, ContextPtr context) const override;

    bool mayBenefitFromIndexForIn(const ASTPtr & node) const override;

    const char* getSerializedFileExtension() const override { return ".idx2"; }
    MergeTreeIndexFormat getDeserializedFormat(const DiskPtr disk, const std::string & path_prefix) const override;
};

}
