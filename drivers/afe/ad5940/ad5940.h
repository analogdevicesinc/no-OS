/***************************************************************************//**
 *   @file   ad5940.h
 *   @brief  Header file of ad5940 driver.
 *   @author Kister Jimenez (kister.jimenez@analog.com)
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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
#ifndef _AD5940_H_
#define _AD5940_H_
#include <stdint.h>
#include "no_os_spi.h"
#include "no_os_gpio.h"

/** @addtogroup AD5940_Library
  * @{
  */

/**
 * @defgroup AD5940RegistersBitfields
 * @brief All AD5940 registers and bitfields definition.
 * @{
*/
#ifndef __ADI_GENERATED_DEF_HEADERS__
#define __ADI_GENERATED_DEF_HEADERS__    1
#endif

#define __ADI_HAS_AGPIO__          1
#define __ADI_HAS_ALLON__          1
#define __ADI_HAS_INTC__           1
#define __ADI_HAS_AFECON__         1
#define __ADI_HAS_SPII2CS__        1
#define __ADI_HAS_WUPTMR__         1
#define __ADI_HAS_AFE__            1

/* ============================================================================================================================
        GPIO
   ============================================================================================================================ */

/* ============================================================================================================================
        AGPIO
   ============================================================================================================================ */
#define REG_AGPIO_GP0CON_RESET               0x00000000            /*      Reset Value for GP0CON  */
#define REG_AGPIO_GP0CON                     0x00000000            /*  AGPIO GPIO Port 0 Configuration */
#define REG_AGPIO_GP0OEN_RESET               0x00000000            /*      Reset Value for GP0OEN  */
#define REG_AGPIO_GP0OEN                     0x00000004            /*  AGPIO GPIO Port 0 Output Enable */
#define REG_AGPIO_GP0PE_RESET                0x00000000            /*      Reset Value for GP0PE  */
#define REG_AGPIO_GP0PE                      0x00000008            /*  AGPIO GPIO Port 0 Pullup/Pulldown Enable */
#define REG_AGPIO_GP0IEN_RESET               0x00000000            /*      Reset Value for GP0IEN  */
#define REG_AGPIO_GP0IEN                     0x0000000C            /*  AGPIO GPIO Port 0 Input Path Enable */
#define REG_AGPIO_GP0IN_RESET                0x00000000            /*      Reset Value for GP0IN  */
#define REG_AGPIO_GP0IN                      0x00000010            /*  AGPIO GPIO Port 0 Registered Data Input */
#define REG_AGPIO_GP0OUT_RESET               0x00000000            /*      Reset Value for GP0OUT  */
#define REG_AGPIO_GP0OUT                     0x00000014            /*  AGPIO GPIO Port 0 Data Output */
#define REG_AGPIO_GP0SET_RESET               0x00000000            /*      Reset Value for GP0SET  */
#define REG_AGPIO_GP0SET                     0x00000018            /*  AGPIO GPIO Port 0 Data Out Set */
#define REG_AGPIO_GP0CLR_RESET               0x00000000            /*      Reset Value for GP0CLR  */
#define REG_AGPIO_GP0CLR                     0x0000001C            /*  AGPIO GPIO Port 0 Data Out Clear */
#define REG_AGPIO_GP0TGL_RESET               0x00000000            /*      Reset Value for GP0TGL  */
#define REG_AGPIO_GP0TGL                     0x00000020            /*  AGPIO GPIO Port 0 Pin Toggle */

/* ============================================================================================================================
        AGPIO Register BitMasks, Positions & Enumerations
   ============================================================================================================================ */
/* -------------------------------------------------------------------------------------------------------------------------
          AGPIO_GP0CON                         Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AGPIO_GP0CON_PIN7CFG            14            /*  P0.7 Configuration Bits */
#define BITP_AGPIO_GP0CON_PIN6CFG            12            /*  P0.6 Configuration Bits */
#define BITP_AGPIO_GP0CON_PIN5CFG            10            /*  P0.5 Configuration Bits */
#define BITP_AGPIO_GP0CON_PIN4CFG             8            /*  P0.4 Configuration Bits */
#define BITP_AGPIO_GP0CON_PIN3CFG             6            /*  P0.3 Configuration Bits */
#define BITP_AGPIO_GP0CON_PIN2CFG             4            /*  P0.2 Configuration Bits */
#define BITP_AGPIO_GP0CON_PIN1CFG             2            /*  P0.1 Configuration Bits */
#define BITP_AGPIO_GP0CON_PIN0CFG             0            /*  P0.0 Configuration Bits */
#define BITM_AGPIO_GP0CON_PIN7CFG            0x0000C000    /*  P0.7 Configuration Bits */
#define BITM_AGPIO_GP0CON_PIN6CFG            0x00003000    /*  P0.6 Configuration Bits */
#define BITM_AGPIO_GP0CON_PIN5CFG            0x00000C00    /*  P0.5 Configuration Bits */
#define BITM_AGPIO_GP0CON_PIN4CFG            0x00000300    /*  P0.4 Configuration Bits */
#define BITM_AGPIO_GP0CON_PIN3CFG            0x000000C0    /*  P0.3 Configuration Bits */
#define BITM_AGPIO_GP0CON_PIN2CFG            0x00000030    /*  P0.2 Configuration Bits */
#define BITM_AGPIO_GP0CON_PIN1CFG            0x0000000C    /*  P0.1 Configuration Bits */
#define BITM_AGPIO_GP0CON_PIN0CFG            0x00000003    /*  P0.0 Configuration Bits */

/* -------------------------------------------------------------------------------------------------------------------------
          AGPIO_GP0OEN                         Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AGPIO_GP0OEN_OEN                 0            /*  Pin Output Drive Enable */
#define BITM_AGPIO_GP0OEN_OEN                0x000000FF    /*  Pin Output Drive Enable */

/* -------------------------------------------------------------------------------------------------------------------------
          AGPIO_GP0PE                          Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AGPIO_GP0PE_PE                   0            /*  Pin Pull Enable */
#define BITM_AGPIO_GP0PE_PE                  0x000000FF    /*  Pin Pull Enable */

/* -------------------------------------------------------------------------------------------------------------------------
          AGPIO_GP0IEN                         Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AGPIO_GP0IEN_IEN                 0            /*  Input Path Enable */
#define BITM_AGPIO_GP0IEN_IEN                0x000000FF    /*  Input Path Enable */

/* -------------------------------------------------------------------------------------------------------------------------
          AGPIO_GP0IN                          Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AGPIO_GP0IN_IN                   0            /*  Registered Data Input */
#define BITM_AGPIO_GP0IN_IN                  0x000000FF    /*  Registered Data Input */

/* -------------------------------------------------------------------------------------------------------------------------
          AGPIO_GP0OUT                         Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AGPIO_GP0OUT_OUT                 0            /*  Data Out */
#define BITM_AGPIO_GP0OUT_OUT                0x000000FF    /*  Data Out */

/* -------------------------------------------------------------------------------------------------------------------------
          AGPIO_GP0SET                         Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AGPIO_GP0SET_SET                 0            /*  Set the Output HIGH */
#define BITM_AGPIO_GP0SET_SET                0x000000FF    /*  Set the Output HIGH */

/* -------------------------------------------------------------------------------------------------------------------------
          AGPIO_GP0CLR                         Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AGPIO_GP0CLR_CLR                 0            /*  Set the Output LOW */
#define BITM_AGPIO_GP0CLR_CLR                0x000000FF    /*  Set the Output LOW */

/* -------------------------------------------------------------------------------------------------------------------------
          AGPIO_GP0TGL                         Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AGPIO_GP0TGL_TGL                 0            /*  Toggle the Output */
#define BITM_AGPIO_GP0TGL_TGL                0x000000FF    /*  Toggle the Output */


/* ============================================================================================================================

   ============================================================================================================================ */

/* ============================================================================================================================
        AFECON
   ============================================================================================================================ */
#define REG_AFECON_ADIID_RESET               0x00000000            /*      Reset Value for ADIID  */
#define REG_AFECON_ADIID                     0x00000400            /*  AFECON ADI Identification */
#define REG_AFECON_CHIPID_RESET              0x00000000            /*      Reset Value for CHIPID  */
#define REG_AFECON_CHIPID                    0x00000404            /*  AFECON Chip Identification */
#define REG_AFECON_CLKCON0_RESET             0x00000441            /*      Reset Value for CLKCON0  */
#define REG_AFECON_CLKCON0                   0x00000408            /*  AFECON Clock Divider Configuration */
#define REG_AFECON_CLKEN1_RESET              0x000002C0            /*      Reset Value for CLKEN1  */
#define REG_AFECON_CLKEN1                    0x00000410            /*  AFECON Clock Gate Enable */
#define REG_AFECON_CLKSEL_RESET              0x00000000            /*      Reset Value for CLKSEL  */
#define REG_AFECON_CLKSEL                    0x00000414            /*  AFECON Clock Select */
#define REG_AFECON_CLKCON0KEY_RESET          0x00000000            /*      Reset Value for CLKCON0KEY  */
#define REG_AFECON_CLKCON0KEY                0x00000420            /*  AFECON Enable Clock Division to 8Mhz,4Mhz and 2Mhz */
#define REG_AFECON_SWRSTCON_RESET            0x00000001            /*      Reset Value for SWRSTCON  */
#define REG_AFECON_SWRSTCON                  0x00000424            /*  AFECON Software Reset */
#define REG_AFECON_TRIGSEQ_RESET             0x00000000            /*      Reset Value for TRIGSEQ  */
#define REG_AFECON_TRIGSEQ                   0x00000430            /*  AFECON Trigger Sequence */

/* ============================================================================================================================
        AFECON Register BitMasks, Positions & Enumerations
   ============================================================================================================================ */
/* -------------------------------------------------------------------------------------------------------------------------
          AFECON_ADIID                         Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFECON_ADIID_ADIID               0            /*  ADI Identifier. */
#define BITM_AFECON_ADIID_ADIID              0x0000FFFF    /*  ADI Identifier. */

/* -------------------------------------------------------------------------------------------------------------------------
          AFECON_CHIPID                        Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFECON_CHIPID_PARTID             4            /*  Part Identifier */
#define BITP_AFECON_CHIPID_REVISION           0            /*  Silicon Revision Number */
#define BITM_AFECON_CHIPID_PARTID            0x0000FFF0    /*  Part Identifier */
#define BITM_AFECON_CHIPID_REVISION          0x0000000F    /*  Silicon Revision Number */

/* -------------------------------------------------------------------------------------------------------------------------
          AFECON_CLKCON0                       Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFECON_CLKCON0_SFFTCLKDIVCNT    10            /*  SFFT Clock Divider Configuration */
#define BITP_AFECON_CLKCON0_ADCCLKDIV         6            /*  ADC Clock Divider Configuration */
#define BITP_AFECON_CLKCON0_SYSCLKDIV         0            /*  System Clock Divider Configuration */
#define BITM_AFECON_CLKCON0_SFFTCLKDIVCNT    0x0000FC00    /*  SFFT Clock Divider Configuration */
#define BITM_AFECON_CLKCON0_ADCCLKDIV        0x000003C0    /*  ADC Clock Divider Configuration */
#define BITM_AFECON_CLKCON0_SYSCLKDIV        0x0000003F    /*  System Clock Divider Configuration */

/* -------------------------------------------------------------------------------------------------------------------------
          AFECON_CLKEN1                        Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFECON_CLKEN1_GPT1DIS            7            /*  GPT1 Clock Enable */
#define BITP_AFECON_CLKEN1_GPT0DIS            6            /*  GPT0 Clock Enable */
#define BITP_AFECON_CLKEN1_ACLKDIS            5            /*  ACLK Clock Enable */
#define BITM_AFECON_CLKEN1_GPT1DIS           0x00000080    /*  GPT1 Clock Enable */
#define BITM_AFECON_CLKEN1_GPT0DIS           0x00000040    /*  GPT0 Clock Enable */
#define BITM_AFECON_CLKEN1_ACLKDIS           0x00000020    /*  ACLK Clock Enable */

/* -------------------------------------------------------------------------------------------------------------------------
          AFECON_CLKSEL                        Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFECON_CLKSEL_ADCCLKSEL          2            /*  Select ADC Clock Source */
#define BITP_AFECON_CLKSEL_SYSCLKSEL          0            /*  Select System Clock Source */
#define BITM_AFECON_CLKSEL_ADCCLKSEL         0x0000000C    /*  Select ADC Clock Source */
#define BITM_AFECON_CLKSEL_SYSCLKSEL         0x00000003    /*  Select System Clock Source */

/* -------------------------------------------------------------------------------------------------------------------------
          AFECON_CLKCON0KEY                    Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFECON_CLKCON0KEY_DIVSYSCLK_ULP_EN  0            /*  Enable Clock Division to 8Mhz,4Mhz and 2Mhz */
#define BITM_AFECON_CLKCON0KEY_DIVSYSCLK_ULP_EN 0x0000FFFF    /*  Enable Clock Division to 8Mhz,4Mhz and 2Mhz */

/* -------------------------------------------------------------------------------------------------------------------------
          AFECON_SWRSTCON                      Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFECON_SWRSTCON_SWRSTL           0            /*  Software Reset */
#define BITM_AFECON_SWRSTCON_SWRSTL          0x0000FFFF    /*  Software Reset */

/* -------------------------------------------------------------------------------------------------------------------------
          AFECON_TRIGSEQ                       Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFECON_TRIGSEQ_TRIG3             3            /*  Trigger Sequence 3 */
#define BITP_AFECON_TRIGSEQ_TRIG2             2            /*  Trigger Sequence 2 */
#define BITP_AFECON_TRIGSEQ_TRIG1             1            /*  Trigger Sequence 1 */
#define BITP_AFECON_TRIGSEQ_TRIG0             0            /*  Trigger Sequence 0 */
#define BITM_AFECON_TRIGSEQ_TRIG3            0x00000008    /*  Trigger Sequence 3 */
#define BITM_AFECON_TRIGSEQ_TRIG2            0x00000004    /*  Trigger Sequence 2 */
#define BITM_AFECON_TRIGSEQ_TRIG1            0x00000002    /*  Trigger Sequence 1 */
#define BITM_AFECON_TRIGSEQ_TRIG0            0x00000001    /*  Trigger Sequence 0 */


/* ============================================================================================================================
        Wakeup Timer
   ============================================================================================================================ */

/* ============================================================================================================================
        WUPTMR
   ============================================================================================================================ */
#define REG_WUPTMR_CON_RESET                 0x00000000            /*      Reset Value for CON  */
#define REG_WUPTMR_CON                       0x00000800            /*  WUPTMR Timer Control */
#define REG_WUPTMR_SEQORDER_RESET            0x00000000            /*      Reset Value for SEQORDER  */
#define REG_WUPTMR_SEQORDER                  0x00000804            /*  WUPTMR Order Control */
#define REG_WUPTMR_SEQ0WUPL_RESET            0x0000FFFF            /*      Reset Value for SEQ0WUPL  */
#define REG_WUPTMR_SEQ0WUPL                  0x00000808            /*  WUPTMR SEQ0 WTimeL (LSB) */
#define REG_WUPTMR_SEQ0WUPH_RESET            0x0000000F            /*      Reset Value for SEQ0WUPH  */
#define REG_WUPTMR_SEQ0WUPH                  0x0000080C            /*  WUPTMR SEQ0 WTimeH (MSB) */
#define REG_WUPTMR_SEQ0SLEEPL_RESET          0x0000FFFF            /*      Reset Value for SEQ0SLEEPL  */
#define REG_WUPTMR_SEQ0SLEEPL                0x00000810            /*  WUPTMR SEQ0 STimeL (LSB) */
#define REG_WUPTMR_SEQ0SLEEPH_RESET          0x0000000F            /*      Reset Value for SEQ0SLEEPH  */
#define REG_WUPTMR_SEQ0SLEEPH                0x00000814            /*  WUPTMR SEQ0 STimeH (MSB) */
#define REG_WUPTMR_SEQ1WUPL_RESET            0x0000FFFF            /*      Reset Value for SEQ1WUPL  */
#define REG_WUPTMR_SEQ1WUPL                  0x00000818            /*  WUPTMR SEQ1 WTimeL (LSB) */
#define REG_WUPTMR_SEQ1WUPH_RESET            0x0000000F            /*      Reset Value for SEQ1WUPH  */
#define REG_WUPTMR_SEQ1WUPH                  0x0000081C            /*  WUPTMR SEQ1 WTimeH (MSB) */
#define REG_WUPTMR_SEQ1SLEEPL_RESET          0x0000FFFF            /*      Reset Value for SEQ1SLEEPL  */
#define REG_WUPTMR_SEQ1SLEEPL                0x00000820            /*  WUPTMR SEQ1 STimeL (LSB) */
#define REG_WUPTMR_SEQ1SLEEPH_RESET          0x0000000F            /*      Reset Value for SEQ1SLEEPH  */
#define REG_WUPTMR_SEQ1SLEEPH                0x00000824            /*  WUPTMR SEQ1 STimeH (MSB) */
#define REG_WUPTMR_SEQ2WUPL_RESET            0x0000FFFF            /*      Reset Value for SEQ2WUPL  */
#define REG_WUPTMR_SEQ2WUPL                  0x00000828            /*  WUPTMR SEQ2 WTimeL (LSB) */
#define REG_WUPTMR_SEQ2WUPH_RESET            0x0000000F            /*      Reset Value for SEQ2WUPH  */
#define REG_WUPTMR_SEQ2WUPH                  0x0000082C            /*  WUPTMR SEQ2 WTimeH (MSB) */
#define REG_WUPTMR_SEQ2SLEEPL_RESET          0x0000FFFF            /*      Reset Value for SEQ2SLEEPL  */
#define REG_WUPTMR_SEQ2SLEEPL                0x00000830            /*  WUPTMR SEQ2 STimeL (LSB) */
#define REG_WUPTMR_SEQ2SLEEPH_RESET          0x0000000F            /*      Reset Value for SEQ2SLEEPH  */
#define REG_WUPTMR_SEQ2SLEEPH                0x00000834            /*  WUPTMR SEQ2 STimeH (MSB) */
#define REG_WUPTMR_SEQ3WUPL_RESET            0x0000FFFF            /*      Reset Value for SEQ3WUPL  */
#define REG_WUPTMR_SEQ3WUPL                  0x00000838            /*  WUPTMR SEQ3 WTimeL (LSB) */
#define REG_WUPTMR_SEQ3WUPH_RESET            0x0000000F            /*      Reset Value for SEQ3WUPH  */
#define REG_WUPTMR_SEQ3WUPH                  0x0000083C            /*  WUPTMR SEQ3 WTimeH (MSB) */
#define REG_WUPTMR_SEQ3SLEEPL_RESET          0x0000FFFF            /*      Reset Value for SEQ3SLEEPL  */
#define REG_WUPTMR_SEQ3SLEEPL                0x00000840            /*  WUPTMR SEQ3 STimeL (LSB) */
#define REG_WUPTMR_SEQ3SLEEPH_RESET          0x0000000F            /*      Reset Value for SEQ3SLEEPH  */
#define REG_WUPTMR_SEQ3SLEEPH                0x00000844            /*  WUPTMR SEQ3 STimeH (MSB) */

/* ============================================================================================================================
        WUPTMR Register BitMasks, Positions & Enumerations
   ============================================================================================================================ */
/* -------------------------------------------------------------------------------------------------------------------------
          WUPTMR_CON                           Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_WUPTMR_CON_MSKTRG                6            /*  Mark Sequence Trigger from Sleep Wakeup Timer */
#define BITP_WUPTMR_CON_CLKSEL                4            /*  Clock Selection */
#define BITP_WUPTMR_CON_ENDSEQ                1            /*  End Sequence */
#define BITP_WUPTMR_CON_EN                    0            /*  Sleep Wake Timer Enable Bit */
#define BITM_WUPTMR_CON_MSKTRG               0x00000040    /*  Mark Sequence Trigger from Sleep Wakeup Timer */
#define BITM_WUPTMR_CON_CLKSEL               0x00000030    /*  Clock Selection */
#define BITM_WUPTMR_CON_ENDSEQ               0x0000000E    /*  End Sequence */
#define BITM_WUPTMR_CON_EN                   0x00000001    /*  Sleep Wake Timer Enable Bit */
#define ENUM_WUPTMR_CON_SWT32K0              0x00000000            /*  CLKSEL: Internal 32kHz OSC */
#define ENUM_WUPTMR_CON_SWTEXT0              0x00000010            /*  CLKSEL: External Clock */
#define ENUM_WUPTMR_CON_SWT32K               0x00000020            /*  CLKSEL: Internal 32kHz OSC */
#define ENUM_WUPTMR_CON_SWTEXT               0x00000030            /*  CLKSEL: External Clock */
#define ENUM_WUPTMR_CON_ENDSEQA              0x00000000            /*  ENDSEQ: The Sleep Wakeup Timer Will Stop At SeqA And Then Go Back To SeqA */
#define ENUM_WUPTMR_CON_ENDSEQB              0x00000002            /*  ENDSEQ: The Sleep Wakeup Timer Will Stop At SeqB And Then Go Back To SeqA */
#define ENUM_WUPTMR_CON_ENDSEQC              0x00000004            /*  ENDSEQ: The Sleep Wakeup Timer Will Stop At SeqC And Then Go Back To SeqA */
#define ENUM_WUPTMR_CON_ENDSEQD              0x00000006            /*  ENDSEQ: The Sleep Wakeup Timer Will Stop At SeqD And Then Go Back To SeqA */
#define ENUM_WUPTMR_CON_ENDSEQE              0x00000008            /*  ENDSEQ: The Sleep Wakeup Timer Will Stop At SeqE And Then Go Back To SeqA */
#define ENUM_WUPTMR_CON_ENDSEQF              0x0000000A            /*  ENDSEQ: The Sleep Wakeup Timer Will Stop At SeqF And Then Go Back To SeqA */
#define ENUM_WUPTMR_CON_ENDSEQG              0x0000000C            /*  ENDSEQ: The Sleep Wakeup Timer Will Stop At SeqG And Then Go Back To SeqA */
#define ENUM_WUPTMR_CON_ENDSEQH              0x0000000E            /*  ENDSEQ: The Sleep Wakeup Timer Will Stop At SeqH And Then Go Back To SeqA */
#define ENUM_WUPTMR_CON_SWTEN                0x00000000            /*  EN: Enable Sleep Wakeup Timer */
#define ENUM_WUPTMR_CON_SWTDIS               0x00000001            /*  EN: Disable Sleep Wakeup Timer */

/* -------------------------------------------------------------------------------------------------------------------------
          WUPTMR_SEQORDER                      Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_WUPTMR_SEQORDER_SEQH            14            /*  SEQH Config */
#define BITP_WUPTMR_SEQORDER_SEQG            12            /*  SEQG Config */
#define BITP_WUPTMR_SEQORDER_SEQF            10            /*  SEQF Config */
#define BITP_WUPTMR_SEQORDER_SEQE             8            /*  SEQE Config */
#define BITP_WUPTMR_SEQORDER_SEQD             6            /*  SEQD Config */
#define BITP_WUPTMR_SEQORDER_SEQC             4            /*  SEQC Config */
#define BITP_WUPTMR_SEQORDER_SEQB             2            /*  SEQB Config */
#define BITP_WUPTMR_SEQORDER_SEQA             0            /*  SEQA Config */
#define BITM_WUPTMR_SEQORDER_SEQH            0x0000C000    /*  SEQH Config */
#define BITM_WUPTMR_SEQORDER_SEQG            0x00003000    /*  SEQG Config */
#define BITM_WUPTMR_SEQORDER_SEQF            0x00000C00    /*  SEQF Config */
#define BITM_WUPTMR_SEQORDER_SEQE            0x00000300    /*  SEQE Config */
#define BITM_WUPTMR_SEQORDER_SEQD            0x000000C0    /*  SEQD Config */
#define BITM_WUPTMR_SEQORDER_SEQC            0x00000030    /*  SEQC Config */
#define BITM_WUPTMR_SEQORDER_SEQB            0x0000000C    /*  SEQB Config */
#define BITM_WUPTMR_SEQORDER_SEQA            0x00000003    /*  SEQA Config */
#define ENUM_WUPTMR_SEQORDER_SEQH0           0x00000000            /*  SEQH: Fill SEQ0 In */
#define ENUM_WUPTMR_SEQORDER_SEQH1           0x00004000            /*  SEQH: Fill SEQ1 In */
#define ENUM_WUPTMR_SEQORDER_SEQH2           0x00008000            /*  SEQH: Fill SEQ2 In */
#define ENUM_WUPTMR_SEQORDER_SEQH3           0x0000C000            /*  SEQH: Fill SEQ3 In */
#define ENUM_WUPTMR_SEQORDER_SEQG0           0x00000000            /*  SEQG: Fill SEQ0 In */
#define ENUM_WUPTMR_SEQORDER_SEQG1           0x00001000            /*  SEQG: Fill SEQ1 In */
#define ENUM_WUPTMR_SEQORDER_SEQG2           0x00002000            /*  SEQG: Fill SEQ2 In */
#define ENUM_WUPTMR_SEQORDER_SEQG3           0x00003000            /*  SEQG: Fill SEQ3 In */
#define ENUM_WUPTMR_SEQORDER_SEQF0           0x00000000            /*  SEQF: Fill SEQ0 In */
#define ENUM_WUPTMR_SEQORDER_SEQF1           0x00000400            /*  SEQF: Fill SEQ1 In */
#define ENUM_WUPTMR_SEQORDER_SEQF2           0x00000800            /*  SEQF: Fill SEQ2 In */
#define ENUM_WUPTMR_SEQORDER_SEQF3           0x00000C00            /*  SEQF: Fill SEQ3 In */
#define ENUM_WUPTMR_SEQORDER_SEQE0           0x00000000            /*  SEQE: Fill SEQ0 In */
#define ENUM_WUPTMR_SEQORDER_SEQE1           0x00000100            /*  SEQE: Fill SEQ1 In */
#define ENUM_WUPTMR_SEQORDER_SEQE2           0x00000200            /*  SEQE: Fill SEQ2 In */
#define ENUM_WUPTMR_SEQORDER_SEQE3           0x00000300            /*  SEQE: Fill SEQ3 In */
#define ENUM_WUPTMR_SEQORDER_SEQD0           0x00000000            /*  SEQD: Fill SEQ0 In */
#define ENUM_WUPTMR_SEQORDER_SEQD1           0x00000040            /*  SEQD: Fill SEQ1 In */
#define ENUM_WUPTMR_SEQORDER_SEQD2           0x00000080            /*  SEQD: Fill SEQ2 In */
#define ENUM_WUPTMR_SEQORDER_SEQD3           0x000000C0            /*  SEQD: Fill SEQ3 In */
#define ENUM_WUPTMR_SEQORDER_SEQC0           0x00000000            /*  SEQC: Fill SEQ0 In */
#define ENUM_WUPTMR_SEQORDER_SEQC1           0x00000010            /*  SEQC: Fill SEQ1 In */
#define ENUM_WUPTMR_SEQORDER_SEQC2           0x00000020            /*  SEQC: Fill SEQ2 In */
#define ENUM_WUPTMR_SEQORDER_SEQC3           0x00000030            /*  SEQC: Fill SEQ3 In */
#define ENUM_WUPTMR_SEQORDER_SEQB0           0x00000000            /*  SEQB: Fill SEQ0 In */
#define ENUM_WUPTMR_SEQORDER_SEQB1           0x00000004            /*  SEQB: Fill SEQ1 In */
#define ENUM_WUPTMR_SEQORDER_SEQB2           0x00000008            /*  SEQB: Fill SEQ2 In */
#define ENUM_WUPTMR_SEQORDER_SEQB3           0x0000000C            /*  SEQB: Fill SEQ3 In */
#define ENUM_WUPTMR_SEQORDER_SEQA0           0x00000000            /*  SEQA: Fill SEQ0 In */
#define ENUM_WUPTMR_SEQORDER_SEQA1           0x00000001            /*  SEQA: Fill SEQ1 In */
#define ENUM_WUPTMR_SEQORDER_SEQA2           0x00000002            /*  SEQA: Fill SEQ2 In */
#define ENUM_WUPTMR_SEQORDER_SEQA3           0x00000003            /*  SEQA: Fill SEQ3 In */

/* -------------------------------------------------------------------------------------------------------------------------
          WUPTMR_SEQ0WUPL                      Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_WUPTMR_SEQ0WUPL_WAKEUPTIME0      0            /*  Sequence 0 Sleep Period */
#define BITM_WUPTMR_SEQ0WUPL_WAKEUPTIME0     0x0000FFFF    /*  Sequence 0 Sleep Period */

/* -------------------------------------------------------------------------------------------------------------------------
          WUPTMR_SEQ0WUPH                      Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_WUPTMR_SEQ0WUPH_WAKEUPTIME0      0            /*  Sequence 0 Sleep Period */
#define BITM_WUPTMR_SEQ0WUPH_WAKEUPTIME0     0x0000000F    /*  Sequence 0 Sleep Period */

/* -------------------------------------------------------------------------------------------------------------------------
          WUPTMR_SEQ0SLEEPL                    Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_WUPTMR_SEQ0SLEEPL_SLEEPTIME0     0            /*  Sequence 0 Active Period */
#define BITM_WUPTMR_SEQ0SLEEPL_SLEEPTIME0    0x0000FFFF    /*  Sequence 0 Active Period */

/* -------------------------------------------------------------------------------------------------------------------------
          WUPTMR_SEQ0SLEEPH                    Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_WUPTMR_SEQ0SLEEPH_SLEEPTIME0     0            /*  Sequence 0 Active Period */
#define BITM_WUPTMR_SEQ0SLEEPH_SLEEPTIME0    0x0000000F    /*  Sequence 0 Active Period */

/* -------------------------------------------------------------------------------------------------------------------------
          WUPTMR_SEQ1WUPL                      Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_WUPTMR_SEQ1WUPL_WAKEUPTIME       0            /*  Sequence 1 Sleep Period */
#define BITM_WUPTMR_SEQ1WUPL_WAKEUPTIME      0x0000FFFF    /*  Sequence 1 Sleep Period */

/* -------------------------------------------------------------------------------------------------------------------------
          WUPTMR_SEQ1WUPH                      Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_WUPTMR_SEQ1WUPH_WAKEUPTIME       0            /*  Sequence 1 Sleep Period */
#define BITM_WUPTMR_SEQ1WUPH_WAKEUPTIME      0x0000000F    /*  Sequence 1 Sleep Period */

/* -------------------------------------------------------------------------------------------------------------------------
          WUPTMR_SEQ1SLEEPL                    Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_WUPTMR_SEQ1SLEEPL_SLEEPTIME1     0            /*  Sequence 1 Active Period */
#define BITM_WUPTMR_SEQ1SLEEPL_SLEEPTIME1    0x0000FFFF    /*  Sequence 1 Active Period */

/* -------------------------------------------------------------------------------------------------------------------------
          WUPTMR_SEQ1SLEEPH                    Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_WUPTMR_SEQ1SLEEPH_SLEEPTIME1     0            /*  Sequence 1 Active Period */
#define BITM_WUPTMR_SEQ1SLEEPH_SLEEPTIME1    0x0000000F    /*  Sequence 1 Active Period */

/* -------------------------------------------------------------------------------------------------------------------------
          WUPTMR_SEQ2WUPL                      Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_WUPTMR_SEQ2WUPL_WAKEUPTIME2      0            /*  Sequence 2 Sleep Period */
#define BITM_WUPTMR_SEQ2WUPL_WAKEUPTIME2     0x0000FFFF    /*  Sequence 2 Sleep Period */

/* -------------------------------------------------------------------------------------------------------------------------
          WUPTMR_SEQ2WUPH                      Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_WUPTMR_SEQ2WUPH_WAKEUPTIME2      0            /*  Sequence 2 Sleep Period */
#define BITM_WUPTMR_SEQ2WUPH_WAKEUPTIME2     0x0000000F    /*  Sequence 2 Sleep Period */

/* -------------------------------------------------------------------------------------------------------------------------
          WUPTMR_SEQ2SLEEPL                    Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_WUPTMR_SEQ2SLEEPL_SLEEPTIME2     0            /*  Sequence 2 Active Period */
#define BITM_WUPTMR_SEQ2SLEEPL_SLEEPTIME2    0x0000FFFF    /*  Sequence 2 Active Period */

/* -------------------------------------------------------------------------------------------------------------------------
          WUPTMR_SEQ2SLEEPH                    Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_WUPTMR_SEQ2SLEEPH_SLEEPTIME2     0            /*  Sequence 2 Active Period */
#define BITM_WUPTMR_SEQ2SLEEPH_SLEEPTIME2    0x0000000F    /*  Sequence 2 Active Period */

/* -------------------------------------------------------------------------------------------------------------------------
          WUPTMR_SEQ3WUPL                      Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_WUPTMR_SEQ3WUPL_WAKEUPTIME3      0            /*  Sequence 3 Sleep Period */
#define BITM_WUPTMR_SEQ3WUPL_WAKEUPTIME3     0x0000FFFF    /*  Sequence 3 Sleep Period */

/* -------------------------------------------------------------------------------------------------------------------------
          WUPTMR_SEQ3WUPH                      Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_WUPTMR_SEQ3WUPH_WAKEUPTIME3      0            /*  Sequence 3 Sleep Period */
#define BITM_WUPTMR_SEQ3WUPH_WAKEUPTIME3     0x0000000F    /*  Sequence 3 Sleep Period */

/* -------------------------------------------------------------------------------------------------------------------------
          WUPTMR_SEQ3SLEEPL                    Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_WUPTMR_SEQ3SLEEPL_SLEEPTIME3     0            /*  Sequence 3 Active Period */
