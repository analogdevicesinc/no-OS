/*!
 * \brief     Trigger and Timestamp functional block definition header
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_TRIGGER_TS
 * @{
 */
#ifndef __ADI_APOLLO_TRIGGER_TS_H__
#define __ADI_APOLLO_TRIGGER_TS_H__

/*============= I N C L U D E S ============*/
#include "adi_apollo_trigts_types.h"

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief       Configures trigger masters offset and period
 * 
 * \param[in]   device        Context variable - Pointer to the APOLLO device data structure
 * \param[in]   terminal      Target terminal \ref adi_apollo_terminal_e
 * \param[in]   side_sel      Target side  \ref adi_apollo_side_select_e
 * \param[in]   trig_masters  trigger masters selection \ref adi_apollo_trig_mst_sel_e
 * \param[in]   config        Configuration parameters. \ref adi_apollo_trig_mst_config_t
 *
 * \return API_CMS_ERROR_OK   API Completed Successfully
 * \return <0                 Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_trigts_mst_config(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_side_select_e side_sel,
                                   uint32_t trig_masters, adi_apollo_trig_mst_config_t *config);

/**
 * \brief       Gets the trigger master configuration
 * 
 * \param[in]   device        Context variable - Pointer to the APOLLO device data structure
 * \param[in]   terminal      Target terminal \ref adi_apollo_terminal_e
 * \param[in]   side_sel      Target side  \ref adi_apollo_side_select_e
 * \param[in]   trig_masters  trigger masters selection \ref adi_apollo_trig_mst_sel_e
 * \param[out]  config        Current trig mst configuration. \ref adi_apollo_trig_mst_config_t
 *
 * \return API_CMS_ERROR_OK   API Completed Successfully
 * \return <0                 Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_trigts_mst_inspect(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_side_select_e side_sel,
                                   uint32_t trig_masters, adi_apollo_trig_mst_config_t *config);

/**
 * \brief       Enables Mute trigger feature
 * 
 * \param[in]   device        Context variable - Pointer to the APOLLO device data structure
 * \param[in]   terminal      Target terminal \ref adi_apollo_terminal_e
 * \param[in]   side_sel      Target side  \ref adi_apollo_side_select_e
 * \param[in]   trig_masters  trigger masters selection \ref adi_apollo_trig_mst_sel_e
 * \param[in]   trigger_mute  Enable trigger mute. \ref adi_apollo_trig_mute_sel_e
 *
 * \return API_CMS_ERROR_OK   API Completed Successfully
 * \return <0                 Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_trigts_mst_mute(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_side_select_e side_sel,
                                 uint32_t trig_masters, adi_apollo_trig_mute_sel_e trigger_mute);

/**
 * \brief       Selects which mute mask to use for muting after specified count number
 * 
 * \param[in]   device        Context variable - Pointer to the APOLLO device data structure
 * \param[in]   terminal      Target terminal \ref adi_apollo_terminal_e
 * \param[in]   side_sel      Target side  \ref adi_apollo_side_select_e
 * \param[in]   trig_masters  trigger masters selection \ref adi_apollo_trig_mst_sel_e
 * \param[in]   trigger_mute  Which trigger counter register to use for trigger mute. \ref adi_apollo_trig_mute_mask_e
 *
 * \return API_CMS_ERROR_OK   API Completed Successfully
 * \return <0                 Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_trigts_mst_mute_mask_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_side_select_e side_sel,
                                 uint32_t trig_masters, adi_apollo_trig_mute_mask_e trigger_mute);

/**
 * \brief       Sets the number of pulses to enable trigger master before muting
 * 
 * \param[in]   device        Context variable - Pointer to the APOLLO device data structure
 * \param[in]   terminal      Target terminal \ref adi_apollo_terminal_e
 * \param[in]   side_sel      Target side  \ref adi_apollo_side_select_e
 * \param[in]   trigger_mute  Which trigger counter register to use for trigger mute. \ref adi_apollo_trig_mute_mask_e
 * \param[in]   trig_mst      Trigger master to count pulses \ref adi_apollo_trig_mst_sel_e
 * \param[in]   pulse_count   Number of pulses after which trigger master is muted
 *
 * \return API_CMS_ERROR_OK   API Completed Successfully
 * \return <0                 Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_trigts_mst_mute_mask_count_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_side_select_e side_sel,
                                 adi_apollo_trig_mute_mask_e trigger_mute, adi_apollo_trig_mst_sel_e trig_mst, uint16_t pulse_count);

/**
 * \brief       Select trigger selection mux output for dynamic reconfig
 * 
 * \param[in]   device        Context variable - Pointer to the APOLLO device data structure
 * \param[in]   terminal      Target terminal \ref adi_apollo_terminal_e
 * \param[in]   side_sel      Target side  \ref adi_apollo_side_select_e
 * \param[in]   trig_sel      Trigger Selection Mux Select Line  \ref adi_apollo_trigger_sel_mux_e
 *
 * \return API_CMS_ERROR_OK   API Completed Successfully
 * \return <0                 Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_trigts_reconfig_trig_sel_mux_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_side_select_e side_sel,
                                              adi_apollo_trigger_sel_mux_e trig_sel);

/**
 * \brief       Select trigger selection mux output for CDDC/CDUC
 * 
 * \param[in]   device        Context variable - Pointer to the APOLLO device data structure
 * \param[in]   terminal      Target terminal \ref adi_apollo_terminal_e
 * \param[in]   cdrcs         Target CDRCs \ref adi_apollo_cddc_select_e or \ref adi_apollo_cduc_select_e
 * \param[in]   trig_sel      Trigger Selection Mux Select Line  \ref adi_apollo_trigger_sel_mux_e
 *
 * \return API_CMS_ERROR_OK   API Completed Successfully
 * \return <0                 Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_trigts_cdrc_trig_sel_mux_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t cdrcs,
                                              adi_apollo_trigger_sel_mux_e trig_sel);

/**
 * \brief       Select trigger selection mux output for FDDC/FDUC
 * 
 * \param[in]   device        Context variable - Pointer to the APOLLO device data structure
 * \param[in]   terminal      Target terminal \ref adi_apollo_terminal_e
 * \param[in]   fdrcs         Target FDRCs \ref adi_apollo_fine_ddc_select_e or \ref adi_apollo_fine_duc_select_e
 * \param[in]   trig_sel      Trigger Selection Mux Select Line  \ref adi_apollo_trigger_sel_mux_e
 *
 * \return API_CMS_ERROR_OK   API Completed Successfully
 * \return <0                 Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_trigts_fdrc_trig_sel_mux_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t fdrcs, adi_apollo_trigger_sel_mux_e trig_sel);

/**
 * \brief       Select trigger selection mux output for BMEM
 * 
 * \param[in]   device        Context variable - Pointer to the APOLLO device data structure
 * \param[in]   terminal      Target terminal \ref adi_apollo_terminal_e
 * \param[in]   bmems         Target BMEM \ref adi_apollo_bmem_sel_e
 * \param[in]   trig_sel      Trigger Selection Mux Select Line  \ref adi_apollo_trigger_sel_mux_e
 *
 * \return API_CMS_ERROR_OK   API Completed Successfully
 * \return <0                 Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_trigts_bmem_trig_sel_mux_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t bmems, adi_apollo_trigger_sel_mux_e trig_sel);

/**
 * \brief       Select trigger selection mux output for PFILT
 * 
 * \param[in]   device        Context variable - Pointer to the APOLLO device data structure
 * \param[in]   terminal      Target terminal \ref adi_apollo_terminal_e
 * \param[in]   pfilts        Target PFILT \ref adi_apollo_pfilt_sel_e
 * \param[in]   trig_sel      Trigger Selection Mux Select Line  \ref adi_apollo_trigger_sel_mux_e
 *
 * \return API_CMS_ERROR_OK   API Completed Successfully
 * \return <0                 Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_trigts_pfilt_trig_sel_mux_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t pfilts, adi_apollo_trigger_sel_mux_e trig_sel);

/**
 * \brief       Select trigger selection mux output for CFIR
 * 
 * \param[in]   device        Context variable - Pointer to the APOLLO device data structure
 * \param[in]   terminal      Target terminal \ref adi_apollo_terminal_e
 * \param[in]   cfirs         Target CFIR \ref adi_apollo_cfir_sel_e
 * \param[in]   trig_sel      Trigger Selection Mux Select Line  \ref adi_apollo_trigger_sel_mux_e
 *
 * \return API_CMS_ERROR_OK   API Completed Successfully
 * \return <0                 Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_trigts_cfir_trig_sel_mux_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t cfirs, adi_apollo_trigger_sel_mux_e trig_sel);

/**
 * \brief       Configures CNCO trigger master's offset and period
 * 
 * \param[in]   device        Context variable - Pointer to the APOLLO device data structure
 * \param[in]   terminal      Target terminal \ref adi_apollo_terminal_e
 * \param[in]   cncos         Target CDDC \ref adi_apollo_cddc_select_e
 * \param[in]   config        Configuration parameters. \ref adi_apollo_trig_mst_config_t
 *
 * \return API_CMS_ERROR_OK   API Completed Successfully
 * \return <0                 Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_trigts_cnco_trig_mst_config(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t cncos, 
                                                 adi_apollo_trig_mst_config_t *config);

/**
 * \brief       Configures FNCO trigger master's offset and period
 * 
 * \param[in]   device        Context variable - Pointer to the APOLLO device data structure
 * \param[in]   terminal      Target terminal \ref adi_apollo_terminal_e
 * \param[in]   fncos         Target FDDC \ref adi_apollo_fine_ddc_select_e
 * \param[in]   config        Configuration parameters. \ref adi_apollo_trig_mst_config_t
 *
 * \return API_CMS_ERROR_OK   API Completed Successfully
 * \return <0                 Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_trigts_fnco_trig_mst_config(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t fncos, 
                                                 adi_apollo_trig_mst_config_t *config);

/**
 * \brief       Configures PFILT trigger master's offset and period
 * 
 * \param[in]   device        Context variable - Pointer to the APOLLO device data structure
 * \param[in]   terminal      Target terminal \ref adi_apollo_terminal_e
 * \param[in]   pfilts        Target PFILT \ref adi_apollo_pfilt_select_e
 * \param[in]   config        Configuration parameters. \ref adi_apollo_trig_mst_config_t
 *
 * \return API_CMS_ERROR_OK   API Completed Successfully
 * \return <0                 Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_trigts_pfilt_trig_mst_config(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t pfilts, 
                                                    adi_apollo_trig_mst_config_t *config);
/**
 * \brief       Configures CFIR trigger master's offset and period
 * 
 * \param[in]   device        Context variable - Pointer to the APOLLO device data structure
 * \param[in]   terminal      Target terminal \ref adi_apollo_terminal_e
 * \param[in]   cfirs         Target CFIR \ref adi_apollo_cfir_select_e
 * \param[in]   config        Configuration parameters. \ref adi_apollo_trig_mst_config_t
 *
 * \return API_CMS_ERROR_OK   API Completed Successfully
 * \return <0                 Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_trigts_cfir_trig_mst_config(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t cfirs,
                                                adi_apollo_trig_mst_config_t *config);

/**
 * \brief       Configures reconfig ctrl trigger masters offset and period
 * 
 * \param[in]   device        Context variable - Pointer to the APOLLO device data structure
 * \param[in]   terminal      Target terminal \ref adi_apollo_terminal_e
 * \param[in]   side_sel      Target side  \ref adi_apollo_side_select_e
 * \param[in]   config        Configuration parameters. \ref adi_apollo_trig_mst_config_t
 *
 * \return API_CMS_ERROR_OK   API Completed Successfully
 * \return <0                 Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_trigts_reconfig_trig_mst_config(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, 
                                                 adi_apollo_side_select_e side_sel, adi_apollo_trig_mst_config_t *config);

/**
 * \brief       Creates a trigger pulse via spi/hsci
 * 
 * \param[in]   device        Context variable - Pointer to the APOLLO device data structure
 * \param[in]   terminal      Target terminal \ref adi_apollo_terminal_e
 * \param[in]   side_sel      Target side  \ref adi_apollo_side_select_e
 *
 * \return API_CMS_ERROR_OK   API Completed Successfully
 * \return <0                 Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_trigts_trig_now(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_side_select_e side_sel);

/**
 * \brief       Return the timestamp counter value
 * 
 * \param[in]   device      Context variable - Pointer to the APOLLO device data structure
 * \param[in]   terminal    Target terminal \ref adi_apollo_terminal_e
 * \param[in]   side_sel    Target side  \ref adi_apollo_side_select_e
 * \param[out]  val         Pointer to 64-bit current timestamp counter value
 *
 * \return API_CMS_ERROR_OK   API Completed Successfully
 * \return <0                 Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_trigts_counter_get(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_side_select_e side_sel, uint64_t *val);

/**
 * \brief       Set the timestamp counter reset mode (via SPI or SYSREF)
 * 
 * \param[in]   device      Context variable - Pointer to the APOLLO device data structure
 * \param[in]   terminal    Target terminal \ref adi_apollo_terminal_e
 * \param[in]   side_sel    Target side  \ref adi_apollo_side_select_e
 * \param[in]   mode        Reset mode. ADI_APOLLO_TRIG_TS_RESET_MODE_SYSREF, _SPI based. \ref adi_apollo_trig_ts_reset_mode_e
 * 
 * Use adi_apollo_trigts_ts_reset() to reset the timestamp counter during program execution.
 *
 * \return API_CMS_ERROR_OK   API Completed Successfully
 * \return <0                 Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_trigts_ts_reset_mode_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, uint16_t side_sel, adi_apollo_trig_ts_reset_mode_e mode);

/**
 * \brief       Reset the timestamp counter
 * 
 * \param[in]   device      Context variable - Pointer to the APOLLO device data structure
 * \param[in]   terminal    Target terminal \ref adi_apollo_terminal_e
 * \param[in]   side_sel    Target side  \ref adi_apollo_side_select_e
 * \param[in]   sync        1 to enable reset on sync event AND perform sync event. 0 to just enable. (n/a for SPI reset mode)
 * 
 * Calling this API with sync=0 enables the sync event, but doesn't execute it. This allows  arming
 * multiple timestamp counters and syncing them together on final call with sync=1.
 *
 * \return API_CMS_ERROR_OK   API Completed Successfully
 * \return <0                 Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_trigts_ts_reset(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, uint16_t side_sel, uint8_t sync);

/**
 * \brief       Clears the timestamp reset done status sticky bit
 * 
 * \param[in]   device      Context variable - Pointer to the APOLLO device data structure
 * \param[in]   terminal    Target terminal \ref adi_apollo_terminal_e
 * \param[in]   side_sel    Target side  \ref adi_apollo_side_select_e
 * 
 * \return API_CMS_ERROR_OK   API Completed Successfully
 * \return <0                 Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_trigts_reset_done_clear(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, uint16_t side_sel);

/**
 * \brief       Returns the reset-done status following a timestamp reset request
 * 
 * \param[in]   device              Context variable - Pointer to the APOLLO device data structure
 * \param[in]   terminal            Target terminal \ref adi_apollo_terminal_e
 * \param[in]   side_sel            Target side  \ref adi_apollo_side_select_e
 * \param[out]  reset_done_status    Pointer to reset-done status. 1 if reset completed, 0 otherwise.
 * 
 * \return API_CMS_ERROR_OK   API Completed Successfully
 * \return <0                 Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_trigts_reset_done_get(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, uint16_t side_sel, uint8_t *reset_done_status);

#ifndef CLIENT_IGNORE
#endif /* CLIENT_IGNORE */

#ifdef __cplusplus
}
#endif

#endif /* __ADI_APOLLO_TRIGGER_TS_H__ */
/*! @} */
