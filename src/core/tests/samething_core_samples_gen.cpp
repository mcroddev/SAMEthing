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

#include "gtest/gtest.h"
#include "samething/core.h"

#ifndef NDEBUG
extern "C" void *samething_dbg_userdata_ = nullptr;

extern "C" [[noreturn]] void samething_dbg_assert_failed(const char *const,
                                                         const char *const,
                                                         const int, void *) {
  std::abort();
}

/// Checks to see if samething_core_samples_gen() asserts when the sequence
/// state specified is >=SAMETHING_CORE_SEQ_STATE_NUM.
TEST(samething_core_samples_gen, AssertsWhenSeqStateIsInvalid) {
  struct samething_core_gen_ctx ctx = {};
  ctx.seq_state = SAMETHING_CORE_SEQ_STATE_NUM;

  EXPECT_DEATH({ samething_core_samples_gen(&ctx); }, ".*");
}

/// Checks to see if samething_core_samples_gen() asserts when the generation
/// context specified is NULL.
TEST(samething_core_samples_gen, AssertsWhenContextIsNULL) {
  EXPECT_DEATH({ samething_core_samples_gen(nullptr); }, ".*");
}
#endif  // NDEBUG

class SeqStateTransitionTest : public ::testing::Test {
 protected:
  void SetUp() override { ctx = {}; }

  void VerifyTransition(
      const enum samething_core_seq_state start_state,
      const enum samething_core_seq_state expected_state) noexcept {
    ctx.seq_state = start_state;
    samething_core_ctx_init(&ctx, &header);

    const unsigned int num_samples_expected =
        ctx.seq_samples_remaining[start_state];

    unsigned int count = 0;

    while (count < num_samples_expected) {
      samething_core_samples_gen(&ctx);
      count += SAMETHING_CORE_SAMPLES_NUM_MAX;
    }
    EXPECT_EQ(ctx.seq_state, expected_state);
  }

 private:
  const struct samething_core_header header = {
      .location_codes = {"101010", "828282",
                         SAMETHING_CORE_LOCATION_CODE_END_MARKER},
      .valid_time_period = "2138",
      .originator_code = "ORG",
      .event_code = "RED",
      .callsign = "XIPHIAS ",
      .originator_time = "3939393",
      .attn_sig_duration = 8};

  struct samething_core_gen_ctx ctx;
};

TEST_F(SeqStateTransitionTest, FirstAFSKHeaderToFirstSilence) {
  VerifyTransition(SAMETHING_CORE_SEQ_STATE_AFSK_HEADER_FIRST,
                   SAMETHING_CORE_SEQ_STATE_SILENCE_FIRST);
}

TEST_F(SeqStateTransitionTest, SecondAFSKHeaderToSecondSilence) {
  VerifyTransition(SAMETHING_CORE_SEQ_STATE_AFSK_HEADER_SECOND,
                   SAMETHING_CORE_SEQ_STATE_SILENCE_SECOND);
}

TEST_F(SeqStateTransitionTest, ThirdAFSKHeaderToThirdSilence) {
  VerifyTransition(SAMETHING_CORE_SEQ_STATE_AFSK_HEADER_THIRD,
                   SAMETHING_CORE_SEQ_STATE_SILENCE_THIRD);
}

TEST_F(SeqStateTransitionTest, AttentionSignalToFourthSilence) {
  VerifyTransition(SAMETHING_CORE_SEQ_STATE_ATTENTION_SIGNAL,
                   SAMETHING_CORE_SEQ_STATE_SILENCE_FOURTH);
}

TEST_F(SeqStateTransitionTest, FirstEOMHeaderToFifthSilence) {
  VerifyTransition(SAMETHING_CORE_SEQ_STATE_AFSK_EOM_FIRST,
                   SAMETHING_CORE_SEQ_STATE_SILENCE_FIFTH);
}

TEST_F(SeqStateTransitionTest, SecondEOMHeaderToSixthSilence) {
  VerifyTransition(SAMETHING_CORE_SEQ_STATE_AFSK_EOM_SECOND,
                   SAMETHING_CORE_SEQ_STATE_SILENCE_SIXTH);
}

TEST_F(SeqStateTransitionTest, ThirdEOMHeaderToSeventhSilence) {
  VerifyTransition(SAMETHING_CORE_SEQ_STATE_AFSK_EOM_THIRD,
                   SAMETHING_CORE_SEQ_STATE_SILENCE_SEVENTH);
}