#define BITM_WUPTMR_SEQ3SLEEPL_SLEEPTIME3    0x0000FFFF    /*  Sequence 3 Active Period */

/* -------------------------------------------------------------------------------------------------------------------------
          WUPTMR_SEQ3SLEEPH                    Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_WUPTMR_SEQ3SLEEPH_SLEEPTIME3     0            /*  Sequence 3 Active Period */
#define BITM_WUPTMR_SEQ3SLEEPH_SLEEPTIME3    0x0000000F    /*  Sequence 3 Active Period */


/* ============================================================================================================================
        Always On Register
   ============================================================================================================================ */

/* ============================================================================================================================
        ALLON
   ============================================================================================================================ */
#define REG_ALLON_PWRMOD_RESET               0x00000001            /*      Reset Value for PWRMOD  */
#define REG_ALLON_PWRMOD                     0x00000A00            /*  ALLON Power Modes */
#define REG_ALLON_PWRKEY_RESET               0x00000000            /*      Reset Value for PWRKEY  */
#define REG_ALLON_PWRKEY                     0x00000A04            /*  ALLON Key Protection for PWRMOD */
#define REG_ALLON_OSCKEY_RESET               0x00000000            /*      Reset Value for OSCKEY  */
#define REG_ALLON_OSCKEY                     0x00000A0C            /*  ALLON Key Protection for OSCCON */
#define REG_ALLON_OSCCON_RESET               0x00000003            /*      Reset Value for OSCCON  */
#define REG_ALLON_OSCCON                     0x00000A10            /*  ALLON Oscillator Control */
#define REG_ALLON_TMRCON_RESET               0x00000000            /*      Reset Value for TMRCON  */
#define REG_ALLON_TMRCON                     0x00000A1C            /*  ALLON Timer Wakeup Configuration */
#define REG_ALLON_EI0CON_RESET               0x00000000            /*      Reset Value for EI0CON  */
#define REG_ALLON_EI0CON                     0x00000A20            /*  ALLON External Interrupt Configuration 0 */
#define REG_ALLON_EI1CON_RESET               0x00000000            /*      Reset Value for EI1CON  */
#define REG_ALLON_EI1CON                     0x00000A24            /*  ALLON External Interrupt Configuration 1 */
#define REG_ALLON_EI2CON_RESET               0x00000000            /*      Reset Value for EI2CON  */
#define REG_ALLON_EI2CON                     0x00000A28            /*  ALLON External Interrupt Configuration 2 */
#define REG_ALLON_EICLR_RESET                0x0000C000            /*      Reset Value for EICLR  */
#define REG_ALLON_EICLR                      0x00000A30            /*  ALLON External Interrupt Clear */
#define REG_ALLON_RSTSTA_RESET               0x00000000            /*      Reset Value for RSTSTA  */
#define REG_ALLON_RSTSTA                     0x00000A40            /*  ALLON Reset Status */
#define REG_ALLON_RSTCONKEY_RESET            0x00000000            /*      Reset Value for RSTCONKEY  */
#define REG_ALLON_RSTCONKEY                  0x00000A5C            /*  ALLON Key Protection for RSTCON Register */
#define REG_ALLON_LOSCTST_RESET              0x0000008F            /*      Reset Value for LOSCTST  */
#define REG_ALLON_LOSCTST                    0x00000A6C            /*  ALLON Internal LF Oscillator Test */
#define REG_ALLON_CLKEN0_RESET               0x00000004            /*      Reset Value for CLKEN0  */
#define REG_ALLON_CLKEN0                     0x00000A70            /*  ALLON 32KHz Peripheral Clock Enable */

/* ============================================================================================================================
        ALLON Register BitMasks, Positions & Enumerations
   ============================================================================================================================ */
/* -------------------------------------------------------------------------------------------------------------------------
          ALLON_PWRMOD                         Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_ALLON_PWRMOD_RAMRETEN           15            /*  Retention for RAM */
#define BITP_ALLON_PWRMOD_ADCRETEN           14            /*  Keep ADC Power Switch on in Hibernate */
#define BITP_ALLON_PWRMOD_SEQSLPEN            3            /*  Auto Sleep by Sequencer Command */
#define BITP_ALLON_PWRMOD_TMRSLPEN            2            /*  Auto Sleep by Sleep Wakeup Timer */
#define BITP_ALLON_PWRMOD_PWRMOD              0            /*  Power Mode Control Bits */
#define BITM_ALLON_PWRMOD_RAMRETEN           0x00008000    /*  Retention for RAM */
#define BITM_ALLON_PWRMOD_ADCRETEN           0x00004000    /*  Keep ADC Power Switch on in Hibernate */
#define BITM_ALLON_PWRMOD_SEQSLPEN           0x00000008    /*  Auto Sleep by Sequencer Command */
#define BITM_ALLON_PWRMOD_TMRSLPEN           0x00000004    /*  Auto Sleep by Sleep Wakeup Timer */
#define BITM_ALLON_PWRMOD_PWRMOD             0x00000003    /*  Power Mode Control Bits */

/* -------------------------------------------------------------------------------------------------------------------------
          ALLON_PWRKEY                         Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_ALLON_PWRKEY_PWRKEY              0            /*  PWRMOD Key Register */
#define BITM_ALLON_PWRKEY_PWRKEY             0x0000FFFF    /*  PWRMOD Key Register */

/* -------------------------------------------------------------------------------------------------------------------------
          ALLON_OSCKEY                         Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_ALLON_OSCKEY_OSCKEY              0            /*  Oscillator Control Key Register. */
#define BITM_ALLON_OSCKEY_OSCKEY             0x0000FFFF    /*  Oscillator Control Key Register. */

/* -------------------------------------------------------------------------------------------------------------------------
          ALLON_OSCCON                         Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_ALLON_OSCCON_HFXTALOK           10            /*  Status of HFXTAL Oscillator */
#define BITP_ALLON_OSCCON_HFOSCOK             9            /*  Status of HFOSC Oscillator */
#define BITP_ALLON_OSCCON_LFOSCOK             8            /*  Status of LFOSC Oscillator */
#define BITP_ALLON_OSCCON_HFXTALEN            2            /*  High Frequency Crystal Oscillator Enable */
#define BITP_ALLON_OSCCON_HFOSCEN             1            /*  High Frequency Internal Oscillator Enable */
#define BITP_ALLON_OSCCON_LFOSCEN             0            /*  Low Frequency Internal Oscillator Enable */
#define BITM_ALLON_OSCCON_HFXTALOK           0x00000400    /*  Status of HFXTAL Oscillator */
#define BITM_ALLON_OSCCON_HFOSCOK            0x00000200    /*  Status of HFOSC Oscillator */
#define BITM_ALLON_OSCCON_LFOSCOK            0x00000100    /*  Status of LFOSC Oscillator */
#define BITM_ALLON_OSCCON_HFXTALEN           0x00000004    /*  High Frequency Crystal Oscillator Enable */
#define BITM_ALLON_OSCCON_HFOSCEN            0x00000002    /*  High Frequency Internal Oscillator Enable */
#define BITM_ALLON_OSCCON_LFOSCEN            0x00000001    /*  Low Frequency Internal Oscillator Enable */

/* -------------------------------------------------------------------------------------------------------------------------
          ALLON_TMRCON                         Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_ALLON_TMRCON_TMRINTEN            0            /*  Enable Wakeup Timer */
#define BITM_ALLON_TMRCON_TMRINTEN           0x00000001    /*  Enable Wakeup Timer */

/* -------------------------------------------------------------------------------------------------------------------------
          ALLON_EI0CON                         Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_ALLON_EI0CON_IRQ3EN             15            /*  External Interrupt 3 Enable Bit */
#define BITP_ALLON_EI0CON_IRQ3MDE            12            /*  External Interrupt 3 Mode Registers */
#define BITP_ALLON_EI0CON_IRQ2EN             11            /*  External Interrupt 2 Enable Bit */
#define BITP_ALLON_EI0CON_IRQ2MDE             8            /*  External Interrupt 2 Mode Registers */
#define BITP_ALLON_EI0CON_IRQ1EN              7            /*  External Interrupt 1 Enable Bit */
#define BITP_ALLON_EI0CON_IRQ1MDE             4            /*  External Interrupt 1 Mode Registers */
#define BITP_ALLON_EI0CON_IRQOEN              3            /*  External Interrupt 0 Enable Bit */
#define BITP_ALLON_EI0CON_IRQ0MDE             0            /*  External Interrupt 0 Mode Registers */
#define BITM_ALLON_EI0CON_IRQ3EN             0x00008000    /*  External Interrupt 3 Enable Bit */
#define BITM_ALLON_EI0CON_IRQ3MDE            0x00007000    /*  External Interrupt 3 Mode Registers */
#define BITM_ALLON_EI0CON_IRQ2EN             0x00000800    /*  External Interrupt 2 Enable Bit */
#define BITM_ALLON_EI0CON_IRQ2MDE            0x00000700    /*  External Interrupt 2 Mode Registers */
#define BITM_ALLON_EI0CON_IRQ1EN             0x00000080    /*  External Interrupt 1 Enable Bit */
#define BITM_ALLON_EI0CON_IRQ1MDE            0x00000070    /*  External Interrupt 1 Mode Registers */
#define BITM_ALLON_EI0CON_IRQOEN             0x00000008    /*  External Interrupt 0 Enable Bit */
#define BITM_ALLON_EI0CON_IRQ0MDE            0x00000007    /*  External Interrupt 0 Mode Registers */

/* -------------------------------------------------------------------------------------------------------------------------
          ALLON_EI1CON                         Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_ALLON_EI1CON_IRQ7EN             15            /*  External Interrupt 7 Enable Bit */
#define BITP_ALLON_EI1CON_IRQ7MDE            12            /*  External Interrupt 7 Mode Registers */
#define BITP_ALLON_EI1CON_IRQ6EN             11            /*  External Interrupt 6 Enable Bit */
#define BITP_ALLON_EI1CON_IRQ6MDE             8            /*  External Interrupt 6 Mode Registers */
#define BITP_ALLON_EI1CON_IRQ5EN              7            /*  External Interrupt 5 Enable Bit */
#define BITP_ALLON_EI1CON_IRQ5MDE             4            /*  External Interrupt 5 Mode Registers */
#define BITP_ALLON_EI1CON_IRQ4EN              3            /*  External Interrupt 4 Enable Bit */
#define BITP_ALLON_EI1CON_IRQ4MDE             0            /*  External Interrupt 4 Mode Registers */
#define BITM_ALLON_EI1CON_IRQ7EN             0x00008000    /*  External Interrupt 7 Enable Bit */
#define BITM_ALLON_EI1CON_IRQ7MDE            0x00007000    /*  External Interrupt 7 Mode Registers */
#define BITM_ALLON_EI1CON_IRQ6EN             0x00000800    /*  External Interrupt 6 Enable Bit */
#define BITM_ALLON_EI1CON_IRQ6MDE            0x00000700    /*  External Interrupt 6 Mode Registers */
#define BITM_ALLON_EI1CON_IRQ5EN             0x00000080    /*  External Interrupt 5 Enable Bit */
#define BITM_ALLON_EI1CON_IRQ5MDE            0x00000070    /*  External Interrupt 5 Mode Registers */
#define BITM_ALLON_EI1CON_IRQ4EN             0x00000008    /*  External Interrupt 4 Enable Bit */
#define BITM_ALLON_EI1CON_IRQ4MDE            0x00000007    /*  External Interrupt 4 Mode Registers */

/* -------------------------------------------------------------------------------------------------------------------------
          ALLON_EI2CON                         Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_ALLON_EI2CON_BUSINTEN            3            /*  BUS Interrupt Detection Enable Bit */
#define BITP_ALLON_EI2CON_BUSINTMDE           0            /*  BUS Interrupt Detection Mode Registers */
#define BITM_ALLON_EI2CON_BUSINTEN           0x00000008    /*  BUS Interrupt Detection Enable Bit */
#define BITM_ALLON_EI2CON_BUSINTMDE          0x00000007    /*  BUS Interrupt Detection Mode Registers */

/* -------------------------------------------------------------------------------------------------------------------------
          ALLON_EICLR                          Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_ALLON_EICLR_AUTCLRBUSEN         15            /*  Enable Auto Clear of Bus Interrupt */
#define BITP_ALLON_EICLR_BUSINT               8            /*  BUS Interrupt */
#define BITM_ALLON_EICLR_AUTCLRBUSEN         0x00008000    /*  Enable Auto Clear of Bus Interrupt */
#define BITM_ALLON_EICLR_BUSINT              0x00000100    /*  BUS Interrupt */

/* -------------------------------------------------------------------------------------------------------------------------
          ALLON_RSTSTA                         Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_ALLON_RSTSTA_PINSWRST            4            /*  Software Reset Pin */
#define BITP_ALLON_RSTSTA_MMRSWRST            3            /*  MMR Software Reset */
#define BITP_ALLON_RSTSTA_WDRST               2            /*  Watchdog Timeout */
#define BITP_ALLON_RSTSTA_EXTRST              1            /*  External Reset */
#define BITP_ALLON_RSTSTA_POR                 0            /*  Power-on Reset */
#define BITM_ALLON_RSTSTA_PINSWRST           0x00000010    /*  Software Reset Pin */
#define BITM_ALLON_RSTSTA_MMRSWRST           0x00000008    /*  MMR Software Reset */
#define BITM_ALLON_RSTSTA_WDRST              0x00000004    /*  Watchdog Timeout */
#define BITM_ALLON_RSTSTA_EXTRST             0x00000002    /*  External Reset */
#define BITM_ALLON_RSTSTA_POR                0x00000001    /*  Power-on Reset */

/* -------------------------------------------------------------------------------------------------------------------------
          ALLON_RSTCONKEY                      Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_ALLON_RSTCONKEY_KEY              0            /*  Reset Control Key Register */
#define BITM_ALLON_RSTCONKEY_KEY             0x0000FFFF    /*  Reset Control Key Register */

/* -------------------------------------------------------------------------------------------------------------------------
          ALLON_LOSCTST                        Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_ALLON_LOSCTST_TRIM               0            /*  Trim Caps to Adjust Frequency. */
#define BITM_ALLON_LOSCTST_TRIM              0x0000000F    /*  Trim Caps to Adjust Frequency. */

/* -------------------------------------------------------------------------------------------------------------------------
          ALLON_CLKEN0                         Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_ALLON_CLKEN0_TIACHPDIS           2            /*  TIA Chop Clock Disable */
#define BITP_ALLON_CLKEN0_SLPWUTDIS           1            /*  Sleep/Wakeup Timer Clock Disable */
#define BITP_ALLON_CLKEN0_WDTDIS              0            /*  Watch Dog Timer Clock Disable */
#define BITM_ALLON_CLKEN0_TIACHPDIS          0x00000004    /*  TIA Chop Clock Disable */
#define BITM_ALLON_CLKEN0_SLPWUTDIS          0x00000002    /*  Sleep/Wakeup Timer Clock Disable */
#define BITM_ALLON_CLKEN0_WDTDIS             0x00000001    /*  Watch Dog Timer Clock Disable */


/* ============================================================================================================================
        Some description.
   ============================================================================================================================ */

/* ============================================================================================================================
        SPII2CS
   ============================================================================================================================ */
#define REG_SPII2CS_PNTR0_RESET              0x00000000            /*      Reset Value for PNTR0  */
#define REG_SPII2CS_PNTR0                    0x00000C00            /*  SPII2CS SPI Slave Pointer 0 */
#define REG_SPII2CS_PNTR1_RESET              0x00000004            /*      Reset Value for PNTR1  */
#define REG_SPII2CS_PNTR1                    0x00000C04            /*  SPII2CS SPI Slave Pointer 1 */
#define REG_SPII2CS_PNTR2_RESET              0x00000008            /*      Reset Value for PNTR2  */
#define REG_SPII2CS_PNTR2                    0x00000C08            /*  SPII2CS SPI Slave Pointer 2 */

/* ============================================================================================================================
        SPII2CS Register BitMasks, Positions & Enumerations
   ============================================================================================================================ */
/* -------------------------------------------------------------------------------------------------------------------------
          SPII2CS_PNTR0                        Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_SPII2CS_PNTR0_PNTR0              0            /*  SPI Pointer 0 */
#define BITM_SPII2CS_PNTR0_PNTR0             0x0000FFFF    /*  SPI Pointer 0 */

/* -------------------------------------------------------------------------------------------------------------------------
          SPII2CS_PNTR1                        Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_SPII2CS_PNTR1_PNTR1              0            /*  SPI Pointer 1 */
#define BITM_SPII2CS_PNTR1_PNTR1             0x0000FFFF    /*  SPI Pointer 1 */

/* -------------------------------------------------------------------------------------------------------------------------
          SPII2CS_PNTR2                        Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_SPII2CS_PNTR2_PNTR2              0            /*  SPI Pointer 2 */
#define BITM_SPII2CS_PNTR2_PNTR2             0x0000FFFF    /*  SPI Pointer 2 */


/* ============================================================================================================================

   ============================================================================================================================ */

/* ============================================================================================================================
        AFE
   ============================================================================================================================ */
#define REG_AFE_AFECON_RESET                 0x00080000            /*      Reset Value for AFECON  */
#define REG_AFE_AFECON                       0x00002000            /*  AFE AFE Configuration */
#define REG_AFE_SEQCON_RESET                 0x00000002            /*      Reset Value for SEQCON  */
#define REG_AFE_SEQCON                       0x00002004            /*  AFE Sequencer Configuration */
#define REG_AFE_FIFOCON_RESET                0x00001010            /*      Reset Value for FIFOCON  */
#define REG_AFE_FIFOCON                      0x00002008            /*  AFE FIFOs Configuration */
#define REG_AFE_SWCON_RESET                  0x0000FFFF            /*      Reset Value for SWCON  */
#define REG_AFE_SWCON                        0x0000200C            /*  AFE Switch Matrix Configuration */
#define REG_AFE_HSDACCON_RESET               0x0000001E            /*      Reset Value for HSDACCON  */
#define REG_AFE_HSDACCON                     0x00002010            /*  AFE High Speed DAC Configuration */
#define REG_AFE_WGCON_RESET                  0x00000030            /*      Reset Value for WGCON  */
#define REG_AFE_WGCON                        0x00002014            /*  AFE Waveform Generator Configuration */
#define REG_AFE_WGDCLEVEL1_RESET             0x00000000            /*      Reset Value for WGDCLEVEL1  */
#define REG_AFE_WGDCLEVEL1                   0x00002018            /*  AFE Waveform Generator - Trapezoid DC Level 1 */
#define REG_AFE_WGDCLEVEL2_RESET             0x00000000            /*      Reset Value for WGDCLEVEL2  */
#define REG_AFE_WGDCLEVEL2                   0x0000201C            /*  AFE Waveform Generator - Trapezoid DC Level 2 */
#define REG_AFE_WGDELAY1_RESET               0x00000000            /*      Reset Value for WGDELAY1  */
#define REG_AFE_WGDELAY1                     0x00002020            /*  AFE Waveform Generator - Trapezoid Delay 1 Time */
#define REG_AFE_WGSLOPE1_RESET               0x00000000            /*      Reset Value for WGSLOPE1  */
#define REG_AFE_WGSLOPE1                     0x00002024            /*  AFE Waveform Generator - Trapezoid Slope 1 Time */
#define REG_AFE_WGDELAY2_RESET               0x00000000            /*      Reset Value for WGDELAY2  */
#define REG_AFE_WGDELAY2                     0x00002028            /*  AFE Waveform Generator - Trapezoid Delay 2 Time */
#define REG_AFE_WGSLOPE2_RESET               0x00000000            /*      Reset Value for WGSLOPE2  */
#define REG_AFE_WGSLOPE2                     0x0000202C            /*  AFE Waveform Generator - Trapezoid Slope 2 Time */
#define REG_AFE_WGFCW_RESET                  0x00000000            /*      Reset Value for WGFCW  */
#define REG_AFE_WGFCW                        0x00002030            /*  AFE Waveform Generator - Sinusoid Frequency Control Word */
#define REG_AFE_WGPHASE_RESET                0x00000000            /*      Reset Value for WGPHASE  */
#define REG_AFE_WGPHASE                      0x00002034            /*  AFE Waveform Generator - Sinusoid Phase Offset */
#define REG_AFE_WGOFFSET_RESET               0x00000000            /*      Reset Value for WGOFFSET  */
#define REG_AFE_WGOFFSET                     0x00002038            /*  AFE Waveform Generator - Sinusoid Offset */
#define REG_AFE_WGAMPLITUDE_RESET            0x00000000            /*      Reset Value for WGAMPLITUDE  */
#define REG_AFE_WGAMPLITUDE                  0x0000203C            /*  AFE Waveform Generator - Sinusoid Amplitude */
#define REG_AFE_ADCFILTERCON_RESET           0x00000301            /*      Reset Value for ADCFILTERCON  */
#define REG_AFE_ADCFILTERCON                 0x00002044            /*  AFE ADC Output Filters Configuration */
#define REG_AFE_HSDACDAT_RESET               0x00000800            /*      Reset Value for HSDACDAT  */
#define REG_AFE_HSDACDAT                     0x00002048            /*  AFE HS DAC Code */
#define REG_AFE_LPREFBUFCON_RESET            0x00000000            /*      Reset Value for LPREFBUFCON  */
#define REG_AFE_LPREFBUFCON                  0x00002050            /*  AFE LPREF_BUF_CON */
#define REG_AFE_SYNCEXTDEVICE_RESET          0x00000000            /*      Reset Value for SYNCEXTDEVICE  */
#define REG_AFE_SYNCEXTDEVICE                0x00002054            /*  AFE SYNC External Devices */
#define REG_AFE_SEQCRC_RESET                 0x00000001            /*      Reset Value for SEQCRC  */
#define REG_AFE_SEQCRC                       0x00002060            /*  AFE Sequencer CRC Value */
#define REG_AFE_SEQCNT_RESET                 0x00000000            /*      Reset Value for SEQCNT  */
#define REG_AFE_SEQCNT                       0x00002064            /*  AFE Sequencer Command Count */
#define REG_AFE_SEQTIMEOUT_RESET             0x00000000            /*      Reset Value for SEQTIMEOUT  */
#define REG_AFE_SEQTIMEOUT                   0x00002068            /*  AFE Sequencer Timeout Counter */
#define REG_AFE_DATAFIFORD_RESET             0x00000000            /*      Reset Value for DATAFIFORD  */
#define REG_AFE_DATAFIFORD                   0x0000206C            /*  AFE Data FIFO Read */
#define REG_AFE_CMDFIFOWRITE_RESET           0x00000000            /*      Reset Value for CMDFIFOWRITE  */
#define REG_AFE_CMDFIFOWRITE                 0x00002070            /*  AFE Command FIFO Write */
#define REG_AFE_ADCDAT_RESET                 0x00000000            /*      Reset Value for ADCDAT  */
#define REG_AFE_ADCDAT                       0x00002074            /*  AFE ADC Raw Result */
#define REG_AFE_DFTREAL_RESET                0x00000000            /*      Reset Value for DFTREAL  */
#define REG_AFE_DFTREAL                      0x00002078            /*  AFE DFT Result, Real Part */
#define REG_AFE_DFTIMAG_RESET                0x00000000            /*      Reset Value for DFTIMAG  */
#define REG_AFE_DFTIMAG                      0x0000207C            /*  AFE DFT Result, Imaginary Part */
#define REG_AFE_SINC2DAT_RESET               0x00000000            /*      Reset Value for SINC2DAT  */
#define REG_AFE_SINC2DAT                     0x00002080            /*  AFE Supply Rejection Filter Result */
#define REG_AFE_TEMPSENSDAT_RESET            0x00000000            /*      Reset Value for TEMPSENSDAT  */
#define REG_AFE_TEMPSENSDAT                  0x00002084            /*  AFE Temperature Sensor Result */
#define REG_AFE_AFEGENINTSTA_RESET           0x00000000            /*      Reset Value for AFEGENINTSTA  */
#define REG_AFE_AFEGENINTSTA                 0x0000209C            /*  AFE Analog Generation Interrupt */
#define REG_AFE_ADCMIN_RESET                 0x00000000            /*      Reset Value for ADCMIN  */
#define REG_AFE_ADCMIN                       0x000020A8            /*  AFE ADC Minimum Value Check */
#define REG_AFE_ADCMINSM_RESET               0x00000000            /*      Reset Value for ADCMINSM  */
#define REG_AFE_ADCMINSM                     0x000020AC            /*  AFE ADCMIN Hysteresis Value */
#define REG_AFE_ADCMAX_RESET                 0x00000000            /*      Reset Value for ADCMAX  */
#define REG_AFE_ADCMAX                       0x000020B0            /*  AFE ADC Maximum Value Check */
#define REG_AFE_ADCMAXSMEN_RESET             0x00000000            /*      Reset Value for ADCMAXSMEN  */
#define REG_AFE_ADCMAXSMEN                   0x000020B4            /*  AFE ADCMAX Hysteresis Value */
#define REG_AFE_ADCDELTA_RESET               0x00000000            /*      Reset Value for ADCDELTA  */
#define REG_AFE_ADCDELTA                     0x000020B8            /*  AFE ADC Delta Value */
#define REG_AFE_HPOSCCON_RESET               0x00000024            /*      Reset Value for HPOSCCON  */
#define REG_AFE_HPOSCCON                     0x000020BC            /*  AFE HPOSC Configuration */
#define REG_AFE_DFTCON_RESET                 0x00000090            /*      Reset Value for DFTCON  */
#define REG_AFE_DFTCON                       0x000020D0            /*  AFE AFE DSP Configuration */
#define REG_AFE_LPTIASW0_RESET               0x00000000            /*      Reset Value for LPTIASW0  */
#define REG_AFE_LPTIASW0                     0x000020E4            /*  AFE ULPTIA Switch Configuration for Channel 0 */
#define REG_AFE_LPTIACON0_RESET              0x00000003            /*      Reset Value for LPTIACON0  */
#define REG_AFE_LPTIACON0                    0x000020EC            /*  AFE ULPTIA Control Bits Channel 0 */
#define REG_AFE_HSRTIACON_RESET              0x0000000F            /*      Reset Value for HSRTIACON  */
#define REG_AFE_HSRTIACON                    0x000020F0            /*  AFE High Power RTIA Configuration */
#define REG_AFE_DE0RESCON_RESET              0x000000FF            /*      Reset Value for DE0RESCON  */
#define REG_AFE_DE0RESCON                    0x000020F8            /*  AFE DE0 HSTIA Resistors Configuration */
#define REG_AFE_HSTIACON_RESET               0x00000000            /*      Reset Value for HSTIACON  */
#define REG_AFE_HSTIACON                     0x000020FC            /*  AFE HSTIA Amplifier Configuration */
#define REG_AFE_LPMODEKEY_RESET             0x00000000            /*      Reset Value for LPMODEKEY  */
#define REG_AFE_LPMODEKEY                   0x0000210C            /*  AFE LP Mode AFE Control Lock */
#define REG_AFE_LPMODECLKSEL_RESET          0x00000000            /*      Reset Value for LPMODECLKSEL  */
#define REG_AFE_LPMODECLKSEL                0x00002110            /*  AFE LFSYSCLKEN */
#define REG_AFE_LPMODECON_RESET             0x00000102            /*      Reset Value for LPMODECON  */
#define REG_AFE_LPMODECON                   0x00002114            /*  AFE LPMODECON */
#define REG_AFE_SEQSLPLOCK_RESET             0x00000000            /*      Reset Value for SEQSLPLOCK  */
#define REG_AFE_SEQSLPLOCK                   0x00002118            /*  AFE Sequencer Sleep Control Lock */
#define REG_AFE_SEQTRGSLP_RESET              0x00000000            /*      Reset Value for SEQTRGSLP  */
#define REG_AFE_SEQTRGSLP                    0x0000211C            /*  AFE Sequencer Trigger Sleep */
#define REG_AFE_LPDACDAT0_RESET              0x00000000            /*      Reset Value for LPDACDAT0  */
#define REG_AFE_LPDACDAT0                    0x00002120            /*  AFE LPDAC Data-out */
#define REG_AFE_LPDACSW0_RESET               0x00000000            /*      Reset Value for LPDACSW0  */
#define REG_AFE_LPDACSW0                     0x00002124            /*  AFE LPDAC0 Switch Control */
#define REG_AFE_LPDACCON0_RESET              0x00000002            /*      Reset Value for LPDACCON0  */
#define REG_AFE_LPDACCON0                    0x00002128            /*  AFE LPDAC Control Bits */
#define REG_AFE_DSWFULLCON_RESET             0x00000000            /*      Reset Value for DSWFULLCON  */
#define REG_AFE_DSWFULLCON                   0x00002150            /*  AFE Switch Matrix Full Configuration (D) */
#define REG_AFE_NSWFULLCON_RESET             0x00000000            /*      Reset Value for NSWFULLCON  */
#define REG_AFE_NSWFULLCON                   0x00002154            /*  AFE Switch Matrix Full Configuration (N) */
#define REG_AFE_PSWFULLCON_RESET             0x00000000            /*      Reset Value for PSWFULLCON  */
#define REG_AFE_PSWFULLCON                   0x00002158            /*  AFE Switch Matrix Full Configuration (P) */
#define REG_AFE_TSWFULLCON_RESET             0x00000000            /*      Reset Value for TSWFULLCON  */
#define REG_AFE_TSWFULLCON                   0x0000215C            /*  AFE Switch Matrix Full Configuration (T) */
#define REG_AFE_TEMPSENS_RESET               0x00000000            /*      Reset Value for TEMPSENS  */
#define REG_AFE_TEMPSENS                     0x00002174            /*  AFE Temp Sensor Configuration */
#define REG_AFE_BUFSENCON_RESET              0x00000037            /*      Reset Value for BUFSENCON  */
#define REG_AFE_BUFSENCON                    0x00002180            /*  AFE HP and LP Buffer Control */
#define REG_AFE_ADCCON_RESET                 0x00000000            /*      Reset Value for ADCCON  */
#define REG_AFE_ADCCON                       0x000021A8            /*  AFE ADC Configuration */
#define REG_AFE_DSWSTA_RESET                 0x00000000            /*      Reset Value for DSWSTA  */
#define REG_AFE_DSWSTA                       0x000021B0            /*  AFE Switch Matrix Status (D) */
#define REG_AFE_PSWSTA_RESET                 0x00006000            /*      Reset Value for PSWSTA  */
#define REG_AFE_PSWSTA                       0x000021B4            /*  AFE Switch Matrix Status (P) */
#define REG_AFE_NSWSTA_RESET                 0x00000C00            /*      Reset Value for NSWSTA  */
#define REG_AFE_NSWSTA                       0x000021B8            /*  AFE Switch Matrix Status (N) */
#define REG_AFE_TSWSTA_RESET                 0x00000000            /*      Reset Value for TSWSTA  */
#define REG_AFE_TSWSTA                       0x000021BC            /*  AFE Switch Matrix Status (T) */
#define REG_AFE_STATSVAR_RESET               0x00000000            /*      Reset Value for STATSVAR  */
#define REG_AFE_STATSVAR                     0x000021C0            /*  AFE Variance Output */
#define REG_AFE_STATSCON_RESET               0x00000000            /*      Reset Value for STATSCON  */
#define REG_AFE_STATSCON                     0x000021C4            /*  AFE Statistics Control */
#define REG_AFE_STATSMEAN_RESET              0x00000000            /*      Reset Value for STATSMEAN  */
#define REG_AFE_STATSMEAN                    0x000021C8            /*  AFE Statistics Mean Output */
#define REG_AFE_SEQ0INFO_RESET               0x00000000            /*      Reset Value for SEQ0INFO  */
#define REG_AFE_SEQ0INFO                     0x000021CC            /*  AFE Sequence 0 Info */
#define REG_AFE_SEQ2INFO_RESET               0x00000000            /*      Reset Value for SEQ2INFO  */
#define REG_AFE_SEQ2INFO                     0x000021D0            /*  AFE Sequence 2 Info */
#define REG_AFE_CMDFIFOWADDR_RESET           0x00000000            /*      Reset Value for CMDFIFOWADDR  */
#define REG_AFE_CMDFIFOWADDR                 0x000021D4            /*  AFE Command FIFO Write Address */
#define REG_AFE_CMDDATACON_RESET             0x00000410            /*      Reset Value for CMDDATACON  */
#define REG_AFE_CMDDATACON                   0x000021D8            /*  AFE Command Data Control */
#define REG_AFE_DATAFIFOTHRES_RESET          0x00000000            /*      Reset Value for DATAFIFOTHRES  */
#define REG_AFE_DATAFIFOTHRES                0x000021E0            /*  AFE Data FIFO Threshold */
#define REG_AFE_SEQ3INFO_RESET               0x00000000            /*      Reset Value for SEQ3INFO  */
#define REG_AFE_SEQ3INFO                     0x000021E4            /*  AFE Sequence 3 Info */
#define REG_AFE_SEQ1INFO_RESET               0x00000000            /*      Reset Value for SEQ1INFO  */
#define REG_AFE_SEQ1INFO                     0x000021E8            /*  AFE Sequence 1 Info */
#define REG_AFE_REPEATADCCNV_RESET           0x00000160            /*      Reset Value for REPEATADCCNV  */
#define REG_AFE_REPEATADCCNV                 0x000021F0            /*  AFE REPEAT ADC Conversions */
#define REG_AFE_FIFOCNTSTA_RESET             0x00000000            /*      Reset Value for FIFOCNTSTA  */
#define REG_AFE_FIFOCNTSTA                   0x00002200            /*  AFE CMD and DATA FIFO INTERNAL DATA COUNT */
#define REG_AFE_CALDATLOCK_RESET             0x00000000            /*      Reset Value for CALDATLOCK  */
#define REG_AFE_CALDATLOCK                   0x00002230            /*  AFE Calibration Data Lock */
#define REG_AFE_ADCOFFSETHSTIA_RESET         0x00000000            /*      Reset Value for ADCOFFSETHSTIA  */
#define REG_AFE_ADCOFFSETHSTIA               0x00002234            /*  AFE ADC Offset Calibration High Speed TIA Channel */
#define REG_AFE_ADCGAINTEMPSENS0_RESET       0x00004000            /*      Reset Value for ADCGAINTEMPSENS0  */
#define REG_AFE_ADCGAINTEMPSENS0             0x00002238            /*  AFE ADC Gain Calibration Temp Sensor Channel */
#define REG_AFE_ADCOFFSETTEMPSENS0_RESET     0x00000000            /*      Reset Value for ADCOFFSETTEMPSENS0  */
#define REG_AFE_ADCOFFSETTEMPSENS0           0x0000223C            /*  AFE ADC Offset Calibration Temp Sensor Channel 0 */
#define REG_AFE_ADCGAINGN1_RESET             0x00004000            /*      Reset Value for ADCGAINGN1  */
#define REG_AFE_ADCGAINGN1                   0x00002240            /*  AFE ADCPGAGN1: ADC Gain Calibration Auxiliary Input Channel */
#define REG_AFE_ADCOFFSETGN1_RESET           0x00000000            /*      Reset Value for ADCOFFSETGN1  */
#define REG_AFE_ADCOFFSETGN1                 0x00002244            /*  AFE ADC Offset Calibration Auxiliary Channel (PGA Gain=1) */
#define REG_AFE_DACGAIN_RESET                0x00000800            /*      Reset Value for DACGAIN  */
#define REG_AFE_DACGAIN                      0x00002260            /*  AFE DACGAIN */
#define REG_AFE_DACOFFSETATTEN_RESET         0x00000000            /*      Reset Value for DACOFFSETATTEN  */
#define REG_AFE_DACOFFSETATTEN               0x00002264            /*  AFE DAC Offset with Attenuator Enabled (LP Mode) */
#define REG_AFE_DACOFFSET_RESET              0x00000000            /*      Reset Value for DACOFFSET  */
#define REG_AFE_DACOFFSET                    0x00002268            /*  AFE DAC Offset with Attenuator Disabled (LP Mode) */
#define REG_AFE_ADCGAINGN1P5_RESET           0x00004000            /*      Reset Value for ADCGAINGN1P5  */
#define REG_AFE_ADCGAINGN1P5                 0x00002270            /*  AFE ADC Gain Calibration Auxiliary Input Channel (PGA Gain=1.5) */
#define REG_AFE_ADCGAINGN2_RESET             0x00004000            /*      Reset Value for ADCGAINGN2  */
#define REG_AFE_ADCGAINGN2                   0x00002274            /*  AFE ADC Gain Calibration Auxiliary Input Channel (PGA Gain=2) */
#define REG_AFE_ADCGAINGN4_RESET             0x00004000            /*      Reset Value for ADCGAINGN4  */
#define REG_AFE_ADCGAINGN4                   0x00002278            /*  AFE ADC Gain Calibration Auxiliary Input Channel (PGA Gain=4) */
#define REG_AFE_ADCPGAOFFSETCANCEL_RESET     0x00000000            /*      Reset Value for ADCPGAOFFSETCANCEL  */
#define REG_AFE_ADCPGAOFFSETCANCEL           0x00002280            /*  AFE ADC Offset Cancellation (Optional) */
#define REG_AFE_ADCGNHSTIA_RESET             0x00004000            /*      Reset Value for ADCGNHSTIA  */
#define REG_AFE_ADCGNHSTIA                   0x00002284            /*  AFE ADC Gain Calibration for HS TIA Channel */
#define REG_AFE_ADCOFFSETLPTIA0_RESET        0x00000000            /*      Reset Value for ADCOFFSETLPTIA0  */
#define REG_AFE_ADCOFFSETLPTIA0              0x00002288            /*  AFE ADC Offset Calibration ULP-TIA0 Channel */
#define REG_AFE_ADCGNLPTIA0_RESET            0x00004000            /*      Reset Value for ADCGNLPTIA0  */
#define REG_AFE_ADCGNLPTIA0                  0x0000228C            /*  AFE ADC GAIN Calibration for LP TIA0 Channel */
#define REG_AFE_ADCPGAGN4OFCAL_RESET         0x00004000            /*      Reset Value for ADCPGAGN4OFCAL  */
#define REG_AFE_ADCPGAGN4OFCAL               0x00002294            /*  AFE ADC Gain Calibration with DC Cancellation(PGA G=4) */
#define REG_AFE_ADCGAINGN9_RESET             0x00004000            /*      Reset Value for ADCGAINGN9  */
#define REG_AFE_ADCGAINGN9                   0x00002298            /*  AFE ADC Gain Calibration Auxiliary Input Channel (PGA Gain=9) */
#define REG_AFE_ADCOFFSETEMPSENS1_RESET      0x00000000            /*      Reset Value for ADCOFFSETEMPSENS1  */
#define REG_AFE_ADCOFFSETEMPSENS1            0x000022A8            /*  AFE ADC Offset Calibration  Temp Sensor Channel 1 */
#define REG_AFE_ADCGAINDIOTEMPSENS_RESET     0x00004000            /*      Reset Value for ADCGAINDIOTEMPSENS  */
#define REG_AFE_ADCGAINDIOTEMPSENS           0x000022AC            /*  AFE ADC Gain Calibration Diode Temperature Sensor Channel */
#define REG_AFE_DACOFFSETATTENHP_RESET       0x00000000            /*      Reset Value for DACOFFSETATTENHP  */
#define REG_AFE_DACOFFSETATTENHP             0x000022B8            /*  AFE DAC Offset with Attenuator Enabled (HP Mode) */
#define REG_AFE_DACOFFSETHP_RESET            0x00000000            /*      Reset Value for DACOFFSETHP  */
#define REG_AFE_DACOFFSETHP                  0x000022BC            /*  AFE DAC Offset with Attenuator Disabled (HP Mode) */
#define REG_AFE_ADCGNLPTIA1_RESET            0x00004000            /*      Reset Value for ADCGNLPTIA1  */
#define REG_AFE_ADCGNLPTIA1                  0x000022C4            /*  AFE ADC GAIN Calibration for LP TIA1 Channel */
#define REG_AFE_ADCOFFSETGN2_RESET           0x00000000            /*      Reset Value for ADCOFFSETGN2  */
#define REG_AFE_ADCOFFSETGN2                 0x000022C8            /*  AFE Offset Calibration Auxiliary Channel (PGA Gain =2) */
#define REG_AFE_ADCOFFSETGN1P5_RESET         0x00000000            /*      Reset Value for ADCOFFSETGN1P5  */
#define REG_AFE_ADCOFFSETGN1P5               0x000022CC            /*  AFE Offset Calibration Auxiliary Channel (PGA Gain =1.5) */
#define REG_AFE_ADCOFFSETGN9_RESET           0x00000000            /*      Reset Value for ADCOFFSETGN9  */
#define REG_AFE_ADCOFFSETGN9                 0x000022D0            /*  AFE Offset Calibration Auxiliary Channel (PGA Gain =9) */
#define REG_AFE_ADCOFFSETGN4_RESET           0x00000000            /*      Reset Value for ADCOFFSETGN4  */
#define REG_AFE_ADCOFFSETGN4                 0x000022D4            /*  AFE Offset Calibration Auxiliary Channel (PGA Gain =4) */
#define REG_AFE_PMBW_RESET                   0x00088800            /*      Reset Value for PMBW  */
#define REG_AFE_PMBW                         0x000022F0            /*  AFE Power Mode Configuration */
#define REG_AFE_SWMUX_RESET                 0x00000000            /*      Reset Value for SWMUX  */
#define REG_AFE_SWMUX                       0x0000235C            /*  AFE Switch Mux for ECG */
#define REG_AFE_AFE_TEMPSEN_DIO_RESET        0x00020000            /*      Reset Value for AFE_TEMPSEN_DIO  */
#define REG_AFE_AFE_TEMPSEN_DIO              0x00002374            /*  AFE AFE_TEMPSEN_DIO */
#define REG_AFE_ADCBUFCON_RESET              0x005F3D00            /*      Reset Value for ADCBUFCON  */
#define REG_AFE_ADCBUFCON                    0x0000238C            /*  AFE Configure ADC Input Buffer */

