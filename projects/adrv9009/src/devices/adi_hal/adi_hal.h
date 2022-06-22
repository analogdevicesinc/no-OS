/**
 * \file adrv9009/src/devices/adi_hal/adi_hal.h
 * \brief Contains Talise ADI HAL function prototypes type definitions for adi_hal.c
 *
 * Copyright 2015-2017 Analog Devices Inc.
 * Released under the AD9378-AD9379 API license, for more information see the "LICENSE.txt" file in this zip file.
 */

#ifndef _ADI_HAL_H_
#define _ADI_HAL_H_

/* include standard types and definitions */
#include <stdint.h>
#include <stddef.h>
#include "no_os_util.h"

#define u16 			uint16_t
#define DIV_U64(x, y) no_os_div_u64(x, y)
#define DIV_S64(x, y) no_os_div_s64(x, y)
#define DIV_ROUND_CLOSEST	NO_OS_DIV_ROUND_CLOSEST
#define DIV_ROUND_CLOSEST_ULL	NO_OS_DIV_ROUND_CLOSEST_ULL

/*========================================
 * Enums and structures
 *=======================================*/

struct adi_hal {
	struct no_os_gpio_desc	*gpio_adrv_resetb;
	struct no_os_gpio_desc	*gpio_adrv_sysref_req;
	struct no_os_spi_desc		*spi_adrv_desc;
	uint32_t		log_level;
	void 			*extra_spi;
	uint8_t			spi_adrv_csn;
	void 			*extra_gpio;
	uint8_t			gpio_adrv_resetb_num;
};

/**
 *  \brief Enum of possible Errors Detected by HAL layer to be communicated
 *         to ADI APIs.
 */
typedef enum {
	ADIHAL_OK = 0,        /*!< HAL function successful. No error Detected */
	ADIHAL_SPI_FAIL,      /*!< HAL SPI operation failure. SPI controller Down */
	ADIHAL_GPIO_FAIL,     /*!< HAL GPIO function Failure */
	ADIHAL_TIMER_FAIL,    /*!< HAL Timer function Failure */
	ADIHAL_WAIT_TIMEOUT,  /*!< HAL function Timeout */
	ADIHAL_GEN_SW,        /*!< HAL function failed due to general invalid  HAL data*/
	ADIHAL_WARNING,       /*!< HAL function warning that non critical error was detected*/
	ADIHAL_ERR
} adiHalErr_t;

/**
 *  \brief An enumerated type in bit mask format to list the log message
 *         categories or groups.
 */
typedef enum {
	ADIHAL_LOG_NONE = 0x0,  /*!< HAL Log enum to represent all types of log messages not selected */
	ADIHAL_LOG_MSG  = 0x1,  /*!< HAL Log enum to represent a log message type*/
	ADIHAL_LOG_WARN = 0x2,  /*!< HAL Log enum to represent a warning message type*/
	ADIHAL_LOG_ERR  = 0x4,  /*!< HAL Log enum to represent a error message type*/
	ADIHAL_LOG_SPI  = 0x8,  /*!< HAL Log enum to represent a spi transaction type*/
	ADIHAL_LOG_ALL  = 0xF   /*!< HAL Log enum to represent all types of log messages selected */

} adiLogLevel_t;

typedef enum {
	SYSREF_CONT_ON,		/*!< Turn on continuous SYSREF request */
	SYSREF_CONT_OFF,	/*!< Turn off continuous SYSREF request */
	SYSREF_PULSE,		/*!< SYSREF request in N-shot mode */
} sysrefReqMode_t;

/*========================================
 * Prototypes
 *=======================================*/
