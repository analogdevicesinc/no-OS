/**
 * Copyright 2020 - 2021 Analog Devices Inc.
 * Released under the ADRV904X Software license, for more information
 * see the "LICENSE.pdf" file in this zip file.
 */

/**
 * \file adrv904x_dfe_svc_err_dfe_boot_status_t.h
 *
 * \brief   Contains DFE CPU boot status error code definitions
 *
 * \details Contains DFE CPU boot status error code definitions
 *
 * DFE SDK API Version: 2.9.0.4
 */

#ifndef __ADRV904X_DFE_SVC_ERR_DFE_BOOT_STATUS_T_H__
#define __ADRV904X_DFE_SVC_ERR_DFE_BOOT_STATUS_T_H__

#include <stdint.h>
#include "adi_adrv904x_platform_pack.h"
#include "adi_adrv904x_dfe_svc_err_codes_t.h"

/* Type and enumeration for DFE boot status and error codes */
typedef uint8_t adrv904x_DfeSvcErrDfeBootStatus_t;
typedef enum adrv904x_DfeSvcErrDfeBootStatus
{
    ADRV904X_DFE_SVC_ERR_DFE_BOOT_STATUS_POWER_UP           = 0u,    /*!< @errcode: 0x0000
                                                              * @desc: Used to put API in wait for CPU state.
                                                              * @maincause: CPU powered up.
                                                              * @mainrecovenum: ADI_ADRV904X_ERR_ACT_NONE
                                                              * @mainrecovtext: No action required.
                                                              */

    ADRV904X_DFE_SVC_ERR_DFE_BOOT_STATUS_READY              = 1u,    /*!< @errcode: 0x0001
                                                              * @desc: CPU booted with no failure.
                                                              * @maincause: CPU booted with no failure.
                                                              * @mainrecovenum: ADI_ADRV904X_ERR_ACT_NONE
                                                              * @mainrecovtext: No action required.
                                                              */

    ADRV904X_DFE_SVC_ERR_DFE_BOOT_STATUS_WAIT_FOR_CPUS      = 2u,    /*!< @errcode: 0x0002
                                                              * @desc: Primary CPU waiting for secondary CPU(s) to boot.
                                                              * @maincause:  Secondary CPU failed to complete. See secondary CPU boot status for more details.
                                                              * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                              * @mainrecovtext: Reset device and try again. Contact ADI if the problem persists.
                                                              */

    ADRV904X_DFE_SVC_ERR_DFE_BOOT_STATUS_BOOT_PREINIT       = 4u,    /*!< @errcode: 0x004
                                                              * @desc: DFE CPU boot has entered pre-init stage
                                                              * @maincause: DFE CPU boot pre-init stage has failed preventing boot from progressing
                                                              * @mainrecovenum:  ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                              * @mainrecovtext: Reset device and try again. Contact ADI if the problem persists.
                                                              */

    ADRV904X_DFE_SVC_ERR_DFE_BOOT_STATUS_PROFILE_INIT       = 5u,    /*!< @errcode: 0x005
                                                              * @desc: DFE CPU boot has entered initialization of device profile stage
                                                              * @maincause: Device profile stage has failed preventing boot from progressing
                                                              * @mainrecovenum:  ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                              * @mainrecovtext: Reset device and try again. Contact ADI if the problem persists.
                                                              */

    ADRV904X_DFE_SVC_ERR_DFE_BOOT_STATUS_OSAL_INIT          = 6u,    /*!< @errcode: 0x006
                                                              * @desc: DFE CPU boot has entered OSAL initialization  stage
                                                              * @maincause: OSAL initialization stage has failed preventing boot from progressing
                                                              * @mainrecovenum:  ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                              * @mainrecovtext: Reset device and try again. Contact ADI if the problem persists.
                                                              */

    ADRV904X_DFE_SVC_ERR_DFE_BOOT_STATUS_MEM_SVC_PREINIT    = 7u,    /*!< @errcode: 0x007
                                                              * @desc: DFE CPU boot has entered memory service pre-initialization stage
                                                              * @maincause: Memory service pre-initialization stage has failed preventing boot from progressing
                                                              * @mainrecovenum:  ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                              * @mainrecovtext: Reset device and try again. Contact ADI if the problem persists.
                                                              */

    ADRV904X_DFE_SVC_ERR_DFE_BOOT_STATUS_THREAD_START       = 8u,    /*!< @errcode: 0x008
                                                              * @desc: DFE CPU boot is attempting to start initial thread
                                                              * @maincause: Starting of initial thread has failed preventing boot from progressing
                                                              * @mainrecovenum:  ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                              * @mainrecovtext: Reset device and try again. Contact ADI if the problem persists.
                                                              */

    ADRV904X_DFE_SVC_ERR_DFE_BOOT_STATUS_APP_CONFIG         = 9u,    /*!< @errcode: 0x009
                                                              * @desc: DFE CPU boot has entered DFE Application application_config() function
                                                              * @maincause: application_config() has failed preventing boot from progressing
                                                              * @mainrecovenum:  ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                              * @mainrecovtext: Reset device and try again. Contact ADI if the problem persists if using ADI-provided DFE image. Otherwise, debug customer application_config().
                                                              */

    ADRV904X_DFE_SVC_ERR_DFE_BOOT_STATUS_APP_INIT           = 10u,   /*!< @errcode: 0x00A
                                                              * @desc: DFE CPU boot has entered DFE Application application_init() function
                                                              * @maincause: application_init() has failed preventing boot from progressing
                                                              * @mainrecovenum:  ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                              * @mainrecovtext: Reset device and try again. Contact ADI if the problem persists if using ADI-provided DFE image. Otherwise, debug customer application_init().
                                                              */

    ADRV904X_DFE_SVC_ERR_DFE_BOOT_STATUS_ERROR_INIT         = 11u,   /*!< @errcode: 0x00B
                                                              * @desc: DFE CPU boot has entered initialization of the error handling service
                                                              * @maincause: Initialization of error handling service has failed preventing boot from progressing
                                                              * @mainrecovenum:  ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                              * @mainrecovtext: Reset device and try again. Contact ADI if the problem persists.
                                                              */

    ADRV904X_DFE_SVC_ERR_DFE_BOOT_STATUS_BUFFER_MGMT_INIT   = 12u,   /*!< @errcode: 0x00C
                                                              * @desc: DFE CPU boot has entered adaptation allocation stage
                                                              * @maincause: Adaptation allocation stage has failed preventing boot from progressing
                                                              * @mainrecovenum:  ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                              * @mainrecovtext: Reset device and try again. Contact ADI if the problem persists.
                                                              */

    ADRV904X_DFE_SVC_ERR_DFE_BOOT_STATUS_MEM_SVC_INIT       = 14u,   /*!< @errcode: 0x00E
                                                              * @desc: DFE CPU boot has entered initialization of the memory allocation service
                                                              * @maincause: Memory allocation service has failed preventing boot from progressing
                                                              * @mainrecovenum:  ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                              * @mainrecovtext: Reset device and try again. Contact ADI if the problem persists.
                                                              */

    ADRV904X_DFE_SVC_ERR_DFE_BOOT_STATUS_LOG_TRACE_INIT     = 15u,   /*!< @errcode: 0x00F
                                                              * @desc: DFE CPU boot has entered initialization of the log and tracing service
                                                              * @maincause: Log and trace initialization has failed preventing boot from progressing
                                                              * @mainrecovenum:  ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                              * @mainrecovtext: Reset device and try again. Contact ADI if the problem persists.
                                                              */

    ADRV904X_DFE_SVC_ERR_DFE_BOOT_STATUS_EVENT_SVC_INIT     = 16u,   /*!< @errcode: 0x010
                                                              * @desc: DFE CPU boot has entered event service initialization
                                                              * @maincause: Event service initialization has failed preventing boot from progressing
                                                              * @mainrecovenum:  ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                              * @mainrecovtext: Reset device and try again. Contact ADI if the problem persists.
                                                              */

    ADRV904X_DFE_SVC_ERR_DFE_BOOT_STATUS_RADIO_CTRL_INIT    = 17u,   /*!< @errcode: 0x011
                                                              * @desc: DFE CPU boot has entered radio control service initialization
                                                              * @maincause: Radio control service initialization has failed preventing boot from progressing
                                                              * @mainrecovenum:  ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                              * @mainrecovtext: Reset device and try again. Contact ADI if the problem persists.
                                                              */


    ADRV904X_DFE_SVC_ERR_DFE_BOOT_STATUS_BRIDGE_INIT        = 18u,   /*!< @errcode: 0x012
                                                              * @desc: DFE CPU has entered BBIC Bridge service initialization
                                                              * @maincause: BBIC Bridge service initialization has failed preventing boot from progressing
                                                              * @mainrecovenum:  ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                              * @mainrecovtext: Reset device and try again. Contact ADI if the problem persists.
                                                              */

    ADRV904X_DFE_SVC_ERR_DFE_BOOT_STATUS_CMD_SVC_INIT       = 19u,   /*!< @errcode: 0x013
                                                              * @desc: DFE CPU has entered command service initialization
                                                              * @maincause: Command service initialization has failed preventing boot from progressing
                                                              * @mainrecovenum:  ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                              * @mainrecovtext: Reset device and try again. Contact ADI if the problem persists.
                                                              */

    ADRV904X_DFE_SVC_ERR_DFE_BOOT_STATUS_CAPTURE_INIT       = 20u,   /*!< @errcode: 0x014
                                                              * @desc: DFE CPU has entered DFE capture service initialization
                                                              * @maincause: DFE capture service initialization has failed preventing boot from progressing
                                                              * @mainrecovenum:  ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                              * @mainrecovtext: Reset device and try again. Contact ADI if the problem persists.
                                                              */

    ADRV904X_DFE_SVC_ERR_DFE_BOOT_STATUS_ADAPTATION_INIT    = 21u,   /*!< @errcode: 0x015
                                                              * @desc: DFE CPU has entered DFE adaptation service initialization
                                                              * @maincause: DFE adaptation service initialization has failed preventing boot from progressing
                                                              * @mainrecovenum:  ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                              * @mainrecovtext: Reset device and try again. Contact ADI if the problem persists.
                                                              */

    ADRV904X_DFE_SVC_ERR_DFE_BOOT_STATUS_EXCEPTION_INIT     = 22u,   /*!< @errcode: 0x016
                                                              * @desc: DFE CPU has entered exception handler initialization
                                                              * @maincause: Exception handler initialization has failed, likely due to unhandled exception prior to initialization.
                                                              * @mainrecovenum:  ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                              * @mainrecovtext: Reset device and try again. Contact ADI if the problem persists.
                                                              */

    ADRV904X_DFE_SVC_ERR_DFE_BOOT_STATUS_FW_CRC_ERR         = 23u,   /*!< @errcode: 0x017
                                                              * @desc: CPU firmware image failed CRC verification.
                                                              * @maincause: Programmed image is corrupted. Original image may be invalid or may have been corrupted during SPI transfer.
                                                              * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                              * @mainrecovtext: Reset device and try again. Contact ADI if the problem persists.
                                                              */

    ADRV904X_DFE_SVC_ERR_DFE_BOOT_STATUS_UNEXPECTED_ERR     = 24u,   /*!< @errcode: 0x018
                                                              * @desc: Unexpected exception/error.
                                                              * @maincause: An unhandled fatal error was encountered.
                                                              * @mainrecovenum:  ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                              * @mainrecovtext: Reset device and try again. Contact ADI if the problem persists.
                                                              */
    ADRV904X_DFE_SVC_ERR_DFE_BOOT_STATUS_INIT_THREAD        = 26u, /*!< @errcode: 0x01A
                                                            * @desc: DFE CPU initial thread creation
                                                            * @maincause: Initial thread creation has failed preventing boot from progressing
                                                            * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                            * @mainrecovtext: Reset device and try again. Contact ADI if the problem persists.
                                                            */

    ADRV904X_DFE_SVC_ERR_DFE_BOOT_STATUS_SYSCLK_INIT        = 27u, /*!< @errcode: 0x01B
                                                            * @desc: DFE CPU system clock initialization
                                                            * @maincause: System clock initialization has failed preventing boot from progressing
                                                            * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                            * @mainrecovtext: Reset device and try again. Contact ADI if the problem persists.
                                                            */

    ADRV904X_DFE_SVC_ERR_DFE_BOOT_STATUS_INTERRUPT_INIT     = 28u, /*!< @errcode: 0x01C
                                                            * @desc: DFE CPU interrupt initialization
                                                            * @maincause: Interrupt initialization has failed preventing boot from progressing
                                                            * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                            * @mainrecovtext: Reset device and try again. Contact ADI if the problem persists.
                                                            */

    ADRV904X_DFE_SVC_ERR_DFE_BOOT_STATUS_HALT_INIT          = 29u, /*!< @errcode: 0x01D
                                                            * @desc: DFE CPU exception halt initialization
                                                            * @maincause: Exception halt initialization has failed preventing boot from progressing
                                                            * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                            * @mainrecovtext: Reset device and try again. Contact ADI if the problem persists.
                                                            */

    ADRV904X_DFE_SVC_ERR_DFE_BOOT_STATUS_OS_TICK_INIT       = 30u, /*!< @errcode: 0x01E
                                                            * @desc: DFE CPU OS tick initialization
                                                            * @maincause: OS tick initialization has failed preventing boot from progressing
                                                            * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                            * @mainrecovtext: Reset device and try again. Contact ADI if the problem persists.
                                                            */

    ADRV904X_DFE_SVC_ERR_DFE_BOOT_STATUS_OS_LAUNCH          = 31u, /*!< @errcode: 0x01F
                                                            * @desc: DFE CPU OS launch
                                                            * @maincause: OS launch has failed preventing boot from progressing
                                                            * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                            * @mainrecovtext: Reset device and try again. Contact ADI if the problem persists.
                                                            */

    ADRV904X_DFE_SVC_ERR_DFE_BOOT_STATUS_OS_STACK_CHK_INIT  = 32u, /*!< @errcode: 0x020
                                                            * @desc: DFE CPU OS stack error checking initialization
                                                            * @maincause: Stack error checking initialization has failed preventing boot from progressing
                                                            * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                            * @mainrecovtext: Reset device and try again. Contact ADI if the problem persists.
                                                            */

    ADRV904X_DFE_SVC_ERR_DFE_BOOT_STATUS_EXCEPTION_PREINIT  = 33u, /*!< @errcode: 0x021
                                                            * @desc: DFE CPU has entered early exception handler initialization
                                                            * @maincause: Early exception handler initialization has failed, likely due to unhandled exception prior to initialization.
                                                            * @mainrecovenum:  ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                            * @mainrecovtext: Reset device and try again. Contact ADI if the problem persists.
                                                            */

    ADRV904X_DFE_SVC_ERR_DFE_BOOT_STATUS_READY_FOR_RELOAD   = 34u, /*!< @errcode: 0x022
                                                            * @desc: CPU halted and prepared for a new image to be loaded by BBIC
                                                            * @maincause: N/A
                                                            * @mainrecovenum:  ADI_ADRV904X_ERR_ACT_NONE
                                                            * @mainrecovtext: No action required
                                                            */

    ADRV904X_DFE_SVC_ERR_DFE_BOOT_STATUS_ECC_INIT           = 35u, /*!< @errcode: 0x023
                                                            * @desc: DFE CPU has failed to initialize ECC
                                                            * @maincause: An ECC error has occurred in memory.
                                                            * @mainrecovenum:  ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                            * @mainrecovtext: Reset device and try again. Contact ADI if the problem persists.
                                                            */
    ADRV904X_DFE_SVC_ERR_DFE_BOOT_STATUS_EXCEPTION_DONE     = 36u, /*!< @errcode: 0x024
                                                            * @desc: DFE CPU has finished with L4 memory flush
                                                            * @maincause: Force exception
                                                            * @mainrecovenum:  ADI_ADRV904X_ERR_ACT_NONE
                                                            * @mainrecovtext: No action required
                                                            */

    ADRV904X_DFE_SVC_ERR_DFE_BOOT_STATUS_WATCHDOG_INIT      = 37u, /*!< @errcode: 0x025
                                                            * @desc: DFE CPU has failed to initialize watchdog service
                                                            * @maincause: Watchdog service encountered an error while initializing
                                                            * @mainrecovenum:  ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                            * @mainrecovtext: Reset device and try again. Contact ADI if the problem persists.
                                                            */

    ADRV904X_DFE_SVC_ERR_DFE_BOOT_STATUS_RADIO_CMD_SVC_INIT = 38u, /*!< @errcode: 0x026
                                                            * @desc: DFE CPU has failed to initialize radio command service
                                                            * @maincause: radio command service encountered an error while initializing
                                                            * @mainrecovenum:  ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                            * @mainrecovtext: Reset device and try again. Contact ADI if the problem persists.
                                                            */
} adrv904x_DfeSvcErrDfeBootStatus_e;

/* Type used to store DFE error status information */
ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeSvcErrDfeErrors
{
    adi_adrv904x_DfeSvcErrCode_e lastCmd;
    adi_adrv904x_DfeSvcErrCode_e system;
} adrv904x_DfeSvcErrDfeErrors_t;
ADI_ADRV904X_PACK_FINISH


#endif /* __ADRV904X_DFE_SVC_ERR_DFE_BOOT_STATUS_T_H__ */


