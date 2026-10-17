#pragma once
#include "pcnet/core/error_model.hpp"
#include <filesystem>
#include <string>
#include <vector>

namespace pcnet {
enum class ExitCode { Success = 0, Warning = 10, ConfigError = 20, TimingUnavailable = 30, IoError = 40, InternalError = 50 };
struct CliConfig { ErrorModel model = ErrorModel::Constant; std::uint32_t time_bins = 8; bool allow_constraint_violations = false; std::uint64_t max_control_points = 0; std::uint64_t max_measures = 0; };
CliConfig load_cli_config(const std::filesystem::path &);
ExitCode run_cli(const std::vector<std::string> &args);
void atomic_write_text(const std::filesystem::path &, const std::string &);
}
