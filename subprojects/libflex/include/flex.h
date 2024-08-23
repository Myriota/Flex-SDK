/// \file flex.h FlexSense Board specific definition
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

#ifndef FLEX_H
#define FLEX_H

#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#include "flex_errors.h"

/// @defgroup Version_Info Version Information
/// Flex Library versioning information.
///@{

/// Get the Flex Library Version in "Major.Minor.Patch" format.
/// \return the Flex Library Version.
const char *FLEX_VersionString(void);
/// Get the Flex Library Major Version information.
/// \return the Flex Library Version Major value.
uint16_t FLEX_VersionMajor(void);
/// Get the Flex Library Minor Version information.
/// \return the Flex Library Version Minor value.
uint16_t FLEX_VersionMinor(void);
/// Get the Flex Library Patch Version information.
/// \return the Flex Library Version Patch value.
uint16_t FLEX_VersionPatch(void);

///@}

/// @defgroup Analog_Input Analog Input
/// Configure and control the Analog Input interface on the FlexSense board.
///@{

/// Analog Input Modes.
typedef enum {
  FLEX_ANALOG_IN_VOLTAGE,  ///< use when measuring Voltage at the Analog Input
  FLEX_ANALOG_IN_CURRENT   ///< use when measuring Current at the Analog Input
} FLEX_AnalogInputMode;

/// Initialise the Analog Input in Voltage or Current mode.
/// \param[in] InputMode the operation mode selected from \p FLEX_AnalogInputMode.
/// \return FLEX_SUCCESS (0) if succeeded and < 0 if failed.
/// \retval -FLEX_ERROR_IO_EXPANDER: failed to initialise i/o expander device.
/// \retval -FLEX_ERROR_POWER_OUT: failed to enable power to analog input interface.
int FLEX_AnalogInputInit(const FLEX_AnalogInputMode InputMode);
/// De-initialise the Analog Input interface.
/// \return FLEX_SUCCESS (0) if succeeded and < 0 if failed.
/// \retval -FLEX_ERROR_IO_EXPANDER: failed to de-initialise i/o expander device.
/// \retval -FLEX_ERROR_POWER_OUT: failed to disable power to analog input interface.
int FLEX_AnalogInputDeinit(void);
/// Get the Current reading in micro-amps from the Analog Input interface.
/// The Analog Input interface needs to be initialised in Current mode
/// before performing a Current reading.
/// \param[out] pMicroAmps the Current reading in uA.
/// \return FLEX_SUCCESS (0) if succeeded and < 0 if failed.
/// \retval -FLEX_ERROR_EINVAL: pMicroAmps is a NULL parameter
/// \retval -FLEX_ERROR_EOPNOTSUPP: Analog input set to incorrect state (i.e trying
///         to read current when init to voltage mode)
/// \retval -FLEX_ERROR_READ_FAIL: Error reading from ADC device
int FLEX_AnalogInputReadCurrent(uint32_t *const pMicroAmps);
/// Get the Voltage reading in milli-volts from the Analog Input interface.
/// The Analog Input interface needs to be initialised in Voltage mode
/// before performing a Voltage reading.
/// \param[out] pMilliVolts the Voltage reading in mV.
/// \return FLEX_SUCCESS (0) if succeeded and < 0 if failed.
/// \retval -FLEX_ERROR_EINVAL: pMilliVolts is a NULL parameter
/// \retval -FLEX_ERROR_EOPNOTSUPP: Analog input set to incorrect state (i.e trying
///         to read voltage when init to current mode)
/// \retval -FLEX_ERROR_READ_FAIL: Error reading from ADC device
int FLEX_AnalogInputReadVoltage(uint32_t *const pMilliVolts);

///@}

/// @defgroup Power_Out Power Out Control
/// Configure and control the external output voltage on the FlexSense board.
///@{

/// Boost PSU Output Voltage Options.
typedef enum {
  FLEX_POWER_OUT_24V,  ///< set output voltage to 24V
  FLEX_POWER_OUT_12V,  ///< set output voltage to 12V
  FLEX_POWER_OUT_5V    ///< set output voltage to 5V
} FLEX_PowerOut;

