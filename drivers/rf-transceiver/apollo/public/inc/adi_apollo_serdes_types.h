/*!
 * \brief     SerDes Block definition headers
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_SERDES
 * @{
 */
#ifndef __ADI_APOLLO_SERDES_TYPES_H__
#define __ADI_APOLLO_SERDES_TYPES_H__

/*============= I N C L U D E S ============*/
#include "adi_cms_api_common.h"
#include "adi_apollo_common.h"
#include "adi_apollo_mailbox_types.h"

/*============= D E F I N E S ==============*/

#define ADI_APOLLO_SERDES_JRX_VERT_EYE_TEST_RESP_BUF_SIZE          (66U)

/*!
* \brief Enumerates the serdes pll powerup selection
*/
typedef enum {
    ADI_APOLLO_SERDES_PLL_POWER_DOWN = 0,               /*!< Power down the PLL */
    ADI_APOLLO_SERDES_PLL_POWER_UP = 1,                 /*!< Power up the PLL */
} adi_apollo_serdes_pll_power_up_down_e;

/*!
* \brief Enumerates the serdes pll reset selection
*/
typedef enum {
    ADI_APOLLO_SERDES_PLL_RESET_SYNTH = 0,              /*!< Synth reset */
    ADI_APOLLO_SERDES_PLL_RESET_ODIV = 1,               /*!< Output divider reset */
    ADI_APOLLO_SERDES_PLL_RESET_NUM = 2                 /*!< Number of serdes reset types */
} adi_apollo_serdes_pll_reset_e;

/*!
* \brief Enumerates the serdes tx clock divider
*/
typedef enum {
    ADI_APOLLO_SERDES_JTX_CLOCK_DIV_1 = 0,          /*!< tx serial clk div 1 */
    ADI_APOLLO_SERDES_JTX_CLOCK_DIV_2 = 1,          /*!< tx serial clk div 2 */
    ADI_APOLLO_SERDES_JTX_CLOCK_DIV_4 = 2,          /*!< tx serial clk div 3 */
    ADI_APOLLO_SERDES_JTX_CLOCK_DIV_PD = 3,         /*!< tx serial clk div powerdown */
} adi_apollo_serdes_tx_clock_div_e;

/*!
* \brief Enumerates the serdes tx parallel data width
*/
typedef enum {
    ADI_APOLLO_SERDES_JTX_DATA_WIDTH_66 = 1,            /*!< Parallel data width 66 (JESD204C) */
    ADI_APOLLO_SERDES_JTX_DATA_WIDTH_40 = 2,            /*!< Parallel data width 40 (JESD204B) */
} adi_apollo_serdes_tx_data_width_e;


/*!
* \brief Enumerates the JRx serdes PRBS pattern mode
*/
typedef enum {
    ADI_APOLLO_SERDES_JRX_PRBS7 = 0,        /*!< PRBS7 serdes test pattern */
    ADI_APOLLO_SERDES_JRX_PRBS9 = 1,        /*!< PRBS9 serdes test pattern */
    ADI_APOLLO_SERDES_JRX_PRBS15 = 2,       /*!< PRBS15 serdes test pattern */
    ADI_APOLLO_SERDES_JRX_PRBS31 = 3        /*!< PRBS31 serdes test pattern */
} adi_apollo_serdes_jrx_prbs_mode_e;

/*!
* \brief Enumerates JTx serdes data and prbs test patterns
*/
typedef enum {
    ADI_APOLLO_SERDES_JTX_DATA_00 = 0,      /*!< 0x00 serdes test pattern */
    ADI_APOLLO_SERDES_JTX_DATA_FF = 1,      /*!< 0xFF serdes test pattern */
    ADI_APOLLO_SERDES_JTX_DATA_AA = 2,      /*!< 0xAA serdes test pattern */
    ADI_APOLLO_SERDES_JTX_DATA_55 = 3,      /*!< 0x55 serdes test pattern */
    ADI_APOLLO_SERDES_JTX_DATA_CC = 4,      /*!< 0xCC serdes test pattern */
    ADI_APOLLO_SERDES_JTX_DATA_33 = 5,      /*!< 0x33 serdes test pattern */
    ADI_APOLLO_SERDES_JTX_DATA_F0 = 6,      /*!< 0xF0 serdes test pattern */
    ADI_APOLLO_SERDES_JTX_DATA_0F = 7,      /*!< 0x0F serdes test pattern */
    ADI_APOLLO_SERDES_JTX_PRBS7 = 8,        /*!< PRBS7 serdes test pattern */
    ADI_APOLLO_SERDES_JTX_PRBS9 = 9,        /*!< PRBS9 serdes test pattern */
    ADI_APOLLO_SERDES_JTX_PRBS15 = 10,      /*!< PRBS15 serdes test pattern */
    ADI_APOLLO_SERDES_JTX_PRBS31 = 11,      /*!< PRBS31 serdes test pattern */
    ADI_APOLLO_SERDES_JTX_DATA_FF00 = 12,   /*!< 0xFF00 serdes test pattern */
    ADI_APOLLO_SERDES_JTX_DATA_00FF = 13,   /*!< 0x00FF serdes test pattern */
} adi_apollo_serdes_jtx_prbs_mode_e;

/*!
* \brief Enumerates CK SPO strobe
*/
typedef enum {
    ADI_APOLLO_SERDES_CK_SPO_STROBE_NONE = 0,
    ADI_APOLLO_SERDES_CK_SPO_ISTROBE = 1,
    ADI_APOLLO_SERDES_CK_SPO_QSTROBE = 2,
} adi_apollo_serdes_ck_spo_strobe_e;