#ifdef __cplusplus
extern "C" {
#endif

/* 3 Bytes per SPI transaction * 341 transactions = ~1024 byte buffer size */
/* Minimum HAL_SPIWRITEARRAY_BUFFERSIZE = 18 */
#define HAL_SPIWRITEARRAY_BUFFERSIZE 341

/*============================================================================
 * ADI Device Hardware Control Functions
 *===========================================================================*/

/**
 * \brief Performs a platform hardware initialization for the  ADI Device
 *
 * This function shall initialize all external hardware resources required by
 * ADI Device for correct functionality such as SPI drivers, GPIOs,
 * clocks (if necessary), as per the target platform and target ADI Device.
 * At minimum any SPI driver for the ADI Device must be initialized here in order
 * for the SPI writes within the API to function.
 *
 * The API based on the required operation shall set the value timeout parameter.
 * Once this value is set it shall be the timeout requirement for all HAL operations.
 * It is the responsibility of the HAL implementation to maintain this
 * timeout value as a reference. The HAL implementation must use this value
 * to ensure the HAL function do not block longer than this time interval.
 *
 * Returns an error of type adiHalErr_t. Error returned will depend on platform
 * specific implementation. API expects ADIHAL_OK if function completed successfully.
 * Any other value represents an error or warning to the API. Error return list
 * below provides possible errors from customer implementation.
 *
 * \pre This API should be called before calling any other ADIHAL functions.
 *
 * <B>Dependencies</B>
 * --Application and Platform Specific modules defined by devHalInfo
 *
 * \param devHalInfo A void pointer to the targeted device state container.
 *                   The Platform HAL defined structure containing
 *                   hardware settings describing the device of interest.
 *
 * \param halTimeout_ms A positive integer value to set the HAL function timeout
 *                     interval.
 *
 * \retval ADIHAL_OK if function completed successfully.
 * \retval ADIHAL_SPI_FAIL if function failed to open SPI driver for the device
 * \retval ADIHAL_GPIO_FAIL, if function failed to access GPIO resources
 * \retval ADIHAL_TIMER_FAIL, if function failed to access timer resources
 * \retval ADIHAL_GEN_SW if device references is unknown.
 * \retval ADIHAL_WARNING if function failed to access non-critical resource eg logging
 */
adiHalErr_t ADIHAL_openHw(void *devHalInfo, uint32_t halTimeout_ms);

/**
 * \brief Closes any platform hardware drivers, handles, etc that were necessary
 *        for the ADI Device.
 *
 * This function shall shutdown all external hardware resources required by
 * ADI Device for correct functionality such as SPI drivers, GPIOs, clocks as
 * as per the targeted platform and target ADI Device.
 * It should close and free any resources assigned in ADIHAL_openHw(void *devHalInfo)
 *
 * Returns an error of type adiHalErr_t. Error returned will depend on platform
 * specific implementation. API expects ADIHAL_OK if function completed successfully.
 * Any other value represents an error or warning to the API. Error return list
 * below provides possible errors from customer implementation.
 *
 * \pre This function is called last to terminate the use of device instance and its
 * hardware resources. ADIHAL_openHw for the same device instance prior to calling
 * ADIHAL_closeHw
 *
 * <B>Dependencies</B>
 * --Application and Platform Specific modules defined by devHalInfo
 *
 * \param devHalInfo Pointer to Platform HAL defined structure containing
 *                   hardware settings describing the device of interest.
 *
 * \retval ADIHAL_OK if function completed successfully.
 * \retval ADIHAL_SPI_FAIL if function failed to close SPI driver for the device
 * \retval ADIHAL_GPIO_FAIL, if function failed to release GPIO resources
 * \retval ADIHAL_GEN_SW if device references is unknown.
 * \retval ADIHAL_WARNING if function failed to access non-critical resource eg logging
 */
adiHalErr_t ADIHAL_closeHw(void *devHalInfo);


/**
 * \brief Sets the a timeout duration for the HAL functions.
 *
 * This function shall set the timeout duration for the HAL functions. If the
 * HAL operation exceed this time the function shall return with the
 * ADIHAL_WAIT_TIMEOUT error. This value shall be set by the API based on the
 * desired required operation. Once this value is set it shall be the timeout
 * requirement for all HAL operations.
 *
 * Returns an error of type adiHalErr_t. Error returned will depend on platform
 * specific implementation. API expects ADIHAL_OK if function completed successfully.
 * Any other value represents an error or warning to the API. Error return list
 * below provides possible errors from customer implementation.
 *
 * \pre  This function can be called at anytime to change the timeout duration
 * for the HAL functions.  However, it is typically used during initialization.
 *
 * <B>Dependencies</B>
 *  --Application and Platform Specific modules defined by devHalInfo
 *
 * \param devHalInfo Pointer to Platform HAL defined structure containing
 *                   hardware settings describing the device of interest.
 * \param halTimeout_ms A positive integer value to set the HAL function timeout
 *                      interval.
 *
 * \retval ADIHAL_OK if function completed successfully.
 * \retval ADIHAL_GEN_SW if device references is unknown. eg devHalInfo
 */
adiHalErr_t ADIHAL_setTimeout(void *devHalInfo, uint32_t halTimeout_ms);

/**
 * \brief Performs a hardware reset on the ADI Device
 *
 * ADI Devices have a RESETB pin. This function shall toggle the appropriate
 * ADI Device RESETB pin according to the specifications outlined in the ADI
 * Device Data Sheet for the targeted Device.
 *
 * ADI does not define the structure members in devHalInfo.  The user
 * developing the platform layer of code is required to reset the correct
 * device based on some identifying information within their devHalInfo
 * structure such as a spi chip select or other device id in devHalInfo.
 *
 * Returns an error of type adiHalErr_t. Error returned will depend on platform
 * specific implementation. API expects ADIHAL_OK if function completed successfully.
 * Any other value represents an error or warning to the API. Error return list
 * below provides possible errors from customer implementation.
 *
 * \pre This function may be called at any time to reset the device after the required
 * GPIO drivers and resources are opened by the ADIHAL_openHw function call.
 * and not after ADIHAL_closeHW.
 *
 *
 * <B>Dependencies</B>
 * --Application and Platform Specific modules defined by devHalInfo
 *
 *
 * \param devHalInfo Pointer to Platform HAL defined structure containing
 *        hardware settings describing the device of interest and required
 *        hardware references to toggle RESETB pin of the ADI device.
 *
 *
 * \retval ADIHAL_OK if function completed successfully.
 * \retval ADIHAL_GEN_SW if device references is unknown.
 * \retval ADIHAL_GPIO_FAIL if GPIO hardware to toggle ADI reset pin unavailable
 * \retval ADIHAL_WAIT_TIMEOUT if HAL timeout expired before reset pin toggle could be completed.
 * \retval ADIHAL_WARNING if function failed to access non-critical resource eg logging
 */
adiHalErr_t ADIHAL_resetHw(void *devHalInfo);

/**
 * \brief Performs a SYSREF request to the clock generation device.
 *
 * \param devHalInfo Pointer to Platform HAL defined structure containing
 *        hardware settings describing the device of interest and required
 *        hardware references to toggle SYSREF pin of the ADI device.
 *
 * \param mode SYSREF request mode.
 *
 * \pre This function may be called at any time to issue a SYSREF request
 *  after the required GPIO drivers and resources are opened by the
 * ADIHAL_openHw function call and not after ADIHAL_closeHW.
 *
 *
 * <B>Dependencies</B>
 * --Application and Platform Specific modules defined by devHalInfo
 *
 *
 * \retval ADIHAL_OK if function completed successfully.
 * \retval ADIHAL_ERROR if wrong mode is provided.
 */
adiHalErr_t ADIHAL_sysrefReq(void *devHalInfo, sysrefReqMode_t mode);

/**
 * \brief Performs a Single SPI write to an ADI Device
 *
 * This function shall perform a single SPI write to an ADI Device. The SPI
 * write implementation must support 15 bit addressing and 8-bit data bytes.
 *
 * Returns an error of type adiHalErr_t. Error returned will depend on platform
 * specific implementation. API expects ADIHAL_OK if function completed successfully.
 * Any other value represents an error or warning to the API. Error return list
 * below provides possible errors from customer implementation.
 *
 * \pre This function may only be used after the required SPI drivers and resources
 * are opened by the ADIHAL_openHw() function call and not after ADIHAL_closeHW.
 *
 * <B>Dependencies</B>
 * --Application and Platform Specific modules
 *
 * \param devHalInfo Pointer to Platform HAL defined structure containing
 *                   hardware settings describing the device of interest.
 *
 * \param addr 15-bit SPI register Address (MSB bit in lower layers sets
 *             read/write bit)
 *
 * \param data 8-bit data value to write to the SPI address specified.
 *
 * \retval ADIHAL_OK if function completed successfully.
 * \retval ADIHAL_GEN_SW if device references is unknown.
 * \retval ADIHAL_SPI_FAIL if function failed to complete SPI transaction
 * \retval ADIHAL_WAIT_TIMEOUT if HAL timeout expired before SPI transaction could be completed.
 * \retval ADIHAL_WARNING if function failed to access non-critical resource eg logging
 */
adiHalErr_t ADIHAL_spiWriteByte(void *devHalInfo, uint16_t addr, uint8_t data);

/**
 * \brief Writes an array of SPI writes to an ADI Device
 *
 * This function allows improved performance to write large sets of SPI
 * registers to a single device by passing the SPI Write address and data to
 * the platform driver in an array instead of many individual calls.
 *
 * If the platform layer SPI driver has no way to write an array to the SPI
 * driver, have this function call ADIHAL_spiWriteByte in a for loop.
 *
 * Each address element corresponds to the same index element in the data
 * array.  addr[0] is the SPI address for data[0], addr[1] is the SPI addr
 * for data[1], etc.
 *
 * Returns an error of type adiHalErr_t. Error returned will depend on platform
 * specific implementation. API expects ADIHAL_OK if function completed successfully.
 * Any other value represents an error or warning to the API. Error return list
 * below provides possible errors from customer implementation.
 *
 * \pre This function may only be used after the required SPI drivers and resources
 * are opened by the ADIHAL_openHw() function call and not after ADIHAL_closeHW.
 *
 * <B>Dependencies</B>
 * --Application and Platform Specific modules
 *
 * \param devHalInfo Pointer to Platform HAL defined structure containing
 *                   hardware settings describing the device of interest.
 *
 * \param addr An array of 15-bit addresses of SPI register to write.
 *             15-bit SPI register Addresses (MSB bit in lower layers sets
 *             read/write bit)
 *
 * \param data An array of 8-bit data values to write to the SPI addresses in
 *             the addr array.
 *
 * \param count The number of SPI Writes to transfer. Must be smaller or equal
 *              to the size of the addr and data arrays.
 *
 * \retval ADIHAL_OK if function completed successfully.
 * \retval ADIHAL_GEN_SW if device references is unknown.
 * \retval ADIHAL_SPI_FAIL if function failed to complete SPI transaction
 * \retval ADIHAL_WAIT_TIMEOUT if HAL timeout expired before SPI transaction could be completed.
 * \retval ADIHAL_WARNING if function failed to access non-critical resource eg logging
 */
adiHalErr_t  ADIHAL_spiWriteBytes(void *devHalInfo, uint16_t *addr,
				  uint8_t *data, uint32_t count);

/**
 * \brief Performs a Single SPI Read from an ADI Device
 *
 * This function shall perform a single SPI read from an ADI Device. The SPI
 * read implementation must support 16 bit addressing and 8-bit data bytes.
 *
 * Returns an error of type adiHalErr_t. Error returned will depend on platform
 * specific implementation. API expects ADIHAL_OK if function completed successfully.
 * Any other value represents an error or warning to the API. Error return list
 * below provides possible errors from customer implementation.
 *
 * \pre This function may only be used after the required SPI drivers and resources
 * are opened by the ADIHAL_openHw() function call and not after ADIHAL_closeHW.
 *
 * <B>Dependencies</B>
 * --Application and Platform Specific modules
 *
 * \param devHalInfo Pointer to Platform HAL defined structure containing
 *                   hardware settings describing the device of interest.
 *
 * \param addr The 16-bit address to read from the SPI device. Actual address
 *             can not exceed 15-bits since MSB bit is used for Read/Write bit
 *             in platform layer.
 *
 * \param readdata The 8-bit data value returned from SPI register addr
 *
 * \retval ADIHAL_OK if function completed successfully.
 * \retval ADIHAL_GEN_SW if device references is unknown.
 * \retval ADIHAL_SPI_FAIL if function failed to complete SPI transaction
 * \retval ADIHAL_WAIT_TIMEOUT if HAL timeout expired before SPI transaction could be completed.
 * \retval ADIHAL_WARNING if function failed to access non-critical resource eg logging
 */
adiHalErr_t ADIHAL_spiReadByte(void *devHalInfo, uint16_t addr,
			       uint8_t *readdata);

/**
 * \brief Performs a Multi SPI Read from an ADI Device
 *
 * This function shall perform multi SPI read from an ADI Device. The SPI
 * read implementation must support 16 bit addressing and 8-bit data bytes.
 *
 * Returns an error of type adiHalErr_t. Error returned will depend on platform
 * specific implementation. API expects ADIHAL_OK if function completed successfully.
 * Any other value represents an error or warning to the API. Error return list
 * below provides possible errors from customer implementation.
 *
 * \pre This function may only be used after the required SPI drivers and resources
 * are opened by the ADIHAL_openHw() function call and not after ADIHAL_closeHW.
 *
 * <B>Dependencies</B>
 * --Application and Platform Specific modules
 *
 * \param devHalInfo Pointer to Platform HAL defined structure containing
 *                   hardware settings describing the device of interest.
 *
 * \param *addr An array of 15-bit addresses of SPI registers to read from.
 *             15-bit SPI register Addresses (MSB bit in lower layers sets
 *             read/write bit)
 *
 * \param *readdata The 8-bit data value returned from SPI register addr
 *
 * \param  count The number of SPI Reads to transfer. Must be  equal
 *              to the size of the addr and data arrays.
 *
 * \retval ADIHAL_OK if function completed successfully.
 * \retval ADIHAL_GEN_SW if device references is unknown.
 * \retval ADIHAL_SPI_FAIL if function failed to complete SPI transaction
 * \retval ADIHAL_WAIT_TIMEOUT if HAL timeout expired before SPI transaction could be completed.
 * \retval ADIHAL_WARNING if function failed to access non-critical resource eg logging
 */
adiHalErr_t ADIHAL_spiReadBytes(void *devHalInfo, uint16_t *addr,
				uint8_t *readdata, uint32_t count);

/**
 * \brief Performs a write to the specified field in a SPI register.
 *
 * This function shall perform a Read/Modify/Write of a single SPI read from an
 * bits in the register, and writing the value back to the SPI register.
 * ADI Device. The mask and startBit parameters allow modifying some or all
 *
 * The mask is expected to be applied to the complete 8-bit read back SPI
 * register value.  The startBit parameter is used to shift the fieldVal
 * parameter to the correct starting bit in the SPI register.
 *
 * Desired Operation:
 * readVal = Read 8-bit SPI register value
 *
 * Modify
 *   Clear bits in readVal for enabled mask bits (readVal & ~mask)
 *   Set bits in field based on mask and startBit ((fieldVal << startBit) & mask)
 *
 * Write the modified value back to the SPI register
 *
 * Returns an error of type adiHalErr_t. Error returned will depend on platform
 * specific implementation. API expects ADIHAL_OK if function completed successfully.
 * Any other value represents an error or warning to the API. Error return list
 * below provides possible errors from customer implementation.
 *
 * \pre  This function may only be used after the required SPI drivers and resources
 * are opened by the ADIHAL_openHw() function call and not after ADIHAL_closeHW.
 *
 * <B>Dependencies</B>
 * --Application and Platform Specific modules
 *
 * \param devHalInfo Pointer to Platform HAL defined structure containing
 *                   hardware settings describing the device of interest.
 *
 * \param addr The 16-bit address to read/modify/write from the SPI device.
 *             Actual address can not exceed 15-bits since MSB bit is used for
 *             Read/Write bit in platform layer.
 *
 * \param fieldVal 8-bit value to update in a bitfield (subset) of the SPI register
 *
 * \param mask 8-bit mask of bits to modify (if a bit = 1, that bit can be modified)
 * \param startBit starting bit of the fieldVal in the SPI reg.  Bit shifts
 *                 the fieldVal up to that bit before masking and modifying the SPI
 *                 reg.
 *
 * \retval ADIHAL_OK if function completed successfully.
 * \retval ADIHAL_GEN_SW if device references is unknown.
 * \retval ADIHAL_SPI_FAIL if function failed to complete SPI transaction
 * \retval ADIHAL_WAIT_TIMEOUT if HAL timeout expired before SPI transaction could be completed.
 * \retval ADIHAL_WARNING if function failed to access non-critical resource eg logging
 */
adiHalErr_t ADIHAL_spiWriteField(void *devHalInfo, uint16_t addr,
				 uint8_t  fieldVal, uint8_t mask, uint8_t startBit);

/**
 * \brief Performs a read from a particular bit field in a SPI register.
 *
 * This function shall perform a single SPI read from an ADI Device. Before
 * the SPI read value is returned, it is masked and bit-shifted to return
 * only a desired subset of bits - the desired bit field.
 *
 * Any necessary SPI drivers or resources are expected to have already been
 * opened by the ADIHAL_openHw() function call.
 *
 * The mask parameter is applied to the entire 8-bit SPI read back value. The
 * startBit parameter shifts the desired subset of bits down to the zero bit
 * position.
 *
 * Desired Operation:
 * readVal = SPI read of a 8-bit SPI register
 * return (readVal & mask) >> startBit
 *
 * Returns an error of type adiHalErr_t. Error returned will depend on platform
 * specific implementation. API expects ADIHAL_OK if function completed successfully.
 * Any other value represents an error or warning to the API. Error return list
 * below provides possible errors from customer implementation.
 *
 * \pre This function may only be used after the required SPI drivers and resources
 * are opened by the ADIHAL_openHw() function call and not after ADIHAL_closeHW.
 *
 * <B>Dependencies</B>
 * --Application and Platform Specific modules
 *
 * \param devHalInfo Pointer to Platform HAL defined structure containing
 *                   hardware settings describing the device of interest.
 *
 * \param addr The 16-bit address to read/modify/write from the SPI device.
 *             Actual address can not exceed 15-bits since MSB bit is used for
 *             Read/Write bit in platform layer.
 *
 * \param fieldVal 8-bit value: Readback value of a bitfield (subset) of the
 *                 SPI register masked and shifted down to the bit 0 position
 *
 * \param mask 8-bit mask of bits to readback (if a bit = 1, that bit can be read)
 * \param startBit starting LSB bit of the fieldVal in the SPI reg.  Bit shifts
 *                 the fieldVal down to that bit before returning in fieldVal
 *
 * \retval ADIHAL_OK if function completed successfully.
 * \retval ADIHAL_GEN_SW if device references is unknown.
 * \retval ADIHAL_SPI_FAIL if function failed to complete SPI transaction
 * \retval ADIHAL_WAIT_TIMEOUT if HAL timeout expired before SPI transaction could be completed.
 * \retval ADIHAL_WARNING if function failed to access non-critical resource eg logging
 */
adiHalErr_t ADIHAL_spiReadField(void *devHalInfo, uint16_t addr,
				uint8_t *fieldVal, uint8_t mask, uint8_t startBit);

/**
 * \brief Delay or sleep for the specified number of microseconds.
 *
 * Performs a thread blocking/sleeping delay of the specified time in us.
 * Function should wait at least for the specified amount of time.
 *
 * Returns an error of type adiHalErr_t. Error returned will depend on platform
 * specific implementation. API expects ADIHAL_OK if function completed successfully.
 * Any other value represents an error or warning to the API. Error return list
 * below provides possible errors from customer implementation.
 *
 * \pre This function can be used at any time to create a delay
 *
 * <B>Dependencies</B>
 * --Application and Platform Specific modules
 *
 *
 * \param devHalInfo Pointer to Platform HAL defined structure containing
 *                   hardware settings describing the device of interest.
 *
 * \param time_us The amount of time in microseconds to block - thread sleep for
 *
 * \retval ADIHAL_OK if function completed successfully.
 * \retval ADIHAL_GEN_SW if device references is unknown.
 * \retval ADIHAL_TIMER_FAIL if function delay function failed to complete
 * \retval ADIHAL_WARNING if function failed to access non-critical resource eg logging
 *
 */
adiHalErr_t ADIHAL_wait_us(void *devHalInfo, uint32_t time_us);

/*========================================
 * Optional Logging Functions
 *
 *=======================================*/
/**
 * \brief Writes a character array to a log file to facilitate debugging
 *
 * This function uses the logLevel parameter to specify what type of
 * message / warning / error to write to the log file. This function
 * uses enums in adiLogLevel_t to set the log level.
 * ADIHAL_LOG_NONE = 0x0,
 * ADIHAL_LOG_MSG  = 0x1,
 * ADIHAL_LOG_WARN = 0x2,
 * ADIHAL_LOG_ERR  = 0x4,
 * ADIHAL_LOG_SPI  = 0x8,
 * ADIHAL_LOG_ALL  = 0xF
 *
 * The user implementation may choose to allow a feature to filter
 * the level of message to log.  This could allow only logging errors
 * for instance.
 *
 * Returns an error of type adiHalErr_t. Error returned will depend on platform
 * specific implementation. API expects ADIHAL_OK if function completed successfully.
 * Any other value represents an error or warning to the API. Error return list
 * below provides possible errors from customer implementation.
 *
 * \pre This function should be used when setting up and configuring the system
 *
 * <B>Dependencies</B>
 * --Application and Platform Specific modules
 *
 * \param devHalInfo Pointer to Platform HAL defined structure containing
 *                   hardware settings describing the device of interest.
 *
 * \param halLogLevel The type of message to log.
 *
 * \retval ADIHAL_OK if function completed successfully.
 * \retval ADIHAL_GEN_SW if device references is unknown.
 */
adiHalErr_t ADIHAL_setLogLevel(void *devHalInfo, uint16_t halLogLevel);

/**
 * \brief Writes a character array to a log file to facilitate debugging
 *
 * This function uses the logLevel parameter to specify what type of
 * message / warning / error to write to the log file. This function
 * uses enums in adiLogLevel_t to set the log level.
 *
 * The user implementation may choose to allow a feature to filter
 * the level of message to log.  This could allow only logging errors
 * for instance.
 *
 * Returns an error of type adiHalErr_t. Error returned will depend on platform
 * specific implementation. API expects ADIHAL_OK if function completed successfully.
 * Any other value represents an error or warning to the API. Error return list
 * below provides possible errors from customer implementation.
 *
 * \pre This function can be used at anytime once the required logging resources
 * are opened by the ADIHAL_openHw() function call and not after ADIHAL_closeHW.
 * The log level should also be set prior to calling this function via
 * ADIHAL_setLogLevel.
 *
 * <B>Dependencies</B>
 * --Application and Platform Specific modules
 *
 * \param devHalInfo Pointer to Platform HAL defined structure containing
 *                   hardware settings describing the device of interest.
 *
 * \param logLevel The type of message to log.  ADIHAL_LOG_MSG,
 *        ADIHAL_LOG_WARN, ADIHAL_LOG_ERR, ADIHAL_LOG_SPI,
 *
 * \param errorCode An error code value to designate a particular error
 * \param comment The character array describing the error
 *
 * \retval ADIHAL_OK if function completed successfully.
 * \retval ADIHAL_GEN_SW if device references is unknown.
 * \retval ADIHAL_WARNING if log message cannot be completed
 */
adiHalErr_t ADIHAL_writeToLog(void *devHalInfo, adiLogLevel_t logLevel,
			      uint32_t errorCode, const char *comment);


#ifdef __cplusplus
}
#endif
#endif
