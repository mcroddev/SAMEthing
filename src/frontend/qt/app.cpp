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

#include "app.h"

#include <QFile>
#include <QMessageBox>

#include "core/core.h"
#include "frontend_audio/audio.h"

SAMEthingApp::SAMEthingApp() noexcept {
  DatabaseLoad();
  AudioInitialize();
  main_window_controller_.show();
}

void SAMEthingApp::AudioInitialize() noexcept {
  if (samething_audio_init() != SAMETHING_AUDIO_OK) {
    QMessageBox::critical(nullptr, tr("Audio failure"), tr("No audio."));
    return;
  }

  char audio_devices[SAMETHING_AUDIO_DEVICES_NUM_MAX]
                    [SAMETHING_AUDIO_DEVICE_NAME_LEN_MAX];
  enum samething_audio_return_codes code;

  const size_t num_devices = samething_audio_devices_get(audio_devices, &code);

  if (code != SAMETHING_AUDIO_OK) {
    QMessageBox::warning(nullptr, tr("Error with audio"), tr("replace me"));
    return;
  }

  for (size_t i = 0; i < num_devices; ++i) {
    QString audio_device_name(audio_devices[i]);
    main_window_controller_.AudioDeviceAdd(audio_device_name);
  }
}

void SAMEthingApp::DatabaseLoad() noexcept {
  QFile database_file("samething_db.txt");

  if (!database_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QMessageBox::critical(nullptr, tr("Unable to open database"),
                          tr("replace me"));
  }
}
