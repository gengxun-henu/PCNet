cmake_minimum_required(VERSION 3.20)
project(PCNet LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
enable_testing()
add_executable(pcnet_p0_probe tools/p0_probe.cpp)
target_compile_features(pcnet_p0_probe PRIVATE cxx_std_17)
find_package(Qt6 REQUIRED COMPONENTS Core Core5Compat)
find_package(Isis10 REQUIRED)
target_link_libraries(pcnet_p0_probe PRIVATE Isis10::isis Qt6::Core Qt6::Core5Compat)
add_executable(p0_probe_test tests/p0_probe_test.cpp)
add_test(NAME p0_probe COMMAND p0_probe_test "${CMAKE_CURRENT_BINARY_DIR}/p0_probe.json")
add_custom_target(run_p0_probe
  COMMAND pcnet_p0_probe --env "${CMAKE_PREFIX_PATH}" --output "${CMAKE_CURRENT_BINARY_DIR}/p0_probe.json"
  DEPENDS pcnet_p0_probe
  USES_TERMINAL)

add_library(pcnet_core STATIC
  src/core/network_view.cpp
  src/core/storage.cpp)
target_include_directories(pcnet_core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_compile_features(pcnet_core PUBLIC cxx_std_17)
add_executable(network_view_test tests/core/network_view_test.cpp)
target_link_libraries(network_view_test PRIVATE pcnet_core)
add_test(NAME network_view COMMAND network_view_test)
add_executable(storage_test tests/core/storage_test.cpp)
target_link_libraries(storage_test PRIVATE pcnet_core)
add_test(NAME storage COMMAND storage_test)
