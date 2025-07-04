/*!
 * \brief     API header file
 *            This file contains all the publicly exposed methods and data
 *            structures to interface with API.
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_COMMON
 * @{
 */

#ifndef __ADI_APOLLO_COMMON_TYPES_H__
#define __ADI_APOLLO_COMMON_TYPES_H__

/*============= I N C L U D E S ============*/
#include "adi_cms_api_common.h"
#include "apollo_cpu_device_profile_types.h"
#include "adi_apollo_hal_regio_spi_types.h"
#include "adi_apollo_hal_regio_hsci_types.h"
#include "adi_apollo_startup_types.h"

/*============= D E F I N E S ==============*/
#define USE_PRIVATE_BF

#define adi_apollo_blk_sel_t uint16_t

#define ADI_APOLLO_ADC_NUM            8
#define ADI_APOLLO_ADC_PER_SIDE_NUM   4
#define ADI_APOLLO_DAC_NUM            8
#define ADI_APOLLO_DAC_PER_SIDE_NUM   4

#define ADI_APOLLO_NUM_JTX_SERDES_12PACKS 2
#define ADI_APOLLO_NUM_JRX_SERDES_12PACKS 2
#define ADI_APOLLO_NUM_JTX_LINKS          4
#define ADI_APOLLO_NUM_JRX_LINKS          4
#define ADI_APOLLO_NUM_JTX_LINKS_PER_SIDE 2
#define ADI_APOLLO_NUM_JRX_LINKS_PER_SIDE 2

#define ADI_APOLLO_NUM_LINX_PER_SIDE      4
#define ADI_APOLLO_NUM_CFIR_PER_SIDE      2
#define ADI_APOLLO_NUM_FSRC_PER_SIDE      2

#define ADI_APOLLO_JESD_SER_COUNT         12
#define ADI_APOLLO_JESD_DESER_COUNT       12

#define ADI_APOLLO_USE_FLOATING_TYPE      0
#define ADI_APOLLO_USE_SPI_BURST_MODE     0

#define ADI_APOLLO_LOW                0
#define ADI_APOLLO_HIGH               1

#define ADI_APOLLO_DISABLE            0
#define ADI_APOLLO_ENABLE             1

#define ADI_APOLLO_NUM_TRIG_MASTER_PER_SIDE      32

#define BF_INFO_EXTRACT(...)                BF_INFO_EXTRACT_2(__VA_ARGS__)
#define BF_INFO_EXTRACT_2(addr, bf_info)    (bf_info)

#define REG_ADDR_EXTRACT(...) 	            REG_ADDR_EXTRACT_2(__VA_ARGS__)
#define REG_ADDR_EXTRACT_2(addr, bf_info) 	(addr)

#define ADI_APOLLO_MAX_SLICES_PER_SIDE_NUM 4 /* Max number of slices per side. For 8T8R, 2 slices are split into 4 */

/**
 * The following ADI_APOLLO_xxx_IDX2B macros convert functional block indexes to a bit-wise selector.
 * Top level APIs use indexes (e.g. 0, 1, ...) plus a side select to address functional blocks.
 * Block level APIs use bitwise selectors and are scaled for 8T8R devices.
 */
