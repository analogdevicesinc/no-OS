LTC3208 no-OS driver
====================

.. no-os-doxygen::

Supported Devices
-----------------

`LTC3208 <https://www.analog.com/en/products/ltc3208.html>`_

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

Applications
------------

* Video/Camera Phones with QVGA + Displays

LTC3208 Device Configuration
----------------------------

Driver Initialization
---------------------

In order to be able to use the device, you will have to provide the support for
the communication protocol (I2C) as mentioned above.

The first API to be called is **ltc3208_init**. Make sure that it returns 0,
which means that the driver was initialized correctly.

4-bit LED Configuration
-----------------------

The following pairs share one register each pair to control the current level:
	RED and GREEN - GREEN uses the hight bits and RED uses
	the low bits.
	BLUE and AUX - AUX uses the hight bits and BLUE uses
	the low bits.

The current level for cam is also split between the high current and low with
CAM_HIGH using the hight bits and CAM_LOW using the low bits.

These current level can be controlled using **ltc3208_set_4_bit_dac**,
where it selects which register to change using an enumeration of the registers.

8-bit LED Configuration
-----------------------

The MAIN and SUB LEDS uses 255 levels to control current.

These current level can be controlled using **ltc3208_set_8_bit_dac**,
where it selects which register to change using an enumeration of the registers.

Device Options Configuration
----------------------------

The device can be forced to operate on a specific charge pump configuration, 
enable dropout signal, choose if cam high or low is used, and choose if 
ENRGBS pin controls RGB or SUB. This can be set individually via 
**ltc3208_set_cpo_2x**, **ltc3208_set_1p5x**, 
**ltc3208_set_droupout_enable**,  **ltc3208_set_cam_enable**, 
**ltc3208_set_sub_enable**, or directly to the register
via **ltc3208_update_options**.
