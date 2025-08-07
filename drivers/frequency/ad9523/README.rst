AD9523 no-OS driver
===================

Supported Devices
-----------------

- :adi:`AD9523`

Overview
---------

The AD9523 is a multi-output clock distribution device with a low jitter
phase-locked loop (PLL) and a voltage-controlled oscillator (VCO) that
operates between 3.6 GHz and 4.0 GHz. It is optimized for LTE and
multicarrier GSM base stations, using an external voltage-controlled
crystal oscillator (VCXO) to minimize reference jitter and enhance phase
noise for superior data converter SNR. With 14 low noise outputs ranging
from 1 MHz to 1 GHz and a dedicated output from the input PLL (PLL1), it
supports diverse communication and instrumentation applications. The
device features a programmable EEPROM for user-defined power-up and
reset configurations.

Applications
------------

- LTE and multicarrier
- GSM base stations
- Wireless and broadband infrastructure
- Medical instrumentation
- Clocking high speed ADCs, DACs, DDSs, DDCs, DUCs, MxFEs
- Low jitter, low phase noise clock distribution
- Clock generation and translation for SONET, 10Ge, 10G FC, and other 
  10Gbps protocols
- Forward error correction (G.710)
- High performance wireless transceivers
- ATE and high-performance instrumentation

Operation Modes
----------------

