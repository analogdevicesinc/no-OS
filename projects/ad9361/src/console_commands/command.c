/**************************************************************************//**
 *   @file   command.c
 *   @brief  Implementation of AD9361 Command Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
 *******************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "command.h"
#include "console.h"
#include "../ad9361_api.h"
#include "dac_core.h"

/******************************************************************************/
/************************ Constants Definitions *******************************/
/******************************************************************************/
command cmd_list[] = {
	{"help?", "Displays all available commands.", "", get_help},
	{"register?", "Gets the specified register value.", "", get_register},
	{"tx_lo_freq?", "Gets current TX LO frequency [MHz].", "", get_tx_lo_freq},
	{"tx_lo_freq=", "Sets the TX LO frequency [MHz].", "", set_tx_lo_freq},
	{"tx_samp_freq?", "Gets current TX sampling frequency [Hz].", "", get_tx_samp_freq},
	{"tx_samp_freq=", "Sets the TX sampling frequency [Hz].", "", set_tx_samp_freq},
	{"tx_rf_bandwidth?", "Gets current TX RF bandwidth [Hz].", "", get_tx_rf_bandwidth},
	{"tx_rf_bandwidth=", "Sets the TX RF bandwidth [Hz].", "", set_tx_rf_bandwidth},
	{"tx1_attenuation?", "Gets current TX1 attenuation [mdB].", "", get_tx1_attenuation},
	{"tx1_attenuation=", "Sets the TX1 attenuation [mdB].", "", set_tx1_attenuation},
	{"tx2_attenuation?", "Gets current TX2 attenuation [mdB].", "", get_tx2_attenuation},
	{"tx2_attenuation=", "Sets the TX2 attenuation [mdB].", "", set_tx2_attenuation},
	{"tx_fir_en?", "Gets current TX FIR state.", "", get_tx_fir_en},
	{"tx_fir_en=", "Sets the TX FIR state.", "", set_tx_fir_en},
	{"rx_lo_freq?", "Gets current RX LO frequency [MHz].", "", get_rx_lo_freq},
	{"rx_lo_freq=", "Sets the RX LO frequency [MHz].", "", set_rx_lo_freq},
	{"rx_samp_freq?", "Gets current RX sampling frequency [Hz].", "", get_rx_samp_freq},
	{"rx_samp_freq=", "Sets the RX sampling frequency [Hz].", "", set_rx_samp_freq},
	{"rx_rf_bandwidth?", "Gets current RX RF bandwidth [Hz].", "", get_rx_rf_bandwidth},
	{"rx_rf_bandwidth=", "Sets the RX RF bandwidth [Hz].", "", set_rx_rf_bandwidth},
	{"rx1_gc_mode?", "Gets current RX1 GC mode.", "", get_rx1_gc_mode},
	{"rx1_gc_mode=", "Sets the RX1 GC mode.", "", set_rx1_gc_mode},
	{"rx2_gc_mode?", "Gets current RX2 GC mode.", "", get_rx2_gc_mode},
	{"rx2_gc_mode=", "Sets the RX2 GC mode.", "", set_rx2_gc_mode},
	{"rx1_rf_gain?", "Gets current RX1 RF gain.", "", get_rx1_rf_gain},
	{"rx1_rf_gain=", "Sets the RX1 RF gain.", "", set_rx1_rf_gain},
	{"rx2_rf_gain?", "Gets current RX2 RF gain.", "", get_rx2_rf_gain},
	{"rx2_rf_gain=", "Sets the RX2 RF gain.", "", set_rx2_rf_gain},
	{"rx_fir_en?", "Gets current RX FIR state.", "", get_rx_fir_en},
	{"rx_fir_en=", "Sets the RX FIR state.", "", set_rx_fir_en},
	{"dds_tx1_tone1_freq?", "Gets current DDS TX1 Tone 1 frequency [Hz].", "", get_dds_tx1_tone1_freq},
	{"dds_tx1_tone1_freq=", "Sets the DDS TX1 Tone 1 frequency [Hz].", "", set_dds_tx1_tone1_freq},
	{"dds_tx1_tone2_freq?", "Gets current DDS TX1 Tone 2 frequency [Hz].", "", get_dds_tx1_tone2_freq},
	{"dds_tx1_tone2_freq=", "Sets the DDS TX1 Tone 2 frequency [Hz].", "", set_dds_tx1_tone2_freq},
	{"dds_tx1_tone1_phase?", "Gets current DDS TX1 Tone 1 phase [degrees].", "", get_dds_tx1_tone1_phase},
	{"dds_tx1_tone1_phase=", "Sets the DDS TX1 Tone 1 phase [degrees].", "", set_dds_tx1_tone1_phase},
	{"dds_tx1_tone2_phase?", "Gets current DDS TX1 Tone 2 phase [degrees].", "", get_dds_tx1_tone2_phase},
	{"dds_tx1_tone2_phase=", "Sets the DDS TX1 Tone 2 phase [degrees].", "", set_dds_tx1_tone2_phase},
	{"dds_tx1_tone1_scale?", "Gets current DDS TX1 Tone 1 scale.", "", get_dds_tx1_tone1_scale},
	{"dds_tx1_tone1_scale=", "Sets the DDS TX1 Tone 1 scale.", "", set_dds_tx1_tone1_scale},
	{"dds_tx1_tone2_scale?", "Gets current DDS TX1 Tone 2 scale.", "", get_dds_tx1_tone2_scale},
	{"dds_tx1_tone2_scale=", "Sets the DDS TX1 Tone 2 scale.", "", set_dds_tx1_tone2_scale},
	{"dds_tx2_tone1_freq?", "Gets current DDS TX2 Tone 1 frequency [Hz].", "", get_dds_tx2_tone1_freq},
	{"dds_tx2_tone1_freq=", "Sets the DDS TX2 Tone 1 frequency [Hz].", "", set_dds_tx2_tone1_freq},
	{"dds_tx2_tone2_freq?", "Gets current DDS TX2 Tone 2 frequency [Hz].", "", get_dds_tx2_tone2_freq},
	{"dds_tx2_tone2_freq=", "Sets the DDS TX2 Tone 2 frequency [Hz].", "", set_dds_tx2_tone2_freq},
	{"dds_tx2_tone1_phase?", "Gets current DDS TX2 Tone 1 phase [degrees].", "", get_dds_tx2_tone1_phase},
	{"dds_tx2_tone1_phase=", "Sets the DDS TX2 Tone 1 phase [degrees].", "", set_dds_tx2_tone1_phase},
	{"dds_tx2_tone2_phase?", "Gets current DDS TX2 Tone 2 phase [degrees].", "", get_dds_tx2_tone2_phase},
	{"dds_tx2_tone2_phase=", "Sets the DDS TX2 Tone 2 phase [degrees].", "", set_dds_tx2_tone2_phase},
	{"dds_tx2_tone1_scale?", "Gets current DDS TX2 Tone 1 scale.", "", get_dds_tx2_tone1_scale},
	{"dds_tx2_tone1_scale=", "Sets the DDS TX2 Tone 1 scale.", "", set_dds_tx2_tone1_scale},
	{"dds_tx2_tone2_scale?", "Gets current DDS TX2 Tone 2 scale.", "", dds_tx2_tone2_scale},
	{"dds_tx2_tone2_scale=", "Sets the DDS TX2 Tone 2 scale.", "", set_dds_tx2_tone2_scale},
};
const char cmd_no = (sizeof(cmd_list) / sizeof(command));

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
extern struct dds_state dds_st;
extern struct ad9361_rf_phy *ad9361_phy;

