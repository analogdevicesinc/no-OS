/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_fpgagen6_tdd.c
* \brief Contains top level tdd fpga related functions
*
* FPGA API Version: 2.9.0.4
*/

#include "../../private/include/fpgagen6_tdd.h"

#define ADI_FILE    ADI_FPGAGEN6_FILE_PRIVATE_TDD

/**
*  \brief   FPGAGEN6 TDD TRX Enable Control Region Function Pointers
*/
const enableRegionSetFnPtr_t trxEnableStartValue_BfSetFnPtrs[ADI_FPGAGEN6_NUM_TDD_REGIONS] =
{
    fpgagen6_Adrvgen6Tdd_TrxEnableRegion0StartValue_BfSet,
    fpgagen6_Adrvgen6Tdd_TrxEnableRegion1StartValue_BfSet,
    fpgagen6_Adrvgen6Tdd_TrxEnableRegion2StartValue_BfSet,
    fpgagen6_Adrvgen6Tdd_TrxEnableRegion3StartValue_BfSet
};

const enableRegionSetFnPtr_t trxEnableDurationValue_BfSetFnPtrs[ADI_FPGAGEN6_NUM_TDD_REGIONS] =
{
    fpgagen6_Adrvgen6Tdd_TrxEnableRegion0Duration_BfSet,
    fpgagen6_Adrvgen6Tdd_TrxEnableRegion1Duration_BfSet,
    fpgagen6_Adrvgen6Tdd_TrxEnableRegion2Duration_BfSet,
    fpgagen6_Adrvgen6Tdd_TrxEnableRegion3Duration_BfSet
};

const enableRegionGetFnPtr_t trxEnableStartValue_BfGetFnPtrs[ADI_FPGAGEN6_NUM_TDD_REGIONS] =
{
    fpgagen6_Adrvgen6Tdd_TrxEnableRegion0StartValue_BfGet,
    fpgagen6_Adrvgen6Tdd_TrxEnableRegion1StartValue_BfGet,
    fpgagen6_Adrvgen6Tdd_TrxEnableRegion2StartValue_BfGet,
    fpgagen6_Adrvgen6Tdd_TrxEnableRegion3StartValue_BfGet
};

const enableRegionGetFnPtr_t trxEnableDurationValue_BfGetFnPtrs[ADI_FPGAGEN6_NUM_TDD_REGIONS] =
{
    fpgagen6_Adrvgen6Tdd_TrxEnableRegion0Duration_BfGet,
    fpgagen6_Adrvgen6Tdd_TrxEnableRegion1Duration_BfGet,
    fpgagen6_Adrvgen6Tdd_TrxEnableRegion2Duration_BfGet,
    fpgagen6_Adrvgen6Tdd_TrxEnableRegion3Duration_BfGet
};

/**
*  \brief   FPGAGEN6 TDD ORX Enable Control Region Function Pointers
*/
const enableRegionSetFnPtr_t orxEnableStartValue_BfSetFnPtrs[ADI_FPGAGEN6_NUM_TDD_REGIONS] =
{
    fpgagen6_Adrvgen6Tdd_OrxEnableRegion0StartValue_BfSet,
    fpgagen6_Adrvgen6Tdd_OrxEnableRegion1StartValue_BfSet,
    fpgagen6_Adrvgen6Tdd_OrxEnableRegion2StartValue_BfSet,
    fpgagen6_Adrvgen6Tdd_OrxEnableRegion3StartValue_BfSet
};

const enableRegionSetFnPtr_t orxEnableDurationValue_BfSetFnPtrs[ADI_FPGAGEN6_NUM_TDD_REGIONS] =
{
    fpgagen6_Adrvgen6Tdd_OrxEnableRegion0Duration_BfSet,
    fpgagen6_Adrvgen6Tdd_OrxEnableRegion1Duration_BfSet,
    fpgagen6_Adrvgen6Tdd_OrxEnableRegion2Duration_BfSet,
    fpgagen6_Adrvgen6Tdd_OrxEnableRegion3Duration_BfSet
};

