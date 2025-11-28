/**
 * Copyright 2015 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information.
 * see the "LICENSE.pdf" file in this zip file.
 */

#ifndef __ADI_PLATFORM_TYPES_H__
#define __ADI_PLATFORM_TYPES_H__

#include "adi_library_types.h"
#include "adi_platform_impl_types.h"

#define PLATFORM_ENV_VARIABLE                "ADI_PLATFORM_NAME"

#define MEM_SIZE_4KB                         (uint32_t) 0x00001000U
#define MEM_SIZE_16PLUSMB                    (uint32_t) 0x010B0000U
#define MEM_SIZE_16MB                        (uint32_t) 0x01000000U
#define MEM_SIZE_256MB                       (uint32_t) 0x10000000U
#define MEM_SIZE_512MB                       (uint32_t) 0x20000000U
#define MEM_SIZE_1GB                         (uint32_t) 0x40000000U

#define SYM2STR(x)                           #x

#define LOG_ERR_COUNT_LIMIT                 (uint8_t) 0xFFU

#define ADI_DEVICE_HAL_OPEN     1U
#define ADI_DEVICE_HAL_CLOSED   0U

/**
 *  \brief  HAL Error Codes
 *
 *      Hal Error Codes is flexible to Extended onto Operating System Codes
 *
 *      Linux System Errors Ranger 1 - 124
 *
 *      Offset HAL Errors to 256
 *
 *      Current ADI Platform will not extend error codes, instead store the OS Error Code into the Device HAL Structure
 */
typedef enum adi_hal_Err
{
    ADI_HAL_ERR_OK              = 0x00L,    /*!< No Error */
    ADI_HAL_ERR_NULL_PTR        = 0xFFL,    /*!< Null Pointer */
    ADI_HAL_ERR_PARAM,                      /*!< Invalid Parameter Error */
    ADI_HAL_ERR_I2C,                        /*!< I2C Error */
    ADI_HAL_ERR_I2C_WRITE,                  /*!< I2C Write Error */
    ADI_HAL_ERR_I2C_READ,                   /*!< I2C Read Error */
    ADI_HAL_ERR_SPI,                        /*!< SPI Error */
    ADI_HAL_ERR_SPI_WRITE,                  /*!< SPI Write Error */
    ADI_HAL_ERR_SPI_READ,                   /*!< SPI Read Error */
    ADI_HAL_ERR_SPI_CS,                     /*!< SPI Chip Select Error */
    ADI_HAL_ERR_GPIO,                       /*!< GPIO Error */
    ADI_HAL_ERR_TIMER,                      /*!< Timer Error */
    ADI_HAL_ERR_BBICCTRL,                   /*!< BBIC Control Error */
    ADI_HAL_ERR_BBICCTRL_CORE,              /*!< BBIC Core Error */
    ADI_HAL_ERR_BBICCTRL_WORD_BOUNDARY,     /*!< BBIC Address must be word aligned */
    ADI_HAL_ERR_BBICCTRL_RAM,               /*!< BBIC RAM Error */
    ADI_HAL_ERR_BBICCTRL_SPI,               /*!< BBIC SPI Error */
    ADI_HAL_ERR_MEMORY,                     /*!< Memory Error (e.g. Allocation) */
    ADI_HAL_ERR_LOG,                        /*!< Logging Error */
    ADI_HAL_ERR_LIBRARY,                    /*!< Library Module Error Code */
    ADI_HAL_ERR_EEPROM_DATA,                /*!< EEPROM Data Error Code (e.g. Failed CRC, Invalid Structure Parameter(s) */
    ADI_HAL_ERR_NOT_IMPLEMENTED             /*!< HAL Feature Not Implemented */
} adi_hal_Err_e;

/**
 *  \brief Logging Error Codes
 */
