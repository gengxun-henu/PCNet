#pragma once
#include "pcnet/core/network_view.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace pcnet {
enum class ErrorModel { Constant, Velocity, Acceleration };
struct NormalizedTiming { TemporalSupport support; bool available = false; bool degraded = false; double envelope_start = 0.0; double envelope_end = 0.0; };
struct TemporalMoments { double mean = 0.0; double second = 0.0; double total_weight = 0.0; };
struct ModelConfig { std::uint32_t time_bins = 8; double regularization = 1e-9; };
struct ModelMetrics {
  ErrorModel model = ErrorModel::Constant;
  std::size_t rank = 0;
  double condition_number = 0.0;
  double max_prediction_variance = 0.0;
  std::size_t available_measures = 0;
  std::size_t unavailable_measures = 0;
  bool high_order_timing_complete = true;
};
TemporalMoments temporal_moments(const TemporalSupport &support);
bool validate_time_bins(std::uint32_t bins);
NormalizedTiming normalize_support(const TemporalSupport &, double envelope_start, double envelope_end, bool degraded = false);
std::vector<NormalizedTiming> normalize_view_timing(const NetworkView &, bool exclude_disabled = true);
ModelMetrics analyze_model(const NetworkView &, ErrorModel, const ModelConfig & = {});
std::string error_model_name(ErrorModel);
}
