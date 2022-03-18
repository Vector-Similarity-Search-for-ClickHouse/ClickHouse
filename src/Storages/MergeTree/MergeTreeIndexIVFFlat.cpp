#include <Storages/MergeTree/MergeTreeIndexIVFFlat.h>

#include <IO/ReadBuffer.h>
#include <IO/WriteBuffer.h>

#include <Parsers/ASTFunction.h>

#include "MergeTreeIndices.h"
#include <Common/FieldVisitorsAccurateComparison.h>

#include <faiss/IndexFlat.h>

namespace DB
{

MergeTreeIndexGranuleIVFFlat::MergeTreeIndexGranuleIVFFlat(const String & index_name_, const Block & index_sample_block_)
    : index_name(index_name_)
    , index_sample_block(index_sample_block_)
{}

void MergeTreeIndexGranuleIVFFlat::serializeBinary(WriteBuffer & /*ostr*/) const
{}

void MergeTreeIndexGranuleIVFFlat::deserializeBinary(ReadBuffer & /*istr*/, MergeTreeIndexVersion /*version*/)
{}

bool MergeTreeIndexGranuleIVFFlat::empty() const
{
    return false;
}


MergeTreeIndexAggregatorIVFFlat::MergeTreeIndexAggregatorIVFFlat(const String & index_name_,
                                                                const Block & index_sample_block_)
    : index_name(index_name_)
    , index_sample_block(index_sample_block_)
{}

bool MergeTreeIndexAggregatorIVFFlat::empty() const
{
    return true;
}

MergeTreeIndexGranulePtr MergeTreeIndexAggregatorIVFFlat::getGranuleAndReset()
{
    return std::make_shared<MergeTreeIndexGranuleIVFFlat>(index_name, index_sample_block);
}

void MergeTreeIndexAggregatorIVFFlat::update(const Block & block, size_t * pos, size_t limit)
{
    if (*pos >= block.rows())
        throw Exception(
                "The provided position is not less than the number of block rows. Position: "
                + toString(*pos) + ", Block rows: " + toString(block.rows()) + ".", ErrorCodes::LOGICAL_ERROR);

    size_t rows_read = std::min(limit, block.rows() - *pos);

    *pos += rows_read;
}


MergeTreeIndexConditionIVFFlat::MergeTreeIndexConditionIVFFlat(
    const IndexDescription & index,
    const SelectQueryInfo & /*query*/,
    ContextPtr /*context*/)
    : index_data_types(index.data_types)
{}

bool MergeTreeIndexConditionIVFFlat::alwaysUnknownOrTrue() const
{
    return false;
}

bool MergeTreeIndexConditionIVFFlat::mayBeTrueOnGranule(MergeTreeIndexGranulePtr /*idx_granule*/) const
{
    faiss::IndexFlatL2 index(64);           // call constructor
    static std::size_t num = 1;
    return num++ % 2 == 0;
}


MergeTreeIndexIVFFlat::MergeTreeIndexIVFFlat(const IndexDescription & index_)
    : IMergeTreeIndex(index_)
{}


MergeTreeIndexGranulePtr MergeTreeIndexIVFFlat::createIndexGranule() const
{
    return std::make_shared<MergeTreeIndexGranuleIVFFlat>(index.name, index.sample_block);
}

MergeTreeIndexAggregatorPtr MergeTreeIndexIVFFlat::createIndexAggregator() const
{
    return std::make_shared<MergeTreeIndexAggregatorIVFFlat>(index.name, index.sample_block);
}

MergeTreeIndexConditionPtr MergeTreeIndexIVFFlat::createIndexCondition(
    const SelectQueryInfo & query, ContextPtr context) const
{
    return std::make_shared<MergeTreeIndexConditionIVFFlat>(index, query, context);
}

bool MergeTreeIndexIVFFlat::mayBenefitFromIndexForIn(const ASTPtr & /*node*/) const
{
    return true;
}

MergeTreeIndexFormat MergeTreeIndexIVFFlat::getDeserializedFormat(const DiskPtr disk, const std::string & relative_path_prefix) const
{
    if (disk->exists(relative_path_prefix + ".idx2"))
        return {2, ".idx2"};
    else if (disk->exists(relative_path_prefix + ".idx"))
        return {1, ".idx"};
    return {0 /* unknown */, ""};
}

MergeTreeIndexPtr skipEvenIndexCreator(
    const IndexDescription & index)
{
    return std::make_shared<MergeTreeIndexIVFFlat>(index);
}

void skipEvenIndexValidator(const IndexDescription & /* index */, bool /* attach */)
{}

}
