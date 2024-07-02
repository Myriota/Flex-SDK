#include <stdio.h>
#include "flex.h"

#define APPLICATION_NAME "YOUR APPLICATION NAME"
#define MESSAGES_PER_DAY 4

typedef struct {
  uint16_t sequence_number;
  uint32_t time;
  /* ADD YOUR PARAMETERS HERE (MAX 20 BYTES) */
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
