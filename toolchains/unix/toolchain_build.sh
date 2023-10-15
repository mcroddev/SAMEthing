#!/bin/sh
# SPDX-License-Identifier: MIT
#
# Copyright 2023 Michael Rodriguez
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the “Software”), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

# This script builds a toolchain suitable for development and deployment of
# samething. The resulting toolchain is usable on Unix(-like) compliant
# systems, far too many to enumerate here.

############################################################################
# When pushing a commit, the CI pipeline will check to see if any of these #
# variables have been changed. If so, the toolchain is rebuilt.            #
toolchain_ver="1.0.0.0"                                                    #
llvm_ver="17.0.2"                                                          #
gcc_ver="13.0.1"                                                           #
cmake_ver="3.27.1"                                                         #
ninja_ver="1.11.1"                                                         #
qt_ver="6.5.3"                                                             #
sdl_ver="2.28.3"                                                           #
supported_archs="AArch64 X86"                                              #
project_name="samething"                                                   #
tarball_name="${project_name}-toolchain-unix-v${toolchain_ver}.tar.gz"     #
############################################################################

# Directory where the build takes place.
staging_dir="staging"

# Directory where the toolchain is stored.
assets_dir="assets"

# Enable link-time optimization (LTO) where appropriate.
use_lto=false

# Enable profile-guided optimization (PGO) where appropriate.
use_pgo=false

# Generate toolchain tarball.
tarball_gen=true

cflags=""
cxxflags=""
linker="ld.lld"

directory_create() {
  mkdir "$1" || exit
  echo "Created directory ${1}..."
}

tarball_extract() {
  tar xvf "$1" || exit
}

file_download() {
  curl -gLOs "$1" || exit
}

usage() {
  cat << EOF
Usage: ./toolchain_build.sh [OPTIONS]

Builds a toolchain suitable for development and deployment of ${project_name}.

Optional arguments:
  --use-lto           Compile the toolchain with link-time optimization where
                      appropriate. The toolchain will be faster, but the build
                      time will be much slower and much more intensive on the
                      host. Default is off.

  --use-pgo           Compile the toolchain with profile-guided optimization
                      where appropriate. The toolchain will be faster, but the
                      build time will be much slower and much more intensive on
                      the host. Default is off.

  --optimize-for-sys  Compile the toolchain with optimizations specific to this
                      system. The toolchain will be slightly faster, but the
                      resulting toolchain will not be portable to other systems.
                      Default is off.

  --no-tarball        Disables generation of toolchain tarball.
EOF
  exit "$1"
}

command_line_arguments_handle() {
  # https://mywiki.wooledge.org/BashFAQ/035
  while :; do
    case $1 in
      -\?|--help)
        usage 0
        ;;

      --use-lto)
        use_lto=true
        ;;

      --use-pgo)
        use_pgo=true
        ;;

      --optimize-for-sys)
        cflags='-mtune=native -march=native'
        cxxflags='-mtune=native -march=native'
        ;;

      --no-tarball)
        tarball_gen=false
        ;;

      --)
        shift
        break
        ;;

      -?*)
        echo "Ignoring unknown option: ${1}" >&2
        ;;

      *)
        break
    esac
    shift
  done
}

llvm_build() {
  echo "Downloading LLVM ${llvm_ver}..."
  file_download "https://github.com/llvm/llvm-project/releases/download/llvmorg-${llvm_ver}/llvm-project-${llvm_ver}.src.tar.xz"

  echo "Extracting LLVM ${llvm_ver}..."
  tarball_extract "llvm-project-${llvm_ver}.src.tar.xz"

  cp ../cmake/llvm-stage1.cmake llvm-project-${llvm_ver}.src/clang/cmake/caches || exit
  cp ../cmake/llvm-stage2.cmake llvm-project-${llvm_ver}.src/clang/cmake/caches || exit
  cd llvm-project-${llvm_ver}.src || exit

  set -- "-DCMAKE_INSTALL_PREFIX:STRING=${assets_dir}/llvm"
  if [ "$use_lto" = true ]; then
    set -- "$@" "-DSAMETHING_TOOLCHAIN_ENABLE_LTO:BOOL=ON"
  fi

  LDFLAGS="-fuse-ld=lld" \
  CFLAGS=${cflags} \
  CXXFLAGS=${cxxflags} \
  CC="clang" \
  CXX="clang++" \
  CMAKE_BUILD_TYPE="Release" \
  cmake -S llvm -B build -G Ninja -C clang/cmake/caches/llvm-stage1.cmake "$@" \
  || exit

  cd build || exit

  echo "Building LLVM ${llvm_ver}, this will take a long time..."
  ninja -v stage2-distribution || exit

  echo "Installing LLVM ${llvm_ver}..."
  ninja -v stage2-install-distribution-stripped || exit

  echo "Installation of LLVM ${llvm_ver} complete."
  cd "${staging_dir}" || exit
}

