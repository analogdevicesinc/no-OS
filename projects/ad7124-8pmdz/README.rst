AD7124-8-PMDZ no-OS Example Project
===================================

.. contents::
    :depth: 3

Supported Evaluation Board
--------------------------

- :adi:`EVAL-AD7124-8-PMDZ`

Overview
--------

The EVAL-AD7124-8-PMDZ is a minimalist 8-channel, low noise, low power,
24-Bit, sigma-delta ADC with PGA and reference, SPI PMOD board for the
AD7124-8. This module is designed as a low-cost alternative to the fully
featured AD7124-8 evaluation board and has no extra signal conditioning
for the ADC. All pins of the AD7124-8 are exposed, which makes the
EVAL-AD7124-8-PMDZ very flexible and easy to use. The AD7124-8 requires
two separate power supplies: an analog domain supply (AVDD) ranging from
2.9V to 3.6V in full power mode or from 2.7V to 3.6V in low/mid
power mode, referenced to AVSS, and a digital domain supply (IOVDD)
ranging from 1.65V to 3.6V, referenced to DGND.

Applications
------------

- Temperature measurement
- Pressure measurement
- Industrial process control
- Instrumentation
- Smart transmitters

Hardware Specification
----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The AD7124-8 requires two separate power supplies: AVDD for the analog
domain (2.9V to 3.6V in full power mode, 2.7V to 3.6V in mid/low
power mode, referenced to AVSS) and IOVDD for the digital domain (1.65V
to 3.6V, referenced to DGND). For best performance, decouple each
supply pin with a 1 μF tantalum and a 0.1 μF ceramic capacitor placed
close to the device, and use 0.1 μF capacitors on the REGCAPA and
REGCAPD LDO pins. Typical current draw ranges from ~255 μA in low power
mode to ~930 μA in full power mode. Proper grounding and PCB layout are
essential to minimize noise and ensure accuracy.

Digital Communication Pins
~~~~~~~~~~~~~~~~~~~~~~~~~~

The following table outlines the digital communication pins for the
AD7124-8, detailing their configuration and functionality essential for
interfacing with the board:

+-----------------------+-----------------------+-----------------------+
| Pin No.               | Pin Mnemonic          | Description           |
+-----------------------+-----------------------+-----------------------+
| 28                    | DOUT/RDY              | Serial Data           |
|                       |                       | Output/Data Ready     |
|                       |                       | Output. Indicates the |
|                       |                       | completion of a       |
|                       |                       | conversion and        |
|                       |                       | outputs data of the   |
|                       |                       | ADC.                  |
+-----------------------+-----------------------+-----------------------+
| 29                    | DIN                   | Serial Data Input for |
|                       |                       | SPI communication.    |
+-----------------------+-----------------------+-----------------------+
| 30                    | SCLK                  | Serial Clock Input    |
|                       |                       | used to synchronize   |
|                       |                       | data transfers.       |
+-----------------------+-----------------------+-----------------------+
| 31                    | CLK                   | Internal clock of the |
|                       |                       | ADC, can be sourced   |
|                       |                       | externally if         |
|                       |                       | required.             |
+-----------------------+-----------------------+-----------------------+
| 32                    | CS                    | Chip Select Pin;      |
|                       |                       | selects the device    |
|                       |                       | for SPI               |
|                       |                       | communication.        |
+-----------------------+-----------------------+-----------------------+
| -                     | EP                    | Exposed Pad; should   |
|                       |                       | be connected to the   |
|                       |                       | analog ground.        |
+-----------------------+-----------------------+-----------------------+

No-OS Build Setup
-----------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
------------------------

IIO Example
~~~~~~~~~~~

The ``iio_ad7124.c`` file serves as an interface between the AD7124
analog-to-digital converter (ADC) and the Industrial I/O (IIO)
subsystem. It provides the necessary functions and structures for
setting up and acquiring data from the AD7124 ADC. Key functionalities
include reading and writing channel attributes such as filter cutoff
frequencies, offsets, raw data, output data rates, and scales. It
communicates with the device registers to ensure precise configuration
and accurate data capture.

Further interaction with the ADC is facilitated through the
``iio_device`` structure defined as ``iio_ad7124_device``. This
structure includes function pointers to enable or disable channels and
read data samples, effectively integrating the ADC’s capabilities within
the IIO subsystem. The overall modular design of the code ensures each
function addresses specific tasks related to the accurate conversion and
handling of analog signals in complex systems.

The initialization data used in the example is taken out from the
:git-no-OS:`Project Source Data Path </projects/ad7124-8pmdz>`.

