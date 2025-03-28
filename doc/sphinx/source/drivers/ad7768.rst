AD7768 no-OS Driver
===================

Supported Devices
-----------------

- :adi:`AD7768`

Overview
--------

The :adi:`AD7768` is a high-performance, 8-channel, 24-bit simultaneous
sampling *Σ-Δ* analog-to-digital converter (ADC). It offers a dynamic
range of 108 dB and a maximum input bandwidth of 110.8 kHz, making it
suitable for precise data acquisition. The AD7768 supports configurable
power and performance modes, such as low power, median, and fast,
allowing trade-offs between input bandwidth, output data rate, and power
consumption. Each channel integrates a *Σ-Δ* modulator and digital
filter, providing extensive filtering options, including a wideband
low-pass filter and a sinc5 filter for low latency applications.
Additionally, the device is interfaced via serial peripheral interface
(SPI), supporting various configurations including power modes, clock
dividers, and conversion modes, making it versatile for applications in
industrial control systems and medical instrumentation.

Applications
------------

- Data acquisition systems
- Instrumentation and industrial control loops
- High precision medical instrumentation
- Vibration and asset condition monitoring
- Three-phase power quality analysis
- Audio testing and measurement
- Sonar applications

Operation Modes
---------------

+-----------------+-----------------+--------------------------+-------------------------+
| Mode Name       | Description     | Configuration Bits       | Typical Use Case        |
+-----------------+-----------------+--------------------------+-------------------------+
| Low Power Mode  | Minimizes power | AD7768_LOW_POWER_MODE    | Battery-operated        |
|                 | consumption.    |                          | devices requiring power |
|                 |                 |                          | efficiency.             |
+-----------------+-----------------+--------------------------+-------------------------+
| Median Mode     | Balances        | AD7768_MEDIAN_MODE       | Applications needing    |
|                 | performance and |                          | moderate speed and      |
|                 | power usage.    |                          | efficiency.             |
+-----------------+-----------------+--------------------------+-------------------------+
| Fast Mode       | Maximizes       | AD7768_FAST_MODE         | High-speed data         |
|                 | performance     |                          | acquisition with swift  |
|                 | with higher     |                          | processing needs.       |
|                 | power use.      |                          |                         |
+-----------------+-----------------+--------------------------+-------------------------+
| Standard        | Continuously    | AD7768_STANDARD_CONV     | Real-time systems       |
| Conversion      | converts        |                          | needing continuous      |
|                 | signals.        |                          | monitoring.             |
+-----------------+-----------------+--------------------------+-------------------------+
| One-shot        | Executes a      | AD7768_ONE_SHOT_CONV     | Trigger-based           |
| Conversion      | single          |                          | measurements like       |
|                 | conversion on   |                          | diagnostics or          |
|                 | request.        |                          | event-driven data       |
|                 |                 |                          | capture.                |
+-----------------+-----------------+--------------------------+-------------------------+

AD7768 Device Configuration
---------------------------

Initialization
^^^^^^^^^^^^^^

The ``ad7768_setup`` function orchestrates the complete initialization
of the AD7768 ADC by managing SPI and GPIO configurations and resources,
ensuring efficient error handling. The setup process is partitioned into
``ad7768_setup_begin`` and ``ad7768_setup_finish``.
``ad7768_setup_begin`` deals with the initial setup steps, such as
memory allocation for the device structure and configuring SPI
interfaces and GPIO settings. Subsequently, ``ad7768_setup_finish``
completes the configuration by setting operational parameters, ensuring
all necessary setups are in place for optimal device performance.

Communication
^^^^^^^^^^^^^

The ``ad7768_spi_read`` and ``ad7768_spi_write`` functions perform basic
SPI register reads and writes, essential for configuring the AD7768
device and handling data. In contrast, ``ad7768_spi_read_mask`` and
``ad7768_spi_write_mask`` support masked operations, allowing precise
bit-level control over register configurations through SPI.

Power and Mode Control
^^^^^^^^^^^^^^^^^^^^^^

The AD7768’s power and sleep mode functions are essential for optimizing
device performance and power consumption. The ``ad7768_set_power_mode``
function allows the AD7768 to switch between low power (eco), median,
and fast power modes, balancing power use and performance by writing the
selected mode to the device’s control registers.
``ad7768_get_power_mode`` retrieves the current setting to ensure
configurations are verifiable. For periods of inactivity,
``ad7768_set_sleep_mode`` puts the device in a low power sleep state or
restores it to active mode by adjusting the relevant register settings,
while ``ad7768_get_sleep_mode`` checks the current sleep state,
confirming the operational status. Together, these functions enable
dynamic adjustment of the AD7768’s power settings to meet varying
performance requirements efficiently.

