ADMT4000 no-OS driver
=====================

.. no-os-doxygen::

Supported Devices
-----------------

- :adi:`ADMT4000`

Overview
--------

The ADMT4000 is a magnetic turn count sensor capable of recording the number of
rotations of a magnetic system even while the device is powered down. On power-up,
the device can be interrogated to report the absolute position of the system. The
absolute position is reported through a serial-peripheral interface (SPI).

The ADMT4000 counts up to 46-turns of an external magnetic field, which increments
the absolute position in the clockwise (CW) direction. The device includes three
magnetic sensors, a giant magneto resistance (GMR) turn count sensor, which is used
to count the number of rotations on the system, a GMR quadrant detecting sensor,
and an anisotropic magnetoresistance (AMR) angle sensor. The AMR angle sensor is
used in combination with a GMR quadrant detecting sensor to determine the absolute
position of the system within 360°. Combining the GMR turn count sensor output with
the AMR angle sensor output enables the device to report the position of the system
with a high degree of angular accuracy.

Applications
------------
* Rotation count detection and storage without power
* Contactless absolute position measurement
* Brushless DC motor control and positioning
* Actuator control and positioning

ADMT4000 Device Configuration
-----------------------------

Driver Initialization
---------------------

The ADMT4000 driver requires SPI communication and optional GPIO pins for reset
and ACALC monitoring. The driver automatically configures the device with ECC
enabled and performs initial calibration during initialization.

ADMT4000 Driver Initialization Example
--------------------------------------

.. code-block:: c

	struct admt4000_dev *admt4000_dev;
	const struct no_os_spi_init_param admt_spi_ip = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = SPI_BAUDRATE,
		.chip_select = SPI_CS,
		.mode = NO_OS_SPI_MODE_0,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.platform_ops = SPI_OPS,
		.extra = SPI_EXTRA,
	};
	struct no_os_gpio_init_param gpio_reset_ip = {
		.port = GPIO_RESET_PORT,
		.number = GPIO_RESET_PIN,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA,
	};
	struct no_os_gpio_init_param gpio_acalc_ip = {
		.port = GPIO_ACALC_PORT,
		.number = GPIO_ACALC_PIN,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA,
	};
	struct admt4000_init_param admt4000_ip = {
		.spi_init_param = admt_spi_ip,
		.gpio_reset_ip = gpio_reset_ip,
		.gpio_acalc_ip = gpio_acalc_ip,  /* Optional: For ACALC monitoring */
		.admt4000_gpio_ip = {
			{.alt_mode = false, .output_en = false}, /* GPIO0: BUSY */
			{.alt_mode = false, .output_en = false}, /* GPIO1: CNV */
			{.alt_mode = false, .output_en = false}, /* GPIO2: Reserved */
			{.alt_mode = true,  .output_en = false}, /* GPIO3: ACALC */
			{.alt_mode = true,  .output_en = true},  /* GPIO4: FAULT */
			{.alt_mode = false, .output_en = false}, /* GPIO5: BOOTLOAD */
		},
	};
	ret = admt4000_init(&admt4000_dev, &admt4000_ip);
	if (ret)
		goto exit;

Register Access
---------------

The driver provides direct register access functions with built-in CRC validation.
The device has two register pages:

* **Page 0**: Measurement registers (angle, turns, sine, cosine, radius, temperature)
* **Page 2**: Configuration registers (harmonics, GPIO, thresholds, etc.)

**Basic Register Operations:**

.. code-block:: c

	uint16_t value;
	uint8_t verif;

	/* Read register with CRC validation (pass NULL for verif if not needed) */
	ret = admt4000_reg_read(admt4000_dev, ADMT4000_AGP_REG_ANGLE, &value, &verif);

	/* Write register */
	ret = admt4000_reg_write(admt4000_dev, ADMT4000_02_REG_GENERAL, value);

	/* Update specific bits in register */
	ret = admt4000_reg_update(admt4000_dev, ADMT4000_02_REG_GENERAL,
	                          ADMT4000_ANGLE_FILT_EN_MASK,
	                          NO_OS_BIT(12));

