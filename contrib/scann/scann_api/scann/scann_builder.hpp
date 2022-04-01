#pragma once

#include <cmath>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "scann_ops_pybind.hpp"
#include "dataset.hpp"

// template <typename T>
// using np_row_major_arr =
//     pybind11::array_t<T, pybind11::array::c_style | pybind11::array::forcecast>;


namespace detail {
struct TreeArgs {
  size_t num_leaves;
  size_t num_leaves_to_search;
  size_t training_sample_size;
  size_t min_partition_size;
  size_t training_iterations;
  bool spherical;
  bool quantize_centroids;
  bool random_init;
  std::string distance_measure;
};
struct ScoreAHArgs {
  size_t dimension_per_block;
  float anisotropic_quantization_threshold;
  size_t training_sample_size;
  size_t min_cluster_size;
  std::string hash_type;
  size_t training_iterations;
  bool residual_quantization;
  size_t n_dims;
};
struct ScoreBruteForceArgs {
  bool quantize;
};
struct ReoederArgs {
  size_t reordering_num_neighbors;
  bool quantize = false;
};
}  // namespace detail

class ScannBuilder {
 public:
  ScannBuilder(ConstDataSetWrapper<float, 2> db, size_t num_neighbors, std::string distance_measure);

  void SetNTrainingThreads(int threads);

  ScannBuilder& Tree(size_t num_leaves, size_t num_leaves_to_search,
                                 size_t training_sample_size = 100000,
                                 size_t min_partition_size = 50,
                                 size_t training_iterations = 12,
                                 bool spherical = false,
                                 bool quantize_centroids = false,
                                 bool random_init = true);

  ScannBuilder& ScoreAh(size_t dimension_per_block,
                        float anisotropic_quantization_threshold,
                        size_t training_sample_size = 100000,
                        size_t min_cluster_size = 100,
                        std::string hash_type = "lut16",
                        size_t training_iterations = 10);

  ScannBuilder& ScoreBruteForce(bool quantize = false);

  ScannBuilder& Reorder(size_t reordering_num_neighbors, bool quantize = false);

  // use inheretance to diferent build
  // ScannBuilder& SetBuilderLambda(void* builder_lambda);
  scann::ScannSearcher Build();

 //private:
  std::string GenerateTreeConfig(detail::TreeArgs& args);
  std::string GenerateScoreAHTreeConfig(detail::ScoreAHArgs& args);
  std::string GenerateScoreBruteForceConfig(detail::ScoreBruteForceArgs& args);
  std::string GenerateReorderConfig(detail::ReoederArgs& args);

  std::string CreateConfig();

  const char* BoolToString(bool value);

 private:
  int training_threads_ = 0;

  ConstDataSetWrapper<float, 2> db_;
  size_t num_neighbors_;
  std::string distance_measure_;

  std::optional<detail::TreeArgs> tree_args_;
  std::optional<detail::ScoreAHArgs> score_ah_args_;
  std::optional<detail::ScoreBruteForceArgs> score_brute_force_args_;
  std::optional<detail::ReoederArgs> reorder_args_;
};