#define ADI_APOLLO_SIDE_IDX2B(side) (1<<(side))
#define ADI_APOLLO_CDDC_IDX2B(side, idx) (1<<((side)*(ADI_APOLLO_CDDC_PER_SIDE_NUM) + (idx)))
#define ADI_APOLLO_FDDC_IDX2B(side, idx) (1<<((side)*(ADI_APOLLO_FDDC_PER_SIDE_NUM) + (idx)))
#define ADI_APOLLO_CDUC_IDX2B(side, idx) (1<<((side)*(ADI_APOLLO_CDUC_PER_SIDE_NUM) + (idx)))
#define ADI_APOLLO_FDUC_IDX2B(side, idx) (1<<((side)*(ADI_APOLLO_FDUC_PER_SIDE_NUM) + (idx)))
#define ADI_APOLLO_PFILT_IDX2B(side, idx) (1<<((side)*(ADI_APOLLO_PFILT_PER_SIDE) + (idx)))
#define ADI_APOLLO_PFILT_COEFF_IDX2B(idx)  (1<< (idx))
#define ADI_APOLLO_CFIR_IDX2B(side, idx) (1<<((side)*(ADI_APOLLO_CFIR_PER_SIDE_NUM) + (idx)))
#define ADI_APOLLO_CFIR_COEFF_SET_IDX2B(idx) (1<< (idx))
#define ADI_APOLLO_CFIR_PROFILE_IDX2B(idx) (1<< (idx))
#define ADI_APOLLO_FSRC_IDX2B(side, idx) (1<<((side)*(ADI_APOLLO_FSRC_PER_SIDE_NUM) + (idx)))
#define ADI_APOLLO_DFMT_IDX2B(side, idx) (1<<((side)*(ADI_APOLLO_JESD_LINKS) + (idx)))

/*!
 * \brief Enumerates ADC Select
 */
typedef enum {
    ADI_APOLLO_ADC_NONE = 0x0,          /*!< No ADC */
    ADI_APOLLO_ADC_A0 = 0x1,            /*!< ADC0 of SIDE A */
    ADI_APOLLO_ADC_A1 = 0x2,            /*!< ADC1 of SIDE A */
    ADI_APOLLO_ADC_A2 = 0x4,            /*!< ADC2 of SIDE A */
    ADI_APOLLO_ADC_A3 = 0x8,            /*!< ADC3 of SIDE A */
    ADI_APOLLO_ADC_B0 = 0x10,           /*!< ADC0 of SIDE B */
    ADI_APOLLO_ADC_B1 = 0x20,           /*!< ADC1 of SIDE B */
    ADI_APOLLO_ADC_B2 = 0x40,           /*!< ADC2 of SIDE B */
    ADI_APOLLO_ADC_B3 = 0x80,           /*!< ADC3 of SIDE B */
    ADI_APOLLO_ADC_ALL = 0xFF,          /*!< ALL ADCs (8T8R) */
    ADI_APOLLO_ADC_A_ALL = 0x0F,        /*!< ALL ADCs of SIDE A (8T8R) */
    ADI_APOLLO_ADC_B_ALL = 0xF0,        /*!< ALL ADCs of SIDE B (8T8R) */
    ADI_APOLLO_ADC_ALL_4T4R = 0x33,     /*!< ALL ADCs (4T4R) */
    ADI_APOLLO_ADC_A_ALL_4T4R = 0x03,   /*!< ALL ADCs of SIDE A (4T4R) */
    ADI_APOLLO_ADC_B_ALL_4T4R = 0x30,   /*!< ALL ADCs of SIDE B (4T4R) */
} adi_apollo_adc_select_e;

/*!
 * \brief Enumerates ADC Select - Side independent
 */
typedef enum {
    ADI_APOLLO_ADC_IDX_NONE = 0x0,          /*!< No ADC */
    ADI_APOLLO_ADC_0 = 0x1,                 /*!< ADC0 */
    ADI_APOLLO_ADC_1 = 0x2,                 /*!< ADC1 */
    ADI_APOLLO_ADC_2 = 0x4,                 /*!< ADC2 */
    ADI_APOLLO_ADC_3 = 0x8,                 /*!< ADC3 */
    ADI_APOLLO_ADC_IDX_ALL = 0xF,           /*!< All ADCs */
} adi_apollo_adc_idx_e;

/*!
 * \brief Enumerates DAC Select - Side independent
 */
typedef enum {
    ADI_APOLLO_DAC_IDX_NONE = 0x0,          /*!< No DAC */
    ADI_APOLLO_DAC_0 = 0x1,                 /*!< DAC0 */
    ADI_APOLLO_DAC_1 = 0x2,                 /*!< DAC1 */
    ADI_APOLLO_DAC_2 = 0x4,                 /*!< DAC2 */
    ADI_APOLLO_DAC_3 = 0x8,                 /*!< DAC3 */
    ADI_APOLLO_DAC_IDX_ALL = 0xF,           /*!< All DACs */
} adi_apollo_dac_idx_e;

