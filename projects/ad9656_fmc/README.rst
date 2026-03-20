AD9656_FMC no-OS Example Project
================================

.. contents::
    :depth: 3

Supported Evaluation Boards
---------------------------

- :adi:`AD9656 Evaluation Board <AD9656EBZ>`

Overview
--------

The AD9656EBZ is an evaluation board for the AD9656, quad 16-bit ADC.
This reference design provides all of the support circuitry required to
operate the devices in their various modes and configurations.

The AD9656 is a quad, 16-bit, 125 MSPS analog-to-digital converter (ADC)
designed for high performance with ease of use. It incorporates an
on-chip sample and hold circuit, operating at up to 125 MSPS. The ADC
features a signal-to-noise ratio (SNR) of 79.9 dBFS at 16 MHz and 
78.1 dBFS at 64 MHz, and a spurious-free dynamic range (SFDR) of 86 dBc to
Nyquist. The analog input range is flexible, from 2.0Vp-p to 2.8Vp-p, 
with a low power consumption of 197 mW per channel. Compliance with
JESD204B Subclass 1 for serial digital outputs, power-down modes, and
digital test pattern generation are notable features. The AD9656 is
suitable for medical ultrasound, MRI, high-speed imaging, and portable
test equipment applications.

Applications
------------

- Medical Ultrasound and MRI
- High-Speed Imaging
- Quadrature Radio Receivers
- Diversity Radio Receivers
- Portable Test Equipment

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~~

The AD9656EBZ can be powered in one of three ways:

1. **Default – via FMC Connector:** The board can receive power from
the ZCU102 or HSC-ADC-EVALEZ capture board through the FMC connector.
For this mode, jumpers P101 and P103 must both have pin 1 tied to pin 2.
Do not connect the external 6 V wall supply when using this
configuration.

2. **External Wall Supply (6V, 2A):** A wall-mountable 6V, 2A
switching power supply can power the board by connecting to the 2.1 mm
jack at P102, with P101 and P103 having pin 2 tied to pin 3. On-board
LDO regulators then supply the appropriate voltages to each section of
the board. The AD9656 itself requires a 1.8V analog/digital core
supply.

3. **ZCU102 via FMC:** When used with the ZCU102 in the no-OS project,
the evaluation board is powered via the FMC HPC0 connector of the
ZCU102.

Connectors
~~~~~~~~~~

The AD9656EBZ includes the following on-board connectors and signal
pins:

+-----------------------------------+-----------------------------------+
| Connector/Pin                     | Description                       |
+-----------------------------------+-----------------------------------+
| P2 / FMC Connector                | High-pin-count FMC connector –    |
|                                   | carries JESD204B differential     |
|                                   | data lanes, SPI, power, and       |
|                                   | clocking signals to/from the      |
|                                   | carrier board                     |
+-----------------------------------+-----------------------------------+
| J302 / CLOCK+ (SMA)               | External single-ended clock input |
|                                   | (AC-coupled, 50Ω terminated)      |
+-----------------------------------+-----------------------------------+
| J304 / Oscillator Jumper          | Jumper to enable/disable the      |
|                                   | on-board 125 MHz crystal          |
|                                   | oscillator (Y801)                 |
+-----------------------------------+-----------------------------------+
| SMA Analog Input Connectors       | Four SMA connectors for           |
| (CHA–CHD)                         | differential analog inputs per    |
|                                   | channel                           |
+-----------------------------------+-----------------------------------+
| P101 / Power Select Jumper        | Selects power source between FMC  |
|                                   | connector and external wall       |
|                                   | supply                            |
+-----------------------------------+-----------------------------------+
| P103 / Power Select Jumper        | Selects power source between FMC  |
|                                   | connector and external wall       |
|                                   | supply                            |
+-----------------------------------+-----------------------------------+
| P102 / DC Jack                    | 2.1 mm barrel jack for 6 V        |
|                                   | external power supply input       |
+-----------------------------------+-----------------------------------+
| SPI Interface (SDIO, SCLK, CSB)   | SPI port for configuration access |
|                                   | to AD9656, AD9508, and AD9953 via |
|                                   | the carrier board                 |
+-----------------------------------+-----------------------------------+
| JESD204B Lane Outputs             | Differential lane outputs routed  |
|                                   | to FMC connector (1000 Ω          |
|                                   | differential traces)              |
+-----------------------------------+-----------------------------------+

