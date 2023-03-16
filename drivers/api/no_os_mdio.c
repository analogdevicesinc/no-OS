/***************************************************************************//**
 *   @file   no_os_mdio.c
 *   @brief  Source file for MDIO interface driver.
 *   @author Darius Berghe (darius.berghe@analog.com)
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
#include <errno.h>
#include "no_os_mdio.h"

/**
 * @brief Initialize the MDIO interface.
 * @param desc - The MDIO descriptor.
 * @param param - MDIO parameters.
 * @return 0 in case of success, error code otherwise.
 */
int no_os_mdio_init(struct no_os_mdio_desc **desc,
		    struct no_os_mdio_init_param *param)
{
	int ret;

	if (!param || !param->ops)
		return -EINVAL;

	if (!param->ops->init)
		return -ENOSYS;

	ret = param->ops->init(desc, param);
	if (ret)
		return ret;

	(*desc)->id = param->id;
	(*desc)->c45 = param->c45;
	(*desc)->addr = param->addr;
	(*desc)->ops = param->ops;

	return 0;
}

/**
 * @brief Free the resources allocated by no_os_mdio_init().
 * @param desc - The MDIO descriptor.
 * @return 0 in case of success, error code otherwise.
 */
int no_os_mdio_remove(struct no_os_mdio_desc *desc)
{
	if (!desc || !desc->ops)
		return -EINVAL;

	if (!desc->ops->remove)
		return -ENOSYS;

	return desc->ops->remove(desc);
}

/**
 * @brief Write a register using MDIO.
 * @param desc - The MDIO descriptor.
 * @param reg - Register address.
 * 		For clause 45 reg is constructed using NO_OS_MDIO_C45_ADDR macro.
 * 		It is otherwise the plain clause 22 address (0...31).
 * @param val - Value to write into register.
 * @return 0 in case of success, error code otherwise.
 */
int no_os_mdio_write(struct no_os_mdio_desc *desc, uint32_t reg, uint16_t val)
{
	if (!desc || !desc->ops)
		return -EINVAL;

	if (!desc->ops->write)
		return -ENOSYS;

	return desc->ops->write(desc, reg, val);
}

/**
 * @brief Read a register using MDIO.
 * @param desc - The MDIO descriptor.
 * @param reg - Register address.
 * 		For clause 45 reg is constructed using NO_OS_MDIO_C45_ADDR macro.
 * 		It is otherwise the plain clause 22 address (0...31).
 * @param val - Value read from register.
 * @return 0 in case of success, error code otherwise.
 */
int no_os_mdio_read(struct no_os_mdio_desc *desc, uint32_t reg, uint16_t *val)
{
	if (!desc || !desc->ops)
		return -EINVAL;

	if (!desc->ops->read)
		return -ENOSYS;

	return desc->ops->read(desc, reg, val);
}
