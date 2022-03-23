/***************************************************************************//**
 *   @file   main.c
 *   @brief  The implementation of the main function of the app.
 *   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
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
#include <stdio.h>
#include <xil_printf.h>
#include <xiic.h>
#include <xil_cache.h>
#include "tx_lib.h"
#include "cf_hdmi.h"
#include <stdlib.h>
#include "transmitter.h"
#include "wrapper.h"
#include "tx_isr.h"
#include "app_config.h"
#include "no_os_i2c.h"
#include "i2c_extra.h"
#include "no_os_irq.h"
#include "irq_extra.h"
#include "no_os_timer.h"
#include "timer_extra.h"
#include "clk_axi_clkgen.h"
#if defined(PLATFORM_ZC702) || defined(PLATFORM_ZC706) || \
		defined(PLATFORM_ZED)
#include <xuartps.h>
#elif defined(PLATFORM_KC705) || defined(PLATFORM_AC701) || \
	defined(PLATFORM_VC707)
#include <xuartlite.h>
#include <xuartlite_l.h>
#endif

#define HDMI_CALL_INTERVAL_MS 10

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/

static uint8_t    major_rev;      /* Major Release Number */
static uint8_t    minor_rev;      /* Usually used for code-drops */
static uint8_t    rc_rev;         /* Release Candidate Number */
static bool     driver_enable;
static bool     last_enable;
extern struct no_os_i2c_desc *i2c_handler;
extern volatile uint32_t timer_counter_intr;

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

#if defined(XSCUTIMER_H)
void timer_isr(void *instance)
#elif defined(XTMRCTR_H)
void timer_isr(void *instance, uint8_t timer_nr)
#endif
{
	timer_counter_intr++;
}

/**
 * Enables the driver.
 *
 * @param [in] enable - true to enable the driver;
 *                      flase to disable the driver.
 *
 * @return void
 */
void app_enable_driver(bool enable)
{
	driver_enable = enable;
}

/**
 * Returns the driver enable status.
 *
 * @param void
 *
 * @return true if the driver is enabled, false if it's disabled.
 */
static bool app_driver_enabled(void)
{
	if ((driver_enable && HAL_GetMBSwitchState()) != last_enable) {
		last_enable = driver_enable && HAL_GetMBSwitchState();
		DBG_MSG("APP: Driver %s\n\r", last_enable? "Enabled": "Disabled");
	}

	return last_enable;
}

/**
 * Displays the application version and the chip revision.
 *
 * @param void
 *
 * @return void
 */
static void app_print_revisions(void)
{
	UINT16 tx_rev;

	ADIAPI_TxGetChipRevision(&tx_rev);

	DBG_MSG("\n\r********************************************************************\r\n");
	DBG_MSG("  ADI HDMI Trasmitter Application Ver R%d.%d.%d\n\r", major_rev,
		minor_rev, rc_rev);
	DBG_MSG("  HDMI-TX:  ADV7511 Rev 0x%x\r\n", tx_rev);
	DBG_MSG("  Created:  %s At %s\n\r", __DATE__, __TIME__);
	DBG_MSG("********************************************************************\r\n\n\r");
}

/**
 * Changes the video resolution.
 *
 * @param void
 *
 * @return void
 */
