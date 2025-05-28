AD9081 no-OS Driver
===================

Overview
--------

The AD9081 is a mixed-signal front-end (MxFE) device
designed for high-speed signal processing applications. It features four
16-bit, 12 GSPS DAC cores and four 12-bit, 4 GSPS ADC cores, offering
flexible digital signal processing with interpolation and decimation
options. The device supports JESD204B/C high-speed serial interfaces
with up to 16 lanes and data rates up to 24.75 Gbps, facilitating
seamless integration with digital systems. It includes advanced
synchronization mechanisms such as SYSREF management and supports
digital upconversion and downconversion, making it suitable for wireless
communications, broadband systems, and phased array radars. Implemented
in C, the AD9081 driver’s API allows for device initialization,
configuration, and control, featuring JESD204 link management, clock
configuration, and NCO synchronization along with error handling and
logging functions.

Supported Devices
-----------------

- :adi:`AD9081`

Applications
------------

- Wireless communications infrastructure
- Microwave point to point, E-band, and 5G mmWave
- Broadband communications systems
- DOCSIS 3.1 and 4.0 CMTS
- Phased array radar and electronic warfare
- Electronic test and measurement systems

Operation Modes
---------------

+----------------------------+---------------------------+-----------------------------+-------------------------------+
| Mode Name                  | Description               | Configuration Bits          | Typical Use                   |
+----------------------------+---------------------------+-----------------------------+-------------------------------+
| adi_ad9081_test_mode_e     | ADC test modes: Midscale, | AD9081_TMODE_* values       | ADC signal pattern testing    |
|                            | Full-Scale, RAMP, PN seq. |                             |                               |
+----------------------------+---------------------------+-----------------------------+-------------------------------+
| adi_ad9081_dac_mod_mux     | DAC signal mux modes.     | AD9081_DAC_MUX_MODE_*       | DAC output routing            |
| _mode_e                    |                           | values                      |                               |
+----------------------------+---------------------------+-----------------------------+-------------------------------+
| adi_ad9081_adc_nco_mode_e  | ADC NCO: Variable/Zero IF | AD9081_ADC_NCO_* values     | ADC NCO configuration         |
+----------------------------+---------------------------+-----------------------------+-------------------------------+
| adi_ad9081_adc_adc_to      | ADC to DDC crossbar       | AD9081_ADC_*_MODE values    | Real/complex ADC config       |
| _cddc_xbar_e               | settings.                 |                             |                               |
+----------------------------+---------------------------+-----------------------------+-------------------------------+
| adi_ad9081_adc_pfir_i      | PFIR I-mode filter config | AD9081_ADC_PFIR_I_MODE_*    | ADC input filtering           |
| _mode_e                    |                           | values                      |                               |
+----------------------------+---------------------------+-----------------------------+-------------------------------+
| adi_ad9081_adc_pfir_q      | PFIR Q-mode filter config | AD9081_ADC_PFIR_Q_MODE_*    | Q-channel filtering           |
| _mode_e                    |                           | values                      |                               |
+----------------------------+---------------------------+-----------------------------+-------------------------------+
| adi_ad9081_adc_bypass      | ADC bypass modes.         | AD9081_ADC_*_MODE values    | Data path/diagnostics         |
| _mode_e                    |                           |                             |                               |
+----------------------------+---------------------------+-----------------------------+-------------------------------+
| adi_ad9081_jesd_rx_prbs    | JESD RX PRBS test modes.  | AD9081_JESD_RX_PRBS_TEST    | RX data integrity test        |
| _test_mode_e               |                           | _MODE_* values              |                               |
+----------------------------+---------------------------+-----------------------------+-------------------------------+
| adi_ad9081_jesd_tx_test    | JESD TX test modes.       | AD9081_JESD_TX_TEST_MODE_*  | TX signal validation          |
| _mode_e                    |                           | values                      |                               |
+----------------------------+---------------------------+-----------------------------+-------------------------------+
| adi_ad9081_deser_mode_e    | JESD deserializer modes.  | AD9081_*_RATE modes         | Set deserialization rate      |
+----------------------------+---------------------------+-----------------------------+-------------------------------+
| adi_ad9081_cal_mode_e      | JESD calibration modes.   | AD9081_CAL_MODE_* values    | JESD calibration              |
+----------------------------+---------------------------+-----------------------------+-------------------------------+

