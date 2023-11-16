/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
*   \file   adi_platform.h
* 
*   \brief  Declarations for Generic ADI Platform
*
*   ADRV904X API Version: 2.9.0.4
*
*   \note   Implementation required to guarantee ADI Device functionality
*
*           WARNING: Declarations in this file MUST NOT be modified
*
*           Set of Global Function Pointers Requiring Definition & Initialization for use with ADI Devices
*
*           HAL Configurations are Opaque to ADI Devices (i.e. using void* for devHalCfg)
*               adi_hal_Interfaces_e - Bit Mask Abstraction for defining interfaces for a HAL Configuration
*
*           One HAL Configuration Required per Device (i.e. ADI Device Structure includes HAL Device Pointer)
*
*           Basic Sequence:
*
*           1) adi_hal_DevHalCfgCreate()    -> Interfaces Defined are Hardware/Feature Specific
*           2) adi_hal_HwOpen()             -> Enable all Selected HAL Interfaces & Features
*           3) Use Device                   -> Hardware Ready for Use
*           4) adi_hal_HwClose()            -> Release all Resources, i.e. Clean-up Task
*           5) adi_hal_DevHalCfgFree()      -> Free Memory, i.e. Clean-up Task
*
*           Logging Feature is per HAL Configuration (i.e. Each ADI Device has its own log file)
*
*           HAL API's include multi-threaded applications support
*               Single Threaded Applications must stub these API's (i.e. return success)
*/

#ifndef __ADI_PLATFORM_H__
#define __ADI_PLATFORM_H__

#ifdef __GNUC__                    /* __unix__ verify if our linux image declare this */
  #define OS_Windows 0
#else     /* windows  */

#if _WIN64 == 0      /* _Win32  */
  #define OS_Windows 32
#elif _WIN64 == 1      /* _Win64  */
  #define OS_Windows 64
#endif
#endif

#ifndef ADI_API
  #ifdef __cplusplus
    #define ADI_API extern "C"
  #else
    #define ADI_API
  #endif
#endif
#ifndef ADI_API_EX
  #ifdef __cplusplus
    #define ADI_API_EX ADI_API
  #else
    #define ADI_API_EX ADI_API extern
  #endif
#endif

/* ADI_HAL_MAX_THREADS defines the maximum number of threads that can simultaneously use the API. Effectively
 * it is the maximum number of threads that can have an adi_common_ErrData_t registered (using adi_hal_TlsSet)
 * at one time. */
#ifndef ADI_HAL_MAX_THREADS
#define ADI_HAL_MAX_THREADS 1U
#endif

#if ADI_HAL_MAX_THREADS <= 0
#error "ADI_HAL_MAX_THREADS defined as <=0. It must be defined to be >= 1 or omitted in which case it defaults to 1."
#endif

#include "adi_platform_types.h"
#include "adi_platform_impl.h"

/**
 * BBIC Init, Open, Close functions
 */

/**
 * \brief Returns a handle to a HAL configuration with the specified interfaces enabled and based
 * on the supplied parameters. Several HAL configurations may be created independently. A HAL
 * configuration must be obtained via this function and must be supplied as the \p devHalCfg
 * parameter to all other HAL API functions.
 *
 * \post After creation the returned devHalCfg needs to be opened with adi_hal_HwOpen before being
 * used.
 *
 * \post To prevent resource leakage all HAL configurations must eventually be freed using
 *  adi_hal_DevHalCfgFree.
 *
 * \retval void* - Pointer to Object Containing HAL Configuration
 */
ADI_API_EX void* (*adi_hal_DevHalCfgCreate)(const uint32_t interfaceMask, const uint8_t spiChipSelect, const char* const logFilename);

/**
 * \brief Prepares the HAL interfaces associated with the devHalCfg handle for use.
 *
 * This function must be called before any of the other HAL functions are called for the
 * same \param devHalCfg. This function must be called only once for each value of \param devHalCfg.
 *
 * Applications do not need to call this function directly. Opening is done by assigning
 * the devHalCfg to an ADI device and opening that device with a device-specific HwOpen
 * call (e.g. adi_adrv904x_HwOpen).
 *
 * \pre The value of \param devHalCfg has been previously returned from a call to
 * adi_hal_DevHalCfgCreate.
 *
 * \retval adi_hal_Err_e - ADI_HAL_ERR_OK if successful
 */
ADI_API_EX adi_hal_Err_e (*adi_hal_HwOpen)(void* const devHalCfg);

