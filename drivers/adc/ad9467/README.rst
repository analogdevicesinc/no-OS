AD9467 no-OS Driver
===================

Supported Devices
-----------------

- :adi:`AD9467`

Overview
--------

The AD9467 is a 16-bit, monolithic, IF sampling analog-to-digital
converter (ADC) optimized for high performance over wide bandwidths and
ease of use. It is ideal for applications such as wireless receivers,
instrumentation, and test equipment. This ADC operates at a 250 MSPS
conversion rate while delivering 75.5 dBFS SNR to 210 MHz at 250 MSPS
and 90 dBFS SFDR to 300 MHz at 250 MSPS. It features 60 fs rms jitter
and maintains excellent linearity at 250 MSPS with DNL = ±0.5 LSB
typical and INL = ±3.5 LSB typical. Additional features such as an
integrated input buffer, external reference support, and selectable
output data formats via LVDS highlight the device’s versatility and
robust performance in demanding applications.

Applications
------------

- Multicarrier, multimode cellular receivers
- Antenna array positioning
- Power amplifier linearization
- Broadband wireless
- Radar
- Infrared imaging
- Communications instrumentation

Operation Modes
----------------

+-----------------+---------------------------------------------+-----------------+-----------------+
| Mode Name       | Description                                 | Configuration   | Typical Use     |
|                 |                                             | Bits            | Case            |
+-----------------+---------------------------------------------+-----------------+-----------------+
| Test Mode       | Configures the ADC’s output to a test       | AD9467_TEST     | Verifying       |
|                 | pattern for diagnostics and calibration.    | _IO_OUT_TEST(x) | signal          |
|                 | Options include various test patterns       |                 | integrity and   |
|                 | selected via the configuration value.       |                 | performing      |
|                 |                                             |                 | system          |
|                 |                                             |                 | calibration     |
|                 |                                             |                 | tests.          |
+-----------------+---------------------------------------------+-----------------+-----------------+
| Normal          | Standard ADC operation with all power       | AD9467_MODES    | Regular data    |
| Operation       | circuits active.                            | _INT_PD_MODE(0) | acquisition     |
| (Power)         |                                             |                 | under typical   |
|                 |                                             |                 | operating       |
|                 |                                             |                 | conditions.     |
+-----------------+---------------------------------------------+-----------------+-----------------+
| Full Power-Down | Reduces power consumption by powering       | AD9467_MODES    | Low-power or    |
| (Power)         | down internal circuitry.                    | _INT_PD_MODE(1) | standby         |
|                 |                                             |                 | applications    |
|                 |                                             |                 | requiring       |
|                 |                                             |                 | energy          |
|                 |                                             |                 | optimization.   |
+-----------------+---------------------------------------------+-----------------+-----------------+
| Output Format   | Selects the ADC data output format to       | AD9467_OUT      | Ensuring        |
| Mode            | meet host interface requirements.           | _FORMAT         | compatibility   |
|                 |                                             |                 | between the ADC |
|                 |                                             |                 | output and      |
|                 |                                             |                 | digital         |
|                 |                                             |                 | processing      |
|                 |                                             |                 | backend.        |
+-----------------+---------------------------------------------+-----------------+-----------------+
| Analog Input    | Specifies the ADC’s input coupling type     | AD9467_ANALOG   | Customizing the |
| Coupling Mode   | (AC or DC) for optimal signal conditioning. | _INPUT_COUPLING | analog          |
|                 |                                             |                 | front-end for   |
|                 |                                             |                 | targeted signal |
|                 |                                             |                 | characteristics |
+-----------------+---------------------------------------------+-----------------+-----------------+

Device Configuration
--------------------

The AD9467 driver header file declares a comprehensive set of public
functions for configuring the ADC device. These functions are organized
into logical categories based on their functionality.

Initialization
~~~~~~~~~~~~~~

In the initialization category, the essential functions for setting up
and tearing down the AD9467 device are provided. The function
``ad9467_setup()`` initializes the device by configuring its SPI
interface and applying default settings, while ``ad9467_remove()``
deallocates associated resources and removes the SPI descriptor when the
device is no longer in use.

