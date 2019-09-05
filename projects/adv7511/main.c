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
#include "platform_drivers.h"
#include "tx_lib.h"
#include "cf_hdmi.h"
#include <stdlib.h>
#include "timer.h"
#include "transmitter.h"
#include "wrapper.h"
#include "tx_isr.h"
#include "config.h"

#define HDMI_CALL_INTERVAL_MS 10

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
static uint8_t    major_rev;      /* Major Release Number */
static uint8_t    minor_rev;      /* Usually used for code-drops */
static uint8_t    rc_rev;         /* Release Candidate Number */
static bool     driver_enable;
static bool     last_enable;
extern struct i2c_desc *i2c_handler;
extern volatile uint32_t timer_counter_intr;

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
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
static void app_change_resolution(void)
{
	char *resolutions[7] = {
		"640x480", "800x600", "1024x768", "1280x720", "1360x768",
		"1600x900", "1920x1080"
	};
	char received_char    = 0;

#if defined(_XPARAMETERS_PS_H_)
	if(XUartPs_IsReceiveData(UART_BASEADDR)) {
#else
	if(!XUartLite_IsReceiveEmpty(UART_BASEADDR)) {
#endif
		received_char = inbyte();
		if((received_char >= 0x30) && (received_char <= 0x36)) {
			SetVideoResolution(received_char - 0x30);
			DBG_MSG("Resolution was changed to %s \r\n",
				resolutions[received_char - 0x30]);
		} else {
			if((received_char != 0x0A) && (received_char != 0x0D)) {
				SetVideoResolution(RESOLUTION_640x480);
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
static int32_t app_set_i2c_mux(struct i2c_desc *adv7511_i2c)
{
#if !(CURRENT_PLATFORM == PLATFORM_ZED)
#if (CURRENT_PLATFORM == PLATFORM_KC705) || \
	(CURRENT_PLATFORM == PLATFORM_VC707) || \
	(CURRENT_PLATFORM == PLATFORM_AC701)
	uint8_t pca9548_setup = 0x20;
#elif (CURRENT_PLATFORM == PLATFORM_ZC706) || \
		(CURRENT_PLATFORM == PLATFORM_ZC702)
	uint8_t pca9548_setup = 0x02;
#endif
	uint8_t mem_val;
	int32_t ret;
	const uint8_t mux_addr = 0x74, adv7511_main_addr = 0x39;
	const uint8_t byte_transfer_no = 1, stop_bit = 1;

	mem_val = pca9548_setup;

	ret = i2c_set_address(adv7511_i2c, mux_addr);
	if(ret != 0)
		return ret;
	ret = i2c_write(adv7511_i2c, &pca9548_setup, byte_transfer_no, stop_bit);
	if(ret != 1)
		return ret;
	pca9548_setup = 0xdd;
	ret = i2c_read(adv7511_i2c, &pca9548_setup, byte_transfer_no, stop_bit);
	if(ret != 1)
		return ret;
	if(pca9548_setup != mem_val)
		return -1;
	ret = i2c_set_address(adv7511_i2c, adv7511_main_addr);
	if(ret != 0)
		return ret;

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
static int32_t hal_platform_init(struct i2c_desc **adv7511_i2c,
				 struct i2c_init_param *adv7511_i2c_init,
				 struct timer_desc **timer_inst_ptr, struct timer_init_param *timer_init,
				 struct gic_desc **gic_inst_ptr, struct gic_init_param *gic_init)
{
	int32_t ret;
#if defined(_XPARAMETERS_PS_H_)
	const uint8_t timer_int_nr = XPAR_SCUTIMER_INTR;
	const uint8_t i2c_int_nr = XPAR_XIICPS_1_INTR;
#else
	const uint8_t timer_int_nr = XPAR_AXI_INTC_AXI_TIMER_INTERRUPT_INTR;
	const uint8_t i2c_int_nr = XPAR_AXI_INTC_AXI_IIC_MAIN_IIC2INTC_IRPT_INTR;
#endif

	ret = timer_setup(timer_inst_ptr, timer_init);
	if(ret != 0)
		return ret;
	ret = i2c_init(adv7511_i2c, adv7511_i2c_init);
	if(ret != 0)
		return ret;

	ret = gic_setup(gic_inst_ptr, gic_init);
	if(ret != 0)
		return ret;
#if defined(_XPARAMETERS_PS_H_)
	ret = gic_connect_handler(*gic_inst_ptr, timer_int_nr, timer_isr,
				  &(*timer_inst_ptr)->instance);
#else
	ret = gic_connect_handler(*gic_inst_ptr, timer_int_nr,
				  XTmrCtr_InterruptHandler, &(*timer_inst_ptr)->instance);
#endif
	if(ret != 0)
		return ret;
	gic_enable_interrupt(*gic_inst_ptr, timer_int_nr);
	ret = gic_connect_handler(*gic_inst_ptr, i2c_int_nr, XIic_InterruptHandler,
				  &(*adv7511_i2c)->instance);
	if(ret != 0)
		return ret;
	gic_enable_interrupt(*gic_inst_ptr, i2c_int_nr);

	timer_enable_interrupt(*timer_inst_ptr);
	timer_start(*timer_inst_ptr);

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
	struct i2c_desc *adv7511_i2c;
	struct i2c_init_param adv7511_i2c_init;
	struct timer_desc *timer_inst_ptr;
	struct timer_init_param timer_init;
	struct gic_desc *gic_inst_ptr;
	struct gic_init_param gic_init;
	int32_t ret;

	adv7511_i2c_init.id = XPAR_AXI_IIC_MAIN_DEVICE_ID;
	adv7511_i2c_init.max_speed_hz = 400000;
	adv7511_i2c_init.slave_address = 0x39;
	adv7511_i2c_init.type = XILINX_I2C;
	timer_init.auto_reload = true;
#if defined(_XPARAMETERS_PS_H_)
	timer_init.id = XPAR_XSCUTIMER_0_DEVICE_ID;
#else
	timer_init.id = XPAR_AXI_TIMER_DEVICE_ID;
	timer_init.active_tmr = 0;
#endif
	timer_init.start_freq = 1000;
#if defined(_XPARAMETERS_PS_H_)
	gic_init.id = XPAR_SCUGIC_0_DEVICE_ID;
#else
	gic_init.id = XPAR_INTC_0_DEVICE_ID;
#endif

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

	Xil_ExceptionEnable();

	SetVideoResolution(RESOLUTION_640x480);
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
		app_change_resolution();
	}

	Xil_DCacheDisable();
	Xil_ICacheDisable();

	return 0;
}