const enableRegionGetFnPtr_t orxEnableStartValue_BfGetFnPtrs[ADI_FPGAGEN6_NUM_TDD_REGIONS] =
{
    fpgagen6_Adrvgen6Tdd_OrxEnableRegion0StartValue_BfGet,
    fpgagen6_Adrvgen6Tdd_OrxEnableRegion1StartValue_BfGet,
    fpgagen6_Adrvgen6Tdd_OrxEnableRegion2StartValue_BfGet,
    fpgagen6_Adrvgen6Tdd_OrxEnableRegion3StartValue_BfGet
};

const enableRegionGetFnPtr_t orxEnableDurationValue_BfGetFnPtrs[ADI_FPGAGEN6_NUM_TDD_REGIONS] =
{
    fpgagen6_Adrvgen6Tdd_OrxEnableRegion0Duration_BfGet,
    fpgagen6_Adrvgen6Tdd_OrxEnableRegion1Duration_BfGet,
    fpgagen6_Adrvgen6Tdd_OrxEnableRegion2Duration_BfGet,
    fpgagen6_Adrvgen6Tdd_OrxEnableRegion3Duration_BfGet
};

/**
*  \brief   FPGAGEN6 TDD GPIO Enable Control Region Function Pointers
*/
const enableRegionSetFnPtr_t gpioEnableStartValue_BfSetFnPtrs[ADI_FPGAGEN6_NUM_TDD_REGIONS] =
{
    fpgagen6_Adrvgen6Tdd_GpioEnableRegion0StartValue_BfSet,
    fpgagen6_Adrvgen6Tdd_GpioEnableRegion1StartValue_BfSet,
    fpgagen6_Adrvgen6Tdd_GpioEnableRegion2StartValue_BfSet,
    fpgagen6_Adrvgen6Tdd_GpioEnableRegion3StartValue_BfSet
};

const enableRegionSetFnPtr_t gpioEnableDurationValue_BfSetFnPtrs[ADI_FPGAGEN6_NUM_TDD_REGIONS] =
{
    fpgagen6_Adrvgen6Tdd_GpioEnableRegion0Duration_BfSet,
    fpgagen6_Adrvgen6Tdd_GpioEnableRegion1Duration_BfSet,
    fpgagen6_Adrvgen6Tdd_GpioEnableRegion2Duration_BfSet,
    fpgagen6_Adrvgen6Tdd_GpioEnableRegion3Duration_BfSet
};

const enableRegionGetFnPtr_t gpioEnableStartValue_BfGetFnPtrs[ADI_FPGAGEN6_NUM_TDD_REGIONS] =
{
    fpgagen6_Adrvgen6Tdd_GpioEnableRegion0StartValue_BfGet,
    fpgagen6_Adrvgen6Tdd_GpioEnableRegion1StartValue_BfGet,
    fpgagen6_Adrvgen6Tdd_GpioEnableRegion2StartValue_BfGet,
    fpgagen6_Adrvgen6Tdd_GpioEnableRegion3StartValue_BfGet
};

const enableRegionGetFnPtr_t gpioEnableDurationValue_BfGetFnPtrs[ADI_FPGAGEN6_NUM_TDD_REGIONS] =
{
    fpgagen6_Adrvgen6Tdd_GpioEnableRegion0Duration_BfGet,
    fpgagen6_Adrvgen6Tdd_GpioEnableRegion1Duration_BfGet,
    fpgagen6_Adrvgen6Tdd_GpioEnableRegion2Duration_BfGet,
    fpgagen6_Adrvgen6Tdd_GpioEnableRegion3Duration_BfGet
};

/**
*  \brief   FPGAGEN6 TDD GPIO Enable Control Region Function Pointers
*/
const enableRegionSetFnPtr_t extEnableStartValue_BfSetFnPtrs[ADI_FPGAGEN6_NUM_TDD_REGIONS] =
{
    fpgagen6_Adrvgen6Tdd_ExtEnableRegion0StartValue_BfSet,
    fpgagen6_Adrvgen6Tdd_ExtEnableRegion1StartValue_BfSet,
    fpgagen6_Adrvgen6Tdd_ExtEnableRegion2StartValue_BfSet,
    fpgagen6_Adrvgen6Tdd_ExtEnableRegion3StartValue_BfSet
};

