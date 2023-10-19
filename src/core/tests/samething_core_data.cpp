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

/** \file samething_core_data.cpp
 *
 * These test cases verify that the data values being used are consistent with
 * the latest version of the EAS protocol as defined by 47 CFR 11.31, which is
 * located at https://preview.tinyurl.com/eas-ecfr.
 */

#include "gtest/gtest.h"
#include "samething/core.h"

#ifndef NDEBUG
extern "C" void *samething_dbg_userdata_ = nullptr;

extern "C" void samething_dbg_assert_failed(const char *const,
                                            const char *const, const int,
                                            void *) {}
#endif  // NDEBUG

TEST(samething_core_data, HeaderDataMaxSizeIsCorrect) {
  EXPECT_EQ(SAMETHING_CORE_HEADER_SIZE_MAX, 268);
}

/// Ensures that the sample rate is 44100Hz.
TEST(samething_core_data, SampleRateIsCorrect) {
  EXPECT_EQ(SAMETHING_CORE_SAMPLE_RATE, 44100);
}

/// Ensures that the Preamble is 0xAB.
TEST(samething_core_data, PreambleIsCorrect) {
  EXPECT_EQ(SAMETHING_CORE_PREAMBLE, 0xAB);
}

/// Ensures that the number of times the Preamble must be transmitted is 16.
TEST(samething_core_data, PreambleAppearsSixteenTimes) {
  EXPECT_EQ(SAMETHING_CORE_PREAMBLE_NUM, 16);
}

/// Ensures that the length of the originator code is 3.
TEST(samething_core_data, OriginatorCodeLenIsCorrect) {
  EXPECT_EQ(SAMETHING_CORE_ORIGINATOR_CODE_LEN, 3);
}

/// Ensures that the number of characters for the event code is 3.
TEST(samething_core_data, EventCodeMaxLenIsCorrect) {
  EXPECT_EQ(SAMETHING_CORE_EVENT_CODE_LEN, 3);
}

/// Ensures that the maximum number of location codes is 31.
TEST(samething_core_data, LocationCodesMaxNumIsCorrect) {
  EXPECT_EQ(SAMETHING_CORE_LOCATION_CODES_NUM_MAX, 31);
}

/// Ensures that the number of characters for the location code is 6.
TEST(samething_core_data, LocationCodeLenIsCorrect) {
  EXPECT_EQ(SAMETHING_CORE_LOCATION_CODE_LEN, 6);
}

/// Ensures that the number of characters for a valid time period is 4.
TEST(samething_core_data, ValidTimePeriodLenIsCorrect) {
  EXPECT_EQ(SAMETHING_CORE_VALID_TIME_PERIOD_LEN, 4);
}

/// Ensures that the maximum number of characters for the originator time is 7.
TEST(samething_core_data, OriginatorTimeLenIsCorrect) {
  EXPECT_EQ(SAMETHING_CORE_ORIGINATOR_TIME_LEN, 7);
}

/// Ensures that the maximum number of characters for the identification code is
/// 8.
TEST(samething_core_data, CallsignLenIsCorrect) {
  EXPECT_EQ(SAMETHING_CORE_CALLSIGN_LEN, 8);
}

/// Ensures that the End of Message (EOM) header size is correct.
TEST(samething_core_data, EndOfMessageHeaderSizeIsCorrect) {
  EXPECT_EQ(SAMETHING_CORE_EOM_HEADER_SIZE, 20);
}

TEST(samething_core_data, AFSKBitRateIsCorrect) {
  EXPECT_FLOAT_EQ(SAMETHING_CORE_AFSK_BIT_RATE, 520.83F);
}

TEST(samething_core_data, AFSKMarkFreqIsCorrect) {
  EXPECT_FLOAT_EQ(SAMETHING_CORE_AFSK_MARK_FREQ, 2083.3F);
}

TEST(samething_core_data, AFSKSpaceFreqIsCorrect) {
  EXPECT_FLOAT_EQ(SAMETHING_CORE_AFSK_SPACE_FREQ, 1562.5F);
}

TEST(samething_core_data, AFSKBitDurationIsCorrect) {
  EXPECT_FLOAT_EQ(SAMETHING_CORE_AFSK_BIT_DURATION,
                  1.0F / SAMETHING_CORE_AFSK_BIT_RATE);
}

TEST(samething_core_data, AFSKBitsPerCharIsCorrect) {
  EXPECT_EQ(SAMETHING_CORE_AFSK_BITS_PER_CHAR, 8);
}

TEST(samething_core_data, AFSKSamplesPerBitIsCorrect) {
  EXPECT_EQ(SAMETHING_CORE_AFSK_SAMPLES_PER_BIT, 85);
}

TEST(samething_core_data, AttnSigFreqFirstIsCorrect) {
  EXPECT_FLOAT_EQ(SAMETHING_CORE_ATTN_SIG_FREQ_FIRST, 853.0F);
}

TEST(samething_core_data, AttnSigFreqSecondIsCorrect) {
  EXPECT_FLOAT_EQ(SAMETHING_CORE_ATTN_SIG_FREQ_SECOND, 960.0F);
}

TEST(samething_core_data, AttnSigMinDurationIsCorrect) {
  EXPECT_EQ(SAMETHING_CORE_ATTN_SIG_DURATION_MIN, 8);
}

TEST(samething_core_data, AttnSigMaxDurationIsCorrect) {
  EXPECT_EQ(SAMETHING_CORE_ATTN_SIG_DURATION_MAX, 25);
}
