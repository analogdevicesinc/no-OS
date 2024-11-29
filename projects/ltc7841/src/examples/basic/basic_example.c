/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Implementation of basic LTC7841 example
 *   @author Marvin Cabuenas (marvinneil.cabuenas@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
#include <stdio.h>
#inclue "basic_example.h"
#include "common_data.h"
#include "ltc7841_regs.h"

/*******************************************************************************
 * @brief LTC7841 basic example main execution.
 *
 * @return ret - Result of the example execution.
*******************************************************************************/

int basic_example_main(void)
{
    int ret;
	struct no_os_uart_desc *uart_desc;
	struct ltc7841_desc *ltc7841_desc;
    enum ltc7841_registers_order reg;
    
    ret = no_os_uart_init(&uart_desc, &uart_ip);
	if (ret)
    {
        goto free_ltc7841;
    }     
    no_os_uart_stdio(uart_desc);

    /* Initializing the device LTC7841 */
	ret = ltc7841_init(&ltc7841_desc, &ltc7841_ip);
    if (ret)
    {
        goto free_ltc7841;
    }
    while (1)
    {
        ret = ltc7841_mfr_clear_peaks(ltc7841_desc);
        if (ret)
        {
            goto free_ltc7841;
        }
        ret = ltc7841_clear_status_word_bits(ltc7841_desc);
        if (ret)
        {
            goto free_ltc7841;
        }
        /* read every readable register for checking*/
        for (reg = LTC7841_OPERATION_ORDER; LTC7841_MFR_RESET_ORDER >= reg; reg++)
        {
            if ((LTC7841_MFR_CLEAR_PEAKS_ORDER != reg) && (LTC7841_MFR_RESET_ORDER != reg)) // write only registers
            {
                ret = ltc7841_reg_read(device, &ltc7841_regs[reg]);
                if (ret)
                {
                    goto free_ltc7841;
                }
            }
			no_os_mdelay(500);
        }
    }

free_ltc7841:
	pr_err("Error code: %d.\n", ret);
	ltc7841_remove(device);
    return 0;
}