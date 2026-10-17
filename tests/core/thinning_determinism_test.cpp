#include "pcnet/thinning/thinner.hpp"
#include <cassert>
#include <iostream>

int main() {
  pcnet::NetworkView view;
  view.images.push_back({0, "SN-1", "a.cub", 100, 100});
  view.points.resize(4);
  for (std::uint32_t i = 0; i < 4; ++i) { view.points[i] = {i, 0, i, 1}; view.measures.push_back({0, i, static_cast<double>(i), static_cast<double>(i), {}, 1.0, 0, pcnet::ReasonCode::None}); }
  pcnet::ThinConfig config; config.max_control_points = 2; config.min_active_points_per_image = 1;
  const auto a = pcnet::thin(view, config); const auto b = pcnet::thin(view, config);
  assert(a.output.points[2].state_flags == b.output.points[2].state_flags);
  assert(a.output.points[3].state_flags == b.output.points[3].state_flags);
  std::cout << "thinning_determinism_test passed\n";
}
