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
| ADXL37x_STANDBY            | Standby mode for    | 0                   | When the device     |
|                            | lowest power        |                     | needs to conserve   |
|                            | consumption.        |                     | energy and data     |
|                            |                     |                     | acquisition is not  |
|                            |                     |                     | required            |
+----------------------------+---------------------+---------------------+---------------------+
| ADXL37x_WAKE_UP            | Low-power motion    | 1                   | Ideal for simple    |
|                            | detection at a set  |                     | detection of the    |
|                            | wake-up rate.       |                     | presence or absence |
|                            |                     |                     | of motion at an     |
|                            |                     |                     | extremely low power |
|                            |                     |                     | consumption         |
+----------------------------+---------------------+---------------------+---------------------+
| ADXL37x_INSTANT_ON         | Extremely low-power | 2                   | Capture impact      |
|                            | impact / shock      |                     | events and store    |
|                            | detection           |                     | peak acceleration   |
|                            |                     |                     | values at very low  |
|                            |                     |                     | power consumption   |
+----------------------------+---------------------+---------------------+---------------------+
| ADXL37x_FULL_BW_MEASUREMENT| Full-bandwidth      | 3                   | When full-bandwidth |
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
the FIFO using the anomaly workaround described in the :adi:`ADXL37x` datasheet.
The raw samples are placed into a data structure representing 3-axis XYZ data.

The ``adxl37x_service_fifo_ev`` function is a wrapper around the
``adxl37x_get_fifo_xyz_data_workaround`` function to streamline the process of
servicing FIFO events.

Driver Initialization Example
------------------------------

.. code-block:: C

   #include "adxl37x.h"
   #include "no_os_i2c.h"
   #include "no_os_spi.h"
   #include "no_os_alloc.h"
   #include "no_os_delay.h"

   #include "adxl37x_noos_data.h"

   #define ACCEL_MAX_XYZ 170

   /* ADXL Device Instance */
   uint8_t status1=0, status2=0;
   uint16_t fifo_entries=0;

   int main(void)
   {
    	struct adxl37x_dev *adxl37x_desc = NULL;
    	struct adxl37x_xyz_accel_data accel_data[ACCEL_MAX_XYZ];
    	uint8_t device_range;
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
   		.spi_init = spiInitParams,
   		.gpio_int1 = int1Init,
   		.int1_config = {},
   		.gpio_int2 = int2Init,
   		.int2_config = {},
   		.comm_type =  ADXL37x_SPI,
   		.odr =  ADXL37x_ODR_5120HZ,
   		.bw =  ADXL37x_BW_2560HZ,
   		.fifo_config = {
   			.fifo_format = ADXL37x_XYZ_PEAK_FIFO,
   			.fifo_mode = ADXL37x_FIFO_STREAMED,
   			.fifo_samples = ACCEL_MAX_XYZ
   		},
   		.op_mode = ADXL37x_FULL_BW_MEASUREMENT,
   		.act_proc_mode = ADXL37x_LOOPED
   	};
   	ret = adxl37x_init(&adxl37x_desc, adxl_ip);
   	if (ret)
   		goto cleanup;

   	ret = adxl37x_set_op_mode(adxl37x_desc, ADXL37x_STANDBY);
   	if (ret)
   		goto cleanup;

   	/** NOTE: Workaround for er001 requires internal sync to start */
   	adxl37x_set_internal_sync(adxl37x_desc);

   	adxl37x_set_op_mode(adxl37x_desc, ADXL37x_FULL_BW_MEASUREMENT);

   	/* Read # available samples into "fifo_entries" */
   	ret = adxl37x_get_status(adxl37x_desc, &status1, &status2, &fifo_entries);
   	if (ret)
   		goto cleanup;

   	/* Read raw acceleration data */
   	ret = adxl37x_get_fifo_xyz_data_workaround(adxl37x_desc, accel_data,
   			fifo_entries);
   	if (ret)
   		goto cleanup;

   	/* Print raw data */
   	printf("FIFO:\n%8s, %8s, %8s,\n", "X", "Y", "Z");
   	for (int i = 0; i < fifo_entries / 3; i++) {
   		/* 12-bit samples should be sign-extended as 16-bits */
   		accel_data[i].x = no_os_sign_extend16(accel_data[i].x, 11);
   		accel_data[i].y = no_os_sign_extend16(accel_data[i].y, 11);
   		accel_data[i].z = no_os_sign_extend16(accel_data[i].z, 11);

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

Each channel exposes 7 attributes. Five are shared across all three channels
and two are per-channel:

* ``calibbias`` — signed 4-bit offset added to the raw value for each axis,
  written to the OFFSET_X/Y/Z registers (0x20–0x22). The device must be in
  standby before writing. Valid range: −8 to 7.
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

To obtain acceleration in m/s²:

.. code-block:: bash

   converted_accel [m/s²] = (raw + calibbias) * scale
   converted_accel [m/s²] = (raw + calibbias) * 0.980665

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

   #define DATA_BUFFER_SIZE (ADXL37X_MAX_BUFFER_SAMPLES * 3 * sizeof(int))
   int32_t iio_data_buffer[ADXL37X_MAX_BUFFER_SAMPLES * 3];

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
   	.comm_type    = ADXL37x_SPI,
   	.spi_init     = spi_ip,
   	.op_mode      = ADXL37x_FULL_BW_MEASUREMENT,
   };

   struct adxl37x_iio_dev_init_param adxl37x_iio_ip = {
   	.adxl37x_dev_init = &adxl37x_ip,
   };

   struct iio_data_buffer accel_buff = {
   	.buff = (void *)iio_data_buffer,
   	.size = DATA_BUFFER_SIZE,
   };

   ret = adxl37x_iio_init(&adxl37x_iio_desc, &adxl37x_iio_ip);
   if (ret)
   	return ret;

   struct iio_app_device iio_devices[] = {
   	{
   		.name        = "adxl37x",
   		.dev         = adxl37x_iio_desc,
   		.dev_descriptor = adxl37x_iio_desc->iio_dev,
   		.read_buff   = &accel_buff,
   		.write_buff  = NULL,
   	}
   };

   return iio_app_run(iio_devices, NO_OS_ARRAY_SIZE(iio_devices));
