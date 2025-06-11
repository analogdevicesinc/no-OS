// SPDX-License-Identifier: GPL-2.0
/*
 * Driver for AD9088 and similar mixed signal front end (MxFE®)
 *
 * Copyright 2022 Analog Devices Inc.
 */

#include "ad9088.h"

//#define DEBUG

#define APOLLO_FW_CPU0_NAME "APOLLO_FW_CPU0_B.bin"
#define APOLLO_FW_CPU1_NAME "APOLLO_FW_CPU1_B.bin"
#define INDIRECT_REG_TEST_ADDR  (0x60366045)
#define ARM_REG_TEST_BASE_ADDR  (0x20000000U)

#define SECR_BOOT_HDR_BIN	"app_signed_encrypted_B/flash_image_0x01030000.bin"
#define CORE_0_TYE_FW_BIN	"app_signed_encrypted_B/flash_image_0x20000000.bin"
#define CORE_1_TYE_FW_BIN	"app_signed_encrypted_B/flash_image_0x02000000.bin"
#define TYE_OPER_FW_BIN		"app_signed_encrypted_B/flash_image_0x21000000.bin"

#define PROD_SECR_BOOT_HDR_BIN	"app_signed_encrypted_prod_B/flash_image_0x01030000.bin"
#define PROD_CORE_0_TYE_FW_BIN	"app_signed_encrypted_prod_B/flash_image_0x20000000.bin"
#define PROD_CORE_1_TYE_FW_BIN	"app_signed_encrypted_prod_B/flash_image_0x02000000.bin"
#define PROD_TYE_OPER_FW_BIN	"app_signed_encrypted_prod_B/flash_image_0x21000000.bin"


#define SECR_BOOT_HDR_MEM_ADDR  (0x01030000U)
#define CORE_0_TYE_FW_MEM_ADDR  (0x20000000U)
#define CORE_1_TYE_FW_MEM_ADDR  (0x02000000U)
#define TYE_OPER_FW_MEM_ADDR    (0x21000000U)

static const uint8_t lanes_all[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23};
//static int ad9088_trig_sync(struct ad9088_phy *phy, int val);

static int ad9088_jesd204_post_setup_stage1(struct jesd204_dev *jdev,
				       enum jesd204_state_op_reason reason);
static int ad9088_jesd204_post_setup_stage2(struct jesd204_dev *jdev,
				       enum jesd204_state_op_reason reason);
static int ad9088_jesd204_post_setup_stage3(struct jesd204_dev *jdev,
				       enum jesd204_state_op_reason reason);


static int ad9088_spi_xfer(void *dev_obj, uint8_t *wbuf, uint8_t *rbuf,
			   uint32_t len)
{
	struct ad9088_phy *phy = dev_obj;
	
	struct no_os_spi_msg t = {
		.tx_buff = wbuf,
		.rx_buff = rbuf,
		.bytes_number = len & 0xFFFF,
	};

	return no_os_spi_transfer(phy->spi, &t, 1);
}

static int ad9088_spi_read(void *dev_obj, const uint8_t tx_data[],
			   uint8_t rx_data[], uint32_t num_tx_rx_bytes,
			   adi_apollo_hal_txn_config_t *txn_config)
{
	struct ad9088_phy *phy = dev_obj;
	
	struct no_os_spi_msg t = {
		.tx_buff = tx_data,
		.rx_buff = rx_data,
		.bytes_number = num_tx_bytes,
	};

	return no_os_spi_transfer(phy->spi, &t, 1);
}

static int ad9088_spi_write(void *dev_obj, const uint8_t tx_data[],
			    uint32_t num_tx_bytes, adi_apollo_hal_txn_config_t *txn_config)
{
	struct ad9088_phy *phy = dev_obj;
	int ret;
	
	struct no_os_spi_msg t = {
		.tx_buff = tx_data,
		.rx_buff = NULL,
		.bytes_number = num_tx_bytes,
	};

	return no_os_spi_transfer(phy->spi, &t, 1);
}

int ad9088_init(struct ad9088_phy **device, 
		const struct ad9088_init_param *init_param)
{
	struct ad9088_phy *phy;
	int ret;

	ret = ad9088_parse_struct(&phy, init_param);

	*device = phy;

	return 0;
}

// void adi_ad9088_hal_add_128(uint64_t ah, uint64_t al, uint64_t bh, uint64_t bl,
// 			    uint64_t *hi, uint64_t *lo)
// {
// 	uint64_t rl = al + bl, rh = ah + bh;
// 	if (rl < al)
// 		rh++;
// 	*lo = rl;
// 	*hi = rh;
// }

// void adi_ad9088_hal_sub_128(uint64_t ah, uint64_t al, uint64_t bh, uint64_t bl,
// 			    uint64_t *hi, uint64_t *lo)
// {
// 	uint64_t rl, rh;
// 	if (bl <= al) {
// 		rl = al - bl;
// 		rh = ah - bh;
// 	} else {
// 		rl = bl - al - 1;
// 		rl = 0xffffffffffffffffull - rl;
// 		ah--;
// 		rh = ah - bh;
// 	}
// 	*lo = rl;
// 	*hi = rh;
// }

// void adi_ad9088_hal_mult_128(uint64_t a, uint64_t b, uint64_t *hi, uint64_t *lo)
// {
// 	uint64_t ah = a >> 32, al = a & 0xffffffff, bh = b >> 32,
// 		 bl = b & 0xffffffff, rh = ah * bh, rl = al * bl, rm1 = ah * bl,
// 		 rm2 = al * bh, rm1h = rm1 >> 32, rm2h = rm2 >> 32,
// 		 rm1l = rm1 & 0xffffffff, rm2l = rm2 & 0xffffffff,
// 		 rmh = rm1h + rm2h, rml = rm1l + rm2l,
// 		 c = ((rl >> 32) + rml) >> 32;
// 	rl = rl + (rml << 32);
// 	rh = rh + rmh + c;
// 	*lo = rl;
// 	*hi = rh;
// }

// void adi_ad9088_hal_lshift_128(uint64_t *hi, uint64_t *lo)
// {
// 	*hi <<= 1;
// 	if (*lo & 0x8000000000000000ull)
// 		*hi |= 1ull;
// 	*lo <<= 1;
// }

// void adi_ad9088_hal_rshift_128(uint64_t *hi, uint64_t *lo)
// {
// 	*lo >>= 1;
// 	if (*hi & 1ull)
// 		*lo |= 0x8000000000000000ull;
// 	*hi >>= 1;
// }

// void adi_ad9088_hal_div_128(uint64_t a_hi, uint64_t a_lo, uint64_t b_hi,
// 			    uint64_t b_lo, uint64_t *hi, uint64_t *lo)
// {
// 	uint64_t remain_lo = a_lo, remain_hi = a_hi, part1_lo = b_lo,
// 		 part1_hi = b_hi;
// 	uint64_t result_lo = 0, result_hi = 0, mask_lo = 1, mask_hi = 0;

// 	while (!(part1_hi & 0x8000000000000000ull)) {
// 		adi_ad9088_hal_lshift_128(&part1_hi, &part1_lo);
// 		adi_ad9088_hal_lshift_128(&mask_hi, &mask_lo);
// 	}

// 	do {
// 		if ((remain_hi > part1_hi) ||
// 		    ((remain_hi == part1_hi) && (remain_lo >= part1_lo))) {
// 			adi_ad9088_hal_sub_128(remain_hi, remain_lo, part1_hi,
// 					       part1_lo, &remain_hi,
// 					       &remain_lo);
// 			adi_ad9088_hal_add_128(result_hi, result_lo, mask_hi,
// 					       mask_lo, &result_hi, &result_lo);
// 		}
// 		adi_ad9088_hal_rshift_128(&part1_hi, &part1_lo);
// 		adi_ad9088_hal_rshift_128(&mask_hi, &mask_lo);
// 	} while ((mask_hi != 0) || (mask_lo != 0));
// 	*lo = result_lo;
// 	*hi = result_hi;
// }

// int32_t adi_ad9088_calc_nco_ftw(adi_apollo_device_t *device,
// 				uint64_t freq, int64_t nco_shift,
// 				uint64_t *ftw)
// {
// 	uint64_t hi, lo;

// 	if (!freq)
// 		return -EINVAL;

// 	nco_shift = clamp_t(int64_t, nco_shift, -(freq >> 1), freq >> 1);

// 	if (nco_shift >= 0) {
// 		adi_ad9088_hal_mult_128(281474976710656ull, nco_shift, &hi,
// 					&lo);
// 		adi_ad9088_hal_add_128(hi, lo, 0, freq >> 1, &hi, &lo);
// 		adi_ad9088_hal_div_128(hi, lo, 0, freq, &hi, ftw);
// 	} else {
// 		adi_ad9088_hal_mult_128(281474976710656ull, -nco_shift, &hi,
// 					&lo);
// 		adi_ad9088_hal_add_128(hi, lo, 0, freq >> 1, &hi, &lo);
// 		adi_ad9088_hal_div_128(hi, lo, 0, freq, &hi, ftw);
// 		*ftw = 281474976710656ull - *ftw;
// 	}

// 	return API_CMS_ERROR_OK;
// }

// int32_t adi_ad9088_calc_nco_freq(adi_apollo_device_t *device,
// 				  uint64_t freq,
// 				  uint64_t ftw, int64_t *nco_shift)
// {
// 	uint64_t hi, lo;
// 	bool neg = false;

// 	if (!freq)
// 		return -EINVAL;

// 	if (ftw > 140737488355328ull) {
// 		ftw = 0x1000000000000 - ftw;
// 		neg = true;
// 	}

// 	adi_ad9088_hal_mult_128(freq, ftw, &hi, &lo);
// 	adi_ad9088_hal_add_128(hi, lo, 0, 281474976710656ull / 2, &hi, &lo);
// 	adi_ad9088_hal_div_128(hi, lo, 0, 281474976710656ull, &hi, nco_shift);

// 	if (neg)
// 		*nco_shift *= -1;

// 	return API_CMS_ERROR_OK;
// }

// int32_t adi_ad9088_calc_nco_ftw32(adi_apollo_device_t *device,
// 				  uint64_t freq, int64_t nco_shift,
// 				  uint64_t *ftw)
// {
// 	uint64_t hi, lo;

// 	if (!freq)
// 		return -EINVAL;

// 	nco_shift = clamp_t(int64_t, nco_shift, -(freq >> 1), freq >> 1);

// 	if (nco_shift >= 0) {
// 		adi_ad9088_hal_mult_128(4294967296ull, nco_shift, &hi, &lo);
// 		adi_ad9088_hal_add_128(hi, lo, 0, freq >> 1, &hi, &lo);
// 		adi_ad9088_hal_div_128(hi, lo, 0, freq, &hi, ftw);
// 	} else {
// 		adi_ad9088_hal_mult_128(4294967296ull, -nco_shift, &hi, &lo);
// 		adi_ad9088_hal_add_128(hi, lo, 0, freq >> 1, &hi, &lo);
// 		adi_ad9088_hal_div_128(hi, lo, 0, freq, &hi, ftw);
// 		*ftw = 4294967296ull - *ftw;
// 	}

// 	return API_CMS_ERROR_OK;
// }

// int32_t adi_ad9088_calc_nco32_freq(adi_apollo_device_t *device,
// 				  uint64_t freq,
// 				  uint64_t ftw, int64_t *nco_shift)
// {
// 	uint64_t hi, lo;
// 	bool neg = false;

// 	if (!freq)
// 		return -EINVAL;

// 	if (ftw > 2147483648ull) {
// 		ftw = 0x100000000 - ftw;
// 		neg = true;
// 	}

// 	adi_ad9088_hal_mult_128(freq, ftw, &hi, &lo);
// 	adi_ad9088_hal_add_128(hi, lo, 0, 4294967296ull / 2, &hi, &lo);
// 	adi_ad9088_hal_div_128(hi, lo, 0, 4294967296ull, &hi, nco_shift);

// 	if (neg)
// 		*nco_shift *= -1;

// 	return API_CMS_ERROR_OK;
// }


// static u16 ad9088_jrx_serdes_en_mask(struct ad9088_phy *phy)
// {
// 	return ((((phy->profile.jrx[0].rx_link_cfg[0].link_in_use) || (phy->profile.jrx[0].rx_link_cfg[1].link_in_use))
// 		? ADI_APOLLO_TXRX_SERDES_12PACK_A : ADI_APOLLO_TXRX_SERDES_12PACK_NONE)) |
// 		((((phy->profile.jrx[1].rx_link_cfg[0].link_in_use) || (phy->profile.jrx[1].rx_link_cfg[1].link_in_use))
// 		? ADI_APOLLO_TXRX_SERDES_12PACK_B : ADI_APOLLO_TXRX_SERDES_12PACK_NONE));
// }

// static const char *const ad9088_fsm_links_to_str[] = {
// 	[DEFRAMER_LINK_A0_TX] = "JESD TX (JRX Deframer Link A0)",
// 	[DEFRAMER_LINK_A1_TX] = "JESD TX (JRX Deframer Link A1)",
// 	[DEFRAMER_LINK_B0_TX] = "JESD TX (JRX Deframer Link B0)",
// 	[DEFRAMER_LINK_B1_TX] = "JESD TX (JRX Deframer Link B1)",
// 	[FRAMER_LINK_A0_RX] = "JESD RX (JTX Framer Link A0)",
// 	[FRAMER_LINK_A1_RX] = "JESD RX (JTX Framer Link A1)",
// 	[FRAMER_LINK_B0_RX] = "JESD RX (JTX Framer Link B0)",
// 	[FRAMER_LINK_B1_RX] = "JESD RX (JTX Framer Link B1)",
// };

// static u8 ad9088_to_link(u8 linkid)
// {
// 	u8 lut[8] = {
// 		ADI_APOLLO_LINK_A0, ADI_APOLLO_LINK_A1, /* DEFRAMER */
// 		ADI_APOLLO_LINK_B0, ADI_APOLLO_LINK_B1,
// 		ADI_APOLLO_LINK_A0, ADI_APOLLO_LINK_A1, /* FRAMER */
// 		ADI_APOLLO_LINK_B0, ADI_APOLLO_LINK_B1
// 	};

// 	return lut[linkid];
// }

// int32_t ad9088_log_write(void *user_data, int32_t log_type, const char *message,
// 			 va_list argp)
// {
// 	struct axiadc_converter *conv = user_data;
// 	char logMessage[160];

// 	if (log_type == ADI_CMS_LOG_SPI)
// 		return 0;

// 	vsnprintf(logMessage, sizeof(logMessage), message, argp);

// 	switch (log_type) {
// 	case ADI_CMS_LOG_NONE:
// 		break;
// 	case ADI_CMS_LOG_MSG:
// 		dev_info(&conv->spi->dev, "%s", logMessage);
// 		break;
// 	case ADI_CMS_LOG_WARN:
// 		dev_warn(&conv->spi->dev, "%s", logMessage);
// 		break;
// 	case ADI_CMS_LOG_ERR:
// 		dev_err(&conv->spi->dev, "%s", logMessage);
// 		break;
// 	case ADI_CMS_LOG_SPI:
// 		dev_dbg(&conv->spi->dev, "%s", logMessage);
// 		break;
// 	case ADI_CMS_LOG_API:
// 		dev_dbg(&conv->spi->dev, "%s", logMessage);
// 		break;
// 	case ADI_CMS_LOG_ALL:
// 		printk(logMessage);
// 		break;
// 	}

// 	return 0;
// }

// static int ad9088_udelay(void *user_data, unsigned int us)
// {
// 	//us = us * 2;
// 	usleep_range(us, (us * 110) / 100);
// 	return 0;
// }

// static int ad9088_spi_xfer(void *dev_obj, uint8_t *wbuf, uint8_t *rbuf,
// 			   uint32_t len)
// {
// 	struct axiadc_converter *conv = dev_obj;
// 	//struct ad9088_phy *phy = conv->phy;
// 	int ret;

// 	struct spi_transfer t = {
// 		.tx_buf = wbuf,
// 		.rx_buf = rbuf,
// 		.len = len & 0xFFFF,
// 	};
// 	//pr_err("%s:%d\n",__func__,__LINE__);

// 	ret = spi_sync_transfer(conv->spi, &t, 1);

// 	return ret;
// }

// static int ad9088_spi_read(void *dev_obj, const uint8_t tx_data[],
// 			   uint8_t rx_data[], uint32_t num_tx_rx_bytes,
// 			   adi_apollo_hal_txn_config_t *txn_config)
// {
// 	struct axiadc_converter *conv = dev_obj;
// 	//struct ad9088_phy *phy = conv->phy;
// 	int ret;

// 	struct spi_transfer t = {
// 		.tx_buf = tx_data,
// 		.rx_buf = rx_data,
// 		.len = num_tx_rx_bytes,
// 	};
// 	//pr_err("%s:%d\n",__func__,__LINE__);

// 	ret = spi_sync_transfer(conv->spi, &t, 1);

// 	return ret;
// }


// static int ad9088_spi_write(void *dev_obj, const uint8_t tx_data[],
// 			    uint32_t num_tx_bytes, adi_apollo_hal_txn_config_t *txn_config)
// {

// 	struct axiadc_converter *conv = dev_obj;
// 	int ret;

// 	struct spi_transfer t = {
// 		.tx_buf = tx_data,
// 		.rx_buf = NULL,
// 		.len = num_tx_bytes,
// 	};
// 	//pr_err("%s:%d\n",__func__,__LINE__);

// 	ret = spi_sync_transfer(conv->spi, &t, 1);

// 	return ret;
// }

// static int ad9088_reset_pin_ctrl(void *user_data, u8 enable)
// {
// 	struct axiadc_converter *conv = user_data;

// 	return gpiod_direction_output(conv->reset_gpio, enable);
// }

// // static int ad9088_sysref_ctrl(struct ad9088_phy *phy, u8 enable)
// // {
// //	if (phy->jdev && enable)
// //		return jesd204_sysref_async_force(phy->jdev);

// //	return 0;
// // }

// static void ad9088_print_link_phase(struct ad9088_phy *phy,
// 	struct jesd204_link *lnk)
// {
// 	adi_apollo_device_t *device = &phy->ad9088;
// 	u8 id = ad9088_to_link(lnk->link_id);
// 	u16 jrx_phase_diff;

// 	adi_apollo_jrx_phase_diff_get(device, id, &jrx_phase_diff);
// 	dev_info(&phy->spi->dev, "%s Phase Difference %d\n",
// 		ad9088_fsm_links_to_str[lnk->link_id], jrx_phase_diff);
// }

// static void ad9088_print_sysref_phase(struct ad9088_phy *phy) {
// 	adi_apollo_device_t *device = &phy->ad9088;
// 	u32 sysref_phase;
// 	int i;

// 	for (i = 0; i < 5; i++) {
// 		adi_apollo_clk_mcs_sysref_phase_get(device, &sysref_phase);
// 		dev_info(&phy->spi->dev, "SYSREF_PHASE = %d (TRY%d)\n", sign_extend32(sysref_phase, 9), i);
// 	}
// }

// static int ad9088_reg_access(struct iio_dev *indio_dev, unsigned int reg,
// 			     unsigned int writeval, unsigned int *readval)
// {
// 	struct axiadc_converter *conv = iio_device_get_drvdata(indio_dev);
// 	struct ad9088_phy *phy = conv->phy;
// 	u8 val;
// 	int ret;

// 	guard(mutex)(&phy->lock);

// 	if (readval == NULL)
// 		return adi_apollo_hal_reg_set(&phy->ad9088, reg, writeval);

// 	ret = adi_apollo_hal_reg_get(&phy->ad9088, reg, &val);
// 	if (ret < 0)
// 		return ret;

// 	*readval = val;

// 	return 0;
// }

// static void ad9088_iiochan_to_fddc_cddc(struct ad9088_phy *phy,
// 					const struct iio_chan_spec *chan, u8 *fddc_num, u32 *fddc_mask,
// 					u8 *cddc_num, u32 *cddc_mask, u8 *side)
// {
// 	const u32 cncos[] = {ADI_APOLLO_CNCO_A0, ADI_APOLLO_CNCO_A1,
// 			     ADI_APOLLO_CNCO_B0, ADI_APOLLO_CNCO_B1
// 			    };
// 	const u32 rx_fncos[] = {ADI_APOLLO_FNCO_A0, ADI_APOLLO_FNCO_A2,
// 				ADI_APOLLO_FNCO_B0, ADI_APOLLO_FNCO_B2
// 			       };

// 	const u32 tx_fncos[] = {ADI_APOLLO_FNCO_A0, ADI_APOLLO_FNCO_A1,
// 				ADI_APOLLO_FNCO_B0, ADI_APOLLO_FNCO_B1
// 			       };

// 	if (chan->channel >= ARRAY_SIZE(cncos)) {
// 		dev_err(&phy->spi->dev,
// 			"ERROR: %s_voltage%d: >= 4",
// 			chan->output ? "out" : "in", chan->channel);
// 		return;
// 	}

// 	*fddc_mask = chan->output ? tx_fncos[chan->channel] : rx_fncos[chan->channel];
// 	*cddc_mask = cncos[chan->channel];

// 	if (*cddc_mask > ADI_APOLLO_CNCO_A3)
// 			*cddc_num = ilog2(*cddc_mask >> 4);
// 	else
// 			*cddc_num = ilog2(*cddc_mask);

// 	if (*fddc_mask > ADI_APOLLO_FNCO_A7)
// 			*fddc_num = ilog2(*fddc_mask >> 8);
// 	else
// 			*fddc_num = ilog2(*fddc_mask);

// 	*side = chan->channel > 1 ? 1 : 0;

// 	dev_dbg(&phy->spi->dev,
// 		"%s_voltage%d: Side-%c fddc_num=%d fddc_mask=%X cddc_num=%d cddc_mask=%X",
// 		chan->output ? "out" : "in", chan->channel, *side ? 'B' : 'A',
// 		*fddc_num, *fddc_mask, *cddc_num, *cddc_mask);
// }

// static void ad9088_iiochan_to_cfir(struct ad9088_phy *phy,
// 					const struct iio_chan_spec *chan, adi_apollo_terminal_e *terminal,
// 					adi_apollo_cfir_sel_e *cfir_sel,
// 					adi_apollo_cfir_dp_sel *dp_sel)
// {

// 	u8 cddc_num, fddc_num, side;
// 	u32 cddc_mask, fddc_mask;

// 	ad9088_iiochan_to_fddc_cddc(phy, chan, &fddc_num,
// 				    &fddc_mask, &cddc_num, &cddc_mask, &side);

// 	if (chan->output) {
// 		*terminal = ADI_APOLLO_TX;
// 	} else {
// 		*terminal = ADI_APOLLO_RX;
// 	}

// 	switch (fddc_mask) {
// 	case ADI_APOLLO_FDDC_A0:
// 		*cfir_sel = ADI_APOLLO_CFIR_A0;
// 		*dp_sel = ADI_APOLLO_CFIR_DP_0;
// 		break;
//     case ADI_APOLLO_FDDC_A1:
// 		*cfir_sel = ADI_APOLLO_CFIR_A0;
// 		*dp_sel = ADI_APOLLO_CFIR_DP_1;
// 		break;
// 	case ADI_APOLLO_FDDC_A2:
// 		*cfir_sel = ADI_APOLLO_CFIR_A1;
// 		*dp_sel = ADI_APOLLO_CFIR_DP_0;
// 		break;
//     case ADI_APOLLO_FDDC_A3:
// 		*cfir_sel = ADI_APOLLO_CFIR_A1;
// 		*dp_sel = ADI_APOLLO_CFIR_DP_1;
// 		break;
// 	case ADI_APOLLO_FDDC_B0:
// 		*cfir_sel = ADI_APOLLO_CFIR_B0;
// 		*dp_sel = ADI_APOLLO_CFIR_DP_0;
// 		break;
//     case ADI_APOLLO_FDDC_B1:
// 		*cfir_sel = ADI_APOLLO_CFIR_B0;
// 		*dp_sel = ADI_APOLLO_CFIR_DP_1;
// 		break;
// 	case ADI_APOLLO_FDDC_B2:
// 		*cfir_sel = ADI_APOLLO_CFIR_B1;
// 		*dp_sel = ADI_APOLLO_CFIR_DP_0;
// 		break;
//     case ADI_APOLLO_FDDC_B3:
// 		*cfir_sel = ADI_APOLLO_CFIR_B1;
// 		*dp_sel = ADI_APOLLO_CFIR_DP_1;
// 		break;
// 	default:
// 		dev_err(&phy->spi->dev, "Unhandled FDDC number 0x%X\n", fddc_mask);
// 	}

// 	dev_dbg(&phy->spi->dev,
// 		"%s_voltage%d: Side-%c fddc_mask=%X terminal=%s, cfir_sel=%u dp_sel=%u\n",
// 		chan->output ? "out" : "in", chan->channel, side ? 'B' : 'A',
// 		fddc_mask, *terminal ? "TX" : "RX", *cfir_sel, *dp_sel);
// }

// #define AD9088_MAX_CLK_NAME 79

// static char *ad9088_clk_set_dev_name(struct ad9088_phy *phy, char *dest,
// 				     const char *name)
// {
// 	size_t len = 0;

// 	if (name == NULL)
// 		return NULL;

// 	if (*name == '-')
// 		len = strlcpy(dest, dev_name(&phy->spi->dev),
// 			      AD9088_MAX_CLK_NAME);
// 	else
// 		*dest = '\0';

// 	return strncat(dest, name, AD9088_MAX_CLK_NAME - len);
// }

// static unsigned long ad9088_bb_recalc_rate(struct clk_hw *hw,
// 		unsigned long parent_rate)
// {
// 	struct ad9088_clock *clk_priv = to_clk_priv(hw);

// 	return clk_priv->rate;
// }

// static int ad9088_bb_set_rate(struct clk_hw *hw, unsigned long rate,
// 			      unsigned long parent_rate)
// {
// 	struct ad9088_clock *clk_priv = to_clk_priv(hw);

// 	clk_priv->rate = rate;

// 	return 0;
// }

// static long ad9088_bb_round_rate(struct clk_hw *hw, unsigned long rate,
// 				 unsigned long *prate)
// {
// 	struct ad9088_clock *clk_priv = to_clk_priv(hw);

// 	dev_dbg(&clk_priv->spi->dev, "%s: Rate %lu Hz", __func__, rate);

// 	return rate;
// }

// static int ad9088_bb_determine_rate(struct clk_hw *hw,
// 				    struct clk_rate_request *req)
// {
// 	return 0;
// }

// static const struct clk_ops bb_clk_ops = {
// 	.round_rate = ad9088_bb_round_rate,
// 	.determine_rate = ad9088_bb_determine_rate,
// 	.set_rate = ad9088_bb_set_rate,
// 	.recalc_rate = ad9088_bb_recalc_rate,
// };

// static int ad9088_clk_register(struct ad9088_phy *phy, const char *name,
// 			       const char *parent_name,
// 			       const char *parent_name2, unsigned long flags,
// 			       u32 source)
// {
// 	struct ad9088_clock *clk_priv = &phy->clk_priv[source];
// 	struct clk_init_data init;
// 	struct clk *clk;
// 	char c_name[AD9088_MAX_CLK_NAME + 1],
// 	     p_name[2][AD9088_MAX_CLK_NAME + 1];
// 	const char *_parent_name[2];

// 	/* struct ad9088_clock assignments */
// 	clk_priv->source = source;
// 	clk_priv->hw.init = &init;
// 	clk_priv->spi = phy->spi;
// 	clk_priv->phy = phy;

// 	_parent_name[0] = ad9088_clk_set_dev_name(phy, p_name[0], parent_name);
// 	_parent_name[1] = ad9088_clk_set_dev_name(phy, p_name[1], parent_name2);

// 	init.name = ad9088_clk_set_dev_name(phy, c_name, name);
// 	init.flags = flags;
// 	init.parent_names = &_parent_name[0];
// 	init.num_parents = _parent_name[1] ? 2 : _parent_name[0] ? 1 : 0;

// 	switch (source) {
// 	case RX_SAMPL_CLK:
// 	case RX_SAMPL_CLK_LINK2:
// 		init.ops = &bb_clk_ops;
// 		break;
// 	case TX_SAMPL_CLK:
// 		init.ops = &bb_clk_ops;
// 		break;
// 	default:
// 		return -EINVAL;
// 	}

// 	of_clk_get_scale(phy->spi->dev.of_node, &name[1],
// 			 &phy->clkscale[source]);

// 	clk = devm_clk_register(&phy->spi->dev, &clk_priv->hw);
// 	phy->clks[source] = clk;

// 	return 0;
// }


// static irqreturn_t ad9088_event_handler(struct axiadc_converter *conv,
// 					unsigned int chn)
// {
// 	u64 event = IIO_UNMOD_EVENT_CODE(IIO_VOLTAGE, chn,
// 					 IIO_EV_TYPE_THRESH, IIO_EV_DIR_RISING);
// 	s64 timestamp = iio_get_time_ns(conv->indio_dev);

// 	if (conv->indio_dev)
// 		iio_push_event(conv->indio_dev, event, timestamp);

// 	return IRQ_HANDLED;
// }

// static irqreturn_t ad9088_fdA_handler(int irq, void *private)
// {
// 	return ad9088_event_handler(private, 0);
// }

// static irqreturn_t ad9088_fdB_handler(int irq, void *private)
// {
// 	return ad9088_event_handler(private, 1);
// }


// static char *ad9088_test_mode_names[4] = {"Normal Magnitude", "Instant Magnitude", "Normal IQ", "Instant IQ"};

// static int ad9088_rx_sniffer_populate_default_params(adi_apollo_sniffer_mode_e mode, adi_apollo_sniffer_param_t *config)
// {

// 	config->init.fft_hold_sel = 1;                 // 0 - gpio 1 - regmap
// 	config->init.fft_enable_sel = 1;               // 0 - gpio 1 - regmap
// 	config->init.real_mode = 1;                    // 1 real 0 complex
// 	config->init.max_threshold = 20;                // Max threshold for max
// 	config->init.min_threshold = 0;                // Min threshold for min
// 	config->init.sniffer_enable = 1;               // Enable spec sniffer

// 	config->pgm.sniffer_mode = mode;               // see \ref adi_apollo_sniffer_mode_e
// 	config->pgm.sort_enable = 1;                   // 1 enable 0 disable
// 	config->pgm.continuous_mode = 0;               // 1 continuous 0 single
// 	config->pgm.bottom_fft_enable = 0;             // 1 enable 0 disable
// 	config->pgm.window_enable = 0;                 // 1 enable 0 disable
// 	config->pgm.low_power_enable = 0;              // 1 enable 0 disable
// 	config->pgm.dither_enable = 0;                 // 1 enable 0 disable
// 	config->pgm.alpha_factor = 0;                  // exp. avg. 0 - disable 1-8 - enable
// 	config->pgm.adc = ADI_APOLLO_ADC_0;

// 	config->read.run_fft_engine_background = 0;
// 	config->read.timeout_us = 1000000;

// 	if (mode > ADI_APOLLO_SNIFFER_INSTANT_MAGNITUDE) { // IQ mode necessities
// 	    config->init.real_mode = 0;

// 	    config->pgm.sort_enable = 0;
// 	    config->pgm.continuous_mode = 0;
// 	    config->pgm.alpha_factor = 0;
// 	}

// 	return API_CMS_ERROR_OK;
// }

// static int ad9088_rx_sniffer_setup(struct ad9088_phy *phy, adi_apollo_side_select_e side_sel, adi_apollo_sniffer_init_t *config)
// {
// 	int err;

// 	/* Init sniffer */
// 	if (err = adi_apollo_sniffer_init(&phy->ad9088, side_sel, config), err != API_CMS_ERROR_OK) {
// 		dev_err(&phy->spi->dev, "Error in adi_apollo_sniffer_pgm: %d\n", err);
// 		return err;
// 	}

// 	return API_CMS_ERROR_OK;

// }

// static int ad9088_rx_sniffer_iterate(struct ad9088_phy *phy, adi_apollo_side_select_e side_sel, adi_apollo_sniffer_param_t *config, adi_apollo_sniffer_fft_data_t *fft_data)
// {
// 	int err;

// 	/* Program the sniffer */
// 	if (err = adi_apollo_sniffer_pgm(&phy->ad9088, side_sel, &config->pgm), err != API_CMS_ERROR_OK) {
// 		dev_err(&phy->spi->dev, "Error in adi_apollo_sniffer_pgm: %d\n", err);
// 		return err;
// 	}

// 	/* Get fft data*/
// 	if (err = adi_apollo_sniffer_data_get(&phy->ad9088, side_sel, config, fft_data), err != API_CMS_ERROR_OK) {
// 		dev_err(&phy->spi->dev, "Error in adi_apollo_sniffer_pgm: %d\n", err);
// 		//return err;
// 	}

// 	return API_CMS_ERROR_OK;
// }

// static int ad9088_rx_sniffer_cleanup(struct ad9088_phy *phy, adi_apollo_side_select_e side_sel, adi_apollo_sniffer_param_t *config)
// {
// 	int err;
// 	/* Disable sniffer */
// 	/* Get fft data*/
// 	if (err = adi_apollo_sniffer_enable_set(&phy->ad9088, side_sel, 0), err != API_CMS_ERROR_OK) {
// 		dev_err(&phy->spi->dev, "Error in adi_apollo_sniffer_pgm: %d\n", err);
// 		return err;
// 	}

// 	return err;
// }

// static int ad9088_rx_sniffer_print_fft_data(struct ad9088_phy *phy, adi_apollo_sniffer_fft_data_t *data)
// {
// 	int i;

// 	dev_info(&phy->spi->dev, "FFT: %d points in from sniffer mode 0x%x\n", data->valid_data_length, data->data_mode);

// 	switch (data->data_mode) {

// 		/* Magnitude mode */
// 		case ADI_APOLLO_SNIFFER_INSTANT_MAGNITUDE:
// 		case ADI_APOLLO_SNIFFER_NORMAL_MAGNITUDE:

// 		for (i = 0; i < data->valid_data_length; i++){
// 			dev_info(&phy->spi->dev, "\tBin %3d; Mag: %3d; %s%s\n", data->bin_q_data[i], data->mag_i_data[i],
// 				 data->bin_below_threshold[i] ? "below threshold" : "", data->bin_above_threshold[i] ? "above threshold" : "");
// 		}
// 		break;

// 		/* IQ mode*/
// 		case ADI_APOLLO_SNIFFER_INSTANT_IQ:
// 		case ADI_APOLLO_SNIFFER_NORMAL_IQ:
// 		for (i = 0; i < data->valid_data_length; i++) {
// 			dev_info(&phy->spi->dev, "\tBin %3d; I: %3d, Q: %3d;\n", i, data->mag_i_data[i], data->bin_q_data[i]);
// 		}
// 		break;

// 		/* Anything else */
// 		default:
// 		return API_CMS_ERROR_INVALID_PARAM;
// 	}

// 	dev_info(&phy->spi->dev, "%s %s FFT\n", ad9088_test_mode_names[data->data_mode], data->fft_is_complex ? "Complex" : "Real");

// 	return API_CMS_ERROR_OK;
// }


// int ad9088_rx_sniffer(struct ad9088_phy *phy, adi_apollo_top_t *profile, int val)
// {
// 	int err = API_CMS_ERROR_OK;

// 	adi_apollo_sniffer_param_t *sniffer_config = &phy->sniffer_config;
// 	adi_apollo_sniffer_fft_data_t *fft_data = &phy->fft_data;
// 	adi_apollo_side_select_e side = ADI_APOLLO_SIDE_A;

// 	// Test each of the 4 modes
// 	adi_apollo_sniffer_mode_e test_modes[4] = {ADI_APOLLO_SNIFFER_INSTANT_MAGNITUDE,
// 						   ADI_APOLLO_SNIFFER_NORMAL_MAGNITUDE,
// 						   ADI_APOLLO_SNIFFER_INSTANT_IQ,
// 						   ADI_APOLLO_SNIFFER_NORMAL_IQ};

// 	if (val < 0 || val > ARRAY_SIZE(test_modes)) {
// 		return -EINVAL;
// 	}
// 	//adi_apollo_gpio_quick_config_mode_set(&phy->ad9088, 0);
// 	//adi_apollo_gpio_cmos_func_mode_set(&phy->ad9088, 17, ADI_APOLLO_FUNC_FFT_DONE_A);
// 	//adi_apollo_gpio_cmos_func_mode_set(&phy->ad9088, 18, ADI_APOLLO_FUNC_FFT_DONE_B);
// 	/* while [ 1 ];do echo 0 > /sys/kernel/debug/iio/iio:device5/rx_sniffer_test;done */

// 	// Get default parameters for initialization
// 	ad9088_rx_sniffer_populate_default_params(test_modes[0], sniffer_config);

// 	// Call setup
// 	if (err = ad9088_rx_sniffer_setup(phy, side, &sniffer_config->init), err != API_CMS_ERROR_OK) {
// 		dev_err(&phy->spi->dev, "Error in ad9088_rx_sniffer_setup: %d\n", err);
// 		goto end;
// 	}

// 	dev_info(&phy->spi->dev, "\nTest mode %s [%d]\n",
// 		ad9088_test_mode_names[test_modes[val]], test_modes[val]);
// 	// Populate config per test mode
// 	ad9088_rx_sniffer_populate_default_params(test_modes[val], sniffer_config);

// 	// Run the iterater
// 	if (err = ad9088_rx_sniffer_iterate(phy, side, sniffer_config, fft_data), err != API_CMS_ERROR_OK) {
// 		dev_err(&phy->spi->dev, "Error in ad9088_rx_sniffer_iterate: %d\n", err);
// 		goto end;
// 	}

// 	// Print data to user and prompt input
// 	ad9088_rx_sniffer_print_fft_data(phy, fft_data);
// end:
// 	// Cleanup
// 	if (err = ad9088_rx_sniffer_cleanup(phy, side, sniffer_config), err != API_CMS_ERROR_OK) {
// 		dev_err(&phy->spi->dev, "Error in ad9088_rx_sniffer_cleanup: %d\n", err);
// 		return err;
// 	}

// 	return err;
// }

// static int ad9088_testmode_read(struct iio_dev *indio_dev,
// 				const struct iio_chan_spec *chan)
// {
// 	struct axiadc_converter *conv = iio_device_get_drvdata(indio_dev);

// 	return conv->testmode[chan->channel];
// }

// static int ad9088_testmode_write(struct iio_dev *indio_dev,
// 				 const struct iio_chan_spec *chan,
// 				 unsigned int item)
// {
// 	struct axiadc_converter *conv = iio_device_get_drvdata(indio_dev);
// 	struct ad9088_phy *phy = conv->phy;
// 	u8 cddc_num, fddc_num, side;
// 	u32 cddc_mask, fddc_mask;
// 	int ret = 0;

// 	ad9088_iiochan_to_fddc_cddc(phy, chan, &fddc_num,
// 				    &fddc_mask, &cddc_num, &cddc_mask, &side);

// 	guard(mutex)(&phy->lock);
// 	ret = adi_apollo_tmode_config_set(&phy->ad9088,
// 					  side ? ADI_APOLLO_LINK_B0 : ADI_APOLLO_LINK_A0,
// 					  item ? 0xFF : 0, item,
// 					  phy->profile.rx_path[side].rx_dformat[0].tmode.res);

// 	if (!ret)
// 		conv->testmode[chan->channel] = item;

// 	return ret;
// }

// static const char *const ad9088_adc_testmodes[] = {
// 	[ADI_APOLLO_TMODE_TYPE_SEL_NORM] = "off",
// 	[ADI_APOLLO_TMODE_TYPE_SEL_MIDSCALE] = "midscale_short",
// 	[ADI_APOLLO_TMODE_TYPE_SEL_POS_FS] = "pos_fullscale",
// 	[ADI_APOLLO_TMODE_TYPE_SEL_NEG_FS] = "neg_fullscale",
// 	[ADI_APOLLO_TMODE_TYPE_SEL_ACB] = "checkerboard",
// 	[ADI_APOLLO_TMODE_TYPE_SEL_PN9] = "pn9",
// 	[ADI_APOLLO_TMODE_TYPE_SEL_PN23] = "pn23",
// 	[ADI_APOLLO_TMODE_TYPE_SEL_WT] = "one_zero_toggle",
// 	[ADI_APOLLO_TMODE_TYPE_SEL_USR] = "user",
// 	[ADI_APOLLO_TMODE_TYPE_SEL_PN7] = "pn7",
// 	[ADI_APOLLO_TMODE_TYPE_SEL_PN15] = "pn15",
// 	[ADI_APOLLO_TMODE_TYPE_SEL_PN31] = "pn31",
// 	[ADI_APOLLO_TMODE_TYPE_SEL_RAMP] = "ramp",
// 	[12] = "",
// 	[13] = "",
// 	[14] = "",
// };

// static const char *const ad9088_jesd_testmodes[] = {
// 	// [AD9088_JESD_TX_TEST_MODE_DISABLED] = "off",
// 	// [AD9088_JESD_TX_TEST_MODE_CHECKER_BOARD] = "checkerboard",
// 	// [AD9088_JESD_TX_TEST_MODE_WORD_TOGGLE] = "word_toggle",
// 	// [AD9088_JESD_TX_TEST_MODE_PN31] = "pn31",
// 	// [AD9088_JESD_TX_TEST_MODE_PN15] = "pn15",
// 	// [AD9088_JESD_TX_TEST_MODE_PN7] = "pn7",
// 	// [AD9088_JESD_TX_TEST_MODE_RAMP] = "ramp",
// 	// [AD9088_JESD_TX_TEST_MODE_USER_REPEAT] = "user_repeat",
// 	// [AD9088_JESD_TX_TEST_MODE_USER_SINGLE] = "user_single",
// };

// static const struct iio_enum ad9088_testmode_enum = {
// 	.items = ad9088_adc_testmodes,
// 	.num_items = ARRAY_SIZE(ad9088_adc_testmodes),
// 	.set = ad9088_testmode_write,
// 	.get = ad9088_testmode_read,
// };

// int ad9088_iio_val_to_str(char *buf, u32 max, int val)
// {
// 	int vals[2];

// 	vals[0] = val;
// 	vals[1] = max;

// 	return iio_format_value(buf, IIO_VAL_FRACTIONAL, 2, vals);
// }

// int ad9088_iio_str_to_val(const char *str, int min, int max, int *val)
// {
// 	int ret, integer, fract;

// 	ret = iio_str_to_fixpoint(str, 100000, &integer, &fract);

// 	*val = DIV_ROUND_CLOSEST(
// 		       max * (integer * 1000 + DIV_ROUND_CLOSEST(fract, 1000)), 1000);

// 	*val = clamp(*val, min, max);

// 	return ret;
// }

// static int ad9088_nyquist_zone_read(struct iio_dev *indio_dev,
// 				    const struct iio_chan_spec *chan)
// {
// 	struct axiadc_converter *conv = iio_device_get_drvdata(indio_dev);
// 	struct ad9088_phy *phy = conv->phy;
// 	u8 cddc_num, fddc_num, side;
// 	u32 cddc_mask, fddc_mask;

// 	ad9088_iiochan_to_fddc_cddc(phy, chan, &fddc_num,
// 				    &fddc_mask, &cddc_num, &cddc_mask, &side);

// 	return phy->rx_nyquist_zone[side][cddc_num];
// }

// static int ad9088_nyquist_zone_write(struct iio_dev *indio_dev,
// 				     const struct iio_chan_spec *chan,
// 				     unsigned int item)
// {
// 	struct axiadc_converter *conv = iio_device_get_drvdata(indio_dev);
// 	struct ad9088_phy *phy = conv->phy;
// 	u8 cddc_num, fddc_num, side;
// 	u32 cddc_mask, fddc_mask;
// 	int ret;

// 	ad9088_iiochan_to_fddc_cddc(phy, chan, &fddc_num,
// 				    &fddc_mask, &cddc_num, &cddc_mask, &side);

// 	guard(mutex)(&phy->lock);

// 	ret = adi_apollo_adc_nyquist_zone_set(&phy->ad9088, cddc_mask, item + 1);
// 	if (!ret)
// 		phy->rx_nyquist_zone[side][cddc_num] = item;

// 	return ret;
// }

// static const char *const ad9088_adc_nyquist_zones[] = {
// 	[0] = "odd",
// 	[1] = "even",
// };

// static const struct iio_enum ad9088_nyquist_zone_enum = {
// 	.items = ad9088_adc_nyquist_zones,
// 	.num_items = ARRAY_SIZE(ad9088_adc_nyquist_zones),
// 	.set = ad9088_nyquist_zone_write,
// 	.get = ad9088_nyquist_zone_read,
// };

// static ssize_t ad9088_ext_info_read(struct iio_dev *indio_dev,
// 				    uintptr_t private,
// 				    const struct iio_chan_spec *chan, char *buf)
// {
// 	struct axiadc_converter *conv = iio_device_get_drvdata(indio_dev);
// 	struct ad9088_phy *phy = conv->phy;
// 	long long val;
// 	u64 range, f;
// 	u8 cddc_num, fddc_num, side, enable;
// 	u32 cddc_mask, fddc_mask;
// 	int /*i,*/ ret = -EINVAL;
// 	u32 cddc_dcm;
// 	adi_apollo_invsinc_inspect_t invsinc_inspect;
// 	adi_apollo_terminal_e terminal;
// 	adi_apollo_cfir_sel_e cfir_sel;
// 	adi_apollo_cfir_dp_sel dp_sel;

// 	guard(mutex)(&phy->lock);

// 	ad9088_iiochan_to_fddc_cddc(phy, chan, &fddc_num,
// 				    &fddc_mask, &cddc_num, &cddc_mask, &side);

// 	switch (private) {
// 	case CDDC_NCO_FREQ:

// 		if (chan->output)
// 			adi_ad9088_calc_nco32_freq(&phy->ad9088, phy->profile.dac_config[side].dac_sampling_rate_Hz, phy->profile.tx_path[side].tx_cduc[cddc_num].nco[0].nco_phase_inc, &val);
// 		else
// 			adi_ad9088_calc_nco32_freq(&phy->ad9088, phy->profile.adc_config[side].adc_sampling_rate_Hz, phy->profile.rx_path[side].rx_cddc[cddc_num].nco[0].nco_phase_inc, &val);

// 		//val = phy->cnco_freq[chan->output][side][cddc_num];
// 		ret = 0;
// 		break;
// 	case FDDC_NCO_FREQ:

// 		if (chan->output) {
// 			u32 cddc_dcm;

// 			adi_apollo_cduc_interp_bf_to_val(&phy->ad9088, phy->profile.tx_path[side].tx_cduc[cddc_num].drc_ratio, &cddc_dcm);
// 			f = phy->profile.dac_config[side].dac_sampling_rate_Hz;
// 			do_div(f, cddc_dcm);
// 			adi_ad9088_calc_nco_freq(&phy->ad9088, f, phy->profile.tx_path[side].tx_fduc[fddc_num].nco[0].nco_phase_inc, &val);
// 		} else {
// 			u32 cddc_dcm;

// 			adi_apollo_cddc_dcm_bf_to_val(&phy->ad9088, phy->profile.rx_path[side].rx_cddc[cddc_num].drc_ratio, &cddc_dcm);
// 			f = phy->profile.adc_config[side].adc_sampling_rate_Hz;
// 			do_div(f, cddc_dcm);
// 			adi_ad9088_calc_nco_freq(&phy->ad9088, f, phy->profile.rx_path[side].rx_fddc[fddc_num].nco[0].nco_phase_inc, &val);
// 		}
// 		//val = phy->fnco_freq[chan->output][side][fddc_num];
// 		ret = 0;
// 		break;
// 	case CDDC_NCO_FREQ_AVAIL:
// 		if (chan->output)
// 			range = DIV_ROUND_CLOSEST_ULL(phy->profile.dac_config[side].dac_sampling_rate_Hz, 2);
// 		else
// 			range = DIV_ROUND_CLOSEST_ULL(phy->profile.adc_config[side].adc_sampling_rate_Hz, 2);

// 		return sprintf(buf, "[%lld 1 %lld]\n", -1 * range, range);
// 	case FDDC_NCO_FREQ_AVAIL:
// 		if (chan->output) {
// 			adi_apollo_cduc_interp_bf_to_val(&phy->ad9088, phy->profile.tx_path[side].tx_cduc[cddc_num].drc_ratio, &cddc_dcm);
// 			f = phy->profile.dac_config[side].dac_sampling_rate_Hz;

// 		} else {
// 			adi_apollo_cddc_dcm_bf_to_val(&phy->ad9088, phy->profile.rx_path[side].rx_cddc[cddc_num].drc_ratio, &cddc_dcm);
// 			f = phy->profile.adc_config[side].adc_sampling_rate_Hz;
// 		}

// 		range = DIV_ROUND_CLOSEST_ULL(f, cddc_dcm * 2);
// 		return sprintf(buf, "[%lld 1 %lld]\n", -1 * range, range);
// 	case CDDC_NCO_PHASE:
// 		val = phy->cnco_phase[chan->output][side][cddc_num];
// 		ret = 0;
// 		break;
// 	case FDDC_NCO_PHASE:
// 		val = phy->fnco_phase[chan->output][side][fddc_num];
// 		ret = 0;
// 		break;
// 	// case FDDC_NCO_GAIN:
// 	//	val = phy->dac_cache.chan_gain[fddc_num];
// 	//	return ad9088_iio_val_to_str(buf, 0xFFF, val);
// 	case CDDC_TB1_6DB_GAIN:
// 		ret = adi_apollo_cddc_gain_enable_get(&phy->ad9088, cddc_mask, ADI_APOLLO_CDDC_GAIN_TB1, &enable);
// 		val = !!enable;
// 		break;
// 	case CDDC_HB1_6DB_GAIN:
// 		ret = adi_apollo_cddc_gain_enable_get(&phy->ad9088, cddc_mask, ADI_APOLLO_CDDC_GAIN_HB1, &enable);
// 		val = !!enable;
// 		break;
// 	case FDDC_6DB_GAIN:
// 		ret = adi_apollo_fddc_gain_enable_get(&phy->ad9088, fddc_mask, &enable);
// 		val = !!enable;
// 		break;
// 	case CDDC_TEST_TONE_EN:
// 		val = phy->cnco_test_tone_en[chan->output][side][cddc_num];
// 		ret = 0;
// 		break;
// 	case FDDC_TEST_TONE_EN:
// 		val = phy->fnco_test_tone_en[chan->output][side][fddc_num];
// 		ret = 0;
// 		break;
// 	case CDDC_TEST_TONE_OFFSET:
// 		val = phy->cnco_test_tone_offset[chan->output][side][cddc_num];
// 		return ad9088_iio_val_to_str(buf, 0x3FFF, val);
// 	case FDDC_TEST_TONE_OFFSET:
// 		val = phy->fnco_test_tone_offset[chan->output][side][fddc_num];
// 		return ad9088_iio_val_to_str(buf, 0x3FFF, val);
// 	case TRX_CONVERTER_RATE:
// 		if (chan->output)
// 			val = phy->profile.dac_config[side].dac_sampling_rate_Hz;
// 		else
// 			val = phy->profile.adc_config[side].adc_sampling_rate_Hz;

// 		ret = 0;
// 		break;
// 	case DAC_INVSINC_EN:
// 		ret = adi_apollo_invsinc_inspect(&phy->ad9088, ADI_APOLLO_CDUC_IDX2B(side, cddc_num), &invsinc_inspect);
// 		val = !!invsinc_inspect.invsinc_en;
// 		break;
// 	case CFIR_PROFILE_SEL:
// 		ad9088_iiochan_to_cfir(phy, chan, &terminal, &cfir_sel, &dp_sel);
// 		val = phy->cfir_profile[terminal][cfir_sel][dp_sel];
// 		ret = 0;
// 		break;
// 	case CFIR_ENABLE:
// 		ad9088_iiochan_to_cfir(phy, chan, &terminal, &cfir_sel, &dp_sel);
// 		val = phy->cfir_enable[terminal][cfir_sel][dp_sel];
// 		ret = 0;
// 		break;
// 	default:
// 		ret = -EINVAL;
// 	}

// 	if (ret == 0)
// 		ret = sprintf(buf, "%lld\n", val);

// 	return ret;
// }

// static ssize_t ad9088_ext_info_write(struct iio_dev *indio_dev,
// 				     uintptr_t private,
// 				     const struct iio_chan_spec *chan,
// 				     const char *buf, size_t len)
// {
// 	struct axiadc_converter *conv = iio_device_get_drvdata(indio_dev);
// 	struct ad9088_phy *phy = conv->phy;
// 	long long readin;
// 	bool enable;
// 	int ret, readin_32;
// 	u8 cddc_num, fddc_num, side;
// 	u32 cddc_mask, fddc_mask;
// 	s32 val32, tmp;
// 	s64 val64;
// 	u64 ftw, f;
// 	adi_apollo_terminal_e terminal;
// 	adi_apollo_cfir_sel_e cfir_sel;
// 	adi_apollo_cfir_dp_sel dp_sel;

// 	guard(mutex)(&phy->lock);

// 	ad9088_iiochan_to_fddc_cddc(phy, chan, &fddc_num,
// 				    &fddc_mask, &cddc_num, &cddc_mask, &side);

// 	switch (private) {
// 	case CDDC_NCO_FREQ:
// 		ret = kstrtoll(buf, 10, &readin);
// 		if (ret)
// 			return ret;

// 		if (chan->output)
// 			adi_ad9088_calc_nco_ftw32(&phy->ad9088, phy->profile.dac_config[side].dac_sampling_rate_Hz, readin, &ftw);
// 		else
// 			adi_ad9088_calc_nco_ftw32(&phy->ad9088, phy->profile.adc_config[side].adc_sampling_rate_Hz, readin, &ftw);

// 		ret = adi_apollo_cnco_ftw_set(&phy->ad9088, chan->output ? ADI_APOLLO_TX : ADI_APOLLO_RX, cddc_mask, 0, 1, ftw);

// 		if (!ret) {
// 			if (chan->output)
// 				phy->profile.tx_path[side].tx_cduc[cddc_num].nco[0].nco_phase_inc = ftw;
// 			else
// 				phy->profile.rx_path[side].rx_cddc[cddc_num].nco[0].nco_phase_inc = ftw;
// 		}

// 		// if (!ret)
// 		//	phy->cnco_freq[chan->output][side][cddc_num] = readin;

// 		break;
// 	case FDDC_NCO_FREQ:
// 		ret = kstrtoll(buf, 10, &readin);
// 		if (ret)
// 			return ret;

// 		if (chan->output) {
// 			u32 cddc_dcm;

// 			adi_apollo_cduc_interp_bf_to_val(&phy->ad9088, phy->profile.tx_path[side].tx_cduc[cddc_num].drc_ratio, &cddc_dcm);
// 			f = phy->profile.dac_config[side].dac_sampling_rate_Hz;
// 			do_div(f, cddc_dcm);
// 		} else {
// 			u32 cddc_dcm;

// 			adi_apollo_cddc_dcm_bf_to_val(&phy->ad9088, phy->profile.rx_path[side].rx_cddc[cddc_num].drc_ratio, &cddc_dcm);
// 			f = phy->profile.adc_config[side].adc_sampling_rate_Hz;
// 			do_div(f, cddc_dcm);
// 		}

// 		adi_ad9088_calc_nco_ftw(&phy->ad9088, f, readin, &ftw);
// 		ret = adi_apollo_fnco_main_phase_inc_set(&phy->ad9088, chan->output ? ADI_APOLLO_TX : ADI_APOLLO_RX, fddc_mask, ftw);

// 		if (!ret) {
// 			if (chan->output)
// 				phy->profile.tx_path[side].tx_fduc[fddc_num].nco[0].nco_phase_inc = ftw;
// 			else
// 				phy->profile.rx_path[side].rx_fddc[fddc_num].nco[0].nco_phase_inc = ftw;
// 		}

// 		// if (!ret)
// 		//	phy->fnco_freq[chan->output][side][fddc_num] = readin;

// 		break;
// 	case CDDC_NCO_PHASE:
// 		ret = kstrtoll(buf, 10, &readin);
// 		if (ret)
// 			return ret;

// 		readin = clamp_t(long long, readin, -180000, 180000);
// 		val32 = div_s64(readin * S32_MAX, 180000LL);

// 		ret = adi_apollo_cnco_pow_set(&phy->ad9088,
// 					      chan->output ? ADI_APOLLO_TX : ADI_APOLLO_RX,
// 					      cddc_mask, 0, 1, val32);
// 		if (!ret)
// 			phy->cnco_phase[chan->output][side][cddc_num] = readin;
// 		else
// 			ret = -EFAULT;
// 		break;
// 	case FDDC_NCO_PHASE:
// 		ret = kstrtoll(buf, 10, &readin);
// 		if (ret)
// 			return ret;

// 		readin = clamp_t(long long, readin, -180000, 180000);
// 		val64 = div_s64(readin * 14073748835533, 18000LL);

// 		ret = adi_apollo_fnco_main_phase_offset_set(
// 			      &phy->ad9088,
// 			      chan->output ? ADI_APOLLO_TX : ADI_APOLLO_RX,
// 			      fddc_mask,
// 			      val64);
// 		if (!ret)
// 			phy->fnco_phase[chan->output][side][fddc_num] = readin;
// 		else
// 			ret = -EFAULT;
// 		break;
// 	// case FDDC_NCO_GAIN:
// 	//	ret = ad9088_iio_str_to_val(buf, 0, 0xFFF, &readin_32);
// 	//	if (ret)
// 	//		return ret;
// 	//	ret = adi_ad9088_dac_duc_nco_gain_set(
// 	//		&phy->ad9088, fddc_mask, readin_32);
// 	//	if (!ret)
// 	//		phy->dac_cache.chan_gain[fddc_num] = readin_32;
// 	//	break;
// 	// case CDDC_6DB_GAIN:
// 	//	ret = strtobool(buf, &enable);
// 	//	if (ret)
// 	//		return ret;

// 	//	ret = adi_ad9088_adc_ddc_coarse_gain_set(
// 	//		&phy->ad9088, cddc_mask, enable);
// 	//	if (ret)
// 	//		return ret;
// 	//	phy->rx_cddc_gain_6db_en[cddc_num] = enable;
// 	//	ret = 0;
// 	//	break;
// 	case CDDC_TB1_6DB_GAIN:
// 		ret = strtobool(buf, &enable);
// 		if (ret)
// 			return ret;
// 		ret = adi_apollo_cddc_gain_enable_set(&phy->ad9088, cddc_mask, ADI_APOLLO_CDDC_GAIN_TB1, enable);
// 		if (ret)
// 			return ret;
// 		break;
// 	case CDDC_HB1_6DB_GAIN:
// 		ret = strtobool(buf, &enable);
// 		if (ret)
// 			return ret;
// 		ret = adi_apollo_cddc_gain_enable_set(&phy->ad9088, cddc_mask, ADI_APOLLO_CDDC_GAIN_HB1, enable);
// 		if (ret)
// 			return ret;
// 		break;
// 	case FDDC_6DB_GAIN:
// 		ret = strtobool(buf, &enable);
// 		if (ret)
// 			return ret;
// 		ret = adi_apollo_fddc_gain_enable_set(
// 			&phy->ad9088, fddc_mask, enable);
// 		if (ret)
// 			return ret;
// 		break;
// 	case CDDC_TEST_TONE_EN:
// 		ret = strtobool(buf, &enable);
// 		if (ret)
// 			return ret;

// 		if (chan->output)
// 			tmp = phy->profile.tx_path[side].tx_cduc[cddc_num].nco[0].nco_if_mode;
// 		else
// 			tmp = phy->profile.rx_path[side].rx_cddc[cddc_num].nco[0].nco_if_mode;

// 		ret = adi_apollo_cnco_mode_set(&phy->ad9088, chan->output ? ADI_APOLLO_TX : ADI_APOLLO_RX,
// 					       cddc_mask, enable ? ADI_APOLLO_MXR_TEST_MODE : tmp);

// 		if (!ret)
// 			phy->cnco_test_tone_en[chan->output][side][cddc_num] = enable;
// 		break;
// 	case FDDC_TEST_TONE_EN:
// 		ret = strtobool(buf, &enable);
// 		if (ret)
// 			return ret;

// 		if (chan->output)
// 			tmp = phy->profile.tx_path[side].tx_fduc[fddc_num].nco[0].nco_if_mode;
// 		else
// 			tmp = phy->profile.rx_path[side].rx_fddc[fddc_num].nco[0].nco_if_mode;

// 		ret = adi_apollo_fnco_mode_set(&phy->ad9088, chan->output ? ADI_APOLLO_TX : ADI_APOLLO_RX,
// 					       fddc_mask, enable ? ADI_APOLLO_MXR_TEST_MODE : tmp);
// 		if (!ret)
// 			phy->fnco_test_tone_en[chan->output][side][fddc_num] = enable;
// 		break;
// 	case CDDC_TEST_TONE_OFFSET:
// 		ret = ad9088_iio_str_to_val(buf, 0, 0x3FFF, &readin_32);
// 		if (ret)
// 			return ret;

// 		ret = adi_apollo_cnco_test_mode_val_set(&phy->ad9088,
// 							chan->output ? ADI_APOLLO_TX : ADI_APOLLO_RX, cddc_mask, readin_32);
// 		if (!ret)
// 			phy->cnco_test_tone_offset[chan->output][side][cddc_num] = readin_32;

// 		break;
// 	case FDDC_TEST_TONE_OFFSET:
// 		ret = ad9088_iio_str_to_val(buf, 0, 0x3FFF, &readin_32);
// 		if (ret)
// 			return ret;

// 		ret = adi_apollo_fnco_test_mode_val_set(&phy->ad9088,
// 							chan->output ? ADI_APOLLO_TX : ADI_APOLLO_RX, fddc_mask, readin_32);
// 		if (!ret)
// 			phy->fnco_test_tone_offset[chan->output][side][fddc_num] = readin_32;
// 		break;
// 	case DAC_INVSINC_EN:
// 		ret = strtobool(buf, &enable);
// 		if (ret)
// 			return ret;
// 		ret = adi_apollo_tx_inv_sinc_configure(&phy->ad9088, side, cddc_num, enable);
// 		break;
// 	case CFIR_PROFILE_SEL:
// 		ret = kstrtoll(buf, 10, &readin);
// 		if (ret)
// 			return ret;

// 		ad9088_iiochan_to_cfir(phy, chan, &terminal, &cfir_sel, &dp_sel);
// 		adi_apollo_cfir_profile_sel(&phy->ad9088, terminal, cfir_sel, dp_sel, readin);
// 		if (ret < 0)
// 			return ret;
// 		phy->cfir_profile[terminal][cfir_sel][dp_sel] = readin;
// 		break;
// 	case CFIR_ENABLE:
// 		ret = strtobool(buf, &enable);
// 		if (ret)
// 			return ret;
// 		ad9088_iiochan_to_cfir(phy, chan, &terminal, &cfir_sel, &dp_sel);
// 		ret = adi_apollo_cfir_mode_enable_set(&phy->ad9088, terminal, cfir_sel, enable);
// 		if (ret < 0)
// 			return ret;
// 		phy->cfir_enable[terminal][cfir_sel][dp_sel] = enable;
// 		break;
// 	default:
// 		ret = -EINVAL;
// 	}

// 	return ret ? ret : len;
// }

// static int ad9088_device_loopback0(struct ad9088_phy *phy,
// 				   adi_apollo_sides_e side)
// {
// 	adi_apollo_side_select_e sides = side ? ADI_APOLLO_SIDE_B : ADI_APOLLO_SIDE_A;
// 	adi_apollo_adc_select_e select_adc = side ? ADI_APOLLO_ADC_B_ALL : ADI_APOLLO_ADC_A_ALL;
// 	uint16_t xbar[ADI_APOLLO_ADC_PER_SIDE_NUM / 2] = {ADI_APOLLO_ADC_0, ADI_APOLLO_ADC_1};
// 	adi_apollo_device_t *device = &phy->ad9088;
// 	int err = API_CMS_ERROR_OK;

// 	err = adi_apollo_loopback_lb0_read_ptr_rst_set(device, select_adc, 2);
// 	if (err != API_CMS_ERROR_OK) {
// 		dev_err(&phy->spi->dev, "Error in adi_apollo_loopback_lb0_read_ptr_rst_set %d\n", err);
// 		goto end;
// 	}

// 	err = adi_apollo_loopback_lb0_write_ptr_rst_set(device, sides, 2);
// 	if (err != API_CMS_ERROR_OK) {
// 		dev_err(&phy->spi->dev, "Error in adi_apollo_loopback_lb0_write_ptr_rst_set %d\n", err);
// 		goto end;
// 	}

// 	err = adi_apollo_loopback_lb0_tx_xbar_set(device, sides, xbar, 2);
// 	if (err != API_CMS_ERROR_OK) {
// 		dev_err(&phy->spi->dev, "Error in adi_apollo_loopback_lb0_tx_xbar_set: %d\n", err);
// 		goto end;
// 	}

// 	err = adi_apollo_loopback_lb0_rx_enable_set(device, sides, 1);
// 	if (err != API_CMS_ERROR_OK) {
// 		dev_err(&phy->spi->dev, "Error in adi_apollo_loopback_lb0_rx_enable_set %d\n", err);
// 		goto end;
// 	}

// 	err = adi_apollo_loopback_lb0_tx_enable_set(device, select_adc, 1);
// 	if (err != API_CMS_ERROR_OK) {
// 		dev_err(&phy->spi->dev, "Error in adi_apollo_loopback_lb0_tx_enable_set %d\n", err);
// 		goto end;
// 	}

// 	phy->loopback_mode[side] = ADI_APOLLO_LOOPBACK_0;

// end:
// 	return err;
// }

// static int ad9088_device_loopback1(struct ad9088_phy *phy,
// 				   adi_apollo_sides_e side)
// {
// 	adi_apollo_side_select_e sides = side ? ADI_APOLLO_SIDE_B : ADI_APOLLO_SIDE_A;
// 	adi_apollo_device_t *device = &phy->ad9088;
// 	int err = API_CMS_ERROR_OK;
// 	uint16_t lb1_cducs = side ?
// 			     ADI_APOLLO_CDUC_B0 | ADI_APOLLO_CDUC_B1 | ADI_APOLLO_CDUC_B2 | ADI_APOLLO_CDUC_B3 :
// 			     ADI_APOLLO_CDUC_A0 | ADI_APOLLO_CDUC_A1 | ADI_APOLLO_CDUC_A2 | ADI_APOLLO_CDUC_A3;

// 	err = adi_apollo_loopback_lb1_enable_set(device, sides, 1);
// 	if (err)
// 		return err;

// 	err = adi_apollo_loopback_lb1_cduc_enable_set(device, lb1_cducs, 1);
// 	if (err)
// 		return err;

// 	err = adi_apollo_loopback_lb1_blend_set(device, lb1_cducs, phy->lb1_blend[side]);
// 	if (err)
// 		return err;

// 	phy->loopback_mode[side] = ADI_APOLLO_LOOPBACK_1;

// 	return err;
// }

// static int ad9088_device_loopback2(struct ad9088_phy *phy,
// 				   adi_apollo_sides_e side)
// {
// 	adi_apollo_side_select_e sides = side ? ADI_APOLLO_SIDE_B : ADI_APOLLO_SIDE_A;
// 	adi_apollo_device_t *device = &phy->ad9088;
// 	int err = API_CMS_ERROR_OK;
// 	uint16_t lb2_fducs = side ? ADI_APOLLO_FDUC_B_ALL : ADI_APOLLO_FDUC_A_ALL;

// 	err = adi_apollo_loopback_lb2_enable_set(device, sides, 1);
// 	if (err != API_CMS_ERROR_OK)
// 		return err;

// 	err = adi_apollo_loopback_lb2_fduc_enable_set(device, lb2_fducs, 1);
// 	if (err != API_CMS_ERROR_OK)
// 		return err;

// 	phy->loopback_mode[side] = ADI_APOLLO_LOOPBACK_2;

// 	return err;
// }

// static int ad9088_device_loopback3(struct ad9088_phy *phy,
// 				   adi_apollo_sides_e side)
// {
// 	adi_apollo_side_select_e sides = side ? ADI_APOLLO_SIDE_B : ADI_APOLLO_SIDE_A;
// 	adi_apollo_device_t *device = &phy->ad9088;
// 	int err;

// 	err = adi_apollo_loopback_jesd_enable_set(device, sides, 1);
// 	if (err != API_CMS_ERROR_OK)
// 		return err;

// 	phy->loopback_mode[side] = ADI_APOLLO_LOOPBACK_3;

// 	return err;
// }

// static int ad9088_device_loopback_disable(struct ad9088_phy *phy,
// 					  adi_apollo_sides_e side)
// {
// 	adi_apollo_adc_select_e select_adc = side ? ADI_APOLLO_ADC_B_ALL : ADI_APOLLO_ADC_A_ALL;
// 	adi_apollo_device_t *device = &phy->ad9088;
// 	int err = API_CMS_ERROR_OK;
// 	uint16_t lb1_cducs = side ?
// 			     ADI_APOLLO_CDUC_B0 | ADI_APOLLO_CDUC_B1 | ADI_APOLLO_CDUC_B2 | ADI_APOLLO_CDUC_B3 :
// 			     ADI_APOLLO_CDUC_A0 | ADI_APOLLO_CDUC_A1 | ADI_APOLLO_CDUC_A2 | ADI_APOLLO_CDUC_A3;
// 	uint16_t lb2_fducs = side ?
// 			     ADI_APOLLO_FDUC_B_ALL : ADI_APOLLO_FDUC_A_ALL;
// 	adi_apollo_side_select_e sides = side ? ADI_APOLLO_SIDE_B : ADI_APOLLO_SIDE_A;

// 	switch (phy->loopback_mode[side]) {
// 	case ADI_APOLLO_LOOPBACK_0:
// 		err = adi_apollo_loopback_lb0_tx_enable_set(device, select_adc, 0);
// 		if (err != API_CMS_ERROR_OK) {
// 			dev_err(&phy->spi->dev, "Error in adi_apollo_loopback_lb0_tx_enable_set %d\n", err);
// 			goto end;
// 		}

// 		err = adi_apollo_loopback_lb0_rx_enable_set(device, sides, 0);
// 		if (err != API_CMS_ERROR_OK) {
// 			dev_err(&phy->spi->dev, "Error in adi_apollo_loopback_lb0_rx_enable_set %d\n", err);
// 			goto end;
// 		}

// 		break;
// 	case ADI_APOLLO_LOOPBACK_1:
// 		err = adi_apollo_loopback_lb1_cduc_enable_set(device, lb1_cducs, 0);
// 		if (err != API_CMS_ERROR_OK) {
// 			dev_err(&phy->spi->dev, "Error adi_apollo_loopback_lb1_cduc_enable_set %d\n", err);
// 			goto end;
// 		}

// 		err = adi_apollo_loopback_lb1_enable_set(device, sides, 0);
// 		if (err != API_CMS_ERROR_OK) {
// 			dev_err(&phy->spi->dev, "Error in adi_apollo_loopback_lb1_enable_set %d\n", err);
// 			goto end;
// 		}

// 		err = adi_apollo_loopback_lb1_blend_set(device, lb1_cducs, ADI_APOLLO_LB1_BLEND_DISABLE);
// 		if (err != API_CMS_ERROR_OK) {
// 			dev_err(&phy->spi->dev, "Error adi_apollo_loopback_lb1_blend_set %d\n", err);
// 			goto end;
// 		}
// 		break;
// 	case ADI_APOLLO_LOOPBACK_2:
// 		err = adi_apollo_loopback_lb2_enable_set(device, sides, 0);
// 		if (err != API_CMS_ERROR_OK) {
// 			dev_err(&phy->spi->dev, "Error in adi_apollo_loopback_lb2_enable_set %d\n", err);
// 			goto end;
// 		}

// 		err = adi_apollo_loopback_lb2_fduc_enable_set(device, lb2_fducs, 0);
// 		if (err != API_CMS_ERROR_OK) {
// 			dev_err(&phy->spi->dev, "Error in adi_apollo_loopback_lb2_fduc_enable_set %d\n", err);
// 			goto end;
// 		}
// 		break;
// 	case ADI_APOLLO_LOOPBACK_3:
// 		err = adi_apollo_loopback_jesd_enable_set(device, sides, 0);
// 		if (err != API_CMS_ERROR_OK) {
// 			dev_err(&phy->spi->dev, "Error in adi_apollo_loopback_jesd_enable_set %d\n", err);
// 			goto end;
// 		}
// 		break;
// 	}

// 	phy->loopback_mode[side] = ADI_APOLLO_LOOPBACK_NONE;

// end:
// 	return err;
// }

// static int ad9088_assert_fs(struct ad9088_phy *phy, u8 side)
// {
// 	u64 dac_fs = phy->profile.dac_config[side].dac_sampling_rate_Hz;
// 	u64 adc_fs = phy->profile.adc_config[side].adc_sampling_rate_Hz;

// 	if (dac_fs != adc_fs)
// 		return -EINVAL;

// 	return 0;
// }

// static int ad9088_loopback_mode_read(struct iio_dev *indio_dev,
// 				     const struct iio_chan_spec *chan)
// {
// 	struct axiadc_converter *conv = iio_device_get_drvdata(indio_dev);
// 	struct ad9088_phy *phy = conv->phy;
// 	u8 cddc_num, fddc_num, side;
// 	u32 cddc_mask, fddc_mask;

// 	ad9088_iiochan_to_fddc_cddc(phy, chan, &fddc_num,
// 				    &fddc_mask, &cddc_num, &cddc_mask, &side);

// 	return phy->loopback_mode[side];
// }

// static int ad9088_loopback_mode_write(struct iio_dev *indio_dev,
// 				      const struct iio_chan_spec *chan,
// 				      unsigned int item)
// {
// 	struct axiadc_converter *conv = iio_device_get_drvdata(indio_dev);
// 	struct ad9088_phy *phy = conv->phy;
// 	u8 cddc_num, fddc_num, side;
// 	u32 cddc_mask, fddc_mask;
// 	int ret = 0;

// 	ad9088_iiochan_to_fddc_cddc(phy, chan, &fddc_num,
// 				    &fddc_mask, &cddc_num, &cddc_mask, &side);

// 	guard(mutex)(&phy->lock);

// 	ad9088_device_loopback_disable(phy, side);
// 	switch (item) {
// 	case ADI_APOLLO_LOOPBACK_NONE:
// 		break;
// 	case ADI_APOLLO_LOOPBACK_0:
// 		ret = ad9088_assert_fs(phy, side);
// 		if (ret)
// 			return ret;
// 		ret = ad9088_device_loopback0(phy, side);
// 		break;
// 	case ADI_APOLLO_LOOPBACK_1:
// 		ret = ad9088_device_loopback1(phy, side);
// 		break;
// 	case ADI_APOLLO_LOOPBACK_2:
// 		ret = ad9088_device_loopback2(phy, side);
// 		break;
// 	case ADI_APOLLO_LOOPBACK_3:
// 		ret = ad9088_device_loopback3(phy, side);
// 		break;
// 	default:
// 		ret = -EINVAL;
// 	}

// 	if (ret)
// 		return ret;

// 	ret = adi_apollo_clk_mcs_dyn_sync_sequence_run(&phy->ad9088);

// 	return ret;
// }

// static const char *const ad9088_loopback_modes[] = {
// 	[ADI_APOLLO_LOOPBACK_NONE] = "off",
// 	[ADI_APOLLO_LOOPBACK_0] = "loopback0",
// 	[ADI_APOLLO_LOOPBACK_1] = "loopback1",
// 	[ADI_APOLLO_LOOPBACK_2] = "loopback2",
// 	[ADI_APOLLO_LOOPBACK_3] = "loopback3_jesd",
// };

// static const struct iio_enum ad9088_loopback_modes_enum = {
// 	.items = ad9088_loopback_modes,
// 	.num_items = ARRAY_SIZE(ad9088_loopback_modes),
// 	.set = ad9088_loopback_mode_write,
// 	.get = ad9088_loopback_mode_read,
// };

// static struct iio_chan_spec_ext_info rxadc_ext_info[] = {
// 	IIO_ENUM("test_mode", IIO_SEPARATE, &ad9088_testmode_enum),
// 	IIO_ENUM_AVAILABLE("test_mode", IIO_SHARED_BY_TYPE, &ad9088_testmode_enum),
// 	IIO_ENUM("loopback", IIO_SEPARATE, &ad9088_loopback_modes_enum),
// 	IIO_ENUM_AVAILABLE("loopback", IIO_SHARED_BY_TYPE, &ad9088_loopback_modes_enum),
// 	IIO_ENUM("nyquist_zone", IIO_SEPARATE, &ad9088_nyquist_zone_enum),
// 	IIO_ENUM_AVAILABLE("nyquist_zone", IIO_SHARED_BY_TYPE, &ad9088_nyquist_zone_enum),
// 	{
// 		.name = "main_nco_frequency",
// 		.read = ad9088_ext_info_read,
// 		.write = ad9088_ext_info_write,
// 		.shared = IIO_SEPARATE,
// 		.private = CDDC_NCO_FREQ,
// 	},
// 	{
// 		.name = "main_nco_frequency_available",
// 		.read = ad9088_ext_info_read,
// 		.write = ad9088_ext_info_write,
// 		.shared = IIO_SHARED_BY_TYPE,
// 		.private = CDDC_NCO_FREQ_AVAIL,
// 	},
// 	{
// 		.name = "channel_nco_frequency",
// 		.read = ad9088_ext_info_read,
// 		.write = ad9088_ext_info_write,
// 		.shared = IIO_SEPARATE,
// 		.private = FDDC_NCO_FREQ,
// 	},
// 	{
// 		.name = "channel_nco_frequency_available",
// 		.read = ad9088_ext_info_read,
// 		.write = ad9088_ext_info_write,
// 		.shared = IIO_SEPARATE,
// 		.private = FDDC_NCO_FREQ_AVAIL,
// 	},
// 	{
// 		.name = "main_nco_phase",
// 		.read = ad9088_ext_info_read,
// 		.write = ad9088_ext_info_write,
// 		.shared = IIO_SEPARATE,
// 		.private = CDDC_NCO_PHASE,
// 	},
// 	{
// 		.name = "channel_nco_phase",
// 		.read = ad9088_ext_info_read,
// 		.write = ad9088_ext_info_write,
// 		.shared = IIO_SEPARATE,
// 		.private = FDDC_NCO_PHASE,
// 	},
// 	{
// 		.name = "main_tb1_6db_digital_gain_en",
// 		.read = ad9088_ext_info_read,
// 		.write = ad9088_ext_info_write,
// 		.shared = IIO_SEPARATE,
// 		.private = CDDC_TB1_6DB_GAIN,
// 	},
// 	{
// 		.name = "main_hb1_6db_digital_gain_en",
// 		.read = ad9088_ext_info_read,
// 		.write = ad9088_ext_info_write,
// 		.shared = IIO_SEPARATE,
// 		.private = CDDC_HB1_6DB_GAIN,
// 	},
// 	{
// 		.name = "channel_6db_digital_gain_en",
// 		.read = ad9088_ext_info_read,
// 		.write = ad9088_ext_info_write,
// 		.shared = IIO_SEPARATE,
// 		.private = FDDC_6DB_GAIN,
// 	},
// 	{
// 		.name = "adc_frequency",
// 		.read = ad9088_ext_info_read,
// 		.shared = IIO_SHARED_BY_TYPE,
// 		.private = TRX_CONVERTER_RATE,
// 	},
// 	{
// 		.name = "cfir_en",
// 		.read = ad9088_ext_info_read,
// 		.write = ad9088_ext_info_write,
// 		.shared = IIO_SEPARATE,
// 		.private = CFIR_ENABLE,
// 	},
// 	{
// 		.name = "cfir_profile_sel",
// 		.read = ad9088_ext_info_read,
// 		.write = ad9088_ext_info_write,
// 		.shared = IIO_SEPARATE,
// 		.private = CFIR_PROFILE_SEL,
// 	},
// 	{},
// };

// static struct iio_chan_spec_ext_info txdac_ext_info[] = {
// 	{
// 		.name = "main_nco_frequency",
// 		.read = ad9088_ext_info_read,
// 		.write = ad9088_ext_info_write,
// 		.shared = IIO_SEPARATE,
// 		.private = CDDC_NCO_FREQ,
// 	},
// 	{
// 		.name = "main_nco_frequency_available",
// 		.read = ad9088_ext_info_read,
// 		.write = ad9088_ext_info_write,
// 		.shared = IIO_SHARED_BY_TYPE,
// 		.private = CDDC_NCO_FREQ_AVAIL,
// 	},
// 	{
// 		.name = "channel_nco_frequency",
// 		.read = ad9088_ext_info_read,
// 		.write = ad9088_ext_info_write,
// 		.shared = IIO_SEPARATE,
// 		.private = FDDC_NCO_FREQ,
// 	},
// 	{
// 		.name = "channel_nco_frequency_available",
// 		.read = ad9088_ext_info_read,
// 		.write = ad9088_ext_info_write,
// 		.shared = IIO_SHARED_BY_TYPE,
// 		.private = FDDC_NCO_FREQ_AVAIL,
// 	},
// 	{
// 		.name = "main_nco_phase",
// 		.read = ad9088_ext_info_read,
// 		.write = ad9088_ext_info_write,
// 		.shared = IIO_SEPARATE,
// 		.private = CDDC_NCO_PHASE,
// 	},
// 	{
// 		.name = "channel_nco_phase",
// 		.read = ad9088_ext_info_read,
// 		.write = ad9088_ext_info_write,
// 		.shared = IIO_SEPARATE,
// 		.private = FDDC_NCO_PHASE,
// 	},
// 	{
// 		.name = "channel_nco_gain_scale",
// 		.read = ad9088_ext_info_read,
// 		.write = ad9088_ext_info_write,
// 		.shared = IIO_SEPARATE,
// 		.private = FDDC_NCO_GAIN,
// 	},
// 	{
// 		.name = "main_nco_test_tone_en",
// 		.read = ad9088_ext_info_read,
// 		.write = ad9088_ext_info_write,
// 		.shared = IIO_SEPARATE,
// 		.private = CDDC_TEST_TONE_EN,
// 	},
// 	{
// 		.name = "channel_nco_test_tone_en",
// 		.read = ad9088_ext_info_read,
// 		.write = ad9088_ext_info_write,
// 		.shared = IIO_SEPARATE,
// 		.private = FDDC_TEST_TONE_EN,
// 	},
// 	{
// 		.name = "main_nco_test_tone_scale",
// 		.read = ad9088_ext_info_read,
// 		.write = ad9088_ext_info_write,
// 		.shared = IIO_SEPARATE,
// 		.private = CDDC_TEST_TONE_OFFSET,
// 	},
// 	{
// 		.name = "channel_nco_test_tone_scale",
// 		.read = ad9088_ext_info_read,
// 		.write = ad9088_ext_info_write,
// 		.shared = IIO_SEPARATE,
// 		.private = FDDC_TEST_TONE_OFFSET,
// 	},
// 	{
// 		.name = "dac_frequency",
// 		.read = ad9088_ext_info_read,
// 		.shared = IIO_SHARED_BY_TYPE,
// 		.private = TRX_CONVERTER_RATE,
// 	},
// 	{
// 		.name = "invsinc_en",
// 		.read = ad9088_ext_info_read,
// 		.write = ad9088_ext_info_write,
// 		.shared = IIO_SEPARATE,
// 		.private = DAC_INVSINC_EN,
// 	},
// 	{
// 		.name = "cfir_en",
// 		.read = ad9088_ext_info_read,
// 		.write = ad9088_ext_info_write,
// 		.shared = IIO_SEPARATE,
// 		.private = CFIR_ENABLE,
// 	},
// 	{
// 		.name = "cfir_profile_sel",
// 		.read = ad9088_ext_info_read,
// 		.write = ad9088_ext_info_write,
// 		.shared = IIO_SEPARATE,
// 		.private = CFIR_PROFILE_SEL,
// 	},
// 	{},
// };

// static int ad9088_set_sample_rate(struct axiadc_converter *conv,
// 				  unsigned int sample_rate)
// {
// 	return -ENOTSUPP;
// }

// static int ad9088_request_clks(struct axiadc_converter *conv)
// {
// 	struct ad9088_phy *phy = conv->phy;
// 	int ret;

// 	phy->dev_clk = devm_clk_get(&conv->spi->dev, "dev_clk");
// 	if (IS_ERR(phy->dev_clk))
// 		return PTR_ERR(phy->dev_clk);

// 	ret = clk_prepare_enable(phy->dev_clk);
// 	if (ret)
// 		return ret;

// 	return 0;
// }


// static const char *const ad9088_jtx_qbf_states[] = {
// 	"CGS", "ILA_M0R", "ILA_M0", "ILA_M1R", "ILA_M1C1", "ILA_M1C2",
// 	"ILA_M1C3", "ILA_M1", "ILA_M2R", "ILA_M2", "ILA_M3R", "ILA_M3",
// 	"ILA_BP", "DATA"
// };

// int ad9088_jesd_tx_link_status_print(struct ad9088_phy *phy,
// 				     struct jesd204_link *lnk, int retry)
// {
// 	int ret;
// 	u16 stat;

// 	do {
// 		ret = adi_apollo_jtx_link_status_get(&phy->ad9088,
// 						     ad9088_to_link(lnk->link_id), &stat);

// 		if (ret)
// 			return -EFAULT;

// 		if (lnk->jesd_version == JESD204_VERSION_C) {
// 			if ((stat & 0x60) == 0x60)
// 				ret = 0;
// 			else
// 				ret = -EIO;

// 			if (ret == 0 || retry == 0)
// 				dev_info(&phy->spi->dev,
// 					 "%s Link%d PLL %s, PHASE %s, MODE %s\n",
// 					 ad9088_fsm_links_to_str[lnk->link_id],
// 					 lnk->link_id,
// 					 stat & BIT(5) ? "locked" : "unlocked",
// 					 stat & BIT(6) ? "established" : "lost",
// 					 stat & BIT(7) ? "invalid" : "valid");
// 			else
// 				msleep(20);
// 		} else {
// 			if ((stat & 0xFF) == 0x7D)
// 				ret = 0;
// 			else
// 				ret = -EIO;

// 			if (ret == 0 || retry == 0)
// 				dev_info(&phy->spi->dev,
// 					 "%s Link%d in %s, SYNC %s, PLL %s, PHASE %s, MODE %s\n",
// 					 ad9088_fsm_links_to_str[lnk->link_id],
// 					 lnk->link_id, ad9088_jtx_qbf_states[stat & 0xF],
// 					 stat & BIT(4) ? "deasserted" : "asserted",
// 					 stat & BIT(5) ? "locked" : "unlocked",
// 					 stat & BIT(6) ? "established" : "lost",
// 					 stat & BIT(7) ? "invalid" : "valid");
// 			else
// 				msleep(20);
// 		}
// 	} while (ret && retry--);

// 	return ret;
// }

// static const char *const ad9088_jrx_204c_states[] = {
// 	"Reset", "Undef", "Sync header alignment done",
// 	"Extended multiblock sync complete",
// 	"Extended multiblock alignment complete",
// 	"Undef", "Link is good", "Undef",
// };

// static int ad9088_jesd_rx_link_status_print(struct ad9088_phy *phy,
// 		struct jesd204_link *lnk, int retry)
// {
// 	int ret, i;
// 	u16 stat, l_stat, mask;
// 	u8 id = ad9088_to_link(lnk->link_id);

// 	do {
// 		ret = adi_apollo_jrx_link_status_get(&phy->ad9088, id, &stat);
// 		if (ret)
// 			return -EFAULT;

// 		if (lnk->jesd_version == JESD204_VERSION_C) {
// 			if (phy->profile.jrx[(lnk->link_id / 2) & 1].common_link_cfg.subclass)
// 				mask = 0x60; /* Subclass 1*/
// 			else
// 				mask = 0x20; /* Ignore SYSREF Phase */

// 			if ((stat & mask) == mask)
// 				ret = 0;
// 			else
// 				ret = -EIO;

// 			if (ret == 0 || retry == 0) {
// 				for (i = 0; i < lnk->num_lanes; i++) {
// 					u8 phys_lane = phy->profile.jrx[(lnk->link_id / 2) & 1].rx_link_cfg[(lnk->link_id % 2) & 1].lane_xbar[i];

// 					adi_apollo_jrx_j204c_lane_status_get(&phy->ad9088, id, phys_lane, &l_stat);
// 					if (6 == (l_stat & 0x7)) {
// 						dev_info(&phy->spi->dev, "%s Link%d 204C Lane-%d@%d status: %s\n",
// 							 ad9088_fsm_links_to_str[lnk->link_id], lnk->link_id, i, phys_lane, ad9088_jrx_204c_states[l_stat & 0x7]);
// 					} else {
// 						dev_err(&phy->spi->dev, "%s Link%d 204C Lane-%d@%d status: %s\n",
// 							ad9088_fsm_links_to_str[lnk->link_id], lnk->link_id, i, phys_lane, ad9088_jrx_204c_states[l_stat & 0x7]);
// 					}
// 				}

// 				dev_info(&phy->spi->dev,
// 					 "%s Link%d 204C User status: %s, SYSREF Phase: %s\n",
// 					 ad9088_fsm_links_to_str[lnk->link_id], lnk->link_id, (stat & 0x20) ? "Ready" :  "Fail",
// 					 (stat & 0x40) ? "Locked" :  "Unlocked");
// 			} else {
// 				msleep(20);
// 			}
// 		} else {
// 			/* FIXME */
// 			mask = (1 << lnk->num_lanes) - 1;

// 			stat = mask & stat;

// 			if (stat == mask)
// 				ret = 0;
// 			else
// 				ret = -EIO;

// 			if (ret == 0 || retry == 0)
// 				dev_info(&phy->spi->dev,
// 					 "%s Link%d 0x%X lanes in DATA\n",
// 					 ad9088_fsm_links_to_str[lnk->link_id], lnk->link_id, stat);
// 			else
// 				msleep(20);
// 		}
// 	} while (ret && retry--);

// 	return ret;
// }



// static int ad9088_read_raw(struct iio_dev *indio_dev,
// 			   const struct iio_chan_spec *chan, int *val,
// 			   int *val2, long info)
// {
// 	struct axiadc_converter *conv = iio_device_get_drvdata(indio_dev);
// 	struct ad9088_phy *phy = conv->phy;
// 	adi_apollo_device_tmu_data_t tmu_data;
// 	//u8 msb, lsb;
// 	u8 cddc_num, fddc_num, side, dir;
// 	u32 cddc_mask, fddc_mask;
// 	u64 freq;
// 	int ret;

// 	ad9088_iiochan_to_fddc_cddc(phy, chan, &fddc_num,
// 				    &fddc_mask, &cddc_num, &cddc_mask, &side);

// 	guard(mutex)(&phy->lock);

// 	switch (info) {
// 	case IIO_CHAN_INFO_SAMP_FREQ:
// 		dir = chan->output ? TX_SAMPL_CLK : RX_SAMPL_CLK;

// 		if (!phy->clks[dir])
// 			return -ENODEV;

// 		freq = clk_get_rate_scaled(phy->clks[dir], &phy->clkscale[dir]);

// 		*val = lower_32_bits(freq);
// 		*val2 = upper_32_bits(freq);
// 		return IIO_VAL_INT_64;
// 	case IIO_CHAN_INFO_ENABLE:
// 		if (chan->output)
// 			*val = !!(phy->tx_en_mask & cddc_mask);
// 		else
// 			*val = !!(phy->rx_en_mask & cddc_mask);

// 		return IIO_VAL_INT;
// 		break;
// 	case IIO_CHAN_INFO_PROCESSED:
// 		if (!phy->is_initialized)
// 			return -EBUSY;

// 		ret = adi_apollo_device_tmu_get(&phy->ad9088, &tmu_data);
// 		if (ret)
// 			return -EFAULT;

// 		*val = tmu_data.temp_degrees_celsius_avg * 1000;
// 		return IIO_VAL_INT;
// 	}
// 	return -EINVAL;
// }

// static int ad9088_write_raw(struct iio_dev *indio_dev,
// 			    const struct iio_chan_spec *chan, int val, int val2,
// 			    long info)
// {
// 	struct axiadc_converter *conv = iio_device_get_drvdata(indio_dev);
// 	struct ad9088_phy *phy = conv->phy;
// 	unsigned long r_clk;
// 	u8 cddc_num, fddc_num, side;
// 	u32 cddc_mask, fddc_mask;
// 	int ret;

// 	guard(mutex)(&phy->lock);

// 	switch (info) {
// 	case IIO_CHAN_INFO_SAMP_FREQ:
// 		if (!conv->clk)
// 			return -ENODEV;

// 		if (chan->extend_name)
// 			return -ENODEV;

// 		if (conv->sample_rate_read_only)
// 			return -EPERM;

// 		return ad9088_set_sample_rate(conv, val);

// 		r_clk = clk_round_rate(conv->clk, val);
// 		if (r_clk < 0 || r_clk > conv->chip_info->max_rate) {
// 			dev_warn(&conv->spi->dev,
// 				 "Error setting ADC sample rate %ld", r_clk);
// 			return -EINVAL;
// 		}

// 		ret = clk_set_rate(conv->clk, r_clk);
// 		if (ret < 0)
// 			return ret;
// 		break;

// 	case IIO_CHAN_INFO_ENABLE:

// 		ad9088_iiochan_to_fddc_cddc(phy, chan, &fddc_num, &fddc_mask, &cddc_num, &cddc_mask, &side);

// 		if (chan->output) {
// 			adi_apollo_txen_pwrup_ctrl_t txen_config = {
// 				.sm_clk_rate = ADI_APOLLO_PUC_CLK_RATE_FS_DIV_32,
// 				.sm_en = 0,
// 				.spi_txen = !!val,
// 				.spi_txen_en = !!val
// 			};

// 			/* Enable Tx blocks - enable/disable via spi */
// 			ret = adi_apollo_txen_pwrup_ctrl_set(&phy->ad9088, cddc_mask, &txen_config);
// 			if (ret) {
// 				dev_err(&phy->spi->dev, "Error activating Tx blocks(%d)\n", ret);
// 				return ret;
// 			}

// 			if (val)
// 				phy->tx_en_mask |= cddc_mask;
// 			else
// 				phy->tx_en_mask &= ~cddc_mask;

// 		} else {
// 			adi_apollo_rxen_pwrup_ctrl_t rxen_config = {
// 				.sm_clk_rate = ADI_APOLLO_PUC_CLK_RATE_FS_DIV_32,
// 				.sm_en = 0,
// 				.spi_rxen = !!val,
// 				.spi_rxen_en = !!val
// 			};

// 			/* Enable Rx blocks - enable/disable via spi */
// 			ret = adi_apollo_rxen_pwrup_ctrl_set(&phy->ad9088, cddc_mask, &rxen_config);
// 			if (ret) {
// 				dev_err(&phy->spi->dev, "Error activating Rx blocks (%d)\n", ret);
// 				return ret;
// 			}
// 			if (val)
// 				phy->rx_en_mask |= cddc_mask;
// 			else
// 				phy->rx_en_mask &= ~cddc_mask;

// 		}
// 		break;
// 	default:
// 		return -EINVAL;
// 	}

// 	return 0;
// }

// static int ad9088_read_label(struct iio_dev *indio_dev,
// 			     const struct iio_chan_spec *chan, char *label)
// {
// 	struct axiadc_converter *conv = iio_device_get_drvdata(indio_dev);
// 	struct ad9088_phy *phy = conv->phy;

// 	return sprintf(label, "%s\n",
// 		       chan->output ? phy->tx_labels[chan->channel] :
// 		       phy->rx_labels[chan->channel]);
// }

// int ad9088_mcs_init_cal_setup(struct ad9088_phy *phy)
// {
// 	adi_apollo_device_t *device = &phy->ad9088;
// 	s64 reference_period_C_femtoseconds;
// 	u64 freq;

// 	freq = phy->profile.clk_cfg.dev_clk_freq_kHz * 1000ULL;
// 	freq = div64_u64(freq, (phy->profile.clk_cfg.clocking_mode == ADI_APOLLO_CLOCKING_MODE_SDR_DIV_8 ? 8 : 4) *
// 			 phy->profile.mcs_cfg.internal_sysref_prd_digclk_cycles_center);
// 	reference_period_C_femtoseconds = div64_u64(1000000000000000ULL, freq);

// 	dev_dbg(&phy->spi->dev, "SYSREF frequeny %llu Hz, SYSREF period: %lld fs\n", freq, reference_period_C_femtoseconds);

// 	adi_apollo_mcs_cal_config_t cal_config  = {
// 		.use_side_A_as_reference            = 0,
// 		.use_gapped_sysref                  = 0,
// 		.pad8                               = 0,
// 		.leave_sysref_receiver_ON_post_sync = 1,
// 		.track_abort                        = 0,
// 		.track_halt                         = 0,
// 		.track_initialize                   = 1,
// 		.pad8_2                             = 0,
// 		.track_foreground                   = 0,
// 		.track_force_foreground             = 0,
// 		.track_background                   = {0, 0},
// 		.track_coarse_jump                  = {0, 0},
// 		.track_force_background_step        = {0, 0},
// 		.measurement_decimation_rate        = 31,  // TDC Decimation setting prioritizing execution time.
// 		.pad16                              = 0,
// 		.reference_period_C_femtoseconds    = reference_period_C_femtoseconds,
// 		.reference_period_A_femtoseconds    = 1,
// 		.reference_period_B_femtoseconds    = 1,
// 		.offset_C_femtoseconds              = 0,
// 		.offset_A_femtoseconds              = 0,
// 		.offset_B_femtoseconds              = 0,
// 		.adf4382_specific_config            = {
// 			.del_mode              = CpBld_Mode,
// 			.track_start_coarse    = 0,
// 			.track_start_bleed_pol = 0,
// 			.track_polarity_select = 0,
// 			.phase_adjustment      = 1,
// 			.pad8                  = 0,
// 			.track_start_fine      = 0,
// 			.DELSTR_gpio           = {30, 0}, /* FIXME Hardcoded */
// 			.pad                   = {0, 0},
// 			.DELADJ_gpio           = {29, 0},
// 			.pad2                  = {0, 0},
// 			.track_win             = 500,
// 			.track_target          = {20000, 0}
// 		}
// 	};

// 	/* Load MCS Calibration Configuration settings. */
// 	return adi_apollo_mcs_cal_config_set(device, &cal_config);
// }

// static int ad9088_mcs_init_cal_status_print(struct ad9088_phy *phy, char *buf, adi_apollo_mcs_cal_init_status_t *cal_status)
// {
// 	int len = 0;

// 	len += sprintf(buf + len,  "errorCode: %d.\n"        , cal_status->hdr.errorCode);
// 	len += sprintf(buf + len,  "percentComplete: %d.\n"  , cal_status->hdr.percentComplete);
// 	len += sprintf(buf + len,  "performanceMetric: %d.\n", cal_status->hdr.performanceMetric);
// 	len += sprintf(buf + len,  "iterCount: %d.\n"        , cal_status->hdr.iterCount);
// 	len += sprintf(buf + len,  "updateCount: %d.\n\n"    , cal_status->hdr.updateCount);

// 	len += sprintf(buf + len,  "mcsErr: %d.\n\n", cal_status->mcsErr);

// 	len += sprintf(buf + len,  "is_C_Locked: %d.\n"  , cal_status->data.is_C_Locked);
// 	len += sprintf(buf + len,  "is_A_Locked: %d.\n"  , cal_status->data.is_A_Locked);
// 	len += sprintf(buf + len,  "is_B_Locked: %d.\n\n", cal_status->data.is_B_Locked);

// 	len += sprintf(buf + len,  "diff_C_Before_femtoseconds: %lld.\n"       , cal_status->data.diff_C_Before_femtoseconds);
// 	len += sprintf(buf + len,  "diff_A_Before_femtoseconds: %lld.\n"       , cal_status->data.diff_A_Before_femtoseconds);
// 	len += sprintf(buf + len,  "diff_B_Before_femtoseconds: %lld.\n"       , cal_status->data.diff_B_Before_femtoseconds);
// 	len += sprintf(buf + len,  "internal_period_C_femtoseconds: %lld.\n"   , cal_status->data.internal_period_C_femtoseconds);
// 	len += sprintf(buf + len,  "internal_period_A_femtoseconds: %lld.\n"   , cal_status->data.internal_period_A_femtoseconds);
// 	len += sprintf(buf + len,  "internal_period_B_femtoseconds: %lld.\n"   , cal_status->data.internal_period_B_femtoseconds);
// 	len += sprintf(buf + len,  "diff_C_After_femtoseconds: %lld.\n"        , cal_status->data.diff_C_After_femtoseconds);
// 	len += sprintf(buf + len,  "diff_A_After_femtoseconds: %lld.\n"        , cal_status->data.diff_A_After_femtoseconds);
// 	len += sprintf(buf + len,  "diff_B_After_femtoseconds: %lld.\n"        , cal_status->data.diff_B_After_femtoseconds);
// 	len += sprintf(buf + len,  "recommended_offset_C_femtoseconds: %lld.\n", cal_status->data.recommended_offset_C_femtoseconds);
// 	len += sprintf(buf + len,  "recommended_offset_A_femtoseconds: %lld.\n", cal_status->data.recommended_offset_A_femtoseconds);
// 	len += sprintf(buf + len,  "recommended_offset_B_femtoseconds: %lld.\n", cal_status->data.recommended_offset_B_femtoseconds);

// 	return len;
// }


// static int ad9088_mcs_track_cal_status_print(struct ad9088_phy *phy, char *buf, adi_apollo_mcs_cal_status_t *cal_status, uint8_t print_full_state)
// {

// 	int len = 0;

// 	if (print_full_state == 1) {
// 		len += sprintf(buf + len,  "errorCode: %d.\n"        , cal_status->hdr.errorCode);
// 		len += sprintf(buf + len,  "percentComplete: %d.\n"  , cal_status->hdr.percentComplete);
// 		len += sprintf(buf + len,  "performanceMetric: %d.\n", cal_status->hdr.performanceMetric);
// 		len += sprintf(buf + len,  "iterCount: %d.\n"        , cal_status->hdr.iterCount);
// 		len += sprintf(buf + len,  "updateCount: %d.\n\n"    , cal_status->hdr.updateCount);

// 		len += sprintf(buf + len,  "foreground_done: %d.\n"         , cal_status->mcs_tracking_cal_status.foreground_done);
// 		len += sprintf(buf + len,  "track_state[0]: %d.\n"          , cal_status->mcs_tracking_cal_status.track_state[0]);
// 		len += sprintf(buf + len,  "track_state[1]: %d.\n"          , cal_status->mcs_tracking_cal_status.track_state[1]);
// 		len += sprintf(buf + len,  "track_lock[0]: %d.\n"           , cal_status->mcs_tracking_cal_status.track_lock[0]);
// 		len += sprintf(buf + len,  "track_lock[1]: %d.\n"           , cal_status->mcs_tracking_cal_status.track_lock[1]);
// 		len += sprintf(buf + len,  "halt_active: %d.\n\n"           , cal_status->mcs_tracking_cal_status.halt_active);
// 		len += sprintf(buf + len,  "force_background_done[0]: %d.\n", cal_status->mcs_tracking_cal_status.force_background_done[0]);
// 		len += sprintf(buf + len,  "force_background_done[1]: %d.\n", cal_status->mcs_tracking_cal_status.force_background_done[1]);
// 		len += sprintf(buf + len,  "abort_done: %d.\n\n"            , cal_status->mcs_tracking_cal_status.abort_done);

// 		len += sprintf(buf + len,  "adf4382_specific_status[0].bleed_pol: %d.\n"           , cal_status->mcs_tracking_cal_status.adf4382_specific_status[0].bleed_pol);
// 		len += sprintf(buf + len,  "adf4382_specific_status[0].current_coarse_value: %d.\n", cal_status->mcs_tracking_cal_status.adf4382_specific_status[0].current_coarse_value);
// 		len += sprintf(buf + len,  "adf4382_specific_status[0].current_fine_value: %d.\n"  , cal_status->mcs_tracking_cal_status.adf4382_specific_status[0].current_fine_value);
// 		len += sprintf(buf + len,  "adf4382_specific_status[0].EOR_POS: %d.\n"             , cal_status->mcs_tracking_cal_status.adf4382_specific_status[0].EOR_POS);
// 		len += sprintf(buf + len,  "adf4382_specific_status[0].EOR_NEG: %d.\n"             , cal_status->mcs_tracking_cal_status.adf4382_specific_status[0].EOR_NEG);
// 		len += sprintf(buf + len,  "adf4382_specific_status[0].EOR_Coarse: %d.\n"          , cal_status->mcs_tracking_cal_status.adf4382_specific_status[0].EOR_Coarse);

// 		len += sprintf(buf + len,  "adf4382_specific_status[1].bleed_pol: %d.\n"           , cal_status->mcs_tracking_cal_status.adf4382_specific_status[1].bleed_pol);
// 		len += sprintf(buf + len,  "adf4382_specific_status[1].current_coarse_value: %d.\n", cal_status->mcs_tracking_cal_status.adf4382_specific_status[1].current_coarse_value);
// 		len += sprintf(buf + len,  "adf4382_specific_status[1].current_fine_value: %d.\n"  , cal_status->mcs_tracking_cal_status.adf4382_specific_status[1].current_fine_value);
// 		len += sprintf(buf + len,  "adf4382_specific_status[1].EOR_POS: %d.\n"             , cal_status->mcs_tracking_cal_status.adf4382_specific_status[1].EOR_POS);
// 		len += sprintf(buf + len,  "adf4382_specific_status[1].EOR_NEG: %d.\n"             , cal_status->mcs_tracking_cal_status.adf4382_specific_status[1].EOR_NEG);
// 		len += sprintf(buf + len,  "adf4382_specific_status[1].EOR_Coarse: %d.\n"          , cal_status->mcs_tracking_cal_status.adf4382_specific_status[1].EOR_Coarse);

// 		len += sprintf(buf + len,  "current_measure[0]: %lld.\n", cal_status->mcs_tracking_cal_status.current_measure[0]);
// 		len += sprintf(buf + len,  "current_measure[1]: %lld.\n", cal_status->mcs_tracking_cal_status.current_measure[1]);
// 	} else {
// 		len += sprintf(buf + len,  "Tracking Cal[0]: --> \t");
// 		len += sprintf(buf + len,  "bleed_pol: %d. \t"           , cal_status->mcs_tracking_cal_status.adf4382_specific_status[0].bleed_pol);
// 		len += sprintf(buf + len,  "current_coarse: %d. \t"      , cal_status->mcs_tracking_cal_status.adf4382_specific_status[0].current_coarse_value);
// 		len += sprintf(buf + len,  "current_fine: %d. \t"        , cal_status->mcs_tracking_cal_status.adf4382_specific_status[0].current_fine_value);
// 		len += sprintf(buf + len,  "current_measure: %lld. \n"   , cal_status->mcs_tracking_cal_status.current_measure[0]);
// 	}

// 	return len;
// }

// static int ad9088_delta_t_measurement_set(struct ad9088_phy *phy, u32 mode)
// {
// 	adi_apollo_mailbox_cmd_mcs_bsync_set_config_t bsync_set_config_cmd = {0};
// 	adi_apollo_mailbox_resp_mcs_bsync_set_config_t bsync_set_config_resp = {0};
// 	adi_apollo_mailbox_resp_mcs_bsync_go_t bsync_go_resp = {0};
// 	adi_apollo_device_t *device = &phy->ad9088;
// 	int ret;

// 	u32 bsync_divider = (phy->profile.clk_cfg.clocking_mode == ADI_APOLLO_CLOCKING_MODE_SDR_DIV_8 ? 8 : 4) *
// 		phy->profile.mcs_cfg.internal_sysref_prd_digclk_cycles_center;

// 	switch (mode) {
// 	case 0:
// 		bsync_set_config_cmd.func_mode = APOLLO_MCS_BSYNC_ALIGN;
// 		break;
// 	case 1:
// 		bsync_set_config_cmd.func_mode = APOLLO_MCS_BSYNC_OUTPUT_EN;
// 		break;
// 	case 2:
// 		bsync_set_config_cmd.func_mode = APOLLO_MCS_BSYNC_OUTPUT_DIS;
// 		break;
// 	default:
// 		return -EINVAL;
// 	}

// 	bsync_set_config_cmd.bsync_div = bsync_divider;

// 	ret = adi_apollo_mailbox_mcs_bsync_set_config(device, &bsync_set_config_cmd, &bsync_set_config_resp);
// 	if (ret)
// 		return ret;

// 	if (bsync_set_config_resp.status)
// 		dev_warn(&phy->spi->dev, "bsync_set_config_resp.status: %d.\n", bsync_set_config_resp.status);

// 	ret = adi_apollo_mailbox_mcs_bsync_go(device, &bsync_go_resp);
// 	if (ret)
// 		return ret;

// 	if (bsync_go_resp.status)
// 		dev_warn(&phy->spi->dev, "bsync_go_resp.status: %d.\n", bsync_go_resp.status);

// 	return 0;
// }

// static int ad9088_delta_t_measurement_get(struct ad9088_phy *phy, u32 mode, s64 *apollo_delta_t)
// {
// 	adi_apollo_mailbox_resp_mcs_bsync_get_config_t bsync_get_config_resp = {0};
// 	adi_apollo_device_t *device = &phy->ad9088;
// 	int ret;

// 	ret = adi_apollo_mailbox_mcs_bsync_get_config(device, &bsync_get_config_resp);
// 	if (ret)
// 		return ret;

// 	switch (mode) {
// 	case 0:
// 		*apollo_delta_t = bsync_get_config_resp.delta_t0;
// 		break;
// 	case 1:
// 		*apollo_delta_t = bsync_get_config_resp.delta_t1;
// 		break;
// 	default:
// 		return -EINVAL;
// 	}

// 	dev_dbg(&phy->spi->dev, "bsync_get_config_resp:\n");
// 	dev_dbg(&phy->spi->dev, "\t status: %d.\n", bsync_get_config_resp.status);
// 	dev_dbg(&phy->spi->dev, "\t func_mode: %d.\n", bsync_get_config_resp.func_mode);
// 	dev_dbg(&phy->spi->dev, "\t bsync_div: %d.\n", bsync_get_config_resp.bsync_div);
// 	dev_dbg(&phy->spi->dev, "\t done_flag: %d.\n", bsync_get_config_resp.done_flag);
// 	dev_dbg(&phy->spi->dev, "\t delta_t0: %lld.\n", bsync_get_config_resp.delta_t0);
// 	dev_dbg(&phy->spi->dev, "\t delta_t1: %lld.\n\n", bsync_get_config_resp.delta_t1);

// 	return 0;
// }

// static int ad9088_mcs_init_cal_validate(struct ad9088_phy *phy,
// 					adi_apollo_mcs_cal_init_status_t *cal_status)
// {
// 	int ret = 0;


// 	adi_apollo_device_t *device = &phy->ad9088;
// 	u64 dev_clk_hz = (u64) phy->profile.clk_cfg.dev_clk_freq_kHz * 1000;

// 	// External and Internal Time Difference must be within +/- 0.4 clock cycles
// 	u32 post_cal_init_sysref_diff_cycles;
// 	u64 int_sysref_align_diff = abs(cal_status->data.diff_C_After_femtoseconds - cal_status->data.recommended_offset_C_femtoseconds);
// 	post_cal_init_sysref_diff_cycles = div64_u64(int_sysref_align_diff * dev_clk_hz, 1000000000000ULL);

// 	if (post_cal_init_sysref_diff_cycles > 400) {
// 		adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Time difference between internal and External SysRefs is too large: %u.%02u\n",
// 					post_cal_init_sysref_diff_cycles / 1000, post_cal_init_sysref_diff_cycles % 1000);
// 		ret = -EFAULT;
// 		goto end;
// 	}

// 	// Check if sysref is locked
// 	if (cal_status->data.is_C_Locked != 1) {
// 		adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "MCS Init Cal did not lock SysRefs.\n");
// 		ret = -EFAULT;
// 		goto end;
// 	}

// 	// Check for Cal errors
// 	if (cal_status->hdr.errorCode != 0) {
// 		adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "MCS Init Cal Apollo CPU errorCode: 0x%X.\n", cal_status->hdr.errorCode);
// 		ret = -EFAULT;
// 		goto end;
// 	}

// 	if (cal_status->mcsErr != 0) {
// 		adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "MCS Init Cal MCS errorCode: 0x%X.\n", cal_status->mcsErr);
// 		ret = -EFAULT;
// 		goto end;
// 	}

// end:
// 	adi_apollo_hal_log_write(device, (ret == 0) ? ADI_CMS_LOG_API : ADI_CMS_LOG_ERR,
// 				"MCS Init Cal Validation: %s.\n", (ret == 0) ? "Passed" : "Failed!");

// 	return ret;
// }

// static int ad9088_mcs_tracking_cal_setup(adi_apollo_device_t *device, u16 mcs_track_decimation, bool initialize_track_cal)
// {
// 		int ret;

// 		// Set MCS tracking cal decimation for more precise TDC measurements.
// 		ret = adi_apollo_mcs_cal_tracking_decimation_set(device, mcs_track_decimation);
// 		if (ret)
// 				return ret;

// 		// Enable MCS Tracking Cal. Tracking decimation needs to be updated before this API call.
// 		ret = adi_apollo_mcs_cal_tracking_enable(device, 1);
// 		if (ret)
// 				return ret;

// 		// Initialize MCS Tracking Cal if not done by device profile or mcs_cal_config struct (i.e. cal_config.track_initialize = 1).
// 		if (initialize_track_cal) {
// 			ret = adi_apollo_mcs_cal_tracking_initialize_set(device);
// 			if (ret)
// 					return ret;
// 		}

// 		return 0;
// }

// static ssize_t ad9088_phy_store(struct device *dev,
// 				struct device_attribute *attr,
// 				const char *buf, size_t len)
// {
// 	struct iio_dev *indio_dev = dev_to_iio_dev(dev);
// 	struct iio_dev_attr *this_attr = to_iio_dev_attr(attr);
// 	struct axiadc_converter *conv = iio_device_get_drvdata(indio_dev);
// 	struct ad9088_phy *phy = conv->phy;
// 	adi_apollo_device_t *device = &phy->ad9088;
// 	adi_apollo_side_select_e side;
// 	unsigned long res;
// 	u8 side_index;
// 	bool bres;
// 	bool enable;
// 	int ret = 0;
// 	u16 addr = (u32)this_attr->address & 0xFF;

// 	guard(mutex)(&phy->lock);

// 	switch (addr) {
// 	case AD9088_LOOPBACK1_BLEND_SIDE_A:
// 	case AD9088_LOOPBACK1_BLEND_SIDE_B:
// 		side = addr == AD9088_LOOPBACK1_BLEND_SIDE_B ? ADI_APOLLO_SIDE_B : ADI_APOLLO_SIDE_A;
// 		side_index = side == ADI_APOLLO_SIDE_A ? 0 : 1;
// 		ret = kstrtoul(buf, 0, &res);
// 		if (ret)
// 			return ret;
// 		if (res == 0x2 || res > 0x3)
// 			return -EINVAL;

// 		phy->lb1_blend[side_index] = res;
// 		if (phy->loopback_mode[side_index] == ADI_APOLLO_LOOPBACK_1) {
// 			uint16_t lb1_cducs = side == ADI_APOLLO_SIDE_A ?
// 					     ADI_APOLLO_CDUC_A0 | ADI_APOLLO_CDUC_A1 | ADI_APOLLO_CDUC_A2 | ADI_APOLLO_CDUC_A3 :
// 					     ADI_APOLLO_CDUC_B0 | ADI_APOLLO_CDUC_B1 | ADI_APOLLO_CDUC_B2 | ADI_APOLLO_CDUC_B3;
// 			ret = adi_apollo_loopback_lb1_blend_set(device, lb1_cducs, phy->lb1_blend[side_index]);
// 			if (ret)
// 				return ret;
// 		}
// 		break;
// 	case AD9088_JESD204_FSM_RESUME:
// 		if (!phy->jdev) {
// 			ret = -ENOTSUPP;
// 			break;
// 		}

// 		ret = jesd204_fsm_resume(phy->jdev, JESD204_LINKS_ALL);
// 		break;
// 	case AD9088_JESD204_FSM_CTRL:
// 		if (!phy->jdev) {
// 			ret = -ENOTSUPP;
// 			break;
// 		}

// 		ret = strtobool(buf, &enable);
// 		if (ret)
// 			break;

// 		if (enable) {
// 			jesd204_fsm_stop(phy->jdev, JESD204_LINKS_ALL);
// 			jesd204_fsm_clear_errors(phy->jdev, JESD204_LINKS_ALL);
// 			ret = jesd204_fsm_start(phy->jdev, JESD204_LINKS_ALL);
// 		} else {
// 			jesd204_fsm_stop(phy->jdev, JESD204_LINKS_ALL);
// 			jesd204_fsm_clear_errors(phy->jdev, JESD204_LINKS_ALL);
// 			ret = 0;
// 		}

// 		break;
// 	// case AD9088_POWER_DOWN:
// 	//	ret = strtobool(buf, &enable);
// 	//	if (ret)
// 	//		break;
// 	//	ret = ad9088_set_power_state(phy, !enable);
// 	//	break;
// 	case AD9088_MCS_INIT:
// 		ret = strtobool(buf, &bres);
// 		if (ret < 0)
// 			break;
// 		if (bres)
// 			ret = ad9088_mcs_init_cal_setup(phy);

// 		break;
// 	case AD9088_DT0_MEASUREMENT:
// 		ret = strtobool(buf, &bres);
// 		if (ret < 0)
// 			break;
// 		if (bres)
// 			ret = ad9088_delta_t_measurement_set(phy, 0);
// 		break;
// 	case AD9088_DT1_MEASUREMENT:
// 		ret = strtobool(buf, &bres);
// 		if (ret < 0)
// 			break;
// 		if (bres)
// 			ret = ad9088_delta_t_measurement_set(phy, 1);
// 		break;
// 	case AD9088_DT_MEASUREMENT_RESTORE:
// 		ret = strtobool(buf, &bres);
// 		if (ret < 0)
// 			break;
// 		if (bres)
// 			ret = ad9088_delta_t_measurement_set(phy, 2);
// 		break;
// 	case AD9088_MCS_CAL_RUN:
// 		ret = strtobool(buf, &bres);
// 		if (ret < 0)
// 			break;
// 		if (bres)
// 			ret = adi_apollo_mcs_cal_init_run(device);
// 		break;
// 	case AD9088_MCS_TRACK_CAL_SETUP:
// 		ret = strtobool(buf, &bres);
// 		if (ret < 0)
// 			break;
// 		if (bres)
// 			ret = ad9088_mcs_tracking_cal_setup(device, 1023, 1);

// 		break;
// 	case AD9088_MCS_FG_TRACK_CAL_RUN:
// 		ret = strtobool(buf, &bres);
// 		if (ret < 0)
// 			break;
// 		if (bres)
// 			ret = adi_apollo_mcs_cal_fg_tracking_run(device);

// 		break;
// 	case AD9088_MCS_BG_TRACK_CAL_RUN:
// 		ret = strtobool(buf, &bres);
// 		if (ret < 0)
// 			break;
// 		if (bres)
// 			ret = adi_apollo_mcs_cal_bg_tracking_run(device);
// 		else
// 			ret = adi_apollo_mcs_cal_bg_tracking_abort(device);

// 		if (!ret)
// 			phy->mcs_cal_bg_tracking_run = bres;

// 		break;
// 	case AD9088_MCS_BG_TRACK_CAL_FREEZE:
// 		ret = strtobool(buf, &bres);
// 		if (ret < 0)
// 			break;

// 		if(!phy->mcs_cal_bg_tracking_run) {
// 			dev_err(&phy->spi->dev, "MCS BG Tracking Cal not running.\n");
// 			ret = -EFAULT;
// 			break;
// 		}

// 		if (bres)
// 			ret = adi_apollo_mcs_cal_bg_tracking_freeze(device);
// 		else
// 			ret = adi_apollo_mcs_cal_bg_tracking_unfreeze(device);

// 		if (!ret)
// 			phy->mcs_cal_bg_tracking_freeze = bres;

// 		break;
// 	default:
// 		ret = -EINVAL;
// 	}

// 	return ret ? ret : len;
// }

// static ssize_t ad9088_phy_show(struct device *dev,
// 			       struct device_attribute *attr,
// 			       char *buf)
// {
// 	struct iio_dev *indio_dev = dev_to_iio_dev(dev);
// 	struct iio_dev_attr *this_attr = to_iio_dev_attr(attr);
// 	struct axiadc_converter *conv = iio_device_get_drvdata(indio_dev);
// 	struct ad9088_phy *phy = conv->phy;
// 	struct jesd204_dev *jdev = phy->jdev;
// 	struct jesd204_link *links[4];
// 	adi_apollo_device_t *device = &phy->ad9088;
// 	adi_apollo_mcs_cal_init_status_t init_cal_status = {{0}};
// 	adi_apollo_mcs_cal_status_t tracking_cal_status = {{0}};
// 	s64 delta;
// 	int i, err, num_links, len;
// 	bool paused;
// 	int ret = 0;

// 	guard(mutex)(&phy->lock); /* CHECK */

// 	switch ((u32)this_attr->address & 0xFF) {
// 	case AD9088_LOOPBACK1_BLEND_SIDE_A:
// 		ret = sprintf(buf, "%x\n", phy->lb1_blend[0]);
// 		break;
// 	case AD9088_LOOPBACK1_BLEND_SIDE_B:
// 		ret = sprintf(buf, "%x\n", phy->lb1_blend[1]);
// 		break;
// 	// case AD9088_MCS:
// 	//	ret = sprintf(buf, "%u\n", phy->mcs_cached_val);
// 	//	break;
// 	case AD9088_JESD204_FSM_ERROR:
// 		if (!phy->jdev) {
// 			ret = -ENOTSUPP;
// 			break;
// 		}

// 		num_links = jesd204_get_active_links_num(jdev);
// 		if (num_links < 0) {
// 			ret = num_links;
// 			break;
// 		}

// 		ret = jesd204_get_links_data(jdev, links, num_links);
// 		if (ret)
// 			break;
// 		err = 0;
// 		for (i = 0; i < num_links; i++) {
// 			if (links[i]->error) {
// 				err = links[i]->error;
// 				break;
// 			}
// 		}
// 		ret = sprintf(buf, "%d\n", err);
// 		break;
// 	case AD9088_JESD204_FSM_PAUSED:
// 		if (!phy->jdev) {
// 			ret = -ENOTSUPP;
// 			break;
// 		}

// 		num_links = jesd204_get_active_links_num(jdev);
// 		if (num_links < 0) {
// 			ret = num_links;
// 			break;
// 		}
// 		ret = jesd204_get_links_data(jdev, links, num_links);
// 		if (ret)
// 			break;
// 		/*
// 		 * Take the slowest link; if there are N links and one is
// 		 * paused, all are paused. Not sure if this can happen yet,
// 		 * but best design it like this here.
// 		 */
// 		paused = false;
// 		for (i = 0; i < num_links; i++) {
// 			if (jesd204_link_get_paused(links[i])) {
// 				paused = true;
// 				break;
// 			}
// 		}
// 		ret = sprintf(buf, "%d\n", paused);
// 		break;
// 	case AD9088_JESD204_FSM_STATE:
// 		if (!phy->jdev) {
// 			ret = -ENOTSUPP;
// 			break;
// 		}

// 		num_links = jesd204_get_active_links_num(jdev);
// 		if (num_links < 0) {
// 			ret = num_links;
// 			break;
// 		}

// 		ret = jesd204_get_links_data(jdev, links, num_links);
// 		if (ret)
// 			break;
// 		/*
// 		 * just get the first link state; we're assuming that all 3
// 		 * are in sync and that AD9088_JESD204_FSM_PAUSED
// 		 * was called before
// 		 */
// 		ret = sprintf(buf, "%s\n",
// 			      jesd204_link_get_state_str(links[0]));
// 		break;
// 	case AD9088_JESD204_FSM_CTRL:
// 		if (!phy->jdev) {
// 			ret = -ENOTSUPP;
// 			break;
// 		}

// 		ret = sprintf(buf, "%d\n", phy->is_initialized);
// 		break;
// 	case AD9088_DT0_MEASUREMENT:
// 		ret = ad9088_delta_t_measurement_get(phy, 0, &delta);
// 		ret = sprintf(buf, "%lld\n", delta);
// 		break;
// 	case AD9088_DT1_MEASUREMENT:
// 		ret = ad9088_delta_t_measurement_get(phy, 1, &delta);
// 		ret = sprintf(buf, "%lld\n", delta);
// 		break;
// 	case AD9088_DT_MEASUREMENT_RESTORE:
// 	case AD9088_MCS_TRACK_CAL_SETUP:
// 	case AD9088_MCS_FG_TRACK_CAL_RUN:
// 	case AD9088_MCS_INIT:
// 		ret = 0;
// 		break;
// 	case AD9088_MCS_BG_TRACK_CAL_RUN:
// 		ret = sprintf(buf, "%d\n", phy->mcs_cal_bg_tracking_run);
// 		break;
// 	case AD9088_MCS_BG_TRACK_CAL_FREEZE:
// 		ret = sprintf(buf, "%d\n", phy->mcs_cal_bg_tracking_freeze);
// 		break;
// 	case AD9088_MCS_CAL_RUN:
// 		ret = adi_apollo_mcs_cal_init_status_get(device, &init_cal_status);
// 		if (ret)
// 			break;

// 		ret = ad9088_mcs_init_cal_validate(phy, &init_cal_status);
// 		ret = sprintf(buf, "%s\n", ret ? "Failed" : "Passed");
// 		break;
// 	case AD9088_MCS_INIT_CAL_STATUS:
// 		ret = adi_apollo_mcs_cal_init_status_get(device, &init_cal_status);
// 		if (ret)
// 			break;
// 		ret = ad9088_mcs_init_cal_status_print(phy, buf, &init_cal_status);
// 		break;
// 	case AD9088_MCS_TRACK_STATUS:
// 		if (!phy->mcs_cal_bg_tracking_run) {
// 			ret = -ENOTSUPP;
// 			break;
// 		}

// 		if (!phy->mcs_cal_bg_tracking_freeze) {
// 			// Halt Tracking calibration to pull data from FW.
// 			ret = adi_apollo_mcs_cal_bg_tracking_freeze(device);
// 			if (ret)
// 				break;
// 		}

// 		// Get tracking calibration status info.
// 		ret = adi_apollo_mcs_cal_tracking_status_get(device, &tracking_cal_status);
// 		if (ret)
// 			break;

// 		len = ad9088_mcs_track_cal_status_print(phy, buf, &tracking_cal_status, 1);

// 		if (!phy->mcs_cal_bg_tracking_freeze) {
// 			// Resume Tracking calibration.
// 			ret = adi_apollo_mcs_cal_bg_tracking_unfreeze(device);
// 			if (ret)
// 				break;
// 		}
// 		ret = len;
// 		break;
// 	default:
// 		ret = -EINVAL;
// 	}

// 	return ret;
// }

// static IIO_DEVICE_ATTR(loopback1_blend_side_a, S_IRUGO | S_IWUSR,
// 		       ad9088_phy_show,
// 		       ad9088_phy_store,
// 		       AD9088_LOOPBACK1_BLEND_SIDE_A);

// static IIO_DEVICE_ATTR(loopback1_blend_side_b, S_IRUGO | S_IWUSR,
// 		       ad9088_phy_show,
// 		       ad9088_phy_store,
// 		       AD9088_LOOPBACK1_BLEND_SIDE_B);

// static IIO_CONST_ATTR(loopback1_blend_available, "0 1 3");

// static IIO_DEVICE_ATTR(jesd204_fsm_error, 0444,
// 		       ad9088_phy_show,
// 		       NULL,
// 		       AD9088_JESD204_FSM_ERROR);

// static IIO_DEVICE_ATTR(jesd204_fsm_paused, 0444,
// 		       ad9088_phy_show,
// 		       NULL,
// 		       AD9088_JESD204_FSM_PAUSED);

// static IIO_DEVICE_ATTR(jesd204_fsm_state, 0444,
// 		       ad9088_phy_show,
// 		       NULL,
// 		       AD9088_JESD204_FSM_STATE);

// static IIO_DEVICE_ATTR(jesd204_fsm_resume, 0200,
// 		       NULL,
// 		       ad9088_phy_store,
// 		       AD9088_JESD204_FSM_RESUME);

// static IIO_DEVICE_ATTR(jesd204_fsm_ctrl, 0644,
// 		       ad9088_phy_show,
// 		       ad9088_phy_store,
// 		       AD9088_JESD204_FSM_CTRL);

// static IIO_DEVICE_ATTR(mcs_init, S_IRUGO | S_IWUSR,
// 			ad9088_phy_show,
// 			ad9088_phy_store,
// 			AD9088_MCS_INIT);

// static IIO_DEVICE_ATTR(mcs_dt0_measurement, S_IRUGO | S_IWUSR,
// 			ad9088_phy_show,
// 			ad9088_phy_store,
// 			AD9088_DT0_MEASUREMENT);

// static IIO_DEVICE_ATTR(mcs_dt1_measurement, S_IRUGO | S_IWUSR,
// 			ad9088_phy_show,
// 			ad9088_phy_store,
// 			AD9088_DT1_MEASUREMENT);

// static IIO_DEVICE_ATTR(mcs_dt1_restore, S_IRUGO | S_IWUSR,
// 			ad9088_phy_show,
// 			ad9088_phy_store,
// 			AD9088_DT_MEASUREMENT_RESTORE);

// static IIO_DEVICE_ATTR(mcs_cal_run, S_IRUGO | S_IWUSR,
// 			ad9088_phy_show,
// 			ad9088_phy_store,
// 			AD9088_MCS_CAL_RUN);

// static IIO_DEVICE_ATTR(mcs_tracking_init, S_IRUGO | S_IWUSR,
// 			ad9088_phy_show,
// 			ad9088_phy_store,
// 			AD9088_MCS_TRACK_CAL_SETUP);

// static IIO_DEVICE_ATTR(mcs_fg_tacking_cal_run, S_IRUGO | S_IWUSR,
// 			ad9088_phy_show,
// 			ad9088_phy_store,
// 			AD9088_MCS_FG_TRACK_CAL_RUN);

// static IIO_DEVICE_ATTR(mcs_bg_tacking_cal_run, S_IRUGO | S_IWUSR,
// 			ad9088_phy_show,
// 			ad9088_phy_store,
// 			AD9088_MCS_BG_TRACK_CAL_RUN);

// static IIO_DEVICE_ATTR(mcs_bg_tacking_cal_freeze, S_IRUGO | S_IWUSR,
// 			ad9088_phy_show,
// 			ad9088_phy_store,
// 			AD9088_MCS_BG_TRACK_CAL_FREEZE);

// static IIO_DEVICE_ATTR(mcs_tracking_status, S_IRUGO,
// 			ad9088_phy_show,
// 			NULL,
// 			AD9088_MCS_TRACK_STATUS);

// static IIO_DEVICE_ATTR(mcs_init_cal_status, S_IRUGO,
// 			ad9088_phy_show,
// 			NULL,
// 			AD9088_MCS_INIT_CAL_STATUS);

// /**
//  * ad9088_phy_attributes - Array of sysfs attributes for the AD9088 PHY driver
//  *
//  * This array contains the sysfs attributes that are exposed by the AD9088 PHY
//  * driver. These attributes allow user-space applications to interact with and
//  * configure various aspects of the AD9088 device.
//  *
//  * Attributes:
//  * - loopback1_blend_side_a: Controls the blend setting for loopback1 on side A.
//  * - loopback1_blend_side_b: Controls the blend setting for loopback1 on side B.
//  * - loopback1_blend_available: Indicates the available blend settings for loopback1.
//  * - jesd204_fsm_error: Reports the error status of the JESD204 finite state machine.
//  * - jesd204_fsm_state: Reports the current state of the JESD204 finite state machine.
//  * - jesd204_fsm_paused: Indicates whether the JESD204 finite state machine is paused.
//  * - jesd204_fsm_resume: Controls the resume operation of the JESD204 finite state machine.
//  * - jesd204_fsm_ctrl: Provides control over the JESD204 finite state machine.
//  * - mcs_init: Initiates the multi-chip synchronization (MCS) process.
//  * - mcs_dt0_measurement: Measures the delay time for MCS delta_t 0.
//  * - mcs_dt1_measurement: Measures the delay time for MCS delta_t 1.
//  * - mcs_dt1_restore: Restores the delay time for MCS delta_t 1.
//  * - mcs_cal_run: Runs the MCS calibration process.
//  * - mcs_tracking_init: Initializes the MCS tracking process.
//  * - mcs_fg_tacking_cal_run: Runs the foreground tracking calibration for MCS.
//  * - mcs_bg_tacking_cal_run: Runs the background tracking calibration for MCS.
//  * - mcs_bg_tacking_cal_freeze: Freezes the background tracking calibration for MCS.
//  * - mcs_tracking_status: Reports the status of the MCS tracking process.
//  * - mcs_init_cal_status: Reports the status of the initial MCS calibration.
//  */

// static struct attribute *ad9088_phy_attributes[] = {
// 	&iio_dev_attr_loopback1_blend_side_a.dev_attr.attr,
// 	&iio_dev_attr_loopback1_blend_side_b.dev_attr.attr,
// 	&iio_const_attr_loopback1_blend_available.dev_attr.attr,
// 	&iio_dev_attr_jesd204_fsm_error.dev_attr.attr,
// 	&iio_dev_attr_jesd204_fsm_state.dev_attr.attr,
// 	&iio_dev_attr_jesd204_fsm_paused.dev_attr.attr,
// 	&iio_dev_attr_jesd204_fsm_resume.dev_attr.attr,
// 	&iio_dev_attr_jesd204_fsm_ctrl.dev_attr.attr,
// 	&iio_dev_attr_mcs_init.dev_attr.attr,
// 	&iio_dev_attr_mcs_dt0_measurement.dev_attr.attr,
// 	&iio_dev_attr_mcs_dt1_measurement.dev_attr.attr,
// 	&iio_dev_attr_mcs_dt1_restore.dev_attr.attr,
// 	&iio_dev_attr_mcs_cal_run.dev_attr.attr,
// 	&iio_dev_attr_mcs_tracking_init.dev_attr.attr,
// 	&iio_dev_attr_mcs_fg_tacking_cal_run.dev_attr.attr,
// 	&iio_dev_attr_mcs_bg_tacking_cal_run.dev_attr.attr,
// 	&iio_dev_attr_mcs_bg_tacking_cal_freeze.dev_attr.attr,
// 	&iio_dev_attr_mcs_tracking_status.dev_attr.attr,
// 	&iio_dev_attr_mcs_init_cal_status.dev_attr.attr,
// 	NULL,
// };

// static const struct attribute_group ad9088_phy_attribute_group = {
// 	.attrs = ad9088_phy_attributes,
// };

// static int ad9088_request_fd_irqs(struct axiadc_converter *conv)
// {
// 	struct device *dev = &conv->spi->dev;
// 	struct gpio_desc *gpio;

// 	gpio = devm_gpiod_get(dev, "fastdetect-a", GPIOD_IN);
// 	if (!IS_ERR(gpio)) {
// 		int ret, irq = gpiod_to_irq(gpio);

// 		if (irq < 0)
// 			return irq;

// 		ret = devm_request_threaded_irq(dev,
// 						irq, NULL, ad9088_fdA_handler,
// 						IRQF_TRIGGER_RISING | IRQF_ONESHOT,
// 						"fastdetect-a", conv);
// 		if (ret < 0)
// 			return ret;
// 	}

// 	gpio = devm_gpiod_get(dev, "fastdetect-b", GPIOD_IN);
// 	if (!IS_ERR(gpio)) {
// 		int ret, irq = gpiod_to_irq(gpio);

// 		if (irq < 0)
// 			return irq;

// 		ret = devm_request_threaded_irq(dev,
// 						irq, NULL, ad9088_fdB_handler,
// 						IRQF_TRIGGER_RISING | IRQF_ONESHOT,
// 						"fastdetect-b", conv);
// 		if (ret < 0)
// 			return ret;
// 	}

// 	return 0;
// }

// static int ad9088_post_setup(struct iio_dev *indio_dev)
// {
// 	struct axiadc_state *st = iio_priv(indio_dev);
// 	struct axiadc_converter *conv = iio_device_get_drvdata(indio_dev);
// 	int i;

// 	for (i = 0; i < conv->chip_info->num_channels; i++) {
// 		axiadc_write(st, ADI_REG_CHAN_CNTRL_2(i),
// 			     (i & 1) ? 0x00004000 : 0x40000000);
// 		axiadc_write(st, ADI_REG_CHAN_CNTRL(i),
// 			     ADI_FORMAT_SIGNEXT | ADI_FORMAT_ENABLE |
// 			     ADI_IQCOR_ENB | ADI_ENABLE);
// 	}

// 	return 0;
// }

// static int ad9088_status_show(struct seq_file *file, void *offset)
// {
// 	struct axiadc_converter *conv = spi_get_drvdata(file->private);
// 	struct ad9088_phy *phy = conv->phy;
// 	adi_apollo_device_t *device = &phy->ad9088;
// 	adi_apollo_jesd_rx_inspect_t jrx_status;
// 	adi_apollo_jesd_tx_inspect_t jtx_status;
// 	uint16_t links_to_inspect[] = { ADI_APOLLO_LINK_A0, ADI_APOLLO_LINK_A1, ADI_APOLLO_LINK_B0, ADI_APOLLO_LINK_B1 };
// 	const char * links_to_inspect_str[] = { "A0", "A1", "B0", "B1" };
// 	int l, i, ret;
// 	u16 stat, l_stat, mask;

// 	for (l = 0; l < sizeof(links_to_inspect) / sizeof(links_to_inspect[0]); l++) {
// 		ret = adi_apollo_jrx_link_inspect(device, links_to_inspect[l], &jrx_status);
// 		if (ret) {
// 			dev_err(&phy->spi->dev,"Error from adi_apollo_jrx_link_inspect() %d", ret);
// 			return ret;
// 		}

// 		if (!jrx_status.np_minus1)
// 			continue;

// 		seq_printf(file,"JRX ADI_APOLLO_LINK_%s: JESD204%c Subclass=%c L=%d M=%d F=%d S=%d Np=%d CS=%d link_en=%-8s\n",
// 			   links_to_inspect_str[l],
// 			   jrx_status.ver == ADI_APOLLO_JESD_204C ? 'C' : 'B',
// 			   jrx_status.subclass ? '1' : '0',
// 			   jrx_status.l_minus1+1,
// 			   jrx_status.m_minus1+1,
// 			   jrx_status.f_minus1+1,
// 			   jrx_status.s_minus1+1,
// 			   jrx_status.np_minus1+1,
// 			   jrx_status.cs,
// 			   jrx_status.link_en ? "Enabled" : "Disabled");

// 		ret = adi_apollo_jrx_link_status_get(&phy->ad9088, links_to_inspect[l], &stat);
// 		if (ret)
// 			return -EFAULT;

// 		if (jrx_status.ver == ADI_APOLLO_JESD_204C) {
// 			for (i = 0; i < /*jrx_status.l_minus1 + 1*/ 12; i++) {
// 				adi_apollo_jrx_j204c_lane_status_get(&phy->ad9088, links_to_inspect[l], i, &l_stat);
// 				seq_printf(file, "    Lane%d status: %s\n", i, ad9088_jrx_204c_states[l_stat & 0x7]);
// 			}

// 			seq_printf(file, "    User status: %s, SYSREF Phase: %s\n",
// 				   (stat & 0x20) ? "Ready" :  "Fail",
// 				   (stat & 0x40) ? "Locked" :  "Unlocked");

// 		} else {
// 			mask = (1 <<  (jrx_status.l_minus1 + 1)) - 1;
// 			stat = mask & stat;
// 			seq_printf(file, "    0x%X lanes in DATA\n", stat);
// 		}

// 	}

// 	for (l = 0; l < sizeof(links_to_inspect) / sizeof(links_to_inspect[0]); l++) {
// 		ret = adi_apollo_jtx_link_inspect(device, links_to_inspect[l], &jtx_status);
// 		if (ret) {
// 			dev_err(&phy->spi->dev,"Error from adi_apollo_jtx_link_inspect() %d", ret);
// 			return ret;
// 		}

// 		if (!jtx_status.np_minus1)
// 			continue;

// 		seq_printf(file, "JTX ADI_APOLLO_LINK_%s: JESD204%c Subclass=%c L=%d M=%d F=%d S=%d Np=%d CS=%d link_en=%-8s\n",
// 			   links_to_inspect_str[l],
// 			   jtx_status.ver == ADI_APOLLO_JESD_204C ? 'C' : 'B',
// 			   jtx_status.subclass ? '1' : '0',
// 			   jtx_status.l_minus1+1,
// 			   jtx_status.m_minus1+1,
// 			   jtx_status.f_minus1+1,
// 			   jtx_status.s_minus1+1,
// 			   jtx_status.np_minus1+1,
// 			   jtx_status.cs,
// 			   jtx_status.link_en ? "Enabled" : "Disabled");

// 		ret = adi_apollo_jtx_link_status_get(&phy->ad9088, links_to_inspect[l], &stat);
// 		if (ret)
// 			return -EFAULT;

// 		if (jtx_status.ver == ADI_APOLLO_JESD_204C)
// 			seq_printf(file,
// 				   "    PLL %s, PHASE %s, MODE %s\n",
// 				   stat & BIT(5) ? "locked" : "unlocked",
// 				   stat & BIT(6) ? "established" : "lost",
// 				   stat & BIT(7) ? "invalid" : "valid");
// 		else
// 			seq_printf(file,
// 				   "    State: %s, SYNC %s, PLL %s, PHASE %s, MODE %s\n",
// 				   ad9088_jtx_qbf_states[stat & 0xF],
// 				   stat & BIT(4) ? "deasserted" : "asserted",
// 				   stat & BIT(5) ? "locked" : "unlocked",
// 				   stat & BIT(6) ? "established" : "lost",
// 				   stat & BIT(7) ? "invalid" : "valid");
// 	}

// 	return 0;
// }

// static void ad9088_work_func(struct work_struct *work)
// {
// 	//u8 status;
// 	//int ret;
// 	struct ad9088_phy *phy =
// 		container_of(work, struct ad9088_phy, dwork.work);

// 	// ret = adi_ad9088_hal_reg_get(&phy->ad9088, REG_IRQ_STATUS0_ADDR,
// 	//			     &status);

// 	// if (!(status & BIT(6))) {
// 	//	dev_err(&phy->spi->dev, "Link Reset IRQ_STATUS0: 0x%X\n", status);

// 	//	phy->jrx_link_tx[0].lane_cal_rate_kbps = 0;
// 	//	jesd204_fsm_stop(phy->jdev, JESD204_LINKS_ALL);
// 	//	jesd204_fsm_clear_errors(phy->jdev, JESD204_LINKS_ALL);
// 	//	jesd204_fsm_start(phy->jdev, JESD204_LINKS_ALL);

// 	//	return;
// 	// }

// 	schedule_delayed_work(&phy->dwork, msecs_to_jiffies(1000));
// }

// static int ad9088_fsc_set(void *arg, const u64 val)
// {
// 	struct iio_dev *indio_dev = arg;
// 	struct axiadc_converter *conv = iio_device_get_drvdata(indio_dev);
// 	struct ad9088_phy *phy = conv->phy;
// 	int ret = 0;

// 	if (!val)
// 		return -EINVAL;

// 	guard(mutex)(&phy->lock);
// 	//ret = adi_ad9088_dac_fsc_set(&phy->ad9088, AD9088_DAC_ALL, val, 1);

// 	return ret;
// }
// DEFINE_DEBUGFS_ATTRIBUTE(ad9088_fsc_fops, NULL, ad9088_fsc_set, "%llu");

// static const char* const pfir_filter_modes[] = {
// 	"disabled", "real_n4", "real_n2", "undef",
// 	"matrix", "undef", "complex_half", "real_n"
// };

// static const char* const pfir_selects[] = {
// 	"pfilt_a0", "pfilt_a1", "pfilt_b0", "pfilt_b1", "pfilt_all", "pfilt_mask"
// };

// static const char* const pfir_filter_banks[] = {
// 	"bank_0", "bank_1", "bank_2", "bank_3", "bank_all", "bank_mask"
// };

// static const char* const terminals[] = {
// 	"rx", "tx"
// };

// static const char* const pfilt_profile_selection_mode[] = {
// 	"direct_regmap", "direct_gpio","direct_gpio1", "trig_regmap", "trig_gpio", "trig_gpio1"
// };

// static u32 ad9088_pfir_gain_enc(int val)
// {
// 	switch (val) {
// 	case 0:
// 		return ADI_APOLLO_PFILT_GAIN_ZERO_DB;
// 	case 6:
// 		return ADI_APOLLO_PFILT_GAIN_POS_6_DB;
// 	case 12:
// 		return ADI_APOLLO_PFILT_GAIN_POS_12_DB;
// 	case 18:
// 		return ADI_APOLLO_PFILT_GAIN_POS_18_DB;
// 	case 24:
// 		return ADI_APOLLO_PFILT_GAIN_POS_24_DB;
// 	case -24:
// 		return ADI_APOLLO_PFILT_GAIN_NEG_24_DB;
// 	case -18:
// 		return ADI_APOLLO_PFILT_GAIN_NEG_18_DB;
// 	case -12:
// 		return ADI_APOLLO_PFILT_GAIN_NEG_12_DB;
// 	case -6:
// 		return ADI_APOLLO_PFILT_GAIN_NEG_6_DB;
// 	default:
// 		return ADI_APOLLO_PFILT_GAIN_ZERO_DB;
// 	}
// };


// /**
//  * ad9088_parse_pfilt - Parse the configuration data for the PFIR filter
//  * @phy: Pointer to the AD9088 PHY structure
//  * @data: Pointer to the configuration data
//  * @size: Size of the configuration data
//  *
//  * This function parses the configuration data for the PFIR (Programmable
//  * Finite Impulse Response) filter of the AD9088 device. The configuration
//  * data is expected to be in a specific format, which is documented below.
//  *
//  * Format of the configuration data:
//  *   - Each line represents a parameter or setting for the PFIR filter.
//  *   - Lines starting with '#' are considered comments and are ignored.
//  *   - The format for each parameter is as follows:
//  *     - mode: <imode> <qmode>
//  *       - Sets the mode for the PFIR filter. The <imode> and <qmode> values
//  *         should be one of the predefined filter modes: "disabled",
//  *         "real_n4", "real_n2", "undef", "matrix", "undef", "complex_half",
//  *         "real_n".
//  *     - gain: <ix> <iy> <qx> <qy>
//  *       - Sets the gain values for the PFIR filter. The <ix>, <iy>, <qx>,
//  *         and <qy> values should be integers representing the gain in dB.
//  *     - scalar_gain: <ix> <iy> <qx> <qy>
//  *       - Sets the scalar gain values for the PFIR filter. The <ix>, <iy>,
//  *         <qx>, and <qy> values should be integers representing the scalar
//  *         gain.
//  *     - dest: <terminal> <pfilt_sel> <bank_sel>
//  *       - Sets the destination for the PFIR filter. The <terminal> value
//  *         should be either "rx" or "tx". The <pfilt_sel> value should be one
//  *         of the predefined filter selects: "pfilt_a0", "pfilt_a1",
//  *         "pfilt_b0", "pfilt_b1", "pfilt_all", "pfilt_mask". The <bank_sel>
//  *         value should be one of the predefined filter banks: "bank_0",
//  *         "bank_1", "bank_2", "bank_3", "bank_all", "bank_mask".
//  *     - hc_delay: <delay>
//  *       - Sets the high cut delay value for the PFIR filter. The <delay>
//  *         value should be an unsigned 8-bit integer.
//  *     - mode_switch_en: <value>
//  *       - Sets the mode switch enable value for the PFIR filter. The <value>
//  *         should be either 0 or 1.
//  *     - mode_switch_add_en: <value>
//  *       - Sets the mode switch add enable value for the PFIR filter. The
//  *         <value> should be either 0 or 1.
//  *     - real_data_mode_en: <value>
//  *       - Sets the real data mode enable value for the PFIR filter. The
//  *         <value> should be either 0 or 1.
//  *     - quad_mode_en: <value>
//  *       - Sets the quad mode enable value for the PFIR filter. The <value>
//  *         should be either 0 or 1.
//  *     - selection_mode: <mode>
//  *       - Sets the profile selection mode for the PFIR filter. The <mode>
//  *         value should be one of the predefined profile selection modes:
//  *         "direct_regmap", "direct_gpio", "direct_gpio1", "trig_regmap",
//  *         "trig_gpio", "trig_gpio1".
//  *     - <sval>
//  *       - Sets the coefficient values for the PFIR filter. The <sval> value
//  *         should be an integer representing the coefficient value.
//  *
//  * Return: 0 on success, negative error code on failure
//  */
// static int ad9088_parse_pfilt(struct ad9088_phy *phy,
// 				 char *data, u32 size)
// {
// 	struct device *dev = &phy->spi->dev;
// 	char *line;
// 	int i = 0, ret, j;
// 	char *ptr = data;
// 	uint16_t pfilt_coeffs[ADI_APOLLO_PFILT_COEFF_NUM];
// 	u32 val;
// 	s32 sval;

// 	adi_apollo_terminal_e terminal;
// 	adi_apollo_pfilt_bank_sel_e bank_sel;
// 	adi_apollo_pfilt_sel_e pfilt_sel;
// 	adi_apollo_pfilt_profile_sel_mode_e selection_mode;

// 	adi_apollo_pfilt_mode_pgm_t pfilt_mode_pgm = { 0 };
// 	adi_apollo_pfilt_gain_dly_pgm_t gain_dly_pgm = {
// 		.pfir_iy_gain = ADI_APOLLO_PFILT_GAIN_ZERO_DB,  /* Gain can be +/-24dB in 6dB steps. */
// 		.pfir_iy_scalar_gain = 0x3f,                    /* code = 64*scalarGain-1. 1/64 <= scalarGain <= 1. (code 6'h0 1/64(-36dB), 6'h1 1/32(-30dB), 6'h3f 1(0dB)) */
// 		.pfir_qx_gain = ADI_APOLLO_PFILT_GAIN_ZERO_DB,  /* Gain can be +/-24dB in 6dB steps */
// 		.pfir_qx_scalar_gain = 0x3f,                    /* code = 64*scalarGain-1. 1/64 <= scalarGain <= 1. (code 6'h0 1/64(-36dB), 6'h1 1/32(-30dB), 6'h3f 1(0dB)) */
// 	};
// 	u8 read_mask = 0;

// 	while ((line = strsep(&ptr, "\n"))) {
// 		if (line >= data + size) {
// 			break;
// 		}

// 		if (line[0] == '#')
// 			continue;

// 		if (~read_mask & BIT(0)) {
// 			char imode[16], qmode[16];

// 			ret = sscanf(line, "mode: %15s %15s",
// 				     imode, qmode);

// 			if (ret == 2)
// 				pfilt_mode_pgm.pfir_q_mode = sysfs_match_string(pfir_filter_modes, qmode);

// 			if (ret == 1 || ret == 2) {
// 				pfilt_mode_pgm.pfir_i_mode = sysfs_match_string(pfir_filter_modes, imode);
// 				read_mask |= BIT(0);
// 				continue;
// 			}
// 		}

// 		if (~read_mask & BIT(1)) {
// 			int ix = 0, iy = 0, qx = 0, qy = 0;

// 			ret = sscanf(line, "gain: %d %d %d %d",
// 				     &ix, &iy, &qx, &qy);

// 			if (ret == 4) {
// 				gain_dly_pgm.pfir_ix_gain = ad9088_pfir_gain_enc(ix);
// 				gain_dly_pgm.pfir_iy_gain = ad9088_pfir_gain_enc(iy);
// 				gain_dly_pgm.pfir_qx_gain = ad9088_pfir_gain_enc(qx);
// 				gain_dly_pgm.pfir_qy_gain = ad9088_pfir_gain_enc(qy);

// 				read_mask |= BIT(1);
// 				continue;
// 			}
// 		}

// 		if (~read_mask & BIT(4)) {
// 			ret = sscanf(line, "scalar_gain: %hhu %hhu %hhu %hhu",
// 				     &gain_dly_pgm.pfir_ix_scalar_gain,
// 				     &gain_dly_pgm.pfir_iy_scalar_gain,
// 				     &gain_dly_pgm.pfir_qx_scalar_gain,
// 				     &gain_dly_pgm.pfir_qy_scalar_gain);

// 			if (ret == 4) {
// 				read_mask |= BIT(4);
// 				continue;
// 			}
// 		}

// 		if (~read_mask & BIT(2)) {
// 			char t[4], p[16], b[16];
// 			u32 mask;

// 			ret = sscanf(line, "dest: %3s %15s %15s", t, p, b);

// 			if (ret == 3) {
// 				ret = sysfs_match_string(terminals, t);
// 				if (ret < 0)
// 					goto out;
// 				terminal = ret ? ADI_APOLLO_TX : ADI_APOLLO_RX;

// 				ret = sysfs_match_string(pfir_selects, p);
// 				if (ret < 0)
// 					goto out;
// 				pfilt_sel = 1 << ret;
// 				if (ret == 4)
// 					pfilt_sel = ADI_APOLLO_PFILT_ALL;
// 				if (ret == 5) {
// 					ret = sscanf(line, "pfilt_mask%u", &mask);
// 					if (ret != 1)
// 						goto out;
// 					pfilt_sel = mask;
// 				}

// 				ret = sysfs_match_string(pfir_filter_banks, b);
// 				if (ret < 0)
// 					goto out;
// 				bank_sel = 1 << ret;
// 				if (ret == 4)
// 					bank_sel = ADI_APOLLO_PFILT_BANK_ALL;
// 				if (ret == 5) {
// 					ret = sscanf(line, "bank_mask%u", &mask);
// 					if (ret != 1)
// 						goto out;
// 					bank_sel = mask;
// 				}

// 				read_mask |= BIT(2);
// 				continue;
// 			}
// 		}

// 		if (~read_mask & BIT(3)) {
// 			ret = sscanf(line, "hc_delay: %hhu", &gain_dly_pgm.hc_delay);
// 			if (ret == 1) {
// 				read_mask |= BIT(3);
// 				continue;
// 			}
// 		}

// 		if (~read_mask & BIT(5)) {
// 			ret = sscanf(line, "mode_switch_en: %u", &val);
// 			if (ret == 1) {
// 				pfilt_mode_pgm.mode_switch = val ? ADI_APOLLO_PFILT_ENABLE_3DB_AVG_MOD_SW : ADI_APOLLO_PFILT_DISABLE_3DB_AVG_MOD_SW;
// 				read_mask |= BIT(5);
// 				continue;
// 			}
// 		}

// 		if (~read_mask & BIT(6)) {
// 			ret = sscanf(line, "mode_switch_add_en: %u", &val);
// 			if (ret == 1) {
// 				pfilt_mode_pgm.add_sub_sel = val ? ADI_APOLLO_PFILT_ADD_FOR_MOD_SW : ADI_APOLLO_PFILT_SUB_FOR_MOD_SW;
// 				read_mask |= BIT(6);
// 				continue;
// 			}
// 		}

// 		if (~read_mask & BIT(7)) {
// 			ret = sscanf(line, "real_data_mode_en: %u", &val);
// 			if (ret == 1) {
// 				pfilt_mode_pgm.data = val ? ADI_APOLLO_PFILT_REAL_DATA : ADI_APOLLO_PFILT_COMPLEX_DATA;
// 				read_mask |= BIT(7);
// 				continue;
// 			}
// 		}

// 		if (~read_mask & BIT(8)) {
// 			ret = sscanf(line, "quad_mode_en: %u", &val);
// 			if (ret == 1) {
// 				pfilt_mode_pgm.dq_mode = val ? ADI_APOLLO_PFILT_QUAD_MODE : ADI_APOLLO_PFILT_DUAL_MODE;
// 				read_mask |= BIT(8);
// 				continue;
// 			}
// 		}

// 		if (~read_mask & BIT(9)) {
// 			char m[16];
// 			ret = sscanf(line, "selection_mode: %s", m);
// 			if (ret == 1) {
// 				selection_mode = sysfs_match_string(pfilt_profile_selection_mode, m);
// 				if (selection_mode < 0)
// 					goto out;
// 				read_mask |= BIT(9);
// 				continue;
// 			}
// 		}

// 		ret = sscanf(line, "%i", &sval);
// 		if (ret == 1) {
// 			if (i >= ADI_APOLLO_PFILT_COEFF_NUM)
// 				return -EINVAL;

// 			pfilt_coeffs[i++] = (u16)sval;
// 			continue;
// 		}
// 	}

// 	dev_dbg(dev,"terminal: %s\n", terminals[terminal]);
// 	dev_dbg(dev,"pfilt_sel: MASK 0x%X\n", pfilt_sel);
// 	dev_dbg(dev,"bank_sel: MASK 0x%x\n", bank_sel);

// 	dev_dbg(dev,"pfilt_mode_pgm.pfir_i_mode: %s\n", pfir_filter_modes[pfilt_mode_pgm.pfir_i_mode]);
// 	dev_dbg(dev,"pfilt_mode_pgm.pfir_q_mode: %s\n", pfir_filter_modes[pfilt_mode_pgm.pfir_q_mode]);
// 	dev_dbg(dev,"pfilt_mode_pgm.mode_switch: %d\n", pfilt_mode_pgm.mode_switch);
// 	dev_dbg(dev,"pfilt_mode_pgm.add_sub_sel: %d\n", pfilt_mode_pgm.add_sub_sel);
// 	dev_dbg(dev,"pfilt_mode_pgm.data: %d\n", pfilt_mode_pgm.data);
// 	dev_dbg(dev,"pfilt_mode_pgm.dq_mode: %d\n", pfilt_mode_pgm.dq_mode);

// 	ret = adi_apollo_pfilt_mode_pgm(&phy->ad9088, terminal, pfilt_sel, &pfilt_mode_pgm);
// 	if (ret < 0)
// 		goto out1;

// 	dev_dbg(dev,"gain_dly_pgm.pfir_ix_gain: %d\n", gain_dly_pgm.pfir_ix_gain);
// 	dev_dbg(dev,"gain_dly_pgm.pfir_iy_gain: %d\n", gain_dly_pgm.pfir_iy_gain);
// 	dev_dbg(dev,"gain_dly_pgm.pfir_qx_gain: %d\n", gain_dly_pgm.pfir_qx_gain);
// 	dev_dbg(dev,"gain_dly_pgm.pfir_qy_gain: %d\n", gain_dly_pgm.pfir_qy_gain);
// 	dev_dbg(dev,"gain_dly_pgm.pfir_ix_scalar_gain: %hhu\n", gain_dly_pgm.pfir_ix_scalar_gain);
// 	dev_dbg(dev,"gain_dly_pgm.pfir_iy_scalar_gain: %hhu\n", gain_dly_pgm.pfir_iy_scalar_gain);
// 	dev_dbg(dev,"gain_dly_pgm.pfir_qx_scalar_gain: %hhu\n", gain_dly_pgm.pfir_qx_scalar_gain);
// 	dev_dbg(dev,"gain_dly_pgm.pfir_qy_scalar_gain: %hhu\n", gain_dly_pgm.pfir_qy_scalar_gain);
// 	dev_dbg(dev,"gain_dly_pgm.hc_delay: %hhu\n", gain_dly_pgm.hc_delay);
// 	dev_dbg(dev,"selection_mode: %u\n", selection_mode);

// 	ret = adi_apollo_pfilt_gain_dly_pgm(&phy->ad9088, terminal, pfilt_sel, bank_sel, &gain_dly_pgm);
// 	if (ret < 0)
// 		goto out1;

// 	if (read_mask & BIT(9)) {
// 		ret = adi_apollo_pfilt_profile_sel_mode_set(&phy->ad9088, terminal, pfilt_sel, selection_mode);
// 		if (ret < 0)
// 			goto out1;
// 	}

// 	for (j = 0; j < i; j++) {
// 		dev_dbg(dev,"0x%X\n", pfilt_coeffs[j]);
// 	}

// 	ret = adi_apollo_pfilt_coeff_pgm(&phy->ad9088, terminal, pfilt_sel, bank_sel, pfilt_coeffs, i);
// 	if (ret < 0)
// 		goto out1;
// 	ret = adi_apollo_pfilt_coeff_transfer(&phy->ad9088, terminal, pfilt_sel, bank_sel);
// 	if (ret < 0)
// 		goto out1;

// out1:
// 	if (ret != API_CMS_ERROR_OK)
// 		dev_err(&phy->spi->dev, "Prgramming filter failed (%d)", ret);

// 	return size;

// out:
// 	dev_err(dev, "malformed pFir filter file detected\n");

// 	return -EINVAL;
// }

// static ssize_t
// ad9088_pfilt_bin_write(struct file *filp, struct kobject *kobj,
// 		       struct bin_attribute *bin_attr,
// 		       char *buf, loff_t off, size_t count)
// {

// 	struct iio_dev *indio_dev = dev_to_iio_dev(kobj_to_dev(kobj));
// 	struct axiadc_converter *conv = iio_device_get_drvdata(indio_dev);
// 	struct ad9088_phy *phy = conv->phy;

// 	guard(mutex)(&phy->lock);

// 	return ad9088_parse_pfilt(phy, buf, count);
// }

// /* CFIR */

// static const char* const cfir_selects[] = {
// 	"cfir_none", "cfir_a0", "cfir_a1", "cfir_b0",
// 	"cfir_b1", "cfir_all", "cfir_mask"
// };

// static const char* const cfir_profiles[] = {
// 	"profile_none", "profile_1", "profile_2", "profile_all"
// };

// static const char* const cfir_datapaths[] = {
// 	"datapath_none", "datapath_0", "datapath_1", "datapath_2",
// 	"datapath_3", "datapath_all", "datapath_mask"
// };

// static const char* const cfir_profile_selection_mode[] = {
// 	"direct_regmap", "direct_gpio", "trig_regmap", "trig_gpio"
// };


// static u32 ad9088_cfir_gain_enc(int val)
// {
// 	switch (val) {
// 	case 0:
// 		return ADI_APOLLO_CFIR_GAIN_ZERO_DB;
// 	case 6:
// 		return ADI_APOLLO_CFIR_GAIN_PLUS6_DB;
// 	case 12:
// 		return ADI_APOLLO_CFIR_GAIN_PLUS12_DB;
// 	case -18:
// 		return ADI_APOLLO_CFIR_GAIN_MINUS18_DB;
// 	case -12:
// 		return ADI_APOLLO_CFIR_GAIN_MINUS12_DB;
// 	case -6:
// 		return ADI_APOLLO_CFIR_GAIN_MINUS6_DB;
// 	default:
// 		return ADI_APOLLO_CFIR_GAIN_ZERO_DB;
// 	}
// };
// /**
//  * ad9088_parse_cfilt - Parse the CFIR filter configuration from a string
//  * @phy: Pointer to the ad9088_phy structure
//  * @data: Pointer to the string containing the CFIR filter configuration
//  * @size: Size of the string
//  *
//  * This function parses the CFIR filter configuration from a string and updates
//  * the ad9088_phy structure accordingly. The string should have the following
//  * format:
//  *
//  * dest: <terminal> <cfir_select> <cfir_profile> <cfir_datapath>
//  * gain: <gain_value>
//  * complex_scalar: <scalar_i> <scalar_q>
//  * bypass: <bypass_value>
//  * sparse_filt_en: <sparse_filt_en_value>
//  * 32taps_en: <32taps_en_value>
//  * coeff_transfer: <coeff_transfer_value>
//  * enable: <enable_value> <enable_profile_value>
//  * selection_mode: <selection_mode_value>
//  * <cfir_coeff0_i> <cfir_coeff0_q>
//  * <cfir_coeff1_i> <cfir_coeff1_q>
//  * ...
//  * <cfir_coeffN_i> <cfir_coeffN_q>
//  *
//  * Each line in the string represents a specific configuration parameter.
//  * The "dest" line specifies the destination terminal, CFIR select, CFIR profile,
//  * and CFIR datapath. The "gain" line specifies the gain value. The "complex_scalar"
//  * line specifies the complex scalar values. The "bypass" line specifies the bypass
//  * value. The "sparse_filt_en" line specifies the sparse filter enable value.
//  * The "32taps_en" line specifies the 32 taps enable value. The "coeff_transfer"
//  * line specifies the coefficient transfer value. The "enable" line specifies the
//  * enable value and enable profile value. The "selection_mode" line specifies the
//  * selection mode value. The <cfir_coeff_i> and <cfir_coeff_q> lines specify the
//  * CFIR coefficient values.
//  *
//  * Return: 0 on success, negative error code on failure
//  */
// static int ad9088_parse_cfilt(struct ad9088_phy *phy,
// 				 char *data, u32 size)
// {
// 	struct device *dev = &phy->spi->dev;
// 	char *line;
// 	int i = 0, ret, j;
// 	char *ptr = data;
// 	uint16_t cfir_coeff_i[ADI_APOLLO_CFIR_COEFF_NUM];
// 	uint16_t cfir_coeff_q[ADI_APOLLO_CFIR_COEFF_NUM];
// 	u32 val, enable, enable_profile, selection_mode;
// 	s32 sval_i, sval_q, gain = 0;
// 	adi_apollo_terminal_e terminal;
// 	adi_apollo_cfir_sel_e cfir_sel;
// 	adi_apollo_cfir_dp_sel dp_sel;
// 	adi_apollo_cfir_profile_sel_e profile_sel;
// 	adi_apollo_cfir_pgm_t cfir_pgm = { 0 };
// 	u16 scalar_i, scalar_q;
// 	u8 read_mask = 0;

// 	while ((line = strsep(&ptr, "\n"))) {
// 		if (line >= data + size) {
// 			break;
// 		}

// 		if (line[0] == '#') /* skip comments */
// 			continue;

// 		if (~read_mask & BIT(0)) {
// 			char t[4], s[16], p[16], d[21];
// 			u32 mask;
// 			/* dest: rx cfir_a0 profile_1 datapath_0 */
// 			ret = sscanf(line, "dest: %3s %15s %15s %20s", t, s, p, d);
// 			if (ret == 4) {
// 				ret = sysfs_match_string(terminals, t);
// 				if (ret < 0) {
// 					dev_err(dev, "dest read:%s %s %s %s", t, s, p, d);
// 					goto out;
// 				}
// 				terminal = ret ? ADI_APOLLO_TX : ADI_APOLLO_RX;

// 				ret = sysfs_match_string(cfir_selects, s);

// 				if (ret < 0)
// 					goto out;

// 				switch (ret) {
// 					case 0:
// 						cfir_sel = ADI_APOLLO_CFIR_NONE;
// 						break;
// 					case 1:
// 						cfir_sel = ADI_APOLLO_CFIR_A0;
// 						break;
// 					case 2:
// 						cfir_sel = ADI_APOLLO_CFIR_A1;
// 						break;
// 					case 3:
// 						cfir_sel = ADI_APOLLO_CFIR_B0;
// 						break;
// 					case 4:
// 						cfir_sel = ADI_APOLLO_CFIR_B1;
// 						break;
// 					case 5:
// 						cfir_sel = ADI_APOLLO_CFIR_ALL;
// 						break;
// 					case 6:
// 						ret = sscanf(s, "cfir_mask%u", &mask);
// 						if (ret != 1)
// 							goto out;
// 						cfir_sel = mask;
// 						break;
// 					default:
// 						goto out;
// 				}

// 				ret = sysfs_match_string(cfir_profiles, p);
// 				if (ret < 0) {
// 					dev_err(dev, "dest read:%s %s %s %s", t, s, p, d);
// 					goto out;
// 				}
// 				switch (ret) {
// 				case 0:
// 					profile_sel = ADI_APOLLO_CFIR_PROFILE_NONE;
// 					break;
// 				case 1:
// 					profile_sel = ADI_APOLLO_CFIR_PROFILE_0;
// 					break;
// 				case 2:
// 					profile_sel = ADI_APOLLO_CFIR_PROFILE_1;
// 					break;
// 				case 3:
// 					profile_sel = ADI_APOLLO_CFIR_PROFILE_ALL;
// 					break;
// 				default:
// 					goto out;
// 				}

// 				ret = sysfs_match_string(cfir_datapaths, d);
// 				if (ret < 0) {
// 					dev_err(dev, "dest read:%s %s %s %s", t, s, p, d);
// 					goto out;
// 				}
// 				switch (ret) {
// 				case 0:
// 					dp_sel = ADI_APOLLO_CFIR_DP_NONE;
// 					break;
// 				case 1:
// 					dp_sel = ADI_APOLLO_CFIR_DP_0;
// 					break;
// 				case 2:
// 					dp_sel = ADI_APOLLO_CFIR_DP_1;
// 					break;
// 				case 3:
// 					dp_sel = ADI_APOLLO_CFIR_DP_2;
// 					break;
// 				case 4:
// 					dp_sel = ADI_APOLLO_CFIR_DP_3;
// 					break;
// 				case 5:
// 					dp_sel = ADI_APOLLO_CFIR_DP_ALL;
// 					break;
// 				case 6:
// 					ret = sscanf(d, "datapath_mask%u", &mask);
// 					if (ret != 1)
// 						goto out;
// 					dp_sel = mask;
// 					break;
// 				default:
// 					goto out;
// 				}

// 				read_mask |= BIT(0);
// 				continue;
// 			}
// 		}

// 		if (~read_mask & BIT(1)) {
// 			ret = sscanf(line, "gain: %d", &val);
// 			if (ret == 1) {
// 				gain = ad9088_cfir_gain_enc(val);
// 				read_mask |= BIT(1);
// 				continue;
// 			}
// 		}

// 		if (~read_mask & BIT(2)) {
// 			ret = sscanf(line, "complex_scalar: %i %i", &sval_i, &sval_q);
// 			if (ret == 2) {
// 				scalar_i = sval_i;
// 				scalar_q = sval_q;
// 				read_mask |= BIT(2);
// 				continue;
// 			}
// 		}

// 		if (~read_mask & BIT(3)) {
// 			ret = sscanf(line, "bypass: %hhu", &cfir_pgm.cfir_bypass);
// 			if (ret == 1) {
// 				read_mask |= BIT(3);
// 				continue;
// 			}
// 		}
// 		if (~read_mask & BIT(4)) {
// 			ret = sscanf(line, "sparse_filt_en: %hhu", &cfir_pgm.cfir_sparse_filt_en);
// 			if (ret == 1) {
// 				read_mask |= BIT(4);
// 				continue;
// 			}
// 		}
// 		if (~read_mask & BIT(5)) {
// 			ret = sscanf(line, "32taps_en: %hhu", &cfir_pgm.cfir_32taps_en);
// 			if (ret == 1) {
// 				read_mask |= BIT(5);
// 				continue;
// 			}
// 		}
// 		if (~read_mask & BIT(6)) {
// 			ret = sscanf(line, "coeff_transfer: %hhu", &cfir_pgm.cfir_coeff_transfer);
// 			if (ret == 1) {
// 				read_mask |= BIT(6);
// 				continue;
// 			}
// 		}
// 		if (~read_mask & BIT(6)) {
// 			char p[16];
// 			ret = sscanf(line, "enable: %u %s", &enable, p);
// 			if (ret == 2) {
// 				enable_profile = sysfs_match_string(cfir_profiles, p);
// 				if (enable_profile < 0)
// 					goto out;
// 				read_mask |= BIT(6);
// 				continue;
// 			}
// 		}
// 		if (~read_mask & BIT(7)) {
// 			char m[16];
// 			ret = sscanf(line, "selection_mode: %s", m);
// 			if (ret == 1) {
// 				selection_mode = sysfs_match_string(cfir_profile_selection_mode, m);
// 				if (selection_mode < 0)
// 					goto out;
// 				read_mask |= BIT(7);
// 				continue;
// 			}
// 		}

// 		ret = sscanf(line, "%i %i", &sval_i, &sval_q);
// 		if (ret == 2) {
// 			if (i >= ADI_APOLLO_CFIR_COEFF_NUM)
// 				return -EINVAL;

// 			cfir_coeff_i[i] = (u16)sval_i;
// 			cfir_coeff_q[i++] = (u16)sval_q;
// 			continue;
// 		}
// 	}

// 	if (read_mask & BIT(0)) {
// 		dev_dbg(dev,"terminal: %s\n", terminals[terminal]);
// 		dev_dbg(dev,"cfir_sel: MASK 0x%X\n", cfir_sel);
// 		dev_dbg(dev,"profile_sel: %s\n", cfir_profiles[profile_sel]);
// 		dev_dbg(dev,"dp_sel: MASK 0x%x\n", dp_sel);
// 	} else {
// 		dev_err(dev, "dest: not found\n");
// 		goto out;
// 	}

// 	for (j = 0; j < i; j++) {
// 		dev_dbg(dev,"0x%X\t0x%X\n", cfir_coeff_i[j], cfir_coeff_q[j]);
// 	}

// 	ret = adi_apollo_cfir_coeff_pgm(&phy->ad9088, terminal, cfir_sel, profile_sel, dp_sel, cfir_coeff_i, cfir_coeff_q, i);
// 	if (ret < 0)
// 		goto out1;

// 	if (read_mask & BIT(2)) {
// 		dev_dbg(dev,"scalar_i: %d scalar_q: %d\n", scalar_i, scalar_q);
// 		ret = adi_apollo_cfir_scalar_pgm(&phy->ad9088, terminal, cfir_sel, profile_sel, dp_sel, scalar_i, scalar_q);
// 		if (ret < 0)
// 			goto out1;
// 	}

// 	if (read_mask & BIT(1)) {
// 		dev_dbg(dev,"gain: %d\n", gain);
// 		ret = adi_apollo_cfir_gain_pgm(&phy->ad9088, terminal, cfir_sel, profile_sel, dp_sel, gain);
// 		if (ret < 0)
// 			goto out1;
// 	}

// 	if (read_mask & BIT(7)) {
// 		dev_dbg(dev,"selection_mode: %d\n", selection_mode);
// 		ret = adi_apollo_cfir_profile_sel_mode_set(&phy->ad9088, terminal, cfir_sel, selection_mode);
// 		if (ret < 0)
// 			goto out1;
// 	}

// 	ret = adi_apollo_cfir_pgm(&phy->ad9088, terminal, cfir_sel, &cfir_pgm);
// 	if (ret < 0)
// 		goto out1;

// 	/* This seems to be required */
// 	ret = adi_apollo_clk_mcs_oneshot_sync(&phy->ad9088);
// 	if (ret < 0)
// 		goto out1;

// 	//ret = adi_apollo_cfir_sparse_coeff_sel_pgm(&phy->ad9088, terminal, cfir_sel, profile_sel, dp_sel, uint16_t cfir_coeff_sel[], uret = len);
// 	//ret = adi_apollo_cfir_sparse_mem_sel_pgm(&phy->ad9088, terminal, cfir_sel, profile_sel, dp_sel, uint8_t cfir_mem_sel[], uret = len);

// 	if (read_mask & BIT(6)) {
// 		u32 j;
// 		dev_dbg(dev,"enable: %u enable_profile: %u\n", enable, enable_profile);
// 		adi_apollo_cfir_profile_sel(&phy->ad9088, terminal, cfir_sel, dp_sel, enable_profile);
// 		if (ret < 0)
// 			goto out1;
// 		ret = adi_apollo_cfir_mode_enable_set(&phy->ad9088, terminal, cfir_sel, enable);
// 		if (ret < 0)
// 			goto out1;

// 		for (i = 0; i < 4; i++) {
// 			if (cfir_sel & BIT(i)) {
// 				for (j = 0; j < 4; j++) {
// 					if (dp_sel & BIT(j)) {
// 						phy->cfir_profile[terminal][BIT(i)][BIT(j)] = enable_profile;
// 						phy->cfir_enable[terminal][BIT(i)][BIT(j)] = enable;
// 					}
// 				}
// 			}
// 		}
// 	}

// out1:
// 	if (ret != API_CMS_ERROR_OK)
// 		dev_err(&phy->spi->dev, "Prgramming filter failed (%d)", ret);

// 	return size;

// out:
// 	dev_err(dev, "malformed CFir filter file detected\n");

// 	return -EINVAL;
// }

// static ssize_t
// ad9088_cfir_bin_write(struct file *filp, struct kobject *kobj,
// 		       struct bin_attribute *bin_attr,
// 		       char *buf, loff_t off, size_t count)
// {

// 	struct iio_dev *indio_dev = dev_to_iio_dev(kobj_to_dev(kobj));
// 	struct axiadc_converter *conv = iio_device_get_drvdata(indio_dev);
// 	struct ad9088_phy *phy = conv->phy;

// 	guard(mutex)(&phy->lock);

// 	return ad9088_parse_cfilt(phy, buf, count);
// }

// #if 1
// static int ad9088_trig_sync(struct ad9088_phy *phy, int val, int val2, int val3, int val4)
// {
// 	adi_apollo_device_t *device = &phy->ad9088;
// 	int ret;

// 	#include "adi_apollo_bf_mcs_sync.h"


// 	dev_err(&phy->spi->dev," MCS - %d 0x%X\n", val, val2);
// 	switch (val){
// 	case 1:
// 		ret = ad9088_jesd204_post_setup_stage1(phy->jdev, JESD204_STATE_OP_REASON_INIT);
// 		break;
// 	case 2:
// 		ret = ad9088_jesd204_post_setup_stage2(phy->jdev, JESD204_STATE_OP_REASON_INIT);
// 		break;
// 	case 3:
// 		ret = ad9088_jesd204_post_setup_stage3(phy->jdev, JESD204_STATE_OP_REASON_INIT);
// 		break;
// 	case 4:
// 		adi_apollo_clk_mcs_oneshot_sync(device);
// 		break;
// 	case 5:
// 		adi_apollo_jrx_phase_adjust_set(device, ADI_APOLLO_LINK_A0 | ADI_APOLLO_LINK_B0, val2);
// 		adi_apollo_jtx_phase_adjust_set(device, ADI_APOLLO_LINK_A0 | ADI_APOLLO_LINK_B0, val2);
// 		break;
// 	case 6:
// 		ret = ad9088_jesd204_post_setup_stage1(phy->jdev, JESD204_STATE_OP_REASON_UNINIT);
// 		break;
// 	case 8:
// 		adi_apollo_jtx_link_enable_set(device, ADI_APOLLO_LINK_A0 | ADI_APOLLO_LINK_B0, val2);
// 		adi_apollo_jrx_link_enable_set(device, ADI_APOLLO_LINK_A0 | ADI_APOLLO_LINK_B0, val2);
// 		break;
// 	case 9:
// 		pr_err("JRX %x\n", val2);
// 		adi_apollo_jrx_phase_adjust_set(device, ADI_APOLLO_LINK_A0 | ADI_APOLLO_LINK_B0, val2);
// 		adi_apollo_clk_mcs_oneshot_sync(device);
// 		break;
// 	case 10:
// 		pr_err("JTX A0 %x\n", val2);
// 		adi_apollo_jtx_phase_adjust_set(device, ADI_APOLLO_LINK_A0, val2);
// 		adi_apollo_clk_mcs_oneshot_sync(device);
// 		break;
// 	case 11:
// 		pr_err("JTX B0 %x\n", val2);
// 		adi_apollo_jtx_phase_adjust_set(device, ADI_APOLLO_LINK_B0, val2);
// 		adi_apollo_clk_mcs_oneshot_sync(device);
// 		break;
// 	case 12:
// 		/* Datapath reset */
// 		if (ret = adi_apollo_serdes_jrx_cal(&phy->ad9088), ret != API_CMS_ERROR_OK) {
// 			pr_err("Error from adi_apollo_serdes_jrx_cal() %d\n", ret);
// 			//return ret;
// 		}
// 		break;
// 	case 13:
// 		adi_apollo_txmisc_dp_reset(device, ADI_APOLLO_SIDE_ALL, 1);
// 		adi_apollo_txmisc_dp_reset(device, ADI_APOLLO_SIDE_ALL, 0);
// 		break;
// 	case 14:
// 		adi_apollo_clk_mcs_dyn_sync_sequence_run(device);
// 		break;

// 	case 16:
// 		adi_apollo_clk_mcs_rx_data_fifo_enable(device, ADI_APOLLO_ADC_ALL, val2);
// 		break;
// 	case 17:
// 		adi_apollo_jrx_link_enable_set(device, ADI_APOLLO_LINK_A0 | ADI_APOLLO_LINK_B0, ADI_APOLLO_DISABLE);
// 		adi_apollo_jrx_link_enable_set(device, ADI_APOLLO_LINK_A1 | ADI_APOLLO_LINK_B1, ADI_APOLLO_DISABLE);
// 		break;
// 	case 18:
// 		adi_apollo_jrx_link_enable_set(device, ADI_APOLLO_LINK_A0 | ADI_APOLLO_LINK_B0, ADI_APOLLO_ENABLE);
// 		adi_apollo_jrx_link_enable_set(device, ADI_APOLLO_LINK_A1 | ADI_APOLLO_LINK_B1, ADI_APOLLO_DISABLE);
// 		break;
// 	case 19:
// 		if (phy->trig_sync_en) {
// 			/* Use Trigger pin A0 to sync Rx and Tx */
// 			adi_apollo_clk_mcs_sync_trig_map(device, ADI_APOLLO_RX_TX_ALL, ADI_APOLLO_TRIG_PIN_A0);

// 			/*
// 			* Resync the Rx and Tx dig only during trig sync
// 			*/
// 			adi_apollo_clk_mcs_trig_sync_enable(device, 0);
// 			adi_apollo_clk_mcs_trig_reset_disable(device);

// 			/*
// 			* Set trig_syn to 1. Apollo will wait for a trigger from the FPGA. When
// 			* received, the FSRC will be reset.
// 			*
// 			* trig_sync is not self-clearing
// 			*/
// 			adi_apollo_clk_mcs_trig_sync_enable(device, 0);
// 			adi_apollo_clk_mcs_trig_reset_dsp_enable(device);
// 			adi_apollo_clk_mcs_trig_sync_enable(device, 1);
// 		}
// 		break;
// 	case 20:
// 		if (phy->triq_req_gpio && phy->trig_sync_en) {
// 			gpiod_set_value(phy->triq_req_gpio, 1);
// 			mdelay(1);
// 			gpiod_set_value(phy->triq_req_gpio, 0);
// 		}
// 		break;
// 	case 21:
// 		if (phy->trig_sync_en) {
// 			u16 phase, phase1;
// 			/*
// 			* Wait for the trigger sync to finish.
// 			*/
// 			adi_apollo_hal_bf_wait_to_set(device, BF_TRIGGER_SYNC_DONE_A0_INFO(MCS_SYNC_MCSTOP0), 1000000, 100);   // Will set when Apollo recvs a trigger pulse
// 			adi_apollo_clk_mcs_trig_phase_get(&phy->ad9088, ADI_APOLLO_TRIG_PIN_A0, &phase, &phase1);


// 			adi_apollo_clk_mcs_trig_sync_enable(device, 0);
// 			adi_apollo_clk_mcs_trig_reset_disable(device);


// 			// adi_apollo_hal_bf_set(device, BF_TRIGGER_ENABLE_INFO(MBIAS0), 0);
// 			// adi_apollo_hal_bf_set(device, BF_TRIGGER_ENABLE_INFO(MBIAS1), 0);

// 			if (1/*__is_defined(DEBUG)*/) {
// 				/*
// 				* Read the trigger sync count. It will increment by two each time the FPGA sysref sequencer is executed
// 				*/

// 				uint8_t sync_input_count[4];
// 				adi_apollo_hal_bf_get(device, BF_SYNC_INPUT_COUNT_INFO(TXRX_PREFSRC_RECONF_TX_SLICE_0_TX_DIGITAL0), &sync_input_count[0], 1);
// 				adi_apollo_hal_bf_get(device, BF_SYNC_INPUT_COUNT_INFO(TXRX_PREFSRC_RECONF_TX_SLICE_1_TX_DIGITAL0), &sync_input_count[1], 1);
// 				adi_apollo_hal_bf_get(device, BF_SYNC_INPUT_COUNT_INFO(TXRX_PREFSRC_RECONF_TX_SLICE_0_TX_DIGITAL1), &sync_input_count[2], 1);
// 				adi_apollo_hal_bf_get(device, BF_SYNC_INPUT_COUNT_INFO(TXRX_PREFSRC_RECONF_TX_SLICE_1_TX_DIGITAL1), &sync_input_count[3], 1);
// 				dev_info(&phy->spi->dev, "sync_input_count=%d %d %d %d PHASE %d\n", sync_input_count[0], sync_input_count[1], sync_input_count[2], sync_input_count[3], phase);
// 			}
// 		}
// 		break;
// 	case 22:
// 		ad9088_print_sysref_phase(phy);
// 		break;
// 	case 23:
// 		ret = adi_apollo_jtx_lane_drive_swing_set(device, val2, val3, val4);
// 		dev_info(&phy->spi->dev, "JTX Lane Drive Swing Set %d %d %d %d (%d)\n", val, val2, val3, val4, ret);
// 		break;
// 	case 24:
// 		ret = adi_apollo_jtx_lane_pre_emphasis_set(device, val2, val3, val4);
// 		dev_info(&phy->spi->dev, "JTX Lane Drive pre emphasis Set %d %d %d %d (%d)\n", val, val2, val3, val4, ret);
// 		break;
// 	case 25:
// 		ret = adi_apollo_jtx_lane_post_emphasis_set(device, val2, val3, val4);
// 		dev_info(&phy->spi->dev, "JTX Lane post emphasis Set %d %d %d %d (%d)\n", val, val2, val3, val4, ret);
// 		break;
// 	default:
// 		return -EINVAL;
// 	}

// 	return 0;
// }
// #endif

// enum ad9088_debugfs_cmd {
// 	DBGFS_NONE,
// 	DBGFS_BIST_PRBS_JRX,
// 	DBGFS_BIST_PRBS_JRX_ERR,
// 	DBGFS_BIST_PRBS_JTX,
// 	DBGFS_BIST_JRX_2D_EYE,
// 	DBGFS_DEV_API_INFO,
// 	DBGFS_DEV_UUID_INFO,
// 	DBGFS_DEV_CHIP_INFO,
// 	DBGFS_DEV_TEMP_INFO,
// 	DBGFS_HSCI_ENABLE,
// 	DBGFS_TRIG_SYNC,
// 	DBGFS_CLK_PWR_STAT,
// 	DBGFS_GENERIC,
// 	DBGFS_JRX_PHASE_ADJUST_CALC
// };

// static ssize_t ad9088_debugfs_read(struct file *file, char __user *userbuf,
// 				   size_t count, loff_t *ppos)
// {
// 	struct ad9088_debugfs_entry *entry = file->private_data;
// 	struct iio_dev *indio_dev = entry->indio_dev;
// 	struct axiadc_converter *conv = iio_device_get_drvdata(indio_dev);
// 	struct ad9088_phy *phy = conv->phy;
// 	adi_apollo_device_tmu_data_t tmu_data;
// 	adi_apollo_serdes_prbs_checker_status_t prbs_stat[24];
// 	adi_apollo_hal_protocol_e protocol;
// 	adi_apollo_clk_input_power_status_e pwr_stat_a, pwr_stat_b;
// 	adi_apollo_serdes_jrx_horiz_eye_resp_t horz_resp;
// 	adi_apollo_serdes_jrx_vert_eye_resp_t vert_resp;
// 	u64 val = 0;
// 	ssize_t len = 0;
// 	int ret, i, lane, prbs, duration;
// 	u8 uuid[ADI_APOLLO_UUID_NUM_BYTES];
// 	u16 api_rev[3];
// 	u8 die_id;

// 	if (*ppos)
// 		return 0;

// 	guard(mutex)(&phy->lock);

// 	if (entry->out_value) {
// 		switch (entry->size) {
// 		case 1:
// 			val = *(u8 *)entry->out_value;
// 			break;
// 		case 2:
// 			val = *(u16 *)entry->out_value;
// 			break;
// 		case 4:
// 			val = *(u32 *)entry->out_value;
// 			break;
// 		case 5:
// 			val = *(bool *)entry->out_value;
// 			break;
// 		case 8:
// 			val = *(u64 *)entry->out_value;
// 			break;
// 		default:
// 			ret = -EINVAL;
// 		}

// 	} else if (entry->cmd) {
// 		switch (entry->cmd) {
// 		case DBGFS_BIST_PRBS_JRX_ERR:
// 			ret = adi_apollo_serdes_jrx_prbs_checker_status(&phy->ad9088, phy->jrx_lanes, prbs_stat, phy->jrx_lanes_used);
// 			if (ret) {
// 				dev_err(&phy->spi->dev, "adi_apollo_serdes_jrx_prbs_checker_status() failed (%d)", ret);
// 				break;
// 			}

// 			for (i = 0; i < phy->jrx_lanes_used; i++)
// 				len += snprintf(phy->dbuf + len, sizeof(phy->dbuf), "%c: lane-%u %u/%u\n",
// 					phy->jrx_lanes[i] < 12 ? 'A' : 'B',
// 					phy->jrx_lanes[i] > 11 ? phy->jrx_lanes[i] - 12 : phy->jrx_lanes[i],
// 					prbs_stat[phy->jrx_lanes[i]].err_count, prbs_stat[phy->jrx_lanes[i]].err_sticky);
// 			break;
// 		case DBGFS_BIST_JRX_2D_EYE:
// 			if (!entry->val)
// 				return -EINVAL;

// 			lane = (entry->val & 0xFF) - 1;
// 			prbs = (entry->val >> 8) & 0xFF;
// 			duration = (entry->val >> 16) & 0xFFFF;

// 			entry->val = 0;

// 			if (phy->profile.jrx[0].common_link_cfg.lane_rate_kHz > 16000000) {
// 				ret = adi_apollo_serdes_jrx_bgcal_freeze(&phy->ad9088, ad9088_jrx_serdes_en_mask(phy));
// 				if (ret) {
// 					dev_err(&phy->spi->dev, "Error from adi_apollo_serdes_jrx_bgcal_freeze() %d\n", ret);
// 					return ret;
// 				}
// 			}

// 			ret = adi_apollo_serdes_jrx_horiz_eye_sweep(&phy->ad9088, lane, prbs);
// 			if (ret < 0) {
// 				dev_err(&phy->spi->dev,
// 					"JRX horizontal eye_sweep lane%u failed (%d)", lane, ret);
// 				return ret;
// 			}

// 			msleep(duration); /* FIXME: Don't think this does anything */

// 			ret = adi_apollo_serdes_jrx_horiz_eye_sweep_resp_get(&phy->ad9088, lane, &horz_resp);
// 			if (ret < 0) {
// 				dev_err(&phy->spi->dev,
// 					"JRX horizontal eye_sweep lane%u failed (%d)", lane, ret);
// 				return ret;
// 			}

// 			//pr_err("lane_num: %02d\t version: %02d\t spo_left: %02d\t spo_right: %02d.\n", lane, horz_resp.ver, horz_resp.spo_left, horz_resp.spo_right);

// 			ret = adi_apollo_serdes_jrx_vert_eye_sweep(&phy->ad9088, lane);
// 			if (ret < 0) {
// 				dev_err(&phy->spi->dev,
// 					"JRX vertical eye_sweep lane%u failed (%d)", lane, ret);
// 				return ret;
// 			}

// 			msleep(duration); /* FIXME: Don't think this does anything */

// 			ret = adi_apollo_serdes_jrx_vert_eye_sweep_resp_get(&phy->ad9088, lane, &vert_resp);
// 			if (ret < 0) {
// 				dev_err(&phy->spi->dev,
// 					"JRX vertical eye_sweep lane%u failed (%d)", lane, ret);
// 				return ret;
// 			}

// 			if (phy->profile.jrx[0].common_link_cfg.lane_rate_kHz > 16000000) {
// 				ret = adi_apollo_serdes_jrx_bgcal_unfreeze(&phy->ad9088, ad9088_jrx_serdes_en_mask(phy));
// 				if (ret) {
// 					dev_err(&phy->spi->dev, "Error from adi_apollo_serdes_jrx_bgcal_unfreeze() %d\n", ret);
// 					return ret;
// 				}
// 			}

// 			len = snprintf(phy->dbuf, sizeof(phy->dbuf),
// 				"# lane %u spo_steps %u rate %u spo_left %u spo_right %u version %u\n",
// 				lane, ADI_APOLLO_SERDES_JRX_VERT_EYE_TEST_RESP_BUF_SIZE / 2,
// 				phy->profile.jrx[lane >=
// 				ADI_APOLLO_JESD_MAX_LANES_PER_SIDE].common_link_cfg.lane_rate_kHz,
// 				horz_resp.spo_left, horz_resp.spo_right, horz_resp.ver);

// 			for (i = 0; i < ADI_APOLLO_SERDES_JRX_VERT_EYE_TEST_RESP_BUF_SIZE; i += 2)
// 					if (!(vert_resp.eye_heights_at_spo[i] == 127 && vert_resp.eye_heights_at_spo[i + 1] == -127))
// 						len += snprintf(phy->dbuf + len,
// 								sizeof(phy->dbuf),
// 								"%d,%d,%d\n", (i / 2) - 16,
// 								vert_resp.eye_heights_at_spo[i] * 4,
// 								vert_resp.eye_heights_at_spo[i + 1] * 4);

// 			break;
// 		case DBGFS_DEV_TEMP_INFO:
// 			if (!phy->is_initialized)
// 				return -EBUSY;
// 			ret = adi_apollo_device_tmu_get(&phy->ad9088, &tmu_data);
// 			if (ret)
// 				break;

// 			len = snprintf(phy->dbuf, sizeof(phy->dbuf), "TMU (deg C): serdes_pll=%d mpu_a=%d mpu_b=%d adc_a=%d clk_a=%d adc_b=%d clk_b=%d clk_c=%d (avg: %d, avg mask: 0x%04x)\n",
// 				tmu_data.temp_degrees_celsius[ADI_APOLLO_DEVICE_TMU_SERDES_PLL],
// 				tmu_data.temp_degrees_celsius[ADI_APOLLO_DEVICE_TMU_MPU_A],
// 				tmu_data.temp_degrees_celsius[ADI_APOLLO_DEVICE_TMU_MPU_B],
// 				tmu_data.temp_degrees_celsius[ADI_APOLLO_DEVICE_TMU_ADC_A],
// 				tmu_data.temp_degrees_celsius[ADI_APOLLO_DEVICE_TMU_CLK_A],
// 				tmu_data.temp_degrees_celsius[ADI_APOLLO_DEVICE_TMU_ADC_B],
// 				tmu_data.temp_degrees_celsius[ADI_APOLLO_DEVICE_TMU_CLK_B],
// 				tmu_data.temp_degrees_celsius[ADI_APOLLO_DEVICE_TMU_CLK_C],
// 				tmu_data.temp_degrees_celsius_avg,
// 				tmu_data.avg_mask);
// 			break;
// 		case DBGFS_DEV_API_INFO:
// 			adi_apollo_device_api_revision_get(&phy->ad9088,
// 							   &api_rev[0], &api_rev[1], &api_rev[2]);

// 			len = snprintf(phy->dbuf, sizeof(phy->dbuf), "%u.%u.%u\n",
// 				       api_rev[0], api_rev[1], api_rev[2]);
// 			break;
// 		case DBGFS_DEV_UUID_INFO:
// 			ret = adi_apollo_device_uuid_get(&phy->ad9088, uuid, ADI_APOLLO_UUID_NUM_BYTES);
// 			if (ret)
// 				break;

// 			len = 0;
// 			for (i = 0; i < ADI_APOLLO_UUID_NUM_BYTES; i++) {
// 				len += snprintf(phy->dbuf + len, sizeof(phy->dbuf) - len, "%02x", uuid[i]);
// 			}
// 			len += snprintf(phy->dbuf + len, sizeof(phy->dbuf) - len, "\n");
// 			break;
// 		case DBGFS_DEV_CHIP_INFO:
// 			ret = adi_apollo_device_chip_id_get(&phy->ad9088, &phy->chip_id);
// 			if (ret < 0) {
// 				dev_err(&phy->spi->dev, "chip_id failed (%d)\n", ret);
// 				break;
// 			}

// 			ret = adi_apollo_device_die_id_get(&phy->ad9088, &die_id);
// 			if (ret < 0) {
// 				dev_err(&phy->spi->dev, "die_id failed (%d)\n", ret);
// 				break;
// 			}

// 			len = snprintf(phy->dbuf, sizeof(phy->dbuf), "AD%X Rev. %u Grade %u DieID %u\n",
// 				       phy->chip_id.prod_id, phy->chip_id.dev_revision, phy->chip_id.prod_grade, die_id);
// 			break;
// 		case DBGFS_CLK_PWR_STAT:
// 			ret = adi_apollo_clk_mcs_input_power_status_get(&phy->ad9088, &pwr_stat_a, &pwr_stat_b);
// 			if (ret)
// 				break;
// 			len = snprintf(phy->dbuf, sizeof(phy->dbuf), "Clock input power detection A: %s\n",
// 				       !pwr_stat_a ? "GOOD" : (pwr_stat_a == ADI_APOLLO_CLK_PWR_UNDERDRIVEN ? "UNDERDRIVEN" :
// 					(pwr_stat_a == ADI_APOLLO_CLK_PWR_OVERDRIVEN ? "OVERDRIVEN" : "UNUSED")));
// 			len += snprintf(phy->dbuf + len, sizeof(phy->dbuf), "Clock input power detection B: %s\n",
// 					!pwr_stat_b ? "GOOD" : (pwr_stat_b == ADI_APOLLO_CLK_PWR_UNDERDRIVEN ? "UNDERDRIVEN" :
// 					(pwr_stat_b == ADI_APOLLO_CLK_PWR_OVERDRIVEN ? "OVERDRIVEN" : "UNUSED")));
// 			break;
// 		case DBGFS_HSCI_ENABLE:
// 			if (phy->hsci) {
// 				adi_apollo_hal_active_protocol_get(&phy->ad9088, &protocol);
// 				val = (protocol == ADI_APOLLO_HAL_PROTOCOL_HSCI);
// 			} else {
// 				val = 0;
// 			}
// 			break;
// 		default:
// 			val = entry->val;
// 		}
// 	} else {
// 		return -EFAULT;
// 	}
// 	if (!len)
// 		len = snprintf(phy->dbuf, sizeof(phy->dbuf), "%llu\n", val);

// 	return simple_read_from_buffer(userbuf, count, ppos, phy->dbuf, len);
// }

// static int ad9088_val_to_jtx_prbs(int val)
// {
// 	switch (val) {
// 	case 0:
// 		return 0;
// 	case 7:
// 		return ADI_APOLLO_SERDES_JTX_PRBS7;
// 	case 9:
// 		return ADI_APOLLO_SERDES_JTX_PRBS9;
// 	case 15:
// 		return ADI_APOLLO_SERDES_JTX_PRBS15;
// 	case 31:
// 		return ADI_APOLLO_SERDES_JTX_PRBS31;
// 	default:
// 		return -EINVAL;
// 	}
// 	return -EINVAL;
// };

// static int ad9088_val_to_jrx_prbs(int val)
// {
// 	switch (val) {
// 	case 0:
// 		return 0;
// 	case 7:
// 		return ADI_APOLLO_SERDES_JRX_PRBS7;
// 	case 9:
// 		return ADI_APOLLO_SERDES_JRX_PRBS9;
// 	case 15:
// 		return ADI_APOLLO_SERDES_JRX_PRBS15;
// 	case 31:
// 		return ADI_APOLLO_SERDES_JRX_PRBS31;
// 	default:
// 		return -EINVAL;
// 	}
// 	return -EINVAL;
// };

// static ssize_t ad9088_debugfs_write(struct file *file,
// 				    const char __user *userbuf, size_t count, loff_t *ppos)
// {
// 	struct ad9088_debugfs_entry *entry = file->private_data;
// 	struct iio_dev *indio_dev = entry->indio_dev;
// 	struct axiadc_converter *conv = iio_device_get_drvdata(indio_dev);
// 	struct ad9088_phy *phy = conv->phy;
// 	adi_apollo_serdes_prbs_generator_enable_t prbs_gen;
// 	adi_apollo_serdes_prbs_checker_enable_t prbs_chk;
// 	int val2, val3, val4, ret, side, lane;
// 	s64 val;
// 	u16 phase;
// 	char buf[80];
// 	// u8 val_u8;
// 	// u8 lv, rv;

// 	guard(mutex)(&phy->lock);

// 	count = min_t(size_t, count, (sizeof(buf) - 1));
// 	if (copy_from_user(buf, userbuf, count))
// 		return -EFAULT;

// 	buf[count] = 0;

// 	ret = sscanf(buf, "%lli %i %i %i", &val, &val2, &val3, &val4);
// 	if (ret < 1)
// 		return -EINVAL;

// 	switch (entry->cmd) {
// 	case DBGFS_BIST_PRBS_JRX:
// 		if (ret < 1)
// 			return -EINVAL;

// 		prbs_chk.enable = !!val;
// 		prbs_chk.auto_mode = 0;
// 		prbs_chk.auto_mode_thres = 4; /* 2^n */
// 		prbs_chk.prbs_mode = ad9088_val_to_jrx_prbs(val);


// 		ret = adi_apollo_jrx_link_enable_set(&phy->ad9088, ADI_APOLLO_LINK_A1 | ADI_APOLLO_LINK_B1, ADI_APOLLO_DISABLE);
// 		if (ret) {
// 			dev_err(&phy->spi->dev, "Error enabling JRx links %d\n", ret);
// 			return ret;
// 		}


// 		adi_apollo_serdes_jrx_prbs_checker_enable(&phy->ad9088, phy->jrx_lanes, phy->jrx_lanes_used, &prbs_chk);

// 		/* Enable Apollo JRx links */
// 		ret = adi_apollo_jrx_link_enable_set(&phy->ad9088, ADI_APOLLO_LINK_A0 | ADI_APOLLO_LINK_B0, ADI_APOLLO_ENABLE);
// 		if (ret) {
// 			dev_err(&phy->spi->dev, "Error enabling JRx links %d\n", ret);
// 			return ret;
// 		}

// 		if (prbs_chk.enable)
// 			adi_apollo_serdes_jrx_prbs_clear_error(&phy->ad9088, phy->jrx_lanes, phy->jrx_lanes_used);

// 		entry->val = val;

// 		return count;
// 	case DBGFS_BIST_PRBS_JTX:
// 		if (ret < 1)
// 			return -EINVAL;

// 		prbs_gen.enable = !!val;
// 		prbs_gen.mode = ad9088_val_to_jtx_prbs(val);

// 		adi_apollo_serdes_prbs_generator_enable(&phy->ad9088, (u8 *) lanes_all, ARRAY_SIZE(lanes_all), &prbs_gen);
// 		entry->val = val;

// 		return count;
// 	case DBGFS_HSCI_ENABLE:
// 		if (ret < 1)
// 			return -EINVAL;

// 		if (phy->hsci) {
// 			ret = adi_apollo_hal_active_protocol_set(&phy->ad9088,
// 				val == 1 ? ADI_APOLLO_HAL_PROTOCOL_HSCI :
// 				ADI_APOLLO_HAL_PROTOCOL_SPI0);
// 		} else {
// 			return -ENODEV;
// 		}
// 		break;
// 	case DBGFS_TRIG_SYNC:
// 		if (ret < 1)
// 			return -EINVAL;

// 		ret = ad9088_trig_sync(phy, val, val2, val3, val4);
// 		break;
// 	case DBGFS_GENERIC:
// 		// pr_err("DBGFS_GENERIC %d %lld %d %d\n", ret, val, val2, val3);
// 		// adi_apollo_gpio_quick_config_mode_set(&phy->ad9088, 0);
// 		// ret = adi_apollo_gpio_cmos_func_mode_set(&phy->ad9088, val, val3);
// 		break;
// 	case DBGFS_BIST_JRX_2D_EYE:
// 		if (ret < 1)
// 			return -EINVAL;

// 		if (ret < 2)
// 			val2 = 7; /* PRBS7 */

// 		if (ret < 3)
// 			val3 = 10; /* 10 ms */

// 		if (val > 23)
// 			return -EINVAL;

// 		lane = val;

// 		if (lane >= ADI_APOLLO_JESD_MAX_LANES_PER_SIDE) {
// 			side = 1;
// 			lane -= ADI_APOLLO_JESD_MAX_LANES_PER_SIDE;
// 		} else {
// 			side = 0;
// 		}

// 		if (!(phy->profile.jrx[side].common_link_cfg.lane_enables & (1 << lane)))
// 			return -EINVAL;

// 		val = val + 1;

// 		ret = ad9088_val_to_jrx_prbs(val2);
// 		if (ret < 0)
// 			return ret;

// 		val2 = ret;
// 		/*           Time          PRBS                 Lane */
// 		entry->val = val3 << 16 | (val2 & 0xFF) << 8 | (val & 0xFF);

// 		return count;
// 	case DBGFS_JRX_PHASE_ADJUST_CALC:
// 		ret  = adi_apollo_jrx_phase_adjust_calc(&phy->ad9088, ADI_APOLLO_LINK_A0 | ADI_APOLLO_LINK_B0,
// 												ADI_APOLLO_JRX_PHASE_ADJ_MARGIN_DEFAULT, &phase);
// 		if (ret != API_CMS_ERROR_OK) {
// 			dev_err(&phy->spi->dev,"Error from adi_apollo_jrx_phase_adjust_calc() %d\n", ret);
// 			return ret;
// 		}
// 		entry->val = phase;
// 		return count;
// 	default:
// 		break;
// 	}

// 	if (entry->out_value) {
// 		switch (entry->size) {
// 		case 1:
// 			*(u8 *)entry->out_value = val;
// 			break;
// 		case 2:
// 			*(u16 *)entry->out_value = val;
// 			break;
// 		case 4:
// 			*(u32 *)entry->out_value = val;
// 			break;
// 		case 5:
// 			*(bool *)entry->out_value = val;
// 			break;
// 		case 8:
// 			*(u64 *)entry->out_value = val;
// 			break;
// 		default:
// 			ret = -EINVAL;
// 		}
// 	}

// 	return count;
// }

// static const struct file_operations ad9088_debugfs_reg_fops = {
// 	.open = simple_open,
// 	.read = ad9088_debugfs_read,
// 	.write = ad9088_debugfs_write,
// };

// static void ad9088_add_debugfs_entry(struct iio_dev *indio_dev,
// 				     const char *propname, unsigned int cmd)
// {
// 	struct axiadc_converter *conv = iio_device_get_drvdata(indio_dev);
// 	struct ad9088_phy *phy = conv->phy;
// 	unsigned int i = phy->ad9088_debugfs_entry_index;

// 	if (WARN_ON(i >= ARRAY_SIZE(phy->debugfs_entry)))
// 		return;

// 	phy->debugfs_entry[i].indio_dev = indio_dev;
// 	phy->debugfs_entry[i].propname = propname;
// 	phy->debugfs_entry[i].cmd = cmd;

// 	phy->ad9088_debugfs_entry_index++;
// }

// static int ad9088_post_iio_register(struct iio_dev *indio_dev)
// {
// 	struct axiadc_converter *conv = iio_device_get_drvdata(indio_dev);
// 	struct ad9088_phy *phy = conv->phy;
// 	int i, ret;

// 	if (iio_get_debugfs_dentry(indio_dev)) {
// 		debugfs_create_devm_seqfile(&conv->spi->dev, "status",
// 					    iio_get_debugfs_dentry(indio_dev),
// 					    ad9088_status_show);

// 		ad9088_add_debugfs_entry(indio_dev,
// 			"bist_prbs_select_jrx", DBGFS_BIST_PRBS_JRX);
// 		ad9088_add_debugfs_entry(indio_dev,
// 			"bist_prbs_select_jtx", DBGFS_BIST_PRBS_JTX);
// 		ad9088_add_debugfs_entry(indio_dev,
// 			"bist_prbs_error_counters_jrx", DBGFS_BIST_PRBS_JRX_ERR);
// 		ad9088_add_debugfs_entry(indio_dev,
// 			"bist_2d_eyescan_jrx", DBGFS_BIST_JRX_2D_EYE);
// 		ad9088_add_debugfs_entry(indio_dev,
// 					 "api_version", DBGFS_DEV_API_INFO);
// 		ad9088_add_debugfs_entry(indio_dev,
// 					 "uuid", DBGFS_DEV_UUID_INFO);
// 		ad9088_add_debugfs_entry(indio_dev,
// 					 "chip_version", DBGFS_DEV_CHIP_INFO);
// 		ad9088_add_debugfs_entry(indio_dev,
// 					 "clk_pwr_stat", DBGFS_CLK_PWR_STAT);
// 		ad9088_add_debugfs_entry(indio_dev,
// 					 "temperature_status", DBGFS_DEV_TEMP_INFO);
// 		ad9088_add_debugfs_entry(indio_dev,
// 					 "hsci_enable", DBGFS_HSCI_ENABLE);
// 		ad9088_add_debugfs_entry(indio_dev,
// 					 "trig_sync", DBGFS_TRIG_SYNC);
// 		ad9088_add_debugfs_entry(indio_dev,
// 					 "generic", DBGFS_GENERIC);
// 		ad9088_add_debugfs_entry(indio_dev,
// 						"jrx_phase_adjust_calc", DBGFS_JRX_PHASE_ADJUST_CALC);

// 		for (i = 0; i < phy->ad9088_debugfs_entry_index; i++)
// 			debugfs_create_file( phy->debugfs_entry[i].propname, 0644,
// 					     iio_get_debugfs_dentry(indio_dev),
// 					     &phy->debugfs_entry[i],
// 					     &ad9088_debugfs_reg_fops);

// 	}

// 	sysfs_bin_attr_init(&phy->pfilt);
// 	phy->pfilt.attr.name = "pfilt_config";
// 	phy->pfilt.attr.mode = S_IWUSR;
// 	phy->pfilt.write = ad9088_pfilt_bin_write;
// 	phy->pfilt.size = 4096;

// 	ret = sysfs_create_bin_file(&indio_dev->dev.kobj, &phy->pfilt);
// 	if (ret)
// 		return ret;

// 	sysfs_bin_attr_init(&phy->cfir);
// 	phy->cfir.attr.name = "cfir_config";
// 	phy->cfir.attr.mode = S_IWUSR;
// 	phy->cfir.write = ad9088_cfir_bin_write;
// 	phy->cfir.size = 4096;

// 	return sysfs_create_bin_file(&indio_dev->dev.kobj, &phy->cfir);
// }

// #if 0
// static int ad9088_reg_from_phandle(struct ad9088_phy *phy,
// 				   const struct device_node *np,
// 				   const char *phandle_name, int index,
// 				   u32 *reg)
// {
// 	int ret;
// 	struct device_node *ph = of_parse_phandle(np, phandle_name, index);

// 	if (np == NULL)
// 		return -EINVAL;

// 	ret = of_property_read_u32(ph, "reg", reg);

// 	of_node_put(ph);

// 	return ret;
// }
// #endif

// static char* ad9088_label_writer(struct ad9088_phy *phy, const struct iio_chan_spec *chan)
// {
// 	u8 cddc_num, fddc_num, side;
// 	u32 cddc_mask, fddc_mask;

// 	ad9088_iiochan_to_fddc_cddc(phy, chan, &fddc_num, &fddc_mask, &cddc_num, &cddc_mask, &side);

// 	if (chan->output) {
// 		snprintf(phy->tx_chan_labels[chan->channel], sizeof(phy->tx_chan_labels[0]),
// 			 "Side-%c:FDUC%u->CDUC%u->DAC%u", side ? 'B' : 'A', fddc_num, cddc_num, cddc_num);

// 		return phy->tx_chan_labels[chan->channel];

// 	}

// 	snprintf(phy->rx_chan_labels[chan->channel], sizeof(phy->rx_chan_labels[0]),
// 		 "Side-%c:FDDC%u->CDDC%u->ADC%u", side ? 'B' : 'A', fddc_num, cddc_num, cddc_num);

// 	return phy->rx_chan_labels[chan->channel];
// }

// static int ad9088_setup_chip_info_tbl(struct ad9088_phy *phy,
// 				      bool complex_rx, bool complex_tx, bool buffer_capable)
// {
// 	int i, c, m, s, l;

// 	for (m = 0, s = 0; s < ADI_APOLLO_NUM_SIDES; s++)
// 		for (l = 0; l < ADI_APOLLO_JESD_LINKS; l++)
// 			if (phy->profile.jtx[s].tx_link_cfg[l].link_in_use)
// 				m += phy->profile.jtx[s].tx_link_cfg[l].m_minus1 + 1;


// 	// switch (m) {
// 	// case 0:
// 	// case 2:
// 	// case 4:
// 	// case 6:
// 	// case 8:
// 	// case 12:
// 	// case 16:
// 	//	break;
// 	// default:
// 	//	return -EINVAL;
// 	// }

// 	phy->rx_labels = devm_kcalloc(&phy->spi->dev,
// 				      m * phy->multidevice_instance_count,
// 				      sizeof(*phy->rx_labels), GFP_KERNEL);
// 	if (!phy->rx_labels)
// 		return -ENOMEM;

// 	for (c = 0, i = 0; i < (m * phy->multidevice_instance_count);
// 	     i++, c++) {
// 		phy->chip_info.channel[c].type = IIO_VOLTAGE;
// 		phy->chip_info.channel[c].output = 0;
// 		phy->chip_info.channel[c].indexed = 1;
// 		phy->chip_info.channel[c].modified = complex_rx ? 1 : 0;
// 		phy->chip_info.channel[c].channel = complex_rx ? i / 2 : i;
// 		phy->chip_info.channel[c].channel2 =
// 			(i & 1) ? IIO_MOD_Q : IIO_MOD_I;

// 		phy->chip_info.channel[c].scan_index = buffer_capable ? i : -1;

// 		if (phy->side_b_use_own_tpl_en &&
// 			(i >= ((phy->profile.jtx[0].tx_link_cfg[0].m_minus1 + 1) *
// 			phy->multidevice_instance_count)))
// 			phy->chip_info.channel[c].scan_index = -1;

// 		phy->chip_info.channel[c].address = i,
// 					 phy->chip_info.channel[c].info_mask_shared_by_type =
// 						 BIT(IIO_CHAN_INFO_SAMP_FREQ);

// 		phy->chip_info.channel[c].scan_type.realbits =
// 			phy->profile.jtx[0].tx_link_cfg[0].n_minus1 + 1;
// 		phy->chip_info.channel[c].scan_type.storagebits =
// 			(phy->profile.jtx[0].tx_link_cfg[0].np_minus1 + 1 > 8) ? 16 : 8;
// 		phy->chip_info.channel[c].scan_type.sign = 's';

// 		if (i < m) {
// 			phy->chip_info.channel[c].ext_info = rxadc_ext_info;
// 			phy->chip_info.channel[c].info_mask_separate =
// 				BIT(IIO_CHAN_INFO_ENABLE);
// 			phy->rx_labels[phy->chip_info.channel[c].channel] =
// 				ad9088_label_writer(phy, &phy->chip_info.channel[c]);
// 		} else {
// 			phy->rx_labels[phy->chip_info.channel[c].channel] = "buffer_only";
// 		}
// 	}

// 	// m = phy->jrx_link_tx[0].jesd_param.jesd_m *
// 	//	(ad9088_link_is_dual(phy->jrx_link_tx) ? 2 : 1);


// 	for (m = 0, s = 0; s < ADI_APOLLO_NUM_SIDES; s++)
// 		for (l = 0; l < ADI_APOLLO_JESD_LINKS; l++)
// 			if (phy->profile.jrx[s].rx_link_cfg[l].link_in_use)
// 				m += phy->profile.jrx[s].rx_link_cfg[l].m_minus1 + 1;

// 	//m = phy->profile.jrx[0].rx_link_cfg[0].m_minus1 + 1;

// 	phy->tx_labels = devm_kcalloc(&phy->spi->dev, m,
// 				      sizeof(*phy->tx_labels), GFP_KERNEL);
// 	if (!phy->tx_labels)
// 		return -ENOMEM;

// 	for (i = 0; i < m; i++, c++) {
// 		phy->chip_info.channel[c].type = IIO_VOLTAGE;
// 		phy->chip_info.channel[c].output = 1;
// 		phy->chip_info.channel[c].indexed = 1;
// 		phy->chip_info.channel[c].modified = complex_tx ? 1 : 0;
// 		phy->chip_info.channel[c].channel = complex_tx ? i / 2 : i;
// 		phy->chip_info.channel[c].channel2 =
// 			(i & 1) ? IIO_MOD_Q : IIO_MOD_I;
// 		phy->chip_info.channel[c].scan_index = -1;
// 		phy->chip_info.channel[c].info_mask_shared_by_type =
// 			BIT(IIO_CHAN_INFO_SAMP_FREQ);

// 		phy->chip_info.channel[c].info_mask_separate =
// 			BIT(IIO_CHAN_INFO_ENABLE);

// 		phy->chip_info.channel[c].ext_info = txdac_ext_info;
// 		phy->tx_labels[phy->chip_info.channel[c].channel] =
// 			ad9088_label_writer(phy, &phy->chip_info.channel[c]);
// 	}

// 	phy->chip_info.channel[c].type = IIO_TEMP;
// 	phy->chip_info.channel[c].indexed = 1;
// 	phy->chip_info.channel[c].info_mask_separate = BIT(IIO_CHAN_INFO_PROCESSED);
// 	phy->chip_info.channel[c].scan_index = -1;
// 	phy->chip_info.channel[c].ext_info = NULL;
// 	c++;

// 	phy->chip_info.num_channels = c;
// 	phy->chip_info.name = "AD9088";
// 	phy->chip_info.max_rate = 28000000000UL;

// 	return 0;
// }

// static const struct iio_info ad9088_iio_info = {
// 	.read_raw = &ad9088_read_raw,
// 	.write_raw = &ad9088_write_raw,
// 	.read_label = &ad9088_read_label,
// 	.debugfs_reg_access = &ad9088_reg_access,
// 	.attrs = &ad9088_phy_attribute_group,
// };

// static int ad9088_register_iiodev(struct axiadc_converter *conv)
// {
// 	struct iio_dev *indio_dev;
// 	struct spi_device *spi = conv->spi;
// 	struct ad9088_phy *phy = conv->phy;
// 	int ret;

// 	indio_dev = devm_iio_device_alloc(&spi->dev, 0);
// 	if (!indio_dev)
// 		return -ENOMEM;

// 	iio_device_set_drvdata(indio_dev, conv);

// 	indio_dev->dev.parent = &spi->dev;

// 	if (spi->dev.of_node)
// 		indio_dev->name = spi->dev.of_node->name;
// 	else
// 		indio_dev->name = spi_get_device_id(spi)->name;

// 	indio_dev->info = &ad9088_iio_info;
// 	indio_dev->modes = INDIO_DIRECT_MODE;
// 	indio_dev->channels = phy->chip_info.channel;
// 	indio_dev->num_channels = phy->chip_info.num_channels;

// 	ret = iio_device_register(indio_dev);
// 	ad9088_post_iio_register(indio_dev);

// 	conv->indio_dev = indio_dev;

// 	return ret;
// }

// static int ad9088_version_info(struct ad9088_phy *phy)
// {
// 	adi_apollo_mailbox_resp_get_fw_version_t fw_ver;
// 	adi_apollo_mailbox_cmd_ping_t ping_cmd = {.echo_data = 0x12345678};
// 	adi_apollo_mailbox_resp_ping_t ping_resp;
// 	adi_apollo_device_t *device =  &phy->ad9088;
// 	int ret;
// 	uint16_t maj, min, rc;

// 	ret = adi_apollo_device_api_revision_get(device, &maj, &min, &rc);
// 	if (ret) {
// 		dev_err(&phy->spi->dev, "API revision get error (%d)\n", ret);
// 		return ret;
// 	}

// 	dev_info(&phy->spi->dev, "API ver: %d.%d.%d\n", maj, min, rc);

// 	ret = adi_apollo_mailbox_ready_check(device);
// 	if (ret) {
// 		dev_err(&phy->spi->dev, "Mailbox Ready error (%d)\n", ret);
// 		return ret;
// 	}

// 	ret = adi_apollo_mailbox_ping(device, &ping_cmd, &ping_resp);
// 	if (ret) {
// 		dev_err(&phy->spi->dev, "Mailbox ping error (%d)\n", ret);
// 		return ret;
// 	}

// 	dev_info(&phy->spi->dev, "Ping cmd/res: 0x%08x/0x%08x\n",
// 		 ping_cmd.echo_data, ping_resp.echo_data);

// 	ret = adi_apollo_mailbox_get_fw_version(device, &fw_ver);
// 	if (ret) {
// 		dev_err(&phy->spi->dev, "Mailbox fw error (%d)\n", ret);
// 		return ret;
// 	}

// 	dev_info(&phy->spi->dev, "FW ver: %04d%02d%02d.%d.%d\n",
// 		 fw_ver.year, fw_ver.month, fw_ver.day, fw_ver.minor, fw_ver.build);

// 	return ret;
// }

// int32_t ad9088_inspect_jrx_link_all(struct ad9088_phy *phy)
// {
// 	int err;
// 	adi_apollo_device_t *device =  &phy->ad9088;
// 	adi_apollo_jesd_rx_inspect_t jrx_status;
// 	uint16_t links_to_inspect[] = { ADI_APOLLO_LINK_A0, ADI_APOLLO_LINK_A1, ADI_APOLLO_LINK_B0, ADI_APOLLO_LINK_B1 };
// 	const char * links_to_inspect_str[] = { "A0", "A1", "B0", "B1" };
// 	int i, l;
// 	u8 phys_lane;

// 	phy->jrx_lanes_used = 0;

// 	for (l = 0; l < sizeof(links_to_inspect) / sizeof(links_to_inspect[0]); l++) {
// 		if (err = adi_apollo_jrx_link_inspect(device, links_to_inspect[l], &jrx_status), err != API_CMS_ERROR_OK) {
// 			dev_err(&phy->spi->dev,"Error from adi_apollo_jrx_link_inspect() %d", err);
// 			return err;
// 		}
// 		dev_info(&phy->spi->dev, "JRX ADI_APOLLO_LINK_%s: L=%2d M=%2d F=%2d S=%2d Np=%2d CS=%2d link_en= %-8s\n",
// 			links_to_inspect_str[l],
// 			jrx_status.l_minus1 + 1,
// 			jrx_status.m_minus1 + 1,
// 			jrx_status.f_minus1 + 1,
// 			jrx_status.s_minus1 + 1,
// 			jrx_status.np_minus1 + 1,
// 			jrx_status.cs,
// 			jrx_status.link_en ? "Enabled" : "Disabled");

// 		if (jrx_status.link_en)
// 			for (i = 0; i < jrx_status.l_minus1 + 1; i++) {
// 				phys_lane = phy->profile.jrx[(l / 2) & 1].rx_link_cfg[(l % 2) & 1].lane_xbar[i];
// 				phy->jrx_lanes[phy->jrx_lanes_used++] = phys_lane + (((l / 2) & 1) * 12);
// 			}

// 	}

// 	return API_CMS_ERROR_OK;
// }

// int32_t ad9088_inspect_jtx_link_all(struct ad9088_phy *phy)
// {
// 	int err;
// 	adi_apollo_device_t *device =  &phy->ad9088;
// 	adi_apollo_jesd_tx_inspect_t jtx_status;
// 	uint16_t links_to_inspect[] = { ADI_APOLLO_LINK_A0, ADI_APOLLO_LINK_A1, ADI_APOLLO_LINK_B0, ADI_APOLLO_LINK_B1 };
// 	const char * links_to_inspect_str[] = { "A0", "A1", "B0", "B1" };
// 	int l;

// 	for (l = 0; l < sizeof(links_to_inspect) / sizeof(links_to_inspect[0]); l++) {
// 		if (err = adi_apollo_jtx_link_inspect(device, links_to_inspect[l], &jtx_status), err != API_CMS_ERROR_OK) {
// 			dev_err(&phy->spi->dev,"Error from adi_apollo_jtx_link_inspect() %d", err);
// 			return err;
// 		}
// 		dev_info(&phy->spi->dev, "JTX ADI_APOLLO_LINK_%s: L=%2d M=%2d F=%2d S=%2d Np=%2d CS=%2d link_en= %-8s\n",
// 			 links_to_inspect_str[l],
// 			 jtx_status.l_minus1+1,
// 			 jtx_status.m_minus1+1,
// 			 jtx_status.f_minus1+1,
// 			 jtx_status.s_minus1+1,
// 			 jtx_status.np_minus1+1,
// 			 jtx_status.cs,
// 			 jtx_status.link_en ? "Enabled" : "Disabled");
// 	}

// 	return API_CMS_ERROR_OK;
// }

// // static int32_t ad9088_print_clock_cond_caldata(struct ad9088_phy *phy, adi_apollo_sides_e side, bool verbose)
// // {
// //	adi_apollo_device_t *device = &phy->ad9088;
// //	int err, i;
// //	adi_apollo_sysclk_cond_caldata_t caldata;

// //	if (err = adi_apollo_sysclk_cond_caldata_get(device, side, &caldata), err != API_CMS_ERROR_OK)
// //	{
// //		dev_info(&phy->spi->dev, "Error from adi_apollo_sysclk_cond_caldata_get() %d\n", err);
// //		return err;
// //	}

// //	dev_info(&phy->spi->dev, "Clock Conditioning Cal Data for Side %s:\n", side == ADI_APOLLO_SIDE_IDX_A ? "A" : "B");
// //	dev_info(&phy->spi->dev, "adc01_recommended_offset = %d\n", caldata.adc01_recommended_offset);
// //	dev_info(&phy->spi->dev, "total_valid_offsets = %d\n", caldata.total_valid_offsets);
// //	dev_info(&phy->spi->dev, "current_offset = %d\n", caldata.current_offset);
// //	dev_info(&phy->spi->dev, "adc0_min_offset = %d\n", caldata.adc0_min_offset);
// //	dev_info(&phy->spi->dev, "adc1_min_offset = %d\n", caldata.adc1_min_offset);
// //	dev_info(&phy->spi->dev, "adc0_max_offset = %d\n", caldata.adc0_max_offset);
// //	dev_info(&phy->spi->dev, "adc1_max_offset = %d\n", caldata.adc1_max_offset);

// //	if (verbose) {
// //		for (i = 0; i < 6; i++) {
// //			dev_info(&phy->spi->dev, "adc01_calcoeffs[%d] = %d\n", i, caldata.adc01_calcoeffs[i]);
// //		}
// //		for (i = 0; i < 41; i++) {
// //			dev_info(&phy->spi->dev, "adc_std_combined_measurements[%d] = %llu\n", i, caldata.adc_std_combined_measurements[i]);
// //		}
// //	}
// //	return API_CMS_ERROR_OK;
// // }

// static int ad9088_tye_fw_load(struct ad9088_phy *phy, u8 scenario)
// {
// 	struct spi_device *spi = phy->spi;
// 	adi_apollo_device_t *device = &phy->ad9088;
// 	int  ret;
// 	u8 si_grade;
// 	char *secr_hdr_filename;
// 	char *core0_fw_filename;
// 	char *core1_fw_filename;
// 	char *oper_fw_filename;

// 	ret  = adi_apollo_device_si_grade_get(device, &si_grade);
// 	if (ret) {
// 		dev_err(&spi->dev, "adi_apollo_device_si_grade_get error (%d)\n", ret);
// 		return ret;
// 	}

// 	dev_info(&spi->dev, "Apollo Silicon Grade = %02d\n", si_grade);

// 	dev_dbg(&spi->dev, "Running scenario = %d\n", scenario);

// 	secr_hdr_filename = (si_grade == 0) ? SECR_BOOT_HDR_BIN : PROD_SECR_BOOT_HDR_BIN;
// 	core0_fw_filename = (si_grade == 0) ? CORE_0_TYE_FW_BIN : PROD_CORE_0_TYE_FW_BIN;
// 	core1_fw_filename = (si_grade == 0) ? CORE_1_TYE_FW_BIN : PROD_CORE_1_TYE_FW_BIN;
// 	oper_fw_filename = (si_grade == 0) ? TYE_OPER_FW_BIN : PROD_TYE_OPER_FW_BIN;


// 	ret = adi_apollo_arm_fwload_pre_config(device);
// 	if (ret) {
// 		dev_err(&spi->dev, "adi_apollo_arm_fwload_pre_config error (%d)\n", ret);
// 		return ret;
// 	}

// 	/* Load the firmware header to memory. */
// 	ret = request_firmware(&phy->fw, secr_hdr_filename, &spi->dev);
// 	if (!ret) {
// 		ret = adi_apollo_arm_tye_firmware_buf_write(device, SECR_BOOT_HDR_MEM_ADDR, (u8 *) phy->fw->data, phy->fw->size, FW_TRANSFER_CHUNK_SIZE);
// 		release_firmware(phy->fw);

// 		if (ret) {
// 			dev_err(&spi->dev, "adi_apollo_arm_tye_firmware_buf_write SECR_BOOT_HDR_BIN error (%d)\n", ret);
// 			return ret;
// 		}
// 	}

// 	/* Scenarios 2 and 4 app_pack contains the core 0 firmware image. */
// 	if (scenario == 2 || scenario == 4) {
// 		ret = request_firmware(&phy->fw, core0_fw_filename, &spi->dev);
// 		if (!ret) {
// 			ret = adi_apollo_arm_tye_firmware_buf_write(device, CORE_0_TYE_FW_MEM_ADDR, (u8 *) phy->fw->data, phy->fw->size, FW_TRANSFER_CHUNK_SIZE);
// 			release_firmware(phy->fw);

// 			if (ret) {
// 				dev_err(&spi->dev, "adi_apollo_arm_tye_firmware_buf_write CORE_0_TYE_FW_BIN error (%d)\n", ret);
// 				return ret;
// 			}
// 		}
// 	}

// 	/* Scenarios 3 and 4 app_pack contains the core 1 firmware image. */
// 	if (scenario == 3 || scenario == 4) {
// 		ret = request_firmware(&phy->fw, core1_fw_filename, &spi->dev);
// 		if (!ret) {
// 			ret = adi_apollo_arm_tye_firmware_buf_write(device, CORE_1_TYE_FW_MEM_ADDR, (u8 *) phy->fw->data, phy->fw->size, FW_TRANSFER_CHUNK_SIZE);
// 			release_firmware(phy->fw);

// 			if (ret) {
// 				dev_err(&spi->dev, "adi_apollo_arm_tye_firmware_buf_write CORE_1_TYE_FW_BIN error (%d)\n", ret);
// 				return ret;
// 			}
// 		}
// 	}

// 	/* Load the operation firmware image to memory */
// 	ret = request_firmware(&phy->fw, oper_fw_filename, &spi->dev);
// 	if (!ret) {
// 		ret = adi_apollo_arm_tye_firmware_buf_write(device, TYE_OPER_FW_MEM_ADDR, (u8 *) phy->fw->data, phy->fw->size, FW_TRANSFER_CHUNK_SIZE);
// 		release_firmware(phy->fw);
// 		if (ret) {
// 			dev_err(&spi->dev, "adi_apollo_arm_tye_firmware_buf_write TYE_OPER_FW_BIN error (%d)\n", ret);
// 			return ret;
// 		}
// 	}

// 	/* Set boot ready bit*/
// 	ret = adi_apollo_arm_tye_boot_ready_set(device);
// 	if (ret) {
// 		dev_err(&spi->dev, "adi_apollo_arm_tye_boot_ready_set error (%d)\n", ret);
// 		return ret;
// 	}

// 	/* Delay 0.1s */
// 	adi_apollo_hal_delay_us(device, 1 * 10000);

// 	ret = adi_apollo_arm_fwload_post_config(device);
// 	if (ret) {
// 		dev_err(&spi->dev, "adi_apollo_arm_fwload_post_config error (%d)\n", ret);
// 		return ret;
// 	}

// 	ret = adi_apollo_arm_tye_validate_boot_completion(device, 1);
// 	if (ret) {
// 		dev_err(&spi->dev, "adi_apollo_arm_tye_validate_boot_completion error (%d)\n", ret);
// 		return ret;
// 	}

// 	return ret;
// }

// static int ad9088_setup(struct ad9088_phy *phy)
// {
// 	adi_apollo_top_t *profile = &phy->profile;
// 	struct spi_device *spi = phy->spi;
// 	//struct axiadc_converter *conv = spi_get_drvdata(spi);
// 	adi_apollo_device_t *device = &phy->ad9088;
// 	//u64 dev_frequency_hz;
// 	u64 sample_rate;
// 	const char *name;
// 	int ret, i;
// 	u8 boot_status = 0, tye_bypassed = 0;
// 	u8 num_tries = 10;
// 	uint32_t device_profile_start_addr;
// 	adi_apollo_sniffer_param_t *sniffer_config = &phy->sniffer_config;

// 	if (of_property_read_string(spi->dev.of_node, "arm-firmware-name", &name))
// 		switch (CHIPID_AD9084) { /* Fixme */
// 		case CHIPID_AD9084:
// 			name = APOLLO_FW_CPU1_NAME;
// 			break;
// 		default:
// 			return -EINVAL;
// 	}

// 	/*
// 	* Defer ADC foreground cal (fgcal) until clock conditioning calibration is completed later in startup sequence.
// 	* Instruct FW to load ADC cal coefficients from Apollo non-volatile-memory (NVM).
// 	* NVM contains factory calibrated coefficients which results in faster ADC cal convergence.
// 	*/

// 	/* ADI_APOLLO_INIT_CAL_ENABLED */

// 	ret = adi_apollo_cfg_adc_init_cal_cfg_set(device, ADI_APOLLO_ADC_ALL, ADI_APOLLO_INIT_CAL_DISABLED_WARMBOOT_FROM_NVM /*ADI_APOLLO_INIT_CAL_DISABLED_WARMBOOT_FROM_NVM*/);
// 	if (ret) {
// 		adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Error from adi_apollo_cfg_adc_init_cal_cfg_set() %d", ret);
// 		return ret;
// 	}

// 	// ret = adi_apollo_cfg_dac_init_cal_cfg_set(device, ADI_APOLLO_DAC_ALL, ADI_APOLLO_INIT_CAL_ENABLED_WARMBOOT_FROM_NVM /*ADI_APOLLO_INIT_CAL_DISABLED_WARMBOOT_FROM_NVM*/);
// 	// if (ret) {
// 	//	adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Error from adi_apollo_cfg_adc_init_cal_cfg_set() %d", ret);
// 	//	return ret;
// 	// }

// 	ret = adi_apollo_arm_tye_bypassed_get(device, &tye_bypassed);
// 	if (ret) {
// 		dev_err(&spi->dev,
// 			"adi_apollo_arm_tye_bypassed_get() failed with %d\n", ret);
// 		return ret;
// 	}

// 	dev_info(&spi->dev, "tye_bypassed: %d\n", tye_bypassed);

// 	if (tye_bypassed) {
// 		ret = request_firmware(&phy->fw, name, &spi->dev);
// 		if (ret) {
// 			dev_err(&spi->dev,
// 				"request_firmware() failed with %d\n", ret);
// 			return ret;
// 		}

// 		ret = adi_apollo_arm_fwload_pre_config(device);
// 		if (ret) {
// 			dev_err(&spi->dev, "adi_apollo_arm_fwload_pre_config error (%d)\n", ret);
// 			return ret;
// 		}

// 		ret = adi_apollo_arm_firmware_write(device, (u8 *) phy->fw->data, phy->fw->size, FW_TRANSFER_CHUNK_SIZE);
// 		/* Load the firmware binary into the ARM */
// 		if (ret) {
// 			dev_err(&spi->dev, "adi_apollo_arm_firmware_write error (%d)\n", ret);
// 			return ret;
// 		}

// 		release_firmware(phy->fw);


// 		ret = request_firmware(&phy->fw, APOLLO_FW_CPU0_NAME, &spi->dev);
// 		if (!ret) {
// 			ret = adi_apollo_arm_firmware_buf_write(device, ADI_APOLLO_CPU_ID_0,
// 								(u8 *) phy->fw->data, phy->fw->size, FW_TRANSFER_CHUNK_SIZE);
// 			release_firmware(phy->fw);
// 			if (ret) {
// 				dev_err(&spi->dev, "adi_apollo_arm_firmware_buf_write error (%d)\n", ret);
// 				return ret;
// 			}
// 		}

// 		ret = adi_apollo_arm_fwload_post_config(device);
// 		if (ret) {
// 			dev_err(&spi->dev, "adi_apollo_arm_fwload_post_config error (%d)\n", ret);
// 			return ret;
// 		}
// 	} else {
// 		ret = ad9088_tye_fw_load(phy, 4 /*TYE_USER_SCENARIO*/);
// 		if (ret) {
// 			dev_err(&spi->dev, "adi_apollo_arm_tye_fw_load error (%d)\n", ret);
// 			return ret;
// 		}
// 	}

// 	/* Wait for FW ready to accept profile */

// 	do {
// 		adi_apollo_hal_delay_us(device, 1 * 500000);
// 		adi_apollo_arm_boot_status(device, &boot_status);
// 	} while (boot_status != ADI_APOLLO_RAM_BOOT_STEP_WAIT_FOR_CONFIG && num_tries--);
// 	if (boot_status != ADI_APOLLO_RAM_BOOT_STEP_WAIT_FOR_CONFIG) {
// 		dev_err(&spi->dev, "FW didn't reach wait for device profile state. boot_status: 0x%x\n", boot_status);
// 		return -EFAULT;
// 	}
// 	/*
// 	* Load the device profile use case into the firmware (arm)
// 	*
// 	* Firmware configures:
// 	*   - Clocks and PLLs
// 	*   - JESD JRx and JTx
// 	*
// 	* The API will configure other functional blocks using the same device profile
// 	*/

// 	dev_info(&spi->dev,"Loading device profile (ver=%d.%d.%d, %s)...\n",
// 		profile->profile_cfg.profile_version.major,
// 		profile->profile_cfg.profile_version.minor,
// 		profile->profile_cfg.profile_version.patch,
// 		profile->profile_cfg.is_8t8r ? "8T8R" : "4T4R");

// 	ret = adi_apollo_hal_bf_set(device, BF_CONFIG_TRANSFER_DONE, 0);
// 	if (ret) {
// 		dev_err(&spi->dev, "BF_CONFIG_TRANSFER_DONE error (%d)\n", ret);
// 		return ret;
// 	}

// 	ret = adi_apollo_arm_boot_status(device, &boot_status);
// 	if (ret) {
// 		dev_err(&spi->dev, "adi_apollo_arm_boot_status error (%d)\n", ret);
// 		return ret;
// 	}

// 	/* Write the profile configuration to ARM memory for firmware processing */
// 	ret = adi_apollo_arm_profile_write(device, profile, 16 * 1024);
// 	if (ret) {
// 		dev_err(&spi->dev, "adi_apollo_arm_profile_write error (%d)\n", ret);
// 		return ret;
// 	}

// 	adi_apollo_hal_reg32_get(device, ADI_APOLLO_FW_DEVICE_PROFILE_PTR, &device_profile_start_addr);

// 	/* Notify firmware that profile load has completed */
// 	ret = adi_apollo_arm_profile_write_post_config(device, profile);
// 	if (ret) {
// 		adi_apollo_arm_boot_status(device, &boot_status);
// 		dev_err(&spi->dev, "Profile write completed error code: %d\n", boot_status);
// 		return ret;
// 	}

// 	/* Verify FW mailbox is ready */
// 	boot_status = 0;
// 	num_tries = 10;
// 	do {
// 		adi_apollo_hal_delay_us(device, 1 * 500000);
// 		adi_apollo_arm_boot_status(device, &boot_status);
// 		//printf("ARM boot status: 0x%x\n", boot_status);
// 	} while (boot_status != ADI_APOLLO_CPU_BOOT_MAILBOX_READY && num_tries--);

// 	if (boot_status != ADI_APOLLO_CPU_BOOT_MAILBOX_READY) {
// 		dev_err(&spi->dev, "FW didn't reach mailbox ready state. boot_status: 0x%x\n",
// 			boot_status);
// 		return -EFAULT;
// 	}

// 	/* Display API and Firmware revision */
// 	ret = ad9088_version_info(phy);
// 	if (ret) {
// 		dev_err(&spi->dev,
// 			"Error displaying version info. This may indicate device clock is incorrect (%d)\n",
// 			ret);
// 		return ret;
// 	}

// 	ad9088_rx_sniffer_populate_default_params(ADI_APOLLO_SNIFFER_INSTANT_MAGNITUDE, sniffer_config);
// 	ret = ad9088_rx_sniffer_setup(phy, ADI_APOLLO_SIDE_ALL, &sniffer_config->init);
// 	if (ret) {
// 		dev_err(&phy->spi->dev, "Error in ad9088_rx_sniffer_setup: %d\n", ret);
// 	}

// 	/* Enable TMU */
// 	ret = adi_apollo_device_tmu_enable(device);
// 	if (ret) {
// 		dev_err(&phy->spi->dev, "Error enabling tmu (%d)\n", ret);
// 	}

// 	/* Configure the Rx and Tx data paths as specified in the device profile */

// 	ret = adi_apollo_cfg_data_path(device, profile);
// 	if (ret) {
// 		dev_err(&spi->dev, "adi_apollo_cfg_data_path error (%d)\n", ret);
// 		return ret;
// 	}

// 	sample_rate = DIV_ROUND_CLOSEST_ULL(phy->profile.dac_config[0].dac_sampling_rate_Hz,
// 					    phy->profile.jrx[0].rx_link_cfg[0].link_total_ratio);
// 	clk_set_rate_scaled(phy->clks[TX_SAMPL_CLK], sample_rate,
// 			    &phy->clkscale[TX_SAMPL_CLK]);

// 	sample_rate = DIV_ROUND_CLOSEST_ULL(phy->profile.adc_config[0].adc_sampling_rate_Hz,
// 					    phy->profile.jtx[0].tx_link_cfg[0].link_total_ratio);
// 	clk_set_rate_scaled(phy->clks[RX_SAMPL_CLK], sample_rate,
// 			    &phy->clkscale[RX_SAMPL_CLK]);

// 	if (0) {
// 		static uint32_t serdes_tx_dig_lane_regmap[] = {
// 			SER_PHY0_SERDES_TX_JTX_TOP_RX_DIGITAL0, SER_PHY1_SERDES_TX_JTX_TOP_RX_DIGITAL0,
// 			SER_PHY2_SERDES_TX_JTX_TOP_RX_DIGITAL0, SER_PHY3_SERDES_TX_JTX_TOP_RX_DIGITAL0,
// 			SER_PHY4_SERDES_TX_JTX_TOP_RX_DIGITAL0, SER_PHY5_SERDES_TX_JTX_TOP_RX_DIGITAL0,
// 			SER_PHY6_SERDES_TX_JTX_TOP_RX_DIGITAL0, SER_PHY7_SERDES_TX_JTX_TOP_RX_DIGITAL0,
// 			SER_PHY8_SERDES_TX_JTX_TOP_RX_DIGITAL0, SER_PHY9_SERDES_TX_JTX_TOP_RX_DIGITAL0,
// 			SER_PHY10_SERDES_TX_JTX_TOP_RX_DIGITAL0, SER_PHY11_SERDES_TX_JTX_TOP_RX_DIGITAL0,
// 			SER_PHY0_SERDES_TX_JTX_TOP_RX_DIGITAL1, SER_PHY1_SERDES_TX_JTX_TOP_RX_DIGITAL1,
// 			SER_PHY2_SERDES_TX_JTX_TOP_RX_DIGITAL1, SER_PHY3_SERDES_TX_JTX_TOP_RX_DIGITAL1,
// 			SER_PHY4_SERDES_TX_JTX_TOP_RX_DIGITAL1, SER_PHY5_SERDES_TX_JTX_TOP_RX_DIGITAL1,
// 			SER_PHY6_SERDES_TX_JTX_TOP_RX_DIGITAL1, SER_PHY7_SERDES_TX_JTX_TOP_RX_DIGITAL1,
// 			SER_PHY8_SERDES_TX_JTX_TOP_RX_DIGITAL1, SER_PHY9_SERDES_TX_JTX_TOP_RX_DIGITAL1,
// 			SER_PHY10_SERDES_TX_JTX_TOP_RX_DIGITAL1, SER_PHY11_SERDES_TX_JTX_TOP_RX_DIGITAL1};

// 		for (i = 0; i < ARRAY_SIZE(serdes_tx_dig_lane_regmap); i++) {
// 			adi_apollo_hal_reg_set(device, serdes_tx_dig_lane_regmap[i] + 0x2, 0x40);
// 		}
// 	}

// 	return 0;
// }

// #if 0
// static u32  ad9088_jrx_linkup(struct ad9088_phy *phy, struct jesd204_link *lnk)
// {
// 	u8 is_link_up = true;
// 	u16 status = 0;
// 	int i;


// 	for (i = 0; i < lnk->num_lanes; i++) {
// 		adi_apollo_jrx_j204c_lane_status_get(&phy->ad9088, ad9088_to_link(lnk->link_id), i, &status);
// 		if (6 == (status & 0x7)) {
// 			pr_err("JRx lane status[%d]: Link ready\n", i);
// 		} else {
// 			is_link_up = false;
// 			pr_err("JRx lane status[%d]: Link not ready (%d)\n", i, status & 0x7);
// 		}
// 	}
// 	adi_apollo_jrx_link_status_get(&phy->ad9088, ad9088_to_link(lnk->link_id), &status);
// 	pr_err("JRx User Ready status: 0x%x\n", (status & 0x20) >> 5);
// 	pr_err("JRx SYSREF Phase lock: 0x%x\n", (status & 0x40) >> 6);
// 	pr_err("ad9088_to_link(lnk->link_id) 0x%x\n", ad9088_to_link(lnk->link_id));

// 	is_link_up &= ((status & 0x20) != 0) && ((status & 0x40) != 0);

// 	return is_link_up;
// }

// static bool ad9088_jtx_linkup(struct ad9088_phy *phy, struct jesd204_link *lnk)
// {
// 	u8 is_link_up = true;
// 	u16 status = 0;

// 	adi_apollo_jtx_link_status_get(&phy->ad9088, ad9088_to_link(lnk->link_id), &status);
// 	pr_err("JTx QBF status: 0x%x\n", (status & 0xF));
// 	pr_err("JTx frame sync status: 0x%x\n", !!(status & BIT(4)));
// 	pr_err("JTx pll locked: 0x%x\n", !!(status & BIT(5)));
// 	pr_err("JTx phase established: 0x%x\n", !!(status & BIT(6)));
// 	pr_err("JTx jtx invalid mode: 0x%x\n", !!(status & BIT(7)));
// 	pr_err("ad9088_to_link(lnk->link_id) 0x%x\n", ad9088_to_link(lnk->link_id));
// 	return 1;
// }
// #endif

// static int ad9088_jesd204_link_init(struct jesd204_dev *jdev,
// 				    enum jesd204_state_op_reason reason,
// 				    struct jesd204_link *lnk)
// {
// 	struct device *dev = jesd204_dev_to_device(jdev);
// 	struct ad9088_jesd204_priv *priv = jesd204_dev_priv(jdev);
// 	struct ad9088_phy *phy = priv->phy;
// 	adi_apollo_jesd_tx_cfg_t *jtx;
// 	adi_apollo_jesd_rx_cfg_t *jrx;
// 	u8 sideIdx, linkIdx;
// 	unsigned long lane_rate_kbps;
// 	int ret;

// 	switch (reason) {
// 	case JESD204_STATE_OP_REASON_INIT:
// 		break;
// 	default:
// 		return JESD204_STATE_CHANGE_DONE;
// 	}

// 	dev_dbg(dev, "%s:%d link_num %u reason %s\n", __func__, __LINE__,
// 		lnk->link_id, jesd204_state_op_reason_str(reason));

// 	switch (lnk->link_id) {
// 	case DEFRAMER_LINK_A0_TX:
// 	case DEFRAMER_LINK_A1_TX:
// 	case DEFRAMER_LINK_B0_TX:
// 	case DEFRAMER_LINK_B1_TX:
// 		sideIdx = (lnk->link_id - DEFRAMER_LINK_A0_TX) / 2;
// 		linkIdx = (lnk->link_id - DEFRAMER_LINK_A0_TX) % 2;

// 		jrx = &phy->profile.jrx[sideIdx];

// 		lnk->is_transmit = 1;
// 		lnk->num_lanes = jrx->rx_link_cfg[linkIdx].l_minus1 + 1;
// 		lnk->num_converters = jrx->rx_link_cfg[linkIdx].m_minus1 + 1;
// 		lnk->octets_per_frame = jrx->rx_link_cfg[linkIdx].f_minus1 + 1;
// 		lnk->frames_per_multiframe = jrx->rx_link_cfg[linkIdx].k_minus1 + 1;
// 		lnk->num_of_multiblocks_in_emb = jrx->rx_link_cfg[linkIdx].e_minus1 + 1;
// 		lnk->bits_per_sample = jrx->rx_link_cfg[linkIdx].np_minus1 + 1;
// 		lnk->converter_resolution = jrx->rx_link_cfg[linkIdx].n_minus1 + 1;
// 		lnk->jesd_version = jrx->common_link_cfg.ver == ADI_APOLLO_JESD_204C ? JESD204_VERSION_C : JESD204_VERSION_B;
// 		lnk->subclass = jrx->common_link_cfg.subclass;;
// 		lnk->scrambling = jrx->rx_link_cfg[linkIdx].scr;
// 		lnk->high_density = jrx->rx_link_cfg[linkIdx].high_dens;
// 		lnk->ctrl_words_per_frame_clk = 0;
// 		lnk->ctrl_bits_per_sample = jrx->rx_link_cfg[linkIdx].cs;
// 		lnk->samples_per_conv_frame = jrx->rx_link_cfg[linkIdx].s_minus1 + 1;

// 		lnk->sample_rate = phy->profile.dac_config[sideIdx].dac_sampling_rate_Hz;
// 		lnk->sample_rate_div = jrx->rx_link_cfg[linkIdx].link_total_ratio;
// 		priv->serdes_jrx_cal_run = false;
// 		break;
// 	case FRAMER_LINK_A0_RX:
// 	case FRAMER_LINK_A1_RX:
// 	case FRAMER_LINK_B0_RX:
// 	case FRAMER_LINK_B1_RX:

// 		sideIdx = (lnk->link_id - FRAMER_LINK_A0_RX) / 2;
// 		linkIdx = (lnk->link_id - FRAMER_LINK_A0_RX) % 2;

// 		jtx = &phy->profile.jtx[sideIdx];

// 		lnk->is_transmit = 0;
// 		lnk->num_lanes = jtx->tx_link_cfg[linkIdx].l_minus1 + 1;
// 		lnk->num_converters = jtx->tx_link_cfg[linkIdx].m_minus1 + 1;
// 		lnk->octets_per_frame = jtx->tx_link_cfg[linkIdx].f_minus1 + 1;
// 		lnk->frames_per_multiframe = jtx->tx_link_cfg[linkIdx].k_minus1 + 1;
// 		lnk->num_of_multiblocks_in_emb = jtx->tx_link_cfg[linkIdx].e_minus1 + 1;
// 		lnk->bits_per_sample = jtx->tx_link_cfg[linkIdx].np_minus1 + 1;
// 		lnk->converter_resolution = jtx->tx_link_cfg[linkIdx].n_minus1 + 1;
// 		lnk->jesd_version = jtx->common_link_cfg.ver == ADI_APOLLO_JESD_204C ? JESD204_VERSION_C : JESD204_VERSION_B;
// 		lnk->subclass = jtx->common_link_cfg.subclass;
// 		lnk->scrambling = jtx->tx_link_cfg[linkIdx].scr;
// 		lnk->high_density = jtx->tx_link_cfg[linkIdx].high_dens;
// 		lnk->ctrl_words_per_frame_clk = 0;
// 		lnk->ctrl_bits_per_sample = jtx->tx_link_cfg[linkIdx].cs;
// 		lnk->samples_per_conv_frame = jtx->tx_link_cfg[linkIdx].s_minus1 + 1;

// 		lnk->sample_rate = phy->profile.adc_config[sideIdx].adc_sampling_rate_Hz;
// 		lnk->sample_rate_div = jtx->tx_link_cfg[linkIdx].link_total_ratio;
// 		break;
// 	default:
// 		return -EINVAL;
// 	}

// 	if (lnk->jesd_version == JESD204_VERSION_C)
// 		lnk->jesd_encoder = JESD204_ENCODER_64B66B;
// 	else
// 		lnk->jesd_encoder = JESD204_ENCODER_8B10B;

// 	ret = jesd204_link_get_rate_khz(lnk, &lane_rate_kbps);
// 	if (ret)
// 		return ret;

// 	// if (phy->sysref_continuous_dis)
// 	//	lnk->sysref.mode = JESD204_SYSREF_ONESHOT;
// 	// else
// 	//	lnk->sysref.mode = JESD204_SYSREF_CONTINUOUS;

// 	return JESD204_STATE_CHANGE_DONE;
// }

// static int ad9088_jesd204_link_setup(struct jesd204_dev *jdev,
// 	enum jesd204_state_op_reason reason)
// {
// 	struct device *dev = jesd204_dev_to_device(jdev);
// 	struct ad9088_jesd204_priv *priv = jesd204_dev_priv(jdev);
// 	struct ad9088_phy *phy = priv->phy;
// 	adi_apollo_device_t *device = &phy->ad9088;
// 	u32 subclass;
// 	int ret;

// 	adi_apollo_rxen_pwrup_ctrl_t rxen_config = {
// 		.sm_clk_rate = ADI_APOLLO_PUC_CLK_RATE_FS_DIV_32,
// 		.sm_en = 0,
// 		.spi_rxen = 1,
// 		.spi_rxen_en = 1
// 	};

// 	adi_apollo_txen_pwrup_ctrl_t txen_config = {
// 		.sm_clk_rate = ADI_APOLLO_PUC_CLK_RATE_FS_DIV_32,
// 		.sm_en = 0,
// 		.spi_txen = 1,
// 		.spi_txen_en = 1
// 	};

// 	dev_dbg(dev, "%s:%d reason %s\n", __func__, __LINE__, jesd204_state_op_reason_str(reason));

// 	if (reason != JESD204_STATE_OP_REASON_INIT) {

// 		return JESD204_STATE_CHANGE_DONE;
// 	}

// 	/* Enable Apollo JTx links */
// 	ret = adi_apollo_jtx_link_enable_set(device, ADI_APOLLO_LINK_A0 | ADI_APOLLO_LINK_B0, ADI_APOLLO_ENABLE);
// 	if (ret) {
// 		dev_err(dev, "Error enabling JTx links %d\n", ret);
// 		return ret;
// 	}
// 	ret = adi_apollo_jtx_link_enable_set(device, ADI_APOLLO_LINK_A1 | ADI_APOLLO_LINK_B1, ADI_APOLLO_DISABLE);
// 	if (ret) {
// 		dev_err(dev, "Error enabling JTx links %d\n", ret);
// 		return ret;
// 	}

// 	/* Enable Apollo JRx links */
// 	ret = adi_apollo_jrx_link_enable_set(device, ADI_APOLLO_LINK_A0 | ADI_APOLLO_LINK_B0, ADI_APOLLO_ENABLE);
// 	if (ret) {
// 		dev_err(dev, "Error enabling JRx links %d\n", ret);
// 		return ret;
// 	}
// 	ret = adi_apollo_jrx_link_enable_set(device, ADI_APOLLO_LINK_A1 | ADI_APOLLO_LINK_B1, ADI_APOLLO_DISABLE);
// 	if (ret) {
// 		dev_err(dev, "Error enabling JRx links %d\n", ret);
// 		return ret;
// 	}

// 	/* Enable Rx blocks - enable/disable via spi */
// 	ret = adi_apollo_rxen_pwrup_ctrl_set(device, ADI_APOLLO_RXEN_ADC_ALL, &rxen_config);
// 	if (ret) {
// 		dev_err(dev, "Error activating Rx blocks (%d)\n", ret);
// 		return ret;
// 	}

// 	/* Enable Tx blocks - enable/disable via spi */
// 	ret = adi_apollo_txen_pwrup_ctrl_set(device, ADI_APOLLO_TXEN_DAC_ALL, &txen_config);
// 	if (ret) {
// 		dev_err(dev, "Error activating Tx blocks(%d)\n", ret);
// 		return ret;
// 	}

// 	/* Datapath reset */
// 	adi_apollo_rxmisc_dp_reset(device, ADI_APOLLO_SIDE_ALL, 1);
// 	adi_apollo_txmisc_dp_reset(device, ADI_APOLLO_SIDE_ALL, 1);
// 	adi_apollo_rxmisc_dp_reset(device, ADI_APOLLO_SIDE_ALL, 0);
// 	adi_apollo_txmisc_dp_reset(device, ADI_APOLLO_SIDE_ALL, 0);

// 	subclass = phy->profile.jtx->common_link_cfg.subclass || phy->profile.jrx->common_link_cfg.subclass;

// 	ret = adi_apollo_clk_mcs_subclass_set(&phy->ad9088, subclass);
// 	if (ret) {
// 		dev_err(dev, "Error setting subclass %d\n", ret);
// 		return ret;
// 	}
// 	ret = adi_apollo_jrx_subclass_set(&phy->ad9088, ADI_APOLLO_LINK_ALL, phy->profile.jrx->common_link_cfg.subclass);
// 	if (ret) {
// 		dev_err(dev, "Error setting subclass %d\n", ret);
// 		return ret;
// 	}

// 	ret = adi_apollo_jtx_subclass_set(&phy->ad9088, ADI_APOLLO_LINK_ALL, phy->profile.jtx->common_link_cfg.subclass);
// 	if (ret) {
// 		dev_err(dev, "Error setting subclass %d\n", ret);
// 		return ret;
// 	}

// 	ret = adi_apollo_clk_mcs_internal_sysref_per_set(&phy->ad9088,
// 		phy->profile.mcs_cfg.internal_sysref_prd_digclk_cycles_center);
// 	if (ret) {
// 		dev_err(dev, "Error setting internal sysref period %d\n", ret);
// 		return ret;
// 	}

// 	/* Enable the MCS SYSREF receiver if subclass 1 */
// 	ret = adi_apollo_clk_mcs_sysref_en_set(&phy->ad9088, (subclass == 1) ? ADI_APOLLO_ENABLE : ADI_APOLLO_DISABLE);
// 	if (ret) {
// 		dev_err(dev, "Error setting MCS SYSREF receiver %d\n", ret);
// 		return ret;
// 	}

// 	ret = adi_apollo_adc_bgcal_freeze(device, ADI_APOLLO_ADC_A0 | ADI_APOLLO_ADC_A1 | ADI_APOLLO_ADC_B0 | ADI_APOLLO_ADC_B1);
// 	if (ret) {
// 		dev_err(dev, "Error in adi_apollo_adc_bgcal_freeze %d\n", ret);
// 		return ret;
// 	}

// 	ret = adi_apollo_clk_mcs_dyn_sync_sequence_run(device);
// 	if (ret) {
// 		dev_err(dev, "Error in adi_apollo_clk_mcs_dyn_sync_sequence_run %d\n", ret);
// 		return ret;
// 	}
// 	ret = adi_apollo_clk_mcs_oneshot_sync(device);
// 	if (ret) {
// 		dev_err(dev, "Error in adi_apollo_clk_mcs_oneshot_sync %d\n", ret);
// 		return ret;
// 	}

// 	return JESD204_STATE_CHANGE_DONE;
// }

// static int ad9088_iio_write_channel_ext_info(struct ad9088_phy *phy, struct iio_channel	*chan,
// 					     const char *ext_name,long long val)
// {
// 	ssize_t size;
// 	char str[16];

// 	snprintf(str, sizeof(str), "%lld\n", val);

// 	size = iio_write_channel_ext_info(chan, ext_name, str, sizeof(str));
// 	if (size != sizeof(str)) {
// 		dev_err(&phy->spi->dev, "%s: Failed to write channel ext info\n", __func__);
// 		return -EINVAL;
// 	}

// 	return 0;
// }

// static int ad9088_jesd204_setup_stage1(struct jesd204_dev *jdev,
// 	enum jesd204_state_op_reason reason)
// {
// 	struct device *dev = jesd204_dev_to_device(jdev);
// 	struct ad9088_jesd204_priv *priv = jesd204_dev_priv(jdev);
// 	struct ad9088_phy *phy = priv->phy;
// 	adi_apollo_device_t *device = &phy->ad9088;
// 	//adi_apollo_hal_protocol_e protocol;
// 	int ret;
// 	u16 jrx_phase_adjust;

// 	if (reason != JESD204_STATE_OP_REASON_INIT)
// 		return JESD204_STATE_CHANGE_DONE;

// 	phy->rx_en_mask = ADI_APOLLO_RXEN_ADC_ALL;
// 	phy->tx_en_mask = ADI_APOLLO_TXEN_DAC_ALL;

// 	// adi_apollo_hal_active_protocol_get(device, &protocol);
// 	// adi_apollo_hal_active_protocol_set(device, ADI_APOLLO_HAL_PROTOCOL_SPI0);

// 	ret = adi_apollo_adc_nyquist_zone_set(device, device->dev_info.is_8t8r ? ADI_APOLLO_ADC_ALL : ADI_APOLLO_ADC_ALL_4T4R,
// 									phy->rx_nyquist_zone[0][0] + 1);
// 	if (ret) {
// 		dev_err(dev, "Error setting ADC Nyquist zone %d\n", ret);
// 		return ret;
// 	}

// 	if (1) {
// 		dev_info(dev, "Run clock conditioning cal (can take up to %d secs)...\n", ADI_APOLLO_SYSCLK_COND_CENTER_MAX_TO);
// 		if (ret = adi_apollo_sysclk_cond_cal(device), ret != API_CMS_ERROR_OK) {
// 			dev_err(dev, "Error in adi_apollo_sysclk_cond_cal(). err=%d\n", ret);
// 			//return ret;
// 		}
// 	}
// 	//adi_apollo_hal_active_protocol_set(device, protocol);

// 	/* Inspect the Apollo JRx and JTx link config */
// 	ret = ad9088_inspect_jrx_link_all(phy);
// 	if (ret) {
// 		dev_err(dev, "Error in ad9088_inspect_jrx_link_all %d\n", ret);
// 		return ret;
// 	}
// 	ret = ad9088_inspect_jtx_link_all(phy);
// 	if (ret) {
// 		dev_err(dev, "Error in ad9088_inspect_jtx_link_all %d\n", ret);
// 		return ret;
// 	}

// 	ret = adi_apollo_jrx_phase_adjust_calc(device, ADI_APOLLO_LINK_A0 | ADI_APOLLO_LINK_B0,
// 					       ADI_APOLLO_JRX_PHASE_ADJ_MARGIN_DEFAULT, &jrx_phase_adjust);
// 	if (ret) {
// 		dev_err(dev, "Error in adi_apollo_jrx_phase_adjust_calc %d\n", ret);
// 		return ret;
// 	}

// 	dev_dbg(dev, "JRX Phase Adjust: %d\n", jrx_phase_adjust);

// 	/* Set the jrx phase adjust */
// 	ret = adi_apollo_jrx_phase_adjust_set(device, ADI_APOLLO_LINK_A0 | ADI_APOLLO_LINK_B0, jrx_phase_adjust);
// 	if (ret) {
// 		dev_err(dev, "Error in adi_apollo_jrx_phase_adjust_set %d\n", ret);
// 		return ret;
// 	}

// 	/* Set the jtx phase adjust */
// 	ret = adi_apollo_jtx_phase_adjust_set(device, ADI_APOLLO_LINK_A0 | ADI_APOLLO_LINK_B0, 0);
// 	if (ret) {
// 		dev_err(dev, "Error in adi_apollo_jtx_phase_adjust_set %d\n", ret);
// 		return ret;
// 	}

// 	/* ADC calibration */
// 	if (1) {
// 		u32 adc_cal_chans = device->dev_info.is_8t8r ? ADI_APOLLO_ADC_ALL : ADI_APOLLO_ADC_ALL_4T4R;
// 		dev_info(dev, "Run ADC cal (can take up to 100 secs)...\n");
// 		ret = adi_apollo_adc_init_cal_start(device, adc_cal_chans, ADI_APOLLO_INIT_CAL_ENABLED);
// 		if (ret) {
// 			dev_err(dev, "Error in adi_apollo_adc_init_cal_start %d\n", ret);
// 			return ret;
// 		}
// 	}

// 	return JESD204_STATE_CHANGE_DONE;
// }

// static int ad9088_jesd204_setup_stage2(struct jesd204_dev *jdev,
// 	enum jesd204_state_op_reason reason)
// {
// 	struct device *dev = jesd204_dev_to_device(jdev);
// 	struct ad9088_jesd204_priv *priv = jesd204_dev_priv(jdev);
// 	struct ad9088_phy *phy = priv->phy;
// 	adi_apollo_device_t *device = &phy->ad9088;
// 	int ret;

// 	if (reason == JESD204_STATE_OP_REASON_INIT) {
// 		ret = adi_apollo_adc_init_cal_complete(device, device->dev_info.is_8t8r ? ADI_APOLLO_ADC_ALL : ADI_APOLLO_ADC_ALL_4T4R);
// 		if (ret) {
// 			dev_err(dev, "Error in adi_apollo_adc_init_cal_complete %d\n", ret);
// 			return ret;
// 		}

// 		ret = adi_apollo_clk_mcs_oneshot_sync(device);
// 		if (ret) {
// 			dev_err(dev, "Error in adi_apollo_clk_mcs_oneshot_sync %d\n", ret);
// 			return ret;
// 		}
// 	}

// 	return JESD204_STATE_CHANGE_DONE;
// }

// static int ad9088_jesd204_clks_enable(struct jesd204_dev *jdev,
// 				      enum jesd204_state_op_reason reason,
// 				      struct jesd204_link *lnk)
// {
// 	struct device *dev = jesd204_dev_to_device(jdev);
// 	struct ad9088_jesd204_priv *priv = jesd204_dev_priv(jdev);
// 	struct ad9088_phy *phy = priv->phy;
// 	int ret;

// 	u16 serdes;

// 	switch (lnk->link_id) {
// 	case DEFRAMER_LINK_A0_TX:
// 	case DEFRAMER_LINK_A1_TX:
// 		serdes = ADI_APOLLO_TXRX_SERDES_12PACK_A;
// 		break;
// 	case DEFRAMER_LINK_B0_TX:
// 	case DEFRAMER_LINK_B1_TX:
// 		serdes = ADI_APOLLO_TXRX_SERDES_12PACK_B;
// 		break;
// 	default:
// 		serdes = ADI_APOLLO_TXRX_SERDES_12PACK_NONE | ADI_APOLLO_TXRX_SERDES_12PACK_NONE;
// 		break;
// 	}

// 	dev_dbg(dev, "%s:%d link_num %u reason %s\n", __func__, __LINE__,
// 		lnk->link_id, jesd204_state_op_reason_str(reason));


// 	if (lnk->is_transmit && (reason == JESD204_STATE_OP_REASON_UNINIT) &&
// 	    phy->profile.jrx[0].common_link_cfg.lane_rate_kHz > 16000000) {
// 		ret = adi_apollo_serdes_jrx_bgcal_freeze(&phy->ad9088, serdes);
// 		if (ret) {
// 			dev_err(dev, "Error from adi_apollo_serdes_jrx_bgcal_freeze() %d\n", ret);
// 			return ret;
// 		}
// 		dev_info(dev, "%s: SERDES JRx bg cal freeze\n", ad9088_fsm_links_to_str[lnk->link_id]);
// 	}


// 	if (lnk->is_transmit && (reason == JESD204_STATE_OP_REASON_INIT) &&
// 		phy->profile.jrx[0].common_link_cfg.lane_rate_kHz > 8000000) {

// 		dev_info(dev, "%s: SERDES JRx cal Rate %u kBps ...\n", ad9088_fsm_links_to_str[lnk->link_id], phy->profile.jrx[0].common_link_cfg.lane_rate_kHz);
// 		ret = adi_apollo_serdes_jrx_init_cal(&phy->ad9088, serdes, ADI_APOLLO_INIT_CAL_ENABLED);
// 		if (ret) {
// 			dev_err(dev, "Error from adi_apollo_serdes_jrx_cal() %d\n", ret);
// 			return ret;
// 		}

// 		if (phy->profile.jrx[0].common_link_cfg.lane_rate_kHz > 16000000) {
// 			ret = adi_apollo_serdes_jrx_bgcal_unfreeze(&phy->ad9088, serdes);
// 			if (ret) {
// 				dev_err(dev, "Error from adi_apollo_serdes_jrx_bgcal_unfreeze() %d\n", ret);
// 				return ret;
// 			}
// 			dev_info(dev, "%s: SERDES JRx bg cal unfreeze\n", ad9088_fsm_links_to_str[lnk->link_id]);
// 		}
// 	}

// 	if (!lnk->is_transmit) {
// 		ret = adi_apollo_jtx_link_enable_set(&phy->ad9088, ad9088_to_link(lnk->link_id),
// 						     reason == JESD204_STATE_OP_REASON_INIT);
// 		if (ret != 0)
// 			return ret;


// 	}

// 	return JESD204_STATE_CHANGE_DONE;
// }

// static int ad9088_jesd204_link_enable(struct jesd204_dev *jdev,
// 				      enum jesd204_state_op_reason reason,
// 				      struct jesd204_link *lnk)
// {
// 	struct device *dev = jesd204_dev_to_device(jdev);
// 	struct ad9088_jesd204_priv *priv = jesd204_dev_priv(jdev);
// 	struct ad9088_phy *phy = priv->phy;
// 	int ret;

// 	dev_dbg(dev, "%s:%d link_num %u reason %s\n", __func__, __LINE__, lnk->link_id, jesd204_state_op_reason_str(reason));

// 	if (lnk->is_transmit) {
// 		/* txfe TX (JRX) link */
// 		ret = adi_apollo_jrx_link_enable_set(&phy->ad9088,
// 						     ad9088_to_link(lnk->link_id),
// 						     reason == JESD204_STATE_OP_REASON_INIT);
// 		if (ret != 0)
// 			return ret;
// 	}

// 	ret = adi_apollo_clk_mcs_oneshot_sync(&phy->ad9088);
// 	if (ret) {
// 		dev_err(dev, "MCS oneshot sync (1) failed (%d)\n", ret);
// 	}
// 	// ret = adi_apollo_clk_mcs_oneshot_sync(&phy->ad9088);
// 	// if (ret) {
// 	// 	dev_err(dev, "MCS oneshot sync (2) failed (%d)\n", ret);
// 	// }

// 	return JESD204_STATE_CHANGE_DONE;
// }

// static int ad9088_jesd204_link_running(struct jesd204_dev *jdev,
// 				       enum jesd204_state_op_reason reason,
// 				       struct jesd204_link *lnk)
// {
// 	struct device *dev = jesd204_dev_to_device(jdev);
// 	struct ad9088_jesd204_priv *priv = jesd204_dev_priv(jdev);
// 	struct ad9088_phy *phy = priv->phy;
// 	int ret;

// 	dev_dbg(dev, "%s:%d link_num %u reason %s\n", __func__, __LINE__,
// 		lnk->link_id, jesd204_state_op_reason_str(reason));

// 	if (reason != JESD204_STATE_OP_REASON_INIT) {
// 		// if (lnk->is_transmit && phy->jrx_link_watchdog_en)
// 		//	cancel_delayed_work(&phy->dwork);

// 		phy->is_initialized = false;

// 		return JESD204_STATE_CHANGE_DONE;
// 	}

// 	if (lnk->is_transmit) {
// 		ad9088_print_link_phase(phy, lnk);
// 		ret = ad9088_jesd_rx_link_status_print(phy, lnk, 3);
// 		if (ret < 0)
// 			return JESD204_STATE_CHANGE_ERROR;
// 	} else {
// 		ret = ad9088_jesd_tx_link_status_print(phy, lnk, 3);
// 		if (ret < 0)
// 			return JESD204_STATE_CHANGE_ERROR;
// 	}

// 	return JESD204_STATE_CHANGE_DONE;
// }

// static int ad9088_jesd204_post_setup_stage1(struct jesd204_dev *jdev,
// 	enum jesd204_state_op_reason reason)
// {
// 	struct device *dev = jesd204_dev_to_device(jdev);
// 	struct ad9088_jesd204_priv *priv = jesd204_dev_priv(jdev);
// 	struct ad9088_phy *phy = priv->phy;
// 	adi_apollo_mcs_cal_init_status_t init_cal_status = {{0}};
// 	adi_apollo_device_t *device = &phy->ad9088;
// 	s64 apollo_delta_t0 = 0;
// 	s64 apollo_delta_t1 = 0;
// 	s64 adf4030_delta_t0 = 0;
// 	s64 adf4030_delta_t1 = 0;
// 	s64 calc_delay = 0;
// 	u64 bsync_out_period_fs;
// 	u64 path_delay, round_trip_delay = 0;
// 	s64 adf4030_phase;
// 	int val, val2;
// 	int ret;

// 	dev_dbg(dev, "%s:%d reason %s\n", __func__, __LINE__, jesd204_state_op_reason_str(reason));

// 	if (reason != JESD204_STATE_OP_REASON_INIT) {

// 		return JESD204_STATE_CHANGE_DONE;
// 	}

// 	if (IS_ERR_OR_NULL(phy->iio_adf4030) || IS_ERR_OR_NULL(phy->iio_adf4382)) {
// 		dev_info(dev, "Skipping MCS calibration\n");
// 		return JESD204_STATE_CHANGE_DONE;
// 	}

// 	ret = iio_read_channel_attribute(phy->iio_adf4030, &val, &val2, IIO_CHAN_INFO_FREQUENCY);
// 	if (ret < 0) {
// 		dev_err(dev, "Failed to read adf4030 frequency\n");
// 		return ret;
// 	}

// 	bsync_out_period_fs = div_u64(1000000000000000ULL, val);

// 	dev_dbg(dev, "bsync_out_period_fs %lld\n", bsync_out_period_fs);

// 	ret = ad9088_iio_write_channel_ext_info(phy, phy->iio_adf4030, "output_enable", 1);
// 	if (ret < 0) {
// 		dev_err(dev, "Failed to enable adf4030 output\n");
// 		return ret;
// 	}
// 	ret = iio_write_channel_attribute(phy->iio_adf4030, 0, 0, IIO_CHAN_INFO_PHASE);
// 	if (ret < 0) {
// 		dev_err(dev, "Failed to set adf4030 phase\n");
// 		return ret;
// 	}

// 	ret = ad9088_mcs_init_cal_setup(phy);
// 	if (ret) {
// 		dev_err(dev, "Failed to setup MCS init cal\n");
// 		return ret;
// 	}

// 	ret = ad9088_delta_t_measurement_set(phy, 0);
// 	if (ret) {
// 		dev_err(dev, "Failed to set delta_t measurement\n");
// 		return ret;
// 	}
// 	ret = ad9088_delta_t_measurement_get(phy, 0, &apollo_delta_t0);
// 	if (ret) {
// 		dev_err(dev, "Failed to get delta_t measurement\n");
// 		return ret;
// 	}
// 	dev_dbg(dev, "apollo_delta_t0 %lld fs\n", apollo_delta_t0);
// 	ret = iio_read_channel_attribute(phy->iio_adf4030, &val, &val2, IIO_CHAN_INFO_PHASE);
// 	if (ret < 0) {
// 		dev_err(dev, "Failed to read adf4030 phase\n");
// 		return ret;
// 	}
// 	adf4030_delta_t0 = (s64)((((u64)val2) << 32) | (u32)val);
// 	dev_dbg(dev, "adf4030_delta_t0 %lld fs\n", adf4030_delta_t0);

// 	ret = ad9088_iio_write_channel_ext_info(phy, phy->iio_adf4030, "output_enable", 0);
// 	if (ret < 0) {
// 		dev_err(dev, "Failed to disable adf4030 output\n");
// 		return ret;
// 	}
// 	ret = ad9088_delta_t_measurement_set(phy, 1);
// 	if (ret) {
// 		dev_err(dev, "Failed to set delta_t measurement\n");
// 		return ret;
// 	}
// 	ret = ad9088_delta_t_measurement_get(phy, 1, &apollo_delta_t1);
// 	if (ret) {
// 		dev_err(dev, "Failed to get delta_t measurement\n");
// 		return ret;
// 	}
// 	dev_dbg(dev, "apollo_delta_t1 %lld fs\n", apollo_delta_t1);

// 	ret = iio_read_channel_attribute(phy->iio_adf4030, &val, &val2, IIO_CHAN_INFO_PHASE);
// 	if (ret < 0) {
// 		dev_err(dev, "Failed to read adf4030 phase\n");
// 		return ret;
// 	}
// 	adf4030_delta_t1 = (s64)((((u64)val2) << 32) | (u32)val);
// 	dev_dbg(dev, "adf4030_delta_t1 %lld fs\n", adf4030_delta_t1);

// 	ret = ad9088_delta_t_measurement_set(phy, 2);
// 	if (ret) {
// 		dev_err(dev, "Failed to set delta_t measurement\n");
// 		return ret;
// 	}

// 	calc_delay = (adf4030_delta_t0 - adf4030_delta_t1) - (apollo_delta_t1 - apollo_delta_t0);
// 	dev_dbg(dev, "calc_delay %lld fs\n", calc_delay);
// 	div64_u64_rem(calc_delay + bsync_out_period_fs, bsync_out_period_fs, &round_trip_delay);
// 	dev_dbg(dev, "round_trip_delay %lld fs\n", round_trip_delay);
// 	path_delay = round_trip_delay >> 1;

// 	dev_info(dev, "Total BSYNC path delay %lld fs\n", path_delay);

// 	ret = ad9088_iio_write_channel_ext_info(phy, phy->iio_adf4030, "output_enable", 1);
// 	if (ret < 0) {
// 		dev_err(dev, "Failed to enable adf4030 output\n");
// 		return ret;
// 	}

// 	val = lower_32_bits(-1 * path_delay);
// 	val2 = upper_32_bits(-1 * path_delay);

// 	ret = iio_write_channel_attribute(phy->iio_adf4030, val, val2, IIO_CHAN_INFO_PHASE);
// 	if (ret < 0) {
// 		dev_err(dev, "Failed to set adf4030 phase\n");
// 		return ret;
// 	}

// 	if (__is_defined(DEBUG)) {
// 		ret = iio_read_channel_attribute(phy->iio_adf4030, &val, &val2, IIO_CHAN_INFO_PHASE);
// 		if (ret < 0) {
// 			dev_err(dev, "Failed to read adf4030 phase\n");
// 			return ret;
// 		}
// 		adf4030_phase = (s64)((((u64)val2) << 32) | (u32)val);
// 		dev_info(dev, "adf4030_phase %lld fs\n", adf4030_phase);
// 	}

// 	ret = adi_apollo_mcs_cal_init_run(device);
// 	if (ret) {
// 		dev_err(dev, "Failed to run MCS init cal\n");
// 		return ret;
// 	}

// 	ret = adi_apollo_mcs_cal_init_status_get(device, &init_cal_status);
// 	if (ret) {
// 		dev_err(dev, "Failed to get MCS init cal status\n");
// 		return ret;
// 	}

// 	ret = ad9088_mcs_init_cal_validate(phy, &init_cal_status);

// 	dev_info(dev, "MCS Initcal Status: %s\n", ret ? "Failed" : "Passed");

// 	ret = ad9088_mcs_tracking_cal_setup(device, 1023, 1);
// 	if (ret) {
// 		dev_err(dev, "Failed to setup MCS tracking cal\n");
// 		return ret;
// 	}

// 	ret = ad9088_iio_write_channel_ext_info(phy, phy->iio_adf4382, "en_auto_align", 1);
// 	if (ret < 0) {
// 		dev_err(dev, "Failed to enable adf4382 auto align\n");
// 		return ret;
// 	}

// 	ret = iio_write_channel_attribute(phy->iio_adf4382, 250, 0, IIO_CHAN_INFO_PHASE);
// 	if (ret < 0) {
// 		dev_err(dev, "Failed to set adf4382 phase\n");
// 		return ret;
// 	}

// 	ret = adi_apollo_mcs_cal_fg_tracking_run(device);
// 	if (ret) {
// 		dev_err(dev, "Failed to run MCS tracking cal\n");
// 		return ret;
// 	}
// 	ret = adi_apollo_mcs_cal_bg_tracking_run(device);
// 	if (ret) {
// 		dev_err(dev, "Failed to run MCS tracking cal\n");
// 		return ret;
// 	}

// 	phy->mcs_cal_bg_tracking_run = true;

// 	if (__is_defined(DEBUG)) {
// 		ret = adi_apollo_mcs_cal_init_status_get(device, &init_cal_status);
// 		if (ret) {
// 			dev_err(dev, "Failed to get MCS init cal status\n");
// 			return ret;
// 		}
// 		ret = ad9088_mcs_init_cal_status_print(phy, phy->dbuf, &init_cal_status);
// 		if (ret <= 0) {
// 			dev_err(dev, "Failed to print MCS init cal status\n");
// 			return ret;
// 		}

// 		dev_info(dev, "MCS Initcal Status: %s\n", phy->dbuf);
// 	}

// 	return JESD204_STATE_CHANGE_DONE;
// }

// static int ad9088_jesd204_post_setup_stage2(struct jesd204_dev *jdev,
// 				       enum jesd204_state_op_reason reason)
// {
// 	struct device *dev = jesd204_dev_to_device(jdev);
// 	struct ad9088_jesd204_priv *priv = jesd204_dev_priv(jdev);
// 	struct ad9088_phy *phy = priv->phy;
// 	adi_apollo_device_t *device = &phy->ad9088;
// 	int ret;

// 	dev_dbg(dev, "%s:%d reason %s\n", __func__, __LINE__, jesd204_state_op_reason_str(reason));

// 	if (reason != JESD204_STATE_OP_REASON_INIT) {
// 		adi_apollo_clk_mcs_trig_sync_enable(device, 0);
// 		adi_apollo_clk_mcs_trig_reset_disable(device);

// 		return JESD204_STATE_CHANGE_DONE;
// 	}

// 	// /* Dynamic reconfig configuration */
// 	// static adi_apollo_reconfig_ctrl_pgm_t tx_reconfig = {
// 	// 	.trig_reconfig_mode = 0,    // 1 = internal triggers, else external
// 	// 	.cnco_reset = 1,
// 	// 	.timestamp_reset_en = 1,
// 	// 	.resync_en = 0,             // 0 for ext trig, 1 for internal
// 	// 	.fnco_reset_en = 1,
// 	// 	.tzero_coherence_en = 1,    // T0 Coherence with FSRC enabled is not supported
// 	// 	.prefsrc_lcm = 0x5f,        // 0x05F if (fduc*cduc) < 128 else 0x17F
// 	// 	.postfsrc_lcm = 0x17F       // 0x17F if (fduc*cduc) < 128 else 0xBFF
// 	// };

// 	// /* Dynamic reconfig configuration */
// 	// static adi_apollo_reconfig_ctrl_pgm_t rx_reconfig = {
// 	// 	.trig_reconfig_mode = 0,    // 0 = ext trig, 1 = internal trig
// 	// 	.cnco_reset = 1,
// 	// 	.timestamp_reset_en = 1,
// 	// 	.resync_en = 0,             // 0 = ext trig, 1 = internal trig
// 	// 	.fnco_reset_en = 1,
// 	// 	.tzero_coherence_en = 1,    // T0 Coherence with FSRC enabled is not supported
// 	// 	.prefsrc_lcm = 0x02F,       // 0x02F if (fddc*cddc) < 128 else 0x0BF
// 	// 	.postfsrc_lcm = 0x17F       // 0x17F if (fddc*cddc) < 128 else 0x5FF
// 	// };

// 	if (phy->trig_sync_en) {

// 		// /* Configure the Tx recofig controller */
// 		// adi_apollo_reconfig_ctrl_pgm(device, ADI_APOLLO_TX, ADI_APOLLO_SIDE_ALL, &tx_reconfig);

// 		// /* Configure the Rx recofig controller */
// 		// adi_apollo_reconfig_ctrl_pgm(device, ADI_APOLLO_RX, ADI_APOLLO_SIDE_ALL, &rx_reconfig);

// 		/* Use Trigger pin A0 to sync Rx and Tx */
// 		ret = adi_apollo_clk_mcs_sync_trig_map(device, ADI_APOLLO_RX_TX_ALL, ADI_APOLLO_TRIG_PIN_A0);
// 		if (ret) {
// 			dev_err(dev, "Error in adi_apollo_clk_mcs_sync_trig_map %d\n", ret);
// 			return ret;
// 		}

// 		/*
// 		* Resync the Rx and Tx dig only during trig sync
// 		*/
// 		ret = adi_apollo_clk_mcs_trig_sync_enable(device, 0);
// 		if (ret) {
// 			dev_err(dev, "Error in adi_apollo_clk_mcs_trig_sync_enable %d\n", ret);
// 			return ret;
// 		}
// 		ret = adi_apollo_clk_mcs_trig_reset_disable(device);
// 		if (ret) {
// 			dev_err(dev, "Error in adi_apollo_clk_mcs_trig_reset_disable %d\n", ret);
// 			return ret;
// 		}

// 		/*
// 		* Set trig_syn to 1. Apollo will wait for a trigger from the FPGA. When
// 		* received, the FSRC will be reset.
// 		*
// 		* trig_sync is not self-clearing
// 		*/
// 		ret = adi_apollo_clk_mcs_trig_sync_enable(device, 0);
// 		if (ret) {
// 			dev_err(dev, "Error in adi_apollo_clk_mcs_trig_sync_enable %d\n", ret);
// 			return ret;
// 		}
// 		ret = adi_apollo_clk_mcs_trig_reset_dsp_enable(device);
// 		if (ret) {
// 			dev_err(dev, "Error in adi_apollo_clk_mcs_trig_reset_dsp_enable %d\n", ret);
// 			return ret;
// 		}
// 		ret = adi_apollo_clk_mcs_trig_sync_enable(device, 1);
// 		if (ret) {
// 			dev_err(dev, "Error in adi_apollo_clk_mcs_trig_sync_enable %d\n", ret);
// 			return ret;
// 		}
// 	}

// 	return JESD204_STATE_CHANGE_DONE;
// }

// static int ad9088_jesd204_post_setup_stage3(struct jesd204_dev *jdev,
// 				       enum jesd204_state_op_reason reason)
// {
// 	struct device *dev = jesd204_dev_to_device(jdev);
// 	struct ad9088_jesd204_priv *priv = jesd204_dev_priv(jdev);
// 	struct ad9088_phy *phy = priv->phy;

// 	if (reason != JESD204_STATE_OP_REASON_INIT)
// 		return JESD204_STATE_CHANGE_DONE;

// 	dev_dbg(dev, "%s:%d reason %s\n", __func__, __LINE__, jesd204_state_op_reason_str(reason));

// 	if (phy->triq_req_gpio && phy->trig_sync_en) {
// 		gpiod_set_value(phy->triq_req_gpio, 1);
// 		udelay(1);
// 		gpiod_set_value(phy->triq_req_gpio, 0);
// 	}

// 	return JESD204_STATE_CHANGE_DONE;
// }

// static int ad9088_jesd204_post_setup_stage4(struct jesd204_dev *jdev,
// 				       enum jesd204_state_op_reason reason)
// {
// 	struct device *dev = jesd204_dev_to_device(jdev);
// 	struct ad9088_jesd204_priv *priv = jesd204_dev_priv(jdev);
// 	struct ad9088_phy *phy = priv->phy;
// 	adi_apollo_device_t *device = &phy->ad9088;
// 	int ret;
// 	u32 periode_fs;

// 	if (reason != JESD204_STATE_OP_REASON_INIT) {
// 		phy->is_initialized = false;
// 		return JESD204_STATE_CHANGE_DONE;
// 	}

// 	dev_dbg(dev, "%s:%d reason %s\n", __func__, __LINE__, jesd204_state_op_reason_str(reason));

// 	if (phy->trig_sync_en) {
// 		u16 phase, phase1;
// 		/*
// 		* Wait for the trigger sync to finish.
// 		*/
// 		ret = adi_apollo_hal_bf_wait_to_set(device, BF_TRIGGER_SYNC_DONE_A0_INFO(MCS_SYNC_MCSTOP0), 1000000, 100);   // Will set when Apollo recvs a trigger pulse
// 		if (ret) {
// 			dev_err(dev, "Error in adi_apollo_hal_bf_wait_to_set %d\n", ret);
// 			return ret;
// 		}
// 		ret = adi_apollo_clk_mcs_trig_phase_get(&phy->ad9088, ADI_APOLLO_TRIG_PIN_A0, &phase, &phase1);
// 		if (ret) {
// 			dev_err(dev, "Error in adi_apollo_clk_mcs_trig_phase_get %d\n", ret);
// 			return ret;
// 		}

// 		periode_fs = div64_u64(1000000000000000ULL * (phy->profile.clk_cfg.clocking_mode ==
// 				ADI_APOLLO_CLOCKING_MODE_SDR_DIV_8 ? 8 : 4),
// 				phy->profile.clk_cfg.dev_clk_freq_kHz * 1000ULL);

// 		dev_info(dev, "Trigger Phase %d (ideal %u) periode %u fs\n", phase,
// 			 phy->profile.mcs_cfg.internal_sysref_prd_digclk_cycles_center / 2, periode_fs);

// 		ret = adi_apollo_clk_mcs_trig_sync_enable(device, 0);
// 		if (ret) {
// 			dev_err(dev, "Error in adi_apollo_clk_mcs_trig_sync_enable %d\n", ret);
// 			return ret;
// 		}
// 		ret = adi_apollo_clk_mcs_trig_reset_disable(device);
// 		if (ret) {
// 			dev_err(dev, "Error in adi_apollo_clk_mcs_trig_reset_disable %d\n", ret);
// 			return ret;
// 		}

// 		if (__is_defined(DEBUG)) {
// 			/*
// 			* Read the trigger sync count. It will increment by two each time the FPGA sysref sequencer is executed
// 			*/

// 			uint8_t sync_input_count[4];
// 			adi_apollo_hal_bf_get(device, BF_SYNC_INPUT_COUNT_INFO(TXRX_PREFSRC_RECONF_TX_SLICE_0_TX_DIGITAL0), &sync_input_count[0], 1);
// 			adi_apollo_hal_bf_get(device, BF_SYNC_INPUT_COUNT_INFO(TXRX_PREFSRC_RECONF_TX_SLICE_1_TX_DIGITAL0), &sync_input_count[1], 1);
// 			adi_apollo_hal_bf_get(device, BF_SYNC_INPUT_COUNT_INFO(TXRX_PREFSRC_RECONF_TX_SLICE_0_TX_DIGITAL1), &sync_input_count[2], 1);
// 			adi_apollo_hal_bf_get(device, BF_SYNC_INPUT_COUNT_INFO(TXRX_PREFSRC_RECONF_TX_SLICE_1_TX_DIGITAL1), &sync_input_count[3], 1);
// 			dev_info(&phy->spi->dev, "sync_input_count=%d %d %d %d PHASE %d\n", sync_input_count[0], sync_input_count[1], sync_input_count[2], sync_input_count[3], phase);
// 		}
// 	}

// 	// if (lnk->is_transmit && phy->jrx_link_watchdog_en)
// 	//	schedule_delayed_work(&phy->dwork, msecs_to_jiffies(1000));

// 	ad9088_print_sysref_phase(phy);

// 	ret = adi_apollo_adc_bgcal_unfreeze(device,
// 		ADI_APOLLO_ADC_A0 | ADI_APOLLO_ADC_A1 |
// 		ADI_APOLLO_ADC_B0 | ADI_APOLLO_ADC_B1);
// 	if (ret) {
// 		dev_err(dev, "Error in adi_apollo_adc_bgcal_unfreeze %d\n", ret);
// 		return ret;
// 	}

// 	phy->is_initialized = true;

// 	if (phy->hsci_disable_after_initial_configuration)
// 		adi_apollo_hal_active_protocol_set(&phy->ad9088, ADI_APOLLO_HAL_PROTOCOL_SPI0);

// 	return JESD204_STATE_CHANGE_DONE;
// }

// int ad9088_jesd204_uninit(struct jesd204_dev *jdev,
// 			  enum jesd204_state_op_reason reason)
// {
// 	struct device *dev = jesd204_dev_to_device(jdev);

// 	if (reason != JESD204_STATE_OP_REASON_UNINIT)
// 		return JESD204_STATE_CHANGE_DONE;

// 	dev_dbg(dev, "%s:%d reason %s\n", __func__, __LINE__, jesd204_state_op_reason_str(reason));

// 	return JESD204_STATE_CHANGE_DONE;
// }

// static const struct jesd204_dev_data jesd204_ad9088_init = {
// 	.state_ops = {
// 		[JESD204_OP_DEVICE_INIT] = {
// 			.per_device = ad9088_jesd204_uninit,
// 		},
// 		[JESD204_OP_LINK_INIT] = {
// 			.per_link = ad9088_jesd204_link_init,
// 		},
// 		[JESD204_OP_LINK_SETUP] = {
// 			.per_device = ad9088_jesd204_link_setup,
// 			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
// 		},
// 		[JESD204_OP_OPT_SETUP_STAGE1] = {
// 			.per_device = ad9088_jesd204_setup_stage1,
// 			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
// 		},
// 		[JESD204_OP_OPT_SETUP_STAGE2] = {
// 			.per_device = ad9088_jesd204_setup_stage2,
// 			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
// 		},
// 		[JESD204_OP_CLOCKS_ENABLE] = {
// 			.per_link = ad9088_jesd204_clks_enable,
// 		},
// 		[JESD204_OP_LINK_ENABLE] = {
// 			.per_link = ad9088_jesd204_link_enable,
// 			.post_state_sysref = true,
// 		},
// 		[JESD204_OP_LINK_RUNNING] = {
// 			.per_link = ad9088_jesd204_link_running,
// 		},
// 		[JESD204_OP_OPT_POST_SETUP_STAGE1] = {
// 			.per_device = ad9088_jesd204_post_setup_stage1,
// 			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
// 		},
// 		[JESD204_OP_OPT_POST_SETUP_STAGE2] = {
// 			.per_device = ad9088_jesd204_post_setup_stage2,
// 			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
// 		},
// 		[JESD204_OP_OPT_POST_SETUP_STAGE3] = {
// 			.per_device = ad9088_jesd204_post_setup_stage3,
// 			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
// 		},
// 		[JESD204_OP_OPT_POST_RUNNING_STAGE] = {
// 			.per_device = ad9088_jesd204_post_setup_stage4,
// 			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
// 		},
// 	},

// 	.max_num_links = 4,
// 	.num_retries = 3,
// 	.sizeof_priv = sizeof(struct ad9088_jesd204_priv),
// };

// static int ad9088_reg_test(adi_apollo_device_t *device)
// {
// 	int32_t err;
// 	uint32_t i, data32;
// 	uint8_t data8, stat;
// 	adi_apollo_hal_protocol_e protocol;

// 	const uint32_t direct_addr[] = { 0x4700000a, 0x4700000a, 0x47000200, 0x47000200 };    // HSCI can't write 0x4700000A, SPI only
// 	const uint8_t  direct_data[] = { 0x55,       0xaa,       0xcc,       0x33 };

// 	uint32_t indirect_addr[] = { INDIRECT_REG_TEST_ADDR + 0, INDIRECT_REG_TEST_ADDR + 1,
// 					INDIRECT_REG_TEST_ADDR + 2, INDIRECT_REG_TEST_ADDR + 3 }; /* indirect register address */
// 	const uint8_t  indirect_data[] = { 0x12,       0x34,       0x56,       0x78 };

// 	uint32_t arm_addr[] = { ARM_REG_TEST_BASE_ADDR + 0, ARM_REG_TEST_BASE_ADDR + 4,
// 				ARM_REG_TEST_BASE_ADDR + 8, ARM_REG_TEST_BASE_ADDR + 12 }; /* ARM core1 register addresses */
// 	const uint32_t arm_data[] = { 0x55aa55aa, 0xdeadbeef, 0xbeefdead, 0xaa55aa55 };

// 	adi_apollo_hal_active_protocol_get(device, &protocol);

// 	/* Direct register SPI scratch loop rd/wr test */
// 	stat = 0;
// 	for (i=0; i<sizeof(direct_addr) / sizeof(direct_addr[0]); i++) {

// 		if ((protocol == ADI_APOLLO_HAL_PROTOCOL_HSCI) && (direct_addr[i] <= 0x4700000Fu)) {
// 			continue;
// 		}

// 		if (err = adi_apollo_hal_reg_set(device, direct_addr[i], direct_data[i]), err != API_CMS_ERROR_OK) {
// 			return err;
// 		}
// 		if (err = adi_apollo_hal_reg_get(device, direct_addr[i], &data8), err != API_CMS_ERROR_OK) {
// 			return err;
// 		}

// 		if (data8 != direct_data[i]) {
// 			pr_err("data8 0x%X != direct_data[i] 0x%X\n", data8, direct_data[i]);
// 			stat = 1;
// 		}
// 	}

// 	pr_err("Test direct register %s\n", (0 == stat) ? "Passed" : "*** FAILED ***");
// 	if (stat != 0) {
// 		return API_CMS_ERROR_ERROR;
// 	}

// 	/* Indirect register SPI loop rd/wr test */
// 	stat = 0;
// 	for (i = 0; i < sizeof(indirect_addr) / sizeof(indirect_addr[0]); i++) {
// 		if (err = adi_apollo_hal_reg_set(device, indirect_addr[i], indirect_data[i]), err != API_CMS_ERROR_OK) {
// 			return err;
// 		}
// 		if (err = adi_apollo_hal_reg_get(device, indirect_addr[i], &data8), err != API_CMS_ERROR_OK) {
// 			return err;
// 		}

// 		if (data8 != indirect_data[i]) {
// 			pr_err("Test indirect register 0x%X - 0x%X\n", data8, indirect_data[i]);
// 			stat = 1;
// 		}
// 	}

// 	pr_err("Test indirect register %s\n", (0 == stat) ? "Passed" : "*** FAILED ***");
// 	if (stat != 0) {
// 		return API_CMS_ERROR_ERROR;
// 	}

// 	/* 32-bit ARM mem rd/wr test */
// 	stat = 0;
// 	for (i = 0; i < sizeof(arm_addr) / sizeof(arm_addr[0]); i++) {
// 		if (err = adi_apollo_hal_reg32_set(device, arm_addr[i], arm_data[i]), err != API_CMS_ERROR_OK) {
// 			return err;
// 		}
// 		if (err = adi_apollo_hal_reg32_get(device, arm_addr[i], &data32), err != API_CMS_ERROR_OK) {
// 			return err;
// 		}


// 		if (data32 != arm_data[i]) {
// 			pr_err("Test ARM memory 0x%X - 0x%X\n", data32, arm_data[i]);
// 			stat = 1;
// 		} else {
// 			stat = 0;
// 		}
// 	}

// 	pr_err("Test ARM memory %s\n", (0 == stat) ? "Passed" : "*** FAILED ***");
// 	if (stat != 0) {
// 		return API_CMS_ERROR_ERROR;
// 	}

// 	return API_CMS_ERROR_OK;
// }

// static irqreturn_t ad9088_irq_handler(int irq, void *p)
// {
// 	struct axiadc_converter *conv = p;
// 	struct ad9088_phy *phy = conv->phy;
// 	u64 status64;

// 	//adi_ad9088_dac_irqs_status_get(&phy->ad9088, &status64);
// 	dev_err(&phy->spi->dev, "DAC IRQ status 0x%llX\n", status64);

// 	return IRQ_HANDLED;
// }

// static void ad9088_reg_disable(void *data)
// {
// 	regulator_disable(data);
// }

// static int ad9088_hsci_manual_linkup(void *user_data, uint8_t enable, uint16_t link_up_signal_bits)
// {
// 	struct axiadc_converter *conv = user_data;
// 	struct ad9088_phy *phy = conv->phy;

// 	dev_dbg(&phy->spi->dev, "%s:%d\n", __func__, __LINE__);

// 	return axi_hsci_manual_linkup(phy->hsci, enable, link_up_signal_bits);
// }

// static int ad9088_hsci_auto_linkup(void *user_data, uint8_t enable, uint8_t hscim_mosi_clk_inv, uint8_t hscim_miso_clk_inv)
// {
// 	struct axiadc_converter *conv = user_data;
// 	struct ad9088_phy *phy = conv->phy;

// 	dev_dbg(&phy->spi->dev, "%s:%d\n", __func__, __LINE__);

// 	return axi_hsci_auto_linkup(phy->hsci, enable, hscim_mosi_clk_inv, hscim_miso_clk_inv);
// }

// static int ad9088_hsci_alink_tbl_get(void *user_data, uint16_t *hscim_alink_table)
// {
// 	struct axiadc_converter *conv = user_data;
// 	struct ad9088_phy *phy = conv->phy;

// 	dev_dbg(&phy->spi->dev, "%s:%d\n", __func__, __LINE__);

// 	return axi_hsci_alink_tbl_get(phy->hsci, hscim_alink_table);
// }

// static int ad9088_hsci_read(void *user_data, const uint8_t tx_data[], uint8_t rx_data[], uint32_t num_tx_rx_bytes, adi_apollo_hal_txn_config_t *txn_config)
// {
// 	struct axiadc_converter *conv = user_data;
// 	struct ad9088_phy *phy = conv->phy;
// 	int ret;

// 	dev_dbg(&phy->spi->dev, "%s:%d\n", __func__, __LINE__);

// 	ret = axi_hsci_readm(phy->hsci, tx_data, rx_data, num_tx_rx_bytes,
// 			      txn_config->addr_len, txn_config->data_len, txn_config->stream_len);

// 	return ret;
// }

// static int ad9088_hsci_write(void *user_data, const uint8_t tx_data[], uint32_t num_tx_bytes, adi_apollo_hal_txn_config_t *txn_config)
// {
// 	struct axiadc_converter *conv = user_data;
// 	struct ad9088_phy *phy = conv->phy;
// 	int ret;

// 	dev_dbg(&phy->spi->dev, "%s:%d\n", __func__, __LINE__);

// 	ret = axi_hsci_writem(phy->hsci, tx_data, num_tx_bytes,
// 			      txn_config->addr_len, txn_config->data_len, txn_config->stream_len);

// 	return ret;
// }

// static int ad9088_input_gpio(struct gpio_chip *chip, unsigned int offset)
// {
// 	struct ad9088_phy *phy = gpiochip_get_data(chip);

// 	guard(mutex)(&phy->lock);

// 	return adi_apollo_gpio_cmos_gpio_mode_set(&phy->ad9088, phy->gpios_exported[offset],
// 						  ADI_APOLLO_GPIO_DIR_INPUT);
// }

// static int ad9088_output_gpio(struct gpio_chip *chip,
// 			      unsigned int offset, int value)
// {
// 	struct ad9088_phy *phy = gpiochip_get_data(chip);
// 	int ret;

// 	guard(mutex)(&phy->lock);

// 	ret = adi_apollo_gpio_cmos_output_set(&phy->ad9088, phy->gpios_exported[offset], value);
// 	if (ret < 0)
// 		return ret;

// 	return adi_apollo_gpio_cmos_gpio_mode_set(&phy->ad9088, phy->gpios_exported[offset],
// 						  ADI_APOLLO_GPIO_DIR_OUTPUT);
// }

// static int ad9088_get_gpio(struct gpio_chip *chip, unsigned int offset)
// {
// 	struct ad9088_phy *phy = gpiochip_get_data(chip);
// 	u8 val;
// 	int ret;

// 	guard(mutex)(&phy->lock);

// 	ret = adi_apollo_gpio_cmos_input_get(&phy->ad9088, phy->gpios_exported[offset], &val);
// 	if (ret < 0)
// 		return ret;

// 	return !!val;
// }

// static void ad9088_set_gpio(struct gpio_chip *chip, unsigned int offset, int value)
// {
// 	struct ad9088_phy *phy = gpiochip_get_data(chip);

// 	guard(mutex)(&phy->lock);

// 	adi_apollo_gpio_cmos_output_set(&phy->ad9088, phy->gpios_exported[offset], value);
// }

// int ad9088_gpio_setup(struct ad9088_phy *phy)
// {
// 	int ret, len;

// 	len = device_property_count_u8(&phy->spi->dev, "adi,gpio-exports");
// 	if (len < 0 || len > ARRAY_SIZE(phy->gpios_exported))
// 		return -EINVAL;

// 	ret = device_property_read_u8_array(&phy->spi->dev, "adi,gpio-exports",
// 					     phy->gpios_exported, len);
// 	if (ret < 0)
// 		return ret;

// 	phy->gpiochip.label = "ad9088";
// 	phy->gpiochip.base = -1;
// 	phy->gpiochip.ngpio = len;
// 	phy->gpiochip.parent = &phy->spi->dev;
// 	phy->gpiochip.can_sleep = true;
// 	phy->gpiochip.direction_input = ad9088_input_gpio;
// 	phy->gpiochip.direction_output = ad9088_output_gpio;
// 	phy->gpiochip.get = ad9088_get_gpio;
// 	phy->gpiochip.set = ad9088_set_gpio;

// 	return devm_gpiochip_add_data(&phy->spi->dev, &phy->gpiochip, phy);
// }

// static int ad9088_probe(struct spi_device *spi)
// {
// 	struct axiadc_converter *conv;
// 	struct ad9088_phy *phy;
// 	struct jesd204_dev *jdev;
// 	struct ad9088_jesd204_priv *priv;
// 	struct gpio_desc *gpio;
// 	struct clock_scale devclk_clkscale;
// 	struct device_node *node = spi->dev.of_node;
// 	u16 api_rev[3];
// 	int ret;

// 	jdev = devm_jesd204_dev_register(&spi->dev, &jesd204_ad9088_init);
// 	if (IS_ERR(jdev))
// 		return PTR_ERR(jdev);

// 	if (!jdev) {
// 		dev_err(&spi->dev, "Failed to register jesd204-fsm device");
// 		return -ENODEV;
// 	}

// 	conv = devm_kzalloc(&spi->dev, sizeof(*conv), GFP_KERNEL);
// 	if (conv == NULL)
// 		return -ENOMEM;

// 	phy = devm_kzalloc(&spi->dev, sizeof(*phy), GFP_KERNEL);
// 	if (phy == NULL)
// 		return -ENOMEM;

// 	phy->hsci = devm_axi_hsci_get_optional(&spi->dev);
// 	if (IS_ERR(phy->hsci))
// 		return PTR_ERR(phy->hsci);

// 	conv->adc_clkscale.mult = 1;
// 	conv->adc_clkscale.div = 1;

// 	spi_set_drvdata(spi, conv);
// 	conv->spi = spi;
// 	conv->phy = phy;
// 	phy->spi = spi;
// 	phy->jdev = jdev;
// 	priv = jesd204_dev_priv(jdev);
// 	priv->phy = phy;

// 	ret = ad9088_request_clks(conv);
// 	if (ret)
// 		return ret;

// 	conv->reset_gpio =
// 		devm_gpiod_get_optional(&spi->dev, "reset", GPIOD_OUT_HIGH);
// 	if (IS_ERR(conv->reset_gpio))
// 		return PTR_ERR(conv->reset_gpio);

// 	phy->rx1_en_gpio =
// 		devm_gpiod_get_optional(&spi->dev, "rx1-enable", GPIOD_OUT_LOW);
// 	if (IS_ERR(phy->rx1_en_gpio))
// 		return PTR_ERR(phy->rx1_en_gpio);

// 	phy->rx2_en_gpio =
// 		devm_gpiod_get_optional(&spi->dev, "rx2-enable", GPIOD_OUT_LOW);
// 	if (IS_ERR(phy->rx2_en_gpio))
// 		return PTR_ERR(phy->rx2_en_gpio);

// 	phy->tx1_en_gpio =
// 		devm_gpiod_get_optional(&spi->dev, "tx1-enable", GPIOD_OUT_LOW);
// 	if (IS_ERR(phy->tx1_en_gpio))
// 		return PTR_ERR(phy->tx1_en_gpio);

// 	phy->tx2_en_gpio =
// 		devm_gpiod_get_optional(&spi->dev, "tx2-enable", GPIOD_OUT_LOW);
// 	if (IS_ERR(phy->tx2_en_gpio))
// 		return PTR_ERR(phy->tx2_en_gpio);

// 	phy->triq_req_gpio =
// 		devm_gpiod_get_optional(&spi->dev, "trig-req", GPIOD_OUT_LOW);
// 	if (IS_ERR(phy->triq_req_gpio))
// 		return PTR_ERR(phy->triq_req_gpio);

// 	phy->transceiver_reset_gpio =
// 		devm_gpiod_get_optional(&spi->dev, "versal-transceiver-reset", GPIOD_OUT_LOW);
// 	if (IS_ERR(phy->transceiver_reset_gpio))
// 		return PTR_ERR(phy->transceiver_reset_gpio);


// 	phy->iio_adf4030 = devm_fwnode_iio_channel_get_by_name(&spi->dev, of_fwnode_handle(node), "bsync");
// 	if (IS_ERR(phy->iio_adf4030) && PTR_ERR(phy->iio_adf4030) != -ENOENT && PTR_ERR(phy->iio_adf4030) != -ENODEV)
// 		return dev_err_probe(&spi->dev, PTR_ERR(phy->iio_adf4030), "%s: error getting channel\n",
// 				     "bsync");
// 	phy->iio_adf4382 = devm_fwnode_iio_channel_get_by_name(&spi->dev, of_fwnode_handle(node), "clk");
// 	if (IS_ERR(phy->iio_adf4382) && PTR_ERR(phy->iio_adf4382) != -ENOENT && PTR_ERR(phy->iio_adf4382) != -ENODEV)
// 		return dev_err_probe(&spi->dev, PTR_ERR(phy->iio_adf4382), "%s: error getting channel\n",
// 				     "clk");

// 	mutex_init(&phy->lock);

// 	ret = ad9088_parse_dt(phy);
// 	if (ret < 0)
// 		return dev_err_probe(&spi->dev, ret, "Parsing devicetree failed \n");

// 	phy->supply_reg = devm_regulator_get(&spi->dev, "vdd");
// 	if (IS_ERR(phy->supply_reg))
// 		return dev_err_probe(&spi->dev, PTR_ERR(phy->supply_reg),
// 				     "failed to get the vdd supply regulator\n");

// 	if (phy->supply_reg) {
// 		ret = regulator_enable(phy->supply_reg);
// 		if (ret) {
// 			dev_err(&spi->dev, "Failed to enable vdd supply voltage!\n");
// 			return ret;
// 		}

// 		ret = devm_add_action_or_reset(&spi->dev, ad9088_reg_disable, phy->supply_reg);
// 		if (ret)
// 			return ret;
// 	}

// 	of_clk_get_scale(spi->dev.of_node, "dev_clk", &devclk_clkscale);

// 	clk_set_rate_scaled(phy->dev_clk,
// 		(u64)phy->profile.clk_cfg.dev_clk_freq_kHz * 1000,
// 		&devclk_clkscale);

// 	phy->ad9088.hal_info.spi0_desc.spi_config.sdo = (spi->mode & SPI_3WIRE || phy->spi_3wire_en) ?
// 			ADI_APOLLO_DEVICE_SPI_SDIO : ADI_APOLLO_DEVICE_SPI_SDO;
// 	phy->ad9088.hal_info.spi0_desc.spi_config.msb = (spi->mode & SPI_LSB_FIRST) ?
// 			ADI_APOLLO_DEVICE_SPI_MSB_LAST : ADI_APOLLO_DEVICE_SPI_MSB_FIRST;
// 	phy->ad9088.hal_info.spi0_desc.spi_config.addr_inc = ADI_APOLLO_DEVICE_SPI_ADDR_INC_AUTO;

// 	phy->ad9088.hal_info.spi0_desc.is_used = 1;
// 	phy->ad9088.hal_info.spi0_desc.dev_obj = conv;
// 	phy->ad9088.hal_info.user_data = conv;
// 	phy->ad9088.hal_info.delay_us = ad9088_udelay;
// 	phy->ad9088.hal_info.spi0_desc.read = ad9088_spi_read;
// 	phy->ad9088.hal_info.spi0_desc.write = ad9088_spi_write;
// 	phy->ad9088.hal_info.spi0_desc.xfer = ad9088_spi_xfer;
// 	phy->ad9088.hal_info.reset_pin_ctrl = ad9088_reset_pin_ctrl;
// 	phy->ad9088.hal_info.log_write = ad9088_log_write;

// 	if (phy->hsci) {
// 		phy->ad9088.hal_info.hsci_desc.is_used = 1;
// 		phy->ad9088.hal_info.hsci_desc.dev_obj = conv;
// 		phy->ad9088.hal_info.hsci_desc.hsci_config.auto_linkup_en = phy->hsci_use_auto_linkup_mode;
// 		phy->ad9088.hal_info.hsci_desc.hsci_config.addr_inc = ADI_APOLLO_DEVICE_HSCI_ADDR_INC_AUTO;
// 		phy->ad9088.hal_info.hsci_desc.manual_linkup = &ad9088_hsci_manual_linkup;
// 		phy->ad9088.hal_info.hsci_desc.auto_linkup = &ad9088_hsci_auto_linkup;
// 		phy->ad9088.hal_info.hsci_desc.alink_tbl_get = &ad9088_hsci_alink_tbl_get;
// 		phy->ad9088.hal_info.hsci_desc.read = &ad9088_hsci_read;
// 		phy->ad9088.hal_info.hsci_desc.write = &ad9088_hsci_write;
// 		phy->ad9088.hal_info.hsci_desc.poll_read = NULL;
// 		phy->ad9088.hal_info.hsci_desc.ptr_data_buf = phy->hsci_buf;
// 	}

// 	ret = adi_apollo_device_hw_open(&phy->ad9088,
// 					conv->reset_gpio ? ADI_APOLLO_HARD_RESET_AND_INIT :
// 					ADI_APOLLO_SOFT_RESET_AND_INIT);
// 	if (ret < 0) {
// 		dev_err(&spi->dev, "reset/init failed (%d)\n", ret);
// 		return -ENODEV;
// 	}

// 	ret = adi_apollo_hal_active_protocol_set(&phy->ad9088, ADI_APOLLO_HAL_PROTOCOL_SPI0);
// 	if (ret < 0) {
// 		dev_err(&spi->dev, "SPI active protocol set failed (%d)\n", ret);
// 		return -ENODEV;
// 	}

// 	ret = adi_apollo_hal_rmw_enable_set(&phy->ad9088, ADI_APOLLO_HAL_PROTOCOL_SPI0, 0);
// 	if (ret < 0) {
// 		dev_err(&spi->dev, "SPI rmw enable failed (%d)\n", ret);
// 		return -ENODEV;
// 	}

// 	if (phy->hsci) {
// 		ret = adi_apollo_hal_rmw_enable_set(&phy->ad9088, ADI_APOLLO_HAL_PROTOCOL_HSCI, 0);
// 		if (ret < 0) {
// 			dev_err(&spi->dev, "HSCI rmw enable  failed (%d)\n", ret);
// 			return -ENODEV;
// 		}
// 		ret = adi_apollo_hal_active_protocol_set(&phy->ad9088, ADI_APOLLO_HAL_PROTOCOL_HSCI);
// 		if (ret < 0) {
// 		dev_err(&spi->dev, "HSCI active protocol set failed (%d)\n", ret);
// 			return -ENODEV;
// 		}
// 	}

// 	ad9088_clk_register(phy, "-rx_sampl_clk",
// 			    __clk_get_name(phy->dev_clk), NULL,
// 			    CLK_GET_RATE_NOCACHE | CLK_IGNORE_UNUSED,
// 			    RX_SAMPL_CLK);

// 	ad9088_clk_register(phy, "-tx_sampl_clk",
// 			    __clk_get_name(phy->dev_clk), NULL,
// 			    CLK_GET_RATE_NOCACHE | CLK_IGNORE_UNUSED,
// 			    TX_SAMPL_CLK);

// 	// if (!phy->rx_disable && ad9088_link_is_dual(phy->jtx_link_rx))
// 	//	ad9088_clk_register(phy, "-rx_sampl_clk_link2",
// 	//			__clk_get_name(phy->dev_clk), NULL,
// 	//			CLK_GET_RATE_NOCACHE | CLK_IGNORE_UNUSED,
// 	//			RX_SAMPL_CLK_LINK2);

// 	phy->clk_data.clks = phy->clks;
// 	phy->clk_data.clk_num = NUM_AD9088_CLKS;

// 	ret = of_clk_add_provider(spi->dev.of_node, of_clk_src_onecell_get,
// 				  &phy->clk_data);

// 	phy->loopback_mode[0] = ADI_APOLLO_LOOPBACK_NONE;
// 	phy->loopback_mode[1] = ADI_APOLLO_LOOPBACK_NONE;
// 	phy->lb1_blend[0] = 0;
// 	phy->lb1_blend[1] = 0;

// 	INIT_DELAYED_WORK(&phy->dwork, ad9088_work_func);

// 	ret = adi_apollo_device_chip_id_get(&phy->ad9088, &phy->chip_id);
// 	if (ret < 0) {
// 		dev_err(&spi->dev, "chip_id failed (%d)\n", ret);
// 		return ret;
// 	}

// 	switch (spi_get_device_id(spi)->driver_data & CHIPID_MASK) {
// 	case CHIPID_AD9084:
// 	case CHIPID_AD9088:
// 		ret = ad9088_reg_test(&phy->ad9088);
// 		if (ret)
// 			break;
// 		ret = ad9088_setup(phy);
// 		if (ret) {
// 			ad9088_reg_test(&phy->ad9088);
// 			break;
// 		}
// 		conv->id = phy->chip_id.prod_id;
// 		conv->chip_info = &phy->chip_info;
// 		ret = ad9088_setup_chip_info_tbl(phy, phy->complex_rx, phy->complex_tx,
// 						 jesd204_dev_is_top(jdev));
// 		if (ret)
// 			break;
// 		break;
// 	default:
// 		dev_err(&spi->dev, "Unrecognized CHIP_ID 0x%X\n", conv->id);
// 		return -ENODEV;
// 	}

// 	if (ret) {
// 		if (ret != -EPROBE_DEFER)
// 			dev_err(&spi->dev, "Failed to initialize: %d\n", ret);
// 		goto out_clk_del_provider;
// 	}

// 	conv->clk = phy->clks[RX_SAMPL_CLK];
// 	conv->reg_access = ad9088_reg_access;
// 	conv->write_raw = ad9088_write_raw;
// 	conv->read_raw = ad9088_read_raw;
// 	conv->read_label = ad9088_read_label;
// #if 0
// 	conv->read_event_value = ad9088_read_thresh,
// 	      conv->write_event_value = ad9088_write_thresh,
// 		    conv->read_event_config = ad9088_read_thresh_en,
// 			  conv->write_event_config = ad9088_write_thresh_en,
// 				conv->set_pnsel = ad9088_set_pnsel;
// #endif
// 	conv->post_setup = ad9088_post_setup;
// 	conv->post_iio_register = ad9088_post_iio_register;

// 	conv->attrs = &ad9088_phy_attribute_group;

// 	if (phy->standalone || !jesd204_dev_is_top(jdev)) {
// 		ret = ad9088_register_iiodev(conv);
// 		if (ret)
// 			goto out_clk_del_provider;
// 	}

// 	ret = ad9088_request_fd_irqs(conv);
// 	if (ret < 0)
// 		dev_warn(&spi->dev,
// 			 "Failed to request FastDetect IRQs (%d)", ret);

// 	gpio = devm_gpiod_get(&spi->dev, "irqb0", GPIOD_IN);
// 	if (0 && !IS_ERR(gpio)) { /* REVIST: Not yet used */
// 		int irq = gpiod_to_irq(gpio);

// 		if (irq >= 0) {
// 			ret = devm_request_threaded_irq(&spi->dev, irq, NULL,
// 							ad9088_irq_handler,
// 							IRQF_TRIGGER_FALLING  | IRQF_ONESHOT,
// 							spi->dev.of_node ? spi->dev.of_node->name :
// 							spi_get_device_id(spi)->name, conv);

// 			if (ret) {
// 				dev_err(&spi->dev,
// 					"request_irq() failed with %d\n", ret);
// 				goto out_clk_del_provider;
// 			}
// 		}
// 	}

// 	adi_apollo_device_api_revision_get(&phy->ad9088, &api_rev[0],
// 					   &api_rev[1], &api_rev[2]);

// 	dev_info(&spi->dev, "AD%X  Rev. %u Grade %u (API %u.%u.%u) probed\n",
// 		 phy->chip_id.prod_id, phy->chip_id.dev_revision,
// 		 phy->chip_id.prod_grade, api_rev[0], api_rev[1], api_rev[2]);

// 	if (phy->transceiver_reset_gpio) {
// 		gpiod_set_value(phy->transceiver_reset_gpio, 1);
// 		ad9088_udelay(NULL, 100000);
// 		gpiod_set_value(phy->transceiver_reset_gpio, 0);
// 	}

// 	ad9088_fft_sniffer_probe(phy, ADI_APOLLO_SIDE_A);
// 	ad9088_fft_sniffer_probe(phy, ADI_APOLLO_SIDE_B);

// 	ad9088_gpio_setup(phy);

// 	ret = jesd204_fsm_start(jdev, JESD204_LINKS_ALL);
// 	if (ret)
// 		goto out_clk_del_provider;

// 	return 0;

// out_clk_del_provider:
// 	of_clk_del_provider(spi->dev.of_node);

// 	return ret;
// }

// static void ad9088_remove(struct spi_device *spi)
// {
// 	struct axiadc_converter *conv = spi_get_drvdata(spi);
// 	struct ad9088_phy *phy = conv->phy;

// 	cancel_delayed_work_sync(&phy->dwork);

// 	if (phy->standalone || !jesd204_dev_is_top(phy->jdev))
// 		iio_device_unregister(conv->indio_dev);

// 	clk_disable_unprepare(phy->dev_clk);
// 	of_clk_del_provider(spi->dev.of_node);
// 	adi_apollo_hal_hw_close(&phy->ad9088);
// }

// static const struct spi_device_id ad9088_id[] = {
// 	{ "ad9084", CHIPID_AD9084 },
// 	{ "ad9088", CHIPID_AD9088 },
// 	{}
// };
// MODULE_DEVICE_TABLE(spi, ad9088_id);

// static const struct of_device_id ad9088_of_match[] = {
// 	{ .compatible = "adi,ad9084" },
// 	{ .compatible = "adi,ad9088" },
// 	{},
// };
// MODULE_DEVICE_TABLE(of, ad9088_of_match);

// static struct spi_driver ad9088_driver = {
// 	.driver = {
// 		.name = "ad9088",
// 		.of_match_table = of_match_ptr(ad9088_of_match),
// 	},
// 	.probe = ad9088_probe,
// 	.remove = ad9088_remove,
// 	.id_table = ad9088_id,
// };
// module_spi_driver(ad9088_driver);

// MODULE_AUTHOR("Michael Hennerich <michael.hennerich@analog.com>");
// MODULE_DESCRIPTION("Analog Devices AD9088 MxFE");
// MODULE_LICENSE("GPL v2");