/*!
 * \brief Enumerates DAC Select
 */
typedef enum {
    ADI_APOLLO_DAC_NONE = 0x0,          /*!< No DAC */
    ADI_APOLLO_DAC_A0 = 0x1,            /*!< DAC0 of SIDE A */
    ADI_APOLLO_DAC_A1 = 0x2,            /*!< DAC1 of SIDE A */
    ADI_APOLLO_DAC_A2 = 0x4,            /*!< DAC2 of SIDE A */
    ADI_APOLLO_DAC_A3 = 0x8,            /*!< DAC3 of SIDE A */
    ADI_APOLLO_DAC_B0 = 0x10,           /*!< DAC0 of SIDE B */
    ADI_APOLLO_DAC_B1 = 0x20,           /*!< DAC1 of SIDE B */
    ADI_APOLLO_DAC_B2 = 0x40,           /*!< DAC2 of SIDE B */
    ADI_APOLLO_DAC_B3 = 0x80,           /*!< DAC3 of SIDE B */
    ADI_APOLLO_DAC_ALL = 0xFF,          /*!< ALL DACs (8T8R) */
    ADI_APOLLO_DAC_A_ALL = 0x0F,        /*!< ALL DACs of SIDE A (8T8R) */
    ADI_APOLLO_DAC_B_ALL = 0xF0,        /*!< ALL DACs of SIDE B (8T8R) */
    ADI_APOLLO_DAC_ALL_4T4R = 0x33,     /*!< ALL DACs (4T4R) */
    ADI_APOLLO_DAC_A_ALL_4T4R = 0x03,   /*!< ALL DACs of SIDE A (4T4R) */
    ADI_APOLLO_DAC_B_ALL_4T4R = 0x30,   /*!< ALL DACs of SIDE B (4T4R) */
} adi_apollo_dac_select_e;

/*!
 * \brief Enumerates East/West side
 */
typedef enum {
    ADI_APOLLO_SIDE_NONE = 0x0,                             /*!< No side */
    ADI_APOLLO_SIDE_A = 0x1,                                /*!< A side select */
    ADI_APOLLO_SIDE_B = 0x2,                                /*!< B side select */
    ADI_APOLLO_SIDE_ALL = 0x3                               /*!< All sides */
} adi_apollo_side_select_e;

/*!
 * \brief Enumerates the serdes 12-packs
 */
typedef enum {
    ADI_APOLLO_TXRX_SERDES_12PACK_NONE =   0x00,            /*!< No select */
    ADI_APOLLO_TXRX_SERDES_12PACK_A =      0x01,            /*!< Tx or Rx 12pack side A */
    ADI_APOLLO_TXRX_SERDES_12PACK_B =      0x02,            /*!< Tx or Rx 12pack side B */
    ADI_APOLLO_TXRX_SERDES_12PACK_ALL =    0x03             /*!< Tx or Rx 12packs all sides */
} adi_apollo_serdes_12pack_select_e;

/*!
 * \brief Enumerates Link Select
 */
typedef enum {
    ADI_APOLLO_LINK_NONE = 0x0,                             /*!< No Link */
    ADI_APOLLO_LINK_A0 = 0x1,                               /*!< A side Link 0 */
    ADI_APOLLO_LINK_A1 = 0x2,                               /*!< A side Link 1 */
    ADI_APOLLO_LINK_B0 = 0x4,                               /*!< B side Link 0 */
    ADI_APOLLO_LINK_B1 = 0x8,                               /*!< B side Link 1 */
    ADI_APOLLO_LINK_ALL = 0xf                               /*!< All Links */
} adi_apollo_jesd_link_select_e;

/*!
 * \brief Enumerates Link side Select
 */
