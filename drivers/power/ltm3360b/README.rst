LTM3360B no-OS driver
=====================

.. no-os-doxygen::

Supported Devices
-----------------

`LTM3360B <https://www.analog.com/en/products/ltm3360b.html>`_

Overview
--------

The LTM3360B is a high-current step-down DC-to-DC µModule regulator featuring
an integrated I2C interface for digital control and monitoring. This complete
power system delivers up to 33A of continuous output current at output voltages
as low as 300mV. It provides real-time monitoring of critical parameters including 
input/output voltage, output current, and die temperature through a built-in ADC.

Key features include programmable current monitoring thresholds, digital enable
control, and comprehensive telemetry acquisition capabilities.

Applications
------------

LTM3360B
--------

* Data Centers xPU Core, Application-Specific
* Integrated Circuit (ASIC), Field-Programmable Gate
* Array (FPGA) Supplies
* Optical Modules
* Industrial and Communications
* Distributed DC Point-of-Load (POL) Power Systems

LTM3360B Device Configuration
-----------------------------

Driver Initialization
---------------------

In order to be able to use the device, you will have to provide the support
for the communication protocol (I2C).

The first API to be called is **ltm3360b_init**. Make sure that it returns 0,
which means that the driver was initialized correctly.

The initialization API uses the device descriptor and an initialization
parameter. The initialization parameter contains the I2C configuration,
auto-enable setting, initial output voltage, and IMON threshold setting.

Output Voltage Configuration
----------------------------

The LTM3360B output voltage can be configured from 297mV to 1000mV with 4mV
resolution using the **ltm3360b_set_vout_mv** API. The current output voltage
setting can be read back using **ltm3360b_get_vout_mv** API.

Device Enable Control
---------------------

The device can be enabled or disabled using the **ltm3360b_enable_device** API.
This controls the main power stage operation of the regulator.

IMON Threshold Configuration
----------------------------

The current monitoring threshold can be configured using the
**ltm3360b_set_imon_threshold** API. Available threshold options are:

* LTM3360B_IMON_TH_99_PERCENT - 99% threshold
* LTM3360B_IMON_TH_89_PERCENT - 89% threshold
* LTM3360B_IMON_TH_79_PERCENT - 79% threshold
* LTM3360B_IMON_TH_69_PERCENT - 69% threshold

ADC and Telemetry
-----------------

The integrated ADC can be enabled using **ltm3360b_adc_enable** API and provides
monitoring for multiple channels using **ltm3360b_adc_read_raw** API:

* LTM3360B_ADC_IMON - Output current monitoring
* LTM3360B_ADC_TEMP - Die temperature
* LTM3360B_ADC_VOUT_P - Positive output voltage
* LTM3360B_ADC_VOUT_N - Negative output voltage
* LTM3360B_ADC_VIN - Input voltage

Register Access
---------------

Direct register access is available through **ltm3360b_reg_read** and
**ltm3360b_reg_write** APIs for advanced configuration and debugging.

LTM3360B Driver Initialization Example
--------------------------------------

.. code-block:: c

	struct ltm3360b_dev *ltm3360b_dev;
	uint8_t vout_raw;
	struct no_os_i2c_init_param ltm3360b_i2c_ip = {
		.device_id = I2C_DEVICE_ID,
		.max_speed_hz = 100000,
		.platform_ops = I2C_OPS,
		.slave_address = LTM3360B_I2C_ADDR,
	};

	struct ltm3360b_init_param ltm3360b_ip = {
		.i2c_init = &ltm3360b_i2c_ip,
		.auto_enable = true,
		.vout_mv = 750,
		.imon_threshold = LTM3360B_IMON_TH_79_PERCENT,
	};

	ret = ltm3360b_init(&ltm3360b_dev, &ltm3360b_ip);
	if (ret) {
		pr_err("Failed to initialize LTM3360B device: %d\n", ret);
		goto error;
	}

	ret = ltm3360b_adc_enable(ltm3360b_dev, true);
	if (ret) {
		pr_err("Failed to enable LTM3360B ADC: %d\n", ret);
		goto error;
	}

	ret = ltm3360b_adc_read_raw(ltm3360b_dev, LTM3360B_ADC_VOUT_P, &vout_raw);
	if (ret) {
		pr_err("Failed to read VOUT_P ADC channel: %d\n", ret);
		goto error;
	}

	pr_info("LTM3360B initialized successfully\n");
	pr_info("VOUT_P raw ADC value: %d\n", vout_raw);

	return 0;

	error:
	if (ltm3360b_dev)
		ltm3360b_remove(ltm3360b_dev);
	return ret;