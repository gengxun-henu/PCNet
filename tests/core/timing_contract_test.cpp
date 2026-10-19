#include "pcnet/core/error_model.hpp"
#include <cassert>
#include <cmath>
#include <iostream>

int main() {
  pcnet::TemporalSupport interval; interval.intervals.push_back({10.0, 12.0, 1.0});
  const auto normalized = pcnet::normalize_support(interval, 10.0, 12.0);
  assert(normalized.available);
  assert(std::abs(normalized.support.intervals.front().start + 1.0) < 1e-12);
  assert(std::abs(normalized.support.intervals.front().end - 1.0) < 1e-12);
  pcnet::TemporalSupport midpoint; midpoint.intervals.push_back({11.0, 11.0, 1.0});
  const auto midpoint_result = pcnet::normalize_support(midpoint, 10.0, 12.0, true);
  assert(midpoint_result.available && midpoint_result.degraded);
  assert(std::abs(midpoint_result.support.intervals.front().start) < 1e-12);

  pcnet::NetworkView view;
  view.images.push_back({0, "A", "a", 10, 10});
  view.images.push_back({1, "B", "b", 10, 10});
  view.points.resize(2);
  pcnet::TemporalSupport a; a.intervals.push_back({0.0, 1.0, 1.0});
  pcnet::TemporalSupport b; b.intervals.push_back({100.0, 101.0, 1.0});
  view.measures.push_back({0, 0, 1, 1, a, 1, 0, pcnet::ReasonCode::None, pcnet::TimingSource::DetectorInterval, false});
  view.measures.push_back({1, 1, 1, 1, b, 1, 0, pcnet::ReasonCode::None, pcnet::TimingSource::DetectorInterval, false});
  const auto per_measure = pcnet::normalize_view_timing(view);
  assert(per_measure.size() == 2);
  assert(std::abs(per_measure[0].support.intervals.front().start + 1.0) < 1e-12);
  assert(std::abs(per_measure[1].support.intervals.front().start + 1.0) < 1e-12);
  std::cout << "timing_contract_test passed\n";
}
