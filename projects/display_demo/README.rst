Display Demo no-OS Example Project
====================================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
----------------------------

* SSD1306 OLED Display

Overview
--------

The Display Demo project demonstrates driving an SSD1306 OLED display from a
Xilinx ZedBoard using the no-OS SPI and GPIO drivers. The application
initializes the SSD1306 display controller, powers on the display via dedicated
VDD and VBAT GPIO controls, and prints a text message on screen.

The initialization data used in the example is taken from the
`Project Source Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/display_demo/src/app>`__.

No-OS Supported Examples
------------------------

Demo Example
~~~~~~~~~~~~

This is the default example. It initializes the SSD1306 OLED display over
SPI, controls the display power-enable GPIOs, and prints a message to the
screen on the ZedBoard platform.

This example is built by selecting the ``demo`` variant (see the Build Command
section below).

No-OS Supported Platforms
-------------------------

Xilinx
~~~~~~

Used Hardware
^^^^^^^^^^^^^

* SSD1306 OLED Display
* `ZedBoard <https://www.xilinx.com/products/boards-and-kits/1-8dyf-11.html>`_

Connections
^^^^^^^^^^^

Connect the SSD1306 OLED display to the ZedBoard SPI and GPIO headers. The
display controller communicates over SPI. The VDD and VBAT power-enable lines
are driven by GPIO outputs as configured in ``src/app/parameters.h``. The UART
console appears on the ZedBoard USB-UART adapter at **115200 baud, 8N1**.

Build Command
^^^^^^^^^^^^^

The Xilinx platform uses the CMake/Ninja build system via the
``no_os_build.py`` helper script. Available variants: ``demo``.
Available boards: ``zed``.

For toolchain setup and prerequisites, see the
`Xilinx CMake build guide <https://analogdevicesinc.github.io/no-OS/build_guides/build_xilinx_cmake.html>`__.

.. code-block:: bash

   # source the Vitis environment (adjust path to your Vitis install)
   source /path/to/Vitis/2025.1/settings64.sh
   # PowerShell (Windows) equivalent:
   #   & "<path\to\Vitis\2025.1\settings64.bat"

   cd no-OS

   # build the demo example on the ZedBoard (requires a .xsa hardware file)
   python tools/scripts/no_os_build.py build \
      --project display_demo --variant demo --board zed \
      --hardware /path/to/system_top.xsa

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project display_demo --variant demo --board zed \
      --hardware /path/to/system_top.xsa \
      --probe openocd --flash
