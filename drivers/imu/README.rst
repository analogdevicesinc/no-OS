ADIS no-OS driver
====================

.. contents::
    :depth: 2

Supported Devices
-----------------

* `ADIS16465 <https://www.analog.com/ADIS16465>`_
* `ADIS16467 <https://www.analog.com/ADIS16467>`_
* `ADIS16470 <https://www.analog.com/ADIS16470>`_
* `ADIS16475 <https://www.analog.com/ADIS16475>`_
* `ADIS16477 <https://www.analog.com/ADIS16477>`_
* `ADIS16500 <https://www.analog.com/ADIS16500>`_
* `ADIS16501 <https://www.analog.com/ADIS16501>`_
* `ADIS16505 <https://www.analog.com/ADIS16505>`_
* `ADIS16550 <https://www.analog.com/ADIS16550>`_
* `ADIS16507 <https://www.analog.com/ADIS16507>`_
* `ADIS16575 <https://www.analog.com/ADIS16575>`_
* `ADIS16576 <https://www.analog.com/ADIS16576>`_
* `ADIS16577 <https://www.analog.com/ADIS16577>`_

Overview
--------

The ADIS1650X device series is a precision, miniature microelectromechanical
system (MEMS) inertial measurement unit (IMU) that includes a triaxial
gyroscope and a triaxial accelerometer. Each inertial sensor in the ADIS1650X
device series combines with signal conditioning that optimizes dynamic
performance.

The factory calibration characterizes each sensor for sensitivity, bias,
alignment, linear acceleration (gyroscope bias), and point of percussion
(accelerometer location). As a result, each sensor has dynamic compensation
formulas that provide accurate sensor measurements over a broad set of
conditions.

Applications
------------

* Navigation, stabilization, and instrumentation
* Unmanned and autonomous vehicles
* Smart agriculture and construction machinery
* Factory/industrial automation, robotics
* Virtual/augmented reality
* Internet of Moving Things

ADIS Driver Source Code
-----------------------

The source code for ADIS generic driver can be found here:

* `Header file of ADIS Generic Driver <https://github.com/analogdevicesinc/no-OS/blob/main/drivers/imu/adis.h>`_
* `Implementation of ADIS Generic Driver <https://github.com/analogdevicesinc/no-OS/blob/main/drivers/imu/adis.c>`_
* `Header file of ADIS Internals <https://github.com/analogdevicesinc/no-OS/blob/main/drivers/imu/adis_internals.h>`_

The generic ADIS driver has to be used together with the chip-specific ADIS driver.
The source code for the supported chips is listed below:

ADIS1646X Driver Source Code:

* `Header file of ADIS1646X Driver <https://github.com/analogdevicesinc/no-OS/blob/main/drivers/imu/adis1646x.h>`_
* `Implementation of ADIS1646X Driver <https://github.com/analogdevicesinc/no-OS/blob/main/drivers/imu/adis1646x.c>`_

Supported devices with ADIS1646X files:

* `ADIS16465 <https://www.analog.com/ADIS16465>`_
* `ADIS16467 <https://www.analog.com/ADIS16467>`_
* `ADIS16470 <https://www.analog.com/ADIS16470>`_
* `ADIS16475 <https://www.analog.com/ADIS16475>`_

ADIS1647X Driver Source Code:

* `Header file of ADIS1647X Driver <https://github.com/analogdevicesinc/no-OS/blob/main/drivers/imu/adis1647x.h>`_
* `Implementation of ADIS1647X Driver <https://github.com/analogdevicesinc/no-OS/blob/main/drivers/imu/adis1647x.c>`_

Supported devices with ADIS1647X files:

* `ADIS16477 <https://www.analog.com/ADIS16477>`_

ADIS1650X Driver Source Code:

* `Header file of ADIS1650X Driver <https://github.com/analogdevicesinc/no-OS/blob/main/drivers/imu/adis1650x.h>`_
* `Implementation of ADIS1650X Driver <https://github.com/analogdevicesinc/no-OS/blob/main/drivers/imu/adis1650x.c>`_

Supported devices with ADIS1650X files:

* `ADIS16500 <https://www.analog.com/ADIS16500>`_
* `ADIS16501 <https://www.analog.com/ADIS16501>`_
* `ADIS16505 <https://www.analog.com/ADIS16505>`_
* `ADIS16507 <https://www.analog.com/ADIS16507>`_

ADIS1654X Driver Source Code:

* `Header file of ADIS1654X Driver <https://github.com/analogdevicesinc/no-OS/blob/main/drivers/imu/adis1654x.h>`_
* `Implementation of ADIS1654X Driver <https://github.com/analogdevicesinc/no-OS/blob/main/drivers/imu/adis1654x.c>`_

Supported devices with ADIS1654X files:

* `ADIS16545 <https://www.analog.com/ADIS16545>`_
* `ADIS16547 <https://www.analog.com/ADIS16547>`_

ADIS1655X Driver Source Code:

* `Header file of ADIS1655X Driver <https://github.com/analogdevicesinc/no-OS/blob/main/drivers/imu/adis1655x.h>`_
* `Implementation of ADIS1655X Driver <https://github.com/analogdevicesinc/no-OS/blob/main/drivers/imu/adis1655x.c>`_

Supported devices with ADIS1655X files:

* `ADIS16550 <https://www.analog.com/ADIS16550>`_

ADIS1657X Driver Source Code:

* `Header file of ADIS1657X Driver <https://github.com/analogdevicesinc/no-OS/blob/main/drivers/imu/adis1657x.h>`_
* `Implementation of ADIS1657X Driver <https://github.com/analogdevicesinc/no-OS/blob/main/drivers/imu/adis1657x.c>`_

Supported devices with ADIS1657X files:

* `ADIS16575 <https://www.analog.com/ADIS16575>`_
* `ADIS16576 <https://www.analog.com/ADIS16576>`_
* `ADIS16577 <https://www.analog.com/ADIS16577>`_

ADIS Driver Usage Outside of No-OS
-----------------------------------

In order to be able to use this driver you will have to provide the specific
implementation for the communication APIs and the specific types they use.
For SPI communication, there are four functions which are called by the ADIS
driver and have to be implemented:

* **no_os_spi_init()** - initializes the communication peripheral.
* **no_os_spi_write_and_read()** - writes and reads data to/from the device.
* **no_os_spi_transfer()** - iterates over transfer lists and sends all SPI messages
* **no_os_spi_remove()** - de-initializes the communication peripheral.

And there are three data types that have to be defined:

* **no_os_spi_desc** - structure holding the SPI descriptor
* **no_os_spi_init_param** - structure holding the parameters for SPI initialization
* **no_os_spi_msg** - structure holding the description for a SPI transfer

An example of a header file containing the prototypes of the functions which have
to be implemented, along with some generic data types they are using can be found
below:

* `Generic header file for SPI Communication APIs <https://github.com/analogdevicesinc/no-OS/blob/main/include/no_os_spi.h>`_

You will also have to provide specific APIs for GPIO handling. There are five
functions which are called by the ADIS driver and have to be implemented:

* **no_os_gpio_get_optional** - returns the descriptor to a specific GPIO
* **no_os_gpio_direction_output** - enables the output direction of the specified GPIO descriptor
* **no_os_gpio_set_value** - sets the value of the specified GPIO to high or low
* **no_os_gpio_remove** - frees the resources allocated by no_os_gpio_get_optional

And there are two data types that have to be defined:

* **no_os_gpio_desc** - structure holding the GPIO descriptor
* **no_os_gpio_init_param** - structure holding the parameters for GPIO initialization

An example of a header file containing the prototypes of the functions which have
to be implemented, along with some generic data types they are using can be found below:

* `Generic header file for GPIO APIs <https://github.com/analogdevicesinc/no-OS/blob/main/include/no_os_gpio.h>`_

The driver also uses the ADI util library, so make sure you also add the necessary
files in your project. The source code for the util library can be found here:

* `Header file of ADI util library <https://github.com/analogdevicesinc/no-OS/blob/main/include/no_os_util.h>`_
* `Implementation file of ADI util library <https://github.com/analogdevicesinc/no-OS/blob/main/util/no_os_util.c>`_

ADIS Driver Usage with No-OS
----------------------------

If you wish to use No-OS for evaluating ADIS devices, you may use the already
existing example projects:

* `ADIS1646X Example Projects <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-adis1646x>`_
* `ADIS1647X Example Projects <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-adis1647x>`_
* `ADIS1650X Example Projects <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-adis1650x>`_
* `ADIS1654X Example Projects <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-adis1654x>`_
* `ADIS1655X Example Projects <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-adis1655x>`_
* `ADIS1657X Example Projects <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-adis1657x>`_

ADIS Device Initialization
--------------------------

In order to be able to use the device, you will have to provide the support for
the communication protocol and GPIO configuration as mentioned above. The first
API to be called is **adis_init** by providing the adis descriptor and the
adis1650x_chip_info. Make sure that it returns 0, which means that the driver
was initialized correctly.

ADIS Device Measurements - Register Readings
--------------------------------------------

Acceleration Data
^^^^^^^^^^^^^^^^^
If you want to obtain a data set for each axis, you may use **adis_read_x_accl**,
**adis_read_y_accl** and **adis_read_z_accl** APIs to obtain the raw data.
The raw data does not have the scaling applied. Use **adis_get_accl_scale** to
obtain the acceleration scale.
The converted value meters / square second is obtained by the following formula:

.. code-block:: bash

	result [m / s^2] = raw data * scale.dividend / scale.divisor

Gyroscope Data
^^^^^^^^^^^^^^

If you want to obtain a data set for each axis, you may use **adis_read_x_gyro**,
**adis_read_y_gyro** and **adis_read_z_gyro** APIs to obtain the raw data.
The raw data does not have the scaling applied. Use **adis_get_anglvel_scale**
to obtain the gyroscope scale.
The converted value in radians / second is obtained by the following formula:

.. code-block:: bash

	result [rad / s] = raw data * scale.dividend / scale.divisor

Delta Velocity Data
^^^^^^^^^^^^^^^^^^^

If you want to obtain a data set for each axis, you may use **adis_read_x_deltvel**,
**adis_read_y_deltvel** and **adis_read_z_deltvel** APIs to obtain the raw data.
The raw data does not have the scaling applied. Use **adis_get_deltavelocity_scale**
to obtain the delta velocity scale.
The converted value in meters / second is obtained by the following formula:

.. code-block:: bash

	result [m / s] = raw data * scale.dividend / scale.divisor

Delta Angle Data
^^^^^^^^^^^^^^^^

If you want to obtain a data set for each axis, you may use **adis_read_x_deltang**,
**adis_read_y_deltang** and **adis_read_z_deltang** APIs to obtain the raw data.
The raw data does not have the scaling applied. Use **adis_get_deltaangl_scale**
to obtain the delta angle scale.
The converted value in radians is obtained by the following formula:

.. code-block:: bash

	result [rad] = raw data * scale.dividend / scale.divisor

Temperature Data
^^^^^^^^^^^^^^^^

If you want to obtain the temperature data of the device, you may use
**adis_read_temp_out** API to obtain the raw data. The raw data does not have the
scaling applied. Use **adis_get_temp_scale**
to obtain the temperature scale.
The converted value in millidegrees Celsius is obtained by the following formula:

.. code-block:: bash

	result [milli °C] = raw data * scale.dividend / scale.divisor

ADIS Device Measurements - Burst Readings
--------------------------------------------

The burst read function provides a way to read a batch of output data registers.
If you want to perform a burst read, you may use **adis_read_burst_data** API.

You will have to provide the following parameters:

* adis descriptor pointer
* buffer - the buffer to be filled with the read data. Sufficient memory has to be allocated, as described with buffer size.
* buffer size - the size of the provided buffer, in bytes. The size of the provided buffer has to be at least 18 bytes if burst32 is false and at least 30 bytes if burst32 is true.
* burst32 - true if 32-bit data is requested for accel and gyro or delta angle and delta velocity measurements, false if 16-bit data is requested. It is supported only by adis1647x, adis1650x and adis1657x devices.
* burst_sel - 0 if accel and gyro data is requested, 1 if delta angle and delta velocity is requested. It is supported only by adis1647x, adis1650x and adis1657x devices.
* fifo_pop - in case FIFO is supported, will send a fifo pop request if parameter value is true. If FIFO is not supported, this parameter is ignored. It is supported only by adis1657x devices.
* burst_request - in case FIFO is supported, will send a burst request and -EAGAIN will be returned if the request has been sent successfully. With this request, no burst data is returned. This burst request is needed if the previous command sent to the device was not a burst read. If FIFO is not supported, this parameter is ignored. It is supported only by adis1657x devices.

The buffer will contain the following data based on burst32 and burst_sel:

burst32 = false burst_sel = 0:

* bytes 0-1:   diagnosis register
* bytes 2-3:   anglvel_x
* bytes 4-5:   anglvel_y
* bytes 6-7:   anglvel_z
* bytes 8-9:   accel_x
* bytes 10-11: accel_y
* bytes 12-13: accel_z
* bytes 14-15: temp0
* bytes 16-17: data_cntr/timestamp

burst32 = false burst_sel = 1:

* bytes 0-1:   diagnosis register
* bytes 2-3:   deltaangl_x
* bytes 4-5:   deltaangl_y
* bytes 6-7:   deltaangl_z
* bytes 8-9:   deltavelocity_x
* bytes 10-11: deltavelocity_y
* bytes 12-13: deltavelocity_z
* bytes 14-15: temp0
* bytes 16-17: data_cntr/timestamp

burst32 = true burst_sel = 0:

* bytes 0-1:   diagnosis register
* bytes 2-3:   anglvel_x lsb
* bytes 4-5:   anglvel_x msb
* bytes 6-7:   anglvel_y lsb
* bytes 8-9:   anglvel_y msb
* bytes 10-11: anglvel_z lsb
* bytes 12-13: anglvel_z msb
* bytes 14-15: accel_x lsb
* bytes 16-17: accel_x msb
* bytes 18-19: accel_y lsb
* bytes 20-21: accel_y msb
* bytes 22-23: accel_z lsb
* bytes 24-25: accel_z msb
* bytes 26-27: temp0
* bytes 28-29: data_cntr/timestamp

burst32 = true burst_sel = 0:

* bytes 0-1:   diagnosis register
* bytes 2-3:   deltaangl_x lsb
* bytes 4-5:   deltaangl_x msb
* bytes 6-7:   deltaangl_y lsb
* bytes 8-9:   deltaangl_y msb
* bytes 10-11: deltaangl_z lsb
* bytes 12-13: deltaangl_z msb
* bytes 14-15: deltavelocity_x lsb
* bytes 16-17: deltavelocity_x msb
* bytes 18-19: deltavelocity_y lsb
* bytes 20-21: deltavelocity_y msb
* bytes 22-23: deltavelocity_z lsb
* bytes 24-25: deltavelocity_z msb
* bytes 26-27: temp0
* bytes 28-29: data_cntr/timestamp

ADIS Diagnosis Data
-------------------

If you want to obtain the diagnosis data of the device, you may use
**adis_read_diag_stat** API to obtain the diagnosis flags structure. You may also
use individual APIs for each diagnosis flag to obtain the individual value. The
APIs for retrieving diagnosis flags are specific to the device, as shown below.

Common
^^^^^^
* **adis_read_diag_data_path_overrun** - to obtain the data path overrun flag value
* **adis_read_diag_fls_mem_update_failure** - to obtain the flash memory update error flag value
* **adis_read_diag_spi_comm_err** - to obtain the SPI communication error flag value
* **adis_read_diag_standby_mode** - to obtain the standby mode flag value
* **adis_read_diag_snsr_failure** - to obtain the sensor self test error flag value
* **adis_read_diag_mem_failure** - to obtain the flash memory test error flag value
* **adis_read_diag_clk_err** - to obtain the clock error flag value
* **adis_read_diag_checksum_err** - to obtain the checksum error flag value from a previous burst read
* **adis_read_diag_fls_mem_wr_cnt_exceed** - to obtain the flash memory write counts exceeded flag value (set to true if the flash memory write counter exceeds the endurance value
* **adis_read_diag_stat** - to obtain all error flags

ADIS1646X
^^^^^^^^^

There are no other specific diagnosis flags for this chip version.

ADIS1647X
^^^^^^^^^

There are no other specific diagnosis flags for this chip version.

ADIS1650X
^^^^^^^^^

* **adis_read_diag_gyro1_failure** - to obtain the gyroscope1 self test error flag value
* **adis_read_diag_gyro2_failure** - to obtain the gyroscope2 self test error flag value
* **adis_read_diag_accl_failure** - to obtain the accelerometer self test error flag value

ADIS1657X
^^^^^^^^^

* **adis_read_diag_snsr_init_failure** - to obtain the sensor initialization failure flag value
* **adis_read_diag_x_axis_gyro_failure** - to obtain the X-Axis Gyroscope failure flag value
* **adis_read_diag_y_axis_gyro_failure** - to obtain the Y-Axis Gyroscope failure flag value
* **adis_read_diag_z_axis_gyro_failure** - to obtain the Z-Axis Gyroscope failure flag value
* **adis_read_diag_x_axis_accl_failure** - to obtain the X-Axis Accelerometer failure flag value
* **adis_read_diag_y_axis_accl_failure** - to obtain the Y-Axis Accelerometer failure flag value
* **adis_read_diag_z_axis_accl_failure** - to obtain the Z-Axis Accelerometer failure flag value
* **adis_read_diag_aduc_mcu_fault** - to obtain the ADuC microcontroller fault flag value

ADIS Identification Data
------------------------

If you want to obtain identification data specific to the device, you may use the following APIs:

* **adis_read_prod_id** - to obtain the product id
* **adis_read_serial_num** - to obtain the product serial number
* **adis_read_firm_rev** - to obtain the firmware revision
* **adis_read_firm_d**, **adis_read_firm_m** and **adis_read_firm_y** - to obtain the firmware date
* **adis_read_gyro_meas_range** to obtain gyroscope measurement range encoded value

ADIS Configuration Data
-----------------------

Accelerometer calibration offset
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If you want to configure the accelerometer calibration offset on any axis, you may use the following APIs:

* **adis_write_xa_bias**
* **adis_write_ya_bias**
* **adis_write_za_bias**

If you want to read the current accelerometer calibration on any axis, you may use the following APIs:

* **adis_read_xa_bias**
* **adis_read_ya_bias**
* **adis_read_za_bias**

Gyroscope calibration offset
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If you want to configure the gyroscope calibration offset on any axis, you may use the following APIs:

* **adis_write_xg_bias**
* **adis_write_yg_bias**
* **adis_write_zg_bias**

If you want to read the current gyroscope calibration on any axis, you may use the following APIs:

* **adis_read_xg_bias**
* **adis_read_yg_bias**
* **adis_read_zg_bias**


Filters configuration
^^^^^^^^^^^^^^^^^^^^^

* **adis_write_filt_size_var_b**, **adis_read_filt_size_var_b** - Bartlett window FIR filter write/read APIs
* **adis_write_dec_rate**, **adis_read_dec_rate** - decimation filter write/read APIs
* **adis_write_up_scale**, **adis_read_up_scale** - scale factor for input clock for scaled sync mode write/read APIs

Continuous Bias Estimation Configuration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Some devices offer continuous bias estimation configuration capabilities.
See the information below to view the configuration APIs for the devices which
offer continuous bias estimation capabilities.

**ADIS1646X**

  * **adis_write_bias_corr_tbc**, **adis_read_bias_corr_tbc** - to write/read the time base control value
  * **adis_write_bias_corr_en_xg**, **adis_read_bias_corr_en_xg** - to write/read the X-axis gyroscope bias correction enable bit (0 - disabled, 1 - enabled)
  * **adis_write_bias_corr_en_yg**, **adis_read_bias_corr_en_yg** - to write/read the Y-axis gyroscope bias correction enable bit (0 - disabled, 1 - enabled)
  * **adis_write_bias_corr_en_zg**, **adis_read_bias_corr_en_zg** - to write/read the Z-axis gyroscope bias correction enable bit (0 - disabled, 1 - enabled)
  * **adis_write_bias_corr_en_xa**, **adis_read_bias_corr_en_xa** - to write/read the X-axis accelerometer bias correction enable bit (0 - disabled, 1 - enabled)
  * **adis_write_bias_corr_en_ya**, **adis_read_bias_corr_en_ya** - to write/read the Y-axis accelerometer bias correction enable bit (0 - disabled, 1 - enabled)
  * **adis_write_bias_corr_en_za**, **adis_read_bias_corr_en_za** - to write/read the Z-axis accelerometer bias correction enable bit (0 - disabled, 1 - enabled)

**ADIS1647X**

  * **adis_write_bias_corr_tbc**, **adis_read_bias_corr_tbc** - to write/read the time base control value
  * **adis_write_bias_corr_en_xg**, **adis_read_bias_corr_en_xg** - to write/read the X-axis gyroscope bias correction enable bit (0 - disabled, 1 - enabled)
  * **adis_write_bias_corr_en_yg**, **adis_read_bias_corr_en_yg** - to write/read the Y-axis gyroscope bias correction enable bit (0 - disabled, 1 - enabled)
  * **adis_write_bias_corr_en_zg**, **adis_read_bias_corr_en_zg** - to write/read the Z-axis gyroscope bias correction enable bit (0 - disabled, 1 - enabled)
  * **adis_write_bias_corr_en_xa**, **adis_read_bias_corr_en_xa** - to write/read the X-axis accelerometer bias correction enable bit (0 - disabled, 1 - enabled)
  * **adis_write_bias_corr_en_ya**, **adis_read_bias_corr_en_ya** - to write/read the Y-axis accelerometer bias correction enable bit (0 - disabled, 1 - enabled)
  * **adis_write_bias_corr_en_za**, **adis_read_bias_corr_en_za** - to write/read the Z-axis accelerometer bias correction enable bit (0 - disabled, 1 - enabled)


**ADIS1650X**

	This device does not offer continuous bias estimation capabilities.

**ADIS1657X**

  * **adis_write_bias_corr_tbc**, **adis_read_bias_corr_tbc** - to write/read the time base control value
  * **adis_write_bias_corr_en_xg**, **adis_read_bias_corr_en_xg** - to write/read the X-axis gyroscope bias correction enable bit (0 - disabled, 1 - enabled)
  * **adis_write_bias_corr_en_yg**, **adis_read_bias_corr_en_yg** - to write/read the Y-axis gyroscope bias correction enable bit (0 - disabled, 1 - enabled)
  * **adis_write_bias_corr_en_zg**, **adis_read_bias_corr_en_zg** - to write/read the Z-axis gyroscope bias correction enable bit (0 - disabled, 1 - enabled)
  * **adis_write_bias_corr_en_xa**, **adis_read_bias_corr_en_xa** - to write/read the X-axis accelerometer bias correction enable bit (0 - disabled, 1 - enabled)
  * **adis_write_bias_corr_en_ya**, **adis_read_bias_corr_en_ya** - to write/read the Y-axis accelerometer bias correction enable bit (0 - disabled, 1 - enabled)
  * **adis_write_bias_corr_en_za**, **adis_read_bias_corr_en_za** - to write/read the Z-axis accelerometer bias correction enable bit (0 - disabled, 1 - enabled)

FIFO Configuration
^^^^^^^^^^^^^^^^^^

Some devices offer a hardware FIFO and offer configuration capabilities for
the FIFO. See the information below to view the FIFO configuration APIs for the
devices which have a hardware FIFO.

**ADIS1646X**

	This device does not offer a hardware FIFO.

**ADIS1647X**

	This device does not offer a hardware FIFO.

**ADIS1650X**

	This device does not offer a hardware FIFO.

**ADIS1657X**

  * **adis_write_fifo_en**, **adis_read_fifo_en** - to write/read the FIFO enable bit (0 - direct output mode, 1 - FIFO output mode)
  * **adis_write_fifo_overflow**, **adis_read_fifo_overflow** - to write/read the FIFO overflow behavior bit (0 - stop enqueuing samples, 1 - overwrite the oldest sample)
  * **adis_write_fifo_wm_int_en**, **adis_read_fifo_wm_int_en** - to write/read the FIFO watermark interrupt enable bit (0 - watermark interrupt disabled, 1 - watermark interrupt enabled)
  * **adis_write_fifo_wm_int_pol**, **adis_read_fifo_wm_int_pol** - to write/read the FIFO watermark interrupt polarity (0 - active low, 1 - active high)
  * **adis_write_fifo_wm_lvl**, **adis_read_fifo_wm_lvl** - to write/read the number of samples which must be enqueued into the FIFO to trigger the watermark interrupt
  * **adis_read_fifo_cnt** - to read the current number of samples in the FIFO

Miscellaneous configuration
^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Common**

  * **adis_write_dr_polarity**, **adis_read_dr_polarity** - data ready polarity encoded value write/read APIs
  * **adis_write_sync_polarity**, **adis_read_sync_polarity** - sync polarity encoded value write/read APIs
  * **adis_write_sync_mode**, **adis_read_sync_mode** - synchronization mode encoded value write/read APIs
  * **adis_write_pt_of_perc_algnmt**, **adis_read_pt_of_perc_algnmt** - write/read APIs for point of percussion alignment enable bit (1 -enabled, 0 - disabled)
  * **adis_write_linear_accl_comp**, **adis_read_linear_accl_comp** - write/read APIs for linear acceleration compensation enable bit (1 -enabled, 0 - disabled)

**ADIS1646X**

	There are no other specific APIs for miscellaneous configuration for this chip version.

**ADIS1647X**

  * **adis_write_burst_sel**, **adis_read_burst_sel** - write/read APIs for burst selection encoded value (0 - acceleration and angular velocity, 1 - delta velocity and delta angle)
  * **adis_write_burst32**, **adis_read_burst32** - write/read APIs for burst32 enable bit (0 - for 16-bit burst data, 1 - for 32-bit burst data)


**ADIS1650X**

  * **adis_write_sens_bw**, **adis_read_sens_bw** - sensor bandwidth encoded value write/read APIs
  * **adis_write_burst_sel**, **adis_read_burst_sel** - write/read APIs for burst selection encoded value (0 - acceleration and angular velocity, 1 - delta velocity and delta angle)
  * **adis_write_burst32**, **adis_read_burst32** - write/read APIs for burst32 enable bit (0 - for 16-bit burst data, 1 - for 32-bit burst data)


**ADIS1657X**

  * **adis_write_sens_bw**, **adis_read_sens_bw** - sensor bandwidth encoded value write/read APIs
  * **adis_write_burst_sel**, **adis_read_burst_sel** - write/read APIs for burst selection encoded value (0 - acceleration and angular velocity, 1 - delta velocity and delta angle)
  * **adis_write_burst32**, **adis_read_burst32** - write/read APIs for burst32 enable bit (0 - for 16-bit burst data, 1 - for 32-bit burst data)
  * **adis_write_timestamp32**, **adis_read_timestamp32** - write/read APIs for timestamp32 enable bit (0 - for 16-bit timestamp, 1 - for 32-bit timestamp)
  * **adis_write_sync_4khz**, **adis_read_sync_4khz** - write/read APIs for 4KHz internal sync enable bit (0 - for 2KHz internal sync, 1 - for 4KHz internal sync)

ADIS Commands
^^^^^^^^^^^^^

**Common**

  * **adis_cmd_fact_calib_restore** - to perform factory calibration restore command
  * **adis_cmd_snsr_self_test** - to perform sensor self test command
  * **adis_cmd_fls_mem_update** - to perform flash memory update command
  * **adis_cmd_fls_mem_test** - to perform flash memory test command
  * **adis_cmd_sw_res** - to perform software reset command

**ADIS1646X**

  * **adis_cmd_bias_corr_update** - to perform bias correction update command

**ADIS1647X**

  * **adis_cmd_bias_corr_update** - to perform bias correction update command

**ADIS1650X**

	There are no other specific APIs for ADIS commands for this chip version.

**ADIS1657X**

  * **adis_cmd_bias_corr_update** - to perform bias correction update command
  * **adis_cmd_fifo_flush** - to perform fifo flush command

ADIS read-only registers
------------------------

**Common**

  * **adis_read_time_stamp** - reads the current sample time stamp
  * **adis_read_data_cntr** - reads the current sample data counter

**ADIS1650X**

	There are no other APIs for this chip version.

**ADIS1657X**

  * **adis_read_spi_chksum** - reads current sample SPI transaction checksum

ADIS scratch pad registers
--------------------------

In order to perform read/write operations for device scratch pad registers use
the following APIs:

* **adis_write_usr_scr_1**, **adis_read_usr_scr_1** - write/read APIs for scratch pad register 1
* **adis_write_usr_scr_2**, **adis_read_usr_scr_2** - write/read APIs for scratch pad register 2
* **adis_write_usr_scr_3**, **adis_read_usr_scr_3** - write/read APIs for scratch pad register 3

ADIS Driver Initialization Example
----------------------------------

ADIS1646X
^^^^^^^^^

.. code-block:: c

	struct no_os_spi_init_param adis1646x_spi_ip = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = SPI_BAUDRATE,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.mode = NO_OS_SPI_MODE_3,
		.platform_ops = SPI_OPS,
		.chip_select = SPI_CS,
		.extra = SPI_EXTRA,
	};

	struct no_os_gpio_init_param adis1646x_gpio_reset_ip = {
		.port = GPIO_RESET_PORT_NUM,
		.number = GPIO_RESET_PIN_NUM,
		.pull = NO_OS_PULL_NONE,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA
	};

	struct adis_init_param adis1646x_ip = {
		.gpio_reset = &adis1646x_gpio_reset_ip,
		.sync_mode = ADIS_SYNC_OUTPUT,
		.dev_id = ADIS16465_1,
	};

	struct adis_dev *adis1646x_desc;
	int ret;
	int val[7];

	adis1646x_chip_info.ip = &adis1646x_ip;
	ret = adis_init(&adis1646x_desc, &adis1646x_chip_info);
	if (ret)
		goto error;

	ret = adis_read_x_gyro(adis1646x_desc, &val[0]);
	if (ret)
		goto error;
	ret = adis_read_y_gyro(adis1646x_desc, &val[1]);
	if (ret)
		goto error;
	ret = adis_read_z_gyro(adis1646x_desc, &val[2]);
	if (ret)
		goto error;
	ret = adis_read_x_accl(adis1646x_desc, &val[3]);
	if (ret)
		goto error;
	ret = adis_read_y_accl(adis1646x_desc, &val[4]);
	if (ret)
		goto error;
	ret = adis_read_z_accl(adis1646x_desc, &val[5]);
	if (ret)
		goto error;
	ret = adis_read_temp_out(adis1646x_desc, &val[6]);
	if (ret)
		goto error;

	error:
		adis_remove(adis1646x_desc);
		pr_info("Error!\n");
	...