/**************************************************************************//***
 * @brief Show the invalid parameter message.
 *
 * @return None.
*******************************************************************************/
void show_invalid_param_message(unsigned char cmd_no)
{
	console_print("Invalid parameter!\n");
	console_print("%s  - %s\n", (char*)cmd_list[cmd_no].name, (char*)cmd_list[cmd_no].description);
	console_print("Example: %s\n", (char*)cmd_list[cmd_no].example);
}

/**************************************************************************//***
 * @brief Displays all available commands.
 *
 * @return None.
*******************************************************************************/
void get_help(double* param, char param_no) // "help?" command
{
	unsigned char display_cmd;

	console_print("Available commands:\n");
	for(display_cmd = 0; display_cmd < cmd_no; display_cmd++)
	{
		console_print("%s  - %s\n", (char*)cmd_list[display_cmd].name,
								  (char*)cmd_list[display_cmd].description);
	}
}

/**************************************************************************//***
 * @brief Displays all available commands.
 *
 * @return None.
*******************************************************************************/
void get_register(double* param, char param_no) // "register?" command
{
	uint16_t reg_addr;
	uint8_t reg_val;
	struct spi_device spi;

	if(param_no >= 1)
	{
		spi.id_no = 0;
		reg_addr = param[0];
		reg_val = ad9361_spi_read(&spi, reg_addr);
		console_print("register[0x%x]=0x%x\n", reg_addr, reg_val);
	}
	else
		show_invalid_param_message(1);
}

/**************************************************************************//***
 * @brief Gets current TX LO frequency [MHz].
 *
 * @return None.
*******************************************************************************/
void get_tx_lo_freq(double* param, char param_no) // "tx_lo_freq?" command
{
	uint64_t lo_freq_hz;

	ad9361_get_tx_lo_freq(ad9361_phy, &lo_freq_hz);
	lo_freq_hz /= 1000000;
	console_print("tx_lo_freq=%d\n", (uint32_t)lo_freq_hz);
}

