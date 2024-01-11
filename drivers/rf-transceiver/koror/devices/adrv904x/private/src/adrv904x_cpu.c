/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adrv904x_cpu.c
*
* ADRV904X API Version: 2.10.0.4
*/

#include "../../private/include/adrv904x_cpu.h"
#include "../../private/include/adrv904x_gpio.h"
#include "../../private/include/adrv904x_cpu_error_codes_types.h"
#include "../../private/include/adrv904x_reg_addr_macros.h"
#include "../../private/include/adrv904x_init.h"
#include "../../private/include/adrv904x_cpu_memory.h"
#include "../../private/include/adrv904x_cpu_device_profile_types.h"
#include "../../private/include/adrv904x_cpu_scratch_registers.h"
#include "adi_adrv904x_cpu_cmd_ecc_scrub.h"

#include "adi_adrv904x_dfe_cpu.h"
#include "../../private/include/adrv904x_dfe_cpu.h"
#include "../../private/include/adrv904x_dfe_svc_bbic_bridge_t.h"
#include "../../private/include/adrv904x_dfe_framework_cmd_t.h"

#include "adi_adrv904x_cpu_types.h"
#include "adi_adrv904x_cpu.h"
#include "adi_adrv904x_utilities.h"


#define ADI_FILE    ADI_ADRV904X_FILE_PRIVATE_CPU

/*****************************************************************************/
/***** Helper functions' prototypes ******************************************/
/*****************************************************************************/
static adi_adrv904x_ErrAction_e adrv904x_CpuAddrInitialize( adi_adrv904x_CpuAddr_t* const   cpuAddr,
                                                            const adi_adrv904x_CpuType_e    type);


/*****************************************************************************/
/***** Local data types ******************************************************/
/*****************************************************************************/

static adi_adrv904x_ErrAction_e adrv904x_CpuAddrInitialize( adi_adrv904x_CpuAddr_t* const   cpuAddr,
                                                            const adi_adrv904x_CpuType_e    type)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    switch (type)
    {
    case ADI_ADRV904X_CPU_TYPE_0:
        cpuAddr->enabled            = ADI_TRUE;
        cpuAddr->ctlAddr            = ADRV904X_CPU_0_ADDR_CTL_1;
        cpuAddr->bootAddr           = ADRV904X_CPU_0_ADDR_BOOT_ADDR_BYTE0;
        cpuAddr->stackPtrAddr       = ADRV904X_CPU_0_ADDR_STACK_PTR_BYTE0;
        cpuAddr->memBankCtrlAddr    = ADRV904X_CPU_0_ADDR_MEM_BANK_CTRL;
        cpuAddr->cmdAddr            = ADRV904X_CPU_0_ADDR_COMMAND;
        cpuAddr->extCmdAddr         = ADRV904X_CPU_0_ADDR_EXT_CMD_BYTE_1;
        cpuAddr->cmdStatusAddr      = ADRV904X_CPU_0_ADDR_CMD_STATUS_0;
        cpuAddr->progStartAddr      = ADRV904X_CPU_0_ADDR_PROG_START;
        cpuAddr->mailboxGetAddr     = ADRV904X_CPU_0_DM_MAILBOX_LINK_0_START_ADDR;
        cpuAddr->versionAddr        = ADRV904X_ADDR_FW_VERSION;
        recoveryAction              = ADI_ADRV904X_ERR_ACT_NONE;
        break;

    case ADI_ADRV904X_CPU_TYPE_1:
        cpuAddr->ctlAddr            = ADRV904X_CPU_1_ADDR_CTL_1;
        cpuAddr->bootAddr           = ADRV904X_CPU_1_ADDR_BOOT_ADDR_BYTE0;
        cpuAddr->stackPtrAddr       = ADRV904X_CPU_1_ADDR_STACK_PTR_BYTE0;
        cpuAddr->memBankCtrlAddr    = ADRV904X_CPU_1_ADDR_MEM_BANK_CTRL;
        cpuAddr->cmdAddr            = ADRV904X_CPU_1_ADDR_COMMAND;
        cpuAddr->extCmdAddr         = ADRV904X_CPU_1_ADDR_EXT_CMD_BYTE_1;
        cpuAddr->cmdStatusAddr      = ADRV904X_CPU_1_ADDR_CMD_STATUS_0;
        cpuAddr->progStartAddr      = ADRV904X_CPU_1_ADDR_PROG_START;
        cpuAddr->mailboxGetAddr     = ADRV904X_CPU_1_DM_MAILBOX_LINK_0_START_ADDR;
        cpuAddr->versionAddr        = ADRV904X_ADDR_FW_VERSION;
        recoveryAction              = ADI_ADRV904X_ERR_ACT_NONE;
        break;

    case ADI_ADRV904X_CPU_TYPE_DFE:
        recoveryAction              = ADI_ADRV904X_ERR_ACT_NONE;
        break;

    case ADI_ADRV904X_CPU_TYPE_UNKNOWN:   /* Fall Through */
    case ADI_ADRV904X_CPU_TYPE_MAX_RADIO: /* Fall Through */
    case ADI_ADRV904X_CPU_TYPE_MAX:       /* Fall Through */

    default:
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        break;
    }

    return recoveryAction;
}

static adi_adrv904x_ErrAction_e adrv904x_CpuMailBoxRangeCheck(adi_adrv904x_Device_t* const    device,
                                                              const adi_adrv904x_CpuType_e    cpuType,
                                                              const adrv904x_LinkId_e         linkId,
                                                              const uint32_t                  byteCount)
{
    adi_adrv904x_ErrAction_e  recoveryAction  = ADI_ADRV904X_ERR_ACT_NONE;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    if ((cpuType >= ADI_ADRV904X_CPU_TYPE_MAX) ||
        (cpuType <= ADI_ADRV904X_CPU_TYPE_UNKNOWN))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common, recoveryAction, cpuType, "Invalid CPU Type Passed");
        return recoveryAction;
    }

    if ((linkId >= ADRV904X_LINK_ID_MAX) ||
            (linkId <= ADRV904X_LINK_ID_UNKNOWN))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,recoveryAction, linkId,"Invalid LinkId Passed");
        return recoveryAction;
    }

    if (cpuType == ADI_ADRV904X_CPU_TYPE_0)
    {
        /* check for valid byteCount */
        if (byteCount >  ADRV904X_CPU_0_MAILBOX_LINK_0_SIZE)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, byteCount, "Invalid Byte Count");
            return recoveryAction;
        }
    }

    if (cpuType == ADI_ADRV904X_CPU_TYPE_1)
    {
        /* check for valid byteCount */
        if (byteCount >  ADRV904X_CPU_1_MAILBOX_LINK_0_SIZE)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, byteCount, "Invalid Byte Count");
            return recoveryAction;
        }
    }

    if (cpuType == ADI_ADRV904X_CPU_TYPE_DFE)
    {
        /* check for valid byteCount */
        if (byteCount >  sizeof(_adi_DfeMailboxBuffer_t))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, byteCount, "Invalid Byte Count");
            return recoveryAction;
        }
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CpuForceException(adi_adrv904x_Device_t* const    device,
                                                            const adi_adrv904x_CpuType_e    cpuType)
{
    adi_adrv904x_ErrAction_e  recoveryAction  = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adi_adrv904x_CpuAddr_t *cpuAddr = NULL;
    uint32_t i = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    if (cpuType == ADI_ADRV904X_CPU_TYPE_0 ||
        cpuType == ADI_ADRV904X_CPU_TYPE_1)
    {
        cpuAddr = &(device->devStateInfo.cpu.cpuAddr[cpuType]);
        ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, cpuAddr);

        /* Command payload */
        for (i = 1U; i <= 4U; ++i)
        {
            recoveryAction = adi_adrv904x_Register32Write(device,
                                                          NULL,
                                                          cpuAddr->cmdAddr + i,
                                                          (uint32_t)0xFFU,
                                                          (uint32_t)0xFFU);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "CPU Mailbox Command Write Issue");
                return recoveryAction;
            }
        }

        /* Opcode */
        recoveryAction = adi_adrv904x_Register32Write(device,
                                                      NULL,
                                                      cpuAddr->cmdAddr,
                                                      (uint32_t)ADRV904X_MAILBOX_FORCE_EXCEPTION,
                                                      (uint32_t)0xFFU);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "CPU Mailbox Command Write Issue");
            return recoveryAction;
        }
    }

    else if (cpuType == ADI_ADRV904X_CPU_TYPE_DFE)
    {
        recoveryAction = adrv904x_Core_A55Spi0Command_BfSet(device,
                                                            NULL,
                                                            ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                            ADRV904X_DFE_SVC_CMD_DFE_MAILBOX_FORCE_EXCEPTION);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register write (for DFE CPU Command) Issue");
            return recoveryAction;
        }
    }
    else
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuType, "Invalid CPU Type Passed");
        return recoveryAction;
    }

    return recoveryAction;
}

