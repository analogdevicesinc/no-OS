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
has two variants: a basic example that exercises every driver feature and
checks each against the hardware, and an IIO example that serves the device to
libiio clients over the UART.

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

IIO Example
~~~~~~~~~~~

The IIO example serves the ADAR300X as an IIO device over the console UART,
at 115200 baud, for libiio clients such as ``iio_info``, ``iio_attr`` and
``iio_rwdev``. The device, its channels and its attributes are described in the
driver documentation, under ``ADAR300X no-OS IIO support``.

If you are not familiar with the no-OS IIO support, please take a look at:
:doc:`IIO Framework </iio_framework>`

Host Requirements
^^^^^^^^^^^^^^^^^

The procedure below uses the libiio command line tools. Install **libiio
1.0.0** or later from the
`libiio releases <https://github.com/analogdevicesinc/libiio/releases>`__,
which provide Linux packages, a macOS package and a Windows installer. Check
the version with:

.. code-block:: bash

   iio_info -V

Older ``iio_writedev`` tools that report ``Using Libiio v0.x -> v1.x
compatibility layer`` silently discard buffer writes, exiting without an error,
and should not be used for this procedure.

IIO Test Procedure
~~~~~~~~~~~~~~~~~~

Build and flash the ``iio_example`` variant as shown in `Build Command`_, and
close any serial terminal on the console port: libiio needs the port to
itself. Wait a few seconds for the board to boot, then set the context URI in
the shell used for all the steps below:

.. code-block:: bash

   U=serial:/dev/ttyUSB0,115200,8n1

The steps are written for a bash shell: Linux, macOS, or Git Bash or WSL on
Windows. With the Windows libiio tools under Git Bash, use the console COM port
instead, for example ``U=serial:COM5,115200,8n1``.

The expected outputs below are those of a freshly booted board; run the steps
in order, since some of them build on the state left by earlier ones. A write
with ``iio_attr`` prints the value read back after the write.

1. Enumeration
^^^^^^^^^^^^^^

.. code-block:: bash

   iio_info -u $U | grep -E "devices:|adar3000|channels found|buffer found|scan elements found"

Expected output:

.. code-block:: bash

   IIO context has 1 devices:
   	iio:device0: adar3000 (buffer capable)
   		33 channels found:
   		1 buffer found:
   			32 scan elements found:

``iio_info -u $U`` without the filter lists every channel and attribute.

2. Temperature
^^^^^^^^^^^^^^

.. code-block:: bash

   raw=$(iio_attr -u $U -c adar3000 temp16 raw)
   offset=$(iio_attr -u $U -c adar3000 temp16 offset)
   scale=$(iio_attr -u $U -c adar3000 temp16 scale)
   echo "raw $raw offset $offset scale $scale"
   awk "BEGIN { printf \"%.1f degC\n\", ($raw + $offset) * $scale / 1000 }"

Expected output; the code and temperature follow the board:

.. code-block:: bash

   raw 125 offset -99.750000000 scale 917.431192660
   23.2 degC

3. Delay and Attenuation Scales
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: bash

   iio_attr -u $U -c -o adar3000 phase0 scale
   iio_attr -u $U -c -o adar3000 power0 scale

Expected output, picoseconds and decibels per step:

.. code-block:: bash

   0.865000000
   0.500000000

4. Direct Control Bank
^^^^^^^^^^^^^^^^^^^^^^

Write, then read back, the delay and attenuation of beam 1, element 1:

.. code-block:: bash

   iio_attr -u $U -c -o adar3000 phase5 raw 21
   iio_attr -u $U -c -o adar3000 phase5 raw
   iio_attr -u $U -c -o adar3000 power5 raw 42
   iio_attr -u $U -c -o adar3000 power5 raw

Expected output:

.. code-block:: bash

   21
   21
   42
   42

5. Beam Mode
^^^^^^^^^^^^

.. code-block:: bash

   iio_attr -u $U -d adar3000 beam1_mode
   iio_attr -u $U -d adar3000 beam1_mode_available

