/***************************************************************************//**
 *   @file   max22516.h
 *   @brief  Header file for max22516 Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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

#ifndef MAX22516_H_
#define MAX22516_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <math.h>
#include <stdbool.h>
#include "no_os_spi.h"
#include "no_os_gpio.h"
#include "no_os_util.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define REG_CHIP_ID			0x00
#define REG_REV_ID			0x01
#define REG_IOL_STAT			0x02
#define REG_DEV_STAT1			0x03
#define REG_DEV_STAT2			0x04
#define REG_ISDU_STAT			0x05
#define REG_IOL_ERR_CNT			0x06
#define REG_FRM_ERR_CNT			0x07
#define REG_IOL_INT			0x08
#define REG_DEV_INT			0x09
#define REG_ISDU_INT			0x0A
#define REG_IOL_INT_EN			0x0E
#define REG_DEV_INT_EN			0x0F
#define REG_ISDU_INT_EN			0x10
#define REG_IOL_CFG			0x14
#define REG_WATCHDOG			0x15
#define REG_WDGCLR			0x16
#define REG_MISC_CFG			0x17
#define REG_CLK_CFG			0x18
#define REG_CLK_TRIM			0x19
#define REG_PG1_MSTCMD			0x1A
#define REG_PG1_MSTCYCTM		0x1B
#define REG_PG1_MINCYCTM		0x1C
#define REG_PG1_MSEQCAP			0x1D
#define REG_PG1_REVID			0x1E
#define REG_PG1_PDIN			0x1F
#define REG_PG1_PDOUT			0x20
#define REG_PG1_VID1			0x21
#define REG_PG1_VID2			0x22
#define REG_PG1_DEVID1			0x23
#define REG_PG1_DEVID2			0x24
#define REG_PG1_DEVID3			0x25
#define REG_PG1_FUNCID1			0x26
#define REG_PG1_FUNCID2			0x27
#define REG_PG1_RES1			0x28
#define REG_PG1_RES2			0x29
#define REG_WDG_EVENT			0x2A
#define REG_STATUS_CODE_DEF		0x2B
#define REG_STATUS_CODE			0x2C
#define REG_EVENT_QUAL			0x2D
#define REG_EVENT_CODE_MSB		0x2E
#define REG_EVENT_CODE_LSB		0x2F
#define REG_EVENT_FLAG			0x30
#define REG_PDIN_FIFO			0x35
#define REG_PDIN_DATA_RDY		0x36
#define REG_PDOUT_FIFO			0x37
#define REG_ISDU_OFFSET			0x3F
#define REG_ISDU_INFIFO			0x40
#define REG_ISDU_DATARDY		0x41
#define REG_ISDU_OUTFIFO		0x42
#define REG_ISDU_LEVEL			0x43
#define REG_LED1_CTRL_MSB		0x50
#define REG_LED1_CTRL_LSB		0x51
#define REG_LED2_CTRL_MSB		0x52
#define REG_LED2_CTRL_LSB		0x53
#define REG_GPIO1_CTRL			0x54
#define REG_GPIO2_CTRL			0x55
#define REG_CQ_CTRL1			0x56
#define REG_CQ_CTRL2			0x57
#define REG_DO_CTRL1			0x58
#define REG_DO_CTRL2			0x59
#define REG_TX_CTRL			0x5A
#define REG_RX_CTRL			0x5B
#define REG_MISC_CTRL			0x5C

/* REG_DEV_STAT2 */
#define DEV_STAT2_SET_DOFAULT		NO_OS_BIT(5)
#define DEV_STAT2_SET_CQFAULT		NO_OS_BIT(4)
#define DEV_STAT2_SET_V24ERR		NO_OS_BIT(3)
#define DEV_STAT2_SET_VMWERR		NO_OS_BIT(2)
#define DEV_STAT2_SET_THWARN		NO_OS_BIT(1)
#define DEV_STAT2_SET_TSHD		NO_OS_BIT(0)

