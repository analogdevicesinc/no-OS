# AD7291 Voltage Monitor (CN0566)

No-OS Linux example for the AD7291 8-channel 12-bit ADC with integrated
temperature sensor, targeting the Raspberry Pi on the
[ADALM-PHASER (CN0566)](https://www.analog.com/en/resources/reference-designs/circuits-from-the-lab/cn0566.html)
phased-array development platform.

## Overview

The AD7291 on the CN0566 board monitors eight power-rail and signal voltages
through resistor dividers. This project reads all eight channels in a loop,
applies the CN0566 schematic scale factors, and prints the actual voltages.

| Channel | Label   | Scale Factor | Description                        |
|---------|---------|--------------|------------------------------------|
| CH0     | VDD1V8  | 2.0          | 1.8 V rail                        |
| CH1     | VDD3V0  | 2.0          | 3.0 V rail                        |
| CH2     | VDD3V3  | 2.0          | 3.3 V rail                        |
| CH3     | VDD4V5  | 4.01         | 4.5 V rail                        |
| CH4     | VDD_AMP | 7.98         | Amplifier supply                   |
| CH5     | VINPUT  | 4.01         | Input voltage                      |
| CH6     | IMON    | 1.0          | Current monitor (LTC4217, 1 V/A)   |
| CH7     | VTUNE   | 7.98         | VCO tuning voltage                 |

## Hardware

- Raspberry Pi (tested on RPi 4)
- ADALM-PHASER (CN0566) board
- AD7291 connected on I2C bus 1 at address `0x2A`

## Prerequisites

- CMake >= 3.10
- GCC toolchain
- I2C enabled on the Raspberry Pi (`sudo raspi-config` ? Interface Options ? I2C)

## Build

```bash
cd projects/ad7291
mkdir build && cd build
cmake ..
make
```

## Run

```bash
sudo ./ad7291
```

Root permissions are required to access `/dev/i2c-1`.

The program prints all channel voltages every second until stopped with
`Ctrl+C`.

### Example output

```
AD7291 initialized at 0x2A on /dev/i2c-1
CH0 VDD1V8 : 1.802 V
CH1 VDD3V0 : 3.012 V
CH2 VDD3V3 : 3.298 V
CH3 VDD4V5 : 4.510 V
CH4 VDD_AMP: 5.012 V
CH5 VINPUT : 4.998 V
CH6 IMON   : 0.350 V
CH7 VTUNE  : 3.200 V
---
```

## Project structure

```
projects/ad7291/
+-- CMakeLists.txt      # Standalone CMake build
+-- README.md
+-- src/
    +-- main.c          # Application entry point
```

## Driver

The AD7291 no-OS driver lives at `drivers/power/ad7291/` and provides:

- `ad7291_init` / `ad7291_remove` : Device lifecycle management
- `ad7291_reg_read` / `ad7291_reg_write`: Raw 16-bit I2C register access
- `ad7291_read_channel_voltage` : Single-channel ADC reading in millivolts
- `ad7291_read_temp` : Internal temperature sensor reading in millidegrees C
