/**
* Copyright 2015 - 2022 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
 * \file adrv904x_binloader.c
 * \brief Contains ADRV904X binary loader related private function implementations.
 *
 * ADRV904X API Version: 2.9.0.4
 */
#include "adi_library_types.h"
#include "../../private/include/adrv904x_binloader.h"
#include "../../public/include/adi_adrv904x_types.h"
#include "../../public/include/adi_adrv904x_user.h"
#include "../../private/include/adrv904x_platform_byte_order.h"

#define ADI_FILE                ADI_ADRV904X_FILE_PRIVATE_BINLOADER

#define SAFE_READ(r, d, f, s, n, file)                                                    \
r =  adrv904x_SafeBinFileRead(&f, s, n, file, sizeof(f));                                 \
if (r != ADI_ADRV904X_ERR_ACT_NONE)                                                       \
{                                                                                         \
    ADI_PARAM_ERROR_REPORT(&d->common, r, f, "Unable to extract variable from bin file.");\
    return recoveryAction;                                                                \
}

#define ADI_MIN(x, y)    (((x) < (y)) ? (x) : (y))

static void adrv904x_EndiannessCheckAndConvert(void* const buffer, const size_t elementSize, const size_t elementCount);
static adi_adrv904x_ErrAction_e adrv904x_SafeBinFileRead(void* const buffer, const size_t elementSize, const size_t elementCount, FILE* const file, const size_t sizeOfBuffer);
static adi_adrv904x_ErrAction_e adrv904x_LoadVersionFromBin(adi_adrv904x_Device_t* const device, adi_adrv904x_Version_t *const target, FILE* const file);
static adi_adrv904x_ErrAction_e adrv904x_LoadFwVersionFromBin(adi_adrv904x_Device_t* const device, adi_adrv904x_CpuFwVersion_t* const target, FILE* const file);
static adi_adrv904x_ErrAction_e adrv904x_LoadInitStructFromBin(adi_adrv904x_Device_t* const device, adi_adrv904x_Init_t* const target, FILE* const file);
static adi_adrv904x_ErrAction_e adrv904x_LoadPostMcsInitStructFromBin(adi_adrv904x_Device_t* const device, adi_adrv904x_PostMcsInit_t* const target, FILE* const file);

static void adrv904x_EndiannessCheckAndConvert(void* const buffer, const size_t elementSize, const size_t elementCount)
{
    /* new function entry reporting */
    uint8_t i = 0U;
    uint16_t* tmp16;
    uint32_t* tmp32;
    uint64_t* tmp64;

    if (elementSize == 2U)
    {
        //biteswap small
        tmp16 = (uint16_t* const)buffer;
        for (i = 0; i < elementCount; i++) {
            tmp16[i] = ADRV904X_HTOCS(tmp16[i]);
        }
    }
    else if (elementSize == 4U)
    {
        //biteswap large
        tmp32 = (uint32_t* const)buffer;
        for (i = 0; i < elementCount; i++) {
            tmp32[i] = ADRV904X_HTOCL(tmp32[i]);
        }
    }
    else if (elementSize == 8U)
    {
        //biteswap large large
        tmp64 = (uint64_t* const)buffer;
        for (i = 0; i < elementCount; i++) {
            tmp64[i] = ADRV904X_HTOCLL(tmp64[i]);
        }
    }
}

static adi_adrv904x_ErrAction_e adrv904x_SafeBinFileRead(void* const buffer, const size_t elementSize, const size_t elementCount, FILE* const file, const size_t sizeOfBuffer)
{
    adi_adrv904x_ErrAction_e recoveryAction  = ADI_ADRV904X_ERR_ACT_NONE;
    ADI_ADRV904X_NULL_PTR_RETURN(buffer);
    ADI_ADRV904X_NULL_PTR_RETURN(file);

    if (sizeOfBuffer > (elementCount * elementSize))
    {
        ADI_LIBRARY_MEMSET(buffer, 0, sizeOfBuffer);
    }

    if(ADI_LIBRARY_FREAD(buffer, 1, ADI_MIN(elementSize * elementCount, sizeOfBuffer), file) != ADI_MIN(elementSize * elementCount, sizeOfBuffer))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    }

    if (ADRV904X_LITTLE_ENDIAN == 0)
    {
        adrv904x_EndiannessCheckAndConvert(buffer, elementSize, elementCount);
    }

    return recoveryAction;
}

