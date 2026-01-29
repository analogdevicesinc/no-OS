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
   #include "ade9000.h"
   #include "no_os_spi.h"
   #include "no_os_delay.h"
   #include "no_os_alloc.h"
   #include "platform.h"

   int main()
   {
       uint8_t ret;

       struct ade9000_init_param ade9000_init_param = {
           .spi_init = {
               .device_id      = 1,
               .max_speed_hz   = 1000000,
               .chip_select    = 1,
               .mode           = NO_OS_SPI_MODE_0,
               .platform_ops   = &max_spi_ops,
               .extra          = &ade9000_spi_extra_ip
           },
       };

       // device structure
       struct ade9000_dev *ade9000_dev = NULL;

       printf("\n");
       printf("\n");
       printf("ADE9000 SPI example \n");

       /* Initialize the device with the values stored
       in the initialization structure */
       ret = ade9000_init(&ade9000_dev, &ade9000_init_param);
       if (ret)
           goto error;
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

           printf("Vrms: %d mV\n", ade9000_dev->vrms_val);
           printf("Irms: %d mA\n", ade9000_dev->irms_val);
           /* read the temperature value */
           ret = ade9000_read_temp(ade9000_dev);
           if (ret)
               goto free_dev;

           printf("Temperature: %d deg C \n", ade9000_dev->temp_deg);
       }

   free_dev:
       no_os_free(ade9000_dev);
   error:
       printf("ERROR\n");
       return ret;

   }