/* ============================================================================================================================
        AFE Register BitMasks, Positions & Enumerations
   ============================================================================================================================ */
/* -------------------------------------------------------------------------------------------------------------------------
          AFE_AFECON                           Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_AFECON_DACBUFEN             21            /*  Enable DC DAC Buffer */
#define BITP_AFE_AFECON_DACREFEN             20            /*  High Speed DAC Reference Enable */
#define BITP_AFE_AFECON_ALDOILIMITEN         19            /*  Analog LDO Current Limiting Enable */
#define BITP_AFE_AFECON_SINC2EN              16            /*  ADC Output 50/60Hz Filter Enable */
#define BITP_AFE_AFECON_DFTEN                15            /*  DFT Hardware Accelerator Enable */
#define BITP_AFE_AFECON_WAVEGENEN            14            /*  Waveform Generator Enable */
#define BITP_AFE_AFECON_TEMPCONVEN           13            /*  ADC Temp Sensor Convert Enable */
#define BITP_AFE_AFECON_TEMPSENSEN           12            /*  ADC Temperature Sensor Channel Enable */
#define BITP_AFE_AFECON_TIAEN                11            /*  High Power TIA Enable */
#define BITP_AFE_AFECON_INAMPEN              10            /*  Enable Excitation Amplifier */
#define BITP_AFE_AFECON_EXBUFEN               9            /*  Enable Excitation Buffer */
#define BITP_AFE_AFECON_ADCCONVEN             8            /*  ADC Conversion Start Enable */
#define BITP_AFE_AFECON_ADCEN                 7            /*  ADC Power Enable */
#define BITP_AFE_AFECON_DACEN                 6            /*  High Power DAC Enable */
#define BITP_AFE_AFECON_HPREFDIS              5            /*  Disable High Power Reference */
#define BITM_AFE_AFECON_DACBUFEN             0x00200000    /*  Enable DC DAC Buffer */
#define BITM_AFE_AFECON_DACREFEN             0x00100000    /*  High Speed DAC Reference Enable */
#define BITM_AFE_AFECON_ALDOILIMITEN         0x00080000    /*  Analog LDO Current Limiting Enable */
#define BITM_AFE_AFECON_SINC2EN              0x00010000    /*  ADC Output 50/60Hz Filter Enable */
#define BITM_AFE_AFECON_DFTEN                0x00008000    /*  DFT Hardware Accelerator Enable */
#define BITM_AFE_AFECON_WAVEGENEN            0x00004000    /*  Waveform Generator Enable */
#define BITM_AFE_AFECON_TEMPCONVEN           0x00002000    /*  ADC Temp Sensor Convert Enable */
#define BITM_AFE_AFECON_TEMPSENSEN           0x00001000    /*  ADC Temperature Sensor Channel Enable */
#define BITM_AFE_AFECON_TIAEN                0x00000800    /*  High Power TIA Enable */
#define BITM_AFE_AFECON_INAMPEN              0x00000400    /*  Enable Excitation Amplifier */
#define BITM_AFE_AFECON_EXBUFEN              0x00000200    /*  Enable Excitation Buffer */
#define BITM_AFE_AFECON_ADCCONVEN            0x00000100    /*  ADC Conversion Start Enable */
#define BITM_AFE_AFECON_ADCEN                0x00000080    /*  ADC Power Enable */
#define BITM_AFE_AFECON_DACEN                0x00000040    /*  High Power DAC Enable */
#define BITM_AFE_AFECON_HPREFDIS             0x00000020    /*  Disable High Power Reference */
#define ENUM_AFE_AFECON_OFF                  0x00000000            /*  DACEN: High Power DAC Disabled */
#define ENUM_AFE_AFECON_ON                   0x00000040            /*  DACEN: High Power DAC Enabled */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_SEQCON                           Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_SEQCON_SEQWRTMR              8            /*  Timer for Sequencer Write Commands */
#define BITP_AFE_SEQCON_SEQHALT               4            /*  Halt Seq */
#define BITP_AFE_SEQCON_SEQHALTFIFOEMPTY      1            /*  Halt Sequencer If Empty */
#define BITP_AFE_SEQCON_SEQEN                 0            /*  Enable Sequencer */
#define BITM_AFE_SEQCON_SEQWRTMR             0x0000FF00    /*  Timer for Sequencer Write Commands */
#define BITM_AFE_SEQCON_SEQHALT              0x00000010    /*  Halt Seq */
#define BITM_AFE_SEQCON_SEQHALTFIFOEMPTY     0x00000002    /*  Halt Sequencer If Empty */
#define BITM_AFE_SEQCON_SEQEN                0x00000001    /*  Enable Sequencer */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_FIFOCON                          Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_FIFOCON_DATAFIFOSRCSEL      13            /*  Selects the Source for the Data FIFO. */
#define BITP_AFE_FIFOCON_DATAFIFOEN          11            /*  Data FIFO Enable. */
#define BITM_AFE_FIFOCON_DATAFIFOSRCSEL      0x0000E000    /*  Selects the Source for the Data FIFO. */
#define BITM_AFE_FIFOCON_DATAFIFOEN          0x00000800    /*  Data FIFO Enable. */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_SWCON                            Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_SWCON_T11CON                19            /*  Control of T[11] */
#define BITP_AFE_SWCON_T10CON                18            /*  Control of T[10] */
#define BITP_AFE_SWCON_T9CON                 17            /*  Control of T[9] */
#define BITP_AFE_SWCON_SWSOURCESEL           16            /*  Switch Control Select */
#define BITP_AFE_SWCON_TMUXCON               12            /*  Control of T Switch MUX. */
#define BITP_AFE_SWCON_NMUXCON                8            /*  Control of N Switch MUX */
#define BITP_AFE_SWCON_PMUXCON                4            /*  Control of P Switch MUX */
#define BITP_AFE_SWCON_DMUXCON                0            /*  Control of D Switch MUX */
#define BITM_AFE_SWCON_T11CON                0x00080000    /*  Control of T[11] */
#define BITM_AFE_SWCON_T10CON                0x00040000    /*  Control of T[10] */
#define BITM_AFE_SWCON_T9CON                 0x00020000    /*  Control of T[9] */
#define BITM_AFE_SWCON_SWSOURCESEL           0x00010000    /*  Switch Control Select */
#define BITM_AFE_SWCON_TMUXCON               0x0000F000    /*  Control of T Switch MUX. */
#define BITM_AFE_SWCON_NMUXCON               0x00000F00    /*  Control of N Switch MUX */
#define BITM_AFE_SWCON_PMUXCON               0x000000F0    /*  Control of P Switch MUX */
#define BITM_AFE_SWCON_DMUXCON               0x0000000F    /*  Control of D Switch MUX */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_HSDACCON                         Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_HSDACCON_INAMPGNMDE         12            /*  Excitation Amplifier Gain Control */
#define BITP_AFE_HSDACCON_RATE                1            /*  DAC Update Rate */
#define BITP_AFE_HSDACCON_ATTENEN             0            /*  PGA Stage Gain Attenuation */
#define BITM_AFE_HSDACCON_INAMPGNMDE         0x00001000    /*  Excitation Amplifier Gain Control */
#define BITM_AFE_HSDACCON_RATE               0x000001FE    /*  DAC Update Rate */
#define BITM_AFE_HSDACCON_ATTENEN            0x00000001    /*  PGA Stage Gain Attenuation */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_WGCON                            Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_WGCON_DACGAINCAL             5            /*  Bypass DAC Gain */
#define BITP_AFE_WGCON_DACOFFSETCAL           4            /*  Bypass DAC Offset */
#define BITP_AFE_WGCON_TYPESEL                1            /*  Selects the Type of Waveform */
#define BITP_AFE_WGCON_TRAPRSTEN              0            /*  Resets the Trapezoid Waveform Generator */
#define BITM_AFE_WGCON_DACGAINCAL            0x00000020    /*  Bypass DAC Gain */
#define BITM_AFE_WGCON_DACOFFSETCAL          0x00000010    /*  Bypass DAC Offset */
#define BITM_AFE_WGCON_TYPESEL               0x00000006    /*  Selects the Type of Waveform */
#define BITM_AFE_WGCON_TRAPRSTEN             0x00000001    /*  Resets the Trapezoid Waveform Generator */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_WGDCLEVEL1                       Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_WGDCLEVEL1_TRAPDCLEVEL1      0            /*  DC Level 1 Value for Trapezoid Waveform Generation */
#define BITM_AFE_WGDCLEVEL1_TRAPDCLEVEL1     0x00000FFF    /*  DC Level 1 Value for Trapezoid Waveform Generation */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_WGDCLEVEL2                       Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_WGDCLEVEL2_TRAPDCLEVEL2      0            /*  DC Level 2 Value for Trapezoid Waveform Generation */
#define BITM_AFE_WGDCLEVEL2_TRAPDCLEVEL2     0x00000FFF    /*  DC Level 2 Value for Trapezoid Waveform Generation */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_WGDELAY1                         Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_WGDELAY1_DELAY1              0            /*  Delay 1 Value for Trapezoid Waveform Generation */
#define BITM_AFE_WGDELAY1_DELAY1             0x000FFFFF    /*  Delay 1 Value for Trapezoid Waveform Generation */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_WGSLOPE1                         Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_WGSLOPE1_SLOPE1              0            /*  Slope 1 Value for Trapezoid Waveform Generation */
#define BITM_AFE_WGSLOPE1_SLOPE1             0x000FFFFF    /*  Slope 1 Value for Trapezoid Waveform Generation */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_WGDELAY2                         Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_WGDELAY2_DELAY2              0            /*  Delay 2 Value for Trapezoid Waveform Generation */
#define BITM_AFE_WGDELAY2_DELAY2             0x000FFFFF    /*  Delay 2 Value for Trapezoid Waveform Generation */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_WGSLOPE2                         Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_WGSLOPE2_SLOPE2              0            /*  Slope 2 Value for Trapezoid Waveform Generation. */
#define BITM_AFE_WGSLOPE2_SLOPE2             0x000FFFFF    /*  Slope 2 Value for Trapezoid Waveform Generation. */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_WGFCW                            Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_WGFCW_SINEFCW                0            /*  Sinusoid Generator Frequency Control Word */
#define BITM_AFE_WGFCW_SINEFCW               0x00FFFFFF    /*  Sinusoid Generator Frequency Control Word */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_WGPHASE                          Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_WGPHASE_SINEOFFSET           0            /*  Sinusoid Phase Offset */
#define BITM_AFE_WGPHASE_SINEOFFSET          0x000FFFFF    /*  Sinusoid Phase Offset */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_WGOFFSET                         Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_WGOFFSET_SINEOFFSET          0            /*  Sinusoid Offset */
#define BITM_AFE_WGOFFSET_SINEOFFSET         0x00000FFF    /*  Sinusoid Offset */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_WGAMPLITUDE                      Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_WGAMPLITUDE_SINEAMPLITUDE    0            /*  Sinusoid Amplitude */
#define BITM_AFE_WGAMPLITUDE_SINEAMPLITUDE   0x000007FF    /*  Sinusoid Amplitude */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_ADCFILTERCON                     Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_ADCFILTERCON_DFTCLKENB      18            /*  DFT Clock Enable */
#define BITP_AFE_ADCFILTERCON_DACWAVECLKENB  17            /*  DAC Wave Clock Enable */
#define BITP_AFE_ADCFILTERCON_SINC2CLKENB    16            /*  SINC2 Filter Clock Enable */
#define BITP_AFE_ADCFILTERCON_AVRGNUM        14            /*  Number of Samples Averaged */
#define BITP_AFE_ADCFILTERCON_SINC3OSR       12            /*  SINC3 OSR */
#define BITP_AFE_ADCFILTERCON_SINC2OSR        8            /*  SINC2 OSR */
#define BITP_AFE_ADCFILTERCON_AVRGEN          7            /*  Average Function Enable */
#define BITP_AFE_ADCFILTERCON_SINC3BYP        6            /*  SINC3 Filter Bypass */
#define BITP_AFE_ADCFILTERCON_LPFBYPEN        4            /*  50/60Hz Low Pass Filter */
#define BITP_AFE_ADCFILTERCON_ADCCLK          0            /*  ADC Data Rate */
#define BITM_AFE_ADCFILTERCON_DFTCLKENB      0x00040000    /*  DFT Clock Enable */
#define BITM_AFE_ADCFILTERCON_DACWAVECLKENB  0x00020000    /*  DAC Wave Clock Enable */
#define BITM_AFE_ADCFILTERCON_SINC2CLKENB    0x00010000    /*  SINC2 Filter Clock Enable */
#define BITM_AFE_ADCFILTERCON_AVRGNUM        0x0000C000    /*  Number of Samples Averaged */
#define BITM_AFE_ADCFILTERCON_SINC3OSR       0x00003000    /*  SINC3 OSR */
#define BITM_AFE_ADCFILTERCON_SINC2OSR       0x00000F00    /*  SINC2 OSR */
#define BITM_AFE_ADCFILTERCON_AVRGEN         0x00000080    /*  Average Function Enable */
#define BITM_AFE_ADCFILTERCON_SINC3BYP       0x00000040    /*  SINC3 Filter Bypass */
#define BITM_AFE_ADCFILTERCON_LPFBYPEN       0x00000010    /*  50/60Hz Low Pass Filter */
#define BITM_AFE_ADCFILTERCON_ADCCLK         0x00000001    /*  ADC Data Rate */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_HSDACDAT                         Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_HSDACDAT_DACDAT              0            /*  DAC Code */
#define BITM_AFE_HSDACDAT_DACDAT             0x00000FFF    /*  DAC Code */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_LPREFBUFCON                      Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_LPREFBUFCON_BOOSTCURRENT     2            /*  Set: Drive 2 Dac ;Unset Drive 1 Dac, and Save Power */
#define BITP_AFE_LPREFBUFCON_LPBUF2P5DIS      1            /*  Low Power Bandgap's Output Buffer */
#define BITP_AFE_LPREFBUFCON_LPREFDIS         0            /*  Set This Bit Will Power Down Low Power Bandgap */
#define BITM_AFE_LPREFBUFCON_BOOSTCURRENT    0x00000004    /*  Set: Drive 2 Dac ;Unset Drive 1 Dac, and Save Power */
#define BITM_AFE_LPREFBUFCON_LPBUF2P5DIS     0x00000002    /*  Low Power Bandgap's Output Buffer */
#define BITM_AFE_LPREFBUFCON_LPREFDIS        0x00000001    /*  Set This Bit Will Power Down Low Power Bandgap */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_SYNCEXTDEVICE                    Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_SYNCEXTDEVICE_SYNC           0            /*  As Output Data of GPIO */
#define BITM_AFE_SYNCEXTDEVICE_SYNC          0x000000FF    /*  As Output Data of GPIO */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_SEQCRC                           Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_SEQCRC_CRC                   0            /*  Sequencer Command CRC Value. */
#define BITM_AFE_SEQCRC_CRC                  0x000000FF    /*  Sequencer Command CRC Value. */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_SEQCNT                           Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_SEQCNT_COUNT                 0            /*  Sequencer Command Count */
#define BITM_AFE_SEQCNT_COUNT                0x0000FFFF    /*  Sequencer Command Count */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_SEQTIMEOUT                       Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_SEQTIMEOUT_TIMEOUT           0            /*  Current Value of the Sequencer Timeout Counter. */
#define BITM_AFE_SEQTIMEOUT_TIMEOUT          0x3FFFFFFF    /*  Current Value of the Sequencer Timeout Counter. */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_DATAFIFORD                       Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_DATAFIFORD_DATAFIFOOUT       0            /*  Data FIFO Read */
#define BITM_AFE_DATAFIFORD_DATAFIFOOUT      0x0000FFFF    /*  Data FIFO Read */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_CMDFIFOWRITE                     Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_CMDFIFOWRITE_CMDFIFOIN       0            /*  Command FIFO Write. */
#define BITM_AFE_CMDFIFOWRITE_CMDFIFOIN      0xFFFFFFFF    /*  Command FIFO Write. */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_ADCDAT                           Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_ADCDAT_DATA                  0            /*  ADC Result */
#define BITM_AFE_ADCDAT_DATA                 0x0000FFFF    /*  ADC Result */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_DFTREAL                          Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_DFTREAL_DATA                 0            /*  DFT Real */
#define BITM_AFE_DFTREAL_DATA                0x0003FFFF    /*  DFT Real */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_DFTIMAG                          Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_DFTIMAG_DATA                 0            /*  DFT Imaginary */
#define BITM_AFE_DFTIMAG_DATA                0x0003FFFF    /*  DFT Imaginary */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_SINC2DAT                         Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_SINC2DAT_DATA                0            /*  LPF Result */
#define BITM_AFE_SINC2DAT_DATA               0x0000FFFF    /*  LPF Result */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_TEMPSENSDAT                      Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_TEMPSENSDAT_DATA             0            /*  Temp Sensor */
#define BITM_AFE_TEMPSENSDAT_DATA            0x0000FFFF    /*  Temp Sensor */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_AFEGENINTSTA                     Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_AFEGENINTSTA_CUSTOMIRQ3      3            /*  Custom IRQ 3. */
#define BITP_AFE_AFEGENINTSTA_CUSTOMIRQ2      2            /*  Custom IRQ 2 */
#define BITP_AFE_AFEGENINTSTA_CUSTOMIRQ1      1            /*  Custom IRQ 1. */
#define BITP_AFE_AFEGENINTSTA_CUSTOMIRQ0      0            /*  Custom IRQ 0 */
#define BITM_AFE_AFEGENINTSTA_CUSTOMIRQ3     0x00000008    /*  Custom IRQ 3. */
#define BITM_AFE_AFEGENINTSTA_CUSTOMIRQ2     0x00000004    /*  Custom IRQ 2 */
#define BITM_AFE_AFEGENINTSTA_CUSTOMIRQ1     0x00000002    /*  Custom IRQ 1. */
#define BITM_AFE_AFEGENINTSTA_CUSTOMIRQ0     0x00000001    /*  Custom IRQ 0 */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_ADCMIN                           Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_ADCMIN_MINVAL                0            /*  ADC Minimum Value Threshold */
#define BITM_AFE_ADCMIN_MINVAL               0x0000FFFF    /*  ADC Minimum Value Threshold */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_ADCMINSM                         Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_ADCMINSM_MINCLRVAL           0            /*  ADCMIN Hysteresis Value */
#define BITM_AFE_ADCMINSM_MINCLRVAL          0x0000FFFF    /*  ADCMIN Hysteresis Value */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_ADCMAX                           Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_ADCMAX_MAXVAL                0            /*  ADC Max Threshold */
#define BITM_AFE_ADCMAX_MAXVAL               0x0000FFFF    /*  ADC Max Threshold */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_ADCMAXSMEN                       Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_ADCMAXSMEN_MAXSWEN           0            /*  ADCMAX Hysteresis Value */
#define BITM_AFE_ADCMAXSMEN_MAXSWEN          0x0000FFFF    /*  ADCMAX Hysteresis Value */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_ADCDELTA                         Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_ADCDELTA_DELTAVAL            0            /*  ADCDAT Code Differences Limit Option */
#define BITM_AFE_ADCDELTA_DELTAVAL           0x0000FFFF    /*  ADCDAT Code Differences Limit Option */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_HPOSCCON                         Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_HPOSCCON_CLK32MHZEN          2            /*  16M/32M Output Selector Signal. */
#define BITM_AFE_HPOSCCON_CLK32MHZEN         0x00000004    /*  16M/32M Output Selector Signal. */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_DFTCON                           Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_DFTCON_DFTINSEL             20            /*  DFT Input Select */
#define BITP_AFE_DFTCON_DFTNUM                4            /*  ADC Samples Used */
#define BITP_AFE_DFTCON_HANNINGEN             0            /*  Hanning Window Enable */
#define BITM_AFE_DFTCON_DFTINSEL             0x00300000    /*  DFT Input Select */
#define BITM_AFE_DFTCON_DFTNUM               0x000000F0    /*  ADC Samples Used */
#define BITM_AFE_DFTCON_HANNINGEN            0x00000001    /*  Hanning Window Enable */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_LPTIASW0                         Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_LPTIASW0_RECAL              15            /*  TIA SW15 Control. Active High */
#define BITP_AFE_LPTIASW0_VZEROSHARE         14            /*  TIA SW14 Control. Active High */
#define BITP_AFE_LPTIASW0_TIABIASSEL         13            /*  TIA SW13 Control. Active High */
#define BITP_AFE_LPTIASW0_PABIASSEL          12            /*  TIA SW12 Control. Active High */
#define BITP_AFE_LPTIASW0_TIASWCON            0            /*  TIA SW[11:0] Control */
#define BITM_AFE_LPTIASW0_RECAL              0x00008000    /*  TIA SW15 Control. Active High */
#define BITM_AFE_LPTIASW0_VZEROSHARE         0x00004000    /*  TIA SW14 Control. Active High */
#define BITM_AFE_LPTIASW0_TIABIASSEL         0x00002000    /*  TIA SW13 Control. Active High */
#define BITM_AFE_LPTIASW0_PABIASSEL          0x00001000    /*  TIA SW12 Control. Active High */
#define BITM_AFE_LPTIASW0_TIASWCON           0x00000FFF    /*  TIA SW[11:0] Control */
#define ENUM_AFE_LPTIASW0_11                 0x00000014            /*  TIASWCON: CAPA test with LP TIA */
#define ENUM_AFE_LPTIASW0_NORM               0x0000002C            /*  TIASWCON: Normal work mode */
#define ENUM_AFE_LPTIASW0_DIO                0x0000002D            /*  TIASWCON: Normal work mode with back-back diode enabled. */
#define ENUM_AFE_LPTIASW0_SHORTSW            0x0000002E            /*  TIASWCON: Work mode with short switch protection */
#define ENUM_AFE_LPTIASW0_LOWNOISE           0x0000006C            /*  TIASWCON: Work mode, vzero-vbias=0. */
#define ENUM_AFE_LPTIASW0_1                  0x00000094            /*  TIASWCON: CAPA test or Ramp test with HP TIA */
#define ENUM_AFE_LPTIASW0_BUFDIS             0x00000180            /*  TIASWCON: Set PA/TIA as unity gain buffer. */
#define ENUM_AFE_LPTIASW0_BUFEN              0x000001A4            /*  TIASWCON: Set PA/TIA as unity gain buffer. Connect amp's output to CE0 & RC01. */
#define ENUM_AFE_LPTIASW0_TWOLEAD            0x0000042C            /*  TIASWCON: Two lead sensor, set PA as unity gain buffer. */
#define ENUM_AFE_LPTIASW0_BUFEN2             0x000004A4            /*  TIASWCON: Set PA/TIA as unity gain buffer. */
#define ENUM_AFE_LPTIASW0_SESHORTRE          0x00000800            /*  TIASWCON: Close SW11 - Short SE0 to RE0. */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_LPTIACON0                        Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_LPTIACON0_CHOPEN            16            /*  Chopping Enable */
#define BITP_AFE_LPTIACON0_TIARF             13            /*  Set LPF Resistor */
#define BITP_AFE_LPTIACON0_TIARL             10            /*  Set RLOAD */
#define BITP_AFE_LPTIACON0_TIAGAIN            5            /*  Set RTIA */
#define BITP_AFE_LPTIACON0_IBOOST             3            /*  Current Boost Control */
#define BITP_AFE_LPTIACON0_HALFPWR            2            /*  Half Power Mode Select */
#define BITP_AFE_LPTIACON0_PAPDEN             1            /*  PA Power Down */
#define BITP_AFE_LPTIACON0_TIAPDEN            0            /*  TIA Power Down */
#define BITM_AFE_LPTIACON0_CHOPEN            0x00030000    /*  Chopping Enable */
#define BITM_AFE_LPTIACON0_TIARF             0x0000E000    /*  Set LPF Resistor */
#define BITM_AFE_LPTIACON0_TIARL             0x00001C00    /*  Set RLOAD */
#define BITM_AFE_LPTIACON0_TIAGAIN           0x000003E0    /*  Set RTIA */
#define BITM_AFE_LPTIACON0_IBOOST            0x00000018    /*  Current Boost Control */
#define BITM_AFE_LPTIACON0_HALFPWR           0x00000004    /*  Half Power Mode Select */
#define BITM_AFE_LPTIACON0_PAPDEN            0x00000002    /*  PA Power Down */
#define BITM_AFE_LPTIACON0_TIAPDEN           0x00000001    /*  TIA Power Down */
#define ENUM_AFE_LPTIACON0_DISCONRF          0x00000000            /*  TIARF: Disconnect TIA output from LPF pin */
#define ENUM_AFE_LPTIACON0_BYPRF             0x00002000            /*  TIARF: Bypass resistor */
#define ENUM_AFE_LPTIACON0_RF20K             0x00004000            /*  TIARF: 20k Ohm */
#define ENUM_AFE_LPTIACON0_RF100K            0x00006000            /*  TIARF: 100k Ohm */
#define ENUM_AFE_LPTIACON0_RF200K            0x00008000            /*  TIARF: 200k Ohm */
#define ENUM_AFE_LPTIACON0_RF400K            0x0000A000            /*  TIARF: 400k Ohm */
#define ENUM_AFE_LPTIACON0_RF600K            0x0000C000            /*  TIARF: 600k Ohm */
#define ENUM_AFE_LPTIACON0_RF1MOHM           0x0000E000            /*  TIARF: 1Meg Ohm */
#define ENUM_AFE_LPTIACON0_RL0               0x00000000            /*  TIARL: 0 ohm */
#define ENUM_AFE_LPTIACON0_RL10              0x00000400            /*  TIARL: 10 ohm */
#define ENUM_AFE_LPTIACON0_RL30              0x00000800            /*  TIARL: 30 ohm */
#define ENUM_AFE_LPTIACON0_RL50              0x00000C00            /*  TIARL: 50 ohm */
#define ENUM_AFE_LPTIACON0_RL100             0x00001000            /*  TIARL: 100 ohm */
#define ENUM_AFE_LPTIACON0_RL1P6K            0x00001400            /*  TIARL: 1.6kohm */
#define ENUM_AFE_LPTIACON0_RL3P1K            0x00001800            /*  TIARL: 3.1kohm */
#define ENUM_AFE_LPTIACON0_RL3P5K            0x00001C00            /*  TIARL: 3.6kohm */
#define ENUM_AFE_LPTIACON0_DISCONTIA         0x00000000            /*  TIAGAIN: Disconnect TIA Gain resistor */
#define ENUM_AFE_LPTIACON0_TIAGAIN200        0x00000020            /*  TIAGAIN: 200 Ohm */
#define ENUM_AFE_LPTIACON0_TIAGAIN1K         0x00000040            /*  TIAGAIN: 1k ohm */
#define ENUM_AFE_LPTIACON0_TIAGAIN2K         0x00000060            /*  TIAGAIN: 2k */
#define ENUM_AFE_LPTIACON0_TIAGAIN3K         0x00000080            /*  TIAGAIN: 3k */
#define ENUM_AFE_LPTIACON0_TIAGAIN4K         0x000000A0            /*  TIAGAIN: 4k */
#define ENUM_AFE_LPTIACON0_TIAGAIN6K         0x000000C0            /*  TIAGAIN: 6k */
#define ENUM_AFE_LPTIACON0_TIAGAIN8K         0x000000E0            /*  TIAGAIN: 8k */
#define ENUM_AFE_LPTIACON0_TIAGAIN10K        0x00000100            /*  TIAGAIN: 10k */
#define ENUM_AFE_LPTIACON0_TIAGAIN12K        0x00000120            /*  TIAGAIN: 12k */
#define ENUM_AFE_LPTIACON0_TIAGAIN16K        0x00000140            /*  TIAGAIN: 16k */
#define ENUM_AFE_LPTIACON0_TIAGAIN20K        0x00000160            /*  TIAGAIN: 20k */
#define ENUM_AFE_LPTIACON0_TIAGAIN24K        0x00000180            /*  TIAGAIN: 24k */
#define ENUM_AFE_LPTIACON0_TIAGAIN30K        0x000001A0            /*  TIAGAIN: 30k */
#define ENUM_AFE_LPTIACON0_TIAGAIN32K        0x000001C0            /*  TIAGAIN: 32k */
#define ENUM_AFE_LPTIACON0_TIAGAIN40K        0x000001E0            /*  TIAGAIN: 40k */
#define ENUM_AFE_LPTIACON0_TIAGAIN48K        0x00000200            /*  TIAGAIN: 48k */
#define ENUM_AFE_LPTIACON0_TIAGAIN64K        0x00000220            /*  TIAGAIN: 64k */
#define ENUM_AFE_LPTIACON0_TIAGAIN85K        0x00000240            /*  TIAGAIN: 85k */
#define ENUM_AFE_LPTIACON0_TIAGAIN96K        0x00000260            /*  TIAGAIN: 96k */
#define ENUM_AFE_LPTIACON0_TIAGAIN100K       0x00000280            /*  TIAGAIN: 100k */
#define ENUM_AFE_LPTIACON0_TIAGAIN120K       0x000002A0            /*  TIAGAIN: 120k */
#define ENUM_AFE_LPTIACON0_TIAGAIN128K       0x000002C0            /*  TIAGAIN: 128k */
#define ENUM_AFE_LPTIACON0_TIAGAIN160K       0x000002E0            /*  TIAGAIN: 160k */
#define ENUM_AFE_LPTIACON0_TIAGAIN196K       0x00000300            /*  TIAGAIN: 196k */
#define ENUM_AFE_LPTIACON0_TIAGAIN256K       0x00000320            /*  TIAGAIN: 256k */
#define ENUM_AFE_LPTIACON0_TIAGAIN512K       0x00000340            /*  TIAGAIN: 512k */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_HSRTIACON                        Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_HSRTIACON_CTIACON            5            /*  Configure Capacitor in Parallel with RTIA */
#define BITP_AFE_HSRTIACON_TIASW6CON          4            /*  SW6 Control */
#define BITP_AFE_HSRTIACON_RTIACON            0            /*  Configure General RTIA Value */
#define BITM_AFE_HSRTIACON_CTIACON           0x00001FE0    /*  Configure Capacitor in Parallel with RTIA */
#define BITM_AFE_HSRTIACON_TIASW6CON         0x00000010    /*  SW6 Control */
#define BITM_AFE_HSRTIACON_RTIACON           0x0000000F    /*  Configure General RTIA Value */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_DE0RESCON                        Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_DE0RESCON_DE0RCON            0            /*  DE0 RLOAD RTIA Setting */
#define BITM_AFE_DE0RESCON_DE0RCON           0x000000FF    /*  DE0 RLOAD RTIA Setting */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_HSTIACON                         Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_HSTIACON_VBIASSEL            0            /*  Select HSTIA Positive Input */
#define BITM_AFE_HSTIACON_VBIASSEL           0x00000003    /*  Select HSTIA Positive Input */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_LPMODEKEY                       Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_LPMODEKEY_KEY               0            /*  LP Key */
#define BITM_AFE_LPMODEKEY_KEY              0x000FFFFF    /*  LP Key */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_LPMODECLKSEL                    Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_LPMODECLKSEL_LFSYSCLKEN     0            /*  Enable Switching System Clock to 32KHz by Sequencer */
#define BITM_AFE_LPMODECLKSEL_LFSYSCLKEN    0x00000001    /*  Enable Switching System Clock to 32KHz by Sequencer */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_LPMODECON                       Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_LPMODECON_ALDOEN            8            /*  Set High to Power Down of Analog LDO */
#define BITP_AFE_LPMODECON_V1P1HPADCEN       7            /*  Set High to Enable 1.1V HP CM Buffer */
#define BITP_AFE_LPMODECON_V1P8HPADCEN       6            /*  Set High to Enable HP 1.8V Reference Buffer */
#define BITP_AFE_LPMODECON_PTATEN            5            /*  Set to High to Generate Ptat Current Bias */
#define BITP_AFE_LPMODECON_ZTATEN            4            /*  Set High to Generate Ztat Current Bias */
#define BITP_AFE_LPMODECON_REPEATADCCNVEN_P  3            /*  Set High to Enable Repeat ADC Conversion */
#define BITP_AFE_LPMODECON_ADCCONVEN         2            /*  Set High to Enable ADC Conversion */
#define BITP_AFE_LPMODECON_HPREFDIS          1            /*  Set High to Power Down HP Reference */
#define BITP_AFE_LPMODECON_HFOSCPD           0            /*  Set High to Power Down HP Power Oscillator */
#define BITM_AFE_LPMODECON_ALDOEN           0x00000100    /*  Set High to Power Down of Analog LDO */
#define BITM_AFE_LPMODECON_V1P1HPADCEN      0x00000080    /*  Set High to Enable 1.1V HP CM Buffer */
#define BITM_AFE_LPMODECON_V1P8HPADCEN      0x00000040    /*  Set High to Enable HP 1.8V Reference Buffer */
#define BITM_AFE_LPMODECON_PTATEN           0x00000020    /*  Set to High to Generate Ptat Current Bias */
#define BITM_AFE_LPMODECON_ZTATEN           0x00000010    /*  Set High to Generate Ztat Current Bias */
#define BITM_AFE_LPMODECON_REPEATADCCNVEN_P 0x00000008    /*  Set High to Enable Repeat ADC Conversion */
#define BITM_AFE_LPMODECON_ADCCONVEN        0x00000004    /*  Set High to Enable ADC Conversion */
#define BITM_AFE_LPMODECON_HPREFDIS         0x00000002    /*  Set High to Power Down HP Reference */
#define BITM_AFE_LPMODECON_HFOSCPD          0x00000001    /*  Set High to Power Down HP Power Oscillator */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_SEQSLPLOCK                       Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_SEQSLPLOCK_SEQ_SLP_PW        0            /*  Password for SLPBYSEQ Register */
#define BITM_AFE_SEQSLPLOCK_SEQ_SLP_PW       0x000FFFFF    /*  Password for SLPBYSEQ Register */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_SEQTRGSLP                        Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_SEQTRGSLP_TRGSLP             0            /*  Trigger Sleep by Sequencer */
#define BITM_AFE_SEQTRGSLP_TRGSLP            0x00000001    /*  Trigger Sleep by Sequencer */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_LPDACDAT0                        Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_LPDACDAT0_DACIN6            12            /*  6BITVAL, 1LSB=34.375mV */
#define BITP_AFE_LPDACDAT0_DACIN12            0            /*  12BITVAL, 1LSB=537uV */
#define BITM_AFE_LPDACDAT0_DACIN6            0x0003F000    /*  6BITVAL, 1LSB=34.375mV */
#define BITM_AFE_LPDACDAT0_DACIN12           0x00000FFF    /*  12BITVAL, 1LSB=537uV */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_LPDACSW0                         Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_LPDACSW0_LPMODEDIS           5            /*  Switch Control */
#define BITP_AFE_LPDACSW0_LPDACSW             0            /*  LPDAC0 Switches Matrix */
#define BITM_AFE_LPDACSW0_LPMODEDIS          0x00000020    /*  Switch Control */
#define BITM_AFE_LPDACSW0_LPDACSW            0x0000001F    /*  LPDAC0 Switches Matrix */
#define ENUM_AFE_LPDACSW0_DACCONBIT5         0x00000000            /*  LPMODEDIS: REG_AFE_LPDACDAT0 Switch controlled by REG_AFE_LPDACDAT0CON0 bit 5 */
#define ENUM_AFE_LPDACSW0_OVRRIDE            0x00000020            /*  LPMODEDIS: REG_AFE_LPDACDAT0 Switches override */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_LPDACCON0                        Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_LPDACCON0_WAVETYPE           6            /*  LPDAC Data Source */
#define BITP_AFE_LPDACCON0_DACMDE             5            /*  LPDAC0 Switch Settings */
#define BITP_AFE_LPDACCON0_VZEROMUX           4            /*  VZERO MUX Select */
#define BITP_AFE_LPDACCON0_VBIASMUX           3            /*  VBIAS MUX Select */
#define BITP_AFE_LPDACCON0_REFSEL             2            /*  Reference Select Bit */
#define BITP_AFE_LPDACCON0_PWDEN              1            /*  LPDAC0 Power Down */
#define BITP_AFE_LPDACCON0_RSTEN              0            /*  Enable Writes to REG_AFE_LPDACDAT00 */
#define BITM_AFE_LPDACCON0_WAVETYPE          0x00000040    /*  LPDAC Data Source */
#define BITM_AFE_LPDACCON0_DACMDE            0x00000020    /*  LPDAC0 Switch Settings */
#define BITM_AFE_LPDACCON0_VZEROMUX          0x00000010    /*  VZERO MUX Select */
#define BITM_AFE_LPDACCON0_VBIASMUX          0x00000008    /*  VBIAS MUX Select */
#define BITM_AFE_LPDACCON0_REFSEL            0x00000004    /*  Reference Select Bit */
#define BITM_AFE_LPDACCON0_PWDEN             0x00000002    /*  LPDAC0 Power Down */
#define BITM_AFE_LPDACCON0_RSTEN             0x00000001    /*  Enable Writes to REG_AFE_LPDACDAT00 */
#define ENUM_AFE_LPDACCON0_MMR               0x00000000            /*  WAVETYPE: Direct from REG_AFE_LPDACDAT0DAT0 */
#define ENUM_AFE_LPDACCON0_WAVEGEN           0x00000040            /*  WAVETYPE: Waveform generator */
#define ENUM_AFE_LPDACCON0_NORM              0x00000000            /*  DACMDE: REG_AFE_LPDACDAT00 switches set for normal mode */
#define ENUM_AFE_LPDACCON0_DIAG              0x00000020            /*  DACMDE: REG_AFE_LPDACDAT00 switches set for Diagnostic mode */
#define ENUM_AFE_LPDACCON0_BITS6             0x00000000            /*  VZEROMUX: VZERO 6BIT */
#define ENUM_AFE_LPDACCON0_BITS12            0x00000010            /*  VZEROMUX: VZERO 12BIT */
#define ENUM_AFE_LPDACCON0_12BIT             0x00000000            /*  VBIASMUX: Output 12Bit */
#define ENUM_AFE_LPDACCON0_EN                0x00000008            /*  VBIASMUX: output 6Bit */
#define ENUM_AFE_LPDACCON0_ULPREF            0x00000000            /*  REFSEL: ULP2P5V Ref */
#define ENUM_AFE_LPDACCON0_AVDD              0x00000004            /*  REFSEL: AVDD Reference */
#define ENUM_AFE_LPDACCON0_PWREN             0x00000000            /*  PWDEN: REG_AFE_LPDACDAT00 Powered On */
#define ENUM_AFE_LPDACCON0_PWRDIS            0x00000002            /*  PWDEN: REG_AFE_LPDACDAT00 Powered Off */
#define ENUM_AFE_LPDACCON0_WRITEDIS          0x00000000            /*  RSTEN: Disable REG_AFE_LPDACDAT00 Writes */
#define ENUM_AFE_LPDACCON0_WRITEEN           0x00000001            /*  RSTEN: Enable REG_AFE_LPDACDAT00 Writes */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_DSWFULLCON                       Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_DSWFULLCON_D8                7            /*  Control of D8 Switch. */
#define BITP_AFE_DSWFULLCON_D7                6            /*  Control of D7 Switch. */
#define BITP_AFE_DSWFULLCON_D6                5            /*  Control of D6 Switch. */
#define BITP_AFE_DSWFULLCON_D5                4            /*  Control of D5 Switch. */
#define BITP_AFE_DSWFULLCON_D4                3            /*  Control of D4 Switch. */
#define BITP_AFE_DSWFULLCON_D3                2            /*  Control of D3 Switch. */
#define BITP_AFE_DSWFULLCON_D2                1            /*  Control of D2 Switch. */
#define BITP_AFE_DSWFULLCON_DR0               0            /*  Control of Dr0 Switch. */
#define BITM_AFE_DSWFULLCON_D8               0x00000080    /*  Control of D8 Switch. */
#define BITM_AFE_DSWFULLCON_D7               0x00000040    /*  Control of D7 Switch. */
#define BITM_AFE_DSWFULLCON_D6               0x00000020    /*  Control of D6 Switch. */
#define BITM_AFE_DSWFULLCON_D5               0x00000010    /*  Control of D5 Switch. */
#define BITM_AFE_DSWFULLCON_D4               0x00000008    /*  Control of D4 Switch. */
#define BITM_AFE_DSWFULLCON_D3               0x00000004    /*  Control of D3 Switch. */
#define BITM_AFE_DSWFULLCON_D2               0x00000002    /*  Control of D2 Switch. */
#define BITM_AFE_DSWFULLCON_DR0              0x00000001    /*  Control of Dr0 Switch. */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_NSWFULLCON                       Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_NSWFULLCON_NL2              11            /*  Control of NL2 Switch. */
#define BITP_AFE_NSWFULLCON_NL               10            /*  Control of NL Switch. */
#define BITP_AFE_NSWFULLCON_NR1               9            /*  Control of Nr1 Switch. Set Will Close Nr1, Unset Open */
#define BITP_AFE_NSWFULLCON_N9                8            /*  Control of N9 Switch. Set Will Close N9, Unset Open */
#define BITP_AFE_NSWFULLCON_N8                7            /*  Control of N8 Switch. Set Will Close N8, Unset Open */
#define BITP_AFE_NSWFULLCON_N7                6            /*  Control of N7 Switch. Set Will Close N7, Unset Open */
#define BITP_AFE_NSWFULLCON_N6                5            /*  Control of N6 Switch. Set Will Close N6, Unset Open */
#define BITP_AFE_NSWFULLCON_N5                4            /*  Control of N5 Switch. Set Will Close N5, Unset Open */
#define BITP_AFE_NSWFULLCON_N4                3            /*  Control of N4 Switch. Set Will Close N4, Unset Open */
#define BITP_AFE_NSWFULLCON_N3                2            /*  Control of N3 Switch. Set Will Close N3, Unset Open */
#define BITP_AFE_NSWFULLCON_N2                1            /*  Control of N2 Switch. Set Will Close N2, Unset Open */
#define BITP_AFE_NSWFULLCON_N1                0            /*  Control of N1 Switch. Set Will Close N1, Unset Open */
#define BITM_AFE_NSWFULLCON_NL2              0x00000800    /*  Control of NL2 Switch. */
#define BITM_AFE_NSWFULLCON_NL               0x00000400    /*  Control of NL Switch. */
#define BITM_AFE_NSWFULLCON_NR1              0x00000200    /*  Control of Nr1 Switch. Set Will Close Nr1, Unset Open */
#define BITM_AFE_NSWFULLCON_N9               0x00000100    /*  Control of N9 Switch. Set Will Close N9, Unset Open */
#define BITM_AFE_NSWFULLCON_N8               0x00000080    /*  Control of N8 Switch. Set Will Close N8, Unset Open */
#define BITM_AFE_NSWFULLCON_N7               0x00000040    /*  Control of N7 Switch. Set Will Close N7, Unset Open */
#define BITM_AFE_NSWFULLCON_N6               0x00000020    /*  Control of N6 Switch. Set Will Close N6, Unset Open */
#define BITM_AFE_NSWFULLCON_N5               0x00000010    /*  Control of N5 Switch. Set Will Close N5, Unset Open */
#define BITM_AFE_NSWFULLCON_N4               0x00000008    /*  Control of N4 Switch. Set Will Close N4, Unset Open */
#define BITM_AFE_NSWFULLCON_N3               0x00000004    /*  Control of N3 Switch. Set Will Close N3, Unset Open */
#define BITM_AFE_NSWFULLCON_N2               0x00000002    /*  Control of N2 Switch. Set Will Close N2, Unset Open */
#define BITM_AFE_NSWFULLCON_N1               0x00000001    /*  Control of N1 Switch. Set Will Close N1, Unset Open */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_PSWFULLCON                       Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_PSWFULLCON_PL2              14            /*  PL2 Switch Control */
#define BITP_AFE_PSWFULLCON_PL               13            /*  PL Switch Control */
#define BITP_AFE_PSWFULLCON_P12              11            /*  Control of P12 Switch. Set Will Close P12, Unset Open */
#define BITP_AFE_PSWFULLCON_P11              10            /*  Control of P11 Switch. Set Will Close P11, Unset Open */
#define BITP_AFE_PSWFULLCON_P10               9            /*  P10 Switch Control */
#define BITP_AFE_PSWFULLCON_P9                8            /*  Control of P9 Switch. Set Will Close P9, Unset Open */
#define BITP_AFE_PSWFULLCON_P8                7            /*  Control of P8 Switch. Set Will Close P8, Unset Open */
#define BITP_AFE_PSWFULLCON_P7                6            /*  Control of P7 Switch. Set Will Close P7, Unset Open */
#define BITP_AFE_PSWFULLCON_P6                5            /*  Control of P6 Switch. Set Will Close P6, Unset Open */
#define BITP_AFE_PSWFULLCON_P5                4            /*  Control of P5 Switch. Set Will Close P5, Unset Open */
#define BITP_AFE_PSWFULLCON_P4                3            /*  Control of P4 Switch. Set Will Close P4, Unset Open */
#define BITP_AFE_PSWFULLCON_P3                2            /*  Control of P3 Switch. Set Will Close P3, Unset Open */
#define BITP_AFE_PSWFULLCON_P2                1            /*  Control of P2 Switch. Set Will Close P2, Unset Open */
#define BITP_AFE_PSWFULLCON_PR0               0            /*  PR0 Switch Control */
#define BITM_AFE_PSWFULLCON_PL2              0x00004000    /*  PL2 Switch Control */
#define BITM_AFE_PSWFULLCON_PL               0x00002000    /*  PL Switch Control */
#define BITM_AFE_PSWFULLCON_P12              0x00000800    /*  Control of P12 Switch. Set Will Close P12, Unset Open */
#define BITM_AFE_PSWFULLCON_P11              0x00000400    /*  Control of P11 Switch. Set Will Close P11, Unset Open */
#define BITM_AFE_PSWFULLCON_P10              0x00000200    /*  P10 Switch Control */
#define BITM_AFE_PSWFULLCON_P9               0x00000100    /*  Control of P9 Switch. Set Will Close P9, Unset Open */
#define BITM_AFE_PSWFULLCON_P8               0x00000080    /*  Control of P8 Switch. Set Will Close P8, Unset Open */
#define BITM_AFE_PSWFULLCON_P7               0x00000040    /*  Control of P7 Switch. Set Will Close P7, Unset Open */
#define BITM_AFE_PSWFULLCON_P6               0x00000020    /*  Control of P6 Switch. Set Will Close P6, Unset Open */
#define BITM_AFE_PSWFULLCON_P5               0x00000010    /*  Control of P5 Switch. Set Will Close P5, Unset Open */
#define BITM_AFE_PSWFULLCON_P4               0x00000008    /*  Control of P4 Switch. Set Will Close P4, Unset Open */
#define BITM_AFE_PSWFULLCON_P3               0x00000004    /*  Control of P3 Switch. Set Will Close P3, Unset Open */
#define BITM_AFE_PSWFULLCON_P2               0x00000002    /*  Control of P2 Switch. Set Will Close P2, Unset Open */
#define BITM_AFE_PSWFULLCON_PR0              0x00000001    /*  PR0 Switch Control */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_TSWFULLCON                       Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_TSWFULLCON_TR1              11            /*  Control of Tr1 Switch. Set Will Close Tr1, Unset Open */
#define BITP_AFE_TSWFULLCON_T11              10            /*  Control of T11 Switch. Set Will Close T11, Unset Open */
#define BITP_AFE_TSWFULLCON_T10               9            /*  Control of T10 Switch. Set Will Close T10, Unset Open */
#define BITP_AFE_TSWFULLCON_T9                8            /*  Control of T9 Switch. Set Will Close T9, Unset Open */
#define BITP_AFE_TSWFULLCON_T7                6            /*  Control of T7 Switch. Set Will Close T7, Unset Open */
#define BITP_AFE_TSWFULLCON_T5                4            /*  Control of T5 Switch. Set Will Close T5, Unset Open */
#define BITP_AFE_TSWFULLCON_T4                3            /*  Control of T4 Switch. Set Will Close T4, Unset Open */
#define BITP_AFE_TSWFULLCON_T3                2            /*  Control of T3 Switch. Set Will Close T3, Unset Open */
#define BITP_AFE_TSWFULLCON_T2                1            /*  Control of T2 Switch. Set Will Close T2, Unset Open */
#define BITP_AFE_TSWFULLCON_T1                0            /*  Control of T1 Switch. Set Will Close T1, Unset Open */
#define BITM_AFE_TSWFULLCON_TR1              0x00000800    /*  Control of Tr1 Switch. Set Will Close Tr1, Unset Open */
#define BITM_AFE_TSWFULLCON_T11              0x00000400    /*  Control of T11 Switch. Set Will Close T11, Unset Open */
#define BITM_AFE_TSWFULLCON_T10              0x00000200    /*  Control of T10 Switch. Set Will Close T10, Unset Open */
#define BITM_AFE_TSWFULLCON_T9               0x00000100    /*  Control of T9 Switch. Set Will Close T9, Unset Open */
#define BITM_AFE_TSWFULLCON_T7               0x00000040    /*  Control of T7 Switch. Set Will Close T7, Unset Open */
#define BITM_AFE_TSWFULLCON_T5               0x00000010    /*  Control of T5 Switch. Set Will Close T5, Unset Open */
#define BITM_AFE_TSWFULLCON_T4               0x00000008    /*  Control of T4 Switch. Set Will Close T4, Unset Open */
#define BITM_AFE_TSWFULLCON_T3               0x00000004    /*  Control of T3 Switch. Set Will Close T3, Unset Open */
#define BITM_AFE_TSWFULLCON_T2               0x00000002    /*  Control of T2 Switch. Set Will Close T2, Unset Open */
#define BITM_AFE_TSWFULLCON_T1               0x00000001    /*  Control of T1 Switch. Set Will Close T1, Unset Open */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_TEMPSENS                         Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_TEMPSENS_CHOPFRESEL          2            /*  Chop Mode Frequency Setting */
#define BITP_AFE_TEMPSENS_CHOPCON             1            /*  Temp Sensor Chop Mode */
#define BITP_AFE_TEMPSENS_ENABLE              0            /*  Unused */
#define BITM_AFE_TEMPSENS_CHOPFRESEL         0x0000000C    /*  Chop Mode Frequency Setting */
#define BITM_AFE_TEMPSENS_CHOPCON            0x00000002    /*  Temp Sensor Chop Mode */
#define BITM_AFE_TEMPSENS_ENABLE             0x00000001    /*  Unused */
#define ENUM_AFE_TEMPSENS_DIS                0x00000000            /*  CHOPCON: Disable chop */
#define ENUM_AFE_TEMPSENS_EN                 0x00000002            /*  CHOPCON: Enable chop */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_BUFSENCON                        Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_BUFSENCON_V1P8THERMSTEN      8            /*  Buffered Reference Output */
#define BITP_AFE_BUFSENCON_V1P1LPADCCHGDIS    6            /*  Controls Decoupling Cap Discharge Switch */
#define BITP_AFE_BUFSENCON_V1P1LPADCEN        5            /*  ADC 1.1V LP Buffer */
#define BITP_AFE_BUFSENCON_V1P1HPADCEN        4            /*  Enable 1.1V HP CM Buffer */
#define BITP_AFE_BUFSENCON_V1P8HPADCCHGDIS    3            /*  Controls Decoupling Cap Discharge Switch */
#define BITP_AFE_BUFSENCON_V1P8LPADCEN        2            /*  ADC 1.8V LP Reference Buffer */
#define BITP_AFE_BUFSENCON_V1P8HPADCILIMITEN  1            /*  HP ADC Input Current Limit */
#define BITP_AFE_BUFSENCON_V1P8HPADCEN        0            /*  HP 1.8V Reference Buffer */
#define BITM_AFE_BUFSENCON_V1P8THERMSTEN     0x00000100    /*  Buffered Reference Output */
#define BITM_AFE_BUFSENCON_V1P1LPADCCHGDIS   0x00000040    /*  Controls Decoupling Cap Discharge Switch */
#define BITM_AFE_BUFSENCON_V1P1LPADCEN       0x00000020    /*  ADC 1.1V LP Buffer */
#define BITM_AFE_BUFSENCON_V1P1HPADCEN       0x00000010    /*  Enable 1.1V HP CM Buffer */
#define BITM_AFE_BUFSENCON_V1P8HPADCCHGDIS   0x00000008    /*  Controls Decoupling Cap Discharge Switch */
#define BITM_AFE_BUFSENCON_V1P8LPADCEN       0x00000004    /*  ADC 1.8V LP Reference Buffer */
#define BITM_AFE_BUFSENCON_V1P8HPADCILIMITEN 0x00000002    /*  HP ADC Input Current Limit */
#define BITM_AFE_BUFSENCON_V1P8HPADCEN       0x00000001    /*  HP 1.8V Reference Buffer */
#define ENUM_AFE_BUFSENCON_DIS               0x00000000            /*  V1P8THERMSTEN: Disable 1.8V Buffered Reference output */
#define ENUM_AFE_BUFSENCON_EN                0x00000100            /*  V1P8THERMSTEN: Enable 1.8V Buffered Reference output */
#define ENUM_AFE_BUFSENCON_ENCHRG            0x00000000            /*  V1P1LPADCCHGDIS: Open switch */
#define ENUM_AFE_BUFSENCON_DISCHRG           0x00000040            /*  V1P1LPADCCHGDIS: Close Switch */
#define ENUM_AFE_BUFSENCON_DISABLE           0x00000000            /*  V1P1LPADCEN: Disable ADC 1.8V LP Reference Buffer */
#define ENUM_AFE_BUFSENCON_ENABLE            0x00000020            /*  V1P1LPADCEN: Enable ADC 1.8V LP Reference Buffer */
#define ENUM_AFE_BUFSENCON_OFF               0x00000000            /*  V1P1HPADCEN: Disable 1.1V HP Common Mode Buffer */
#define ENUM_AFE_BUFSENCON_ON                0x00000010            /*  V1P1HPADCEN: Enable 1.1V HP Common Mode Buffer */
#define ENUM_AFE_BUFSENCON_OPEN              0x00000000            /*  V1P8HPADCCHGDIS: Open switch */
#define ENUM_AFE_BUFSENCON_CLOSED            0x00000008            /*  V1P8HPADCCHGDIS: Close Switch */
#define ENUM_AFE_BUFSENCON_LPADCREF_DIS      0x00000000            /*  V1P8LPADCEN: Disable LP 1.8V Reference Buffer */
#define ENUM_AFE_BUFSENCON_LPADCREF_EN       0x00000004            /*  V1P8LPADCEN: Enable LP 1.8V Reference Buffer */
#define ENUM_AFE_BUFSENCON_LIMIT_DIS         0x00000000            /*  V1P8HPADCILIMITEN: Disable buffer Current Limit */
#define ENUM_AFE_BUFSENCON_LIMIT_EN          0x00000002            /*  V1P8HPADCILIMITEN: Enable buffer Current Limit */
#define ENUM_AFE_BUFSENCON_HPBUF_DIS         0x00000000            /*  V1P8HPADCEN: Disable 1.8V HP ADC Reference Buffer */
#define ENUM_AFE_BUFSENCON_HPBUF_EN          0x00000001            /*  V1P8HPADCEN: Enable 1.8V HP ADC Reference Buffer */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_ADCCON                           Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_ADCCON_GNPGA                16            /*  PGA Gain Setup */
#define BITP_AFE_ADCCON_GNOFSELPGA           15            /*  Internal Offset/Gain Cancellation */
#define BITP_AFE_ADCCON_GNOFFSEL             13            /*  Obsolete */
#define BITP_AFE_ADCCON_MUXSELN               8            /*  Select Negative Input */
#define BITP_AFE_ADCCON_MUXSELP               0            /*  Select Positive Input */
#define BITM_AFE_ADCCON_GNPGA                0x00070000    /*  PGA Gain Setup */
#define BITM_AFE_ADCCON_GNOFSELPGA           0x00008000    /*  Internal Offset/Gain Cancellation */
#define BITM_AFE_ADCCON_GNOFFSEL             0x00006000    /*  Obsolete */
#define BITM_AFE_ADCCON_MUXSELN              0x00001F00    /*  Select Negative Input */
#define BITM_AFE_ADCCON_MUXSELP              0x0000003F    /*  Select Positive Input */
#define ENUM_AFE_ADCCON_RESERVED             0x00000011            /*  MUXSELP: Reserved */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_DSWSTA                           Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_DSWSTA_D8STA                 7            /*  Status of D8 Switch. */
#define BITP_AFE_DSWSTA_D7STA                 6            /*  Status of D7 Switch. */
#define BITP_AFE_DSWSTA_D6STA                 5            /*  Status of D6 Switch. */
#define BITP_AFE_DSWSTA_D5STA                 4            /*  Status of D5 Switch. */
#define BITP_AFE_DSWSTA_D4STA                 3            /*  Status of D4 Switch. */
#define BITP_AFE_DSWSTA_D3STA                 2            /*  Status of D3 Switch. */
#define BITP_AFE_DSWSTA_D2STA                 1            /*  Status of D2 Switch. */
#define BITP_AFE_DSWSTA_D1STA                 0            /*  Status of Dr0 Switch. */
#define BITM_AFE_DSWSTA_D8STA                0x00000080    /*  Status of D8 Switch. */
#define BITM_AFE_DSWSTA_D7STA                0x00000040    /*  Status of D7 Switch. */
#define BITM_AFE_DSWSTA_D6STA                0x00000020    /*  Status of D6 Switch. */
#define BITM_AFE_DSWSTA_D5STA                0x00000010    /*  Status of D5 Switch. */
#define BITM_AFE_DSWSTA_D4STA                0x00000008    /*  Status of D4 Switch. */
#define BITM_AFE_DSWSTA_D3STA                0x00000004    /*  Status of D3 Switch. */
#define BITM_AFE_DSWSTA_D2STA                0x00000002    /*  Status of D2 Switch. */
#define BITM_AFE_DSWSTA_D1STA                0x00000001    /*  Status of Dr0 Switch. */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_PSWSTA                           Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_PSWSTA_PL2STA               14            /*  PL Switch Control */
#define BITP_AFE_PSWSTA_PLSTA                13            /*  PL Switch Control */
#define BITP_AFE_PSWSTA_P13STA               12            /*  Status of P13 Switch. */
#define BITP_AFE_PSWSTA_P12STA               11            /*  Status of P12 Switch. */
#define BITP_AFE_PSWSTA_P11STA               10            /*  Status of P11 Switch. */
#define BITP_AFE_PSWSTA_P10STA                9            /*  Status of P10 Switch. */
#define BITP_AFE_PSWSTA_P9STA                 8            /*  Status of P9 Switch. */
#define BITP_AFE_PSWSTA_P8STA                 7            /*  Status of P8 Switch. */
#define BITP_AFE_PSWSTA_P7STA                 6            /*  Status of P7 Switch. */
#define BITP_AFE_PSWSTA_P6STA                 5            /*  Status of P6 Switch. */
#define BITP_AFE_PSWSTA_P5STA                 4            /*  Status of P5 Switch. */
#define BITP_AFE_PSWSTA_P4STA                 3            /*  Status of P4 Switch. */
#define BITP_AFE_PSWSTA_P3STA                 2            /*  Status of P3 Switch. */
#define BITP_AFE_PSWSTA_P2STA                 1            /*  Status of P2 Switch. */
#define BITP_AFE_PSWSTA_PR0STA                0            /*  PR0 Switch Control */
#define BITM_AFE_PSWSTA_PL2STA               0x00004000    /*  PL Switch Control */
#define BITM_AFE_PSWSTA_PLSTA                0x00002000    /*  PL Switch Control */
#define BITM_AFE_PSWSTA_P13STA               0x00001000    /*  Status of P13 Switch. */
#define BITM_AFE_PSWSTA_P12STA               0x00000800    /*  Status of P12 Switch. */
#define BITM_AFE_PSWSTA_P11STA               0x00000400    /*  Status of P11 Switch. */
#define BITM_AFE_PSWSTA_P10STA               0x00000200    /*  Status of P10 Switch. */
#define BITM_AFE_PSWSTA_P9STA                0x00000100    /*  Status of P9 Switch. */
#define BITM_AFE_PSWSTA_P8STA                0x00000080    /*  Status of P8 Switch. */
#define BITM_AFE_PSWSTA_P7STA                0x00000040    /*  Status of P7 Switch. */
#define BITM_AFE_PSWSTA_P6STA                0x00000020    /*  Status of P6 Switch. */
#define BITM_AFE_PSWSTA_P5STA                0x00000010    /*  Status of P5 Switch. */
#define BITM_AFE_PSWSTA_P4STA                0x00000008    /*  Status of P4 Switch. */
#define BITM_AFE_PSWSTA_P3STA                0x00000004    /*  Status of P3 Switch. */
#define BITM_AFE_PSWSTA_P2STA                0x00000002    /*  Status of P2 Switch. */
#define BITM_AFE_PSWSTA_PR0STA               0x00000001    /*  PR0 Switch Control */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_NSWSTA                           Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_NSWSTA_NL2STA               11            /*  Status of NL2 Switch. */
#define BITP_AFE_NSWSTA_NLSTA                10            /*  Status of NL Switch. */
#define BITP_AFE_NSWSTA_NR1STA                9            /*  Status of NR1 Switch. */
#define BITP_AFE_NSWSTA_N9STA                 8            /*  Status of N9 Switch. */
#define BITP_AFE_NSWSTA_N8STA                 7            /*  Status of N8 Switch. */
#define BITP_AFE_NSWSTA_N7STA                 6            /*  Status of N7 Switch. */
#define BITP_AFE_NSWSTA_N6STA                 5            /*  Status of N6 Switch. */
#define BITP_AFE_NSWSTA_N5STA                 4            /*  Status of N5 Switch. */
#define BITP_AFE_NSWSTA_N4STA                 3            /*  Status of N4 Switch. */
#define BITP_AFE_NSWSTA_N3STA                 2            /*  Status of N3 Switch. */
#define BITP_AFE_NSWSTA_N2STA                 1            /*  Status of N2 Switch. */
#define BITP_AFE_NSWSTA_N1STA                 0            /*  Status of N1 Switch. */
#define BITM_AFE_NSWSTA_NL2STA               0x00000800    /*  Status of NL2 Switch. */
#define BITM_AFE_NSWSTA_NLSTA                0x00000400    /*  Status of NL Switch. */
#define BITM_AFE_NSWSTA_NR1STA               0x00000200    /*  Status of NR1 Switch. */
#define BITM_AFE_NSWSTA_N9STA                0x00000100    /*  Status of N9 Switch. */
#define BITM_AFE_NSWSTA_N8STA                0x00000080    /*  Status of N8 Switch. */
#define BITM_AFE_NSWSTA_N7STA                0x00000040    /*  Status of N7 Switch. */
#define BITM_AFE_NSWSTA_N6STA                0x00000020    /*  Status of N6 Switch. */
#define BITM_AFE_NSWSTA_N5STA                0x00000010    /*  Status of N5 Switch. */
#define BITM_AFE_NSWSTA_N4STA                0x00000008    /*  Status of N4 Switch. */
#define BITM_AFE_NSWSTA_N3STA                0x00000004    /*  Status of N3 Switch. */
#define BITM_AFE_NSWSTA_N2STA                0x00000002    /*  Status of N2 Switch. */
#define BITM_AFE_NSWSTA_N1STA                0x00000001    /*  Status of N1 Switch. */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_TSWSTA                           Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_TSWSTA_TR1STA               11            /*  Status of TR1 Switch. */
#define BITP_AFE_TSWSTA_T11STA               10            /*  Status of T11 Switch. */
#define BITP_AFE_TSWSTA_T10STA                9            /*  Status of T10 Switch. */
#define BITP_AFE_TSWSTA_T9STA                 8            /*  Status of T9 Switch. */
#define BITP_AFE_TSWSTA_T8STA                 7            /*  Status of T8 Switch. */
#define BITP_AFE_TSWSTA_T7STA                 6            /*  Status of T7 Switch. */
#define BITP_AFE_TSWSTA_T6STA                 5            /*  Status of T6 Switch. */
#define BITP_AFE_TSWSTA_T5STA                 4            /*  Status of T5 Switch. */
#define BITP_AFE_TSWSTA_T4STA                 3            /*  Status of T4 Switch. */
#define BITP_AFE_TSWSTA_T3STA                 2            /*  Status of T3 Switch. */
#define BITP_AFE_TSWSTA_T2STA                 1            /*  Status of T2 Switch. */
#define BITP_AFE_TSWSTA_T1STA                 0            /*  Status of T1 Switch. */
#define BITM_AFE_TSWSTA_TR1STA               0x00000800    /*  Status of TR1 Switch. */
#define BITM_AFE_TSWSTA_T11STA               0x00000400    /*  Status of T11 Switch. */
#define BITM_AFE_TSWSTA_T10STA               0x00000200    /*  Status of T10 Switch. */
#define BITM_AFE_TSWSTA_T9STA                0x00000100    /*  Status of T9 Switch. */
#define BITM_AFE_TSWSTA_T8STA                0x00000080    /*  Status of T8 Switch. */
#define BITM_AFE_TSWSTA_T7STA                0x00000040    /*  Status of T7 Switch. */
#define BITM_AFE_TSWSTA_T6STA                0x00000020    /*  Status of T6 Switch. */
#define BITM_AFE_TSWSTA_T5STA                0x00000010    /*  Status of T5 Switch. */
#define BITM_AFE_TSWSTA_T4STA                0x00000008    /*  Status of T4 Switch. */
#define BITM_AFE_TSWSTA_T3STA                0x00000004    /*  Status of T3 Switch. */
#define BITM_AFE_TSWSTA_T2STA                0x00000002    /*  Status of T2 Switch. */
#define BITM_AFE_TSWSTA_T1STA                0x00000001    /*  Status of T1 Switch. */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_STATSVAR                         Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_STATSVAR_VARIANCE            0            /*  Statistical Variance Value */
#define BITM_AFE_STATSVAR_VARIANCE           0x7FFFFFFF    /*  Statistical Variance Value */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_STATSCON                         Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_STATSCON_STDDEV              7            /*  Standard Deviation Configuration */
#define BITP_AFE_STATSCON_SAMPLENUM           4            /*  Sample Size */
#define BITP_AFE_STATSCON_RESRVED             1            /*  Reserved */
#define BITP_AFE_STATSCON_STATSEN             0            /*  Statistics Enable */
#define BITM_AFE_STATSCON_STDDEV             0x00000F80    /*  Standard Deviation Configuration */
#define BITM_AFE_STATSCON_SAMPLENUM          0x00000070    /*  Sample Size */
#define BITM_AFE_STATSCON_RESRVED            0x0000000E    /*  Reserved */
#define BITM_AFE_STATSCON_STATSEN            0x00000001    /*  Statistics Enable */
#define ENUM_AFE_STATSCON_DIS                0x00000000            /*  STATSEN: Disable Statistics */
#define ENUM_AFE_STATSCON_EN                 0x00000001            /*  STATSEN: Enable Statistics */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_STATSMEAN                        Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_STATSMEAN_MEAN               0            /*  Mean Output */
#define BITM_AFE_STATSMEAN_MEAN              0x0000FFFF    /*  Mean Output */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_SEQ0INFO                         Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_SEQ0INFO_LEN                16            /*  SEQ0 Instruction Number */
#define BITP_AFE_SEQ0INFO_ADDR                0            /*  SEQ0 Start Address */
#define BITM_AFE_SEQ0INFO_LEN                0x07FF0000    /*  SEQ0 Instruction Number */
#define BITM_AFE_SEQ0INFO_ADDR               0x000007FF    /*  SEQ0 Start Address */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_SEQ2INFO                         Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_SEQ2INFO_LEN                16            /*  SEQ2 Instruction Number */
#define BITP_AFE_SEQ2INFO_ADDR                0            /*  SEQ2 Start Address */
#define BITM_AFE_SEQ2INFO_LEN                0x07FF0000    /*  SEQ2 Instruction Number */
#define BITM_AFE_SEQ2INFO_ADDR               0x000007FF    /*  SEQ2 Start Address */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_CMDFIFOWADDR                     Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_CMDFIFOWADDR_WADDR           0            /*  Write Address */
#define BITM_AFE_CMDFIFOWADDR_WADDR          0x000007FF    /*  Write Address */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_CMDDATACON                       Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_CMDDATACON_DATAMEMMDE        9            /*  Data FIFO Mode Select */
#define BITP_AFE_CMDDATACON_DATA_MEM_SEL      6            /*  Data FIFO Size Select */
#define BITP_AFE_CMDDATACON_CMDMEMMDE         3            /*  This is Command Fifo Mode Register */
#define BITP_AFE_CMDDATACON_CMD_MEM_SEL       0            /*  Command Memory Select */
#define BITM_AFE_CMDDATACON_DATAMEMMDE       0x00000E00    /*  Data FIFO Mode Select */
#define BITM_AFE_CMDDATACON_DATA_MEM_SEL     0x000001C0    /*  Data FIFO Size Select */
#define BITM_AFE_CMDDATACON_CMDMEMMDE        0x00000038    /*  This is Command Fifo Mode Register */
#define BITM_AFE_CMDDATACON_CMD_MEM_SEL      0x00000007    /*  Command Memory Select */
#define ENUM_AFE_CMDDATACON_DFIFO            0x00000400            /*  DATAMEMMDE: FIFO MODE */
#define ENUM_AFE_CMDDATACON_DSTM             0x00000600            /*  DATAMEMMDE: STREAM MODE */
#define ENUM_AFE_CMDDATACON_DMEM32B          0x00000000            /*  DATA_MEM_SEL: 32B_1 Local Memory */
#define ENUM_AFE_CMDDATACON_DMEM2K           0x00000040            /*  DATA_MEM_SEL: 2K_2 SRAM */
#define ENUM_AFE_CMDDATACON_DMEM4K           0x00000080            /*  DATA_MEM_SEL: 2K_2~1 SRAM */
#define ENUM_AFE_CMDDATACON_DMEM6K           0x000000C0            /*  DATA_MEM_SEL: 2K_2~0 SRAM */
#define ENUM_AFE_CMDDATACON_CMEM             0x00000008            /*  CMDMEMMDE: MEMORY MODE */
#define ENUM_AFE_CMDDATACON_CFIFO            0x00000010            /*  CMDMEMMDE: FIFO MODE */
#define ENUM_AFE_CMDDATACON_CSTM             0x00000018            /*  CMDMEMMDE: STREAM MODE */
#define ENUM_AFE_CMDDATACON_CMEM32B          0x00000000            /*  CMD_MEM_SEL: 32B_0 Local Memory */
#define ENUM_AFE_CMDDATACON_CMEM2K           0x00000001            /*  CMD_MEM_SEL: 2K_0 SRAM */
#define ENUM_AFE_CMDDATACON_CMEM4K           0x00000002            /*  CMD_MEM_SEL: 2K_0~1 SRAM */
#define ENUM_AFE_CMDDATACON_CMEM6K           0x00000003            /*  CMD_MEM_SEL: 2K_0~2 SRAM */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_DATAFIFOTHRES                    Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_DATAFIFOTHRES_HIGHTHRES     16            /*  High Threshold */
#define BITM_AFE_DATAFIFOTHRES_HIGHTHRES     0x07FF0000    /*  High Threshold */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_SEQ3INFO                         Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_SEQ3INFO_LEN                16            /*  SEQ3 Instruction Number */
#define BITP_AFE_SEQ3INFO_ADDR                0            /*  SEQ3 Start Address */
#define BITM_AFE_SEQ3INFO_LEN                0x07FF0000    /*  SEQ3 Instruction Number */
#define BITM_AFE_SEQ3INFO_ADDR               0x000007FF    /*  SEQ3 Start Address */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_SEQ1INFO                         Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_SEQ1INFO_LEN                16            /*  SEQ1 Instruction Number */
#define BITP_AFE_SEQ1INFO_ADDR                0            /*  SEQ1 Start Address */
#define BITM_AFE_SEQ1INFO_LEN                0x07FF0000    /*  SEQ1 Instruction Number */
#define BITM_AFE_SEQ1INFO_ADDR               0x000007FF    /*  SEQ1 Start Address */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_REPEATADCCNV                     Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_REPEATADCCNV_NUM             4            /*  Repeat Value */
#define BITP_AFE_REPEATADCCNV_EN              0            /*  Enable Repeat ADC Conversions */
#define BITM_AFE_REPEATADCCNV_NUM            0x00000FF0    /*  Repeat Value */
#define BITM_AFE_REPEATADCCNV_EN             0x00000001    /*  Enable Repeat ADC Conversions */
#define ENUM_AFE_REPEATADCCNV_DIS            0x00000000            /*  EN: Disable Repeat ADC Conversions */
#define ENUM_AFE_REPEATADCCNV_EN             0x00000001            /*  EN: Enable Repeat ADC Conversions */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_FIFOCNTSTA                       Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_FIFOCNTSTA_DATAFIFOCNTSTA   16            /*  Current Number of Words in the Data FIFO */
#define BITM_AFE_FIFOCNTSTA_DATAFIFOCNTSTA   0x07FF0000    /*  Current Number of Words in the Data FIFO */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_CALDATLOCK                       Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_CALDATLOCK_KEY               0            /*  Password for Calibration Data Registers */
#define BITM_AFE_CALDATLOCK_KEY              0xFFFFFFFF    /*  Password for Calibration Data Registers */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_ADCOFFSETHSTIA                   Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_ADCOFFSETHSTIA_VALUE         0            /*  HSTIA Offset Calibration */
#define BITM_AFE_ADCOFFSETHSTIA_VALUE        0x00007FFF    /*  HSTIA Offset Calibration */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_ADCGAINTEMPSENS0                 Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_ADCGAINTEMPSENS0_VALUE       0            /*  Gain Calibration Temp Sensor Channel */
#define BITM_AFE_ADCGAINTEMPSENS0_VALUE      0x00007FFF    /*  Gain Calibration Temp Sensor Channel */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_ADCOFFSETTEMPSENS0               Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_ADCOFFSETTEMPSENS0_VALUE     0            /*  Offset Calibration Temp Sensor */
#define BITM_AFE_ADCOFFSETTEMPSENS0_VALUE    0x00007FFF    /*  Offset Calibration Temp Sensor */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_ADCGAINGN1                       Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_ADCGAINGN1_VALUE             0            /*  Gain Calibration PGA Gain 1x */
#define BITM_AFE_ADCGAINGN1_VALUE            0x00007FFF    /*  Gain Calibration PGA Gain 1x */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_ADCOFFSETGN1                     Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_ADCOFFSETGN1_VALUE           0            /*  Offset Calibration Gain1 */
#define BITM_AFE_ADCOFFSETGN1_VALUE          0x00007FFF    /*  Offset Calibration Gain1 */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_DACGAIN                          Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_DACGAIN_VALUE                0            /*  HS DAC Gain Correction Factor */
#define BITM_AFE_DACGAIN_VALUE               0x00000FFF    /*  HS DAC Gain Correction Factor */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_DACOFFSETATTEN                   Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_DACOFFSETATTEN_VALUE         0            /*  DAC Offset Correction Factor */
#define BITM_AFE_DACOFFSETATTEN_VALUE        0x00000FFF    /*  DAC Offset Correction Factor */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_DACOFFSET                        Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_DACOFFSET_VALUE              0            /*  DAC Offset Correction Factor */
#define BITM_AFE_DACOFFSET_VALUE             0x00000FFF    /*  DAC Offset Correction Factor */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_ADCGAINGN1P5                     Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_ADCGAINGN1P5_VALUE           0            /*  Gain Calibration PGA Gain 1.5x */
#define BITM_AFE_ADCGAINGN1P5_VALUE          0x00007FFF    /*  Gain Calibration PGA Gain 1.5x */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_ADCGAINGN2                       Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_ADCGAINGN2_VALUE             0            /*  Gain Calibration PGA Gain 2x */
#define BITM_AFE_ADCGAINGN2_VALUE            0x00007FFF    /*  Gain Calibration PGA Gain 2x */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_ADCGAINGN4                       Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_ADCGAINGN4_VALUE             0            /*  Gain Calibration PGA Gain 4x */
#define BITM_AFE_ADCGAINGN4_VALUE            0x00007FFF    /*  Gain Calibration PGA Gain 4x */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_ADCPGAOFFSETCANCEL               Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_ADCPGAOFFSETCANCEL_OFFSETCANCEL  0            /*  Offset Cancellation */
#define BITM_AFE_ADCPGAOFFSETCANCEL_OFFSETCANCEL 0x00007FFF    /*  Offset Cancellation */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_ADCGNHSTIA                       Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_ADCGNHSTIA_VALUE             0            /*  Gain Error Calibration HS TIA Channel */
#define BITM_AFE_ADCGNHSTIA_VALUE            0x00007FFF    /*  Gain Error Calibration HS TIA Channel */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_ADCOFFSETLPTIA0                  Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_ADCOFFSETLPTIA0_VALUE        0            /*  Offset Calibration for ULP-TIA0 */
#define BITM_AFE_ADCOFFSETLPTIA0_VALUE       0x00007FFF    /*  Offset Calibration for ULP-TIA0 */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_ADCGNLPTIA0                      Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_ADCGNLPTIA0_VALUE            0            /*  Gain Error Calibration ULPTIA0 */
#define BITM_AFE_ADCGNLPTIA0_VALUE           0x00007FFF    /*  Gain Error Calibration ULPTIA0 */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_ADCPGAGN4OFCAL                   Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_ADCPGAGN4OFCAL_ADCGAINAUX    0            /*  DC Calibration Gain=4 */
#define BITM_AFE_ADCPGAGN4OFCAL_ADCGAINAUX   0x00007FFF    /*  DC Calibration Gain=4 */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_ADCGAINGN9                       Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_ADCGAINGN9_VALUE             0            /*  Gain Calibration PGA Gain 9x */
#define BITM_AFE_ADCGAINGN9_VALUE            0x00007FFF    /*  Gain Calibration PGA Gain 9x */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_ADCOFFSETEMPSENS1                Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_ADCOFFSETEMPSENS1_VALUE      0            /*  Offset Calibration Temp Sensor */
#define BITM_AFE_ADCOFFSETEMPSENS1_VALUE     0x00007FFF    /*  Offset Calibration Temp Sensor */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_ADCGAINDIOTEMPSENS               Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_ADCGAINDIOTEMPSENS_VALUE     0            /*  Gain Calibration for Diode Temp Sensor */
#define BITM_AFE_ADCGAINDIOTEMPSENS_VALUE    0x00007FFF    /*  Gain Calibration for Diode Temp Sensor */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_DACOFFSETATTENHP                 Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_DACOFFSETATTENHP_VALUE       0            /*  DAC Offset Correction Factor */
#define BITM_AFE_DACOFFSETATTENHP_VALUE      0x00000FFF    /*  DAC Offset Correction Factor */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_DACOFFSETHP                      Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_DACOFFSETHP_VALUE            0            /*  DAC Offset Correction Factor */
#define BITM_AFE_DACOFFSETHP_VALUE           0x00000FFF    /*  DAC Offset Correction Factor */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_ADCGNLPTIA1                      Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_ADCGNLPTIA1_ULPTIA1GN        0            /*  Gain Calibration ULP-TIA1 */
#define BITM_AFE_ADCGNLPTIA1_ULPTIA1GN       0x00007FFF    /*  Gain Calibration ULP-TIA1 */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_ADCOFFSETGN2                     Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_ADCOFFSETGN2_VALUE           0            /*  Offset Calibration Auxiliary Channel (PGA Gain =2) */
#define BITM_AFE_ADCOFFSETGN2_VALUE          0x00007FFF    /*  Offset Calibration Auxiliary Channel (PGA Gain =2) */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_ADCOFFSETGN1P5                   Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_ADCOFFSETGN1P5_VALUE         0            /*  Offset Calibration Gain1.5 */
#define BITM_AFE_ADCOFFSETGN1P5_VALUE        0x00007FFF    /*  Offset Calibration Gain1.5 */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_ADCOFFSETGN9                     Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_ADCOFFSETGN9_VALUE           0            /*  Offset Calibration Gain9 */
#define BITM_AFE_ADCOFFSETGN9_VALUE          0x00007FFF    /*  Offset Calibration Gain9 */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_ADCOFFSETGN4                     Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_ADCOFFSETGN4_VALUE           0            /*  Offset Calibration Gain4 */
#define BITM_AFE_ADCOFFSETGN4_VALUE          0x00007FFF    /*  Offset Calibration Gain4 */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_PMBW                             Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_PMBW_SYSBW                   2            /*  Configure System Bandwidth */
#define BITP_AFE_PMBW_SYSHP                   0            /*  Set High Speed DAC and ADC in High Power Mode */
#define BITM_AFE_PMBW_SYSBW                  0x0000000C    /*  Configure System Bandwidth */
#define BITM_AFE_PMBW_SYSHP                  0x00000001    /*  Set High Speed DAC and ADC in High Power Mode */
#define ENUM_AFE_PMBW_BWNA                   0x00000000            /*  SYSBW: no action for system configuration */
#define ENUM_AFE_PMBW_BW50                   0x00000004            /*  SYSBW: 50kHz -3dB bandwidth */
#define ENUM_AFE_PMBW_BW100                  0x00000008            /*  SYSBW: 100kHz -3dB bandwidth */
#define ENUM_AFE_PMBW_BW250                  0x0000000C            /*  SYSBW: 250kHz -3dB bandwidth */
#define ENUM_AFE_PMBW_LP                     0x00000000            /*  SYSHP: LP mode */
#define ENUM_AFE_PMBW_HP                     0x00000001            /*  SYSHP: HP mode */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_SWMUX                           Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_SWMUX_CMMUX                 3            /*  CM Resistor Select for Ain2, Ain3 */
#define BITM_AFE_SWMUX_CMMUX                0x00000008    /*  CM Resistor Select for Ain2, Ain3 */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_AFE_TEMPSEN_DIO                  Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_AFE_TEMPSEN_DIO_TSDIO_PD    17            /*  Power Down Control */
#define BITP_AFE_AFE_TEMPSEN_DIO_TSDIO_EN    16            /*  Test Signal Enable */
#define BITP_AFE_AFE_TEMPSEN_DIO_TSDIO_CON    0            /*  Bias Current Selection */
#define BITM_AFE_AFE_TEMPSEN_DIO_TSDIO_PD    0x00020000    /*  Power Down Control */
#define BITM_AFE_AFE_TEMPSEN_DIO_TSDIO_EN    0x00010000    /*  Test Signal Enable */
#define BITM_AFE_AFE_TEMPSEN_DIO_TSDIO_CON   0x0000FFFF    /*  Bias Current Selection */

