/***************************************************************************//**
 *   @file   adp20086.h
 *   @brief  Header file for the ADP20086 driver.
 *   @author Mark John Lerry Casero (markjohnlerry.casero@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __ADP20086_H__
#define __ADP20086_H__

#include "no_os_i2c.h"
#include "no_os_util.h"

#define ADP20086_REG_MASK					0x00
#define ADP20086_REG_CONFIG					0x01
#define ADP20086_REG_ID						0x02
#define ADP20086_REG_STAT1					0x03
#define ADP20086_REG_STAT2					0x04
#define ADP20086_REG_STAT3					0x05
#define ADP20086_REG_ADC_LEGACY(n)			(0x06 + (n))
#define ADP20086_REG_MASK2					0x0A
#define ADP20086_REG_MASK3					0x0B
#define ADP20086_REG_ILIM(ch)				(0x0C + (ch >> 1))	/* 0x0C for ILIM12, 0x0D for ILIM34*/
#define ADP20086_REG_LDET_SET(ch)			(0x0E + (ch))
#define ADP20086_REG_OVIN_SET				0x12
#define ADP20086_REG_UVIN_SET				0x13
#define ADP20086_REG_IOPEN_SET(ch)			(0x14 + (ch))
#define ADP20086_REG_UVOUT_SET(ch)			(0x18 + (ch))
#define ADP20086_REG_START_SLEW				0x1C
#define ADP20086_REG_SHDN_SLEW				0x1D
#define ADP20086_REG_VIN_READ				0x1E
#define ADP20086_REG_VOUT_READ(ch)			(0x1F + (ch))
#define ADP20086_REG_IOUT_READ(ch)			(0x23 + (ch))
#define ADP20086_REG_VDD_READ				0x27
#define ADP20086_REG_STAT4					0x28
#define ADP20086_REG_STAT5					0x29
#define ADP20086_REG_LOAD_INT_EN			0x2A

#define ADP20086_MASK_OVTST					NO_OS_BIT(7)
#define ADP20086_MASK_ACCM					NO_OS_BIT(6)
#define ADP20086_MASK_TSM					NO_OS_BIT(5)
#define ADP20086_MASK_VDD					NO_OS_BIT(4)
#define ADP20086_MASK_VIN					NO_OS_BIT(3)
#define ADP20086_MASK_OC					NO_OS_BIT(2)
#define ADP20086_MASK_OV					NO_OS_BIT(1)
#define ADP20086_MASK_UV					NO_OS_BIT(0)

#define ADP20086_MASK2_UV4M					NO_OS_BIT(7)
#define ADP20086_MASK2_UV3M					NO_OS_BIT(6)
#define ADP20086_MASK2_UV2M					NO_OS_BIT(5)
#define ADP20086_MASK2_UV1M					NO_OS_BIT(4)
#define ADP20086_MASK2_OPNM4				NO_OS_BIT(3)
#define ADP20086_MASK2_OPNM3				NO_OS_BIT(2)
#define ADP20086_MASK2_OPNM2				NO_OS_BIT(1)
#define ADP20086_MASK2_OPNM1				NO_OS_BIT(0)

#define ADP20086_MASK3_FCALM				NO_OS_BIT(6)
#define ADP20086_MASK3_IIR_EN				NO_OS_BIT(5)
#define ADP20086_MASK3_DISCH_EN				NO_OS_BIT(4)
#define ADP20086_MASK3_BISTM				NO_OS_BIT(3)
#define ADP20086_MASK3_PARERRM				NO_OS_BIT(2)
#define ADP20086_MASK3_ECC_CORM				NO_OS_BIT(1)
#define ADP20086_MASK3_ECC_DETM				NO_OS_BIT(0)

#define ADP20086_CONFIG_MUX					NO_OS_GENMASK(7, 6)
#define ADP20086_CONFIG_ENC					NO_OS_BIT(5)
#define ADP20086_CONFIG_CLR					NO_OS_BIT(4)  /* clear-faults-on-read (reset=1) */
#define ADP20086_CONFIG_EN(ch)				NO_OS_BIT(ch) /* ch = 0..3 -> EN1..EN4 */
#define ADP20086_CONFIG_EN_MASK				NO_OS_GENMASK(3, 0)
#define ADP20086_SINGLE_CH_ASSERTED			0x1
#define ADP20086_ALL_CH_ASSERTED			0xF

