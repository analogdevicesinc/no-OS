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
#include "no_os_gpio.h"
#include "no_os_irq.h"

#define ADP20086_REG_MASK					0x00
#define ADP20086_REG_CONFIG					0x01
#define ADP20086_REG_ID						0x02
#define ADP20086_REG_STAT1					0x03
#define ADP20086_REG_STAT2					0x04
#define ADP20086_REG_STAT3					0x05
#define ADP20086_REG_ADC_LEGACY(n)			(0x06 + (n))
#define ADP20086_REG_MASK2					0x0A
#define ADP20086_REG_MASK3					0x0B

/* 0x0C for ILIM12, 0x0D for ILIM34*/
#define ADP20086_REG_ILIM(ch)				(0x0C + ((ch) >> 1))
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
#define ADP20086_MASK_INTMASKS				NO_OS_GENMASK(6, 0)

#define ADP20086_MASK2_UV4M					NO_OS_BIT(7)
#define ADP20086_MASK2_UV3M					NO_OS_BIT(6)
#define ADP20086_MASK2_UV2M					NO_OS_BIT(5)
#define ADP20086_MASK2_UV1M					NO_OS_BIT(4)
#define ADP20086_MASK2_UVM(ch)				(NO_OS_BIT((ch) + 4))
#define ADP20086_MASK2_OPNM4				NO_OS_BIT(3)
#define ADP20086_MASK2_OPNM3				NO_OS_BIT(2)
#define ADP20086_MASK2_OPNM2				NO_OS_BIT(1)
#define ADP20086_MASK2_OPNM1				NO_OS_BIT(0)
#define ADP20086_MASK2_OPNM(ch)				(NO_OS_BIT((ch)))

#define ADP20086_MASK3_FCALM				NO_OS_BIT(6)
#define ADP20086_MASK3_IIR_EN				NO_OS_BIT(5)
#define ADP20086_MASK3_DISCH_EN				NO_OS_BIT(4)
#define ADP20086_MASK3_BISTM				NO_OS_BIT(3)
#define ADP20086_MASK3_PARERRM				NO_OS_BIT(2)
#define ADP20086_MASK3_ECC_CORM				NO_OS_BIT(1)
#define ADP20086_MASK3_ECC_DETM				NO_OS_BIT(0)
#define ADP20086_MASK3_INTMASKS				(NO_OS_BIT(6) | NO_OS_GENMASK(3, 0))

#define ADP20086_REG_MASK_DEFAULT			0x40
#define ADP20086_REG_MASK2_DEFAULT			0x00
#define ADP20086_REG_MASK3_DEFAULT			0x00

#define ADP20086_CONFIG_MUX					NO_OS_GENMASK(7, 6)
#define ADP20086_CONFIG_ENC					NO_OS_BIT(5)
#define ADP20086_CONFIG_CLR					NO_OS_BIT(4)
#define ADP20086_CONFIG_EN(ch)				(NO_OS_BIT((ch)))
#define ADP20086_CONFIG_EN_MASK				NO_OS_GENMASK(3, 0)
#define ADP20086_ALL_CH_ASSERTED			0xF

#define ADP20086_ID_PECE					NO_OS_BIT(6)
#define ADP20086_ID_ID						NO_OS_GENMASK(5, 4)
#define ADP20086_ID_REV						NO_OS_GENMASK(3, 0)

/* ILIM1, ILIM3 mask (7:4). ILIM2, ILIM4 mask (3:0).*/
#define ADP20086_ILIM_MASK(ch)				(((ch) & 1) ? NO_OS_GENMASK(3, 0) : NO_OS_GENMASK(7,4))

#define ADP20086_START_SLEW_MASK(ch)		(NO_OS_GENMASK((ch) * 2 + 1, (ch) * 2))
#define ADP20086_SHDN_SLEW_MASK(ch)			(NO_OS_GENMASK((ch) * 2 + 1, (ch) * 2))

/* STAT2/STAT3 are two channels per byte: low nibble = lower ch, high = upper ch.
 * Per-channel bit within a nibble: UV=bit0, OV=bit1, OC=bit2, TS=bit3. */