/// Enable and sets the Output Voltage of the Boost PSU.
/// \param[in] Voltage the required output voltage selected from \p FLEX_PowerOut.
/// \return FLEX_SUCCESS (0) if succeeded and < 0 if failed.
/// \retval -FLEX_ERROR_EALREADY: already initialised
/// \retval -FLEX_ERROR_IO_EXPANDER: failed to initialise or configure expander device
/// \retval -FLEX_ERROR_EOPNOTSUPP: tried to set invalid power out value
/// \retval -FLEX_ERROR_PWM: failed to setup or configure pwm device
int FLEX_PowerOutInit(const FLEX_PowerOut Voltage);
/// Disable the Boost PSU.
/// \return FLEX_SUCCESS (0) if succeeded and < 0 if failed.
/// \retval -FLEX_ERROR_IO_EXPANDER: failed to configure expander device
int FLEX_PowerOutDeinit(void);

///@}

/// @defgroup LED_Control LED Control
/// Control the Green LED on the FlexSense board.
///@{

/// LED States.
typedef enum {
  FLEX_LED_ON,   ///< turn LED On
  FLEX_LED_OFF,  ///< turn LED Off
} FLEX_LEDState;

/// Change the state of the Green LED.
/// \param[in] LEDState the required LED state selected from \p FLEX_LEDState.
/// \return FLEX_SUCCESS (0) if succeeded and < 0 if failed.
/// \retval -FLEX_ERROR_IO_EXPANDER: failed to initialise or configure expander device
int FLEX_LEDGreenStateSet(const FLEX_LEDState LEDState);

///@}

/// @defgroup Ext_Digital_IO External Digital I/O
/// Configure and control the External Digital I/O interface on the FlexSense board.
///@{

/// Available Digital I/O Pins.
typedef enum {
  FLEX_EXT_DIGITAL_IO_1,  ///< Digital I/O Pin 1
  FLEX_EXT_DIGITAL_IO_2   ///< Digital I/O Pin 2
} FLEX_DigitalIOPin;

/// Digital I/O level Options.
typedef enum {
  FLEX_EXT_DIGITAL_IO_LOW = 0,  ///< Digital I/O level Low
  FLEX_EXT_DIGITAL_IO_HIGH      ///< Digital I/O level High
} FLEX_DigitalIOLevel;

/// Set the level of an external Digital I/O pin High or Low.
/// \param[in] PinNum the external Digital I/O pin number.
/// \param[in] Level the required Digital I/O level for the selected pin.
/// \return FLEX_SUCCESS (0) if succeeded and < 0 if failed.
/// \retval -FLEX_ERROR_IO_EXPANDER: failed to initialise or configure expander device
/// \retval -FLEX_ERROR_EINVAL: Invalid level parameter
/// \retval -FLEX_ERROR_I2C: Error with i2c device when setting IO
int FLEX_ExtDigitalIOSet(const FLEX_DigitalIOPin PinNum, const FLEX_DigitalIOLevel Level);
/// Get the level of an External Digital I/O pin.
/// \param[in] PinNum the external Digital I/O pin number.
/// \return FLEX_DigitalIOLevel if succeeded and < 0 if failed.
/// \retval -FLEX_ERROR_IO_EXPANDER: failed to initialise or configure expander device
/// \retval -FLEX_ERROR_GPIO: Error with GPIO device when setting IO
int FLEX_ExtDigitalIOGet(const FLEX_DigitalIOPin PinNum);

/// External Digital I/O Wakeup Modify Actions
typedef enum {
  FLEX_EXT_DIGITAL_IO_WAKEUP_ENABLE,   ///< action to enable wakeup for an External Digital I/O pin
  FLEX_EXT_DIGITAL_IO_WAKEUP_DISABLE,  ///< action to disable wakeup for an External Digital I/O pin
} FLEX_ExtDigitalIOWakeupModifyAction;

/// Enable/Disable an External Digital IO with wakeup capability for next wakeup.
/// This wakeups on a falling edge of the External Digital IO.
/// \param[in] PinNum the external Digital I/O pin number.
/// \param[in] Action Enable/Disable wakeup for the selected Digital I/O pin.
/// \return FLEX_SUCCESS (0) if succeeded and < 0 if failed.
/// \retval -FLEX_ERROR_GPIO: Error with GPIO device when setting IO
int FLEX_ExtDigitalIOWakeupModify(const FLEX_DigitalIOPin PinNum,
  const FLEX_ExtDigitalIOWakeupModifyAction Action);

