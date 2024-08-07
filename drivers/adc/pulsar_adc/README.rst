AD400X no-OS driver
====================

Supported Devices
-----------------

`ad4000 <https://www.analog.com/en/products/ad4000.html>`_
`ad4001 <https://www.analog.com/en/products/ad4001.html>`_
`ad4002 <https://www.analog.com/en/products/ad4002.html>`_
`ad4003 <https://www.analog.com/en/products/ad4003.html>`_
`ad4020 <https://www.analog.com/en/products/ad4020.html>`_

`ad4004 <https://www.analog.com/en/products/ad4004.html>`_
`ad4005 <https://www.analog.com/en/products/ad4005.html>`_
`ad4006 <https://www.analog.com/en/products/ad4006.html>`_
`ad4007 <https://www.analog.com/en/products/ad4007.html>`_
`ad4011 <https://www.analog.com/en/products/ad4011.html>`_

`adaq4003 <https://www.analog.com/en/products/adaq4003.html>`_


Overview
--------

The AD4000/AD4001/AD4002/AD4003/AD4020 family of evaluation boards enable quick,
simplified evaluation of the AD4000 family of 16-/18-/20-bit, precision
successive approx-imation register (SAR) analog-to-digital converters (ADCs).
The AD4000/AD4001/AD4002/AD4003/AD4020 are low power, 16-bit/18-bit/20-bit,
precision SAR ADCs that offer high performance with throughputs up to
2 MSPS (1.8 MSPS for the AD4020). The evaluation board demonstrates the
performance of the AD4000/AD4001/AD4002/AD4003/AD4020 family of ADCs and
provides a simplified interface for a variety of system applications.


AD400X Device Configuration
----------------------------

Driver Initialization
^^^^^^^^^^^^^^^^^^^^^^

**ad400x_init** is called with the settings dma, pwm, clkgen, and the 4 gpios to
set the operation mode.

**ad400x_remove** Free the allocated resources

**ad400x_spi_reg_write** called to write a register

**ad400x_spi_reg_read** called to read a register

**ad400x_read_data** called to fill a buffer with n number of samples.

AD400X Driver Initialization Example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: bash

	struct no_os_spi_init_param ad400x_spi_init = {
		.device_id = SPI_DEVICE_ID,
		.chip_select = SPI_CS,
		.max_speed_hz = SPI_BAUDRATE,
		.mode = NO_OS_SPI_MODE_0,
		.platform_ops = SPI_OPS,
		.extra = (void*)SPI_EXTRA,
	};

	struct spi_engine_offload_init_param spi_engine_offload_init_param = {
		.offload_config = OFFLOAD_RX_EN,
		.rx_dma_baseaddr = DMA_BASEADDR,
	};

	struct axi_clkgen_init clkgen_init = {
		.name = "rx_clkgen",
		.base = RX_CLKGEN_BASEADDR,
		.parent_rate = 100000000,
	};

	struct no_os_pwm_init_param trigger_pwm_init = {
		.period_ns = PWM_PERIOD,
		.duty_cycle_ns = PWM_DUTY,
		.polarity = NO_OS_PWM_POLARITY_HIGH,
		.platform_ops = PWM_OPS,
		.extra = PWM_EXTRA,
	};

	struct no_os_gpio_init_param gpio_cnv = {
		.port = GPIO_CNV_PORT,
		.pull = NO_OS_PULL_NONE,
		.number = GPIO_CNV,
		.platform_ops = GPIO_OPS,
	};

	struct ad400x_init_param ad400x_init_param = {
		.dev_id = ID_AD4020,
		.spi_init = &ad400x_spi_init,
		.trigger_pwm_init = &trigger_pwm_init,
		.clkgen_init = &clkgen_init,
		.axi_clkgen_rate = REFCLK_RATE,
		.reg_access_speed = 1000000,
		.offload_init_param = &spi_engine_offload_init_param,
		.dcache_invalidate_range = DCACHE_INVALIDATE,
		.gpio_cnv = &gpio_cnv,
		.offload_enable = 1,
		.turbo_mode = 1,
		.high_z_mode = 0,
		.span_compression = 0,
		.en_status_bits = 0,
	}

	struct ad400x_dev *dev;
	uint32_t data[1000];
	int32_t ret, i;
	uint16_t resolution;
	char sign;

	ret = ad400x_init(&dev, &ad400x_init_param);
	if (ret)
		return ret;

	sign = dev->dev_info->sign;
	resolution = dev->dev_info->resolution;

	ret = ad400x_read_data(dev, data, 1000);
	if (ret) {
		pr_info("Error: ad400x_read_data: %ld\n", ret);
		ad400x_remove(dev);
		return ret;
	}

	for(i = 0; i < 1000; i++) {
		if (sign == 's')
			printf("ADC: %ld\n\r", no_os_sign_extend32(data[i], resolution - 1));
		else
			printf("ADC: %ld\n\r", data[i]);
	}

