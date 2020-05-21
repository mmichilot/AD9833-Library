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

/* Since we are using 8-bit values to keep track of what
 *  two frequency and phase registers are in use, we only 
 *  care about the first bit.
 */
#define GET_FREQ_REG(freqReg) freqReg & BIT_MASK_1
#define GET_PHASE_REG(phaseReg) phaseReg & BIT_MASK_1

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
void AD9833::begin(float freq0, float phase0, 
                    float freq1, float phase1) {

  _curFreqReg = 0;
  _curPhaseReg = 0;
  
  // set up fsync
  pinMode(_fsync, OUTPUT);
  digitalWrite(_fsync, HIGH);

  /* AD9833 initialization */
  // enable reset
  SPI.beginTransaction(SPI_SETTINGS(_spiFreq));
  write16(CTRL, RESET); 
  SPI.endTransaction();

  // initialize FREQ0 and PHASE0 registers
  frequency(freq0, FREQ0);
  phase(phase0, PHASE0);

  // initialize FREQ1 and PHASE1 registers
  frequency(freq1, FREQ1);
  phase(phase1, PHASE1);

  // disable reset
  SPI.beginTransaction(SPI_SETTINGS(_spiFreq));
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
  uint32_t data;
  uint16_t lsb14;
  uint16_t msb14;
  
  // Don't change freq if freq > MCLK or freq < 0
  if (freq > _mclk || freq < 0){
    return;
  }

  // Calulate the value to store
  data = FREQ_TO_REG(freq);

  // Split into lower 14 and upper 14 bits
  lsb14 = data & MASK_14;
  msb14 = (data >> 14) & MASK_14;

  // Write to registers
  SPI.beginTransaction(SPI_SETTINGS(_spiFreq));
  write16(CTRL, B28);
  write16(freqReg, lsb14);
  write16(freqReg, msb14);
  SPI.endTransaction();
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
  uint16_t data;
  
  // Don't change if phase if phase > 360 or phase < 0
  if (phase > 360 || phase < 0) {
    return;
  }

  // Change degrees to radians
  rad = DEG_TO_RAD(phase);

  // Calculate register value
  data = PHASE_TO_REG(rad);
  
  SPI.beginTransaction(SPI_SETTINGS(_spiFreq));
  write16(phaseReg, (data & MASK_12));
  SPI.endTransaction();
}

void AD9833::write16(uint8_t reg, uint16_t data) {
  digitalWrite(_fsync, LOW);
  SPI.transfer16((reg << 13) | data);
  digitalWrite(_fsync, HIGH);
}