const enableRegionSetFnPtr_t extEnableDurationValue_BfSetFnPtrs[ADI_FPGAGEN6_NUM_TDD_REGIONS] =
{
    fpgagen6_Adrvgen6Tdd_ExtEnableRegion0Duration_BfSet,
    fpgagen6_Adrvgen6Tdd_ExtEnableRegion1Duration_BfSet,
    fpgagen6_Adrvgen6Tdd_ExtEnableRegion2Duration_BfSet,
    fpgagen6_Adrvgen6Tdd_ExtEnableRegion3Duration_BfSet
};

const enableRegionGetFnPtr_t extEnableStartValue_BfGetFnPtrs[ADI_FPGAGEN6_NUM_TDD_REGIONS] =
{
    fpgagen6_Adrvgen6Tdd_ExtEnableRegion0StartValue_BfGet,
    fpgagen6_Adrvgen6Tdd_ExtEnableRegion1StartValue_BfGet,
    fpgagen6_Adrvgen6Tdd_ExtEnableRegion2StartValue_BfGet,
    fpgagen6_Adrvgen6Tdd_ExtEnableRegion3StartValue_BfGet
};

const enableRegionGetFnPtr_t extEnableDurationValue_BfGetFnPtrs[ADI_FPGAGEN6_NUM_TDD_REGIONS] =
{
    fpgagen6_Adrvgen6Tdd_ExtEnableRegion0Duration_BfGet,
    fpgagen6_Adrvgen6Tdd_ExtEnableRegion1Duration_BfGet,
    fpgagen6_Adrvgen6Tdd_ExtEnableRegion2Duration_BfGet,
    fpgagen6_Adrvgen6Tdd_ExtEnableRegion3Duration_BfGet
};


