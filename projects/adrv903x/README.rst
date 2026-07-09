adrv903x no-OS Example Project
================================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Devices
-----------------

* `ADRV9032 <https://www.analog.com/en/products/adrv9032r.html>`_

Supported Carriers
------------------

* `ZCU102 <https://www.xilinx.com/ZCU102>`_

Overview
--------

The ADRV9032 is a 4-transmitter / 4-receiver (4T4R) integrated RF
transceiver operating from 650 MHz to 6 GHz with an on-chip ARM
Cortex-A55 application processor. This project brings up the ADRV9032
on the ZCU102 using a full JESD204C link managed by the no-OS JESD204
FSM.

The active profile is **UC101 4T4R1OR NLS** (4 TX / 4 RX / 1
observation receiver, no loopback sync):

* **JESD204C**, 64B/66B encoding, 8 lanes TX + 8 lanes RX
* Lane rate: 16220.160 Mbps
* DEVCLK: 245.760 MHz (AD9528 channel 1)
* LEMC rate: 7.680 MHz
* 8 TX / 8 RX IQ channels (4 physical antennas x I + Q)

Three example variants are provided: Basic, DMA, and IIO.

Driver Layout
-------------

::

   no-OS/drivers/rf-transceiver/palma/
   ├── adrv903x.c                         <- no-OS driver + JESD204 FSM callbacks
   ├── adrv903x.h
   ├── common/                            <- ADI common error, HAL, logging
   │   ├── adi_common.h
   │   ├── adi_error/
   │   ├── adi_hal/
   │   └── adi_logging/
   ├── devices/
   │   └── adrv903x/                      <- Palma device API
   │       ├── private/                   <- Bitfield access, internal calibrations
   │       └── public/                    <- Public API headers and sources
   └── platforms/                         <- Platform abstraction layer

No-OS Supported Examples
-------------------------

Three mutually exclusive example variants are provided.

.. note::

   All three examples share the same JESD204C link bring-up sequence. The
   debug UART baud rate is **115200** for the Basic and DMA examples. The
   IIO example uses **921600** for the IIOD protocol (the same UART port
   is shared between the initial link-up log and the IIO server).

Basic Example
~~~~~~~~~~~~~

The basic example (variant ``basic_example``) initializes all hardware
components and brings up the JESD204C link via the FSM. Once the link is
running, JESD204 TX/RX status is printed and the application exits. No
DMA transfers take place.

Connect a serial terminal at **115200 8-N-1** to observe output.

Expected output:

.. code-block:: bash

   ADRV903X basic example - JESD204 link bring-up
   AD9528 locked, DEVCLK 245.76 MHz on channel 1
   rx_adxcvr: Using QPLL with previously defined settings.
   adrv903x-phy Rev 0, API version: 2.12.1.4 found
   adrv903x: FW 0.0.0.0, stream 0.0.0.0
   adrv903x: firmware loaded, ARM CPU running
   ADRV903X initialized successfully
   rx_clkgen: MMCM-PLL locked (245760000 Hz)
   tx_clkgen: MMCM-PLL locked (245760000 Hz)
   adrv903x: deframer0 M=4 F=4 K=64 Np=16 lanes=2 204C
   adrv903x: framer0 M=4 F=4 K=64 Np=16 lanes=2 204C
   tx_adxcvr: OK (16220160 kHz)
   adrv903x: MCS complete (status=0x1, 1 pulse(s))
   adrv903x: PostMcsInit complete
   adrv903x: framer 1 enabled
   adrv903x: SERDES calibration complete
   adrv903x: deframers enabled, JESD204 link ready
   rx_adxcvr: OK (16220160 kHz)
   ...
   adrv903x: JESD204 link up
   tx_jesd status:
           Link is enabled
           Measured Link Clock: 245.761 MHz
           Reported Link Clock: 245.760 MHz
           Lane rate: 16220.160 MHz
           Lane rate / 66: 245.760 MHz
           LEMC rate: 7.680 MHz
           Link status: DATA
           SYSREF captured: Yes
           SYSREF alignment error: No
   rx_jesd status:
           Link is enabled
           Measured Link Clock: 245.761 MHz
           Reported Link Clock: 245.760 MHz
           Lane rate: 16220.160 MHz
           Lane rate / 66: 245.760 MHz
           LEMC rate: 7.680 MHz
           Link status: DATA
           SYSREF captured: Yes
           SYSREF alignment error: No

.. note::

   The repeated ``rx_adxcvr: OK`` lines and the single ``QPLL RX buffer
   underflow error`` are expected and do not indicate a failure. The HDL
   transceiver block is built for 8 physical lanes to support lane
   remapping; the active JESD204C link uses only 2. The 6 inactive
   instances report a transient buffer underflow during the CDR lock
   retry loop.

   If ``SYSREF alignment error: Yes`` appears, re-running the application
   typically resolves it.

DMA Example
~~~~~~~~~~~

