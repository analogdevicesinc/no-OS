AD4692 no-OS Driver
===================

Supported Devices
-----------------

`AD4692 <https://www.analog.com/AD4692>`
`AD4691 <https://www.analog.com/AD4691>`
`AD4694 <https://www.analog.com/AD4694>`
`AD4693 <https://www.analog.com/AD4693>`

Overview
--------

The AD4692 family are compact, Easy Drive, 16-bit MUX SAR ADCs incorporating
accumulators per-channel and a flexible channel sequencer to enable a diverse
range of space-constrained precision measurement applications.

The AD4692 family combines the analog core functions of the AD4699 family
(e.g. pre-charge buffers on MUX and REF inputs) with expanded digital
processing capabilities and a microcontroller-friendly interface to better
fit into low bandwidth, high channel count control loops, e.g. optical
transceiver modules, etc.

The AD4692 family also includes new ADC operating modes and an internal
oscillator that require fewer microcontroller resources to manage convert-start
timing and data readback. An autonomous sampling mode allows the controller SPI
to initiate and terminate conversions through software with minimal digital I/O
lines. With the CNV and SPI burst modes, the controller can trigger a
pre-determined number of samples via hardware (CNV) or software (SPI),
and the AD4692 will automatically cease sampling based off user programmable
conditions.

All register accesses use address-descending SPI framing: the 16-bit register
address is split across the first two bytes (MSB first), followed by the
register data byte, with the read/write direction bit embedded in the address.

AD4692 Device Configuration
---------------------------

Driver Initialization
---------------------

In order to be able to use the device, you will have to provide the support for
the communication protocol (SPI) as well as a GPIO for reset and, optionally,
GPIOs for the four configurable GPIO pins. A PWM descriptor is required for
operating modes that use an external CNV signal (CNV Clock and CNV Burst modes).

The first API to be called is **ad4692_init**. Make sure that it returns 0,
which means that the driver was initialized correctly.

Operating Modes
---------------

The driver supports five operating modes selected via the ``mode`` field in
``ad4692_init_param``:

* ``AD4692_CNV_CLOCK`` — The external CNV signal drives convert-start. The
  sampling frequency equals ``1 / PWM_period``. The ``conv_param`` PWM
  descriptor is required.

* ``AD4692_CNV_BURST`` — The external CNV triggers a burst of N conversions
  (one per active channel) at the internal oscillator rate. After the burst
  completes the device stops automatically. The sampling frequency in this mode
  is the internal oscillator frequency. The ``conv_param`` PWM descriptor is
  required; the driver automatically computes the minimum safe CNV period via
  **ad4692_cnv_burst_period_ns** and programs it whenever the active channel
  set changes.

* ``AD4692_AUTONOMOUS`` — The internal oscillator drives convert-start with no
  external CNV signal required. The OSC register directly sets the sampling
  frequency. No PWM descriptor is needed.

* ``AD4692_SPI_BURST`` — A software command initiates a burst of conversions at
  the internal oscillator rate. The OSC register directly sets the sampling
  frequency. No PWM descriptor is needed.

* ``AD4692_MANUAL_MODE`` — The controller fully controls conversion timing
  through the SPI command interface. Each 24-bit SPI transaction simultaneously
  triggers a conversion for the next channel and returns the result of the
  previous one; therefore N+1 transfers are required per N-channel scan. No
  PWM descriptor is needed.

CNV Burst Period Utility
------------------------

The public function **ad4692_cnv_burst_period_ns** computes the minimum CNV
burst period required to safely complete all conversions and SPI readback for a
given number of active channels:

.. code-block:: c

	uint32_t period_ns = ad4692_cnv_burst_period_ns(ad4692_desc, n_active);

The computation accounts for the total conversion time at the current OSC
frequency plus the SPI readback time at the current ``max_speed_hz``, with a
3/2 safety margin on the SPI portion. This is used internally by the IIO driver
in CNV Burst mode to auto-program the PWM period whenever the active channel
set is updated.

Channel Configuration
---------------------

In order to read channel data, **ad4692_get_ch** API has to be called, also
the channel sequencer, voltage reference, and sampling frequency of the internal
oscillator can be changed using their specific APIs: **ad4692_std_seq_ch**,
**ad4692_set_ref**, **ad4692_get_ref**, **ad4692_get_osc** and
**ad4692_set_osc**.

PWM Configuration
-----------------

An external PWM connected to the CNV pin is required for CNV Clock and CNV
Burst modes. The PWM descriptor is provided through ``conv_param`` in the
initialization parameter and is configured by the driver during
**ad4692_init**. The PWM can also be manually enabled or disabled using
**ad4692_sampling_enable**.

In CNV Burst mode, the CNV period is a derived quantity that depends on the
number of active channels and the internal oscillator frequency. Do not
treat the PWM period as the sampling period in this mode.

Reset
-----

Two reset paths are available:

* **ad4692_hardware_reset** — Toggles the RESET GPIO low to perform a full
  hardware reset of all registers to their default values.

* **ad4692_software_reset** — Writes the software reset pattern to
  ``INTERFACE_CONFIG_A`` to reset all registers through the SPI interface.

AD4692 Driver Initialization Example
-------------------------------------

