#include "pcnet/core/error_model.hpp"
#include <cassert>
#include <cmath>
#include <iostream>

int main() {
  pcnet::TemporalSupport support;
  support.intervals.push_back({10.0, 12.0, 1.0});
  const auto moments = pcnet::temporal_moments(support);
  assert(std::abs(moments.mean - 11.0) < 1e-12);
  assert(std::abs(moments.second - (364.0 / 3.0)) < 1e-12);
  assert(moments.total_weight > 1.99 && moments.total_weight < 2.01);
  assert(!pcnet::validate_time_bins(3));
  assert(pcnet::validate_time_bins(4));
  assert(pcnet::validate_time_bins(32));
  assert(!pcnet::validate_time_bins(33));
  std::cout << "temporal_support_test passed\n";
}
