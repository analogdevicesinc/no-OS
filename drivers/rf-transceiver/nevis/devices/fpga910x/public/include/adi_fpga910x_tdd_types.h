/**
 * \file
 * \brief The TDD timing, sequence, and framing parameters are illustrated in the following timing diagram. \image html
 *        "FPGA TDD timing diagram.png" "FPGA TDD timing diagram"
 *
 * Copyright (c) 2022 - 2026 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 * Released under the ADRV910X API license.
 * For more information, see the "LICENSE.txt" file.
 */

#ifndef _ADI_FPGA910X_TDD_TYPES_H_
#define _ADI_FPGA910X_TDD_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "adi_adrv910x_types.h"
#include "adi_fpga910x_gpio_types.h"
#include "axi_adrv9102_gpio_regs.h"
#include "axi_adrv9102_ssi_regs.h"
#include "axi_adrv9102_tdd_regs.h"
#include "axi_adrv9102_top_regs.h"
#include "axi_adrv9102.h"
#include "stdbool.h"
#include "stdint.h"

/**
 * \brief TDD enables supported
 */
typedef enum adi_fpga910x_TddSelect
{
    ADI_FPGA910X_TDDSELECT_RX1 = 0,
    ADI_FPGA910X_TDDSELECT_TX1 = 1,
    ADI_FPGA910X_TDDSELECT_RX2 = 2,
    ADI_FPGA910X_TDDSELECT_ORX1 = 3,
    ADI_FPGA910X_TDDSELECT_ORX2 = 4,
    ADI_FPGA910X_TDDSELECT_DMA_RX1 = 5,
    ADI_FPGA910X_TDDSELECT_DMA_TX1 = 6,
    ADI_FPGA910X_TDDSELECT_DMA_RX2 = 7,
    ADI_FPGA910X_TDDSELECT_DMA_ORX1 = 8,
    ADI_FPGA910X_TDDSELECT_DMA_ORX2 = 9,
    ADI_FPGA910X_TDDSELECT_GENERAL_PURPOSE_1 = 10,
    ADI_FPGA910X_TDDSELECT_GENERAL_PURPOSE_2 = 11,
    ADI_FPGA910X_TDDSELECT_GENERAL_PURPOSE_3 = 12,
    ADI_FPGA910X_TDDSELECT_GENERAL_PURPOSE_4 = 13,
    ADI_FPGA910X_TDDSELECT_GENERAL_PURPOSE_5 = 14,
    ADI_FPGA910X_TDDSELECT_GENERAL_PURPOSE_6 = 15,
    ADI_FPGA910X_TDDSELECT_SMA1_TRIG = 16,
    ADI_FPGA910X_TDDSELECT_SMA2_TRIG = 17,
    ADI_FPGA910X_TDDSELECT_RX1_DMA_TRIG = 18,
    ADI_FPGA910X_TDDSELECT_TX1_DMA_TRIG = 19,
    ADI_FPGA910X_TDDSELECT_RX2_DMA_TRIG = 20,
    ADI_FPGA910X_TDDSELECT_ORX1_DMA_TRIG = 21,
    ADI_FPGA910X_TDDSELECT_ORX2_DMA_TRIG = 22,
    ADI_FPGA910X_TDDSELECT_SPI_TRIG = 23,
    ADI_FPGA910X_TDDSELECT_GPIO_TRIG = 24,
    ADI_FPGA910X_TDDSELECT_MAX_COUNT = 25
} adi_fpga910x_TddSelect_e;

/**
 * \brief Enumerated list of FPGA TDD trigger sources
 */
typedef enum adi_fpga910x_TddTrigger
{
    ADI_FPGA910X_TDD_TRIGGER_SMA_1 = 0,
    ADI_FPGA910X_TDD_TRIGGER_SMA_2 = 1,
    ADI_FPGA910X_TDD_TRIGGER_MCS = 2,
    ADI_FPGA910X_TDD_TRIGGER_GPIO = 3,
    ADI_FPGA910X_TDD_TRIGGER_IMMEDIATE = 4
} adi_fpga910x_TddTrigger_e;

