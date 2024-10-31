LTC4162L no-OS driver
=====================

Supported Devices
-----------------

`LTC4162L <https://www.analog.com/LTC4162L>`_
`LTC4162F <https://www.analog.com/LTC4162F>`_
`LTC4162S <https://www.analog.com/LTC4162S>`_
`LTC4015 <https://www.analog.com/LTC4015>`_

Overview
--------

The LTC4162-L is an advanced monolithic synchronous step-down switching battery
charger and PowerPath™ manager that seamlessly manages power distribution
between input sources such as wall adapters, backplanes, solar panels, etc., and
a rechargeable Lithium-Ion/Polymer battery. 

A high-resolution measurement system provides extensive telemetry information
for circuit voltages, currents, battery resistance and temperature which can all
be read back over the I2C port. The I2C port can also be used to configure many
charging parameters including charging voltages and currents, termination
algorithms and numerous system status alerts.

The LTC4162-L can charge Lithium-Ion cell stacks from 1 cell to 8 cells with as 
much as 3.2A of charge current. The power path topology decouples the output 
voltage from the battery allowing a portable product to start up instantly under 
very low battery voltage conditions. The LTC4162-L is available in a thermally 
enhanced 28-pin 4mm x 5mm x 0.75mm QFN surface mount package.

Applications
------------

LTC4162L
--------

* Medical Instruments
* USB-C Power Delivery
* Industrial Handhelds
* Ruggedized Notebooks
* Tablet Computers

LTC4162L Device Configuration
-----------------------------

Driver Initialization
---------------------

In order to be able to use the device, you will have to provide the support
for the communication protocol (I2C).

The first API to be called is **ltc4162l_init**. Make sure that it returns 0,
which means that the driver was initialized correctly.

Battery Count
-------------

The battery count can be acquired via **ltc4162l_get_cell_count** API.

Battery Chemistry Type
----------------------

The battery chemistry type can be acquired via **ltc4162l_get_chem_type** API.

The following read register values indicates the chemistry type of the battery:

* LTC4162_LAD = 0
* LTC4162_L42 = 1
* LTC4162_L41 = 2
* LTC4162_L40 = 3
* LTC4162_FAD = 4
* LTC4162_FFS = 5
* LTC4162_FST = 6
* LTC4162_SST = 8
* LTC4162_SAD = 9

Battery Charge State
--------------------

The battery charge state can be acquired via **ltc4162l_get_charger_state** API.

The following read register values indicates the charge state of the battery:

* bat_detect_failed_fault = 4096,
* battery_detection = 2048,
* charger_suspended = 256,
* precharge = 128,
* cc_cv_charge = 64,
* ntc_pause = 32,
* timer_term = 16,
* c_over_x_term = 8,
* max_charge_time_fault = 4,
* bat_missing_fault = 2,
* bat_short_fault = 1

Charge Status
--------------

The charging stage can be read via **ltc4162l_read_charge_status** API. Values
read can be any of the following:

* ilim_reg_active = 32,
* thermal_reg_active = 16,
* vin_uvcl_active = 8,
* iin_limit_active = 4,
* constant_current = 2,
* constant_voltage = 1,
* charger_off = 0

Battery Online Status
---------------------

The battery charge state can be acquired via **ltc4162l_get_charger_state** API.

Indicates that the VIN pin voltage is sufficiently above the battery voltage to 
begin a charge cycle (typically +150mV).

LTC4162L Driver Initialization Example
--------------------------------------

.. code-block:: bash

        struct ltc4162l_device *dev;

	struct no_os_uart_init_param uip = {
		.device_id = UART_DEVICE_ID,
		.irq_id = UART_IRQ_ID,
		.asynchronous_rx = true,
                .baud_rate = UART_BAUDRATE,
                .size = NO_OS_UART_CS_8,
                .parity = NO_OS_UART_PAR_NO,
                .stop = NO_OS_UART_STOP_1_BIT,
                .platform_ops = UART_OPS,
                .extra = UART_EXTRA,
	};

	const struct no_os_i2c_init_param ltc4162l_i2c_ip = {
                .device_id = I2C_DEVICE_ID,
                .max_speed_hz = 100000,
                .slave_address = 0x68,
                .platform_ops = I2C_OPS,
                .extra = I2C_EXTRA,
	};

	struct ltc4162l_init_param ltc4162l_ip = {
                .i2c_init_param = ltc4162l_i2c_ip,
                .rsnsb = 1000,
                .rsnsi = 1000,
                .type = ID_LTC4162L,
	};

	ret = ltc4162l_init(&dev, &ltc4162l_ip);
	if (ret)
		goto error;


LTC4162L no-OS IIO support
--------------------------

The LTC4162L IIO driver comes on top of the LTC4162L driver and offers support
for interfacing IIO clients through libiio.

LTC4162L IIO Device Configuration
---------------------------------

Channel Attributes
------------------

LTC4162L has a total of 6 channel attributes:

* ``in_vbat_raw - raw battery voltage value``
* ``in_avg_vbat_raw - raw average battery voltage value``
* ``in_ibat_raw - raw battery current value``
* ``in_voltage_raw - raw input voltage value``
* ``in_current_raw - raw input current value``
* ``in_temp_raw - raw battery temperature value``

Debug Attributes
----------------

The device has a total of 9 debug attributes:

* ``cell_count - Number of cells``
* ``chem_type - Battery chemistry type``
* ``charger_status - Battery charging status``
* ``charge_state - Battery charge state``
* ``online_status - Battery online status``
* ``icharge - Charge current``
* ``vcharge - Charge voltage``
* ``term_current - Termination current``
* ``telemetry - Telemetry data``

LTC4162L IIO Driver Initialization Example
------------------------------------------

.. code-block:: bash

	int ret;

	struct ltc4162l_iio_device *ltc4162l_iio_dev;
	struct ltc4162l_iio_init_param ltc4162l_iio_ip = {
		.ltc4162l_init_param = &ltc4162l_ip,
	};

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = {0};

	ret = ltc4162l_iio_init(&ltc4162l_iio_dev, &ltc4162l_iio_ip);
	if (ret)
		goto exit;
	struct iio_app_device iio_devices[] = {
		{
			.name = "ltc4162l",
			.dev = ltc4162l_iio_dev,
			.dev_descriptor = ltc4162l_iio_dev->iio_dev,
		},
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = uip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto remove_iio_ltc4162l;

	return iio_app_run(app);
