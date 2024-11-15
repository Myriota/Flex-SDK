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

// An example using the Myriota Modbus library with Myriota's "FlexSense"
// board. This example demonstrates how to read the temperature and humidity
// from the "DFRobot SEN0438" sensor, via the FlexSenses Myriota Modbus
// library.
//! [CODE]

#include <stdio.h>
#include <string.h>

#include "flex.h"
#include "myriota/modbus.h"

#define APPLICATION_NAME "DFRobot SEN0438 Modbus Driver Application"
#define MESSAGES_PER_DAY 4

typedef struct {
  uint8_t sequence_number;
  uint32_t time;
  int32_t latitude;
  int32_t longitude;
  int16_t temperature;
  int16_t humidity;
} __attribute__((packed)) Message;
_Static_assert(sizeof(Message) <= FLEX_MAX_MESSAGE_SIZE, "can't exceed the max message size");

typedef struct {
  FLEX_SerialProtocol protocol;
  uint32_t baud_rate;
  uint32_t rx_timeout_ticks;
} SerialContext;

typedef struct {
  MYRIOTA_ModbusHandle modbus_handle;
  SerialContext serial_context;
} ApplicationContext;

static ApplicationContext application_context = {0};

static int serial_init(void *const ctx) {
  SerialContext *const serial = ctx;
  FLEX_PowerOutInit(FLEX_POWER_OUT_12V);
  // Let power out stabilise
  FLEX_DelayMs(1500);
  return FLEX_SerialInit(serial->protocol, serial->baud_rate);
}

static void serial_deinit(void *const ctx) {
  (void)ctx;
  FLEX_SerialDeinit();
  FLEX_PowerOutDeinit();
}

static ssize_t serial_read(void *const ctx, uint8_t *const buffer, const size_t count) {
  SerialContext *const serial = ctx;

  uint8_t *curr = buffer;
  const uint8_t *const end = buffer + count;
  const uint32_t end_ticks = FLEX_TickGet() + serial->rx_timeout_ticks;
  while (FLEX_TickGet() <= end_ticks) {
    if (curr >= end) {
      return -1;
    }

    int num_bytes = FLEX_SerialRead(curr, 1);
    if (num_bytes < 0) {
      return -1;
    }
    if (num_bytes == 1) {
      ++curr;
    }
  }

  return curr - buffer;
}

static ssize_t serial_write(void *const ctx, const uint8_t *const buffer, const size_t count) {
  (void)ctx;
  const int result = FLEX_SerialWrite(buffer, count);
  if (result != FLEX_SUCCESS) {
    return result;
  }
  FLEX_DelayMs(1000);
  return count;
}

inline uint16_t merge_i16(const uint8_t hi, const uint8_t low) {
  return (int16_t)((uint16_t)hi << 8 | (uint16_t)low);
}

static void read_temperature_and_humidity(int16_t *const temperature, int16_t *const humidity) {
  const MYRIOTA_ModbusHandle handle = application_context.modbus_handle;

  // NOTE: Enable/disable the Modbus driver in order to conserve power.
  MYRIOTA_ModbusEnable(handle);

  uint8_t bytes[4] = {0};
  const MYRIOTA_ModbusDeviceAddress slave = 0x01;
  const MYRIOTA_ModbusDataAddress addr = 0x0000;
  MYRIOTA_ModbusReadHoldingRegisters(handle, slave, addr, 2, bytes);

  *humidity = merge_i16(bytes[0], bytes[1]);
  *temperature = merge_i16(bytes[2], bytes[3]);

  MYRIOTA_ModbusDisable(handle);
}

static time_t send_message(void) {
  static uint8_t sequence_number = 0;

  Message message = {0};
  message.sequence_number = sequence_number++;
  message.time = FLEX_TimeGet();

  int32_t latitude = 0;
  int32_t longitude = 0;
  FLEX_LastLocationAndLastFixTime(&latitude, &longitude, NULL);
  message.latitude = latitude;
  message.longitude = longitude;

  int16_t temperature = 0;
  int16_t humidity = 0;
  read_temperature_and_humidity(&temperature, &humidity);
  message.temperature = temperature;
  message.humidity = humidity;

  // Schedule messages for satellite transmission
  FLEX_MessageSchedule((const uint8_t *const)&message, sizeof(message));
  printf("Scheduled message: \n");
  printf("  sequence_number: %u\n", sequence_number - 1);
  printf("  time: %lu\n", message.time);
  printf("  latitude: %ld\n", message.latitude);
  printf("  longitude: %ld\n", message.longitude);
  printf("  temperature: %d\n", message.temperature);
  printf("  humidity: %d\n", message.humidity);

  return (FLEX_TimeGet() + 24 * 3600 / MESSAGES_PER_DAY);
}

void FLEX_AppInit() {
  printf("%s\n", APPLICATION_NAME);

  // Initialize Modbus device
  application_context.serial_context.protocol = FLEX_SERIAL_PROTOCOL_RS485;
  application_context.serial_context.baud_rate = 9600;
  application_context.serial_context.rx_timeout_ticks = 2000;
  const MYRIOTA_ModbusInitOptions options = {
    .framing_mode = MODBUS_FRAMING_MODE_RTU,
    .serial_interface =
      {
        .ctx = &application_context.serial_context,
        .init = serial_init,
        .deinit = serial_deinit,
        .read = serial_read,
        .write = serial_write,
      },
  };
  application_context.modbus_handle = MYRIOTA_ModbusInit(options);
  if (application_context.modbus_handle <= 0) {
    printf("Failed to initialized Modbus: %d\n", application_context.modbus_handle);
    while (true) {
    };
  }

  FLEX_JobSchedule(send_message, FLEX_ASAP());
}

//! [CODE]
