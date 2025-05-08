AD7768-1 no-OS Driver
=====================

.. no-os-doxygen::

Supported Devices
-----------------

- :adi:`AD7768-1`

Overview
--------

The :adi:`AD7768-1` is a precision 24-bit analog-to-digital converter (ADC)
optimized for high-performance applications in dynamic signal analysis.
It offers flexible operational modes, including continuous, one-shot,
single, periodic, and standby, making it adaptable to various scenarios.
The converter supports SPI and pin control modes, providing functions
like digital filtering, power mode adjustments, and GPIO configurations.
The AD7768-1 optimizes data transfer with features such as continuous
read enable, programmable finite impulse response (FIR) filters, and
support for high-speed data acquisition.

Applications
------------

- Platform ADC to serve a superset of measurements and sensor types
    - Sound and vibration, acoustic, and material science research and development
    - Control and hardware in loop verification
    - Condition monitoring for predictive maintenance
    - Electrical test and measurement
    - Audio testing and current and voltage measurement
    - Clinical electroencephalogram (EEG), electromyogram (EMG), and electrocardiogram (ECG) vital signs monitoring
    - USB-, PXI-, and Ethernet-based modular data acquisition (DAQ)
    - Channel to channel isolated modular DAQ designs

Operational Modes
-----------------

+-------------------+-----------------------------------+-------------------------+-----------------------------+
| Mode Name         | Description                       | Configuration           | Typical Use Case            |
+===================+===================================+=========================+=============================+
| Continuous        | Uninterrupted data conversion,    |                         | Real-time sound measurement.|
|                   | ideal for continuous signal       | AD77681_CONV_CONTINUOUS |                             |
|                   | monitoring.                       |                         |                             |
+-------------------+-----------------------------------+-------------------------+-----------------------------+
| One-shot          | Single conversion on demand,      |                         | Intermittent readings in    |
|                   | suitable for low power            | AD77681_CONV_ONE_SHOT   | battery-powered devices.    |
|                   | applications.                     |                         |                             |
+-------------------+-----------------------------------+-------------------------+-----------------------------+
| Periodic          | Converts data at regular          |                         | Environmental monitoring    |
|                   | intervals, balancing power        | AD77681_CONV_PERIODIC   | with scheduled readings.    |
|                   | consumption and performance.      |                         |                             |
+-------------------+-----------------------------------+-------------------------+-----------------------------+
| Standby           | Low-power state, ready for quick  | AD77681_CONV_STANDBY    | Devices needing quick       |
|                   | conversion resumption.            |                         | wake up capability.         |
+-------------------+-----------------------------------+-------------------------+-----------------------------+
| Single            | Similar to one-shot but implied   |                         | Single event measurement    |
|                   | more for one-time single use      | AD77681_CONV_SINGLE     | and precision testing.      |
|                   | without a signal.                 |                         |                             |
+-------------------+-----------------------------------+-------------------------+-----------------------------+

Driver Configuration
--------------------

Initialization
~~~~~~~~~~~~~~

To initialize the AD7768-1 device, ensure SPI communication connections
and adequate power supply. Begin software setup by defining the
``ad77681_init_param`` structure for initialization parameters. This
includes configuring the SPI using ``spi_eng_dev_init``, setting power
mode to ``AD77681_FAST``, selecting master clock division as
``AD77681_MCLK_DIV_8``, and choosing the conversion mode as
``AD77681_CONV_CONTINUOUS``. Execute the ``ad77681_setup`` function to
allocate memory for the device structures and configure the ADC
operational parameters.

Power Configurations
~~~~~~~~~~~~~~~~~~~~

Control the ADC power mode using ``ad77681_set_power_mode``, selecting from
``AD77681_ECO``, ``AD77681_MEDIAN``, or ``AD77681_FAST modes``. Adjust the master
clock with ``ad77681_set_mclk_div`` using dividers such as
``AD77681_MCLK_DIV_16``, ``AD77681_MCLK_DIV_8``, ``AD77681_MCLK_DIV_4``, and
``AD77681_MCLK_DIV_2``.

Conversion Modes
~~~~~~~~~~~~~~~~

Mode Selection for the AD7768-1 ADC involves using the
``ad77681_set_conv_mode function`` to configure the conversion mode and
related settings. The available conversion modes are continuous
(``AD77681_CONV_CONTINUOUS``), one-shot (``AD77681_CONV_ONE_SHOT``), single
(``AD77681_CONV_SINGLE``), periodic (``AD77681_CONV_PERIODIC``), and standby.
The conversion length can be defined with ``ad77681_set_convlen``, selecting
between 24-bit (``AD77681_CONV_24BIT``) and 16-bit (``AD77681_CONV_16BIT``).
These configurations are written to the device’s registers via SPI.

