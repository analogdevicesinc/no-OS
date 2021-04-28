/*!
  @brief     Use Case Settings
 *
 * @copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * @addtogroup __ADI_AD9083_APP__
 * @{
 */

/*============= I N C L U D E S ============*/
#include <stdio.h>
#include <unistd.h>
#include "adi_ad9083.h"
#include "uc_settings.h"

/*============= D A T A ====================*/
uint64_t clk_hz[][3] = {
	/*-----dev_ref----*//*----fpga_ref----*//*-----adc_clk----*/
	{ (uint64_t)250e6,    (uint64_t)500e6,    (uint64_t)2000e6 }, /* uc0,  Lane Rate: 10Gbps */
	{ (uint64_t)250e6,    (uint64_t)750e6,    (uint64_t)2000e6 }, /* uc1,  Lane Rate: 15Gbps */
	{ (uint64_t)250e6,    (uint64_t)500e6,    (uint64_t)2000e6 }, /* uc2,  Lane Rate: 10Gbps */
	{ (uint64_t)200e6,    (uint64_t)800e6,    (uint64_t)1600e6 }, /* uc3,  Lane Rate: 16Gbps */
	{ (uint64_t)125e6,    (uint64_t)500e6,    (uint64_t)1000e6 }, /* uc4,  Lane Rate: 10Gbps */
	{ (uint64_t)250e6,    (uint64_t)375e6,    (uint64_t)1000e6 }, /* uc5,  Lane Rate: 7.5Gbps */
	{ (uint64_t)100e6,    (uint64_t)500e6,    (uint64_t)2000e6 }, /* uc6,  Lane Rate: 10Gbps */
	{ (uint64_t)250e6,    (uint64_t)500e6,    (uint64_t)2000e6 }, /* uc7,  Lane Rate: 10Gbps */
	{ (uint64_t)200e6,    (uint64_t)600e6,    (uint64_t)1600e6 }, /* uc8,  Lane Rate: 12Gbps */
	{ (uint64_t)250e6,    (uint64_t)500e6,    (uint64_t)2000e6 }, /* uc9,  Lane Rate: 10Gbps */
};

/* Full scale valtage, unit: mv */
uint32_t vmax[] = {
	1800,     /* uc0 */
	1800,     /* uc1 */
	1800,     /* uc2 */
	1800,     /* uc3 */
	1800,     /* uc4 */
	1800,     /* uc5 */
	1800,     /* uc6 */
	1800,     /* uc7 */
	1800,     /* uc8 */
	1800,     /* uc9 */
};

/* Cut-off frequency of low-pass filter */
uint32_t fc[] = {
	800e6,    /* uc0 */
	800e6,    /* uc1 */
	800e6,    /* uc2 */
	800e6,    /* uc3 */
	800e6,    /* uc4 */
	800e6,    /* uc5 */
	800e6,    /* uc6 */
	800e6,    /* uc7 */
	800e6,    /* uc8 */
	800e6,    /* uc9 */
};

/* Termination resistor: 100Ohm, 200Ohm, open */
uint8_t rterm[] = {
	AD9083_ADC_TERM_RES_100,     /* uc0 */
	AD9083_ADC_TERM_RES_100,     /* uc1 */
	AD9083_ADC_TERM_RES_100,     /* uc2 */
	AD9083_ADC_TERM_RES_100,     /* uc3 */
	AD9083_ADC_TERM_RES_100,     /* uc4 */
	AD9083_ADC_TERM_RES_100,     /* uc5 */
	AD9083_ADC_TERM_RES_100,     /* uc6 */
	AD9083_ADC_TERM_RES_100,     /* uc7 */
	AD9083_ADC_TERM_RES_100,     /* uc8 */
	AD9083_ADC_TERM_RES_100,     /* uc9 */
};

/* Enable/disable high perfermance */
uint32_t en_hp[] = {
	0,        /* uc0 */
	0,        /* uc1 */
	0,        /* uc2 */
	0,        /* uc3 */
	0,        /* uc4 */
	0,        /* uc5 */
	0,        /* uc6 */
	0,        /* uc7 */
	0,        /* uc8 */
	0,        /* uc9 */
};

/* the backoff in terms of noiseterms of noise, 100 * dB */
uint32_t backoff[] = {
	0,      /* uc0 */
	100,      /* uc1 */
	100,      /* uc2 */
	100,      /* uc3 */
	100,      /* uc4 */
	100,      /* uc5 */
	100,      /* uc6 */
	0,      /* uc7 */
	0,      /* uc8 */
	0,      /* uc9 */
};

/* max input */
uint32_t finmax[] = {
	100e6,    /* uc0 */
	100e6,    /* uc1 */
	80e6,     /* uc2 */
	50e6,     /* uc3 */
	50e6,     /* uc4 */
	100e6,    /* uc5 */
	100e6,    /* uc6 */
	100e6,    /* uc7 */
	100e6,    /* uc8 */
	100e6,    /* uc9 */
};

