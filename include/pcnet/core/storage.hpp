#pragma once
#include "pcnet/core/network_view.hpp"
#include <filesystem>

namespace pcnet {
struct StorageOptions { bool include_quality = false; };
class StorageWriter {
public: static void write(const std::filesystem::path &, const NetworkView &, StorageOptions = {});
};
class StorageReader {
public: static NetworkView read(const std::filesystem::path &);
};
}
