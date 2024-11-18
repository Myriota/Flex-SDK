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

#include "myriota/modbus.h"
#include <string.h>

// NOTE: you can provide your own assert
#ifndef MODBUS_ASSERT
#include <stdio.h>
#define MODBUS_ASSERT(cond)                          \
  do {                                               \
    if (!(cond)) {                                   \
      printf("Assert @%s:%d\n", __FILE__, __LINE__); \
      while (1) {                                    \
      }                                              \
    }                                                \
  } while (0)
#endif
#define MODBUS_UNREACHABLE MODBUS_ASSERT(false)
#define MODBUS_ARRAY_SIZE(array) (sizeof(array) / sizeof(*array))

#define MODBUS_ADU_BUFFER_SIZE 256
// ADU has at least a slave address, a PDU with a function code, and a crc16.
#define MODBUS_ADU_MIN_SIZE 4
// PDU is at maximum the max size of the ADU minus the slave address and the crc16.
#define MODBUS_PDU_MAX_SIZE (MODBUS_ADU_BUFFER_SIZE - 3)

// NOTE: Increase to support being run on system with more then one Modbus interface.
#ifndef MODBUS_INSTANCE_MAX
#define MODBUS_INSTANCE_MAX 1
#endif

// TODO: Add support for unsupported commands
enum modbus_function_code {
  MODBUS_FUNCTION_CODE_READ_COILS = 0x01,
  MODBUS_FUNCTION_CODE_READ_DISCRETE_INPUTS = 0x02,
  MODBUS_FUNCTION_CODE_READ_HOLDING_REGISTERS = 0x03,
  MODBUS_FUNCTION_CODE_READ_INPUT_REGISTERS = 0x04,
  MODBUS_FUNCTION_CODE_WRITE_SINGLE_COIL = 0x05,
  MODBUS_FUNCTION_CODE_WRITE_SINGLE_REGISTER = 0x06,
  // MODBUS_FUNCTION_CODE_READ_EXCEPTION_STATUS = 0x07,
  // MODBUS_FUNCTION_CODE_DIAGNOSTICS = 0x08,
  // MODBUS_FUNCTION_CODE_COMM_EVENT_COUNTER = 0x0B,
  // MODBUS_FUNCTION_CODE_COMM_EVENT_LOG = 0x0C,
  MODBUS_FUNCTION_CODE_WRITE_MULTIPLE_COILS = 0x0F,
  MODBUS_FUNCTION_CODE_WRITE_MULTIPLE_REGISTERS = 0x10,
  // MODBUS_FUNCTION_CODE_REPORT_SLAVE_ID = 0x11,
  // MODBUS_FUNCTION_CODE_READ_FILE_RECORD = 0x14,
  // MODBUS_FUNCTION_CODE_WRITE_FILE_RECORD = 0x15,
  // MODBUS_FUNCTION_CODE_MASK_WRITE_REGISTER = 0x16,
  // MODBUS_FUNCTION_CODE_READ_WRITE_MULTIPLE_REGISTERS = 0x17,
  // MODBUS_FUNCTION_CODE_READ_FIFO_QUEUE = 0x18,
  // MODBUS_FUNCTION_CODE_ENCAPSULATED_INTERFACE_TRANSPORT = 0x2B,
  MODBUS_FUNCTION_CODE_ERROR_BASE = 0x80,
};

// enum modbus_encapsulated_interface_type {
//   MODBUS_ENCAPSULATED_INTERFACE_TYPE_CANOPEN_GENERAL_REFERENCE_REQUEST_AND_RESPONSE_PDU = 0x0D,
//   MODBUS_ENCAPSULATED_INTERFACE_TYPE_READ_DEVICE_IDENTIFICATION = 0x0E,
// };

struct protocol_data_unit_parser {
  enum modbus_function_code function_code;
  const uint8_t *ptr;
  const uint8_t *end;
};

struct application_data_uint {
  size_t size;
  uint8_t buffer[MODBUS_ADU_BUFFER_SIZE];
};

struct modbus_instance {
  bool initialized;
  bool enabled;
  MYRIOTA_ModbusFramingMode framing_mode;
  MYRIOTA_ModbusSerialInterface serial_interface;
  struct application_data_uint adu_tx;
  struct application_data_uint adu_rx;
};

