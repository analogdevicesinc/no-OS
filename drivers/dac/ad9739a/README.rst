AD9739A no-OS Driver
====================

.. no-os-doxygen::

Supported Devices
------------------

* `AD9739A <https://www.analog.com/AD9739A>`_

Overview
----------

The AD9739A is a 14-bit RF digital-to-analog converter (DAC), capable of
creating wideband signals from DC to 3 GHz. Featuring an update rate up
to 2.5 GSPS, this device is ideal for broadband communications, military
jamming, instrumentation, and radar systems. It features low-voltage
differential signaling (LVDS) for enhanced data and signal quality,
advanced clock management with delay lock loops (DLL) for optimized
timing, crucial for high-speed data transfer, and minimizes signal
distortion. Its dynamic performance is highlighted by Spurious-Free
Dynamic Range (SFDR) and Intermodulation Distortion (IMD). Integration
is streamlined by flexible initialization via SPI configurations for
reliable operation in complex systems.

Applications
-------------

* Broadband communications systems

  * DOCSIS CMTS systems

* Military jammers
* Instrumentation, automatic test equipment
* Radar, avionics

Operation Modes
----------------

The AD9739A supports the following operation modes:

* **Normal Baseband Mode**: Standard DAC output mode for baseband signal
  generation.
* **Mix Mode**: Enables the DAC to operate in mix-mode for direct RF
  synthesis applications.
* **Sleep Mode**: Reduces power consumption by placing the DAC in a
  low-power state.

Device Configuration
--------------------

The ``ad9739a_setup`` function initializes the AD9739A device by first
allocating memory using ``no_os_malloc``, then establishing SPI
communication through ``no_os_spi_init``, and confirming the device ID via
``ad9739a_read``. It proceeds to configure various registers using
``ad9739a_write`` and performs a device reset with ``ad9739a_reset``. To ensure
the MU controller and LVDS receiver are properly locked and operational,
the function repeatedly writes to specific registers and checks their
status, incorporating ``delay_fdata_cycles`` for timing control.
Additionally, it sets the DAC's full-scale current using
``ad9739a_dac_fs_current``, displays a success message, and returns the
initialized device structure.

Initialization and Cleanup
~~~~~~~~~~~~~~~~~~~~~~~~~~

The initialization and cleanup functions prepare the device for
operation and ensure proper resource management. The ``ad9739a_setup()``
function initializes the AD9739A DAC by configuring its SPI interface,
verifying the device ID, setting up the MU controller, configuring the
LVDS receiver, and establishing the full-scale output current. After the
device has served its purpose, the ``ad9739a_remove()`` function is used
to deallocate resources by removing the SPI descriptor and freeing
allocated memory. Together, these functions ensure that the device is
correctly integrated into a system during initialization and safely
removed when no longer needed.

Register Communication Functions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For basic register access, the driver provides functions that facilitate
SPI-based communication with the AD9739A DAC registers. The
``ad9739a_write()`` function constructs and sends SPI commands needed to
update register values, while the ``ad9739a_read()`` function retrieves
register values using similarly structured SPI transactions. These two
functions abstract the low-level details of SPI communication to provide
a simple and effective API for register read and write operations.

Reset and Power Management
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Robust operation of the device requires efficient management of its
reset and power states. The ``ad9739a_reset()`` function performs a
software reset by writing commands that restore default settings,
thereby clearing the device's reset state. Complementing this, the
``ad9739a_power_down()`` function is used to manage the device's power
configuration by either applying a new power setting or retrieving the
current power status when invalid configurations are provided. These
functions allow high-level control over device resets and power
sequences in a streamlined, consistent manner.

Mode and Output Current Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For fine-tuning the DAC's operational behavior, the driver includes
functions that adjust both the operational mode and the full-scale
output current. The ``ad_serdes_clk()`` function selects among different
operational modes—such as normal baseband mode or mixed mode—by
configuring the device's mode register, and it also supports reading the
current mode setting. In addition, the ``ad9739a_dac_fs_current()``
function configures or retrieves the DAC's full-scale output current.
Together, these functions provide the flexibility needed to tailor the
device's performance to diverse application requirements.

Timing and Delay Control
~~~~~~~~~~~~~~~~~~~~~~~~

Precise timing control is critical for synchronizing DAC operations, and
the driver addresses this need with a dedicated function. The
``delay_fdata_cycles()`` function introduces a delay based on a
specified number of DAC cycles, effectively translating cycle counts
into controlled timing delays. This functionality is essential for
ensuring proper synchronization across the device's operations.

Driver Initialization Example
-----------------------------

.. code-block:: c

   #include <stdlib.h>
   #include <stdio.h>
   #include "ad9739a.h"
   #include "no_os_spi.h"
   #include "no_os_alloc.h"

   int main(void)
   {
       struct ad9739a_dev *device;
       int32_t status;

       struct ad9739a_init_param init_param = {
           .spi_init = {
               .device_id    = 0,
               .max_speed_hz = 10000000,
               .chip_select  = 0,
               .mode         = NO_OS_SPI_MODE_0,
               .platform_ops = &xil_spi_ops,
               .extra        = &xil_spi_param,
           },
           .common_mode_voltage_dacclk_p = 0xF,
           .common_mode_voltage_dacclk_n = 0xF,
           .full_scale_current           = 20.0,
       };

       status = ad9739a_setup(&device, init_param);
       if (status < 0) {
           printf("AD9739A initialization failed.\n");
           goto error;
       }

       printf("AD9739A initialization successful.\n");

   error:
       ad9739a_remove(device);

       return status;
   }
