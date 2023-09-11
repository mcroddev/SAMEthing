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

#include "main_window.h"

#include <QMessageBox>

#include "core/core.h"

MainWindowController::MainWindowController() noexcept {
  ui_.setupUi(this);

  ui_.attn_sig_duration_->setMinimum(SAMETHING_CORE_ATTN_SIG_MIN);
  ui_.attn_sig_duration_->setMaximum(SAMETHING_CORE_ATTN_SIG_MAX);

  ui_.callsign_->setMaxLength(SAMETHING_CORE_ID_LEN_MAX);

  SignalsConnectToSlots();
}

void MainWindowController::SignalsConnectToSlots() noexcept {
  connect(ui_.actionAbout_Qt, &QAction::triggered,
          [this]() { QMessageBox::aboutQt(this); });
}

void MainWindowController::AudioDeviceAdd(QString &audio_device_name) noexcept {
  ui_.audio_device_list_->addItem(audio_device_name);
}

auto MainWindowController::SelectedAudioDeviceGetName() const noexcept
    -> QString {
  return ui_.audio_device_list_->currentText();
}
