MAX22017 no-OS Driver
=====================

.. no-os-doxygen::

Supported Devices
-----------------

`MAX22017 <https://www.analog.com/MAX22017>`_

Overview
--------

The MAX22017 is a two-channel industrial-grade 
configurable analog output device that can be changed 
on-the-fly in software to voltage or current output mode. 
The MAX22017 offers one 16-bit DAC per output 
channel. The MAX22017 can operate from an internal or 
external reference.

MAX22017 Device Configuration
-----------------------------

Driver Initialization
---------------------

In order to be able to use the device, you will have to provide the support for
the communication protocol (SPI) as well as an external GPIO for the RSTB pin.

The first API to be called is **max22017_init**. Make sure that it returns 0,
which means that the driver was intialized correctly.

GPIO Controller Configuration
-----------------------------

Both devices have an internal GPIO controller, only difference is that MAX22017
supports 6 GPIOs meanwhile MAX22018 only supports 4 GPIOs.
In the driver files the **max22017_gpio_ops** can be found and used when
declaring a no-OS GPIO initializaon parameter, therefor all the platform
specific operation are called from the no-OS GPIO API as a standalone platform.

Channel Configuration
---------------------

Channel data can be set/get with the **max22017_set_data** and
**max22017_get_data**.

Channel slew-rate configuration can also be set using **max22017_slew_rate**

Channel gain and offset can be calibrated with **max22017_offset_calib** and
**max22017_gain_calib**.

Channel operation mode can also be configured using **max22017_op_mode** as well
as other channel specific configurations that can be changed using
**max22017_config**

Soft Reset
----------

MAX22017 can be soft reseted by using **max22017_soft_reset**.

MAX22017 Driver Initialization Example
--------------------------------------

.. code-block:: bash

	struct max22017_desc *max22017;
	struct no_os_spi_init_param spi_ip = {
		.device_id = 0,
		.extra = &max22017_spi_extra,
		.max_speed_hz = 100000,
		.platform_ops = &max_spi_ops,
		.chip_select = 0,
	};
	struct no_os_gpio_init_param max22017_rstb_ip = {
		.port = 1,
		.pull = NO_OS_PULL_NONE,
		.number = 6,
		.platform_ops = &max_gpio_ops,
		.extra = &max22017_gpio_extra,
	};
	struct max22017_init_param max22017_ip = {
		.chip_id = ID_MAX22017,
		.comm_param = &max22017_spi_ip,
		.crc_en = false,
		.ext_dac_ref = false,
		.rstb_param = &max22017_rstb_ip,
	};

	ret = max22017_init(&max22017, &max22017_ip);
	if (ret)
		goto error;

MAX22017 no-OS IIO Support
--------------------------

The MAX22017 IIO driver comes on top of the MAX22017 driver and offers support
for interfacing IIO clients through IIO lib.

MAX22017 IIO Device Configuration
---------------------------------

Device Attributes
-----------------

Channel Attributes
------------------

MAX22017 has a total of 2 channel attributes:

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

MAX22017 has a total of 2 channels.

max22017 IIO Driver Initialization Example
------------------------------------------

.. code-block:: bash

	int ret;

	struct max22017_iio_desc *max22017_iio_desc;
	struct max22017_iio_desc_init_param max22017_iio_ip = {
		.max22017_init_param = &max22017_ip,
	};

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	ret = max22017_iio_init(&max22017_iio_desc, &max22017_iio_ip);
	if (ret)
		goto error;

	struct iio_app_device iio_devices[] = {
		{
			.name = "max22017",
			.dev = max22017_iio_desc,
			.dev_descriptor = max22017_iio_desc->iio_dev,
		},
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = max22017_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto app_error;

	return iio_app_run(app);
