/***************************************************************************//**
 *   @file   stm32/stm32_xspi.c
 *   @brief  Implementation of stm32 xspi driver.
 *   @author Naga Himanshu Indraganti (naga.indraganti@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "no_os_irq.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"
#include "no_os_mutex.h"
#include "no_os_dma.h"
#include "stm32_gpio.h"
#include "stm32_dma.h"
#include "stm32_xspi.h"

/*
 * @brief Initialize the XSPI peripheral.
 * @param desc - The XSPI descriptor
 * @param param - The structure that contains the XSPI parameters.
 * @return 0 in case of success, negative code otherwise.
 */
static int32_t stm32_xspi_peripheral_init(struct no_os_spi_desc *desc,
		const struct no_os_spi_init_param *param)
{
	int32_t ret;
	uint32_t prescaler = 0u;
	struct stm32_xspi_desc *sdesc;
	XSPI_TypeDef *base = NULL;

	if (!desc || !param)
		return -EINVAL;

	sdesc = desc->extra;

	/* Calculate prescaler based on given clock frequency */
	if (desc->max_speed_hz != 0u) {
		uint32_t div = sdesc->input_clock / desc->max_speed_hz;

		/* If exact division is possible, use div - 1 (since the peripheral does
		 * clock frequency as 2 ^ (prescaler + 1) */
		if (!(sdesc->input_clock % desc->max_speed_hz)) {
			prescaler = div - 1;
		} else {
			prescaler = div;
		}
	} else {
		prescaler = 24;
	}

	switch (desc->device_id) {
	case 1:
#if defined(OCTOSPI1)
		base = OCTOSPI1;
		break;
#endif
#if defined(OCTOSPI2)
	case 2:
		base = OCTOSPI2;
		break;
#endif
#if defined(OCTOSPI3)
	case 3:
		base = OCTOSPI3;
		break;
#endif
#if defined(OCTOSPI4)
	case 4:
		base = OCTOSPI4;
		break;
#endif
	default:
		return -EINVAL;
	};

	/* Assign the XSPI base register address */
	sdesc->hxspi.Instance = base;

	/* Populate the initialization parameters of XSPI handle */
	sdesc->hxspi.Init.MemoryMode = HAL_XSPI_SINGLE_MEM;
	sdesc->hxspi.Init.MemoryType = HAL_XSPI_MEMTYPE_MICRON;
	sdesc->hxspi.Init.MemorySize = 0;
	sdesc->hxspi.Init.FreeRunningClock = 0;
	sdesc->hxspi.Init.WrapSize = HAL_XSPI_WRAP_NOT_SUPPORTED;
	sdesc->hxspi.Init.DelayHoldQuarterCycle = HAL_XSPI_DHQC_DISABLE;
	sdesc->hxspi.Init.ChipSelectBoundary = HAL_XSPI_BONDARYOF_NONE;
	sdesc->hxspi.Init.DelayBlockBypass = HAL_XSPI_DELAY_BLOCK_BYPASS;
	sdesc->hxspi.Init.Refresh = 0;

	sdesc->hxspi.Init.FifoThresholdByte = sdesc->fifo_threshold;
	sdesc->hxspi.Init.ChipSelectHighTimeCycle = sdesc->cs_high_time;
	sdesc->hxspi.Init.ClockMode =
		param->mode & NO_OS_SPI_CPOL ?
		HAL_XSPI_CLOCK_MODE_3 :
		HAL_XSPI_CLOCK_MODE_0;
	sdesc->hxspi.Init.SampleShifting =
		sdesc->sample_shifting ?
		HAL_XSPI_SAMPLE_SHIFT_HALFCYCLE :
		HAL_XSPI_SAMPLE_SHIFT_NONE;
	sdesc->hxspi.Init.ClockPrescaler =
		(prescaler << XSPI_DCR2_PRESCALER_Pos);

	/* Initialize the XSPI peripheral */
	ret = HAL_XSPI_Init(&sdesc->hxspi);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Configure XSPI with init parameters.
 * @param desc - The XSPI descriptor
 * @param params - The structure that contains the XSPI parameters.
 * @return 0 in case of success, negative code otherwise.
 */
static int32_t stm32_xspi_config(struct no_os_spi_desc *desc,
				 const struct no_os_spi_init_param *params)
{
	int32_t ret = 0;
	struct stm32_xspi_desc *xspi_desc;
	XSPI_RegularCmdTypeDef *xspi_cmd;
	struct stm32_xspi_init_param *spi_params;
	struct stm32_xspi_command *cmd;

	if (!desc || !params)
		return -EINVAL;

	if (!desc->extra)
		return -EINVAL;

	xspi_desc = desc->extra;
	xspi_cmd = xspi_desc->command;
	spi_params = params->extra;

	/* Abort XSPI transaction before configuration */
	(void)HAL_XSPI_Abort(&xspi_desc->hxspi);

	/* Store the data mode in the descriptor */
	switch(params->lanes) {
	case NO_OS_SPI_SINGLE_LANE:
		xspi_cmd->DataMode = HAL_XSPI_DATA_1_LINE;
		break;
	case NO_OS_SPI_DUAL_LANE:
		xspi_cmd->DataMode = HAL_XSPI_DATA_2_LINES;
		break;
	case NO_OS_SPI_QUAD_LANE:
		xspi_cmd->DataMode = HAL_XSPI_DATA_4_LINES;
		break;
	case NO_OS_SPI_OCTO_LANE:
		xspi_cmd->DataMode = HAL_XSPI_DATA_8_LINES;
		break;
	default:
		xspi_cmd->DataMode = HAL_XSPI_DATA_NONE;
		break;
	}

	/* If device specific parameters are provided, configure them */
	if (spi_params != NULL) {
		cmd = spi_params->cmd;

		if (xspi_cmd != NULL && cmd != NULL) {
			xspi_cmd->Instruction = cmd->Instruction;
			xspi_cmd->InstructionMode = cmd->InstructionMode;
			xspi_cmd->InstructionWidth = cmd->InstructionWidth;
			xspi_cmd->Address = cmd->Address;
			xspi_cmd->AddressMode = cmd->AddressMode;
			xspi_cmd->AddressWidth = cmd->AddressWidth;
			xspi_cmd->DataLength = cmd->DataLength;
			xspi_cmd->AlternateBytes = cmd->AlternateBytes;
			xspi_cmd->AlternateBytesMode = cmd->AlternateBytesMode;
			xspi_cmd->AlternateBytesWidth = cmd->AlternateBytesWidth;
			xspi_cmd->DummyCycles = cmd->DummyCycles;
			xspi_cmd->IOSelect = 0;
			xspi_cmd->AddressDTRMode = 0;
			xspi_cmd->AlternateBytesDTRMode = 0;
			xspi_cmd->DataDTRMode = 0;
			xspi_cmd->DQSMode = 0;
			xspi_cmd->InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
			xspi_cmd->SIOOMode = 0;
		}
	}

	/* Configure the XSPI command */
	ret = HAL_XSPI_Command(&xspi_desc->hxspi, xspi_cmd,
			       HAL_XSPI_TIMEOUT_DEFAULT_VALUE);
	if (ret)
		return ret;

	/* Explicitly disable address mode */
	OCTOSPI1->CCR &= ~OCTOSPI_CCR_ADMODE_Msk;
	OCTOSPI1->CCR &= ~OCTOSPI_CCR_IMODE_Msk;

	return 0;
}

/**
 * @brief Initialize the XSPI communication peripheral.
 * @param desc - The XSPI descriptor.
 * @param param - The structure that contains the XSPI parameters.
 * @return 0 in case of success, negative code otherwise.
 */
static int32_t stm32_xspi_init(struct no_os_spi_desc **desc,
			       const struct no_os_spi_init_param *param)
{
	int32_t ret;
	struct no_os_spi_desc *xspi_desc = NULL;
	struct stm32_xspi_desc *sdesc = NULL;
	struct stm32_xspi_init_param *sinit;
	XSPI_RegularCmdTypeDef *xspi_cmd = NULL;

	if (!desc || !param)
		return -EINVAL;

	if (!param->extra)
		return -EINVAL;

	sinit = (struct stm32_xspi_init_param *) param->extra;

	/* Allocate memory for no-os xspi descriptor */
	xspi_desc = no_os_calloc(1, sizeof(*xspi_desc));
	if (!xspi_desc) {
		ret = -ENOMEM;
		goto xspi_desc_alloc_fail;
	}

	/* Allocate memory for stm32 xspi descriptor */
	sdesc = no_os_calloc(1, sizeof(*sdesc));
	if (!sdesc) {
		ret = -ENOMEM;
		goto sdesc_alloc_fail;
	}

	/* Allocate memory for STM32 specific platform XSPI command */
	xspi_cmd = no_os_calloc(1, sizeof(*xspi_cmd));
	if (!xspi_cmd) {
		ret = -ENOMEM;
		goto xspi_cmd_alloc_fail;
	}

	/* Copy settings to device descriptor */
	xspi_desc->device_id = param->device_id;
	xspi_desc->max_speed_hz = param->max_speed_hz;
	xspi_desc->extra = sdesc;
	xspi_desc->platform_ops = param->platform_ops;

	/* Assign the command pointer with the allocated memory */
	sdesc->command = xspi_cmd;

	/* Get the input clock for the XSPI */
	if (sinit->get_input_clock) {
		sdesc->input_clock = sinit->get_input_clock();
	}

	sdesc->sample_shifting = sinit->sample_shifting;
	sdesc->cs_high_time = sinit->cs_high_time;
	sdesc->fifo_threshold = sinit->fifo_threshold;

	/* Initialize the XSPI peripheral */
	ret = stm32_xspi_peripheral_init(xspi_desc, param);
	if (ret)
		goto error;

	/* Configure the XSPI with command */
	ret = stm32_xspi_config(xspi_desc, param);
	if (ret)
		goto error;

	/* If DMA for XSPI exists in init params, initialize DMA */
	if (sinit->dma_init) {
		ret = no_os_dma_init(&sdesc->dma_desc, sinit->dma_init);
		if (ret)
			goto error;

		if (sinit->dma_ch) {
			sdesc->dma_ch = &sdesc->dma_desc->channels[0];
			sdesc->dma_ch->id = sinit->dma_ch->hdma;
			sdesc->dma_ch->extra = sinit->dma_ch;
			sdesc->dma_ch->irq_num = sinit->irq_num;
		}
	}

	*desc = xspi_desc;

	return 0;

error:
	no_os_dma_remove(sdesc->dma_desc);
	no_os_free(xspi_cmd);
xspi_cmd_alloc_fail:
	no_os_free(sdesc);
sdesc_alloc_fail:
	no_os_free(xspi_desc);
xspi_desc_alloc_fail:
	return ret;
}

/**
 * @brief Free the resources allocated by no_os_spi_init().
 * @param desc - The XSPI descriptor.
 * @return 0 in case of success, negative code otherwise.
 */
static int32_t stm32_xspi_remove(struct no_os_spi_desc *desc)
{
	struct stm32_xspi_desc *sdesc;

	if (!desc || !desc->extra)
		return -EINVAL;

	sdesc = desc->extra;

	/* Remove the DMA descriptor */
	no_os_dma_remove(sdesc->dma_desc);

	/* De-Initialize XSPI peripheral */
	HAL_XSPI_DeInit(&sdesc->hxspi);

	/* Free the command */
	no_os_free(sdesc->command);

	/* Free the platform specific descriptor */
	no_os_free(sdesc);

	/* Free the no-os descriptor */
	no_os_free(desc);

	return 0;
}

/**
 * @brief Write/read multiple messages to/from XSPI.
 * @param desc - The XSPI descriptor.
 * @param msgs - The messages array.
 * @param len - Number of messages.
 * @return 0 in case of success, negative code otherwise.
 */
static int32_t stm32_xspi_transfer(struct no_os_spi_desc *desc,
				   struct no_os_spi_msg *msgs,
				   uint32_t len)
{
	int32_t ret;
	struct stm32_xspi_desc *sdesc;
	XSPI_RegularCmdTypeDef *cmd;

	if (!desc || !desc->extra || !msgs)
		return -EINVAL;

	sdesc = desc->extra;
	cmd = sdesc->command;

	for (uint32_t i = 0; i < len; i++) {
		if (!msgs[i].tx_buff && !msgs[i].rx_buff)
			return -EINVAL;

		if(msgs[i].cs_delay_first)
			no_os_udelay(msgs[i].cs_delay_first);

		/* Configure the command with updated length */
		cmd->DataLength = msgs[i].bytes_number;

		/* Abort XSPI transaction before command configuration */
		(void)HAL_XSPI_Abort(&sdesc->hxspi);

		ret = HAL_XSPI_Command(&sdesc->hxspi, sdesc->command,
				       HAL_XSPI_TIMEOUT_DEFAULT_VALUE);
		if (ret)
			return ret;

		/* Explicitly disable address mode */
		OCTOSPI1->CCR &= ~OCTOSPI_CCR_ADMODE_Msk;
		OCTOSPI1->CCR &= ~OCTOSPI_CCR_IMODE_Msk;

		if (msgs[i].tx_buff && !msgs[i].rx_buff)
			ret = HAL_XSPI_Transmit(&sdesc->hxspi, msgs[i].tx_buff, 0xFFFF);
		else if (!msgs[i].tx_buff && msgs[i].rx_buff)
			ret = HAL_XSPI_Receive(&sdesc->hxspi, msgs[i].rx_buff, 0xFFFF);
		else
			ret = -ENOTSUP;

		if (ret != HAL_OK) {
			if (ret == HAL_TIMEOUT)
				ret = -ETIMEDOUT;
			else
				ret = -EIO;
		}

		if(msgs[i].cs_delay_last)
			no_os_udelay(msgs[i].cs_delay_last);

		if(msgs[i].cs_change_delay)
			no_os_udelay(msgs[i].cs_change_delay);

		if (ret)
			return ret;
	}

	return 0;
}

/**
 * @brief Write and read data to/from XSPI.
 * @param desc - The XSPI descriptor.
 * @param data - The buffer with the transmitted/received data.
 * @param bytes_number - Number of bytes to write/read.
 * @return 0 in case of success, negative code otherwise.
 */
static int32_t stm32_xspi_write_and_read(struct no_os_spi_desc *desc,
		uint8_t *data,
		uint16_t bytes_number)
{
	struct no_os_spi_msg msg = {
		.bytes_number = bytes_number,
		.cs_change = true,
		.rx_buff = data,
		.tx_buff = data,
	};

	if (!desc || !desc->extra || !data)
		return -EINVAL;

	if (!bytes_number)
		return 0;

	return stm32_xspi_transfer(desc, &msg, 1);
}

static int32_t stm32_xspi_setup_dma_transfer(struct no_os_spi_desc* desc,
		struct no_os_dma_xfer_desc* ch_xfer)
{
	int32_t ret;
	struct stm32_xspi_desc* sdesc = desc->extra;
	uint32_t addr_reg = sdesc->hxspi.Instance->AR;
	uint32_t ir_reg = sdesc->hxspi.Instance->IR;

	/* Configure the command with initial message length */
	((XSPI_RegularCmdTypeDef *) sdesc->command)->DataLength = ch_xfer->length;

	/* Abort XSPI transaction before command configuration */
	(void) HAL_XSPI_Abort(&sdesc->hxspi);

	ret = HAL_XSPI_Command(&sdesc->hxspi,
			       sdesc->command,
			       HAL_XSPI_TIMEOUT_DEFAULT_VALUE);
	if (ret)
		return ret;

	/* Explicitly disable address mode */
	OCTOSPI1->CCR &= ~OCTOSPI_CCR_ADMODE_Msk;
	OCTOSPI1->CCR &= ~OCTOSPI_CCR_IMODE_Msk;

	/* Clear flags related to interrupt */
	WRITE_REG(sdesc->hxspi.Instance->FCR, HAL_XSPI_FLAG_TE | HAL_XSPI_FLAG_TC);

	if (ch_xfer->xfer_type == MEM_TO_DEV) {
		/* Configure CR register with functional mode as indirect write */
		MODIFY_REG(sdesc->hxspi.Instance->CR, XSPI_CR_FMODE, 0);
	} else {
		/* Configure CR register with functional mode as indirect read */
		MODIFY_REG(sdesc->hxspi.Instance->CR, XSPI_CR_FMODE, XSPI_CR_FMODE_0);

		if (sdesc->hxspi.Init.MemoryType == HAL_XSPI_MEMTYPE_HYPERBUS) {
			WRITE_REG(sdesc->hxspi.Instance->AR, sdesc->hxspi.Instance->AR);
		} else {
			if (READ_BIT(sdesc->hxspi.Instance->CCR, XSPI_CCR_ADMODE) !=
			    HAL_XSPI_ADDRESS_NONE) {
				WRITE_REG(sdesc->hxspi.Instance->AR, addr_reg);
			} else {
				WRITE_REG(sdesc->hxspi.Instance->IR, ir_reg);
			}
		}
	}

	/* Enable the DMA transfer by setting the DMAEN bit  */
	SET_BIT(sdesc->hxspi.Instance->CR, XSPI_CR_DMAEN);

	return 0;
}

static void stm32_xspi_dma_callback(struct no_os_dma_xfer_desc *old_xfer,
				    struct no_os_dma_xfer_desc *next_xfer,
				    void *ctx)
{
	int32_t ret;
	struct no_os_spi_desc* desc = ctx;
	struct stm32_xspi_desc* sdesc = desc->extra;
	XSPI_TypeDef *XSPIx = sdesc->hxspi.Instance;

	/* If more xfers pending configure dma and enable xspi dma transfer */
	if (next_xfer) {
		/* Configure DMA channel */
		ret = sdesc->dma_desc->platform_ops->dma_config_xfer(sdesc->dma_ch, next_xfer);
		if (ret)
			goto abort;

		/* Start DMA transfer */
		ret = no_os_dma_xfer_start(sdesc->dma_desc, sdesc->dma_ch);
		if (ret)
			goto abort;

		/* Setup DMA transfer for XSPI */
		ret = stm32_xspi_setup_dma_transfer(desc, next_xfer);
		if (ret)
			goto abort;

		return;
	}

abort:
	/* Disable the DMA transfer on the XSPI side */
	CLEAR_BIT(XSPIx->CR, XSPI_CR_DMAEN);

	no_os_dma_xfer_abort(sdesc->dma_desc, sdesc->dma_ch);

	no_os_free(sdesc->dma_ch_xfer);

	sdesc->stm32_xspi_dma_done = true;

	if (sdesc->stm32_xspi_dma_user_cb)
		sdesc->stm32_xspi_dma_user_cb(sdesc->stm32_xspi_dma_user_ctx);
}

/**
 * @brief Configure and start a series of transfers using DMA.
 * @param desc - The XSPI descriptor.
 * @param msgs - The messages array.
 * @param len - Number of messages.
 * @param callback - Function to be invoked after transfers
 * @param ctx - User defined parameter for the callback function.
 * @return 0 in case of success, errno codes otherwise.
 */
static int32_t stm32_xspi_config_dma_and_start(struct no_os_spi_desc* desc,
		struct no_os_spi_msg* msgs,
		uint32_t len,
		void (*callback)(
			struct no_os_dma_xfer_desc *old_xfer,
			struct no_os_dma_xfer_desc *next_xfer,
			void *ctx),
		void* ctx)
{
	struct stm32_xspi_desc* sdesc = desc->extra;
	XSPI_TypeDef* XSPIx = sdesc->hxspi.Instance;
	struct no_os_dma_xfer_desc* ch_xfer = NULL;
	int32_t ret;
	uint8_t i;
	bool xfer_tx; // Flag to indicate transmit

	if (!desc || !msgs)
		return -EINVAL;

	ch_xfer = no_os_calloc(len, sizeof(*ch_xfer));
	if (!ch_xfer)
		return -ENOMEM;

	for (i = 0; i < len; i++) {
		if (msgs[i].tx_buff != NULL && msgs[i].rx_buff == NULL) {
			xfer_tx = true;
		} else if (msgs[i].tx_buff == NULL && msgs[i].rx_buff != NULL) {
			xfer_tx = false;
		} else {
			ret = -ENOTSUP;
			goto free_ch_xfer;
		}

		if (xfer_tx) {
			ch_xfer[i].src = msgs[i].tx_buff;
			ch_xfer[i].dst = (uint8_t*)&(XSPIx->DR);
			ch_xfer[i].xfer_type = MEM_TO_DEV;
			ch_xfer[i].periph = NO_OS_DMA_IRQ;
			ch_xfer[i].length = msgs[i].bytes_number;
		} else {
			ch_xfer[i].src = (uint8_t*)&(XSPIx->DR);
			ch_xfer[i].dst = msgs[i].rx_buff;
			ch_xfer[i].periph = NO_OS_DMA_IRQ;
			ch_xfer[i].xfer_type = DEV_TO_MEM;
			ch_xfer[i].length = msgs[i].bytes_number;
		}

		if (callback) {
			ch_xfer[i].xfer_complete_cb = callback;
			ch_xfer[i].xfer_complete_ctx = ctx;
		}
	}

	sdesc->dma_ch_xfer = ch_xfer;

	/* Configure DMA channel */
	ret = no_os_dma_config_xfer(sdesc->dma_desc, ch_xfer, len, sdesc->dma_ch);
	if (ret)
		goto remove_dma;

	/* Start DMA transfer */
	ret = no_os_dma_xfer_start(sdesc->dma_desc, sdesc->dma_ch);
	if (ret)
		goto abort_transfer;

	/* Setup DMA transfer for XSPI */
	ret = stm32_xspi_setup_dma_transfer(desc, ch_xfer);
	if (ret)
		goto abort_transfer;

	return 0;

abort_transfer:
	no_os_dma_xfer_abort(sdesc->dma_desc, sdesc->dma_ch);
remove_dma:
	no_os_dma_remove(sdesc->dma_desc);
free_ch_xfer:
	no_os_free(ch_xfer);

	return ret;
}

/**
 * @brief Write/read multiple messages to/from XSPI using DMA asynchronously.
 * @param desc - The XSPI descriptor.
 * @param msgs - The messages array.
 * @param len - Number of messages.
 * @param callback - A function which will be called after all the transfers are done.
 * @param ctx - User specific data which should be passed to the callback function.
 * @return 0 in case of success, negative code otherwise.
 */
static int32_t stm32_xspi_dma_transfer_async(struct no_os_spi_desc *desc,
		struct no_os_spi_msg *msgs,
		uint32_t len,
		void (*callback)(void *),
		void *ctx)
{
	struct stm32_xspi_desc *sdesc;

	if(!desc)
		return -EINVAL;

	sdesc = desc->extra;
	sdesc->stm32_xspi_dma_user_cb = callback;
	sdesc->stm32_xspi_dma_user_ctx = ctx;
	return stm32_xspi_config_dma_and_start(desc, msgs, len, stm32_xspi_dma_callback,
					       desc);
}

/**
 * @brief Write/read multiple messages to/from XSPI using DMA. Wait until all
 *        transfers are done.
 * @param desc - The XSPI descriptor.
 * @param msgs - The messages array.
 * @param len - Number of messages.
 * @return 0 in case of success, negative code otherwise.
 */
static int32_t stm32_xspi_dma_transfer_sync(struct no_os_spi_desc *desc,
		struct no_os_spi_msg *msgs,
		uint32_t len)
{
	int32_t ret;
	uint32_t timeout;
	struct stm32_xspi_desc *sdesc;

	if(!desc)
		return -EINVAL;

	sdesc = desc->extra;
	sdesc->stm32_xspi_dma_done = false;
	ret = stm32_xspi_config_dma_and_start(desc, msgs, len, stm32_xspi_dma_callback,
					      desc);
	if (ret)
		return ret;

	timeout = msgs->bytes_number;
	while(timeout--) {
		no_os_mdelay(1);
		if (sdesc->stm32_xspi_dma_done)
			break;
	};

	/* need some cleanup here? */
	if (timeout == 0) {
		no_os_dma_xfer_abort(sdesc->dma_desc, sdesc->dma_ch);
		return -ETIME;
	}

	return 0;
}

/**
 * @brief stm32 platform specific XSPI platform ops structure
 */
const struct no_os_spi_platform_ops stm32_xspi_ops = {
	.init = &stm32_xspi_init,
	.remove = &stm32_xspi_remove,
	.transfer = &stm32_xspi_transfer,
	.write_and_read = &stm32_xspi_write_and_read,
	.dma_transfer_sync = stm32_xspi_dma_transfer_sync,
	.dma_transfer_async = stm32_xspi_dma_transfer_async,
};
