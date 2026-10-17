#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace {
int fail(const std::string &message) {
  std::cerr << "p0_probe_test: " << message << '\n';
  return 1;
}
}

int main(int argc, char **argv) {
  if (argc != 2) return fail("usage: p0_probe_test <report.json>");
  std::ifstream input(argv[1]);
  if (!input) return fail("report does not exist");
  std::ostringstream buffer;
  buffer << input.rdbuf();
  const std::string report = buffer.str();
  for (const char *key : {"environment_path", "library_path", "controlnet", "timing", "failures"}) {
    if (report.find(std::string("\"") + key + "\"") == std::string::npos)
      return fail(std::string("missing report field: ") + key);
  }
  return 0;
}
