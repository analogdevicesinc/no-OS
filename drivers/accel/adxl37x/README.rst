ADXL37x no-OS Driver
====================

Supported Devices
-----------------

- :adi:`ADXL371`
- :adi:`ADXL372`

Overview
--------

The ADXL37x is an ultra low power, 3-axis, ±200 g accelerometer that consumes
28 µA at a 2560 Hz output data rate (ODR). The ADXL37x does not power
cycle its front end to achieve its low power operation and therefore
does not run the risk of aliasing the output of the sensor.

In addition to its ultra low power consumption, the ADXL37x enables
impact detection while providing system level power reduction. The device
includes a deep multimode output first in, first out (FIFO) queue,
several activity detection modes, and a method for capturing only the peak
acceleration of overthreshold events.

Applications
------------

* Impact and Shock Detection
* Asset Health Assessment
* Portable Internet of Things (IoT) edge nodes
* Concussion and head trauma detection

Operation Modes
---------------

+----------------------------+---------------------+---------------------+---------------------+
| Mode Name                  | Description         | Configuration Bits  | Typical Use Case    |
+----------------------------+---------------------+---------------------+---------------------+
| ADXL37X_STANDBY            | Standby mode for    | 0                   | When the device     |
|                            | lowest power        |                     | needs to conserve   |
|                            | consumption.        |                     | energy and data     |
|                            |                     |                     | acquisition is not  |
|                            |                     |                     | required            |
+----------------------------+---------------------+---------------------+---------------------+
| ADXL37X_WAKE_UP            | Low-power motion    | 1                   | Ideal for simple    |
|                            | detection at a set  |                     | detection of the    |
|                            | wake-up rate.       |                     | presence or absence |
|                            |                     |                     | of motion at an     |
|                            |                     |                     | extremely low power |
|                            |                     |                     | consumption         |
+----------------------------+---------------------+---------------------+---------------------+
| ADXL37X_INSTANT_ON         | Extremely low-power | 2                   | Capture impact      |
|                            | impact / shock      |                     | events and store    |
|                            | detection           |                     | peak acceleration   |
|                            |                     |                     | values at very low  |
|                            |                     |                     | power consumption   |
+----------------------------+---------------------+---------------------+---------------------+
| ADXL37X_FULL_BW_MEASUREMENT| Full-bandwidth      | 3                   | When full-bandwidth |
|                            | measurement mode    |                     | sampling is required|
|                            |                     |                     |                     |
|                            |                     |                     |                     |
|                            |                     |                     |                     |
+----------------------------+---------------------+---------------------+---------------------+

Device Configuration
--------------------

Initialization Functions
~~~~~~~~~~~~~~~~~~~~~~~~

The ``adxl37x_init`` function sets up ADXL37x accelerometer by
configuring the device for SPI or I2C communication, establishing
initial operational settings, and managing errors. The ``adxl37x_reset``
function also resets the device to power-on default settings.

Communication and Control Functions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ADXL37x driver facilitates communication and control through
functions like ``adxl37x_write_reg``, ``adxl37x_read_reg``, and
``adxl37x_write_mask`` for manipulating register values.
The ``adxl37x_set_op_mode`` and ``adxl37x_get_op_mode`` functions allow
switching between standby and measurement modes.

Switching between ADXL versions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To switch between supported devices, set the adxl_type field in the
adxl37x_init_param struct to the desired part (ADXL371 or ADXL372).
The driver automatically applies the correct per-device defaults from the
chip_info table in adxl37x.h, which captures the attributes that differ
between parts (ODR, bandwidth, wake-up rate, and filter settle time).

Data Processing Functions
~~~~~~~~~~~~~~~~~~~~~~~~~

To process data from ADXL37x accelerometers, first configure the FIFO to
the appropriate settings using the ``adxl37x_configure_fifo`` function.

The ``adxl37x_get_fifo_xyz_data_workaround`` function retrieves raw data from
the FIFO using the anomaly workaround described in the :adi:`ADXL371` and
:adi:`ADXL372` datasheets.
The raw samples are placed into a data structure representing 3-axis XYZ data.

The ``adxl37x_service_fifo_ev`` function is a wrapper around the
``adxl37x_get_fifo_xyz_data_workaround`` function to streamline the process of
servicing FIFO events.

Changing the Axes and the Number of Axes
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The set of axes stored in the FIFO is selected by ``fifo_config.fifo_format``.
The FIFO holds ``ADXL37X_FIFO_MAX_ENTRIES`` (512) entries of two bytes each,
and one entry is consumed per enabled axis per sample set, so the number of
axes also decides how many complete XYZ sample sets fit in the FIFO.