#define ADP20086_ID_PECE					NO_OS_BIT(6)
#define ADP20086_ID_ID						NO_OS_GENMASK(5, 4)
#define ADP20086_ID_REV						NO_OS_GENMASK(3, 0)

/* ILIM1, ILIM3 mask (7:4). ILIM2, ILIM4 mask (3:0).*/
#define ADP20086_ILIM_MASK(ch)				NO_OS_GENMASK(3 + (4 * ((ch ^ 1) & 1)), 0 + (4 * ((ch ^ 1) & 1)))

#define ADP20086_START_SLEW_MASK(ch)		NO_OS_GENMASK((ch * 2 + 1), (ch * 2))
#define ADP20086_SHDN_SLEW_MASK(ch)			NO_OS_GENMASK((ch * 2 + 1), (ch * 2))

/* STAT2/STAT3 are two channels per byte: low nibble = lower ch, high = upper ch.
 * Per-channel bit within a nibble: UV=bit0, OV=bit1, OC=bit2, TS=bit3. */
#define ADP20086_CHAN_STATUS_TS(ch)			(NO_OS_BIT(3) << (4 * ((ch) & 1)))
#define ADP20086_CHAN_STATUS_OC(ch)			(NO_OS_BIT(2) << (4 * ((ch) & 1)))
#define ADP20086_CHAN_STATUS_OV(ch)			(NO_OS_BIT(1) << (4 * ((ch) & 1)))
#define ADP20086_CHAN_STATUS_UV(ch)			(NO_OS_BIT(0) << (4 * ((ch) & 1)))
#define ADP20086_CHAN_STATUS_LOAD_DET(ch)	(NO_OS_BIT(ch) << 4)
#define ADP20086_CHAN_STATUS_OPEN(ch)		(NO_OS_BIT(ch))

#define ADP20086_STAT1_TLIM_SERV			NO_OS_BIT(7)
#define ADP20086_STAT1_ACC					NO_OS_BIT(4)
#define ADP20086_STAT1_OVIN					NO_OS_BIT(3)
#define ADP20086_STAT1_UVIN					NO_OS_BIT(2)
#define ADP20086_STAT1_OVDD					NO_OS_BIT(1)
#define ADP20086_STAT1_UVDD					NO_OS_BIT(0)

#define ADP20086_STAT5_ADC_FCAL				NO_OS_BIT(5)
#define ADP20086_STAT5_INTBSTS				NO_OS_BIT(4)
#define ADP20086_STAT5_BIST					NO_OS_BIT(3)
#define ADP20086_STAT5_PARERR				NO_OS_BIT(2)
#define ADP20086_STAT5_ECC_COR				NO_OS_BIT(1)
#define ADP20086_STAT5_ECC_DET				NO_OS_BIT(0)

/* Fatal STAT5 self-test / integrity faults: ADC_FCAL..PARERR (bits 5:2)
 * plus ECC_DET (bit 0). ECC_COR (bit 1) is a corrected, non-fatal error
 * and is intentionally excluded. */
#define ADP20086_SELFTEST_FAULT_MASK		(NO_OS_GENMASK(5, 2) | ADP20086_STAT5_ECC_DET)

#define ADP20086_LDET_EN_MASK				NO_OS_GENMASK(7, 4)
#define ADP20086_LDET_EN(ch)				(NO_OS_BIT(ch + 4))
#define ADP20086_OPEN_PROT_MASK				NO_OS_GENMASK(3, 0)
#define ADP20086_OPEN_PROT(ch)				(NO_OS_BIT(ch))

/* ---- Telemetry LSBs (integer micro-units) ---- */
#define ADP20086_VIN_LSB_UV					72000u	/* 72 mV */
#define ADP20086_VOUT_LSB_UV				72000u	/* 72 mV */
#define ADP20086_VDD_LSB_UV					9000u	/* 9 mV */
#define ADP20086_IOUT_LSB_UA				3500u	/* 3.5 mA */
#define ADP20086_ILIM_LSB_UA				52000u
#define ADP20086_LDET_LSB_UA				3500u	/* LDET: 3.5 mA */
#define ADP20086_IOPEN_LSB_UA				3500u	/* IOPEN: 3.5 mA */