**Note:** Page switching is handled automatically by the driver API functions.
Users don't need to manually manage register pages when using the APIs.

Configuration API
-----------------

Device Configuration
~~~~~~~~~~~~~~~~~~~~

The driver provides high-level functions to configure device operation:

.. code-block:: c

	/* Set conversion mode (one-shot or continuous) */
	ret = admt4000_set_conv_mode(admt4000_dev, true);  /* true for one-shot */

	/* Configure conversion synchronization */
	ret = admt4000_set_conv_sync_mode(admt4000_dev, ADMT4000_CNV_REG_CONT);

	/* Enable angle filter */
	ret = admt4000_set_angle_filt(admt4000_dev, true);

	/* Configure GPIO functions:  GPIO3 as ACALC alternate function */
	ret = admt4000_gpio_config(admt4000_dev, 3, true, false, false);


Harmonic Calibration
~~~~~~~~~~~~~~~~~~~~

The ADMT4000 supports harmonic calibration to improve measurement accuracy.
Harmonics H1, H2, H3, and H8 can be configured with magnitude and phase values:

.. code-block:: c

	/* Batch operations for all harmonics (more efficient) */
	struct admt4000_harmonics_calib harmonics;

	/* Configure all harmonics at once */
	harmonics.h1 = (struct admt4000_harmonic_coeff){.magnitude_raw = 1024, .phase_raw = 512};
	harmonics.h2 = (struct admt4000_harmonic_coeff){.magnitude_raw = 512,  .phase_raw = 256};
	harmonics.h3 = (struct admt4000_harmonic_coeff){.magnitude_raw = 256,  .phase_raw = 128};
	harmonics.h8 = (struct admt4000_harmonic_coeff){.magnitude_raw = 128,  .phase_raw = 64};
	ret = admt4000_set_harmonics_calib(admt4000_dev, &harmonics);

	/* Select harmonic correction source */
	ret = admt4000_set_h8_ctrl(admt4000_dev, ADMT4000_H8_CTRL_USER);  /* Use user values */

**Note:** Harmonic calibration requires disabling and re-enabling ECC. The batch
functions handle this automatically for optimal performance.

Conversion and Data Acquisition
--------------------------------

Reading Position Data
~~~~~~~~~~~~~~~~~~~~~

The device provides several methods to read position and sensor data:

.. code-block:: c

	/* Read turns and angle in one operation (most efficient) */
	uint8_t turns;
	uint16_t angles[2];  /* [0] = 10-bit absolute angle, [1] = 12-bit incremental angle */
	ret = admt4000_get_raw_turns_and_angle(admt4000_dev, &turns, angles);

	/* Convert turns to signed quarter-turn count */
	int16_t qturn_count;
	
	ret = admt4000_quarter_turns_cnt(turns, &qturn_count);

	/* Calculate full turns from quarter turns */
	float full_turns = (float)qturn_count / 4.0;

	/* Convert raw angle to degrees */
	float angle_degrees;
	ret = admt4000_angle_raw_to_scaled(angles[1], &angle_degrees);

	/* Example: 11.25 turns at 110 degrees */
	/* qturn_count = 45 */
	/* full_turns = 11.25 */
	/* angle_degrees = 110.0 */

	/* Read individual measurements */
	int16_t sine_val, cosine_val;
	uint16_t radius_val, temp_val;
	bool is_new_data;

	ret = admt4000_get_sin(admt4000_dev, &sine_val, &is_new_data);
	ret = admt4000_get_cos(admt4000_dev, &cosine_val, &is_new_data);
	ret = admt4000_get_radius(admt4000_dev, &radius_val, &is_new_data);
	ret = admt4000_get_temp(admt4000_dev, &temp_val);

	/* Read scaled/converted values */
	float radius_mvv;
	float temp_scaled;
	ret = admt4000_radius_raw_to_scaled(raw_radius, &radius_mvv);  /* mV/V */
	ret = admt4000_temp_raw_to_scaled(raw_temp, &temp_scaled);  /* Degrees Celsius */