+-----------------+-----------------+-----------------+-----------------+
| Mode Name       | Description     | Configuration   | Typical Use     |
|                 |                 | Bits            | Case            |
+-----------------+-----------------+-----------------+-----------------+
| TRISTATE        | Set the output  | Set the         | Disable the     |
|                 | driver to a     | ``driver_mode`` | output or leave |
|                 | high-impedance  | field to        | it in high      |
|                 | state.          | ``TRISTATE``    | impedance.      |
|                 |                 | using the       |                 |
|                 |                 | driver API.     |                 |
+-----------------+-----------------+-----------------+-----------------+
| LVPECL_8mA      | Set the output  | Set the         | Use LVPECL      |
|                 | driver for      | ``driver_mode`` | signaling that  |
|                 | LVPECL          | field to        | needs higher    |
|                 | operation with  | ``LVPECL_8mA``  | drive strength. |
|                 | an 8mA drive    | via the driver  |                 |
|                 | current.        | interface.      |                 |
+-----------------+-----------------+-----------------+-----------------+
| LVDS_4mA        | Set the output  | Set the         | Use low-current |
|                 | driver for LVDS | ``driver_mode`` | differential    |
|                 | operation with  | field to        | LVDS signaling. |
|                 | a 4mA drive     | ``LVDS_4mA``    |                 |
|                 | current.        | using the       |                 |
|                 |                 | provided macro. |                 |
+-----------------+-----------------+-----------------+-----------------+
| LVDS_7mA        | Set the output  | Set the         | Use LVDS        |
|                 | driver for LVDS | ``driver_mode`` | signaling with  |
|                 | operation with  | field to        | a higher drive  |
|                 | a 7mA drive     | ``LVDS_7mA``    | current.        |
|                 | current.        | using the       |                 |
|                 |                 | driver header.  |                 |
+-----------------+-----------------+-----------------+-----------------+
| HSTL0_16mA      | Set the output  | Set the         | Use HSTL        |
|                 | driver for HSTL | ``driver_mode`` | signaling that  |
|                 | operation with  | field to        | needs higher    |
|                 | a 16mA drive    | ``HSTL0_16mA``  | current.        |
|                 | current.        | through the     |                 |
|                 |                 | header API.     |                 |
+-----------------+-----------------+-----------------+-----------------+
| HSTL1_8mA       | Set the output  | Set the         | Use HSTL        |
|                 | driver for HSTL | ``driver_mode`` | signaling with  |
|                 | operation with  | field to        | lower current   |
|                 | an 8mA drive    | ``HSTL1_8mA``   | requirements.   |
|                 | current.        | via the driver  |                 |
|                 |                 | interface.      |                 |
+-----------------+-----------------+-----------------+-----------------+
| CMOS_CONF1      | Set the output  | Set the         | Use CMOS        |
|                 | driver for CMOS | ``driver_mode`` | signaling with  |
|                 | operation in    | field to        | configuration   |
|                 | configuration   | ``CMOS_CONF1``  | mode 1.         |
|                 | mode 1.         | using the       |                 |
|                 |                 | defined         |                 |
|                 |                 | enumeration.    |                 |
+-----------------+-----------------+-----------------+-----------------+
| CMOS_CONF2      | Set the output  | Set the         | Use CMOS        |
|                 | driver for CMOS | ``driver_mode`` | signaling with  |
|                 | operation in    | field to        | configuration   |
|                 | configuration   | ``CMOS_CONF2``  | mode 2.         |
|                 | mode 2.         | via the header. |                 |
+-----------------+-----------------+-----------------+-----------------+
| CMOS_CONF3      | Set the output  | Set the         | Use CMOS        |
|                 | driver for CMOS | ``driver_mode`` | applications    |
|                 | operation in    | field to        | with mode 3     |
|                 | configuration   | ``CMOS_CONF3``  | settings.       |
|                 | mode 3.         | using the API.  |                 |
+-----------------+-----------------+-----------------+-----------------+
| CMOS_CONF4      | Set the output  | Set the         | Use CMOS        |
|                 | driver for CMOS | ``driver_mode`` | signaling with  |
|                 | operation in    | field to        | configuration   |
|                 | configuration   | ``CMOS_CONF4``  | mode 4.         |
|                 | mode 4.         | via the driver  |                 |
|                 |                 | header.         |                 |
+-----------------+-----------------+-----------------+-----------------+
| CMOS_CONF5      | Set the output  | Set the         | Use CMOS        |
|                 | driver for CMOS | ``driver_mode`` | applications in |
|                 | operation in    | field to        | configuration   |
|                 | configuration   | ``CMOS_CONF5``  | mode 5.         |
|                 | mode 5.         | using the       |                 |
|                 |                 | driver          |                 |
|                 |                 | interface.      |                 |
+-----------------+-----------------+-----------------+-----------------+
| CMOS_CONF6      | Set the output  | Set the         | Use CMOS        |
|                 | driver for CMOS | ``driver_mode`` | signaling that  |
|                 | operation in    | field to        | requires mode   |
|                 | configuration   | ``CMOS_CONF6``  | 6.              |
|                 | mode 6.         | via the API.    |                 |
+-----------------+-----------------+-----------------+-----------------+
| CMOS_CONF7      | Set the output  | Set the         | Use CMOS        |
|                 | driver for CMOS | ``driver_mode`` | applications    |
|                 | operation in    | field to        | with mode 7     |
|                 | configuration   | ``CMOS_CONF7``  | parameters.     |
|                 | mode 7.         | using the       |                 |
|                 |                 | header.         |                 |
+-----------------+-----------------+-----------------+-----------------+
| CMOS_CONF8      | Set the output  | Set the         | Use CMOS        |
|                 | driver for CMOS | ``driver_mode`` | signaling with  |
|                 | operation in    | field to        | configuration   |
|                 | configuration   | ``CMOS_CONF8``  | mode 8.         |
|                 | mode 8.         | via the driver  |                 |
|                 |                 | API.            |                 |
+-----------------+-----------------+-----------------+-----------------+
| CMOS_CONF9      | Set the output  | Set the         | Use CMOS        |
|                 | driver for CMOS | ``driver_mode`` | applications    |
|                 | operation in    | field to        | with mode 9     |
|                 | configuration   | ``CMOS_CONF9``  | parameters.     |
|                 | mode 9.         | using the       |                 |
|                 |                 | defined macro.  |                 |
+-----------------+-----------------+-----------------+-----------------+

Device Configuration
---------------------

Initialization
~~~~~~~~~~~~~~

The initialization functions manage the basic setup and removal of the
device structure and communication settings. The function
``ad9523_init()`` initializes the device data with default values. The
function ``ad9523_setup()`` allocates the device structure, configures
the SPI interface, resets the device, and applies the initial
configuration. The function ``ad9523_remove()`` cleans up resources by
removing the SPI descriptor and freeing the device structure.

SPI Communication
~~~~~~~~~~~~~~~~~

