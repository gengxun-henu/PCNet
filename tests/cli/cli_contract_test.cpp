#include "pcnet/cli/config.hpp"
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>

int main() {
  const auto config_path = std::filesystem::temp_directory_path() / "pcnet-cli.yaml";
  std::ofstream(config_path) << "error_model: velocity\ntime_bins: 8\n";
  const auto config = pcnet::load_cli_config(config_path);
  assert(config.model == pcnet::ErrorModel::Velocity);
  assert(config.time_bins == 8);
  assert(pcnet::run_cli({"pcnet", "inspect", "--config", config_path.string()}) == pcnet::ExitCode::Success);
  assert(pcnet::run_cli({"pcnet", "unknown"}) == pcnet::ExitCode::ConfigError);
  std::ofstream(config_path) << "time_bins: 3\n";
  assert(pcnet::run_cli({"pcnet", "inspect", "--config", config_path.string()}) == pcnet::ExitCode::ConfigError);
  const auto output = std::filesystem::temp_directory_path() / "pcnet-atomic.json";
  pcnet::atomic_write_text(output, "{\"status\":\"ok\"}\n");
  std::ifstream in(output); std::string text((std::istreambuf_iterator<char>(in)), {}); assert(text.find("status") != std::string::npos);
  std::filesystem::remove(config_path); std::filesystem::remove(output);
  std::cout << "cli_contract_test passed\n";
}