/**
 * \brief Notifies the HAL that the configuration is no longer required and that any resources
 * acquired for that configuration may be released. Must be called before adi_hal_DevHalCfgFree.
 *
 * Applications do not need to call this function directly. Closing is done by closing the ADI
 * device to which the devHalCfg was assigned. i.e by calling a device-specific HwClose function
 * (e.g. adi_adrv904x_HwClose).
 *
 * \pre The value of \param devHalCfg has been previously returned from a call to
 * adi_hal_DevHalCfgCreate.
 *
 * \pre The value of \param devHalCfg has been opened with adi_hal_HwOpen.
 *
 * \post After this call the configuration must be freed using adi_hal_DevHalCfgFree.
 * \post After this call no function except adi_hal_DevHalCfgFree may be called in relation to this
 * devHalCfg. The devHalCfg may not be re-opened.
 *
 * \retval adi_hal_Err_e - ADI_HAL_ERR_OK if successful
 */
ADI_API_EX adi_hal_Err_e (*adi_hal_HwClose)(void* const devHalCfg);

/**
 * \brief Performs a reset (which may be a physical power-on reset or power-cycle) of the device
 * addressed by the HAL configuration's SPI interface.
 *
 * \pre The value of \param devHalCfg has been previously returned from a call to
 * adi_hal_DevHalCfgCreate.
 *
 * \retval adi_hal_Err_e - ADI_HAL_ERR_OK if successful
 */
ADI_API_EX adi_hal_Err_e (*adi_hal_HwReset)(void* const devHalCfg, const uint8_t pinLevel);

/**
 * \brief Indicate that the client is finished with a HAL configuration itself. The configuration
 * can only be freed after first being closed with adi_hal_HwClose. After this function the
 * supplied HAL configuration handle may no longer be used with any HAL API function.
 *
 * \pre The value of \param devHalCfg has been previously returned from a call to
 * adi_hal_DevHalCfgCreate.
 *
 * \pre The \param devHalCfg has been closed with adi_hal_HwClose.
 *
 * \retval adi_hal_Err_e - ADI_HAL_ERR_OK if successful
 */
ADI_API_EX adi_hal_Err_e (*adi_hal_DevHalCfgFree)(void* devHalCfg);

/**
 * \brief Function to write the the specified data to the device I2C port
 *
 * If numTxBytes is 0, no transaction is to take place and function is to
 * return ADI_HAL_ERR_OK.
 *
 * \param devHalCfg Pointer to device instance specific platform settings
 * \param txData Byte array of data to write to the I2C device.  First byte
*               should be the I2C register address followed by one or more data bytes.
 * \param numTxBytes Number of bytes in the txData array
 *
 * \retval adi_hal_Err_e - ADI_HAL_ERR_OK if successful
 */
ADI_API_EX adi_hal_Err_e(*adi_hal_I2cWrite)(void* const devHalCfg, const uint8_t txData[], const uint32_t numTxBytes);

/**
 * \brief Function to read data from the device via I2C port
 *
 * If numRxBytes is 0, no transaction is to take place and function is to
 * return ADI_HAL_ERR_OK.
 *
 * \param devHalCfg Pointer to device instance specific platform settings
 * \param txData Byte array of data to write to the I2C device. Depending on the
*               I2C device, this might just be 1 byte containing the register
*               address to read
 * \param numTxBytes Number of bytes in the txData array
 * \param rxData Byte array to return the read back data
 * \param numRxBytes Number of bytes to read back, and size of rxData array
 *
 * \retval adi_hal_Err_e - ADI_HAL_ERR_OK if successful
 */
ADI_API_EX adi_hal_Err_e(*adi_hal_I2cRead)(void* const devHalCfg, const uint8_t txData[], const uint32_t numTxBytes,
                                           uint8_t rxData[], const uint32_t numRxBytes);

/**
 * \brief Function to write the the specified data to the device SPI port
 *
 * If numTxBytes is 0, no transaction is to take place and function is to
 * return ADI_HAL_ERR_OK.
 *
 * \param devHalCfg Pointer to device instance specific platform settings
 * \param txData Array of data to be written to the device
 * \param numTxBytes Size of txData 
 *
 * \retval adi_hal_Err_e - ADI_HAL_ERR_OK if successful
 */
ADI_API_EX adi_hal_Err_e (*adi_hal_SpiWrite)(void* const devHalCfg, const uint8_t txData[], const uint32_t numTxBytes);

