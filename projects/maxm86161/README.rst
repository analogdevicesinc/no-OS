MAXM86161 no-OS Example Project
===============================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
---------------------------

* `MAXM86161EVSYS <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/maxm86161evsys.html>`_

Overview
--------

This project demonstrates the MAXM86161 integrated optical sensor module and its
integration with the no-OS framework. The MAXM86161 is a complete,
ultra-low-power, integrated optical data acquisition system, ideal for optical
pulse-oximetry (SpO2) and heart-rate (HR) monitoring in wearable applications.

The module combines three programmable LEDs (green, red, and IR), a low-noise
photodiode signal path, an ambient-light-cancellation (ALC) 19-bit ADC, and a
128-word FIFO into a single package. Key characteristics:

* Three on-chip LED drivers (green, red, IR), programmable pulse amplitude and
  current range
* 19-bit optical ADC with programmable integration time and ADC range
* Programmable sample rate and sample averaging
* 128-word (32-bit) FIFO with configurable watermark, roll-over, and interrupt
* Programmable LED exposure sequence (up to six slots)
* On-chip die temperature sensor
* Low-power (shutdown) mode for battery-powered wearables
* I2C interface (7-bit address 0x62) with an INTB interrupt output

Applications
------------

* Optical heart-rate monitoring
* Pulse oximetry (SpO2)
* Wearable and fitness health devices
* Proximity detection

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~~~

The MAXM86161 operates from a 1.8 V analog/digital supply and a separate LED
supply. On the evaluation system these rails are provided by the EV board; the
I2C and INTB logic levels are 1.8 V. When wiring to a carrier, make sure the
carrier's I2C bus and interrupt pin operate at the same logic level (the Maxim
platform configures its I/O for the ``VDDIO`` rail).

I2C Interface
~~~~~~~~~~~~~

* 7-bit device address: 0x62
* Supported speeds: Standard (100 kHz), Fast (400 kHz); the examples use 400 kHz
* INTB: active-low interrupt output (FIFO watermark / data-ready)

No-OS Supported Examples
------------------------

This project is organized around the no-OS variant based build flow. Selecting a
variant at build time (``--variant <name>``) chooses which application is
compiled. The platform ``main()`` is a thin dispatcher that calls
``example_main()``, provided by the selected example. Shared initialization data
is defined in
`src/common <https://github.com/analogdevicesinc/no-OS/tree/main/projects/maxm86161/src/common>`__,
and platform-specific macros and extra init parameters are in
`src/platform <https://github.com/analogdevicesinc/no-OS/tree/main/projects/maxm86161/src/platform>`__.

Basic Example
~~~~~~~~~~~~~

The basic example (``--variant basic``) initializes the MAXM86161 and
demonstrates fundamental functionality without any host tooling:

- Device initialization and part/revision ID read-back
- PPG configuration: integration time, ADC range, sample averaging, sample rate
- LED configuration: settling time, PD bias, per-LED current range and pulse
  amplitude, and the six-slot LED exposure sequence (green / red / IR / ambient)
- FIFO configuration: watermark and roll-over
- Low-power mode enable and exit-shutdown to start sampling
- Continuous polling loop that reads the die temperature and drains the FIFO,
  decoding each sample and printing it grouped by tag over the UART console

The tunable acquisition parameters live at the top of
`src/examples/basic/basic_example.c <https://github.com/analogdevicesinc/no-OS/tree/main/projects/maxm86161/src/examples/basic/basic_example.c>`__.

IIO Example
~~~~~~~~~~~

The IIO example (``--variant iio``) exposes the MAXM86161 as an IIO device over
the UART transport, so the FIFO data stream and device attributes can be read by
a host IIO client (libiio / IIO Oscilloscope). It configures the INTB GPIO
interrupt, sets a FIFO watermark, and streams decoded samples through a circular
buffer into the IIO buffer. A small in-code drain self-test
(``MAXM86161_SELFTEST``) can be toggled at the top of
`src/examples/iio_example/iio_example.c <https://github.com/analogdevicesinc/no-OS/tree/main/projects/maxm86161/src/examples/iio_example/iio_example.c>`__
to exercise the ISR → circular buffer → decode path over the console when no
libiio host is available.

