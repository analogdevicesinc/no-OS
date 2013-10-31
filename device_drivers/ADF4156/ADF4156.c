/**************************************************************************//**
*   @file   adf4156.c
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
#include "ADF4156.h"
#include "ADF4156_cfg.h"
#include "Console.h"

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
adf4156_state adf4156_st;


/***************************************************************************//**
 * @brief Initialize the SPI communication with the device.
 *
 * @param None.
 *
 * @return status - Result of the initialization procedure.
 *                  Example:  0 - if initialization was successful;
 *                           -1 - if initialization was unsuccessful.
*******************************************************************************/
char ADF4156_Init(void)
{
    unsigned long   cfgValue = 0;
    char            status   = -1;

    /* Setup Control GPIO Pins */
    ADF4156_LE_OUT;
    ADF4156_LE_LOW;
    ADF4156_CE_OUT;
    ADF4156_CE_HIGH;
    //ADF4156_LE2_OUT;
    //ADF4156_CE2_OUT;

    /* Setup SPI Interface */
    status = SPI_Init(0, 1000000, 0, 1);

    /* R4 */
    cfgValue = adf4156_pdata_lpc.r4_user_settings |
               ADF4156_R4_CTRL;
    ADF4156_Set(cfgValue);
    adf4156_st.reg_val[ADF4156_REG4] = cfgValue;

    /* R3 */
    cfgValue = adf4156_pdata_lpc.r3_user_settings |
               ADF4156_R3_CTRL;
    ADF4156_Set(cfgValue);
    adf4156_st.reg_val[ADF4156_REG3] = cfgValue;

    /* R2 */
    cfgValue = adf4156_pdata_lpc.r2_user_settings |
               ADF4156_PRESCALER(0)               |
               (adf4156_pdata_lpc.ref_div2_en ? ADF4156_RDIV2(1) : ADF4156_RDIV2(0))        |
               (adf4156_pdata_lpc.ref_doubler_en ? ADF4156_REF_DBL(1) : ADF4156_REF_DBL(0)) |
               ADF4156_R_CNT(1)                   |
               ADF4156_MOD_WORD(0)                |
               ADF4156_R2_CTRL;
    ADF4156_Set(cfgValue);
    adf4156_st.reg_val[ADF4156_REG2] = cfgValue;

    /* R1 */
    cfgValue = ADF4156_PHASE_VAL(1)     |
               ADF4156_R1_CTRL;
    ADF4156_Set(cfgValue);
    adf4156_st.reg_val[ADF4156_REG1] = cfgValue;

    /* R0 */
    cfgValue = adf4156_pdata_lpc.r0_user_settings |
               ADF4156_INT_VAL(1)                 |
               ADF4156_FRAC_VAL(1)                |
               ADF4156_R0_CTRL;
    ADF4156_Set(cfgValue);
    adf4156_st.reg_val[ADF4156_REG0] = cfgValue;

    return status;
}

