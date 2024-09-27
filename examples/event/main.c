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

// This example demonstrates the handling of an External Digital IO wakeup
// event. RunsOnExtDigitalIOWakeup job is scheduled when the
// External Digital IO is pulled Low.
//! [CODE]

#include <stdio.h>
#include "flex.h"

#define APPLICATION_NAME "External Digital I/O Wake-up Event"

// Note: Change this to set the desired External Digital IO Pin for wakeup.
const FLEX_DigitalIOPin WakeupPin = FLEX_EXT_DIGITAL_IO_1;

static void RunsOnExtDigitalIOWakeup() {
  printf("Woken up by External Digital IO @ %u\n", (unsigned int)FLEX_TimeGet());

  printf("External Digital IO %u level is %s\n", (unsigned int)WakeupPin + 1,
    ((FLEX_ExtDigitalIOGet(WakeupPin) == FLEX_EXT_DIGITAL_IO_HIGH) ? "high" : "low"));
}

void FLEX_AppInit() {
  printf("%s\n", APPLICATION_NAME);

  // Enabling the Digital IO Wakeup
  FLEX_ExtDigitalIOWakeupModify(WakeupPin, FLEX_EXT_DIGITAL_IO_WAKEUP_ENABLE);

  // Registering the Wakeup handler
  FLEX_ExtDigitalIOWakeupHandlerModify(RunsOnExtDigitalIOWakeup, FLEX_HANDLER_MODIFY_ADD);
}

//! [CODE]
