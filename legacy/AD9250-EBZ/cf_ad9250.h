/***************************************************************************//**
 *   @file   cf_ad9250.h
 *   @brief  Header file of cf_ad9250 Driver.
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
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
#ifndef __CF_AD9250_H__
#define __CF_AD9250_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include "xparameters.h"

/******************************************************************************/
/******************* Macros and Constants Definitions *************************/
/******************************************************************************/
#ifdef _XPARAMETERS_PS_H_
    #define CF_BASEADDR      XPAR_AXI_AD9250_0_BASEADDR
    #define JESD_BASEADDR    XPAR_AXI_JESD204B_RX2_0_BASEADDR
    #define DDR_BASEADDR     XPAR_DDR_MEM_BASEADDR + 128*1024*1024
    #define DMA_BASEADDR     XPAR_AXI_DMA_0_BASEADDR
    #define UART_BASEADDR    XPS_UART1_BASEADDR
    #define SPI_BASEADDR     XPAR_AXI_SPI_0_BASEADDR
    #define LCD_BASEADDR     XPAR_AXI_GPIO_0_BASEADDR
#else
    #define CF_BASEADDR      XPAR_AXI_AD9250_0_BASEADDR
    #define JESD_BASEADDR    XPAR_AXI_JESD204B_RX2_0_BASEADDR
    #define DDR_BASEADDR     XPAR_DDR3_SDRAM_S_AXI_BASEADDR
    #define DMA_BASEADDR     XPAR_AXI_DMA_0_BASEADDR
    #define UART_BASEADDR    XPAR_RS232_UART_1_BASEADDR
    #define SPI_BASEADDR     XPAR_AXI_SPI_0_BASEADDR
    #define LCD_BASEADDR     XPAR_AXI_GPIO_0_BASEADDR
#endif

#ifdef _XPARAMETERS_PS_H_
    #define CYCLES_FOR_1_MS     100000
#else
    #define CYCLES_FOR_1_MS     800000
#endif

/* CF register map. */
#define CF_REG_VERSION          0x00
#define CF_REG_CAPTURE_CTRL     0x0C
#define CF_REG_ADC_STATUS       0x10
#define CF_REG_DATA_MONITOR     0x14
#define CF_REG_PN_TYPE          0x24
#define CF_REG_DATA_SELECT      0x28

/***************************************************************************//**
* This register controls the capture of ADC data. A 0 to 1 transition in the
* capture bit starts the capture. The capture count is the number of quad words
* minus one. To capture 16 quad words (64bit), sw must write 0x0_0000 followed
* by 0x1_000f.
*******************************************************************************/
/* CF_REG_CAPTURE_CTRL bit definition. */
#define CF_CAPTURE_CTRL_CAPTURE_START(x)    (((x) & 0x1) << 16)
#define CF_CAPTURE_CTRL_CAPTURE_COUNT(x)    ((((x) - 1) & 0xFFFF) << 0)

/***************************************************************************//**
* This register indicates the status of an ADC data capture. Software must clear
* all the bits before starting a data capture.
*******************************************************************************/
/* CF_REG_ADC_STATUS bit definition. */
#define CF_ADC_STATUS_UNDERFLOW         (1 << 2) // (Write 1 to clear)
#define CF_ADC_STATUS_OVERFLOW          (1 << 1) // (Write 1 to clear)
#define CF_ADC_STATUS_BUSY              (1 << 0) // (Read Only)

/***************************************************************************//**
* This register indicates the status of ADC data monitoring (regardless of data
* capture). These bits are set whenever a corresponding error is detected.
* Software must clear these bits and read them back to monitor the status.
*******************************************************************************/
/* CF_REG_DATA_MONITOR bit definition. */
#define CF_DATA_MONITOR_PN_ERR          (1 << 2) // (Write 1 to clear)
#define CF_DATA_MONITOR_PN_OUT_OF_SYNC  (1 << 1) // (Write 1 to clear)
#define CF_DATA_MONITOR_OVER_RNG        (1 << 0) // (Write 1 to clear)

/***************************************************************************//**
* This register selects the pseudo-random sequence used by the PN Monitor.
* PN9 sequence is used when bit 0 is cleared.
* PN23 sequence is used when bit 0 is set.
*******************************************************************************/
/* CF_REG_PN_TYPE bit definition. */
#define  CF_PN_TYPE_BIT(x)              (((x) & 0x1) << 0) // PN9(0) or PN23(1)

