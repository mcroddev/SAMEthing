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

#ifndef SAMETHING_FRONTEND_COMMON_AUDIO_H
#define SAMETHING_FRONTEND_COMMON_AUDIO_H

#pragma once

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

#include <stdint.h>

/// The maximum number of playback audio devices that we support.
#define SAMETHING_AUDIO_DEVICES_NUM_MAX (8)

/// The maximum length an audio device name can be.
#define SAMETHING_AUDIO_DEVICE_NAME_LEN_MAX (128)

/// Defines return codes for error
enum samething_audio_return_codes {
  /// No error.
  SAMETHING_AUDIO_OK,

  /// Error initializing the audio system.
  SAMETHING_AUDIO_INIT_FAILED,

  /// Error enumerating over output devices.
  SAMETHING_AUDIO_ENUMERATION_FAILED,

  /// No output devices found.
  SAMETHING_AUDIO_DEVICES_NOT_FOUND,

  SAMETHING_AUDIO_DEVICE_CANNOT_OPEN,

  SAMETHING_AUDIO_DEVICE_QUEUE_ERROR
};

/// Defines an audio device.
struct samething_audio_device {
  /// The actual name of the audio device.
  char name[SAMETHING_AUDIO_DEVICE_NAME_LEN_MAX];

  /// Native handle to the audio device, whatever that might be. This is not for
  /// your use; it is internal to the audio module.
  void *id;
};

/// Defines the audio formats we support.
enum samething_audio_format {
  /// Signed 16-bit samples in little-endian byte order.
  SAMETHING_AUDIO_FORMAT_S16
};

/// Defines the specification of the audio we plan to submit to the audio
/// device.
struct samething_audio_spec {
  /// The number of samples per second.
  int sample_rate;

  /// The size of the audio buffer in sample frames. This must be a power of 2.
  uint16_t samples;

  /// The audio format we are sending to the audio device.
  enum samething_audio_format format;
};

/// Initializes the audio module.
///
/// @returns SAMETHING_AUDIO_OK if successful, or any other code otherwise.
/// Refer to samething_audio_return_codes for more details.
enum samething_audio_return_codes samething_audio_init(void);

/// Shuts down the audio module.
void samething_audio_shutdown(void);

const char *samething_audio_error_get(void);

/// Enumerates over the output devices.
size_t samething_audio_devices_get(
    char devices[SAMETHING_AUDIO_DEVICES_NUM_MAX]
                [SAMETHING_AUDIO_DEVICE_NAME_LEN_MAX],
    enum samething_audio_return_codes *const code);

enum samething_audio_return_codes samething_audio_open_device(
    const char *const name, struct samething_audio_device *const dev,
    const struct samething_audio_spec *const audio_spec);

enum samething_audio_return_codes samething_audio_buffer_play(
    const struct samething_audio_device *const dev, const int16_t *const buffer,
    const size_t buffer_size);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SAMETHING_FRONTEND_COMMON_AUDIO_H
