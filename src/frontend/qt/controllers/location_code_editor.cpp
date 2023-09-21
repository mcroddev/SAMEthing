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

#include "location_code_editor.h"

LocationCodeEditorDialogController::
    LocationCodeEditorDialogController() noexcept
    : ui_({}) {
  ui_.setupUi(this);
  SignalsConnectToSlots();
}

void LocationCodeEditorDialogController::SignalsConnectToSlots() noexcept {
  connect(ui_.state_, &QComboBox::currentIndexChanged,
          [this](const int index) { emit StateChanged(index); });

  connect(ui_.buttonBox, &QDialogButtonBox::accepted, [this]() {
    emit NewEntry(ui_.county_subdivision_->currentIndex(),
                  ui_.state_->currentIndex(), ui_.county_->currentIndex());
  });
}

void LocationCodeEditorDialogController::CountySubdivisionAdd(
    const QString &str) noexcept {
  ui_.county_subdivision_->addItem(str);
}

void LocationCodeEditorDialogController::StateAdd(const QString &str) noexcept {
  ui_.state_->addItem(str);
}

void LocationCodeEditorDialogController::CountyAdd(
    const QString &str) noexcept {
  ui_.county_->addItem(str);
}

void LocationCodeEditorDialogController::CountyListClear() noexcept {
  ui_.county_->clear();
}