ADI_API adi_fpgagen6_ErrAction_e fpgagen6_TddEnableControlCfgSet(   adi_fpgagen6_Device_t* const                    device,
                                                                    const fpgagen6_TddEnableModule_e                tddEnableModule,
                                                                    const uint8_t                                   tddEnableIdx,
                                                                    const adi_fpgagen6_TddEnableControl_t* const    tddControlCfg)
{
    adi_fpgagen6_ErrAction_e            recoveryAction          = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t                             regionIdx               = 0U;
    uint8_t                             regionEnableMask        = 0U;
    fpgagen6_TddEnableControlBfSet_t    enableModuleSetFnPtr;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    ADI_FPGAGEN6_NULL_PTR_REPORT_RETURN(&device->common, tddControlCfg);

    ADI_LIBRARY_MEMSET(&enableModuleSetFnPtr, 0, sizeof(enableModuleSetFnPtr));

    /* Configure Set Function Pointers for TDD Enable Module */

    switch (tddEnableModule)
    {
        case FPGAGEN6_TDD_ENABLEMODULE_TRX:
            for (regionIdx = 0U; regionIdx < ADI_FPGAGEN6_NUM_TDD_REGIONS; ++regionIdx)
            {
                enableModuleSetFnPtr.enableStartValue_BfSetFnPtrs[regionIdx]    = trxEnableStartValue_BfSetFnPtrs[regionIdx];
                enableModuleSetFnPtr.enableDurationValue_BfSetFnPtrs[regionIdx] = trxEnableDurationValue_BfSetFnPtrs[regionIdx];
            }
            enableModuleSetFnPtr.enableRegionBfSet          = fpgagen6_Adrvgen6Tdd_TrxRegionEnables_BfSet;
            enableModuleSetFnPtr.enableFramesOffsetBfSet    = fpgagen6_Adrvgen6Tdd_TrxEnableFramesOffset_BfSet;
            enableModuleSetFnPtr.enableFramesActiveBfSet    = fpgagen6_Adrvgen6Tdd_TrxEnableFramesActive_BfSet;
            enableModuleSetFnPtr.enableFramesInctiveBfSet   = fpgagen6_Adrvgen6Tdd_TrxEnableFramesInactive_BfSet;
            break;

        case FPGAGEN6_TDD_ENABLEMODULE_ORX:
            for (regionIdx = 0U; regionIdx < ADI_FPGAGEN6_NUM_TDD_REGIONS; ++regionIdx)
            {
                enableModuleSetFnPtr.enableStartValue_BfSetFnPtrs[regionIdx]    = orxEnableStartValue_BfSetFnPtrs[regionIdx];
                enableModuleSetFnPtr.enableDurationValue_BfSetFnPtrs[regionIdx] = orxEnableDurationValue_BfSetFnPtrs[regionIdx];
            }
            enableModuleSetFnPtr.enableRegionBfSet          = fpgagen6_Adrvgen6Tdd_OrxRegionEnables_BfSet;
            enableModuleSetFnPtr.enableFramesOffsetBfSet    = fpgagen6_Adrvgen6Tdd_OrxEnableFramesOffset_BfSet;
            enableModuleSetFnPtr.enableFramesActiveBfSet    = fpgagen6_Adrvgen6Tdd_OrxEnableFramesActive_BfSet;
            enableModuleSetFnPtr.enableFramesInctiveBfSet   = fpgagen6_Adrvgen6Tdd_OrxEnableFramesInactive_BfSet;
            break;

        case FPGAGEN6_TDD_ENABLEMODULE_GPIO:
            for (regionIdx = 0U; regionIdx < ADI_FPGAGEN6_NUM_TDD_REGIONS; ++regionIdx)
            {
                enableModuleSetFnPtr.enableStartValue_BfSetFnPtrs[regionIdx]    = gpioEnableStartValue_BfSetFnPtrs[regionIdx];
                enableModuleSetFnPtr.enableDurationValue_BfSetFnPtrs[regionIdx] = gpioEnableDurationValue_BfSetFnPtrs[regionIdx];
            }
            enableModuleSetFnPtr.enableRegionBfSet          = fpgagen6_Adrvgen6Tdd_GpioRegionEnables_BfSet;
            enableModuleSetFnPtr.enableFramesOffsetBfSet    = fpgagen6_Adrvgen6Tdd_GpioEnableFramesOffset_BfSet;
            enableModuleSetFnPtr.enableFramesActiveBfSet    = fpgagen6_Adrvgen6Tdd_GpioEnableFramesActive_BfSet;
            enableModuleSetFnPtr.enableFramesInctiveBfSet   = fpgagen6_Adrvgen6Tdd_GpioEnableFramesInactive_BfSet;
            break;

        case FPGAGEN6_TDD_ENABLEMODULE_EXT:
            for (regionIdx = 0U; regionIdx < ADI_FPGAGEN6_NUM_TDD_REGIONS; ++regionIdx)
            {
                enableModuleSetFnPtr.enableStartValue_BfSetFnPtrs[regionIdx]    = extEnableStartValue_BfSetFnPtrs[regionIdx];
                enableModuleSetFnPtr.enableDurationValue_BfSetFnPtrs[regionIdx] = extEnableDurationValue_BfSetFnPtrs[regionIdx];
            }
            enableModuleSetFnPtr.enableRegionBfSet          = fpgagen6_Adrvgen6Tdd_ExtRegionEnables_BfSet;
            enableModuleSetFnPtr.enableFramesOffsetBfSet    = fpgagen6_Adrvgen6Tdd_ExtEnableFramesOffset_BfSet;
            enableModuleSetFnPtr.enableFramesActiveBfSet    = fpgagen6_Adrvgen6Tdd_ExtEnableFramesActive_BfSet;
            enableModuleSetFnPtr.enableFramesInctiveBfSet   = fpgagen6_Adrvgen6Tdd_ExtEnableFramesInactive_BfSet;
            break;

        default:
            recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                    recoveryAction,
                                    tddEnableModule,
                                    "Invalid TDD Enable Type");
            return recoveryAction;
            break;
    }

    for (regionIdx = 0U; regionIdx < ADI_FPGAGEN6_NUM_TDD_REGIONS; ++regionIdx)
    {
        if (enableModuleSetFnPtr.enableStartValue_BfSetFnPtrs[regionIdx] == NULL)
        {
            recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "TDD Enable Module Set Function Pointer Setup Issue");
            return recoveryAction;
        }
        if (enableModuleSetFnPtr.enableDurationValue_BfSetFnPtrs[regionIdx] == NULL)
        {
            recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "TDD Enable Module Set Function Pointer Setup Issue");
            return recoveryAction;
        }
    }

    if ((enableModuleSetFnPtr.enableRegionBfSet         == NULL)    ||
        (enableModuleSetFnPtr.enableFramesOffsetBfSet   == NULL)    ||
        (enableModuleSetFnPtr.enableFramesActiveBfSet   == NULL)    ||
        (enableModuleSetFnPtr.enableFramesInctiveBfSet  == NULL))
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "TDD Enable Module Set Function Pointer Setup Issue");
        return recoveryAction;
    }

    /* Set Start & Duration Values per Region Per Pin */

    for (regionIdx = 0U; regionIdx < ADI_FPGAGEN6_NUM_TDD_REGIONS; ++regionIdx)
    {
        recoveryAction = enableModuleSetFnPtr.enableStartValue_BfSetFnPtrs[regionIdx](  device,
                                                                                        FPGAGEN6_BF_ADS10_TOP_TDD,
                                                                                        tddEnableIdx,
                                                                                        tddControlCfg->start[regionIdx]);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        recoveryAction = enableModuleSetFnPtr.enableDurationValue_BfSetFnPtrs[regionIdx](   device,
                                                                                            FPGAGEN6_BF_ADS10_TOP_TDD,
                                                                                            tddEnableIdx,
                                                                                            tddControlCfg->duration[regionIdx]);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        /* Configure Region Enable Mask */
        if (tddControlCfg->duration[regionIdx] != 0U)
        {
            regionEnableMask |= (1U << regionIdx);
        }
    }

    /* Enable Regions Per Pin */

    recoveryAction = enableModuleSetFnPtr.enableRegionBfSet(device,
                                                            FPGAGEN6_BF_ADS10_TOP_TDD,
                                                            tddEnableIdx,
                                                            regionEnableMask);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                    ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction);
    }

    /* Set Frame Offset Per Pin */

    recoveryAction = enableModuleSetFnPtr.enableFramesOffsetBfSet( device,
                                                                FPGAGEN6_BF_ADS10_TOP_TDD,
                                                                tddEnableIdx,
                                                                tddControlCfg->framesOffset);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                  ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                  ADI_NO_VARIABLE,
                                                  recoveryAction);
    }

    /* Set Frame  Active Per Pin */

    recoveryAction = enableModuleSetFnPtr.enableFramesActiveBfSet( device,
                                                                FPGAGEN6_BF_ADS10_TOP_TDD,
                                                                tddEnableIdx,
                                                                tddControlCfg->framesActive);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                    ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction);
    }

    /* Set Frame  Inactive Per Pin */

    recoveryAction = enableModuleSetFnPtr.enableFramesInctiveBfSet(device,
                                                                FPGAGEN6_BF_ADS10_TOP_TDD,
                                                                tddEnableIdx,
                                                                tddControlCfg->framesInactive);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                    ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction);
    }

    return recoveryAction;
}