/**
 * \brief TDD modes supported
 */
typedef enum adi_fpga910x_TddMode
{
    ADI_FPGA910X_TDD_ENABLE_LOW = 0,
    ADI_FPGA910X_TDD_ENABLE_HIGH = 1,
    ADI_FPGA910X_TDD_ENABLE_AUTO = 2
} adi_fpga910x_TddMode_e;

/**
* \brief TDD frame timing info
*
* The TDD counter starts at 0, ends at framePeriod, it can optionally be repeated,
* it can also be prematurely terminated.
*/
typedef struct adi_fpga910x_TddFrameTiming
{
    uint32_t framePeriod;                     /*!< Duration of TDD frame in tdd_clk cycles */
    uint32_t numberFrames;                    /*!< Desired number of TDD frames in sequence */
    bool repeatFrameSequence;                 /*!< Set this value to 1 to repeat the TDD frame sequence (if not, tdd frame comes to an end). */
    uint32_t frameSwitchTime;                 /*!< When current frame == (frameSwitchNum, frameSwitchTime), that frame is prematurely terminated. */
    uint32_t frameSwitchNum;                  /*!< When current frame == (frameSwitchNum, frameSwitchTime), that frame is prematurely terminated. */
    bool frameSwitchEnable;                   /*!< Set this value to 1 to enable frame premature switching. Otherwise, set to 0 */
    adi_fpga910x_TddTrigger_e frameStartTrig; /*!< Set this to the desired frame synchronization trigger source. */
    bool inMicroSeconds;                      /*!< Set this to true if all parameters are in micro seconds otherwise counts. */
} adi_fpga910x_TddFrameTiming_t;

/**
* \brief Timing of enable assert/deassert within a TDD frame
*
* All parameters can be between 0 and framePeriod (inclusive). A pair can be disabled by
* setting the assert and deassert parameters to be the same
*/
typedef struct adi_fpga910x_TddEnableTiming
{
    bool enable;                     /*!< Enable TDD auto or keep TDD enable asserted low. */
    uint32_t primaryAssert;          /*!< Number of tdd_clk cycles after start of TDD frame to perform primary assert */
    uint32_t primaryFrameAssert;     /*!< First frame in primary sequence in which enable is asserted */
    uint32_t primaryDeassert;        /*!< Number of tdd_clk cycles after start of TDD frame to perform primary deassert */
    uint32_t primaryFrameDeassert;   /*!< Final frame in primary sequence in which enable is deasserted */
    uint32_t secondaryAssert;        /*!< Number of tdd_clk cycles after start of TDD frame to perform secondary assert */
    uint32_t secondaryFrameAssert;   /*!< First frame in secondary sequence in which enable is asserted */
    uint32_t secondaryDeassert;      /*!< Number of tdd_clk cycles after start of TDD frame to perform secondary deassert */
    uint32_t secondaryFrameDeassert; /*!< Final frame in secondary sequence in which enable is deasserted */
    bool inMicroSeconds;             /*!< Set this to true if all parameters are in micro seconds otherwise counts. */
    adi_fpga910x_GpioPin_e gpioPin;  /*!< FPGA GPIO Pin which will be driven by the same pattern as the TDDSELECT pin */
    /**
    * If true, enables are synchronized with the datapath clocks, enable-on-time is rounded to the nearest sample count.
    * If false, enable-on-time is as specified
    */
    bool enableSyncToSample;
} adi_fpga910x_TddEnableTiming_t;

/**
 * \brief structure to hold TDD configuration
 */
typedef struct adi_fpga910x_TddConfig
{
    adi_fpga910x_TddFrameTiming_t frameTiming;
    adi_fpga910x_TddEnableTiming_t enables[ADI_FPGA910X_TDDSELECT_MAX_COUNT];
} adi_fpga910x_TddConfig_t;

#ifdef __cplusplus
}
#endif

#endif /* _ADI_FPGA910X_TDD_TYPES_H_ */
