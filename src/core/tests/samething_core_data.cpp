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

/// These test cases verify that the data values being used are consistent with
/// the latest version of the EAS protocol as defined by 47 CFR 11.31, which
/// is located at https://preview.tinyurl.com/eas-ecfr.

/// Ensures that the sample rate is 44100Hz.
TEST(samething_core_data, SampleRateIsCorrect) {
  EXPECT_FLOAT_EQ(SAMETHING_CORE_SAMPLE_RATE, 44100.0F);
}

/// Ensures that the Preamble is 0xAB.
TEST(samething_core_data, PreambleIsCorrect) {
  EXPECT_EQ(SAMETHING_CORE_PREAMBLE, 0xAB);
}

/// Ensures that the number of bursts to transmit is 3.
TEST(samething_core_data, MaxNumBurstsIsCorrect) {
  EXPECT_EQ(SAMETHING_CORE_BURSTS_NUM_MAX, 3);
}

/// Ensures that the maximum length of the originator code is 3.
TEST(samething_core_data, OriginatorCodeLenMaxIsCorrect) {
  EXPECT_EQ(SAMETHING_CORE_ORIGINATOR_CODE_LEN_MAX, 3);
}

/// Ensures that the maximum number of characters for the event code is 3.
TEST(samething_core_data, EventCodeLenMaxIsCorrect) {
  EXPECT_EQ(SAMETHING_CORE_EVENT_CODE_LEN_MAX, 3);
}

/// Ensures that the maximum number of location codes is 31.
TEST(samething_core_data, LocationCodesNumMaxIsCorrect) {
  EXPECT_EQ(SAMETHING_CORE_LOCATION_CODE_NUM_MAX, 31);
}

/// Ensures that the maximum number of characters for the location code is 6.
TEST(samething_core_data, LocationCodeLenMaxIsCorrect) {
  EXPECT_EQ(SAMETHING_CORE_LOCATION_CODE_LEN_MAX, 6);
}

/// Ensures that the maximum number of characters for a valid time period is 4.
TEST(samething_core_data, ValidTimePeriodLenMaxIsCorrect) {
  EXPECT_EQ(SAMETHING_CORE_VALID_TIME_PERIOD_LEN_MAX, 4);
}

/// Ensures that the maximum number of characters for the originator time is 7.
TEST(samething_core_data, OriginatorTimeLenMaxIsCorrect) {
  EXPECT_EQ(SAMETHING_CORE_ORIGINATOR_TIME_LEN_MAX, 7);
}

/// Ensures that the maximum number of characters for the identification code is
/// 8.
TEST(samething_core_data, IDLenMaxIsCorrect) {
  EXPECT_EQ(SAMETHING_CORE_ID_LEN_MAX, 8);
}

/// Ensures that the End of Message (EOM) header size is correct.
TEST(samething_core_data, EndOfMessageHeaderSizeIsCorrect) {
  EXPECT_EQ(SAMETHING_CORE_EOM_HEADER_SIZE, 20);
}

/// Ensures that the End of Message (EOM) header is correct.
TEST(samething_core_data, EndOfMessageHeaderIsCorrect) {

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
  int kExpectedAFSKSamplesPerBit =
      static_cast<int>(SAMETHING_CORE_AFSK_BIT_DURATION) *
      SAMETHING_CORE_SAMPLE_RATE;

  EXPECT_EQ(SAMETHING_CORE_AFSK_SAMPLES_PER_BIT, kExpectedAFSKSamplesPerBit);
}

TEST(samething_core_data, AttnSigFreqFirstIsCorrect) {
  EXPECT_FLOAT_EQ(SAMETHING_CORE_ATTN_SIG_FREQ_FIRST, 853.F);
}

TEST(samething_core_data, AttnSigFreqSecondIsCorrect) {
  EXPECT_FLOAT_EQ(SAMETHING_CORE_ATTN_SIG_FREQ_SECOND, 960.F);
}

TEST(samething_core_data, AttnSigMinLenIsCorrect) {
  EXPECT_FLOAT_EQ(SAMETHING_CORE_ATTN_SIG_MIN, 8.F);
}

TEST(samething_core_data, AttnSigMaxLenIsCorrect) {
  EXPECT_FLOAT_EQ(SAMETHING_CORE_ATTN_SIG_MAX, 25.F);
}
