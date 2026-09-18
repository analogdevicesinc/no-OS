ADAR300X no-OS Example Project
==============================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
---------------------------

* :adi:`ADAR3000`
* :adi:`ADAR3001`

Overview
--------

The ADAR3000 is a 17.7 GHz to 21.2 GHz transmit or receive, 4-beam and
4-element Ka band beamforming core chip for phased arrays. Each of the 16 RF
channels provides a 31.5 dB gain range in 0.5 dB steps and up to 54.5 ps of
time delay in 0.865 ps steps. All on-chip registers are controlled through a
4-wire SPI, and four address pins allow up to 16 devices to share the same
serial lines.

This project runs on a Xilinx ZCU102 carrier connected to the
ADAR3000/ADAR3001-EVALZ evaluation board through an SDP-I-FMC interposer. It
brings the device up and validates the control path: it releases the hardware
reset, issues the soft reset sequence, proves the SPI link in both directions
with a scratchpad write and read back test, validates the ``CHIPTYPE``
register, and prints the product ID and SPI revision over the UART console.

Applications
------------

* Geosynchronous high-throughput satellite (GEO HTS)
* Low earth orbit (LEO) constellations
* Mobile satellite terminals (land, air, and sea)
* Terrestrial, airborne and satellite phased arrays

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The ADAR3000/ADAR3001-EVALZ is supplied from two bench supplies, applied to the
test loops on the evaluation board:

============= ======================== ============= ==================
Rail          Test Points              Current Limit Expected Draw
============= ======================== ============= ==================
1.2 V analog  Red ``1P2V`` to ``AGND`` 300 mA        ~128 mA (ADAR3000)
1.8 V digital Red ``1P8V`` to ``DGND`` 100 mA        ~7 mA
============= ======================== ============= ==================

The data sheet gives absolute maximum ratings of 1.32 V on V1P2 and 1.98 V on
V1P8, so set the supply over-voltage protection below those values.

Headers ``P8`` and ``P9`` must have jumpers shorting pin 1 to pin 2. These
connect the chip's internal 1.0 V LDO outputs, and the device will not operate
without them.

FMC VADJ Prerequisite
~~~~~~~~~~~~~~~~~~~~~

The carrier's FMC ``VADJ`` rail must be at **1.8 V**. On the ZCU102 this rail is
not fixed: the on-board system controller reads the IPMI FRU EEPROM of the card
in the FMC slot, per the VITA 57.1 standard, and sets ``VADJ`` from the DC Load
record it finds. If no valid record is present it leaves ``VADJ`` at 0 V.

``VADJ`` supplies VCCO for the FPGA I/O banks behind the FMC connector, so at
0 V no pin on that connector can drive. The symptom is distinctive and easy to
misread: the FPGA configures, the application runs, SPI initialization
succeeds, and every register read returns ``0xFF`` because MISO floats.

The SDP-I-FMC interposer is the card in the FMC slot, so it is the one that
must carry the FRU record, and it ships with a blank EEPROM. If the project
reports a scratchpad mismatch, verify ``VADJ`` before suspecting the driver or
the wiring.

On-board Connectors
~~~~~~~~~~~~~~~~~~~

The ZCU102 drives the evaluation board over its FMC HPC1 connector through the
SDP-I-FMC interposer. SPI comes from the ZynqMP PS SPI0 controller routed over
EMIO, and the control pins come from EMIO GPIO:

=============== ================== ===============================================
Signal          ZynqMP Source      Function
=============== ================== ===============================================
SPI0            PS SPI0 via EMIO   Register access, MODE 0, MSB first, 1 MHz
``spi_sel_a``   spi0_csn[0]        Chip select
GPIO 3          EMIO 35            Evaluation board SPI mux select, driven low
GPIO 4          EMIO 36            ``UPDATE``, beamstate latch strobe
GPIO 5          EMIO 37            ``RESET``, beam qualifier
GPIO 6          EMIO 38            ``MUTE``, beam qualifier
GPIO 7          EMIO 39            ``RSTB``, active low hardware reset
=============== ================== ===============================================

The evaluation board's SPI mux select must be low for the FMC SPI bus to reach
the device; the driver drives it low during initialization.

The UART console is the ZCU102 USB-UART at 115200 baud, 8N1.

No-OS Supported Examples
------------------------

Basic Example
~~~~~~~~~~~~~

The basic example performs SPI and power validation. It initializes the device,
which releases the hardware reset, issues the soft reset sequence, writes and
reads back two scratchpad patterns, and checks ``CHIPTYPE``. It then prints the
product ID and SPI revision, and removes the device.

Expected console output on a working setup:

.. code-block:: bash

   ADAR300X basic example
   SPI_CONFIG after soft reset: 0x3C
   SCRATCHPAD 0xAD: OK
   SCRATCHPAD 0xEA: OK
   CHIPTYPE: 0x01
   PRODUCT_ID: 0x01 0xC0   SPI_REV: 0x01
   adar300x_init: OK (ADAR3000, dev_addr 0)

No-OS Supported Platforms
-------------------------

Xilinx Platform
~~~~~~~~~~~~~~~

Used Hardware
^^^^^^^^^^^^^

* :adi:`ADAR3000` or :adi:`ADAR3001` evaluation board
* SDP-I-FMC interposer
* Xilinx ZCU102 carrier board
* Two bench power supplies

Connections
^^^^^^^^^^^

Fit the interposer to the ZCU102 **FMC HPC1** connector and the evaluation
board to the interposer's 120-pin SDP connector. Apply the bench supplies as
described in `Power Supply Requirements`_, and confirm the ``VADJ``
prerequisite described in `FMC VADJ Prerequisite`_.

Build Command
^^^^^^^^^^^^^

The Xilinx platform uses the CMake/Ninja build system via the
``no_os_build.py`` helper script. Available variants: ``basic_example``.
Available boards: ``zcu102``.

For toolchain setup and prerequisites, see the
:doc:`Xilinx CMake build guide </build_guides/build_xilinx_cmake>`.

.. code-block:: bash

   # Source the Xilinx tools environment
   source /path/to/Vitis/settings64.sh
   # PowerShell equivalent:
   # & "C:\Xilinx\Vitis\2025.1\settings64.ps1"

   # Build
   python tools/scripts/no_os_build.py build --project adar300x \
           --variant basic_example --board zcu102 \
           --hardware /path/to/system_top.xsa

   # Build and flash
   python tools/scripts/no_os_build.py build --project adar300x \
           --variant basic_example --board zcu102 \
           --hardware /path/to/system_top.xsa --probe jlink --flash

The ``--hardware`` argument takes the ``.xsa`` exported from the ``adar3000``
ZCU102 HDL project, which supplies the PS configuration and the bitstream.
