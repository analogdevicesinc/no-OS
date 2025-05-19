/*!
 * \brief     RXEN Block definition type headers
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_RXEN
 * @{
 */
#ifndef __ADI_APOLLO_RXEN_TYPES_H__
#define __ADI_APOLLO_RXEN_TYPES_H__

/*============= I N C L U D E S ============*/
#include "adi_cms_api_common.h"
#include "adi_apollo_common.h"

/*============= D E F I N E S ==============*/
#define ADI_APOLLO_DIG_EN_NUM 8              /* Num of dig enables from TX/RX enable pwr control */
#define ADI_APOLLO_DIG_EN_PER_SIDE_NUM 4     /* Num of dig enables from TX/RX enable pwr control per side */

/*!
 * \brief Enumerates ADC Select
 */
typedef enum {
    ADI_APOLLO_RXEN_ADC_NONE = 0x0,                 /*!< No ADC */
    ADI_APOLLO_RXEN_ADC_A0 = 0x1,                   /*!< ADC0 of side A */
    ADI_APOLLO_RXEN_ADC_A1 = 0x2,                   /*!< ADC1 of side A */
    ADI_APOLLO_RXEN_ADC_A2 = 0x4,                   /*!< ADC2 of side A */
    ADI_APOLLO_RXEN_ADC_A3 = 0x8,                   /*!< ADC3 of side A */
    ADI_APOLLO_RXEN_ADC_B0 = 0x10,                  /*!< ADC0 of side B */
    ADI_APOLLO_RXEN_ADC_B1 = 0x20,                  /*!< ADC1 of side B */
    ADI_APOLLO_RXEN_ADC_B2 = 0x40,                  /*!< ADC2 of side B */
    ADI_APOLLO_RXEN_ADC_B3 = 0x80,                  /*!< ADC3 of side B */
    ADI_APOLLO_RXEN_ADC_ALL = 0xFF                  /*!< ALL ADCs */
} adi_apollo_rxen_adc_select_e;

/*!
* \brief Dig enable select from powerup control block
*/
typedef enum {
    ADI_APOLLO_RXEN_EN_NONE = 0,                 /*!< No dig enable pins */
    ADI_APOLLO_RXEN_EN_A0 = 0x01,                /*!< Digital enable A0 from pwr control input */
    ADI_APOLLO_RXEN_EN_A1 = 0x02,                /*!< Digital enable A1 from pwr control input */
    ADI_APOLLO_RXEN_EN_A2 = 0x04,                /*!< Digital enable A2 from pwr control input */
    ADI_APOLLO_RXEN_EN_A3 = 0x08,                /*!< Digital enable A3 from pwr control input */
    ADI_APOLLO_RXEN_EN_B0 = 0x10,                /*!< Digital enable B0 from pwr control input */
    ADI_APOLLO_RXEN_EN_B1 = 0x20,                /*!< Digital enable B1 from pwr control input */
    ADI_APOLLO_RXEN_EN_B2 = 0x40,                /*!< Digital enable B2 from pwr control input */
    ADI_APOLLO_RXEN_EN_B3 = 0x80,                /*!< Digital enable B3 from pwr control input */
    ADI_APOLLO_RXEN_EN_ALL = 0xFF,               /*!< All dig enable pins */
} adi_apollo_rxen_en_select_e;

/*!
* \brief Dig enable from powerup is active polarity select
*/
typedef enum {
    ADI_APOLLO_DIG_EN_ACTIVE_HIGH = 0,          /*!< Polarity of respective dig en is active high */
    ADI_APOLLO_DIG_EN_ACTIVE_LOW = 1,           /*!< Polarity of respective dig en is active low */
} adi_apollo_rxen_en_polarity_e;


/*!
* \brief RxEN powerup control SPI
*/
typedef struct {
    uint8_t     sm_en;                          /*!< 0: State machine bypassed. (dac_stdby, dig_en and pa_en follow RxEN), 1: Edge control state machine enabled. */
    uint8_t     spi_rxen_en;                    /*!< 0: Use the RxEN input signal, 1: Ignore RxEN input signal and use input from SPI control */
    uint8_t     spi_rxen;                       /*!< 0: Rx blocks gated off/standby, 1: RX blocks are enabled (spi_rxen_en must be enabled to use this control) */
    adi_apollo_puc_clk_rate_sel_e sm_clk_rate;  /*!< Select clock rate of state machine counters. */
} adi_apollo_rxen_pwrup_ctrl_t;