ADIS1647X
^^^^^^^^^

.. code-block:: c

	struct no_os_spi_init_param adis1647x_spi_ip = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = SPI_BAUDRATE,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.mode = NO_OS_SPI_MODE_3,
		.platform_ops = SPI_OPS,
		.chip_select = SPI_CS,
		.extra = SPI_EXTRA,
	};

	struct no_os_gpio_init_param adis1647x_gpio_reset_ip = {
		.port = GPIO_RESET_PORT_NUM,
		.number = GPIO_RESET_PIN_NUM,
		.pull = NO_OS_PULL_NONE,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA
	};

	struct adis_init_param adis1647x_ip = {
		.gpio_reset = &adis1647x_gpio_reset_ip,
		.sync_mode = ADIS_SYNC_OUTPUT,
		.dev_id = ADIS16477_1,
	};

	struct adis_dev *adis1647x_desc;
	int ret;
	int val[7];

	adis1647x_chip_info.ip = &adis1647x_ip;
	ret = adis_init(&adis1647x_desc, &adis1647x_chip_info);
	if (ret)
		goto error;

	ret = adis_read_x_gyro(adis1647x_desc, &val[0]);
	if (ret)
		goto error;
	ret = adis_read_y_gyro(adis1647x_desc, &val[1]);
	if (ret)
		goto error;
	ret = adis_read_z_gyro(adis1647x_desc, &val[2]);
	if (ret)
		goto error;
	ret = adis_read_x_accl(adis1647x_desc, &val[3]);
	if (ret)
		goto error;
	ret = adis_read_y_accl(adis1647x_desc, &val[4]);
	if (ret)
		goto error;
	ret = adis_read_z_accl(adis1647x_desc, &val[5]);
	if (ret)
		goto error;
	ret = adis_read_temp_out(adis1647x_desc, &val[6]);
	if (ret)
		goto error;

	error:
		adis_remove(adis1647x_desc);
		pr_info("Error!\n");
	...

ADIS1650X
^^^^^^^^^

.. code-block:: c

	struct no_os_spi_init_param adis1650x_spi_ip = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = SPI_BAUDRATE,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.mode = NO_OS_SPI_MODE_3,
		.platform_ops = SPI_OPS,
		.chip_select = SPI_CS,
		.extra = SPI_EXTRA,
	};

	struct no_os_gpio_init_param adis1650x_gpio_reset_ip = {
		.port = GPIO_RESET_PORT_NUM,
		.number = GPIO_RESET_PIN_NUM,
		.pull = NO_OS_PULL_NONE,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA
	};

	struct adis_init_param adis1650x_ip = {
		.gpio_reset = &adis1650x_gpio_reset_ip,
		.sync_mode = ADIS_SYNC_OUTPUT,
		.dev_id = ADIS16505_2,
	};

	struct adis_dev *adis1650x_desc;
	int ret;
	int val[7];

	adis1650x_chip_info.ip = &adis1650x_ip;
	ret = adis_init(&adis1650x_desc, &adis1650x_chip_info);
	if (ret)
		goto error;

	ret = adis_read_x_gyro(adis1650x_desc, &val[0]);
	if (ret)
		goto error;
	ret = adis_read_y_gyro(adis1650x_desc, &val[1]);
	if (ret)
		goto error;
	ret = adis_read_z_gyro(adis1650x_desc, &val[2]);
	if (ret)
		goto error;
	ret = adis_read_x_accl(adis1650x_desc, &val[3]);
	if (ret)
		goto error;
	ret = adis_read_y_accl(adis1650x_desc, &val[4]);
	if (ret)
		goto error;
	ret = adis_read_z_accl(adis1650x_desc, &val[5]);
	if (ret)
		goto error;
	ret = adis_read_temp_out(adis1650x_desc, &val[6]);
	if (ret)
		goto error;

	error:
		adis_remove(adis1650x_desc);
		pr_info("Error!\n");
	...

ADIS1654X
^^^^^^^^^

