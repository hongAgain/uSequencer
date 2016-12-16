# uSequencer

Small, simple sequence control code for automation tasks


This program is a simple way to implement PLC-like control of small systems.

The program can be adjusted to work within available memory on devices like the Arduino Uno, or expanded for more capable systems to do things such as control ethernet field I/O devices. The default configuration is written to use 64 digital inputs and 64 digital outputs, with 128 bits for internal state control and a 100ms tick rate.
