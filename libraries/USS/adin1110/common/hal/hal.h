/*
 *---------------------------------------------------------------------------
 *
 * Copyright (c) 2020, 2021 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc.
 * and its licensors.By using this software you agree to the terms of the
 * associated Analog Devices Software License Agreement.
 *
 *---------------------------------------------------------------------------
 */

/** @addtogroup hal HAL API
 *  @{
 */

#ifndef HAL_H
#define HAL_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "adi_eth_common.h"
#include "hal_port_specific.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*! HAL return code indicating successful execution. */
#define ADI_HAL_SUCCESS         (0)
/*! HAL return code indicating an error has been encountered. */
#define ADI_HAL_ERROR           (1)

/*! Disables all processor interrupts. */
#define     ADI_HAL_ENTER_CRITICAL_SECTION(...)     //__disable_irq()
/*! Re-enables processor interrupts. */
#define     ADI_HAL_EXIT_CRITICAL_SECTION(...)      //__enable_irq()

#ifndef MXC_INFO
#define MXC_INFO 1
#endif
#ifndef MXC_DEBUG
#define MXC_DEBUG 1
#endif

#define MXC_ERR_MSG(...) \
    do { \
        printf("\x1b[31;1m"); \
        printf(__VA_ARGS__); \
        printf("\x1b[0m"); \
    } while(0)

#define MXC_INFO_MSG(...) \
    do { \
        if (MXC_INFO) { \
            printf(__VA_ARGS__); \
        } \
    } while(0)

#define MXC_DEBUG_MSG(...) \
    do\
        if (MXC_DEBUG)\
            printf(__VA_ARGS__); \
    while(0)

#define MXC_DEBUG_MSG_RED(...) \
    do { \
        if (MXC_DEBUG) { \
            printf("\x1b[31;1m"); \
            printf(__VA_ARGS__); \
            printf("\x1b[0m"); \
        } \
    } while(0)

#define MXC_DEBUG_MSG_GRN(...) \
    do { \
        if (MXC_DEBUG) { \
            printf("\x1b[32;1m"); \
            printf(__VA_ARGS__); \
            printf("\x1b[0m"); \
        } \
    } while(0)

#define MXC_DEBUG_MSG_YEL(...) \
    do { \
        if (MXC_DEBUG) { \
            printf("\x1b[33;1m"); \
            printf(__VA_ARGS__); \
            printf("\x1b[0m"); \
        } \
    } while(0)

#define MXC_DEBUG_ASSERT(s,result,expected_value) \
    do { \
        if ((result) != (expected_value)) { \
            printf("\x1b[31;1m"); \
            printf("%s, Error Code: 0x%08X\n\r", (s),(result)); \
            printf("@ %s  %d\n", __FILE__,__LINE__); \
            printf("\x1b[0m"); \
            exit(0); \
        } \
    } while (0)

inline void print_hex(uint8_t *buf, uint32_t size)
{
    int i;

    if(size) {
        for(i = 0; i < size; i++) {
            MXC_DEBUG_MSG("%02x ", buf[i]);
            if (i % 16 == 15)
                MXC_DEBUG_MSG("\n");
        }

        if ((i % 16) != 15)
        MXC_DEBUG_MSG("\n");
    }
}
/*!
* @brief Callback function definition for HAL.
*/
typedef void (*HAL_Callback_t) (
    void      *pCBParam,            /*!< Client-supplied callback parameter. */
    uint32_t   Event,               /*!< Event ID specific to the Driver/Service. */
    void      *pArg                 /*!< Pointer to the event-specific argument. */
    );         

#if !defined(ADI_HAL_USE_DEVICE_HANDLE)

/*! 
 * @name HAL Wrappers
 * These macros will strip out the device handle before passing arguments to the HAL functions.
 * If the HAL implementation for a certain platform requires use of the device handle to implement HAL functions, 
 * define ADI_HAL_USE_DEVICE_HANDLE and supply definitions for the HAL function prototypes in the port-specific headers.            
 */

/** @{ */

/*!
 * @brief       Wrapper for HAL_Init_Hook().
 *
 * @copybrief   HAL_Init_Hook()
 * 
 * @param[in]   dev     Device driver handle.
 */
#define     ADI_HAL_INIT(dev, ...)                  HAL_Init_Hook(__VA_ARGS__)

