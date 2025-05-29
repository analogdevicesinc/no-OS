ADE9113 no-OS Driver
====================

Supported Devices
-----------------

- :adi:`ADE9103`
- :adi:`ADE9112`
- :adi:`ADE9113`

Overview
--------

The ADE9113 is an isolated, precision Σ-Δ analog-to-digital converter (ADC) 
tailored for energy metering, especially in shunt-based DC and polyphase systems. 
It features three concurrently sampled 24-bit Σ-Δ ADC channels with SNRs of
86 dBFS for the current channel and 91 dBFS for the voltage channels,
operating at 4 kSPS. The device includes safety-certified galvanic
isolation for signal and power. It is designed to support lower
resistance shunts, minimizing heat loss and optimizing performance for
DC metering and energy monitoring applications.

Applications
------------

- Shunt-based polyphase meters
- Electric vehicle supply equipment
- DC meters
- Power quality monitoring
- Solar inverters
- Process monitoring
- Protective devices
- Isolated sensor interfaces

Operation Modes
----------------

+--------------------------+------------------------------+------------------------+-------------------------------+
| Mode Name                | Description                  | Configuration Bits     | Typical Use Case              |
+--------------------------+------------------------------+------------------------+-------------------------------+
| ADE9113_STREAM_NORMAL    | Standard operation mode      | Not specified          | Used for regular operation    |
| MODE                     | for stream debugging.        |                        | without special data handling |
|                          |                              |                        | or debugging actions.         |
+--------------------------+------------------------------+------------------------+-------------------------------+
| ADE9113_STREAM_STATIC    | ADC registers hold values    | STREAM_DBG[3:2] = 01b  | Used to verify data           |
| MODE                     | until overwritten,           |                        | transmission integrity by     |
|                          | facilitating static data     |                        | outputting known ADC values.  |
|                          | read/write.                  |                        |                               |
+--------------------------+------------------------------+------------------------+-------------------------------+
| ADE9113_STREAM           | Data increments at ADC       | STREAM_DBG[3:2] = 10b  | Ensures continuous data       |
| _INCREMENTS_MODE         | conversion rate, available   |                        | reading/writing interface     |
|                          | for waveform streaming       |                        | integrity through incremented |
|                          | verification.                |                        | data streaming.               |
+--------------------------+------------------------------+------------------------+-------------------------------+
| ADE9113_STREAM           | Similar to ADE9113 normal    | Not specified          | Regular operational use       |
| _FUNCTIONAL_MODE         | functional mode, typically   |                        | similar to standard mode.     |
|                          | used in standard operation.  |                        |                               |
+--------------------------+------------------------------+------------------------+-------------------------------+
| ADE9113_L_OP             | Long read/write operations   | ADE9113_OP_MODE_LONG   | Used for comprehensive data   |
|                          | for extended communication   | (NO_OS_BIT(6))         | transactions with the device, |
|                          | sessions.                    |                        | involving more data than      |
|                          |                              |                        | covered by short operations.  |
+--------------------------+------------------------------+------------------------+-------------------------------+
| ADE9113_S_OP             | Short read/write operations  | Not specified          | Optimized for scenarios with  |
|                          | limiting communication to    |                        | smaller, less data-intensive  |
|                          | concise sessions.            |                        | communications.               |
+--------------------------+------------------------------+------------------------+-------------------------------+

Device Configuration
--------------------

Initialization and Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ADE9113 is initialized using ``ade9113_init``, setting up
communication descriptors, GPIO, IRQ configurations, and handling
daisy-chain configurations when necessary. Operational modes are
configured with ``ade9113_set_normal_mode``,
``ade9113_set_static_mode``, and ``ade9113_set_data_increments_mode``.
Digital signal processing parameters, including sampling rates and
filtering options, are set with ``ade9113_set_dsp_config``.

Communication Management
~~~~~~~~~~~~~~~~~~~~~~~~

