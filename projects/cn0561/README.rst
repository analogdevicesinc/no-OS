CN0561 no-OS Example Project
=============================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
----------------------------

* `EVAL-CN0561-ARDZ <https://www.analog.com/en/resources/reference-designs/circuits-from-the-lab/cn0561.html>`_

Overview
--------

The EVAL-CN0561-ARDZ is a data acquisition system tailored for vibration
analysis, primarily utilizing IEPE sensors. Designed to evaluate and forecast
mechanical health, it employs a precise 24-bit AD4134 ADC capable of
continuous-time sampling at 1.496 MSPS. Supplementing this is the LTC6373, a
differential output instrumentation amplifier, and the LT3092, a programmable
two-terminal current source, enhancing data processing efficiency. Its
compatibility with Arduino systems facilitates integration with diverse
platforms, offering robust fault detection for increased system reliability.

Applications
------------

* Data Acquisition for IEPE Sensors
* Temperature Monitoring
* Strain Measurement
* Shock Detection
* Displacement Sensing
* Condition-Based Monitoring

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-CN0561-ARDZ requires a DC power supply input ranging from 9V to 12V.
The board is designed to draw power from a microcontroller or development board
via Arduino-style or FMCZ-type connectors. It handles an initial inrush current
of up to 200 mA for power-up, while steady state operation needs around 250 mA.
The power system within the board generates necessary operational voltages: 5V
and 1.8V for the AD4134 ADC, and +/-15V for the instrumentation amplifier and
current source circuits. The LT3092 component provides a constant current of
2.5 mA to IEPE sensors. To accommodate potential power fluctuations, a power
source capable of delivering 1.5A consistently is recommended.

On-board Connectors
~~~~~~~~~~~~~~~~~~~~

================= ========================= =====================================
Connector         Type                      Description
================= ========================= =====================================
P1, P2, P4, P5    Arduino Connectors        General data interfacing with the
                                            evaluation board
P17               Arduino Zio Connector     Supports additional Arduino
                                            expansions
P3                FMCZ Connector            Used for high-speed data acquisition
P6, P7, J6, J8    SMA Connectors            Interface for IEPE sensor connections
                                            and analog signals
P8                Analog Input              Primary input for data acquisition
                                            processes
P9                External Power Input      Supports 9V to 12V DC power for
                                            board operation
P14               External Supply           Provides +/-15V for analog front-end
                                            capabilities
J5, J7            Additional SMA            Amenities for further analog channels
                  Connectors
P11, P12          Optional Connectors       To accommodate extra analog inputs
                                            as required
================= ========================= =====================================

No-OS Supported Examples
-------------------------

This project is organized around the no-OS variant based build flow.
Selecting a variant at build time (``--variant <name>``) chooses which
application is compiled. The platform ``main()`` is a thin dispatcher that
calls ``example_main()``, provided by the selected example. Shared
initialization data is defined in
`src/common <https://github.com/analogdevicesinc/no-OS/tree/main/projects/cn0561/src>`__,
and platform-specific macros and extra init parameters are in
`src/platform <https://github.com/analogdevicesinc/no-OS/tree/main/projects/cn0561/src/platform>`__.

UART Example
~~~~~~~~~~~~

The UART example initializes the CN0561 (AD4134 quad-channel ADC) via the AXI
SPI Engine offload core with DMA, captures samples and prints the results over
UART at 115200 baud. The carrier board is selected at build time through the
variant: use ``uart_zed`` for ZedBoard or ``uart_cora`` for Cora Z7-07S.

IIO Example
~~~~~~~~~~~

The IIO example launches an IIOD server on the board so that the user may
connect to it via an IIO client. Using the IIO Oscilloscope application, users
can configure the AD4134 ADC for data acquisition, using DMA to transfer ADC
data to memory and converting raw data into voltage readings accessible via the
IIO interface. The carrier board is selected at build time through the variant:
use ``iio_zed`` for ZedBoard or ``iio_cora`` for Cora Z7-07S.

If you are not familiar with ADI IIO Application, please take a look at:
:dokuwiki:`IIO No-OS </resources/tools-software/no-os-software/iio>`

If you are not familiar with ADI IIO-Oscilloscope Client, please take a
look at:
:dokuwiki:`IIO Oscilloscope </resources/tools-software/linux-software/iio_oscilloscope>`

