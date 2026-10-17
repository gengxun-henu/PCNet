#include "pcnet/filtering/outlier_filter.hpp"
#include <cmath>
namespace pcnet {
FilterResult filter_outliers(const NetworkView &input, const FilterEvidence &evidence, const FilterConfig &config) {
  FilterResult result{input, {}, {}};
  const auto count = std::min(result.output.measures.size(), evidence.size());
  for (std::size_t i = 0; i < count; ++i) {
    const auto &e = evidence[i]; auto &measure = result.output.measures[i]; if (measure.state_flags & kStateDisabled) continue;
    if (e.has_geometry && config.geometry_threshold > 0.0 && e.geometry_score < config.geometry_threshold) { measure.state_flags |= kStateDisabled; measure.reason_code = ReasonCode::GeometricOutlier; result.audit.push_back({static_cast<std::uint32_t>(i), measure.reason_code, "local_geometry", e.geometry_score, 0}); continue; }
    if (e.has_residual && config.residual_threshold > 0.0 && e.residual_magnitude > config.residual_threshold) { measure.state_flags |= kStateDisabled; measure.reason_code = ReasonCode::ResidualOutlier; result.audit.push_back({static_cast<std::uint32_t>(i), measure.reason_code, "external_residual", e.residual_magnitude, 0}); continue; }
    if (e.has_dom && config.dom_support_threshold > 0.0 && e.dom_distance > config.dom_support_threshold && !e.has_geometry && !e.has_residual) result.unresolved.push_back(static_cast<std::uint32_t>(i));
  }
  return result;
}
}
