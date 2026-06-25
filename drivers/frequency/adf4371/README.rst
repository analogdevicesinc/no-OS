ADF4371 no-OS driver
====================

.. no-os-doxygen::

Supported Devices
-----------------

* `ADF4371 <https://www.analog.com/en/products/adf4371.html>`_
* `ADF4372 <https://www.analog.com/en/products/adf4372.html>`_

Overview
--------

The `ADF4371 <https://www.analog.com/en/products/adf4371.html>`_ and
`ADF4372 <https://www.analog.com/en/products/adf4372.html>`_ are wideband microwave
fractional-N and integer-N PLL frequency synthesizers that operate with an external
reference and loop filter. Both devices integrate a VCO with a fundamental frequency
range of 4 GHz to 8 GHz and include programmable dividers (÷1 to ÷64), enabling RF
output frequencies as low as 62.5 MHz. Control of all on-chip registers is achieved
via a 3-wire serial interface, and both devices support hardware and software
power-down modes. They operate from 3.15 V to 3.45 V analog and digital supplies,
with a dedicated 5 V supply for the VCO.

The `ADF4371 <https://www.analog.com/en/products/adf4371.html>`_ extends the available
output frequency range up to 32 GHz through integrated frequency multiplication
stages. In addition to the divider outputs (RF8x), it includes RF16x and RF32x
outputs generated via an internal frequency multiplier and quadrupler, respectively,
with harmonic filtering to suppress unwanted products. The
`ADF4372 <https://www.analog.com/en/products/adf4372.html>`_ supports output
frequencies up to 16 GHz, providing RF8x divider outputs and an RF16x multiplied
output, also with harmonic filtering. Both devices include an auxiliary output
(RFAUX8x) that can mirror the RF8x output or provide direct access to the VCO,
offering added flexibility in system design.

Applications
------------

* High Performance Data Converter Clocking
* Wireless infrastructure (MC-GSM, 5G, 6G)
* Test and Measurement

ADF4371 Device Configuration
-----------------------------

Driver Initialization
---------------------

In order to be able to use the device, you will have to provide the support for
the communication protocol (SPI).

The first API to be called is **adf4371_init**. Make sure that it returns 0,
which means that the driver was initialized correctly. This function will also
reset the ADF4371 by performing a soft reset, putting the device in a known
state.

Reference Clock Configuration
------------------------------

The device requires an external reference clock between 10 MHz and 600 MHz. This
can be configured using the function **adf4371_set_ref_clk**. Values exceeding
the supported range will be clamped to the maximum (600 MHz) or minimum (10 MHz)
supported value. The current configuration can be read out using
**adf4371_get_ref_clk**. The input reference clock value for the function will
have to be expressed in Hz.

Reference Doubler Configuration
--------------------------------

The ADF4371 has a reference doubler on the input path for the reference
frequency. By default it is disabled and can be enabled using
**adf4371_set_ref_doubler**. The current configuration can be read out using
**adf4371_get_ref_doubler**.

This feature can only be enabled or disabled.

Together with the reference divider and RDIV2, this feature will determine the
PFD frequency.

Reference Divide-by-2 Configuration
-------------------------------------

The ADF4371 has a reference divide-by-2 on the input path for the reference
frequency. By default it is disabled and can be enabled using
**adf4371_set_rdiv2_enable**. The current configuration can be read out using
**adf4371_get_rdiv2_enable**.

This feature can only be enabled or disabled.

Together with the reference doubler and reference divider, this feature will
determine the PFD frequency.

Reference Divider Configuration
--------------------------------

On the reference input path there is a divider (R Word), allowing the input
reference frequency to be divided by up to 31. Default this is set to 1 and can
be changed using **adf4371_set_r_word**. The current configuration can be read
out using **adf4371_get_r_word**.

Together with the reference doubler and RDIV2, this feature will determine the
PFD frequency.

Charge Pump Current Configuration
-----------------------------------

The ADF4371 has the option for 16 preset charge pump currents, ranging from 0.35
mA to 5.6 mA. The supported currents can be configured using
**adf4371_set_charge_pump_current** API, which will expect the register value,
ranging from 0 to 15, as input. The current configuration can be read out using
**adf4371_get_charge_pump_current**, which will also return the value in the
same range.

In order to determine which value corresponds to your design charge pump
current, please refer to the datasheet
`ADF4371 <https://www.analog.com/en/products/adf4371.html>`_ in the register details
section for register REG001E.

Bleed Current Configuration
----------------------------

The charge pump current can be optimized using the bleed current feature, with
a maximum value of 255. This can be configured using the
**adf4371_set_bleed_icp** API, by providing the bleed word as a parameter on
8 bits. The current configuration can be read out using
**adf4371_get_bleed_icp**.

Output Frequency Configuration
-------------------------------

The ADF4371 has four output channels (RF8x, RFAUX8x, RF16x, RF32x). The RF8x
and RFAUX8x channels support 62.5 MHz to 8 GHz, RF16x supports 8 GHz to 16 GHz,
and RF32x supports 16 GHz to 32 GHz. The RF8x output frequency can be set using
the **adf4371_set_rf8_freq** API by providing the desired frequency in Hz. The
RF16x and RF32x frequencies are derived from the VCO frequency and are not
independently configurable. The output frequency of each channel can be read out
using **adf4371_get_rfout**, **adf4371_get_rfout8aux**, **adf4371_get_rfout16**,
and **adf4371_get_rfout32** respectively.

