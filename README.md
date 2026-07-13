
# no-OS
[![Build Status](https://dev.azure.com/AnalogDevices/no-OS/_apis/build/status%2Fanalogdevicesinc.no-OS?branchName=main)](https://dev.azure.com/AnalogDevices/no-OS/_build/latest?definitionId=48&branchName=main)

[Analog Devices Inc.](http://www.analog.com/en/index.html) bare-metal software framework for systems without an operating system.

## What is no-OS?

Most of ADI's products are peripherals to a non-ADI digital engine (FPGA, microprocessor, or microcontroller). While there is major work underway on FPGAs (Xilinx and Intel/Altera) and microprocessors (running an operating system like Linux), the efforts on microcontrollers are fragmented due to the diverse nature of the microcontroller market. no-OS provides reference projects for lower-end processors that can't run Linux or aren't running a specific operating system, helping customers use FPGAs/microcontrollers with ADI parts.

It provides:

* **Platform-agnostic device drivers** for ADI components that run without an operating system.
* **A hardware abstraction layer** so the same driver works across Xilinx, Intel, Maxim, STM32, ADuCM3029, Raspberry Pi Pico and more.
* **Ready-to-build project examples** you can flash to supported evaluation boards to get started quickly.

## Getting started

New to no-OS? Follow these steps:

1. **Clone the repository** with the `--recursive` flag to pull in all submodules:

   ```
   git clone --recursive https://github.com/analogdevicesinc/no-OS
   ```

2. **Set up your toolchain and build** by following the [Build Guide](https://analogdevicesinc.github.io/no-OS/build_guide.html) for your platform.

3. **Pick a project** from the [projects list](https://analogdevicesinc.github.io/no-OS/projects_doc.html) that matches your evaluation board, then build and flash it.

4. **Explore the drivers** you need in the [drivers list](https://analogdevicesinc.github.io/no-OS/drivers_doc.html).

## Documentation

Full documentation is hosted at [analogdevicesinc.github.io/no-OS](https://analogdevicesinc.github.io/no-OS/):

* [Build Guide](https://analogdevicesinc.github.io/no-OS/build_guide.html) — set up your toolchain and build no-OS for your platform.
* [Projects list](https://analogdevicesinc.github.io/no-OS/projects_doc.html) — ready-to-build example projects, grouped by application area.
* [Drivers list](https://analogdevicesinc.github.io/no-OS/drivers_doc.html) — reference documentation for every no-OS device driver.
* [Drivers guide](https://analogdevicesinc.github.io/no-OS/drivers_guide.html) — how no-OS drivers are structured and how to create or port one.
* [Contributing](https://analogdevicesinc.github.io/no-OS/contributing.html) — submission checklist and coding style for pull requests.

Code documentation is automatically generated using Doxygen and is available at [no-OS GitHub Pages](http://analogdevicesinc.github.io/no-OS/doxygen/).

> **Note:** We are migrating the no-OS documentation from [wiki.analog.com](https://wiki.analog.com/resources/no-os) to the [GitHub Pages site](https://analogdevicesinc.github.io/no-OS/). Some content may still only be available on the wiki during this transition.

## Contributing

Contributions are welcome! Please read the [Contributing guide](https://analogdevicesinc.github.io/no-OS/contributing.html) for the submission checklist and coding style before opening a pull request.

## Which branch should I use?

* If you want to use the most stable code base, always use the [latest release branch](https://github.com/analogdevicesinc/no-OS/branches/all?utf8=%E2%9C%93&query=201).

* If you want to use the greatest and latest, check out the main branch.

## Support

Feel free to ask any question at [EngineerZone](https://ez.analog.com/linux-device-drivers/microcontroller-no-os-drivers/f/q-a).
