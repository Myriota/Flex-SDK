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
// The supported Analog Sensor Output Types are: 4-20mA and 0-10V.
// This example will supply power to an Analog sensor,
// read the current (in uA ) OR voltage (in mV) level at the
// EXT_ANALOG_IN pin and print the value on the debug console.
//! [CODE]

#include <stdio.h>
#include "flex.h"

#define APPLICATION_NAME "Analog Example"

// Note: Set this to 0 if the Analog Sensor Output Type is 0-10V, and
// set this to 1 if the Analog Sensor Output Type is 4-20mA.
#define MEASURE_CURRENT 1

#if MEASURE_CURRENT
#define ANALOG_IN_MODE FLEX_ANALOG_IN_CURRENT
#define APPLICATION_MODE "current"
#else
#define ANALOG_IN_MODE FLEX_ANALOG_IN_VOLTAGE
#define APPLICATION_MODE "voltage"
#endif

// The number of sensor readings a day.
// Modify this as required by the application.
#define SENSOR_READINGS_PER_DAY 4

// Modify this according to the power requirements of the Analog sensor.
// The FlexSense board supports an output voltage given by the enum FLEX_PowerOut.
#define ANALOG_SENSOR_POWER_IN FLEX_POWER_OUT_24V

// Modify this delay to save power based on the sensor stabilisation time
#define DELAY_SENSOR_STABILISE_MS 1500

static uint32_t MeasureAnalogInput(void) {
  uint32_t SensorReading = UINT32_MAX;

  if (FLEX_PowerOutInit(ANALOG_SENSOR_POWER_IN) != 0) {
    printf("Failed to Init Power Out.\r\n");
    goto fail_0;
  }

  if (FLEX_AnalogInputInit(ANALOG_IN_MODE) != 0) {
    printf("Failed to Init Analog Input.\r\n");
    goto fail_1;
  }

  FLEX_DelayMs(DELAY_SENSOR_STABILISE_MS);

#if MEASURE_CURRENT
  if (FLEX_AnalogInputReadCurrent(&SensorReading) != 0) {
    printf("Failed to Read Current.\r\n");
  }
#else
  if (FLEX_AnalogInputReadVoltage(&SensorReading) != 0) {
    printf("Failed to Read Voltage.\r\n");
  }
#endif

  FLEX_AnalogInputDeinit();
fail_1:
  FLEX_PowerOutDeinit();
fail_0:
  return SensorReading;
}

static time_t PrintSensorReading(void) {
  uint32_t analog_sensor_reading = MeasureAnalogInput();

  if (analog_sensor_reading != UINT32_MAX) {
#if MEASURE_CURRENT
    printf("Current = %luuA.\r\n", analog_sensor_reading);
#else
    printf("Voltage = %lumV.\r\n", analog_sensor_reading);
#endif
  }

  return (FLEX_TimeGet() + 24 * 3600 / SENSOR_READINGS_PER_DAY);
}

void FLEX_AppInit() {
  printf("%s (%s mode).\r\n", APPLICATION_NAME, APPLICATION_MODE);
  FLEX_JobSchedule(PrintSensorReading, FLEX_ASAP());
}

//! [CODE]
