/*!
 * \brief     Clock and multi chip sync related API prototypes
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_CLK
 * @{
 */
#ifndef __ADI_APOLLO_CLK_MCS_H__
#define __ADI_APOLLO_CLK_MCS_H__

/*============= I N C L U D E S ============*/
#include "adi_apollo_clk_mcs_types.h"

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Configures the ADC/RX data fifo
 *
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure
 * \param[in] adcs          ADC selection mask \ref adi_apollo_adc_select_e
 * \param[in] fifo_config   Fifo configuration \ref adi_apollo_data_fifo_config_t
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_clk_mcs_rx_data_fifo_configure(adi_apollo_device_t *device, const uint16_t adcs, adi_apollo_data_fifo_config_t *fifo_config);

/**
 * \brief Enables the ADC/RX data fifo
 *
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure
 * \param[in] adcs          ADC selection mask \ref adi_apollo_adc_select_e
 * \param[in] enable        Fifo enable 1 = enable, 0 = disable
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_clk_mcs_rx_data_fifo_enable(adi_apollo_device_t *device, const uint16_t adcs, uint8_t enable);

/**
 * \brief Writes the ADC/RX data fifo wr/rd fifo offsets
 *
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure
 * \param[in] adcs          ADC selection mask \ref adi_apollo_adc_select_e
 * \param[in] wr_offset     Write fifo offset
 * \param[in] rd_offset     Read fifo offset
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_clk_mcs_rx_data_fifo_offset_set(adi_apollo_device_t *device, const uint16_t adcs, uint8_t wr_offset, uint8_t rd_offset);

/**
 * \brief  Configures ADC/RX data fifo mode
 *
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure
 * \param[in] adcs          ADC selection mask \ref adi_apollo_adc_select_e
 * \param[in] mode          Fifo mode Deterministic Latency mode typical. \ref adi_apollo_data_fifo_mode_e
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_clk_mcs_rx_data_fifo_mode_set(adi_apollo_device_t *device, const uint16_t adcs, adi_apollo_data_fifo_mode_e mode);

/**
 * \brief Configures the DAC/TX data fifo
 *
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure
 * \param[in] dacs          DAC selection mask \ref adi_apollo_dac_select_e
 * \param[in] fifo_config   Fifo configuration \ref adi_apollo_data_fifo_config_t
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_clk_mcs_tx_data_fifo_configure(adi_apollo_device_t *device, const uint16_t dacs, adi_apollo_data_fifo_config_t *fifo_config);

/**
 * \brief  Enables the DAC/TX data fifo
 *
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure
 * \param[in] dacs          DAC selection mask \ref adi_apollo_dac_select_e
 * \param[in] enable        Fifo enable 1 = enable, 0 = disable
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_clk_mcs_tx_data_fifo_enable(adi_apollo_device_t *device, const uint16_t dacs, uint8_t enable);

/**
 * \brief Writes the DAC/TX data fifo wr/rd fifo offsets
 *
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure
 * \param[in] dacs          DAC selection mask \ref adi_apollo_dac_select_e
 * \param[in] wr_offset     Write fifo offset
 * \param[in] rd_offset     Read fifo offset
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_clk_mcs_tx_data_fifo_offset_set(adi_apollo_device_t *device, const uint16_t dacs, uint8_t wr_offset, uint8_t rd_offset);

/**
 * \brief Configures DAC/TX data fifo mode
 *
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure
 * \param[in] dacs          DAC selection mask \ref adi_apollo_dac_select_e
 * \param[in] mode          Fifo mode Deterministic Latency mode typical. \ref adi_apollo_data_fifo_mode_e
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_clk_mcs_tx_data_fifo_mode_set(adi_apollo_device_t *device, const uint16_t dacs, adi_apollo_data_fifo_mode_e mode);

/**
 * \brief Align internal to external sysref and issue system sync
 *
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_clk_mcs_sync_hw_align_set(adi_apollo_device_t *device);

/**
 * \brief Check if oneshot sync is complete.
 *
 * \note Called in response to \ref adi_apollo_clk_mcs_sync_hw_align_set
 *
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure
 * \param[out] status       Sync complete if bit = 1. For single clock, b0 = center status. For dual clock, b0 = A side, b1 = B side.
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_clk_mcs_sync_hw_align_get(adi_apollo_device_t *device, uint8_t *status);

/**
 * \brief Issue system sync w/o internal sysref alignment
 *
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_clk_mcs_sync_only_set(adi_apollo_device_t *device);

/**
 * \brief Check if sync only is complete.
 *
 * \note Called in response to \ref adi_apollo_clk_mcs_sync_only_set
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[out] status       Sync complete if bit = 1. For single clock, b0 = center status. For dual clock, b0 = A side, b1 = B side.
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_clk_mcs_sync_only_get(adi_apollo_device_t *device, uint8_t *status);

/**
 * \brief Enable or disable the use of triggers for internal clock resets or digital configurations
 *
 * \note When using trigger to reset datapath clocks or issue dynamic reconfiguration
 *
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure
 * \param[in] enable        1 to enable, 0 to disable
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_clk_mcs_trig_sync_enable(adi_apollo_device_t *device, uint8_t enable);

/**
 * \brief Get trigger active state for internal clock resets / dynamic reconfig
 *
 * \note When using trigger to reset datapath clocks or issue dynamic reconfiguration
 *
 * \param[in]  device        Context variable - Pointer to the APOLLO device data structure
 * \param[out] enable        1 to enable, 0 to disable
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_clk_mcs_trig_sync_enable_get(adi_apollo_device_t *device, uint8_t *enable);

/**
 * \brief Enable the resets for Rx/Tx datapath clocks when trigger occurs
 *
 * \note After using any trigger based reset, run this function to clear any enables set for trigger.
 *
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_clk_mcs_trig_reset_dsp_enable(adi_apollo_device_t *device);

/**
 * \brief Enable the resets for Serdes conv_clk and lmfc/lemc when trigger occurs
 *
 * \note After using any trigger based reset, run this function to clear any enables set for trigger.
 *
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_clk_mcs_trig_reset_serdes_enable(adi_apollo_device_t *device);

/**
 * \brief Dsiable the resets for all clocks by trigger
 *
 * \note After using any trigger based reset, run this function to clear any enables set for trigger.
 *
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_clk_mcs_trig_reset_disable(adi_apollo_device_t *device);

/**
 * \brief Read back the phase value of input trigger wrt the internal SYSREF.
 *
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure
 * \param[in] trig			Index of trigger (ADI_APOLLO_TRIG_PIN_A0, _A1, _B0, _B1) \ref adi_apollo_trig_pin_e
 * \param[out] phase_0      A side phase val for dual clock mode, else center phase val
 * \param[out] phase_1      B side phase val for dual clock, else 0 for single
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_clk_mcs_trig_phase_get(adi_apollo_device_t *device, adi_apollo_trig_pin_e trig, uint16_t *phase_0, uint16_t *phase_1);

/**
 * \brief Datapath trigger reset masks.
 *
 * \note For example, to reset only RxA and TxB from a trigger, set rxtx_mask = ADI_APOLLO_SYNC_MASK_RX_DIG_A | ADI_APOLLO_SYNC_MASK_TX_DIG_B
 *
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure
 * \param[in] rxtx_mask     Reset disable mask for datapaths. A selected path will ignore reset trigger. \ref adi_apollo_sync_mask_rxtx_e
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_clk_mcs_dig_path_reset_mask_set(adi_apollo_device_t *device, adi_apollo_sync_mask_rxtx_e rxtx_mask);

/**
 * \brief Disable all data path resets based on a trigger
 *
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_clk_mcs_dig_path_reset_disable(adi_apollo_device_t *device);

/**
 * \brief Disable all JTx/JRx link resets based on a trigger
 *
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_clk_mcs_rxtx_link_reset_disable(adi_apollo_device_t *device);

/**
 * \brief Disable all data path link resets based on a trigger
 *
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_clk_mcs_lpbk_fifo_reset_disable(adi_apollo_device_t *device);

/**
 * \brief Oneshot sync.
 *
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_clk_mcs_oneshot_sync(adi_apollo_device_t *device);

/**
 * \brief Dynamic sync.
 *
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_clk_mcs_dynamic_sync(adi_apollo_device_t *device);

/**
 * \brief Manual reconfiguration sync.
 *
 * Sync modes	    Key behaviors
 * ----------       -------------
 * oneshot_sync	    HW state machine controlled sync flow, with HW based sysref alignment operation.
 * dyn_cfg_sync	    Programmable HW state machine controlled sync flow only (no sysref alignment).
 * manual_sync	    SPI controlled single step sync.
 * trigger_sync	    Trigger controlled/selected single step or continuous sync.

 * \param[in] device        Context variable - Pointer to the APOLLO device data structure
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_clk_mcs_man_reconfig_sync(adi_apollo_device_t *device);

/**
 * \brief Run default Dynamic Sync Sequence.
 *
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_clk_mcs_dyn_sync_sequence_run(adi_apollo_device_t *device);

/**
 * \brief Run JTx and JRx SerDes Link Dynamic Sync Sequence with digital root clks masked.
 *
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_clk_mcs_dyn_sync_rxtxlinks_sequence_run(adi_apollo_device_t *device);

/**
 * \brief Set the SYSREF receiver enable
 *
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure
 * \param[in] enable		MCS SYSREF receiver enable. ADI_APOLLO_ENABLE or ADI_APOLLO_DISABLE
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_clk_mcs_sysref_en_set(adi_apollo_device_t *device, uint8_t enable);

/**
 * \brief Get the MCS SYSREF receiver enable state
 *
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure
 * \param[out] enable		Pointer to sysref receiver enable result
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_clk_mcs_sysref_en_get(adi_apollo_device_t* device, uint8_t* enable);

/**
 * \brief Set the MCS internal sysref period
 *
 * The MCS internal SYSREF period is in units of Fclk.
 *
 * For 4T4R 1:1                   mode: sysref_per = (Fclk/Fsysref)/8
 * For 4T4R 1:2, 1:4 and all 8T8R mode: sysref_per = (Fclk/Fsysref)/4
 *
 * Fadc:Fdac ratio
 *
 *
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure
 * \param[in] sysref_per    The SYSREF period in units of fclk. Min: 32(fastest), Max: 65534(slowest). Must be even.
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_clk_mcs_internal_sysref_per_set(adi_apollo_device_t *device, uint16_t sysref_per);

/**
 * \brief Get the MCS internal sysref period
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[out] sysref_per   Pointer to sysref period result
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_clk_mcs_internal_sysref_per_get(adi_apollo_device_t *device, uint16_t *sysref_per);

/**
 * \brief Set the MCS subclass
 *
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure
 * \param[in] subclass      The JESD subclass. 0 = subclass 0, 1 = subclass 1.
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_clk_mcs_subclass_set(adi_apollo_device_t *device, uint32_t subclass);

/**
 * \brief Get the MCS subclass
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[out] subclass     Pointer to subclass result
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_clk_mcs_subclass_get(adi_apollo_device_t *device, uint32_t *subclass);

/**
 * \brief Get the external to internal SYSREF phase difference
 *
 * This API returns a measurement of the external to internal phase difference. After
 * a synchronization (e.g. adi_apollo_clk_mcs_oneshot_sync()) the result should be 0.
 * Result is units of Fclk.
 *
 * \param[in]  device        Context variable - Pointer to the APOLLO device data structure
 * \param[out] phase_diff    Pointer to ext-to-int SYSREF phase delta in units or FClk.
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_clk_mcs_sysref_phase_get(adi_apollo_device_t *device, uint32_t *sysref_phase);

/**
 * \brief Number of SYSREFs to ignore before using one as a proper phase sampling instant
 *
 * Assuming that the first several sysrefs arriving at the device will be noisy or invalid in time.
 * Ask the device to wait a predetermined number of pulses before using one to use as a
 * proper phase sample of the internal counter.
 *
 * \param[in]  device        Context variable - Pointer to the APOLLO device data structure
 * \param[in]  sysref_count  Number of sysrefs to ignore (range 0-255)
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_clk_mcs_sysref_count_set(adi_apollo_device_t* device, uint32_t sysref_count);

/**
 * \brief Set the SYSREF internal termination resistor enable.
 *
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure.
 * \param[in] enable        MCS SYSREF internal termination resistor enable.
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_clk_mcs_sysref_internal_term_en_set(adi_apollo_device_t *device, uint8_t enable);

/**
 * \brief Maps a trigger pin to Rx and Tx digital
 *
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure.
 * \param[in] rx_tx_select  Rx and Tx digital selection mask to map. \ref adi_apollo_rx_tx_dig_select_e
 * \param[in] trig_pin      The trigger pin to map to the selected digital blocks
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_clk_mcs_sync_trig_map(adi_apollo_device_t *device, uint16_t rx_tx_select, adi_apollo_trig_pin_e trig_pin);

/**
 * \brief Get the status of the clock input power detector
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure.
 * \param[out] status_a		Clock input power detector status for center clock (single mode) or side A (dual mode)
 * \param[out] status_b		Clock input power detector status unused (single mode) or side B (dual mode)
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_clk_mcs_input_power_status_get(adi_apollo_device_t *device, adi_apollo_clk_input_power_status_e *status_a, adi_apollo_clk_input_power_status_e *status_b);


#ifndef CLIENT_IGNORE
#endif /* CLIENT_IGNORE*/

#ifdef __cplusplus
}
#endif

#endif /* __ADI_APOLLO_CLK_MCS_H__ */
/*! @} */

