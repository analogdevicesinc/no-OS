ADE7816 no-OS documentation
===========================

Supported Devices
-----------------

`ADE7816 <https://www.analog.com/ADE7816>`

Overview
--------

The ADE7816 is a highly accurate, multichannel metering
device that is capable of measuring one voltage channel and up
to six current channels. It measures line voltage and current and
calculates active and reactive energy, as well as instantaneous rms
voltage and current. The device incorporates seven sigma-delta
(Σ-Δ) ADCs with a high accuracy energy measurement core.
The six current input channels allow multiple loads to be measured
simultaneously. The voltage channel and the six current channels
each have a complete signal path allowing for a full range of
measurements. Each input channel supports a flexible gain stage
and is suitable for use with current transformers (CTs). Six on-
chip digital integrators facilitate the use of the Rogowski coil
sensors.

ADE7816 Device Configuration
----------------------------

Both SPI or I2C communication protocols are supported but only one can be used
and it is to be set at the device initialization.

The first API to be called is **ade7816_init**. Make sure that it returns 0,
which means that the driver was initialized correctly.

Active and Reactive Energy Measurements
---------------------------------------

Bot active and reactive energy can be read from their registers by using either
**ade7816_read_active_energy** or **ade7816_read_reactive_energy** APIs, more
to this active and reactive energy threshold values can be set depending on the
desired update rate.

Line-Cycle mode and no-load conditions can be set by using
**ade7816_set_lcycle_mode** API and **ade7816_set_no_load** API.

Root Mean Square Measurement
----------------------------

The RMS value for any channel can be read using **ade7816_read_rms** API.

Calibration
-----------

Gain, offset, phase and rms can be calibrated by using the following APIs:
**ade7816_set_gain**, **ade7816_set_offset**, **ade7816_set_phase** and
**ade7816_calib_rms**.

Group Selection
---------------

Channels can be grouped in thirds (A,B,C or D,E,F) by using
**ade7816_group_sel** API, more to this zero-crossing, peak detection, power
direction and angle measurements are available by using the following APIs:
**ade7816_zx_detect**, **ade7816_peak_detect**, **ade7816_power_dir** and
**ade7816_angle_meas**.

Interrupt Configuration
-----------------------

Each interrupt available on the IRQ0 and IRQ1 pins depending on the STATUS0
and STATUS1 registers can be configured using **ade7816_set_interrupt** API.

Default interrupt handlers are provided within the driver in case user doesn't
assign a callback to the initialization parameter referring to the specific IRQ
pin.

Priorities for the interrupt handlers can also be configured by changing the
priority variable in the initialization parameter structure before init.

For a more detailed description about interrupt priorities see :
`NoOS Interrupt API <https://wiki.analog.com/resources/no-os/drivers/interrupt>`

RMS Scaling
-----------

RMS value can be scaled in microunits by using **ade7816_rms_to_micro** API.

ADE7816 Driver Initialization Example
--------------------------------------

.. code-block:: bash

	struct ade7816 *ade7816;
	struct no_os_spi_init_param spi_ip = {
		.device_id = 0,
		.extra = &ade7816_spi_extra,
		.max_speed_hz = 100000,
		.platform_ops = &max_spi_ops,
		.chip_select = 0,
	};
	struct ade7816_init_param max22196_ip = {
		.spi_param = &ade7816_spi_ip,
		.i2c_param = NULL,
		.reset_param = &ade7816_reset_ip,
		.ss_param = NULL,
		.gpio_irq0_param = &ade7816_gpio_irq0_ip,
		.gpio_irq1_param = NULL,
		.active_irq = ADE7816_IRQ0,
		.irq0_callback = NULL,
		.comm_type = ADE7816_SPI,
		.handle_irq0 = MXC_GPIO_GET_GPIO(2),
		.irq0_priority = 1,
	};

	/** GPIO Pin Interrupt Controller */
	struct no_os_irq_ctrl_desc *gpio_irq_desc;
	struct no_os_irq_init_param gpio_irq_desc_param = {
		.irq_ctrl_id = GPIO_IRQ_ID,
		.platform_ops = GPIO_IRQ_OPS,
		.extra = GPIO_IRQ_EXTRA
	};

	ret = no_os_irq_ctrl_init(&gpio_irq_desc, &gpio_irq_desc_param);
	if (ret)
		goto exit;

	ade7816_ip.irq_ctrl = gpio_irq_desc;

	ret = ade7816_init(&ade7816, &ade7816_ip);
	if (ret)
		goto error;
