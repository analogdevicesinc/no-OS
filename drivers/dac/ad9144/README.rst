AD9144 no-OS Driver
===================

Supported Devices
-----------------

- :adi:`AD9144`

Overview
--------

The AD9144 is a high-speed, quad 16-bit digital-to-analog converter
(DAC) with a sample rate up to 2.8 GSPS. It’s designed to generate
multicarrier signals up to the Nyquist frequency, interfacing seamlessly
with Analog Devices’ ADRF6720 analog quadrature modulator. The DAC
supports 3-wire and 4-wire serial port interfaces, allowing for
programming and parameter readback. Its output current is programmable
between 13.9 mA and 27.0 mA. The device is housed in an 88-lead LFCSP
package, supporting high-quality synthesis of wideband signals suitable
for modern wideband and multiband wireless applications. Key features
include JESD204B Subclass 1 for synchronized multichip use, a reduced
pin count via an eight-lane SERDES JESD204B interface, and a
programmable transmit enable function to optimize power consumption and
wake-up time.

Applications
-------------

- Wireless communications
  - 3G/4G W-CDMA base stations
  - Wideband repeaters
  - Software defined radios

- Wideband communications
  - Point-to-point
  - Local multipoint distribution service (LMDS) and multichannel
    multipoint distribution service (MMDS)
  - Transmit diversity, multiple input/multiple output (MIMO)
  - Instrumentation
  - Automated test equipment

Operation Modes
---------------

JESD204B Link Modes
~~~~~~~~~~~~~~~~~~~

+-----------------+-----------------+-----------------+-----------------+
| Mode            | Description     | Configuration   | JESD204B Link   |
|                 |                 | Bits            | Type            |
+-----------------+-----------------+-----------------+-----------------+
| 0               | 4 converters, 8 | 0x453[4:0] =    | Single-Link     |
|                 | lanes, 1        | 0x7             |                 |
|                 | sample/frame, 1 |                 |                 |
|                 | octet/frame     |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| 1               | 4 converters, 8 | 0x454[7:0] =    | Single-Link     |
|                 | lanes, 2        | 0x00            |                 |
|                 | samples/frame,  |                 |                 |
|                 | 2 octets/frame  |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| 2               | 4 converters, 4 | 0x455[4:0] =    | Single-Link     |
|                 | lanes, 1        | 0x1F            |                 |
|                 | sample/frame, 2 |                 |                 |
|                 | octets/frame    |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| 3               | 4 converters, 2 | 0x456[7:0] =    | Single-Link     |
|                 | lanes, 1        | 0x03            |                 |
|                 | sample/frame, 4 |                 |                 |
|                 | octets/frame    |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| 4               | 2 converters, 4 | 0x457[4:0] =    | Single          |
|                 | lanes, 1        | 0xF             | /Dual-Link      |
|                 | sample/frame, 1 |                 |                 |
|                 | octet/frame     |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| 5               | 2 converters, 4 | 0x458[4:0] =    | Single          |
|                 | lanes, 1        | 0xF             | /Dual-Link      |
|                 | samples/frame,  |                 |                 |
|                 | 2 octets/frame  |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| 6               | 2 converters, 2 | 0x459[7:5] =    | Single          |
|                 | lanes, 1        | 0xF             | /Dual-Link      |
|                 | sample/frame, 2 |                 |                 |
|                 | octets/frame    |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| 7               | 1 converter, 1  | 0x459[4:0] =    | Single          |
|                 | lanes, 1        | 0xF             | /Dual-Link      |
|                 | sample/frame, 4 |                 |                 |
|                 | octets/frame    |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| 9               | 1 converter, 2  | 0x458[4:0] =    | Single          |
|                 | lanes, 1        | 0xF             | /Dual-Link      |
|                 | sample/frame, 1 |                 |                 |
|                 | octet/frame     |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| 10              | 1 converter, 1  | 0x458[4:0] =    | Single          |
|                 | lanes, 1        | 0xF             | /Dual-Link      |
|                 | sample/frame, 2 |                 |                 |
|                 | octets/frame    |                 |                 |
+-----------------+-----------------+-----------------+-----------------+

Synchronization Modes
~~~~~~~~~~~~~~~~~~~~~

+-----------------+-----------------+-----------------+-----------------+
| MODE NAME       | Description     | Configuration   | Typical Use     |
|                 |                 | Bits            | case            |
+-----------------+-----------------+-----------------+-----------------+
| One-Shot Sync   | Performs a      | 0x01            | Useful when a   |
| Mode            | phase check on  |                 | single          |
|                 | the first       |                 | synchronization |
|                 | alignment edge  |                 | event is        |
|                 | after the sync  |                 | needed.         |
|                 | machine is      |                 |                 |
|                 | armed. If the   |                 |                 |
|                 | phase error     |                 |                 |
|                 | exceeds the     |                 |                 |
|                 | window          |                 |                 |
|                 | tolerance, a    |                 |                 |
|                 | clock           |                 |                 |
|                 | adjustment is   |                 |                 |
|                 | made.           |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| Continuous Sync | Allows a phase  | 0x02            | Suitable for    |
| Mode            | check on every  |                 | applications    |
|                 | alignment edge, |                 | requiring       |
|                 | enabling        |                 | continuous      |
|                 | ongoing         |                 | s               |
|                 | s               |                 | ynchronization. |
|                 | ynchronization. |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| One-Shot Then   | Begins with a   | 0x09            | Ideal for       |
| Monitor Mode    | one-shot sync,  |                 | systems where   |
|                 | then shifts to  |                 | initial         |
|                 | monitoring for  |                 | alignment is    |
|                 | phase error     |                 | critical, but   |
|                 | without         |                 | ongoing         |
|                 | automatic       |                 | monitoring is   |
|                 | corrections.    |                 | preferred.      |
+-----------------+-----------------+-----------------+-----------------+

