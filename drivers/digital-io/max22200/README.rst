MAX22200 no-OS driver
=====================

Supported Devices
-----------------

`MAX22200 <https://www.analog.com/MAX22200>`

Overview
--------

The MAX22200 is an octal 36V serial-controlled solenoid driver.
Each channel features a low impedance (200mΩ typ) push-pull output stage with 
sink-and-source driving capability and up to 1ARMS driving current. 
A serial interface (SPI) that also supports daisy-chain configurations is 
provided to individually control each channel.

Applications
------------

* Relays Driver
* Solenoid, Valves, Electromagnetic Drivers
* Generic Low-Side and High-Side Switch Applications
* Latched (Bi-Stable) Solenoid Valve Drivers
* Brushed DC Motor Driver

MAX22200 Device Configuration
-----------------------------

In order to use the MAX22200, you will have to provide support for the
communication protocol (SPI), but also a GPIO for the CMD pin since before every
SPI transaction  there needs to be a SPI write to the Command Register while
CMD pin is logic high, and then performing the actual SPI transaction with CMD
pin logic low.
Also the ENABLE pin needs to be logic high for the device to be able to be
initialized.

The first API to be called is **max22200_init**. Make sure that it returns 0,
which means that the driver was initialized correctly.

Chopping frequency can also be changed with **max22200_set_chop_freq**.

External trigger configuration
------------------------------

An external trigger can be assigned to the TRIG pin, but in order for that to
work a GPIO initalization parameter for the trig pin needs to be set so the GPIO
is initialized and controlled by the master.
This can be used in both Half and Full Bridge modes for applications like
DC motor drivers.

The state of the trigger can be change with the help of
**max22200_set_trig_state** API.

Channel configuration
---------------------

Each channel can be configured in a lot of ways, but the first one of all is
the **max22200_set_ch_state** which is to be called by the user after the
initialization is performed correctly in order to activate a specific channel.

After that the channel's configuration can be changed on-the-fly.

HIT time and current can be changed with **max22200_set_ch_hit**.
HOLD current can be changed with **max222000_set_ch_hold**.

Scale can be set for a channel with **max22200_set_ch_scale** which for
half the fullscale will double the resistor and the current will be reduced
to half.

Channel trigger can be changed to be either controlled by the SPI interface
or the external trigger with **max22200_set_ch_trig**.

Channel mode and frequency can also be set with the help of
**max22200_set_ch_mode** and **max22200_set_ch_freq**.

And at last for every channel there are some specific enables that the user
can enable/disable with the help of **max22200_set_ch_enable**.

For every API to set a channel configuration in the CFG CH register there is
also a API to get a channel configuration form the CFG CH register.

Fault mask configuration
------------------------

Fault masks can be set for the whole device in the STATUS register with the
help of **max22200_fault_mask_set**, and read with **max22200_fault_mask_get**.

DPM configuration
-----------------

Detection of plunger movement can be configured as well with the help of the
**max22200_set_cfg_dpm** where the user can set specific values for the DPM.
The same specific values can be read with the help of **max22200_get_cfg_dpm**.

MAX22200 Driver Initialization Example
--------------------------------------

.. code-block:: bash

	struct max22200_desc *max22200;
	struct no_os_spi_init_param spi_ip = {
		.device_id = 0,
		.extra = &max22200_spi_extra,
		.max_speed_hz = 100000,
		.platform_ops = &max_spi_ops,
		.chip_select = 0,
	};
	struct no_os_gpio_init_param cmd_ip = {
		.port = 0,
		.pull = NO_OS_PULL_NONE,
		.number = 5,
		.platform_ops = &max_gpio_ops,
		.extra = &max22200_gpio_extra,
	};
	struct max22200_init_param max22200_ip = {
		.comm_desc = &spi_ip,
		.cmd_param = &cmd_ip,
		.ch_config = {
		MAX22200_INDEPENDENT_MODE,
		MAX22200_INDEPENDENT_MODE,
		MAX22200_INDEPENDENT_MODE,
		MAX22200_INDEPENDENT_MODE,
		}
	};

	ret = max22200_init(&max22200, &max22200_ip);
	if (ret)
		goto error;

Basic Example Channel Configuration Table
-----------------------------------------

