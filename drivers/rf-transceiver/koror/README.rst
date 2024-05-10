ADRV904x (Koror) no-OS Driver
==============================

Supported Devices
-----------------

`ADRV9040 <https://www.analog.com/ADRV9040>`_

Overview
--------

The ADRV9040 is a highly integrated, system on chip (SoC) radio frequency (RF) agile transceiver with integrated digital front end (DFE). The SoC contains eight transmitters, two observation receivers to monitor transmitter channels, eight receivers, integrated LO and clock synthesizers, and digital signal processing functions. The SoC meets the high radio performance and low power consumption demanded by cellular infrastructure applications including small cell base-station radios, macro 3G/4G/5G systems, and massive MIMO base stations.

Applications
------------

* Macro base stations
* Massive MIMO
* Small cells

Markets and Technologies
------------------------

* Aerospace and Defense
* Communications

Driver Structure
-----------------

| drivers/rf-transceiver/koror/
| ├── adrv904x.c
| ├── adrv904x_conv.c
| ├── adrv904x.h
| ├── common
| │   ├── adi_common.h
| │   ├── adi_common_macros.h
| │   ├── adi_common_types.h
| │   ├── adi_common_user.h
| │   ├── adi_error
| │   │   ├── adi_common_error.c
| │   │   ├── adi_common_error.h
| │   │   └── adi_common_error_types.h
| │   ├── adi_hal
| │   │   ├── adi_common_hal.c
| │   │   └── adi_common_hal.h
| │   └── adi_logging
| │       ├── adi_common_log.c
| │       ├── adi_common_log.h
| │       └── adi_common_log_types.h
| ├── devices
| │   └── adrv904x
| │       ├── multiver
| │       ├── private
| │       └── public
| ├── platforms
| │   ├── adi_library.c
| │   ├── adi_library.h
| │   ├── adi_library_types.h
| │   ├── adi_platform_aux.c
| │   ├── adi_platform_aux.h
| │   ├── adi_platform.c
| │   ├── adi_platform.h
| │   ├── adi_platform_impl.h
| │   ├── adi_platform_impl_types.h
| │   ├── adi_platform_types.h
| │   └── common
| │       ├── adi_logging.c
| │       ├── adi_logging.h
| │       ├── tls.c
| │       └── tls.h
| └── README.rst

ADRV904x Device Configuration
-----------------------------

Driver Initialization
---------------------

In order to be able to use the device, you will have to provide the support for
the SPI communication protocol.

The SPI settings must be provided as follows:

.. code-block:: C

	static adi_adrv904x_SpiConfigSettings_t spiSettings = {
		.msbFirst = 1,
		.fourWireMode = 1,
		.cmosPadDrvStrength = ADI_ADRV904X_CMOSPAD_DRV_STRONG,
	};

	static adi_adrv904x_SpiOptions_t spiOptions = {
		.allowSpiStreaming = 0,
		.allowAhbAutoIncrement = 1,
		.allowAhbSpiFifoMode = 0,
	};

The following functions will then be used by the driver for obtaining the SPI settings in the initialization phase:

.. code-block:: C

	/***************************************************************************//**
	* @brief Function for obtaining SPI settings.
	*
	* @return SPI settings
	*******************************************************************************/
	adi_adrv904x_SpiConfigSettings_t *adrv904x_spi_settings_get(void)
	{
		return &spiSettings;
	}

	/***************************************************************************//**
	* @brief Function for obtaining SPI options.
	*
	* @return SPI settings
	*******************************************************************************/
	adi_adrv904x_SpiOptions_t *adrv904x_spi_options_get(void)
	{
		return &spiOptions;
	}

Before calling the *adrv904x_init* function, the initialization parameter structure must be populated with the expected values. An example is given below:

.. code-block:: C

	struct adrv904x_init_param adrv904x_init_par = { 0 };
	adrv904x_init_par.adrv904x_device = &adrv904x_device;
	adrv904x_init_par.dev_clk = ad9528_device->clk_desc[1];

