#pragma once

#include "pcnet/temporal_support.h"

#include <stdexcept>
#include <vector>

namespace pcnet {

enum class ErrorModel {
  Constant,
  Velocity,
  Acceleration,
};

inline std::vector<double> design_vector(ErrorModel model,
                                         const TemporalSupport &support) {
  const double duration = support.total_duration();
  if (duration <= 0.0) {
    throw std::invalid_argument("error model requires non-empty temporal support");
  }

  const double mean_tau = support.first_moment() / duration;
  if (model == ErrorModel::Constant) {
    return {1.0};
  }

  if (model == ErrorModel::Velocity) {
    return {1.0, mean_tau};
  }

  return {1.0, mean_tau, support.second_moment() / duration};
}

}  // namespace pcnet
