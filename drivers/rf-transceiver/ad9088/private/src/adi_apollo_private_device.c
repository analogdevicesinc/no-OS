/*!
 * \brief     Private device API implementations
 *
 * \copyright copyright(c) 2024 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_PRIVATE_DEVICE
 * @{
 */

/*============= I N C L U D E S ============*/
#include "adi_apollo_config.h"
#include "adi_apollo_hal.h"
#include "adi_apollo_bmem_types.h"
#include "adi_apollo_cddc_types.h"
#include "adi_apollo_cduc_types.h"
#include "adi_apollo_cnco_types.h"
#include "adi_apollo_fnco_types.h"
#include "adi_apollo_fddc_types.h"
#include "adi_apollo_fduc_types.h"
#include "adi_apollo_invsinc_types.h"
#include "adi_apollo_pfilt_types.h"
#include "adi_apollo_sniffer_types.h"
#include "adi_apollo_smon_types.h"
#include "adi_apollo_txmux_types.h"
#include "adi_apollo_private_device.h"
#include "adi_apollo_bf_hsci.h"
#include "adi_utils.h"

#define SEVEN_CONSECUTIVE_1S_VAL            (0x7FU)     //  0b01111111
#define FIVE_CONSECUTIVE_1S_VAL             (0x1FU)     //  0b00011111
#define THREE_CONSECUTIVE_1S_VAL            (0x07U)     //  0b00000111
#define SEVEN_CONSECUTIVE_1S_INDEX_BOUNDS   3, 8        //  Lower index = 3, Upper Index = 8
#define FIVE_CONSECUTIVE_1S_INDEX_BOUNDS    2, 9        //  Lower index = 2, Upper Index = 9
#define THREE_CONSECUTIVE_1S_INDEX_BOUNDS   1, 10       //  Lower index = 1, Upper Index = 10

/*============= C O D E ====================*/

static int32_t hsci_manual_linkup_enable_set(adi_apollo_device_t *device, uint8_t enable, uint16_t link_up_signal_bits);
static int32_t calc_rxclk_adj_from_eyediagram(adi_apollo_device_t *device, uint16_t eye_diagram, uint8_t *calc_rx_clk_adj);
static int32_t hsci_auto_linkup_enable_set(adi_apollo_device_t *device, uint8_t enable, uint8_t hscim_mosi_clk_inv, uint8_t hscim_miso_clk_inv);
static int32_t calc_rxclk_adj_from_link_table(adi_apollo_device_t *device, uint16_t apollo_alink_table, uint8_t *calc_rx_clk_adj);
static int32_t calc_txclk_adj_from_link_table(adi_apollo_device_t *device, uint16_t hscim_alink_table, uint8_t *calc_tx_clk_adj);
static int32_t apollo_hsci_clk_values_set(adi_apollo_device_t *device, uint8_t rx_inv, uint8_t rx_adj, uint8_t tx_inv, uint8_t tx_adj);
static int32_t apollo_hsci_clk_values_get(adi_apollo_device_t *device, uint8_t *rx_inv, uint8_t *rx_adj, uint8_t *tx_inv, uint8_t *tx_adj);
static int32_t apollo_hsci_auto_link_clk_values_get(adi_apollo_device_t *device, uint8_t *auto_rx_inv, uint8_t *auto_rx_adj, uint8_t *auto_tx_inv, uint8_t *auto_tx_adj);
static int32_t apollo_hsci_reset(adi_apollo_device_t *device, uint8_t do_hard_rst);
static __maybe_unused int32_t apollo_hsci_debug_clk_values_get(adi_apollo_device_t *device);
static __maybe_unused int32_t apollo_hsci_debug_error_reg_get(adi_apollo_device_t *device);


