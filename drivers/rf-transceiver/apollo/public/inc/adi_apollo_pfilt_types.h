/*!
 * \brief     PFILT Block definition headers
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_PFILT
 * @{
 */
#ifndef __ADI_APOLLO_PFILT_TYPES_H__
#define __ADI_APOLLO_PFILT_TYPES_H__

/*============= I N C L U D E S ============*/
#include "adi_cms_api_common.h"
#include "adi_apollo_common.h"

/*============= D E F I N E S ==============*/
#define ADI_APOLLO_PFILT_NUM 4
#define ADI_APOLLO_PFILT_BANK_NUM 4
#define ADI_APOLLO_PFILT_PER_SIDE 2
#define ADI_APOLLO_PFILT_BANKS_PER_REGMAP 2
#define ADI_APOLLO_PFILT_COEFF_NUM 32
#define ADI_APOLLO_PFILT_STREAMS_NUM 2

/*!
* \brief Enumerates PFLIT selection
*/

/*!
* \brief Enumerates A PFILT selection
*/
typedef enum
{
    ADI_APOLLO_PFILT_NONE = 0x0,    /*!< No PFILT */
    ADI_APOLLO_PFILT_A0 = 0x1,      /*!< PFILT 0 of SIDE A */
    ADI_APOLLO_PFILT_A1 = 0x2,      /*!< PFILT 1 of SIDE A (8t8r only) */
    ADI_APOLLO_PFILT_B0 = 0x4,      /*!< PFILT 0 of SIDE B */
    ADI_APOLLO_PFILT_B1 = 0x8,      /*!< PFILT 1 of SIDE B (8t8r only) */
    ADI_APOLLO_PFILT_ALL = 0xF,     /*!< All PFILT (8T8R) */
    ADI_APOLLO_PFILT_ALL_4T4R = 0x5 /*!< All PFILT (4T4R) */
} adi_apollo_pfilt_sel_e;

/*!
* \brief Enumerates A PFILT stream selection
*/
typedef enum {
    ADI_APOLLO_PFILT_STREAM_NONE  = 0x0,                        /*!< No PFILT streams */
    ADI_APOLLO_PFILT_STREAM_0     = 0x1,                        /*!< Stream 0 (I) */
    ADI_APOLLO_PFILT_STREAM_1     = 0x2,                        /*!< Stream 1 (Q) */
    ADI_APOLLO_PFILT_STREAM_ALL   = 0x3,                        /*!< All PFILT streams */
} adi_apollo_pfilt_stream_sel_e;

/*!
* \brief Enumerates PFLIT COEFF BANK selection
*/
typedef enum {
    ADI_APOLLO_PFILT_BANK_NONE = 0x00,                        /*!< No PFILT BANK */
    ADI_APOLLO_PFILT_BANK0     = 0x01,                        /*!< 1st coeff bank */
    ADI_APOLLO_PFILT_BANK1     = 0x02,                        /*!< 2nd coeff bank */
    ADI_APOLLO_PFILT_BANK2     = 0x04,                        /*!< 3rd coeff bank */
    ADI_APOLLO_PFILT_BANK3     = 0x08,                        /*!< 4th coeff bank */
    ADI_APOLLO_PFILT_BANK_ALL  = 0x0F,                        /*!< All PFILT BANK */
} adi_apollo_pfilt_bank_sel_e;

/*!
* \brief Enumerates PFILT profile selection mode
*/
typedef enum
{
    ADI_APOLLO_PFILT_CHAN_SEL_DIRECT_REGMAP = 0, /*!< Direct spi/hsci PFILT hop select */
    ADI_APOLLO_PFILT_CHAN_SEL_DIRECT_GPIO = 1,   /*!< Direct GPIO hop select cfg 0, GPIO[1:0] switch between all 4 coeff sets  */
    ADI_APOLLO_PFILT_CHAN_SEL_DIRECT_GPIO_1 = 2, /*!< Direct GPIO hop select cfg 1, GPIO[0] stream 0, GPIO[1] stream 1  */
    ADI_APOLLO_PFILT_CHAN_SEL_TRIG_REGMAP = 3,   /*!< Trigger based hopping. Scheduled Regmap  */
    ADI_APOLLO_PFILT_CHAN_SEL_TRIG_GPIO = 4,     /*!< Trigger based hopping. Use GPIO[1:0] to select between all 4 coeff sets  */
    ADI_APOLLO_PFILT_CHAN_SEL_TRIG_GPIO_1 = 5,   /*!< Trigger based hopping alternate select. Use GPIO[0] to select for stream 0, GPIO[1] for stream 1  */
    ADI_APOLLO_PFILT_CHAN_SEL_NUM
} adi_apollo_pfilt_profile_sel_mode_e;

