AD7490 no-OS driver
===================

.. no-os-doxygen::

Supported Devices
-----------------

`AD7490 <https://www.analog.com/AD7490>`_

Overview
--------

The AD7490 is a 12-bit high speed, low power, 16-channel,
successive approximation ADC. The part operates from a single
2.7 V to 5.25 V power supply and features throughput rates up
to 1 MSPS. The part contains a low noise, wide bandwidth
track-and-hold amplifier that can handle input frequencies in
excess of 1 MHz.

Applications
------------

* Multichannel system monitoring
* Battery-powered equipment
* Power line monitoring
* Data acquisition, instrumentation, and process control

AD7490 Device Configuration
---------------------------

Driver Initialization
---------------------

In order to be able to use the device, you will have to provide the support for
the communication protocol (SPI).

The first API to be called is *ad7490_init*. Make sure that it returns 0,
which means that the driver was initialized correctly.

Operation Mode Configuration
----------------------------

AD7490 has a total of 4 operation modes:

* Normal Mode
* Full Shutdown Mode
* Auto Shutdown Mode
* Auto Standby Mode

By using the *ad7490_set_op_mode* API, one can configure the AD7490 to
work using the requested operation mode.

General Configuration
---------------------

Other configurations such as range can be either REFIN (2.5V) or double 
the REFIN (5V), and coding wich can be either twos complement or binary can
be configured by using *ad7490_config* API. Also the state of the DOUT pin
is also controlled using the same API.

AD7490 Device Measurements
--------------------------

Single Channel Readings
-----------------------

By using the *ad7490_read_ch* API, a single channel can be read, and the
output value is the raw value of the channel.

Multiple Channel Readings
-------------------------

Multiple channels can be read at once by using the sequencer, which can be
started by using the *ad7490_start_seq* API, where either a shadow, control
or consecutive type of sequencer cand be configured. (Please refer to the
datasheet for each type of sequencer).

More to this, the type of sequencer can be changed without stopping the sequence
by using *ad7490_change_seq* API, or stopped by using the *ad7490_stop_seq*.
API.

After starting the sequencer, in order to read the channel values, one should
use the *ad7490_read_seq* API which handles all the required operations
related to the sequencer and returning all requested channel values
in their ascending index order.

AD7490 Driver Initialization Example
------------------------------------

.. code-block:: bash

	struct ad7490_desc *ad7490;
	struct ad7490_init_param ad7490_ip = {
		.spi_param =  {
			.device_id = 4,
			.max_speed_hz = 100000,
			.mode = NO_OS_SPI_MODE_2,
			.chip_select = 0,
			.platform_ops = &max_spi_ops,
			.extra = &max_spi_extra,
		},
		.op_mode = AD7490_MODE_NORMAL,
		.vdd = AD7490_VDD_5V,
	};
	int ret, i;
	int16_t val;

	ret = ad7490_init(&ad7490, &ad7490_ip);
	if (ret)
		goto error;

AD7490 no-OS IIO Support
------------------------

The AD7490 IIO driver comes on top of AD7490 driver and offers support for
interfacing IIO clients through IIO lib.

IIO AD7490 Device Configuration
-------------------------------

Device Attributes
-----------------

AD7490 IIO device has a total of 2 device attributes:

* range - The range can be selected as REFIN (2.5V) or 2x REFIN (5V)
* polarity - The polarity can be selected as UNIPOLAR or BIPOLAR.
* sampling_frequency - The sampling frequency for AD7490 (1MSPS).

Device Channels
---------------

AD7490 IIO device has 16 input channels : 16 voltage channels.

The voltage channels are :

* Channel 0: Voltage
* Channel 1: Voltage
* Channel 2: Voltage
* Channel 3: Voltage
* Channel 4: Voltage
* Channel 5: Voltage
* Channel 6: Voltage
* Channel 7: Voltage
* Channel 8: Voltage
* Channel 9: Voltage
* Channel 10: Voltage
* Channel 11: Voltage
* Channel 12: Voltage
* Channel 13: Voltage
* Channel 14: Voltage
* Channel 15: Voltage

Each voltage channel has 2 attributes:

* raw - the raw voltage value read from the device.
* scale - is the scale that has to be applied to the raw value in order to obtain the converted real value in Volts. It depends on the selected range and has a specific formula.

In order to obtain the value in the V units, raw has to be scaled by scale:

.. code-block:: bash

	converted_voltage [V] = raw * scale
	converted_voltage [V] = raw * (range/4096)

Device buffers
--------------

The AD7490 IIO devices driver supports the usage of a data buffer for reading purposes.

AD7490 IIO Driver Initialization Example
----------------------------------------

.. code-block:: bash

	#define DATA_BUFFER_SIZE 400
	uint8_t iio_data_buffer[DATA_BUFFER_SIZE * 16 * sizeof(int16_t)];
	struct ad7490_iio_desc *ad7490_iio_desc;
	struct ad7490_iio_init_param ad7490_iio_ip;
	struct iio_app_desc *app;
	struct iio_data_buffer voltage_buff = {
		.buff = (void *)iio_data_buffer,
		.size = DATA_BUFFER_SIZE * 16 * sizeof(int16_t)
	};
	struct iio_app_init_param app_init_param = {0};
	int ret;

	ad7490_iio_ip.ad7490_init = &ad7490_ip;
	ad7490_iio_ip.dout_state = AD7490_DOUT_TRISTATE;
	ad7490_iio_ip.range = AD7490_RANGE_REFIN;
	ad7490_iio_ip.coding = AD7490_CODING_BINARY;
	ret = ad7490_iio_init(&ad7490_iio_desc, &ad7490_iio_ip);
	if (ret)
		return ret;

	struct iio_app_device iio_devices[] = {
		{
			.name = "ad7490",
			.dev = ad7490_iio_desc,
			.dev_descriptor = ad7490_iio_desc->iio_dev,
			.read_buff = &voltage_buff,
		},
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		return ret;

	return iio_app_run(app);
