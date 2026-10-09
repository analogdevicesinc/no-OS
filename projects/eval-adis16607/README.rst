ADIS16607 Family no-OS Example Project
======================================

.. no-os-doxygen::


Contents
--------

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
---------------------------

* `ADIS16607-2 <https://www.analog.com/ADIS16607>`_
* `ADIS16607-3 <https://www.analog.com/ADIS16607>`_

Overview
--------

The ADIS16607 is a precision inertial measurement unit (IMU) that includes a
triaxial gyroscope and a triaxial accelerometer. Each inertial sensor in the
ADIS16607 combines industry leading MEMS technology with signal conditioning
that optimizes dynamic performance. The factory calibration characterizes each
sensor for sensitivity, bias, and alignment. As a result, each sensor has its
own dynamic compensation formulas that provide accurate sensor measurements.

The ADIS16607 provides a simple, cost-effective method for integrating accurate,
multiaxis inertial sensing into industrial systems. All necessary motion testing
and calibration are part of the production process at the factory, greatly
reducing system integration time.

The device supports both SPI (half-duplex and full-duplex modes) and I2C
communication interfaces. Additional features include:

* 32-bit burst mode for high-resolution data acquisition
* FIFO buffer for efficient data collection
* External synchronization modes (direct and scaled)
* Programmable decimation filter
* Built-in self-test functionality

**Device Variants**

+-------------+------------------+------------------+
| Device      | Gyroscope Range  | Accelerometer    |
+-------------+------------------+------------------+
| ADIS16607-2 | +/-450 deg/s     | +/-40 g          |
+-------------+------------------+------------------+
| ADIS16607-3 | +/-2000 deg/s    | +/-40 g          |
+-------------+------------------+------------------+

Applications
------------

* Precision instrumentation, stabilization
* Guidance, navigation, and control
* Precision autonomous machines and robotics
* Dead reckoning in global positioning system (GPS) denied environments
* Industrial automation and motion control

Hardware Specifications
-----------------------

Power Supply Requirements
^^^^^^^^^^^^^^^^^^^^^^^^^

The ADIS16607 eval devices have to be supplied with 3.3V voltage on VDD pin.

**Pin Description**

        Please see the following table for the pin assignments for the interface connector (J1).

        +-----+------+-------------------------------------------------------+
        | Pin | Name | Description                                           |
        +-----+------+-------------------------------------------------------+
        | 1   | ~RST | Reset, active low                                     |
        +-----+------+-------------------------------------------------------+
        | 2   | SCLK | Serial Clock (Serial Peripheral Interface)            |
        +-----+------+-------------------------------------------------------+
        | 3   | ~CS  | Chip Select (Serial Peripheral Interface), Active Low |
        +-----+------+-------------------------------------------------------+
        | 4   | DOUT | Data Output (Serial Peripheral Interface)             |
        +-----+------+-------------------------------------------------------+
        | 5   | DNC  | Do not connect                                        |
        +-----+------+-------------------------------------------------------+
        | 6   | DIN  | Data Input (Serial Peripheral Interface)              |
        +-----+------+-------------------------------------------------------+
        | 7   | GND  | Ground                                                |
        +-----+------+-------------------------------------------------------+
        | 8   | GND  | Ground                                                |
        +-----+------+-------------------------------------------------------+
        | 9   | GND  | Ground                                                |
        +-----+------+-------------------------------------------------------+
        | 10  | VDD  | Power Supply, +3.3V                                   |
        +-----+------+-------------------------------------------------------+
        | 11  | VDD  | Power Supply, +3.3V                                   |
        +-----+------+-------------------------------------------------------+
        | 12  | VDD  | Power Supply, +3.3V                                   |
        +-----+------+-------------------------------------------------------+
        | 13  | DR   | Data Ready                                            |
        +-----+------+-------------------------------------------------------+
        | 14  | SYNC | Sync Input                                            |
        +-----+------+-------------------------------------------------------+
        | 15  | DNC  | Do not connect                                        |
        +-----+------+-------------------------------------------------------+
        | 16  | DNC  | Do not connect                                        |
        +-----+------+-------------------------------------------------------+

**Cabling**

        J1 supports connection with the following style of cables: 2.00 mm IDC Ribbon Cable Assembly.

        TIP: Use "2.00 mm IDC Ribbon Cable Assembly" as search criteria to find the latest options on the market.

        At the time of initial release for these breakout boards, we were most familiar with the `TCSD Series from Samtec <https://www.samtec.com/products/tcsd>`_.

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-adis16607/src/common>`_

The macros used in Common Data are defined in platform specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-adis16607/src/platform>`_

