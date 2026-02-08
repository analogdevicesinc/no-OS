AD9083 no-OS Example Project
============================

.. contents::
    :depth: 3

Supported Evaluation Boards
---------------------------

- :adi:`EVAL-AD9083`

Overview
--------

The AD9083 is a 16-channel, 125 MHz bandwidth, continuous time Σ-Δ
(CTSD) ADC with an integrated programmable antialiasing filter and
termination resistor, making it suitable for low power applications in a
compact form. Each ADC core features a CTSD modulator with background
nonlinearity correction and dither for noise reduction. It supports
differential signals for analog and clock inputs. Signal processing
includes cascaded integrator comb (CIC) filters and quadrature digital
downconverters (DDCs) for effective band-shaped noise suppression. The
ADC boasts high-speed serialized outputs via a Subclass 1 JESD204B
interface, supporting various lane configurations. Synchronization
across multiple units is possible using SYSREF, TRIG, and SYNCINB pins.
The AD9083 allows power savings through flexible power-down modes and is
configurable via a 1.8V-capable 3-wire SPI interface. It is housed in a
100-ball CSP_BGA package, designed to operate in temperatures from -40°C
to +85°C.

Applications
------------

- Millimeter wave imaging
- Electronic beam forming and phased arrays
- Multichannel wireless receivers
- Electronic support measures

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The AD9083 evaluation board requires external power sources, as it lacks
internal power management for autonomous operation. It relies on
external supplies for power domains such as AVDD, AVDD1P8, DVDD, and
DVDD1P8, which have specific voltage and tolerance requirements. No
provisions are integrated into the board for powering it internally
without these external inputs.

+-----------------+-----------------+----------------+
| Domain          | Voltage (V)     | Tolerance (%)  |
+-----------------+-----------------+----------------+
| AVDD            | 1.0             | ±5             |
+-----------------+-----------------+----------------+
| AVDD1P8         | 1.8             | ±5             |
+-----------------+-----------------+----------------+
| DVDD            | 1.0             | ±5             |
+-----------------+-----------------+----------------+
| DVDD1P8         | 1.8             | ±5             |
+-----------------+-----------------+----------------+

No-OS Build Setup
-----------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the 
`Project SRC Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad9083/src>`__ .

IIO example
~~~~~~~~~~~

This project is actually an IIOD demo for the EVAL-AD9083 evaluation
board. The project launches the IIOD server on the board, so that the
user may connect to it via an IIO-Oscilloscope Client app. Using the
IIO-Oscilloscope, the user can read samples from the device.

If you are not familiar with ADI IIO Application, please take a look at:
:dokuwiki:`IIO No-OS </resources/tools-software/no-os-software/iio>`

If you are not familiar with ADI IIO-Oscilloscope Client, please take a
look at: :dokuwiki:`IIO Oscilloscope </resources/tools-software/linux-software/iio_oscilloscope>`.

The No-OS IIO Application together with the No-OS IIO AD9083 driver take
care of all the back-end logic needed to setup the IIO server.

This example initializes the IIO device and calls the IIO app as shown
in the `IIO Example <https://github.com/analogdevicesinc/no-OS/blob/main/drivers/adc/ad9083/iio_ad9083.c>`__ .

In order to build the IIO project make sure you have the following
configuration in the 
`Makefile <https://github.com/analogdevicesinc/no-OS/blob/main/projects/ad9083/Makefile>`__ .

.. code-block:: bash

   # Select the example you want to enable by choosing y for enabling and n for disabling

   IIO_EXAMPLE = y

No-OS Supported Platforms
-------------------------

Xilinx
~~~~~~

Used Hardware
^^^^^^^^^^^^^

- AD9083 Evaluation Board
- Zynq ZCU102 (Xilinx)

Connections
^^^^^^^^^^^

Use the HPC FMC connectors to establish a connection between the
EVAL-AD9083 and ZCU102 boards. Make sure to correctly interface the
ZCU102 HPC connectors (J4, J5) with EVAL-AD9083. Configure the I/O
standards and voltage levels, typically setting FMC connections to 1.8V.
To power up the Zynq ZCU102 board, ensure the main power switch (SW1) is
initially in the “OFF” position. Connect the power supply to the power
connector J52 on the board. Turn switch SW1 to the “ON” position to
power the board, at which point LED DS2 will illuminate, indicating
successful power supply. Verify correct polarity and voltage levels to
prevent damage. For further configuration, including switch settings and
connector interfaces, refer to the ZCU102 Evaluation Board User Guide.

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
