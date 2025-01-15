/// \file modbus.h Myriota Modbus Driver
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

#ifndef MYRIOTA_MODBUS_H
#define MYRIOTA_MODBUS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

/** \defgroup Modbus Modbus Library
 * Flex Library versioning information.
 * \{
 */

/** Error and exception codes for the modbus driver. */
typedef enum {
  MODBUS_SUCCESS = 0,
  MODBUS_ERROR_EXCEPTION_ILLEGAL_FUNCTION = 0x01,
  MODBUS_ERROR_EXCEPTION_ILLEGAL_DATA_ADDRESS = 0x02,
  MODBUS_ERROR_EXCEPTION_ILLEGAL_DATA_VALUE = 0x03,
  MODBUS_ERROR_EXCEPTION_SLAVE_DEVICE_FAILURE = 0x04,
  MODBUS_ERROR_EXCEPTION_ACKNOWLEDGE = 0x05,
  MODBUS_ERROR_EXCEPTION_SLAVE_DEVICE_BUSY = 0x06,
  MODBUS_ERROR_EXCEPTION_NEGATIVE_ACKNOWLEDGMENT = 0x07,
  MODBUS_ERROR_EXCEPTION_MEMORY_PARITY_ERROR = 0x08,
  MODBUS_ERROR_EXCEPTION_GATEWAY_PATH_UNAVAILABLE = 0x0A,
  MODBUS_ERROR_EXCEPTION_GATEWAY_TARGET_DEVICE_FAILED_TO_RESPOND = 0x0B,
  MODBUS_ERROR_INVALID_HANDLE,
  MODBUS_ERROR_INVALID_CRC16,
  MODBUS_ERROR_MALFORMED_RESPONSE,
  MODBUS_ERROR_RESPONSE_FROM_WRONG_SLAVE_ADDRESS,
  MODBUS_ERROR_IO_FAILURE,
  MODBUS_ERROR_BAD_STATE,
  MODBUS_ERROR_OVERFLOW,
} MYRIOTA_ModbusErrors;

/** Modbus driver instance handle type. */
typedef uint8_t MYRIOTA_ModbusHandle;

/** Modbus device address type. */
typedef uint8_t MYRIOTA_ModbusDeviceAddress;

/** Modbus data (i.e. coils/registers) address type. */
typedef uint16_t MYRIOTA_ModbusDataAddress;

/**
 * Initialization function for the serial interface used by Modbus driver.
 *
 * \param[in,out] ctx The user defined data context used by the serial interface.
 * \return 0 on success else < 0 on error.
 */
typedef int (*MYRIOTA_ModbusSerialInterfaceInitFn_t)(void *const ctx);

/**
 * De-initialization function for the serial interface used by Modbus driver.
 *
 * \param[in,out] ctx The user defined data context used by the serial interface.
 * \return the number of bytes written on success else < 0 on error.
 */
typedef void (*MYRIOTA_ModbusSerialInterfaceDeinitFn_t)(void *const ctx);

/**
 * Read function for the serial interface used by Modbus driver.
 *
 * \param[in,out] ctx The user defined data context used by the serial interface.
 * \param[out] buffer The buffer for filling with bytes read by the serial device.
 * \param[in] count The total size of the buffer in bytes.
 * \return the number of bytes read on success, else < 0 on error.
 */
typedef ssize_t (*MYRIOTA_ModbusSerialInterfaceReadFn_t)(void *const ctx, uint8_t *const buffer,
  const size_t count);

/**
 * Write function for the serial interface used by Modbus driver.
 *
 * \param[in,out] ctx The user defined data context used by the serial interface.
 * \param[out] buffer The buffer of bytes to write to the serial device.
 * \param[in] count The number of bytes to write from the buffer.
 * \return the number of bytes written on success, else < 0 on error.
 */
typedef ssize_t (*MYRIOTA_ModbusSerialInterfaceWriteFn_t)(void *const ctx,
  const uint8_t *const buffer, const size_t count);

/** Interface for the serial device used by the Modbus driver. */
typedef struct {
  /** User defined data context to be used by the serial interfaces functions. */
  void *ctx;
  /** Serial device initialization function. */
  MYRIOTA_ModbusSerialInterfaceInitFn_t init;
  /** Serial device de-initialization function. */
  MYRIOTA_ModbusSerialInterfaceDeinitFn_t deinit;
  /** Serial device read function. */
  MYRIOTA_ModbusSerialInterfaceReadFn_t read;
  /** Serial device write function. */
  MYRIOTA_ModbusSerialInterfaceWriteFn_t write;
} MYRIOTA_ModbusSerialInterface;