If you are not familiar with ADI IIO Application, please take a look at:
:dokuwiki:`IIO No-OS </resources/tools-software/no-os-software/iio>`

If you are not familiar with ADI IIO-Oscilloscope Client, please take a look at:
:dokuwiki:`IIO Oscilloscope </resources/tools-software/linux-software/iio_oscilloscope>`

Attribute Test Example
~~~~~~~~~~~~~~~~~~~~~~~

The attribute test example (``--variant attribute_test``) is a
hardware-in-the-loop (HIL) integration test that runs against a real device over
the live I2C bus. It mirrors the offline unit-test suite: every setter is
followed by its matching getter to confirm the value reached the silicon and
reads back unchanged, and it re-checks parameter validation and FIFO-decode
logic plus a short live-acquisition sanity check. A compact Unity-style
assertion layer prints a PASS/FAIL line per check and a summary over the UART
console. The run is non-destructive but does not restore the original device
state, so re-flash or power-cycle before switching to a measurement example.

No-OS Supported Platforms
-------------------------

Maxim Platform
~~~~~~~~~~~~~~

Used Hardware
^^^^^^^^^^^^^

* `MAXM86161EVSYS <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/maxm86161evsys.html>`_
* `MAX32655FTHR <https://www.analog.com/MAX32655FTHR>`_

Connections
^^^^^^^^^^^

The MAXM86161 communicates over I2C. The examples use the MAX32655's ``I2C2``
peripheral and take the INTB interrupt on ``P2_7``. The UART console appears on
the MAX32655FTHR USB serial port at **230400 baud** (8N1).

+-------------+------------------+--------------------------------------------------+
| MAXM86161   | MAX32655FTHR     | Function                                         |
+=============+==================+==================================================+
| VDD / VLED  | 1V8              | Power supply (see EV board)                      |
+-------------+------------------+--------------------------------------------------+
| GND         | GND              | Ground                                           |
+-------------+------------------+--------------------------------------------------+
| SCL         | P0_30            | I2C clock (I2C2)                                 |
+-------------+------------------+--------------------------------------------------+
| SDA         | P0_31            | I2C data (I2C2)                                  |
+-------------+------------------+--------------------------------------------------+
| INTB        | P2_7             | Interrupt (FIFO watermark / data ready)          |
+-------------+------------------+--------------------------------------------------+

The I2C pins, INTB pin, UART device, and baud rate are defined in
`src/platform/maxim/parameters.h <https://github.com/analogdevicesinc/no-OS/tree/main/projects/maxm86161/src/platform/maxim/parameters.h>`__.

Build Command
^^^^^^^^^^^^^

The Maxim platform uses the CMake/Ninja build system via the ``no_os_build.py``
helper script. Available variants: ``basic``, ``iio``, ``attribute_test``.
Available boards: ``max32655fthr``.

For toolchain setup and prerequisites, see the
:doc:`Maxim CMake build guide </build_guides/build_maxim_cmake>`.

.. code-block:: bash

   # point at your MaximSDK Libraries directory
   export MAXIM_LIBRARIES=/path/to/MaximSDK/Libraries
   # PowerShell (Windows) equivalent:
   #   $env:MAXIM_LIBRARIES = "C:\MaximSDK\Libraries"

   cd no-OS

   # build a variant on the max32655fthr board (basic | iio | attribute_test)
   python tools/scripts/no_os_build.py build \
      --project maxm86161 --variant basic --board max32655fthr

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project maxm86161 --variant basic --board max32655fthr \
      --probe openocd --flash

To build the IIO or attribute-test applications instead, swap the ``--variant``
value:

.. code-block:: bash

   # IIO application (host libiio / IIO Oscilloscope over UART)
   python tools/scripts/no_os_build.py build \
      --project maxm86161 --variant iio --board max32655fthr

   # hardware-in-the-loop attribute/register test
   python tools/scripts/no_os_build.py build \
      --project maxm86161 --variant attribute_test --board max32655fthr