Expected output:

.. code-block:: bash

   direct
   direct memory fifo instant_direct reset mute

6. RAM Sequencer Range
^^^^^^^^^^^^^^^^^^^^^^

Read the power-on range of beam 1, set it to 10 to 20 and read it back:

.. code-block:: bash

   iio_attr -u $U -d adar3000 beam1_ram_start
   iio_attr -u $U -d adar3000 beam1_ram_stop
   iio_attr -u $U -d adar3000 beam1_ram_start 10
   iio_attr -u $U -d adar3000 beam1_ram_stop 20
   iio_attr -u $U -d adar3000 beam1_ram_start
   iio_attr -u $U -d adar3000 beam1_ram_stop

Expected output:

.. code-block:: bash

   0
   63
   10
   20
   10
   20

7. Amplifier Bias and Enable
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Read the power-on operational state of beam amplifier 0, then write and read
back the sleep state of beam amplifier 2 and of element amplifier 1:

.. code-block:: bash

   iio_attr -u $U -d adar3000 beam0_amp_bias_operational
   iio_attr -u $U -d adar3000 beam0_amp_en_operational
   iio_attr -u $U -d adar3000 beam2_amp_bias_sleep 5
   iio_attr -u $U -d adar3000 beam2_amp_bias_sleep
   iio_attr -u $U -d adar3000 element1_amp_bias_sleep 2
   iio_attr -u $U -d adar3000 element1_amp_bias_sleep
   iio_attr -u $U -d adar3000 element1_amp_en_sleep 0
   iio_attr -u $U -d adar3000 element1_amp_en_sleep

Expected output:

.. code-block:: bash

   3
   1
   5
   5
   2
   2
   0
   0

8. Update Source
^^^^^^^^^^^^^^^^

The update, reset and mute commands of the next steps are driven over SPI:

.. code-block:: bash

   iio_attr -u $U -d adar3000 update_intf_ctrl
   iio_attr -u $U -d adar3000 update_intf_ctrl_available
   iio_attr -u $U -d adar3000 update_intf_ctrl SPI

Expected output:

.. code-block:: bash

   pin
   pin SPI
   SPI

9. Memory Load Mode
^^^^^^^^^^^^^^^^^^^

With beam 2 in memory load mode its channels address RAM beamstate
``beam2_ram_index``. Write a different delay to beamstates 5 and 6 of beam 2,
element 0, and read each back:

.. code-block:: bash

   iio_attr -u $U -d adar3000 beam2_load_mode memory
   iio_attr -u $U -d adar3000 beam2_ram_index 5
   iio_attr -u $U -c -o adar3000 phase8 raw 33
   iio_attr -u $U -d adar3000 beam2_ram_index 6
   iio_attr -u $U -c -o adar3000 phase8 raw 44
   iio_attr -u $U -d adar3000 beam2_ram_index 5
   iio_attr -u $U -c -o adar3000 phase8 raw
   iio_attr -u $U -d adar3000 beam2_ram_index 6
   iio_attr -u $U -c -o adar3000 phase8 raw

Expected output:

.. code-block:: bash

   memory
   5
   33
   6
   44
   5
   33
   6
   44

10. FIFO, Update and Reset
^^^^^^^^^^^^^^^^^^^^^^^^^^

Put beam 3 in FIFO mode and queue two beamstates through its channels; the
eighth value of each, ``power15``, commits it. Then issue an update, which
dequeues one beamstate, and a reset, which empties the queue:

.. code-block:: bash

   iio_attr -u $U -d adar3000 beam3_mode fifo
   iio_attr -u $U -d adar3000 beam3_load_mode fifo
   for n in 1 2; do
   	for c in phase12 power12 phase13 power13 phase14 power14 phase15 power15; do
   		iio_attr -u $U -c -o adar3000 $c raw $n > /dev/null
   	done
   done
   echo "queued:     wr $(iio_attr -u $U -d adar3000 beam3_fifo_wr) rd $(iio_attr -u $U -d adar3000 beam3_fifo_rd)"
   iio_attr -u $U -d adar3000 beam3_update 1
   echo "one update: wr $(iio_attr -u $U -d adar3000 beam3_fifo_wr) rd $(iio_attr -u $U -d adar3000 beam3_fifo_rd)"
   iio_attr -u $U -d adar3000 beam3_mode reset
   iio_attr -u $U -d adar3000 beam3_update 1
   echo "reset:      wr $(iio_attr -u $U -d adar3000 beam3_fifo_wr) rd $(iio_attr -u $U -d adar3000 beam3_fifo_rd)"