SPI communication is managed through ``ade9113_read``,
``ade9113_read_dc``, ``ade9113_write``, and ``ade9113_write_dc`` for
register operations, supporting the daisy-chain setup. Communication
integrity is verified with CRC checks via ``ade9113_set_crc_en_state``
and ``ade9113_get_crc_en_state``. Soft and hard resets are executed
using ``ade9113_sw_reset`` and ``ade9113_hw_reset``.

Error and Interrupt Handling
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``ade9113_enable_status1x_int``, ``ade9113_disable_status1x_int``,
``ade9113_enable_com_up_int`` manage various interrupts. Zero crossing
detection configuration is managed by
``ade9113_select_zero_crossing_edge`` and
``ade9113_select_zero_crossing_channel``. Error counts are retrieved
with ``ade9113_get_err_count``, ensuring communication integrity.

Device Control and Utility
~~~~~~~~~~~~~~~~~~~~~~~~~~

Polarity inversion of input channels is controlled with
``ade9113_invert_i_inputs`` and ``ade9113_invert_v1_inputs``. ADC data
is converted to millivolts using ``ade9113_convert_to_millivolts``.
EFuse memory updates are refreshed with ``ade9113_efuse_refresh``.
Secure write operations are managed with ``ade9113_wr_lock`` and
``ade9113_wr_unlock``.

Driver Initialization Example
-----------------------------

.. code-block:: C

   #include "ade9113.h"
   #include "no_os_spi.h"
   #include "no_os_gpio.h"
   #include "no_os_irq.h"
   #include "no_os_uart.h"

   #define SPI_MAX_SPEED_HZ 10000000
   #define SPI_CHIP_SELECT 0
   #define GPIO_RDY_NUMBER 5
   #define GPIO_RESET_NUMBER 6
   #define IRQ_PLATFORM_OPS some_platform_irq_ops
   #define UART_BAUDRATE 115200

   // SPI, GPIO, and IRQ initialization parameters
   struct no_os_spi_init_param spi_init = {
       .max_speed_hz = 1000000,
       .chip_select = 0,
       .mode = NO_OS_SPI_MODE_0,
       .platform_ops = &max_spi_ops
   };

   struct no_os_gpio_init_param gpio_rdy_init = {
       .number = 15,
       .pull = NO_OS_PULL_NONE,
       .platform_ops = &max_gpio_ops
   };

   struct no_os_gpio_init_param gpio_reset_init = {
       .number = 0,
       .pull = NO_OS_PULL_NONE,
       .platform_ops = &max_gpio_ops
   };

   struct no_os_irq_ctrl_desc irq_ctrl = {
       .platform_ops = &max_gpio_irq_ops
   };

   // ADE9113 initialization parameters
   struct ade9113_init_param ade9113_ip = {
       .spi_init = &spi_init,
       .gpio_rdy = &gpio_rdy_init,
       .gpio_reset = &gpio_reset_init,
       .irq_ctrl = &irq_ctrl,
       .drdy_callback = NULL // Use the default callback
   };

   int main() {
       struct ade9113_dev *dev;
       int ret;

       // Initialize ADE9113 device
       ret = ade9113_init(&dev, &ade9113_ip);
       if (ret) {
           printf("Initialization failed with error: %d\n", ret);
           return ret;
       }

       // Set device to normal mode
       ret = ade9113_set_normal_mode(dev);
       if (ret) {
           printf("Failed to set normal mode: %d\n", ret);
           ade9113_remove(dev);
           return ret;
       }

       // Enable data ready interrupt
       ret = ade9113_drdy_int_enable(dev);
       if (ret) {
           printf("Failed to enable DRDY interrupt: %d\n", ret);
           ade9113_remove(dev);
           return ret;
       }

       printf("ADE9113 initialization successful.\n");
       // Implement additional functionality or application logic here

       // Clean up and release resources
       ade9113_remove(dev);
       return 0;
   }
