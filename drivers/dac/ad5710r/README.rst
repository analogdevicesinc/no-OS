AD5710R no-OS Driver
====================

.. no-os-doxygen::

Supported Devices
-----------------

`AD5710R <https://www.analog.com/en/products/ad5710r.html>`_

Overview
--------

The AD5710R is an 8-channel, 16-bit precision digital-to-analog converter (DAC)
with On-chip reference. It features
both voltage and current output modes, making it versatile for driving various 
industrial loads. The device supports SPI communication with CRC error 
checking, hardware and software LDAC triggering, and multiple operating modes 
per channel. The device includes internal/external voltage reference options, 
flexible output ranges.

Applications
-------------

- Optical networking
- Instrumentation
- Data acquisition
- Automatic test equipment
- Process control
- Industrial automation

Output Modes
------------

+---------------------------+-----------------+-----------+-----------------+
| Output Mode               | Description     | Value     | Typical Use     |
|                           |                 |           | Case            |
+---------------------------+-----------------+-----------+-----------------+
| IMODE                     | Current output  | 0x0       | Driving voltage |
|                           | mode            |           | controlled      |
|                           |                 |           | devices         |
+---------------------------+-----------------+-----------+-----------------+
| VMODE                     | Voltage output  | 0x1       | Driving current |
|                           | mode            |           | controlled      |
|                           |                 |           | devices         |
+---------------------------+-----------------+-----------+-----------------+

Write Modes
-----------

+--------------------------------------------+------------------------+-------------------+
| Write Mode                                 | Description            | Typical Use Case  |
+--------------------------------------------+------------------------+-------------------+
| AD5710R_WRITE_DAC_REGS                     | Write directly to DAC  | Immediate output  |
|                                            | registers, immediate   | update            |
|                                            | output update          |                   |
+--------------------------------------------+------------------------+-------------------+
| AD5710R_WRITE_INPUT_REGS                   | Write to input         | Synchronized      |
|                                            | registers, manual      | updates with LDAC |
|                                            | LDAC trigger required  |                   |
+--------------------------------------------+------------------------+-------------------+
| AD5710R_WRITE_INPUT_REGS_AND_TRIGGER_LDAC  | Write to input         | Synchronized      |
|                                            | registers and auto     | updates with LDAC |
|                                            | trigger LDAC           |                   |
+--------------------------------------------+------------------------+-------------------+

Device Configuration
---------------------

Device Setup and Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ``ad5710r_init`` function initializes and configures the AD5710R,
involving memory allocation, SPI initialization, GPIO setup for RESET and LDAC
pins, and comprehensive device configuration. The initialization process includes
product ID verification, scratch pad testing, and configuration of operating
modes, output ranges, reference selection, and CRC settings.

Resource Management
~~~~~~~~~~~~~~~~~~~

The ``ad5710r_remove`` function deallocates resources associated with the
AD5710R, removing GPIO descriptors, SPI descriptor, and freeing allocated
memory to ensure proper resource release and prevent memory leaks.

SPI Communication
~~~~~~~~~~~~~~~~~

The ``ad5710r_reg_read`` and ``ad5710r_reg_write`` functions handle SPI
interactions for the AD5710R. These functions support both short and long
instruction modes, CRC error checking, and multi-drop configurations.

Channel Configuration
~~~~~~~~~~~~~~~~~~~~~

Individual channels can be configured for voltage or current output modes using
``ad5710r_channel_output_select``. Operating modes are set per channel using
``ad5710r_set_operating_mode``, allowing fine-tuned control of each DAC output.

DAC Value Setting
~~~~~~~~~~~~~~~~~

DAC values can be set using ``ad5710r_set_dac_value`` for individual channels
or ``ad5710r_set_multidac_value`` for simultaneous updates across multiple
channels. The driver supports different write modes for immediate updates or
synchronized updates using LDAC triggers.

Hardware Features
~~~~~~~~~~~~~~~~~

