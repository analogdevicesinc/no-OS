/** 
 * \file fpgagen6_bf_adrv904x_ads10_shim_tx_data_path.c Automatically generated file with generator ver
 * 0.0.16.0.
 * 
 * \brief Contains BitField functions to support FPGAGEN6 transceiver device.
 * 
 * FPGAGEN6 BITFIELD VERSION: 0.0.0.11
 * 
 * Disclaimer Legal Disclaimer
 * 
 * Copyright 2015 - 2023 Analog Devices Inc.
 * 
 * Released under the FPGAGEN6 API license, for more information see the "LICENSE.PDF" file in this zip file.
 */

#include "../../private/bf/fpgagen6_bf_adrv904x_ads10_shim_tx_data_path.h"
#include "adi_fpgagen6_hal.h"
#include "adi_common_error.h"
#include "adi_fpgagen6_error.h"

#include "fpgagen6_bf_error_types.h"

#define ADI_FILE ADI_FPGAGEN6_FILE_PRIVATE_BF_ADRV904X_ADS10_SHIM_TX_DATA_PATH


/**
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_Adrvgen6Ads10ShimTxDataPathChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Ads10ShimTxDataPath_InstanceId_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                       const fpgagen6_BfAdrvgen6Ads10ShimTxDataPathChanAddr_e baseAddr,
                                                                                       uint8_t* const bfValue)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint32_t bfValueTmp = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if FPGAGEN6_BITFIELD_NULL_CHECK > 0
    ADI_FPGAGEN6_NULL_PTR_REPORT_RETURN(&device->common, bfValue);
#endif /* FPGAGEN6_BITFIELD_NULL_CHECK */

    *bfValue = (uint8_t) 0;


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_ADS10_SHIM_TX_DATA_PATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_Adrvgen6Ads10ShimTxDataPath_InstanceId_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x5U),
                                                    &bfValueTmp,
                                                    0xFF,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

/**
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6Ads10ShimTxDataPathChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Ads10ShimTxDataPath_Scratch_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                    const fpgagen6_BfAdrvgen6Ads10ShimTxDataPathChanAddr_e baseAddr,
                                                                                    const uint32_t bfValue)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);


#if FPGAGEN6_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 4294967295U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         bfValue,
                         "Invalid BitField Value in fpgagen6_Adrvgen6Ads10ShimTxDataPath_Scratch_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_ADS10_SHIM_TX_DATA_PATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_Adrvgen6Ads10ShimTxDataPath_Scratch_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x8),
                                                     bfValue,
                                                     0xFF,
                                                     0x0);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x9),
                                                     bfValue >> 8,
                                                     0xFF,
                                                     0x0);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0xA),
                                                     bfValue >> 16,
                                                     0xFF,
                                                     0x0);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0xB),
                                                     bfValue >> 24,
                                                     0xFF,
                                                     0x0);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

/**
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_Adrvgen6Ads10ShimTxDataPathChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Ads10ShimTxDataPath_Scratch_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                    const fpgagen6_BfAdrvgen6Ads10ShimTxDataPathChanAddr_e baseAddr,
                                                                                    uint32_t* const bfValue)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint32_t bfValueTmp = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if FPGAGEN6_BITFIELD_NULL_CHECK > 0
    ADI_FPGAGEN6_NULL_PTR_REPORT_RETURN(&device->common, bfValue);
#endif /* FPGAGEN6_BITFIELD_NULL_CHECK */

    *bfValue = (uint32_t) 0;


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_ADS10_SHIM_TX_DATA_PATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_Adrvgen6Ads10ShimTxDataPath_Scratch_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x8U),
                                                    &bfValueTmp,
                                                    0xFF,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint32_t) bfValueTmp);

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x9U),
                                                    &bfValueTmp,
                                                    0xFF,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint32_t) bfValueTmp << 8);

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0xAU),
                                                    &bfValueTmp,
                                                    0xFF,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint32_t) bfValueTmp << 16);

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0xBU),
                                                    &bfValueTmp,
                                                    0xFF,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint32_t) bfValueTmp << 24);

    return recoveryAction;
}

/**
 * \brief This is self-clearing and resets the Data Path FIFOs in the Shim layer.  This should be performed before initiating all transmissions.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6Ads10ShimTxDataPathChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Ads10ShimTxDataPath_SoftReset_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                      const fpgagen6_BfAdrvgen6Ads10ShimTxDataPathChanAddr_e baseAddr,
                                                                                      const uint8_t bfValue)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);


#if FPGAGEN6_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 1U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         bfValue,
                         "Invalid BitField Value in fpgagen6_Adrvgen6Ads10ShimTxDataPath_SoftReset_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_ADS10_SHIM_TX_DATA_PATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_Adrvgen6Ads10ShimTxDataPath_SoftReset_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0xCU),
                                                     (uint32_t) bfValue,
                                                     0x1,
                                                     0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

/**
 * \brief This is self-clearing and resets the Data Path FIFOs in the Shim layer.  This should be performed before initiating all transmissions.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_Adrvgen6Ads10ShimTxDataPathChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Ads10ShimTxDataPath_SoftReset_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                      const fpgagen6_BfAdrvgen6Ads10ShimTxDataPathChanAddr_e baseAddr,
                                                                                      uint8_t* const bfValue)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint32_t bfValueTmp = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if FPGAGEN6_BITFIELD_NULL_CHECK > 0
    ADI_FPGAGEN6_NULL_PTR_REPORT_RETURN(&device->common, bfValue);
#endif /* FPGAGEN6_BITFIELD_NULL_CHECK */

    *bfValue = (uint8_t) 0;


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_ADS10_SHIM_TX_DATA_PATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_Adrvgen6Ads10ShimTxDataPath_SoftReset_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0xCU),
                                                    &bfValueTmp,
                                                    0x1,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

/**
 * \brief This is a local parameter in HDL and follows a MAJOR.MINOR.PATCH format, integers only.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_Adrvgen6Ads10ShimTxDataPathChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Ads10ShimTxDataPath_Version_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                    const fpgagen6_BfAdrvgen6Ads10ShimTxDataPathChanAddr_e baseAddr,
                                                                                    uint32_t* const bfValue)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint32_t bfValueTmp = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if FPGAGEN6_BITFIELD_NULL_CHECK > 0
    ADI_FPGAGEN6_NULL_PTR_REPORT_RETURN(&device->common, bfValue);
#endif /* FPGAGEN6_BITFIELD_NULL_CHECK */

    *bfValue = (uint32_t) 0;


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_ADS10_SHIM_TX_DATA_PATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_Adrvgen6Ads10ShimTxDataPath_Version_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x0U),
                                                    &bfValueTmp,
                                                    0xFF,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint32_t) bfValueTmp);

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x1U),
                                                    &bfValueTmp,
                                                    0xFF,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint32_t) bfValueTmp << 8);

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x2U),
                                                    &bfValueTmp,
                                                    0xFF,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint32_t) bfValueTmp << 16);

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x3U),
                                                    &bfValueTmp,
                                                    0xFF,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint32_t) bfValueTmp << 24);

    return recoveryAction;
}