static struct modbus_instance modbus_instances[MODBUS_INSTANCE_MAX] = {0};

static struct modbus_instance *get_modbus_instance(const MYRIOTA_ModbusHandle handle) {
  if (handle == 0) {
    return NULL;
  }

  const uint8_t index = handle - 1;
  if (index >= MODBUS_INSTANCE_MAX) {
    return NULL;
  }

  struct modbus_instance *instance = &modbus_instances[index];
  if (!instance->initialized) {
    return NULL;
  }

  return instance;
}

static uint16_t modbus_calulate_crc16(const uint8_t *const buffer, const size_t size) {
  static const uint16_t lut[] = {0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
    0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440, 0xCC01, 0x0CC0, 0x0D80, 0xCD41,
    0x0F00, 0xCFC1, 0xCE81, 0x0E40, 0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
    0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40, 0x1E00, 0xDEC1, 0xDF81, 0x1F40,
    0xDD01, 0x1DC0, 0x1C80, 0xDC41, 0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
    0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040, 0xF001, 0x30C0, 0x3180, 0xF141,
    0x3300, 0xF3C1, 0xF281, 0x3240, 0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
    0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41, 0xFA01, 0x3AC0, 0x3B80, 0xFB41,
    0x3900, 0xF9C1, 0xF881, 0x3840, 0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
    0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40, 0xE401, 0x24C0, 0x2580, 0xE541,
    0x2700, 0xE7C1, 0xE681, 0x2640, 0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
    0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240, 0x6600, 0xA6C1, 0xA781, 0x6740,
    0xA501, 0x65C0, 0x6480, 0xA441, 0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
    0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840, 0x7800, 0xB8C1, 0xB981, 0x7940,
    0xBB01, 0x7BC0, 0x7A80, 0xBA41, 0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
    0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640, 0x7200, 0xB2C1, 0xB381, 0x7340,
    0xB101, 0x71C0, 0x7080, 0xB041, 0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
    0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440, 0x9C01, 0x5CC0, 0x5D80, 0x9D41,
    0x5F00, 0x9FC1, 0x9E81, 0x5E40, 0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
    0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40, 0x4E00, 0x8EC1, 0x8F81, 0x4F40,
    0x8D01, 0x4DC0, 0x4C80, 0x8C41, 0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
    0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040};
  uint16_t crc = 0xFFFF;
  for (size_t i = 0; i < size; ++i) {
    const uint8_t xor = buffer[i] ^ crc;
    crc >>= 8;
    crc ^= lut[xor];
  }
  return crc;
}

static inline uint8_t hi_u16(const uint16_t value) {
  return (uint8_t)(value >> 8);
}

static inline uint8_t low_u16(const uint16_t value) {
  return (uint8_t)value;
}

static inline uint16_t merge_u16(const uint8_t hi, const uint8_t low) {
  return ((uint16_t)hi << 8) | (uint16_t)low;
}

static inline bool is_read_function_code(const enum modbus_function_code function_code) {
  return function_code == MODBUS_FUNCTION_CODE_READ_COILS ||
         function_code == MODBUS_FUNCTION_CODE_READ_DISCRETE_INPUTS ||
         function_code == MODBUS_FUNCTION_CODE_READ_HOLDING_REGISTERS ||
         function_code == MODBUS_FUNCTION_CODE_READ_INPUT_REGISTERS;
}

static inline bool is_read_register(const enum modbus_function_code function_code) {
  return function_code == MODBUS_FUNCTION_CODE_READ_HOLDING_REGISTERS ||
         function_code == MODBUS_FUNCTION_CODE_READ_INPUT_REGISTERS;
}

static inline bool is_write_function_code(const enum modbus_function_code function_code) {
  return function_code == MODBUS_FUNCTION_CODE_WRITE_SINGLE_COIL ||
         function_code == MODBUS_FUNCTION_CODE_WRITE_SINGLE_REGISTER ||
         function_code == MODBUS_FUNCTION_CODE_WRITE_MULTIPLE_COILS ||
         function_code == MODBUS_FUNCTION_CODE_WRITE_MULTIPLE_REGISTERS;
}

static inline bool is_write_multiple(const enum modbus_function_code function_code) {
  return function_code == MODBUS_FUNCTION_CODE_WRITE_MULTIPLE_COILS ||
         function_code == MODBUS_FUNCTION_CODE_WRITE_MULTIPLE_REGISTERS;
}