typedef enum {
    ADI_APOLLO_LINK_SIDE_NONE = 0x0,                        /*!< No Link */
    ADI_APOLLO_LINK_SIDE_A = 0x1,                           /*!< A side. Corresponds to both link 0 and link 1 of side A */
    ADI_APOLLO_LINK_SIDE_B = 0x4,                           /*!< B side. Corresponds to both link 0 and link 1 of side B */
    ADI_APOLLO_LINK_SIDE_ALL = 0x5                          /*!< All sides. Corresponds to all Links */
} adi_apollo_jesd_link_side_select_e;

/*!
* \brief Enumerates Terminal type selection
*/
typedef enum {
    ADI_APOLLO_RX     = 0x0,                 /*!< RX TERMINAL SIGNAL PATH*/
    ADI_APOLLO_TX     = 0x1,                 /*!< TX TERMINAL SIGNAL PATH */
} adi_apollo_terminal_e;

/*!
 * \brief Enumerates Reset Operation
 */
typedef enum {
    ADI_APOLLO_NO_RESET = 0,                              /*!< No Reset */
    ADI_APOLLO_SOFT_RESET = 1,                            /*!< Soft Reset */
    ADI_APOLLO_HARD_RESET = 2,                            /*!< Hard Reset */
    ADI_APOLLO_SOFT_RESET_AND_INIT = 3,                   /*!< Soft Reset Then Init */
    ADI_APOLLO_HARD_RESET_AND_INIT = 4                    /*!< Hard Reset Then Init */
} adi_apollo_reset_e;

/*!
 * \brief Enumerates JESD Serializer Swing Settings
 */
typedef enum {
    ADI_APOLLO_SER_SWING_1000 = 0,                        /*!< 1000 mV Swing */
    ADI_APOLLO_SER_SWING_850 = 1,                         /*!< 850 mV Swing */
    ADI_APOLLO_SER_SWING_750 = 2,                         /*!< 750 mV Swing */
    ADI_APOLLO_SER_SWING_500 = 3                          /*!< 500 mV Swing */
} adi_apollo_ser_swing_e;

/*!
 * \brief Enumerates JESD Serializer Pre-Emphasis Settings
 */
typedef enum {
    ADI_APOLLO_SER_PRE_EMP_0DB = 0,                       /*!< 0 db Pre-Emphasis */
    ADI_APOLLO_SER_PRE_EMP_3DB = 1,                       /*!< 3 db Pre-Emphasis */
    ADI_APOLLO_SER_PRE_EMP_6DB = 2                        /*!< 6 db Pre-Emphasis */
} adi_apollo_ser_pre_emp_e;

/*!
 * \brief Enumerates JESD Serializer Post-Emphasis Settings
 */
typedef enum {
    ADI_APOLLO_SER_POST_EMP_0DB = 0,                      /*!< 0 db Post-Emphasis */
    ADI_APOLLO_SER_POST_EMP_3DB = 1,                      /*!< 3 db Post-Emphasis */
    ADI_APOLLO_SER_POST_EMP_6DB = 2,                      /*!< 6 db Post-Emphasis */
    ADI_APOLLO_SER_POST_EMP_9DB = 3,                      /*!< 9 db Post-Emphasis */
    ADI_APOLLO_SER_POST_EMP_12DB = 4                      /*!< 12 db Post-Emphasis */
} adi_apollo_ser_post_emp_e;

/*!
* \brief Powerup control enable pin select
*/
typedef enum {
    ADI_APOLLO_PUC_EN_PIN_0 = 0,                /*!< Enable pin 0 select for a pwr control input */
    ADI_APOLLO_PUC_EN_PIN_1 = 1,                /*!< Enable pin 1 select for a pwr control input */
    ADI_APOLLO_PUC_EN_PIN_2 = 2,                /*!< Enable pin 2 select for a pwr control input */
    ADI_APOLLO_PUC_EN_PIN_3 = 3,                /*!< Enable pin 3 select for a pwr control input */
} adi_apollo_puc_en_pin_sel_e;

