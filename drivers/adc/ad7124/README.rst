AD7124 no-OS Driver
===================

Supported Devices
-----------------

- :adi:`AD7124-4`
- :adi:`AD7124-8`

Overview
--------

The AD7124-4 and AD7124-8 are low power, low noise, completely integrated
analog front ends for high precision measurement applications. Both devices
contain a low noise, 24-bit Σ-∆ ADC with an integrated PGA (gain 1 to 128),
a reference buffer, and an on-chip temperature sensor. Up to 8 setups can be
independently configured with different gains, filter types, output data
rates, reference sources, and polarity modes, allowing each input channel to
be optimized for its specific signal.

Communication is via a 4-wire SPI-compatible interface supporting up to 5 MHz.
The ADC supports multiple operating modes including continuous, single, standby,
and power-down, as well as system and internal calibration modes.

Applications
------------

- Industrial process control
- Temperature measurement (RTD, thermocouple)
- Pressure and load cell measurement
- Weigh scales
- Medical instrumentation
- Chromatography

Operation Modes
---------------

+-------------------------------+--------------------------------------------------+
| Mode                          | Description                                      |
+-------------------------------+--------------------------------------------------+
| AD7124_CONTINUOUS             | ADC converts continuously; DRDY pulses low after |
|                               | each conversion. Default operating mode.         |
+-------------------------------+--------------------------------------------------+
| AD7124_SINGLE                 | ADC performs one conversion then returns to      |
|                               | IDLE automatically. DRDY asserts once and        |
|                               | remains high after data is read.                 |
+-------------------------------+--------------------------------------------------+
| AD7124_STANDBY                | Low power standby; faster wake-up than           |
|                               | power-down. Reference buffers may be kept on.    |
+-------------------------------+--------------------------------------------------+
| AD7124_POWER_DOWN             | Lowest power state; full wake-up cycle required  |
|                               | before the next conversion.                      |
+-------------------------------+--------------------------------------------------+
| AD7124_IDLE                   | ADC stopped; registers accessible. Used before   |
|                               | reconfiguring channels or setups.                |
+-------------------------------+--------------------------------------------------+
| AD7124_INT_ZERO_SCALE_CALIB   | Internal zero-scale (offset) calibration.        |
+-------------------------------+--------------------------------------------------+
| AD7124_INT_FULL_SCALE_CALIB   | Internal full-scale (gain) calibration.          |
+-------------------------------+--------------------------------------------------+
| AD7124_SYS_ZERO_SCALE_OFF     | System zero-scale offset calibration; requires   |
|                               | the zero-scale signal to be applied externally.  |
+-------------------------------+--------------------------------------------------+
| AD7124_SYS_ZERO_SCALE_GAIN    | System full-scale gain calibration; requires     |
|                               | the full-scale signal to be applied externally.  |
+-------------------------------+--------------------------------------------------+

Device Configuration
--------------------

Initialization and Deinitialization
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``ad7124_setup()`` allocates and initializes the device descriptor, performs
a hardware reset, writes ADC control, configuration, and channel registers,
and sets the power and operating mode. ``ad7124_remove()`` releases all
allocated SPI and memory resources.

Register Access
~~~~~~~~~~~~~~~

Low-level register access is provided by ``ad7124_read_register2()`` and
``ad7124_write_register2()``, which take a register enum and a 32-bit value.
The legacy ``ad7124_read_register()`` / ``ad7124_write_register()`` wrappers
take a ``struct ad7124_st_reg`` pointer. Direct register access is intended
only for the base driver itself; upper layers should use the semantic helpers.

Channel and Setup Helpers
~~~~~~~~~~~~~~~~~~~~~~~~~~

These functions provide a clean, register-read-based API for querying and
modifying per-channel and per-setup state. All channel queries resolve the
assigned setup by reading the hardware CH_MAP register, so they always reflect
the current hardware configuration:

