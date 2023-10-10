MAX149X6 no-OS driver
=====================

Supported Devices
-----------------

`MAX14906 <https://www.analog.com/MAX14906>`_
`MAX14916 <https://www.analog.com/MAX14916>`_

Overview
--------

MAX14906 and MAX14916 share the same API when referring to the CRC algorithm, 
register read, write and update. Although, each of them has its own register map
and a specific API, therefore there are two files (header and source file) for
them called max149x6-base.h and max149x6-base.c which includes the common stuff.
Each of these devices then has its own driver files which includes the base
driver files and uses the common API within their specific APIs.

MAX14906 is an IEC 61131-2 compliant, high-speed, four-channel industrial
digital output/input device that can be configured on a per-channel basis
as a high-side (HS) switch, push-pull (PP) driver, or a Type 1 and 3, or Type 2
digital input. The MAX14906 is specified for operation with a supply voltage up
to 40V and is tolerant to 65V.

MAX14916 on the other side has eight high-side switched specified to deliver up
to 1.1A (min) continuous current per channel. The two adjacent channels can pe
paralleled to form four 2.4A high-side switches. The high-side switches have
on-resistance of 250mOhms (max) at 125 degrees Celsius ambient temperature.

Applications
------------

MAX14906
--------
* Industrial Digital Output and Input Module
* Configurable Digital Input/Output
* Motor Control
* PLC systems
* Distributed Control Systems (DCS)

MAX14906 Device Configuration
-----------------------------

Driver Initialization
---------------------

In order to be able to use the device, you will have to provide the support
for the communication protocol (SPI).

The first API to be called is **max14906_init**. Make sure that it returns 0,
which means that the driver was initialized correctly.

Channel Configuration
---------------------

Each channel cand be configured either as a digital input or a digital output.
In order to do that you need to call the **max14906_ch_func** API.
The state of a channel (voltage state) cand be read with the **max14906_ch_get**
API.
If a specific channel is set as an output you can also set the logic state
of the channel with **max14906_ch_set** API.

Current Limit Configuration
---------------------------

Current limit for each channel can be set and read with **max14906_climit_set**,
respectively with **max14906_climit_get** APIs.

Interrupts Configuration
------------------------

The device allows the usage of interrupts which can be mapped to FAULT pin
and is configured like any other no-os interrupt (using no_os_irq API).

MAX14906 Driver Initialization Example
--------------------------------------

.. code-block:: bash

	struct max149x6_desc *max14906;
	struct no_os_spi_init_param spi_ip = {
		.device_id = 0,
		.extra = &max14906_spi_extra,
		.max_speed_hz = 100000,
		.platform_ops = &max_spi_ops,
		.chip_select = 0,
	};
	struct max149x6_init_param max14906_ip = {
		.chip_address = 0;
		.comm_desc = &spi_ip;
		.crc_en = true;
	}

	ret = max14906_init(&max14906, &max14906_ip);
	if (ret)
		goto error;
	
MAX14906 no-OS IIO support
--------------------------

The MAX14906 IIO driver comes on top of the MAX14906 driver and offers support
for interfacing IIO clients through IIO lib.

MAX14906 IIO Devce Configuration
--------------------------------

Device Attributes
-----------------

Output Attributes
-----------------

MAX14906 has 2 output attributes(do mode, current limit).

DO MODE means the configuration of the device, there is also a do mode
avail attribute which means the do mode available(list of configuration modes)
for the device.

Current Limit means the current limit set on by the device on a specific
output channel. There is also a current limit avail attribute which contains
a list with the available limits for the output current.

Input Attributes
----------------

IEC Type attribute contains information about the IEC type used by the device.
For this one user also has to select from the IEC type available attribute to
assign IEC Type a value.

Device Channels
---------------

MAX14906 has a specific API, **max14906_iio_setup_channels** for configuring the
channels at the initialization, therefore the channels can be configured as
input or output as requested.

MAX14906 IIO Driver Initialization Example
------------------------------------------

