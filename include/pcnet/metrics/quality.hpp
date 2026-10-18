#pragma once
#include "pcnet/core/error_model.hpp"
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace pcnet {
struct QualityMetadata { std::uint32_t time_bins = 8; std::string denominator; std::string coordinate_rule; };
struct ImageQuality {
  std::uint32_t image_index = 0; std::uint64_t n_points = 0, n_measures = 0, n_neighbors = 0;
  double time_coverage = 0.0, cross_track_coverage = 0.0, max_time_gap = 0.0;
  std::size_t design_rank = 0; double condition_number = 0.0; bool sparse = false;
  std::vector<std::string> warning_flags;
};
struct PairQuality {
  std::uint32_t image_a = 0, image_b = 0; std::uint64_t n_valid_tracks = 0, n_independent_tracks = 0;
  double overlap_area = 0.0, along_track_span = 0.0, cross_track_span = 0.0; bool overlap_fallback = false;
  std::vector<std::string> warning_flags;
};
struct NetworkQuality {
  std::uint64_t n_images = 0, n_points = 0, n_measures = 0; std::uint64_t n_components = 0, n_bridge_edges = 0;
  std::vector<std::uint64_t> point_count_quantiles; std::vector<std::uint32_t> weak_images;
};
struct QualityConfig { ErrorModel model = ErrorModel::Constant; std::uint32_t time_bins = 8; std::uint64_t sparse_point_threshold = 3; };
struct QualityReport { QualityMetadata metadata; std::vector<ImageQuality> images; std::vector<PairQuality> pairs; NetworkQuality network; };
QualityReport evaluate_quality(const NetworkView &, const QualityConfig & = {});
void write_quality_csv_json(const QualityReport &, const std::filesystem::path &directory);
}
