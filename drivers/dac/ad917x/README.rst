AD917x Family no-OS Driver
==========================

Supported Devices
-----------------

- :adi:`AD9171`
- :adi:`AD9172`
- :adi:`AD9173`
- :adi:`AD9174`
- :adi:`AD9175`
- :adi:`AD9176`

Overview
--------

The AD9172 is a high-performance, dual, 16-bit digital-to-analog
converter (DAC) that supports DAC sample rates up to 12.6 GSPS, making
it ideal for advanced wireless communications infrastructure and
multiband base station radios. It supports multiband wireless
applications and provides three bypassable, complex data input channels
per RF DAC. Each channel is equipped with an independent numerical
controlled oscillator (NCO) and a selectable interpolation filter,
enabling flexible multiband frequency planning at a maximum complex
input data rate of 1.54 GSPS per channel. The AD9172’s proprietary, low
spurious design achieves excellent performance metrics, including a
2-tone intermodulation distortion (IMD) of −83 dBc at 1.8 GHz and a
spurious free dynamic range (SFDR) of <−80 dBc. Additionally, its
flexible 8-lane, 15.4 Gbps JESD204B interface makes it a robust solution
for high-bandwidth applications.

Applications
------------

- Wireless communications infrastructure
- Multiband base station radios
- Microwave/E-band backhaul systems
- Instrumentation, automatic test equipment (ATE)
- Radars and jammers

Operation Modes
----------------