+---------+---------+----------------+-------+------+-----------+-------+---------+
| Channel |  State  | Operation Mode | Drive | Side | Frequency | Scale | Trigger |
+---------+---------+----------------+-------+------+-----------+-------+---------+
|  OUT0   | Enabled |  HALF-BRIDGE   |  CDR  |  LS  |   40KHz   |   FS  |   SPI   |
+---------+---------+----------------+-------+------+-----------+-------+---------+
|  OUT1   | Enabled |  HALF-BRIDGE   |  CDR  |  LS  |   40KHz   |   FS  |   SPI   |
+---------+---------+----------------+-------+------+-----------+-------+---------+
|  OUT2   |Disabled |  INDEPENDENT   |  CDR  |  LS  |   80KHz   |   FS  |   SPI   |
+---------+---------+----------------+-------+------+-----------+-------+---------+
|  OUT3   |Disabled |  INDEPENDENT   |  CDR  |  LS  |   80KHZ   |   FS  |   SPI   | 
+---------+---------+----------------+-------+------+-----------+-------+---------+
|  OUT4   |Disabled |  INDEPENDENT   |  CDR  |  LS  |   80KHZ   |   FS  |   SPI   |
+---------+---------+----------------+-------+------+-----------+-------+---------+
|  OUT5   |Disabled |  INDEPENDENT   |  CDR  |  LS  |   80KHZ   |   FS  |   SPI   |
+---------+---------+----------------+-------+------+-----------+-------+---------+
|  OUT6   |Disabled |  INDEPENDENT   |  CDR  |  LS  |   80KHZ   |   FS  |   SPI   |
+---------+---------+----------------+-------+------+-----------+-------+---------+
|  OUT7   |Disabled |  INDEPENDENT   |  CDR  |  LS  |   80KHZ   |   FS  |   SPI   |
+---------+---------+----------------+-------+------+-----------+-------+---------+

MAX22200 no-OS IIO support
--------------------------

Channel Attributes
------------------

* raw - state of the channel.
* scale - scale of the channel.
* scale_available - available scales for the channel.
* hit_current - HIT current to be set in the CFG_CH register.
* hit_time - HIT time to be set in the CFG_CH register.
* hold_current - HOLD current to be set in the CFG_CH register.
* channel_trig - ONCH_SPI/EXT_TRIG trigger selection for the channel.
* channel_trig_available - available trigger for the channel.
* channel_drive - Channel's voltage or current drive.
* channel_drive_available - Channel available drives.
* channel_side - LOW/HIGH SIDE selection for the channel.
* channel_side_available - Available sides for the channel.
* channel_op_mode - Channel operation mode selection.
* channel_op_mode_available - Channel operation modes available for the channel.
* channel_freq - Channel frequency to be selected. Calculated using the device frequency.
* channel_freq_available - Channel frequencies available for the channel.
* slew_rate_control - Slew Rate Control enable/disable.
* open_load_detection - Open Load Detection enable/disable.
* dpm_enable - Detection of Plunger Movement enable/disable.
* hit_current_check - HIT current check enable/disable.

Global Attributes
-----------------

* chopping_frequency - Chopping frequency selection.
* chopping_frequency_available  Chopping frequencies available for the device.
* external_trig_state - External trigger's state (enabled = 1/disabled = 0).

Debug Attributes
----------------

* thermal_protection_mask - Mask for the thermal protection of the device.
* overcurrent_protection_mask - Mask for the overcurrent protection of the device.
* open_load_mask - Mask for the open load detection of the device.
* hit_current_fault_mask - Mask for the hit current fault of the device.
* dpm_fault_mask - Mask for the detection of plunger movement fault of the device.
* com_err_mask - Mask for the communication error detection of the device.
* uvlo_mask - Mask for the Undervoltage Low detection of the device.

Device Channels
---------------

MAX22200 has a specific API, **max22200_iio_setup_channels** for configuring the
channels at the initialization, therefore the channels can be configured as  
enabled/disabled and attributes are assigned to each channel (if enabled).

MAX22200 IIO Driver Initialization Example
------------------------------------------

.. code-block:: bash

	struct max22200_iio_desc *max22200_iio_desc;
	struct max22200_iio_desc_init_param max22200_iio_ip = {
		.max22200_init_param = &max22200_ip,
		.ch_enable = {
			true, false, false, false, false, false, false, false
		},
	};

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	ret = max22200_iio_init(&max22200_iio_desc, &max22200_iio_ip);
	if (ret)
		goto exit;

	struct iio_app_device iio_devices[] = {
		{
			.name = "max22200",
			.dev = max22200_iio_desc,
			.dev_descriptor = max22200_iio_desc->iio_dev,
			.read_buff = NULL,
		},
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = max22200_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto remove_iio_max22200;

	ret = iio_app_run(app);
