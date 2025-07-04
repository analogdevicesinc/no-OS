/*!
 * \brief     Common API definitions header file.
 *            This file contains all common API definitions.
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_API_COMMON
 * @{
 */
#ifndef __ADI_CMS_API_COMMON_H__
#define __ADI_CMS_API_COMMON_H__

/*============= I N C L U D E S ============*/
#ifdef __KERNEL__
#include <linux/kernel.h>
#include <linux/math64.h>
#include <linux/delay.h>

#ifndef UINT64_MAX
#define UINT64_MAX  U64_MAX
#endif

#else
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#endif
#include "adi_cms_api_config.h"

/*============= D E F I N E S ==============*/

#ifndef __maybe_unused
#define __maybe_unused		__attribute__((__unused__))
#endif

/*!
 * \brief  Enumerate error code
 */
typedef enum {
    API_CMS_ERROR_OK                        =  0,  /*!< No Error */
    API_CMS_ERROR_ERROR                     = -1,  /*!< General Error  */
    API_CMS_ERROR_NULL_PARAM                = -2,  /*!< Null parameter */
    API_CMS_ERROR_OVERFLOW                  = -3,  /*!< General overflow */
    API_CMS_ERROR_DIV_BY_ZERO               = -4,  /*!< Divide by zero */
    API_CMS_ERROR_FEAT_LOCKOUT              = -5,  /*!< Device feature is locked out */
    API_CMS_ERROR_SPI_SDO                   = -10, /*!< Wrong value assigned to the SDO in device structure */
    API_CMS_ERROR_INVALID_HANDLE_PTR        = -11, /*!< Device handler pointer is invalid */
    API_CMS_ERROR_INVALID_XFER_PTR          = -12, /*!< Invalid pointer to the SPI write or read function assigned */
    API_CMS_ERROR_INVALID_DELAYUS_PTR       = -13, /*!< Invalid pointer to the delay_us function assigned */
    API_CMS_ERROR_INVALID_PARAM             = -14, /*!< Invalid parameter passed */
    API_CMS_ERROR_INVALID_RESET_CTRL_PTR    = -15, /*!< Invalid pointer to the reset control function assigned */
    API_CMS_ERROR_NOT_SUPPORTED             = -16, /*!< Not supported */
    API_CMS_ERROR_INVALID_MASK_SELECT       = -17, /*!< Invalid bitmask select parameter passed */
    API_CMS_ERROR_IN_REF_STATUS             = -19, /*!< The Input Reference Signal is not available */
    API_CMS_ERROR_VCO_OUT_OF_RANGE          = -20, /*!< The VCO is out of range */
    API_CMS_ERROR_PLL_NOT_LOCKED            = -21, /*!< PLL is not locked */
    API_CMS_ERROR_DLL_NOT_LOCKED            = -22, /*!< DLL is not locked */
    API_CMS_ERROR_MODE_NOT_IN_TABLE         = -23, /*!< JESD Mode not in table */
    API_CMS_ERROR_CLK_CKT                   = -24, /*!< Clock circuit error */
    API_CMS_ERROR_FTW_LOAD_ACK              = -30, /*!< FTW acknowledge not received */
    API_CMS_ERROR_NCO_NOT_ENABLED           = -31, /*!< The NCO is not enabled */
    API_CMS_ERROR_INIT_SEQ_FAIL             = -40, /*!< Initialization sequence failed */
    API_CMS_ERROR_TEST_FAILED               = -50, /*!< Test failed */
    API_CMS_ERROR_SPI_XFER                  = -60, /*!< SPI transfer error */
    API_CMS_ERROR_TX_EN_PIN_CTRL            = -62, /*!< TX enable function error */
    API_CMS_ERROR_RESET_PIN_CTRL            = -63, /*!< HW reset function error */
    API_CMS_ERROR_EVENT_HNDL                = -64, /*!< Event handling error */
    API_CMS_ERROR_HW_OPEN                   = -65, /*!< HW open function error */
    API_CMS_ERROR_HW_CLOSE                  = -66, /*!< HW close function error */
    API_CMS_ERROR_LOG_OPEN                  = -67, /*!< Log open error */
    API_CMS_ERROR_LOG_WRITE                 = -68, /*!< Log write error */
    API_CMS_ERROR_LOG_CLOSE                 = -69, /*!< Log close error */
    API_CMS_ERROR_DELAY_US                  = -70, /*!< Delay error */
    API_CMS_ERROR_HSCI_LINK_UP              = -79, /*!< HSCI Linkup error */
    API_CMS_ERROR_SPI_REGIO_XFER            = -80, /*!< Register transaction error spi */
    API_CMS_ERROR_HSCI_REGIO_XFER           = -81, /*!< Register transaction error hsci */
    API_CMS_ERROR_OPERATION_TIMEOUT         = -82, /*!< Operation timeout */
    API_CMS_ERROR_LINK_DOWN                 = -83, /*!< JESD links down */
    API_CMS_ERROR_FILE_OPEN                 = -84, /*!< File open error */
    API_CMS_ERROR_SERDES_CAL_ERROR          = -85, /*!< SERDES cal error */
    API_CMS_ERROR_SERDES_CAL_TIMEOUT        = -86, /*!< SERDES cal timeout */
    API_CMS_ERROR_PLATFORM_READ             = -87, /*!< Platform (e.g. FPGA) read error */
    API_CMS_ERROR_PLATFORM_WRITE            = -88, /*!< Platform (e.g. FPGA) write error */
    API_CMS_ERROR_FILE_READ                 = -89, /*!< File read error */
    API_CMS_ERROR_FILE_WRITE                = -90, /*!< File write error */
    API_CMS_ERROR_FILE_OPERATION            = -91, /*!< General file error (e.g. seek) */
    API_CMS_ERROR_PLATFORM_IMAGE_LOAD       = -92, /*!< Error loading platform FPGA image */
    API_CMS_ERROR_NOT_IMPLEMENTED           = -93, /*!< Feature not currently implemented */
    API_CMS_ERROR_STRUCT_UNPOPULATED        = -94, /*!< Struct not populated */
    API_CMS_ERROR_PROTOCOL_OP_NOT_SUPPORTED = -95, /*!< Protocol not supported for operation */

    API_CMS_ERROR_MEM_ALLOC                 = -100, /*!< memory allocation error */
    API_CMS_ERROR_MMAP                      = -101, /*!< memory mapping error */
    API_CMS_ERROR_DEV_MEM_OPEN              = -102, /*!< device memory open error */

    API_CMS_ERROR_I2C_ERROR                 = -110, /*!< I2C General Error.  */
    API_CMS_ERROR_I2C_WRITE                 = -111, /*!< I2C Write Operation Failed. */
    API_CMS_ERROR_I2C_READ                  = -112, /*!< I2C Read Operation Failed. */
    API_CMS_ERROR_I2C_BUSY                  = -113, /*!< I2C controller or device is busy.  */
    API_CMS_ERROR_PMOD_NVM_LOCK             = -114, /*!< A fault occurred while accessing the Power Module's NVM (Non Volatile Memory).  */

    API_CMS_ERROR_EC_RAM_LOCK_ERROR         = -120, /*!< EC ram-lock error */
    API_CMS_ERROR_PROFILE_CRC               = -121, /*!< Profile CRC invalid */
    API_CMS_ERROR_MAILBOX_RESP_STATUS       = -122, /*!< Mailbox Command Response Status Error */

    API_CMS_ERROR_MCS_CAL_CONFIG_ERROR      = -130,  /*!< MCS Cal Configuration related error */
    API_CMS_ERROR_MCS_INIT_CAL_ERROR        = -131,  /*!< MCS Init Cal related error */
    API_CMS_ERROR_MCS_TRACK_CAL_ERROR       = -132,  /*!< MCS Tracking Cal related error */
    API_CMS_ERROR_MCS_CAL_TIMEOUT           = -133,  /*!< MCS Cal run or status check response timed out error */
    API_CMS_ERROR_ADC_INIT_CAL_ERROR        = -134,  /*!< ADC Init Cal related error */
    API_CMS_ERROR_ADC_TRACK_CAL_ERROR       = -135,  /*!< ADC Tracking Cal related error */
    API_CMS_ERROR_ADC_CAL_TIMEOUT           = -136,  /*!< ADC Cal run or status check response timed out error */

    API_CMS_ERROR_BAD_STATE                 = -140,  /*!< Device is not in appropriate state to perform operation */
    
    API_CMS_ERROR_STARTUP_FW_RDY_FOR_PROFILE_ERROR = -150,   /*!< FW did not reach ready for profile config state */
    API_CMS_ERROR_STARTUP_FW_MAILBOX_RDY_ERROR = -151,  /*!< FW did not reach mailbox ready state */

    API_CMS_ERROR_PLATFORM_CAPTURE_INVALID_CONFIG = -200, /*!< Invalid platform capture configuration */
                                               
                                              
                              
} adi_cms_error_e;