/**************************************************************************//***
 * @brief Sets the TX LO frequency [MHz].
 *
 * @return None.
*******************************************************************************/
void set_tx_lo_freq(double* param, char param_no) // "tx_lo_freq=" command
{
	uint64_t lo_freq_hz;

	if(param_no >= 1)
	{
		lo_freq_hz = param[0];
		lo_freq_hz *= 1000000;
		ad9361_set_tx_lo_freq(ad9361_phy, lo_freq_hz);
		lo_freq_hz /= 1000000;
		console_print("tx_lo_freq=%d\n", (uint32_t)lo_freq_hz);
	}
}

/**************************************************************************//***
 * @brief Gets current sampling frequency [Hz].
 *
 * @return None.
*******************************************************************************/
void get_tx_samp_freq(double* param, char param_no) // "tx_samp_freq?" command
{
	uint32_t sampling_freq_hz;

	ad9361_get_tx_sampling_freq(ad9361_phy, &sampling_freq_hz);
	console_print("tx_samp_freq=%d\n", sampling_freq_hz);
}

/**************************************************************************//***
 * @brief Sets the sampling frequency [Hz].
 *
 * @return None.
*******************************************************************************/
void set_tx_samp_freq(double* param, char param_no) // "tx_samp_freq=" command
{
	uint32_t sampling_freq_hz;

	if(param_no >= 1)
	{
		sampling_freq_hz = (uint32_t)param[0];
		ad9361_set_tx_sampling_freq(ad9361_phy, sampling_freq_hz);
		ad9361_get_tx_sampling_freq(ad9361_phy, &sampling_freq_hz);
		dds_update(ad9361_phy);
		console_print("tx_samp_freq=%d\n", sampling_freq_hz);
	}
	else
		show_invalid_param_message(1);
}

/**************************************************************************//***
 * @brief Gets current TX RF bandwidth [Hz].
 *
 * @return None.
*******************************************************************************/
void get_tx_rf_bandwidth(double* param, char param_no) // "tx_rf_bandwidth?" command
{
	uint32_t bandwidth_hz;

	ad9361_get_tx_rf_bandwidth(ad9361_phy, &bandwidth_hz);
	console_print("tx_rf_bandwidth=%d\n", bandwidth_hz);
}

/**************************************************************************//***
 * @brief Sets the TX RF bandwidth [Hz].
 *
 * @return None.
*******************************************************************************/
void set_tx_rf_bandwidth(double* param, char param_no) // "tx_rf_bandwidth=" command
{
	uint32_t bandwidth_hz;

	if(param_no >= 1)
	{
		bandwidth_hz = param[0];
		ad9361_set_tx_rf_bandwidth(ad9361_phy, bandwidth_hz);
	}
	else
		show_invalid_param_message(1);
}

/**************************************************************************//***
 * @brief Gets current TX1 attenuation [mdB].
 *
 * @return None.
*******************************************************************************/
void get_tx1_attenuation(double* param, char param_no) // "tx1_attenuation?" command
{
	uint32_t attenuation_mdb;

	ad9361_get_tx_attenuation(ad9361_phy, 0, &attenuation_mdb);
	console_print("tx1_attenuation=%d\n", attenuation_mdb);
}

/**************************************************************************//***
 * @brief Sets the TX1 attenuation [mdB].
 *
 * @return None.
*******************************************************************************/
void set_tx1_attenuation(double* param, char param_no) // "tx1_attenuation=" command
{
	uint32_t attenuation_mdb;

	if(param_no >= 1)
	{
		attenuation_mdb = param[0];
		ad9361_set_tx_attenuation(ad9361_phy, 0, attenuation_mdb);
		console_print("tx1_attenuation=%d\n", attenuation_mdb);
	}
	else
		show_invalid_param_message(1);
}

/**************************************************************************//***
 * @brief Gets current TX2 attenuation [mdB].
 *
 * @return None.
*******************************************************************************/
void get_tx2_attenuation(double* param, char param_no) // "tx1_attenuation?" command
{
	uint32_t attenuation_mdb;

	ad9361_get_tx_attenuation(ad9361_phy, 1, &attenuation_mdb);
	console_print("tx2_attenuation=%d\n", attenuation_mdb);
}

/**************************************************************************//***
 * @brief Sets the TX2 attenuation [mdB].
 *
 * @return None.
*******************************************************************************/
void set_tx2_attenuation(double* param, char param_no) // "tx1_attenuation=" command
{
	uint32_t attenuation_mdb;

	if(param_no >= 1)
	{
		attenuation_mdb = param[0];
		ad9361_set_tx_attenuation(ad9361_phy, 1, attenuation_mdb);
		console_print("tx2_attenuation=%d\n", attenuation_mdb);
	}
	else
		show_invalid_param_message(1);
}