/* REG_CQ_CTRL1 */
#define BIT_CQCTRL1_CQ_SLEW0		0x00
#define BIT_CQCTRL1_CQ_SLEW1		NO_OS_BIT(6)
#define BIT_CQCTRL1_CQ_SLEW2		NO_OS_BIT(7)
#define BIT_CQCTRL1_CQ_SLEW3		NO_OS_BIT(7, 6)
#define BIT_CQCTRL1_CQ_PD		NO_OS_BIT(5)
#define BIT_CQCTRL1_CQ_PU		NO_OS_BIT(4)
#define BIT_CQCTRL1_CQ_NPN		NO_OS_BIT(3)
#define BIT_CQCTRL1_CQ_PP		NO_OS_BIT(2)
#define BIT_CQCTRL1_CQ_INV		NO_OS_BIT(1)
#define BIT_CQCTRL1_CQ_EN		NO_OS_BIT(0)

/* REG_CQ_CTRL2 */
#define BIT_CQ_CL_50MA			0x00
#define BIT_CQ_CL_100MA			NO_OS_BIT(6)
#define BIT_CQ_CL_200MA			NO_OS_BIT(7)
#define BIT_CQ_CL_250MA			NO_OS_BIT(7, 6)
#define BIT_CQ_CLBL_128US		0x00
#define BIT_CQ_CLBL_500US		NO_OS_BIT(3)
#define BIT_CQ_CLBL_1000US		NO_OS_BIT(4)
#define BIT_CQ_CLBL_5000US		NO_OS_BIT(4, 3)
#define BIT_CQ_AUTORTY_TIME_50MS	0x00
#define BIT_CQ_AUTORTY_TIME_100MS	NO_OS_BIT(1)
#define BIT_CQ_AUTORTY_TIME_200MS	NO_OS_BIT(2)
#define BIT_CQ_AUTORTY_TIME_500MS	NO_OS_BIT(2, 1)
#define BIT_CQ_AUTORTY			NO_OS_BIT(0)

/* REG_DO_CTRL1 */
#define BIT_DOCTRL1_DO_SLEW0		0x00
#define BIT_DOCTRL1_DO_SLEW1		NO_OS_BIT(6)
#define BIT_DOCTRL1_DO_SLEW2		NO_OS_BIT(7)
#define BIT_DOCTRL1_DO_SLEW3		NO_OS_BIT(7, 6)
#define BIT_DOCTRL1_DO_PD		NO_OS_BIT(5)
#define BIT_DOCTRL1_DO_PU		NO_OS_BIT(4)
#define BIT_DOCTRL1_DO_NPN		NO_OS_BIT(3)
#define BIT_DOCTRL1_DO_PP		NO_OS_BIT(2)
#define BIT_DOCTRL1_DO_INV		NO_OS_BIT(1)
#define BIT_DOCTRL1_DO_EN		NO_OS_BIT(0)

/* REG_DO_CTRL2 */
#define BIT_DO_CL_50MA			0x00
#define BIT_DO_CL_100MA			NO_OS_BIT(6)
#define BIT_DO_CL_200MA			NO_OS_BIT(7)
#define BIT_DO_CL_250MA			NO_OS_BIT(7, 6)
#define BIT_DO_CLBL_128US		0x00
#define BIT_DO_CLBL_500US		NO_OS_BIT(3)
#define BIT_DO_CLBL_1000US		NO_OS_BIT(4)
#define BIT_DO_CLBL_5000US		NO_OS_BIT(4, 3)
#define BIT_DO_AUTORTY_TIME_50MS	0x00
#define BIT_DO_AUTORTY_TIME_100MS	NO_OS_BIT(1)
#define BIT_DO_AUTORTY_TIME_200MS	NO_OS_BIT(2)
#define BIT_DO_AUTORTY_TIME_500MS	NO_OS_BIT(2, 1)
#define BIT_DO_AUTORTY			NO_OS_BIT(0)

/* REG_TX_CTRL */
#define BIT_TXC_CQTX			NO_OS_BIT(7)
#define BIT_TXC_CQTXEN			NO_OS_BIT(6)
#define BIT_TXC_CQDRVSEL		NO_OS_BIT(5)
#define BIT_TXC_DOTX			NO_OS_BIT(4)
#define BIT_TXC_DODRVSEL		NO_OS_BIT(3)
#define BIT_TXC_CQDOPAR			NO_OS_BIT(1)
#define BIT_TXC_DO_AV			NO_OS_BIT(0)

