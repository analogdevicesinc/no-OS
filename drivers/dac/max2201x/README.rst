MAX2201X no-OS Driver
=====================

Supported Devices
-----------------

`MAX22017 <https://www.analog.com/MAX22017>`_
`MAX22018 <https://www.analog.com/MAX22018>`_

Overview
--------

The MAX22017 is a two-channel industrial-grade 
configurable analog output device that can be changed 
on-the-fly in software to voltage or current output mode. 
The MAX22017 offers one 16-bit DAC per output 
channel. The MAX22017 can operate from an internal or 
external reference.

The MAX22018 is a single-channel industrial-grade 
configurable analog output device that can be changed 
on-the-fly in software to voltage or current output mode. 
The MAX22018 can operate from an internal or external 
reference. The MAX22018 features zero-drift high-voltage amplifiers
and high voltage switches to be compliant with standard industrial-grade
voltage and current ranges.

Both MAX22017 and MAX22018 have pretty much the same register map except
channel's 2 registers in case of MAX22018, therefor the max2201x.c and
max2201x.h driver files support both devices.

MAX2201X Device Configuration
-----------------------------

Driver Initialization
---------------------

In order to be able to use the device, you will have to provide the support for
the communication protocol (SPI) as well as an external GPIO for the RSTB pin.

The first API to be called is **max2201x_init**. Make sure that it returns 0,
which means that the driver was intialized correctly.

GPIO Controller Configuration
-----------------------------

Both devices have an internal GPIO controller, only difference is that MAX22017
supports 6 GPIOs meanwhile MAX22018 only supports 4 GPIOs.
In the driver files the **max2201x_gpio_ops** can be found and used when
declaring a no-OS GPIO initializaon parameter, therefor all the platform
specific operation are called from the no-OS GPIO API as a standalone platform.

Channel Configuration
---------------------

Channel data can be set/get with the **max2201x_set_data** and
**max2201x_get_data**.

Channel slew-rate configuration can also be set using **max2201x_slew_rate**

Channel gain and offset can be calibrated with **max2201x_offset_calib** and
**max2201x_gain_calib**.

Channel operation mode can also be configured using **max2201x_op_mode** as well
as other channel specific configurations that can be changed using
**max2201x_config**

Soft Reset
----------

Both devices can be soft reseted by using **max2201x_soft_reset**.

MAX2201X Driver Initialization Example
--------------------------------------

.. code-block:: bash

	struct max2201x_desc *max2201x;
	struct no_os_spi_init_param spi_ip = {
		.device_id = 0,
		.extra = &max2201x_spi_extra,
		.max_speed_hz = 100000,
		.platform_ops = &max_spi_ops,
		.chip_select = 0,
	};
	struct no_os_gpio_init_param max2201x_rstb_ip = {
		.port = 1,
		.pull = NO_OS_PULL_NONE,
		.number = 6,
		.platform_ops = &max_gpio_ops,
		.extra = &max2201x_gpio_extra,
	};
	struct max2201x_init_param max2201x_ip = {
		.chip_id = ID_MAX22017,
		.comm_param = &max2201x_spi_ip,
		.crc_en = false,
		.ext_dac_ref = false,
		.rstb_param = &max2201x_rstb_ip,
	};

	ret = max2201x_init(&max2201x, &max2201x_ip);
	if (ret)
		goto error;

MAX2201X no-OS IIO Support
--------------------------

The MAX2201X IIO driver comes on top of the MAX2201X driver and offers support
for interfacing IIO clients through IIO lib.

MAX2201X IIO Device Configuration
---------------------------------

Device Attributes
-----------------

Channel Attributes
------------------

MAX2201X has a total of 2 channel attributes:

* ``raw - the output data to be transmitted to the channel's DAC``
* ``offset - offset value used for channel's offset calibration``

Debug Attributes
----------------

The device has a total of 37 debug attributes.

* ``ow_detect - Open Wire detection status``
* ``ow_detect_available - List of possible options for Open Wire detection``
* ``timeout - Timeout value of the device``
* ``timeout_available - List of possible values for the timeout``
* ``timeout_cfg - Timeout Configuration``
* ``timeout_cfg_available - List of possible options for the timeout configuration``
* ``timeout_en - Timeout enable/disable``
* ``timeout_en_available - List of possible options for timeout status``
* ``thermal_shutdown - Thermal Shutdown status``
* ``thermal_shutdown_available - List of possible options for the thermal shutdown status``
* ``overcurrent_shutdown - Overcurrent Shutdown status``
* ``overcurrent_shutdown_available - List of possible options for the overcurrrent shutdown status``
* ``current_limit - Current Limit value``
* ``current_limit_available - List of possible values for current limit``
* ``dac_ref - DAC reference status``
* ``dac_ref_available - List of possible options for the DAC reference status``
* ``interrupts - Interrupts Word value``
* ``interrupts_enable - Interrups Enable Word value``
* ``ld_ctrl - Load DAC ctrl configuration``
* ``ld_ctrl_available - List of possible options for Load DAC ctrl configuration``
* ``gain - gain value used for channel's gain calibration``
* ``slew_rate - enable/disable slew rate``
* ``slew_rate_available - list of possible options for slew rate status``
* ``range - Channel's AO slew rate range``
* ``range_available - List of possible ranges depending on the application``
* ``step_size - Step size of the slew rate``
* ``step_size_available - list of possible values for the step size``
* ``update_rate - Update rate valuof the Channel's AO``
* ``update_rate_available - List of possible update rates``
* ``operation_mode - Operation mode of the channel``
* ``operation_mode_available - List of possible operation modes``
* ``polarity - Polarity of the channel``
* ``polarity_available - List of possible polarities``
* ``cm_sense - Common mode sense restistor value``
* ``cm_sense_available - List of possible values for the common mode resistor``
* ``ldac - Load DAC status``
* ``ldac - List of possible options for Load DAC status``

Device Channels
---------------

MAX2201X has a specific API, **max2201x_iio_setup_channels** for configuring the
channels at initialization depending on the chip id. Therefore the channels can
be enabled/disabled only at initialization.

MAX2201X IIO Driver Initialization Example
------------------------------------------

.. code-block:: bash

	int ret;

	struct max2201x_iio_desc *max2201x_iio_desc;
	struct max2201x_iio_desc_init_param max2201x_iio_ip = {
		.max2201x_init_param = &max2201x_ip,
	};

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	ret = max2201x_iio_init(&max2201x_iio_desc, &max2201x_iio_ip);
	if (ret)
		goto error;

	struct iio_app_device iio_devices[] = {
		{
			.name = "max2201x",
			.dev = max2201x_iio_desc,
			.dev_descriptor = max2201x_iio_desc->iio_dev,
		},
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = max2201x_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto app_error;

	return iio_app_run(app);
