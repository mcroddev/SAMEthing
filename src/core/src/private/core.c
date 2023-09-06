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

// A few points to make note of here:
//
// * The sample rate is fixed to 44100Hz. There appears to be no good reason to
//   go above or below that value. Unfortunately, an authoritative answer is not
//   defined in the specification.
//
// * Single-precision floating point is enforced; double precision is not
//   necessary and many embedded systems do not have double precision FPUs.
//   Using double-precision in such a case for which it is already unnecessary
//   would lead to an increase in code size and a performance penalty as the
//   compiler would then have to pull in soft-float routines.
//
// * Dynamic memory allocation is forbidden; all sizes are fixed and all the
//   upper bounds are known at compile-time.
//
// * The maximum number of samples that can be generated begins with the
//   following premises:
//
//     a) The sample rate is 44100Hz.
//
//     b) There are 7 periods of silence, each lasting 1 second: 3 after each
//        message transmission, 1 after the attention signal, and 3 more after
//        each End of Message (EOM) transmission.
//
//     c) There is a maximum of 252 bytes that can be transmitted during an AFSK
//        burst, which occurs 3 times.
//
//     d) There is a maximum of 20 bytes that can be transmitted during an AFSK
//        burst for the End of Message (EOM) transmission, which occurs 3 times.
//
//     e) There are a total of 6 burst transmissions; 3 for the message portion,
//        and 3 for the EOM portion.
//
//     f) The maximum amount of time an attention signal can last for is 25
//        seconds.
//
//     These axioms give us the following calculations:
//
//     8 bits/char * 84 samples/bit * 252 bytes * 3 bursts = + 508,032 samples
//     7 seconds * 44,100                                  = + 308,700 samples
//     25 seconds * 44,100                                 = + 1,102,500 samples
//     8 bits/char * 84 samples/bit * 20 bytes * 3 bursts  = + 40,320 samples
//                                                           = 1,959,552 samples
//                                                             -----------------
//
//     Since we use a signed 16-bit integer type to store the sample data, the
//     amount of space required is (2 bytes * 1,959,552 samples) = 3,919,104
//     bytes, or ~4 MB.
//
//     It is not practical to use such a large value on the stack on most
//     embedded targets, and even on traditional desktop systems this would
//     still be dangerous. Since dynamic memory allocation is off the table, the
//     solution is to generate chunks of audio samples and then push them to
//     the audio device incrementally. In our case, we choose to generate 4,096
//     samples at a time.

#include "core/core.h"

#include <math.h>
#include <string.h>

#include "core/debug.h"

SAMETHING_CORE_STATIC void samething_core_field_add(uint8_t *const data,
                                                    size_t *data_size,
                                                    const char *const field,
                                                    const size_t field_len) {
  memcpy(&data[*data_size], field, field_len);
  *data_size += field_len;
  data[(*data_size)++] = '-';
}

SAMETHING_CORE_STATIC void samething_core_afsk_gen_header(
    struct samething_core_gen_ctx *const ctx, const size_t num_samples) {
  samething_core_afsk_gen(ctx, ctx->header_data, ctx->header_size, num_samples);
}

SAMETHING_CORE_STATIC void samething_core_afsk_gen_eom(
    struct samething_core_gen_ctx *const ctx, const size_t num_samples) {
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

  samething_core_afsk_gen(ctx, SAMETHING_CORE_EOM_HEADER,
                          SAMETHING_CORE_EOM_HEADER_SIZE, num_samples);
}

