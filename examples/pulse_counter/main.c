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

// An example running on Myriota's "FlexSense" board
// This example demonstrates how to configure and use the Pulse Counter APIs.
//! [CODE]

#include <inttypes.h>
#include <stdio.h>
#include "flex.h"

#define APPLICATION_NAME "Pulse Counter Example"

#define PULSE_WAKEUP_COUNT 6

static void RunsOnPulseCounterEvent() {
  printf("Woken up by Pulse Counter @ %u\n", (unsigned int)FLEX_TimeGet());

  printf("Current pulse counter value: %" PRIu32 "\n", (uint32_t)FLEX_PulseCounterGet());
}

void FLEX_AppInit() {
  printf("%s\n", APPLICATION_NAME);

  // Initialise to generate event every N pulses
  if (FLEX_PulseCounterInit(PULSE_WAKEUP_COUNT, FLEX_PCNT_DEFAULT_OPTIONS)) {
    printf("Failed to initialise pulse counter\n");
  }

  // Enable pulse counter event
  FLEX_PulseCounterHandlerModify(RunsOnPulseCounterEvent, FLEX_HANDLER_MODIFY_ADD);
}

//! [CODE]
