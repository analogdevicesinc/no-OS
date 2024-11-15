/***************************************************************************//**
 *   @file   iio_lt8722.c
 *   @brief  Source file for the LT8722 IIO Driver
 *   @author Ramon Cristopher Calam (ramoncristopher.calam@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_units.h"
#include "no_os_util.h"
#include "lt8722.h"
#include "iio_lt8722.h"

/**
 * @struct lt8722_iio_attr_avail
 * @brief Structure holding the available values for an attribute.
 */
struct lt8722_iio_attr_avail {
	int num_values;
	const char *const *values;
	const char *const *alt_values;
};

static const char *const lt8722_pin_avail[] = {
	"Low",
	"High",
};

static const char *const lt8722_enable_avail[] = {
	"Disabled",
	"Enabled",
};

static const char *const lt8722_sw_frq_set_avail[] = {
	"0.5MHz",
	"1MHz",
	"1.5MHz",
	"2MHz",
	"2.5MHz",
	"3MHz",
};

static const char *const lt8722_sw_frq_adj_avail[] = {
	"0%",
	"+15%",
	"-15%",
	"0%",
};

static const char *const lt8722_sys_dc_avail[] = {
	"20%-80%",
	"15%-85%",
	"10%-90%",
};

static const char *const lt8722_vcc_vreg_avail[] = {
	"3.1V",
	"3.4V",
};

static const char *const lt8722_sw_vc_int_avail[] = {
	"0.252A",
	"0.594A",
	"0.936A",
	"1.278A",
	"1.620A",
	"1.962A",
	"2.304A",
	"2.646A",
};

static const char *const lt8722_spi_rst_avail[] = {
	"Normal",
	"Reset",
};

static const char *const lt8722_pwr_lim_avail[] = {
	"2W",
	"No_Limit",
	"3W",
	"3.5W",
};

static const char *const lt8722_active_avail[] = {
	"Normal",
	"Active",
};

static const char *const lt8722_reset_avail[] = {
	"No_Reset",
	"Reset",
};

static const char *const lt8722_fault_avail[] = {
	"No_Fault",
	"Fault",
};

static const char *const lt8722_amux_avail[] = {
	"V_ILIMP",
	"V_ILIMN",
	"V_1P25-0.8*V_DAC",
	"V_1P25-V_OUT/16",
	"V_1P25-I_OUT/8",
	"0.6*V_2P5",
	"V_1P25",
	"V_1P65",
	"V_TEMP",
	"0.9*V_2P5-V_IN/8",
	"0.4*V_CC",
	"0.4*V_DDIO",
	"(16/17)*V_1P25+V_SFB/17",
};

static const char *const lt8722_amux_alt_avail[] = {
	"V_ILIMP",
	"V_ILIMN",
	"V_1P25-0.8*V_DAC",
	"V_1P25-V_OUT/16",
	"V_1P25-I_OUT/8",
	"(6/13)*V_2P5",
	"0.8*V_1P25+0.2*V_CC",
	"(2/3)*V_1P65",
	"0.855*V_TEMP",
	"0.9*V_2P5-V_IN/8",
	"(3/8)*V_CC",
	"(0.4375)*V_DDIO",
	"(16/17)*V_1P25+V_SFB/17",
};

static const char *const lt8722_amux_test_avail[] = {
	"0",
	"1",
	"2",
	"3",
};

static const char *const lt8722_ov_clamp_avail[] = {
	"1249",
	"2499",
	"3749",
	"4999",
	"6249",
	"7499",
	"8749",
	"9999",
	"11249",
	"12499",
	"13749",
	"14999",
	"16249",
	"17499",
	"18749",
	"19999",
};

static const char *const lt8722_uv_clamp_avail[] = {
	"-20000",
	"-18750",
	"-17500",
	"-16250",
	"-15000",
	"-13750",
	"-12500",
	"-11250",
	"-10000",
	"-8750",
	"-7500",
	"-6250",
	"-5000",
	"-3750",
	"-2500",
	"-1250",
};

enum lt8722_iio_spis_command_attrs {
	LT8722_ENABLE_REQ,
	LT8722_SWEN_REQ,
	LT8722_SW_FRQ_SET,
	LT8722_SW_FRQ_ADJ,
	LT8722_SYS_DC,
	LT8722_VCC_VREG,
	LT8722_SW_VC_INT,
	LT8722_SPI_RST,
	LT8722_PWR_LIM,
};

enum lt8722_iio_spis_status_attrs {
	LT8722_SWEN,
	LT8722_SRVO_ILIM,
	LT8722_SRVO_PLIM,
	LT8722_MIN_OT,
	LT8722_POR_OCC,
	LT8722_OVER_CURRENT,
	LT8722_TSD,
	LT8722_VCC_UVLO,
	LT8722_VDDIO_UVLO,
	LT8722_CP_UVLO,
	LT8722_V2P5_UVLO,
};

enum lt8722_iio_spis_amux_attrs {
	LT8722_AMUX,
	LT8722_AMUX_TEST,
	LT8722_AOUT_EN,
};

enum lt8722_iio_pins {
	LT8722_EN_PIN,
	LT8722_SWEN_PIN,
};

