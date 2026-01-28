EVAL-ADXL367Z no-OS Example Project
===================================

.. contents::
    :depth: 3

Supported Evaluation Boards
---------------------------

- :adi:`EVAL-ADXL367Z`

Overview
--------

The EVAL-ADXL367Z is a compact breakout board designed to simplify the
integration of the ADXL367 into existing systems. The ADXL367 is an
ultra-low-power, 3-axis digital-output accelerometer that combines
industry-leading energy efficiency with advanced built-in features,
making it suitable for a broad range of applications. Thanks to its
small footprint, the EVAL-ADXL367Z has minimal impact on both system
performance and accelerometer functionality, making it an excellent
choice for evaluating the ADXL367 in real-world setups.

Applications
------------

- 24/7 sensing
- Hearing aids
- Vital signs monitoring devices
- Motion-enabled power save switches
- Motion-enabled metering devices
- Smart watch with single-cell operation
- Smart home

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-ADXL367Z operates within a voltage range of 1.1V to 3.6V and
is optimized for low-power applications such as smartwatches and
motion-activated devices. Powered by the ADXL367 accelerometer, it
features several power-saving modes. At a supply voltage of 2.0V, with
an output data rate of 100 Hz, it consumes 0.89 µA. In motion-activated
wake-up mode, power consumption drops to 180 nA, and in standby mode, it
uses just 40 nA. The board includes a FIFO buffer and an I2C interface
to further minimize power usage by limiting processor intervention,
making it well-suited for battery-operated devices.

Connections
~~~~~~~~~~~

========== ================================ =========
Pin Number Pin Function                     Mnemonic
1          Serial Clock                     SCLK
2          Main Out Subordinate In          MOSI/SDA
3          Main In Subordinate Out          MISO/ASEL
4          Chip Select                      CSN/SCL
5          Interrupt 1                      INT1
6          Interrupt 2                      INT2
7          Not Connected                    T1
8          ADC In                           T2
9          Digital Ground                   T3
10         Supply Voltage                   VS
11         Digital Ground                   DGND
12         Digital Interface Supply Voltage VDDIO
========== ================================ =========

No-OS Build Setup
-----------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from: 
:git-no-OS:`Project Common Data Path </projects/eval-adxl367z/src/common>`

The macros used in Common Data are defined in platform specific files
found in: 
:git-no-OS:`Project Platform Configuration Path </projects/eval-adxl367z/src/platform>`

Basic example
~~~~~~~~~~~~~

The dummy example code for the ADXL367 in the ``eval-adxl367z`` project
initializes the ADXL367 accelerometer and conducts a self-test. It
enables temperature reading, sets the output data rate to 200 Hz,
configures the FIFO for data collection, and switches the device to
measurement mode. The code reads acceleration data along the x, y, and z
axes and temperature data from the buffer, which is then printed to the
console. This operation utilizes functions like ``adxl367_init``,
``adxl367_self_test``, ``adxl367_temp_read_en``,
``adxl367_set_output_rate``, ``adxl367_fifo_setup``, and
``adxl367_read_converted_fifo`` to effectively manage and operate the
device.

In order to build the basic example, make sure you have the following
configuration in the Makefile:

.. code-block:: bash

   DUMMY_EXAMPLE = y

No-OS Supported Platforms
-------------------------

Xilinx
~~~~~~

Hardware Used
^^^^^^^^^^^^^

- EVAL-ADXL367Z
- ZedBoard

Connections
^^^^^^^^^^^

=========== ================== =================================
ADXL367 Pin ZedBoard Connector Function
VDD         3V3                Power Supply
GND         GND                Ground
SCLK        MIO0               SPI Serial Clock
MISO        MIO1               SPI Main Input Subordinate Output
MOSI        MIO2               SPI Main Output Subordinate Input
CS          MIO3               Chip Select (Active Low)
=========== ================== =================================

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
