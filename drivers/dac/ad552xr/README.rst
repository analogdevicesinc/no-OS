AD552XR no-OS Driver
====================

.. no-os-doxygen::

Supported Devices
-----------------

`AD5529R <https://www.analog.com/en/products/ad5529r.html>`_

Overview
--------

The AD552XR is a family of 40V, Bipolar High Voltage Precision DACs. It supports
hardware and software LDAC triggering with four toggle pins, function generator
modes (toggle, dither, sawtooth, triangular), and flexible output ranges (unipolar
and bipolar). These features makes the devices versatile for instrumentation,
process control and industrial automation applications.
The AD5529R of this family features 16-bit, 16-channel DAC with an integrated
precision reference.

Device Configuration
--------------------

Device Setup and Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ``ad552xr_init`` function initializes and configures the AD5529R, involving
memory allocation, SPI initialization, GPIO setup for RESET, CLEAR, ALARM,
multi-drop address, and LDAC/TGP pins. The initialization process includes
hardware or software reset, product ID verification, and scratch pad register
testing.

Resource Management
~~~~~~~~~~~~~~~~~~~

The ``ad552xr_remove`` function deallocates resources associated with the
device, removing GPIO descriptors, SPI descriptor, and freeing allocated memory
to ensure proper resource release and prevent memory leaks.

SPI Communication
~~~~~~~~~~~~~~~~~

The ``ad552xr_spi_reg_read`` and ``ad552xr_spi_reg_write`` functions handle SPI
interactions for the device. These functions support address ascension mode and
multi-drop device addressing via GPIO lines. The ``ad552xr_spi_write_mask``
function provides masked register writes for modifying specific bit fields.

Channel Configuration
~~~~~~~~~~~~~~~~~~~~~

Individual channels can be configured for different output ranges using
``ad552xr_set_ch_output_range``, supporting unipolar and bipolar modes. Channel
outputs are enabled or disabled using ``ad552xr_channel_output_en``.

DAC Value Setting
~~~~~~~~~~~~~~~~~

DAC values can be set using ``ad552xr_set_dac_a_value`` for DAC input register A
or ``ad552xr_set_dac_b_value`` for DAC input register B (used in function generator
modes). Updates are applied to the output via LDAC triggers.

LDAC Configuration
~~~~~~~~~~~~~~~~~~

Each channel can be independently configured for hardware or software LDAC mode
using ``ad552xr_set_hw_sw_ldac``, synchronous or asynchronous operation using
``ad552xr_set_sync_async_ldac``, edge trigger selection using
``ad552xr_set_hw_edge_trigger``, and toggle pin assignment using
``ad552xr_set_hw_ldac_toggle_pin``. The driver supports up to 4 hardware
LDAC/TGP pins. Hardware triggers are issued via ``ad552xr_hw_ldac_trigger``
and software triggers via ``ad552xr_sw_ldac_trigger``.

Function Generator
~~~~~~~~~~~~~~~~~~

The built-in function generator supports toggle, dither, sawtooth, and
triangular waveforms. It is enabled per channel using ``ad552xr_func_en`` and
configured with ``ad552xr_func_mode_select``. Dither-specific parameters are
set using ``ad552xr_set_dither_period`` and ``ad552xr_set_dither_phase``. Ramp
step size is configured using ``ad552xr_set_ramp_step_size``.

Hardware Features
~~~~~~~~~~~~~~~~~

Reference voltage can be configured as internal (4.096V) or external using
``ad552xr_set_reference``. The MUX output selection allows monitoring of channel
voltages, currents, temperature sensors, and other internal signals using
``ad552xr_set_mux_out_select``.

Driver Initialization Example
-----------------------------

.. code-block:: C

   #include <stdint.h>
   #include "ad552xr.h"

   // Initialization parameters for the AD5529R device
   struct ad552xr_init_param init_param = {
       .type = ID_AD5529R,
       .dev_addr = 0,
       .spi_init_prm = &spi_init_params,
       .gpio_alarmb = &gpio_alarm_n_init_params,
       .gpio_clearb = &gpio_clear_n_init_params,
       .gpio_ldac_tgp = {
           &gpio_ldac_tgp0_init_params, NULL, NULL, NULL,
       },
       .gpio_md_addr = {&gpio_md_addr0_init_params, &gpio_md_addr1_init_params},
       .gpio_resetb = &gpio_reset_n_init_params,
       .vref = AD552XR_INTERNAL_VREF,
   };

   // Device structure for the AD5529R
   struct ad552xr_dev *ad552xr_device;

   // Set up the AD5529R device
   int32_t setup_ad552xr(void)
   {
       int32_t ret;

       // Initialize the device
       ret = ad552xr_init(&ad552xr_device, &init_param);
       if (ret)
           return ret;

       // Set internal voltage reference
       ret = ad552xr_set_reference(ad552xr_device, AD552XR_INTERNAL_VREF);
       if (ret)
           return ret;

       // Configure channel 1 output range to unipolar 10V
       ret = ad552xr_set_ch_output_range(ad552xr_device, 1,
                       AD552XR_UNIPOLAR_10V);
       if (ret)
           return ret;

       return 0;
   }

   // Example of hardware LDAC triggered update
   int32_t hw_ldac_example(void)
   {
       int32_t ret;

       // Configure channel 1 for synchronous LDAC
       ret = ad552xr_set_sync_async_ldac(ad552xr_device, 1, true);
       if (ret)
           return ret;

       // Configure channel 1 for hardware LDAC on TGP0
       ret = ad552xr_set_hw_sw_ldac(ad552xr_device, 1, AD552XR_HW_LDAC);
       if (ret)
           return ret;

       ret = ad552xr_set_hw_ldac_toggle_pin(ad552xr_device, 1, AD552XR_LDAC_TGP_0);
       if (ret)
           return ret;

       ret = ad552xr_set_hw_edge_trigger(ad552xr_device, 1, RISING_EDGE_TRIG);
       if (ret)
           return ret;

       // Write DAC value to input register A
       ret = ad552xr_set_dac_a_value(ad552xr_device, 1, 0xFFFF);
       if (ret)
           return ret;

       // Enable channel 1 output
       ret = ad552xr_channel_output_en(ad552xr_device, 1, true);
       if (ret)
           return ret;

       // Trigger hardware LDAC with 100us pulse
       ret = ad552xr_hw_ldac_trigger(ad552xr_device, AD552XR_LDAC_TGP_0, 100);
       if (ret)
           return ret;

       return 0;
   }

   // Main function for AD5529R demonstration
   int main(void)
   {
       int32_t status;

       status = setup_ad552xr();
       if (status)
           return status;

       status = hw_ldac_example();
       if (status)
            return status;

       // Clean up resources
       ad552xr_remove(ad552xr_device);

       return 0;
   }