static int lt8722_iio_reg_read(void *dev, uint32_t reg, uint32_t *readval);
static int lt8722_iio_reg_write(void *dev, uint32_t reg, uint32_t writeval);
static int lt8722_iio_read_output_voltage(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int lt8722_iio_write_output_voltage(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int lt8722_iio_read_min_voltage(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel, intptr_t priv);
static int lt8722_iio_read_min_voltage_available(void *dev, char *buf,
		uint32_t len, const struct iio_ch_info *channel, intptr_t priv);
static int lt8722_iio_write_min_voltage(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel, intptr_t priv);
static int lt8722_iio_read_max_voltage(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel, intptr_t priv);
static int lt8722_iio_read_max_voltage_available(void *dev, char *buf,
		uint32_t len, const struct iio_ch_info *channel, intptr_t priv);
static int lt8722_iio_write_max_voltage(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel, intptr_t priv);
static int lt8722_iio_read_command(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel, intptr_t priv);
static int lt8722_iio_write_command(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel, intptr_t priv);
static int lt8722_iio_read_command_available(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int lt8722_iio_read_status(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel, intptr_t priv);
static int lt8722_iio_write_status(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel, intptr_t priv);
static int lt8722_iio_read_status_available(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int lt8722_iio_write_dac(void *dev, char *buf, uint32_t len,
				const struct iio_ch_info *channel, intptr_t priv);
static int lt8722_iio_read_dac(void *dev, char *buf, uint32_t len,
			       const struct iio_ch_info *channel, intptr_t priv);
static int lt8722_iio_read_dac_ilimn(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel, intptr_t priv);
static int lt8722_iio_write_dac_ilimn(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel, intptr_t priv);
static int lt8722_iio_read_dac_ilimp(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel, intptr_t priv);
static int lt8722_iio_write_dac_ilimp(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel, intptr_t priv);
static int lt8722_iio_read_amux(void *dev, char *buf, uint32_t len,
				const struct iio_ch_info *channel, intptr_t priv);
static int lt8722_iio_write_amux(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t priv);
static int lt8722_iio_read_amux_available(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int lt8722_iio_read_pin(void *dev, char *buf, uint32_t len,
			       const struct iio_ch_info *channel, intptr_t priv);
static int lt8722_iio_write_pin(void *dev, char *buf, uint32_t len,
				const struct iio_ch_info *channel, intptr_t priv);
static int lt8722_iio_read_pin_available(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);

static struct iio_attribute lt8722_global_attrs[] = {
	{
		.name = "enable_req",
		.show = lt8722_iio_read_command,
		.store = lt8722_iio_write_command,
		.priv = LT8722_ENABLE_REQ,
	},
	{
		.name = "enable_req_available",
		.show = lt8722_iio_read_command_available,
		.priv = LT8722_ENABLE_REQ,
	},
	{
		.name = "swen_req",
		.show = lt8722_iio_read_command,
		.store = lt8722_iio_write_command,
		.priv = LT8722_SWEN_REQ,
	},
	{
		.name = "swen_req_available",
		.show = lt8722_iio_read_command_available,
		.priv = LT8722_SWEN_REQ,
	},
	{
		.name = "sw_frq_set",
		.show = lt8722_iio_read_command,
		.store = lt8722_iio_write_command,
		.priv = LT8722_SW_FRQ_SET,
	},
	{
		.name = "sw_frq_set_available",
		.show = lt8722_iio_read_command_available,
		.priv = LT8722_SW_FRQ_SET,
	},
	{
		.name = "sw_frq_adj",
		.show = lt8722_iio_read_command,
		.store = lt8722_iio_write_command,
		.priv = LT8722_SW_FRQ_ADJ,
	},
	{
		.name = "sw_frq_adj_available",
		.show = lt8722_iio_read_command_available,
		.priv = LT8722_SW_FRQ_ADJ,
	},
	{
		.name = "sys_dc",
		.show = lt8722_iio_read_command,
		.store = lt8722_iio_write_command,
		.priv = LT8722_SYS_DC,
	},
	{
		.name = "sys_dc_available",
		.show = lt8722_iio_read_command_available,
		.priv = LT8722_SYS_DC,
	},
	{
		.name = "vcc_vreg",
		.show = lt8722_iio_read_command,
		.store = lt8722_iio_write_command,
		.priv = LT8722_VCC_VREG,
	},
	{
		.name = "vcc_vreg_available",
		.show = lt8722_iio_read_command_available,
		.priv = LT8722_VCC_VREG,
	},
	{
		.name = "sw_vc_int",
		.show = lt8722_iio_read_command,
		.store = lt8722_iio_write_command,
		.priv = LT8722_SW_VC_INT,
	},
	{
		.name = "sw_vc_int_available",
		.show = lt8722_iio_read_command_available,
		.priv = LT8722_SW_VC_INT,
	},
	{
		.name = "spi_rst",
		.show = lt8722_iio_read_command,
		.store = lt8722_iio_write_command,
		.priv = LT8722_SPI_RST,
	},
	{
		.name = "spi_rst_available",
		.show = lt8722_iio_read_command_available,
		.priv = LT8722_SPI_RST,
	},
	{
		.name = "pwr_lim",
		.show = lt8722_iio_read_command,
		.store = lt8722_iio_write_command,
		.priv = LT8722_PWR_LIM,
	},
	{
		.name = "pwr_lim_available",
		.show = lt8722_iio_read_command_available,
		.priv = LT8722_PWR_LIM,
	},
	{
		.name = "spis_dac",
		.show = lt8722_iio_read_dac,
		.store = lt8722_iio_write_dac,
	},
	{
		.name = "spis_dac_ilimn",
		.show = lt8722_iio_read_dac_ilimn,
		.store = lt8722_iio_write_dac_ilimn,
	},
	{
		.name = "spis_dac_ilimp",
		.show = lt8722_iio_read_dac_ilimp,
		.store = lt8722_iio_write_dac_ilimp,
	},
	{
		.name = "amux",
		.show = lt8722_iio_read_amux,
		.store = lt8722_iio_write_amux,
		.priv = LT8722_AMUX,
	},
	{
		.name = "amux_available",
		.show = lt8722_iio_read_amux_available,
		.priv = LT8722_AMUX,
	},
	{
		.name = "amux_test",
		.show = lt8722_iio_read_amux,
		.store = lt8722_iio_write_amux,
		.priv = LT8722_AMUX_TEST,
	},
	{
		.name = "amux_test_available",
		.show = lt8722_iio_read_amux_available,
		.priv = LT8722_AMUX_TEST,
	},
	{
		.name = "aout_en",
		.show = lt8722_iio_read_amux,
		.store = lt8722_iio_write_amux,
		.priv = LT8722_AOUT_EN,
	},
	{
		.name = "aout_en_available",
		.show = lt8722_iio_read_amux_available,
		.priv = LT8722_AOUT_EN,
	},
	{
		.name = "en_pin",
		.show = lt8722_iio_read_pin,
		.store = lt8722_iio_write_pin,
		.priv = LT8722_EN_PIN,
	},
	{
		.name = "en_pin_available",
		.show = lt8722_iio_read_pin_available,
		.priv = LT8722_EN_PIN,
	},
	{
		.name = "swen_pin",
		.show = lt8722_iio_read_pin,
		.store = lt8722_iio_write_pin,
		.priv = LT8722_SWEN_PIN,
	},
	{
		.name = "swen_pin_available",
		.show = lt8722_iio_read_pin_available,
		.priv = LT8722_SWEN_PIN,
	},
	END_ATTRIBUTES_ARRAY,
};

static struct iio_attribute lt8722_debug_attrs[] = {
	{
		.name = "swen",
		.show = lt8722_iio_read_status,
		.store = lt8722_iio_write_status,
		.priv = LT8722_SWEN,
	},
	{
		.name = "swen_available",
		.show = lt8722_iio_read_status_available,
		.priv = LT8722_SWEN,
	},
	{
		.name = "srvo_ilim",
		.show = lt8722_iio_read_status,
		.store = lt8722_iio_write_status,
		.priv = LT8722_SRVO_ILIM,
	},
	{
		.name = "srvo_ilim_available",
		.show = lt8722_iio_read_status_available,
		.priv = LT8722_SRVO_ILIM,
	},
	{
		.name = "srvo_plim",
		.show = lt8722_iio_read_status,
		.store = lt8722_iio_write_status,
		.priv = LT8722_SRVO_PLIM,
	},
	{
		.name = "srvo_plim_available",
		.show = lt8722_iio_read_status_available,
		.priv = LT8722_SRVO_PLIM,
	},
	{
		.name = "min_ot",
		.show = lt8722_iio_read_status,
		.store = lt8722_iio_write_status,
		.priv = LT8722_MIN_OT,
	},
	{
		.name = "min_ot_available",
		.show = lt8722_iio_read_status_available,
		.priv = LT8722_MIN_OT,
	},
	{
		.name = "por_occ",
		.show = lt8722_iio_read_status,
		.store = lt8722_iio_write_status,
		.priv = LT8722_POR_OCC,
	},
	{
		.name = "por_occ_available",
		.show = lt8722_iio_read_status_available,
		.priv = LT8722_POR_OCC,
	},
	{
		.name = "over_current",
		.show = lt8722_iio_read_status,
		.store = lt8722_iio_write_status,
		.priv = LT8722_OVER_CURRENT,
	},
	{
		.name = "over_current_available",
		.show = lt8722_iio_read_status_available,
		.priv = LT8722_OVER_CURRENT,
	},
	{
		.name = "tsd",
		.show = lt8722_iio_read_status,
		.store = lt8722_iio_write_status,
		.priv = LT8722_TSD,
	},
	{
		.name = "tsd_available",
		.show = lt8722_iio_read_status_available,
		.priv = LT8722_TSD,
	},
	{
		.name = "vcc_uvlo",
		.show = lt8722_iio_read_status,
		.store = lt8722_iio_write_status,
		.priv = LT8722_VCC_UVLO,
	},
	{
		.name = "vcc_uvlo_available",
		.show = lt8722_iio_read_status_available,
		.priv = LT8722_VCC_UVLO,
	},
	{
		.name = "vddio_uvlo",
		.show = lt8722_iio_read_status,
		.store = lt8722_iio_write_status,
		.priv = LT8722_VDDIO_UVLO,
	},
	{
		.name = "vddio_uvlo_available",
		.show = lt8722_iio_read_status_available,
		.priv = LT8722_VDDIO_UVLO,
	},
	{
		.name = "cp_uvlo",
		.show = lt8722_iio_read_status,
		.store = lt8722_iio_write_status,
		.priv = LT8722_CP_UVLO,
	},
	{
		.name = "cp_uvlo_available",
		.show = lt8722_iio_read_status_available,
		.priv = LT8722_CP_UVLO,
	},
	{
		.name = "v2p5_uvlo",
		.show = lt8722_iio_read_status,
		.store = lt8722_iio_write_status,
		.priv = LT8722_V2P5_UVLO,
	},
	{
		.name = "v2p5_uvlo_available",
		.show = lt8722_iio_read_status_available,
		.priv = LT8722_V2P5_UVLO,
	},
	END_ATTRIBUTES_ARRAY,
};

static struct iio_attribute lt8722_channel_attrs[] = {
	{
		.name = "output",
		.show = lt8722_iio_read_output_voltage,
		.store = lt8722_iio_write_output_voltage,
	},
	{
		.name = "min",
		.show = lt8722_iio_read_min_voltage,
		.store = lt8722_iio_write_min_voltage,
	},
	{
		.name = "min_available",
		.show = lt8722_iio_read_min_voltage_available,
	},
	{
		.name = "max",
		.show = lt8722_iio_read_max_voltage,
		.store = lt8722_iio_write_max_voltage,
	},
	{
		.name = "max_available",
		.show = lt8722_iio_read_max_voltage_available,
	},
	END_ATTRIBUTES_ARRAY,
};

static struct iio_channel const lt8722_channels[] = {
	{
		.name = "voltage_0",
		.attributes = lt8722_channel_attrs,
		.ch_out = true,
		.scan_type = NULL,
		.indexed = true,
		.channel = 0,
		.address = 0,
		.ch_type = IIO_VOLTAGE,
	},
};

static struct iio_device lt8722_iio_dev = {
	.num_ch = NO_OS_ARRAY_SIZE(lt8722_channels),
	.channels = lt8722_channels,
	.attributes = lt8722_global_attrs,
	.debug_attributes = lt8722_debug_attrs,
	.debug_reg_read = (int32_t (*)())lt8722_iio_reg_read,
	.debug_reg_write = (int32_t (*)())lt8722_iio_reg_write,
};

/**
 * @brief Get the mask for the SPIS command register.
 * @param attr - The attribute for which the mask is needed.
 * @return The mask for the SPIS command register.
 */
uint32_t lt8722_iio_get_mask_command(enum lt8722_iio_spis_command_attrs attr)
{
	switch (attr) {
	case LT8722_ENABLE_REQ:
		return LT8722_ENABLE_REQ_MASK;
	case LT8722_SWEN_REQ:
		return LT8722_SWEN_REQ_MASK;
	case LT8722_SW_FRQ_SET:
		return LT8722_SW_FRQ_SET_MASK;
	case LT8722_SW_FRQ_ADJ:
		return LT8722_SW_FRQ_ADJ_MASK;
	case LT8722_SYS_DC:
		return LT8722_SYS_DC_MASK;
	case LT8722_VCC_VREG:
		return LT8722_VCC_VREG_MASK;
	case LT8722_SW_VC_INT:
		return LT8722_SW_VC_INT_MASK;
	case LT8722_SPI_RST:
		return LT8722_SPI_RST_MASK;
	case LT8722_PWR_LIM:
		return LT8722_PWR_LIM_MASK;
	default:
		return LT8722_SPIS_COMMAND_MASK;
	}
}

/**
 * @brief Get the mask for the SPIS status register.
 * @param attr - The attribute for which the mask is needed.
 * @return The mask for the SPIS status register.
 */
uint32_t lt8722_iio_get_mask_status(enum lt8722_iio_spis_status_attrs attr)
{
	switch (attr) {
	case LT8722_SWEN:
		return LT8722_SWEN_MASK;
	case LT8722_SRVO_ILIM:
		return LT8722_SRVO_ILIM_MASK;
	case LT8722_SRVO_PLIM:
		return LT8722_SRVO_PLIM_MASK;
	case LT8722_MIN_OT:
		return LT8722_MIN_OT_MASK;
	case LT8722_POR_OCC:
		return LT8722_POR_OCC_MASK;
	case LT8722_OVER_CURRENT:
		return LT8722_OVER_CURRENT_MASK;
	case LT8722_TSD:
		return LT8722_TSD_MASK;
	case LT8722_VCC_UVLO:
		return LT8722_VCC_UVLO_MASK;
	case LT8722_VDDIO_UVLO:
		return LT8722_VDDIO_UVLO_MASK;
	case LT8722_CP_UVLO:
		return LT8722_CP_UVLO_MASK;
	case LT8722_V2P5_UVLO:
		return LT8722_V2P5_UVLO_MASK;
	default:
		return LT8722_SPIS_STATUS_MASK;
	}
}

/**
 * @brief Get the mask for the SPIS AMUX register.
 * @param attr - The attribute for which the mask is needed.
 * @return The mask for the SPIS AMUX register.
 */
uint32_t lt8722_iio_get_mask_amux(enum lt8722_iio_spis_amux_attrs attr)
{
	switch (attr) {
	case LT8722_AMUX:
		return LT8722_AMUX_MASK;
	case LT8722_AMUX_TEST:
		return LT8722_AMUX_TEST_MASK;
	case LT8722_AOUT_EN:
		return LT8722_AOUT_EN_MASK;
	default:
		return LT8722_SPIS_AMUX_MASK;
	}
}

/**
 * @brief Get the list of available values of a SPIS_COMMAND register attribute.
 * @param attr - The attribute for which the list is needed.
 * @return The list of available values of the attribute.
 */

struct lt8722_iio_attr_avail lt8722_iio_get_command_available(
	enum lt8722_iio_spis_command_attrs attr)
{
	struct lt8722_iio_attr_avail avail;

	switch (attr) {
	case LT8722_ENABLE_REQ:
		avail.num_values = NO_OS_ARRAY_SIZE(lt8722_enable_avail);
		avail.values = lt8722_enable_avail;
		break;
	case LT8722_SWEN_REQ:
		avail.num_values = NO_OS_ARRAY_SIZE(lt8722_enable_avail);
		avail.values = lt8722_enable_avail;
		break;
	case LT8722_SW_FRQ_SET:
		avail.num_values = NO_OS_ARRAY_SIZE(lt8722_sw_frq_set_avail);
		avail.values = lt8722_sw_frq_set_avail;
		break;
	case LT8722_SW_FRQ_ADJ:
		avail.num_values = NO_OS_ARRAY_SIZE(lt8722_sw_frq_adj_avail);
		avail.values = lt8722_sw_frq_adj_avail;
		break;
	case LT8722_SYS_DC:
		avail.num_values = NO_OS_ARRAY_SIZE(lt8722_sys_dc_avail);
		avail.values = lt8722_sys_dc_avail;
		break;
	case LT8722_VCC_VREG:
		avail.num_values = NO_OS_ARRAY_SIZE(lt8722_vcc_vreg_avail);
		avail.values = lt8722_vcc_vreg_avail;
		break;
	case LT8722_SW_VC_INT:
		avail.num_values = NO_OS_ARRAY_SIZE(lt8722_sw_vc_int_avail);
		avail.values = lt8722_sw_vc_int_avail;
		break;
	case LT8722_SPI_RST:
		avail.num_values = NO_OS_ARRAY_SIZE(lt8722_spi_rst_avail);
		avail.values = lt8722_spi_rst_avail;
		break;
	case LT8722_PWR_LIM:
		avail.num_values = NO_OS_ARRAY_SIZE(lt8722_pwr_lim_avail);
		avail.values = lt8722_pwr_lim_avail;
		break;
	}

	return avail;
}

/**
 * @brief Get the list of available values of a SPIS_STATUS register attribute.
 * @param attr - The attribute for which the list is needed.
 * @return The list of available values of the attribute.
 */
struct lt8722_iio_attr_avail lt8722_iio_get_status_attr_available(
	enum lt8722_iio_spis_status_attrs attr)
{
	struct lt8722_iio_attr_avail avail;

	switch (attr) {
	case LT8722_SWEN:
		avail.num_values = NO_OS_ARRAY_SIZE(lt8722_enable_avail);
		avail.values = lt8722_enable_avail;
		break;
	case LT8722_SRVO_ILIM:
		avail.num_values = NO_OS_ARRAY_SIZE(lt8722_active_avail);
		avail.values = lt8722_active_avail;
		break;
	case LT8722_SRVO_PLIM:
		avail.num_values = NO_OS_ARRAY_SIZE(lt8722_active_avail);
		avail.values = lt8722_active_avail;
		break;
	case LT8722_MIN_OT:
		avail.num_values = NO_OS_ARRAY_SIZE(lt8722_active_avail);
		avail.values = lt8722_active_avail;
		break;
	case LT8722_POR_OCC:
		avail.num_values = NO_OS_ARRAY_SIZE(lt8722_reset_avail);
		avail.values = lt8722_reset_avail;
		break;
	case LT8722_OVER_CURRENT:
		avail.num_values = NO_OS_ARRAY_SIZE(lt8722_fault_avail);
		avail.values = lt8722_fault_avail;
		break;
	case LT8722_TSD:
		avail.num_values = NO_OS_ARRAY_SIZE(lt8722_fault_avail);
		avail.values = lt8722_fault_avail;
		break;
	case LT8722_VCC_UVLO:
		avail.num_values = NO_OS_ARRAY_SIZE(lt8722_fault_avail);
		avail.values = lt8722_fault_avail;
		break;
	case LT8722_VDDIO_UVLO:
		avail.num_values = NO_OS_ARRAY_SIZE(lt8722_fault_avail);
		avail.values = lt8722_fault_avail;
		break;
	case LT8722_CP_UVLO:
		avail.num_values = NO_OS_ARRAY_SIZE(lt8722_fault_avail);
		avail.values = lt8722_fault_avail;
		break;
	case LT8722_V2P5_UVLO:
		avail.num_values = NO_OS_ARRAY_SIZE(lt8722_fault_avail);
		avail.values = lt8722_fault_avail;
		break;
	}

	return avail;
}

/**
 * @brief Get the list of available values of a SPIS_AMUX register attribute.
 * @param attr - The attribute for which the list is needed.
 * @return The list of available values of the attribute.
 */
struct lt8722_iio_attr_avail lt8722_iio_get_amux_available(
	enum lt8722_iio_spis_amux_attrs attr)
{
	struct lt8722_iio_attr_avail avail;

	switch (attr) {
	case LT8722_AMUX:
		avail.num_values = NO_OS_ARRAY_SIZE(lt8722_amux_avail);
		avail.values = lt8722_amux_avail;
		avail.alt_values = lt8722_amux_alt_avail;
		break;
	case LT8722_AMUX_TEST:
		avail.num_values = NO_OS_ARRAY_SIZE(lt8722_amux_test_avail);
		avail.values = lt8722_amux_test_avail;
		break;
	case LT8722_AOUT_EN:
		avail.num_values = NO_OS_ARRAY_SIZE(lt8722_enable_avail);
		avail.values = lt8722_enable_avail;
		break;
	}

	return avail;
}

/**
 * @brief Wrapper function for reading data from the LT8722 device.
 * @param desc - The LT8722 device structure.
 * @param reg - The register address to read from.
 * @param readval - The data read from the register.
 * @return 0 in case of success, an error code otherwise.
 */
static int lt8722_iio_reg_read(void *dev, uint32_t reg, uint32_t *readval)
{
	struct lt8722_iio_dev *iio_lt8722;
	struct lt8722_dev *lt8722;

	if (!dev || !readval)
		return -EINVAL;

	iio_lt8722 = (struct lt8722_iio_dev *)dev;
	lt8722 = iio_lt8722->lt8722_dev;

	return lt8722_reg_read(lt8722, reg, readval);
}

/**
 * @brief Wrapper function for writing data to the LT8722 device.
 * @param desc - The LT8722 device structure.
 * @param reg - The register address to write to.
 * @param writeval - The data to write to the register.
 * @return 0 in case of success, an error code otherwise.
 */
static int lt8722_iio_reg_write(void *dev, uint32_t reg, uint32_t writeval)
{
	struct lt8722_iio_dev *iio_lt8722;
	struct lt8722_dev *lt8722;

	if (!dev)
		return -EINVAL;

	iio_lt8722 = (struct lt8722_iio_dev *)dev;
	lt8722 = iio_lt8722->lt8722_dev;

	return lt8722_reg_write(lt8722, reg, writeval);
}

/**
 * @brief Handles the read request for SPIS DAC register attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
 */
static int lt8722_iio_read_dac(void *dev, char *buf, uint32_t len,
			       const struct iio_ch_info *channel, intptr_t priv)
{
	int ret;
	int32_t value;
	struct lt8722_iio_dev *iio_lt8722;
	struct lt8722_dev *lt8722;

	if (!dev)
		return -EINVAL;

	iio_lt8722 = (struct lt8722_iio_dev *)dev;
	lt8722 = iio_lt8722->lt8722_dev;

	ret = lt8722_get_dac(lt8722, (uint32_t *)&value);
	if (ret)
		return ret;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&value);
}

/**
 * @brief Handles the write request for SPIS DAC register attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
 */
static int lt8722_iio_write_dac(void *dev, char *buf, uint32_t len,
				const struct iio_ch_info *channel, intptr_t priv)
{
	int ret;
	int32_t value;
	struct lt8722_iio_dev *iio_lt8722;
	struct lt8722_dev *lt8722;

	if (!dev)
		return -EINVAL;

	iio_lt8722 = (struct lt8722_iio_dev *)dev;
	lt8722 = iio_lt8722->lt8722_dev;

	ret = iio_parse_value(buf, IIO_VAL_INT, (int32_t *)&value, NULL);
	if (ret)
		return ret;

	return lt8722_set_dac(lt8722, value);
}

/**
 * @brief Handles the read request for available SPIS_COMMAND register
 * 		attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int lt8722_iio_read_command_available(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	int length = 0;
	int32_t i;
	struct lt8722_iio_attr_avail avail =
		lt8722_iio_get_command_available(priv);

	for (i = 0; i < avail.num_values; i++) {
		if (i == avail.num_values - 1)
			length += sprintf(buf + length, "%s", avail.values[i]);
		else
			length += sprintf(buf + length, "%s ", avail.values[i]);
	}

	return length;
}

/**
 * @brief Handles the read request for SPIS_COMMAND register attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int lt8722_iio_read_command(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel, intptr_t priv)
{
	int ret;
	int32_t value;
	uint32_t reg_data;
	uint32_t mask = lt8722_iio_get_mask_command(priv);
	struct lt8722_iio_attr_avail avail =
		lt8722_iio_get_command_available(priv);
	struct lt8722_iio_dev *iio_lt8722;
	struct lt8722_dev *lt8722;

	if (!dev)
		return -EINVAL;

	iio_lt8722 = (struct lt8722_iio_dev *)dev;
	lt8722 = iio_lt8722->lt8722_dev;

	ret = lt8722_reg_read(lt8722, LT8722_SPIS_COMMAND, &reg_data);
	if (ret)
		return ret;

	value = no_os_field_get(mask, reg_data);

	return sprintf(buf, "%s", avail.values[value]);
}

/**
 * @brief Handles the write request for SPIS_COMMAND register attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
 */
static int lt8722_iio_write_command(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t value;
	uint32_t mask = lt8722_iio_get_mask_command(priv);
	struct lt8722_iio_attr_avail avail =
		lt8722_iio_get_command_available(priv);
	struct lt8722_iio_dev *iio_lt8722;
	struct lt8722_dev *lt8722;

	if (!dev)
		return -EINVAL;

	iio_lt8722 = (struct lt8722_iio_dev *)dev;
	lt8722 = iio_lt8722->lt8722_dev;

	for (value = 0; value < avail.num_values; value++)
		if (!strcmp(buf, avail.values[value]))
			break;

	if (value == avail.num_values)
		return -EINVAL;

	return lt8722_reg_write_mask(lt8722, LT8722_SPIS_COMMAND, mask, value);
}

/**
 * @brief Handles the read request for available SPIS_STATUS register attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int lt8722_iio_read_status_available(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	int length = 0;
	int32_t i;
	struct lt8722_iio_attr_avail avail =
		lt8722_iio_get_status_attr_available(priv);

	for (i = 0; i < avail.num_values; i++)
		if (i == avail.num_values - 1)
			length += sprintf(buf + length, "%s", avail.values[i]);
		else
			length += sprintf(buf + length, "%s ", avail.values[i]);

	return length;
}

/**
 * @brief Handles the read request for SPIS_STATUS register attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int lt8722_iio_read_status(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel, intptr_t priv)
{
	int ret;
	int32_t value;
	uint32_t reg_data;
	uint32_t mask = lt8722_iio_get_mask_status(priv);
	struct lt8722_iio_attr_avail avail =
		lt8722_iio_get_status_attr_available(priv);
	struct lt8722_iio_dev *iio_lt8722;
	struct lt8722_dev *lt8722;

	if (!dev)
		return -EINVAL;

	iio_lt8722 = (struct lt8722_iio_dev *)dev;
	lt8722 = iio_lt8722->lt8722_dev;

	ret = lt8722_reg_read(lt8722, LT8722_SPIS_STATUS, &reg_data);
	if (ret)
		return ret;

	value = no_os_field_get(mask, reg_data);

	return sprintf(buf, "%s", avail.values[value]);
}

/**
 * @brief Handles the write request for SPIS_STATUS register attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
 */
static int lt8722_iio_write_status(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t value;
	uint32_t mask = lt8722_iio_get_mask_status(priv);
	struct lt8722_iio_attr_avail avail =
		lt8722_iio_get_status_attr_available(priv);
	struct lt8722_iio_dev *iio_lt8722;
	struct lt8722_dev *lt8722;

	if (!dev)
		return -EINVAL;

	iio_lt8722 = (struct lt8722_iio_dev *)dev;
	lt8722 = iio_lt8722->lt8722_dev;

	for (value = 0; value < avail.num_values; value++)
		if (!strcmp(buf, avail.values[value]))
			break;

	if (value == avail.num_values)
		return -EINVAL;

	return lt8722_reg_write_mask(lt8722, LT8722_SPIS_STATUS, mask, value);
}

/**
 * @brief Handles the read request for SPIS DAC ILIMN register attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
 */
static int lt8722_iio_read_dac_ilimn(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel, intptr_t priv)
{
	int ret;
	uint32_t value;
	struct lt8722_iio_dev *iio_lt8722;
	struct lt8722_dev *lt8722;

	if (!dev)
		return -EINVAL;

	iio_lt8722 = (struct lt8722_iio_dev *)dev;
	lt8722 = iio_lt8722->lt8722_dev;

	ret = lt8722_get_spis_dac_ilimn(lt8722, &value);
	if (ret)
		return ret;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&value);
}

/**
 * @brief Handles the write request for SPIS DAC ILIMN register attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
 */
static int lt8722_iio_write_dac_ilimn(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel, intptr_t priv)
{
	int ret;
	uint32_t value;
	struct lt8722_iio_dev *iio_lt8722;
	struct lt8722_dev *lt8722;

	if (!dev)
		return -EINVAL;

	iio_lt8722 = (struct lt8722_iio_dev *)dev;
	lt8722 = iio_lt8722->lt8722_dev;

	ret = iio_parse_value(buf, IIO_VAL_INT, (int32_t *)&value, NULL);
	if (ret)
		return ret;

	return lt8722_set_spis_dac_ilimn(lt8722, value);
}

/**
 * @brief Handles the read request for SPIS DAC ILIMP register attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
 */
static int lt8722_iio_read_dac_ilimp(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel, intptr_t priv)
{
	int ret;
	uint32_t value;
	struct lt8722_iio_dev *iio_lt8722;
	struct lt8722_dev *lt8722;

	if (!dev)
		return -EINVAL;

	iio_lt8722 = (struct lt8722_iio_dev *)dev;
	lt8722 = iio_lt8722->lt8722_dev;

	ret = lt8722_get_spis_dac_ilimp(lt8722, &value);
	if (ret)
		return ret;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&value);
}

/**
 * @brief Handles the write request for SPIS DAC ILIMP register attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
 */
static int lt8722_iio_write_dac_ilimp(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel, intptr_t priv)
{
	int ret;
	int32_t value;
	struct lt8722_iio_dev *iio_lt8722;
	struct lt8722_dev *lt8722;

	if (!dev)
		return -EINVAL;

	iio_lt8722 = (struct lt8722_iio_dev *)dev;
	lt8722 = iio_lt8722->lt8722_dev;

	ret = iio_parse_value(buf, IIO_VAL_INT, &value, NULL);
	if (ret)
		return ret;

	return lt8722_set_spis_dac_ilimp(lt8722, value);
}

/**
 * @brief Handles the read request for LT8722 output voltage in millivolts.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
 */
static int lt8722_iio_read_output_voltage(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	int ret;
	int64_t value;
	int32_t value_int;
	struct lt8722_iio_dev *iio_lt8722;
	struct lt8722_dev *lt8722;

	if (!dev)
		return -EINVAL;

	iio_lt8722 = (struct lt8722_iio_dev *)dev;
	lt8722 = iio_lt8722->lt8722_dev;

	ret = lt8722_get_output_voltage(lt8722, &value);
	if (ret)
		return ret;

	value_int = (int32_t)no_os_div_s64(value, 1000000);

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &value_int);
}

/**
 * @brief Handles the write request for LT8722 output voltage in millivolts.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
 */
static int lt8722_iio_write_output_voltage(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	int ret;
	int64_t value;
	int32_t value_int;
	struct lt8722_iio_dev *iio_lt8722;
	struct lt8722_dev *lt8722;

	if (!dev)
		return -EINVAL;

	iio_lt8722 = (struct lt8722_iio_dev *)dev;
	lt8722 = iio_lt8722->lt8722_dev;

	ret = iio_parse_value(buf, IIO_VAL_INT, &value_int, NULL);
	if (ret)
		return ret;

	value = value_int * 1000000;

	return lt8722_set_output_voltage(lt8722, value);
}

/**
 * @brief Handles the read request for available minimum output voltage
 * 	in millivolts.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int lt8722_iio_read_min_voltage_available(void *dev, char *buf,
		uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
{
	int length = 0;
	int32_t i;
	struct lt8722_iio_attr_avail avail = {
		.num_values = NO_OS_ARRAY_SIZE(lt8722_uv_clamp_avail),
		.values = lt8722_uv_clamp_avail,
	};

	for (i = 0; i < avail.num_values; i++) {
		if (i == avail.num_values - 1)
			length += sprintf(buf + length, "%s", avail.values[i]);
		else
			length += sprintf(buf + length, "%s ", avail.values[i]);
	}

	return length;
}

/**
 * @brief Handles the read request for minimum output voltage in millivolts.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
 */
static int lt8722_iio_read_min_voltage(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel, intptr_t priv)
{
	int ret;
	uint8_t value;
	struct lt8722_iio_dev *iio_lt8722;
	struct lt8722_dev *lt8722;

	if (!dev)
		return -EINVAL;

	iio_lt8722 = (struct lt8722_iio_dev *)dev;
	lt8722 = iio_lt8722->lt8722_dev;

	ret = lt8722_get_spis_uv_clamp(lt8722, &value);
	if (ret)
		return ret;

	return sprintf(buf, "%s", lt8722_uv_clamp_avail[value]);
}

/**
 * @brief Handles the write request for minimum output voltage in millivolts
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
 */
static int lt8722_iio_write_min_voltage(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel, intptr_t priv)
{
	uint32_t i;
	struct lt8722_iio_dev *iio_lt8722;
	struct lt8722_dev *lt8722;

	if (!dev)
		return -EINVAL;

	iio_lt8722 = (struct lt8722_iio_dev *)dev;
	lt8722 = iio_lt8722->lt8722_dev;

	for (i = 0; i < NO_OS_ARRAY_SIZE(lt8722_uv_clamp_avail); i++)
		if (!strcmp(buf, lt8722_uv_clamp_avail[i]))
			break;

	if (i == NO_OS_ARRAY_SIZE(lt8722_uv_clamp_avail))
		return -EINVAL;

	return lt8722_set_spis_uv_clamp(lt8722, i);
}

/**
 * @brief Handles the read request for available maximum output voltage
 * 		in millivolts.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int lt8722_iio_read_max_voltage_available(void *dev, char *buf,
		uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
{
	int length = 0;
	int32_t i;
	struct lt8722_iio_attr_avail avail = {
		.num_values = NO_OS_ARRAY_SIZE(lt8722_ov_clamp_avail),
		.values = lt8722_ov_clamp_avail,
	};

	for (i = 0; i < avail.num_values; i++) {
		if (i == avail.num_values - 1)
			length += sprintf(buf + length, "%s", avail.values[i]);
		else
			length += sprintf(buf + length, "%s ", avail.values[i]);
	}

	return length;
}

/**
 * @brief Handles the read request for maximum output voltage in millivolts.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
 */
static int lt8722_iio_read_max_voltage(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel, intptr_t priv)
{
	int ret;
	uint8_t value;
	struct lt8722_iio_dev *iio_lt8722;
	struct lt8722_dev *lt8722;

	if (!dev)
		return -EINVAL;

	iio_lt8722 = (struct lt8722_iio_dev *)dev;
	lt8722 = iio_lt8722->lt8722_dev;

	ret = lt8722_get_spis_ov_clamp(lt8722, &value);
	if (ret)
		return ret;

	return sprintf(buf, "%s", lt8722_ov_clamp_avail[value]);
}

/**
 * @brief Handles the write request for maximum output voltage in millivolts.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
 */
static int lt8722_iio_write_max_voltage(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel, intptr_t priv)
{
	uint32_t i;
	struct lt8722_iio_dev *iio_lt8722;
	struct lt8722_dev *lt8722;

	if (!dev)
		return -EINVAL;

	iio_lt8722 = (struct lt8722_iio_dev *)dev;
	lt8722 = iio_lt8722->lt8722_dev;

	for (i = 0; i < NO_OS_ARRAY_SIZE(lt8722_ov_clamp_avail); i++)
		if (!strcmp(buf, lt8722_ov_clamp_avail[i]))
			break;

	if (i == NO_OS_ARRAY_SIZE(lt8722_ov_clamp_avail))
		return -EINVAL;

	return lt8722_set_spis_ov_clamp(lt8722, i);
}

/**
 * @brief Handles the read request for available SPIS_AMUX register attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int lt8722_iio_read_amux_available(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	int ret;
	int length = 0;
	int32_t i;
	struct lt8722_iio_attr_avail avail =
		lt8722_iio_get_amux_available(priv);
	const char *values;
	uint8_t amux;
	uint8_t amux_test;
	struct lt8722_iio_dev *iio_lt8722;
	struct lt8722_dev *lt8722;

	if (!dev)
		return -EINVAL;

	iio_lt8722 = (struct lt8722_iio_dev *)dev;
	lt8722 = iio_lt8722->lt8722_dev;

	ret = lt8722_get_amux(lt8722, &amux, &amux_test);
	if (ret)
		return ret;

	for (i = 0; i < avail.num_values; i++) {
		if (priv == LT8722_AMUX)
			if (amux_test == 1 || amux_test == 3)
				values = avail.alt_values[i];
			else
				values = avail.values[i];
		else
			values = avail.values[i];

		if (i == avail.num_values - 1)
			length += sprintf(buf + length, "%s", values);
		else
			length += sprintf(buf + length, "%s ", values);
	}

	return length;
}

/**
 * @brief Handles the read request for SPIS_AMUX register attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int lt8722_iio_read_amux(void *dev, char *buf, uint32_t len,
				const struct iio_ch_info *channel, intptr_t priv)
{
	int ret;
	int32_t value;
	uint32_t reg_data;
	uint32_t mask = lt8722_iio_get_mask_amux(priv);
	struct lt8722_iio_attr_avail avail =
		lt8722_iio_get_amux_available(priv);
	const char *values;
	struct lt8722_iio_dev *iio_lt8722;
	struct lt8722_dev *lt8722;

	if (!dev)
		return -EINVAL;

	iio_lt8722 = (struct lt8722_iio_dev *)dev;
	lt8722 = iio_lt8722->lt8722_dev;

	if (priv == LT8722_AMUX) {
		uint8_t amux;
		uint8_t amux_test;

		ret = lt8722_get_amux(lt8722, &amux, &amux_test);
		if (ret)
			return ret;

		if (amux_test == 1 || amux_test == 3)
			values = avail.alt_values[amux];
		else
			values = avail.values[amux];
	} else {
		ret = lt8722_reg_read(lt8722, LT8722_SPIS_AMUX, &reg_data);
		if (ret)
			return ret;

		value = no_os_field_get(mask, reg_data);
		values = avail.values[value];
	}

	return sprintf(buf, "%s", values);
}

/**
 * @brief Handles the write request for SPIS_AMUX register attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
 */
static int lt8722_iio_write_amux(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t priv)
{
	int ret;
	int32_t value;
	uint32_t mask = lt8722_iio_get_mask_amux(priv);
	struct lt8722_iio_attr_avail avail =
		lt8722_iio_get_amux_available(priv);
	const char *values;
	uint8_t amux;
	uint8_t amux_test;
	struct lt8722_iio_dev *iio_lt8722;
	struct lt8722_dev *lt8722;

	if (!dev)
		return -EINVAL;

	iio_lt8722 = (struct lt8722_iio_dev *)dev;
	lt8722 = iio_lt8722->lt8722_dev;

	ret = lt8722_get_amux(lt8722, &amux, &amux_test);
	if (ret)
		return ret;

	for (value = 0; value < avail.num_values; value++) {
		if (priv == LT8722_AMUX)
			if (amux_test == 1 || amux_test == 3)
				values = avail.alt_values[value];
			else
				values = avail.values[value];
		else
			values = avail.values[value];

		if (!strcmp(buf, values))
			break;
	}

	if (value == avail.num_values)
		return -EINVAL;

	return lt8722_reg_write_mask(lt8722, LT8722_SPIS_AMUX, mask, value);
}

/**
 * @brief Handles the read request available for LT8722 pins states.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int lt8722_iio_read_pin_available(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	int length = 0;
	uint32_t i;
	uint32_t num_values = NO_OS_ARRAY_SIZE(lt8722_pin_avail);

	if (!dev)
		return -EINVAL;

	for (i = 0; i < num_values; i++)
		if (i == num_values - 1)
			length += sprintf(buf + length, "%s",
					  lt8722_pin_avail[i]);
		else
			length += sprintf(buf + length, "%s ",
					  lt8722_pin_avail[i]);

	return length;
}

/**
 * @brief Handles the read request for LT8722 pins.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int lt8722_iio_read_pin(void *dev, char *buf, uint32_t len,
			       const struct iio_ch_info *channel, intptr_t priv)
{
	int ret;
	struct lt8722_iio_dev *iio_lt8722;
	struct lt8722_dev *lt8722;

	if (!dev)
		return -EINVAL;

	iio_lt8722 = (struct lt8722_iio_dev *)dev;
	lt8722 = iio_lt8722->lt8722_dev;

	uint8_t value;

	switch(priv) {
	case LT8722_EN_PIN:
		ret = lt8722_get_en_pin(lt8722, &value);
		if (ret)
			return ret;

		break;
	case LT8722_SWEN_PIN:
		ret = lt8722_get_swen_pin(lt8722, &value);
		if (ret)
			return ret;

		break;
	}

	return sprintf(buf, "%s", lt8722_pin_avail[value]);
}

/**
 * @brief Handles the write request for LT8722 pins.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
 */
static int lt8722_iio_write_pin(void *dev, char *buf, uint32_t len,
				const struct iio_ch_info *channel, intptr_t priv)
{
	int ret;
	uint8_t value;
	uint32_t num_values = NO_OS_ARRAY_SIZE(lt8722_pin_avail);
	struct lt8722_iio_dev *iio_lt8722;
	struct lt8722_dev *lt8722;

	if (!dev)
		return -EINVAL;

	iio_lt8722 = (struct lt8722_iio_dev *)dev;
	lt8722 = iio_lt8722->lt8722_dev;

	for (value = 0; value < num_values; value++)
		if (!strcmp(buf, lt8722_pin_avail[value]))
			break;

	if (value == num_values)
		return -EINVAL;

	switch(priv) {
	case LT8722_EN_PIN:
		ret = lt8722_set_en_pin(lt8722, value);
	case LT8722_SWEN_PIN:
		ret = lt8722_set_swen_pin(lt8722, value);
	}

	return ret;
}

/**
 * @brief Initializes the LT8722 IIO descriptor.
 * @param iio_dev - The iio device descriptor.
 * @param init_param - The structure that contains the device
 * 		initial parameters.
 * @return 0 in case of success, an error code otherwise.
 */
int lt8722_iio_init(struct lt8722_iio_dev **iio_dev,
		    struct lt8722_iio_dev_init_param *init_param)
{
	struct lt8722_iio_dev *descriptor;
	int ret;

	if (!iio_dev || !init_param || !init_param->lt8722_init_param)
		return -EINVAL;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = lt8722_init(&descriptor->lt8722_dev,
			  init_param->lt8722_init_param);
	if (ret)
		goto free_desc;

	descriptor->iio_dev = &lt8722_iio_dev;

	*iio_dev = descriptor;

	return 0;

free_desc:
	lt8722_iio_remove(descriptor);

	return ret;
}

/**
 * @brief Free resources allocated by the init function.
 * @param desc - The iio device descriptor.
 * @return 0 in case of success, an error code otherwise.
 */
int lt8722_iio_remove(struct lt8722_iio_dev *desc)
{
	if (!desc)
		return -ENODEV;

	no_os_free(desc->iio_dev->channels);
	lt8722_remove(desc->lt8722_dev);
	no_os_free(desc);

	return 0;
}
