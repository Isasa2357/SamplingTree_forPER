# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "C:/Work/UV_env/workstation_project/make_PER/2-CMake/build2/build/_deps/pybind11-src")
  file(MAKE_DIRECTORY "C:/Work/UV_env/workstation_project/make_PER/2-CMake/build2/build/_deps/pybind11-src")
endif()
file(MAKE_DIRECTORY
  "C:/Work/UV_env/workstation_project/make_PER/2-CMake/build2/build/_deps/pybind11-build"
  "C:/Work/UV_env/workstation_project/make_PER/2-CMake/build2/build/_deps/pybind11-subbuild/pybind11-populate-prefix"
  "C:/Work/UV_env/workstation_project/make_PER/2-CMake/build2/build/_deps/pybind11-subbuild/pybind11-populate-prefix/tmp"
  "C:/Work/UV_env/workstation_project/make_PER/2-CMake/build2/build/_deps/pybind11-subbuild/pybind11-populate-prefix/src/pybind11-populate-stamp"
  "C:/Work/UV_env/workstation_project/make_PER/2-CMake/build2/build/_deps/pybind11-subbuild/pybind11-populate-prefix/src"
  "C:/Work/UV_env/workstation_project/make_PER/2-CMake/build2/build/_deps/pybind11-subbuild/pybind11-populate-prefix/src/pybind11-populate-stamp"
)

set(configSubDirs Debug)
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Work/UV_env/workstation_project/make_PER/2-CMake/build2/build/_deps/pybind11-subbuild/pybind11-populate-prefix/src/pybind11-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Work/UV_env/workstation_project/make_PER/2-CMake/build2/build/_deps/pybind11-subbuild/pybind11-populate-prefix/src/pybind11-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
