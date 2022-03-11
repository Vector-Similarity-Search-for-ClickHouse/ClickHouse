#include "scann/scann_ops/cpp_interface/scann_ops_pybind.hpp"

namespace research_scann {

ScannSearcher::ScannSearcher(ConstDataSetWrapper<float, 2> dataset,
                             const std::string& config, int training_threads) {
  ConstSpan<float> span_dataset(dataset.Data(), dataset.Size());
  // RuntimeError???
  scann_.Initialize(span_dataset, dataset.Shape()[0], config, training_threads);
}

std::pair<std::vector<DatapointIndex>, std::vector<float>>
ScannSearcher::Search(ConstDataSetWrapper<float, 1> query,
                      int final_num_neighbors = -1,
                      int pre_reorder_num_neighbors = -1,
                      int leaves_to_search = -1) {
  DatapointPtr<float> ptr(nullptr, query.Data(), query.Size(), query.Size());
  NNResultsVector res;
  auto status = scann_.Search(ptr, &res, final_num_neighbors, pre_reorder_num_neighbors, leaves_to_search);
  // Check status 

  // ???? return res;
  std::vector<DatapointIndex> indices(res.size());
  std::vector<float> distances(res.size());
  
}

}  // namespace research_scann