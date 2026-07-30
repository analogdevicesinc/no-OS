ADV7511 no-OS Example Project
=============================

.. no-os-doxygen::

.. contents:: Table of Contents
   :depth: 3

Supported Evaluation Boards
---------------------------

* ADV7511 HDMI transmitter design on ZedBoard

Overview
--------

The ADV7511 project demonstrates HDMI video and audio output using an ADV7511
HDMI transmitter connected to a Xilinx ZedBoard design. The application
initializes the ADV7511 transmitter library, configures the HDMI video path,
sets the initial video mode to 640x480, and allows the output resolution to be
changed from the UART console.

The project uses the no-OS Xilinx platform I2C, IRQ, timer and AXI helpers, plus
the AXI DMAC and AXI clock generator drivers used by the HDL design.

External ADV7511 API Dependency
-------------------------------

The ADV7511 transmitter API source code is provided by the Analog Devices
``ADV7511_API_Library.exe`` package:

`ADV7511 API Library <https://www.analog.com/media/en/dsp-hardware-software/software-modules/ADV7511_API_Library.exe>`__

Before building, extract the package and copy the ``TX`` directory from its
``Src`` directory into ``projects/adv7511``. The expected layout is:

.. code-block:: text

   projects/adv7511/
   |-- TX/
   |   |-- HAL/
   |   `-- LIB/
   |-- src/
   `-- CMakeLists.txt

No-OS Supported Examples
------------------------

Demo Example
~~~~~~~~~~~~

The ``demo`` variant initializes the HDMI transmitter and starts the video and
audio paths. The UART console accepts numeric keys ``0`` through ``6`` to select
one of the supported output resolutions.

No-OS Supported Platforms
-------------------------

Xilinx
~~~~~~

Used Hardware
^^^^^^^^^^^^^

* ADV7511 HDMI transmitter design
* `ZedBoard <https://www.xilinx.com/products/boards-and-kits/1-8dyf-11.html>`_

Connections
^^^^^^^^^^^

Use the HDL design exported as ``adv7511_zed.xsa``. Connect the ZedBoard UART to
the host PC for console output and resolution control. Connect the HDMI output to
a display that supports the selected video mode.

Build Command
^^^^^^^^^^^^^

The Xilinx platform uses the CMake/Ninja build system via the
``no_os_build.py`` helper script. Available variants: ``demo``. Available
boards: ``zed``.

For toolchain setup and prerequisites, see the
`Xilinx CMake build guide <https://analogdevicesinc.github.io/no-OS/build_guides/build_xilinx_cmake.html>`__.

.. code-block:: bash

   # Source the Vitis environment (adjust path to your installation)
   source /path/to/Vitis/settings64.sh

   cd no-OS

   # Copy the external ADV7511 API TX directory first, then build the demo
   python tools/scripts/no_os_build.py build \
      --project adv7511 --variant demo --board zed \
      --hardware /path/to/adv7511_zed.xsa

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project adv7511 --variant demo --board zed \
      --hardware /path/to/adv7511_zed.xsa \
      --flash
