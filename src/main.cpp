#include <PL_ADXL355.h>
#include <Arduino.h>

//==============================================================================

// Using default ESP32-S3 SPI2 (FSPI) pins
// ADXL355   GPIO
// CS          10
// MOSI        11
// SCLK        12
// MISO        13

#define SPI_CS_PIN  10
#define ADC_PIN      3  // ADC1 channel 2, free from SPI

PL::ADXL355 adxl355;

//==============================================================================

void setup() {
  Serial.begin(1000000);
  while (!Serial);

  adxl355.beginSPI(SPI_CS_PIN);
  adxl355.reset();
  delay(1000);

  // Most sensitive scale: ±2g
  adxl355.setRange(PL::ADXL355_Range::range2g);
  adxl355.enableMeasurement();

  analogReadResolution(12);  // 0–4095
  Serial.println("X (g), Y (g), Z (g), ADC (raw), ADC (V)");
}

//==============================================================================

void loop() {
 
 // int adcRaw = analogRead(ADC_PIN);
  // float adcVolts = analogReadMilliVolts(ADC_PIN) / 1000.0f;
  int adcMv = analogReadMilliVolts(ADC_PIN);
 /**auto a = adxl355.getAccelerations();
  Serial.print(a.x, 6);
  Serial.print(", ");
  Serial.print(a.y, 6);
  Serial.print(", ");
  Serial.print(a.z, 6);
  Serial.print(", ");
  **/
 // Serial.print(adcRaw);
 // Serial.print(", ");
  Serial.println(adcMv);
}