/// Wakeup Handler Modify Actions
typedef enum {
  FLEX_DIGITAL_IO_WAKEUP_HANDLER_MODIFY_ADD,     ///< action to add an External Digital I/O wakeup
                                                 ///< handler
  FLEX_DIGITAL_IO_WAKEUP_HANDLER_MODIFY_REMOVE,  ///< action to remove an External Digital I/O
                                                 ///< wakeup handler
} FLEX_ExtDigitalIOWakeupHandlerModifyAction;

/// Wakeup Handler Function Pointer Declaration.
typedef void (*FLEX_IOWakeupHandler)(void);

/// Add or remove an external Digital I/O wakeup handler that will be called on a falling edge on
/// the external Digital I/O pin.
/// \param[in] Handler function pointer of the external Digital I/O wakeup handler.
/// \param[in] Action Add/Remove the input IO wakeup handler.
/// \return FLEX_SUCCESS (0) if succeeded and < 0 if failed.
/// \retval -FLEX_ERROR_EALREADY: handler already exists, remove first
/// \retval -FLEX_ERROR_EINVAL: attempt to remove non-existant handler
int FLEX_ExtDigitalIOWakeupHandlerModify(const FLEX_IOWakeupHandler Handler,
  const FLEX_ExtDigitalIOWakeupHandlerModifyAction Action);

///@}

/// @defgroup Ext_I2C External I2C
/// Interact with external i2c peripheral devices on the FlexSense board.
///@{

/// Write to a an i2c device at a given address.
/// \param[in] Address the peripheral device address.
/// \param[in] TxData pointer to the TX buffer containing registers address and command.
/// \param[in] TxLength length of data to be sent.
/// \return FLEX_SUCCESS (0) if succeeded and < 0 if failed.
/// \retval -FLEX_ERROR_EINVAL: invalid i2c address (address in use by internal device)
/// \retval -FLEX_ERROR_IO_EXPANDER: failed to initialise or configure expander device
/// \retval -FLEX_ERROR_POWER_OUT: failed to power on external i2c bus
/// \retval -FLEX_ERROR_I2C: error initialising i2c bus
int FLEX_ExtI2CWrite(int Address, const uint8_t *const TxData, uint16_t TxLength);

/// Write to a an i2c device at a given address and then read the response.
/// \param[in] Address the peripheral device address.
/// \param[in] TxData pointer to TX buffer containing registers address and command.
/// \param[in] TxLength length of data to be sent.
/// \param[out] RxData pointer to the RX buffer.
/// \param[in] RxLength length of data to be received.
/// \return FLEX_SUCCESS (0) if succeeded and < 0 if failed.
/// \retval -FLEX_ERROR_EINVAL: invalid i2c address (address in use by internal device)
/// \retval -FLEX_ERROR_IO_EXPANDER: failed to initialise or configure expander device
/// \retval -FLEX_ERROR_POWER_OUT: failed to power on external i2c bus
/// \retval -FLEX_ERROR_I2C: error initialising i2c bus
int FLEX_ExtI2CRead(int Address, const uint8_t *const TxData, uint16_t TxLength,
  uint8_t *const RxData, uint16_t RxLength);

///@}

/// @defgroup Serial Serial (RS485/RS232)
/// Configure and control the serial interface on the FlexSense board.
///@{

/// Serial Interface - Protocol Options
typedef enum {
  FLEX_SERIAL_PROTOCOL_RS485,  ///< use RS-485 as the Serial Protocol
  FLEX_SERIAL_PROTOCOL_RS232,  ///< use RS-232 as the Serial Protocol
} FLEX_SerialProtocol;

/// Initialise the selected serial interface (RS-485 or RS-232).
/// Note that both RS-485 and RS-232 cannot be initialised at the same time.
/// \param[in] Protocol required protocol for serial communication.
/// \param[in] BaudRate required baudrate.
/// \return FLEX_SUCCESS (0) if succeeded and < 0 if failed.
/// \retval -FLEX_ERROR_EALREADY: device already initialised
/// \retval -FLEX_ERROR_IO_EXPANDER: failed to initialise or configure expander device
/// \retval -FLEX_ERROR_SERIAL: failed to initialise serial device
/// \retval -FLEX_ERROR_POWER_OUT: failed to enable power to serial interface
int FLEX_SerialInit(FLEX_SerialProtocol Protocol, uint32_t BaudRate);
/// Write to the serial interface synchronously.
/// \param[in] Tx pointer to the transmit buffer to be sent.
/// \param[in] Length length of data to be sent.
/// \return FLEX_SUCCESS (0) if succeeded and < 0 if failed.
/// \retval -FLEX_ERROR_NOT_INIT: device not initialised
int FLEX_SerialWrite(const uint8_t *Tx, size_t Length);
/// Read from input buffer of the serial interface. The buffer size is 50 bytes.
/// \param[out] Rx pointer to the receive buffer.
/// \param[in] Length length of the receive buffer.
/// \return number of bytes read back or < 0 if read failed.
/// \retval -FLEX_ERROR_NOT_INIT: device not initialised
int FLEX_SerialRead(uint8_t *Rx, size_t Length);
/// Deinitialise the serial interface.
/// \return FLEX_SUCCESS (0) if succeeded and < 0 if failed.
/// \retval -FLEX_ERROR_POWER_OUT: failed to disable power to serial interface
int FLEX_SerialDeinit(void);

