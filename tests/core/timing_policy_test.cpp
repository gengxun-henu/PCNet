#include "pcnet/core/error_model.hpp"
#include <cassert>
#include <iostream>

int main() {
  pcnet::NetworkView view;
  view.images.push_back({0, "SN-1", "image.cub", 10, 20});
  view.points.push_back({0, 1, 0, 2});
  pcnet::TemporalSupport support;
  support.intervals.push_back({10.0, 11.0, 1.0});
  view.measures.push_back({0, 0, 1.0, 1.0, support, 1.0, 0, pcnet::ReasonCode::None});
  view.measures.push_back({0, 0, 2.0, 2.0, {}, 1.0, 0, pcnet::ReasonCode::None});
  const auto metrics = pcnet::analyze_model(view, pcnet::ErrorModel::Velocity);
  assert(metrics.available_measures == 1);
  assert(metrics.unavailable_measures == 1);
  assert(!metrics.high_order_timing_complete);
  std::cout << "timing_policy_test passed\n";
}
