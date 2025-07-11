cmake_minimum_required(VERSION 3.14)
project(sampling_tree_bindings LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

# --- Ç±Ç±Ç©ÇÁí«â¡ ---
include(FetchContent)
FetchContent_Declare(
  pybind11
  GIT_REPOSITORY https://github.com/pybind/pybind11.git
  GIT_TAG        v2.11.1
)


FetchContent_MakeAvailable(pybind11)
# --- Ç±Ç±Ç‹Ç≈í«â¡ ---

# Python3.10 à»è„ÇóvãÅ
find_package(Python3 3.10 COMPONENTS Interpreter Development REQUIRED)

pybind11_add_module(sampling_tree
    py_sampling_tree.cpp
)

set_target_properties(sampling_tree PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