static inline bool is_write_multiple_coil(const enum modbus_function_code function_code) {
  return function_code == MODBUS_FUNCTION_CODE_WRITE_MULTIPLE_COILS;
}

static inline enum modbus_function_code get_error_function_code(
  const enum modbus_function_code function_code) {
  return function_code | MODBUS_FUNCTION_CODE_ERROR_BASE;
}

static inline void application_data_unit_pack_u8(struct application_data_uint *const adu,
  const uint8_t value) {
  MODBUS_ASSERT(adu != NULL);
  MODBUS_ASSERT(adu->size < MODBUS_ADU_BUFFER_SIZE);
  adu->buffer[adu->size++] = value;
}

static inline void application_data_unit_pack_u16(struct application_data_uint *const adu,
  const uint16_t value) {
  MODBUS_ASSERT(adu != NULL);
  MODBUS_ASSERT((adu->size + 1) < MODBUS_ADU_BUFFER_SIZE);
  adu->buffer[adu->size++] = hi_u16(value);
  adu->buffer[adu->size++] = low_u16(value);
}

static inline void application_data_unit_pack_crc16(struct application_data_uint *const adu,
  const uint16_t crc) {
  MODBUS_ASSERT(adu != NULL);
  MODBUS_ASSERT((adu->size + 1) < MODBUS_ADU_BUFFER_SIZE);
  adu->buffer[adu->size++] = low_u16(crc);
  adu->buffer[adu->size++] = hi_u16(crc);
}

static inline void application_data_unit_pack_bytes(struct application_data_uint *const adu,
  const uint8_t *const bytes, const size_t nbytes) {
  MODBUS_ASSERT(adu != NULL);
  MODBUS_ASSERT((adu->size + nbytes) <= MODBUS_ADU_BUFFER_SIZE);
  memcpy(&adu->buffer[adu->size], bytes, nbytes);
  adu->size += nbytes;
}

static void begin_application_data_unit_pack(struct application_data_uint *const adu,
  const MYRIOTA_ModbusDeviceAddress slave_address, const enum modbus_function_code function_code) {
  MODBUS_ASSERT(adu != NULL);
  adu->size = 0;
  application_data_unit_pack_u8(adu, slave_address);
  application_data_unit_pack_u8(adu, function_code);
}

static void end_application_data_unit_pack(struct application_data_uint *const adu) {
  MODBUS_ASSERT(adu != NULL);
  const uint16_t crc16 = modbus_calulate_crc16(adu->buffer, adu->size);
  application_data_unit_pack_crc16(adu, crc16);
}

static uint8_t protocol_data_unit_unpack_u8(struct protocol_data_unit_parser *const parser) {
  MODBUS_ASSERT(parser != NULL);
  MODBUS_ASSERT(parser->ptr < parser->end);
  const uint8_t value = parser->ptr[0];
  ++parser->ptr;
  return value;
}

static int protocol_data_unit_parser(const struct application_data_uint *const adu,
  const MYRIOTA_ModbusDeviceAddress slave_address_out,
  const enum modbus_function_code function_code, struct protocol_data_unit_parser *const parser) {
  MODBUS_ASSERT(adu != NULL);
  MODBUS_ASSERT(parser != NULL);
  MODBUS_ASSERT(adu->size >= MODBUS_ADU_MIN_SIZE);

  // Application Data Unit (ADU)/(Protocol Data Unit (PDU) Packing Diagram
  // | 0     | Slave Address |
  // | 1     | Function Code |
  // | 2*    | PDU Payload   |
  // | N - 2 | CRC16 High    |
  // | N - 1 | CRC16 Low     |
  // Where N is the size of the ADU packet.
  // * The PDU payload will be at index 2 if it exist. If the message doesn't
  // have a payload, then index 2 will be the CRC16 High.
  const MYRIOTA_ModbusDeviceAddress slave_address_in = adu->buffer[0];
  parser->function_code = adu->buffer[1];
  parser->ptr = &adu->buffer[2];
  parser->end = parser->ptr + (adu->size - MODBUS_ADU_MIN_SIZE);

  const uint8_t packed_crc16_high = adu->buffer[adu->size - 1];
  const uint8_t packed_crc16_low = adu->buffer[adu->size - 2];
  const uint16_t packed_crc16 = merge_u16(packed_crc16_high, packed_crc16_low);

  const size_t size = adu->size - 2;
  const uint16_t calculated_crc16 = modbus_calulate_crc16(adu->buffer, size);
  if (packed_crc16 != calculated_crc16) {
    return -MODBUS_ERROR_INVALID_CRC16;
  }

  if (slave_address_out != slave_address_in) {
    return -MODBUS_ERROR_RESPONSE_FROM_WRONG_SLAVE_ADDRESS;
  }

  // Exception Responses
  // | 1 | Function Code with Most Significant Bit set |
  // | 2 | Exception code                              |
  if (parser->function_code != function_code) {
    if (parser->function_code == get_error_function_code(function_code)) {
      const uint8_t exception_code = protocol_data_unit_unpack_u8(parser);
      return -exception_code;
    }
    return -MODBUS_ERROR_MALFORMED_RESPONSE;
  }

  return MODBUS_SUCCESS;
}