AD400X no_OS IIO Support
-------------------------

The AD400X uses the iio_ad400x driver for iio support.

ad400x specific IIO Initialization
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**ad400x_iio_init** is called with a ad400x_iio_init_param structure that
contains the ad400x_init_param plus iio specific parameters

**ad400x_iio_remove** Free the allocated resources



AD400X IIO Driver Initialization Example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: bash

	struct no_os_spi_init_param ad400x_spi_init = {
		.device_id = SPI_DEVICE_ID,
		.chip_select = SPI_CS,
		.max_speed_hz = SPI_BAUDRATE,
		.mode = NO_OS_SPI_MODE_0,
		.platform_ops = SPI_OPS,
		.extra = (void*)SPI_EXTRA,
	};

	struct spi_engine_offload_init_param spi_engine_offload_init_param = {
		.offload_config = OFFLOAD_RX_EN,
		.rx_dma_baseaddr = DMA_BASEADDR,
	};

	struct axi_clkgen_init clkgen_init = {
		.name = "rx_clkgen",
		.base = RX_CLKGEN_BASEADDR,
		.parent_rate = 100000000,
	};

	struct no_os_pwm_init_param trigger_pwm_init = {
		.period_ns = PWM_PERIOD,
		.duty_cycle_ns = PWM_DUTY,
		.polarity = NO_OS_PWM_POLARITY_HIGH,
		.platform_ops = PWM_OPS,
		.extra = PWM_EXTRA,
	};

	struct no_os_gpio_init_param gpio_cnv = {
		.port = GPIO_CNV_PORT,
		.pull = NO_OS_PULL_NONE,
		.number = GPIO_CNV,
		.platform_ops = GPIO_OPS,
	};

	struct ad400x_init_param ad400x_init_param = {
		.dev_id = ID_AD4020,
		.spi_init = &ad400x_spi_init,
		.trigger_pwm_init = &trigger_pwm_init,
		.clkgen_init = &clkgen_init,
		.axi_clkgen_rate = REFCLK_RATE,
		.reg_access_speed = 1000000,
		.offload_init_param = &spi_engine_offload_init_param,
		.dcache_invalidate_range = DCACHE_INVALIDATE,
		.gpio_cnv = &gpio_cnv,
		.offload_enable = 1,
		.turbo_mode = 1,
		.high_z_mode = 0,
		.span_compression = 0,
		.en_status_bits = 0,
	}

	struct iio_data_buffer adc_buff = {
		.buff = (void *)ADC_DDR_BASEADDR,
		.size = MAX_SIZE_BASE_ADDR,
	};

	struct ad400x_iio_init_param ad400x_iio_ip = {
		.init_param = &ad400x_init_param,
		.ref_voltage_mv = AD400X_ADC_REF_VOLTAGE,
	};

	struct ad400x_iio_dev *dev;
	struct iio_app_init_param app_init_param = {0};
	struct iio_app_desc *app;
	int ret;

	ret = ad400x_iio_init(&dev, &ad400x_iio_ip);
	if (ret)
		return ret;

	struct iio_app_device iio_devices[] = {
		IIO_APP_DEVICE( "ad400x", dev,
				dev->iio_dev, &adc_buff, NULL, NULL)
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = ad400x_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret) {
		pr_info("Error: iio_app_init: %d\n", ret);
		ad400x_iio_remove(dev);
		return ret;
	}

	ret = iio_app_run(app);
	if (ret)
		pr_info("Error: iio_app_run: %d\n", ret);

	iio_app_remove(app);

	ad400x_iio_remove(dev);

	return ret;
