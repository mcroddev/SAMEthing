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

#include <stdlib.h>

#include "samething/core.h"

int main(void) {
  struct samething_core_gen_ctx ctx = {};

  const struct samething_core_header header = {
      .location_codes = {"101010", "828282",
                         SAMETHING_CORE_LOCATION_CODE_END_MARKER},
      .callsign = "BENCH/TE ",
      .event_code = "EEE",
      .originator_code = "ORG",
      .originator_time = "89238993",
      .valid_time_period = "1234",
      .attn_sig_duration = 25};

  samething_core_ctx_init(&ctx, &header);

  while (ctx.seq_state != SAMETHING_CORE_SEQ_STATE_NUM) {
    samething_core_samples_gen(&ctx);
  }
  return EXIT_SUCCESS;
}
