// Copyright (c) 2024, Myriota Pty Ltd, All Rights Reserved
// SPDX-License-Identifier: BSD-3-Clause-Attribution
//
// This file is licensed under the BSD with attribution  (the "License"); you
// may not use these files except in compliance with the License.
//
// You may obtain a copy of the License here:
// LICENSE-BSD-3-Clause-Attribution.txt and at
// https://spdx.org/licenses/BSD-3-Clause-Attribution.html
//
// See the License for the specific language governing permissions and
// limitations under the License.

// An example running on Myriota's "FlexSense" board.
// This example, at fixed intervals, will attempt to acquire a GNSS fix to
// retrieve the current location and time, and print it on the debug console.
//! [CODE]

#include <stdio.h>
#include "flex.h"

#define APPLICATION_NAME "GNSS Example"

// GNSS FIX Interval
#define GNSS_FIX_INTERVAL_HOURS 6

static time_t LocationAndTime(void) {
  time_t time;
  int32_t lat, lon;

  if (FLEX_GNSSFix(&lat, &lon, &time) < 0) {
    printf("Failed to get a valid GNSS sync!\n");

  } else {
    printf("Lat: %f, Lon: %f, Time: %u.\n", lat * 1e-7, lon * 1e-7, (unsigned int)time);
  }

  return FLEX_HoursFromNow(GNSS_FIX_INTERVAL_HOURS);
}

void FLEX_AppInit() {
  printf("%s\n", APPLICATION_NAME);
  FLEX_JobSchedule(LocationAndTime, FLEX_ASAP());
}

//! [CODE]
