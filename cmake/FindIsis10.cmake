find_path(ISIS10_INCLUDE_DIR
  NAMES isis/ControlNet.h
  HINTS "${CMAKE_PREFIX_PATH}/include" "/home/gengxun/miniconda3/envs/asp370/include")
find_library(ISIS10_LIBRARY
  NAMES isis10.0.0 isis
  HINTS "${CMAKE_PREFIX_PATH}/lib" "/home/gengxun/miniconda3/envs/asp370/lib")
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Isis10 DEFAULT_MSG ISIS10_INCLUDE_DIR ISIS10_LIBRARY)
if(Isis10_FOUND AND NOT TARGET Isis10::isis)
  add_library(Isis10::isis UNKNOWN IMPORTED)
  set_target_properties(Isis10::isis PROPERTIES
    IMPORTED_LOCATION "${ISIS10_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${ISIS10_INCLUDE_DIR};${CMAKE_PREFIX_PATH}/include/qt6;${CMAKE_PREFIX_PATH}/include/qt6/QtCore;${CMAKE_PREFIX_PATH}/include/cspice;${CMAKE_PREFIX_PATH}/include/qt6/QtCore5Compat")
endif()
