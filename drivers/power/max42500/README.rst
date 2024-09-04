MAX42500 no-OS driver
=====================

Supported Devices
-----------------
`MAX42500 <https://www.analog.com/en/products/max42500>`_

Overview
--------

The MAX42500 is a SoC power-system monitor with up to seven voltage monitor 
inputs that will be SIL 3-Certified. Each input has programmable OV/UV 
thresholds of between 2.5% and 10% with ±1.3% accuracy over the full temperature 
range. Two of the inputs have a separate remote ground-sense input and support 
DVS through the integrated I2C interface.

The MAX42500 contains a programmable flexible power sequence recorder (FPSR). 
This recorder stores power-up and power-down timestamps separately, and supports 
on/off and sleep/standby power sequences. The MAX42500 also contains a 
programmable challenge/response watchdog, which is accessible through the I2C 
interface, along with a configurable RESET output.

Applications
------------
* Industrial Process Control
* Robotics
* Remote Sensor Modules
* Power System Supervision
* MCU/SoC Monitoring

MAX42500 Device Configuration
-----------------------------

Driver Initialization
---------------------
In order to be able to use the device, you will have to provide the support for 
the communication protocol (I2C) alongside other GPIO pins if needed for the 
specific application (depends on the way the device is used).

The first API to be called is **max42500_init**. Make sure that it returns 0, which
means that the driver was initialized correctly.

Voltage Monitor
---------------
The MAX42500 has up to seven voltage monitor channels. Five of the monitors 
feature single-ended inputs. For these channels, you first set a nominal
voltage, followed by setting the overvoltage (OV) and undervoltage 
(UV) thresholds as a percentage of that nominal voltage. The other two monitors 
have differential inputs and share a remote ground-sense pin (INM). Unlike the 
single-ended monitors, which use a nominal voltage plus percentage-based OV/UV 
configuration, the differential inputs have independent OV and UV comparators, 
each of which can be configured with its own reference voltage. 

IN1 through IN4 have a nominal voltage set-point range of 0.50V to 3.6875V, 
while IN5 has an extended range of 0.50V to 5.50V. This can be set using 
**max42500_set_nominal_voltage** API. 

The OV and UV thresholds can be set using
**max42500_set_ov_thresh1** API and **max42500_set_uv_thresh1** API for IN1 
through IN5, from 2.5% to 10% in 0.5% steps. IN6P and IN7P have the differential 
configuration. Their OV and UV set points can range from 0.50V to 1.775V and can 
be set using **max42500_set_ov_thresh2** API and **max42500_set_uv_thresh2** API.

Aside from OV and UV comparators, every monitor channel also has an OFF 
comparator that asserts when the monitor input voltage falls below 0.25V. The
statuses of these comparators can be read using the **max42500_get_comp_status**
API.

Power-Up and Power-Down Timestamps
-----------------------------------
A power-up timestamp is recorded for an enabled channel when the associated 
voltage rises above the programmed UV threshold. A power-down timestamp is 
recorded for an enabled channel when the associated voltage falls below the 
OFF threshold.

The power-up and power-down timestamps can be read using the
**max42500_get_power_up_timestamp** API and **max42500_get_power_down_timestamp**
API respectively.

MAX42500 Driver Initialization Example
--------------------------------------

.. code-block:: bash

	struct max42500_dev *device;
	struct max_i2c_init_param max42500_i2c_ip;
	struct max_gpio_init_param max42500_gpio_extra;

	struct max42500_init_param max42500_ip = {
		.comm_param = {
			.device_id      = 0,
			.max_speed_hz   = 400000,
			.slave_address  = MAX42500_ADDR(0),
			.platform_ops   = &max_i2c_ops,
			.extra          = (void *)&max42500_i2c_ip,
		},
		.en0_param = {
			.port           = 2,
			.number         = 26,
			.pull           = NO_OS_PULL_DOWN,
			.platform_ops   = &max_gpio_ops,
			.extra          = (void *)&max42500_gpio_extra,
		},
		.en1_param = {
			.port           = 2,
			.number         = 29,
			.pull           = NO_OS_PULL_DOWN,
			.platform_ops   = &max_gpio_ops,
			.extra          = (void *)&max42500_gpio_extra,
		},
		.addr_param = {
			.port           = 2,
			.number         = 9,
			.pull           = NO_OS_PULL_DOWN,
			.platform_ops   = &max_gpio_ops,
			.extra          = (void *)&max42500_gpio_extra,
		},
		.addr_sel   = 0,
		.pece       = 0x01,
		.vmon_en    = 0x00,
		.vmon_vmpd  = 0x01,
		.reset_map  = 0x00,
	};

	ret = max42500_init(&device, &max42500_ip);
	if (ret)
		goto error;
