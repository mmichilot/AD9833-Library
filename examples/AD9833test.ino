#include <SPI.h>
#include <AD9833.h>

/* Pin Map
 * Arduino       AD9833
 * D9 ---------> FSYNC
 * D11 (MOSI) -> SDATA
 * D13 (SCK) --> SCLK
 */

#define FSYNC 9  // Change to pin connected to FYSNC
#define SS    10
#define MOSI  11
#define SCK   13

AD9833 chip(FSYNC);

void setup() {
  /* SPI setup */
  pinMode(SS, OUTPUT); // ensure that Arduino is master
  pinMode(MOSI, OUTPUT);
  pinMode(SCK, OUTPUT);

  Serial.begin(9600);

  /* Initialize AD9833
   * Default output is a 0 Hz sine wave.
   */
  chip.begin();
}

void loop() {
  /* Sweep from 0 Hz to 500 kHz in 0.1 Hz increments.
   * Assumes MCLK is 25 MHz. If its lower, the increments
   * can be a smaller.
   */
  for (float i = 0; i < 500000; i+= 0.1) {
    Serial.print("Frequency: ");
    Serial.print(i);
    Serial.println(" Hz");
    // Verify waveform frequency and shape
    // using an oscilloscope.
    chip.setFrequency(i);
    delay(500);
  }
}