static adi_adrv904x_ErrAction_e adrv904x_LoadVersionFromBin(adi_adrv904x_Device_t* const device, adi_adrv904x_Version_t *const target, FILE* const file)
{
    adi_adrv904x_ErrAction_e recoveryAction  = ADI_ADRV904X_ERR_ACT_NONE;
    /* hash designed to check if data inside the binary is for this type of structure */
    const uint8_t            STRUCT_HASH[16] = { 0xD1, 0xE9, 0xDC, 0x16, 0x3B, 0x18, 0x9F, 0xFD, 0x95, 0xB9, 0xE6, 0x24, 0x39, 0x13, 0xF7, 0xCC };
    char                     buf[16];
    ADI_LIBRARY_MEMSET(&buf, 0, sizeof(buf));
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_RETURN(target);
    ADI_ADRV904X_NULL_PTR_RETURN(file);
    
    if (ADI_LIBRARY_FREAD(buf, 1, 16, file) != 16)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, file, "Unable to read bin file.");
        return recoveryAction;
    }

    if (ADI_LIBRARY_MEMCMP(buf, STRUCT_HASH, 16) != 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, file, "Incorrect structure format written in bin file.");
        return recoveryAction;
    }

    SAFE_READ(recoveryAction, device, target->majorVer, 4U, 1U, file);       // UInt32 at 16
    SAFE_READ(recoveryAction, device, target->minorVer, 4U, 1U, file);       // UInt32 at 20
    SAFE_READ(recoveryAction, device, target->maintenanceVer, 4U, 1U, file); // UInt32 at 24
    SAFE_READ(recoveryAction, device, target->buildVer, 4U, 1U, file);       // UInt32 at 28
    return recoveryAction;
}

static adi_adrv904x_ErrAction_e adrv904x_LoadFwVersionFromBin(adi_adrv904x_Device_t* const device, adi_adrv904x_CpuFwVersion_t* const target, FILE* const file)
{
    adi_adrv904x_ErrAction_e recoveryAction  = ADI_ADRV904X_ERR_ACT_NONE;
    /* hash designed to check if data inside the binary is for this type of structure */
    const uint8_t            STRUCT_HASH[16] = { 0x8D, 0xDA, 0x34, 0xE4, 0x46, 0x71, 0x5D, 0xC8, 0xA4, 0x08, 0x00, 0xC0, 0x2D, 0x59, 0x32, 0x2B };
    char                     buf[16];
    ADI_LIBRARY_MEMSET(&buf, 0, sizeof(buf));
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_RETURN(target);
    ADI_ADRV904X_NULL_PTR_RETURN(file);
    
    if (ADI_LIBRARY_FREAD(buf, 1, 16, file) != 16)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, file, "Unable to read bin file.");
        return recoveryAction;
    }

    if (ADI_LIBRARY_MEMCMP(buf, STRUCT_HASH, 16) != 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, file, "Incorrect structure format written in bin file.");
        return recoveryAction;
    }

    SAFE_READ(recoveryAction, device, target->commVer.majorVer, 4U, 1U, file);         // UInt32 at 16
    SAFE_READ(recoveryAction, device, target->commVer.minorVer, 4U, 1U, file);         // UInt32 at 20
    SAFE_READ(recoveryAction, device, target->commVer.maintenanceVer, 4U, 1U, file);   // UInt32 at 24
    SAFE_READ(recoveryAction, device, target->commVer.buildVer, 4U, 1U, file);         // UInt32 at 28
    SAFE_READ(recoveryAction, device, target->cpuFwBuildType, 4U, 1U, file);           // Int32 at 32
    return recoveryAction;
}

