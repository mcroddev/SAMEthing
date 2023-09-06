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

#include <SDL2/SDL.h>

#include "core/core.h"

int main(void) {
  SDL_Init(SDL_INIT_AUDIO);

  SDL_AudioDeviceID dev;

  SDL_AudioSpec audio_spec;
  SDL_zero(audio_spec);

  audio_spec.freq = SAMETHING_CORE_SAMPLE_RATE;
  audio_spec.format = AUDIO_S16LSB;
  audio_spec.channels = 1;
  audio_spec.samples = 1024;

  dev = SDL_OpenAudioDevice(NULL, 0, &audio_spec, NULL, 0);

  if (dev == 0) {
    perror("SDL_OpenAudioDevice()");
    return EXIT_FAILURE;
  }
  printf("Using %s\n", SDL_GetAudioDeviceName(dev, 0));

  struct samething_core_header header = {
      .location_codes = {"101010", SAMETHING_CORE_LOCATION_CODE_END_MARKER},
      .originator_time = "39393933",
      .valid_time_period = "1111",
      .event_code = "EAN",
      .originator_code = "RMT",
      .attn_sig_duration = 8,
      .id = "ZID/HEAT "};

  struct samething_core_gen_ctx ctx;
  memset(&ctx, 0, sizeof(ctx));

  samething_core_ctx_config(&ctx, &header);

  int num_samples = 0;

  SDL_PauseAudioDevice(dev, 0);

  while (num_samples < ctx.samples_num_max) {
    samething_core_samples_gen(&ctx);
    num_samples += SAMETHING_CORE_SAMPLES_NUM_MAX;
    SDL_QueueAudio(dev, ctx.data, sizeof(int16_t) * SAMETHING_CORE_SAMPLES_NUM_MAX);
  }
  SDL_Delay(25000);
  return EXIT_SUCCESS;
}