/* REG_DEV_STAT2 */
#define DEV_STAT2_SET_V24ERR		NO_OS_BIT(3)
#define DEV_STAT2_SET_VMERR		NO_OS_BIT(2)
#define DEV_STAT2_SET_THWARN		NO_OS_BIT(1)
#define DEV_STAT2_SET_TSHD		NO_OS_BIT(0)

/* REG_EVENT_FLAG */
#define EVF_EVENT_FLG			NO_OS_BIT(0)

/* MAX22516 EVENT CODE Masks */
#define REG_EVENT_CODE_MSB_MSK		NO_OS_GENMASK(15, 8)
#define REG_EVENT_CODE_LSB_MSK		NO_OS_GENMASK(7, 0)

/* MAX22516 VID Masks*/
#define PG1_VID1_MSK			NO_OS_GENMASK(15, 8)
#define PG1_VID2_MSK			NO_OS_GENMASK(7, 0)

/* MAX22516 DEVID Masks*/
#define PG1_DEVID1_MSK			NO_OS_GENMASK(23, 16)
#define PG1_DEVID2_MSK			NO_OS_GENMASK(15, 8)
#define PG1_DEVID3_MSK			NO_OS_GENMASK(7, 0)

/* MAX22516 FUNCID1 Masks */
#define PG1_FUNCID1_MSB_MSK		NO_OS_GENMASK(15, 8)
#define PG1_FUNCID1_LSB_MSK		NO_OS_GENMASK(7, 0)

/* MAX22516 LED1_CTRL Masks */
#define REG_LED1_CTRL_MSB_MSK		NO_OS_GENMASK(15, 8)
#define REG_LED1_CTRL_LSB_MSK		NO_OS_GENMASK(7, 0)

/* MAX22516 LED2_CTRL Masks */
#define REG_LED2_CTRL_MSB_MSK		NO_OS_GENMASK(15, 8)
#define REG_LED2_CTRL_LSB_MSK		NO_OS_GENMASK(7, 0)

/* MAX22516 Extra Definitions */
#define MAX22516_SPI_DUMMY_DATA		0x00
#define MAX22516_BUFF_SIZE_BYTES     	64
#define MAX22516_SPI_READ_CMD		NO_OS_BIT(7)

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct max22516_init_param
 * @brief MAX22516 Initialization Parameters structure.
 */
struct max22516_init_param {
	/* SPI Initialization parameters */
	struct no_os_spi_init_param	*spi_init;
};

/**
 * @struct max22516_dev
 * @brief MAX22516 Device Descriptor.
 */
