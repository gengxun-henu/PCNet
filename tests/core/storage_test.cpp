#include "pcnet/core/storage.hpp"
#include <cassert>
#include <filesystem>
#include <iostream>

int main() {
  pcnet::NetworkView input;
  input.images.push_back({0, "SN-1", "image.cub", 1024, 2048});
  input.points.push_back({0, 1, 0, 1});
  pcnet::TemporalSupport timing;
  timing.intervals.push_back({10.0, 11.0, 0.5});
  input.measures.push_back({0, 0, 12.5, 42.25, timing, 0.75, 1, pcnet::ReasonCode::UserDisabled});
  input.quality_columns = {"residual"};
  input.quality_values = {{3.25}};
  const auto path = std::filesystem::temp_directory_path() / "pcnet-storage-test.bin";
  pcnet::StorageOptions options;
  options.include_quality = true;
  pcnet::StorageWriter::write(path, input, options);
  auto output = pcnet::StorageReader::read(path);
  assert(output.validate().empty());
  assert(output.images[0].serial_number == "SN-1");
  assert(output.measures[0].timing.intervals[0].weight == 0.5);
  assert(output.measures[0].reason_code == pcnet::ReasonCode::UserDisabled);
  assert(output.quality_values[0][0] == 3.25);
  std::filesystem::remove(path);
  std::cout << "storage_test passed\n";
}
