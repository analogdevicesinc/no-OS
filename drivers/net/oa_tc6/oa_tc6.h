/*******************************************************************************
 *   @file   oa_tc6.h
 *   @brief  Header file for the Open Alliance TC6 SPI driver.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
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
#ifndef _NO_OS_OA_TC6_H
#define _NO_OS_OA_TC6_H

#include "no_os_spi.h"
#include "no_os_util.h"
#include <stdint.h>

#ifndef CONFIG_OA_TX_FRAME_BUFF_NUM
#define CONFIG_OA_TX_FRAME_BUFF_NUM	2
#endif

#ifndef CONFIG_OA_RX_FRAME_BUFF_NUM
#define CONFIG_OA_RX_FRAME_BUFF_NUM	5
#endif

#ifndef CONFIG_OA_CHUNK_BUFFER_SIZE
#define CONFIG_OA_CHUNK_BUFFER_SIZE	1514
#endif

#define OA_TX_FRAME_BUFF_NUM		CONFIG_OA_TX_FRAME_BUFF_NUM
#define OA_RX_FRAME_BUFF_NUM		CONFIG_OA_RX_FRAME_BUFF_NUM

/* Space for one full frame + 24 chunk headers (68 * 24)*/
#define OA_SPI_BUFF_LEN		1632

/* Space for 2 Header + Reg Data + Inverse Reg Data (PROTE) */
#define OA_SPI_CTRL_LEN		16

#define OA_CHUNK_SIZE		64
#define OA_REG_LEN		4
#define OA_HEADER_LEN		4
#define OA_FOOTER_LEN		4

#define OA_MMS_REG(m, r)	(((m) << 16) | ((r) & NO_OS_GENMASK(15, 0)))
#define OA_CTRL_ADDR_MMS_MASK	NO_OS_GENMASK(27, 8)

#define OA_CTRL_HDRB_MASK	NO_OS_BIT(30)
#define OA_CTRL_WNR_MASK	NO_OS_BIT(29)
#define OA_CTRL_AID_MASK	NO_OS_BIT(28)
#define OA_CTRL_MMS_MASK	NO_OS_GENMASK(27, 24)
#define OA_CTRL_ADDR_MASK	NO_OS_GENMASK(23, 8)
#define OA_CTRL_LEN_MASK	NO_OS_GENMASK(7, 1)
#define OA_CTRL_P_MASK		NO_OS_BIT(0)

#define OA_DATA_FOOTER_EXST_MASK	NO_OS_BIT(31)
#define OA_DATA_FOOTER_HDRB_MASK	NO_OS_BIT(30)
#define OA_DATA_FOOTER_SYNC_MASK	NO_OS_BIT(29)
#define OA_DATA_FOOTER_RCA_MASK		NO_OS_GENMASK(28, 24)
#define OA_DATA_FOOTER_VS_MASK		NO_OS_GENMASK(23, 22)
#define OA_DATA_FOOTER_DV_MASK		NO_OS_BIT(21)
#define OA_DATA_FOOTER_SV_MASK		NO_OS_BIT(20)
#define OA_DATA_FOOTER_SWO_MASK		NO_OS_GENMASK(19, 16)
#define OA_DATA_FOOTER_EV_MASK		NO_OS_BIT(14)
#define OA_DATA_FOOTER_EBO_MASK		NO_OS_GENMASK(13, 8)
#define OA_DATA_FOOTER_TXC_MASK		NO_OS_GENMASK(5, 1)
#define OA_DATA_FOOTER_P_MASK		NO_OS_BIT(0)

