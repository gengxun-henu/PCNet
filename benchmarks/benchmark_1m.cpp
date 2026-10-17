#include "pcnet/thinning/thinner.hpp"
#include <chrono>
#include <fstream>
#include <iostream>
int main(int argc, char **argv) {
  const std::uint32_t n = 1000000; pcnet::NetworkView view; view.images.push_back({0, "SYNTH-1", "synthetic", 1000, 1000}); view.points.reserve(n); view.measures.reserve(n);
  for (std::uint32_t i = 0; i < n; ++i) { view.points.push_back({i, 0, i, 1}); view.measures.push_back({0, i, static_cast<double>(i % 1000), static_cast<double>(i / 1000), {}, 1.0, 0, pcnet::ReasonCode::None}); }
  const auto start = std::chrono::steady_clock::now(); pcnet::ThinConfig config; config.max_control_points = n; config.min_active_points_per_image = 1; const auto result = pcnet::thin(view, config); const double seconds = std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
  const std::string output = argc > 1 ? argv[1] : "benchmark_1m.json"; std::ofstream out(output); out << "{\"control_points\":" << n << ",\"control_measures\":" << n << ",\"seconds\":" << seconds << ",\"feasible\":" << (result.feasible ? "true" : "false") << "}\n"; std::cout << output << '\n'; return result.feasible ? 0 : 1;
}