/**************************************************************************//***
 * @brief Gets current TX FIR state.
 *
 * @return None.
*******************************************************************************/
void get_tx_fir_en(double* param, char param_no) // "tx_fir_en?" command
{
	uint8_t en_dis;

	ad9361_get_tx_fir_en_dis(ad9361_phy, &en_dis);
	console_print("tx_fir_en=%d\n", en_dis);
}

/**************************************************************************//***
 * @brief Sets the TX FIR state.
 *
 * @return None.
*******************************************************************************/
void set_tx_fir_en(double* param, char param_no) // "tx_fir_en=" command
{
	uint8_t en_dis;

	if(param_no >= 1)
	{
		en_dis = param[0];
		ad9361_set_tx_fir_en_dis(ad9361_phy, en_dis);
		console_print("tx_fir_en=%d\n", en_dis);
	}
	else
		show_invalid_param_message(1);
}

/**************************************************************************//***
 * @brief Gets current RX LO frequency [MHz].
 *
 * @return None.
*******************************************************************************/
void get_rx_lo_freq(double* param, char param_no) // "rx_lo_freq?" command
{
	uint64_t lo_freq_hz;

	ad9361_get_rx_lo_freq(ad9361_phy, &lo_freq_hz);
	lo_freq_hz /= 1000000;
	console_print("rx_lo_freq=%d\n", (uint32_t)lo_freq_hz);
}

/**************************************************************************//***
 * @brief Sets the RX LO frequency [MHz].
 *
 * @return None.
*******************************************************************************/
void set_rx_lo_freq(double* param, char param_no) // "rx_lo_freq=" command
{
	uint64_t lo_freq_hz;

	if(param_no >= 1)
	{
		lo_freq_hz = param[0];
		lo_freq_hz *= 1000000;
		ad9361_set_rx_lo_freq(ad9361_phy, lo_freq_hz);
		lo_freq_hz /= 1000000;
		console_print("rx_lo_freq=%d\n", (uint32_t)lo_freq_hz);
	}
}

/**************************************************************************//***
 * @brief Gets current RX sampling frequency [Hz].
 *
 * @return None.
*******************************************************************************/
void get_rx_samp_freq(double* param, char param_no) // "rx_samp_freq?" command
{
	uint32_t sampling_freq_hz;

	ad9361_get_rx_sampling_freq(ad9361_phy, &sampling_freq_hz);
	console_print("rx_samp_freq=%d\n", sampling_freq_hz);
}

/**************************************************************************//***
 * @brief Sets the RX sampling frequency [Hz].
 *
 * @return None.
*******************************************************************************/
void set_rx_samp_freq(double* param, char param_no) // "rx_samp_freq=" command
{
	uint32_t sampling_freq_hz;

	if(param_no >= 1)
	{
		sampling_freq_hz = (uint32_t)param[0];
		ad9361_set_rx_sampling_freq(ad9361_phy, sampling_freq_hz);
		ad9361_get_rx_sampling_freq(ad9361_phy, &sampling_freq_hz);
		dds_update(ad9361_phy);
		console_print("rx_samp_freq=%d\n", sampling_freq_hz);
	}
	else
		show_invalid_param_message(1);
}

/**************************************************************************//***
 * @brief Gets current RX RF bandwidth [Hz].
 *
 * @return None.
*******************************************************************************/
void get_rx_rf_bandwidth(double* param, char param_no) // "rx_rf_bandwidth?" command
{
	uint32_t bandwidth_hz;

	ad9361_get_rx_rf_bandwidth(ad9361_phy, &bandwidth_hz);
	console_print("rx_rf_bandwidth=%d\n", bandwidth_hz);
}

/**************************************************************************//***
 * @brief Sets the RX RF bandwidth [Hz].
 *
 * @return None.
*******************************************************************************/
void set_rx_rf_bandwidth(double* param, char param_no) // "rx_rf_bandwidth=" command
{
	uint32_t bandwidth_hz;

	if(param_no >= 1)
	{
		bandwidth_hz = param[0];
		ad9361_set_rx_rf_bandwidth(ad9361_phy, bandwidth_hz);
		console_print("rx_rf_bandwidth=%d\n", bandwidth_hz);
	}
	else
		show_invalid_param_message(1);
}

/**************************************************************************//***
 * @brief Gets current RX1 GC mode.
 *
 * @return None.
*******************************************************************************/
void get_rx1_gc_mode(double* param, char param_no) // "rx1_gc_mode?" command
{
	uint8_t gc_mode;

	ad9361_get_rx_gain_control_mode(ad9361_phy, 0, &gc_mode);
	console_print("rx1_gc_mode=%d\n", gc_mode);
}

