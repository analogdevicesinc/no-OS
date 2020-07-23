/**
 * \file
 * \brief Type definitions for ADRV9001 clock settings
 * \copyright Analog Devices Inc. 2019. All rights reserved.
 * Released under the ADRV9001 API license, for more information see "LICENSE.txt" in the SDK
 */

#ifndef _ADI_ADRV9001_CLOCKSETTINGS_TYPES_H_
#define _ADI_ADRV9001_CLOCKSETTINGS_TYPES_H_

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
#include <stdbool.h>
#endif

/**
 *  \brief Enum of possible ADRV9001 HS divider settings
 */
typedef enum adi_adrv9001_HsDiv
{
    ADI_ADRV9001_HSDIV_4 = 0x0, /*!< High speed clock divide by 4 setting */
    ADI_ADRV9001_HSDIV_6 = 0x1, /*!< High speed clock divide by 6 setting */
    ADI_ADRV9001_HSDIV_8 = 0x2  /*!< High speed clock divide by 8 setting */
} adi_adrv9001_HsDiv_e;

/**
 *  \brief Available modes for the CLK_PLL that drives the High Speed Digital Clock
 */
typedef enum adi_adrv9001_ClkPllMode
{
    ADI_ADRV9001_CLK_PLL_HP_MODE = 0x0, /*!< Clock PLL HP Mode */
    ADI_ADRV9001_CLK_PLL_LP_MODE = 0x1, /*!< Clock PLL LP Mode */
} adi_adrv9001_ClkPllMode_e;

/**
 * \brief Enumeration of the a component power level
 */
typedef enum
{
    ADI_ADRV9001_COMPONENT_POWER_LEVEL_LOW = 0,  /*!< Low power */
    ADI_ADRV9001_COMPONENT_POWER_LEVEL_MEDIUM,   /*!< Medium power */
    ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH      /*!< High power */
} adi_adrv9001_ComponentPowerLevel_e;

/**
 *  \brief Enum of possible ADRV9001 internal Clock divisor settings
 */
typedef enum adi_adrv9001_InternalClock_Divisor
{
    ADI_ADRV9001_INTERNAL_CLOCK_DIV_1 = 0x1, /*!< Clock divide by 1 setting */
    ADI_ADRV9001_INTERNAL_CLOCK_DIV_2 = 0x2, /*!< Clock divide by 2 setting */
    ADI_ADRV9001_INTERNAL_CLOCK_DIV_3 = 0x3, /*!< Clock divide by 3 setting */
    ADI_ADRV9001_INTERNAL_CLOCK_DIV_4 = 0x4, /*!< Clock divide by 4 setting */
    ADI_ADRV9001_INTERNAL_CLOCK_DIV_5 = 0x5, /*!< Clock divide by 5 setting */
    ADI_ADRV9001_INTERNAL_CLOCK_DIV_6 = 0x6, /*!< Clock divide by 6 setting */
    ADI_ADRV9001_INTERNAL_CLOCK_DIV_7 = 0x7, /*!< Clock divide by 7 setting */
    ADI_ADRV9001_INTERNAL_CLOCK_DIV_8 = 0x8, /*!< Clock divide by 8 setting */
    ADI_ADRV9001_INTERNAL_CLOCK_DIV_9 = 0x9, /*!< Clock divide by 9 setting */
} adi_adrv9001_InternalClock_Divisor_e;

/**
 * \brief Enumerated list of options to use to set the ADRV9001 PLL Output and Input modes.
 */
typedef enum adi_adrv9001_PllLoMode
{
    ADI_ADRV9001_INT_LO1 = 0,       /*!< Internal RFPLL LO1 frequency will output from ADRV9001 */
    ADI_ADRV9001_INT_LO2 = 0,       /*!< Internal RFPLL LO2 frequency will output from ADRV9001 */
    ADI_ADRV9001_EXT_LO_OUTPUT = 1, /*!< External RFPLL LO in ADRV9001 to generate the LO frequency */
    ADI_ADRV9001_EXT_LO_INPUT1 = 2, /*!< Provided external LO1 in ADRV9001 to generate the LO frequency */
    ADI_ADRV9001_EXT_LO_INPUT2 = 3  /*!< Provided external LO2 in ADRV9001 to generate the LO frequency */
} adi_adrv9001_PllLoMode_e;