/**
 * The framing mode to be used by the Modbus driver.
 * \note Only RTU framing is supported at the moment, but ASCII framing will be
 * added in the future.
 */
typedef enum {
  /** RTU (Remote Transmission Unit) Framing */
  MODBUS_FRAMING_MODE_RTU,
} MYRIOTA_ModbusFramingMode;

/** Initialization options for Modbus driver */
typedef struct {
  /** The Modbus driver's framing mode*/
  MYRIOTA_ModbusFramingMode framing_mode;
  /** The Modbus driver's serial interface */
  MYRIOTA_ModbusSerialInterface serial_interface;
} MYRIOTA_ModbusInitOptions;

/**
 * Initializes a Modbus driver instance.
 *
 * \param[in] options The driver options to initialise with.
 * \returns a Modus handle > 0 on success else <= 0 on error.
 */
MYRIOTA_ModbusHandle MYRIOTA_ModbusInit(const MYRIOTA_ModbusInitOptions options);

/*
 * De-initializes a Modbus driver instance.
 *
 * \param[in] handle The handle for the Modbus driver to de-initialize.
 */
void MYRIOTA_ModbusDeinit(const MYRIOTA_ModbusHandle handle);

/**
 * Enables the Modbus driver.
 *
 * \note Disabling the modbus driver helps to preserve device power.
 *
 * \param[in] handle The handle for the Modbus driver to enable.
 * \returns a Modus handle > 0 on success else <= 0 on error.
 */
int MYRIOTA_ModbusEnable(const MYRIOTA_ModbusHandle handle);

/**
 * Disable the Modbus driver.
 *
 * \note Disabling the modbus driver helps to preserve device power.
 *
 * \param[in] handle The handle for the Modbus driver to disable.
 * \returns a Modus handle > 0 on success else <= 0 on error.
 */
int MYRIOTA_ModbusDisable(const MYRIOTA_ModbusHandle handle);

/**
 * Given a buffer of bytes packed using Modbus's byte format set the
 * value of the bit at the given index.
 *
 * \note In this context `bit_index` can be interpreted as the
 * (coil_address - coil_start_address) where coil_start_address is bit_index
 * zero.
 *
 * \param[out] bytes A buffer of bytes using Modbus's byte packing format.
 * \param[in] count The size of the buffer must be at least 1
 * \param[in] bit_index The index of the bit you would like to set, where 0 =< index < (count / 8).
 * \param[in] value The boolean value of the bit where 1 = true and 0 = false.
 */
void MYRIOTA_ModbusBytesSetBit(uint8_t *const bytes, const size_t count, const uint8_t bit_index,
  const bool value);

/**
 * Given a buffer of bytes packed using Modbus's byte format get the
 * value of the bit at the given index.
 *
 * \note In this context `bit_index` can be interpreted as the
 * (coil_address - coil_start_address) where coil_start_address is bit_index
 * zero.
 *
 * \param[in] bytes A buffer of bytes using Modbus's byte packing format.
 * \param[in] count The size of the buffer must be at least 1
 * \param[in] bit_index The index of the bit you would like to set, where 0 =< index < (count / 8).
 * \param[out] value The boolean value of the bit where 1 = true and 0 = false.
 */
void MYRIOTA_ModbusBytesGetBit(const uint8_t *const bytes, const size_t count,
  const uint8_t bit_index, bool *const value);

/*
 * Read the values of a list of consecutive coils.
 *
 * \param[in] handle The handle for the Modbus driver to read from.
 * \param[in] slave The address of the slave device to read from.
 * \param[in] addr The start address of the coils to read from.
 * \param[in] count The number of coils to read.
 * \param[out] bytes The buffer to fill with the values of the coils where the size
 * of the buffer must = ceil((count / 8)). (NOTE: Shall be in Modbus MSB bit packed format)
 * \return 0 on success else < 0 on error.
 */
int MYRIOTA_ModbusReadCoils(const MYRIOTA_ModbusHandle handle,
  const MYRIOTA_ModbusDeviceAddress slave, const MYRIOTA_ModbusDataAddress addr, const size_t count,
  uint8_t *const bytes);

/*
 * Read the values of a list of consecutive discrete inputs.
 *
 * \param[in] handle The handle for the Modbus driver to read from.
 * \param[in] slave The address of the slave device to read from.
 * \param[in] addr The start address of the discrete inputs to read from.
 * \param[in] count The number of discrete inputs to read.
 * \param[out] bytes The buffer to fill with the values of the discrete inputs where
 * the size of the buffer must = ceil((count / 8)). (NOTE: Shall be in Modbus MSB bit packed format)
 * \return 0 on success else < 0 on error
 */