static int modbus_transmit(struct modbus_instance *const instance) {
  MODBUS_ASSERT(instance != NULL);
  const MYRIOTA_ModbusSerialInterface *const serial = &instance->serial_interface;

  const uint8_t *tx_buffer = instance->adu_tx.buffer;
  size_t tx_nbytes = instance->adu_tx.size;
  while (tx_nbytes > 0) {
    ssize_t nbytes = serial->write(serial->ctx, tx_buffer, tx_nbytes);
    if (nbytes < 0) {
      return -MODBUS_ERROR_IO_FAILURE;
    }
    tx_nbytes = ((size_t)nbytes > tx_nbytes) ? 0 : tx_nbytes - nbytes;
    tx_buffer += nbytes;
  }

  const ssize_t rx_nbytes =
    serial->read(serial->ctx, instance->adu_rx.buffer, MODBUS_ADU_BUFFER_SIZE);
  if (rx_nbytes <= 0) {
    return -MODBUS_ERROR_IO_FAILURE;
  }
  instance->adu_rx.size = rx_nbytes;

  return MODBUS_SUCCESS;
}

static int modbus_read(const MYRIOTA_ModbusHandle handle,
  const MYRIOTA_ModbusDeviceAddress slave_address, const enum modbus_function_code function_code,
  const MYRIOTA_ModbusDataAddress data_address, const size_t count, uint8_t *const bytes) {
  MODBUS_ASSERT(is_read_function_code(function_code) == true);
  MODBUS_ASSERT(bytes != NULL);

  struct modbus_instance *instance = get_modbus_instance(handle);
  if (instance == NULL) {
    return -MODBUS_ERROR_INVALID_HANDLE;
  }

  if (!instance->enabled) {
    return -MODBUS_ERROR_BAD_STATE;
  }

  // For `read commands` packing descriptions see section 6.1, 6.2, 6.3, 6.4
  // of https://modbus.org/docs/Modbus_Application_Protocol_V1_1b.pdf.
  struct application_data_uint *const adu_tx = &instance->adu_tx;
  begin_application_data_unit_pack(adu_tx, slave_address, function_code);
  application_data_unit_pack_u16(adu_tx, data_address);
  application_data_unit_pack_u16(adu_tx, count);
  end_application_data_unit_pack(adu_tx);

  const int transmit_result = modbus_transmit(instance);
  if (transmit_result != MODBUS_SUCCESS) {
    return transmit_result;
  }

  struct protocol_data_unit_parser parser = {0};
  const int parser_result =
    protocol_data_unit_parser(&instance->adu_rx, slave_address, function_code, &parser);
  if (parser_result != MODBUS_SUCCESS) {
    return parser_result;
  }

  // NOTE/TODO: validate nbytes?
  const uint8_t nbytes = (is_read_register(function_code))
                           ? protocol_data_unit_unpack_u8(&parser) * 2
                           : protocol_data_unit_unpack_u8(&parser);
  memcpy(bytes, parser.ptr, nbytes);

  return MODBUS_SUCCESS;
}

