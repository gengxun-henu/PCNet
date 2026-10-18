#include "pcnet/cli/config.hpp"
#include "pcnet/core/storage.hpp"
#include "pcnet/metrics/quality.hpp"
#include "pcnet/thinning/thinner.hpp"
#include "pcnet/filtering/outlier_filter.hpp"
#include "pcnet/adapters/isis/controlnet_adapter.hpp"
#include <fstream>
#include <stdexcept>
namespace pcnet {
namespace {
std::string trim(std::string value) { const auto first = value.find_first_not_of(" \t"); const auto last = value.find_last_not_of(" \t\r\n"); return first == std::string::npos ? std::string{} : value.substr(first, last - first + 1); }
ErrorModel parse_model(const std::string &value) { if (value == "constant") return ErrorModel::Constant; if (value == "velocity") return ErrorModel::Velocity; if (value == "acceleration") return ErrorModel::Acceleration; throw std::invalid_argument("error_model must be constant, velocity, or acceleration"); }
}
CliConfig load_cli_config(const std::filesystem::path &path) {
  std::ifstream input(path); if (!input) throw std::runtime_error("cannot open config"); CliConfig config; std::string line;
  while (std::getline(input, line)) { line = trim(line); if (line.empty() || line[0] == '#') continue; const auto colon = line.find(':'); if (colon == std::string::npos) throw std::invalid_argument("config line must contain ':'"); const auto key = trim(line.substr(0, colon)); const auto value = trim(line.substr(colon + 1)); if (key == "error_model") config.model = parse_model(value); else if (key == "time_bins") config.time_bins = static_cast<std::uint32_t>(std::stoul(value)); else if (key == "allow_constraint_violations") config.allow_constraint_violations = value == "true"; else if (key == "max_control_points") config.max_control_points = std::stoull(value); else if (key == "max_measures") config.max_measures = std::stoull(value); else throw std::invalid_argument("unknown config key: " + key); }
  if (!validate_time_bins(config.time_bins)) throw std::invalid_argument("time_bins must be between 4 and 32"); return config;
}
void atomic_write_text(const std::filesystem::path &path, const std::string &text) {
  const auto temporary = path.string() + ".tmp"; { std::ofstream output(temporary, std::ios::trunc); if (!output) throw std::runtime_error("cannot write temporary output"); output << text; }
  std::error_code ec; std::filesystem::rename(temporary, path, ec); if (ec) { std::filesystem::remove(path, ec); std::filesystem::rename(temporary, path, ec); } if (ec) throw std::runtime_error("cannot atomically replace output");
}
ExitCode run_cli(const std::vector<std::string> &args) {
  if (args.size() < 2) return ExitCode::ConfigError;
  const auto &command = args[1];
  if (command != "inspect" && command != "quality" && command != "thin" && command != "filter" && command != "validate") return ExitCode::ConfigError;
  std::filesystem::path config_path, input_path, output_path, report_path;
  for (std::size_t i = 2; i + 1 < args.size(); ++i) { if (args[i] == "--config") config_path = args[i + 1]; else if (args[i] == "--input") input_path = args[i + 1]; else if (args[i] == "--output") output_path = args[i + 1]; else if (args[i] == "--report") report_path = args[i + 1]; }
  try {
    CliConfig config = config_path.empty() ? CliConfig{} : load_cli_config(config_path);
    if (input_path.empty()) return ExitCode::ConfigError;
    const bool is_isis = input_path.extension() == ".net" || input_path.extension() == ".controlnet";
    auto view = is_isis ? ControlNetAdapter::read(input_path) : StorageReader::read(input_path);
    if (command == "inspect" || command == "validate") {
      if (auto error = view.validate(); !error.empty()) return ExitCode::ConfigError;
      if (!report_path.empty()) atomic_write_text(report_path, "{\"status\":\"ok\",\"control_points\":" + std::to_string(view.points.size()) + "}\n");
      return ExitCode::Success;
    }
    if (command == "quality") {
      QualityConfig quality_config; quality_config.model = config.model; quality_config.time_bins = config.time_bins;
      const auto report = evaluate_quality(view, quality_config); const auto directory = report_path.empty() || report_path.parent_path().empty() ? std::filesystem::path(".") : report_path.parent_path(); write_quality_csv_json(report, directory); return ExitCode::Success;
    }
    if (output_path.empty()) return ExitCode::ConfigError;
    if (command == "thin") { ThinConfig thin_config; thin_config.allow_constraint_violations = config.allow_constraint_violations; thin_config.max_control_points = config.max_control_points; thin_config.max_measures = config.max_measures; thin_config.model = config.model; const auto result = thin(view, thin_config); if (!result.feasible && !thin_config.allow_constraint_violations) return ExitCode::Warning; if (output_path.extension() == ".net" || output_path.extension() == ".controlnet") ControlNetAdapter::write(output_path, result.output, WriteMode::PreserveDisabled, output_path.string() + ".audit.json"); else StorageWriter::write(output_path, result.output, {}); return result.feasible ? ExitCode::Success : ExitCode::Warning; }
    if (command == "filter") { FilterEvidence evidence(view.measures.size()); const auto result = filter_outliers(view, evidence, {}); if (output_path.extension() == ".net" || output_path.extension() == ".controlnet") ControlNetAdapter::write(output_path, result.output, WriteMode::PreserveDisabled, output_path.string() + ".audit.json"); else StorageWriter::write(output_path, result.output, {}); if (!report_path.empty()) atomic_write_text(report_path, "{\"status\":\"warning\",\"reason\":\"no external filter evidence supplied\"}\n"); return result.unresolved.empty() ? ExitCode::Warning : ExitCode::Warning; }
  } catch (const std::invalid_argument &) { return ExitCode::ConfigError; } catch (const std::exception &) { return ExitCode::IoError; }
  return ExitCode::InternalError;
}
}
