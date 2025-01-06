LT3074 no-OS driver
====================

Supported Devices
-----------------

`LT3074 <https://www.analog.com/LT3074>`_

Overview
--------

The LT3074 is a low voltage, ultralow noise, and ultrafast transient response
linear regulator with a PMBus serial interface. The device supplies up to 3A
with a typical dropout voltage of 45mV. 

Applications
------------

LT3074
-------

* RF Power Supplies: PLLs, VCOs, Mixers, LNAs, PAs
* High Speed/High Precision Data Converters
* Low Noise Instrumentation
* Post-Regulator for Switching Supplies
* FPGA and DSP Power Supplies
* Medical Applications

LT3074 Device Configuration
----------------------------

Driver Initialization
---------------------

In order to be able to use the device, you will have to provide the support
for the communication protocol (I2C) alongside other GPIO pins if needed in the
specific application (depends on the way the device is used).

The first API to be called is **lt3074_init**. Make sure that it return 0,
which means that the driver was initialized correctly.

The initialization API uses the device descriptor and an initialization
parameter. The initialization parameter contains the optional GPIO pin
parameters for interfacing with ALERT and EN pins, and configurations for packet
error checking.

Status Bytes
------------

Assertion in the status bytes/words indicates fault/warning in device input/
output, temperature, and communication, memory and logic. These statuses can be
accessed via the **lt3074_read_status** API.

Telemetry
---------

Measurements for each output channel can be read using the
**lt3074_read_value** API. Some telemetry values includes input/output voltage,
input/output current, die temperature, and output power.

Overvalue and Undervalue Limits Configuration
---------------------------------------------

Overvalue and undervalue limits sets the threshold at which the device voltage,
current, and temperature must meet. When these measurements cross the limits, a
status bit may be asserted. These limits can be configured using the
**lt3074_set_limit** API.

VOUT Margin Configuration
-------------------------

The LT3074 output voltage margin is programmable from 0% to 30%. These can be
configured using the **lt3074_vout_margin** API.

LT3074 Driver Initialization Example
-------------------------------------

.. code-block:: bash

	struct lt3074_dev *lt3074_dev;
	struct no_os_i2c_init_param lt3074_i2c_ip = {
		.device_id = I2C_DEVICE_ID,
		.max_speed_hz = 100000,
		.platform_ops = I2C_OPS,
		.slave_address = LT3074_PMBUS_ADDRESS,
		.extra = I2C_EXTRA,
        };

	struct no_os_gpio_init_param lt3074_enable_ip = {
		.port = GPIO_EN_PORT,
		.number = GPIO_EN_NUMBER,
		.pull = NO_OS_PULL_UP,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA,
	};

	struct lt3074_init_param lt3074_ip = {
		.i2c_init = &lt3074_i2c_ip,
		.alert_param = NULL,
		.pg_param = NULL,
		.en_param = lt3074_enable_ip,
		.crc_en = false,
	};

	ret = lt3074_init(&lt3074_dev, &lt3074_ip);
	if (ret)
		goto error;

LT3074 no-OS IIO support
-------------------------

The LT3074 IIO driver comes on top of the LT3074 driver and offers support
for interfacing IIO clients through libiio.

LT3074 IIO Device Configuration
--------------------------------

Channels
--------

The device has a total of 3 input channels and 2 output channels. The input
consists of the input voltage, bias voltage, and the die temperature. The
output consists of the output voltage and current.

* ``vout - output voltage``
* ``iout - output current``
* ``vin - input voltage``
* ``vbias - bias voltage``
* ``temperature - die temperature``

Channel Attributes
------------------

EAch channels have 2 channel attributes:

* ``raw - the raw value of the channel``
* ``scale - the scale value of the channel calculated accordingly to each specific channel using a priv``

Global Attributes
-----------------

The device has a total of 12 global attributes:

* ``vout_margin_low - VOUT margin low limit``
* ``vout_margin_high - VOUT margin high limit``
* ``vout_margin_available - Available state of the vout_margin_low and vout_margin_high``
* ``vout_ov_warn_limit - Output overvoltage warning limit``
* ``vout_uv_warn_limit - Output undervoltage warning limit``
* ``iout_oc_fault_limit - Output overcurrent fault limit``
* ``ot_warn_limit - Overtemperature warning limit``
* ``vin_ov_warn_limit - Input overvoltage warning limit``
* ``vin_uv_warn_limit - Input undervoltage warning limit``
* ``vbias_ov_warn_limit - Bias overvoltage warning limit``
* ``vbias_uv_warn_limit - Bias undervoltage warning limit``
* ``iout_min_warn_limit - Output minimum current warning limit``

Debug Attributes
----------------

The device has a total of 8 debug attributes:

* ``status_byte - Status byte value``
* ``status_vout - VOUT status byte value``
* ``status_iout - IOUT status byte value``
* ``status_input - INPUT status byte value``
* ``status_mfr_specific - MFR_SPECIFIC status byte value``
* ``status_word - Status word value``
* ``status_temperature - TEMPERATURE status byte value of the device``
* ``status_cml - CML status byte value of the device``

LT3074 IIO Driver Initialization Example
-----------------------------------------

.. code-block:: bash

	int ret;

	struct lt3074_iio_desc *lt3074_iio_desc;
	struct lt3074_iio_desc_init_param lt3074_iio_ip = {
		.lt3074_init_param = &lt3074_ip,
	};

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	ret = lt3074_iio_init(&lt3074_iio_desc, &lt3074_iio_ip);
	if (ret)
		return ret;

	struct iio_app_device iio_devices[] = {
		{
			.name = "lt3074",
			.dev = lt3074_iio_desc,
			.dev_descriptor = lt3074_iio_desc->iio_dev,
		}
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = lt3074_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		return ret;

	return iio_app_run(app);