The driver supports hardware LDAC triggering via GPIO pin using
``ad5710r_hw_ldac_trigger`` and software LDAC triggering using
``ad5710r_sw_ldac_trigger``. Reference voltage can be configured as internal
or external using ``ad5710r_set_reference``.

Driver Initialization Example
-----------------------------

.. code-block:: C

   #include <stdint.h>
   #include "no_os_delay.h"
   #include "no_os_spi.h"
   #include "no_os_gpio.h"
   #include "ad5710r.h"

   // Initialization parameters for the AD5710R device
   struct ad5710r_init_param init_param = {
       .chip_id = AD5710R_ID,
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
       .vref_enable = AD5710R_INTERNAL_VREF_PIN_2P5V,  // Internal 2.5V reference
       .chn_op_mode = {
           AD5710R_CH_OPERATING_MODE_0, // Channel 0
           AD5710R_CH_OPERATING_MODE_0, // Channel 1
           AD5710R_CH_OPERATING_MODE_0, // Channel 2
           AD5710R_CH_OPERATING_MODE_0, // Channel 3
           AD5710R_CH_OPERATING_MODE_0, // Channel 4
           AD5710R_CH_OPERATING_MODE_0, // Channel 5
           AD5710R_CH_OPERATING_MODE_0, // Channel 6
           AD5710R_CH_OPERATING_MODE_0  // Channel 7
       },
       .range = AD5710R_CH_OUTPUT_RANGE_0_VREF, // 0V to VREF range
       .hw_ldac_mask = 0x00, // No hardware LDAC enabled
       .sw_ldac_mask = 0xFF, // All channels software LDAC enabled
       .crc_en = 0,          // Disable CRC error checking
       .mux_out_sel = VOUT0, // Monitor channel 0 voltage output
       .dev_addr = 0         // Device address for multi-drop
   };

   // Device structure for the AD5710R
   struct ad5710r_desc *ad5710r_device;

   // Set up the AD5710R device
   int32_t setup_ad5710r() {
       int32_t ret;
       uint16_t dac_value;

       // Initialize the device
       ret = ad5710r_init(&ad5710r_device, &init_param);
       if (ret) {
           printf("AD5710R initialization failed with error %d\n", ret);
           return ret;
       }

       // Configure channel 0 for voltage output mode
       ret = ad5710r_channel_output_select(ad5710r_device, 0, VMODE);
       if (ret) {
           printf("Channel 0 configuration failed with error %d\n", ret);
           return ret;
       }

       // Set DAC value for channel 0 (mid-scale: 32768 for 16-bit)
       dac_value = 32768;
       ret = ad5710r_set_dac_value(ad5710r_device, dac_value, 0, 
                                   AD5710R_WRITE_DAC_REGS);
       if (ret) {
           printf("Setting DAC value failed with error %d\n", ret);
           return ret;
       }

       printf("AD5710R setup completed successfully\n");
       return 0;
   }

   // Example of multi-channel synchronized update
   int32_t multi_channel_update() {
       int32_t ret;
       uint16_t dac_value = 40000; // Near full scale
       uint8_t channel_mask = 0x0F; // Channels 0-3

       // Set multiple channels simultaneously with LDAC trigger
       ret = ad5710r_set_multidac_value(ad5710r_device, dac_value, 
                                        channel_mask, 
                                        AD5710R_WRITE_INPUT_REGS_AND_TRIGGER_LDAC);
       if (ret) {
           printf("Multi-channel update failed with error %d\n", ret);
           return ret;
       }

       printf("Multi-channel update completed successfully\n");
       return 0;
   }

   // Main function for AD5710R demonstration
   int main() {
       int32_t status;

       // Setup the device
       status = setup_ad5710r();
       if (status) {
           return status;
       }

       // Perform multi-channel update
       status = multi_channel_update();
       if (status) {
           return status;
       }

       // Clean up resources
       status = ad5710r_remove(ad5710r_device);
       if (status) {
           printf("Device cleanup failed with error %d\n", status);
           return status;
       }

       printf("AD5710R demonstration completed successfully\n");
       return 0;
   }