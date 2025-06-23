AD3552R no-OS Driver
=====================

Supported Devices
-----------------

- :adi:`AD3552R`

Overview
--------

The AD3552R is a dual-channel digital-to-analog converter (DAC) designed
for precision signal processing applications, supporting bipolar (-10V
to 10V) and unipolar (0V to 5V) output configurations. It operates
via SPI communication in single, dual, and quad modes. The DAC features
adjustable gain and offset, a scratch pad test, and allows for both
hardware and software resets. Dedicated registers provide control over
channel attributes, streaming, and input/output settings.

Applications
-------------

- Instrumentation
- Hardware in the loop
- Process control equipment
- Medical devices
- Automated test equipment
- Data acquisition systems
- Programmable voltage sources
- Optical communications

Operation Modes
---------------

+------------------+--------------------------------+----------------------------------+--------------------------------+
| Mode Name        | Description                    | Configuration                    | Typical Use                    |
+------------------+--------------------------------+----------------------------------+--------------------------------+
| Single           | SPI mode with                  | *AD3552R_SPI_MULTI_IO_MODE*      | Basic applications             |
|                  | unidirectional SDI/SDIO0       | set to 0                         | requiring standard SPI         |
|                  | and SDO/SDIO1 lines for        |                                  | communication.                 |
|                  | basic serial communication.    |                                  |                                |
+------------------+--------------------------------+----------------------------------+--------------------------------+
| Dual             | Bidirectional communication    | Set *multi_io_mode* to 1         | Applications requiring         |
|                  | on SDI/SDIO lines for          | in *ad3552_transfer_config*      | higher data throughput.        |
|                  | enhanced throughput.           |                                  |                                |
+------------------+--------------------------------+----------------------------------+--------------------------------+
| Quad             | Utilizes four lines for        | Set *multi_io_mode* to 2         | High-speed data-               |
|                  | data transfer for              | in *ad3552_transfer_config*      | intensive applications.        |
|                  | significantly increased        |                                  |                                |
|                  | throughput.                    |                                  |                                |
+------------------+--------------------------------+----------------------------------+--------------------------------+
| DDR Mode         | Double Data Rate transfer      | *AD3552R_MASK_SPI_CONFIG_DDR*    | Situations requiring           |
|                  | using both edges of the        | used to enable this mode         | rapid data updates.            |
|                  | clock for faster data          |                                  |                                |
|                  | transfers.                     |                                  |                                |
+------------------+--------------------------------+----------------------------------+--------------------------------+
| Synchronous      | Dual SPI synchronous mode      | *AD3552R_SPI_SYNCHRONOUS_ENABLE* | Systems needing                |
|                  | for synchronized data          | for enabling this feature        | synchronized operations        |
|                  | transfer across channels.      |                                  | for data integrity.            |
+------------------+--------------------------------+----------------------------------+--------------------------------+

Device Configuration
--------------------

Initialization
~~~~~~~~~~~~~~

The ``ad3552r_init`` function initializes the AD3552R DAC, configuring
SPI or AXI interfaces based on parameters and setting up GPIOs for reset
and LDAC signals. It includes programming CRC settings for data
integrity and writing initial configuration registers.

Data Transfer and Access
~~~~~~~~~~~~~~~~~~~~~~~~

Key functions for data transfer include ``ad3552r_write_reg``, which
writes data to specific registers, and ``ad3552r_transfer``, which
handles data transfer operations. For read operations,
``ad3552r_read_reg`` retrieves data from specified registers.

Configuration and Mode Setting
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Functions like ``ad3552r_set_dev_value`` adjust specific device
settings, such as enabling or disabling CRC. The ``ad3552r_axi_init``
function is used for additional initialization when configured in AXI
mode. ``_ad3552r_update_reg_field`` updates specific fields in
configuration registers.

Error Handling
~~~~~~~~~~~~~~

Error handling is managed by functions like ``ad3552r_get_status``,
which retrieves and optionally clears error states. This functionality
assists in managing and reporting errors related to interface
configurations, CRC mismatches, and access attempts to invalid
registers.

Reset and Utilities
~~~~~~~~~~~~~~~~~~~

