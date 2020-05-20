/*!
 * @file AD9833.cpp
 * 
 * This is a library for the AD9833 Programmable Waveform Generator
 * 
 * Written by Matthew Michilot
 */
 
#include <SPI.h>
#include "AD9833.h"

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
  * @param freq0
  *        Optionally initialize FREQ0. 
  *        Defaults to 0
  * @param phase0
  *        Optionally initialize PHASE0. 
  *        Defaults to 0
  * @param freq1
  *        Optionally initialize FREQ1. 
  *        Defaults to 0
  * @param phase1
  *        Optionally initialize PHASE1. 
  *        Defaults to 0
  */
void AD9833::begin(uint32_t freq0, uint32_t phase0, 
                    uint32_t freq1, uint32_t phase1) {
  _freq0 = freq0;
  _freq1 = freq1;
  _phase0 = phase0;
  _phase1 - phase1;

  _curFreqReg = FREQ0;
  _curPhaseReg = PHASE0;
  
  // set up fsync
  pinMode(_fsync, OUTPUT);
  digitalWrite(_fsync, HIGH);

  // AD9833 initialization
  
  // reset
  SPI.beginTransaction(SPI_SETTINGS(_spiFreq));
  write16(CTRL, (1 << 9)); 
  SPI.endTransaction();

  frequency(_freq0, _curFreqReg);
  phase(_phase0, _curPhaseReg);

  // remove reset
  SPI.beginTransaction(SPI_SETTINGS(_spiFreq));
  write16(CTRL, 0x00);
  SPI.endTransaction();
}

/*!
 * @brief Sets the frequency
 * @param freq
 *        Frequency to set
 * @param freqReg
 *        Optional frequency register to write to. 
 *        Defaults to FREQ0
 */
void AD9833::frequency(uint32_t freq, uint8_t freqReg) {
  uint32_t data;
  uint16_t lsb14;
  uint16_t msb14;
  
  // Don't change freq if its greater than the MCLK freq
  if (freq > _mclk){
    return;
  }

  // Calulate the value to store
  data = calcFreqReg(freq);

  // Split into lower 14 and upper 14 bits
  lsb14 = data & 0x03FFF;
  msb14 = (data >> 14) & 0x3FFF;

  // Write to registers
  SPI.beginTransaction(SPI_SETTINGS(_spiFreq));
  write16(CTRL, (1 << 13));
  write16(freqReg, lsb14);
  write16(freqReg, msb14);
  SPI.endTransaction();
}

/*!
 * @brief Sets the phase
 * @param freq
 *        Frequency to set
 * @param freqReg
 *        Optional phase register to write to. 
 *        Defaults to PHASE0
 */
void AD9833::phase(uint32_t phase, uint8_t phaseReg) {
  SPI.beginTransaction(SPI_SETTINGS(_spiFreq));
  write16(phaseReg, (phase & 0x0FFF));
  SPI.endTransaction();
}

/*!
 * @brief Calculates the value to store into a frequency register
 * @param freq
 *        Frequency value to be converted
 */
uint32_t AD9833::calcFreqReg(uint32_t freq) {
  return ((float)freq / (float)_mclk) * 268435456;
}

void AD9833::write16(uint8_t reg, uint16_t data) {
  digitalWrite(_fsync, LOW);
  SPI.transfer16((reg << 13) | data);
  digitalWrite(_fsync, HIGH);
}
