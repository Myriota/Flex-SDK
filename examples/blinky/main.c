// Copyright (c) 2016-2024, Myriota Pty Ltd, All Rights Reserved
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
// This example will blink the Green LED "NO_OF_FLASHES" times,
// every "BLINKY_INTERVAL_MIN" minutes.
//! [CODE]

#include <stdio.h>
#include "flex.h"

#define APPLICATION_NAME "Blinky"

// Blinky Interval
#define BLINKY_INTERVAL_MIN 1

// Number of LED Flashes
#define NO_OF_FLASHES 10

// Green LED ON time
#define LED_ON_TIME_SEC 1

// Green LED OFF time
#define LED_OFF_TIME_SEC 1

static time_t Blinky(void) {
  for (uint8_t i = 0; i < NO_OF_FLASHES; i++) {
    FLEX_LEDGreenStateSet(FLEX_LED_ON);
    printf("Green LED On\n");
    FLEX_Sleep(LED_ON_TIME_SEC);

    FLEX_LEDGreenStateSet(FLEX_LED_OFF);
    printf("Green LED Off\n");
    FLEX_Sleep(LED_OFF_TIME_SEC);
  }

  return FLEX_MinutesFromNow(BLINKY_INTERVAL_MIN);
}

void FLEX_AppInit() {
  printf("%s\n", APPLICATION_NAME);
  FLEX_JobSchedule(Blinky, FLEX_ASAP());
}

//! [CODE]