#define ADP20086_CHAN_STATUS_TS(ch)			(NO_OS_BIT(3) << (4 * ((ch) & 1)))
#define ADP20086_CHAN_STATUS_OC(ch)			(NO_OS_BIT(2) << (4 * ((ch) & 1)))
#define ADP20086_CHAN_STATUS_OV(ch)			(NO_OS_BIT(1) << (4 * ((ch) & 1)))
#define ADP20086_CHAN_STATUS_UV(ch)			(NO_OS_BIT(0) << (4 * ((ch) & 1)))
#define ADP20086_CHAN_STATUS_LOAD_DET(ch)	(NO_OS_BIT((ch)) << 4)
#define ADP20086_CHAN_STATUS_OPEN(ch)		(NO_OS_BIT((ch)))

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
#define ADP20086_LDET_EN(ch)				(NO_OS_BIT((ch) + 4))
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

#define ADP20086_CHANNEL_COUNT				4

/**
 * @brief Output channel index, used as the per-channel array index throughout
 *	  the driver.
 */
enum adp20086_channel {
	ADP20086_CHANNEL1,
	ADP20086_CHANNEL2,
	ADP20086_CHANNEL3,
	ADP20086_CHANNEL4,
};

/**
 * @brief Selectable output current limits, in 52 mA steps.
 */
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

/**
 * @brief Selectable soft-start ramp times (SS_RAMPn).
 */
enum adp20086_soft_startup {
	ADP20086_SSU_0R5MS, ADP20086_SSU_1MS, ADP20086_SSU_2MS, ADP20086_SSU_4MS,
};

/**
 * @brief Selectable soft-shutdown ramp times (SSD_RAMPn).
 */
enum adp20086_soft_shutdown {
	ADP20086_SSD_0R25MS, ADP20086_SSD_0R5MS, ADP20086_SSD_1MS, ADP20086_SSD_2MS,
};

/**
 * @brief Interrupt sources that can be masked from asserting INTB.
 */
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

/**
 * @brief Individual status flags across the five STAT registers.
 */
enum adp20086_status_field {
	ADP20086_STATUS_TLIM_SERV,
	ADP20086_STATUS_ACC,
	ADP20086_STATUS_OVIN,
	ADP20086_STATUS_UVIN,
	ADP20086_STATUS_OVDD,
	ADP20086_STATUS_UVDD,
	ADP20086_STATUS_TSOUT1,
	ADP20086_STATUS_TSOUT2,
	ADP20086_STATUS_TSOUT3,
	ADP20086_STATUS_TSOUT4,
	ADP20086_STATUS_OCOUT1,
	ADP20086_STATUS_OCOUT2,
	ADP20086_STATUS_OCOUT3,
	ADP20086_STATUS_OCOUT4,
	ADP20086_STATUS_OVOUT1,
	ADP20086_STATUS_OVOUT2,
	ADP20086_STATUS_OVOUT3,
	ADP20086_STATUS_OVOUT4,
	ADP20086_STATUS_UVOUT1,
	ADP20086_STATUS_UVOUT2,
	ADP20086_STATUS_UVOUT3,
	ADP20086_STATUS_UVOUT4,
	ADP20086_STATUS_LDET1,
	ADP20086_STATUS_LDET2,
	ADP20086_STATUS_LDET3,
	ADP20086_STATUS_LDET4,
	ADP20086_STATUS_OPEN1,
	ADP20086_STATUS_OPEN2,
	ADP20086_STATUS_OPEN3,
	ADP20086_STATUS_OPEN4,
	ADP20086_STATUS_ADCFCAL,
	ADP20086_STATUS_INTBSTS,
	ADP20086_STATUS_BIST,
	ADP20086_STATUS_PARERR,
	ADP20086_STATUS_ECC_COR,
	ADP20086_STATUS_ECC_DET,
};

/**
 * @brief Source routed to the legacy MAX20087-compatible ADC registers.
 */
enum adp20086_legacy_adc_mux_config {
	ADP20086_LEGACY_MUX_IOUT,
	ADP20086_LEGACY_MUX_VOUT,
	ADP20086_LEGACY_MUX_VIN_VDD,
};

/**
 * @brief Per-output enable state of a channel-wide feature.
 */
