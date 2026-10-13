#pragma once

#include "pcnet/temporal_support.h"

#include <cstdint>

namespace pcnet {

enum class TimingSource : std::uint8_t {
  ExposureInterval,
  ExposureMidpoint,
  LinearInterpolation,
};

struct ObservationTiming {
  double scan_sample = 0.0;
  double scan_line = 0.0;
  std::int32_t channel_id = -1;
  TemporalSupport support;
  TimingSource source = TimingSource::ExposureInterval;
  bool time_degraded = false;
};

}  // namespace pcnet
