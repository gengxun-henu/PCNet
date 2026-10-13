cmake_minimum_required(VERSION 3.20)
project(PCNet LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)
enable_testing()

add_executable(pcnet_temporal_support_test
  tests/temporal_support_test.cpp
)
target_include_directories(pcnet_temporal_support_test PRIVATE include)
target_compile_options(pcnet_temporal_support_test PRIVATE -Wall -Wextra -Wpedantic)
add_test(NAME temporal_support COMMAND pcnet_temporal_support_test)

add_executable(pcnet_error_model_test
  tests/error_model_test.cpp
)
target_include_directories(pcnet_error_model_test PRIVATE include)
target_compile_options(pcnet_error_model_test PRIVATE -Wall -Wextra -Wpedantic)
add_test(NAME error_model COMMAND pcnet_error_model_test)

add_executable(pcnet_observation_timing_test
  tests/observation_timing_test.cpp
)
target_include_directories(pcnet_observation_timing_test PRIVATE include)
target_compile_options(pcnet_observation_timing_test PRIVATE -Wall -Wextra -Wpedantic)
add_test(NAME observation_timing COMMAND pcnet_observation_timing_test)
