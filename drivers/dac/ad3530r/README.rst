AD3530R no-OS Driver
====================

.. no-os-doxygen::

Supported Devices
-----------------

`AD3530R <https://www.analog.com/en/products/ad3530r.html>`_

`AD3531R <https://www.analog.com/en/products/ad3531r.html>`_

`AD3532R <https://www.analog.com/en/products/ad3532r.html>`_

Overview
--------

The AD3530R/AD3531R/AD3532R are precision digital-to-analog converters (DACs)
with multiple channels and 16-bit resolution. The AD3530R features 8 channels,
AD3531R features 4 channels, and AD3532R features 16 channels. These devices
support voltage output modes, making them versatile for driving
various industrial loads. The devices support SPI communication with CRC error
checking, hardware and software LDAC triggering, and multiple operating modes
per channel. They include internal/external voltage reference options and
flexible output ranges.

Device Configuration
--------------------

Device Setup and Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ``ad3530r_init`` function initializes and configures the AD3530R/AD3531R/
AD3532R, involving memory allocation, SPI initialization, GPIO setup for RESET
and LDAC pins, and comprehensive device configuration. The initialization
process includes product ID verification, scratch pad testing, and configuration
of operating modes, output ranges, reference selection, and CRC settings.

Resource Management
~~~~~~~~~~~~~~~~~~~

The ``ad3530r_remove`` function deallocates resources associated with the
device, removing GPIO descriptors, SPI descriptor, and freeing allocated memory
to ensure proper resource release and prevent memory leaks.

SPI Communication
~~~~~~~~~~~~~~~~~

The ``ad3530r_reg_read`` and ``ad3530r_reg_write`` functions handle SPI
interactions for the device. These functions support both short and long
instruction modes, CRC error checking, and streaming mode configurations.

Channel Configuration
~~~~~~~~~~~~~~~~~~~~~

Individual channels can be configured for different operating modes using
``ad3530r_set_operating_mode``. Output ranges are set per channel group using
``ad3530r_set_output_range``, allowing fine-tuned control of each DAC output.

DAC Value Setting
~~~~~~~~~~~~~~~~~

DAC values can be set using ``ad3530r_set_dac_value`` for individual channels
or ``ad3530r_set_multidac_value`` for simultaneous updates across multiple
channels. The driver supports different write modes for immediate updates or
synchronized updates using LDAC triggers.

Hardware Features
~~~~~~~~~~~~~~~~~

The driver supports hardware LDAC triggering via GPIO pin using
``ad3530r_hw_ldac_trigger`` and software LDAC triggering using
``ad3530r_sw_ldac_trigger``. Reference voltage can be configured as internal
or external using ``ad3530r_set_reference``. The MUX output selection allows
monitoring of various signals using ``ad3530r_set_mux_out_select``.

Driver Initialization Example
-----------------------------

.. code-block:: C

   #include <stdint.h>
   #include "no_os_delay.h"
   #include "no_os_spi.h"
   #include "no_os_gpio.h"
   #include "ad3530r.h"

   // Initialization parameters for the AD3530R device
   struct ad3530r_init_param init_param = {
       .chip_id = AD3530R_ID, // Device Selection
       .spi_cfg = {
           .stream_mode_length = 0,
           .addr_asc = 0,
           .single_instr = 1,
           .short_instr = 0,
           .stream_length_keep_value = 0
       },
       .spi_param = &spi_init_param, // SPI configuration
       .reset_gpio_param_optional = &reset_gpio_param, // Optional reset GPIO
       .ldac_gpio_param_optional = &ldac_gpio_param,   // Optional LDAC GPIO
       .vref_enable = AD3530R_INTERNAL_VREF_PIN_2P5V,  // Internal 2.5V reference
       .chn_op_mode = {
           AD3530R_CH_OPERATING_MODE_0, // Channel 0
           AD3530R_CH_OPERATING_MODE_0, // Channel 1
           AD3530R_CH_OPERATING_MODE_0, // Channel 2
           AD3530R_CH_OPERATING_MODE_0, // Channel 3
           AD3530R_CH_OPERATING_MODE_0, // Channel 4
           AD3530R_CH_OPERATING_MODE_0, // Channel 5
           AD3530R_CH_OPERATING_MODE_0, // Channel 6
           AD3530R_CH_OPERATING_MODE_0  // Channel 7
       },
       .range = AD3530R_CH_OUTPUT_RANGE_0_VREF, // 0V to VREF range
       .hw_ldac_mask = 0x00, // No hardware LDAC enabled
       .sw_ldac_mask = 0xFF, // All channels software LDAC enabled
       .crc_en = 0,          // Disable CRC error checking
       .mux_out_sel = VOUT0  // Monitor channel 0 voltage output
   };

   // Device structure for the AD3530R
   struct ad3530r_desc *ad3530r_device;

   // Set up the AD3530R device
   int32_t setup_ad3530r() {
       int32_t ret;
       uint16_t dac_value;

       // Initialize the device
       ret = ad3530r_init(&ad3530r_device, &init_param);
       if (ret) {
           printf("AD3530R initialization failed with error %d\n", ret);
           return ret;
       }

       // Configure channel 0 operating mode to normal mode
       ret = ad3530r_set_operating_mode(ad3530r_device, 0,
                                        AD3530R_CH_OPERATING_MODE_0);
       if (ret) {
           printf("Channel 0 configuration failed with error %d\n", ret);
           return ret;
       }

       // Set DAC value for channel 0 (mid-scale: 32768 for 16-bit)
       dac_value = 32768;
       ret = ad3530r_set_dac_value(ad3530r_device, dac_value, 0,
                                   AD3530R_WRITE_DAC_REGS);
       if (ret) {
           printf("Setting DAC value failed with error %d\n", ret);
           return ret;
       }

       printf("AD3530R setup completed successfully\n");
       return 0;
   }

   // Example of multi-channel synchronized update
   int32_t multi_channel_update() {
       int32_t ret;
       uint16_t dac_value = 40000; // Near full scale
       uint16_t channel_mask = 0x0F; // Channels 0-3

       // Set multiple channels simultaneously with LDAC trigger
       ret = ad3530r_set_multidac_value(ad3530r_device, dac_value,
                                        channel_mask,
                                        AD3530R_WRITE_INPUT_REGS_AND_TRIGGER_LDAC);
       if (ret) {
           printf("Multi-channel update failed with error %d\n", ret);
           return ret;
       }

       printf("Multi-channel update completed successfully\n");
       return 0;
   }

   // Main function for AD3530R demonstration
   int main() {
       int32_t status;

       // Setup the device
       status = setup_ad3530r();
       if (status) {
           return status;
       }

       // Perform multi-channel update
       status = multi_channel_update();
       if (status) {
           return status;
       }

       // Clean up resources
       status = ad3530r_remove(ad3530r_device);
       if (status) {
           printf("Device cleanup failed with error %d\n", status);
           return status;
       }

       printf("AD3530R demonstration completed successfully\n");
       return 0;
   }
