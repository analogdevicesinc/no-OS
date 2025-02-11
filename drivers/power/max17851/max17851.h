/***************************************************************************//**
 *   @file   max17851.h
 *   @brief  Header file of the MAX17851 driver
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef _MAX17851_H_
#define _MAX17851_H_

#include <stdint.h>
#include <string.h>
#include "no_os_uart.h"
#include "no_os_gpio.h"
#include "no_os_spi.h"
#include "no_os_util.h"

#define MAX17851_UART_MAX_BYTES_NB		31

#define MAX17851_FRAME_SIZE			2

#define MAX17851_RD_REG(reg)			((reg) + 1)

#define MAX17851_GPIO_RECOVERY_DELAY_DISABLED	0xFF
#define MAX17851_CONTACT_TIMER_DELAY_INFINITE	0xFF

#define MAX17851_CONFIG_GEN2_MASK(sel)		NO_OS_BIT(sel)
#define MAX17851_ALRTPCKT_TIME_MASK		NO_OS_GENMASK(3, 0)
#define MAX17851_LOADQ_MASK			NO_OS_GENMASK(1, 0)
#define MAX17851_TXQ_MASK			NO_OS_GENMASK(3, 2)
#define MAX17851_ALRT_MASK(alrt)		NO_OS_BIT(((alrt) % 8))
#define MAX17851_BAUD_MASK			NO_OS_GENMASK(6, 4)
#define MAX17851_SINGLE_MASK			NO_OS_BIT(7)
#define MAX17851_MS_MASK			NO_OS_GENMASK(5, 4)
#define MAX17851_ALRTPCKT_DBNC_MASK		NO_OS_GENMASK(5, 3)
#define MAX17851_SAFEMON_GPIO_MASK(x)		NO_OS_BIT(x)
#define MAX17851_COMM_RETRY_COUNT_MASK		NO_OS_GENMASK(5, 4)
#define MAX17851_COMM_TIMEOUT_DELAY_MASK	NO_OS_GENMASK(2, 0)

#define MAX17851_STATUS_RX_REG			0x00
#define MAX17851_STATUS_TX_REG			0x02
#define MAX17851_STATUS_LSSM_BYTE_REG		0x04
#define MAX17851_STATUS_GEN_REG			0x06
#define MAX17851_STATUS_OPSTATE_REG		0x08
#define MAX17851_STATUS_BUF_REG			0x0A
#define MAX17851_STATUS_WD_REG			0x0C
#define MAX17851_STATUS_GPIO_REG		0x0E

#define MAX17851_ALERT_RX_REG			0x10
#define MAX17851_ALERT_TX_REG			0x12
#define MAX17851_ALERT_LSSM_BYTE_REG		0x14
#define MAX17851_ALERT_GEN_REG			0x16
#define MAX17851_ALERT_OPSTATE_REG		0x18
#define MAX17851_ALERT_BUF_REG			0x1A
#define MAX17851_ALERT_WD_REG			0x1C
#define MAX17851_ALERT_GPIO_REG			0x1E

#define MAX17851_ALRTEN_RX_REG			0x20
#define MAX17851_ALRTEN_TX_REG			0x22
#define MAX17851_ALRTEN_LSSM_BYTE_REG		0x24
#define MAX17851_ALRTEN_GEN_REG			0x26
#define MAX17851_ALRTEN_OPSTATE_REG		0x28
#define MAX17851_ALRTEN_BUF_REG			0x2A
#define MAX17851_ALRTEN_WD_REG			0x2C
#define MAX17851_ALRTEN_GPIO_REG		0x2E

#define MAX17851_CLR_TXBUF_REG			0x40
#define MAX17851_CLR_RXBUF_REG			0x42
#define MAX17851_CLR_LSSM_REG			0x44
#define MAX17851_CLR_ALIVECOUNT_SEED_REG	0x48
#define MAX17851_SWPOR_REG			0x4A
#define MAX17851_SLP_EN_REG			0x4C
#define MAX17851_VER_CONFIG_REG			0x4E
#define MAX17851_LOAD_CONFIG_REG		0x50
#define MAX17851_WD_KEY_REG			0x52

#define MAX17851_CONFIG_GEN0_REG		0x60
#define MAX17851_CONFIG_GEN1_REG		0x62
#define MAX17851_CONFIG_GEN2_REG		0x64
#define MAX17851_CONFIG_GEN3_REG		0x66
#define MAX17851_CONFIG_GEN4_REG		0x68
#define MAX17851_CONFIG_GEN5_REG		0x6A
#define MAX17851_CONFIG_SAFEMON0_REG		0x6C
#define MAX17851_CONFIG_SAFEMON1_REG		0x6E
#define MAX17851_CONFIG_SAFEMON2_REG		0x70
#define MAX17851_CONFIG_SAFEMON3_REG		0x72
#define MAX17851_CONFIG_SLP_REG			0x74
#define MAX17851_CONFIG_COMM_REG		0x76
#define MAX17851_STATUS_DBNC_MASK0_REG		0x78
#define MAX17851_STATUS_DBNC_MASK1_REG		0x7A
#define MAX17851_STATUS_ERR_MASK0_REG		0x7C
#define MAX17851_STATUS_ERR_MASK1_REG		0x7E
#define MAX17851_CONFIG_GPIO12_REG		0x80
#define MAX17851_CONFIG_GPIO34_REG		0x82
#define MAX17851_CONFIG_WD0_REG			0x84
#define MAX17851_CONFIG_WD1_REG			0x86
#define MAX17851_CONFIG_WD2_REG			0x88

#define MAX17851_ALRTPCKTBUF_RD_PTR_REG		0x8C
#define MAX17851_ALRTPCKTBUF_RD_MSG_REG		0x8E
#define MAX17851_RX_RD_MSG_REG			0x90
#define MAX17851_RX_RD_NXT_MSG_REG		0x92
#define MAX17851_TX_QUEUE_SEL_REG		0x94
#define MAX17851_RX_RD_PTR_REG			0x96
#define MAX17851_RX_WR_PTR_REG			0x98
#define MAX17851_RX_NXT_MSG_PTR_REG		0x9A
#define MAX17851_RX_SPACE_REG			0x9C
#define MAX17851_RX_BYTE_REG			0x9E

#define MAX17851_NXT_LDQ_REG			0xB0
#define MAX17851_LDQ_REG			0xC0
#define MAX17851_LDQ_PTR_REG			0xC2
#define MAX17851_CONFIGQ_REG			0xD0
#define MAX17851_CONFIG_PTR_REG			0xD2

#define MAX17851_STATE_REG			0xDC
#define MAX17851_COMM_RTRY_CNT_REG		0xDE
#define MAX17851_ALRTPCKT_ERR_CNT_REG		0xE0
#define MAX17851_WD_FAULT_CNT_REG		0xE2
#define MAX17851_ALIVECOUNT_SEED_REG		0xE4
#define MAX17851_ALIVECOUNT_RET_REG		0xE6
#define MAX17851_ALIVECOUNT_Q_REG		0xE8
#define MAX17851_FAULT_TIMER0_REG		0xEA
#define MAX17851_FAULT_TIMER1_REG		0xEC
#define MAX17851_SLP_CBTIMER0_REG		0xEE
#define MAX17851_SLP_CBTIMER1_REG		0xF0
#define MAX17851_VERSION_REG			0xF2
#define MAX17851_MODEL_REG			0xF4

enum max17851_command {
	MAX17851_CLR_LSSM = MAX17851_CLR_LSSM_REG,
	MAX17851_CLR_ALIVECOUNT_SEED = MAX17851_CLR_ALIVECOUNT_SEED_REG,
	MAX17851_SWPOR = MAX17851_SWPOR_REG,
	MAX17851_SLP_EN = MAX17851_SLP_EN_REG,
	MAX17851_VER_CONFIG = MAX17851_VER_CONFIG_REG,
	MAX17851_LOAD_CONFIG = MAX17851_LOAD_CONFIG_REG,
	MAX17851_WD_KEY = MAX17851_WD_KEY_REG,
};

enum max17851_alert {
	MAX17851_RX_EMPTY_ALRT,
	MAX17851_RX_STOP_ALRT,
	MAX17851_RX_OVRFLW_ERR_ALRT,
	MAX17851_RX_IDLE_ALRT,
	MAX17851_RX_BUSY_ALRT,
	MAX17851_RX_ERR_ALRT = 7,
	MAX17851_TX_EMPTY_ALRT,
	MAX17851_TX_AVAIL_ALRT,
	MAX17851_TX_FULL_ALRT,
	MAX17851_TX_OVRFLW_ERR_ALRT,
	MAX17851_TX_IDLE_ALRT,
	MAX17851_TX_BUSY_ALRT,
	MAX17851_TX_HELD_ALRT = 15,
	MAX17851_HW_ERR_ALRT,
	MAX17851_ALIVECNT_ERR_ALRT,
	MAX17851_COMMAND_OP_ALRT,
	MAX17851_COMM_MSMTCH_ERR_ALRT,
	MAX17851_ALRTPCKT_ERR_ALRT,
	MAX17851_COMM_ERR_ALRT,
	MAX17851_ALRTPCKT_STATUS_ERR_ALRT,
	MAX17851_RX_READY_ALRT,
	MAX17851_ALRTPCKTBUF_HW_ERR_ALRT,
	MAX17851_ALRTPCKT_COMM_ERR_ALRT,
	MAX17851_SPI_ERR_ALRT,
	MAX17851_DATAPATH_ERR_ALRT,
	MAX17851_GPIO_ERR_ALRT,
	MAX17851_WD_ERR_ALRT,
	MAX17851_DEV_COUNT_ERR_ALRT,
	MAX17851_SAFEMON_CFG_ERR_ALRT = 32,
	MAX17851_SAFEMON_STATUS_ERR_ALRT,
	MAX17851_SAFEMON_GPIO12_ALRT,
	MAX17851_SAFEMON_ALRT,
	MAX17851_SLP_STATUS_ERR_ALRT,
	MAX17851_SLP_ALRT,
	MAX17581_LSSM_FULL_ALRT = 43,
	MAX17851_ALRTPCKTBUF_FULL_ALRT = 47,
	MAX17851_WD_EXP_ERR_ALRT,
	MAX17851_WD_RJCT_ERR_ALRT,
	MAX17851_WD_LFSR_ERR_ALRT,
	MAX17851_WD_OPEN_ALRT,
	MAX17851_WD_TO_ERR_ALRT,
	MAX17851_GPIO1_ERR_ALRT = 56,
	MAX17851_GPIO2_ERR_ALRT,
	MAX17851_GPIO3_ERR_ALRT,
	MAX17851_GPIO4_ERR_ALRT,
};

enum max17851_queue_sel {
	MAX17851_LOADQ_0_SEL,
	MAX17851_LOADQ_1_SEL,
	MAX17851_LOADQ_2_SEL,
	MAX17851_LOADQ_3_SEL,
	MAX17851_TXQ_0_SEL,
	MAX17851_TXQ_1_SEL,
	MAX17851_TXQ_2_SEL,
	MAX17851_TXQ_3_SEL,
};

enum max17851_config_gen2_sel {
	MAX17851_TX_NOPREAMBLE_SEL,
	MAX17851_TX_NOSTOP_SEL,
	MAX17851_TX_PAUSE_SEL,
	MAX17851_TX_ODDPARITY_SEL,
	MAX17851_TX_QUEUE_SEL,
	MAX17851_TX_PREAMBLES_SEL,
	MAX17851_TX_RAW_DATA_SEL,
	MAX17851_RX_RAW_DATA_SEL,
};

enum max17851_alrtpckt_time {
	MAX17851_ALRTPCKT_0US,
	MAX17851_ALRTPCKT_10US,
	MAX17851_ALRTPCKT_20US,
	MAX17851_ALRTPCKT_40US,
	MAX17851_ALRTPCKT_80US,
	MAX17851_ALRTPCKT_160US,
	MAX17851_ALRTPCKT_320US,
	MAX17851_ALRTPCKT_640US,
	MAX17851_ALRTPCKT_1280US,
	MAX17851_ALRTPCKT_2560US,
	MAX17851_ALRTPCKT_5120US,
	MAX17851_ALRTPCKT_10240US,
	MAX17851_ALRTPCKT_INFINITE = 15,
};

enum max17851_baud_rate {
	MAX17851_UART_BAUD_500K = 1,
	MAX17851_UART_BAUD_1M,
	MAX17851_UART_BAUD_2M,
};

enum max17851_uart_op_mode {
	MAX17851_SLAVE_DUAL_UART,
	MAX17851_MASTER_SINGLE_UART = 2,
	MAX17851_MASTER_DUAL_UART,
};

enum max17851_status_debounce {
	MAX17851_ALERTPCKT_DBNC_1,
	MAX17851_ALERTPCKT_DBNC_2,
	MAX17851_ALERTPCKT_DBNC_4,
	MAX17851_ALERTPCKT_DBNC_8,
	MAX17851_ALERTPCKT_DBNC_16,
	MAX17851_ALERTPCKT_DBNC_32,
	MAX17851_ALERTPCKT_DBNC_64,
	MAX17851_ALERTPCKT_DBNC_128,
};

enum max17851_safemon_dly {
	MAX17851_SAFEMON_DLY_500MS,
	MAX17851_SAFEMON_DLY_1000MS,
	MAX17851_SAFEMON_DLY_1500MS,
};

enum max17851_safemon_gpio {
	MAX17851_SAFEMON_GPIO1,
	MAX17851_SAFEMON_GPIO2,
	MAX17851_SAFEMON_GPIO3,
	MAX17851_SAFEMON_GPIO4,
};

enum max17851_comm_try_count {
	MAX17851_RETRY_COUNT_2,
	MAX17851_RETRY_COUNT_4,
	MAX17851_RETRY_COUNT_8,
	MAX17851_RETRY_COUNT_16,
};

enum max17851_comm_timeout_delay {
	MAX17851_TIMEOUT_DELAY_276BITS,
	MAX17851_TIMEOUT_DELAY_516BITS,
	MAX17851_TIMEOUT_DELAY_996BITS,
	MAX17851_TIMEOUT_DELAY_1956BITS,
	MAX17851_TIMEOUT_DELAY_DISABLED,
};

struct max17851_init_param {
	struct no_os_spi_init_param *spi_param;

	struct no_os_gpio_init_param *gpio1_param;
	struct no_os_gpio_init_param *gpio2_param;

	enum max17851_comm_timeout_delay comm_to_delay;
	enum max17851_safemon_dly safemon_dly;
	enum max17851_uart_op_mode op_mode;
	enum max17851_status_debounce dbnc;
	enum max17851_baud_rate baud_rate;

	uint8_t contact_tmr_dly_4xmin;
	uint8_t gpio_rec_dly_csec;

	uint8_t no_dev;
	bool single;
};

struct max17851_desc {
	struct no_os_spi_desc *spi_desc;

	struct no_os_gpio_desc *gpio1_desc;
	struct no_os_gpio_desc *gpio2_desc;

	enum max17851_comm_timeout_delay comm_to_delay;
	enum max17851_safemon_dly safemon_dly;
	enum max17851_uart_op_mode op_mode;
	enum max17851_status_debounce dbnc;
	enum max17851_baud_rate baud_rate;

	uint8_t contact_tmr_dly_4xmin;
	uint8_t gpio_rec_dly_csec;

	uint8_t lssm_byte;
	uint8_t buff[33];
	uint8_t no_dev;
	bool single;
};

int max17851_send_command(struct max17851_desc *desc,
			  enum max17851_command command, bool enable);

int max17851_read_msg(struct max17851_desc *desc, uint8_t *data, uint8_t len);

int max17851_write_msg(struct max17851_desc *desc, const uint8_t *data,
		       uint8_t len);

int max17851_config_gen2(struct max17851_desc *desc,
			 enum max17851_config_gen2_sel sel, bool enable);

int max17851_sel_alrtpckt_time(struct max17851_desc *desc,
			       enum max17851_alrtpckt_time time);

int max17851_select_queue(struct max17851_desc *desc,
			  enum max17851_queue_sel sel);

int max17851_clear_buff(struct max17851_desc *desc, bool tx_buf);

int max17851_get_alert(struct max17851_desc *desc, enum max17851_alert alert,
		       bool *enable);

int max17851_set_alert(struct max17851_desc *desc, enum max17851_alert alert,
		       bool enable);

int max17851_set_alert_debounce(struct max17851_desc *desc,
				enum max17851_status_debounce dbnc);

int max17851_set_safemon_dly(struct max17851_desc *desc,
			     enum max17851_safemon_dly delay);

int max17851_set_safemon_gpio(struct max17851_desc *desc,
			      enum max17851_safemon_gpio gpio, bool enable);

int max17851_set_comm(struct max17851_desc *desc,
		      enum max17851_comm_try_count count,
		      enum max17851_comm_timeout_delay delay);

int max17851_reg_read(struct max17851_desc *desc, uint8_t reg, uint8_t *data);

int max17851_reg_write(struct max17851_desc *desc, uint8_t reg, uint8_t data);

int max17851_reg_update(struct max17851_desc *desc, uint8_t reg, uint8_t mask,
			uint8_t data);

int max17851_init(struct max17851_desc **desc,
		  struct max17851_init_param *init_param);

int max17851_remove(struct max17851_desc *desc);

extern const struct no_os_uart_platform_ops max17851_uart_ops;

#endif /* _MAX17851_H_ */
