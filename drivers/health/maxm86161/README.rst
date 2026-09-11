MAXM86161 no-OS driver
======================

.. no-os-doxygen::

Supported Devices
-----------------

- :adi:`MAXM86161`

Overview
--------

The MAXM86161 is a complete, integrated optical data-acquisition system
(PPG analog front-end) designed for optical pulse-oximetry (SpO2) and
heart-rate monitoring in wearable and clinical form factors. It combines a
low-noise photodiode readout channel, a 19-bit sigma-delta ADC, three
programmable LED drivers (green, red and IR) and a 128-sample FIFO in a single
package, communicating over a standard I2C interface at the fixed address
``0x62``.

Each measurement is captured under a programmable LED-exposure sequence of up
to six slots, letting the host interleave green, red, IR, pilot and ambient
(direct-ambient) samples in a single acquisition frame. Per-LED drive current
is set through an 8-bit pulse-amplitude code combined with one of four
full-scale current ranges (up to 124 mA). On the receive side the PPG channel
offers selectable ADC full-scale ranges (4 to 32 uA), four integration times
(14.8 to 117.3 us), programmable sample averaging, ambient-light cancellation
(ALC) and a selectable digital filter (CDM/FDM).

On-chip features include an ambient-light cancellation loop, a photodiode-bias
selector, a proximity-detection mode with a programmable pilot LED and entry
threshold, a picket-fence detect-and-replace filter, a die-temperature sensor
(0.0625 degC LSB), an offset-DAC calibration engine, a SHA-256 authentication
block and an INTB interrupt output that flags FIFO almost-full, data-ready,
ALC-overflow, proximity, LED-compliance, die-temperature-ready and power-ready
events.

Applications
------------

* Wearable heart-rate monitors
* Pulse oximetry (SpO2)
* Fitness and activity trackers
* Clinical and remote patient monitoring
* Optical vital-signs sensing

MAXM86161 Device Configuration
------------------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~~~

In order to be able to use the device, you will have to provide the support for
the communication protocol (I2C), and optionally a GPIO plus IRQ controller for
the INTB interrupt line.

The first API to be called is **maxm86161_init**. Make sure that it returns 0,
which means that the driver was initialized correctly. It verifies the part ID,
performs a software reset, places the device into shutdown and single-PPG mode,
clears the interrupt status and, when the corresponding init parameters are
supplied, configures the INTB GPIO and registers a falling-edge interrupt
callback. Resources are released with **maxm86161_remove**.

Register Access
~~~~~~~~~~~~~~~

Direct register access is available through **maxm86161_reg_read**,
**maxm86161_reg_write** and **maxm86161_reg_update_bits** for single-register
operations, and **maxm86161_burst_reg_read** / **maxm86161_burst_reg_write** for
multi-byte transfers. The part identity and revision can be re-checked with
**maxm86161_verify_id**, and a software reset issued with **maxm86161_reset**.

Power Management
~~~~~~~~~~~~~~~~

The device is put into and taken out of shutdown with **maxm86161_shutdown**
and **maxm86161_set_shutdown**. Low-power mode (recommended for sample rates of
256 sps and below) is controlled with **maxm86161_set_low_power_mode**.

PPG Configuration
~~~~~~~~~~~~~~~~~

The PPG measurement front-end is configured through paired get/set helpers:
**maxm86161_set_sample_rate** / **maxm86161_get_sample_rate**,
**maxm86161_set_integration_time** / **maxm86161_get_integration_time**,
**maxm86161_set_adc_range** / **maxm86161_get_adc_range**,
**maxm86161_set_sample_averaging** / **maxm86161_get_sample_averaging**,
**maxm86161_set_alc_disable** / **maxm86161_get_alc_disable**,
**maxm86161_set_add_offset** / **maxm86161_get_add_offset**,
**maxm86161_set_led_settling** / **maxm86161_get_led_settling**,
**maxm86161_set_digital_filter** / **maxm86161_get_digital_filter** and
**maxm86161_set_pd_bias** / **maxm86161_get_pd_bias**.

LED Sequence and Drive
~~~~~~~~~~~~~~~~~~~~~~~

