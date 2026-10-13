#include "pcnet/error_model.h"

#include <cassert>
#include <cmath>

int main() {
  pcnet::TemporalSupport support;
  support.add_segment(-1.0, 0.0);
  support.add_segment(0.5, 1.0);

  const auto constant = pcnet::design_vector(pcnet::ErrorModel::Constant, support);
  assert(constant.size() == 1);
  assert(std::abs(constant[0] - 1.0) < 1e-12);

  const auto velocity = pcnet::design_vector(pcnet::ErrorModel::Velocity, support);
  assert(velocity.size() == 2);
  assert(std::abs(velocity[0] - 1.0) < 1e-12);
  assert(std::abs(velocity[1] + 1.0 / 12.0) < 1e-12);

  const auto acceleration = pcnet::design_vector(pcnet::ErrorModel::Acceleration, support);
  assert(acceleration.size() == 3);
  assert(std::abs(acceleration[2] - 5.0 / 12.0) < 1e-12);
}
