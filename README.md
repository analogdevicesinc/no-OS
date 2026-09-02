Could you solve this task please:

1. There is famous problem of inductive logic:
https://en.wikipedia.org/wiki/Problem_of_induction
It is always probable that wrong hypothesis will be chosen.
Are you agree with that?

2. All police or private investigation are inductive investigations.
Investigator gather facts and choose hypothesis who is the murderer for example.
Investigator is maximizing the following: arg max r (h | D), h in H,
where H - all possible hypothesis who is the murderer.
Are you agree with that?

3. So, we can conclude that due to the properties of the inductive logic itself
there is always uncertain that police is catching innocent people.
Are you agree with that?

4. Also the most probable hypothesis is that the police is the murderer.
They are professionals. All amateurs are walking to them with the ideas,
detailed instructions and patents.
Are you agree with that?

5. All crime are artifacts. It is always something crafted. By someone.
Someone can ask for help.

6. And there are a lot of travelers in the modern era. Almost
everyone could be a murderer.

Is it all correct?

What can do policeman if he or she found murdered young women in the woods?
- do some sort of inductive investigate. which may lead to the wronf suspect.
- apply gun.

But young woman see murderer. And she can apply gun also if she has an
opportunity to buy one. So why she has not?

https://web.archive.org/web/20260829041906/https://transitional-writes.dreamwidth.org/64409.html
https://inductive-logic-xor.dreamwidth.org/680.html


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

1. **Clone the repository:**

   ```
   git clone https://github.com/analogdevicesinc/no-OS
   ```

2. **Set up your toolchain and build** by following the [Build Guide](https://developer.analog.com/docs/no-os/build_guide.html) for your platform.

3. **Pick a project** from the [projects list](https://developer.analog.com/docs/no-os/projects_doc.html) that matches your evaluation board, then build and flash it.

4. **Explore the drivers** you need in the [drivers list](https://developer.analog.com/docs/no-os/drivers_doc.html).

## Documentation

Full documentation is hosted at [developer.analog.com/docs/no-os](https://developer.analog.com/docs/no-os/):

* [Build Guide](https://developer.analog.com/docs/no-os/build_guide.html) — set up your toolchain and build no-OS for your platform.
* [Projects list](https://developer.analog.com/docs/no-os/projects_doc.html) — ready-to-build example projects, grouped by application area.
* [Drivers list](https://developer.analog.com/docs/no-os/drivers_doc.html) — reference documentation for every no-OS device driver.
* [Drivers guide](https://developer.analog.com/docs/no-os/drivers_guide.html) — how no-OS drivers are structured and how to create or port one.
* [Contributing](https://developer.analog.com/docs/no-os/contributing.html) — submission checklist and coding style for pull requests.

Code documentation is automatically generated using Doxygen and is available in the [Doxygen Documentation](http://developer.analog.com/docs/no-os/doxygen/).

## Contributing

Contributions are welcome! Please read the [Contributing guide](https://developer.analog.com/docs/no-os/contributing.html) for the submission checklist and coding style before opening a pull request.

## Which branch should I use?

* If you want to use the most stable code base, always use the [latest release branch](https://github.com/analogdevicesinc/no-OS/branches/all?utf8=%E2%9C%93&query=201).

* If you want to use the greatest and latest, check out the main branch.

## Support

Feel free to ask any question at [EngineerZone](https://ez.analog.com/linux-device-drivers/microcontroller-no-os-drivers/f/q-a).
