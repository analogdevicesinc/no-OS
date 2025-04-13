Evaluating the EVAL-ADES1754
============================

.. no-os-doxygen::

.. contents::
	:depth: 3

Supported Evaluation Boards
---------------------------

`EVAL-ADES1754 <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-ades1754.html>`_

Overview
--------

The ADES1754 evaluation kit (EV kit) is used to
demonstrate the features and capabilities of the
ADES1754/ADES1755/ADES1756 14-channel, high-
voltage, smart sensor, data-acquisition interface ICs. The
ADES1754 EV kit is coupled with a MAX17851 EV kit to
establish communication with a host MCU.

Hardware Specifications
-----------------------

MAX17851EVKIT
-------------

In case the SPI-to-UART bridge example is to be run, an external
MAX17851EVKIT board is also needed.

`MAX17851EVKIT <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/max17851evkit.html>`_

Power Supply Requirments
^^^^^^^^^^^^^^^^^^^^^^^^

For this specific project, an external 12V PSU is needed in order
to simulate the voltage over the voltage cells.

A 3V3 supply is needed for the SHDNL pin of the ADES1754.

Also a 5V supply and a 3V3 supply are needed for UART and SPI communication,
in case the MAX17851EVKIT board, or only a 3V3 supply in case of the
default UART communication.

Board Connector
^^^^^^^^^^^^^^^

Default UART
------------

**Pin Description**

	Please see the following table for the pin assignments for
	the Hex Inverter and the EVAL-ADES1754, respectively:

	+-----------+-------------------------------------------------+
	|   Name    |	Description				      |
	+-----------+-------------------------------------------------+
	|  VCC_3V3  |   Connect to VCC pin of Hex Inverter	      |
	+-----------+-------------------------------------------------+
	|  GND      |   Connect to GND pin			      |
	+-----------+-------------------------------------------------+
	|  IN1+     |   Connect to TX Host Signal and RXP	      |
	+-----------+-------------------------------------------------+
	|  IN1-	    |   Connect to RXN	      			      |
	+-----------+-------------------------------------------------+
	|  IN2+	    |   Connect to RX Host Signal and TXP	      |
	+-----------+-------------------------------------------------+
	|  IN2-	    |   Float or Connected to TXN 	      	      |
	+-----------+-------------------------------------------------+

	+-----------+-------------------------------------------------+
	|   Name    |	Description				      |
	+-----------+-------------------------------------------------+
	|  VCC_3V3  |   Connect to SHDNL pin			      |
	+-----------+-------------------------------------------------+
	|  GND      |   Connect to AGND pin			      |
	+-----------+-------------------------------------------------+
	|  RXN      |   Connect to inverted TX Host Signal	      |
	+-----------+-------------------------------------------------+
	|  RXP	    |   Connect to positive TX Host Signal	      |
	+-----------+-------------------------------------------------+
	|  TXN	    |   Float or connectec to IN2-		      |
	+-----------+-------------------------------------------------+
	|  TXP	    |   Connect to positive RX Host Signal	      |
	+-----------+-------------------------------------------------+

SPI-to-UART
-----------

	**Pin Description**

	Please see the following table for the pin assignments for
	the MAX17851EVKIT and the EVAL-ADES1754, respectively:

	+-----------+-------------------------------------------------+
	|   Name    |	Description				      |
	+-----------+-------------------------------------------------+
	|  VCC_3V3  |   Connect to DCIN2 pin of MAX17851EVKIT	      |
	+-----------+-------------------------------------------------+
	|  VCC_5V   |   Connect to DCIN1 pin of MAX17851EVKIT	      |
	+-----------+-------------------------------------------------+
	|  GND      |   Connect to GND pin of MAX17851EVKIT	      |
	+-----------+-------------------------------------------------+
	|  CS	    |   Connect to SPI Chip Select(CS) 		      |
	+-----------+-------------------------------------------------+
	|  MOSI	    |   Connect to SPI Master-Out-Slave-In(MOSI)      |
	+-----------+-------------------------------------------------+
	|  MISO	    |   Connect to SPI Master-In-Slave-Out(MISO)      |
	+-----------+-------------------------------------------------+
	|  SCK	    |   Connect to SPI Clock(SCK) 	      	      |
	+-----------+-------------------------------------------------+

	+-----------+-------------------------------------------------+
	|   Name    |	Description				      |
	+-----------+-------------------------------------------------+
	|  VCC_3V3  |   Connect to SHDNL pin			      |
	+-----------+-------------------------------------------------+
	|  GND      |   Connect to AGND pin			      |
	+-----------+-------------------------------------------------+
	|  RXN      |   Connect to TXN signal of MAX17851EVKIT	      |
	+-----------+-------------------------------------------------+
	|  RXP	    |   Connect to TXP signal of MAX17851EVKIT	      |
	+-----------+-------------------------------------------------+
	|  TXN	    |   Connect to RXN signal of MAX17851EVKIT        |
	+-----------+-------------------------------------------------+
	|  TXP	    |   Connect to RXP signal of MAX17851EVKIT	      |
	+-----------+-------------------------------------------------+

