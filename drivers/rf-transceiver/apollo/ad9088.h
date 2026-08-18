// SPDX-License-Identifier: GPL-2.0
/*
 * Driver for AD9088 and similar mixed signal front end (MxFE®)
 *
 * Copyright 2022 Analog Devices Inc.
 */
#ifndef __AD9088_H__
#define __AD9088_H__
//#define DEBUG

#include <stdio.h>
#include <stdlib.h>

#include "no_os_error.h"
#include "no_os_gpio.h"
#include "no_os_print_log.h"
#include "no_os_spi.h"

#include "jesd204.h"
#include "adi_apollo_bf_serdes_txdig_phy_core1p2.h"
#include "adi_apollo_bf_serdes_rxdig_phy_core1p3.h"
#include "public/inc/adi_apollo.h"
#include "adi_apollo_bf_custom.h"
#include "adi_apollo_adc.h"
#include "adi_apollo_cfg.h"
#include "adi_apollo_sysclk_cond.h"
#include "adi_apollo_hal.h"
#include "adi_apollo_mailbox.h"
#include "adi_apollo_loopback.h"
#include "adi_apollo_utils.h"

#include "adi_apollo_bf_mcs_sync.h"
#include "adi_apollo_bf_txrx_prefsrc_reconf.h"
#include "adi_apollo_bf_master_bias_ctrl.h"
#include "adi_apollo_sniffer.h"
#include "adi_apollo_cnco.h"
#include "adi_apollo_fnco.h"
#include "adi_apollo_cddc.h"
#include "adi_apollo_cduc.h"
#include "adi_utils.h"


// #include "../cf_axi_adc.h"

// #include <dt-bindings/iio/adc/adi,ad9088.h>
// #include "../../../misc/adi-axi-hsci.h"

#define JESD204_OF_PREFIX	"adi,"

#define DEFRAMER_LINK_A0_TX	0
#define DEFRAMER_LINK_A1_TX	1
#define DEFRAMER_LINK_B0_TX	2
#define DEFRAMER_LINK_B1_TX	3
#define FRAMER_LINK_A0_RX	4
#define FRAMER_LINK_A1_RX	5
#define FRAMER_LINK_B0_RX	6
#define FRAMER_LINK_B1_RX	7

#define CHIPID_AD9084 0x9084
#define CHIPID_AD9088 0x9088

#define CHIPID_MASK 0xFFFF
#define FW_TRANSFER_CHUNK_SIZE	(16 * 1024)


#define MAX_NUM_MAIN_DATAPATHS ADI_APOLLO_CNCO_NUM
#define MAX_NUM_CHANNELIZER ADI_APOLLO_FNCO_NUM
#define MAX_NUM_RX_NCO_CHAN_REGS 16
#define MAX_NUM_TX_NCO_CHAN_REGS 31

#define NUM_RXTX 2

/* MCS tracking cal decimation used when the init parameter leaves it at 0. */
#define AD9088_MCS_TRACK_DECIMATION_DEFAULT	1023

/*
 * Phase adjustment handed to the device-clock PLL when MCS tracking cal is
 * armed. Deliberately a minimal nudge: what matters is that the PLL has a
 * standing correction for the tracking loop to steer, not its magnitude.
 */
#define AD9088_MCS_CLK_PHASE_FS			125

#define for_each_cddc(bit, mask) \
	for ((bit) = 0; (bit) < MAX_NUM_MAIN_DATAPATHS; (bit)++) \
		if ((mask) & BIT(bit))

#define for_each_fddc(bit, mask) \
	for ((bit) = 0; (bit) < MAX_NUM_CHANNELIZER; (bit)++) \
		if ((mask) & BIT(bit))

/* Datapath loopback modes. Modes 2 (FDUC) and 3 (JESD) are not ported yet. */
enum {
	ADI_APOLLO_LOOPBACK_NONE,
	ADI_APOLLO_LOOPBACK_0,
	ADI_APOLLO_LOOPBACK_1,
	ADI_APOLLO_LOOPBACK_2,
	ADI_APOLLO_LOOPBACK_3,
};