The measurement sequence of up to six slots is programmed one slot at a time
with **maxm86161_set_led_sequence** / **maxm86161_get_led_sequence**. A single
LED slot can be configured in one call (range, sequence slot and pulse
amplitude) with **maxm86161_set_led_cfg**. Per-LED drive current is set with
**maxm86161_set_led_pulse_amplitude** / **maxm86161_get_led_pulse_amplitude**
(the pilot LED is addressed by passing **MAXM86161_LED_NUM_PILOT_GREEN** as the
LED number), while the full-scale current range is selected with
**maxm86161_set_led_range** / **maxm86161_get_led_range**.

The high-resolution offset DAC per sequence slot is controlled with
**maxm86161_set_hires_dac** / **maxm86161_get_hires_dac**.

FIFO Management
~~~~~~~~~~~~~~~

The 128-sample FIFO is configured with **maxm86161_set_fifo_watermark** /
**maxm86161_get_fifo_watermark**, **maxm86161_set_fifo_rollover** /
**maxm86161_get_fifo_rollover**, **maxm86161_set_fifo_a_full_type** /
**maxm86161_get_fifo_a_full_type** and **maxm86161_set_fifo_stat_clr**. The
FIFO can be emptied with **maxm86161_fifo_flush**, and its fill level and
overflow counter read with **maxm86161_get_fifo_count** and
**maxm86161_fifo_overflow_check**. Sample data is read with
**maxm86161_read_fifo_data** and decoded (into a 5-bit tag plus the measurement
value) with **maxm86161_decode_fifo_sample**.

Interrupts
~~~~~~~~~~

The combined interrupt status is read with **maxm86161_get_interrupt_status**
and acknowledged with **maxm86161_clear_interrupt_status**. Per-source callbacks
are registered and enabled/disabled with **maxm86161_set_irq_handler**, and
**maxm86161_irq_handler** is the top-level dispatcher intended to be registered
as the INTB GPIO interrupt callback.

Proximity and Picket Fence
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Proximity-mode entry threshold is set with **maxm86161_set_prox_threshold** /
**maxm86161_get_prox_threshold**. The picket-fence detect-and-replace filter is
configured one field at a time with the per-bitfield
helpers **maxm86161_set_pf_enable** / **maxm86161_get_pf_enable**,
**maxm86161_set_pf_order** / **maxm86161_get_pf_order**,
**maxm86161_set_pf_iir_tc** / **maxm86161_get_pf_iir_tc**,
**maxm86161_set_pf_iir_init** / **maxm86161_get_pf_iir_init** and
**maxm86161_set_pf_threshold_sigma** / **maxm86161_get_pf_threshold_sigma**.

Die Temperature and Calibration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The on-chip temperature sensor is read (in micro-degrees Celsius) with
**maxm86161_read_die_temperature**. Offset-DAC calibration is started with
**maxm86161_start_calibration** and polled with
**maxm86161_get_calibration_status**.

Burst and Sync
~~~~~~~~~~~~~~

Burst sampling mode and rate are configured with **maxm86161_set_burst_mode**.
PPG synchronization and sample tagging are controlled with
**maxm86161_set_timestamp_enable**, **maxm86161_set_dac_code_tag**,
**maxm86161_set_gpio_ctrl** and **maxm86161_force_sync**.

SHA-256 Authentication and Memory
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The SHA-256 authentication engine is controlled with **maxm86161_sha_enable**,
**maxm86161_sha_start**, **maxm86161_sha_write_challenge** and
**maxm86161_sha_read_response**. Raw device memory can be accessed with
**maxm86161_memory_read** and **maxm86161_memory_write**, and the active memory
bank is selected with **maxm86161_memory_bank_set** /
**maxm86161_memory_bank_get**.

MAXM86161 Driver Initialization Example
---------------------------------------

.. code-block:: c

	struct maxm86161_dev *dev;

	struct no_os_uart_init_param uip = {
		.device_id = UART_DEVICE_ID,
		.baud_rate = UART_BAUDRATE,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.platform_ops = UART_OPS,
		.extra = UART_EXTRA,
	};

	struct no_os_gpio_init_param gpio_intb_ip = {
		.port = GPIO_INTB_PORT_NUM,
		.number = GPIO_INTB_PIN_NUM,
		.pull = NO_OS_PULL_UP,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_IRQ_EXTRA,
	};

	struct no_os_irq_init_param gpio_irq_ip = {
		.irq_ctrl_id = GPIO_IRQ_ID,
		.platform_ops = GPIO_IRQ_OPS,
		.extra = GPIO_IRQ_EXTRA,
	};

	struct maxm86161_init_param maxm86161_ip = {
		.i2c_init = {
			.device_id = I2C_DEVICE_ID,
			.max_speed_hz = 400000,
			.slave_address = MAXM86161_I2C_ADDR,
			.platform_ops = I2C_OPS,
			.extra = I2C_EXTRA,
		},
		.gpio_intb_init = &gpio_intb_ip,
		.irq_init = &gpio_irq_ip,
		.irq_handle = MAXM86161_GPIO_CB_HANDLE,
	};

	ret = maxm86161_init(&dev, &maxm86161_ip);
	if (ret)
		goto error;

