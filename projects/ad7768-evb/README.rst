AD7768-EVB no-OS Example Project
================================

.. contents::
    :depth: 3

Supported Evaluation Boards
----------------------------

- :adi:`EVAL-AD7768FMCZ`

Overview
--------

The EVAL-AD7768FMCZ evaluation board allows for comprehensive
examination of the AD7768 24-bit Sigma-Delta ADC’s performance and
features. It supports eight channels of simultaneous sampling with
analog inputs adaptable via terminal blocks or SMB connectors. Signal
routing through ADA4896 amplifiers ensures flexibility across an input
voltage range of 0V to 4.096V, supported by a precise 4.096 V reference.
Configuration and operation are managed through a 4-wire SPI interface,
with options for serial data output.

Applications
------------

- Data acquisition systems: USB/PXI/Ethernet
- Instrumentation and industrial control loops
- Audio testing and measurement
- Vibration and asset condition monitoring
- 3-phase power quality analysis
- Sonar
- High precision medical electroencephalogram (EEG) or
  electromyography (EMG)/electrocardiogram (ECG)

Hardware Specifications
------------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-AD7768FMCZ evaluation board requires a flexible power supply
setup. It accepts external power via a DC plug or a bench top supply,
needing a voltage input between 7V and 9V. Additionally, it can be
powered by another microcontroller board with a 12V input, essential
for operating amplifiers and external reference. For different
operational configurations, careful positioning of jumpers on connectors
for AVDD1, AVDD2, IOVDD, AVSS, and AGND is necessary. The IOVDD voltage
for digital logic is maintained between 2.25V and 3.6V. However,
support for a 1.8V mode is available by bypassing the internal LDO,
ensuring it meets AD7768’s power requirements effectively.

No-OS Build Setup
-----------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
`Project Source Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad7768-evb/src>`__

Demo example
~~~~~~~~~~~~~

The demo example code for the EVAL-AD7768FMCZ evaluation board initializes
the AD7768 ADC by enabling instruction and data caches, then utilizes SPI
and GPIO interfaces. This setup involves allocating memory for the
device structure, establishing SPI communication, and configuring GPIO
pins. The AXI ADC core is set up for channel management, and an
AXI-based DMAC is employed for data acquisition.

In order to build the demo example, make sure you have the following
configuration in the `Makefile <https://github.com/analogdevicesinc/no-OS/blob/main/projects/ad7768-evb/Makefile>`__ :

.. code-block:: bash

   cd projects/ad7768-evb

   DEMO_EXAMPLE = y
   IIOD = n

IIO example
~~~~~~~~~~~~

The industrial I/O subsystem provides a unified framework for drivers
for many different types of converters and sensors using a number of
different physical interfaces (I2C, API, etc). The IIO example code
initializes and configures the AD7768 ADC by establishing SPI, GPIO, and
UART interfaces. The code also allows for the setup of power modes,
clock dividers, and sampling frequency through SPI communication. It
manages data acquisition using AXI DMA, which facilitates sample
collection, cyclic data transfers, and communication with DDR memory.
IIO support enables interaction with libiio clients via the serial
backend, ensuring efficient data conversion and acquisition by the
AD7768 hardware.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`__

In order to build the IIO project, make sure you have the following
configuration in the Makefile:

.. code-block:: bash

   cd projects/ad7768-evb

   DEMO_EXAMPLE = n
   IIOD = y

No-OS Supported Platforms
--------------------------

- Xilinx
- ADuCM3029

Xilinx
~~~~~~

Hardware Used
^^^^^^^^^^^^^

- EVAL-AD7768FMCZ
- Zedboard

Connections
^^^^^^^^^^^

To set up the EVAL-AD7768FMCZ evaluation board with a ZedBoard, follow
these steps:

1. Download the latest ADI Kuiper Linux image for evaluation purposes.
   Follow the instructions in this page to configure SD card.

2. Insert the configured SD card into the ``J12`` slot on the ZedBoard.

3. Connect the Evaluation Board: Attach the EVAL-AD7768FMCZ board to the
   FMC connector on the ZedBoard.

4. Connect the 12V wall adapter to the ``J20`` input on the ZedBoard.

5. Connect an Ethernet cable from the ``J11`` port on the ZedBoard to
   router or directly to PC.

6. Connect your desired input signal to the evaluation board.

7. Toggle ``SW8`` to the ON position. Verify that the green power LED
   (LD13) is illuminated.

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   cp <SOME_PATH>/system_top.xsa .

   # to delete current build
   make clean

   make PLATFORM=xilinx

   # to build the project
   make

   # to flash the code
   make run

ADuCM3029
~~~~~~~~~

Hardware Used
^^^^^^^^^^^^^

- EVAL-AD7768FMCZ
- EVAL-ADICUP3029

Connections
^^^^^^^^^^^

The ADuCM3029 Eval Board does not have an FMC connector, so you will
need to manually wire signals with the EVAL-AD7768FMCZ using jumper wires 
or a custom breakout board.

====================== ==================== ==================
Signal                 From EVAL-ADICUP3029 To EVAL-AD7768FMCZ
SPI_MOSI (SPI Data In) SPI MOSI             SDI
SPI_MISO               SPI MISO             SDO
SPI_SCLK               SPI SCLK             SCLK
SPI_CS                 GPIO                 CS
GPIO (Reset)           GPIO                 RESET
GPIO (DRDY)            GPIO (interrupt)     DRDY
GND                    GND                  GND
Power (3.3V)           3.3V                 VCC
====================== ==================== ==================

- Supply power to EVAL-AD7768FMCZ via an external power source. Connect
  a 7V to 9V DC power source to the J3 connector (P6 on the board) and
  set the jumper link LK1 to position A.

- Use separate external supply for EVAL-ADICUP3029 board (via barrel
  jack connector P2) or connect to just connect to PC for power over
  USB.

Build Command
^^^^^^^^^^^^^^

   .. code-block:: bash

      # Clean previous build artifacts
      make clean

      # Build the project with ADuCM3029-specific settings
      make PLATFORM=aducm3029

      # Flash the newly compiled firmware onto the ADuCM device
      make flash

      # Launch a debugging session for the ADuCM platform
      make debug
