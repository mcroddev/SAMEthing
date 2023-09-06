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

#ifndef SAMETHING_CORE_H
#define SAMETHING_CORE_H

#pragma once

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/// How many times should we send a burst?
#define SAMETHING_CORE_BURSTS_NUM_MAX (3)

/// The number of ASCII alphabetical characters which compose the originator
/// code.
#define SAMETHING_CORE_ORIGINATOR_CODE_LEN_MAX (3)

/// The number of ASCII alphabetical characters which compose the event code.
#define SAMETHING_CORE_EVENT_CODE_LEN_MAX (3)

/// There may be 31 Location codes in an EAS alert.
#define SAMETHING_CORE_LOCATION_CODE_NUM_MAX (31)

/// The number of ASCII numerical characters which compose a location code.
#define SAMETHING_CORE_LOCATION_CODE_LEN_MAX (6)

/// The number of ASCII numerical characters which compose a valid time period.
#define SAMETHING_CORE_VALID_TIME_PERIOD_LEN_MAX (4)

/// The number of ASCII characters, both numerical and alphabetical, which
/// compose a valid originator time period.
#define SAMETHING_CORE_ORIGINATOR_TIME_LEN_MAX (7)

/// The number of ASCII characters, both numerical and alphabetical, which
/// compose a valid identification code.
#define SAMETHING_CORE_ID_LEN_MAX (8)

/// How many bytes compose the End of Message (EOM) transmission?
#define SAMETHING_CORE_EOM_HEADER_SIZE (20)

/// The maximum number of characters a header can be.
#define SAMETHING_CORE_HEADER_SIZE_MAX (252)

/// ...
#define SAMETHING_CORE_LOCATION_CODE_END_MARKER ("FFFFFF")

/// How many samples will we generate for every `samething_core_samples_gen()`
/// call?
#define SAMETHING_CORE_SAMPLES_NUM_MAX (4096)

/// Consecutive string of bits (sixteen bytes of AB hexadecimal [8 bit byte
/// 1010'1011] sent to clear the system, set AGC and set asynchronous decoder
/// clocking cycles. The preamble must be transmitted before each header and
/// End of Message code.
#define SAMETHING_CORE_PREAMBLE (0xAB)

/// The number of audio samples per second.
#define SAMETHING_CORE_SAMPLE_RATE (44100)

/// How long should a period of silence last for in seconds?
#define SAMETHING_CORE_SILENCE_DURATION (1.0F)

/// First fundamental frequency of an attention signal.
#define SAMETHING_CORE_ATTN_SIG_FREQ_FIRST (853.0F)

/// Second fundamental frequency of an attention signal.
#define SAMETHING_CORE_ATTN_SIG_FREQ_SECOND (960.0F)

/// The Preamble and EAS codes must use Audio Frequency Shift Keying at a rate
/// of 520.83 bits per second to transmit the codes.
#define SAMETHING_CORE_AFSK_BIT_RATE (520.83F)

/// Mark and space time must be 1.92 milliseconds.
#define SAMETHING_CORE_AFSK_BIT_DURATION (1.0F / SAMETHING_CORE_AFSK_BIT_RATE)

/// Mark frequency is 2083.3Hz.
#define SAMETHING_CORE_AFSK_MARK_FREQ (2083.3F)

/// Space frequency is 1562.5Hz.
#define SAMETHING_CORE_AFSK_SPACE_FREQ (1562.5F)

/// How many samples should we generate for each bit?
#define SAMETHING_CORE_AFSK_SAMPLES_PER_BIT \
  (SAMETHING_CORE_AFSK_BIT_DURATION * SAMETHING_CORE_SAMPLE_RATE)

/// How many bits per character?
#define SAMETHING_CORE_AFSK_BITS_PER_CHAR (8)

/// The minimum amount of time an attention signal can be.
#define SAMETHING_CORE_ATTN_SIG_MIN (8.0F)

/// The maximum amount of time an attention signal can last for.
#define SAMETHING_CORE_ATTN_SIG_MAX (25.0F)

