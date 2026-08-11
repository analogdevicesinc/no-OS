Evaluating the MAX31827
=======================

.. no-os-doxygen::

.. contents::
	:depth: 3

Supported Evaluation Boards
----------------------------

* `MAX31827EVKIT <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/max31827evkit.html>`_

Overview
--------

The MAX31827 evaluation kit (EV kit) demonstrates the MAX31827 I2C temperature
switch and sensor with hardware-selectable address and alarm. The MAX31827 EV
kit comes with the 6-pin WLP MAX31827ANTABRPF+ installed.

The MAX31827 is a low-power temperature sensor and programmable thermal switch
that communicates via a 400 kHz I2C-compatible interface. It provides a
resolution of 0.0625 degrees C (10-bit) and operates from a 1.7 V to 3.6 V
supply. The device features programmable over-temperature and under-temperature
alarm thresholds with configurable hysteresis, a one-shot conversion mode, and
a hardware address-selection pin that allows up to eight unique I2C addresses
on the same bus.

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The MAX31827EVKIT operates from a 1.7 V to 3.6 V supply connected to the VDD
pin. When used with the MAX32666FTHR carrier, the board is powered from the
3.3 V rail provided by the carrier. When used with the USB2PMB2 FTDI adapter,
the 3.3 V output of the adapter supplies power.

Board Connector and Jumper Settings
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Pin Description**

The following table describes the signal pins of the MAX31827EVKIT:

+----------+-------------------------------------------+
| Name     | Description                               |
+----------+-------------------------------------------+
| VDD      | Connect to 3.3 V supply                   |
+----------+-------------------------------------------+
| GND      | Connect to Ground                         |
+----------+-------------------------------------------+
| SCL      | Connect to I2C Clock (SCL)                |
+----------+-------------------------------------------+
| SDA      | Connect to I2C Data (SDA)                 |
+----------+-------------------------------------------+

No-OS Supported Examples
------------------------

This project is organized around the no-OS variant based build flow.
Selecting a variant at build time (``--variant <name>``) chooses which
application is compiled. The platform ``main()`` is a thin dispatcher that
calls ``example_main()``, provided by the selected example. Shared
initialization data is defined in
`src/common <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max31827-evkit/src/common>`__,
and platform-specific macros and extra init parameters are in
`src/platform <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max31827-evkit/src/platform>`__.

Basic example
~~~~~~~~~~~~~

This is a simple example that initializes the MAX31827 and reads input
temperature periodically. The temperature values are printed to standard output
in a continuous loop.

IIO example
~~~~~~~~~~~

This project is actually a IIOD demo for MAX31827EVKIT. The project launches a
IIOD server on the board so that the user may connect to it via an IIO client.

Using IIO-Oscilloscope, the user can configure the device.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with ADI IIO-Oscilloscope Client, please take a look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

The No-OS IIO Application together with the No-OS IIO MAX31827 driver take care
of all the back-end logic needed to setup the IIO server.

This example initializes the IIO device and calls the IIO app as shown in:
`IIO Example <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max31827-evkit/src/examples/iio_example>`_

No-OS Supported Platforms
--------------------------

Maxim Platform
~~~~~~~~~~~~~~

Used Hardware
^^^^^^^^^^^^^

* `MAX31827EVKIT <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/max31827evkit.html>`_
* `MAX32666FTHR <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/max32666fthr.html>`_

Connections
^^^^^^^^^^^

+-----------------------+-----------------------+------------------+
| MAX31827EVKIT Pin     | Function              | MAX32666FTHR Pin |
+-----------------------+-----------------------+------------------+
| VDD                   | VDD                   | 3V3              |
+-----------------------+-----------------------+------------------+
| SCL                   | I2C Clock (SCL)       | P0_6 (I2C0_SCL)  |
+-----------------------+-----------------------+------------------+
| SDA                   | I2C Data (SDA)        | P0_7 (I2C0_SDA)  |
+-----------------------+-----------------------+------------------+
| GND                   | Ground (GND)          | GND              |
+-----------------------+-----------------------+------------------+

Build Command
^^^^^^^^^^^^^

For toolchain setup and prerequisites, see the
:doc:`Maxim CMake build guide </build_guides/build_maxim_cmake>`.

Available variants: ``basic``, ``iio_example``.
Available boards: ``max32666fthr``.
Replace ``--variant`` / ``--board`` accordingly.

.. code-block:: bash

   # point at the Maxim SDK libraries (only if not auto-detected)
   export MAXIM_LIBRARIES=</path/to/MaximSDK/Libraries>
   # Windows (PowerShell): $env:MAXIM_LIBRARIES = "C:\MaximSDK\Libraries"

   cd no-OS

   # build the project (basic example on the max32666fthr board)
   python tools/scripts/no_os_build.py build \
      --project max31827-evkit --variant basic --board max32666fthr

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project max31827-evkit --variant basic --board max32666fthr \
      --probe openocd --flash

Linux Host (USB2PMB2) Platform
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Used Hardware
^^^^^^^^^^^^^

* `MAX31827EVKIT <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/max31827evkit.html>`_
* `USB2PMB2 <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/usb2pmb2.html>`_
  (FTDI FT2232HQ-based USB-to-PMOD adapter)

Connections
^^^^^^^^^^^

The MAX31827EVKIT communicates via I2C. Connect the board to the USB2PMB2
adapter as follows:

+-------------------+-----------------------------+
| MAX31827EVKIT Pin | USB2PMB2 Pin                |
+===================+=============================+
| VDD               | 3.3 V output                |
+-------------------+-----------------------------+
| GND               | GND                         |
+-------------------+-----------------------------+
| SCL               | ADBUS1 (I2C SCL)            |
+-------------------+-----------------------------+
| SDA               | ADBUS2 (I2C SDA)            |
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
      --project max31827-evkit --variant basic --board usb2pmb2

   # build the iio_example for the USB2PMB2 board
   python3 tools/scripts/no_os_build.py build \
      --project max31827-evkit --variant iio_example --board usb2pmb2

Run the resulting binary directly on the Linux host (``sudo`` is required for
D2XX USB access):

.. code-block:: bash

   sudo ./build/max31827-evkit-basic-usb2pmb2/build/max31827-evkit

For the ``iio_example`` variant, the IIOD server listens on the loopback
interface. Connect an IIO client once the binary is running:

.. code-block:: bash

   iio_info -u ip:127.0.0.1
