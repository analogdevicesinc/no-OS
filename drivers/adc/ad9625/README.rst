AD9625 no-OS Driver
===================

Supported Devices
-----------------

- :adi:`AD9625`

Overview
--------

The AD9625 is a 12-bit monolithic analog-to-digital converter (ADC)
optimized for high-speed applications with sampling rates up to 2.6GSPS. 
It supports wide bandwidth analog signals up to the second Nyquist zone, 
making it suitable for spectrum analyzers, radar, and 
electronic countermeasures. The ADC provides differential
inputs for analog, clock, and synchronous signals, with JESD204B-based
outputs configurable across one to eight lanes. Operating over a
temperature range of -40°C to +85°C, it ensures excellent linearity and
performance in demanding environments.

Applications
-------------

- Spectrum analyzers
- Military communications
- Radar systems
- High performance digital storage oscilloscopes
- Active jamming/anti-jamming
- Electronic surveillance and countermeasures

Operation Modes
----------------

+------------------------+-----------------+-----------+-----------------+
| Mode Name              | Description     | Config    | Typical Use     |
|                        |                 | Bits      | Case            |
+------------------------+-----------------+-----------+-----------------+
| AD9625_TEST_OFF        | No test mode is | 0x000     | Normal ADC      |
|                        | active          |           | operation       |
|                        |                 |           | without test    |
|                        |                 |           | patterns        |
+------------------------+-----------------+-----------+-----------------+
| AD9625_TEST_MID_SCALE  | Mid-scale input | 0x001     | Testing         |
|                        | test pattern    |           | mid-scale       |
|                        |                 |           | linearity       |
+------------------------+-----------------+-----------+-----------------+
| AD9625_TEST_POS_FSCALE | Positive        | 0x002     | Testing         |
|                        | full-scale      |           | positive        |
|                        | input test      |           | full-scale      |
|                        | pattern         |           | response        |
+------------------------+-----------------+-----------+-----------------+
| AD9625_TEST_NEG_FSCALE | Negative        | 0x003     | Testing         |
|                        | full-scale      |           | negative        |
|                        | input test      |           | full-scale      |
|                        | pattern         |           | response        |
+------------------------+-----------------+-----------+-----------------+
| AD9625_TEST_CHECKBOARD | Alternating     | 0x004     | Testing data    |
|                        | checkerboard    |           | integrity with  |
|                        | pattern         |           | alternating     |
|                        |                 |           | pattern         |
+------------------------+-----------------+-----------+-----------------+
| AD9625_TEST_PNLONG     | Long pattern    | 0x005     | Comprehensive   |
|                        | sequence        |           | long sequence   |
|                        |                 |           | test for        |
|                        |                 |           | pattern         |
|                        |                 |           | integrity       |
+------------------------+-----------------+-----------+-----------------+
| AD9625_TEST_ONE2ZERO   | Alternating 1   | 0x007     | Testing         |
|                        | to 0 test       |           | alternating     |
|                        | pattern         |           | toggle response |
+------------------------+-----------------+-----------+-----------------+
| AD9625_TEST_PATTERN    | Custom test     | 0x008     | Testing         |
|                        | pattern         |           | user-defined    |
|                        |                 |           | patterns        |
+------------------------+-----------------+-----------+-----------------+
| AD9625_TEST_RAMP       | Ramp test       | 0x00F     | Testing         |
|                        | pattern         |           | linearity       |
|                        |                 |           | across full     |
|                        |                 |           | range           |
+------------------------+-----------------+-----------+-----------------+

Device Configuration
---------------------

Device Setup and Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ``ad9625_setup`` function initializes and configures the AD9625,
involving memory allocation, SPI initialization, and register
configuration. Verification steps check the chip ID and PLL lock status
to ensure proper ADC configuration.

Resource Management
~~~~~~~~~~~~~~~~~~~

The ``ad9625_remove`` function deallocates resources associated with the
AD9625, removing the SPI descriptor and freeing allocated memory to
ensure proper resource release.

SPI Communication
~~~~~~~~~~~~~~~~~

The ``ad9625_spi_read`` and ``ad9625_spi_write`` functions handle SPI
interactions for the AD9625. The read function constructs a command to
fetch data from a register, while the write function sends a command to
write a byte of data to a register.

Testing and Validation
~~~~~~~~~~~~~~~~~~~~~~

The ``ad9625_test`` function configures the ADC for various operational
tests using a ‘test_mode’ parameter, setting the ADC’s test control
register accordingly. This allows for performance verification under
different conditions, ensuring the ADC operates correctly across various
setups.

Driver Initialization Example
-----------------------------

.. code-block:: C

   #include <stdint.h>
   #include "no_os_delay.h"
   #include "no_os_spi.h"

   // Initialization parameters for the AD9625 device
   struct ad9625_init_param {
       struct no_os_spi_init_param spi_init; // SPI configuration
       uint32_t lane_rate_kbps; // Lane rate in kbps
       uint32_t test_samples[4]; // Test samples
   };

   // Device structure for the AD9625
   struct ad9625_dev {
       struct no_os_spi_desc *spi_desc; // SPI descriptor
   };

   // Set up the AD9625 device
   int32_t ad9625_setup(struct ad9625_dev **device, struct ad9625_init_param init_param) {
       struct ad9625_dev *dev;
       int32_t ret;
       uint8_t chip_id;
       uint8_t pll_stat;

       // Memory allocation and SPI initialization
       dev = (struct ad9625_dev *)no_os_malloc(sizeof(*dev));
       ret = no_os_spi_init(&dev->spi_desc, &init_param.spi_init);
       if (ret < 0) {
           no_os_free(dev);
           return ret;
       }

       // Device register configuration
       ad9625_spi_write(dev, AD9625_REG_CHIP_PORT_CONF, 0x00);
       ad9625_spi_write(dev, AD9625_REG_OUTPUT_MODE, 0x01);
       ad9625_spi_write(dev, AD9625_REG_OUTPUT_ADJUST, 0x10);
       ad9625_spi_write(dev, AD9625_REG_JESD204B_LINK_CNTRL_1, 0x14);
       ad9625_spi_write(dev, AD9625_REG_TRANSFER, 0x01);
       no_os_mdelay(10);

       // Check chip ID and PLL status
       ad9625_spi_read(dev, AD9625_REG_CHIP_ID, &chip_id);
       if (chip_id != AD9625_CHIP_ID) {
           printf("%s Error: Invalid CHIP ID (0x%x).\n", __func__, chip_id);
           return -1;
       }
       ad9625_spi_read(dev, AD9625_REG_PLL_STATUS, &pll_stat);
       if ((pll_stat & 0x80) != 0x80) {
           printf("%s Error: AD9625 PLL is NOT locked (0x%x).\n", __func__, chip_id);
           return -1;
       }

       *device = dev;
       return ret;
   }

   // Main function for AD9625 initialization
   int main() {
       struct ad9625_dev *ad9625_device;
       struct ad9625_init_param init_param = {
           .spi_init = {/* SPI initialization parameters */},
           .lane_rate_kbps = 20000, // Example lane rate
           .test_samples = {0, 1, 2, 3} // Example test samples
       };
       
       int32_t status = ad9625_setup(&ad9625_device, init_param);
       if (status == 0) {
           printf("AD9625 setup successfully.\n");
       } else {
           printf("AD9625 setup failed with status %d.\n", status);
       }

       // Further application logic
       return 0;
   }
