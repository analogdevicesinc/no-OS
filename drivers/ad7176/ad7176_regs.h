/**************************************************************************//**
*   @file   AD7176_regs.h
*   @brief  AD7176 Registers Definitions.
*   @author acozma (andrei.cozma@analog.com)
*
*******************************************************************************
* Copyright 2011(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
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
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT, MERCHANTABILITY
* AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
******************************************************************************/

#ifndef __AD7176_REGS_H__
#define __AD7176_REGS_H__

/*! AD7176 register info */
typedef struct _st_reg
{
	int32_t addr;
	int32_t value;
	int32_t size;
}st_reg;

/*! AD7176 registers list*/
enum AD7176_registers
{
	Status_Register = 0x00,
	ADC_Mode_Register,
	Interface_Mode_Register,
	Register_Check,
	Data_Register,
	IOCon_Register,
	ID_st_reg,
	CH_Map_1,
	CH_Map_2,
	CH_Map_3,
	CH_Map_4,
	Setup_Config_1,
	Setup_Config_2,
	Setup_Config_3,
	Setup_Config_4,
	Filter_Config_1,
	Filter_Config_2,
	Filter_Config_3,
	Filter_Config_4,
	Offset_1,
	Offset_2,
	Offset_3,
	Offset_4,
	Gain_1,
	Gain_2,
	Gain_3,
	Gain_4,
	Communications_Register,
	AD7176_REG_NO
};

#ifdef AD7176_INIT
/*! Array holding the info for the AD7176 registers - address, initial value, size */
st_reg AD7176_regs[] =
{
	{0x00, 0x00,   1}, //Status_Register
	{0x01, 0x0000, 2}, //ADC_Mode_Register
	{0x02, 0x0100, 2}, //Interface_Mode_Register
	{0x03, 0x0000, 3}, //Register_Check
	{0x04, 0x0000, 3}, //Data_Register
	{0x06, 0x0000, 2}, //IOCon_Register
	{0x07, 0x0000, 2}, //ID_st_reg
	{0x10, 0x8002, 2}, //CH_Map_1
	{0x11, 0x0000, 2}, //CH_Map_2
	{0x12, 0x0000, 2}, //CH_Map_3
	{0x13, 0x0000, 2}, //CH_Map_4
	{0x20, 0x0000, 2}, //Setup_Config_1
	{0x21, 0x0000, 2}, //Setup_Config_2
	{0x22, 0x0000, 2}, //Setup_Config_3
	{0x23, 0x0000, 2}, //Setup_Config_4
	{0x28, 0x020A, 2}, //Filter_Config_1
	{0x29, 0x0200, 2}, //Filter_Config_2
	{0x2a, 0x0200, 2}, //Filter_Config_3
	{0x2b, 0x0200, 2}, //Filter_Config_4
	{0x30, 0, 3}, //Offset_1
	{0x31, 0, 3}, //Offset_2
	{0x32, 0, 3}, //Offset_3
	{0x33, 0, 3}, //Offset_4
	{0x38, 0, 3}, //Gain_1
	{0x39, 0, 3}, //Gain_2
	{0x3a, 0, 3}, //Gain_3
	{0x3b, 0, 3}, //Gain_4
	{0xFF, 0, 1} //Communications_Register
};
#else
extern st_reg AD7176_regs[AD7176_REG_NO];
#endif

#define AD7176_SLAVE_ID    1

/* Communication Register bits */
#define COMM_REG_WEN    (0 << 7)
#define COMM_REG_WR     (0 << 6)
#define COMM_REG_RD     (1 << 6)

/* Status Register bits */
#define STATUS_REG_RDY      (1 << 7)
#define STATUS_REG_ADC_ERR  (1 << 6)
#define STATUS_REG_CRC_ERR  (1 << 5)
#define STATUS_REG_REG_ERR  (1 << 4)
#define STATUS_REG_CH(x)    ((x) & 0x03)

/* ADC Mode Register */
#define ADC_MODE_REG_REF_EN         (1 << 15)
#define ADC_MODE_SING_CYC           (1 << 13)
#define ADC_MODE_REG_DELAY(x)       (((x) & 0x7) << 8)
#define ADC_MODE_REG_MODE(x)        (((x) & 0x7) << 4)
#define ADC_MODE_REG_CLKSEL(x))     (((x) & 0x3) << 2)

/* Interface Mode Register bits */
#define INTF_MODE_REG_ALT_SYNC      (1 << 12)
#define INTF_MODE_REG_IOSTRENGTH    (1 << 11)
#define INTF_MODE_REG_DOUT_RESET    (1 << 8)
#define INTF_MODE_REG_CONT_READ     (1 << 7)
#define INTF_MODE_REG_DATA_STAT     (1 << 6)
#define INTF_MODE_REG_XOR_EN        (0x01 << 2)
#define INTF_MODE_REG_CRC_EN        (0x02 << 2)
#define INTF_MODE_REG_XOR_STAT(x)   (((x) & INTF_MODE_REG_XOR_EN) == INTF_MODE_REG_XOR_EN)
#define INTF_MODE_REG_CRC_STAT(x)   (((x) & INTF_MODE_REG_CRC_EN) == INTF_MODE_REG_CRC_EN)

/* GPIO Configuration Register */
#define GPIO_CONF_REG_MUX_IO        (1 << 12)
#define GPIO_CONF_REG_SYNC_EN       (1 << 11)
#define GPIO_CONF_REG_ERR_EN(x)     (((x) & 0x3) << 9)
#define GPIO_CONF_REG_ERR_DAT       (1 << 8)
#define GPIO_CONF_REG_IP_EN1        (1 << 5)
#define GPIO_CONF_REG_IP_EN0        (1 << 4)
#define GPIO_CONF_REG_OP_EN1        (1 << 3)
#define GPIO_CONF_REG_OP_EN0        (1 << 2)
#define GPIO_CONF_REG_DATA1         (1 << 1)
#define GPIO_CONF_REG_DATA0         (1 << 0)

/* Channel Map Register 1-4 */
#define CH_MAP_REG_CH_EN        (1 << 15)
#define CH_MAP_REG_SETUP(x)     (((x) & 0x7) << 12)
#define CH_MAP_REG_AINPOS(x)    (((x) & 0x1F) << 5)
#define CH_MAP_REG_AINNEG(x)    (((x) & 0x1F) << 0)

/* Setup Configuration Register 1-4 */
#define SETUP_CONF_REG_BI_UNIPOLAR      (1 << 12)
#define SETUP_CONF_REG_REF_SEL(x)       (((x) & 0x3) << 4)

/* Filter Configuration Register 1-4 */
#define FILT_CONF_REG_SINC3_MAP         (1 << 15)
#define FILT_CONF_REG_ENHFILTEN         (1 << 11)
#define FILT_CONF_REG_ENHFILTSEL(x)     (((x) & 0x7) << 8)
#define FILT_CONF_REG_ORDER(x)          (((x) & 0x7) << 5)
#define FILT_CONF_REG_ODR(x)            (((x) & 0x1F) << 0)

#endif //__AD7176_REGS_H__