static int modbus_write(const MYRIOTA_ModbusHandle handle,
  const MYRIOTA_ModbusDeviceAddress slave_address, const enum modbus_function_code function_code,
  const MYRIOTA_ModbusDataAddress data_address, const size_t count, const uint8_t *const bytes) {
  MODBUS_ASSERT(is_write_function_code(function_code) == true);
  MODBUS_ASSERT(bytes != NULL);

  struct modbus_instance *instance = get_modbus_instance(handle);
  if (instance == NULL) {
    return -MODBUS_ERROR_INVALID_HANDLE;
  }

  if (!instance->enabled) {
    return -MODBUS_ERROR_BAD_STATE;
  }

  // For `write commands` packing descriptions see section 6.5, 6.6, 6.11, 6.12
  // of https://modbus.org/docs/Modbus_Application_Protocol_V1_1b.pdf.
  struct application_data_uint *const adu_tx = &instance->adu_tx;
  begin_application_data_unit_pack(adu_tx, slave_address, function_code);
  application_data_unit_pack_u16(adu_tx, data_address);
  const uint8_t nbytes = (is_write_multiple_coil(function_code)) ? (count + 8 - 1) / 8 : count * 2;
  if (is_write_multiple(function_code)) {
    application_data_unit_pack_u16(adu_tx, count);
    application_data_unit_pack_u8(adu_tx, nbytes);
  }
  application_data_unit_pack_bytes(adu_tx, bytes, nbytes);
  end_application_data_unit_pack(adu_tx);

  const int transmit_result = modbus_transmit(instance);
  if (transmit_result != MODBUS_SUCCESS) {
    return transmit_result;
  }

  struct protocol_data_unit_parser parser = {0};
  const int parser_result =
    protocol_data_unit_parser(&instance->adu_rx, slave_address, function_code, &parser);
  if (parser_result != MODBUS_SUCCESS) {
    return parser_result;
  }

  // NOTE/TODO: validate echoed response?
  return MODBUS_SUCCESS;
}

MYRIOTA_ModbusHandle MYRIOTA_ModbusInit(const MYRIOTA_ModbusInitOptions options) {
  MYRIOTA_ModbusHandle result = -MODBUS_ERROR_INVALID_HANDLE;
  for (size_t i = 0; i < MODBUS_ARRAY_SIZE(modbus_instances); ++i) {
    if (modbus_instances[i].initialized == false) {
      modbus_instances[i].initialized = true;
      modbus_instances[i].framing_mode = options.framing_mode;
      modbus_instances[i].serial_interface = options.serial_interface;
      result = i + 1;
    }
  }
  return result;
}

void MYRIOTA_ModbusDeinit(const MYRIOTA_ModbusHandle handle) {
  struct modbus_instance *instance = get_modbus_instance(handle);
  if (instance != NULL && instance->initialized == true) {
    if (instance->enabled) {
      instance->serial_interface.deinit(instance->serial_interface.ctx);
    }
    instance->enabled = false;
    instance->initialized = false;
  }
}

int MYRIOTA_ModbusEnable(const MYRIOTA_ModbusHandle handle) {
  struct modbus_instance *instance = get_modbus_instance(handle);
  if (instance == NULL) {
    return -MODBUS_ERROR_INVALID_HANDLE;
  }

  if (instance->enabled) {
    return -MODBUS_ERROR_BAD_STATE;
  }

  if (instance->serial_interface.init(instance->serial_interface.ctx) < 0) {
    return -MODBUS_ERROR_IO_FAILURE;
  }
  instance->enabled = true;

  return MODBUS_SUCCESS;
}

int MYRIOTA_ModbusDisable(const MYRIOTA_ModbusHandle handle) {
  struct modbus_instance *instance = get_modbus_instance(handle);
  if (instance == NULL) {
    return -MODBUS_ERROR_INVALID_HANDLE;
  }

  if (!instance->enabled) {
    return -MODBUS_ERROR_BAD_STATE;
  }

  instance->serial_interface.deinit(instance->serial_interface.ctx);
  instance->enabled = false;

  return MODBUS_SUCCESS;
}

void MYRIOTA_ModbusBytesSetBit(uint8_t *const bytes, const size_t count, const uint8_t bit_index,
  const bool value) {
  const uint8_t byte_index = bit_index / 8;
  MODBUS_ASSERT(byte_index < count);

  const uint8_t bit_offset = bit_index % 8;
  const uint8_t mask = (0x1 << bit_offset);
  if (value) {
    bytes[byte_index] |= mask;
  } else {
    bytes[byte_index] &= ~mask;
  }
}