int MYRIOTA_ModbusReadDiscreteInputs(const MYRIOTA_ModbusHandle handle,
  const MYRIOTA_ModbusDeviceAddress slave, const MYRIOTA_ModbusDataAddress addr, const size_t count,
  uint8_t *const bytes);

/*
 * Read the values of a list of consecutive holding registers.
 *
 * \param[in] handle The handle for the Modbus driver to read from.
 * \param[in] slave The address of the slave device to read from.
 * \param[in] addr The start address of the holding registers to read from.
 * \param[in] count The number of holding registers to read.
 * \param[out] bytes The buffer to fill with the values of the holding registers,
 * where the size of the buffer must = count * 2.
 * \return 0 on success else < 0 on error.
 */
int MYRIOTA_ModbusReadHoldingRegisters(const MYRIOTA_ModbusHandle handle,
  const MYRIOTA_ModbusDeviceAddress slave, const MYRIOTA_ModbusDataAddress addr, const size_t count,
  uint8_t *const bytes);

/*
 * Read the values of a list of consecutive input registers.
 *
 * \param[in] handle The handle for the Modbus driver to read from.
 * \param[in] slave The address of the slave device to read from.
 * \param[in] addr The start address of the holding registers to read from.
 * \param[in] count The number of holding registers to read.
 * \param[out] bytes The buffer to fill with the values of the input registers,
 * where the size of the buffer must = count * 2.
 * \return 0 on success else < 0 on error.
 */
int MYRIOTA_ModbusReadInputRegisters(const MYRIOTA_ModbusHandle handle,
  const MYRIOTA_ModbusDeviceAddress slave, const MYRIOTA_ModbusDataAddress addr, const size_t count,
  uint8_t *const bytes);

/*
 * Write a value to a coil.
 *
 * \param[in] handle The handle for the Modbus driver to write to.
 * \param[in] slave The address of the slave device to write to.
 * \param[in] addr The address of the coils to write to.
 * \param[in] word The word to write to the coil, where word must be either
 * 0x00FF = 1 or 0x0000 = 0 (NOTE: These values are the Modbus MSB packed format).
 * \return 0 on success else < 0 on error.
 */
int MYRIOTA_ModbusWriteCoil(const MYRIOTA_ModbusHandle handle,
  const MYRIOTA_ModbusDeviceAddress slave, const MYRIOTA_ModbusDataAddress addr,
  const uint16_t word);

/*
 * Write a value to a holding register.
 *
 * \param[in] handle The handle for the Modbus driver to write to.
 * \param[in] slave The address of the slave device to write to.
 * \param[in] addr The address of the holding registers to write to.
 * \param[in] word The word to write to the holding registers.
 * \return 0 on success else < 0 on error.
 */
int MYRIOTA_ModbusWriteHoldingRegister(const MYRIOTA_ModbusHandle handle,
  const MYRIOTA_ModbusDeviceAddress slave, const MYRIOTA_ModbusDataAddress addr,
  const uint16_t word);

/*
 * Writes values to a list of consecutive coils.
 *
 * \param[in] handle The handle for the Modbus driver to write from.
 * \param[in] slave The address of the slave device to write from.
 * \param[in] addr The start address of the coils to write from.
 * \param[in] count The number of coils to write.
 * \param[in] bytes The buffer to fill with the values of the coils where the size
 * of the buffer must = ceil((count / 8)). (NOTE: Must be in Modbus MSB bit packed format)
 * \return 0 on success else < 0 on error.
 */
int MYRIOTA_ModbusWriteCoils(const MYRIOTA_ModbusHandle handle,
  const MYRIOTA_ModbusDeviceAddress slave, const MYRIOTA_ModbusDataAddress addr, const size_t count,
  const uint8_t *const bytes);

/*
 * Write values to a list of consecutive holding registers.
 *
 * \param[in] handle The handle for the Modbus driver to write from.
 * \param[in] slave The address of the slave device to write from.
 * \param[in] addr The start address of the input registers to write from.
 * \param[in] count The number of input registers to write.
 * \param[in] bytes The buffer to fill with the values of the holding registers
 * where the size of the buffer must = count * 2.
 * \return 0 on success else < 0 on error.
 */
int MYRIOTA_ModbusWriteHoldingRegisters(const MYRIOTA_ModbusHandle handle,
  const MYRIOTA_ModbusDeviceAddress slave, const MYRIOTA_ModbusDataAddress addr, const size_t count,
  const uint8_t *const bytes);

/**
 * \}
 */

#endif /* MYRIOTA_MODBUS_H */
