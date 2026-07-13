AD7124-8-PMDZ no-OS Example Project
====================================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
----------------------------

* `EVAL-AD7124-8-PMDZ <https://www.analog.com/EVAL-AD7124-8>`_

Overview
--------

The EVAL-AD7124-8-PMDZ is a PMOD-compatible evaluation board for the
AD7124-8, a low power, low noise, completely integrated analog front end
for high precision measurement applications. The AD7124-8 contains a low
noise, 24-bit sigma-delta ADC and can be configured to have 8
differential inputs or 15 single-ended or pseudo differential inputs.

The device features an on-chip programmable gain amplifier (PGA) with
gains from 1 to 128, a band gap reference with 10 ppm/C drift, matched
programmable excitation currents, and an internal clock oscillator. It
offers three power modes (low, mid, full) with typical current draw
ranging from 255 uA to 930 uA. The AD7124-8 requires an analog domain
supply (AVDD) of 2.7 V to 3.6 V and a digital domain supply (IOVDD) of
1.65 V to 3.6 V.

Applications
------------

* Temperature measurement
* Pressure measurement
* Industrial process control
* Instrumentation
* Smart transmitters

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-AD7124-8-PMDZ board is powered via the PMOD connector (IOVDD).
The AD7124-8 requires an analog supply (AVDD) of 2.7 V to 3.6 V and a
digital supply (IOVDD) of 1.65 V to 3.6 V.

On-board Connectors
~~~~~~~~~~~~~~~~~~~

========= =====================================================
Connector Function
========= =====================================================
P1        PMOD digital interface (CS, SDIN, SDO, SCLK, IOVDD)
P2        Analog inputs (AIN0-AIN7, REFIN1+/-, AVDD, AVSS, CLK)
P3        Extended I/O (AIN8-AIN15, REFOUT, SYNC, PSW)
========= =====================================================

Eight solder jumpers on the board control default connections for
REFIN1, REFOUT, AVDD, AVSS, and SDO routing, allowing operational
mode adjustments without component changes.

No-OS Build Setup
-----------------

Please see: `No-OS Build Guide <https://wiki.analog.com/resources/no-os/build>`_

No-OS Supported Examples
------------------------

This project is organized around the no-OS variant based build flow.
Selecting a variant at build time (``--variant <name>``) chooses which
application is compiled. The platform ``main()`` is a thin dispatcher that
calls ``example_main()``, provided by the selected example. Shared
initialization data (UART, SPI, and AD7124 init parameters) is defined in
`src/common <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad7124-8pmdz/src/common>`__,
and platform-specific macros and extra init parameters are in
`src/platform <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad7124-8pmdz/src/platform>`__.

IIO Example
~~~~~~~~~~~

This project is an IIOD demo for the EVAL-AD7124-8-PMDZ evaluation
board. The project launches the IIOD server on the board so that the
user may connect to it via an IIO client application. Using
IIO-Oscilloscope, the user can read and configure ADC channels,
filters, and data rates.

If you are not familiar with ADI IIO Application, please take a look at:
:dokuwiki:`IIO No-OS </resources/tools-software/no-os-software/iio>`

If you are not familiar with ADI IIO-Oscilloscope Client, please take a
look at:
:dokuwiki:`IIO Oscilloscope </resources/tools-software/linux-software/iio_oscilloscope>`

No-OS Supported Platforms
-------------------------

ADuCM3029
~~~~~~~~~

Used Hardware
^^^^^^^^^^^^^

* `EVAL-ADICUP3029 <https://www.analog.com/EVAL-ADICUP3029>`_
* `EVAL-AD7124-8-PMDZ <https://www.analog.com/EVAL-AD7124-8>`_

Connections
^^^^^^^^^^^

Connect the EVAL-AD7124-8-PMDZ to the EVAL-ADICUP3029 via the PMOD
connector. The SPI signals (CS, DIN, DOUT, SCLK) and power are routed
through the PMOD interface. Connect a micro-USB cable to the P10
connector of the EVAL-ADICUP3029 for UART communication with a PC.

Build Command
^^^^^^^^^^^^^

The ADuCM3029 platform uses the CMake/Ninja build system via the
``no_os_build.py`` helper script. Available variants: ``iio_example``,
``iio_wifi``. Available board: ``eval-adicup3029``.

.. code-block:: bash

   # point at the CrossCore Embedded Studio install (only if not auto-detected)
   export CCES_HOME=/opt/analog/cces/3.0.3

   cd no-OS

   # build the IIO example on the EVAL-ADICUP3029
   python tools/scripts/no_os_build.py build \
      --project ad7124-8pmdz --variant iio_example --board eval-adicup3029

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project ad7124-8pmdz --variant iio_example --board eval-adicup3029 \
      --probe openocd --flash

Maxim
~~~~~

Used Hardware
^^^^^^^^^^^^^

* `AD-APARD32690-SL <https://www.analog.com/AD-APARD32690-SL>`_ (MAX32690)
* `MAX32666FTHR <https://www.analog.com/MAX32666FTHR>`_ (MAX32665)
* `MAX32655FTHR <https://www.analog.com/MAX32655FTHR>`_ (MAX32655)
* `EVAL-AD7124-8-PMDZ <https://www.analog.com/EVAL-AD7124-8>`_

Connections
^^^^^^^^^^^

Connect the EVAL-AD7124-8-PMDZ PMOD to the SPI header of the Maxim
board. The UART console is available at 115200 baud on the default UART
port for each target:

* MAX32690 (AD-APARD32690-SL): SPI4, UART0
* MAX32665 (MAX32666FTHR): SPI1, UART1 (MAP_B, pins P1.12/P1.13)
* MAX32655 (MAX32655FTHR): SPI0, UART0

Build Command
^^^^^^^^^^^^^

The Maxim platform uses the CMake/Ninja build system via the
``no_os_build.py`` helper script. Available boards:
``ad-apard32690-sl``, ``max32655fthr``, ``max32665fthr``.

.. code-block:: bash

   export MAXIM_LIBRARIES=</path/to/MaximSDK/Libraries>

   cd no-OS

   # build the IIO example for AD-APARD32690-SL (MAX32690)
   python tools/scripts/no_os_build.py build \
      --project ad7124-8pmdz --variant iio_example --board ad-apard32690-sl

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project ad7124-8pmdz --variant iio_example --board ad-apard32690-sl \
      --probe openocd --flash

   # build for MAX32655FTHR
   python tools/scripts/no_os_build.py build \
      --project ad7124-8pmdz --variant iio_example --board max32655fthr

   # build for MAX32666FTHR (MAX32665)
   python tools/scripts/no_os_build.py build \
      --project ad7124-8pmdz --variant iio_example --board max32665fthr

