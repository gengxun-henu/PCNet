#pragma once
#include "pcnet/core/reason_codes.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace pcnet {
constexpr std::uint8_t kStateDisabled = 1u;
struct TemporalInterval { double start = 0.0; double end = 0.0; double weight = 1.0; };
struct TemporalSupport { std::vector<TemporalInterval> intervals; };
struct ImageRecord {
  std::uint32_t image_index = 0; std::string serial_number; std::string name;
  std::uint32_t width = 0; std::uint32_t height = 0;
};
struct PointRecord {
  std::uint32_t point_index = 0; std::uint8_t state_flags = 0;
  std::uint64_t measure_begin = 0; std::uint32_t measure_count = 0;
};
struct MeasureRecord {
  std::uint32_t image_index = 0; std::uint32_t point_index = 0;
  double sample = 0.0; double line = 0.0; TemporalSupport timing;
  double quality_weight = 1.0; std::uint8_t state_flags = 0;
  ReasonCode reason_code = ReasonCode::None;
};
struct NetworkView {
  std::vector<ImageRecord> images; std::vector<PointRecord> points;
  std::vector<MeasureRecord> measures;
  std::vector<std::string> quality_columns;
  std::vector<std::vector<double>> quality_values;
  std::string validate() const;
};
}
