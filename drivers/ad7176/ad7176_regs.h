/**************************************************************************//**
*   @file   AD7176_regs.h
*   @brief  AD7176 Registers Definitions.
*   @author acozma (andrei.cozma@analog.com)
*           dnechita (dan.nechita@analog.com)
*
*******************************************************************************
* Copyright 2015(c) Analog Devices, Inc.
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
typedef struct _ad7176_st_reg
{
	int32_t addr;
	int32_t value;
	int32_t size;
}ad7176_st_reg;

/*! AD7176 registers list*/
enum ad7176_registers
{
	AD7176_Status = 0x00,
	AD7176_ADC_Mode,
	AD7176_Interface_Mode,
	AD7176_Register_Check,
	AD7176_Data,
	AD7176_GPIOCon,
	AD7176_ID,
	AD7176_CH_Map_0,
	AD7176_CH_Map_1,
	AD7176_CH_Map_2,
	AD7176_CH_Map_3,
	AD7176_Setup_Config_0,
	AD7176_Setup_Config_1,
	AD7176_Setup_Config_2,
	AD7176_Setup_Config_3,
	AD7176_Filter_Config_0,
	AD7176_Filter_Config_1,
	AD7176_Filter_Config_2,
	AD7176_Filter_Config_3,
	AD7176_Offset_0,
	AD7176_Offset_1,
	AD7176_Offset_2,
	AD7176_Offset_3,
	AD7176_Gain_0,
	AD7176_Gain_1,
	AD7176_Gain_2,
	AD7176_Gain_3,
	AD7176_REG_NO,
};

#ifdef AD7176_INIT
/*! Array holding the info for the AD7176 registers - address, initial value, size */
ad7176_st_reg ad7176_regs[] =
{
	{0x00, 0x00,   1}, /* AD7176_Status */
	{0x01, 0x0000, 2}, /* AD7176_ADC_Mode */
	{0x02, 0x0108, 2}, /* AD7176_Interface_Mode */
	{0x03, 0x0000, 3}, /* AD7176_Register_Check */
	{0x04, 0x0000, 3}, /* AD7176_Data */
	{0x06, 0x0000, 2}, /* AD7176_GPIOCon */
	{0x07, 0x0000, 2}, /* AD7176_ID */
	{0x10, 0x8002, 2}, /* AD7176_CH_Map_0 */
	{0x11, 0x0000, 2}, /* AD7176_CH_Map_1 */
	{0x12, 0x0000, 2}, /* AD7176_CH_Map_2 */
	{0x13, 0x0000, 2}, /* AD7176_CH_Map_3 */
	{0x20, 0x0000, 2}, /* AD7176_Setup_Config_0 */
	{0x21, 0x0000, 2}, /* AD7176_Setup_Config_1 */
	{0x22, 0x0000, 2}, /* AD7176_Setup_Config_2 */
	{0x23, 0x0000, 2}, /* AD7176_Setup_Config_3 */
	{0x28, 0x020A, 2}, /* AD7176_Filter_Config_0 */
	{0x29, 0x0200, 2}, /* AD7176_Filter_Config_1 */
	{0x2a, 0x0200, 2}, /* AD7176_Filter_Config_2 */
	{0x2b, 0x0200, 2}, /* AD7176_Filter_Config_3 */
	{0x30, 0, 3}, /* AD7176_Offset_0 */
	{0x31, 0, 3}, /* AD7176_Offset_1 */
	{0x32, 0, 3}, /* AD7176_Offset_2 */
	{0x33, 0, 3}, /* AD7176_Offset_3 */
	{0x38, 0, 3}, /* AD7176_Gain_0 */
	{0x39, 0, 3}, /* AD7176_Gain_1 */
	{0x3a, 0, 3}, /* AD7176_Gain_2 */
	{0x3b, 0, 3}, /* AD7176_Gain_3 */
};
#endif

#define AD7176_SLAVE_ID    1

