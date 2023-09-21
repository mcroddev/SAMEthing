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
#include "frontend_database/database.h"

SAMEthingApp::SAMEthingApp() noexcept : model_db_({}) {
  DatabaseLoad();
  AudioInitialize();
  SignalsConnectToSlots();
  MainWindowPopulateFields();
  LocationCodeEditorPopulateFields();

  main_window_controller_.show();
}

void SAMEthingApp::MainWindowPopulateFields() noexcept {
  std::size_t i;

  for (i = 0; i < model_db_.org_code.num_entries; ++i) {
    const QString org_code(model_db_.org_code.entries[i].desc);
    main_window_controller_.OriginatorCodeAdd(org_code);
  }

  for (i = 0; i < model_db_.event_code.num_entries; ++i) {
    const QString event_code(model_db_.event_code.entries[i].desc);
    main_window_controller_.EventCodeAdd(event_code);
  }
}

void SAMEthingApp::LocationCodeEditorPopulateFields() noexcept {
  std::size_t i;

  for (i = 0; i < model_db_.county_subdivisions.num_entries; ++i) {
    const QString str(model_db_.county_subdivisions.entries[i].desc);
    location_code_editor_.CountySubdivisionAdd(str);
  }

  for (i = 0; i < model_db_.state_county_map.num_states; ++i) {
    const QString str(model_db_.state_county_map.states[i].name);
    location_code_editor_.StateAdd(str);
  }
}

void SAMEthingApp::DatabaseLoad() noexcept {
  QFile database_file("samething_db.txt");

  if (!database_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    const QString error_str =
        QString("Unable to load the database file samething_db.txt: %1.")
            .arg(database_file.errorString());

    QMessageBox::critical(nullptr, tr("Database read error"), error_str);
    return;
  }

  QTextStream stream(&database_file);

  // This function is called by samething_db_read() to read another line of the
  // file.
  constexpr auto line_read_func = [](char* str, int num,
                                     void* stream) noexcept -> char* {
    auto* reader = static_cast<QTextStream*>(stream);

    if (reader->atEnd()) {
      // No more lines to read; we're done.
      return nullptr;
    }

    const char* const line = reader->readLine(num).toUtf8().constData();
    std::memcpy(str, line, num);

    return str;
  };

  if (!samething_db_read(&model_db_, line_read_func, &stream)) {
    // QMessageBox::critical(nullptr, tr("Database parse error"), error_str);
    qApp->exit();
  }
}

void SAMEthingApp::SignalsConnectToSlots() noexcept {
  connect(&main_window_controller_, &MainWindowController::SAMEHeaderPlay,
          [this]() {
            if (!samething_audio_is_init()) {
              // This button should've been disabled...
              return;
            }

            samething_core_header header = {};
            SAMEHeaderPopulate(header);
            SAMEHeaderPlay(header);
          });

  connect(&main_window_controller_,
          &MainWindowController::LocationCodeDialogAddShow,
          [this]() { location_code_editor_.show(); });

  connect(&location_code_editor_,
          &LocationCodeEditorDialogController::StateChanged,
          [this](const int num) {
            location_code_editor_.CountyListClear();

            for (size_t i = 0;
                 i < model_db_.state_county_map.states[num].num_counties; ++i) {
              location_code_editor_.CountyAdd(
                  model_db_.state_county_map.states[num].counties[i].name);
            }
          });

  connect(&location_code_editor_, &LocationCodeEditorDialogController::NewEntry,
          [this](const int county_subdivision_index, const int state_index,
                 const int county_index) {
            LocationCodeData info;
            info.county_subdivision_index = county_subdivision_index;
            info.state_index = state_index;
            info.county_index = county_index;

            char* county_subdivision_name =
                model_db_.county_subdivisions.entries[county_subdivision_index]
                    .desc;

            char* state_name =
                model_db_.state_county_map.states[state_index].name;
            char* county_name = model_db_.state_county_map.states[state_index]
                                    .counties[county_index]
                                    .name;

            main_window_controller_.LocationCodeAdd(
                info, county_subdivision_name, state_name, county_name);
          });
}

void SAMEthingApp::SAMEHeaderPopulate(samething_core_header& header) noexcept {
  header.attn_sig_duration =
      main_window_controller_.AttentionSignalDurationGet();

  const char* const callsign =
      main_window_controller_.CallsignGet().toUtf8().constData();
  std::memcpy(header.id, callsign, SAMETHING_CORE_ID_LEN_MAX);

  const char* const valid_time_period =
      main_window_controller_.ValidTimePeriodGet().toUtf8().constData();
  std::memcpy(header.valid_time_period, valid_time_period,
              SAMETHING_CORE_VALID_TIME_PERIOD_LEN_MAX);

  const int org_code = main_window_controller_.OriginatorCodeGet();
  std::memcpy(header.originator_code, model_db_.org_code.entries[org_code].code,
              SAMETHING_DB_ORG_CODE_LEN_MAX);

  const char* const org_time =
      main_window_controller_.OriginatorTimeGet().toUtf8().constData();
  std::memcpy(header.originator_time, org_time,
              SAMETHING_CORE_ORIGINATOR_TIME_LEN_MAX);

  const int event_code = main_window_controller_.EventCodeGet();
  std::memcpy(header.event_code, model_db_.event_code.entries[event_code].code,
              SAMETHING_DB_EVENT_CODE_LEN_MAX);
}

void SAMEthingApp::SAMEHeaderPlay(samething_core_header& header) noexcept {
  samething_core_gen_ctx ctx = {};
  samething_core_ctx_config(&ctx, &header);

  unsigned int sample_cnt = 0;
  while (sample_cnt < ctx.samples_num_max) {
    samething_core_samples_gen(&ctx);
    sample_cnt += SAMETHING_CORE_SAMPLES_NUM_MAX;
  }
}

void SAMEthingApp::AudioInitialize() noexcept {
  if (!samething_audio_init()) {
    const char* const str = samething_audio_error_get();

    const QString error_str =
        QString(
            "Unable to initialize the audio module: %1. You will be able to "
            "save SAME headers to WAV files, but you will not be able to play "
            "them through the application unless you either restart the "
            "application, or try to reinitialize the audio module.")
            .arg(str);

    QMessageBox::warning(nullptr, tr("Audio init failure"), error_str);
    return;
  }

  char audio_devices[SAMETHING_AUDIO_DEVICES_NUM_MAX]
                    [SAMETHING_AUDIO_DEVICE_NAME_LEN_MAX];
  size_t num_devices = 0;

  if (!samething_audio_devices_get(audio_devices, &num_devices)) {
    const char* const str = samething_audio_error_get();

    const QString error_str =
        QString(
            "Unable to retrieve a list of output devices: %1. You will be able "
            "to save SAME headers to WAV files, but you will not be able to "
            "play them through the application unless you either restart the "
            "application, or try to rescan for output devices.")
            .arg(str);

    QMessageBox::warning(nullptr, tr("Error with audio"), error_str);
    return;
  }

  for (size_t i = 0; i < num_devices; ++i) {
    const QString audio_device_name(audio_devices[i]);
    main_window_controller_.AudioDeviceAdd(audio_device_name);
  }
}
