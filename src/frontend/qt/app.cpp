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

#include "samething/core.h"
#include "samething/frontend/audio.h"
#include "samething/frontend/database.h"

SAMEthingApp::SAMEthingApp() noexcept : model_db_({}) {}

#ifndef NDEBUG
void SAMEthingApp::DebugAssertionEncountered(const char* const expr,
                                             const char* const file_name,
                                             const int line_no) noexcept {
  const QString error_msg = QString(
                                "Debug assertion '%1' failed.\n"
                                "File: %2\n"
                                "Line: %3")
                                .arg(expr, file_name, QString::number(line_no));

  QMessageBox box;
  box.setIcon(QMessageBox::Critical);
  box.addButton(QMessageBox::Abort);
  box.addButton(QMessageBox::Ignore);
  box.setWindowTitle(tr("Debug assertion!"));
  box.setText(error_msg);

  switch (box.exec()) {
    case QMessageBox::Abort:
      abort();
      return;

    case QMessageBox::Ignore:
    default:
      return;
  }
}
#endif  // NDEBUG

void SAMEthingApp::Initialize() noexcept {
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
  const QString file_name = QString("db_%1.ini").arg(QLocale::system().name());
  QFile database_file(file_name);

  if (!database_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    const QString error_str =
        QString("Unable to load the database file %1: %2.")
            .arg(file_name, database_file.errorString());

    QMessageBox::critical(nullptr, tr("Database read error"), error_str);
    return;
  }

  constexpr auto line_read_func = [](char* str, int num,
                                     void* stream) noexcept -> char* {
    auto* reader = static_cast<QTextStream*>(stream);

    if (reader->atEnd()) {
      // No more lines to read; we're done.
      return nullptr;
    }
    std::memset(str, 0, static_cast<std::size_t>(num));

    const QString line = reader->readLine(num);
    std::memcpy(str, line.toUtf8().constData(),
                static_cast<std::size_t>(line.length()));
    return str;
  };

  QTextStream stream(&database_file);

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

            if (main_window_controller_.LocationCodeCountGet() <= 0) {
              QMessageBox::warning(
                  &main_window_controller_, tr("No location codes"),
                  tr("You must specify at least one location code."));
              return;
            }

            if (main_window_controller_.CallsignGet().isEmpty()) {
              QMessageBox::warning(&main_window_controller_, tr("No callsign"),
                                   tr("You must specify a callsign."));
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
  header.attn_sig_duration = static_cast<unsigned int>(
      main_window_controller_.AttentionSignalDurationGet());

  std::memcpy(header.callsign,
              main_window_controller_.CallsignGet().toUtf8().constData(),
              SAMETHING_CORE_CALLSIGN_LEN);

  std::memcpy(header.valid_time_period,
              main_window_controller_.ValidTimePeriodGet().toUtf8().constData(),
              SAMETHING_CORE_VALID_TIME_PERIOD_LEN);

  const int org_code = main_window_controller_.OriginatorCodeGet();
  std::memcpy(header.originator_code, model_db_.org_code.entries[org_code].code,
              SAMETHING_DB_ORG_CODE_LEN_MAX);

  std::memcpy(header.originator_time,
              main_window_controller_.OriginatorTimeGet().toUtf8().constData(),
              SAMETHING_CORE_ORIGINATOR_TIME_LEN);

  const int event_code = main_window_controller_.EventCodeGet();
  std::memcpy(header.event_code, model_db_.event_code.entries[event_code].code,
              SAMETHING_DB_EVENT_CODE_LEN_MAX);

  const int location_code_count =
      main_window_controller_.LocationCodeCountGet();

  int loc_code;
  for (loc_code = 0; loc_code < location_code_count; ++loc_code) {
    const LocationCodeData loc_data =
        main_window_controller_.LocationCodeDataGet(loc_code);

    size_t ctr = 0;
    std::memcpy(
        &header.location_codes[loc_code][ctr],
        model_db_.county_subdivisions.entries[loc_data.county_subdivision_index]
            .code,
        SAMETHING_DB_COUNTY_SUBDIVISION_LEN_MAX);
    ctr += SAMETHING_DB_COUNTY_SUBDIVISION_LEN_MAX;

    std::memcpy(&header.location_codes[loc_code][ctr],
                model_db_.state_county_map.states[loc_data.state_index].code,
                SAMETHING_DB_STATE_CODE_LEN_MAX);
    ctr += SAMETHING_DB_STATE_CODE_LEN_MAX;

    std::memcpy(&header.location_codes[loc_code][ctr],
                model_db_.state_county_map.states[loc_data.state_index]
                    .counties[loc_data.county_index]
                    .code,
                SAMETHING_DB_COUNTY_CODE_LEN_MAX);
  }
  std::memcpy(&header.location_codes[loc_code],
              SAMETHING_CORE_LOCATION_CODE_END_MARKER,
              SAMETHING_CORE_LOCATION_CODE_LEN);
}

void SAMEthingApp::SAMEHeaderPlay(samething_core_header& header) noexcept {
  samething_audio_spec spec = {};
  spec.format = SAMETHING_AUDIO_FORMAT_S16;
  spec.sample_rate = SAMETHING_CORE_SAMPLE_RATE;
  spec.samples = SAMETHING_CORE_SAMPLES_NUM_MAX;

  samething_audio_device dev = {};

  if (!samething_audio_open_device(
          main_window_controller_.SelectedAudioDeviceNameGet()
              .toUtf8()
              .constData(),
          &dev, &spec)) {
    // Error!
    return;
  }

  samething_core_gen_ctx ctx = {};
  samething_core_ctx_init(&ctx, &header);

  while (ctx.seq_state != SAMETHING_CORE_SEQ_STATE_NUM) {
    samething_core_samples_gen(&ctx);

    if (!samething_audio_buffer_play(&dev, ctx.sample_data,
                                     SAMETHING_CORE_SAMPLES_NUM_MAX)) {
      // Error!
      return;
    }
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