enum {
	CDDC_NCO_FREQ,
	FDDC_NCO_FREQ,
	CDDC_NCO_FREQ_AVAIL,
	FDDC_NCO_FREQ_AVAIL,
	CDDC_NCO_PHASE,
	FDDC_NCO_PHASE,
	FDDC_NCO_GAIN,
	CDDC_HB1_6DB_GAIN,
	CDDC_TB1_6DB_GAIN,
	FDDC_6DB_GAIN,
	CDDC_TEST_TONE_EN,
	FDDC_TEST_TONE_EN,
	CDDC_TEST_TONE_OFFSET,
	FDDC_TEST_TONE_OFFSET,
	TRX_CONVERTER_RATE,
	TRX_ENABLE,
	CDDC_FFH_HOPF_SET,
	ADC_CDDC_FFH_TRIG_HOP_EN,
	ADC_FFH_GPIO_MODE_SET,
	CDDC_FFH_INDEX_SET,
	DAC_FFH_GPIO_MODE_SET,
	DAC_FFH_FREQ_SET,
	DAC_INVSINC_EN,
	CFIR_PROFILE_SEL,
	CFIR_ENABLE,
};

enum ad9088_iio_dev_attr {
	AD9088_JESD204_FSM_ERROR,
	AD9088_JESD204_FSM_PAUSED,
	AD9088_JESD204_FSM_STATE,
	AD9088_JESD204_FSM_RESUME,
	AD9088_JESD204_FSM_CTRL,
	AD9088_MCS_INIT,
	AD9088_DT0_MEASUREMENT,
	AD9088_DT1_MEASUREMENT,
	AD9088_DT_MEASUREMENT_RESTORE,
	AD9088_MCS_CAL_RUN,
	AD9088_MCS_TRACK_CAL_SETUP,
	AD9088_MCS_FG_TRACK_CAL_RUN,
	AD9088_MCS_BG_TRACK_CAL_RUN,
	AD9088_MCS_BG_TRACK_CAL_FREEZE,
	AD9088_MCS_TRACK_STATUS,
	AD9088_MCS_INIT_CAL_STATUS,
	AD9088_LOOPBACK_MODE_SIDE_A,
	AD9088_LOOPBACK_MODE_SIDE_B,
	AD9088_LOOPBACK1_BLEND_SIDE_A,
	AD9088_LOOPBACK1_BLEND_SIDE_B,
};

struct ad9088_jesd204_priv {
	struct ad9088_phy *phy;
	bool serdes_jrx_cal_run;
};

enum ad9088_clocks {
	RX_SAMPL_CLK,
	TX_SAMPL_CLK,
	RX_SAMPL_CLK_LINK2, /* Dual Link */
	NUM_AD9088_CLKS,
};

/* CNCO block-select masks indexed by [side][cddc_num] */
static const uint32_t cnco_masks[ADI_APOLLO_NUM_SIDES][4] = {
	{
		ADI_APOLLO_CNCO_A0, ADI_APOLLO_CNCO_A1,
		ADI_APOLLO_CNCO_A2, ADI_APOLLO_CNCO_A3
	},
	{
		ADI_APOLLO_CNCO_B0, ADI_APOLLO_CNCO_B1,
		ADI_APOLLO_CNCO_B2, ADI_APOLLO_CNCO_B3
	},
};

/* FNCO/FDDC block-select masks indexed by [side][fddc_num] */
static const uint32_t fnco_masks[ADI_APOLLO_NUM_SIDES][8] = {
	{
		ADI_APOLLO_FDDC_A0, ADI_APOLLO_FDDC_A1,
		ADI_APOLLO_FDDC_A2, ADI_APOLLO_FDDC_A3,
		ADI_APOLLO_FDDC_A4, ADI_APOLLO_FDDC_A5,
		ADI_APOLLO_FDDC_A6, ADI_APOLLO_FDDC_A7
	},
	{
		ADI_APOLLO_FDDC_B0, ADI_APOLLO_FDDC_B1,
		ADI_APOLLO_FDDC_B2, ADI_APOLLO_FDDC_B3,
		ADI_APOLLO_FDDC_B4, ADI_APOLLO_FDDC_B5,
		ADI_APOLLO_FDDC_B6, ADI_APOLLO_FDDC_B7
	},
};