No-OS Build Setup
-----------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
:git-no-OS:`Project SRC Path </projects/ad9656_fmc/src>`.

IIO example
~~~~~~~~~~~

The IIO example launches an IIOD server on the ZCU102 board so that the
user may connect to it via an IIO client. Using the IIO Oscilloscope
application, users can access device settings such as sampling
frequency, channel data scale, JESD204B link parameters, and other
AD9656 configuration attributes, facilitating seamless data management
and real-time streaming of the quad-channel, 16-bit ADC data over the
JESD204B interface.

If you are not familiar with ADI IIO Application, please take a look at:
`https://wiki.analog.com/resources/tools-software/no-os-software/iio`

f you are not familiar with ADI IIO Oscilloscope Client, please take a
look at: `IIO Oscilloscope 
<https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`__

To build the IIOD demo, add the following flag when invoking make. This
will build the IIOD server and the IIO section of the driver:

.. code-block:: bash

   # Configure the example you want to enable by setting 'y' for enable and 'n' for disable
   IIOD = y

No-OS Supported Platforms
-------------------------

Xilinx
~~~~~~

Hardware Used
^^^^^^^^^^^^^

- AD9656EBZ Evaluation Board
- ZCU102 Development Board

Connections
^^^^^^^^^^^

To run the no-OS project, connect the AD9656EBZ to the ZCU102 evaluation
board through the FMC HPC0 connector. The JESD204B differential output
lanes, SPI configuration bus, and power signals are all routed through
this single FMC connector.

The pin assignments between the AD9656EBZ and ZCU102 via the FMC HPC0
connector are as follows:

+-----------------+-----------------+-----------------+-----------------+
| **AD9656EBZ     | **FMC Pin**     | **ZCU102        | **Description** |
| Signal**        |                 | Signal**        |                 |
+-----------------+-----------------+-----------------+-----------------+
| JESD204B Lane   | FMC DP0 TX/RX   | GTH Transceiver | JESD204B        |
| 0+ / 0-         |                 | Lane 0          | high-speed      |
|                 |                 |                 | serial data,    |
|                 |                 |                 | Lane 0          |
+-----------------+-----------------+-----------------+-----------------+
| JESD204B Lane   | FMC DP1 TX/RX   | GTH Transceiver | JESD204B        |
| 1+ / 1-         |                 | Lane 1          | high-speed      |
|                 |                 |                 | serial data,    |
|                 |                 |                 | Lane 1          |
+-----------------+-----------------+-----------------+-----------------+
| JESD204B Lane   | FMC DP2 TX/RX   | GTH Transceiver | JESD204B        |
| 2+ / 2-         |                 | Lane 2          | high-speed      |
|                 |                 |                 | serial data,    |
|                 |                 |                 | Lane 2          |
+-----------------+-----------------+-----------------+-----------------+
| JESD204B Lane   | FMC DP3 TX/RX   | GTH Transceiver | JESD204B        |
| 3+ / 3-         |                 | Lane 3          | high-speed      |
|                 |                 |                 | serial data,    |
|                 |                 |                 | Lane 3          |
+-----------------+-----------------+-----------------+-----------------+
| REF CLK+ / REF  | FMC GBTCLK0     | GTH Reference   | 125 MHz         |
| CLK-            |                 | Clock           | reference clock |
|                 |                 |                 | for JESD204B    |
+-----------------+-----------------+-----------------+-----------------+
| SPI SDIO        | FMC LA17        | PS SPI0 MOSI    | SPI data to     |
|                 |                 |                 | AD9656          |
+-----------------+-----------------+-----------------+-----------------+
| SPI SCLK        | FMC LA16        | PS SPI0 SCLK    | SPI clock       |
+-----------------+-----------------+-----------------+-----------------+
| GND             | FMC GND         | GND             | Common ground   |
+-----------------+-----------------+-----------------+-----------------+
| VADJ (1.8 V)    | FMC VADJ        | ZCU102 VADJ     | I/O supply from |
|                 |                 |                 | ZCU102 FMC VADJ |
|                 |                 |                 | rail            |
+-----------------+-----------------+-----------------+-----------------+

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   cp <SOME_PATH>/system_top.xsa .
   # to delete current build
   make reset
   # to build the project
   make
   # to flash the code
   make run
