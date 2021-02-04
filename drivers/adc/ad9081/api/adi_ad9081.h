/*!
 * @brief     API header file
 *            This file contains all the publicly exposed methods and data
 *            structures to interface with API.
 *
 * @copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * @addtogroup ADI_AD9081
 * @{
 */
#ifndef __ADI_AD9081_H__
#define __ADI_AD9081_H__

/*============= I N C L U D E S ============*/
#include "adi_cms_api_common.h"

/*============= D E F I N E S ==============*/
#define AD9081_ID 0x9081
#define AD9081_DAC_CLK_FREQ_HZ_MIN 2850000000ULL
#define AD9081_DAC_CLK_FREQ_HZ_MAX 12600000000ULL
#define AD9081_ADC_CLK_FREQ_HZ_MIN 1425000000ULL
#define AD9081_ADC_CLK_FREQ_HZ_MAX 4200000000ULL
#ifdef AD9209
#define AD9081_ID 0x9209
#define AD9081_DAC_CLK_FREQ_HZ_MIN 2850000000ULL
#define AD9081_DAC_CLK_FREQ_HZ_MAX 12600000000ULL
#define AD9081_ADC_CLK_FREQ_HZ_MIN 1425000000ULL
#define AD9081_ADC_CLK_FREQ_HZ_MAX 6300000000ULL
#endif
#define AD9081_REF_CLK_FREQ_HZ_MIN 100000000ULL
#define AD9081_REF_CLK_FREQ_HZ_MAX 2000000000ULL

#define AD9081_JESD_SER_COUNT 8
#define AD9081_JESD_DESER_COUNT 8

#define AD9081_USE_FLOATING_TYPE 0
#define AD9081_USE_SPI_BURST_MODE 0

/*!
 * @brief Enumerates Chip Output Resolution
 */
typedef enum {
	AD9081_CHIP_OUT_RES_16BIT = 0x0, /*!< 16Bit */
	AD9081_CHIP_OUT_RES_15BIT = 0x1, /*!< 15Bit */
	AD9081_CHIP_OUT_RES_14BIT = 0x2, /*!< 14Bit */
	AD9081_CHIP_OUT_RES_13BIT = 0x3, /*!< 13Bit */
	AD9081_CHIP_OUT_RES_12BIT = 0x4, /*!< 12Bit */
	AD9081_CHIP_OUT_RES_11BIT = 0x5, /*!< 11Bit */
	AD9081_CHIP_OUT_RES_10BIT = 0x6, /*!< 10Bit */
	AD9081_CHIP_OUT_RES_09BIT = 0x7, /*!<  9Bit */
	AD9081_CHIP_OUT_RES_08BIT = 0x8 /*!<  8Bit */
} adi_ad9081_chip_output_res_e;

/*!
 * @brief Enumerates DAC Select
 */
typedef enum {
	AD9081_DAC_NONE = 0x0, /*!< No DAC */
	AD9081_DAC_0 = 0x1, /*!< DAC0 */
	AD9081_DAC_1 = 0x2, /*!< DAC1 */
	AD9081_DAC_2 = 0x4, /*!< DAC2 */
	AD9081_DAC_3 = 0x8, /*!< DAC3 */
	AD9081_DAC_ALL = 0x0F /*!< ALL DACs */
} adi_ad9081_dac_select_e;

/*!
 * @brief Enumerates DAC Channel Select
 */
typedef enum {
	AD9081_DAC_CH_NONE = 0x00, /*!< No Channel */
	AD9081_DAC_CH_0 = 0x01, /*!< Channel 0 */
	AD9081_DAC_CH_1 = 0x02, /*!< Channel 1 */
	AD9081_DAC_CH_2 = 0x04, /*!< Channel 2 */
	AD9081_DAC_CH_3 = 0x08, /*!< Channel 3 */
	AD9081_DAC_CH_4 = 0x10, /*!< Channel 4 */
	AD9081_DAC_CH_5 = 0x20, /*!< Channel 5 */
	AD9081_DAC_CH_6 = 0x40, /*!< Channel 6 */
	AD9081_DAC_CH_7 = 0x80, /*!< Channel 7 */
	AD9081_DAC_CH_ALL = 0xFF /*!< ALL Channels */
} adi_ad9081_dac_channel_select_e;

/*!
 * @brief Enumerates DAC Mode Switch Group Select
 */
typedef enum {
	AD9081_DAC_MODE_SWITCH_GROUP_NONE = 0x00, /*!< No Group */
	AD9081_DAC_MODE_SWITCH_GROUP_0 = 0x01, /*!< Group 0 (DAC0 & DAC1) */
	AD9081_DAC_MODE_SWITCH_GROUP_1 = 0x02, /*!< Group 1 (DAC2 & DAC3) */
	AD9081_DAC_MODE_SWITCH_GROUP_ALL = 0x03, /*!< All Groups */
} adi_ad9081_dac_mode_switch_group_select_e;

/*!
 * @brief Enumerates DAC Mode Select
 */
typedef enum {
	AD9081_DAC_MODE_0 = 0x00, /*!< I0.Q0 -> DAC0, I1.Q1 -> DAC1 */
	AD9081_DAC_MODE_1 =
		0x01, /*!< (I0 + I1) / 2 -> DAC0, (Q0 + Q1) / 2 -> DAC1, Data Path NCOs Bypassed */
	AD9081_DAC_MODE_2 =
		0x02, /*!< I0 -> DAC0, Q0 -> DAC1, Datapath0 NCO Bypassed, Datapath1 Unused */
	AD9081_DAC_MODE_3 =
		0x03, /*!< (I0 + I1) / 2 -> DAC0, DAC1 Output Tied To Midscale */
} adi_ad9081_dac_mode_e;

/*!
 * @brief Enumerates ADC Select
 */
typedef enum {
	AD9081_ADC_NONE = 0x0, /*!< No ADC */
	AD9081_ADC_0 = 0x1, /*!< ADC0 */
	AD9081_ADC_1 = 0x2, /*!< ADC1 */
	AD9081_ADC_2 = 0x4, /*!< ADC2 */
	AD9081_ADC_3 = 0x8, /*!< ADC3 */
	AD9081_ADC_ALL = 0x0F /*!< ALL ADCs */
} adi_ad9081_adc_select_e;

/*!
 * @brief Enumerates ADC NCO Modes
 */
typedef enum {
	AD9081_ADC_NCO_VIF = 0, /*!< Variable IF Mode */
	AD9081_ADC_NCO_ZIF = 1, /*!< Zero IF Mode */
	AD9081_ADC_NCO_FS_4_IF = 2, /*!< Fs/4 Hz IF Mode */
	AD9081_ADC_NCO_TEST = 3, /*!< Test Mode */
	AD9081_ADC_NCO_INVALID = 4 /*!< Invalid NCO Mode */
} adi_ad9081_adc_nco_mode_e;

/*!
 * @brief Enumerates ADC->Coarse DDC CrossBar
 */
typedef enum {
	AD9081_ADC_4_ADC_REAL_MODE = 0, /*!< Quad ADC Real Mode */
	AD9081_ADC_4_ADC_COMP_MODE = 1, /*!< Quad ADC Complex Mode */
	AD9081_ADC_2_ADC_REAL_MODE = 2, /*!< Dual ADC Real Mode */
	AD9081_ADC_2_ADC_COMP_MODE = 3 /*!< Dual ADC Complex MOde */
} adi_ad9081_adc_adc_to_cddc_xbar_e;

/*!
 * @brief Enumerates Coarse DDC0/1->Fine DDC0 CrossBar
 */
typedef enum {
	AD9081_ADC_CDDC0_TO_FDDC0 = 0,
	AD9081_ADC_CDDC1_TO_FDDC0 = 1
} adi_ad9081_adc_cddc_to_fddc0_xbar_e;

/*!
 * @brief Enumerates Coarse DDC0/1->Fine DDC1 CrossBar
 */
typedef enum {
	AD9081_ADC_CDDC0_TO_FDDC1 = 0,
	AD9081_ADC_CDDC1_TO_FDDC1 = 2
} adi_ad9081_adc_cddc_to_fddc1_xbar_e;

/*!
 * @brief Enumerates Coarse DDC0/1->Fine DDC2 CrossBar
 */
typedef enum {
	AD9081_ADC_CDDC0_TO_FDDC2 = 0,
	AD9081_ADC_CDDC1_TO_FDDC2 = 4
} adi_ad9081_adc_cddc_to_fddc2_xbar_e;

/*!
 * @brief Enumerates Coarse DDC0/1->Fine DDC3 CrossBar
 */
typedef enum {
	AD9081_ADC_CDDC0_TO_FDDC3 = 0,
	AD9081_ADC_CDDC1_TO_FDDC3 = 8
} adi_ad9081_adc_cddc_to_fddc3_xbar_e;

/*!
 * @brief Enumerates Coarse DDC2/3->Fine DDC4 CrossBar
 */
typedef enum {
	AD9081_ADC_CDDC2_TO_FDDC4 = 0,
	AD9081_ADC_CDDC3_TO_FDDC4 = 0x10
} adi_ad9081_adc_cddc_to_fddc4_xbar_e;

/*!
 * @brief Enumerates Coarse DDC2/3->Fine DDC5 CrossBar
 */
typedef enum {
	AD9081_ADC_CDDC2_TO_FDDC5 = 0,
	AD9081_ADC_CDDC3_TO_FDDC5 = 0x20
} adi_ad9081_adc_cddc_to_fddc5_xbar_e;

/*!
 * @brief Enumerates Coarse DDC2/3->Fine DDC6 CrossBar
 */
typedef enum {
	AD9081_ADC_CDDC2_TO_FDDC6 = 0,
	AD9081_ADC_CDDC3_TO_FDDC6 = 0x40
} adi_ad9081_adc_cddc_to_fddc6_xbar_e;

/*!
 * @brief Enumerates Coarse DDC2/3->Fine DDC7 CrossBar
 */
typedef enum {
	AD9081_ADC_CDDC2_TO_FDDC7 = 0,
	AD9081_ADC_CDDC3_TO_FDDC7 = 0x80
} adi_ad9081_adc_cddc_to_fddc7_xbar_e;

/*!
 * @brief Enumerates ADC coarse DDC select
 */
typedef enum {
	AD9081_ADC_CDDC_NONE = 0x00, /*!< No COARSE DDC */
	AD9081_ADC_CDDC_0 = 0x01, /*!< COARSE DDC 0 */
	AD9081_ADC_CDDC_1 = 0x02, /*!< COARSE DDC 1 */
	AD9081_ADC_CDDC_2 = 0x04, /*!< COARSE DDC 2 */
	AD9081_ADC_CDDC_3 = 0x08, /*!< COARSE DDC 3 */
	AD9081_ADC_CDDC_ALL = 0x0F /*!< ALL COARSE DDCs */
} adi_ad9081_adc_coarse_ddc_select_e;

/*!
 * @brief Enumerates ADC fine DDC select
 */
typedef enum {
	AD9081_ADC_FDDC_NONE = 0x00, /*!< No FINE DDC */
	AD9081_ADC_FDDC_0 = 0x01, /*!< FINE DDC 0 */
	AD9081_ADC_FDDC_1 = 0x02, /*!< FINE DDC 1 */
	AD9081_ADC_FDDC_2 = 0x04, /*!< FINE DDC 2 */
	AD9081_ADC_FDDC_3 = 0x08, /*!< FINE DDC 3 */
	AD9081_ADC_FDDC_4 = 0x10, /*!< FINE DDC 4 */
	AD9081_ADC_FDDC_5 = 0x20, /*!< FINE DDC 5 */
	AD9081_ADC_FDDC_6 = 0x40, /*!< FINE DDC 6 */
	AD9081_ADC_FDDC_7 = 0x80, /*!< FINE DDC 7 */
	AD9081_ADC_FDDC_ALL = 0xFF /*!< ALL FINE DDCs */
} adi_ad9081_adc_fine_ddc_select_e;

/*!
 * @brief Enumerates ADC Coarse DDC decimation
 */
typedef enum {
	AD9081_CDDC_DCM_1 = 0xC, /*!< Decimate by 1 */
	AD9081_CDDC_DCM_2 = 0x0, /*!< Decimate by 2 */
	AD9081_CDDC_DCM_3 = 0x8, /*!< Decimate by 3 */
	AD9081_CDDC_DCM_4 = 0x1, /*!< Decimate by 4 */
	AD9081_CDDC_DCM_6 = 0x5, /*!< Decimate by 6 */
	AD9081_CDDC_DCM_8 = 0x2, /*!< Decimate by 8 */
	AD9081_CDDC_DCM_9 = 0x9, /*!< Decimate by 9 */
	AD9081_CDDC_DCM_12 = 0x6, /*!< Decimate by 12 */
	AD9081_CDDC_DCM_16 = 0x3, /*!< Decimate by 16 */
	AD9081_CDDC_DCM_18 = 0xA, /*!< Decimate by 18 */
	AD9081_CDDC_DCM_24 = 0x7, /*!< Decimate by 24 */
	AD9081_CDDC_DCM_36 = 0xB /*!< Decimate by 36 */
} adi_ad9081_adc_coarse_ddc_dcm_e;

/*!
* @brief Enumerates ADC Fine DDC decimation
*/
typedef enum {
	AD9081_FDDC_DCM_1 = 0x8, /*!< Decimate by 1 */
	AD9081_FDDC_DCM_2 = 0x0, /*!< Decimate by 2 */
	AD9081_FDDC_DCM_3 = 0x4, /*!< Decimate by 3 */
	AD9081_FDDC_DCM_4 = 0x1, /*!< Decimate by 4 */
	AD9081_FDDC_DCM_6 = 0x5, /*!< Decimate by 6 */
	AD9081_FDDC_DCM_8 = 0x2, /*!< Decimate by 8 */
	AD9081_FDDC_DCM_12 = 0x6, /*!< Decimate by 12 */
	AD9081_FDDC_DCM_16 = 0x3, /*!< Decimate by 16 */
	AD9081_FDDC_DCM_24 = 0x7, /*!< Decimate by 24 */
} adi_ad9081_adc_fine_ddc_dcm_e;

/*!
* @brief Enumerates ADC Fine DDC I/Q Converter
*/
typedef enum {
	AD9081_FDDC_0_I = 0x0, /*!< FDDC0 I */
	AD9081_FDDC_0_Q = 0x1, /*!< FDDC0 Q */
	AD9081_FDDC_1_I = 0x2, /*!< FDDC1 I */
	AD9081_FDDC_1_Q = 0x3, /*!< FDDC1 Q */
	AD9081_FDDC_2_I = 0x4, /*!< FDDC2 I */
	AD9081_FDDC_2_Q = 0x5, /*!< FDDC2 Q */
	AD9081_FDDC_3_I = 0x6, /*!< FDDC3 I */
	AD9081_FDDC_3_Q = 0x7, /*!< FDDC3 Q */
	AD9081_FDDC_4_I = 0x8, /*!< FDDC4 I */
	AD9081_FDDC_4_Q = 0x9, /*!< FDDC4 Q */
	AD9081_FDDC_5_I = 0xA, /*!< FDDC5 I */
	AD9081_FDDC_5_Q = 0xB, /*!< FDDC5 Q */
	AD9081_FDDC_6_I = 0xC, /*!< FDDC6 I */
	AD9081_FDDC_6_Q = 0xD, /*!< FDDC6 Q */
	AD9081_FDDC_7_I = 0xE, /*!< FDDC7 I */
	AD9081_FDDC_7_Q = 0xF, /*!< FDDC7 Q */
} adi_ad9081_adc_fine_ddc_converter_e;

/*!
 *@brief Enumerates ADC Nyquist Zone
 */
typedef enum {
	AD9081_ADC_NYQUIST_ZONE_ODD = 0x00, /*!< Odd  Zone */
	AD9081_ADC_NYQUIST_ZONE_EVEN = 0x01 /*!< Even Zone */
} adi_ad9081_adc_nyquist_zone_e;

/*!
 *@brief Enumerates PFIR Control Page
 */
typedef enum {
	AD9081_ADC_PFIR_ADC_PAIR0 = 0x01, /*!< PFIR ADC Pair0 */
	AD9081_ADC_PFIR_ADC_PAIR1 = 0x02, /*!< PFIR ADC Pair1 */
	AD9081_ADC_PFIR_ADC_PAIR_ALL = 0x03 /*!< PFIR ADC Pair All */
} adi_ad9081_adc_pfir_ctl_page_e;

/*!
 *@brief Enumerates PFIR Coefficient Page
 */
typedef enum {
	AD9081_ADC_PFIR_COEFF_PAGE0 = 0x01, /*!< PFIR Coefficient Page0 */
	AD9081_ADC_PFIR_COEFF_PAGE1 = 0x02, /*!< PFIR Coefficient Page1 */
	AD9081_ADC_PFIR_COEFF_PAGE2 = 0x04, /*!< PFIR Coefficient Page2 */
	AD9081_ADC_PFIR_COEFF_PAGE3 = 0x08, /*!< PFIR Coefficient Page3 */
	AD9081_ADC_PFIR_COEFF_PAGE_ALL = 0x0F /*!< PFIR Coefficient Page All */
} adi_ad9081_adc_pfir_coeff_page_e;

/*!
 *@brief Enumerates ADC PFIR I-Mode
 */
typedef enum {
	AD9081_ADC_PFIR_I_MODE_DISABLE =
		0x0, /*!< Disabled (filters bypassed) */
	AD9081_ADC_PFIR_I_MODE_REAL_N4 = 0x1, /*!< Real N/4 tap filter */
	AD9081_ADC_PFIR_I_MODE_REAL_N2 = 0x2, /*!< Real N/2 tap filter */
	AD9081_ADC_PFIR_I_MODE_MATRIX = 0x4, /*!< N/4 tap matrix mode */
	AD9081_ADC_PFIR_I_MODE_COMPLEX_FULL = 0x5, /*!< N/3 tap full complex */
	AD9081_ADC_PFIR_I_MODE_COMPLEX_HALF =
		0x6, /*!< Half complex using 2 N/2 tap filters */
	AD9081_ADC_PFIR_I_MODE_REAL_N = 0x7 /*!< Real N tap filter */
} adi_ad9081_adc_pfir_i_mode_e;

/*!
 *@brief Enumerates ADC PFIR Q-Mode
 */
typedef enum {
	AD9081_ADC_PFIR_Q_MODE_DISABLE =
		0x0, /*!< Disabled (filters bypassed) */
	AD9081_ADC_PFIR_Q_MODE_REAL_N4 = 0x1, /*!< Real N/4 tap filter */
	AD9081_ADC_PFIR_Q_MODE_REAL_N2 = 0x2, /*!< Real N/2 tap filter */
	AD9081_ADC_PFIR_Q_MODE_MATRIX = 0x4, /*!< N/4 tap matrix mode */
	AD9081_ADC_PFIR_Q_MODE_COMPLEX_FULL = 0x5, /*!< N/3 tap full complex */
	AD9081_ADC_PFIR_Q_MODE_COMPLEX_HALF =
		0x6, /*!< Half complex using 2 N/2 tap filters */
	AD9081_ADC_PFIR_Q_MODE_REAL_N = 0x7 /*!< Real N tap filter */
} adi_ad9081_adc_pfir_q_mode_e;

/*!
 *@brief Enumerates ADC PFIR Gain
 */
typedef enum {
	AD9081_ADC_PFIR_GAIN_N12DB = 0x6, /*!< -12dB */
	AD9081_ADC_PFIR_GAIN_N6DB = 0x7, /*!<  -6dB */
	AD9081_ADC_PFIR_GAIN_0DB = 0x0, /*!<   0dB */
	AD9081_ADC_PFIR_GAIN_P6DB = 0x1, /*!<   6dB */
	AD9081_ADC_PFIR_GAIN_P12DB = 0x2 /*!<  12dB */
} adi_ad9081_adc_pfir_gain_e;

/*!
 * @brief Enumerates Link Select
 */
typedef enum {
	AD9081_LINK_NONE = 0x0, /*!< No Link */
	AD9081_LINK_0 = 0x1, /*!< Link 0 */
	AD9081_LINK_1 = 0x2, /*!< Link 1 */
	AD9081_LINK_ALL = 0x3 /*!< All Links */
} adi_ad9081_jesd_link_select_e;

/*!
 * @brief Enumerates JESD Rx Test Data Source (Inject Point)
 */
