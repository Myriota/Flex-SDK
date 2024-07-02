# RS485 & RS232 Example

This example demonstrates RS-485/RS-232 interface communication between
an external device and the FlexSense Device. This also demonstrates a
basic modem application. Operates as follows:

1. The external device wakes up the module by setting the selected WakeupPin Low.
2. Waits for the "READY\n" string from the FlexSense Device.
3. Sends message string terminated by "\n".
4. Sets the selected WakeupPin High again.
5. The FlexSense Device acknowledges reception with string "\nOK\n".
