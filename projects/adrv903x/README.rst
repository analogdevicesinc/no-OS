================================
adrv903x no-OS Example Project
================================

.. no-os-doxygen::

Supported Devices
=================

* `ADRV9032 <https://www.analog.com/en/products/adrv9032r.html>`_

Supported Carriers
==================

* `ZCU102 <https://www.xilinx.com/ZCU102>`_

Overview
========

The ADRV9032 is a 4-transmitter / 4-receiver (4T4R) integrated RF transceiver
operating from 650 MHz to 6 GHz with an on-chip ARM Cortex-A55 application
processor.  This project brings up the ADRV9032 on the ZCU102 using a full
JESD204C link managed by the no-OS JESD204 FSM.

The active profile is **UC101 4T4R1OR NLS** (4 TX / 4 RX / 1 observation
receiver, no loopback sync):

* **JESD204C**, 64B/66B encoding, 8 lanes TX + 8 lanes RX
* Lane rate: 16220.160 Mbps
* DEVCLK: 245.760 MHz (AD9528 channel 1)
* LEMC rate: 7.680 MHz
* 8 TX / 8 RX IQ channels (4 physical antennas × I + Q)

Three example applications are provided: Basic, DMA, and IIO.

Prerequisites
=============

Prior to building the project, the Xilinx development environment must be set
up.  Follow the build guide for your Vitis version:

* Vitis 2025.1: https://analogdevicesinc.github.io/no-OS/build_guides/build_xilinx_vitis2025.html
* General no-OS build guide: https://wiki.analog.com/resources/no-os/build

Connect the ADRV9032 evaluation board to the **FMC2** connector on the ZCU102
before programming.

A ``system_top.xsa`` file for the ZCU102 + ADRV9032 HDL design must be present
in the project directory before building.  Either obtain a pre-built file or
build the HDL yourself:
https://wiki.analog.com/resources/fpga/docs/build.

Building the Project
====================

Open a terminal, navigate to the project directory, and invoke ``make``.  The
heap size is already configured in the ``Makefile`` (``-Xlinker
--defsym=_HEAP_SIZE=0x800000``); no manual linker-script editing is required.

.. code-block:: bash

   cd no-OS/projects/adrv903x
   make -j

A successful build ends with output similar to:

.. code-block:: bash

   [HH:MM:SS] Creating BOOT.BIN and archive with files
   text       data        bss        dec        hex  filename
   ...                                                /path/to/build/adrv903x.elf
   [HH:MM:SS] Done (build/adrv903x.elf)

For more details on available make targets, see:
https://wiki.analog.com/resources/no-os/make.

Project Layout
==============

::

   no-OS/projects/adrv903x/
   ├── Makefile
   ├── README.rst
   ├── src
   │   ├── common
   │   │   ├── app_config.h              ← JESD204 parameters, UART baud rate
   │   │   ├── clkgen_routines.c/h       ← AD9528 + AXI clkgen helper
   │   │   ├── common_data.c/h           ← Shared init structures
   │   │   ├── initdata.c/h              ← Pre-initialized PostMcsInit defaults
   │   │   ├── firmware
   │   │   │   ├── ADRV9030_DeviceProfileTest.h
   │   │   │   ├── ADRV9030_FW.h
   │   │   │   ├── ADRV9030_RxGainTable.h
   │   │   │   └── ADRV9030_stream_image.h
   │   │   └── hal
   │   │       ├── no_os_platform.c
   │   │       └── no_os_platform.h
   │   ├── examples
   │   │   ├── basic_example
   │   │   │   ├── basic_example.c
   │   │   │   └── basic_example.h
   │   │   ├── dma_example
   │   │   │   ├── dma_example.c
   │   │   │   └── dma_example.h
   │   │   └── iio_example
   │   │       ├── iio_example.c
   │   │       ├── iio_example.h
   │   │       └── iio_example.mk
   │   ├── LICENSE_API
   │   ├── LICENSE_FW
   │   └── platform
   │       ├── platform_includes.h
   │       └── xilinx
   │           ├── main.c
   │           ├── parameters.c
   │           ├── parameters.h           ← AXI base addresses, buffer sizes
   │           └── platform_src.mk
   ├── src.mk
   └── system_top.xsa

Driver Layout
=============

::

   no-OS/drivers/rf-transceiver/palma/
   ├── adrv903x.c                         ← no-OS driver + JESD204 FSM callbacks
   ├── adrv903x.h
   ├── common/                            ← ADI common error, HAL, logging
   │   ├── adi_common.h
   │   ├── adi_error/
   │   ├── adi_hal/
   │   └── adi_logging/
   ├── devices/
   │   └── adrv903x/                      ← Palma device API
   │       ├── private/                   ← Bitfield access, internal calibrations
   │       └── public/                    ← Public API headers and sources
   └── platforms/                         ← Platform abstraction layer

