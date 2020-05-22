/*!
 * @file AD9833.cpp
 *
 * This is a library for the AD9833 Programmable Waveform Generator
 *
 * Written by Matthew Michilot
 */

#include <SPI.h>
#include "AD9833.h"

/* Constants */
#define MAX_12BIT 4096
#define MAX_28BIT 268435456

/* Commonly used math calculations macros*/
#define FREQ_TO_REG(freq) ((freq/_mclk) * MAX_28BIT)
#define PHASE_TO_REG(rad) ((rad*MAX_12BIT) / (2*PI))

/* Bit masks */
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
  *        The device will use FREQ0 when it's initialized
  */
void AD9833::begin() {
  // set up fsync
  pinMode(_fsync, OUTPUT);
  digitalWrite(_fsync, HIGH);

  /* AD9833 initialization */
  SPI.beginTransaction(SPI_SETTINGS(_spiFreq));

  // enable reset
  write16(CTRL_ADDR, RESET);

  // clear FREQ0 and FREQ1
  write16(CTRL_ADDR, B28);
  write16(FREQ0_ADDR, 0x00);
  write16(FREQ0_ADDR, 0x00);

  write16(CTRL_ADDR, B28);
  write16(FREQ1_ADDR, 0x00);
  write16(FREQ1_ADDR, 0x00);

  // clear PHASE0 and PHASE1
  write16(PHASE0_ADDR, 0x00);
  write16(PHASE1_ADDR, 0x00);

  // disable reset
  write16(CTRL_ADDR, !RESET);

  SPI.endTransaction();
}

/*!
 * @brief Wrapper function that allows the frequency register
 *        in use to be modified.
 * @param freq
 *        Frequency to set in Hz
 */
void AD9833::setFrequency(float freq) {
   setFrequency(freq, (_registers[CTRL].data16 & FSELECT) ? FREQ1 : FREQ0);
}

/*!
 * @brief Sets the frequency for a specific register
 * @param freq
 *        Frequency to set in Hz
 * @param reg
 *        Frequency register to change
 */
void AD9833::setFrequency(float freq, enum RegisterName reg) {
  uint32_t oldFreq = _registers[reg].data32;
  uint32_t newFreq = FREQ_TO_REG(freq);
  uint16_t ctrlData = _registers[CTRL].data16;

  // Don't change frequency if freq > MCLK or freq < 0
  if (freq > _mclk || freq < 0){
    return;
  }

  // Don't change frequency if it's the same
  if (newFreq == _registers[reg].data32) {
    return;
  }

  // Get SPI ready
  SPI.beginTransaction(SPI_SETTINGS(_spiFreq));

  // Compare upper and lower 14 bits
  if (MSB_14(newFreq) == MSB_14(oldFreq)) {
    /* Write the 14 LSB */
    ctrlData &= ~HLB & ~B28;
    write16(CTRL_ADDR, ctrlData);
    write16(_registers[reg].addr, LSB_14(newFreq));
  }
  else if (LSB_14(newFreq) == LSB_14(oldFreq)) {
    /* Write the 14 MSB */
    ctrlData &= ~B28;
    ctrlData |= HLB;
    write16(CTRL_ADDR, ctrlData);
    write16(_registers[reg].addr, MSB_14(newFreq));
  }
  else {
    /* Write full 28-bits (requires two writes) */
    ctrlData |= B28;
    write16(CTRL_ADDR, ctrlData);
    write16(_registers[reg].addr, LSB_14(newFreq));
    write16(_registers[reg].addr, MSB_14(newFreq));
  }

  write16(CTRL_ADDR, _registers[CTRL].data16); // restore CTRL register

  SPI.endTransaction();

  // Store the new frequency
  _registers[reg].data32 = newFreq;

}

/*!
 * @brief Wrapper function that allows the phase register in use
 *        to be modified.
 * @param phase
 *        Phase to set in degrees
 */
void AD9833::setPhase(float phase) {
  setPhase(phase, (_registers[CTRL].data16 & PSELECT) ? PHASE1 : PHASE0);
}

/*!
 * @brief Sets the phase for a specific register
 * @param phase
 *        Phase to set in degrees
 * @param phaseReg
 *        Specift the phase register to modify
 */
void AD9833::setPhase(float phase, enum RegisterName reg) {
  float rad;
  uint16_t oldPhase = _registers[reg].data16;
  uint16_t newPhase;

  // Don't change if phase if phase > 360 or phase < 0
  if (phase > 360 || phase < 0)
    return;

  // Change degrees to radians
  rad = DEG_TO_RAD * phase;

  // Calculate register value
  newPhase = PHASE_TO_REG(rad);

  if (newPhase == oldPhase)
    return;

  SPI.beginTransaction(SPI_SETTINGS(_spiFreq));
  write16(_registers[reg].addr, (newPhase & BIT_MASK_12));
  SPI.endTransaction();

  _registers[reg].data16 = newPhase;
}

/*!
 * @brief Toggles between FREQ0 and FREQ1 for the currently
 *        selected frequency register
 */
void AD9833::toggleFreqReg() {
  // toggle FSELECT
  _registers[CTRL].data16 ^= FSELECT;

  SPI.beginTransaction(SPI_SETTINGS(_spiFreq));
  write16(CTRL_ADDR, _registers[CTRL].data16);
  SPI.endTransaction();
}

/*!
 * @brief Toogles between PHASE0 and PHASE1 for the
 *        currently selected phase register
 */
void AD9833::togglePhaseReg() {
  // toggle PSELECT
  _registers[CTRL].data16 ^= PSELECT;

  SPI.beginTransaction(SPI_SETTINGS(_spiFreq));
  write16(CTRL_ADDR, _registers[CTRL].data16);
  SPI.endTransaction();
}

/*!
 * @brief Set the waveform to output
 * @param state
 *        Waveform to set
 */
 void AD9833::setWaveform(Waveform state) {
  uint16_t data = _registers[CTRL].data16;
  if (state == _curWave) {
    return;
  }

  switch(state) {
    case SINE:
      data &= ~OPBITEN & ~MODE;
      break;
    case TRIANGLE:
      data &= ~OPBITEN;
      data |= MODE;
      break;
    case SQUARE_DIV2:
      data &= ~DIV2 & ~MODE;
      data |= OPBITEN;
      break;
    case SQUARE:
      data &= ~MODE;
      data |= OPBITEN | DIV2;
      break;
  }
  SPI.beginTransaction(SPI_SETTINGS(_spiFreq));
  write16(CTRL_ADDR, data);
  SPI.endTransaction();

  _registers[CTRL].data16 = data;
  _curWave = state;
 }

/*!
 * @brief Writes 16 bits via SPI
 * @param reg
 *        register to write to
 * @param data
 *        data to write to register
 */
void AD9833::write16(uint16_t addr, uint16_t data) {
  digitalWrite(_fsync, LOW);
  SPI.transfer16(addr | data);
  digitalWrite(_fsync, HIGH);
}
