/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

/**
 * \file adi_adrv904x_dfe_svc_err_object_ids_t.h
 *
 * \brief   Contains DFE Object ID definitions
 *
 * \details Contains DFE Object ID definitions
 *
 * ADRV904X API Version: 2.10.0.4
 */

#ifndef __ADI_ADRV904X_DFE_SVC_ERR_OBJID_T_H__
#define __ADI_ADRV904X_DFE_SVC_ERR_OBJID_T_H__

#include <stdint.h>

/**
 * \brief DFE firmware object IDs
 */
typedef enum
{
    /* Common Object */
    ADI_ADRV904X_DFE_SVC_ERR_OBJID_COMMON              = 0x00u,           /*!< Start of Common OBJID */

    /* Service Layer */
    ADI_ADRV904X_DFE_SVC_ERR_OBJID_SVC_CMD             = 0x01u,           /*!< CMD handler service          */
    ADI_ADRV904X_DFE_SVC_ERR_OBJID_SVC_OSAL            = 0x02u,           /*!< OS Abstraction Layer Services */
    ADI_ADRV904X_DFE_SVC_ERR_OBJID_SVC_LOG             = 0x03u,           /*!< Log Service                  */
    ADI_ADRV904X_DFE_SVC_ERR_OBJID_SVC_DFE             = 0x04u,           /*!< DFE Service                  */
    ADI_ADRV904X_DFE_SVC_ERR_OBJID_SVC_TIMER           = 0x05u,           /*!< Timer Service                */
    ADI_ADRV904X_DFE_SVC_ERR_OBJID_SVC_RADIO_CTRL      = 0x06u,           /*!< Radio Ctrl Service           */
    ADI_ADRV904X_DFE_SVC_ERR_OBJID_SVC_EVENT           = 0x07u,           /*!< Event Service                */
    ADI_ADRV904X_DFE_SVC_ERR_OBJID_SVC_CLI             = 0x08u,           /*!< CLI Service                  */
    ADI_ADRV904X_DFE_SVC_ERR_OBJID_SVC_MALLOC          = 0x09u,           /*!< Malloc Service               */
    ADI_ADRV904X_DFE_SVC_ERR_OBJID_SVC_BBIC_BRIDGE     = 0x0Au,           /*!< BBIC Bridge service          */
    ADI_ADRV904X_DFE_SVC_ERR_OBJID_SVC_BUF_MGMT        = 0x0Bu,           /*!< Buffer Mgmt Service          */
    ADI_ADRV904X_DFE_SVC_ERR_OBJID_SVC_CFG             = 0x0Cu,           /*!< Configuration Framework Service */
    ADI_ADRV904X_DFE_SVC_ERR_OBJID_SVC_ERRORLOG        = 0x0Du,           /*!< Error Logging Service */
    ADI_ADRV904X_DFE_SVC_ERR_OBJID_SVC_INIT            = 0x0Eu,           /*!< App Init Service */
    ADI_ADRV904X_DFE_SVC_ERR_OBJID_SVC_ECC             = 0x0Fu,           /*!< ECC Service */
    ADI_ADRV904X_DFE_SVC_ERR_OBJID_SVC_WATCHDOG        = 0x10u,           /*!< Watchdog Timer Service */
    ADI_ADRV904X_DFE_SVC_ERR_OBJID_SVC_RADIO_CMD       = 0x11u,           /*!< Radio Command Service */
    ADI_ADRV904X_DFE_SVC_ERR_OBJID_SVC_EXCEPTION       = 0x12u,           /*!< Exception handler */

    /* Debug mode object */
    ADI_ADRV904X_DFE_SVC_ERR_OBJID_SVC_DBG             = 0x20u,          /*!< Debug Mode */

    /* Driver objects*/
    ADI_ADRV904X_DFE_SVC_ERR_OBJID_DRV_GPIO            = 0x30u,          /*!< GPIO */
    ADI_ADRV904X_DFE_SVC_ERR_OBJID_DRV_GPT             = 0x31u,          /*!< GPT  */
    ADI_ADRV904X_DFE_SVC_ERR_OBJID_DRV_INTERRUPT       = 0x32u,          /*!< GPT  */
    ADI_ADRV904X_DFE_SVC_ERR_OBJID_DRV_UART            = 0x33u,          /*!< UART  */
    ADI_ADRV904X_DFE_SVC_ERR_OBJID_DRV_MB              = 0x34u,          /*!< MailBox  */
    ADI_ADRV904X_DFE_SVC_ERR_OBJID_DRV_PINTSW          = 0x35u,          /*!< PINTSW Interrupts*/
    ADI_ADRV904X_DFE_SVC_ERR_OBJID_DRV_SYSCLK          = 0x36u,          /*!< SYSCLK  */
    ADI_ADRV904X_DFE_SVC_ERR_OBJID_DRV_STREAM          = 0x37u,          /*!< STREAM  */
    ADI_ADRV904X_DFE_SVC_ERR_OBJID_DRV_PWR_MTR         = 0x38u,          /*!< Power Meter */
    ADI_ADRV904X_DFE_SVC_ERR_OBJID_DRV_DMA             = 0x39u,          /*!< DMA  */
    ADI_ADRV904X_DFE_SVC_ERR_OBJID_DRV_CAPTURE         = 0x3Au,          /*!< Capture driver  */
    ADI_ADRV904X_DFE_SVC_ERR_OBJID_DRV_HW_IF           = 0x3Bu,          /*!< Hw If */
    ADI_ADRV904X_DFE_SVC_ERR_OBJID_DRV_DFE             = 0x3cu,          /*!< DFE */

    /* Configuration objects */
    ADI_ADRV904X_DFE_SVC_ERR_OBJID_CFG_DEVICE_PROFILE  = 0x80u,          /*!< Configuration profile       */
    ADI_ADRV904X_DFE_SVC_ERR_OBJID_CFG_RADIO_SEQUENCER = 0x81u,          /*!< Radio sequencer module         */

    /* Calibration objects ( 0xB0 - 0xBF ) */
    ADI_ADRV904X_DFE_SVC_ERR_OBJID_CAL_START           = 0xB0u,          /*!< Cal start index */

    ADI_ADRV904X_DFE_SVC_ERR_OBJID_CAL_END             = 0xBFu,          /*!< Cal end index */
    /* SDK Application objects, always keep in end */
    ADI_ADRV904X_DFE_SVC_ERR_OBJID_APP_START           = 0xE0u,          /*!< Start of APP OBJID */
    ADI_ADRV904X_DFE_SVC_ERR_OBJID_APP_ERROR           = 0xE0u,          /*!< APP error */
} adi_adrv904x_DfeSvcErrObjId_e;