.. code-block:: bash

	struct max149x6_init_param max14906_ip = {
		.chip_address = 0;
		.comm_desc = &spi_ip;
		.crc_en = true;
	}
	struct max14906_iio_desc *max14906_iio_desc;
	struct max14906_iio_desc_init_param max14906_iio_ip = {
		.max14906_ip = &max14906_ip,
		.channel_configs = {
			{
				.enabled = true,
				.function = MAX14906_OUT,
			},
			{
				.enabled = true,
				.function = MAX14906_IN,
			},
			{
				.enabled = true,
				.function = MAX14906_OUT,
			},
			{
				.enabled = true,
				.funtion = MAX14906_IN,
			},
		},
	};

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	ret = max14906_iio_init(&max14906_iio_desc, &max14906_iio_ip);
	if (ret)
		goto error;
	
	struct iio_app_device iio_devices[] = {
		{
			.name = "max14906",
			.dev = max14906_iio_desc,
			.dev_descriptor = max14906_iio_desc->iio_dev,
		}
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = max14906_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto error;
	
	return iio_app_run(app);

MAX14916
--------
* Industrial Digital Outputs
* PLC systems

MAX14916 Device Configuration
-----------------------------

Driver Initialization
---------------------

In order to be able to use the device, you will have to provide the support
for the communication protocol (SPI).

The first API to be called is **max14916_init**. Make sure that it returns 0,
which means that the driver was initialized correctly.

Channel Configuration
---------------------

Each channel can be configured as a digital output only.
The state of a channel(voltage state) cand be read with the **max14916_ch_get**
API and also set with **max14916_ch_set**.

LED Configuration
---------------------------

Status LEDs cand be configured with **max14916_sled_set** which takes as
parameters the device descriptor, the number of the channel requested for
the LED state to change and the requested state.

Fault LEDs state however can not be set by the user since they indicate
the case of a fault, still they have a configurable minimum on-time value
assigned to them which the user can change through the
**max14916_fled_time_set** API.

Interrupts Configuration
------------------------

The device allows the usage of interrupts which can be mapped to FAULT pin
and is configured like any other no-os interrupt(using no_os_irq API) since
the descriptor is common for both MAX14916 and MAX14906.

MAX14916 Driver Initialization Example
--------------------------------------

.. code-block:: bash

	struct max149x6_desc *max14916;
	struct no_os_spi_init_param spi_ip = {
		.device_id = 1,
		.extra = &max14916_spi_extra,
		.max_speed_hz = 100000,
		.platform_ops = &max_spi_ops,
		.chip_select = 1,
	};
	struct max149x6_init_param max14916_ip = {
		.chip_address = 0;
		.comm_desc = &spi_ip;
		.crc_en = true;
	}

	ret = max14916_init(&max14916, &max14916_ip);
	if (ret)
		goto error;
	
MAX14916 no-OS IIO support
--------------------------

The MAX14916 IIO driver comes on top of the MAX14916 driver and offers support
for interfacing IIO clients through IIO lib.

MAX14916 IIO Devce Configuration
--------------------------------

Device Attributes
-----------------

MAX14916 has only 3 attributes :

* ``raw - indicates the state of a channel(0 or 1).``
* ``scale - which is always 1 because the device doesn't have any other scale.``
* ``offset - which is 0.``

MAX14916 IIO Driver Initialization Example
------------------------------------------

.. code-block:: bash
	
	struct max149x6_init_param max14916_ip = {
		.chip_address = 0;
		.comm_desc = &spi_ip;
		.crc_en = true;
	}
	struct max14916_iio_desc *max14916_iio_desc;
	struct max14916_iio_desc_init_param max14916_iio_ip = {
		.max14916_ip = &max14916_ip,
		.ch_enable = {
				true, true, true, true
			}
		},
	};

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	ret = max14916_iio_init(&max14916_iio_desc, &max14916_iio_ip);
	if (ret)
		goto error;
	
	struct iio_app_device iio_devices[] = {
		{
			.name = "max14916",
			.dev = max14916_iio_desc,
			.dev_descriptor = max14916_iio_desc->iio_dev,
		}
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = max14916_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto error;
	
	return iio_app_run(app);
