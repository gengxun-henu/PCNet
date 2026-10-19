#pragma once
#include "pcnet/core/reason_codes.hpp"
#include "pcnet/core/timing.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace pcnet {
constexpr std::uint8_t kStateDisabled = 1u;
struct ImageRecord {
  std::uint32_t image_index = 0; std::string serial_number; std::string name;
  std::uint32_t width = 0; std::uint32_t height = 0;
  std::uint32_t acquisition_group_id = 0; std::int32_t channel_id = -1;
  double acquisition_start = 0.0; double acquisition_end = 0.0;
};
struct PointRecord {
  std::uint32_t point_index = 0; std::uint8_t state_flags = 0;
  std::uint64_t measure_begin = 0; std::uint32_t measure_count = 0; std::string id;
};
struct MeasureRecord {
  std::uint32_t image_index = 0; std::uint32_t point_index = 0;
  double sample = 0.0; double line = 0.0; TemporalSupport timing;
  double quality_weight = 1.0; std::uint8_t state_flags = 0;
  ReasonCode reason_code = ReasonCode::None;
  TimingSource timing_source = TimingSource::Unavailable;
  bool timing_degraded = false;
};
struct NetworkView {
  std::vector<ImageRecord> images; std::vector<PointRecord> points;
  std::vector<MeasureRecord> measures;
  std::vector<std::string> quality_columns;
  std::vector<std::vector<double>> quality_values;
  std::string validate() const;
};
}