// struct ad9088_clock {
// 	struct clk_hw hw;
// 	struct spi_device *spi;
// 	struct ad9088_phy *phy;
// 	unsigned long rate;
// 	enum ad9088_clocks source;
// };

// #define to_clk_priv(_hw) container_of(_hw, struct ad9088_clock, hw)

struct ad9088_debugfs_entry {
	struct iio_dev *indio_dev;
	const char *propname;
	void *out_value;
	uint32_t val;
	uint8_t size;
	uint8_t cmd;
};

/**
 * @struct ad9088_bsync_ops
 * @brief Accessors for the board's BSYNC (SYSREF) provider, used by MCS
 *	  calibration.
 *
 * MCS calibration has to drive and measure the SYSREF source, but which chip
 * that is belongs to the board rather than to this driver. The board supplies
 * these callbacks so the driver stays independent of the clock chip.
 */
struct ad9088_bsync_ops {
	/** Opaque handle passed back to every callback. */
	void *ctx;
	/** BSYNC output rate, in Hz. */
	int (*freq_get)(void *ctx, uint32_t *freq_hz);
	/** Drive the SYSREF line (true) or release it so the AD9088 can (false). */
	int (*output_en_set)(void *ctx, bool en);
	/**
	 * Time difference from the alignment reference, in fs. State-perturbing:
	 * it arms the provider's TDC, so a following delay_set() is required to
	 * return the provider to its aligned state.
	 */
	int (*tdc_measure)(void *ctx, int64_t *tdc_fs);
	/** Set the SYSREF channel delay, in fs, and realign the channel. */
	int (*delay_set)(void *ctx, int64_t delay_fs);
	/** Enable continuous background realignment of all synced channels. */
	int (*bg_align_set)(void *ctx, bool en);
};

/**
 * @struct ad9088_clk_ops
 * @brief Accessors for the board's device-clock PLL, used by MCS tracking
 *	  calibration to correct clock drift.
 */
struct ad9088_clk_ops {
	/** Opaque handle passed back to every callback. */
	void *ctx;
	/** Re-apply the phase adjustment on every resync rather than once. */
	int (*auto_align_set)(void *ctx, bool en);
	/** Set the signed clock phase adjustment, in fs. */
	int (*phase_set_fs)(void *ctx, int32_t phase_fs);
};

struct ad9088_init_param {
	struct no_os_spi_init_param *spi_init; //
	struct no_os_gpio_init_param *gpio_reset; //
	struct no_os_gpio_init_param *gpio_tri_req; //
	struct no_os_gpio_init_param *versal_xvr_reset; //
	const char *device_profile_fw_name;
	// struct no_os_regulator_init_param *supply_reg;
	bool spi_3wire_en; //
	bool rx_real_channel_en; //
	bool tx_real_channel_en; //
	bool side_b_use_own_tpl_en; //
	uint32_t multidevice_instance_count; //
	bool trig_sync_en; //
	bool standalone_en; //
	uint32_t nyquist_zone; //
	uint32_t subclass;
	uint32_t jtx0_logical_lane_mapping[12]; //
	uint32_t jtx1_logical_lane_mapping[12]; //
	uint32_t jrx0_physical_lane_mapping[12]; //
	uint32_t jrx1_physical_lane_mapping[12]; //
	uint32_t jtx_ser_amplitude;
	uint32_t jtx_ser_pre_emphasis;
	uint32_t jtx_ser_post_emphasis;
	/*
	 * Optional. Supply both to enable MCS calibration; leave either NULL and
	 * it is skipped, which is the correct configuration for a board with no
	 * BSYNC provider wired to the AD9088's SYSREF pin.
	 */
	const struct ad9088_bsync_ops *bsync_ops;
	const struct ad9088_clk_ops *clk_ops;
	/**
	 * MCS tracking cal decimation. 0 selects
	 * AD9088_MCS_TRACK_DECIMATION_DEFAULT.
	 */
	uint32_t mcs_track_decimation;
	/**
	 * Device-clock drift window in fs. 0 leaves the value carried by the
	 * loaded device profile in place.
	 */
	uint32_t mcs_track_win;
	/** Run the BSYNC provider's background serial alignment while linked. */
	bool aion_background_serial_alignment_en;
};

