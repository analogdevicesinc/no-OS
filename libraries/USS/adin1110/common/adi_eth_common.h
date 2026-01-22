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

/** @addtogroup adi_eth_common Common definition for the ADI Ethernet device drivers
 *  @{
 */

#ifndef ADI_ETH_COMMON_H
#define ADI_ETH_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*!
* @brief Status codes for the Ethernet devices.
*/
typedef enum
{
    ADI_ETH_SUCCESS = 0,                /*!< Success.                                                   */
    ADI_ETH_MDIO_TIMEOUT,               /*!< MDIO timeout.                                              */
    ADI_ETH_COMM_ERROR,                 /*!< Communication error.                                       */
    ADI_ETH_COMM_ERROR_SECOND,          /*!< Communication error.                                       */
    ADI_ETH_COMM_TIMEOUT,               /*!< Communications timeout with the host.                      */
    ADI_ETH_UNSUPPORTED_DEVICE,         /*!< Unsupported device.                                        */
    ADI_ETH_DEVICE_UNINITIALIZED,       /*!< Device not initialized.                                    */
    ADI_ETH_HW_ERROR,                   /*!< Hardware error.                                            */
    ADI_ETH_INVALID_PARAM,              /*!< Invalid parameter.                                         */
    ADI_ETH_PARAM_OUT_OF_RANGE,         /*!< Parameter out of range.                                    */
    ADI_ETH_INVALID_HANDLE,             /*!< Invalid device handle.                                     */
    ADI_ETH_IRQ_PENDING,                /*!< Interrupt request is pending.                              */
    ADI_ETH_READ_STATUS_TIMEOUT,        /*!< Timeout when reading status registers.                     */
    ADI_ETH_INVALID_POWER_STATE,        /*!< Invalid power state.                                       */
    ADI_ETH_HAL_INIT_ERROR,             /*!< HAL initialization error.                                  */
    ADI_ETH_INSUFFICIENT_FIFO_SPACE,    /*!< Insufficient TxFIFO space when trying to write a frame.    */
    ADI_ETH_CRC_ERROR,                  /*!< SPI integrity check failure (generic SPI).                 */
    ADI_ETH_PROTECTION_ERROR,           /*!< SPI integrity check failure (OPEN Alliance SPI).           */
    ADI_ETH_QUEUE_FULL,                 /*!< Transmit queue is full.                                    */
    ADI_ETH_QUEUE_EMPTY,                /*!< Receive queue is empty.                                    */
    ADI_ETH_BUFFER_TOO_SMALL,           /*!< Buffer is too small for received data.                     */
    ADI_ETH_INVALID_PORT,               /*!< Invalid port value.                                        */
    ADI_ETH_ADDRESS_FILTER_TABLE_FULL,  /*!< Address filter table is full.                              */
    ADI_ETH_MAC_BUSY,                   /*!< MAC is busy.                                               */
    ADI_ETH_COMM_BUSY,                  /*!< SPI communication busy.                                    */
    ADI_ETH_SPI_ERROR,                  /*!< SPI error.                                                 */
    ADI_ETH_SW_RESET_TIMEOUT,           /*!< Software reset timeout.                                    */
    ADI_ETH_CONFIG_SYNC_ERROR,          /*!< Configuration change attempted after configuration sync.   */
    ADI_ETH_VALUE_MISMATCH_ERROR,       /*!< Value does not match expected value.                       */
    ADI_ETH_FIFO_SIZE_ERROR,            /*!< Desired FIFO size exceeds 28k byte limit.                  */
    ADI_ETH_TS_COUNTERS_DISABLED,       /*!< Timestamp counters are not enabled.                        */
    ADI_ETH_NO_TS_FORMAT,               /*!< No timstamp format selected or timestamps captured.        */
    ADI_ETH_NOT_IMPLEMENTED,            /*!< Not implemented in hardware.                               */
    ADI_ETH_NOT_IMPLEMENTED_SOFTWARE,   /*!< Not implemented in software.                               */
    ADI_ETH_UNSUPPORTED_FEATURE,        /*!< Hardware feature not supported by the software driver.     */
    ADI_ETH_PLACEHOLDER_ERROR,          /*!< Unassigned (placeholder) error.                            */
} adi_eth_Result_e;

/*!
* @brief Callback function definition for the Ethernet devices.
*/
typedef void (* adi_eth_Callback_t) (
    void      *pCBParam,                /*!< Client-supplied callback parameter. */
    uint32_t   Event,                   /*!< Event ID specific to the Driver/Service. */
    void      *pArg                     /*!< Pointer to the event-specific argument. */

    );                   
#ifdef __cplusplus
}
#endif

#endif /* ADI_ETH_COMMON_H */

/** @}*/