The DMA example (variant ``dma_example``) transmits a sine wave
continuously on all 8 TX channels via DMA, waits 1 s for the signal to
settle, then captures one full RX buffer (262144 bytes, 16384 samples
per channel) using the AXI data offload in normal (store-and-forward)
mode. To observe the received sine wave, connect a loopback cable from
**TX0 -> RX0** and **TX4 -> RX4** on the evaluation board.

After the JESD204 link-up messages the DMA example prints:

.. code-block:: bash

   RX data offload: normal mode (store+forward)
   ADRV903X JESD204 link up
   DMA TX: address=0x001e4900 size=262144
   DMA RX: address=0x002f2c00 bytes=262144 samples_per_ch=16384 channels=8 bits=16
   ADRV903X DMA example complete

Use the ``capture.tcl`` script to read RX samples from DDR memory into
per-channel CSV files. Substitute ``<rx_address>`` with the address
printed by the application:

.. code-block:: bash

   xsct ../../tools/scripts/platform/xilinx/capture.tcl \
       ZYNQ_PSU 0x002f2c00 131072 8 16

Where ``131072`` is the total number of 16-bit samples across all 8
channels (262144 bytes / 2 bytes/sample = 131072 samples). The script
generates 8 CSV files, one per channel.

To plot all 8 channels:

.. code-block:: bash

   python3 -m venv .venv && source .venv/bin/activate && pip install matplotlib
   python ../../tools/scripts/platform/xilinx/plot.py 8

For more information about the DMA capture workflow, see:
https://wiki.analog.com/resources/no-os/dac_dma_example.

IIO Example
~~~~~~~~~~~

The IIO example (variant ``iio_example``) starts an IIOD server over
UART. Once running, connect IIO Oscilloscope to configure the DAC (DDS
tones) and stream live ADC captures.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with ADI IIO-Oscilloscope Client, please take a
look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

The following IIO devices are exposed:

.. list-table::
   :header-rows: 1
   :widths: 35 15 50

   * - Device name
     - Type
     - Description
   * - ``cf-adrv903x-lpc``
     - ADC
     - 4 RX channels (voltage0-voltage3), 16-bit IQ samples
   * - ``cf-adrv903x-dds-core-lpc``
     - DAC
     - 2 TX channels (TX 1, TX 2) with dual DDS tone generators per channel

Connect a serial terminal at **115200 8-N-1** to observe initialization
messages. After the JESD204 link-up messages, the application prints:

.. code-block:: bash

   Running IIOD server...
   If successful, you may connect an IIO client application by:
   1. Disconnecting the serial terminal you use to view this message.
   2. Connecting the IIO client application using the serial backend:
          Baudrate: 921600
          Data size: 8 bits
          Parity: none
          Stop bits: 1
          Flow control: none

Close the serial terminal. The UART is shared between debug output and
the IIOD protocol; leaving it open will prevent IIO Oscilloscope from
connecting.

No-OS Supported Platforms
--------------------------

Xilinx
~~~~~~

Used Hardware
^^^^^^^^^^^^^

* `ZCU102 Evaluation Kit <https://www.xilinx.com/ZCU102>`__ (Zynq UltraScale+ MPSoC)
* ADRV9032 evaluation board

Connections
^^^^^^^^^^^

Connect the ADRV9032 evaluation board to the **FMC2** connector on the
ZCU102 before programming. Connect a micro-USB cable to the ZCU102 JTAG
connector (J2) and another to the USB-UART connector (J83).

Build Command
^^^^^^^^^^^^^

The Xilinx platform uses the CMake/Ninja build system via the
``no_os_build.py`` helper script. Available variants: ``basic_example``,
``dma_example``, ``iio_example``. Available boards: ``zcu102``.

A Xilinx XSA hardware description file is required. The HDL design name
is ``adrv903x``; the hardware name is composed as ``adrv903x_<board>``
(e.g. ``adrv903x_zcu102``).

For toolchain setup and prerequisites, see the
`Xilinx CMake build guide <https://analogdevicesinc.github.io/no-OS/build_guides/build_xilinx_cmake.html>`__.

.. code-block:: bash

   # Source the Vitis toolchain environment
   source ~/.xilinx/2025.1/Vitis/settings64.sh
   # PowerShell (Windows) equivalent:
   #   & "$env:USERPROFILE\.xilinx\2025.1\Vitis\settings64.bat"

   cd no-OS

   # Build the basic example for ZCU102
   python tools/scripts/no_os_build.py build \
       --project adrv903x --variant basic_example --board zcu102 \
       --hardware /path/to/adrv903x_zcu102/system_top.xsa

   # Build and flash via JTAG
   python tools/scripts/no_os_build.py build \
       --project adrv903x --variant basic_example --board zcu102 \
       --hardware /path/to/adrv903x_zcu102/system_top.xsa \
       --probe openocd --flash

   # Build the DMA example for ZCU102
   python tools/scripts/no_os_build.py build \
       --project adrv903x --variant dma_example --board zcu102 \
       --hardware /path/to/adrv903x_zcu102/system_top.xsa

   # Build the IIO example for ZCU102
   python tools/scripts/no_os_build.py build \
       --project adrv903x --variant iio_example --board zcu102 \
       --hardware /path/to/adrv903x_zcu102/system_top.xsa