/**************************************************************************//***
 * @brief Sets the RX1 GC mode.
 *
 * @return None.
*******************************************************************************/
void set_rx1_gc_mode(double* param, char param_no) // "rx1_gc_mode=" command
{
	uint8_t gc_mode;

	if(param_no >= 1)
	{
		gc_mode = param[0];
		ad9361_set_rx_gain_control_mode(ad9361_phy, 0, gc_mode);
		console_print("rx1_gc_mode=%d\n", gc_mode);
	}
	else
		show_invalid_param_message(1);
}

/**************************************************************************//***
 * @brief Gets current RX2 GC mode.
 *
 * @return None.
*******************************************************************************/
void get_rx2_gc_mode(double* param, char param_no) // "rx2_gc_mode?" command
{
	uint8_t gc_mode;

	ad9361_get_rx_gain_control_mode(ad9361_phy, 1, &gc_mode);
	console_print("rx2_gc_mode=%d\n", gc_mode);
}

/**************************************************************************//***
 * @brief Sets the RX2 GC mode.
 *
 * @return None.
*******************************************************************************/
void set_rx2_gc_mode(double* param, char param_no) // "rx2_gc_mode=" command
{
	uint8_t gc_mode;

	if(param_no >= 1)
	{
		gc_mode = param[0];
		ad9361_set_rx_gain_control_mode(ad9361_phy, 1, gc_mode);
		console_print("rx2_gc_mode=%d\n", gc_mode);
	}
	else
		show_invalid_param_message(1);
}

/**************************************************************************//***
 * @brief Gets current RX1 RF gain.
 *
 * @return None.
*******************************************************************************/
void get_rx1_rf_gain(double* param, char param_no) // "rx1_rf_gain?" command
{
	int32_t gain_db;

	ad9361_get_rx_rf_gain (ad9361_phy, 0, &gain_db);
	console_print("rx1_rf_gain=%d\n", gain_db);
}

/**************************************************************************//***
 * @brief Sets the RX1 RF gain.
 *
 * @return None.
*******************************************************************************/
void set_rx1_rf_gain(double* param, char param_no) // "rx1_rf_gain=" command
{
	int32_t gain_db;

	if(param_no >= 1)
	{
		gain_db = param[0];
		ad9361_set_rx_rf_gain (ad9361_phy, 0, gain_db);
		console_print("rx1_rf_gain=%d\n", gain_db);
	}
	else
		show_invalid_param_message(1);
}

/**************************************************************************//***
 * @brief Gets current RX2 RF gain.
 *
 * @return None.
*******************************************************************************/
void get_rx2_rf_gain(double* param, char param_no) // "rx2_rf_gain?" command
{
	int32_t gain_db;

	ad9361_get_rx_rf_gain (ad9361_phy, 1, &gain_db);
	console_print("rx2_rf_gain=%d\n", gain_db);
}

/**************************************************************************//***
 * @brief Sets the RX2 RF gain.
 *
 * @return None.
*******************************************************************************/
void set_rx2_rf_gain(double* param, char param_no) // "rx2_rf_gain=" command
{
	int32_t gain_db;

	if(param_no >= 1)
	{
		gain_db = param[0];
		ad9361_set_rx_rf_gain (ad9361_phy, 1, gain_db);
		console_print("rx2_rf_gain=%d\n", gain_db);
	}
	else
		show_invalid_param_message(1);
}

/**************************************************************************//***
 * @brief Gets current RX FIR state.
 *
 * @return None.
*******************************************************************************/
void get_rx_fir_en(double* param, char param_no) // "rx_fir_en?" command
{
	uint8_t en_dis;

	ad9361_get_rx_fir_en_dis(ad9361_phy, &en_dis);
	console_print("rx_fir_en=%d\n", en_dis);
}

/**************************************************************************//***
 * @brief Sets the RX FIR state.
 *
 * @return None.
*******************************************************************************/
void set_rx_fir_en(double* param, char param_no) // "rx_fir_en=" command
{
	uint8_t en_dis;

	if(param_no >= 1)
	{
		en_dis = param[0];
		ad9361_set_rx_fir_en_dis(ad9361_phy, en_dis);
		console_print("rx_fir_en=%d\n", en_dis);
	}
	else
		show_invalid_param_message(1);
}

/**************************************************************************//***
 * @brief Gets current DDS TX1 Tone 1 frequency [Hz].
 *
 * @return None.
*******************************************************************************/
void get_dds_tx1_tone1_freq(double* param, char param_no)	// dds_tx1_tone1_freq?
{
	uint32_t freq = dds_st.cached_freq[DDS_CHAN_TX1_I_F1];

	console_print("dds_tx1_tone1_freq=%d\n", freq);
}