#define ADP20086_MAX_ILIM_UA				832000u
#define ADP20086_MAX_LDET_UA				892500u
#define ADP20086_MAX_IOPEN_UA				892500u
#define ADP20086_MAX_VOUT_UV				18360000u
#define ADP20086_MAX_VIN_UV					18360000u

/* ---- I2C addressing ---- */
#define ADP20086_BASE_ADDR					0x28
#define ADP20086_3P3KOHM_ADDR				0x29
#define ADP20086_6P8KOHM_ADDR				0x2A
#define ADP20086_13P7KOHM_ADDR				0x2B
#define ADP20086_20KOHM_ADDR				0x2C
#define ADP20086_33KOHM_ADDR				0x2D
#define ADP20086_61P9KOHM_ADDR				0x2E
#define ADP20086_OPEN_ADDR					0x2F

/* ---- PEC ---- */
#define ADP20086_CRC8_POLY					0x07	/* x^8 + x^2 + x + 1 */
#define ADP20086_WR_FRAME_SIZE				4		/* address, register, data, PEC*/
#define ADP20086_RD_FRAME_SIZE				5		/* address, register, address, data, PEC*/

enum adp20086_channel {
	ADP20086_ALL_CHANNELS = -1,
	ADP20086_CHANNEL1,
	ADP20086_CHANNEL2,
	ADP20086_CHANNEL3,
	ADP20086_CHANNEL4,
};

enum adp20086_ilim_available {
	ADP20086_ILIM_52MA,
	ADP20086_ILIM_104MA,
	ADP20086_ILIM_156MA,
	ADP20086_ILIM_208MA,
	ADP20086_ILIM_260MA,
	ADP20086_ILIM_312MA,
	ADP20086_ILIM_364MA,
	ADP20086_ILIM_416MA,
	ADP20086_ILIM_468MA,
	ADP20086_ILIM_520MA,
	ADP20086_ILIM_572MA,
	ADP20086_ILIM_624MA,
	ADP20086_ILIM_676MA,
	ADP20086_ILIM_728MA,
	ADP20086_ILIM_780MA,
	ADP20086_ILIM_832MA,
};

enum adp20086_parameters {
	ADP20086_PARAM_LDET,
	ADP20086_PARAM_OVIN,
	ADP20086_PARAM_UVIN,
	ADP20086_PARAM_IOPEN,
	ADP20086_PARAM_UVOUT,
	ADP20086_PARAM_CHANNELS,
	ADP20086_PARAM_LOAD_DETECTION,
	ADP20086_PARAM_OPEN_PROTECTION,
};

enum adp20086_soft_start {        /* SS_RAMPn */
    ADP20086_SSU_0R5MS, ADP20086_SSU_1MS, ADP20086_SSU_2MS, ADP20086_SSU_4MS,
};

enum adp20086_soft_shutdown {         /* SSD_RAMPn */
    ADP20086_SSD_0R25MS, ADP20086_SSD_0R5MS, ADP20086_SSD_1MS, ADP20086_SSD_2MS,
};

enum adp20086_interrupt_source {
	ADP20086_INT_ACCM,
	ADP20086_INT_TSM,
	ADP20086_INT_VDDM,
	ADP20086_INT_VINM,
	ADP20086_INT_OCM,
	ADP20086_INT_OVM,
	ADP20086_INT_UVM,
	ADP20086_INT_UV4M,
	ADP20086_INT_UV3M,
	ADP20086_INT_UV2M,
	ADP20086_INT_UV1M,
	ADP20086_INT_OPNM4,
	ADP20086_INT_OPNM3,
	ADP20086_INT_OPNM2,
	ADP20086_INT_OPNM1,
	ADP20086_INT_FCALM,
	ADP20086_INT_BISTM,
	ADP20086_INT_PARERRM,
	ADP20086_INT_ECC_CORM,
	ADP20086_INT_ECC_DETM,
};

enum adp20086_legacy_adc_mux_config {
	ADP20086_LEGACY_MUX_IOUT,
	ADP20086_LEGACY_MUX_VOUT,
	ADP20086_LEGACY_MUX_VIN_VDD,
};

struct adp20086_channel_status {
	bool thermal_shutdown;
	bool overcurrent;
	bool overvoltage;
	bool undervoltage;
	bool load_detected;
	bool open_load;
};