Device Configuration
---------------------

Initialization Functions
~~~~~~~~~~~~~~~~~~~~~~~~

The initialization functions in the AD9081 driver focus on setting up
essential hardware components such as clocks, JESD204 links, and data
converters. The ``app_clock_init`` function initializes application
clocks by configuring SPI and GPIO parameters. The ``app_jesd_init``
function sets up the JESD204 transmitter and receiver interfaces and
ADXCVR transceivers. The ``main`` function orchestrates the overall
setup process, ensuring all components are correctly configured for
high-speed data transactions and synchronization.

Configuration Functions
~~~~~~~~~~~~~~~~~~~~~~~

Configuration functions in the AD9081 system specify parameters for
devices and interfaces. For instance, ``app_jesd_init`` configures clock
rates and JESD204 subclass settings crucial for data alignment and
synchronization. These configurations customize the operation of the
AD9081 to meet specific application needs by defining frequencies and
JESD link parameters.

Synchronization Functions
~~~~~~~~~~~~~~~~~~~~~~~~~

Synchronization functions ensure coherent high-speed data transfer and
signal processing on the AD9081 device. This includes managing
SYSREF signals to align JESD204 data streams, maintaining timing
accuracy between converters and interfaces. Functions leverage devices
like the HMC7044 clock generator for clock distribution and jitter
cleaning, which is critical for processes requiring coherent signal
phases.

Data Handling Functions
~~~~~~~~~~~~~~~~~~~~~~~

Data handling functions manage data flow through ADC and DAC interfaces
and DMA operations. The ``main`` function initializes ADC and DAC cores,
ensuring efficient digital-to-analog and analog-to-digital conversions.
Buffers like ``dac_buffer`` and ``adc_buffer`` store samples for
processing or transmission, while DMA controllers move data to and from
memory. These functions support high-speed signal processing
applications by efficiently handling high-rate data streams.

GPIO Configuration
~~~~~~~~~~~~~~~~~~

To configure GPIO pins on the AD9081, utilize the no-OS codebase driver
API functions. Use macros like ``AD9081_PERI_SEL_GPIO`` to define GPIO
pins and functions. Functions like
``adi_ad9081_dac_gpio_as_sync1_out_set`` configure synchronization
outputs, and ``adi_ad9081_device_nco_sync_gpio_set`` adjusts GPIO states
for NCO synchronization. Configuration parameters are available within
structures like ``ad9081_init_param``, and GPIO descriptors are outlined
in ``ad9081_phy``.

Driver Initialization Example
-----------------------------

