/* SPDX-License-Identifier: GPL-2.0 */
/**
 * Copyright 2015 - 2019 Analog Devices Inc.
 * Released under the ADRV9025 API license, for more information.
 * see the "LICENSE.pdf" file in this zip file.
 */

#ifndef __ADI_PLATFORM_TYPES_H__
#define __ADI_PLATFORM_TYPES_H__

#define ADI_HAL_STRING_LENGTH 64
#define ADI_HAL_MAX_LOG_LINE 1000

#ifdef __cplusplus
extern
"C"
 {
#endif

#ifdef __KERNEL__

#include <linux/kernel.h>
#include <linux/firmware.h>
#include <linux/slab.h>
#include <linux/spi/spi.h>
#include <linux/gpio/consumer.h>
#include <linux/fs.h>
#else
#include <stdio.h>
#include <stdint.h>
#include "no_os_gpio.h"
#include "adi_common_types.h"
#endif

#define SPI_CHIP_SELECT_0                            0
#define SPI_CHIP_SELECT_1                            1
#define SPI_CHIP_SELECT_2                            2

#define ADI_MANUFACTURER                     "Analog Devices"
#ifdef __KERNEL__
#define SPI_CHIP_DEV_PATH_0                  "/dev/spidev12345.0\0"
#define SPI_CHIP_DEV_PATH_1                  "/dev/spidev12345.1\0"
#define SPI_CHIP_DEV_PATH_2                  "/dev/spidev12345.2\0"

#define BBIC_CORE_REG_PATH                   "/dev/uio0\0"
#define BBIC_RAM_PATH                        "/dev/uio1\0"
#define BBIC_SPI_ADV_REG_PATH                "/dev/uio2\0"

#define I2C_EEPROM_PATH                      "/sys/bus/i2c/devices/3-0050/eeprom"

#define MOTHERBOARD_NAME_PATH                "/sys/class/uio/uio0/name"
#endif
#define MEM_SIZE_16M                         0x1000
#define MEM_SIZE_256MB                       0x10000000
#define MEM_SIZE_512MB                       0x20000000
#define MEM_SIZE_1GB                         0x40000000

#define SYM2STR(x)                           #x


/**
 *  \brief Enum of possible Errors Detected by HAL layer to be communicated
 *         to ADI APIs.
 */
typedef enum adi_hal_Err
{
    ADI_HAL_OK                    = 0,  /*!< HAL function successful. No error Detected */
    ADI_HAL_I2C_FAIL,                   /*!< HAL I2C operation failure. I2C controller Down */
    ADI_HAL_SPI_FAIL,                   /*!< HAL SPI operation failure. SPI controller Down */
    ADI_HAL_GPIO_FAIL,                  /*!< HAL GPIO function Failure */
    ADI_HAL_TIMER_FAIL,                 /*!< HAL Timer function Failure */
    ADI_HAL_LOGGING_FAIL,               /*!< HAL Logging function Failure */
    ADI_HAL_LOGGGING_LEVEL_FAIL,        /*!< HAL Logging level Failure */
    ADI_HAL_WAIT_TIMEOUT,               /*!< HAL function Timeout */
    ADI_HAL_GEN_SW,                     /*!< HAL function failed due to general invalid  HAL data*/
    ADI_HAL_FUNCTION_NOT_IMP,           /*!< HAL function error, not implemented in HAL layer */
    ADI_HAL_LIBRARY_NOT_AVAILABLE,      /*!< HAL function error, HAL layer library not found or available in this build */
    ADI_HAL_WARNING,                    /*!< HAL function warning that non critical error was detected*/
    ADI_HAL_BBICCTRL_FAIL,              /*!< BBIC control interface HAL function failure */
    ADI_HAL_NULL_PTR                    /*!< Function supplied with null pointer */
} adi_hal_Err_e;

/**
 *  \brief An enumerated type in bit mask format to list the log message
 *         categories or groups.
 */
/* In order to separate trx_hal and the platform layer this enum is duplicated.
   its sister enum is adi_common_LogLevel_e in adi_common_log_types.h which need to reflect
   any changes to this enum.
*/
typedef enum adi_hal_LogLevel
{
    ADI_HAL_LOG_NONE = 0x0,  /*!< HAL Log enum to represent all types of log messages not selected */
    ADI_HAL_LOG_MSG  = 0x1,  /*!< HAL Log enum to represent a log message type*/
    ADI_HAL_LOG_WARN = 0x2,  /*!< HAL Log enum to represent a warning message type*/
    ADI_HAL_LOG_ERR  = 0x4,  /*!< HAL Log enum to represent a error message type*/
    ADI_HAL_LOG_API  = 0x8,  /*!< HAL Log enum to represent an API function entry for logging purposes*/
    ADI_HAL_LOG_API_PRIV = 0x10,  /*!< HAL Log enum to represent an Private API function entry for logging purposes*/
    ADI_HAL_LOG_BF   = 0x20, /*!< HAL Log enum to represent a BF function entry for logging purposes*/
    ADI_HAL_LOG_HAL  = 0x40, /*!< HAL Log enum to represent a ADI HAL function entry for logging purposes*/
    ADI_HAL_LOG_SPI  = 0x80, /*!< HAL Log enum to represent a spi transaction type*/
    ADI_HAL_LOG_ALL  = 0xFF  /*!< HAL Log enum to represent all types of log messages selected */
} adi_hal_LogLevel_e;

/**
 * \brief Enum type for HAL platforms
 */
typedef enum adi_hal_Platforms
{
    ADI_ADS8_PLATFORM,      /*!< ADS8 Platform (default for ADRV9010) */
    ADI_ADS9_PLATFORM,      /*!< ADS9 Platform (default for ADRV9025) */
	ADI_NO_OS_PLATFORM,

    ADI_UNKNOWN_PLATFORM
} adi_hal_Platforms_e;

/**
* \brief Enum type for HAL Board (daughter card) Identity
*/
typedef enum adi_hal_Boards
{
    ADI_BOARD_UNKNOWN,
    ADI_BOARD_BLANK_EEPROM,   /*!< Selects for no daughter board present */
    ADI_BOARD_9010CE01,      /*!< 9010 CE 01 board */
    ADI_BOARD_9010EE01,      /*!< 9010 EE 01 board */
    ADI_BOARD_9010STSHAL,     /*!< 9010 STS board */
    ADI_BOARD_9025CE01,      /*!< 9025 CE 01 board */
    ADI_BOARD_9025EE01,      /*!< 9025 EE 01 board */
    ADI_BOARD_9026CE01,      /*!< 9025 CE 01 board */
    ADI_BOARD_9025STSHAL,     /*!< 9025 STS board */

    /* for range checking */
    ADI_BOARD_9010_MIN = ADI_BOARD_9010CE01,
    ADI_BOARD_9010_MAX = ADI_BOARD_9010STSHAL,
    ADI_BOARD_9025_MIN = ADI_BOARD_9025CE01,
    ADI_BOARD_9025_MAX = ADI_BOARD_9025STSHAL,
} adi_hal_Boards_e;

/**
* \brief Enum type for HAL Board (daughter card) and platform initialization
*/
typedef enum adi_hal_dev_initialization
{
    ADI_DEV_CFG_CREATED     =  0x00000001,
    ADI_PLATFORM_CREATED    =  0x00000002, /*!< 9010 CE board rev 1D */
    ADI_BOARD_CREATED       =  0x00000004, /*!< 9010 EE board rev 1C */
} adi_hal_dev_initialization_e;

/**
 * \brief Enum type for the hardware interfaces supported by the platform.
 *        Each device specifies which interfaces it needs from the BBIC in its
 *        devHalCfg structure.
 */
typedef enum adi_hal_Interfaces
{
    ADI_HAL_INTERFACE_SPI       = 0x01, /* Device requires SPI interface from BBIC */
    ADI_HAL_INTERFACE_LOG       = 0x02, /* Device requires Logging interface from BBIC */
    ADI_HAL_INTERFACE_BBICCTRL  = 0x04, /* Device requires BBIC Register/Control interface from BBIC. Mututally exclusive with HWRESET */
    ADI_HAL_INTERFACE_HWRESET   = 0x08, /* Device requires GPIO (reset pins) interface from BBIC. Mututally exclusive with BBICCTRL */
    ADI_HAL_INTERFACE_TIMER     = 0x10, /* Device requires Timer interface from BBIC */
    ADI_HAL_INTERFACE_I2C       = 0x20  /* Device requires I2C interface from BBIC */
} adi_hal_Interfaces_e;


/**
 * \brief Enum for the specific BBIC interfaces
 */
typedef enum adi_hal_BbicInterfaces
{
    ADI_HAL_BBIC_CORE       = 0x01, /* Core FPGA registers, including HwReset pins */
    ADI_HAL_BBIC_RAM        = 0x02, /* Ram registers */
    ADI_HAL_BBIC_SPI        = 0x04, /* Advanced SPI configuration registers */
} adi_hal_BbicInterfaces_e;

/**
 * \brief Data structure for Logging facility
 */
typedef struct adi_hal_LogCfg
{
    uint8_t interfaceEnabled;
    int32_t logLevel;         /*!< valid 0 - 0xFF */
    uintptr_t logfd;
    char logFileName[ADI_HAL_STRING_LENGTH];
    uint32_t currentLineNumber;
} adi_hal_LogCfg_t;

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
    uint8_t  spiActionDisable;      /*!< Disables the spi Driver actually writing the data */
} adi_hal_SpiCfg_t;

