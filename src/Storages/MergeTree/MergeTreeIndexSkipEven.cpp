#include <Storages/MergeTree/MergeTreeIndexSkipEven.h>

#include <IO/ReadBuffer.h>
#include <IO/WriteBuffer.h>

#include <Parsers/ASTFunction.h>

#include "MergeTreeIndices.h"

namespace DB
{

MergeTreeIndexGranuleSkipEven::MergeTreeIndexGranuleSkipEven(const String & index_name_, const Block & index_sample_block_)
    : index_name(index_name_)
    , index_sample_block(index_sample_block_)
{}

void MergeTreeIndexGranuleSkipEven::serializeBinary(WriteBuffer & /*ostr*/) const
{
//    ostr.write("kek", 3);
//    ostr.finalize();
}

void MergeTreeIndexGranuleSkipEven::deserializeBinary(ReadBuffer & /*istr*/, MergeTreeIndexVersion /*version*/)
{
//    char kek[3];
//    istr.read(kek, 3);
}

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
    return false;
}

MergeTreeIndexGranulePtr MergeTreeIndexAggregatorSkipEven::getGranuleAndReset()
{
    return std::make_shared<MergeTreeIndexGranuleSkipEven>(index_name, index_sample_block);
}

void MergeTreeIndexAggregatorSkipEven::update(const Block & /*block*/, size_t * /*pos*/, size_t /*limit*/)
{
    // pass
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
    int num = rand() % 2;
    return num == 1;
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

bool MergeTreeIndexSkipEven::mayBenefitFromIndexForIn(const ASTPtr & node) const
{
    const String column_name = node->getColumnName();

    for (const auto & cname : index.column_names)
        if (column_name == cname)
            return true;

    if (const auto * func = typeid_cast<const ASTFunction *>(node.get()))
        if (func->arguments->children.size() == 1)
            return mayBenefitFromIndexForIn(func->arguments->children.front());

    return false;
}

MergeTreeIndexFormat MergeTreeIndexSkipEven::getDeserializedFormat(const DiskPtr /*disk*/, const std::string & /*path_prefix*/) const
{
    return {1, ".idx"};
}

MergeTreeIndexPtr skipEvenIndexCreator(
    const IndexDescription & index)
{
    return std::make_shared<MergeTreeIndexSkipEven>(index);
}

void skipEvenIndexValidator(const IndexDescription & /* index */, bool /* attach */)
{
}

}