struct max22516_dev {
	/* SPI Initialization parameters */
	struct no_os_spi_desc	*spi_desc;
	/** Buffer used for communication with MAX22516 */
	uint8_t comm_buff[MAX22516_BUFF_SIZE_BYTES];
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* MAX22516 SPI write */
int max22516_write(struct max22516_dev *dev, uint8_t reg_addr,
		   uint8_t data);

/* MAX22516 SPI Read */
int max22516_read(struct max22516_dev *dev, uint8_t reg_addr,
		  uint8_t *data);

/* MAX22516 Register Update */
int max22516_update(struct max22516_dev *dev, uint8_t reg_addr,
		    uint8_t mask, uint8_t data);

/* MAX22516 Burst Write */
int max22516_burst_write_register(struct max22516_dev *dev, uint8_t reg_addr,
				  uint8_t count, uint8_t *data);

/* MAX22516 Burst Read */
int max22516_burst_read_register(struct max22516_dev *dev, uint8_t reg_addr,
				 uint8_t count, uint8_t *data);

/* MAX22516 build tcyc */
void max22516_build_tcyc(int16_t t, uint8_t *tmr);

/* MAX22516 rebuild min cyct to microseconds */
void max22516_rebuild_min_cyct_to_us(int16_t t, uint8_t *tmr);

/* MAX22516 set min ctmr */
int max22516_set_min_ctmr(struct max22516_dev *dev, uint16_t min_t);

/* MAX22516 set id */
int max22516_set_id(struct max22516_dev *dev, uint16_t vid, uint32_t id,
		    uint16_t fid);

/* MAX22516 decode tcyc */
void max22516_decode_tcyc(uint8_t tmr, int16_t *t);

/* MAX22516 get mst ctmr */
int max22516_get_mst_ctmr(struct max22516_dev *dev, uint16_t min_t,
			  int16_t *c_tmr);

/* MAX22516 get dl mode */
int max22516_get_dl_mode(struct max22516_dev *dev, uint8_t *mode);

/* MAX22516 get iol err cnt */
int max22516_get_iol_err_cnt(struct max22516_dev *dev, uint8_t *cnt);

/* MAX22516 get frm err cnt */
int max22516_get_frm_err_cnt(struct max22516_dev *dev, uint8_t *cnt);

/* MAX22516 clr iol err cnt */
int max22516_clr_iol_err_cnt(struct max22516_dev *dev);

/* MAX22516 clr frm err cnt */
int max22516_clr_frm_err_cnt(struct max22516_dev *dev);

/* MAX22516 set led1 */
int max22516_set_led1(struct max22516_dev *dev, uint16_t ltmr);

/* MAX22516 set led2 */
int max22516_set_led2(struct max22516_dev *dev, uint16_t ltmr);

/* MAX22516 get v24 */
int max22516_get_v24(struct max22516_dev *dev, uint8_t *status3);

/* MAX22516 get THD */
int max22516_get_thd(struct max22516_dev *dev, uint8_t *status3);

/* MAX22516 setup cq dis */
int max22516_setup_cq_dis(struct max22516_dev *dev);

/* MAX22516 setup cq pp */
int max22516_setup_cq_pp(struct max22516_dev *dev);

/* MAX22516 setup cq pnp */
int max22516_setup_cq_pnp(struct max22516_dev *dev);

/* MAX22516 setup cq npn */
int max22516_setup_cq_npn(struct max22516_dev *dev);

/* MAX22516 tx set */
int max22516_tx_set(struct max22516_dev *dev, uint8_t low_high);

/* MAX22516 txen set */
int max22516_txen_set(struct max22516_dev *dev, uint8_t lvl);

/* MAX22516 set cq */
int max22516_set_cq(struct max22516_dev *dev, uint8_t lvl);

/* MAX22516 tx get */
int max22516_tx_get(struct max22516_dev *dev, uint8_t *low_high);

/* MAX22516 get cq */
int max22516_get_cq(struct max22516_dev *dev, uint8_t *cq);

/* MAX22516 get cq stat */
int max22516_get_cq_stat(struct max22516_dev *dev, uint8_t *status3);

/* MAX22516 get cq */
int max22516_setup_do_dis(struct max22516_dev *dev);

/* MAX22516 setup do pp */
int max22516_setup_do_pp(struct max22516_dev *dev);

/* MAX22516 setup do pnp */
int max22516_setup_do_pnp(struct max22516_dev *dev);

/* MAX22516 setup do npn */
int max22516_setup_do_npn(struct max22516_dev *dev);

/* MAX22516 do set */
int max22516_do_set(struct max22516_dev *dev, uint8_t lvl);

/* MAX22516 do get */
int max22516_do_get(struct max22516_dev *dev, uint8_t *lvl);

/* MAX22516 get do stat */
int max22516_get_do_stat(struct max22516_dev *dev, uint8_t *status3);

/* MAX22516 set event */
int max22516_set_event(struct max22516_dev *dev, uint8_t ev_qual,
		       uint16_t ev_code);

/* MAX22516 setup watchdog */
int max22516_setup_watchdog(struct max22516_dev *dev, uint8_t wd_timeout,
			    uint8_t wd_clr, uint8_t wd_event_en,
			    uint8_t wd_event_flag);

/* MAX22516 Initialization */
int max22516_init(struct max22516_dev **device,
		  struct max22516_init_param *init_param);

/* MAX22516 Resources Deallocation */
int max22516_remove(struct max22516_dev *dev);

#endif /* MAX22516_H_ */
