ADE9000 no-OS Driver
====================

.. no-os-doxygen::

Supported Devices
-----------------

* `ADE9000 <https://www.analog.com/ADE9000>`_

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

* Energy and power monitoring
* Power quality monitoring
* Protective devices
* Machine health
* Smart power distribution units
* Polyphase energy meters

Operation Modes
----------------

The ADE9000 supports the following operation modes:

* **Energy Accumulation with Reset**: Accumulates energy measurements
  and resets the accumulator after each read.
* **Half Line Cycle Accumulation**: Accumulates energy over half line
  cycles for faster updates.
* **Sample-Based Accumulation**: Accumulates energy over a configurable
  number of samples.
* **Waveform Buffer Modes**: Supports fixed data rate waveform capture
  with stop-when-full or continuous fill trigger-based modes.
* **4-Wire Wye / 3-Wire Delta**: Configurable hardware topology for
  multiphase measurement setups.

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

.. code-block:: c

   #include <stdio.h>
   #include "ade9000.h"
   #include "no_os_spi.h"
   #include "no_os_delay.h"
   #include "no_os_alloc.h"
   #include "platform.h"

   int main()
   {
       uint8_t ret;

       struct no_os_spi_init_param ade9000_spi_ip = {
           .device_id      = 1,
           .max_speed_hz   = 1000000,
           .chip_select    = 1,
           .mode           = NO_OS_SPI_MODE_0,
           .platform_ops   = &max_spi_ops,
           .extra          = &ade9000_spi_extra_ip
       };

       struct ade9000_init_param ade9000_ip = {
           .spi_init = &ade9000_spi_ip,
           .temp_en  = ENABLE,
       };

       /* device structure */
       struct ade9000_dev *ade9000_dev = NULL;

       printf("\n");
       printf("\n");
       printf("ADE9000 SPI example \n");

       /* Initialize the device with the values stored
       in the initialization structure */
       ret = ade9000_init(&ade9000_dev, ade9000_ip);
       if (ret)
           goto error;
       /* setup the ade9000 device */
       ret = ade9000_setup(ade9000_dev);
       if (ret)
           goto free_dev;

       no_os_mdelay(RESET_TIME);

       while (1) {
           /* time delay between readings */
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
       ade9000_remove(ade9000_dev);
   error:
       printf("ERROR\n");
       return ret;

   }
