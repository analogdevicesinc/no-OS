/**************************************************************************//**
*   @file   ADF4157.c
*   @brief  Implementation of ad9833 Driver for Microblaze processor.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*******************************************************************************
* Copyright 2013(c) Analog Devices, Inc.
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
*
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "ADF4157.h"
#include "ADF4157_cfg.h"
#include "Console.h"

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
ADF4157_state adf4157_st;


/***************************************************************************//**
 * @brief Initialize the SPI communication with the device.
 *
 * @param None.
 *
 * @return status - Result of the initialization procedure.
 *                  Example:  0 - if initialization was successful;
 *                           -1 - if initialization was unsuccessful.
*******************************************************************************/
char ADF4157_Init()
{
    unsigned long   cfgValue = 0;
    char            status   = -1;

    /* Setup Control GPIO Pins */
    ADF4157_LE_OUT;
    ADF4157_LE_LOW;
    ADF4157_CE_OUT;
    ADF4157_CE_HIGH;
    //ADF4157_LE2_OUT;
    //ADF4157_CE2_OUT;

    /* Setup SPI Interface */
    status = SPI_Init(0, 1000000, 0, 1);

    /* R4 */
    cfgValue = adf4157_pdata_lpc.r4_user_settings |
               ADF4157_R4_CTRL;
    ADF4157_Set(cfgValue);
    adf4157_st.reg_val[ADF4157_REG4] = cfgValue;

    /* R3 */
    cfgValue = adf4157_pdata_lpc.r3_user_settings |
               ADF4157_R3_CTRL;
    ADF4157_Set(cfgValue);
    adf4157_st.reg_val[ADF4157_REG3] = cfgValue;

    /* R2 */
    cfgValue = adf4157_pdata_lpc.r2_user_settings |
               ADF4157_PRESCALER(0)               |
               (adf4157_pdata_lpc.ref_div2_en ? ADF4157_RDIV2(1) : ADF4157_RDIV2(0))        |
               (adf4157_pdata_lpc.ref_doubler_en ? ADF4157_REF_DBL(1) : ADF4157_REF_DBL(0)) |
               ADF4157_R_CNT(1)                   |
               ADF4157_R2_CTRL;
    ADF4157_Set(cfgValue);
    adf4157_st.reg_val[ADF4157_REG2] = cfgValue;

    /* R1 */
    cfgValue = ADF4157_FRAC_VAL_LSB(1)            |
               ADF4157_R1_CTRL;
    ADF4157_Set(cfgValue);
    adf4157_st.reg_val[ADF4157_REG1] = cfgValue;

    /* R0 */
    cfgValue = adf4157_pdata_lpc.r0_user_settings |
               ADF4157_INT_VAL(1)                 |
               ADF4157_FRAC_VAL_MSB(1)            |
               ADF4157_R0_CTRL;
    ADF4157_Set(cfgValue);
    adf4157_st.reg_val[ADF4157_REG0] = cfgValue;

    return status;
}

/**************************************************************************//**
 * @brief Transmits 32 bits on SPI.
 *
 * @param value - Data which will be transmitted.
 *
 * @return none.
******************************************************************************/
char ADF4157_Set(unsigned long value)
{
    char          validation  = 0;
    char          status      = 0;
    unsigned char txBuffer[4] = {0, 0, 0, 0}; // TX SPI Buffer

    txBuffer[0] = (unsigned char)((value & 0xFF000000) >> 24);
    txBuffer[1] = (unsigned char)((value & 0x00FF0000) >> 16);
    txBuffer[2] = (unsigned char)((value & 0x0000FF00) >> 8);
    txBuffer[3] = (unsigned char)((value & 0x000000FF) >> 0);

    ADF4157_LE_LOW;
    validation = SPI_Write(ADF4157_SLAVE_ID, txBuffer, 4);
    if (validation != 4)
    {
        status = -1;
    }
    ADF4157_LE_HIGH;

    return status;
}

/***************************************************************************//**
 * @brief Increases the R counter value until the PFD frequency is
 *        smaller than ADF4351_MAX_FREQ_PFD.
 *
 * @param st    - The selected structure.
 * @param r_cnt - Initial r_cnt value.
 *
 * @return Final r_cnt value.
*******************************************************************************/
long ADF4157_tune_r_cnt(ADF4157_state *st, long r_cnt)
{
    ADF4157_platform_data *pdata = st->pdata;

    do {
            r_cnt++;
            st->fpfd = (pdata->clkin * (pdata->ref_doubler_en ? 2 : 1)) /
                       (r_cnt * (pdata->ref_div2_en ? 2 : 1));
    } while (st->fpfd > ADF4157_MAX_FREQ_PFD);

    return r_cnt;
}

