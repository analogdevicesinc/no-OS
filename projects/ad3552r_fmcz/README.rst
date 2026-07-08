EVAL-AD3552R no-OS Example Project
===================================

.. no-os-doxygen::

.. contents:: Table of Contents
   :depth: 3

Supported Evaluation Boards
---------------------------

* `EVAL-AD3552RFMCZ <https://www.analog.com/EVAL-AD3552RFMCZ>`_

Overview
--------

The EVAL-AD3552RFMCZ Evaluation Board is designed for the dual-channel,
16-bit AD3552R digital-to-analog converter (DAC). It features two
channels with distinct transimpedance amplifiers — one for dynamic
performance and one for DC precision over temperature. The board
supports comprehensive testing across all DAC outputs, waveform
generation, and offers various power supply and reference options.
Connectivity is facilitated through either the SDP-H1 system
demonstration platform or via a pin header for ZedBoard or other
controllers.

Applications
------------

* Instrumentation
* Hardware in the loop
* Process control equipment
* Medical devices
* Automated test equipment
* Data acquisition systems
* Programmable voltage sources
* Optical communications

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

To power the EVAL-AD3552RFMCZ board, connect it using either a
controller board or external power supplies via connector P3.

Pin Assignment on Power Supply Connector P3
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

+-----------------------+-----------------------+-----------------------+
| Pin Number            | Signal                | Description           |
+-----------------------+-----------------------+-----------------------+
| 1                     | EXT_PVDD              | External positive     |
|                       |                       | supply for            |
|                       |                       | transconductance      |
|                       |                       | amplifier. 12V ± 5%.  |
+-----------------------+-----------------------+-----------------------+
| 2                     | EXT_PVSS              | External negative     |
|                       |                       | supply for            |
|                       |                       | transconductance      |
|                       |                       | amplifier. −12V ± 5%  |
+-----------------------+-----------------------+-----------------------+
| 3                     | EXT_VDD               | External AVDD analog  |
|                       |                       | supply for AD3552R.   |
|                       |                       | 5V ± 5%.              |
+-----------------------+-----------------------+-----------------------+
| 4                     | EXT_DVDD              | External DVDD digital |
|                       |                       | supply for AD3552R.   |
|                       |                       | 1.8V ± 5%.            |
+-----------------------+-----------------------+-----------------------+
| 5                     | EXT_VLOGIC            | External VLOGIC       |
|                       |                       | digital I/O supply    |
|                       |                       | for AD3552R. 1.1V to  |
|                       |                       | 1.9V.                 |
+-----------------------+-----------------------+-----------------------+
| 6                     | GND                   | Ground.               |
+-----------------------+-----------------------+-----------------------+

Digital Signals
~~~~~~~~~~~~~~~

Pin Assignment on Connector P5
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

