# SPDX-License-Identifier: MIT
#
# Copyright 2023 Michael Rodriguez
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the “Software”),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
# DEALINGS IN THE SOFTWARE.

find_package(Qt6 REQUIRED COMPONENTS Widgets)
qt_standard_project_setup()

set(CMAKE_AUTOUIC_SEARCH_PATHS views)

set(SRCS main.cpp app.cpp)
set(HDRS app.h)

set(VIEWS_FILES views/main_window.ui)

set(CONTROLLERS_SRCS controllers/main_window.cpp)
set(CONTROLLERS_HDRS controllers/main_window.h)

qt_add_executable(samething_qt ${SRCS}
                               ${HDRS}
                               ${CONTROLLERS_SRCS}
                               ${CONTROLLERS_HDRS}
                               ${VIEWS_FILES})

target_link_libraries(samething_qt PRIVATE Qt6::Widgets
                                           frontendaudio
                                           core
                                           samething-build-settings-cpp)

set_target_properties(samething_qt PROPERTIES
                      WIN32_EXECUTABLE ON
                      MACOSX_BUNDLE ON)
