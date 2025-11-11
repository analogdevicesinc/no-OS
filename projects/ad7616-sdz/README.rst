AD7616-SDZ no-OS Example Project
================================

.. contents::
    :depth: 3

Supported Evaluation Boards
---------------------------

- :adi:`EVAL-AD7616SDZ`

Overview
--------

The EVAL-AD7616SDZ evaluation board is designed to assess the
performance of the AD7616 and AD7616-P dual, 16-bit, simultaneous
sampling ADCs. These devices support up to 16 bipolar input channels
with independently selectable ranges and operate from a single 5V
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

- Power line monitoring
- Protective relays
- Multiphase motor control
- Instrumentation and control systems
- Data acquisition systems

Hardware Specifications
------------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-AD7616SDZ evaluation board requires an external power supply
via a DC barrel connector (J7) supporting 7V to 9V input to generate
VCC and VDRIVE voltages. Optional external supplies can be connected
through the VSUPPLY (5V to 10V) and VLOGIC (2.3V to 3.6V) connectors
for analog and digital supply rails, respectively. Correct link
configuration (as per Table 3 in the user guide) is necessary before
powering the board in any mode, whether standalone or controlled 
by a controller board.

On-board Connector
~~~~~~~~~~~~~~~~~~

========= ===================================================
Connector Function
J1        Analog inputs (V0A to V3A)
J2        Analog inputs (V4A to V7A)
J3        Analog inputs (V0A to V3B)
J4        Analog inputs (V4B to V7B)
J5        Digital input/output pins for debug/standalone mode
J6        External reference SMA input
J7        External power connector, 7V to 9V DC input
J8        External VDRIVE power connector
J9        External VCC power connector
J10       120-way connector for the EVAL-SDP-CB1Z
========= ===================================================

No-OS Build Setup
-----------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
`Project Data Source Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad7616-sdz/src>`__

Basic example
~~~~~~~~~~~~~

The basic example code in the AD7616 no-OS project showcases the setup
and data acquisition capabilities of the AD7616 ADC. Execution begins
with enabling instruction and data caches. The hardware components are
initialized using structures like ``spi_engine_init_param``,
``axi_clkgen_init``, and ``no_os_pwm_init_param`` to configure SPI
communication, clock generation, and PWM for data conversions. The
``ad7616_setup()`` function initializes the AD7616 with these
parameters. Data sampling varies based on the mode:
``ad7616_read_data_parallel()`` or ``ad7616_read_data_serial()`` is used
for parallel or serial mode, respectively. Data from channels A and B
are printed, and caches are disabled post-capture. This example
demonstrates the foundational operations of initializing and running the
AD7616 in a no-OS environment.

No-OS Supported Platforms
-------------------------

Xilinx
~~~~~~~

Hardware Used
^^^^^^^^^^^^^

- **EVAL-AD7616SDZ**: Evaluation board for the AD7616 ADC
- **ZedBoard**: Xilinx Zynq-7000 development board
- **SDP-I-FMC Interposer**: Interface board for connecting the EVAL-AD7616SDZ to the ZedBoard

Connections
^^^^^^^^^^^

1. Connect the EVAL-AD7616SDZ to the SDP-I-FMC interposer using the provided headers or cables.
2. Insert the SDP-I-FMC interposer into the FMC connector on the ZedBoard.
3. Set the logic level (VIO) on the ZedBoard to 3.3V.
4. Power the ZedBoard using its 12V DC barrel jack.
5. Provide a separate 6V to 12V DC power supply to the EVAL-AD7616SDZ via its power jack.
6. Ensure both boards share a common ground connection to avoid communication issues.

.. note::

    - The VADJ voltage level on the carrier board must be set to 3.3V when using the SDP-I-FMC interposer.
    - Depending on the desired interface mode, the following hardware modifications are required:

      - For **Parallel Interface**: Ensure SL5 is unmounted.
      - For **Serial Interface**: Ensure SL5 is mounted.

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   cp <SOME_PATH>/system_top.xsa .
   # to delete current build
   make reset
   # to build the project
   make
   # to flash the code
   make run