.. code-block:: c

	struct no_os_spi_init_param adis1654x_spi_ip = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = SPI_BAUDRATE,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.mode = NO_OS_SPI_MODE_3,
		.platform_ops = SPI_OPS,
		.chip_select = SPI_CS,
		.extra = SPI_EXTRA,
	};

	struct no_os_gpio_init_param adis1654x_gpio_reset_ip = {
		.port = GPIO_RESET_PORT_NUM,
		.number = GPIO_RESET_PIN_NUM,
		.pull = NO_OS_PULL_NONE,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA
	};

	struct adis_init_param adis1654x_ip = {
		.gpio_reset = &adis1654x_gpio_reset_ip,
		.sync_mode = ADIS_SYNC_DEFAULT,
		.dev_id = ADIS16545_3,
	};

	struct adis_dev *adis1654x_desc;
	int ret;
	int val[7];

	adis1654x_chip_info.ip = &adis1654x_ip;
	ret = adis_init(&adis1654x_desc, &adis1654x_chip_info);
	if (ret)
		goto error;

	ret = adis_read_x_gyro(adis1654x_desc, &val[0]);
	if (ret)
		goto error;
	ret = adis_read_y_gyro(adis1654x_desc, &val[1]);
	if (ret)
		goto error;
	ret = adis_read_z_gyro(adis1654x_desc, &val[2]);
	if (ret)
		goto error;
	ret = adis_read_x_accl(adis1654x_desc, &val[3]);
	if (ret)
		goto error;
	ret = adis_read_y_accl(adis1654x_desc, &val[4]);
	if (ret)
		goto error;
	ret = adis_read_z_accl(adis1654x_desc, &val[5]);
	if (ret)
		goto error;
	ret = adis_read_temp_out(adis1654x_desc, &val[6]);
	if (ret)
		goto error;

	error:
		adis_remove(adis1654x_desc);
		pr_info("Error!\n");

ADIS1655X
^^^^^^^^^

.. code-block:: c

	struct no_os_spi_init_param adis1655x_spi_ip = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = SPI_BAUDRATE,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.mode = NO_OS_SPI_MODE_3,
		.platform_ops = SPI_OPS,
		.chip_select = SPI_CS,
		.extra = SPI_EXTRA,
	};

	struct no_os_gpio_init_param adis1655x_gpio_reset_ip = {
		.port = GPIO_RESET_PORT_NUM,
		.number = GPIO_RESET_PIN_NUM,
		.pull = NO_OS_PULL_NONE,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA
	};

	struct adis_init_param adis1655x_ip = {
		.gpio_reset = &adis1655x_gpio_reset_ip,
		.sync_mode = ADIS_SYNC_DEFAULT,
		.dev_id = ADIS16550,
	};

	struct adis_dev *adis1655x_desc;
	int ret;
	int val[7];

	adis1655x_chip_info.ip = &adis1655x_ip;
	ret = adis_init(&adis1655x_desc, &adis1655x_chip_info);
	if (ret)
		goto error;

	ret = adis_read_x_gyro(adis1655x_desc, &val[0]);
	if (ret)
		goto error;
	ret = adis_read_y_gyro(adis1655x_desc, &val[1]);
	if (ret)
		goto error;
	ret = adis_read_z_gyro(adis1655x_desc, &val[2]);
	if (ret)
		goto error;
	ret = adis_read_x_accl(adis1655x_desc, &val[3]);
	if (ret)
		goto error;
	ret = adis_read_y_accl(adis1655x_desc, &val[4]);
	if (ret)
		goto error;
	ret = adis_read_z_accl(adis1655x_desc, &val[5]);
	if (ret)
		goto error;
	ret = adis_read_temp_out(adis1655x_desc, &val[6]);
	if (ret)
		goto error;

	error:
		adis_remove(adis1655x_desc);
		pr_info("Error!\n");
	...

ADIS1657X
^^^^^^^^^

.. code-block:: c

	struct no_os_spi_init_param adis1657x_spi_ip = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = SPI_BAUDRATE,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.mode = NO_OS_SPI_MODE_3,
		.platform_ops = SPI_OPS,
		.chip_select = SPI_CS,
		.extra = SPI_EXTRA,
	};

	struct no_os_gpio_init_param adis1657x_gpio_reset_ip = {
		.port = GPIO_RESET_PORT_NUM,
		.number = GPIO_RESET_PIN_NUM,
		.pull = NO_OS_PULL_NONE,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA
	};

	struct adis_init_param adis1657x_ip = {
		.gpio_reset = &adis1657x_gpio_reset_ip,
		.sync_mode = ADIS_SYNC_OUTPUT,
		.dev_id = ADIS16577_3,
	};

	struct adis_dev *adis1657x_desc;
	int ret;
	int val[7];

	adis1657x_chip_info.ip = &adis1657x_ip;
	ret = adis_init(&adis1657x_desc, &adis1657x_chip_info);
	if (ret)
		goto error;

	ret = adis_read_x_gyro(adis1657x_desc, &val[0]);
	if (ret)
		goto error;
	ret = adis_read_y_gyro(adis1657x_desc, &val[1]);
	if (ret)
		goto error;
	ret = adis_read_z_gyro(adis1657x_desc, &val[2]);
	if (ret)
		goto error;
	ret = adis_read_x_accl(adis1657x_desc, &val[3]);
	if (ret)
		goto error;
	ret = adis_read_y_accl(adis1657x_desc, &val[4]);
	if (ret)
		goto error;
	ret = adis_read_z_accl(adis1657x_desc, &val[5]);
	if (ret)
		goto error;
	ret = adis_read_temp_out(adis1657x_desc, &val[6]);
	if (ret)
		goto error;

	error:
		adis_remove(adis1657x_desc);
		pr_info("Error!\n");
	...

ADIS IIO Driver Source Code
---------------------------

The IIO ADIS driver comes on top of ADIS driver and offers support for interfacing
IIO clients through IIO lib.

The source code for ADIS generic driver can be found here:

* `Header file of ADIS IIO Driver Internals <https://github.com/analogdevicesinc/no-OS/blob/main/drivers/imu/iio_adis_internals.h>`_
* `Implementation of ADIS Generic IIO Driver <https://github.com/analogdevicesinc/no-OS/blob/main/drivers/imu/iio_adis.c>`_

The generic ADIS driver has to be used together with the chip-specific ADIS driver.
The source code for the supported chips is listed below:

ADIS1646X IIO Driver Source Code:

* `Header file of ADIS1646X IIO Driver <https://github.com/analogdevicesinc/no-OS/blob/main/drivers/imu/iio_adis1646x.h>`_
* `Implementation of ADIS1646X IIO Driver <https://github.com/analogdevicesinc/no-OS/blob/main/drivers/imu/iio_adis1646x.c>`_

Supported devices with IIO ADIS1646X files:

* `ADIS16465 <https://www.analog.com/ADIS16465>`_
* `ADIS16467 <https://www.analog.com/ADIS16467>`_
* `ADIS16470 <https://www.analog.com/ADIS16470>`_
* `ADIS16475 <https://www.analog.com/ADIS16475>`_

ADIS1647X IIO Driver Source Code:

* `Header file of ADIS1647X IIO Driver <https://github.com/analogdevicesinc/no-OS/blob/main/drivers/imu/iio_adis1647x.h>`_
* `Implementation of ADIS1647X IIO Driver <https://github.com/analogdevicesinc/no-OS/blob/main/drivers/imu/iio_adis1647x.c>`_

Supported devices with IIO ADIS1646X files:

* `ADIS16477 <https://www.analog.com/ADIS16477>`_

ADIS1650X IIO Driver Source Code:

* `Header file of ADIS1650X IIO Driver <https://github.com/analogdevicesinc/no-OS/blob/main/drivers/imu/iio_adis1650x.h>`_
* `Implementation of ADIS1650X IIO Driver <https://github.com/analogdevicesinc/no-OS/blob/main/drivers/imu/iio_adis1650x.c>`_

Supported devices with IIO ADIS1650X files:

* `ADIS16500 <https://www.analog.com/ADIS16500>`_
* `ADIS16501 <https://www.analog.com/ADIS16501>`_
* `ADIS16505 <https://www.analog.com/ADIS16505>`_
* `ADIS16507 <https://www.analog.com/ADIS16507>`_

ADIS1654X IIO Driver Source Code:

* `Header file of ADIS1654X IIO Driver <https://github.com/analogdevicesinc/no-OS/blob/main/drivers/imu/iio_adis1654x.h>`_
* `Implementation of ADIS1654X IIO Driver <https://github.com/analogdevicesinc/no-OS/blob/main/drivers/imu/iio_adis1654x.c>`_

Supported devices with IIO ADIS1654X files:

* `ADIS16545 <https://www.analog.com/ADIS16545>`_
* `ADIS16547 <https://www.analog.com/ADIS16547>`_

ADIS1655X IIO Driver Source Code:

* `Header file of ADIS1655X IIO Driver <https://github.com/analogdevicesinc/no-OS/blob/main/drivers/imu/iio_adis1655x.h>`_
* `Implementation of ADIS1655X IIO Driver <https://github.com/analogdevicesinc/no-OS/blob/main/drivers/imu/iio_adis1655x.c>`_

Supported devices with IIO ADIS1655X files:

* `ADIS16550 <https://www.analog.com/ADIS16550>`_

ADIS1657X IIO Driver Source Code:

* `Header file of ADIS1657X IIO Driver <https://github.com/analogdevicesinc/no-OS/blob/main/drivers/imu/iio_adis1657x.h>`_
* `Implementation of ADIS1657X IIO Driver <https://github.com/analogdevicesinc/no-OS/blob/main/drivers/imu/iio_adis1657x.c>`_

Supported devices with ADIS1657X files:

* `ADIS16575 <https://www.analog.com/ADIS16575>`_
* `ADIS16576 <https://www.analog.com/ADIS16576>`_
* `ADIS16577 <https://www.analog.com/ADIS16577>`_

IIO ADIS Device Configuration
-----------------------------

Device Attributes
^^^^^^^^^^^^^^^^^

The generic IIO ADIS device has the following device specific attributes:

* **filter_low_pass_3db_frequency** - which allows the configuration of the ADIS Bartlett window FIR filter
* **sampling_frequency** - which allows the configuration of the ADIS sampling frequency

Device Channels
^^^^^^^^^^^^^^^

The generic IIO ADIS device has 0 output channels and 14 input channels:
3 angular velocity channels, 3 acceleration channels, 3 rotation channels,
3 velocity channels, 1 temperature channel and 1 counter channel.

**Angular Velocity Channels**

	The angular velocity channels are:

	* Channel 0: **anglvel_x**
	* Channel 1: **anglvel_y**
	* Channel 2: **anglvel_z**

	Each angular velocity channel has 3 attributes:

	* **calibbias** - calibration offset correction
	* **raw** - the raw angular velocity value read from the device
	* **scale** - the scale that has to be applied to the raw value in order to obtain the converted real value in rot/s, it has a constant value which is chip-specific

**Acceleration Channels**

	The acceleration channels are:

	* Channel 3: **accel_x**
	* Channel 4: **accel_y**
	* Channel 5: **accel_z**

	Each acceleration channel has 3 attributes:

	* **calibbias** - calibration offset correction
	* **raw** - the raw acceleration value read from the device
	* **scale** - the scale that has to be applied to the raw value in order to obtain the converted real value in m/s^2, it has a constant value which is chip-specific

**Temperature Channel**

	The temperature channel is:

	* Channel 6: **temp0**

	The temperature channel has 2 attributes:

	* **raw** - the raw temperature value read from the device
	* **scale** - the scale that has to be applied to the raw value in order to obtain the converted real value in millidegrees Celsius, it has a constant value which is chip-specific

**Delta Angle Channels**

	The delta angle channels are:

	* Channel 7: **deltaangl_x**
	* Channel 8: **deltaangl_y**
	* Channel 9: **deltaangl_z**

	Each rotation channel has 2 attributes:

	* **raw** - the raw delta angle value read from the device
	* **scale** - the scale that has to be applied to the raw value in order to obtain the converted real value in radians, it has a constant value which is chip-specific

**Delta Velocity Channels**

	The delta velocity channels are:

	* Channel 10: **deltavelocity_x**
	* Channel 11: **deltavelocity_y**
	* Channel 12: **deltavelocity_z**

	Each delta velocity channel has 2 attributes:

	* **raw** - the raw delta velocity value read from the device
	* **scale** - the scale that has to be applied to the raw value in order to obtain the converted real value in m/s, it has a constant value which is chip-specific

Device Debug Attributes
^^^^^^^^^^^^^^^^^^^^^^^

The IIO driver offers the possibility to configure the device and to retrieve
diagnosis and configuration data from the device using debug attributes.
The following list of debug attributes is available:

**Common**

+----------------------------------------------+-------------+--------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| Debug Attribute Name                         | Access Type | Debug Attribute Description                                        | Debug Attribute Valid Values                                                                                                                |
+----------------------------------------------+-------------+--------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| diag_data_path_overrun                       | Read-only   | Data Path Overrun Error Flag                                       | 0 - error did not occur or 1 - error occurred                                                                                               |
+----------------------------------------------+-------------+--------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| diag_flash_memory_update_error               | Read-only   | Flash Memory Update Error Flag                                     | 0 - error did not occur or 1 - error occurred                                                                                               |
+----------------------------------------------+-------------+--------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| diag_spi_communication_error                 | Read-only   | SPI Communication Error Flag                                       | 0 - error did not occur or 1 - error occurred                                                                                               |
+----------------------------------------------+-------------+--------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| diag_standby_mode                            | Read-only   | Standby Mode Flag                                                  | 0 - device is in processing mode, 1 - device is in standby mode (not enough voltage supplied)                                               |
+----------------------------------------------+-------------+--------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| diag_sensor_self_test_error                  | Read-only   | Sensor Self Test Error Flag                                        | 0 - error did not occur or 1 - error occurred                                                                                               |
+----------------------------------------------+-------------+--------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| diag_flash_memory_test_error                 | Read-only   | Flash Memory Test Error Flag                                       | 0 - error did not occur or 1 - error occurred                                                                                               |
+----------------------------------------------+-------------+--------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| diag_clock_error                             | Read-only   | Clock Error Flag                                                   | 0 - error did not occur or 1 - error occurred                                                                                               |
+----------------------------------------------+-------------+--------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| diag_checksum_error_flag                     | Read-only   | SPI Checksum Error Flag                                            | 0 - error did not occur or 1 - error occurred                                                                                               |
+----------------------------------------------+-------------+--------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| diag_flash_memory_write_count_exceeded_error | Read-only   | Flash Memory Write Counts Exceeded Flag Error                      | 0 - error did not occur or 1 - error occurred                                                                                               |
+----------------------------------------------+-------------+--------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| lost_samples_count                           | Read-only   | The number of lost samples during the previous buffer read command | 0 - 4294967295                                                                                                                              |
+----------------------------------------------+-------------+--------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| time_stamp                                   | Read-only   | The TIME_STAMP register value                                      | 0 - 65535                                                                                                                                   |
+----------------------------------------------+-------------+--------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| data_counter                                 | Read-only   | The DATA_CNTR register value                                       | 0 - 65535                                                                                                                                   |
+----------------------------------------------+-------------+--------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| filter_size                                  | Read/Write  | The FILT_CTRL register value                                       | 0 - 6                                                                                                                                       |
+----------------------------------------------+-------------+--------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| gyroscope_measurement_range                  | Read-only   | The measurement range identifier                                   | chip specific value with format "+/-###_degrees_per_sec"                                                                                    |
+----------------------------------------------+-------------+--------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| data_ready_polarity Read/Write               | Read/Write  | Data Ready Pin Polarity Encoded Value                              | 0 - active low, 1 - active high                                                                                                             |
+----------------------------------------------+-------------+--------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| sync_polarity                                | Read/Write  | Sync Pin Polarity Encoded Value                                    | 0 - active low, 1 - active high                                                                                                             |
+----------------------------------------------+-------------+--------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| point_of_percussion_alignment                | Read/Write  | Point Of Percussion Alignment Enable Bit                           | 0 - disabled, 1 - enabled                                                                                                                   |
+----------------------------------------------+-------------+--------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| linear_acceleration_compensation             | Read/Write  | Linear Acceleration Compensation Enable Bit                        | 0 - disabled, 1 - enabled                                                                                                                   |
+----------------------------------------------+-------------+--------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| sync_signal_scale                            | Read/Write  | Sync Input Frequency Multiplier Register Value                     | 0 - 65535                                                                                                                                   |
+----------------------------------------------+-------------+--------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| factory_calibration_restore                  | Write-only  | Triggers a factory calibration restore command                     | Any written value will trigger a factory calibration restore command on the device                                                          |
+----------------------------------------------+-------------+--------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| sensor_self_test                             | Write-only  | Triggers a self test command                                       | Any written value will trigger a self test command on the device                                                                            |
+----------------------------------------------+-------------+--------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| flash_memory_update                          | Write-only  | Triggers a flash memory update command                             | Any written value will trigger a flash memory update command on the device                                                                  |
+----------------------------------------------+-------------+--------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| flash_memory_test                            | Write-only  | Triggers a flash memory test command                               | Any written value will trigger a flash memory test command on the device                                                                    |
+----------------------------------------------+-------------+--------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| software_reset                               | Write-only  | Triggers a software reset command                                  | Any written value will trigger a software reset command on the device                                                                       |
+----------------------------------------------+-------------+--------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| firmware_revision                            | Read-only   | The firmware revision value                                        | String containing the firmware revision in the following format ##.##                                                                       |
+----------------------------------------------+-------------+--------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| firmware_date                                | Read-only   | The firmware date                                                  | String containing the firmware date in the following format mm-dd-yyyy                                                                      |
+----------------------------------------------+-------------+--------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| product_id                                   | Read-only   | The product id                                                     | Chip specific product id, e.g. 16505, 16575, 16576, 16577, etc.)                                                                            |
+----------------------------------------------+-------------+--------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| serial_number                                | Read-only   | The serial number                                                  | The serial number of the chip - hexadecimal format                                                                                          |
+----------------------------------------------+-------------+--------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| scratch_pad_register1                        | Read/Write  | The scratch path register 1                                        | 0 - 65535                                                                                                                                   |
+----------------------------------------------+-------------+--------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| scratch_pad_register2                        | Read/Write  | The scratch path register 2                                        | 0 - 65535                                                                                                                                   |
+----------------------------------------------+-------------+--------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| scratch_pad_register3                        | Read/Write  | The scratch path register 3                                        | 0 - 65535                                                                                                                                   |
+----------------------------------------------+-------------+--------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| flash_count                                  | Read-only   | The number of the flash writes performed on the device             | 0 - 65535                                                                                                                                   |
+----------------------------------------------+-------------+--------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+

**ADIS1646X**

+---------------------------------------------+-------------+-------------------------------------------------------+--------------------------------------------------------------------------------------------+
| Debug Attribute Name                        | Access Type | Debug Attribute Description                           | Debug Attribute Valid Values                                                               |
+---------------------------------------------+-------------+-------------------------------------------------------+--------------------------------------------------------------------------------------------+
| bias_correction_time_base_control           | Read/Write  | Bias Correction Time Base Control Value               | 0 - 12                                                                                     |
+---------------------------------------------+-------------+-------------------------------------------------------+--------------------------------------------------------------------------------------------+
| x_axis_gyroscope_bias_correction_enable     | Read/Write  | X Axis Gyroscope Bias Correction Enable Bit Value     | 0 - correction disabled, 1 - correction enabled                                            |
+---------------------------------------------+-------------+-------------------------------------------------------+--------------------------------------------------------------------------------------------+
| y_axis_gyroscope_bias_correction_enable     | Read/Write  | Y Axis Gyroscope Bias Correction Enable Bit Value     | 0 - correction disabled, 1 - correction enabled                                            |
+---------------------------------------------+-------------+-------------------------------------------------------+--------------------------------------------------------------------------------------------+
| z_axis_gyroscope_bias_correction_enable     | Read/Write  | Z Axis Gyroscope Bias Correction Enable Bit Value     | 0 - correction disabled, 1 - correction enabled                                            |
+---------------------------------------------+-------------+-------------------------------------------------------+--------------------------------------------------------------------------------------------+
| x_axis_accelerometer_bias_correction_enable | Read/Write  | X Axis Accelerometer Bias Correction Enable Bit Value | 0 - correction disabled, 1 - correction enabled                                            |
+---------------------------------------------+-------------+-------------------------------------------------------+--------------------------------------------------------------------------------------------+
| y_axis_accelerometer_bias_correction_enable | Read/Write  | Y Axis Accelerometer Bias Correction Enable Bit Value | 0 - correction disabled, 1 - correction enabled                                            |
+---------------------------------------------+-------------+-------------------------------------------------------+--------------------------------------------------------------------------------------------+
| z_axis_accelerometer_bias_correction_enable | Read/Write  | Z Axis Accelerometer Bias Correction Enable Bit Value | 0 - correction disabled, 1 - correction enabled                                            |
+---------------------------------------------+-------------+-------------------------------------------------------+--------------------------------------------------------------------------------------------+
| bias_correction_update                      | Write-only  | Trigger a bias correction update command              | Any written value will trigger a bias correction update command on the device              |
+---------------------------------------------+-------------+-------------------------------------------------------+--------------------------------------------------------------------------------------------+
| decimation_filter                           | Read/Write  | Decimation Filter Register Value                      | 0 - 1999                                                                                   |
+---------------------------------------------+-------------+-------------------------------------------------------+--------------------------------------------------------------------------------------------+
| sync_mode_select                            | Read/Write  | Sync Mode Select Encoded Value                        | 0 - internal sync, 1 - direct input sync, 2 - scaled sync, 3 - output sync, 5 - pulse sync |
+---------------------------------------------+-------------+-------------------------------------------------------+--------------------------------------------------------------------------------------------+

**ADIS1647X**

+---------------------------------------------+-------------+-------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| Debug Attribute Name                        | Access Type | Debug Attribute Description                           | Debug Attribute Valid Values                                                                                                                |
+---------------------------------------------+-------------+-------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| bias_correction_time_base_control           | Read/Write  | Bias Correction Time Base Control Value               | 0 - 12                                                                                                                                      |
+---------------------------------------------+-------------+-------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| x_axis_gyroscope_bias_correction_enable     | Read/Write  | X Axis Gyroscope Bias Correction Enable Bit Value     | 0 - correction disabled, 1 - correction enabled                                                                                             |
+---------------------------------------------+-------------+-------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| y_axis_gyroscope_bias_correction_enable     | Read/Write  | Y Axis Gyroscope Bias Correction Enable Bit Value     | 0 - correction disabled, 1 - correction enabled                                                                                             |
+---------------------------------------------+-------------+-------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| z_axis_gyroscope_bias_correction_enable     | Read/Write  | Z Axis Gyroscope Bias Correction Enable Bit Value     | 0 - correction disabled, 1 - correction enabled                                                                                             |
+---------------------------------------------+-------------+-------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| x_axis_accelerometer_bias_correction_enable | Read/Write  | X Axis Accelerometer Bias Correction Enable Bit Value | 0 - correction disabled, 1 - correction enabled                                                                                             |
+---------------------------------------------+-------------+-------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| y_axis_accelerometer_bias_correction_enable | Read/Write  | Y Axis Accelerometer Bias Correction Enable Bit Value | 0 - correction disabled, 1 - correction enabled                                                                                             |
+---------------------------------------------+-------------+-------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| z_axis_accelerometer_bias_correction_enable | Read/Write  | Z Axis Accelerometer Bias Correction Enable Bit Value | 0 - correction disabled, 1 - correction enabled                                                                                             |
+---------------------------------------------+-------------+-------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| bias_correction_update                      | Write-only  | Trigger a bias correction update command              | Any written value will trigger a bias correction update command on the device                                                               |
+---------------------------------------------+-------------+-------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| decimation_filter                           | Read/Write  | Decimation Filter Register Value                      | 0 - 1999                                                                                                                                    |
+---------------------------------------------+-------------+-------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| burst_data_selection                        | Read/Write  | Burst Data Selection Encoded Bit                      | 0 - burst data contains acceleration and angular velocity measurements, 1 - burst data contains delta-angle and delta-velocity measurements |
+---------------------------------------------+-------------+-------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| burst_size_selection                        | Read/Write  | Burst Size Selection Encoded Bit                      | 0 - burst data contains 16-bit values, 1 - burst data contains 32-bit values                                                                |
+---------------------------------------------+-------------+-------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| sync_mode_select                            | Read/Write  | Sync Mode Select Encoded Value                        | 0 - internal sync, 1 - direct input sync, 2 - scaled sync, 3 - output sync, 5 - pulse sync                                                  |
+---------------------------------------------+-------------+-------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+