struct ad9088_phy {
	struct no_os_spi_desc *spi;
	struct jesd204_dev *jdev;
	adi_apollo_device_t ad9088;
	adi_apollo_fw_provider_t fw_provider;
	adi_apollo_top_t profile;
	adi_cms_chip_id_t chip_id;
	// struct axiadc_chip_info chip_info;
	// struct clk *dev_clk;
	// struct bin_attribute pfilt;
	// struct bin_attribute cfir;
	// struct gpio_chip gpiochip;

	struct no_os_gpio_desc *triq_req_gpio;
	struct no_os_gpio_desc *reset_gpio;
	// struct regulator *supply_reg;

	// struct clk *clks[NUM_AD9088_CLKS];
	// struct clock_scale clkscale[NUM_AD9088_CLKS];
	// struct ad9088_clock clk_priv[NUM_AD9088_CLKS];
	// struct clk_onecell_data clk_data;

	// struct delayed_work dwork;


	// /*
	//  * Synchronize access to members of driver state, and ensure atomicity
	//  * of consecutive regmap operations.
	//  */
	// struct mutex		lock;

	bool is_initialized;
	bool standalone; //
	bool device_profile_firmware_load;
	bool side_b_use_own_tpl_en; //
	bool complex_tx;
	bool complex_rx;
	bool spi_3wire_en; //
	bool log_silent;
	bool trig_sync_en; //
	bool mcs_cal_bg_tracking_run;
	bool mcs_cal_bg_tracking_freeze;
	uint32_t multidevice_instance_count; //

	struct ad9088_debugfs_entry debugfs_entry[16];
	uint32_t ad9088_debugfs_entry_index;

	const char **rx_labels;
	const char **tx_labels;

	char rx_chan_labels[MAX_NUM_CHANNELIZER][32];
	char tx_chan_labels[MAX_NUM_CHANNELIZER][32];

	//long long cnco_freq[NUM_RXTX][ADI_APOLLO_NUM_SIDES][MAX_NUM_MAIN_DATAPATHS];
	long long cnco_phase[NUM_RXTX][ADI_APOLLO_NUM_SIDES][MAX_NUM_MAIN_DATAPATHS];
	uint16_t cnco_test_tone_offset[NUM_RXTX][ADI_APOLLO_NUM_SIDES][MAX_NUM_MAIN_DATAPATHS];
	bool cnco_test_tone_en[NUM_RXTX][ADI_APOLLO_NUM_SIDES][MAX_NUM_MAIN_DATAPATHS];

	//long long fnco_freq[NUM_RXTX][ADI_APOLLO_NUM_SIDES][MAX_NUM_CHANNELIZER];
	long long fnco_phase[NUM_RXTX][ADI_APOLLO_NUM_SIDES][MAX_NUM_CHANNELIZER];
	uint16_t fnco_test_tone_offset[NUM_RXTX][ADI_APOLLO_NUM_SIDES][MAX_NUM_CHANNELIZER];
	bool fnco_test_tone_en[NUM_RXTX][ADI_APOLLO_NUM_SIDES][MAX_NUM_CHANNELIZER];

	bool cnco_dual_modulus_mode_en;
	bool fnco_dual_modulus_mode_en;

	uint8_t cfir_profile[NUM_RXTX][ADI_APOLLO_CFIR_ALL][ADI_APOLLO_CFIR_DP_ALL];
	uint8_t cfir_enable[NUM_RXTX][ADI_APOLLO_CFIR_ALL][ADI_APOLLO_CFIR_DP_ALL];

	uint8_t rx_nyquist_zone[ADI_APOLLO_NUM_SIDES][MAX_NUM_MAIN_DATAPATHS];
	uint8_t jrx_lanes[24]; //
	uint8_t jtx_lanes[24]; //
	uint8_t jrx_lanes_used;
	uint8_t jtx_lanes_used;

	uint8_t rx_en_mask; //
	uint8_t tx_en_mask; //

	// u32 hsci_rx_clk_adj;
	// u32 hsci_tx_clk_adj;
	// bool hsci_rx_clk_inv;
	// bool hsci_tx_clk_inv;
	// bool hsci_use_dt_param;
	// bool hsci_use_auto_linkup_mode;
	// bool hsci_disable_after_initial_configuration;

