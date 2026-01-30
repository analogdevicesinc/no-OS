AD5592R/AD5593R/LM75 no-OS Example Project
=======================================================

.. contents::
   :depth: 3
   :local:


Overview
--------

The Analog Devices Active Learning Module - Low Speed Mixed Signal Playground
or ADALM-LSMSPG is a board containing two ADC-DAC chips and a temperature 
sensor. 

Both the AD5592R and AD5593R are 8-channel, 12-bit configurable ADC/DAC IC with
the primary difference being that one (AD5592R) uses an SPI for communications
and the other (AD5593R) uses IIC. 

Additionally, the ADALM-LSMSPG board also utilizes an LM75 temperature sensor
with a 9-bit Delta-Sigma ADC.


Applications
------------

Control and Monitoring
General purpose analog and digital inputs/outputs


Hardware Specifications
-----------------------

The ADALM-LSMSPG board is powered entirely by the 3v3 supply coming from the 
connected carrier board.

Pin Description
^^^^^^^^^^^^^^^

Please see the following table for the pin assignments for the interface 

+------------------+------------------+-----------+
| ADALM-LSMSPG Pin | Max32666Fthr Pin |   Target  |
+==================+==================+===========+
|          SPI.SCK |      P0_27       |           |
+------------------+------------------+           |
|         SPI.MOSI |      P0_25       |  AD5592R  |
+------------------+------------------+           |
|         SPI.MISO |      P0_26       |           |
+------------------+------------------+           |
|           SPI.SS |      P0_2        |           |
+------------------+------------------+-----------+
|          I2C.SCL |      P0_6        |  AD5593R  |
+------------------+------------------+    LM75   |
|          I2C.SDA |      P0_7        |           |
+------------------+------------------+-----------+
|    Heartbeat LED |      P0_21       |    LED    |   
+------------------+------------------+-----------+


No-OS Build Setup
-----------------

#. cd projects/adalm-lsmspg
#. make PLATFORM=maxim TARGET=max32665

For more information, please see https://wiki.analog.com/resources/no-os/build


No-OS Supported Examples
-------------------------

The initialization data used in the example is taken out from:
**Project Common Data Path**.

The macros used in Common Data Path are defined in platform specific files
found in **Project Platform Configuration Path**

IIO Example
^^^^^^^^^^^

The IIO example initializes and configures AD5592R, AD5593R and LM75 using the 
Industrial I/O (IIO) framework. It showcases the setup of the mentioned devices
via their corresponding drivers to reach operational mode. Because it is IIO 
enabled, real-time data acquisition and processing of ADC data is efficiently 
transferred from device to host client application.

Maxim Platform
^^^^^^^^^^^^^^

**Demo Hardware**

- ADALM-LSMSPG board
- `Max32666FTHR <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/max32666fthr.html>`_

Build Setup
^^^^^^^^^^^

.. code-block:: bash
   # go to the project directory
   $ cd <no_os_directory>/projects/adalm-lsmspg
   # to clean the build directory
   $ make TARGET=max32665 PLATFORM=maxim reset
   # to build the code
   $ make TARGET=max32665 PLATFORM=maxim
   # to flash the adalm-lsmspg binary
   $ make TARGET=max32665 PLATFORM=maxim run


Connection Setup
^^^^^^^^^^^^^^^^

#. Use a USB Type A to mini-B cable to Connect the Max32665 FTHR board to the 
   feather connector header of the adalm-lsmspg board.
#. The adalm-lsmspg board will be powered up and the red HEARTBEAT LED should
   start blinking a pre-defined pattern similar to a plane's beacon.
#. A serial device should appear in the device manager of the operating system.
#. You can use the various IIO software applications like IIO-Oscilloscope to
   connect and communicate to the ADALM-LSMSPG board.
