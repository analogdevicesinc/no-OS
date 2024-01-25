MAX14914 no-OS driver
=====================

Supported Devices
-----------------

`MAX14914 <https://www.analog.com/MAX14914>`_

Overview
--------

The MAX14914, MAX14914A and MAX14914B are the
family of high-side/push-pull drivers that operate as both
an industrial digital output (DO) and an industrial digital input (DI). 
The MAX14914 family features full IEC 61131-2
compliance in both their DO and DI modes of operation.
The high-side switch current is resistor settable from
135mA (min) to 1.3A (min). The high-side driver’s on-resistance is 120mΩ (typ)
at 125°C ambient temperature. Optional push-pull operation allows driving of 
cables and fast discharge of load capacitance. The output voltage
is monitored and indicated through the DOI_LVL pin for
safety applications.

Applications
------------

MAX14914
--------

* Configurable Digital Input/Output
* Industrial Digital Outputs and Inputs Modules
* Motor Control
* Safety Systems

MAX14914 Device Configuration
-----------------------------

Driver Initialization
---------------------

In order to use the device, you will have to provide the support for
5 GPIO pins:

* FAULT
* DOI_LVL
* DI_EN
* IN
* PP

These pins will be initialized alongside the device using
**max14914_init** API.

State Configuration
-------------------

The MAX14914 device can be set in the next 5 states using
**max14914_set_state** API:

* DIGITAL OUTPUT PUSH-PULL,
* DIGITAL OUTPUT HIGH_SIDE,
* DIGITAL OUTPUT LOW_SIDE,
* DIGITAL INPUT IEC TYPE1/3,
* DIGITAL INPUT IEC TYPE2

MAX14914 Driver Initialization Example
--------------------------------------

.. code-block:: bash

	struct max14914_desc *max14914_desc;
	struct no_os_gpio_init_param max14914_fault_ip = {
		.port = 2,
		.pull = NO_OS_PULL_NONE,
		.number = 21,
		.platform_ops = &max_gpio_ops,
		.extra = &max14914_gpio_extra_ip,
	};
	struct no_os_gpio_init_param max14914_doilvl_ip = {
		.port = 1,
		.pull = NO_OS_PULL_NONE,
		.number = 0,
		.platform_ops = &max_gpio_ops,
		.extra = &max14914_gpio_extra_ip,
	};
	struct no_os_gpio_init_param max14914_di_en_ip = {
		.port = 1,
		.pull = NO_OS_PULL_NONE,
		.number = 1,
		.platform_ops = &max_gpio_ops,
		.extra = &max14914_gpio_extra_ip,
	};
	struct no_os_gpio_init_param max14914_in_ip = {
		.port = 1,
		.pull = NO_OS_PULL_NONE,
		.number = 2,
		.platform_ops = &max_gpio_ops,
		.extra = &max14914_gpio_extra_ip,
	};
	struct no_os_gpio_init_param max14914_pp_ip = {
		.port = 1,
		.pull = NO_OS_PULL_NONE,
		.number = 3,
		.platform_ops = &max_gpio_ops,
		.extra = &max14914_gpio_extra_ip,
	};
	struct max14914_init_param max14914_ip = {
		.di_en_init_param = &max14914_di_en_ip,
		.doilvl_init_param = &max14914_doilvl_ip,
		.fault_init_param = &max14914_fault_ip,
		.in_init_param = &max14914_in_ip,
		.pp_init_param = &max14914_pp_ip
	};
	ret = max14914_init(&max14914_desc, &max14914_ip);
	if (ret)
		goto error;