/* -------------------------------------------------------------------------------------------------------------------------
          AFE_ADCBUFCON                        Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_AFE_ADCBUFCON_AMPDIS             4            /*  Disable OpAmp. */
#define BITP_AFE_ADCBUFCON_CHOPDIS            0            /*  Disable Chop */
#define BITM_AFE_ADCBUFCON_AMPDIS            0x000001F0    /*  Disable OpAmp. */
#define BITM_AFE_ADCBUFCON_CHOPDIS           0x0000000F    /*  Disable Chop */


/* ============================================================================================================================
        Interrupt Controller Register Map
   ============================================================================================================================ */

/* ============================================================================================================================
        INTC
   ============================================================================================================================ */
#define REG_INTC_INTCPOL_RESET               0x00000000            /*      Reset Value for INTCPOL  */
#define REG_INTC_INTCPOL                     0x00003000            /*  INTC Interrupt Polarity Register */
#define REG_INTC_INTCCLR_RESET               0x00000000            /*      Reset Value for INTCCLR  */
#define REG_INTC_INTCCLR                     0x00003004            /*  INTC Interrupt Clear Register */
#define REG_INTC_INTCSEL0_RESET              0x00002000            /*      Reset Value for INTCSEL0  */
#define REG_INTC_INTCSEL0                    0x00003008            /*  INTC INT0 Select Register */
#define REG_INTC_INTCSEL1_RESET              0x00000000            /*      Reset Value for INTCSEL1  */
#define REG_INTC_INTCSEL1                    0x0000300C            /*  INTC INT1 Select Register */
#define REG_INTC_INTCFLAG0_RESET             0x00000000            /*      Reset Value for INTCFLAG0  */
#define REG_INTC_INTCFLAG0                   0x00003010            /*  INTC INT0 FLAG Register */
#define REG_INTC_INTCFLAG1_RESET             0x00000000            /*      Reset Value for INTCFLAG1  */
#define REG_INTC_INTCFLAG1                   0x00003014            /*  INTC INT1 FLAG Register */

