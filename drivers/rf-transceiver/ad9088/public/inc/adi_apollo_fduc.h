/*!
 * \brief     Fine DUC functional block API prototypes
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_FDUC
 * @{
 */
#ifndef __ADI_APOLLO_FDUC_H__
#define __ADI_APOLLO_FDUC_H__

/*============= I N C L U D E S ============*/
#include "adi_apollo_fduc_types.h"

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief  Sets fine DUC interpolation value
 *
 * \param[in]  device     Context variable - Pointer to the APOLLO device data structure
 * \param[in]  fducs      Fine DUC selection. ADI_APOLLO_FDUC_A0, _A0 to _A7, _B0 to _B7, _ALL \ref adi_apollo_fduc_select_e
 * \param[in]  interp     Interpolation enum. ADI_APOLLO_FDUC_RATIO_1, _2, _4, _8, _16, _32, _64 \ref adi_apollo_fduc_ratio_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_fduc_interp_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t fducs, uint8_t interp);

/**
 * \brief  Enable fine DUC sub datapath gain
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  fducs        Fine DUC selection. \ref adi_apollo_fduc_select_e
 * \param[in]  enable       Enable flag. 0: disable, 1: enable 
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_fduc_subdp_gain_enable(adi_apollo_device_t *device, adi_apollo_blk_sel_t fducs, uint8_t enable);

/**
 * \brief  Sets the fine DUC sub datapath gain
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  fducs        Fine DUC selection. \ref adi_apollo_fduc_select_e
 * \param[in]  gain         12-bit gain value 
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_fduc_subdp_gain_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t fducs, uint16_t gain);

/**
 * \brief  Sets the fine DUC integer time delay
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  fducs        Fine DUC selection. \ref adi_apollo_fduc_select_e
 * \param[in]  dly          Delay value 
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_fduc_int_tdly_hb_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t fducs, uint8_t dly);

/**
 * \brief  Sets fine DUC interpolator block parameters
 *
 * \param[in] device       Context variable - Pointer to the APOLLO device data structure
 * \param[in] fducs        Fine DUC selection. \ref adi_apollo_fduc_select_e
 * \param[in] config     Pointer to fine interpolation parameter struct. \ref adi_apollo_fduc_pgm_t
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_fduc_pgm(adi_apollo_device_t *device, adi_apollo_blk_sel_t fducs, const adi_apollo_fduc_pgm_t *config);

/**
 * \brief  Inspect FDUC parameters
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  fduc         Single Target FDUC \ref adi_apollo_fduc_select_e
 * \param[out] fduc_inspect     Pointer to FDUC inspect structure. \ref adi_apollo_fduc_inspect_t
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_fduc_inspect(adi_apollo_device_t *device, adi_apollo_blk_sel_t fduc, adi_apollo_fduc_inspect_t *fduc_inspect);

/**
 * \brief Enable or disable register control for setting a FDUC's active state
 *
 *  This method allows FDUC enabling using SPI, which overrides FDUC enables auto decoded 
 *  by summer configuration.
 * 
 * fduc_spien_en must be set for fduc_spi_en to have an effect.
 * 
 * \param[in]  device           Context variable - Pointer to the APOLLO device data structure
 * \param[in]  fducs            Coarse FUC selection. \ref adi_apollo_fduc_select_e
 * \param[in]  fduc_spien_en    Enables(1) or disables(0) register control of FDUC active state. Bypasses auto decode. (Typically disabled)
 * \param[in]  fduc_spi_en      If fduc_spien_en is true, then a 1 will activate and 0 deactivate the selected FDUCs
 * 
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_fduc_enable_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t fducs, uint8_t fduc_spien_en, uint8_t fduc_spi_en);


/**
 * \brief  Determine the numeric FDUC interpolation value from the associated enum. Enum vals are HW specific
 *
 * \param[in]  device           Context variable - Pointer to the APOLLO device data structure
 * \param[in]  bf_enum          Enum that contains register bitfield value for decimation.
 * \param[out] val              Numeric value of decimation bf enum.
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_fduc_interp_bf_to_val(adi_apollo_device_t* device, adi_apollo_fduc_ratio_e bf_enum, uint32_t* val);

/**
 * \brief  Enable FDUC overflows to trigger an interrupt
 *
 * \param[in]   device              Context variable - Pointer to the APOLLO device data structure
 * \param[in]   fducs               Target FDUC \ref adi_apollo_fduc_select_e
 * \param[in]   enable              1 to enable, 0 to disable
 * 
 * \return API_CMS_ERROR_OK                     Completed successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_fduc_irq_enable_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t fducs, uint8_t enable);


#ifndef CLIENT_IGNORE

#endif /* CLIENT_IGNORE */

#ifdef __cplusplus
}
#endif

#endif /* __ADI_APOLLO_FDUC_H__ */
/*! @} */
