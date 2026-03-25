ADF5902 no-OS Example Project
=============================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
---------------------------

* `EV-ADF5902SD1Z <https://www.analog.com/EV-ADF5902SD1Z>`_

Overview
--------

The ADF5902 is a 24 GHz ISM band VCO and PGA with 2-channel transmitter
designed for FMCW radar applications. It features a 24 GHz to 24.25 GHz
VCO, a 2-channel 24 GHz power amplifier with 8 dBm output, and an on-chip
temperature sensor. The device supports frequency ramp generation and
calibration for automotive and industrial radar systems.

The EV-ADF5902SD1Z evaluation board enables characterization of the ADF5902
device. It interfaces with an FPGA platform via SPI and GPIO using the
SDP-I-FMC interposer board.

Applications
------------

* Automotive radars
* Industrial radars
* Microwave radar sensors

No-OS Build Setup
-----------------

Please see: `No-OS Build Guide <https://wiki.analog.com/resources/no-os/build>`_

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
`Project Source Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adf5902_sdz/src>`__.

Application example
~~~~~~~~~~~~~~~~~~~

This is the default example which initializes the ADF5902 device, performs
calibration using ``adf5902_recalibrate``, computes the locked frequency
with ``adf5902f_compute_frequency``, and reads the on-chip temperature
sensor with ``adf5902_read_temp``.

No-OS Supported Platforms
-------------------------

Xilinx
~~~~~~

Used Hardware
^^^^^^^^^^^^^

* `EV-ADF5902SD1Z <https://www.analog.com/EV-ADF5902SD1Z>`_
* `Zedboard <https://www.xilinx.com/products/boards-and-kits/1-8dyf-11.html>`_
* `SDP-I-FMC Interposer <https://www.analog.com/EVAL-SDP-I-FMC>`_

Connections
^^^^^^^^^^^

Connect the EV-ADF5902SD1Z to the Zedboard via the SDP-I-FMC interposer
board. The SPI interface and GPIO signals (CE, TX_DATA) are managed by
the Xilinx PS7 peripherals.

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
