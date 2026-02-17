Evaluating the EVAL-ADG1736
===========================

.. no-os-doxygen::

.. contents::
	:depth: 3

Supported Evaluation Boards
---------------------------

* `EVAL-ADG1736 <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-adg1736.html>`_

Overview
--------

The ADG1736 is a low voltage, 2.4 Ohm dual SPDT analog switch in a 2mm x 2mm
LGA package. It contains two independently selectable single-pole, double-throw
(SPDT) switches and operates with single supply (+1.08V to +5.5V) or dual supply
(+/-1.08V to +/-2.75V). An EN input is used to disable all switches.

Key specifications:

* On Resistance: 2.4 Ohm typical
* Signal Range: Rail-to-rail
* Logic Levels: 1.8V and 3V JEDEC compliant
* Bandwidth: 300 MHz typical
* Operating Temperature: -40C to +125C

The ADG1736 data sheet provides full specifications and details on device
operation. Consult it in conjunction with this user guide.

Hardware Specifications
-----------------------

Power Supply Requirements
^^^^^^^^^^^^^^^^^^^^^^^^^

The EVAL-ADG1736 can be powered via USB Type-C or external supply:

* Single supply: +1.08V to +5.5V (VDD to VSS)
* Dual supply: +/-1.08V to +/-2.75V

For this project, 3.3V from the MAX32655FTHR is used as VDD.

Board Connector and Jumper Settings
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Before connecting the MCU, configure the EVAL-ADG1736 jumpers.

**Common settings (required for both SW1 and SW2):**

+----------+------------------+-----------------------------------------------+
| Jumper   | Setting          | Description                                   |
+----------+------------------+-----------------------------------------------+
| JP4      | **Remove**       | Disconnect EN from manual control             |
+----------+------------------+-----------------------------------------------+
| JP5      | **Position A**   | Connect VL to VL/EN header                    |
+----------+------------------+-----------------------------------------------+
| P13      | **Insert**       | Connect EN to DIGIO3 for MCU control          |
+----------+------------------+-----------------------------------------------+
| P21      | **Insert**       | Set logic level VL to 3.3V                    |
+----------+------------------+-----------------------------------------------+

**SW1 testing:**

+----------+------------------+-----------------------------------------------+
| Jumper   | Setting          | Description                                   |
+----------+------------------+-----------------------------------------------+
| JP2      | **Remove**       | Disconnect IN1 from manual A/B control        |
+----------+------------------+-----------------------------------------------+
| P11      | **Insert**       | Connect IN1 to DIGIO1 for MCU control         |
+----------+------------------+-----------------------------------------------+

**SW2 testing:**

+----------+------------------+-----------------------------------------------+
| Jumper   | Setting          | Description                                   |
+----------+------------------+-----------------------------------------------+
| JP3      | **Remove**       | Disconnect IN2 from manual A/B control        |
+----------+------------------+-----------------------------------------------+
| P12      | **Insert**       | Connect IN2 to DIGIO2 for MCU control         |
+----------+------------------+-----------------------------------------------+

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-adg1736/src/common>`_

The macros used in Common Data are defined in platform specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-adg1736/src/platform>`_

Basic example
^^^^^^^^^^^^^

This example tests ADG1736 switch routing using 4 GPIOs (P1.6-P1.9):

* Initializes the ADG1736 driver with IN and EN GPIO control
* Toggles the switch between A and B positions
* Sends a test signal through D and reads back on S
* Verifies switch routing with LED feedback (green = pass, red = fail)

**GPIO Configuration:**

* IN1 (P1.6) -> SW1 control: HIGH=A, LOW=B. Wire to P11 (DIGIO1) header
* IN2 (P2.0) -> SW2 control: HIGH=A, LOW=B. Wire to P12 (DIGIO2) header
* EN (P1.7) -> Enable: HIGH=enabled, LOW=disabled. Wire to P13 header
* D (P1.8) -> Test signal output. Connect to D1 or D2 terminal
* S (P1.9) -> Test signal input. Connect to S1A/S1B/S2A/S2B (based on TEST_SIDE)

