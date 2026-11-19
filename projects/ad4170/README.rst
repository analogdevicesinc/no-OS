AD4170 no-OS Example Project
============================

.. no-os-doxygen::


Contents
--------

.. contents:: Table of Contents
	:depth: 3

Supported Evaluation Boards
---------------------------

* `EVAL-AD4170-4ARDZ <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-ad4170-4ardz.html>`_
* `EVAL-AD4190-4ARDZ <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-ad4190-4ardz.html>`_
* `EVAL-AD4171-8ARDZ <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-ad4171-8ardz.html>`_
* `SDP-K1 <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/sdp-k1.html>`_

Overview
--------

The AD4170-4 is a low noise, low power, 24-bit sigma-delta ADC with a
programmable gain amplifier, an integrated 2.5 V reference and a 16 entry
channel sequencer selecting from 4 differential or 8 single-ended analog
inputs. The digital filter can be configured as sinc5+average, sinc5, sinc3
or FIR, with output data rates from 1.6 SPS up to 500 kSPS.

This project also covers the pin and register compatible family members:

* AD4171-4 and AD4172-2, which differ in the available analog input count.
* AD4190-4 and AD4195-4, which drop the DAC and limit the sinc5 filter
  configuration.
* AD4171-8, which exposes 8 differential or 16 single-ended analog inputs.

The device variant driven by the project is selected through the
``AD4170_DEV_ID`` macro in
`common_data.h <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad4170/src/common/common_data.h>`_,
which defaults to ``ID_AD4171_8``.

Refer to the AD4170-4 data sheet for detailed information regarding operation
of the IC.

Applications
------------

* Precision instrumentation
* Weigh scales and strain gauges
* Temperature measurement (RTD, thermocouple, thermistor)
* Process control

Hardware Specifications
-----------------------

Power Supply Requirements
^^^^^^^^^^^^^^^^^^^^^^^^^

The evaluation board is powered from the SDP-K1 through the Arduino headers, so
no external supply is required for the default configuration.

**Pin Description**

	The evaluation boards are Arduino form factor shields, so they mate
	directly with the Arduino headers of the SDP-K1. The following table
	lists the signals used by this project.

	+-----------+---------------+-----------------------------------------+
	| Arduino   | SDP-K1 pin    | Description                             |
	+-----------+---------------+-----------------------------------------+
	| D10       | PA15          | SPI Chip Select (GPIO)                  |
	+-----------+---------------+-----------------------------------------+
	| D11       | PA7           | SPI1 Master Out Slave In                |
	+-----------+---------------+-----------------------------------------+
	| D12       | PB4           | SPI1 Master In Slave Out                |
	+-----------+---------------+-----------------------------------------+
	| D13       | PB3           | SPI1 Clock                              |
	+-----------+---------------+-----------------------------------------+

	The ADC samples SDI on the SCLK rising edge with SCLK idling high, so the
	SPI controller is configured for mode 3 at 11.25 MHz.

	The example polls the ``RDYB`` bit of the ``DATA_STATUS`` register
	instead of monitoring the DIG_AUX1 output, so no data ready interrupt
	line needs to be wired. Consequently the ``ad4170_read24s()`` and
	``ad4170_continuous_read()`` fast paths, which require a DIG_AUX1 GPIO
	configured as RDY, are not exercised here.

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad4170/src/common>`_

The macros used in Common Data are defined in platform specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad4170/src/platform>`_

IIO example (default)
^^^^^^^^^^^^^^^^^^^^^

This project is an IIOD demo for the AD4170 family evaluation boards.
The project launches an IIOD server on the board so that the user may connect
to it via an IIO client.
Using IIO-Oscilloscope, the user can configure the ADC and view the measured
data on a plot.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with ADI IIO-Oscilloscope Client, please take a look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

The No-OS IIO Application together with the No-OS IIO AD4170 driver take care
of all the back-end logic needed to setup the IIO server.

The ADC is brought up with sequencer channels 0 to 7 mapped to the AIN0-AIN1 up
to the AIN14-AIN15 differential pairs, using the internal 2.5 V reference, a
gain of 1 and the sinc5+average filter. Sequencer channels 8 to 15 have both
multiplexer inputs left ``AD4170_OPEN``, since the AD4171-8 has no further
analog inputs to pair; map them in ``common_data.c`` to measure anything on
them. One IIO channel is exposed for each of the 16 sequencer channels
regardless of the Channel_En register contents, so that the channels the ADC
converts can be picked from the IIO client at buffer enable or at ``raw`` read
time. Channels are named ``voltage0`` to ``voltage15`` after their sequencer
channel, not after the multiplexer inputs they convert. Each channel provides
the ``raw``, ``scale`` and ``sampling_frequency`` attributes.

The output data rate is a property of the setup a channel is mapped to, and the
16 sequencer channels share only 8 setups, so writing ``sampling_frequency``
retunes every channel mapped to the same setup. Map a channel to a setup of its
own in ``common_data.c`` to rate it independently.

This example initializes the IIO device and calls the IIO app as shown in:
`IIO Example <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad4170/src/examples/iio_example>`_

To build the IIO example:

.. code-block:: bash

	make EXAMPLE=iio_example

No-OS Supported Platforms
-------------------------

STM32 Platform
^^^^^^^^^^^^^^

**Used hardware**

* `EVAL-AD4171-8ARDZ <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-ad4171-8ardz.html>`_ (or any
  of the boards listed above, with ``AD4170_DEV_ID`` set accordingly)
* `SDP-K1 <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/sdp-k1.html>`_

**Build Command**

.. code-block:: bash

	# to delete current build
	make reset
	# to build the project for STM32 platform
	make PLATFORM=stm32 HARDWARE=sdp-k1.ioc EXAMPLE=iio_example
	# to flash the code
	make run