/* AD7176 Register Map */
#define AD7176_COMM_REG       0x00
#define AD7176_STATUS_REG     0x00
#define AD7176_ADCMODE_REG    0x01
#define AD7176_IFMODE_REG     0x02
#define AD7176_REGCHECK_REG   0x03
#define AD7176_DATA_REG       0x04
#define AD7176_GPIOCON_REG    0x06
#define AD7176_ID_REG         0x07
#define AD7176_CHMAP0_REG     0x10
#define AD7176_CHMAP1_REG     0x11
#define AD7176_CHMAP2_REG     0x12
#define AD7176_CHMAP3_REG     0x13
#define AD7176_CHMAP4_REG     0x14
#define AD7176_CHMAP5_REG     0x15
#define AD7176_CHMAP6_REG     0x16
#define AD7176_CHMAP7_REG     0x17
#define AD7176_CHMAP8_REG     0x18
#define AD7176_CHMAP9_REG     0x19
#define AD7176_CHMAP10_REG    0x1A
#define AD7176_CHMAP11_REG    0x1B
#define AD7176_CHMAP12_REG    0x1C
#define AD7176_CHMAP13_REG    0x1D
#define AD7176_CHMAP14_REG    0x1E
#define AD7176_CHMAP15_REG    0x1F
#define AD7176_SETUPCON0_REG  0x20
#define AD7176_SETUPCON1_REG  0x21
#define AD7176_SETUPCON2_REG  0x22
#define AD7176_SETUPCON3_REG  0x23
#define AD7176_SETUPCON4_REG  0x24
#define AD7176_SETUPCON5_REG  0x25
#define AD7176_SETUPCON6_REG  0x26
#define AD7176_SETUPCON7_REG  0x27
#define AD7176_FILTCON0_REG   0x28
#define AD7176_FILTCON1_REG   0x29
#define AD7176_FILTCON2_REG   0x2A
#define AD7176_FILTCON3_REG   0x2B
#define AD7176_FILTCON4_REG   0x2C
#define AD7176_FILTCON5_REG   0x2D
#define AD7176_FILTCON6_REG   0x2E
#define AD7176_FILTCON7_REG   0x2F
#define AD7176_OFFSET0_REG    0x30
#define AD7176_OFFSET1_REG    0x31
#define AD7176_OFFSET2_REG    0x32
#define AD7176_OFFSET3_REG    0x33
#define AD7176_OFFSET4_REG    0x34
#define AD7176_OFFSET5_REG    0x35
#define AD7176_OFFSET6_REG    0x36
#define AD7176_OFFSET7_REG    0x37
#define AD7176_GAIN0_REG      0x38
#define AD7176_GAIN1_REG      0x39
#define AD7176_GAIN2_REG      0x3A
#define AD7176_GAIN3_REG      0x3B
#define AD7176_GAIN4_REG      0x3C
#define AD7176_GAIN5_REG      0x3D
#define AD7176_GAIN6_REG      0x3E
#define AD7176_GAIN7_REG      0x3F

/* Communication Register bits */
#define AD7176_COMM_REG_WEN    (0 << 7)
#define AD7176_COMM_REG_WR     (0 << 6)
#define AD7176_COMM_REG_RD     (1 << 6)
#define AD7176_COMM_REG_RA(x)  ((x) & 0x3F)

/* Status Register bits */
#define AD7176_STATUS_REG_RDY      (1 << 7)
#define AD7176_STATUS_REG_ADC_ERR  (1 << 6)
#define AD7176_STATUS_REG_CRC_ERR  (1 << 5)
#define AD7176_STATUS_REG_REG_ERR  (1 << 4)
#define AD7176_STATUS_REG_CH(x)    ((x) & 0x03)

/* ADC Mode Register bits */
#define AD7176_ADCMODE_REG_REF_EN     (1 << 15)
#define AD7176_ADCMODE_SING_CYC       (1 << 13)
#define AD7176_ADCMODE_REG_DELAY(x)   (((x) & 0x7) << 8)
#define AD7176_ADCMODE_REG_MODE(x)    (((x) & 0x7) << 4)
#define AD7176_ADCMODE_REG_CLKSEL(x)) (((x) & 0x3) << 2)

/* ADC Mode Register additional bits for AD7172-2 and AD7172-4 */
#define AD7176_ADCMODE_REG_HIDE_DELAY   (1 << 14)

