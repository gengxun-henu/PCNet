#include "pcnet/temporal_support.h"

#include <cassert>
#include <cmath>

int main() {
  pcnet::TemporalSupport support;
  support.add_segment(0.0, 2.0, 1.0);
  support.add_segment(4.0, 6.0, 1.0);

  assert(std::abs(support.total_duration() - 4.0) < 1e-12);
  assert(std::abs(support.first_moment() - 12.0) < 1e-12);
  assert(std::abs(support.second_moment() - 160.0 / 3.0) < 1e-12);
  assert(support.segment_count() == 2);
}