	/*
	 * MCS calibration accessors for the board's BSYNC (SYSREF) source and
	 * device-clock PLL. NULL when the board has no such chips wired, in
	 * which case MCS calibration is skipped.
	 */
	const struct ad9088_bsync_ops *bsync_ops;
	const struct ad9088_clk_ops *clk_ops;

	/** MCS tracking cal decimation. */
	uint32_t mcs_track_decimation;
	/** Device-clock drift window, in fs, before tracking cal corrects. */
	uint32_t mcs_track_win;
	/** Run the BSYNC provider's background serial alignment while linked. */
	bool aion_background_serial_alignment_en;

	adi_apollo_sniffer_param_t sniffer_config;
	adi_apollo_sniffer_fft_data_t fft_data;

	// u8 hsci_buf[32767 + 4];
	uint8_t gpios_exported[ADI_APOLLO_NUM_GPIO];
	char dbuf[1024];

	uint8_t loopback_mode[ADI_APOLLO_NUM_SIDES];
	uint8_t lb1_blend[ADI_APOLLO_NUM_SIDES];
};

extern const uint8_t
_binary_drivers_rf_transceiver_apollo_firmware_usecase_bin_start[];
extern const uint8_t
_binary_drivers_rf_transceiver_apollo_firmware_usecase_bin_end[];
extern const uint8_t
_binary_drivers_rf_transceiver_apollo_firmware_usecase_bin_size[];

extern const uint8_t
_binary_drivers_rf_transceiver_apollo_firmware_app_signed_encrypted_B_flash_image_0x01030000_bin_start[];
extern const uint8_t
_binary_drivers_rf_transceiver_apollo_firmware_app_signed_encrypted_B_flash_image_0x01030000_bin_end[];
extern const uint8_t
_binary_drivers_rf_transceiver_apollo_firmware_app_signed_encrypted_B_flash_image_0x20000000_bin_start[];
extern const uint8_t
_binary_drivers_rf_transceiver_apollo_firmware_app_signed_encrypted_B_flash_image_0x20000000_bin_end[];
extern const uint8_t
_binary_drivers_rf_transceiver_apollo_firmware_app_signed_encrypted_B_flash_image_0x02000000_bin_start[];
extern const uint8_t
_binary_drivers_rf_transceiver_apollo_firmware_app_signed_encrypted_B_flash_image_0x02000000_bin_end[];
extern const uint8_t
_binary_drivers_rf_transceiver_apollo_firmware_app_signed_encrypted_B_flash_image_0x21000000_bin_start[];
extern const uint8_t
_binary_drivers_rf_transceiver_apollo_firmware_app_signed_encrypted_B_flash_image_0x21000000_bin_end[];

extern const uint8_t
_binary_drivers_rf_transceiver_apollo_firmware_app_signed_encrypted_prod_B_flash_image_0x01030000_bin_start[];
extern const uint8_t
_binary_drivers_rf_transceiver_apollo_firmware_app_signed_encrypted_prod_B_flash_image_0x01030000_bin_end[];
extern const uint8_t
_binary_drivers_rf_transceiver_apollo_firmware_app_signed_encrypted_prod_B_flash_image_0x20000000_bin_start[];
extern const uint8_t
_binary_drivers_rf_transceiver_apollo_firmware_app_signed_encrypted_prod_B_flash_image_0x20000000_bin_end[];
extern const uint8_t
_binary_drivers_rf_transceiver_apollo_firmware_app_signed_encrypted_prod_B_flash_image_0x02000000_bin_start[];
extern const uint8_t
_binary_drivers_rf_transceiver_apollo_firmware_app_signed_encrypted_prod_B_flash_image_0x02000000_bin_end[];
extern const uint8_t
_binary_drivers_rf_transceiver_apollo_firmware_app_signed_encrypted_prod_B_flash_image_0x21000000_bin_start[];
extern const uint8_t
_binary_drivers_rf_transceiver_apollo_firmware_app_signed_encrypted_prod_B_flash_image_0x21000000_bin_end[];

extern const struct jesd204_dev_data jesd204_ad9088_init;