enum samething_core_gen_state {
  SAMETHING_CORE_GEN_STATE_AFSK_HEADER_FIRST,
  SAMETHING_CORE_GEN_STATE_SILENCE_FIRST,
  SAMETHING_CORE_GEN_STATE_AFSK_HEADER_SECOND,
  SAMETHING_CORE_GEN_STATE_SILENCE_SECOND,
  SAMETHING_CORE_GEN_STATE_AFSK_HEADER_THIRD,
  SAMETHING_CORE_GEN_STATE_SILENCE_THIRD,
  SAMETHING_CORE_GEN_STATE_ATTENTION_SIGNAL,
  SAMETHING_CORE_GEN_STATE_SILENCE_FOURTH,
  SAMETHING_CORE_GEN_STATE_AFSK_EOM_FIRST,
  SAMETHING_CORE_GEN_STATE_SILENCE_FIFTH,
  SAMETHING_CORE_GEN_STATE_AFSK_EOM_SECOND,
  SAMETHING_CORE_GEN_STATE_SILENCE_SIXTH,
  SAMETHING_CORE_GEN_STATE_AFSK_EOM_THIRD,
  SAMETHING_CORE_GEN_STATE_SILENCE_SEVENTH,
  SAMETHING_CORE_GEN_STATE_NUM
};

struct samething_core_header {
  /// Indicates the geographic areas affected by the EAS alert.
  char location_codes[SAMETHING_CORE_LOCATION_CODE_NUM_MAX]
                     [SAMETHING_CORE_LOCATION_CODE_LEN_MAX + 1];

  /// Indicates the valid time period of a message.
  char valid_time_period[SAMETHING_CORE_VALID_TIME_PERIOD_LEN_MAX + 1];

  /// Indicates who originally initiated the activation of the EAS.
  char originator_code[SAMETHING_CORE_ORIGINATOR_CODE_LEN_MAX + 1];

  /// Indicates the nature of the EAS activation.
  char event_code[SAMETHING_CORE_EVENT_CODE_LEN_MAX + 1];

  /// Identification of the EAS Participant, NWS office, etc., transmitting or
  /// retransmitting the message. These codes will automatically be affixed to
  /// all outgoing messages by the EAS encoder.
  char id[SAMETHING_CORE_ID_LEN_MAX + 1];

  /// Indicates when the message was initially released by the originator.
  char originator_time[SAMETHING_CORE_ORIGINATOR_TIME_LEN_MAX + 1];

  float attn_sig_duration;
};

/// Defines the generation context.
///
/// A generation context keeps track of the audio generation state over each
/// call to the samething_core_samples_gen function.
struct samething_core_gen_ctx {
  // The buffer containing the audio samples.
  int16_t data[SAMETHING_CORE_SAMPLES_NUM_MAX];

  /// The header data to generate an AFSK burst from.
  uint8_t header_data[SAMETHING_CORE_HEADER_SIZE_MAX];

  /// The number of samples remaining for each state of the generation.
  int state_samples_remaining[SAMETHING_CORE_GEN_STATE_NUM];

  /// The actual size of the header to care about.
  size_t header_size;

  /// The maximum number of samples we should generate.
  int samples_num_max;

  /// The current generation state.
  enum samething_core_gen_state gen_state;

  struct {
    size_t data_pos;
    int bit_pos;
    int sample_num;
  } afsk;

  struct {
    int sample_num;
  } attn_sig;
};

#ifdef SAMETHING_TESTING
void samething_core_afsk_gen(struct samething_core_gen_ctx *const ctx,
                             const uint8_t *const data, const size_t data_size,
                             size_t num_samples);

void samething_core_silence_gen(struct samething_core_gen_ctx *const ctx,
                                const size_t num_samples);

void samething_core_attn_sig_gen(struct samething_core_gen_ctx *const ctx,
                                 const size_t num_samples);

void samething_core_field_add(uint8_t *const data, size_t *data_size,
                              const char *const field, const size_t field_len);
#endif  // SAMETHING_TESTING

/// Configures a generation context to generate the specified header.
void samething_core_ctx_config(
    struct samething_core_gen_ctx *const ctx,
    const struct samething_core_header *const header);

/// Generates audio samples from a Specific Area Message Encoding (SAME) header.
///
/// \param ctx The generation context.
void samething_core_samples_gen(struct samething_core_gen_ctx *const ctx);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SAMETHING_CORE_H
