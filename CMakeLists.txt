# SPDX-License-Identifier: MIT
#
# Copyright 2023 Michael Rodriguez
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the “Software”),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
# DEALINGS IN THE SOFTWARE.

# Although each toolchain ships with CMake 3.27.7, some IDEs will fail when
# opening the project. This is often times because the built-in CMake version
# isn't >=3.27.7. Even using a preset, some IDEs do not respect the
# "cmakeExecutable" field in configure presets. The easiest solution is to
# require the most minimal CMake version that satisfies all IDEs, although the
# better solution is for IDEs to support this field.
#
# The FATAL_ERROR option is accepted but ignored by CMake 2.6 and higher. It
# should be specified so CMake versions 2.4 and lower fail with an error instead
# of just a warning.
cmake_minimum_required(VERSION 3.26.4 FATAL_ERROR)

project(SAMEthing
        VERSION 1.0.0.0
        DESCRIPTION "Specific Area Message Encoding (SAME) header generator"
        HOMEPAGE_URL "https://github.com/mcroddev/samething"
        LANGUAGES C CXX)

# Toolchain files are required so that CMake knows where to look for our custom
# toolchain.
if (NOT CMAKE_TOOLCHAIN_FILE)
  message(FATAL_ERROR
          "An appropriate toolchain file is required to be specified. Use one "
          "of the presets to do this for you.")
endif ()