Conversion Triggering
~~~~~~~~~~~~~~~~~~~~~

In one-shot mode, conversions must be triggered manually:

.. code-block:: c

	/* Configure for one-shot mode */
	ret = admt4000_set_conv_mode(admt4000_dev, true);

	/* Trigger a conversion */
	ret = admt4000_toggle_cnv(admt4000_dev);

	/* Wait for conversion to complete (typically 2-3ms) */
	no_os_mdelay(3);

	/* Read the results */
	ret = admt4000_get_raw_turns_and_angle(admt4000_dev, &turns, angles);

In continuous mode, the device automatically updates measurements at regular intervals.

Fault Monitoring
~~~~~~~~~~~~~~~~

The driver provides fault detection and clearing capabilities:

.. code-block:: c

	/* Read all fault status flags */
	uint16_t faults;
	ret = admt4000_get_faults(admt4000_dev, &faults);

	/* Clear all faults */
	ret = admt4000_clear_all_faults(admt4000_dev);

Device Reset and Reinitialization
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

	/* Hardware reset using GPIO pin */
	ret = admt4000_hard_reset(admt4000_dev);

	/* Reinitialize with stored configuration */
	ret = admt4000_reinitialize(admt4000_dev);



ADMT4000 no-OS IIO support
--------------------------
The ADMT4000 IIO driver comes on top of the ADMT4000 driver and offers support
for interfacing IIO clients through libiio.

ADMT4000 IIO Device Configuration
----------------------------------
Attributes
----------

* ``conv_sync_mode`` - Configure conversion synchronization mode
* ``conv_sync_mode_available`` - List available conversion sync modes
* ``angle_filter_enable`` - Enable/disable angle sensor filter
* ``conversion_mode`` - Set/read conversion mode (one-shot or continuous)
* ``h8_corr_src`` - Set/read harmonic correction source (H8 or User)
* ``h8_corr_src_available`` - List available harmonic correction sources
* ``harmonics`` - Read/write all harmonic calibration values (space-separated)
  Format: ``"mag1 phase1 mag2 phase2 mag3 phase3 mag8 phase8"``
  Example: ``"1024 2048 512 1024 64 1024 32 512"``
* ``harmonics_magnitude_scale`` - Read harmonic magnitude scaling factors
* ``harmonics_phase_scale`` - Read harmonic phase scaling factors
* ``faults`` - Read device fault status

Channel Attributes
~~~~~~~~~~~~~~~~~~

* ``turns`` channel: ``raw``, ``scale``
* ``angle`` channel: ``raw``, ``scale``
* ``temp`` channel: ``raw``, ``scale``, ``offset``
* ``sine`` channel: ``raw``
* ``cosine`` channel: ``raw``
* ``radius`` channel: ``raw``, ``scale`` (voltage in mV)

Debug Attributes
~~~~~~~~~~~~~~~~

* ``reset`` - Trigger a software reset of the device

ADMT4000 IIO Driver Initialization Example
------------------------------------------

.. code-block:: c

	int ret;

	struct admt4000_iio_dev *admt4000_iio_desc;

	struct admt4000_iio_dev_init_param admt4000_iio_ip = {
		.admt4000_init_param = &admt4000_ip,
	};

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	ret = admt4000_iio_init(&admt4000_iio_desc, &admt4000_iio_ip);
	if (ret)
		goto exit;

	struct iio_app_device iio_devices[] = {
		{
			.name = "admt4000",
			.dev = admt4000_iio_desc,
			.dev_descriptor = admt4000_iio_desc->iio_dev,
		},
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto iio_admt4000_remove;

	ret = iio_app_run(app);

	iio_app_remove(app);