/* ============================================================================================================================
        INTC Register BitMasks, Positions & Enumerations
   ============================================================================================================================ */
/* -------------------------------------------------------------------------------------------------------------------------
          INTC_INTCPOL                         Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_INTC_INTCPOL_INTPOL              0
#define BITM_INTC_INTCPOL_INTPOL             0x00000001

/* -------------------------------------------------------------------------------------------------------------------------
          INTC_INTCCLR                         Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_INTC_INTCCLR_INTCLR31           31
#define BITP_INTC_INTCCLR_INTCLR30           30
#define BITP_INTC_INTCCLR_INTCLR29           29
#define BITP_INTC_INTCCLR_INTCLR28           28
#define BITP_INTC_INTCCLR_INTCLR27           27
#define BITP_INTC_INTCCLR_INTCLR26           26
#define BITP_INTC_INTCCLR_INTCLR25           25
#define BITP_INTC_INTCCLR_INTCLR24           24
#define BITP_INTC_INTCCLR_INTCLR23           23
#define BITP_INTC_INTCCLR_INTCLR22           22
#define BITP_INTC_INTCCLR_INTCLR21           21
#define BITP_INTC_INTCCLR_INTCLR20           20
#define BITP_INTC_INTCCLR_INTCLR19           19
#define BITP_INTC_INTCCLR_INTCLR18           18
#define BITP_INTC_INTCCLR_INTCLR17           17
#define BITP_INTC_INTCCLR_INTCLR16           16
#define BITP_INTC_INTCCLR_INTCLR15           15
#define BITP_INTC_INTCCLR_INTCLR14           14
#define BITP_INTC_INTCCLR_INTCLR13           13
#define BITP_INTC_INTCCLR_INTCLR12           12            /*  Custom IRQ 3. Write 1 to clear. */
#define BITP_INTC_INTCCLR_INTCLR11           11            /*  Custom IRQ 2. Write 1 to clear. */
#define BITP_INTC_INTCCLR_INTCLR10           10            /*  Custom IRQ 1. Write 1 to clear. */
#define BITP_INTC_INTCCLR_INTCLR9             9            /*  Custom IRQ 0. Write 1 to clear */
#define BITP_INTC_INTCCLR_INTCLR8             8
#define BITP_INTC_INTCCLR_INTCLR7             7
#define BITP_INTC_INTCCLR_INTCLR6             6
#define BITP_INTC_INTCCLR_INTCLR5             5
#define BITP_INTC_INTCCLR_INTCLR4             4
#define BITP_INTC_INTCCLR_INTCLR3             3
#define BITP_INTC_INTCCLR_INTCLR2             2
#define BITP_INTC_INTCCLR_INTCLR1             1
#define BITP_INTC_INTCCLR_INTCLR0             0
#define BITM_INTC_INTCCLR_INTCLR31           0x80000000
#define BITM_INTC_INTCCLR_INTCLR30           0x40000000
#define BITM_INTC_INTCCLR_INTCLR29           0x20000000
#define BITM_INTC_INTCCLR_INTCLR28           0x10000000
#define BITM_INTC_INTCCLR_INTCLR27           0x08000000
#define BITM_INTC_INTCCLR_INTCLR26           0x04000000
#define BITM_INTC_INTCCLR_INTCLR25           0x02000000
#define BITM_INTC_INTCCLR_INTCLR24           0x01000000
#define BITM_INTC_INTCCLR_INTCLR23           0x00800000
#define BITM_INTC_INTCCLR_INTCLR22           0x00400000
#define BITM_INTC_INTCCLR_INTCLR21           0x00200000
#define BITM_INTC_INTCCLR_INTCLR20           0x00100000
#define BITM_INTC_INTCCLR_INTCLR19           0x00080000
#define BITM_INTC_INTCCLR_INTCLR18           0x00040000
#define BITM_INTC_INTCCLR_INTCLR17           0x00020000
#define BITM_INTC_INTCCLR_INTCLR16           0x00010000
#define BITM_INTC_INTCCLR_INTCLR15           0x00008000
#define BITM_INTC_INTCCLR_INTCLR14           0x00004000
#define BITM_INTC_INTCCLR_INTCLR13           0x00002000
#define BITM_INTC_INTCCLR_INTCLR12           0x00001000    /*  Custom IRQ 3. Write 1 to clear. */
#define BITM_INTC_INTCCLR_INTCLR11           0x00000800    /*  Custom IRQ 2. Write 1 to clear. */
#define BITM_INTC_INTCCLR_INTCLR10           0x00000400    /*  Custom IRQ 1. Write 1 to clear. */
#define BITM_INTC_INTCCLR_INTCLR9            0x00000200    /*  Custom IRQ 0. Write 1 to clear */
#define BITM_INTC_INTCCLR_INTCLR8            0x00000100
#define BITM_INTC_INTCCLR_INTCLR7            0x00000080
#define BITM_INTC_INTCCLR_INTCLR6            0x00000040
#define BITM_INTC_INTCCLR_INTCLR5            0x00000020
#define BITM_INTC_INTCCLR_INTCLR4            0x00000010
#define BITM_INTC_INTCCLR_INTCLR3            0x00000008
#define BITM_INTC_INTCCLR_INTCLR2            0x00000004
#define BITM_INTC_INTCCLR_INTCLR1            0x00000002
#define BITM_INTC_INTCCLR_INTCLR0            0x00000001

/* -------------------------------------------------------------------------------------------------------------------------
          INTC_INTCSEL0                        Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_INTC_INTCSEL0_INTSEL31          31
#define BITP_INTC_INTCSEL0_INTSEL30          30
#define BITP_INTC_INTCSEL0_INTSEL29          29
#define BITP_INTC_INTCSEL0_INTSEL28          28
#define BITP_INTC_INTCSEL0_INTSEL27          27
#define BITP_INTC_INTCSEL0_INTSEL26          26
#define BITP_INTC_INTCSEL0_INTSEL25          25
#define BITP_INTC_INTCSEL0_INTSEL24          24
#define BITP_INTC_INTCSEL0_INTSEL23          23
#define BITP_INTC_INTCSEL0_INTSEL22          22
#define BITP_INTC_INTCSEL0_INTSEL21          21
#define BITP_INTC_INTCSEL0_INTSEL20          20
#define BITP_INTC_INTCSEL0_INTSEL19          19
#define BITP_INTC_INTCSEL0_INTSEL18          18
#define BITP_INTC_INTCSEL0_INTSEL17          17
#define BITP_INTC_INTCSEL0_INTSEL16          16
#define BITP_INTC_INTCSEL0_INTSEL15          15
#define BITP_INTC_INTCSEL0_INTSEL14          14
#define BITP_INTC_INTCSEL0_INTSEL13          13
#define BITP_INTC_INTCSEL0_INTSEL12          12            /*  Custom IRQ 3 Enable */
#define BITP_INTC_INTCSEL0_INTSEL11          11            /*  Custom IRQ 2 Enable */
#define BITP_INTC_INTCSEL0_INTSEL10          10            /*  Custom IRQ 1 Enable */
#define BITP_INTC_INTCSEL0_INTSEL9            9            /*  Custom IRQ 0 Enable */
#define BITP_INTC_INTCSEL0_INTSEL8            8
#define BITP_INTC_INTCSEL0_INTSEL7            7
#define BITP_INTC_INTCSEL0_INTSEL6            6
#define BITP_INTC_INTCSEL0_INTSEL5            5
#define BITP_INTC_INTCSEL0_INTSEL4            4
#define BITP_INTC_INTCSEL0_INTSEL3            3
#define BITP_INTC_INTCSEL0_INTSEL2            2
#define BITP_INTC_INTCSEL0_INTSEL1            1
#define BITP_INTC_INTCSEL0_INTSEL0            0
#define BITM_INTC_INTCSEL0_INTSEL31          0x80000000
#define BITM_INTC_INTCSEL0_INTSEL30          0x40000000
#define BITM_INTC_INTCSEL0_INTSEL29          0x20000000
#define BITM_INTC_INTCSEL0_INTSEL28          0x10000000
#define BITM_INTC_INTCSEL0_INTSEL27          0x08000000
#define BITM_INTC_INTCSEL0_INTSEL26          0x04000000
#define BITM_INTC_INTCSEL0_INTSEL25          0x02000000
#define BITM_INTC_INTCSEL0_INTSEL24          0x01000000
#define BITM_INTC_INTCSEL0_INTSEL23          0x00800000
#define BITM_INTC_INTCSEL0_INTSEL22          0x00400000
#define BITM_INTC_INTCSEL0_INTSEL21          0x00200000
#define BITM_INTC_INTCSEL0_INTSEL20          0x00100000
#define BITM_INTC_INTCSEL0_INTSEL19          0x00080000
#define BITM_INTC_INTCSEL0_INTSEL18          0x00040000
#define BITM_INTC_INTCSEL0_INTSEL17          0x00020000
#define BITM_INTC_INTCSEL0_INTSEL16          0x00010000
#define BITM_INTC_INTCSEL0_INTSEL15          0x00008000
#define BITM_INTC_INTCSEL0_INTSEL14          0x00004000
#define BITM_INTC_INTCSEL0_INTSEL13          0x00002000
#define BITM_INTC_INTCSEL0_INTSEL12          0x00001000    /*  Custom IRQ 3 Enable */
#define BITM_INTC_INTCSEL0_INTSEL11          0x00000800    /*  Custom IRQ 2 Enable */
#define BITM_INTC_INTCSEL0_INTSEL10          0x00000400    /*  Custom IRQ 1 Enable */
#define BITM_INTC_INTCSEL0_INTSEL9           0x00000200    /*  Custom IRQ 0 Enable */
#define BITM_INTC_INTCSEL0_INTSEL8           0x00000100
#define BITM_INTC_INTCSEL0_INTSEL7           0x00000080
#define BITM_INTC_INTCSEL0_INTSEL6           0x00000040
#define BITM_INTC_INTCSEL0_INTSEL5           0x00000020
#define BITM_INTC_INTCSEL0_INTSEL4           0x00000010
#define BITM_INTC_INTCSEL0_INTSEL3           0x00000008
#define BITM_INTC_INTCSEL0_INTSEL2           0x00000004
#define BITM_INTC_INTCSEL0_INTSEL1           0x00000002
#define BITM_INTC_INTCSEL0_INTSEL0           0x00000001

