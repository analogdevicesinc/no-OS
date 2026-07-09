ADRV904x no-OS Example Project
================================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
----------------------------

* `ADRV9040 <https://www.analog.com/ADRV9040>`_

Overview
--------

The ADRV904x family is a software-defined radio (SDR) transceiver family from
Analog Devices. The ADRV9040 is an integrated quad RF transceiver capable of
simultaneous multi-band operation. The device integrates four transmit channels,
four receive channels, an observation receiver, and an integrated frequency
synthesizer with phase-locked loop (PLL), covering frequencies from 650 MHz to
6 GHz. The transceiver supports 5G NR, LTE, and multi-standard radio
applications.

The JESD204C-based digital interface connects to an FPGA host platform. On-chip
features include digital predistortion (DPD), crest factor reduction (CFR), an
ARM Cortex-A55 application processor for running on-chip firmware, and integrated
3 Gbps JESD204C SerDes lanes. The device requires the Koror vendor API, bundled
under ``drivers/rf-transceiver/koror/``, for initialization and control.

Applications
------------

* 5G New Radio (NR) base stations
* Multi-standard radio (MSR) systems
* Wireless backhaul and fronthaul
* Software-defined radio (SDR) platforms
* Military and defense communications
* Test and measurement

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The ADRV904x evaluation board is powered through the FMC connector of the
carrier board (ZCU102). The ZCU102 supplies the required voltages to the
evaluation board over the high-pin-count FMC2 connector. Ensure the ZCU102
power supply is rated for the full evaluation board current draw.

On-board Connectors
~~~~~~~~~~~~~~~~~~~~

=================== ===========================================================
Connector           Function
=================== ===========================================================
FMC2 (HPC)          High-speed digital interface to carrier (JESD204C lanes,
                    SPI, GPIO, clocks)
SMA RF ports        Transmit (Tx) and Receive (Rx) RF signal connectors
=================== ===========================================================

Connect the ADRV904x evaluation board to the FMC2 connector on the ZCU102
before powering the system.

No-OS Supported Examples
-------------------------

This project is organized around the no-OS variant based build flow.
Selecting a variant at build time (``--variant <name>``) chooses which
application is compiled. The platform ``main()`` is a thin dispatcher that
calls ``example_main()``, provided by the selected example. Shared
initialization data is defined in
`src/common <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adrv904x/src/common>`__,
and platform-specific macros and extra init parameters are in
`src/platform <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adrv904x/src/platform>`__.

Basic Example
~~~~~~~~~~~~~

The basic example initializes the ADRV904x transceiver via the Koror vendor API,
brings up the JESD204C RX/TX links using the AXI JESD204, AXI ADC core, AXI DAC
core, and AXI DMAC IP cores, and enables the default DDS waveform on Tx. Runtime
status messages (link rates, SYSREF alignment, deframer status) are printed over
UART. This is the recommended starting point for board bring-up.

Select this variant with ``--variant basic_example``.

The output from a successful initialization looks like:

.. code-block:: bash

    rx_adxcvr: Using QPLL with previously defined settings.
    Firmware file: ADRV9040_FW.bin
    Gain Table file: RxGainTable.csv
    Streams file: stream_image.bin
    Device Profile file: DeviceProfileTest.bin
    DFE file: ADRV9040_DFE_CALS_FW.bin

    adrv904x_setup()
    adrv904x-phy Rev 0, API version: 2.10.0.4 found
    adrv904x-device revision: 0xa0

        Using the Profile Init and PostMcsInit Structures
    tx_dac: Successfully initialized (491564941 Hz)
    tx_adxcvr: OK (16220160 kHz)
    rx_adxcvr: OK (16220160 kHz)

    tx_jesd status:
        Link is enabled
        Measured Link Clock: 245.782 MHz
        Reported Link Clock: 245.760 MHz
        Lane rate: 16220.160 MHz
        Lane rate / 66: 245.760 MHz
        LEMC rate: 7.680 MHz
        Link status: DATA
        SYSREF captured: Yes
        SYSREF alignment error: No
    rx_jesd status:
        Link is enabled
        ...
        Link status: DATA
        SYSREF captured: Yes
        SYSREF alignment error: No

DMA Example
~~~~~~~~~~~

The DMA example sends a sinewave on the Tx channels using DMA from an in-memory
lookup table. When a Tx channel is physically looped back to an Rx channel via
an electrical wire, the received data can be read from its memory address. After
the link initialization output, the application prints the memory addresses:

.. code-block:: bash

    DMA_EXAMPLE Tx: address=0x434800 samples=8192 channels=16 bits=32
    DMA_EXAMPLE Rx: address=0x43c800 samples=262144 channels=16 bits=16

The Xilinx ``xsct`` tool can retrieve data from memory and save it to CSV files:

.. code-block:: bash

    xsct tools/scripts/platform/xilinx/capture.tcl ZYNQ_PSU 0x43c800 262144 16 16

The resulting CSV files can be visualized using the ``plot.py`` script:

.. code-block:: bash

    python tools/scripts/platform/xilinx/plot.py 16

More information about the DMA data format is available at
`no-OS DAC DMA example <https://wiki.analog.com/resources/no-os/dac_dma_example>`_.

Select this variant with ``--variant dma_example``.

IIO Example
~~~~~~~~~~~

The IIO example launches an IIOD server on the ZCU102 so that a host IIO client
can connect via the serial backend. The AXI ADC and AXI DAC cores are exposed
through the IIO framework (``iio_axi_adc`` and ``iio_axi_dac``), enabling
host-side data capture and playback using IIO-Oscilloscope or ``libiio`` tools.

If you are not familiar with ADI IIO Application, please take a look at:
:dokuwiki:`IIO No-OS </resources/tools-software/no-os-software/iio>`

If you are not familiar with ADI IIO-Oscilloscope Client, please take a
look at:
:dokuwiki:`IIO Oscilloscope </resources/tools-software/linux-software/iio_oscilloscope>`

After the JESD link is established, the application prints:

.. code-block:: bash

    Running IIOD server...
    If successful, you may connect an IIO client application by:
    1. Disconnecting the serial terminal you use to view this message.
    2. Connecting the IIO client application using the serial backend configured as shown:
        Baudrate: 921600
        Data size: 8 bits
        Parity: none
        Stop bits: 1
        Flow control: none

Connect to the IIOD server using a serial-backend enabled iio-oscilloscope with
the settings printed at the serial terminal.

Select this variant with ``--variant iio_example``.

No-OS Supported Platforms
--------------------------

Xilinx
~~~~~~

Used Hardware
^^^^^^^^^^^^^

* `ADRV9040 evaluation board <https://www.analog.com/ADRV9040>`_
* `ZCU102 Zynq UltraScale+ MPSoC Evaluation Kit <https://www.xilinx.com/ZCU102>`_

Connections
^^^^^^^^^^^

Connect the ADRV904x evaluation board to the **FMC2** (HPC) connector on the
ZCU102 before powering the system.

The UART console appears on the ZCU102 USB-UART adapter (``/dev/ttyUSB0`` or
``/dev/ttyUSB1`` on Linux, ``COMx`` on Windows) at **115200 baud, 8N1**.

Build Command
^^^^^^^^^^^^^

The Xilinx platform uses the CMake/Ninja build system via the
``no_os_build.py`` helper script. Available variants: ``basic_example``,
``dma_example``, ``iio_example``. Available boards: ``zcu102``.

A Xilinx hardware description file (``.xsa``) generated from the matching HDL
design is required. The HDL design name for all variants is ``adrv904x`` (see
``CONFIG_XILINX_HDL_DESIGN`` in each ``.conf`` file). Obtain the pre-built
``adrv904x_zcu102/system_top.xsa`` from the ADI HDL repository releases, or
build it yourself by following the
`Building HDL guide <https://wiki.analog.com/resources/fpga/docs/build>`_.

For toolchain setup and prerequisites, see the
`Xilinx CMake build guide <https://analogdevicesinc.github.io/no-OS/build_guides/build_xilinx_cmake.html>`__.

.. code-block:: bash

   # source the Vitis environment (adjust path to your Vitis install)
   source ~/.xilinx/2025.1/Vitis/settings64.sh
   # PowerShell (Windows) equivalent:
   #   & "$env:USERPROFILE\.xilinx\2025.1\Vitis\settings64.bat"

   cd no-OS

   # build the basic example on the ZCU102
   python tools/scripts/no_os_build.py build \
      --project adrv904x --variant basic_example --board zcu102 \
      --hardware /path/to/adrv904x_zcu102/system_top.xsa

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project adrv904x --variant basic_example --board zcu102 \
      --hardware /path/to/adrv904x_zcu102/system_top.xsa \
      --probe openocd --flash

Replace ``basic_example`` with ``dma_example`` or ``iio_example`` to build
the corresponding variant.

.. note::

   After programming, the Vitis IDE may require manual heap size adjustment
   to 0x800000 and adding the math library (``-lm``) to the GCC linker flags.
