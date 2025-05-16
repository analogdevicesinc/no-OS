ADRV9002 no-OS Driver
=====================

.. no-os-doxygen::

Supported Devices
-----------------

* `ADRV9002 <https://www.analog.com/ADRV9002>`_

Overview
--------

The ADRV9002 is a highly integrated, dual-channel RF transceiver featuring
dual transmitters, dual receivers, integrated synthesizers, and digital signal
processing functions. It operates over a frequency range of 30 MHz to 6000 MHz,
covering VHF, UHF, ISM, and cellular bands with transmitter and receiver
bandwidths from 12 kHz to 40 MHz. The device supports both TDD and FDD
operation.

The transceiver consists of direct conversion signal paths with state-of-the-art
noise figure and linearity. Each complete receiver and transmitter subsystem
includes DC offset correction, quadrature error correction (QEC), and
programmable digital filters. Two fully integrated, fractional-N RF synthesizers
provide high performance, low power frequency synthesis. All VCO and loop filter
components are integrated to minimize the external component count.

The ADRV9002 includes low power sleep and monitor modes for extended battery life
in portable devices, fully integrated low power digital predistortion (DPD)
optimized for both narrow-band and wideband signals, fast frequency hopping,
dynamic profile switching, and multichip synchronization capabilities. Auxiliary
ADCs, DACs, and GPIOs are integrated to provide additional monitoring and
control capability.

High data rate and low data rate interfaces are supported using configurable
CMOS or LVDS synchronous serial interface (SSI) options. The device is
controlled via a standard 4-wire SPI and is packaged in a 12 mm x 12 mm,
196-ball CSP_BGA.

Applications
------------

* Mission critical communications
* Very high frequency (VHF), ultrahigh frequency (UHF), and cellular to 6 GHz
* Time division duplexing (TDD) and frequency division duplexing (FDD)
  applications

Device Configuration
--------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~

The ADRV9002 driver is initialized through ``adrv9002_dev_init`` which
configures the device channels, AGC parameters, and stores the JSON device
profile. The initialization requires an ``adrv9002_init_param`` structure
containing chip information, a reference clock descriptor, AGC configuration
defaults, and the device profile. Following device initialization, the AXI
ADC and DAC cores must be set up and ``adrv9002_post_setup`` called to perform
digital interface tuning and complete the transceiver configuration.

Transceiver Path Management
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ADRV9002 driver manages RX and TX signal paths through internal
configuration functions that handle gain control, transmit power, and channel
state transitions. RX path configuration applies pin-mode gain control and AGC
settings, while TX path configuration handles digital pre-distortion and
attenuation pin control. Channels can be transitioned between primed and RF
enabled states for TDD and FDD operation using ``adrv9002_channel_to_state``.

Frequency Hopping and Calibration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The driver supports dynamic frequency switching through frequency hopping modes
configured during initialization. The ADRV9002 dynamically adapts its operating
frequency to limit interference using PLL retunes and hop table management.
Initial calibrations are performed during setup via the ADI API to align
internal parameters with hardware states and environmental conditions.

Digital Interface Tuning
~~~~~~~~~~~~~~~~~~~~~~~~

The ``adrv9002_post_setup`` function configures the AXI ADC and DAC cores,
determines the SSI type (CMOS or LVDS), and performs iterative clock and data
delay adjustments through ``adrv9002_axi_intf_tune`` to optimize the digital
interface. Test pattern functions such as ``adrv9002_check_tx_test_pattern``
validate signal integrity across transmission and reception paths.

Driver Initialization Example
-----------------------------