static adi_adrv904x_ErrAction_e adrv904x_LoadInitStructFromBin(adi_adrv904x_Device_t* const device, adi_adrv904x_Init_t* const target, FILE* const file)
{
    adi_adrv904x_ErrAction_e recoveryAction  = ADI_ADRV904X_ERR_ACT_NONE;
    /* hash designed to check if data inside the binary is for this type of structure */

    const uint8_t            STRUCT_HASH[16] = { 0x32, 0xC1, 0xBA, 0xA5, 0x9D, 0xDE, 0x21, 0x12, 0xD7, 0xF6, 0x10, 0xAF, 0xFB, 0x74, 0x2A, 0x41 };
    char                     buf[16];
    uint8_t                  i               = 0U;
    uint8_t                  j               = 0U;
    ADI_LIBRARY_MEMSET(&buf, 0, sizeof(buf));

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_RETURN(target);
    ADI_ADRV904X_NULL_PTR_RETURN(file);
    
    if (ADI_LIBRARY_FREAD(buf, 1, 16, file) != 16)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, file, "Unable to read bin file.");
        return recoveryAction;
    }

    if (ADI_LIBRARY_MEMCMP(buf, STRUCT_HASH, 16) != 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, file, "Incorrect structure format written in bin file.");
        return recoveryAction;
    }

    SAFE_READ(recoveryAction, device, target->spiOptionsInit.allowSpiStreaming, 1U, 1U, file);         // Byte at 16
    SAFE_READ(recoveryAction, device, target->spiOptionsInit.allowAhbAutoIncrement, 1U, 1U, file);     // Byte at 17
    SAFE_READ(recoveryAction, device, target->spiOptionsInit.allowAhbSpiFifoMode, 1U, 1U, file);       // Byte at 18
    SAFE_READ(recoveryAction, device, target->clocks.DevClkOnChipTermResEn, 1U, 1U, file);             // Byte at 19
    SAFE_READ(recoveryAction, device, target->cpuMemDump.filePath, 1U, 256U, file);                    // Byte[256] at 20-275

    for (i = 0U; i < 10U; i++)
    {
        SAFE_READ(recoveryAction, device, target->rx.rxChannelCfg[i].rxDataFormat.formatSelect, 4U, 1U, file);                                // Int32 at 276
        SAFE_READ(recoveryAction, device, target->rx.rxChannelCfg[i].rxDataFormat.floatingPointConfig.fpDataFormat, 4U, 1U, file);            // Int32 at 280
        SAFE_READ(recoveryAction, device, target->rx.rxChannelCfg[i].rxDataFormat.floatingPointConfig.fpRoundMode, 4U, 1U, file);             // Int32 at 284
        SAFE_READ(recoveryAction, device, target->rx.rxChannelCfg[i].rxDataFormat.floatingPointConfig.fpNumExpBits, 4U, 1U, file);            // Int32 at 288
        SAFE_READ(recoveryAction, device, target->rx.rxChannelCfg[i].rxDataFormat.floatingPointConfig.fpAttenSteps, 4U, 1U, file);            // Int32 at 292
        SAFE_READ(recoveryAction, device, target->rx.rxChannelCfg[i].rxDataFormat.floatingPointConfig.fpHideLeadingOne, 4U, 1U, file);        // Int32 at 296
        SAFE_READ(recoveryAction, device, target->rx.rxChannelCfg[i].rxDataFormat.floatingPointConfig.fpEncodeNan, 4U, 1U, file);             // Int32 at 300
        SAFE_READ(recoveryAction, device, target->rx.rxChannelCfg[i].rxDataFormat.integerConfigSettings.intEmbeddedBits, 4U, 1U, file);       // Int32 at 304
        SAFE_READ(recoveryAction, device, target->rx.rxChannelCfg[i].rxDataFormat.integerConfigSettings.intSampleResolution, 4U, 1U, file);   // Int32 at 308
        SAFE_READ(recoveryAction, device, target->rx.rxChannelCfg[i].rxDataFormat.integerConfigSettings.intParity, 4U, 1U, file);             // Int32 at 312
        SAFE_READ(recoveryAction, device, target->rx.rxChannelCfg[i].rxDataFormat.integerConfigSettings.intEmbeddedPos, 4U, 1U, file);        // Int32 at 316
        SAFE_READ(recoveryAction, device, target->rx.rxChannelCfg[i].rxDataFormat.slicerConfigSettings.extSlicerStepSize, 4U, 1U, file);      // Int32 at 320
        SAFE_READ(recoveryAction, device, target->rx.rxChannelCfg[i].rxDataFormat.slicerConfigSettings.intSlicerStepSize, 4U, 1U, file);      // Int32 at 324
        SAFE_READ(recoveryAction, device, target->rx.rxChannelCfg[i].rxDataFormat.slicerConfigSettings.extSlicerGpioSelect, 4U, 1U, file);    // Int32 at 328

        for (j = 0U; j < 4U; j++)
        {
            SAFE_READ(recoveryAction, device, target->rx.rxChannelCfg[i].rxDataFormat.slicerConfigSettings.intSlicerGpioSelect[j], 4U, 1U, file);      // Int32 at 332
        }

        SAFE_READ(recoveryAction, device, target->rx.rxChannelCfg[i].rxDataFormat.embOvldMonitorSettings.embeddedMonitorSrcLsbI, 4U, 1U, file);          // Int32 at 348
        SAFE_READ(recoveryAction, device, target->rx.rxChannelCfg[i].rxDataFormat.embOvldMonitorSettings.embeddedMonitorSrcLsbQ, 4U, 1U, file);          // Int32 at 352
        SAFE_READ(recoveryAction, device, target->rx.rxChannelCfg[i].rxDataFormat.embOvldMonitorSettings.embeddedMonitorSrcLsbPlusOneI, 4U, 1U, file);   // Int32 at 356
        SAFE_READ(recoveryAction, device, target->rx.rxChannelCfg[i].rxDataFormat.embOvldMonitorSettings.embeddedMonitorSrcLsbPlusOneQ, 4U, 1U, file);   // Int32 at 360
        SAFE_READ(recoveryAction, device, target->rx.rxChannelCfg[i].rxDataFormat.embOvldMonitorSettings.embeddedMonitorHb2LowSrcSel, 4U, 1U, file);     // Int32 at 364
        SAFE_READ(recoveryAction, device, target->rx.rxChannelCfg[i].rxDataFormat.embOvldMonitorSettings.embeddedMonitorHb2HighSrcSel, 4U, 1U, file);    // Int32 at 368
        SAFE_READ(recoveryAction, device, target->rx.rxChannelCfg[i].rxDataFormat.embOvldMonitorSettings.embeddedMonitorApdLowSrcSel, 4U, 1U, file);     // Int32 at 372
        SAFE_READ(recoveryAction, device, target->rx.rxChannelCfg[i].rxDataFormat.embOvldMonitorSettings.embeddedMonitorApdHighSrcSel, 4U, 1U, file);    // Int32 at 376
        SAFE_READ(recoveryAction, device, target->rx.rxChannelCfg[i].rxDataFormat.embOvldMonitorSettings.invertHb2Flag, 1U, 1U, file);                   // Byte at 380
        SAFE_READ(recoveryAction, device, target->rx.rxChannelCfg[i].rxDataFormat.embOvldMonitorSettings.invertApdFlag, 1U, 1U, file);                   // Byte at 381
        SAFE_READ(recoveryAction, device, target->rx.rxChannelCfg[i].rxDataFormat.externalLnaGain, 1U, 1U, file);                                        // Byte at 382
        SAFE_READ(recoveryAction, device, target->rx.rxChannelCfg[i].rxDataFormat.tempCompensationEnable, 1U, 1U, file);                                 // Byte at 383
        SAFE_READ(recoveryAction, device, target->rx.rxChannelCfg[i].rxGainIndexInit, 1U, 1U, file);                                                     // Byte at 384
    }

    for (i = 0U; i < 8U; i++)
    {
        SAFE_READ(recoveryAction, device, target->tx.txChannelCfg[i].txAttenCfg.updateCfg.srcCfg.updateSrc, 4U, 1U, file);         // Int32 at 1366
        SAFE_READ(recoveryAction, device, target->tx.txChannelCfg[i].txAttenCfg.updateCfg.srcCfg.s0OrS1Gpio, 4U, 1U, file);        // Int32 at 1370
        SAFE_READ(recoveryAction, device, target->tx.txChannelCfg[i].txAttenCfg.updateCfg.trgCfg.updateTrg, 4U, 1U, file);         // Int32 at 1374
        SAFE_READ(recoveryAction, device, target->tx.txChannelCfg[i].txAttenCfg.updateCfg.trgCfg.triggerGpio, 4U, 1U, file);       // Int32 at 1378
        SAFE_READ(recoveryAction, device, target->tx.txChannelCfg[i].txAttenCfg.txAttenStepSize, 4U, 1U, file);                    // Int32 at 1382
        SAFE_READ(recoveryAction, device, target->tx.txChannelCfg[i].txAttenInit_mdB, 2U, 1U, file);                               // UInt16 at 1386
        SAFE_READ(recoveryAction, device, target->tx.txChannelCfg[i].txpowerMonitorCfg.inputSel, 4U, 1U, file);                    // Int32 at 1388
        SAFE_READ(recoveryAction, device, target->tx.txChannelCfg[i].txpowerMonitorCfg.peakThreshold, 2U, 1U, file);               // UInt16 at 1392
        SAFE_READ(recoveryAction, device, target->tx.txChannelCfg[i].txpowerMonitorCfg.measDuration, 1U, 1U, file);                // Byte at 1394
        SAFE_READ(recoveryAction, device, target->tx.txChannelCfg[i].txpowerMonitorCfg.peakCount, 1U, 1U, file);                   // Byte at 1395
        SAFE_READ(recoveryAction, device, target->tx.txChannelCfg[i].txpowerMonitorCfg.peakErrorClearRequired, 1U, 1U, file);      // Byte at 1396
        SAFE_READ(recoveryAction, device, target->tx.txChannelCfg[i].txpowerMonitorCfg.peakPowerEnable, 1U, 1U, file);             // Byte at 1397
        SAFE_READ(recoveryAction, device, target->tx.txChannelCfg[i].txpowerMonitorCfg.peakPowerIrqEnable, 1U, 1U, file);          // Byte at 1398
        SAFE_READ(recoveryAction, device, target->tx.txChannelCfg[i].txpowerMonitorCfg.avgThreshold, 2U, 1U, file);                // UInt16 at 1399
        SAFE_READ(recoveryAction, device, target->tx.txChannelCfg[i].txpowerMonitorCfg.avgErrorClearRequired, 1U, 1U, file);       // Byte at 1401
        SAFE_READ(recoveryAction, device, target->tx.txChannelCfg[i].txpowerMonitorCfg.avgPowerEnable, 1U, 1U, file);              // Byte at 1402
        SAFE_READ(recoveryAction, device, target->tx.txChannelCfg[i].txpowerMonitorCfg.avgPowerIrqEnable, 1U, 1U, file);           // Byte at 1403
        SAFE_READ(recoveryAction, device, target->tx.txChannelCfg[i].txpowerMonitorCfg.avgPeakRatioEnable, 1U, 1U, file);          // Byte at 1404
        SAFE_READ(recoveryAction, device, target->tx.txChannelCfg[i].srlCfg.inputSel, 4U, 1U, file);                               // Int32 at 1405
        SAFE_READ(recoveryAction, device, target->tx.txChannelCfg[i].srlCfg.srdOffset, 2U, 1U, file);                              // UInt16 at 1409
        SAFE_READ(recoveryAction, device, target->tx.txChannelCfg[i].srlCfg.srdEnable, 1U, 1U, file);                              // Byte at 1411
        SAFE_READ(recoveryAction, device, target->tx.txChannelCfg[i].srlCfg.srdIrqEnable, 1U, 1U, file);                           // Byte at 1412
        SAFE_READ(recoveryAction, device, target->tx.txChannelCfg[i].srlCfg.autoRecoveryWaitTime, 1U, 1U, file);                   // Byte at 1413
        SAFE_READ(recoveryAction, device, target->tx.txChannelCfg[i].srlCfg.autoRecoveryEnable, 1U, 1U, file);                     // Byte at 1414
        SAFE_READ(recoveryAction, device, target->tx.txChannelCfg[i].srlCfg.autoRecoveryDisableTimerWhenTxOff, 1U, 1U, file);      // Byte at 1415
        SAFE_READ(recoveryAction, device, target->tx.txChannelCfg[i].srlCfg.srdStatisticsEnable, 1U, 1U, file);                    // Byte at 1416
        SAFE_READ(recoveryAction, device, target->tx.txChannelCfg[i].srlCfg.srdStatisticsMode, 1U, 1U, file);                      // Byte at 1417
        SAFE_READ(recoveryAction, device, target->tx.txChannelCfg[i].protectionRampCfg.rampDownMask, 4U, 1U, file);                // UInt32 at 1418
        SAFE_READ(recoveryAction, device, target->tx.txChannelCfg[i].protectionRampCfg.altEventClearReqd, 1U, 1U, file);           // Byte at 1422
    }

    for (i = 0U; i < 4U; i++)
    {
        SAFE_READ(recoveryAction, device, target->uart[i].enable, 1U, 1U, file);       // Byte at 1822
        SAFE_READ(recoveryAction, device, target->uart[i].pinSelect, 4U, 1U, file);    // Int32 at 1823
    }

    SAFE_READ(recoveryAction, device, target->gpIntPreInit.gpInt0Mask.lowerMask, 8U, 1U, file);   // UInt64 at 1842
    SAFE_READ(recoveryAction, device, target->gpIntPreInit.gpInt0Mask.upperMask, 8U, 1U, file);   // UInt64 at 1850
    SAFE_READ(recoveryAction, device, target->gpIntPreInit.gpInt1Mask.lowerMask, 8U, 1U, file);   // UInt64 at 1858
    SAFE_READ(recoveryAction, device, target->gpIntPreInit.gpInt1Mask.upperMask, 8U, 1U, file);   // UInt64 at 1866

    SAFE_READ(recoveryAction, device, target->radioCtrlPreInit.radioSequencerSsbSyncGpioCtrl, 4U, 1U, file);
    for (i = 0U; i < ADI_ADRV904X_GPIO_COUNT; i++)
    {
        SAFE_READ(recoveryAction, device, target->radioCtrlPreInit.radioSequencerGpioDigOut[i], 4U, 1U, file);
    }
    
    for (i = 0U; i < ADI_ADRV904X_GPIO_ANALOG_COUNT; i++)
    {
        SAFE_READ(recoveryAction, device, target->radioCtrlPreInit.radioSequencerGpioAnaOut[i], 4U, 1U, file);
    }
    
    SAFE_READ(recoveryAction, device, target->dfeUart0.enableGpioTx, 1U, 1U, file);
    SAFE_READ(recoveryAction, device, target->dfeUart0.enableGpioRx, 1U, 1U, file);
    SAFE_READ(recoveryAction, device, target->dfeUart0.enableGpioCts, 1U, 1U, file);
    SAFE_READ(recoveryAction, device, target->dfeUart0.enableGpioRts, 1U, 1U, file);
    return recoveryAction;
}

