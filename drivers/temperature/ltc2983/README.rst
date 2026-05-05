LTC2983 no-OS driver
====================

.. no-os-doxygen::

Supported Devices
-----------------

`LTC2983 <https://www.analog.com/LTC2983>`_

`LTC2984 <https://www.analog.com/LTC2984>`_

`LTC2986 <https://www.analog.com/LTC2986>`_

`LTM2983 <https://www.analog.com/LTM2985>`_

ADT7604

Overview
--------

The LTC2983 measures a wide variety of temperature sensors and digitally
outputs the result, in °C or °F, with 0.1°C accuracy and 0.001°C resolution. The
LTC2983 can measure the temperature of virtually all standard (type B, E, J, K,
N, S, R, T) or custom thermocouples, automatically compensate for cold junction
temperatures and linearize the results. The device can also measure temperature
with standard 2-, 3- or 4-wire RTDs, thermistors and diodes. It has 20
reconfigurable analog inputs enabling many sensor connections and configuration
options. The LTC2983 includes excitation current sources and fault detection
circuitry appropriate for each type of temperature sensor.

The LTC2983 allows direct interfacing to ground referenced sensors without the
need for level shifters, negative supply voltages, or external amplifiers. All
signals are buffered and simultaneously digitized with three high accuracy,
24-bit ∆∑ ADCs, driven by an internal 15ppm/°C (maximum) reference.

LTC2984, LTC2986, and LTM2985 include an EEPROM for storing custom coefficients
and channel configuration data.

The LTC2986/LTC2986-1 are 10-channel software and pin-compatible versions of the
20-channel LTC2983/LTC2984. Additional features include special modes that
enable easy protection in universal multi-sensor applications, custom tables for
generic ADC readings, and direct temperature readout from active analog
temperature sensors. The LTC2986-1 is the EEPROM version of the LTC2986.

The LTM2985 is an isolated 10-channel temperature measurement system software
compatible with the LTC2986-1. It provides 5kV isolated power and SPI interface
to the precision temperature-to-bits converter.

Applications
------------

LTC2983
-------

* Direct Thermocouple Measurements
* Direct RTD Measurements
* Direct Thermistor Measurements
* Custom Sensor Applications

LTC2983 Device Configuration
----------------------------

Driver Initialization
---------------------

In order to be able to use the device, you will have to provide the support
for the communication protocol (SPI) alongside RST GPIO pin.

The first API to be called is **ltc2983_init**. Make sure that it return 0,
which means that the driver was initialized correctly.

Inside the **ltc2983_init** API, **ltc2983_setup** API is called to setup the
values of the device registers including Channel Assignment.

LTC2983 Device Measurements
----------------------------

Temperature Data
-----------------

In case the operation mode selected includes the temperature measurement, then
data can be obtained by calling **ltc2983_chan_read** API. The temperature is in
millidegrees Celsius, with scaling already applied. 

If you want to obtain the raw temperature data without any scaling applies,
simply call **ltc2983_chan_read_raw** API.

LTC2983 Driver Initialization Example
-------------------------------------