/***************************************************************************//**
 * @brief Computes the greatest common divider of two numbers.
 *
 * @return Returns the gcd.
*******************************************************************************/
unsigned long gcd(unsigned long x, unsigned long y)
{
        long tmp;

        tmp = y > x ? x : y;


        while((x % tmp) || (y % tmp))
        {
                tmp--;
        }

        return tmp;
}

/***************************************************************************//**
 * @brief Sets the ADF4157 output frequency.
 *
 * @param st      - The selected structure.
 * @param freq    - The desired frequency value.
 *
 * @return calculatedFrequency - The actual frequency value that was set.
*******************************************************************************/
double adf4157_set_freq(ADF4157_state *st, double freq)
{
        unsigned long long  tmp;
        unsigned long       div_gcd, prescaler;
        float               chspc;
        unsigned short      mdiv;
        unsigned short      r_cnt = 0;
        double              result;

        if ((freq > ADF4157_MAX_OUT_FREQ) || (freq < ADF4157_MIN_OUT_FREQ))
                return -1;

        if (freq > ADF4157_MAX_FREQ_45_PRESC) {
                prescaler = ADF4157_PRESCALER(1);
                mdiv = 75;
        } else {
                prescaler = ADF4157_PRESCALER(0);
                mdiv = 23;
        }

        freq *= 1000000;
        st->r2_mod = ADF4157_FIXED_MODULUS;
        if ((st->pdata->clkin > ADF4157_MAX_FREQ_REFIN) ||
        (st->pdata->clkin < ADF4157_MIN_FREQ_REFIN))
        {
            return -1;
        }
        do{
                do{
                        r_cnt = ADF4157_tune_r_cnt(st, r_cnt);
                        chspc = (float)(st->fpfd) / st->r2_mod;
                   }while (r_cnt == 0);
             st->r_cnt = r_cnt;
             st->channel_spacing = chspc;

             tmp = freq * (unsigned long long)st->r2_mod + (st->fpfd >> 1);
             tmp = (unsigned long long)(tmp / st->fpfd);
             st->r0_fract = tmp % st->r2_mod;
             tmp = tmp / st->r2_mod;
             st->r0_int = (unsigned long)tmp;
        }while (mdiv > st->r0_int);

        /* register R3 */
        st->reg_val[ADF4157_REG3] &= ~(ADF4157_CNT_RST(-1));
        st->reg_val[ADF4157_REG3] |= (ADF4157_CNT_RST(1));
        ADF4157_Set(st->reg_val[ADF4157_REG3]);

        /* register R2 */
        st->reg_val[ADF4157_REG2] &= ~(ADF4157_R_CNT(-1)    |
                                       ADF4157_PRESCALER(-1));
        st->reg_val[ADF4157_REG2] |= (ADF4157_R_CNT(st->r_cnt)     |
                                      prescaler);
        ADF4157_Set(st->reg_val[ADF4157_REG2]);
        /* register R1 */
        st->reg_val[ADF4157_REG1] &= ~ADF4157_FRAC_VAL_LSB(-1);
        st->reg_val[ADF4157_REG1] |= ADF4157_FRAC_VAL_LSB(st->r0_fract);
        ADF4157_Set(st->reg_val[ADF4157_REG1]);
        /* register R0 */
        st->reg_val[ADF4157_REG0] &= ~(ADF4157_INT_VAL(-1) |
                                       ADF4157_FRAC_VAL_MSB(-1));
        st->reg_val[ADF4157_REG0] |= (ADF4157_INT_VAL(st->r0_int) |
                                      ADF4157_FRAC_VAL_MSB(st->r0_fract));
        ADF4157_Set(st->reg_val[ADF4157_REG0]);

        /* register R3 */
        st->reg_val[ADF4157_REG3] &= ~(ADF4157_CNT_RST(-1));
        ADF4157_Set(st->reg_val[ADF4157_REG3]);

        result = st->r0_int * (float)(st->fpfd / 1000000);
        result = result + ((float)st->r0_fract / st->r2_mod) * (st->fpfd / 1000000);

        return result;
}
