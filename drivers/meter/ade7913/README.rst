ADE7913 no-OS Driver
=====================

Supported Devices
------------------

- :adi:`ADE7912`
- :adi:`ADE7913`

Overview
---------

The ADE7912/ADE7913 are isolated, 3-channel Σ-Δ ADCs designed for
polyphase energy metering applications using shunt current sensors. Data
and power isolation are based on Analog Devices, Inc.’s iCoupler®
technology, ensuring robust operational performance in environments
requiring high reliability and stringent electrical isolation. The
devices feature two (ADE7912) or three (ADE7913) isolated, Σ-Δ
analog-to-digital converters with an integrated iso power, isolated
DC-to-DC converter, on-chip temperature sensor, and 4-wire SPI serial
interface. Additionally, up to four ADE7912/ADE7913 devices can be
clocked from a single crystal or an external clock with synchronization
of multiple devices. The current ADC provides a 67 dB signal-to-noise
ratio (SNR) over a 3 kHz signal bandwidth, while the voltage ADCs
deliver an SNR of 72 dB over the same bandwidth.

Applications
-------------

- Shunt-based polyphase meters
- Power quality monitoring
- Solar inverters
- Process monitoring
- Protective devices
- Isolated sensor interfaces
- Industrial PLCs

Operation Modes
----------------

+---------------------+-------------------------+-------------------------+-------------------------------+
| Mode Name           | Description             | Configuration           | Typical Use                   |
+---------------------+-------------------------+-------------------------+-------------------------------+
| Clock Output        | Enables or disables     | ADE7913_CLKOUT_EN_MSK   | When system synchronization   |
|                     | the clock output        |                         | is required with an external  |
|                     | signal via the clkout   |                         | clock source.                 |
|                     | enable function.        |                         |                               |
+---------------------+-------------------------+-------------------------+-------------------------------+
| Power Down Mode     | Activates power down    | ADE7913_PWRDWN_EN_MSK   | In battery-powered or         |
|                     | mode to reduce          |                         | energy-sensitive applications.|
|                     | consumption during      |                         |                               |
|                     | idle periods using      |                         |                               |
|                     | the power down enable   |                         |                               |
|                     | function.               |                         |                               |
+---------------------+-------------------------+-------------------------+-------------------------------+
| Temperature Sensing | Enables measurement     | ADE7913_TEMP_EN_MSK     | For systems needing           |
| Mode                | of the device’s         |                         | temperature monitoring for    |
|                     | internal temperature    |                         | performance or safety.        |
|                     | through the temperature |                         |                               |
|                     | enable function.        |                         |                               |
+---------------------+-------------------------+-------------------------+-------------------------------+
| ADC Frequency Mode  | Configures the ADC      | ADE7913_ADC_FREQ_MSK    | To tailor the sampling rate   |
|                     | conversion rate by      |                         | to the application’s          |
|                     | selecting one of the    |                         | performance requirements.     |
|                     | available ADC frequency |                         |                               |
|                     | configurations via the  |                         |                               |
|                     | provided enumeration.   |                         |                               |
+---------------------+-------------------------+-------------------------+-------------------------------+
| Digital LPF         | Selects the digital     | ADE7913_BW_MSK          | When attenuating high         |
| Bandwidth Mode      | low-pass filter         |                         | frequency noise is necessary  |
|                     | bandwidth applied to    |                         | for improved measurement      |
|                     | the measurements via    |                         | accuracy.                     |
|                     | the digital LPF         |                         |                               |
|                     | bandwidth select        |                         |                               |
|                     | function.               |                         |                               |
+---------------------+-------------------------+-------------------------+-------------------------------+

Device Configuration
---------------------

Initialization
~~~~~~~~~~~~~~~

The ADE7913 driver provides essential functions for proper device
initialization and lifecycle management. The function ``ade7913_init()``
initializes the ADE7913 device by setting up the necessary parameters
and configurations to ready the device for operation, while the
complementary function ``ade7913_remove()`` properly releases allocated
resources when the device is no longer needed.

Register Operations
~~~~~~~~~~~~~~~~~~~

For effective handling of the device’s registers, the ADE7913 driver
offers a set of functions that facilitate both individual and group
register transactions. The functions ``ade7913_read()`` and
``ade7913_write()`` are used to read from and write to individual device
registers respectively. For operations involving multiple devices, the
functions ``ade7913_read_waveforms()`` and ``ade7913_write_broadcast()``
allow for simultaneous data transactions and broadcast configurations.

