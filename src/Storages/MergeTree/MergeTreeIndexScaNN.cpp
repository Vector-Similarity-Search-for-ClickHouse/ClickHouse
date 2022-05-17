#include <cmath>
#include <memory>
#include <vector>
#include <libunwind.h>

#include <Core/Field.h>
#include <Storages/MergeTree/MergeTreeIndexScaNN.h>

// #include <scann/dataset.hpp>
// #include <scann/io.hpp>
// #include <scann/scann_builder.hpp>
#include "IO/ReadBuffer.h"
#include "IO/WriteBuffer.h"
#include "Interpreters/Context.h"
#include "Interpreters/Context_fwd.h"
#include "base/logger_useful.h"

#include <Interpreters/scann_builder.hpp>

namespace DB
{

MergeTreeIndexGranuleScaNN::MergeTreeIndexGranuleScaNN(const String & index_name_, const Block & index_sample_block_, ContextPtr context)
    : MergeTreeIndexGranuleScaNN(index_name_, index_sample_block_, context, {})
{
}

MergeTreeIndexGranuleScaNN::MergeTreeIndexGranuleScaNN(
    const String & index_name_, const Block & index_sample_block_, ContextPtr  context, std::vector<Float32> && data_)
    : index_name(index_name_), index_sample_block(index_sample_block_), data(data_)
{
    const int dimension = 3;
    const int sample_size = data.size() / dimension;

    scann::ConstDataSetWrapper<Float32, 2> data_set(data, {sample_size, dimension});

    const size_t num_neighbors = 10;
    const std::string distance_measure = "dot_product";

    const size_t num_leaves = 2000;
    const size_t num_leaves_to_search = 100;
    const size_t training_sample_size = sample_size;

    const size_t dimension_per_block = 2;
    const float anisotropic_quantization_threshold = 0.2;

    const size_t reordering_num_neighbors = 100;

    // searcher = 
    std::move(scann::ScannBuilder(data_set, num_neighbors, distance_measure)
                   .Tree(num_leaves, num_leaves_to_search, training_sample_size)
                   .ScoreAh(dimension_per_block, anisotropic_quantization_threshold)
                   .Reorder(reordering_num_neighbors)
                   .Build(context));

    // searcher = ;
}

// class Writer final : public scann::IWriter
// {
// public:
//     explicit Writer(WriteBuffer & ostr_) : ostr(ostr_) { }

//     void write(const char * from, size_t n) final { ostr.write(from, n); }
//     ~Writer() override = default;

// private:
//     WriteBuffer & ostr;
// };

void MergeTreeIndexGranuleScaNN::serializeBinary(WriteBuffer & /*ostr*/) const
{
    // Writer writer(ostr);
    // searcher.Serialize(writer);
}

// class Reader : public scann::IReader
// {
// public:
//     explicit Reader(ReadBuffer & istr_) : istr(istr_) { }

//     void read(char * to, size_t n) final { istr.read(to, n); }
//     ~Reader() override = default;

// private:
//     ReadBuffer & istr;
// };

void MergeTreeIndexGranuleScaNN::deserializeBinary(ReadBuffer & /*istr*/, MergeTreeIndexVersion /*version*/)
{
    // Reader reader(istr);
    // searcher.Deserialize(reader);
}

bool MergeTreeIndexGranuleScaNN::empty() const
{
    // return !searcher.IsInitialized();
    // TODO
    return true;
}


MergeTreeIndexAggregatorScaNN::MergeTreeIndexAggregatorScaNN(const String & index_name_, const Block & index_sample_block_, ContextPtr context_)
    : index_name(index_name_), index_sample_block(index_sample_block_), context(context_)
{
    // TODO: Check number of column
    // TODO: extract from index_sample_block_ vector dimension
}

MergeTreeIndexGranulePtr MergeTreeIndexAggregatorScaNN::getGranuleAndReset()
{
    return std::make_shared<MergeTreeIndexGranuleScaNN>(index_name, index_sample_block, context, std::move(data));
}

bool MergeTreeIndexAggregatorScaNN::empty() const
{
    return data.empty();
}

void MergeTreeIndexAggregatorScaNN::update(const Block & block, size_t * pos, size_t limit)
{
    if (*pos >= block.rows())
        throw Exception(
            "The provided position is not less than the number of block rows. Position: " + toString(*pos)
                + ", Block rows: " + toString(block.rows()) + ".",
            ErrorCodes::LOGICAL_ERROR);

    size_t rows_read = std::min(limit, block.rows() - *pos);

    auto index_column_name = index_sample_block.getByPosition(0).name;
    const auto & column = block.getByName(index_column_name).column->cut(*pos, rows_read);

    // TODO: Fix dummy way of the access to the Field
    for (size_t i = 0; i < rows_read; ++i)
    {
        Field field;
        column->get(i, field);

        auto field_array = field.safeGet<Tuple>();

        // Store vectors in the flatten arrays
        for (const auto & value : field_array)
        {
            auto num = value.safeGet<Float32>();
            data.push_back(num);
        }
    }

    *pos += rows_read;
}

MergeTreeIndexConditionScaNN::MergeTreeIndexConditionScaNN(
    const IndexDescription & index, const SelectQueryInfo & /*query*/, ContextPtr /*context*/)
    : index_data_types(index.data_types)
{
}

bool MergeTreeIndexConditionScaNN::alwaysUnknownOrTrue() const
{
    // TODO
    return false;
}

bool MergeTreeIndexConditionScaNN::mayBeTrueOnGranule(MergeTreeIndexGranulePtr /*idx_granule*/) const
{
    // TODO
    return true;
}


MergeTreeIndexGranulePtr MergeTreeIndexScaNN::createIndexGranule() const
{
    return std::make_shared<MergeTreeIndexGranuleScaNN>(index.name, index.sample_block, context);
}
MergeTreeIndexAggregatorPtr MergeTreeIndexScaNN::createIndexAggregator() const
{
    return std::make_shared<MergeTreeIndexAggregatorScaNN>(index.name, index.sample_block, context);
}

MergeTreeIndexConditionPtr MergeTreeIndexScaNN::createIndexCondition(const SelectQueryInfo & query, ContextPtr context_) const
{
    context = context_;
    return std::make_shared<MergeTreeIndexConditionScaNN>(index, query, context);
}

bool MergeTreeIndexScaNN::mayBenefitFromIndexForIn(const ASTPtr & /*node*/) const
{
    // TODO
    return true;
}

MergeTreeIndexFormat MergeTreeIndexScaNN::getDeserializedFormat(const DiskPtr disk, const std::string & relative_path_prefix) const
{
    if (disk->exists(relative_path_prefix + ".idx2"))
        return {2, ".idx2"};
    else if (disk->exists(relative_path_prefix + ".idx"))
        return {1, ".idx"};
    return {0 /* unknown */, ""};
}

MergeTreeIndexPtr scannIndexCreator(const IndexDescription & index)
{
    return std::make_shared<MergeTreeIndexScaNN>(index);
}

void scannIndexValidator(const IndexDescription & /* index */, bool /* attach */)
{
}

}
