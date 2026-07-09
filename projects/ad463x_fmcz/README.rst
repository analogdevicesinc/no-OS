AD463x-FMCZ no-OS Example Project
==================================

.. no-os-doxygen::

.. contents:: Table of Contents
   :depth: 3

Supported Evaluation Boards
---------------------------

* `EVAL-AD4030-24FMCZ <https://www.analog.com/EVAL-AD4030-24>`_
* `EVAL-AD4630-16FMCZ <https://www.analog.com/EVAL-AD4630-16>`_
* `EVAL-AD4630-20FMCZ <https://www.analog.com/EVAL-AD4630-20>`_
* `EVAL-AD4630-24FMCZ <https://www.analog.com/EVAL-AD4630-24>`_
* `EVAL-ADAQ4224-FMCZ <https://www.analog.com/ADAQ4224>`_
* `EV-ISO-4224-FMCZ <https://www.analog.com/ADAQ4224>`_

Overview
--------

The EVAL-AD4630-24FMCZ, EVAL-AD4030-24FMCZ, and EVAL-AD4630-16FMCZ
evaluation boards enable quick and easy evaluation of the AD4X3X family
of 24-bit and 16-bit precision successive approximation register (SAR)
analog-to-digital converters (ADCs). The `AD4630-24 <https://www.analog.com/AD4630-24>`_
and `AD4630-16 <https://www.analog.com/AD4630-16>`_ are 2 MSPS per channel,
low power, dual channel 24-bit or 16-bit SAR ADCs, while the
`AD4030-24 <https://www.analog.com/AD4030-24>`_ is a single channel
24-bit precision SAR ADC that supports a sampling rate of up to 2 MSPS.
The evaluation boards demonstrate the performance of either the
AD4630-24, AD4030-24, or AD4630-16 and provide a configurable analog
front end (AFE) for a variety of system applications.

The evaluation boards are designed for use with a ZedBoard or the
SDP-K1 controller board. The ZedBoard uses the FMC low pin count (LPC)
connector for high-speed data capture and buffering.

.. note::

   The name ``AD463x`` is used as a generic identifier in the no-OS driver code;
   this driver supports multiple devices in the AD463x family including
   AD4630-24, AD4630-20, AD4630-16, AD4631-24, AD4631-20, AD4631-16,
   AD4632-24, AD4632-20, AD4632-16, AD4030, ADAQ4216, and ADAQ4224.

Applications
------------

* Automatic test equipment
* Digital control loops
* Medical instrumentation
* Seismology
* Semiconductor manufacturing
* Scientific instrumentation

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-AD4630-24FMCZ evaluation board derives its primary 12V power
supply directly from the ZedBoard via the FMC connector. This 12V input
is first converted to a 7.5V intermediate voltage by a switching
regulator before being post-regulated to provide various required
voltage rails. Moreover, the 12V is instrumental in generating negative
rail voltages, such as the -3.3V needed for the buffers and drive
amplifiers.

============  =========================================  ========  ========
Power Supply  Function                                   Min. (V)  Max. (V)
============  =========================================  ========  ========
\+12V         12V primary supply via FMC connector       N/A       N/A
GND           Ground connection                          N/A       N/A
\+3.3V        3.3V for various digital logic             3.26      3.33
\+1.8V        1.8V for the ADC                           1.77      1.81
VIO           1.8V supply for the ADC digital I/O        1.77      1.81
\+5.4V        5.4V for the ADC                           5.34      5.46
REFIN         5V ADC reference input                     4.95      5.05
VAMP+         Positive supply for the amplifiers         6.35      6.5
VAMP-         Negative supply for the amplifiers         -3.35     -3.28
VP1           7.5V at the input of the switcher          7.425     7.575
REF           5V at the ADC reference output             4.95      5.05
EN            1.8V enable signal for power supplies      1.75      1.85
============  =========================================  ========  ========

No-OS Build Setup
-----------------

Please see: `No-OS Build Guide <https://wiki.analog.com/resources/no-os/build>`__

No-OS Supported Examples
------------------------

This project uses the no-OS variant-based build flow. Selecting a variant
at build time (``--variant <name>``) chooses which application is compiled.
The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad463x_fmcz/src/common>`__

The macros used in Common Data are defined in platform specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad463x_fmcz/src/platform>`__

Basic Example
~~~~~~~~~~~~~

The basic example demonstrates initialization, configuration, and data
acquisition for AD463x series ADCs. The ``basic_example_main`` function
initializes the ADC using ``ad463x_init``. If the ADAQ4224 is used, gain
settings are configured via a scale table with ``ad463x_set_pgia_gain``.
The function then exits the register configuration mode, reads ADC data
into a buffer, sign-extends the results with ``no_os_sign_extend32``, and
prints data for both channels to the console.

