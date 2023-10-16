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

/** \file debug.h
 * Defines the debugging facilities of SAMEthingCore.
 */

#ifndef SAMETHING_DEBUG_H
#define SAMETHING_DEBUG_H

#pragma once

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

#ifndef SAMETHING_TESTING
/// When unit tests are built, static functions become externally visible so
/// each one can also be directly tested. Functions which are meant to be
/// statically declared in a normal case should not use `static` directly, but
/// rather `SAMETHING_STATIC`.
#define SAMETHING_STATIC static
#else
#define SAMETHING_STATIC
#endif  // SAMETHING_TESTING

#ifndef NDEBUG
#define SAMETHING_ASSERT(expr)                               \
  do {                                                       \
    if (!(expr)) {                                           \
      samething_dbg_assert_failed(#expr, __FILE__, __LINE__, \
                                  samething_dbg_userdata_);  \
    }                                                        \
  } while (0)

/// This function is called when an assertion is hit.
///
/// Control should never be returned to the originator of an assertion.
///
/// @param expr The expression which failed.
/// @param file The file where the assertion occurred.
/// @param line_no The line number where the assertion was hit.
/// @param userdata Application specific user data, if any.
/// @note This function is meant to be implemented by the application developer,
///       where it will be resolved at link time. As such, a definition does not
///       exist in SAMEthingCore.
void samething_dbg_assert_failed(const char *const expr, const char *const file,
                                 const int line_no, void *userdata);

/// Application specific user data to pass to samething_dbg_assert_failed() if
/// an assertion occurs.
///
/// @note This variable is meant to be implemented by the application developer,
///       where it will be resolved at link time. As such, a definition does not
///       exist in SAMEthingCore.
extern void *samething_dbg_userdata_;

#else
#define SAMETHING_ASSERT(x)
#endif  // NDEBUG

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SAMETHING_DEBUG_H