/**
 * \brief Enumerated list of options to use to set Differential/Single Ended for Ext Lo
 */
typedef enum adi_adrv9001_ExtLoType
{
    ADI_ADRV9001_EXT_LO_DIFFERENTIAL = 0,
    ADI_ADRV9001_EXT_LO_SINGLE_ENDED = 1

} adi_adrv9001_ExtLoType_e;

/**
 * \brief Enumerated list of options for Rx pin selection
 */
typedef enum adi_adrv9001_RxRfInputSel
{
    ADI_ADRV9001_RX_A = 0,
    ADI_ADRV9001_RX_B = 1

} adi_adrv9001_RxRfInputSel_e;

/**
 * \brief Enumerated list of RFPLL phase synchronization modes
 *
 * RFPLL Phase sync requires extra time to sync each time the RFPLL frequency
 * is changed. If RFPLL phase sync is not required, it may be desired to
 * disable the feature to allow the RFPLL to lock faster.
 *
 * Depending on the desired accuracy of the RFPLL phase sync, several options
 * are provided.
 */
typedef enum adi_adrv9001_RfPllMcs
{
    ADI_ADRV9001_RFPLLMCS_NOSYNC = 0,            /*!< Disable RFPLL phase synchronization */
    ADI_ADRV9001_RFPLLMCS_INIT_AND_SYNC = 1,     /*!< Enable RFPLL phase sync init only */
    ADI_ADRV9001_RFPLLMCS_INIT_AND_CONTTRACK = 2 /*!< Enable RFPLL phase sync init and track continuously */
} adi_adrv9001_RfPllMcs_e;

/**
 * \brief Enumerated list of options to use to set the LO source for the Rx/Tx mixers.
 */
typedef enum adi_adrv9001_LoSel
{
    ADI_ADRV9001_LOSEL_LO1 = 1,
    ADI_ADRV9001_LOSEL_LO2 = 2,
    ADI_ADRV9001_LOSEL_AUXLO = 3
} adi_adrv9001_LoSel_e;

/**
 * \brief LO divider mode
 */
typedef enum adi_adrv9001_LoDividerMode
{
    ADI_ADRV9001_LO_DIV_MODE_HIGH_PERFORMANCE = 0,
    ADI_ADRV9001_LO_DIV_MODE_LOW_POWER
} adi_adrv9001_LoDividerMode_e;

/**
 * \brief Enumerated list of options to use to set the LO Gen Power.
 */
typedef enum adi_adrv9001_LoGenPower
{
    ADI_ADRV9001_LOGENPOWER_RFPLL_LDO = 1,
    ADI_ADRV9001_LOGENPOWER_OFFCHIP = 2
} adi_adrv9001_LoGenPower_e;

/**
 *  \brief Data structure to hold digital clock settings
 */
