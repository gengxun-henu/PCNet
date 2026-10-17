#include "pcnet/metrics/quality.hpp"
#include <cassert>
#include <cmath>
#include <iostream>

static pcnet::NetworkView make_view() {
  pcnet::NetworkView view;
  view.images.push_back({0, "SN-1", "a.cub", 100, 100});
  view.images.push_back({1, "SN-2", "b.cub", 100, 100});
  view.points.resize(2);
  view.points[0] = {0, 2, 0, 2}; view.points[1] = {1, 2, 2, 2};
  for (std::uint32_t i = 0; i < 4; ++i) {
    pcnet::TemporalSupport timing;
    timing.intervals.push_back({static_cast<double>(i), static_cast<double>(i) + 0.25, 1.0});
    view.measures.push_back({i < 2 ? 0u : 1u, i % 2, 10.0 + i * 20.0, static_cast<double>(i), timing, 1.0, 1, pcnet::ReasonCode::None});
  }
  return view;
}

int main() {
  pcnet::QualityConfig config; config.model = pcnet::ErrorModel::Velocity; config.time_bins = 8;
  const auto report = pcnet::evaluate_quality(make_view(), config);
  assert(report.images.size() == 2);
  assert(report.pairs.size() == 1);
  assert(report.network.n_images == 2);
  assert(report.images[0].time_coverage > 0.0);
  assert(report.images[0].cross_track_coverage > 0.0);
  assert(report.metadata.time_bins == 8);
  assert(!report.metadata.denominator.empty());
  assert(report.pairs[0].overlap_fallback);
  std::cout << "quality_report_test passed\n";
}
