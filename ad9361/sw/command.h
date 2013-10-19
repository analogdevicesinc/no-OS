/**************************************************************************//**
 *   @file   command.h
 *   @brief  Header file of AD9361 Command Driver.
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
#ifndef __COMMAND_H__
#define __COMMAND_H__

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define NULL		((void *)0)
#define SUCCESS		0
#define ERROR		-1

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
typedef void (*cmd_function)(double* param, char param_no);
typedef struct
{
	const char* name;
	const char* description;
	const char* example;
	cmd_function function;
}command;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Displays all available commands. */
void get_help(double* param, char param_no);

/* Gets the specified register value. */
void get_register(double* param, char param_no);

/* Gets current TX LO frequency. */
void get_tx_lo_freq(double* param, char param_no);

/* Sets the TX LO frequency. */
void set_tx_lo_freq(double* param, char param_no);

/* Gets current TX sampling frequency. */
void get_tx_samp_freq(double* param, char param_no);

/* Sets the TX sampling frequency. */
void set_tx_samp_freq(double* param, char param_no);

/* Gets current TX RF bandwidth. */
void get_tx_rf_bandwidth(double* param, char param_no);

/* Sets the TX RF bandwidth. */
void set_tx_rf_bandwidth(double* param, char param_no);

/* Gets current TX1 attenuation. */
void get_tx1_attenuation(double* param, char param_no);

/* Sets the TX1 attenuation. */
void set_tx1_attenuation(double* param, char param_no);

/* Gets current TX2 attenuation. */
void get_tx2_attenuation(double* param, char param_no);

/* Sets the TX2 attenuation. */
void set_tx2_attenuation(double* param, char param_no);

/* Gets current TX FIR state. */
void get_tx_fir_en(double* param, char param_no);

/* Sets the TX FIR state. */
void set_tx_fir_en(double* param, char param_no);

/* Gets current RX LO frequency. */
void get_rx_lo_freq(double* param, char param_no);

/* Sets the RX LO frequency. */
void set_rx_lo_freq(double* param, char param_no);

/* Gets current RX sampling frequency. */
void get_rx_samp_freq(double* param, char param_no);

/* Sets the RX sampling frequency. */
void set_rx_samp_freq(double* param, char param_no);

/* Gets current RX RF bandwidth. */
void get_rx_rf_bandwidth(double* param, char param_no);

/* Sets the RX RF bandwidth. */
void set_rx_rf_bandwidth(double* param, char param_no);

/* Gets current RX1 GC mode. */
void get_rx1_gc_mode(double* param, char param_no);

/* Sets the RX1 GC mode. */
void set_rx1_gc_mode(double* param, char param_no);

/* Gets current RX2 GC mode. */
void get_rx2_gc_mode(double* param, char param_no);

/* Sets the RX2 GC mode. */
void set_rx2_gc_mode(double* param, char param_no);

/* Gets current RX1 RF gain. */
void get_rx1_rf_gain(double* param, char param_no);

/* Sets the RX1 RF gain. */
void set_rx1_rf_gain(double* param, char param_no);

/* Gets current RX2 RF gain. */
void get_rx2_rf_gain(double* param, char param_no);

/* Sets the RX2 RF gain. */
void set_rx2_rf_gain(double* param, char param_no);

/* Gets current RX FIR state. */
void get_rx_fir_en(double* param, char param_no);

/* Sets the RX FIR state. */
void set_rx_fir_en(double* param, char param_no);

/* Gets current DDS TX1 Tone 1 frequency [Hz]. */
void get_dds_tx1_tone1_freq(double* param, char param_no);

/* Sets the DDS TX1 Tone 1 frequency [Hz]. */
void set_dds_tx1_tone1_freq(double* param, char param_no);

/* Gets current DDS TX1 Tone 2 frequency [Hz]. */
void get_dds_tx1_tone2_freq(double* param, char param_no);

/* Sets the DDS TX1 Tone 2 frequency [Hz]. */
void set_dds_tx1_tone2_freq(double* param, char param_no);

/* Gets current DDS TX1 Tone 1 phase [degrees]. */
void get_dds_tx1_tone1_phase(double* param, char param_no);

/* Sets the DDS TX1 Tone 1 phase [degrees]. */
void set_dds_tx1_tone1_phase(double* param, char param_no);

/* Gets current DDS TX1 Tone 2 phase [degrees]. */
void get_dds_tx1_tone2_phase(double* param, char param_no);

/* Sets the DDS TX1 Tone 2 phase [degrees]. */
void set_dds_tx1_tone2_phase(double* param, char param_no);

/* Gets current DDS TX1 Tone 1 scale. */
void get_dds_tx1_tone1_scale(double* param, char param_no);

/* Sets the DDS TX1 Tone 1 scale. */
void set_dds_tx1_tone1_scale(double* param, char param_no);

/* Gets current DDS TX1 Tone 2 scale. */
void get_dds_tx1_tone2_scale(double* param, char param_no);

/* Sets the DDS TX1 Tone 2 scale. */
void set_dds_tx1_tone2_scale(double* param, char param_no);

/* Gets current DDS TX2 Tone 1 frequency [Hz]. */
void get_dds_tx2_tone1_freq(double* param, char param_no);

/* Sets the DDS TX2 Tone 1 frequency [Hz]. */
void set_dds_tx2_tone1_freq(double* param, char param_no);

/* Gets current DDS TX2 Tone 2 frequency [Hz]. */
void get_dds_tx2_tone2_freq(double* param, char param_no);

/* Sets the DDS TX2 Tone 2 frequency [Hz]. */
void set_dds_tx2_tone2_freq(double* param, char param_no);

/* Gets current DDS TX2 Tone 1 phase [degrees]. */
void get_dds_tx2_tone1_phase(double* param, char param_no);

/* Sets the DDS TX2 Tone 1 phase [degrees]. */
void set_dds_tx2_tone1_phase(double* param, char param_no);

/* Gets current DDS TX2 Tone 2 phase [degrees]. */
void get_dds_tx2_tone2_phase(double* param, char param_no);

/* Sets the DDS TX2 Tone 2 phase [degrees]. */
void set_dds_tx2_tone2_phase(double* param, char param_no);

/* Gets current DDS TX2 Tone 1 scale. */
void get_dds_tx2_tone1_scale(double* param, char param_no);

/* Sets the DDS TX2 Tone 1 scale. */
void set_dds_tx2_tone1_scale(double* param, char param_no);

/* Gets current DDS TX2 Tone 2 scale. */
void dds_tx2_tone2_scale(double* param, char param_no);

/* Sets the DDS TX2 Tone 2 scale. */
void set_dds_tx2_tone2_scale(double* param, char param_no);

#endif  // __COMMAND_H__
