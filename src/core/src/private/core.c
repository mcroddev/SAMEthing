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

/** \file core.c
 * Defines the implementation logic of SAME header generation.
 *
 * The sample rate is fixed at 44100Hz. There appears to be no good reason to
 * go above or below that value. Unfortunately, an authoritative answer is
 * not defined in the specification. Experimentation with various different
 * decoders has not shown any problems.
 *
 * Single-precision floating point is enforced; double-precision is not
 * necessary, and many embedded systems do not have double precision FPUs.
 * Using double-precision in a case for which it is already unnecessary would
 * lead to an increase in code size as the compiler would then have to pull in
 * soft-float routines in addition to incurring a performance penalty.
 *
 * Dynamic memory allocation is forbidden; all sizes are fixed, and all the
 * upper bounds are known at compile time.
 *
 * The maximum number of samples that can be generated is defined by the
 * following premises:
 *
 *     * The sample rate is 44100 Hz.
 *
 *     * There are 7 periods of silence, each lasting 1 second: 1 after each
 *       message transmission (there are 3), 1 after the attention signal, and 1
 *       after each End of Message (EOM) transmission (there are 3).
 *
 *     * There is a maximum of 252 bytes that can be transmitted during an AFSK
 *       burst, which occurs three times.
 *
 *     * The End of Message (EOM) transmission is 20 bytes, which occurs three
 *       times.
 *
 *     * There are a total of 6 burst transmissions; 3 for the message portion,
 *       and 3 for the EOM portion.
 *
 *     * The maximum amount of time an attention signal can last for is 25
 *       seconds.
 *
 *     * The duration of each bit is 1.92ms, and we must produce 520.83 bits per
 *       second. This gives us a calculation of ((1.0F / 520.83F) * 44100U)
 *       which gives us 84.672539. However, the value needs to be rounded UP to
 *       85.
 *
 *     * There are 8 bits in a character.
 *
 * These axioms give us the following calculations:
 *
 *     8 bits/char * 85 samples/bit * 252 bytes * 3 bursts = + 514,080 samples
 *     7 seconds * 44,100                                  = + 308,700 samples
 *     25 seconds * 44,100                                 = + 1,102,500 samples
 *     8 bits/char * 85 samples/bit * 20 bytes * 3 bursts  = + 40,800 samples
 *                                                           = 1,966,080 samples
 *                                                             -----------------
 *
 * Since we use a 16-bit integer type to store the sample data, the amount of
 * space required is (2 bytes * 1,966,080 samples) = **3,932,160 bytes, or
 * ~4 MB.**
 *
 * It is not practical to use such a large value on the stack on most embedded
 * targets, and even on traditional desktop systems this would still be
 * dangerous. Since dynamic memory allocation is off the table, the solution is
 * to generate chunks of audio samples and then push them to the audio device
 * incrementally. In our case, we choose to generate 4,096 samples at a time.
 */

#include "samething/core.h"

#include <math.h>
#include <string.h>

#include "samething/compiler.h"
#include "samething/debug.h"

#define SAMETHING_PI 3.141593F

SAMETHING_STATIC void samething_core_field_add(uint8_t *const restrict data,
                                               size_t *restrict data_size,
                                               const char *restrict const field,
                                               const size_t field_len) {
  SAMETHING_ASSERT(data != NULL);
  SAMETHING_ASSERT(data_size != NULL);
  SAMETHING_ASSERT(field != NULL);
  SAMETHING_ASSERT(field_len > 0);

  // XXX: SAMETHING_CORE_CALLSIGN_LEN is the largest field, if this ever
  // changes this must change as well.
  SAMETHING_ASSERT(field_len <= SAMETHING_CORE_CALLSIGN_LEN);

  memcpy(&data[*data_size], field, field_len);
  *data_size += field_len;
  data[(*data_size)++] = '-';
}