ADI_API adi_fpgagen6_ErrAction_e fpgagen6_TddEnableControlCfgGet(   adi_fpgagen6_Device_t* const            device,
                                                                    const fpgagen6_TddEnableModule_e        tddEnableModule,
                                                                    const uint8_t                           tddEnableIdx,
                                                                    adi_fpgagen6_TddEnableControl_t* const  tddControlCfg)
{
    adi_fpgagen6_ErrAction_e            recoveryAction          = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t                             regionIdx               = 0U;
    fpgagen6_TddEnableControlBfGet_t    enableModuleGetFnPtr;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    ADI_FPGAGEN6_NULL_PTR_REPORT_RETURN(&device->common, tddControlCfg);

    ADI_LIBRARY_MEMSET(&enableModuleGetFnPtr, 0, sizeof(enableModuleGetFnPtr));

    /* Configure Set Function Pointers for TDD Enable Module */

    switch (tddEnableModule)
    {
        case FPGAGEN6_TDD_ENABLEMODULE_TRX:
            for (regionIdx = 0U; regionIdx < ADI_FPGAGEN6_NUM_TDD_REGIONS; ++regionIdx)
            {
                enableModuleGetFnPtr.enableStartValue_BfGetFnPtrs[regionIdx]    = trxEnableStartValue_BfGetFnPtrs[regionIdx];
                enableModuleGetFnPtr.enableDurationValue_BfGetFnPtrs[regionIdx] = trxEnableDurationValue_BfGetFnPtrs[regionIdx];
            }
            enableModuleGetFnPtr.enableFramesOffsetBfGet  = fpgagen6_Adrvgen6Tdd_TrxEnableFramesOffset_BfGet;
            enableModuleGetFnPtr.enableFramesActiveBfGet  = fpgagen6_Adrvgen6Tdd_TrxEnableFramesActive_BfGet;
            enableModuleGetFnPtr.enableFramesInctiveBfGet = fpgagen6_Adrvgen6Tdd_TrxEnableFramesInactive_BfGet;
            break;

        case FPGAGEN6_TDD_ENABLEMODULE_ORX:
            for (regionIdx = 0U; regionIdx < ADI_FPGAGEN6_NUM_TDD_REGIONS; ++regionIdx)
            {
                enableModuleGetFnPtr.enableStartValue_BfGetFnPtrs[regionIdx]    = orxEnableStartValue_BfGetFnPtrs[regionIdx];
                enableModuleGetFnPtr.enableDurationValue_BfGetFnPtrs[regionIdx] = orxEnableDurationValue_BfGetFnPtrs[regionIdx];
            }
            enableModuleGetFnPtr.enableFramesOffsetBfGet  = fpgagen6_Adrvgen6Tdd_OrxEnableFramesOffset_BfGet;
            enableModuleGetFnPtr.enableFramesActiveBfGet  = fpgagen6_Adrvgen6Tdd_OrxEnableFramesActive_BfGet;
            enableModuleGetFnPtr.enableFramesInctiveBfGet = fpgagen6_Adrvgen6Tdd_OrxEnableFramesInactive_BfGet;
            break;

        case FPGAGEN6_TDD_ENABLEMODULE_GPIO:
            for (regionIdx = 0U; regionIdx < ADI_FPGAGEN6_NUM_TDD_REGIONS; ++regionIdx)
            {
                enableModuleGetFnPtr.enableStartValue_BfGetFnPtrs[regionIdx]    = gpioEnableStartValue_BfGetFnPtrs[regionIdx];
                enableModuleGetFnPtr.enableDurationValue_BfGetFnPtrs[regionIdx] = gpioEnableDurationValue_BfGetFnPtrs[regionIdx];
            }
            enableModuleGetFnPtr.enableFramesOffsetBfGet  = fpgagen6_Adrvgen6Tdd_GpioEnableFramesOffset_BfGet;
            enableModuleGetFnPtr.enableFramesActiveBfGet  = fpgagen6_Adrvgen6Tdd_GpioEnableFramesActive_BfGet;
            enableModuleGetFnPtr.enableFramesInctiveBfGet = fpgagen6_Adrvgen6Tdd_GpioEnableFramesInactive_BfGet;
            break;

        case FPGAGEN6_TDD_ENABLEMODULE_EXT:
            for (regionIdx = 0U; regionIdx < ADI_FPGAGEN6_NUM_TDD_REGIONS; ++regionIdx)
            {
                enableModuleGetFnPtr.enableStartValue_BfGetFnPtrs[regionIdx]    = extEnableStartValue_BfGetFnPtrs[regionIdx];
                enableModuleGetFnPtr.enableDurationValue_BfGetFnPtrs[regionIdx] = extEnableDurationValue_BfGetFnPtrs[regionIdx];
            }
            enableModuleGetFnPtr.enableFramesOffsetBfGet  = fpgagen6_Adrvgen6Tdd_ExtEnableFramesOffset_BfGet;
            enableModuleGetFnPtr.enableFramesActiveBfGet  = fpgagen6_Adrvgen6Tdd_ExtEnableFramesActive_BfGet;
            enableModuleGetFnPtr.enableFramesInctiveBfGet = fpgagen6_Adrvgen6Tdd_ExtEnableFramesInactive_BfGet;
            break;

        default:
            recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                   recoveryAction,
                                   tddEnableModule,
                                   "Invalid TDD Enable Type");
            return recoveryAction;
            break;
    }


    for (regionIdx = 0U; regionIdx < ADI_FPGAGEN6_NUM_TDD_REGIONS; ++regionIdx)
    {
        if (enableModuleGetFnPtr.enableStartValue_BfGetFnPtrs[regionIdx] == NULL)
        {
            recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "TDD Enable Module Get Function Pointer Setup Issue");
            return recoveryAction;
        }
        if (enableModuleGetFnPtr.enableDurationValue_BfGetFnPtrs[regionIdx] == NULL)
        {
            recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "TDD Enable Module Get Function Pointer Setup Issue");
            return recoveryAction;
        }
    }

    if ((enableModuleGetFnPtr.enableFramesOffsetBfGet   == NULL)    ||
        (enableModuleGetFnPtr.enableFramesActiveBfGet   == NULL)    ||
        (enableModuleGetFnPtr.enableFramesInctiveBfGet  == NULL))
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "TDD Enable Module Get Function Pointer Setup Issue");
        return recoveryAction;
    }

    /* Get Start & Duration Values per Region Per Pin */

    for (regionIdx = 0U; regionIdx < ADI_FPGAGEN6_NUM_TDD_REGIONS; ++regionIdx)
    {
        recoveryAction = enableModuleGetFnPtr.enableStartValue_BfGetFnPtrs[regionIdx](  device,
                                                                                        FPGAGEN6_BF_ADS10_TOP_TDD,
                                                                                        tddEnableIdx,
                                                                                        &tddControlCfg->start[regionIdx]);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        recoveryAction = enableModuleGetFnPtr.enableDurationValue_BfGetFnPtrs[regionIdx](   device,
                                                                                            FPGAGEN6_BF_ADS10_TOP_TDD,
                                                                                            tddEnableIdx,
                                                                                            &tddControlCfg->duration[regionIdx]);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }
    }

    /* Get Frame Offset Per Pin */

    recoveryAction = enableModuleGetFnPtr.enableFramesOffsetBfGet(  device,
                                                                    FPGAGEN6_BF_ADS10_TOP_TDD,
                                                                    tddEnableIdx,
                                                                    &tddControlCfg->framesOffset);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                    ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction);
    }

    /* Get Frame Active Per Pin */

    recoveryAction = enableModuleGetFnPtr.enableFramesActiveBfGet(  device,
                                                                    FPGAGEN6_BF_ADS10_TOP_TDD,
                                                                    tddEnableIdx,
                                                                    &tddControlCfg->framesActive);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                    ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction);
    }

    /* Get Frame Inactive Per Pin */

    recoveryAction = enableModuleGetFnPtr.enableFramesInctiveBfGet( device,
                                                                    FPGAGEN6_BF_ADS10_TOP_TDD,
                                                                    tddEnableIdx,
                                                                    &tddControlCfg->framesInactive);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                    ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction);
    }

    return recoveryAction;
}

