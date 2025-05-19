/*!
 * \brief     BMEM Block definition headers
 *
 * \copyright copyright(c) 2024 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_BMEM
 * @{
 */
#ifndef __ADI_APOLLO_BMEM_H__
#define __ADI_APOLLO_BMEM_H__

/*============= I N C L U D E S ============*/
#include "adi_apollo_bmem_types.h"

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief  Configures BMEM to AWG
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  bmems        Target BMEM \ref adi_apollo_bmem_sel_e
 * \param[in]  config       \ref adi_apollo_bmem_awg_t
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_bmem_hsdin_awg_config(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems, adi_apollo_bmem_awg_t *config);

/**
 * \brief  Starts normal BMEM AWG in loop
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  bmems        Target BMEM \ref adi_apollo_bmem_sel_e
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_bmem_hsdin_awg_start(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems);

/**
 * \brief  Stops normal BMEM AWG
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  bmems        Target BMEM \ref adi_apollo_bmem_sel_e
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_bmem_hsdin_awg_stop(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems);

/**
 * \brief Writes Rx BMEM SRAM.
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  bmems        Target BMEM \ref adi_apollo_bmem_sel_e
 * \param[in]  data         Array of data to write. Each 32-bit word contains two 16-bit samples
 * \param[in]  length       Number of 32-bit words to write. 64K max.
 */
int32_t adi_apollo_bmem_hsdin_awg_sram_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems, uint32_t data[], uint32_t length);

/**
 * \brief  Configures BMEM to AWG with 16-bit sample data.
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  bmems        Target BMEM \ref adi_apollo_bmem_sel_e
 * \param[in]  data16       Array of 16-bit data samples to play out of bmem awg. 
 * \param[in]  data16_len   Length of data array
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_bmem_hsdin_awg_sample_write(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems, int16_t data16[], uint32_t data16_len);

/**
 * \brief  Configures BMEM to AWG with 16-bit sample data for 8T8R devices
 * 
 * \note 
 * For 8T8R devices, BMEM instances split the sram memory between ADC pairs (e.g. A0/A2).
 * This results in half the number of samples per converter channel.
 * 
 * This function requires a user provided scratch memory. Prevents exceeding local stack limits. 
 * Apollo APIs don't use dynamic memory allocation.
 *
 * \param[in]  device           Context variable - Pointer to the APOLLO device data structure
 * \param[in]  bmems            Target BMEM \ref adi_apollo_bmem_sel_e
 * \param[in]  data16_0         Array of 16-bit data samples to play out of bmem awg for first ADC in pair
 * \param[in]  data16_1         Array of 16-bit data samples to play out of bmem awg for second ADC in pair
 * \param[in]  data16_len       Length of data16_0 and data16_1 arrays
 * \param[in]  scratch32        User provided 32-bit scratch array. Used by function to assemble data and write mem. 
 * \param[in]  scratch32_len    Length of scratch32 array
 * 
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_bmem_hsdin_awg_sample_shared_write(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems,
                                             int16_t data16_0[], int16_t data16_1[], uint32_t data16_len,
                                             uint32_t scratch32[], uint32_t scratch32_len);

/**
 * \brief  Configures BMEM to Capture
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  bmems        Target BMEM \ref adi_apollo_bmem_sel_e
 * \param[in]  config       \ref adi_apollo_bmem_capture_t
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_bmem_hsdin_capture_config(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems, adi_apollo_bmem_capture_t *config);

/**
 * \brief  Runs normal BMEM capture
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  bmems        Target BMEM \ref adi_apollo_bmem_sel_e
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_bmem_hsdin_capture_run(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems);

/**
 * \brief Reads Rx BMEM SRAM.
 *
 * \param[in]   device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]   bmems        Target BMEM \ref adi_apollo_bmem_sel_e
 * \param[out]  data         Data that is read out of each \ref adi_apollo_bmem_sel_e. Each 32-bit word contains two 16-bit samples
 * \param[in]   length       Number of 32-bit words to read. 64K max.
 */
int32_t adi_apollo_bmem_hsdin_capture_get(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems, uint32_t data[], uint32_t length);

