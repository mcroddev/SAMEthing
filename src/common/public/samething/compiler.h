// SPDX-License-Identifier: MIT
//
// Copyright 2023 Michael Rodriguez
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the “Software”), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

/** \file compiler.h
 * Defines the compiler abstraction facilities of SAMEthingCore.
 */

#ifndef SAMETHING_COMPILER_H
#define SAMETHING_COMPILER_H

#pragma once

#ifdef __clang__
/// Inform the compiler that anything past the current scope is unreachable.
#define SAMETHING_UNREACHABLE __builtin_unreachable()

/// Forcibly performs inline expansion of a function.
#define SAMETHING_ALWAYS_INLINE inline __attribute__((always_inline))
#else
/// Inform the compiler that anything past the current scope is unreachable.
#define SAMETHING_UNREACHABLE

/// Forcibly performs inline expansion of a function.
#define SAMETHING_ALWAYS_INLINE
#endif  // __clang__

#endif  // SAMETHING_COMPILER_H