.. code-block:: c

	struct ad4692_desc *ad4692_desc;

	struct no_os_spi_init_param ad4692_spi_ip = {
		.device_id = 4,
		.extra = &ad4692_spi_extra,
		.max_speed_hz = 2000000,
		.platform_ops = &max_spi_ops,
		.chip_select = 0,
		.mode = NO_OS_SPI_MODE_3,
	};

	struct no_os_gpio_init_param ad4692_gpio0_ip = {
		.port = 3,
		.pull = NO_OS_PULL_NONE,
		.number = 0,
		.platform_ops = &max_gpio_ops,
		.extra = &ad4692_gpio_extra_ip,
	};

	struct no_os_gpio_init_param ad4692_reset_ip = {
		.port = 1,
		.pull = NO_OS_PULL_NONE,
		.number = 0,
		.platform_ops = &max_gpio_ops,
		.extra = &ad4692_gpio_extra_ip,
	};

	struct no_os_pwm_init_param ad4692_pwm_ip = {
		.id = 2,
		.polarity = NO_OS_PWM_POLARITY_LOW,
		.platform_ops = &max_pwm_ops,
		.period_ns = 28000,
		.duty_cycle_ns = 210,
		.extra = &ad4692_pwm_extra_ip,
	};

	struct ad4692_init_param ad4692_ip = {
		.comm_param = &ad4692_spi_ip,
		.conv_param = &ad4692_pwm_ip,
		.gpio0_param = &ad4692_gpio0_ip,
		.reset_param = &ad4692_reset_ip,
		.id = ID_AD4692,
		.mode = AD4692_MANUAL_MODE,
		.vref = 5000000,
	};

	ret = ad4692_init(&ad4692_desc, &ad4692_ip);
	if (ret)
		goto remove_irq;

AD4692 no-OS IIO Support
------------------------

The AD4692 IIO driver comes on top of the AD4692 driver and offers support
for interfacing IIO clients through IIO lib.

AD4692 IIO Device Configuration
---------------------------------

Channel Setup
-------------

The IIO driver selects the appropriate channel array at initialization time
via the internal **ad4692_iio_setup_channels** function based on the operating
mode and device variant:

* Non-manual modes (CNV Clock, CNV Burst, Autonomous, SPI Burst): channels
  address the ``AVG_IN`` registers (``AD4692_AVG_IN_REG(n)``), which hold the
  16-bit averaged accumulator output for each channel.

* Manual mode: channels address the per-channel SPI command values
  (``AD4692_IN_COMMAND(n)``). Each 24-bit SPI frame returns the previous
  conversion result in bits [23:8].

AD4692 and AD4691 (16-channel variants) use 16-entry channel arrays.
AD4693 and AD4694 (8-channel variants) use 8-entry channel arrays.

Scan Types
----------

Two scan types are defined:

* Non-manual scan type: ``storagebits = 32``, ``realbits = 16``, ``shift = 0``,
  big-endian. The 16-bit averaged result is stored in the upper halfword of a
  32-bit container.

* Manual scan type: ``storagebits = 32``, ``realbits = 16``, ``shift = 8``,
  big-endian. The raw 24-bit SPI frame is stored as-is; the ADC data occupies
  bits [23:8], so a right-shift of 8 recovers the 16-bit sample.

Device Attributes
-----------------

Channel Attributes
------------------

All device channels expose four attributes:

* ``raw`` — The converted raw data from the ADC (single-channel, immediate
  read via **ad4692_get_ch**).

* ``scale`` — The full-scale voltage in millivolts divided by 2^16:
  ``scale = vref_mV / 65536``.

* ``sampling_frequency`` — The effective sampling rate of the device.
  The meaning depends on the operating mode:

  - ``AD4692_CNV_CLOCK``: ``1 / PWM_period`` (set by programming the PWM
    period; must not exceed ``max_rate``).
  - ``AD4692_CNV_BURST``, ``AD4692_AUTONOMOUS``, ``AD4692_SPI_BURST``:
    the internal oscillator frequency. Must be one of the discrete values
    listed in ``sampling_frequency_available``.
  - ``AD4692_MANUAL_MODE``: read-only; computed as
    ``SPI_rate / (24 * (N_active + 1))``. Cannot be set through this attribute.

* ``sampling_frequency_available`` — Space-separated list of the 16 discrete
  internal oscillator frequencies (in Hz) supported by the OSC register.
  Applies to CNV Burst, Autonomous, and SPI Burst modes.

Buffer Operation
----------------

In non-manual modes, the IIO trigger handler (**ad4692_trigger_handler**) is
registered and used when a DATA_READY GPIO interrupt trigger is available. It
stops sampling, reads all active channels from the ``AVG_IN`` registers, resets
the device state, and re-enables sampling for the next scan.

In all modes, **ad4692_iio_submit_buffer** is available for software-driven
data capture without a hardware trigger. In manual mode, it performs N+1 SPI
transfers per channel scan set to account for the pipelined conversion/readback
behavior. In non-manual modes, it waits for one accumulation period per
channel, reads the results, and resets for the next iteration.

In CNV Burst mode, the CNV PWM period is automatically updated to the minimum
safe value computed by **ad4692_cnv_burst_period_ns** each time the active
channel mask changes (``pre_enable``). This ensures the burst completes before
the next CNV rising edge regardless of the oscillator frequency or number of
active channels.

AD4692 IIO Driver Initialization Example
------------------------------------------

.. code-block:: c

	int ret;

	struct ad4692_iio_desc *ad4692_iio_desc;
	struct ad4692_iio_desc_init_param ad4692_iio_ip = {
		.ad4692_init_param = &ad4692_ip,
	};

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	ret = ad4692_iio_init(&ad4692_iio_desc, &ad4692_iio_ip);
	if (ret)
		goto error;

	struct iio_app_device iio_devices[] = {
		{
			.name = "ad4692",
			.dev = ad4692_iio_desc,
			.dev_descriptor = ad4692_iio_desc->iio_dev,
		},
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = ad4692_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto app_error;

	return iio_app_run(app);