typedef struct adi_adrv9001_ClockSettings
{
    uint32_t deviceClock_kHz;                         /*!< Device clock frequency in kHz */
    uint32_t clkPllVcoFreq_daHz;                      /*!< CLKPLL VCO frequency in daHz (dekaHertz, 10^1) */
    adi_adrv9001_HsDiv_e  clkPllHsDiv;                /*!< CLKPLL high speed clock divider */
    adi_adrv9001_ClkPllMode_e  clkPllMode;            /*!< CLKPLL Mode */
    adi_adrv9001_InternalClock_Divisor_e  clk1105Div; /*!< CLK1105 clock divider */
    adi_adrv9001_InternalClock_Divisor_e  armClkDiv;  /*!< ARM Clock divider */
    uint16_t armPowerSavingClkDiv;                    /*!< B0: ARM clock divider used by FW for power saving; Ranges from 1 to 256 */
    bool refClockOutEnable;                           /*!< Reference clock out enable; False: Disable reference clock out, True: Enable reference clock out */
    adi_adrv9001_ComponentPowerLevel_e  auxPllPower;  /*!< AUXPLL power level */
    adi_adrv9001_ComponentPowerLevel_e  clkPllPower;  /*!< CLKPLL power level */
    uint8_t padRefClkDrv;                             /*!< Output Clock Buffer Drive (valid 0-3) */
    uint32_t extLo1OutFreq_kHz;                       /*!< EXT LO1 output frequency in kHz */
    uint32_t extLo2OutFreq_kHz;                       /*!< EXT LO2 output frequency in kHz */
    adi_adrv9001_PllLoMode_e rfPll1LoMode;            /*!< internal LO generation for RF LO1, internal LO can be output, or external LO can be input. */
    adi_adrv9001_PllLoMode_e rfPll2LoMode;            /*!< internal LO generation for RF LO2, internal LO can be output, or external LO can be input. */
    adi_adrv9001_ExtLoType_e ext1LoType;              /*!< 0 = Differential, 1= Single Ended */
    adi_adrv9001_ExtLoType_e ext2LoType;              /*!< 0 = Differential, 1= Single Ended */
    adi_adrv9001_RxRfInputSel_e  rx1RfInputSel;       /*!< 0 = Rx1A, 1 = Rx1B */
    adi_adrv9001_RxRfInputSel_e  rx2RfInputSel;       /*!< 0 = Rx2A, 1 = Rx2B */
    uint16_t extLo1Divider;					          /*!< External LO1 Output divider (valid 2 to 1022) */
    uint16_t extLo2Divider;					          /*!< External LO2 Output divider (valid 2 to 1022) */
    adi_adrv9001_RfPllMcs_e rfPllPhaseSyncMode;       /*!< Set RF PLL phase synchronization mode. Adds extra time to lock RF PLL when PLL frequency changed. See enum for options */
    adi_adrv9001_LoSel_e rx1LoSelect;		          /*!< Rx1 LO select: 1 = LO1, 2 = LO2  */
    adi_adrv9001_LoSel_e rx2LoSelect;		          /*!< Rx2 LO select: 1 = LO1, 2 = LO2  */
    adi_adrv9001_LoSel_e tx1LoSelect;		          /*!< Tx1 LO select: 1 = LO1, 2 = LO2  */
    adi_adrv9001_LoSel_e tx2LoSelect;		          /*!< Tx2 LO select: 1 = LO1, 2 = LO2  */

    adi_adrv9001_LoDividerMode_e rx1LoDivMode;        /*!< RX1 LO divider mode select: 0: BEST_PHASE_NOISE; 1: BEST_POWER_SAVING */
    adi_adrv9001_LoDividerMode_e rx2LoDivMode;        /*!< RX2 LO divider mode select: 0: BEST_PHASE_NOISE; 1: BEST_POWER_SAVING */
    adi_adrv9001_LoDividerMode_e tx1LoDivMode;        /*!< TX1 LO divider mode select: 0: BEST_PHASE_NOISE; 1: BEST_POWER_SAVING */
    adi_adrv9001_LoDividerMode_e tx2LoDivMode;        /*!< TX2 LO divider mode select: 0: BEST_PHASE_NOISE; 1: BEST_POWER_SAVING */

    adi_adrv9001_LoGenPower_e loGen1Select;		      /*!< D0 - LoGen1 Sel: 0 = RFPLL1_LDO, 1 = OffChip */
    adi_adrv9001_LoGenPower_e loGen2Select;		      /*!< D1 - LoGen2 Sel: 0 = RFPLL2_LDO, 1 = OffChip */
} adi_adrv9001_ClockSettings_t;

#endif /* _ADI_ADRV9001_CLOCKSETTINGS_TYPES_H_ */