/**************************************************************************//**
 * @brief Transmits 32 bits on SPI.
 *
 * @param value - Data which will be transmitted.
 *
 * @return none.
******************************************************************************/
char ADF4156_Set(unsigned long value)
{
    char          validation  = 0;
    char          status      = 0;
    unsigned char txBuffer[4] = {0, 0, 0, 0}; // TX SPI Buffer

    txBuffer[0] = (unsigned char)((value & 0xFF000000) >> 24);
    txBuffer[1] = (unsigned char)((value & 0x00FF0000) >> 16);
    txBuffer[2] = (unsigned char)((value & 0x0000FF00) >> 8);
    txBuffer[3] = (unsigned char)((value & 0x000000FF) >> 0);

    ADF4156_LE_LOW;
    validation = SPI_Write(ADF4156_SLAVE_ID, txBuffer, 4);
    if (validation != 4)
    {
        status = -1;
    }
    ADF4156_LE_HIGH;

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
long adf4156_tune_r_cnt(adf4156_state *st, long r_cnt)
{
    adf4156_platform_data *pdata = st->pdata;

    do {
            r_cnt++;
            st->fpfd = (pdata->clkin * (pdata->ref_doubler_en ? 2 : 1)) /
                       (r_cnt * (pdata->ref_div2_en ? 2 : 1));
    } while (st->fpfd > ADF4156_MAX_FREQ_PFD);

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
 * @brief Sets the ADF4156 output frequency.
 *
 * @param st      - The selected structure.
 * @param freq    - The desired frequency value.
 *
 * @return calculatedFrequency - The actual frequency value that was set.
*******************************************************************************/
double adf4156_set_freq(adf4156_state *st,double freq)
{
        unsigned long long  tmp;
        unsigned long       div_gcd, prescaler, chspc;
        unsigned short      mdiv;
        unsigned short      r_cnt = 0;
        double              result;


        if ((freq > ADF4156_MAX_OUT_FREQ) || (freq < ADF4156_MIN_OUT_FREQ))
        {
            return -1;
        }
        if (freq > ADF4156_MAX_FREQ_45_PRESC) {
                prescaler = ADF4156_PRESCALER(1);
                mdiv = 75;
        } else {
                prescaler = ADF4156_PRESCALER(0);
                mdiv = 23;
        }

        freq *= 1000000;
        chspc = st->pdata->channel_spacing;

        if ((st->pdata->clkin > ADF4156_MAX_FREQ_REFIN) ||
            (st->pdata->clkin < ADF4156_MIN_FREQ_REFIN))
        {
            return -1;
        }

        do{
                do{
                        r_cnt = adf4156_tune_r_cnt(st, r_cnt);
                        st->r2_mod = st->fpfd / chspc;
                        if (r_cnt > ADF4156_MAX_R_CNT)
                        {
                            CONSOLE_Print("Error! R-counter has a too high \
value for the selected spacing. Try with a higher spacing value.\r\n");
                            return -1;
                        }
                   }while ((st->r2_mod > ADF4156_MAX_MODULUS) && r_cnt);
             st->r_cnt = r_cnt;
             tmp = freq * (unsigned long long)st->r2_mod + (st->fpfd >> 1);
             tmp = (unsigned long long)(tmp / st->fpfd);
             st->r0_fract = tmp % st->r2_mod;
             tmp = tmp / st->r2_mod;
             st->r0_int = (unsigned long)tmp;
        }while (mdiv > st->r0_int);

        if (st->r0_fract && st->r2_mod)
        {
            div_gcd = gcd(st->r2_mod, st->r0_fract);
            st->r2_mod /= div_gcd;
            st->r0_fract /= div_gcd;
        }else
        {
            st->r0_fract = 0;
            st->r2_mod = 1;
        }

        /* register R3 */
        st->reg_val[ADF4156_REG3] &= ~(ADF4156_CNT_RST(-1));
        st->reg_val[ADF4156_REG3] |= (ADF4156_CNT_RST(1));

        ADF4156_Set(st->reg_val[ADF4156_REG3]);

        /* register R2 */
        st->reg_val[ADF4156_REG2] &= ~(ADF4156_MOD_WORD(-1) |
                                       ADF4156_R_CNT(-1)    |
                                       ADF4156_PRESCALER(-1));
        st->reg_val[ADF4156_REG2] |= (ADF4156_MOD_WORD(st->r2_mod) |
                                      ADF4156_R_CNT(st->r_cnt)     |
                                      prescaler);
        ADF4156_Set(st->reg_val[ADF4156_REG2]);

        /* register R0 */
        st->reg_val[ADF4156_REG0] &= ~(ADF4156_INT_VAL(-1) |
                                       ADF4156_FRAC_VAL(-1));
        st->reg_val[ADF4156_REG0] |= (ADF4156_INT_VAL(st->r0_int) |
                                      ADF4156_FRAC_VAL(st->r0_fract));
        ADF4156_Set(st->reg_val[ADF4156_REG0]);

        /* register R3 */
        st->reg_val[ADF4156_REG3] &= ~(ADF4156_CNT_RST(-1));
        ADF4156_Set(st->reg_val[ADF4156_REG3]);

        result = st->r0_int * (float)(st->fpfd / 1000000);
        result = result + ((float)st->r0_fract / st->r2_mod) * (st->fpfd / 1000000);

        return result;
}