+-------------+--------------+---------------------------------------------------------------+
| Pin Number  | Signal       | Description                                                   |
+=============+==============+===============================================================+
| 1           | +12V_FMC     | External 12 V power supply. Use this pin to supply the        |
|             |              | EVAL-AD3552RFMCZ board when using a custom controller         |
|             |              | different from the SDP-H1 or the ZedBoard.                    |
+-------------+--------------+---------------------------------------------------------------+
| 2           | POWER_ON_FMC | Enable signal for the onboard regulators. This pin is used to |
|             |              | turn on the LDOs and DC/DC converters. Set a voltage higher   |
|             |              | than 1.24 V to turn on.                                       |
+-------------+--------------+---------------------------------------------------------------+
| 3           | VIO          | Voltage supply used for AD3552R I/O pins. If this pin is      |
|             |              | used to supply VLOGIC, remove the VLOGIC link or set it to    |
|             |              | EXT.                                                          |
+-------------+--------------+---------------------------------------------------------------+
| 4           | SPI_QPI1     | Signal SPI_QPI. A high level sets the bus in Quad SPI mode.   |
|             |              | A low level sets the bus in classic SPI mode.                 |
+-------------+--------------+---------------------------------------------------------------+
| 5           | GND          | Ground.                                                       |
+-------------+--------------+---------------------------------------------------------------+
| 6           | SPI_CS1      | SPI chip select signal.                                       |
+-------------+--------------+---------------------------------------------------------------+
| 7           | SPI_SCK1     | SPI clock signal.                                             |
+-------------+--------------+---------------------------------------------------------------+
| 8           | SPI_SDI1     | SPI data input signal.                                        |
+-------------+--------------+---------------------------------------------------------------+
| 9           | GND          | Ground.                                                       |
+-------------+--------------+---------------------------------------------------------------+
| 10          | SPI_SDIO01   | SDO/MISO signal in Classic SPI mode or SDIO0 signal in        |
|             |              | Dual/Quad SPI modes.                                          |
+-------------+--------------+---------------------------------------------------------------+
| 11          | GND          | Ground.                                                       |
+-------------+--------------+---------------------------------------------------------------+
| 12          | SPI_SDIO11   | SDO/MISO signal in Classic SPI mode or SDIO1 signal in        |
|             |              | Dual/Quad SPI modes.                                          |
+-------------+--------------+---------------------------------------------------------------+
| 13          | GND          | Ground.                                                       |
+-------------+--------------+---------------------------------------------------------------+
| 14          | SPI_SDIO21   | SPI SDIO2 signal in Quad SPI mode.                            |
+-------------+--------------+---------------------------------------------------------------+
| 15          | GND          | Ground.                                                       |
+-------------+--------------+---------------------------------------------------------------+
| 16          | SPI_SDIO31   | SPI SDIO3 signal in Quad SPI mode.                            |
+-------------+--------------+---------------------------------------------------------------+
| 17          | GND          | Ground.                                                       |
+-------------+--------------+---------------------------------------------------------------+
| 18          | /LDAC1       | LDAC signal.                                                  |
+-------------+--------------+---------------------------------------------------------------+
| 19          | /RESET1      | RESET signal.                                                 |
+-------------+--------------+---------------------------------------------------------------+
| 20          | /ALERT1      | ALERT signal.                                                 |
+-------------+--------------+---------------------------------------------------------------+

No-OS Build Setup
-----------------

Please see: `No-OS Build Guide <https://wiki.analog.com/resources/no-os/build>`__

No-OS Supported Examples
------------------------

This project uses the no-OS variant-based build flow. Selecting a variant
at build time (``--variant <name>``) chooses which application is compiled.
Shared initialization data is defined in
`src/common <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad3552r_fmcz/src/common>`__,
and platform-specific macros are in
`src/platform <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad3552r_fmcz/src/platform>`__.

AXI QSPI Example
~~~~~~~~~~~~~~~~

The ``axi_qspi`` variant drives the AD3552R DAC via the AXI QSPI interface
with AXI DMA and AXI clock generator IP. It demonstrates hardware
initialization (GPIOs, SPI, DAC), raw sample data transfer to the DAC,
and cyclic DMA-based sine wave generation using the ``no_os_sine_lut_16``
lookup table.

AXI QSPI IIO Example
~~~~~~~~~~~~~~~~~~~~

The ``axi_qspi_iio`` variant extends the AXI QSPI example with an IIOD
server, exposing the AD3552R DAC through the IIO framework via ``iio_ad3552r``
and ``iio_axi_dac``. A host can connect using any libiio-compatible client
to configure DAC attributes and trigger waveform sequences.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`__

If you are not familiar with ADI IIO-Oscilloscope Client, please take a
look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`__

Generic SPI Example
~~~~~~~~~~~~~~~~~~~

The ``generic_spi`` variant drives the AD3552R DAC using the Zynq PS SPI
controller (no AXI offload). It initializes GPIOs and SPI, configures the
DAC, and streams a sine wave lookup table to both channels using cyclic DMA.

Generic SPI IIO Example
~~~~~~~~~~~~~~~~~~~~~~~

The ``generic_spi_iio`` variant extends the generic SPI example with an
IIOD server, exposing the AD3552R DAC through the IIO framework via
``iio_ad3552r``. A host can connect using any libiio-compatible client to
configure DAC attributes and control waveform output.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`__

If you are not familiar with ADI IIO-Oscilloscope Client, please take a
look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`__

No-OS Supported Platforms
-------------------------

Xilinx
~~~~~~

Used Hardware
^^^^^^^^^^^^^

* `EVAL-AD3552RFMCZ <https://www.analog.com/EVAL-AD3552RFMCZ>`_
* `ZedBoard <https://www.analog.com/en/resources/reference-designs/powering-zynq-evaluation-development-board-zedboard.html>`_