+---------------------+-------------------------------+-------------------------------+-------------------------------+
| Mode Name           | Description                   | Configuration                 | Typical Use                   |
|                     |                               | Bits                          |                               |
+=====================+===============================+===============================+===============================+
| JESD Mode           | Configures the high-speed     | AD917X_JESD_MODE_REG          | Configuring the JESD          |
|                     | serial JESD interface, link   | with AD917X_JESD_MODE(x) and  | interface for serial data     |
|                     | selection and mode validation.| AD917X_LINK_MODE              | transfer between the DAC      |
|                     |                               |                               | and processing elements.      |
+---------------------+-------------------------------+-------------------------------+-------------------------------+
| DDSM Mode           | Sets up the Delta-Sigma       | AD917X_DDSM_DATA-PATH         | Enabling precise frequency    |
|                     | Digital Modulator datapath,   | _CFG_REG using                | conversion and digital        |
|                     | controlling modulation and    | AD917X_DDSM_MODE(x),          | dithering in applications     |
|                     | NCO features.                 | AD917X_DDSM_NCO_EN,           | requiring signal modulation.  |
|                     |                               | AD917X_DDSM_MODULUS_EN,       |                               |
|                     |                               | AD917X_DDSM_SEL_SIDEBAND,     |                               |
|                     |                               | AD917X_DDSM_EN_SYNC_ALL       |                               |
|                     |                               | _CHNL_NCO_RESETS              |                               |
+---------------------+-------------------------------+-------------------------------+-------------------------------+
| DDSC Mode           | Configures the alternate      | AD917X_DDSC_DATA-PATH         | Generating calibration test   |
|                     | digital synthesis engine,     | _CFG_REG with                 | tones or steady frequency     |
|                     | test tone generation and      | AD917X_DDSC_NCO_EN,           | signals for system            |
|                     | modulation control.           | AD917X_DDSC_MODULUS_EN,       | verification and diagnostics. |
|                     |                               | AD917X_DDSC_SEL_SIDEBAND,     |                               |
|                     |                               | AD917X_DDSC_TEST_TONE_EN      |                               |
+---------------------+-------------------------------+-------------------------------+-------------------------------+
| Integer NCO Mode    | Operates using an integer     | AD917X_DDSM_NCO_EN and        | Used when the desired         |
|                     | frequency tuning word with    | AD917X_DDSM_MODE(0);          | carrier frequency is an       |
|                     | modulus disabled. The FTW is  | AD917X_DDSC_NCO_EN without    | integer division of the DAC   |
|                     | set directly without          | AD917X_DDSC_MODULUS_EN        | frequency.                    |
|                     | fractional components.        |                               |                               |
+---------------------+-------------------------------+-------------------------------+-------------------------------+
| Modulus NCO Mode    | Enables fractional frequency  | AD917X_DDSM_MODULUS_EN and    | Applied when fine frequency   |
|                     | tuning by configuring the FTW | AD917X_DDSC_MODULUS_EN,       | adjustments are required and  |
|                     | with a non-zero accumulator   | along with AD917X_DDSM        | the frequency is not an       |
|                     | modulus and delta values.     | _NCO_EN and AD917X_DDSC       | integer division of the DAC   |
|                     |                               | _NCO_EN                       | frequency.                    |
+---------------------+-------------------------------+-------------------------------+-------------------------------+
| JESD Single Link    | Configures the JESD datapath  | AD917X_JESD_MODE(jesd_mode)   | Suitable for applications     |
| Mode                | for operation with a single   | with the AD917X_LINK_MODE     | where lower data throughput   |
|                     | link, dual-link flag disabled.| bit cleared                   | and simplified lane mapping   |
|                     |                               |                               | are sufficient.               |
+---------------------+-------------------------------+-------------------------------+-------------------------------+
| JESD Dual Link Mode | Sets up the JESD datapath for | AD917X_JESD_MODE(jesd_mode)   | Ideal for scenarios           |
|                     | dual-link operation,          | combined with AD917X_         | requiring increased           |
|                     | increasing throughput by      | LINK_MODE bit set             | throughput by using multiple  |
|                     | activating the dual-link flag.|                               | JESD lanes concurrently.      |
+---------------------+-------------------------------+-------------------------------+-------------------------------+
| SYSREF_NONE         | Disables SYSREF support.      | SYSREF_NONE                   | Used when SYSREF              |
|                     |                               |                               | synchronization is not        |
|                     |                               |                               | required.                     |
+---------------------+-------------------------------+-------------------------------+-------------------------------+
| SYSREF_ONESHOT      | Activates one-shot SYSREF     | SYSREF_ONESHOT                | Ideal for single              |
|                     | mode to trigger a single      |                               | synchronization events        |
|                     | synchronization event.        |                               | during initialization or      |
|                     |                               |                               | calibration.                  |
+---------------------+-------------------------------+-------------------------------+-------------------------------+
| SYSREF_CONT         | Enables continuous SYSREF     | SYSREF_CONT                   | Best for applications that    |
|                     | synchronization, providing    |                               | need ongoing, periodic        |
|                     | periodic events.              |                               | synchronization.              |
+---------------------+-------------------------------+-------------------------------+-------------------------------+
| SYSREF_MON          | Sets the driver to SYSREF     | SYSREF_MON                    | Useful when it is necessary   |
|                     | monitor mode, observing       |                               | to monitor SYSREF signals     |
|                     | SYSREF signals without        |                               | without actively              |
|                     | synchronizing.                |                               | synchronizing.                |
+---------------------+-------------------------------+-------------------------------+-------------------------------+
| Mode 0              | DAC0 outputs I0 and DAC1      | AD9172_MODE_0                 | Standard dual-channel         |
|                     | outputs I1.                   |                               | operation with independent    |
|                     |                               |                               | I/Q paths.                    |
+---------------------+-------------------------------+-------------------------------+-------------------------------+
| Mode 1              | DAC0 outputs I0 + I1 and      | AD9172_MODE_1                 | Combined channel output for   |
|                     | DAC1 outputs Q0 + Q1.         |                               | summed signal processing.     |
+---------------------+-------------------------------+-------------------------------+-------------------------------+
| Mode 2              | DAC0 outputs I0 and DAC1      | AD9172_MODE_2                 | Direct mapping of separate    |
|                     | outputs Q0.                   |                               | I and Q signals.              |
+---------------------+-------------------------------+-------------------------------+-------------------------------+
| Mode 3              | DAC0 outputs I0 + I1 and      | AD9172_MODE_3                 | Used for single-channel       |
|                     | DAC1 outputs 0.               |                               | operation when one DAC        |
|                     |                               |                               | output is disabled.           |
+---------------------+-------------------------------+-------------------------------+-------------------------------+
| Direct Mode         | DAC0 carries I0 and DAC1      | AD9172_DDSM_MODE (0x0)        | Standard dual-channel         |
|                     | carries I1.                   |                               | operation with independent    |
|                     |                               |                               | I and Q signals.              |
+---------------------+-------------------------------+-------------------------------+-------------------------------+
| Sum Mode            | DAC0 carries the sum of I0    | AD9172_DDSM_MODE (0x1)        | Suitable for applications     |
|                     | and I1, DAC1 carries Q0 + Q1. |                               | requiring combined outputs    |
|                     |                               |                               | for increased driving power.  |
+---------------------+-------------------------------+-------------------------------+-------------------------------+

Device Configuration
--------------------

Initialization
~~~~~~~~~~~~~~

