AD7616-SDZ no-OS Example Project
================================

.. no-os-doxygen::

.. contents::
	:depth: 3

Supported Evaluation Boards
----------------------------

* :adi:`EVAL-AD7616SDZ`

Overview
--------

The EVAL-AD7616SDZ evaluation board is designed to assess the
performance of the AD7616 and AD7616-P dual, 16-bit, simultaneous
sampling ADCs. These devices support up to 16 bipolar input channels
with independently selectable ranges and operate from a single 5 V
analog supply. Key features include high input impedance, analog input
clamp protection, a first-order antialiasing filter, and an accurate
on-chip reference. They offer up to 1 MSPS throughput per channel pair
with 90.5 dB SNR, and up to 92 dB SNR with digital oversampling. The
AD7616 supports both serial and parallel interfaces, while the AD7616-P
supports only parallel. Additional capabilities include a flexible
channel sequencer, burst mode, CRC, and compatibility with various
digital communication protocols. The board is ideal for evaluating the
ADCs in applications such as power-line monitoring, motor control,
instrumentation, and data acquisition systems.

Applications
-------------

* Power line monitoring
* Protective relays
* Multiphase motor control
* Instrumentation and control systems
* Data acquisition systems

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-AD7616SDZ evaluation board requires an external power supply
via a DC barrel connector (J7) supporting 7 V to 9 V input to generate
VCC and VDRIVE voltages. Optional external supplies can be connected
through the VSUPPLY (5 V to 10 V) and VLOGIC (2.3 V to 3.6 V) connectors
for analog and digital supply rails, respectively. Correct link
configuration (as per Table 3 in the user guide) is necessary before
powering the board in any mode, whether standalone or controlled
by a controller board.

On-board Connectors
~~~~~~~~~~~~~~~~~~~

=========  ===================================================
Connector  Function
=========  ===================================================
J1         Analog inputs (V0A to V3A)
J2         Analog inputs (V4A to V7A)
J3         Analog inputs (V0B to V3B)
J4         Analog inputs (V4B to V7B)
J5         Digital input/output pins for debug/standalone mode
J6         External reference SMA input
J7         External power connector, 7 V to 9 V DC input
J8         External VDRIVE power connector
J9         External VCC power connector
J10        120-way connector for the EVAL-SDP-CB1Z
=========  ===================================================

No-OS Build Setup
-----------------

Please see: `No-OS Build Guide <https://wiki.analog.com/resources/no-os/build>`_

No-OS Supported Examples
-------------------------

The initialization data used in the examples is taken out from:
`Project Data Source Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad7616-sdz/src>`_

Demo Example
~~~~~~~~~~~~

The demo example initializes the AD7616 ADC using SPI Engine, clock
generation, and PWM-triggered conversions. Data sampling varies based
on the interface mode: ``ad7616_read_data_parallel()`` or
``ad7616_read_data_serial()`` is used for parallel or serial mode,
respectively. Data from channels A and B are printed on the serial
interface.

No-OS Supported Platforms
--------------------------

Xilinx Platform
~~~~~~~~~~~~~~~

**Used Hardware**

* `EVAL-AD7616SDZ <https://www.analog.com/EVAL-AD7616SDZ>`_
* `ZedBoard <https://www.analog.com/en/resources/reference-designs/powering-zynq-evaluation-development-board-zedboard.html>`_
* SDP-I-FMC interposer board

**Connections**

* Connect the EVAL-AD7616SDZ to the SDP-I-FMC interposer using the
  provided headers or cables.
* Insert the SDP-I-FMC interposer into the FMC connector on the
  ZedBoard.
* Set the logic level (VIO) on the ZedBoard to 3.3 V.
* Power the ZedBoard using its 12 V DC barrel jack.
* Provide a separate 7 V to 9 V DC power supply to the EVAL-AD7616SDZ
  via its power jack (J7).
* Ensure both boards share a common ground connection to avoid
  communication issues.

.. note::

   * The VADJ voltage level on the carrier board must be set to 3.3 V
     when using the SDP-I-FMC interposer.
   * Depending on the desired interface mode, the following hardware
     modifications are required:

     * For **Parallel Interface**: Ensure SL5 is unmounted.
     * For **Serial Interface**: Ensure SL5 is mounted.

**Build Command**

.. code-block:: bash

   cp <SOME_PATH>/system_top.xsa .
   # to delete current build
   make reset
   # to build the project
   make
   # to flash the code
   make run
