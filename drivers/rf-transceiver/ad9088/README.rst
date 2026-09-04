AD9088 no-OS driver
===================

.. no-os-doxygen::

Supported Devices
-----------------

- :adi:`AD9088`
- :adi:`AD9084`

Overview
--------

The AD9088 and AD9084 are Apollo MxFE (mixed-signal front end) devices that
integrate wideband RF DACs, wideband RF ADCs, and a configurable digital
datapath into a single package. The AD9088 is an octal 16-bit, 16 GSPS RF DAC
with an octal 12-bit, 8 GSPS RF ADC (8T8R); the AD9084 is a quad 16-bit,
28 GSPS RF DAC with a quad 12-bit, 20 GSPS RF ADC (4T4R). Both are organized
as two independent *sides*; each side owns its converters, its coarse and fine
digital up/down converters (CDDC/FDDC and CDUC/FDUC), and its JESD204C link to
the host FPGA.

The datapath geometry is not fixed. It is described by a *device profile*
image that the driver loads over SPI during initialization, alongside signed
core firmware for the on-chip processor. The profile determines the JESD204
mode, converter counts, decimation and interpolation factors, and sample
rates. Both the profile and the firmware images are embedded in the
application binary at build time and handed to the device through a firmware
provider structure, so the driver needs no filesystem.

Control is over a 4-wire SPI bus (3-wire is selectable). A GPIO drives the
device reset, and an optional second GPIO carries the trigger request. Link
bring-up is driven by the no-OS JESD204 state machine: the driver registers a
``jesd204_dev_data`` table and the framework walks it through link setup,
clock enable, link enable, and the post-setup stages.

Most of the directory is the vendor Apollo API, shipped in-tree under
``public/``, ``private/``, ``adi_inc/``, and ``adi_utils/``. The ``ad9088_*``
files are the no-OS layer on top of it: they own the descriptor, the HAL
callbacks, the JESD204 state operations, and the higher-level helpers
documented below.

Applications
------------

* Multiband, multimode digital radios
* 3G/4G/5G massive MIMO base stations
* Wideband phased-array radar and electronic warfare
* Electronic test and measurement equipment
* Wideband satellite communications

AD9088 Device Configuration
---------------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~

In order to be able to use the device, you will have to provide the support
for the communication protocol (SPI) and a GPIO for the device reset.

The first API to be called is **ad9088_init**. Make sure that it returns 0,
which means that the driver was initialized correctly. It allocates the device
descriptor, resets the part, loads the core firmware and the device profile,
reads back the chip ID, and applies the JESD204 lane mappings and SERDES
settings from the init parameters.

**ad9088_parse_struct** performs the init-parameter-to-descriptor translation
on its own, without touching the hardware. **ad9088_init** calls it
internally; call it directly only when the descriptor has to exist before the
device is powered.

**ad9088_remove** frees the resources allocated by the init call.

The whole configuration of the part is passed through a single
**ad9088_init_param** structure. It covers the SPI mode and reset GPIO, the
JESD204 subclass and Nyquist zone, the board lane crossbars and serializer
settings, and the optional MCS handles.

``nyquist_zone`` must be 1 or 2; **ad9088_parse_struct** rejects any other
value with ``-EINVAL``. The four lane-mapping arrays —
``jtx0``/``jtx1_logical_lane_mapping`` and
``jrx0``/``jrx1_physical_lane_mapping`` — hold up to 12 entries each, one per
lane, and describe the board's crossbar between the device's lanes and the
FPGA's.

On success the driver prints the part it found, for example
``AD9088 Rev. 1 Grade 0 (API 1.2.3) probed``. The product id distinguishes the
two supported parts: ``0x9088`` for the 8T8R device and ``0x9084`` for the
4T4R one.

JESD204 Link Bring-up
~~~~~~~~~~~~~~~~~~~~~

The driver does not bring the link up on its own. It exports the
``jesd204_ad9088_init`` state-operations table, which the application passes
to the JESD204 framework when it builds the topology; the framework then runs
the link through to ``LINK_RUNNING``.

The AD9088 is the **top device** of that topology. The AXI JESD204
transport-layer cores in the FPGA are its inputs: the receive core feeds the
framer link and the transmit core the deframer link, one pair per side in use.
The application registers those cores alongside the AD9088 before starting the
state machine, so ordering across the devices is the framework's job rather
than the driver's.

Besides the mandatory states, the driver implements two optional setup stages
before the link is enabled. They exist as a pair because the converter
calibrations are slow — up to about 100 seconds — so the work is started in one
stage and collected in the next, leaving the framework free to run the other
devices' stage 1 in between rather than blocking the whole topology.

``OPT_SETUP_STAGE1``
  Applies the requested Nyquist zone, runs the clock-conditioning calibration,
  prints the negotiated JRx and JTx link parameters for every link, zeroes the
  JTx phase adjust, and then *starts* the ADC initial calibrations without
  waiting for them.

``OPT_SETUP_STAGE2``
  Blocks until those calibrations report complete, re-applies the Nyquist zone
  on top of the calibrated state, and re-runs the dynamic RX/TX link sync so
  the link clocks line up with it.