/*!
 * \brief  Enumerate log source type
 */
typedef enum {
    ADI_CMS_LOG_NONE = 0x0000,                  /*!< all not selected */
    ADI_CMS_LOG_ERR  = 0x0001,                  /*!< error message */
    ADI_CMS_LOG_WARN = 0x0002,                  /*!< warning message */
    ADI_CMS_LOG_MSG  = 0x0004,                  /*!< tips info message */
    ADI_CMS_LOG_SPI  = 0x0010,                  /*!< spi r/w info message */
    ADI_CMS_LOG_API  = 0x0020,                  /*!< api info message */
    ADI_CMS_LOG_ALL  = 0xFFFF                   /*!< all selected */
} adi_cms_log_type_e;

/*!
 * \brief  ADI Device Identification Data
 */
typedef struct {
    uint8_t  chip_type;                         /*!< Chip Type Code */
    uint16_t prod_id;                           /*!< Product ID Code */
    uint8_t  prod_grade;                        /*!< Product Grade Code */
    uint8_t  dev_revision;                      /*!< Device Revision */
}adi_cms_chip_id_t;

/*!
 * \brief  API register access structure
 */
typedef struct {
    uint16_t reg;                               /*!< Register address */
    uint8_t  val;                               /*!< Register value */
}adi_cms_reg_data_t;

