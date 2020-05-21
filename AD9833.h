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
// Selects the proper freq register address
#define FREQ_ADDR(sel) (sel ? FREQ1 : FREQ0)

#define PHASE0 0x6
#define PHASE1 0x7
// Selects the proper phase register address
#define PHASE_ADDR(sel) (sel ? PHASE1 : PHASE0)

/* Control bits */
#define MODE(mode) (mode << 1)
#define DIV2 (1 << 3)
#define OPBITEN (1 << 5)
#define RESET (1 << 8)
#define PSELECT(reg) (reg << 10)
#define FSELECT(reg) (reg << 11)
#define HLB (1 << 12)
#define B28 (1 << 13)

/* Default SPI frequency: 20 MHz*/
#define SPI_FREQ 20000000
#define SPI_SETTINGS(rate) SPISettings(rate, MSBFIRST, SPI_MODE2)

#define MCLK_FREQ 25000000

/* Waveform states */
enum Waveform {
  SINE=0,
  TRIANGLE,
  SQUARE_DIV2, // Divides the frequency of the square wave by 2
  SQUARE
};

/*!
 * @brief Class that stores state and functions for AD9833
 */
 class AD9833
 {
  public:
    AD9833(uint8_t fsync, uint32_t spiFreq = SPI_FREQ,
            uint32_t mclk = MCLK_FREQ);
    void begin();
    void frequency(float f);
    void frequency(float freq, uint8_t freqReg);
    void phase(float p);
    void phase(float phase, uint8_t phaseReg);
    void switchFrequency();
    void switchPhase();
    void setWaveform(Waveform state);

  private:
    void write16(uint8_t reg, uint16_t data);

    uint32_t _mclk, _spiFreq;
    uint8_t _fsync;

    // Keep track of the current waveform
    uint8_t _curWave;

    // Keep track of which freq and phase register is in use
    uint8_t _curFreqReg = 0, _curPhaseReg = 0;

    // Keep track of freq and phase values as stored in the registers
    uint32_t _freq[2] = {0};
    uint16_t _phase[2] = {0};

 };

 #endif
