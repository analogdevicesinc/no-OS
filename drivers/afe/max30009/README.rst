MAX30009 no-OS Driver
=====================

.. no-os-doxygen::

Supported Devices
-----------------

`MAX30009 <https://www.analog.com/MAX30009>`_

Overview
--------

The MAX30009 is a low-power, high-performance analog front-end (AFE) for bioimpedance
(BioZ) measurement applications. The device features dual-channel (I and Q) data
acquisition, frequency and phase lock detection, 256-word FIFO for data buffering,
lead-off detection for electrode monitoring, and I2C interface with configurable address.
It supports programmable measurement gain (10 V/V to 100 V/V), configurable drive
current (32 μA to 96 μA peak), and adjustable amplifier bandwidth (32 kHz to 256 kHz).

Applications
------------

* Body composition analysis
* Impedance cardiography (ICG)
* Respiration monitoring
* Wearable medical devices
* Vital signs monitoring

MAX30009 Device Configuration
------------------------------

Driver Initialization
---------------------

In order to use the device, you will have to provide support for the communication
protocol (I2C). The first API to be called is **max30009_init**. Make sure that it
returns 0, which means the driver was initialized correctly.

.. code-block:: c

	struct max30009_dev *dev;
	struct max30009_init_param init_param = {
		.use_i2c = true,
		.i2c_init = {
			.device_id = 0,
			.slave_address = MAX30009_I2C_ADDRESS,
			.max_speed_hz = 400000,
		},
		.int_gpio_init = NULL,
	};

	ret = max30009_init(&dev, &init_param);
	if (ret)
		return ret;

PLL Configuration
-----------------

The PLL must be configured and enabled before BioZ measurements. Use **max30009_set_pll_config**
to configure the PLL parameters and **max30009_pll_enable** to enable the PLL.

.. code-block:: c

	struct max30009_pll_config pll_cfg = {
		.use_external_clk = false,
		.use_32768_hz = true,
		.mdiv = 511,
		.ndiv = MAX30009_NDIV_1024,
		.kdiv = MAX30009_KDIV_2,
	};

	ret = max30009_set_pll_config(dev, &pll_cfg);
	if (ret)
		return ret;

	ret = max30009_pll_enable(dev, true);
	if (ret)
		return ret;

BioZ Measurement Configuration
-------------------------------

Configure BioZ measurement parameters using **max30009_set_bioz_config**. This includes
gain, drive mode, oversampling ratios, and filters.

.. code-block:: c

	struct max30009_bioz_config bioz_cfg;

	bioz_cfg.gain = MAX30009_BIOZ_GAIN_10;
	bioz_cfg.drv_mode = MAX30009_CURRENT_DRV_MODE;
	bioz_cfg.dac_osr = MAX30009_DAC_OSR_256;
	bioz_cfg.adc_osr = MAX30009_ADC_OSR_1024;

	ret = max30009_set_bioz_config(dev, &bioz_cfg);
	if (ret)
		return ret;

FIFO Management
---------------

The MAX30009 has a 256-word FIFO for buffered data acquisition. Configure the FIFO
using **max30009_set_fifo_config**, read the count with **max30009_fifo_get_count**,
and read data using **max30009_read_fifo_data**.

.. code-block:: c

	/* Configure FIFO */
	struct max30009_fifo_config fifo_cfg = {
		.watermark = 128,
		.fifo_rollover = true,
	};
	ret = max30009_set_fifo_config(dev, &fifo_cfg);

	/* Read FIFO data */
	uint16_t count;
	uint8_t fifo_data[768];

	ret = max30009_fifo_get_count(dev, &count);
	if (ret)
		return ret;

	if (count > 0) {
		ret = max30009_read_fifo_data(dev, fifo_data, count * 3);
		if (ret)
			return ret;
	}

Lead-Off Detection
------------------

Configure lead-off detection using **max30009_set_lead_detect_config** and check
the status with **max30009_check_lead_off**.

.. code-block:: c

	struct max30009_lead_detect_config lead_cfg = {
		.en_lon_det = true,
		.en_loff_det = true,
		.loff_ipol = true,
		.loff_imag = 8,
	};

	ret = max30009_set_lead_detect_config(dev, &lead_cfg);
	if (ret)
		return ret;

	/* Check lead status */
	bool leads_on;
	ret = max30009_check_lead_off(dev, &leads_on);
	if (ret)
		return ret;

Status Monitoring
-----------------

Read device status using **max30009_get_status** to check PLL lock, power ready,
and FIFO status.

.. code-block:: c

	struct max30009_status status;

	ret = max30009_get_status(dev, &status);
	if (ret)
		return ret;

	if (status.freq_lock && status.phase_lock)
		printf("PLL locked\\n");