SAMETHING_CORE_STATIC void samething_core_afsk_gen(
    struct samething_core_gen_ctx *const ctx, const uint8_t *const data,
    const size_t data_size, size_t num_samples) {
  int16_t *samples = ctx->data;

  while (num_samples--) {
    const float freq = ((data[ctx->afsk.data_pos] >> ctx->afsk.bit_pos) & 1)
                           ? SAMETHING_CORE_AFSK_MARK_FREQ
                           : SAMETHING_CORE_AFSK_SPACE_FREQ;

    const float t = ctx->afsk.sample_num / (float)SAMETHING_CORE_SAMPLE_RATE;

    *samples++ = sinf(M_PI * 2 * t * freq) * INT16_MAX;
    ctx->afsk.sample_num++;

    if (ctx->afsk.sample_num >= SAMETHING_CORE_AFSK_SAMPLES_PER_BIT) {
      ctx->afsk.sample_num = 0;
      ctx->afsk.bit_pos++;

      if (ctx->afsk.bit_pos >= SAMETHING_CORE_AFSK_BITS_PER_CHAR) {
        ctx->afsk.bit_pos = 0;
        ctx->afsk.data_pos++;

        if (ctx->afsk.data_pos >= data_size) {
          ctx->afsk.data_pos = 0;
        }
      }
    }
  }
}

SAMETHING_CORE_STATIC
void samething_core_silence_gen(struct samething_core_gen_ctx *const ctx,
                                size_t num_samples) {
  while (num_samples--) {
    ctx->data[num_samples] = 0;
  }
}

SAMETHING_CORE_STATIC void samething_core_attn_sig_gen(
    struct samething_core_gen_ctx *const ctx, size_t num_samples) {
  int16_t *data = ctx->data;

  while (num_samples--) {
    const float t = ctx->attn_sig.sample_num / (float)SAMETHING_CORE_SAMPLE_RATE;
    const float calc = M_PI * 2 * t;

    *data++ =
        (sinf(calc * SAMETHING_CORE_ATTN_SIG_FREQ_FIRST) / sizeof(int16_t) +
         sinf(calc * SAMETHING_CORE_ATTN_SIG_FREQ_SECOND) / sizeof(int16_t)) *
        INT16_MAX;

    ctx->attn_sig.sample_num++;
  }
}

