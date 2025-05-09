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

#include <stdio.h>
#include "flex.h"

#define APPLICATION_NAME "YOUR APPLICATION NAME"
#define MESSAGES_PER_DAY 4

typedef struct {
  uint16_t sequence_number;
  uint32_t time;
  // ADD YOUR PARAMETERS HERE
} __attribute__((packed)) message;

static time_t SendMessage(void) {
  static uint16_t sequence_number = 0;
  message message;

  message.sequence_number = sequence_number++;
  message.time = FLEX_TimeGet();

  /* POPULATE YOUR DATA HERE */

  FLEX_MessageSchedule((void *)&message, sizeof(message));
  return (FLEX_TimeGet() + 24 * 3600 / MESSAGES_PER_DAY);
}

/*
 * IMPORTANT! `FLEX_AppInit` is the entry point for your application. You
 * must provide an implementation, otherwise you will experience linking
 * errors similar to this: "undefined reference to `FLEX_AppInit'"
 */
void FLEX_AppInit() {
  printf("%s\n", APPLICATION_NAME);

  FLEX_JobSchedule(SendMessage, FLEX_ASAP());
}
