
# no-OS

[![Build Status](https://travis-ci.org/analogdevicesinc/no-OS.svg?branch=master)](https://travis-ci.org/analogdevicesinc/no-OS) [![Codacy Badge](https://api.codacy.com/project/badge/Grade/9f57b0bbc3f6465ea211aa6c96afcb23)](https://www.codacy.com/gh/analogdevicesinc/no-OS?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=analogdevicesinc/no-OS&amp;utm_campaign=Badge_Grade)

[Analog Devices Inc.](http://www.analog.com/en/index.html) Software drivers for systems without OS.


## Software

The majority of ADI's products are peripherals to a non-ADI digital engine (FPGA, microprocessor, or microcontroller). While there is major work underway on FPGAs (Xilinx and Intel/Altera) and microprocessors (running an operating system like Linux), the efforts on microcontrollers are fragmented due to the diverse nature of the microcontroller market. The goal of these projects (microcontroller/no-OS) is to be able to provide reference projects for lower end processors, which can't run Linux or aren't running a specific operating system, to help customers using FPGA/microcontrollers with ADI parts.

## Documentation

For more information about no-OS drivers, please visit our [wiki](https://wiki.analog.com/resources/tools-software/uc-drivers-all) page.

Code documentation is automatically generated using Doxygen tool, available at [no-OS Github Pages](http://analogdevicesinc.github.io/no-OS/).

## Build

Clone this repository:
```
git clone https://github.com/analogdevicesinc/no-OS
```
This repo is using different submodules, so depending of your needs you should initialize some of them:

- **Tinyiiod**: To use projects which support iio
```
  git submodule update --init libraries/iio/libtinyiiod
```
- **STM32CubeF4**: To use projects for stm32 platforms
```
  git submodule update --init libraries/stm32/STM32CubeF4
```
- **Mqtt client**: To use projects that are using the mqtt client to connect to an mqtt borker
```
  git submodule update --init libraries/mqtt/paho.mqtt.embedded-c
```
- **MbedTLS**: To use the network layer with TLS integrated
```
  git submodule update --init libraries/mbedtls
```

Build guides for no-OS projects:
  * [Linux](https://github.com/analogdevicesinc/no-OS/wiki/Building-no-OS-on-Linux)
  * [Windows](https://github.com/analogdevicesinc/no-OS/wiki/Building-no-OS-on-Windows)

## Code Style
When writing code, please follow the [style guidelines](https://github.com/analogdevicesinc/no-OS/wiki/Code-Style-guidelines).

## Which branch should I use?

  * If you want to use the most stable code base, always use the [latest release branch]( https://github.com/analogdevicesinc/no-OS/branches/all?utf8=%E2%9C%93&query=201).

  * If you want to use the greatest and latest, check out the master branch.


## Support

Feel free to ask any question at [EngineerZone]( https://ez.analog.com/linux-device-drivers/microcontroller-no-os-drivers/f/q-a).
