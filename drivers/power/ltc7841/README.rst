LTC7841 no-OS driver
====================

Supported Devices
-----------------

`LTC7841 <https://www.analog.com/LTC7841>`_

Overview
--------

The LTC7841 is a high performance PolyPhase single output synchronous boost converter
controller that drives two N-channel power MOSFET stages out-of-phase. Multiphase operation
reduces input and output capacitor requirements and allows the use of smaller inductors than the
single-phase equivalent. Synchronous rectification increases efficiency, reduces power loss and
eases thermal requirements, enabling high power boost applications. The output voltage can be
adjusted up to 60V with 0.2% resolution via a PMBUS-compliant serial interface. The serial interface
can also be used to read back fault status, input/output current, input/output voltage and
temperature.

Applications
------------

LTC7841
-------

* Automotive Systems, Medical, Industrial

LTPC7841 Device Configuration
----------------------------

Driver Initialization
---------------------

In order to be able to use the device, you will have to provide the support
for the communication protocol (I2C).

The first API to be called is **ltc7841_init**. Make sure that it returns 0,
which means that the driver was initialized correctly.

Status Bytes
------------

Assertion in the status bytes/words indicates fault/warning in device input/
output, temperature, and communication, memory and logic. These statuses can be
accessed via the **ltc7841_read_status** API.

Telemetry
---------

Measurements for each output channel can be read using the
**ltc7841_read_value** API. Some telemetry values includes input/output voltage,
input/output current and die temperature.

VOUT Configuration
------------------

The LTC7841 output voltage is programmable from 0V to 48V. These can be
configured using the **ltc7841_vout_value** API.

LTC7841 Driver Initialization Example
-------------------------------------

.. code-block:: bash

	struct ltc7841_desc *ltp8800_desc;

	struct ltc7841_init_param ltc7841_ip = {
		.comm_param = {
		.device_id      = LTC7841_I2C_DEVICE_ID,
		.max_speed_hz   = LTC7841_I2C_CLK_SPEED,
		.slave_address  = LTC7841_I2C_ADDR,
		.platform_ops   = &max_i2c_ops,
		.extra          = (void *)&ltc7841_i2c_extra,
	    },
	    .regs = ltc7841_regs
	};

	ret = ltc7841_init(&ltp8800_desc, &ltc7841_ip);
	if (ret)
	{
		goto error;
	}

LTC7841 no-OS IIO support
-------------------------

The LTC7841 IIO driver comes on top of the LTC7841 driver and offers support
for interfacing IIO clients through libiio.

LTC7841 IIO Device Configuration
--------------------------------

Channels
--------

The device has a total of 3 input channels and 2 output channels. The input
consists of the input voltage, input current, and the forward diode
temperature. The output consists of the output voltage and current.

* ``vout - output voltage``
* ``iout - output current``
* ``vin - input voltage``
* ``iin - input current``
* ``temperature - forward diode temperature``

Channel Attributes
------------------

Each channels has 1 channel attribute:

* ``raw - the raw value of the channel``

Global Attributes
-----------------

The device has a total of 1 global attribute:

* ``vout_command - VOUT_COMMAND value of the channel output``

Debug Attributes
----------------

The device has a total of 1 debug attribute:

* ``status_word - Status word value``

LTC7841 IIO Driver Initialization Example
-----------------------------------------

.. code-block:: bash

	int ret;

	struct ltc7841_iio_desc *ltc7841_iio_desc;
	struct ltc7841_iio_desc_init_param ltc7841_iio_ip = 
    {
		.ltc7841_init_param = &ltc7841_ip,
	};

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	ret = ltc7841_iio_init(&ltc7841_iio_desc, &ltc7841_iio_ip);
	if (ret)
    {
		goto exit;
    }

	struct iio_app_device iio_devices[] = 
    {
		{
			.name = "ltc7841",
			.dev = ltc7841_iio_desc,
			.dev_descriptor = ltc7841_iio_desc->iio_dev,
		}
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
	{
		return ret;
	}

	return iio_app_run(app);