/*!
* \brief Enumerates current state of SERDES Tracking / BG Calibration
*/
typedef enum {
    ADI_APOLLO_SERDES_BGCAL_STATE_SUSPENDED    = APOLLO_CALFRMWRK_STATE_SUSPENDED,     /*!< code: 0x01 Cal's timer is not running */
    ADI_APOLLO_SERDES_BGCAL_STATE_RESUMED      = APOLLO_CALFRMWRK_STATE_RESUMED,       /*!< code: 0x02 Cal's timer is running */
    ADI_APOLLO_SERDES_BGCAL_STATE_INACTIVE     = APOLLO_CALFRMWRK_STATE_INACTIVE,      /*!< code: 0x04 Cal's Main function is not executing */
    ADI_APOLLO_SERDES_BGCAL_STATE_RUNNING      = APOLLO_CALFRMWRK_STATE_RUNNING,       /*!< code: 0x08 Cal's Main function is executing */
    ADI_APOLLO_SERDES_BGCAL_STATE_ENABLED      = APOLLO_CALFRMWRK_STATE_ENABLED,       /*!< code: 0x10 Cal is enabled back from the host */
    ADI_APOLLO_SERDES_BGCAL_STATE_DISABLED     = APOLLO_CALFRMWRK_STATE_DISABLED,      /*!< code: 0x20 Cal is disabled from the host */
    ADI_APOLLO_SERDES_BGCAL_STATE_ERROR        = APOLLO_CALFRMWRK_STATE_ERROR,         /*!< code: 0x40 Cal is errored out */
    ADI_APOLLO_SERDES_BGCAL_STATE_UNKNOWN      = 0xFFFF
} adi_apollo_serdes_bgcal_state_e;

/*!
* \brief SERDES Tracking / BG Cal error and status struct
*/
typedef struct {
    uint32_t state_valid;           /* 1 = bgcal_error and bgcal_state fields have valid values, 0 = state unknown */
    uint32_t bgcal_error;           /* 0 = no error */
    uint32_t bgcal_state;           /* cal state bits \ref adi_apollo_serdes_bgcal_state_e */
} adi_apollo_serdes_bgcal_state_t;

/*!
* \brief SerDes Pll configuration struct
*/
typedef struct {
    uint16_t    sdm_int;                        /*!< Feedback divider (N) */
    uint8_t     ref_clk_byte0;                  /*!< Ref clock divider (1..63) (R) */
    uint8_t     serdes_pll_odiv;                /*!< /33 feedback clock divider (for async mode) */
    uint8_t     root_clkdiv_fund;               /*!< /33 low or high range divider (for async mode) */
    uint8_t     root_clkdiv_div2;               /*!< /33 post /1 or /2 */
    uint8_t     vco_sel;                        /*!< Vco sel */
} adi_apollo_serdes_pll_pgm_t;

/*!
* \brief Serializer configuration struct
*/
typedef struct {
    uint8_t tx_clock_div;                       /*!< Serdes tx data width. \ref adi_apollo_serdes_tx_clock_div_e */
    uint8_t tx_data_width;                      /*!< Serdes tx data width. \ref adi_apollo_serdes_tx_data_width_e */
    uint8_t pd_ser;                             /*!< 1 = powerdown serialize. 0 = normal */
    uint8_t jesd_ver;                           /*!< JESD204 version. 0 = 204B, 1 = 204C */
} adi_apollo_serializer_pgm_t;

/*!
* \brief PRBS Generator (JTx) configuration struct
*/
typedef struct {
    uint8_t enable;                             /*!< 1 = Enable. 0 = Disable */
    uint8_t mode;                               /*!< PRBS and data test pattern. \ref adi_apollo_serdes_jtx_prbs_mode_e */
} adi_apollo_serdes_prbs_generator_enable_t;

/*!
* \brief JRx PRBS checker configuration struct
*/
typedef struct {
    uint8_t enable;                             /*!< In regular mode 1 = Enable. 0 = Disable */
    uint8_t auto_mode;                          /*!< 1 = auto mode. 0 = regular mode */
    uint8_t auto_mode_thres;                    /*!< How many words of data to check threshold - in 2^. 6-bits */
    uint8_t prbs_mode;                          /*!< PRBS pattern \ref adi_apollo_serdes_jrx_prbs_mode_e */
} adi_apollo_serdes_prbs_checker_enable_t;

/*!
* \brief PRBS Checker status
*/
typedef struct {
    uint8_t err;                                 /*!< Non-sticky bit, active output of error checker */
    uint8_t err_sticky;                          /*!< Sticky bit of 'have any errors occurred since last error count clear' */
    uint8_t auto_mode_done;                      /*!< error checker has finished checking the correct number of words in auto mode */
    uint32_t err_count;                          /*!< count of errors */
} adi_apollo_serdes_prbs_checker_status_t;

/*!
 * \brief JRx Horizontal Eye Monitor Test Response
 */
typedef struct
{
    uint8_t ver;            /*!< Results version */
    int8_t  spo_left;       /*!< Left Static Phase Offset Value */
    int8_t  spo_right;      /*!< Right Static Phase Offset Value */
} adi_apollo_serdes_jrx_horiz_eye_resp_t;

/*!
 * \brief JRx Vertical Eye Monitor Test Response
 */
typedef struct
{
    uint8_t ver;                                                                    /*!< Results version */
    int8_t  eye_heights_at_spo[ADI_APOLLO_SERDES_JRX_VERT_EYE_TEST_RESP_BUF_SIZE];  /*!< Test results buffer */
} adi_apollo_serdes_jrx_vert_eye_resp_t;

#endif /* __ADI_APOLLO_SERDES_TYPES_H__ */

/*! @} */