static adi_adrv904x_ErrAction_e adrv904x_CpuMailboxBufferWrite(adi_adrv904x_Device_t* const    device,
                                                               const adi_adrv904x_CpuType_e    cpuType,
                                                               const adrv904x_LinkId_e         linkId,
                                                               const uint8_t                   data[],
                                                               const uint32_t                  byteCount)
{
    adi_adrv904x_ErrAction_e  recoveryAction  = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adi_adrv904x_SpiCache_t spiCache = { {0U}, 0U, 0U, 0U };
    uint32_t cpuAddr = ADRV904X_CPU_0_DM_MAILBOX_LINK_0_START_ADDR;
    uint32_t i = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, data);

    if (0U == byteCount)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, byteCount, "Invalid Byte Count");
        return recoveryAction;
    }

    recoveryAction = adrv904x_CpuMailBoxRangeCheck( device, cpuType, linkId, byteCount);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "CPU Mailbox Range Check Issue");
        return recoveryAction;
    }

    /* get the particular processor's address map */
    if (cpuType == ADI_ADRV904X_CPU_TYPE_1)
    {
        cpuAddr = ADRV904X_CPU_1_DM_MAILBOX_LINK_0_START_ADDR;

    }
    else if (cpuType == ADI_ADRV904X_CPU_TYPE_DFE)
    {
        uint32_t sdkdata_address;

        /* get the data address from the scratchpad register */
        recoveryAction = adrv904x_DfeSdkDataAddrGet(device, &sdkdata_address);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "DFE Mailbox Address Issue");
            return recoveryAction;
        }

        cpuAddr = sdkdata_address + ADI_LIBRARY_OFFSETOF(adrv904x_DfeSvcBbicBridgeSdkData_t, dfeMailboxBuffer) + ADI_LIBRARY_OFFSETOF(_adi_DfeMailboxBuffer_t, buffer);
    }

    for (i = 0U; i < byteCount; ++i)
    {
        recoveryAction = adi_adrv904x_Register32Write(  device,
                                                        &spiCache,
                                                        cpuAddr + i,
                                                        (uint32_t)data[i],
                                                        (uint32_t)0xFFU);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "CPU Mailbox Buffer Write Issue");
            return recoveryAction;
        }
    }

    recoveryAction = adi_adrv904x_SpiFlush( device, spiCache.data, &spiCache.count);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI Flush Issue After CPU Mailbox Buffer Write");
        return recoveryAction;
    }

    return recoveryAction;
}


static adi_adrv904x_ErrAction_e adrv904x_CpuMailboxBufferRead(adi_adrv904x_Device_t* const    device,
                                                              const adi_adrv904x_CpuType_e    cpuType,
                                                              const adrv904x_LinkId_e         linkId,
                                                              uint8_t                         data[],
                                                              const uint32_t                  byteCount)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t cpuAddr = ADRV904X_CPU_0_DM_MAILBOX_LINK_0_START_ADDR;
    

    uint8_t readData[SVC_BBIC_BRIDGE_MAX_MAILBOX_LINK_SIZE] = { 0U };
    
    uint32_t i = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, data);

    if (0U == byteCount)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, byteCount, "Invalid Byte Count");
        return recoveryAction;
    }

    recoveryAction = adrv904x_CpuMailBoxRangeCheck(device, cpuType, linkId, byteCount);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "CPU Mailbox Range Check Issue");
        return recoveryAction;
    }

    /* get the particular processor's address map */
    if (cpuType == ADI_ADRV904X_CPU_TYPE_1)
    {
        cpuAddr = ADRV904X_CPU_1_DM_MAILBOX_LINK_0_START_ADDR;
    }
    else if (cpuType == ADI_ADRV904X_CPU_TYPE_DFE)
    {
        uint32_t sdkdata_address;

        /* get the data address from the scratchpad register */
        recoveryAction = adrv904x_DfeSdkDataAddrGet(device, &sdkdata_address);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "DFE Mailbox Address Issue");
            return recoveryAction;
        }
        
        cpuAddr = sdkdata_address + ADI_LIBRARY_OFFSETOF(adrv904x_DfeSvcBbicBridgeSdkData_t, dfeMailboxBuffer) + ADI_LIBRARY_OFFSETOF(_adi_DfeMailboxBuffer_t, buffer);
    }

    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, cpuAddr, readData, NULL, byteCount);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "CPU Mailbox Buffer Read Issue");
        return recoveryAction;
    }

    for (i = 0U; i < byteCount; ++i)
    {
        data[i] = readData[i];
    }

    return recoveryAction;
}

static adi_adrv904x_ErrAction_e adrv904x_CpuMailboxBusyGet(adi_adrv904x_Device_t* const    device,
                                                           const adi_adrv904x_CpuType_e    cpuType,
                                                           uint8_t* const                  mailboxBusy)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, mailboxBusy);

    /* Read cpu_command_busy bit in cpu_command(0x00c3) register*/
    if (cpuType == ADI_ADRV904X_CPU_TYPE_0)
    {
        recoveryAction =  adrv904x_Core_Arm0Spi0CommandBusy_BfGet(device,
                                                                  NULL,
                                                                  ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                  mailboxBusy);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register Read (for CPU 0 Command Busy Bit) Issue");
            return recoveryAction;
        }
    }
    else if (cpuType == ADI_ADRV904X_CPU_TYPE_1)
    {
        recoveryAction =  adrv904x_Core_Arm1Spi0CommandBusy_BfGet(device,
                                                                  NULL,
                                                                  ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                  mailboxBusy);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register Read (for CPU 1 Command Busy Bit) Issue");
            return recoveryAction;
        }
    }
    else if (cpuType == ADI_ADRV904X_CPU_TYPE_DFE)
    {
        recoveryAction =  adrv904x_Core_A55Spi0CommandBusy_BfGet(device,
                                                                 NULL,
                                                                 ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                 mailboxBusy);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register Read (for DFE CPU Command Busy Bit) Issue");
            return recoveryAction;
        }
    }
     else
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuType, "Invalid CPU Type Provided");
        return recoveryAction;
    }

    return recoveryAction;
}

static adi_adrv904x_ErrAction_e adrv904x_CpuCmdStatusLinkIdGet(adi_adrv904x_Device_t* const    device,
                                                               const adi_adrv904x_CpuType_e    cpuType,
                                                               const adrv904x_LinkId_e         linkId,
                                                               uint8_t* const                  cmdStatByte)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t cmdByteIndex = 0U;
    uint8_t cmdByte = 0U;
    uint8_t cmdByteVerify = 0U;
    uint32_t readData = 0U;
    adi_adrv904x_CpuAddr_t *cpuAddr = NULL;
    static const uint8_t CPU_STATUS_MASK = 0x0FU;
    static const uint8_t CPU_STATUS_SHIFT_HI = 4U;
    static const uint8_t CPU_PENDING = 0x01U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, cmdStatByte);

    recoveryAction = adrv904x_CpuMailBoxRangeCheck( device, cpuType, linkId, 0U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "CPU Mailbox Range Check Issue");
        return recoveryAction;
    }
    
    if ((cpuType == ADI_ADRV904X_CPU_TYPE_0) ||
        (cpuType == ADI_ADRV904X_CPU_TYPE_1))
    {    
        /* get the particular processor's address map */
        cpuAddr = &(device->devStateInfo.cpu.cpuAddr[cpuType]);
        ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, cpuAddr);

        /* calculating command status register offset based on linkId status packing */
        cmdByteIndex = (uint8_t)linkId >> 2U;

        /* reading the command status register for given linkId */
        /* Two reads back to back for verify correct SPI operation */
        recoveryAction = adi_adrv904x_Register32Read(device, NULL, cpuAddr->cmdStatusAddr + cmdByteIndex, &readData, 0xFFU);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Command Status Read Issue (1)");
            return recoveryAction;
        }
        cmdByte = readData & 0xFFU;
        recoveryAction = adi_adrv904x_Register32Read(device, NULL, cpuAddr->cmdStatusAddr + cmdByteIndex, &readData, 0xFFU);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Command Status Read Issue (2)");
            return recoveryAction;
        }
        cmdByteVerify = readData & 0xFFU;
    }    
        else if(cpuType == ADI_ADRV904X_CPU_TYPE_DFE)
    {
        uint64_t cmdStatus = 0;
        /* reading the command status register for given linkId */
        /* Two reads back to back for verify correct SPI operation */
        recoveryAction = adrv904x_Core_A55Spi0CmdStatusDwl_BfGet(device,
                                                                 NULL,
                                                                 ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                 &cmdStatus);
        cmdByte = cmdStatus & 0xFFU;
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Command Status Read Issue (1)");
            return recoveryAction;
        }

        recoveryAction = adrv904x_Core_A55Spi0CmdStatusDwl_BfGet(device,
                                                                 NULL,
                                                                 ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                 &cmdStatus);
        cmdByteVerify = cmdStatus & 0xFFU;
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Command Status Read Issue (2)");
            return recoveryAction;
        }
    }

    /* identifying nibble location in command register for given linkId */
    /* Manually set the pending bit if verification fails */
    if ((((uint8_t)linkId >> 1) & (uint8_t)0x01) == (uint8_t)0x01)
    {
        *cmdStatByte = ((cmdByte == cmdByteVerify) ? ((cmdByte >> CPU_STATUS_SHIFT_HI) & CPU_STATUS_MASK) : ((cmdByte >> CPU_STATUS_SHIFT_HI) & CPU_STATUS_MASK) | CPU_PENDING);
    }
    else
    {
        *cmdStatByte = ((cmdByte == cmdByteVerify) ? (cmdByte & CPU_STATUS_MASK) : (cmdByte & CPU_STATUS_MASK) | CPU_PENDING);
    }

    return recoveryAction;
}

