MAX22190 no-OS driver
=====================

Supported Devices
-----------------

`MAX22190 <https://www.analog.com/MAX22190>`_

Overview
--------

The MAX22190 is an IEC 61131-2 compliant industrial digital input device.
The MAX22190 translates eight, 24V current-sinking, industrial inputs to a 
serialized SPI-compatible output that interfaces with 3V to 5.5V logic.
A current setting resistor allows the MAX22190 to be configured for Type 1, 
Type 2, or Type 3 inputs. Field wiring is verified for proximity switches, 
by a second threshold detector on each input. When wire-break is enabled, 
the active-low FAULT output is asserted and a register flag is set if the 
input current drops below the wire-break threshold for more than 20ms. 
Additional diagnostics that assert the active-ow FAULT pin include: 
overtemperature, low 24V field supply, 24V field supply missing, 
CRC communication error, etc.

Applications
------------

MAX22190
--------

* Building Automation
* Industrial Automation
* Process Automation
* Programmable Logic Controllers

MAX22190 Device Configuration
-----------------------------

Driver Initialization
---------------------

In order to be able to use the device, you will have to provide the support
for the communication protocol (SPI).

The first API to be called is **max22190_init**. Make sure that it returns 0,
which means that the driver was initialized correctly.

Channel Configuration
---------------------

Each channel's state can be set with the **max22190_chan_state** API.
The user can also set a wire break detection for the channel with
**max22190_wbe_set** API, and read the Wire Break state with
**max22190_wbe_get** API.

Filter Configuration
--------------------

Filter can be bypassed for each channel separately, although if they are not,
the user can set a filter delay.
All this can be done with the **max22190_filter_set** API, and also the user
can read the filter configuration for each channel with **max22190_filter_get**.

Fault Enable Configuration
--------------------------

Fault Enable bits can be configured with the help of the
**max22190_fault_enable_set** API and read with **max22190_fault_enable_get**.
If an enable bit is set, then the fault will be signaled in the fault register,
otherwise it will not.

MAX22190 Driver Initialization Example
--------------------------------------

.. code-block:: bash

	struct max22190_desc *max22190_desc;
	struct no_os_spi_init_param max22190_spi_ip = {
		.device_id = 0,
		.extra = &max22190_spi_extra,
		.max_speed_hz = 100000,
		.platform_ops = &max_spi_ops,
		.chip_select = 1,
	};
	struct max22190_init_param max22190_ip = {
		.comm_param = &max22190_spi_ip,
		.crc_en = false,
	};
	ret = max22190_init(&max22190_desc, &max22190_ip);
	if (ret)
		goto error;

MAX22190 no-OS IIO support
--------------------------

The MAX22190 IIO driver comes on top of the MAX22190 driver and offers support
for interfacing IIO clients through libiio.

MAX22190 IIO Device Configuration
---------------------------------

Channel Attributes
------------------

MAX22190 has a total of 6 channel attributes:

* ``raw - the state of a channel``
* ``offset - always 0``
* ``scale - always 1``
* ``filter_bypass - determines if the filter is used on the requested channel or not.``
* ``filter_delay - if the filter_bypass is set to 1, the filter's delay can be set from here.``
* ``filter_delay_available - list of possible filter delay's.``

Debug Attributes
----------------

 * ``fault1 - fault1 register value``
 * ``fault2 - fault2 register value``
 * ``wbg_enable - Wire Break detection asserted to fault pin``
 * ``24vm_enable - 24V Supply Missing detection asserted to fault pin``
 * ``24vl_enable - 24V Supply Low detection asserted to fault pin``
 * ``alrmt1_enable - Alarm 1 threshold detection asserted to fault pin``
 * ``alrmt2_enable - Alarm 2 threshold detection asserted to fault pin``
 * ``fault2_enable - Any bit from fault2 is asserted to fault pin``
 * ``rfwbs_enable - RFWB short detection is asserted to fault pin``
 * ``rfwbo_enable - RFWB open detection is asserted to fault pin``
 * ``rfdis_enable - RFDI short detection is asserted to fault pin``
 * ``rfdio_enable - RFDI open detection is asserted to fault pin``
 * ``otshdn_enable - Overtemperature shutdown``
 * ``fault8ck_enable - SPI receives bad number of clock pulses``

Device Channels
---------------

MAX22190 has a specific API, **max22190_iio_setup_channels** for configuring the
channels at the initialization, therefore the channels can be enabled or
disabled at initialization only.

MAX22190 IIO Driver Initialization Example
------------------------------------------

.. code-block:: bash

	int ret;

	struct max22190_iio_desc *max22190_iio_desc;
	struct max22190_iio_desc_init_param max22190_iio_ip = {
		.max22190_init_param = &max22190_ip,
		.ch_enabled = {
			true, true, true, false, false, false, false, false
		},
	};

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	ret = max22190_iio_init(&max22190_iio_desc, &max22190_iio_ip);
	if (ret)
		goto error;

	struct iio_app_device iio_devices[] = {
		{
			.name = "max22190",
			.dev = max22190_iio_desc,
			.dev_descriptor = max22190_iio_desc->iio_dev,
		},
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = max22190_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto app_error;

	return iio_app_run(app);
