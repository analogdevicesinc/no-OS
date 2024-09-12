MAX25603 no-OS driver
=====================

Supported Devices
-----------------

`MAX25603 <https://www.analog.com/MAX25603>`_

Overview
--------

The MAX25603 is a synchronous four-switch, buckboost LED driver controller
suitable for multifunction automotive combination head lamps.

The controller regulates the LED current for LED string voltages from 0V to 60V.
The MAX25603 can be used as a seamless buck-boost LED driver for applications
that require an efficient buck-boost LED driver with synchronous rectification.

The MAX25603 is ideal for high-power applications that require a current source
with PWM dimming capability.

Applications
------------

MAX25603
--------

* Combination Automotive Head Lamps

MAX25603 Device Configuration
-----------------------------

Driver Initialization
---------------------
The first API to be called is **max25603_init**. Make sure that it returns 0,
which means that the driver was initialized correctly.

Global Configuration
--------------------

**max25603_enable** - API designed to enable either LOW or HIGH BEAM or disable
them.

**max25603_sel_comp** - API designed to select which comparator to use
(drive high when dimming on either set of LED strings).

MAX25603 Driver Initialization Example
--------------------------------------

.. code-block:: bash

	struct max25603_desc *max25603_desc;
	struct max25603_init_param max25603_ip = {
		.en1_param = &max25603_en1_ip,
		.en2_param = &max25603_en2_ip,
		.flt_param = NULL,
		.shunt_param = &max25603_shunt_ip,
		.sw_freq = MAX25603_SW_FREQ_200KHZ,
	};

	ret = max25603_init(&max25603_desc, &max25603_ip);
	if (ret)
		goto error;