struct adp20086_device_status {
	bool tlim_serv;
	bool acc;
	bool ovin;
	bool uvin;
	bool ovdd;
	bool uvdd;
	bool adc_fcal;
	bool intbsts;
	bool bist;
	bool parerr;
	bool ecc_cor;
	bool ecc_det;
};

enum adp20086_legacy_adc {
	ADP20086_ADC1_LEGACY,
	ADP20086_ADC2_LEGACY,
	ADP20086_ADC3_LEGACY,
	ADP20086_ADC4_LEGACY,
};

struct adp20086_dev {
	struct no_os_i2c_desc *i2c_desc;
	bool pece;									/* PEC enable (auto-detected from ID.PECE) */
	uint8_t part_id;							/* ID[5:4] read back at init */
	uint8_t revision;							/* Rev[3:0] read back at init */
};

struct adp20086_init_param {
	struct no_os_i2c_init_param *i2c_param;
	bool check_part_id;							/* verify ID[5:4] at init */
	uint8_t expected_part_id;					/* expected ID[5:4] when check_part_id */
};

/* lifecycle */
int adp20086_init(struct adp20086_dev **dev,
		  struct adp20086_init_param *init_param);
int adp20086_remove(struct adp20086_dev *dev);

/* register primitives */
int adp20086_read(struct adp20086_dev *dev, uint8_t reg, uint8_t *data);
int adp20086_write(struct adp20086_dev *dev, uint8_t reg, uint8_t data);
int adp20086_update_register(struct adp20086_dev *dev, uint8_t reg, uint8_t mask, uint8_t data);
int adp20086_get_register_field(struct adp20086_dev *dev, uint8_t reg, uint8_t mask, uint8_t *data);

/* CONFIG register */
int adp20086_configure_mux(struct adp20086_dev *dev, enum adp20086_legacy_adc_mux_config config);
int adp20086_configure_enc(struct adp20086_dev *dev, bool enabled);
int adp20086_configure_clr(struct adp20086_dev *dev, bool enabled);
int adp20086_get_mux_config(struct adp20086_dev *dev, enum adp20086_legacy_adc_mux_config *config);
int adp20086_get_enc_config(struct adp20086_dev *dev, bool *enabled);
int adp20086_get_clr_config(struct adp20086_dev *dev, bool *enabled);

/* channel / protection enable */
int adp20086_enable_channel(struct adp20086_dev *dev, enum adp20086_channel ch);
int adp20086_disable_channel(struct adp20086_dev *dev, enum adp20086_channel ch);
int adp20086_enable_load_detection(struct adp20086_dev *dev, enum adp20086_channel ch);
int adp20086_disable_load_detection(struct adp20086_dev *dev, enum adp20086_channel ch);
int adp20086_enable_open_protection(struct adp20086_dev *dev, enum adp20086_channel ch);
int adp20086_disable_open_protection(struct adp20086_dev *dev, enum adp20086_channel ch);

/* identity */
int adp20086_get_pece(struct adp20086_dev *dev, bool *pec_enabled);
int adp20086_get_part_id(struct adp20086_dev *dev, uint8_t *part_id);
int adp20086_get_revision(struct adp20086_dev *dev, uint8_t *revision);

/* interrupt masks & feature bits */
int adp20086_set_interrupt_mask(struct adp20086_dev *dev, enum adp20086_interrupt_source src, bool masked);
int adp20086_set_ovtst(struct adp20086_dev *dev, bool enabled);
int adp20086_set_iir_filter(struct adp20086_dev *dev, bool enabled);
int adp20086_set_discharge_resistors(struct adp20086_dev *dev, bool enabled);
int adp20086_get_interrupt_mask(struct adp20086_dev *dev, enum adp20086_interrupt_source src, bool *masked);
int adp20086_get_ovtst(struct adp20086_dev *dev, bool *enabled);
int adp20086_get_iir_filter(struct adp20086_dev *dev, bool *enabled);
int adp20086_get_discharge_resistors(struct adp20086_dev *dev, bool *enabled);

/* status */
int adp20086_get_channel_status(struct adp20086_dev *dev, enum adp20086_channel ch, struct adp20086_channel_status *status);
int adp20086_get_device_status(struct adp20086_dev *dev, struct adp20086_device_status *status);
int adp20086_check_self_test(struct adp20086_dev *dev);

