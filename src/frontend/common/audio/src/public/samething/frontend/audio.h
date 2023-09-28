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

#include <stdbool.h>
#include <stdint.h>

/// The maximum number of playback audio devices that we support.
#define SAMETHING_AUDIO_DEVICES_NUM_MAX (8)

/// The maximum length an audio device name can be.
#define SAMETHING_AUDIO_DEVICE_NAME_LEN_MAX (128)

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

  /// The audio format we are sending to the audio device.
  enum samething_audio_format format;

  /// The size of the audio buffer in sample frames. This must be a power of 2.
  uint16_t samples;
};

/// Initializes the audio module.
///
/// @returns true if the module was successfully initialized, or false
/// otherwise.
bool samething_audio_init(void);

/// Checks to see if the audio module is initialized.
///
/// @returns true if the module is initialized, or false otherwise.
bool samething_audio_is_init(void);

/// Shuts down the audio module.
void samething_audio_shutdown(void);

/// Retrieves the error string corresponding to the last failed operation.
///
/// @returns The error string corresponding to the last failed operation.
const char *samething_audio_error_get(void);

/// Enumerates over the output devices.
///
/// @param devices The 2D array in which to store the names of the output
/// devices that were found.
/// @param num_devices The number of devices that were found.
/// @returns true if this operation was successful, or false otherwise.
bool samething_audio_devices_get(
    char devices[SAMETHING_AUDIO_DEVICES_NUM_MAX]
                [SAMETHING_AUDIO_DEVICE_NAME_LEN_MAX],
    size_t *num_devices);

/// Opens the specified device.
///
/// @param name The actual name of the audio device.
/// @param dev The audio device structure to populate.
/// @param audio_spec Information about the audio we plan to feed to the device
/// @returns true if this operation was successful, or false otherwise.
bool samething_audio_open_device(
    const char *const name, struct samething_audio_device *const dev,
    const struct samething_audio_spec *const audio_spec);

/// Sends audio data to the device.
///
/// @param dev The audio device to feed data to.
/// @param buffer The audio data to feed.
/// @param buffer_size The size of the audio data.
/// @returns true if this operation was successful, or false otherwise.
bool samething_audio_buffer_play(const struct samething_audio_device *const dev,
                                 const int16_t *const buffer,
                                 const size_t buffer_size);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SAMETHING_FRONTEND_COMMON_AUDIO_H
