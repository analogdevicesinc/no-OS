Evaluating the LTC3220
======================


Supported Evaluation Boards
---------------------------

* DC1265A-A (LTC3220)
* DC1265A-B (LTC3220-1)


Overview
--------

The LTC3220/LTC3220-1 are highly integrated multi-display LED drivers.
These parts contain a high efficiency, low noise charge pump to provide power
to up to eighteen universal LED current sources.The LED currents are set by an
internal precision current reference. Independent dimming, on/off, blinking and
gradation control for all universal current sources are achieved via the
I2C serial interface. 6-bit linear DACs are available to adjust brightness
levels independently for each universal LED current source.

The LTC3220/LTC3220-1 charge pump optimizes efficiency based on the voltage 
across the LED current sources. The part powers up in 1x mode and will 
automatically switch to boost mode whenever any enabled LED current source 
begins to enter dropout. The first dropout switches the parts into 1.5x mode 
and a subsequent dropout switches the LTC3220/LTC3220-1 into 2x mode.

The LTC3220 maintains an I2C address of 0x1C. Its LTC3220-1 maintains an address
of 0x1D. There is no other difference between the two apart from this.

Power Supply Requirements
-------------------------

A seperate power supply capable of providing 3V, up to 400mA is required for
the device's maximum capabilities. The logic signals used for the I2C protocol
uses 3.3V via the controller as a logic high reference.

**Pin Description**:

+-----+-----------+-------------------------------------------+
| Pin | Name      | Description                               |
+-----+-----------+-------------------------------------------+
| 1   | ULED1     | Current Source Output 1                   |
+-----+-----------+-------------------------------------------+
| 2   | ULED2     | Current Source Output 2                   |
+-----+-----------+-------------------------------------------+
| 3   | ULED3     | Current Source Output 3                   |
+-----+-----------+-------------------------------------------+
| 4   | ULED4     | Current Source Output 4                   |
+-----+-----------+-------------------------------------------+
| 5   | ULED5     | Current Source Output 5                   |
+-----+-----------+-------------------------------------------+
| 6   | ULED6     | Current Source Output 6                   |
+-----+-----------+-------------------------------------------+
| 7   | ULED7     | Current Source Output 7                   |
+-----+-----------+-------------------------------------------+
| 8   | ULED8     | Current Source Output 8                   |
+-----+-----------+-------------------------------------------+
| 9   | ULED9     | Current Source Output 9                   |
+-----+-----------+-------------------------------------------+
| 10  | V_CC      | Digital I/O Supply Voltage                |
+-----+-----------+-------------------------------------------+
| 11  | SCL       | I2C Clock Input                           |
+-----+-----------+-------------------------------------------+
| 12  | SDA       | I2C Data Input                            |
+-----+-----------+-------------------------------------------+
| 13  | ULED10    | Current Source Output 10                  |
+-----+-----------+-------------------------------------------+
| 14  | ULED11    | Current Source Output 11                  |
+-----+-----------+-------------------------------------------+
| 15  | ULED12    | Current Source Output 12                  |
+-----+-----------+-------------------------------------------+
| 16  | ULED13    | Current Source Output 13                  |
+-----+-----------+-------------------------------------------+
| 17  | ULED14    | Current Source Output 14                  |
+-----+-----------+-------------------------------------------+
| 18  | ULED15    | Current Source Output 15                  |
+-----+-----------+-------------------------------------------+
| 19  | ULED16    | Current Source Output 16                  |
+-----+-----------+-------------------------------------------+
| 20  | ULED17    | Current Source Output 17                  |
+-----+-----------+-------------------------------------------+
| 21  | ULED18    | Current Source Output 18                  |
+-----+-----------+-------------------------------------------+
| 24  | V_IN      | Supply Voltage                            |
+-----+-----------+-------------------------------------------+
| 25  | RST       | Active Low Reset Input                    |
+-----+-----------+-------------------------------------------+
| 28  | CPO       | Charge Pump Output                        |
+-----+-----------+-------------------------------------------+
| 29  | GND       | Ground Pad                                |
+-----+-----------+-------------------------------------------+

No-OS Build Setup
-----------------

Please refer to the Analog Wiki: https://wiki.analog.com/resources/no-os/build


No-OS Supported Platforms
-------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ltc3220/src/common>`_

The macros used in Common data are defined in platfor-specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ltc3220/src/platform>`_

Maxim Platform
--------------

**Used Hardware**

* `DC1265A-A <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/dc1265a-a.html>`_
* `MAX32690 <https://www.analog.com/en/products/max32690.html>`_

**Connections**:

+----------+-----------------------+
| LTC3220  | MAX32655 Pinouts      |
+----------+-----------------------+
| V_IN     | Power Supply          |
+----------+-----------------------+
| V_CC     | 3v3                   |
+----------+-----------------------+
| GND      | GND                   |
+----------+-----------------------+
| I2C_SCL  | P2_8                  |
+----------+-----------------------+
| I2C_SDA  | P2_7                  |
+----------+-----------------------+
| RST      | P0_14                 |
+----------+-----------------------+

** Build Command **

.. code-block:: bash

    # to delete current build
    make reset

    #to build project
    make TARGET=max32690

    # to flash the code to the controller
    make TARGET=max32690 run