/**************************************************************************//***
 * @brief Sets the DDS TX1 Tone 1 frequency [Hz].
 *
 * @return None.
*******************************************************************************/
void set_dds_tx1_tone1_freq(double* param, char param_no)	// dds_tx1_tone1_freq=
{
	uint32_t freq = (uint32_t)param[0];

	if(param_no >= 1)
	{
		dds_set_frequency(ad9361_phy, DDS_CHAN_TX1_I_F1, freq);
		dds_set_frequency(ad9361_phy, DDS_CHAN_TX1_Q_F1, freq);
		console_print("dds_tx1_tone1_freq=%d\n", freq);
	}
	else
		show_invalid_param_message(1);
}

/**************************************************************************//***
 * @brief Gets current DDS TX1 Tone 2 frequency [Hz].
 *
 * @return None.
*******************************************************************************/
void get_dds_tx1_tone2_freq(double* param, char param_no)	// dds_tx1_tone2_freq?
{
	uint32_t freq = dds_st.cached_freq[DDS_CHAN_TX1_I_F2];

	console_print("dds_tx1_tone2_freq=%d\n", freq);
}

/**************************************************************************//***
 * @brief Sets the DDS TX1 Tone 2 frequency [Hz].
 *
 * @return None.
*******************************************************************************/
void set_dds_tx1_tone2_freq(double* param, char param_no)	// dds_tx1_tone2_freq=
{
	uint32_t freq = (uint32_t)param[0];

	if(param_no >= 1)
	{
		dds_set_frequency(ad9361_phy, DDS_CHAN_TX1_I_F2, freq);
		dds_set_frequency(ad9361_phy, DDS_CHAN_TX1_Q_F2, freq);
		console_print("dds_tx1_tone2_freq=%d\n", freq);
	}
	else
		show_invalid_param_message(1);
}

/**************************************************************************//***
 * @brief Gets current DDS TX1 Tone 1 phase [degrees].
 *
 * @return None.
*******************************************************************************/
void get_dds_tx1_tone1_phase(double* param, char param_no)	// dds_tx1_tone1_phase?
{
	uint32_t phase = dds_st.cached_phase[DDS_CHAN_TX1_I_F1];

	phase /= 1000;
	console_print("dds_tx1_tone1_phase=%d\n", phase);
}

/**************************************************************************//***
 * @brief Sets the DDS TX1 Tone 1 phase [degrees].
 *
 * @return None.
*******************************************************************************/
void set_dds_tx1_tone1_phase(double* param, char param_no)	// dds_tx1_tone1_phase=
{
	int32_t phase = (uint32_t)param[0];

	if(param_no >= 1)
	{
		dds_set_phase(ad9361_phy, DDS_CHAN_TX1_I_F1, (uint32_t)(phase * 1000));
		if ((phase - 90) < 0)
			phase += 360;
		dds_set_phase(ad9361_phy, DDS_CHAN_TX1_Q_F1, (uint32_t)((phase - 90) * 1000));
		phase = dds_st.cached_phase[DDS_CHAN_TX1_I_F1] / 1000;
		console_print("dds_tx1_tone1_phase=%d\n", phase);
	}
	else
		show_invalid_param_message(1);
}

/**************************************************************************//***
 * @brief Gets current DDS TX1 Tone 2 phase [degrees].
 *
 * @return None.
*******************************************************************************/
void get_dds_tx1_tone2_phase(double* param, char param_no)	// dds_tx1_tone2_phase?
{
	uint32_t phase = dds_st.cached_phase[DDS_CHAN_TX1_I_F2];

	phase /= 1000;
	console_print("dds_tx1_tone2_phase=%d\n", phase);
}

/**************************************************************************//***
 * @brief Sets the DDS TX1 Tone 2 phase [degrees].
 *
 * @return None.
*******************************************************************************/
void set_dds_tx1_tone2_phase(double* param, char param_no)	// dds_tx1_tone2_phase=
{
	int32_t phase = (uint32_t)param[0];

	if(param_no >= 1)
	{
		dds_set_phase(ad9361_phy, DDS_CHAN_TX1_I_F2, (uint32_t)(phase * 1000));
		if ((phase - 90) < 0)
			phase += 360;
		dds_set_phase(ad9361_phy, DDS_CHAN_TX1_Q_F2, (uint32_t)((phase - 90) * 1000));
		phase = dds_st.cached_phase[DDS_CHAN_TX1_I_F2] / 1000;
		console_print("dds_tx1_tone2_phase=%d\n", phase);
	}
	else
		show_invalid_param_message(1);
}