/*!
* \brief Powerup control clock rate
*/
typedef enum {
    ADI_APOLLO_PUC_CLK_RATE_FS_DIV_32 = 0,      /*!< Clock rate of Fs/32 (default) */
    ADI_APOLLO_PUC_CLK_RATE_FS_DIV_256 = 1,     /*!< Clock rate of Fs/32 (default) */
} adi_apollo_puc_clk_rate_sel_e;


/*!
* \brief Powerup control rising edge select
*/
typedef enum {
    ADI_APOLLO_PUC_EDGE_RISE_E0 = 0,     /*!< Converter enable rise on E0: after few retiming clock cycles from RxEn rising edge */
    ADI_APOLLO_PUC_EDGE_RISE_E1 = 1,     /*!< Converter enable rise on E1: after counter A reaches max programmed value. */
    ADI_APOLLO_PUC_EDGE_RISE_E2 = 2,     /*!< Converter enable rise on E2: after counters A+B reach max programmed value. */
    ADI_APOLLO_PUC_EDGE_RISE_E3 = 3,     /*!< Converter enable rise on E3: after counters A+B+C reach max programmed value. */
} adi_apollo_puc_edge_rise_sel_e;

/*!
* \brief Powerup control falling edge select
*/
typedef enum {
    ADI_APOLLO_PUC_EDGE_FALL_E4 = 0,     /*!< Converter enable fall on E4: after few retiming clock cycles from RxEn falling edge */
    ADI_APOLLO_PUC_EDGE_FALL_E5 = 1,     /*!< Converter enable fall on E5: after counter D reaches max programmed value. */
    ADI_APOLLO_PUC_EDGE_FALL_E6 = 2,     /*!< Converter enable fall on E6: after counters D+E reach max programmed value. */
    ADI_APOLLO_PUC_EDGE_FALL_E7 = 3,     /*!< Converter enable fall on E7: after counters D+E+F reach max programmed value. */
} adi_apollo_puc_edge_fall_sel_e;

/*!
 * \brief Enumerates slice select per side
 */
typedef enum {
    ADI_APOLLO_RX_SLICE_NONE = 0x00,              /*!< No slices */
    ADI_APOLLO_RX_SIDE_A_SLICE_0 = 0x01,          /*!< Side A slice 0 select */
    ADI_APOLLO_RX_SIDE_A_SLICE_1 = 0x02,          /*!< Side A slice 1 select */
    ADI_APOLLO_RX_SIDE_A_SLICE_2 = 0x04,          /*!< Side A slice 2 select (8t8r only) */
    ADI_APOLLO_RX_SIDE_A_SLICE_3 = 0x08,          /*!< Side A slice 3 select (8t8r only) */
    ADI_APOLLO_RX_SIDE_B_SLICE_0 = 0x10,          /*!< Side B slice 0 select */
    ADI_APOLLO_RX_SIDE_B_SLICE_1 = 0x20,          /*!< Side B slice 1 select */
    ADI_APOLLO_RX_SIDE_B_SLICE_2 = 0x40,          /*!< Side B slice 2 select (8t8r only) */
    ADI_APOLLO_RX_SIDE_B_SLICE_3 = 0x80,          /*!< Side B slice 3 select (8t8r only) */
    ADI_APOLLO_RX_SLICE_ALL = 0xFF,               /*!< All slices */
} adi_apollo_rx_slice_select_e;

/*!
 * \brief Enumerates slice select per side
 */