static adi_adrv904x_ErrAction_e adrv904x_CpuCmdStatusWait(adi_adrv904x_Device_t* const    device,
                                                          const adi_adrv904x_CpuType_e    cpuType,
                                                          const adrv904x_LinkId_e         linkId,
                                                          uint8_t* const                  cmdStatusByte,
                                                          const uint32_t                  timeout_us,
                                                          uint32_t                        waitInterval_us)
{
    adi_adrv904x_ErrAction_e  recoveryAction  = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t                eventCheck      = 0U;
    uint32_t                numEventChecks  = 0U;
    static const uint8_t    CPU_PENDING     = 0x01U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, cmdStatusByte);

    recoveryAction = adrv904x_CpuMailBoxRangeCheck(device, cpuType, linkId, 0U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "CPU Mailbox Range Check Issue");
        return recoveryAction;
    }

    waitInterval_us = (waitInterval_us > timeout_us) ? timeout_us : waitInterval_us;
    numEventChecks = (waitInterval_us == 0U) ? 1U : (timeout_us / waitInterval_us);

        /* timeout event check loop */
    for (eventCheck = 0U; eventCheck <= numEventChecks; ++eventCheck)
    {
        /* read status of linkId */
        recoveryAction = adrv904x_CpuCmdStatusLinkIdGet( device, cpuType, linkId, cmdStatusByte);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "LinkId Status Check Issue");
            return recoveryAction;
        }

        /* if pending bit is set for linkId of interest and the number of events have not expired, perform wait */
        if (((*cmdStatusByte & CPU_PENDING) > 0U) &&
            (eventCheck < numEventChecks))
        {
            recoveryAction = (adi_adrv904x_ErrAction_e) adi_common_hal_Wait_us(&device->common, waitInterval_us) ;
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "HAL Wait Request Issue");
                return recoveryAction;
            }
        }
        else
        {
                        break;
        }
    }

    /* if CPU Command did not complete within the timeout period */
    if ((*cmdStatusByte & CPU_PENDING) > 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_RESET_FEATURE;
        ADI_ERROR_REPORT(   &device->common,
                            ADI_ADRV904X_ERRSRC_CPU,
                            ADI_ADRV904X_ERRCODE_CPU_CMD_TIMEOUT,
                            recoveryAction,
                            *cmdStatusByte,
                            "Timed out waiting for command response");
        return recoveryAction;
    }

    return recoveryAction;
}

/*****************************************************************************/
/***** Public functions' definition ******************************************/
/*****************************************************************************/
ADI_API adi_adrv904x_CpuAddr_t* adrv904x_CpuAddrGet(adi_adrv904x_Cpu_t* const       cpu,
                                                    const adi_adrv904x_CpuType_e    cpuType)
{
    if (cpu == NULL)
    {
        return NULL;
    }
    return &cpu->cpuAddr[cpuType];
}

/* Interprets the memory at buf as a little-endian 2 or 4 byte integer and returns it as
 * a uint32_t */
ADI_API uint32_t adrv904x_CpuIntFromBytesGet(const uint8_t* const buf, const uint8_t size)
{
    uint32_t result = 0U;
    uint8_t i = 0U;

    if (buf == NULL)
    {
        return 0U;
    }

    if (size <= 4U)
    {
        for (i = 0U; i < size; ++i)
        {
            result |= (uint32_t)(*(buf + i)) << (i * 8U);
        }
    }

    return result;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CpuInitialize(adi_adrv904x_Device_t* const device)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adi_adrv904x_Cpu_t* cpu = NULL;
    adi_adrv904x_CpuType_e cpuType = ADI_ADRV904X_CPU_TYPE_UNKNOWN;
    uint8_t idx = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    cpu = &device->devStateInfo.cpu;

    //This is retrieved from the FW image directly
    cpu->devProfileAddr = 0;

    for (idx = 0U; idx < ADI_ADRV904X_MAX_NUM_CPUS; ++idx)
    {
        /* Skip ADI_ADRV904X_CPU_TYPE_MAX_RADIO value */
        if (idx == (uint8_t)ADI_ADRV904X_CPU_TYPE_MAX_RADIO) 
        {
            continue;
        }
        cpuType = (adi_adrv904x_CpuType_e)idx;
               
        recoveryAction = adrv904x_CpuAddrInitialize(&cpu->cpuAddr[idx], cpuType);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid CPU Type Provided");
            return recoveryAction;
            break;
        }

        if (idx < (uint8_t)ADI_ADRV904X_CPU_TYPE_MAX_RADIO) 
        {
            /* Set the memory bank configuration for the CPU0 and CPU1. Must be done here
            * before any attempt to program the CPU image is made.
            */
            ADRV904X_SPIWRITEBYTE_RETURN("CPU_ADDR_MEM_BANK_CTRL", cpu->cpuAddr[idx].memBankCtrlAddr, ADRV904X_CPU_MEM_BANK_CTRL_REG_VAL, recoveryAction);
        }
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CpuCmdWrite(adi_adrv904x_Device_t* const    device,
                                                      const adi_adrv904x_CpuType_e    cpuType,
                                                      const adrv904x_LinkId_e         linkId,
                                                      const adrv904x_CpuCmdId_t       cmdId,
                                                      adrv904x_CpuCmd_t* const        cmd,
                                                      const uint32_t                  payloadSize)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t cpuCommandBusy = 0U;
    uint32_t exceptionValue = 0U;
    uint32_t timeout_us = ADI_ADRV904X_SENDCPUCMD_TIMEOUT_US;
    uint32_t waitInterval_us = ADI_ADRV904X_SENDCPUCMD_INTERVAL_US;
    uint32_t eventCheck = 0U;
    uint32_t numEventChecks = 0U;
    adi_adrv904x_CpuAddr_t *cpuAddr = NULL;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, cmd);

    if ((cpuType == ADI_ADRV904X_CPU_TYPE_0) ||
        (cpuType == ADI_ADRV904X_CPU_TYPE_1))
    {
        /* check for valid cmd id */
        if (cmdId >= ADRV904X_CPU_CMD_ID_NUM_CMDS)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   cmdId,
                                   "Invalid Command ID Provided");
            return recoveryAction;
        }
    }
    else if(cpuType == ADI_ADRV904X_CPU_TYPE_DFE)
    {
        /* check for valid cmd id */
        if (cmdId > ADRV904X_DFE_SVC_CPU_CMD_ID_DFE_APPLICATION)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   cmdId,
                                   "Invalid Command ID Provided");
            return recoveryAction;
        }
    }
    else
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               cpuType,
                               "Invalid CPU Type Provided");
        return recoveryAction;
    }

    /* setting a 2 second timeout for mailbox busy bit to be clear (can't send an cpu mailbox command until mailbox is ready) */
#if ADI_ADRV904X_SENDCPUCMD_INTERVAL_US > ADI_ADRV904X_SENDCPUCMD_TIMEOUT_US
    waitInterval_us = timeout_us;
#elif ADI_ADRV904X_SENDCPUCMD_INTERVAL_US == 0
    waitInterval_us = timeout_us;
