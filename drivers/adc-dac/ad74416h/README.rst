AD74416H no-OS Driver
======================

Supported Devices
-----------------

- :adi:`AD74416H`
- :adi:`AD74414H`

Overview
---------

The AD74416H is a quad-channel, software-configurable input and output
device designed for industrial control applications, providing a wide
range of use cases integrated on a single chip. It features quad-channel
software-configurable input and output that supports analog
input/output, digital input/output, current measurement capability, and
temperature sensor measurements through a single SPI interface. The
device implements adaptive power switching, which reduces overall power
dissipation by 40%, thereby optimizing performance and thermal
management. Additionally, it incorporates a 16-bit monotonic DAC per
channel and a 24-bit, Σ-∆ ADC along with an integrated HART modem per
channel, offering versatile and reliable communication in demanding
environments.

Applications
------------

- Industrial control systems
- Process control
- Factory automation

Operation Modes
----------------

+-------------------------+---------------------------+-------------------------+-------------------------+
| Mode Name               | Description               | Configuration Bits      | Typical Use Case        |
+-------------------------+---------------------------+-------------------------+-------------------------+
| AD74416H_HIGH_Z         | Puts the channel into a   | 0x0 or                  | Safely disabling a      |
|                         | high impedance state,     | AD74416H_HIGH_Z (via    | channel; sensor         |
|                         | effectively disconnecting | CH_FUNC_SETUP mask)     | isolation and           |
|                         | it from any active        |                         | transition between      |
|                         | drive signals.            |                         | active configurations.  |
+-------------------------+---------------------------+-------------------------+-------------------------+
| AD74416H_VOLTAGE_OUT    | Configures the channel    | 0x1 (set in             | Driving loads requiring |
|                         | for analog output by      | CH_FUNC_SETUP);         | precise voltage levels  |
|                         | converting a provided     | optionally enables      | (e.g., analog signal    |
|                         | millivolt value to a      | VOUT_4W_EN for 4-wire   | generation, actuator    |
|                         | corresponding DAC code.   | mode                    | control, instrumentation|
+-------------------------+---------------------------+-------------------------+-------------------------+
| AD74416H_CURRENT_OUT    | Sets the channel to       | 0x2 (via CH_FUNC_SETUP) | Driving devices such as |
|                         | drive a constant current  |                         | LEDs and motors that    |
|                         | output. In some           |                         | require a stable,       |
|                         | configurations (e.g.,     |                         | accurate current signal.|
|                         | with HART support)        |                         |                         |
|                         | additional bits (like     |                         |                         |
|                         | SLEW_EN) are required.    |                         |                         |
+-------------------------+---------------------------+-------------------------+-------------------------+
| AD74416H_VOLTAGE_IN     | Uses the channel as an    | 0x3 (via CH_FUNC_SETUP) | Acquiring analog voltage|
|                         | ADC input to measure      |                         | measurements from       |
|                         | incoming voltage levels.  |                         | sensors in data         |
|                         | The internal ADC is       |                         | acquisition systems.    |
|                         | configured with the       |                         |                         |
|                         | proper range (typically   |                         |                         |
|                         | 0V to 12V or bipolar      |                         |                         |
|                         | ranges).                  |                         |                         |
+-------------------------+---------------------------+-------------------------+-------------------------+
| AD74416H_CURRENT_IN_EXT | Configures the channel    | 0x4 (via CH_FUNC_SETUP) | Measuring current from  |
|                         | to measure externally     |                         | external sensors or     |
|                         | provided current, with    |                         | sources where the       |
|                         | the internal ADC          |                         | current-sensing path is |
|                         | digitizing the voltage    |                         | externally powered.     |
|                         | across an internal        |                         |                         |
|                         | sensing resistor.         |                         |                         |
+-------------------------+---------------------------+-------------------------+-------------------------+
| AD74416H_CURRENT_IN_LOOP| Selects the internal      | 0x5 (via CH_FUNC_SETUP) | On-board current        |
|                         | loopback measurement      |                         | loopback tests and      |
|                         | mode for current, where   |                         | calibration routines.   |
|                         | the channel both          |                         |                         |
|                         | sources and measures      |                         |                         |
|                         | current for self-         |                         |                         |
|                         | calibration or testing.   |                         |                         |
+-------------------------+---------------------------+-------------------------+-------------------------+
| AD74416H_RESISTANCE     | Enables resistance        | 0x7 (via CH_FUNC_SETUP) | Resistance sensing for  |
|                         | measurement by            |                         | circuit diagnostics,    |
|                         | leveraging internal       |                         | RTD sensor measurements,|
|                         | excitation and sensing    |                         | or component            |
|                         | circuitry. Variants       |                         | characterization.       |
|                         | (e.g., 3-wire or 2-wire   |                         |                         |
|                         | RTD measurements) may     |                         |                         |
|                         | be applied for accurate   |                         |                         |
|                         | lead compensation.        |                         |                         |
+-------------------------+---------------------------+-------------------------+-------------------------+
| AD74416H_DIGITAL_INPUT  | Sets the channel as a     | 0x8 (via CH_FUNC_SETUP) | Monitoring digital      |
|                         | digital input for         |                         | sensor outputs, user    |
|                         | reading logic-level       |                         | inputs, or interfacing  |
|                         | signals. Additional       |                         | with control logic.     |
|                         | configuration (such as    |                         |                         |
|                         | debounce or diagnostic    |                         |                         |
|                         | detection) may be         |                         |                         |
|                         | available in extended     |                         |                         |
|                         | designs.                  |                         |                         |
+-------------------------+---------------------------+-------------------------+-------------------------+
| AD74416H_DIGITAL_INPUT  | Configures the channel    | 0x9 (via CH_FUNC_SETUP) | System integrity checks |
| _LOOP                   | for digital input         |                         | and internal digital I/O|
|                         | loopback operation,       |                         | loopback tests.         |
|                         | enabling internal         |                         |                         |
|                         | diagnostics and self-     |                         |                         |
|                         | test mechanisms.          |                         |                         |
+-------------------------+---------------------------+-------------------------+-------------------------+
| AD74416H_CURRENT_OUT    | Adjusts the current       | 0x10 (via CH_FUNC_SETUP)| Industrial process      |
| _HART                   | output mode for HART-     | may require SLEW_EN bit | control applications    |
|                         | enabled applications—     |                         | where HART communication|
|                         | ensuring that the         |                         | protocols are used for  |
|                         | current drive meets the   |                         | enhanced device         |
|                         | protocol’s requirements.  |                         | interfacing.            |
+-------------------------+---------------------------+-------------------------+-------------------------+
| AD74416H_CURRENT_IN_EXT | Provides an externally    | 0x11 (via CH_FUNC_SETUP)| Externally powered      |
| _HART                   | powered current           |                         | current sensing in      |
|                         | measurement mode with     |                         | systems that require    |
|                         | HART compatibility,       |                         | HART protocol           |
|                         | ensuring that voltage/    |                         | integration.            |
|                         | current thresholds meet   |                         |                         |
|                         | HART standards.           |                         |                         |
+-------------------------+---------------------------+-------------------------+-------------------------+
| AD74416H_CURRENT_IN_LOOP| Enables loop-powered      | 0x12 (via CH_FUNC_SETUP)| On-board current        |
| _HART                   | current measurement       |                         | loopback in HART-       |
|                         | with HART support,        |                         | enabled devices for     |
|                         | combining on-board        |                         | self-diagnostic and     |
|                         | current sourcing and      |                         | standardized industrial |
|                         | diagnostic loopback       |                         | sensor monitoring.      |
|                         | tailored for HART         |                         |                         |
|                         | networks.                 |                         |                         |
+-------------------------+---------------------------+-------------------------+-------------------------+

Device Configuration
---------------------

Initialization and Deinitialization
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

These functions initialize the AD74416H device into a
known state and perform necessary cleanup operations when the device is
no longer required. These routines include ``ad74416h_init()`` to
allocate and initialize the device descriptor, ``ad74416h_reset()`` to
issue hardware or software resets and enforce delays,
``ad74416h_remove()`` to free resources, and optionally
``ad74416h_device_id_check()`` to verify proper communication with the
device.

Register Access Functions
~~~~~~~~~~~~~~~~~~~~~~~~~~

These functions provide low-level access to device registers and include
mechanisms to read, write, and update registers using SPI transactions
with CRC validation. The entire set consists of ``ad74416h_reg_write()``
for writing a 16-bit value, ``ad74416h_reg_read()`` for reading a
register value with CRC verification, ``ad74416h_reg_read_raw()`` for
retrieving a raw communication frame, ``ad74416h_reg_update()`` for
read-modify-write operations, and ``ad74416h_nb_active_channels()`` to
query the number of active channels.

DAC Conversion and Channel Output Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Dedicated to configuring the DAC and output channels, this group
converts user-friendly analog values to DAC codes and sets the channel
output modes. The functions include ``ad74416h_dac_voltage_to_code()``
for converting voltage values, ``ad74416h_dac_current_to_code()`` for
converting current values, ``ad74416h_set_channel_function()`` to set a
channel’s operational mode, ``ad74416h_set_channel_dac_code()`` to load
a specific DAC code, ``ad74416h_set_channel_vout_range()`` to define the
voltage output range, ``ad74416h_set_channel_i_limit()`` to specify
current limits, as well as ``ad74416h_dac_slew_enable()`` and
``ad74416h_dac_slew_disable()`` for controlling the DAC slew rate.

ADC Configuration and Conversion Functions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This set of functions handles the precise analog-to-digital conversion
process and includes configuration of ADC channels and retrieval of
conversion results. The functions are
``ad74416h_set_adc_channel_enable()`` to enable or disable ADC
conversion on a channel, ``ad74416h_set_adc_conv_seq()`` to manage the
ADC conversion sequence, ``ad74416h_get_raw_adc_result()`` for obtaining
raw ADC data, ``ad74416h_get_adc_single()`` to perform a complete
single-shot conversion, ``ad74416h_get_adc_range()`` and
``ad74416h_set_adc_range()`` for managing ADC measurement ranges,
``ad74416h_get_adc_rate()`` and ``ad74416h_set_adc_rate()`` for
conversion speed, and ``ad74416h_get_adc_conv_mux()`` together with
``ad74416h_set_adc_conv_mux()`` for input node selection.

Feedback, RTD, and Open-Circuit Detection Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Focused on specialized sensor measurements, these functions configure
the device for 3-wire RTD measurements, 2-wire sensing, and
open-circuit/short-circuit detection. The full complement includes
``ad74416h_enable_4wire_mode()`` for 4-wire voltage output,
``ad74416h_configure_3wire_feedback()`` for RTD sensing,
``ad74416h_configure_2wire_feedback()`` for alternative sensing modes,
``ad74416h_set_rtd_config()`` to select RTD operating modes and
excitation parameters, ``ad74416h_config_oc_detection()`` for
open-circuit detection configuration, ``ad74416h_get_oc_status()`` to
read alert statuses, ``ad74416h_read_adc_code()`` for current sensing,
and ``ad74416h_calculate_rsense_current()`` to convert ADC codes into
current measurements.

Digital I/O and GPIO Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

These functions manage the digital input/output capabilities, including
GPIO control, debounce settings, threshold configuration, and direct
digital manipulation. The functions under this category include
``ad74416h_set_debounce_mode()`` to configure input debounce filtering,
``ad74416h_set_debounce_time()`` to set the debounce delay,
``ad74416h_gpio_get()`` and ``ad74416h_gpio_set()`` for reading and
writing GPIO states, ``ad74416h_set_gpio_config()`` to set GPIO
operational modes, ``ad74416h_do_set()`` for direct digital output, and
``ad74416h_set_threshold()`` to configure voltage threshold levels.

Diagnostic and Live Status Functions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For real-time monitoring and fault management, this grouping provides
access to diagnostic data and live operational statuses. The functions
include ``ad74416h_get_temp()`` to read die temperature,
``ad74416h_set_diag()`` and ``ad74416h_get_diag()`` to configure and
retrieve diagnostic information, and ``ad74416h_get_live()`` to access
live status flags such as ADC busy indicators, temperature alerts, and
power supply information.

Power and Thermal Management
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Efficient power usage and thermal stability are managed by functions
that control thermal reset thresholds and buffer power states. This
category includes ``ad74416h_set_therm_rst()`` to configure the thermal
reset threshold, and optionally ``ad74416h_power_control_enable()`` for
enabling power control on sense buffers and
``ad74416h_set_buffer_power_mode()`` to place unused buffers in
low-power standby mode.

Driver Initialization Example
------------------------------

.. code-block:: C

        #include "xparameters.h"
        #include "ad74416h.h"
        #include "no_os_spi.h"
        #include "no_os_gpio.h"
        #include "no_os_delay.h"
        #include "stdio.h"

        /* SPI initialization parameters */
        struct no_os_spi_init_param spi_ip = {
                .device_id = 0,
                .max_speed_hz = 1000000,
                .chip_select = ARDUINO_UNO_D10,
                .mode = NO_OS_SPI_MODE_0,
                .platform_ops = &mbed_spi_ops,
                .extra = &ad74416h_spi_extra,
        };

        /* GPIO initialization parameters for reset */
        struct no_os_gpio_init_param reset_gpio_param = {
                .number = 0, // Replace with the actual GPIO number
                .platform_ops = &mbed_gpio_ops,
                .extra = NULL
        };

        /* AD74416H device initialization parameters */
        struct ad74416h_init_param init_param = {
                .id = ID_AD74416H,
                .dev_addr = 0x01,           /* Device address */
                .spi_ip = spi_ip,
                .reset_gpio_param = &reset_gpio_param
        };

        struct ad74416h_desc *ad74416h_dev;
        int ret;

        /* Initialize the AD74416H device */
        ret = ad74416h_init(&ad74416h_dev, &init_param);
        if (ret) {
                printf("AD74416H initialization failed (err: %d)\n", ret);
                return ret; // Exit on failure
        }

        /* Configure Channel A as a voltage output */
        ret = ad74416h_set_channel_function(ad74416h_dev, AD74416H_CH_A, AD74416H_VOLTAGE_OUT);
        if (ret) {
                printf("Channel A function setup failed (err: %d)\n", ret);
                ad74416h_remove(ad74416h_dev); // Cleanup
                return ret;
        }

        /* Set the voltage output range for Channel A to 0-12V */
        ret = ad74416h_set_channel_vout_range(ad74416h_dev, AD74416H_CH_A, AD74416H_VOUT_RANGE_0_12V);
        if (ret) {
                printf("Channel A vout range configuration failed (err: %d)\n", ret);
                ad74416h_remove(ad74416h_dev); // Cleanup
                return ret;
        }

        /* Convert 0 mV to a DAC code and load it on Channel A */
        uint16_t dac_code;
        ret = ad74416h_dac_voltage_to_code(ad74416h_dev, 0, &dac_code, AD74416H_CH_A);
        if (ret) {
                printf("DAC voltage-to-code conversion failed (err: %d)\n", ret);
                ad74416h_remove(ad74416h_dev); // Cleanup
                return ret;
        }

        ret = ad74416h_set_channel_dac_code(ad74416h_dev, AD74416H_CH_A, dac_code);
        if (ret) {
                printf("Setting DAC code failed (err: %d)\n", ret);
                ad74416h_remove(ad74416h_dev); // Cleanup
                return ret;
        }

        /* Read live status bits from the device */
        union ad74416h_live_status live_status;
        ret = ad74416h_get_live(ad74416h_dev, &live_status);
        if (ret) {
                printf("Failed to read live status (err: %d)\n", ret);
                ad74416h_remove(ad74416h_dev); // Cleanup
                return ret;
        }

        /* Cleanup resources */
        ad74416h_remove(ad74416h_dev);