**ADIS1650X**

+-----------------------------------+-------------+-----------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| Debug Attribute Name              | Access Type | Debug Attribute Description             | Debug Attribute Valid Values                                                                                                                |
+-----------------------------------+-------------+-----------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| diag_gyroscope1_self_test_error   | Read-only   | Gyroscope 1 Self Test Error Fla         | 0 - error did not occur or 1 - error occurred                                                                                               |
+-----------------------------------+-------------+-----------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| diag_gyroscope1_self_test_error   | Read-only   | Gyroscope 1 Self Test Error Fla         | 0 - error did not occur or 1 - error occurred                                                                                               |
+-----------------------------------+-------------+-----------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| diag_acceleration_self_test_error | Read-only   | Accelerometer Self Test Error Flag      | 0 - error did not occur or 1 - error occurred                                                                                               |
+-----------------------------------+-------------+-----------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| decimation_filter                 | Read/Write  | Decimation Filter Register Value        | 0 - 1999                                                                                                                                    |
+-----------------------------------+-------------+-----------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| burst_data_selection              | Read/Write  | Burst Data Selection Encoded Bit        | 0 - burst data contains acceleration and angular velocity measurements, 1 - burst data contains delta-angle and delta-velocity measurements |
+-----------------------------------+-------------+-----------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| burst_size_selection              | Read/Write  | Burst Size Selection Encoded Bit        | 0 - burst data contains 16-bit values, 1 - burst data contains 32-bit values                                                                |
+-----------------------------------+-------------+-----------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| sync_mode_select                  | Read/Write  | Sync Mode Select Encoded Value          | 0 - internal sync, 1 - direct input sync, 2 - scaled sync, 3 - output sync                                                                  |
+-----------------------------------+-------------+-----------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| internal_sensor_bandwidth         | Read/Write  | Internal Sensor Bandwidth Encoded Value | 0 - wide bandwidth, 1 - 370 Hz                                                                                                              |
+-----------------------------------+-------------+-----------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+

**ADIS1657X**

+---------------------------------------------+-------------+-------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| Debug Attribute Name                        | Access Type | Debug Attribute Description                           | Debug Attribute Valid Values                                                                                                                |
+---------------------------------------------+-------------+-------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| diag_sensor_initialization_failure          | Read-only   | Sensor Initialization Failure Flag                    | 0 - error did not occur or 1 - error occurred                                                                                               |
+---------------------------------------------+-------------+-------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| diag_x_axis_gyroscope_failure               | Read-only   | X Axis Gyroscope Failure Flag                         | 0 - error did not occur or 1 - error occurred                                                                                               |
+---------------------------------------------+-------------+-------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| diag_y_axis_gyroscope_failure               | Read-only   | Y Axis Gyroscope Failure Flag                         | 0 - error did not occur or 1 - error occurred                                                                                               |
+---------------------------------------------+-------------+-------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| diag_z_axis_gyroscope_failure               | Read-only   | Z Axis Gyroscope Failure Flag                         | 0 - error did not occur or 1 - error occurred                                                                                               |
+---------------------------------------------+-------------+-------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| diag_x_axis_accelerometer_failure           | Read-only   | X Axis Accelerometer Failure Flag                     | 0 - error did not occur or 1 - error occurred                                                                                               |
+---------------------------------------------+-------------+-------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| diag_y_axis_accelerometer_failure           | Read-only   | Y Axis Accelerometer Failure Flag                     | 0 - error did not occur or 1 - error occurred                                                                                               |
+---------------------------------------------+-------------+-------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| diag_z_axis_accelerometer_failure           | Read-only   | Z Axis Accelerometer Failure Flag                     | 0 - error did not occur or 1 - error occurred                                                                                               |
+---------------------------------------------+-------------+-------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| diag_aduc_mcu_fault                         | Read-only   | Internal Mcu Fault Flag                               | 0 - error did not occur or 1 - error occurred                                                                                               |
+---------------------------------------------+-------------+-------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| fifo_sample_count                           | Read-only   | The FIFO_CNT register value                           | 0 - 511                                                                                                                                     |
+---------------------------------------------+-------------+-------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| spi_checksum                                | Read-only   | The SPI_CHKSUM register value                         | 0 - 65535                                                                                                                                   |
+---------------------------------------------+-------------+-------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| fifo_enable                                 | Read/Write  | IFO Enable Bit Value                                  | 0 - FIFO disabled, 1 - FIFO enabled                                                                                                         |
+---------------------------------------------+-------------+-------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| fifo_overflow_behavior                      | Read/Write  | FIFO Overflow Behavior Encoded Value                  | 0 - stop enqueuing samples, 1 - overwrite the oldest sample                                                                                 |
+---------------------------------------------+-------------+-------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| fifo_watermark_interrupt_enable             | Read/Write  | FIFO Watermark Interrupt Enable Bit Value             | 0 - watermark interrupt disabled, 1 - watermark interrupt enabled                                                                           |
+---------------------------------------------+-------------+-------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| fifo_watermark_interrupt_polarity           | Read/Write  | FIFO Watermark Interrupt Polarity Encoded Value       | 0 - active low, 1 - active high                                                                                                             |
+---------------------------------------------+-------------+-------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| fifo_watermark_threshold_level              | Read/Write  | FIFO Watermark Threshold Level                        | 0 - 511                                                                                                                                     |
+---------------------------------------------+-------------+-------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| time_stamp_size                             | Read/Write  | Timestamp Size Encoded Bit                            | 0 - timestamp is in 16-bit format, 1 - timestamp is in 32-bit format                                                                        |
+---------------------------------------------+-------------+-------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| internal_sync_enable_4khz                   | Read/Write  | 4KHz Internal Sync Enable bit                         | 0 - 2KHz Internal Sync, 1 - 4KHz Internal Sync                                                                                              |
+---------------------------------------------+-------------+-------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| bias_correction_time_base_control           | Read/Write  | Bias Correction Time Base Control Value               | 0 - 12                                                                                                                                      |
+---------------------------------------------+-------------+-------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| x_axis_gyroscope_bias_correction_enable     | Read/Write  | X Axis Gyroscope Bias Correction Enable Bit Value     | 0 - correction disabled, 1 - correction enabled                                                                                             |
+---------------------------------------------+-------------+-------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| y_axis_gyroscope_bias_correction_enable     | Read/Write  | Y Axis Gyroscope Bias Correction Enable Bit Value     | 0 - correction disabled, 1 - correction enabled                                                                                             |
+---------------------------------------------+-------------+-------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| z_axis_gyroscope_bias_correction_enable     | Read/Write  | Z Axis Gyroscope Bias Correction Enable Bit Value     | 0 - correction disabled, 1 - correction enabled                                                                                             |
+---------------------------------------------+-------------+-------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| x_axis_accelerometer_bias_correction_enable | Read/Write  | X Axis Accelerometer Bias Correction Enable Bit Value | 0 - correction disabled, 1 - correction enabled                                                                                             |
+---------------------------------------------+-------------+-------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| y_axis_accelerometer_bias_correction_enable | Read/Write  | Y Axis Accelerometer Bias Correction Enable Bit Value | 0 - correction disabled, 1 - correction enabled                                                                                             |
+---------------------------------------------+-------------+-------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| z_axis_accelerometer_bias_correction_enable | Read/Write  | Z Axis Accelerometer Bias Correction Enable Bit Value | 0 - correction disabled, 1 - correction enabled                                                                                             |
+---------------------------------------------+-------------+-------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| bias_correction_update                      | Write-only  | Trigger a bias correction update command              | Any written value will trigger a bias correction update command on the device                                                               |
+---------------------------------------------+-------------+-------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| fifo_flush                                  | Write-only  | Triggers a FIFO flush command                         | Any written value will trigger a FIFO flush command on the device                                                                           |
+---------------------------------------------+-------------+-------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| decimation_filter                           | Read/Write  | Decimation Filter Register Value                      | 0 - 3999                                                                                                                                    |
+---------------------------------------------+-------------+-------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| burst_data_selection                        | Read/Write  | Burst Data Selection Encoded Bit                      | 0 - burst data contains acceleration and angular velocity measurements, 1 - burst data contains delta-angle and delta-velocity measurements |
+---------------------------------------------+-------------+-------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| burst_size_selection                        | Read/Write  | Burst Size Selection Encoded Bit                      | 0 - burst data contains 16-bit values, 1 - burst data contains 32-bit values                                                                |
+---------------------------------------------+-------------+-------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| sync_mode_select                            | Read/Write  | Sync Mode Select Encoded Value                        | 0 - internal sync, 1 - direct input sync, 2 - scaled sync, 3 - output sync                                                                  |
+---------------------------------------------+-------------+-------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+
| internal_sensor_bandwidth                   | Read/Write  | Internal Sensor Bandwidth Encoded Value               | 0 - wide bandwidth, 1 - 370 Hz                                                                                                              |
+---------------------------------------------+-------------+-------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------+

Device Buffers
^^^^^^^^^^^^^^

The IIO AIDS device driver supports the usage of a data buffer for samples reading purposes.
The following channels are available for buffer readings, based on the chip used:

**ADIS1646X**:

* anglvel_x
* anglvel_y
* anglvel_z
* accel_x
* accel_y
* accel_z
* temp0

**ADIS1647X**:

Burst data selection = 0:

  * anglvel_x
  * anglvel_y
  * anglvel_z
  * accel_x
  * accel_y
  * accel_z
  * temp0

Burst data selection = 1:

  * deltaangl_x
  * deltaangl_y
  * deltaangl_z
  * deltavelocity_x
  * deltavelocity_y
  * deltavelocity_z
  * temp0

**ADIS1650X**:

Burst data selection = 0:

  * anglvel_x
  * anglvel_y
  * anglvel_z
  * accel_x
  * accel_y
  * accel_z
  * temp0

Burst data selection = 1:

  * deltaangl_x
  * deltaangl_y
  * deltaangl_z
  * deltavelocity_x
  * deltavelocity_y
  * deltavelocity_z
  * temp0

**ADIS1657X**:

Burst data selection = 0:

  * anglvel_x
  * anglvel_y
  * anglvel_z
  * accel_x
  * accel_y
  * accel_z
  * temp0

Burst data selection = 1:

  * deltaangl_x
  * deltaangl_y
  * deltaangl_z
  * deltavelocity_x
  * deltavelocity_y
  * deltavelocity_z
  * temp0


IIO ADIS Driver Initialization Example
--------------------------------------

ADIS1646X
^^^^^^^^^

