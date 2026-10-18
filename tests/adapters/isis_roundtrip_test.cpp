#include "pcnet/adapters/isis/controlnet_adapter.hpp"
#include <isis/ControlNet.h>
#include <isis/ControlPoint.h>
#include <isis/ControlMeasure.h>
#include <QString>
#include <cassert>
#include <filesystem>
#include <iostream>

int main() {
  const auto dir = std::filesystem::temp_directory_path();
  const auto input = dir / "pcnet-isis-input.net";
  const auto output = dir / "pcnet-isis-output.net";
  Isis::ControlNet net;
  auto *point = new Isis::ControlPoint("P1");
  auto *measure = new Isis::ControlMeasure();
  measure->SetCubeSerialNumber("SN-1");
  measure->SetCoordinate(10.0, 20.0);
  point->Add(measure);
  net.AddPoint(point);
  net.Write(QString::fromStdString(input.string()), true);
  auto view = pcnet::ControlNetAdapter::read(input);
  assert(view.images.size() == 1);
  assert(view.points.size() == 1);
  assert(view.points[0].id == "P1");
  assert(view.measures.size() == 1);
  assert(view.measures[0].sample == 10.0);
  view.measures[0].state_flags = 1;
  view.measures[0].reason_code = pcnet::ReasonCode::UserDisabled;
  pcnet::ControlNetAdapter::write(output, view, pcnet::WriteMode::PreserveDisabled, output.string() + ".audit.json");
  auto roundtrip = pcnet::ControlNetAdapter::read(output);
  assert(roundtrip.measures.size() == 1);
  assert(roundtrip.measures[0].state_flags == 1);
  assert(roundtrip.points[0].id == "P1");
  std::filesystem::remove(input);
  std::filesystem::remove(output);
  std::filesystem::remove(output.string() + ".audit.json");
  std::cout << "isis_roundtrip_test passed\n";
}
