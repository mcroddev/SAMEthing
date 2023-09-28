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

#include "samething/frontend/database.h"

#include <string.h>

#include "ini.h"
#include "samething/debug.h"

SAMETHING_STATIC bool samething_db_str_ends_with(const char *const str,
                                                 const char *const suffix) {
  const size_t len_str = strlen(str);
  const size_t len_suffix = strlen(suffix);

  return memcmp(&str[(len_str - len_suffix)], suffix, len_suffix) == 0;
}

SAMETHING_STATIC void samething_db_org_code_add(struct samething_db *const db,
                                                const char *const code,
                                                const char *const desc) {
  strncpy(db->org_code.entries[db->org_code.num_entries].code, code,
          SAMETHING_DB_ORG_CODE_LEN_MAX);
  strncpy(db->org_code.entries[db->org_code.num_entries].desc, desc,
          SAMETHING_DB_ORG_CODE_DESC_LEN_MAX);
  db->org_code.num_entries++;
}

SAMETHING_STATIC void samething_db_county_subdivision_add(
    struct samething_db *const db, const char *const code,
    const char *const desc) {
  strncpy(
      db->county_subdivisions.entries[db->county_subdivisions.num_entries].code,
      code, SAMETHING_DB_COUNTY_SUBDIVISION_LEN_MAX);
  strncpy(
      db->county_subdivisions.entries[db->county_subdivisions.num_entries].desc,
      desc, SAMETHING_DB_COUNTY_SUBDIVISION_DESC_LEN_MAX);
  db->county_subdivisions.num_entries++;
}

SAMETHING_STATIC void samething_db_event_code_add(struct samething_db *const db,
                                                  const char *const code,
                                                  const char *const desc) {
  strncpy(db->event_code.entries[db->event_code.num_entries].code, code,
          SAMETHING_DB_EVENT_CODE_LEN_MAX);
  strncpy(db->event_code.entries[db->event_code.num_entries].desc, desc,
          SAMETHING_DB_EVENT_CODE_DESC_LEN_MAX);
  db->event_code.num_entries++;
}

SAMETHING_STATIC void samething_db_state_code_add(struct samething_db *const db,
                                                  const char *const code,
                                                  const char *const name) {
  strncpy(db->state_county_map.states[db->state_county_map.num_states].code,
          code, SAMETHING_DB_STATE_CODE_LEN_MAX);
  strncpy(db->state_county_map.states[db->state_county_map.num_states].name,
          name, SAMETHING_DB_STATE_NAME_LEN_MAX);
  db->state_county_map.num_states++;
}

SAMETHING_STATIC void samething_db_state_county_add(
    struct samething_db *const db, const size_t state, const char *const code,
    const char *const name) {
  strncpy(db->state_county_map.states[state]
              .counties[db->state_county_map.states[state].num_counties]
              .code,
          code, SAMETHING_DB_COUNTY_CODE_LEN_MAX);
  strncpy(db->state_county_map.states[state]
              .counties[db->state_county_map.states[state].num_counties]
              .name,
          name, SAMETHING_DB_COUNTY_NAME_LEN_MAX);
  db->state_county_map.states[state].num_counties++;
}

SAMETHING_STATIC int samething_db_ini_parse_event(void *user,
                                                  const char *section,
                                                  const char *name,
                                                  const char *value) {
  struct samething_db *const db = (struct samething_db *const)user;

  if (strcmp(section, "originator_codes") == 0) {
    samething_db_org_code_add(db, name, value);
  } else if (strcmp(section, "event_codes") == 0) {
    samething_db_event_code_add(db, name, value);
  } else if (strcmp(section, "county_subdivisions") == 0) {
    samething_db_county_subdivision_add(db, name, value);
  } else if (strcmp(section, "state_codes") == 0) {
    samething_db_state_code_add(db, name, value);
  } else if (samething_db_str_ends_with(section, "_county_codes")) {
    char state_code[SAMETHING_DB_STATE_CODE_LEN_MAX];
    memcpy(state_code, section, SAMETHING_DB_STATE_CODE_LEN_MAX);

    for (size_t state = 0; state < db->state_county_map.num_states; ++state) {
      if (memcmp(state_code, db->state_county_map.states[state].code,
                 SAMETHING_DB_STATE_CODE_LEN_MAX) == 0) {
        samething_db_state_county_add(db, state, name, value);
      }
    }
  } else {
    return 0;
  }
  return 1;
}

bool samething_db_read(struct samething_db *const db,
                       samething_db_line_read_cb read_cb, void *stream) {
  return ini_parse_stream((ini_reader)read_cb, stream,
                          &samething_db_ini_parse_event, db) == 0;
}
