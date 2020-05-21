/*!
 * @file AD9833.cpp
 *
 * This is a library for the AD9833 Programmable Waveform Generator
 *
 * Written by Matthew Michilot
 */

#include <SPI.h>
#include "AD9833.h"

#define MAX_12BIT 0xFFF
#define MAX_28BIT 0xFFFFFFF

/* Commonly used math calculations macros*/
#define FREQ_TO_REG(freq) ((freq/_mclk) * MAX_28BIT)
#define PHASE_TO_REG(rad) ((rad*MAX_12BIT) / (2*PI))

/* Bit masks */
#define BIT_MASK_1 0x1
#define BIT_MASK_12 0xFFF
#define BIT_MASK_14 0x3FFF

#define LSB_14(val) (val & BIT_MASK_14)
#define MSB_14(val) ((val >> 14) & BIT_MASK_14)

/*!
 * @brief Instantiates a new AD9833 class
 * @param fsync
 *        Pin number used for fsync
 * @param mclk
 *        Optional frequency of the AD9833's MCLK.
 *        Defaults to 25 MHz
 * @param spiFreq
 *        Optional frequency for SPI.
 *        Defaults to 20MHz
 */
AD9833::AD9833(uint8_t fsync, uint32_t spiFreq, uint32_t mclk) {
  _fsync = fsync;
  _spiFreq = spiFreq;
  _mclk = mclk;

 }

 /*!
  * @brief Setup fsync pin and initialize AD9833.
  *        The device will use FREQ0 when it's finish initializing.
  */
void AD9833::begin() {
  // set up fsync
  pinMode(_fsync, OUTPUT);
  digitalWrite(_fsync, HIGH);

  /* AD9833 initialization */
  // enable reset
  SPI.beginTransaction(SPI_SETTINGS(_spiFreq));
  write16(CTRL, RESET);

  // clear FREQ0 and FREQ1
  write16(CTRL, B28);
  write16(FREQ0, 0x00);
  write16(FREQ0, 0x00);

  write16(CTRL, B28);
  write16(FREQ1, 0x00);
  write16(FREQ1, 0x00);

  // clear PHASE0 and PHASE1
  write16(PHASE0, 0x00);
  write16(PHASE1, 0x00);

  // disable reset
  write16(CTRL, 0x00);
  SPI.endTransaction();
}

/*!
 * @brief Sets the frequency
 * @param freq
 *        Frequency to set in Hz
 * @param freqReg
 *        Optional frequency register to write to.
 *        Defaults to FREQ0
 */
void AD9833::frequency(float freq, uint8_t freqReg) {

  // Register values
  uint32_t newFreq = FREQ_TO_REG(freq);
  uint32_t oldFreq = _freq[_curFreqReg];

  // Don't change frequency if freq > MCLK or freq < 0
  if (freq > _mclk || freq < 0){
    return;
  }

  // Don't change frequency if it's the same
  if (newFreq == oldFreq) {
    return;
  }

  // Get SPI ready
  SPI.beginTransaction(SPI_SETTINGS(_spiFreq));

  // Compare upper and lower 14 bits
  if (MSB_14(newFreq) == MSB_14(oldFreq)) {
    write16(CTRL, 0x00); // Only write lower 14 bits
    write16(freqReg, LSB_14(newFreq));
  }
  else if (LSB_14(newFreq) == LSB_14(oldFreq)) {
    write16(CTRL, HLB); // Only write upper 14 bits
    write16(freqReg, MSB_14(newFreq));
  }
  else {
    write16(CTRL, B28); // Write full 28-bits
    write16(freqReg, MSB_14(newFreq));
    write16(freqReg, LSB_14(newFreq));
  }

  SPI.endTransaction();

  // Store the new frequency
  _freq[_curFreqReg] = newFreq;

}

/*!
 * @brief Sets the phase
 * @param freq
 *        Phase to set in degrees
 * @param freqReg
 *        Optional phase register to write to.
 *        Defaults to PHASE0
 */
void AD9833::phase(float phase, uint8_t phaseReg) {
  float rad;
  uint16_t newPhase;

  // Don't change if phase if phase > 360 or phase < 0
  if (phase > 360 || phase < 0) {
    return;
  }

  // Change degrees to radians
  rad = DEG_TO_RAD * phase;

  // Calculate register value
  newPhase = PHASE_TO_REG(rad);

  SPI.beginTransaction(SPI_SETTINGS(_spiFreq));
  write16(phaseReg, (newPhase & BIT_MASK_12));
  SPI.endTransaction();

  _phase[_curPhaseReg] = newPhase;
}

/*!
 * @brief Switches the frequency register
 */
void AD9833::switchFrequency() {
  // switch the current frequency register
  _curFreqReg ^= _curFreqReg;

  SPI.beginTransaction(SPI_SETTINGS(_spiFreq));
  write16(CTRL, (FSELECT(_curFreqReg)));
  SPI.endTransaction();
}

/*!
 * @brief Switches the phase register
 */
void AD9833::switchPhase() {
  // switch the current frequency register
  _curPhaseReg ^= _curPhaseReg;

  SPI.beginTransaction(SPI_SETTINGS(_spiFreq));
  write16(CTRL, (PSELECT(_curPhaseReg)));
  SPI.endTransaction();
}

/*!
 * @brief Set the waveform for VOUT
 * @param state
 *        Waveform to set
 */
 void AD9833::setWaveform(Waveform state) {
  if (state == _curWave) {
    return;
  }

  SPI.beginTransaction(SPI_SETTINGS(_spiFreq));
  
  switch(state) {
    case SINE:
      write16(CTRL, MODE(SINE));
      break;
    case TRIANGLE:
      write16(CTRL, MODE(TRIANGLE));
      break;
    case SQUARE_DIV2:
      write16(CTRL, OPBITEN);
      break;
    case SQUARE:
      write16(CTRL, OPBITEN | DIV2);
      break;
  }

  _curWave = state;

  SPI.endTransaction();
 }

/*!
 * @brief Writes 16 bits via SPI
 * @param reg
 *        register to write to
 * @param data
 *        data to write to register
 */
void AD9833::write16(uint8_t reg, uint16_t data) {
  digitalWrite(_fsync, LOW);
  SPI.transfer16((reg << 13) | data);
  digitalWrite(_fsync, HIGH);
}
