adrv902x no-OS Example Project
================================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Devices
-----------------

* `ADRV9026 <https://www.analog.com/ADRV9026>`_
* `ADRV9029 <https://www.analog.com/ADRV9029>`_

Supported Carriers
------------------

* `ZCU102 <https://www.xilinx.com/ZCU102>`_

Overview
--------

This guide is a source of information for system engineers and software
developers using the Analog Devices, Inc., ADRV902x family of software
defined radio transceivers. This family consists of the ADRV9026
integrated quad RF transceiver and the ADRV9029 integrated quad RF
transceiver with digital predistortion (DPD) and crest factor reduction
(CFR) capability.

The ADRV9025/ADRV9026/ADRV9029 devices feature four transmitters, four
receivers, and dual observation receivers covering a frequency range of
75 MHz to 6 GHz. They integrate JESD204B and JESD204C interfaces for
high-speed digital data transfer and ARM Cortex-M3 based firmware
(Madura API) for device control.

Driver Layout
-------------

The Madura API is located in the no-OS driver directory under:

::

   no-OS/drivers/rf-transceiver/madura/
   ├── adrv9025.c
   ├── adrv9025_conv.c
   ├── adrv9025.h
   ├── common
   │   ├── adi_common.h
   │   ├── adi_common_macros.h
   │   ├── adi_common_types.h
   │   ├── adi_common_user.h
   │   ├── adi_error
   │   └── adi_logging
   ├── devices
   │   └── adrv9025
   └── platforms
       ├── adi_platform.h
       └── adi_platform_types.h

Selecting the JESD Use Case
---------------------------

The JESD use case is selected **automatically from the bitstream** at
configure time - there is normally nothing to edit by hand.
``scripts/xsa_profile.sh`` reads the hardware handoff (``system.hwh``)
inside the ``.xsa`` and reports which firmware profile the design needs:

* ``LINK_MODE 2`` in the handoff selects **JESD204C**, otherwise JESD204B;
* an ``axi_adrv9026_rx_os_jesd_rx`` core means **ORx** is present;
* the main-Rx ``adc_tpl_core`` ``NUM_CHANNELS`` gives the Rx converter count.

``CMakeLists.txt`` uses that result to put exactly one firmware profile
directory on the include path and to derive the build-time settings, so
``#include "ActiveUseCase_profile.h"`` always resolves to the profile that
matches the loaded bitstream:

=========================  =======  ==========================  ====================
JESD link mode (from XSA)  ORx      Firmware profile directory  Compile settings
=========================  =======  ==========================  ====================
JESD204B                   absent   ``JESD204B_no_ORx``         204B
JESD204B                   present  ``JESD204B_ORx``            204B
JESD204C                   present  ``JESD204C_ORx``            ``JESD204C_PROFILE``
=========================  =======  ==========================  ====================

The main-Rx converter count is passed as
``-DADRV9025_RX_JESD_CONVS_PER_DEVICE=<n>`` and the 204C profile also
defines ``JESD204C_PROFILE``; ``app_config.h`` picks up both, so those
axes need no manual editing. If the XSA asks for a profile whose directory
is missing, the configure step stops with an error telling you which one
to generate.

Adding or Updating a Use Case
-----------------------------

You only need these steps to introduce a new profile directory or refresh
an existing one from Madura TES:

#. From the Madura TES GUI, generate the resources folder that contains
   the files listed below:

   * Firmware files (ADRV9025_FW.bin and ADRV9025_DPDCORE_FW.bin),
   * Stream binary (e.g., stream_image_6E3E00EFB74FE7D465FA88A171B81B8F.bin),
   * ActiveUseCase.profile and ActiveUtilInit.profile.