SAMETHING_STATIC void samething_core_afsk_gen(
    struct samething_core_gen_ctx *const restrict ctx,
    const uint8_t *const restrict data, const size_t data_size,
    const size_t sample_pos) {
  SAMETHING_ASSERT(ctx != NULL);
  SAMETHING_ASSERT(data != NULL);
  SAMETHING_ASSERT(data_size > 0);

  // ~9.50% of time in this function is spent on this operation.
  const float freq = ((data[ctx->afsk.data_pos] >> ctx->afsk.bit_pos) & 1)
                         ? SAMETHING_CORE_AFSK_MARK_FREQ
                         : SAMETHING_CORE_AFSK_SPACE_FREQ;

  const float t =
      (float)ctx->afsk.sample_num / (float)SAMETHING_CORE_SAMPLE_RATE;

  const int16_t sample =
      (int16_t)(sinf(SAMETHING_PI * 2 * t * freq) * INT16_MAX);
  ctx->sample_data[sample_pos] = sample;

  ctx->afsk.sample_num++;

  if (ctx->afsk.sample_num >= SAMETHING_CORE_AFSK_SAMPLES_PER_BIT) {
    ctx->afsk.sample_num = 0;
    ctx->afsk.bit_pos++;

    if (ctx->afsk.bit_pos >= SAMETHING_CORE_AFSK_BITS_PER_CHAR) {
      ctx->afsk.bit_pos = 0;
      ctx->afsk.data_pos++;

      if (ctx->afsk.data_pos >= data_size) {
        // By the time we get here, we're completely done caring about the AFSK
        // state for the current state; clear it to prepare for the next one.
        memset(&ctx->afsk, 0, sizeof(ctx->afsk));
      }
    }
  }
}

SAMETHING_STATIC void samething_core_silence_gen(
    struct samething_core_gen_ctx *const restrict ctx,
    const size_t sample_pos) {
  SAMETHING_ASSERT(ctx != NULL);
  ctx->sample_data[sample_pos] = 0;
}

SAMETHING_STATIC void samething_core_attn_sig_gen(
    struct samething_core_gen_ctx *const restrict ctx,
    const size_t sample_pos) {
  SAMETHING_ASSERT(ctx != NULL);

  const float t =
      (float)ctx->attn_sig_sample_num / (float)SAMETHING_CORE_SAMPLE_RATE;

  const float calc = SAMETHING_PI * 2 * t;

  const float first_freq =
      sinf(calc * SAMETHING_CORE_ATTN_SIG_FREQ_FIRST) / sizeof(int16_t);

  const float second_freq =
      sinf(calc * SAMETHING_CORE_ATTN_SIG_FREQ_SECOND) / sizeof(int16_t);

  ctx->sample_data[sample_pos] =
      (int16_t)((first_freq + second_freq) * INT16_MAX);
  ctx->attn_sig_sample_num++;
}

