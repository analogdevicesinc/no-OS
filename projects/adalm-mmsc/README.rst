AD4080 no-OS Example Project
==============================

.. contents::
        :depth: 3

Supported Evaluation Boards
---------------------------

* `EVAL-AD4080ARDZ <https://www.analog.com/en/products/eval-ad4080ardz>`_

Overview
--------

The AD4080 is a 20-bit, high-speed, low-noise, low-distortion SAR ADC designed 
for precision and wide bandwidth data acquisition.It maintains high performance
(SINAD > 90 dBFS) even at signal frequencies above 1 MHz. It supports
oversampling and includes integrated digital filtering and decimation, which
simplifies anti-aliasing filter design and allows for reduced noise and lower
output data rates in applications where low latency is not critical.

Applications
------------

* Digital imaging
* Cell analysis
* Spectroscopy
* Automated test equipment
* High speed data acquisition
* Digital control loops, hardware in the loop
* Power quality analysis
* Source measurement units
* Electron and x-ray microscopy
* Radar level measurement
* Nondestructive test
* Predictive maintenance and structural health

Hardware Specifications
-----------------------

Power Supply Requirements
^^^^^^^^^^^^^^^^^^^^^^^^^

The EVAL-AD4080ARDZ evaluation device must be supplied using the in-package
power supply and 3.3V external supply.

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from **Project Common Data Path**.

The macros used in Common Data are defined in platform specific files found in **Project Platform Configuration Path**.

IIO example
^^^^^^^^^^^^^^^^^^

This example initializes and configures the AD4080 ADC device using the
Industrial I/O (IIO) framework, sets up GPIO pins for clock control, allocates a
buffer for ADC data, and runs an IIO application to facilitate data acquisition
and interaction with the device via UART, while handling errors and cleaning up
resources as needed.

To build the basic example, ensure you have the following configuration 
in the **Makefile**:

.. code-block:: bash

	# Select the example you want to enable by choosing y for enabling and n for disabling
	IIOD = y

No-OS Supported Platforms
-------------------------

Maxim Platform
^^^^^^^^^^^^^^

**Used hardware**

* `AD4080 <https://www.analog.com/en/products/ad4080>`_
* `MAX32666FTHR <https://www.analog.com/en/design-center/evaluation-hardware-and-software/evaluation-boards-kits/max32666fthr.html>`_

+----------------------+----------------------+-----------------------------+
| EVAL-AD4080ARDZ Pins | Function             | MAX32666FTHR Pins           |
+======================+======================+=============================+
| SCLK                 | Serial Clock Line    | SCK                         |
+----------------------+----------------------+-----------------------------+
| MISO                 | Master In Slave Out  | MISO                        |
+----------------------+----------------------+-----------------------------+
| MOSI                 | Master Out Slave In  | MOSI                        |
+----------------------+----------------------+-----------------------------+
| CS                   | Config Chip Select   | A5                          |
+----------------------+----------------------+-----------------------------+
| ~9                   | Serial Data Line     | RX                          |
+----------------------+----------------------+-----------------------------+
| 8                    | Data Chip Select     | A4                          |
+----------------------+----------------------+-----------------------------+
| GP3                  | Side Band Signal 3   | IO2                         |
+----------------------+----------------------+-----------------------------+
| OSC_EN_40M           | 40MHz Clock          | 3V3                         |
+----------------------+----------------------+-----------------------------+
| IOREF                | Reference Voltage    | 3V3                         |
+----------------------+----------------------+-----------------------------+
| 3V3                  | Supply Voltage       | 3V3                         |
+----------------------+----------------------+-----------------------------+
| GND                  | GND                  | GND                         |
+----------------------+----------------------+-----------------------------+

**Build Command**

.. code-block:: bash

        # to delete current build
        make reset
        # to build the project
        make PLATFORM=maxim TARGET=max32665
        # to flash the code
        make run
