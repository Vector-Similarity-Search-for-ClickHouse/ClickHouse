
#include "scann_builder.hpp"

#include <cstdio>

ScannBuilder& ScannBuilder::Tree(size_t num_leaves, size_t num_leaves_to_search,
                                 size_t training_sample_size = 100000,
                                 size_t min_partition_size = 50,
                                 size_t training_iterations = 12,
                                 bool spherical = false,
                                 bool quantize_centroids = false,
                                 bool random_init = true) {
  tree_args_ = detail::TreeArgs{num_leaves,           num_leaves_to_search,
                                training_sample_size, min_partition_size,
                                training_iterations,  spherical,
                                quantize_centroids,   random_init};
  return *this;
}

ScannBuilder& ScannBuilder::ScoreAh(size_t dimension_per_block,
                                    float anisotropic_quantization_threshold,
                                    size_t training_sample_size = 100000,
                                    size_t min_cluster_size = 100,
                                    std::string hash_type = "lut16",
                                    size_t training_iterations = 10) {
  score_ah_args_ = detail::ScoreAHArgs{dimension_per_block,
                                       anisotropic_quantization_threshold,
                                       training_sample_size,
                                       min_cluster_size,
                                       hash_type,
                                       training_iterations};
  return *this;
}

ScannBuilder& ScannBuilder::ScoreBruteForce(bool quantize = false) {
  score_brute_force_args_ = detail::ScoreBruteForceArgs{quantize};
  return *this;
}

ScannBuilder& ScannBuilder::Reorder(size_t reordering_num_neighbors,
                                    bool quantize = false) {
  reorder_args_ = detail::ReoederArgs{reordering_num_neighbors, quantize};
  return *this;
}

ScannSearcher ScannBuilder::Build() {
  ScannSearcher(db_, CreateConfig(), training_threads_);
}

std::string ScannBuilder::GenerateTreeConfig(detail::TreeArgs& args) {
  static const char* fmt_template =
      "\
          partitioning {{\
            num_children: %u\
            min_cluster_size: %u\
            max_clustering_iterations: %u\
            single_machine_center_initialization: %s\
            partitioning_distance {{\
            distance_measure: \"SquaredL2Distance\"\
            }}\
            query_spilling {{\
            spilling_type: FIXED_NUMBER_OF_CENTERS\
            max_spill_centers: %u\
            }}\
            expected_sample_size: %u\
            query_tokenization_distance_override %s\
            partitioning_type: %s\
            query_tokenization_type: %s\
          }}\
        ";
  char complete_str[1024];
  std::sprintf(
      complete_str, fmt_template, args.num_leaves, args.min_partition_size,
      args.training_iterations,
      args.random_init ? "RANDOM_INITIALIZATION" : "DEFAULT_KMEANS_PLUS_PLUS",
      args.num_leaves_to_search, args.training_sample_size,
      args.distance_measure, args.spherical ? "SPHERICAL" : "GENERIC",
      args.quantize_centroids ? "FIXED_POINT_INT8" : "FLOAT");
  return complete_str;
}

