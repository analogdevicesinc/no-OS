ADM1177 no-OS Driver
====================

Supported Devices
-----------------

`ADM1177 <https://www.analog.com/ADM1177>`_

Overview
--------

The ADM1177 is an integrated hot swap controller that offers digital current
and voltage monitorng via an on-chip, 12 bit analog-to-digital converter (ADC),
communicated through an I2C interface.

The 12 bit ADC can measure the current seen in the sense resistor, as well as
the supply voltage on the VCC pin. Measurements can be initiated by an I2C 
command.

Applications
------------

* Power monitoring/power budgeting
* Central office equipment
* Telecommunications and data communications equipment PCs/servers

ADM1177 Device Configuration
----------------------------

Driver Initialization
---------------------

In order to be able to use the device, you will have to provide the support for
the communication protocol (I2C) as mentioned above.

The first API to be called is **adm1177_init**. Make sure that it returns 0,
which means that the driver was initialized correctly.

Range Configuration
-------------------

By default, the device uses 26.35V range configuration. You may modify this
value to 6.65V by using **adm1177_set_vrange** API.

ADM1177 Device Measurements
---------------------------

Write Operation
---------------

In order to communicate to the ADC what data you want to read, you will need to
call the **adm1177_write** API, and at the reg_address put 0 so you will perform
a write command operation.

If you want to perform a write extended command operation, you will need to call
the same API but at the reg_address put the specific register address
corresponding to the extended command you want to perform.

Status Read
-----------

In order to read the status, after you used the **adm1177_write** API, with the
command corresponding to the Status Reading Operation, you will need to call
the **adm1177_read_status** API and a status_byte will be returned through the
status_byte pointer.

Conversion Data
---------------

In order to read the Current, Voltage or Both data form the ADC, after you used
the **adm1177_write** API to tell the ADC you need Voltage, Current or Both,
use the **adm1177_read_conv** API, and voltage and current data will be received
through conv_voltage and conv_current pointers.

Raw Sample to True value
------------------------

If you want the raw data received from the ADC through the previously mentioned
pointers converted to real-life values, use the **adm1177_to_microvolts** API
for voltage and **adm1177_to_microampers** API for current.

ADM1177 Driver Initialization Example
-------------------------------------

.. code-block:: bash

	struct adm1177_dev *adm1177;
	//Particular I2C Configuration
	struct adm1177_init_param adm1177_ip = {
		.i2c_init = {
			.device_id = I2C_DEVICE_ID,
			.slave_address = ADM1177_ADDRESS,
			.extra = I2C_EXTRA,
			.platform_ops = I2C_OPS,
			.max_speed_hz = 100000,
		},
	};
	uint8_t status_byte;
	uint16_t conv_voltage, conv_current;
	uint64_t voltage, current;

	ret = adm1177_init(&adm1177, &adm1177_ip);
	if(ret)
		goto error;

	// Initialize the continuous voltage and crurrent readback.
	ret = adm1177_write(adm1177, ADM1177_CMD_V_CONT | ADM1177_CMD_I_CONT, 0);
	if(ret)
		goto error;

	ret = adm1177_read_conv(adm1177, &conv_voltage, &conv_current);
	if(ret)
		goto error;

	ret = adm1177_to_microvolts(adm1177, conv_voltage, &voltage);
	if(ret)
		goto error;
	
	ret = adm1177_to_microampers(conv_current, &current);
	if(ret)
		goto error;

ADM1177 no_OS IIO Support
-------------------------

The ADM1177 IIO driver comes on top of ADM1177 driver and offers support for interfacing IIO clients through IIO lib.

ADM1177 Device Configuration
----------------------------

Device Attributes
-----------------

ADM1177 IIO device does not have any device specific attributes.

Device Channels
---------------

ADM1177 IIO device has 0 output channels and 2 input channels: 1 voltage channel and 1 current channel.

Voltage Channel
---------------

The voltage channel is: 

* Channel 0: Voltage

The voltage channel has 2 attributes:

* raw - the raw voltage value read from the device.
* scale - is the scale that has to be applied to the raw value in order to obtain the converted real value in Volts. It depends on the ADC V Range and has a specific formula.

In order to obtain the value in the V units, raw has to be scaled by scale:

.. code-block:: bash

	converted_voltage [V] = raw * scale
	converted_voltage [V] = raw * (vrange/4096)

Current Channel
---------------

The voltage channel is: 

* Channel 1: Current

The voltage channel has 2 attributes:

* raw - the raw currrent value read from the device.
* scale - is the scale that has to be applied to the raw value in order to obtain the converted real value in mA. It depends on the ADC I_FULLSCALE and has a specific formula.

In order to obtain the value in the mA units, raw has to be scaled by scale:

.. code-block:: bash

	converted_current [V] = raw * scale
	converted_current [V] = raw * (I_FULLSCALE*R_SENSE/4096)

Device buffers
--------------

The ADM1177 IIO devices driver supports the usage of a data buffer for reading purposes.

ADM1177 IIO Driver Initialization Example
-----------------------------------------

.. code-block:: bash

	#define DATA_BUFFER_SIZE 400
	uint16_t iio_data_buffer[DATA_BUFFER_SIZE*2*sizeof(int)];

	struct adm1177_iio_dev *adm1177_iio_dev = NULL;
	struct adm1177_iio_init_param adm1177_iio_init_param;

	struct iio_app_desc *app;
	struct iio_data_buffer adm1177_buff = {
		.buff = (void *)iio_data_buffer,
		.size = DATA_BUFFER_SIZE*2*sizeof(int),
	};
	struct iio_app_init_param app_init_param = { 0 };

	struct adm1177_init_param adm1177_ip = {
		.i2c_init = {
			.device_id = I2C_DEVICE_ID,
			.slave_address = ADM1177_ADDRESS,
			.extra = I2C_EXTRA,
			.platform_ops = I2C_OPS,
			.max_speed_hz = 100000,
		},
	};

	adm1177_iio_init_param.adm1177_initial = &adm1177_ip;
	ret = adm1177_iio_init(&adm1177_iio_dev, &adm1177_iio_init_param);
	if(ret)
		goto error;

	struct iio_app_device iio_devices[] = {
		{
			.name = "adm1177",
			.dev = adm1177_iio_dev,
			.dev_descriptor = adm1177_iio_dev->iio_dev,
			.read_buff = &adm1177_buff,
		},
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if(ret)
		goto error;

	return iio_app_run(app);