cmake_build() {
  echo "Downloading CMake v${cmake_ver}..."
  file_download "https://github.com/Kitware/CMake/releases/download/v${cmake_ver}/cmake-${cmake_ver}.tar.gz"

  echo "Extracting CMake v${cmake_ver}..."
  tarball_extract "cmake-${cmake_ver}.tar.gz"

  cd "cmake-${cmake_ver}" || exit

  echo "Configuring CMake v${cmake_ver}, please wait..."

  set -- "-DCMAKE_INSTALL_PREFIX:STRING="${assets_dir}/cmake""
  if [ "$use_lto" = true ]; then
    set -- "$@" "-DCMake_BUILD_LTO:BOOL=ON"
  fi

  LDFLAGS="-fuse-ld=lld --ld-path=${assets_dir}/llvm/bin/${linker}" \
  CC="${assets_dir}/llvm/bin/clang" \
  CXX="${assets_dir}/llvm/bin/clang++" \
  CFLAGS=${cflags} \
  CXXFLAGS=${cxxflags} \
  CMAKE_BUILD_TYPE="Release" \
  cmake -S . -B build -G Ninja "$@" || exit

  echo "Building and installing CMake v${cmake_ver}, this may take a while."
  cd build || exit
  ninja -v install || exit
  echo "Installation of CMake v${cmake_ver} complete."

  cd "${staging_dir}" || exit
}

ninja_build() {
  echo "Downloading ninja v${ninja_ver}..."
  file_download "https://github.com/ninja-build/ninja/archive/refs/tags/v${ninja_ver}.tar.gz"

  echo "Extracting ninja v${ninja_ver}..."
  tarball_extract "v${ninja_ver}.tar.gz"

  cd "ninja-${ninja_ver}" || exit

  echo "Configuring ninja v${ninja_ver}, please wait..."

  # LTO is automatically enabled with a build type of Release. The overhead for
  # an LTO build of Ninja is quite low, so the `--use-lto` flag is a no-op here.
  LDFLAGS="-fuse-ld=lld --ld-path=${assets_dir}/llvm/bin/${linker}" \
  CC="${assets_dir}/llvm/bin/clang" \
  CXX="${assets_dir}/llvm/bin/clang++" \
  CFLAGS=${cflags} \
  CXXFLAGS=${cxxflags} \
  CMAKE_BUILD_TYPE="Release" \
  ${assets_dir}/cmake/bin/cmake -S . -B build -G Ninja \
  -DCMAKE_INSTALL_PREFIX:STRING="${assets_dir}/ninja" || exit

  echo "Building and installing ninja v${ninja_ver}, this may take a while."
  cd build || exit
  ninja -v install || exit
  echo "Installation of ninja v${ninja_ver} complete."

  cd "${staging_dir}" || exit
}

qt_build() {
  echo "Downloading Qt ${qt_ver}..."
  shortened_qt_ver=$(echo ${qt_ver} | cut -c 1-3)
  file_download "https://download.qt.io/archive/qt/${shortened_qt_ver}/${qt_ver}/single/qt-everywhere-src-${qt_ver}.tar.xz"

  echo "Extracting Qt ${qt_ver}..."
  tarball_extract "qt-everywhere-src-${qt_ver}.tar.xz"

  echo "Stubbed."
  cd "${staging_dir}" || exit
}