/**
 * \brief Data structure for Hardware reset pin functionality
 */
    typedef struct adi_hal_HwResetCfg
{
    uint8_t interfaceEnabled;
    uint32_t resetPinIndex;
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
 * these structures per transceiver device to specify the HAL settings
 * per transceiver device.
 */
typedef struct adi_hal_BbicCfg
{
    uint8_t interfaceEnabled;               /*!< Bitmask for the BBIC interfaces. Follows adi_hal_BbicInterfaces_e */
    adi_hal_fpga_AxiCfg_t coreRegsCfg;
    adi_hal_fpga_AxiCfg_t ramCfg;
    adi_hal_fpga_AxiCfg_t spiAdvRegsCfg;
} adi_hal_BbicCfg_t;

typedef struct adi_hal_TimerCfg
{
    uint8_t interfaceEnabled;
    /* ADS9 platform does not require any instance members per device */
} adi_hal_TimerCfg_t;


/**
 * \brief Data structure to hold platform Hardware layer
 *        settings for all system/platform related features.

 * If the system has multiple ADI transceivers/devices, the user should create one of
 * these structures per device to specify the HAL settings
 * per transceiver device.
 */
typedef struct adi_hal_Cfg
{
    char typeName[ADI_HAL_STRING_LENGTH];
    adi_hal_SpiCfg_t spiCfg;
    adi_hal_LogCfg_t logCfg;
    adi_hal_BbicCfg_t bbicCfg;
    adi_hal_HwResetCfg_t hwResetCfg;
    adi_hal_I2cCfg_t i2cCfg;
    adi_hal_TimerCfg_t timerCfg;
    uint32_t interfaceMask;

#ifdef __KERNEL__
    struct spi_device *spi;
    struct gpio_desc *reset_gpio;
    struct gpio_desc *int_gpio;
#else
    struct no_os_spi_desc *spi;
    struct no_os_gpio_desc *reset_gpio;
    struct no_os_gpio_desc *int_gpio;
#endif

} adi_hal_Cfg_t;


#ifdef __KERNEL__
typedef struct linux_hal_fileio
{
    adi_hal_Cfg_t *hal;
    const struct firmware *fw;
    char *ptr, *start, *end;
} FILE;

FILE* fopen (const char * filename, const char *mode);

extern int fseek (FILE * stream, long int offset, int origin);
extern long int ftell (FILE *stream);

extern int fclose (FILE *stream);
extern char * fgets(char *dst, int num, FILE *stream);
extern size_t fread(void *ptr, size_t size, size_t count, FILE *stream);
extern size_t fwrite (const void * ptr, size_t size, size_t count, FILE *stream);

extern FILE* __fopen (adi_hal_Cfg_t *hal, const char * filename, const char *mode);
#define fopen(filename, mode) __fopen(device->common.devHalInfo, filename, mode)
#else
typedef struct no_os_hal_fileio
{
    char *data;
    char *ptr, *start, *end;
} CUSTOM_FILE;

FILE* fopen(const char * filename, const char *mode);

extern int fseek (FILE * stream, long int offset, int origin);
extern long int ftell (FILE *stream);

extern int __fclose (FILE *stream);
#define fclose(filename) __fclose(filename)
extern char * fgets(char *dst, int num, FILE *stream);
extern size_t fread(void *ptr, size_t size, size_t count, FILE *stream);
extern size_t fwrite (const void * ptr, size_t size, size_t count, FILE *stream);

extern FILE* __fopen(const char * filename, const char *mode);
#define fopen(filename, mode) __fopen(filename, mode)
#endif

#ifdef __cplusplus
}
#endif
#endif /* __ADI_PLATFORM_TYPES_H__*/