/**
 * \brief Function to read data from the device via SPI port
 *
 * Input arrays will be the same size to support full duplex SPI drivers.
 * 
 * If numRxBytes is 0, no transaction is to take place and function is to
 * return ADI_HAL_ERR_OK.
 *
 * \param devHalCfg Pointer to device instance specific platform settings
 * \param txData Array of bytes to write to the device
 * \param rxData Array of bytes read from the device
 * \param numRxBytes Size of txData and rxData arrays. Arrays must be of
 * the same size.
 *
 * \retval adi_hal_Err_e - ADI_HAL_ERR_OK if successful
 */
ADI_API_EX adi_hal_Err_e (*adi_hal_SpiRead)(void* const     devHalCfg,
                                            const uint8_t   txData[],
                                            uint8_t         rxData[],
                                            const uint32_t  numRxBytes);

/**
* \brief Service to get Logging Status
*
* \param devHalCfg  Pointer to device instance specific platform settings
* \param logStatus  Pointer to Structure that will be populated with Log Status
*
* \note Error Count & Flags are reset after each Status Call
*
* \retval adi_hal_Err_e - ADI_HAL_ERR_OK if successful
*/
ADI_API_EX adi_hal_Err_e (*adi_hal_LogStatusGet)(void* const devHalCfg, adi_hal_LogStatusGet_t* const logStatus);

/**
* \brief Opens a logFile. If the file is already open it will be closed and reopened.
*
* This function opens the file for writing and saves the resulting file 
* descriptor to the devHalCfg structure.
*
* \param devHalCfg Pointer to device instance specific platform settings
* \param filename The user provided name of the file to open.
*
* \retval adi_hal_Err_e - ADI_HAL_ERR_OK if successful
*
*/
ADI_API_EX adi_hal_Err_e (*adi_hal_LogFileOpen)(void* const devHalCfg, const char* const filename);

/**
* \brief Service to Close a Log File
*
* \param devHalCfg Pointer to device instance specific platform settings
*
* \retval adi_hal_Err_e - ADI_HAL_ERR_OK if successful
*
*/
ADI_API_EX adi_hal_Err_e (*adi_hal_LogFileClose)(void* const devHalCfg);

/**
* \brief Sets the log level, allowing the end user to select the granularity of
*        what events get logged.
*
* \param devHalCfg  Pointer to device instance specific platform settings
* \param logMask    Bitwise Mask of adi_hal_LogLevel_e values to capture messages of a specific type
*
* \retval adi_hal_Err_e - ADI_HAL_ERR_OK if successful
*
*/
ADI_API_EX adi_hal_Err_e (*adi_hal_LogLevelSet)(void* const devHalCfg, const uint32_t logMask);

/**
 * \brief Gets the currently set log level: the mask of different types of log
 *         events that are currently enabled to be logged.
 *
 * \param devHalCfg Pointer to device instance specific platform settings
 * \param logMask   Bitwise Mask of adi_hal_LogLevel_e values to capture messages of a specific type
 *
 * \retval adi_hal_Err_e - ADI_HAL_ERR_OK if successful
 *
 */
ADI_API_EX adi_hal_Err_e (*adi_hal_LogLevelGet)(void* const devHalCfg, uint32_t* const logMask);

/**
 * \brief Service to Set to Logging to Console Flag
 *
 * \param devHalCfg         Pointer to device instance specific platform settings
 * \param logConsoleFlag    Log to Console Flag Setting
 *
 * \retval adi_hal_Err_e - ADI_HAL_ERR_OK if successful
 *
 */
ADI_API_EX adi_hal_Err_e (*adi_hal_LogConsoleSet)(void* const devHalCfg, const adi_hal_LogConsole_e logConsoleFlag);


/**
* \brief Writes a message to the currently open logFile specified in the 
*        adi_hal_LogCfg_t of the devHalCfg structure passed
* 
* Uses the vfprintf functionality to allow the user to supply the format and
* the number of arguments that will be logged.
*
* \param devHalCfg  Pointer to device instance specific platform settings
* \param logLevel   Specified Logging Level
* \param comment    the string to include in the line added to the log.
* \param argp       variable argument list to be printed
*
* \retval adi_hal_Err_e - ADI_HAL_ERR_OK if successful
*
*/
ADI_API_EX adi_hal_Err_e (*adi_hal_LogWrite)(   void* const                 devHalCfg,
                                                const adi_hal_LogLevel_e    logLevel,
                                                const uint8_t               indent,
                                                const char* const           comment,
                                                va_list                     argp);

/**
* \brief Provides a blocking delay of the current thread
*
* \param devHalCfg Pointer to device instance specific platform settings
* \param time_us the time to delay in micro seconds
*
* \retval ADI_HAL_ERR_OK Function completed successfully
*/
ADI_API_EX adi_hal_Err_e (*adi_hal_Wait_us)(void* const devHalCfg, const uint32_t time_us);