*adrv904x_device* is an *adi_adrv904x_Device* structure initialized with zeros, *ad9528_device->clk_desc[1]* is the device clock (here, channel 1 of the AD9528 chip).

Make sure that *adrv904x_init* returns 0, which means that the driver was initialized correctly.

The following sequence of instructions is then required for initializing the AXI ADC and AXI DAC cores, before calling *adrv904x_post_setup*:

.. code-block:: C

	status = axi_dac_init(&phy->tx_dac, &tx_dac_init);
	if (status)
		goto error_6;
	status = axi_adc_init_begin(&phy->rx_adc, &rx_adc_init);
	if (status)
		goto error_7;

	// Reset Rx ADC
	axi_adc_write(phy->rx_adc, AXI_ADC_REG_RSTN, 0);
	axi_adc_write(phy->rx_adc, AXI_ADC_REG_RSTN,
		      AXI_ADC_MMCM_RSTN | AXI_ADC_RSTN);

	status = adrv904x_post_setup(phy);
	if (status) {
		pr_err("error: adrv904x_post_setup() failed\n");
		goto error_8;
	}

Configuration
-------------------

All configurations are handled by the functions in the *adrv904x.c* and *adrv904x_conv.c* files.
More information regarding the system operation and setup can be found in the `ADRV9040 System Development User Guide <https://www.analog.com/media/en/technical-documentation/user-guides/adrv9040-reference-manual-ug-2192.pdf>`_.

ADRV904X Driver Initialization Example
--------------------------------------

.. code-block:: bash

	static adi_adrv904x_SpiConfigSettings_t spiSettings = {
		.msbFirst = 1,
		.fourWireMode = 1,
		.cmosPadDrvStrength = ADI_ADRV904X_CMOSPAD_DRV_STRONG,
	};

	static adi_adrv904x_SpiOptions_t spiOptions = {
		.allowSpiStreaming = 0,
		.allowAhbAutoIncrement = 1,
		.allowAhbSpiFifoMode = 0,
	};

	/***************************************************************************//**
	* @brief Function for obtaining SPI settings.
	*
	* @return SPI settings
	*******************************************************************************/
	adi_adrv904x_SpiConfigSettings_t *adrv904x_spi_settings_get(void)
	{
		return &spiSettings;
	}

	/***************************************************************************//**
	* @brief Function for obtaining SPI options.
	*
	* @return SPI settings
	*******************************************************************************/
	adi_adrv904x_SpiOptions_t *adrv904x_spi_options_get(void)
	{
		return &spiOptions;
	}
	...

	struct adrv904x_init_param adrv904x_init_par = { 0 };
	struct adi_adrv904x_Device adrv904x_device = { 0 };
	struct adrv904x_rf_phy *phy;

	struct axi_adc_init rx_adc_init = {
		.name = "rx_adc",
		.base = RX_CORE_BASEADDR,
		.num_channels = 16
	};
	struct axi_dac_init tx_dac_init = {
		.name = "tx_dac",
		.base = TX_CORE_BASEADDR,
		.channels = NULL,
		.rate = 3,
		.num_channels = 16
	};

	...

	adrv904x_init_par.adrv904x_device = &adrv904x_device;
	adrv904x_init_par.dev_clk = ad9528_device->clk_desc[1];

	status = adrv904x_init(&phy, &adrv904x_init_par);
	if (status) {
		pr_err("error: adrv904x_init() failed\n");
		goto error_5;
	}

	status = axi_dac_init(&phy->tx_dac, &tx_dac_init);
	if (status)
		goto error_6;
	status = axi_adc_init_begin(&phy->rx_adc, &rx_adc_init);
	if (status)
		goto error_7;

	// Reset Rx ADC
	axi_adc_write(phy->rx_adc, AXI_ADC_REG_RSTN, 0);
	axi_adc_write(phy->rx_adc, AXI_ADC_REG_RSTN,
		      AXI_ADC_MMCM_RSTN | AXI_ADC_RSTN);

	status = adrv904x_post_setup(phy);
	if (status) {
		pr_err("error: adrv904x_post_setup() failed\n");
		goto error_8;
	}

	...


ADRV904X no-OS IIO support
--------------------------

