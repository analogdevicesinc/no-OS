/*!
 * \brief     Contains spi hal register io types
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

#ifndef __ADI_APOLLO_HAL_REGIO_SPI_TYPES_H__
#define __ADI_APOLLO_HAL_REGIO_SPI_TYPES_H__

#include "adi_cms_api_common.h"
#include "adi_apollo_hal_types.h"
#include "adi_apollo_device_types.h"
#include "adi_apollo_hal_regio.h"

#define ADI_APOLLO_DIRECT_SPI_REGION_LEN 0x4000ul

/*!
 * \brief SPI transaction data size
 */
typedef enum {
    ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D8 = 0,
    ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D16 = 1,
    ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D32 = 2
} adi_apollo_hal_regio_spi_bus_size_e;

#ifndef CLIENT_IGNORE
/*!
 * \brief SPI register I/O descriptor
 */
typedef struct {
    uint8_t is_used;                                /*!< 0 = protocol block not used (default), 1 = is used */
    uint8_t is_spi1;                                /*!< Indicates spi instance: 0 = spi0, 1 = spi1 */
    uint8_t rd_stream_en;                           /*!< Enable rd streaming transactions by API. 0 = disable, 1 = enable */    
    uint8_t wr_stream_en;                           /*!< Enable we streaming transactions by API. 0 = disable, 1 = enable */

    /* These functions are supplied by the customer platform driver */
    adi_apollo_hal_init_t       init;               /*!< Platform init function ptr. \ref adi_apollo_hal_init_t */
    adi_apollo_hal_xfer_t       xfer;               /*!< Platform rd/wr transaction function ptr. \ref adi_apollo_hal_xfer_t */
    adi_apollo_hal_write_t      write;              /*!< Platform write transaction function ptr.  \ref adi_apollo_hal_write_t */
    adi_apollo_hal_read_t       read;               /*!< Platform read transaction function ptr. \ref adi_apollo_hal_read_t */
    adi_apollo_hal_poll_read_t  poll_read;          /*!< Platform poll read transaction function ptr. \ref adi_apollo_hal_poll_read_t */
    
    adi_apollo_device_spi_settings_t spi_config;    /*!< Device spi settings. \ref adi_apollo_device_spi_settings_t */
    void                        *dev_obj;           /*!< Platform defined object for SPI instance */

    /* Internal API fields */
    adi_apollo_hal_regio_t      base_regio;         /*!< Base HAL register I/O */
    uint32_t                    page_base_addr;     /*!< Current SPI page base address */
    adi_apollo_hal_txn_config_t txn_config;         /*!< Transaction info passed to HAL read/write functions */
} adi_apollo_hal_regio_spi_desc_t;
#endif /* CLIENT_IGNORE*/

#endif /* __ADI_APOLLO_HAL_REGIO_SPI_TYPES_H__ */

/*! @} */