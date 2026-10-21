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
ADAR3000/ADAR3001-EVALZ evaluation board through an SDP-I-FMC interposer. Its
basic example exercises every driver feature and checks each one against the
hardware.

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

The UART console is the ZCU102 USB-UART at 115200 baud, 8N1. The ZCU102
enumerates four USB-UART ports; the console is the first of them,
``/dev/ttyUSB0`` on a Linux host when no other adapter is attached.

No-OS Supported Examples
------------------------

This project is organized around the no-OS variant based build flow. Selecting
a variant at build time (``--variant <name>``) chooses which application is
compiled, and the platform ``main()`` calls the entry point of the selected
example. Shared initialization data is defined in
`src/common <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adar300x/src/common>`__,
and platform-specific macros and extra init parameters are in
`src/platform <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adar300x/src/platform>`__.

Basic Example
~~~~~~~~~~~~~

The basic example exercises every feature of the driver and checks each one
against the hardware, stopping with an error at the first mismatch. In order,
it:

* initializes the device: hardware reset, soft reset, scratchpad write and read
  back, ``CHIPTYPE`` and ``PRODUCT_ID_L`` checks
* writes a distinct delay and attenuation to all 16 elements of the direct
  control bank, reads them back and strobes an update
* checks that the amplifiers power up enabled with a bias of 3, then writes and
  reads back the beam and element amplifier bias and enable
* reads the temperature sensor twice, checks that the readings agree and lie
  in a plausible ambient range, and prints the temperature
* writes beamstates into the RAM of beam 0 and of beam 3, the last beamstate of
  the last beam, and reads them back
* queues three beamstates on the FIFO of beam 0 and checks that the write
  pointer advances by three
* writes and reads back the reset and mute beamstate banks, then checks through
  the FIFO read pointer that an update dequeues exactly one beamstate, that a
  reset empties the queue, and that the next update is a plain update again
* checks the power-on RAM sequencer range of every beam, then writes a wrapping
  range and reads it back

To run it, open a serial terminal on the console port, for example
``picocom -b 115200 /dev/ttyUSB0``, then build and flash the ``basic_example``
variant as shown in `Build Command`_. Expected console output on a working
setup; the temperature follows the board:

.. code-block:: bash

   ADAR300X basic example
   ADAR3000 initialized: PRODUCT_ID 0xC001, SPI_REV 0x01
   DIRECT beamstate write/readback: OK
   UPDATE strobed on all 4 beams
   Amplifier power-up defaults: OK (enabled, bias 3)
   Amplifier bias/enable write/readback: OK
   ADC temperature: 24.083 degC (code 126)
   RAM beamstate write/readback: OK
   FIFO beam 0 write pointer 0 to 3 after 3 loads, read 0
   RESET and MUTE beamstate write/readback: OK
   Beam UPDATE and RESET via FIFO read pointer, and MUTE: OK
   RAM sequencer range defaults and write/readback: OK

Any other final line is an error message naming the check that failed and the
values it saw.

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
prerequisite described in `FMC VADJ Prerequisite`_. Connect the ZCU102 USB-UART
and JTAG ports to the host.

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

   # Build the basic example
   python tools/scripts/no_os_build.py build --project adar300x \
           --variant basic_example --board zcu102 \
           --hardware /path/to/system_top.xsa

   # Build and flash the basic example
   python tools/scripts/no_os_build.py build --project adar300x \
           --variant basic_example --board zcu102 \
           --hardware /path/to/system_top.xsa --probe jlink --flash

The ``--hardware`` argument takes the ``.xsa`` exported from the ``adar3000``
ZCU102 HDL project, which supplies the PS configuration and the bitstream.
