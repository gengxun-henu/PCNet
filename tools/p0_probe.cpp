#include <filesystem>
#if __has_include(<isis/ControlNet.h>)
#include <isis/ControlNet.h>
#define PCNET_HAS_CONTROLNET 1
#else
#define PCNET_HAS_CONTROLNET 0
#endif
#if __has_include(<isis/PushFrameCameraDetectorMap.h>)
#include <isis/PushFrameCameraDetectorMap.h>
#define PCNET_HAS_TIMING_HEADERS 1
#else
#define PCNET_HAS_TIMING_HEADERS 0
#endif
#include <fstream>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

static std::string json_escape(const std::string &value) {
  std::string result;
  for (char c : value) {
    if (c == '\\' || c == '"') result.push_back('\\');
    result.push_back(c);
  }
  return result;
}

int main(int argc, char **argv) {
  fs::path env;
  fs::path output;
  for (int i = 1; i + 1 < argc; i += 2) {
    const std::string option(argv[i]);
    if (option == "--env") env = argv[i + 1];
    else if (option == "--output") output = argv[i + 1];
  }
  if (env.empty() || output.empty()) {
    std::cerr << "usage: pcnet_p0_probe --env <environment> --output <report.json>\n";
    return 2;
  }
  const fs::path library = env / "lib" / "libisis10.0.0.so";
  const bool library_exists = fs::exists(library);
  std::ofstream report(output);
  if (!report) return 3;
  report << "{\n"
         << "  \"environment_path\": \"" << json_escape(env.string()) << "\",\n"
         << "  \"library_path\": \"" << json_escape(library.string()) << "\",\n"
         << "  \"compiler\": \"" << json_escape(__VERSION__) << "\",\n"
         << "  \"controlnet\": {\"header_available\": " << (PCNET_HAS_CONTROLNET ? "true" : "false")
         << ", \"library_present\": " << (library_exists ? "true" : "false") << "},\n"
         << "  \"timing\": {\"detector_map_headers_available\": " << (PCNET_HAS_TIMING_HEADERS ? "true" : "false") << "},\n"
         << "  \"failures\": [";
  if (!library_exists) report << "\"ISIS library not found at expected path\"";
  report << "]\n}\n";
  return 0;
}