/**
 * \brief  Sets BMEM sample delay
 *
 * \param[in]  device              Context variable - Pointer to the APOLLO device data structure
 * \param[in]  bmems               Target BMEM \ref adi_apollo_bmem_sel_e
 * \param[in]  sample_delay        Sample delay
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_bmem_hsdin_delay_sample_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems, uint16_t sample_delay);

/**
 * \brief  Sets BMEM delay for the 4 hopping profiles
 *
 * \param[in]  device              Context variable - Pointer to the APOLLO device data structure
 * \param[in]  bmems               Target BMEM \ref adi_apollo_bmem_sel_e
 * \param[in]  sample_delay        Sample delay array - 4 elements
 * \param[in]  sample_delay_length Sample delay array length (must be 4)
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_bmem_hsdin_delay_hop_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems, uint16_t sample_delay[], uint32_t sample_delay_length);

/**
 * \brief  Configures BMEM to delay
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  bmems        Target BMEM \ref adi_apollo_bmem_sel_e
 * \param[in]  config       BMEM delay config \ref adi_apollo_bmem_delay_sample_t
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_bmem_hsdin_delay_sample_config(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems, adi_apollo_bmem_delay_sample_t *config);

/**
 * \brief  Configures BMEM to delay with hopping mode
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  bmems        Target BMEM \ref adi_apollo_bmem_sel_e
 * \param[in]  config       BMEM delay config \ref adi_apollo_bmem_delay_hop_t
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_bmem_hsdin_delay_hop_config(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems, adi_apollo_bmem_delay_hop_t *config);

/**
 * \brief  Starts BMEM sample delay 
 *
 * \param[in]  device              Context variable - Pointer to the APOLLO device data structure
 * \param[in]  bmems               Target BMEM \ref adi_apollo_bmem_sel_e
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_bmem_hsdin_delay_start(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems);

/**
 * \brief  Sets BMEM sample delay
 *
 * \param[in]  device              Context variable - Pointer to the APOLLO device data structure
 * \param[in]  bmems               Target BMEM \ref adi_apollo_bmem_sel_e
 * \param[in]  sample_delay        Sample delay
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_bmem_cddc_delay_sample_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems, uint16_t sample_delay);

/**
 * \brief  Sets BMEM delay for the 4 hopping profiles
 *
 * \param[in]  device              Context variable - Pointer to the APOLLO device data structure
 * \param[in]  bmems               Target BMEM \ref adi_apollo_bmem_sel_e
 * \param[in]  sample_delay        Sample delay array - 4 elements
 * \param[in]  sample_delay_length Sample delay array length (must be 4)
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_bmem_cddc_delay_hop_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems, uint16_t sample_delay[], uint32_t sample_delay_length);

/**
 * \brief  Configures BMEM to delay
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  bmems        Target BMEM \ref adi_apollo_bmem_sel_e
 * \param[in]  config       BMEM delay config \ref adi_apollo_bmem_delay_sample_t
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_bmem_cddc_delay_sample_config(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems, adi_apollo_bmem_delay_sample_t *config);

/**
 * \brief  Configures BMEM to delay with hopping mode
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  bmems        Target BMEM \ref adi_apollo_bmem_sel_e
 * \param[in]  config       BMEM delay config \ref adi_apollo_bmem_delay_hop_t
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_bmem_cddc_delay_hop_config(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems, adi_apollo_bmem_delay_hop_t *config);

/**
 * \brief  Starts BMEM sample delay 
 *
 * \param[in]  device              Context variable - Pointer to the APOLLO device data structure
 * \param[in]  bmems               Target BMEM \ref adi_apollo_bmem_sel_e
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_bmem_cddc_delay_start(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems);

/**
 * \brief  Sets BMEM sample delay
 *
 * \param[in]  device              Context variable - Pointer to the APOLLO device data structure
 * \param[in]  bmems               Target BMEM \ref adi_apollo_bmem_sel_e
 * \param[in]  sample_delay        Sample delay
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_bmem_fddc_delay_sample_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems, uint16_t sample_delay);

/**
 * \brief  Sets BMEM delay for the 4 hopping profiles
 *
 * \param[in]  device              Context variable - Pointer to the APOLLO device data structure
 * \param[in]  bmems               Target BMEM \ref adi_apollo_bmem_sel_e
 * \param[in]  sample_delay        Sample delay array - 4 elements
 * \param[in]  sample_delay_length Sample delay array length (must be 4)
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_bmem_fddc_delay_hop_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems, uint16_t sample_delay[], uint32_t sample_delay_length);

/**
 * \brief  Configures BMEM to delay
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  bmems        Target BMEM \ref adi_apollo_bmem_sel_e
 * \param[in]  config       BMEM delay config \ref adi_apollo_bmem_delay_sample_t
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_bmem_fddc_delay_sample_config(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems, adi_apollo_bmem_delay_sample_t *config);

/**
 * \brief  Configures BMEM to delay with hopping mode
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  bmems        Target BMEM \ref adi_apollo_bmem_sel_e
 * \param[in]  config       BMEM delay config \ref adi_apollo_bmem_delay_hop_t
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_bmem_fddc_delay_hop_config(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems, adi_apollo_bmem_delay_hop_t *config);

/**
 * \brief  Starts BMEM sample delay 
 *
 * \param[in]  device              Context variable - Pointer to the APOLLO device data structure
 * \param[in]  bmems               Target BMEM \ref adi_apollo_bmem_sel_e
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_bmem_fddc_delay_start(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems);

/* DEPRECATED */

