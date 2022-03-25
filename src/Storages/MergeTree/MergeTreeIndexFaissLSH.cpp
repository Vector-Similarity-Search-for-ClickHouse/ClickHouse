#include <cstddef>
#include <Storages/MergeTree/MergeTreeIndexFaissLSH.h>
#include <faiss/IndexLSH.h>


namespace FaissLSH::Detail {

using namespace faiss::lsh::data;

template <typename T>
std::vector<T> readVector(size_t buffer_size, DB::ReadBuffer &istr) {
    size_t elements_count = buffer_size / sizeof(T);
    std::vector<T> buffer;
    buffer.resize(elements_count, T());
    istr.read(toCharPtr(buffer.data()), buffer_size);
    return std::move(buffer);
}

 void deserializeIntoIndex(IndexDataSetter& setter, DB::ReadBuffer &istr) {
     istr.read(toCharPtr(&setter.d_in), setter.fields_sizes[0]);
     istr.read(toCharPtr(&setter.d_out), setter.fields_sizes[1]);
     istr.read(toCharPtr(&setter.is_trained), setter.fields_sizes[2]);
     istr.read(toCharPtr(&setter.have_bias), setter.fields_sizes[3]);
     istr.read(toCharPtr(&setter.is_orthonormal), setter.fields_sizes[4]);

     setter.A = readVector<float>(setter.fields_sizes[5], istr);
     setter.b = readVector<float>(setter.fields_sizes[6], istr);

     istr.read(toCharPtr(&setter.nbits), setter.fields_sizes[7]);
     istr.read(toCharPtr(&setter.rotate_data), setter.fields_sizes[8]);
     istr.read(toCharPtr(&setter.train_thresholds), setter.fields_sizes[9]);

     setter.thresholds = readVector<float>(setter.fields_sizes[10], istr);
 }
}

namespace DB {
    MergeTreeIndexGranuleFaissLSH::MergeTreeIndexGranuleFaissLSH(
        const String & index_name_,
        const Block & index_sample_block_,
        const faiss::IndexLSH & index) : index_name(index_name_), index_sample_block(index_sample_block_), index_impl(index) {}

    void MergeTreeIndexGranuleFaissLSH::serializeBinary(WriteBuffer &ostr) const {
        faiss::lsh::data::IndexDataGetter data_getter(index_impl);
        auto index_data = data_getter.getFieldsRawData();


        // serialize fields sizes
        for(const auto& pair : index_data) {
            ostr.write(reinterpret_cast<const char*>(&pair.size), sizeof(pair.size));
        }

        // serialize field's data   
        for(const auto& pair : index_data) {
            ostr.write(pair.raw_ptr, pair.size);
        }

    }

    void MergeTreeIndexGranuleFaissLSH::deserializeBinary(ReadBuffer &istr, MergeTreeIndexVersion version) {

        if (version != 1)
            throw Exception(ErrorCodes::LOGICAL_ERROR, "Unknown index version {}.", version);

        faiss::lsh::data::IndexDataSetter index_data_setter(index_impl);
        size_t fields_count = index_data_setter.fieldsCount();
        for (size_t i = 0; i < fields_count; ++i) {
            size_t size = 0;
            istr.read(reinterpret_cast<char*>(&size), sizeof(size));
            index_data_setter.fields_sizes[i] = size;
        }

        FaissLSH::Detail::deserializeIntoIndex(index_data_setter, istr);

    }

    MergeTreeIndexPtr faissLSHIndexCreator(const IndexDescription & index) {
        return std::make_shared<MergeTreeIndexFaissLSH>(index);
    }

    void faissLSHIndexValidator(const IndexDescription & /*index*/, bool /*attach*/) {

    }
}
