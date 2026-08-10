MAX14916 no-OS Example Project
==============================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
----------------------------

* `MAX14916EVKIT <https://www.analog.com/MAX14916EVKIT>`_

Overview
--------

The `MAX14916 <https://www.analog.com/MAX14916>`_ is a compact industrial octal
high-side switch with integrated diagnostics. Each of the eight channels
features an on-resistance of only 120 mOhm (typ) and handles up to 1.1 A of
continuous current. Two adjacent channels can be combined to form four 2.4 A
switches. The device is controlled via SPI with built-in chip addressing for
up to four devices on a common chip select.

The SPI interface provides per-channel configuration and diagnostics including
undervoltage detection, open-wire detection (in both on and off states),
overload and current limiting reporting, and thermal shutdown protection.
Integrated surge protection (IEC61000-4-5) requires only a single external TVS
on VDD. The device operates from -40 C to +125 C and is available in a compact
48-pin 6 mm x 6 mm QFN package.

Applications
------------

* Industrial digital outputs
* PLC systems

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

* Absolute maximum VDD: 65 V
* Supply current: 2 mA (typ)
* On-resistance: 250 mOhm (max) at 125 C ambient
* Logic voltage interface: 2.5 V to 5.5 V

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max14916/src/common>`__.

The macros used in Common Data are defined in platform specific files
found in the
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max14916/src/platform>`__.

Basic example
~~~~~~~~~~~~~

The basic example initializes the MAX14916 device via SPI and demonstrates
LED control by cycling the status LEDs on each channel. It also configures
device parameters such as the flash LED time, watchdog timer, open-wire OFF
current source, and short-to-VDD threshold, then reads back the configuration
registers for verification.

IIO example
~~~~~~~~~~~

The IIO example initializes the MAX14916 device and configures the IIO
application descriptor with output channels, enabling users to interact
with the device using IIO-based tools.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`__

If you are not familiar with ADI IIO Oscilloscope Client, please take a
look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`__

No-OS Supported Platforms
--------------------------

Maxim
~~~~~

Used Hardware
^^^^^^^^^^^^^

* `MAX14916EVKIT <https://www.analog.com/MAX14916EVKIT>`_
* `AD-APARD32690-SL <https://www.analog.com/AD-APARD32690-SL>`_ or
  `MAX32666FTHR <https://www.analog.com/MAX32666FTHR>`_

Connections
^^^^^^^^^^^

The MAX14916EVKIT communicates via SPI. Connect the SPI signals (SCLK, SDI,
SDO, CS) and GND between the MAX14916EVKIT and the target microcontroller
board.

+-----------------+---------------------+------------------+
| SPI Parameter   | AD-APARD32690-SL    | MAX32666FTHR     |
+=================+=====================+==================+
| SPI Device ID   | 4                   | 1                |
+-----------------+---------------------+------------------+
| SPI Chip Select | 0                   | 0                |
+-----------------+---------------------+------------------+
| SPI Baudrate    | 100000              | 100000           |
+-----------------+---------------------+------------------+

Build Command
^^^^^^^^^^^^^

For toolchain setup and prerequisites, see the
:doc:`Maxim CMake build guide </build_guides/build_maxim_cmake>`.

Available variants: ``basic``, ``iio_example``.
Available boards: ``ad-apard32690-sl``, ``max32666fthr``.
Replace ``--variant`` / ``--board`` accordingly.

.. code-block:: bash

   # point at the Maxim SDK libraries (only if not auto-detected)
   export MAXIM_LIBRARIES=</path/to/MaximSDK/Libraries>
   # Windows (PowerShell): $env:MAXIM_LIBRARIES = "C:\MaximSDK\Libraries"

   cd no-OS

   # build the project (basic example on the ad-apard32690-sl board)
   python tools/scripts/no_os_build.py build \
      --project max14916 --variant basic --board ad-apard32690-sl

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project max14916 --variant basic --board ad-apard32690-sl \
      --probe openocd --flash

Linux Host (USB2PMB2) Platform
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Used Hardware
^^^^^^^^^^^^^

* `MAX14916EVKIT <https://www.analog.com/MAX14916EVKIT>`_
* `USB2PMB2 <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/usb2pmb2.html>`_
  (FTDI FT2232HQ-based USB-to-PMOD adapter)

Connections
^^^^^^^^^^^

The MAX14916EVKIT communicates via SPI. Connect the board to the USB2PMB2
adapter as follows:

+-------------------+-----------------------------+
| MAX14916EVKIT Pin | USB2PMB2 Pin                |
+===================+=============================+
| VDD (3.3 V logic) | 3.3 V output                |
+-------------------+-----------------------------+
| GND               | GND                         |
+-------------------+-----------------------------+
| SCLK              | ADBUS0 (SPI Clock)          |
+-------------------+-----------------------------+
| SDI (MOSI)        | ADBUS1 (SPI MOSI)           |
+-------------------+-----------------------------+
| SDO (MISO)        | ADBUS2 (SPI MISO)           |
+-------------------+-----------------------------+
| CS                | ADBUS3 (SPI Chip Select)    |
+-------------------+-----------------------------+

Prerequisites
^^^^^^^^^^^^^

The linux-userspace build for the USB2PMB2 board uses the FTDI D2XX and
LibMPSSE libraries. Because the FTDI website requires a browser-based download,
obtain the archives manually first and then install them with the provided
helper script:

.. code-block:: bash

   # Install libftd2xx and LibMPSSE from locally downloaded archives
   python3 tools/scripts/config_ftd2xx.py \
      --local-mpsse D2XX_MPSSE_1.0.7_Linux.zip \
      --local-d2xx libftd2xx-linux-x86_64-1.4.35.tgz

Before running the compiled binary, unload the ``ftdi_sio`` kernel module so
the D2XX library can claim the USB device directly:

.. code-block:: bash

   sudo rmmod ftdi_sio usbserial

Build Command
^^^^^^^^^^^^^

For toolchain setup and prerequisites, see the
`Linux Userspace CMake build guide <https://analogdevicesinc.github.io/no-OS/build_guides/build_linux_userspace_cmake.html>`__.

Available variants: ``basic``, ``iio_example``.
Available boards: ``usb2pmb2``.
Replace ``--variant`` accordingly.

No toolchain environment variable is required — the system ``gcc`` is
used automatically.

.. code-block:: bash

   cd no-OS

   # build the basic example for the USB2PMB2 board
   python3 tools/scripts/no_os_build.py build \
      --project max14916 --variant basic --board usb2pmb2

   # build the iio_example for the USB2PMB2 board
   python3 tools/scripts/no_os_build.py build \
      --project max14916 --variant iio_example --board usb2pmb2

Run the resulting binary directly on the Linux host (``sudo`` is required for
D2XX USB access):

.. code-block:: bash

   sudo ./build/max14916-basic-usb2pmb2/build/max14916

For the ``iio_example`` variant, the IIOD server listens on the loopback
interface. Connect an IIO client once the binary is running:

.. code-block:: bash

   iio_info -u ip:127.0.0.1
