MAX14919 no-OS driver
=====================

Supported Devices
-----------------

`MAX14919 <https://www.analog.com/MAX14919>`_

Overview
--------

The MAX14919/MAX14919A industrial-protected quad-channel lowside switch features
140mΩ (typ) on-resistance (RON) per channel with integrated ±1kV/42Ω surge
protection for robust operation.

Resistor-settable accurate current limiting provides guaranteed operating
currents in the range of 100mA to 800mA. Loads that draw large activation
or inrush currents are supported using the 2x inrush load-current option.
The outputs can be connected in parallel to achieve higher load currents.
The four switches are pin-controlled to allow for simple and fast
switching of up to 500kHz.

The MAX14919/MAX14919A features reverse current detection.
The MAX14919 implements reverse-current protection by driving an external FET
for non-capacitive loads. The MAX14919A has reverse current indication.

Applications
------------

MAX14919
--------

* Industrial Digital Outputs
* Relay and Solenoid Drivers
* PLC and DCS Systems
* Motor Control

MAX14919 Device Configuration
-----------------------------

Driver Initialization
---------------------

In order to use the device, you will have to provide support for at least
5 GPIO pins, and 2 more optional GPIO pins in case you want to use those
specific device functions:

* IN GPIOs x4 (mandatory)
* FAULT GPIO (mandatory)
* IN_RUSH GPIO (optional - used for current limiting)
* REV GPIO (optional - used for detection of reverse current's polarity)

Theese pins will be initialized alongside the device using
**max14919_init** API.

State Configuration
-------------------

The MAX14919 OUTPUT channels can be configured with the **max14919_set_out**
API where an state array should be passed as an argument to set the OUTPUT
channels state.

Fault Detection
---------------

Fault detection is available through **max14919_get_fault** API.

Current Limiting and Reverse Polarity
-------------------------------------

Current Limiting is available through the **max14919_set_climit** API if the
IN_RUSH GPIO has been initialized.

Reverse Polarity of the current detection is available through the
**max14919_detect_rev** API if a REV GPIO has been initialized.

MAX14919 Driver Initialization Example
--------------------------------------

.. code-block:: bash

	struct max14919_desc *max14919_desc;
	struct no_os_gpio_init_param *max14919_in_ip[4] = {
		{
			.port = 1,
			.pull = NO_OS_PULL_NONE,
			.number = 0,
			.platform_ops = &max_gpio_ops,
			.extra = &max14919_gpio_extra_ip,
		},
		{
			.port = 1,
			.pull = NO_OS_PULL_NONE,
			.number = 1,
			.platform_ops = &max_gpio_ops,
			.extra = &max14919_gpio_extra_ip,
		},
		{
			.port = 1,
			.pull = NO_OS_PULL_NONE,
			.number = 2,
			.platform_ops = &max_gpio_ops,
			.extra = &max14919_gpio_extra_ip,
		},
		{
			.port = 1,
			.pull = NO_OS_PULL_NONE,
			.number = 3,
			.platform_ops = &max_gpio_ops,
			.extra = &max14919_gpio_extra_ip,
		},
	};
	struct no_os_gpio_init_param max14919_fault_ip = {
		.port = 2,
		.pull = NO_OS_PULL_NONE,
		.number = 21,
		.platform_ops = &max_gpio_ops,
		.extra = &max14919_gpio_extra_ip,
	};
	struct max14919_init_param max14919_ip = {
		.in_param = max14919_in_ip,
		.fault_param = &max14919_fault_ip,
	};
	ret = max14919(&max14919_desc, &max14919_ip);
	if (ret)
		goto error;
