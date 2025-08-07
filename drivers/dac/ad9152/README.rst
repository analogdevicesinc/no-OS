AD9152 no-OS driver
=====================

Supported Devices
-----------------

- :adi:`AD9152`

Overview
--------

The AD9152 is a dual, 16-bit digital-to-analog converter (DAC) with a
sampling rate up to 2.25GSPS, suitable for generating multicarrier
signals up to its Nyquist frequency. It features advanced low spurious
and distortion techniques for high-quality wideband signal synthesis and
integrates a JESD204B Subclass 1 interface for streamlined multichip
synchronization. The DAC provides a 3- or 4-wire SPI interface,
allowing for programming and readback of parameters, with an adjustable
full-scale output current ranging from 4mA to 20mA. Encased in a
56-lead LFCSP, it is well-suited for applications including wireless
communications, software-defined radios, and instrumentation.

Applications
-------------

- Wireless communications
    - Multicarrier LTE and GSM base stations
    - Wideband repeaters
    - Software defined radios
- Wideband communications
    - Point to point microwave radios
    - LMDS/MMDS
- Transmit diversity, multiple input/multiple output (MIMO)
- Instrumentation
- Automated test equipment

Operation Modes
----------------

+-----------------+-----------------+-----------------+-----------------+
| **Mode Name**   | **Description** | **Register      | **Typical Use   |
|                 |                 | Configuration** | Case**          |
+-----------------+-----------------+-----------------+-----------------+
| **DC Test       | Provides a DC   | Register 0x0F7, | Testing and     |
| Mode**          | midscale test   | Bit 1           | calibration of  |
|                 | pattern to the  |                 | DAC outputs.    |
|                 | DACs for        |                 |                 |
|                 | evaluation      |                 |                 |
|                 | purposes.       |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| **1x            | Operates at     | Register 0x112: | Max bandwidth   |
| Interpolation** | maximum DAC     | 0x00            | applications    |
|                 | update rate     |                 | needing         |
|                 | without         |                 | high-speed      |
|                 | interpolation.  |                 | data.           |
+-----------------+-----------------+-----------------+-----------------+
| **2x            | Reduces input   | Register 0x112: | Efficient       |
| Interpolation** | data rate by    | 0x01            | processing of   |
|                 | half, applying  |                 | standard data   |
|                 | light           |                 | streams.        |
|                 | interpolation.  |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| **4x            | Further reduces | Register 0x112: | For scenarios   |
| Interpolation** | input data      | 0x02            | requiring       |
|                 | rate,           |                 | optimized       |
|                 | increasing      |                 | bandwidth       |
|                 | interpolation   |                 | usage.          |
|                 | for better      |                 |                 |
|                 | efficacy.       |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| **8x            | Maximizes       | Register 0x112: | Applications    |
| Interpolation** | interpolation,  | 0x03            | with stringent  |
|                 | significantly   |                 | data management |
|                 | lowering input  |                 | needs.          |
|                 | data needs.     |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| **CDR           | Clock and data  | REG_CDR         | Used in         |
| Half-Rate**     | recovery at     | _OPERATING_MODE | high-speed      |
|                 | half rate for   | _REG_0(0x230)   | serial data     |
|                 | high lane       | Bit 5           | transmission.   |
|                 | rates.          |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| **Continuous    | Continuously    | Register 0x03A, | Synchronizing   |
| Sync**          | checks for      | Bit 6           | clocks in       |
|                 | alignment edges |                 | multi-device    |
|                 | using periodic  |                 | setups.         |
|                 | SYSREF.         |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| **One Shot Sync | Phase checks    | Register 0x03A  | Single event    |
| Mode**          | occur once per  |                 | phase alignment |
|                 | alignment edge. |                 | checks.         |
+-----------------+-----------------+-----------------+-----------------+
| **Initial Setup | Configures      | REG             | Initial         |
| Mode**          | basic JESD204B  | _JESD204B_TERM0 | JESD204B link   |
|                 | interface       | (0x2AA),        | setup for       |
|                 | parameters.     | REG_SERDES_PLL  | communication   |
|                 |                 | _VCO_CONTROL0   | between         |
|                 |                 | (0x28A),        | converters and  |
|                 |                 | REG_CDR_OPERA   | processors.     |
|                 |                 | _OPERATING_MODE |                 |
|                 |                 | _REG_0 (0x230), |                 |
|                 |                 | REG_SYNTH       |                 |
|                 |                 | _ENABLE_CNTRL   |                 |
|                 |                 | (0x280)         |                 |
+-----------------+-----------------+-----------------+-----------------+
|                 | Controls        | REG_SYNC_CTRL   | Ensures         |
| Synchronization | synchronization | (0x03A),        | transmitter and |
| Mode            | of the JESD204B | REG_SYNC_STATUS | receiver        |
|                 | link.           | (0x03B)         | alignment in RF |
|                 |                 |                 | and             |
|                 |                 |                 | multi-ADC/DAC   |
|                 |                 |                 | systems.        |
+-----------------+-----------------+-----------------+-----------------+
| **Data Recovery | Restores raw    | REG_CDR_RESET   | Ensures data    |
| Mode**          | data from       | (0x206),        | integrity in    |
|                 | physical layer  | REG_EQ_BIAS_REG | high-data-rate  |
|                 | to digital      | (0x268)         | serial          |
|                 | signal.         |                 | communication.  |
+-----------------+-----------------+-----------------+-----------------+

