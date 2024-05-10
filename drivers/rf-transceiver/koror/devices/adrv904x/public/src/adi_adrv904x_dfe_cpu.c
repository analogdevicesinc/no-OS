/**
* Copyright 2015 - 2023 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_adrv904x_dfe_cpu.c
* \brief Contains CPU features related function implementation defined in
* adi_adrv904x_dfe_cpu.h
*
* ADRV904X API Version: 2.10.0.4
*/


#include "adi_adrv904x_dfe_cpu.h"
#include "adi_adrv904x_dfe_framework_tracking_cal_t.h"
#include "adi_adrv904x_dfe_svc_log_trace_events_t.h"
#include "adi_adrv904x_core.h"
#include "adi_adrv904x_hal.h"
#include "adi_adrv904x_error_type_action.h"
#include "adi_adrv904x_cpu.h"
#include "adi_adrv904x_cpu_sys_types.h"
#include "../../private/include/adrv904x_cpu_cmd_cal_status.h"
#include "../../private/include/adrv904x_cpu_cmd_ctrl.h"
#include "../../private/bf/adrv904x_bf_core_types.h"
#include "../../private/bf/adrv904x_bf_core.h"
#include "../../private/include/adrv904x_init.h"
#include "../../private/include/adrv904x_dfe_framework_cmd_t.h"
#include "../../private/include/adrv904x_dfe_memory.h"
#include "../../private/include/adrv904x_dfe_svc_bbic_bridge_t.h"
#include "../../private/include/adrv904x_cpu_scratch_registers.h"
#include "../../private/include/adrv904x_dfe_svc_err_dfe_boot_status_t.h"
#include "../../private/include/adrv904x_cpu.h"
#include "../../private/include/adrv904x_gpio.h"
#include "../../private/include/adrv904x_dfe_cpu.h"
#include "../../private/include/adrv904x_cpu_dfe_init_memory.h"
#include "../../private/include/adrv904x_cpu_archive_types.h"
#include "../../private/include/adrv904x_dfe_memory.h"
#include "../../private/include/adrv904x_radioctrl.h"
#include "../../private/include/adrv904x_dfe_svc_cmd_t.h"
#include "../../private/include/adrv904x_dfe_framework_cmd_sys_status_t.h"
#include "../../private/include/adrv904x_dfe_svc_bbic_bridge_t.h"


#define ADI_FILE    ADI_ADRV904X_FILE_PUBLIC_DFE_CPU
#define ADI_ADRV904X_CPU_BINARY_IMAGE_LOAD_HEADER_BYTES (sizeof(uint32_t) + sizeof(adrv904x_CpuArchiveHeader_t) )
#define ADRV904X_DFE_CPU_CRC_SEED 0xFFFFFFFFU

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeLoadGet(   adi_adrv904x_Device_t* const        device,
                                                            adi_adrv904x_DfeCpuLoad_t* const    pDfeLoad)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_DfeSvcCmdCpuAndMemLoadGetResp_t respStruct;
    adrv904x_DfeSvcCmdStatus_t cmdStatus = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, pDfeLoad, cleanup);

    ADI_LIBRARY_MEMSET(pDfeLoad, 0, sizeof(adi_adrv904x_DfeCpuLoad_t));
    ADI_LIBRARY_MEMSET(&respStruct, 0, sizeof(respStruct));

    /* Send command to DFE */
    recoveryAction = adrv904x_DfeSvcCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_DFE_SVC_CMD_ID_CPU_LOAD_GET,
                                            0U,
                                            0U,
                                            (void *)&respStruct,
                                            sizeof(adrv904x_DfeSvcCmdCpuAndMemLoadGetResp_t),
                                            &cmdStatus);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "DfeSvcCmdSend - DFE Load info get");
        goto cleanup;
    }

    if (respStruct.status != ADI_ADRV904X_DFE_SVC_ERR_CODE_NO_ERROR)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, respStruct.status, "DFE load info status");
        goto cleanup;
    }

    /* Copy response parameters to pDfeLoadInfo */
    ADI_LIBRARY_MEMCPY((adi_adrv904x_DfeCpuLoad_t*)pDfeLoad, (adi_adrv904x_DfeCpuLoad_t*)&(respStruct.load.cpuLoad), sizeof(adi_adrv904x_DfeCpuLoad_t));

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeCpuAndMemLoadGet(  adi_adrv904x_Device_t* const            device,
                                                                    adi_adrv904x_DfeCpuAndMemLoad_t* const  pDfeLoad)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_DfeSvcCmdCpuAndMemLoadGetResp_t respStruct;
    adrv904x_DfeSvcCmdStatus_t cmdStatus = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, pDfeLoad, cleanup);

    ADI_LIBRARY_MEMSET(pDfeLoad, 0, sizeof(adi_adrv904x_DfeCpuAndMemLoad_t));
    ADI_LIBRARY_MEMSET(&respStruct, 0, sizeof(respStruct));

    /* Send command to DFE */
    recoveryAction = adrv904x_DfeSvcCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_DFE_SVC_CMD_ID_CPU_LOAD_GET,
                                            0U,
                                            0U,
                                            (void *)&respStruct,
                                            sizeof(adrv904x_DfeSvcCmdCpuAndMemLoadGetResp_t),
                                            &cmdStatus);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "DfeSvcCmdSend - DFE Load info get");
        goto cleanup;
    }

    if (respStruct.status != ADI_ADRV904X_DFE_SVC_ERR_CODE_NO_ERROR)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, respStruct.status, "DFE load info status");
        goto cleanup;
    }

    /* Copy response parameters to pDfeLoadInfo */
    ADI_LIBRARY_MEMCPY((adi_adrv904x_DfeCpuAndMemLoad_t*)pDfeLoad, (adi_adrv904x_DfeCpuAndMemLoad_t*)&(respStruct.load), sizeof(adi_adrv904x_DfeCpuAndMemLoad_t));

    /* Convertion between BE and LE */
    pDfeLoad->memAvailable = ADRV904X_CTOHL(pDfeLoad->memAvailable);
    pDfeLoad->memAllocated = ADRV904X_CTOHL(pDfeLoad->memAllocated);

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeCpuStart(adi_adrv904x_Device_t* const device)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    static const uint32_t A55_CONFIG_SIGNAL_ADDRESS = 0xb2220000U;
    static const uint32_t CLK_RST_CTRL_CFG_ADDRESS  = 0xb2210000U;

    static const uint32_t CPU_RVBARADDR0 = 0xb2220020U;
    static const uint32_t CPU_RVBARADDR1 = 0xb2220040U;
    static const uint32_t CPU_RVBARADDR2 = 0xb2220060U;
    static const uint32_t CPU_RVBARADDR3 = 0xb2220080U;

    uint32_t a55CfgSignalRegVal  = 0xF0FU; /* 111100001111 : 64 bit architecture, others default */
    uint32_t resetVectorAddress = ADRV904X_DFE_CODE_MEM_REGION_START;

    uint32_t clkRstCtrlCfgRegVal = 0x280001U;
    uint32_t READ_WRITE_MASK = 0xFFFFFFFFU;
    
    /* Check if M4 radio processor is up and running */
    recoveryAction = adi_adrv904x_CpuStartStatusCheck(device, ADI_ADRV904X_GETCPUBOOTUP_TIMEOUT_US);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Radio Processor Need to be up and running before DFE processor Boot.");
        goto cleanup;
    }

    /* MMR Configuration: A55_CONFIG_SIGNAL_ADDRESS : Select 64 bit architecture, rest all default 111100001111

    Bits        Name                Description                                                                 Reset
    31:24       CLUSTERIDAFF3       Value read in ClusterID Affinity Level-3 field, MPIDR bits[39:32].          'h00
    23:16       CLUSTERIDAFF2       Value read in ClusterID Affinity Level-2 field, MPIDR bits[23:16].          'h00
    15:12       VINITHI             Enable high exception vectors,0:Starts at 0x00000000;1:Starts at 0xFFFF0000.'h0
    11:08       CFGTE               Enable Thumb exceptions, 0 TE is LOW, 1 TE is HIGH                          'hf
    7:04        CFGEND              Endianness configuration .0 EE bit is LOW; 1 EE bit is HIGH.                'h0
    3:00        AA64NAA32           Register width state. The options are: 0 AArch32; 1 AArch64.                'hF
    */

    recoveryAction = adi_adrv904x_Register32Write(device, NULL, A55_CONFIG_SIGNAL_ADDRESS, a55CfgSignalRegVal, READ_WRITE_MASK);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Unable to write to DFE A55_CONFIG_SIGNALS");
        goto cleanup;
    }

    /* Set reset vector base address for all cores in registers RVBARADDR0/1/2/3 */
    /* Core 0 */
    recoveryAction = adi_adrv904x_Register32Write(device, NULL, CPU_RVBARADDR0, resetVectorAddress, READ_WRITE_MASK);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Unable to write to DFE RVBARADDR0");
        goto cleanup;
    }

    /* Core 1 */
    recoveryAction = adi_adrv904x_Register32Write(device, NULL, CPU_RVBARADDR1, resetVectorAddress, READ_WRITE_MASK);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Unable to write to DFE RVBARADDR1");
        goto cleanup;
    }

    /* Core 2 */
    recoveryAction = adi_adrv904x_Register32Write(device, NULL, CPU_RVBARADDR2, resetVectorAddress, READ_WRITE_MASK);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Unable to write to DFE RVBARADDR2");
        goto cleanup;
    }

    /* Core 3 */
    recoveryAction = adi_adrv904x_Register32Write(device, NULL, CPU_RVBARADDR3, resetVectorAddress, READ_WRITE_MASK);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Unable to write to DFE RVBARADDR3");
        goto cleanup;
    }

    recoveryAction = adi_adrv904x_Register32Write(device, NULL, CLK_RST_CTRL_CFG_ADDRESS, clkRstCtrlCfgRegVal, READ_WRITE_MASK);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Unable to write to DFE CLK_RST_CTRL_CFG");
        goto cleanup;
    }
cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeCpuImageLoad(adi_adrv904x_Device_t* const                  device,
                                                              const adi_adrv904x_DfeCpuBinaryInfo_t* const  cpuDfeBinaryInfo)
{
        adi_adrv904x_ErrAction_e  recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    FILE*                     cpuImageFilePtr     = NULL;
    uint32_t                  fileSize            = 0U;
    uint32_t                  byteCount           = 0U;
    uint32_t                  offset              = 0U;
    uint32_t                  fileCRC             = 0U;
    uint32_t            crc = ADRV904X_DFE_CPU_CRC_SEED;
    
    uint8_t streamingEnabled = 0U;
    uint8_t cpuImageChunk[ADI_ADRV904X_CPU_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES] = { 0 };

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cpuDfeBinaryInfo, cleanup);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cpuDfeBinaryInfo->filePath, cleanup);
    
    recoveryAction = adrv904x_PackagingInformationValidate(device, cpuDfeBinaryInfo);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Cannot validate DFE package");
        goto cleanup;
    }
    
    if (ADI_LIBRARY_STRNLEN((const char*)cpuDfeBinaryInfo->filePath, ADI_ADRV904X_MAX_FILE_LENGTH) == ADI_ADRV904X_MAX_FILE_LENGTH)
    {
        /* Path is not terminated */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuDfeBinaryInfo->filePath, "Unterminated path string");
        goto cleanup;
    }

    /* Open CPU archive file */
#ifdef __GNUC__
    cpuImageFilePtr = ADI_LIBRARY_FOPEN((const char *)cpuDfeBinaryInfo->filePath, "rb");
#else
    if (ADI_LIBRARY_FOPEN_S(&cpuImageFilePtr, (const char *)cpuDfeBinaryInfo->filePath, "rb") != 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuDfeBinaryInfo->filePath, "Invalid Binary File or Path Detected");
        goto cleanup;
    }
