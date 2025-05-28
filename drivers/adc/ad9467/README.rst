AD9467 no-OS Driver
===================

.. no-os-doxygen::

Supported Devices
-----------------

* `AD9467 <https://www.analog.com/AD9467>`_

Overview
--------

The AD9467 is a 16-bit, monolithic, IF sampling analog-to-digital converter
(ADC) optimized for high performance over wide bandwidths and ease of use.
The device operates at a 250 MSPS conversion rate and is designed for wireless
receivers, instrumentation, and test equipment that require a high dynamic
range. It delivers 75.5 dBFS SNR to 210 MHz and 90 dBFS SFDR to 300 MHz at
250 MSPS, with 60 fs rms jitter and excellent linearity (DNL = +/-0.5 LSB
typical, INL = +/-3.5 LSB typical).

The ADC requires 1.8 V and 3.3 V power supplies and a low voltage differential
input clock for full performance operation. No external reference or driver
components are required for many applications. Data outputs are LVDS compatible
(ANSI-644 compatible). The device features an integrated input buffer, clock
duty cycle stabilizer, programmable full-scale input range (2.0 V p-p to
2.5 V p-p), selectable output data formats (offset binary, two's complement,
or Gray code), and built-in digital test pattern generation. The AD9467 is
controlled via a standard SPI interface.

Applications
------------

* Multicarrier, multimode cellular receivers
* Antenna array positioning
* Power amplifier linearization
* Broadband wireless
* Radar
* Infrared imaging
* Communications instrumentation

Device Configuration
--------------------

The AD9467 driver header file declares a comprehensive set of public
functions for configuring the ADC device. These functions are organized
into logical categories based on their functionality.

Initialization
~~~~~~~~~~~~~~

In the initialization category, the essential functions for setting up
and tearing down the AD9467 device are provided. The function
``ad9467_setup`` initializes the device by configuring its SPI
interface and applying default settings, while ``ad9467_remove``
deallocates associated resources and removes the SPI descriptor when the
device is no longer in use.

Low-Level Register Access
~~~~~~~~~~~~~~~~~~~~~~~~~

This function category contains the fundamental functions for performing
low-level register operations. Through these routines,
``ad9467_write`` and ``ad9467_read`` are used to write to and read
from device registers via SPI communication, and
``ad9467_set_bits_to_reg`` allows for modifying specific bits within a
register without affecting the other bits.

Operational Modes
~~~~~~~~~~~~~~~~~

The operational modes category groups functions that configure and query
various ADC operating modes. The function ``ad9467_pwr_mode`` sets the
power mode of the device, while ``ad9467_test_mode`` manages the
activation and status of test patterns. Additionally,
``ad9467_reset_pn9`` and ``ad9467_reset_pn23`` control the reset
behavior of the device's pseudo-random sequence tests.

Analog Input and Reference Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Functions in this group manage the analog input attributes and reference
voltage settings. The function ``ad9467_external_ref`` enables or
disables an external reference voltage, and
``ad9467_analog_input_disconnect`` toggles the connection of the
analog input. Additionally, ``ad9467_offset_adj`` is used for offset
adjustments, and ``ad9467_analog_input_coupling`` selects between AC
and DC coupling modes for optimal signal conditioning.

Output Configuration
~~~~~~~~~~~~~~~~~~~~

This category comprises functions that control the presentation format
and active state of the ADC output signals. The function
``ad9467_output_disable`` is used to enable or disable the data
output, ``ad9467_output_invert`` sets the inversion mode of the output
signal, and ``ad9467_output_format`` allows the selection of different
data formatting modes such as offset binary, two's complement, or Gray
code.

LVDS and Clock Settings
~~~~~~~~~~~~~~~~~~~~~~~

Focusing on the LVDS outputs and clock signals, the functions in this
category adjust the related parameters. Specifically,
``ad9467_coarse_lvds_adj`` adjusts the LVDS output current levels,
``ad9467_dco_clock_invert`` configures the inversion state of the DCO
clock, and ``ad9467_dco_output_clock_delay`` applies the necessary
delay to the DCO output clock for fine timing control.

Current and Buffer Adjustments
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This group allows fine adjustments to the device's output current and
buffer characteristics. The function ``ad9467_output_current_adj``
sets the output current, while ``ad9467_buffer_current_1`` and
``ad9467_buffer_current_2`` control the current levels of the input
buffers to optimize drive strength and buffering performance.

Voltage Range Adjustment
~~~~~~~~~~~~~~~~~~~~~~~~

The voltage range adjustment category focuses on configuring the
full-scale input voltage range of the ADC. The function
``ad9467_full_scale_range`` ensures that the device operates within
proper voltage limits by setting or querying the input range based on
system requirements.

Device Update Transfer
~~~~~~~~~~~~~~~~~~~~~~

Finally, the device update transfer category includes the function
``ad9467_transfer``, which commits configuration changes by initiating
an update transfer. This function monitors the status bit to confirm
that all register modifications are properly applied, ensuring stable
operation before any further actions are taken.

Driver Initialization Example
-----------------------------

.. code-block:: c

   #include "ad9467.h"
   #include "no_os_spi.h"

   struct ad9467_init_param ad9467_default_init = {
       .spi_init = {
           .device_id = SPI_DEVICE_ID,
           .max_speed_hz = 2000000u,
           .chip_select = 0,
           .mode = NO_OS_SPI_MODE_0,
           .platform_ops = SPI_OPS,
           .extra = SPI_EXTRA,
       }
   };

   struct ad9467_dev *dev = NULL;
   int ret;

   ret = ad9467_setup(&dev, ad9467_default_init);
   if (ret)
       return ret;