int ad9088_check_apollo_error(int ret, const char *api_name);
uint8_t ad9088_to_link(uint8_t linkid);
int ad9088_inspect_jrx_link_all(struct ad9088_phy *phy);
int ad9088_inspect_jtx_link_all(struct ad9088_phy *phy);
void ad9088_print_link_phase(struct ad9088_phy *phy, struct jesd204_link *lnk);
void ad9088_print_sysref_phase(struct ad9088_phy *phy);
int ad9088_jesd_tx_link_status_print(struct ad9088_phy *phy,
				     struct jesd204_link *lnk, int retry);
int ad9088_jesd_rx_link_status_print(struct ad9088_phy *phy,
				     struct jesd204_link *lnk, int retry);
extern const char *const ad9088_fsm_links_to_str[];

/* MCS (Multi-Chip Synchronization) calibration -- ad9088_mcs.c */
int ad9088_mcs_init_cal_setup(struct ad9088_phy *phy);
int ad9088_delta_t_measurement_set(struct ad9088_phy *phy, uint32_t mode);
int ad9088_delta_t_measurement_get(struct ad9088_phy *phy, uint32_t mode,
				   int64_t *apollo_delta_t);
int ad9088_mcs_init_cal_validate(struct ad9088_phy *phy,
				 adi_apollo_mcs_cal_init_status_t *cal_status);
void ad9088_mcs_init_cal_status_print(struct ad9088_phy *phy,
				      adi_apollo_mcs_cal_init_status_t *cal_status);
void ad9088_mcs_track_cal_status_print(struct ad9088_phy *phy,
				       adi_apollo_mcs_cal_status_t *cal_status,
				       uint8_t print_full_state);
int ad9088_mcs_tracking_cal_setup(struct ad9088_phy *phy,
				  uint32_t mcs_track_decimation,
				  uint16_t initialize_track_cal);

/* NCO (Numerically Controlled Oscillator) control -- Step 6 */
int adi_ad9088_calc_nco_ftw(struct ad9088_phy *phy, uint64_t freq,
			    int64_t nco_shift, uint32_t div, uint32_t bits,
			    uint64_t *ftw, uint64_t *frac_a, uint64_t *frac_b);
int adi_ad9088_calc_nco_freq(struct ad9088_phy *phy, uint64_t freq,
			     uint64_t ftw, uint32_t a, uint32_t b,
			     uint32_t bits, int64_t *nco_shift);
int ad9088_set_cnco_freq(struct ad9088_phy *phy, adi_apollo_terminal_e terminal,
			 uint8_t side, uint8_t cddc_num, int64_t freq_hz);
int ad9088_get_cnco_freq(struct ad9088_phy *phy, adi_apollo_terminal_e terminal,
			 uint8_t side, uint8_t cddc_num, int64_t *freq_hz);
int ad9088_set_fnco_freq(struct ad9088_phy *phy, adi_apollo_terminal_e terminal,
			 uint8_t side, uint8_t fddc_num, int64_t freq_hz);
int ad9088_get_fnco_freq(struct ad9088_phy *phy, adi_apollo_terminal_e terminal,
			 uint8_t side, uint8_t fddc_num, int64_t *freq_hz);

/* NCO test tones and datapath loopback -- Step 9 (partial) */
int ad9088_set_cnco_test_tone(struct ad9088_phy *phy,
			      adi_apollo_terminal_e terminal, uint8_t side,
			      uint8_t cddc_num, bool enable, uint16_t offset);
int ad9088_set_fnco_test_tone(struct ad9088_phy *phy,
			      adi_apollo_terminal_e terminal, uint8_t side,
			      uint8_t fddc_num, bool enable, uint16_t offset);
int ad9088_set_loopback_mode(struct ad9088_phy *phy, uint8_t side,
			     uint8_t mode);

int ad9088_parse_struct(struct ad9088_phy **device,
			const struct ad9088_init_param *init_param);
int ad9088_init(struct ad9088_phy **device,
		const struct ad9088_init_param *init_param);
int ad9088_remove(struct ad9088_phy *phy);
int ad9088_fft_sniffer_probe(struct ad9088_phy *phy,
			     adi_apollo_side_select_e side_sel);

#endif /* __AD9088_H__ */
