ADG1712 no-OS driver
====================

Supported Devices
-----------------

`ADG1712 <https://www.analog.com/en/products/adg1712.html>`_

Overview
--------

The ADG1712 is a low voltage, 2.4 Ω quad SPST (Single Pole Single Throw) switch
designed for applications requiring high performance switching with low on-resistance.
The device operates from a single +1.8 V to +5.5 V supply and is controlled via
four digital inputs (IN1, IN2, IN3, IN4) that control the corresponding switch pairs
(S1-D1, S2-D2, S3-D3, S4-D4).

Applications
------------

* Audio and video signal routing
* Communications systems
* Test and measurement equipment
* Industrial control systems
* Battery-powered equipment

ADG1712 Device Configuration
----------------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~~

In order to be able to use the device, you will have to provide the support for the
communication protocol (GPIO) as described in the no-OS documentation.

The first API to be called is **adg1712_init**. Make sure that it returns 0, which means
that the driver was initialized correctly.

Switch Control
~~~~~~~~~~~~~~

The ADG1712 driver provides functions to control individual switches:

* **adg1712_set_switch_state** - Enable or disable a specific switch
* **adg1712_get_switch_state** - Read the current state of a specific switch

ADG1712 Driver Initialization Example
--------------------------------------

.. code-block:: bash

	struct adg1712_dev *dev;
	struct adg1712_init_param init_param;
	
	init_param.gpio_in1.number = 1;
	init_param.gpio_in1.platform_ops = &gpio_ops;
	init_param.gpio_in2.number = 2;
	init_param.gpio_in2.platform_ops = &gpio_ops;
	init_param.gpio_in3.number = 3;
	init_param.gpio_in3.platform_ops = &gpio_ops;
	init_param.gpio_in4.number = 4;
	init_param.gpio_in4.platform_ops = &gpio_ops;
	
	ret = adg1712_init(&dev, &init_param);
	if (ret)
		return ret;
	
	// Enable switch 1
	ret = adg1712_set_switch_state(dev, ADG1712_SW1, true);
	if (ret)
		return ret;
	
	// Check switch 1 state
	bool enabled;
	ret = adg1712_get_switch_state(dev, ADG1712_SW1, &enabled);
	if (ret)
		return ret;
