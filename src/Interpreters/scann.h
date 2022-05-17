#pragma once

#include <array>
#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <Poco/Util/AbstractConfiguration.h>

#include "dataset.h"
#include "io.h"


struct ScaNNWrapperAPI;

class ScaNNAPIrovider
{
public:
    virtual ~ScaNNAPIrovider() = default;
    virtual const ScaNNWrapperAPI & getAPI() const = 0;
};

namespace scann
{

using DatapointIndex = uint32_t;

class ScannBuilder;

class ScannSearcher
{
public:
    ScannSearcher(const ScannSearcher&) = delete;
    ScannSearcher operator=(const ScannSearcher&) = delete;
    ~ScannSearcher();

    std::pair<std::vector<DatapointIndex>, std::vector<float>> SearchBatched(  // NOLINT
        ConstDataSetWrapper<float, 2> queries,
        int final_nn = -1,
        int pre_reorder_nn = -1,
        int leaves_to_search = -1,
        bool parallel = false);

    void Serialize(IWriter & writer);  // NOLINT
    void Deserialize(IReader & reader);  // NOLINT

private:

    friend ScannBuilder;
    ScannSearcher(std::string lib_path, ConstDataSetWrapper<float, 2> dataset,
                const std::string& config, int training_threads);

    using ScannSearcherHolder = void;

    std::string lib_path;
    std::shared_ptr<ScaNNAPIrovider> api_provider;

    ScannSearcherHolder * holder;
};

// std::optional<ScannSearcher> CreateScannSearcher(DB::ContextPtr context);

} // namespace scann