struct adp20086_enabled_channels {
	bool out1;
	bool out2;
	bool out3;
	bool out4;
};

/**
 * @brief Snapshot of every device and per-output status flag.
 */
struct adp20086_status {
	bool tlim_serv;
	bool acc;
	bool ovin;
	bool uvin;
	bool ovdd;
	bool uvdd;
	bool thermal_shutdown[ADP20086_CHANNEL_COUNT];
	bool overcurrent[ADP20086_CHANNEL_COUNT];
	bool ovout[ADP20086_CHANNEL_COUNT];
	bool uvout[ADP20086_CHANNEL_COUNT];
	bool load_detected[ADP20086_CHANNEL_COUNT];
	bool open_load[ADP20086_CHANNEL_COUNT];
	bool adc_fcal;
	bool intbsts;
	bool bist;
	bool parerr;
	bool ecc_cor;
	bool ecc_det;
};

/**
 * @brief Interrupt mask bits. A set bit keeps the source from asserting INTB;
 *	  the matching status bit still latches.
 */
struct adp20086_masks {
	bool accm;
	bool tsm;
	bool vddm;
	bool vinm;
	bool ocm;
	bool ovm;
	bool uvm;
	bool uvoutm[ADP20086_CHANNEL_COUNT];
	bool opnoutm[ADP20086_CHANNEL_COUNT];
	bool fcalm;
	bool bistm;
	bool parerrm;
	bool ecc_corm;
	bool ecc_detm;
};

/**
 * @brief The four legacy MAX20087-compatible ADC registers.
 */
enum adp20086_legacy_adc {
	ADP20086_ADC1_LEGACY,
	ADP20086_ADC2_LEGACY,
	ADP20086_ADC3_LEGACY,
	ADP20086_ADC4_LEGACY,
};

struct adp20086_dev;

/**
 * @brief Callback run by adp20086_process_interrupt() after an INTB assertion.
 * @param dev - The ADP20086 device descriptor.
 * @param status - Status snapshot read after the assertion.
 * @param ctx - User context given to adp20086_set_interrupt_callback().
 */
typedef void (*adp20086_int_cb_t)(struct adp20086_dev *dev,
				  const struct adp20086_status *status,
				  void *ctx);

/**
 * @brief ADP20086 device descriptor.
 */
struct adp20086_dev {
	struct no_os_i2c_desc *i2c_desc;
	struct no_os_gpio_desc *en_gpio_desc;
	struct no_os_gpio_desc *intb_gpio_desc;
	struct no_os_irq_ctrl_desc *irq_ctrl;
	struct no_os_callback_desc irq_cb;
	volatile bool int_pending;
	bool int_armed;		/**< INTB armed by adp20086_arm_interrupt() */
	adp20086_int_cb_t int_cb;
	void *int_cb_ctx;
	bool pece;		/**< PEC enable, auto-detected from ID.PECE */
	uint8_t part_id;	/**< ID[5:4] read back at init */
	uint8_t revision;	/**< Rev[3:0] read back at init */
};

/**
 * @brief ADP20086 device initialization parameter.
 */
struct adp20086_init_param {
	struct no_os_i2c_init_param *i2c_ip;
	struct no_os_gpio_init_param *en_gpio_ip;
	struct no_os_gpio_init_param *intb_gpio_ip;
	struct no_os_irq_ctrl_desc *irq_ctrl;
};

int adp20086_init(struct adp20086_dev **dev,
		  struct adp20086_init_param *adp20086_ip);

int adp20086_remove(struct adp20086_dev *dev);

int adp20086_set_gpio_en(struct adp20086_dev *dev, bool enabled);

int adp20086_get_gpio_en(struct adp20086_dev *dev, bool *enabled);

int adp20086_get_intb(struct adp20086_dev *dev, bool *asserted);

int adp20086_set_interrupt_callback(struct adp20086_dev *dev,
				    adp20086_int_cb_t cb, void *ctx);

int adp20086_arm_interrupt(struct adp20086_dev *dev);

int adp20086_disarm_interrupt(struct adp20086_dev *dev);

int adp20086_get_interrupt_armed(struct adp20086_dev *dev, bool *armed);

int adp20086_process_interrupt(struct adp20086_dev *dev);

