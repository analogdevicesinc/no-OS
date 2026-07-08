AD7606x no-OS Example Project
==============================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
---------------------------

* `EVAL-AD7606B-FMCZ <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-ad7606b-fmcz.html>`_
* `EVAL-AD7606CFMCZ <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-ad7606c-18.html>`_

Overview
--------

The EVAL-AD7606BFMCZ is a full featured evaluation board that allows users to
easily evaluate the features of the AD7606B analog-to-digital converter (ADC).

The EVAL-AD7606CFMCZ is a fully featured evaluation board that allows users to
evaluate the features of the AD7606C-18 and AD7606C-16 analog-to-digital
converters (ADCs).

These boards operate in conjunction with the ZedBoard. The evaluation software
is provided to enable the user to perform detailed analysis of the AD7606B or
AD7606C-16/AD7606C-18's performance. The AD7606B and AD7606C are 8-channel,
simultaneous sampling, 16-bit ADCs with a serial or parallel interface.
The devices support input ranges up to ±10 V and include on-chip oversampling
filters and a 2.5 V reference.

Applications
------------

* Industrial process control
* Data acquisition systems
* Motor control
* Power line monitoring
* Medical instrumentation

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-AD7606B-FMCZ and EVAL-AD7606CFMCZ boards are powered via the FMC
connector from the carrier board (ZedBoard). No separate external supply is
required when correctly connected to a supported carrier. The FMC connector
provides the necessary supply rails to the evaluation board.

On-board Connectors
~~~~~~~~~~~~~~~~~~~

==============================  =====================================================
Connector                       Description
==============================  =====================================================
FMC Connector                   LPC FMC interface to carrier board (ZedBoard)
J1 to J8                        Analog input channels (differential pairs)
SPI / Parallel Interface        Serial or parallel digital interface pins
GPIO                            Configuration and control signals
==============================  =====================================================

No-OS Supported Examples
------------------------

The initialization data used in the examples can be found in:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/blob/main/projects/ad7606x-fmc/src/common_data.c>`_

This contains data for both Serial and Parallel interfaces.

Depending on how the ``system_top.xsa`` was built (with Serial or Parallel
interface), the driver will adapt to the correct interface. The driver will
look for certain macro definitions which are generated only for parallel or
serial interfaces.

Basic Example
~~~~~~~~~~~~~

This is a simple example that initializes the AD7606x, collects a number of
samples and prints the result on the UART console.

IIO Example
~~~~~~~~~~~

This project launches an IIOD server on the board so that the user may connect
to it via an IIO client. Using IIO Oscilloscope, the user can read samples from
the device in real time.

If you are not familiar with ADI IIO Application, please take a look at:
:dokuwiki:`IIO No-OS </resources/tools-software/no-os-software/iio>`

If you are not familiar with ADI IIO-Oscilloscope Client, please take a look at:
:dokuwiki:`IIO Oscilloscope </resources/tools-software/linux-software/iio_oscilloscope>`

No-OS Supported Platforms
--------------------------

Xilinx Platform
~~~~~~~~~~~~~~~

Used Hardware
^^^^^^^^^^^^^

* `EVAL-AD7606B-FMCZ <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-ad7606b-fmcz.html>`_
* `EVAL-AD7606CFMCZ <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-ad7606c-18.html>`_
* `ZedBoard <https://www.analog.com/en/resources/reference-designs/powering-zynq-evaluation-development-board-zedboard.html>`_

Connections
^^^^^^^^^^^

Mount the EVAL-AD7606B-FMCZ or EVAL-AD7606CFMCZ evaluation board onto the
ZedBoard via the FMC LPC connector. The FMC connector on the evaluation board
connects directly to the ZedBoard FMC-LPC slot. Connect a micro-USB cable from
the ZedBoard USB-UART port (J14) to your PC for serial console output
(115200 baud, 8N1).

Build Command
^^^^^^^^^^^^^

The Xilinx platform uses the CMake/Ninja build system via the
``no_os_build.py`` helper script. Available variants: ``basic_example``,
``iio_example``. Available boards: ``zed``.

For toolchain setup and prerequisites, see the
`Xilinx CMake build guide <https://analogdevicesinc.github.io/no-OS/build_guides/build_xilinx_cmake.html>`__.

.. code-block:: bash

   # source the Vitis environment (adjust path to your installation)
   source /path/to/Vitis/settings64.sh
   # PowerShell (Windows) equivalent:
   #   & "C:\path\to\Vitis\settings64.bat"

   cd no-OS

   # build the basic example on the ZedBoard
   python tools/scripts/no_os_build.py build \
      --project ad7606x-fmc --variant basic_example --board zed \
      --hardware /path/to/ad7606x_fmc_zed/system_top.xsa

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project ad7606x-fmc --variant basic_example --board zed \
      --hardware /path/to/ad7606x_fmc_zed/system_top.xsa \
      --probe openocd --flash
