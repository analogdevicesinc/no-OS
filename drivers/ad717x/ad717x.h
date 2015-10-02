/**************************************************************************//**
*   @file    AD717X.h
*   @brief   AD717X header file.
*   @devices AD7172-2, AD7172-4, AD7173-8, AD7175-2, AD7175-8, AD7176-2,
*            AD7177-2
*   @author  acozma (andrei.cozma@analog.com)
*            dnechita (dan.nechita@analog.com)
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

#ifndef __AD717X_H__
#define __AD717X_H__

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/
#include <stdint.h>

typedef enum ad717x_crc_mode ad717x_crc_mode;

enum ad717x_crc_mode
{
	AD717X_DISABLE,
	AD717X_USE_CRC,
	AD717X_USE_XOR,
};

/*! AD717X register info */
typedef struct _ad717x_st_reg
{
	int32_t addr;
	int32_t value;
	int32_t size;
}ad717x_st_reg;

/*
 * The structure describes the device and is used with the ad717x driver.
 * @slave_select_id: The ID of the Slave Select to be passed to the SPI calls.
 * @regs: A reference to the register list of the device that the user must
 *       provide when calling the Setup() function.
 * @num_regs: The length of the register list. 
 * @userCRC: Error check type to use on SPI transfers.
 */
struct ad717x_device {
	uint8_t slave_select_id;
	ad717x_st_reg *regs;
	uint8_t num_regs;
	ad717x_crc_mode useCRC;
};

/*****************************************************************************/
/***************** AD717X Register Definitions *******************************/
/*****************************************************************************/

/* AD717X Register Map */
#define AD717X_COMM_REG       0x00
#define AD717X_STATUS_REG     0x00
#define AD717X_ADCMODE_REG    0x01
#define AD717X_IFMODE_REG     0x02
#define AD717X_REGCHECK_REG   0x03
#define AD717X_DATA_REG       0x04
#define AD717X_GPIOCON_REG    0x06
#define AD717X_ID_REG         0x07
#define AD717X_CHMAP0_REG     0x10
#define AD717X_CHMAP1_REG     0x11
#define AD717X_CHMAP2_REG     0x12
#define AD717X_CHMAP3_REG     0x13
#define AD717X_CHMAP4_REG     0x14
#define AD717X_CHMAP5_REG     0x15
#define AD717X_CHMAP6_REG     0x16
#define AD717X_CHMAP7_REG     0x17
#define AD717X_CHMAP8_REG     0x18
#define AD717X_CHMAP9_REG     0x19
#define AD717X_CHMAP10_REG    0x1A
#define AD717X_CHMAP11_REG    0x1B
#define AD717X_CHMAP12_REG    0x1C
#define AD717X_CHMAP13_REG    0x1D
#define AD717X_CHMAP14_REG    0x1E
#define AD717X_CHMAP15_REG    0x1F
#define AD717X_SETUPCON0_REG  0x20
#define AD717X_SETUPCON1_REG  0x21
#define AD717X_SETUPCON2_REG  0x22
#define AD717X_SETUPCON3_REG  0x23
#define AD717X_SETUPCON4_REG  0x24
#define AD717X_SETUPCON5_REG  0x25
#define AD717X_SETUPCON6_REG  0x26
#define AD717X_SETUPCON7_REG  0x27
#define AD717X_FILTCON0_REG   0x28
#define AD717X_FILTCON1_REG   0x29
#define AD717X_FILTCON2_REG   0x2A
#define AD717X_FILTCON3_REG   0x2B
#define AD717X_FILTCON4_REG   0x2C
#define AD717X_FILTCON5_REG   0x2D
#define AD717X_FILTCON6_REG   0x2E
#define AD717X_FILTCON7_REG   0x2F
#define AD717X_OFFSET0_REG    0x30
#define AD717X_OFFSET1_REG    0x31
#define AD717X_OFFSET2_REG    0x32
#define AD717X_OFFSET3_REG    0x33
#define AD717X_OFFSET4_REG    0x34
#define AD717X_OFFSET5_REG    0x35
#define AD717X_OFFSET6_REG    0x36
#define AD717X_OFFSET7_REG    0x37
#define AD717X_GAIN0_REG      0x38
#define AD717X_GAIN1_REG      0x39
#define AD717X_GAIN2_REG      0x3A
#define AD717X_GAIN3_REG      0x3B
#define AD717X_GAIN4_REG      0x3C
#define AD717X_GAIN5_REG      0x3D
#define AD717X_GAIN6_REG      0x3E
#define AD717X_GAIN7_REG      0x3F

