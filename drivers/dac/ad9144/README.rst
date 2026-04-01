AD9144 no-OS Driver
===================

.. no-os-doxygen::

Supported Devices
-----------------

* `AD9144 <https://www.analog.com/AD9144>`_

Overview
--------

The AD9144 is a quad, 16-bit, 2.8 GSPS digital-to-analog converter (DAC)
designed to generate multicarrier signals up to the Nyquist frequency. It
interfaces seamlessly with the ADRF6720 analog quadrature modulator from
Analog Devices. The device features a JESD204B Subclass 1 serial data
interface with up to eight lanes, reducing the required pin count while
enabling synchronized multichip operation. The DAC output current is
programmable between 13.9 mA and 27.0 mA. Additional features include
3-wire and 4-wire SPI support, a programmable transmit enable function for
optimizing power consumption and wake-up time, and an internal PLL for
flexible clocking. The AD9144 is housed in an 88-lead LFCSP package.

Applications
------------

* 3G/4G W-CDMA base stations
* Wideband repeaters
* Software defined radios
* Point-to-point communications
* Local multipoint distribution service (LMDS) and multichannel multipoint
  distribution service (MMDS)
* Transmit diversity and MIMO
* Instrumentation
* Automated test equipment

Device Configuration
--------------------

Initialization and Setup
~~~~~~~~~~~~~~~~~~~~~~~~

The driver provides two initialization paths. The ``ad9144_setup_legacy``
function initializes the device without the JESD204 finite state machine,
while ``ad9144_setup_jesd_fsm`` initializes the device with JESD204 FSM
support for automated link bring-up. Both functions accept an
``ad9144_init_param`` structure containing SPI configuration, JESD204B
link parameters, converter/lane counts, interpolation settings, and PLL
configuration.

SPI Communication
~~~~~~~~~~~~~~~~~

The AD9144 uses SPI for register access. The driver provides
``ad9144_spi_read`` for reading register values, ``ad9144_spi_write``
for writing register values, and ``ad9144_spi_check_status`` for verifying
that a register field matches an expected value.

JESD204B Link Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The JESD204B link is configured during initialization via the
``ad9144_setup_jesd204_link`` function, which sets up the link parameters
including the number of converters, lanes, samples per frame, and octets
per frame based on the selected JESD204B mode. The device supports multiple
link modes with varying combinations of converters (1, 2, or 4), lanes
(1 to 8), and framing options.

NCO Configuration
~~~~~~~~~~~~~~~~~

The ``ad9144_set_nco`` function configures the numerically controlled
oscillator, allowing the user to set a carrier frequency offset (in kHz)
and phase for the DAC output signal.

Testing and Calibration
~~~~~~~~~~~~~~~~~~~~~~~

The driver includes ``ad9144_dac_calibrate`` for DAC calibration,
``ad9144_short_pattern_test`` for verifying JESD204B short transport layer
pattern data, and ``ad9144_datapath_prbs_test`` for pseudo-random bit
sequence testing of the data path.

Status and Cleanup
~~~~~~~~~~~~~~~~~~

The ``ad9144_status`` function retrieves the operational status of the
device, including lane and link synchronization state. The
``ad9144_remove`` function releases all allocated resources associated
with the device.

Driver Initialization Example
-----------------------------

.. code-block:: c

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
       .spi3wire            = 0,
       .num_converters      = 4,
       .num_lanes           = 8,
       .interpolation       = 2,
       .fcenter_shift       = 0,
       .stpl_samples        = { { 0x1111, 0x2222, 0x3333, 0x4444 },
                                { 0x5555, 0x6666, 0x7777, 0x8888 },
                                { 0x9999, 0xAAAA, 0xBBBB, 0xCCCC },
                                { 0xDDDD, 0xEEEE, 0xFFFF, 0x0000 } },
       .lane_rate_kbps      = 5000000,
       .prbs_type           = 0,
       .jesd204_mode        = 0,
       .jesd204_subclass    = 1,
       .jesd204_scrambling  = 1,
       .lane_mux            = { 0, 1, 2, 3, 4, 5, 6, 7 },
       .pll_enable          = 1,
       .pll_ref_frequency_khz = 50000,
       .pll_dac_frequency_khz = 1000000,
   };

   struct ad9144_dev *dev;
   int32_t ret;

   ret = ad9144_setup_legacy(&dev, &init_param);
   if (ret)
       goto error;
