# Changelog

## Flex SDK Release v2.3.0

* Adds support to transmit messages larger than 20 bytes.
* Adds `FLEX_MessageSlotsFree` API to return the number of messages that can be scheduled in the message queue
* Allow user applications to control the onboard blue LED using `FLEX_LEDBlueStateSet`.
* Adds Serial interface configuration options for message stopbits and databits.
* Fixed an issue with Serial interface parity settings.

## Flex SDK Release v2.2.4

* Fix issue where `FLEX_SerialDeinit(void)` was not correctly turning off RS232
  and RS485 power. This ensures that devices using RS232 and RS485 do not
  consume power in sleep mode.

## Flex SDK Release v2.2.3

* Fix a problem in the Modbus example where the sequence number would print
  4294967295 instead of 255.

* Fix a bug the Modbus library where `modbus_read` would cause a buffer
  overflow.

## Flex SDK Release v2.2.2

* Fix remove hard coded time stamp from `bulidkey.py`.

* Add missing Modbus library README.

## Flex SDK Release v2.2.1

* Fix a problem where the Modbus library would crashing when disconnected from
  sensor.

* Add retries attempts to `read_temperature_and_humidity` in the modbus
  example.

* Add README for the modbus library.

## Flex SDK Release v2.2.0

* Add Modbus library to simplify the development of firmware to interface
  Modbus sensors with the FlexSense device.

* Add the `message_store.py` script to Flex-SDK. This script provides the user
  with the ability to query device messages successfully received and stored in
  the "Myriota Message Store".

* Document `FLEX_AppVersionString` function in `flex.h`, clarifying that this
  function can be used to communicate the application version via BLE.

* Fix a problem where the pulse counter was configured by default to use the
  internal pull-down which was poorly interacting with an external pull-up. As
  a part of this change the `FLEX_PCNT_PULL_UP` flag has been deprecated. Users
  of the pulse counter should notice reduced power consumption from the
  device due to this change.

* Add support to configure the parity of the RS232/R485 serial interfaces using
  the new `FLEX_SerialInitEx` function.

* Updates to Flex SDK's documentation:

    - Fix the GCC toolchain path reference.

    - Add links to Flex SDK and Flex Reference Application source code on
      Github.

    - General improvements to development environment setup, build, and
      programming instructions.

## Release Flex SDK v2.1.0

* Change prefix for applications without network information merged in from
  "_build_key.bin" to ".app.bin"

* Add option to build application with cold_start network information via
  `-Dcold_start` configuration flag. NOTE: Builds default to using warm start
  network information.

* Unify `FLEX_***HandlerModifyAction` enums into a single enum
  `FLEX_HandlerModifyAction`.

* Meson now builds both RS-485 and RS-232 examples simultaneously

* Now supports basic diagnostic features when used in conjunction with
  FlexSense's BLE firmware version `ble_firmware-v0.7.5_385ebbd-FlexSense.bin`

* Update libflex dependency to `libflex-v1.3.1.a`

* Update system_image dependency to `system_image-2.0.2-FlexSense.bin`

* Fix "const" correctness for `FLEX_ModuleIDGet` and `FLEX_RegistrationCodeGet`

* Fix error is libflex meson.build  where it wasn't formatting an error message
  string correctly.

## Release Flex SDK v2.0.0

* Updates the ARM GNU toolchain to v13.2

* Flex SDK is now built on top of Myriota SDK 2.0

* Removes the need for network information programming

* User Application are now built as merged binary requiring the user to
  programming them using 'updater.py' with the '-m' flag.

* Add Downlink functionality with the `FLEX_MessageReceiveHandlerModify` API

* Improve the communication of error with the introduction of flex_error.h

* Builds both the rs232 and rs485 examples simultaneously

## Release Flex SDK v1.0.0

* Initial Release
