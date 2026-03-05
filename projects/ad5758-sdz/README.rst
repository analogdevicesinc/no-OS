AD5758-SDZ no-OS Example Project
=================================

.. no-os-doxygen::

.. contents::
	:depth: 3

Supported Evaluation Boards
---------------------------

* `EVAL-AD5758SDZ <https://www.analog.com/EVAL-AD5758SDZ>`_

Overview
--------

The `EVAL-AD5758SDZ <https://www.analog.com/EVAL-AD5758SDZ>`_ is an
evaluation board for the `AD5758 <https://www.analog.com/AD5758>`_,
a single-channel, 16-bit digital-to-analog converter with dynamic power
control and HART connectivity. The board uses an ADP1031-1 power
management unit to generate the necessary supply voltages for the AD5758
DAC. The AD5758 communicates over a 4-wire SPI interface capable of
operating at up to 50 MHz, with optional CRC error checking and a
watchdog timer. It also integrates a 12-bit ADC for internal diagnostics
and includes VIOUT line protection. When paired with the ADP1031-1,
the AD5758 enables the development of an isolated eight-channel analog
output module with sub-2 W power dissipation.

Applications
------------

* Process control
* Actuator control
* Channel isolated analog outputs
* Programmable logic controller (PLC) and distributed control systems
  (DCS)
* HART network connectivity

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-AD5758SDZ evaluation board uses an ADP1031-1 power management
unit to generate the required voltage outputs for the AD5758 DAC. The
board requires a 26.7 V supply for VCC, 15 V for AVM, and +/-14.5 V
for AVS. A 3.3 V supply can be connected to the VIO pin via JP1, while
JP2 connects the VAUX input through the ADP1031-1 PMU. The AD5758
operates within a power supply range from -33 V to +33 V (AVSS) and
supports up to a 60 V operating voltage differential between AVDD1 and
AVSS rails.

On-Board Connectors
~~~~~~~~~~~~~~~~~~~~

=============== =============== ================================================
Jumper          Link Position   Function
=============== =============== ================================================
JP1             B               Selects VOUT3 pin of ADP1031-1, or ties AVSS
                                to GND for unipolar supply option
JP2             Inserted        Connects VLOGIC pin of AD5758 to SVDD1 pin of
                                ADP1031-1
JP3             A               Selects 3.3 V output from SDP-S to MVDD of
                                ADP1031-1, or 3.3 V input via EXT+3.3 V header
JP4             A               Connects LDAC pin to GND, or to VLOGIC pin
JP5             A               Selects VOUT2 of ADP1031-1 for ADR4525 input,
                                or VLDO pin
JP6             Not inserted    Shorts VDPC+ pin to AVDD1 pin, bypassing
                                positive dc-to-dc circuitry
JP7             A               Connects AD0 pin to GND, or to VLOGIC pin
JP8             A               Connects AD1 pin to GND, or to VLOGIC pin
JP9             Inserted        Connects return signal to GND
JP10            B               Selects ADR4525 output for REFIN pin, or REFOUT
                                pin of AD5758
JP11            Inserted        Various configurations based on application
                                requirements
=============== =============== ================================================

No-OS Build Setup
-----------------

Please see: `No-OS Build Guide <https://wiki.analog.com/resources/no-os/build>`_

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
`Project Source Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad5758-sdz/src/app>`_

Application Example
~~~~~~~~~~~~~~~~~~~~

This is a simple example that initializes the AD5758 DAC using
``ad5758_init()`` with default parameters for SPI communication, GPIO
settings, CRC, and DC-DC converter mode. Once initialized, it writes
a mid-scale value (0x7FFF) to the DAC input register using
``ad5758_dac_input_write()``, setting the output to half scale on the
V1_OUT pin (5 V for the 0 V to 10 V range).

No-OS Supported Platforms
-------------------------

Xilinx
~~~~~~

Hardware Used
^^^^^^^^^^^^^

* `EVAL-AD5758SDZ <https://www.analog.com/EVAL-AD5758SDZ>`_
* `ZedBoard <https://www.amd.com/en/products/adaptive-socs-and-fpgas/evaluation-boards/zedboard-zynq-7000.html>`_

Connections
^^^^^^^^^^^

* Connect the EVAL-AD5758SDZ board to the ZedBoard using the FMC-LPC
  connector.
* Connect a USB cable to the ZedBoard USB-UART port for serial
  communication.
* The EVAL-AD5758SDZ generates its required voltages internally via the
  onboard ADP1031-1 power management unit. Power is supplied from the
  ZedBoard FMC connector.

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