IIO Example
~~~~~~~~~~~

The IIO example initializes the AD463x ADC and sets up the IIO interface
through ``iio_example_main()``. It allocates and initializes an
``ad463x_dev`` structure, alongside an ``iio_ad463x`` structure to support
IIO functions. The example routinely streams data from the ADC via an
IIO-configured buffer, showcasing ADC configuration and data management
capabilities using the IIO framework.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`__

If you are not familiar with ADI IIO-Oscilloscope Client, please take a
look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`__

No-OS Supported Platforms
--------------------------

Xilinx
~~~~~~

Used Hardware
^^^^^^^^^^^^^

* `EVAL-AD4630-24FMCZ <https://www.analog.com/EVAL-AD4630-24>`_
* `ZedBoard <https://www.analog.com/en/resources/reference-designs/powering-zynq-evaluation-development-board-zedboard.html>`_

Connections
^^^^^^^^^^^

* Connect the EVAL-AD4630-24FMCZ to the ZedBoard through the FMC LPC
  connector for data and 12V power supply.
* Connect differential signal sources to the input channels via the SMA
  connectors.
* Set the VADJ SELECT jumper to the correct voltage (e.g., 2.5V) to avoid
  damage.
* Optionally connect an external clock input (up to 100 MHz) via the CLK IN
  SMA connector.
* Configure the ZedBoard boot jumpers for SD card boot.
* Connect a USB cable to the ZedBoard USB-UART port for serial communication.
* Connect a power supply to J20/DC Input to power the system.

The UART console appears on the ZedBoard USB-UART port at 115200 baud, 8N1.

Build Command
^^^^^^^^^^^^^

The Xilinx platform uses the CMake/Ninja build system via the
``no_os_build.py`` helper script. Available variants: ``basic``, ``iio``.
Available boards: ``zed``.

For toolchain setup and prerequisites, see the
`Xilinx CMake build guide <https://analogdevicesinc.github.io/no-OS/build_guides/build_xilinx_cmake.html>`__.

.. code-block:: bash

   # Source the Vitis environment (sets XILINX_VITIS and adds tools to PATH)
   source /path/to/Vitis/settings64.sh
   # PowerShell (Windows) equivalent:
   #   $env:XILINX_VITIS = "<C:\path\to\Vitis>"

   cd no-OS

   # build the example on the ZedBoard (replace --variant as needed)
   python tools/scripts/no_os_build.py build \
      --project ad463x_fmcz --variant basic --board zed \
      --hardware /path/to/system_top.xsa

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project ad463x_fmcz --variant basic --board zed \
      --hardware /path/to/system_top.xsa \
      --probe openocd --flash

STM32
~~~~~

Used Hardware
^^^^^^^^^^^^^

* `EVAL-AD4630-24FMCZ <https://www.analog.com/EVAL-AD4630-24>`_
* `EVAL-SDP-CK1Z (SDP-K1) <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/sdp-k1.html>`_

Connections
^^^^^^^^^^^

* Align and mate the EVAL-AD4630-24FMCZ with the SDP-K1 120-pin FMC
  connector. The evaluation board receives +12V, +3.3V, and +18V through
  the connector interface.
* Connect a 7V to 12V DC power supply (center positive, min 300 mA,
  max 3.5 A) to the SDP-K1 DC jack.
* Set the VIO_ADJUST header to the correct voltage (1.8V or 3.3V) before
  powering to avoid damage.
* Verify the SYS_PWR LED lights up to indicate proper power.
* Connect a USB standard-A to mini-B cable from the SDP-K1 USB mini-B
  port to the PC.

The UART console appears on the SDP-K1 USB virtual COM port at 115200 baud, 8N1.

Build Command
^^^^^^^^^^^^^

The STM32 platform uses the CMake/Ninja build system via the
``no_os_build.py`` helper script. Available variants: ``basic``, ``iio``.
Available boards: ``sdp-ck1z``.

For toolchain setup and prerequisites, see the
`STM32 CMake build guide <https://analogdevicesinc.github.io/no-OS/build_guides/build_stm32_cmake.html>`__.

.. code-block:: bash

   # point at your STM32CubeMX installation
   export STM32CUBEMX_EXECUTABLE=/path/to/STM32CubeMX
   # PowerShell (Windows) equivalent:
   #   $env:STM32CUBEMX_EXECUTABLE = "<C:\path\to\STM32CubeMX>"

   cd no-OS

   # build the example on the SDP-K1 board (replace --variant as needed)
   python tools/scripts/no_os_build.py build \
      --project ad463x_fmcz --variant basic --board sdp-ck1z

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project ad463x_fmcz --variant basic --board sdp-ck1z \
      --probe openocd --flash