Demo Applications
=================

Three mutually exclusive examples are provided.  Select one by passing
``EXAMPLE=<name>`` to ``make``; the default is ``basic_example``.

.. list-table::
   :header-rows: 1
   :widths: 20 30 50

   * - Example
     - Build command
     - Description
   * - Basic
     - ``make EXAMPLE=basic_example``
     - JESD204C link bring-up only; no data movement
   * - DMA
     - ``make EXAMPLE=dma_example``
     - TX sine wave + RX one-shot DMA capture
   * - IIO
     - ``make EXAMPLE=iio_example``
     - IIOD server; use IIO Oscilloscope for live capture and DDS control

Only one example can be active at a time.

.. note::

   All three examples share the same JESD204C link bring-up sequence.  The
   debug UART baud rate is **115200** for the Basic and DMA examples.  The IIO
   example uses **921600** for the IIOD protocol (the same UART port is shared
   between the initial link-up log and the IIO server).

Basic Example
-------------

Initializes all hardware components and brings up the JESD204C link via the
FSM.  Once the link is running, JESD204 TX/RX status is printed and the
application exits.  No DMA transfers take place.

Build and run
^^^^^^^^^^^^^

.. code-block:: bash

   make EXAMPLE=basic_example

To program the FPGA and run the ELF over JTAG in one step:

.. code-block:: bash

   make run

Alternatively, open the project in Vitis IDE for source-level debugging:

.. code-block:: bash

   make sdkopen

Then use *Run As* or *Debug As* from within the IDE.

Connect a serial terminal at **115200 8-N-1** to observe output.  Example
using ``picocom``:

.. code-block:: bash

   picocom -b 115200 /dev/ttyUSB1

Expected output
^^^^^^^^^^^^^^^

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
   rx_adxcvr: OK (16220160 kHz)
   ... (repeated ~100 times during CDR lock retry loop)
   ERR: adrv903x/.../axi_adxcvr.c:508: QPLL RX buffer underflow error, status: 0x21
   adrv903x: ADC_RX calibration complete
   adrv903x: link0 deframer linkState 0x3
   adrv903x: link0 lane0 status 0xE
   adrv903x: link0 lane1 status 0xE
   adrv903x: link2 framer status 0x82
   adrv903x: initialized channels 0x11311 (TX=0x11 RX=0x11)
   adrv903x: signal chain activated
   ADRV903X JESD204 link up
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
   underflow error`` are expected and do not indicate a failure.

   The HDL transceiver block is built for 8 physical lanes to support the lane
   remapping required between the ADRV9032 chip and the FPGA GTH transceivers
   (for example, chip lane 7 may be wired to FPGA GTH lane 0).  However, the
   active JESD204C link uses only 2 of those 8 lanes.  During the CDR lock
   retry loop in ``adxcvr_clk_enable()``, the driver polls the buffer status
   of all 8 transceiver instances; the 6 inactive instances report a transient
   buffer underflow.  This error is non-fatal: the driver continues retrying
   until the 2 active lanes lock, after which the link reaches DATA state
   normally.

   If ``SYSREF alignment error: Yes`` appears in the JESD status, it indicates
   a SYSREF timing issue.  Re-running the application typically resolves this.

DMA Example
-----------

Transmits a sine wave continuously on all 8 TX channels via DMA, waits 1 s
for the signal to settle, then captures one full RX buffer (262144 bytes,
16384 samples per channel) using the AXI data offload in normal
(store-and-forward) mode.  To observe the received sine wave, connect a
loopback cable from **TX0 → RX0** and **TX4 → RX4** on the evaluation board.

Build and run
^^^^^^^^^^^^^

.. code-block:: bash

   make EXAMPLE=dma_example

To program and run over JTAG:

.. code-block:: bash

   make run

Alternatively, open the project in Vitis IDE:

.. code-block:: bash

   make sdkopen

Connect a serial terminal at **115200 8-N-1**:

.. code-block:: bash

   picocom -b 115200 /dev/ttyUSB1

Expected output
^^^^^^^^^^^^^^^

After the JESD204 link-up messages (same as Basic Example), the DMA example
prints:

.. code-block:: bash

   RX data offload: normal mode (store+forward)
   ADRV903X JESD204 link up
   DMA TX: address=0x001e4900 size=262144
   DMA RX: address=0x002f2c00 bytes=262144 samples_per_ch=16384 channels=8 bits=16
   ADRV903X DMA example complete

.. note::

   The TX and RX buffer addresses shown above are determined by the linker and
   will vary between builds.  Always use the address printed by the application
   in the ``DMA RX: address=`` line when running the capture script below.