/* Interface Mode Register bits */
#define AD7176_IFMODE_REG_ALT_SYNC      (1 << 12)
#define AD7176_IFMODE_REG_IOSTRENGTH    (1 << 11)
#define AD7176_IFMODE_REG_HIDE_DELAY    (1 << 10)
#define AD7176_IFMODE_REG_DOUT_RESET    (1 << 8)
#define AD7176_IFMODE_REG_CONT_READ     (1 << 7)
#define AD7176_IFMODE_REG_DATA_STAT     (1 << 6)
#define AD7176_IFMODE_REG_REG_CHECK     (1 << 5)
#define AD7176_IFMODE_REG_XOR_EN        (0x01 << 2)
#define AD7176_IFMODE_REG_CRC_EN        (0x02 << 2)
#define AD7176_IFMODE_REG_XOR_STAT(x)   (((x) & AD7176_IFMODE_REG_XOR_EN) == AD7176_IFMODE_REG_XOR_EN)
#define AD7176_IFMODE_REG_CRC_STAT(x)   (((x) & AD7176_IFMODE_REG_CRC_EN) == AD7176_IFMODE_REG_CRC_EN)
#define AD7176_IFMODE_REG_DATA_WL16     (1 << 0)

/* GPIO Configuration Register bits */
#define AD7176_GPIOCON_REG_MUX_IO      (1 << 12)
#define AD7176_GPIOCON_REG_SYNC_EN     (1 << 11)
#define AD7176_GPIOCON_REG_ERR_EN(x)   (((x) & 0x3) << 9)
#define AD7176_GPIOCON_REG_ERR_DAT     (1 << 8)
#define AD7176_GPIOCON_REG_IP_EN1      (1 << 5)
#define AD7176_GPIOCON_REG_IP_EN0      (1 << 4)
#define AD7176_GPIOCON_REG_OP_EN1      (1 << 3)
#define AD7176_GPIOCON_REG_OP_EN0      (1 << 2)
#define AD7176_GPIOCON_REG_DATA1       (1 << 1)
#define AD7176_GPIOCON_REG_DATA0       (1 << 0)

/* GPIO Configuration Register additional bits for AD7172-4, AD7173-8 */
#define AD7176_GPIOCON_REG_GP_DATA3    (1 << 7)
#define AD7176_GPIOCON_REG_GP_DATA2    (1 << 6)
#define AD7176_GPIOCON_REG_GP_DATA1    (1 << 1)
#define AD7176_GPIOCON_REG_GP_DATA0    (1 << 0)

/* GPIO Configuration Register additional bits for AD7173-8 */
#define AD7176_GPIOCON_REG_PDSW        (1 << 14)
#define AD7176_GPIOCON_REG_OP_EN2_3    (1 << 13)

/* Channel Map Register 0-3 bits */
#define AD7176_CHMAP_REG_CH_EN         (1 << 15)
#define AD7176_CHMAP_REG_SETUP_SEL(x)  (((x) & 0x3) << 12)
#define AD7176_CHMAP_REG_AINPOS(x)     (((x) & 0x1F) << 5)
#define AD7176_CHMAP_REG_AINNEG(x)     (((x) & 0x1F) << 0)

/* Setup Configuration Register 0-3 bits */
#define AD7176_SETUP_CONF_REG_BI_UNIPOLAR  (1 << 12)
#define AD7176_SETUP_CONF_REG_REF_SEL(x)   (((x) & 0x3) << 4)

/* Setup Configuration Register additional bits for AD7173-8 */
#define AD7176_SETUP_CONF_REG_REF_BUF(x)  (((x) & 0x3) << 10)
#define AD7176_SETUP_CONF_REG_AIN_BUF(x)  (((x) & 0x3) << 8)
#define AD7176_SETUP_CONF_REG_BURNOUT_EN  (1 << 7)
#define AD7176_SETUP_CONF_REG_BUFCHOPMAX  (1 << 6)

/* Setup Configuration Register additional bits for AD7172-2, AD7172-4, AD7175-2 */
#define AD7176_SETUP_CONF_REG_REFBUF_P    (1 << 11)
#define AD7176_SETUP_CONF_REG_REFBUF_N    (1 << 10)
#define AD7176_SETUP_CONF_REG_AINBUF_P    (1 << 9)
#define AD7176_SETUP_CONF_REG_AINBUF_N    (1 << 8)

/* Filter Configuration Register 0-3 bits */
#define AD7176_FILT_CONF_REG_SINC3_MAP    (1 << 15)
#define AD7176_FILT_CONF_REG_ENHFILTEN    (1 << 11)
#define AD7176_FILT_CONF_REG_ENHFILT(x)   (((x) & 0x7) << 8)
#define AD7176_FILT_CONF_REG_ORDER(x)     (((x) & 0x3) << 5)
#define AD7176_FILT_CONF_REG_ODR(x)       (((x) & 0x1F) << 0)

#endif /*__AD7176_REGS_H__ */
