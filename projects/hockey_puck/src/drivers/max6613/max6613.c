/***************************************************************************//**
 *   @file   max6613.c
 *   @brief  Source for the MAX6613 driver
 *   @author Mark Sapungan (Mark.Sapungan@analog.com)
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "no_os_error.h"
#include "no_os_alloc.h"
#include "max6613.h"

#undef  ADC_DMA     // Define to perform DMA-driven ADC conversion
#undef  HW_TRIGGER  // Define to enable HW trigger for conversion

/******************************************************************************/
/******************************* Globals **************************************/
/******************************************************************************/

#ifndef ADC_DMA
static volatile unsigned int adc_done = 0;
static uint32_t adc_index = 0;
#endif

#ifdef ADC_DMA
static volatile unsigned int dma_done = 0;
#endif

static int32_t adc_val[(MAX_ADC_FIFO_LEN / 2)];

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

#ifndef ADC_DMA
void adc_complete_cb(void *req, int flags)
{
    if (flags & MXC_F_ADC_INTFL_SEQ_DONE) {
        adc_index += MXC_ADC_GetData(&adc_val[adc_index]);
        adc_done = 1;
    }

    if (flags & MXC_F_ADC_INTFL_FIFO_LVL) {
        adc_index += MXC_ADC_GetData(&adc_val[adc_index]);
    }
}

void ADC_IRQHandler(void)
{
    MXC_ADC_DisableConversion();

    MXC_ADC_Handler();
}
#endif

#ifdef ADC_DMA
void ADC_IRQHandler(void)
{
    MXC_ADC_ClearFlags(0xFFFFFFFF);
}

void adc_dma_callback(int ch, int err)
{
    MXC_ADC_DisableConversion();

    dma_done = 1;
}

void DMA0_IRQHandler(void)
{
    MXC_DMA_Handler();
}
#endif

#ifdef HW_TRIGGER
static int max6613_hw_trig_timer(void)
{
    int ret;
    mxc_tmr_cfg_t tmr;
    uint32_t periodTicks = MXC_TMR_GetPeriod(MXC_TMR1, MXC_TMR_APB_CLK, 32, 2);

    MXC_TMR_Shutdown(MXC_TMR1);

    tmr.pres = TMR_PRES_32;
    tmr.mode = TMR_MODE_CONTINUOUS;
    tmr.bitMode = TMR_BIT_MODE_32;
    tmr.clock = MXC_TMR_APB_CLK;
    tmr.cmp_cnt = periodTicks;
    tmr.pol = 1;

    ret = MXC_TMR_Init(MXC_TMR1, &tmr, false);
    if (ret) {
        printf("Failed PWM timer Initialization.\n");
        return ret;
    }

    MXC_TMR_Start(MXC_TMR1);
    printf("Timer started.\n\n");

    return 0;
}
#endif

/**
 * @brief Device init function
 * @param device - MAX6613 descriptor to be initialized
 * @param init_param - Init parameter for descriptor
 * @return 0 in case of success, errno errors otherwise
 */
int max6613_init(struct max6613_dev **device, struct max6613_init_param *init_param)
{
    int ret;
    struct max6613_dev *desc;

    desc = no_os_calloc(1, sizeof(*desc));
    if (!desc)
        return -ENOMEM;

    /* Initialize ADC */
    desc->adc_cfg = init_param->adc_cfg;
    desc->adc_cfg.clock = MXC_ADC_HCLK;
    desc->adc_cfg.clkdiv = MXC_ADC_CLKDIV_16;
    desc->adc_cfg.cal = MXC_ADC_EN_CAL;
    desc->adc_cfg.trackCount = 4;
    desc->adc_cfg.idleCount = 17;

    ret = MXC_ADC_Init(&desc->adc_cfg);
    if (ret) {
        goto adc_error;
    }

    /* Configure ADC */
    desc->adc_conv = init_param ->adc_conv;

#ifdef HW_TRIGGER
    desc->adc_conv.trig = MXC_ADC_TRIG_HARDWARE;
    desc->adc_conv.hwTrig = MXC_ADC_TRIG_SEL_TMR1;

    max6613_hw_trig_timer();
#else
    desc->adc_conv.trig = MXC_ADC_TRIG_SOFTWARE;
#endif

#ifdef ADC_DMA
    desc->adc_conv.fifo_threshold = 0,
#else
    desc->adc_conv.fifo_threshold = MAX_ADC_FIFO_LEN >> 1,
#endif

    ret = MXC_ADC_Configuration(&desc->adc_conv);
    if (ret) {
        goto adc_error;
    }

    /* Configrue ADC channel/slot */
    desc->adc_slot = init_param->adc_slot;

    ret = MXC_ADC_SlotConfiguration(&desc->adc_slot, desc->adc_conv.num_slots);
    if (ret) {
        goto adc_error;
    }

    NVIC_EnableIRQ(ADC_IRQn);

#ifdef ADC_DMA
    MXC_DMA_Init();
    NVIC_EnableIRQ(DMA0_IRQn);
#endif

    *device = desc;
    return 0;

adc_error:
    no_os_free(desc);

    return ret;
}

/**
 * @brief Remove resources allocated by the init function
 * @param device - MAX6613 descriptor
 * @return 0 in case of success, errno errors otherwise
 */
int max6613_remove(struct max6613_dev *device)
{
    if (!device)
        return -ENODEV;

    /* Stop any conversion */
    MXC_ADC_DisableConversion();

    no_os_free(device);

    return 0;
}

/**
 * @brief Read raw ADC temperature value
 * @param device - MAX6613 descriptor
 * @param val - ADC value
 * @return 0 in case of success, negative error code otherwise
 */
int max6613_read_raw(struct max6613_dev *device, int32_t *value)
{
    int ret;
    if (!device)
        return -ENODEV;

#ifndef ADC_DMA
    adc_done = 0;
    adc_index = 0;

    ret = MXC_ADC_StartConversionAsync(adc_complete_cb);
    if (ret) {
        return ret;
    }

    while (!adc_done) {}
#endif

#ifdef ADC_DMA
    dma_done = 0;

    MXC_DMA_ReleaseChannel(0);

    ret = MXC_ADC_StartConversionDMA(&device->adc_conv, &adc_val[0], adc_dma_callback);
    if (ret) {
        return ret;
    }

    while (!dma_done) {}
#endif

    *value = adc_val[0] & 0xfff;
    return 0;
}

/**
 * @brief Convert raw ADC value to temperature in Celsius
 * @param val - Raw ADC value
 * @return Floating point temperature value in Celsius units
 */
float max6613_raw_to_celsius(int32_t value)
{
    return ((1.8455f - ((value * 2.048f) / 4096.0f)) / 0.01123f);
}