Configuration Options
~~~~~~~~~~~~~~~~~~~~~

The ADE7913 driver supports various configuration options to tailor its
operation to specific application requirements. The driver enables
configuration of several device features through the functions
``ade7913_set_clkout_en()``, ``ade7913_pwrdwn()``, and
``ade7913_temp_en()``, which are used to enable or disable the clock
output, power down mode, and temperature measurement, respectively. In
addition, the functions ``ade7913_adc_freq()`` and ``ade7913_lfp_bw()``
are provided for setting the ADC frequency and selecting the digital
low-pass filter bandwidth.

Synchronization and Status
~~~~~~~~~~~~~~~~~~~~~~~~~~

Accurate synchronization and monitoring of device status are critical to
ensuring reliable operation of the ADE7913. The function
``ade7913_get_sync_cnt_val()`` retrieves the synchronization counter
value for precise timing, while the functions ``ade7913_crc_status()``,
``ade7913_ic_prot_status()`` and ``ade7913_adc_na_status()`` return
status information regarding the device’s configuration registers,
protection status, and ADC access respectively.

Device Control
~~~~~~~~~~~~~~

To maintain secure and stable operation, the ADE7913 driver includes
functions for device control and access management. The functions
``ade7913_wr_lock()`` and ``ade7913_wr_unlock()`` are used to lock and
unlock the configuration registers to prevent unauthorized
modifications, and the function ``ade7913_sw_reset()`` enables a
software reset of the device for error recovery or reinitialization.

Driver Initialization Example
-----------------------------

.. code-block:: c

   #include <stdlib.h>
   #include <errno.h>
   #include <stdbool.h>
   #include <stdint.h>
   #include <string.h>
   #include "ade7913.h"
   #include "no_os_delay.h"
   #include "no_os_print_log.h"
   #include "no_os_spi.h"
   #include "no_os_gpio.h"
   #include "no_os_irq.h"
   #include "platform.h"

    /* Define constants for better readability */
    #define GPIO_ADE7913_RESET_PIN  5  // Replace with the actual GPIO pin number
    #define ADE7913_MCLK_FREQ       32768000

    /* Define extra parameters for SPI and GPIO */
    struct no_os_spi_extra ade7913_spi_extra_ip = {
        // ...initialize SPI-specific extra parameters...
    };

    struct no_os_gpio_extra ade7913_gpio_extra_ip = {
        // ...initialize GPIO-specific extra parameters...
    };

    int32_t ret;
    struct ade7913_dev *dev;
    struct ade7913_init_param init_param = {
        .spi_init = {
            .device_id     = 1,
            .max_speed_hz  = 1000000,
            .chip_select   = 0,
            .mode          = NO_OS_SPI_MODE_3,
            .platform_ops  = &max_spi_ops,
            .extra         = &ade7913_spi_extra_ip
        },
        .gpio_reset = {
            .number        = GPIO_ADE7913_RESET_PIN,
            .platform_ops  = &max_gpio_ops,
            .extra         = &ade7913_gpio_extra_ip
        },
        .gpio_reset_value   = NO_OS_GPIO_HIGH,
        .mclk               = ADE7913_MCLK_FREQ,
        .datalines          = 8,
        .no_devs            = 1,
        .burst_mode         = false
    };

    no_os_pr_info("Starting ADE7913 initialization\n");
    ret = ade7913_init(&dev, &init_param);
    if (ret) {
        no_os_pr_err("ADE7913 initialization failed with error code: %d\n", ret);
        goto error;
    }
    no_os_pr_info("ADE7913 initialization successful\n");

    ret = ade7913_sw_reset(dev);
    if (ret) {
        no_os_pr_err("ADE7913 software reset failed with error code: %d\n", ret);
        goto error_remove;
    }
    no_os_pr_info("ADE7913 software reset successful\n");

    /* Interrupt initialization is not implemented. Remove or replace this section if needed. */
    no_os_pr_info("Skipping interrupt initialization as 'inter_init' is not defined\n");

    goto end;

    error_remove:
    ade7913_remove(dev);
    error:
    no_os_pr_err("Exiting initialization due to error\n");
    end:
    no_os_pr_info("Initialization process complete\n");
