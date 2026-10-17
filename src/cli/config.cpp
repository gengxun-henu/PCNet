#include "pcnet/cli/config.hpp"
#include <fstream>
#include <stdexcept>
namespace pcnet {
namespace {
std::string trim(std::string value) { const auto first = value.find_first_not_of(" \t"); const auto last = value.find_last_not_of(" \t\r\n"); return first == std::string::npos ? std::string{} : value.substr(first, last - first + 1); }
ErrorModel parse_model(const std::string &value) { if (value == "constant") return ErrorModel::Constant; if (value == "velocity") return ErrorModel::Velocity; if (value == "acceleration") return ErrorModel::Acceleration; throw std::invalid_argument("error_model must be constant, velocity, or acceleration"); }
}
CliConfig load_cli_config(const std::filesystem::path &path) {
  std::ifstream input(path); if (!input) throw std::runtime_error("cannot open config"); CliConfig config; std::string line;
  while (std::getline(input, line)) { line = trim(line); if (line.empty() || line[0] == '#') continue; const auto colon = line.find(':'); if (colon == std::string::npos) throw std::invalid_argument("config line must contain ':'"); const auto key = trim(line.substr(0, colon)); const auto value = trim(line.substr(colon + 1)); if (key == "error_model") config.model = parse_model(value); else if (key == "time_bins") config.time_bins = static_cast<std::uint32_t>(std::stoul(value)); else if (key == "allow_constraint_violations") config.allow_constraint_violations = value == "true"; else throw std::invalid_argument("unknown config key: " + key); }
  if (!validate_time_bins(config.time_bins)) throw std::invalid_argument("time_bins must be between 4 and 32"); return config;
}
void atomic_write_text(const std::filesystem::path &path, const std::string &text) {
  const auto temporary = path.string() + ".tmp"; { std::ofstream output(temporary, std::ios::trunc); if (!output) throw std::runtime_error("cannot write temporary output"); output << text; }
  std::error_code ec; std::filesystem::rename(temporary, path, ec); if (ec) { std::filesystem::remove(path, ec); std::filesystem::rename(temporary, path, ec); } if (ec) throw std::runtime_error("cannot atomically replace output");
}
ExitCode run_cli(const std::vector<std::string> &args) {
  if (args.size() < 2) return ExitCode::ConfigError; const auto &command = args[1]; if (command != "inspect" && command != "quality" && command != "thin" && command != "filter" && command != "validate") return ExitCode::ConfigError;
  std::filesystem::path config_path; for (std::size_t i = 2; i + 1 < args.size(); ++i) if (args[i] == "--config") config_path = args[i + 1]; try { if (!config_path.empty()) (void)load_cli_config(config_path); else if (command != "inspect") return ExitCode::ConfigError; } catch (const std::invalid_argument &) { return ExitCode::ConfigError; } catch (const std::exception &) { return ExitCode::IoError; } return ExitCode::Success;
}
}
