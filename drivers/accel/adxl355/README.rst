ADXL355 no-OS driver
====================

Supported Devices
-----------------

`ADXL355 <https://www.analog.com/ADXL355>`_

Overview
--------

The digital output `ADXL355 <https://www.analog.com/ADXL355>`_ is low noise
density, low 0 g offset drift, low power, 3-axis accelerometer with selectable
measurement ranges. The `ADXL355 <https://www.analog.com/ADXL355>`_ supports
the ±2 g, ±4 g, and ±8 g ranges. The
`ADXL355 <https://www.analog.com/ADXL355>`_ offer industry leading noise,
minimal offset drift over temperature, and long-term stability enabling
precision applications with minimal calibration.

Highly integrated in a compact form factor, the low power
`ADXL355 <https://www.analog.com/ADXL355>`_ is ideal in an Internet of Things
(IoT) sensor node and other wireless product designs.

The `ADXL355 <https://www.analog.com/ADXL355>`_ multifunction pin names may be
referenced by their relevant function only for either the serial peripheral
interface (SPI) or I2C interface.

Applications
------------

* Inertial measurement units (IMUs)/altitude and heading reference systems (AHRS)
* Platform stabilization systems
* Structural health monitoring
* Seismic imaging
* Tilt sensing
* Robotics
* Condition Monitoring

ADXL355 Device Configuration
----------------------------

Driver Initialization
---------------------

In order to be able to use the device, you will have to provide the support for
the communication protocol (SPI or I2C) as mentioned above.

The first API to be called is **adxl355_init**. Make sure that it returns 0,
which means that the driver was initialized correctly.

After, it is recommended to perform a soft reset, by calling
**adxl355_soft_reset**, in order to put the device in a known state.

Range Configuration
-------------------

By default, the device uses +/- 2g range configuration. You may modify this
value to +/- 4g or +/- 8g by using **adxl355_set_range** API.

Filters Configuration
---------------------

The ADXL355 uses an analog, low-pass, antialiasing filter to reduce out of band
noise and to limit bandwidth. The ADXL355 provides further digital filtering
options to maintain optimal noise performance at various output data rates
(ODRs). The default configuration for output data rate and low-pass filter
corner is 4000Hz and 1000 Hz respectively. You may modify the ODR and low-pass
filter configuration using **adxl355_set_odr_lpf** API.

The ADXL355 also includes an optional digital high-pass filter with a
programmable corner frequency. By default, the high-pass filter is disabled.
You may enable it with the desired configuration by using
**adxl355_set_hpf_corner** API.

Axis Calibration Configuration
------------------------------

You may set an offset for each axis for calibration purposes by using
**adxl355_set_offset**.

The offset is disabled by default.

The data provided in **adxl355_set_offset** all be in two's complement format.

The offset is added after all other signal processing is taking place.

FIFO Configuration
------------------

There are 96 locations in the FIFO. Each location contains an acceleration data
point on an axis.

In case you want to use the FIFO, you are able to modify the number of FIFO
samples to be stored in the FIFO. The default and maximum value is 96, but can
be modified using **adxl355_set_fifo_samples** API.

Activity Detection Configuration
--------------------------------

In case you want to use the activity detection algorithm, you can enable this
feature on any axis using **adxl355_conf_act_en** API. By default the activity
detection feature is disabled.

If activity detection feature is enabled, you will also have to specify a
threshold for the activity detection using **adxl355_conf_act_thr** API and a
number of consecutive measurements above the threshold which would trigger the
detection of an activity. This number has to be set using
**adxl355_set_act_cnt_reg**. The default value is 1.

Interrupts Configuration
------------------------

The device allows the usage of interrupts which can be mapped to INT1 or INT2
pins.

In case you want to use interrupts, make sure you configure the interrupt map
by using **adxl355_config_int_pins** API. You may also configure the polarity
of the interrupt, whether is active high or active low, by using
**adxl355_set_int_pol** API.

The following events can be mapped to the interrupt pins:

* DATA READY interrupt - it is triggered when new acceleration data is available to the interface and it is cleared on a read of the status register, using **adxl355_get_sts_reg** API.
* FIFO FULL interrupt - it is triggered when the entries  in the FIFO are equal to the setting made when calling **adxl355_set_fifo_samples** API. It is cleared when the number of entries in the FIFO is less than the number of samples indicated by **adxl355_get_nb_of_fifo_entries** API, which is only the case if sufficient data is read from the FIFO. It can also be cleared on a read of the status register, using **adxl355_get_sts_reg** API, but only when the entries in the FIFO are less than the value returned by **adxl355_get_nb_of_fifo_entries** API.
* FIFO OVERFLOW interrupt - it is triggered when the FIFO is so far overrange that data is lost. The specified side of the FIFO is 96 locations. The interrupt is triggered only when there is an attempt to write past this 96-location limit. A read to the status register, using **adxl355_get_sts_reg** API clears the interrupt.
* ACTIVITY interrupt - it is set when the measured acceleration on any axis is above the set threshold, using **adxl355_conf_act_thr** API for a number of consecutive measurement, equal with the value set using **adxl355_set_act_cnt_reg** API. A read of the status register, using **adxl355_get_sts_reg** API clears the interrupt, but this interrupt is triggered again at the end of the next measurement if the activity conditions are still satisfied.

ADXL355 Device Measurements
---------------------------

Operation Mode Setting
----------------------

After the specific configuration was performed as mentioned above, you can set
the device in the desired measurement mode, using **adxl355_set_op_mode** API.
The available operation modes for measurement are as follows:

* **ADXL355_MEASUREMENT_TEMP_ON_DRDY_ON** - measurement mode, with temperature measurement enabled and data ready pin functionality enabled.
* **ADXL355_MEASUREMENT_TEMP_OFF_DRDY_ON** - measurement mode, with temperature measurement disabled and data ready pin functionality enabled.
* **ADXL355_MEASUREMENT_TEMP_ON_DRDY_OFF** - measurement mode, with temperature measurement enabled and data ready pin functionality disabled.
* **ADXL355_MEASUREMENT_TEMP_OFF_DRDY_OFF** - measurement mode, with temperature measurement disabled and data ready pin functionality disabled.

Temperature Data
----------------

In case the operation mode selected includes the temperature measurement, then
data can be obtained by calling **adxl355_get_temp** API. The temperature is in
millidegrees Celsius, with scaling already applied. 

If you want to obtain the raw temperature data without any scaling applies,
simply call **adxl355_get_raw_temp** API.

Acceleration Data
-----------------

Single Data Set
---------------

If you want to obtain a single data set, you may use **adxl355_get_xyz** API to
obtain the data converted to g, or **adxl355_get_raw_xyz** API to obtain the
raw data. The raw data is in two's complement format and it does not have the
scaling applied.

FIFO Data
---------

If you want to read from the FIFO, you may use **adxl355_get_fifo_data** API to
obtain the data converted to g, or **adxl355_get_raw_fifo_data** api to obtain
the raw data. The raw data is in two's complement format and it does not have
the scaling applied.

The parameter fifo_entries shows the number of valid measurements in the FIFO
which were read.

ADXL355 Driver Initialization Example
-------------------------------------

SPI Communication Example
-------------------------

