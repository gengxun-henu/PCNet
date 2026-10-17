#pragma once
#include "pcnet/core/network_view.hpp"
#include <filesystem>

namespace pcnet {
enum class WriteMode { PreserveDisabled, Compact };
class ControlNetAdapter {
public:
  static NetworkView read(const std::filesystem::path &path);
  static void write(const std::filesystem::path &path, const NetworkView &, WriteMode mode, const std::filesystem::path &audit_sidecar);
};
}
