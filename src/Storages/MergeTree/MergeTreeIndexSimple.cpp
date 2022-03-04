#include <Storages/MergeTree/MergeTreeIndexSimple.h>

namespace DB
{

MergeTreeIndexGranulePtr MergeTreeIndexAggregatorSimple::getGranuleAndReset()
{
    return std::make_shared<MergeTreeIndexGranuleSimple>();
}

bool MergeTreeIndexConditionSimple::mayBeTrueOnGranule(MergeTreeIndexGranulePtr /*idx_granule*/) const
{
    std::random_device device;
    std::mt19937 generator(device());
    std::uniform_int_distribution<> distribution;

    return static_cast<bool>(distribution(generator));
}

MergeTreeIndexGranulePtr MergeTreeIndexSimple::createIndexGranule() const
{
    return std::make_shared<MergeTreeIndexGranuleSimple>();
}


MergeTreeIndexAggregatorPtr MergeTreeIndexSimple::createIndexAggregator() const
{
    return std::make_shared<MergeTreeIndexAggregatorSimple>();
}

MergeTreeIndexConditionPtr MergeTreeIndexSimple::createIndexCondition(
    const SelectQueryInfo & /*query*/, ContextPtr /*context*/) const
{
    return std::make_shared<MergeTreeIndexConditionSimple>();
}

MergeTreeIndexFormat MergeTreeIndexSimple::getDeserializedFormat(const DiskPtr disk, const std::string & relative_path_prefix) const
{
    if (disk->exists(relative_path_prefix + ".idx2"))
        return {2, ".idx2"};
    else if (disk->exists(relative_path_prefix + ".idx"))
        return {1, ".idx"};
    return {0 /* unknown */, ""};
}

MergeTreeIndexPtr simpleIndexCreator(const IndexDescription & index) {
    return std::make_shared<MergeTreeIndexSimple>(index);
}

void simpleIndexValidator(const IndexDescription & /* index */, bool /* attach */)
{
}

}