Connections
^^^^^^^^^^^

The EVAL-AD3552RFMCZ board interfaces with the ZedBoard via connector P5,
which handles both digital signal transmission and power requirements.

* Connect the EVAL-AD3552RFMCZ board into the ZedBoard FMC connector.
* Connect USB UART J14 (Micro USB) to your host PC.
* Plug your ethernet cable into the RJ45 ethernet connector (J11).
* Plug the Power Supply into the 12V Power input connector (J20).

For power, utilize the +12V_FMC pin and activate the POWER_ON_FMC pin to
enable the board's regulators. I/O voltage is selected through the VIO pin.
The SPI bus can operate in Quad mode or classic configuration controlled by
the SPI_QPI pin. Proper initialization requires setting GPIO_RESET_N and
GPIO_LDAC_N pins high at startup.

+-----------------------+-----------------------+-----------------------+
| Pin Function          | Pin Name              | Description           |
+-----------------------+-----------------------+-----------------------+
| SPI Mode Selector     | SPI_QPI1              | High level sets to    |
|                       |                       | Quad SPI; low level   |
|                       |                       | sets to Classic SPI.  |
+-----------------------+-----------------------+-----------------------+
| SPI Chip Select       | SPI_CS1               | SPI chip select       |
|                       |                       | signal.               |
+-----------------------+-----------------------+-----------------------+
| SPI Clock             | SPI_SCK1              | SPI clock signal.     |
+-----------------------+-----------------------+-----------------------+
| SPI Data Input        | SPI_SDI1              | SPI data input        |
|                       |                       | signal.               |
+-----------------------+-----------------------+-----------------------+
| SPI SDIO Bit 0        | SPI_SDIO01            | SDO/MISO in Classic   |
|                       |                       | SPI or SDIO0 in       |
|                       |                       | Dual/Quad mode.       |
+-----------------------+-----------------------+-----------------------+
| SPI SDIO Bit 1        | SPI_SDIO11            | SDO/MISO in Classic   |
|                       |                       | SPI or SDIO1 in       |
|                       |                       | Dual/Quad mode.       |
+-----------------------+-----------------------+-----------------------+
| SPI SDIO Bit 2        | SPI_SDIO21            | SDIO2 in Quad SPI     |
|                       |                       | mode.                 |
+-----------------------+-----------------------+-----------------------+
| SPI SDIO Bit 3        | SPI_SDIO31            | SDIO3 in Quad SPI     |
|                       |                       | mode.                 |
+-----------------------+-----------------------+-----------------------+
| Load DAC Data         | /LDAC1                | Load DAC data.        |
+-----------------------+-----------------------+-----------------------+
| Reset                 | /RESET1               | Reset signal.         |
+-----------------------+-----------------------+-----------------------+
| Alert                 | /ALERT1               | Indicates errors or   |
|                       |                       | alerts.               |
+-----------------------+-----------------------+-----------------------+

The UART console appears on the ZedBoard USB-UART port at 115200 baud, 8N1.

Build Command
^^^^^^^^^^^^^

The Xilinx platform uses the CMake/Ninja build system via the
``no_os_build.py`` helper script. Available variants: ``axi_qspi``,
``axi_qspi_iio``, ``generic_spi``, ``generic_spi_iio``.
Available boards: ``zed``.

For toolchain setup and prerequisites, see the
`Xilinx CMake build guide <https://analogdevicesinc.github.io/no-OS/build_guides/build_xilinx_cmake.html>`__.

.. code-block:: bash

   # Source the Vitis environment (sets XILINX_VITIS and adds tools to PATH)
   source /path/to/Vitis/settings64.sh
   # PowerShell (Windows) equivalent:
   #   $env:XILINX_VITIS = "<C:\path\to\Vitis>"

   cd no-OS

   # build the example on the ZedBoard (replace --variant as needed)
   python tools/scripts/no_os_build.py build \
      --project ad3552r_fmcz --variant axi_qspi --board zed \
      --hardware /path/to/system_top.xsa

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project ad3552r_fmcz --variant axi_qspi --board zed \
      --hardware /path/to/system_top.xsa \
      --probe openocd --flash