No-OS Build Setup
-----------------

Please see: `No-OS Build Wiki <https://wiki.analog.com/resources/no-os/build>`_

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ades1754/src/common>`_

The macros used for Common Data structures are defined in platform specific files found at:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ades1754/src/platform>`_

BASIC Example
^^^^^^^^^^^^^

Steps of the examples:

* Initializes the ADES1754
* Reads Cell Voltage for each Cell.
* Scans for Alerts.
* In case any alerts are detected the example is stopped.
* If the example is stopped a specific message is shown on the Std I/O UART.

In order to build the default uart example make sure you have the following
configuration in the Makefile:
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ades1754/Makefile>`_

.. code-block:: bash

	EXAMPLE ?= def_uart_example

SPI-to-UART Bridge Example
^^^^^^^^^^^^^^^^^^^^^^^^^^

Steps of the examples:

* Specific structures for the example are defined.
* MAX17851 then the ADES1754 are intialized.
* Reads Cell Voltage for each Cell.
* Scans for Alerts.
* In case any alerts are detected the example is stopped.
* If the example is stopped a specific message is shown on the Std I/O UART.

In order to build the spi uart example make sure you have the following
configuration in the Makefile:
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ades1754/Makefile>`_

.. code-block:: bash

	EXAMPLE ?= spi_uart_example

No-OS Supported Platforms
-------------------------

Maxim Platform
^^^^^^^^^^^^^^

**Used Hardware**

* `EVAL-ADES1754 <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-ades1754.html>`_
* `AD-APARD32690-SL <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/ad-apard32690-sl.html>`_
* `Standard Industry Hex Inverter (Default UART Example)`
* `MAX17851EVKIT (SPI UART Example) <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/max17851evkit.html>`_

+--------------+---------------------------------+-------------------+------------------+----------------------+
| ADES1754 Pin | Function			 | Hex Inverter	     | MAX17851 Pin	| AD-APARD32690-SL Pin |
+--------------+---------------------------------+-------------------+------------------+----------------------+
| -	       | Low Voltage Power Supply	 | -		     | DCIN1		| 5V0 Supply	       |
+--------------+---------------------------------+-------------------+------------------+----------------------+
| -	       | Low Voltage Power Supply	 | VCC		     | DCIN2		| 3V3 Supply	       |
+--------------+---------------------------------+-------------------+------------------+----------------------+
| SHDNL	       | Low Voltage Power Supply	 | -                 | -                | 3V3 Supply           |
+--------------+---------------------------------+-------------------+------------------+----------------------+
| AGND         | Ground				 | GND		     | GND		| GND		       |
+--------------+---------------------------------+-------------------+------------------+----------------------+
| RXP          | Receive Line(+) for UART slave  | IN1+		     | TXP (Master)     | P2_14                |
+--------------+---------------------------------+-------------------+------------------+----------------------+
| RXN          | Receive Line(-) for UART slave  | IN1-              | TXN (Master)     | -                    |
+--------------+---------------------------------+-------------------+------------------+----------------------+
| TXP          | Transmit Line(+) for UART slave | IN2+              | RXP (Master)     | P2_16		       |
+--------------+---------------------------------+-------------------+------------------+----------------------+
| TXN          | Transmit Line(-) for UART slave | IN2-              | RXN (Master)     | -                    |
+--------------+---------------------------------+-------------------+------------------+----------------------+
| -            | SPI Chip-Select                 | -                 | SPI-CS (PORT0)   | P1_0		       |
+--------------+---------------------------------+-------------------+------------------+----------------------+
| -            | SPI Master-Out-Slave-In         | -                 | SPI-MOSI (PORT0) | P1_1                 |
+--------------+---------------------------------+-------------------+------------------+----------------------+
| -            | SPI Master-In-Slave-Out         | -           	     | SPI_MISO (PORT0) | P1_2                 |
+--------------+---------------------------------+-------------------+------------------+----------------------+
| -            | SPI Clock                       | -                 | SPI-SCK (PORT0)  | P1_3                 |
+--------------+---------------------------------+-------------------+------------------+----------------------+

**Build Command**

Default UART Example
--------------------

.. code-block:: bash

	# to delete current build
	make PLATFORM=maxim TARGET=max32690 EXAMPLE=def_uart_example reset
	# to build the project and flash the code
	make PLATFORM=maxim TARGET=max32690 EXAMPLE=def_uart_example run

SPI UART Example
----------------

.. code-block:: bash

	# to delete current build
	make PLATFORM=maxim TARGET=max32690 EXAMPLE=spi_uart_example reset
	# to build the project and flash the code
	make PLATFORM=maxim TARGET=max32690 EXAMPLE=spi_uart_example run
