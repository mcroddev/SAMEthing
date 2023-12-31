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

#include <cstring>

#include "gtest/gtest.h"
#include "samething/core.h"

#ifndef NDEBUG
extern "C" void *samething_dbg_userdata_ = nullptr;

extern "C" [[noreturn]] void samething_dbg_assert_failed(const char *const,
                                                         const char *const,
                                                         const int, void *) {
  std::abort();
}

TEST(samething_core_silence_gen, AssertsWhenContextIsNULL) {
  EXPECT_DEATH({ samething_core_silence_gen(nullptr, 0); }, ".*");
}
#endif  // NDEBUG

TEST(samething_core_silence_gen, GeneratesFullChunkOfSilence) {
  struct samething_core_gen_ctx ctx = {};

  // Fill the sample data with a constant to ensure that the data is not already
  // 0.
  std::memset(ctx.sample_data, 0xAB, sizeof(ctx.sample_data));

  // Essentially, this just zeroes out the chunk.
  for (size_t i = 0; i < SAMETHING_CORE_SAMPLES_NUM_MAX; ++i) {
    samething_core_silence_gen(&ctx, i);
  }

  // Check to see if the chunk is entirely 0.
  for (size_t i = 0; i < SAMETHING_CORE_SAMPLES_NUM_MAX; ++i) {
    EXPECT_EQ(ctx.sample_data[i], 0);
  }
}