Device Configuration
--------------------

Initialization and Setup
~~~~~~~~~~~~~~~~~~~~~~~~

Functions ``ad9144_setup``, and ``ad9144_setup_jesd_fsm`` prepare the
AD9144 for operation by initializing it and ensuring compatibility
across various systems for signal conversion tasks.

SPI Communication
~~~~~~~~~~~~~~~~~

The AD9144 utilizes SPI communication for register interface, enabling
configuration and operational status queries. Functions include
``ad9144_spi_read`` for reading register values, ``ad9144_spi_write``
for writing configurations, ``ad9144_spi_check_status`` for verifying
SPI communication, and ``ad9144_spi_write_seq`` to manage sequences of
register writes efficiently.

JESD204 Link Management
~~~~~~~~~~~~~~~~~~~~~~~

This category involves setup and maintenance of the JESD204B link,
crucial for transmitting data. It includes functions like
``ad9144_setup_jesd204_link`` and ``ad9144_jesd204_link_setup`` for
initialization and setup, and ``ad9144_jesd204_link_init``,
``ad9144_jesd204_link_enable``, and ``ad9144_setup_link`` for enabling
and configuring link parameters. ``ad9144_link_status_get`` and
``ad9144_jesd204_link_running`` provide link status checks.

NCO Configuration and Sample Rate Adjustment
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

NCO settings, handled by ``ad9144_set_nco`` and ``ad9144_set_nco_freq``,
allow frequency and phase control critical for tuning output signals.
Sample rate adjustments are achieved through functions like
``ad9144_setup_samplerate`` and ``ad9144_get_sample_rate``, aligning DAC
operation with system clock requirements using the PLL setup functions
``ad9144_pll_setup`` and ``ad9144_setup_pll``.

Testing and Calibration
~~~~~~~~~~~~~~~~~~~~~~~

Functions including ``ad9144_dac_calibrate``,
``ad9144_short_pattern_test``, and ``ad9144_datapath_prbs_test``
validate and calibrate the DAC’s performance, ensuring the precision and
reliability of signal outputs and data paths under varied conditions.

Miscellaneous
~~~~~~~~~~~~~

Additional functions include ``ad9144_remove`` for hardware cleanup,
``ad9144_status`` for retrieving the DAC’s operational status, and
``ad9144_get_lane_rate`` which is used for obtaining current lane rate
configurations.

Driver Initialization Example
-----------------------------

.. code-block:: C

   #include <stdlib.h>
   #include <stdio.h>
   #include <stdbool.h>
   #include <stdint.h>
   #include "no_os_delay.h"
   #include "no_os_spi.h"
   #include "no_os_util.h"
   #include "jesd204.h"
   #include "ad9144.h"

   struct ad9144_init_param init_param = {
       .spi_init = {
           .device_id       = SPI_DEVICE_ID,
           .max_speed_hz    = 1000000,
           .chip_select     = SPI_AD9144_CS,
           .mode            = NO_OS_SPI_MODE_0,
           .platform_ops    = &xil_spi_ops,
           .extra           = &xil_spi_init_param,
       },
       .spi3wire        = false,
       .num_converters  = 4,
       .num_lanes       = 8,
       .interpolation   = 2,
       .fcenter_shift   = 0,
       .stpl_samples    = { { 0x1111, 0x2222, 0x3333, 0x4444 },
                            { 0x5555, 0x6666, 0x7777, 0x8888 },
                            { 0x9999, 0xAAAA, 0xBBBB, 0xCCCC },
                            { 0xDDDD, 0xEEEE, 0xFFFF, 0x0000 } },
       .lane_rate_kbps  = 5000000,
       .prbs_type       = 0,
       .jesd204_mode    = 0,
       .jesd204_subclass= 1,
       .jesd204_scrambling = true,
       .lane_mux        = { 0, 1, 2, 3, 4, 5, 6, 7 },
       .pll_enable      = true,
       .pll_ref_frequency_khz = 50000,
       .pll_dac_frequency_khz = 1000000
   };

   struct ad9144_dev *dev;
   int32_t ret = ad9144_setup_legacy(&dev, &init_param);
   if (ret != 0)
       goto error;
   printf("AD9144 initialization successful\n");
   goto done;
   error:
       printf("AD9144 initialization failed: %ld\n", ret);
   done:
       ;
