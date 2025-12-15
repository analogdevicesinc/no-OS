MAX22915 no-OS driver
=====================

.. no-os-doxygen::

Supported Devices
-----------------

`MAX22915 <https://www.analog.com/en/products/max22915.html>`_

Overview
--------

The MAX22915 is an industrial octal high-side switch with diagnostics.
It is specified for operation with a supply voltage tolerant up to 65V.

MAX22915 on the other side has eight high-side switched specified to deliver up
to 700mA (min) continuous current per channel. The high-side switches have
low on-resistance of 120mOhms (max).

Applications
------------

* Industrial Digital Output Systems
* PLC Systems
* Distributed Control Systems (DCS)

MAX22915 Device Configuration
-----------------------------

Driver Initialization
---------------------

In order to be able to use the device, you will have to provide the support
for the communication protocol (SPI).

The first API to be called is **max22915_init**. Make sure that it returns 0,
which means that the driver was initialized correctly.

Channel Configuration
---------------------

Each channel can be configured as a digital output only.
The state of a channel (switch state) can be set with the **max22915_set_sw_state**
API which takes as parameters the device descriptor, the channel number and the
requested switch state (SWITCH_OPEN or SWITCH_CLOSE).


Diagnostics Configuration
-------------------------

The MAX22915 provides several diagnostic features that can be configured:

* **Open-Wire Detection**: Configure open-wire detection for both on and off states
  using **max22915_set_owon_config** and **max22915_set_owoff_config** APIs.

* **Short to VDD Detection**: Configure short to VDD detection using
  **max22915_set_shvdd_config** API.

* **Current Measurement**: Configure current measurement settings using
  **max22915_set_current_config** API with a **max22915_current_config** structure.

* **Temperature Measurement**: Configure temperature measurement settings using
  **max22915_set_temperature_config** API with a **max22915_temp_config** structure.

MAX22915 Driver Initialization Example
--------------------------------------

.. code-block:: c

	struct max22915_desc *max22915;
	struct no_os_spi_init_param spi_ip = {
		.device_id = 0,
		.extra = &max22915_spi_extra,
		.max_speed_hz = 100000,
		.platform_ops = &max_spi_ops,
		.chip_select = 0,
	};
	struct max22915_current_config curr_config = {
		.curr_average = 0,
		.curr_one_shot_en = false,
		.curr_cont = false,
	};
	struct max22915_temp_config temp_config = {
		.temp_average = 0,
		.therm_warn_en = false,
		.temp_cont = false,
	};
	struct max22915_init_param max22915_ip = {
		.spi_init = &spi_ip,
		.sw_state = {SWITCH_OPEN, SWITCH_OPEN, SWITCH_OPEN, SWITCH_OPEN,
			     SWITCH_OPEN, SWITCH_OPEN, SWITCH_OPEN, SWITCH_OPEN},
		.shtvdd_enable = {false, false, false, false,
				  false, false, false, false},
		.curr_config = curr_config,
		.temp_config = temp_config,
		.crc_en = true,
	};

	ret = max22915_init(&max22915, &max22915_ip);
	if (ret)
		goto error;