Note: IN1 and IN2 use separate GPIO pins to allow independent control of both switches.

The test toggles the switch between A and B positions:

* When in the configured TEST_SIDE position, S should read D's value
* When in the opposite position, S should read LOW (disconnected)

**LED feedback:**

* Green: Test passing (switch routing correctly)
* Red: Test failing

The test configuration is set via ``parameters.h``:

* ``TEST_SWITCH``: 0 = SW1, 1 = SW2
* ``TEST_SIDE``: 0 = A side, 1 = B side

**Test Configuration Truth Table:**

IN1 (P1.6) controls SW1, IN2 (P2.0) controls SW2. Each switch has its own
dedicated GPIO pin for independent control.

+------------+-----------+--------+----------------+------------------+
| TEST_SWITCH| TEST_SIDE | Switch | S Connection   | IN Wiring        |
+------------+-----------+--------+----------------+------------------+
| 0          | 0         | SW1    | S1A            | P1.6 to P11      |
+------------+-----------+--------+----------------+------------------+
| 0          | 1         | SW1    | S1B            | P1.6 to P11      |
+------------+-----------+--------+----------------+------------------+
| 1          | 0         | SW2    | S2A            | P2.0 to P12      |
+------------+-----------+--------+----------------+------------------+
| 1          | 1         | SW2    | S2B            | P2.0 to P12      |
+------------+-----------+--------+----------------+------------------+

No-OS Supported Platforms
-------------------------

Maxim Platform
^^^^^^^^^^^^^^

**Used hardware**

* `EVAL-ADG1736 <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-adg1736.html>`_
* `MAX32655FTHR <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/max32655fthr.html>`_

**Power Options**

Option 1: USB-C power (recommended)
  * Connect USB-C cable to P27 on EVAL-ADG1736
  * Set JP1 to position A (connect VDD to EXT_VDD)
  * Insert P17 (connect POS_SUPPLY to 3.3V)
  * Insert P25 (connect NEG_SUPPLY to GND)
  * Connect GND between boards for common reference

Option 2: Power from MAX32655FTHR
  * Set JP1 to position A (connect VDD to EXT_VDD)
  * Connect MAX32655FTHR 3V3 to POS_SUPPLY header
  * Connect MAX32655FTHR GND to NEG_SUPPLY header
  * Insert P19 (connect POS_SUPPLY to VDD)
  * Insert P25 (connect NEG_SUPPLY to GND)

**Signal Connections for SW1, A side test (default configuration)**

+------------------+---------------------------+------------------------------+
| MAX32655FTHR     | EVAL-ADG1736              | Function                     |
+------------------+---------------------------+------------------------------+
| GND              | NEG_SUPPLY header         | Common ground                |
+------------------+---------------------------+------------------------------+
| P1.6             | DIGIO1 (via P11 header)   | IN1 switch control           |
+------------------+---------------------------+------------------------------+
| P1.7             | DIGIO3 (via P13 header)   | EN enable control            |
+------------------+---------------------------+------------------------------+
| P1.8             | D1 terminal               | Test signal output           |
+------------------+---------------------------+------------------------------+
| P1.9             | S1A terminal              | Test signal readback         |
+------------------+---------------------------+------------------------------+

For other test configurations, change the D and S connections per the truth table above:

* SW1, B side: P1.9 to S1B, set ``TEST_SWITCH=0``, ``TEST_SIDE=1``
* SW2, A side: P2.0 to P12, P1.8 to D2, P1.9 to S2A, set ``TEST_SWITCH=1``, ``TEST_SIDE=0``
* SW2, B side: P2.0 to P12, P1.8 to D2, P1.9 to S2B, set ``TEST_SWITCH=1``, ``TEST_SIDE=1``

Note: P1.6 is on J7 header (pin 4), P2.0 is on J9 header (pin 5).

**Build Command**

.. code-block:: bash

	# to delete current build
	make PLATFORM=maxim TARGET=max32655 reset
	# to build the project
	make PLATFORM=maxim TARGET=max32655
	# to flash the code
	make run