.. code-block:: bash

	struct adxl355_dev *adxl355;
	// Particular SPI configuration
	struct no_os_spi_init_param sip = {
		.max_speed_hz = 10000000,
		.bit_order = SPI_BIT_ORDER_MSB_FIRST,
		.mode = SPI_CPOL0_SPI_CPHA0,
	};
	struct adxl355_init_param init_data_adxl355 = {
		.comm_init.spi_init = sip,
		.comm_type = ADXL355_SPI_COMM,
	};

	ret = adxl355_init(&adxl355, init_data_adxl355);
	if (ret < 0)
		goto error;
	ret = adxl355_soft_reset(adxl355);
	if (ret < 0)
		goto error;
	ret = adxl355_set_odr_lpf(adxl355, ADXL355_ODR_4000HZ);
	if (ret < 0)
		goto error;
	ret = adxl355_set_op_mode(adxl355, ADXL355_MEAS_TEMP_ON_DRDY_OFF);
	if (ret < 0)
		goto error;

	// Read single accel data 
	struct adxl355_frac_repr x;
	struct adxl355_frac_repr y;
	struct adxl355_frac_repr z;

	ret = adxl355_get_xyz(adxl355,&x[0], &y[0], &z[0]);
	if (ret < 0)
		goto error;

	// Read FIFO accel data
	struct adxl355_frac_repr x[32] = {0};
	struct adxl355_frac_repr y[32] = {0};
	struct adxl355_frac_repr z[32] = {0};
	uint8_t fifo_entries = 0;
	ret = adxl355_get_fifo_data(adxl355, &fifo_entries,
	                            &x[0], &y[0], &z[0]);
	if (ret < 0)
		goto error;

	// Read temperature data
	struct adxl355_frac_repr temp;
	ret = adxl355_get_temp(adxl355, &temp);
	if (ret < 0)
		goto error;

ADXL355 no-OS IIO support
-------------------------

The ADXL355 IIO driver comes on top of ADXL355 driver and offers support for interfacing IIO clients through IIO lib.

ADXL355 IIO Device Configuration
--------------------------------

Device Attributes
-----------------

ADXL355 IIO device does not have any device specific attributes.

Device Channels
---------------

ADXL355 IIO device has 0 output channels and 4 input channels: 3 acceleration channels and 1 temperature channel.

Acceleration channels
---------------------

The acceleration channels are:

* Channel 0: accel_x
* Channel 1: accel_y
* Channel 2: accel_z

Each acceleration channel has 7 attributes. 5 of these attributes are shared in value with the other acceleration channels and 2 of these attributes can have different values for each channel.

The attributes are:

* calibbias - offset added to the axis after all other signal processing. The calibbias value will be applied as an offset to the raw value bits [19:4]. See formula bellow. 
* filter_high_pass_3db_frequency (shared) - is the current setting for the high pass filter corner frequency. This value is common for all three acceleration channels and the value is influenced by the current sampling frequency (see table below).
* filter_high_pass_3db_frequency_available (shared) - is the list of available high pass filter corner frequency values. This list is common for all three acceleration channels and the values are influenced by the current sampling frequency (see table below).
* raw - is the raw acceleration value read from the device.
* sampling_frequency (shared) - is the sampling frequency for acceleration data. This value is common for all three acceleration channels.
* sampling_frequency_available (shared) - is the list of available sampling frequency values. This list is common for all three acceleration channels.
* scale (shared) - is the scale that has to be applied to the raw value in order to obtain the converted real value in m/s^2 (see formula below). It has a constant value equal to: **0.000038245**.

