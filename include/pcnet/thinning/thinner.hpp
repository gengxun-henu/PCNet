#pragma once
#include "pcnet/core/network_view.hpp"
#include "pcnet/core/error_model.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace pcnet {
struct ThinConfig {
  std::uint64_t max_control_points = 0;
  std::uint64_t max_measures = 0;
  std::uint64_t min_active_points_per_image = 0;
  std::vector<std::uint32_t> protected_points;
  bool allow_constraint_violations = false;
  ErrorModel model = ErrorModel::Constant;
  std::size_t min_model_rank = 0;
};
struct ThinResult { NetworkView output; bool feasible = true; std::vector<std::string> conflicts; };
ThinResult thin(const NetworkView &, const ThinConfig &);
}
