Evaluating AD7616 on SDP-K1
===========================

.. contents::
    :depth: 3

Supported Evaluation Boards
---------------------------

* `AD7616 <https://www.analog.com/AD7616>`_

Overview
--------

The AD7616 is a 16-bit, DAS that supports dual simultaneous sampling of 16
channels. The AD7616 operates from a single 5 V supply and can accommodate
±10 V, ±5 V, and ±2.5 V true bipolar input signals while sampling at throughput
rates up to 1 MSPS per channel pair with 90.5 dB SNR. Higher SNR performance can
be achieved with the on-chip oversampling mode (92 dB for an oversampling ratio
(OSR) of 2).

The input clamp protection circuitry can tolerate voltages up to ±21 V.
The AD7616 has 1 MÙ analog input impedance, regardless of sampling frequency.
The single-supply operation, on-chip filtering, and high input impedance
eliminate the need for driver op amps and external bipolar supplies.

The device contains analog input clamp protection, a dual, 16-bit charge
redistribution SAR analog-to-digital converter (ADC), a flexible digital filter,
a 2.5 V reference and reference buffer, and high speed serial and parallel
interfaces.

Applications
------------

* Power line monitoring
* Protective relays
* Multiphase motor control
* Instrumentation and control systems
* Data acquisition systems (DASs)

Hardware Specifications
-----------------------

Power Supply Requirements
^^^^^^^^^^^^^^^^^^^^^^^^^

Make sure to power both EVAL-AD7616 and SDP-K1 via the barrel jack connector.

EVAL-AD7616 Jumpers setup
^^^^^^^^^^^^^^^^^^^^^^^^^

+--------------------+-----------+--------------------------------------+
| Jumper/Solder link | Position  | Description                          |
+--------------------+-----------+--------------------------------------+
| SL1                | Unmounted | Channel Sequencer Enable             |
+--------------------+-----------+--------------------------------------+
| SL2                | Unmounted | RC Enable input                      |
+--------------------+-----------+--------------------------------------+
| SL3                | Unmounted | Selects 1 MISO mode                  |
+--------------------+-----------+--------------------------------------+
| SL4                | Unmounted | Oversampling Ratio Selection OS2     |
+--------------------+-----------+--------------------------------------+
| SL5                | Mounted   | If mounted, selects serial interface |
+--------------------+-----------+--------------------------------------+
| SL6                | Unmounted | Oversampling Ratio Selection OS1     |
+--------------------+-----------+--------------------------------------+
| SL7                | Unmounted | Oversampling Ratio Selection OS0     |
+--------------------+-----------+--------------------------------------+
| LK40               | A         | Onboard 5V0 power supply selected    |
+--------------------+-----------+--------------------------------------+
| LK41               | A         | Onboard 3V3 power supply selected    |
+--------------------+-----------+--------------------------------------+


No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

IIO example
^^^^^^^^^^^

This project is actually a IIOD demo for EVAL-AD7616 device.
The project launches a IIOD server on the board so that the user may connect
to it via an IIO client.
Using IIO-Oscilloscope, the user can configure the ADC and view the measured data
on a plot.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with ADI IIO-Oscilloscope Client, please take a look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

The No-OS IIO Application together with the No-OS IIO AD7616 driver take care of
all the back-end logic needed to setup the IIO server.

The read buffer is used for storing the burst data which shall be retrieved
by any LibIIO client.

No-OS Supported Platforms
-------------------------

STM32 Platform
^^^^^^^^^^^^^^

**Used hardware**:

* `EVAL-AD7616 <https://www.analog.com/en/design-center/evaluation-hardware-and-software/evaluation-boards-kits/EVAL-AD7616.html>`_ with
* `SDP-K1 <https://www.analog.com/en/design-center/evaluation-hardware-and-software/evaluation-boards-kits/sdp-k1.html>`_ and
* ST debugger

**Fly-wire connections**

+-------------+-----------------------+
| EVAL-AD7616 | SDP-K1 Arduino header |
+-------------+-----------------------+
| SCLK        | D13                   |
+-------------+-----------------------+
| DB10/SDI    | D11                   |
+-------------+-----------------------+
| DB12/SDOA   | D12                   |
+-------------+-----------------------+
| CS          | D10                   |
+-------------+-----------------------+
| CONVST      | D5                    |
+-------------+-----------------------+
| RESET       | D7                    |
+-------------+-----------------------+
| BUSY        | D6                    |
+-------------+-----------------------+

**Build Command**

.. code-block:: bash

        # to delete current build
        make reset
        # to build the project
        make
        # to flash the code
        make run
        # to debug the code
        make debug
