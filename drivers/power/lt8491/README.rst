LT8491 no-OS driver
=====================

.. no-os-doxygen::

Supported Devices
-----------------

`LT8491 <https://www.analog.com/LT8491>`_

Overview
--------

The LT8491 is a buck-boost switching regulator battery charger that implements a
constant-current constant-voltage (CCCV) charging profile used for most battery
types, including sealed lead-acid (SLA), flooded, gel and lithium-ion.

The device operates from input voltages above, below or equal to the output
voltage and can be powered by a solar panel or a DC power supply. On-chip logic
provides automatic maximum power point tracking (MPPT) for solar powered
applications. The LT8491 can perform automatic temperature compensation by
sensing an external thermistor thermally coupled to the battery. The STATUS pin
can be used to drive an LED indicator lamp. The device is available in a low
profile (0.75mm) 7mm x 11mm 64-lead QFN package.

Applications
------------

LT8491
--------

* Solar Powered Battery Chargers
* Multiple Types of Lead-Acid Battery Charging
* Li-Ion Battery Charger
* Battery Equipped Industrial or Portable Military Equipment

LT8491 Device Configuration
-----------------------------

Driver Initialization
---------------------

In order to be able to use the device, you will have to provide the support
for the communication protocol (I2C).

The first API to be called is **lt8491_init**. Make sure that it returns 0,
which means that the driver was initialized correctly.

Restarting LT8491
-----------------

The startup sequence is initiated when the VDD pin rises above 2.7V (typical).
If the device is already powered on, the startup sequence can be re-initiated
via **lt8491_restart_chip** API. This resets the device register values.

Enabling or Disabling Charger
-----------------------------

Enabling or disabling charger is done using **lt8491_write_enable** API. This
can also be read using **lt8491_read_enable** API. Enabling charger starts
battery charging, restricts I2C write access, and commences telemetry
acquisition.

Charging Status
---------------

The charging status can be read via **lt8491_read_charging_status** API.

1 indicates that battery charging is ongoing in one of the 4 charging stages
(Stage 0 to 3).

0 indicates that the charging is not active, and the power stage has been turned
off. This is indicated due to any of the following conditions:

* The charging logic is disabled
* The charging has been stopped due to a fault condition
* The charging has reached the done charging stage
* The charging logic was recently enabled but the logic hasn't started Stage 0 charging yet

Charging Stage
--------------

The charging stage can be read via **lt8491_read_charging_stage** API. Values
read can be any of the following:

* '0' - Stage 0
* '1' - Stage 1
* '2' - Stage 2
* '3' - Stage 3
* 'd' - Done Charging

Configuring Telemetry
---------------------

Specific device registers must contain valid data before the LT8491 can properly
calculate telemetry for volts, amprs, power, and efficiency. The contents of
these registers don't affect any aspect of the charger's operation. The contents
of these registers are only used by the LT8491 to calculate the read-only
telemetry values.

These register values can be set via setting the resistor values inside
**lt8491_init_param** and using **lt8491_configure_telemetry** API.

Telemetry Acquisition
---------------------

* TBAT - battery temperature in millidegrees Celsius read via **lt8491_read_tbat** API
* POUT - power delivered out of the charger in milliWatts read via **lt8491_read_pout** API
* PIN - power draw from VIN supply in milliWatts read via **lt8491_read_pin** API
* EFF - power conversion efficiency in 0.001 % read via **lt8491_read_efficiency** API
* IOUT - current flowing out of the charger in milliamps read via **lt8491_read_iout** API
* IIN - current flowing into the charger in milliamps read via **lt8491_read_iin** API
* VBAT - battery voltage in millivolts read via **lt8491_read_vbat** API
* VIN - input voltage from the supply in millivolts read via **lt8491_read_vin** API

Using Scratch Pad Register
--------------------------

The scratch pad register can be read and write via **lt8491_read_scratch** and
**lt8491_write_scratch** APIs, respectively.

Serial Number
-------------

LT8491 has a unique serial number that consists of three (3) 16-bit numbers that
can be read via **lt8491_read_serial_id** API.

LT8491 Driver Initialization Example
--------------------------------------

.. code-block:: bash

	struct lt8491_device *dev;

	struct no_os_uart_init_param uip = {
		.device_id = UART_DEVICE_ID,
		.baud_rate = UART_BAUDRATE,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.platform_ops = UART_OPS,
		.extra = UART_EXTRA,
	};

	const struct no_os_i2c_init_param lt8491_i2c_ip = {
		.device_id = I2C_DEVICE_ID,
		.max_speed_hz = 100000,
		.slave_address = 0x10,
		.platform_ops = I2C_OPS,
		.extra = I2C_EXTRA,
	};

	struct lt8491_init_param lt8491_ip = {
		.i2c_init_param = lt8491_i2c_ip,
		.rsense1_micro_ohms = 5000,
		.rimon_out_ohms = 124000,
		.rsense2_micro_ohms = 3000,
		.rdaco_ohms = 64900,
		.rfbout1_ohms = 113000,
		.rfbout2_ohms = 10000,
		.rdaci_ohms = 7000,
		.rfbin2_ohms = 3480,
		.rfbin1_ohms = 10200,
	};

	ret = lt8491_init(&dev, &lt8491_ip);
	if (ret)
		goto error;

LT8491 no-OS IIO support
--------------------------

The LT8491 IIO driver comes on top of the LT8491 driver and offers support
for interfacing IIO clients through libiio.

LT8491 IIO Device Configuration
---------------------------------

Channel Attributes
------------------

LT8491 has a total of 10 channel attributes:

* ``in_temp_raw - raw battery temperature value``
* ``in_temp_scale - scale that has to be applied to the raw value in order to obtain the temperature value in mC``
* ``out_current_raw - raw current value flowing out of the charger``
* ``out_current_scale - scale that has to be applied to the raw value in order to obtain the current value in mA``
* ``in_current_supply_raw - raw current value flowing into the charger``
* ``in_current_supply_scale - scale that has to be applied to the raw value in order to obtain the current value in mA``
* ``out_voltage_raw - raw battery voltage value``
* ``out_voltage_scale - scale that has to be applied to the raw value in order to obtain the voltage value in mV``
* ``in_voltage_supply_raw - raw input voltage value``
* ``in_voltage_supply_scale - scale that has to be applied to the raw value in order to obtain the voltage value in mV``

Debug Attributes
----------------

The device has a total of 9 debug attributes:

* ``pout - Power delivered out of the charger in milliWatts``
* ``pin - Power draw from VIN supply in milliWatts``
* ``efficiency - Power conversion efficiency in 0.001 %``
* ``charging_stage - Charging stage``
* ``charging_status - Charging status``
* ``enable - Enable or disable charger``
* ``reset - Restart LT8491``
* ``scratch - Scratch pad register``
* ``serial_id - Serial number``

LT8491 IIO Driver Initialization Example
------------------------------------------

.. code-block:: bash

	int ret;

	struct lt8491_iio_device *lt8491_iio_dev;
	struct lt8491_iio_init_param lt8491_iio_ip = {
		.lt8491_init_param = &lt8491_ip,
	};

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = {0};

	ret = lt8491_iio_init(&lt8491_iio_dev, &lt8491_iio_ip);
	if (ret)
		goto exit;

	struct iio_app_device iio_devices[] = {
		{
			.name = "lt8491",
			.dev = lt8491_iio_dev,
			.dev_descriptor = lt8491_iio_dev->iio_dev,
		},
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = uip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto remove_iio_lt8491;

	return iio_app_run(app);