.. code-block:: bash

	struct ltc2983_dev *ltc2983_dev;
	struct ltc2983_rsense ltc2983_rsense_2;
	struct ltc2983_thermistor ltc2983_thermistor_8;
	struct ltc2983_thermistor ltc2983_thermistor_10;

	struct no_os_uart_init_param uip = {
		.device_id = UART_DEVICE_ID,
		.baud_rate = UART_BAUDRATE,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.platform_ops = UART_OPS,
		.extra = UART_EXTRA,
	};

	const struct no_os_spi_init_param ltc2983_spi_ip = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = SPI_MAX_SPEED,
		.chip_select = SPI_CS,
		.mode = NO_OS_SPI_MODE_0,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.platform_ops = SPI_OPS,
		.extra = SPI_EXTRA,
		.parent = NULL,
	};

	const struct no_os_gpio_init_param ltc2983_gpio_rstn = {
		.port = GPIO_RSTN_PORT_NUM,
		.number = GPIO_RSTN_PIN_NUM,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA,
	};

	struct ltc2983_init_param ltc2983_ip = {
		.spi_init = ltc2983_spi_ip,
		.gpio_rstn = ltc2983_gpio_rstn,
		.mux_delay_config_us = 1000,
		.filter_notch_freq = 0,
		.sensors[0] = NULL,
		.sensors[1] = &ltc2983_rsense_2.sensor,
		.sensors[2] = NULL,
		.sensors[3] = NULL,
		.sensors[4] = NULL,
		.sensors[5] = NULL,
		.sensors[6] = NULL,
		.sensors[7] = &ltc2983_thermistor_8.sensor,
		.sensors[8] = NULL,
		.sensors[9] = &ltc2983_thermistor_10.sensor,
		.sensors[10] = NULL,
		.sensors[11] = NULL,
		.sensors[12] = NULL,
		.sensors[13] = NULL,
		.sensors[14] = NULL,
		.sensors[15] = NULL,
		.sensors[16] = NULL,
		.sensors[17] = NULL,
		.sensors[18] = NULL,
		.sensors[19] = NULL,
		.dev_type = ID_LTC2983,
	};

	/**
	* Sense resistor connected to channels 1-2
	* Resistance: 2k ohms
	*/
	struct ltc2983_rsense ltc2983_rsense_2 = {
		.sensor = {
			.chan = 2,
			.type = LTC2983_RSENSE,
		},
		.r_sense_val = { // 10kohms
			.r_sense_val_int = 10000,
		}
	};

	/**
	* Thermistor simulator connected to channel 8
	* Sense Resistor: Channel 2
	*/
	struct ltc2983_thermistor ltc2983_thermistor_8 = {
		.sensor = {
			.chan = 8,
			.type = LTC2983_THERMISTOR_44008_44032,
		},
		.r_sense_chan = 2,
		.sensor_config = 0x2,
		.excitation_current = 0x3,
	};

	/**
	* Thermistor fixed simulator connected to channel 10
	* Sense Resistor: Channel 2
	* Expected: -30.59 C
	*/
	struct ltc2983_thermistor ltc2983_thermistor_10 = {
		.sensor = {
			.chan = 10,
			.type = LTC2983_THERMISTOR_44008_44032,
		},
		.r_sense_chan = 2,
		.sensor_config = 0x2,
		.excitation_current = 0x3,
	};

	ret = ltc2983_init(&ltc2983_dev, &ltc2983_ip);
	if (ret)
		goto error;

LTC2983 no-OS IIO support
-------------------------

The LTC2983 IIO driver comes on top of the LTC2983 driver and offers support
for interfacing IIO clients through libiio.

LTC2983 IIO Device Configuration
--------------------------------

Input Channel Attributes
-------------------------

LTC2983 IIO device has input channels which are either **input_tempX** or
**input_voltageX** depending on the sensor connected to **channel X**. The
input channel has a total of 2 channel attributes:

* ``raw - the raw value read from the device``
* ``scale - the scale that has to be applied to the raw value in order to obtain the converted real value in mC or mV``

LTC2983 IIO Driver Initialization Example
-----------------------------------------

