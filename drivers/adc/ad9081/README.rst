AD9081 no-OS Driver
===================

.. no-os-doxygen::

Supported Devices
-----------------

* `AD9081 <https://www.analog.com/AD9081>`_
* `AD9082 <https://www.analog.com/AD9082>`_

Overview
--------

The AD9081 is a mixed-signal front-end (MxFE) device featuring four 16-bit,
12 GSPS maximum sample rate RF digital-to-analog converter (DAC) cores and
four 12-bit, 4 GSPS RF analog-to-digital converter (ADC) cores. The device
is well suited for applications requiring both wideband ADCs and DACs to
process signals that have wide instantaneous bandwidth.

The device features eight transmit and eight receive lanes that support
24.75 Gbps/lane JESD204C or 15.5 Gbps/lane JESD204B standards. An on-chip
clock multiplier and digital signal processing (DSP) capability supports
either wideband or multiband direct-to-RF applications. The DSP datapaths
include eight fine complex DUCs and four coarse complex DUCs for transmit,
eight fine complex DDCs and four coarse complex DDCs for receive, each with
48-bit NCO. The DSP datapaths can be bypassed to allow a direct connection
between the converter cores and the JESD204 data transceiver port.

The AD9081 also features low latency loopback and frequency hopping modes
targeted at phased array radar and electronic warfare applications, on-chip
PLL with multichip synchronization, fast frequency hopping, receive AGC
support, transmit DPD support, and programmable 192-tap PFIR filter for
receive equalization. The device is packaged in a 15 mm x 15 mm, 324-ball
BGA with 0.8 mm pitch.

Applications
------------

* Wireless communications infrastructure
* Microwave point to point, E-band, and 5G mmWave
* Broadband communications systems
* DOCSIS 3.1 and 4.0 CMTS
* Phased array radar and electronic warfare
* Electronic test and measurement systems

Device Configuration
--------------------

The AD9081 driver provides a high-level interface to the underlying ADI API
for configuring and controlling the MxFE device. The driver functions are
organized into logical categories based on their role in the device setup and
operational pipeline.

Initialization
~~~~~~~~~~~~~~

The initialization function ``ad9081_init`` allocates resources and performs
the complete device bring-up sequence. It initializes the SPI interface and
GPIO reset pin, sets up HAL function pointers and serializer/deserializer
settings for JESD204 lanes, performs a hardware device reset, and validates
the chip identification. After successful identification, it calls the
internal ``ad9081_setup`` function to configure clocks, SYSREF, and data
converter paths, and then registers the device with the JESD204 framework.
The companion function ``ad9081_remove`` releases all allocated resources.

TX Path Configuration
~~~~~~~~~~~~~~~~~~~~~

The TX path is configured through the internal ``ad9081_setup_tx`` function,
which is called during initialization. This function sets up the four DAC
main datapaths and eight DAC channelizers by calling
``adi_ad9081_device_startup_tx`` with the interpolation rates, DAC-to-channel
crossbar selections, and NCO frequency shifts specified in the initialization
parameters. It also configures individual DAC channel gains, JESD RX LMFC
delay compensation, and DAC full-scale output currents.

RX Path Configuration
~~~~~~~~~~~~~~~~~~~~~

The RX path is configured through the internal ``ad9081_setup_rx`` function,
which sets up the four ADC main datapaths (coarse DDCs) and eight ADC
channelizers (fine DDCs). It calls ``adi_ad9081_device_startup_rx`` with
the decimation rates, CDDC and FDDC selections, NCO frequency shifts, and
complex-to-real enable settings. The function also configures the JESD TX
converter selection for each link and enables NCO synchronization when
applicable.

Clock and SYSREF Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ``ad9081_setup`` function manages the top-level device configuration
including SYSREF input coupling (AC or DC), CMOS input enable settings, and
the device clock configuration via ``adi_ad9081_device_clk_config_set``.
SYSREF management ensures proper alignment of JESD204 data streams and
maintains timing accuracy between converters and interfaces. The driver
supports both continuous and one-shot SYSREF modes with configurable
averaging.

GPIO Configuration
~~~~~~~~~~~~~~~~~~