/* -------------------------------------------------------------------------------------------------------------------------
          INTC_INTCSEL1                        Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_INTC_INTCSEL1_INTSEL31          31
#define BITP_INTC_INTCSEL1_INTSEL30          30
#define BITP_INTC_INTCSEL1_INTSEL29          29
#define BITP_INTC_INTCSEL1_INTSEL28          28
#define BITP_INTC_INTCSEL1_INTSEL27          27
#define BITP_INTC_INTCSEL1_INTSEL26          26
#define BITP_INTC_INTCSEL1_INTSEL25          25
#define BITP_INTC_INTCSEL1_INTSEL24          24
#define BITP_INTC_INTCSEL1_INTSEL23          23
#define BITP_INTC_INTCSEL1_INTSEL22          22
#define BITP_INTC_INTCSEL1_INTSEL21          21
#define BITP_INTC_INTCSEL1_INTSEL20          20
#define BITP_INTC_INTCSEL1_INTSEL19          19
#define BITP_INTC_INTCSEL1_INTSEL18          18
#define BITP_INTC_INTCSEL1_INTSEL17          17
#define BITP_INTC_INTCSEL1_INTSEL16          16
#define BITP_INTC_INTCSEL1_INTSEL15          15
#define BITP_INTC_INTCSEL1_INTSEL14          14
#define BITP_INTC_INTCSEL1_INTSEL13          13
#define BITP_INTC_INTCSEL1_INTSEL12          12            /*  Custom IRQ 3 Enable */
#define BITP_INTC_INTCSEL1_INTSEL11          11            /*  Custom IRQ 2 Enable */
#define BITP_INTC_INTCSEL1_INTSEL10          10            /*  Custom IRQ 1 Enable */
#define BITP_INTC_INTCSEL1_INTSEL9            9            /*  Custom IRQ 0 Enable */
#define BITP_INTC_INTCSEL1_INTSEL8            8
#define BITP_INTC_INTCSEL1_INTSEL7            7
#define BITP_INTC_INTCSEL1_INTSEL6            6
#define BITP_INTC_INTCSEL1_INTSEL5            5
#define BITP_INTC_INTCSEL1_INTSEL4            4
#define BITP_INTC_INTCSEL1_INTSEL3            3
#define BITP_INTC_INTCSEL1_INTSEL2            2
#define BITP_INTC_INTCSEL1_INTSEL1            1
#define BITP_INTC_INTCSEL1_INTSEL0            0
#define BITM_INTC_INTCSEL1_INTSEL31          0x80000000
#define BITM_INTC_INTCSEL1_INTSEL30          0x40000000
#define BITM_INTC_INTCSEL1_INTSEL29          0x20000000
#define BITM_INTC_INTCSEL1_INTSEL28          0x10000000
#define BITM_INTC_INTCSEL1_INTSEL27          0x08000000
#define BITM_INTC_INTCSEL1_INTSEL26          0x04000000
#define BITM_INTC_INTCSEL1_INTSEL25          0x02000000
#define BITM_INTC_INTCSEL1_INTSEL24          0x01000000
#define BITM_INTC_INTCSEL1_INTSEL23          0x00800000
#define BITM_INTC_INTCSEL1_INTSEL22          0x00400000
#define BITM_INTC_INTCSEL1_INTSEL21          0x00200000
#define BITM_INTC_INTCSEL1_INTSEL20          0x00100000
#define BITM_INTC_INTCSEL1_INTSEL19          0x00080000
#define BITM_INTC_INTCSEL1_INTSEL18          0x00040000
#define BITM_INTC_INTCSEL1_INTSEL17          0x00020000
#define BITM_INTC_INTCSEL1_INTSEL16          0x00010000
#define BITM_INTC_INTCSEL1_INTSEL15          0x00008000
#define BITM_INTC_INTCSEL1_INTSEL14          0x00004000
#define BITM_INTC_INTCSEL1_INTSEL13          0x00002000
#define BITM_INTC_INTCSEL1_INTSEL12          0x00001000    /*  Custom IRQ 3 Enable */
#define BITM_INTC_INTCSEL1_INTSEL11          0x00000800    /*  Custom IRQ 2 Enable */
#define BITM_INTC_INTCSEL1_INTSEL10          0x00000400    /*  Custom IRQ 1 Enable */
#define BITM_INTC_INTCSEL1_INTSEL9           0x00000200    /*  Custom IRQ 0 Enable */
#define BITM_INTC_INTCSEL1_INTSEL8           0x00000100
#define BITM_INTC_INTCSEL1_INTSEL7           0x00000080
#define BITM_INTC_INTCSEL1_INTSEL6           0x00000040
#define BITM_INTC_INTCSEL1_INTSEL5           0x00000020
#define BITM_INTC_INTCSEL1_INTSEL4           0x00000010
#define BITM_INTC_INTCSEL1_INTSEL3           0x00000008
#define BITM_INTC_INTCSEL1_INTSEL2           0x00000004
#define BITM_INTC_INTCSEL1_INTSEL1           0x00000002
#define BITM_INTC_INTCSEL1_INTSEL0           0x00000001

/* -------------------------------------------------------------------------------------------------------------------------
          INTC_INTCFLAG0                       Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_INTC_INTCFLAG0_FLAG31           31
#define BITP_INTC_INTCFLAG0_FLAG30           30
#define BITP_INTC_INTCFLAG0_FLAG29           29
#define BITP_INTC_INTCFLAG0_FLAG28           28
#define BITP_INTC_INTCFLAG0_FLAG27           27
#define BITP_INTC_INTCFLAG0_FLAG26           26
#define BITP_INTC_INTCFLAG0_FLAG25           25
#define BITP_INTC_INTCFLAG0_FLAG24           24
#define BITP_INTC_INTCFLAG0_FLAG23           23
#define BITP_INTC_INTCFLAG0_FLAG22           22
#define BITP_INTC_INTCFLAG0_FLAG21           21
#define BITP_INTC_INTCFLAG0_FLAG20           20
#define BITP_INTC_INTCFLAG0_FLAG19           19
#define BITP_INTC_INTCFLAG0_FLAG18           18
#define BITP_INTC_INTCFLAG0_FLAG17           17
#define BITP_INTC_INTCFLAG0_FLAG16           16
#define BITP_INTC_INTCFLAG0_FLAG15           15
#define BITP_INTC_INTCFLAG0_FLAG14           14
#define BITP_INTC_INTCFLAG0_FLAG13           13
#define BITP_INTC_INTCFLAG0_FLAG12           12            /*  Custom IRQ 3 Status */
#define BITP_INTC_INTCFLAG0_FLAG11           11            /*  Custom IRQ 2 Status */
#define BITP_INTC_INTCFLAG0_FLAG10           10            /*  Custom IRQ 1 Status */
#define BITP_INTC_INTCFLAG0_FLAG9             9            /*  Custom IRQ 0 Status */
#define BITP_INTC_INTCFLAG0_FLAG8             8            /*  Variance IRQ status. */
#define BITP_INTC_INTCFLAG0_FLAG7             7
#define BITP_INTC_INTCFLAG0_FLAG6             6
#define BITP_INTC_INTCFLAG0_FLAG5             5
#define BITP_INTC_INTCFLAG0_FLAG4             4
#define BITP_INTC_INTCFLAG0_FLAG3             3
#define BITP_INTC_INTCFLAG0_FLAG2             2
#define BITP_INTC_INTCFLAG0_FLAG1             1
#define BITP_INTC_INTCFLAG0_FLAG0             0
#define BITM_INTC_INTCFLAG0_FLAG31           0x80000000
#define BITM_INTC_INTCFLAG0_FLAG30           0x40000000
#define BITM_INTC_INTCFLAG0_FLAG29           0x20000000
#define BITM_INTC_INTCFLAG0_FLAG28           0x10000000
#define BITM_INTC_INTCFLAG0_FLAG27           0x08000000
#define BITM_INTC_INTCFLAG0_FLAG26           0x04000000
#define BITM_INTC_INTCFLAG0_FLAG25           0x02000000
#define BITM_INTC_INTCFLAG0_FLAG24           0x01000000
#define BITM_INTC_INTCFLAG0_FLAG23           0x00800000
#define BITM_INTC_INTCFLAG0_FLAG22           0x00400000
#define BITM_INTC_INTCFLAG0_FLAG21           0x00200000
#define BITM_INTC_INTCFLAG0_FLAG20           0x00100000
#define BITM_INTC_INTCFLAG0_FLAG19           0x00080000
#define BITM_INTC_INTCFLAG0_FLAG18           0x00040000
#define BITM_INTC_INTCFLAG0_FLAG17           0x00020000
#define BITM_INTC_INTCFLAG0_FLAG16           0x00010000
#define BITM_INTC_INTCFLAG0_FLAG15           0x00008000
#define BITM_INTC_INTCFLAG0_FLAG14           0x00004000
#define BITM_INTC_INTCFLAG0_FLAG13           0x00002000
#define BITM_INTC_INTCFLAG0_FLAG12           0x00001000    /*  Custom IRQ 3 Status */
#define BITM_INTC_INTCFLAG0_FLAG11           0x00000800    /*  Custom IRQ 2 Status */
#define BITM_INTC_INTCFLAG0_FLAG10           0x00000400    /*  Custom IRQ 1 Status */
#define BITM_INTC_INTCFLAG0_FLAG9            0x00000200    /*  Custom IRQ 0 Status */
#define BITM_INTC_INTCFLAG0_FLAG8            0x00000100    /*  Variance IRQ status. */
#define BITM_INTC_INTCFLAG0_FLAG7            0x00000080
#define BITM_INTC_INTCFLAG0_FLAG6            0x00000040
#define BITM_INTC_INTCFLAG0_FLAG5            0x00000020
#define BITM_INTC_INTCFLAG0_FLAG4            0x00000010
#define BITM_INTC_INTCFLAG0_FLAG3            0x00000008
#define BITM_INTC_INTCFLAG0_FLAG2            0x00000004
#define BITM_INTC_INTCFLAG0_FLAG1            0x00000002
#define BITM_INTC_INTCFLAG0_FLAG0            0x00000001

/* -------------------------------------------------------------------------------------------------------------------------
          INTC_INTCFLAG1                       Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_INTC_INTCFLAG1_FLAG31           31
#define BITP_INTC_INTCFLAG1_FLAG30           30
#define BITP_INTC_INTCFLAG1_FLAG29           29
#define BITP_INTC_INTCFLAG1_FLAG28           28
#define BITP_INTC_INTCFLAG1_FLAG27           27
#define BITP_INTC_INTCFLAG1_FLAG26           26
#define BITP_INTC_INTCFLAG1_FLAG25           25
#define BITP_INTC_INTCFLAG1_FLAG24           24
#define BITP_INTC_INTCFLAG1_FLAG23           23
#define BITP_INTC_INTCFLAG1_FLAG22           22
#define BITP_INTC_INTCFLAG1_FLAG21           21
#define BITP_INTC_INTCFLAG1_FLAG20           20
#define BITP_INTC_INTCFLAG1_FLAG19           19
#define BITP_INTC_INTCFLAG1_FLAG18           18
#define BITP_INTC_INTCFLAG1_FLAG17           17
#define BITP_INTC_INTCFLAG1_FLAG16           16
#define BITP_INTC_INTCFLAG1_FLAG15           15
#define BITP_INTC_INTCFLAG1_FLAG14           14
#define BITP_INTC_INTCFLAG1_FLAG13           13
#define BITP_INTC_INTCFLAG1_FLAG12           12            /*  Custom IRQ 3 Status */
#define BITP_INTC_INTCFLAG1_FLAG11           11            /*  Custom IRQ 2 Status */
#define BITP_INTC_INTCFLAG1_FLAG10           10            /*  Custom IRQ 1 Status */
#define BITP_INTC_INTCFLAG1_FLAG9             9            /*  Custom IRQ 0 Status */
#define BITP_INTC_INTCFLAG1_FLAG8             8            /*  Variance IRQ status. */
#define BITP_INTC_INTCFLAG1_FLAG7             7
#define BITP_INTC_INTCFLAG1_FLAG6             6
#define BITP_INTC_INTCFLAG1_FLAG5             5
#define BITP_INTC_INTCFLAG1_FLAG4             4
#define BITP_INTC_INTCFLAG1_FLAG3             3
#define BITP_INTC_INTCFLAG1_FLAG2             2
#define BITP_INTC_INTCFLAG1_FLAG1             1
#define BITP_INTC_INTCFLAG1_FLAG0             0
#define BITM_INTC_INTCFLAG1_FLAG31           0x80000000
#define BITM_INTC_INTCFLAG1_FLAG30           0x40000000
#define BITM_INTC_INTCFLAG1_FLAG29           0x20000000
#define BITM_INTC_INTCFLAG1_FLAG28           0x10000000
#define BITM_INTC_INTCFLAG1_FLAG27           0x08000000
#define BITM_INTC_INTCFLAG1_FLAG26           0x04000000
#define BITM_INTC_INTCFLAG1_FLAG25           0x02000000
#define BITM_INTC_INTCFLAG1_FLAG24           0x01000000
#define BITM_INTC_INTCFLAG1_FLAG23           0x00800000
#define BITM_INTC_INTCFLAG1_FLAG22           0x00400000
#define BITM_INTC_INTCFLAG1_FLAG21           0x00200000
#define BITM_INTC_INTCFLAG1_FLAG20           0x00100000
#define BITM_INTC_INTCFLAG1_FLAG19           0x00080000
#define BITM_INTC_INTCFLAG1_FLAG18           0x00040000
#define BITM_INTC_INTCFLAG1_FLAG17           0x00020000
#define BITM_INTC_INTCFLAG1_FLAG16           0x00010000
#define BITM_INTC_INTCFLAG1_FLAG15           0x00008000
#define BITM_INTC_INTCFLAG1_FLAG14           0x00004000
#define BITM_INTC_INTCFLAG1_FLAG13           0x00002000
#define BITM_INTC_INTCFLAG1_FLAG12           0x00001000    /*  Custom IRQ 3 Status */
#define BITM_INTC_INTCFLAG1_FLAG11           0x00000800    /*  Custom IRQ 2 Status */
#define BITM_INTC_INTCFLAG1_FLAG10           0x00000400    /*  Custom IRQ 1 Status */
#define BITM_INTC_INTCFLAG1_FLAG9            0x00000200    /*  Custom IRQ 0 Status */
#define BITM_INTC_INTCFLAG1_FLAG8            0x00000100    /*  Variance IRQ status. */
#define BITM_INTC_INTCFLAG1_FLAG7            0x00000080
#define BITM_INTC_INTCFLAG1_FLAG6            0x00000040
#define BITM_INTC_INTCFLAG1_FLAG5            0x00000020
#define BITM_INTC_INTCFLAG1_FLAG4            0x00000010
#define BITM_INTC_INTCFLAG1_FLAG3            0x00000008
#define BITM_INTC_INTCFLAG1_FLAG2            0x00000004
#define BITM_INTC_INTCFLAG1_FLAG1            0x00000002
#define BITM_INTC_INTCFLAG1_FLAG0            0x00000001
/**
 * @} AD5940RegistersBitfields
 * */

/**
 * @addtogroup SPI_Block
 * @{
 *    @defgroup SPI_Block_Const
 *    @{
 *
*/
/* SPI Module */
typedef enum {
	SPI_POINTER_SPI   =  0,  /**< Base address set to SPI block. 16B Peripheral*/
	SPI_POINTER_AGPIO =  1,  /**< Base address set to AGPIO block. 16B Peripheral */
	SPI_POINTER_AFECON=  2,  /**< Base address set to AFECON block. 16B Peripheral */
	SPI_POINTER_WUPTMR=  4,  /**< Base address set to Wakeup Timer block. 16B Peripheral */
	SPI_POINTER_ALLON =  6,  /**< Base address set to Always on block. 16B Peripheral */
	SPI_POINTER_INTC  =  7,  /**< Base address set to INTC(interrupt controller) block. 32B Peripheral */
	SPI_POINTER_AFE   =  11, /**< Base address set to AFE block. 32B Peripheral */
	SPI_POINTER_USER0 =  13, /**< User defined base address 0 */
	SPI_POINTER_USER1 =  14, /**< User defined base address 1 */
	SPI_POINTER_USER2 =  15, /**< User defined base address 2 */
} SPIPointer_Type;

#define SPICMD_SETADDR	0x20
#define SPICMD_READREG	0x6d
#define SPICMD_WRITEREG	0x2d
#define SPICMD_READFIFO	0x5f
/**
 * @} SPI_Block_Const
 * @} SPI_Block
*/

/**
 * @addtogroup AFE_Control
 * @{
 * */

/**
 * @defgroup AFE_Control_Const
 * @{
 * */

/**
 * @defgroup AFEINTC_Const
 * @brief AD5940 has two interrupt controller INTC0 and INTC1. Both of them have ability to generate interrupt signal from GPIO.
 * @{
 * */
/* AFE Interrupt controller selection */
#define AFEINTC_0                   0   /**< Interrupt controller 0 */
#define AFEINTC_1                   1   /**< Interrupt controller 1 */
/** @} */

/**
 * @defgroup AFEINTC_SRC_Const
 * @brief Interrupt source selection. These sources are defined as bitmask. They are available for register INTCCLR, INTCSEL0/1, INTCFLAG0/1
 * @{
 * */
#define AFEINTSRC_ADCRDY            0x00000001  /**<  Bit0, ADC Result Ready Status */
#define AFEINTSRC_DFTRDY            0x00000002  /**<  Bit1, DFT Result Ready Status */
#define AFEINTSRC_SINC2RDY          0x00000004  /**<  Bit2, SINC2/Low Pass Filter Result Status */
#define AFEINTSRC_TEMPRDY           0x00000008  /**<  Bit3, Temp Sensor Result Ready */
#define AFEINTSRC_ADCMINERR         0x00000010	/**<  Bit4, ADC Minimum Value */
#define AFEINTSRC_ADCMAXERR         0x00000020	/**<  Bit5, ADC Maximum Value */
#define AFEINTSRC_ADCDIFFERR        0x00000040  /**<  Bit6, ADC Delta Ready */
#define AFEINTSRC_MEANRDY           0x00000080	/**<  Bit7, Mean Result Ready */
#define AFEINTSRC_VARRDY            0x00000100	/**<  Bit8, Variance Result Ready */
#define AFEINTSRC_CUSTOMINT0        0x00000200  /**<  Bit9,  Custom interrupt source 0. It happens when **sequencer** writes 1 to register AFEGENINTSTA.BIT0 */
#define AFEINTSRC_CUSTOMINT1        0x00000400  /**<  Bit10, Custom interrupt source 1. It happens when **sequencer** writes 1 to register AFEGENINTSTA.BIT1*/
#define AFEINTSRC_CUSTOMINT2        0x00000800  /**<  Bit11, Custom interrupt source 2. It happens when **sequencer** writes 1 to register AFEGENINTSTA.BIT2 */
#define AFEINTSRC_CUSTOMINT3        0x00001000  /**<  Bit12, Custom interrupt source 3. It happens when **sequencer** writes 1 to register AFEGENINTSTA.BIT3 */
#define AFEINTSRC_BOOTLDDONE        0x00002000  /**<  Bit13, OTP Boot Loading Done */
#define AFEINTSRC_WAKEUP            0x00004000  /**<  Bit14, AFE Woken up*/
#define AFEINTSRC_ENDSEQ    	      0x00008000  /**<  Bit15, End of Sequence Interrupt. */
#define AFEINTSRC_SEQTIMEOUT   	    0x00010000  /**<  Bit16, Sequencer Timeout Command Finished. */
#define AFEINTSRC_SEQTIMEOUTERR     0x00020000  /**<  Bit17, Sequencer Timeout Command Error. */
#define AFEINTSRC_CMDFIFOFULL       0x00040000  /**<  Bit18, Command FIFO Full Interrupt. */
#define AFEINTSRC_CMDFIFOEMPTY      0x00080000  /**<  Bit19, Command FIFO Empty */
#define AFEINTSRC_CMDFIFOTHRESH     0x00100000  /**<  Bit20, Command FIFO Threshold Interrupt. */
#define AFEINTSRC_CMDFIFOOF         0x00200000  /**<  Bit21, Command FIFO Overflow Interrupt. */
#define AFEINTSRC_CMDFIFOUF         0x00400000  /**<  Bit22, Command FIFO Underflow Interrupt. */
#define AFEINTSRC_DATAFIFOFULL      0x00800000  /**<  Bit23, Data FIFO Full Interrupt. */
#define AFEINTSRC_DATAFIFOEMPTY     0x01000000  /**<  Bit24, Data FIFO Empty */
#define AFEINTSRC_DATAFIFOTHRESH    0x02000000  /**<  Bit25, Data FIFO Threshold Interrupt. */
#define AFEINTSRC_DATAFIFOOF        0x04000000  /**<  Bit26, Data FIFO Overflow Interrupt. */
#define AFEINTSRC_DATAFIFOUF        0x08000000  /**<  Bit27, Data FIFO Underflow Interrupt. */
#define AFEINTSRC_WDTIRQ            0x10000000  /**<  Bit28, WDT Timeout Interrupt. */
#define AFEINTSRC_CRC_OUTLIER       0x20000000  /**<  Bit29, CRC interrupt for M355, Outlier Int for AD5940  */
#define AFEINTSRC_GPT0INT_SLPWUT    0x40000000  /**<  Bit30, Gneral Pupose Timer0 IRQ for M355. Sleep or Wakeup Tiemr timeout for AD5940*/
#define AFEINTSRC_GPT1INT_TRYBRK    0x80000000  /**<  Bit31, Gneral Pupose Timer1 IRQ for M355. Tried to Break IRQ for AD5940*/
#define AFEINTSRC_ALLINT            0xffffffff
/** @} */

/**
 * @defgroup AFEPWR_Const
 * @brief AFE power mode.
 * @details It will set the whold analog system power mode include HSDAC, Excitation Buffer, HSTIA, ADC front-buffer etc.
 * @{
*/
#define AFEPWR_LP                   0   /**< Set AFE to Low Power mode. For signal <80kHz, use it. */
#define AFEPWR_HP                   1   /**< Set AFE to High Power mode. For signal >80kHz, use it. */
/**
 * @}
*/

/**
 * @defgroup AFEBW_Const
 * @brief AFE system bandwidth.
 * @details It will set the whold analog bandwitdh include HSDAC, Excitation Buffer, HSTIA, ADC front-buffer etc.
 * @{
*/
#define AFEBW_NOACT                 0   /**< Keep current setting */
#define AFEBW_50KHZ                 1   /**< 50kHZ system bandwidth(DAC/ADC) */
#define AFEBW_100KHZ                2   /**< 100kHZ system bandwidth(DAC/ADC) */
#define AFEBW_250KHZ                3   /**< 250kHZ system bandwidth(DAC/ADC) */
/**
 * @}
*/

/**
 * @defgroup AFECTRL_Const
 * @brief AFE Conrol signal set. Bit masks for register AFECON.
 * @details This is all the available control signal for function @ref AD5940_AFECtrlS
 * @warning Bit field in register AFECON has some opposite meaning as below definitions. We use all positive word here
 *          like HPREF instead of HPREFDIS. This set is only used in function @ref AD5940_AFECtrlS, the second parameter
 *          decides whether enable it or disable it.
 * @{
*/
#define AFECTRL_HPREFPWR            (1<<5)    /**< High power reference on-off control */
#define AFECTRL_HSDACPWR            (1<<6)    /**< High speed DAC on-off control */
#define AFECTRL_ADCPWR              (1<<7)    /**< ADC power on-off control */
#define AFECTRL_ADCCNV              (1<<8)    /**< Start ADC convert enable */
#define AFECTRL_EXTBUFPWR           (1<<9)    /**< Excitation buffer power control */
#define AFECTRL_INAMPPWR            (1<<10)   /**< Excitation loop input amplifier before P/N node power control */
#define AFECTRL_HSTIAPWR            (1<<11)   /**< High speed TIA amplifier power control */
#define AFECTRL_TEMPSPWR            (1<<12)   /**< Temperature sensor power */
#define AFECTRL_TEMPCNV             (1<<13)   /**< Start Temperature sensor convert */
#define AFECTRL_WG                  (1<<14)   /**< Waveform generator on-off control */
#define AFECTRL_DFT                 (1<<15)   /**< DFT engine on-off control */
#define AFECTRL_SINC2NOTCH          (1<<16)	  /**< SIN2+Notch block on-off control */
#define AFECTRL_ALDOLIMIT           (1<<19)	  /**< ALDO current limit on-off control */
#define AFECTRL_DACREFPWR           (1<<20)	  /**< DAC reference buffer power control */
#define AFECTRL_DCBUFPWR            (1<<21)	  /**< Excitation loop DC offset buffer sourced from LPDAC power control */
#define AFECTRL_ALL                 0x39ffe0  /**< All control signals */
/**
 * @}
*/

/**
 * @defgroup LPMODECTRL_Const
 * @brief   LP Control signal(bit mask) for register LPMODECON
 * @details  This is all the available control signal for function @ref AD5940_LPModeCtrlS
 * @warning Bit field in register LPMODECON has some opposite meaning as below definitions. We use all positive word here
 *          like HPREFPWR instead of HPREFDIS. This set is only used in function @ref AD5940_AFECtrlS, the second parameter
 *          decides whether enable or disable selected block(s).
 * @{
*/
#define LPMODECTRL_HFOSCEN             (1<<0)  /**< Enable internal HFOSC. Note: the register defination is set this bit to 1 to disable it. */
#define LPMODECTRL_HPREFPWR            (1<<1)  /**< High power reference power EN. Note: the register defination is set this bit to 1 to disable it. */
#define LPMODECTRL_ADCCNV              (1<<2)  /**< Start ADC convert enable */
#define LPMODECTRL_REPEATEN            (1<<3)  /**< Enable repeat convert function. This will enable ADC power automatically */
#define LPMODECTRL_GLBBIASZ            (1<<4)  /**< Enable Global ZTAT bias. Disable it to save more power */
#define LPMODECTRL_GLBBIASP            (1<<5)  /**< Enable Global PTAT bias. Disable it to save more power */
#define LPMODECTRL_BUFHP1P8V           (1<<6)  /**< High power 1.8V reference buffer */
#define LPMODECTRL_BUFHP1P1V           (1<<7)  /**< High power 1.1V reference buffer */
#define LPMODECTRL_ALDOPWR             (1<<8)  /**< Enable ALDO. Note: register defination is set this bit to 1 to disable ALDO. */
#define LPMODECTRL_ALL                 0x1ff   /**< All Control signal Or'ed together*/
#define LPMODECTRL_NONE                0       /**< No blocks selected */
/** @} */

/**
 * @defgroup AFERESULT_Const
 * @brief The available AFE results type. Used for function @ref AD5940_ReadAfeResult
 * @{
*/
enum ad5940_afe_result {
	AFERESULT_SINC3, /**< SINC3 result */
	AFERESULT_SINC2, /**< SINC2+NOTCH result */
	AFERESULT_TEMPSENSOR,  /**< Temperature sensor result */
	AFERESULT_DFTREAL, /**< DFT Real result */
	AFERESULT_DFTIMAGE, /**< DFT Imaginary result */
	AFERESULT_STATSMEAN, /**< Statistic Mean result */
	AFERESULT_STATSVAR, /**< Statistic Variance result */
	AFERESULT_COUNT
};
/** @} */

/**
 * @} AFE_Control_Const
 * @} AFE_Control
 * */

/**
 * @addtogroup High_Speed_Loop
 * @{
 *    @defgroup High_Speed_Loop_Const
 *    @{
*/

/**
 * @defgroup Switch_Matrix_Block_Const
 * @{
 *    @defgroup SWD_Const
 *    @brief Switch D set. This is bitmask for register DSWFULLCON.
 *    @details
 *        It's used to initialize structure @ref SWMatrixCfg_Type
 *        The bitmasks can be OR'ed together. For example
 *          - `SWD_AIN1|SWD_RCAL0` means close SWD_AIN1 and SWD_RCAL0 in same time, and open all other D switches.
 *          - `SWD_AIN2` means close SWD_AIN2 and open all other D switches.
 *    @{
*/
#define SWD_OPEN                    (0<<0)
#define SWD_RCAL0                   (1<<0)
#define SWD_AIN1                    (1<<1)
#define SWD_AIN2                    (1<<2)
#define SWD_AIN3                    (1<<3)
#define SWD_CE0                     (1<<4)
//#define SWD_CE1                     (1<<5)  /** @todo add Switch D configuration for new added pins */
#define SWD_SE0                     (1<<6)
//#define SWD_SE1                     (1<<7)
/** @} */

/**
 * @defgroup SWP_Const
 * @brief Switch P set. This is bitmask for register PSWFULLCON.
 * @details
 *        It's used to initialize structure @ref SWMatrixCfg_Type.
 *        The bitmasks can be OR'ed together. For example
 *          - `SWP_RCAL0|SWP_AIN1` means close SWP_RCAL0 and SWP_AIN1 in same time, and open all other P switches.
 *          - `SWP_SE0` means close SWP_SE0 and open all other P switches.
 * @{
*/
#define SWP_OPEN                    0       /* Open all P switches */
#define SWP_RCAL0                   (1<<0)
#define SWP_AIN1                    (1<<1)
#define SWP_AIN2                    (1<<2)
#define SWP_AIN3                    (1<<3)
#define SWP_RE0                     (1<<4)
#define SWP_RE1                     (1<<5)
#define SWP_SE0                     (1<<6)
#define SWP_DE0                     (1<<7)
//#define SWP_SE1                     (1<<8)
//#define SWP_DE1                     (1<<9)
#define SWP_CE0                     (1<<10)
//#define SWP_CE1                     (1<<11)
#define SWP_PL                      (1<<13)
#define SWP_PL2                     (1<<14)
/** @} */

/**
 * @defgroup SWN_Const
 * @brief Switch N set. This is bitmask for register NSWFULLCON.
 * @details
 *        It's used to initialize structure @ref SWMatrixCfg_Type.
 *        The bitmasks can be OR'ed together. For example
 *          - `SWN_RCAL0|SWN_AIN1` means close SWN_RCAL0 and SWN_AIN1 in same time, and open all other N switches.
 *          - `SWN_SE0` means close SWN_SE0 and open all other N switches.
 * @{
*/
#define SWN_OPEN                    0       /**< Open all N switches */
#define SWN_RCAL1                   (1<<9)
#define SWN_AIN0                    (1<<0)
#define SWN_AIN1                    (1<<1)
#define SWN_AIN2                    (1<<2)
#define SWN_AIN3                    (1<<3)
#define SWN_SE0LOAD                 (1<<4)  /**< SE0_LOAD is different from PIN SE0. It's the point after 100Ohm load resistor */
#define SWN_DE0LOAD                 (1<<5)
#define SWN_SE1LOAD                 (1<<6)
#define SWN_DE1LOAD                 (1<<7)
#define SWN_SE0                     (1<<8)  /**< SE0 here means the PIN SE0. */
#define SWN_NL                      (1<<10)
#define SWN_NL2                     (1<<11)
/** @} */

