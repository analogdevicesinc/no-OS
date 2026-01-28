AD9680 no-OS Driver
===================

Supported Devices
-----------------

- :adi:`AD9680`

Overview
--------

The AD9680 is a dual analog-to-digital converter (ADC) featuring JESD204B
(Subclass 1) coded serial digital outputs and operates with 1.65W total
power per channel at 1 GSPS (default settings). It offers exceptional
dynamic performance with SFDR at 1 GSPS = 85 dBFS at 340 MHz, 80 dBFS at
1 GHz, and SNR at 1 GSPS = 65.3 dBFS at 340 MHz (AIN = −1.0 dBFS), 
60.5 dBFS at 1 GHz (AIN = −1.0 dBFS). Additionally, it delivers 
ENOB = 10.8 bits at 10 MHz, DNL = ±0.5 LSB, INL = ±2.5 LSB, and 
Noise density = −154 dBFS/Hz at 1 GSPS. 

Designed for sampling wide bandwidth analog signals of up to 2 GHz, 
the AD9680 is a dual, 14-bit, 1.25GSPS/1GSPS/820MSPS/500MSPS ADC 
that features an on-chip buffer and sample-and-hold circuit 
engineered for low power, small size, and ease of use. The device is 
optimized for wide input bandwidth, high sampling rate, excellent linearity, 
and low power in a small package, supporting multiple power supply 
configurations with 1.25V, 2.5V, and 3.3V dc supply operation and 
offering a flexible input range of 1.46Vp-p to 1.94Vp-p.

Applications
-------------

- Communications
- Diversity multiband, multimode digital receivers
     - TD-SCDMA, W-CDMA, GSM, LTE
- General-purpose software radios
- Ultrawideband satellite receivers
- Instrumentation
- Radars
- Signals intelligence (SIGINT)
- DOCSIS 3.0 CMTS upstream receive paths
- HFC digital reverse path receivers

Operation Modes
----------------

+-----------------+-----------------+--------------------------+-----------------+
| Mode Name       | Description     | Configuration Bits       | Typical Use     |
+-----------------+-----------------+--------------------------+-----------------+
| Test Off        | Disables any    | AD9680_TEST_OFF          | Standard ADC    |
|                 | test mode,      |                          | operation       |
|                 | allowing normal |                          | without         |
|                 | ADC operation.  |                          | self-test mode. |
+-----------------+-----------------+--------------------------+-----------------+
| PN9 Test        | Enables the PN9 | AD9680_TEST_PN9          | Verifying data  |
|                 | pseudo-random   |                          | integrity       |
|                 | test sequence   |                          | through a PN9   |
|                 | for signal      |                          | pattern.        |
|                 | validation.     |                          |                 |
+-----------------+-----------------+--------------------------+-----------------+
| PN23 Test       | Activates the   | AD9680_TEST_PN23         | Testing and     |
|                 | PN23            |                          | debugging with  |
|                 | pseudo-random   |                          | a PN23 pattern. |
|                 | test sequence   |                          |                 |
|                 | for data        |                          |                 |
|                 | pattern checks. |                          |                 |
+-----------------+-----------------+--------------------------+-----------------+
| Ramp Test       | Initiates a     | AD9680_TEST_RAMP         | Calibration and |
|                 | ramp test mode, |                          | performance     |
|                 | outputting a    |                          | evaluation      |
|                 | linear          |                          | tests.          |
|                 | incrementing    |                          |                 |
|                 | sequence.       |                          |                 |
+-----------------+-----------------+--------------------------+-----------------+
| SYSREF None     | Configures the  | AD9680_SYSREF_NONE       | Applications    |
|                 | device with no  |                          | where SYSREF    |
|                 | SYSREF support. |                          | synchronization |
|                 |                 |                          | is not          |
|                 |                 |                          | required.       |
+-----------------+-----------------+--------------------------+-----------------+
| SYSREF One-shot | Sets SYSREF to  | AD9680_SYSREF_ONESHOT    | Systems needing |
|                 | one-shot mode,  |                          | a singular      |
|                 | triggering a    |                          | SYSREF          |
|                 | single          |                          | synchronization |
|                 | synchronization |                          | pulse at        |
|                 | event.          |                          | startup.        |
+-----------------+-----------------+--------------------------+-----------------+
| SYSREF          | Puts the device | AD9680_SYSREF_CONT       | Environments    |
| Continuous      | in continuous   |                          | requiring       |
|                 | SYSREF mode for |                          | continuous      |
|                 | ongoing         |                          | SYSREF sync for |
|                 | synchronization.|                          | stability.      |
+-----------------+-----------------+--------------------------+-----------------+
| SYSREF Monitor  | Configures      | AD9680_SYSREF_MON        | Diagnostic      |
|                 | SYSREF in       |                          | scenarios where |
|                 | monitor mode to |                          | SYSREF events   |
|                 | observe SYSREF  |                          | need to be      |
|                 | events.         |                          | tracked.        |
+-----------------+-----------------+--------------------------+-----------------+