///@}

/// @defgroup Pulse_Counter Pulse Counter
/// Configure and control the Pulse Counter hardware on the FlexSense board.
///@{

/// Pulse Counter Options, bit-wise, can be ORed
typedef enum {
  FLEX_PCNT_DEFAULT_OPTIONS = 0,    ///< default option
  FLEX_PCNT_EDGE_FALLING = 1 << 0,  ///< count on falling edge, default rising edge
  FLEX_PCNT_DEBOUNCE_DISABLE =
    1 << 1,                   ///< disable hardware debouncing, default enabled for about 160us
  FLEX_PCNT_PULL_UP = 1 << 2  ///< enable pull-up, default pull-down
} FLEX_PulseCounterOption;

/// Initialise the pulse counter and configure the event generation logic.
/// Event is generated when pulse count hits multiple of Limit. Limit can be set
/// to 0 to 256, or multiple of 256. Set Limit to 0 to disable event generation.
/// Options are used to configure the pulse counter. Set option to 0 to
/// count on rising edge, and enable debouncing.
/// \param[in] Limit maximum value to count to before overflow occurs and reset counter to 0.
/// \param[in] Options configuration options selected from \p FLEX_PulseCounterOption.
/// \return FLEX_SUCCESS (0) if succeeded and < 0 if failed.
int FLEX_PulseCounterInit(const uint32_t Limit, const uint32_t Options);
/// Get the total count of the pulse counter.
/// \return the total count.
uint64_t FLEX_PulseCounterGet(void);
/// De-initialise the pulse counter.
void FLEX_PulseCounterDeinit(void);

/// PCNT Wakeup Handler Modify Actions
typedef enum {
  FLEX_PCNT_WAKEUP_HANDLER_MODIFY_ADD,     ///< action to add a Pulse Count wakeup handler
  FLEX_PCNT_WAKEUP_HANDLER_MODIFY_REMOVE,  ///< action to remove a Pulse Count wakeup handler
} FLEX_PulseCounterWakeupHandlerModifyAction;

/// Wakeup Handler Function Pointer Declaration.
typedef void (*FLEX_PCNTWakeupHandler)(void);

/// Add or remove a wakeup handler that will be called when pulse counter is
/// triggered.
/// \param[in] Handler function pointer to the Pulse Count wakeup handler.
/// \param[in] Action Add/Remove the input Pulse Count wakeup handler.
/// \return FLEX_SUCCESS (0) if succeeded and < 0 if failed.
/// \retval -FLEX_ERROR_EALREADY: handler already exists, remove first
/// \retval -FLEX_ERROR_EINVAL: attempt to remove non-existant handler
int FLEX_PulseCounterHandlerModify(const FLEX_PCNTWakeupHandler Handler,
  const FLEX_PulseCounterWakeupHandlerModifyAction Action);

///@}

/// @defgroup Delays Delays
/// Application delays.
///@{

/// Delay for a number of milliseconds.
/// \param[in] mSec delay time in milliseconds.
void FLEX_DelayMs(const uint32_t mSec);
/// Delay for a number of microseconds.
/// \param[in] uSec delay time in microseconds.
void FLEX_DelayUs(const uint32_t uSec);
/// Put the system in lower power mode for \p Sec seconds.
/// The calling job won't be interrupted by other jobs except for events while
/// sleeping.
/// \param[in] Sec sleep time in seconds.
void FLEX_Sleep(const uint32_t Sec);

///@}

