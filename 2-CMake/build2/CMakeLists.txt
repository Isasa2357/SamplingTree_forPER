cmake_minimum_required(VERSION 3.14)
project(sampling_tree_bindings LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

# ここから追加
# ───────────────────────────────
# FindPythonInterp/Libs モジュールを無効にして新モジュールを使う
if(POLICY CMP0148)
  cmake_policy(SET CMP0148 NEW)
endif()

set(Python3_FIND_STRATEGY LOCATION)
set(Python3_ROOT_DIR "C:/Users/iwata/AppData/Local/Programs/Python/Python310")
set(Python3_EXECUTABLE "${Python3_ROOT_DIR}/python.exe")
# ───────────────────────────────
# ここまで追加

include(FetchContent)
FetchContent_Declare(
  pybind11
  GIT_REPOSITORY https://github.com/pybind/pybind11.git
  GIT_TAG        v2.11.1
)
FetchContent_MakeAvailable(pybind11)

find_package(Python3 3.10 COMPONENTS Interpreter Development REQUIRED)

pybind11_add_module(sampling_tree
    py_sampling_tree.cpp
)
set_target_properties(sampling_tree PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