typedef enum {
	AD9081_JESD_RX_PRBS_TEST_DATA_SRC_LANE =
		0x0, /*!< Lane Data As Test Data Source */
	AD9081_JESD_RX_PRBS_TEST_DATA_SRC_SAMPLE =
		0x1 /*!< Sample Data As Test Data Source (M0 Only) */
} adi_ad9081_jesd_rx_prbs_test_data_src_e;

/*!
 * @brief Enumerates JESD Rx PRBS Test Mode
 */
typedef enum {
	AD9081_JESD_RX_PRBS_TEST_MODE_OFF = 0x0, /*!< Disable PRBS Test Mode */
	AD9081_JESD_RX_PRBS_TEST_MODE_PRBS7 = 0x1, /*!< PRBS7 */
	AD9081_JESD_RX_PRBS_TEST_MODE_PRBS9 = 0x2, /*!< PRBS9 */
	AD9081_JESD_RX_PRBS_TEST_MODE_PRBS15 = 0x3, /*!< PRBS15 */
	AD9081_JESD_RX_PRBS_TEST_MODE_PRBS31 = 0x4 /*!< PRBS31 */
} adi_ad9081_jesd_rx_prbs_test_mode_e;

/*!
 * @brief Enumerates JESD Tx Test Data Source (Inject Point)
 */
typedef enum {
	AD9081_JESD_TX_TEST_DATA_SAMPLE =
		0x0, /*!< Sample Data As Test Data Source */
	AD9081_JESD_TX_TEST_DATA_PHY = 0x1, /*!< PHY Data As Test Data Source */
	AD9081_JESD_TX_TEST_DATA_SCRAMBLER_INPUT =
		0x2 /*!< Scrambler Input Data As Data Source */
} adi_ad9081_jesd_tx_test_data_src_e;

/*!
 * @brief Enumerates JESD Tx Test Mode
 */
typedef enum {
	AD9081_JESD_TX_TEST_MODE_DISABLED = 0x0, /*!< Disable Test Mode */
	AD9081_JESD_TX_TEST_MODE_CHECKER_BOARD =
		0x1, /*!< Checker Board Test Mode */
	AD9081_JESD_TX_TEST_MODE_WORD_TOGGLE =
		0x2, /*!< Word Toggle Test Mode */
	AD9081_JESD_TX_TEST_MODE_PN31 = 0x3, /*!< PN31 Test Mode */
	AD9081_JESD_TX_TEST_MODE_PN15 = 0x5, /*!< PN15 Test Mode */
	AD9081_JESD_TX_TEST_MODE_PN7 = 0x7, /*!< PN7  Test Mode */
	AD9081_JESD_TX_TEST_MODE_RAMP = 0x8, /*!< Ramp Test Mode */
	AD9081_JESD_TX_TEST_MODE_USER_REPEAT =
		0xE, /*!< Repeated User Data Test Mode */
	AD9081_JESD_TX_TEST_MODE_USER_SINGLE =
		0xF /*!< Single Time User Data Test Mode */
} adi_ad9081_jesd_tx_test_mode_e;

/*!
 * @brief Enumerates ADC Test Mode Type
 */
typedef enum {
	AD9081_TMODE_OFF = 0x0, /*!< Normal Operation */
	AD9081_TMODE_MIDSCALE = 0x1, /*!< Midscale Short */
	AD9081_TMODE_POS_FULL = 0x2, /*!< Positive Full-Scale, 0x7FFF */
	AD9081_TMODE_NEG_FULL = 0x3, /*!< Negative Full-Scale, 0x8000 */
	AD9081_TMODE_ALT_CHECKER =
		0x4, /*!< Alternating Checker Board, 0x5555-0xAAAA */
	AD9081_TMODE_PN23 = 0x5, /*!< PN23 Sequence */
	AD9081_TMODE_PN9 = 0x6, /*!< PN9  Sequence */
	AD9081_TMODE_1_0_TOGG = 0x7, /*!< 1/0 Word Toggle, 0x0000-0xFFFF */
	AD9081_TMODE_USER_PAT = 0x8, /*!< User Pattern Test Mode */
	AD9081_TMODE_PN7 = 0x9, /*!< PN7 Sequence */
	AD9081_TMODE_PN15 = 0xA, /*!< PN15 Sequence */
	AD9081_TMODE_PN31 = 0xB, /*!< PN31 Sequence */
	AD9081_TMODE_RAMP = 0xF /*!< Ramp Output */
} adi_ad9081_test_mode_e;

/*!
 * @brief Enumerates DESER CTLE Settings
 */
typedef enum {
	IL_GREATER_THAN_10DB = 0, /*!< CTLE settings with > 10dB */
	IL_LESS_THAN_10DB = 1 /*!< CTLE settings with < 10dB */
} adi_ad9081_il_settings_e;

/*!
 * @brief Enumerates Reset Operation
 */
typedef enum {
	AD9081_SOFT_RESET = 0, /*!< Soft Reset */
	AD9081_HARD_RESET = 1, /*!< Hard Reset */
	AD9081_SOFT_RESET_AND_INIT = 2, /*!< Soft Reset Then Init */
	AD9081_HARD_RESET_AND_INIT = 3 /*!< Hard Reset Then Init */
} adi_ad9081_reset_e;

/*!
 * @brief Enumerates JESD Deserilizer Operation
 */
typedef enum {
	AD9081_FULL_RATE = 0, /*!< Full rate operation */
	AD9081_HALF_RATE = 1, /*!< Half rate operation */
	AD9081_QUART_RATE = 2 /*!< Quarter rate operation */
} adi_ad9081_deser_mode_e;

/*!
 * @brief JESD PRBS Test Result Structure
 */
typedef struct {
	uint32_t phy_prbs_err_cnt; /*!< PRBS Test Error Count */
	uint8_t phy_prbs_pass; /*!< PRBS Test Status */
	uint8_t phy_src_err_cnt; /*!< PRBS Test Source Error Count */
} adi_ad9081_prbs_test_t;

/*!
 * @brief JESD SPO Structure
 */
typedef struct {
	uint8_t left_spo; /*!< Left good SPO */
	uint8_t right_spo; /*!< Right good SPO */
} adi_ad9081_spo_t;

/*!
 * @brief Enumerates JESD Serializer Swing Settings
 */
typedef enum {
	AD9081_SER_SWING_1000 = 0, /*!< 1000 mV Swing */
	AD9081_SER_SWING_850 = 1, /*!< 850 mV Swing */
	AD9081_SER_SWING_750 = 2, /*!< 750 mV Swing */
	AD9081_SER_SWING_500 = 3 /*!< 500 mV Swing */
} adi_ad9081_ser_swing_e;

/*!
 * @brief Enumerates JESD Serializer Pre-Emphasis Settings
 */
typedef enum {
	AD9081_SER_PRE_EMP_0DB = 0, /*!< 0 db Pre-Emphasis */
	AD9081_SER_PRE_EMP_3DB = 1, /*!< 3 db Pre-Emphasis */
	AD9081_SER_PRE_EMP_6DB = 2 /*!< 6 db Pre-Emphasis */
} adi_ad9081_ser_pre_emp_e;

/*!
 * @brief Enumerates JESD Serializer Post-Emphasis Settings
 */
typedef enum {
	AD9081_SER_POST_EMP_0DB = 0, /*!< 0 db Post-Emphasis */
	AD9081_SER_POST_EMP_3DB = 1, /*!< 3 db Post-Emphasis */
	AD9081_SER_POST_EMP_6DB = 2, /*!< 6 db Post-Emphasis */
	AD9081_SER_POST_EMP_9DB = 3, /*!< 9 db Post-Emphasis */
	AD9081_SER_POST_EMP_12DB = 4 /*!< 12 db Post-Emphasis */
} adi_ad9081_ser_post_emp_e;

/*!
 * @brief Per lane JESD Serializer Settings
 */
typedef struct {
	adi_ad9081_ser_swing_e swing_setting;
	adi_ad9081_ser_pre_emp_e pre_emp_setting;
	adi_ad9081_ser_post_emp_e post_emp_setting;
} adi_ad9081_ser_lane_settings_t;

/*!
 * @brief Full JESD Serializer Settings Structure
 */
typedef struct {
	adi_ad9081_ser_lane_settings_t lane_settings[8];
	uint8_t invert_mask;
	uint8_t lane_mapping[2][8];
} adi_ad9081_ser_settings_t;

/*!
 * @brief Full JESD Deserializer Settings Structure
 */
typedef struct {
	uint8_t boost_mask;
	uint8_t invert_mask;
	uint8_t ctle_filter[8];
	uint8_t lane_mapping[2][8];
} adi_ad9081_des_settings_t;

/*!
 * @brief Full JESD SERDES Settings Structure
 */
typedef struct {
	adi_ad9081_ser_settings_t ser_settings; /*! Jesd Tx Serializer Settings */
	adi_ad9081_des_settings_t
		des_settings; /*! Jesd Rx Deserializer Settings */
} adi_ad9081_serdes_settings_t;

/*!
 * @brief JTX Virtual Converter Selection Structure
 */
typedef struct {
	uint8_t virtual_converter0_index; /*! Index for JTX virtual converter0  */
	uint8_t virtual_converter1_index; /*! Index for JTX virtual converter1  */
	uint8_t virtual_converter2_index; /*! Index for JTX virtual converter2  */
	uint8_t virtual_converter3_index; /*! Index for JTX virtual converter3  */
	uint8_t virtual_converter4_index; /*! Index for JTX virtual converter4  */
	uint8_t virtual_converter5_index; /*! Index for JTX virtual converter5  */
	uint8_t virtual_converter6_index; /*! Index for JTX virtual converter6  */
	uint8_t virtual_converter7_index; /*! Index for JTX virtual converter7  */
	uint8_t virtual_converter8_index; /*! Index for JTX virtual converter8  */
	uint8_t virtual_converter9_index; /*! Index for JTX virtual converter9  */
	uint8_t virtual_convertera_index; /*! Index for JTX virtual converter10 */
	uint8_t virtual_converterb_index; /*! Index for JTX virtual converter11 */
	uint8_t virtual_converterc_index; /*! Index for JTX virtual converter12 */
	uint8_t virtual_converterd_index; /*! Index for JTX virtual converter13 */
	uint8_t virtual_convertere_index; /*! Index for JTX virtual converter14 */
	uint8_t virtual_converterf_index; /*! Index for JTX virtual converter15 */
} adi_ad9081_jtx_conv_sel_t;

/*!
 * @brief Device Hardware Abstract Layer Structure
 */
typedef struct {
	void *user_data; /*!< Pointer to connect customer data related to this device */

	adi_cms_spi_sdo_config_e
		sdo; /*!< SPI interface 3/4 wire mode configuration */
	adi_cms_spi_msb_config_e
		msb; /*!< SPI interface MSB/LSB bit order configuration */
	adi_cms_spi_addr_inc_e
		addr_inc; /*!< SPI interface address increment configuration */

	adi_spi_xfer_t
		spi_xfer; /*!< Function pointer to hal spi access function */
	adi_delay_us_t delay_us; /*!< Function pointer to hal delay function */
	adi_hw_open_t
		hw_open; /*!< Function pointer to hal initialization function */
	adi_hw_close_t
		hw_close; /*!< Function pointer to hal de-initialization function */
	adi_log_write_t
		log_write; /*!< Function pointer to hal log write function */
	adi_tx_en_pin_ctrl_t
		tx_en_pin_ctrl; /*!< Function pointer to hal tx_enable pin control function */
	adi_reset_pin_ctrl_t
		reset_pin_ctrl; /*!< Function pointer to hal reset# pin control function */
} adi_ad9081_hal_t;

/*!
 * @brief Device Internal Information Structure
 */
typedef struct {
	uint64_t dev_freq_hz; /*!< Device clock frequency in Hz */
	uint64_t dac_freq_hz; /*!< DAC clock frequency in Hz */
	uint64_t adc_freq_hz; /*!< ADC clock frequency in Hz */
	uint8_t dev_rev; /*!< Device revision, 0:r0, 1:r1, 2:r1r, 3:r2 */
} adi_ad9081_info_t;

/*!
 * @brief Device Structure
 */
typedef struct {
	adi_ad9081_hal_t hal_info;
	adi_ad9081_info_t dev_info;
	adi_ad9081_serdes_settings_t serdes_info;
} adi_ad9081_device_t;

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Open hardware platform
 *         Just call user callback for function pointer 'hw_open'. Please note this is optional if user
 *         configure hardware platform by themselves.
 *
 * @param  device Pointer to the device structure
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_device_hw_open(adi_ad9081_device_t *device);

