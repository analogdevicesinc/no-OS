AD413x no-OS Example Project
============================

.. contents::
    :depth: 3

Supported Evaluation Boards
---------------------------

- :adi:`EVAL-AD4130-8WARDZ`

Overview
--------

The EVAL-AD4130-8WARDZ is an evaluation board that allows users to
evaluate the AD4130-8, an ultra low power, high precision ADC. The fully
integrated analog front end (AFE) includes a multiplexer for up to 16
single-ended or eight differential inputs, programmable gain amplifier
(PGA), 24-bit sigma-delta (Σ-Δ) ADC. It features programmable gain
amplifiers and multiplexers for selecting input channels, supporting
digital communication over an SPI interface.

Applications
------------

- Smart transmitters
- Wireless battery and harvester powered sensor nodes
- Portable instrumentation
- Temperature measurement: thermocouple, RTD, thermistors
- Pressure measurement: bridge transducers
- Healthcare and wearables

Hardware Specifications
------------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-AD4130-8 evaluation board operates with a 5V USB power supply
and provides flexibility in power configuration. It features a default
3.3V regulator for AVDD and an optional 1.8V regulator. External
AVDD/AVSS connections can be made via Connector J7. The board is
equipped to facilitate both AVDD and IOVDD selections with integrated
regulators, enhancing its utility in ADC performance testing and data
acquisition tasks.

============ =========================
J7 Connector Function
============ =========================
1            External IOVDD connection
2            External GND connection
3            External AVSS connection
4            External AVDD connection
============ =========================

On-board Connectors
~~~~~~~~~~~~~~~~~~~

+-----------------------------------+-----------------------------------+
| Connector                         | Description                       |
+-----------------------------------+-----------------------------------+
| Connector J10 (Wire Bridge)       | Handles analog input for 4/6 Wire |
|                                   | bridge setups, with specific pin  |
|                                   | functions such as Excitation,     |
|                                   | External Reference, and Ground    |
|                                   | connections.                      |
+-----------------------------------+-----------------------------------+
| Connector J12 (DC Input)          | Provides DC connections for AIN8  |
|                                   | to AIN15, facilitating analog     |
|                                   | input without DC filtering.       |
+-----------------------------------+-----------------------------------+

Digital Communication Pins
~~~~~~~~~~~~~~~~~~~~~~~~~~

================= ========= ========== ========
PMOD Connector P7 Function  Connection Function
1                 CS_N      7          SYNC_N
2                 MOSI/DIN  8          INT
3                 MISO/DOUT 9          N/C
4                 SCLK      10         N/C
5                 GND       11         GND
6                 V_USB     12         V_USB
================= ========= ========== ========

No-OS Build Setup
-----------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
-------------------------

The AD413x no-OS project supports two primary example types: BASIC and
IIO. The BASIC example focuses on initializing the AD413x, demonstrating
continuous conversion, SPI data reading, and data display. Conversely,
the IIO example sets up a device and application for more intricate
operations, initializing UART communication and IIO buffers.

The initialization data used in the examples is taken out from the
`Project Data Source Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad413x/src/app>`__

Basic example
~~~~~~~~~~~~~

The basic example code for the AD413x device initializes SPI
communication in four-wire mode, configures GPIO for conversion
readiness, and applies predefined presets for channel setup. These
presets specify input sources, gain settings, filters, and reference
settings. A soft reset is performed to prepare the device for operation,
and key parameters such as master clock mode and standby controls are
set. Once initialized, ADC operation modes, either single or continuous,
are managed to facilitate data acquisition. This configuration enables
ready interaction with the AD413x AFE using the no-OS driver setup,
allowing effective data collection and management.

In order to build the basic example, make sure you have the following
configuration in the 
`Makefile <https://github.com/analogdevicesinc/no-OS/blob/main/projects/ad413x/Makefile>`__

.. code-block:: bash

   # Select the example you want to enable by choosing y for enabling and n for disabling
   BASIC_EXAMPLE = y
   IIOD = n

IIO example
~~~~~~~~~~~

The IIO example code for the AD413x ADC series demonstrates integration
with the Industrial I/O framework for streamlined data acquisition.
Initialization is handled by ``ad413x_iio_init``, which configures the
ADC per given parameters and allocates resources. Channel management is
dynamic, utilizing ``ad413x_iio_close_channels`` and
``ad413x_iio_update_active_channels`` to modify channel states. Sample
data is retrieved using ``ad413x_iio_read_samples``, applying continuous
conversion techniques. The code enables IIO attribute setup for scale
and raw data properties, ensuring seamless integration and management of
AD413x devices within IIO applications.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`__

In order to build the IIO project, make sure you have the following
configuration in the Makefile:

.. code-block:: bash

   # Configure the example you want to enable by setting 'y' for enable and 'n' for disable
   BASIC_EXAMPLE = n
   IIOD = y

No-OS Supported Platforms
-------------------------

Xilinx
~~~~~~

Hardware Used
^^^^^^^^^^^^^

- EVAL-AD4130-8WARDZ
- ZedBoard

Connections
^^^^^^^^^^^

- Power the EVAL-AD4130-8 board with a 5V USB supply.

- Use on-board regulators to set up AVDD (default 3.3V) and IOVDD.

- Connect SPI lines of the EVAL-AD4130-8 board to the ZedBoard’s
  appropriate GPIO/PMOD headers.

- Ensure a common ground is established between EVAL-AD4130-8 and
  ZedBoard.

+-----------------+-----------------+-----------------+-----------------+
| **EVAL-AD4130-8 | **Function**    | **ZedBoard PMOD | **Function**    |
| PMOD            |                 | Header**        |                 |
| Connector**     |                 |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| P7-1            | CS_N            | Pmod Header Pin | Chip Select     |
|                 |                 | 1               | (Active Low)    |
+-----------------+-----------------+-----------------+-----------------+
| P7-2            | MOSI/DIN        | Pmod Header Pin | Master Out      |
|                 |                 | 2               | Slave In / Data |
|                 |                 |                 | Input           |
+-----------------+-----------------+-----------------+-----------------+
| P7-3            | MISO/DOUT       | Pmod Header Pin | Master In Slave |
|                 |                 | 3               | Out / Data      |
|                 |                 |                 | Output          |
+-----------------+-----------------+-----------------+-----------------+
| P7-4            | SCLK            | Pmod Header Pin | Serial Clock    |
|                 |                 | 4               |                 |
+-----------------+-----------------+-----------------+-----------------+
| P7-5            | GND             | Pmod Header Pin | Ground          |
|                 |                 | 5               | reference for   |
|                 |                 |                 | SPI             |
|                 |                 |                 | communication   |
+-----------------+-----------------+-----------------+-----------------+
| P7-6            | V_USB           | Pmod Header Pin | Voltage supply  |
|                 |                 | 6               | for the board   |
|                 |                 |                 | and peripherals |
+-----------------+-----------------+-----------------+-----------------+

- Use a 12V supply in the barrel jack (J20) to power the ZedBoard.

- Connect the ZedBoard’s USB-UART to your PC via MicroUSB.

Build Command
^^^^^^^^^^^^^^

.. code-block:: bash

  cp <SOME_PATH>/system_top.xsa .

  # to delete current build 
  make reset 
  # to build the project 
  make 
  # to flash the code 
  make run
