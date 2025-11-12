LTC3220 no-OS driver
====================

.. no-os-doxygen::

Supported Devices
-----------------

`LTC3220 <https://www.analog.com/en/products/ltc3220.html>`_

Overview
--------

The LTC3220 is a highly integrated multi-display LED drivers. 
These parts contain a high efficiency, low noise charge pump to provide power 
to up to eighteen universal LED current sources. The LED currents are set by 
an internal precision current reference. Independent dimming, on/off, 
blinking and gradation control for all universal current sources is achieved 
via the I2C serial interface. 6-bit linear DACs are available to adjust 
brightness levels independently for each universal LED current source.

The LTC3220 charge pump optimizes efﬁciency based on the voltage 
across the LED current sources. The part powers up in 1x mode and 
will automatically switch to boost mode whenever any enabled LED current source 
begins to enter dropout. The ﬁrst dropout switches the parts into 1.5x mode 
and a subsequent dropout switches the LTC3220 into 2x mode. The parts reset to 
1x mode whenever a data bit is updated via the I2C port.

There are two I2C addresses available. The LTC3220 I2C address is 0011100 
and the LTC3220-1 I2C address is 0011101. The I2C address is the only difference
between the LTC3220 and LTC3220-1.

Applications
------------

* Video Phones with QVGA+ Displays
* Keypad Lighting
* General/Miscellaneous Lighting

LTC3220 Device Configuration
----------------------------

Driver Initialization
---------------------

In order to be able to use the device, you will have to provide the support for
the communication protocol (I2C) as mentioned above.

The first API to be called is **ltc3220_init**. Make sure that it returns 0,
which means that the driver was initialized correctly.

After successful initialization, it is recommended to perform a device reset by
calling **ltc3220_reset** in order to put the device in a known initial state,
where all the registers on the device are reset to 0.

ULED Configuration
------------------

Each of the 18 LEDs are individually configurable to a specific mode
of operation and adjustable current output, using a 6-bit DAC
to provide 64 levels of resolution from 0mA to 20mA.

The supported modes of operation are:

1. Normal - provides a current output for an LED attached to its pins.
2. Blinking - performs a blinking operation with configurable period and 
   rise time in the Blink Configuration
3. Gradation - performs a gradation to gradually adjust the current provided 
   to the output, configurable to rising or falling and adjusting its ramp time
   and period.
4. General Purpouse Output (GPO) - Configures the output to run in current 
   limited mode or as a strong pull-down output if the output current is 0mA.

The ULED registers can be updated by using **ltc3220_set_uled_mode** and
**ltc3220_set_uled_current**, or together in the respective ULEDs' register via 
**ltc3220_update_reg_uled**, where the 2 upper bits written correspond to the
mode of operation, and the 6 lower bits are for the DAC.

If the device command register has quick write set, writing a mode and current
output level on ULED1 will perform a parallel write to all ULED outputs.

Device Command Configuration
----------------------------

The device can be forced to operate on a specific charge pump configuration, be
written to in parallel through ULED1, and be shutdown while maintaining the 
latest device configuration registers. This can be set individually via 
**ltc3220_set_quick_write**, **ltc3220_set_cpo_1x**, **ltc3220_set_cpo_1p5x**, 
**ltc3220_set_cpo_2x**, **ltc3220_set_shutdown**, or directly to the register
via **ltc3220_update_reg_command**, where the 4 lower bits of the register 
corresponds to the above set configurations, according to the
device's datasheet.

Blink Configuration
-------------------

The blinking mode of operation for ULED outputs can be configured to have a
longer blinking period and a faster rise time in current. These can be
configured with **ltc3220_set_blink_fast** and **ltc3220_set_blink_long**, or
together in the Blink and Gradation Configuration register via 
**ltc3220_update_reg_blink_grad**. Note that this also configures
the gradation configuration as they share the same configuration register.

Gradation Configuration
-----------------------

The gradation mode of operation for ULED outputs can be configured to increase
or decrease in brightness by gradually adjusting the current output based on an
internal counter. This counter resets to its maximum when set to decreasing, and
resets to its minimum when set to increasing. This can be set by using 
**ltc3220_set_grad_increasing**.

Note that in increasing gradation, the counter remains at its maximum if another
LED is set to gradation mode. The newly set LED will immediately appear with its
maximum current and will not rise towards it.

The ramp time and period of gradation is also configurable from disabling
gradation to a ramp time of 0.96s to 0.24s and a period of 1.25s to 0.313s,
set by using **ltc3220_set_grad_speed**.

Alternatively these can be set together in the Blink and Gradation Configuration 
register via **ltc3220_update_reg_blink_grad**. Note that this also configures
the blink configuration as they share the same configuration register.
