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

#include "valid_time_period_spinbox.h"

ValidTimePeriodWidget::ValidTimePeriodWidget(QWidget* parent) noexcept
    : QTimeEdit(parent) {
  setMinimumTime(QTime(0, 15, 0));
}

void ValidTimePeriodWidget::stepBy(int steps) {
  QTime current_time = time();

  int segments = 0;

  if (current_time.hour() >= 1) {
    segments = 30;
  } else {
    segments = 15;
  }

  if (currentSection() == QTimeEdit::MinuteSection) {
    if (steps >= 0) {
      current_time = current_time.addSecs(60 * segments);
    } else {
      current_time = current_time.addSecs(60 * -segments);
    }
    setTime(current_time);
  } else if (currentSection() == QTimeEdit::HourSection) {
    if (steps >= 0) {
      current_time = current_time.addSecs(3600);

      if (current_time.hour() == 1) {
        current_time = current_time.addSecs(60 * -current_time.minute());
      }
    } else {
      current_time = current_time.addSecs(-3600);

      if (current_time.hour() == 0) {
        // Will automatically clip back to the minimum time.
        current_time = current_time.addSecs(60 * -current_time.minute());
      }
    }
    setTime(current_time);
  }
}