#endif
    numEventChecks = timeout_us / waitInterval_us;

    /* timeout event loop to permit non-blocking of thread */
    for (eventCheck = 0U; eventCheck <= numEventChecks; ++eventCheck)
    {
        recoveryAction = adrv904x_CpuMailboxBusyGet( device, cpuType, &cpuCommandBusy);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Cpu Mailbox busy state cannot be read");
            return recoveryAction;
        }

        if (cpuCommandBusy == ADI_TRUE)
        {
            recoveryAction = (adi_adrv904x_ErrAction_e) adi_common_hal_Wait_us(&device->common, waitInterval_us);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "HAL Wait Request Issue");
                return recoveryAction;
            }

            recoveryAction = adi_adrv904x_CpuCheckException(device, &exceptionValue);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "CPU exception state cannot be read.");
                return recoveryAction;
            }

            if (exceptionValue != 0U)
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_RESET_DEVICE;
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, exceptionValue, "CPU exception detected waiting for mailbox ready.");
                return recoveryAction;
            }


            exceptionValue = 0U;
            recoveryAction = adi_adrv904x_DfeCpuCheckException(device, &exceptionValue);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "DFE CPU exception state cannot be read.");
                return recoveryAction;
            }

            if (exceptionValue != 0U)
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_RESET_DEVICE;
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, exceptionValue, "DFE CPU exception detected waiting for mailbox ready.");
                return recoveryAction;
            }

        }
        else
        {
            break;
        }
    }

    /* if busy bit remains set after timeout event loop function is exited, otherwise command is sent */
    if (cpuCommandBusy == ADI_TRUE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_RESET_FEATURE;
        ADI_ERROR_REPORT(   &device->common,
                            ADI_ADRV904X_ERRSRC_CPU,
                            ADI_ADRV904X_ERRCODE_CPU_CMD_TIMEOUT,
                            recoveryAction,
                            cpuCommandBusy,
                            "CPU Mailbox Busy; Timeout Occurred");
        return recoveryAction;
    }

    cpuAddr = &(device->devStateInfo.cpu.cpuAddr[cpuType]);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, cpuAddr);

    ++cpuAddr->curTransactionId[linkId];
    cmd->cmdId = ADRV904X_HTOCS(cmdId);
    cmd->tId = ADRV904X_HTOCS(cpuAddr->curTransactionId[linkId]);

    recoveryAction = adrv904x_CpuMailboxBufferWrite(device,
                                                    cpuType,
                                                    linkId,
                                                    (const uint8_t *)cmd,
                                                    payloadSize+sizeof(adrv904x_CpuCmd_t));
    if(recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "CPU Mailbox Buffer Write Issue");
        return recoveryAction;
    }

    if ((cpuType == ADI_ADRV904X_CPU_TYPE_0) ||
           (cpuType == ADI_ADRV904X_CPU_TYPE_1))
    {
        ADRV904X_SPIWRITEBYTE_RETURN("CPU_COMMAND", cpuAddr->cmdAddr, linkId, recoveryAction);
    }
    else if (cpuType == ADI_ADRV904X_CPU_TYPE_DFE)
    {
        recoveryAction = adrv904x_Core_A55Spi0Command_BfSet(device,
                                                                 NULL,
                                                                 ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                 (uint8_t)linkId);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register write (for DFE CPU Command) Issue");
            return recoveryAction;
        }
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CpuCmdRespRead(adi_adrv904x_Device_t* const    device,
                                                         const adi_adrv904x_CpuType_e    cpuType,
                                                         const adrv904x_LinkId_e         linkId,
                                                         adrv904x_CpuCmdId_t*const       cmdId,
                                                         adrv904x_CpuCmdResp_t* const    cmdRsp,
                                                         const uint32_t                  payloadSize,
                                                         adrv904x_CpuCmdStatus_e* const  status)
{
    adi_adrv904x_ErrAction_e        recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_CpuCmdTransactionId_t  rxdTransactionId    = 0U;
    const adi_adrv904x_CpuAddr_t*   cpuAddr             = NULL;
    uint8_t                         cmdStatusByte       = 0U;
    adrv904x_CpuCmdStatus_e         cmdStatus           = ADRV904X_CPU_CMD_STATUS_GENERIC;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, cmdRsp);

    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, cmdId);

    /* Caller is allowed to pass in NULL for status,
     * so we must check every time before dereferencing.
     */
    if (status != NULL)
    {
        *status = ADRV904X_CPU_CMD_STATUS_GENERIC;
    }

    /* Wait for a command response from the CPU */
    recoveryAction = adrv904x_CpuCmdStatusWait( device,
                                                cpuType,
                                                linkId,
                                                &cmdStatusByte,
                                                ADI_ADRV904X_READCPURESP_TIMEOUT_US,
                                                ADI_ADRV904X_READCPURESP_INTERVAL_US);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        if (status != NULL)
        {
            *status = ADRV904X_CPU_CMD_STATUS_LINK_ERROR;
        }
        ADI_ADRV904X_ERROR_INFO_GET_REPORT( ADI_ADRV904X_ERRSRC_CPU,
                                            ADI_ADRV904X_ERRCODE_CPU_CMD_TIMEOUT,
                                            ADI_NO_VARIABLE,
                                            recoveryAction);
        return recoveryAction;
    }

    cpuAddr = &(device->devStateInfo.cpu.cpuAddr[cpuType]);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, cpuAddr);

    /* Read the response from the CPU */
    recoveryAction = adrv904x_CpuMailboxBufferRead( device,
                                                    cpuType,
                                                    linkId,
                                                    (uint8_t *)cmdRsp,
                                                    payloadSize + sizeof(adrv904x_CpuCmdResp_t));
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        if (status != NULL)
        {
            *status = ADRV904X_CPU_CMD_STATUS_LINK_ERROR;
        }
        ADI_ADRV904X_ERROR_INFO_GET_REPORT( ADI_ADRV904X_ERRSRC_CPU,
                                            ADI_ADRV904X_ERRCODE_CPU_MAILBOX_READ,
                                            ADI_NO_VARIABLE,
                                            recoveryAction);
        return recoveryAction;
    }

    /* Extract the cmd ID and transaction ID from the response */
    *cmdId = ADRV904X_CTOHS(cmdRsp->cmdId);
    rxdTransactionId = ADRV904X_CTOHS(cmdRsp->tId);

    /* Verify the transaction ID */
    if (rxdTransactionId != cpuAddr->curTransactionId[linkId])
    {
        if (status != NULL)
        {
            *status = ADRV904X_CPU_CMD_STATUS_UNEXPECTED_TRANSACTION_ID;
        }
        recoveryAction = ADI_ADRV904X_ERR_ACT_RESET_FEATURE;
        ADI_ERROR_REPORT(   &device->common,
                            ADI_ADRV904X_ERRSRC_CPU,
                            ADI_ADRV904X_ERRCODE_CPU_CMD_ID,
                            recoveryAction,
                            rxdTransactionId,
                            "Command Failed: Unexpected Transaction Id");
        return recoveryAction;
    }
    else
    {
        /* Transaction ID is correct. Check the command status for failure. */
        cmdStatus = (adrv904x_CpuCmdStatus_e)ADRV904X_CTOHS(cmdRsp->status);

        if (status != NULL)
        {
            *status = cmdStatus;
        }

        if (cmdStatus != ADRV904X_CPU_CMD_STATUS_NO_ERROR)
        {
            ADI_ADRV904X_ERROR_INFO_GET_REPORT( ADI_ADRV904X_ERRSRC_CPU,
                                                ADI_ADRV904X_ERRCODE_CPU_CMD_RESPONSE,
                                                cmdStatus,
                                                recoveryAction);
            return recoveryAction;
        }
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CpuCmdSend(adi_adrv904x_Device_t* const    device,
                                                     const adi_adrv904x_CpuType_e    cpuType,
                                                     const adrv904x_LinkId_e         linkId,
                                                     const adrv904x_CpuCmdId_t       cmdId,
                                                     void* const                     pCmdPayload,
                                                     const size_t                    cmdPayloadSz,
                                                     void* const                     pRespPayload,
                                                     const size_t                    respPayloadSz,
                                                     adrv904x_CpuCmdStatus_e* const  status)
{
    /* Union to determine maximum buffer size needed for both CPU command and response */
    typedef union
    {
        uint8_t maxCmdBuf[ADRV904X_CPU_CMD_MAX_SIZE_BYTES];
        uint8_t maxCmdRspBuf[ADRV904X_CPU_CMD_RESP_MAX_SIZE_BYTES];
    } adrv904x_MaxCpuCmdBufSz_t;

    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_CpuCmd_t *txCmd = NULL;
    adrv904x_CpuCmdResp_t *rxRsp = NULL;
    adrv904x_CpuCmdId_t rspCmdId;
    adrv904x_CpuCmdStatus_e cmdStatus;
    uint8_t cmdBuf[sizeof(adrv904x_MaxCpuCmdBufSz_t)];

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    /* Initialize the caller's status parameter, if applicable. */
    if (status != NULL)
    {
        *status = ADRV904X_CPU_CMD_STATUS_GENERIC;
    }

    /* If the command payload size is nonzero, verify the pointer is not null */
    if (cmdPayloadSz != 0)
    {
        ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, pCmdPayload);
    }
    else if (pCmdPayload != NULL)
    {
        /* If the command payload size is zero, pointer must be null */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, pCmdPayload, "pCmdPayload must be NULL if cmdPayloadSz is 0");
        return recoveryAction;
    }

    /* If the command response payload size is nonzero, verify the pointer is not null */
    if (respPayloadSz != 0)
    {
        ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, pRespPayload);
    }
    else if (pRespPayload != NULL)
    {
        /* If the command response payload size is zero, pointer must be null */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, pRespPayload, "pRespPayload must be NULL if respPayloadSz is 0");
        return recoveryAction;
    }

    /* Verify cpuType */
    if ((cpuType >= ADI_ADRV904X_CPU_TYPE_MAX_RADIO) ||
            (cpuType <= ADI_ADRV904X_CPU_TYPE_UNKNOWN))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuType, "Invalid CPU type");
        return recoveryAction;
    }

    /* Verify cmdId */
    if (cmdId >= ADRV904X_CPU_CMD_ID_NUM_CMDS)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cmdId, "Invalid command ID");
        return recoveryAction;
    }

    /* Verify linkId */
    if ((linkId >= ADRV904X_LINK_ID_MAX) || (linkId <= ADRV904X_LINK_ID_UNKNOWN))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, linkId, "Invalid link ID");
        return recoveryAction;
    }

    /* Verify command payload size is acceptable */
    if (cmdPayloadSz > (sizeof(cmdBuf) - sizeof(adrv904x_CpuCmd_t)))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cmdPayloadSz, "cmdPayloadSz is too large for command buffer.");
        return recoveryAction;
    }

    /* Verify response payload size is acceptable */
    if (respPayloadSz > (sizeof(cmdBuf) - sizeof(adrv904x_CpuCmdResp_t)))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, respPayloadSz, "respPayloadSz is too large for command buffer.");
        return recoveryAction;
    }

    /* Overlay the command header and response header on the buffer */
    txCmd = (adrv904x_CpuCmd_t*)cmdBuf;
    rxRsp = (adrv904x_CpuCmdResp_t*)cmdBuf;

    /* Copy the caller's payload to the correct location. Size is verified above. */
    if (cmdPayloadSz != 0)
    {
        ADI_LIBRARY_MEMCPY((void*)((uint8_t*)txCmd + sizeof(adrv904x_CpuCmd_t)), pCmdPayload, cmdPayloadSz) ;
    }

    /* Send the command */
    recoveryAction = adrv904x_CpuCmdWrite(device, cpuType, linkId, cmdId, txCmd, cmdPayloadSz);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to send command to CPU");
        goto debug;
    }

    /* Wait for the response, then receive the payload into rxRsp. rxRsp is verified to be able to hold respPayloadSz payload above. */
    recoveryAction = adrv904x_CpuCmdRespRead(device, cpuType, linkId, &rspCmdId, rxRsp, (uint32_t)respPayloadSz, &cmdStatus);

    /* Copy cmd status out for caller, if applicable */
    if (status != NULL)
    {
        *status = cmdStatus;
    }

    /* Process any cmd errors */
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        if (cmdStatus == ADRV904X_CPU_CMD_STATUS_CMD_FAILED)
        {
            /* For command-specific failures, copy the payload to the caller's buffer before aborting. */
            if (respPayloadSz != 0)
            {
                ADI_LIBRARY_MEMCPY(pRespPayload, (void*)((uint8_t*)rxRsp + sizeof(adrv904x_CpuCmdResp_t)), respPayloadSz);
            }
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "CPU Command Response Error");
        }
        else
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "No CPU Command Response Received");
        }

        goto debug;
    }

    /* Copy the payload to the caller's buffer */
    if (respPayloadSz != 0)
    {
        ADI_LIBRARY_MEMCPY(pRespPayload, (void*)((uint8_t*)rxRsp + sizeof(adrv904x_CpuCmdResp_t)), respPayloadSz);
    }

    return recoveryAction;

