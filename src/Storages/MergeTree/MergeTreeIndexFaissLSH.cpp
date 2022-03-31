#include <cstddef>
#include <exception>
#include <memory>
#include <Storages/MergeTree/MergeTreeIndexFaissLSH.h>

#include <faiss/IndexFlat.h>
#include <faiss/IndexIVF.h>
#include <faiss/IndexIVFFlat.h>
#include <faiss/IndexLSH.h>
#include <faiss/MetricType.h>
#include <faiss/impl/io.h>
#include <faiss/index_io.h>
#include "Common/typeid_cast.h"
#include "Storages/MergeTree/MergeTreeIndexGranuleBloomFilter.h"

namespace DB {

/// Serialization helpers //////////////////////////////////////////////////////////////

namespace Detail {
  class WriteBufferFaissWrapper : public faiss::IOWriter {
    public:
        explicit WriteBufferFaissWrapper(WriteBuffer & ostr_)
        : ostr(ostr_) {}

        size_t operator()(const void* ptr, size_t size, size_t nitems) override 
        {
            ostr.write(reinterpret_cast<const char*>(ptr), size * nitems);

            // WriteBuffer guarantees to write all items
            return nitems;
        }

    private:
        WriteBuffer & ostr;
    };

  class ReadBufferFaissWrapper : public faiss::IOReader {
    public:        
        explicit ReadBufferFaissWrapper(ReadBuffer & istr_)
        : istr(istr_) {}

        size_t operator()(void* ptr, size_t size, size_t nitems) override 
        {
            return istr.read(reinterpret_cast<char*>(ptr), size * nitems) / size;
        }

    private:
        ReadBuffer & istr;
    };
}

/// Granule ////////////////////////////////////////////////////////////////////////////

    MergeTreeIndexGranuleFaissLSH::MergeTreeIndexGranuleFaissLSH(
        const String & index_name_, 
        const Block & sample_block)
        : index_name(index_name_), 
          index_sample_block(sample_block), 
          index_impl(nullptr) {}

    MergeTreeIndexGranuleFaissLSH::MergeTreeIndexGranuleFaissLSH(
        const String & index_name_,
        const Block & sample_block,
        std::shared_ptr<faiss::IndexLSH> index) 
        : index_name(index_name_), 
          index_sample_block(sample_block), 
          index_impl(std::move(index)) {}

    void MergeTreeIndexGranuleFaissLSH::serializeBinary(WriteBuffer &ostr) const {
        Detail::WriteBufferFaissWrapper ostr_wrapped(ostr);
        faiss::write_index(index_impl.get(), &ostr_wrapped);
    }

    void MergeTreeIndexGranuleFaissLSH::deserializeBinary(ReadBuffer &istr, MergeTreeIndexVersion version) {

        if (version != 1)
            throw Exception(ErrorCodes::LOGICAL_ERROR, "Unknown index version {}.", version);

        Detail::ReadBufferFaissWrapper istr_wrapped(istr);
        index_impl = std::shared_ptr<faiss::IndexLSH>(dynamic_cast<faiss::IndexLSH*>(faiss::read_index(&istr_wrapped)));

    }

    bool MergeTreeIndexGranuleFaissLSH::empty() const {
        return index_impl == nullptr;
    }

/// Aggregator /////////////////////////////////////////////////////////////////////////

    MergeTreeIndexAggregatorFaissLSH::MergeTreeIndexAggregatorFaissLSH(
        const String & index_name_,
        const Block & sample_block)
        : index_name(index_name_),
          index_sample_block(sample_block),
          index_impl(nullptr) {}

    MergeTreeIndexAggregatorFaissLSH::MergeTreeIndexAggregatorFaissLSH(
        const String & index_name_, 
        const Block & sample_block,
        const std::shared_ptr<faiss::IndexLSH> & index)
        : index_name(index_name_),
          index_sample_block(sample_block),
          index_impl(index) {}    

    bool MergeTreeIndexAggregatorFaissLSH::empty() const {
        // should think about it
        return index_impl == nullptr;
    }

    MergeTreeIndexGranulePtr MergeTreeIndexAggregatorFaissLSH::getGranuleAndReset() {
        return std::make_shared<MergeTreeIndexGranuleFaissLSH>(index_name, index_sample_block, std::move(index_impl));
    }

    void MergeTreeIndexAggregatorFaissLSH::update(const Block &block, size_t *pos, size_t limit) {
        if (*pos >= block.rows())
        throw Exception(
                "The provided position is not less than the number of block rows. Position: "
                + toString(*pos) + ", Block rows: " + toString(block.rows()) + ".", ErrorCodes::LOGICAL_ERROR);

        size_t rows_read = std::min(limit, block.rows() - *pos);

        const Names index_columns = index_sample_block.getNames();
        for (const auto & column_name : index_columns) {
            const auto & column = block.getByName(column_name).column->cut(*pos, rows_read);
            std::vector<Float32> points;

            // Get data from column
            for (size_t i = 0; i < rows_read; ++i) {
                Field field;
                column->get(i, field);
                auto field_array = field.safeGet<Tuple>();

                for (const auto& value : field_array) {
                    points.push_back(value.safeGet<Float32>());
                }
            }

            // Update by training
            int64_t n = points.size() / index_impl->rrot.d_in;
            try {
                index_impl->train(n, points.data());
            } catch(...) {
                throw Exception(
                "Something went worng while training the index"
                , ErrorCodes::LOGICAL_ERROR);
            }
        }

        *pos += rows_read;
    }

/// Condition //////////////////////////////////////////////////////////////////////////


bool MergeTreeIndexConditionFaissLSH::alwaysUnknownOrTrue() const {
    /// ???
    return true;
}

bool MergeTreeIndexConditionFaissLSH::mayBeTrueOnGranule(MergeTreeIndexGranulePtr /*idx_granule*/) const {
    /// ???
    return true;
}


/// Index //////////////////////////////////////////////////////////////////////////////

MergeTreeIndexFaissLSH::MergeTreeIndexFaissLSH(const IndexDescription & index_)
        : IMergeTreeIndex(index_) {}

MergeTreeIndexGranulePtr MergeTreeIndexFaissLSH::createIndexGranule() const {
    /// need index_impl constructed here
    return std::make_shared<MergeTreeIndexGranuleFaissLSH>(index.name, index.sample_block);
}

MergeTreeIndexAggregatorPtr MergeTreeIndexFaissLSH::createIndexAggregator() const {
    /// need index_impl constructed here
    return std::make_shared<MergeTreeIndexAggregatorFaissLSH>(index.name, index.sample_block);
}

const char* MergeTreeIndexFaissLSH::getSerializedFileExtension() const {
     return ".idxFaissLSH"; 
}


/// Registration ///////////////////////////////////////////////////////////////////////



    MergeTreeIndexPtr faissLSHIndexCreator(const IndexDescription & index) {
        return std::make_shared<MergeTreeIndexFaissLSH>(index);
    }

    void faissLSHIndexValidator(const IndexDescription & /*index*/, bool /*attach*/) {
        /// ???
    }
}
