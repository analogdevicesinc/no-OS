/*!
 * \brief     TXEN Block definition headers
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_TXEN
 * @{
 */
#ifndef __ADI_APOLLO_TXEN_TYPES_H__
#define __ADI_APOLLO_TXEN_TYPES_H__

/*============= I N C L U D E S ============*/
#include "adi_cms_api_common.h"
#include "adi_apollo_common.h"

/*============= D E F I N E S ==============*/

#define ADI_APOLLO_NUM_TX_EN 8              /* Num of TX_EN pins */
#define ADI_APOLLO_NUM_TX_EN_PER_SIDE 4     /* Num of TX_EN pins per side */
#define ADI_APOLLO_NUM_TX_GAIN_PER_SIDE         4
#define ADI_APOLLO_NUM_TX_SRD_PER_SIDE          4
#define ADI_APOLLO_NUM_TX_INVSINC_PER_SIDE      4

/*!
 * \brief Enumerates DAC Select
 */
typedef enum {
    ADI_APOLLO_TXEN_DAC_NONE = 0x0,                              /*!< No DAC */
    ADI_APOLLO_TXEN_DAC_A0 = 0x1,                                /*!< DAC0 of SIDE A */
    ADI_APOLLO_TXEN_DAC_A1 = 0x2,                                /*!< DAC1 of SIDE A */
    ADI_APOLLO_TXEN_DAC_A2 = 0x4,                                /*!< DAC2 of SIDE A */
    ADI_APOLLO_TXEN_DAC_A3 = 0x8,                                /*!< DAC3 of SIDE A */
    ADI_APOLLO_TXEN_DAC_B0 = 0x10,                               /*!< DAC0 of SIDE B */
    ADI_APOLLO_TXEN_DAC_B1 = 0x20,                               /*!< DAC1 of SIDE B */
    ADI_APOLLO_TXEN_DAC_B2 = 0x40,                               /*!< DAC2 of SIDE B */
    ADI_APOLLO_TXEN_DAC_B3 = 0x80,                               /*!< DAC3 of SIDE B */
    ADI_APOLLO_TXEN_DAC_ALL = 0xFF                               /*!< ALL DACs */
} adi_apollo_txen_dac_select_e;

/*!
* \brief Tx enable pin polarity select
*/
typedef enum {
    ADI_APOLLO_TXEN_EN_ACTIVE_HIGH = 0,            /*!< Polarity of respective Tx en is active high */
    ADI_APOLLO_TXEN_EN_ACTIVE_LOW  = 1,            /*!< Polarity of respective Tx en is active low */
} adi_apollo_txen_en_polarity_e;

/*!
* \brief Enumerates TX_EN pin select
*/
typedef enum {
    ADI_APOLLO_TX_EN_NONE = 0x00,                      /*!< No TX_EN pins */
    ADI_APOLLO_TX_EN_A0   = 0x01,                      /*!< TX_EN pin 0 of SIDE A */
    ADI_APOLLO_TX_EN_A1   = 0x02,                      /*!< TX_EN pin 1 of SIDE A */
    ADI_APOLLO_TX_EN_A2   = 0x04,                      /*!< TX_EN pin 2 of SIDE A */
    ADI_APOLLO_TX_EN_A3   = 0x08,                      /*!< TX_EN pin 3 of SIDE A */
    ADI_APOLLO_TX_EN_B0   = 0x10,                      /*!< TX_EN pin 0 of SIDE B */
    ADI_APOLLO_TX_EN_B1   = 0x20,                      /*!< TX_EN pin 1 of SIDE B */
    ADI_APOLLO_TX_EN_B2   = 0x40,                      /*!< TX_EN pin 2 of SIDE B */
    ADI_APOLLO_TX_EN_B3   = 0x80,                      /*!< TX_EN pin 3 of SIDE B */
    ADI_APOLLO_TX_EN_ALL  = 0xFF,                      /*!< All TX_EN pins */
} adi_apollo_txen_en_select_e;

/*!
 * \brief TxEN Block Config
 */
