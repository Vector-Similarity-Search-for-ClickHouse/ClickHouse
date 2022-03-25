#pragma once

#include <Storages/MergeTree/MergeTreeIndices.h>
#include <Storages/MergeTree/MergeTreeData.h>
#include <Storages/MergeTree/KeyCondition.h>

#include <faiss/IndexLSH.h>

namespace FaissLSH::Detail {
 void deserializeIntoIndex(faiss::lsh::data::IndexDataSetter& data_setter, DB::ReadBuffer &istr);  
}

namespace DB {

struct MergeTreeIndexGranuleFaissLSH final : public IMergeTreeIndexGranule
{
    /// Should think about arguments
    explicit MergeTreeIndexGranuleFaissLSH(const String & index_name_, int64_t d);
    MergeTreeIndexGranuleFaissLSH(
        const String & index_name_,
        const Block & index_sample_block_,
        const faiss::IndexLSH & index);

    ~MergeTreeIndexGranuleFaissLSH() override = default;

    void serializeBinary(WriteBuffer & ostr) const override;
    void deserializeBinary(ReadBuffer & istr, MergeTreeIndexVersion version) override;

    bool empty() const override { return false; }

    String index_name;
    Block index_sample_block;
    faiss::IndexLSH index_impl;
};

struct MergeTreeIndexAggregatorFaissLSH final : IMergeTreeIndexAggregator
{
    MergeTreeIndexAggregatorFaissLSH(
        const String & index_name_, 
        const Block & index_sample_block,
        const faiss::IndexLSH & index );
    ~MergeTreeIndexAggregatorFaissLSH() override = default;

    bool empty() const override { return false; }
    MergeTreeIndexGranulePtr getGranuleAndReset() override;
    void update(const Block & block, size_t * pos, size_t limit) override;

    String index_name;
    Block index_sample_block;
    faiss::IndexLSH index_impl;
};

class MergeTreeIndexConditionFaissLSH final : public IMergeTreeIndexCondition
{
public:
    explicit MergeTreeIndexConditionFaissLSH(
        const String & index_name_);

    bool alwaysUnknownOrTrue() const override;

    bool mayBeTrueOnGranule(MergeTreeIndexGranulePtr idx_granule) const override;

    ~MergeTreeIndexConditionFaissLSH() override = default;
private:
    void traverseAST(ASTPtr & node) const;
    bool atomFromAST(ASTPtr & node) const;
    static bool operatorFromAST(ASTPtr & node);

    bool checkASTUseless(const ASTPtr & node, bool atomic = false) const;


    String index_name;
    /// ??? mystery
};

class MergeTreeIndexFaissLSH : public IMergeTreeIndex
{
public:
    explicit MergeTreeIndexFaissLSH(const IndexDescription & index_)
        : IMergeTreeIndex(index_)
    {}

    ~MergeTreeIndexFaissLSH() override = default;

    MergeTreeIndexGranulePtr createIndexGranule() const override;
    MergeTreeIndexAggregatorPtr createIndexAggregator() const override;

    MergeTreeIndexConditionPtr createIndexCondition(
        const SelectQueryInfo & query, ContextPtr context) const override;

    bool mayBenefitFromIndexForIn(const ASTPtr & node) const override;

    /// Questions, questions
    const char* getSerializedFileExtension() const override { return ".idxFaissLSH"; }
    MergeTreeIndexFormat getDeserializedFormat(const DiskPtr disk, const std::string & path_prefix) const override;
};

}