/**
 * @defgroup SWT_Const
 * @brief Switch T set. This is bitmask for register TSWFULLCON.
 * @details
 *        It's used to initialize structure @ref SWMatrixCfg_Type.
 *        The bitmasks can be OR'ed together. For example
 *          - SWT_RCAL0|SWT_AIN1 means close SWT_RCAL0 and SWT_AIN1 in same time, and open all other T switches.
 *          - SWT_SE0LOAD means close SWT_SE0LOAD and open all other T switches.
 * @{
*/
#define SWT_OPEN                    0         /**< Open all T switches */
#define SWT_RCAL1                   (1<<11)
#define SWT_AIN0                    (1<<0)
#define SWT_AIN1                    (1<<1)
#define SWT_AIN2                    (1<<2)
#define SWT_AIN3                    (1<<3)
#define SWT_SE0LOAD                 (1<<4)    /**< SE0_LOAD is different from PIN SE0. It's the point after 100Ohm load resistor */
#define SWT_DE0                     (1<<5)    /**< Metal edit */
#define SWT_SE1LOAD                 (1<<6)
#define SWT_DE1                     (1<<7)    /**< Metal edit */
#define SWT_TRTIA                   (1<<8)    /**< T9 switch. Connect RTIA to T matrix */
#define SWT_DE0LOAD                 (1<<9)
#define SWT_DE1LOAD                 (1<<10)
/** @} */

/** @} Switch_Matrix_Block_Const */


/**
 * @defgroup Waveform_Generator_Block_Const
 * @{
*/
/**
 * @defgroup WGTYPE_Const
 * @brief Waveform generator signal type
 * @{
*/
#define WGTYPE_MMR                  0 /**< Direct write to DAC using register */
#define WGTYPE_SIN                  2 /**< Sine wave generator */
#define WGTYPE_TRAPZ                3 /**< Trapezoid generator */
/** @} */
/** @} Waveform_Generator_Block_Const */

/**
 * @defgroup HSDAC_Block_Const
 * @{
*/
/* Excitation buffer gain selection */
/**
 * @defgroup EXCITBUFGAIN_Const
 * @{
*/
#define EXCITBUFGAIN_2              0   /**< Excitation buffer gain is x2 */
#define EXCITBUFGAIN_0P25           1   /**< Excitation buffer gain is x1/4 */
/** @} */

/**
 * @defgroup HSDACGAIN_Const
 * @{
*/
/* HSDAC PGA Gain selection(DACCON.BIT0) */
#define HSDACGAIN_1                 0   /**< Gain is x1 */
#define HSDACGAIN_0P2               1   /**< Gain is x1/5 */
/** @} */
/** @} */ //HSDAC_Block_Const

/**
 * @defgroup HSTIA_Block_Const
 * @{
 * */
/* HSTIA Amplifier Positive Input selection */

/**
 * @defgroup HSTIABIAS_Const
 * @warning When select Vzero0 as bias, close LPDAC switch<xxx>
 * @todo add LPDAC switch description.
 * @{
*/
#define HSTIABIAS_1P1               0   /**< Internal 1.1V common voltage from internal 1.1V reference buffer */
#define HSTIABIAS_VZERO0            1   /**< From LPDAC0 Vzero0 output */
/** @} */


/* HSTIA Internal RTIA selection */

/**
 * @defgroup HSTIARTIA_Const
 * @{
*/
#define HSTIARTIA_200               0
#define HSTIARTIA_1K                1
#define HSTIARTIA_5K                2
#define HSTIARTIA_10K               3
#define HSTIARTIA_20K               4
#define HSTIARTIA_40K               5
#define HSTIARTIA_80K               6
#define HSTIARTIA_160K              7
#define HSTIARTIA_OPEN              8
/** @} */

/**
 * @defgroup HSTIADERTIA_Const
 * @todo add notes for DE node RTIA settings.
 * @{
*/
#define HSTIADERTIA_50              0     /**< Settings depends on RLOAD resistor. */
#define HSTIADERTIA_100             1
#define HSTIADERTIA_200             2
#define HSTIADERTIA_1K              3     /**< set bit[7:3] to 0x0b(11) */
#define HSTIADERTIA_5K              4     /**< set bit[7:3] to 0x0c(12) */
#define HSTIADERTIA_10K             5     /**< set bit[7:3] to 0x0d(13) */
#define HSTIADERTIA_20K             6     /**< set bit[7:3] to 0x0e(14) */
#define HSTIADERTIA_40K             7     /**< set bit[7:3] to 0x0f(15) */
#define HSTIADERTIA_80K             8     /**< set bit[7:3] to 0x10(16) */
#define HSTIADERTIA_160K            9     /**< set bit[7:3] to 0x11(17) */
#define HSTIADERTIA_TODE            10    /**< short HSTIA output to DE0 pin. set bit[7:3] to 0x12(18) */
#define HSTIADERTIA_OPEN            11    /**< Default state is set to OPEN RTIA by setting bit[7:3] to 0x1f */
/** @} */

/* HSTIA DE0 Terminal internal RLOAD selection */
/**
 * @defgroup HSTIADERLOAD_Const
 * @{
*/
#define HSTIADERLOAD_0R             0     /**< set bit[2:0] to 0x00 */
#define HSTIADERLOAD_10R            1     /**< set bit[2:0] to 0x01 */
#define HSTIADERLOAD_30R            2     /**< set bit[2:0] to 0x02 */
#define HSTIADERLOAD_50R            3     /**< set bit[2:0] to 0x03 */
#define HSTIADERLOAD_100R           4     /**< set bit[2:0] to 0x04 */
#define HSTIADERLOAD_OPEN           5     /**< RLOAD open means open switch between HSTIA negative input and Rload resistor(<S1>).Default state is OPEN RLOAD by setting HSTIARES03CON[2:0] to 0x5, 0x6 or 0x7 */
/** @} */

/** @} HSTIA_Block_Const */
/**
 * @} High_Speed_Loop_Const
 * @} High_Speed_Loop
*/

/**
 * @addtogroup Low_Power_Loop
 * @{
 *    @defgroup Low_Power_Loop_Const
 *              Low power includes low power DAC and two low power amplifiers(PA and TIA)
 *    @{
*/

/**
 * @defgroup LPDAC_Block_Const
 * @{
 * */
/**
 * @defgroup LPDACSRC_Const
 * @{
*/
#define LPDACSRC_MMR                0   /**< Get data from register REG_AFE_LPDACDAT0DATA0 */
#define LPDACSRC_WG                 1   /**< Get data from waveform generator */
/** @} */

/**
 * @defgroup LPDACSW_Const
 * @brief LPDAC switch settings
 * @{
*/
#define LPDACSW_VBIAS2LPPA        0x10  /**< switch between LPDAC Vbias output and LPPA(low power PA(Potential Amplifier)) */
#define LPDACSW_VBIAS2PIN         0x08  /**< Switch between LPDAC Vbias output and Vbias pin */
#define LPDACSW_VZERO2LPTIA       0x04  /**< Switch between LPDAC Vzero output and LPTIA positive input */
#define LPDACSW_VZERO2PIN         0x02  /**< Switch between LPDAC Vzero output and Vzero pin */
#define LPDACSW_VZERO2HSTIA       0x01  /**< Switch between LPDAC Vzero output and HSTIA positive input MUX */
/** @} */

/**
 * @defgroup LPDACVZERO_Const
 * @brief Vzero Mux selection
 * @{
*/
#define LPDACVZERO_6BIT             0   /**< Connect Vzero to 6bit LPDAC output */
#define LPDACVZERO_12BIT            1   /**< Connect Vzero to 12bit LPDAC output */
/** @} */

/**
 * @defgroup LPDACVBIAS_Const
 * @brief Vbias Mux selection
 * @{
*/
#define LPDACVBIAS_6BIT             1   /**< Connect Vbias to 6bit LPDAC output */
#define LPDACVBIAS_12BIT            0   /**< Connect Vbias to 12bit LPDAC output */
/** @} */


/**
 * @defgroup LPDACREF_Const
 * @brief LPDAC reference selection
 * @{
*/
#define LPDACREF_2P5                0   /**< Internal 2.5V reference */
#define LPDACREF_AVDD               1   /**< Use AVDD as reference */
/** @} */

/** @} */ //LPDAC_Block_Const

/**
 * @defgroup LPAMP_Block_Const
 * @brief Low power amplifies include potential-state amplifire(PA in short) and TIA.
 * @{
 * */

/**
 * @defgroup LPTIARF_Const
 * @brief LPTIA LPF Resistor selection
 * @{
 * */
#define LPTIARF_OPEN                0   /**< Disconnect Rf resistor */
#define LPTIARF_SHORT               1   /**< Bypass Rf resistor */
#define LPTIARF_20K                 2   /**< 20kOhm Rf */
#define LPTIARF_100K                3
#define LPTIARF_200K                4
#define LPTIARF_400K                5
#define LPTIARF_600K                6
#define LPTIARF_1M                  7
/** @} */

/**
 * @defgroup LPTIARLOAD_Const
 * @brief LPTIA Rload Selection
 * @{
*/
#define LPTIARLOAD_SHORT            0   /**< 0Ohm Rload */
#define LPTIARLOAD_10R              1   /**< 10Ohm Rload */
#define LPTIARLOAD_30R              2
#define LPTIARLOAD_50R              3
#define LPTIARLOAD_100R             4
#define LPTIARLOAD_1K6              5   /**< Only available when RTIA setting >= 2KOHM */
#define LPTIARLOAD_3K1              6   /**< Only available when RTIA setting >= 4KOHM */
#define LPTIARLOAD_3K6              7   /**< Only available when RTIA setting >= 4KOHM */
/** @} */

/**
 * @defgroup LPTIARTIA_Const
 * @brief LPTIA RTIA Selection
 * @note The real RTIA resistor value dependents on Rload settings.
 * @todo Add explanation of relation between Rload and RTIA.
 * @{
*/
#define LPTIARTIA_OPEN              0   /**< Disconnect LPTIA Internal RTIA */
#define LPTIARTIA_200R              1   /**< 200Ohm Internal RTIA */
#define LPTIARTIA_1K                2   /**< 1KOHM */
#define LPTIARTIA_2K                3   /**< 2KOHM */
#define LPTIARTIA_3K                4   /**< 3KOHM */
#define LPTIARTIA_4K                5   /**< 4KOHM */
#define LPTIARTIA_6K                6   /**< 6KOHM */
#define LPTIARTIA_8K                7   /**< 8KOHM */
#define LPTIARTIA_10K               8   /**< 10KOHM */
#define LPTIARTIA_12K               9   /**< 12KOHM */
#define LPTIARTIA_16K               10  /**< 16KOHM */
#define LPTIARTIA_20K               11  /**< 20KOHM */
#define LPTIARTIA_24K               12  /**< 24KOHM */
#define LPTIARTIA_30K               13  /**< 30KOHM */
#define LPTIARTIA_32K               14  /**< 32KOHM */
#define LPTIARTIA_40K               15  /**< 40KOHM */
#define LPTIARTIA_48K               16  /**< 48KOHM */
#define LPTIARTIA_64K               17  /**< 64KOHM */
#define LPTIARTIA_85K               18  /**< 85KOHM */
#define LPTIARTIA_96K               19  /**< 96KOHM */
#define LPTIARTIA_100K              20  /**< 100KOHM */
#define LPTIARTIA_120K              21  /**< 120KOHM */
#define LPTIARTIA_128K              22  /**< 128KOHM */
#define LPTIARTIA_160K              23  /**< 160KOHM */
#define LPTIARTIA_196K              24  /**< 196KOHM */
#define LPTIARTIA_256K              25  /**< 256KOHM */
#define LPTIARTIA_512K              26  /**< 512KOHM */
/** @} */

/**
 * @defgroup LPAMPPWR_Const
 * @brief Low power amplifier(PA and TIA) power mode selection.
 * @{
*/
#define LPAMPPWR_NORM               0   /**< Normal Power mode */
#define LPAMPPWR_BOOST1             1   /**< @todo add explanation for power mode Boost PA powrer? */
#define LPAMPPWR_BOOST2             2
#define LPAMPPWR_BOOST3             3
#define LPAMPPWR_HALF               4   /**< Put PA and TIA in half power mode */
/** @} */

#define LPTIASW(n)                  (1L<<n) /**< LPTIA switch control. Use this macro to set @ref LPAmpCfg_Type  */

/**
 * @} LPAMP_Block_Const
 * @} Low_Power_Loop_Const
 * @} Low_Power_Loop
 *
 * */

/**
 * @addtogroup DSP_Block
 * @{
 *    @defgroup DSP_Block_Const
 *    @{
 *        @defgroup ADC_Block_Const
 *        @{
 */

/**
 * @defgroup ADCPGA_Const
 * @brief ADC PGA Selection
 * @{
*/
#define ADCPGA_1                    0
#define ADCPGA_1P5                  1
#define ADCPGA_2                    2
#define ADCPGA_4                    3
#define ADCPGA_9                    4
#define IS_ADCPGA(pga)              (((pga) == ADCPGA_1) ||\
                                    (pga) == ADCPGA_1P5) ||\
                                    (pga) == ADCPGA_2) ||\
                                    (pga) == ADCPGA_4) ||\
                                    (pga) == ADCPGA_9))
/**
 * @}
 * */

/**
 * @defgroup ADCMUXP_Const
 * @brief ADC Channel P Configuration
 * @{
*/
#define ADCMUXP_FLOAT               0x0
#define ADCMUXP_HSTIA_P             0x1
#define ADCMUXP_AIN0                0x4
#define ADCMUXP_AIN1                0x5
#define ADCMUXP_AIN2                0x6
#define ADCMUXP_AIN3                0x7
#define ADCMUXP_AVDD_2              0x8
#define ADCMUXP_DVDD_2              0x9
#define ADCMUXP_AVDDREG             0xA     /**< AVDD internal regulator output. It's around 1.8V */
#define ADCMUXP_TEMP                0xB     /**< Internal temperature output */
#define ADCMUXP_VSET1P1             0xC     /**< Internal 1.1V bias voltage */
#define ADCMUXP_VDE0                0xD     /**< Voltage of DE0 pin  */
#define ADCMUXP_VSE0                0xE     /**< Voltage of SE0 pin  */
#define ADCMUXP_VREF2P5             0x10    /**< The internal 2.5V reference buffer output. */
#define ADCMUXP_VREF1P8DAC          0x12    /**< HSDAC 1.8V internal reference. It's only available when both AFECON.BIT20 and AFECON.BIT6 are set. */
#define ADCMUXP_TEMPN               0x13    /**< Internal temperature output */
#define ADCMUXP_AIN4                0x14    /**< Voltage of AIN4/LPF0 pin  */
#define ADCMUXP_AIN6                0x16    /**< Voltage of AIN6 pin, not available on AD5941  */
#define ADCMUXP_VZERO0              0x17    /**< Voltage of Vzero0 pin  */
#define ADCMUXP_VBIAS0              0x18    /**< Voltage of Vbias0 pin  */
#define ADCMUXP_VCE0                0x19
#define ADCMUXP_VRE0                0x1A
#define ADCMUXP_VCE0_2              0x1F    /**< VCE0 divide by 2 */
#define ADCMUXP_LPTIA0_P            0x21
#define ADCMUXP_AGND                0x23
#define ADCMUXP_P_NODE              0x24    /**< Buffered voltage of excitation buffer P node.  */
/**@}*/

/**
 * @defgroup ADCMUXN_Const
 * @brief ADC Channel N Configuration
 * @{
*/
#define ADCMUXN_FLOAT               0x0
#define ADCMUXN_HSTIA_N             0x1
#define ADCMUXN_LPTIA0_N            0x2
#define ADCMUXN_AIN0                0x4
#define ADCMUXN_AIN1                0x5
#define ADCMUXN_AIN2                0x6
#define ADCMUXN_AIN3                0x7
#define ADCMUXN_VSET1P1             0x8
#define ADCMUXN_TEMPN               0xB
#define ADCMUXN_AIN4                0xC
#define ADCMUXN_AIN6                0xE
#define ADCMUXN_VZERO0              0x10
#define ADCMUXN_VBIAS0              0x11
#define ADCMUXN_N_NODE              0x14    /**< Buffered voltage of excitation buffer N node.  */
/** @} */

/**
 * @defgroup ADCRATE_Const
 * @brief ADC Current Sample Rate. If ADC clock is 32MHz, set it to ADCRATE_1P6MHZ. Otherwise, set it to ADCRATE_800KHZ.
 * @{
*/
#define ADCRATE_800KHZ              1
#define ADCRATE_1P6MHZ              0
#define IS_ADCRATE(rate)            (((rate) == ADCRATE_800KHZ) ||\
                                    (rate) == ADCRATE_1P6MHZ))
/** @} */

/**
 * @defgroup ADCSINC3OSR_Const
 * @brief ADC SINC3 Filter OSR. 2, 4 is recommended value. 5 is not recommended.
 * @{
*/
#define ADCSINC3OSR_2               2
#define ADCSINC3OSR_4               1
#define ADCSINC3OSR_5               0
#define IS_ADCSINC3OSR(osr)        (((osr) == ADCSINC3OSR_2) ||\
                                    (osr) == ADCSINC3OSR_4) ||\
                                    (osr) == ADCSINC3OSR_5))
/** @} */

/**
 * @defgroup ADCSINC2OSR_Const
 * @brief ADC SINC2 Filter OSR.
 * @{
*/
#define ADCSINC2OSR_22              0
#define ADCSINC2OSR_44              1
#define ADCSINC2OSR_89              2
#define ADCSINC2OSR_178             3
#define ADCSINC2OSR_267             4
#define ADCSINC2OSR_533             5
#define ADCSINC2OSR_640             6
#define ADCSINC2OSR_667             7
#define ADCSINC2OSR_800             8
#define ADCSINC2OSR_889             9
#define ADCSINC2OSR_1067            10
#define ADCSINC2OSR_1333            11
#define IS_ADCSINC2OSR(osr)        (((osr) == ADCSINC2OSR_22) ||\
                                    (osr) == ADCSINC2OSR_44) ||\
                                    (osr) == ADCSINC2OSR_89) ||\
                                    (osr) == ADCSINC2OSR_178) ||\
                                    (osr) == ADCSINC2OSR_267) ||\
                                    (osr) == ADCSINC2OSR_533) ||\
                                    (osr) == ADCSINC2OSR_640) ||\
                                    (osr) == ADCSINC2OSR_667) ||\
                                    (osr) == ADCSINC2OSR_800) ||\
                                    (osr) == ADCSINC2OSR_889) ||\
                                    (osr) == ADCSINC2OSR_1067) ||\
                                    (osr) == ADCSINC2OSR_1333))
/** @} */

/**
 * @defgroup ADCAVGNUM_Const
 * @brief ADC Average filter for DFT. The average block locates after SINC3 filter.
 *        The output of average filter is directly feed into DFT block.
 * @warning Once average filter is enalbed, DFT source is automatically changed to averaged data.
 * @{
*/
#define ADCAVGNUM_2                 0
#define ADCAVGNUM_4                 1
#define ADCAVGNUM_8                 2
#define ADCAVGNUM_16                3
#define IS_ADCAVGNUM(num)          (((num) == ADCAVGNUM_2) ||\
                                    (num) == ADCAVGNUM_4) ||\
                                    (num) == ADCAVGNUM_8) ||\
                                    (num) == ADCAVGNUM_16))
/** @} */

/** @} ADC_Block_Const */

/**
 * @defgroup DFT_Block_Const
 * @{
 * */

/**
 * @defgroup DFTSRC_Const
 * @brief DFT source selection. When average function is enabled, DFT source automatically switch to average output.
 * @{
 * */
#define DFTSRC_SINC2NOTCH           0   /**< SINC2+Notch filter block output. Bypass Notch to use SINC2 data */
#define DFTSRC_SINC3                1   /**< SINC3 filter */
#define DFTSRC_ADCRAW               2   /**< Raw ADC data */
#define DFTSRC_AVG                  3   /**< Average output of SINC3. */
/** @} */

/**
 * @defgroup DFTNUM_Const
 * @brief DFT number selection.
 * @{
 * */
#define DFTNUM_4                    0   /**< 4 Point */
#define DFTNUM_8                    1
#define DFTNUM_16                   2
#define DFTNUM_32                   3
#define DFTNUM_64                   4
#define DFTNUM_128                  5
#define DFTNUM_256                  6
#define DFTNUM_512                  7
#define DFTNUM_1024                 8
#define DFTNUM_2048                 9
#define DFTNUM_4096                 10
#define DFTNUM_8192                 11
#define DFTNUM_16384                12
/** @} */

/**
 * @} DFT_Block_Const
*/

/**
 * @defgroup Statistic_Block_Const
 * @{
  */
/**
 * @defgroup STATSAMPLE_Const
 * @brief The statistic module sample size. It decides how much data is used to do calculation.
 * @{
*/
#define STATSAMPLE_128              0   /**< Sample size 128 */
#define STATSAMPLE_64               1   /**< Sample size 64 */
#define STATSAMPLE_32               2
#define STATSAMPLE_16               3
#define STATSAMPLE_8                4
/** @} */

/* Statistic standard deviation configure */
/**
 * @defgroup STATDEV_Const
 * @brief The standard deviation configue
 * @todo Add explanation.
 * @{
*/
#define STATDEV_1                   1
#define STATDEV_4                   4
#define STATDEV_9                   9
#define STATDEV_16                  16
#define STATDEV_25                  25
/** @} */

/**
 * @} Statistic_Block_Const
 * @} DSP_Block_Const
 * @} DSP_Block
 *
*/

/**
 * @addtogroup Sequencer_FIFO
 * @{
 *    @defgroup Sequencer_FIFO_Const
 *    @brief This block includes sequencer and FIFO related all paramters.
 *    @{
*/

/**
 * @defgroup SEQID_Const
 * @{
*/
#define SEQID_0                     0     /**< Sequence0 */
#define SEQID_1                     1
#define SEQID_2                     2
#define SEQID_3                     3
/** @} */

/**
 * @defgroup SEQID_Const
 * @brief Sequencer memory size. SRAM is shared between FIFO and Sequencer
 * @warning The total available SRAM is 6kB. It's shared by FIFO and seuqncer.
 * @{
*/
#define SEQMEMSIZE_32B              0     /**< The selfbuild in 32Byte for sequencer. All 6kB SRAM  can be used for data FIFO */
#define SEQMEMSIZE_2KB              1     /**< Sequencer use 2kB. The reset 4kB can be used for data FIFO */
#define SEQMEMSIZE_4KB              2     /**< 4kB for Sequencer. 2kB for data FIFO */
#define SEQMEMSIZE_6KB              3     /**< All 6kB for Sequencer. Build in 32Bytes memory can be used for data FIFO */
/** @} */


/* Mode of GPIO detecting used for triggering sequence */
/**
 * @defgroup SEQGPIOTRIG_Const
 * @{
*/
#define SEQGPIOTRIG_RISING        0     /**< Rising edge */
#define SEQGPIOTRIG_FALLING       1     /**< Falling edge */
#define SEQGPIOTRIG_RISINGFALLING 2     /**< Rising or falling */
#define SEQGPIOTRIG_HIGHL         3     /**< High level */
#define SEQGPIOTRIG_LOW           4     /**< Low level */
/** @} */

/* Sequencer helper */
/**
 * @defgroup Sequencer_Helper
 * @{
*/

/* Three kinds of sequencer commands: wait, time-out, write */
/* Decoded by BIT[31:30] */
/**
 * Wait command. Wait some clocks-code Command Code: 'b00
 * @warning Maximum wait time is 0x3fffffff/System clock.
 */
#define SEQ_WAIT(ClkNum)            (0x00000000| ((uint32_t)(ClkNum)&0x3fffffff))

/**
 * Time-Out command. Set time-out count down value. Command Code: 'b01
 * @warning maximum time-out timer value is 0x3fffffff
 * */
#define SEQ_TOUT(ClkNum)            (0x40000000| ((uint32_t)(ClkNum)&0x3fffffff))

/**
 * Write register command. Command Code: 'b10 or 'b11
 * @warning Address range is 0x2000 to 0x21FF. Data is limited to 24bit width.
 * */
#define SEQ_WR(addr,data)           (0x80000000|(((((uint32_t)(addr))>>2)&0x7f)<<24)  \
                                        |(((uint32_t)(data))&0xffffff))

/* Some commands used frequently */
#define SEQ_NOP()                   SEQ_WAIT(0) /**< SEQ_NOP is just a simple wait command that wait one system clock */
#define SEQ_HALT()                  SEQ_WR(REG_AFE_SEQCON,0x12)   /**< Can halt sequencer. Used for debug */
#define SEQ_STOP()                  SEQ_WR(REG_AFE_SEQCON,0x00)   /**< Disable sequencer, this will generate End of Sequence interrupt */

#define SEQ_SLP()                   SEQ_WR(REG_AFE_SEQTRGSLP, 1)  /**< Trigger sleep. If sleep is allowed, AFE will go to sleep/hibernate mode */

#define SEQ_INT0()                  SEQ_WR(REG_AFE_AFEGENINTSTA, (1L<<0)) /**< Generate custom interrupt 0 */
#define SEQ_INT1()                  SEQ_WR(REG_AFE_AFEGENINTSTA, (1L<<1)) /**< Generate custom interrupt 1 */
#define SEQ_INT2()                  SEQ_WR(REG_AFE_AFEGENINTSTA, (1L<<2)) /**< Generate custom interrupt 2 */
#define SEQ_INT3()                  SEQ_WR(REG_AFE_AFEGENINTSTA, (1L<<3)) /**< Generate custom interrupt 3 */

/* Helper to calculate sequence length in array */
#define SEQ_LEN(n)                  (sizeof(n)/4)   /**< Calculate how many commands are in sepecified array. */
/** @} */ //Sequencer_Helper

/* FIFO */
/**
 * @defgroup FIFOMODE_Const
 * @{
*/
#define FIFOMODE_FIFO               0     /**< Standard FIFO mode. If FIFO is full, reject all comming data and put FIFO to fault state, report interrupt if enabled */
#define FIFOMODE_STREAM             1     /**< Stream mode. If FIFO is full, discard older data. Report FIFO is full interrupt if enabled */
/** @} */

/**
 * @defgroup FIFOSRC_Const
 * @{
*/
#define FIFOSRC_SINC3               0     /**< SINC3 data */
#define FIFOSRC_DFT                 2     /**< DFT real and imaginary part */
#define FIFOSRC_SINC2NOTCH          3     /**< SINC2+NOTCH block. Notch can be bypassed, so SINC2 data can be feed to FIFO */
#define FIFOSRC_VAR                 4     /**< Statistic variarance output */
#define FIFOSRC_MEAN                5     /**< Statistic mean output */
/** @} */

/**
 * @defgroup FIFOSIZE_Const
 * @brief Set FIFO size.
 * @warning The total available SRAM is 6kB. It's shared by FIFO and seuqncer.
 * @{
*/
#define FIFOSIZE_32B                0     /**< The selfbuild in 32Byte for data FIFO. All 6kB SRAM for sequencer */
#define FIFOSIZE_2KB                1     /**< DATA FIFO use 2kB. The reset 4kB is used for sequencer */
#define FIFOSIZE_4KB                2     /**< 4kB for Data FIFO. 2kB for sequencer */
#define FIFOSIZE_6KB                3     /**< All 6kB for Data FIFO. Build in 32Bytes memory for sequencer */
/** @} */

/* Wake up timer */
/**
 * @defgroup WUPTENDSEQ_Const
 * @{
*/
#define WUPTENDSEQ_A                0   /**< End sequence at A */
#define WUPTENDSEQ_B                1
#define WUPTENDSEQ_C                2
#define WUPTENDSEQ_D                3
#define WUPTENDSEQ_E                4
#define WUPTENDSEQ_F                5
#define WUPTENDSEQ_G                6
#define WUPTENDSEQ_H                7
/** @} */

/**
 * @} End of sequencer_and_FIFO block
 * @} Sequencer_FIFO
 * */

/**
 * @addtogroup MISC_Block
 * @{
 *    @defgroup MISC_Block_Const
 *    @brief This block includes clock, GPIO, configuration.
 *    @{
*/

/* Helper for calculate clocks needed for various of data type */
/**
 * @defgroup DATATYPE_Const
 * @{
*/
#define DATATYPE_ADCRAW             0     /**< ADC raw data */
#define DATATYPE_SINC3              1     /**< SINC3 data */
#define DATATYPE_SINC2              2     /**< SINC2 Data */
#define DATATYPE_DFT                3     /**< DFT */
//#define DATATYPE_MEAN
/** @} */


/**
 * @defgroup SLPKEY_Const
 * @{
*/
#define SLPKEY_LOCK                 0       /**< any incorrect value will lock the key */
#define SLPKEY_UNLOCK               0xa47e5 /**< The correct key for register SEQSLPLOCK */
/** @} */

/**
 * @defgroup HPOSCOUT_Const
 * @brief Set HPOSC output clock frequency, 16MHz or 32MHz.
 * @{
*/
#define HPOSCOUT_32MHZ              0   /**< Configure internal HFOSC output 32MHz clock */
#define HPOSCOUT_16MHZ              1   /**< 16MHz Clock */
/** @} */

/* GPIO */
/**
 * @defgroup AGPIOPIN_Const
 * @brief The pin masks for register GP0OEN, GP0PE, GP0IEN,..., GP0TGL
 * @{
*/
#define AGPIO_Pin0                  0x01
#define AGPIO_Pin1                  0x02
#define AGPIO_Pin2                  0x04
#define AGPIO_Pin3                  0x08
#define AGPIO_Pin4                  0x10
#define AGPIO_Pin5                  0x20
#define AGPIO_Pin6                  0x40
#define AGPIO_Pin7                  0x80
/** @} */

/**
 * @defgroup GP0FUNC_Const
 * @{
*/
#define GP0_INT                     0        /**< Interrupt Controller 0 output */
#define GP0_TRIG                    1        /**< Sequence0 trigger */
#define GP0_SYNC                    2        /**< Use Sequencer to controll GP0 output level */
#define GP0_GPIO                    3        /**< Normal GPIO function */
/** @} */

/**
 * @defgroup GP1FUNC_Const
 * @{
*/
#define GP1_GPIO                    (0<<2)   /**< Normal GPIO function */
#define GP1_TRIG                    (1<<2)   /**< Sequence1 trigger */
#define GP1_SYNC                    (2<<2)   /**< Use Sequencer to controll GP1 output level */
#define GP1_SLEEP                   (3<<2)   /**< Internal Sleep Signal */
/** @} */

/**
 * @defgroup GP2FUNC_Const
 * @{
*/
#define GP2_PORB                    (0<<4)   /**< Internal Power ON reset signal */
#define GP2_TRIG                    (1<<4)   /**< Sequence1 trigger */
#define GP2_SYNC                    (2<<4)   /**< Use Sequencer to controll GP2 output level */
#define GP2_EXTCLK                  (3<<4)   /**< External Clock input(32kHz/16MHz/32MHz) */
/** @} */

/**
 * @defgroup GP3FUNC_Const
 * @{
*/
#define GP3_GPIO                    (0<<6)   /**< Normal GPIO function */
#define GP3_TRIG                    (1<<6)   /**< Sequence3 trigger */
#define GP3_SYNC                    (2<<6)   /**< Use Sequencer to controll GP3 output level */
#define GP3_INT0                    (3<<6)   /**< Interrupt Controller 0 output */
/** @} */

/**
 * @defgroup GP4FUNC_Const
 * @note GP4 (Not available on AD5941)
 * @{
*/
#define GP4_GPIO                    (0<<8)   /**< Normal GPIO function */
#define GP4_TRIG                    (1<<8)   /**< Sequence0 trigger */
#define GP4_SYNC                    (2<<8)   /**< Use Sequencer to controll GP4 output level */
#define GP4_INT1                    (3<<8)   /**< Interrupt Controller 1 output */
/** @} */

/**
 * @defgroup GP5FUNC_Const
 * @note GP5 (Not available on AD5941)
 * @{
*/
#define GP5_GPIO                    (0<<10)  /**< Internal Power ON reset signal */
#define GP5_TRIG                    (1<<10)  /**< Sequence1 trigger */
#define GP5_SYNC                    (2<<10)  /**< Use Sequencer to controll GP5 output level */
#define GP5_EXTCLK                  (3<<10)  /**< External Clock input(32kHz/16MHz/32MHz) */
/** @} */

/**
 * @defgroup GP6FUNC_Const
 * @note GP6 (Not available on AD5941)
 * @{
*/
#define GP6_GPIO                    (0<<12)  /**< Normal GPIO function */
#define GP6_TRIG                    (1<<12)  /**< Sequence2 trigger */
#define GP6_SYNC                    (2<<12)  /**< Use Sequencer to controll GP6 output level */
#define GP6_INT0                    (3<<12)  /**< Interrupt Controller 0 output */
/** @} */

/**
 * @defgroup GP7FUNC_Const
 * @note GP7 (Not available on AD5941)
 * @{
*/
#define GP7_GPIO                    (0<<14)  /**< Normal GPIO function */
#define GP7_TRIG                    (1<<14)  /**< Sequence2 trigger */
#define GP7_SYNC                    (2<<14)  /**< Use Sequencer to controll GP7 output level */
#define GP7_INT                     (3<<14)  /**< Interrupt Controller 1 output */
/** @} */

//LPModeClk
/**
 * @defgroup LPMODECLK_Const
 * @{
*/
#define LPMODECLK_HFOSC             0       /* Use HFOSC 16MHz/32MHz clock as system clock */
#define LPMODECLK_LFOSC             1       /* Use LFOSC 32kHz clock as system clock */
/** @} */

