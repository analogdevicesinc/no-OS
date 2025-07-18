AD7768-EVB no-OS Example Project
================================

.. no-os-doxygen::

.. contents::
	:depth: 3

Supported Evaluation Boards
----------------------------

* `EVAL-AD7768FMCZ <https://www.analog.com/ad7768>`_

Overview
--------

The AD7768 is an 8-channel, simultaneously sampling, 24-bit sigma-delta
(Sigma-Delta) ADC with power scaling and 110.8 kHz input bandwidth. Each channel
has a dedicated Sigma-Delta modulator and digital filter, enabling synchronized
sampling of ac and dc signals. The AD7768 achieves 108 dB dynamic range at
maximum input bandwidth, with typical performance of +/-2 ppm integral
nonlinearity (INL), +/-50 uV offset error, and +/-30 ppm gain error. The
device offers three power modes: fast (256 kSPS, 110.8 kHz bandwidth,
51.5 mW per channel), median (128 kSPS, 55.4 kHz bandwidth, 27.5 mW per
channel), and low power (32 kSPS, 13.8 kHz bandwidth, 9.375 mW per channel).

The EVAL-AD7768FMCZ evaluation board provides full-featured evaluation of
the AD7768 with eight differential analog inputs via SMA connectors, signal
routing through ADA4896 amplifiers, and a precise 4.096 V ADR444 reference.
Configuration and operation are managed through a 4-wire SPI interface.

Applications
-------------

* Data acquisition systems: USB/PXI/Ethernet
* Instrumentation and industrial control loops
* Audio testing and measurement
* Vibration and asset condition monitoring
* 3-phase power quality analysis
* Sonar
* High precision medical electroencephalogram (EEG) or
  electromyography (EMG)/electrocardiogram (ECG)

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-AD7768FMCZ evaluation board requires an external 7 V to 9 V
power supply, applied via either the J1 dc plug connector or the J3
bench top supply connector. On-board ADP7118 LDOs regulate the input
to 5 V (for AVDD1 and on-board amplifiers) and 3.3 V (for IOVDD).
The IOVDD voltage for digital logic is maintained between 2.25 V and
3.6 V, with support for 1.8 V mode available by bypassing the internal
LDO. Alternatively, the board can be powered from the EVAL-SDP-CH1Z
12 V rail by setting LK2 to Position A.

On-board Connectors
~~~~~~~~~~~~~~~~~~~

===================================  ==========================================================
Connector                            Description
===================================  ==========================================================
J1                                    Wall wart (dc plug) power supply input (7 V to 9 V)
J3                                    Bench top power supply input (7 V to 9 V)
J2                                    External supply for AVDD1, AVDD2, IOVDD, AVSS, AGND
J6, J7                                SMA/SMB connectors for clock input (XTAL2/CLKIN, XTAL1)
J8, J9                                8-pin connector for analog input to Channel 4-7
J13, J14                              8-pin connector for analog input to Channel 0-3
AI0+ to AI7+, AI0- to AI7-           SMA connectors for differential analog inputs per channel
P1                                    FMC connector (underside) that mates with carrier board
===================================  ==========================================================

No-OS Build Setup
-----------------

Please see: `No-OS Build Guide <https://wiki.analog.com/resources/no-os/build>`_

No-OS Supported Examples
-------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad7768-evb/src>`_

Demo Example
~~~~~~~~~~~~

The demo example initializes the AD7768 ADC using SPI and GPIO interfaces.
The AXI ADC core is set up for channel management, and an AXI-based DMAC
is employed for data acquisition.

IIO Example
~~~~~~~~~~~

The IIO example initializes and configures the AD7768 ADC and enables
interaction with libiio clients via the serial backend. It manages data
acquisition using AXI DMA, which facilitates sample collection, cyclic
data transfers, and communication with DDR memory.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

To build the IIO example, add the following flag when invoking make:

.. code-block:: bash

   IIOD=y

No-OS Supported Platforms
--------------------------

Xilinx Platform
~~~~~~~~~~~~~~~

**Used Hardware**

* `EVAL-AD7768FMCZ <https://www.analog.com/ad7768>`_
* `ZedBoard <https://www.analog.com/en/resources/reference-designs/powering-zynq-evaluation-development-board-zedboard.html>`_

**Connections**

* Mount the EVAL-AD7768FMCZ to the ZedBoard through the FMC LPC
  connector.
* Connect a 7 V to 9 V power supply to the J1 (dc plug) or J3
  (bench top) connector on the EVAL-AD7768FMCZ.
* Connect a micro-USB cable to the ZedBoard for UART communication.
* Connect your desired input signal to the evaluation board via the
  SMA connectors.
* Power on the ZedBoard. Verify that the green power LED is illuminated.

**Build Command**

.. code-block:: bash

   cp <SOME_PATH>/system_top.xsa .
   # to delete current build
   make reset
   # to build the project
   make
   # to flash the code
   make run
