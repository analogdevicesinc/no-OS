/***************************************************************************//**
 *   @file   AD8366.c
 *   @brief  Implementation of AD8366 Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
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
 *
********************************************************************************
 *   SVN Revision: $WCREV$
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "spi_interface.h"
#include "AD8366.h"

/******************************************************************************/
/************************ Local variables and types ***************************/
/******************************************************************************/
struct ad8366_state 
{
	uint8_t ch[2];
}ad8366_st;

/***************************************************************************//**
 * @brief Initializes the AD8366. 
 *
 * @return Negative error code or 0 in case of success.
*******************************************************************************/
int32_t ad8366_setup()
{
	int32_t ret;

	/* Sets the gain of channel A to 4.5 dB */
	ret = ad8366_out_voltage0_hardwaregain(4500);
    if(ret < 0)
        return -1;

	/* Sets the gain of channel B to 4.5 dB */
	ret = ad8366_out_voltage1_hardwaregain(4500);
    if(ret < 0)
        return -1;

	return 0;
}

/***************************************************************************//**
 * @brief Writes data nto the AD8366
 *
 * @param chAgain - gain value for channel A
 * @param chBgain - gain value for channel B
 *
 * @return Returns 0 in case of success or negative error code
*******************************************************************************/
int32_t ad8366_write(uint8_t chAgain, uint8_t chBgain)
{
	uint8_t bit				= 0;
	uint8_t reversedChAgain	= 0;
	uint8_t reversedChBgain	= 0;
	uint8_t regAddr			= 0;
	uint16_t regValue			= 0;
	
	/* Maximum value of the gain can be 0x3F. */
	chAgain = (chAgain & 0x3F);
	chBgain = (chBgain & 0x3F);
	/* AD8366 accepts the LSB first transfer format. */
	for(bit = 0; bit < 8; bit++)
	{
		reversedChAgain = (reversedChAgain << 1) + (chAgain & 1);
		reversedChBgain = (reversedChBgain << 1) + (chBgain & 1);
		chAgain >>= 1;
		chBgain >>= 1;
	}
	
	regValue = (reversedChBgain << 4) | (reversedChAgain >> 2);
	return SPI_Write(SPI_SEL_AD8366, regAddr, regValue);
}

/***************************************************************************//**
 * @brief Reads data from the AD8366
 *
 * @param channel - channel number (0 or 1)
 * @param val1000 - pointer to store the data x1000
 * @param m - the code of the data to read
 *
 * @return Returns 0 in case of success or negative error code
*******************************************************************************/
int32_t ad8366_read_raw(int32_t channel,
			        int32_t *val1000,
			        int32_t m)
{
	struct ad8366_state *st = &ad8366_st;
	int32_t ret;
	uint32_t code;

	switch (m) 
    {
	case 0:
		code = st->ch[channel];

		/* Values in dB */
		*val1000 = code * 253 + 4500;
		ret = 0;
		break;
	default:
		ret = -1;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Writes data to the AD8366
 *
 * @param channel - channel number (0 or 1)
 * @param val1000 - the value to write x1000 
 * @param m - the code of the data to write
 *
 * @return Returns 0 in case of success or negative error code
*******************************************************************************/
static int32_t ad8366_write_raw(int32_t channel,
			                int32_t val1000,
			                long mask)
{
	struct ad8366_state *st = &ad8366_st;
	uint32_t code;
	int32_t ret;

	/* Values in dB */
	if (val1000 > AD8366_MAX_GAIN || val1000 < AD8366_MIN_GAIN)
		return -1;

	code = (val1000 - 4500) / 253;

    if((val1000 - (4500 + code*253)) > ((4500 + (code+1)*253) - val1000))
    {
        code++;
    }

	switch (mask) 
    {
	case 0:
		st->ch[channel] = code;
		ret = ad8366_write(st->ch[0], st->ch[1]);
		break;
	default:
		ret = -1;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Sets the gain of channel A.
 *
 * @param gain_dB - the gain to be set in dB * 1000
 *
 * @return Returns the actual set gain * 1000 or negative error code
*******************************************************************************/
int32_t ad8366_out_voltage0_hardwaregain(int32_t gain1000_dB)
{
    int32_t ret;
    int32_t act_gain1000;

    if((gain1000_dB <= AD8366_MAX_GAIN) && (gain1000_dB >= AD8366_MIN_GAIN))
    {
        ret = ad8366_write_raw(0, gain1000_dB, 0);
        if(ret < 0)
            return -1;
    }

    ret = ad8366_read_raw(0, &act_gain1000, 0);
    if(ret < 0)
        return -1;
    
    return act_gain1000;
}

/***************************************************************************//**
 * @brief Sets the gain of channel B.
 *
 * @param gain1000_dB - the gain to be set in dB * 1000
 *
 * @return Returns the actual set gain * 1000 or negative error code
*******************************************************************************/
int32_t ad8366_out_voltage1_hardwaregain(int32_t gain1000_dB)
{
	int32_t ret;
	int32_t act_gain1000;

    if((gain1000_dB <= AD8366_MAX_GAIN) && (gain1000_dB >= AD8366_MIN_GAIN))
    {
        ret = ad8366_write_raw(1, gain1000_dB, 0);
        if(ret < 0)
            return -1;
    }

    ret = ad8366_read_raw(1, &act_gain1000, 0);
    if(ret < 0)
        return -1;
    
    return act_gain1000;
}
