#pragma once

#include <string>
#include <vector>

// #include "scann/scann_ops/cc/scann_npy.h"
#include "scann/scann_ops/cc/scann.h"
//#include "scann_builder.hpp"
#include "dataset.hpp"
#include "scann/utils/types.h"
#include "scann/utils/common.h"


namespace scann {

using research_scann::DatapointIndex;

class ScannSearcher {
 public:
  ScannSearcher(ConstDataSetWrapper<float, 2> dataset, const std::string& config, int training_threads);

//   std::pair<std::vector<DatapointIndex>, std::vector<float>> Search(ConstDataSetWrapper<float, 1> query, int final_num_neighbors = -1,
//          int pre_reorder_num_neighbors = -1,
//          int leaves_to_search = -1);

  std::pair<std::vector<DatapointIndex>, std::vector<float>> SearchBatched(ConstDataSetWrapper<float, 2> queries, int final_nn = -1,
         int pre_reorder_nn = -1,
         int leaves_to_search = -1, bool parallel = false);

//   ... SearchBatchedParallel(NpArray queries, int final_num_neighbors = -1,
//          int pre_reorder_num_neighbors = -1,
//          int leaves_to_search = -1);

  void Serialize(std::string artifacts_dir);

 private:
  research_scann::ScannInterface scann_;
};

// ScannSearcher LoadSearcher() {
//   // Not implemented
// }

}  // namespace research_scann
