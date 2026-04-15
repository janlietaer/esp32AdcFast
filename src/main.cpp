#include <PL_ADXL355.h>

//==============================================================================

// Using default ESP32-S3 SPI2 (FSPI) pins
// ADXL355   GPIO
// CS          10
// MOSI        11
// SCLK        12
// MISO        13

#define SPI_CS_PIN 10

PL::ADXL355 adxl355;

//==============================================================================

void setup() {
  Serial.begin(115200);
  while (!Serial);

  adxl355.beginSPI(SPI_CS_PIN);
  adxl355.reset();
  delay(1000);

  // Most sensitive scale: ±2g
  adxl355.setRange(PL::ADXL355_Range::range2g);
  adxl355.enableMeasurement();

  Serial.println("X (g), Y (g), Z (g)");
}

//==============================================================================

void loop() {
  auto a = adxl355.getAccelerations();
  Serial.print(a.x, 6);
  Serial.print(", ");
  Serial.print(a.y, 6);
  Serial.print(", ");
  Serial.println(a.z, 6);
}