Expected output; ``beam3_update`` reads 0 because the update strobe clears
itself:

.. code-block:: bash

   fifo
   fifo
   queued:     wr 2 rd 0
   0
   one update: wr 2 rd 1
   reset
   0
   reset:      wr 2 rd 2

11. Instantaneous Direct Mode Guard
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

A beam in instantaneous direct mode cannot enter its reset or mute beamstate,
so selecting mute is refused:

.. code-block:: bash

   iio_attr -u $U -d adar3000 beam3_mode instant_direct
   iio_attr -u $U -d adar3000 beam3_mode mute; echo "exit $?"

Expected output:

.. code-block:: bash

   instant_direct
   ERROR: iio_attr: Unable to write attribute: Invalid argument (22)
   exit 1

12. Buffer Load into RAM
^^^^^^^^^^^^^^^^^^^^^^^^

Push four beamstates for beam 0 through the output buffer into RAM beamstates
0 to 3, then read them back through the channels. Scan ``n`` carries the
values ``8n`` to ``8n + 7``.

The ``iio_rwdev`` of libiio 1.0.0 never sends the last block of a write, yet
exits successfully. The data is therefore sent twice, with twice the samples
requested: the first block, carrying the beamstates, is sent and only the
duplicate is lost. Each block starts at ``beam0_ram_index``, so a tool that
sends both blocks writes the same beamstates twice.

.. code-block:: bash

   iio_attr -u $U -d adar3000 beam0_load_mode memory
   iio_attr -u $U -d adar3000 beam0_ram_index 0
   python3 -c "import sys; d = bytes(range(32)); sys.stdout.buffer.write(d + d)" > beamstates.bin
   iio_rwdev -u $U -w -b 4 -s 8 adar3000 \
   	phase0 power0 phase1 power1 phase2 power2 phase3 power3 < beamstates.bin
   echo "iio_rwdev exit $?"
   for s in 0 1 2 3; do
   	iio_attr -u $U -d adar3000 beam0_ram_index $s > /dev/null
   	echo "beamstate $s: $(for c in phase0 power0 phase1 power1 phase2 power2 phase3 power3; do
   		iio_attr -u $U -c -o adar3000 $c raw; done | tr '\n' ' ')"
   done

Expected output:

.. code-block:: bash

   memory
   0
   iio_rwdev exit 0
   beamstate 0: 0 1 2 3 4 5 6 7
   beamstate 1: 8 9 10 11 12 13 14 15
   beamstate 2: 16 17 18 19 20 21 22 23
   beamstate 3: 24 25 26 27 28 29 30 31

13. Register Access
^^^^^^^^^^^^^^^^^^^

Read the SPI revision register, then write and read back the scratchpad:

.. code-block:: bash

   iio_reg -u $U adar3000 0x00B
   iio_reg -u $U adar3000 0x00A 0x5A
   iio_reg -u $U adar3000 0x00A

Expected output:

.. code-block:: bash

   0x1
   0x5a

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
``no_os_build.py`` helper script. Available variants: ``basic_example``,
``iio_example``. Available boards: ``zcu102``.

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

   # Build and flash the IIO example
   python tools/scripts/no_os_build.py build --project adar300x \
           --variant iio_example --board zcu102 \
           --hardware /path/to/system_top.xsa --probe jlink --flash

The ``--hardware`` argument takes the ``.xsa`` exported from the ``adar3000``
ZCU102 HDL project, which supplies the PS configuration and the bitstream.
