/***************************************************************************//**
*   @file   mdio_spi.c
*   @brief  Source file for MDIO implementation.
*   @author Johnson Ralph Perez (Johnsonralph.Perez@analog.com)
********************************************************************************
* Copyright 2026(c) Analog Devices, Inc.
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

#include <errno.h>
#include "no_os_alloc.h"
#include "mdio_spi.h"

/**
 * @struct mdio_spi_extra
 * @brief Extra data specific to the MDIO SPI implementation.
 */
struct mdio_spi_extra {
	/** SPI descriptor for the MDIO bus */
	struct no_os_spi_desc *mdio;
};

/**
 * @brief Initialize the MDIO SPI Interface
 * @param dev - The MDIO device descriptor
 * @param ip - Initialization parameter containing information about the
 * 		 MDIO SPI interface to be initialized.
 * @return 0 in case of success, negative code otherwise
 */
int mdio_spi_init(struct no_os_mdio_desc **dev,
		  struct no_os_mdio_init_param *ip)
{
	int ret;
	struct mdio_spi_init_param *msip;
	struct mdio_spi_extra *mse;
	struct no_os_mdio_desc *d;

	if (!dev || !ip || !ip->extra)
		return -EINVAL;

	msip = ip->extra;

	mse = no_os_calloc(1, sizeof(*mse));
	if (!mse)
		return -ENOMEM;

	d = no_os_calloc(1, sizeof(*d));
	if (!d) {
		ret = -ENOMEM;
		goto error_mdio_spi_calloc;
	}

	ret = no_os_spi_init(&mse->mdio, &msip->mdio);
	if (ret)
		goto error_mdio_calloc;

	d->extra = mse;
	*dev = d;

	return 0;

error_mdio_calloc:
	no_os_free(d);
error_mdio_spi_calloc:
	no_os_free(mse);

	return ret;
}

/**
 * @brief Perform MDIO Read/Write Transaction over SPI
 * @param dev - The MDIO device descriptor
 * @param reg - Register address
 * @param op - MDIO operation (address, write, or read)
 * @param data - Pointer to data to write or read into
 * @param c45 - True if using Clause 45 frame format
 * @return 0 in case of success, negative code otherwise
 */
static int mdio_spi_read_write(struct no_os_mdio_desc *dev, uint32_t reg,
			       uint16_t op, uint16_t *data, bool c45)
{
	int ret;
	uint8_t buffer[MDIO_SPI_BUFF_SIZE_UINT8] = {0};
	struct mdio_spi_extra *mse = dev->extra;
	uint8_t start = c45 ? NO_OS_MDIO_C45_START : NO_OS_MDIO_C22_START;
	uint8_t regaddr = c45 ? no_os_field_get(NO_OS_MDIO_C45_DEVADDR_MASK, reg) : reg;
	uint16_t data_val;
	uint16_t data_field;
	uint32_t preamble = MDIO_SPI_PREAMBLE;
	uint32_t tx_frame;
	uint32_t rx_frame = 0;

	if (op == NO_OS_MDIO_OP_WRITE && !data)
		return -EINVAL;

	data_val = (op == NO_OS_MDIO_OP_WRITE) ? *data :
		   MDIO_SPI_DATA_FIELD_READ;
	data_field = (op == NO_OS_MDIO_OP_ADDRESS) ? (uint16_t) reg : data_val;
	tx_frame = no_os_field_prep(NO_OS_MDIO_START_MASK, start) |
		   no_os_field_prep(NO_OS_MDIO_OP_MASK, op) |
		   no_os_field_prep(NO_OS_MDIO_PHYADDR_MASK, dev->addr) |
		   no_os_field_prep(NO_OS_MDIO_REGADDR_MASK, regaddr) |
		   no_os_field_prep(NO_OS_MDIO_TURNAROUND_MASK,
				    NO_OS_MDIO_TURNAROUND) |
		   no_os_field_prep(NO_OS_MDIO_DATA_MASK, data_field);

	/* Setup preamble to buffer */
	no_os_put_unaligned_be32(preamble, buffer);

	/* Setup TX frame to buffer */
	no_os_put_unaligned_be32(tx_frame, &buffer[MDIO_SPI_BUFF_SIZE_HALF_UINT8]);

	/* Perform MDIO write and read */
	ret = no_os_spi_write_and_read(mse->mdio, buffer, MDIO_SPI_BUFF_SIZE_UINT8);
	if (ret)
		return ret;

	/* Rearrange buffer content to RX frame */
	rx_frame = no_os_get_unaligned_be32(&buffer[MDIO_SPI_BUFF_SIZE_HALF_UINT8]);

	if (no_os_field_get(NO_OS_MDIO_TURNAROUND_MASK,
			    rx_frame) != NO_OS_MDIO_TURNAROUND)
		return -EBADMSG;

	if (no_os_field_get(NO_OS_MDIO_PHYADDR_MASK,
			    rx_frame) != dev->addr)
		return -EBADMSG;

	if (data)
		*data = no_os_field_get(NO_OS_MDIO_DATA_MASK, rx_frame);

	return 0;
}

/**
 * @brief MDIO Write over SPI with Clause 22 or Clause 45
 * @param dev - The MDIO device descriptor
 * @param reg - Register address
 * @param in - Value to write to the register
 * @return 0 in case of success, negative code otherwise
 */
static int mdio_spi_write(struct no_os_mdio_desc *dev, uint32_t reg,
			  uint16_t in)
{
	int ret;
	bool c45 = dev->c45 && reg >= NO_OS_MDIO_C22_REGS;

	if (c45) {
		ret = mdio_spi_read_write(dev, reg, NO_OS_MDIO_OP_ADDRESS,
					  NULL, c45);
		if (ret)
			return ret;
	}

	return mdio_spi_read_write(dev, reg, NO_OS_MDIO_OP_WRITE, &in, c45);
}

/**
 * @brief MDIO Read over SPI with Clause 22 or Clause 45
 * @param dev - The MDIO device descriptor
 * @param reg - Register address
 * @param out - Pointer to store the read value
 * @return 0 in case of success, negative code otherwise
 */
static int mdio_spi_read(struct no_os_mdio_desc *dev, uint32_t reg,
			 uint16_t *out)
{
	int ret;
	bool c45;

	if (!out)
		return -EINVAL;

	c45 = dev->c45 && reg >= NO_OS_MDIO_C22_REGS;

	if (c45) {
		ret = mdio_spi_read_write(dev, reg, NO_OS_MDIO_OP_ADDRESS,
					  NULL, c45);
		if (ret)
			return ret;
	}

	return mdio_spi_read_write(dev, reg, NO_OS_MDIO_OP_READ, out, c45);
}

/**
 * @brief Remove MDIO SPI Interface
 * @param dev - The MDIO device descriptor
 * @return 0 in case of success, negative code otherwise
 */
static int mdio_spi_remove(struct no_os_mdio_desc *dev)
{
	struct mdio_spi_extra *mse;

	if (!dev)
		return -EINVAL;

	mse = dev->extra;

	no_os_spi_remove(mse->mdio);
	no_os_free(mse);
	no_os_free(dev);

	return 0;
}

struct no_os_mdio_ops mdio_spi_ops = {
	.init = mdio_spi_init,
	.write = mdio_spi_write,
	.read = mdio_spi_read,
	.remove = mdio_spi_remove,
};
