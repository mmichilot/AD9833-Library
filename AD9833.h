/*!
 * @file AD9833.h
 * 
 * This is a library for the AD9833 Programmable Waveform Generator
 * 
 * Written by Matthew Michilot
 */

 #ifndef AD9833_h
 #define AD9833_h

/* Registers */
#define CTRL 0x0
#define FREQ0 0x2
#define FREQ1 0x4
#define PHASE0 0x6
#define PHASE1 0x7

/* Default SPI frequency: 20 MHz*/
#define SPI_FREQ 20000000
#define SPI_SETTINGS(rate) SPISettings(rate, MSBFIRST, SPI_MODE2)

#define MCLK_FREQ 25000000


/*!
 * @brief Class that stores state and functions for AD9833
 */
 class AD9833
 {
  public:
    AD9833(uint8_t fsync, uint32_t spiFreq = SPI_FREQ,
            uint32_t mclk = MCLK_FREQ);
    void begin(uint32_t freq0 = 0, uint32_t phase0 = 0, 
                uint32_t freq1 = 0, uint32_t phase1 = 0);
    void frequency(uint32_t freq, uint8_t freqReg = FREQ0);
    void phase(uint32_t phase, uint8_t phaseReg = PHASE0);
    
  private:
    void write16(uint8_t reg, uint16_t data);
    uint32_t calcFreqReg(uint32_t freq);

    uint32_t _mclk, _spiFreq;
    uint8_t _fsync;

    // Keep track of which freq and phase register is in use
    uint8_t _curFreqReg, _curPhaseReg;

    // Keep track of freq and phase values
    uint32_t _freq0, _freq1, _phase0, _phase1;

 };

 #endif