typedef enum {
    ADI_APOLLO_TX_SLICE_NONE     = 0x00,                    /*!< No slices */
    ADI_APOLLO_TX_SIDE_A_SLICE_0 = 0x01,                    /*!< Side A slice 0 select */
    ADI_APOLLO_TX_SIDE_A_SLICE_1 = 0x02,                    /*!< Side A slice 1 select */
    ADI_APOLLO_TX_SIDE_A_SLICE_2 = 0x04,                    /*!< Side A slice 2 select (8t8r only) */
    ADI_APOLLO_TX_SIDE_A_SLICE_3 = 0x08,                    /*!< Side A slice 3 select (8t8r only) */
    ADI_APOLLO_TX_SIDE_B_SLICE_0 = 0x10,                    /*!< Side B slice 0 select */
    ADI_APOLLO_TX_SIDE_B_SLICE_1 = 0x20,                    /*!< Side B slice 1 select */
    ADI_APOLLO_TX_SIDE_B_SLICE_2 = 0x40,                    /*!< Side B slice 2 select (8t8r only) */
    ADI_APOLLO_TX_SIDE_B_SLICE_3 = 0x80,                    /*!< Side B slice 3 select (8t8r only) */
    ADI_APOLLO_TX_SLICE_ALL      = 0xFF,                    /*!< All slices */
} adi_apollo_tx_slice_select_e;

/*!
 * \brief Enumerates LINX select
 */
typedef enum {
    ADI_APOLLO_LINX_NONE = 0x00,                           /*!< No LINX */
    ADI_APOLLO_LINX_A0   = 0x01,                           /*!< LINX 0 of SIDE A */
    ADI_APOLLO_LINX_A1   = 0x02,                           /*!< LINX 1 of SIDE A */
    ADI_APOLLO_LINX_A2   = 0x04,                           /*!< LINX 2 of SIDE A (8t8r only) */
    ADI_APOLLO_LINX_A3   = 0x08,                           /*!< LINX 3 of SIDE A (8t8r only) */
    ADI_APOLLO_LINX_B0   = 0x10,                           /*!< LINX 0 of SIDE B */
    ADI_APOLLO_LINX_B1   = 0x20,                           /*!< LINX 1 of SIDE B*/
    ADI_APOLLO_LINX_B2   = 0x40,                           /*!< LINX 2 of SIDE B (8t8r only) */
    ADI_APOLLO_LINX_B3   = 0x80,                           /*!< LINX 3 of SIDE B (8t8r only) */
    ADI_APOLLO_LINX_ALL  = 0xFF                            /*!< ALL LINXs */
} adi_apollo_linx_sel_e;

/*!
 * \brief Enumerates INVSINC select
 */
typedef enum {
    ADI_APOLLO_TX_INVSINC_NONE = 0x00,                     /*!< No TX_INVSINC */
    ADI_APOLLO_TX_INVSINC_A0   = 0x01,                     /*!< TX_INVSINC 0 of SIDE A */
    ADI_APOLLO_TX_INVSINC_A1   = 0x02,                     /*!< TX_INVSINC 1 of SIDE A */
    ADI_APOLLO_TX_INVSINC_A2   = 0x04,                     /*!< TX_INVSINC 2 of SIDE A (8t8r only) */
    ADI_APOLLO_TX_INVSINC_A3   = 0x08,                     /*!< TX_INVSINC 3 of SIDE A (8t8r only) */
    ADI_APOLLO_TX_INVSINC_B0   = 0x10,                     /*!< TX_INVSINC 0 of SIDE B */
    ADI_APOLLO_TX_INVSINC_B1   = 0x20,                     /*!< TX_INVSINC 1 of SIDE B*/
    ADI_APOLLO_TX_INVSINC_B2   = 0x40,                     /*!< TX_INVSINC 2 of SIDE B (8t8r only) */
    ADI_APOLLO_TX_INVSINC_B3   = 0x80,                     /*!< TX_INVSINC 3 of SIDE B (8t8r only) */
    ADI_APOLLO_TX_INVSINC_ALL  = 0xFF                      /*!< ALL TX_INVSINCs */
} adi_apollo_tx_invsinc_sel_e;

/*!
 * \brief Enumerates GAIN CTRL select
 */