static void app_change_resolution(struct axi_clkgen *clk_gen_core)
{
	char *resolutions[7] = {
		"640x480", "800x600", "1024x768", "1280x720", "1360x768",
		"1600x900", "1920x1080"
	};
	char received_char;

#if defined(_XPARAMETERS_PS_H_)
	if(XUartPs_IsReceiveData(UART_BASEADDR)) {
#else
	if(!XUartLite_IsReceiveEmpty(UART_BASEADDR)) {
#endif
		received_char = inbyte();
		if((received_char >= 0x30) && (received_char <= 0x36)) {
			SetVideoResolution(clk_gen_core, (received_char - 0x30));
			DBG_MSG("Resolution was changed to %s \r\n",
				resolutions[received_char - 0x30]);
		} else {
			if((received_char != 0x0A) && (received_char != 0x0D)) {
				SetVideoResolution(clk_gen_core, RESOLUTION_640x480);
				DBG_MSG("Resolution was changed to %s \r\n", resolutions[0]);
			}
		}
	}
}

/**
 * For the KC705 and ZC702 boards an I2C mux must be set before the ADV7511 can
 * be accessed.
 *
 * @param [in] adv7511_i2c - Pointer to the I2C handler.
 *
 * @return 0 in case of success, error code otherwise.
 */
static int32_t app_set_i2c_mux(struct no_os_i2c_desc *adv7511_i2c)
{
#if !defined(PLATFORM_ZED)
#if defined(PLATFORM_KC705) || defined(PLATFORM_VC707) || \
	defined(PLATFORM_AC701)
	uint8_t pca9548_setup = 0x20;
#elif defined(PLATFORM_ZC706) || defined(PLATFORM_ZC702)
	uint8_t pca9548_setup = 0x02;
#endif
	uint8_t mem_val;
	int32_t ret;
	const uint8_t mux_addr = 0x74;
	const uint8_t byte_transfer_no = 1, stop_bit = 1;
	struct no_os_i2c_desc *i2c_mux;
	struct no_os_i2c_init_param i2c_mux_init;
	struct xil_i2c_init_param i2c_mux_init_extra;

	i2c_mux_init_extra.device_id = XPAR_AXI_IIC_MAIN_DEVICE_ID;
	i2c_mux_init_extra.type = IIC_PL;
	i2c_mux_init.max_speed_hz = 400000;
	i2c_mux_init.slave_address = mux_addr;
	i2c_mux_init.platform_ops = &xil_i2c_ops;
	i2c_mux_init.extra = &i2c_mux_init_extra;

	mem_val = pca9548_setup;

	ret = no_os_i2c_init(&i2c_mux, &i2c_mux_init);
	if(ret != 0)
		return ret;
	ret = no_os_i2c_write(i2c_mux, &pca9548_setup, byte_transfer_no, stop_bit);
	if(ret != 0)
		return ret;
	pca9548_setup = 0xdd;
	ret = no_os_i2c_read(i2c_mux, &pca9548_setup, byte_transfer_no, stop_bit);
	if(ret != 0)
		return ret;
	if(pca9548_setup != mem_val)
		return -1;
	return no_os_i2c_remove(i2c_mux);

	return ret;
#else
	return 0;
#endif
}

/**
 * Initialize the I2C, timer and interrupt controller drivers and hardware.
 *
 * @param [in] adv7511_i2c_init - Pointer to the initialization structure for
 *                                the I2C.
 * @param [in] timer_init - Pointer to the initialization structure for the
 *                          timer.
 * @param [in] gic_init - Pointer to the initialization structure for the
 *                        interrupt controller.
 * @param [out] adv7511_i2c - Pointer to the I2C pointer handler.
 * @param [out] timer_inst_ptr - Pointer to the timer pointer handler.
 * @param [out] gic_inst_ptr - Pointer to the interrupt controller pointer
 *                             handler.
 *
 * @return 0 in case of success, error code otherwise.
 */
static int32_t hal_platform_init(struct no_os_i2c_desc **adv7511_i2c,
				 struct no_os_i2c_init_param *adv7511_i2c_init,
				 struct no_os_timer_desc **timer_inst_ptr,
				 struct no_os_timer_init_param *timer_inits,
				 struct no_os_irq_ctrl_desc **gic_inst_ptr,
				 struct no_os_irq_init_param *gic_init)
{
	int32_t ret;
	struct xil_timer_desc *xil_tmr;
	struct xil_i2c_desc *ps_i2c_extra;
#if defined(_XPARAMETERS_PS_H_)
	const uint8_t timer_int_nr = XPAR_SCUTIMER_INTR;
	const uint8_t i2c_int_nr = XPAR_XIICPS_1_INTR;
#else
	const uint8_t timer_int_nr = XPAR_AXI_INTC_AXI_TIMER_INTERRUPT_INTR;
	const uint8_t i2c_int_nr = XPAR_AXI_INTC_AXI_IIC_MAIN_IIC2INTC_IRPT_INTR;
#endif
	struct no_os_callback_desc cb_desc_temp;

	ret = no_os_timer_init(timer_inst_ptr, timer_inits);
	if(ret != 0)
		return ret;
	xil_tmr = (*timer_inst_ptr)->extra;
	ret = no_os_i2c_init(adv7511_i2c, adv7511_i2c_init);
	if(ret != 0)
		return ret;
	ps_i2c_extra = (*adv7511_i2c)->extra;

	ret = no_os_irq_ctrl_init(gic_inst_ptr, gic_init);
	if(ret != 0)
		return ret;
	no_os_irq_global_enable(*gic_inst_ptr);
#if defined(_XPARAMETERS_PS_H_)
	cb_desc_temp.legacy_callback = timer_isr;
	cb_desc_temp.legacy_config = NULL;
	cb_desc_temp.ctx = xil_tmr->instance;
	ret = no_os_irq_register_callback(*gic_inst_ptr, timer_int_nr, &cb_desc_temp);
#else
	cb_desc_temp.legacy_callback = XTmrCtr_InterruptHandler;
	cb_desc_temp.legacy_config = NULL;
	cb_desc_temp.ctx = xil_tmr->instance;
	ret = no_os_irq_register_callback(*gic_inst_ptr, timer_int_nr, &cb_desc_temp);
	XTmrCtr_SetHandler(xil_tmr->instance, timer_isr, xil_tmr->instance);
#endif
	if(ret != 0)
		return ret;
	ret = no_os_irq_enable(*gic_inst_ptr, timer_int_nr);
	if(ret != 0)
		return ret;
	cb_desc_temp.legacy_callback = XIic_InterruptHandler;
	cb_desc_temp.legacy_config = NULL;
	cb_desc_temp.ctx = &ps_i2c_extra->instance;
	ret = no_os_irq_register_callback(*gic_inst_ptr, i2c_int_nr, &cb_desc_temp);
	if(ret != 0)
		return ret;
	ret = no_os_irq_enable(*gic_inst_ptr, i2c_int_nr);
	if(ret != 0)
		return ret;

	no_os_timer_start(*timer_inst_ptr);

	i2c_handler = *adv7511_i2c;

	app_set_i2c_mux(*adv7511_i2c);

	return ret;
}

/**
 * Main function of the application.
 *
 * @param none
 *
 * @return 0 in case of success, non-zero error code otherwise
 */
int main()
{
	uint32_t start_count;
	struct no_os_i2c_desc *adv7511_i2c;
	struct no_os_i2c_init_param adv7511_i2c_init;
	struct xil_i2c_init_param adv7511_extra_i2c_init;
	struct no_os_timer_desc *timer_inst_ptr;
	struct no_os_timer_init_param timer_init;
	struct xil_timer_init_param xil_timer_init;
	struct no_os_irq_ctrl_desc *gic_inst_ptr;
	struct no_os_irq_init_param gic_init;
	struct xil_irq_init_param gic_init_extra;
	struct axi_clkgen *clk_gen_core;
	struct axi_clkgen_init clk_gen_core_initial = {
		.name = "adv7511_clk_gen",
#ifdef XPAR_AXI_CLKGEN_0_BASEADDR
		.base = XPAR_AXI_CLKGEN_0_BASEADDR,
#else
		.base = XPAR_AXI_HDMI_CLKGEN_BASEADDR,
#endif
		.parent_rate = 200000000
	};
	int32_t ret;

	adv7511_extra_i2c_init.device_id = XPAR_AXI_IIC_MAIN_DEVICE_ID;
	adv7511_extra_i2c_init.type = IIC_PL;
	adv7511_i2c_init.max_speed_hz = 400000;
	adv7511_i2c_init.slave_address = 0x39;
	adv7511_i2c_init.platform_ops = &xil_i2c_ops;
	adv7511_i2c_init.extra = &adv7511_extra_i2c_init;
#if defined(_XPARAMETERS_PS_H_)
	xil_timer_init.active_tmr = 0;
	xil_timer_init.type = TIMER_PS;
	timer_init.id = XPAR_XSCUTIMER_0_DEVICE_ID;
	timer_init.freq_hz = XPAR_CPU_CORTEXA9_CORE_CLOCK_FREQ_HZ / 2;
	timer_init.load_value = timer_init.freq_hz / 1000;
	timer_init.extra = &xil_timer_init;
#else
	xil_timer_init.active_tmr = 0;
	xil_timer_init.type = TIMER_PL;
	timer_init.id = XPAR_AXI_TIMER_DEVICE_ID;
	timer_init.freq_hz = XPAR_AXI_TIMER_CLOCK_FREQ_HZ;
	timer_init.load_value = timer_init.freq_hz / 1000;
	timer_init.extra = &xil_timer_init;
#endif
#if defined(_XPARAMETERS_PS_H_)
	gic_init_extra.type = IRQ_PS;
	gic_init.irq_ctrl_id = XPAR_SCUGIC_0_DEVICE_ID;
#else
	gic_init_extra.type = IRQ_PL;
	gic_init.irq_ctrl_id = XPAR_INTC_0_DEVICE_ID;
#endif
	gic_init.extra = &gic_init_extra;

	major_rev     = 1;
	minor_rev     = 1;
	rc_rev        = 1;
	driver_enable = true;
	last_enable   = false;

	Xil_ICacheEnable();
	Xil_DCacheEnable();

	ret = hal_platform_init(&adv7511_i2c, &adv7511_i2c_init, &timer_inst_ptr,
				&timer_init, &gic_inst_ptr, &gic_init);
	if(ret != 0)
		return ret;
	ret = axi_clkgen_init(&clk_gen_core, &clk_gen_core_initial);
	if(ret != 0)
		return ret;
	transmitter_link_clkgen(clk_gen_core);

	SetVideoResolution(clk_gen_core, RESOLUTION_640x480);
	InitHdmiAudioPcore();

	app_print_revisions();       /* Display S/W and H/W revisions */

	DBG_MSG("To change the video resolution press:\r\n");
	DBG_MSG("  '0' - 640x480;  '1' - 800x600;  '2' - 1024x768; '3' - 1280x720 \r\n");
	DBG_MSG("  '4' - 1360x768; '5' - 1600x900; '6' - 1920x1080.\r\n");

	ADIAPI_TransmitterInit();   /* Initialize ADI repeater software and h/w */

	ADIAPI_TransmitterSetPowerMode(REP_POWER_UP);

	start_count = HAL_GetCurrentMsCount();

	while(1) {
		if (ATV_GetElapsedMs (start_count, NULL) >= HDMI_CALL_INTERVAL_MS) {
			start_count = HAL_GetCurrentMsCount();
			if (app_driver_enabled())
				ADIAPI_TransmitterMain();
		}
		app_change_resolution(clk_gen_core);
	}

	Xil_DCacheDisable();
	Xil_ICacheDisable();

	return 0;
}