Capturing data with xsct
^^^^^^^^^^^^^^^^^^^^^^^^^

Use the ``capture.tcl`` script from the no-OS tools directory to read RX
samples from DDR memory into per-channel CSV files.  The script arguments are:

.. code-block:: bash

   xsct ../../tools/scripts/platform/xilinx/capture.tcl \
       ZYNQ_PSU <rx_address> <total_samples> <num_channels> <bits>

Substitute ``<rx_address>`` with the address printed by the application.  For
the DMA example configuration (8 channels, 16-bit, 16384 samples/channel):

.. code-block:: bash

   xsct ../../tools/scripts/platform/xilinx/capture.tcl \
       ZYNQ_PSU 0x002f2c00 131072 8 16

Where ``131072`` is the total number of 16-bit samples across all 8 channels
(262144 bytes ÷ 2 bytes/sample = 131072 samples).  The script generates 8 CSV
files, one per channel.

Visualising the CSV data
^^^^^^^^^^^^^^^^^^^^^^^^

Use the ``plot.py`` script in the no-OS tools directory.  It requires
``matplotlib``, so create and activate a virtual environment first:

.. code-block:: bash

   python3 -m venv .venv
   source .venv/bin/activate
   pip install matplotlib

Then plot all 8 channels (all samples):

.. code-block:: bash

   python ../../tools/scripts/platform/xilinx/plot.py 8

To plot only the first *N* samples per channel, pass a second argument:

.. code-block:: bash

   python ../../tools/scripts/platform/xilinx/plot.py 8 1024

For more information about the DMA capture workflow, see:
https://wiki.analog.com/resources/no-os/dac_dma_example.

IIO Example
-----------

Starts an IIOD server over UART.  Once running, connect IIO Oscilloscope to
configure the DAC (DDS tones) and stream live ADC captures.

The following IIO devices are exposed:

.. list-table::
   :header-rows: 1
   :widths: 35 15 50

   * - Device name
     - Type
     - Description
   * - ``cf-adrv903x-lpc``
     - ADC
     - 4 RX channels (voltage0–voltage3), 16-bit IQ samples
   * - ``cf-adrv903x-dds-core-lpc``
     - DAC
     - 2 TX channels (TX 1, TX 2) with dual DDS tone generators per channel

Build
^^^^^

.. code-block:: bash

   make EXAMPLE=iio_example

Alternatively, open the project in Vitis IDE:

.. code-block:: bash

   make sdkopen

Running and connecting
^^^^^^^^^^^^^^^^^^^^^^

1. Program and run the ELF:

   .. code-block:: bash

      make run

2. Connect a serial terminal at **115200 8-N-1** to observe initialisation
   messages (same JESD link-up sequence as the Basic Example, but at 115200
   baud because the IIO server takes over the same UART at that rate):

   .. code-block:: bash

      picocom -b 115200 /dev/ttyUSB1

   After the JESD204 link-up messages (the ``rx_adxcvr`` CDR retry loop may
   also produce a ``QPLL RX buffer overflow error`` in addition to the
   underflow — both are non-fatal), the application prints:

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

3. **Close the serial terminal.**  The UART is shared between debug output
   and the IIOD protocol; leaving it open will prevent IIO Oscilloscope from
   connecting.

4. Launch `IIO Oscilloscope <https://github.com/analogdevicesinc/iio-oscilloscope>`_.

5. In the *Connect* dialog, select **Serial Context** and enter:

   .. code-block:: bash

      Port:         /dev/ttyUSB1   (or the appropriate serial port)
      Baud rate:    921600
      Data bits:    8
      Parity:       none
      Stop bits:    1

   Click **Refresh** to scan for devices, then click **Connect**.

Configuring DAC tones with IIO Oscilloscope
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

1. Open the **DAC Data Manager** tab.
2. Select **DDS** Mode.
3. Set **Scale(dBFS)** to **0 dB** and select **One CW Tone**.
4. Set the desired **Frequency** (Hz) for each TX channel.  The DAC starts
   transmitting immediately.

Capturing ADC data with IIO Oscilloscope
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

1. Go to **File → New Plot** to open a capture window.
2. In the **Plot Channels** panel, expand **cf-adrv903x-lpc** and check the
   channels you want to capture (voltage0–voltage3), or click **Enable All**.
3. Under **Plot Type**, select **Time Domain** to view the sine waveform, or
   **Frequency Domain** (FFT) to verify the tone frequency and spectral
   purity.
4. Set the desired sample count (default 400; use a larger value such as
   16384 for the FFT to improve frequency resolution).
5. Click **Capture** for a single-shot capture or enable continuous mode for
   a live view.

