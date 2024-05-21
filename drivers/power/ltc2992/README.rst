LTC2992 no-OS driver
====================

Supported Devices
-----------------

`LTC2992 <https://www.analog.com/LTC2992>`_

Overview
--------

The LTC2992 is a rail-to-rail system monitor that measures current, voltage,
and power of two supplies. It features an operating range of 2.7V to 100V and
includes a shunt regulator for supplies above 100V. The voltage measurement
range of 0V to 100V is independent of the input supply. Two ADCs simultaneously
measure each supply current. A third ADC monitors the input voltages and four
auxiliary external voltages. Each supply current and power is added for total
system consumption. Minimum and maximum values are stored and an overrange alert
with programmable thresholds minimizes the need for software polling.

Applications
------------

LTC2992
-------

* Telecom infrastructure
* Industrial equipment
* Automotive
* Computer systems and servers

LTC2992 Device Configuration
----------------------------

Driver Initialization
---------------------

In order to be able to use the device, you will have to provide the support
for the communication protocol (I2C) alongside other GPIO pins if needed in the
specific application (depends on the way the device is used).

The first API function to be called is ltc2992_addr_gen to generate the
corresponding device's slave address. Once a proper address is generated, use
the function **ltc2992_init** to initialize the device and make sure that it
returns 0. This indicates a successful driver initialization.

The LTC2992 must be configured such that ADC resolution and shunt resistor
values are indicated in the **ltc2992_init_params** structure. Optionally, other
parameters such as calibration mode, measurement mode, and voltage selection can
also be set at initialization.

Voltage, Current and Power readings
-----------------------------------

The LTC2992 voltage, current and power monitoring features are exposed via the
**ltc2992_get_voltage**, **ltc2992_get_vshunt**, **ltc2992_get_current**, and
**ltc2992_get_power** functions. LTC2992 also saves the maximum and minimum
voltage, current and power data. These can be accessed via the
**ltc2992_get_max_data** and the **ltc2992_get_min_data** APIs.

Alert and Thresholds
--------------------

The LTC2992 can be configured to assert alert when a voltage, current or power
readings reaches a maximum or minimum threshold limit. To enable alert, the
**ltc2992_set_alert** can be used. This requires an **enum ltc2992_alert_value**
parameter. 

Threshold limits can then be set using the **ltc2992_set_max_thresh** and
**ltc2992_set_min_thresh** functions. Fault statuses can be polled by calling
**ltc2992_get_fault** and can also be cleared by **ltc2992_clear_fault**.

General Purpose Input Output
----------------------------

The LTC2992 also features GPIOs that can be configured as a general purpose
output, logic input or data converter input. GPIO3 can be used as a data ready
signal which pulses low when ADC data becomes available. GPIO4 can be used as an
alert pin which allows interrupt-based fault monitoring. An alert pin can be
configured as an interrupt input using the **alert_gpio_init** parameter at
initialization.

LTC2992 Driver Initialization Example
-------------------------------------

.. code-block:: bash

	struct ltc2992_init_param ltc2992_ip = {
		.i2c_init = {
			.device_id = 1,
			.max_speed_hz = MXC_I2C_STD_MODE,
			.extra = &max32655_i2c_extras,
			.platform_ops = &max_i2c_ops,
		},
		.shunt_resistor = 10,
		.alert_clear = true,
		.alert_gpio_init = {
			.number = -1,
		},
		.resolution = LTC2992_RESOLUTION_12_BIT,
	};

	ret = ltc2992_addr_gen(&ltc2992_ip, LTC2992_ADR_NC, LTC2992_ADR_NC);
	if (ret)
		goto error;

	ret = ltc2992_init(&dev, &ltc2992_ip);
	if (ret)
		goto error;
