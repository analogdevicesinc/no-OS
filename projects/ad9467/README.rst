AD9467 no-OS Example project
============================

Supported Evaluation Boards
----------------------------

- :adi:`AD9467-FMC-250EBZ`

Overview
---------

The AD9467 evaluation board (AD9467-FMC-250EBZ) is engineered to
facilitate the comprehensive evaluation of the AD9467 analog-to-digital
converter. It offers high-performance data conversion capabilities by
providing leading signal-to-noise ratio (SNR) and spurious-free dynamic
range (SFDR) within a compact design. Notable features include
differential analog inputs optimized for broad bandwidth down
conversion, configurable clock inputs for both single-ended and
differential signals, and a flexible power supply setup. This board is
ideally suited for demanding applications such as instrumentation,
spectrum analysis, and radar systems requiring precise and rapid data
acquisition.

Applications
------------

- Multicarrier, multimode cellular receivers
- Power amplifier linearization
- Broadband wireless
- Radar
- Infrared imaging
- Communications instrumentation
- Antenna array positioning

Hardware Specifications
------------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~~

To power the AD9467 evaluation board with an external power supply,
connect a switching power supply rated for 100V to 240V AC, configured
to output 6 V at up to 2.5 A, directly to the power input jack labeled
P700. Ensure all jumpers, including those for reference settings and
clock input configurations, are correctly placed. Consider using
additional components like COAX cables or terminators if deviating from
standard configurations.

No-OS Build Setup
------------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
-------------------------

The initialization data used in the example is taken out from the
`Project Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad9467/src>`__

IIO Example
-----------

The IIO example code demonstrates the implementation and configuration
of the Industrial I/O subsystem within the no-OS environment.
Specifically, it showcases the setup of AD9467 ADC via its driver,
involving SPI communication, register manipulation, and power mode
management. The code provides functions such as ``ad9467_test_mode`` to
configure operational modes, output current, and clock delay,
illustrating how to control various ADC parameters. Additionally, the
examples include initializing and running IIO applications with
functions like ``iio_app_run``, facilitating real-time data acquisition
and processing, thereby leveraging the modularity of the no-OS
repository to interface efficiently with hardware components through IIO
client application.

If you are not familiar with ADI IIO Application, please take a look at:
:dokuwiki:`IIO No-OS </resources/tools-software/no-os-software/iio>`

In order to build the IIO project, make sure you have the following
configuration in the 
`Makefile <https://github.com/analogdevicesinc/no-OS/blob/main/projects/ad9467/Makefile>`__

.. code-block:: bash

   # Force select an example by assigning y for enabling.
      
   IIO_EXAMPLE = y

No-OS Supported Platforms
--------------------------

Xilinx Platform
~~~~~~~~~~~~~~~~

Hardware Used
^^^^^^^^^^^^^^

- AD9467 Evaluation Board (AD9467-FMC-250EBZ)
- `AMD Kintex™ 7 FPGA KC705 Evaluation Kit <https://www.amd.com/en/products/adaptive-socs-and-fpgas/evaluation-boards/ek-k7-kc705-g.html>`__

Connection Setup
^^^^^^^^^^^^^^^^^

1. Connect the AD9467-FMC-250EBZ board to the KC705 Zedboard using the
   FMC interface connector.

2. Use a USB Type A to mini-B cable to connect the KC705 ZedBoard to the
   host PC via the USB-UART port.

3. Attach the 12 VDC power adapter to the KC705 ZedBoard (via J49).

4. Set the adjustable voltage on the KC705 ZedBoard to supply 2.5V for
   the AD9467 board.

5. Power on the KC705 ZedBoard using switch SW15.

Build Command
^^^^^^^^^^^^^^

.. code-block:: bash

   cp <SOME_PATH>/system_top.xsa .

   # to delete current build
   make reset 
   # to build the project
   make
   # to flash the code
   make run
   # to debug the code
   make debug
