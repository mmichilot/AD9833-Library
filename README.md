# AD9833 Library
An AD9833 Library for the Arduino. There are pre-existing libraries out there, but I wanted to try my hand at making my own library. This library will eventually become a stepping stone to building some of my own lab equipment, such as a waveform generator and spectrum analyzer.

# Functions
AD9833(uint8_t fsync) - Specify which pin is connected to fsync. Also has the option to set your own SPI and MCLK frequency. Check out the code for defaults

begin() - Initializes the AD9833 by resetting it. By default it will initialize all registers to 0, however the frequency and phase registers can be initialized to a value other than 0. 

frequency(uint32_t freq) - Sets the frequency.  By default it will write to the FREQ0 register, but a second argument can be passed to specify which frequency register to write to.

phase(uint32_t phase) - Sets the phase.  By default it will write to the PHASE0 register, but a second argument can be passed to specify which phase register to write to. 

# In-progress
switchFreq() - switches the frequency register

switchPhase() - switches the phase register

The ability to change the type of waveform (square, triangle, etc...). 

I want to also make the frequency register writes more efficient. Right now two writes are always being used in order to write, but I want to make it so that only two writes would be needed if all 28 bits are changed, otherwise only write the appropriate lower or upper bits.