Channel Power Down Configuration
----------------------------------

Each channel can be powered down individually using
**adf4371_set_channel_power_down** API. The API expects the channel number and a
boolean value for power down or power up. To determine if a channel is currently
powered down the **adf4371_get_channel_power_down** API can be used.

ADF4371 Frequency Generation
-----------------------------

Frequency Setting
-----------------

The output frequency depends on all of the previous configurations and is
recalculated based on them using the **adf4371_set_frequency** API. When the API
is called, it will calculate the VCO frequency, the output divider, the integer
and fractional parts of the feedback loop for the PLL while trying to obtain the
configured output frequency. The API will also write the corresponding registers
with the computed values.

Because of the link between the calculated values and the configurations, when
an attribute is reconfigured using the corresponding API (reference clock,
reference doubler, reference divider, RDIV2) it will automatically trigger a
frequency recalculation via **adf4371_set_frequency**.

ADF4371 Driver Initialization Example
--------------------------------------

SPI Communication Example
-------------------------

.. code-block:: bash

	struct adf4371_dev *dev;
	int ret;

	struct no_os_uart_init_param adf4371_uart_ip = {
		.device_id = UART_DEVICE_ID,
		.irq_id = UART_IRQ_ID,
		.asynchronous_rx = true,
		.baud_rate = UART_BAUDRATE,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.extra = UART_EXTRA,
		.platform_ops = UART_OPS,
	};

	struct no_os_spi_init_param adf4371_spi_ip = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = 1000000,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.mode = NO_OS_SPI_MODE_0,
		.platform_ops = SPI_OPS,
		.chip_select = SPI_CS,
		.extra = SPI_EXTRA,
	};

	struct adf4371_init_param adf4371_ip = {
		.spi_init = &adf4371_spi_ip,
		.spi_3wire_enable = false,
		.clkin_frequency = 125000000,
		.rfout8_freq = 8000000000,
		.rf_div_sel = 0,
		.charge_pump_microamp = 1750,
		.muxout_select = ADF4371_DIG_LOCK,
	};

	ret = adf4371_init(&dev, &adf4371_ip);
	if (ret)
		goto error;

	ret = adf4371_set_rf8_freq(dev, 5000000000);
	if (ret)
		goto error;

ADF4371 no-OS IIO support
--------------------------

The ADF4371 IIO driver comes on top of ADF4371 driver and offers support for
interfacing IIO clients through IIO lib.

ADF4371 IIO Device Configuration
---------------------------------

Device Attributes
-----------------

The device attributes provide control over the global device configuration.

The attributes are:

* ref_clk - is the current set input reference frequency.
* bleed_current - is the adjustment value for the set charge pump current.
* charge_pump_current - is the current set in your design.
* charge_pump_available_currents - lists the available and predefined charge
                                   pump currents of the ADF4371.
* r_word - is the current value of the input reference divider.
* ref_divider2_enable - enables the reference divide-by-2.
* ref_doubler_enable - enables the input reference doubler.

Device Channels
---------------

ADF4371 IIO device has 4 output channels corresponding to the four RF outputs.

The channels are:

* output altvoltage0 - corresponding to RF8x on the device
* output altvoltage1 - corresponding to RFAUX8x on the device
* output altvoltage2 - corresponding to RF16x on the device
* output altvoltage3 - corresponding to RF32x on the device

Each channel has the following attributes:

* powerdown - powers down the channel.
* frequency - is the output frequency of the channel. Only the RF8x channel
              (altvoltage0) supports writing the frequency. The RF16x and RF32x
              frequencies are derived from the VCO and are read-only.

ADF4371 IIO Driver Initialization Example
------------------------------------------

.. code-block:: bash

	struct adf4371_iio_dev *adf4371_iio_dev;
	struct adf4371_iio_dev_init_param adf4371_iio_ip;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };
	int ret;

	struct no_os_uart_init_param adf4371_uart_ip = {
		.device_id = UART_DEVICE_ID,
		.irq_id = UART_IRQ_ID,
		.asynchronous_rx = true,
		.baud_rate = UART_BAUDRATE,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.extra = UART_EXTRA,
		.platform_ops = UART_OPS,
	};

	struct no_os_spi_init_param adf4371_spi_ip = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = 1000000,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.mode = NO_OS_SPI_MODE_0,
		.platform_ops = SPI_OPS,
		.chip_select = SPI_CS,
		.extra = SPI_EXTRA,
	};

	struct adf4371_init_param adf4371_ip = {
		.spi_init = &adf4371_spi_ip,
		.spi_3wire_enable = false,
		.clkin_frequency = 125000000,
		.rfout8_freq = 8000000000,
		.rf_div_sel = 0,
		.charge_pump_microamp = 1750,
		.muxout_select = ADF4371_DIG_LOCK,
	};

	adf4371_iio_ip.adf4371_dev_init = &adf4371_ip;
	ret = adf4371_iio_init(&adf4371_iio_dev, &adf4371_iio_ip);
	if (ret)
		return ret;

	struct iio_app_device iio_devices[] = {
		{
			.name = "adf4371",
			.dev = adf4371_iio_dev,
			.dev_descriptor = adf4371_iio_dev->iio_dev,
		}
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = adf4371_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		return ret;

	return iio_app_run(app);