/*!
 * \brief     DDC Block definition headers
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_DFORMAT
 * @{
 */
#ifndef __ADI_APOLLO_DFORMAT_TYPES_H__
#define __ADI_APOLLO_DFORMAT_TYPES_H__

/*============= I N C L U D E S ============*/
#include "adi_cms_api_common.h"
#include "adi_apollo_common.h"
#include "adi_apollo_rxmux_types.h"

/*============= D E F I N E S ==============*/
#define ADI_APOLLO_DFORMAT_FDDC_SMON_I_PATH 0
#define ADI_APOLLO_DFORMAT_FDDC_SMON_Q_PATH 1

/*!
* \brief Enumerates Digital ADC Data Format selection
*/
typedef enum {
    ADI_APOLLO_DFOR_COMPLEMENT_2   = 0x0,                   /*!< 2's complement */
    ADI_APOLLO_DFOR_OFFSET_BINARY  = 0x1,                   /*!< Offset Binary */
    ADI_APOLLO_DFOR_GRAY_CODE      = 0x2,                   /*!< Gray Code */
} adi_apollo_dformat_sel_e;

/*!
 * \brief Enumerates Chip Output Resolution
 */
typedef enum {
    ADI_APOLLO_CHIP_OUT_RES_16BIT = 0x0,                  /*!< 16Bit */
    ADI_APOLLO_CHIP_OUT_RES_15BIT = 0x1,                  /*!< 15Bit */
    ADI_APOLLO_CHIP_OUT_RES_14BIT = 0x2,                  /*!< 14Bit */
    ADI_APOLLO_CHIP_OUT_RES_13BIT = 0x3,                  /*!< 13Bit */
    ADI_APOLLO_CHIP_OUT_RES_12BIT = 0x4,                  /*!< 12Bit */
    ADI_APOLLO_CHIP_OUT_RES_11BIT = 0x5,                  /*!< 11Bit */
    ADI_APOLLO_CHIP_OUT_RES_10BIT = 0x6,                  /*!< 10Bit */
    ADI_APOLLO_CHIP_OUT_RES_09BIT = 0x7,                  /*!<  9Bit */
    ADI_APOLLO_CHIP_OUT_RES_08BIT = 0x8                   /*!<  8Bit */
} adi_apollo_chip_output_res_e;

/*!
 * \brief Enumerates JESD control bit selection
 */
typedef enum {
    ADI_APOLLO_DFOR_CTRL_OVR = 0x0,                     /*!< Ctrl bit assigned to overflow */
    ADI_APOLLO_DFOR_CTRL_LOW = 0x1,                     /*!< Ctrl bit always low */
    ADI_APOLLO_DFOR_CTRL_SMON = 0x2,                    /*!< Ctrl bit assigned to SMON */
    ADI_APOLLO_DFOR_CTRL_FD = 0x3,                      /*!< Ctrl bit assigned to Fast Detect */
    ADI_APOLLO_DFOR_CTRL_SYSREF = 0x5,                  /*!< Ctrl bit assigned to SYSREF (fullbandwidth mode only) */
    ADI_APOLLO_DFOR_CTRL_INVALID = 0x6,                 /*!< Ctrl bit assigned to invalid data status */
    ADI_APOLLO_DFOR_CTRL_NEGFS = 0x7,                   /*!< Ctrl bit assigned to negative fullscale */
} adi_apollo_dformat_ctrl_bit_e;

/*!
 * \brief Enumerates FDDC I/Q to ADC select for FD and SMON
 */
typedef enum {
    ADI_APOLLO_DFOR_FDDC_ADC_A0 = 0,                    /*!< FDDC I/Q to ADC-A0 sel */
    ADI_APOLLO_DFOR_FDDC_ADC_A1 = 2,                    /*!< FDDC I/Q to ADC-A1 sel */
    ADI_APOLLO_DFOR_FDDC_ADC_A2 = 1,                    /*!< FDDC I/Q to ADC-A2 sel */
    ADI_APOLLO_DFOR_FDDC_ADC_A3 = 3                     /*!< FDDC I/Q to ADC-A3 sel */
} adi_apollo_dformat_fddc_adc_e;

/*!
* \brief DFORMAT programming data
*/
typedef struct {
    adi_apollo_dformat_sel_e dfor_sel;                /*!< \ref adi_apollo_dformat_sel_e */
    uint8_t dfor_inv;                                 /*!< Digital ADC Sample Invert on link. 0: ADC sample data is NOT inverted, 1: ADC sample data is inverted */
    uint8_t dfor_ddc_dither_en;                       /*!< Dformat dither enable for DDC mode for link. 0: dformat dither disable, 1: dformat dither enable */
    adi_apollo_chip_output_res_e dfor_res;            /*!< \ref adi_apollo_chip_output_res_e */
    uint8_t link_en;                                  /*!< Link 1:Enable, 0:Disable */
    uint16_t dcm_ratio;                               /*!< Chip Decimation Ratio corresponding to link. The least overall decimation among DDCs present in link */
    uint16_t total_dcm;                               /*!< Link Total Decimation */
	uint8_t invalid_en;                               /*!< Invalid sample enable */
	uint8_t sample_repeat_en;                         /*!< Sample repeat enable */
} adi_apollo_dformat_pgm_t;

/*!
* \brief DFORMAT SMON and Fast Detect map to FDDC I/Q path sel
*/
typedef struct {
    adi_apollo_rxmux_cbout_sel_e cbout;               /*!< SMON to ADC connect via Rx Mux1. CBout_x */
    adi_apollo_blk_sel_t fddc_select;                 /*!< FDDC select \ref adi_apollo_fine_ddc_select_e */
    uint8_t i_q_select;                               /*!< FDDC I or Q path */
} adi_apollo_dformat_smon_fd_map_t;

/*!
 * \brief DFORMAT inspect params
 */
typedef struct
{
    adi_apollo_dformat_cfg_t dp_cfg;                  /*!< \ref adi_apollo_dformat_cfg_t */
    uint8_t link_en;                                  /*!< Link 1:Enable, 0:Disable */
    uint16_t dcm_ratio;                               /*!< Chip Decimation Ratio corresponding to link. The least overall decimation among DDCs present in link */
    uint16_t total_dcm;                               /*!< Link Total Decimation */

} adi_apollo_dformat_inspect_t;

#endif /* __ADI_APOLLO_DFORMAT_TYPES_H__ */
/*! @} */