/*!
 * @brief       Wrapper for HAL_UnInit_Hook().
 *
 * @copybrief   HAL_UnInit_Hook()
 * 
 * @param[in]   dev     Device driver handle.
 */
#define     ADI_HAL_UNINIT(dev, ...)                HAL_UnInit_Hook(__VA_ARGS__)

#if defined(ADIN1100)
/*!
 * @brief       Wrapper for HAL_PhyRead().
 *
 * @copybrief   HAL_PhyRead()
 * 
 * @param[in]   dev     Device driver handle.
 */
#define     ADI_HAL_PHY_READ(dev, ...)              HAL_PhyRead(__VA_ARGS__)

/*!
 * @brief       Wrapper for HAL_PhyWrite().
 *
 * @copybrief   HAL_PhyWrite()
 * 
 * @param[in]   dev     Device driver handle.
 */
#define     ADI_HAL_PHY_WRITE(dev, ...)             HAL_PhyWrite(__VA_ARGS__)

#endif

/*!
 * @brief       Wrapper for HAL_EnableIrq().
 *
 * @copybrief   HAL_EnableIrq()
 * 
 * @param[in]   dev     Device driver handle.
 */
#define     ADI_HAL_ENABLE_IRQ(dev, ...)            HAL_EnableIrq(__VA_ARGS__)

/*!
 * @brief       Wrapper for HAL_DisableIrq().
 *
 * @copybrief   HAL_DisableIrq()
 * 
 * @param[in]   dev     Device driver handle.
 */
#define     ADI_HAL_DISABLE_IRQ(dev, ...)           HAL_DisableIrq(__VA_ARGS__)

/*!
 * @brief       Wrapper for HAL_GetEnableIrq().
 *
 * @copybrief   HAL_GetEnableIrq()
 * 
 * @param[in]   dev     Device driver handle.
 */
#define     ADI_HAL_GET_ENABLE_IRQ(dev, ...)        HAL_GetEnableIrq(__VA_ARGS__)

/*!
 * @brief       Wrapper for HAL_SetPendingIrq().
 *
 * @copybrief   HAL_SetPendingIrq()
 * 
 * @param[in]   dev     Device driver handle.
 */
#define     ADI_HAL_SET_PENDING_IRQ(dev, ...)       HAL_SetPendingIrq(__VA_ARGS__)

/*!
 * @brief       Wrapper for HAL_GetPendingIrq().
 *
 * @copybrief   HAL_GetPendingIrq()
 * 
 * @param[in]   dev     Device driver handle.
 */
#define     ADI_HAL_GET_PENDING_IRQ(dev, ...)       HAL_GetPendingIrq(__VA_ARGS__)

/*!
 * @brief       Wrapper for HAL_RegisterCallback().
 *
 * @copybrief   HAL_RegisterCallback()
 * 
 * @param[in]   dev     Device driver handle.
 */
#define     ADI_HAL_REGISTER_CALLBACK(dev, ...)     HAL_RegisterCallback(__VA_ARGS__)

#if defined(ADIN1110) || defined(ADIN2111)

/*!
 * @brief       Wrapper for HAL_SpiRegisterCallback().
 *
 * @copybrief   HAL_SpiRegisterCallback()
 * 
 * @param[in]   dev     Device driver handle.
 */
#define     ADI_HAL_SPI_REGISTER_CALLBACK(dev, ...) HAL_SpiRegisterCallback(__VA_ARGS__)

/*!
 * @brief       Wrapper for HAL_SpiReadWrite().
 *
 * @copybrief   HAL_SpiReadWrite()
 * 
 * @param[in]   dev     Device driver handle.
 */
#define     ADI_HAL_SPI_READ_WRITE(dev, ...)        HAL_SpiReadWrite(__VA_ARGS__)

/*!
 * @brief       Wrapper for HAL_FcsInit().
 *
 * @copybrief   HAL_FcsInit()
 * 
 * @param[in]   dev     Device driver handle.
 */
#define     ADI_HAL_FCS_INIT(dev, ...)              HAL_FcsInit(__VA_ARGS__)

/*!
 * @brief       Wrapper for HAL_FcsUnInit().
 *
 * @copybrief   HAL_FcsUnInit()
 * 
 * @param[in]   dev     Device driver handle.
 */
