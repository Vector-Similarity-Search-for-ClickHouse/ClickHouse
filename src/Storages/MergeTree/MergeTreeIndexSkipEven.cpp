#include <Storages/MergeTree/MergeTreeIndexSkipEven.h>

#include <IO/ReadBuffer.h>
#include <IO/WriteBuffer.h>

#include <Parsers/ASTFunction.h>

#include "MergeTreeIndices.h"
#include <Common/FieldVisitorsAccurateComparison.h>

namespace DB
{

MergeTreeIndexGranuleSkipEven::MergeTreeIndexGranuleSkipEven(const String & index_name_, const Block & index_sample_block_)
    : index_name(index_name_)
    , index_sample_block(index_sample_block_)
{}

void MergeTreeIndexGranuleSkipEven::serializeBinary(WriteBuffer & /*ostr*/) const
{}

void MergeTreeIndexGranuleSkipEven::deserializeBinary(ReadBuffer & /*istr*/, MergeTreeIndexVersion /*version*/)
{}

bool MergeTreeIndexGranuleSkipEven::empty() const
{
    return false;
}


MergeTreeIndexAggregatorSkipEven::MergeTreeIndexAggregatorSkipEven(const String & index_name_,
                                                                   const Block & index_sample_block_)
    : index_name(index_name_)
    , index_sample_block(index_sample_block_)
{}

bool MergeTreeIndexAggregatorSkipEven::empty() const
{
    return true;
}

MergeTreeIndexGranulePtr MergeTreeIndexAggregatorSkipEven::getGranuleAndReset()
{
    return std::make_shared<MergeTreeIndexGranuleSkipEven>(index_name, index_sample_block);
}

void MergeTreeIndexAggregatorSkipEven::update(const Block & block, size_t * pos, size_t limit)
{
    if (*pos >= block.rows())
        throw Exception(
                "The provided position is not less than the number of block rows. Position: "
                + toString(*pos) + ", Block rows: " + toString(block.rows()) + ".", ErrorCodes::LOGICAL_ERROR);

    size_t rows_read = std::min(limit, block.rows() - *pos);

    *pos += rows_read;
}


MergeTreeIndexConditionSkipEven::MergeTreeIndexConditionSkipEven(
    const IndexDescription & index,
    const SelectQueryInfo & /*query*/,
    ContextPtr /*context*/)
    : index_data_types(index.data_types)
{}

bool MergeTreeIndexConditionSkipEven::alwaysUnknownOrTrue() const
{
    return false;
}

bool MergeTreeIndexConditionSkipEven::mayBeTrueOnGranule(MergeTreeIndexGranulePtr /*idx_granule*/) const
{
    static std::size_t num = 1;
    return num++ % 2 == 0;
}


MergeTreeIndexSkipEven::MergeTreeIndexSkipEven(const IndexDescription & index_)
    : IMergeTreeIndex(index_)
{}


MergeTreeIndexGranulePtr MergeTreeIndexSkipEven::createIndexGranule() const
{
    return std::make_shared<MergeTreeIndexGranuleSkipEven>(index.name, index.sample_block);
}

MergeTreeIndexAggregatorPtr MergeTreeIndexSkipEven::createIndexAggregator() const
{
    return std::make_shared<MergeTreeIndexAggregatorSkipEven>(index.name, index.sample_block);
}

MergeTreeIndexConditionPtr MergeTreeIndexSkipEven::createIndexCondition(
    const SelectQueryInfo & query, ContextPtr context) const
{
    return std::make_shared<MergeTreeIndexConditionSkipEven>(index, query, context);
}

bool MergeTreeIndexSkipEven::mayBenefitFromIndexForIn(const ASTPtr & /*node*/) const
{
    return true;
}

MergeTreeIndexFormat MergeTreeIndexSkipEven::getDeserializedFormat(const DiskPtr disk, const std::string & relative_path_prefix) const
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
    return std::make_shared<MergeTreeIndexSkipEven>(index);
}

void skipEvenIndexValidator(const IndexDescription & /* index */, bool /* attach */)
{}

}