std::string ScannBuilder::GenerateScoreAHTreeConfig(detail::ScoreAHArgs& args) {
  size_t clusters_per_block;
  std::string lookup_type;
  if (args.hash_type == "lut16") {
    clusters_per_block = 16;
    lookup_type = "INT8_LUT16";
  } else if (args.hash_type == "lut256") {
    clusters_per_block = 256;
    lookup_type = "INT8";
  } else {
    // exception?
  }

  char proj_config[256];

  if (args.n_dims % args.dimension_per_block == 0) {
    static const char* fmt_template =
        "\
        projection_type: CHUNK\
        num_blocks: %u\
        num_dims_per_block: %u\
      ";

    std::sprintf(proj_config, fmt_template,
                 args.n_dims / args.dimension_per_block,
                 args.dimension_per_block);
  } else {
    static const char* fmt_template =
        "\
        projection_type: VARIABLE_CHUNK\
        variable_blocks {{\
          num_blocks: %u\
          num_dims_per_block: %u\
        }}\
        variable_blocks {{\
          num_blocks: %d\
          num_dims_per_block: %u\
        }}\
          ";

    std::sprintf(
        proj_config, fmt_template, args.n_dims / args.dimension_per_block,
        args.dimension_per_block, 1, args.n_dims % args.dimension_per_block);
  }

  size_t num_blocks =
      std::ceil(static_cast<double>(args.n_dims) / args.dimension_per_block);
  bool global_topn = (args.hash_type == "lut16") && (num_blocks <= 256) &&
                     args.residual_quantization;

  static const char* fmt_template =
      "\
      hash {{\
        asymmetric_hash {{\
          lookup_type: %s\
          use_residual_quantization: %s\
          use_global_topn: %s\
          quantization_distance {{\
            distance_measure: \"SquaredL2Distance\"\
          }}\
          num_clusters_per_block: %u\
          projection {{\
            input_dim: %u\
            %s\
          }}\
          noise_shaping_threshold: %f\
          expected_sample_size: %u\
          min_cluster_size: %u\
          max_clustering_iterations: %u\
        }}\
      }} ";
  char complete_str[1024];
  std::sprintf(complete_str, fmt_template, lookup_type,
               BoolToString(args.residual_quantization),
               BoolToString(global_topn), clusters_per_block, args.n_dims,
               proj_config, args.anisotropic_quantization_threshold,
               args.training_iterations, args.min_cluster_size,
               args.training_iterations);
  return complete_str;
}

std::string ScannBuilder::GenerateScoreBruteForceConfig(
    detail::ScoreBruteForceArgs& args) {
  static const char* fmt_template =
      "\
      brute_force {{\
        fixed_point {{\
          enabled: %s\
        }}\
      }}\
    ";
  char complete_str[256];
  std::sprintf(complete_str, fmt_template, BoolToString(args.quantize));
  return complete_str;
}

std::string ScannBuilder::GenerateReorderConfig(detail::ReoederArgs& args) {
  static const char* fmt_template =
      "\
      exact_reordering {{\
        approx_num_neighbors: %u\
        fixed_point {{\
          enabled: %s\
        }}\
      }}\
    ";
  char complete_str[256];
  std::sprintf(complete_str, fmt_template, args.reordering_num_neighbors,
               BoolToString(args.quantize));
  return complete_str;
}

std::string ScannBuilder::CreateConfig() {
  std::string distance_measure_conf;
  if (distance_measure_ == "dot_product") {
    distance_measure_conf = '{distance_measure: "DotProductDistance"}';
  } else if (distance_measure_ == "squared_l2") {
    distance_measure_conf = '{distance_measure: "SquaredL2Distance"}';
  } else {
    // exception?
  }
  static const char* fmt_template =
      "\
      num_neighbors: %u\
      distance_measure %s\
    ";
  char complete_str[128];
  std::sprintf(complete_str, fmt_template, num_neighbors_,
               distance_measure_conf);
  std::string config = complete_str;

  if (tree_args_.has_value()) {
    tree_args_.value().distance_measure = distance_measure_conf;
    config += GenerateTreeConfig(tree_args_.value());
  }

  if (score_ah_args_.has_value() && !score_brute_force_args_.has_value()) {
    score_ah_args_.value().residual_quantization =
        tree_args_.has_value() && distance_measure_ == "dot_product";
    score_ah_args_.value().n_dims = db_.Shape()[1]; 
    config += GenerateScoreAHTreeConfig(score_ah_args_.value());
  } else if (!score_ah_args_.has_value() && score_brute_force_args_.has_value()) {
    config += GenerateScoreBruteForceConfig(score_brute_force_args_.value());
  } else {
    // exception?
  }

  if (reorder_args_.has_value()) {
    config += GenerateReorderConfig(reorder_args_.value());
  }

  return config;
}

std::string ScannBuilder::BoolToString(bool value) {
  return value ? "True" : "False";
}