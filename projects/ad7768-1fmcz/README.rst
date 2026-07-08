AD7768-1 no-OS Example Project
==============================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
---------------------------

* `EV-AD77681FMCZ <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/ev-ad77681fmcz.html>`_

Overview
--------

The EV-AD77681FMCZ evaluation kit contains the AD7768-1, a 24-bit, 256 kSPS
precision sigma-delta ADC with an integrated PGA and on-chip reference buffer.
The device achieves 108 dB dynamic range and offers three power modes
(fast, median, and low power), making it well suited for sensor measurement
systems that require both high accuracy and flexible power consumption. A 4-wire
SPI interface provides register access for configuring the input range, power
mode, and data formatting.

The evaluation board plugs into a Xilinx carrier board (such as the ZedBoard)
via the FMC LPC connector. An AXI SPI Engine core on the FPGA manages SPI
communication, and an AXI clock generator provides the master clock to the ADC.

Applications
------------

* Sound and vibration, acoustic, and materials science research
* Control and hardware-in-the-loop verification
* Condition monitoring for predictive maintenance
* Electrical test and measurement
* Audio testing, current and voltage measurement
* Clinical EEG, EMG, and ECG vital-signs monitoring
* USB-, PXI-, and Ethernet-based modular DAQ
* Channel-to-channel isolated modular DAQ designs

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EV-AD77681FMCZ is powered from the carrier board via the FMC connector.
Set the VINSEL jumper to Position B to derive power from the EVAL-SDP-CH1Z
(5 V and 3.3 V). On-board linear regulators (U8, U9, U10) provide 5 V and 7 V;
ADP1718 and ADP7118 generate 3.3 V and 10 V. AD840/AD841 reference amplifiers
stabilize operating conditions.

Link Configuration
~~~~~~~~~~~~~~~~~~

+-----------------------+---------------------+------------------------------------------+
| Link Name             | Default Setting     | Description                              |
+-----------------------+---------------------+------------------------------------------+
| GPIO0                 | R10                 | Driver amplifier power-mode control      |
+-----------------------+---------------------+------------------------------------------+
| GPIO1                 | R11                 | SDP-H1 board connection                  |
+-----------------------+---------------------+------------------------------------------+
| GPIO2                 | R12                 | SDP-H1 board test-mode connection        |
+-----------------------+---------------------+------------------------------------------+
| GPIO3                 | R13                 | SDP-H1 board mode-selection connection   |
+-----------------------+---------------------+------------------------------------------+
| PIN/SPI               | SPI1_A              | Selects SPI control mode                 |
+-----------------------+---------------------+------------------------------------------+
| MCLK                  | MCLK1A              | Master clock source selection            |
+-----------------------+---------------------+------------------------------------------+
| CLK_SEL               | CSEL1A              | Clock source selection                   |
+-----------------------+---------------------+------------------------------------------+
| XTL1                  | R36                 | External oscillator configuration        |
+-----------------------+---------------------+------------------------------------------+
| SYNC_IN               | R25                 | Synchronization input setting            |
+-----------------------+---------------------+------------------------------------------+
| Voltage Input Select  | VVINSEL A/B         | Power input source selection             |
+-----------------------+---------------------+------------------------------------------+

On-board Connectors
~~~~~~~~~~~~~~~~~~~

+-------------------+-----------------------+----------------------------------------------------+
| Connector         | Function              | Description                                        |
+-------------------+-----------------------+----------------------------------------------------+
| SMB Connectors    | AIN1+, AIN1-          | Main differential analog input channels            |
+-------------------+-----------------------+----------------------------------------------------+
| Terminal Blocks   | Additional Inputs     | Alternative access points for analog signals       |
+-------------------+-----------------------+----------------------------------------------------+
| J4 (FMC LPC)      | Carrier Connection    | Connects to the carrier board (e.g. ZedBoard)      |
+-------------------+-----------------------+----------------------------------------------------+
| P1 Socket         | Primary Interface     | Main connection socket for external connections    |
+-------------------+-----------------------+----------------------------------------------------+

No-OS Supported Examples
------------------------

This project is organized around the no-OS variant-based build flow.
Selecting a variant at build time (``--variant <name>``) chooses which
application is compiled. Shared initialization data is defined in
`src <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad7768-1fmcz/src>`__.

Demo Example
~~~~~~~~~~~~

The demo example initializes the AD7768-1 ADC using the AXI SPI Engine core
and the AXI clock generator. It then reads conversion results continuously
in standard (non-offload) SPI Engine mode and prints them over UART.

SPI Engine Offload Example
~~~~~~~~~~~~~~~~~~~~~~~~~~

The SPI Engine offload example configures the AXI SPI Engine in offload mode
with DMA to capture a burst of samples from the AD7768-1 into DDR memory and
prints the raw results over UART.

No-OS Supported Platforms
-------------------------

Xilinx
~~~~~~

Used Hardware
^^^^^^^^^^^^^

* `EV-AD77681FMCZ <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/ev-ad77681fmcz.html>`_
* `ZedBoard <https://www.analog.com/en/resources/reference-designs/powering-zynq-evaluation-development-board-zedboard.html>`_

Connections
^^^^^^^^^^^

The EV-AD77681FMCZ plugs into the ZedBoard via the FMC LPC connector. The SPI
Engine core on the Zynq PL manages the SPI bus (SPI_CLK, SPI_MISO, SPI_MOSI,
SPI_CS) routed through the FMC connector. Connect a USB-to-serial cable to the
ZedBoard UART (J14, 115200 baud) to view output.

+-----------+---------------------------+---------------------+---------------------+
| Signal    | Description               | ZedBoard MIO Pin    | EV-AD77681FMCZ Pin  |
+-----------+---------------------------+---------------------+---------------------+
| SPI_CLK   | Serial Clock              | P16                 | P6                  |
+-----------+---------------------------+---------------------+---------------------+
| SPI_MISO  | Master In Slave Out       | P17                 | P5                  |
+-----------+---------------------------+---------------------+---------------------+
| SPI_MOSI  | Master Out Slave In       | P21                 | P4                  |
+-----------+---------------------------+---------------------+---------------------+
| SPI_CS    | Chip Select               | P1                  | P3                  |
+-----------+---------------------------+---------------------+---------------------+

Build Command
^^^^^^^^^^^^^

The Xilinx platform uses the CMake/Ninja build system via the ``no_os_build.py``
helper script. Available variants: ``demo``, ``spi_engine_offload``.
Available boards: ``zed``.

For toolchain setup and prerequisites, see the
`Xilinx CMake build guide <https://analogdevicesinc.github.io/no-OS/build_guides/build_xilinx_cmake.html>`__.

.. code-block:: bash

   # Source the Vitis environment (adjust path for your installation):
   source ~/.xilinx/2025.1/Vitis/settings64.sh
   # PowerShell (Windows) equivalent:
   #   & "$env:XILINX_VITIS\settings64.bat"

   cd no-OS

   # Build the demo example on zed
   python tools/scripts/no_os_build.py build \
      --project ad7768-1fmcz --variant demo --board zed \
      --hardware /path/to/ad77681evb_zed/system_top.xsa

   # Build the SPI Engine offload example on zed
   python tools/scripts/no_os_build.py build \
      --project ad7768-1fmcz --variant spi_engine_offload --board zed \
      --hardware /path/to/ad77681evb_zed/system_top.xsa

   # Build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project ad7768-1fmcz --variant demo --board zed \
      --hardware /path/to/ad77681evb_zed/system_top.xsa \
      --probe openocd --flash
