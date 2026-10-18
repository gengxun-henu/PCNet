#include "pcnet/thinning/thinner.hpp"
#include <algorithm>
#include <set>
namespace pcnet {
namespace {
std::uint64_t active_points(const NetworkView &view) { std::uint64_t count = 0; for (const auto &p : view.points) if (!(p.state_flags & kStateDisabled)) ++count; return count; }
std::uint64_t active_measures(const NetworkView &view) { std::uint64_t count = 0; for (const auto &m : view.measures) if (!(m.state_flags & kStateDisabled)) ++count; return count; }
bool can_disable(const NetworkView &view, std::uint32_t point_index, std::uint64_t min_per_image, const ThinConfig &config) {
  std::vector<std::uint64_t> counts(view.images.size()); for (const auto &m : view.measures) if (!(m.state_flags & kStateDisabled) && m.point_index != point_index && m.image_index < counts.size()) ++counts[m.image_index]; for (auto count : counts) if (count < min_per_image) return false;
  if (config.min_model_rank > 0) { auto candidate = view; for (auto &m : candidate.measures) if (m.point_index == point_index) m.state_flags |= kStateDisabled; auto metrics = analyze_model(candidate, config.model); if (metrics.rank < config.min_model_rank) return false; }
  return true;
}
}
ThinResult thin(const NetworkView &input, const ThinConfig &config) {
  ThinResult result{input, true, {}}; std::set<std::uint32_t> protected_points(config.protected_points.begin(), config.protected_points.end());
  if (config.max_control_points && protected_points.size() > config.max_control_points) { result.feasible = false; result.conflicts.push_back("protected points exceed max_control_points"); return result; }
  auto disable_point = [&](std::uint32_t point_index) { auto &point = result.output.points.at(point_index); point.state_flags |= kStateDisabled; for (auto &m : result.output.measures) if (m.point_index == point_index) { m.state_flags |= kStateDisabled; m.reason_code = ReasonCode::DuplicateObservation; } };
  while (config.max_control_points && active_points(result.output) > config.max_control_points) {
    bool removed = false; for (std::size_t i = result.output.points.size(); i-- > 0;) { const auto index = static_cast<std::uint32_t>(i); if (result.output.points[i].state_flags & 1 || protected_points.count(index) || !can_disable(result.output, index, config.min_active_points_per_image, config)) continue; disable_point(index); removed = true; break; }
    if (!removed) { result.feasible = false; result.conflicts.push_back("point budget conflicts with protection or image coverage"); return result; }
  }
  while (config.max_measures && active_measures(result.output) > config.max_measures) {
    bool removed = false; for (std::size_t i = result.output.points.size(); i-- > 0;) { const auto index = static_cast<std::uint32_t>(i); if (result.output.points[i].state_flags & 1 || protected_points.count(index) || !can_disable(result.output, index, config.min_active_points_per_image, config)) continue; disable_point(index); removed = true; break; }
    if (!removed) { result.feasible = false; result.conflicts.push_back("measure budget conflicts with protection or image coverage"); return result; }
  }
  if (!result.feasible && !config.allow_constraint_violations) result.output = input; return result;
}
}