/***************************************************************************//**
* This register controls the data select from DDR. The incoming data word's
* bytes are assumed to be rising edge first. That is first byte appears on the
* rising edge followed by second byte on the falling edge.
*******************************************************************************/
/* CF_REG_DATA_SELECT bit definition. */
#define CF_DATA_SELECT_BIT(x)        (((x) & 0x1) << 0) // Rising (0) or
                                                        // Falling (1) edge first
/* JESD register map */
#define JESD_REG_VERSION        0x00
#define JESD_REG_LANE_CTRL      0x04
#define JESD_REG_SYSREF         0x08
#define JESD_REG_FRAMES         0x0C
#define JESD_REG_DELAY          0x10
#define JESD_REG_TEST           0x14
#define JESD_REG_BUFCNT         0x18
#define JESD_REG_INIT_DATA0     0x1c
#define JESD_REG_INIT_DATA1     0x20
#define JESD_REG_INIT_DATA2     0x24
#define JESD_REG_INIT_DATA3     0x28
#define JESD_REG_TEST_MFCNT     0x2c
#define JESD_REG_TEST_ILACNT    0x30
#define JESD_REG_TEST_ERRCNT    0x34

/* JESD_REG_LANE_CTRL bit definition. */
#define JESD_LANE_CTRL_SYSREF_SEL(x)    (((x) & 0x1) << 4) // Select sw(0x1) or hw(0x0) sysref generation
#define JESD_LANE_CTRL_LANE_SYNC_EN(x)  (((x) & 0x1) << 3) // Enable (0x1) lane synchronization
#define JESD_LANE_CTRL_SCR_EN(x)        (((x) & 0x1) << 2) // Enable (0x1) scrambling
#define JESD_LANE_CTRL_SYSREF_EN(x)     (((x) & 0x1) << 1) // Enable (0x1) re-alignment at every sysref pulses
#define JESD_LANE_CTRL_ERR_DISB(x)      (((x) & 0x1) << 0) // Disable (0x1) error reporting via sync

/* JESD_REG_SYSREF bit definition. */
#define JESD_SYSREF_INT(x)              (((x) & 0x1) << 0) // Software controlled sysref (0->1 transition)

/* JESD_REG_FRAMES bit definition. */
#define JESD_FRAMES_FRM_CNT(x)          ((((x) - 1) & 0x1F) << 8) // Number of frames per multi-frame
#define JESD_FRAMES_BYTE_CNT(x)         ((((x) - 1) & 0xFF) << 0) // Number of bytes (octets) per frame

/* JESD_REG_DELAY bit definition. */
#define JESD_DELAY_BUFF_DELAY(x)        (((x) & 0x1FFF) << 0) // Buffer delay from multi-frame

/* JESD_REG_TEST bit definition. */
#define JESD_TEST_MODE(x)               (((x) & 0x3) << 2) // Test mode (?)
#define JESD_TEST_LANE_SEL(x)           (((x) & 0x3) << 0) // Select lanes (0, 1, 2 or 3) for reporting

/*  The following registers are based on lane_sel

    JESD_REG_BUFCNT      - Buffer count (latency)
    JESD_REG_INIT_DATA0  - ILS Data 1
    JESD_REG_INIT_DATA1  - ILS Data 1
    JESD_REG_INIT_DATA2  - ILS Data 2
    JESD_REG_INIT_DATA3  - ILS Data 3
    JESD_REG_TEST_MFCNT  - Test MF count (?)
    JESD_REG_TEST_ILACNT - Test ILA count (?)
    JESD_REG_TEST_ERRCNT - Test ERROR count (?)
*/

/******************************************************************************/
/************************** Types Declarations ********************************/
/******************************************************************************/
typedef enum _TestModes
{
    TEST_DISABLE = 0,
    MIDSCALE,
    POS_FULLSCALE,
    NEG_FULLSCALE,
    CHECKERBOARD,
    PN_23_SEQUENCE,
    PN_9_SEQUENCE,
    ONE_ZERO_TOGGLE

}typeTestModes;

typedef enum _OutputModes
{
    OFFSET_BINARY = 0,
    TWOS_COMPLEMENT

}typeOutputModes;

/*******************************************************************************/
/************************ Functions Declarations *******************************/
/*******************************************************************************/

/*! Initializes JESD FPGA core. */
void jesd_core_setup(void);

/*! Initializes DMA core. */
void dma_core_setup(void);

/*! Captures a specified number of samples from the ADC. */
void adc_capture(uint32_t size, uint32_t address);

/*! Configures the AD9250 device to run in the selected test mode. */
void adc_test(uint32_t mode, uint32_t format);

#endif /* __CF_AD9250_H__ */