/*!
* \brief RxEN powerup control edges
*/
typedef struct {
    adi_apollo_puc_edge_rise_sel_e  adc_rise;   /*!< adc enable to rise after RxEN rise. (\ref adi_apollo_puc_edge_rise_sel_e) */
    adi_apollo_puc_edge_fall_sel_e  adc_fall;   /*!< adc enable to fall after RxEN fall. (\ref adi_apollo_puc_edge_fall_sel_e) */
    adi_apollo_puc_edge_rise_sel_e  dig_rise;   /*!< dig block enable to rise after RxEN rise. (\ref adi_apollo_puc_edge_rise_sel_e) */
    adi_apollo_puc_edge_fall_sel_e  dig_fall;   /*!< dig block enable to fall after RxEN fall. (\ref adi_apollo_puc_edge_fall_sel_e) */
    adi_apollo_puc_edge_rise_sel_e  pa_rise;    /*!< PA enable to rise after RxEN rise. (\ref adi_apollo_puc_edge_rise_sel_e) */
    adi_apollo_puc_edge_fall_sel_e  pa_fall;    /*!< PA enable to fall after RxEN fall. (\ref adi_apollo_puc_edge_fall_sel_e) */
} adi_apollo_rxen_pwrup_ctrl_edge_t;

/*!
* \brief RxEN powerup control counters
*/
typedef struct {
    uint8_t     count_maxa;                     /*!< Maximum programmed counter value from E0 to E1 (Fs/32 per count) */
    uint8_t     count_maxb;                     /*!< Maximum programmed counter value from E1 to E2 (Fs/32 per count) */
    uint8_t     count_maxc;                     /*!< Maximum programmed counter value from E2 to E3 (Fs/32 per count) */
    uint8_t     count_maxd;                     /*!< Maximum programmed counter value from E4 to E5 (Fs/32 per count) */
    uint8_t     count_maxe;                     /*!< Maximum programmed counter value from E5 to E6 (Fs/32 per count) */
    uint8_t     count_maxf;                     /*!< Maximum programmed counter value from E6 to E7 (Fs/32 per count) */
} adi_apollo_rxen_pwrup_ctrl_count_t;

/*!
* \brief RxEN powerup control block configuration
*/
typedef struct {
    adi_apollo_rxen_pwrup_ctrl_t ctrl;
    adi_apollo_rxen_pwrup_ctrl_edge_t edge;
    adi_apollo_rxen_pwrup_ctrl_count_t count;
    adi_apollo_puc_en_pin_sel_e pin_en;         /*!< Select which enable pin is mapped to the pwrup control. */
} adi_apollo_rxen_pwrup_blk_config_t;

/*!
* \brief RxEN control SPI
*/
typedef struct {
    adi_apollo_rxen_en_polarity_e enable_polarity;   /*!< Enable input polarity */
    uint8_t spien_en;                               /*!< 0: Use the dig input signal, 1: Ignore input signal and use SPI control */
    uint8_t spi_en;                                 /*!< SPI Enable/Disable. Applicable when spien_en set true */
} adi_apollo_rxen_ctrl_t;

typedef struct {
    adi_apollo_rxen_en_polarity_e enable_polarity;
    uint8_t spi_en;
    uint8_t spien_en;
    uint16_t slice_sel;
    uint8_t linx_sel;
    uint16_t pfilt_sel;
    uint16_t cddc_sel;
    uint16_t fddc_sel;
    uint8_t cfir_sel;
    uint8_t fsrc_sel;
    uint16_t jtx_link_sel;
    uint16_t jtx_phy_sel_side_a;                    /*!< JTX phy lane bit-wise select side a */
    uint16_t jtx_phy_sel_side_b;                    /*!< JTX phy lane bit-wise select side b */
    uint8_t modsw_sel;
    uint8_t invsinc_sel;
    uint8_t gain_sel;
    uint8_t srd_sel;
} adi_apollo_rxen_blk_config_t;

#endif /* __ADI_APOLLO_RXEN_TYPES_H__ */
/*! @} */
