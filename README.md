So as a quick intro:

This is the firmware for a telemetry transmitter on 434MHz. It parses GPS using UART interrupts. Every so often the board sends a telemetry message via CW (because that's what I have written right now. It will change in the future).

The problem right now:

When the microcontroller wants to send a telemetry message it:
* Disables the GPS parsing (which waits for the end of a GPGGA packet if its in one).
* Copies the GPS variables over to ones it uses (the ones in the GPS library update with every packet "in the background" (by interrupts)).
* Writes that to uart (the serial port) for debugging.

The function to copy the variables over is "gps_read". It takes the addresses of the variables to write to, which it should do. Its not doing that though. I get >00000001,00000000,0000,00 written over uart.

Any suggestions?