Three further fields have to stay coherent with the chosen format:

- ``bytes_per_sample`` — size of one sample set, ``2 * number_of_axes``.
  ``adxl37x_get_fifo_xyz_data_workaround`` uses it to size the burst read, and
  ``adxl37x_service_fifo_ev`` divides it by ``ADXL37X_BYTES_PER_AXIS`` to
  convert the FIFO_ENTRIES count into a sample-set count. A value of 0 makes
  both functions return ``-EINVAL``.
- ``fifo_samples`` — the watermark, expressed in 2-byte entries (0 to 512).
  Keep it a multiple of the number of axes so that a sample set is never split
  across the watermark.
- ``extracted_samples`` — the number of sample sets the application should
  request, one less than the maximum so that at least one set is left in the
  FIFO for sample integrity.

+---------------------------+------+------------------+-------------+-------------------+
| ``fifo_format``           | Axes | bytes_per_sample | Sample sets | extracted_samples |
+===========================+======+==================+=============+===================+
| ``ADXL37X_X_FIFO``,       | 1    | 2                | 512         | 511               |
| ``ADXL37X_Y_FIFO``,       |      |                  |             |                   |
| ``ADXL37X_Z_FIFO``        |      |                  |             |                   |
+---------------------------+------+------------------+-------------+-------------------+
| ``ADXL37X_XY_FIFO``,      | 2    | 4                | 256         | 255               |
| ``ADXL37X_XZ_FIFO``,      |      |                  |             |                   |
| ``ADXL37X_YZ_FIFO``       |      |                  |             |                   |
+---------------------------+------+------------------+-------------+-------------------+
| ``ADXL37X_XYZ_FIFO``      | 3    | 6                | 170         | 169               |
+---------------------------+------+------------------+-------------+-------------------+

Samples are always returned in ``struct adxl37x_xyz_accel_data``, whatever the
format is. The axes that are not part of the selected format are reported as 0.

``ADXL37X_XYZ_PEAK_FIFO`` is accepted by ``adxl37x_configure_fifo``, but it has
no de-interleaving strategy, so ``adxl37x_get_fifo_xyz_data_workaround`` returns
``-EINVAL`` for it.

Changing the Axes With the Base Driver
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The base driver never derives the axis count on its own, so the caller owns the
four fields above. They can be set once in ``adxl37x_init_param.fifo_config``,
or changed at runtime by writing ``dev->fifo_config.bytes_per_sample`` and
calling ``adxl37x_configure_fifo`` with the new format and watermark.

The eval-adxl37x project drives all of them from a single knob in
``projects/eval-adxl37x/src/common/common_data.h``:

.. code-block:: C

   #define AXES_USED 3
   #define BYTES_PER_SAMPLE (2 * AXES_USED)

   #if AXES_USED == 3
   #define MAX_SAMPLES 170
   #elif AXES_USED == 2
   #define MAX_SAMPLES 256
   #elif AXES_USED == 1
   #define MAX_SAMPLES 512
   #endif

   #define EXTRACTED_SAMPLES (MAX_SAMPLES - 1)

Changing ``AXES_USED`` there still leaves ``fifo_format`` in
``common_data.c`` to be updated to a format with the matching number of axes,
since the macro says how many axes are used but not which ones.

Changing the Axes With the IIO Driver
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Nothing has to be changed in the code. The axis selection arrives from the IIO
client as a channel mask (bit 0 = X, bit 1 = Y, bit 2 = Z), for example from
the channel checkboxes of IIO Oscilloscope.

``adxl37x_iio_update_channels``, the ``pre_enable`` hook, counts the enabled
axes and then derives everything from that count: it sets ``bytes_per_sample``
and ``extracted_samples``, translates the mask into a format with
``adxl37x_mask_to_fifo_format``, and reprograms the chip through
``adxl37x_configure_fifo``, restoring the operating mode that was active before
the reconfiguration. An empty mask is rejected with ``-EINVAL``.

Driver Initialization Example
------------------------------

