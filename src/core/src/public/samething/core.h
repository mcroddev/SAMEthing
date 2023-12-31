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

/// Consecutive string of bits (sixteen bytes of AB hexadecimal [8 bit byte
/// 1010'1011]) sent to clear the system, set AGC and set asynchronous decoder
/// clocking cycles.
///
/// The preamble must be transmitted before each header and End of Message code.
#define SAMETHING_CORE_PREAMBLE (UINT8_C(0xAB))

/// The number of times the preamble will appear.
#define SAMETHING_CORE_PREAMBLE_NUM (16U)

/// The length of the identifier sent to indicate the start of ASCII code.
#define SAMETHING_CORE_ASCII_ID_LEN (4U)

/// The length of the originator code (ORG) field.
#define SAMETHING_CORE_ORIGINATOR_CODE_LEN (3U)

/// The length of the event code (EEE) field.
#define SAMETHING_CORE_EVENT_CODE_LEN (3U)

/// The maximum number of location codes we're allowed to transmit.
#define SAMETHING_CORE_LOCATION_CODES_NUM_MAX (31U)

/// The length of a location code (PSSCCC) field.
#define SAMETHING_CORE_LOCATION_CODE_LEN (6U)

/// The length of the valid time period (TTTT) field.
#define SAMETHING_CORE_VALID_TIME_PERIOD_LEN (4U)

/// The length of the originator time (JJJHHMM) field.
#define SAMETHING_CORE_ORIGINATOR_TIME_LEN (7U)

/// The length of the callsign (LLLLLLLL) field.
#define SAMETHING_CORE_CALLSIGN_LEN (8U)

/// The total number of fields which make up a message, not counting the
/// preamble or the ASCII start code.
#define SAMETHING_CORE_FIELDS_NUM_TOTAL (6U)

/// The number of bytes which compose the End of Message (EOM) transmission.
#define SAMETHING_CORE_EOM_HEADER_SIZE (SAMETHING_CORE_PREAMBLE_NUM + 4U)

/// The maximum number of characters a header can hold.
///
/// There should be no need to adjust this macro directly; adjust the values it
/// references instead.
#define SAMETHING_CORE_HEADER_SIZE_MAX                                         \
  (SAMETHING_CORE_PREAMBLE_NUM + SAMETHING_CORE_ASCII_ID_LEN +                 \
   SAMETHING_CORE_ORIGINATOR_CODE_LEN + SAMETHING_CORE_EVENT_CODE_LEN +        \
   (SAMETHING_CORE_LOCATION_CODES_NUM_MAX *                                    \
    SAMETHING_CORE_LOCATION_CODE_LEN) +                                        \
   SAMETHING_CORE_VALID_TIME_PERIOD_LEN + SAMETHING_CORE_ORIGINATOR_TIME_LEN + \
   SAMETHING_CORE_CALLSIGN_LEN + SAMETHING_CORE_LOCATION_CODES_NUM_MAX +       \
   SAMETHING_CORE_FIELDS_NUM_TOTAL)

/// This value should be copied to the location code index corresponding to the
/// final entry + 1.
///
/// Example: If you specify only two location codes, the location codes
/// specified in a samething_core_header's location code array should be
/// arranged like so:
///
///     [0]: 010101
///     [1]: 101101
///     [2]: SAMETHING_CORE_LOCATION_CODE_END_MARKER
#define SAMETHING_CORE_LOCATION_CODE_END_MARKER ("SPOOKY")

/// The number of audio samples per each chunk.
#define SAMETHING_CORE_SAMPLES_NUM_MAX (4096U)

/// The number of audio samples per second.
///
/// This value is not defined in any specification; however, it is not
/// unreasonable to assume 44100 Hz. Testing with various decoders has not
/// revealed any issues.
#define SAMETHING_CORE_SAMPLE_RATE (44100U)

/// The length of a period of silence in seconds.
#define SAMETHING_CORE_SILENCE_DURATION (1U)

/// The first fundamental frequency of the attention signal.
#define SAMETHING_CORE_ATTN_SIG_FREQ_FIRST (853.0F)

/// The second fundamental frequency of the attention signal.
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

/// How many samples should we generate for each bit during an AFSK burst?
///
/// **WARNING:** The result should always be rounded up!
#define SAMETHING_CORE_AFSK_SAMPLES_PER_BIT           \
  ((unsigned int)((SAMETHING_CORE_AFSK_BIT_DURATION * \
                   SAMETHING_CORE_SAMPLE_RATE) +      \
                  0.5F))

/// How many bits per character?
#define SAMETHING_CORE_AFSK_BITS_PER_CHAR (8U)

/// The minimum number of seconds the attention signal can last for.
#define SAMETHING_CORE_ATTN_SIG_DURATION_MIN (8U)

/// The maximum number of seconds the attention signal can last for.
#define SAMETHING_CORE_ATTN_SIG_DURATION_MAX (25U)

/// Defines the generation sequence states.
///
/// The sequence states dictate what portion of the SAME header we are
/// generating. These states are laid out in the natural order as one would
/// hear.
enum samething_core_seq_state {
  /// First AFSK burst of header
  SAMETHING_CORE_SEQ_STATE_AFSK_HEADER_FIRST,

  /// 1 period of silence.
  SAMETHING_CORE_SEQ_STATE_SILENCE_FIRST,

  /// Second AFSK burst of header.
  SAMETHING_CORE_SEQ_STATE_AFSK_HEADER_SECOND,

  /// 1 period of silence.
  SAMETHING_CORE_SEQ_STATE_SILENCE_SECOND,

  /// Third AFSK burst of header.
  SAMETHING_CORE_SEQ_STATE_AFSK_HEADER_THIRD,