After the link is running the driver implements the three
``OPT_POST_SETUP_STAGE`` hooks and ``OPT_POST_RUNNING_STAGE``. MCS calibration
runs in the first of those, once the link is already up.

Link state can be inspected at any point with
**ad9088_inspect_jrx_link_all** and **ad9088_inspect_jtx_link_all**, which
walk every link on the device, and with
**ad9088_jesd_rx_link_status_print** and **ad9088_jesd_tx_link_status_print**,
which report a single link and optionally retry while it settles.
**ad9088_print_link_phase** and **ad9088_print_sysref_phase** report the
deterministic-latency phase measurements that subclass 1 depends on.

**ad9088_to_link** converts a framework link id into the device's own link
index, and **ad9088_check_apollo_error** maps a vendor API status into a no-OS
error code with a diagnostic naming the API that failed.

Multi-Chip Synchronization
~~~~~~~~~~~~~~~~~~~~~~~~~~

MCS calibration compensates the BSYNC path delay between the synchronization
source and the device, so that SYSREF is captured at a repeatable phase. It is 
skipped entirely unless both ``bsync_ops`` and ``clk_ops`` are supplied in the 
init parameters — the correct configuration for a board with no BSYNC provider 
wired to the device's SYSREF pin.

**ad9088_mcs_init_cal_setup** runs the initial calibration.
**ad9088_delta_t_measurement_set** and **ad9088_delta_t_measurement_get**
start and read back the round-trip delay measurement.
**ad9088_mcs_init_cal_validate** checks the achieved alignment against the
device clock tolerance, and **ad9088_mcs_init_cal_status_print** reports the
detail behind a result. **ad9088_mcs_tracking_cal_setup** arms the background
tracking calibration that holds the alignment while the link runs.

NCO Control
~~~~~~~~~~~

Each datapath carries a coarse NCO (CNCO) and a fine NCO (FNCO), selected by
terminal (receive or transmit), side, and converter index.

**ad9088_set_cnco_freq** and **ad9088_get_cnco_freq** set and read the coarse
NCO frequency in Hz; **ad9088_set_fnco_freq** and **ad9088_get_fnco_freq** do
the same for the fine NCO. The frequency-to-tuning-word conversion is exposed
separately as **adi_ad9088_calc_nco_ftw** and its inverse
**adi_ad9088_calc_nco_freq**, for callers that need the raw tuning word or
want to check the achievable frequency before committing to it.

Test Tones and Loopback
~~~~~~~~~~~~~~~~~~~~~~~

**ad9088_set_cnco_test_tone** and **ad9088_set_fnco_test_tone** enable a tone
generated inside the coarse or fine datapath at a given amplitude offset. A
receive-side test tone validates the capture path without any signal applied
to the ADC input.

**ad9088_set_loopback_mode** selects the internal datapath loopback mode for
one side, routing transmit data back into the receive path without leaving the
device.

AD9088 Driver Initialization Example
------------------------------------

.. code-block:: bash

	struct ad9088_phy *ad9088_phy;

	struct no_os_spi_init_param ad9088_spi_ip = {
		.device_id = APOLLO_SPI_DEVICE_ID,
		.max_speed_hz = 13000000,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.mode = NO_OS_SPI_MODE_0,
		.platform_ops = SPI_OPS_APOLLO,
		.extra = SPI_EXTRA_APOLLO,
		.chip_select = SPI_CS_APOLLO,
	};

	struct no_os_gpio_init_param gpio_reset_ip = {
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA,
		.number = GPIO_OFFSET + GPIO_RESET_N,
	};

	struct ad9088_init_param ad9088_ip = {
		.spi_init = &ad9088_spi_ip,
		.gpio_reset = &gpio_reset_ip,
		.gpio_tri_req = NULL,
		.spi_3wire_en = false,
		.trig_sync_en = false,
		.nyquist_zone = 1,
		.subclass = 1,
		/* Board lane crossbar; one entry per physical lane. */
		.jtx0_logical_lane_mapping = AD9088_TX0_LOGICAL_LANE_MAPPING,
		.jrx0_physical_lane_mapping = AD9088_RX0_PHYSICAL_LANE_MAPPING,
		.jtx_ser_amplitude = ADI_APOLLO_JESD_DRIVE_SWING_VTT_100,
		.jtx_ser_pre_emphasis = ADI_APOLLO_JESD_PRE_TAP_LEVEL_6_DB,
		.jtx_ser_post_emphasis = ADI_APOLLO_JESD_POST_TAP_LEVEL_3_DB,
		/*
		 * Leave bsync_ops/clk_ops NULL and MCS calibration is skipped. 
		 * Fill them in once the BSYNC provider and the clock chip have 
		 * been probed.
		 */
		.bsync_ops = NULL,
		.clk_ops = NULL,
		.mcs_track_decimation = 0,	/* use the driver default */
		.mcs_track_win = 0,		/* keep the profile's window */
	};

	ret = ad9088_init(&ad9088_phy, &ad9088_ip);
	if (ret)
		goto error;

The device profile embedded in the binary is selected by the
``CONFIG_AD9088_PROFILE`` Kconfig string and must match the HDL design loaded
on the FPGA.