/*!
 * \brief  SPI mode settings
 */
typedef enum {
    SPI_NONE = 0,                               /*!< Keep this for test */
    SPI_SDO = 1,                                /*!< SDO  active, 4-wire only */
    SPI_SDIO = 2,                               /*!< SDIO active, 3-wire only */
    SPI_CONFIG_MAX = 3                          /*!< Keep it last */
}adi_cms_spi_sdo_config_e;

/*!
 * \brief  SPI bit order settings
 */
typedef enum {
    SPI_MSB_LAST  = 0,                          /*!< LSB first */
    SPI_MSB_FIRST = 1                           /*!< MSB first */
}adi_cms_spi_msb_config_e;

/*!
 * \brief  SPI address increment settings
 */
typedef enum {
    SPI_ADDR_DEC_AUTO = 0,                      /*!< auto decremented */
    SPI_ADDR_INC_AUTO = 1                       /*!< auto incremented */
}adi_cms_spi_addr_inc_e;

/*!
 *\brief driver operation mode
 */
typedef enum
{
    OPEN_DRAIN_MODE = 0,
    CMOS_MODE = 1
}adi_cms_driver_mode_config_e;

/*!
 * \brief  Enumerate Impedance Types
 */
typedef enum {
    ADI_CMS_NO_INTERNAL_RESISTOR = 0,           /*!< disable internal resistor */
    ADI_CMS_INTERNAL_RESISTOR_100_OHM = 1,      /*!< internal 100ohm resistor */
    ADI_CMS_INTERNAL_RESISTOR_50_OHM = 2,       /*!< internal  50ohm resistor */
    ADI_CMS_INTERNAL_RESISTOR_UNKNOWN = 3        /*!< unknown type */
}adi_cms_signal_impedance_type_e;

/*!
 * \brief  Enumerate Signal Types
 */
typedef enum {
    SIGNAL_CMOS = 0,                            /*!< CMOS signal */
    SIGNAL_LVDS = 1,                            /*!< LVDS signal */
    SIGNAL_CML = 2,                             /*!< CML  signal */
    SIGNAL_LVPECL = 3,                          /*!< LVPECL signal */
    SIGNAL_UNKNOWN = 4                          /*!< UNKNOW signal */
}adi_cms_signal_type_e;