.. code-block:: C

   #include "adxl37x.h"
   #include "no_os_i2c.h"
   #include "no_os_spi.h"
   #include "no_os_alloc.h"
   #include "no_os_delay.h"

   #define ACCEL_MAX_XYZ 170

   /* ADXL Device Instance */
   uint8_t status1=0, status2=0;
   uint16_t fifo_entries=0;

   int main(void)
   {
    	struct adxl37x_dev *adxl37x_desc = NULL;
    	struct adxl37x_xyz_accel_data accel_data[ACCEL_MAX_XYZ];
    	uint16_t sample_sets;
    	int ret;

   	/* SPI Initialization Parameters */
   	static struct no_os_spi_init_param spiInitParams = {
   		.device_id    = ACCEL_SPI,
   		.max_speed_hz = ACCEL_SPI_FREQ,     // 10 MHz max
   		.chip_select  = 0,                  // CS0
   		.mode         = NO_OS_SPI_MODE_0,   // Mode 0
   		.bit_order    = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
   		.platform_ops = &max_spi_ops,
   		.extra        = &max_spi_param,
   		.parent       = NULL
   	};

   	/* Initialize the device */
   	struct adxl37x_init_param adxl_ip = {
   		.adxl_type = ADXL371,
   		.spi_init = spiInitParams,
   		.gpio_int1 = int1Init,
   		.int1_config = {},
   		.gpio_int2 = int2Init,
   		.int2_config = {},
   		.comm_type =  ADXL37X_SPI,
   		.fifo_config = {
   			.fifo_format = ADXL37X_XYZ_FIFO,
   			.fifo_mode = ADXL37X_FIFO_STREAMED,
   			.bytes_per_sample = 6,
   			.extracted_samples = ACCEL_MAX_XYZ - 1,
   			.fifo_samples = ACCEL_MAX_XYZ
   		},
   		.op_mode = ADXL37X_FULL_BW_MEASUREMENT,
   		.act_proc_mode = ADXL37X_LOOPED
   	};
   	ret = adxl37x_init(&adxl37x_desc, adxl_ip);
   	if (ret)
   		goto cleanup;

   	ret = adxl37x_set_op_mode(adxl37x_desc, ADXL37X_STANDBY);
   	if (ret)
   		goto cleanup;

   	/** NOTE: Workaround for er001 requires internal sync to start */
   	adxl37x_set_internal_sync(adxl37x_desc);

   	adxl37x_set_op_mode(adxl37x_desc, ADXL37X_FULL_BW_MEASUREMENT);

   	/* Read # available FIFO entries into "fifo_entries" */
   	ret = adxl37x_get_status(adxl37x_desc, &status1, &status2, &fifo_entries);
   	if (ret)
   		goto cleanup;

   	/*
   	 * FIFO_ENTRIES counts 2-byte entries, one per axis. Convert to XYZ
   	 * sample sets before asking for data.
   	 */
   	sample_sets = fifo_entries / 3;

   	/* Read raw acceleration data */
   	ret = adxl37x_get_fifo_xyz_data_workaround(adxl37x_desc, accel_data,
   			sample_sets);
   	if (ret)
   		goto cleanup;

   	/* Print raw data */
   	printf("FIFO:\n%8s, %8s, %8s, %8s,\n", "#", "X", "Y", "Z");
   	for (int i = 0; i < sample_sets; i++) {
   		/* The 12-bit samples are already sign extended to 16 bits by
   		 * adxl37x_get_fifo_xyz_data_workaround().
   		 */

   		/* Print sample data, multiplied by scale factor of 0.1g/LSB */
   		printf("%d, %8.2lf, %8.2lf, %8.2lf,\n",
   			i + 1,
   			(double)(accel_data[i].x * 0.1),
   			(double)(accel_data[i].y * 0.1),
   			(double)(accel_data[i].z * 0.1));
   	}

   	cleanup:
   	if (adxl37x_desc)
   		adxl37x_remove(adxl37x_desc);

   	return ret;
   	}

ADXL37x no-OS IIO Support
--------------------------

The ADXL37x IIO driver builds on top of the base ADXL37x driver and provides
support for interfacing IIO clients through the IIO lib.

ADXL37x IIO Device Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Device Attributes
^^^^^^^^^^^^^^^^^

The ADXL37x IIO device does not have any device-level attributes.

Device Channels
^^^^^^^^^^^^^^^

The ADXL37x IIO device has 3 input channels and 0 output channels — one
acceleration channel per axis:

* Channel 0: accel_x
* Channel 1: accel_y
* Channel 2: accel_z

Each channel exposes 8 attributes. Six are shared across all three channels
and two are per-channel:

* ``calibbias`` — signed 4-bit offset trim for each axis, written to the
  OFFSET_X/Y/Z registers (0x20–0x22). The driver places the device in standby
  for the write and restores the previous operating mode afterwards. Valid
  range: −8 to 7.
* ``filter_high_pass_enable`` *(shared)* — enables or disables the high-pass
  filter through the ``POWER_CTL`` HPF_DISABLE bit. Writing 1 enables the
  filter, 0 disables it.
* ``filter_low_pass_3db_frequency`` *(shared)* — current low-pass filter
  corner frequency in Hz, derived from the ``MEASURE[2:0]`` bandwidth field.
  Values differ between ADXL371 and ADXL372 (see table below).