.. code-block:: c

	struct no_os_spi_init_param adis1646x_spi_ip = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = SPI_BAUDRATE,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.mode = NO_OS_SPI_MODE_3,
	.platform_ops = SPI_OPS,
	.chip_select = SPI_CS,
	.extra = SPI_EXTRA,
	};

	struct no_os_gpio_init_param adis1646x_gpio_reset_ip = {
		.port = GPIO_RESET_PORT_NUM,
		.number = GPIO_RESET_PIN_NUM,
		.pull = NO_OS_PULL_NONE,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA
	};

	struct adis_init_param adis1646x_ip = {
		.gpio_reset = &adis1646x_gpio_reset_ip,
		.sync_mode = ADIS_SYNC_OUTPUT,
		.dev_id = ADIS16465_1,
	};

	struct no_os_irq_init_param adis1646x_gpio_irq_ip = {
		.irq_ctrl_id = GPIO_IRQ_ID,
		.platform_ops = GPIO_IRQ_OPS,
		.extra = GPIO_IRQ_EXTRA,
	};

	const struct iio_hw_trig_cb_info gpio_cb_info = {
		.event = NO_OS_EVT_GPIO,
		.peripheral = NO_OS_GPIO_IRQ,
		.handle = ADIS1646X_GPIO_CB_HANDLE,
	};

	struct iio_hw_trig_init_param adis1646x_gpio_trig_ip = {
		.irq_id = ADIS1646X_GPIO_TRIG_IRQ_ID,
		.irq_trig_lvl = NO_OS_IRQ_EDGE_RISING,
		.cb_info = gpio_cb_info,
		.name = ADIS1646X_GPIO_TRIG_NAME,
	};

	#define DATA_BUFFER_SIZE 400
	uint8_t iio_data_buffer[DATA_BUFFER_SIZE * 7 * sizeof(int)];
	struct adis_iio_dev *adis1646x_iio_desc;

	struct iio_data_buffer data_buff = {
		.buff = (void *)iio_data_buffer,
		.size = DATA_BUFFER_SIZE * 7 * sizeof(int)
	};

	struct iio_hw_trig *adis1646x_trig_desc;
	struct no_os_irq_ctrl_desc *adis1646x_irq_desc;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	ret = adis1646x_iio_init(&adis1646x_iio_desc, &adis1646x_ip);
	if (ret)
		goto exit;

	/* Initialize interrupt controller */
	ret = no_os_irq_ctrl_init(&adis1646x_irq_desc, &adis1646x_gpio_irq_ip);
	if (ret)
		goto exit;

	ret = no_os_irq_set_priority(adis1646x_irq_desc, adis1646x_gpio_trig_ip.irq_id, 1);
	if (ret)
		goto exit;

	adis1646x_gpio_trig_ip.irq_ctrl = adis1646x_irq_desc;

	/* Initialize hardware trigger */
	ret = iio_hw_trig_init(&adis1646x_trig_desc, &adis1646x_gpio_trig_ip);
	if (ret)
		goto exit;

	/* List of devices */
	struct iio_app_device iio_devices[] = {
		{
			.name = "adis16465-1",
			.dev = adis1646x_iio_desc,
			.dev_descriptor = adis1646x_iio_desc->iio_dev,
			.read_buff = &data_buff,
		}
	};

	/* List of triggers */
	struct iio_trigger_init trigs[] = {
		IIO_APP_TRIGGER(ADIS1646X_GPIO_TRIG_NAME, adis1646x_trig_desc, &adis_iio_trig_desc)
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = adis1646x_uart_ip;
	app_init_param.trigs = trigs;
	app_init_param.nb_trigs = NO_OS_ARRAY_SIZE(trigs);
	app_init_param.irq_desc = adis1646x_irq_desc;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto exit;

	/* Update the reference to iio_desc */
	adis1646x_trig_desc->iio_desc = app->iio_desc;

	ret = iio_app_run(app);

	iio_app_remove(app);

	exit:
		iio_hw_trig_remove(adis1646x_trig_desc);
		no_os_irq_ctrl_remove(adis1646x_irq_desc);
		adis1646x_iio_remove(adis1646x_iio_desc);
		if (ret)
			pr_info("Error!\n");
		return ret;

ADIS1647X
^^^^^^^^^

.. code-block:: c

	struct no_os_spi_init_param adis1647x_spi_ip = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = SPI_BAUDRATE,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.mode = NO_OS_SPI_MODE_3,
	.platform_ops = SPI_OPS,
	.chip_select = SPI_CS,
	.extra = SPI_EXTRA,
	};

	struct no_os_gpio_init_param adis1647x_gpio_reset_ip = {
		.port = GPIO_RESET_PORT_NUM,
		.number = GPIO_RESET_PIN_NUM,
		.pull = NO_OS_PULL_NONE,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA
	};

	struct adis_init_param adis1647x_ip = {
		.gpio_reset = &adis1647x_gpio_reset_ip,
		.sync_mode = ADIS_SYNC_OUTPUT,
		.dev_id = ADIS16477_1,
	};

	struct no_os_irq_init_param adis1647x_gpio_irq_ip = {
		.irq_ctrl_id = GPIO_IRQ_ID,
		.platform_ops = GPIO_IRQ_OPS,
		.extra = GPIO_IRQ_EXTRA,
	};

	const struct iio_hw_trig_cb_info gpio_cb_info = {
		.event = NO_OS_EVT_GPIO,
		.peripheral = NO_OS_GPIO_IRQ,
		.handle = ADIS1647X_GPIO_CB_HANDLE,
	};

	struct iio_hw_trig_init_param adis1647x_gpio_trig_ip = {
		.irq_id = ADIS1647X_GPIO_TRIG_IRQ_ID,
		.irq_trig_lvl = NO_OS_IRQ_EDGE_RISING,
		.cb_info = gpio_cb_info,
		.name = ADIS1647X_GPIO_TRIG_NAME,
	};

	#define DATA_BUFFER_SIZE 400
	uint8_t iio_data_buffer[DATA_BUFFER_SIZE * 13 * sizeof(int)];
	struct adis_iio_dev *adis1647x_iio_desc;

	struct iio_data_buffer data_buff = {
		.buff = (void *)iio_data_buffer,
		.size = DATA_BUFFER_SIZE * 13 * sizeof(int)
	};

	struct iio_hw_trig *adis1647x_trig_desc;
	struct no_os_irq_ctrl_desc *adis1647x_irq_desc;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	ret = adis1647x_iio_init(&adis1647x_iio_desc, &adis1647x_ip);
	if (ret)
		goto exit;

	/* Initialize interrupt controller */
	ret = no_os_irq_ctrl_init(&adis1647x_irq_desc, &adis1647x_gpio_irq_ip);
	if (ret)
		goto exit;

	ret = no_os_irq_set_priority(adis1647x_irq_desc, adis1647x_gpio_trig_ip.irq_id, 1);
	if (ret)
		goto exit;

	adis1647x_gpio_trig_ip.irq_ctrl = adis1647x_irq_desc;

	/* Initialize hardware trigger */
	ret = iio_hw_trig_init(&adis1647x_trig_desc, &adis1647x_gpio_trig_ip);
	if (ret)
		goto exit;

	/* List of devices */
	struct iio_app_device iio_devices[] = {
		{
			.name = "adis16477-1",
			.dev = adis1647x_iio_desc,
			.dev_descriptor = adis1647x_iio_desc->iio_dev,
			.read_buff = &data_buff,
		}
	};

	/* List of triggers */
	struct iio_trigger_init trigs[] = {
		IIO_APP_TRIGGER(ADIS1647X_GPIO_TRIG_NAME, adis1647x_trig_desc, &adis_iio_trig_desc)
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = adis1647x_uart_ip;
	app_init_param.trigs = trigs;
	app_init_param.nb_trigs = NO_OS_ARRAY_SIZE(trigs);
	app_init_param.irq_desc = adis1647x_irq_desc;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto exit;

	/* Update the reference to iio_desc */
	adis1647x_trig_desc->iio_desc = app->iio_desc;

	ret = iio_app_run(app);

	iio_app_remove(app);

	exit:
		iio_hw_trig_remove(adis1647x_trig_desc);
		no_os_irq_ctrl_remove(adis1647x_irq_desc);
		adis1647x_iio_remove(adis1647x_iio_desc);
		if (ret)
			pr_info("Error!\n");
		return ret;

ADIS1650X
^^^^^^^^^

.. code-block:: c

	struct no_os_spi_init_param adis1650x_spi_ip = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = SPI_BAUDRATE,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.mode = NO_OS_SPI_MODE_3,
	.platform_ops = SPI_OPS,
	.chip_select = SPI_CS,
	.extra = SPI_EXTRA,
	};

	struct no_os_gpio_init_param adis1650x_gpio_reset_ip = {
		.port = GPIO_RESET_PORT_NUM,
		.number = GPIO_RESET_PIN_NUM,
		.pull = NO_OS_PULL_NONE,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA
	};

	struct adis_init_param adis1650x_ip = {
		.gpio_reset = &adis1650x_gpio_reset_ip,
		.sync_mode = ADIS_SYNC_OUTPUT,
		.dev_id = ADIS16505_2,
	};

	struct no_os_irq_init_param adis1650x_gpio_irq_ip = {
		.irq_ctrl_id = GPIO_IRQ_ID,
		.platform_ops = GPIO_IRQ_OPS,
		.extra = GPIO_IRQ_EXTRA,
	};

	const struct iio_hw_trig_cb_info gpio_cb_info = {
		.event = NO_OS_EVT_GPIO,
		.peripheral = NO_OS_GPIO_IRQ,
		.handle = ADIS1650X_GPIO_CB_HANDLE,
	};

	struct iio_hw_trig_init_param adis1650x_gpio_trig_ip = {
		.irq_id = ADIS1650X_GPIO_TRIG_IRQ_ID,
		.irq_trig_lvl = NO_OS_IRQ_EDGE_RISING,
		.cb_info = gpio_cb_info,
		.name = ADIS1650X_GPIO_TRIG_NAME,
	};

	#define DATA_BUFFER_SIZE 400
	uint8_t iio_data_buffer[DATA_BUFFER_SIZE * 13 * sizeof(int)];
	struct adis_iio_dev *adis1650x_iio_desc;

	struct iio_data_buffer data_buff = {
		.buff = (void *)iio_data_buffer,
		.size = DATA_BUFFER_SIZE * 13 * sizeof(int)
	};

	struct iio_hw_trig *adis1650x_trig_desc;
	struct no_os_irq_ctrl_desc *adis1650x_irq_desc;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	ret = adis1650x_iio_init(&adis1650x_iio_desc, &adis1650x_ip);
	if (ret)
		goto exit;

	/* Initialize interrupt controller */
	ret = no_os_irq_ctrl_init(&adis1650x_irq_desc, &adis1650x_gpio_irq_ip);
	if (ret)
		goto exit;

	ret = no_os_irq_set_priority(adis1650x_irq_desc, adis1650x_gpio_trig_ip.irq_id, 1);
	if (ret)
		goto exit;

	adis1650x_gpio_trig_ip.irq_ctrl = adis1650x_irq_desc;

	/* Initialize hardware trigger */
	ret = iio_hw_trig_init(&adis1650x_trig_desc, &adis1650x_gpio_trig_ip);
	if (ret)
		goto exit;

	/* List of devices */
	struct iio_app_device iio_devices[] = {
		{
			.name = "adis16505-2",
			.dev = adis1650x_iio_desc,
			.dev_descriptor = adis1650x_iio_desc->iio_dev,
			.read_buff = &data_buff,
		}
	};

	/* List of triggers */
	struct iio_trigger_init trigs[] = {
		IIO_APP_TRIGGER(ADIS1650X_GPIO_TRIG_NAME, adis1650x_trig_desc, &adis_iio_trig_desc)
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = adis1650x_uart_ip;
	app_init_param.trigs = trigs;
	app_init_param.nb_trigs = NO_OS_ARRAY_SIZE(trigs);
	app_init_param.irq_desc = adis1650x_irq_desc;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto exit;

	/* Update the reference to iio_desc */
	adis1650x_trig_desc->iio_desc = app->iio_desc;

	ret = iio_app_run(app);

	iio_app_remove(app);

	exit:
		iio_hw_trig_remove(adis1650x_trig_desc);
		no_os_irq_ctrl_remove(adis1650x_irq_desc);
		adis1650x_iio_remove(adis1650x_iio_desc);
		if (ret)
			pr_info("Error!\n");
		return ret;

ADIS1654X
^^^^^^^^^

.. code-block:: c

	struct no_os_spi_init_param adis1654x_spi_ip = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = SPI_BAUDRATE,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.mode = NO_OS_SPI_MODE_3,
	.platform_ops = SPI_OPS,
	.chip_select = SPI_CS,
	.extra = SPI_EXTRA,
	};

	struct no_os_gpio_init_param adis1654x_gpio_reset_ip = {
		.port = GPIO_RESET_PORT_NUM,
		.number = GPIO_RESET_PIN_NUM,
		.pull = NO_OS_PULL_NONE,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA
	};

	struct adis_init_param adis1654x_ip = {
		.gpio_reset = &adis1654x_gpio_reset_ip,
		.sync_mode = ADIS_SYNC_DEFAULT,
		.dev_id = ADIS16545_3,
	};

	struct no_os_irq_init_param adis1654x_gpio_irq_ip = {
		.irq_ctrl_id = GPIO_IRQ_ID,
		.platform_ops = GPIO_IRQ_OPS,
		.extra = GPIO_IRQ_EXTRA,
	};

	const struct iio_hw_trig_cb_info gpio_cb_info = {
		.event = NO_OS_EVT_GPIO,
		.peripheral = NO_OS_GPIO_IRQ,
		.handle = ADIS1654X_GPIO_CB_HANDLE,
	};

	struct iio_hw_trig_init_param adis1654x_gpio_trig_ip = {
		.irq_id = ADIS1654X_GPIO_TRIG_IRQ_ID,
		.irq_trig_lvl = NO_OS_IRQ_EDGE_RISING,
		.cb_info = gpio_cb_info,
		.name = ADIS1654X_GPIO_TRIG_NAME,
	};

	#define DATA_BUFFER_SIZE 400
	uint8_t iio_data_buffer[DATA_BUFFER_SIZE * 13 * sizeof(int)];
	struct adis_iio_dev *adis1654x_iio_desc;

	struct iio_data_buffer data_buff = {
		.buff = (void *)iio_data_buffer,
		.size = DATA_BUFFER_SIZE * 13 * sizeof(int)
	};

	struct iio_hw_trig *adis1654x_trig_desc;
	struct no_os_irq_ctrl_desc *adis1654x_irq_desc;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	ret = adis1654x_iio_init(&adis1654x_iio_desc, &adis1654x_ip);
	if (ret)
		goto exit;

	/* Initialize interrupt controller */
	ret = no_os_irq_ctrl_init(&adis1654x_irq_desc, &adis1654x_gpio_irq_ip);
	if (ret)
		goto exit;

	ret = no_os_irq_set_priority(adis1654x_irq_desc, adis1654x_gpio_trig_ip.irq_id, 1);
	if (ret)
		goto exit;

	adis1654x_gpio_trig_ip.irq_ctrl = adis1654x_irq_desc;

	/* Initialize hardware trigger */
	ret = iio_hw_trig_init(&adis1654x_trig_desc, &adis1654x_gpio_trig_ip);
	if (ret)
		goto exit;

	/* List of devices */
	struct iio_app_device iio_devices[] = {
		{
			.name = "adis16545-3",
			.dev = adis1654x_iio_desc,
			.dev_descriptor = adis1654x_iio_desc->iio_dev,
			.read_buff = &data_buff,
		}
	};

	/* List of triggers */
	struct iio_trigger_init trigs[] = {
		IIO_APP_TRIGGER(ADIS1654X_GPIO_TRIG_NAME, adis1654x_trig_desc, &adis_iio_trig_desc)
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = adis1654x_uart_ip;
	app_init_param.trigs = trigs;
	app_init_param.nb_trigs = NO_OS_ARRAY_SIZE(trigs);
	app_init_param.irq_desc = adis1654x_irq_desc;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto exit;

	/* Update the reference to iio_desc */
	adis1654x_trig_desc->iio_desc = app->iio_desc;

	ret = iio_app_run(app);

	iio_app_remove(app);

	exit:
		iio_hw_trig_remove(adis1654x_trig_desc);
		no_os_irq_ctrl_remove(adis1654x_irq_desc);
		adis1654x_iio_remove(adis1654x_iio_desc);
		if (ret)
			pr_info("Error!\n");
		return ret;

ADIS1655X
^^^^^^^^^

.. code-block:: c

	struct no_os_spi_init_param adis1655x_spi_ip = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = SPI_BAUDRATE,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.mode = NO_OS_SPI_MODE_3,
	.platform_ops = SPI_OPS,
	.chip_select = SPI_CS,
	.extra = SPI_EXTRA,
	};

	struct no_os_gpio_init_param adis1655x_gpio_reset_ip = {
		.port = GPIO_RESET_PORT_NUM,
		.number = GPIO_RESET_PIN_NUM,
		.pull = NO_OS_PULL_NONE,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA
	};

	struct adis_init_param adis1655x_ip = {
		.gpio_reset = &adis1655x_gpio_reset_ip,
		.sync_mode = ADIS_SYNC_DEFAULT,
		.dev_id = ADIS16550,
	};

	struct no_os_irq_init_param adis1655x_gpio_irq_ip = {
		.irq_ctrl_id = GPIO_IRQ_ID,
		.platform_ops = GPIO_IRQ_OPS,
		.extra = GPIO_IRQ_EXTRA,
	};

	const struct iio_hw_trig_cb_info gpio_cb_info = {
		.event = NO_OS_EVT_GPIO,
		.peripheral = NO_OS_GPIO_IRQ,
		.handle = ADIS1655X_GPIO_CB_HANDLE,
	};

	struct iio_hw_trig_init_param adis1655x_gpio_trig_ip = {
		.irq_id = ADIS1655X_GPIO_TRIG_IRQ_ID,
		.irq_trig_lvl = NO_OS_IRQ_EDGE_RISING,
		.cb_info = gpio_cb_info,
		.name = ADIS1655X_GPIO_TRIG_NAME,
	};

	#define DATA_BUFFER_SIZE 400
	uint8_t iio_data_buffer[DATA_BUFFER_SIZE * 13 * sizeof(int)];
	struct adis_iio_dev *adis1655x_iio_desc;

	struct iio_data_buffer data_buff = {
		.buff = (void *)iio_data_buffer,
		.size = DATA_BUFFER_SIZE * 13 * sizeof(int)
	};

	struct iio_hw_trig *adis1655x_trig_desc;
	struct no_os_irq_ctrl_desc *adis1655x_irq_desc;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	ret = adis1655x_iio_init(&adis1655x_iio_desc, &adis1655x_ip);
	if (ret)
		goto exit;

	/* Initialize interrupt controller */
	ret = no_os_irq_ctrl_init(&adis1655x_irq_desc, &adis1655x_gpio_irq_ip);
	if (ret)
		goto exit;

	ret = no_os_irq_set_priority(adis1655x_irq_desc, adis1655x_gpio_trig_ip.irq_id, 1);
	if (ret)
		goto exit;

	adis1655x_gpio_trig_ip.irq_ctrl = adis1655x_irq_desc;

	/* Initialize hardware trigger */
	ret = iio_hw_trig_init(&adis1655x_trig_desc, &adis1655x_gpio_trig_ip);
	if (ret)
		goto exit;

	/* List of devices */
	struct iio_app_device iio_devices[] = {
		{
			.name = "adis16550",
			.dev = adis1655x_iio_desc,
			.dev_descriptor = adis1655x_iio_desc->iio_dev,
			.read_buff = &data_buff,
		}
	};

	/* List of triggers */
	struct iio_trigger_init trigs[] = {
		IIO_APP_TRIGGER(ADIS1655X_GPIO_TRIG_NAME, adis1655x_trig_desc, &adis_iio_trig_desc)
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = adis1655x_uart_ip;
	app_init_param.trigs = trigs;
	app_init_param.nb_trigs = NO_OS_ARRAY_SIZE(trigs);
	app_init_param.irq_desc = adis1655x_irq_desc;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto exit;

	/* Update the reference to iio_desc */
	adis1655x_trig_desc->iio_desc = app->iio_desc;

	ret = iio_app_run(app);

	iio_app_remove(app);

	exit:
		iio_hw_trig_remove(adis1655x_trig_desc);
		no_os_irq_ctrl_remove(adis1655x_irq_desc);
		adis1655x_iio_remove(adis1655x_iio_desc);
		if (ret)
			pr_info("Error!\n");
		return ret;

ADIS1657X
^^^^^^^^^

.. code-block:: c

	struct no_os_spi_init_param adis1657x_spi_ip = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = SPI_BAUDRATE,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.mode = NO_OS_SPI_MODE_3,
	.platform_ops = SPI_OPS,
	.chip_select = SPI_CS,
	.extra = SPI_EXTRA,
	};

	struct no_os_gpio_init_param adis1657x_gpio_reset_ip = {
		.port = GPIO_RESET_PORT_NUM,
		.number = GPIO_RESET_PIN_NUM,
		.pull = NO_OS_PULL_NONE,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA
	};

	struct adis_init_param adis1657x_ip = {
		.gpio_reset = &adis1657x_gpio_reset_ip,
		.sync_mode = ADIS_SYNC_OUTPUT,
		.dev_id = ADIS16577_3,
	};

	struct no_os_irq_init_param adis1657x_gpio_irq_ip = {
		.irq_ctrl_id = GPIO_IRQ_ID,
		.platform_ops = GPIO_IRQ_OPS,
		.extra = GPIO_IRQ_EXTRA,
	};

	const struct iio_hw_trig_cb_info gpio_cb_info = {
		.event = NO_OS_EVT_GPIO,
		.peripheral = NO_OS_GPIO_IRQ,
		.handle = ADIS1657X_GPIO_CB_HANDLE,
	};

	struct iio_hw_trig_init_param adis1657x_gpio_trig_ip = {
		.irq_id = ADIS1657X_GPIO_TRIG_IRQ_ID,
		.irq_trig_lvl = NO_OS_IRQ_EDGE_RISING,
		.cb_info = gpio_cb_info,
		.name = ADIS1657X_GPIO_TRIG_NAME,
	};

	#define DATA_BUFFER_SIZE 400
	uint8_t iio_data_buffer[DATA_BUFFER_SIZE * 13 * sizeof(int)];
	struct adis_iio_dev *adis1657x_iio_desc;

	struct iio_data_buffer data_buff = {
		.buff = (void *)iio_data_buffer,
		.size = DATA_BUFFER_SIZE * 13 * sizeof(int)
	};

	struct iio_hw_trig *adis1657x_trig_desc;
	struct no_os_irq_ctrl_desc *adis1657x_irq_desc;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	ret = adis1657x_iio_init(&adis1657x_iio_desc, &adis1657x_ip);
	if (ret)
		goto exit;

	/* Initialize interrupt controller */
	ret = no_os_irq_ctrl_init(&adis1657x_irq_desc, &adis1657x_gpio_irq_ip);
	if (ret)
		goto exit;

	ret = no_os_irq_set_priority(adis1657x_irq_desc, adis1657x_gpio_trig_ip.irq_id, 1);
	if (ret)
		goto exit;

	adis1657x_gpio_trig_ip.irq_ctrl = adis1657x_irq_desc;

	/* Initialize hardware trigger */
	ret = iio_hw_trig_init(&adis1657x_trig_desc, &adis1657x_gpio_trig_ip);
	if (ret)
		goto exit;

	/* List of devices */
	struct iio_app_device iio_devices[] = {
		{
			.name = "adis16577-3",
			.dev = adis1657x_iio_desc,
			.dev_descriptor = adis1657x_iio_desc->iio_dev,
			.read_buff = &data_buff,
		}
	};

	/* List of triggers */
	struct iio_trigger_init trigs[] = {
		IIO_APP_TRIGGER(ADIS1657X_GPIO_TRIG_NAME, adis1657x_trig_desc, &adis_iio_trig_desc)
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = adis1657x_uart_ip;
	app_init_param.trigs = trigs;
	app_init_param.nb_trigs = NO_OS_ARRAY_SIZE(trigs);
	app_init_param.irq_desc = adis1657x_irq_desc;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto exit;

	/* Update the reference to iio_desc */
	adis1657x_trig_desc->iio_desc = app->iio_desc;

	ret = iio_app_run(app);

	iio_app_remove(app);

	exit:
		iio_hw_trig_remove(adis1657x_trig_desc);
		no_os_irq_ctrl_remove(adis1657x_irq_desc);
		adis1657x_iio_remove(adis1657x_iio_desc);
		if (ret)
			pr_info("Error!\n");
		return ret;