Device Configuration
--------------------

Initialization
~~~~~~~~~~~~~~

The core initialization function is ``ad9152_setup`` , which is 
responsible for allocating resources, establishing SPI communication, 
verifying the chip ID, and initiating the power-up sequence for 
the AD9152 device. It ensures necessary configurations for the digital data path, 
transport, and physical layer including PLL locking. Conversely, 
``ad9152_remove`` handles deallocation of resources, removing the SPI descriptor 
and freeing memory to clean up driver usage.

Data Path Testing
~~~~~~~~~~~~~~~~~~

Functions ``ad9152_short_pattern_test`` and
``ad9152_datapath_prbs_test`` are designed for data path testing.
``ad9152_short_pattern_test`` writes and verifies short patterns in the
DAC registers to check data path integrity, whereas
``ad9152_datapath_prbs_test`` uses a pseudo-random binary sequence to
test and ensure synchronization in the I and Q data channels, overall
validating data handling capabilities.

Status Checking
~~~~~~~~~~~~~~~

The ``ad9152_status`` function is utilized to assess the JESD204B link
status. It reads several synchronization and error-checking registers,
reporting discrepancies to highlight potential issues with link
stability or data integrity, thereby evaluating the operational status
of the device.

Driver Initialization Example
------------------------------

.. code-block:: C

   #include <stdio.h>
   #include <stdint.h>
   #include "no_os_delay.h"
   #include "no_os_spi.h"
   #include "ad9152.h"

   int32_t ret;
   struct ad9152_dev *ad9152_device = NULL;
   ad9152_init_param ad9152_init = {
       .spi_init = {
           .device_id      = 0,
           .max_speed_hz   = 2000000,
           .chip_select    = 1,
           .mode       = NO_OS_SPI_MODE_0,
           .platform_ops   = &xil_spi_ops,
           .extra      = &xil_spi_param
       },
       .stpl_samples = {
           { 0x12345678, 0x23456789, 0x3456789A, 0x456789AB },
           { 0x56789ABC, 0x6789ABCD, 0x789ABCDE, 0x89ABCDEF }
       },
       .interpolation    = 2,
       .prbs_type        = PRBS_TYPE_1,
       .lane_rate_kbps   = 3072000
   };

   ret = ad9152_setup(&ad9152_device, &ad9152_init);
   if (ret)
       goto error;
   printf("AD9152 initialization success\n");
   ret = 0;
   goto end;
   error:
   printf("AD9152 initialization failed\n");
   ret = -1;
   end:
   if (ad9152_device)
       ad9152_remove(ad9152_device);