/**
* \brief Provides a blocking delay of the current thread
*
* \param devHalCfg Pointer to device instance specific platform settings
* \param time_ms the Time to delay in milli seconds
*
* \retval ADI_HAL_ERR_OK Function completed successfully
*
*/
ADI_API_EX adi_hal_Err_e (*adi_hal_Wait_ms)(void* const devHalCfg, const uint32_t time_ms);

/**
 * \brief Function to read a single BBIC control register
 *
 * \param devHalCfg Pointer to device instance specific platform settings
 * \param addr 32bit address of BBIC register to read
 * \param data 32bit Pointer to store return value representing the data of the register at the specified address
 *
 * \retval ADI_HAL_ERR_OK Function completed successfully
 */
ADI_API_EX adi_hal_Err_e (*adi_hal_BbicRegisterRead)(void* const devHalCfg, const uint32_t addr, uint32_t* const data);

/**
 * \brief Function to write a single BBIC control register
 *
 * \param devHalCfg Pointer to device instance specific platform settings
 * \param addr      32bit address of BBIC register to write
 * \param data      32bit data to write to the register at the specified address
 *
 * \retval ADI_HAL_ERR_OK Function completed successfully
 */
ADI_API_EX adi_hal_Err_e (*adi_hal_BbicRegisterWrite)(void* const devHalCfg, const uint32_t addr, const uint32_t data);

/**
 * \brief Function to read multiple consecutive BBIC control registers starting at a specified register address.
 *
 * If numDataWords is 0, no transaction is to take place and function is to
 * return ADI_HAL_ERR_OK.
 *
 * \param devHalCfg Pointer to device instance specific platform settings
 * \param addr 32bit address of BBIC register to start reading from
 * \param data 32bit Pointer to store return array representing the data starting at the specified register address
               and ending at (addr + numDataWords -1)
 * \param numDataWords Number of elements in the data array
 *
 * \retval ADI_HAL_ERR_OK Function completed successfully
 */
ADI_API_EX adi_hal_Err_e (*adi_hal_BbicRegistersRead)(  void* const     devHalCfg,
                                                        const uint32_t  addr,
                                                        uint32_t        data[],
                                                        const uint32_t  numDataWords);

/**
 * \brief Function to write multiple consecutive BBIC control registers starting at a specified register address.
 *
 * If numDataWords is 0, no transaction is to take place and function is to
 * return ADI_HAL_ERR_OK.
 *
 * \param devHalCfg Pointer to device instance specific platform settings
 * \param addr 32bit address of BBIC register to start writing to
 * \param data 32bit Pointer to array representing the data to write starting at the specified register address
 *              and ending at (addr + numDataWords -1)
 * \param numDataWords Number of elements in the data array
 *
 * \retval ADI_HAL_ERR_OK Function completed successfully
 */
ADI_API_EX adi_hal_Err_e (*adi_hal_BbicRegistersWrite)( void* const     devHalCfg,
                                                        const uint32_t  addr,
                                                        const uint32_t  data[],
                                                        const uint32_t  numDataWords);

/**
 * \brief Returns the ID of the calling thread.
 *
 * Non-multithreaded HALs must implement this function but may return the same value regardless
 * of the calling thread.
 */
ADI_API_EX adi_hal_thread_t (*adi_hal_ThreadSelf)(void);

/**
 * \brief Before being used a mutex must be initialized with this function. The mutex is
 * initialized into the unlocked state.
 *
 * The HAL mutex API intends to be a subset of the Linux implementation of the POSIX API.
 * Some differences:
 * - There is no HAL_MUTEX_INITIALIZER - adi_hal_mutex_init must be used.
 * - There is no adi_hal_mutexattr_settype - all mutex's are recursive.
 * - adi_hal_mutex_lock blocks indefinitely - there is no adi_hal_trylock nor
 *     pthread_mutex_timedlock.
 * - Any failure of any functions of the API is indicated by returning a non-zero value.
 * - There is no requirement to allow mutex's to be re-initialized after being destroyed.
 * - TODO BOM - refine error handling.
 * - TODO BOM - refine blocking semantics. Do we need to specify 'blocks indefinitely' we could
 *     allow a HAL specific timeout returning something like EAGAIN but need a way to
 *     route this platform specific code back to the API client.
 *
 * If the HAL provided to the API does not support multi-threading the HAL must still
 * define hal_mutex_t and supply implementations of adi_hal_MutexXxx functions returning
 * ADI_HAL_ERR_OK.
 */