/*!
* \brief Enumerates PFILT averaging selection mode
*/
typedef enum
{
    ADI_APOLLO_PFILT_AVE_DISABLE = 0,    /*!< Disable the averaging block ahead of PFILT */
    ADI_APOLLO_PFILT_AVE_ENABLE_ADD = 1, /*!< Enable the averaging block, add the two streams */
    ADI_APOLLO_PFILT_AVE_ENABLE_SUB = 2, /*!< Enable the averaging block, subtract the two streams */
    ADI_APOLLO_PFILT_AVE_NUM
} adi_apollo_pfilt_ave_mode_e;

/*!
* \brief PFILT MODE programming data
*/
typedef struct
{
    adi_apollo_pfilt_dq_mode_e dq_mode;        /*!< Dual or quad mode \ref adi_apollo_pfilt_dq_mode_e */
    adi_apollo_pfilt_data_e data;              /*!< Real or Complex data \ref adi_apollo_pfilt_data_e */
    uint8_t mode_switch;                       /*!< 0: Disable Modsw (averaging), 1: Enable */
    uint8_t add_sub_sel;                       /*!< 0: subtraction, 1: addition for Modsw (averaging) */
    adi_apollo_pfilt_mode_e pfir_i_mode;       /*!< Filter mode (e.g. bypass, real n/2, etc.) \ref adi_apollo_pfilt_mode_e */
    adi_apollo_pfilt_mode_e pfir_q_mode;       /*!< Filter mode (e.g. bypass, real n/2, etc.) \ref adi_apollo_pfilt_mode_e */
} adi_apollo_pfilt_mode_pgm_t;

/*!
* \brief PFILT gain and delay programming data
*/
typedef struct {
    adi_apollo_pfilt_gain_e pfir_ix_gain;      /*!< \ref adi_apollo_pfilt_gain_e PFIR IX GAIN */
    adi_apollo_pfilt_gain_e pfir_iy_gain;      /*!< \ref adi_apollo_pfilt_gain_e PFIR IY GAIN */
    adi_apollo_pfilt_gain_e pfir_qx_gain;      /*!< \ref adi_apollo_pfilt_gain_e PFIR QX GAIN */
    adi_apollo_pfilt_gain_e pfir_qy_gain;      /*!< \ref adi_apollo_pfilt_gain_e PFIR QY GAIN */
    uint8_t pfir_ix_scalar_gain;               /*!< PFIR_IX_SCALAR_GAIN (6 bit Gain Multiplier)*/
    uint8_t pfir_iy_scalar_gain;               /*!< PFIR_IY_SCALAR_GAIN (6 bit Gain Multiplier)*/
    uint8_t pfir_qx_scalar_gain;               /*!< PFIR_QX_SCALAR_GAIN (6 bit Gain Multiplier)*/
    uint8_t pfir_qy_scalar_gain;               /*!< PFIR_QY_SCALAR_GAIN (6 bit Gain Multiplier)*/
    uint8_t hc_delay;                          /*!< Programmable Delay for Half Complex Mode. Units are adc samples */
} adi_apollo_pfilt_gain_dly_pgm_t;

/*!
* \brief RX PFILT inspect param
*/
typedef struct {
    adi_apollo_pfilt_cfg_t dp_cfg;           /*!< Parameters defined from device profile \ref apollo_cpu_device_profile_types.h */

    uint8_t bank_sel;                        /*! Active filter bank (0-3) */
    uint8_t pfir_coeff_transfer;             /*!< Coefficient Transfer Signal. Transfers all coefficient data from master to slave registers. */
    uint8_t eq_gpio_sel;                     /*!< 0: REGMAP_SELECT: use rd_coeffIpage_sel to select the coeff bank, 1: GPIO_SELECT: use i_eq_gpio to select coeff bank */
    uint8_t gpio_config1;                    /*!< 0: Disable: use i_eq_gpio[1:0] to select the coeff bank, 1: Enable: use part select of i_eq_gpio[1:0] to select coeff bank */
} adi_apollo_pfilt_inspect_t;

#endif /* __ADI_APOLLO_PFILT_TYPES_H__ */

/*! @} */