typedef enum {
    ADI_APOLLO_TX_GAIN_NONE = 0x00,                        /*!< No TX_GAIN */
    ADI_APOLLO_TX_GAIN_A0   = 0x01,                        /*!< TX_GAIN 0 of SIDE A */
    ADI_APOLLO_TX_GAIN_A1   = 0x02,                        /*!< TX_GAIN 1 of SIDE A */
    ADI_APOLLO_TX_GAIN_A2   = 0x04,                        /*!< TX_GAIN 2 of SIDE A (8t8r only) */
    ADI_APOLLO_TX_GAIN_A3   = 0x08,                        /*!< TX_GAIN 3 of SIDE A (8t8r only) */
    ADI_APOLLO_TX_GAIN_B0   = 0x10,                        /*!< TX_GAIN 0 of SIDE B */
    ADI_APOLLO_TX_GAIN_B1   = 0x20,                        /*!< TX_GAIN 1 of SIDE B*/
    ADI_APOLLO_TX_GAIN_B2   = 0x40,                        /*!< TX_GAIN 2 of SIDE B (8t8r only) */
    ADI_APOLLO_TX_GAIN_B3   = 0x80,                        /*!< TX_GAIN 3 of SIDE B (8t8r only) */
    ADI_APOLLO_TX_GAIN_ALL  = 0xFF                         /*!< ALL TX_GAINs */
} adi_apollo_tx_gain_ctrl_sel_e;

typedef enum {
    ADI_APOLLO_TX_SRD_NONE = 0x00,                        /*!< No TX_SRD */
    ADI_APOLLO_TX_SRD_A0   = 0x01,                        /*!< TX_SRD 0 of SIDE A */
    ADI_APOLLO_TX_SRD_A1   = 0x02,                        /*!< TX_SRD 1 of SIDE A */
    ADI_APOLLO_TX_SRD_A2   = 0x04,                        /*!< TX_SRD 2 of SIDE A (8t8r only) */
    ADI_APOLLO_TX_SRD_A3   = 0x08,                        /*!< TX_SRD 3 of SIDE A (8t8r only) */
    ADI_APOLLO_TX_SRD_B0   = 0x10,                        /*!< TX_SRD 0 of SIDE B */
    ADI_APOLLO_TX_SRD_B1   = 0x20,                        /*!< TX_SRD 1 of SIDE B*/
    ADI_APOLLO_TX_SRD_B2   = 0x40,                        /*!< TX_SRD 2 of SIDE B (8t8r only) */
    ADI_APOLLO_TX_SRD_B3   = 0x80,                        /*!< TX_SRD 3 of SIDE B (8t8r only) */
    ADI_APOLLO_TX_SRD_ALL  = 0xFF                         /*!< ALL TX_SRDs */
} adi_apollo_tx_srd_sel_e;

/*!
* \brief NCO auto hop profile selection mode
*/
typedef enum {
    ADI_APOLLO_NCO_AUTO_HOP_DECR = 0,                   /*!< Auto hop decrement mode*/
    ADI_APOLLO_NCO_AUTO_HOP_INCR = 1,                   /*!< Auto hop increment mode */
    ADI_APOLLO_NCO_AUTO_HOP_FLIP = 2                    /*!< Auto hop flip mode. Uses hi/lo limits. */
} adi_apollo_nco_auto_flip_incdir_e;

#ifndef CLIENT_IGNORE

typedef struct {
    adi_apollo_blk_sel_t   adc;
    adi_apollo_blk_sel_t   bmem;
    adi_apollo_blk_sel_t   cddc;
    adi_apollo_blk_sel_t   cduc;
    adi_apollo_blk_sel_t   cfir;
    adi_apollo_blk_sel_t   cnco;
    adi_apollo_blk_sel_t   dac; 
    adi_apollo_blk_sel_t   fddc;
    adi_apollo_blk_sel_t   fduc;
    adi_apollo_blk_sel_t   fnco;
    adi_apollo_blk_sel_t   fsrc;
    adi_apollo_blk_sel_t   invsinc;
    adi_apollo_blk_sel_t   pfilt;
    adi_apollo_blk_sel_t   rxen;
    adi_apollo_blk_sel_t   smon;
    adi_apollo_blk_sel_t   sniffer;
    adi_apollo_blk_sel_t   txen;
    adi_apollo_blk_sel_t   txmux;
} adi_apollo_device_blk_sel_mask_t;