Low-Level Register Access
~~~~~~~~~~~~~~~~~~~~~~~~~

This function category contains the fundamental functions for performing
low-level register operations. Through these routines,
``ad9467_write()`` and ``ad9467_read()`` are used to write to and read
from device registers via SPI communication, and
``ad9467_set_bits_to_reg()`` allows for modifying specific bits within a
register without affecting the other bits.

Operational Modes
~~~~~~~~~~~~~~~~~

The operational modes category groups functions that configure and query
various ADC operating modes. The function ``ad9467_pwr_mode()`` sets the
power mode of the device, while ``ad9467_test_mode()`` manages the
activation and status of test patterns. Additionally,
``ad9467_reset_pn9()`` and ``ad9467_reset_pn23()`` control the reset
behavior of the device’s pseudo-random sequence tests.

Analog Input and Reference Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Functions in this group manage the analog input attributes and reference
voltage settings. The function ``ad9467_external_ref()`` enables or
disables an external reference voltage, and
``ad9467_analog_input_disconnect()`` toggles the connection of the
analog input. Additionally, ``ad9467_offset_adj()`` is used for offset
adjustments, and ``ad9467_analog_input_coupling()`` selects between AC
and DC coupling modes for optimal signal conditioning.

Output Configuration
~~~~~~~~~~~~~~~~~~~~

This category comprises functions that control the presentation format
and active state of the ADC output signals. The function
``ad9467_output_disable()`` is used to enable or disable the data
output, ``ad9467_output_invert()`` sets the inversion mode of the output
signal, and ``ad9467_output_format()`` allows the selection of different
data formatting modes such as offset binary, two’s complement, or gray
code.

LVDS and Clock Settings
~~~~~~~~~~~~~~~~~~~~~~~

Focusing on the LVDS outputs and clock signals, the functions in this
category adjust the related parameters. Specifically,
``ad9467_coarse_lvds_adj()`` adjusts the LVDS output current levels,
``ad9467_dco_clock_invert()`` configures the inversion state of the DCO
clock, and ``ad9467_dco_output_clock_delay()`` applies the necessary
delay to the DCO output clock for fine timing control.

Current and Buffer Adjustments
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This group allows fine adjustments to the device’s output current and
buffer characteristics. The function ``ad9467_output_current_adj()``
sets the output current, while ``ad9467_buffer_current_1()`` and
``ad9467_buffer_current_2()`` control the current levels of the output
buffers to optimize drive strength and buffering performance.

Voltage Range Adjustment
~~~~~~~~~~~~~~~~~~~~~~~~

The voltage range adjustment category focuses on configuring the
full-scale input voltage range of the ADC. The function
``ad9467_full_scale_range()`` ensures that the device operates within
proper voltage limits by setting or querying the input range based on
system requirements.

Device Update Transfer
~~~~~~~~~~~~~~~~~~~~~~

Finally, the device update transfer category includes the function
``ad9467_transfer()``, which commits configuration changes by initiating
an update transfer. This function monitors the status bit to confirm
that all register modifications are properly applied, ensuring stable
operation before any further actions are taken.

Driver Initialization Example
-----------------------------

.. code-block::C

   #include <stdio.h>
   #include <stdlib.h>
   #include <stdint.h>
   #include "ad9467.h"
   #include "no_os_spi.h"
   #include "no_os_alloc.h"

   ad9467_init_param ad9467_default_init = {
       .spi_init = {
           .device_id = SPI_DEVICE_ID,
           .max_speed_hz = 2000000u,
           .chip_select = 0,
           .mode = NO_OS_SPI_MODE_0,
           .platform_ops = &xil_spi_ops,
           .extra = &(struct xil_spi_init_param){
   #ifdef PLATFORM_MB
               .type = SPI_PL,
   #else
               .type = SPI_PS,
   #endif
           }
       }
   };

   struct ad9467_dev *dev = NULL;
   int ret = 0;

   ret = ad9467_setup(&dev, ad9467_default_init);
   if (ret != 0)
       goto err;

   printf("AD9467 initialization successful\n");
   goto exit;
   err:
   printf("Error during AD9467 initialization: %d\n", ret);
   exit:
       ;

