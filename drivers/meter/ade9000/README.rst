ADE9000 no-OS Driver
====================

Supported Devices
-----------------

- :adi:`ADE9000`

Overview
--------

The ADE9000 is a precise energy and power quality monitoring device
designed for multiphase applications. It combines high-performance
analog features with a digital signal processing core, ensuring accuracy
with a drift of less than ±25 ppm/°C. The device offers measurements 
for root mean square (rms), active, reactive, and apparent powers and
energies, alongside advanced functions like dip and swell monitoring,
frequency, phase angle, voltage and current total harmonic distortion
(THD), and power factor assessment. The flexible waveform buffer allows
for efficient storage and resampling, enabling external processors to
perform fast Fourier transform computations for harmonics, simplifying
power monitoring implementations.

Applications
------------

- Energy and power monitoring
- Power quality monitoring
- Protective devices
- Machine health
- Smart power distribution units
- Polyphase energy meters

Operation Modes
----------------

+-------------------------+-----------------+-----------------+-----------------+
| Mode Name               | Description     | Configuration   | Typical Use     |
|                         |                 | Bits            | Case            |
+-------------------------+-----------------+-----------------+-----------------+
| ADE9000_DISRPLPF        | Disables the    | Bit 13          | Use this to     |
|                         | ripple removal  |                 | prevent the     |
|                         | low-pass        |                 | removal of      |
|                         | filter.         |                 | ripple from the |
|                         |                 |                 | signal.         |
+-------------------------+-----------------+-----------------+-----------------+
| ADE9000_DISAPLPF        | Disables the    | Bit 12          | Useful when     |
|                         | active power    |                 | active power    |
|                         | low-pass        |                 | filtration is   |
|                         | filter.         |                 | not required.   |
+-------------------------+-----------------+-----------------+-----------------+
| ADE9000_ININTEN         | Enables         | Bit 11          | Used to trigger |
|                         | interrupt.      |                 | an interrupt    |
|                         |                 |                 | when certain    |
|                         |                 |                 | conditions are  |
|                         |                 |                 | met.            |
+-------------------------+-----------------+-----------------+-----------------+
| ADE9000_VNOMC_EN        | Enables the     | Bit 10          | Activated to    |
|                         | voltage nominal |                 | monitor voltage |
|                         | C phase.        |                 | for the C       |
|                         |                 |                 | phase.          |
+-------------------------+-----------------+-----------------+-----------------+
| ADE9000_VNOMB_EN        | Enables the     | Bit 9           | Activated to    |
|                         | voltage nominal |                 | monitor voltage |
|                         | B phase.        |                 | for the B       |
|                         |                 |                 | phase.          |
+-------------------------+-----------------+-----------------+-----------------+
| ADE9000_VNOMA_EN        | Enables the     | Bit 8           | Activated to    |
|                         | voltage nominal |                 | monitor voltage |
|                         | A phase.        |                 | for the A       |
|                         |                 |                 | phase.          |
+-------------------------+-----------------+-----------------+-----------------+
| ADE9000_RMS_SRC_SEL     | Selects the RMS | Bit 7           | Used to         |
|                         | calculation     |                 | determine the   |
|                         | source.         |                 | source for RMS  |
|                         |                 |                 | calculations.   |
+-------------------------+-----------------+-----------------+-----------------+
| ADE9000_ZX_SRC_SEL      | Selects the     | Bit 6           | Helpful in      |
|                         | zero crossing   |                 | applications    |
|                         | source.         |                 | requiring zero  |
|                         |                 |                 | crossing        |
|                         |                 |                 | detection.      |
+-------------------------+-----------------+-----------------+-----------------+
| ADE9000_INTEN           | General         | Bit 5           | Enables         |
|                         | interrupt       |                 | interrupts for  |
|                         | enable.         |                 | general use.    |
+-------------------------+-----------------+-----------------+-----------------+
| ADE9000_MTEN            | Enables         | Bit 4           | Used in         |
|                         | multi-tariff    |                 | scenarios where |
|                         | feature.        |                 | multi-tariff    |
|                         |                 |                 | metering is     |
|                         |                 |                 | necessary.      |
+-------------------------+-----------------+-----------------+-----------------+
| ADE9000_HPFDIS          | Disables the    | Bit 3           | Applied when    |
|                         | high-pass       |                 | high-pass       |
|                         | filter.         |                 | filtering is    |
|                         |                 |                 | not needed.     |
+-------------------------+-----------------+-----------------+-----------------+
| ADE9000_ISUM_CFG        | Configuration   | Bits 0-1        | Used to set up  |
|                         | for current     |                 | current         |
|                         | summation RMS   |                 | summation       |
|                         | calculation.    |                 | calculations.   |
+-------------------------+-----------------+-----------------+-----------------+

Device Configuration
---------------------

