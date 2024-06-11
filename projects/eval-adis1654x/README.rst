Evaluating the ADIS1654X Family
===============================


Contents
--------

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
---------------------------

* `ADIS16545 <https://www.analog.com/ADIS16545>`__
* `ADIS16547 <https://www.analog.com/ADIS16547>`_

Overview
--------

The ADIS16545/ADIS16547 are a complete inertial system that includes a triaxis
gyroscope and a triaxis accelerometer. Each inertial sensor in the
ADIS16545/ADIS16547 combines with signal conditioning that optimizes dynamic
performance. The factory calibration characterizes each sensor for sensitivity,
bias, and alignment. As a result, each sensor has its own dynamic compensation
formulas that provide accurate sensor measurements.

The ADIS16545/ADIS16547 provide a simple, cost effective method for integrating
accurate, multiaxis inertial sensing into industrial systems, especially when
compared with the complexity and investment associated with discrete designs.
All necessary motion testing and calibration are part of the production process
at the factory, greatly reducing system integration time. Tight orthogonal
alignment and precision alignment features simplify inertial frame alignment in
navigation systems. The serial peripheral interface (SPI) and register structure
provide a simple interface for data collection and configuration control.

Applications
------------

* Precision instrumentation, stabilization
* Guidance, navigation, and control
* Avionics, unmanned vehicles
* Precision autonomous machines and robotics

Hardware Specifications
-----------------------

Power Supply Requirements
^^^^^^^^^^^^^^^^^^^^^^^^^

The ADIS1654X eval devices have to be supplied with 3.3V voltage on VDD pin.

**Pin Description**

        Please see the following table for the pin assignments for the interface connector (J1).

        +----------------+--------+-------------------------------------------------------+
        | Pin            | Name   | Description                                           |
        +----------------+--------+-------------------------------------------------------+
        | 1              | DIO3   | Configurable Digital Input and Output 3               |
        +----------------+--------+-------------------------------------------------------+
        | 2              | DIO4   | Configurable Digital Input and Output 4               |
        +----------------+--------+-------------------------------------------------------+
        | 3              | SCLK   | Serial Clock (Serial Peripheral Interface)            |
        +----------------+--------+-------------------------------------------------------+
        | 4              | DOUT   | Data Output (Serial Peripheral Interface)             |
        +----------------+--------+-------------------------------------------------------+
        | 5              | DIN    | Data Input (Serial Peripheral Interface)              |
        +----------------+--------+-------------------------------------------------------+
        | 6              | ~CS    | Chip Select (Serial Peripheral Interface), Active Low |
        +----------------+--------+-------------------------------------------------------+
        | 7              | DIO1   | Configurable Digital Input and Output 1               |
        +----------------+--------+-------------------------------------------------------+
        | 8              | ~RST   | Reset, active low                                     |
        +----------------+--------+-------------------------------------------------------+
        | 9              | DIO2   | Configurable Digital Input and Output 2               |
        +----------------+--------+-------------------------------------------------------+
        | 10, 11         | VDD    | Power Supply, +5V                                     |
        +----------------+--------+-------------------------------------------------------+
        | 12, 15         | NO PIN | No pin                                                |
        +----------------+--------+-------------------------------------------------------+
        | 13, 14         | GND    | Ground                                                |
        +----------------+--------+-------------------------------------------------------+
        | 16 to 24       | DNC    | Do not connect these pins.                            |
        +----------------+--------+-------------------------------------------------------+


**Cabling**

        ADIS1654X is compatible with `ADIS16IMU1/PCBZ <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/EVAL-ADIS16IMU1.html>`_
        which has a J1 connector.

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
        | 13  | DIO1 | Configurable Digital Input and Output 1               |
        +-----+------+-------------------------------------------------------+
        | 14  | DIO2 | Configurable Digital Input and Output 2               |
        +-----+------+-------------------------------------------------------+
        | 15  | DIO3 | Configurable Digital Input and Output 3               |
        +-----+------+-------------------------------------------------------+
        | 16  | DIO4 | Configurable Digital Input and Output 4               |
        +-----+------+-------------------------------------------------------+

        J1 supports connection with the following style of cables: 2.00 mm IDC Ribbon Cable Assembly.

        TIP: Use “2.00 mm IDC Ribbon Cable Assembly” as search criteria to find the latest options on the market.

        At the time of initial release for these breakout boards, we were most familiar with the `TCSD Series from Samtec <https://www.samtec.com/products/tcsd>`_.

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-adis1654x/src/common>`_

The macros used in Common Data are defined in platform specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-adis1654x/src/platform>`_