#define     ADI_HAL_FCS_UNINIT(dev, ...)            HAL_FcsUnInit(__VA_ARGS__)

/*!
 * @brief       Wrapper for HAL_FcsCalculate().
 *
 * @copybrief   HAL_FcsCalculate()
 * 
 * @param[in]   dev     Device driver handle.
 */
#define     ADI_HAL_FCS_CALCULATE(dev, ...)         HAL_FcsCalculate(__VA_ARGS__)

#endif

/** @} */

/*!
 * @name Standard HAL Functions
 * The standard HAL implementation here does not pass the device handle to the HAL functions; it is stripped out by the HAL macros.
 * If that capability is needed, define ADI_HAL_USE_DEVICE_HANDLE and implement the required HAL function prototypes in the port-specific headers.
 */

/** @{ */

#if defined(ADIN1100)

/*!
 * @brief           HAL read from PHY register.
 *
 * @param [in]      phyAddr     PHY address.
 * @param [in]      regAddr     Register address as represented in the header file (DEVTYPE + ADDR).
 * @param [out]     data        16-bit data read from the register.
 *
 * @return          Status
 *                  - #ADI_HAL_SUCCESS      Call completed successfully.
 *                  - #ADI_HAL_ERROR        Error encountered during function execution.
 *
 * @details         Reads from a PHY register using the MDIO interface.
 *
 *                  Implements a blocking call.
 *
 * @sa              HAL_PhyWrite()
 */
uint32_t        HAL_PhyRead             (uint8_t phyAddr, uint32_t regAddr, uint16_t *data);

/*!
 * @brief           HAL write to PHY register.
 *
 * @param [in]      phyAddr     PHY address.
 * @param [in]      regAddr     Register address as represented in the header file (DEVTYPE + ADDR).
 * @param [out]     data        16-bit data read from the register.
 *
 * @return          Status
 *                  - #ADI_HAL_SUCCESS      Call completed successfully.
 *                  - #ADI_HAL_ERROR        Error encountered during function execution.
 *
 * @details         Write to a PHY register using the MDIO interface.
 *
 *                  Implements a blocking call.
 *
 * @sa              HAL_PhyRead()
 */
uint32_t        HAL_PhyWrite            (uint8_t phyAddr, uint32_t regAddr, uint16_t data);
#endif

/*!
 * @brief           Interrupt enable.
 *
 * @return          Status
 *                  - #ADI_HAL_SUCCESS      Call completed successfully.
 *                  - #ADI_HAL_ERROR        Error encountered during function execution.
 *
 * @details         Enables the external interrupt (INT_N) in the host processor.
 *
 * @sa              HAL_DisableIrq()
 */
uint32_t        HAL_EnableIrq           (void);

/*!
 * @brief           Interrupt disable.
 *
 * @return          Status
 *                  - #ADI_HAL_SUCCESS      Call completed successfully.
 *                  - #ADI_HAL_ERROR        Error encountered during function execution.
 *
 * @details         Disables the external interrupt (INT_N) in the host processor.
 *
 * @sa              HAL_EnableIrq()
 */
uint32_t        HAL_DisableIrq          (void);

/*!
 * @brief           Get interrupt enable status.
 *
 * @return          Enable status
 *                  - 1         Interrupt enabled.
 *                  - 0         Interrupt disabled.
 *
 * @details         Disables the external interrupt (INT_N) in the host processor.
 *
 * @sa
 */
uint32_t        HAL_GetEnableIrq        (void);

/*!
 * @brief           Set pending interrupt.
 *
 * @return          Status
 *                  - #ADI_HAL_SUCCESS      Call completed successfully.
 *                  - #ADI_HAL_ERROR        Error encountered during function execution.
 *
 * @details         Sets the external interrupt (INT_N) as pending, causing the IRQ handler
 *                  to be executed. This is used for deferred operations.
 *
 * @sa
 */
uint32_t        HAL_SetPendingIrq       (void);

/*!
 * @brief           Get pending interrupt.
 *
 * @return          Status
 *                  - 0 Interrupt is not pending.
 *                  - 1 Interrupt is pending.
 *
 * @details         Checks the status of the external interrupt (INT_N).
 *
 * @sa
 */