  /// 1 period of silence.
  SAMETHING_CORE_SEQ_STATE_SILENCE_THIRD,

  /// Attention signal for 8..25 seconds.
  SAMETHING_CORE_SEQ_STATE_ATTENTION_SIGNAL,

  /// 1 period of silence.
  SAMETHING_CORE_SEQ_STATE_SILENCE_FOURTH,

  /// First AFSK burst of EOM.
  SAMETHING_CORE_SEQ_STATE_AFSK_EOM_FIRST,

  /// 1 period of silence.
  SAMETHING_CORE_SEQ_STATE_SILENCE_FIFTH,

  /// Second AFSK burst of EOM.
  SAMETHING_CORE_SEQ_STATE_AFSK_EOM_SECOND,

  /// 1 period of silence.
  SAMETHING_CORE_SEQ_STATE_SILENCE_SIXTH,

  /// Third AFSK burst of EOM.
  SAMETHING_CORE_SEQ_STATE_AFSK_EOM_THIRD,

  /// 1 period of silence.
  SAMETHING_CORE_SEQ_STATE_SILENCE_SEVENTH,

  /// The total number of sequence states. Do not modify or remove this entry.
  SAMETHING_CORE_SEQ_STATE_NUM
};

/// Defines the header to be used for generating a full SAME header. This is
/// what users should be using.
///
/// Be aware that no error checking takes place here!
struct samething_core_header {
  /// Indicates the geographic areas affected by the EAS alert.
  char location_codes[SAMETHING_CORE_LOCATION_CODES_NUM_MAX]
                     [SAMETHING_CORE_LOCATION_CODE_LEN + 1];

  /// Indicates the valid time period of a message.
  char valid_time_period[SAMETHING_CORE_VALID_TIME_PERIOD_LEN + 1];

  /// Indicates who originally initiated the activation of the EAS.
  char originator_code[SAMETHING_CORE_ORIGINATOR_CODE_LEN + 1];

  /// Indicates the nature of the EAS activation.
  char event_code[SAMETHING_CORE_EVENT_CODE_LEN + 1];

  /// Identification of the EAS Participant, NWS office, etc., transmitting or
  /// retransmitting the message. These codes will automatically be affixed to
  /// all outgoing messages by the EAS encoder.
  char callsign[SAMETHING_CORE_CALLSIGN_LEN + 1];

  /// Indicates when the message was initially released by the originator.
  char originator_time[SAMETHING_CORE_ORIGINATOR_TIME_LEN + 1];

  /// How long will the attention signal last for?
  unsigned int attn_sig_duration;
};

/// Defines the generation context.
///
/// A generation context keeps track of the audio generation state over each
/// call to the samething_core_samples_gen function.
struct samething_core_gen_ctx {
  // The buffer containing the audio samples.
  int16_t sample_data[SAMETHING_CORE_SAMPLES_NUM_MAX];

  /// The header data to generate an AFSK burst from.
  uint8_t header_data[SAMETHING_CORE_HEADER_SIZE_MAX];

  /// The number of samples remaining for each generation sequence.
  unsigned int seq_samples_remaining[SAMETHING_CORE_SEQ_STATE_NUM];

  struct {
    /// The current position within the data.
    size_t data_pos;

    /// The current bit we're generating a sine wave for.
    unsigned int bit_pos;

    /// The current sample we're generating.
    unsigned int sample_num;
  } afsk;

  /// The actual size of the header to care about.
  size_t header_size;

  /// The current sequence of the generation.
  enum samething_core_seq_state seq_state;

  /// The current sample we're generating for the attention signal.
  unsigned int attn_sig_sample_num;
};

#ifdef SAMETHING_TESTING
/// Generates an Audio Frequency Shift Keying (AFSK) burst.
///
/// @param ctx The generation context in use, which stores the state of the
///            generation.
/// @param data The data to generate an AFSK burst from.
/// @param data_size The size of the data to generate an AFSK burst from.
/// @param num_samples The number of samples to generate.
void samething_core_afsk_gen(struct samething_core_gen_ctx *const ctx,
                             const uint8_t *const data, const size_t data_size,
                             const size_t sample_pos);

/// Generates silence for a period of 1 second.
///
/// @param ctx The generation context in use, which stores the state of the
///            generation.
/// @param num_samples The number of samples to generate.
void samething_core_silence_gen(struct samething_core_gen_ctx *const ctx,
                                const size_t sample_pos);

/// Generates the attention signal.
///
/// @param ctx The generation context in use, which stores the state of the
///            generation.
/// @param num_samples The number of samples to generate.
void samething_core_attn_sig_gen(struct samething_core_gen_ctx *const ctx,
                                 const size_t sample_pos);

/// Adds a field to the data.
///
/// A field is defined as any portion of the SAME header which must be populated
/// (e.g., originator code, event code).
///
/// @param data The data to append the field to.
/// \param data_size The new occupied space of the data.
/// \param field The field to append.
/// \param field_len The length of the field to append.
void samething_core_field_add(uint8_t *const data, size_t *data_size,
                              const char *const field, const size_t field_len);
#endif  // SAMETHING_TESTING

/// Configures a generation context to generate the specified header.
///
/// @param ctx The generation context.
/// @param header The header data to generate a SAME header from.
void samething_core_ctx_init(struct samething_core_gen_ctx *const ctx,
                             const struct samething_core_header *const header);

/// Generates audio samples from a Specific Area Message Encoding (SAME) header.
///
/// \param ctx The generation context.
void samething_core_samples_gen(struct samething_core_gen_ctx *const ctx);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SAMETHING_CORE_H