/*!
 * \brief  Enumerate coupling mode
 */
typedef enum {
    COUPLING_AC = 0,                            /*!< AC coupled signal */
    COUPLING_DC = 1,                            /*!< DC signal */
    COUPLING_UNKNOWN = 2                        /*!< UNKNOWN coupling */
}adi_cms_signal_coupling_e;

/*!
 * \brief  Enumerates JESD LINK Signals
 */
typedef enum {
    JESD_LINK_NONE = 0,                         /*!< JESD link none  */
    JESD_LINK_0 = 1,                            /*!< JESD link 0 */
    JESD_LINK_1 = 2,                            /*!< JESD link 1 */
    JESD_LINK_ALL = 3                           /*!< All JESD links  */
}adi_cms_jesd_link_e;

/*!
 * \brief  Enumerates SYNCOUTB Output Signals
 */
typedef enum {
    SYNCOUTB_0 = 0x0,                           /*!< SYNCOUTB 0 */
    SYNCOUTB_1 = 0x1,                           /*!< SYNCOUTB 1 */
    SYNCOUTB_ALL = 0xFF                         /*!< ALL SYNCOUTB SIGNALS */
}adi_cms_jesd_syncoutb_e;

/*!
 * \brief  Enumerates SYSREF Synchronization Mode
 */
typedef enum {
    SYSREF_NONE = 0,                            /*!< No SYSREF Support */
    SYSREF_ONESHOT = 1,                         /*!< ONE-SHOT SYSREF */
    SYSREF_CONT = 2,                            /*!< Continuous SysRef sync. */
    SYSREF_MON = 3,                             /*!< SYSREF monitor mode */
    SYSREF_MODE_INVALID = 4
}adi_cms_jesd_sysref_mode_e;

/*!
 * \brief  Enumerates PRBS pattern type
 */
typedef enum {
    PRBS_NONE = 0,                              /*!< PRBS   off */
    PRBS7  = 1,                                 /*!< PRBS7  pattern */
    PRBS9  = 2,                                 /*!< PRBS9  pattern */
    PRBS15 = 3,                                 /*!< PRBS15 pattern */
    PRBS23 = 4,                                 /*!< PRBS23 pattern */
    PRBS31 = 5,                                 /*!< PRBS31 pattern */
    PRBS_MAX = 6                                /*!< Number of member */
}adi_cms_jesd_prbs_pattern_e;

/*!
 * \brief  Enumerates all available Jesd Subclass Modes
 */
typedef enum {
    JESD_SUBCLASS_0 = 0,                        /*!< JESD SUBCLASS 0 */
    JESD_SUBCLASS_1 = 1,                        /*!< JESD SUBCLASS 1 */
    JESD_SUBCLASS_INVALID = 2
}adi_cms_jesd_subclass_e;

/*!
 * \brief  Defines JESD Parameters
 */
typedef struct {
    uint8_t jesd_l;                             /*!< No of lanes */
    uint8_t jesd_f;                             /*!< No of octets in a frame */
    uint8_t jesd_m;                             /*!< No of converters */
    uint8_t jesd_s;                             /*!< No of samples */
    uint8_t jesd_hd;                            /*!< High Density */
    uint16_t jesd_k;                            /*!< No of frames for a multi-frame */
    uint8_t jesd_n;                             /*!< Converter resolution */
    uint8_t jesd_np;                            /*!< Bit packing sample */
    uint8_t jesd_cf;                            /*!< Parameter CF */
    uint8_t jesd_cs;                            /*!< Parameter CS */
    uint8_t jesd_did;                           /*!< Device ID DID */
    uint8_t jesd_bid;                           /*!< Bank ID.  BID */
    uint8_t jesd_lid0;                          /*!< Lane ID for lane0 */
    uint8_t jesd_subclass;                      /*!< Subclass */
    uint8_t jesd_scr;                           /*!< Scramble enable */
    uint8_t jesd_duallink;                      /*!< Link mode (single/dual) */
    uint8_t jesd_jesdv;                         /*!< Version (0:204A, 1:204B, 2:204C) */
    uint8_t jesd_mode_id;                       /*!< JESD mode ID */
    uint8_t jesd_mode_c2r_en;                   /*!< JESD mode C2R enable */
    uint8_t jesd_mode_s_sel;                    /*!< JESD mode S value */
    uint8_t jtx_ns;                             /*!< JTx NS value. ADI, non-jesd standard param */
}adi_cms_jesd_param_t;