Buffer and Reference Settings
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To configure the AD7768-1 ADC input and reference buffers, use
``ad77681_set_AINn_buffer`` and ``ad77681_set_AINp_buffer`` for the negative and
positive analog input precharge buffers, respectively. For managing the
negative and positive reference voltages, utilize
``ad77681_set_REFn_buffer`` and ``ad77681_set_REFp_buffer``.

Filter Configurations
~~~~~~~~~~~~~~~~~~~~~

The filter type for SINC filters can be set using the
``ad77681_set_filter_type`` function, while power line rejection can be
enabled using the ``ad77681_set_50HZ_rejection`` function for the AD7768-1
ADC device.

Gain and Offset Adjustments
~~~~~~~~~~~~~~~~~~~~~~~~~~~

The functions ``ad77681_apply_gain`` and ``ad77681_apply_offset`` are used to
calibrate the AD7768-1 ADC by writing specific 24-bit values to its gain
and offset registers. 

``ad77681_apply_gain`` accepts a 24-bit gain value,
splits it into three 8-bit segments (high, middle, and low), and writes
each to the GAIN_HI, GAIN_MID, and GAIN_LO registers using SPI
communication. Similarly, ``ad77681_apply_offset`` configures the offset
setting by dividing a 24-bit offset value into high, middle, and low
bytes, then writing them to the OFFSET_HI, OFFSET_MID, and OFFSET_LO
registers via SPI. Both functions return 0 on successful execution or a
negative error code if any SPI write operations fail.

**Note:** When configuring the AD7768-1 device, verify that all
configuration values are consistent with the specific platform or
hardware in use, especially regarding clock speeds, power modes, and SPI
settings. Accurate device-specific configurations are crucial to avoid
performance or compatibility issues.

Driver Initialization Example
-----------------------------

.. code-block:: c

   #include "ad77681.h"
   #include "no_os_spi.h"

   // Driver Initialization Code for AD7768-1 ADC

   struct ad77681_init_param ADC_default_init_param = {
       /* SPI Configuration */
       .spi_eng_dev_init = {
           .chip_select = AD77681_SPI_CS,       // Chip select configuration
           .max_speed_hz = 1000000,            // Max speed of SPI in Hz
           .mode = NO_OS_SPI_MODE_3,           // SPI mode 3
           .platform_ops = &spi_eng_platform_ops,
           .extra = (void*)&spi_eng_init_param // Additional configuration parameters
       },
       /* ADC Configuration */
       .power_mode = AD77681_FAST,              // High-speed power mode
       .mclk_div = AD77681_MCLK_DIV_8,          // Master clock division
       .conv_mode = AD77681_CONV_CONTINUOUS,    // Continuous conversion mode
       .diag_mux_sel = AD77681_POSITIVE_FS,     // Diagnostic mux selection
       .conv_diag_sel = false,                  // Conversion diagnostics disabled
       .conv_len = AD77681_CONV_16BIT,          // Conversion bit length
       .crc_sel = AD77681_CRC,                  // CRC data integrity check enabled
       .status_bit = 0,                         // Default status
       .vcmo = AD77681_VCM_HALF_VCC,            // VCM configuration
       .ainn_buf = AD77681_AINn_ENABLED,        // Negative input precharge buffer enabled
       .ainp_buf = AD77681_AINp_ENABLED,        // Positive input precharge buffer enabled
       .refn_buf = AD77681_BUFn_ENABLED,        // REF- buffer enabled
       .refp_buf = AD77681_BUFp_ENABLED,        // REF+ buffer enabled
       .fir_filter = AD77681_FIR,               // FIR filter configuration
       .dec_rate = AD77681_SINC5_FIR_DECx32,    // Decimation rate
       .sinc3_osr = 0,                         // Oversampling rate of SINC3
       .ref_voltage = 4096,                     // Reference voltage in mV
       .mclk_freq = 16384                       // MCLK frequency in kHz

   };

   // Function to initialize the AD7768-1 device
   int main()
   {
       struct ad77681_dev *adc_dev;
       struct ad77681_status_registers *adc_status;
       uint8_t adc_data[5];
       int32_t ret;

       // Perform setup using the predefined initialization parameters
       ret = ad77681_setup(&adc_dev, &ADC_default_init_param, &adc_status);
       if (ret != 0) {
           printf("AD7768-1 ADC device setup failed\n");
           return -1;
       }

       // Enter continuous data reading loop
       while (1) {
           ad77681_spi_read_adc_data(adc_dev, adc_data, AD77681_CONTINUOUS_DATA_READ);
           printf("[ADC DATA]: 0x");
           for (int i = 0; i < sizeof(adc_data); i++) {
               printf("%02x", adc_data[i]);
           }
           printf("\n");
           no_os_mdelay(1000);  // Delay for 1 second
       }

       return 0;
   }