The ADRV904X suppord is based on the AXI DAC and AXI ADC IIO drivers.

ADRV904X IIO Device Configuration
---------------------------------

Device Attributes
-----------------

The ADRV904X IIO device does not have any device specific attributes.

Device Channels
---------------

The ADRV904X IIO device has 8 output voltage channels and 8 input voltage channels.

ADRV904X IIO Driver Initialization Example
------------------------------------------

.. code-block:: bash

	#define DAC_BUFFER_SAMPLES              8192
	#define ADC_BUFFER_SAMPLES              32768
	#define ADC_CHANNELS                    8

		uint32_t dac_buffer[DAC_BUFFER_SAMPLES] __attribute__ ((aligned));
			uint16_t adc_buffer[ADC_BUFFER_SAMPLES * ADC_CHANNELS] __attribute__ ((
						aligned(1024)));

			uint16_t adc_buffer_dma[ADC_BUFFER_SAMPLES * ADC_CHANNELS] __attribute__ ((
				aligned(1024)));

		/**
		* IIO application descriptor
		*/
		struct iio_app_desc *app;

		/**
		* IIO application initialization parameters
		*/
		struct iio_app_init_param app_init_param = { 0 };

		/**
		* iio axi adc configurations.
		*/
		struct iio_axi_adc_init_param iio_axi_adc_init_par;

		/**
		* iio axi dac configurations.
		*/
		struct iio_axi_dac_init_param iio_axi_dac_init_par;

		/**
		* iio instance descriptor.
		*/
		struct iio_axi_adc_desc *iio_axi_adc_desc;

		/**
		* iio instance descriptor.
		*/
		struct iio_axi_dac_desc *iio_axi_dac_desc;

		/**
		* iio device instance descriptor.
		*/
		struct iio_device *adc_dev_desc, *dac_dev_desc;

		iio_axi_adc_init_par = (struct iio_axi_adc_init_param) {
			.rx_adc = phy->rx_adc,
			.rx_dmac = rx_dmac,
	#ifndef PLATFORM_MB
			.dcache_invalidate_range = (void (*)(uint32_t,
							uint32_t))Xil_DCacheInvalidateRange,
	#endif
		};

		status = iio_axi_adc_init(&iio_axi_adc_desc, &iio_axi_adc_init_par);
		if(status < 0)
			goto error_10;

		struct iio_data_buffer read_buff = {
			.buff = (void *)adc_buffer,
			.size = sizeof(adc_buffer),
		};
		iio_axi_adc_get_dev_descriptor(iio_axi_adc_desc, &adc_dev_desc);

		iio_axi_dac_init_par = (struct iio_axi_dac_init_param) {
			.tx_dac = phy->tx_dac,
			.tx_dmac = tx_dmac,
	#ifndef PLATFORM_MB
			.dcache_flush_range = (void (*)(uint32_t, uint32_t))Xil_DCacheFlushRange,
	#endif
		};

		status = iio_axi_dac_init(&iio_axi_dac_desc, &iio_axi_dac_init_par);
		if(status < 0)
			goto error_11;
		struct iio_data_buffer write_buff = {
			.buff = (void *)dac_buffer,
			.size = sizeof(dac_buffer),
		};
		iio_axi_dac_get_dev_descriptor(iio_axi_dac_desc, &dac_dev_desc);
		struct iio_app_device devices[] = {
			IIO_APP_DEVICE("cf-adrv904x-lpc", iio_axi_adc_desc, adc_dev_desc, &read_buff, NULL, NULL),
			IIO_APP_DEVICE("cf-adrv904x-dds-core-lpc", iio_axi_dac_desc, dac_dev_desc, NULL, &write_buff, NULL)
		};

		app_init_param.devices = devices;
		app_init_param.nb_devices = NO_OS_ARRAY_SIZE(devices);
		app_init_param.uart_init_params = iio_uart_ip;

		/* Wait to display previous messages */
		no_os_mdelay(100);

		status = iio_app_init(&app, app_init_param);
		if (status)
			goto error_12;

		return iio_app_run(app);