debug:
    /* Disable Error Clearing for Private API Call Case e.g. CpuPing */
    ++device->common.publicCnt;

    adrv904x_CpuErrorDebugCheck(device);


    adrv904x_DfeCpuErrorDebugCheck(device);

    --device->common.publicCnt;

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CpuChannelMappingGet(adi_adrv904x_Device_t* const    device,
                                                               const adi_adrv904x_Channels_e   channel,
                                                               const adrv904x_CpuObjectId_e    objId,
                                                               adi_adrv904x_CpuType_e* const   cpuType)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t cpuNum = 2u; /* Set to bad CPU */
    uint32_t idx = 0u;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_NULL_PTR_RETURN(cpuType);

    /* Detect any ORx channel commands */
    if (ADRV904X_CPU_OBJID_IS_ORX(objId))
    {
        for (idx = 0U; idx < ADI_ADRV904X_MAX_ORX; idx++)
        {
            if ((uint32_t)channel == ((uint32_t)1U << idx))
            {
                cpuNum = device->initExtract.orxCpuConfig[idx];
                recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
                break;
            }
        }
    }
    /* The SERDES cals spread the cals for different lanes evenly across the available CPU cores.
     * For these cals, the channel passed in is the SERDES lane of interest
     */
    else if ((objId == ADRV904X_CPU_OBJID_IC_SERDES) ||
             (objId == ADRV904X_CPU_OBJID_TC_SERDES))
    {
        for (idx = 0U; idx < ADI_ADRV904X_MAX_SERDES_LANES; idx++)
        {
            if ((uint32_t)channel == ((uint32_t)1U << idx))
            {
                cpuNum = device->initExtract.jesd204DesLaneCpuConfig[idx];
                recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
                break;
            }
        }
    }
    /* The RC TUNER CAL is only supported on CPU0 */
    else if (objId == ADRV904X_CPU_OBJID_IC_RC_TUNER)
    {
        cpuNum = 0;
        recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    }
    else /* Assume Rx or Tx applies */
    {
        for (idx = 0U; idx < ADI_ADRV904X_MAX_CHANNELS; idx++)
        {
            if ((uint32_t)channel == ((uint32_t)1U << idx))
            {
                cpuNum = device->initExtract.rxTxCpuConfig[idx];
                recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
                break;
            }
        }
    }

    if (recoveryAction == ADI_ADRV904X_ERR_ACT_NONE)
    {
        if (cpuNum == 0u)
        {
            *cpuType = ADI_ADRV904X_CPU_TYPE_0;
        }
        else if (cpuNum == 1u)
        {
            *cpuType = ADI_ADRV904X_CPU_TYPE_1;
        }
        else
        {
            *cpuType = ADI_ADRV904X_CPU_TYPE_UNKNOWN;
        }
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CpuRamAccessStart(adi_adrv904x_Device_t* const            device,
                                                            const adrv904x_CpuCmd_CaptureRamType_e  captureRamType,
                                                            const adi_adrv904x_Channels_e           channelNumber,
                                                            uint8_t* const                          success)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_CpuCmd_RamAccessStart_t cmd;
    adrv904x_CpuCmd_RamAccessStartResp_t cmdRsp;
    adrv904x_CpuCmdStatus_e cmdStatus = ADRV904X_CPU_CMD_STATUS_GENERIC;
    adrv904x_CpuErrorCode_e cpuErrorCode = ADRV904X_CPU_SYSTEM_CAPTURE_RAM_LOCK_ERROR;
    adi_adrv904x_CpuType_e cpuType = ADI_ADRV904X_CPU_TYPE_UNKNOWN;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, success);

    ADI_LIBRARY_MEMSET(&cmd, 0, sizeof(adrv904x_CpuCmd_RamAccessStart_t));
    ADI_LIBRARY_MEMSET(&cmdRsp, 0, sizeof(adrv904x_CpuCmd_RamAccessStartResp_t));

    /* Initialize success flag to false */
    *success = 0U;

    /* Get the CPU assigned to this channel */
    recoveryAction = adrv904x_CpuChannelMappingGet(device, channelNumber, ADRV904X_CPU_OBJID_SYSTEM_END, &cpuType);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to get CPU channel mapping");
        return recoveryAction;
    }

    /* Setup cmd-specific payload */
    cmd.captureRamType = (adrv904x_CpuCmd_CaptureRamType_t)captureRamType;
    cmd.channelNumber = ADRV904X_HTOCL((uint32_t)channelNumber);

    /* Send command and receive response */
    recoveryAction = adrv904x_CpuCmdSend(   device,
                                            cpuType,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_CPU_CMD_ID_RAM_ACCESS_START,
                                            (void*)&cmd,
                                            sizeof(cmd),
                                            (void*)&cmdRsp,
                                            sizeof(cmdRsp),
                                            &cmdStatus);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_CPU_CMD_RESP_CHECK_RETURN(cmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction);
    }

    /* Extract cmd-specific response */
    cpuErrorCode = (adrv904x_CpuErrorCode_e)ADRV904X_CTOHL((uint32_t)cmdRsp.status);

    /* If NO_ERROR was returned, the start command was successful */
    if (cpuErrorCode == ADRV904X_CPU_NO_ERROR)
    {
        *success = 1U;
    }
    else if (cpuErrorCode != ADRV904X_CPU_SYSTEM_CAPTURE_RAM_LOCK_ERROR)
    {
        /* Any error other than LOCK_ERROR is unexpected */
        ADI_ADRV904X_ERROR_INFO_GET_REPORT( ADI_ADRV904X_ERRSRC_CPU,
                                            ADI_ADRV904X_ERRCODE_CPU_RAM_LOCK,
                                            cpuErrorCode,
                                            recoveryAction);
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CpuRamAccessStop( adi_adrv904x_Device_t* const            device,
                                                            const adrv904x_CpuCmd_CaptureRamType_e  captureRamType,
                                                            const adi_adrv904x_Channels_e           channelNumber,
                                                            uint8_t* const                          success)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_CpuCmd_RamAccessStop_t cmd;
    adrv904x_CpuCmd_RamAccessStopResp_t cmdRsp;
    adrv904x_CpuCmdStatus_e cmdStatus = ADRV904X_CPU_CMD_STATUS_GENERIC;
    adrv904x_CpuErrorCode_e cpuErrorCode = ADRV904X_CPU_SYSTEM_CAPTURE_RAM_LOCK_ERROR;
    adi_adrv904x_CpuType_e cpuType = ADI_ADRV904X_CPU_TYPE_UNKNOWN;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, success);
    ADI_LIBRARY_MEMSET(&cmd, 0, sizeof(adrv904x_CpuCmd_RamAccessStop_t));
    ADI_LIBRARY_MEMSET(&cmdRsp, 0, sizeof(adrv904x_CpuCmd_RamAccessStopResp_t));

    /* Initialize success flag to false */
    *success = 0U;

    /* Get the CPU assigned to this channel */
    recoveryAction = adrv904x_CpuChannelMappingGet(device, channelNumber, ADRV904X_CPU_OBJID_SYSTEM_END, &cpuType);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to get CPU channel mapping");
        return recoveryAction;
    }

    /* Setup cmd-specific payload */
    cmd.captureRamType = (adrv904x_CpuCmd_CaptureRamType_t)captureRamType;
    cmd.channelNumber = ADRV904X_HTOCL((uint32_t)channelNumber);

    /* Send command and receive response */
    recoveryAction = adrv904x_CpuCmdSend(   device,
                                            cpuType,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_CPU_CMD_ID_RAM_ACCESS_STOP,
                                            (void*)&cmd,
                                            sizeof(cmd),
                                            (void*)&cmdRsp,
                                            sizeof(cmdRsp),
                                            &cmdStatus);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_CPU_CMD_RESP_CHECK_RETURN(cmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction);
    }

    /* Extract cmd-specific response */
    cpuErrorCode = (adrv904x_CpuErrorCode_e)ADRV904X_CTOHL((uint32_t)cmdRsp.status);

    /* If NO_ERROR was returned, the stop command was successful */
    if (cpuErrorCode == ADRV904X_CPU_NO_ERROR)
    {
        *success = 1U;
    }
    else if (cpuErrorCode != ADRV904X_CPU_SYSTEM_CAPTURE_RAM_UNLOCK_ERROR)
    {
        /* Any error other than UNLOCK_ERROR is unexpected */
        ADI_ADRV904X_ERROR_INFO_GET_REPORT( ADI_ADRV904X_ERRSRC_CPU,
                                            ADI_ADRV904X_ERRCODE_CPU_RAM_LOCK,
                                            cpuErrorCode,
                                            recoveryAction);
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CpuPing(adi_adrv904x_Device_t* const    device,
                                                  const adi_adrv904x_CpuType_e    cpuType,
                                                  const uint32_t                  writeData,
                                                  uint32_t* const                 readData)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_CpuCmd_Ping_t pingCmd;
    adrv904x_CpuCmd_PingResp_t pingCmdRsp;
    adrv904x_CpuCmdStatus_e cmdStatus = ADRV904X_CPU_CMD_STATUS_GENERIC;
    adrv904x_CpuErrorCode_e cpuErrorCode = ADRV904X_CPU_SYSTEM_SIMULATED_ERROR;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, readData);

    ADI_LIBRARY_MEMSET(&pingCmd, 0, sizeof(adrv904x_CpuCmd_Ping_t));
    ADI_LIBRARY_MEMSET(&pingCmdRsp, 0, sizeof(adrv904x_CpuCmd_PingResp_t));

    /* Setup ping-specific payload */
    pingCmd.echoData = ADRV904X_HTOCL(writeData);

    if ((cpuType == ADI_ADRV904X_CPU_TYPE_0) ||
        (cpuType == ADI_ADRV904X_CPU_TYPE_1))
    {
        recoveryAction = adrv904x_CpuCmdSend(device,
                                             cpuType,
                                             ADRV904X_LINK_ID_0,
                                             ADRV904X_CPU_CMD_ID_PING,
                                             (void*)&pingCmd,
                                             sizeof(pingCmd),
                                             (void*)&pingCmdRsp,
                                             sizeof(pingCmdRsp),
                                             &cmdStatus);

        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_ADRV904X_CPU_CMD_RESP_CHECK_RETURN(pingCmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction);
        }
    }
    /* Send command and receive response */
    else if (cpuType == ADI_ADRV904X_CPU_TYPE_DFE)
    {
        adrv904x_DfeSvcCmdStatus_t svcCmdStatus = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;

        recoveryAction = adrv904x_DfeSvcCmdSend(device,
                                                ADRV904X_LINK_ID_0,
                                                ADRV904X_DFE_SVC_CMD_ID_PING,
                                                (void*)&pingCmd,
                                                sizeof(pingCmd),
                                                (void*)&pingCmdRsp,
                                                sizeof(pingCmdRsp),
                                                &svcCmdStatus);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_ADRV904X_DFE_SVC_CMD_RESP_CHECK_RETURN(pingCmdRsp.status, svcCmdStatus, cpuErrorCode, recoveryAction);
        }

        /* Send command for the DFE processor to ping the radio */
        recoveryAction = adrv904x_DfeSvcCmdSend(device,
                                                ADRV904X_LINK_ID_0,
                                                ADRV904X_DFE_SVC_CMD_ID_RADIO_PING,
                                                (void*)&pingCmd,
                                                sizeof(pingCmd),
                                                (void*)&pingCmdRsp,
                                                sizeof(pingCmdRsp),
                                                &svcCmdStatus);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_ADRV904X_DFE_SVC_CMD_RESP_CHECK_RETURN(pingCmdRsp.status, svcCmdStatus, cpuErrorCode, recoveryAction);
        }
    }

    /* Extract ping-specific response */
    cpuErrorCode = (adrv904x_CpuErrorCode_e)ADRV904X_CTOHL((uint32_t)pingCmdRsp.status);
    *readData    = ADRV904X_CTOHL(pingCmdRsp.echoData);

    /* Handle ping-specific errors */
    if ((cpuErrorCode != ADRV904X_CPU_NO_ERROR) || (*readData != writeData))
    {
        ADI_ADRV904X_ERROR_INFO_GET_REPORT( ADI_ADRV904X_ERRSRC_CPU,
                                            ADI_ADRV904X_ERRCODE_CPU_PING,
                                            ADI_NO_VARIABLE,
                                            recoveryAction);
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CpuEfuseGet(adi_adrv904x_Device_t* const device,
                                                      const uint32_t               address,
                                                      uint32_t* const              value)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_CpuCmd_EfuseGet_t efuseCmd;
    adrv904x_CpuCmd_EfuseGetResp_t efuseCmdRsp;
    adrv904x_CpuCmdStatus_e cmdStatus = ADRV904X_CPU_CMD_STATUS_GENERIC;
    adrv904x_CpuErrorCode_e cpuErrorCode = ADRV904X_CPU_SYSTEM_SIMULATED_ERROR;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, value);

    ADI_LIBRARY_MEMSET(&efuseCmd, 0, sizeof(adrv904x_CpuCmd_EfuseGet_t));
    ADI_LIBRARY_MEMSET(&efuseCmdRsp, 0, sizeof(adrv904x_CpuCmd_EfuseGetResp_t));

    if (address >= 0x20U)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid EFUSE address (must be 0 - 0x1F)");
        return recoveryAction;
    }

    /* Setup efuse-specific payload */
    efuseCmd.addr = ADRV904X_HTOCL(address);

    /* Send command and receive response */
    recoveryAction = adrv904x_CpuCmdSend(device,
                                         ADI_ADRV904X_CPU_TYPE_0,   /* assume CPU0 is the primary CPU */
                                         ADRV904X_LINK_ID_0,
                                         ADRV904X_CPU_CMD_ID_EFUSE_GET,
                                         (void*)&efuseCmd,
                                         sizeof(efuseCmd),
                                         (void*)&efuseCmdRsp,
                                         sizeof(efuseCmdRsp),
                                         &cmdStatus);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_CPU_CMD_RESP_CHECK_RETURN(efuseCmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction);
    }

    /* Extract efuse-specific response */
    *value = ADRV904X_CTOHL(efuseCmdRsp.value);

    return recoveryAction;
}

