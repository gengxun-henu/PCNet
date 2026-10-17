#include "pcnet/filtering/outlier_filter.hpp"
#include <cassert>
#include <iostream>

int main() {
  pcnet::NetworkView view;
  view.images.push_back({0, "SN-1", "a.cub", 100, 100});
  view.points.resize(3);
  for (std::uint32_t i = 0; i < 3; ++i) { view.points[i] = {i, 0, i, 1}; view.measures.push_back({0, i, 10.0 + i, 20.0 + i, {}, 1.0, 0, pcnet::ReasonCode::None}); }
  pcnet::FilterEvidence evidence(3);
  evidence[0].has_geometry = true; evidence[0].geometry_score = 0.1;
  evidence[1].has_residual = true; evidence[1].residual_magnitude = 20.0;
  evidence[2].has_dom = true; evidence[2].dom_distance = 100.0;
  pcnet::FilterConfig config; config.residual_threshold = 10.0; config.geometry_threshold = 0.5; config.dom_support_threshold = 10.0;
  const auto result = pcnet::filter_outliers(view, evidence, config);
  assert(result.output.measures[0].state_flags & pcnet::kStateDisabled);
  assert(result.output.measures[0].reason_code == pcnet::ReasonCode::GeometricOutlier);
  assert(result.output.measures[1].reason_code == pcnet::ReasonCode::ResidualOutlier);
  assert(result.output.measures[2].state_flags == 0);
  assert(result.output.measures[2].reason_code == pcnet::ReasonCode::None);
  assert(result.audit.size() == 2);
  std::cout << "outlier_filter_test passed\n";
}