/// @defgroup Time_Location Time and Location
/// GNSS interface on the FlexSense board.
/// \subsection sync_job_note GNSS Location and Time Synchronisation Job
/// The system in the background schedules a periodic task to synchronise
/// the system location and time, with the GNSS location and time by calling
/// the FLEX_GNSSFix() function.
/// This job runs periodically with increasing intervals until a GNSS fix is
/// obtained, once every week. This helps to compensate for system clock drift,
/// but also ensures that there is an accurate location fix.
/// Please also note that when building user applications, enabling the
/// \p skip_gnss option in the build will disable this GNSS synchronisation
/// feature and warning messages will be printed on the debug console.
/// During production builds, please validate that the \p skip_gnss option is
/// disabled, to ensure correct operation of the FlexSense device.
/// View section "Building the User Application" for more information regarding
/// the \p skip_gnss option.
///@{

/// Performs a GNSS FIX. On success it outputs the fixed GNSS
/// latitude, longitude, and time values. It also will updates the
/// systems real-time clock and latitude and longitude values.
/// \param[out] Lat the recorded latitude.
/// \param[out] Lon the recorded longitude.
/// \param[out] Time the recored fix time.
/// \return FLEX_SUCCESS (0) if succeeded and < 0 if failed.
/// \retval -FLEX_ERROR_GNSS: error on gnss device, failed to read
/// \retval -FLEX_ERROR_POWER_OUT: failed to enable power to module
int FLEX_GNSSFix(int32_t *const Lat, int32_t *const Lon, time_t *const Time);

/// Returns true if the system has obtained a valid time and location fix.
bool FLEX_GNSSHasValidFix(void);

/// Gets the the latitude, longitude, and time of the last GNSS fix.
/// \param[out] LastLatitude the last GNSS fixes latitude in degrees multiplied by 1e7.
/// \param[out] LastLongitude the last GNSS fixes longitude in degrees multiplied by 1e7.
/// \param[out] LastFixTime the time of the last GNSS fix as a epoch time.
void FLEX_LastLocationAndLastFixTime(int32_t *const LastLatitude, int32_t *const LastLongitude,
  time_t *const LastFixTime);

/// Get current epoch time
time_t FLEX_TimeGet(void);

///@}

/// @defgroup User_Msg User Message
/// Build and control user messages for the application.
///@{

/// Maximum size in bytes of individual transmit message.
#define FLEX_MAX_MESSAGE_SIZE 20
/// Schedule a message of bytes of given size for transmission.
/// The maximum message size is given by #FLEX_MAX_MESSAGE_SIZE.
/// Regardless of the value of \p MessageSize the number bytes
/// consumed is FLEX_MAX_MESSAGE_SIZE. Calling ScheduleMessage when the
/// number of bytes returned by MessageBytesFree is less than FLEX_MAX_MESSAGE_SIZE
/// replaces an existing message in the queue. This may result in dropped
/// messages. See also MessageBytesFree.
/// \param[in] Message pointer to the message to be scheduled.
/// \param[in] MessageSize length of the message.
/// \return FLEX_SUCCESS (0) if succeeded and < 0 if failed.
int FLEX_MessageSchedule(const uint8_t *const Message, const size_t MessageSize);
/// Returns number of bytes remaining in internal queue, that is,
/// the number of bytes that can be scheduled with ScheduleMessage.
/// \return number of bytes remaining in internal queue.
size_t FLEX_MessageBytesFree(void);
/// Save all messages in the message queue to module's persistent storage.
/// Saved messages will be transmitted after reset.
void FLEX_MessageSave(void);
/// Clear all messages in the message queue.
void FLEX_MessageQueueClear(void);

///@}

/// @defgroup Device Control
/// @{

/// Message Receive Handler Modify Actions
typedef enum {
  FLEX_MESSAGE_RECEIVE_HANDLER_MODIFY_ADD,     ///< action to add a receive handler
  FLEX_MESSAGE_RECEIVE_HANDLER_MODIFY_REMOVE,  ///< action to remove a receive handler
} FLEX_MessageReceiveHandlerModifyAction;

/// Message Receive Handler Function Pointer Declaration.
/// \param message A pointer to the message if size is greater than 0 else NULL.
/// \param size The length of received message.
typedef void (*FLEX_MessageReceiveHandler)(uint8_t *const message, const int size);