The ``ad3552r_reset`` function handles resetting the device to its
initial state. It is important for re-initialization and
troubleshooting. Additional utility functions exist to support various
device-specific operations.

Initialization and Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ``ad3552r_init`` function initializes the AD3552R DAC, setting up
SPI or AXI interfaces and configuring necessary GPIOs. Additional
configurations may include enabling CRC and setting the device’s
operational parameters. Functions like ``ad3552r_reset`` are used to
restore the device to its initial state, important for both
initialization and troubleshooting. ``ad3552r_axi_init`` is particularly
used when configured in AXI mode. The reset capabilities are crucial to
establish a known state for successful operations.

Register Access and Data Transfer
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Functions such as ``ad3552r_write_reg`` and ``ad3552r_read_reg`` are
designed for register access to update settings or retrieve data from
the DAC. ``ad3552r_single_transfer`` and ``ad3552r_transfer`` handle
data transactions, ensuring efficient communication via SPI or AXI
setups. Operations like ``ad3552r_write_simulatneously`` allow multiple
channel updates, enhancing the DAC’s data throughput in high-speed
applications. DMA support is facilitated through the
``ad3552r_axi_write_data`` for bulk data operations.

Device Control and Customization
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Functions under this category, like ``ad3552r_set_dev_value`` and
``ad3552r_get_dev_value``, handle device parameter adjustments.
``ad3552r_config_custom_gain`` and ``ad3552r_configure_device`` allow
configuration of custom output ranges and gain. ``ad3552r_ldac_trigger``
is utilized for asynchronous operations. These functions enable
adaptation of the DAC to specific operational needs, ensuring accurate
performance.

Error Handling
~~~~~~~~~~~~~~

Error detection and handling functions like ``ad3552r_get_status``
provide mechanisms to monitor and address device malfunctions. This is
crucial for identifying issues such as CRC mismatches or illegal
accesses, ensuring system reliability. Error states can be retrieved for
diagnostic purposes, facilitating robust device integration.

Utility and Support
~~~~~~~~~~~~~~~~~~~

Utility functions, including ``ad3552r_remove``, manage device cleanup
and resource deallocation. Functions like ``ad3552r_check_scratch_pad``
and ``ad3552r_get_scale`` offer additional support for device
diagnostics and calibration checks. This holistic approach covers
various operational needs, enhancing device management and support
throughout its lifecycle.

Device Initialization Example
-----------------------------

.. code-block:: C
    
   #include "ad3552r.h"
   static struct ad3552r_init_param default_ad3552r_param = {
       .chip_id = 0x4008,
       .spi_param = {
           .device_id = 1,
           .chip_select = 0,
           .mode = NO_OS_SPI_MODE_0,
           .max_speed_hz = 66000000,
           .bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
           .platform_ops = &xil_spi_ops,
           .extra = SPI_EXTRA
       },
       .ldac_gpio_param_optional = &gpio_ldac_param,
       .reset_gpio_param_optional = &gpio_reset_param,
       .sdo_drive_strength = 1,
       .channels = {
           [0] = {
               .en = 1,
               .range = AD3552R_CH_OUTPUT_RANGE_NEG_10__10V,
           },
           [1] = {
               .en = 1,
               .range = AD3552R_CH_OUTPUT_RANGE_NEG_10__10V,
           }
       },
       .crc_en = 0,
       .single_transfer = 1,
   };
   int example_main()
   {
       int32_t err;
       struct ad3552r_desc *dac;
       err = init_gpios_to_defaults();
       if (NO_OS_IS_ERR_VALUE(err)) {
           pr_err("init_gpios_to_defaults failed: %d\n", err);
           return err;
       }
       err = ad3552r_init(&dac, &default_ad3552r_param);
       if (NO_OS_IS_ERR_VALUE(err)) {
           pr_err("ad3552r_init failed with code: %d\n", err);
           return err;
       }
       set_power_up_success_led();
       err = run_example(dac);
       if (NO_OS_IS_ERR_VALUE(err)) {
           pr_debug("Example failed with code: %d\n", err);
           return err;
       }
       ad3552r_remove(dac);
       return 0;
   }