/**************************************************************************//***
 * @brief Gets current DDS TX1 Tone 1 scale.
 *
 * @return None.
*******************************************************************************/
void get_dds_tx1_tone1_scale(double* param, char param_no)	// dds_tx1_tone1_scale?
{
	int32_t scale = dds_st.cached_scale[DDS_CHAN_TX1_I_F1];

	console_print("dds_tx1_tone1_scale=%d\n", scale);
}

/**************************************************************************//***
 * @brief Sets the DDS TX1 Tone 1 scale.
 *
 * @return None.
*******************************************************************************/
void set_dds_tx1_tone1_scale(double* param, char param_no)	// dds_tx1_tone1_scale=
{
	int32_t scale = (int32_t)param[0];

	if(param_no >= 1)
	{
		dds_set_scale(ad9361_phy, DDS_CHAN_TX1_I_F1, scale);
		dds_set_scale(ad9361_phy, DDS_CHAN_TX1_Q_F1, scale);
		scale = dds_st.cached_scale[DDS_CHAN_TX1_I_F1];
		console_print("dds_tx1_tone1_scale=%d\n", scale);
	}
	else
		show_invalid_param_message(1);
}

/**************************************************************************//***
 * @brief Gets current DDS TX1 Tone 2 scale.
 *
 * @return None.
*******************************************************************************/
void get_dds_tx1_tone2_scale(double* param, char param_no)	// dds_tx1_tone2_scale?
{
	int32_t scale = dds_st.cached_scale[DDS_CHAN_TX1_I_F2];

	console_print("dds_tx1_tone2_scale=%d\n", scale);
}

/**************************************************************************//***
 * @brief Sets the DDS TX1 Tone 2 scale.
 *
 * @return None.
*******************************************************************************/
void set_dds_tx1_tone2_scale(double* param, char param_no)	// dds_tx1_tone2_scale=
{
	int32_t scale = (int32_t)param[0];

	if(param_no >= 1)
	{
		dds_set_scale(ad9361_phy, DDS_CHAN_TX1_I_F2, scale);
		dds_set_scale(ad9361_phy, DDS_CHAN_TX1_Q_F2, scale);
		scale = dds_st.cached_scale[DDS_CHAN_TX1_I_F2];
		console_print("dds_tx1_tone2_scale=%d\n", scale);
	}
	else
		show_invalid_param_message(1);
}

/**************************************************************************//***
 * @brief Gets current DDS TX2 Tone 1 frequency [Hz].
 *
 * @return None.
*******************************************************************************/
void get_dds_tx2_tone1_freq(double* param, char param_no)	// dds_tx2_tone1_freq?
{
	uint32_t freq = dds_st.cached_freq[DDS_CHAN_TX2_I_F1];

	console_print("dds_tx2_tone1_freq=%d\n", freq);
}

/**************************************************************************//***
 * @brief Sets the DDS TX2 Tone 1 frequency [Hz].
 *
 * @return None.
*******************************************************************************/
void set_dds_tx2_tone1_freq(double* param, char param_no)	// dds_tx2_tone1_freq=
{
	uint32_t freq = (uint32_t)param[0];

	if(param_no >= 1)
	{
		dds_set_frequency(ad9361_phy, DDS_CHAN_TX2_I_F1, freq);
		dds_set_frequency(ad9361_phy, DDS_CHAN_TX2_Q_F1, freq);
		console_print("dds_tx2_tone1_freq=%d\n", freq);
	}
	else
		show_invalid_param_message(1);
}

/**************************************************************************//***
 * @brief Gets current DDS TX2 Tone 2 frequency [Hz].
 *
 * @return None.
*******************************************************************************/
void get_dds_tx2_tone2_freq(double* param, char param_no)	// dds_tx2_tone2_freq?
{
	uint32_t freq = dds_st.cached_freq[DDS_CHAN_TX2_I_F2];

	console_print("dds_tx2_tone2_freq=%d\n", freq);
}

/**************************************************************************//***
 * @brief Sets the DDS TX2 Tone 2 frequency [Hz].
 *
 * @return None.
*******************************************************************************/
void set_dds_tx2_tone2_freq(double* param, char param_no)	// dds_tx2_tone2_freq=
{
	uint32_t freq = (uint32_t)param[0];

	if(param_no >= 1)
	{
		dds_set_frequency(ad9361_phy, DDS_CHAN_TX2_I_F2, freq);
		dds_set_frequency(ad9361_phy, DDS_CHAN_TX2_Q_F2, freq);
		console_print("dds_tx2_tone2_freq=%d\n", freq);
	}
	else
		show_invalid_param_message(1);
}

/**************************************************************************//***
 * @brief Gets current DDS TX2 Tone 1 phase [degrees].
 *
 * @return None.
*******************************************************************************/
void get_dds_tx2_tone1_phase(double* param, char param_no)	// dds_tx2_tone1_phase?
{
	uint32_t phase = dds_st.cached_phase[DDS_CHAN_TX2_I_F1];

	phase /= 1000;
	console_print("dds_tx2_tone1_phase=%d\n", phase);
}

