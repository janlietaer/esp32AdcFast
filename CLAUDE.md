# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build & Flash Commands

```bash
# Build
pio run

# Build + upload to connected ESP32-S3
pio run --target upload

# Open serial monitor (1,000,000 baud as configured)
pio device monitor

# Build + upload + monitor in one step
pio run --target upload && pio device monitor
```

There are no tests in this project.

## Architecture

Single-file Arduino/PlatformIO project targeting the **ESP32-S3-DevKitC-1**.

**`src/main.cpp`** is the entire application. It has two concerns:

1. **ADC readout** (active) — uses ESP-IDF's low-level `driver/adc.h` + `esp_adc_cal.h` instead of Arduino's `analogRead()` for speed. GPIO 3 = ADC1 channel 2. Calibrated millivolt output via eFuse curve fitting.

2. **ADXL355 accelerometer** (commented out) — SPI-connected on GPIO 10–13 (ESP32-S3 default FSPI). Library: `plasmapper/PL ADXL355`. When active, reads X/Y/Z accelerations at ±2g range.

## Key Configuration

- **Serial baud rate:** 1,000,000 — set in both `Serial.begin()` and `platformio.ini` `monitor_speed`. Both must match.
- **ADC attenuation:** `ADC_ATTEN_DB_12` covers 0–3.1 V input range.
- **ADC1 vs ADC2:** Always use ADC1 (GPIO 1–10). ADC2 is unavailable when Wi-Fi is active.
- **`esp_adc/adc_continuous.h`** is NOT available through the Arduino framework include path — use `driver/adc.h` instead for low-level access.

## Hardware Pin Map

| Signal      | GPIO |
|-------------|------|
| ADC input   | 3    |
| SPI CS      | 10   |
| SPI MOSI    | 11   |
| SPI SCLK    | 12   |
| SPI MISO    | 13   |
