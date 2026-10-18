#include "pcnet/core/error_model.hpp"
#include <cassert>
#include <iostream>

static pcnet::NetworkView make_view() {
  pcnet::NetworkView view;
  view.images.push_back({0, "SN-1", "image.cub", 10, 20});
  view.points.resize(3);
  for (std::uint32_t i = 0; i < 3; ++i) view.points[i] = {i, 1, i, 1};
  for (std::uint32_t i = 0; i < 3; ++i) {
    pcnet::TemporalSupport timing;
    timing.intervals.push_back({static_cast<double>(i), static_cast<double>(i) + 0.1, 1.0});
    view.measures.push_back({0, i, 1.0, static_cast<double>(i), timing, 1.0, 1, pcnet::ReasonCode::None});
  }
  return view;
}

int main() {
  const auto view = make_view();
  const auto constant = pcnet::analyze_model(view, pcnet::ErrorModel::Constant);
  const auto velocity = pcnet::analyze_model(view, pcnet::ErrorModel::Velocity);
  const auto acceleration = pcnet::analyze_model(view, pcnet::ErrorModel::Acceleration);
  assert(constant.rank == 1);
  assert(velocity.rank == 2);
  assert(acceleration.rank == 3);
  assert(velocity.condition_number >= 1.0);
  assert(acceleration.max_prediction_variance > 0.0);
  std::cout << "model_metrics_test passed\n";
}
