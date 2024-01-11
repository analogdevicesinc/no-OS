/**
 * Copyright 2020 - 2021 Analog Devices Inc.
 * Released under the ADRV904X Software license, for more information
 * see the "LICENSE.pdf" file in this zip file.
 */

/**
 * \file
 * \brief Radio Control SDK Service API types
 *
 * SDK API Version: 2.10.0.4
 */

#ifndef ADRV904X_DFE_SVC_RADIOCTRL_T_H_
#define ADRV904X_DFE_SVC_RADIOCTRL_T_H_

/**
 * \brief Enumerated list for VSWR Fwd/Rev Settings.
 */
typedef enum
{
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_VSWR_FWD = 1,           /*!< Vswr Forward setting */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_VSWR_REV = 0,           /*!< Vswr Reverse setting */
} adi_adrv904x_DfeSvcRadioCtrlVswrFwdRev_e;

/**
 * \brief Enumerated list for Alarm Settings.
 */
typedef enum
{
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_ALARM_ON  = 1,           /*!< Alarm on  */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_ALARM_OFF = 0,           /*!< Alarm off */
} adi_adrv904x_DfeSvcAlarmState_e;

/**
 * \brief Enumerated list for Alarms.
 */
typedef enum
{
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_ALARM_0 = 0x01,              /*!< Alarm  0   */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_ALARM_1 = 0x02,              /*!< Alarm  1   */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_ALARM_2 = 0x04,              /*!< Alarm  2   */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_ALARM_3 = 0x08,              /*!< Alarm  3   */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_ALARM_4 = 0x10,              /*!< Alarm  4   */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_ALARM_5 = 0x20,              /*!< Alarm  5   */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_ALARM_6 = 0x40,              /*!< Alarm  6   */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_ALARM_7 = 0x80,              /*!< Alarm  7   */
} adi_adrv904x_DfeSvcRadioCtrlAlarms_e;

/**
 * \brief defined max number of alarms
 */
#define ADI_ADRV904X_DFE_SVC_RADIO_CTRL_ALARM_MAX    8


#endif  /* ADRV904X_DFE_SVC_RADIOCTRL_T_H_ */


