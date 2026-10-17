#include "pcnet/thinning/thinner.hpp"
#include <cassert>
#include <iostream>

static pcnet::NetworkView make_view() {
  pcnet::NetworkView view;
  view.images.push_back({0, "SN-1", "a.cub", 100, 100});
  view.images.push_back({1, "SN-2", "b.cub", 100, 100});
  view.points.resize(3);
  for (std::uint32_t i = 0; i < 3; ++i) view.points[i] = {i, 0, i * 2, 2};
  for (std::uint32_t i = 0; i < 3; ++i) {
    view.measures.push_back({0, i, 10.0 + i, 20.0 + i, {}, 1.0, 0, pcnet::ReasonCode::None});
    view.measures.push_back({1, i, 30.0 + i, 40.0 + i, {}, 1.0, 0, pcnet::ReasonCode::None});
  }
  return view;
}

int main() {
  auto config = pcnet::ThinConfig{};
  config.max_control_points = 2;
  config.protected_points = {0};
  config.min_active_points_per_image = 1;
  const auto result = pcnet::thin(make_view(), config);
  assert(result.feasible);
  assert(result.output.points[0].state_flags == 0);
  assert(result.output.points[2].state_flags == 1);
  assert(result.output.points[2].measure_count == 2);
  assert(result.output.measures[4].reason_code == pcnet::ReasonCode::DuplicateObservation);

  auto impossible = config;
  impossible.max_control_points = 0;
  impossible.protected_points = {0, 1, 2};
  impossible.max_control_points = 2;
  const auto failed = pcnet::thin(make_view(), impossible);
  assert(!failed.feasible);
  assert(!failed.conflicts.empty());
  std::cout << "thinning_constraints_test passed\n";
}