int adp20086_read(struct adp20086_dev *dev, uint8_t reg, uint8_t *data);

int adp20086_write(struct adp20086_dev *dev, uint8_t reg, uint8_t data);

int adp20086_update_register(struct adp20086_dev *dev, uint8_t reg,
			     uint8_t mask, uint8_t data);

int adp20086_get_register_field(struct adp20086_dev *dev, uint8_t reg,
				uint8_t mask, uint8_t *data);

int adp20086_set_mux_config(struct adp20086_dev *dev,
			    enum adp20086_legacy_adc_mux_config config);

int adp20086_set_enc_config(struct adp20086_dev *dev, bool enabled);

int adp20086_set_clr_config(struct adp20086_dev *dev, bool enabled);

int adp20086_get_mux_config(struct adp20086_dev *dev,
			    enum adp20086_legacy_adc_mux_config *config);

int adp20086_get_enc_config(struct adp20086_dev *dev, bool *enabled);

int adp20086_get_clr_config(struct adp20086_dev *dev, bool *enabled);

int adp20086_set_channel(struct adp20086_dev *dev, enum adp20086_channel ch,
			 bool enabled);

int adp20086_set_all_channels(struct adp20086_dev *dev, bool enabled);

int adp20086_get_enabled_channels(struct adp20086_dev *dev,
				  struct adp20086_enabled_channels *enabled_channels);

int adp20086_set_load_detection(struct adp20086_dev *dev,
				enum adp20086_channel ch, bool enabled);

int adp20086_set_all_load_detection(struct adp20086_dev *dev, bool enabled);

int adp20086_get_enabled_load_detection(struct adp20086_dev *dev,
					struct adp20086_enabled_channels *enabled_channels);

int adp20086_set_open_protection(struct adp20086_dev *dev,
				 enum adp20086_channel ch, bool enabled);

int adp20086_set_all_open_protection(struct adp20086_dev *dev, bool enabled);

int adp20086_get_enabled_open_protection(struct adp20086_dev *dev,
		struct adp20086_enabled_channels *enabled_channels);

int adp20086_get_pece(struct adp20086_dev *dev, bool *pec_enabled);

int adp20086_get_part_id(struct adp20086_dev *dev, uint8_t *part_id);

int adp20086_get_revision(struct adp20086_dev *dev, uint8_t *revision);

int adp20086_get_ovtst(struct adp20086_dev *dev, bool *enabled);

int adp20086_set_ovtst(struct adp20086_dev *dev, bool enabled);

int adp20086_get_iir_filter(struct adp20086_dev *dev, bool *enabled);

int adp20086_set_iir_filter(struct adp20086_dev *dev, bool enabled);

int adp20086_get_discharge_resistors(struct adp20086_dev *dev, bool *enabled);

int adp20086_set_discharge_resistors(struct adp20086_dev *dev, bool enabled);

int adp20086_get_interrupt_mask(struct adp20086_dev *dev,
				enum adp20086_interrupt_source src, bool *masked);

int adp20086_set_interrupt_mask(struct adp20086_dev *dev,
				enum adp20086_interrupt_source src, bool masked);

struct adp20086_masks adp20086_get_default_mask(void);

int adp20086_get_configured_interrupt_masks(struct adp20086_dev *dev,
		struct adp20086_masks *mask);

int adp20086_configure_interrupt_masks(struct adp20086_dev *dev,
				       struct adp20086_masks mask);

int adp20086_reset_masks(struct adp20086_dev *dev);

int adp20086_get_status(struct adp20086_dev *dev,
			struct adp20086_status *status);

int adp20086_get_status_field(struct adp20086_dev *dev,
			      enum adp20086_status_field field, bool *asserted);

int adp20086_clear_status(struct adp20086_dev *dev);

int adp20086_check_self_test(struct adp20086_dev *dev);

/* legacy MAX20087-compatible ADC mux read */
int adp20086_read_legacy_adc(struct adp20086_dev *dev,
			     enum adp20086_legacy_adc adc, uint8_t *data);

int adp20086_set_ilim(struct adp20086_dev *dev, enum adp20086_channel ch,
		      enum adp20086_ilim_available ilim);

