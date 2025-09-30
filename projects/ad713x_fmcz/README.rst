AD713x FMCZ no-OS Example Project
==================================

.. no-os-doxygen::

.. contents::
	:depth: 3

Supported Evaluation Boards
----------------------------

* `EVAL-AD7134FMCZ <https://www.analog.com/EVAL-AD7134FMCZ>`_

Overview
--------

The AD7134 is a quad channel, low noise, simultaneous sampling,
precision analog-to-digital converter (ADC), based on the continuous time
sigma-delta (CTSD) modulation scheme. The AD7134 achieves a dynamic range
of 108 dB, with +/-2 ppm integral nonlinearity (INL), and +/-5 uV offset error.
Operating at a maximum output data rate of 1.5 MSPS per channel, it supports
24-bit resolution with an input bandwidth of up to 391 kHz.

The EVAL-AD7134FMCZ evaluation board integrates two AD7134 ADCs for
multidevice sampling demonstrations. It connects to a host platform
via an FMC connector (VITA 57). Required input power ranges from 8 V
to 14 V, regulated internally to 5 V, 1.8 V, and 1.2 V by onboard
regulators. Distinct 4.096 V references are provided by the ADR444 for
each ADC. Clock inputs can be sourced from a crystal, MEMS oscillator,
or external CMOS input.

Applications
-------------

* Electrical test and measurement
* Audio test
* 3-phase power quality analysis
* Control and hardware in loop verification
* Sonars
* Condition monitoring for predictive maintenance
* Acoustic and material science research and development

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-AD7134FMCZ evaluation board requires an input power supply
ranging from 8 V to 14 V. The onboard regulators convert this to the
necessary operating voltages of 5 V, 1.8 V, and 1.2 V needed for the
system's components. Distinct 4.096 V references are provided by the
ADR444 for each ADC to ensure accurate data conversion.

On-board Connectors
~~~~~~~~~~~~~~~~~~~

======================  =====================================================
Connector               Description
======================  =====================================================
J1                      SMB connector for XTAL2, external clock input
J2 to J17               SMB connectors for analog input (Channel 0 to 7)
P1, P7                  8-pin connectors for differential analog inputs
P3                      Bench top power supply voltage input
P5, P9                  Optional external connectors for driver (Ch 0, Ch 4)
P6, P10                 Optional external connectors for driver (Ch 0, Ch 1)
P8                      FMC connector (160-pin, VITA 57)
======================  =====================================================

No-OS Build Setup
-----------------

Please see: `No-OS Build Guide <https://wiki.analog.com/resources/no-os/build>`_

No-OS Supported Examples
-------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad713x_fmcz/src>`_

Demo Example
~~~~~~~~~~~~

The demo example initializes two AD7134 ADC devices over SPI using the
SPI Engine and AXI DMAC cores. It configures PWM signals for sample
triggering, acquires 1024 samples across all 8 channels (4 channels
per ADC), converts the raw 24-bit data to voltage values, and prints
the results over UART at 115200 baud.

IIO Example
~~~~~~~~~~~

The IIO example initializes both AD7134 ADCs and launches an IIOD
server on the board so that the user may connect to it via an IIO
client. Using IIO Oscilloscope, users can access device attributes
such as offset, sampling frequency, and data scale, facilitating data
acquisition and conversion of raw ADC readings into voltage signals.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with ADI IIO Oscilloscope Client, please take a look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

To build the IIO example, add the following flag when invoking make:

.. code-block:: bash

   IIOD=y

No-OS Supported Platforms
--------------------------

Xilinx Platform
~~~~~~~~~~~~~~~

**Used Hardware**

* `EVAL-AD7134FMCZ <https://www.analog.com/EVAL-AD7134FMCZ>`_
* `ZedBoard <https://www.analog.com/en/resources/reference-designs/powering-zynq-evaluation-development-board-zedboard.html>`_

**Connections**

* Mount the EVAL-AD7134FMCZ onto the ZedBoard through the FMC LPC
  connector (P8 on the evaluation board).
* Supply the ZedBoard with 12 VDC using its barrel jack (J20). Power
  for the EVAL-AD7134FMCZ is supplied via the FMC connector from the
  ZedBoard.
* Connect a micro-USB cable from the ZedBoard USB-UART port (J14) to
  your PC for serial communication.

**Build Command**

.. code-block:: bash

   cp <SOME_PATH>/system_top.xsa .
   # to delete current build
   make reset
   # to build the project
   make
   # to flash the code
   make run