Basic example
^^^^^^^^^^^^^

This is a simple example which initializes the ADIS16607 selected device and
performs angular velocity, acceleration and temperature readings in a while loop
with a period of 1s. The data is printed on the serial interface.

In order to build the basic example use the ``basic_example.conf`` configuration file when
invoking CMake (see the **Build Command** section below).

IIO example
^^^^^^^^^^^

This project is actually a IIOD demo for EVAL-ADIS16607 device series.
The project launches a IIOD server on the board so that the user may connect
to it via an IIO client.
Using IIO-Oscilloscope, the user can configure the IMU and view the measured data on a plot.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with ADI IIO-Oscilloscope Client, please take a look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

The No-OS IIO Application together with the No-OS IIO ADIS driver take care of
all the back-end logic needed to setup the IIO server.

This example initializes the IIO device and calls the IIO app as shown in:
`IIO Trigger Example <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-adis16607/src/examples/iio_trigger_example>`_

The read buffer is used for storing the burst data which shall be retrieved periodically by any LibIIO client.
The measured data is sampled using a hardware trigger (e.g. interrupts).
ADIS16607 offers the capability to use DATA_READY pin as a flag which shows when
new measurements are available. Thus, DATA_READY pin is used as a hardware trigger.
The example code maps the DATA_READY pin as GPIO input with interrupt capabilities.
When DATA_READY pin transitions from low to high, new data is available and will
be read based on is_synchronous flag setting used in adis_iio_trigger_desc.
If the flag is set to true, the data will be read immediately, in the interrupt context.
If the flag is set to false, the data will be read from application context. In this case some samples might be missed.

The ADIS16607 driver also supports FIFO mode for efficient data collection. When FIFO
is enabled, multiple samples can be buffered in the device, reducing the interrupt
frequency and allowing for more efficient data retrieval.

In order to build the IIO trigger example use the ``iio_trigger_example.conf`` configuration
file when invoking CMake (see the **Build Command** section below).

No-OS Supported Platforms
-------------------------

Maxim Platform
^^^^^^^^^^^^^^

**Used hardware**:

* `ADIS16607 <https://www.analog.com/ADIS16607>`_ with
* `AD-APARD32690-SL <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/ad-apard32690-sl.html>`_

**Connections**:

+---------------------------+----------+-------------------------------------------------------+--------------------------+
| EVAL-ADIS16607 Pin Number | Mnemonic | Function                                              | AD-APARD32690 Pin Number |
+---------------------------+----------+-------------------------------------------------------+--------------------------+
| 1                         | ~RST     | Reset, active low                                     | P2_21                    |
+---------------------------+----------+-------------------------------------------------------+--------------------------+
| 2                         | SCLK     | Serial Clock (SPI4)                                   | P2_27                    |
+---------------------------+----------+-------------------------------------------------------+--------------------------+
| 3                         | ~CS      | Chip Select (Serial Peripheral Interface), Active Low | P2_26                    |
+---------------------------+----------+-------------------------------------------------------+--------------------------+
| 4                         | DOUT     | Data Output (Serial Peripheral Interface)             | P2_28                    |
+---------------------------+----------+-------------------------------------------------------+--------------------------+
| 6                         | DIN      | Data Input (Serial Peripheral Interface)              | P2_29                    |
+---------------------------+----------+-------------------------------------------------------+--------------------------+
| 7                         | GND      | Ground                                                | GND                      |
+---------------------------+----------+-------------------------------------------------------+--------------------------+
| 10                        | VDD      | Power Supply, +3.3V                                   | 3V3                      |
+---------------------------+----------+-------------------------------------------------------+--------------------------+
| 13                        | DR       | Data Ready                                            | P1_6                     |
+---------------------------+----------+-------------------------------------------------------+--------------------------+

**Build Command**

.. code-block:: bash

        # Configure (select example via the conf file)
        cmake --preset ad-apard32690-sl \
              -DPROJECT_DEFCONFIG=eval-adis16607/iio_trigger_example.conf \
              -S . -B build/eval-adis16607
        # Build
        cmake --build build/eval-adis16607 --target eval-adis16607
        # Flash (using OpenOCD)
        cmake --build build/eval-adis16607 --target eval-adis16607-flash