uint32_t        HAL_GetPendingIrq       (void);


/*!
 * @brief           Register device IRQ callback function.
 *
 * @param [in]      intCallback     Callback function.
 * @param [in]      hDevice         Pointer to device handler.
 * 
 * @return          Status
 *                  - #ADI_HAL_SUCCESS      Call completed successfully.
 *                  - #ADI_HAL_ERROR        Error encountered during function execution.
 *
 * @details         Registers a callback for the PHY interrupt.
 *
 * @sa
 */
uint32_t        HAL_RegisterCallback    (HAL_Callback_t const *intCallback, void *hDevice);

#if defined(ADIN1110) || defined(ADIN2111)

/*!
 * @brief           Register SPI IRQ callback function.
 *
 * @param [in]      spiCallback     Callback function.
 * @param [in]      hDevice         Pointer to device handler.
 * 
 * @return          Status
 *                  - #ADI_HAL_SUCCESS      Call completed successfully.
 *                  - #ADI_HAL_ERROR        Error encountered during function execution.
 *
 * @details         Registers a callback for the SPI interrupt.
 *
 * @sa
 */
uint32_t        HAL_SpiRegisterCallback (HAL_Callback_t const *spiCallback, void * hDevice);

/*!
 * @brief           HAL SPI read/write.
 *
 * @param [in]      pBufferTx   Pointer to transmit data buffer.
 * @param [in]      nbBytes     Number bytes to transfer.
 * @param [in]      useDma      Enable/disable DMA for SPI.
 * @param [out]     pBufferRx   Pointer to receive data buffer.
 *
 * @return          Status
 *                  - #ADI_HAL_SUCCESS      Call completed successfully.
 *                  - #ADI_HAL_ERROR        Error encountered during function execution.
 *
 * @details         Initiate data transfer using the SPI interface.
 *                  Implements a non blocking call.
 *
 * @sa
 */
uint32_t        HAL_SpiReadWrite (uint8_t *pBufferTx, uint8_t *pBufferRx, uint32_t nbBytes, bool useDma);

/*!
 * @brief           HAL Frame check sequence (FCS) initialization.
 * 
 * @return          Status
 *                  - #ADI_HAL_SUCCESS      Call completed successfully.
 *                  - #ADI_HAL_ERROR        Error encountered during function execution.
 *
 * @details         Initialize FCS calculator.
 * 
 * @sa
 */
uint32_t        HAL_FcsInit (void);

/*!
 * @brief           HAL Frame check sequence (FCS) uninitialization.
 * 
 * @return          Status
 *                  - #ADI_HAL_SUCCESS      Call completed successfully.
 *                  - #ADI_HAL_ERROR        Error encountered during function execution.
 *
 * @details         Uninitialize FCS calculator.
 * 
 * @sa
 */
uint32_t        HAL_FcsUnInit (void);

/*!
 * @brief           HAL Frame check sequence (FCS) calculation.
 * 
 * @param [in]      pBuf      Pointer to data buffer on which to perfrom the calculation.
 * @param [in]      nbBytes   Number of bytes on which to perfrom the calculation.  
 * 
 * @return          4-byte Ethernet FCS as U32.
 *
 * @details         Calculate and return the 32-bit FCS. Per the 802.3 standard, 
 *                  the computation is via CRC32 BZIP2 with polynomial = 0x4C11DB7, 
 *                  initial CRC = 0xFFFFFFFF, and with the CRC post-complemented.
 * 
 * @sa
 */
uint32_t        HAL_FcsCalculate (uint8_t *pBuf, uint32_t nbBytes);

#endif

/*!
 * @brief           HAL initialization hook.
 *
 * @return          Status
 *                  - #ADI_HAL_SUCCESS      Call completed successfully.
 *                  - #ADI_HAL_ERROR        Error encountered during function execution.
 *
 * @details         Called during the driver initialization, to allow the user to run application-specific
 *                  configuration, if needed. Can be left empty if such functionality is not required.
 *
 * @sa              HAL_UnInit_Hook()
 */
uint32_t        HAL_Init_Hook(void);

