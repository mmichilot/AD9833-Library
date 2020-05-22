# AD9833 Library for Arduino

This library allows an Arduino board to control an AD9833 Programmable Waveform Generator.

Designed primarily to work with an AD9833 Breakout like the one below:

<img src="https://abra-electronics.com/images/detailed/143/MOD-AD9833-_3_.jpg" width="100" height="100">

These chips use SPI to communicate so 3 pins are required to interface.

| Arduino Pin | AD9833 Pin |
| ----------- | ---------- |
| Any         |   FSYNC    |
| MOSI (D11)  |   SDATA    |
| SCLK (D13)  |   SCLK     | 

This library will **NOT** setup and configure SPI, that is the user's responsibility. All this library needs to know is which pin is connected to FSYNC. Check out the example to get started.

For more information about this library please visit
https://mmichilot.github.io/AD9833-Library

Written by Matthew Michilot

# Installation

To download, click the **Clone or Download** button and select **Download ZIP**. 

Open the Arduino IDE and select *Sketch* > *Include Library* > *Add .ZIP Library...* and navigate to the downloaded zip file. Now the library should be added. 

To include it in your sketch, select *Sketch* and select *AD9833* under **Contributed libraries**.
 
This line is added to the sketch:
```c
#include <AD9833.h>
```
With that line, the library's functions are now available for use!
