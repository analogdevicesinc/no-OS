Evaluating AD7091R-2/-4/-8 Devices
==================================


Contents
--------

.. contents:: Table of Contents
    :depth: 3

Supported Devices
-----------------

* `AD7091R-2 <https://www.analog.com/AD7091R-2>`_
* `AD7091R-4 <https://www.analog.com/AD7091R-4>`_
* `AD7091R-8 <https://www.analog.com/AD7091R-8>`_

Supported Evaluation Boards
---------------------------

* `EVAL-AD7091R-xSDZ <https://www.analog.com/eval-ad7091r-xsdz>`_

Overview
--------

The ad7091r-2, ad7091r-4, and AD7091r-8 are 2-/4-/8-channel, ultra low power
power dissipation, successive approximation register (SAR) analog-to-digital
converter (ADC). The AD7091R-2/-4/-8 ADCs are multichannel versions of AD7091r.
The AD7091R-2/AD7091R-4/AD7091R-8 operates from a single 2.7 V to 5.25 V power
supply.

See complementary documentation at
https://wiki.analog.com/resources/tools-software/uc-drivers/ad7091r8.

Hardware Specifications
-----------------------

Use the test points for interfacing the EVAL-AD7091R-xSDZ with the microcontroller.

The instructions below describe how to set up EVAL-AD7091R-xSDZ_ with MAX78000FTHR_.

The evaluation board default configuration is intended to work with
EVAL-SDP-CB1Z_ so it must be adapted to connect to a different platform.


**EVAL Board Jumper Links**

For the jumper links, use the following configuration:

+------+---------------+
| Link | Link Position |
+------+---------------+
| LK1  |      B        |
+------+---------------+
| LK2  |      B        |
+------+---------------+
| LK3  |      B        |
+------+---------------+
| LK8  |      A        |
+------+---------------+
| LK9  |      B        |
+------+---------------+
| LK11 |      A        |
+------+---------------+

**EVAL Board Soldering Links**

Use a soldering iron to change a few soldering links.

  * Unsolder R126 (enable drive CS through CS test point)
  * Unsolder R127 (enable drive SDI through SDI test point)
  * Unsolder R128 (enable drive CONVST through CONVST test point)
  * Unsolder SL19 (enable drive SCLK through SCLK test point)
  * Unsolder SL20 (enable drive SDO through SDO test point)
  * Power VDD and Vdrive supplied with MAX78000FTHR_ 3.3V pins.
  * Solder R68 points together (enable drive the RESET pin through RESET test point)
  * Unsolder LK13 and LK14 from position A and solder them in position C (tie MUX_OUT with ADC_IN).
  * Unsolder SL09 and SL10 from position A and solder each of them to position B to allow the VIN7 signal to bypass the input buffer.


.. _EVAL-AD7091R-xSDZ: https://www.analog.com/eval-ad7091r-xsdz
.. _MAX78000FTHR: https://www.analog.com/en/design-center/evaluation-hardware-and-software/evaluation-boards-kits/max78000fthr.html
.. _EVAL-SDP-CB1Z: https://www.analog.com/en/design-center/evaluation-hardware-and-software/evaluation-boards-kits/SDP-B.html


No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad7091r8-sdz/src/common>`_

The macros used in Common Data are defined in platform specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad7091r8-sdz/src/platform>`_

Basic example
^^^^^^^^^^^^^

This is a simple example which initializes the ad7091r-2/-4/-8 selected device
and performs analog to digital conversions in a while loop with a period of 1s.
The data is printed on the serial interface.

In order to build the basic example project, add the following configuration in
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad7091r8-sdz/Makefile>`_

.. code-block:: bash

        # Force select an example by assigning y for enabling.
        BASIC_EXAMPLE = y

Alternatively, one can set the desired example flag when calling the compiler to
make it build the project with the specified example.

.. code-block:: bash

        # Builds the project's basic example.
        make BASIC_EXAMPLE=y

IIO example
^^^^^^^^^^^

This project is actually a IIOD demo for EVAL-AD7091R-xSDZ device series.
The project launches a IIOD server on the board so that the user may connect
to it via an IIO client running in the microcontroller.
Using IIO-Oscilloscope, the user can configure the ADC and view the measured
data in the Digital Multi Meter tab (DMM).

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with ADI IIO-Oscilloscope Client, please take a look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

The No-OS IIO Application together with the No-OS IIO ad7091r8 driver take care of
all the back-end logic needed to setup the IIO server.

This example initializes the IIO device and calls the IIO app as shown in:
`IIO Example <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad7091r8-sdz/src/examples/iio_example>`_

This example supports single-shot ADC readings. At each read request, the
ad7091r8 driver writes to the channel register (address 0x01) to set the desired
channel for conversion, does a dummy conversion to allow the channel sequencer
to update, then does the another conversion to get the date for the requested
channel. Before every transfer during an ADC read procedure, the CONVST line is
pulsed either to update the channel sequencer or to trigger an actual ADC read.
The CONVST pin is not pulsed if only reading or writing to configuration
registers.

In order to build the IIO project make sure you have the following configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad7091r8-sdz/Makefile>`_

