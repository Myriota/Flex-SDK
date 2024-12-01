# Myriota Modbus Library

A simple Modbus client library for Myriota edge devices. The library is
intended to support Myriota edge devices (Master) communicating to Modbus
senors devices (slaves) via a serial interface (RS484/RS232). The library
currently only supports RTU framing, however ASCII Framing maybe added in
future releases.

## Modbus Protocol Function Support

The library currently only supports a subset of the
[Modbus Protocol](https://modbus.org/docs/Modbus_Application_Protocol_V1_1b3.pdf)

| Function Code | Supported |
| ------------- | --------- |
|  Read coils | ✅ |
|  Read discrete inputs | ✅ |
|  Read holding registers | ✅ |
|  Read input registers | ✅ |
|  Write single coil | ✅ |
|  Write single register | ✅ |
|  Read exception status | ❌ |
|  Diagnostics | ❌ |
|  Comm event counter | ❌ |
|  Comm event log | ❌ |
|  Write multiple coils | ✅ |
|  Write multiple registers | ✅ |
|  Report slave id | ❌ |
|  Read file record | ❌ |
|  Write file record | ❌ |
|  Mask write register | ❌ |
|  Read write multiple registers | ❌ |
|  Read fifo queue | ❌ |
|  Encapsulated interface transport | ❌ |