void samething_core_ctx_init(
    struct samething_core_gen_ctx *const restrict ctx,
    const struct samething_core_header *const restrict header) {
  SAMETHING_ASSERT(ctx != NULL);
  SAMETHING_ASSERT(header != NULL);

  static const uint8_t SAMETHING_CORE_INITIAL_HEADER[] = {
      SAMETHING_CORE_PREAMBLE,
      SAMETHING_CORE_PREAMBLE,
      SAMETHING_CORE_PREAMBLE,
      SAMETHING_CORE_PREAMBLE,
      SAMETHING_CORE_PREAMBLE,
      SAMETHING_CORE_PREAMBLE,
      SAMETHING_CORE_PREAMBLE,
      SAMETHING_CORE_PREAMBLE,
      SAMETHING_CORE_PREAMBLE,
      SAMETHING_CORE_PREAMBLE,
      SAMETHING_CORE_PREAMBLE,
      SAMETHING_CORE_PREAMBLE,
      SAMETHING_CORE_PREAMBLE,
      SAMETHING_CORE_PREAMBLE,
      SAMETHING_CORE_PREAMBLE,
      SAMETHING_CORE_PREAMBLE,
      'Z',
      'C',
      'Z',
      'C',
      '-',
      'O',
      'R',
      'G',
      '-',
      'E',
      'E',
      'E',
      '-',
      'P',
      'S',
      'S',
      'C',
      'C',
      'C'};

  memcpy(&ctx->header_data, SAMETHING_CORE_INITIAL_HEADER,
         sizeof(SAMETHING_CORE_INITIAL_HEADER));

  // We want to start populating the fields after the first dash.
  ctx->header_size =
      SAMETHING_CORE_PREAMBLE_NUM + SAMETHING_CORE_ASCII_ID_LEN + 1;

  samething_core_field_add(ctx->header_data, &ctx->header_size,
                           header->originator_code,
                           SAMETHING_CORE_ORIGINATOR_CODE_LEN);
  samething_core_field_add(ctx->header_data, &ctx->header_size,
                           header->event_code, SAMETHING_CORE_EVENT_CODE_LEN);

  for (size_t i = 0; i < SAMETHING_CORE_LOCATION_CODES_NUM_MAX; ++i) {
    if (memcmp(header->location_codes[i],
               SAMETHING_CORE_LOCATION_CODE_END_MARKER,
               SAMETHING_CORE_LOCATION_CODE_LEN) == 0) {
      break;
    }
    samething_core_field_add(ctx->header_data, &ctx->header_size,
                             header->location_codes[i],
                             SAMETHING_CORE_LOCATION_CODE_LEN);
  }
  ctx->header_data[ctx->header_size - 1] = '+';

  samething_core_field_add(ctx->header_data, &ctx->header_size,
                           header->valid_time_period,
                           SAMETHING_CORE_VALID_TIME_PERIOD_LEN);

  samething_core_field_add(ctx->header_data, &ctx->header_size,
                           header->originator_time,
                           SAMETHING_CORE_ORIGINATOR_TIME_LEN);

  samething_core_field_add(ctx->header_data, &ctx->header_size,
                           header->callsign, SAMETHING_CORE_CALLSIGN_LEN);

  // clang-format off
  ctx->seq_samples_remaining[SAMETHING_CORE_SEQ_STATE_AFSK_HEADER_FIRST] =
  ctx->seq_samples_remaining[SAMETHING_CORE_SEQ_STATE_AFSK_HEADER_SECOND] =
  ctx->seq_samples_remaining[SAMETHING_CORE_SEQ_STATE_AFSK_HEADER_THIRD] =
  SAMETHING_CORE_AFSK_BITS_PER_CHAR * SAMETHING_CORE_AFSK_SAMPLES_PER_BIT *
  (unsigned int)ctx->header_size;

  ctx->seq_samples_remaining[SAMETHING_CORE_SEQ_STATE_AFSK_EOM_FIRST] =
  ctx->seq_samples_remaining[SAMETHING_CORE_SEQ_STATE_AFSK_EOM_SECOND] =
  ctx->seq_samples_remaining[SAMETHING_CORE_SEQ_STATE_AFSK_EOM_THIRD] =
  SAMETHING_CORE_AFSK_BITS_PER_CHAR * SAMETHING_CORE_AFSK_SAMPLES_PER_BIT *
  SAMETHING_CORE_EOM_HEADER_SIZE;

  ctx->seq_samples_remaining[SAMETHING_CORE_SEQ_STATE_SILENCE_FIRST] =
  ctx->seq_samples_remaining[SAMETHING_CORE_SEQ_STATE_SILENCE_SECOND] =
  ctx->seq_samples_remaining[SAMETHING_CORE_SEQ_STATE_SILENCE_THIRD] =
  ctx->seq_samples_remaining[SAMETHING_CORE_SEQ_STATE_SILENCE_FOURTH] =
  ctx->seq_samples_remaining[SAMETHING_CORE_SEQ_STATE_SILENCE_FIFTH] =
  ctx->seq_samples_remaining[SAMETHING_CORE_SEQ_STATE_SILENCE_SIXTH] =
  ctx->seq_samples_remaining[SAMETHING_CORE_SEQ_STATE_SILENCE_SEVENTH] =
  SAMETHING_CORE_SILENCE_DURATION * SAMETHING_CORE_SAMPLE_RATE;

  ctx->seq_samples_remaining[SAMETHING_CORE_SEQ_STATE_ATTENTION_SIGNAL] =
  header->attn_sig_duration * SAMETHING_CORE_SAMPLE_RATE;
  // clang-format on
}

