#include "pcnet/thinning/thinner.hpp"
#include <cassert>
#include <iostream>

int main() {
  pcnet::NetworkView bridge;
  bridge.images.push_back({0,"A","A",10,10}); bridge.images.push_back({1,"B","B",10,10});
  bridge.points.resize(2); bridge.points[0]={0,0,0,2}; bridge.points[1]={1,0,2,1};
  bridge.measures.push_back({0,0,1,1,{},1,0,pcnet::ReasonCode::None});
  bridge.measures.push_back({1,0,2,2,{},1,0,pcnet::ReasonCode::None});
  bridge.measures.push_back({0,1,3,3,{},1,0,pcnet::ReasonCode::None});
  pcnet::ThinConfig bridge_config; bridge_config.max_control_points=1; bridge_config.min_active_points_per_image=0;
  const auto bridge_result=pcnet::thin(bridge,bridge_config);
  assert(bridge_result.feasible);
  assert(bridge_result.output.points[0].state_flags==0);
  assert(bridge_result.output.points[1].state_flags & pcnet::kStateDisabled);

  pcnet::NetworkView timed; timed.images.push_back({0,"A","A",10,10}); timed.points.resize(3);
  for(std::uint32_t i=0;i<3;++i){timed.points[i]={i,0,i,1};pcnet::TemporalSupport t;t.intervals.push_back({static_cast<double>(i),static_cast<double>(i)+0.1,1});timed.measures.push_back({0,i,1,1,t,1,0,pcnet::ReasonCode::None});}
  pcnet::ThinConfig model_config; model_config.max_control_points=2; model_config.model=pcnet::ErrorModel::Acceleration; model_config.min_model_rank=3; model_config.min_active_points_per_image=0;
  const auto model_result=pcnet::thin(timed,model_config);
  assert(!model_result.feasible);
  assert(!model_result.conflicts.empty());
  std::cout << "thinning_bridge_model_test passed\n";
}