uint64_t nco_freq_hz[][3] = {
	/*NCO0       NCO1     NCO2 */
	{ 74.21875e6,0,        0 },   /* uc0 */
	{ 0,         0,        0 },   /* uc1 */
	{ 0,         0,        0 },   /* uc2 */
	{ 75e6,      0,        0 },   /* uc3 */
	{ 0,         0,        0 },   /* uc4 */
	{ 0,         0,        0 },   /* uc5 */
	{ 100e6,     0,        0 },   /* uc6 */
	{ 0,         0,        0 },   /* uc7 */
	{ 48e6,    51e6,     54e6 },  /* uc8 */
	{ 0,         0,        0 },   /* uc9 */
};

uint8_t decimation[][4] = {
	/* CIC,                J                G   H  */
	{   AD9083_CIC_DEC_4,   AD9083_J_DEC_16,  0,  0 }, /* uc0 */
	{   AD9083_CIC_DEC_4,   AD9083_J_DEC_8,   0,  0 }, /* uc1 */
	{   AD9083_CIC_DEC_8,   AD9083_J_DEC_16,  0,  0 }, /* uc2 */
	{   AD9083_CIC_DEC_4,   AD9083_J_DEC_16,  0,  0 }, /* uc3 */
	{   AD9083_CIC_DEC_8,   AD9083_J_DEC_1,   0,  0 }, /* uc4 */
	{   AD9083_CIC_DEC_8,   AD9083_J_DEC_1,   0,  0 }, /* uc5 */
	{   AD9083_CIC_DEC_4,   AD9083_J_DEC_16,  0,  0 }, /* uc6 */
	{   AD9083_CIC_DEC_4,   AD9083_J_DEC_4,   0,  0 }, /* uc7 */
	{   AD9083_CIC_DEC_4,   AD9083_J_DEC_1,  16, 16 }, /* uc8 */
	{   AD9083_CIC_DEC_4,   AD9083_J_DEC_4,   0,  0 }, /* uc9 */
};

uint8_t nco0_datapath_mode[] = {
	AD9083_DATAPATH_ADC_CIC_NCO_J,   /* uc0 */
	AD9083_DATAPATH_ADC_J,           /* uc1 */
	AD9083_DATAPATH_ADC_CIC_NCO_J,   /* uc2 */
	AD9083_DATAPATH_ADC_CIC_NCO_J,   /* uc3 */
	AD9083_DATAPATH_ADC_CIC_J,       /* uc4 */
	AD9083_DATAPATH_ADC_CIC_J,       /* uc5 */
	AD9083_DATAPATH_ADC_CIC_NCO_J,   /* uc6 */
	AD9083_DATAPATH_ADC_CIC_J,       /* uc7 */
	AD9083_DATAPATH_ADC_CIC_NCO_G_H, /* uc8 */
	AD9083_DATAPATH_ADC_CIC_J,       /* uc9 */
};

uint8_t jtx_logiclane_mapping_pe_brd[4] = { 0, 1, 2, 3 };

adi_cms_jesd_param_t jtx_param[] = {
	/*L   F   M   S   HD  K    N    N'   CF  CS DID BID LID  SC SCR */
	{ 2, 32, 32,  1,  1,  32,  16,  16,  0,  0,  0,  0,  0,  0,  1 }, /* uc0 */
	{ 4,  6, 16,  1,  1,  32,  12,  12,  0,  0,  0,  0,  0,  0,  1 }, /* uc1 */
	{ 1, 64, 32,  1,  1,  16,  16,  16,  0,  0,  0,  0,  0,  0,  1 }, /* uc2 */
	{ 1, 64, 32,  1,  1,  16,  16,  16,  0,  0,  0,  0,  0,  0,  1 }, /* uc3 */
	{ 3,  8, 16,  1,  1,  32,  12,  12,  0,  0,  0,  0,  0,  0,  0 }, /* uc4 */
	{ 4,  6, 16,  1,  1,  32,  12,  12,  0,  0,  0,  0,  0,  0,  1 }, /* uc5 */
	{ 2, 32, 32,  1,  1,  32,  16,  16,  0,  0,  0,  0,  0,  0,  1 }, /* uc6 */
	{ 4,  8, 16,  1,  1,  32,  16,  16,  0,  0,  0,  0,  0,  0,  1 }, /* uc7 */
	{ 4, 48, 96,  1,  1,  16,  16,  16,  0,  0,  0,  0,  0,  0,  1 }, /* uc8 */
	{ 4,  8, 16,  1,  1,  32,  16,  16,  0,  0,  0,  0,  0,  1,  1 }, /* uc9 */
};

static struct uc_settings uc_settings = {
	.clk_hz = clk_hz,
	.vmax = vmax,
	.fc = fc,
	.rterm = rterm,
	.en_hp = en_hp,
	.backoff = backoff,
	.finmax = finmax,
	.nco_freq_hz = nco_freq_hz,
	.decimation = decimation,
	.nco0_datapath_mode = nco0_datapath_mode,
	.jtx_logiclane_mapping_pe_brd = jtx_logiclane_mapping_pe_brd,
	.jtx_param = jtx_param,
};

/**
 * @brief Get use case settings.
 * @return use case settings.
 */
struct uc_settings* get_uc_settings()
{
	return &uc_settings;
}

/*! @} */
