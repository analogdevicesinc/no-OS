AD7124-8-PMDZ no-OS Example Project
====================================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
----------------------------

* `EVAL-AD7124-8-PMDZ <https://www.analog.com/EVAL-AD7124-8>`_

Overview
--------

The EVAL-AD7124-8-PMDZ is a PMOD-compatible evaluation board for the
AD7124-8, a low power, low noise, completely integrated analog front end
for high precision measurement applications. The AD7124-8 contains a low
noise, 24-bit sigma-delta ADC and can be configured to have 8
differential inputs or 15 single-ended or pseudo differential inputs.

The device features an on-chip programmable gain amplifier (PGA) with
gains from 1 to 128, a band gap reference with 10 ppm/C drift, matched
programmable excitation currents, and an internal clock oscillator. It
offers three power modes (low, mid, full) with typical current draw
ranging from 255 uA to 930 uA. The AD7124-8 requires an analog domain
supply (AVDD) of 2.7 V to 3.6 V and a digital domain supply (IOVDD) of
1.65 V to 3.6 V.

Applications
------------

* Temperature measurement
* Pressure measurement
* Industrial process control
* Instrumentation
* Smart transmitters

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-AD7124-8-PMDZ board is powered via the PMOD connector (IOVDD).
The AD7124-8 requires an analog supply (AVDD) of 2.7 V to 3.6 V and a
digital supply (IOVDD) of 1.65 V to 3.6 V.

On-board Connectors
~~~~~~~~~~~~~~~~~~~

========= =====================================================
Connector Function
========= =====================================================
P1        PMOD digital interface (CS, SDIN, SDO, SCLK, IOVDD)
P2        Analog inputs (AIN0-AIN7, REFIN1+/-, AVDD, AVSS, CLK)
P3        Extended I/O (AIN8-AIN15, REFOUT, SYNC, PSW)
========= =====================================================

Eight solder jumpers on the board control default connections for
REFIN1, REFOUT, AVDD, AVSS, and SDO routing, allowing operational
mode adjustments without component changes.

No-OS Build Setup
-----------------

Please see: `No-OS Build Guide <https://wiki.analog.com/resources/no-os/build>`_

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad7124-8pmdz/src>`__.

IIO example
~~~~~~~~~~~

This project is an IIOD demo for the EVAL-AD7124-8-PMDZ evaluation
board. The project launches the IIOD server on the board so that the
user may connect to it via an IIO client application. Using
IIO-Oscilloscope, the user can read and configure ADC channels,
filters, and data rates.

If you are not familiar with ADI IIO Application, please take a look at:
:dokuwiki:`IIO No-OS </resources/tools-software/no-os-software/iio>`

If you are not familiar with ADI IIO-Oscilloscope Client, please take a
look at:
:dokuwiki:`IIO Oscilloscope </resources/tools-software/linux-software/iio_oscilloscope>`

In order to build the IIO project, make sure you have the following
configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/blob/main/projects/ad7124-8pmdz/Makefile>`__:

.. code-block:: bash

   # Select the example you want to enable by choosing y for enabling and n for disabling
   IIOD = y

No-OS Supported Platforms
-------------------------

ADuCM3029
~~~~~~~~~

Used Hardware
^^^^^^^^^^^^^

* `EVAL-ADICUP3029 <https://www.analog.com/EVAL-ADICUP3029>`_
* `EVAL-AD7124-8-PMDZ <https://www.analog.com/EVAL-AD7124-8>`_

Connections
^^^^^^^^^^^

Connect the EVAL-AD7124-8-PMDZ to the EVAL-ADICUP3029 via the PMOD
connector. The SPI signals (CS, DIN, DOUT, SCLK) and power are routed
through the PMOD interface. Connect a micro-USB cable to the P10
connector of the EVAL-ADICUP3029 for UART communication with a PC.

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   # to build the project
   make
   # to flash the code
   make run
