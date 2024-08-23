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
// This example demonstrates how to set/get the level of an External Digital
// I/O pin.
// First, it will set the pin HIGH and read the level of the pin. The expected
// read value is HIGH.
// Finally, it will set the pin LOW and read the level of the pin. The expected
// read value is LOW.
//! [CODE]

#include <stdio.h>
#include "flex.h"

#define APPLICATION_NAME "External Digital I/O Example"

// Note: change this to the required External Digital I/O Pin.
// Available pins are given in enum FLEX_DigitalIOPin.
#define DIGITAL_IO_PIN FLEX_EXT_DIGITAL_IO_1

const char* get_digital_io_level_str(const FLEX_DigitalIOLevel Level) {
  return (Level == FLEX_EXT_DIGITAL_IO_HIGH) ? "HIGH" : "LOW";
}

static time_t DigitalIOExample(void) {
  for (int i = 0; i < 2; i++) {
    const FLEX_DigitalIOLevel set_level_val =
      (i % 2) ? FLEX_EXT_DIGITAL_IO_LOW : FLEX_EXT_DIGITAL_IO_HIGH;

    printf("Setting: External Digital IO Pin %d to %s\n", DIGITAL_IO_PIN + 1,
      get_digital_io_level_str(set_level_val));
    FLEX_ExtDigitalIOSet(DIGITAL_IO_PIN, set_level_val);

    const FLEX_DigitalIOLevel get_level_val = FLEX_ExtDigitalIOGet(DIGITAL_IO_PIN);
    printf("Reading: External Digital IO Pin %d level is %s\n", DIGITAL_IO_PIN + 1,
      get_digital_io_level_str(get_level_val));
  }

  return FLEX_Never();
}

void FLEX_AppInit() {
  printf("%s\n", APPLICATION_NAME);
  FLEX_JobSchedule(DigitalIOExample, FLEX_ASAP());
}

//! [CODE]
