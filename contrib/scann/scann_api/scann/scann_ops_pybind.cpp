#include "scann_ops_pybind.hpp"

namespace scann {

using research_scann::ConstSpan;
using research_scann::DatapointPtr;
using research_scann::DenseDataset;
using research_scann::MakeMutableSpan;
using research_scann::NNResultsVector;
using research_scann::Status;
using research_scann::MakeConstSpan;



ScannSearcher::ScannSearcher(ConstDataSetWrapper<float, 2> dataset,
                             const std::string& config, int training_threads) {
  ConstSpan<float> span_dataset(dataset.Data(), dataset.Size());

  scann_.Initialize(span_dataset, dataset.Shape()[0], config, training_threads);
}

// std::pair<std::vector<DatapointIndex>, std::vector<float>>
// ScannSearcher::Search(ConstDataSetWrapper<float, 1> query,
//                       int final_num_neighbors,
//                       int pre_reorder_num_neighbors,
//                       int leaves_to_search) {
//   DatapointPtr<float> ptr(nullptr, query.Data(), query.Size(), query.Size());
//   NNResultsVector res;
//   auto status = scann_.Search(ptr, &res, final_num_neighbors,
//   pre_reorder_num_neighbors, leaves_to_search);
//   // Check status

//   // ???? return res;
//   std::vector<DatapointIndex> indices(res.size());
//   std::vector<float> distances(res.size());

// }

std::pair<std::vector<DatapointIndex>, std::vector<float>>
ScannSearcher::SearchBatched(ConstDataSetWrapper<float, 2> queries,
                             int final_nn, int pre_reorder_nn,
                             int leaves_to_search, bool parallel) {
  std::vector<float> queries_vec(queries.Data(),
                                 queries.Data() + queries.Size());
  auto query_dataset = DenseDataset<float>(queries_vec, queries.Shape()[0]);

  std::vector<NNResultsVector> res(query_dataset.size());
  //Status status;
  if (parallel)
    scann_.SearchBatchedParallel(query_dataset, MakeMutableSpan(res),
                                          final_nn, pre_reorder_nn,
                                          leaves_to_search);
  else
    scann_.SearchBatched(query_dataset, MakeMutableSpan(res), final_nn,
                                  pre_reorder_nn, leaves_to_search);

  for (const auto& nn_res : res) {
    final_nn = std::max<int>(final_nn, nn_res.size());
  }
  
  std::vector<DatapointIndex> indices(query_dataset.size() * final_nn);
  std::vector<float> distances(query_dataset.size() * final_nn);
  

  scann_.ReshapeBatchedNNResult(MakeConstSpan(res), indices.data(), distances.data(), final_nn);
  return {indices, distances};
}

}  // namespace scann