/*!
 * \brief Device transaction config structure
 */
typedef struct {
    uint8_t                     addr_len;       /*!< Interface reg address size, in bytes */
    uint8_t                     data_len;       /*!< Interface reg data size, in bytes */
    uint16_t                    stream_len;     /*!< Interface transaction stream length. Number of data words to transfer. */
    uint32_t                    mask;           /*!< Interface reg mask */
} adi_cms_hal_txn_config_t;

#ifndef CLIENT_IGNORE

typedef int32_t(*adi_cms_spi_write_t)(void *user_data, const uint8_t *in_data, uint32_t size_bytes, adi_cms_hal_txn_config_t *txn_config);
typedef int32_t(*adi_cms_spi_read_t)(void *user_data, const uint8_t *in_data, uint8_t *out_data, uint32_t size_bytes, adi_cms_hal_txn_config_t *txn_config);
typedef int32_t(*adi_cms_delay_us_t)(void *user_data, uint32_t us);

#endif

#define _ADI_CMS_ERROR_PRINT_ENABLED(err) fprintf(stderr, "TB:ERROR %d in %s(): line %d in file \"%s\"\n", err, __func__, __LINE__, __FILE__)
#define _ADI_CMS_ERROR_PRINT_DISABLED(err) ;

#ifdef ADI_EN_STDERR
#define ADI_CMS_ERROR_PRINT(err) _ADI_CMS_ERROR_PRINT_ENABLED(err)
#else
#define ADI_CMS_ERROR_PRINT(err) _ADI_CMS_ERROR_PRINT_DISABLED(err)
#endif /*ADI_EN_STDERR*/

#define ADI_CMS_ERROR_RETURN(err)          \
    {                                      \
        if (err != API_CMS_ERROR_OK) {     \
            ADI_CMS_ERROR_PRINT(err);      \
            return err;                    \
        }                                  \
    }

#define ADI_CMS_ERROR_GOTO(err, label)     \
    {                                      \
        if (err != API_CMS_ERROR_OK) {     \
            ADI_CMS_ERROR_PRINT(err);      \
            goto label;                    \
        }                                  \
    }

#define ADI_CMS_CHECK(r, err)              {if (r) ADI_CMS_ERROR_RETURN(err);}
#define ADI_CMS_NULL_PTR_CHECK(r)          ADI_CMS_CHECK(r == NULL, API_CMS_ERROR_NULL_PARAM)
#define ADI_CMS_MEM_ALLOC_CHECK(r)         ADI_CMS_CHECK(r == NULL, API_CMS_ERROR_MEM_ALLOC)
#define ADI_CMS_FILE_OPEN_CHECK(r)         ADI_CMS_CHECK(r == NULL, API_CMS_ERROR_FILE_OPEN)
#define ADI_CMS_INVALID_PARAM_CHECK(r)     ADI_CMS_CHECK(r, API_CMS_ERROR_INVALID_PARAM)
#define ADI_CMS_SINGLE_SELECT_CHECK(r)     ADI_CMS_CHECK(!(r > 0 && (r & (r - 1)) == 0), API_CMS_ERROR_INVALID_MASK_SELECT)
#define ADI_CMS_RANGE_CHECK(r, min, max)   ADI_CMS_CHECK(r < min || r > max, API_CMS_ERROR_INVALID_PARAM)

#define ADI_CMS_MASK_MATCH(value, mask)    ((value & mask) == mask)

#define ADI_CMS_MEM_ALLOC_FREE(r)          if (r != NULL) {free(r); r = NULL;}
#define ADI_CMS_FILE_CLOSE(r)              if (r != NULL) {fclose(r); r = NULL;}

#endif /* __ADI_API_COMMON_H__ */
/*! @} */