typedef enum adi_hal_LogErr
{
    ADI_LOG_ERR_OK              = 0x00UL,   /*!< No Error */
    ADI_LOG_ERR_OPENING_FILE    = 0x01UL,   /*!< Opening Log File Error Flag */
    ADI_LOG_ERR_CLOSING_FILE    = 0x02UL,   /*!< Closing Log File Error Flag */
    ADI_LOG_ERR_FFLUSH          = 0x04UL,   /*!< Stream Buffer Flushing Error */
    ADI_LOG_ERR_LEVEL           = 0x08UL,   /*!< Log Level Error Flag */
    ADI_LOG_ERR_WRITE           = 0x10UL,   /*!< Writing to File Error Flag */
    ADI_LOG_ERR_REWIND          = 0x20UL    /*!< File Position Reset */
} adi_hal_LogErr_e;

/**
 *  \brief  Log Level Message Categories
 */
typedef enum adi_hal_LogLevel
{
    ADI_HAL_LOG_NONE        = 0x0UL,    /*!< No Logging Flag */
    ADI_HAL_LOG_MSG         = 0x1UL,    /*!< Message Logging Flag */
    ADI_HAL_LOG_WARN        = 0x2UL,    /*!< Warning Message Logging Flag */
    ADI_HAL_LOG_ERR         = 0x4UL,    /*!< Error Message Logging Flag */
    ADI_HAL_LOG_API         = 0x8UL,    /*!< API Entry Logging Flag */
    ADI_HAL_LOG_API_PRIV    = 0x10UL,   /*!< Private API Entry Flag */
    ADI_HAL_LOG_BF          = 0x20UL,   /*!< Bitfield API Entry Flag */
    ADI_HAL_LOG_HAL         = 0x40UL,   /*!< HAL API Entry Flag */
    ADI_HAL_LOG_SPI         = 0x80UL,   /*!< SPI Transaction Flag */
    ADI_HAL_LOG_ALL         = 0xFFUL    /*!< All Logging Enabled */
} adi_hal_LogLevel_e;

/**
 * \brief Enum type for Console Logging
 */
typedef enum adi_hal_LogConsole
{
    ADI_LOG_CONSOLE_OFF = 0UL,          /*!< Logging to Console Disabled */
    ADI_LOG_CONSOLE_ON                  /*!< Logging to Console Enabled */
} adi_hal_LogConsole_e;

/**
 * \brief Enum type for HAL platforms
 */
typedef enum adi_hal_Platforms
{
    ADI_ADS10_PLATFORM = 0U,        /*!< ADS10 Platform */
    ADI_UNKNOWN_PLATFORM
} adi_hal_Platforms_e;

/**
* \brief Enum type for HAL Board (daughter card) Identity
*/
typedef enum adi_hal_Boards
{
    ADI_BOARD_UNKNOWN = 0U,         /*!< Unknown Board Type */
    ADI_BOARD_BLANK_EEPROM         /*!< Selects for no daughter board present */
} adi_hal_Boards_e;

/**
 * \brief Enum type for the hardware interfaces supported by the platform.
 *        Each device specifies which interfaces it needs from the BBIC in its
 *        devHalCfg structure.
 */
typedef enum adi_hal_Interfaces
{
    ADI_HAL_INTERFACE_SPI       = 0x01U,    /* Device requires SPI interface from BBIC */
    ADI_HAL_INTERFACE_LOG       = 0x02U,    /* Device requires Logging interface from BBIC */
    ADI_HAL_INTERFACE_BBICCTRL  = 0x04U,    /* Device requires BBIC Register/Control interface from BBIC. Mutually exclusive with HWRESET */
    ADI_HAL_INTERFACE_HWRESET   = 0x08U,    /* Device requires GPIO (reset pins) interface from BBIC. Mutually exclusive with BBICCTRL */
    ADI_HAL_INTERFACE_TIMER     = 0x10U,    /* Device requires Timer interface from BBIC */
    ADI_HAL_INTERFACE_I2C       = 0x20U,    /* Device requires I2C interface from BBIC */
    ADI_HAL_INTERFACE_EEPROM    = 0x40U     /* Device requires EEPROM interface from BBIC */
} adi_hal_Interfaces_e;

/**
 * \brief BBIC interfaces
 */