typedef struct {
    adi_apollo_txen_en_polarity_e enable_polarity;
    uint8_t spi_en;
    uint8_t spien_en;
    uint16_t slice_sel;
    uint8_t linx_sel;
    uint16_t pfilt_sel;
    uint16_t cduc_sel;
    uint16_t fduc_sel;
    uint8_t cfir_sel;
    uint8_t fsrc_sel;
    uint16_t jrx_link_sel;
    uint16_t jrx_phy_sel_side_a;                    /*!< JRX phy lane bit-wise select side a */
    uint16_t jrx_phy_sel_side_b;                    /*!< JRX phy lane bit-wise select side b */
    uint8_t modsw_sel;
    uint8_t invsinc_sel;
    uint8_t gain_sel;
    uint8_t srd_sel;

} adi_apollo_txen_blk_config_t;

/*!
* \brief TxEN powerup control SPI
*/
typedef struct {
    uint8_t     sm_en;                          /*!< 0: State machine bypassed. (dac_stdby, dig_en and pa_en follow TxEN), 1: Edge control state machine enabled. */
    uint8_t     spi_txen_en;                    /*!< 0: Use the TxEN input signal, 1: Ignore TxEN input signal and use input from SPI control */
    uint8_t     spi_txen;                       /*!< 0: Tx blocks gated off/standby, 1:TRX blocks are enabled (spi_txen_en must be enabled to use this control) */
    adi_apollo_puc_clk_rate_sel_e sm_clk_rate;  /*!< Select clock rate of state machine counters. */
} adi_apollo_txen_pwrup_ctrl_t;

/*!
* \brief TxEN powerup control edges
*/
typedef struct {
    adi_apollo_puc_edge_rise_sel_e  dac_rise;   /*!< dac enable to rise after TxEN rise. (\ref adi_apollo_puc_edge_rise_sel_e) */
    adi_apollo_puc_edge_fall_sel_e  dac_fall;   /*!< dac enable to fall after TxEN fall. (\ref adi_apollo_puc_edge_fall_sel_e) */
    adi_apollo_puc_edge_rise_sel_e  dig_rise;   /*!< dig block enable to rise after TxEN rise. (\ref adi_apollo_puc_edge_rise_sel_e) */
    adi_apollo_puc_edge_fall_sel_e  dig_fall;   /*!< dig block enable to fall after TxEN fall. (\ref adi_apollo_puc_edge_fall_sel_e) */
    adi_apollo_puc_edge_rise_sel_e  pa_rise;    /*!< PA enable to rise after TxEN rise. (\ref adi_apollo_puc_edge_rise_sel_e) */
    adi_apollo_puc_edge_fall_sel_e  pa_fall;    /*!< PA enable to fall after TxEN fall. (\ref adi_apollo_puc_edge_fall_sel_e) */
} adi_apollo_txen_pwrup_ctrl_edge_t;

/*!
* \brief TxEN powerup control counters
*/
typedef struct {
    uint8_t     count_maxa;                     /*!< Maximum programmed counter value from E0 to E1 (Fs/32 per count) */
    uint8_t     count_maxb;                     /*!< Maximum programmed counter value from E1 to E2 (Fs/32 per count) */
    uint8_t     count_maxc;                     /*!< Maximum programmed counter value from E2 to E3 (Fs/32 per count) */
    uint8_t     count_maxd;                     /*!< Maximum programmed counter value from E4 to E5 (Fs/32 per count) */
    uint8_t     count_maxe;                     /*!< Maximum programmed counter value from E5 to E6 (Fs/32 per count) */
    uint8_t     count_maxf;                     /*!< Maximum programmed counter value from E6 to E7 (Fs/32 per count) */
} adi_apollo_txen_pwrup_ctrl_count_t;

/*!
* \brief TxEN powerup control block configuration
*/
typedef struct {
    adi_apollo_txen_pwrup_ctrl_t ctrl;
    adi_apollo_txen_pwrup_ctrl_edge_t edge;
    adi_apollo_txen_pwrup_ctrl_count_t count;
    adi_apollo_puc_en_pin_sel_e pin_en;         /*!< Select which enable pin is mapped to the pwrup control. */
} adi_apollo_txen_pwrup_blk_config_t;

#endif /* __ADI_APOLLO_TXEN_TYPES_H__ */
/*! @} */
