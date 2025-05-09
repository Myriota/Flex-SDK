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

// This example demonstrates RS-485/RS-232 interface communication between
// an external device and the FlexSense Device.
// This also demonstrates a basic modem application.
// Operates as follows:
// 1. The external device wakes up the module by setting the selected
//    WakeupPin Low.
// 2. Waits for the "READY\n" string from the FlexSense Device.
// 3. Sends message string terminated by "\n".
// 4. Sets the selected WakeupPin High again.
// 5. The FlexSense Device acknowledges reception with string "\nOK\n".
//! [CODE]

#include <stdio.h>
#include <string.h>
#include "flex.h"

#define READY_STRING "READY\n"
#define RECEIVE_TIMEOUT_MS 2000
#define ACK_STRING "\nOK\n"
#define BAUDRATE 115200

// Note: Change this to set the desired External Digital IO Pin for Wakeup.
const FLEX_DigitalIOPin WakeupPin = FLEX_EXT_DIGITAL_IO_1;

#define USING_RS232 0
#define USING_RS485 1
#define RX_BUFFER_MAX 20

#if SERIAL_INTERFACE == USING_RS485
#define SERIAL_PROTOCOL FLEX_SERIAL_PROTOCOL_RS485
#define APPLICATION_NAME "RS485 Example"

#elif SERIAL_INTERFACE == USING_RS232
#define SERIAL_PROTOCOL FLEX_SERIAL_PROTOCOL_RS232
#define APPLICATION_NAME "RS232 Example"

#else
#error "Must supply a valid 'SERIAL_INTERFACE' to the build!"
#endif

// Read new line terminated string from the Serial interface with timeout
// Return number of bytes read or -1 on timeout or string is too long
int ReadStringWithTimeout(uint8_t *Rx, size_t MaxLength) {
  const uint32_t start = FLEX_TickGet();
  size_t count = 0;
  while (FLEX_TickGet() - start < RECEIVE_TIMEOUT_MS) {
    uint8_t ch;
    if (FLEX_SerialRead(&ch, 1) == 1) {
      if (ch == '\n')
        return count;
      Rx[count++] = ch;
      if (count == MaxLength)
        return -1;
    }
  }
  return -1;
}

static void Comm() {
  if (FLEX_SerialInit(SERIAL_PROTOCOL, BAUDRATE) != 0) {
    printf("Failed to initialise Serial interface\n");
    return;
  }
  FLEX_SerialWrite((uint8_t *)READY_STRING, strlen(READY_STRING));

  uint8_t Rx[RX_BUFFER_MAX] = {0};
  int len = ReadStringWithTimeout(Rx, RX_BUFFER_MAX);
  if (len <= 0) {
    printf("Failed to receive message\n");
  } else {
    FLEX_SerialWrite((uint8_t *)Rx, len);
    FLEX_SerialWrite((uint8_t *)ACK_STRING, strlen(ACK_STRING));
    printf("Received message: ");
    for (int i = 0; i < len; i++)
      printf("%02x", Rx[i]);
    printf("\n");
  }

  FLEX_SerialDeinit();
}

void FLEX_AppInit() {
  printf("%s\n", APPLICATION_NAME);

  // Enabling the Digital IO Wakeup
  FLEX_ExtDigitalIOWakeupModify(WakeupPin, FLEX_EXT_DIGITAL_IO_WAKEUP_ENABLE);

  // Registering the Wakeup handler
  FLEX_ExtDigitalIOWakeupHandlerModify(Comm, FLEX_HANDLER_MODIFY_ADD);
}

//! [CODE]