The ``ad9172_init()`` function is responsible for setting up the AD9172 DAC 
by configuring the necessary communication interfaces such as SPI and GPIO, 
allocating memory for the device descriptor and state, and transferring the
initialization parameters into the driver’s internal structures. This
function initializes all essential registers to ensure the device is
ready for operation and must be the first API invoked during the device
lifecycle.

JESD Interface Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ``ad917x_jesd_set_scrambler_enable()`` function enables or
disables the descrambler for the JESD interface and adjusts the
scrambler settings by taking in the device handle and the appropriate
control parameter. The function must be called after successful
initialization to properly configure the high-speed JESD data transfer
path.

Resource Removal
~~~~~~~~~~~~~~~~

``ad9172_remove()`` function is designed to gracefully de-allocate resources that
were previously allocated during device initialization and
configuration. It handles the removal of SPI and GPIO descriptors and
frees any memory associated with device state or descriptor structures,
ensuring that system resources are reclaimed without causing leaks.

Driver Initialization Example
-----------------------------

.. code-block:: C

   #include <stdio.h>
   #include <stdlib.h>
   #include "ad9172.h"
   #include "no_os_gpio.h"
   #include "no_os_spi.h"
   #include "no_os_delay.h"
   #include <stdint.h>
   #include <errno.h>
   #include <string.h>
   #include <inttypes.h>
   #include "no_os_alloc.h"
   #include "api_error.h"

   struct xil_spi_init_param xil_spi_param = {
       .type = SPI_PS,
       .flags = 0
   };

   ad9172_init_param default_init_param = {
       .spi_init = {
           .device_id = XPAR_PSU_SPI_0_DEVICE_ID,
           .max_speed_hz = 1000000,
           .mode = NO_OS_SPI_MODE_0,
           .chip_select = SPI_AD9172_CS,
           .platform_ops = &xil_spi_ops,
           .extra = &xil_spi_param
       },
       .gpio_txen0 = {
           .number = 76,
           .platform_ops = &xil_gpio_ops,
           .extra = &xilinx_gpio_init_param
       },
       .gpio_txen1 = {
           .number = 77,
           .platform_ops = &xil_gpio_ops,
           .extra = &xilinx_gpio_init_param
       },
       .gpio_reset = {
           .number = 54,
           .platform_ops = &xil_gpio_ops,
           .extra = &xilinx_gpio_init_param
       },
       .dac_rate_khz = 11796480,
       .dac_clkin_Hz = 368640000,
       .jesd_link_mode = 4,
       .jesd_subclass = 1,
       .dac_interpolation = 8,
       .channel_interpolation = 4,
       .clock_output_config = 4,
       .syncoutb_type = SIGNAL_LVDS,
       .sysref_coupling = COUPLING_AC,
       .dac_freq_hz = 368640000,
       .dev_xfer = client_spi_xfer,
       .delay_us = client_delay_us,
       .tx_en_pin_ctrl = client_tx_en_pin_ctrl,
       .reset_pin_ctrl = client_reset_pin_ctrl,
       .hw_open = client_hw_open,
       .hw_close = client_hw_close
   };

   ad9172_dev *device;
   int ret;

   ret = ad9172_init(&device, &default_init_param);
   if (ret) {
       printf("AD9172 initialization failed: %s\n", adi_api_get_error_string(ret));
       goto error;
   }
   printf("AD9172 successfully initialized\n");

   uint8_t revision[3] = {0, 0, 0};
   adi_chip_id_t dac_chip_id;

   ret = ad917x_get_chip_id(device, &dac_chip_id);
   if (ret) {
       printf("Failed to get chip ID: %s\n", adi_api_get_error_string(ret));
       goto error;
   }

   ret = ad917x_get_revision(device, &revision[0], &revision[1], &revision[2]);
   if (ret) {
       printf("Failed to get revision: %s\n", adi_api_get_error_string(ret));
       goto error;
   }

   printf("*********************************************\n");
   printf("AD917x DAC Chip ID: %d\n", dac_chip_id.chip_type);
   printf("AD917x DAC Product ID: %d\n", dac_chip_id.prod_id);
   printf("AD917x DAC Product Grade: %d\n", dac_chip_id.prod_grade);
   printf("AD917x DAC Product Revision: %d\n", dac_chip_id.dev_revision);
   printf("AD917x Revision: %d.%d.%d\n", revision[0], revision[1], revision[2]);
   printf("*********************************************\n");

   goto finish;

   error:
       printf("Error occurred during AD9172 initialization: %s\n", adi_api_get_error_string(ret));

   finish:
       ;
