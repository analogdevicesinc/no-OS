PULSAR-ADC no-OS Example Project
==================================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
----------------------------

* `EVAL-AD400x-FMCZ <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/EVAL-AD400x-FMCZ.html>`_

Overview
--------

The AD4000/AD4001/AD4002/AD4003/AD4020 family of evaluation boards enable quick,
simplified evaluation of the AD4000 family of 16-/18-/20-bit, precision
successive approximation register (SAR) analog-to-digital converters (ADCs).

The AD4000/AD4001/AD4002/AD4003/AD4020 are low power, 16-bit/18-bit/20-bit,
precision SAR ADCs that offer high performance with throughputs up to 2 MSPS
(1.8 MSPS for the AD4020). The evaluation board demonstrates the performance
of the AD4000/AD4001/AD4002/AD4003/AD4020 family of ADCs and provides a
simplified interface for a variety of system applications.

Applications
------------

* Industrial automation technology
* Instrumentation and measurement solutions
* Intelligent building solutions
* Healthcare solutions
* Aerospace and defense systems
* Wireless communication solutions

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-AD400x-FMCZ receives power through the FMC connector from the attached
FPGA carrier board, or via the SDP connector when used with an SDP-K1. No
external power supply is required.

No-OS Supported Examples
-------------------------

This project is organized around the no-OS variant based build flow.
Selecting a variant at build time (``--variant <name>``) chooses which
application is compiled. The platform ``main()`` is a thin dispatcher that
calls ``example_main()``, provided by the selected example. Shared
initialization data is defined in
`src/common <https://github.com/analogdevicesinc/no-OS/tree/main/projects/pulsar-adc/src/common>`__,
and platform-specific macros and extra init parameters are in
`src/platform <https://github.com/analogdevicesinc/no-OS/tree/main/projects/pulsar-adc/src/platform>`__.

Basic Example
~~~~~~~~~~~~~

The basic example initializes the AD400x ADC and prints sampled data out to
the UART console. On Xilinx, the SPI Engine offload core is used for
high-speed data capture.

This example is built by selecting the ``basic`` variant (see the Build Command
sections below).

IIO Example
~~~~~~~~~~~

This project is an IIOD demo for the EVAL-AD400x evaluation board. The project
launches an IIOD server on the board so that the user may connect to it via an
IIO client. Using IIO-Oscilloscope, the user can configure the ADC and view the
measured data on a plot.

If you are not familiar with ADI IIO Application, please take a look at:
:dokuwiki:`IIO No-OS </resources/tools-software/no-os-software/iio>`

If you are not familiar with ADI IIO-Oscilloscope Client, please take a
look at:
:dokuwiki:`IIO Oscilloscope </resources/tools-software/linux-software/iio_oscilloscope>`

This example is built by selecting the ``iio`` variant (see the Build Command
sections below).

No-OS Supported Platforms
--------------------------

Xilinx
~~~~~~

Used Hardware
^^^^^^^^^^^^^

* `EVAL-AD400x-FMCZ <https://www.analog.com/eval-ad400x-fmcz.html>`_
* `ZedBoard <https://www.analog.com/en/resources/reference-designs/powering-zynq-evaluation-development-board-zedboard.html>`_

Connections
^^^^^^^^^^^

Connect the EVAL-AD400x-FMCZ to the ZedBoard via the FMC connector. On Xilinx,
the SPI Engine offload core is used; the device part in use can be configured in
``src/common/common_data.c`` via the ``dev_id`` field of the
``ad400x_init_param`` struct (for example ``ID_AD4020``). The UART console
appears on the ZedBoard USB-UART adapter at **115200 baud, 8N1**.

Build Command
^^^^^^^^^^^^^

The Xilinx platform uses the CMake/Ninja build system via the
``no_os_build.py`` helper script. Available variants: ``basic``, ``iio``.
Available boards: ``zed``.

A Xilinx hardware description file (``.xsa``) generated from the matching HDL
design is required. The HDL design name for all variants is ``pulsar_adc`` (see
``CONFIG_XILINX_HDL_DESIGN`` in each ``.conf`` file). Obtain the pre-built
``pulsar_adc_zed/system_top.xsa`` from the ADI HDL repository releases, or
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

   # build the basic example on the ZedBoard (requires a .xsa hardware file)
   python tools/scripts/no_os_build.py build \
      --project pulsar-adc --variant basic --board zed \
      --hardware /path/to/pulsar_adc_zed/system_top.xsa

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project pulsar-adc --variant basic --board zed \
      --hardware /path/to/pulsar_adc_zed/system_top.xsa \
      --probe openocd --flash

STM32
~~~~~

Used Hardware
^^^^^^^^^^^^^

* `EVAL-AD400x-FMCZ <https://www.analog.com/eval-ad400x-fmcz.html>`_
* `SDP-K1 <https://www.analog.com/SDP-K1>`_
* ST debugger

Connections
^^^^^^^^^^^

Connect the EVAL-AD400x-FMCZ to the SDP-K1 via the SDP connector. The
UART console appears on the SDP-K1 USB port at **115200 baud, 8N1**.

Build Command
^^^^^^^^^^^^^

The STM32 platform uses the CMake/Ninja build system via the
``no_os_build.py`` helper script. Available variants: ``basic``, ``iio``.
Available boards: ``sdp-ck1z``.

For toolchain setup and prerequisites, see the
`STM32 CMake build guide <https://analogdevicesinc.github.io/no-OS/build_guides/build_stm32_cmake.html>`__.

.. code-block:: bash

   # set the path to STM32CubeMX and STM32CubeIDE (only if they are not
   # in a default install location)
   export STM32CUBEMX=</path/to/stm32cubemx>
   export STM32CUBEIDE=</path/to/stm32cubeide>
   # PowerShell (Windows) equivalent:
   #   $env:STM32CUBEMX = "<path\to\stm32cubemx>"
   #   $env:STM32CUBEIDE = "<path\to\stm32cubeide>"

   cd no-OS

   # build the basic example on the SDP-K1 board
   python tools/scripts/no_os_build.py build \
      --project pulsar-adc --variant basic --board sdp-ck1z

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project pulsar-adc --variant basic --board sdp-ck1z \
      --probe openocd --flash
