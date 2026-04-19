/*
 * ESP32-S3 ADC Example using ESP-IDF low-level driver with calibration
 * Platform: ESP32-S3-DevKitC-1 | Framework: Arduino (PlatformIO)
 * Serial output: 1,000,000 baud
 *
 * WHY NOT analogRead()?
 * Arduino's analogRead() is convenient but adds significant overhead: it
 * reconfigures the ADC on every call and uses a generic wrapper that is
 * noticeably slower than direct register access. For any application that
 * needs high sample rates, bypassing the Arduino layer is worthwhile.
 *
 * THE LOW-LEVEL DRIVER: driver/adc.h
 * ESP-IDF exposes adc1_get_raw() which reads ADC1 directly without
 * reinitialising the peripheral each time. Setup is done once in setup():
 *   - adc1_config_width()         sets the bit resolution (12-bit = 0..4095)
 *   - adc1_config_channel_atten() sets the input attenuation
 * After that, adc1_get_raw() in the loop is a bare hardware read.
 *
 * PIN CHOICE: GPIO 3 = ADC1 channel 2
 * The ESP32-S3 has two ADC units. ADC1 (GPIO 1-10) is preferred because
 * ADC2 cannot be used while Wi-Fi is active. GPIO 3 / ADC1_CH2 was chosen
 * here because it is physically close to the 3.3 V and GND pins on the
 * DevKitC-1 header, making wiring convenient.
 *
 * ATTENUATION: ADC_ATTEN_DB_12
 * The ESP32-S3 ADC input range depends on the attenuation setting:
 *   0 dB  -> ~0..950 mV
 *   2.5 dB-> ~0..1250 mV
 *   6 dB  -> ~0..1750 mV
 *   12 dB -> ~0..3100 mV  (covers the full 3.3 V supply)
 * DB_12 is used here to cover the full 0-3.3 V range.
 *
 * CALIBRATION: esp_adc_cal
 * The ESP32 ADC is non-linear and varies between chips. Espressif stores
 * per-chip calibration data in eFuse during factory programming.
 * esp_adc_cal_characterize() reads that data and builds a correction curve.
 * esp_adc_cal_raw_to_voltage() then converts a raw 12-bit value to a
 * calibrated millivolt reading, which is significantly more accurate than
 * the naive linear formula (raw * 3300 / 4095).
 *
 * SERIAL SPEED: 1,000,000 baud
 * At default 115200 baud, Serial.println() becomes the bottleneck at high
 * sample rates (the TX buffer fills faster than bytes are sent). Running at
 * 1 Mbaud allows ~100,000 bytes/sec, enough to stream ~20,000 text-format
 * samples per second before serial becomes the limiting factor again.
 */

// #include <PL_ADXL355.h>
#include <Arduino.h>
#include "driver/adc.h"
#include "esp_adc_cal.h"

//==============================================================================

// Using default ESP32-S3 SPI2 (FSPI) pins (reserved for ADXL355, inactive)
// ADXL355   GPIO
// CS          10
// MOSI        11
// SCLK        12
// MISO        13

// #define SPI_CS_PIN    10
#define ADC_PIN        3   // ADC1 channel 2, free from SPI
#define ADC_CHANNEL    ADC1_CHANNEL_2
#define ADC_ATTEN_CFG  ADC_ATTEN_DB_12
#define ADC_WIDTH_CFG  ADC_WIDTH_BIT_12

// PL::ADXL355 adxl355;

static esp_adc_cal_characteristics_t adcChars;

//==============================================================================

void setup() {
  Serial.begin(1000000);
  while (!Serial);

  // adxl355.beginSPI(SPI_CS_PIN);
  // adxl355.reset();
  // delay(1000);
  // adxl355.setRange(PL::ADXL355_Range::range2g);
  // adxl355.enableMeasurement();

  adc1_config_width(ADC_WIDTH_CFG);
  adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTEN_CFG);
  esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_CFG, ADC_WIDTH_CFG, 1100, &adcChars);

  Serial.println("ADC (mV)");
}

//==============================================================================

void loop() {
  // auto a = adxl355.getAccelerations();
  uint32_t raw = adc1_get_raw(ADC_CHANNEL);
  uint32_t mv  = esp_adc_cal_raw_to_voltage(raw, &adcChars);

  // Serial.print(a.x, 6); Serial.print(", ");
  // Serial.print(a.y, 6); Serial.print(", ");
  // Serial.print(a.z, 6); Serial.print(", ");
  Serial.println(mv);
}