.. code-block:: bash

        # Select the example you want to enable by choosing y for enabling and n for disabling
        BASIC_EXAMPLE = n
        IIO_EXAMPLE = y

Alternatively, one can set the desired example flag when calling the compiler to
make it build the project with the specified example.

.. code-block:: bash

        # Builds the project with a simple IIO example.
        make IIO_EXAMPLE=y


IIO timer trigger example
^^^^^^^^^^^^^^^^^^^^^^^^^

This example extends the IIO example by enabling faster data capture rates
triggered by a hardware timer.

Using IIO-Oscilloscope, the user can configure the ADC and view the measured
data on a plot.

The captures happen at a predefined sample rate which is defined by the timer
configuration at build time. The timer settings can be modified to reach slower
or faster sample rates.

The initialization data used in the timer is taken out from platform parameter
files under subdirectories of
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad7091r8-sdz/src/platform>`_

The initialization of IIO device, buffer, and IIO app is done in:
`IIO Timer Trigger Example <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad7091r8-sdz/src/examples/iio_timer_trigger_example>`_

The hardware timer periodically triggers an interrupt that runs a callback
function. That callback function calls the IIO app which in turn calls the
trigger handler in the ad7091r8 driver. The ad7091r8 trigger handler then runs
the ADC to get samples from each enabled channel and pushed the data to a buffer
which is latter delivered to the application.

In order to build the IIO project make sure you have the following configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad7091r8-sdz/Makefile>`_

.. code-block:: bash

        # Select the example you want to enable by choosing y for enabling and n for disabling
        BASIC_EXAMPLE = n
        IIO_TIMER_TRIGGER_EXAMPLE = y

Alternatively, one can set the desired example flag when calling the compiler to
make it build the project with the specified example.

.. code-block:: bash

        # Builds the project with an IIO example supporting buffered captures.
        make IIO_TIMER_TRIGGER_EXAMPLE=y

No-OS Supported Platforms
-------------------------

Maxim Platform
^^^^^^^^^^^^^^

**Used hardware**:

* `EVAL-AD7091R-xSDZ <https://www.analog.com/eval-ad7091r-xsdz>`_ with
* `MAX78000FTHR <https://www.analog.com/en/design-center/evaluation-hardware-and-software/evaluation-boards-kits/max78000fthr.html>`_

**EVAL Board and Microcontroller Connections**

There are a number of connections to make between the ADC evaluation board and the microcontroller.

+-------------------------------+-------------------------------+-----------------------------------------+
| EVAL-AD7091R-xSDZ_ test point | Pin Function                  | MAX78000FTHR_ Pin function (Pin number) |
+-------------------------------+-------------------------------+-----------------------------------------+
| CS                            | Chip Select                   |          SS0     (P0_11)                |
+-------------------------------+-------------------------------+-----------------------------------------+
| SCLK                          | Serial Clock                  |          SCLK    (P0_7)                 |
+-------------------------------+-------------------------------+-----------------------------------------+
| SDO                           | Serial Data Out               |          MISO    (P0_6)                 |
+-------------------------------+-------------------------------+-----------------------------------------+
| SDI                           | Serial Data In                |          MOSI    (P0_5)                 |
+-------------------------------+-------------------------------+-----------------------------------------+
| RESET                         | ADC Reset                     |          GPIO    (P0_19)                |
+-------------------------------+-------------------------------+-----------------------------------------+
| CONVST                        | Conversion Start Signal       |          GPIO    (P1_6)                 |
+-------------------------------+-------------------------------+-----------------------------------------+
| VDRIVE (J4)                   | Digital Supply Voltage Input  |          3.3V    (pin 2)                |
+-------------------------------+-------------------------------+-----------------------------------------+
| GND    (J4)                   | Digital Supply Voltage Ground |          GND     (pin 4)                |
+-------------------------------+-------------------------------+-----------------------------------------+
| VDD (J9)                      | Power Supply Input            |          3.3V    (pin 2)                |
+-------------------------------+-------------------------------+-----------------------------------------+
| GND (J9)                      | Power Supply Ground           |          GND     (pin 4)                |
+-------------------------------+-------------------------------+-----------------------------------------+

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