# Generate a "compile_commands.json" file containing the exact compiler calls
# for all translation units of a project in machine-readable form. This is
# required for certain IDEs to provide code completion, error checking, etc.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Configures the project wide build settings for C targets.
function(samething_build_settings_c_configure)
  # When targets link to this library, the settings set here will be passed to
  # the target.
  add_library(samething-build-settings-c INTERFACE)

  # Enforce the use of the C17 standard with extensions enabled.
  target_compile_features(samething-build-settings-c INTERFACE
                          c_std_17)

  if (SAMETHING_BUILD_TESTS)
    target_compile_definitions(samething-build-settings-c INTERFACE
                               SAMETHING_TESTING)
  endif()

  # These flags are set regardless of the compiler, since we only support Clang
  # and gcc anyway.
  target_compile_options(samething-build-settings-c INTERFACE
                         -fdata-sections
                         -ffunction-sections
                         -fstack-usage
                         -Rpass-analysis=loop-vectorize
                         -Rpass=loop-vectorize
                         -Rpass-missed=loop-vectorize
                         -Wall
                         -Walloca
                         -Wcast-align
                         -Wcast-qual
                         -Wconversion
                         -Wdisabled-optimization
                         -Wdouble-promotion
                         -Werror
                         -Wextra
                         -Wformat=2
                         -Winline
                         -Wmissing-include-dirs
                         -Wmissing-noreturn
                         -Wno-error=disabled-optimization
                         -Wno-error=inline
                         -Wno-error=padded
                         -Wno-error=unused
                         -Wnull-dereference
                         -Wpadded
                         -Wpointer-arith
                         -Wshadow
                         -Wswitch-default
                         -Wundef
                         -Wvla
                         -Wwrite-strings)

  if (CMAKE_C_COMPILER_ID STREQUAL "Clang")
    target_link_options(samething-build-settings-c INTERFACE
                        -fuse-ld=lld -Wl,--Map=SAMEthing.map -Wl,--gc-sections)

    if (SAMETHING_ENABLE_SANITIZERS)
      target_compile_options(samething-build-settings-c INTERFACE
                             -fno-omit-frame-pointer
                             -fno-optimize-sibling-calls)

      target_compile_options(samething-build-settings-c INTERFACE
                             -fsanitize=address,undefined)
      target_link_options(samething-build-settings-c INTERFACE
                          -fsanitize=address,undefined)
    endif()

    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
      target_compile_options(samething-build-settings-c INTERFACE -glldb -O1)
    elseif (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
      target_compile_options(samething-build-settings-c INTERFACE -Ofast)

      if (SAMETHING_BUILD_BENCHMARKS)
        target_compile_options(samething-build-settings-c INTERFACE
                               -fno-omit-frame-pointer)
      endif()
    else()
      message(FATAL_ERROR
              "Unable to set project wide build settings for C targets: the "
              "build type ${CMAKE_BUILD_TYPE} is unhandled!")
    endif()
  else()
    message(FATAL_ERROR
            "Unable to set project wide build settings for C targets: the C "
            "compiler ${CMAKE_C_COMPILER_ID} is unhandled!")
  endif()
endfunction()

# Configures the project wide build settings for C++ targets.
function(samething_build_settings_cpp_configure)
  # When targets link to this library, the settings set here will be passed to
  # the target.
  add_library(samething-build-settings-cpp INTERFACE)

  target_compile_features(samething-build-settings-cpp INTERFACE
                          cxx_std_17)

  if (SAMETHING_BUILD_TESTS)
    target_compile_definitions(samething-build-settings-cpp INTERFACE
                               SAMETHING_TESTING)
  endif()

  # These flags are set regardless of the compiler, since we only support Clang
  # and gcc anyway.
  target_compile_options(samething-build-settings-cpp INTERFACE
                         -fdata-sections
                         -ffunction-sections
                         -fstack-usage
                         -Rpass-analysis=loop-vectorize
                         -Rpass=loop-vectorize
                         -Rpass-missed=loop-vectorize
                         -Wall
                         -Walloca
                         -Wcast-align
                         -Wcast-qual
                         -Wconversion
                         -Wdisabled-optimization
                         -Wdouble-promotion
                         -Werror
                         -Wextra
                         -Wformat=2
                         -Winline
                         -Wmissing-include-dirs
                         -Wmissing-noreturn
                         -Wno-error=disabled-optimization
                         -Wno-error=inline
                         -Wno-error=padded
                         -Wno-error=unused
                         -Wnull-dereference
                         -Wpadded
                         -Wpointer-arith
                         -Wshadow
                         -Wswitch-default
                         -Wundef
                         -Wvla
                         -Wwrite-strings)

  if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if (SAMETHING_ENABLE_SANITIZERS)
      target_compile_options(samething-build-settings-cpp INTERFACE
                             -fno-omit-frame-pointer
                             -fno-optimize-sibling-calls)

      target_compile_options(samething-build-settings-cpp INTERFACE
                             -fsanitize=address,undefined)
      target_link_options(samething-build-settings-cpp INTERFACE
                          -fsanitize=address,undefined)
    endif()

    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
      target_compile_options(samething-build-settings-cpp INTERFACE -glldb -O1)
    elseif (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
      target_compile_options(samething-build-settings-cpp INTERFACE -Ofast)

      if (SAMETHING_BUILD_BENCHMARKS)
        target_compile_options(samething-build-settings-cpp INTERFACE
                               -fno-omit-frame-pointer)
      endif()
    else()
      message(FATAL_ERROR
              "Unable to set project wide build settings for C++ targets: the "
              "build type ${CMAKE_BUILD_TYPE} is unhandled!")
    endif()
  else()
    message(FATAL_ERROR
            "Unable to set project wide build settings for C++ targets: the C++ "
            "compiler ${CMAKE_CXX_COMPILER_ID} is unhandled!")
  endif()
endfunction()

function(samething_options_handle)
  option(SAMETHING_BUILD_TESTS "Build the unit tests (enables sanitizers)" OFF)
  option(SAMETHING_BUILD_BENCHMARKS "Build the benchmarks" OFF)

  option(SAMETHING_ENABLE_LTO
         "Enable link-time optimization (only applies to Release builds)" OFF)

  option(SAMETHING_ENABLE_SANITIZERS
         "Enables the address sanitizer and the undefined behavior sanitizer"
         OFF)

  option(SAMETHING_BUILD_QT_FRONTEND "Build the Qt frontend" OFF)
endfunction()

function(samething_tests_enable)
  include(FetchContent)

  # XXX: Due to the fact googletest adheres to the Abseil Live at Head
  # philosophy, this is the recommended way to pull in googletest according to
  # https://google.github.io/googletest/quickstart-cmake.html.
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG main
  )

  # For Windows: Prevent overriding the parent project's compiler/linker
  # settings.
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

  FetchContent_MakeAvailable(googletest)

  include(GoogleTest)
  include(cmake/test_helpers.cmake)

  enable_testing()
endfunction()

samething_build_settings_c_configure()
samething_build_settings_cpp_configure()

samething_options_handle()

if (SAMETHING_ENABLE_LTO)
  set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO TRUE)
endif()

if (SAMETHING_BUILD_TESTS)
  samething_tests_enable()
endif ()

# Continue with the build process.
add_subdirectory(src)