/*!
 * @brief           HAL uninitialization hook.
 *
 * @return          Status
 *                  - #ADI_HAL_SUCCESS      Call completed successfully.
 *                  - #ADI_HAL_ERROR        Error encountered during function execution.
 *
 * @details         Called during the driver uninitialization, to allow the user to run application-specific
 *                  configuration, if needed. Can be left empty if such functionality is not required.
 *
 * @sa              HAL_Init_Hook()
 */
uint32_t        HAL_UnInit_Hook(void);

/** @} */

#else

/*! 
 * @name HAL Wrappers
 * If the HAL implementation for a certain platform requires use of the device handle to implement HAL functions, 
 * define ADI_HAL_USE_DEVICE_HANDLE to allow the following macros to pass the device handle and all other arguments to the HAL functions.
 * If ADI_HAL_USE_DEVICE_HANDLE is defined, the HAL function prototypes must be defined in the port-specific headers.
 */

/** @{ */

/*!
 * @brief       Wrapper for HAL_Init_Hook() with platform-specific prototype.
 */
#define     ADI_HAL_INIT(...)                       HAL_Init_Hook(__VA_ARGS__)

/*!
 * @brief       Wrapper for HAL_UnInit_Hook() with platform-specific prototype.
 */
#define     ADI_HAL_UNINIT(...)                     HAL_UnInit_Hook(__VA_ARGS__)

/*!
 * @brief       Wrapper for HAL_PhyRead() with platform-specific prototype.
 */
#define     ADI_HAL_PHY_READ(...)                   HAL_PhyRead(__VA_ARGS__)

/*!
 * @brief       Wrapper for HAL_PhyWrite() with platform-specific prototype.
 */
#define     ADI_HAL_PHY_WRITE(...)                  HAL_PhyWrite(__VA_ARGS__)

/*!
 * @brief       Wrapper for HAL_EnableIrq() with platform-specific prototype.
 */
#define     ADI_HAL_ENABLE_IRQ(...)                 HAL_EnableIrq(__VA_ARGS__)

/*!
 * @brief       Wrapper for HAL_DisableIrq() with platform-specific prototype.
 */
#define     ADI_HAL_DISABLE_IRQ(...)                HAL_DisableIrq(__VA_ARGS__)

/*!
 * @brief       Wrapper for HAL_GetEnableIrq() with platform-specific prototype.
 */
#define     ADI_HAL_GET_ENABLE_IRQ(...)             HAL_GetEnableIrq(__VA_ARGS__)

/*!
 * @brief       Wrapper for HAL_SetPendingIrq() with platform-specific prototype.
 */
#define     ADI_HAL_SET_PENDING_IRQ(...)            HAL_SetPendingIrq(__VA_ARGS__)

/*!
 * @brief       Wrapper for HAL_GetPendingIrq() with platform-specific prototype.
 */
#define     ADI_HAL_GET_PENDING_IRQ(...)            HAL_GetPendingIrq(__VA_ARGS__)

/*!
 * @brief       Wrapper for HAL_RegisterCallback() with platform-specific prototype.
 */
#define     ADI_HAL_REGISTER_CALLBACK(...)          HAL_RegisterCallback(__VA_ARGS__)

/*!
 * @brief       Wrapper for HAL_SpiRegisterCallback() with platform-specific prototype.
 */
#define     ADI_HAL_SPI_REGISTER_CALLBACK(...)      HAL_SpiRegisterCallback(__VA_ARGS__)

/*!
 * @brief       Wrapper for HAL_SpiReadWrite() with platform-specific prototype.
 */
#define     ADI_HAL_SPI_READ_WRITE(...)             HAL_SpiReadWrite(__VA_ARGS__)

/*!
 * @brief       Wrapper for HAL_FcsInit() with platform-specific prototype.
 */
#define     ADI_HAL_FCS_INIT(...)                   HAL_FcsInit(__VA_ARGS__)

/*!
 * @brief       Wrapper for HAL_FcsUnInit() with platform-specific prototype.
 */
#define     ADI_HAL_FCS_UNINIT...)                  HAL_FcsUnInit(__VA_ARGS__)

/*!
 * @brief       Wrapper for HAL_FcsCalculate() with platform-specific prototype.
 */
#define     ADI_HAL_FCS_CALCULATE(...)              HAL_FcsCalculate(__VA_ARGS__)

/** @} */

#endif

#ifdef __cplusplus
}
#endif

#endif /* HAL_H */

/** @}*/