Device Initialization and Setup
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ADE9000 initialization involves functions such as ``ade9000_init`` 
and ``ade9000_setup``. The ``ade9000_init`` function initializes the 
device, sets up communication interfaces, verifies the device ID, and 
enables features like the temperature sensor. ``ade9000_setup`` configures 
operational settings, such as gain levels and DSP enablement, 
preparing the device for energy measurement.

Register Operations
~~~~~~~~~~~~~~~~~~~

This category includes functions for direct register manipulation, such
as ``ade9000_read``, ``ade9000_write``, and ``ade9000_update_bits``.
These functions allow reading from registers, writing data to them, and
modifying specific bits, providing a comprehensive interface for
low-level device interaction.

Measurement and Data Retrieval
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Functions like ``ade9000_read_temp`` and ``ade9000_read_data_ph``
facilitate access to measurement data. ``ade9000_read_temp`` retrieves
temperature data, while ``ade9000_read_data_ph`` obtains energy and
power metrics from a specified phase, simplifying data retrieval for
energy monitoring.

User Configuration Management
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ``ade9000_set_egy_model`` function allows for user-specific energy
monitoring configurations. It enables setting the energy accumulation
model based on user requirements, adapting the device for various power
monitoring scenarios.

Cleanup and Resource Management
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ``ade9000_remove`` function ensures proper resource deallocation,
releasing communication descriptors and preventing memory leaks when the
device is no longer in use.

Driver Initialization Example
-----------------------------

.. code-block:: C

   #include <stdio.h>
   #include "no_os_uart.h"
   #include "no_os_pwm.h"
   #include "no_os_delay.h"
   #include "no_os_gpio.h"
   #include "no_os_spi.h"
   #include "no_os_print_log.h"
   #include "no_os_units.h"
   #include "no_os_util.h"
   #include "no_os_error.h"
   #include "maxim_uart.h"
   #include "maxim_gpio.h"
   #include "maxim_uart_stdio.h"
   #include "maxim_pwm.h"
   #include "maxim_spi.h"
   #include "ade9000.h"
   #include "platform.h"

   int main(void)
   {
       uint8_t ret;

       // parameters initialization structure
       struct ade9000_init_param ade9000_ip;
       // device structure
       struct ade9000_dev *ade9000_dev;

       // uart descriptor
       struct no_os_uart_desc *uart_desc;
       // gpio descriptor
       struct no_os_gpio_desc *gpio_desc;

       ret = no_os_uart_init(&uart_desc, &uart_ip);
       if (ret)
           goto error;

       ret = no_os_gpio_get_optional(&gpio_desc,
                                     &gpio_led1_ip);
       if (ret)
           goto remove_uart;

       if (gpio_desc)
           ret = no_os_gpio_direction_output(gpio_desc,
                                             NO_OS_GPIO_LOW);
       if (ret)
           goto remove_led;

       /* Initialize SPI */
       ade9000_ip.spi_init = &ade9000_spi_ip;
       /* Enable the temperature sensor */
       ade9000_ip.temp_en = ENABLE;

       no_os_uart_stdio(uart_desc);

       pr_info("\n");
       pr_info("\n");
       pr_info("ADE9000 SPI example \n");

       /* Init ade9000 struct */
       ade9000_dev = (struct ade9000_dev *)no_os_calloc(1, sizeof(*ade9000_dev));
       if (!ade9000_dev)
           return -ENOMEM;

       /* Initialize the device with the values stored
       in the initialization structure */
       ret = ade9000_init(&ade9000_dev, ade9000_ip);
       if (ret)
           goto free_dev;
       /* setup the ade9000 device */
       ret = ade9000_setup(ade9000_dev);
       if (ret)
           goto free_dev;

       no_os_mdelay(RESET_TIME);

       while (1) {
           // time delay between readings
           no_os_mdelay(READ_INTERVAL);
           /* read and print the ade9000 rms measured values for PHASE A */
           ret = ade9000_read_data_ph(ade9000_dev, ADE9000_PHASE_A);
           if (ret)
               goto free_dev;

           pr_info("Vrms: %d mV\n", ade9000_dev->vrms_val);
           pr_info("Irms: %d mA\n", ade9000_dev->irms_val);
           /* read the temperature value */
           ret = ade9000_read_temp(ade9000_dev);
           if (ret)
               goto free_dev;

           pr_info("Temperature: %d deg C \n", ade9000_dev->temp_deg);
           /* toggle the LED on the MCU board */
           ret = interface_toggle_led(gpio_desc);
           if (ret)
               goto free_dev;
       }

   free_dev:
       no_os_free(ade9000_dev);
   remove_led:
       no_os_gpio_remove(gpio_desc);
   remove_uart:
       no_os_uart_remove(uart_desc);

   error:
       pr_err("ERROR\n");
       return ret;

   }
