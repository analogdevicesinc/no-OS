ADRV903x (Palma) no-OS Driver
==============================

Supported Devices
-----------------

`ADRV9032 <https://www.analog.com/en/products/adrv9032r.html>`_

Overview
--------

The ADRV9032 is a 4-transmitter / 4-receiver (4T4R) integrated RF transceiver
operating from 650 MHz to 6 GHz with an on-chip ARM Cortex-A55 application
processor.  It features JESD204C high-speed serial interfaces, integrated PLLs,
and on-chip digital signal processing, providing a complete transceiver solution
for cellular infrastructure applications.

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
----------------

| no-OS/drivers/rf-transceiver/palma/
| ├── adrv903x.c                ← no-OS driver + JESD204 FSM callbacks
| ├── adrv903x.h
| ├── common/                   ← ADI common error, HAL, logging
| │   ├── adi_common.h
| │   ├── adi_error/
| │   ├── adi_hal/
| │   └── adi_logging/
| ├── devices/
| │   └── adrv903x/             ← Palma device API
| │       ├── private/          ← Bitfield access, internal calibrations
| │       └── public/           ← Public API headers and sources
| ├── platforms/                ← Platform abstraction layer
| └── README.rst

The *common* folder contains the ADI HAL, error handling, and logging
infrastructure.  The *devices* folder contains the Palma device API; files in
this folder must not be modified.  The *platforms* folder provides the platform
abstraction layer that must be implemented for each target.

ADRV903x Device Configuration
------------------------------

Driver Initialization
---------------------

The driver integrates with the no-OS JESD204 FSM.  All link bring-up steps
(MCS, PostMcsInit, SERDES calibration, framer/deframer enable) are driven by
FSM callbacks registered in ``adrv903x.c``.

Before calling ``adrv903x_init()``, populate the initialization parameter
structure:

.. code-block:: C

	struct adrv903x_init_param adrv903x_init_par = {
		.palmaDevice      = &palma_device,
		.dev_clk          = ad9528_device->clk_desc[1],
		.reset_gpio_param = &reset_gpio_param,
	};

	status = adrv903x_init(&phy, &adrv903x_init_par);
	if (status) {
		pr_err("adrv903x_init() failed: %d\n", status);
		goto error;
	}

``palmaDevice`` is a zero-initialized ``adi_adrv903x_Device_t`` structure.
``dev_clk`` is the DEVCLK descriptor (245.760 MHz on channel 1 of the AD9528).
``reset_gpio_param`` describes the GPIO connected to the ADRV903x RESETB pin.

After ``adrv903x_init()`` succeeds, start the JESD204 FSM:

.. code-block:: C

	status = jesd204_fsm_start(phy->jdev, JESD204_LINKS_ALL);
	if (status) {
		pr_err("jesd204_fsm_start() failed: %d\n", status);
		goto error;
	}

The FSM drives the full link bring-up sequence through the registered
callbacks and signals completion when the link reaches DATA state.

Configuration
-------------

All transceiver configurations (profile, gain tables, calibrations) are
provided via the firmware header files embedded in the project under
``src/common/firmware/``.  Runtime parameters such as TX attenuation and
RX gain can be adjusted using the Palma API directly after FSM completion.

More information can be found in the ADRV9032 System Development User Guide
available from the `ADRV9032 product page
<https://www.analog.com/en/products/adrv9032r.html>`_.

ADRV903x no-OS IIO Support
---------------------------

The ADRV903x IIO support is based on the AXI DAC and AXI ADC IIO drivers.

IIO Device Configuration
-------------------------

Device Attributes
-----------------

The ADRV903x IIO device does not have any device-specific attributes.

Device Channels
---------------

The ADRV903x IIO device exposes 4 input voltage channels (ADC) and 4 output
voltage channels (DAC).

ADRV903x IIO Driver Initialization Example
-------------------------------------------

.. code-block:: C

	struct iio_axi_adc_init_param iio_axi_adc_init_par = {
		.rx_adc = phy->rx_adc,
		.rx_dmac = rx_dmac,
		.dcache_invalidate_range = (void (*)(uint32_t,
						     uint32_t))Xil_DCacheInvalidateRange,
	};

	status = iio_axi_adc_init(&iio_axi_adc_desc, &iio_axi_adc_init_par);
	if (status < 0)
		goto error;

	struct iio_axi_dac_init_param iio_axi_dac_init_par = {
		.tx_dac = phy->tx_dac,
		.tx_dmac = tx_dmac,
		.dcache_flush_range = (void (*)(uint32_t, uint32_t))Xil_DCacheFlushRange,
	};

	status = iio_axi_dac_init(&iio_axi_dac_desc, &iio_axi_dac_init_par);
	if (status < 0)
		goto error;

	struct iio_app_device devices[] = {
		IIO_APP_DEVICE("cf-adrv903x-lpc", iio_axi_adc_desc,
			       adc_dev_desc, &read_buff, NULL, NULL),
		IIO_APP_DEVICE("cf-adrv903x-dds-core-lpc", iio_axi_dac_desc,
			       dac_dev_desc, NULL, &write_buff, NULL),
	};

	app_init_param.devices = devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(devices);
	app_init_param.uart_init_params = iio_uart_ip;

	status = iio_app_init(&app, app_init_param);
	if (status)
		goto error;

	return iio_app_run(app);