int adp20086_set_all_ilim(struct adp20086_dev *dev,
			  enum adp20086_ilim_available ilim);

int adp20086_get_ilim_code(struct adp20086_dev *dev, enum adp20086_channel ch,
			   uint8_t *code);

int adp20086_get_ilim(struct adp20086_dev *dev, enum adp20086_channel ch,
		      uint32_t *ilim_ua);

int adp20086_get_ldet_threshold_code(struct adp20086_dev *dev,
				     enum adp20086_channel ch, uint8_t *code);

int adp20086_get_ldet_threshold(struct adp20086_dev *dev,
				enum adp20086_channel ch, uint32_t *threshold_ua);

int adp20086_set_ldet_threshold(struct adp20086_dev *dev,
				enum adp20086_channel ch, uint32_t threshold_ua);

/* input over-voltage threshold */
int adp20086_get_ovin_code(struct adp20086_dev *dev, uint8_t *code);

int adp20086_get_ovin(struct adp20086_dev *dev, uint32_t *ovin_uv);

int adp20086_set_ovin(struct adp20086_dev *dev, uint32_t ovin_uv);

/* input under-voltage threshold */
int adp20086_get_uvin_code(struct adp20086_dev *dev, uint8_t *code);

int adp20086_get_uvin(struct adp20086_dev *dev, uint32_t *uvin_uv);

int adp20086_set_uvin(struct adp20086_dev *dev, uint32_t uvin_uv);

/* open-load current threshold */
int adp20086_get_iopen_code(struct adp20086_dev *dev, enum adp20086_channel ch,
			    uint8_t *code);

int adp20086_get_iopen(struct adp20086_dev *dev, enum adp20086_channel ch,
		       uint32_t *iopen_ua);

int adp20086_set_iopen(struct adp20086_dev *dev, enum adp20086_channel ch,
		       uint32_t iopen_ua);

/* output under-voltage threshold */
int adp20086_get_uvout_code(struct adp20086_dev *dev, enum adp20086_channel ch,
			    uint8_t *code);

int adp20086_get_uvout(struct adp20086_dev *dev, enum adp20086_channel ch,
		       uint32_t *uvout_uv);

int adp20086_set_uvout(struct adp20086_dev *dev, enum adp20086_channel ch,
		       uint32_t uvout_uv);

/* soft-start / soft-shutdown ramps (per channel) */
int adp20086_get_soft_startup(struct adp20086_dev *dev,
			      enum adp20086_channel ch,
			      enum adp20086_soft_startup *ssu);

int adp20086_set_soft_startup(struct adp20086_dev *dev,
			      enum adp20086_channel ch,
			      enum adp20086_soft_startup ssu);

int adp20086_set_all_soft_startup(struct adp20086_dev *dev,
				  enum adp20086_soft_startup ssu);

int adp20086_get_soft_shutdown(struct adp20086_dev *dev,
			       enum adp20086_channel ch, enum adp20086_soft_shutdown *ssd);

int adp20086_set_soft_shutdown(struct adp20086_dev *dev,
			       enum adp20086_channel ch, enum adp20086_soft_shutdown ssd);

int adp20086_set_all_soft_shutdown(struct adp20086_dev *dev,
				   enum adp20086_soft_shutdown ssd);

int adp20086_read_vin_code(struct adp20086_dev *dev, uint8_t *code);

int adp20086_read_vin(struct adp20086_dev *dev, uint32_t *vin_uv);

int adp20086_read_vout_code(struct adp20086_dev *dev, enum adp20086_channel ch,
			    uint8_t *code);

int adp20086_read_vout(struct adp20086_dev *dev, enum adp20086_channel ch,
		       uint32_t *vout_uv);

int adp20086_read_iout_code(struct adp20086_dev *dev, enum adp20086_channel ch,
			    uint8_t *code);

int adp20086_read_iout(struct adp20086_dev *dev, enum adp20086_channel ch,
		       uint32_t *iout_ua);

int adp20086_read_vdd_code(struct adp20086_dev *dev, uint8_t *code);

int adp20086_read_vdd(struct adp20086_dev *dev, uint32_t *vdd_uv);

#endif	/* __ADP20086_H__ */