/* Clock */
/**
 * @defgroup SYSCLKSRC_Const
 * @brief Select system clock source. The clock must be available. If unavailable clock is selected, we can reset AD5940.
 *        The system clock should be limited to 32MHz. If external clock or XTAL is faster than 16MHz, we use system clock divider to ensure it's always in range of 16MHz.
 * @warning Maximum SPI clock has relation with system clock. Limit the SPI clock to ensure SPI clock is slower than system clock.
 * @{
*/
#define SYSCLKSRC_HFOSC             0     /**< Internal HFOSC. CLock is 16MHz or 32MHz configurable. Set clock divider to ensure system clock is always 16MHz */
#define SYSCLKSRC_XTAL              1     /**< External crystal. It can be 16MHz or 32MHz.Set clock divider to ensure system clock is always 16MHz */
#define SYSCLKSRC_LFOSC             2     /**< Internal 32kHz clock. Note the SPI clock also sourced with 32kHz so the register read/write frequency is lower down. */
#define SYSCLKSRC_EXT               3     /**< External clock from GPIO */
/** @} */

/**
 * @defgroup ADCCLKSRC_Const
 * @brief Select ADC clock source.
 *        The maximum clock is 32MHz.
 * @warning The ADC raw data update rate is equal to ADCClock/20. When ADC clock is 32MHz, sample rate is 1.6MSPS.
 *          The SINC3 filter clock are sourced from ADC clock and should be limited to 16MHz. When ADC clock is set to 32MHz. Clear bit ADCFILTERCON.BIT0
 *          to enable the SINC3 clock divider.
 * @{
*/
#define ADCCLKSRC_HFOSC             0     /**< Internal HFOSC. 16MHz or 32MHz which is configurable */
#define ADCCLKSRC_XTAL              1     /**< External crystal. Set ADC clock divider to get either 16MHz or 32MHz clock */
//#define ADCCLKSRC_LFOSC             2     /**< Do not use */
#define ADCCLKSRC_EXT               3     /**< External clock from GPIO. Set ADC clock divider to get the clock you want */
/** @} */


/**
 * @defgroup ADCCLKDV_Const
 * @brief The divider for ADC clock. ADC clock = ClockSrc/Divider.
 * @{
*/
#define ADCCLKDIV_1                 1     /**< Divider ADCClk = ClkSrc/1 */
#define ADCCLKDIV_2                 2     /**< Divider ADCClk = ClkSrc/2 */
/** @} */


/**
 * @defgroup SYSCLKDV_Const
 * @brief The divider for system clock. System clock = ClockSrc/Divider.
 * @{
*/
#define SYSCLKDIV_1                 1     /**< Divider SysClk = ClkSrc/1 */
#define SYSCLKDIV_2                 2     /**< Divider SysClk = ClkSrc/2 */
/** @} */

#ifndef NULL
#define NULL      (void *) 0
#endif
#define MATH_PI                   3.1415926f

#define AD5940_ADIID              0x4144      /**< ADIID is fixed to 0x4144 */
#define AD5940_CHIPID             0x0000      /**< CHIPID is changing with silicon version */
#define KEY_OSCCON                0xcb14      /**< key of register OSCCON. The key is auto locked after writing to any other register */
#define KEY_CALDATLOCK	          0xde87a5af  /**< Calibration key. */
#define KEY_LPMODEKEY             0xc59d6     /**< LP mode key */

#define PARA_CHECK(n)   /** @todo add parameter check, Add DEBUG switch  */

/**
 * @} MISC_Block_Const
 * @} MISC_Block
 * */

#define SILICON_VER         2               /* 1: initial silicon version. 2: second version silicon. */
/**
 * #Differences between Si1 and Si2
 * - WGFCW register bit width has been extended from 20bit to 24bit. The equation to calculate frequency is FCW[23:0]/2^30*Sytemclock.
 *    For Si1, it's FCW[19:0]/2^26*Sytemclock
 * - ULPDACCONPRO0 register is now available. The LPDAC switches can be configured freely.
*/
#define __BITWIDTH_WGFCW            (SILICON_VER==1?26:30)	/** @todo remove it. Current silicon value is 26. In future, it's 30 */

/**
 * @defgroup TypeDefinitions
 * @{
*/

typedef struct {
	/* ADC/DAC/TIA reference and buffer */
	bool HpBandgapEn;     /**< Enable High power band-gap. Clear bit AFECON.HPREFDIS will enable Bandgap, while set this bit will disable bandgap */
	bool Hp1V8BuffEn;     /**< High power 1.8V reference buffer enable */
	bool Hp1V1BuffEn;     /**< High power 1.1V reference buffer enable */
	bool Lp1V8BuffEn;     /**< Low power 1.8V reference buffer enable */
	bool Lp1V1BuffEn;     /**< Low power 1.1V reference buffer enable */
	/* Low bandwidth loop reference and buffer */
	bool LpBandgapEn;     /**< Enable Low power band-gap. */
	bool LpRefBufEn;      /**< Enable the 2.5V low power reference buffer */
	bool LpRefBoostEn;    /**< Boost buffer current */
	/* DAC Reference Buffer */
	bool HSDACRefEn;      /**< Enable DAC reference buffer from HP Bandgap */
	/* Misc. control  */
	bool Hp1V8ThemBuff;   /**< Thermal Buffer for internal 1.8V reference to AIN3 pin  */
	bool Hp1V8Ilimit;     /**< Current limit for High power 1.8V reference buffer */
	bool Disc1V8Cap;      /**< Discharge 1.8V capacitor. Short external 1.8V decouple capacitor to ground. Be careful when use this bit  */
	bool Disc1V1Cap;      /**< Discharge 1.1V capacitor. Short external 1.1V decouple capacitor to ground. Be careful when use this bit  */
} AFERefCfg_Type;

/**
 * @defgroup ADC_BlockType
 * @{
*/

/**
 * Structure for ADC Basic settings include MUX and PGA.
*/
typedef struct {
	uint32_t ADCMuxP;         /**< ADC Positive input channel selection. select from @ref ADCMUXP */
	uint32_t ADCMuxN;         /**< ADC negative input channel selection. select from @ref ADCMUXN */
	uint32_t ADCPga;          /**< ADC PGA settings, select from @ref ADCPGA */
} ADCBaseCfg_Type;

/**
 * Structure for ADC filter settings.
*/
typedef struct {
	uint32_t ADCSinc3Osr;
	uint32_t ADCSinc2Osr;
	uint32_t ADCAvgNum;
	uint32_t ADCRate;             /**< ADC Core sample rate */
	bool BpNotch;             /**< Bypass Notch filter module. ADCFILTERCON.BIT4 */
	bool BpSinc3;             /**< Bypass SINC3 Module */
	bool Sinc3ClkEnable;      /**< Enable SINC3 clock */
	bool Sinc2NotchClkEnable; /**< Enable SINC2+Notch clock @todo, delete it because we already have an 'Enable' switch. */
	bool Sinc2NotchEnable;    /**< Enable SINC2+Notch block */
	bool DFTClkEnable;        /**< Enable DFT clock */
	bool WGClkEnable;         /**< Enable Waveform Generator clock */
} ADCFilterCfg_Type;
/** @} */

/**
 * DFT Configuration structure.
*/
typedef struct {
	uint32_t DftNum;      /**< DFT number */
	uint32_t DftSrc;      /**< DFT Source */
	bool HanWinEn;    /**< Enable Hanning window */
} DFTCfg_Type;

/**
 * ADC digital comparator
*/
typedef struct {
	uint16_t ADCMin;      /**< The ADC code minimum limit value */
	uint16_t ADCMinHys;
	uint16_t ADCMax;      /**< The ADC code maximum limit value */
	uint16_t ADCMaxHys;
} ADCDigComp_Type; /**< @todo not tested */

/**
 * Statistic function
*/
typedef struct {
	uint32_t StatDev;     /**< Statistic standard deviation configure */
	uint32_t StatSample;  /**< Sample size */
	bool StatEnable;  /**< Set ture to enable statistic block */
} StatCfg_Type;

/**
 * Switch matrix configure
 */
typedef struct {
	uint32_t Dswitch; /**< Select from SWD_RCAL0, SWD_AIN0, ... */
	uint32_t Pswitch;
	uint32_t Nswitch;
	uint32_t Tswitch;
} SWMatrixCfg_Type;

/** HSTIA Configure */
typedef struct {
	uint32_t HstiaBias;         /**< @todo When select Vzero as bias, the switch at LPDAC should be closed */
	uint32_t HstiaRtiaSel;
	uint32_t HstiaCtia;         /**< Set internal CTIA value from 1 to 32 pF */
	bool DiodeClose;        /**< Close the switch for internal back to back diode */
	uint32_t HstiaDeRtia;
	uint32_t HstiaDeRload;
} HSTIACfg_Type;

/** HSDAC Configure */
typedef struct {
	uint32_t ExcitBufGain;      /**< Select from  EXCITBUFGAIN_2, EXCITBUFGAIN_0P25 */
	uint32_t HsDacGain;         /**< Select from  HSDACGAIN_1, HSDACGAIN_0P2 */
	uint32_t HsDacUpdateRate;   /**< Divider for DAC update. Available range is 7~255. */
} HSDACCfg_Type;

/** LPDAC Configure
 * @note The LPDAC structure:
 * Switch to select DAC ouput to Vzero and Vbias nodes. Vzero and Vbias can select from DAC6BIT and DAC12BIT output freely.
 * LPDAC  >DAC6BIT ---- Vzero   LPDACVZERO_12BIT
 *                 \--- Vbias   LPDACVBIAS_6BIT
 *        >DAC12BIT---- Vzero   LPDACVZERO_6BIT
 *                 \--- Vbias   LPDACVBIAS_12BIT
 * Vzero/Vbias switch, controlled by @ref LPDACCfg_Type LpDacSW
 * Vzero ------PIN
 *       \-----LPTIA  LPDACSW_VZERO2LPTIA. LPTIA positive input
 *        \----HSTIA  LPDACSW_VZERO2LPAMP. HSTIA positive input. Note, there is a MUX on HSTIA positive input pin to select the bias voltage between Vzero and 1.1V fixed internal reference.
 * Vbias ------PIN    LPDACSW_VBIAS2PIN
 *       \-----LPAMP  LPDACSW_VBIAS2LPAMP positive input. The potential state amplifier input, or called LPAMP or PA(potential amplifier).
*/
typedef struct {
	uint32_t LpDacSrc;        /**< LPDACSRC_MMR or LPDACSRC_WG. Note: HSDAC is always connects to WG. Disable HSDAC if there is need. */
	uint32_t LpDacVzeroMux;   /**< Select which DAC output connects to Vzero. 6Bit or 12Bit DAC */
	uint32_t LpDacVbiasMux;   /**< Select which DAC output connects to Vbias */
	uint32_t LpDacSW;         /**< LPDAC switche set. Only available from Si2 */
	uint32_t LpDacRef;        /**< Reference selection */
	bool DataRst;         /**< Keep Reset register REG_AFE_LPDACDAT0DATA */
	bool PowerEn;         /**< Power up REG_AFE_LPDACDAT0 */
	uint16_t DacData12Bit;    /**< Data for 12bit DAC */
	uint16_t DacData6Bit;     /**< Data for 6bit DAC */
	/** @todo connection with HSTIA Vbias selection */
} LPDACCfg_Type;

/**
 * Low power amplifiers(PA and TIA)
*/
typedef struct {
	uint32_t LpTiaRf;
	uint32_t LpTiaRload;
	uint32_t LpTiaRtia;
	uint32_t LpAmpPwrMod;     /**< Power mode for LP PA and LPTIA */
	uint32_t LpTiaSW;         /**< Set of switches, using macro LPTIASW() to close switch */
	bool LpPaPwrEn;
	bool LpTiaPwrEn;
} LPAmpCfg_Type;

/**
 * Trapezoid Generator parameters
*/
typedef struct {
	uint32_t WGTrapzDCLevel1;
	uint32_t WGTrapzDCLevel2;
	uint32_t WGTrapzDelay1;
	uint32_t WGTrapzDelay2;
	uint32_t WGTrapzSlope1;
	uint32_t WGTrapzSlope2;
} WGTrapzCfg_Type;

/**
 * Sin wave generator parameters
*/
typedef struct {
	uint32_t SinFreqWord;       /**< Frequency word */
	uint32_t SinAmplitudeWord;  /**< Amplitude word, range is 0 to 2047 */
	uint32_t SinOffsetWord;     /**< Offset word, range is 0 to 4095 */
	uint32_t SinPhaseWord;
} WGSinCfg_Type;

/**
 * Waveform generator configuration
*/
typedef struct {
	uint32_t WgType;            /**< Select from WGTYPE_MMR, WGTYPE_SIN, WGTYPE_TRAPZ. HSDAC is always connected to WG. */
	bool GainCalEn;         /**< Enable Gain calibration */
	bool OffsetCalEn;       /**< Enable offset calibration */
	WGTrapzCfg_Type TrapzCfg;   /**< Configure Trapezoid generator */
	WGSinCfg_Type SinCfg;       /**< Configure Sine wave generator */
	uint32_t WgCode;            /**< The 12bit data WG will move to DAC data register. */
} WGCfg_Type;

/**
 * High speed loop configuration
 * */
typedef struct {
	SWMatrixCfg_Type SWMatCfg;
	HSDACCfg_Type HsDacCfg;
	WGCfg_Type WgCfg;
	HSTIACfg_Type HsTiaCfg;
} HSLoopCfg_Type;

/**
 * Low power loop Configure
 * */
typedef struct {
	LPDACCfg_Type LpDacCfg;
	LPAmpCfg_Type LpAmpCfg;
} LPLoopCfg_Type;

/**
 * DSP Configure
 * */
typedef struct {
	ADCBaseCfg_Type ADCBaseCfg;
	ADCFilterCfg_Type ADCFilterCfg;
	ADCDigComp_Type ADCDigCompCfg;
	DFTCfg_Type DftCfg;
	StatCfg_Type StatCfg;
} DSPCfg_Type;

/**
 * GPIO Configure
 * */
typedef struct {
	uint32_t FuncSet;         /**< AGP0 to AGP7 function sets */
	uint32_t OutputEnSet;     /**< AGPIO_Pin0|AGPIO_Pin1|...|AGPIO_Pin7, Enable output of selected pins, disable other pins */
	uint32_t InputEnSet;      /**< Enable input of selected pins, disable other pins */
	uint32_t PullEnSet;       /**< Enable pull up or down on selected pin. disable other pins */
	uint32_t OutVal;          /**< Value for GPIOOUT register */
} AGPIOCfg_Type;

/**
 * FIFO configure
*/
typedef struct {
	bool FIFOEn;          /**< Enable DATAFIFO. Disable FIFO will reset FIFO */
	uint32_t FIFOMode;        /**< Stream mode or standard FIFO mode */
	uint32_t FIFOSize;        /**< How to allocate the internal 6kB SRAM. Data FIFO and sequencer share all 6kB SRAM */
	uint32_t FIFOSrc;         /**< Select which data source will be stored to FIFO */
	uint32_t FIFOThresh;      /**< FIFO threshold value. Threshold can be used to generate interrupt so MCU can read back data before FIFO is full */
} FIFOCfg_Type;

/**
 * Sequencer configure
*/
typedef struct {
	uint32_t SeqMemSize;      /**< Sequencer memory size. SRAM is used by both FIFO and Sequencer. Make sure the total SRAM used is less than 6kB. */
	bool SeqEnable;       /**< Enable sequencer. Only with valid trigger, sequencer can run */
	bool SeqBreakEn;      /**< Do not use it */
	bool SeqIgnoreEn;     /**< Do not use it */
	bool SeqCntCRCClr;    /**< Clear sequencer count and CRC */
	uint32_t SeqWrTimer;      /**< Set wait how much clocks after every commands executed */
} SEQCfg_Type;

/**
 * Sequence info structure
*/
typedef struct {
	uint32_t SeqId;
	uint32_t SeqRamAddr;      /**< The start address that in AF5940 SRAM */
	uint32_t SeqLen;          /**< Sequence length */
	bool WriteSRAM;       /**< Write command to SRAM or not. */
	const uint32_t
	*pSeqCmd;  /**< Pointer to the sequencer commands that stored in MCU */
} SEQInfo_Type;

/**
 * Wakeup Timer Configure
 * */
typedef struct {
	uint32_t WuptEndSeq;       /**<  */
	uint32_t WuptOrder[8];     /**<  */
	uint32_t SeqxSleepTime[4];  /**< 0 to 0x000f_ffff */
	uint32_t SeqxWakeupTime[4]; /**< For Wupt. */
	bool WuptEn;            /**< Timer Enable. Once enabled, it starts to run. */
} WUPTCfg_Type;


/**
 * Clock configure
*/
typedef struct {
	uint32_t SysClkSrc;
	uint32_t ADCCLkSrc;
	uint32_t SysClkDiv;
	uint32_t ADCClkDiv;
	bool HFOSCEn;         /**< Enable internal 16MHz/32MHz HFOSC */
	bool HfOSC32MHzMode;  /**< Enable internal HFOSC to output 32MHz */
	bool LFOSCEn;         /**< Enable internal 32kHZ OSC */
	bool HFXTALEn;        /**< Enable XTAL driver */
} CLKCfg_Type;


/**
 * HSTIA internal RTIA calibration structure
*/
typedef struct {
	float fFreq;                /**< Calibration frequency */
	float fRcal;                /**< Rcal resistor value in Ohm*/
	float SysClkFreq;           /**< The real frequency of system clock */
	float AdcClkFreq;           /**< The real frequency of ADC clock */

	HSTIACfg_Type HsTiaCfg;
	uint32_t ADCSinc3Osr;   /**< SINC3OSR_5, SINC3OSR_4 or SINC3OSR_2 */
	uint32_t ADCSinc2Osr;   /**< SINC3OSR_5, SINC3OSR_4 or SINC3OSR_2 */
	DFTCfg_Type DftCfg;
	uint32_t bPolarResult;   /**< bTRUE-Polar coordinate:Return results in Magnitude and Phase. bFALSE-Cartesian coordinate: Return results in Real part and Imaginary Part */
} HSRTIACal_Type;

/**
 * LPTIA internal RTIA calibration structure
*/
typedef struct {
	float fFreq;                /**< Calibration frequency. @todo DC not vefiried now. Set it to 0.0 for DC calibration */
	float fRcal;                /**< Rcal resistor value in Ohm*/
	float SysClkFreq;           /**< The real frequency of system clock */
	float AdcClkFreq;           /**< The real frequency of ADC clock */

	LPAmpCfg_Type LpAmpCfg;     /**< Select which RTIA you are going to calibrate */
	uint32_t ADCSinc3Osr;       /**< SINC3OSR_5, SINC3OSR_4 or SINC3OSR_2 */
	uint32_t ADCSinc2Osr;       /**< SINC3OSR_5, SINC3OSR_4 or SINC3OSR_2 */
	DFTCfg_Type DftCfg;
	uint32_t bPolarResult;      /**< bTRUE-Polar coordinate:Return results in Magnitude and Phase. bFALSE-Cartesian coordinate: Return results in Real part and Imaginary Part */
} LPRTIACal_Type;

/**
 * LFOSC frequency measure structrue
*/
typedef struct {
	uint32_t CalSeqAddr;        /**< Sequence start address */
	float CalDuration;          /**< Time used for calibration in unit of ms. Recommend to use tens of millisecond like 10ms */
	float SystemClkFreq;        /**< System clock frequency.  */
} LFOSCMeasure_Type; /**< Parameters to measure LFOSC frequency */

/**
 * Structrue for calculating how much system clocks needed for specified number of data
*/
typedef struct {
	uint32_t ADCSinc3Osr;
	uint32_t ADCSinc2Osr;
	uint32_t ADCAvgNum;
	uint32_t DftSrc;
	/**< @todo statistic?*/
	uint32_t DataType;
	uint32_t DataCount;
	float RatioSys2AdcClk;/**< Ratio of system clock to ADC clock frequency */
} ClksCalInfo_Type;

/**
 * Software controlled Sweep Function
 * */
typedef struct {
	bool SweepEn;         /**< Software can automatically sweep frequency from following parameters. Set value to 1 to enable it. */
	float SweepStart;         /**< Sweep start frequency. Software will go back to the start frequency when it reaches SWEEP_STOP */
	float SweepStop;          /**< Sweep end frequency. */
	uint32_t SweepPoints;     /**< How many points from START to STOP frequency */
	bool SweepLog;        /**< The step is linear or logarithmic. 0: Linear, 1: Logarithmic*/
	uint32_t SweepIndex;      /**< Current position of sweep */
} SoftSweepCfg_Type;

/**
 * Impedance result in Polar coordinate
*/
typedef struct {
	float Magnitude;
	float Phase;
} fImpPol_Type; //Polar

/**
 * Impedance result in Cartesian coordinate
*/
typedef struct {
	float Real;
	float Image;
} fImpCar_Type; //Cartesian

/**
 * int32_t type Impedance result in Cartesian coordinate
*/
typedef struct {
	int32_t Real;
	int32_t Image;
} iImpCar_Type;

/**
 * Structure used to store register information(address and its data)
 * */
typedef struct {
	uint32_t RegAddr  :8;   /* 8bit address is enough for sequencer */
	uint32_t RegValue :24;  /* Reg data is limited to 24bit by sequencer  */
} SEQGenRegInfo_Type;

/**
 * Sequencer generator data base.
 */
struct SeqGen {
	bool EngineStart;   /* Start the generator now */
	uint32_t BufferSize;    /* Total buffer size */

	uint32_t *pSeqBuff;
	uint32_t SeqLen;        /* Generated sequence length */
	SEQGenRegInfo_Type *pRegInfo;
	uint32_t RegCount;
	int LastError;
};

/**
 * ad5940 device driver initialization parameters.
 */
struct ad5940_init_param {
	struct no_os_spi_init_param spi_init;
	struct no_os_gpio_init_param reset_gpio_init;
	struct no_os_gpio_init_param gp0_gpio_init;
};

/**
 * ad5940 device driver handler.
 */
struct ad5940_dev {
	struct no_os_spi_desc *spi;
	struct no_os_gpio_desc *reset_gpio;
	struct no_os_gpio_desc *gp0_gpio;
	struct SeqGen SeqGenDB;
};

/**
 * @} TypeDefinitions
*/

/**
 * @defgroup Exported_Functions
 * @{
*/
/* 0. Device driver init/remove functions */
int ad5940_init(struct ad5940_dev **device,
		struct ad5940_init_param *init_param);
int ad5940_remove(struct ad5940_dev *dev);

/* 1. Basic SPI functions */
int ad5940_WriteReg(struct ad5940_dev *dev, uint16_t RegAddr, uint32_t RegData);
int ad5940_ReadReg(struct ad5940_dev *dev, uint16_t RegAddr, uint32_t *RegData);
int ad5940_WriteReg_mask(struct ad5940_dev *dev, uint16_t RegAddr,
			 uint32_t mask, uint32_t RegData);
int ad5940_FIFORd(struct ad5940_dev *dev, uint32_t *pBuffer,
		  uint32_t uiReadCount);

/* 2. AD5940 Top Control functions */
int ad5940_AFECtrlS(struct ad5940_dev *dev, uint32_t AfeCtrlSet, bool State);
int ad5940_LPModeCtrlS(struct ad5940_dev *dev, uint32_t EnSet);
int ad5940_AFEPwrBW(struct ad5940_dev *dev, uint32_t AfePwr,
		    uint32_t AfeBw); /* AFE power mode and system bandwidth control */
int ad5940_REFCfgS(struct ad5940_dev *dev, AFERefCfg_Type *pBufCfg);

/* 3. High_Speed_Loop Functions */
int ad5940_HSLoopCfgS(struct ad5940_dev *dev, HSLoopCfg_Type *pHsLoopCfg);
int ad5940_SWMatrixCfgS(struct ad5940_dev *dev, SWMatrixCfg_Type *pSwMatrix);
int ad5940_HSDacCfgS(struct ad5940_dev *dev, HSDACCfg_Type *pHsDacCfg);
int ad5940_HSTIACfgS(struct ad5940_dev *dev, HSTIACfg_Type *pHsTiaCfg);

/* 4. Low_Power_Loop Functions*/
int ad5940_LPLoopCfgS(struct ad5940_dev *dev, LPLoopCfg_Type *pLpLoopCfg);
int ad5940_LPDACCfgS(struct ad5940_dev *dev, LPDACCfg_Type *pLpDacCfg);
int ad5940_LPDACWriteS(struct ad5940_dev *dev, uint16_t Data12Bit,
		       uint8_t Data6Bit);
int ad5940_LPAMPCfgS(struct ad5940_dev *dev, LPAmpCfg_Type *pLpAmpCfg);

/* 5. DSP_Block_Functions */
int ad5940_DSPCfgS(struct ad5940_dev *dev, DSPCfg_Type *pDSPCfg);
int ad5940_ReadAfeResult(struct ad5940_dev *dev, uint32_t AfeResultSel,
			 uint32_t *rd);

/* 5.1 ADC Block */
int ad5940_ADCBaseCfgS(struct ad5940_dev *dev, ADCBaseCfg_Type *pADCInit);
int ad5940_ADCFilterCfgS(struct ad5940_dev *dev, ADCFilterCfg_Type *pFiltCfg);
int ad5940_ADCPowerCtrlS(struct ad5940_dev *dev, bool State);
int ad5940_ADCConvtCtrlS(struct ad5940_dev *dev, bool State);
int ad5940_ADCMuxCfgS(struct ad5940_dev *dev, uint32_t ADCMuxP,
		      uint32_t ADCMuxN);
int ad5940_ADCDigCompCfgS(struct ad5940_dev *dev, ADCDigComp_Type *pCompCfg);
int ad5940_StatisticCfgS(struct ad5940_dev *dev, StatCfg_Type *pStatCfg);
int ad5940_ADCRepeatCfgS(struct ad5940_dev *dev, uint32_t Number);
int ad5940_DFTCfgS(struct ad5940_dev *dev, DFTCfg_Type *pDftCfg);

/* 5.2 Waveform Generator Block */
int ad5940_WGCfgS(struct ad5940_dev *dev, WGCfg_Type *pWGInit);
int ad5940_WGDACCodeS(struct ad5940_dev *dev,
		      uint32_t code); /* Directly write DAC Code */
int ad5940_WGFreqCtrlS(struct ad5940_dev *dev, float SinFreqHz, float WGClock);
uint32_t ad5940_WGFreqWordCal(float SinFreqHz, float WGClock);

/* 6. Sequencer_FIFO */
int ad5940_FIFOCfg(struct ad5940_dev *dev, FIFOCfg_Type *pFifoCfg);
int ad5940_FIFOGetCfg(struct ad5940_dev *dev,
		      FIFOCfg_Type *pFifoCfg);  /* Read back current configuration */
int ad5940_FIFOCtrlS(struct ad5940_dev *dev, uint32_t FifoSrc,
		     bool FifoEn);   /* Configure FIFO data source. This function will also enable FIFO in same time */
int ad5940_FIFOThrshSet(struct ad5940_dev *dev, uint32_t FIFOThresh);
int ad5940_FIFOGetCnt(struct ad5940_dev *dev,
		      uint32_t *cnt);     /* Get current FIFO count */
int ad5940_SEQCfg(struct ad5940_dev *dev, SEQCfg_Type *pSeqCfg);
int ad5940_SEQGetCfg(struct ad5940_dev *dev,
		     SEQCfg_Type *pSeqCfg);    /* Read back current configuration */
int ad5940_SEQCtrlS(struct ad5940_dev *dev, bool SeqEn);
int ad5940_SEQHaltS(struct ad5940_dev *dev);
int ad5940_SEQMmrTrig(struct ad5940_dev *dev,
		      uint32_t SeqId); /* Manually trigger sequence */
int ad5940_SEQCmdWrite(struct ad5940_dev *dev, uint32_t StartAddr,
		       const uint32_t *pCommand, uint32_t CmdCnt);
int ad5940_SEQInfoCfg(struct ad5940_dev *dev, SEQInfo_Type *pSeq);
int ad5940_SEQInfoGet(struct ad5940_dev *dev, uint32_t SeqId,
		      SEQInfo_Type *pSeqInfo);
int ad5940_SEQGpioCtrlS(struct ad5940_dev *dev,
			uint32_t GpioSet);   /* Sequencer can control GPIO0~7 if the GPIO function is set to SYNC */
int ad5940_SEQTimeOutRd(struct ad5940_dev *dev,
			uint32_t *cnt);  /* Read back current sequence time out value */
int ad5940_SEQGpioTrigCfg(void); /* @todo not done */
int ad5940_WUPTCfg(struct ad5940_dev *dev, WUPTCfg_Type *pWuptCfg);
int ad5940_WUPTCtrl(struct ad5940_dev *dev,
		    bool Enable);  /* Enable or disable Wakeup timer */
int ad5940_WUPTTime(struct ad5940_dev *dev, uint32_t SeqId, uint32_t SleepTime,
		    uint32_t WakeupTime);

/* 7. MISC_Block */
/* 7.1 Clock system */
int ad5940_CLKCfg(struct ad5940_dev *dev, CLKCfg_Type *pClkCfg);
int ad5940_HFOSC32MHzCtrl(struct ad5940_dev *dev, bool Mode32MHz);

/* 7.2 AFE Interrupt */
int ad5940_INTCCfg(struct ad5940_dev *dev, uint32_t AfeIntcSel,
		   uint32_t AFEIntSrc, bool State);
int ad5940_INTCGetCfg(struct ad5940_dev *dev, uint32_t AfeIntcSel,
		      uint32_t *cfg);
int ad5940_INTCClrFlag(struct ad5940_dev *dev, uint32_t AfeIntSrcSel);
bool ad5940_INTCTestFlag(struct ad5940_dev *dev, uint32_t AfeIntcSel,
			 uint32_t AfeIntSrcSel); /* Check if selected interrupt happened */
int ad5940_INTCGetFlag(struct ad5940_dev *dev, uint32_t AfeIntcSel,
		       uint32_t *flag); /* Get current INTC interrupt flag */

/* 7.3 GPIO */
int ad5940_AGPIOCfg(struct ad5940_dev *dev, AGPIOCfg_Type *pAgpioCfg);
int ad5940_AGPIOFuncCfg(struct ad5940_dev *dev, uint32_t uiCfgSet);
int ad5940_AGPIOOen(struct ad5940_dev *dev, uint32_t uiPinSet);
int ad5940_AGPIOIen(struct ad5940_dev *dev, uint32_t uiPinSet);
int ad5940_AGPIOPen(struct ad5940_dev *dev, uint32_t uiPinSet);

/* 7.4 LPMODE */
int ad5940_LPModeEnS(struct ad5940_dev *dev,
		     bool LPModeEn); /* Enable LP mode or disable it. */
int ad5940_LPModeClkS(struct ad5940_dev *dev, uint32_t LPModeClk);

/* 7.5 Power */
int ad5940_SleepKeyCtrlS(struct ad5940_dev *dev,
			 uint32_t SlpKey); /* enter the correct key to allow AFE to enter sleep mode */
int ad5940_EnterSleepS(struct ad5940_dev
		       *dev);      /* Put AFE to hibernate/sleep mode and keep LP loop as the default settings. */
int ad5940_ShutDownS(struct ad5940_dev
		     *dev);    /* Unlock the key, turn off LP loop and enter sleep/hibernate mode  */
int ad5940_WakeUp(struct ad5940_dev *dev,
		  int32_t TryCount);   /* Try to wakeup AFE by read register */
int ad5940_HWReset(struct ad5940_dev
		   *dev);       /* Do hardware reset to AD5940 using RESET pin */

/* Calibration functions */
/* 8. Calibration */
int ad5940_HSRtiaCal(struct ad5940_dev *dev, HSRTIACal_Type *pCalCfg,
		     void *pResult);
int ad5940_LPRtiaCal(struct ad5940_dev *dev, LPRTIACal_Type *pCalCfg,
		     void *pResult);
int ad5940_LFOSCMeasure(struct ad5940_dev *dev, LFOSCMeasure_Type *pCfg,
			float *pFreq) ; /* Measure current LFOSC frequency. */
/** @todo add temperature sensor functions */

/* 9. Pure software functions. Functions with no register access. These functions are helpers */
/* Sequence Generator */
int ad5940_SEQGenInit(struct ad5940_dev *dev, uint32_t *pBuffer,
		      uint32_t BufferSize);/* Initialize sequence generator workspace */
int ad5940_SEQGenCtrl(struct ad5940_dev *dev,
		      bool bTRUE);  /* Enable or disable sequence generator */
int ad5940_SEQGenInsert(struct ad5940_dev *dev,
			uint32_t CmdWord); /* Manually insert a sequence command */
int ad5940_SEQGenFetchSeq(struct ad5940_dev *dev, const uint32_t **ppSeqCmd,
			  uint32_t *pSeqCount);  /* Fetch generated sequence and start a new sequence */
int ad5940_ClksCalculate(struct ad5940_dev *dev, ClksCalInfo_Type *pFilterInfo,
			 uint32_t *pClocks);  /* @todo add notch filter calculation. Calculate how much clocks to reach n points of data */
void ad5940_SweepNext(struct ad5940_dev *dev, SoftSweepCfg_Type *pSweepCfg,
		      float *pNextFreq);
fImpCar_Type ad5940_ComplexDivFloat(fImpCar_Type *a, fImpCar_Type *b);
fImpCar_Type ad5940_ComplexMulFloat(fImpCar_Type *a, fImpCar_Type *b);
fImpCar_Type ad5940_ComplexAddFloat(fImpCar_Type *a, fImpCar_Type *b);
fImpCar_Type ad5940_ComplexSubFloat(fImpCar_Type *a, fImpCar_Type *b);
fImpCar_Type ad5940_ComplexDivInt(iImpCar_Type *a, iImpCar_Type *b);
fImpCar_Type ad5940_ComplexMulInt(iImpCar_Type *a, iImpCar_Type *b);
float     ad5940_ComplexMag(fImpCar_Type *a);
float     ad5940_ComplexPhase(fImpCar_Type *a);

/**
 * @} Exported_Functions
*/

/**
  * @} AD5940_Library
  */

#endif