/**
 * \brief  Sets BMEM sample delay
 * 
 * \deprecated  Use \ref adi_apollo_bmem_hsdin_delay_sample_set() instead.
 *              This function will be removed in the future.
 *
 * \param[in]  device              Context variable - Pointer to the APOLLO device data structure
 * \param[in]  bmems               Target BMEM \ref adi_apollo_bmem_sel_e
 * \param[in]  sample_delay        Sample delay
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_bmem_sample_delay_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems, uint16_t sample_delay);

/**
 * \brief  Sets BMEM delay for the 4 hopping profiles
 * 
 * \deprecated  Use \ref adi_apollo_bmem_hsdin_delay_hop_set() instead.
 *              This function will be removed in the future.
 *
 * \param[in]  device              Context variable - Pointer to the APOLLO device data structure
 * \param[in]  bmems               Target BMEM \ref adi_apollo_bmem_sel_e
 * \param[in]  sample_delay        Sample delay array - 4 elements
 * \param[in]  sample_delay_length Sample delay array length (must be 4)
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_bmem_hop_delay_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems, uint16_t sample_delay[], uint32_t sample_delay_length);

/**
 * \brief  Configures BMEM to delay
 * 
 * \deprecated  Use \ref adi_apollo_bmem_hsdin_delay_sample_config() instead.
 *              This function will be removed in the future.
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  bmems        Target BMEM \ref adi_apollo_bmem_sel_e
 * \param[in]  config       BMEM delay config \ref adi_apollo_bmem_delay_sample_t
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_bmem_sample_delay_config(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems, adi_apollo_bmem_delay_sample_t *config);

/**
 * \brief  Configures BMEM to delay with hopping mode
 * 
 * \deprecated  Use \ref adi_apollo_bmem_hsdin_delay_hop_config() instead.
 *              This function will be removed in the future.
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  bmems        Target BMEM \ref adi_apollo_bmem_sel_e
 * \param[in]  config       BMEM delay config \ref adi_apollo_bmem_delay_hop_t
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_bmem_hop_delay_config(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems, adi_apollo_bmem_delay_hop_t *config);

/**
 * \brief  Starts BMEM sample delay 
 * 
 * \deprecated  Use \ref adi_apollo_bmem_hsdin_delay_start() instead.
 *              This function will be removed in the future.
 *
 * \param[in]  device              Context variable - Pointer to the APOLLO device data structure
 * \param[in]  bmems               Target BMEM \ref adi_apollo_bmem_sel_e
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_bmem_delay_start(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems);

/**
 * \brief  Configures BMEM to AWG with 16-bit sample data.
 * 
 * \deprecated  Use \ref adi_apollo_bmem_hsdin_awg_sample_write() instead.
 *              This function will be removed in the future.
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  bmems        Target BMEM \ref adi_apollo_bmem_sel_e
 * \param[in]  setup        If set, will configure BMEM and issue oneshot sync. Typically needed on first call.
 * \param[in]  data16       Array of 16-bit data samples to play out of bmem awg. 
 * \param[in]  data16_len   Length of data array
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_bmem_awg_data16_config(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems, uint8_t setup, int16_t data16[], uint32_t data16_len);


/**
 * \brief  Configures BMEM to AWG with 16-bit sample data for 8T8R devices
 * 
 * \deprecated  Use \ref adi_apollo_bmem_hsdin_awg_sample_shared_write() instead.
 *              This function will be removed in the future.
 * 
 * \note 
 * For 8T8R devices, BMEM instances split the sram memory between ADC pairs (e.g. A0/A2).
 * This results in half the number of samples per converter channel.
 * 
 * This function requires a user provided scratch memory. Prevents exceeding local stack limits. 
 * Apollo APIs don't use dynamic memory allocation.
 *
 * \param[in]  device           Context variable - Pointer to the APOLLO device data structure
 * \param[in]  bmems            Target BMEM \ref adi_apollo_bmem_sel_e
 * \param[in]  setup            If set, will configure BMEM and issue oneshot sync. Typically needed on first call
 * \param[in]  data16_0         Array of 16-bit data samples to play out of bmem awg for first ADC in pair
 * \param[in]  data16_1         Array of 16-bit data samples to play out of bmem awg for second ADC in pair
 * \param[in]  data16_len       Length of data16_0 and data16_1 arrays
 * \param[in]  scratch32        User provided 32-bit scratch array. Used by function to assemble data and write mem. 
 * \param[in]  scratch32_len    Length of scratch32 array
 * 
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_bmem_awg_data16_2x_config(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems, uint8_t setup,
                                             int16_t data16_0[], int16_t data16_1[], uint32_t data16_len,
                                             uint32_t scratch32[], uint32_t scratch32_len);
/**
 * \brief  Configures BMEM to Capture
 * 
 * \deprecated  Use \ref adi_apollo_bmem_hsdin_capture_config() instead.
 *              This function will be removed in the future.
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  bmems        Target BMEM \ref adi_apollo_bmem_sel_e
 * \param[in]  config       \ref adi_apollo_bmem_capture_t
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_bmem_capture_config(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems, adi_apollo_bmem_capture_t *config);

/**
 * \brief  Sets BMEM Capture to normal mode
 * 
 * \deprecated  Use \ref adi_apollo_bmem_hsdin_capture_config() instead.
 *              This function will be removed in the future.
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  bmems        Target BMEM \ref adi_apollo_bmem_sel_e
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_bmem_capture_normal_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems);

/**
 * \brief  Runs normal BMEM capture
 * 
 * \deprecated  Use \ref adi_apollo_bmem_hsdin_capture_run() instead.
 *              This function will be removed in the future.
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  bmems        Target BMEM \ref adi_apollo_bmem_sel_e
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_bmem_capture_normal_run(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems);

/**
 * \brief  Configures BMEM to AWG
 * 
 * \deprecated  Use \ref adi_apollo_bmem_hsdin_awg_config() instead.
 *              This function will be removed in the future.
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  bmems        Target BMEM \ref adi_apollo_bmem_sel_e
 * \param[in]  config       \ref adi_apollo_bmem_awg_t
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_bmem_awg_config(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems, adi_apollo_bmem_awg_t *config);

/**
 * \brief  Sets BMEM AWG to normal mode
 * 
 * \deprecated  Use \ref adi_apollo_bmem_hsdin_awg_config() instead.
 *              This function will be removed in the future.
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  bmems        Target BMEM \ref adi_apollo_bmem_sel_e
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_bmem_awg_normal_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems);

/**
 * \brief  Sets BMEM to allow AWG data write to memory
 * 
 * \deprecated  Use \ref adi_apollo_bmem_hsdin_awg_sample_write() instead.
 *              This function will be removed in the future.
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  bmems        Target BMEM \ref adi_apollo_bmem_sel_e
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_bmem_awg_normal_slow(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems);

/**
 * \brief  Starts normal BMEM AWG in loop
 * 
 * \deprecated  Use \ref adi_apollo_bmem_hsdin_awg_start() instead.
 *              This function will be removed in the future.
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  bmems        Target BMEM \ref adi_apollo_bmem_sel_e
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_bmem_awg_normal_start(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems);

/**
 * \brief  Stops normal BMEM AWG
 * 
 * \deprecated  Use \ref adi_apollo_bmem_hsdin_awg_stop() instead.
 *              This function will be removed in the future.
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  bmems        Target BMEM \ref adi_apollo_bmem_sel_e
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_bmem_awg_normal_stop(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems);

/**
 * \brief Writes Rx BMEM SRAM.
 * 
 * \deprecated  Use \ref adi_apollo_bmem_hsdin_awg_sram_set() instead.
 *              This function will be removed in the future.
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  bmems        Target BMEM \ref adi_apollo_bmem_sel_e
 * \param[in]  data         Array of data to write. Each 32-bit word contains two 16-bit samples
 * \param[in]  length       Number of 32-bit words to write. 64K max.
 */
int32_t adi_apollo_bmem_awg_normal_sram_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems, uint32_t data[], uint32_t length);

/**
 * \brief Reads Rx BMEM SRAM.
 * 
 * \deprecated  Use \ref adi_apollo_bmem_hsdin_capture_get() instead.
 *              This function will be removed in the future.
 *
 * \param[in]   device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]   bmems        Target BMEM \ref adi_apollo_bmem_sel_e
 * \param[out]  data         Data that is read out of each \ref adi_apollo_bmem_sel_e. Each 32-bit word contains two 16-bit samples
 * \param[in]   length       Number of 32-bit words to read. 64K max.
 */
int32_t adi_apollo_bmem_sram_get(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems, uint32_t data[], uint32_t length);

#ifdef __cplusplus
}
#endif

#endif /* __ADI_APOLLO_BMEM_H__ */
/*! @} */