void samething_core_ctx_config(
    struct samething_core_gen_ctx *const ctx,
    const struct samething_core_header *const header) {
  uint8_t data[SAMETHING_CORE_HEADER_SIZE_MAX] = {SAMETHING_CORE_PREAMBLE,
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

  // We want to start populating the fields after the first dash.
  size_t data_size = 21;

  samething_core_field_add(data, &data_size, header->originator_code,
                           SAMETHING_CORE_ORIGINATOR_CODE_LEN_MAX);
  samething_core_field_add(data, &data_size, header->event_code,
                           SAMETHING_CORE_EVENT_CODE_LEN_MAX);

  for (size_t i = 0; i < SAMETHING_CORE_LOCATION_CODE_NUM_MAX; ++i) {
    if (memcmp(header->location_codes[i],
               SAMETHING_CORE_LOCATION_CODE_END_MARKER,
               SAMETHING_CORE_LOCATION_CODE_LEN_MAX) == 0) {
      break;
    }
    samething_core_field_add(data, &data_size, header->location_codes[i],
                             SAMETHING_CORE_LOCATION_CODE_LEN_MAX);
  }
  data[data_size - 1] = '+';

  samething_core_field_add(data, &data_size, header->valid_time_period,
                           SAMETHING_CORE_VALID_TIME_PERIOD_LEN_MAX);

  samething_core_field_add(data, &data_size, header->originator_time,
                           SAMETHING_CORE_ORIGINATOR_TIME_LEN_MAX);

  samething_core_field_add(data, &data_size, header->id,
                           SAMETHING_CORE_ID_LEN_MAX);

  memcpy(ctx->header_data, data, data_size);
  ctx->header_size = data_size;

  // clang-format off

  ctx->state_samples_remaining[SAMETHING_CORE_GEN_STATE_AFSK_HEADER_FIRST] =
  ctx->state_samples_remaining[SAMETHING_CORE_GEN_STATE_AFSK_HEADER_SECOND] =
  ctx->state_samples_remaining[SAMETHING_CORE_GEN_STATE_AFSK_HEADER_THIRD] =
      SAMETHING_CORE_AFSK_BITS_PER_CHAR * SAMETHING_CORE_AFSK_SAMPLES_PER_BIT *
      data_size;

  ctx->state_samples_remaining[SAMETHING_CORE_GEN_STATE_AFSK_EOM_FIRST] =
  ctx->state_samples_remaining[SAMETHING_CORE_GEN_STATE_AFSK_EOM_SECOND] =
  ctx->state_samples_remaining[SAMETHING_CORE_GEN_STATE_AFSK_EOM_THIRD] =
  SAMETHING_CORE_AFSK_BITS_PER_CHAR * SAMETHING_CORE_AFSK_SAMPLES_PER_BIT *
  SAMETHING_CORE_EOM_HEADER_SIZE;

  ctx->state_samples_remaining[SAMETHING_CORE_GEN_STATE_SILENCE_FIRST] =
  ctx->state_samples_remaining[SAMETHING_CORE_GEN_STATE_SILENCE_SECOND] =
  ctx->state_samples_remaining[SAMETHING_CORE_GEN_STATE_SILENCE_THIRD] =
  ctx->state_samples_remaining[SAMETHING_CORE_GEN_STATE_SILENCE_FOURTH] =
  ctx->state_samples_remaining[SAMETHING_CORE_GEN_STATE_SILENCE_FIFTH] =
  ctx->state_samples_remaining[SAMETHING_CORE_GEN_STATE_SILENCE_SIXTH] =
  ctx->state_samples_remaining[SAMETHING_CORE_GEN_STATE_SILENCE_SEVENTH] =
  SAMETHING_CORE_SILENCE_DURATION * SAMETHING_CORE_SAMPLE_RATE;

  ctx->state_samples_remaining[SAMETHING_CORE_GEN_STATE_ATTENTION_SIGNAL] =
  header->attn_sig_duration * SAMETHING_CORE_SAMPLE_RATE;

  // clang-format on

  for (int i = 0; i < SAMETHING_CORE_GEN_STATE_NUM; ++i) {
    ctx->samples_num_max += ctx->state_samples_remaining[i];
  }
}

void samething_core_samples_gen(struct samething_core_gen_ctx *const ctx) {
  static void (*gen_table[])(struct samething_core_gen_ctx *const,
                             const size_t) = {
      &samething_core_afsk_gen_header,  // First AFSK burst of header
      &samething_core_silence_gen,      // 1 second of silence
      &samething_core_afsk_gen_header,  // Second AFSK burst of header
      &samething_core_silence_gen,      // 1 second of silence
      &samething_core_afsk_gen_header,  // Third AFSK burst of header
      &samething_core_silence_gen,      // 1 second of silence
      &samething_core_attn_sig_gen,     // Attention signal for 8..25 seconds
      &samething_core_silence_gen,      // 1 second of silence
      &samething_core_afsk_gen_eom,     // First AFSK burst of EOM
      &samething_core_silence_gen,      // 1 second of silence
      &samething_core_afsk_gen_eom,     // Second AFSK burst of EOM
      &samething_core_silence_gen,      // 1 second of silence
      &samething_core_afsk_gen_eom,     // Third AFSK burst of EOM
      &samething_core_silence_gen,      // 1 second of silence
  };

  if (ctx->gen_state >= SAMETHING_CORE_GEN_STATE_NUM) {
    return;
  }

  // Generate only SAMETHING_CORE_SAMPLES_NUM_MAX.
  for (int sample_count = 0; sample_count < SAMETHING_CORE_SAMPLES_NUM_MAX;) {
    if (ctx->state_samples_remaining[ctx->gen_state] <= 0) {
      // Move onto the next generation in the sequence
      ctx->gen_state++;

      if (ctx->gen_state == SAMETHING_CORE_GEN_STATE_NUM) {
        // We're done generating.
        return;
      }
    }

    int num_samples = ctx->state_samples_remaining[ctx->gen_state];

    if (num_samples >= SAMETHING_CORE_SAMPLES_NUM_MAX) {
      num_samples = SAMETHING_CORE_SAMPLES_NUM_MAX;
    }

    gen_table[ctx->gen_state](ctx, num_samples);
    ctx->state_samples_remaining[ctx->gen_state] -= num_samples;

    sample_count += num_samples;
  }
}
