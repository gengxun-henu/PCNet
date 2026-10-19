#pragma once
#include <cstdint>
#include <vector>

namespace pcnet {
struct TemporalInterval { double start = 0.0; double end = 0.0; double weight = 1.0; };
struct TemporalSupport { std::vector<TemporalInterval> intervals; };
enum class TimingSource : std::uint8_t {
  DetectorInterval = 0, ExposureInterval = DetectorInterval,
  EphemerisMidpoint = 1, ExposureMidpoint = EphemerisMidpoint,
  LineInterpolation = 2, LinearInterpolation = LineInterpolation,
  Unavailable = 3
};
struct ObservationTiming {
  double scan_sample = 0.0;
  double scan_line = 0.0;
  std::int32_t channel_id = -1;
  TemporalSupport support;
  TimingSource source = TimingSource::Unavailable;
  bool degraded = false;
};
}