#endif
    if (NULL == cpuImageFilePtr)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuDfeBinaryInfo->filePath, "Invalid Binary File or Path Detected");
        goto cleanup;
    }

    /* Determine file size */
    if (ADI_LIBRARY_FSEEK(cpuImageFilePtr, 0U, SEEK_END) != 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuDfeBinaryInfo->filePath, "Seek to EOF Failed");
        goto cleanup;
    }

    /* ADI_LIBRARY_FTELL returns long type */
    fileSize = (uint32_t) ADI_LIBRARY_FTELL(cpuImageFilePtr);

    /* Check if DFE Cpu Image File is Empty */
    if (fileSize == 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuDfeBinaryInfo->filePath, "Zero Length DFE Cpu Image Detected");
        goto cleanup;
    }

    /* Check that size of the file is a multiple of 4 */
    if ((fileSize & 0x3U) != 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuDfeBinaryInfo->filePath, "Incorrect DFE Cpu Binary File Block Size");
        goto cleanup;
    }
    
    /* Rewind the file pointer to beginning of the file */
    if (ADI_LIBRARY_FSEEK(cpuImageFilePtr,0L, SEEK_SET) < 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuDfeBinaryInfo->filePath, "Unable to Rewind File Pointer for DFE Cpu Binary");
        goto cleanup;
    }
    
    /* read fileCRC */
    byteCount = ADI_LIBRARY_FREAD(&fileCRC, 1, sizeof(fileCRC), cpuImageFilePtr);
    if ( byteCount != sizeof(fileCRC) )
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuDfeBinaryInfo->filePath, "Error reading File CRC from CPU image file");
        goto cleanup;
    }
    fileCRC = ADRV904X_CTOHL( fileCRC );
    
    /* Read the header */
    byteCount = ADI_LIBRARY_FREAD(&cpuImageChunk[0U], 1, sizeof(adrv904x_CpuArchiveHeader_t), cpuImageFilePtr);
    if (byteCount != sizeof(adrv904x_CpuArchiveHeader_t) )
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuDfeBinaryInfo->filePath, "Unable to Read Header from CPU image file");
        goto cleanup;
    }

    crc = adrv904x_DfeCrc32Chunk( &cpuImageChunk[0U], byteCount, crc, 0 );

    /* Put the part into streaming mode to help facilitate the large, contiguous SPI writes */
    recoveryAction = adrv904x_SpiStreamingEntry(device);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue with entering Spi Streaming mode");
        goto cleanup;
    }
    streamingEnabled = 1U;
    
    /* enable ahb spi bridge  */
    recoveryAction = adrv904x_Core_AhbSpiBridgeEnable_BfSet(device,
                                                            NULL,
                                                            ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                            ADI_ENABLE);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue while attempting to set ahb spi bridge enable bit");
        goto cleanup;
    }

    /* Load DFE CPU image */
    offset = ADRV904X_DFE_CODE_MEM_REGION_START;
    fileSize -= ADI_ADRV904X_CPU_BINARY_IMAGE_LOAD_HEADER_BYTES;
    while( fileSize > 0) 
    {
        uint32_t blockSize = ( ADI_ADRV904X_CPU_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES < fileSize)? ADI_ADRV904X_CPU_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES : fileSize;
        byteCount = ADI_LIBRARY_FREAD(&cpuImageChunk[0U], 1, blockSize, cpuImageFilePtr);
        if (byteCount != blockSize)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuDfeBinaryInfo->filePath, "Error reading from CPU image file");
            goto cleanup;
        }
        /* SPI write the chunk to memory */
        recoveryAction = adi_adrv904x_RegistersByteWrite(device, NULL, offset, &cpuImageChunk[0U], blockSize);
        offset += blockSize;
        fileSize -= blockSize;
        crc = adrv904x_DfeCrc32Chunk( &cpuImageChunk[0U], byteCount, crc, (fileSize == 0) ? 1:0 );
    }


    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error loading DFE CPU image");
        goto cleanup;
    }
    /* check the file CRC */
    if ( fileCRC != crc )
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, crc, "DFE File CRC is invalid");
        goto cleanup;
    }

    /* Load successful. Update device state. */
    device->devStateInfo.devState = (adi_adrv904x_ApiStates_e)(device->devStateInfo.devState | ADI_ADRV904X_STATE_DFECPULOADED);

    /* Close CPU Image File */
    if (0 == ADI_LIBRARY_FCLOSE(cpuImageFilePtr))
    {
        cpuImageFilePtr = NULL;
    }
    else
    {
        cpuImageFilePtr = NULL;
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuDfeBinaryInfo->filePath, "Cannot Close CPU Image File");
        goto cleanup;
    }

cleanup:

    if (cpuImageFilePtr != NULL)
    {
        /* Close CPU Image File */
        if (0 != ADI_LIBRARY_FCLOSE(cpuImageFilePtr))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Cannot Close DFE CPU Image File");
        }
    }

    if (streamingEnabled)
    {
        if (ADI_ADRV904X_ERR_ACT_NONE != adrv904x_SpiStreamingExit(device))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_RESET_INTERFACE;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Spi Streaming Exit Issue");
        }
    }
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeCpuStartStatusCheck(adi_adrv904x_Device_t* const   device,
                                                                     const uint32_t                 timeout_us)
{
        adi_adrv904x_ErrAction_e            recoveryAction  = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_DfeSvcErrDfeBootStatus_e   bootStatus      = ADRV904X_DFE_SVC_ERR_DFE_BOOT_STATUS_POWER_UP;
    uint32_t                            waitInterval_us = 0U;
    uint32_t                            numEventChecks  = 1U;
    uint32_t                            eventCheck      = 0U;
    uint32_t                            readyForReload  = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    /* Determine the number of reads equivalent to the requested timeout */
    waitInterval_us = (ADI_ADRV904X_DFE_CPUBOOTUP_INTERVAL_US > timeout_us) ?
        timeout_us : ADI_ADRV904X_DFE_CPUBOOTUP_INTERVAL_US;
    numEventChecks = (waitInterval_us == 0U) ? 1U : (timeout_us / waitInterval_us);

    /* Note: Waits for the initial boot thread to complete. SDK services are expected
     * to be up, but any threads spun up from the initial boot thread, such as the thread
     * containing application_init() may or may not have completed by the time status = READY.
     */
    for (eventCheck = 0U; eventCheck < numEventChecks; ++eventCheck)
    {
        /* Get the CPU boot status */
        recoveryAction =  adrv904x_Core_ScratchReg_BfGet(device,
                                                         NULL,
                                                         ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                         ADRV904X_DFE_BOOT_STATUS_SCRATCH_REG_ID,
                                                         (uint8_t*) &bootStatus);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "bootStatus read failed");
            goto cleanup;
        }
        if (bootStatus == ADRV904X_DFE_SVC_ERR_DFE_BOOT_STATUS_FW_CRC_ERR ||
            bootStatus == ADRV904X_DFE_SVC_ERR_DFE_BOOT_STATUS_UNEXPECTED_ERR)
        {
            /* Known error conditions. No need to continue to wait. Error out now. */
            ADI_ADRV904X_ERROR_INFO_GET_REPORT( ADI_ADRV904X_ERRSRC_DFE_CPU_BOOT,
                                                bootStatus,
                                                ADI_NO_VARIABLE,
                                                recoveryAction);

            /* DFE boot error */
            ADI_ADRV904X_ERROR_INFO_GET_REPORT( ADI_ADRV904X_ERRSRC_DFE_CPU,
                                                ADI_ADRV904X_ERRCODE_DFE_CPU_BOOT_ERROR,
                                                ADI_NO_VARIABLE,
                                                recoveryAction);
            goto cleanup;
        }
        else if (bootStatus == ADRV904X_DFE_SVC_ERR_DFE_BOOT_STATUS_READY)
        {
            /* CPU booted successfully to 'normal' mode */
            break;
        }
        else if (bootStatus == ADRV904X_DFE_SVC_ERR_DFE_BOOT_STATUS_READY_FOR_RELOAD) 
        {
            /* If boot status is READY FOR RELOAD, exit early and let timeout error reporting return the boot status */
            readyForReload = 1U;
            break;
        }
        else
        {
            /* Wait interval then check again */
            recoveryAction = (adi_adrv904x_ErrAction_e) adi_common_hal_Wait_us(&device->common, waitInterval_us);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Wait failed");
                goto cleanup;
            }
            continue;
        }
    }

    if ((eventCheck >= numEventChecks) ||
        (readyForReload == 1U))
    {
        /* Last boot state */
        ADI_ADRV904X_ERROR_INFO_GET_REPORT( ADI_ADRV904X_ERRSRC_DFE_CPU_BOOT,
                                            bootStatus,
                                            bootStatus,
                                            recoveryAction);

        /* Timeout message */
        ADI_ADRV904X_ERROR_INFO_GET_REPORT( ADI_ADRV904X_ERRSRC_DFE_CPU,
                                            ADI_ADRV904X_ERRCODE_DFE_CPU_BOOT_TIMEOUT,
                                            ADI_NO_VARIABLE,
                                            recoveryAction);

        adrv904x_CpuErrorDebugCheck(device);

        adrv904x_DfeCpuErrorDebugCheck(device);

        goto cleanup;
    }


cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeCpuCheckException(adi_adrv904x_Device_t* const    device,
                                                                   uint32_t* const                 isException)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t address = 0U;
    uint32_t coreIdx = 0U;
    adrv904x_DfeSvcCmdDfeExceptionData_t exceptionData;

    /* Verify inputs */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, isException, cleanup);
    
    /* Initialize output, local struct */
    *isException = 0U;
    ADI_LIBRARY_MEMSET(&exceptionData, 0, sizeof(exceptionData));
    
    /* Get DFE exception data address from CPU */
    recoveryAction = adrv904x_DfeSdkDataAddrGet(device, &address);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE exception data address");
        goto cleanup;
    }    
    address += ADI_LIBRARY_OFFSETOF(adrv904x_DfeSvcBbicBridgeSdkData_t, dfeExceptionSnapshotData);
 
    /* Read the exception data */
    recoveryAction = adi_adrv904x_Registers32Read(device, NULL, address, (uint32_t*)&exceptionData, NULL, sizeof(adrv904x_DfeSvcCmdDfeExceptionData_t) / sizeof(uint32_t));
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE exception data registers");
        goto cleanup;
    }
    
    /* Analyse exception data for each DFE core */
    for (coreIdx = 0; coreIdx < ADRV904X_DFE_PLATFORM_NUM_CORES; coreIdx++)
    {
        if (exceptionData.ExceptionFlag[coreIdx] != (uint32_t)ADRV904X_DFE_PLATFORM_NO_EXCEPTION)
        {
            *isException |= (1U << coreIdx);
        }
    }
cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfePrepareLoad(adi_adrv904x_Device_t* const   device,
                                                             const uint8_t                  reload)
{
        adi_adrv904x_ErrAction_e  recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    static const uint32_t CLK_RST_CTRL_CFG_ADDRESS  = 0xb2210000U;
    static const uint32_t A55_MISC_ADDRESS          = 0xb22200a0U;
    static const uint32_t L4CTL_CFG_INIT_START      = 0xb0800024U;
    static const uint32_t L4CTL_CFG_INIT_END        = 0xb0b00024U;
    static const uint32_t L4CTL_BANK_OFFSET         = 0x100000U;       /* Offset between L4CTL Bank addresses */
    static const uint32_t READ_WRITE_MASK           = 0xFFFFFFFFU;
    static const uint32_t L4_INIT_BANK_MASK         = 0xFFU;
    static const uint32_t L4_INIT_TIME_US           = 1000U;

    uint8_t  dummyWrite          = 0U;
    uint32_t i                   = 0U;
    uint32_t clkRstCtrlCfgRegVal = 0x280000U; /* TRACECLK divider value CLK_RST_CTRL_CFG[21:19] */
    uint32_t a55MiscRegVal       = 0U;

    adrv904x_DfeSvcCmdPrepareReload_t prepReload         = { 0U };
    adrv904x_DfeSvcCmdPrepareReloadResp_t prepReloadResp = { ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_EVENT_UNEXPECTED };
    adrv904x_DfeSvcCmdStatus_t cmdStatus = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;
    adi_adrv904x_DfeSvcErrCode_e cpuErrorCode = ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_EVENT_UNEXPECTED;
    uint32_t dfeSwBootFlagAddr                            = ADRV904X_DFE_CPU_INIT_ADDR_GET(swBootFlag);
    adrv904x_DfeSvcBbicBridgeSdkBootFlag_t dfeCpuBootFlag = ADRV904X_DFE_SVC_BBIC_BRIDGE_SDK_BOOT_FLAG_DEFAULT;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    if (reload > 0U)
    {
        /* Send SDK command to prepare for reload. Receive response */
        recoveryAction = adrv904x_DfeSvcCmdSend(device,
                                                ADRV904X_LINK_ID_0,
                                                ADRV904X_DFE_SVC_CMD_ID_PREPARE_RELOAD,
                                                (void*)&prepReload,
                                                sizeof(adrv904x_DfeSvcCmdPrepareReload_t),
                                                (void*)&prepReloadResp,
                                                sizeof(adrv904x_DfeSvcCmdPrepareReloadResp_t),
                                                &cmdStatus);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_ADRV904X_DFE_SVC_CMD_RESP_CHECK_GOTO(prepReloadResp.cmdStatus, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
        }
        
        /* Override Init Boot Flag as BBIC Reload */
        dfeCpuBootFlag = ADRV904X_DFE_SVC_BBIC_BRIDGE_SDK_BOOT_FLAG_BBIC_RELOAD;
    }

    recoveryAction = adrv904x_Core_A55SysclkEnable_BfSet(device, NULL, ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS, 1U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue while attempting to set a55 sys clk enable");
        goto cleanup;
    }

    /* Kill a55 clock by setting GLOBAL_A55_CLK_EN bit to zero */
    clkRstCtrlCfgRegVal &= ~(1U << 0U);
    recoveryAction = adi_adrv904x_Register32Write(device, NULL, CLK_RST_CTRL_CFG_ADDRESS, clkRstCtrlCfgRegVal, READ_WRITE_MASK);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Unable to write to CLK_RST_CTRL_CFG");
        goto cleanup;
    }

    /* Setup A55 clock divider ratios */
    recoveryAction = adrv904x_Core_A55CclkDivideRatio_BfSet(device, NULL, ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS, 0U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue while attempting to set a55 Clk Divide Ratio");
        goto cleanup;
    }

    recoveryAction = adrv904x_Core_A55CclkDivideRatioDevClk_BfSet(device, NULL, ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS, 0U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue while attempting to set a55 Clk Divide Ratio dev clk");
        goto cleanup;
    }

    /* warm reset via CLK_RST_CTRL_CFG register bit3 (GLOBAL_WARM_SW_RST)*/
    clkRstCtrlCfgRegVal |= (1U << 3U);

    recoveryAction = adi_adrv904x_Register32Write(device, NULL, CLK_RST_CTRL_CFG_ADDRESS, clkRstCtrlCfgRegVal, READ_WRITE_MASK);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Unable to write to CLK_RST_CTRL_CFG");
        goto cleanup;
    }

    /* Perform 3 dummy reads to pass time waiting for clocks to settle */
    for (i = 0U; i < 3U; i++)
    {
        recoveryAction = adrv904x_Core_ScratchPadWord_BfSet(device, NULL, (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR, dummyWrite);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Dummy Readback issue");
            goto cleanup;
        }
    }

    clkRstCtrlCfgRegVal = 0x28FFB8U;

    recoveryAction = adi_adrv904x_Register32Write(device, NULL, CLK_RST_CTRL_CFG_ADDRESS, clkRstCtrlCfgRegVal, READ_WRITE_MASK);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Unable to write to CLK_RST_CTRL_CFG");
        goto cleanup;
    }

    /* TODO: Assert common reset of sysClk sw rst bit in digital register (NOT YET DEFINED IN REGMAP)*/

    /* Configure A55_CONFIG.A55_MISC register to define the L4 memory view. */

    /* Set bit 16 (arb_scheme_a55_periph_fabric)*/
    a55MiscRegVal = (1U << 16U);

    recoveryAction = adi_adrv904x_Register32Write(device, NULL, A55_MISC_ADDRESS, a55MiscRegVal, READ_WRITE_MASK);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Unable to write to A55_MISC_ADDRESS");
        goto cleanup;
    }

    /* zero fill each L4 memory cfg addresses*/
    for (i = L4CTL_CFG_INIT_START; i <= L4CTL_CFG_INIT_END; i += L4CTL_BANK_OFFSET)
    {
        /* clear bits for banks 0-7 */
        recoveryAction = adi_adrv904x_Register32Write(device, NULL, i, 0U, READ_WRITE_MASK);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Unable to write to L4CTL_CFG_INIT");
            goto cleanup;
        }
    }
    for (i = L4CTL_CFG_INIT_START; i <= L4CTL_CFG_INIT_END; i += L4CTL_BANK_OFFSET)
    {
        /* set bits for banks 0-7 */
        recoveryAction = adi_adrv904x_Register32Write(device, NULL, i, L4_INIT_BANK_MASK, READ_WRITE_MASK);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Unable to write to L4CTL_CFG_INIT");
            goto cleanup;
        }
    }

    /* Give L4 RAMs time to initialize*/
    recoveryAction = (adi_adrv904x_ErrAction_e) adi_common_hal_Wait_us(&device->common, L4_INIT_TIME_US);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "HAL Wait Request Issue");
        goto cleanup;
    }
    
    /* Set init struct boot flag to indicate standard load vs. reload */
    recoveryAction = adi_adrv904x_RegistersByteWrite(device, NULL, dfeSwBootFlagAddr, &dfeCpuBootFlag, 1U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Cannot write sw test flag to DFE memory");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeCpuReload(adi_adrv904x_Device_t* const                 device,
                                                           const adi_adrv904x_DfeCpuBinaryInfo_t* const cpuDfeBinaryInfo)
{
        adi_adrv904x_ErrAction_e recoveryAction                 = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_DfeSvcErrDfeBootStatus_e powerUpBootStatus     = ADRV904X_DFE_SVC_ERR_DFE_BOOT_STATUS_POWER_UP;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cpuDfeBinaryInfo, cleanup);
    
    /* Prepare for Load */
    recoveryAction = adi_adrv904x_DfePrepareLoad(device, 1);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "DFE processor reload failed during prepare for load.");
        goto cleanup;
    }

    /* Load Images for all the DFE CPU cores*/
    recoveryAction = adi_adrv904x_DfeCpuImageLoad(device, cpuDfeBinaryInfo);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "DFE processor reload failed during image load.");
        goto cleanup;
    }

    /* Reload the radio sequencer pattern table.
     * This will do nothing if the radio sequencer is not enabled.
     */
    recoveryAction = adrv904x_RadioSequencerDfePtrnLookupTableSet(device);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to reload radio sequencer pattern lookup table for DFE.");
        goto cleanup;
    }
    
    /* Set the CPU boot status to POWERUP */
    recoveryAction =  adrv904x_Core_ScratchReg_BfSet(device,
                                                     NULL,
                                                     ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                     ADRV904X_DFE_BOOT_STATUS_SCRATCH_REG_ID,
                                                     (uint8_t) powerUpBootStatus);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "bootStatus set failed");
        goto cleanup;
    }
    
    /* Start DFE CPU core(s) in broadcast mode*/
    recoveryAction = adi_adrv904x_DfeCpuStart(device);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "DFE processor reload failed during start.");
        goto cleanup;
    }
    
    /* Check DFE processor boot status*/
    recoveryAction = adi_adrv904x_DfeCpuStartStatusCheck(device, ADI_ADRV904X_DFE_CPUBOOTUP_TIMEOUT_US);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "DFE processor reload failed.");
        goto cleanup;
    }
cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeAppCommand(adi_adrv904x_Device_t* const    device,
                                                            const uint8_t                   command,
                                                            const uint8_t                   payload[],
                                                            const uint32_t                  payload_length,
                                                            const uint8_t                   response[],
                                                            const uint32_t                  response_length)
{
        adi_adrv904x_ErrAction_e                        recoveryAction                                      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t                                         txBuf[sizeof(adrv904x_CpuCmd_SetDfeConfigMaxSize_t)];
    uint8_t                                         rxBuf[sizeof(adrv904x_CpuCmd_SetDfeConfigMaxSize_t)];
    adrv904x_DfeSvcCmdStatus_t                      cmdStatus                                           = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;
    adi_adrv904x_DfeSvcErrCode_e                    cpuErrorCode                                        = ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_EVENT_UNEXPECTED;
    adrv904x_DfeAppFrameworkCmdGenericResp_t* const cmdRsp                                              = (adrv904x_DfeAppFrameworkCmdGenericResp_t*)&rxBuf;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, payload, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, response, cleanup);

    /* payload pointer/length mismatch check is performed in adrv904x_CpuCmdSend */
    if (payload_length > sizeof(txBuf))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, payload_length, "payload_length is larger than transmit buffer size");
        goto cleanup;
    }

    /* response pointer/length mismatch check is performed in adrv904x_CpuCmdSend */
    if (response_length > sizeof(rxBuf))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, response_length, "response_length is larger than receive buffer size");
        goto cleanup;
    }

    ADI_LIBRARY_MEMSET(&txBuf[0U], 0, sizeof(txBuf));
    ADI_LIBRARY_MEMSET(&rxBuf[0U], 0, sizeof(rxBuf));
    ADI_LIBRARY_MEMCPY((void*)((uint8_t*)txBuf), payload, payload_length);

    /* Send command and receive response */
    recoveryAction = adrv904x_DfeAppCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            command,
                                            (void*)txBuf,
                                            payload_length,
                                            (void*)&rxBuf,
                                            sizeof(rxBuf),
                                            &cmdStatus);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_DFE_APP_CMD_RESP_CHECK_GOTO(cmdRsp->status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

    /* copy the command response back to the caller's buffer */
    ADI_LIBRARY_MEMCPY((void *)response, (void *)rxBuf, response_length);
cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeTrackingCalsEnableSet(adi_adrv904x_Device_t* const                         device,
                                                                       const uint32_t                                       calMask,
                                                                       const uint32_t                                       channelMask,
                                                                       const adi_adrv904x_DfeAppTrackingCalEnableDisable_t  enableDisableFlag)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_FEATURE;
    adrv904x_DfeSvcCmdStatus_t cmdStatus = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;
    adi_adrv904x_DfeSvcErrCode_e cpuErrorCode = ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_EVENT_UNEXPECTED;

    adrv904x_DfeAppFrameworkCmdAppSetEnabledCals_t setTrackCal;
    adrv904x_DfeAppFrameworkCmdAppSetEnabledCalsResp_t setTrackCalRsp;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_LIBRARY_MEMSET(&setTrackCal, 0, sizeof(adrv904x_DfeAppFrameworkCmdAppSetEnabledCals_t));
    ADI_LIBRARY_MEMSET(&setTrackCalRsp, 0, sizeof(adrv904x_DfeAppFrameworkCmdAppSetEnabledCalsResp_t));

    setTrackCal.calMask = ADRV904X_HTOCL(calMask);
    setTrackCal.channelMask = ADRV904X_HTOCL(channelMask);
    setTrackCal.bEnable = (bool)enableDisableFlag;

    /* validate input parameters */
    if ((enableDisableFlag != ADI_ADRV904X_TRACKING_CAL_DISABLE) && (enableDisableFlag != ADI_ADRV904X_TRACKING_CAL_ENABLE))
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, enableDisableFlag, "Invalid enableDisableFlag provided.");
        goto cleanup;
    }

    if (calMask == 0U)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, calMask, "Invalid calMask provided.");
        goto cleanup;
    }

    /* channelMask == 0 is a valid input for system calibrations (e.g. SERDES cal), so it is not checked. */
    if ((channelMask & ~0xFFU) != 0U)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channelMask, "Invalid channelMask provided.");
        goto cleanup;
    }

    /* Send command and receive response */
    recoveryAction = adrv904x_DfeAppCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_DFE_APP_FRAMEWORK_CMD_TRACKINGCAL_ENABLEDISABLE,
                                            (void*)&setTrackCal,
                                            sizeof(adrv904x_DfeAppFrameworkCmdAppSetEnabledCals_t),
                                            (void*)&setTrackCalRsp,
                                            sizeof(adrv904x_DfeAppFrameworkCmdAppSetEnabledCalsResp_t),
                                            &cmdStatus);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_DFE_APP_CMD_RESP_CHECK_GOTO(setTrackCalRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }
cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeTrackingCalsEnableGet(adi_adrv904x_Device_t* const                                 device,
                                                                       adi_adrv904x_DfeAppFrameworkTrackingCalEnableMasks_t* const  enableMasks)
{
        uint32_t channelIdx = 0U;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_FEATURE;
    adrv904x_DfeSvcCmdStatus_t cmdStatus = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;
    adi_adrv904x_DfeAppErrCode_e cpuErrorCode = ADI_ADRV904X_DFE_APP_ERR_CODE_NO_ERROR;
    adrv904x_DfeAppFrameworkCmdAppGetEnabledTrackingCalsResp_t getTrackCalRsp;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, enableMasks, cleanup);

    ADI_LIBRARY_MEMSET(enableMasks, 0, sizeof(*enableMasks));
    ADI_LIBRARY_MEMSET(&getTrackCalRsp, 0, sizeof(adrv904x_DfeAppFrameworkCmdAppGetEnabledTrackingCalsResp_t));

    /* Send command and receive response */
    recoveryAction = adrv904x_DfeAppCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_DFE_APP_FRAMEWORK_CMD_TRACKINGCAL_GETENABLEDISABLE,
                                            0U,
                                            0U,
                                            (void*)&getTrackCalRsp,
                                            sizeof(adrv904x_DfeAppFrameworkCmdAppGetEnabledTrackingCalsResp_t),
                                            &cmdStatus);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_DFE_APP_CMD_RESP_CHECK_GOTO(getTrackCalRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

    for (channelIdx = 0U; channelIdx < ADI_ADRV904X_NUM_TRACKING_CAL_CHANNELS; channelIdx++)
    {
        enableMasks->enableMask[channelIdx] |= ADRV904X_CTOHL(getTrackCalRsp.enableMasks.enableMask[channelIdx]);
    }
cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeTrackingCalTimerSet(adi_adrv904x_Device_t* const                                 device,
                                                                     const adi_adrv904x_DfeAppFrameworkTrackingCalType_t          calType,
                                                                     const uint32_t                                               calTimerValue)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_FEATURE;
    adrv904x_DfeSvcCmdStatus_t cmdStatus = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;
    adi_adrv904x_DfeAppErrCode_e cpuErrorCode = ADI_ADRV904X_DFE_APP_ERR_CODE_NO_ERROR;

    adrv904x_DfeAppFrameworkCmdAppSetTimer_t setTrackCalTimer;
    adrv904x_DfeAppFrameworkCmdAppSetTimerResp_t setTrackCalTimerRsp;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_LIBRARY_MEMSET(&setTrackCalTimer, 0, sizeof(adrv904x_DfeAppFrameworkCmdAppSetTimer_t));
    ADI_LIBRARY_MEMSET(&setTrackCalTimerRsp, 0, sizeof(adrv904x_DfeAppFrameworkCmdAppSetTimerResp_t));

    setTrackCalTimer.calType = ADRV904X_HTOCL(calType);
    setTrackCalTimer.calTimerSetValue = ADRV904X_HTOCL(calTimerValue);

    /* validate input parameters */
    if (calType >= NUM_TRACKCAL_TYPES)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, calType, "Invalid calType provided.");
        goto cleanup;
    }

    if ((calTimerValue < 200U) || (calTimerValue > 5000U))  /* cal timer constrained to between 200 and 5000 milliseconds */
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, calTimerValue, "Invalid calTimerValue provided. Timer value must be between 200mS and 5000mS.");
        goto cleanup;
    }

    /* Send command and receive response */
    recoveryAction = adrv904x_DfeAppCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_DFE_APP_FRAMEWORK_CMD_TRACKINGCAL_SETTIMER,
                                            (void*)&setTrackCalTimer,
                                            sizeof(adrv904x_DfeAppFrameworkCmdAppSetTimer_t),
                                            (void*)&setTrackCalTimerRsp,
                                            sizeof(adrv904x_DfeAppFrameworkCmdAppSetTimerResp_t),
                                            &cmdStatus);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_DFE_APP_CMD_RESP_CHECK_GOTO(setTrackCalTimerRsp.cmdStatus, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }
cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeTrackingCalTimerGet(adi_adrv904x_Device_t* const                                 device,
                                                                     const adi_adrv904x_DfeAppFrameworkTrackingCalType_t          calType,
                                                                     uint32_t* const                                              calTimerValue)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_FEATURE;
    adrv904x_DfeSvcCmdStatus_t cmdStatus = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;
    adi_adrv904x_DfeAppErrCode_e cpuErrorCode = ADI_ADRV904X_DFE_APP_ERR_CODE_NO_ERROR;

    adrv904x_DfeAppFrameworkCmdAppGetTimer_t getTrackCalTimer;
    adrv904x_DfeAppFrameworkCmdAppGetTimerResp_t getTrackCalTimerRsp;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_LIBRARY_MEMSET(&getTrackCalTimer, 0, sizeof(adrv904x_DfeAppFrameworkCmdAppGetTimer_t));
    ADI_LIBRARY_MEMSET(&getTrackCalTimerRsp, 0, sizeof(adrv904x_DfeAppFrameworkCmdAppGetTimerResp_t));

    getTrackCalTimer.calType = ADRV904X_HTOCL(calType);

    /* validate input parameters */
    if (calType >= NUM_TRACKCAL_TYPES)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, calType, "Invalid calType provided.");
        goto cleanup;
    }

    /* Send command and receive response */
    recoveryAction = adrv904x_DfeAppCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_DFE_APP_FRAMEWORK_CMD_TRACKINGCAL_GETTIMER,
                                            (void*)&getTrackCalTimer,
                                            sizeof(adrv904x_DfeAppFrameworkCmdAppGetTimer_t),
                                            (void*)&getTrackCalTimerRsp,
                                            sizeof(adrv904x_DfeAppFrameworkCmdAppGetTimerResp_t),
                                            &cmdStatus);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_DFE_APP_CMD_RESP_CHECK_GOTO(getTrackCalTimerRsp.cmdStatus, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }
    *calTimerValue = ADRV904X_CTOHL(getTrackCalTimerRsp.calTimerGetValue);

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeSvcConfigSet(adi_adrv904x_Device_t* const  device,
                                                              const uint8_t                 configID,
                                                              const uint32_t                offset,
                                                              const uint8_t                 buffer[],
                                                              const uint32_t                size)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t cmdBuf[sizeof(adrv904x_DfeSvcCmdSetConfig_t) + MAX_DFE_CONFIG_DATA_SIZE];
    adrv904x_DfeSvcCmdSetConfig_t *setConfig;
    adrv904x_DfeSvcCmdSetConfigResp_t setConfigResp;
    adrv904x_DfeSvcCmdStatus_t cmdStatus = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;
    adi_adrv904x_DfeSvcErrCode_e cpuErrorCode = ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_EVENT_UNEXPECTED;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, buffer, cleanup);

    /* verify the configuration data size is not too large */
    if (size > MAX_DFE_CONFIG_DATA_SIZE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, size, "size is larger than maximum configuration data size");
        goto cleanup;
    }

    ADI_LIBRARY_MEMSET(&cmdBuf[0U], 0, sizeof(cmdBuf));
    ADI_LIBRARY_MEMSET(&setConfigResp, 0, sizeof(adrv904x_DfeSvcCmdSetConfigResp_t));

    setConfig = (adrv904x_DfeSvcCmdSetConfig_t *)&cmdBuf;
    setConfig->objId = (adi_adrv904x_DfeSvcErrObjId_e)configID;
    setConfig->offset = ADRV904X_HTOCL(offset);
    setConfig->length = ADRV904X_HTOCL(size);
    ADI_LIBRARY_MEMCPY(&cmdBuf[sizeof(adrv904x_DfeSvcCmdSetConfig_t)], buffer, size);

    /* Send command and receive response */
    recoveryAction = adrv904x_DfeSvcCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_DFE_SVC_CMD_ID_SET_CONFIG,
                                            (void*)cmdBuf,
                                            sizeof(adrv904x_DfeSvcCmdSetConfig_t) + size,
                                            (void*)&setConfigResp,
                                            sizeof(setConfigResp),
                                            &cmdStatus);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_DFE_SVC_CMD_RESP_CHECK_GOTO(setConfigResp.cmdStatus, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeAppConfigSet(adi_adrv904x_Device_t* const  device,
                                                              const uint8_t                 configID,
                                                              const uint32_t                offset,
                                                              const uint8_t                 buffer[],
                                                              const uint32_t                size)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t cmdBuf[sizeof(adrv904x_DfeAppFrameworkCmdAppSetConfig_t) + MAX_DFE_CONFIG_DATA_SIZE];
    adrv904x_DfeAppFrameworkCmdAppSetConfig_t    *setConfig = NULL;
    adrv904x_DfeAppFrameworkCmdAppSetConfigResp_t setConfigResp;
    adrv904x_DfeSvcCmdStatus_t cmdStatus = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;
    adi_adrv904x_DfeAppErrCode_e cpuErrorCode = ADI_ADRV904X_DFE_APP_ERR_CODE_NO_ERROR;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, buffer, cleanup);

    /* verify the configuration data size is not too large */
    if (size > MAX_DFE_CONFIG_DATA_SIZE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, size, "size is larger than maximum configuration data size");
        goto cleanup;
    }

    ADI_LIBRARY_MEMSET(&cmdBuf[0U], 0, sizeof(cmdBuf));
    ADI_LIBRARY_MEMSET(&setConfigResp, 0, sizeof(adrv904x_DfeAppFrameworkCmdAppSetConfigResp_t));

    setConfig         = (adrv904x_DfeAppFrameworkCmdAppSetConfig_t*)&cmdBuf;
    setConfig->objId  = ADRV904X_HTOCL(configID);
    setConfig->offset = ADRV904X_HTOCL(offset);
    setConfig->length = ADRV904X_HTOCL(size);
    ADI_LIBRARY_MEMCPY(&cmdBuf[sizeof(adrv904x_DfeAppFrameworkCmdAppSetConfig_t)], buffer, size);

    /* Send command and receive response */
    recoveryAction = adrv904x_DfeAppCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_DFE_APP_FRAMEWORK_CMD_TRACKINGCAL_SETCONFIG,
                                            (void*)cmdBuf,
                                            sizeof(adrv904x_DfeAppFrameworkCmdAppSetConfig_t) + size,
                                            (void*)&setConfigResp,
                                            sizeof(setConfigResp),
                                            &cmdStatus);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_DFE_APP_CMD_RESP_CHECK_GOTO(setConfigResp.cmdStatus, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeSvcConfigGet(adi_adrv904x_Device_t* const  device,
                                                              const uint8_t                 configID,
                                                              const uint32_t                offset,
                                                              const uint8_t                 buffer[],
                                                              const uint32_t                size)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t rspBuf[sizeof(adrv904x_DfeSvcCmdGetConfigResp_t) + MAX_DFE_CONFIG_DATA_SIZE];
    adrv904x_DfeSvcCmdGetConfig_t getConfig;
    adrv904x_DfeSvcCmdGetConfigResp_t* const getConfigResp = (adrv904x_DfeSvcCmdGetConfigResp_t*)&rspBuf;
    adrv904x_DfeSvcCmdStatus_t cmdStatus = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;
    adi_adrv904x_DfeSvcErrCode_e cpuErrorCode = ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_EVENT_UNEXPECTED;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, buffer, cleanup);

    /* verify the configuration data size is not too large */
    if (size > MAX_DFE_CONFIG_DATA_SIZE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, size, "size is larger than maximum configuration data size");
        goto cleanup;
    }

    ADI_LIBRARY_MEMSET(&rspBuf[0U], 0, sizeof(rspBuf));

    getConfig.objId  = (adi_adrv904x_DfeSvcErrObjId_e)configID;
    getConfig.offset = ADRV904X_HTOCL(offset);
    getConfig.length = ADRV904X_HTOCL(size);

    /* Send command and receive response */
    recoveryAction = adrv904x_DfeSvcCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_DFE_SVC_CMD_ID_GET_CONFIG,
                                            (void*)&getConfig,
                                            sizeof(adrv904x_DfeSvcCmdGetConfig_t),
                                            (void*)getConfigResp,
                                            sizeof(adrv904x_DfeSvcCmdGetConfigResp_t) + size,
                                            &cmdStatus);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_DFE_SVC_CMD_RESP_CHECK_GOTO(getConfigResp->cmdStatus, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

    ADI_LIBRARY_MEMCPY((void*)buffer, (void*)((uint8_t*)getConfigResp + sizeof(adrv904x_DfeSvcCmdGetConfigResp_t)), size);
cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeAppConfigGet(adi_adrv904x_Device_t* const  device,
                                                              const uint8_t                 configID,
                                                              const uint32_t                offset,
                                                              const uint8_t                 buffer[],
                                                              const uint32_t                size)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t rspBuf[sizeof(adrv904x_DfeAppFrameworkCmdAppGetConfigResp_t) + MAX_DFE_CONFIG_DATA_SIZE];
    adrv904x_DfeAppFrameworkCmdAppGetConfig_t  getConfig;
    adrv904x_DfeAppFrameworkCmdAppGetConfigResp_t* const getConfigResp = (adrv904x_DfeAppFrameworkCmdAppGetConfigResp_t*)&rspBuf;
    adrv904x_DfeSvcCmdStatus_t cmdStatus = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;
    adi_adrv904x_DfeAppErrCode_e cpuErrorCode = ADI_ADRV904X_DFE_APP_ERR_CODE_NO_ERROR;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, buffer, cleanup);

    /* verify the configuration data size is not too large */
    if (size > MAX_DFE_CONFIG_DATA_SIZE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, size, "size is larger than maximum configuration data size");
        goto cleanup;
    }

    ADI_LIBRARY_MEMSET(&rspBuf[0U], 0, sizeof(rspBuf));

    getConfig.objId  = configID;
    getConfig.offset = ADRV904X_HTOCL(offset);
    getConfig.length = ADRV904X_HTOCL(size);

    /* Send command and receive response */
    recoveryAction = adrv904x_DfeAppCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_DFE_APP_FRAMEWORK_CMD_TRACKINGCAL_GETCONFIG,
                                            (void*)&getConfig,
                                            sizeof(adrv904x_DfeAppFrameworkCmdAppGetConfig_t),
                                            (void*)getConfigResp,
                                            sizeof(adrv904x_DfeAppFrameworkCmdAppGetConfigResp_t) + size,
                                            &cmdStatus);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_DFE_APP_CMD_RESP_CHECK_GOTO(getConfigResp->cmdStatus, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

    ADI_LIBRARY_MEMCPY((void*)buffer, (void*)((uint8_t*)getConfigResp + sizeof(adrv904x_DfeAppFrameworkCmdAppGetConfigResp_t)), size);
cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeConfigUnlock(adi_adrv904x_Device_t* const  device,
                                                              const uint32_t                cfgKey)
{
      adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_DfeSvcCmdUnlockConfig_t unlockCfg;
    adrv904x_DfeSvcCmdUnlockConfigResp_t unlockCfgResp;
    adrv904x_DfeSvcCmdStatus_t cmdStatus = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;
    adi_adrv904x_DfeSvcErrCode_e cpuErrorCode = ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_EVENT_UNEXPECTED;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    unlockCfg.configKey = ADRV904X_HTOCL(cfgKey);

    /* Send command and receive response */
    recoveryAction = adrv904x_DfeSvcCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_DFE_SVC_CMD_ID_UNLOCK_CONFIG,
                                            (void*)&unlockCfg,
                                            sizeof(adrv904x_DfeSvcCmdUnlockConfig_t),
                                            (void*)&unlockCfgResp,
                                            sizeof(adrv904x_DfeSvcCmdUnlockConfigResp_t),
                                            &cmdStatus);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_DFE_SVC_CMD_RESP_CHECK_GOTO(unlockCfgResp.cmdStatus, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }
cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeSysStatusGet(adi_adrv904x_Device_t* const          device,
                                                              const adi_adrv904x_Channels_e         channel,
                                                              const uint32_t                        objID,
                                                              adi_adrv904x_CpuSysStatus_t* const    status)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, status, cleanup);

    /* validate the input parameters  */
    recoveryAction = adrv904x_DfeVerifyChannel(channel);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channel, "channel parameter is invalid.");
        goto cleanup;
    }

    if (((objID  < (uint32_t) ADI_ADRV904X_DFE_APP_FRAMEWORK_OBJID_CAL_VSWR) ||
        (objID  > (uint32_t) ADI_ADRV904X_DFE_APP_FRAMEWORK_OBJID_CAL_DPD)) &&
        (objID != (uint32_t) ADRV904X_DFE_APP_FRAMEWORK_CMD_OBJID_DRV_DUMMY))          /*  DUMMY objID used for testing */ 
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, objID, "objID parameter is invalid.");
        goto cleanup;
    }

    /* Use the common system status get handler to get the data from the CPU */
    recoveryAction = adrv904x_DfeSysStatusGet(device, (adrv904x_CpuObjectId_e)objID, channel, status);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error getting cpu system status.");
        goto cleanup;
    }
cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeAppControlCmdExec(adi_adrv904x_Device_t* const     device,
                                                                   const uint32_t                   objId,
                                                                   const uint16_t                   cpuCmd,
                                                                   const adi_adrv904x_Channels_e    channel,
                                                                   const uint8_t                    cpuCtrlData[],
                                                                   const uint32_t                   lengthSet,
                                                                   uint32_t* const                  lengthResp,
                                                                   uint8_t                          ctrlResp[],
                                                                   const uint32_t                   lengthGet)
{

        adi_adrv904x_ErrAction_e                            recoveryAction                                  = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t                                             txBuf[SVC_BBIC_BRIDGE_MAX_MAILBOX_LINK_SIZE];
    uint8_t                                             rxBuf[SVC_BBIC_BRIDGE_MAX_MAILBOX_LINK_SIZE];
    adrv904x_DfeAppFrameworkCmdAppSetCtrl_t*            setInfo                                         = NULL;
    adrv904x_DfeAppFrameworkCmdAppSetCtrlResp_t* const  cmdRsp                                          = (adrv904x_DfeAppFrameworkCmdAppSetCtrlResp_t*)&rxBuf;
    adrv904x_DfeSvcCmdStatus_t                          cmdStatus                                       = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;
    adi_adrv904x_DfeAppErrCode_e                        cpuErrorCode                                    = ADI_ADRV904X_DFE_APP_ERR_CODE_NO_ERROR;
    uint32_t                                            fwChannel                                       = 0U;
    uint16_t                                            cmdRspLen                                       = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    if (lengthSet > 0U)
    {
        ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cpuCtrlData, cleanup);
    }

    if (lengthGet > 0U)
    {
        ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, ctrlResp, cleanup);
    }

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, lengthResp, cleanup);

    if (objId > 0xFFU)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, objId, "Ctrl command - Invalid Object ID.");
        goto cleanup;
    }

    /* Allow lengthSet 0 for some use cases (RX_QEC) */
    if (lengthSet > (SVC_BBIC_BRIDGE_MAX_MAILBOX_LINK_SIZE - sizeof(adrv904x_DfeAppFrameworkCmdAppSetCtrl_t)
                                                           - sizeof(adrv904x_DfeAppFrameworkCmdAppCmd_t)
                                                           - sizeof(adrv904x_CpuCmd_t)))
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, lengthSet, "Ctrl command length is greater than SVC_BBIC_BRIDGE_MAX_MAILBOX_LINK_SIZE");
        goto cleanup;
    }


    if (lengthGet > (SVC_BBIC_BRIDGE_MAX_MAILBOX_LINK_SIZE - sizeof(adrv904x_DfeAppFrameworkCmdAppSetCtrlResp_t) -
                                                             sizeof(adrv904x_CpuCmdResp_t)))
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, lengthGet, "Ctrl command response length is greater than SVC_BBIC_BRIDGE_MAX_MAILBOX_LINK_SIZE");
        goto cleanup;
    }

    fwChannel = adrv904x_DfeChannelToChannelId(channel);

    ADI_LIBRARY_MEMSET(&txBuf[0U], 0, sizeof(txBuf));
    ADI_LIBRARY_MEMSET(&rxBuf[0U], 0, sizeof(rxBuf));

    /* Prepare the command payload */
    setInfo     = (adrv904x_DfeAppFrameworkCmdAppSetCtrl_t*)((uint8_t*)txBuf);
    setInfo->objId = ADRV904X_HTOCL(objId);
    setInfo->ctrlCmd = ADRV904X_HTOCS(cpuCmd);
    setInfo->channelNum = ADRV904X_HTOCL(fwChannel);
    setInfo->length = ADRV904X_HTOCS((uint16_t)lengthSet);

    if (lengthSet > 0U)
    {
        ADI_LIBRARY_MEMCPY((void*)((uint8_t*)setInfo + sizeof(adrv904x_DfeAppFrameworkCmdAppSetCtrl_t)), cpuCtrlData, lengthSet);
    }

    /* Send command and receive response.
     * Since we don't know how much data the CPU will send back,
     * we have to request MAX_DFE_CONFIG_DATA_SIZE bytes. We will only
     * copy out cmdRsp->length bytes of it below.
     */

    /* Then send it */
    recoveryAction = adrv904x_DfeAppCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_DFE_APP_FRAMEWORK_CMD_TRACKINGCAL_CTRL, 
                                            (void*)&txBuf,
                                            (sizeof(adrv904x_DfeAppFrameworkCmdAppSetCtrl_t) + lengthSet),
                                            (void*)cmdRsp,
                                            (sizeof(adrv904x_DfeAppFrameworkCmdAppSetCtrlResp_t) + lengthGet),
                                            &cmdStatus);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_DFE_APP_CMD_RESP_CHECK_GOTO(cmdRsp->cmdStatus, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

    cmdRspLen = ADRV904X_CTOHS(cmdRsp->length);

    if (cmdRspLen < sizeof(adrv904x_DfeAppFrameworkCmdAppSetCtrlResp_t))
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cmdRspLen, "Invalid Ctrl command response size");
        goto cleanup;
    }

    /* Read the response data from the CPU, if the caller's buffer can hold it. */
    *lengthResp = cmdRspLen - sizeof(adrv904x_DfeAppFrameworkCmdAppSetCtrlResp_t);

    if (*lengthResp <= lengthGet)
    {
        ADI_LIBRARY_MEMSET(ctrlResp, 0, lengthGet);
        ADI_LIBRARY_MEMCPY(ctrlResp, (void*)((uint8_t*)cmdRsp + sizeof(adrv904x_DfeAppFrameworkCmdAppSetCtrlResp_t)), *lengthResp);
    }
    else
    {
        *lengthResp = 0U;
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, lengthGet, "Ctrl Cmd Response size is greater than response buffer size");
        goto cleanup;
    }
cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeTrackingCalAllStateGet(adi_adrv904x_Device_t* const device, 
                                                                        adi_adrv904x_DfeAppFrameworkTrackingCalState_t* const calState)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_DfeSvcCmdStatus_t cmdStatus    = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;
    uint32_t channelIdx = 0U;
    uint32_t calIdx = 0U;
    adrv904x_DfeAppFrameworkCmdAppGetTrackingCalStateResp_t getStateCmdRsp;
    adi_adrv904x_DfeAppErrCode_e cpuErrorCode = ADI_ADRV904X_DFE_APP_ERR_CODE_NO_ERROR;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, calState, cleanup);

    /* Initialize the caller's buffer */
    ADI_LIBRARY_MEMSET(calState, 0, sizeof(*calState));

    ADI_LIBRARY_MEMSET(&getStateCmdRsp, 0, sizeof(adrv904x_DfeAppFrameworkCmdAppGetTrackingCalStateResp_t));

    /* Send command and receive response */
    recoveryAction = adrv904x_DfeAppCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_DFE_APP_FRAMEWORK_CMD_TRACKINGCAL_GETCALSTATE, 
                                            NULL,
                                            0U,
                                            &getStateCmdRsp,
                                            sizeof(getStateCmdRsp),
                                            &cmdStatus);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO(getStateCmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

    /* If the command was successful, OR the result with the existing result in calState.
     * Each CPU will only set the state information for a particular cal/channel if it "owns" that
     * channel (otherwise it will be set to 0).
     */
    for (channelIdx = 0U; channelIdx < ADI_ADRV904X_NUM_TRACKING_CAL_CHANNELS; channelIdx++)
    {
        calState->calError[channelIdx] |= ADRV904X_CTOHL(getStateCmdRsp.calState.calError[channelIdx]);

        for (calIdx = 0U; calIdx < NUM_TRACKCAL_TYPES; calIdx++)  /*  NUM_TRACKCAL_TYPES  is of adi_app_FrameworkTrackingCalType_e */
        {
            calState->calState[channelIdx][calIdx] |= getStateCmdRsp.calState.calState[channelIdx][calIdx];
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeTrackingCalStatusGet(adi_adrv904x_Device_t* const                                  device,
                                                                      const adi_adrv904x_DfeAppFrameworkTrackingCalibrationMask_e   calId,
                                                                      const adi_adrv904x_Channels_e                                 channel,
                                                                      adi_adrv904x_CalStatus_t* const                               calStatus)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, calStatus, cleanup);
    
    if (calId == 0U)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, calId, "calId cannot be zero.");
        goto cleanup; 
    }

    recoveryAction = adrv904x_DfeVerifyChannel(channel);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channel, "channel parameter is invalid.");
        goto cleanup;
    }

    /* Use the common cal status get handler to get the data from the CPU */
    recoveryAction = adrv904x_DfeCalCommonStatusGet(device, adrv904x_DfeTrackingCalToObjId(calId), channel, calStatus);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error getting tracking cal status.");
        goto cleanup;
    }
cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeCaptureSequencerConfigSet(adi_adrv904x_Device_t* const                            device,
                                                                           const adi_adrv904x_DfeAppCaptureSequencerCfg_t* const   capSeqCfg)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    char txBuf[ADRV904X_CPU_CMD_MAX_SIZE_BYTES];
    char rxBuf[ADRV904X_CPU_CMD_RESP_MAX_SIZE_BYTES];
    adrv904x_DfeAppFrameworkCmdAppSetCapSeqCfg_t * capSeqCfgCmd = NULL;
    adrv904x_DfeAppFrameworkCmdAppSetCapSeqCfgResp_t* capSeqCfgCmdRsp = NULL;
    adi_adrv904x_DfeAppErrCode_e cpuErrorCode = ADI_ADRV904X_DFE_APP_ERR_CODE_NO_ERROR;
    adrv904x_DfeSvcCmdStatus_t cmdStatus = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, capSeqCfg, cleanup);
    
    ADI_LIBRARY_MEMSET(&txBuf[0U], 0, sizeof(txBuf));
    ADI_LIBRARY_MEMSET(&rxBuf[0U], 0, sizeof(rxBuf));

    if (capSeqCfg->skipCapturePeriodIfNoRequest > 1U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, capSeqCfg->skipCapturePeriodIfNoRequest, "skipCapturePeriodIfNoRequest cannot be greater than 1.");
        goto cleanup; 
    }
    
    if (capSeqCfg->singleCapturePeriod > 1U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, capSeqCfg->singleCapturePeriod, "skipCapturePeriodIfNoRequest cannot be greater than 1.");
        goto cleanup; 
    }
    
    if (capSeqCfg->dummyCapPerNumOfMultiframes == 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, capSeqCfg->dummyCapPerNumOfMultiframes, "dummyCapPerNumOfMultiframes cannot be 0");
        goto cleanup; 
    }
    
    if ((capSeqCfg->dummyCapPerLengthMs < 2U))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, capSeqCfg->dummyCapPerLengthMs, "dummyCapPerLengthMs cannot be smaller than 2 ms");
        goto cleanup; 
    }
    
    if ((capSeqCfg->dummyCapPerMinimumPeriodMs < 100U) || (capSeqCfg->dummyCapPerMinimumPeriodMs > 5000U))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, capSeqCfg->dummyCapPerMinimumPeriodMs, "dummyCapPerMinimumPeriodMs range is [100,5000]");
        goto cleanup; 
    }
    
    
    /* Build the CPU command */
    capSeqCfgCmd = (adrv904x_DfeAppFrameworkCmdAppSetCapSeqCfg_t*)((uint8_t*)txBuf);
    capSeqCfgCmd->capSeqCfgSet.skipCapturePeriodIfNoRequest = capSeqCfg->skipCapturePeriodIfNoRequest;
    capSeqCfgCmd->capSeqCfgSet.singleCapturePeriod = capSeqCfg->singleCapturePeriod;
    capSeqCfgCmd->capSeqCfgSet.dummyCapPerNumOfMultiframes = capSeqCfg->dummyCapPerNumOfMultiframes;
    capSeqCfgCmd->capSeqCfgSet.dummyCapPerLengthMs = capSeqCfg->dummyCapPerLengthMs;
    capSeqCfgCmd->capSeqCfgSet.dummyCapPerMinimumPeriodMs = ADRV904X_HTOCS(capSeqCfg->dummyCapPerMinimumPeriodMs);

    capSeqCfgCmdRsp = (adrv904x_DfeAppFrameworkCmdAppSetCapSeqCfgResp_t*)((uint8_t*)rxBuf);

    /* Then send it */
    recoveryAction = adrv904x_DfeAppCmdSend(device,
        ADRV904X_LINK_ID_0,
        ADRV904X_DFE_APP_FRAMEWORK_CMD_TRACKINGCAL_SETCAPSEQCFG,
        (void*)txBuf,
        sizeof(adrv904x_DfeAppFrameworkCmdAppSetCapSeqCfg_t),
        (void*)capSeqCfgCmdRsp,
        sizeof(adrv904x_DfeAppFrameworkCmdAppSetCapSeqCfgResp_t),
        &cmdStatus);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_DFE_APP_CMD_RESP_CHECK_RETURN(capSeqCfgCmdRsp->cmdStatus, cmdStatus, cpuErrorCode, recoveryAction);
        goto cleanup;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeCaptureSequencerConfigGet(adi_adrv904x_Device_t* const                       device,
                                                                           adi_adrv904x_DfeAppCaptureSequencerCfg_t* const    capSeqCfg)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    char txBuf[ADRV904X_CPU_CMD_MAX_SIZE_BYTES];
    char rxBuf[ADRV904X_CPU_CMD_RESP_MAX_SIZE_BYTES];
    adrv904x_DfeAppFrameworkCmdAppGetCapSeqCfgResp_t* capSeqCfgCmdRsp = NULL;
    adi_adrv904x_DfeAppErrCode_e cpuErrorCode = ADI_ADRV904X_DFE_APP_ERR_CODE_NO_ERROR;
    adrv904x_DfeSvcCmdStatus_t cmdStatus = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, capSeqCfg, cleanup);
    
    ADI_LIBRARY_MEMSET(&txBuf[0U], 0, sizeof(txBuf));
    ADI_LIBRARY_MEMSET(&rxBuf[0U], 0, sizeof(rxBuf));

    /* Build the CPU command */
    capSeqCfgCmdRsp = (adrv904x_DfeAppFrameworkCmdAppGetCapSeqCfgResp_t*)((uint8_t*)rxBuf);

    /* Then send it */
    recoveryAction = adrv904x_DfeAppCmdSend(device,
        ADRV904X_LINK_ID_0,
        ADRV904X_DFE_APP_FRAMEWORK_CMD_TRACKINGCAL_GETCAPSEQCFG,
        (void*)txBuf,
        1U,
        (void*)capSeqCfgCmdRsp,
        sizeof(adrv904x_DfeAppFrameworkCmdAppGetCapSeqCfgResp_t),
        &cmdStatus);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_DFE_APP_CMD_RESP_CHECK_RETURN(capSeqCfgCmdRsp->cmdStatus, cmdStatus, cpuErrorCode, recoveryAction);
        goto cleanup;
    }
    
    capSeqCfg->skipCapturePeriodIfNoRequest = capSeqCfgCmdRsp->capSeqCfgGet.skipCapturePeriodIfNoRequest;
    capSeqCfg->singleCapturePeriod = capSeqCfgCmdRsp->capSeqCfgGet.singleCapturePeriod;
    capSeqCfg->dummyCapPerNumOfMultiframes = capSeqCfgCmdRsp->capSeqCfgGet.dummyCapPerNumOfMultiframes;
    capSeqCfg->dummyCapPerLengthMs = capSeqCfgCmdRsp->capSeqCfgGet.dummyCapPerLengthMs;
    capSeqCfg->dummyCapPerMinimumPeriodMs = ADRV904X_CTOHS(capSeqCfgCmdRsp->capSeqCfgGet.dummyCapPerMinimumPeriodMs);

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);

}
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeCalPvtStatusGet(adi_adrv904x_Device_t* const   device,
                                                                 const adi_adrv904x_Channels_e  channel,
                                                                 const uint32_t                 objId,
                                                                 uint8_t                        calStatusGet[],
                                                                 const uint32_t                 length)
{
        /*  stubbed var to bypass stubbed function build errors  */
    /*  remove the 3 lines below for implementation */
    (void) objId;
    (void) calStatusGet;
    (void) channel;
    (void) length;

    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_FEATURE;

    /* TODO: Function not implemented yet. */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_API_NOT_IMPLEMENTED_REPORT_GOTO(&device->common, cleanup);
cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeCalSpecificStatusGet(adi_adrv904x_Device_t* const  device,
                                                                      const adi_adrv904x_Channels_e channel,
                                                                      const uint32_t                objId,
                                                                      uint8_t                       calStatusGet[],
                                                                      const uint32_t                length)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_DfeSvcCmdStatus_t cmdStatus = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;
    uint8_t txBuf[ADRV904X_CPU_CMD_MAX_SIZE_BYTES];
    uint8_t rxBuf[ADRV904X_CPU_CMD_RESP_MAX_SIZE_BYTES];
    adrv904x_DfeAppFrameworkCmdAppGetCalStatus_t* calStatusCmd = NULL;
    adrv904x_DfeAppFrameworkCmdAppGetCalStatusResp_t* sendCalStatusCmdRsp = NULL;
    adi_adrv904x_DfeAppErrCode_e cpuErrorCode = ADI_ADRV904X_DFE_APP_ERR_CODE_NO_ERROR;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, calStatusGet, cleanup);

    ADI_LIBRARY_MEMSET(&txBuf[0U], 0, sizeof(txBuf));
    ADI_LIBRARY_MEMSET(&rxBuf[0U], 0, sizeof(rxBuf));

    /* validate the input parameters  */
    recoveryAction = adrv904x_DfeVerifyChannel(channel);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, 
                               recoveryAction, 
                               channel, 
                               "channel parameter is invalid.");
        goto cleanup;
    }

    /* Build the CPU command */
    calStatusCmd = (adrv904x_DfeAppFrameworkCmdAppGetCalStatus_t*)((uint8_t*)txBuf);
    calStatusCmd->type = ADI_ADRV904X_DFE_APP_FRAMEWORK_TRACKINGCAL_STATUS_SPECIFIC;
    calStatusCmd->calObjId = ADRV904X_HTOCL(objId);
    calStatusCmd->channelNum = ADRV904X_HTOCL(adrv904x_DfeChanMaskToNum(channel));

    sendCalStatusCmdRsp = (adrv904x_DfeAppFrameworkCmdAppGetCalStatusResp_t*)((uint8_t*)rxBuf);

    /* Then send it */
    recoveryAction = adrv904x_DfeAppCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_DFE_APP_FRAMEWORK_CMD_TRACKINGCAL_GETCALSTATUS, 
                                            (void*)txBuf,
                                            sizeof(adrv904x_DfeAppFrameworkCmdAppGetCalStatus_t),
                                            (void*)sendCalStatusCmdRsp,
                                            sizeof(adrv904x_DfeAppFrameworkCmdAppGetCalStatusResp_t) + length,
                                            &cmdStatus);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_DFE_APP_CMD_RESP_CHECK_RETURN(sendCalStatusCmdRsp->cmdStatus, cmdStatus, cpuErrorCode, recoveryAction);
        goto cleanup;
    }
    ADI_LIBRARY_MEMCPY(calStatusGet,(uint8_t *)(rxBuf + sizeof(adrv904x_DfeAppFrameworkCmdAppGetCalStatusResp_t)), length);

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeCalExtendedStatusGet(adi_adrv904x_Device_t* const  device,
                                                                      const adi_adrv904x_Channels_e channel,
                                                                      const uint32_t                objId,
                                                                      uint8_t                       calStatusGet[],
                                                                      const uint32_t                length)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_DfeSvcCmdStatus_t cmdStatus = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;
    char txBuf[ADRV904X_CPU_CMD_MAX_SIZE_BYTES];
    char rxBuf[ADRV904X_CPU_CMD_RESP_MAX_SIZE_BYTES];
    adrv904x_DfeAppFrameworkCmdAppGetCalStatus_t* calStatusCmd = NULL;
    adrv904x_DfeAppFrameworkCmdAppGetCalStatusResp_t* sendCalStatusCmdRsp = NULL;
    adi_adrv904x_DfeAppErrCode_e cpuErrorCode = ADI_ADRV904X_DFE_APP_ERR_CODE_NO_ERROR;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, calStatusGet, cleanup);

    ADI_LIBRARY_MEMSET(&txBuf[0U], 0, sizeof(txBuf));
    ADI_LIBRARY_MEMSET(&rxBuf[0U], 0, sizeof(rxBuf));

    /* validate the input parameters  */
    recoveryAction = adrv904x_DfeVerifyChannel(channel);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, 
                               recoveryAction, 
                               channel, 
                               "channel parameter is invalid.");
        goto cleanup;
    }

    /* Build the CPU command */    
    calStatusCmd = (adrv904x_DfeAppFrameworkCmdAppGetCalStatus_t*)((uint8_t*)txBuf);
    calStatusCmd->type = ADI_ADRV904X_DFE_APP_FRAMEWORK_TRACKINGCAL_STATUS_EXTENDED;
    calStatusCmd->calObjId = ADRV904X_HTOCL(objId);
    calStatusCmd->channelNum = ADRV904X_HTOCL(adrv904x_DfeChanMaskToNum(channel));

    sendCalStatusCmdRsp = (adrv904x_DfeAppFrameworkCmdAppGetCalStatusResp_t*)((uint8_t*)rxBuf);

    /* Then send it */
    recoveryAction = adrv904x_DfeAppCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_DFE_APP_FRAMEWORK_CMD_TRACKINGCAL_GETCALSTATUS, 
                                            (void*)txBuf,
                                            sizeof(adrv904x_DfeAppFrameworkCmdAppGetCalStatus_t),
                                            (void*)sendCalStatusCmdRsp,
                                            sizeof(adrv904x_DfeAppFrameworkCmdAppGetCalStatusResp_t) + length,
                                            &cmdStatus);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_DFE_APP_CMD_RESP_CHECK_RETURN(sendCalStatusCmdRsp->cmdStatus, cmdStatus, cpuErrorCode, recoveryAction);
        goto cleanup;
    }
    ADI_LIBRARY_MEMCPY(calStatusGet, (uint8_t *)(rxBuf + sizeof(adrv904x_DfeAppFrameworkCmdAppGetCalStatusResp_t)), length);

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}



ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeAppInitStatusGet(adi_adrv904x_Device_t* const  device,
                                                                  uint32_t* const               dfeAppInitStatus)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t address = 0U;
    uint32_t status = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, dfeAppInitStatus, cleanup);

    if ((device->devStateInfo.devState & ADI_ADRV904X_STATE_DFECPULOADED) != ADI_ADRV904X_STATE_DFECPULOADED)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "DFE CPU bin must be loaded before getting SDK app init status");
        goto cleanup;
    }

    /* Get dfe app init status address from CPU */
    recoveryAction = adrv904x_DfeSdkDataAddrGet(device, &address);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE SDK app init status address");
        goto cleanup;
    }

    address += ADI_LIBRARY_OFFSETOF(adrv904x_DfeSvcBbicBridgeSdkData_t, appInitStatus);
    /* Read and Store the app init status */
    recoveryAction = adi_adrv904x_Register32Read(device, NULL, address, (uint32_t*)&status, 0xFFFFFFFF);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE SDK app init status registers");
        goto cleanup;
    }

    *dfeAppInitStatus = adrv904x_CpuIntFromBytesGet((uint8_t*)&status, 4U);
cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeBootErrorGet(  adi_adrv904x_Device_t* const    device,
                                                                uint32_t* const                 dfeBootError)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t address = 0U;
    uint32_t booterrorCode = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, dfeBootError, cleanup);

    if ((device->devStateInfo.devState & ADI_ADRV904X_STATE_DFECPULOADED) != ADI_ADRV904X_STATE_DFECPULOADED)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "DFE CPU bin must be loaded before getting DFE SDK boot error code");
        goto cleanup;
    }

    /* Get DFE boot error code address from CPU */
    recoveryAction = adrv904x_DfeSdkDataAddrGet(device, &address);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE boot error code address");
        goto cleanup;
    }

    address += ADI_LIBRARY_OFFSETOF(adrv904x_DfeSvcBbicBridgeSdkData_t, dfeBootError);
    /* Read and Store the DFE boot error code */
    recoveryAction = adi_adrv904x_Register32Read(device, NULL, address, (uint32_t*)&booterrorCode, 0xFFFFFFFF);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE SDK Boot Error code registers");
        goto cleanup;
    }

    *dfeBootError = adrv904x_CpuIntFromBytesGet((uint8_t*)&booterrorCode, 4U);
cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeFwDebugModeSet(adi_adrv904x_Device_t* const        device,
                                                                const adi_adrv904x_DfeFwDebugMode_e dfeFwDebugMode)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t gpioPinList[][2] = {
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_ARM_TRST,            10U },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_ARM_TDO_SWO_MUX,     11U },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_ARM_TDI,             12U },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_ARM_TMS,             13U },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_ARM_TCK,             14U },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_TRACE_CLK_OUT,       15U },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_TRACE_DATA_OUT_0,    16U },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_TRACE_DATA_OUT_1,    17U },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_TRACE_DATA_OUT_2,    18U },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_TRACE_DATA_OUT_3,    19U },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_TRACE_DATA_OUT_4,    20U },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_TRACE_DATA_OUT_5,    21U },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_TRACE_DATA_OUT_6,    22U },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_TRACE_DATA_OUT_7,    23U }
    };
    uint8_t  gpioPinIdx = 0U;
    uint32_t GPIO_PIN_START = 0U;
    uint32_t GPIO_PIN_END = 0U;
    /* TODO: Remove as part of TPGSWE-5768 */
    uint32_t dfeSwBootFlagAddr = ADRV904X_DFE_CPU_INIT_ADDR_GET(swBootFlag);
    adrv904x_DfeSvcBbicBridgeSdkBootFlag_t jtagDebugEnable = ADRV904X_DFE_SVC_BBIC_BRIDGE_SDK_BOOT_FLAG_DEBUG_JTAG_ENABLE;
    /* TODO: End remove */


    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    if (dfeFwDebugMode == ADI_ADRV904X_DFE_FW_DEBUG_MODE_SWD)
    {
        GPIO_PIN_START = 3U;
        GPIO_PIN_END = 5U;
    }
    else if (dfeFwDebugMode == ADI_ADRV904X_DFE_FW_DEBUG_MODE_JTAG)
    {
        GPIO_PIN_START = 0U;
        GPIO_PIN_END = 5U;
    }
    else if (dfeFwDebugMode == ADI_ADRV904X_DFE_FW_DEBUG_MODE_JTAG4)
    {
        GPIO_PIN_START = 0U;
        GPIO_PIN_END = 10U;
    }
    else if (dfeFwDebugMode == ADI_ADRV904X_DFE_FW_DEBUG_MODE_JTAG8)
    {
        GPIO_PIN_START = 0U;
        GPIO_PIN_END = 14U;
    }
    else
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid DFE FW Debug Mode selection.");
        goto cleanup;
    }

    /* Now set the GPIO Pin for Debug */
    for (gpioPinIdx = GPIO_PIN_START; gpioPinIdx < GPIO_PIN_END; gpioPinIdx++)
    {
        /* Take the GPIO Pin if it is used by some other features */
        recoveryAction = adrv904x_GpioSignalRelease(device,
                                                    (adi_adrv904x_GpioPinSel_e) gpioPinList[gpioPinIdx][1],
                                                    (adi_adrv904x_GpioSignal_e) gpioPinList[gpioPinIdx][0],
                                                    0U); 
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting GPIO pin for FW Debug");
            goto cleanup;
        }
            
        recoveryAction = adrv904x_GpioSignalSet(device,
                                                (adi_adrv904x_GpioPinSel_e) gpioPinList[gpioPinIdx][1],
                                                (adi_adrv904x_GpioSignal_e) gpioPinList[gpioPinIdx][0],
                                                0U);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting GPIO pin for FW Debug");
            goto cleanup;
        }
    }

    /* TODO: Remove as part of TPGSWE-5768 */
    /* Enable SDK debug SWD/JTAG in the SDK */
    recoveryAction = adi_adrv904x_RegistersByteWrite(device, NULL, dfeSwBootFlagAddr, &jtagDebugEnable, 1U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Cannot write SDK debug enable flag to DFE memory");
        goto cleanup;
    }
    /* TODO: End remove */


cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeSysPvtStatusGet(adi_adrv904x_Device_t* const   device,
                                                                 const adi_adrv904x_Channels_e  channel,
                                                                 const uint32_t                 objID,
                                                                 uint8_t                        cpuSysStatusGet[],
                                                                 const uint32_t                 length)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    const void* pSysStatus = NULL;
    const void** const ppSysStatus = &pSysStatus;
    char rxBuf[ADRV904X_CPU_CMD_RESP_MAX_SIZE_BYTES];

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cpuSysStatusGet, cleanup);

    /* validate the input parameters  */
    recoveryAction = adrv904x_DfeVerifyChannel(channel);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channel, "channel parameter is invalid.");
        goto cleanup;
    }

    if (((objID  < (uint32_t) ADI_ADRV904X_DFE_APP_FRAMEWORK_OBJID_CAL_VSWR) ||
        (objID  > (uint32_t) ADI_ADRV904X_DFE_APP_FRAMEWORK_OBJID_CAL_DPD)) &&
        (objID != (uint32_t) ADRV904X_DFE_APP_FRAMEWORK_CMD_OBJID_DRV_DUMMY))          /*  DUMMY objID used for testing */ 
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, objID, "objID parameter is invalid.");
        goto cleanup;
    }

    if (length == 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, length, "Length is zero.");
        goto cleanup;
    }

    /* Send the system status get command to the CPU */
    recoveryAction = adrv904x_DfeSendSysStatusCmd(device,
                                                  ADRV904X_DFE_FRAMEWORK_CMD_SYS_STATUS_PRIVATE,
                                                  (adrv904x_CpuObjectId_e)objID,
                                                  channel);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SendCalStatusCmd failed");
        goto cleanup;
    }

    /* Get the response from the CPU */
    recoveryAction = adrv904x_DfeGetSysStatusCmdResp(device,
                                                     channel,
                                                     length,
                                                     (void*)&rxBuf[0],
                                                     ppSysStatus);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "GetCalStatusCmd failed");
        goto cleanup;
    }

    if (pSysStatus == NULL)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "GET_CAL_STATUS command failed");
        goto cleanup;
    }

    ADI_LIBRARY_MEMCPY(cpuSysStatusGet, pSysStatus, length);
cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeCpuLogMaskSet( adi_adrv904x_Device_t* const    device,
                                                                const uint32_t                  logMaskSet)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_FEATURE;
    adrv904x_DfeSvcCmdStatus_t cmdStatus = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;
    adi_adrv904x_DfeSvcErrCode_e cpuErrorCode = ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_EVENT_UNEXPECTED;
    /* All Logging Enabled in Log Mask*/
    const uint32_t LOG_TRACE_LEVEL_ALL = 0x1FU;

    adrv904x_DfeSvcCmdSetLogMaskCmd_t setLogMask;
    adrv904x_DfeSvcCmdGetLogMaskResp_t setLogMaskRsp;
    ADI_LIBRARY_MEMSET(&setLogMask, 0, sizeof(adrv904x_DfeSvcCmdSetLogMaskCmd_t));
    ADI_LIBRARY_MEMSET(&setLogMaskRsp, 0, sizeof(adrv904x_DfeSvcCmdGetLogMaskResp_t));

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    setLogMask.logMask = ADRV904X_HTOCL(logMaskSet);

    /* validate input parameters */

    if ((logMaskSet == 0U) || 
       (logMaskSet > LOG_TRACE_LEVEL_ALL))
    {

        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, logMaskSet, "Invalid logMask provided.");
        goto cleanup;
    }


    /* Send command and receive response */
    recoveryAction = adrv904x_DfeSvcCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_DFE_SVC_CMD_ID_LOG_MASK_SET,
                                            (void*)&setLogMask,
                                            sizeof(adrv904x_DfeSvcCmdSetLogMaskCmd_t),
                                            (void*)&setLogMaskRsp,
                                            sizeof(adrv904x_DfeSvcCmdGetLogMaskResp_t),
                                            &cmdStatus);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_DFE_SVC_CMD_RESP_CHECK_GOTO(setLogMaskRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }
cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);

}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeCpuLogMaskGet( adi_adrv904x_Device_t* const    device,
                                                                uint32_t* const                 logMaskGet)
{
            adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_FEATURE;
    adrv904x_DfeSvcCmdStatus_t cmdStatus = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;
    adi_adrv904x_DfeSvcErrCode_e cpuErrorCode = ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_EVENT_UNEXPECTED;
    adrv904x_DfeSvcCmdGetLogMaskResp_t getLogMaskRsp;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, logMaskGet, cleanup);

    ADI_LIBRARY_MEMSET(logMaskGet, 0, sizeof(*logMaskGet));
    ADI_LIBRARY_MEMSET(&getLogMaskRsp, 0, sizeof(adrv904x_DfeSvcCmdGetLogMaskResp_t));

    /* Send command and receive response */
    recoveryAction = adrv904x_DfeSvcCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_DFE_SVC_CMD_ID_LOG_MASK_GET,
                                            0U,
                                            0U,
                                            (void*)&getLogMaskRsp,
                                            sizeof(adrv904x_DfeSvcCmdGetLogMaskResp_t),
                                            &cmdStatus);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_DFE_SVC_CMD_RESP_CHECK_GOTO(getLogMaskRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

    *logMaskGet = getLogMaskRsp.logMask;

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);

}
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeCpuTelemetryMaskSet( adi_adrv904x_Device_t* const    device,
                                                                      const uint64_t                  sdkTelemetryMask0,
                                                                      const uint64_t                  sdkTelemetryMask1,
                                                                      const uint64_t                  appTelemetryMask0,
                                                                      const uint64_t                  appTelemetryMask1)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_FEATURE;
    adrv904x_DfeSvcCmdStatus_t cmdStatus = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;
    adi_adrv904x_DfeSvcErrCode_e cpuErrorCode = ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_EVENT_UNEXPECTED;

    adrv904x_DfeSvcCmdSetTelemetryMaskCmd_t setTelemetryMask;
    adrv904x_DfeSvcCmdGenericResp_t setTelemetryMaskRsp;
    ADI_LIBRARY_MEMSET(&setTelemetryMask, 0, sizeof(adrv904x_DfeSvcCmdSetTelemetryMaskCmd_t));
    ADI_LIBRARY_MEMSET(&setTelemetryMaskRsp, 0, sizeof(adrv904x_DfeSvcCmdGenericResp_t));

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    setTelemetryMask.sdkTelemetryMask0 = ADRV904X_HTOCLL(sdkTelemetryMask0);
    setTelemetryMask.sdkTelemetryMask1 = ADRV904X_HTOCLL(sdkTelemetryMask1);
    setTelemetryMask.appTelemetryMask0 = ADRV904X_HTOCLL(appTelemetryMask0);
    setTelemetryMask.appTelemetryMask1 = ADRV904X_HTOCLL(appTelemetryMask1);

    /* Send command and receive response */
    recoveryAction = adrv904x_DfeSvcCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_DFE_SVC_CMD_ID_TELEMETRY_MASK_SET,
                                            (void*)&setTelemetryMask,
                                            sizeof(adrv904x_DfeSvcCmdSetTelemetryMaskCmd_t),
                                            (void*)&setTelemetryMaskRsp,
                                            sizeof(adrv904x_DfeSvcCmdGenericResp_t),
                                            &cmdStatus);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_DFE_SVC_CMD_RESP_CHECK_GOTO(setTelemetryMaskRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }
cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);

}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeCpuTelemetryMaskGet( adi_adrv904x_Device_t* const    device,
                                                                      uint64_t * const                sdkTelemetryMask0,
                                                                      uint64_t * const                sdkTelemetryMask1,
                                                                      uint64_t * const                appTelemetryMask0,
                                                                      uint64_t * const                appTelemetryMask1)
{
         adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_FEATURE;
    adrv904x_DfeSvcCmdStatus_t cmdStatus = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;
    adi_adrv904x_DfeSvcErrCode_e cpuErrorCode = ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_EVENT_UNEXPECTED;
    adrv904x_DfeSvcCmdGetTelemetryMaskResp_t getTelemetryMaskRsp;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, sdkTelemetryMask0, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, sdkTelemetryMask1, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, appTelemetryMask0, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, appTelemetryMask1, cleanup);

    ADI_LIBRARY_MEMSET(&getTelemetryMaskRsp, 0, sizeof(adrv904x_DfeSvcCmdGetTelemetryMaskResp_t));

    /* Send command and receive response */
    recoveryAction = adrv904x_DfeSvcCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_DFE_SVC_CMD_ID_TELEMETRY_MASK_GET,
                                            0U,
                                            0U,
                                            (void*)&getTelemetryMaskRsp,
                                            sizeof(adrv904x_DfeSvcCmdGetTelemetryMaskResp_t),
                                            &cmdStatus);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_DFE_SVC_CMD_RESP_CHECK_GOTO(getTelemetryMaskRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

    *sdkTelemetryMask0 = ADRV904X_CTOHLL(getTelemetryMaskRsp.sdkTelemetryMask0);
    *sdkTelemetryMask1 = ADRV904X_CTOHLL(getTelemetryMaskRsp.sdkTelemetryMask1);
    *appTelemetryMask0 = ADRV904X_CTOHLL(getTelemetryMaskRsp.appTelemetryMask0);
    *appTelemetryMask1 = ADRV904X_CTOHLL(getTelemetryMaskRsp.appTelemetryMask1);

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeDebugMode(adi_adrv904x_Device_t* const device,
                                                           const uint32_t               debugModeKey)
{
      adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_DfeSvcCmdDebugMode_t debugMode;
    adrv904x_DfeSvcCmdDebugModeResp_t debugModeResp;
    adrv904x_DfeSvcCmdStatus_t cmdStatus = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;
    adi_adrv904x_DfeSvcErrCode_e cpuErrorCode = ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_EVENT_UNEXPECTED;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    debugMode.svcDebugModeKey = ADRV904X_HTOCL(debugModeKey);

    /* Send command and receive response */
    recoveryAction = adrv904x_DfeSvcCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_DFE_SVC_CMD_ID_DEBUG,
                                            (void*)&debugMode,
                                            sizeof(adrv904x_DfeSvcCmdDebugMode_t),
                                            (void*)&debugModeResp,
                                            sizeof(adrv904x_DfeSvcCmdDebugModeResp_t),
                                            &cmdStatus);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_DFE_SVC_CMD_RESP_CHECK_GOTO(debugModeResp.cmdStatus, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }
cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeDebugModeCmd(adi_adrv904x_Device_t* const    device,
                                                              const uint32_t                  objId,
                                                              const uint16_t                  cpuCmd,
                                                              const uint8_t                   cpuDebugData[],
                                                              const uint32_t                  lengthSet,
                                                              uint32_t* const                 lengthResp,
                                                              uint8_t                         debugResp[],
                                                              const uint32_t                  lengthGet)
{
      adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    char txBuf[sizeof(adrv904x_CpuCmd_DebugMaxSize_t)];
    char rxBuf[sizeof(adrv904x_CpuCmd_DebugRespMaxSize_t)];
    adrv904x_DfeSvcCmdDebugCmd_t* const debugModeCmd = (adrv904x_DfeSvcCmdDebugCmd_t*)&txBuf;
    adrv904x_DfeSvcCmdDebugCmdResp_t* const debugModeCmdResp = (adrv904x_DfeSvcCmdDebugCmdResp_t*)&rxBuf;
    adrv904x_DfeSvcCmdStatus_t cmdStatus = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;
    adi_adrv904x_DfeSvcErrCode_e cpuErrorCode = ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_EVENT_UNEXPECTED;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cpuDebugData, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, debugResp, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, lengthResp, cleanup);

    if ((objId != ADI_ADRV904X_DFE_SVC_ERR_OBJID_SVC_ECC) &&  /* currently, only two */
        (objId != ADI_ADRV904X_DFE_SVC_ERR_OBJID_SVC_LOG))
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, objId, "DFE Cpu Debug command - Invalid Object ID.");
        goto cleanup;
    }

    if (lengthSet > MAX_DEBUG_DATA_SIZE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, lengthSet, "DFE Cpu Debug command length is greater than MAX_DEBUG_DATA_SIZE");
        goto cleanup;
    }

    if (lengthSet == 0U)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, lengthSet, "DFE Cpu Debug command length must be greater than 0");
        goto cleanup;
    }

    if (lengthGet == 0U)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, lengthGet, "DFE Cpu Debug command response buffer size must be greater than 0");
        goto cleanup;
    }

    ADI_LIBRARY_MEMSET(&txBuf[0U], 0, sizeof(txBuf));
    ADI_LIBRARY_MEMSET(&rxBuf[0U], 0, sizeof(rxBuf));

    /* Prepare the command payload */
    debugModeCmd->objId = ADRV904X_HTOCL(objId);
    debugModeCmd->debugCmd = ADRV904X_HTOCS(cpuCmd);
    debugModeCmd->length = ADRV904X_HTOCS((uint16_t)lengthSet);

    ADI_LIBRARY_MEMCPY((void*)((uint8_t*)debugModeCmd + sizeof(adrv904x_DfeSvcCmdDebugCmd_t)), cpuDebugData, lengthSet);

    /* Send command and receive response */
    recoveryAction = adrv904x_DfeSvcCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_DFE_SVC_CMD_ID_DEBUG_CMD,
                                            (void*)txBuf,
                                            sizeof(adrv904x_DfeSvcCmdDebugCmd_t)+lengthSet,
                                            (void*)rxBuf,
                                            sizeof(adrv904x_DfeSvcCmdDebugCmdResp_t),
                                            &cmdStatus);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_DFE_SVC_CMD_RESP_CHECK_GOTO(debugModeCmdResp->cmdStatus, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

    if (debugModeCmdResp->length <= lengthGet)
    {
        ADI_LIBRARY_MEMSET(debugResp, 0, lengthGet);
        *lengthResp = debugModeCmdResp->length;
        ADI_LIBRARY_MEMCPY(debugResp, ((adrv904x_CpuCmd_DebugMaxSize_t *)debugModeCmdResp)->debugData, *lengthResp);
    }
    else
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, lengthGet, "Debug Cmd Response size is greater than response buffer size");
        goto cleanup;
    }
cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeWatchdogConfigSet(adi_adrv904x_Device_t* const                 device,
                                                                   const adi_adrv904x_DfeSvcWatchdogInstance_t  wdt,
                                                                   const uint32_t                               timeout_ms)
{

        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_DfeSvcCmdWatchdogConfig_t setConfig;
    adrv904x_DfeSvcCmdWatchdogConfigResp_t setConfigResp;
    adrv904x_DfeSvcCmdStatus_t cmdStatus = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;
    adi_adrv904x_CpuErrorCode_t cpuErrorCode = 0U;
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    /* verify the configuration wdt */
    if (wdt > ADI_ADRV904X_DFE_SVC_WATCHDOG_NUM_WDT)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, wdt, "Invalid wdt provided.");
        goto cleanup;
    }
    
    setConfig.wdt = (uint8_t)wdt;
    setConfig.timeout_ms = ADRV904X_HTOCL(timeout_ms);

    /* Send command and receive response */
    recoveryAction = adrv904x_DfeSvcCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_DFE_SVC_CMD_ID_WATCHDOG_CONFIG,
                                            (void*)&setConfig,
                                            sizeof(adrv904x_DfeSvcCmdWatchdogConfig_t),
                                            (void*)&setConfigResp,
                                            sizeof(adrv904x_DfeSvcCmdWatchdogConfigResp_t),
                                            &cmdStatus);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_DFE_SVC_CMD_RESP_CHECK_GOTO(setConfigResp.cmdStatus, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }
cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeUpdateEcc(adi_adrv904x_Device_t* const device)
{
      adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_DfeSvcEccUpdateResp_t eccUpdateResp;
    adrv904x_DfeSvcCmdStatus_t cmdStatus = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;
    adi_adrv904x_DfeSvcErrCode_e cpuErrorCode = ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_EVENT_UNEXPECTED;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    /* Send command and receive response */
    recoveryAction = adrv904x_DfeSvcCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_DFE_SVC_CMD_ID_UPDATE_ECC,
                                            0U,
                                            0U,
                                            (void*)&eccUpdateResp,
                                            sizeof(adrv904x_DfeSvcEccUpdateResp_t),
                                            &cmdStatus);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_DFE_SVC_CMD_RESP_CHECK_GOTO(eccUpdateResp.cmdStatus, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }
cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeCpuPintSwStatusGet(adi_adrv904x_Device_t* const    device,
                                                                    uint8_t* const                  statusPintSw0,
                                                                    uint8_t* const                  statusPintSw1)
{
      const uint32_t ADDRESS_DFE_CPU_READ_PINT_SW0  = 0xb2220108U;
    const uint32_t ADDRESS_DFE_CPU_READ_PINT_SW1  = 0xb2220118U;
    const uint32_t READ_WRITE_MASK                = 0xFFFFFFFFU;

    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t tmpRead = 0U;
    
    /* Check inputs */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, statusPintSw0, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, statusPintSw1, cleanup);
    
    /* Read PintSw0 interrupt status */
    recoveryAction = adi_adrv904x_Register32Read(device, NULL, ADDRESS_DFE_CPU_READ_PINT_SW0, &tmpRead, READ_WRITE_MASK);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to read DFE CPU PintSw0 Status.");
        goto cleanup;
    }
    *statusPintSw0 = (uint8_t)tmpRead;
    
    /* Read PintSw1 interrupt status */
    recoveryAction = adi_adrv904x_Register32Read(device, NULL, ADDRESS_DFE_CPU_READ_PINT_SW1, &tmpRead, READ_WRITE_MASK);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to read DFE CPU PintSw1 Status.");
        goto cleanup;
    }
    *statusPintSw1 = (uint8_t)tmpRead;
cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeCpuPintSwStatusClear(adi_adrv904x_Device_t* const  device,
                                                                      const uint8_t                 clearPintSw0,
                                                                      const uint8_t                 clearPintSw1)
{
      const uint32_t ADDRESS_DFE_CPU_CLEAR_PINT_SW0  = 0xb2220104U;
    const uint32_t ADDRESS_DFE_CPU_CLEAR_PINT_SW1  = 0xb2220114U;
    const uint32_t READ_WRITE_MASK                 = 0xFFFFFFFFU;
    
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    
    /* Check inputs */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    
    recoveryAction = adi_adrv904x_Register32Write(device, NULL, ADDRESS_DFE_CPU_CLEAR_PINT_SW0, (uint32_t)clearPintSw0, READ_WRITE_MASK);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to clear selected interrupts in DFE CPU PINTSW0.");
        goto cleanup;
    }

    recoveryAction = adi_adrv904x_Register32Write(device, NULL, ADDRESS_DFE_CPU_CLEAR_PINT_SW1, (uint32_t)clearPintSw1, READ_WRITE_MASK);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to clear selected interrupts in DFE CPU PINTSW1.");
        goto cleanup;
    }
cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeCpuPintSw0DetailedInfoGet(adi_adrv904x_Device_t* const                 device,
                                                                           const adi_adrv904x_DfeCpuPintSw0Status_e     interruptSelect,
                                                                           uint32_t* const                              pInfo)
{
      adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    const uint32_t DEFAULT_INFO = 0U;
    const uint32_t MAXNUM_ERROBJ = sizeof(adi_adrv904x_DfeSvcLogTraceErrLog_t) / sizeof(adi_adrv904x_DfeSvcLogTraceErrLogData_t);
    uint32_t loop = 0U;
    uint64_t lastLatestTimeStamp = 0U;
    adi_adrv904x_DfeSvcLogTraceLevel_t logTraceLevelSelect = 0U;
    adi_adrv904x_DfeSvcLogTraceErrLog_t lastError;
    adi_adrv904x_DfeSvcLogTraceErrLogData_t* pLastErrorData = (adi_adrv904x_DfeSvcLogTraceErrLogData_t*)&lastError;
    
    /* Check inputs */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, pInfo, cleanup);

    ADI_LIBRARY_MEMSET(&lastError, 0, sizeof(lastError));

    /* Return detailed info stored from the the last instance of selected interrupt. Limited to uint32_t data */
    if (interruptSelect == ADI_ADRV904X_DFE_CPU_PINTSW0_STATUS_INT_0_FATAL_ERROR)
    {
        logTraceLevelSelect = ADI_ADRV904X_DFE_SVC_LOG_TRACE_LEVEL_FATAL;
    }
    else if (interruptSelect == ADI_ADRV904X_DFE_CPU_PINTSW0_STATUS_INT_0_CRITICAL_ERROR)
    {
        logTraceLevelSelect = ADI_ADRV904X_DFE_SVC_LOG_TRACE_LEVEL_CRITICAL;
    }
    else  /* Nothing useful to return in this case*/
    {
        /* Allow the user to call this function with no error selected and it be a no-op */
        recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
        *pInfo = DEFAULT_INFO;
        goto cleanup;
    }

    if (logTraceLevelSelect != 0U)
    {
        recoveryAction = adrv904x_DfeLastErrorCodeGet(device, &lastError);

        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error retrieving logTrace Error");
            goto cleanup;
        }
        else
        {
            *pInfo = DEFAULT_INFO;
            for (loop = 0U; loop < MAXNUM_ERROBJ; loop++)
            {
                if (pLastErrorData->errLogLevel == logTraceLevelSelect)
                {
                    if (pLastErrorData->errLogTimeStamp > lastLatestTimeStamp)
                    {
                        lastLatestTimeStamp = pLastErrorData->errLogTimeStamp;
                        *pInfo = pLastErrorData->errLogCode;
                    }
                }
                pLastErrorData++;
            }
        }
    }
cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeSvcLogTraceErrLogGet(adi_adrv904x_Device_t* const                     device,
                                                                      adi_adrv904x_DfeSvcLogTraceErrLog_t* const       lastErrorData)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    /* Check inputs */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, lastErrorData, cleanup);

    recoveryAction = adrv904x_DfeLastErrorCodeGet(device, lastErrorData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error retrieving last Error Data");
        goto cleanup;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeCpuPintSw1DetailedInfoGet(adi_adrv904x_Device_t* const                 device,
                                                                           const adi_adrv904x_DfeCpuPintSw1Status_e     interruptSelect,
                                                                           uint32_t* const                              pInfo)
{
      const uint32_t DEFAULT_INFO = 0U;
    
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t readScratch = 0U;
    uint32_t address = 0U;
    uint32_t vswrAlarm = 0U;

    /* Check inputs */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, pInfo, cleanup);
    
    /* Return detailed info stored from the the last instance of selected interrupt. Limited to uint32_t data */
    switch (interruptSelect)
    {
        case (ADI_ADRV904X_DFE_CPU_PINTSW1_STATUS_INT_0_TDD_LUT_UPDATE_IN_PROGRESS_TXON):
            /* Readback the Tx Channel that last triggered this error. Stored in a scratchpad by Core Stream Proc */
            recoveryAction =  adrv904x_Core_ScratchReg_BfGet(device,
                                                             NULL,
                                                             ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                             ADRV904X_DFE_DMA_REQUEST_ERROR,
                                                             &readScratch);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed readback of Tx Channel Mask of last DFE CPU LUT Copy Error");
                goto cleanup;
            }
            *pInfo = (uint32_t)(readScratch);
            break;

        case (ADI_ADRV904X_DFE_CPU_PINTSW1_STATUS_INT_1_VSWR_ALARM):
            /* Get VSWR alarm status address */
            recoveryAction = adrv904x_DfeSdkDataAddrGet(device, &address);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue reading SDK data address");
                goto cleanup;
            }
            address += ADI_LIBRARY_OFFSETOF(adrv904x_DfeSvcBbicBridgeSdkData_t, vswrAlarm);
        
            /* Read the VSWR alarm data */
            recoveryAction = adi_adrv904x_Register32Read(device, NULL, address, &vswrAlarm, 0xFFFFFFFF);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue reading VSWR alarm status");
                goto cleanup;
            }
            *pInfo = adrv904x_CpuIntFromBytesGet((uint8_t*)&vswrAlarm, 4U);
            break;

        case (ADI_ADRV904X_DFE_CPU_PINTSW1_STATUS_INT_NONE): /* Fallthrough */
        default:
            /* Allow the user to call this function with no error selected and it be a no-op */
            recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
            *pInfo = DEFAULT_INFO;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}