void samething_core_samples_gen(struct samething_core_gen_ctx *const ctx) {
  SAMETHING_ASSERT(ctx != NULL);

  static const uint8_t
      SAMETHING_CORE_EOM_HEADER[SAMETHING_CORE_EOM_HEADER_SIZE] = {
          SAMETHING_CORE_PREAMBLE,
          SAMETHING_CORE_PREAMBLE,
          SAMETHING_CORE_PREAMBLE,
          SAMETHING_CORE_PREAMBLE,
          SAMETHING_CORE_PREAMBLE,
          SAMETHING_CORE_PREAMBLE,
          SAMETHING_CORE_PREAMBLE,
          SAMETHING_CORE_PREAMBLE,
          SAMETHING_CORE_PREAMBLE,
          SAMETHING_CORE_PREAMBLE,
          SAMETHING_CORE_PREAMBLE,
          SAMETHING_CORE_PREAMBLE,
          SAMETHING_CORE_PREAMBLE,
          SAMETHING_CORE_PREAMBLE,
          SAMETHING_CORE_PREAMBLE,
          SAMETHING_CORE_PREAMBLE,
          'N',
          'N',
          'N',
          'N'};

  // Tried to generate a SAME header using a context for which a SAME header was
  // already generated; bug.
  SAMETHING_ASSERT(ctx->seq_state < SAMETHING_CORE_SEQ_STATE_NUM);

  // Generate only SAMETHING_CORE_SAMPLES_NUM_MAX samples at a time.
  for (unsigned int sample_count = 0;
       sample_count < SAMETHING_CORE_SAMPLES_NUM_MAX; ++sample_count) {
    switch (ctx->seq_state) {
      case SAMETHING_CORE_SEQ_STATE_AFSK_HEADER_FIRST:
      case SAMETHING_CORE_SEQ_STATE_AFSK_HEADER_SECOND:
      case SAMETHING_CORE_SEQ_STATE_AFSK_HEADER_THIRD:
        samething_core_afsk_gen(ctx, ctx->header_data, ctx->header_size,
                                sample_count);
        break;

      case SAMETHING_CORE_SEQ_STATE_SILENCE_FIRST:
      case SAMETHING_CORE_SEQ_STATE_SILENCE_SECOND:
      case SAMETHING_CORE_SEQ_STATE_SILENCE_THIRD:
      case SAMETHING_CORE_SEQ_STATE_SILENCE_FOURTH:
      case SAMETHING_CORE_SEQ_STATE_SILENCE_FIFTH:
      case SAMETHING_CORE_SEQ_STATE_SILENCE_SIXTH:
      case SAMETHING_CORE_SEQ_STATE_SILENCE_SEVENTH:
        samething_core_silence_gen(ctx, sample_count);
        break;

      case SAMETHING_CORE_SEQ_STATE_ATTENTION_SIGNAL:
        samething_core_attn_sig_gen(ctx, sample_count);
        break;

      case SAMETHING_CORE_SEQ_STATE_AFSK_EOM_FIRST:
      case SAMETHING_CORE_SEQ_STATE_AFSK_EOM_SECOND:
      case SAMETHING_CORE_SEQ_STATE_AFSK_EOM_THIRD:
        samething_core_afsk_gen(ctx, SAMETHING_CORE_EOM_HEADER,
                                SAMETHING_CORE_EOM_HEADER_SIZE, sample_count);
        break;

      default:
        SAMETHING_UNREACHABLE;
        break;
    }
    ctx->seq_samples_remaining[ctx->seq_state]--;

    if (ctx->seq_samples_remaining[ctx->seq_state] == 0) {
      ctx->seq_state++;

      if (ctx->seq_state >= SAMETHING_CORE_SEQ_STATE_NUM) {
        // We're done generating.
        return;
      }
    }
  }
}