* ``filter_low_pass_3db_frequency_available`` *(shared)* — space-separated
  list of valid LPF corner frequencies for the active chip variant.
* ``raw`` — raw 12-bit signed acceleration value read from the device.
* ``sampling_frequency`` *(shared)* — current output data rate in Hz, derived
  from the ``TIMING[7:5]`` ODR field. Values differ between ADXL371 and
  ADXL372 (see table below).
* ``sampling_frequency_available`` *(shared)* — space-separated list of valid
  ODR values for the active chip variant.
* ``scale`` *(shared)* — constant scale factor to convert raw to m/s².
  Value: **0.980665** (100 mg/LSB for both chip variants).

The ``calibbias`` trim is applied by the device itself, so ``raw`` already
contains it. To obtain acceleration in m/s²:

.. code-block:: bash

   converted_accel [m/s²] = raw * scale
   converted_accel [m/s²] = raw * 0.980665

ODR and LPF available values per chip variant:

+-------+--------------------+--------------------+--------------------+--------------------+
| Index | ADXL371 ODR (Hz)   | ADXL372 ODR (Hz)   | ADXL371 LPF (Hz)   | ADXL372 LPF (Hz)   |
+-------+--------------------+--------------------+--------------------+--------------------+
| 0     | 320                | 400                | 160                | 200                |
+-------+--------------------+--------------------+--------------------+--------------------+
| 1     | 640                | 800                | 320                | 400                |
+-------+--------------------+--------------------+--------------------+--------------------+
| 2     | 1280               | 1600               | 640                | 800                |
+-------+--------------------+--------------------+--------------------+--------------------+
| 3     | 2560               | 3200               | 1280               | 1600               |
+-------+--------------------+--------------------+--------------------+--------------------+
| 4     | 5120               | 6400               | 2560               | 3200               |
+-------+--------------------+--------------------+--------------------+--------------------+

Device Buffers
^^^^^^^^^^^^^^

The ADXL37x IIO driver supports buffered data reads. Samples are collected
using ``adxl37x_get_fifo_xyz_data_workaround`` and up to
``ADXL37X_FIFO_MAX_SAMPLES`` (170) XYZ triplets can be read per call.
The active channel mask set during ``pre_enable`` controls which axes are
written into the IIO buffer.

ADXL37x IIO Driver Initialization Example
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: C

   #include "iio_adxl37x.h"
   #include "adxl37x.h"
   #include "iio_app.h"

   #define DATA_BUFFER_SIZE (ADXL37X_FIFO_MAX_SAMPLES * 3 * sizeof(int))
   int32_t iio_data_buffer[ADXL37X_FIFO_MAX_SAMPLES * 3];

   struct adxl37x_iio_dev *adxl37x_iio_desc;

   struct no_os_spi_init_param spi_ip = {
   	.device_id    = 0,
   	.max_speed_hz = 10000000,
   	.mode         = NO_OS_SPI_MODE_0,
   	.bit_order    = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
   	.platform_ops = &max_spi_ops,
   	.extra        = &max_spi_param,
   };

   struct adxl37x_init_param adxl37x_ip = {
   	.adxl_type    = ADXL371,
   	.comm_type    = ADXL37X_SPI,
   	.spi_init     = spi_ip,
   	.op_mode      = ADXL37X_FULL_BW_MEASUREMENT,
   };

   struct adxl37x_iio_dev_init_param adxl37x_iio_ip = {
   	.adxl37x_dev_init = &adxl37x_ip,
   };

   struct iio_data_buffer accel_buff = {
   	.buff = (void *)iio_data_buffer,
   	.size = DATA_BUFFER_SIZE,
   };

   struct iio_app_desc *app;
   struct iio_app_init_param app_init_param = { 0 };

   ret = adxl37x_iio_init(&adxl37x_iio_desc, &adxl37x_iio_ip);
   if (ret)
   	return ret;

   struct iio_app_device iio_devices[] = {
   	{
   		.name = "adxl37x",
   		.dev = adxl37x_iio_desc,
   		.dev_descriptor = adxl37x_iio_desc->iio_dev,
   		.read_buff = &accel_buff,
   	}
   };

   app_init_param.devices = iio_devices;
   app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
   app_init_param.uart_init_params = adxl37x_uart_ip;

   ret = iio_app_init(&app, app_init_param);
   if (ret)
   	goto error_dev;

   ret = iio_app_run(app);

   iio_app_remove(app);
   error_dev:
   	adxl37x_iio_remove(adxl37x_iio_desc);

   return ret;