typedef uint32_t adi_adrv904x_DfeSvcErrObjId_t;

/* The mask of object Id to get object types */
#define ADI_ADRV904X_DFE_SVC_ERR_OBJID_BASE_MASK      0xF0u

/* The first service layer error object ID */
#define ADI_ADRV904X_DFE_SVC_ERR_OBJID_SVC_BASE       0x00u

/* Needed to identify more than 16 service layer objects */
#define ADI_ADRV904X_DFE_SVC_ERR_OBJID_SVC_BASE1      0x10u
#define ADI_ADRV904X_DFE_SVC_ERR_OBJID_SVC_BASE2      0x20u

/* The first driver error object ID */
#define ADI_ADRV904X_DFE_SVC_ERR_OBJID_DRV_BASE       0x30u

/* The first configuration error object ID */
#define ADI_ADRV904X_DFE_SVC_ERR_OBJID_CONFIG_BASE    0x80u

/* The first calibration error object ID */
#define ADI_ADRV904X_DFE_SVC_ERR_OBJID_CAL_BASE       0xB0u

/* The first SDK Application error object ID */
#define ADI_ADRV904X_DFE_SVC_ERR_OBJID_SDKAPP_BASE    0xE0u

#endif /* __ADI_ADRV904X_DFE_SVC_ERR_OBJID_T_H__ */


