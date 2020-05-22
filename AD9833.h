/*!
 * @file AD9833.h
 *
 * This is a library for the AD9833 Programmable Waveform Generator
 *
 * Written by Matthew Michilot
 */

#ifndef AD9833_h
#define AD9833_h

/* Default SPI frequency: 20 MHz */
#define SPI_FREQ 20000000
#define SPI_SETTINGS(rate) SPISettings(rate, MSBFIRST, SPI_MODE2)

/* MCLK frequency 25 MHz */
#define MCLK_FREQ 25000000

 /* Control bits */
#define MODE (1 << 1)
#define DIV2 (1 << 3)
#define OPBITEN (1 << 5)
#define RESET (1 << 8)
#define PSELECT (1 << 10)
#define FSELECT (1 << 11)
#define HLB (1 << 12)
#define B28 (1 << 13)

/* Registers
 * D15 | D14 | D13 | D12-D0|
 *  0     0     0      X     -> CTRL (0x0000)
 *  0     1     0      X     -> FREQ0 (0x4000)
 *  1     0     0      X     -> FREQ1 (0x8000)
 *  1     1     0      X     -> PHASE0 (0xC000)
 *  1     1     1      X     -> PHASE1 (0xE000)
 */
#define CTRL_ADDR 0x0000
#define FREQ0_ADDR 0x4000
#define FREQ1_ADDR 0x8000
#define PHASE0_ADDR 0xC000
#define PHASE1_ADDR 0xE000

struct Register {
  uint16_t addr;

  /* Mainly just to help specify the size of
   * the data being stored.
   *
   * CTRL and PHASE (12 bits) -> data16
   * FREQ (28 bits) -> data32
   */
  union {
    uint16_t data16;
    uint32_t data32;
  };
};

/* Register names to for indexing*/
enum RegisterName {
  CTRL = 0,
  FREQ0,
  FREQ1,
  PHASE0,
  PHASE1,
};

/* Waveform states */
enum Waveform {
  SINE=0,
  TRIANGLE,
  SQUARE_DIV2, // Divides the frequency of the square wave by 2
  SQUARE,
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

    void setFrequency(float freq);
    void setFrequency(float freq, enum RegisterName reg);

    void setPhase(float phase);
    void setPhase(float phase, enum RegisterName reg);

    void toggleFreqReg();
    void togglePhaseReg();

    void setWaveform(Waveform state);

  private:
    void write16(uint16_t addr, uint16_t data);

    uint32_t _mclk, _spiFreq;
    uint8_t _fsync;

    // AD9833 Registers
    struct Register _registers[5] = {{CTRL_ADDR, 0},
                                     {FREQ0_ADDR, 0},
                                     {FREQ1_ADDR, 0},
                                     {PHASE0_ADDR, 0},
                                     {PHASE1_ADDR, 0}};

    // Keep track of the current waveform
    enum Waveform _curWave = SINE;
 };

 #endif