#define OA_DATA_HEADER_DNC_MASK		NO_OS_BIT(31)
#define OA_DATA_HEADER_SEQ_MASK		NO_OS_BIT(30)
#define OA_DATA_HEADER_NORX_MASK	NO_OS_BIT(29)
#define OA_DATA_HEADER_VS_MASK		NO_OS_GENMASK(23, 22)
#define OA_DATA_HEADER_DV_MASK		NO_OS_BIT(21)
#define OA_DATA_HEADER_SV_MASK		NO_OS_BIT(20)
#define OA_DATA_HEADER_SWO_MASK		NO_OS_GENMASK(19, 16)
#define OA_DATA_HEADER_EV_MASK		NO_OS_BIT(14)
#define OA_DATA_HEADER_EBO_MASK		NO_OS_GENMASK(13, 8)
#define OA_DATA_HEADER_P_MASK		NO_OS_BIT(0)

/* Standard control and status registers (MMS 0) */

#define OA_TC6_IDVER_REG		OA_MMS_REG(0x0, 0x0000)
#define OA_TC6_PHYID_REG		OA_MMS_REG(0x0, 0x0001)
#define OA_TC6_STDCAP_REG		OA_MMS_REG(0x0, 0x0002)
#define OA_TC6_RESET_REG		OA_MMS_REG(0x0, 0x0003)
#define OA_TC6_CONFIG0_REG		OA_MMS_REG(0x0, 0x0004)
#define OA_TC6_CONFIG1_REG		OA_MMS_REG(0x0, 0x0005)
#define OA_TC6_CONFIG2_REG		OA_MMS_REG(0x0, 0x0006)
#define OA_TC6_STATUS0_REG		OA_MMS_REG(0x0, 0x0008)
#define OA_TC6_STATUS1_REG		OA_MMS_REG(0x0, 0x0009)
#define OA_TC6_BUFST_REG		OA_MMS_REG(0x0, 0x000B)
#define OA_TC6_IMSK0_REG		OA_MMS_REG(0x0, 0x000C)
#define OA_TC6_IMSK1_REG		OA_MMS_REG(0x0, 0x000D)

#define OA_TC6_TTSCAH_REG		OA_MMS_REG(0x0, 0x0010)
#define OA_TC6_TTSCAL_REG		OA_MMS_REG(0x0, 0x0011)
#define OA_TC6_TTSCBH_REG		OA_MMS_REG(0x0, 0x0012)
#define OA_TC6_TTSCBL_REG		OA_MMS_REG(0x0, 0x0013)
#define OA_TC6_TTSCCH_REG		OA_MMS_REG(0x0, 0x0014)
#define OA_TC6_TTSCCL_REG		OA_MMS_REG(0x0, 0x0015)
#define OA_TC6_MDIOACC0_REG		OA_MMS_REG(0x0, 0x0020)
#define OA_TC6_MDIOACC1_REG 		OA_MMS_REG(0x0, 0x0021)
#define OA_TC6_MDIOACC2_REG 		OA_MMS_REG(0x0, 0x0022)
#define OA_TC6_MDIOACC3_REG 		OA_MMS_REG(0x0, 0x0023)
#define OA_TC6_MDIOACC4_REG 		OA_MMS_REG(0x0, 0x0024)
#define OA_TC6_MDIOACC5_REG 		OA_MMS_REG(0x0, 0x0025)
#define OA_TC6_MDIOACC6_REG 		OA_MMS_REG(0x0, 0x0026)
#define OA_TC6_MDIOACC7_REG 		OA_MMS_REG(0x0, 0x0027)

#define OA_TC6_CONFIG0_ZARFE_MASK	NO_OS_BIT(12)

#define OA_TC6_BUFSTS_TXC_MASK 		NO_OS_GENMASK(15, 8)
#define OA_TC6_BUFSTS_RCA_MASK 		NO_OS_GENMASK(7, 0)

/**
 * @brief State for data buffers containing Ethernet frames
 */
enum oa_tc6_user_buffer_state {
	/*
	 * Neither the OA nor the user application doesn't currently
	 * use this buffer. OA can start writing it.
	 */
	OA_BUFF_FREE,

	/*
	 * The buffer is currently written by OA, but it doesn't yet
	 * contain a complete frame. The user application cannot reference
	 * the data yet.
	 */
	OA_BUFF_RX_IN_PROGRESS,