Device Configuration
---------------------

Initialization Functions
~~~~~~~~~~~~~~~~~~~~~~~~

The initialization functions form the foundation for configuring the
AD9680 device. The ``ad9680_setup()`` function initializes SPI
communication, verifies the chip identity, configures JESD204B settings,
and confirms PLL lock status, while the ``ad9680_setup_jesd_fsm()``
function sets up and registers the JESD204B interface using the proper
device parameters.

SPI Communication Functions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The SPI communication functions provide low-level access for register
transactions over the Serial Peripheral Interface. The
``ad9680_spi_read()`` function retrieves the current value from a
specified register, and the ``ad9680_spi_write()`` function writes new
data to a designated register.

JESD204 Link Configuration Functions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The JESD204 link configuration functions are responsible for
establishing and managing the high-speed serial interface required for
data conversion. The ``ad9680_jesd204_link_init()`` function initializes
the JESD204 link by setting critical link parameters,
``ad9680_jesd_enable_link()`` modifies registers to enable or disable
the link, ``ad9680_jesd204_clks_enable()`` ensures that the link clocks
are active, and ``ad9680_jesd204_link_enable()`` finalizes the
activation sequence necessary for full link operation.

Test Mode Configuration Function
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The test mode configuration function simplifies the process of
activating diagnostic and validation routines. The ``ad9680_test()``
function configures the ADC’s internal test mode and output format
settings based on the supplied test mode parameter.

Device Removal Function
~~~~~~~~~~~~~~~~~~~~~~~~

The device removal function is dedicated to proper deallocation of
system resources when the AD9680 is no longer in use. The
``ad9680_remove()`` function handles the removal of the SPI descriptor
and frees the internal device structure to ensure that memory leaks and
communication issues do not occur.

Driver Initialization Example
-----------------------------

.. code-block:: C

   #include <stdint.h>
   #include <stdlib.h>
   #include <stdio.h>
   #include "no_os_delay.h"
   #include "no_os_spi.h"
   #include "jesd204.h"
   #include "no_os_util.h"
   #include "ad9680.h"
   #include "no_os_error.h"
   #include "no_os_print_log.h"
   #include "no_os_alloc.h"

   struct ad9680_init_param init_param = {
       .spi_init = {
           .device_id       = 1,
           .max_speed_hz    = 1000000,
           .chip_select     = 1,
           .mode            = NO_OS_SPI_MODE_0,
           .platform_ops    = &xil_spi_ops,
           .extra           = &xil_spi_initial,
       },
       .lane_rate_kbps      = 10000,
       .jesd204_link = {
           .link_id         = 0,
           .sample_rate     = 1000000000,
           .sample_rate_div = 1,
           .jesd_encoder    = JESD204_ENCODER_8B10B,
           .sysref = {
               .mode        = JESD204_SYSREF_CONTINUOUS,
           },
       },
       .sampling_frequency_hz = 1000000000,
       .dcm                  = 1,
       .sysref_mode          = AD9680_SYSREF_CONT
   };

   struct ad9680_dev *device;
   int32_t status;

   status = ad9680_setup(&device, &init_param);
   if (status < 0) {
       printf("AD9680 initialization failed.\n");
       goto error;
   }

   printf("AD9680 initialization successful.\n");

   error:
       ad9680_remove(device);
