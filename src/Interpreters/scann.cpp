#include "scann.h"

#include <Common/SharedLibrary.h>

#include <mutex>
#include <optional>
#include <string>

#include "base/logger_useful.h"

struct ScaNNWrapperAPI
{
    void * (*ScannSearcherCreate)( // NOLINT
        scann::ConstDataSetWrapper<float, 2>* dataset,
        std::string* config,
        int training_threads);

    void (*ScannSearcherDestroy)(void * self); // NOLINT

    void * (*ScannSearcherSearchBatched)( // NOLINT
        void * self,
        scann::ConstDataSetWrapper<float, 2> queries,
        int final_nn,
        int pre_reorder_nn,
        int leaves_to_search,
        bool parallel);

    void (*ScannSearcherSerialize)(void * self, scann::IWriter & writer); // NOLINT

    void (*ScannSearcherDeserialize)( // NOLINT
        void * self, 
        scann::IReader & reader);
};

class ScaNNLibHolder : public ScaNNAPIrovider
{
public:
    explicit ScaNNLibHolder(std::string lib_path_) : lib_path(std::move(lib_path_)), lib(lib_path) { initAPI(); }

    const ScaNNWrapperAPI & getAPI() const override { return api; }
    const std::string & getCurrentPath() const { return lib_path; }

private:
    ScaNNWrapperAPI api;
    std::string lib_path;
    DB::SharedLibrary lib;

    void initAPI();

    template <typename T>
    void load(T & func, const std::string & name)
    {
        func = lib.get<T>(name);
    }

    template <typename T>
    void tryLoad(T & func, const std::string & name)
    {
        func = lib.tryGet<T>(name);
    }
};

void ScaNNLibHolder::initAPI()
{
    load(api.ScannSearcherCreate, "ScannSearcherCreate");
    load(api.ScannSearcherDestroy, "ScannSearcherDestroy");
    load(api.ScannSearcherSearchBatched, "ScannSearcherSearchBatched");
    load(api.ScannSearcherSerialize, "ScannSearcherSerialize");
    load(api.ScannSearcherDeserialize, "ScannSearcherDeserialize");
}

std::shared_ptr<ScaNNLibHolder> getScaNNLibHolder(const std::string & lib_path)
{
    static std::shared_ptr<ScaNNLibHolder> ptr;
    static std::mutex mutex;

    std::lock_guard lock(mutex);

    if (!ptr || ptr->getCurrentPath() != lib_path)
        ptr = std::make_shared<ScaNNLibHolder>(lib_path);

    return ptr;
}

namespace scann
{

// std::optional<ScannSearcher> CreateScannSearcher(DB::ContextPtr /*context*/)
// {
//     // if (context == nullptr) {
//     //     LOG_DEBUG(&Poco::Logger::get("ScaNN"), "Faill ScaNNSearcher");
//     //     return std::nullopt;
//     // }
//     // LOG_DEBUG(&Poco::Logger::get("ScaNN"), "Create ScaNNSearcher");
//     // return
//     // ScannSearcher(context->getConfigRef().getString("scann_dynamic_library_path"));
//     return ScannSearcher("/home/ubuntu/disk/NikeVas/ClickHouse/contrib/google-research/scann/"
//                          "build/scann_api/scann/libscann_interface.so");
// }

ScannSearcher::ScannSearcher(std::string lib_path_, ConstDataSetWrapper<float, 2> dataset, const std::string & config, int training_threads)
    : lib_path(std::move(lib_path_))
{
    LOG_DEBUG(&Poco::Logger::get("ScaNN"), "TRY Load Lib");
    api_provider = getScaNNLibHolder(lib_path);
    LOG_DEBUG(&Poco::Logger::get("ScaNN"), "Lib loaded");
    std::string str = config;
    holder = api_provider->getAPI().ScannSearcherCreate(&dataset, &str, training_threads);
}

ScannSearcher::~ScannSearcher()
{
    api_provider->getAPI().ScannSearcherDestroy(holder);
}


std::pair<std::vector<DatapointIndex>, std::vector<float>>
ScannSearcher::SearchBatched(ConstDataSetWrapper<float, 2> queries, int final_nn, int pre_reorder_nn, int leaves_to_search, bool parallel)
{
    void * raw_result
        = api_provider->getAPI().ScannSearcherSearchBatched(holder, queries, final_nn, pre_reorder_nn, leaves_to_search, parallel);
    auto * result = reinterpret_cast<std::pair<std::vector<DatapointIndex>, std::vector<float>> *>(raw_result);
    return std::move(*result);
}

void ScannSearcher::Serialize(IWriter & writer)
{
    api_provider->getAPI().ScannSearcherSerialize(holder, writer);
}
void ScannSearcher::Deserialize(IReader & reader)
{
    api_provider->getAPI().ScannSearcherDeserialize(holder, reader);
}

} // namespace scann
