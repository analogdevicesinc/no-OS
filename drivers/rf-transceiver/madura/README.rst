ADRV902x (Madura) no-OS Driver
==============================

Supported Devices
-----------------

`ADRV9026 <https://www.analog.com/ADRV9026>`_

`ADRV9029 <https://www.analog.com/ADRV9029>`_

Overview
--------

As demand for data increases globally, telecom infrastructure manufacturers are challenged by shorter time to market, increased antenna count, ever-growing cost pressure, and proliferation in variants of form factors, frequency bands, output power, and software. The `ADRV9026 <https://www.analog.com/ADRV9026>`_, `ADI <https://www.analog.com/en/index.html>`_'s 4th generation wideband RF transceiver, delivers quad-channel integration with the lowest power, smallest size, common platform solution available to simplify design and reduce system power, size, weight, and costs for 3G/4G/5G applications, including multi-standard base stations, massive MIMO, and small cells.

The `ADRV9026 <https://www.analog.com/ADRV9026>`_ is a highly integrated, radio frequency (RF) agile transceiver offering four independently controlled transmitters, dedicated observation receiver inputs for monitoring each transmitter channel, four independently controlled receivers, integrated synthesizers, and digital signal processing functions providing a complete transceiver solution. The device provides the performance demanded by cellular infrastructure applications, such as small cell base station radios, macro 3G/4G/5G systems, and massive multiple in/multiple out (MIMO) base stations. 

The `ADRV9029 <https://www.analog.com/ADRV9029>`_ is a highly integrated, radio frequency (RF) agile transceiver offering four independently controlled transmitters, dedicated observation receiver inputs for monitoring each transmitter channel, four independently controlled receivers, integrated synthesizers, and digital signal processing functions providing a complete transceiver solution. The device provides the performance demanded by cellular infrastructure applications, such as small cell base station radios, macro 3G/4G/5G systems, and massive multiple in/multiple out (MIMO) base stations.

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

Note that ADRV9025 is the generic transceiver reference number for the `ADRV902x <https://www.analog.com/en/applications/technology/sdr-radioverse-pavilion-home/wideband-transceivers/adrv9026-wideband-transceiver.html>`_ family; all API functions use the ADRV9025 number to delineate the product from other transceiver products. With
respect to this user guide, ADRV9025 is interchangeable with `ADRV9026 <https://www.analog.com/ADRV9026>`_ and `ADRV9029 <https://www.analog.com/ADRV9029>`_.

| no-OS/drivers/rf-transceiver/madura/
| ├── adrv9025.c
| ├── adrv9025_conv.c
| ├── adrv9025.c.orig
| ├── adrv9025.h
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
| │   │   ├── adi_common_hal.h
| │   │   └── adi_common_hal_types.h
| │   └── adi_logging
| │       ├── adi_common_log.c
| │       ├── adi_common_log.h
| │       └── adi_common_log_types.h
| ├── devices
| │   └── adrv9025
| │       ├── private
| │       │   ├── include
| │       │   └── src
| │       └── public
| │           ├── include
| │           └── src
| ├── platforms
| │   ├── adi_platform.h
| │   └── adi_platform_types.h
| └── README.rst

The *common* folder contains the API code for the ADI HAL, for logging, and for error handling. The user modificable defines can be placed in the *adi_common_user.h* file.

The *devices* folder includes the main API code for the transceiver. The */adrv9025* folder contains the high 
level function prototypes, data types, macros, and source code to build the final user software system. 
The user is strictly forbidden to modify the files contained in this folder. Note that software support cannot be provided if these files have been modified. 
Analog Devices maintains this code. 
The only exception is that the developer may modify user-selectable *#define macros*, such as *ADI_ADRV9025_VERBOSE* mode to enable or disable API logging, and
user configurable macros defined in */adrv9025/public/include/adi_adrv9025_user.h*.

The *platforms* folder provides the means for a developer to insert custom platform hardware driver code for
system integration with the API. The files here contain the required prototypes necessary for the API
to work correctly. The developer is responsible for implementing the code for each function with the prototype declared in *adi_platform.h* and *adi_platform_types.h* in the application layer.
The correct hardware drivers must be called for the platform hardware of the user.

