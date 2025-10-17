AD738x_FMCZ no-OS Example Project
=================================

.. contents::
    :depth: 3

Supported Evaluation Boards
---------------------------

- :adi:`EVAL-AD7380FMCZ`
- :adi:`EVAL-AD7381FMCZ`

Overview
--------

The EVAL-AD7380FMCZ or EVAL-AD7381FMCZ are evaluation boards designed
for testing the dual simultaneous sampling AD7380 and AD7381 ADCs,
designed for high-speed data acquisition. The board supports various
applications by providing dedicated power and signal connections for
optimal ADC performance. Through integrated software, users can
visualize waveforms and analyze data, facilitating engineering
development and testing. The board is equipped to handle complex
operations, supporting engineers in effectively evaluating ADC
performance in different scenarios.

Applications
------------

- Motor control feedback
- Motor control current sense
- Sonar
- Power quality
- Data acquisition systems
- Erbium doped fiber amplifier (EDFA) applications
- Quadrature modulation (I and Q demodulation)

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-AD738xFMCZ evaluation board requires a 12V ±10% main power
supply for its internal voltage regulators. Additionally, it features an
ADC analog power supply (VCC), a digital serial peripheral interface
power supply (VLOGIC), and amplifier power supplies (AMP_PWR+ and
AMP_PWR−). Power connection points include P4 for the main board power
supply, P5 for the ADC and SPI interface, and P6 for the amplifier,
ensuring efficient power distribution to support high-speed data
acquisition applications.

On-board Connectors
~~~~~~~~~~~~~~~~~~~

+-----------------------------------+-----------------------------------+
| **Connector**                     | **Function**                      |
+-----------------------------------+-----------------------------------+
| J1 to J4                          | Analog Inputs                     |
+-----------------------------------+-----------------------------------+
| P1                                | Amplifier mezzanine card input    |
+-----------------------------------+-----------------------------------+
| P2                                | Amplifier mezzanine card output   |
+-----------------------------------+-----------------------------------+
| P3                                | Digital SPI signals               |
+-----------------------------------+-----------------------------------+
| P4                                | Main power supply for internal    |
|                                   | regulators                        |
+-----------------------------------+-----------------------------------+
| P5                                | ADC and digital SPI power supply  |
+-----------------------------------+-----------------------------------+
| P6                                | Amplifier power supply            |
+-----------------------------------+-----------------------------------+
| P7                                | FPGA mezzanine card (FMC)         |
|                                   | connection to low pin count (LPC) |
|                                   | connector                         |
+-----------------------------------+-----------------------------------+
| EXT_REF                           | External voltage reference        |
+-----------------------------------+-----------------------------------+

Link Connectors
~~~~~~~~~~~~~~~

+-----------------+-----------------+-----------------+-----------------+
| **Link Name**   | **Function**    | **Position**    | **Description** |
+-----------------+-----------------+-----------------+-----------------+
| LK1             | AMP_PWR–        | 1               | Use internal    |
|                 |                 |                 | –2.5V from U9   |
|                 |                 |                 | for AMP_PWR–    |
+-----------------+-----------------+-----------------+-----------------+
| LK2             | AMP_PWR+        | 1               | Use internal 5V |
|                 |                 |                 | from U8 for     |
|                 |                 |                 | AMP_PWR+        |
+-----------------+-----------------+-----------------+-----------------+
| LK3             | Ext 12V         | 1               | Use 12V power   |
|                 |                 |                 | supply from SDP |
+-----------------+-----------------+-----------------+-----------------+
| LK4             | VREF            | 3               | Use internal    |
|                 |                 |                 | +3.3V from U3   |
|                 |                 |                 | for VREF        |
+-----------------+-----------------+-----------------+-----------------+
| LK5             | VLOGIC          | 3               | Use internal    |
|                 |                 |                 | 2.3V from U6    |
|                 |                 |                 | for VLOGIC      |
+-----------------+-----------------+-----------------+-----------------+
| JP1             | AINA–           | 1 SMD RES       | Connect         |
|                 |                 |                 | external SMB    |
|                 |                 |                 | Connector J1 to |
|                 |                 |                 | A1 buffer       |
|                 |                 |                 | amplifier       |
+-----------------+-----------------+-----------------+-----------------+
| JP2             | AINA–           | 1 SMD RES       | Connect         |
|                 |                 |                 | internal signal |
|                 |                 |                 | from A2 to ADC  |
|                 |                 |                 | U10 input AINA– |
+-----------------+-----------------+-----------------+-----------------+
| JP3             | AINA+           | 1 SMD RES       | Connect         |
|                 |                 |                 | internal signal |
|                 |                 |                 | from A2 to ADC  |
|                 |                 |                 | U10 input AINA+ |
+-----------------+-----------------+-----------------+-----------------+
| JP4             | REFIO           | 3 SMD RES       | REFIO pin is    |
|                 |                 |                 | driven with     |
|                 |                 |                 | external        |
|                 |                 |                 | onboard         |
|                 |                 |                 | reference       |
+-----------------+-----------------+-----------------+-----------------+
| JP5             | VCC             | 1               | Use internal    |
|                 |                 |                 | +3.3V from U2   |
|                 |                 |                 | for VCC         |
+-----------------+-----------------+-----------------+-----------------+
| JP6             | AINA+           | 1 SMD RES       | Connect         |
|                 |                 |                 | external SMB    |
|                 |                 |                 | Connector J2 to |
|                 |                 |                 | A1 buffer       |
|                 |                 |                 | amplifier       |
+-----------------+-----------------+-----------------+-----------------+

