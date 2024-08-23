/// \file flex_errors.h FlexSense Error code values
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

#ifndef FLEX_ERRORS_H
#define FLEX_ERRORS_H

enum FLEX_ERROR_VALUES {
  FLEX_SUCCESS = 0,

  // Standard error codes
  FLEX_ERROR_EPERM = 1,              // Operation not permitted
  FLEX_ERROR_EIO = 5,                // Input/output error
  FLEX_ERROR_ENXIO = 6,              // No such device or address
  FLEX_ERROR_EAGAIN = 11,            // Resource temporarily unavailable
  FLEX_ERROR_ENOMEM = 12,            // Cannot allocate memory
  FLEX_ERROR_EACCES = 13,            // Permission denied
  FLEX_ERROR_EFAULT = 14,            // Bad address
  FLEX_ERROR_EBUSY = 16,             // Device or resource busy
  FLEX_ERROR_ENODEV = 19,            // No such device
  FLEX_ERROR_EINVAL = 22,            // Invalid argument
  FLEX_ERROR_ENOSPC = 28,            // No space left on device
  FLEX_ERROR_EPIPE = 32,             // Broken pipe
  FLEX_ERROR_ERANGE = 34,            // Numerical result out of range
  FLEX_ERROR_ENOSYS = 38,            // Function not implemented
  FLEX_ERROR_EWOULDBLOCK = 11,       // Resource temporarily unavailable
  FLEX_ERROR_ENODATA = 61,           // No data available
  FLEX_ERROR_ETIME = 62,             // Timer expired
  FLEX_ERROR_ECOMM = 70,             // Communication error
  FLEX_ERROR_EPROTO = 71,            // Protocol error
  FLEX_ERROR_EBADMSG = 74,           // Bad message
  FLEX_ERROR_EOVERFLOW = 75,         // Value too large for defined data type
  FLEX_ERROR_EMSGSIZE = 90,          // Message too long
  FLEX_ERROR_EOPNOTSUPP = 95,        // Operation not supported
  FLEX_ERROR_ENOBUFS = 105,          // No buffer space available
  FLEX_ERROR_ETIMEDOUT = 110,        // Timed out
  FLEX_ERROR_EALREADY = 114,         // Operation already in progress, device already initialised
  FLEX_ERROR_ECANCELED = 125,        // Operation cancelled
  FLEX_ERROR_ENOTRECOVERABLE = 131,  // State not recoverable

  // Flex device errors
  FLEX_ERROR_SUB_SYSTEM = 160,   // Unspecified error in internal system
  FLEX_ERROR_I2C = 161,          // I2C bus interface error
  FLEX_ERROR_IO_EXPANDER = 162,  // GPIO expander device error
  FLEX_ERROR_POWER_OUT = 163,    // Enable/Disable power to a device or an interface error
  FLEX_ERROR_PWM = 164,          // PWM device error
  FLEX_ERROR_GPIO = 165,         // GPIO device error
  FLEX_ERROR_SERIAL = 166,       // UART/Serial device error
  FLEX_ERROR_GNSS = 167,         // GNSS device error
  FLEX_ERROR_BUTTON = 168,       // Push button device error
  FLEX_ERROR_LED = 169,          // LED device error
  FLEX_ERROR_TEMP = 169,         // Temperature device error

  // Flex specific error cases
  FLEX_ERROR_READ_FAIL = 192,  // Failed to read data from device
  FLEX_ERROR_NOT_INIT = 193,   // Device not initialised

  // Error codes equal to or above this value can be found in a specific
  // set given below, these are context dependent and the numbers are
  // overloaded, so ensure to check them based on the context/functionality
  FLEX_EXTENDED_ERROR = 224,
};

enum FLEX_ANALOG_ERROR {
  FLEX_ERROR_ANALOG_INPUT_MODE = FLEX_EXTENDED_ERROR,  // Device in wrong input mode
};

#endif
