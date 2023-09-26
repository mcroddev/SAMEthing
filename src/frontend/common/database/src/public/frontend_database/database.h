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

#ifndef SAMETHING_FRONTEND_COMMON_DATABASE_H
#define SAMETHING_FRONTEND_COMMON_DATABASE_H

#pragma once

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

#include <stdbool.h>
#include <stddef.h>

/// The maximum number of originator codes which can be stored.
#define SAMETHING_DB_ORG_CODE_NUM_MAX (4)

/// The maximum length of the actual originator code.
#define SAMETHING_DB_ORG_CODE_LEN_MAX (3)

/// The maximum length of the originator code's description.
#define SAMETHING_DB_ORG_CODE_DESC_LEN_MAX (128)

/// The maximum number of event codes which can be stored.
#define SAMETHING_DB_EVENT_CODE_NUM_MAX (60)

/// The maximum length of the actual event code.
#define SAMETHING_DB_EVENT_CODE_LEN_MAX (3)

/// The maximum length of the event code's description.
#define SAMETHING_DB_EVENT_CODE_DESC_LEN_MAX (128)

/// The maximum number of county subdivisions which can be stored.
#define SAMETHING_DB_COUNTY_SUBDIVISION_NUM_MAX (10)

/// The maximum length of the actual county subdivision.
#define SAMETHING_DB_COUNTY_SUBDIVISION_LEN_MAX (1)

/// The maximum length of the county subdivision's description.
#define SAMETHING_DB_COUNTY_SUBDIVISION_DESC_LEN_MAX (128)

/// The maximum number of states which can be stored.
#define SAMETHING_DB_STATE_NUM_MAX (50)

/// The maximum length of the actual state code.
#define SAMETHING_DB_STATE_CODE_LEN_MAX (2)

/// The maximum length of the state's name.
#define SAMETHING_DB_STATE_NAME_LEN_MAX (20)

/// The maximum length of the actual county code.
#define SAMETHING_DB_COUNTY_CODE_LEN_MAX (3)

/// The maximum length of the county.
#define SAMETHING_DB_COUNTY_NAME_LEN_MAX (20)

/// The maximum number of counties supported per state.
#define SAMETHING_DB_COUNTY_NUM_MAX (20)

struct samething_db {
  struct {
    struct {
      struct {
        char name[SAMETHING_DB_COUNTY_NAME_LEN_MAX];
        char code[SAMETHING_DB_COUNTY_CODE_LEN_MAX];
      } counties[SAMETHING_DB_COUNTY_NUM_MAX];

      char name[SAMETHING_DB_STATE_NAME_LEN_MAX];
      size_t num_counties;
      char code[SAMETHING_DB_STATE_CODE_LEN_MAX];
    } states[SAMETHING_DB_STATE_NUM_MAX];

    size_t num_states;
  } state_county_map;

  struct {
    struct {
      char desc[SAMETHING_DB_EVENT_CODE_DESC_LEN_MAX];
      char code[SAMETHING_DB_EVENT_CODE_LEN_MAX];
    } entries[SAMETHING_DB_EVENT_CODE_NUM_MAX];

    size_t num_entries;
  } event_code;

  struct {
    struct {
      char desc[SAMETHING_DB_COUNTY_SUBDIVISION_DESC_LEN_MAX];
      char code[SAMETHING_DB_COUNTY_SUBDIVISION_LEN_MAX];
    } entries[SAMETHING_DB_COUNTY_SUBDIVISION_NUM_MAX];

    size_t num_entries;
  } county_subdivisions;

  struct {
    struct {
      char code[SAMETHING_DB_ORG_CODE_LEN_MAX];
      char desc[SAMETHING_DB_ORG_CODE_DESC_LEN_MAX];
    } entries[SAMETHING_DB_ORG_CODE_NUM_MAX];

    size_t num_entries;
  } org_code;
};

typedef char *(*samething_db_line_read_cb)(char *str, int num, void *stream);

#ifdef SAMETHING_TESTING
/// Checks to see if a string ends with the specified string.
///
/// @param str The string to inspect.
/// @param suffix The string to check.
/// @returns true if the str ends with suffix, or false otherwise.
bool samething_db_str_ends_with(const char *const str,
                                const char *const suffix);

void samething_db_org_code_add(struct samething_db *const db,
                               const char *const code, const char *const desc);

void samething_db_county_subdivision_add(struct samething_db *const db,
                                         const char *const code,
                                         const char *const desc);

void samething_db_event_code_add(struct samething_db *const db,
                                 const char *const code,
                                 const char *const desc);

void samething_db_state_code_add(struct samething_db *const db,
                                 const char *const code,
                                 const char *const name);

void samething_db_state_county_add(struct samething_db *const db,
                                   const size_t state, const char *const code,
                                   const char *const name);

int samething_db_ini_parse_event(void *user, const char *section,
                                 const char *name, const char *value);

#endif  // SAMETHING_TESTING

bool samething_db_read(struct samething_db *const db,
                       samething_db_line_read_cb read_cb, void *stream);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SAMETHING_FRONTEND_COMMON_DATABASE_H