/* Communication Register bits */
#define AD717X_COMM_REG_WEN    (0 << 7)
#define AD717X_COMM_REG_WR     (0 << 6)
#define AD717X_COMM_REG_RD     (1 << 6)
#define AD717X_COMM_REG_RA(x)  ((x) & 0x3F)

/* Status Register bits */
#define AD717X_STATUS_REG_RDY      (1 << 7)
#define AD717X_STATUS_REG_ADC_ERR  (1 << 6)
#define AD717X_STATUS_REG_CRC_ERR  (1 << 5)
#define AD717X_STATUS_REG_REG_ERR  (1 << 4)
#define AD717X_STATUS_REG_CH(x)    ((x) & 0x03)

/* ADC Mode Register bits */
#define AD717X_ADCMODE_REG_REF_EN     (1 << 15)
#define AD717X_ADCMODE_SING_CYC       (1 << 13)
#define AD717X_ADCMODE_REG_DELAY(x)   (((x) & 0x7) << 8)
#define AD717X_ADCMODE_REG_MODE(x)    (((x) & 0x7) << 4)
#define AD717X_ADCMODE_REG_CLKSEL(x)) (((x) & 0x3) << 2)

/* ADC Mode Register additional bits for AD7172-2 and AD7172-4 */
#define AD717X_ADCMODE_REG_HIDE_DELAY   (1 << 14)

/* Interface Mode Register bits */
#define AD717X_IFMODE_REG_ALT_SYNC      (1 << 12)
#define AD717X_IFMODE_REG_IOSTRENGTH    (1 << 11)
#define AD717X_IFMODE_REG_HIDE_DELAY    (1 << 10)
#define AD717X_IFMODE_REG_DOUT_RESET    (1 << 8)
#define AD717X_IFMODE_REG_CONT_READ     (1 << 7)
#define AD717X_IFMODE_REG_DATA_STAT     (1 << 6)
#define AD717X_IFMODE_REG_REG_CHECK     (1 << 5)
#define AD717X_IFMODE_REG_XOR_EN        (0x01 << 2)
#define AD717X_IFMODE_REG_CRC_EN        (0x02 << 2)
#define AD717X_IFMODE_REG_XOR_STAT(x)   (((x) & AD717X_IFMODE_REG_XOR_EN) == AD717X_IFMODE_REG_XOR_EN)
#define AD717X_IFMODE_REG_CRC_STAT(x)   (((x) & AD717X_IFMODE_REG_CRC_EN) == AD717X_IFMODE_REG_CRC_EN)
#define AD717X_IFMODE_REG_DATA_WL16     (1 << 0)

/* GPIO Configuration Register bits */
#define AD717X_GPIOCON_REG_MUX_IO      (1 << 12)
#define AD717X_GPIOCON_REG_SYNC_EN     (1 << 11)
#define AD717X_GPIOCON_REG_ERR_EN(x)   (((x) & 0x3) << 9)
#define AD717X_GPIOCON_REG_ERR_DAT     (1 << 8)
#define AD717X_GPIOCON_REG_IP_EN1      (1 << 5)
#define AD717X_GPIOCON_REG_IP_EN0      (1 << 4)
#define AD717X_GPIOCON_REG_OP_EN1      (1 << 3)
#define AD717X_GPIOCON_REG_OP_EN0      (1 << 2)
#define AD717X_GPIOCON_REG_DATA1       (1 << 1)
#define AD717X_GPIOCON_REG_DATA0       (1 << 0)

/* GPIO Configuration Register additional bits for AD7172-4, AD7173-8 */
#define AD717X_GPIOCON_REG_GP_DATA3    (1 << 7)
#define AD717X_GPIOCON_REG_GP_DATA2    (1 << 6)
#define AD717X_GPIOCON_REG_GP_DATA1    (1 << 1)
#define AD717X_GPIOCON_REG_GP_DATA0    (1 << 0)