This project serves as a direct, ready-to-build example for using the
AD7124-8 with the No-OS framework and the IIO subsystem.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`__

In order to build the IIO project, make sure you have the following
configuration in the :git-no-OS:`Makefile: </projects/ad7124-8pmdz>`:

.. code-block:: bash

   # Configure the example you want to enable by setting 'y' for enable and 'n' for disable
   IIO_EXAMPLE = y

No-OS Supported Platform:
-------------------------

Xilinx
~~~~~~

Hardware Used
^^^^^^^^^^^^^

- EVAL-AD7124-8PMDZ
- Xilinx Zynq/ZCU102 Board

Connections
^^^^^^^^^^^

+-----------------------+-----------------------+-----------------------+
| Signal                | EVAL-AD7124-8-PMDZ    | Zynq/ZCU102 Board Pin |
|                       | Pin Number            | Number                |
+-----------------------+-----------------------+-----------------------+
| CS                    | 32                    | J12                   |
+-----------------------+-----------------------+-----------------------+
| MOSI (DIN)            | 29                    | K12                   |
+-----------------------+-----------------------+-----------------------+
| MISO (DOUT/RDY)       | 28                    | L12                   |
+-----------------------+-----------------------+-----------------------+
| CLK (SCLK)            | 30                    | M12                   |
+-----------------------+-----------------------+-----------------------+
| GND                   | EP                    | Any GND Pin           |
+-----------------------+-----------------------+-----------------------+
| AVDD                  | 26                    |                       |
+-----------------------+-----------------------+-----------------------+

Typical digital communication is via the SPI lines. The UART interface
is handled internally on the FPGA/host connection. Ensure your FPGA
platform’s voltage levels match those required by the AD7124-8
(typically 3.3V I/O).

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   cp <SOME_PATH>/system_top.xsa .
   # to delete current build
   make reset PLATFORM=xilinx
   # to build the IIO project
   make EXAMPLE=iio PLATFORM=xilinx
   # to flash the code
   make run

ADuCM3029
---------

Hardware Used
^^^^^^^^^^^^^

- EVAL-ADICUP3029
- EVAL-AD7124-8-PMDZ

Connections
^^^^^^^^^^^

The EVAL-ADICUP3029 Eval Board does not feature an FMC connector. This
requires manual wiring of signals to the EVAL-AD7124-8-PMDZ.

- Manually wire the SPI signals between the boards using jumper wires or a
  custom breakout board.

+-----------------+-----------------+-----------------+-----------------+
| Pin No.         | Pin Mnemonic    | Description     | EVAL-ADICUP3029 |
|                 |                 |                 | Pin             |
+-----------------+-----------------+-----------------+-----------------+
| 28              | DOUT/RDY        | Serial Data     | Connect to GPIO |
|                 |                 | Output/Data     | (e.g., P1.4)    |
|                 |                 | Ready Output.   |                 |
|                 |                 | Indicates       |                 |
|                 |                 | conversion      |                 |
|                 |                 | completion.     |                 |
+-----------------+-----------------+-----------------+-----------------+
| 29              | DIN             | Serial Data     | Connect to MISO |
|                 |                 | Input for SPI   | (e.g., P1.5)    |
|                 |                 | communication.  |                 |
+-----------------+-----------------+-----------------+-----------------+
| 30              | SCLK            | Serial Clock    | Connect to SCLK |
|                 |                 | Input used to   | (e.g., P1.6)    |
|                 |                 | synchronize     |                 |
|                 |                 | data transfers. |                 |
+-----------------+-----------------+-----------------+-----------------+
| 31              | CLK             | Internal clock  | Leave           |
|                 |                 | of the ADC, can | unconnected     |
|                 |                 | be sourced      | unless external |
|                 |                 | externally if   | clock is needed |
|                 |                 | required.       |                 |
+-----------------+-----------------+-----------------+-----------------+
| 32              | CS              | Chip Select     | Connect to CS   |
|                 |                 | Pin; selects    | (e.g., P1.3)    |
|                 |                 | the device for  |                 |
|                 |                 | SPI             |                 |
|                 |                 | communication.  |                 |
+-----------------+-----------------+-----------------+-----------------+
| EP              | Exposed Pad     | Connect to      | Connect to AGND |
|                 |                 | analog ground.  |                 |
+-----------------+-----------------+-----------------+-----------------+

- Connect a micro-USB cable to the P10 connector of the EVAL-ADICUP3029
  and connect the micro-USB cable to a PC.


Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   cd projects/ad7124-8pmdz

   export PLATFORM=aducm3029
   # to build the project
   make
   # to flash the code
   make run
   # to debug the code
   make debug
