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

/* Control bits */
#define PSELECT(reg) (reg << 10)
#define FSELECT(reg) (reg << 11)
#define RESET (1 << 9)
#define HLB (1 << 12)
#define B28 (1 << 13)

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
    void begin(float freq0 = 0, float phase0 = 0, 
                float freq1 = 0, float phase1 = 0);
    void frequency(float freq, uint8_t freqReg = FREQ0);
    void phase(float phase, uint8_t phaseReg = PHASE0);
    void switchFrequency();
    void switchPhase();
    
  private:
    void write16(uint8_t reg, uint16_t data);

    uint32_t _mclk, _spiFreq;
    uint8_t _fsync;

    // Keep track of which freq and phase register is in use
    uint8_t _curFreqReg, _curPhaseReg;

    // Keep track of freq and phase valuesas stored in the registers
    uint32_t _freq0, _freq1;
    uint16_t _phase0, _phase1;

 };

 #endif