ADI_API_EX adi_hal_Err_e (*adi_hal_MutexInit)(adi_hal_mutex_t* const mutex);

/**
 * \brief Acquire the mutex.
 *
 * Will block indefinitely to acquire the mutex. All mutex's are recursive so the same thread can
 * lock the same mutex multiple times but must take care to unlock it the same number of times
 * before another thread can acquire it.
 *
 * Non-multithreaded HALs must implement this function but it can just simply return 0
 */
ADI_API_EX adi_hal_Err_e (*adi_hal_MutexLock)(adi_hal_mutex_t* const mutex);

/**
 * \brief Unlocks the mutex.
 */
ADI_API_EX adi_hal_Err_e (*adi_hal_MutexUnlock)(adi_hal_mutex_t* const mutex);

/**
 * \brief Signal to the HAL that the mutex is no longer required and any associated resources can
 * be released. The mutex must itself must already be released (i.e. has been unlocked as many
 * times as it has been locked). After being destroyed a mutex must not be used or initialized
 * again.
 *
 * Non-multithreaded HALs must implement this function but it can just simply return 0
 */
ADI_API_EX adi_hal_Err_e (*adi_hal_MutexDestroy)(adi_hal_mutex_t* const mutex);

/**
 * \brief Store \param value on behalf of the calling thread and associate it with \param tlsType.
 * To retrieve the value later the same thread calls adi_hal_TlsGet using the same value of
 * \param tlsType.
 *
 * If adi_hal_TlsSet was previously called with a different value the new value overwrites the existing
 * value stored. Until a thread calls adi_hal_TlsSet the value associated with that thread is NULL for
 * all values of \param tlsType.
 *
 * The HAL thread-local storage API intends to be similar to the POSIX pthread_set/get specific API.
 * But only allows a small number of key values which removes the need for a function similar to
 * pthread_key_create() and replaces pthread_key_t with adi_hal_TlsType_e.
 *
 * Implementations must support tlsTypes of HAL_TLS_ERR and may support other values.
 *
 * A call supplying a \param value of NULL marks the TLS slot for the calling thread as empty and
 * the slot may be assigned subsequently to another thread. A call supplying a \param value of NULL
 * cannot fail.
 *
 * A HAL implementation may have a fixed, low limit on TLS slots. All client threads that call an
 * ADI API function MUST after it's final API call and before termination call
 * adi_hal_set_tls(HAL_TLS_END, NULL) to ensure its TLS slots are made available for subsequent
 * threads using the API.
 *
 * Non-multithreaded HALs must implement this function and still keep an association between \param
 * tlsType and \param value but can use the same association for each thread - i.e. simply ignore
 * the identity of the calling thread so that a value set by one thread overwrites the value
 * set by any previous thread.
 */
ADI_API_EX adi_hal_Err_e (*adi_hal_TlsSet)(const adi_hal_TlsType_e tlsType, void* const value);

/**
 * \brief Return the value associated with the calling thread for \param tlsType.
 *
 * Non-multithreaded HALs must implement this function and still keep an association between \param
 * tlsType and \param value but can use the same association for each thread - i.e. simply ignore
 * the identity of the calling thread so the value returned for all threads is the same.
 *
 * Further details of the HAL thread-local storage API are detailed by adi_hal_TlsSet.
 */
ADI_API_EX void* (*adi_hal_TlsGet)(const adi_hal_TlsType_e tlsType);

/**
 * \brief HAL Platform Setup
 * 
 *        This function assigns each function pointer declared in adi_platform.h to the correct
 *        implementation defined in the platform files associated with the adi_hal_Platforms_e
 *        platform input parameter.
 * 
 * \param devHalInfo    Pointer to be assumed as an ADI HAL Configuration
 * \param platform      ADI Platform Type to be Configured
 * 
 * \retval adi_hal_Err_e - ADI_HAL_ERR_OK if successful
 */
ADI_API_EX adi_hal_Err_e adi_hal_PlatformSetup(const adi_hal_Platforms_e platform);


/**
 * \brief HAL Board Identify
 * 
 * \param boardName Pointer to Board Name String
 * 
 * \retval adi_hal_Err_e - ADI_HAL_ERR_OK if successful
 */
ADI_API_EX adi_hal_Err_e(*adi_hal_BoardIdentify)(char** boardNames, int32_t* numBoards);

long int_20db_to_mag(long a, int mdB);

#endif /* __ADI_PLATFORM_H__ */


