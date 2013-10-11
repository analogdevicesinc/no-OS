
/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "xparameters.h"
#include "stdint.h"
#include "xil_io.h"
#include "no_os_port.h"
#include "dac_core.h"

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
struct dds_state dds_st;
extern struct ad9361_rf_phy *phy;

/***************************************************************************//**
 * @brief dac_read
*******************************************************************************/
void dac_read(uint32_t regAddr, uint32_t *data)
{
    *data = Xil_In32(XPAR_AXI_AD9361_0_BASEADDR + 0x4000 + regAddr);
}

/***************************************************************************//**
 * @brief dac_write
*******************************************************************************/
void dac_write(uint32_t regAddr, uint32_t data)
{
	Xil_Out32(XPAR_AXI_AD9361_0_BASEADDR + 0x4000 + regAddr, data);
}

/***************************************************************************//**
 * @brief dds_default_setup
*******************************************************************************/
static int dds_default_setup(uint32_t chan, uint32_t phase,
							 uint32_t freq, uint32_t scale)
{
	uint64_t val64;
	uint32_t val;

	dds_st.cached_freq[chan] = freq;
	dds_st.cached_phase[chan] = phase;
	dds_st.cached_scale[chan] = scale;
	val64 = (u64) freq * 0xFFFFULL;
	do_div(val64, *dds_st.dac_clk);
	val = ADI_DDS_INCR(val64) | 1;

	val64 = (u64) phase * 0x10000ULL + (360000 / 2);
	do_div(val64, 360000);
	val |= ADI_DDS_INIT(val64);

	dac_write(ADI_REG_CHAN_CNTRL_1_IIOCHAN(chan), ADI_DDS_SCALE(scale));
	dac_write(ADI_REG_CHAN_CNTRL_2_IIOCHAN(chan), val);

	return 0;
}

/***************************************************************************//**
 * @brief dac_init
*******************************************************************************/
void dac_init(uint8_t data_sel)
{
	uint32_t txcount = 0;

	dac_write(ADI_REG_RSTN, 0x0);
	dac_write(ADI_REG_RSTN, ADI_RSTN);

	dac_write(ADI_REG_RATECNTRL, ADI_RATE(3));

	dds_st.dac_clk = &phy->clks[TX_SAMPL_CLK]->rate;

	dac_write(ADI_REG_CNTRL_1, 0);
	switch (data_sel) {
	case DATA_SEL_DDS:
		dds_default_setup(DDS_CHAN_TX1_I_F1, 90000, 1000000, 4);
		dds_default_setup(DDS_CHAN_TX1_I_F2, 90000, 1000000, 4);
		dds_default_setup(DDS_CHAN_TX1_Q_F1, 0, 1000000, 4);
		dds_default_setup(DDS_CHAN_TX1_Q_F2, 0, 1000000, 4);
		dds_default_setup(DDS_CHAN_TX2_I_F1, 90000, 1000000, 4);
		dds_default_setup(DDS_CHAN_TX2_I_F2, 90000, 1000000, 4);
		dds_default_setup(DDS_CHAN_TX2_Q_F1, 0, 1000000, 4);
		dds_default_setup(DDS_CHAN_TX2_Q_F2, 0, 1000000, 4);
		dac_write(ADI_REG_CNTRL_2, ADI_DATA_SEL(DATA_SEL_DDS));
		break;
	case DATA_SEL_DMA:
		dac_write(ADI_REG_VDMA_FRMCNT, txcount);

		dac_write(ADI_REG_CNTRL_2, ADI_DATA_SEL(DATA_SEL_DMA));
		dac_write(ADI_REG_VDMA_STATUS, ADI_VDMA_OVF | ADI_VDMA_UNF);

		break;
	default:
		break;
	}

	dac_write(ADI_REG_CNTRL_1, ADI_ENABLE);
}

/***************************************************************************//**
 * @brief dds_set_frequency
*******************************************************************************/
void dds_set_frequency(uint32_t chan, uint32_t freq)
{
	uint64_t val64;
	uint32_t ctrl_reg, reg;

	dac_read(ADI_REG_CNTRL_1, &ctrl_reg);
	dds_st.cached_freq[chan] = freq;
	dac_write(ADI_REG_CNTRL_1, 0);
	dac_read(ADI_REG_CHAN_CNTRL_2_IIOCHAN(chan), &reg);
	reg &= ~ADI_DDS_INCR(~0);
	val64 = (u64) freq * 0xFFFFULL;
	do_div(val64, *dds_st.dac_clk);
	reg |= ADI_DDS_INCR(val64) | 1;
	dac_write(ADI_REG_CHAN_CNTRL_2_IIOCHAN(chan), reg);
	dac_write(ADI_REG_CNTRL_1, ctrl_reg);
}

/***************************************************************************//**
 * @brief dds_set_phase
*******************************************************************************/
void dds_set_phase(uint32_t chan, uint32_t phase)
{
	uint64_t val64;
	uint32_t ctrl_reg, reg;

	dac_read(ADI_REG_CNTRL_1, &ctrl_reg);
	dds_st.cached_phase[chan] = phase;
	dac_write(ADI_REG_CNTRL_1, 0);
	dac_read(ADI_REG_CHAN_CNTRL_2_IIOCHAN(chan), &reg);
	reg &= ~ADI_DDS_INIT(~0);
	val64 = (u64) phase * 0x10000ULL + (360000 / 2);
	do_div(val64, 360000);
	reg |= ADI_DDS_INIT(val64);
	dac_write(ADI_REG_CHAN_CNTRL_2_IIOCHAN(chan), reg);
	dac_write(ADI_REG_CNTRL_1, ctrl_reg);
}

/***************************************************************************//**
 * @brief dds_set_phase
*******************************************************************************/
void dds_set_scale(uint32_t chan, uint32_t scale)
{
	uint32_t ctrl_reg;

	dac_read(ADI_REG_CNTRL_1, &ctrl_reg);
	dds_st.cached_scale[chan] = scale;
	dac_write(ADI_REG_CNTRL_1, 0);
	dac_write(ADI_REG_CHAN_CNTRL_1_IIOCHAN(chan), ADI_DDS_SCALE(scale));
	dac_write(ADI_REG_CNTRL_1, ctrl_reg);
}

/***************************************************************************//**
 * @brief dds_update
*******************************************************************************/
void dds_update(void)
{
	uint32_t chan;

	for(chan = DDS_CHAN_TX1_I_F1; chan <= DDS_CHAN_TX2_Q_F2; chan++)
	{
		dds_set_frequency(chan, dds_st.cached_freq[chan]);
		dds_set_phase(chan, dds_st.cached_phase[chan]);
		dds_set_scale(chan, dds_st.cached_scale[chan]);
	}
}