- ``ad7124_get_ch_setup()`` — returns the setup index (0-7) assigned to a
  hardware channel.
- ``ad7124_get_bipolar()`` / ``ad7124_get_pga()`` — read polarity and PGA bits
  (0-7, where gain = 2^pga_bits) for a hardware channel via its setup's
  Config register.
- ``ad7124_get_vref_mv()`` / ``ad7124_set_vref_mv()`` — get/set the reference
  voltage (mV) stored per setup in the driver descriptor. Defaults to 2500 mV
  (internal 2.5 V reference) for all setups at init time.
- ``ad7124_get_odr_ch()`` / ``ad7124_set_odr_ch()`` — get/set the output data
  rate (Hz) for a hardware channel, resolving the setup from hardware and
  delegating to ``ad7124_get_odr()`` / ``ad7124_set_odr()``.

Filter Configuration
~~~~~~~~~~~~~~~~~~~~~

Filter type is selected per setup using the ``ad7124_filter_type`` enum, which
mirrors the Linux kernel AD7124 driver:

- ``ad7124_get_filter_type()`` — decodes the hardware Filter register
  (FILTER, REJ60, POST_FILTER fields) into an ``ad7124_filter_type`` enum.
- ``ad7124_set_filter_type()`` — encodes the enum back into the hardware
  Filter register.
- ``ad7124_get_3db_filter_factor()`` — returns the 3 dB cutoff factor ×1000
  for a given filter type. Multiply by ODR and divide by 1000 to get the
  3 dB frequency in Hz.

+-------------------------------+--------------------+
| Filter Type                   | 3 dB Factor ×1000  |
+-------------------------------+--------------------+
| sinc4                         | 230                |
+-------------------------------+--------------------+
| sinc3                         | 272                |
+-------------------------------+--------------------+
| sinc3_pf1                     | 633                |
+-------------------------------+--------------------+
| sinc3_pf2                     | 605                |
+-------------------------------+--------------------+
| sinc3_pf3                     | 669                |
+-------------------------------+--------------------+
| sinc3_pf4                     | 759                |
+-------------------------------+--------------------+
| sinc3_sinc1 / sinc4_sinc1     | 272 / 230          |
+-------------------------------+--------------------+

Calibration
~~~~~~~~~~~

``ad7124_do_calibration()`` performs a system zero-scale or full-scale
calibration on the specified hardware channel. It enables the channel, sets
the appropriate calibration mode, waits for completion via
``ad7124_wait_for_conv_ready()``, then restores continuous conversion mode.

Temperature Sensor
~~~~~~~~~~~~~~~~~~

``ad7124_read_temp_raw()`` reads one 24-bit raw sample from the on-chip
temperature sensor using hardware channel 15 and setup 7. It configures the
channel (bipolar, internal reference, PGA=1, AINP=TEMP_SENSOR, AINM=AVSS),
writes the Filter_7 register from the software cache to ensure a valid ODR
before starting a single conversion, then disables the channel. The raw
value is converted to temperature by the IIO layer using:

.. code-block:: none

   temperature_mC = (raw + offset) * scale
   temperature_°C = temperature_mC / 1000

With the default parameters (offset = -12090248, scale = 0.073242 mC/LSB)
a raw value of ~12.4 M at room temperature (~22 °C) is typical.

IIO Interface
-------------

The IIO driver (``iio_ad7124.c``) exposes nine channels:

- **voltage0–voltage7** (IIO indices 0-7): differential voltage channels
  mapped to hardware channels 0, 2, 4, 6, 8, 10, 12, 14.
- **temp0** (IIO index 8): on-chip temperature sensor, hardware channel 15.

All channels use a 24-bit unsigned (offset binary), big-endian scan type
with 32-bit storage.

Voltage Channel Attributes
~~~~~~~~~~~~~~~~~~~~~~~~~~~

