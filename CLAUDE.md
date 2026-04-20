# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build & Flash Commands

```bash
# Build
pio run

# Build + upload to connected ESP32-S3 (COM3)
pio run --target upload

# Open serial monitor (1,000,000 baud as configured)
pio device monitor

# Build + upload + monitor in one step
pio run --target upload && pio device monitor
```

There are no tests in this project.

## Architecture

Single-file **ESP-IDF** PlatformIO project targeting the **ESP32-S3-DevKitC-1**.

**[src/main.cpp](src/main.cpp)** is the entire application. Entry point is `app_main()` (not Arduino `setup()`/`loop()`).

**ADC continuous DMA readout:**
- Uses `esp_adc/adc_continuous.h`, `esp_adc/adc_cali.h`, `esp_adc/adc_cali_scheme.h` (available because framework is `espidf`, not Arduino).
- GPIO 3 = ADC1 channel 2, 12-bit, `ADC_ATTEN_DB_12` (0–3.1 V range).
- Sample rate: 83,333 Hz (max for ESP32-S3), DMA frame size: 256 samples.
- Every 5 raw samples are averaged (oversampled) and one millivolt value is printed — effective output rate ≈ 16,667 values/sec.
- Calibration via eFuse curve fitting (`adc_cali_curve_fitting`).
- UART baud set directly with `uart_set_baudrate(UART_NUM_0, 1000000)`.

The ADXL355 accelerometer code has been removed entirely.

## Key Configuration

- **Serial baud rate:** 1,000,000 — set via `uart_set_baudrate()` in code and `monitor_speed` in `platformio.ini`. Both must match.
- **Framework:** `espidf` — `esp_adc/adc_continuous.h` and related headers are available (they are NOT reachable under the Arduino framework).
- **ADC1 vs ADC2:** Always use ADC1 (GPIO 1–10). ADC2 is unavailable when Wi-Fi is active.
- **Upload port:** COM3 (set in `platformio.ini`).

## Hardware Pin Map

| Signal    | GPIO |
|-----------|------|
| ADC input | 3    |