No-OS Supported Platforms
--------------------------

Xilinx
~~~~~~

Used Hardware
^^^^^^^^^^^^^

* `EVAL-CN0561-ARDZ <https://www.analog.com/en/resources/reference-designs/circuits-from-the-lab/cn0561.html>`_
* `ZedBoard <https://digilent.com/shop/zedboard-zynq-7000-arm-fpga-soc-development-board/>`_ (Zynq-7000)
  **or** `Cora Z7-07S <https://digilent.com/shop/cora-z7-zynq-7000-single-core-for-arm-fpga-soc-development/>`_ (Zynq-7000)

Connections
^^^^^^^^^^^

**ZedBoard**

Connect the EVAL-CN0561-ARDZ to the ZedBoard via the FMC connector (P3) for
data and power. Connect IEPE sensors to the CN0561 SMA connectors. The ZedBoard
supplies 9V to 12V power to the evaluation board via the FMC connector. Connect
the ZedBoard USB-UART adapter to the host PC.

The UART console appears on the ZedBoard USB-UART adapter at **115200 baud, 8N1**.

**Cora Z7-07S**

Plug the EVAL-CN0561-ARDZ directly onto the Cora Z7-07S Arduino/chipKIT shield
connector. Do not power the CN0561 through the shield; use the Cora Z7S main
power input only. Observe the following constraints:

==================== ===================================================
Parameter            Value
==================== ===================================================
I/O Voltage Limit    Max 3.4V (absolute max 3.75V)
Analog Input Range   Differential: 0V to 1V; Single-ended: 0V to 3.3V
                     (scaled via resistor divider)
Power Input          Cora Z7S main input only; do not power via shield
==================== ===================================================

The UART console appears on the Cora Z7S USB-UART adapter at **115200 baud, 8N1**.

Build Command
^^^^^^^^^^^^^

The Xilinx platform uses the CMake/Ninja build system via the
``no_os_build.py`` helper script. Available variants and their corresponding
boards:

+-------------------+-----------------------------------------------+
| Variant           | Board / Carrier                               |
+===================+===============================================+
| ``uart_zed``      | ZedBoard (``zed``)                            |
+-------------------+-----------------------------------------------+
| ``uart_cora``     | Cora Z7-07S (``coraz7s``)                     |
+-------------------+-----------------------------------------------+
| ``iio_zed``       | ZedBoard (``zed``)                            |
+-------------------+-----------------------------------------------+
| ``iio_cora``      | Cora Z7-07S (``coraz7s``)                     |
+-------------------+-----------------------------------------------+

A Xilinx hardware description file (``.xsa``) generated from the matching HDL
design is required. The HDL design name for all variants is ``cn0561`` (see
``CONFIG_XILINX_HDL_DESIGN`` in each ``.conf`` file). Obtain the pre-built
``.xsa`` from the ADI HDL repository releases, or build it yourself by following
the `Building HDL guide <https://wiki.analog.com/resources/fpga/docs/build>`_.

For toolchain setup and prerequisites, see the
`Xilinx CMake build guide <https://analogdevicesinc.github.io/no-OS/build_guides/build_xilinx_cmake.html>`__.

.. code-block:: bash

   # source the Vitis environment (adjust path to your Vitis install)
   source ~/.xilinx/2025.1/Vitis/settings64.sh
   # PowerShell (Windows) equivalent:
   #   & "$env:USERPROFILE\.xilinx\2025.1\Vitis\settings64.bat"

   cd no-OS

   # build the UART example for ZedBoard
   python tools/scripts/no_os_build.py build \
      --project cn0561 --variant uart_zed --board zed \
      --hardware /path/to/cn0561_zed/system_top.xsa

   # build and flash for ZedBoard (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project cn0561 --variant uart_zed --board zed \
      --hardware /path/to/cn0561_zed/system_top.xsa \
      --probe openocd --flash

   # build the IIO example for Cora Z7-07S
   python tools/scripts/no_os_build.py build \
      --project cn0561 --variant iio_cora --board coraz7s \
      --hardware /path/to/cn0561_coraz7s/system_top.xsa

   # build and flash for Cora Z7-07S (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project cn0561 --variant iio_cora --board coraz7s \
      --hardware /path/to/cn0561_coraz7s/system_top.xsa \
      --probe openocd --flash
