AD796X no-OS driver
====================

Supported Devices
-----------------

`AD7960 <https://www.analog.com/en/products/ad7960.html>`_

Overview
--------

The EVAL-AD7960FMCZ is a fully featured evaluation Kit designed to demonstrate
the performance of the low power 18-bit,5 MSPS PulSAR® Differential ADC AD7960.

This board operates in conjunction with the Zedboard. The evaluation software
is provided to enable the user to perform detailed analysis of the AD7960's
performance. The technical user guide includes the detailed description of
the operation and set up of the evaluation board and software when operated
with the System Demonstration Platform board.


AD796X Device Configuration
----------------------------

Driver Initialization
^^^^^^^^^^^^^^^^^^^^^^

In order to be able to use the device, the pulsar lvds HDL is used. The usage
of the driver is:

**ad796x_init** is called with the settings dma, pwm, clkgen, and the 4 gpios to
set the operation mode.

**ad796x_read_data** called to fill a buffer with n number of samples.

**ad796x_remove** Free the allocated resources

AD796X Driver Initialization Example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: bash

	uint32_t *buf = (uintptr_t)ADC_DDR_BASEADDR;
	int i, ret;
	struct ad796x_dev *adc_dev;

	struct axi_adc_init ad796x_core_ip = {
		.name = "ad796x_core",
		.num_channels = 1,
		.base = RX_CORE_BASEADDR
	};

	struct axi_clkgen_init clkgen_ip = {
		.name = "rx_clkgen",
		.base = RX_CLKGEN_BASEADDR,
		.parent_rate = 100000000,
	};

	struct axi_dmac_init dmac_ip = {
		.name = "ad796x_dmac",
		.base = RX_DMA_BASEADDR,
		.irq_option = IRQ_DISABLED
	};

	struct axi_pwm_init_param axi_pwm_0_extra = {
		.base_addr = AXI_PWMGEN_BASEADDR,
		.ref_clock_Hz = 125000000,
		.channel = 0
	};

	struct no_os_pwm_init_param axi_pwm_0_ip = {
		.period_ns = PWM_0_PERIOD_NS,
		.duty_cycle_ns = PWM_0_DUTY_NS,
		.phase_ns = PWM_0_PHASE,
		.platform_ops = &axi_pwm_ops,
		.extra = &axi_pwm_0_extra
	};

	struct axi_pwm_init_param axi_pwm_1_extra = {
		.base_addr = AXI_PWMGEN_BASEADDR,
		.ref_clock_Hz = 125000000,
		.channel = 1
	};

	struct no_os_pwm_init_param axi_pwm_1_ip = {
		.period_ns = PWM_1_PERIOD_NS,
		.duty_cycle_ns = PWM_1_DUTY_NS,
		.phase_ns = PWM_1_PHASE,
		.platform_ops = &axi_pwm_ops,
		.extra = &axi_pwm_1_extra
	};

	struct xil_gpio_init_param xil_gpio_init = {
		.device_id = GPIO_DEVICE_ID,
		.type = GPIO_PS
	};

	struct no_os_gpio_init_param gpio_adc_en3_fmc_ip = {
		.number = GPIO_EN3_FMC,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA
	};

	struct no_os_gpio_init_param gpio_adc_en2_fmc_ip = {
		.number = GPIO_EN2_FMC,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA
	};

	struct no_os_gpio_init_param gpio_adc_en1_fmc_ip = {
		.number = GPIO_EN1_FMC,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA
	};

	struct no_os_gpio_init_param gpio_adc_en0_fmc_ip = {
		.number = GPIO_EN0_FMC,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA
	};

	struct ad796x_init_param ad796x_init_param = {
		.ad796x_core_ip = &ad796x_core_ip,
		.clkgen_ip = &clkgen_ip,
		.dmac_ip = &dmac_ip,
		.axi_pwm_0_ip = &axi_pwm_0_ip,
		.axi_pwm_1_ip = &axi_pwm_1_ip,
		.gpio_ip[3] = &gpio_adc_en3_fmc_ip,
		.gpio_ip[2] = &gpio_adc_en2_fmc_ip,
		.gpio_ip[1] = &gpio_adc_en1_fmc_ip,
		.gpio_ip[0] = &gpio_adc_en0_fmc_ip,
		.mode = AD796X_MODE1_EXT_REF_5P0,
	};

	ret = ad796x_init(&adc_dev, &ad796x_init_param);
	if (ret)
		return ret;

	pr_info("Capture start.\n");
	ret = ad796x_read_data(adc_dev, buf, SAMPLES_PER_CHANNEL);
	if (ret) {
		pr_err("read data error %d\n", ret);
		ad796x_remove(adc_dev);
		return ret;
	}

	for (i = 0; i < SAMPLES_PER_CHANNEL; i++, buf++)
		printf("CH1: %ld\n", *buf);


	return ad796x_remove(adc_dev);

AD796X no_OS IIO Support
-------------------------

The AD796X uses the iio_axi_adc driver to provide iio support.


AD796X IIO Driver Initialization Example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: bash

	struct ad796x_dev *adc_dev;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };
	struct iio_axi_adc_desc *iio_axi_adc_desc;
	struct iio_device *dev_desc;
	struct iio_data_buffer adc_buff = {
		.buff = (void *)ADC_DDR_BASEADDR,
		.size = MAX_SIZE_BASE_ADDR
	};
	struct scan_type init_scan_type = {
		.sign = 's',
		.realbits = 32,
		.storagebits = 32,
		.shift = 0,
		.is_big_endian = false
	};

	ret = ad796x_init(&adc_dev, &ad796x_init_param);
	if (ret) {
		pr_err("Error: ad796x_init: %d\n", ret);
		return ret;
	}

	struct iio_axi_adc_init_param iio_axi_adc_init_par = {
		.rx_adc = adc_dev->ad796x_core,
		.rx_dmac = adc_dev->axi_dmac,
		.scan_type_common = &init_scan_type,
		.dcache_invalidate_range = (void (*)(uint32_t,
						     uint32_t))Xil_DCacheInvalidateRange,
	};

	ret = iio_axi_adc_init(&iio_axi_adc_desc, &iio_axi_adc_init_par);
	if (ret) {
		pr_err("Error: iio_axi_adc_init: %d\n", ret);
		goto err_adc_init;
	}

	iio_axi_adc_get_dev_descriptor(iio_axi_adc_desc, &dev_desc);

	struct iio_app_device devices[] = {
		IIO_APP_DEVICE("ad796x", iio_axi_adc_desc,
			       dev_desc, &adc_buff, NULL, NULL),
	};

	app_init_param.devices = devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(devices);
	app_init_param.uart_init_params = iio_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret) {
		pr_err("Error: iio_app_init: %d\n", ret);
		goto err_app_init;
	}

	ret = iio_app_run(app);
