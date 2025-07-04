ADF4350 no-OS Driver
====================

Supported Devices
------------------

- :adi:`ADF4350`

Overview
--------

The ADF4350 is a wideband synthesizer with an integrated
voltage-controlled oscillator (VCO), capable of generating frequencies
from 137.5 MHz to 4400 MHz. It supports both fractional-N and integer-N
synthesizer capabilities and includes a programmable output divider 
(1 to 16) to offer flexibility in frequency generation. The device delivers
low phase noise with typical RMS jitter of less than 0.4 ps, making it
suitable for RF applications. The ADF4350 features RF output muting, 
a 3-wire serial interface, and can be configured with external loop
filters and reference frequencies, facilitating integration into various
RF communication systems including wireless infrastructure and test
equipment.

Applications
-------------

- Wireless infrastructure (W-CDMA, TD-SCDMA, WiMAX, GSM, PCS, DCS, DECT)
- Test equipment
- Wireless LANs, CATV equipment
- Clock generation

Device Configuration
---------------------

Initialization
~~~~~~~~~~~~~~

The ``adf4350_setup`` function initializes the ADF4350 frequency
synthesizer by allocating memory for the device structure and
configuring the SPI interface. Essential parameters such as input clock
frequency, channel spacing, and power-up frequency are set based on user
configuration, preparing the device for operation.

Frequency Configuration
~~~~~~~~~~~~~~~~~~~~~~~

These functions manage frequency configuration. ``adf4350_set_freq`` sets
the output frequency by calculating and applying necessary register
values. ``adf4350_out_altvoltage0_frequency`` leverages ``adf4350_set_freq``
to implement frequency adjustments, simplifying the frequency setting
application.

``adf4350_out_altvoltage0_frequency_resolution`` manages the channel
spacing or frequency resolution by updating internal configurations. 
It ensures that frequency steps align with application requirements,
providing fine-grained frequency control.

Reference Input
~~~~~~~~~~~~~~~

``adf4350_out_altvoltage0_refin_frequency`` function allows setting and
retrieving the reference input frequency. It ensures the internal clock
complies with specified operational parameters, aiding in maintaining
stable frequency synthesis.

Power Management
~~~~~~~~~~~~~~~~

``adf4350_out_altvoltage0_powerdown`` handles transitions between active
and low-power states by setting appropriate register bits, supporting
applications prioritizing power efficiency.

Data Communication Functions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ``adf4350_write`` function transmits data to the ADF4350 registers,
segmenting 32-bit data into 8-bit chunks for SPI compatibility.
``adf4350_sync_config`` synchronizes software with hardware register
values, ensuring configuration consistency through conditional
buffering.

Miscellaneous Functions
~~~~~~~~~~~~~~~~~~~~~~~

``adf4350_tune_r_cnt`` adjusts the R counter for maintaining the PFD
frequency under a defined limit. It incrementally adjusts values to
achieve suitable frequency division settings. The ``gcd`` function computes
the greatest common divisor using the Euclidean algorithm. It supports
optimizing modulus calculations in frequency settings, reinforcing
configuration precision.

Operation Modes
----------------