.. code-block:: bash

	int ret;

	struct ltc2983_iio_dev *ltc2983_iio_dev;
	struct ltc2983_iio_dev_init_param ltc2983_iio_ip = {
		.ltc2983_init_param = &ltc2983_ip,
	};

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	ret = ltc2983_iio_init(&ltc2983_iio_dev, &ltc2983_iio_ip);
	if (ret)
		goto exit;

	struct iio_app_device iio_devices[] = {
		{
			.name = "ltc2983",
			.dev = ltc2983_iio_dev,
			.dev_descriptor = ltc2983_iio_dev->iio_dev,
		}
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = ltc2983_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto remove_iio_ltc2983;

	return iio_app_run(app);

ADT7604 Support
---------------

The ADT7604 is a 20-channel temperature and leak detection IC that extends the
LTC2983 with dedicated copper trace resistance measurement and liquid leak
detection capabilities. It uses the same SPI register interface and is
supported through the same driver by setting ``dev_type = ID_ADT7604`` in the
init parameters.

Thermocouple, diode, and direct ADC sensor types are not supported on the
ADT7604 and will be rejected during initialization.

ADT7604 Sensor Types
^^^^^^^^^^^^^^^^^^^^

Two sensor types are used for ADT7604-specific measurements:

* ``LTC2983_RTD_CUSTOM`` — Copper trace channel. Exists in two variants:

  * **Sub-ohm** (``sub_ohm = true``): for traces with resistance below 1Ω.
    Custom lookup tables and excitation current are not allowed; bits 17:0
    of the channel word must be zero.

  * **Above 1Ω** (``sub_ohm = false``): excitation current is required.
    A custom resistance-to-temperature lookup table
    (``struct ltc2983_custom_sensor``) is optional. When provided, the
    device also writes a converted result (e.g. temperature) to the
    temperature result bank, readable with **ltc2983_chan_read**.

  The primary output for both variants is raw resistance from the ADT7604
  resistance result bank (0x0060), readable with
  **ltc2983_chan_read_resistance**.

* ``LTC2983_THERMISTOR_CUSTOM`` — Leak detector channel. Measures the
  resistance of a leak detector element. A resistance-to-coverage lookup
  table (``struct ltc2983_custom_sensor``) is optional; when provided, a
  coverage percentage result is written to the temperature bank and
  readable with **ltc2983_chan_read** (output in milli-percent).

ADT7604 Resistance Measurement
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In addition to the temperature result bank (0x0010), the ADT7604 provides a
dedicated resistance result bank at register 0x0060. Use
**ltc2983_chan_read_resistance** to read from it:

* Copper trace (``LTC2983_RTD_CUSTOM``): result in 1/1024 mΩ per LSB
  (IIO scale: 1/1024000 Ω/LSB).
* Leak detector (``LTC2983_THERMISTOR_CUSTOM``): result in 1/1024 Ω per LSB
  (IIO scale: 1/1024 Ω/LSB).

The IIO scale factors are returned by **ltc2983_chan_read_scale_resistance**.

.. note::

   Applying the copper trace IIO scale (1/1024000 Ω/LSB) in integer
   arithmetic causes precision loss (result rounds to zero). Read the raw
   value with **ltc2983_chan_read_resistance** and scale in the application
   as needed.

ADT7604 Initialization Example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: bash

	struct ltc2983_desc *dev;

	/* 100Ω sense resistor for copper trace channels 4/6/8 (value in mΩ) */
	struct ltc2983_rsense ltc2983_rsense_2 = {
		.sensor = { .chan = 2, .type = LTC2983_RSENSE },
		.r_sense_val = { .r_sense_val_int = 100000 },
	};

	/* Sub-ohm copper trace channel — no custom table, no excitation */
	struct ltc2983_rtd ltc2983_copper_4 = {
		.sensor = { .chan = 4, .type = LTC2983_RTD_CUSTOM },
		.r_sense_chan = 2,
		.sub_ohm = true,
	};

	/* Leak detector with resistance-to-coverage lookup table */
	static uint32_t leak_table[20] = {
		       0, 382106,  /* P0:      0 Ohm -> 100% */
		    3232, 381082,  /* P1: 202.02 Ohm ->  99% */
		/* ... */
		16000000, 279706,  /* P9:   1 MOhm ->   0% */
	};
	struct ltc2983_custom_sensor leak_custom = {
		.table = leak_table, .len = 20, .is_steinhart = false,
	};
	struct ltc2983_thermistor ltc2983_leak_14 = {
		.sensor = { .chan = 14, .type = LTC2983_THERMISTOR_CUSTOM },
		.r_sense_chan = 12,
		.excitation_current = 3,
		.custom = &leak_custom,
	};

	struct ltc2983_init_param ltc2983_ip = {
		.spi_init = ltc2983_spi_ip,
		.gpio_rstn = ltc2983_gpio_rstn,
		.mux_delay_config_us = 1000,
		.filter_notch_freq = 0,
		.sensors[1]  = &ltc2983_rsense_2.sensor,
		.sensors[3]  = &ltc2983_copper_4.sensor,
		.sensors[13] = &ltc2983_leak_14.sensor,
		/* ... */
		.dev_type = ID_ADT7604,
	};

	ret = ltc2983_init(&dev, &ltc2983_ip);
	if (ret)
		goto error;

	/* Read copper trace raw resistance (raw / 1024 = mΩ) */
	uint32_t raw;
	ret = ltc2983_chan_read_resistance(dev, 4, &raw);

	/* Read leak detector coverage from temperature bank (milli-percent) */
	int coverage;
	ret = ltc2983_chan_read(dev, 14, &coverage);