/* GPIO Configuration Register additional bits for AD7173-8 */
#define AD717X_GPIOCON_REG_PDSW        (1 << 14)
#define AD717X_GPIOCON_REG_OP_EN2_3    (1 << 13)

/* Channel Map Register 0-3 bits */
#define AD717X_CHMAP_REG_CH_EN         (1 << 15)
#define AD717X_CHMAP_REG_SETUP_SEL(x)  (((x) & 0x3) << 12)
#define AD717X_CHMAP_REG_AINPOS(x)     (((x) & 0x1F) << 5)
#define AD717X_CHMAP_REG_AINNEG(x)     (((x) & 0x1F) << 0)

/* Setup Configuration Register 0-3 bits */
#define AD717X_SETUP_CONF_REG_BI_UNIPOLAR  (1 << 12)
#define AD717X_SETUP_CONF_REG_REF_SEL(x)   (((x) & 0x3) << 4)

/* Setup Configuration Register additional bits for AD7173-8 */
#define AD717X_SETUP_CONF_REG_REF_BUF(x)  (((x) & 0x3) << 10)
#define AD717X_SETUP_CONF_REG_AIN_BUF(x)  (((x) & 0x3) << 8)
#define AD717X_SETUP_CONF_REG_BURNOUT_EN  (1 << 7)
#define AD717X_SETUP_CONF_REG_BUFCHOPMAX  (1 << 6)

/* Setup Configuration Register additional bits for AD7172-2, AD7172-4, AD7175-2 */
#define AD717X_SETUP_CONF_REG_REFBUF_P    (1 << 11)
#define AD717X_SETUP_CONF_REG_REFBUF_N    (1 << 10)
#define AD717X_SETUP_CONF_REG_AINBUF_P    (1 << 9)
#define AD717X_SETUP_CONF_REG_AINBUF_N    (1 << 8)

/* Filter Configuration Register 0-3 bits */
#define AD717X_FILT_CONF_REG_SINC3_MAP    (1 << 15)
#define AD717X_FILT_CONF_REG_ENHFILTEN    (1 << 11)
#define AD717X_FILT_CONF_REG_ENHFILT(x)   (((x) & 0x7) << 8)
#define AD717X_FILT_CONF_REG_ORDER(x)     (((x) & 0x3) << 5)
#define AD717X_FILT_CONF_REG_ODR(x)       (((x) & 0x1F) << 0)

/*****************************************************************************/
/******************* AD717X Constants ****************************************/
/*****************************************************************************/
#define AD717X_CRC8_POLYNOMIAL_REPRESENTATION 0x07 /* x8 + x2 + x + 1 */

/*****************************************************************************/
/************************ Functions Declarations *****************************/
/*****************************************************************************/

/*! Retrieves a pointer to the register that matches the given address */
ad717x_st_reg *AD717X_GetReg(struct ad717x_device *device, uint8_t reg_address);

/*! Reads the value of the specified register. */
int32_t AD717X_ReadRegister(struct ad717x_device *device, uint8_t addr);

/*! Writes the value of the specified register. */
int32_t AD717X_WriteRegister(struct ad717x_device *device, uint8_t);

/*! Resets the device. */
int32_t AD717X_Reset(struct ad717x_device *device);

/*! Waits until a new conversion result is available. */
int32_t AD717X_WaitForReady(struct ad717x_device *device, uint32_t timeout);

/*! Reads the conversion result from the device. */
int32_t AD717X_ReadData(struct ad717x_device *device, int32_t* pData);

/*! Computes the CRC checksum for a data buffer. */
uint8_t AD717X_ComputeCRC8(uint8_t* pBuf, uint8_t bufSize);

/*! Computes the XOR checksum for a data buffer. */
uint8_t AD717X_ComputeXOR8(uint8_t * pBuf, uint8_t bufSize);

/*! Updates the CRC settings. */
int32_t AD717X_UpdateCRCSetting(struct ad717x_device *device);

/*! Initializes the AD717X. */
int32_t AD717X_Setup(struct ad717x_device *device, uint8_t slave_select,
			ad717x_st_reg *regs, uint8_t num_regs);

#endif /* __AD717X_H__ */