+-----------------+-----------------+-------------------------------------+-----------------+
| **Mode Name**   | **Description** | **Configuration Bits**              | **Typical Use   |
|                 |                 |                                     | Case**          |
+-----------------+-----------------+-------------------------------------+-----------------+
| Band Select     | Activates high  | band_select_clock_mode_high_enable  | Scenarios       |
| Clock Mode      | band select     |                                     | requiring fine  |
|                 | clock mode for  |                                     | frequency       |
|                 | specific        |                                     | selection.      |
|                 | frequency       |                                     |                 |
|                 | selection.      |                                     |                 |
+-----------------+-----------------+-------------------------------------+-----------------+
| Feedback Mode   | Adjusts         | ADF4350_REG4_FEEDBACK_DIVIDED       | Systems needing |
|                 | feedback path   |                                     | stabilized      |
|                 | to divided      |                                     | frequency       |
|                 | mode, altering  |                                     | output.         |
|                 | the feedback    |                                     |                 |
|                 | loop            |                                     |                 |
|                 | configuration.  |                                     |                 |
+-----------------+-----------------+-------------------------------------+-----------------+
| LDF Mode        | Configures load | ADF4350_REG2_LDF_FRACT_N            | Applications    |
|                 | detection for   |                                     | needing precise |
|                 | fractional-N    |                                     | frequency       |
|                 | mode,           |                                     | synthesis.      |
|                 | optimizing PLL  |                                     |                 |
|                 | operation.      |                                     |                 |
+-----------------+-----------------+-------------------------------------+-----------------+
| Noise Mode      | Sets noise mode | ADF4350_REG2_NOISE_MODE             | Environments    |
|                 | via register    |                                     | sensitive to    |
|                 | configuration,  |                                     | noise           |
|                 | affecting       |                                     | performance.    |
|                 | synthesizer     |                                     |                 |
|                 | noise.          |                                     |                 |
+-----------------+-----------------+-------------------------------------+-----------------+
| MUXOUT Mode     | Controls output | ADF4350_REG2_MUXOUT                 | Situations      |
|                 | mode of the     |                                     | requiring       |
|                 | MUXOUT pin by   |                                     | specific pin    |
|                 | setting the     |                                     | output modes,   |
|                 | configuration.  |                                     | like test       |
|                 |                 |                                     | setups.         |
+-----------------+-----------------+-------------------------------------+-----------------+
| Lock Detect     | Sets lock       | ADF4350_REG5_LD_PIN_MODE_DIGITAL    | Applications    |
| Mode            | detect pin to   |                                     | monitoring      |
|                 | digital mode to |                                     | frequency lock  |
|                 | manage lock     |                                     | stability.      |
|                 | indication.     |                                     |                 |
+-----------------+-----------------+-------------------------------------+-----------------+

Driver Initialization Example
-----------------------------

.. code-block:: C

   #include <stdio.h>
   #include "xil_cache.h"
   #include "xparameters.h"
   #include "no_os_spi.h"
   #include "adf4350.h"
   #include "parameters.h"
   #include "xilinx_spi.h"
   #include "no_os_print_log.h"

   /* SPI Initialization Structures */
   struct xil_spi_init_param xil_spi_param = {
       .type = SPI_PS,
   };

   struct no_os_spi_init_param adf4350_spi_param = {
       .device_id = XPAR_PS7_SPI_0_DEVICE_ID,
       .max_speed_hz = 5000000u,
       .chip_select = 0,
       .mode = NO_OS_SPI_MODE_0,
       .extra = &xil_spi_param,
       .platform_ops = &xil_spi_ops,
   };

   adf4350_init_param default_adf4350_init_param = {
       .spi_init = adf4350_spi_param,
       .clkin = 25000000,
       .channel_spacing = 10000,
       .power_up_frequency = 2500000000ul,
       .reference_div_factor = 0,
       .reference_doubler_enable = 0,
       .reference_div2_enable = 0,
       .phase_detector_polarity_positive_enable = 1,
       .lock_detect_precision_6ns_enable = 0,
       .lock_detect_function_integer_n_enable = 0,
       .charge_pump_current = 2500,
       .muxout_select = 0,
       .low_spur_mode_enable = 0,
       .cycle_slip_reduction_enable = 0,
       .charge_cancellation_enable = 0,
       .anti_backlash_3ns_enable = 0,
       .band_select_clock_mode_high_enable = 0,
       .clk_divider_12bit = 0,
       .clk_divider_mode = 0,
       .aux_output_enable = 0,
       .aux_output_fundamental_enable = 1,
       .mute_till_lock_enable = 0,
       .output_power = 3,
       .aux_output_power = 0,
   };

   adf4350_dev *adf4350_device;

   int main(void)
   {
       int32_t status;

       Xil_ICacheEnable();
       Xil_DCacheEnable();

       status = adf4350_setup(&adf4350_device, default_adf4350_init_param);
       if (status != 0) {
           pr_info("adf4350_setup() failed!");
           return -1;
       }

       pr_info("Done.\n");

       Xil_ICacheDisable();
       Xil_DCacheDisable();

       return 0;
   }
