#!/bin/sh
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

if [ ! -f "env_setup.sh" ]; then
  echo "Working directory is not correct; run this script in its directory!" >&2
  exit 1
fi

ASSETS_DIR="$(cd "$(dirname "assets")" || exit; pwd)/$(basename "assets")"

case $- in
  *i*) : ;;
    *)
cat >&2 << EOF
This script configures the current shell session to use software from the
toolchain in the current shell session, which overrides this session's
existing environment.

For example, when executing "clang", it will execute the clang executable
contained in the toolchain.

As such, this script is not meant to be run interactively.

Execute the following command instead: source env_setup.sh
EOF
  exit 1
esac

alias llvm-config='$ASSETS_DIR/llvm/bin/llvm-config'
alias git-clang-format='$ASSETS_DIR/llvm/bin/git-clang-format'
alias clang-format='$ASSETS_DIR/llvm/bin/clang-format'
alias clangd='$ASSETS_DIR/llvm/bin/clangd'
alias dsymutil='$ASSETS_DIR/llvm/bin/dsymutil'
alias llvm-objdump='$ASSETS_DIR/llvm/bin/llvm-objdump'
alias clang='$ASSETS_DIR/llvm/bin/clang-16'
alias clang++='$ASSETS_DIR/llvm/bin/clang++'
alias llvm-ar='$ASSETS_DIR/llvm/bin/llvm-ar'
alias llvm-mca='$ASSETS_DIR/llvm/bin/llvm-mca'
alias llvm-objcopy='$ASSETS_DIR/llvm/bin/llvm-objcopy'
alias lldb='$ASSETS_DIR/llvm/bin/lldb'
alias llvm-cov='$ASSETS_DIR/llvm/bin/llvm-cov'
alias llvm-nm='$ASSETS_DIR/llvm/bin/llvm-nm'
alias llvm-dwarfdump='$ASSETS_DIR/llvm/bin/llvm-dwarfdump'
alias clang-tidy='$ASSETS_DIR/llvm/bin/clang-tidy'
alias llvm-strings='$ASSETS_DIR/llvm/bin/llvm-strings'
alias llvm-cxxfilt='$ASSETS_DIR/llvm/bin/llvm-cxxfilt'
alias llvm-profdata='$ASSETS_DIR/llvm/bin/llvm-profdata'
alias llvm-size='$ASSETS_DIR/llvm/bin/llvm-size'
alias llvm-strip='$ASSETS_DIR/llvm/bin/llvm-strip'
alias clangd='$ASSETS_DIR/llvm/bin/clangd'
alias cmake='$ASSETS_DIR/cmake/bin/cmake'
alias cpack='$ASSETS_DIR/cmake/bin/cpack'
alias ctest='$ASSETS_DIR/cmake/bin/ctest'
alias ccmake='$ASSETS_DIR/cmake/bin/ccmake'
alias ninja='$ASSETS_DIR/ninja/bin/ninja'

echo "This shell session is now configured for use with samething."
