# AD9833 Library for Arduino

This library allows an Arduino board to control an AD9833 Programmable Waveform Generator.

Designed specifically to work with an AD9833 Breakou like the one below:

<img src="https://abra-electronics.com/images/detailed/143/MOD-AD9833-_3_.jpg" width="100" height="100">

However, there is an option to set a custom MCLK frequency.

These chips use SPI to communicate so 3 pins are required to interface.

| Arduino Pin | AD9833 Pin |
| ----------- | ---------- |
| Any         |   FSYNC    |
| MOSI (D11)  |   SDATA    |
| SCLK (D13)  |   SCLK     | 

This library will **NOT** setup and configure SPI, that is the user's responsibility. All this library needs to know is which pin is connected to FSYNC.

For more information about this library please visit
https://mmichilot.github.io/AD9833-library

Written by Matthew Michilot
