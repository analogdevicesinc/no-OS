LTC7871 no-OS driver
=====================

.. no-os-doxygen::

Supported Devices
-----------------

`LTC7871 <https://www.analog.com/LTC7871>`_

Overview
--------

The LTC7871 is a high performance bidirectional buck or boost switching 
regulator controller that operates in either buck or boost mode on demand. 
It regulates in buck mode from VHIGH-to-VLOW and boost mode from 
VLOW-to-VHIGH depending on a control signal, making it ideal for 48V/12V 
automotive dual battery systems. An accurate current programming loop 
regulates the maximum current that can be delivered in either direction. 
The LTC7871 allows both batteries to supply energy to the load simultaneously 
by driving energy from either battery to the other.

Its proprietary constant frequency current mode architecture enhances the 
signal-to-noise ratio enabling low noise operation and provides excellent 
current matching between phases. Additional features include an SPI-compliant 
serial interface, discontinuous or continuous mode of operation, OV/UV 
monitors, independent loop compensation for buck and boost operation, 
accurate inductor current monitoring and overcurrent protection.

The LTC7871 is available in a 64 pin 10mm × 10mm LWE package.

Applications
------------

LTC7871
--------

* Automotive 48V/12V Dual Battery Systems
* Backup Power Systems

LTC7871 Device Configuration
-----------------------------

Driver Initialization
---------------------

In order to be able to use the device, you will have to provide the support
for the communication protocol (SPI).

The first API to be called is **ltc7871_init**. Make sure that it returns 0,
which means that the driver was initialized correctly.

Reseting LT8491
-----------------

All write/read registers can be reset via **ltc7871_reset**. It writes 1 
to the reset bit of chip_ctrl register which resets all registers.

Monitoring Fault Condition
-----------------------------

Fault condition can be read and monitored via several registers.
**ltc7871_get_mfr_fault** provides the unit'faults condition, **ltc7871_get_mfr_oc_fault** 
provides unit's  overcurrent fault condition. **ltc7871_get_mfr_noc_fault**. 
gives the unit's negative overcurrent condition.

Monitoring Configuration of controller programmed by pins
-----------------------------------------------------------

Using **ltc7871_get_mfr_config1_setting** setcur pin, DRVcc and maximum 
current sense can be monitored. **ltc7871_get_mfr_config1_setting** provide
the modes of the device. It tells if the module is on burst mode, DCM,
Hi-Z mode, spread spectrum mode and boost/buck mode operation.

Managing communcation fault
-----------------------------

LTC7871 communication operates via SPI. Every transaction should 
contain parity error checking byte to ensure fata inegrity.
By using **ltc7871_clear_pec_fault** and **ltc7871_clear_pec_fault**,
managing/ handling fault related to PEC can be handled.

IDAC configuration
---------------------

There are three DAC registers that determines vlow and vhigh and setcur
by controlling the current magnitude and direction. 

To control these, this APIs can be used:
**ltc7871_get_mfr_idac_vlow**/**ltc7871_set_mfr_idac_vlow**
**ltc7871_get_mfr_idac_vhigh**/**ltc7871_set_mfr_idac_vhigh**
**ltc7871_get_mfr_idac_setcur**/**ltc7871_set_mfr_idac_setcur**

Writing  on these DAC registers can also be restricted via 
**ltc7871_is_write_protected**/**ltc7871_set_write_protect**


LTC7871 Driver Initialization Example
--------------------------------------

.. code-block:: bash

	struct ltc7871_device *dev;

	struct no_os_uart_init_param ltc7871_uart_ip = {
		.device_id = UART_DEVICE_ID,
		.baud_rate = UART_BAUDRATE,
		.size = NO_OS_UART_CS_8,
		.platform_ops = UART_OPS,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.extra = UART_EXTRA,
	};

	struct no_os_spi_init_param ltc7871_spi_ip = {
		.device_id = SPI_DEVICE_ID,
		.extra = SPI_EXTRA,
		.max_speed_hz = SPI_BAUDRATE,
		.platform_ops = SPI_OPS,
		.chip_select = SPI_CS,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.mode = NO_OS_SPI_MODE_0
	};

	struct no_os_gpio_init_param ltc7871_pwmen_ip = {
		.port = GPIO_PWMEN_PORT_NUM,
		.pull = NO_OS_PULL_NONE,
		.number = GPIO_PWMEN_PIN_NUM,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA
	};

	struct ltc7871_init_param ltc7871_ip = {
		.spi = &ltc7871_spi_ip,
		.gpio_pwmen = &ltc7871_pwmen_ip,
	};

	ret = ltc7871_init(&ltc7871_dev, &ltc7871_ip);
	if (ret)
		goto error;

LTC7871 no-OS IIO support
--------------------------

The LT8491 IIO driver comes on top of the LTC7871 driver and offers support
for interfacing IIO clients through libiio.

LTC7871 IIO Device Configuration
---------------------------------

Channel Attributes
------------------

Debug Attributes
----------------

The device has a total of 29 debug attributes:

* ``vhigh_ov - The OVLOW pin is higher than 1.2V threshold.``
* ``vhigh_uv - The OVHIGH pin is higher than 1.2V threshold``
* ``drvcc_uv - The DRVCC pin is undervoltage``
* ``v5_uv - The V5 pin is undervoltage``
* ``vref_bad - The internal reference self-check fails``
* ``over_temp - An over temperature fault has occurred``
* ``OC_FAULT_[1-6] - Channel x overcurrent fault has occurred``
* ``NOC_FAULT_[1-6] - Channel x negative overcurrent fault has occurred``
* ``ss_done - The soft-start is finished.``
* ``max_current - The maximum current programmed by the ILIM pin is reached.``
* ``pgood - The regulated VLOW/VHIGH is within ±10% regulation windows``
* ``sercur_warning - The SETCUR pin is programmed to be above 1.25V``
* ``drvcc_set - DRVCC value``
* ``ilim_set - maximum current sense threshold``
* ``burst - The controller is in burst mode operation.``
* ``dcm - The controller is in DCM.``
* ``hiz - The controller is in Hi-Z mode.``
* ``sprd - The controller is in spread spectrum mode.``
* ``buck_boost - buc/boost mode operation``

Global Attributes
---------------------
The device has a total of 8 debug attributes:

* ``reset - reset al W/R registers.``
* ``pec_fault - pec fault bit indicating communication error.``
* ``write_protect - protects IDAC registers from write.``
* ``idac_vlow - current DAC values for Vlow.``
* ``idac_vhigh - current DAC values for Vhigh.``
* ``idac_setcur - current DAC values for Vsetcur.``
* ``freq_spread_range - frequency spread range.``
* ``mod_freq - Modulation signal frequency.``

LTC7871 IIO Driver Initialization Example
------------------------------------------

.. code-block:: bash

	int ret;

	struct ltc7871_iio_dev *ltc7871_iio_dev;
	struct ltc7871_iio_dev_init_param ltc7871_iio_ip = {	
		.ltc7871_init_param = &ltc7871_ip,
	};

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	ret = ltc7871_iio_init(&ltc7871_iio_dev, &ltc7871_iio_ip);
	if (ret)
		goto exit;

	struct iio_app_device iio_devices[] = {
		{
			.name = "ltc7871",
			.dev = ltc7871_iio_dev,
			.dev_descriptor = ltc7871_iio_dev->iio_dev,
		}
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = ltc7871_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto remove_iio_ltc7871;

	ret = iio_app_run(app);
