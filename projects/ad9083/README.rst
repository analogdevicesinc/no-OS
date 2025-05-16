AD9083 no-OS Example Project
============================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
---------------------------

* `EVAL-AD9083 <https://www.analog.com/EVAL-AD9083>`_

Overview
--------

The AD9083 is a 16-channel, 125 MHz bandwidth, continuous time sigma-delta
(CTSD) ADC with an integrated programmable single-pole antialiasing filter
and termination resistor. Each ADC core features a first-order CTSD modulator
with background nonlinearity correction and self-cancelling dither. The analog
input and clock signals are differential inputs with a variety of user-selectable
input ranges.

Each ADC has a signal processing tile containing a cascaded integrator comb
(CIC) filter and a quadrature digital downconverter (DDC) with multiple FIR
decimation filters for out-of-band noise rejection and sample rate reduction.
The ADC features high-speed serialized outputs via a Subclass 1 JESD204B
interface, supporting various lane configurations (up to four).

The EVAL-AD9083 evaluation board integrates the AD9083 ADC alongside an
AD9528 JESD204B clock generator with a 100 MHz VCXO reference and on-board
power management using the LTM8074 Silent Switcher regulator. Synchronization
across multiple units is possible using SYSREF, TRIG, and SYNCINB pins.
The AD9083 is configurable via a 1.8 V capable 3-wire SPI interface.

Applications
------------

* Millimeter wave imaging
* Electronic beam forming and phased arrays
* Multichannel wideband receivers
* Electronic support measures

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-AD9083 evaluation board requires an external 12 V supply. On-board
regulators generate the required voltage domains for the AD9083.

+------------+-------------+-------------------------------+
| Test Point | Voltage (V) | Description                   |
+------------+-------------+-------------------------------+
| TP19       | 12.0        | Board input supply            |
+------------+-------------+-------------------------------+
| TP6        | 1.0         | V1P0V (analog and digital)    |
+------------+-------------+-------------------------------+
| TP14       | 1.8         | V1P8V (analog and digital)    |
+------------+-------------+-------------------------------+

Board Connectors
~~~~~~~~~~~~~~~~

* **J20** — SMA connector for analog signal input
* **J2** — SMA connector for external trigger input (1.8 V active high)
* **J3** — SMA connector for system ready indicator

No-OS Build Setup
-----------------

Please see: `No-OS Build Guide <https://wiki.analog.com/resources/no-os/build>`_

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad9083/src>`__.

Demo example
~~~~~~~~~~~~

This is the default example which initializes the AD9083 device, configures
the AD9528 clock generator, sets up the JESD204B link, and verifies the
link status. The AXI ADC core and AXI DMA are initialized for data capture.

In order to build the demo project make sure you have the following
configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/blob/main/projects/ad9083/Makefile>`__:

.. code-block:: bash

   # IIOD flag is disabled by default
   IIOD ?= n

IIO example
~~~~~~~~~~~

This project is an IIOD demo for the EVAL-AD9083 evaluation board. The
project launches the IIOD server on the board so that the user may connect
to it via an IIO client application. Using IIO-Oscilloscope or Scopy, the
user can read samples from the device.

If you are not familiar with ADI IIO Application, please take a look at:
:dokuwiki:`IIO No-OS </resources/tools-software/no-os-software/iio>`

If you are not familiar with ADI IIO-Oscilloscope Client, please take a
look at:
:dokuwiki:`IIO Oscilloscope </resources/tools-software/linux-software/iio_oscilloscope>`

In order to build the IIO project make sure you have the following
configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/blob/main/projects/ad9083/Makefile>`__:

.. code-block:: bash

   # Select the example you want to enable by choosing y for enabling and n for disabling
   IIOD = y

No-OS Supported Platforms
-------------------------

Xilinx
~~~~~~

Used Hardware
^^^^^^^^^^^^^

* `EVAL-AD9083 <https://www.analog.com/EVAL-AD9083>`_
* `ZCU102 <https://www.xilinx.com/ZCU102>`_

Connections
^^^^^^^^^^^

Connect the EVAL-AD9083 to the ZCU102 using the FMC HPC connector.
The JESD204B serial lanes interface with the FPGA transceivers via the
FMC connector. The AD9083 is configured over SPI from the FPGA. Ensure
the FMC I/O voltage levels are set to 1.8 V on the ZCU102.

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   # copy the Xilinx hardware description file
   cp <SOME_PATH>/system_top.xsa .
   # to delete current build
   make reset
   # to build the project
   make
   # to flash the code
   make run