static const uint32_t LUT_CRC32[16] = {
    0x00000000u,
    0x04c11db7u,
    0x09823b6eu,
    0x0d4326d9u,
    0x130476dcu,
    0x17c56b6bu,
    0x1a864db2u,
    0x1e475005u,
    0x2608edb8u,
    0x22c9f00fu,
    0x2f8ad6d6u,
    0x2b4bcb61u,
    0x350c9b64u,
    0x31cd86d3u,
    0x3c8ea00au,
    0x384fbdbdu
};

ADI_API uint32_t adrv904x_Crc32ForChunk(const uint8_t       buf[],
                                        const uint32_t      bufLen,
                                        const uint32_t      seedCrc,
                                        const uint8_t       finalCrc)
{
    uint32_t i;
    uint32_t a, b, c, d;

    if (buf == NULL)
    {
        return 0;
    }

    a = seedCrc;

    for (i = 0u; i < bufLen; i++)
    {
        b = (a >> 28u) & 0xfu;
        c = LUT_CRC32[b];
        d = buf[i];
        a = (a << 4u) | (d >> 4u);
        a = a ^ c;
        b = (a >> 28u) & 0xfu;
        c = LUT_CRC32[b];
        a = (a << 4u) | (d & 0xfu);
        a = a ^ c;
    }

    if (finalCrc > 0)
    {
        for (i = 0u; i < 4u; i++)
        {
            b = (a >> 28u) & 0xfu;
            c = LUT_CRC32[b];
            a = (a << 4u);
            a = a ^ c;
            b = (a >> 28u) & 0xfu;
            c = LUT_CRC32[b];
            a = (a << 4u);
            a = a ^ c;
        }
    }

    return a;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_HealthMonitorPrivateCpuStatusGet( adi_adrv904x_Device_t* const                        device,
                                                                            adrv904x_HealthMonitorPrivateCpuStatus_t* const     healthMonitorStatus)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, healthMonitorStatus);

    ADI_LIBRARY_MEMSET(healthMonitorStatus, 0, sizeof(adrv904x_HealthMonitorPrivateCpuStatus_t));

    /* TODO: When Private CPU Health Monitoring becomes useful, populate return data appropriately */
    /* For now, simply return all zero data */
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CpuGpioSet(adi_adrv904x_Device_t* const device,
                                                     const adi_adrv904x_GpioPinSel_e gpioSelect,
                                                     const adrv904x_CpuCmd_GpioSignal_e cpuGpioSignal,
                                                     const uint8_t isInput)
{
    adi_adrv904x_ErrAction_e  recoveryAction  = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_CpuCmd_SetGpio_t       setGpioCmd;
    adrv904x_CpuCmd_SetGpioResp_t   setGpioResp;
    adrv904x_CpuCmd_GetGpio_t       getGpioCmd;
    adrv904x_CpuCmd_GetGpioResp_t   getGpioResp;
    uint32_t cpuTypeIdx = 0U;
    adrv904x_CpuErrorCode_e cpuErrorCode = ADRV904X_CPU_SYSTEM_SIMULATED_ERROR;
    adrv904x_CpuCmdStatus_e cmdStatus = ADRV904X_CPU_CMD_STATUS_GENERIC;
    adi_adrv904x_GpioSignal_e signal = ADI_ADRV904X_GPIO_SIGNAL_UNUSED;

    adi_adrv904x_GpioSignal_e outputSignals[ADI_ADRV904X_GPIO_COUNT] = {
        ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_0,
        ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_1,
        ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_2,
        ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_3,
        ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_4,
        ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_5,
        ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_6,
        ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_7,
        ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_8,
        ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_9,
        ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_10,
        ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_11,
        ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_12,
        ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_13,
        ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_14,
        ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_15,
        ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_16,
        ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_17,
        ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_18,
        ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_19,
        ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_20,
        ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_21,
        ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_22,
        ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_23
    };

    adi_adrv904x_GpioSignal_e inputSignals[ADI_ADRV904X_GPIO_COUNT] = {
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_0,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_1,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_2,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_3,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_4,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_5,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_6,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_7,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_8,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_9,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_10,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_11,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_12,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_13,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_14,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_15,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_16,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_17,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_18,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_19,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_20,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_21,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_22,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_23
    };

    /* Prepare the command payload */
    getGpioCmd.signal = (uint8_t)cpuGpioSignal;

    /* Send command and receive response */
    recoveryAction = adrv904x_CpuCmdSend(device,
                                         ADI_ADRV904X_CPU_TYPE_0,
                                         ADRV904X_LINK_ID_0,
                                         ADRV904X_CPU_CMD_ID_GET_GPIO,
                                         (void*)&getGpioCmd,
                                         sizeof(getGpioCmd),
                                         (void*)&getGpioResp,
                                         sizeof(getGpioResp),
                                         &cmdStatus);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_CPU_CMD_RESP_CHECK_RETURN(getGpioResp.cmdStatus, cmdStatus, cpuErrorCode, recoveryAction);
    }

    /* Release the pin assigned to same CPU signal */
    if (getGpioResp.pin != (uint8_t)ADI_ADRV904X_GPIO_INVALID)
    {
        if (isInput == ADI_TRUE)
        {
            signal = inputSignals[getGpioResp.pin];
        }
        else
        {
            signal = outputSignals[getGpioResp.pin];
        }

        recoveryAction = adrv904x_GpioSignalRelease(device, (adi_adrv904x_GpioPinSel_e)getGpioResp.pin, signal, ADI_ADRV904X_CHOFF);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   getGpioResp.pin,
                                   "GpioSignalRelease issue");
            return recoveryAction;
        }

        /* Send GPIO set command to CPU to disable the pin */
        /* Fill out set gpio cmd params with user-provided params */
        setGpioCmd.signal = (uint8_t)cpuGpioSignal;
        setGpioCmd.pin = (uint8_t)getGpioResp.pin;
        setGpioCmd.polarity = (uint8_t)ADRV904X_CPU_CMD_GPIO_PIN_POLARITY_NORMAL;
        setGpioCmd.enable = (uint8_t)ADRV904X_CPU_CMD_GPIO_PIN_DISABLE;

        /* For each CPU, send the SET_GPIO command, wait for a response, and process any errors. */
        for (cpuTypeIdx = 0U; cpuTypeIdx < (uint32_t) ADI_ADRV904X_CPU_TYPE_MAX_RADIO; ++cpuTypeIdx)
        {
            /* Send command and receive response */
            recoveryAction = adrv904x_CpuCmdSend(device,
                                                 (adi_adrv904x_CpuType_e)cpuTypeIdx,
                                                 ADRV904X_LINK_ID_0,
                                                 ADRV904X_CPU_CMD_ID_SET_GPIO,
                                                 (void*)&setGpioCmd,
                                                 sizeof(setGpioCmd),
                                                 (void*)&setGpioResp,
                                                 sizeof(setGpioResp),
                                                 &cmdStatus);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_ADRV904X_CPU_CMD_RESP_CHECK_RETURN(setGpioResp.cmdStatus, cmdStatus, cpuErrorCode, recoveryAction);
            }
        }
    }

    /* Set if valid GPIO */
    if (gpioSelect != ADI_ADRV904X_GPIO_INVALID)
    {
        if (isInput == ADI_TRUE)
        {
            signal = inputSignals[gpioSelect];
        }
        else
        {
            signal = outputSignals[gpioSelect];
        }

        /* Set the signal to the GPIO */
        recoveryAction = adrv904x_GpioSignalSet(device, gpioSelect, signal, ADI_ADRV904X_CHOFF);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while routing CPU signal to GPIO.");
            return recoveryAction;
        }

        /* Send GPIO set command to CPU to enable the pin */
        /* Fill out set gpio cmd params with user-provided params */
        setGpioCmd.signal = (uint8_t)cpuGpioSignal;
        setGpioCmd.pin = (uint8_t)gpioSelect;
        setGpioCmd.polarity = (uint8_t)ADRV904X_CPU_CMD_GPIO_PIN_POLARITY_NORMAL;
        setGpioCmd.enable = (uint8_t)ADRV904X_CPU_CMD_GPIO_PIN_ENABLE;

        /* For each CPU, send the SET_GPIO command, wait for a response, and process any errors. */
        for (cpuTypeIdx = 0U; cpuTypeIdx < (uint32_t) ADI_ADRV904X_CPU_TYPE_MAX_RADIO; ++cpuTypeIdx)
        {
            /* Send command and receive response */
            recoveryAction = adrv904x_CpuCmdSend(device,
                                                 (adi_adrv904x_CpuType_e)cpuTypeIdx,
                                                 ADRV904X_LINK_ID_0,
                                                 ADRV904X_CPU_CMD_ID_SET_GPIO,
                                                 (void*)&setGpioCmd,
                                                 sizeof(setGpioCmd),
                                                 (void*)&setGpioResp,
                                                 sizeof(setGpioResp),
                                                 &cmdStatus);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_ADRV904X_CPU_CMD_RESP_CHECK_RETURN(setGpioResp.cmdStatus, cmdStatus, cpuErrorCode, recoveryAction);
            }
        }
    }

    recoveryAction  = ADI_ADRV904X_ERR_ACT_NONE;
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CpuGpioGet(adi_adrv904x_Device_t* const device,
                                                     adi_adrv904x_GpioPinSel_e * const gpioSelect,
                                                     const adrv904x_CpuCmd_GpioSignal_e cpuGpioSignal)
{
    adi_adrv904x_ErrAction_e  recoveryAction  = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_CpuCmd_GetGpio_t getGpioCmd;
    adrv904x_CpuCmd_GetGpioResp_t getGpioResp;
    adrv904x_CpuErrorCode_e cpuErrorCode = ADRV904X_CPU_SYSTEM_SIMULATED_ERROR;
    adrv904x_CpuCmdStatus_e cmdStatus = ADRV904X_CPU_CMD_STATUS_GENERIC;

    /* Prepare the command payload */
    getGpioCmd.signal = (uint8_t)cpuGpioSignal;

    /* Send command and receive response */
    recoveryAction = adrv904x_CpuCmdSend(device,
                                         ADI_ADRV904X_CPU_TYPE_0,
                                         ADRV904X_LINK_ID_0,
                                         ADRV904X_CPU_CMD_ID_GET_GPIO,
                                         (void*)&getGpioCmd,
                                         sizeof(getGpioCmd),
                                         (void*)&getGpioResp,
                                         sizeof(getGpioResp),
                                         &cmdStatus);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_CPU_CMD_RESP_CHECK_RETURN(getGpioResp.cmdStatus, cmdStatus, cpuErrorCode, recoveryAction);
    }

    *gpioSelect = (adi_adrv904x_GpioPinSel_e)getGpioResp.pin;

    return recoveryAction;
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_EccEnableGet(adi_adrv904x_Device_t* const device,
                                                           uint8_t* const  eccEnable)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_CpuCmdStatus_e cmdStatus = ADRV904X_CPU_CMD_STATUS_GENERIC;
    adrv904x_CpuErrorCode_e cmdErrorCode = ADRV904X_CPU_NO_ERROR;

    adi_adrv904x_CpuGetEccScrubEnableCmdResp_t eccEnableGetRsp;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(eccEnable);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_LIBRARY_MEMSET(&eccEnableGetRsp, 0, sizeof(adi_adrv904x_CpuGetEccScrubEnableCmdResp_t));

    /* Send command and receive response */
    recoveryAction = adrv904x_CpuCmdSend(device,
                                         ADI_ADRV904X_CPU_TYPE_0,
                                         ADRV904X_LINK_ID_0,
                                         ADRV904X_CPU_CMD_ID_GET_ENABLE_ECC_SCRUB,
                                         0U,
                                         0U,
                                         (void*)&eccEnableGetRsp,
                                         sizeof(eccEnableGetRsp),
                                         &cmdStatus);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        /* If the command failed for a command-specific reason, extract the command status code and log the error. */
        cmdErrorCode = (adrv904x_CpuErrorCode_e)ADRV904X_CTOHL(eccEnableGetRsp.status);
        ADI_ERROR_REPORT(&device->common,
                         ADI_ADRV904X_ERRSRC_API,
                         ADI_COMMON_ERRCODE_API,
                         recoveryAction,
                         cmdErrorCode,
                         "ECC Enable Get command failed, CPU0");

        ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO((adrv904x_CpuErrorCode_e)(eccEnableGetRsp.status), cmdStatus, cmdErrorCode, recoveryAction, cleanup);
    }
    else
    {
        *eccEnable = eccEnableGetRsp.eccScrubEnable;

        /* reset for the next get  */
        ADI_LIBRARY_MEMSET(&eccEnableGetRsp, 0, sizeof(adi_adrv904x_CpuGetEccScrubEnableCmdResp_t));
        recoveryAction = adrv904x_CpuCmdSend(device,
                                             ADI_ADRV904X_CPU_TYPE_1,
                                             ADRV904X_LINK_ID_0,
                                             ADRV904X_CPU_CMD_ID_GET_ENABLE_ECC_SCRUB,
                                             0U,
                                             0U,
                                             (void*)&eccEnableGetRsp,
                                             sizeof(eccEnableGetRsp),
                                             &cmdStatus);

        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            /* If the command failed for a command-specific reason, extract the command status code and log the error. */
            cmdErrorCode = (adrv904x_CpuErrorCode_e)ADRV904X_CTOHL(eccEnableGetRsp.status);
            ADI_ERROR_REPORT(&device->common,
                             ADI_ADRV904X_ERRSRC_API,
                             ADI_COMMON_ERRCODE_API,
                             recoveryAction,
                             cmdErrorCode,
                             "ECC Enable Get command failed, CPU1");
            ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO((adrv904x_CpuErrorCode_e)(eccEnableGetRsp.status), cmdStatus, cmdErrorCode, recoveryAction, cleanup);
        }
        else
        {
                if (eccEnableGetRsp.eccScrubEnable > 0)
                {
                    *eccEnable |= 1u;
                }
        }
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_EccEnableSet(adi_adrv904x_Device_t* const device,
                                                           uint8_t const  eccEnable)
{

    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_CpuCmdStatus_e cmdStatus = ADRV904X_CPU_CMD_STATUS_GENERIC;
    adrv904x_CpuErrorCode_e cmdErrorCode = ADRV904X_CPU_NO_ERROR;
    adi_adrv904x_CpuSetEccScrubEnableCmd_t eccEnableSet;
    adi_adrv904x_CpuSetEccScrubEnableCmdResp_t eccEnableSetRsp;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_LIBRARY_MEMSET(&eccEnableSetRsp, 0, sizeof(adi_adrv904x_CpuSetEccScrubEnableCmdResp_t));

    eccEnableSet.eccScrubEnable = (uint8_t)(eccEnable);

    /* Send command and receive response */
    recoveryAction = adrv904x_CpuCmdSend(device,
                                         ADI_ADRV904X_CPU_TYPE_0,
                                         ADRV904X_LINK_ID_0,
                                         ADRV904X_CPU_CMD_ID_SET_ENABLE_ECC_SCRUB,
                                         (void*)&eccEnableSet,
                                         sizeof(eccEnableSet),
                                         (void*)&eccEnableSetRsp,
                                         sizeof(eccEnableSetRsp),
                                         &cmdStatus);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        /* If the command failed for a command-specific reason, extract the command status code and log the error. */
        cmdErrorCode = (adrv904x_CpuErrorCode_e)ADRV904X_CTOHL(eccEnableSetRsp.status);
        ADI_ERROR_REPORT(&device->common,
                         ADI_ADRV904X_ERRSRC_API,
                         ADI_COMMON_ERRCODE_API,
                         recoveryAction,
                         cmdErrorCode,
                         "ECC Enable Set command failed, CPU0");
        ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO((adrv904x_CpuErrorCode_e)(eccEnableSetRsp.status), cmdStatus, cmdErrorCode, recoveryAction, cleanup);

    }
    else
    {
        /* reset for the next set  */
        ADI_LIBRARY_MEMSET(&eccEnableSetRsp, 0, sizeof(adi_adrv904x_CpuSetEccScrubEnableCmdResp_t));
        recoveryAction = adrv904x_CpuCmdSend(device,
                                             ADI_ADRV904X_CPU_TYPE_1,
                                             ADRV904X_LINK_ID_0,
                                             ADRV904X_CPU_CMD_ID_SET_ENABLE_ECC_SCRUB,
                                             (void*)&eccEnableSet,
                                             sizeof(eccEnableSet),
                                             (void*)&eccEnableSetRsp,
                                             sizeof(eccEnableSetRsp),
                                             &cmdStatus);

        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            /* If the command failed for a command-specific reason, extract the command status code and log the error. */
            cmdErrorCode = (adrv904x_CpuErrorCode_e)ADRV904X_CTOHL(eccEnableSetRsp.status);
            ADI_ERROR_REPORT(&device->common,
                             ADI_ADRV904X_ERRSRC_API,
                             ADI_COMMON_ERRCODE_API,
                             recoveryAction,
                             cmdErrorCode,
                             "ECC Enable Set command failed, CPU1");
            ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO((adrv904x_CpuErrorCode_e)(eccEnableSetRsp.status), cmdStatus, cmdErrorCode, recoveryAction, cleanup);
        }
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API void adrv904x_CpuErrorDebugCheck(adi_adrv904x_Device_t* const device)
{
    adi_adrv904x_ErrAction_e    recoveryAction  = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t                    cpuException    = 0U;
    uint32_t                    errAddress      = 0U;
    adrv904x_CpuErrors_t        cpu0Status      = { ADRV904X_CPU_NO_ERROR, ADRV904X_CPU_NO_ERROR, ADRV904X_CPU_NO_ERROR };
    adrv904x_CpuErrors_t        cpu1Status      = { ADRV904X_CPU_NO_ERROR, ADRV904X_CPU_NO_ERROR, ADRV904X_CPU_NO_ERROR };

    if (device == NULL)
    {
        return;
    }

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    if ((device->devStateInfo.devState & ADI_ADRV904X_STATE_ALLCPUSLOADED) == ADI_ADRV904X_STATE_ALLCPUSLOADED)
    {
        recoveryAction = adi_adrv904x_CpuCheckException(device, &cpuException);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Radio CPU Exception Check Issue");
            /* Continue Debug */
        }

        if (cpuException != 0U)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_RESET_DEVICE;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuException, "Radio CPU Exception Detected");
            /* Continue Debug */
        }
    }

    /* Get CPU 0 Runtime Status */
    if (ADI_ADRV904X_ERR_ACT_NONE == adi_adrv904x_Register32Read(   device,
                                                                    NULL,
                                                                    ADRV904X_CPU_0_PM_ERROR_STATUS,
                                                                    &errAddress,
                                                                    0xFFFFFFFFU))
    {
        (void) adi_adrv904x_Registers32Read(device,
                                            NULL,
                                            errAddress,
                                            (uint32_t*) &cpu0Status,
                                            NULL,
                                            (uint32_t) (sizeof(adrv904x_CpuErrors_t) / (sizeof(uint32_t))));
    }

    /* Get CPU 1 Runtime Status */
    if (ADI_ADRV904X_ERR_ACT_NONE == adi_adrv904x_Register32Read(   device,
                                                                    NULL,
                                                                    ADRV904X_CPU_1_PM_ERROR_STATUS,
                                                                    &errAddress,
                                                                    0xFFFFFFFFU))
    {
        (void) adi_adrv904x_Registers32Read(device,
                                            NULL,
                                            errAddress,
                                            (uint32_t*) &cpu1Status,
                                            NULL,
                                            (uint32_t) (sizeof(adrv904x_CpuErrors_t) / (sizeof(uint32_t))));
    }

    if (cpu0Status.system != ADRV904X_CPU_NO_ERROR)
    {
        ADI_ADRV904X_ERROR_INFO_GET_REPORT( ADI_ADRV904X_ERRSRC_CPU_RUNTIME,
                                            cpu0Status.system,
                                            cpu0Status.system,
                                            recoveryAction);
    }


    if (cpu1Status.system != ADRV904X_CPU_NO_ERROR)
    {
        ADI_ADRV904X_ERROR_INFO_GET_REPORT( ADI_ADRV904X_ERRSRC_CPU_RUNTIME,
                                            cpu1Status.system,
                                            cpu1Status.system,
                                            recoveryAction);
    }

    (void) recoveryAction;
}