No-OS Build Setup
-----------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad738x_fmcz/src/common>`__

The macros used in Common Data are defined in platform specific files
found in the 
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad738x_fmcz/src/platform>`__

Basic example
~~~~~~~~~~~~~

The basic example for the AD738x no-OS setup showcases the
initialization and data retrieval features of the AD738x ADC driver. The
initialization involves configuring SPI, UART, and PWM crucial for
device interaction. The ``ad738x_init`` function sets up the ADC with
specified conversion modes and reference selections while preparing
hardware components like SPI and PWM. Post-initialization, the ADC reads
data in a loop, using the ``ad738x_read_data`` function to acquire 400
samples continuously. These samples, processed through bit manipulation,
yield ADC channel values representing analog-to-digital conversions,
concluding with printed verification to demonstrate the AD738x’s rapid
data handling in a no-OS environment.

In order to build the basic example, make sure you have the following
configuration in the Makefile:

.. code-block:: bash

   # Select the example you want to enable by choosing y for enabling and n for disabling
   BASIC_EXAMPLE = y
   IIO_EXAMPLE = n

IIO example
~~~~~~~~~~~

The IIO example code initializes the IIO application to facilitate
real-time data acquisition and control of the AD738x ADC. The setup
includes configuring the IIO device using the ``ad738x_iio_init``
function to integrate the ADC driver. This process involves reading ADC
data into a buffer and exposing device attributes such as raw data and
scaling through the IIO client interface. Channels are registered, scan
types are defined for determining data resolution, and data is
continuously managed for client applications. Users can utilize the IIO
Oscilloscope to monitor and visualize waveforms, ensuring effective ADC
performance evaluation.

The IIO demo is a standard example, provided in most `no-OS
projects <https://github.com/analogdevicesinc/no-OS/tree/main/projects>`__,
that launches an IIOD server on the board so that the user may connect
to it via an IIO client. Using the IIO Oscilloscope application, users
can access device settings such as channels, scan types, and data
resolution.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`__

If you are not familiar with ADI IIO Oscilloscope Client, please take a
look at: 
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`__

To build the IIOD demo, add the following flag when invoking make. This
will build the IIOD server and the IIO section of the driver:

.. code-block:: bash

   # Configure the example you want to enable by setting 'y' for enable and 'n' for disable
   BASIC_EXAMPLE = n
   IIO_EXAMPLE = y

No-OS Supported Platforms
-------------------------

Xilinx 
~~~~~~

Hardware Used
^^^^^^^^^^^^^

- :adi:`EVAL-AD7380FMCZ` or :adi:`EVAL-AD7381FMCZ`
- ZedBoard

Connections
^^^^^^^^^^^

The EVAL-AD7380FMCZ has a standard FMC connector, which can be directly
plugged into the FMC slot on the Zedboard.

+-----------------+-----------------------------------+-----------------+
| **Signal**      | **EVAL-AD7380FMCZ** FMC-LPC       | **ZedBoard**    |
|                 | Connector (P7)                    | FMC-LPC         |
|                 |                                   | Connector       |
+-----------------+-----------------------------------+-----------------+
| **MOSI**        | SPI MOSI (Digital Pin)            | SPI MOSI        |
+-----------------+-----------------------------------+-----------------+
| **MISO**        | SPI MISO (Digital Pin)            | SPI MISO        |
+-----------------+-----------------------------------+-----------------+
| **SCK**         | SPI SCK (Digital Pin)             | SPI SCK         |
+-----------------+-----------------------------------+-----------------+
| **CS**          | SPI CS (Digital Pin)              | SPI CS          |
+-----------------+-----------------------------------+-----------------+

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   cp <SOME_PATH>/system_top.xsa .
   # to delete current build
   make reset
   # to build the project
   make PLATFORM=xilinx
   # to flash the code
   make run

STM32 Platform
~~~~~~~~~~~~~~

Hardware Used
^^^^^^^^^^^^^

- EVAL-AD7380FMCZ or EVAL-AD7381FMCZ
- EVAL-SDP-CK1Z (SDP-K1)

Connections
^^^^^^^^^^^

To connect the EVAL-AD7380FMCZ to the EVAL-SDP-CK1Z board, align their
160-pin connectors and press them together firmly; then connect the SDP
board to PC via USB.

========== ====================== =============
Signal     EVAL-AD7380FMCZ        EVAL-SDP-CK1Z
MOSI       SPI MOSI (Digital Pin) SPI MOSI
MISO       SPI MISO (Digital Pin) SPI MISO
SCK        SPI SCK (Digital Pin)  SPI SCK
CS         SPI CS (Digital Pin)   SPI CS
========== ====================== =============

Build Command
^^^^^^^^^^^^^^

.. code-block:: bash

   # to delete current build
   make reset
   # to build the project
   make PLATFORM=stm32
   # to flash the code
   make run
   # to debug the code
   make debug
