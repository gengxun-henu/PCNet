#include "pcnet/core/network_view.hpp"
#include <cassert>
#include <cstdint>
#include <iostream>

int main() {
  pcnet::NetworkView view;
  view.images.push_back({0, "SN-1", "image.cub", 1024, 2048});
  view.points.push_back({0, 1, 0, 1});
  view.measures.push_back({0, 0, 12.5, 42.25, {}, 1.0, 1, pcnet::ReasonCode::None});
  assert(view.validate().empty());
  view.measures[0].image_index = 7;
  assert(!view.validate().empty());
  std::cout << "network_view_test passed\n";
}
