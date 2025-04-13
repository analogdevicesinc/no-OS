AD8460 no-OS driver
=====================

.. no-os-doxygen::

Supported Devices
-----------------

`AD8460 <https://www.analog.com/AD8460>`_

Overview
--------

The AD8460 is a “bits in, power out” high voltage, high-power, high-speed driver
optimized for large output current (up to ±1 A) and high slew rate (up to ±1800
V/μs) at high voltage (up to ±40 V) into capacitive loads. Combining a 14-bit
high-speed DAC, a high voltage, high output current (HV-HI) analog driver, and
fault monitoring and protection circuits, the AD8460 is ideally suited for high
power applications such as arbitrary waveform generation (AWG), programmable
power supplies, and high voltage automated test equipment (ATE).

A proprietary high-voltage BCDMOS process, novel high voltage architecture, and
thermally enhanced package from Analog Devices Inc. enable this high-performance
driver. A digital engine implements user-configurable features: modes for
digital input, programmable supply current, and fault monitoring and
programmable protection settings for output current, output voltage, and
junction temperature. Analog features extend functionality: external
compensation enables unlimited capacitive load drive, programmable shutdown
delay, and full-scale adjustment. The AD8460 operates on high voltage dual
supplies up to ±55 V and a single low voltage supply of 5 V.

Applications
------------

AD8460
--------

* Automatic test equipment (ATE)
* Display panel formation and testing
* Piezo drivers
* Programmable power supplies

AD8460 Device Configuration
-----------------------------

Driver Initialization
---------------------

In order to be able to use the device, you will have to provide the support
for the communication protocol (SPI).

The first API to be called is **ad8460_init**. Make sure that it returns 0,
which means that the driver was initialized correctly.

Reseting AD8460
---------------

Manual POR can be achieved by pulling RESETB LOW and then HIGH, which resets all
the digital registers to default. Also, reset to defaults can also be commanded
at any time through the SOFT_RESET register bit. These are done using the API
**ad8460_reset**.

Reading and Writing HVDAC Values
--------------------------------

Reading and writing HVDAC values is done using **ad8460_get_hvdac_word** and
**ad8460_set_hvdac_word** APIs, respectively.

Switching between APG and AWG Modes
-----------------------------------

The AD8460 can be switched between APG and AWG modes using
**ad8460_enable_apg_mode** API. If 1 is passed as a parameter, the device will
be switched to APG mode; otherwise, it will be switched to AWG mode.

Shutdown Flag
-------------

The shutdown flag can be read using **ad8460_read_shutdown_flag** API. If the
value read is 1, the device is in shutdown mode; otherwise, it is not.

Resetting HV Driver
-------------------

The HV driver can be reset using **ad8460_hv_reset** API.

Setting a Sample
----------------

A single sample can be set in APG mode using **ad8460_set_sample** API.

AD8460 Driver Initialization Example
--------------------------------------

.. code-block:: bash

	struct ad8460_device *dev;

	struct no_os_uart_init_param uip = {
		.device_id = UART_DEVICE_ID,
		.baud_rate = UART_BAUDRATE,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.platform_ops = UART_OPS,
		.extra = UART_EXTRA,
	};

	const struct no_os_spi_init_param ad8460_spi_ip = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = SPI_MAX_SPEED,
		.chip_select = SPI_CS,
		.mode = NO_OS_SPI_MODE_0,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.platform_ops = SPI_OPS,
		.extra = SPI_EXTRA,
		.parent = NULL,
	};

	const struct no_os_gpio_init_param ad8460_gpio_rstn = {
		.port = GPIO_RSTN_PORT_NUM,
		.number = GPIO_RSTN_PIN_NUM,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA,
	};

	struct ad8460_init_param ad8460_ip = {
		.spi_init_param = ad8460_spi_ip,
		.gpio_rstn = ad8460_gpio_rstn,
		.refio_1p2v_mv = 1200,
		.ext_resistor_ohms = 2000,
	};

	ret = ad8460_init(&dev, &ad8460_ip);
	if (ret)
		goto error;

AD8460 no-OS IIO support
--------------------------

The AD8460 IIO driver comes on top of the AD8460 driver and offers support
for interfacing IIO clients through libiio.

AD8460 IIO Device Configuration
---------------------------------

Channel Attributes
------------------

AD8460 has a total of 22 channel attributes:

* ``out_voltage_raw - raw single HVDAC data``
* ``out_voltage_scale - scale that has to be applied to the raw value in order to obtain the voltage value in mV``
* ``out_voltage_raw0 - raw HVDAC data word 0``
* ``out_voltage_raw1 - raw HVDAC data word 1``
* ``out_voltage_raw2 - raw HVDAC data word 2``
* ``out_voltage_raw3 - raw HVDAC data word 3``
* ``out_voltage_raw4 - raw HVDAC data word 4``
* ``out_voltage_raw5 - raw HVDAC data word 5``
* ``out_voltage_raw6 - raw HVDAC data word 6``
* ``out_voltage_raw7 - raw HVDAC data word 7``
* ``out_voltage_raw8 - raw HVDAC data word 8``
* ``out_voltage_raw9 - raw HVDAC data word 9``
* ``out_voltage_raw10 - raw HVDAC data word 10``
* ``out_voltage_raw11 - raw HVDAC data word 11``
* ``out_voltage_raw12 - raw HVDAC data word 12``
* ``out_voltage_raw13 - raw HVDAC data word 13``
* ``out_voltage_raw14 - raw HVDAC data word 14``
* ``out_voltage_raw15 - raw HVDAC data word 15``
* ``out_voltage_toggle_en - enable or disable APG mode``
* ``out_voltage_symbol - how many symbols of HVDAC data to be used``
* ``out_voltage_powerdown - power down the HV driver``
* ``out_current_raw - raw quiescent current value of HV driver``

AD8460 IIO Driver Initialization Example
------------------------------------------

.. code-block:: bash

	int ret;

	struct ad8460_iio_device *ad8460_iio_dev;
	struct ad8460_iio_init_param ad8460_iio_ip = {
		.ad8460_init_param = &ad8460_ip,
	};

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = {0};

	ret = ad8460_iio_init(&ad8460_iio_dev, &ad8460_iio_ip);
	if (ret)
		goto exit;

	struct iio_app_device iio_devices[] = {
		{
			.name = "ad8460",
			.dev = ad8460_iio_dev,
			.dev_descriptor = ad8460_iio_dev->iio_dev,
		},
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = uip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto remove_iio_ad8460;

	return iio_app_run(app);
