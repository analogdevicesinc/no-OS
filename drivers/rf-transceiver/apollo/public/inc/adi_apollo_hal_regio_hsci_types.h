/*!
 * \brief     Contains hsci hal register io types
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_HAL
 * @{
 */

#ifndef __ADI_APOLLO_HAL_REGIO_HSCI_TYPES_H__
#define __ADI_APOLLO_HAL_REGIO_HSCI_TYPES_H__

#include "adi_cms_api_common.h"
#include "adi_apollo_hal_regio.h"

#define ADI_APOLLO_HAL_REGIO_HSCI_NO_MASK 0

/* Default payload buffer size in case no optional buffer is provided for stream_read and stream_write and multiple of 4 */
#define ADI_APOLLO_HAL_REGIO_HSCI_STREAM_DEFAULT_PAYLOAD_SIZE   (32760U)
/* 4 Bytes header overhead to stream bytes*/
#define ADI_APOLLO_HAL_REGIO_HSCI_STREAM_HEADER_OVERHEAD        (4U)
/* Default total buffer size in case no optional buffer is provided for stream_read and stream_write and multiple of 4 */
#define ADI_APOLLO_HAL_REGIO_HSCI_STREAM_DEFAULT_SIZE           (ADI_APOLLO_HAL_REGIO_HSCI_STREAM_DEFAULT_PAYLOAD_SIZE + ADI_APOLLO_HAL_REGIO_HSCI_STREAM_HEADER_OVERHEAD)

/*!
 * \brief HSCI transaction data size
 */
typedef enum {
    ADI_APOLLO_HAL_REGIO_HSCI_BUS_SIZE_D8 = 0,
    ADI_APOLLO_HAL_REGIO_HSCI_BUS_SIZE_D16 = 1,
    ADI_APOLLO_HAL_REGIO_HSCI_BUS_SIZE_D32 = 2,
} adi_apollo_hal_regio_hsci_bus_size_e;

#ifndef CLIENT_IGNORE
/*!
 * \brief HSCI register I/O descriptor
 */
typedef struct {
    uint8_t is_used;                                    /*!< 0 = protocol block not used (default), 1 = is used */

    /* These functions are supplied by the customer platform driver */
    adi_apollo_hal_init_t          init;                /*!< Platform init function ptr. \ref adi_apollo_hal_init_t */
    adi_apollo_hal_xfer_t          xfer;                /*!< Platform rd/wr transaction function ptr. \ref adi_apollo_hal_xfer_t */
    adi_apollo_hal_write_t         write;               /*!< Platform write transaction function ptr.  \ref adi_apollo_hal_write_t */
    adi_apollo_hal_read_t          read;                /*!< Platform read transaction function ptr. \ref adi_apollo_hal_read_t */
    adi_apollo_hal_poll_read_t     poll_read;           /*!< Platform poll read transaction function ptr. \ref adi_apollo_hal_poll_read_t */
    adi_apollo_hal_manual_linkup_t manual_linkup;       /*!< Platform manual linkup transaction function ptr. \ref adi_apollo_hal_manual_linkup_t */
    adi_apollo_hal_auto_linkup_t   auto_linkup;         /*!< Platform auto linkup transaction function ptr. \ref adi_apollo_hal_auto_linkup_t */
    adi_apollo_hal_alink_tbl_get_t alink_tbl_get;       /*!< Platform auto link_table get function ptr. \ref adi_apollo_hal_alink_tbl_get_t */

    adi_apollo_device_hsci_settings_t hsci_config;      /*!< Device hsci settings. \ref adi_apollo_device_hsci_settings_t */
    void                       *dev_obj;                /*!< Platform defined object */

    /* Internal API fields */
    adi_apollo_hal_regio_t     base_regio;              /*!< Pointer to base register I/O */
    
    uint8_t                    *buff;                   /*!< User allocated memory for stream transaction usage. */
    uint32_t                   buff_len;                /*!< Length of allocated memory for stream transaction usage. */
} adi_apollo_hal_regio_hsci_desc_t;
#endif /* CLIENT_IGNORE*/

#endif /* __ADI_APOLLO_HAL_REGIO_HSCI_TYPES_H__ */

/*! @} */