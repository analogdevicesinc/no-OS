AD7291 no-OS Example Project
============================

.. no-os-doxygen::

.. contents::
    :depth: 3

Supported Devices
-----------------

* `AD7291 <https://www.analog.com/AD7291>`_

Overview
--------

The AD7291 is an 8-channel, 12-bit ADC with an integrated temperature sensor
communicating over I2C. On the `ADALM-PHASER (CN0566)
<https://www.analog.com/en/resources/reference-designs/circuits-from-the-lab/cn0566.html>`_
board, the AD7291 monitors eight power-rail and signal voltages through
resistor dividers.

This project reads all eight channels in a loop, applies the CN0566 schematic
scale factors, and prints the actual voltages over UART.

+------+---------+--------------+------------------------------------+
| Chan | Label   | Scale Factor | Description                        |
+======+=========+==============+====================================+
| CH0  | VDD1V8  | 2.0          | 1.8 V rail                         |
+------+---------+--------------+------------------------------------+
| CH1  | VDD3V0  | 2.0          | 3.0 V rail                         |
+------+---------+--------------+------------------------------------+
| CH2  | VDD3V3  | 2.0          | 3.3 V rail                         |
+------+---------+--------------+------------------------------------+
| CH3  | VDD4V5  | 4.01         | 4.5 V rail                         |
+------+---------+--------------+------------------------------------+
| CH4  | VDD_AMP | 7.98         | Amplifier supply                   |
+------+---------+--------------+------------------------------------+
| CH5  | VINPUT  | 4.01         | Input voltage                      |
+------+---------+--------------+------------------------------------+
| CH6  | IMON    | 1.0          | Current monitor (LTC4217, 1 V/A)   |
+------+---------+--------------+------------------------------------+
| CH7  | VTUNE   | 7.98         | VCO tuning voltage                 |
+------+---------+--------------+------------------------------------+

Hardware Specifications
-----------------------

**Used hardware**:

* Raspberry Pi (tested on RPi 4)
* `ADALM-PHASER (CN0566) <https://www.analog.com/en/resources/reference-designs/circuits-from-the-lab/cn0566.html>`_
* AD7291 connected on I2C bus 1 at address ``0x2A``

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

Prerequisites:

* CMake >= 3.10
* GCC toolchain
* I2C enabled on the Raspberry Pi (``sudo raspi-config`` ? Interface Options ? I2C)

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Source Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad7291/src>`_

Basic example
^^^^^^^^^^^^^

This is a simple example which initializes the AD7291 and reads all eight
voltage channels and the internal temperature sensor in a while loop with a
period of 1 s. The data is printed on the serial interface.

In order to build the basic example:

.. code-block:: bash

        cd projects/ad7291
        mkdir build && cd build
        cmake ..
        make

Root permissions are required to access ``/dev/i2c-1``:

.. code-block:: bash

        sudo ./ad7291

**Example output**:

.. code-block:: text

        AD7291 initialized at 0x2A on /dev/i2c-1
        CH0 VDD1V8 : 1.802 V
        CH1 VDD3V0 : 3.012 V
        CH2 VDD3V3 : 3.298 V
        CH3 VDD4V5 : 4.510 V
        CH4 VDD_AMP: 5.012 V
        CH5 VINPUT : 4.998 V
        CH6 IMON   : 0.350 V
        CH7 VTUNE  : 3.200 V
        ---

No-OS Supported Platforms
-------------------------

Linux Platform
^^^^^^^^^^^^^^

**Used hardware**:

* `ADALM-PHASER (CN0566) <https://www.analog.com/en/resources/reference-designs/circuits-from-the-lab/cn0566.html>`_ with
* Raspberry Pi 4

**Build Command**

.. code-block:: bash

        cd projects/ad7291
        mkdir build && cd build
        cmake ..
        make
        sudo ./ad7291
