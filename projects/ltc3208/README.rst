Evaluating the LTC3208
======================


Supported Evaluation Boards
---------------------------

* DC824A (LTC3208)


Overview
--------

The LTC3208 is a highly integrated multidisplay LED
controller. The part contains a 1A high efficiency, low noise
charge pump to provide power to the MAIN, SUB, RGB, CAM
and AUX LED displays. The LTC3208 requires only small
ceramic capacitors and one current set resistor to form a
complete LED power supply and current controller

The maximum display currents are set by a single external resistor.
Current for each LED is controlled by a precision internal
current source. Dimming and On/Off for all displays is achieved
via the I2C serial interface.
256 brightness levels are available for the MAIN and SUB displays.
16 levels are available for the RGB and CAM displays.
Four AUX current sources can be independently
assigned via the I2C port to the CAM, SUB, MAIN or AUX
DAC controlled displays.

Power Supply Requirements
-------------------------

A seperate power supply capable of providing 3V. The logic signals used for the I2C protocol
uses 3.3V via the controller as a logic high reference.

**Pin Description**:

+-----+-----------+-------------------------------------------+
| Pin | Name      | Description                               |
+-----+-----------+-------------------------------------------+
| 1   | CAM1      | Camera Display Current Source Output 1    |
+-----+-----------+-------------------------------------------+
| 2   | CAM2      | Camera Display Current Source Output 2    |
+-----+-----------+-------------------------------------------+
| 3   | CAM3      | Camera Display Current Source Output 3    |
+-----+-----------+-------------------------------------------+
| 4   | CAM4      | Camera Display Current Source Output 4    |
+-----+-----------+-------------------------------------------+
| 5   | AUX1      | Auxillary Display Current Source Output 1 |
+-----+-----------+-------------------------------------------+
| 6   | AUX2      | Auxillary Display Current Source Output 2 |
+-----+-----------+-------------------------------------------+
| 7   | AUX3      | Auxillary Display Current Source Output 3 |
+-----+-----------+-------------------------------------------+
| 8   | AUX4      | Auxillary Display Current Source Output 4 |
+-----+-----------+-------------------------------------------+
| 9   | CAMHL     | CAM High Input                            |
+-----+-----------+-------------------------------------------+
| 10  | SCL       | I2C Clock Input                           |
+-----+-----------+-------------------------------------------+
| 11  | SDA       | I2C Data Input                            |
+-----+-----------+-------------------------------------------+
| 12  | VBAT3     | Supply Voltage                            |
+-----+-----------+-------------------------------------------+
| 14  | DVCC      | Supply Voltage for all Digital I/O Lines  |
+-----+-----------+-------------------------------------------+
| 15  | MAIN1     | Main Display Current Source Output 1      |
+-----+-----------+-------------------------------------------+
| 16  | MAIN2     | Main Display Current Source Output 2      |
+-----+-----------+-------------------------------------------+
| 17  | MAIN3     | Main Display Current Source Output 3      |
+-----+-----------+-------------------------------------------+
| 18  | MAIN4     | Main Display Current Source Output 4      |
+-----+-----------+-------------------------------------------+
| 19  | SUB2      | Sub Display Current Source Output 2       |
+-----+-----------+-------------------------------------------+
| 20  | SUB1      | Sub Display Current Source Output 1       |
+-----+-----------+-------------------------------------------+
| 21  | BLUE      | Blue LED Current Source Output            |
+-----+-----------+-------------------------------------------+
| 22  | GRN       | Green LED Current Source Output           |
+-----+-----------+-------------------------------------------+
| 23  | RED       | Red LED Current Source Output             |
+-----+-----------+-------------------------------------------+
| 24  | VBAT2     | Supply Voltage                            |
+-----+-----------+-------------------------------------------+
| 25  | GND       | Ground Pad                                |
+-----+-----------+-------------------------------------------+
| 28  | ENRGBS    | RGB Input                                 |
+-----+-----------+-------------------------------------------+
| 30  | VBAT1     | Supply Voltage                            |
+-----+-----------+-------------------------------------------+
| 32  | CPO       | Charge Pump Output                        |
+-----+-----------+-------------------------------------------+
| 33  | GND       | Ground Pad                                |
+-----+-----------+-------------------------------------------+

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build


No-OS Supported Platforms
-------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ltc3208/src/common>`_

The macros used in Common data are defined in platfor-specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ltc3208/src/platform>`_

Basic example
^^^^^^^^^^^^^

This is a comprehensive example that demonstrates the full functionality of the
LTC3208 multidisplay LED controller.

The example runs continuously in a loop, cycling through all LED configurations
with timing delays to visually demonstrate each feature.

In order to build the basic example make sure you have the following
configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ltc3208/Makefile>`_

.. code-block:: bash

	EXAMPLE = basic_example

Maxim Platform
--------------

**Used Hardware**

* `DC824A <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/dc824a.html>`_
* `MAX32655FTHR <https://www.analog.com/MAX32655FTHR>`_ or `MAX32690EVKIT <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/MAX32690EVKIT.html>`_

**Connections**:

+----------+----------------------+----------------------+
| LTC3208  | MAX32655FTHR         | MAX32690EVKIT        |
+----------+----------------------+----------------------+
| V_IN     | Power Supply         | Power Supply         |
+----------+----------------------+----------------------+
| V_CC     | 3v3                  | 3v3                  |
+----------+----------------------+----------------------+
| GND      | GND                  | GND                  |
+----------+----------------------+----------------------+
| I2C_SCL  | P0_30                | P2_8                 |
+----------+----------------------+----------------------+
| I2C_SDA  | P0_31                | P2_7                 |
+----------+----------------------+----------------------+
| ENRGBS   | P1_9                 | P0_14                |
+----------+----------------------+----------------------+

Build Command
^^^^^^^^^^^^^

Available variants: ``basic``.
Available boards: ``ad-apard32690-sl``, ``max32655fthr``.
Replace ``--variant`` / ``--board`` accordingly.

.. code-block:: bash

   export MAXIM_LIBRARIES=</path/to/MaximSDK/Libraries>

   cd no-OS

   # build the project (basic example on the ad-apard32690-sl board)
   python tools/scripts/no_os_build.py build \
      --project ltc3208 --variant basic --board ad-apard32690-sl

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project ltc3208 --variant basic --board ad-apard32690-sl \
      --probe openocd --flash
