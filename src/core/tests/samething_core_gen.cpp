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

#include "core/core.h"
#include "gtest/gtest.h"

namespace {};

TEST(samething_core_gen, HandlesOriginatorCode) {
#if 0
  const struct samething_core_header header = {
      .location_codes = {"010101", SAMETHING_CORE_LOCATION_CODE_END_MARKER},
      .valid_time_period = "1000",
      .originator_code = "EAS",
      .event_code = "RMT",
      .id = "WABC/FM",
      .originator_time = "0231233",
      .attn_sig_duration = 8.0F};

  std::size_t num_samples = 0;
  auto samples = (float *)malloc(sizeof(float) * 3'000'000);

  samething_core_gen(&header, samples, &num_samples);
#endif
}
