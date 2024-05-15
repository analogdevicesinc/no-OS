AD405X no-OS Driver
===================

Supported Devices
-----------------

`AD4052 <https://www.analog.com/AD4052>`_

Overview
--------

The AD4052 is a low power, compact 16-bit successive approximation register (SAR) analog-to-digital converter (ADC) designed
for battery powered precision measurement and monitoring applications. The AD4052 feature set supports event-driven programming
for dynamic tradeoff between system power and precision. Using
a patented, power efficient window comparator, the AD4052 autonomously monitors signals while the host sleeps. The programmable
averaging filter enables on-demand high resolution measurements
for optimizing precision for the power consumed.

The AD4052 includes AFE control signals to minimize the complexity of host timers. The control signals automate the power cycling
of the AFE relative to ADC sampling to reduce system power while
minimizing settling error artifacts. The Easy Drive analog inputs
enables compact and low power signal conditioning circuitry by
reducing the dependence on high-speed ADC driver amplifiers. The
small 3.4 pF sampling capacitors result in low dynamic and average
input current, broadening compatibility with low power amplifiers or
direct sensor interfacing. The AD4052 wide common mode input
range supports both differential and single-ended input signals.

The AD4052 features a 4-wire SPI with a dedicated CNV input.
Cyclic redundancy check (CRC) is available on all interface read
and write operations and internal memory to ensure reliable device
configuration and operation.

AD405X Device Configuration
---------------------------

Driver Initialization
---------------------

In order to be able to use the device, you will have to provide the support for
the communication protocol (SPI) as well as 3 external GPIOs for the CNV pin and two 
general-purpose input/output pins (GP0 and GP1).

The first API to be called is **ad405x_init**. Make sure that it returns 0,
which means that the driver was intialized correctly.

GPIO Configuration
-----------------------------

The device has two general purpose input/output pins GPIO controller. 
General Purpose Output 1, GP1, is a digital output hat can be configured as multiple device status signals.
General Purpose Input/Output 0. GP0 is a digital input/output that can be configured as multiple device
control or status signals.
In the driver files the **ad405x_set_gp_mode** can be found and used to choose the specific signal for the GPIOs.

Channel Configuration
---------------------

Channel data can be fetched with **ad405x_read_val**.

The channel data format can be set using **ad405x_set_data_format**

Channel operation mode can also be configured using **ad405x_set_operating_mode**.

Soft Reset
----------

The device can be soft reseted by using **ad405x_soft_reset**.

MAX2201X Driver Initialization Example
--------------------------------------

.. code-block:: bash

	struct ad405x_dev *ad405x;
	struct no_os_spi_init_param spi_init_params = {
		.device_id = 0,
		.max_speed_hz = 22500000,
		.mode = NO_OS_SPI_MODE_0,
		.chip_select = 15,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.platform_ops = &spi_ops,
		.extra = &spi_extra_init_params,
	};
	struct no_os_gpio_init_param gpio_cnv_param = {
		.port = 0,
		.number = 10,
		.platform_ops = &gpio_ops,
		.extra = &cnv_extra_init_params
	};
	struct no_os_gpio_init_param gpio_gpio0_param = {
		.port = 1,
		.number = 15,
		.platform_ops = &gpio_ops,
		.extra = &gp0_extra_init_params
	};
	struct no_os_gpio_init_param gpio_gpio1_param = {
		.port = 6,
		.number = 11,
		.platform_ops = &gpio_ops,
		.extra = &gp1_extra_init_params
	};
	struct ad405x_init_param ad405x_init_params = {
		.spi_init = &spi_init_params,
		.active_device = 0,
		.gpio_cnv = &gpio_cnv_param,
		.gpio_gpio0 = &gpio_gpio0_param,
		.gpio_gpio1 = &gpio_gpio1_param,
		.rate = AD405X_2_MSPS,
		.filter_length = AD405X_LENGTH_2,
		.operation_mode = AD405X_CONFIG_MODE_OP
	};
	ret = ad405x_init(&ad405x, &ad405x_init_params);
	if (ret)
		goto error;