/// Add or remove a handler that will be called when a message is received.
/// \param[in] Handler function pointer to the Message Receive handler.
/// \param[in] Action Add/Remove the input Message Receive handler.
/// \return FLEX_SUCCESS (0) if succeeded and < 0 if failed.
/// \retval -FLEX_ERROR_EALREADY: handler already exists, remove first
/// \retval -FLEX_ERROR_EINVAL: attempt to remove non-existant handler
int FLEX_MessageReceiveHandlerModify(const FLEX_MessageReceiveHandler Handler,
  const FLEX_MessageReceiveHandlerModifyAction Action);

/// @}

/// @defgroup Time_Job_Scheduling Time and Job Scheduling
/// Create and schedule jobs for the application.
///@{

/// Scheduled Job Function Pointer Declaration.
/// The return value is the time at which the job should next run.
typedef time_t (*FLEX_ScheduledJob)(void);
/// Reset the schedule for an existing job or add a new job.
/// \param[in] Job function pointer to the job to be scheduled.
/// \param[in] Time time to run the job.
/// \return FLEX_SUCCESS (0) if succeeded and < 0 if failed.
/// \retval -FLEX_ERROR_ENOMEM:  if maximum number of jobs reached.
int FLEX_JobSchedule(const FLEX_ScheduledJob Job, const time_t Time);

/// Use this function to schedule a job to run immediately,
/// by assigning the return value of this function to the time value
/// at which the job should next run.
/// \return time at which the job will next run.
time_t FLEX_ASAP(void);
/// Use this function to stop a job being scheduled again,
/// by assigning the return value of this function to the time value
/// at which the job should next run.
/// \return time at which the job will next run.
time_t FLEX_Never(void);
/// Use this function to set time value at which a job will run next.
/// The job will run \p Secs after the current run.
/// \param[in] Secs seconds after to run the job next.
/// \return time at which the job will next run.
time_t FLEX_SecondsFromNow(const unsigned Secs);
/// Use this function to set time value at which a job will run next.
/// The job will run \p Mins after the current run.
/// \param[in] Mins minutes after to run the job next.
/// \return time at which the job will next run.
time_t FLEX_MinutesFromNow(const unsigned Mins);
/// Use this function to set time value at which a job will run next.
/// The job will run \p Hours after the current run.
/// \param[in] Hours hours after to run the job next.
/// \return time at which the job will next run.
time_t FLEX_HoursFromNow(const unsigned Hours);
/// Use this function to set time value at which a job will run next.
/// The job will run \p Days after the current run.
/// \param[in] Days days after to the run the job next.
/// \return time at which the job will next run.
time_t FLEX_DaysFromNow(const unsigned Days);

///@}

/// @defgroup Mod_ID_and_Reg_ID Module ID and Part Number
/// Module and Registration ID of the FlexSense board.
///@{

/// Returns the string of module ID in the format of "00xxxxxxxx Mx-2x" where
/// 00xxxxxxxx is the module ID in hexadecimal string and Mx-2x is the module's
/// part number.
/// \return null if module ID is not available.
char *FLEX_ModuleIDGet(void);

/// Returns the string of registration code.
/// \return null if registration code is not available.
char *FLEX_RegistrationCodeGet(void);

///@}

/// @defgroup Temp_Sensor Temperature Sensor
/// Temperature Sensor interface on the FlexSense board.
///@{

/// Get the temperature inside the module in degrees Celsius.
/// \param[out] Temperature recorded temperature value in degrees Celsius.
/// \return FLEX_SUCCESS (0) if succeeded and < 0 if failed.
int FLEX_TemperatureGet(float *const Temperature);

///@}

/// @defgroup Tests Tests
/// Run tests on the FlexSense board.
///@{

/// Performs a Hardware test for the FlexSense. The test enables the 5V power
/// on the external connector, and blinks the boards led from green to blue.
/// The test block all other board functionality until push button pressed.
/// \return FLEX_SUCCESS (0) if succeeded and < 0 if failed.
/// \retval -FLEX_ERROR_POWER_OUT: unable to initialise power out or LED
/// \retval -FLEX_ERROR_BUTTON: error enabling or disabling push button
/// \retval -FLEX_ERROR_LED: error setting LED state
int FLEX_HWTest(void);

///@}

/// @defgroup System_Tick System Tick
/// System tick.
///@{

/// Get the current system tick (1000 ticks per second).
/// \return the current system tick
uint32_t FLEX_TickGet(void);

///@}

#endif /* FLEX_H */