The SPI communication functions handle the low-level operations to
access the device registers. The function ``ad9523_spi_read()`` reads
registers from the device using SPI, and the function
``ad9523_spi_write()`` writes values to the device registers via SPI.

I/O Update and Synchronization
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The I/O update and synchronization functions apply configuration changes
and synchronize the device settings. The function ``ad9523_io_update()``
writes to the I/O update register to make configuration changes
effective. The function ``ad9523_sync()`` toggles the manual control bit
in the status signals register and updates the configuration to
synchronize the device.

Output Mapping and Calibration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The output mapping and calibration functions adjust the clock output
mapping and perform calibration. The function ``ad9523_vco_out_map()``
configures the clock provider for a selected output channel by writing
to the appropriate device registers. The function ``ad9523_calibrate()``
performs calibration of the voltage-controlled oscillator and verifies
the operation by checking the status.

Device status
~~~~~~~~~~~~~

The device status function monitors the current state of the device. The
function ``ad9523_status()`` checks the lock status of the VCXO and the
PLLs, and returns an error code if the device status is not as expected.

Driver Initialization Example
-----------------------------

.. code-block:: C

   #include <stdlib.h>
   #include <stdio.h>
   #include <stdint.h>
   #include "ad9523.h"
   #include "no_os_alloc.h"
   #include "no_os_delay.h"
   #include "no_os_spi.h"

   struct ad9523_channel_spec ch0 = {
       .channel_num                = 0,
       .divider_output_invert_en   = 0,
       .sync_ignore_en             = 0,
       .low_power_mode_en          = 0,
       .use_alt_clock_src          = 0,
       .output_dis                 = 0,
       .driver_mode                = LVDS_7mA,
       .divider_phase              = 0,
       .channel_divider            = 100,
       .extended_name              = "CH0"
   };

   struct ad9523_platform_data pdata = {
       .vcxo_freq                  = 40000000,
       .spi3wire                   = 0,
       .refa_diff_rcv_en           = 1,
       .refb_diff_rcv_en           = 1,
       .zd_in_diff_en              = 0,
       .osc_in_diff_en             = 0,
       .refa_cmos_neg_inp_en       = 0,
       .refb_cmos_neg_inp_en       = 0,
       .zd_in_cmos_neg_inp_en      = 0,
       .osc_in_cmos_neg_inp_en     = 0,
       .refa_r_div                 = 10,
       .refb_r_div                 = 10,
       .pll1_feedback_div          = 125,
       .pll1_charge_pump_current_nA = 2000,
       .zero_delay_mode_internal_en = 1,
       .osc_in_feedback_en         = 0,
       .pll1_bypass_en             = 0,
       .pll1_loop_filter_rzero     = RZERO_135_OHM,
       .ref_mode                   = SELECT_REFA,
       .pll2_charge_pump_current_nA = 1500,
       .pll2_ndiv_a_cnt            = 4,
       .pll2_ndiv_b_cnt            = 20,
       .pll2_freq_doubler_en       = 0,
       .pll2_r2_div                = 15,
       .pll2_vco_diff_m1           = 4,
       .pll2_vco_diff_m2           = 4,
       .rpole2                     = RPOLE2_300_OHM,
       .rzero                      = RZERO_2250_OHM,
       .cpole1                     = CPOLE1_16_PF,
       .rzero_bypass_en            = 0,
       .num_channels               = 1,
       .channels                   = &ch0,
       .name                       = "AD9523Device"
   };

   struct ad9523_init_param init_param = {
       .spi_init = {
           .device_id   = 0,
           .max_speed_hz = 1000000,
           .chip_select = 0,
           .mode        = NO_OS_SPI_MODE_0,
           .platform_ops = &xil_spi_ops,
           .extra       = &xil_spi_param
       },
       .pdata = &pdata
   };

   struct ad9523_dev *dev;
   int32_t ret;

   ret = ad9523_setup(&dev, &init_param);
   if (ret != 0)
       goto error;
   printf("AD9523 initialization success\n");
   goto cleanup;

   #error
   printf("AD9523 initialization failed\n");

   cleanup:
   ret = ad9523_remove(dev);
   if (ret != 0)
       printf("AD9523 remove failed\n");