/*!
 * \brief Per lane JESD Serializer Settings
 */
typedef struct {
    adi_apollo_ser_swing_e    swing_setting;
    adi_apollo_ser_pre_emp_e  pre_emp_setting;
    adi_apollo_ser_post_emp_e post_emp_setting;
} adi_apollo_ser_lane_settings_t;



/*!
 * \brief Device Hardware Abstract Layer Structure
 */
typedef struct {
    void * user_data;                               /*!< Pointer to generic user data. */
    void * dev_hal_info;                            /*!< Pointer to user platform hal object */

    adi_apollo_hal_regio_spi_desc_t spi0_desc;      /*!< SPI0 HAL descriptor */
    adi_apollo_hal_regio_spi_desc_t spi1_desc;      /*!< SPI1 HAL descriptor */
    adi_apollo_hal_regio_hsci_desc_t hsci_desc;     /*!< HSCI HAL descriptor */

    void *active_regio;                             /*!< Active device protocol (e.g. sp0, sp1, hsci) */

    adi_apollo_hal_open_t hw_open;                  /*!< Platform hal open (optional) */
    adi_apollo_hal_close_t hw_close;                /*!< Platform hal close (optional) */
    adi_apollo_hal_delay_us_t delay_us;             /*!< Platform hal delay us (required) */
    adi_apollo_hal_reset_pin_ctrl_t reset_pin_ctrl; /*!< Platform hal reset pin control (required) */
    adi_apollo_hal_log_write_t log_write;           /*!< Platform hal log write function (optional) */
} adi_apollo_hal_t;

/*!
 * \brief Device Internal Information Structure
 */
typedef struct {
    uint64_t dev_freq_hz;                           /*!< Device clock frequency in Hz */
    uint64_t dac_freq_hz;                           /*!< DAC clock frequency in Hz */
    uint64_t adc_freq_hz;                           /*!< ADC clock frequency in Hz */
    uint8_t dev_rev;                                /*!< Device revision, 0:a0, 1:b0 */
    uint8_t is_8t8r;                                /*!< 0 if 4t4r device, 1 if 8t8r device */
    uint8_t is_dual_clk;                            /*!< Whether the device is in dual clk mode */

    adi_apollo_device_lockout_t lockout_mask;       /*!< device variant feature lockout */
    adi_apollo_device_blk_sel_mask_t blk_sel_mask;  /*!< device block instance select masks */
} adi_apollo_info_t;

/*!
 * \brief Device startup sequence information structure
 *
 * The adi_apollo_hal_fw_provider_t data struct, along with open, close and get methods, create an interface for providing 
 * FW binaries during adi_apollo_startup_execute() execution. This provides a platform agnostic interface for loading
 * the Apollo firmware.
 * 
 * All memory management, file I/O, etc. is handled in user app space.
 */
typedef struct {
    adi_apollo_fw_provider_t *fw_provider;          /*!< Platform FW image provider. Used in startup sequence */

    adi_apollo_fw_provider_open_t open;             /*!< Called from adi_apollo_startup_execute() before 'get' when writing a FW binary to ARM (may be NULL) */
    adi_apollo_fw_provider_close_t close;           /*!< Called from adi_apollo_startup_execute() after 'get' when writing a FW binary ARM (may be NULL) */
    adi_apollo_fw_provider_get_t get;               /*!< Called from adi_apollo_startup_execute() to obtain binary data to be loaded to ARM. Required. */
} adi_apollo_startup_t;

/*!
* \brief Device Structure
*/
typedef struct adi_apollo_device_t {
    adi_apollo_hal_t  hal_info;
    adi_apollo_info_t dev_info;
    adi_apollo_startup_t startup_info;
} adi_apollo_device_t;


#endif /* CLIENT_IGNORE*/

#endif /* __ADI_APOLLO_COMMON_TYPES_H__ */
/*! @} */