+-----------------------------------+--------+-------------------------------------------+
| Attribute                         | Access | Description                               |
+-----------------------------------+--------+-------------------------------------------+
| raw                               | R      | Single raw ADC conversion result.         |
+-----------------------------------+--------+-------------------------------------------+
| scale                             | R/W    | LSB value in mV. Write to change PGA.     |
+-----------------------------------+--------+-------------------------------------------+
| scale_available                   | R      | All 8 valid scale values (PGA 1-128).     |
+-----------------------------------+--------+-------------------------------------------+
| offset                            | R      | -2^23 (bipolar) or 0 (unipolar).          |
+-----------------------------------+--------+-------------------------------------------+
| sampling_frequency                | R/W    | Output data rate in Hz.                   |
+-----------------------------------+--------+-------------------------------------------+
| filter_low_pass_3db_frequency     | R      | 3 dB cutoff: factor × ODR / 1000.         |
+-----------------------------------+--------+-------------------------------------------+
| filter_type                       | R/W    | Active filter (e.g. sinc4, sinc3_pf2).    |
+-----------------------------------+--------+-------------------------------------------+
| filter_type_available             | R      | All 10 supported filter names.            |
+-----------------------------------+--------+-------------------------------------------+
| sys_calibration                   | W      | Trigger system calibration (write-only).  |
+-----------------------------------+--------+-------------------------------------------+
| sys_calibration_mode              | R/W    | "zero_scale" or "full_scale".             |
+-----------------------------------+--------+-------------------------------------------+
| sys_calibration_mode_available    | R      | "zero_scale full_scale"                   |
+-----------------------------------+--------+-------------------------------------------+

Temperature Channel Attributes
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

+-------------------+--------+-------------------------------------------+
| Attribute         | Access | Description                               |
+-------------------+--------+-------------------------------------------+
| raw               | R      | Single raw ADC sample from temp sensor.   |
+-------------------+--------+-------------------------------------------+
| scale             | R      | 0.073242 milli-°C per LSB (fixed).        |
+-------------------+--------+-------------------------------------------+
| offset            | R      | -12090248 (fixed).                        |
+-------------------+--------+-------------------------------------------+
| sampling_frequency| R/W    | Output data rate in Hz.                   |
+-------------------+--------+-------------------------------------------+

Driver Initialization Example
------------------------------

.. code-block:: C

        #include "ad7124.h"
        #include "iio_ad7124.h"
        #include "no_os_spi.h"

        extern struct ad7124_st_reg ad7124_init_regs_default[];

        struct no_os_spi_init_param spi_ip = {
                .device_id    = 0,
                .max_speed_hz = 1000000,
                .chip_select  = 0,
                .mode         = NO_OS_SPI_MODE_3,
                .platform_ops = &max_spi_ops,
                .extra        = &spi_extra,
        };

        struct ad7124_init_param ad7124_ip = {
                .spi_init        = &spi_ip,
                .regs            = ad7124_init_regs_default,
                .spi_rdy_poll_cnt = 1000,
                .active_device   = ID_AD7124_8,
        };

        struct ad7124_dev *dev;
        int ret;

        ret = ad7124_setup(&dev, &ad7124_ip);
        if (ret) {
                printf("AD7124 init failed (%d)\n", ret);
                return ret;
        }

        /* Optionally set external reference voltage for setup 0 */
        ret = ad7124_set_vref_mv(dev, 0, 4096);
        if (ret)
                goto remove;

        /* Set filter type on setup 0 to sinc3 post-filter 2 */
        ret = ad7124_set_filter_type(dev, 0, AD7124_FILTER_TYPE_SINC3_PF2);
        if (ret)
                goto remove;

        /* Read temperature */
        int32_t temp_raw;
        ret = ad7124_read_temp_raw(dev, &temp_raw);
        if (ret)
                goto remove;

        /* temperature_mC = (temp_raw - 12090248) * 0.073242 */

remove:
        ad7124_remove(dev);