.. code-block:: C

   #include <stdio.h>
   #include <inttypes.h>
   #include "no_os_gpio.h"
   #include "no_os_spi.h"
   #include "ad9081.h"
   #include "app_clock.h"
   #include "app_jesd.h"

   int int_main(void) {
       // Declare variables for SPI, GPIO initialization
       struct xil_gpio_init_param xil_gpio_param = {
   #ifdef PLATFORM_MB
           .type = GPIO_PL,
   #else
           .type = GPIO_PS,
   #endif
           .device_id = XPAR_AXI_GPIO_DEVICE_ID
       };

       struct no_os_gpio_init_param gpio_phy_resetb = {
           .number = 55,
           .platform_ops = &xil_gpio_ops,
           .extra = &xil_gpio_param
       };

       struct xil_spi_init_param xil_spi_param = {
   #ifdef PLATFORM_MB
           .type = SPI_PL,
   #else
           .type = SPI_PS,
   #endif
       };

       struct no_os_spi_init_param phy_spi_init_param = {
           .device_id = XPAR_AXI_SPI_DEVICE_ID,
           .max_speed_hz = 1000000,
           .mode = NO_OS_SPI_MODE_0,
           .chip_select = 0,
           .platform_ops = &xil_spi_ops,
           .extra = &xil_spi_param
       };

       // Define the AD9081 initialization parameters
       struct ad9081_init_param phy_param = {
           .gpio_reset = &gpio_phy_resetb,
           .spi_init = &phy_spi_init_param,
           .dev_clk = &app_clk[0],
           .jesd_tx_clk = &jesd_clk[1],
           .jesd_rx_clk = &jesd_clk[0],
           .sysref_coupling_ac_en = 0,
           .sysref_cmos_input_enable = 0,
           .config_sync_0a_cmos_enable = 0,
           .multidevice_instance_count = 1,
           .jesd_sync_pins_01_swap_enable = false,
           .lmfc_delay_dac_clk_cycles = 0,
           .nco_sync_ms_extra_lmfc_num = 0,
           .nco_sync_direct_sysref_mode_enable = 0,
           .sysref_average_cnt_exp = 7,
           .continuous_sysref_mode_disable = 0,
           .tx_disable = false,
           .rx_disable = false,
           .dac_frequency_hz = AD9081_DAC_FREQUENCY,
           .tx_main_interpolation = AD9081_TX_MAIN_INTERPOLATION,
           .tx_main_nco_frequency_shift_hz = AD9081_TX_MAIN_NCO_SHIFT,
           .tx_dac_channel_crossbar_select = AD9081_TX_DAC_CHAN_CROSSBAR,
           .tx_channel_interpolation = AD9081_TX_CHAN_INTERPOLATION,
           .tx_channel_nco_frequency_shift_hz = AD9081_TX_CHAN_NCO_SHIFT,
           .tx_channel_gain = AD9081_TX_CHAN_GAIN,
           .jrx_link_tx[0] = &jrx_link_tx,
           .jrx_link_tx[1] = NULL,
           .adc_frequency_hz = AD9081_ADC_FREQUENCY,
           .nyquist_zone = AD9081_ADC_NYQUIST_ZONE,
           .rx_main_nco_frequency_shift_hz = AD9081_RX_MAIN_NCO_SHIFT,
           .rx_main_decimation = AD9081_RX_MAIN_DECIMATION,
           .rx_main_complex_to_real_enable = {0, 0, 0, 0},
           .rx_main_enable = AD9081_RX_MAIN_ENABLE,
           .rx_channel_nco_frequency_shift_hz = AD9081_RX_CHAN_NCO_SHIFT,
           .rx_channel_decimation = AD9081_RX_CHAN_DECIMATION,
           .rx_channel_complex_to_real_enable = {0, 0, 0, 0, 0, 0, 0, 0},
           .rx_channel_enable = AD9081_RX_CHAN_ENABLE,
           .jtx_link_rx[0] = &jtx_link_rx,
           .jtx_link_rx[1] = NULL,
       };

       // Placeholder for function execution output, success expected (0)
       int status = 0;

       // Initialize clocks
       status = app_clock_init(app_clk);
       if (status < 0) {
           printf("Clock initialization failed\n");
           return -1;
       }

       // Initialize JESD interfaces
       status = app_jesd_init(jesd_clk, 0, 0, 0, 0, 0);
       if (status < 0) {
           printf("JESD initialization failed\n");
           return -1;
       }

       // Initialize AD9081 device with provided parameters
       status = ad9081_setup(&phy_param);
       if (status < 0) {
           printf("AD9081 initialization failed\n");
           return -1;
       }

       printf("AD9081 initialization successful\n");

       // Implement further operational setup here...

       return 0;
   }
