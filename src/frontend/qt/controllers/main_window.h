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

#ifndef SAMETHING_CONTROLLERS_MAIN_WINDOW_H
#define SAMETHING_CONTROLLERS_MAIN_WINDOW_H

#pragma once

#include <QMainWindow>
#include <QRegularExpressionValidator>

#include "ui_main_window.h"

/// Defines data that is saved within each location code item.
struct LocationCodeData {
  /// The index pointing to the specified county subdivision in the database.
  int county_subdivision_index;

  /// The index pointing to the specified state in the database.
  int state_index;

  /// The index pointing to the specified county in the state in the database.
  int county_index;
};
Q_DECLARE_METATYPE(LocationCodeData);

/// Defines the controller for the main window view.
class MainWindowController final : public QMainWindow {
  Q_OBJECT

 public:
  MainWindowController() noexcept;

  /// Adds an audio device name to the list of selectable audio devices.
  ///
  /// @param audio_device_name The audio device name to add.
  void AudioDeviceAdd(const QString& audio_device_name) noexcept;

  /// Returns the name of the audio device currently specified by the audio
  /// devices combo box.
  ///
  /// @returns The name of the audio device currently specified by the audio
  /// devices combo box.
  [[nodiscard]] auto SelectedAudioDeviceNameGet() const noexcept -> QString;

  /// Adds a location code to the location code widget.
  ///
  /// @param loc_code The data containing the indices of each field in the
  /// database.
  /// @param county_subdivision The description of the county subdivision.
  /// @param state The full name of a state.
  /// @param county The full name of a county.
  void LocationCodeAdd(const LocationCodeData& loc_code,
                       const QString& county_subdivision, const QString& state,
                       const QString& county) noexcept;

  /// Retrieves the number of location codes specified.
  ///
  /// @returns The number of location codes specified.
  [[nodiscard]] auto LocationCodeCountGet() const noexcept -> int;

  /// Retrieves the location database data stored in an item.
  ///
  /// @param loc_index The index pointing to a specific item.
  ///
  /// @returns The database data.
  [[nodiscard]] auto LocationCodeDataGet(const int loc_index) const noexcept
      -> LocationCodeData;

  /// Retrieves the value of the valid time period.
  ///
  /// @returns The value of the valid time period.
  [[nodiscard]] auto ValidTimePeriodGet() const noexcept -> QString;

  /// Retrieves the index of the currently selected originator code; this
  /// corresponds to the originator code in the database.
  ///
  /// @returns The index of the currently selected originator code in the combo
  /// box.
  [[nodiscard]] auto OriginatorCodeGet() const noexcept -> int;

  /// Retrieves the value of the originator time.
  ///
  /// @returns The value of the originator time.
  [[nodiscard]] auto OriginatorTimeGet() const noexcept -> QString;

  /// Retrieves the index of the currently selected event code in the combo box;
  /// this corresponds to the event code in the database.
  ///
  /// @returns The index of the currently selected event code in the combo box.
  [[nodiscard]] auto EventCodeGet() const noexcept -> int;

  /// Retrieves the value of the callsign.
  ///
  /// @returns The value of the callsign.
  [[nodiscard]] auto CallsignGet() const noexcept -> QString;

  /// Retrieves the value of the attention signal.
  ///
  /// @returns The value of the attention signal.
  [[nodiscard]] auto AttentionSignalDurationGet() const noexcept -> int;

  /// Adds an originator code to the originator code combo box.
  ///
  /// @param name The description of the originator code.
  void OriginatorCodeAdd(const QString& name) noexcept;

  /// Adds an event code to the event code combo box.
  ///
  /// @param name The description of the event code.
  void EventCodeAdd(const QString& name) noexcept;

 private:
  /// Connects signals to slots.
  void SignalsConnectToSlots() noexcept;

  /// The UI as generated by Qt Designer.
  Ui::MainWindow ui_;

  QRegularExpressionValidator callsign_validator_;

 signals:
  /// Emitted when the user requests to add a location code.
  void LocationCodeDialogAddShow();

  /// Emitted when the user requests to delete a location code.
  void LocationCodeDelete();

  /// Emitted when the user requests to play the configured SAME header.
  void SAMEHeaderPlay();

  /// Emitted when the user requests to stop playback of the configured SAME
  /// header.
  void SAMEHeaderStop();
};

#endif  // SAMETHING_CONTROLLERS_MAIN_WINDOW_H