int32_t adi_apollo_private_device_hsci_manual_linkup_configure(adi_apollo_device_t *device)
{
    int32_t err;
    uint16_t link_up_signal_bits = 0x155;
    uint16_t eye_diagram = 0x00;
    uint8_t rxclk_inv = 0;
    uint8_t rxclk_adj = 0;

    ADI_CMS_NULL_PTR_CHECK(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_CMS_NULL_PTR_CHECK(device->hal_info.hsci_desc.manual_linkup);

    // Hard Reset HSCI On DUT
    err = apollo_hsci_reset(device, 1);
    ADI_CMS_ERROR_RETURN(err);

    // Enable HSCI Manual Linkup on Apollo and FPGA
    err = hsci_manual_linkup_enable_set(device, 1, link_up_signal_bits);
    ADI_CMS_ERROR_RETURN(err);

    // Clear Rx CLK Invert.
    err = adi_apollo_hal_bf_set(device, BF_HSCI_RXCLK_INV_INFO, rxclk_inv);
    ADI_APOLLO_ERROR_RETURN(err);

    // Read Positive Eye-diagram
    err = adi_apollo_hal_bf_get(device, BF_HSCI_DIG_EYE_POS_INFO, (uint8_t *)&eye_diagram, 2);
    ADI_CMS_ERROR_RETURN(err);

    // Calculate rxclk_adj value based on Positive Eye Diagram
    err = calc_rxclk_adj_from_eyediagram(device, eye_diagram, &rxclk_adj);
    ADI_CMS_ERROR_RETURN(err);

    // Set Rx CLK ADJ
    err = adi_apollo_hal_bf_set(device, BF_HSCI_RXCLK_ADJ_INFO, rxclk_adj);
    ADI_CMS_ERROR_RETURN(err);

    // Disable HSCI Manual linkup on Apollo and FPGA
    err = hsci_manual_linkup_enable_set(device, 0, 0x00);
    ADI_CMS_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_private_device_hsci_auto_linkup_configure(adi_apollo_device_t *device)
{
    int32_t err;
    uint8_t retry_count = 0;
    uint8_t hscim_mosi_clk_inv = 0;
    uint8_t hscim_miso_clk_inv = 0;
    uint16_t apollo_alink_table = 0;
    uint16_t hscim_alink_table = 0;
    uint8_t auto_rxclk_inv = 0;
    uint8_t auto_txclk_inv = 0;
    uint8_t auto_rxclk_adj = 0;     // Useful for debug only
    uint8_t auto_txclk_adj = 0;     // Useful for debug only
    uint8_t calc_rx_clk_adj = 0;
    uint8_t calc_tx_clk_adj = 0;
    uint8_t rx_calc_err_status = 0;
    uint8_t tx_calc_err_status = 0;
    uint8_t auto_linkup_achieved = 0;

    ADI_CMS_NULL_PTR_CHECK(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_CMS_NULL_PTR_CHECK(device->hal_info.hsci_desc.auto_linkup);
    ADI_CMS_NULL_PTR_CHECK(device->hal_info.hsci_desc.alink_tbl_get);

    // Hard Reset HSCI On DUT
    err = apollo_hsci_reset(device, 1);
    ADI_CMS_ERROR_RETURN(err);

    do {
        apollo_alink_table = 0x00;
        hscim_alink_table = 0x00;

        // Enable HSCI Auto Linkup on Apollo and FPGA
        err = hsci_auto_linkup_enable_set(device, 1, hscim_mosi_clk_inv, hscim_miso_clk_inv);
        ADI_CMS_ERROR_RETURN(err);

        // Get Apollo auto linkup table
        err = adi_apollo_hal_bf_get(device, BF_AUTO_LINK_TABLE_INFO, (uint8_t *) &apollo_alink_table, 2);
        ADI_CMS_ERROR_RETURN(err);

        // Get FPGA auto linkup table
        err = adi_apollo_hal_alink_tbl_get(device, &hscim_alink_table);
        ADI_CMS_ERROR_RETURN(err);

        // Calculate rxclk_adj value based on AUTO_LINK_TABLE
        // If valid rxclk_adj value isn't found, flip the MOSI_CLK_INV Bit and set error flag
        err = calc_rxclk_adj_from_link_table(device, apollo_alink_table, &calc_rx_clk_adj);
        if (err == API_CMS_ERROR_HSCI_LINK_UP) {
            hscim_mosi_clk_inv ^= 0x01;
            hscim_mosi_clk_inv &= 0x01;
            rx_calc_err_status = 1;
        } else {
            rx_calc_err_status = 0;
        }

        // Calculate txclk_adj value based on HSCIM_LINK_TABLE
        // If valid txclk_adj value isn't found, flip the MISO_CLK_INV Bit and set error flag
        err = calc_txclk_adj_from_link_table(device, hscim_alink_table, &calc_tx_clk_adj);
        if (err == API_CMS_ERROR_HSCI_LINK_UP) {
            hscim_miso_clk_inv ^= 0x01;
            hscim_miso_clk_inv &= 0x01;
            tx_calc_err_status = 1;
        } else {
            tx_calc_err_status = 0;
        }

        // If any tx/rxclk_adj value is invalid, disable HSCI auto linkup on Apollo and FPGA
        if ((rx_calc_err_status != 0) || (tx_calc_err_status != 0)) {
            err = hsci_auto_linkup_enable_set(device, 0, hscim_mosi_clk_inv, hscim_miso_clk_inv);
            ADI_CMS_ERROR_RETURN(err);

            auto_linkup_achieved = 0;
            retry_count++;
        } else {
            auto_linkup_achieved = 1;
        }
    } while ((auto_linkup_achieved != 1) && (retry_count < 5));

    if (auto_linkup_achieved == 0x01) {
        ADI_APOLLO_LOG_MSG("Apollo HSCI Auto Link: Established.");
    } else {
        ADI_APOLLO_LOG_MSG("Apollo HSCI Auto Link: *** FAILED ***.");
        ADI_CMS_ERROR_RETURN(API_CMS_ERROR_HSCI_LINK_UP);
    }

    // Readback CLK setting updated during Auto linkup procedure
    apollo_hsci_auto_link_clk_values_get(device, &auto_rxclk_inv, &auto_rxclk_adj, &auto_txclk_inv, &auto_txclk_adj);
    ADI_CMS_ERROR_RETURN(err);

    // Disable HSCI Auto linkup on Apollo and FPGA
    err = hsci_auto_linkup_enable_set(device, 0, hscim_mosi_clk_inv, hscim_miso_clk_inv);
    ADI_CMS_ERROR_RETURN(err);

    // Overwrite HSCI port's clk setting with AUTO Tx/Rx CLK Invert and Calculated Tx/Rx CLK Adjust values.
    err = apollo_hsci_clk_values_set(device, auto_rxclk_inv, calc_rx_clk_adj, auto_txclk_inv, calc_tx_clk_adj);
    ADI_CMS_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

static int32_t hsci_manual_linkup_enable_set(adi_apollo_device_t *device, uint8_t enable, uint16_t link_up_signal_bits)
{
    int32_t err = 0;

    ADI_CMS_NULL_PTR_CHECK(device);
    ADI_APOLLO_INVALID_PARAM_RETURN(enable > 1);

    // Enable HSCI port On Apollo
    err = adi_apollo_hal_bf_set(device, BF_HSCI_EN_INFO, 1);
    ADI_CMS_ERROR_RETURN(err);

    // Enables (or disables) manual linkup procedure in Apollo
    err = adi_apollo_hal_bf_set(device, BF_HSCI_RX_LINKUP_MODE_INFO, enable);
    ADI_CMS_ERROR_RETURN(err);

    // Enables (or disables) eye diagram
    err = adi_apollo_hal_bf_set(device, BF_HSCI_CTRL_RX_EYE_EN_INFO, enable);
    ADI_CMS_ERROR_RETURN(err);

    // Enable link-up on Master and Write Link-up signal on Master
    err = adi_apollo_hal_manual_linkup(device, enable, link_up_signal_bits);
    ADI_CMS_ERROR_RETURN(err);

    err = adi_apollo_hal_delay_us(device, 1000);
    ADI_CMS_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

static int32_t calc_rxclk_adj_from_eyediagram(adi_apollo_device_t *device, uint16_t eye_diagram, uint8_t *calc_rx_clk_adj)
{
    ADI_APOLLO_LOG_VAR(ADI_CMS_LOG_API, "Apollo EYE_DIAGRAM: 0x%04X.", eye_diagram);

    if (!adi_api_utils_check_consecutive_ones_bounded(eye_diagram, FIVE_CONSECUTIVE_1S_VAL, FIVE_CONSECUTIVE_1S_INDEX_BOUNDS, calc_rx_clk_adj)) {
        ADI_APOLLO_LOG_VAR(ADI_CMS_LOG_API, "There are 5 consecutive 1's in the Apollo's Eye-Diagram. Rx_clk_adj centered at index %d.", *calc_rx_clk_adj);
    } else if (!adi_api_utils_check_consecutive_ones_bounded(eye_diagram, THREE_CONSECUTIVE_1S_VAL, THREE_CONSECUTIVE_1S_INDEX_BOUNDS, calc_rx_clk_adj)) {
        ADI_APOLLO_LOG_VAR(ADI_CMS_LOG_API, "There are 3 consecutive 1's in the Apollo's Eye-Diagram. Rx_clk_adj centered at index %d.", *calc_rx_clk_adj);
    } else {
        *calc_rx_clk_adj = 0;   // 0 is an invalid clk_adj value
        ADI_APOLLO_LOG_ERR("There are no consecutive 1's of length 3 or 5, in Apollo's Eye-Diagram.");
        ADI_CMS_ERROR_RETURN(API_CMS_ERROR_HSCI_LINK_UP);
    }

    return API_CMS_ERROR_OK;
}

static int32_t hsci_auto_linkup_enable_set(adi_apollo_device_t *device, uint8_t enable, uint8_t hscim_mosi_clk_inv, uint8_t hscim_miso_clk_inv)
{
    int32_t err = 0;
    uint8_t i = 0;
    uint8_t retry_count = 20;
    uint8_t link_active = 0;

    ADI_CMS_NULL_PTR_CHECK(device);
    ADI_APOLLO_INVALID_PARAM_RETURN(enable > 1);
    ADI_APOLLO_INVALID_PARAM_RETURN(hscim_mosi_clk_inv > 1);
    ADI_APOLLO_INVALID_PARAM_RETURN(hscim_miso_clk_inv > 1);

    // Enables (or disables) txclk_adj value to be overridden during auto linkup
    err = adi_apollo_hal_bf_set(device, BF_TXCLK_ADJ_OVERRIDE_INFO, enable);
    ADI_CMS_ERROR_RETURN(err);

    // Enables (or disables) auto linkup procedure in Apollo
    err = adi_apollo_hal_bf_set(device, BF_HSCI_AUTO_LINKUP_INFO, enable);
    ADI_CMS_ERROR_RETURN(err);

    // Enables HSCI port on Apollo
    err = adi_apollo_hal_bf_set(device, BF_HSCI_EN_INFO, 1);
    ADI_CMS_ERROR_RETURN(err);

    // Enables HSCI on FPGA using HAL func ptr and reset HSCIM if disabled
    err = adi_apollo_hal_auto_linkup(device, enable, hscim_mosi_clk_inv, hscim_miso_clk_inv);
    ADI_CMS_ERROR_RETURN(err);

    // When enabled, check if HSCI Link is established on Apollo
    // And when disabled, soft resets the HSCI port on Apollo
    if (enable) {
        for (i = 0; i < retry_count; ++i) {
            err = adi_apollo_hal_bf_get(device, BF_LINK_ACTIVE_INFO, &link_active, 1);
            ADI_CMS_ERROR_RETURN(err);

            if (link_active == 0x01) {
                ADI_APOLLO_LOG_MSG("Apollo HSCI Auto Link: Active.");
                break;
            } else if ((link_active == 0x00) && (i < (retry_count - 1))) {
                ADI_APOLLO_LOG_MSG("Waiting for link to establish...");
                err = adi_apollo_hal_delay_us(device, 1000);
                ADI_CMS_ERROR_RETURN(err);
            } else {
                ADI_APOLLO_LOG_MSG("Apollo HSCI Auto Link: *** FAILED ***.");
                ADI_CMS_ERROR_RETURN(API_CMS_ERROR_OPERATION_TIMEOUT);
            }
        }
    } else {
        err = apollo_hsci_reset(device, 0);
        ADI_CMS_ERROR_RETURN(err);
    }

    return API_CMS_ERROR_OK;
}


static int32_t calc_rxclk_adj_from_link_table(adi_apollo_device_t *device, uint16_t apollo_alink_table, uint8_t *calc_rx_clk_adj)
{
    ADI_APOLLO_LOG_VAR(ADI_CMS_LOG_API, "Apollo AUTO_LINK_TABLE: 0x%04X.", apollo_alink_table);

    if (!adi_api_utils_check_consecutive_ones_bounded(apollo_alink_table, SEVEN_CONSECUTIVE_1S_VAL, SEVEN_CONSECUTIVE_1S_INDEX_BOUNDS, calc_rx_clk_adj)) {
        ADI_APOLLO_LOG_VAR(ADI_CMS_LOG_API, "There are 7 consecutive 1's in the Apollo's AUTO_LINK_TABLE. Rx_clk_adj centered at index %d.", *calc_rx_clk_adj);
    } else if (!adi_api_utils_check_consecutive_ones_bounded(apollo_alink_table, FIVE_CONSECUTIVE_1S_VAL, FIVE_CONSECUTIVE_1S_INDEX_BOUNDS, calc_rx_clk_adj)) {
        ADI_APOLLO_LOG_VAR(ADI_CMS_LOG_API, "There are 5 consecutive 1's in the Apollo's AUTO_LINK_TABLE. Rx_clk_adj centered at index %d.", *calc_rx_clk_adj);
    } else if (!adi_api_utils_check_consecutive_ones_bounded(apollo_alink_table, THREE_CONSECUTIVE_1S_VAL, THREE_CONSECUTIVE_1S_INDEX_BOUNDS, calc_rx_clk_adj)) {
        ADI_APOLLO_LOG_VAR(ADI_CMS_LOG_API, "There are 3 consecutive 1's in the Apollo's AUTO_LINK_TABLE. Rx_clk_adj centered at index %d.", *calc_rx_clk_adj);
    } else {
        *calc_rx_clk_adj = 0;   // 0 is an invalid clk_adj value
        ADI_APOLLO_LOG_ERR("There are no consecutive 1's of length 3, 5 or 7, in Apollo's AUTO_LINK_TABLE.");
        ADI_CMS_ERROR_RETURN(API_CMS_ERROR_HSCI_LINK_UP);
    }

    return API_CMS_ERROR_OK;
}

static int32_t calc_txclk_adj_from_link_table(adi_apollo_device_t *device, uint16_t hscim_alink_table, uint8_t *calc_tx_clk_adj)
{
    ADI_APOLLO_LOG_VAR(ADI_CMS_LOG_API, "FPGA HSCI_M ALINK_TABLE: 0x%04X.", hscim_alink_table);

    if (!adi_api_utils_check_consecutive_ones_bounded(hscim_alink_table, SEVEN_CONSECUTIVE_1S_VAL, SEVEN_CONSECUTIVE_1S_INDEX_BOUNDS, calc_tx_clk_adj)) {
        ADI_APOLLO_LOG_VAR(ADI_CMS_LOG_API, "There are 7 consecutive 1's in the FPGA's ALINK_TABLE. Tx_clk_adj centered at index %d.", *calc_tx_clk_adj);
    } else if (!adi_api_utils_check_consecutive_ones_bounded(hscim_alink_table, FIVE_CONSECUTIVE_1S_VAL, FIVE_CONSECUTIVE_1S_INDEX_BOUNDS, calc_tx_clk_adj)) {
        ADI_APOLLO_LOG_VAR(ADI_CMS_LOG_API, "There are 5 consecutive 1's in the FPGA's ALINK_TABLE. Tx_clk_adj centered at index %d.", *calc_tx_clk_adj);
    } else if (!adi_api_utils_check_consecutive_ones_bounded(hscim_alink_table, THREE_CONSECUTIVE_1S_VAL, THREE_CONSECUTIVE_1S_INDEX_BOUNDS, calc_tx_clk_adj)) {
        ADI_APOLLO_LOG_VAR(ADI_CMS_LOG_API, "There are 3 consecutive 1's in the FPGA's ALINK_TABLE. Tx_clk_adj centered at index %d.", *calc_tx_clk_adj);
    } else {
        *calc_tx_clk_adj = 0;   // 0 is an invalid clk_adj value
        ADI_APOLLO_LOG_ERR("There are no consecutive 1's of length 3, 5 or 7, in FPGA's ALINK_TABLE.");
        ADI_CMS_ERROR_RETURN(API_CMS_ERROR_HSCI_LINK_UP);
    }

    return API_CMS_ERROR_OK;
}


static int32_t apollo_hsci_clk_values_set(adi_apollo_device_t *device, uint8_t rx_inv, uint8_t rx_adj, uint8_t tx_inv, uint8_t tx_adj)
{
    int32_t err = 0;

    ADI_CMS_NULL_PTR_CHECK(device);
    ADI_APOLLO_INVALID_PARAM_RETURN(rx_inv > 1);
    ADI_APOLLO_INVALID_PARAM_RETURN(rx_adj > 15);
    ADI_APOLLO_INVALID_PARAM_RETURN(tx_inv > 1);
    ADI_APOLLO_INVALID_PARAM_RETURN(tx_adj > 15);

    // Set Rx CLK Invert.
    err = adi_apollo_hal_bf_set(device, BF_HSCI_RXCLK_INV_INFO, rx_inv);
    ADI_CMS_ERROR_RETURN(err);

    // Set Rx CLK ADJ
    err = adi_apollo_hal_bf_set(device, BF_HSCI_RXCLK_ADJ_INFO, rx_adj);
    ADI_CMS_ERROR_RETURN(err);

    // Set Tx CLK Invert.
    err = adi_apollo_hal_bf_set(device, BF_HSCI_TXCLK_INV_INFO, tx_inv);
    ADI_CMS_ERROR_RETURN(err);

    // Set CLK ADJ
    err = adi_apollo_hal_bf_set(device, BF_HSCI_TXCLK_ADJ_INFO, tx_adj);
    ADI_CMS_ERROR_RETURN(err);

    ADI_APOLLO_LOG_VAR(ADI_CMS_LOG_API, "Set HSCI_  RXCLK_INV: %02d  RXCLK_ADJ: %02d  TXCLK_INV: %02d  TXCLK_ADJ: %02d.", rx_inv, rx_adj, tx_inv, tx_adj);

    return API_CMS_ERROR_OK;
}

static int32_t apollo_hsci_clk_values_get(adi_apollo_device_t *device, uint8_t *rx_inv, uint8_t *rx_adj, uint8_t *tx_inv, uint8_t *tx_adj)
{
    int32_t err = 0;

    ADI_CMS_NULL_PTR_CHECK(device);
    ADI_CMS_NULL_PTR_CHECK(rx_inv);
    ADI_CMS_NULL_PTR_CHECK(rx_adj);
    ADI_CMS_NULL_PTR_CHECK(tx_inv);
    ADI_CMS_NULL_PTR_CHECK(tx_adj);

    // Get HSCI RxCLK Invert
    err = adi_apollo_hal_bf_get(device, BF_HSCI_RXCLK_INV_INFO, rx_inv, 1);
    ADI_CMS_ERROR_RETURN(err);

    // Get HSCI RxCLK ADJ
    err = adi_apollo_hal_bf_get(device, BF_HSCI_RXCLK_ADJ_INFO, rx_adj, 1);
    ADI_CMS_ERROR_RETURN(err);

    // Get HSCI TxCLK Invert
    err = adi_apollo_hal_bf_get(device, BF_HSCI_TXCLK_INV_INFO, tx_inv, 1);
    ADI_CMS_ERROR_RETURN(err);

    // Get HSCI TxCLK ADJ
    err = adi_apollo_hal_bf_get(device, BF_HSCI_TXCLK_ADJ_INFO, tx_adj, 1);
    ADI_CMS_ERROR_RETURN(err);

    ADI_APOLLO_LOG_VAR(ADI_CMS_LOG_API, "Get HSCI_  RXCLK_INV: %02d  RXCLK_ADJ: %02d  TXCLK_INV: %02d  TXCLK_ADJ: %02d.",
                       *rx_inv, *rx_adj, *tx_inv, *tx_adj);

    return API_CMS_ERROR_OK;
}


static int32_t apollo_hsci_auto_link_clk_values_get(adi_apollo_device_t *device, uint8_t *auto_rx_inv, uint8_t *auto_rx_adj, uint8_t *auto_tx_inv, uint8_t *auto_tx_adj)
{
    int32_t err = 0;

    ADI_CMS_NULL_PTR_CHECK(device);
    ADI_CMS_NULL_PTR_CHECK(auto_rx_inv);
    ADI_CMS_NULL_PTR_CHECK(auto_rx_adj);
    ADI_CMS_NULL_PTR_CHECK(auto_tx_inv);
    ADI_CMS_NULL_PTR_CHECK(auto_tx_adj);

    // Get AUTO RxCLK Invert
    err = adi_apollo_hal_bf_get(device, BF_AUTO_RXCLK_INV_INFO, auto_rx_inv, 1);
    ADI_CMS_ERROR_RETURN(err);

    // Get AUTO RxCLK ADJ
    err = adi_apollo_hal_bf_get(device, BF_AUTO_RXCLK_ADJ_INFO, auto_rx_adj, 1);
    ADI_CMS_ERROR_RETURN(err);

    // Get AUTO TxCLK Invert
    err = adi_apollo_hal_bf_get(device, BF_AUTO_TXCLK_INV_INFO, auto_tx_inv, 1);
    ADI_CMS_ERROR_RETURN(err);

    // Get AUTO TxCLK ADJ
    err = adi_apollo_hal_bf_get(device, BF_AUTO_TXCLK_ADJ_INFO, auto_tx_adj, 1);
    ADI_CMS_ERROR_RETURN(err);

    ADI_APOLLO_LOG_VAR(ADI_CMS_LOG_API, "Get AUTO_  RXCLK_INV: %02d  RXCLK_ADJ: %02d  TXCLK_INV: %02d  TXCLK_ADJ: %02d.",
                       *auto_rx_inv, *auto_rx_adj, *auto_tx_inv, *auto_tx_adj);

    return API_CMS_ERROR_OK;
}


static int32_t apollo_hsci_reset(adi_apollo_device_t *device, uint8_t do_hard_rst)
{
    int32_t err = 0;
    uint32_t reg = REG_MAIN_CTRL_ADDR;
    uint32_t info = 0;

    ADI_CMS_NULL_PTR_CHECK(device);
    ADI_APOLLO_INVALID_PARAM_RETURN(do_hard_rst > 1);

    info = (do_hard_rst == 1) ? BF_INFO_EXTRACT(BF_HSCI_HARD_RESET_INFO) : BF_INFO_EXTRACT(BF_HSCI_SOFT_RESET_INFO);

    err = adi_apollo_hal_bf_set(device, reg, info, 1);
    ADI_CMS_ERROR_RETURN(err);

    err = adi_apollo_hal_delay_us(device, 1000);
    ADI_CMS_ERROR_RETURN(err);

    err = adi_apollo_hal_bf_set(device, reg, info, 0);
    ADI_CMS_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

static int32_t apollo_hsci_debug_clk_values_get(adi_apollo_device_t *device)
{
    int32_t err = 0;
    uint8_t rx_inv, rx_adj = 0;
    uint8_t tx_inv, tx_adj = 0;

    ADI_CMS_NULL_PTR_CHECK(device);

    // Readback CLK setting used by HSCI Port
    err = apollo_hsci_clk_values_get(device, &rx_inv, &rx_adj, &tx_inv, &tx_adj);
    ADI_CMS_ERROR_RETURN(err);

    // Readback CLK setting updated during Auto linkup procedure
    err = apollo_hsci_auto_link_clk_values_get(device, &rx_inv, &rx_adj, &tx_inv, &tx_adj);
    ADI_CMS_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

static int32_t apollo_hsci_debug_error_reg_get(adi_apollo_device_t *device)
{
    int32_t err = 0;
    uint8_t reg_read = 0;

    ADI_CMS_NULL_PTR_CHECK(device);

    // ERROR_STATUS
    err = adi_apollo_hal_reg_get(device, REG_ERROR_STATUS_ADDR, &reg_read);
    ADI_CMS_ERROR_RETURN(err);
    ADI_APOLLO_LOG_VAR(ADI_CMS_LOG_API, "ERROR_STATUS       (0x%08X): 0x%02X.", REG_ERROR_STATUS_ADDR, reg_read);

    // ERROR_FLAG_DIS
    err = adi_apollo_hal_reg_get(device, REG_ERROR_FLAG_DIS_ADDR, &reg_read);
    ADI_CMS_ERROR_RETURN(err);
    ADI_APOLLO_LOG_VAR(ADI_CMS_LOG_API, "ERROR_FLAG_DIS     (0x%08X): 0x%02X.", REG_ERROR_FLAG_DIS_ADDR, reg_read);

    // FSM_STATUS
    err = adi_apollo_hal_reg_get(device, REG_FSM_STATUS_ADDR, &reg_read);
    ADI_CMS_ERROR_RETURN(err);
    ADI_APOLLO_LOG_VAR(ADI_CMS_LOG_API, "FSM_STATUS         (0x%08X): 0x%02X.", REG_FSM_STATUS_ADDR, reg_read);

    // AHB_FSM_STATUS
    err = adi_apollo_hal_reg_get(device, REG_AHB_FSM_STATUS_ADDR, &reg_read);
    ADI_CMS_ERROR_RETURN(err);
    ADI_APOLLO_LOG_VAR(ADI_CMS_LOG_API, "AHB_FSM_STATUS     (0x%08X): 0x%02X.", REG_AHB_FSM_STATUS_ADDR, reg_read);

    // AUTO_LINK_TABLE_LO
    err = adi_apollo_hal_reg_get(device, REG_AUTO_LINK_TABLE_LO_ADDR, &reg_read);
    ADI_CMS_ERROR_RETURN(err);
    ADI_APOLLO_LOG_VAR(ADI_CMS_LOG_API, "AUTO_LINK_TABLE_LO (0x%08X): 0x%02X.", REG_AUTO_LINK_TABLE_LO_ADDR, reg_read);

    // AUTO_LINK_TABLE_HI
    err = adi_apollo_hal_reg_get(device, REG_AUTO_LINK_TABLE_HI_ADDR, &reg_read);
    ADI_CMS_ERROR_RETURN(err);
    ADI_APOLLO_LOG_VAR(ADI_CMS_LOG_API, "AUTO_LINK_TABLE_HI (0x%08X): 0x%02X.", REG_AUTO_LINK_TABLE_HI_ADDR, reg_read);

    // HSCI_ERR_INFO
    err = adi_apollo_hal_reg_get(device, REG_HSCI_ERR_INFO_ADDR, &reg_read);
    ADI_CMS_ERROR_RETURN(err);
    ADI_APOLLO_LOG_VAR(ADI_CMS_LOG_API, "HSCI_ERR_INFO      (0x%08X): 0x%02X.", REG_HSCI_ERR_INFO_ADDR, reg_read);

    return API_CMS_ERROR_OK;
}


uint8_t adi_apollo_private_device_lockout_get(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_private_device_feat_lockout_e feat)
{
    return (terminal == ADI_APOLLO_RX ? 
           (device->dev_info.lockout_mask.rx_lockout_mask & (1ul << feat)) : 
           (device->dev_info.lockout_mask.tx_lockout_mask & (1ul << feat))) != 0;
}

/*! @} */
