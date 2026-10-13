#include "pcnet/observation_timing.h"

#include <cassert>

int main() {
  pcnet::ObservationTiming timing;
  timing.scan_sample = 12.5;
  timing.scan_line = 42.0;
  timing.channel_id = 3;
  timing.source = pcnet::TimingSource::ExposureInterval;
  timing.support.add_segment(10.0, 10.5);

  assert(timing.channel_id == 3);
  assert(timing.source == pcnet::TimingSource::ExposureInterval);
  assert(timing.support.segment_count() == 1);
  assert(!timing.time_degraded);
}
