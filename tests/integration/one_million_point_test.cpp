#include "pcnet/thinning/thinner.hpp"
#include <cassert>
#include <chrono>
#include <iostream>

int main() {
  constexpr std::uint32_t n = 1000000;
  pcnet::NetworkView view;
  view.images.push_back({0, "SYNTH-1", "synthetic", 1000, 1000});
  view.points.reserve(n); view.measures.reserve(n);
  for (std::uint32_t i = 0; i < n; ++i) { view.points.push_back({i, 0, i, 1}); view.measures.push_back({0, i, static_cast<double>(i % 1000), static_cast<double>(i / 1000), {}, 1.0, 0, pcnet::ReasonCode::None}); }
  const auto start = std::chrono::steady_clock::now();
  pcnet::ThinConfig config; config.max_control_points = n; config.min_active_points_per_image = 1;
  const auto result = pcnet::thin(view, config);
  const auto elapsed = std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
  assert(result.feasible); assert(result.output.points.size() == n); assert(result.output.measures.size() == n);
  std::cout << "one_million_point_test passed seconds=" << elapsed << " points=" << n << "\n";
}
