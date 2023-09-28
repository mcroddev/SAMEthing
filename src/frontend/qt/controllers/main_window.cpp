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
#include <QTreeWidgetItem>

#include "samething/core.h"

MainWindowController::MainWindowController() noexcept {
  ui_.setupUi(this);

  ui_.attn_sig_duration_->setMinimum(SAMETHING_CORE_ATTN_SIG_MIN);
  ui_.attn_sig_duration_->setMaximum(SAMETHING_CORE_ATTN_SIG_MAX);

  ui_.callsign_->setMaxLength(SAMETHING_CORE_ID_LEN_MAX);
  ui_.callsign_->setValidator(&callsign_validator_);

  // Set the default originator time to be the current time to avoid the default
  // being 2000/1/1 0000.
  ui_.org_time_->setDateTime(QDateTime::currentDateTime());

  callsign_validator_.setRegularExpression(
      QRegularExpression("[/,A-Z,a-z, ]+"));
  ui_.callsign_->setValidator(&callsign_validator_);

  QFont f = ui_.callsign_->font();
  f.setCapitalization(QFont::AllUppercase);
  ui_.callsign_->setFont(f);

  SignalsConnectToSlots();
}

void MainWindowController::AudioDeviceAdd(
    const QString& audio_device_name) noexcept {
  ui_.audio_device_list_->addItem(audio_device_name);
}

auto MainWindowController::SelectedAudioDeviceNameGet() const noexcept
    -> QString {
  return ui_.audio_device_list_->currentText();
}

void MainWindowController::LocationCodeAdd(const LocationCodeData& loc_code,
                                           const QString& county_subdivision,
                                           const QString& state,
                                           const QString& county) noexcept {
  auto* item = new QTreeWidgetItem(ui_.location_codes_);
  item->setText(0, county_subdivision);
  item->setText(1, state);
  item->setText(2, county);

  QVariant var;
  var.setValue(loc_code);
  item->setData(0, Qt::UserRole, var);
}

[[nodiscard]] auto MainWindowController::LocationCodeDataGet(
    const int loc_index) const noexcept -> LocationCodeData {
  auto* item = ui_.location_codes_->topLevelItem(loc_index);
  return qvariant_cast<LocationCodeData>(item->data(0, Qt::UserRole));
}

[[nodiscard]] auto MainWindowController::LocationCodeCountGet() const noexcept
    -> int {
  return ui_.location_codes_->topLevelItemCount();
}

void MainWindowController::OriginatorCodeAdd(const QString& name) noexcept {
  ui_.org_code_->addItem(name);
}

void MainWindowController::EventCodeAdd(const QString& name) noexcept {
  ui_.event_code_->addItem(name);
}

void MainWindowController::SignalsConnectToSlots() noexcept {
  connect(ui_.actionAbout_Qt, &QAction::triggered,
          [this]() { QMessageBox::aboutQt(this); });

  connect(ui_.play_button_, &QPushButton::clicked,
          [this]() { emit SAMEHeaderPlay(); });

  connect(ui_.add_location_code_button_, &QPushButton::clicked,
          [this]() { emit LocationCodeDialogAddShow(); });
}

[[nodiscard]] auto MainWindowController::CallsignGet() const noexcept
    -> QString {
  QString callsign = ui_.callsign_->text().toUpper();
  qsizetype callsign_len = callsign.length();

  if (!callsign.isEmpty() && callsign_len < SAMETHING_CORE_ID_LEN_MAX) {
    callsign_len = SAMETHING_CORE_ID_LEN_MAX - callsign_len;

    while (callsign_len--) {
      callsign += ' ';
    }
  }
  return callsign;
}

[[nodiscard]] auto MainWindowController::ValidTimePeriodGet() const noexcept
    -> QString {
  QString time_period;

  time_period += QString::number(ui_.valid_time_period_->time().hour())
                     .rightJustified(2, '0');
  time_period += QString::number(ui_.valid_time_period_->time().minute())
                     .rightJustified(2, '0');

  return time_period;
}

[[nodiscard]] auto MainWindowController::OriginatorCodeGet() const noexcept
    -> int {
  return ui_.org_code_->currentIndex();
}

[[nodiscard]] auto MainWindowController::OriginatorTimeGet() const noexcept
    -> QString {
  QString org_time;

  org_time +=
      QString::number(ui_.org_time_->date().dayOfYear()).rightJustified(3, '0');
  org_time +=
      QString::number(ui_.org_time_->time().hour()).rightJustified(2, '0');
  org_time +=
      QString::number(ui_.org_time_->time().minute()).rightJustified(2, '0');

  return org_time;
}

[[nodiscard]] auto MainWindowController::EventCodeGet() const noexcept -> int {
  return ui_.event_code_->currentIndex();
}

[[nodiscard]] auto MainWindowController::AttentionSignalDurationGet()
    const noexcept -> int {
  return ui_.attn_sig_duration_->value();
}