ADRV902x Device Configuration
-----------------------------

Driver Initialization
---------------------

In order to be able to use the device, you will have to provide the support for
the SPI communication protocol.

The SPI settings must be provided as follows:

.. code-block:: C

	/* SPI settings */
	static adi_adrv9025_SpiSettings_t spiSettings = {
		.msbFirst = 1,
		.enSpiStreaming = 0,
		.autoIncAddrUp = 1,
		.fourWireMode = 1,
		.cmosPadDrvStrength = ADI_ADRV9025_CMOSPAD_DRV_STRONG,
	};

The following function will then be used by the driver for obtaining the SPI settings in the initialization phase:

.. code-block:: C

	adi_adrv9025_SpiSettings_t *adrv9025_spi_settings_get(void)
	{
		return &spiSettings;
	}

Before calling the *adrv9025_init* function, the initialization parameter structure must be populated with the expected values. An example is given below:

.. code-block:: C

	adrv9025_init_par.adrv9025_device = &adrv9025_device;
	adrv9025_init_par.dev_clk = ad9528_device->clk_desc[1];
	adrv9025_init_par.streamImageFile = ADRV9025_STREAM_IMAGE_FILE;

*adrv9025_device* is an *adi_adrv9025_Device* structure initialized with zeros, *ad9528_device->clk_desc[1]* is the device clock (here, channel 1 of the AD9528 chip), while *ADRV9025_STREAM_IMAGE_FILE* is the name of the stream image generated for the embedded ARM processor core in the ADRV902x family devices (defined in *app_config.h*).

Make sure that *adrv9025_init* returns 0, which means that the driver was initialized correctly.

The following sequence of instructions is then required for initializing the AXI ADC and AXI DAC cores, before calling *adrv9025_post_setup*:

.. code-block:: C

	status = axi_dac_init_begin(&adrv9025_phy->tx_dac, &tx_dac_init);
	if (status)
		goto error_6;
	status = axi_adc_init_begin(&adrv9025_phy->rx_adc, &rx_adc_init);
	if (status)
		goto error_7;

	// Reset Tx DAC
	axi_adc_write(adrv9025_phy->rx_adc, 0x4040, 0);
	axi_adc_write(adrv9025_phy->rx_adc, 0x4040,
		      NO_OS_BIT(1) | NO_OS_BIT(0));

	// Reset Rx ADC
	axi_adc_write(adrv9025_phy->rx_adc, AXI_ADC_REG_RSTN, 0);
	axi_adc_write(adrv9025_phy->rx_adc, AXI_ADC_REG_RSTN,
		      AXI_ADC_MMCM_RSTN | AXI_ADC_RSTN);

	status = adrv9025_post_setup(adrv9025_phy);
	if (status) {
		pr_err("error: adrv9025_post_setup() failed\n");
		goto error_8;
	}

Configuration
-------------------

All configurations are handled by the functions in the *adrv9025.c* and *adrv9025_conv.c* files.
More information regarding the system operation and setup can be found in the `ADRV9026/ADRV9029 System Development User Guide <https://www.analog.com/media/radioverse-adrv9026/adrv9026-system-development-user-guide-ug-1727.pdf>`_.

ADRV9025 Driver Initialization Example
--------------------------------------

