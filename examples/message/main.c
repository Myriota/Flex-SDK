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
// This example demonstrates how to schedule messages for satellite transmission.
// "SendMessage" job will schedule "MESSAGES_PER_DAY" messages per day.
// The "FLEX_MessageSchedule" function pushes each message to the queue.
// The message will be transmitted when a satellite is overhead.
// The satellite passes can be checked from Access Times in Device Manager.
// The return value of "FLEX_MessageSchedule" indicates the load of the queue.
// A return value greater than one indicates that the queue is overloaded and
// that messages may begin to be dropped.
//! [CODE]

#include <stdio.h>
#include "flex.h"

#define APPLICATION_NAME "Schedule Message Example"

// Note: Modify this according to the application requirements.
#define MESSAGES_PER_DAY 4

typedef struct {
  uint16_t sequence_number;
  uint32_t time;
  // ADD YOUR PARAMETERS HERE (MAX 20 BYTES)
} __attribute__((packed)) message;

_Static_assert(sizeof(message) <= FLEX_MAX_MESSAGE_SIZE, "can't exceed the max message size");

static time_t SendMessage(void) {
  static uint16_t sequence_number = 0;
  message message;

  message.sequence_number = sequence_number++;
  message.time = FLEX_TimeGet();
  // POPULATE YOUR DATA HERE

  // Schedule messages for satellite transmission
  FLEX_MessageSchedule((void *)&message, sizeof(message));
  printf("Scheduled message: %u %lu\n", message.sequence_number, message.time);

  return (FLEX_TimeGet() + 24 * 3600 / MESSAGES_PER_DAY);
}

void FLEX_AppInit() {
  printf("%s\n", APPLICATION_NAME);
  FLEX_JobSchedule(SendMessage, FLEX_ASAP());
}

//! [CODE]
