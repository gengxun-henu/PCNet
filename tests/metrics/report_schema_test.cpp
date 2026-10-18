#include "pcnet/metrics/quality.hpp"
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>

int main() {
  pcnet::NetworkView view;
  view.images.push_back({0, "SN-1", "a.cub", 100, 100});
  pcnet::QualityConfig config;
  auto report = pcnet::evaluate_quality(view, config);
  const auto dir = std::filesystem::temp_directory_path() / "pcnet-quality-test";
  std::filesystem::create_directories(dir);
  pcnet::write_quality_csv_json(report, dir);
  std::ifstream json(dir / "network_quality.json");
  std::string contents((std::istreambuf_iterator<char>(json)), {});
  assert(contents.find("ImageQuality") != std::string::npos);
  assert(contents.find("denominator") != std::string::npos);
  assert(std::filesystem::exists(dir / "image_quality.csv"));
  std::filesystem::remove_all(dir);
  std::cout << "report_schema_test passed\n";
}