+------------------------+----------+----------+----------+----------+----------+----------+----------+
|Sampling frequency (Hz) | High Pass Filter Corner Frequency available values (Hz)                    |
+------------------------+----------+----------+----------+----------+----------+----------+----------+
| 4000.000000            | 0.000000 | 9.880000 | 2.483360 | 0.621800 | 0.154480 | 0.038160 | 0.009520 |
+------------------------+----------+----------+----------+----------+----------+----------+----------+
| 2000.000000            | 0.000000 | 4.940000 | 1.241680 | 0.310900 | 0.077240 | 0.019080 | 0.004760 |
+------------------------+----------+----------+----------+----------+----------+----------+----------+
| 1000.000000            | 0.000000 | 2.470000 | 0.620840 | 0.155450 | 0.038620 | 0.009540 | 0.002380 |
+------------------------+----------+----------+----------+----------+----------+----------+----------+
| 500.000000             | 0.000000 | 1.235000 | 0.310420 | 0.077725 | 0.019310 | 0.004770 | 0.001190 |
+------------------------+----------+----------+----------+----------+----------+----------+----------+
| 250.000000             | 0.000000 | 0.617500 | 0.155210 | 0.038862 | 0.009655 | 0.002385 | 0.000595 |
+------------------------+----------+----------+----------+----------+----------+----------+----------+
| 125.000000             | 0.000000 | 0.308750 | 0.077605 | 0.019431 | 0.004827 | 0.001192 | 0.000297 |
+------------------------+----------+----------+----------+----------+----------+----------+----------+
| 62.500000              | 0.000000 | 0.154375 | 0.038802 | 0.009715 | 0.002413 | 0.000596 | 0.000148 |
+------------------------+----------+----------+----------+----------+----------+----------+----------+
| 31.250000              | 0.000000 | 0.077187 | 0.019401 | 0.004857 | 0.001206 | 0.000298 | 0.000074 |
+------------------------+----------+----------+----------+----------+----------+----------+----------+
| 15.625000              | 0.000000 | 0.038593 | 0.009700 | 0.002428 | 0.000603 | 0.000149 | 0.000037 |
+------------------------+----------+----------+----------+----------+----------+----------+----------+
| 7.813000               | 0.000000 | 0.019298 | 0.004850 | 0.001214 | 0.000301 | 0.000074 | 0.000018 |
+------------------------+----------+----------+----------+----------+----------+----------+----------+
| 3.906000               | 0.000000 | 0.009647 | 0.002425 | 0.000607 | 0.000150 | 0.000037 | 0.000009 |
+------------------------+----------+----------+----------+----------+----------+----------+----------+

In order to obtain the value in the m/s^2 units, raw has to be scaled by scale:

.. code-block:: bash

	converted_accel [m/s^2] = (raw + calibbias << 4) * scale 
	converted_accel [m/s^2] = (raw + calibbias << 4) * 0.000038245

Temperature channel
-------------------

The temperature channel is:

* Channel 4: temp

The channel has 3 attributes, as follows:

* offset - is the offset that has to be applied to the raw value in order to obtain the converted real value in degrees Celsius (see formula below). It has a constant value equal to **-2111.250000**.
* raw - is the raw temperature value read from the device.
* scale - is the scale that has to be applied to the raw value in order to obtain the converted real value in degrees Celsius (see formula below). It has a constant value equal to: **-110.497238**.

In order to obtain the value in the degrees Celsius units, offset has to be added to raw prior to scaling by scale:

.. code-block:: bash

	converted_temp [degrees Celsius] = (raw + offset) * scale 
	converted_temp [degrees Celsius] = (2111.25 - raw) * 110.497238 

Device buffers
--------------

The ADXL355 IIO devices driver supports the usage of a data buffer for reading purposes.

ADXL355 IIO Driver Initialization Example
-----------------------------------------

.. code-block:: bash

	#define DATA_BUFFER_SIZE 400
	uint8_t iio_data_buffer[DATA_BUFFER_SIZE*4*sizeof(int)];

	struct adxl355_iio_dev *adxl355_iio_desc;
	struct adxl355_iio_init_param adxl355_init_par;

	// Particular SPI configuration
	struct no_os_spi_init_param sip = {
		.max_speed_hz = 10000000,
		.bit_order = SPI_BIT_ORDER_MSB_FIRST,
		.mode = SPI_CPOL0_SPI_CPHA0,
	};

	struct iio_data_buffer accel_buff = {
		.buff = (void *)iio_data_buffer,
		.size = DATA_BUFFER_SIZE*4*sizeof(int)
	};

	struct adxl355_init_param init_data_adxl355 = {
		.comm_init.spi_init = sip,
		.comm_type = ADXL355_SPI_COMM,
	};

	ret = adxl355_iio_init(&adxl355_iio_desc, &adxl355_init_par);
	if (ret != SUCCESS)
		return ret;

	struct iio_app_device iio_devices[] = {
		{
			.name = "adxl355",
			.dev = adxl355_iio_desc,
			.dev_descriptor = adxl355_iio_desc->iio_dev,
			.read_buff = &accel_buff,
			.write_buff = NULL
		}
	};

	return iio_app_run(iio_devices, NO_OS_ARRAY_SIZE(iio_devices));
