#pragma once
#include "pcnet/core/network_view.hpp"
#include <filesystem>

namespace pcnet {
enum class WriteMode { PreserveDisabled, Compact };
struct TimingPolicy { bool allow_line_interpolation = false; };
class ControlNetAdapter {
public:
  static NetworkView read(const std::filesystem::path &path, TimingPolicy policy = {});
  static void write(const std::filesystem::path &path, const NetworkView &, WriteMode mode, const std::filesystem::path &audit_sidecar);
};
}