Clock Management
^^^^^^^^^^^^^^^^

The ``ad7768_set_mclk_div`` and ``ad7768_get_mclk_div`` functions are
designed to modify and retrieve the master clock (MCLK) divider
settings, which influence the ADC sampling rate. Similarly, the
``ad7768_set_dclk_div`` and ``ad7768_get_dclk_div`` functions adjust and
obtain the data clock (DCLK) divider settings, which are crucial for
maintaining precise timing and synchronization during data transfer.
These functions enable precise clock configuration to optimize the ADC’s
performance and ensure data integrity based on specific application
requirements.

Channel and Conversion Operations
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The ``ad7768_set_ch_mode`` and ``ad7768_get_ch_mode`` functions define
and query ADC channel modes for data filtering and conversion settings
in the AD7768 device. The ``ad7768_set_conv_op`` and
``ad7768_get_conv_op`` functions configure and obtain conversion
operations, enabling selection between continuous or one-shot modes
based on application requirements. The ``ad7768_set_ch_state`` and
``ad7768_get_ch_state`` functions manage the activation or standby state
of ADC channels, facilitating selective channel operation.

Error Checking and CRC Functions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The ``ad7768_set_crc_sel`` and ``ad7768_get_crc_sel`` functions in the
AD7768 driver are used to configure and retrieve the cyclic redundancy
check (CRC) settings of the AD7768 ADC. The ``ad7768_set_crc_sel``
function allows selection among CRC options such as ``AD7768_NO_CRC``,
``AD7768_CRC_4``, or ``AD7768_CRC_16``, by writing the chosen value to
the device’s interface configuration register using the
``ad7768_spi_write_mask`` function, ensuring the integrity of data
transactions over SPI by verifying data transfer correctness. Meanwhile,
``ad7768_get_crc_sel`` retrieves the current CRC setting from the device
structure, allowing verification and confirmation of the CRC
configuration in use. Both functions return 0 on successful execution,
or a negative error code if an error occurs.

The AD7768/AD7768-4 use a CRC polynomial to calculate the CRC message.
The 8-bit CRC polynomial used is x8 + x2 + x + 1. The AD7768/AD7768-4 can 
be configured to output a CRC message per channel every 4 or 16 samples. 
This function is available only with SPI control. CRC is enabled in the 
interface configuration register, Register 0x07.


**Note:** When configuring the AD7768 device, verify that all configuration 
values are consistent with the specific platform or hardware in use, 
especially regarding clock speeds, power modes, and SPI settings. 
Accurate device-specific configurations are crucial to avoid performance 
or compatibility issues.


Driver Initialization Example
-----------------------------

 .. code-block:: C

   #include <xparameters.h>
   #include "ad7768.h"
   #include "xilinx_spi.h"
   #include "xilinx_gpio.h"
   #include "no_os_gpio.h"
   #include "no_os_spi.h"

   // Initializing the AD7768 device
   ad7768_init_param default_init_param = {
       .spi_init = {
           .device_id = SPI_DEVICE_ID,
           .max_speed_hz = 1000000,
           .chip_select = SPI_AD7768_CS,
           .mode = NO_OS_SPI_MODE_0,
           .platform_ops = &xil_spi_ops,
           .extra = NULL
       },
       .gpio_reset = {
           .number = GPIO_RESET_N,
           .platform_ops = &xil_gpio_ops,
           .extra = NULL
       },
       .gpio_reset_value = NO_OS_GPIO_HIGH,
       .mclk = 32768000,
       .datalines = 8
   };

   ad7768_dev *dev;
   int ret;

   // Begin AD7768 device setup
   ret = ad7768_setup_begin(&dev, default_init_param);
   if (ret) {
       printf("AD7768 setup begin failed\n");
       return ret;
   }

   // Reset AD7768 to ensure clean startup
   ret = no_os_gpio_set_value(dev->gpio_reset, NO_OS_GPIO_LOW);
   if (ret) return ret;
   no_os_udelay(100);
   ret = no_os_gpio_set_value(dev->gpio_reset, NO_OS_GPIO_HIGH);
   if (ret) return ret;
   no_os_udelay(1660);

   // Read and check the device revision ID
   uint8_t reg_data;
   ret = ad7768_spi_read(dev, AD7768_REG_REV_ID, &reg_data);
   if (ret || reg_data != 0x06) {
       printf("Device ID error: %#x\n", reg_data);
       return -1;
   }

   printf("AD7768 Rev ID %#x.\n", reg_data);

   // Finish the AD7768 device initialization
   ret = ad7768_setup_finish(dev, default_init_param);
   if (ret) {
       printf("AD7768 setup finish failed\n");
       return ret;
   }

   // Setup complete
   return 0;