MAXM86161 no-OS IIO support
---------------------------

The MAXM86161 IIO driver comes on top of the MAXM86161 driver and offers support
for interfacing IIO clients through libiio.

The IIO layer exposes a single unsigned voltage input channel that streams the
packed FIFO samples (5-bit tag in bits [23:19] and the 19-bit measurement in
bits [18:0], carried in a 32-bit scan element). The FIFO almost-full (A_FULL)
interrupt drives the INTB handler, which drains the device FIFO into a circular
buffer for consumption by libiio clients. An IRQ controller and an enabled IRQ
in the driver init parameters are mandatory for IIO operation.

MAXM86161 IIO Device Configuration
----------------------------------

Debug Attributes
~~~~~~~~~~~~~~~~

* ``sample_rate - PPG sample-rate selection code``
* ``integration_time - PPG ADC integration-time selection``
* ``adc_range - PPG ADC full-scale range selection``
* ``sample_averaging - number of samples averaged per FIFO data point``
* ``alc_disable - ambient-light cancellation disable flag``
* ``add_offset - ADC add-offset enable flag``
* ``led_settling - LED settling-time selection``
* ``dig_filter - PPG digital-filter selection (CDM/FDM)``
* ``fifo_watermark - FIFO almost-full watermark threshold``
* ``fifo_rollover - FIFO roll-over-on-full enable flag``
* ``fifo_a_full_type - FIFO almost-full interrupt assertion behavior``
* ``fifo_count - number of samples currently in the FIFO (read-only)``
* ``led_seq1..led_seq6 - LED/measurement source for each of the six sequence slots``
* ``led1_pa..led3_pa - pulse amplitude (drive current) of each LED``
* ``led_pilot_pa - pilot LED pulse amplitude used in proximity mode``
* ``led1_range..led3_range - full-scale current range of each LED driver``
* ``pd_bias - photodiode bias selection``
* ``prox_threshold - proximity-mode entry threshold``
* ``picket_fence_enable - picket-fence detect-and-replace enable flag``
* ``picket_fence_order - picket-fence ordering flag``
* ``picket_fence_iir_tc - picket-fence IIR time-constant selection``
* ``picket_fence_iir_init - picket-fence IIR initialization value``
* ``picket_fence_threshold_sigma - picket-fence threshold sigma multiplier``
* ``shutdown - device shutdown (power-down) state``
* ``low_power_mode - low-power-mode enable flag``
* ``burst_enable - burst-sampling enable flag``
* ``burst_rate - burst-sampling rate selection``
* ``part_id - device part ID (read-only)``
* ``rev_id - device revision ID (read-only)``
* ``interrupt_status - combined 16-bit interrupt status (read-only)``
* ``die_temperature - die temperature in micro-degrees Celsius (read-only)``
* ``buffer_enable - start/stop data capture (exit/enter shutdown and reset the buffer)``

MAXM86161 IIO Driver Initialization Example
-------------------------------------------

.. code-block:: c

	int ret;

	struct maxm86161_iio_desc *maxm86161_iio_dev;
	struct maxm86161_iio_init_param maxm86161_iio_ip = {
		.drv_init_param = &maxm86161_ip,
		.irq_ctrl = maxm86161_irq_ctrl,
		.irq_id = GPIO_IRQ_ID,
		.fifo_buf_size = 256,
		.fifo_watermark = 15,
	};

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = {0};

	ret = maxm86161_iio_init(&maxm86161_iio_dev, &maxm86161_iio_ip);
	if (ret)
		goto exit;

	struct iio_app_device iio_devices[] = {
		{
			.name = "maxm86161",
			.dev = maxm86161_iio_dev,
			.dev_descriptor = maxm86161_iio_dev->iio_dev,
		},
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = uip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto remove_iio_maxm86161;

	return iio_app_run(app);