/**
 * @brief  Close hardware platform
 *         Just call user callback for function pointer 'hw_close'. Please note this is optional if user
 *         close hardware platform by themselves.
 *
 * @param  device Pointer to the device structure
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_device_hw_close(adi_ad9081_device_t *device);

/**
 * @brief  Initialize ad9081 device
 *         This API will configure device SPI working mode, and check power supplies status. Must be called
 *         after platform SPI master is already initialized and adi_ad9081_device_reset() is called.
 *
 * @param  device Pointer to the device structure
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_device_init(adi_ad9081_device_t *device);

/**
 * @brief  De-initialize device
 *		   This API will do hard then soft reset.
 *
 * @param  device Pointer to the device structure
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_device_deinit(adi_ad9081_device_t *device);

/**
 * @brief  Configure ad9081 direct loopback mode
 *         Note ADC sample rate must be same as DAC sample rate to get this loopback mode working.
 *         Should be called after adi_ad9081_device_init().
 *
 * @param  device  Pointer to the device structure
 * @param  mode    bit[0]: 1 to enable ana loop back feature
 *                 bit[1]: 1 to enable direct adc data to dac
 *                         0 to control adc data overflow before looping back to dac
 * @param  mapping bit[1:0]: controls which ADC map to DAC0, 0: ADC0, 1: ADC1
 *                 bit[3:2]: controls which ADC map to DAC1, 0: ADC0, 1: ADC1
 *                 bit[5:4]: controls which ADC map to DAC2, 0: ADC0, 1: ADC1
 *                 bit[7:6]: controls which ADC map to DAC3, 0: ADC0, 1: ADC1
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_device_direct_loopback_set(adi_ad9081_device_t *device,
					      uint8_t mode, uint8_t mapping);

/**
 * @brief  Calculate FTW word and modulus value
 *
 * @param  device    Pointer to the device structure
 * @param  freq      ADC or DAC freq (need to divide coarse decimation or main interpolation for find ddc or channel)
 * @param  nco_shift NCO value in Hz
 * @param  ftw       Calculated FTW value in 48bits
 * @param  a         Numerator value in 48bits
 * @param  b         Dominator value in 48bits
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_device_calc_nco_ftw(adi_ad9081_device_t *device,
				       uint64_t freq, int64_t nco_shift,
				       uint64_t *ftw, uint64_t *a, uint64_t *b);

/**
 * @brief  Perform SPI interface configuration
 *         This API will be called by adi_ad9081_device_init().
 *
 * @param  device Pointer to the device structure
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_device_spi_config(adi_ad9081_device_t *device);

/**
 * @brief  Perform SPI register write access to device
 *
 * @param  device   Pointer to the device structure
 * @param  addr     SPI address to which the value of data parameter shall be written
 * @param  data     8-bit value to be written to SPI register defined by the address parameter.
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_device_spi_register_set(adi_ad9081_device_t *device,
					   uint16_t addr, uint8_t data);

/**
 * @brief  Perform SPI register read access from device
 *
 * @param  device   Pointer to the device structure
 * @param  addr     SPI address from which the value of data parameter shall be read,
 * @param  data     Pointer to an 8-bit variable to which the value of the
 *                  SPI register at the address defined by address parameter shall be stored.
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_device_spi_register_get(adi_ad9081_device_t *device,
					   uint16_t addr, uint8_t *data);

/**
 * @brief  Perform SPI write access to device JRX cbus registers
 *
 * @param  device   Pointer to the device structure
 * @param  addr     SPI address to which the value of data parameter shall be written
 * @param  data     8-bit value to be written to SPI register defined by the address parameter.
 * @param  lane     lane index, 0 ~ 7
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_device_cbusjrx_register_set(adi_ad9081_device_t *device,
					       uint8_t addr, uint8_t data,
					       uint8_t lane);

/**
 * @brief  Perform SPI read access from device JRX cbus registers
 *
 * @param  device   Pointer to the device structure
 * @param  addr     SPI address from which the value of data parameter shall be read,
 * @param  data     Pointer to an 8-bit variable to which the value of the
 *                  SPI register at the address defined by address parameter shall be stored.
 * @param  lane     lane index, 0 ~ 7
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_device_cbusjrx_register_get(adi_ad9081_device_t *device,
					       uint8_t addr, uint8_t *data,
					       uint8_t lane);

/**
 * @brief  Perform SPI write access to device JTX cbus registers
 *
 * @param  device   Pointer to the device structure
 * @param  addr     SPI address to which the value of data parameter shall be written
 * @param  data     8-bit value to be written to SPI register defined by the address parameter.
 * @param  lane     lane index, 0 ~ 7
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_device_cbusjtx_register_set(adi_ad9081_device_t *device,
					       uint8_t addr, uint8_t data,
					       uint8_t lane);

/**
 * @brief  Perform SPI read access from device JTX cbus registers
 *
 * @param  device   Pointer to the device structure
 * @param  addr     SPI address from which the value of data parameter shall be read,
 * @param  data     Pointer to an 8-bit variable to which the value of the
 *                  SPI register at the address defined by address parameter shall be stored.
 * @param  lane     lane index, 0 ~ 7
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_device_cbusjtx_register_get(adi_ad9081_device_t *device,
					       uint8_t addr, uint8_t *data,
					       uint8_t lane);

/**
 * @brief  Perform SPI write access to device PLL cbus registers
 *
 * @param  device   Pointer to the device structure
 * @param  addr     SPI address to which the value of data parameter shall be written
 * @param  data     8-bit value to be written to SPI register defined by the address parameter.
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_device_cbuspll_register_set(adi_ad9081_device_t *device,
					       uint8_t addr, uint8_t data);

/**
 * @brief  Perform SPI read access from device PLL cbus registers
 *
 * @param  device   Pointer to the device structure
 * @param  addr     SPI address from which the value of data parameter shall be read,
 * @param  data     Pointer to an 8-bit variable to which the value of the
 *                  SPI register at the address defined by address parameter shall be stored.
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_device_cbuspll_register_get(adi_ad9081_device_t *device,
					       uint8_t addr, uint8_t *data);

/**
 * @brief  Reset device
 *         Issues a hard reset or soft reset of the device.
 *         Performs a full reset of device via the hardware pin (hard) or
 *         via the SPI register (soft).
 *         Resetting all SPI registers to default and triggering the required
 *         initialization sequence.
 *         adi_ad9081_device_reset() will be called if operation is AD9081_SOFT_RESET_AND_INIT or
 *         AD9081_HARD_RESET_AND_INIT.
 *
 * @param  device    Pointer to the device structure
 * @param  operation A parameter to indicate reset operation. @see adi_ad9081_reset_e
 *                   AD9081_SOFT_RESET indicates a software reset is required.
 *                   AD9081_HARD_RESET indicates a hardware reset is required.
 *                   AD9081_SOFT_RESET_AND_INIT indicates a software reset + initialization is required.
 *                   AD9081_HARD_RESET_AND_INIT indicates a hardware reset + initialization is required.
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_device_reset(adi_ad9081_device_t *device,
				adi_ad9081_reset_e operation);

/**
 * @brief  Get pll lock status
 *         This API will be called by adi_ad9081_device_clk_config_set().
 *
 * @param  device     Pointer to the device structure
 * @param  status     Pointer to pll lock fast (bit1) and slow (bit0) status
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_device_clk_pll_lock_status_get(adi_ad9081_device_t *device,
						  uint8_t *status);

/**
 * @brief  Configure the clock circuitry based on the desired clocks frequencies
 *         This API should be called after adi_ad9081_device_init().
 *
 * @param  device     Pointer to the device structure
 * @param  dac_clk_hz Desired dac clock frequency
 * @param  adc_clk_hz Desired adc clock frequency
 * @param  ref_clk_hz Reference clock frequency
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_device_clk_config_set(adi_ad9081_device_t *device,
					 uint64_t dac_clk_hz,
					 uint64_t adc_clk_hz,
					 uint64_t ref_clk_hz);

/**
 * @brief  power up/down analog clock receiver
 *         This API will be called by adi_ad9081_device_clk_config_set().
 *
 * @param  device   Pointer to the device structure
 * @param  enable   Enable clock receiver
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_device_aclk_receiver_enable_set(adi_ad9081_device_t *device,
						   uint8_t enable);

/**
 * @brief  Set Main Auto Clock Gen Enable
 *
 * @param  device  Pointer to the device structure
 * @param  enable  0x0 ~ 0xf
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_device_main_auto_clk_gen_enable(adi_ad9081_device_t *device,
						   uint8_t enable);

/**
 * @brief  Get chip identification data
 *         Read-back product type, identification and revision data. Should be called after
 *         adi_ad9081_device_clk_config_set().
 *
 * @param  device   Pointer to the device structure
 * @param  chip_id  @see adi_cms_chip_id_t
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_device_chip_id_get(adi_ad9081_device_t *device,
				      adi_cms_chip_id_t *chip_id);

/**
 * @brief  Get API revision
 *
 * @param  device    Pointer to the device structure.
 * @param  rev_major Pointer to variable to store the major revision
 * @param  rev_minor Pointer to variable to store the minor revision
 * @param  rev_rc    Pointer to variable to store the rc    revision
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_device_api_revision_get(adi_ad9081_device_t *device,
					   uint8_t *rev_major,
					   uint8_t *rev_minor, uint8_t *rev_rc);

/**
 * @brief  Get Laminate ID
 *         This API will be called after adi_ad9081_device_clk_config_set().
 *
 * @param  device    Pointer to the device structure.
 * @param  id        Pointer to silicon laminate id.
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_device_laminate_id_get(adi_ad9081_device_t *device,
					  uint8_t *id);

/**
 * @brief  Get DIE ID
 *         This API will be called after adi_ad9081_device_clk_config_set().
 *
 * @param  device    Pointer to the device structure.
 * @param  id        Pointer to silicon DIE id.
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_device_die_id_get(adi_ad9081_device_t *device, uint8_t *id);

/**
 * @brief  Do some pre-settings for nco sync.
 *
 * @param  device       Pointer to the device structure
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_device_nco_sync_pre(adi_ad9081_device_t *device);

/**
 * @brief  Do some post-settings for nco sync.
 *
 * @param  device       Pointer to the device structure
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_device_nco_sync_post(adi_ad9081_device_t *device);

/**
 * @brief  Startup Tx
 *         This API will be called after adi_ad9081_device_clk_config_set().
 *
 * @param  device         Pointer to the device structure
 * @param  main_interp    Main interpolator
 * @param  chan_interp    Channel interpolator
 * @param  dac_chan       Enabled channels for each DAC
 * @param  main_shift     Main NCO shift
 * @param  chan_shift     Channel NCO shift
 * @param  jesd_param     JRX JESD link settings
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_device_startup_tx(adi_ad9081_device_t *device,
				     uint8_t main_interp, uint8_t chan_interp,
				     uint8_t dac_chan[4], int64_t main_shift[4],
				     int64_t chan_shift[8],
				     adi_cms_jesd_param_t *jesd_param);

/**
 * @brief  Startup Tx As NCO Test Mode
 *         This API will be called after adi_ad9081_device_clk_config_set().
 *
 * @param  device         Pointer to the device structure
 * @param  main_interp    Main interpolator
 * @param  chan_interp    Channel interpolator
 * @param  dac_chan       Enabled channels for each DAC
 * @param  main_shift     Main NCO shift
 * @param  chan_shift     Channel NCO shift
 * @param  dc_offset      DC offset for NCO test mode
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t
adi_ad9081_device_startup_nco_test(adi_ad9081_device_t *device,
				   uint8_t main_interp, uint8_t chan_interp,
				   uint8_t dac_chan[4], int64_t main_shift[4],
				   int64_t chan_shift[8], uint16_t dc_offset);

/**
 * @brief  Startup Rx
 *         This API will be called after adi_ad9081_device_clk_config_set().
 *
 * @param  device         Pointer to the device structure
 * @param  cddcs          Coarse DDC selection
 * @param  fddcs          Fine   DDC selection
 * @param  cddc_shift     Coarse DDC NCO shift
 * @param  fddc_shift     Foarse DDC NCO shift
 * @param  cddc_dcm       Coarse DDC decimation value
 * @param  fddc_dcm       Fine   DDC decimation value
 * @param  cc2r_en        Coase  DDC complex-to-real enable
 * @param  fc2r_en        Fine   DDC complex-to-real enable
 * @param  jesd_param     JTX JESD link settings
 * @param  jesd_conv_sel  JTX virutal converter selection, @see adi_ad9081_adc_fine_ddc_converter_e
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_device_startup_rx(
	adi_ad9081_device_t *device, uint8_t cddcs, uint8_t fddcs,
	int64_t cddc_shift[4], int64_t fddc_shift[8], uint8_t cddc_dcm[4],
	uint8_t fddc_dcm[8], uint8_t cc2r_en[4], uint8_t fc2r_en[8],
	adi_cms_jesd_param_t jesd_param[2],
	adi_ad9081_jtx_conv_sel_t jesd_conv_sel[2]);

/**
 * @brief  Select active DACs
 *
 * @param  device   Pointer to the device structure
 * @param  dacs     DAC mask
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_select_set(adi_ad9081_device_t *device, uint8_t dacs);

/**
 * @brief  Select active channels
 *
 * @param  device   Pointer to the device structure
 * @param  channels Channel Mask
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_chan_select_set(adi_ad9081_device_t *device,
				       uint8_t channels);

/**
 * @brief  Select active DACs and active channels
 *
 * @param  device   Pointer to the device structure
 * @param  dacs     DAC mask
 * @param  channels Channel Mask
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_duc_select_set(adi_ad9081_device_t *device, uint8_t dacs,
				      uint8_t channels);

/**
 * @brief  Power up or Power down Dac
 *
 * @param  device   Pointer to the device structure
 * @param  dacs     Target DAC to power up
 * @param  enable   Enable setting for dac power.
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_power_up_set(adi_ad9081_device_t *device, uint8_t dacs,
				    uint8_t enable);

/**
 * @brief  Enable test tone generation
 *         This API will be called by adi_ad9081_device_startup_nco_test().
 *
 * @param  device   Pointer to the device structure
 * @param  channels Channel mask
 * @param  enable   1:enable, 0:disable
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_dc_test_tone_en_set(adi_ad9081_device_t *device,
					   uint8_t channels, uint8_t enable);

/**
 * @brief  DC test tone amplitude setting
 *         This API will be called by adi_ad9081_device_startup_nco_test().
 *
 * @param  device   Pointer to the device structure
 * @param  channels Channel mask
 * @param  offset   Test tone offset value
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_dc_test_tone_offset_set(adi_ad9081_device_t *device,
					       uint8_t channels,
					       uint16_t offset);

/**
 * @brief  Enable test tone generation
 *         This API will be called by adi_ad9081_device_startup_nco_test().
 *
 * @param  device   Pointer to the device structure
 * @param  dacs     DAC mask
 * @param  enable   1:enable, 0:disable
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_duc_main_dc_test_tone_en_set(adi_ad9081_device_t *device,
						    uint8_t dacs,
						    uint8_t enable);

/**
 * @brief  DC test tone amplitude setting
 *         This API will be called by adi_ad9081_device_startup_nco_test().
 *
 * @param  device   Pointer to the device structure
 * @param  dacs     DAC mask
 * @param  offset   Test tone offset value
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t
adi_ad9081_dac_duc_main_dc_test_tone_offset_set(adi_ad9081_device_t *device,
						uint8_t dacs, uint16_t offset);

/**
 * @brief  Set DAC working mode
 *         Call after adi_ad9081_device_startup_tx()
 *
 * @param  device   Pointer to the device structure
 * @param  groups   Mode switch groups, @see adi_ad9081_dac_mode_switch_group_select_e
 * @param  mode     Working mode, @see adi_ad9081_dac_mode_e
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t
adi_ad9081_dac_mode_set(adi_ad9081_device_t *device,
			adi_ad9081_dac_mode_switch_group_select_e groups,
			adi_ad9081_dac_mode_e mode);

/**
 * @brief  Set DAC complex modulation enable
 *         Call after adi_ad9081_device_startup_tx()
 *
 * @param  device   Pointer to the device structure
 * @param  groups   Mode switch groups, @see adi_ad9081_dac_mode_switch_group_select_e
 * @param  enable   1 to enable complex modulation
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_complex_modulation_enable_set(
	adi_ad9081_device_t *device,
	adi_ad9081_dac_mode_switch_group_select_e groups, uint8_t enable);

/**
 * @brief  Set Fine DUC gain
 *         Call after adi_ad9081_device_startup_tx().
 *
 * @param  device   Pointer to the device structure
 * @param  channels Channel mask
 * @param  gain     Gain value (0~4095)
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_duc_nco_gain_set(adi_ad9081_device_t *device,
					uint8_t channels, uint16_t gain);

/**
 * @brief  Set Fine DUC gain
 *         Call after adi_ad9081_device_startup_tx().
 *
 * @param  device   Pointer to the device structure
 * @param  gains    Channel gain values (0~4095)
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_duc_nco_gains_set(adi_ad9081_device_t *device,
					 uint16_t gains[8]);

/**
 * @brief  Set Fine DUC Skew
 *         Call after adi_ad9081_device_startup_tx().
 *
 * @param  device   Pointer to the device structure
 * @param  channels Channel mask
 * @param  skew     Channel skew adjust value
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_duc_chan_skew_set(adi_ad9081_device_t *device,
					 uint8_t channels, uint8_t skew);

/**
 * @brief  Reset NCO
 *
 * @param  device         Pointer to the device structure
 * @param  chan_nco_reset 1 to reset channel NCO
 * @param  main_nco_reset 1 to reset main NCO
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_duc_nco_reset_set(adi_ad9081_device_t *device,
					 uint8_t chan_nco_reset,
					 uint8_t main_nco_reset);

/**
 * @brief  Enable NCO on DAC and Channels
 *
 * @param  device   Pointer to the device structure
 * @param  dacs     DAC mask
 * @param  channels Channel mask
 * @param  enable   NCO enable
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_duc_nco_enable_set(adi_ad9081_device_t *device,
					  uint8_t dacs, uint8_t channels,
					  uint8_t enable);

/**
 * @brief  Enable phase offset for the NCOs on the DAC and Channels
 *         Call after adi_ad9081_device_startup_tx().
 *
 * @param  device           Pointer to the device structure
 * @param  dacs             DAC mask
 * @param  dac_phase_offset Desired DAC channel NCO phase offset
 * @param  channels         Channel mask
 * @param  ch_phase_offset  Desired fine NCO phase offset
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_duc_nco_phase_offset_set(adi_ad9081_device_t *device,
						uint8_t dacs,
						uint16_t dac_phase_offset,
						uint8_t channels,
						uint16_t ch_phase_offset);

/**
 * @brief  Configure NCO's FTW
 *         Call after adi_ad9081_device_startup_tx().
 *
 * @param  device      Pointer to the device structure
 * @param  dacs        DAC mask, like AD9081_DAC_0, ...
 * @param  channels    Channel mask, like AD9081_DAC_CH_0, ...
 * @param  ftw         48bit frequency tuning word
 * @param  acc_modulus
 * @param  acc_delta
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_duc_nco_ftw_set(adi_ad9081_device_t *device,
				       uint8_t dacs, uint8_t channels,
				       uint64_t ftw, uint64_t acc_modulus,
				       uint64_t acc_delta);

/**
 * @brief  Configure NCO Shift Freq
 *         Call after adi_ad9081_device_startup_tx().
 *
 * @param  device       Pointer to the device structure
 * @param  dacs         DAC mask, like AD9081_DAC_0, ...
 * @param  channels     Channel mask, like AD9081_DAC_CH_0, ...
 * @param  nco_shift_hz NCO shift freq in Hz
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_duc_nco_set(adi_ad9081_device_t *device, uint8_t dacs,
				   uint8_t channels, int64_t nco_shift_hz);

/**
 * @brief  Configure Main NCO's FFH FTW
 *         Call after adi_ad9081_device_startup_tx().
 *
 * @param  device     Pointer to the device structure
 * @param  dacs       DAC mask, like AD9081_DAC_0, ...
 * @param  hopf_index Select which hopping frequency tuning word to set (1 ~ 31)
 * @param  hopf_ftw   32bit hopping frequency tuning word
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_duc_main_nco_hopf_ftw_set(adi_ad9081_device_t *device,
						 uint8_t dacs,
						 uint8_t hopf_index,
						 uint32_t hopf_ftw);

/**
 * @brief  Configure Main NCO's FFH mode
 *         Call after adi_ad9081_device_startup_tx().
 *
 * @param  device    Pointer to the device structure
 * @param  dacs      DAC mask, like AD9081_DAC_0, ...
 * @param  hopf_mode hopping mode, 0: phase continuous switch, 1: phase in-continuous switch, 2: phase coherent switch
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_duc_main_nco_hopf_mode_set(adi_ad9081_device_t *device,
						  uint8_t dacs,
						  uint8_t hopf_mode);

/**
 * @brief  Select Which Main NCO's Hopping Frequency to use
 *         Call after adi_ad9081_device_startup_tx().
 *
 * @param  device     Pointer to the device structure
 * @param  dacs       DAC mask, like AD9081_DAC_0, ...
 * @param  hopf_index Select which hopping frequency tuning word to use (0 ~ 31)
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_duc_main_nco_hopf_select_set(adi_ad9081_device_t *device,
						    uint8_t dacs,
						    uint8_t hopf_index);

/**
 * @brief  Enable/Disable GPIO No-Glitch Option
 *         Call after adi_ad9081_device_startup_tx().
 *
 * @param  device     Pointer to the device structure
 * @param  dacs       DAC mask, like AD9081_DAC_0, ...
 * @param  enable     0 or 1, if gpio aligned strobe signal not given, set this bit to use internal glitch free logic to
 *                    remove GPIO glitch, but this will introduce longer response, since the glitch remove logic will
 *                    need 4 pipes delay.
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_duc_main_nco_hopf_gpio_no_glitch_en_set(
	adi_ad9081_device_t *device, uint8_t dacs, uint8_t enable);

/**
 * @brief  Enable/Disable Main NCO's Hopping Frequency Selection Coming From GPIOs
 *         Call after adi_ad9081_device_startup_tx().
 *
 * @param  device     Pointer to the device structure
 * @param  enable     0 or 1, disable or enable frequency hopping tuning word selection from gpio.
 *                        sync1_outbp: ffh0
 *                        sync1_outbn: ffh1
 *                        gpio0      : ffh2
 *                        gpio1      : ffh3
 *                        gpio2      : ffh4
 *                        gpio3      : ffh5
 *                        gpio4      : ffh6
 *                        gpio5      : ffh strobe
 *                        [ffh1 ~ ffh0]: seletc DAC, 0 - DAC0, 1 - DAC1, 2 - DAC2, 3 - DAC3
 *                        [ffh6 ~ ffh2]: select fast frequence ftw
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t
adi_ad9081_dac_duc_main_nco_hopf_gpio_as_hop_en_set(adi_ad9081_device_t *device,
						    uint8_t enable);

/**
 * @brief  Set GPIO as SYNC1 out
 *
 * @param  device     Pointer to the device structure
 * @param  mode       0: link1_sync, 1: link1_sync with diff mode
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_gpio_as_sync1_out_set(adi_ad9081_device_t *device,
					     uint8_t mode);

/**
 * @brief  Enable/Disable GPIOs Output For PA Protection
 *
 * @param  device     Pointer to the device structure
 * @param  enable     0 or 1, disable or enable gpio output
 *                        gpio0 - pa0_en
 *                        gpio1 - pa1_en
 *                        gpio2 - pa2_en
 *                        gpio3 - pa3_en
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_gpio_as_pa_en_set(adi_ad9081_device_t *device,
					 uint8_t enable);

/**
 * @brief  Enable/Disable GPIOs Input For Tx Enable Control
 *
 * @param  device     Pointer to the device structure
 * @param  enable     0 or 1, disable or enable gpio input
 *                        gpio4 - txen1
 *                        gpio5 - txen3
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_gpio_as_tx_en_set(adi_ad9081_device_t *device,
					 uint8_t enable);

/**
 * @brief  Configure the Digital Up Conversion Channels
 *         Call after adi_ad9081_device_startup_tx().
 *
 * @param  device      Pointer to the device structure
 * @param  main_interp Main interpolation
 * @param  ch_interp   Channel interpolation
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_interpolation_set(adi_ad9081_device_t *device,
					 uint8_t main_interp,
					 uint8_t ch_interp);

/**
 * @brief  Enable Soft-Off Gain
 *
 * @param  device      Pointer to the device structure
 * @param  dacs        Target DAC Channel Output
 * @param  enable      1 to enable soft-off gain
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_soft_off_gain_enable_set(adi_ad9081_device_t *device,
						uint8_t dacs, uint8_t enable);

/**
 * @brief  Enable New Soft-Off Gain Block
 *
 * @param  device      Pointer to the device structure
 * @param  dacs        Target DAC Channel Output
 * @param  enable      1 to enable new soft-off gain block
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_soft_off_new_gain_enable_set(adi_ad9081_device_t *device,
						    uint8_t dacs,
						    uint8_t enable);

/**
 * @brief  Set Soft-Off Gain Ramp Rate
 *
 * @param  device      Pointer to the device structure
 * @param  dacs        Target DAC Channel Output
 * @param  rate        Ramp rate. The gain will ramp from 0-1 (or 1-0) in 32 steps over 2^(code+8) DAC clock periods.
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_soft_off_gain_ramp_rate_set(adi_ad9081_device_t *device,
						   uint8_t dacs, uint8_t rate);

/**
 * @brief  Enable Soft-Off
 *
 * @param  device      Pointer to the device structure
 * @param  dacs        Target DAC Channel Output
 * @param  enable      bit0: spi soft-off,           bit1: txen soft-off
 *                     bit2: rotate soft-off,        bit3: jesd error soft-off
 *                     bit4: hwip err soft-off,      bit5:
 *                     bit6: long paerr soft-off,    bit7: short paerr soft-off
 *                     bit8: dll unlock soft-off,    bit9: 204c crc err soft-off
 *                     bit10:                        bit11: hi-lo-fail soft-off
 *                     bit12: slew rate err soft-off
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_soft_off_enable_set(adi_ad9081_device_t *device,
					   uint8_t dacs, uint16_t enable);

/**
 * @brief  Enable Soft-On
 *
 * @param  device      Pointer to the device structure
 * @param  dacs        Target DAC Channel Output
 * @param  enable      bit4: hi-lo-recv soft-on, bit5:
 *                     bit6: long level soft-on, bit7: spi soft-on (force soft-on when gain is 0)
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_soft_on_enable_set(adi_ad9081_device_t *device,
					  uint8_t dacs, uint8_t enable);

/**
 * @brief  Set Long Averaging configuration for PA
 *
 * @param  device            Pointer to the device structure
 * @param  dacs              Target DAC Channel Output
 * @param  enable            1 to enable average power calculation and error detection
 * @param  averaging_time    Time for long averaging
 * @param  average_threshold Long average power threshold
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_long_pa_set(adi_ad9081_device_t *device, uint8_t dacs,
				   uint8_t enable, uint8_t averaging_time,
				   uint16_t average_threshold);

/**
 * @brief  Get Long Average Power
 *
 * @param  device Pointer to the device structure
 * @param  dacs   Target DAC Channel Output
 * @param  power  Long average power (= I^2 + Q^2)
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_long_pa_power_get(adi_ad9081_device_t *device,
					 uint8_t dacs, uint16_t *power);

/**
 * @brief  Set Short Averaging configuration for PA
 *
 * @param  device            Pointer to the device structure
 * @param  dacs              Target DAC Channel Output
 * @param  enable            1 to enable average power calculation and error detection
 * @param  averaging_time    Time for short averaging
 * @param  average_threshold Short average power threshold
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_short_pa_set(adi_ad9081_device_t *device, uint8_t dacs,
				    uint8_t enable, uint8_t averaging_time,
				    uint16_t average_threshold);

/**
 * @brief  Get Short Average Power
 *
 * @param  device Pointer to the device structure
 * @param  dacs   Target DAC Channel Output
 * @param  power  Short average power (= I^2 + Q^2)
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_short_pa_power_get(adi_ad9081_device_t *device,
					  uint8_t dacs, uint16_t *power);

/**
 * @brief  Set Rotation Mode
 *
 * @param  device Pointer to the device structure
 * @param  mode   bit0: enable jesd auto off/on during rotation
 *                bit1: enable data path auto soft off/on during rotation
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_rotation_mode_set(adi_ad9081_device_t *device,
					 uint8_t mode);

/**
 * @brief  Set Main DAC to Channel Xbar
 *         Call after adi_ad9081_device_startup_tx().
 *
 * @param  device     Pointer to the device structure
 * @param  dacs       Target DAC Channel Output
 * @param  channel    Channel to be mapped to target DAC Channel Output
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_xbar_set(adi_ad9081_device_t *device, uint8_t dacs,
				uint8_t channel);

/**
 * @brief  Enalbe DAC Interrupts Request
 *         Call after adi_ad9081_device_startup_tx().
 *
 * @param  device     Pointer to the device structure
 * @param  enable     Enable or disable interrupts
 *                    bit0:  PRBS_I        bit1:  PRBS_Q        bit2: SYSREF         bit3:
 *                    bit4:  CAPTURE_DONE  bit5:  LANE_FIFO     bit6: DATA_READY     bit7:
 *                    bit8:                bit9:                bit10:               bit11: PAERR0
 *                    bit12:               bit13:               bit14:               bit15: PAERR1
 *                    bit16:               bit17:               bit18:               bit19: PAERR2
 *                    bit20:               bit21:               bit22:               bit23: PAERR3
 *                    bit24: PLL_LOCK_FAST bit25: PLL_LOCK_SLOW bit26: PLL_LOST_FAST bit27: PLL_LOST_SLOW
 *                    bit28: DLL_LOCK01    bit29: DLL_LOST01    bit30: DLL_LOCK23    bit31: DLL_LOST23
 *                    bit32: DP0_DLL_VTH_F bit33: DP1_DLL_VTH_F bit34: DP2_DLL_VTH_F bit35: DP3_DLL_VTH_F
 *                    bit36: DP0_DLL_VTH_P bit37: DP1_DLL_VTH_P bit38: DP2_DLL_VTH_P bit39: DP3_DLL_VTH_P
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_irqs_enable_set(adi_ad9081_device_t *device,
				       uint64_t enable);

/**
 * @brief  Get DAC Interrupts Status
 *
 * @param  device     Pointer to the device structure
 * @param  status     Interrupt status
 *                    bit0:  PRBS_I        bit1:  PRBS_Q        bit2: SYSREF         bit3:
 *                    bit4:  CAPTURE_DONE  bit5:  LANE_FIFO     bit6: DATA_READY     bit7:
 *                    bit8:                bit9:                bit10:               bit11: PAERR0
 *                    bit12:               bit13:               bit14:               bit15: PAERR1
 *                    bit16:               bit17:               bit18:               bit19: PAERR2
 *                    bit20:               bit21:               bit22:               bit23: PAERR3
 *                    bit24: PLL_LOCK_FAST bit25: PLL_LOCK_SLOW bit26: PLL_LOST_FAST bit27: PLL_LOST_SLOW
 *                    bit28: DLL_LOCK01    bit29: DLL_LOST01    bit30: DLL_LOCK23    bit31: DLL_LOST23
 *                    bit32: DP0_DLL_VTH_F bit33: DP1_DLL_VTH_F bit34: DP2_DLL_VTH_F bit35: DP3_DLL_VTH_F
 *                    bit36: DP0_DLL_VTH_P bit37: DP1_DLL_VTH_P bit38: DP2_DLL_VTH_P bit39: DP3_DLL_VTH_P
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_irqs_status_get(adi_ad9081_device_t *device,
				       uint64_t *status);

/**
 * @brief  Clear DAC Interrupts Status
 *
 * @param  device     Pointer to the device structure
 * @param  clear      Interrupt status clear bits
 *                    bit0:  PRBS_I        bit1:  PRBS_Q        bit2: SYSREF         bit3:
 *                    bit4:  CAPTURE_DONE  bit5:  LANE_FIFO     bit6: DATA_READY     bit7:
 *                    bit8:                bit9:                bit10:               bit11: PAERR0
 *                    bit12:               bit13:               bit14:               bit15: PAERR1
 *                    bit16:               bit17:               bit18:               bit19: PAERR2
 *                    bit20:               bit21:               bit22:               bit23: PAERR3
 *                    bit24: PLL_LOCK_FAST bit25: PLL_LOCK_SLOW bit26: PLL_LOST_FAST bit27: PLL_LOST_SLOW
 *                    bit28: DLL_LOCK01    bit29: DLL_LOST01    bit30: DLL_LOCK23    bit31: DLL_LOST23
 *                    bit32: DP0_DLL_VTH_F bit33: DP1_DLL_VTH_F bit34: DP2_DLL_VTH_F bit35: DP3_DLL_VTH_F
 *                    bit36: DP0_DLL_VTH_P bit37: DP1_DLL_VTH_P bit38: DP2_DLL_VTH_P bit39: DP3_DLL_VTH_P
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_irqs_status_clr(adi_ad9081_device_t *device,
				       uint64_t clear);

/**
 * @brief  Configure DAC NCO or DAC Channel NCO based on desired frequency
 *
 * @param  device       Pointer to the device structure
 * @param  dacs         Target DAC NCO, set to AD9081_DAC_NONE if configuring DAC Channel NCO
 * @param  channels     Target DAC Channel NCO, set to AD9081_DAC_CH_NONE if configuring DAC NCO
 * @param  shift_hz     Desired NCO frequency.
 * @param  offset       Desired NCO test tone DC offset
 * @param  test_tone_en Enable test tone
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_nco_set(adi_ad9081_device_t *device, uint8_t dacs,
			       uint8_t channels, int64_t shift_hz,
			       uint16_t offset, uint8_t test_tone_en);

/**
 * @brief  Set DAC Data Scrabmling
 *         Call after adi_ad9081_device_startup_tx().
 *
 * @param  device  Pointer to the device structure
 * @param  dacs    Target DAC Channel to enable data output
 * @param  enable  Enable dac data scrambling in sync and retimer
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_data_xor_set(adi_ad9081_device_t *device, uint8_t dacs,
				    uint8_t enable);

/**
 * @brief  Set TXEN State Machine Enable
 *
 * @param  device  Pointer to the device structure
 * @param  dacs    Target DAC Channel to enable data output
 * @param  enable  1 to enable txen state machine, 0 to disable
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t
adi_ad9081_dac_tx_enable_state_machine_enable_set(adi_ad9081_device_t *device,
						  uint8_t dacs, uint8_t enable);

/**
 * @brief  Enable SPI as TXEN Control
 *
 * @param  device  Pointer to the device structure
 * @param  dacs    Target DAC Channel to enable data output
 * @param  enable  1 to enable spi to control tx enable, 0 to disable
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_spi_as_tx_en_set(adi_ad9081_device_t *device,
					uint8_t dacs, uint8_t enable);

/**
 * @brief  Set Enable on DAC outputs
 *         Call after adi_ad9081_device_startup_tx().
 *
 * @param  device  Pointer to the device structure
 * @param  dacs    Target DAC Channel to enable data output
 * @param  enable  1 to enable dac data, 0 to disable
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_tx_enable_set(adi_ad9081_device_t *device, uint8_t dacs,
				     uint8_t enable);

/**
 * @brief  Set Enable on DAC DSA
 *
 * @param  device  Pointer to the device structure
 * @param  dacs    Target DAC Channel to enable data output
 * @param  enable  1 to enable dac dsa, 0 to disable
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_duc_main_dsa_enable_set(adi_ad9081_device_t *device,
					       uint8_t dacs, uint8_t enable);

/**
 * @brief  Configure DAC DSA
 *
 * @param  device  Pointer to the device structure
 * @param  dacs    Target DAC Channel to enable data output
 * @param  code    Attenuation code, 0 equals no attention and 235 equals 47dB attenuation.
 * @param  cutover Governs the switch over from analog to digital gain control
 * @param  boost   Boost ability to elevate the full_scale current above the 26mA baseline current
 * @param  gain    12 bit data path digital gain
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_duc_main_dsa_set(adi_ad9081_device_t *device,
					uint8_t dacs, uint8_t code,
					uint8_t cutover, uint8_t boost,
					uint16_t gain);

/**
 * @brief  Set full scale current of DAC outputs
 *         Call after adi_ad9081_device_startup_tx().
 *
 * @param  device  Pointer to the device structure
 * @param  dacs    Target DAC Channel to enable data output
 * @param  uA      Desired current value in uA
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_fsc_set(adi_ad9081_device_t *device, uint8_t dacs,
			       uint32_t uA);

/**
 * @brief  Set SYSREF Phase
 *
 * @param  device  Pointer to the device structure
 * @param  phase   Phase of measured SYSREF Event in dac clocks
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_sysref_phase_set(adi_ad9081_device_t *device,
					uint16_t phase);

/**
 * @brief  Set SYSREF Sample Type
 *
 * @param  device       Pointer to the device structure
 * @param  sample_type  0 - sampled by reference clock then by high speed clock, 1 - sampled directly by high speed clock
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_sysref_sample_type_set(adi_ad9081_device_t *device,
					      uint8_t sample_type);

/**
 * @brief  Reset Main and Channel NCO
 *
 * @param  device  Pointer to the device structure
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_nco_reset_set(adi_ad9081_device_t *device);

/**
 * @brief  Use SYSREF to Reset NCO
 *
 * @param  device  Pointer to the device structure
 * @param  enable  1 to use SYSREF signal ot reset NCO
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t
adi_ad9081_dac_nco_sync_reset_via_sysref_set(adi_ad9081_device_t *device,
					     uint8_t enable);

/**
 * @brief  Set How to Sync NCO by SYSREF
 *
 * @param  device  Pointer to the device structure
 * @param  mode    0: immediately by sysref, 1: by next lmfc rising edge, 2: by next lmfc falling edge
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_nco_sync_sysref_mode_set(adi_ad9081_device_t *device,
						uint8_t mode);

/**
 * @brief  Align NCOs (configure NCO FTW and Phase offset first)
 *
 * @param  device        Pointer to the device structure
 * @param  align_source  0: oneshot, 1: spi, 2: sysref, 3: lmfc rising edge, 4: lmfc falling edge
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_nco_sync_set(adi_ad9081_device_t *device,
				    uint8_t align_source);

/**
 * @brief  Set Device As Master Or Slave to Sync NCO
 *
 * @param  device  Pointer to the device structure
 * @param  mode    0: disable, 1: master, 2: slave
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_nco_master_slave_mode_set(adi_ad9081_device_t *device,
						 uint8_t mode);

/**
 * @brief  Select GPIO as NCO LFMC Output or Input
 *
 * @param  device     Pointer to the device structure
 * @param  gpio_index 0~5 to select GPIO0 ~ GPIO5
 * @param  output     1 for master output, 0 for slave input
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_nco_master_slave_gpio_set(adi_ad9081_device_t *device,
						 uint8_t gpio_index,
						 uint8_t output);

/**
 * @brief  Set Trigger Source of Master Slave Mode
 *
 * @param  device  Pointer to the device structure
 * @param  source  0: sysref, 1: lmfc rising edge, 2: lmfc falling edge
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t
adi_ad9081_dac_nco_master_slave_trigger_source_set(adi_ad9081_device_t *device,
						   uint8_t source);

/**
 * @brief  Set Extra LMFC Number in NCO Master-Slave Syc Mode
 *
 * @param  device  Pointer to the device structure
 * @param  num     Extra lmfc number in nco master-slave sync mode
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t
adi_ad9081_dac_nco_master_slave_extra_lmfc_num_set(adi_ad9081_device_t *device,
						   uint8_t num);

/**
 * @brief  Trigger Master-Slave NCO Sync
 *
 * @param  device  Pointer to the device structure
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t
adi_ad9081_dac_nco_master_slave_trigger_set(adi_ad9081_device_t *device);

/**
 * @brief  High level API for Master-Slave NCO Sync
 *
 * @param  device         Pointer to the device structure
 * @param  is_master      1 for master, 0 for slave
 * @param  trigger_src    0: sysref, 1: lmfc rising edge, 2: lmfc falling edge
 * @param  gpio_index     0~5 to select GPIO0 ~ GPIO5
 * @param  extra_lmfc_num Extra lmfc number in nco master-slave sync mode
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_nco_master_slave_sync(adi_ad9081_device_t *device,
					     uint8_t is_master,
					     uint8_t trigger_src,
					     uint8_t gpio_index,
					     uint8_t extra_lmfc_num);

/**
 * @brief  Enable DAC Core SPI Reg Access
 *         Called by adi_ad9081_device_clk_config_set().
 *
 * @param  device  Pointer to the device structure
 * @param  enable  Desired enable status
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_spi_enable_set(adi_ad9081_device_t *device,
				      uint8_t enable);

/**
 * @brief  Reset rx digital datapath
 *
 * @param  device   Pointer to the device structure
 * @param  reset    1 to reset, 0 to release
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_digital_datapath_reset_set(adi_ad9081_device_t *device,
						  uint8_t reset);

/**
 * @brief  Read back the current ADC clock frequency
 *
 * @param  device     Pointer to the device structure
 * @param  adc_clk_hz Pointer to variable to store the current clk frequency in Hz
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_clk_freq_get(adi_ad9081_device_t *device,
				    uint64_t *adc_clk_hz);

/**
 * @brief  Configure adc to cddc cross-bar
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param  device           Pointer to the device structure
 * @param  adc_cddc_xbar    Configure ADC to Coarse DDC Crossbar, @see adi_ad9081_adc_adc_to_cddc_xbar_e
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_adc2cddc_xbar_set(adi_ad9081_device_t *device,
					 uint8_t adc_cddc_xbar);

/**
 * @brief  Configure the rx data path cross-bar
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param  device           Pointer to the device structure
 * @param  cddc_fddc_xbar   Coarse to Fine DDC Crossbar, @see adi_ad9081_adc_cddc_to_fddc0_xbar_e, ...
 *                              Bit[0] - 0/1 connects CDDC0/1 to FDDC0
 *                              Bit[1] - 0/1 connects CDDC0/1 to FDDC1
 *                              Bit[2] - 0/1 connects CDDC0/1 to FDDC2
 *                              Bit[3] - 0/1 connects CDDC0/1 to FDDC3
 *                              Bit[4] - 0/1 connects CDDC2/3 to FDDC4
 *                              Bit[5] - 0/1 connects CDDC2/3 to FDDC5
 *                              Bit[6] - 0/1 connects CDDC2/3 to FDDC6
 *                              Bit[7] - 0/1 connects CDDC2/3 to FDDC7
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_cddc2fddc_xbar_set(adi_ad9081_device_t *device,
					  uint8_t cddc_fddc_xbar);

/**
 * @brief  Configure the rx data path cross-bar
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param  device           Pointer to the device structure
 * @param  adc_cddc_xbar    ADC to Coarse DDC Crossbar, @see adi_ad9081_adc_adc_to_cddc_xbar_e
 * @param  cddc_fddc_xbar   Coarse to Fine DDC Crossbar, @see adi_ad9081_adc_cddc_to_fddc0_xbar_e, ...
 *                              Bit[0] - 0/1 connects CDDC0/1 to FDDC0
 *                              Bit[1] - 0/1 connects CDDC0/1 to FDDC1
 *                              Bit[2] - 0/1 connects CDDC0/1 to FDDC2
 *                              Bit[3] - 0/1 connects CDDC0/1 to FDDC3
 *                              Bit[4] - 0/1 connects CDDC2/3 to FDDC4
 *                              Bit[5] - 0/1 connects CDDC2/3 to FDDC5
 *                              Bit[6] - 0/1 connects CDDC2/3 to FDDC6
 *                              Bit[7] - 0/1 connects CDDC2/3 to FDDC7
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_xbar_set(adi_ad9081_device_t *device,
				uint8_t adc_cddc_xbar, uint8_t cddc_fddc_xbar);

/**
 * @brief  Get the rx data path cross-bar
 *
 * @param  device           Pointer to the device structure
 * @param  adc_cddc_xbar    ADC to coarse DDC Crossbar
 * @param  cddc_fddc_xbar   Coarse to fine DDC Crossbar
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_xbar_get(adi_ad9081_device_t *device,
				uint8_t *adc_cddc_xbar,
				uint8_t *cddc_fddc_xbar);

/**
 * @brief  Find Coarse DDC For Given Fine DDC Based on Xbar Settings
 *
 * @param  device           Pointer to the device structure
 * @param  fddc             Fine DDC, @see adi_ad9081_adc_fine_ddc_select_e
 * @param  cddc             Coarse DDC pointer, @see adi_ad9081_adc_coarse_ddc_select_e
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_xbar_find_cddc(adi_ad9081_device_t *device, uint8_t fddc,
				      uint8_t *cddc);

/**
 * @brief  Configure the chip decimation ratio
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param  device   Pointer to the device structure
 * @param  links    JESD links selection, @see adi_ad9081_jesd_link_select_e
 * @param  dcm      Value of total decimation
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_chip_dcm_ratio_set(adi_ad9081_device_t *device,
					  adi_ad9081_jesd_link_select_e links,
					  uint8_t dcm);

/**
 * @brief  Get the chip decimation ratio
 *
 * @param  device   Pointer to the device structure
 * @param  links    JESD links selection, @see adi_ad9081_jesd_link_select_e
 * @param  dcm      Pointer to value of total decimation
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_chip_dcm_ratio_get(adi_ad9081_device_t *device,
					  adi_ad9081_jesd_link_select_e links,
					  uint8_t *dcm);

/**
 * @brief  Page Targeted Coarse DDCs
 *
 * @param  device   Pointer to the device structure
 * @param  cddcs    Coarse DDCs selection, @see adi_ad9081_adc_coarse_ddc_select_e
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_ddc_coarse_select_set(adi_ad9081_device_t *device,
					     uint8_t cddcs);

/**
 * @brief  Reset Coarse DDCs
 *
 * @param  device   Pointer to the device structure
 * @param  cddcs    Coarse DDCs selection, @see adi_ad9081_adc_coarse_ddc_select_e
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_ddc_coarse_reset_set(adi_ad9081_device_t *device,
					    uint8_t cddcs);

/**
 * @brief  Enable/Disable Coarse DDCs Syncronization
 *
 * @param  device   Pointer to the device structure
 * @param  cddcs    Coarse DDCs selection, @see adi_ad9081_adc_coarse_ddc_select_e
 * @param  enable   0 to disable, 1 to enable
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_ddc_coarse_sync_enable_set(adi_ad9081_device_t *device,
						  uint8_t cddcs,
						  uint8_t enable);

/**
 * @brief  Configure Coarse DDC Decimation
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param  device   Pointer to the device structure
 * @param  cddcs    Coarse DDCs selection, @see adi_ad9081_adc_coarse_ddc_select_e
 * @param  dcm      Value of desired decimation for the coarse DDCs
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_ddc_coarse_dcm_set(adi_ad9081_device_t *device,
					  uint8_t cddcs,
					  adi_ad9081_adc_coarse_ddc_dcm_e dcm);

/**
 * @brief  Configure Coarse DDC C2R (complex to real) Enable
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param  device   Pointer to the device structure
 * @param  cddcs    Coarse DDCs selection, @see adi_ad9081_adc_coarse_ddc_select_e
 * @param  c2r_en   Complex to real enable
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_ddc_coarse_c2r_set(adi_ad9081_device_t *device,
					  uint8_t cddcs, uint8_t c2r_en);

/**
 * @brief  Configure Coarse DDC Gain Value
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param  device   Pointer to the device structure
 * @param  cddcs    Coarse DDCs selection, @see adi_ad9081_adc_coarse_ddc_select_e
 * @param  gain     Value of desired gain. 0: no gain. 1: 6dB gain
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_ddc_coarse_gain_set(adi_ad9081_device_t *device,
					   uint8_t cddcs, uint8_t gain);

/**
 * @brief  Configure NCO frequency for the coarse DDCs
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param  device        Pointer to the device structure
 * @param  cddcs         Coarse DDCs selection, @see adi_ad9081_adc_coarse_ddc_select_e
 * @param  cddc_shift_hz Value of frequency shift in Hz
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_ddc_coarse_nco_set(adi_ad9081_device_t *device,
					  uint8_t cddcs, int64_t cddc_shift_hz);

/**
 * @brief  Configure NCO frequency and modulus for the coarse DDCs
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param  device     Pointer to the device structure
 * @param  cddcs      Coarse DDCs selection, @see adi_ad9081_adc_coarse_ddc_select_e
 * @param  ftw        Value of frequency tuning word
 * @param  modulus_a  Value of modulus A word
 * @param  modulus_b  Value of modulus B word
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_ddc_coarse_nco_ftw_set(adi_ad9081_device_t *device,
					      uint8_t cddcs, uint64_t ftw,
					      uint64_t modulus_a,
					      uint64_t modulus_b);

/**
 * @brief  Get NCO frequency and modulus for the coarse DDCs
 *
 * @param  device     Pointer to the device structure
 * @param  cddc       Coarse DDCs selection, @see adi_ad9081_adc_coarse_ddc_select_e
 * @param  ftw        Pointer of frequency tuning word
 * @param  modulus_a  Pointer of modulus A word
 * @param  modulus_b  Pointer of modulus B word
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_ddc_coarse_nco_ftw_get(adi_ad9081_device_t *device,
					      uint8_t cddc, uint64_t *ftw,
					      uint64_t *modulus_a,
					      uint64_t *modulus_b);

/**
 * @brief  Set NCO Phase Offset
 *
 * @param  device     Pointer to the device structure
 * @param  cddcs      Coarse DDCs selection, @see adi_ad9081_adc_coarse_ddc_select_e
 * @param  offset     Phase offset
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t
adi_ad9081_adc_ddc_coarse_nco_phase_offset_set(adi_ad9081_device_t *device,
					       uint8_t cddcs, uint64_t offset);

/**
 * @brief  Configure NCO mode for the coarse DDCs
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param  device     Pointer to the device structure
 * @param  cddcs      Coarse DDCs selection, @see adi_ad9081_adc_coarse_ddc_select_e
 * @param  nco_mode   NCO mode selection, @see adi_ad9081_adc_nco_mode_e
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t
adi_ad9081_adc_ddc_coarse_nco_mode_set(adi_ad9081_device_t *device,
				       uint8_t cddcs,
				       adi_ad9081_adc_nco_mode_e nco_mode);

/**
 * @brief  Enable NCO for the coarse DDCs
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param  device     Pointer to the device structure
 * @param  cddcs      Coarse DDCs selection, @see adi_ad9081_adc_coarse_ddc_select_e
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_ddc_coarse_nco_enable_set(adi_ad9081_device_t *device,
						 uint8_t cddcs);

/**
 * @brief  Select Coarse NCO Channel
 *
 * @param  device     Pointer to the device structure
 * @param  cddcs      Coarse DDCs selection, @see adi_ad9081_adc_coarse_ddc_select_e
 * @param  channel    Coarse NCO channel index, 0 ~ 15
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_ddc_coarse_nco_channel_selection_set(
	adi_ad9081_device_t *device, uint8_t cddcs, uint8_t channel);

/**
 * @brief  Set Coarse NCO Channel Selection Mode
 *
 * @param  device     Pointer to the device structure
 * @param  cddcs      Coarse DDCs selection, @see adi_ad9081_adc_coarse_ddc_select_e
 * @param  mode       Coarse NCO channel selection mode
 *                        0: Register Map control
 *                        1: profile_pins[0]    is used. Pin level control {3'b0, profile_pins[0]}
 *                        2: profile_pins[1:0] are used. Pin level control {2'b0, profile_pins[1:0]}
 *                        3: profile_pins[2:0] are used. Pin level control {1'b0, profile_pins[2:0]}
 *                        4: profile_pins[3:0] are used. Pin level control {profile_pins[3:0]}
 *                        8: profile_pins[0] increment internal counter at rising edge of profile_pins[0] pin
 *                        9: profile_pins[1] increment internal counter at rising edge of profile_pins[1] pin
 *                       10: profile_pins[2] increment internal counter at rising edge of profile_pins[2] pin
 *                       11: profile_pins[3] increment internal counter at rising edge of profile_pins[3] pin
 *                       12: FHT expire based control - increment internal counter when FHT is expired
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_ddc_coarse_nco_channel_select_via_gpio_set(
	adi_ad9081_device_t *device, uint8_t cddcs, uint8_t mode);

/**
 * @brief  Set Coarse NCO Channel For Update
 *
 * @param  device     Pointer to the device structure
 * @param  cddcs      Coarse DDCs selection, @see adi_ad9081_adc_coarse_ddc_select_e
 * @param  channel    Coarse NCO channel index for update, 0 ~ 15
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_ddc_coarse_nco_channel_update_index_set(
	adi_ad9081_device_t *device, uint8_t cddcs, uint8_t channel);

/**
 * @brief  Page the targeted fine DDCs
 *
 * @param  device    Pointer to the device structure
 * @param  fddcs     Fine DDCs selection, @see adi_ad9081_adc_fine_ddc_select_e
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_ddc_fine_select_set(adi_ad9081_device_t *device,
					   uint8_t fddcs);

/**
 * @brief  Reset Fine DDCs
 *
 * @param  device   Pointer to the device structure
 * @param  fddcs    Fine DDCs selection, @see adi_ad9081_adc_fine_ddc_select_e
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_ddc_fine_reset_set(adi_ad9081_device_t *device,
					  uint8_t fddcs);

/**
 * @brief  Enable/Disable Fine DDCs Syncronization
 *
 * @param  device   Pointer to the device structure
 * @param  fddcs    Fine DDCs selection, @see adi_ad9081_adc_fine_ddc_select_e
 * @param  enable   0 to disable, 1 to enable
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_ddc_fine_sync_enable_set(adi_ad9081_device_t *device,
						uint8_t fddcs, uint8_t enable);

/**
 * @brief  Configure the fine DDC decimation setting
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param  device   Pointer to the device structure
 * @param  fddcs    Fine DDCs selection, @see adi_ad9081_adc_fine_ddc_select_e
 * @param  dcm      Value of desired decimation for the fine DDCs
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_ddc_fine_dcm_set(adi_ad9081_device_t *device,
					uint8_t fddcs,
					adi_ad9081_adc_fine_ddc_dcm_e dcm);

/**
 * @brief  Configure the fine DDC complex to real
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param  device   Pointer to the device structure
 * @param  fddcs    Fine DDCs selection, @see adi_ad9081_adc_fine_ddc_select_e
 * @param  c2r_en   Complex to real enable
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_ddc_fine_c2r_set(adi_ad9081_device_t *device,
					uint8_t fddcs, uint8_t c2r_en);

/**
 * @brief  Configure NCO frequency and modulus for the fine DDCs
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param  device     Pointer to the device structure
 * @param  fddcs      Fine DDCs selection, @see adi_ad9081_adc_fine_ddc_select_e
 * @param  dcm        Value of overall decimation (=fddc_dcm * cddc_dcm, or =fddc_dcm * cddc_dcm / 2 if C2R enabled)
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_ddc_fine_overall_dcm_set(adi_ad9081_device_t *device,
						uint8_t fddcs, uint8_t dcm);

/**
 * @brief  Configure the fine DDC gain block
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param  device   Pointer to the device structure
 * @param  fddcs    Fine DDCs selection, @see adi_ad9081_adc_fine_ddc_select_e
 * @param  gain     Value of desired gain
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_ddc_fine_gain_set(adi_ad9081_device_t *device,
					 uint8_t fddcs, uint8_t gain);

/**
 * @brief  Configure NCO frequency for fine DDCs
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param  device        Pointer to the device structure
 * @param  fddcs         Fine DDCs selection, @see adi_ad9081_adc_fine_ddc_select_e
 * @param  fddc_shift_hz Value of frequency shift in Hz
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_ddc_fine_nco_set(adi_ad9081_device_t *device,
					uint8_t fddcs, int64_t fddc_shift_hz);

/**
 * @brief  Configure NCO frequency and modulus for fine DDCs
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param  device     Pointer to the device structure
 * @param  fddcs      Fine DDCs selection, @see adi_ad9081_adc_fine_ddc_select_e
 * @param  ftw        Value of frequency tuning word
 * @param  modulus_a  Value of modulus A word
 * @param  modulus_b  Value of modulus B word
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_ddc_fine_nco_ftw_set(adi_ad9081_device_t *device,
					    uint8_t fddcs, uint64_t ftw,
					    uint64_t modulus_a,
					    uint64_t modulus_b);

/**
 * @brief  Get NCO frequency and modulus for the fine DDC
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param  device     Pointer to the device structure
 * @param  fddc       Fine DDCs selection, @see adi_ad9081_adc_fine_ddc_select_e
 * @param  ftw        Pointer of frequency tuning word
 * @param  modulus_a  Pointer of modulus A word
 * @param  modulus_b  Pointer of modulus B word
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_ddc_fine_nco_ftw_get(adi_ad9081_device_t *device,
					    uint8_t fddc, uint64_t *ftw,
					    uint64_t *modulus_a,
					    uint64_t *modulus_b);

/**
 * @brief  Get NCO frequency and modulus for the fine DDC
 *
 * @param  device     Pointer to the device structure
 * @param  fddcs      Fine DDCs selection, @see adi_ad9081_adc_fine_ddc_select_e
 * @param  offset     Phase offset
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t
adi_ad9081_adc_ddc_fine_nco_phase_offset_set(adi_ad9081_device_t *device,
					     uint8_t fddcs, uint64_t offset);

/**
 * @brief  Configure NCO mode for the fine DDCs
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param  device     Pointer to the device structure
 * @param  fddcs      Fine DDCs selection, @see adi_ad9081_adc_fine_ddc_select_e
 * @param  nco_mode   NCO mode selection
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t
adi_ad9081_adc_ddc_fine_nco_mode_set(adi_ad9081_device_t *device, uint8_t fddcs,
				     adi_ad9081_adc_nco_mode_e nco_mode);

/**
 * @brief  Enable NCO for the fine DDCs
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param  device     Pointer to the device structure
 * @param  fddcs      Fine DDCs selection, @see adi_ad9081_adc_fine_ddc_select_e
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_ddc_fine_nco_enable_set(adi_ad9081_device_t *device,
					       uint8_t fddcs);

/**
 * @brief  Select Fine DDC's NCO Channel
 *
 * @param  device     Pointer to the device structure
 * @param  fddcs      Fine DDCs selection, @see adi_ad9081_adc_fine_ddc_select_e
 * @param  channel    Channel index, 0 ~ 15
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_ddc_fine_nco_channel_selection_set(
	adi_ad9081_device_t *device, uint8_t fddcs, uint8_t channel);

/**
 * @brief  Set Fine DDC's NCO Channel Selection Mode
 *
 * @param  device     Pointer to the device structure
 * @param  fddcs      Fine DDCs selection, @see adi_ad9081_adc_fine_ddc_select_e
 * @param  mode       Channel selection mode
 *                        0: Register Map control
 *                        1: profile_pins[0]    is used. Pin level control {3'b0, profile_pins[0]}
 *                        2: profile_pins[1:0] are used. Pin level control {2'b0, profile_pins[1:0]}
 *                        3: profile_pins[2:0] are used. Pin level control {1'b0, profile_pins[2:0]}
 *                        4: profile_pins[3:0] are used. Pin level control {profile_pins[3:0]}
 *                        8: profile_pins[0] increment internal counter at rising edge of profile_pins[0] pin
 *                        9: profile_pins[1] increment internal counter at rising edge of profile_pins[1] pin
 *                       10: profile_pins[2] increment internal counter at rising edge of profile_pins[2] pin
 *                       11: profile_pins[3] increment internal counter at rising edge of profile_pins[3] pin
 *                       12: FHT expire based control - increment internal counter when FHT is expired
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_ddc_fine_nco_channel_select_via_gpio_set(
	adi_ad9081_device_t *device, uint8_t fddcs, uint8_t mode);

/**
 * @brief  Select Fine DDC's NCO Channel For Update
 *
 * @param  device     Pointer to the device structure
 * @param  fddcs      Fine DDCs selection, @see adi_ad9081_adc_fine_ddc_select_e
 * @param  channel    Channel index, 0 ~ 15
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_ddc_fine_nco_channel_update_index_set(
	adi_ad9081_device_t *device, uint8_t fddcs, uint8_t channel);

/**
 * @brief  Configure Basic Rx Path
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param  device       Pointer to the device structure
 * @param  cddcs        Coarse DDCs selection, @see adi_ad9081_adc_coarse_ddc_select_e
 * @param  fddcs        Fine   DDCs selection, @see adi_ad9081_adc_fine_ddc_select_e
 * @param  cddc_shift   Coarse DDC NCO shift
 * @param  fddc_shift   Fine   DDC NCO shift
 * @param  cddc_dcm     Coarse DDC decimation value
 * @param  fddc_dcm     Fine   DDC decimation value
 * @param  cc2r_en      Coarse DDC complex to real enable
 * @param  fc2r_en      Fine   DDC complex to real enable
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_config(adi_ad9081_device_t *device, uint8_t cddcs,
			      uint8_t fddcs, int64_t cddc_shift[4],
			      int64_t fddc_shift[8], uint8_t cddc_dcm[4],
			      uint8_t fddc_dcm[8], uint8_t cc2r_en[4],
			      uint8_t fc2r_en[8]);

/**
 * @brief  Enable ADC Clock Out Driver
 *
 * @param  device     Pointer to the device structure
 * @param  enable     1:Enable, 0:Disable
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_clk_out_enable_set(adi_ad9081_device_t *device,
					  uint8_t enable);

/**
 * @brief  Enable ADC Clock Divider
 *
 * @param  device     Pointer to the device structure
 * @param  enable     1:Enable, 0:Disable
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_clk_enable_set(adi_ad9081_device_t *device,
				      uint8_t enable);

/**
 * @brief  configure adc clock divider and enable adc clk
 *
 * @param  device     Pointer to the device structure
 * @param  div        Divider setting
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_clk_div_set(adi_ad9081_device_t *device, uint8_t div);

/**
 * @brief  Configure mapping between NCO FFH selection to GPIO
 *
 * @param  device     Pointer to the device structure
 * @param  ffh        ffh0 ~ ffh5, 2 - gpio6, 3 - gpio7, 4 - gpio8, 5 - gpio9, 6 - gpio10, 7 - syncinb1_p, 8 - syncinb1_n
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t
adi_ad9081_adc_ddc_ffh_sel_to_gpio_mapping_set(adi_ad9081_device_t *device,
					       uint8_t ffh[6]);

/**
 * @brief  Configure mapping between fast detection enable to GPIO
 *
 * @param  device     Pointer to the device structure
 * @param  fd_en_gpio 2 - gpio6, 3 - gpio7, 4 - gpio8, 5 - gpio9, 6 - gpio10, 7 - syncinb1_p, 8 - syncinb1_n
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_fd_en_to_gpio_mapping_set(adi_ad9081_device_t *device,
						 uint8_t fd_en_gpio);

/**
 * @brief  Configure mapping between rx enable to GPIO
 *
 * @param  device     Pointer to the device structure
 * @param  rxen1_gpio 2 - gpio6, 3 - gpio7, 4 - gpio8, 5 - gpio9, 6 - gpio10, 7 - syncinb1_p, 8 - syncinb1_n
 * @param  rxen3_gpio 2 - gpio6, 3 - gpio7, 4 - gpio8, 5 - gpio9, 6 - gpio10, 7 - syncinb1_p, 8 - syncinb1_n
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_rx_en_to_gpio_mapping_set(adi_ad9081_device_t *device,
						 uint8_t rxen1_gpio,
						 uint8_t rxen3_gpio);

/**
 * @brief  Configure mapping between signal monitor enable to GPIO
 *
 * @param  device       Pointer to the device structure
 * @param  smon_en_gpio 0 - disable, 1 - enable, 2 - gpio6 to enable, 3 - gpio7, 4 - gpio8, 5 - gpio9, 6 - gpio10, 7 - syncinb1_p, 8 - syncinb1_n
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_smon_en_to_gpio_mapping_set(adi_ad9081_device_t *device,
						   uint8_t smon_en_gpio);

/**
 * @brief  Configure rxen1 pin
 *
 * @param  device     Pointer to the device structure
 * @param  spi_en     enable pin
 * @param  pol        polarity
 * @param  rxen       value
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_rxen1_gpio_ctrl_set(adi_ad9081_device_t *device,
					   uint8_t spi_en, uint8_t pol,
					   uint8_t rxen);

/**
 * @brief  Configure rxen3 pin
 *
 * @param  device     Pointer to the device structure
 * @param  spi_en     enable pin
 * @param  pol        polarity
 * @param  rxen       value
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_rxen3_gpio_ctrl_set(adi_ad9081_device_t *device,
					   uint8_t spi_en, uint8_t pol,
					   uint8_t rxen);

/**
 * @brief  Configure blockes to gate clock
 *
 * @param  device     Pointer to the device structure
 * @param  cddcs      Coarse DDC (bit0: cddc0, ..., bit3: cddc3)
 * @param  fddcs      Fine DDC (bit0: fddc0, ..., bit7: fddc7)
 * @param  adcs       ADC core (bit0: core0, bit1: core1)
 * @param  jtx        Link (bit0: link0, bit1: link1)
 * @param  jtx_phy    Lanes (bit0: lane0, bit1: lane1, ..., bit7: lane7)
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_cddc_fddc_jtx_clk_en_via_rxen1_gpio_set(
	adi_ad9081_device_t *device, uint8_t cddcs, uint8_t fddcs, uint8_t adcs,
	uint8_t jtx, uint8_t jtx_phy);

/**
 * @brief  Configure blockes to gate clock
 *
 * @param  device     Pointer to the device structure
 * @param  cddcs      Coarse DDC (bit0: cddc0, ..., bit3: cddc3)
 * @param  fddcs      Fine DDC (bit0: fddc0, ..., bit7: fddc7)
 * @param  adcs       ADC core (bit0: core0, bit1: core1)
 * @param  jtx        Link (bit0: link0, bit1: link1)
 * @param  jtx_phy    Lanes (bit0: lane0, bit1: lane1, ..., bit7: lane7)
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_cddc_fddc_jtx_clk_en_via_rxen3_gpio_set(
	adi_ad9081_device_t *device, uint8_t cddcs, uint8_t fddcs, uint8_t adcs,
	uint8_t jtx, uint8_t jtx_phy);

/**
 * @brief  Configure rxen0
 *
 * @param  device     Pointer to the device structure
 * @param  use_txen   use txen as rxen
 * @param  spi_en     enable pin
 * @param  rxen0_pol  polarity
 * @param  rxen0      value
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_rxen0_ctrl_set(adi_ad9081_device_t *device,
				      uint8_t use_txen, uint8_t spi_en,
				      uint8_t rxen0_pol, uint8_t rxen0);

/**
 * @brief  Configure rxen1
 *
 * @param  device     Pointer to the device structure
 * @param  use_txen   use txen as rxen
 * @param  spi_en     enable pin
 * @param  rxen1_pol  polarity
 * @param  rxen1      value
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_rxen1_ctrl_set(adi_ad9081_device_t *device,
				      uint8_t use_txen, uint8_t spi_en,
				      uint8_t rxen1_pol, uint8_t rxen1);

/**
 * @brief  Configure blockes to gate clock
 *
 * @param  device     Pointer to the device structure
 * @param  cddcs      Coarse DDC (bit0: cddc0, ..., bit3: cddc3)
 * @param  fddcs      Fine DDC (bit0: fddc0, ..., bit7: fddc7)
 * @param  adcs       ADC core (bit0: core0, bit1: core1)
 * @param  jtx        Link (bit0: link0, bit1: link1)
 * @param  jtx_phy    Lanes (bit0: lane0, bit1: lane1, ..., bit7: lane7)
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_rxen0_sel_set(adi_ad9081_device_t *device, uint8_t cddcs,
				     uint8_t fddcs, uint8_t adcs, uint8_t jtx,
				     uint8_t jtx_phy);

/**
 * @brief  Configure blockes to gate clock
 *
 * @param  device     Pointer to the device structure
 * @param  cddcs      Coarse DDC (bit0: cddc0, ..., bit3: cddc3)
 * @param  fddcs      Fine DDC (bit0: fddc0, ..., bit7: fddc7)
 * @param  adcs       ADC core (bit0: core0, bit1: core1)
 * @param  jtx        Link (bit0: link0, bit1: link1)
 * @param  jtx_phy    Lanes (bit0: lane0, bit1: lane1, ..., bit7: lane7)
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_rxen1_sel_set(adi_ad9081_device_t *device, uint8_t cddcs,
				     uint8_t fddcs, uint8_t adcs, uint8_t jtx,
				     uint8_t jtx_phy);

/**
 * @brief  Set ADC Nyquist Zone
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param  device         Pointer to the device structure
 * @param  zone           AD9081_ADC_NYQUIST_ZONE_ODD / AD9081_ADC_NYQUIST_ZONE_EVEN
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_nyquist_zone_set(adi_ad9081_device_t *device,
					adi_ad9081_adc_nyquist_zone_e zone);

/**
 * @brief  Set ADC User Pattern of Test Mode
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param  device         Pointer to the device structure
 * @param  i_pattern      I user pattern
 * @param  q_pattern      Q user pattern
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_test_mode_usr_pattern_set(adi_ad9081_device_t *device,
						 uint8_t i_pattern[16],
						 uint8_t q_pattern[16]);

/**
 * @brief  Set ADC Test Mode
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param  device         Pointer to the device structure
 * @param  i_mode         I test mode, @see adi_ad9081_test_mode_e
 * @param  q_mode         Q test mode, @see adi_ad9081_test_mode_e
 * @param  links          Target link
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_test_mode_config_set(
	adi_ad9081_device_t *device, adi_ad9081_test_mode_e i_mode,
	adi_ad9081_test_mode_e q_mode, adi_ad9081_jesd_link_select_e links);

/**
 * @brief  Set PFIR Control Page
 *
 * @param  device         Pointer to the device structure
 * @param  page           PFIR control page @see adi_ad9081_adc_pfir_ctl_page_e
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_pfir_ctl_page_set(adi_ad9081_device_t *device,
					 adi_ad9081_adc_pfir_ctl_page_e page);

/**
 * @brief  Set PFIR coefficient page
 *
 * @param  device         Pointer to the device structure
 * @param  page           PFIR coefficient page @see adi_ad9081_adc_pfir_coeff_page_e
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t
adi_ad9081_adc_pfir_coeff_page_set(adi_ad9081_device_t *device,
				   adi_ad9081_adc_pfir_coeff_page_e page);

/**
 * @brief  Set PFIR Data Input Selection
 *
 * @param  device         Pointer to the device structure
 * @param  ctl_pages      PFIR control pages @see adi_ad9081_adc_pfir_ctl_page_e
 * @param  i_sel          I-path data input
 *                            ctrl_pages == AD9081_ADC_PFIR_ADC_PAIR0: 0 - ADC0, 1 - ADC2, 2 - ADC1, 3 - ADC3
 *                            ctrl_pages == AD9081_ADC_PFIR_ADC_PAIR1: 0 - ADC1, 1 - ADC3, 2 - ADC0, 3 - ADC2
 * @param  q_sel          Q-path data input
 *                            ctrl_pages == AD9081_ADC_PFIR_ADC_PAIR0: 0 - ADC2, 1 - ADC1, 2 - ADC3, 3 - ADC0
 *                            ctrl_pages == AD9081_ADC_PFIR_ADC_PAIR1: 0 - ADC3, 1 - ADC0, 2 - ADC2, 3 - ADC1
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t
adi_ad9081_adc_pfir_din_select_set(adi_ad9081_device_t *device,
				   adi_ad9081_adc_pfir_ctl_page_e ctl_pages,
				   uint8_t i_sel, uint8_t q_sel);

/**
 * @brief  Set PFIR Data Output Selection
 *
 * @param  device         Pointer to the device structure
 * @param  adcs           ADC to set, @see adi_ad9081_adc_select_e
 * @param  out_sel        Output data input, 0 - ADC Data, 1 - PFIR I-Path Data, 2 - PFIR Q-Path Data
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_pfir_dout_select_set(adi_ad9081_device_t *device,
					    uint8_t adcs, uint8_t out_sel);

/**
 * @brief  Set PFIR I-mode
 *
 * @param  device         Pointer to the device structure
 * @param  ctl_pages      PFIR control pages @see adi_ad9081_adc_pfir_ctl_page_e
 * @param  i_mode         PFIR i-mode @see adi_ad9081_adc_pfir_i_mode_e
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_pfir_i_mode_set(adi_ad9081_device_t *device,
				       adi_ad9081_adc_pfir_ctl_page_e ctl_pages,
				       adi_ad9081_adc_pfir_i_mode_e i_mode);

/**
 * @brief  Set PFIR Q-mode
 *
 * @param  device         Pointer to the device structure
 * @param  ctl_pages      PFIR control pages @see adi_ad9081_adc_pfir_ctl_page_e
 * @param  q_mode         PFIR q-mode @see adi_ad9081_adc_pfir_q_mode_e
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_pfir_q_mode_set(adi_ad9081_device_t *device,
				       adi_ad9081_adc_pfir_ctl_page_e ctl_pages,
				       adi_ad9081_adc_pfir_q_mode_e q_mode);

/**
 * @brief  Set PFIR Mode
 *
 * @param  device         Pointer to the device structure
 * @param  ctl_pages      PFIR control pages @see adi_ad9081_adc_pfir_ctl_page_e
 * @param  i_mode         PFIR i-mode @see adi_ad9081_adc_pfir_i_mode_e
 * @param  q_mode         PFIR q-mode @see adi_ad9081_adc_pfir_q_mode_e
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_pfir_mode_set(adi_ad9081_device_t *device,
				     adi_ad9081_adc_pfir_ctl_page_e ctl_pages,
				     adi_ad9081_adc_pfir_i_mode_e i_mode,
				     adi_ad9081_adc_pfir_q_mode_e q_mode);

/**
 * @brief  Set PFIR I Gain
 *
 * @param  device         Pointer to the device structure
 * @param  ctl_pages      PFIR control pages @see adi_ad9081_adc_pfir_ctl_page_e
 * @param  ix_gain        PFIR Ix gain @see adi_ad9081_adc_pfir_gain_e
 * @param  iy_gain        PFIR Iy gain @see adi_ad9081_adc_pfir_gain_e
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_pfir_i_gain_set(adi_ad9081_device_t *device,
				       adi_ad9081_adc_pfir_ctl_page_e ctl_pages,
				       adi_ad9081_adc_pfir_gain_e ix_gain,
				       adi_ad9081_adc_pfir_gain_e iy_gain);

/**
 * @brief  Set PFIR Q Gain
 *
 * @param  device         Pointer to the device structure
 * @param  ctl_pages      PFIR control pages @see adi_ad9081_adc_pfir_ctl_page_e
 * @param  qx_gain        PFIR Qx gain @see adi_ad9081_adc_pfir_gain_e
 * @param  qy_gain        PFIR Qy gain @see adi_ad9081_adc_pfir_gain_e
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_pfir_q_gain_set(adi_ad9081_device_t *device,
				       adi_ad9081_adc_pfir_ctl_page_e ctl_pages,
				       adi_ad9081_adc_pfir_gain_e qx_gain,
				       adi_ad9081_adc_pfir_gain_e qy_gain);

/**
 * @brief  Set PFIR I Gain
 *
 * @param  device         Pointer to the device structure
 * @param  ctl_pages      PFIR control pages @see adi_ad9081_adc_pfir_ctl_page_e
 * @param  ix_gain        PFIR Ix gain @see adi_ad9081_adc_pfir_gain_e
 * @param  iy_gain        PFIR Iy gain @see adi_ad9081_adc_pfir_gain_e
 * @param  qx_gain        PFIR Qx gain @see adi_ad9081_adc_pfir_gain_e
 * @param  qy_gain        PFIR Qy gain @see adi_ad9081_adc_pfir_gain_e
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_pfir_gain_set(adi_ad9081_device_t *device,
				     adi_ad9081_adc_pfir_ctl_page_e ctl_pages,
				     adi_ad9081_adc_pfir_gain_e ix_gain,
				     adi_ad9081_adc_pfir_gain_e iy_gain,
				     adi_ad9081_adc_pfir_gain_e qx_gain,
				     adi_ad9081_adc_pfir_gain_e qy_gain);

/**
 * @brief  Set PFIR Delay for Half Complex Mode
 *
 * @param  device         Pointer to the device structure
 * @param  ctl_pages      PFIR control pages @see adi_ad9081_adc_pfir_ctl_page_e
 * @param  delay          Delay value (<= 0xff)
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_pfir_half_complex_delay_set(
	adi_ad9081_device_t *device, adi_ad9081_adc_pfir_ctl_page_e ctl_pages,
	uint8_t delay);

/**
 * @brief  Set PFIR Delay for Half Complex Mode
 *
 * @param  device         Pointer to the device structure
 * @param  ctl_pages      PFIR control pages @see adi_ad9081_adc_pfir_ctl_page_e
 * @param  enable         Enable PFIR coefficients transfer
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t
adi_ad9081_adc_pfir_coeff_xfer_set(adi_ad9081_device_t *device,
				   adi_ad9081_adc_pfir_ctl_page_e ctl_pages,
				   uint8_t enable);

/**
 * @brief  Set PFIR I Programmable Delay Line Setting For Image Cancellation Filter
 *
 * @param  device         Pointer to the device structure
 * @param  coeff_pages    PFIR coefficient pages @see adi_ad9081_adc_pfir_ctl_page_e
 * @param  delay          Delay value (<= 0x7f)
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_pfir_hc_prog_delay_set(
	adi_ad9081_device_t *device,
	adi_ad9081_adc_pfir_coeff_page_e coeff_pages, uint8_t delay);

/**
 * @brief  Set PFIR Quard Mode
 *
 * @param  device         Pointer to the device structure
 * @param  ctl_pages      PFIR control pages @see adi_ad9081_adc_pfir_ctl_page_e
 * @param  enable         Enable quad mode or not
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t
adi_ad9081_adc_pfir_quad_mode_set(adi_ad9081_device_t *device,
				  adi_ad9081_adc_pfir_ctl_page_e ctl_pages,
				  uint8_t enable);

/**
 * @brief  Set PFIR Coefficients Load Selection
 *
 * @param  device         Pointer to the device structure
 * @param  ctl_pages      PFIR control pages @see adi_ad9081_adc_pfir_ctl_page_e
 * @param  sel            Load selection
 *                            bit 0: real_i load,       bit 1: real_q load
 *                            bit 2: real_cross_i load, bit 3: real_cross_q load
 *                            bit 4: complex load
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t
adi_ad9081_adc_pfir_coeff_load_sel_set(adi_ad9081_device_t *device,
				       adi_ad9081_adc_pfir_ctl_page_e ctl_pages,
				       uint8_t sel);

/**
 * @brief  Clear Coefficient Bank
 *
 * @param  device         Pointer to the device structure
 * @param  ctl_pages      PFIR control pages @see adi_ad9081_adc_pfir_ctl_page_e
 * @param  clear          1 to clear selected master coefficient bank
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t
adi_ad9081_adc_pfir_coeff_clear_set(adi_ad9081_device_t *device,
				    adi_ad9081_adc_pfir_ctl_page_e ctl_pages,
				    uint8_t clear);

/**
 * @brief  Set Input Coefficients Encoding Scheme
 *
 * @param  device         Pointer to the device structure
 * @param  ctl_pages      PFIR control pages @see adi_ad9081_adc_pfir_ctl_page_e
 * @param  vle            1 to use VLE encoding scheme
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t
adi_ad9081_adc_pfir_coeff_vle_set(adi_ad9081_device_t *device,
				  adi_ad9081_adc_pfir_ctl_page_e ctl_pages,
				  uint8_t vle);

/**
 * @brief  Set PFIR Coefficient Page
 *
 * @param  device         Pointer to the device structure
 * @param  ctl_pages      PFIR control pages @see adi_ad9081_adc_pfir_ctl_page_e
 * @param  sel            Coefficient page selection
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_pfir_rd_coeff_page_sel_set(
	adi_ad9081_device_t *device, adi_ad9081_adc_pfir_ctl_page_e ctl_pages,
	uint8_t sel);

/**
 * @brief  Validat PFIR Coefficients
 *
 * @param  device         Pointer to the device structure
 * @param  ntaps          PFIR taps number (192, 96, 64, 48)
 * @param  coeffs         Coefficient array
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_pfir_coeff_validate(adi_ad9081_device_t *device,
					   uint8_t ntaps, uint16_t coeffs[192]);

/**
 * @brief  Set PFIR Coefficient
 *
 * @param  device         Pointer to the device structure
 * @param  coeff_pages    PFIR coefficient pages @see adi_ad9081_adc_pfir_ctl_page_e
 * @param  index          Coefficient index (0~191)
 * @param  coeff          Coefficient value
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t
adi_ad9081_adc_pfir_coeff_set(adi_ad9081_device_t *device,
			      adi_ad9081_adc_pfir_coeff_page_e coeff_pages,
			      uint8_t index, uint16_t coeff);

/**
 * @brief  Set PFIR Coefficient
 *
 * @param  device         Pointer to the device structure
 * @param  coeff_pages    PFIR coefficient pages @see adi_ad9081_adc_pfir_ctl_page_e
 * @param  coeffs         Coefficient value array (size >= 192) pointer
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t
adi_ad9081_adc_pfir_coeffs_set(adi_ad9081_device_t *device,
			       adi_ad9081_adc_pfir_coeff_page_e coeff_pages,
			       uint16_t *coeffs);

/**
 * @brief  Set PFIR
 *
 * @param  device         Pointer to the device structure
 * @param  ctl_pages      PFIR control pages @see adi_ad9081_adc_pfir_ctl_page_e
 * @param  coeff_pages    PFIR coefficient pages @see adi_ad9081_adc_pfir_ctl_page_e
 * @param  i_mode         PFIR i-mode @see adi_ad9081_adc_pfir_i_mode_e
 * @param  q_mode         PFIR q-mode @see adi_ad9081_adc_pfir_q_mode_e
 * @param  ix_gain        PFIR Ix gain @see adi_ad9081_adc_pfir_gain_e
 * @param  iy_gain        PFIR Iy gain @see adi_ad9081_adc_pfir_gain_e
 * @param  qx_gain        PFIR Qx gain @see adi_ad9081_adc_pfir_gain_e
 * @param  qy_gain        PFIR Qy gain @see adi_ad9081_adc_pfir_gain_e
 * @param  coeff_load_sel Load selection
 *                            bit 0: real_i load,       bit 1: real_q load
 *                            bit 2: real_cross_i load, bit 3: real_cross_q load
 *                            bit 4: complex load
 * @param  coeffs         Coefficient value array pointer
 * @param  coeffs_size    Coefficient value array(coeffs) size
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_pfir_config_set(
	adi_ad9081_device_t *device, adi_ad9081_adc_pfir_ctl_page_e ctl_pages,
	adi_ad9081_adc_pfir_coeff_page_e coeff_pages,
	adi_ad9081_adc_pfir_i_mode_e i_mode,
	adi_ad9081_adc_pfir_q_mode_e q_mode, adi_ad9081_adc_pfir_gain_e ix_gain,
	adi_ad9081_adc_pfir_gain_e iy_gain, adi_ad9081_adc_pfir_gain_e qx_gain,
	adi_ad9081_adc_pfir_gain_e qy_gain, uint8_t coeff_load_sel,
	uint16_t *coeffs, uint8_t coeffs_size);

/**
 * @brief  Set Fine DDC Samples Status Selection
 *
 * @param  device         Pointer to the device structure
 * @param  fddcs          @see adi_ad9081_adc_fine_ddc_select_e
 * @param  i_status_adc   @see adi_ad9081_adc_select_e
 * @param  q_status_adc   @see adi_ad9081_adc_select_e
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t
adi_ad9081_adc_ddc_fine_status_sel_set(adi_ad9081_device_t *device,
				       adi_ad9081_adc_fine_ddc_select_e fddcs,
				       adi_ad9081_adc_select_e i_status_adc,
				       adi_ad9081_adc_select_e q_status_adc);

/**
 * @brief  Set Fast Detection Threshold
 *
 * @param  device            Pointer to the device structure
 * @param  adcs              ADC selection, @see adi_ad9081_adc_select_e
 * @param  low_thresh        Lower threshold for ADC_FD0/ADC_FD1
 * @param  up_thresh         Up threshold for ADC_FD0/ADC_FD1
 * @param  low_thresh2       Lower threshold for ADC_FD1
 * @param  up_thresh2        Up threshold for ADC_FD1
 * @param  low_dwell_thresh  Low DWELL time for ADC_FD0/ADC_FD1
 * @param  low_dwell_thresh2 Low DWELL time for ADC_FD1
 * @param  up_dwell_thresh   Up DWELL time for ADC_FD1
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_fd_thresh_set(adi_ad9081_device_t *device, uint8_t adcs,
				     uint16_t low_thresh, uint16_t up_thresh,
				     uint16_t low_thresh2, uint16_t up_thresh2,
				     uint16_t low_dwell_thresh,
				     uint16_t low_dwell_thresh2,
				     uint16_t up_dwell_thresh);

/**
 * @brief  Force Value on ADC_FD0
 *
 * @param  device         Pointer to the device structure
 * @param  adcs           ADC selection, @see adi_ad9081_adc_select_e
 * @param  enable         Enable to set force value
 * @param  value          Force value for ADC_FD0
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_fd0_force_set(adi_ad9081_device_t *device, uint8_t adcs,
				     uint8_t enable, uint8_t value);

/**
 * @brief  Force Value on ADC_FD1
 *
 * @param  device         Pointer to the device structure
 * @param  adcs           ADC selection, @see adi_ad9081_adc_select_e
 * @param  enable         Enable to set force value
 * @param  value          Force value for ADC_FD1
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_fd1_force_set(adi_ad9081_device_t *device, uint8_t adcs,
				     uint8_t enable, uint8_t value);

/**
 * @brief  Enable Fast Detect on Corrected ADC Data
 *
 * @param  device         Pointer to the device structure
 * @param  adcs           ADC selection, @see adi_ad9081_adc_select_e
 * @param  enable         1 to Enable Fast Detect
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_fd_en_set(adi_ad9081_device_t *device, uint8_t adcs,
				 uint8_t enable);

/**
 * @brief  Enable Fast Detect on Corrected ADC Data via GPIO
 *
 * @param  device         Pointer to the device structure
 * @param  adcs           ADC selection, @see adi_ad9081_adc_select_e
 * @param  enable         1 to Enable Fast Detect via GPIO
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_fd_en_via_gpio_set(adi_ad9081_device_t *device,
					  uint8_t adcs, uint8_t enable);

/**
 * @brief  Set FD0 Mux
 *
 * @param  device         Pointer to the device structure
 * @param  src_adc        Source ADC selection (FD0) @see adi_ad9081_adc_select_e
 * @param  dst_adc        Output ADC selection (FD0) @see adi_ad9081_adc_select_e
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_fd0_mux_sel_set(adi_ad9081_device_t *device,
				       uint8_t src_adc, uint8_t dst_adc);

/**
 * @brief  Set FD1 Mux
 *
 * @param  device         Pointer to the device structure
 * @param  src_adc        Source ADC selection (FD1) @see adi_ad9081_adc_select_e
 * @param  dst_adc        Output ADC selection (FD1) @see adi_ad9081_adc_select_e
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_fd1_mux_sel_set(adi_ad9081_device_t *device,
				       uint8_t src_adc, uint8_t dst_adc);

/**
 * @brief  Set ADC_FD0 Function Mode
 *
 * @param  device         Pointer to the device structure
 * @param  adcs           ADC selection, @see adi_ad9081_adc_select_e
 * @param  mode           0 to function based on upper and lower threholds, 1 to use only fd_up_thresh
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_fd0_function_mode_set(adi_ad9081_device_t *device,
					     uint8_t adcs, uint8_t mode);

/**
 * @brief  Set ADC_FD1 Function Mode
 *
 * @param  device         Pointer to the device structure
 * @param  adcs           ADC selection, @see adi_ad9081_adc_select_e
 * @param  mode           0 to function based on up_dwell_thresh, 1 to function same as FD0 but based on thresh2 values
 *                        @see adi_ad9081_adc_fd_thresh_set
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_fd1_function_mode_set(adi_ad9081_device_t *device,
					     uint8_t adcs, uint8_t mode);

/**
 * @brief  Select Cycle Delay Value
 *
 * @param  device         Pointer to the device structure
 * @param  adcs           ADC selection, @see adi_ad9081_adc_select_e
 * @param  cd_index       Value index (0~3)
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_cycle_delay_selection_set(adi_ad9081_device_t *device,
						 uint8_t adcs,
						 uint8_t cd_index);

/**
 * @brief  Set Cycle Delay Enable
 *
 * @param  device         Pointer to the device structure
 * @param  adcs           ADC selection, @see adi_ad9081_adc_select_e
 * @param  enable         0: pfilt out mux data at cdelay mux output, 1: cdelay output data at cdelay mux output
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_cycle_delay_enable_set(adi_ad9081_device_t *device,
					      uint8_t adcs, uint8_t enable);

/**
 * @brief  Set Cycle Delay Enable via GPIO
 *
 * @param  device         Pointer to the device structure
 * @param  adcs           ADC selection, @see adi_ad9081_adc_select_e
 * @param  enable         1 to enable cycle delay via gpio
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t
adi_ad9081_adc_cycle_delay_enable_via_gpio_set(adi_ad9081_device_t *device,
					       uint8_t adcs, uint8_t enable);

/**
 * @brief  Set Cycle Delay Values
 *
 * @param  device         Pointer to the device structure
 * @param  adcs           ADC selection, @see adi_ad9081_adc_select_e
 * @param  delay          Delay values
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_cycle_delay_set(adi_ad9081_device_t *device,
				       uint8_t adcs, uint8_t delay[4]);

/**
 * @brief  Set Fractional Delay Status Select
 *
 * @param  device         Pointer to the device structure
 * @param  i_status_sel   Fractional delay i status select, @see adi_ad9081_adc_select_e
 * @param  q_status_sel   Fractional delay q status select, @see adi_ad9081_adc_select_e
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_fractional_delay_status_selection_set(
	adi_ad9081_device_t *device, adi_ad9081_adc_select_e i_status_sel,
	adi_ad9081_adc_select_e q_status_sel);

/**
 * @brief  Set Fractional Delay Value Index
 *
 * @param  device         Pointer to the device structure
 * @param  fd_index       Fractional delay value index (0~3)
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t
adi_ad9081_adc_fractional_delay_selection_set(adi_ad9081_device_t *device,
					      uint8_t fd_index);

/**
 * @brief  Set Fractional Delay Down Sample Enable
 *
 * @param  device         Pointer to the device structure
 * @param  enable         1 to enable down sample
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_fractional_delay_down_sample_enable_set(
	adi_ad9081_device_t *device, uint8_t enable);

/**
 * @brief  Set Fractional Delay Enable
 *
 * @param  device         Pointer to the device structure
 * @param  enable         1 to enable fractional delay
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_fractional_delay_enable_set(adi_ad9081_device_t *device,
						   uint8_t enable);

/**
 * @brief  Set Fractional Delay Enable Via GPIO
 *
 * @param  device         Pointer to the device structure
 * @param  enable         1 to enable fractional delay via gpio
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t
adi_ad9081_adc_fractional_delay_enable_via_gpio_set(adi_ad9081_device_t *device,
						    uint8_t enable);

/**
 * @brief  Set Fractional Delay Value
 *
 * @param  device         Pointer to the device structure
 * @param  delay          Fractional delay value
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_fractional_delay_set(adi_ad9081_device_t *device,
					    uint8_t delay[4]);

/**
 * @brief  Set Fractional Delay IO Mux
 *
 * @param  device         Pointer to the device structure
 * @param  mux            0: fdelay io <-> coarse ddc0 mixer, 1: fdelay io <-> coarse ddc3 mixer
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_fractional_delay_io_mux_set(adi_ad9081_device_t *device,
						   uint8_t mux);

/**
 * @brief  Set Triggering Programmable Delay
 *
 * @param  device         Pointer to the device structure
 * @param  delay          Triggering programmable delay
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_trig_prog_delay_set(adi_ad9081_device_t *device,
					   uint8_t delay);

/**
 * @brief  Set Rising Edge Triggering Enable
 *
 * @param  device         Pointer to the device structure
 * @param  enable         1 to enable risedge detection on trig_ph0 input (if using GPIO as trigger input, risedge detection is always enabled)
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_trig_rise_edge_enable_set(adi_ad9081_device_t *device,
						 uint8_t enable);

/**
 * @brief  Set Sysref Programmable Delay
 *
 * @param  device         Pointer to the device structure
 * @param  delay          Sysref programmable delay, a delay on input sysref before usage for resynchronization or timestamp modes
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_sysref_prog_delay_set(adi_ad9081_device_t *device,
					     uint8_t delay);

/**
 * @brief  Set Sysref Resync Mode
 *
 * @param  device         Pointer to the device structure
 * @param  mode           0: timestamp mode, 1: resync mode
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_sysref_resync_mode_set(adi_ad9081_device_t *device,
					      uint8_t mode);

/**
 * @brief  Set Sysref Rising Edge Enable
 *
 * @param  device         Pointer to the device structure
 * @param  enable         1 to enable risedge detection on sysref_ph0 input
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_sysref_rise_edge_enable_set(adi_ad9081_device_t *device,
						   uint8_t enable);

/**
 * @brief  Set Master Trigger Enable
 *
 * @param  device         Pointer to the device structure
 * @param  enable         1 to enable master trigger
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_master_trig_enable_set(adi_ad9081_device_t *device,
					      uint8_t enable);

/**
 * @brief  Set Loopback Master Trigger Enable
 *
 * @param  device         Pointer to the device structure
 * @param  enable         1 to enable loopback master trigger
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t
adi_ad9081_adc_loopback_master_trig_enable_set(adi_ad9081_device_t *device,
					       uint8_t enable);

/**
 * @brief  High level API for Master-Slave NCO Sync
 *
 * @param  device         Pointer to the device structure
 * @param  is_master      1 for master, 0 for slave
 * @param  trigger_src    0: sysref, 1: lmfc rising edge, 2: lmfc falling edge
 * @param  gpio_index     0~5 to select GPIO0 ~ GPIO5
 * @param  extra_lmfc_num Extra lmfc number in nco master-slave sync mode
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_nco_master_slave_sync(adi_ad9081_device_t *device,
					     uint8_t is_master,
					     uint8_t trigger_src,
					     uint8_t gpio_index,
					     uint8_t extra_lmfc_num);

/**
 * @brief  Set Signal Monitor's Clock Enable
 *
 * @param  device         Pointer to the device structure
 * @param  adcs           @see adi_ad9081_adc_select_e
 * @param  enable         1 to enable smon clock
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_smon_clk_enable_set(adi_ad9081_device_t *device,
					   uint8_t adcs, uint8_t enable);

/**
 * @brief  Set Signal Monitor's Serial Framer Mode Enable
 *
 * @param  device         Pointer to the device structure
 * @param  adcs           @see adi_ad9081_adc_select_e
 * @param  enable         1 to enable smon serial mode
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_smon_sframer_mode_enable_set(adi_ad9081_device_t *device,
						    uint8_t adcs,
						    uint8_t enable);

/**
 * @brief  Get Signal Monitor Frame Counter
 *
 * @param  device         Pointer to the device structure
 * @param  adcs           @see adi_ad9081_adc_select_e
 * @param  fcnt           Pointer to save frame counter
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_smon_frame_counter_get(adi_ad9081_device_t *device,
					      uint8_t adcs, uint8_t *fcnt);

/**
 * @brief  Set Signal Monitor's Period
 *
 * @param  device         Pointer to the device structure
 * @param  adcs           @see adi_ad9081_adc_select_e
 * @param  period         Period value
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_smon_period_set(adi_ad9081_device_t *device,
				       uint8_t adcs, uint32_t period);

/**
 * @brief  Get Signal Monitor's Status
 *
 * @param  device         Pointer to the device structure
 * @param  adcs           @see adi_ad9081_adc_select_e
 * @param  status         Signal monitor status
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_smon_status_get(adi_ad9081_device_t *device,
				       uint8_t adcs, uint32_t *status);

/**
 * @brief  Set Signal Monitor's Threshold
 *
 * @param  device         Pointer to the device structure
 * @param  adcs           @see adi_ad9081_adc_select_e
 * @param  thresh_low     Signal monitor GPIO lower threshold
 * @param  thresh_high    Signal monitor GPIO upper threshold
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_smon_thresh_set(adi_ad9081_device_t *device,
				       uint8_t adcs, uint16_t thresh_low,
				       uint16_t thresh_high);

/**
 * @brief  Set Signal Monitor's Update Trigger
 *
 * @param  device         Pointer to the device structure
 * @param  adcs           @see adi_ad9081_adc_select_e
 * @param  update         High transition on this signal will cause status value to change
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_smon_status_update_set(adi_ad9081_device_t *device,
					      uint8_t adcs, uint8_t update);

/**
 * @brief  Set Signal Monitor's Status Read Selection
 *
 * @param  device         Pointer to the device structure
 * @param  adcs           @see adi_ad9081_adc_select_e
 * @param  select         Signal monitor status readback selection, 1 for peak detector
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_smon_status_read_select_set(adi_ad9081_device_t *device,
						   uint8_t adcs,
						   uint8_t select);

/**
 * @brief  Enable Signal Monitor Peak Detector
 *
 * @param  device         Pointer to the device structure
 * @param  adcs           @see adi_ad9081_adc_select_e
 * @param  enable         1 to enable signal monitor peak detector
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t
adi_ad9081_adc_smon_peak_detector_enable_set(adi_ad9081_device_t *device,
					     uint8_t adcs, uint8_t enable);

/**
 * @brief  Set Signal Monitor JESD Link Selection
 *
 * @param  device         Pointer to the device structure
 * @param  adcs           @see adi_ad9081_adc_select_e
 * @param  select         0: jesd link0, 1: jesd link1
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_smon_jlink_select_set(adi_ad9081_device_t *device,
					     uint8_t adcs, uint8_t select);

/**
 * @brief  Set Signal Monitor GPIO Enable
 *
 * @param  device         Pointer to the device structure
 * @param  adcs           @see adi_ad9081_adc_select_e
 * @param  enable         1 to allow GPIO output of peak indication. Peak detector needs to be enabled for this feature.
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_smon_output_via_gpio_set(adi_ad9081_device_t *device,
						uint8_t adcs, uint8_t enable);

/**
 * @brief  Set Signal Monitor Serial Framer Enable
 *
 * @param  device         Pointer to the device structure
 * @param  adcs           @see adi_ad9081_adc_select_e
 * @param  enable         1 to enable serial monitor
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_smon_sframer_enable_set(adi_ad9081_device_t *device,
					       uint8_t adcs, uint8_t enable);

/**
 * @brief  Set Signal Monitor Serial Framer Mode
 *
 * @param  device         Pointer to the device structure
 * @param  adcs           @see adi_ad9081_adc_select_e
 * @param  mode           Mode value
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_smon_sframer_mode_set(adi_ad9081_device_t *device,
					     uint8_t adcs, uint8_t mode);

/**
 * @brief  Set Signal Monitor Serial Framer Input Selection
 *
 * @param  device         Pointer to the device structure
 * @param  adcs           @see adi_ad9081_adc_select_e
 * @param  select         Input selection value
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t
adi_ad9081_adc_smon_sframer_input_select_set(adi_ad9081_device_t *device,
					     uint8_t adcs, uint8_t select);

/**
 * @brief  Set Signal Monitor Synchronization Enable
 *
 * @param  device         Pointer to the device structure
 * @param  adcs           @see adi_ad9081_adc_select_e
 * @param  enable         1 to enable synchronization
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_smon_sync_enable_set(adi_ad9081_device_t *device,
					    uint8_t adcs, uint8_t enable);

/**
 * @brief  Set Signal Monitor Next Synchronization Mode
 *
 * @param  device         Pointer to the device structure
 * @param  adcs           @see adi_ad9081_adc_select_e
 * @param  mode           0: continuous mode, 1: next synchronization mode
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_adc_smon_next_sync_mode_set(adi_ad9081_device_t *device,
					       uint8_t adcs, uint8_t mode);

/**
 * @brief  Select jesd rx link
 *
 * @param  device         Pointer to the device structure
 * @param  links          Target link
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_rx_link_select_set(adi_ad9081_device_t *device,
					   adi_ad9081_jesd_link_select_e links);

/**
 * @brief  Enable or disable the descrambler
 *         Call after adi_ad9081_device_startup_tx().
 *
 * @param  device     Pointer to the device structure
 * @param  links      Target link
 * @param  dsr_en     1:Enable, 0:Disable
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_rx_descrambler_set(adi_ad9081_device_t *device,
					   adi_ad9081_jesd_link_select_e links,
					   uint8_t dsr_en);

/**
 * @brief  Enable or disable data invert on a particular lane for Rx
 *         Call after adi_ad9081_device_startup_tx().
 *
 * @param  device          Pointer to the device structure
 * @param  links           Target link
 * @param  logical_lane    Logical lane index (0~7)
 * @param  invert_en       1:Enable, 0:Disable
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_rx_lane_invert_set(adi_ad9081_device_t *device,
					   adi_ad9081_jesd_link_select_e links,
					   uint8_t logical_lane,
					   uint8_t invert_en);

/**
 * @brief  Configure the JESD Rx lane cross bar between physical lane and logic lane
 *         Call after adi_ad9081_device_startup_tx().
 *
 * @param  device          Pointer to the device structure
 * @param  links           Target link
 * @param  physical_lane   Physical lane index (0~7)
 * @param  logical_lane    Logical lane index (0~7)
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_rx_lane_xbar_set(adi_ad9081_device_t *device,
					 adi_ad9081_jesd_link_select_e links,
					 uint8_t physical_lane,
					 uint8_t logical_lane);

/**
 * @brief  Configure the JESD Rx lanes cross bar between physical lane and logic lane
 *         Call after adi_ad9081_device_startup_tx().
 *
 * @param  device          Pointer to the device structure
 * @param  links           Target link
 * @param  logical_lanes   Logical lane index (0~7 for each value)
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_rx_lanes_xbar_set(adi_ad9081_device_t *device,
					  adi_ad9081_jesd_link_select_e links,
					  uint8_t logical_lanes[8]);

/**
 * @brief  Configure the JESD Rx Synca Mode
 *         Call after adi_ad9081_device_startup_tx().
 *
 * @param  device          Pointer to the device structure
 * @param  mode            0: CMOS, 1: LVDS
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_rx_synca_mode_set(adi_ad9081_device_t *device,
					  uint8_t mode);

/**
 * @brief  Configure the JESD Rx Syncb Mode
 *         Call after adi_ad9081_device_startup_tx().
 *
 * @param  device          Pointer to the device structure
 * @param  mode            0: CMOS, 1: LVDS
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_rx_syncb_mode_set(adi_ad9081_device_t *device,
					  uint8_t mode);

/**
 * @brief  Set Synca Driver Power Down
 *         Call after adi_ad9081_device_startup_tx().
 *
 * @param  device          Pointer to the device structure
 * @param  powerdown       1 to power down
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t
adi_ad9081_jesd_rx_synca_driver_powerdown_set(adi_ad9081_device_t *device,
					      uint8_t powerdown);

/**
 * @brief  Set Syncb Driver Power Down
 *         Call after adi_ad9081_device_startup_tx().
 *
 * @param  device          Pointer to the device structure
 * @param  powerdown       1 to power down
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t
adi_ad9081_jesd_rx_syncb_driver_powerdown_set(adi_ad9081_device_t *device,
					      uint8_t powerdown);

/**
 * @brief  Configure the Rx link settings
 *         Call after adi_ad9081_device_startup_tx().
 *
 * @param  device     Pointer to the device structure
 * @param  links      Target link
 * @param  jesd_param l,f,k,m,s,n,np,hd,dscr,did,lid, etc.
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_rx_link_config_set(adi_ad9081_device_t *device,
					   adi_ad9081_jesd_link_select_e links,
					   adi_cms_jesd_param_t *jesd_param);

/**
 * @brief  Enable or disable the JESD link for Rx
 *         Call after adi_ad9081_device_startup_tx().
 *
 * @param  device     Pointer to the device structure
 * @param  links      Target link
 * @param  link_en    1:Enable, 0:Disable
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_rx_link_enable_set(adi_ad9081_device_t *device,
					   adi_ad9081_jesd_link_select_e links,
					   uint8_t link_en);

/**
 * @brief  JRX link bring up (setting up JESD PLL, etc.)
 *         Call after adi_ad9081_device_startup_tx().
 *
 * @param  device     Pointer to the device structure
 * @param  links      Target link
 * @param  lanes      Target lanes to enable
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_rx_bring_up(adi_ad9081_device_t *device,
				    adi_ad9081_jesd_link_select_e links,
				    uint8_t lanes);

/**
 * @brief  Set LMFC delay
 *         Call after adi_ad9081_device_startup_tx().
 *
 * @param  device Pointer to the device structure
 * @param  links  Target link
 * @param  delay  Phase adjustment in conv_clk cycles. Maximum value is k*s/ns.
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_rx_lmfc_delay_set(adi_ad9081_device_t *device,
					  adi_ad9081_jesd_link_select_e links,
					  uint16_t delay);

/**
 * @brief  Set JRX SYNC# signal mode
 *         Call after adi_ad9081_device_startup_tx().
 *
 * @param  device     Pointer to the device structure
 * @param  links      Target link
 * @param  sync_mode  0: '1', 1: sync# signal from link0, 2: sync# signal from link1, 3: ANDed sync# signal from link0 & link1
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_rx_sync_mode_set(adi_ad9081_device_t *device,
					 adi_ad9081_jesd_link_select_e links,
					 uint8_t sync_mode);

/**
 * @brief  Set rx_run_cal_mask
 *         Call after adi_ad9081_device_startup_tx().
 *
 * @param  device        Pointer to the device structure
 * @param  mask          Value for rx_run_cal_mask, bit0 -> lane0, ...
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_rx_run_cal_mask_set(adi_ad9081_device_t *device,
					    uint8_t mask);

/**
 * @brief  Set rx_boost_mask
 *         Call after adi_ad9081_device_startup_tx().
 *
 * @param  device        Pointer to the device structure
 * @param  mask          Value for rx_boost_mask, bit0 -> lane0, ...
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_rx_boost_mask_set(adi_ad9081_device_t *device,
					  uint8_t mask);

/**
 * @brief  Calibrate 204C for JRX
 *         Call after adi_ad9081_device_startup_tx() and JESD TX is transmitting data.
 *
 * @param  device          Pointer to the device structure
 * @param  force_cal_reset Force calibration reset, must be 1 for the 1st time calibration after boot
 * @param  boost_mask      One hot per lane that sets what boost mode to operate in. 1 indicates high boost mode.
 *                         One usually sets high boost mode if the channels insertion loss is greater than 10 dB.
 * @param  run_bg_cal      Run background calibration or not
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_rx_calibrate_204c(adi_ad9081_device_t *device,
					  uint8_t force_cal_reset,
					  uint8_t boost_mask,
					  uint8_t run_bg_cal);

/**
 * @brief  Read jesd jrx link configuration status
 *
 * @param  device        Pointer to the device structure
 * @param  cfg_valid     Pointer to variable to store the jrx link status
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_rx_config_status_get(adi_ad9081_device_t *device,
					     uint8_t *cfg_valid);

/**
 * @brief  Get jrx link status
 *
 * @param  device     Pointer to the device structure
 * @param  links      Target link
 * @param  status     Pointer to link status
 *                        bit[ 7:0] - status for lane7:0 (1 means cgs, fs and ilas all exist for a lane)
 *                        bit[15:8] - 204c state (6 means link is up)
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_rx_link_status_get(adi_ad9081_device_t *device,
					   adi_ad9081_jesd_link_select_e links,
					   uint16_t *status);

/**
 * @brief  Set 204C CRC Error Interrupt Enable
 *
 * @param  device     Pointer to the device structure
 * @param  links      Target link
 * @param  enable     1 to enable crc error interrupt
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t
adi_ad9081_jesd_rx_204c_crc_irq_enable(adi_ad9081_device_t *device,
				       adi_ad9081_jesd_link_select_e links,
				       uint8_t enable);

/**
 * @brief  Get 204C CRC Error Interrupt Status
 *
 * @param  device     Pointer to the device structure
 * @param  links      Target link
 * @param  status     1 means interrupt has happened
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t
adi_ad9081_jesd_rx_204c_crc_irq_status_get(adi_ad9081_device_t *device,
					   adi_ad9081_jesd_link_select_e links,
					   uint8_t *status);

/**
 * @brief  Clear 204C CRC Error Interrupt Status
 *
 * @param  device     Pointer to the device structure
 * @param  links      Target link
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t
adi_ad9081_jesd_rx_204c_crc_irq_clr(adi_ad9081_device_t *device,
				    adi_ad9081_jesd_link_select_e links);

/**
 * @brief  Set 204C Multi-Block Error Interrupt Enable
 *
 * @param  device     Pointer to the device structure
 * @param  links      Target link
 * @param  enable     1 to enable multi-block error interrupt
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t
adi_ad9081_jesd_rx_204c_mb_irq_enable(adi_ad9081_device_t *device,
				      adi_ad9081_jesd_link_select_e links,
				      uint8_t enable);

/**
 * @brief  Get 204C Multi-Block Error Interrupt Status
 *
 * @param  device     Pointer to the device structure
 * @param  links      Target link
 * @param  status     1 means interrupt has happened
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t
adi_ad9081_jesd_rx_204c_mb_irq_status_get(adi_ad9081_device_t *device,
					  adi_ad9081_jesd_link_select_e links,
					  uint8_t *status);

/**
 * @brief  Clear 204C Multi-Block Error Interrupt Status
 *
 * @param  device     Pointer to the device structure
 * @param  links      Target link
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_rx_204c_mb_irq_clr(adi_ad9081_device_t *device,
					   adi_ad9081_jesd_link_select_e links);

/**
 * @brief  Set 204C Sync Header Error Interrupt Enable
 *
 * @param  device     Pointer to the device structure
 * @param  links      Target link
 * @param  enable     1 to enable sync header error interrupt
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t
adi_ad9081_jesd_rx_204c_sh_irq_enable(adi_ad9081_device_t *device,
				      adi_ad9081_jesd_link_select_e links,
				      uint8_t enable);

/**
 * @brief  Get 204C Sync Header Error Interrupt Status
 *
 * @param  device     Pointer to the device structure
 * @param  links      Target link
 * @param  status     1 means interrupt has happened
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t
adi_ad9081_jesd_rx_204c_sh_irq_status_get(adi_ad9081_device_t *device,
					  adi_ad9081_jesd_link_select_e links,
					  uint8_t *status);

/**
 * @brief  Clear 204C Sync Header Error Interrupt Status
 *
 * @param  device     Pointer to the device structure
 * @param  links      Target link
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_rx_204c_sh_irq_clr(adi_ad9081_device_t *device,
					   adi_ad9081_jesd_link_select_e links);

/**
 * @brief  Set JESD RX CTLE
 *         Call after adi_ad9081_device_startup_tx().
 *
 * @param  device      Pointer to the device structure
 * @param  lanes       Lane mask to apply passed il settings to.
 * @param  il_settings CTLE setting, @see adi_il_settings_e
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_rx_ctle_set(adi_ad9081_device_t *device, uint8_t lanes,
				    adi_ad9081_il_settings_e il_settings);

/**
 * @brief  Select jesd tx link
 *
 * @param  device     Pointer to the device structure
 * @param  links      Target link
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_tx_link_select_set(adi_ad9081_device_t *device,
					   adi_ad9081_jesd_link_select_e links);

/**
 * @brief  Configure the Tx link settings
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param  device     Pointer to the device structure
 * @param  links      Target link select
 * @param  jesd_param @see adi_cms_jesd_param_t, pass array with 2 elements for dual link
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_tx_link_config_set(adi_ad9081_device_t *device,
					   adi_ad9081_jesd_link_select_e links,
					   adi_cms_jesd_param_t *jesd_param);

/**
 * @brief  Enable or disable the scrambler
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param  device     Pointer to the device structure
 * @param  links      Target link select
 * @param  scr_en     1:Enable, 0:Disable
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_tx_scrambler_set(adi_ad9081_device_t *device,
					 adi_ad9081_jesd_link_select_e links,
					 uint8_t scr_en);

/**
 * @brief  Startup serializer
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param  device     Pointer to the device structure
 * @param  lanes      Active lanes
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_tx_startup_ser(adi_ad9081_device_t *device,
				       uint8_t lanes);

/**
 * @brief  Enable or disable 'force_power_down' on a particular lane for Tx
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param  device          Pointer to the device structure
 * @param  physical_lane   Physical lane index (0~7)
 * @param  power_down      1:Force power down, 0:Not force power down
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_tx_lane_force_pd_set(adi_ad9081_device_t *device,
					     uint8_t physical_lane,
					     uint8_t power_down);

/**
 * @brief  Enable or disable data invert on a particular lane for Tx
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param  device          Pointer to the device structure
 * @param  links           Target link select
 * @param  physical_lane   Physical lane index (0~7)
 * @param  invert_en       1:Enable, 0:Disable
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_tx_lane_invert_set(adi_ad9081_device_t *device,
					   adi_ad9081_jesd_link_select_e links,
					   uint8_t physical_lane,
					   uint8_t invert_en);

/**
 * @brief  Configure the JESD Tx lane cross bar between physical lane and logical lane
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param  device          Pointer to the device structure
 * @param  links           Target link select
 * @param  physical_lane   Physical lane index (0~7)
 * @param  logical_lane    Logical lane index (0~7)
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_tx_lane_xbar_set(adi_ad9081_device_t *device,
					 adi_ad9081_jesd_link_select_e links,
					 uint8_t physical_lane,
					 uint8_t logical_lane);

/**
 * @brief  Configure the JESD Tx lanes cross bar between physical lane and logical lane
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param  device          Pointer to the device structure
 * @param  links           Target link select
 * @param  logical_lanes   Logical lanes index (0~7 for each value)
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_tx_lanes_xbar_set(adi_ad9081_device_t *device,
					  adi_ad9081_jesd_link_select_e links,
					  uint8_t logical_lanes[8]);

/**
 * @brief  Select virtual converter
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param  device          Pointer to the device structure
 * @param  links           Target link select
 * @param  conv_index      Converter select index
 * @param  val             Converter select value (virutal converter index)
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_tx_conv_sel_set(adi_ad9081_device_t *device,
					adi_ad9081_jesd_link_select_e links,
					uint8_t conv_index, uint8_t val);

/**
 * @brief  Select converter control bit function
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param  device          Pointer to the device structure
 * @param  links           Target link select
 * @param  bit0_sel        0: overrange, 1: low, 2: SMON, 3: Fast Detection, 5: SYSREF, 8/9/10/11: NCO Chan Sel 0/1/2/3
 * @param  bit1_sel        0: overrange, 1: low, 2: SMON, 3: Fast Detection, 5: SYSREF, 8/9/10/11: NCO Chan Sel 0/1/2/3
 * @param  bit2_sel        0: overrange, 1: low, 2: SMON, 3: Fast Detection, 5: SYSREF, 8/9/10/11: NCO Chan Sel 0/1/2/3
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_tx_ctrl_bit_sel_set(adi_ad9081_device_t *device,
					    adi_ad9081_jesd_link_select_e links,
					    uint8_t bit0_sel, uint8_t bit1_sel,
					    uint8_t bit2_sel);

/**
 * @brief  Select output data format
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param  device          Pointer to the device structure
 * @param  links           Target link select
 * @param  format          0: 2's complement, 1: offset binary, 2: gray code
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_tx_format_sel_set(adi_ad9081_device_t *device,
					  adi_ad9081_jesd_link_select_e links,
					  uint8_t format);

/**
 * @brief  Configure chip output resolution
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param  device          Pointer to the device structure
 * @param  links           Target link select
 * @param  resolution      Chip output resolution, @see adi_ad9081_chip_output_res_e
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_tx_res_sel_set(adi_ad9081_device_t *device,
				       adi_ad9081_jesd_link_select_e links,
				       uint8_t resolution);

/**
 * @brief  Select converters to apply for fractional delay
 *
 * @param  device          Pointer to the device structure
 * @param  links           Target link select
 * @param  converters      bit0: converter0, bit1: conveter1, ... , bit15: conveter15
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_tx_fractional_delay_converter_selection_set(
	adi_ad9081_device_t *device, adi_ad9081_jesd_link_select_e links,
	uint16_t converters);

/**
 * @brief  Configure cross bar between coarse DDC and fine DDC
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param  device          Pointer to the device structure
 * @param  links           Target link select
 * @param  converters      Converters having FBW output
 *                             Bit[0] - converter 0
 *                             Bit[1] - converter 1
 *                             ...
 *                             Bit[F] - converter F
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_tx_fbw_sel_set(adi_ad9081_device_t *device,
				       adi_ad9081_jesd_link_select_e links,
				       uint16_t converters);

/**
 * @brief  Enable virtual converter test mode
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param  device          Pointer to the device structure
 * @param  links           Target link select
 * @param  enable          Enable test mode, bit0 - virtual converter0, bit1 - virtual converter1, ...
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_tx_conv_test_mode_enable_set(
	adi_ad9081_device_t *device, adi_ad9081_jesd_link_select_e links,
	uint16_t enable);

/**
 * @brief  Configure the lid for each lane
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param  device          Pointer to the device structure
 * @param  links           Target link select
 * @param  lane            lane index
 * @param  val             the val of lid
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_tx_lid_cfg_set(adi_ad9081_device_t *device,
				       adi_ad9081_jesd_link_select_e links,
				       uint8_t lane, uint8_t val);

/**
 * @brief  Configure the lid for all lanes
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param  device          Pointer to the device structure
 * @param  links           Target link select
 * @param  lids            LID values for all lanes
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_tx_lids_cfg_set(adi_ad9081_device_t *device,
					adi_ad9081_jesd_link_select_e links,
					uint8_t lids[8]);

/**
 * @brief  Force JESD Tx links reset
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param  device     Pointer to the device structure
 * @param  reset      1:reset, 0:not reset
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_tx_link_reset(adi_ad9081_device_t *device,
				      uint8_t reset);

/**
 * @brief  Enable/Disable jtx synca onchip termination
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param  device     Pointer to the device structure
 * @param  enable     1:Enable, 0:Disable
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_tx_synca_onchip_term_enable(adi_ad9081_device_t *device,
						    uint8_t enable);

/**
 * @brief  Enable/Disable jtx syncb onchip termination
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param  device     Pointer to the device structure
 * @param  enable     1:Enable, 0:Disable
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_tx_syncb_onchip_term_enable(adi_ad9081_device_t *device,
						    uint8_t enable);

/**
 * @brief  Enable or Disable jtx link
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param  device     Pointer to the device structure
 * @param  links      Target link select
 * @param  link_en    1:Enable, 0:Disable
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_tx_link_enable_set(adi_ad9081_device_t *device,
					   adi_ad9081_jesd_link_select_e links,
					   uint8_t link_en);

/**
 * @brief  Bring up jtx link
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param  device        Pointer to the device structure
 * @param  links         Target link select
 * @param  lanes         Active lanes, bit0 - lane0, bit1 - lane1, ...
 * @param  jesd_conv_sel Virtual converter selection, @see adi_ad9081_adc_fine_ddc_converter_e
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_tx_bring_up(adi_ad9081_device_t *device,
				    adi_ad9081_jesd_link_select_e links,
				    uint8_t lanes,
				    adi_ad9081_jtx_conv_sel_t jesd_conv_sel[2]);

/**
 * @brief  Get jtx link status
 *
 * @param  device       Pointer to the device structure
 * @param  links        Target link select
 * @param  status       Pointer to jtx status
 *                          bit[3:0] - QBF status
 *                          bit4     - frame sync status
 *                          bit5     - jtx pll locked
 *                          bit6     - phase established
 *                          bit7     - jtx invalid mode
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_tx_link_status_get(adi_ad9081_device_t *device,
					   adi_ad9081_jesd_link_select_e links,
					   uint16_t *status);

/**
 * @brief  Set JTX SYNC# signal mode
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param  device     Pointer to the device structure
 * @param  links      Target link
 * @param  sync_mode  0 : sync0# as link sync source, 1: sync1# as link sync source
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_tx_sync_mode_set(adi_ad9081_device_t *device,
					 adi_ad9081_jesd_link_select_e links,
					 uint8_t sync_mode);

/**
 * @brief  Start onshot sync
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param  device       Pointer to the device structure
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_oneshot_sync(adi_ad9081_device_t *device);

/**
 * @brief  Run PRBS Test for JESD Receiver
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param device         Pointer to the device reference handle.
 * @param prbs_pattern   PRBS pattern identifier,
 *                       R0: PRBS7, PRBS15, PRBS31
 *                       R1: PRBS7, PRBS9, PRBS15, PRBS31
 * @param time_sec       Seconds for PRBS test duration time
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t
adi_ad9081_jesd_rx_phy_prbs_test(adi_ad9081_device_t *device,
				 adi_cms_jesd_prbs_pattern_e prbs_pattern,
				 uint32_t time_sec);

/**
 * @brief  Get PRBS test result for specific jrx lane
 *
 * @param device         Pointer to the device reference handle.
 * @param lane           lane index (0~7)
 * @param prbs_rx_result @see adi_ad9081_prbs_test_t
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_rx_phy_prbs_test_result_get(
	adi_ad9081_device_t *device, uint8_t lane,
	adi_ad9081_prbs_test_t *prbs_rx_result);

/**
 * @brief  Disable PHY PRBS Test
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param device         Pointer to the device reference handle.
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t
adi_ad9081_jesd_rx_phy_prbs_test_disable_set(adi_ad9081_device_t *device);

/**
 * @brief  Run Sample PRBS Test for JESD Receiver
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param device         Pointer to the device reference handle.
 * @param prbs_pattern   PRBS pattern identifier, @see adi_cms_jesd_prbs_pattern_e
 * @param lane           Lane index (0~7)
 * @param time_sec       Seconds for PRBS test duration time
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t
adi_ad9081_jesd_rx_sample_prbs_test(adi_ad9081_device_t *device,
				    adi_cms_jesd_prbs_pattern_e prbs_pattern,
				    uint8_t lane, uint32_t time_sec);

/**
 * @brief  Get Sample PRBS Test Result for JESD Receiver
 *
 * @param device         Pointer to the device reference handle.
 * @param error_flag     Error flag, bit0: prbs_invalid_data_flag_i, bit1: prbs_invalid_data_flag_q, bit2: prbs_error_flag_i, bit3: prbs_error_flag_q
 * @param error_count_i  Error counter
 * @param error_count_q  Error counter
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_rx_sample_prbs_test_result_get(
	adi_ad9081_device_t *device, uint8_t *error_flag,
	uint32_t *error_count_i, uint32_t *error_count_q);

/**
 * @brief  Run SPO Sweep for JESD Receiver
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param device         Pointer to the device reference handle.
 * @param lane           Lane index, 0 ~ 7
 * @param prbs_pattern   PRBS pattern identifier,
 *                       R0: PRBS7, PRBS15, PRBS31
 *                       R1: PRBS7, PRBS9, PRBS15, PRBS31
 * @param deser_mode     AD9081_HALF_RATE, AD9081_QUART_RATE
 * @param prbs_delay_sec Seconds for PRBS test duration time
 * @param left_spo       Good left SPO
 * @param right_spo      Good right SPO
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_rx_spo_sweep(adi_ad9081_device_t *device, uint8_t lane,
				     adi_cms_jesd_prbs_pattern_e prbs_pattern,
				     adi_ad9081_deser_mode_e deser_mode,
				     uint32_t prbs_delay_sec, uint8_t *left_spo,
				     uint8_t *right_spo);

/**
 * @brief  Run Checker Board (10101010, 10101010) Test for JESD Transmitter
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param device         Pointer to the device reference handle.
 * @param links          Target link
 * @param data_source    @see adi_ad9081_jesd_tx_test_data_src_e
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_tx_checker_board_test(
	adi_ad9081_device_t *device, adi_ad9081_jesd_link_select_e links,
	adi_ad9081_jesd_tx_test_data_src_e data_source);

/**
 * @brief  Run Word Toggle Test (11111111, 00000000) for JESD Transmitter
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param device         Pointer to the device reference handle.
 * @param links          Target link
 * @param data_source    @see adi_ad9081_jesd_tx_test_data_src_e
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_tx_word_toggle_test(
	adi_ad9081_device_t *device, adi_ad9081_jesd_link_select_e links,
	adi_ad9081_jesd_tx_test_data_src_e data_source);

/**
 * @brief  Run Ramp Pattern (00000100, 00000101) Test for JESD Transmitter
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param device         Pointer to the device reference handle.
 * @param links          Target link
 * @param data_source    @see adi_ad9081_jesd_tx_test_data_src_e
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t
adi_ad9081_jesd_tx_ramp_test(adi_ad9081_device_t *device,
			     adi_ad9081_jesd_link_select_e links,
			     adi_ad9081_jesd_tx_test_data_src_e data_source);

/**
 * @brief  Run Repeated User Pattern (uuuuuuuu, uuuuuuuu) Test for JESD Transmitter
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param device         Pointer to the device reference handle.
 * @param links          Target link
 * @param data_source    @see adi_ad9081_jesd_tx_test_data_src_e
 * @param data           user data
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_tx_repeat_user_data_test(
	adi_ad9081_device_t *device, adi_ad9081_jesd_link_select_e links,
	adi_ad9081_jesd_tx_test_data_src_e data_source, uint8_t data[9]);

/**
 * @brief  Run PRBS Test for JESD Transmitter (phy has prbs data)
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param device         Pointer to the device reference handle.
 * @param links          Target link, @see adi_ad9081_jesd_link_select_e
 * @param prbs_pattern   PRBS pattern identifier, @see adi_cms_jesd_prbs_pattern_e
 *                       PRBS7, PRBS15, PRBS31
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t
adi_ad9081_jesd_tx_phy_prbs_test(adi_ad9081_device_t *device,
				 adi_ad9081_jesd_link_select_e links,
				 adi_cms_jesd_prbs_pattern_e prbs_pattern);

/**
 * @brief  Run PRBS Test for JESD Transmitter (sample data has prbs data)
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param device         Pointer to the device reference handle.
 * @param links          Target link, @see adi_ad9081_jesd_link_select_e
 * @param prbs_pattern   PRBS pattern identifier, @see adi_cms_jesd_prbs_pattern_e
 *                       PRBS7, PRBS15, PRBS31
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_tx_sample_data_prbs_test(
	adi_ad9081_device_t *device, adi_ad9081_jesd_link_select_e links,
	adi_cms_jesd_prbs_pattern_e prbs_pattern);

/**
 * @brief  Enable ILAS Test Mode (Repeated ILAS)
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param device         Pointer to the device reference handle
 * @param links          Target link
 * @param enable         Test mode enable
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_tx_ilas_test_mode_enable_set(
	adi_ad9081_device_t *device, adi_ad9081_jesd_link_select_e links,
	uint8_t enable);

/**
 * @brief  Enable Continuous D21.5 Test Mode
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param device         Pointer to the device reference handle
 * @param links          Target link
 * @param lane_id        Lane index (0~7)
 * @param enable         Test mode enable
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_tx_continuous_d215_enable_set(
	adi_ad9081_device_t *device, adi_ad9081_jesd_link_select_e links,
	uint8_t lane_id, uint8_t enable);

/**
 * @brief  Enable RPAT Test Mode
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param device         Pointer to the device reference handle
 * @param links          Target link
 * @param lane_id        Lane index (0~7)
 * @param enable         Test mode enable
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_tx_rpat_enable_set(adi_ad9081_device_t *device,
					   adi_ad9081_jesd_link_select_e links,
					   uint8_t lane_id, uint8_t enable);

/**
 * @brief  Enable JSPAT Test Mode
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param device         Pointer to the device reference handle
 * @param links          Target link
 * @param lane_id        Lane index (0~7)
 * @param enable         Test mode enable
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_tx_jspat_enable_set(adi_ad9081_device_t *device,
					    adi_ad9081_jesd_link_select_e links,
					    uint8_t lane_id, uint8_t enable);

/**
 * @brief  Enable JTSPAT Test Mode
 *         Call after adi_ad9081_device_startup_rx().
 *
 * @param device         Pointer to the device reference handle
 * @param links          Target link
 * @param lane_id        Lane index (0~7)
 * @param enable         Test mode enable
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t
adi_ad9081_jesd_tx_jtspat_enable_set(adi_ad9081_device_t *device,
				     adi_ad9081_jesd_link_select_e links,
				     uint8_t lane_id, uint8_t enable);

/**
 * @brief  Set sysref enable
 *
 * @param  device     Pointer to the device structure
 * @param  enable     1:Enable, 0:Disable
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_sysref_enable_set(adi_ad9081_device_t *device,
					  uint8_t enable);

/**
 * @brief  Set sysref capture enable
 *
 * @param  device     Pointer to the device structure
 * @param  enable     1:Enable, 0:Disable
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_sysref_spi_enable_set(adi_ad9081_device_t *device,
					      uint8_t enable);

/**
 * @brief  Set sysref input mode
 *
 * @param  device     Pointer to the device structure
 * @param  input_mode 0:AC couple, 1:DC couple
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_sysref_input_mode_set(adi_ad9081_device_t *device,
					      uint8_t input_mode);

/**
 * @brief  Set loop back mode
 *
 * @param device         Pointer to the device reference handle.
 * @param mode           loop back mode (0/1)
 *                           0: no loop back
 *                           1: jrx lane fifo -> jtx lane fifo
 *
 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_jesd_loopback_mode_set(adi_ad9081_device_t *device,
					  uint8_t mode);

#ifdef __cplusplus
}
#endif

#endif /* __ADI_AD9081_H__ */
/*! @} */