.. code-block:: c

   #include "adrv9002.h"
   #include "adi_adrv9001.h"
   #include "no_os_clk.h"
   #include "Navassa_CMOS_profile.h"

   struct adrv9002_init_param init_par = { 0 };
   struct adi_adrv9001_Device adrv9001_device = { 0 };
   struct no_os_clk_init_param clk_init = { 0 };
   struct adrv9002_chip_info chip = { 0 };
   struct adrv9002_rf_phy phy = { 0 };
   int ret;

   /* Configure chip information */
   chip.cmos_profile = "Navassa_CMOS_profile.json";
   chip.lvd_profile = "Navassa_LVDS_profile.json";
   chip.name = "adrv9002-phy";
   chip.num_channels = 4;
   chip.n_tx = ADRV9002_CHANN_MAX;

   /* Set up ADI device HAL */
   phy.adrv9001 = &adrv9001_device;
   phy.adrv9001->common.devHalInfo = &phy.hal;

   /* Initialize reference clock */
   clk_init.name = "adrv9002_ext_refclk";
   clk_init.hw_ch_num = 1;
   clk_init.platform_ops = &dev_clk_ops;
   clk_init.dev_desc = &phy;

   ret = no_os_clk_init(&init_par.dev_clk, &clk_init);
   if (ret)
       return ret;

   /* Configure init parameters */
   init_par.chip = &chip;
   init_par.agcConfig_init_param = &agc_cfg;
   init_par.profile = (char *)json_profile;
   init_par.profile_length = strlen(json_profile);

   /* Initialize the ADRV9002 device */
   ret = adrv9002_dev_init(&phy, &init_par);
   if (ret)
       return ret;

   /* Post-setup: digital interface tuning */
   ret = adrv9002_post_setup(&phy);
   if (ret)
       goto error;

IIO Device Initialization Example
----------------------------------

.. code-block:: c

   static int32_t iio_run(struct iio_axi_adc_init_param *adc_pars,
                          struct iio_axi_dac_init_param *dac_pars)
   {
       struct iio_axi_adc_desc     *adcs[IIO_DEV_COUNT];
       struct iio_axi_dac_desc     *dacs[IIO_DEV_COUNT];
       struct iio_data_buffer      iio_dac_buffers[IIO_DEV_COUNT];
       struct iio_data_buffer      iio_adc_buffers[IIO_DEV_COUNT];
       struct iio_device           *iio_descs[IIO_DEV_COUNT * 2];
       struct iio_app_device       app_devices[IIO_DEV_COUNT * 2] = {0};
       struct xil_uart_init_param  platform_uart_init_par = {
           .type = UART_PS,
           .irq_id = UART_IRQ_ID
       };
       struct no_os_uart_init_param iio_uart_ip = {
           .device_id = UART_DEVICE_ID,
           .irq_id = UART_IRQ_ID,
           .baud_rate = UART_BAUDRATE,
           .size = NO_OS_UART_CS_8,
           .parity = NO_OS_UART_PAR_NO,
           .stop = NO_OS_UART_STOP_1_BIT,
           .extra = &platform_uart_init_par,
           .platform_ops = &xil_uart_ops
       };
       struct iio_app_desc         *app;
       struct iio_app_init_param   app_init_param = { 0 };
       int32_t                     i, ret;
       int32_t                     a;

       for (i = 0; i < IIO_DEV_COUNT; i++) {
           /* ADC setup */
           iio_adc_buffers[i].buff = adc_buffers[i];
           iio_adc_buffers[i].size = sizeof(adc_buffers[i]);
           ret = iio_axi_adc_init(&adcs[i], &adc_pars[i]);
           if (ret < 0)
               return ret;
           a = 2 * i;
           iio_axi_adc_get_dev_descriptor(adcs[i], &iio_descs[a]);
           app_devices[a].name = (char *)adc_pars[i].rx_adc->name;
           app_devices[a].dev = adcs[i];
           app_devices[a].dev_descriptor = iio_descs[a];
           app_devices[a].read_buff = &iio_adc_buffers[i];

           /* DAC setup */
           iio_dac_buffers[i].buff = dac_buffers[i];
           iio_dac_buffers[i].size = sizeof(dac_buffers[i]);
           ret = iio_axi_dac_init(&dacs[i], &dac_pars[i]);
           if (ret < 0)
               return ret;
           a = 2 * i + 1;
           iio_axi_dac_get_dev_descriptor(dacs[i], &iio_descs[a]);
           app_devices[a].name = (char *)dac_pars[i].tx_dac->name;
           app_devices[a].dev = dacs[i];
           app_devices[a].dev_descriptor = iio_descs[a];
           app_devices[a].write_buff = &iio_dac_buffers[i];
       }

       app_init_param.devices = app_devices;
       app_init_param.nb_devices = NO_OS_ARRAY_SIZE(app_devices);
       app_init_param.uart_init_params = iio_uart_ip;

       ret = iio_app_init(&app, app_init_param);
       if (ret)
           return ret;

       return iio_app_run(app);
   }
