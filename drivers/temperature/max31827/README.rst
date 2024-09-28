MAX31827 no-OS driver
=====================

Supported Devices
-----------------

`MAX31827 <https://www.analog.com/MAX31827>`_

`MAX31828 <https://www.analog.com/MAX31828>`_

`MAX31829 <https://www.analog.com/MAX31829>`_

Overview
--------

The MAX31827 is a ±1°C accuracy from -40°C to +125°C (12 bits) local temperature
switch and sensor with I2C/SMBus interface. The combination of small 6-bump
wafer-level package (WLP) and high accuracy makes this temperature sensor/switch
ideal for a wide range of applications. It can be used as a temperature switch
with preconfigured thresholds and/or as a temperature sensor with I2C interface.
When the part operates as an independent temperature switch, the I2C interface
doesn't have to be used. This enables use of the part in systems that require
thermal protection implemented in hardware, without the need for reconfiguration
or use of software/firmware during normal operation.

Applications
------------

MAX31827
--------

* Battery-Powered Equipment 
* Handheld Electronics 
* Data Communications Equipment 
* Servers 
* Industrial Equipment 

MAX31827 Device Configuration
-----------------------------

Driver Initialization
---------------------

In order to be able to use the device, you will have to provide the support
for the communication protocol (I2C).

The first API to be called is **max31827_init**. Make sure that it returns 0,
which means that the driver was initialized correctly.

Inside the **max31827_init** API, **max31827_init_client** API is called to
setup the values inside the Configuration Register.

MAX31827 Device Measurements
----------------------------

Temperature Data (Input)
------------------------

The data can be obtained by calling **max31827_read_temp_input** API. The
temperature is in milli-degrees Celsius, with scaling already applied.

Temperature Data (Thresholds and Hysteresis)
--------------------------------------------

The data can be obtained by calling **max31827_read_temp** API. The
temperature is in milli-degrees Celsius, with scaling already applied.

Shutdown Write
--------------

The **max31827_shutdown_write** API is used to write in certain registers that
needs the device to be in shutdown mode.

Before the Temperature Threshold Alarm, Alarm Hysteresis Threshold and
Resolution bits from Configuration register are changed over I2C, the part must
be in shutdown mode.

Write Alarm Temperature Values
------------------------------

The **max31827_write_alarm_val** API is used to write the Temperature Threshold
Alarm and Alarm Hysteresis Threshold temperature values.

MAX31827 Driver Initialization Example
--------------------------------------

.. code-block:: bash

	struct max31827_device *dev;

	struct no_os_uart_init_param uip = {
		.device_id = UART_DEVICE_ID,
		.baud_rate = UART_BAUDRATE,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.platform_ops = UART_OPS,
		.extra = UART_EXTRA,
	};

	const struct no_os_i2c_init_param max31827_i2c_ip = {
		.device_id = I2C_DEVICE_ID,
		.max_speed_hz = 100000,
		.slave_address = 0x42,
		.platform_ops = I2C_OPS,
		.extra = I2C_EXTRA,
	};

	struct max31827_init_param max31827_ip = {
		.i2c_init_param = max31827_i2c_ip,
		.comp_int = 0,
		.alarm_pol = 0,
		.fault_q = 1,
		.timeout_enable = 0,
	};

	ret = max31827_init(&dev, &max31827_ip);
	if (ret)
		goto error;

MAX31827 no-OS IIO support
--------------------------

The MAX31827 IIO driver comes on top of the MAX31827 driver and offers support
for interfacing IIO clients through libiio.

MAX31827 IIO Device Configuration
---------------------------------

Global Attributes
-----------------

The device has a total of 10 global attributes:

* ``temp_enable - enables or disables automatic mode``
* ``temp_input - Temperature input value in millidegree Celsius``
* ``temp_min - Minimum temperature threshold in millidegree Celsius``
* ``temp_min_hyst - Minimum temperature hysteresis threshold in millidegree Celsius``
* ``temp_min_alarm - Minimum temperature alarm state``
* ``temp_max - Maximum temperature threshold in millidegree Celsius``
* ``temp_max_hyst - Maximum temperature hysteresis threshold in millidegree Celsius``
* ``temp_max_alarm - Maximum temperature alarm state``
* ``temp_resolution - Temperature resolution in millidegree Celsius``
* ``update_interval - Update interval in milliseconds``

MAX31827 IIO Driver Initialization Example
------------------------------------------

.. code-block:: bash

	int ret;

	struct max31827_iio_device *max31827_iio_dev;
	struct max31827_iio_init_param max31827_iio_ip = {
		.max31827_init_param = &max31827_ip,
	};

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = {0};

	ret = max31827_iio_init(&max31827_iio_dev, &max31827_iio_ip);
	if (ret)
		goto exit;

	struct iio_app_device iio_devices[] = {
		{
			.name = "max31827",
			.dev = max31827_iio_dev,
			.dev_descriptor = max31827_iio_dev->iio_dev,
		},
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = uip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto remove_iio_max31827;

	return iio_app_run(app);
