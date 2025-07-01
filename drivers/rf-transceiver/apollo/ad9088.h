// SPDX-License-Identifier: GPL-2.0
/*
 * Driver for AD9088 and similar mixed signal front end (MxFE®)
 *
 * Copyright 2022 Analog Devices Inc.
 */
//#define DEBUG


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
#include "adi_apollo_hal.h"
#include "adi_apollo_mailbox.h"
#include "adi_apollo_loopback.h"
#include "adi_apollo_utils.h"

#include "adi_apollo_bf_mcs_sync.h"
#include "adi_apollo_bf_txrx_prefsrc_reconf.h"
#include "adi_apollo_bf_master_bias_ctrl.h"
#include "adi_apollo_sniffer.h"


// #include "../cf_axi_adc.h"

// #include <dt-bindings/iio/adc/adi,ad9088.h>
// #include "../../../misc/adi-axi-hsci.h"

#define JESD204_OF_PREFIX	"adi,"

#define CHIPID_AD9084 0x9084
#define CHIPID_AD9088 0x9088

#define CHIPID_MASK 0xFFFF
#define FW_TRANSFER_CHUNK_SIZE	(16 * 1024)


#define MAX_NUM_MAIN_DATAPATHS ADI_APOLLO_CNCO_NUM
#define MAX_NUM_CHANNELIZER ADI_APOLLO_FNCO_NUM
#define MAX_NUM_RX_NCO_CHAN_REGS 16
#define MAX_NUM_TX_NCO_CHAN_REGS 31

#define NUM_RXTX 2

#define for_each_cddc(bit, mask) \
	for ((bit) = 0; (bit) < MAX_NUM_MAIN_DATAPATHS; (bit)++) \
		if ((mask) & BIT(bit))

#define for_each_fddc(bit, mask) \
	for ((bit) = 0; (bit) < MAX_NUM_CHANNELIZER; (bit)++) \
		if ((mask) & BIT(bit))

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

struct ad9088_init_param {
	struct no_os_spi_init_param *spi_init; //
	struct no_os_gpio_init_param *gpio_reset; //
	struct no_os_gpio_init_param *gpio_tri_req; //
	struct no_os_gpio_init_param *gpio_rx1_en; //
	struct no_os_gpio_init_param *gpio_rx2_en; //
	struct no_os_gpio_init_param *gpio_tx1_en; //
	struct no_os_gpio_init_param *gpio_tx2_en; //
	struct no_os_gpio_init_param *versal_xvr_reset; //
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
	uint32_t jrx0_logical_lane_mapping[12]; //
	uint32_t jrx1_logical_lane_mapping[12]; //
	uint32_t jtx_ser_amplitude;
	uint32_t jtx_ser_pre_emphasis;
	uint32_t jtx_ser_post_emphasis;
};

struct ad9088_phy {
	struct no_os_spi_desc *spi;
	struct jesd204_dev *jdev;
	const struct firmware *fw;
	adi_apollo_device_t ad9088;
	adi_apollo_top_t profile;
	adi_cms_chip_id_t chip_id;
	// struct axiadc_chip_info chip_info;
	// struct clk *dev_clk;
	// struct bin_attribute pfilt;
	// struct bin_attribute cfir;
	// struct gpio_chip gpiochip;

	struct no_os_gpio_desc *rx1_en_gpio;
	struct no_os_gpio_desc *rx2_en_gpio;
	struct no_os_gpio_desc *tx1_en_gpio;
	struct no_os_gpio_desc *tx2_en_gpio;
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

	struct iio_channel	*iio_adf4030;
	struct iio_channel	*iio_adf4382;

    adi_apollo_sniffer_param_t sniffer_config;
    adi_apollo_sniffer_fft_data_t fft_data;

	// u8 hsci_buf[32767 + 4];
	uint8_t gpios_exported[ADI_APOLLO_NUM_GPIO];
	char dbuf[1024];

	uint8_t loopback_mode[ADI_APOLLO_NUM_SIDES];
	uint8_t lb1_blend[ADI_APOLLO_NUM_SIDES];
};

int ad9088_parse_struct(struct ad9088_phy **device, 
			const struct ad9088_init_param *init_param);
int ad9088_init(struct ad9088_phy **device, 
		const struct ad9088_init_param *init_param);
int ad9088_fft_sniffer_probe(struct ad9088_phy *phy, adi_apollo_side_select_e side_sel);
