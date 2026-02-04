AD5758-SDZ no-OS Example Project
=================================

.. no-os-doxygen::
.. contents::
    :depth: 3

Supported Evaluation Boards
---------------------------

- :adi:`EVAL-AD5758SDZ`

Overview
--------

The EVAL-AD5758SDZ is an evaluation board for the AD5758 DAC, a
single-channel, 16-bit digital-to-analog converter with dynamic power
control and HART connectivity. It operates with a 26.7V Vcc, 15V AVM,
and ±14.5V AVS while maintaining functionality across a −33V (minimum)
to +33V (maximum) power supply range on AVSS and supporting up to a
maximum 60V operating voltage. The board communicates with a host PC
via the :adi:`EVAL-SDP-CS1Z` USB interface, utilizing ACE software for
simplified control. The AD5758 utilizes a versatile 4-wire SPI capable
of operating at 50 MHz with standard SPI, and features optional CRC and
a watchdog timer, alongside enhanced diagnostics such as an integrated
12-bit ADC and VIOUT line protection. When paired with the ADP1031-1, the
AD5758 enables the development of an isolated eight-channel analog
output module with sub-2 W power dissipation, compliant with CISPR 11
Class B. 
CISPR 11 Class B is an electromagnetic compatibility (EMC) standard 
that lays out the limits for radiated and conducted emissions for 
industrial, scientific, and medical (ISM) devices.

Applications
------------

- Process control
- Actuator control
- Channel isolated analog outputs
- Programmable logic controller (PLC) and distributed control systems
  (DCS) applications
- HART network connectivity

Note that the HART protocol in the context of the EVAL-AD5758SDZ 
setup allows communication with field instruments in a 4-20 mA 
current loop, integrating digital communication into analog systems 
to facilitate remote diagnostics and configuration.

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-AD5758SDZ evaluation board requires a specific power supply
configuration for optimal performance. It uses an ADP1031-1 power
management unit to generate necessary voltage outputs for the AD5758
DAC. The board demands a 26.7V supply for Vcc, 15V for AVM, and ±14.5V
for AVS. Additionally, a 3.3V supply can be connected to the Vio pin
via JP1, while JP2 connects the Vaux input through the ADP1031-1 PMU.
The AD5758 works within a power supply range from −33V to +33V and up
to a 60V operating voltage differential between AVDD1 and AVSS rails.
This configuration supports dynamic power control and HART connectivity,
offering diagnostic features and broad operational capabilities.

On-board Connectors
~~~~~~~~~~~~~~~~~~~

+-----------------------+-----------------------+-----------------------+
| Connector             | Link Position         | Function              |
+-----------------------+-----------------------+-----------------------+
| JP1                   | B                     | Select VOUT3 pin of   |
|                       |                       | ADP1031-1, or AVSS    |
|                       |                       | pin to GND for        |
|                       |                       | unipolar supply       |
|                       |                       | option                |
+-----------------------+-----------------------+-----------------------+
| JP2                   | Inserted              | Connects VLOGIC pin   |
|                       |                       | of AD5758 to SVDD1    |
|                       |                       | pin of ADP1031-1      |
+-----------------------+-----------------------+-----------------------+
| JP3                   | A                     | Selects 3.3 V output  |
|                       |                       | from SDP-S to MVDD of |
|                       |                       | ADP1031-1, or 3.3 V   |
|                       |                       | input via EXT+3.3 V   |
|                       |                       | header                |
+-----------------------+-----------------------+-----------------------+
| JP4                   | A                     | Connects LDAC pin to  |
|                       |                       | GND, or to VLOGIC pin |
+-----------------------+-----------------------+-----------------------+
| JP5                   | A                     | Selects VOUT2 of      |
|                       |                       | ADP1031-1 for ADR4525 |
|                       |                       | input, or VLDO pin    |
+-----------------------+-----------------------+-----------------------+
| JP6                   | Not inserted          | Shorts VDPC+ pin to   |
|                       |                       | AVDD1 pin, bypassing  |
|                       |                       | positive dc-to-dc     |
|                       |                       | circuitry             |
+-----------------------+-----------------------+-----------------------+
| JP7                   | A                     | Connects AD0 pin to   |
|                       |                       | GND, or to VLOGIC pin |
+-----------------------+-----------------------+-----------------------+
| JP8                   | A                     | Connects AD1 pin to   |
|                       |                       | GND, or to VLOGIC pin |
+-----------------------+-----------------------+-----------------------+
| JP9                   | Inserted              | Connects return       |
|                       |                       | signal to GND         |
+-----------------------+-----------------------+-----------------------+
| JP10                  | B                     | Selects ADR4525       |
|                       |                       | output for REFIN pin, |
|                       |                       | or REFOUT pin of      |
|                       |                       | AD5758                |
+-----------------------+-----------------------+-----------------------+
| JP11                  | Inserted              | Various               |
|                       |                       | configurations based  |
|                       |                       | on application        |
|                       |                       | requirements          |
+-----------------------+-----------------------+-----------------------+

No-OS Build Setup
-----------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
`Project Source Data Path: <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad5758-sdz/src/app>`__

Application example
~~~~~~~~~~~~~~~~~~~

The application example on the EVAL-AD5758SDZ board configures the
AD5758 DAC using an executable application for SPI and GPIO
communication setup. This involves enabling instruction and data caches
and invoking ``ad5758_init()`` with default parameters to initialize the
DAC for SPI communication, GPIO settings, CRC features, and the DC-DC
converter mode. Once initialized, the example writes a mid-scale value
to the DAC using ``ad5758_dac_input_write()``, illustrating the setting
of output voltage or current levels. Internal settings are utilized for
diagnostics, ensuring operational accuracy and error handling through
integrated diagnostics. 

No-OS Supported Platforms
-------------------------

Xilinx
~~~~~~

Hardware Used
^^^^^^^^^^^^^

- EVAL-AD5758-SDZ Board
- ZedBoard

Connections
^^^^^^^^^^^

- Use the FMC-LPC connector on the ZedBoard. The EVAL-AD5758-SDZ is
  designed to mate with FMC-compatible expansion headers, providing
  access to control and data lines via the FMC interface.

- Carefully insert the EVAL-AD5758-SDZ FMC connector into the ZedBoard’s
  FMC-LPC connector.

- The EVAL-AD5758-SDZ generates its required voltages internally via an
  onboard ADP1031-1 Power Management Unit. Power is supplied from the
  ZedBoard’s FMC connector.

- No additional bench power supply is required if the ZedBoard FMC port
  is used and properly supplies power.

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