static adi_adrv904x_ErrAction_e adrv904x_LoadPostMcsInitStructFromBin(adi_adrv904x_Device_t* const device, adi_adrv904x_PostMcsInit_t* const target, FILE* const file)
{
    adi_adrv904x_ErrAction_e  recoveryAction  = ADI_ADRV904X_ERR_ACT_NONE;
    /* hash designed to check if data inside the binary is for this type of structure */
    const uint8_t STRUCT_HASH[16] = { 0xAC, 0xB7, 0x4F, 0x9A, 0xF2, 0x5E, 0xD2, 0x96, 0xDD, 0xA8, 0xA8, 0x75, 0xAC, 0xDB, 0xA4, 0xF2 };
    char buf[16];
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_RETURN(target);
    ADI_ADRV904X_NULL_PTR_RETURN(file);
    
    if (ADI_LIBRARY_FREAD(buf, 1, 16, file) != 16)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, file, "Unable to read bin file.");
        return recoveryAction;
    }

    if (ADI_LIBRARY_MEMCMP(buf, STRUCT_HASH, 16) != 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, file, "Incorrect structure format written in bin file.");
        return recoveryAction;
    }

    SAFE_READ(recoveryAction, device, target->radioCtrlCfg.txRadioCtrlModeCfg.txEnableMode, 4U, 1U, file);     // Int32 at 16
    SAFE_READ(recoveryAction, device, target->radioCtrlCfg.txRadioCtrlModeCfg.txChannelMask, 4U, 1U, file);    // UInt32 at 20
    SAFE_READ(recoveryAction, device, target->radioCtrlCfg.rxRadioCtrlModeCfg.rxEnableMode, 4U, 1U, file);     // Int32 at 24
    SAFE_READ(recoveryAction, device, target->radioCtrlCfg.rxRadioCtrlModeCfg.rxChannelMask, 4U, 1U, file);    // UInt32 at 28
    SAFE_READ(recoveryAction, device, target->radioCtrlCfg.orxRadioCtrlModeCfg.orxEnableMode, 4U, 1U, file);   // Int32 at 32
    SAFE_READ(recoveryAction, device, target->radioCtrlCfg.orxRadioCtrlModeCfg.orxChannelMask, 4U, 1U, file);  // UInt32 at 36
    SAFE_READ(recoveryAction, device, target->radioCtrlGpioCfg.txEnMapping, 1U, 8U, file);                     // Byte[8] at 40-47
    SAFE_READ(recoveryAction, device, target->radioCtrlGpioCfg.txAltMapping, 1U, 8U, file);                    // Byte[8] at 48-55
    SAFE_READ(recoveryAction, device, target->radioCtrlGpioCfg.rxEnMapping, 1U, 8U, file);                     // Byte[8] at 56-63
    SAFE_READ(recoveryAction, device, target->radioCtrlGpioCfg.rxAltMapping, 1U, 8U, file);                    // Byte[8] at 64-71
    SAFE_READ(recoveryAction, device, target->radioCtrlTxRxEnPinSel, 1U, 1U, file);                            // Byte at 72
    SAFE_READ(recoveryAction, device, target->radioCtrlTxRxEnCfgSel, 1U, 1U, file);                            // Byte at 73
    SAFE_READ(recoveryAction, device, target->gpIntPostInit.gpInt0Mask.lowerMask, 8U, 1U, file);               // UInt64 at 74
    SAFE_READ(recoveryAction, device, target->gpIntPostInit.gpInt0Mask.upperMask, 8U, 1U, file);               // UInt64 at 82
    SAFE_READ(recoveryAction, device, target->gpIntPostInit.gpInt1Mask.lowerMask, 8U, 1U, file);               // UInt64 at 90
    SAFE_READ(recoveryAction, device, target->gpIntPostInit.gpInt1Mask.upperMask, 8U, 1U, file);               // UInt64 at 98
    SAFE_READ(recoveryAction, device, target->initCals.calMask, 8U, 1U, file);                                 // UInt64 at 106
    SAFE_READ(recoveryAction, device, target->initCals.rxChannelMask, 4U, 1U, file);                           // UInt32 at 114
    SAFE_READ(recoveryAction, device, target->initCals.txChannelMask, 4U, 1U, file);                           // UInt32 at 118
    SAFE_READ(recoveryAction, device, target->initCals.orxChannelMask, 4U, 1U, file);                          // UInt32 at 122
    SAFE_READ(recoveryAction, device, target->initCals.warmBoot, 1U, 1U, file);                                // Byte at 126
    
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_LoadBinFile(adi_adrv904x_Device_t* const       device, 
                                                      FILE* const                        file, 
                                                      adi_adrv904x_Version_t* const      apiVer, 
                                                      adi_adrv904x_CpuFwVersion_t* const fwVer, 
                                                      adi_adrv904x_Version_t* const      streamVer, 
                                                      adi_adrv904x_Init_t* const         initStruct, 
                                                      adi_adrv904x_PostMcsInit_t* const  postMcsInitStruct, 
                                                      const uint32_t                     fileOffset)
{
    
    adi_adrv904x_ErrAction_e  recoveryAction  = ADI_ADRV904X_ERR_ACT_NONE;
    uint32_t                  totalFileSize   = 0U;
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, file);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, apiVer);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, fwVer);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, streamVer);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, initStruct);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, postMcsInitStruct);
    
    
    /* Determine file size */
    if (ADI_LIBRARY_FSEEK(file, 0U, SEEK_END) < 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, file, "Seek to EOF Failed");
        return recoveryAction;
    }    
    
    /* ADI_LIBRARY_FTELL returns long type */
    totalFileSize = (uint32_t) ADI_LIBRARY_FTELL(file);
    
    /* Check that FW binary file is not empty */
    if (0U == totalFileSize)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, totalFileSize, "Zero Length CPU Profile Binary Detected");
        return recoveryAction;
    }

    if (ADI_LIBRARY_FSEEK(file, 0U, SEEK_SET) < 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, file, "Could not rewind to start");
        return recoveryAction;
    }
    
    if (ADI_LIBRARY_FSEEK(file, fileOffset, SEEK_SET) < 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, file, "Could not rewind to file Offset");
        return recoveryAction;
    }

    recoveryAction = adrv904x_LoadVersionFromBin(device, apiVer, file);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Unable to read bin file to extract api version");
        return recoveryAction;
    }
    
    recoveryAction = adrv904x_LoadFwVersionFromBin(device, fwVer, file);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Unable to read bin file to extract firmware version");
        return recoveryAction;
    }
    
    recoveryAction = adrv904x_LoadVersionFromBin(device, streamVer, file);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Unable to read bin file to extract stream version");
        return recoveryAction;
    }
    
    recoveryAction = adrv904x_LoadInitStructFromBin(device, initStruct, file);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Unable to read bin file to extract adi_adrv904x_Init_t struct");
        return recoveryAction;
    }
    
    recoveryAction = adrv904x_LoadPostMcsInitStructFromBin(device, postMcsInitStruct, file);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Unable to read bin file to extract adi_adrv904x_PostMcsInit_t struct");
        return recoveryAction;
    }

    return recoveryAction;
}
