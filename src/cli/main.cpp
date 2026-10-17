#include "pcnet/cli/config.hpp"
#include <iostream>
#include <string>
#include <vector>
int main(int argc, char **argv) { std::vector<std::string> args; for (int i = 0; i < argc; ++i) args.emplace_back(argv[i]); const auto code = pcnet::run_cli(args); return static_cast<int>(code); }
