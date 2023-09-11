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

#include "frontend_audio/audio.h"

enum samething_audio_return_codes samething_audio_init(void) {
  if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
    return SAMETHING_AUDIO_INIT_FAILED;
  }
  return SAMETHING_AUDIO_OK;
}

void samething_audio_shutdown(void) {
  if (!SDL_WasInit(SDL_INIT_AUDIO)) {
    // Shouldn't be called if audio wasn't initialized to begin with...
    return;
  }
  SDL_Quit();
}

const char *samething_audio_error_get(void) { return SDL_GetError(); }

size_t samething_audio_devices_get(
    char devices[SAMETHING_AUDIO_DEVICES_NUM_MAX]
                [SAMETHING_AUDIO_DEVICE_NAME_LEN_MAX],
    enum samething_audio_return_codes *const code) {
  const int audio_device_count = SDL_GetNumAudioDevices(0);

  if (audio_device_count < 0) {
    // This function will return -1 if an explicit list of devices can't be
    // determined. Returning -1 is not an error. For example, if SDL is set up
    // to talk to a remote audio server, it can't list every one available on
    // the Internet, but it will still allow a specific host to be specified in
    // SDL_OpenAudioDevice().
    //
    // XXX: This isn't the case here, so we go ahead and say that we didn't find
    // any audio devices.
    *code = SAMETHING_AUDIO_DEVICES_NOT_FOUND;
    return 0;
  }

  for (int audio_device_id = 0; audio_device_id < audio_device_count;
       ++audio_device_id) {
    const char *device_name = SDL_GetAudioDeviceName(audio_device_id, 0);

    if (device_name == NULL) {
      *code = SAMETHING_AUDIO_ENUMERATION_FAILED;
      return 0;
    }

    strncpy(devices[audio_device_id], device_name,
            SAMETHING_AUDIO_DEVICE_NAME_LEN_MAX);
  }
  *code = SAMETHING_AUDIO_OK;
  return audio_device_count;
}

enum samething_audio_return_codes samething_audio_buffer_play(
    const struct samething_audio_device *const dev, const int16_t *const buffer,
    const size_t buffer_size) {
  const SDL_AudioDeviceID id = (SDL_AudioDeviceID)(uintptr_t)dev->id;

  const int res = SDL_QueueAudio(id, buffer, sizeof(int16_t) * buffer_size);

  if (res < 0) {
    return SAMETHING_AUDIO_DEVICE_QUEUE_ERROR;
  }
  return SAMETHING_AUDIO_OK;
}

enum samething_audio_return_codes samething_audio_open_device(
    const char *const name, struct samething_audio_device *const dev,
    const struct samething_audio_spec *const audio_spec) {
  SDL_AudioSpec spec;
  SDL_zero(spec);

  spec.freq = audio_spec->sample_rate;
  spec.samples = audio_spec->samples;
  spec.channels = 1;

  switch (audio_spec->format) {
    case SAMETHING_AUDIO_FORMAT_S16:
      spec.format = AUDIO_S16LSB;
      break;

    default:
      // assert
      break;
  }

  const SDL_AudioDeviceID audio_device =
      SDL_OpenAudioDevice(name, 0, &spec, NULL, 0);

  if (audio_device == 0) {
    return SAMETHING_AUDIO_DEVICE_CANNOT_OPEN;
  }

  dev->id = (void *)(uintptr_t)audio_device;
  strncpy(dev->name, name, SAMETHING_AUDIO_DEVICE_NAME_LEN_MAX);

  /// Enable the audio device.
  SDL_PauseAudioDevice(audio_device, 0);

  return SAMETHING_AUDIO_OK;
}