ADI_API adi_fpgagen6_ErrAction_e fpgagen6_TddGpioSyncEnableSet(adi_fpgagen6_Device_t* const       device,
                                                               const adi_fpgagen6_TddGpioSync_e   enable)
{
    adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    

        /* Verify parameter */
    if ((enable != ADI_FPGAGEN6_TDD_GPIO_SYNC_DISABLE) &&
        (enable != ADI_FPGAGEN6_TDD_GPIO_SYNC_ENABLE) &&
        (enable != ADI_FPGAGEN6_TDD_GPIO_SYNC_CONTINUOUS))
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               enable,
                               "Invalid enable selection");
        return recoveryAction;
    }
    recoveryAction = fpgagen6_Adrvgen6Tdd_GpioSyncEnable_BfSet(device,
                                                               FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_KOROR_TDD,
                                                               enable);
    if (recoveryAction != ADI_FPGAGEN6_ERR_ACT_NONE)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(ADI_FPGAGEN6_ERRSRC_TDD,
                                                  ADI_FPGAGEN6_ERRCODE_TDD_GPIO_SYNC,
                                                  ADI_NO_VARIABLE,
                                                  recoveryAction);
        return recoveryAction;
    }
    
    return recoveryAction;
}

ADI_API adi_fpgagen6_ErrAction_e fpgagen6_TddGpioSyncEnableGet(adi_fpgagen6_Device_t* const        device,
                                                               adi_fpgagen6_TddGpioSync_e * const  enable)
{
    adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_FPGAGEN6_NULL_PTR_REPORT_RETURN(&device->common, enable);
    

    uint8_t bfValue = 0U;
    recoveryAction = fpgagen6_Adrvgen6Tdd_GpioSyncEnable_BfGet(device,
                                                               FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_KOROR_TDD,
                                                               &bfValue);
    if (recoveryAction != ADI_FPGAGEN6_ERR_ACT_NONE)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(ADI_FPGAGEN6_ERRSRC_TDD,
                                                  ADI_FPGAGEN6_ERRCODE_TDD_GPIO_SYNC,
                                                  ADI_NO_VARIABLE,
                                                  recoveryAction);
        return recoveryAction;
    }
    
    *enable = (adi_fpgagen6_TddGpioSync_e) bfValue;
    
    return recoveryAction;
}