void MYRIOTA_ModbusBytesGetBit(const uint8_t *const bytes, const size_t count,
  const uint8_t bit_index, bool *const value) {
  const uint8_t byte_index = bit_index / 8;
  MODBUS_ASSERT(byte_index < count);

  const uint8_t bit_offset = bit_index % 8;
  const uint8_t mask = (0x1 << bit_offset);
  *value = ((bytes[byte_index] & mask) > 0);
}

int MYRIOTA_ModbusReadCoils(const MYRIOTA_ModbusHandle handle,
  const MYRIOTA_ModbusDeviceAddress slave, const MYRIOTA_ModbusDataAddress addr, const size_t count,
  uint8_t *const bytes) {
  return modbus_read(handle, slave, MODBUS_FUNCTION_CODE_READ_COILS, addr, count, bytes);
}

int MYRIOTA_ModbusReadDiscreteInputs(const MYRIOTA_ModbusHandle handle,
  const MYRIOTA_ModbusDeviceAddress slave, const MYRIOTA_ModbusDataAddress addr, const size_t count,
  uint8_t *const bytes) {
  return modbus_read(handle, slave, MODBUS_FUNCTION_CODE_READ_DISCRETE_INPUTS, addr, count, bytes);
}

int MYRIOTA_ModbusReadHoldingRegisters(const MYRIOTA_ModbusHandle handle,
  const MYRIOTA_ModbusDeviceAddress slave, const MYRIOTA_ModbusDataAddress addr, const size_t count,
  uint8_t *const bytes) {
  return modbus_read(handle, slave, MODBUS_FUNCTION_CODE_READ_HOLDING_REGISTERS, addr, count,
    bytes);
}

int MYRIOTA_ModbusReadInputRegisters(const MYRIOTA_ModbusHandle handle,
  const MYRIOTA_ModbusDeviceAddress slave, const MYRIOTA_ModbusDataAddress addr, const size_t count,
  uint8_t *const bytes) {
  return modbus_read(handle, slave, MODBUS_FUNCTION_CODE_READ_INPUT_REGISTERS, addr, count, bytes);
}

int MYRIOTA_ModbusWriteCoil(const MYRIOTA_ModbusHandle handle,
  const MYRIOTA_ModbusDeviceAddress slave, const MYRIOTA_ModbusDataAddress addr,
  const uint16_t word) {
  const uint8_t *const bytes = (const uint8_t *const)&word;
  return modbus_write(handle, slave, MODBUS_FUNCTION_CODE_WRITE_SINGLE_COIL, addr, 1, bytes);
}

int MYRIOTA_ModbusWriteHoldingRegister(const MYRIOTA_ModbusHandle handle,
  const MYRIOTA_ModbusDeviceAddress slave, const MYRIOTA_ModbusDataAddress addr,
  const uint16_t word) {
  const uint8_t *const bytes = (const uint8_t *const)&word;
  return modbus_write(handle, slave, MODBUS_FUNCTION_CODE_WRITE_SINGLE_REGISTER, addr, 1, bytes);
}

int MYRIOTA_ModbusWriteCoils(const MYRIOTA_ModbusHandle handle,
  const MYRIOTA_ModbusDeviceAddress slave, const MYRIOTA_ModbusDataAddress addr, const size_t count,
  const uint8_t *const bytes) {
  return modbus_write(handle, slave, MODBUS_FUNCTION_CODE_WRITE_MULTIPLE_COILS, addr, count, bytes);
}

int MYRIOTA_ModbusWriteHoldingRegisters(const MYRIOTA_ModbusHandle handle,
  const MYRIOTA_ModbusDeviceAddress slave, const MYRIOTA_ModbusDataAddress addr, const size_t count,
  const uint8_t *const bytes) {
  return modbus_write(handle, slave, MODBUS_FUNCTION_CODE_WRITE_MULTIPLE_REGISTERS, addr, count,
    bytes);
}

#ifdef MYRIOTA_MODBUS_UNIT_TESTS
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
/*
 * `cmocka.h` must be included after standard the above library headers.
 * NOTE: This comment has dual purpose:
 * 1. Document the ordering requirement.
 * 2. Prevent `clang-format` from reordering the headers.
 */
#include <cmocka.h>

static void test_x(void **state) {
  (void)state;
}

int main(void) {
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_x),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
#endif /** MYRIOTA_MODBUS_UNIT_TESTS */