sdl_build() {
  echo "Downloading SDL ${sdl_ver}..."
  file_download "https://github.com/libsdl-org/SDL/releases/download/release-${sdl_ver}/SDL2-${sdl_ver}.tar.gz"

  echo "Extracting SDL ${sdl_ver}..."
  tarball_extract "SDL2-${sdl_ver}.tar.gz"

  cd "SDL2-${sdl_ver}" || exit

  echo "Configuring SDL2-${sdl_ver} release version, please wait..."
  LDFLAGS="-fuse-ld=lld --ld-path=${assets_dir}/llvm/bin/${linker}" \
  CC="${assets_dir}/llvm/bin/clang" \
  CXX="${assets_dir}/llvm/bin/clang++" \
  CFLAGS=${cflags} \
  CXXFLAGS=${cxxflags} \
  CMAKE_BUILD_TYPE="Release" \
  ${assets_dir}/cmake/bin/cmake -S . -B release_build -G Ninja \
  -DCMAKE_INSTALL_PREFIX:STRING="${assets_dir}/sdl-release" \
  -DSDL_STATIC:BOOL=ON \
  -DSDL_SHARED:BOOL=OFF \
  -DSDL_TEST:BOOL=OFF \
  -DSDL_ATOMIC:BOOL=OFF \
  -DSDL_AUDIO:BOOL=ON \
  -DSDL_VIDEO:BOOL=OFF \
  -DSDL_RENDER:BOOL=OFF \
  -DSDL_EVENTS:BOOL=ON \
  -DSDL_JOYSTICK:BOOL=OFF \
  -DSDL_HAPTIC:BOOL=OFF \
  -DSDL_HIDAPI:BOOL=OFF \
  -DSDL_POWER:BOOL=OFF \
  -DSDL_TIMERS:BOOL=OFF \
  -DSDL_FILE:BOOL=OFF \
  -DSDL_CPUINFO:BOOL=OFF \
  -DSDL_FILESYSTEM:BOOL=OFF \
  -DSDL_SENSOR:BOOL=OFF \
  -DSDL_LOCALE:BOOL=OFF \
  -DSDL_MISC:BOOL=OFF || exit

  echo "Building and installing release version of SDL2-${sdl_ver}, this may " \
  "take a while."

  cd release_build || exit
  ninja -v install || exit
  echo "Installation of release version of SDL2-${sdl_ver} complete."

  cd .. || exit

  echo "Configuring SDL2-${sdl_ver} debug version, please wait..."
  LDFLAGS="-fuse-ld=lld --ld-path=${assets_dir}/llvm/bin/${linker}" \
  CC="${assets_dir}/llvm/bin/clang" \
  CXX="${assets_dir}/llvm/bin/clang++" \
  CFLAGS=${cflags} \
  CXXFLAGS=${cxxflags} \
  CMAKE_BUILD_TYPE="Debug" \
  ${assets_dir}/cmake/bin/cmake -S . -B debug_build -G Ninja \
  -DCMAKE_INSTALL_PREFIX:STRING="${assets_dir}/sdl-debug" \
  -DSDL_STATIC:BOOL=ON \
  -DSDL_SHARED:BOOL=OFF \
  -DSDL_TEST:BOOL=OFF \
  -DSDL_ATOMIC:BOOL=OFF \
  -DSDL_AUDIO:BOOL=ON \
  -DSDL_VIDEO:BOOL=OFF \
  -DSDL_RENDER:BOOL=OFF \
  -DSDL_EVENTS:BOOL=ON \
  -DSDL_JOYSTICK:BOOL=OFF \
  -DSDL_HAPTIC:BOOL=OFF \
  -DSDL_HIDAPI:BOOL=OFF \
  -DSDL_POWER:BOOL=OFF \
  -DSDL_TIMERS:BOOL=OFF \
  -DSDL_FILE:BOOL=OFF \
  -DSDL_CPUINFO:BOOL=OFF \
  -DSDL_FILESYSTEM:BOOL=OFF \
  -DSDL_SENSOR:BOOL=OFF \
  -DSDL_LOCALE:BOOL=OFF \
  -DSDL_MISC:BOOL=OFF || exit

  echo "Building and installing debug version of SDL2-${sdl_ver}, please wait..."
  cd debug_build || exit
  ninja -v install || exit
  echo "Installation of debug version of SDL2-${sdl_ver} complete."

  cd "${assets_dir}" || exit

  cp sdl-debug/lib/libSDL2d.a sdl-release/lib || exit
  cp sdl-debug/lib/libSDL2maind.a sdl-release/lib || exit
  mv sdl-release sdl || exit
  rm -rf sdl-debug || exit

  cd "${staging_dir}" || exit
}

check_required() {
  all_commands_found=true

  for required in cmake ninja gcc clang lld re2c; do
    if ! command -v "$required" >/dev/null; then
      echo "Required command ${required} not found."
      all_commands_found=false
    fi
  done

  if [ "${all_commands_found}" = false ]; then
    exit 1
  fi
}

tarball_create() {
  cd "${staging_dir}"/.. || exit
  echo "Creating toolchain deployment tarball..."
  tar czf "${tarball_name}" "assets" || exit
}

if [ ! -f "toolchain_build.sh" ]; then
  echo "Working directory is not correct; run this script in its directory!" >&2
  exit 1
fi

command_line_arguments_handle "$@"
check_required

staging_dir=$(cd "$(dirname "${staging_dir}")" || exit; pwd)/$(basename "${staging_dir}")
assets_dir=$(cd "$(dirname "${assets_dir}")" || exit; pwd)/$(basename "${assets_dir}")

directory_create "${staging_dir}"
directory_create "${assets_dir}"
cd "${staging_dir}" || exit

# First, compile the compilers.
llvm_build

# Using the generated LLVM compiler, compile the rest of the software for the
# host.
cmake_build
ninja_build

# Now compile dependencies for targets using the generated LLVM compiler.
qt_build
sdl_build

echo "Removing staging directory..."
rm -rf "${staging_dir}" || exit

if [ "${tarball_gen}" = true ]; then
  tarball_create
fi

echo "Toolchain build complete."