/**************************************************************************//***
 * @brief Sets the DDS TX2 Tone 1 phase [degrees].
 *
 * @return None.
*******************************************************************************/
void set_dds_tx2_tone1_phase(double* param, char param_no)	// dds_tx2_tone1_phase=
{
	int32_t phase = (uint32_t)param[0];

	if(param_no >= 1)
	{
		dds_set_phase(ad9361_phy, DDS_CHAN_TX2_I_F1, (uint32_t)(phase * 1000));
		if ((phase - 90) < 0)
			phase += 360;
		dds_set_phase(ad9361_phy, DDS_CHAN_TX2_Q_F1, (uint32_t)((phase - 90) * 1000));
		phase = dds_st.cached_phase[DDS_CHAN_TX2_I_F1] / 1000;
		console_print("dds_tx2_tone1_phase=%d\n", phase);
	}
	else
		show_invalid_param_message(1);
}

/**************************************************************************//***
 * @brief Gets current DDS TX2 Tone 2 phase [degrees].
 *
 * @return None.
*******************************************************************************/
void get_dds_tx2_tone2_phase(double* param, char param_no)	// dds_tx2_tone2_phase?
{
	uint32_t phase = dds_st.cached_phase[DDS_CHAN_TX2_I_F2];

	phase /= 1000;
	console_print("dds_tx2_f2_phase=%d\n", phase);
}

/**************************************************************************//***
 * @brief Sets the DDS TX2 Tone 2 phase [degrees].
 *
 * @return None.
*******************************************************************************/
void set_dds_tx2_tone2_phase(double* param, char param_no)	// dds_tx2_tone2_phase=
{
	int32_t phase = (uint32_t)param[0];

	if(param_no >= 1)
	{
		dds_set_phase(ad9361_phy, DDS_CHAN_TX2_I_F2, (uint32_t)(phase * 1000));
		if ((phase - 90) < 0)
			phase += 360;
		dds_set_phase(ad9361_phy, DDS_CHAN_TX2_Q_F2, (uint32_t)((phase - 90) * 1000));
		phase = dds_st.cached_phase[DDS_CHAN_TX2_I_F2] / 1000;
		console_print("dds_tx2_tone2_phase=%d\n", phase);
	}
	else
		show_invalid_param_message(1);
}

/**************************************************************************//***
 * @brief Gets current DDS TX2 Tone 1 scale.
 *
 * @return None.
*******************************************************************************/
void get_dds_tx2_tone1_scale(double* param, char param_no)	// dds_tx2_tone1_scale?
{
	int32_t scale = dds_st.cached_scale[DDS_CHAN_TX2_I_F1];

	console_print("dds_tx2_tone1_scale=%d\n", scale);
}

/**************************************************************************//***
 * @brief Sets the DDS TX2 Tone 1 scale.
 *
 * @return None.
*******************************************************************************/
void set_dds_tx2_tone1_scale(double* param, char param_no)	// dds_tx2_tone1_scale=
{
	int32_t scale = (int32_t)param[0];

	if(param_no >= 1)
	{
		dds_set_scale(ad9361_phy, DDS_CHAN_TX2_I_F1, scale);
		dds_set_scale(ad9361_phy, DDS_CHAN_TX2_Q_F1, scale);
		scale = dds_st.cached_scale[DDS_CHAN_TX2_I_F1];
		console_print("dds_tx2_tone1_scale=%d\n", scale);
	}
	else
		show_invalid_param_message(1);
}

/**************************************************************************//***
 * @brief Gets current DDS TX2 Tone 2 scale.
 *
 * @return None.
*******************************************************************************/
void dds_tx2_tone2_scale(double* param, char param_no)	// dds_tx2_tone2_scale?
{
	int32_t scale = dds_st.cached_scale[DDS_CHAN_TX2_I_F2];

	console_print("dds_tx2_tone2_scale=%d\n", scale);
}

/**************************************************************************//***
 * @brief Sets the DDS TX2 Tone 2 scale.
 *
 * @return None.
*******************************************************************************/
void set_dds_tx2_tone2_scale(double* param, char param_no)	// dds_tx2_tone2_scale=
{
	int32_t scale = (int32_t)param[0];

	if(param_no >= 1)
	{
		dds_set_scale(ad9361_phy, DDS_CHAN_TX2_I_F2, scale);
		dds_set_scale(ad9361_phy, DDS_CHAN_TX2_Q_F2, scale);
		scale = dds_st.cached_scale[DDS_CHAN_TX2_I_F2];
		console_print("dds_tx2_tone2_scale=%d\n", scale);
	}
	else
		show_invalid_param_message(1);
}