typedef enum adi_hal_BbicInterfaces
{
    ADI_HAL_BBIC_CORE       = 0x01U, /* Core FPGA registers, including HwReset pins */
    ADI_HAL_BBIC_RAM        = 0x02U, /* Ram registers */
    ADI_HAL_BBIC_SPI        = 0x04U  /* Advanced SPI configuration registers */
} adi_hal_BbicInterfaces_e;

/**
 * \brief The set of key values HAL supports for associating a value with a thread.
 */
typedef enum
{
    HAL_TLS_ERR = 0U,   /*!< Key for associating an adi_common_ErrData_t with a thread. */
    HAL_TLS_USR,        /*!< Key for associating an arbitrary void* with a thread. */
    HAL_TLS_END         /*!< Marks end of valid values; And a special case in adi_hal_TlsSet */
} adi_hal_TlsType_e;

/**
 * \brief Data Structure for Logging Facility
 */
typedef struct adi_hal_LogCfg
{
    uint8_t                 interfaceEnabled;                       /*!< Interface Enabled Flag */
    uint32_t                logMask;                                /*!< Logging BitMask */
    FILE*                   logfd;                                  /*!< File Pointer for Log File */
    char                    logFileName[ADI_HAL_STRING_LENGTH];     /*!< Log File Name */
    uint32_t                currentLineNumber;                      /*!< Current Line Number */
    adi_hal_LogConsole_e    logConsole;                             /*!< Logging to Console Flag */
    uint8_t                 errCount;                               /*!< Logging Feature Error Count */
    uint32_t                errFlags;                               /*!< Bit Mask for Logging Feature Error Types */
} adi_hal_LogCfg_t;

/**
 * \brief Data Structure for Logging Status
 */
typedef struct adi_hal_LogStatusGet
{
    uint8_t                 interfaceEnabled;   /*!< Interface Enabled Flag */
    uint8_t                 fileOpen;           /*!< Logging BitMask */
    const char*             logFileName;        /*!< File Pointer for Log File */
    uint32_t                logMask;            /*!< Log File Name */
    adi_hal_LogConsole_e    logConsole;         /*!< Logging to Console Flag */
    uint8_t                 errCount;           /*!< Logging Feature Error Count */
    uint32_t                errFlags;           /*!< Bit Mask of adi_hal_LogErr_e Flags for Logging Feature Error Types */
    uint32_t                currentLineNumber;  /*!< Current Line Number */
} adi_hal_LogStatusGet_t;

/**
 * \brief Data structure for SPI configuration
 */
typedef struct adi_hal_SpiCfg
{
    uint8_t interfaceEnabled;
    int fd;
    char spidevName[ADI_HAL_STRING_LENGTH];
    uint8_t chipSelectIndex;        /*!< valid 1~8 */
    uint8_t CPHA;                   /*!< clock phase, sets which clock edge the data updates (valid 0 or 1) */
    uint8_t CPOL;                   /*!< clock polarity 0 = clock starts low, 1 = clock starts high */
    uint8_t enSpiStreaming;         /*!< Not implemented. SW feature to improve SPI throughput. */
    uint8_t autoIncAddrUp;          /*!< Not implemented. For SPI Streaming, set address increment direction. 1= next addr = addr+1, 0:addr = addr-1 */
    uint8_t fourWireMode;           /*!< 1: Use 4-wire SPI, 0: 3-wire SPI (SDIO pin is bidirectional). NOTE: ADI's FPGA platform always uses 4-wire mode. */
    uint32_t spiClkFreq_Hz;         /*!< SPI Clk frequency in Hz (default 25000000), platform will use next lowest frequency that it's baud rate generator can create */
} adi_hal_SpiCfg_t;

/**
 * \brief Data structure for Hardware reset pin functionality
 */
    typedef struct adi_hal_HwResetCfg
{
    uint8_t     interfaceEnabled;   /*!<  */
    uint32_t    resetPinIndex;      /*!<  */
} adi_hal_HwResetCfg_t;

/**
 * \brief Data structure for I2C configuration
 */
