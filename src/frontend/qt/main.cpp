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

#include <QApplication>

#include "app.h"

void *samething_dbg_userdata_ = nullptr;

extern "C" void samething_dbg_assert_failed(const char *const expr,
                                            const char *const file_name,
                                            const int line_no, void *userdata) {
  auto *app = static_cast<SAMEthingApp *>(userdata);
  app->DebugAssertionEncountered(expr, file_name, line_no);
}

/// Program entry point.
///
/// @param argc The number of arguments passed to the program from the
/// environment in which the program is run.
/// @param argv The arguments passed to the program from the environment in
/// which the program is run.
/// @returns See documentation for QApplication::exec().
auto main(int argc, char *argv[]) -> int {
  const QApplication qt_instance(argc, argv);

  QApplication::setApplicationName("samething");
  QApplication::setApplicationVersion("1.0.0");

  QApplication::setOrganizationName("mcroddev");
  QApplication::setOrganizationDomain("https://mcrod.dev");

  SAMEthingApp samething_app;
  samething_dbg_userdata_ = &samething_app;
  samething_app.Initialize();

  return QApplication::exec();
}