#. no-OS cannot read files at runtime, so the .bin files are committed as
   C arrays. These are shared across all profiles and live at the root of
   ``src/common/firmware/``. Regenerate them only when the firmware or
   stream image itself changes:
   ::

      xxd -i ADRV9025_FW.bin > ADRV9025_FW.h

   Copy the generated unsigned char array into the corresponding header
   file in the
   `project structure <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adrv902x/src/common/firmware>`__
   (ADRV9025_FW.h, ADRV9025_DPDCORE_FW.h or ADRV9025_stream_image.h).

#. Convert the ``.profile`` JSON into the header the firmware expects with
   `profile_to_header.py <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adrv902x/scripts/profile_to_header.py>`__,
   writing it into the profile directory that ``xsa_profile.sh`` selects
   for the target bitstream:
   ::

      scripts/profile_to_header.py ActiveUseCase.profile \
          src/common/firmware/JESD204C_ORx/ActiveUseCase_profile.h

   ``profile_to_header.py`` is whitespace-exact - round-tripping an
   unchanged ``.profile`` reproduces the committed header byte-for-byte,
   so it doubles as a parity check against the Linux ``firmware/*.profile``
   the same bitstream is validated against. (The older ``json2cstring.sh``
   only adds the C-string wrapper for a file literally named
   ``ActiveUseCase.profile``; prefer the Python script for the per-profile
   headers.) ``ActiveUtilInit_profile.h`` is shared and lives at the
   firmware root.

#. Build the project. The profile is selected from the XSA; no source edit
   is required.

No-OS Supported Examples
-------------------------

The demo applications highlight the functionality of the adrv902x
evaluation board. Three example variants are provided in the project:

Basic Example
~~~~~~~~~~~~~

The basic example (variant ``basic_example``) simply initializes the
components on the evaluation board and enables a JESD link. TX will
transmit a DDS waveform with the default parameters set by the DAC
driver. The output looks like the one below:

.. code-block:: bash

   adrv9025-phy Rev 0, API version: 7.0.0.14 found
   tx_adxcvr: OK (9830400 kHz)
   rx_adxcvr: OK (9830400 kHz)
   adrv9025-phy Rev 176, Firmware 6.4.0.6 API version: 7.0.0.14 Stream version: 9.4.0.1 successfully initialized via jesd204-fsm
   tx_jesd status:
       Link is enabled
       Measured Link Clock: 245.778 MHz
       Reported Link Clock: 245.760 MHz
       Lane rate: 9830.400 MHz
       Lane rate / 40: 245.760 MHz
       LMFC rate: 7.680 MHz
       SYNC~: deasserted
       Link status: DATA
       SYSREF captured: Yes
       SYSREF alignment error: No
   rx_jesd status:
       Link is enabled
       Measured Link Clock: 245.778 MHz
       Reported Link Clock: 245.760 MHz
       Lane rate: 9830.400 MHz
       Lane rate / 40: 245.760 MHz
       LMFC rate: 7.680 MHz
       Link status: DATA
       SYSREF captured: Yes
       SYSREF alignment error: No
   Bye

DMA Example
~~~~~~~~~~~

The DMA example (variant ``dma_example``) sends a sine wave on TX
channels using DMA from a lookup table. If you physically loopback a TX
channel to an RX channel via an electrical wire, you may run the DMA
example and read the received data at RX from its particular memory
address.

After the output from the basic example, the application will eventually
print something like this:

.. code-block:: bash

    DMA_EXAMPLE Tx: address=0x1dc900 samples=8192 channels=8 bits=32
    DMA_EXAMPLE Rx: address=0x1e4900 samples=65536 channels=8 bits=16

This means that the memory address where the data at RX is stored is
``0x1e4900``. There are a total of 65536 samples, 16-bit wide across 8
channels, which is equivalent to 8192, 16-bit samples per channel. The
location of the transmitted data is also given (0x1dc900).

At this point you may use a Tcl script to retrieve data from memory and
store it into .csv files for processing:

.. code-block:: bash

   xsct tools/scripts/platform/xilinx/capture.tcl ZYNQ_PSU 0x1e4900 65536 8 16

You can find more information about the data `here <https://wiki.analog.com/resources/no-os/dac_dma_example>`_.

The data in the .csv files generated can be visualised using the
`plot.py <https://github.com/analogdevicesinc/no-OS/blob/master/tools/scripts/platform/xilinx/plot.py>`_
script in the no-OS repository. The following command will display the
data on all 8 channels:

.. code-block:: bash

   python tools/scripts/platform/xilinx/plot.py 8

IIO Example
~~~~~~~~~~~

The IIO example (variant ``iio_example``) launches a IIOD server on the
board so that the user may connect to it via an IIO client. Using
iio-oscilloscope, the user can configure the DAC and view the ADC data
on a plot.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with ADI IIO-Oscilloscope Client, please take a
look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

To run the IIOD demo, connect to the board via UART with the following
settings:

.. code-block:: bash

   Baud Rate: 115200bps
   Data: 8 bit
   Parity: None
   Stop bits: 1 bit
   Flow Control: none

After the link bring-up messages, the application will eventually print:

.. code-block:: bash

   Running IIOD server...
   If successful, you may connect an IIO client application by:
   1. Disconnecting the serial terminal you use to view this message.
   2. Connecting the IIO client application using the serial backend configured as shown:
       Baudrate: 115200
       Data size: 8 bits
       Parity: none
       Stop bits: 1
       Flow control: none

This message implies a IIOD server is being run and you may connect to it
using a serial-backend enabled
`iio-oscilloscope <https://github.com/analogdevicesinc/iio-oscilloscope>`__
with the settings indicated at the serial terminal.

No-OS Supported Platforms
--------------------------

Xilinx
~~~~~~

Used Hardware
^^^^^^^^^^^^^

* `ZCU102 Evaluation Kit <https://www.xilinx.com/ZCU102>`__ (Zynq UltraScale+ MPSoC)
* ADRV9026 or ADRV9029 evaluation board

Connections
^^^^^^^^^^^

Connect the adrv902x evaluation board to the correct FMC connector on
the ZCU102 carrier board before programming it. Connect a USB cable to
the carrier board USB-UART port and the host PC for serial console
access at 115200 baud, 8N1.

Build Command
^^^^^^^^^^^^^

The Xilinx platform uses the CMake/Ninja build system via the
``no_os_build.py`` helper script. Available variants: ``basic_example``,
``dma_example``, ``iio_example``. Available boards: ``zcu102``.

A Xilinx XSA hardware description file is required. The HDL design name
is ``adrv9026``; the hardware name is composed as ``adrv9026_<board>``
(e.g. ``adrv9026_zcu102``).

For toolchain setup and prerequisites, see the
:doc:`Xilinx CMake build guide </build_guides/build_xilinx_cmake>`.

.. code-block:: bash

   # Source the Vitis toolchain environment
   source ~/.xilinx/2025.1/Vitis/settings64.sh
   # PowerShell (Windows) equivalent:
   #   & "$env:USERPROFILE\.xilinx\2025.1\Vitis\settings64.bat"

   cd no-OS

   # Build the basic example for ZCU102
   python tools/scripts/no_os_build.py build \
       --project adrv902x --variant basic_example --board zcu102 \
       --hardware /path/to/adrv9026_zcu102/system_top.xsa

   # Build and flash via JTAG
   python tools/scripts/no_os_build.py build \
       --project adrv902x --variant basic_example --board zcu102 \
       --hardware /path/to/adrv9026_zcu102/system_top.xsa \
       --probe openocd --flash

   # Build the DMA example for ZCU102
   python tools/scripts/no_os_build.py build \
       --project adrv902x --variant dma_example --board zcu102 \
       --hardware /path/to/adrv9026_zcu102/system_top.xsa

   # Build the IIO example for ZCU102
   python tools/scripts/no_os_build.py build \
       --project adrv902x --variant iio_example --board zcu102 \
       --hardware /path/to/adrv9026_zcu102/system_top.xsa