ADI_API adi_fpgagen6_ErrAction_e fpgagen6_TddGpioSyncSelectSet(adi_fpgagen6_Device_t* const     device,
                                                               const adi_fpgagen6_GpioPinSel_e  gpioSyncSel)
{
    adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    

    
    if (gpioSyncSel >= ADI_FPGAGEN6_GPIO_INVALID)
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               gpioSyncSel,
                               "Invalid GPIO selected");
        return recoveryAction;
    }
    
    recoveryAction = fpgagen6_Adrvgen6Tdd_GpioSyncGpioSel_BfSet(device,
                                                                FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_KOROR_TDD,
                                                                gpioSyncSel);
    if (recoveryAction != ADI_FPGAGEN6_ERR_ACT_NONE)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(ADI_FPGAGEN6_ERRSRC_TDD,
                                                  ADI_FPGAGEN6_ERRCODE_TDD_GPIO_SYNC_SELECT,
                                                  ADI_NO_VARIABLE,
                                                  recoveryAction);
        return recoveryAction;
    }
    
    return recoveryAction;
}

ADI_API adi_fpgagen6_ErrAction_e fpgagen6_TddGpioSyncSelectGet(adi_fpgagen6_Device_t* const       device,
                                                               adi_fpgagen6_GpioPinSel_e * const  gpioSyncSel)
{
    adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_FPGAGEN6_NULL_PTR_REPORT_RETURN(&device->common, gpioSyncSel);
    

    uint8_t bfValue = 0U;
    recoveryAction = fpgagen6_Adrvgen6Tdd_GpioSyncGpioSel_BfGet(device,
                                                                FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_KOROR_TDD,
                                                                &bfValue);
    if (recoveryAction != ADI_FPGAGEN6_ERR_ACT_NONE)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(ADI_FPGAGEN6_ERRSRC_TDD,
                                                  ADI_FPGAGEN6_ERRCODE_TDD_GPIO_SYNC_SELECT,
                                                  ADI_NO_VARIABLE,
                                                  recoveryAction);
        return recoveryAction;
    }
    
    *gpioSyncSel = (adi_fpgagen6_GpioPinSel_e) bfValue;
    
    return recoveryAction;
}