	/*
	 * The buffer has a complete frame, and is no longer accessed by OA.
	 * The user application could use this.
	 */
	OA_BUFF_RX_COMPLETE,

	/* The buffer is accessed by the user application. OA will not overwrite it */
	OA_BUFF_RX_USER_OWNED,

	/* The user writes to this buffer. OA doesn't access it. */
	OA_BUFF_TX_BUSY,

	/* The buffer is ready to be transmitted. The user won't access it anymore. */
	OA_BUFF_TX_READY,
};

/**
 * @brief Stores an Ethernet frame along with metadata needed for parsing.
 * The MAC driver or the user application will receive and submit frames for transmission
 * in this format.
 */
struct oa_tc6_frame_buffer {
	uint32_t index;
	uint32_t len;
	uint8_t data[CONFIG_OA_CHUNK_BUFFER_SIZE];
	enum oa_tc6_user_buffer_state state;
	uint8_t vs;
};

/**
 * @brief Holds the frame buffers and the communication descriptor for the OA TC6 driver.
 */
struct oa_tc6_desc {
	struct no_os_spi_desc *comm_desc;
	uint8_t ctrl_chunks[OA_SPI_CTRL_LEN];
	uint8_t data_chunks[OA_SPI_BUFF_LEN];

	struct oa_tc6_frame_buffer user_rx_frame_buffer[OA_RX_FRAME_BUFF_NUM];
	struct oa_tc6_frame_buffer user_tx_frame_buffer[OA_TX_FRAME_BUFF_NUM];

	uint32_t data_tx_credit;
	uint32_t data_rx_credit;

	uint32_t ctrl_tx_credit;
	uint32_t ctrl_rx_credit;
	bool	 prote_spi;
};

/**
 * @brief Holds the initialization parameters for the OA TC6 driver.
 */
struct oa_tc6_init_param {
	struct no_os_spi_desc *comm_desc;

	/* The OASPI device uses Protected SPI for control transactions */
	bool prote_spi;
};

/* Read a register from the MAC device */
int oa_tc6_reg_read(struct oa_tc6_desc *, uint32_t, uint32_t *);

/* Write a register of the MAC device */
int oa_tc6_reg_write(struct oa_tc6_desc *, uint32_t, uint32_t);

/* Update a register field */
int oa_tc6_reg_update(struct oa_tc6_desc *, uint32_t, uint32_t, uint32_t);

/* Get a received frame with a matching VS field (in the RX chunks) */
int oa_tc6_get_rx_frame_match_vs(struct oa_tc6_desc *,
				 struct oa_tc6_frame_buffer **,
				 uint8_t, uint8_t);

/* Get the first frame in the RX queue */
int oa_tc6_get_rx_frame(struct oa_tc6_desc *, struct oa_tc6_frame_buffer **);

/* Mark the frame buffer as ready to be reused for a new frame. */
int oa_tc6_put_rx_frame(struct oa_tc6_desc *, struct oa_tc6_frame_buffer *);

/* Get a frame buffer which can be filled and submitted for transmission */
int oa_tc6_get_tx_frame(struct oa_tc6_desc *, struct oa_tc6_frame_buffer **);

/* Mark the frame buffer as filled and ready for transmission */
int oa_tc6_put_tx_frame(struct oa_tc6_desc *, struct oa_tc6_frame_buffer *);

/*
 * Transmit all the frames in the OA_BUFF_TX_READY state and receive the
 * available chunks.
 */
int oa_tc6_thread(struct oa_tc6_desc *);

/* Initialize the OA TC6 SPI driver */
int oa_tc6_init(struct oa_tc6_desc **, struct oa_tc6_init_param *);

/* Free the resources allocated by the oa_tc6_init() function */
int oa_tc6_remove(struct oa_tc6_desc *);

#endif /* _NO_OS_OA_TC6_H */