To configure GPIO pins on the AD9081, the driver utilizes ADI API functions.
Functions like ``adi_ad9081_dac_gpio_as_sync1_out_set`` configure
synchronization outputs, and ``adi_ad9081_device_nco_sync_gpio_set`` adjusts
GPIO states for NCO synchronization. Configuration parameters are available
within the ``ad9081_init_param`` structure, and GPIO descriptors are outlined
in the ``ad9081_phy`` device structure.

Driver Initialization Example
-----------------------------

.. code-block:: c

   #include "ad9081.h"
   #include "app_clock.h"
   #include "app_jesd.h"
   #include "parameters.h"
   #include "app_config.h"

   struct no_os_clk app_clk[MULTIDEVICE_INSTANCE_COUNT];
   struct no_os_clk jesd_clk[2];

   struct link_init_param jrx_link_tx = {
       .device_id = 0,
       .octets_per_frame = AD9081_TX_JESD_F,
       .frames_per_multiframe = AD9081_TX_JESD_K,
       .samples_per_converter_per_frame = AD9081_TX_JESD_S,
       .high_density = AD9081_TX_JESD_HD,
       .converter_resolution = AD9081_TX_JESD_N,
       .bits_per_sample = AD9081_TX_JESD_NP,
       .converters_per_device = AD9081_TX_JESD_M,
       .control_bits_per_sample = AD9081_TX_JESD_CS,
       .lanes_per_device = AD9081_TX_JESD_L,
       .subclass = AD9081_TX_JESD_SUBCLASS,
       .link_mode = AD9081_TX_JESD_MODE,
       .dual_link = 0,
       .version = AD9081_TX_JESD_VERSION,
       .logical_lane_mapping = AD9081_TX_LOGICAL_LANE_MAPPING,
       .tpl_phase_adjust = AD9081_JRX_TPL_PHASE_ADJUST,
   };

   struct link_init_param jtx_link_rx = {
       .device_id = 0,
       .octets_per_frame = AD9081_RX_JESD_F,
       .frames_per_multiframe = AD9081_RX_JESD_K,
       .samples_per_converter_per_frame = AD9081_RX_JESD_S,
       .high_density = AD9081_RX_JESD_HD,
       .converter_resolution = AD9081_RX_JESD_N,
       .bits_per_sample = AD9081_RX_JESD_NP,
       .converters_per_device = AD9081_RX_JESD_M,
       .control_bits_per_sample = AD9081_RX_JESD_CS,
       .lanes_per_device = AD9081_RX_JESD_L,
       .subclass = AD9081_RX_JESD_SUBCLASS,
       .link_mode = AD9081_RX_JESD_MODE,
       .dual_link = 0,
       .version = AD9081_RX_JESD_VERSION,
       .logical_lane_mapping = AD9081_RX_LOGICAL_LANE_MAPPING,
       .link_converter_select = AD9081_RX_LINK_CONVERTER_SELECT,
   };

   struct ad9081_init_param phy_param = {
       .gpio_reset = &gpio_phy_resetb,
       .spi_init = &phy_spi_init_param,
       .dev_clk = &app_clk[0],
       .jesd_tx_clk = &jesd_clk[1],
       .jesd_rx_clk = &jesd_clk[0],
       .sysref_coupling_ac_en = 0,
       .multidevice_instance_count = 1,
       .jesd_sync_pins_01_swap_enable = false,
       .config_sync_0a_cmos_enable = false,
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
       .tx_maindp_dac_1x_non1x_crossbar_select = AD9081_TX_DAC_1X_NON1X_CROSSBAR,
       .tx_full_scale_current_ua = AD9081_TX_FSC,
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

   struct ad9081_phy *phy;
   int32_t status;

   status = app_clock_init(app_clk);
   if (status != 0)
       return status;

   status = app_jesd_init(jesd_clk, ADXCVR_REF_CLK_KHZ,
                          ADXCVR_RX_DEV_CLK_KHZ, ADXCVR_TX_DEV_CLK_KHZ,
                          ADXCVR_RX_LANE_CLK_KHZ, ADXCVR_TX_LANE_CLK_KHZ);
   if (status != 0)
       return status;

   status = ad9081_init(&phy, &phy_param);
   if (status != 0)
       return status;