Basic example
^^^^^^^^^^^^^

This is a simple example which initializes the adis1654x selected device and
performs angular velocity, acceleration and temperature readings in a while loop
with a period of 1s. The data is printed on the serial interface.

In order to build the IIO project make sure you have the following configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-adis1654x/Makefile>`_

.. code-block:: bash

        # Select the example you want to enable by choosing y for enabling and n for disabling
        BASIC_EXAMPLE = y
        IIO_TRIGGER_EXAMPLE = n

IIO example
^^^^^^^^^^^

This project is actually a IIOD demo for EVAL-ADIS1654X device series.
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
`IIO Trigger Example <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-adis1654x/src/examples/iio_trigger_example>`_

The read buffer is used for storing the burst data which shall be retrieved periodically by any LibIIO client.
The measured data is sampled using a hardware trigger (e.g. interrupts).
ADIS1654X offers the capability to use DATA_READY pin as a flag which shows when
new measurements are available. Thus, DATA_READY pin is used as a hardware trigger.
The example code maps the DATA_READY pin as GPIO input with interrupt capabilities.
When DATA_READY pin transitions from low to high, new data is available and will
be read based on is_synchronous flag setting used in adis_iio_trigger_desc.
If the flag is set to true, the data will be read immediately, in the interrupt context.
If the flag is set to false, the data will be read from application context. In this case some samples might be missed.

In order to build the IIO project make sure you have the following configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-adis1654x/Makefile>`_

.. code-block:: bash

        # Select the example you want to enable by choosing y for enabling and n for disabling
        BASIC_EXAMPLE = n
        IIO_TRIGGER_EXAMPLE = y

No-OS Supported Platforms
-------------------------

STM32 Platform
^^^^^^^^^^^^^^

**Used hardware**:

* `ADIS16545 <https://www.analog.com/en/products/adis16545.html>`__ with
* `SDP-K1 <https://www.analog.com/en/design-center/evaluation-hardware-and-software/evaluation-boards-kits/sdp-k1.html>`_ and
* `ADIS16IMU1/PCBZ <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/EVAL-ADIS16IMU1.html>`_ and
* ST debugger

**Connections**:

+-----------------------------------+----------+-------------------------------------------------------+-------------------+
| ADIS16IMU1/PCBZ J1 with ADIS1654X | Mnemonic | Function                                              | SDP-K1 Pin Number |
+-----------------------------------+----------+-------------------------------------------------------+-------------------+
| 1                                 | ~RST     | Reset, active low                                     | ANALOG IN A1      |
+-----------------------------------+----------+-------------------------------------------------------+-------------------+
| 2                                 | SCLK     | Serial Clock                                          | DIGITAL 13        |
+-----------------------------------+----------+-------------------------------------------------------+-------------------+
| 3                                 | ~CS      | Chip Select (Serial Peripheral Interface), Active Low | DIGITAL 10        |
+-----------------------------------+----------+-------------------------------------------------------+-------------------+
| 4                                 | DOUT     | Data Output (Serial Peripheral Interface)             | DIGITAL 12        |
+-----------------------------------+----------+-------------------------------------------------------+-------------------+
| 6                                 | DIN      | Data Input (Serial Peripheral Interface)              | DIGITAL 11        |
+-----------------------------------+----------+-------------------------------------------------------+-------------------+
| 7                                 | GND      | Ground                                                | DIGITAL GND       |
+-----------------------------------+----------+-------------------------------------------------------+-------------------+
| 10                                | VDD      | Power Supply, +3.3V                                   | POWER 3.3V        |
+-----------------------------------+----------+-------------------------------------------------------+-------------------+
| 14                                | DIO2     | Configurable Digital Input and Output 2               | ANALOG IN A0      |
+-----------------------------------+----------+-------------------------------------------------------+-------------------+

**Build Command**

.. code-block:: bash

        # to delete current build
        make reset
        # to build the project
        make PLATFORM=stm32
        # to flash the code
        make run
        # to debug the code
        make debug

Maxim Platform
^^^^^^^^^^^^^^

**Used hardware**:

* `ADIS16545 <https://www.analog.com/en/products/adis16545.html>`__ with
* `ADIS16IMU1/PCBZ <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/EVAL-ADIS16IMU1.html>`_ and
* `MAX78000 <https://www.analog.com/en/products/max78000.html>`_

**Connections**:

+-----------------------------------+----------+-------------------------------------------------------+---------------------+
| ADIS16IMU1/PCBZ J1 with ADIS1654X | Mnemonic | Function                                              | MAX78000 Pin Number |
+-----------------------------------+----------+-------------------------------------------------------+---------------------+
| 1                                 | ~RST     | Reset, active low                                     | P0_19               |
+-----------------------------------+----------+-------------------------------------------------------+---------------------+
| 2                                 | SCLK     | Serial Clock                                          | P0_7                |
+-----------------------------------+----------+-------------------------------------------------------+---------------------+
| 3                                 | ~CS      | Chip Select (Serial Peripheral Interface), Active Low | P0_11               |
+-----------------------------------+----------+-------------------------------------------------------+---------------------+
| 4                                 | DOUT     | Data Output (Serial Peripheral Interface)             | P0_6                |
+-----------------------------------+----------+-------------------------------------------------------+---------------------+
| 6                                 | DIN      | Data Input (Serial Peripheral Interface)              | P0_5                |
+-----------------------------------+----------+-------------------------------------------------------+---------------------+
| 7                                 | GND      | Ground                                                | GND                 |
+-----------------------------------+----------+-------------------------------------------------------+---------------------+
| 10                                | VDD      | Power Supply, +3.3V                                   | 3V3                 |
+-----------------------------------+----------+-------------------------------------------------------+---------------------+
| 14                                | DIO2     | Configurable Digital Input and Output 2               | P1_6                |
+-----------------------------------+----------+-------------------------------------------------------+---------------------+

**Build Command**

.. code-block:: bash

        # to delete current build
        make reset
        # to build the project
        make PLATFORM=maxim TARGET=max78000
        # to flash the code
        make run
        # to debug the code
        make debug

Pico Platform
^^^^^^^^^^^^^

**Used hardware**:

* `ADIS16545 <https://www.analog.com/en/products/adis16545.html>`__ with
* `ADIS16IMU1/PCBZ <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/EVAL-ADIS16IMU1.html>`_ and
* Raspberry Pi Pico with
* `ADALM-UARTJTAG <https://www.analog.com/en/design-center/evaluation-hardware-and-software/evaluation-boards-kits/ADALM-UARTJTAG.html>`_ Adapter for Raspberry Pi Pico UART to USB Connection

**Connections**:

+-----------------------------------+----------+-------------------------------------------------------+---------------------+
| ADIS16IMU1/PCBZ J1 with ADIS1654X | Mnemonic | Function                                              | MAX78000 Pin Number |
+-----------------------------------+----------+-------------------------------------------------------+---------------------+
| 1                                 | ~RST     | Reset, active low                                     | GP20                |
+-----------------------------------+----------+-------------------------------------------------------+---------------------+
| 2                                 | SCLK     | Serial Clock                                          | GP18                |
+-----------------------------------+----------+-------------------------------------------------------+---------------------+
| 3                                 | ~CS      | Chip Select (Serial Peripheral Interface), Active Low | GP17                |
+-----------------------------------+----------+-------------------------------------------------------+---------------------+
| 4                                 | DOUT     | Data Output (Serial Peripheral Interface)             | GP16                |
+-----------------------------------+----------+-------------------------------------------------------+---------------------+
| 6                                 | DIN      | Data Input (Serial Peripheral Interface)              | GP19                |
+-----------------------------------+----------+-------------------------------------------------------+---------------------+
| 7                                 | GND      | Ground                                                | GND                 |
+-----------------------------------+----------+-------------------------------------------------------+---------------------+
| 10                                | VDD      | Power Supply, +3.3V                                   | 3V3                 |
+-----------------------------------+----------+-------------------------------------------------------+---------------------+
| 14                                | DIO2     | Configurable Digital Input and Output 2               | GP21                |
+-----------------------------------+----------+-------------------------------------------------------+---------------------+

The following table shows how the connection between ADALM-UARTJTAG and Raspberry Pi Pico is realized in this project example.

+---------------------------+------------------------------+--------------+
| ADALM-UARTJTAG Pin Number | Raspberry Pi Pico Pin Number | Function     |
+---------------------------+------------------------------+--------------+
| VIO                       | VBUS                         | Bus voltage  |
+---------------------------+------------------------------+--------------+
| GND                       | GND                          | Ground       |
+---------------------------+------------------------------+--------------+
| TX                        | GP1 (Pico RX)                | Pico UART RX |
+---------------------------+------------------------------+--------------+
| RX                        | GP0 (Pico Tx)                | Pico UART TX |
+---------------------------+------------------------------+--------------+

**Build Command**

.. code-block:: bash

        # to delete current build
        make reset
        # to build the project
        make PLATFORM=pico
        # to flash the code
        make run
        # to debug the code
        make debug