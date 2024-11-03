/***************************************************************************//**
 *   @file   ad5293.h
 *   @brief  Header file for the AD5293 Driver
 *   @author Ming Zeng
********************************************************************************
The MIT License (MIT)

Copyright (c) 2024 Ming Zeng

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*******************************************************************************/

#ifndef _AD5293_H_
#define _AD5293_H_

/****************************** Include Files ********************************/
#include <stdint.h>
#include "no_os_gpio.h"
#include "no_os_spi.h"
#include "no_os_util.h"

/* SPI Read/Write commands */
#define CMD_NOP        0
#define CMD_W_WIPER    1
#define CMD_R_WIPER    2
#define CMD_RESET      4
#define CMD_W_CTRL     6
#define CMD_R_CTRL     7
#define CMD_PD         8
#define CMD_SDO_Z      0x8001

/* Bit mask*/
#define CALI_MSK       NO_OS_BIT(2)
#define PROTECT_MSK    NO_OS_BIT(1)

/* calibration mode */
enum calibration_mode_t {
	CALI_PERFORMANCE,
	CALI_NORMAL
};

/* protect mode */
enum protect_mode_t {
	PROTECT_LOCK,
	PROTECT_UNLOCK // enable update wiper position through digital interface
};

/* shutdown mode */
enum shutdown_t {
	NORMAL,
	SHUTDOWN
};

/* operation mode */
enum operation_mode_t {
	BUS,
	DAISY_CHAIN
};

/* SDO mode */
enum sdo_mode_t {
	SDO_DISENABLE,
	SDO_ENABLE
};

/* SPI frame */
#define AD5293_W_CTRL_FRAME(pm,cali)     (uint16_t)((CMD_W_CTRL<<10)|(pm<<1)|(cali<<2))
#define AD5293_R_CTRL_FRAME              (uint16_t)CMD_R_CTRL<<10
#define AD5293_W_WIPER_FRAME(data)       (uint16_t)((CMD_W_WIPER<<10)|data)
#define AD5293_R_WIPER_FRAME             (uint16_t)CMD_R_WIPER<<10
#define SHUTDOWN_FRAME(sd)               (uint16_t)((CMD_PD<<10)|sd)
#define RESET_FRAME                      (uint16_t)CMD_RESET<<10

/**********************************struct*****************************************/
struct ad5293_chip_info {
	enum calibration_mode_t cali;
	uint16_t Wiper_value; //10bit
};

struct ad5293_dev {
	/* SPI */
	struct no_os_spi_desc	*spi_desc;
	/* GPIO */
	struct no_os_gpio_desc	*gpio_reset;
	/* number of chips */
	uint16_t chip_num;
	/* pointer of chip struct  */
	struct ad5293_chip_info* chip; //point to chip 0
};

struct ad5293_init_param {
	/* SPI */
	struct no_os_spi_init_param spi_init;
	/* GPIO */
	struct no_os_gpio_init_param* gpio_reset;
	/* number of chips */
	uint16_t chip_num;
};

/*******************************functions prototype*******************************/
/* Initialize the ad5293 device structure. */
int32_t ad5293_init(struct ad5293_dev **device,
		    struct ad5293_init_param* init_param);
/* Free any resource used by the driver. */
int32_t ad5293_remove(struct ad5293_dev* dev);
/* Reset ad5293 through Hardware Reset Pin. */
int32_t ad5293_hard_reset(struct ad5293_dev* dev);
/* Reset ad5293 through software reset command. */
int32_t ad5293_soft_reset(struct ad5293_dev* dev);
/* Place SDO pin in high impedence mode. */
int32_t ad5293_sdo_setfloat(struct ad5293_dev* dev);
/* Place ad5293 in shutdown mode or recovery. */
int32_t ad5293_shutdown(struct ad5293_dev* dev, enum shutdown_t sd);
/* Update ad5293 chip info struct calibration information. */
int32_t ad5293_update_cali(struct ad5293_dev* dev, enum calibration_mode_t cali,
			   uint8_t num);
/* Update ad5293 chip info struct wiper value information. */
int32_t ad5293_update_wiper(struct ad5293_dev* dev, uint16_t val, uint8_t num);
/* Write ad5293 chip info struct calibration value through SPI interface. */
int32_t ad5293_write_cali(struct ad5293_dev* dev);
/* Write ad5293 chip info struct wiper value through SPI interface. */
int32_t ad5293_write_wiper(struct ad5293_dev* dev);
/* Read ad5293 device register calibration mode through SPI interface. */
int32_t ad5293_read_reg_cali(struct ad5293_dev* dev);
/* Read ad5293 device register wiper value through SPI interface. */
int32_t ad5293_read_reg_wiper(struct ad5293_dev* dev);
/* Get calibration of a certain chip. */
int8_t ad5293_get_cali(struct ad5293_dev* dev, uint8_t num);
/* Get wiper of a certain chip. */
int16_t ad5293_get_wiper(struct ad5293_dev* dev, uint8_t num);

#endif  // _AD5293_H_