.. code-block:: bash

	/* SPI settings */
	static adi_adrv9025_SpiSettings_t spiSettings = {
		.msbFirst = 1,
		.enSpiStreaming = 0,
		.autoIncAddrUp = 1,
		.fourWireMode = 1,
		.cmosPadDrvStrength = ADI_ADRV9025_CMOSPAD_DRV_STRONG,
	};

	/***************************************************************************//**
	* @brief Function for obtaining SPI settings.
	*
	* @return SPI settings
	*******************************************************************************/
	adi_adrv9025_SpiSettings_t *adrv9025_spi_settings_get(void)
	{
		return &spiSettings;
	}

	...

	struct adrv9025_init_param adrv9025_init_par = { 0 };
	struct adi_adrv9025_Device adrv9025_device = { 0 };
	struct adrv9025_rf_phy *phy;
	int status;

	struct axi_adc_init rx_adc_init = {
		.name = "rx_adc",
		.base = RX_CORE_BASEADDR,
		.num_channels = 8
	};
	struct axi_dac_init tx_dac_init = {
		.name = "tx_dac",
		.base = TX_CORE_BASEADDR,
		.channels = NULL,
		.rate = 3,
		.num_channels = 8
	};

	...

	adrv9025_init_par.adrv9025_device = &adrv9025_device;
	adrv9025_init_par.dev_clk = ad9528_device->clk_desc[1];
	adrv9025_init_par.streamImageFile = ADRV9025_STREAM_IMAGE_FILE;

	status = adrv9025_init(&phy, &adrv9025_init_par);
	if (status) {
		pr_err("error: adrv9025_init() failed\n");
		goto error_5;
	}

	status = axi_dac_init_begin(&phy->tx_dac, &tx_dac_init);
	if (status)
		goto error_6;
	status = axi_adc_init_begin(&phy->rx_adc, &rx_adc_init);
	if (status)
		goto error_7;

	// Reset Tx DAC
	axi_adc_write(phy->rx_adc, 0x4040, 0);
	axi_adc_write(phy->rx_adc, 0x4040,
		      NO_OS_BIT(1) | NO_OS_BIT(0));

	// Reset Rx ADC
	axi_adc_write(phy->rx_adc, AXI_ADC_REG_RSTN, 0);
	axi_adc_write(phy->rx_adc, AXI_ADC_REG_RSTN,
		      AXI_ADC_MMCM_RSTN | AXI_ADC_RSTN);

	status = adrv9025_post_setup(phy);
	if (status) {
		pr_err("error: adrv9025_post_setup() failed\n");
		goto error_8;
	}

	status = axi_dmac_init(&tx_dmac, &tx_dmac_init);
	if (status) {
		printf("axi_dmac_init tx init error: %d\n", status);
		goto error_8;
	}
	status = axi_dmac_init(&rx_dmac, &rx_dmac_init);
	if (status) {
		printf("axi_dmac_init rx init error: %d\n", status);
		goto error_9;
	}

	...


ADRV9025 no-OS IIO support
--------------------------

The ADRV9025 suppord is based on the AXI DAC and AXI ADC IIO drivers.

ADRV9025 IIO Device Configuration
---------------------------------

Device Attributes
-----------------

The ADRV9025 IIO device does not have any device specific attributes.

Device Channels
---------------

The ADRV9025 IIO device has 4 output voltage channels and 4 input voltage channels.

ADRV9025 IIO Driver Initialization Example
------------------------------------------

.. code-block:: bash

	#define DAC_BUFFER_SAMPLES 8192
	#define ADC_BUFFER_SAMPLES              16384
	#define ADC_CHANNELS                    4
	uint32_t dac_buffer[DAC_BUFFER_SAMPLES] __attribute__ ((aligned));
	uint16_t adc_buffer[ADC_BUFFER_SAMPLES * ADC_CHANNELS] __attribute__ ((
			aligned));

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
		.dcache_invalidate_range = (void (*)(uint32_t,
						     uint32_t))Xil_DCacheInvalidateRange,
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
		.dcache_flush_range = (void (*)(uint32_t, uint32_t))Xil_DCacheFlushRange,
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
		IIO_APP_DEVICE("cf-adrv9025-lpc", iio_axi_adc_desc, adc_dev_desc, &read_buff, NULL, NULL),
		IIO_APP_DEVICE("cf-adrv9025-dds-core-lpc", iio_axi_dac_desc, dac_dev_desc, NULL, &write_buff, NULL)
	};

	app_init_param.devices = devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(devices);
	app_init_param.uart_init_params = iio_uart_ip;

	status = iio_app_init(&app, app_init_param);
	if (status)
		goto error_12;

	return iio_app_run(app);