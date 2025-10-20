LTC2378-20 no-OS driver
=======================

.. no-os-doxygen::

Supported Devices
-----------------

`LTC2378-20 <https://www.analog.com/LTC2378-20>`_

Overview
--------

The LTC2378-20 is a high-speed, low-power, and low-noise 20-bit SAR
(Successive Approximation Register) analog-to-digital converter (ADC). It
operates from a 2.5V supply and supports both unipolar (0 to VREF) and
bipolar (±VREF) input ranges, with VREF values ranging from 2.5V to 5.1V.
Despite its high resolution, the device consumes only 21mW and delivers
excellent performance with a maximum ±2ppm INL and a signal-to-noise ratio
(SNR) of 104dB, ensuring no missing codes at 20-bit resolution.

The ADC features a high-speed SPI-compatible serial interface that supports
multiple logic levels —1.8V, 2.5V, 3.3V, and 5V—and includes a daisy-chain
mode for connecting multiple devices. Its fast 1Msps throughput and zero
cycle latency make it ideal for high-speed applications. An internal
oscillator simplifies timing by setting the conversion time automatically,
and the device powers down between conversions to reduce power consumption,
which scales with the sampling rate.

The LTC2378-20 supports both unipolar and bipolar input modes. In unipolar
mode, the input range is from 0V to VREF with 20-bit resolution (2^20 codes).
In bipolar mode, the input range is from -VREF to +VREF with effective
19-bit resolution (2^19 codes for each polarity). The device uses a BUSY
signal to indicate conversion completion and a CNV signal to initiate
conversions.

Applications
------------

* Medical Imaging
* High Speed Data Acquisition
* Portable or Compact Instrumentation
* Industrial Process Control
* Low Power Battery-Operated Instrumentation
* Automated Test Equipment (ATE)

LTC2378-20 Device Configuration
-------------------------------

Driver Initialization
---------------------

In order to be able to use the device, you will have to provide the support
for the communication protocol (SPI) alongside GPIO pins for conversion
control (CNV) and busy indication (BUSY).

The first API to be called is **ltc2378_init**. Make sure that it returns 0,
which means that the driver was initialized correctly.

The initialization API uses the device descriptor and an initialization
parameter. The initialization parameter contains the SPI configuration,
GPIO parameters for CNV and BUSY pins, reference voltage (VREF) in microvolts,
and input mode (unipolar or bipolar). These are defined in the header file
of the driver.

ADC Configuration
-----------------

The LTC2378-20 can be configured for either unipolar or bipolar input modes
using the **input_mode** parameter during initialization.

In **unipolar mode** (LTC2378_UNIPOLAR):
- Input range: 0V to VREF
- Resolution: 20 bits (2^20 = 1,048,576 codes)
- Code 0 represents 0V, code 1048575 represents VREF

In **bipolar mode** (LTC2378_BIPOLAR):
- Input range: -VREF to +VREF
- Resolution: 19 bits effective (2^19 = 524,288 codes per polarity)
- Two's complement format with proper sign extension

ADC Data Acquisition
--------------------

Data acquisition is performed using the **ltc2378_read_raw** API, which:

1. Initiates a conversion using the CNV signal
2. Waits for the BUSY signal to indicate conversion completion
3. Reads the 20-bit result via SPI
4. Returns the raw digital code

The **ltc2378_raw_to_uv** API converts the raw digital code to voltage
in microvolts, taking into account the configured input mode and reference
voltage.

Power Management
----------------

The LTC2378-20 automatically powers down between conversions to minimize
power consumption. The **ltc2378_power_down** API can be used to manually
put the device into power-down mode by holding the CNV signal low.

LTC2378-20 Driver Initialization Example
----------------------------------------

.. code-block:: bash

	struct ltc2378_dev *ltc2378_dev;
	struct no_os_spi_init_param ltc2378_spi_ip = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = 1000000,
		.chip_select = SPI_CS,
		.mode = NO_OS_SPI_MODE_0,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.platform_ops = SPI_OPS,
		.extra = SPI_EXTRA,
	};

	struct no_os_gpio_init_param ltc2378_gpio_cnv = {
		.port = GPIO_CNV_PORT_NUM,
		.number = GPIO_CNV_PIN_NUM,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA
	};

	struct no_os_gpio_init_param ltc2378_gpio_busy = {
		.port = GPIO_BUSY_PORT_NUM,
		.number = GPIO_BUSY_PIN_NUM,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA
	};

	struct ltc2378_init_param ltc2378_ip = {
		.spi_init = &ltc2378_spi_ip,
		.gpio_cnv_init = &ltc2378_gpio_cnv,
		.gpio_busy_init = &ltc2378_gpio_busy,
		.vref_uv = 2500000,  // 2.5V reference
		.input_mode = LTC2378_BIPOLAR
	};

	ret = ltc2378_init(&ltc2378_dev, &ltc2378_ip);
	if (ret)
		goto error;

	/* Read ADC value */
	uint32_t raw_data;
	int32_t voltage_uv;

	ret = ltc2378_read_raw(ltc2378_dev, &raw_data);
	if (ret)
		goto error;

	ret = ltc2378_raw_to_uv(ltc2378_dev, raw_data, &voltage_uv);
	if (ret)
		goto error;

LTC2378-20 no-OS IIO support
----------------------------

The LTC2378-20 IIO driver comes on top of the LTC2378-20 driver and offers support
for interfacing IIO clients through libiio.

LTC2378-20 IIO Device Configuration
-----------------------------------

Voltage Channel Attributes
--------------------------

The LTC2378-20 IIO device provides a single voltage input channel with the
following attributes:

* ``raw`` - The raw digital code from the ADC (0 to 1048575 for unipolar, -524288 to 524287 for bipolar)
* ``scale`` - The scale factor to convert raw values to microvolts
* ``processed`` - The processed voltage value in microvolts (raw * scale)

The scale factor is automatically calculated based on the configured reference
voltage and input mode:

- **Unipolar mode**: scale = VREF / 2^20
- **Bipolar mode**: scale = VREF / 2^19

For example, with VREF = 2.5V:
- Unipolar scale = 2500000 µV / 1048576 ≈ 2.384 µV/LSB
- Bipolar scale = 2500000 µV / 524288 ≈ 4.768 µV/LSB

LTC2378-20 IIO Driver Initialization Example
--------------------------------------------

.. code-block:: bash

	int ret;

	struct ltc2378_iio_desc *ltc2378_iio_desc;
	struct ltc2378_iio_desc_init_param ltc2378_iio_ip = {
		.ltc2378_init_param = &ltc2378_ip,
	};

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	ret = ltc2378_iio_init(&ltc2378_iio_desc, &ltc2378_iio_ip);
	if (ret)
		goto exit;

	struct iio_app_device iio_devices[] = {
		{
			.name = "ltc2378-20",
			.dev = ltc2378_iio_desc,
			.dev_descriptor = ltc2378_iio_desc->iio_dev,
		},
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = uip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto remove_iio_ltc2378;

	ret = iio_app_run(app);

	iio_app_remove(app);

	remove_iio_ltc2378:
	ltc2378_iio_remove(ltc2378_iio_desc);
	exit:
	if (ret)
		pr_info("Error!\n");
	return ret;
