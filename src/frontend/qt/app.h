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

#ifndef SAMETHING_FRONTEND_QT_APP_H
#define SAMETHING_FRONTEND_QT_APP_H

#pragma once

#include <QObject>

#include "controllers/location_code_editor.h"
#include "controllers/main_window.h"
#include "samething/core.h"
#include "samething/frontend/audio.h"
#include "samething/frontend/database.h"

/// Defines the main application class, which is essentially interconnectivity
/// between models, views, and controllers.
class SAMEthingApp final : public QObject {
  Q_OBJECT

 public:
  SAMEthingApp() noexcept;

#ifndef NDEBUG
  void DebugAssertionEncountered(const char* const expr,
                                 const char* const file_name,
                                 const int line_no) noexcept;
#endif  // NDEBUG

  void Initialize() noexcept;

 private:
  /// Populates various fields of the main window.
  void MainWindowPopulateFields() noexcept;

  /// Populates various fields of the location code editor.
  void LocationCodeEditorPopulateFields() noexcept;

  /// Initializes the audio module.
  ///
  /// If initialization fails, the generation of the SAME header to a WAV file
  /// is still possible.
  void AudioInitialize() noexcept;

  /// Initializes the database module.
  ///
  /// If initialization fails, the program will fatally terminate.
  void DatabaseLoad() noexcept;

  void SignalsConnectToSlots() noexcept;

  void SAMEHeaderPopulate(samething_core_header& header) noexcept;
  void SAMEHeaderPlay(samething_core_header& header) noexcept;

  MainWindowController main_window_controller_;
  LocationCodeEditorDialogController location_code_editor_;

  samething_db model_db_;
};

#endif  // SAMETHING_FRONTEND_QT_APP_H