typedef struct adi_hal_I2cCfg
{
    uint8_t interfaceEnabled;
    int fd;
    char drvName[ADI_HAL_STRING_LENGTH];
} adi_hal_I2cCfg_t;

/**
 * \brief Data structure for EEPROM configuration
 */
typedef struct adi_hal_EepromCfg
{
    uint8_t interfaceEnabled;
    FILE* fd;
    char drvName[ADI_HAL_STRING_LENGTH];
} adi_hal_EepromCfg_t;

/**
 * \brief Data structure for memory Fpga Driver configuration
 */
typedef struct adi_hal_fpga_AxiCfg
{
    char drvName[ADI_HAL_STRING_LENGTH];
    uint32_t mapSize;
    uintptr_t mappedMemoryPtr;
    int fd;
} adi_hal_fpga_AxiCfg_t;

/**
 * \brief Data structure to hold platform Hardware layer
 *        settings for all system related feature of the ADI transceiver device
 * If the system has multiple ADI transceivers, the user should create one of
 * theses structures per transceiver device to specify the HAL settings
 * per transceiver device.
 */
typedef struct adi_hal_BbicCfg
{
    uint8_t interfaceEnabled;               /*!< Bitmask for the BBIC interfaces. Follows adi_hal_BbicInterfaces_e */
    adi_hal_fpga_AxiCfg_t coreRegsCfg;
    adi_hal_fpga_AxiCfg_t ramCfg;
    adi_hal_fpga_AxiCfg_t spiAdvRegsCfg;
    adi_hal_fpga_AxiCfg_t spi2AdvRegsCfg;
} adi_hal_BbicCfg_t;

typedef struct adi_hal_TimerCfg
{
    uint8_t interfaceEnabled; /*!< 1: Enable timer interface // 0: Disable timer interface */
} adi_hal_TimerCfg_t;

/**
 * \brief Data structure to hold platform Hardware layer
 *        settings for all system/platform related features.
 
 * If the system has multiple ADI transceivers/devices, the user should create one of
 * theses structures per device to specify the HAL settings
 * per transceiver device.
 */
typedef struct adi_hal_Cfg
{
    uint32_t                interfacemask;                      /*!< Interface Mask Requested */
    uint8_t                 openFlag;                           /*!< Device Open Status Flag */
    char                    typeName[ADI_HAL_STRING_LENGTH];    /*!< Type Name */
    adi_hal_SpiCfg_t        spiCfg;                             /*!< SPI Configuration */
    adi_hal_LogCfg_t        logCfg;                             /*!< LOG Configuration */
    adi_hal_BbicCfg_t       bbicCfg;                            /*!< BBIC Configuration */
    adi_hal_HwResetCfg_t    hwResetCfg;                         /*!< HW Reset Configuration */
    adi_hal_I2cCfg_t        i2cCfg;                             /*!< I2C Configuration */
    adi_hal_TimerCfg_t      timerCfg;                           /*!< Timer Configuration */
    adi_hal_EepromCfg_t     eepromCfg;                          /*!< Eeprom Configuration */
    int32_t                 error;                              /*!< Operating System Error Code */
} adi_hal_Cfg_t;

/**
 * \brief HAL Board Information Structure
 */
typedef struct adi_hal_BoardInfo
{
    uint8_t manufacturer[ADI_HAL_STRING_LENGTH];    /* Manufacturer */
    uint8_t productName[ADI_HAL_STRING_LENGTH];     /* Product Name */
    uint8_t serialNumber[ADI_HAL_STRING_LENGTH];    /* Serial Number */
    uint8_t partNumber[ADI_HAL_STRING_LENGTH];      /* Part Number */
    uint8_t pcbId[ADI_HAL_STRING_LENGTH];           /* PCB ID */
    uint8_t pcbName[ADI_HAL_STRING_LENGTH];         /* PCB Name */
    uint8_t bomRev[ADI_HAL_STRING_LENGTH];          /* BOM Revision */
} adi_hal_BoardInfo_t;

#endif /* __ADI_PLATFORM_TYPES_H__*/