/* legacy MAX20087-compatible ADC mux read */
int adp20086_read_legacy_adc(struct adp20086_dev *dev, enum adp20086_legacy_adc adc, uint8_t *data);

/* current limit */
int adp20086_set_ilim(struct adp20086_dev *dev, enum adp20086_channel ch, enum adp20086_ilim_available ilim);
int adp20086_get_ilim_code(struct adp20086_dev *dev, enum adp20086_channel ch, uint8_t *code);
int adp20086_get_ilim_ua(struct adp20086_dev *dev, enum adp20086_channel ch, uint32_t *ilim_ua);

/* load-detect threshold */
int adp20086_set_ldet_threshold(struct adp20086_dev *dev, enum adp20086_channel ch, uint32_t threshold_ua);
int adp20086_get_ldet_threshold_code(struct adp20086_dev *dev, enum adp20086_channel ch, uint8_t *code);
int adp20086_get_ldet_threshold_ua(struct adp20086_dev *dev, enum adp20086_channel ch, uint32_t *threshold_ua);

/* input over-voltage threshold */
int adp20086_set_ovin(struct adp20086_dev *dev, uint32_t ovin_uv);
int adp20086_get_ovin_code(struct adp20086_dev *dev, uint8_t *code);
int adp20086_get_ovin_uv(struct adp20086_dev *dev, uint32_t *ovin_uv);

/* input under-voltage threshold */
int adp20086_set_uvin(struct adp20086_dev *dev, uint32_t uvin_uv);
int adp20086_get_uvin_code(struct adp20086_dev *dev, uint8_t *code);
int adp20086_get_uvin_uv(struct adp20086_dev *dev, uint32_t *uvin_uv);

/* open-load current threshold */
int adp20086_set_iopen(struct adp20086_dev *dev, enum adp20086_channel ch, uint32_t iopen_ua);
int adp20086_get_iopen_code(struct adp20086_dev *dev, enum adp20086_channel ch, uint8_t *code);
int adp20086_get_iopen_ua(struct adp20086_dev *dev, enum adp20086_channel ch, uint32_t *iopen_ua);

/* output under-voltage threshold */
int adp20086_set_uvout(struct adp20086_dev *dev, enum adp20086_channel ch, uint32_t uvout_uv);
int adp20086_get_uvout_code(struct adp20086_dev *dev, enum adp20086_channel ch, uint8_t *code);
int adp20086_get_uvout_uv(struct adp20086_dev *dev, enum adp20086_channel ch, uint32_t *uvout_uv);

/* soft-start / soft-shutdown ramps (per channel) */
int adp20086_set_soft_start(struct adp20086_dev *dev, enum adp20086_channel ch, enum adp20086_soft_start ssu);
int adp20086_get_soft_start(struct adp20086_dev *dev, enum adp20086_channel ch, enum adp20086_soft_start *ssu);
int adp20086_set_soft_shutdown(struct adp20086_dev *dev, enum adp20086_channel ch, enum adp20086_soft_shutdown ssd);
int adp20086_get_soft_shutdown(struct adp20086_dev *dev, enum adp20086_channel ch, enum adp20086_soft_shutdown *ssd);

/* ADC telemetry */
int adp20086_read_vin_code(struct adp20086_dev *dev, uint8_t *code);
int adp20086_read_vin_uv(struct adp20086_dev *dev, uint32_t *vin_uv);
int adp20086_read_vout_code(struct adp20086_dev *dev, enum adp20086_channel ch, uint8_t *code);
int adp20086_read_vout_uv(struct adp20086_dev *dev, enum adp20086_channel ch, uint32_t *vout_uv);
int adp20086_read_iout_code(struct adp20086_dev *dev, enum adp20086_channel ch, uint8_t *code);
int adp20086_read_iout_ua(struct adp20086_dev *dev, enum adp20086_channel ch, uint32_t *iout_ua);
int adp20086_read_vdd_code(struct adp20086_dev *dev, uint8_t *code);
int adp20086_read_vdd_uv(struct adp20086_dev *dev, uint32_t *vdd_uv);


#endif	/** __ADP20086_H__ */
