# ESP32-S3 Fast ADC — 83 kHz continuous DMA with calibration

Read your analog signal at **83,333 samples/second** on an ESP32-S3, stream calibrated millivolt values over serial at 1 Mbaud, and still have the CPU nearly idle. No RTOS tricks, no bitbanging — just the ESP-IDF continuous ADC driver doing DMA transfers in the background while your code sleeps.

## What you get

| Feature | Value |
|---|---|
| Sample rate | 83,333 Hz (hardware max on ESP32-S3) |
| Output rate | ~16,667 values/sec (5× oversampled) |
| Resolution | 12-bit ADC, calibrated to mV via eFuse curve fitting |
| Input range | 0–3.1 V (`ADC_ATTEN_DB_12`) |
| CPU load | Minimal — DMA fills a buffer, `app_main` blocks until data arrives |
| Serial speed | 1,000,000 baud |

## Why not `analogRead()`?

`analogRead()` reconfigures the ADC peripheral on **every single call**. At high rates it hits a wall fast, and it can't use DMA at all. The ESP-IDF continuous driver sets up the hardware once and then pumps samples into a ring buffer via DMA — the CPU only wakes up to consume a full frame of 256 samples at a time.

## Why ESP-IDF instead of Arduino?

The `esp_adc/adc_continuous.h` API — the one that gives you DMA — is not reachable from the Arduino framework include path. Switching `framework = espidf` in `platformio.ini` is the only change needed; you still build and flash with PlatformIO exactly as before. The entry point is `app_main()` instead of `loop()`, but the rest of the code is plain C++.

If you were already using Arduino just for convenience, this is a one-line framework switch with a massive throughput payoff.

## Hardware

- **Board:** ESP32-S3-DevKitC-1
- **ADC input:** GPIO 3 (ADC1 channel 2) — physically next to the 3.3 V and GND pins
- **Use ADC1 only** (GPIO 1–10). ADC2 is disabled when Wi-Fi is active.

## Oversampling

Every 5 raw samples are summed and divided before printing. This trades output rate for noise reduction — effective bandwidth is ~16.7 kHz with slightly better SNR than a single 12-bit read. Change the `count == 5` threshold to trade off between rate and noise.

## Getting started

```bash
# Install PlatformIO, then:
pio run --target upload && pio device monitor
```

Serial output is one millivolt value per line at 1,000,000 baud. Pipe it straight into Python, MATLAB, or a logic analyser terminal.

## Calibration

ESP32 ADCs are non-linear and vary per chip. Espressif burns per-chip correction data into eFuse at the factory. `adc_cali_curve_fitting` reads that data and turns raw 12-bit counts into accurate millivolt readings — much better than the naive `raw * 3300 / 4095` formula.
