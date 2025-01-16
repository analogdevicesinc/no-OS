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

/** @addtogroup mac MAC Definitions
 *  @{
 */

#ifndef ADI_MAC_H
#define ADI_MAC_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* The PHY registers are included because the MAC code has to access */
/* interrupt status registers from the PHY.                          */
#if defined(ADIN1110)
#include "ADIN1110_mac_addr_rdef.h"
#include "ADIN1110_mac_typedefs.h"
#include "ADIN1110_phy_addr_rdef.h"
#elif defined(ADIN2111)
#include "ADIN2111_mac_addr_rdef.h"
#include "ADIN2111_mac_typedefs.h"
#include "ADIN2111_phy_addr_rdef.h"
#endif

#include "adi_eth_common.h"
#include "hal.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*! Maximum number of MAC addresses allowed by the MAC device. */
#define ADI_MAC_ADDR_NUM_ENTRIES        (16)

/*! Size of the Tx queue, can be previously defined by the application. */
#if !defined(TX_QUEUE_NUM_ENTRIES)
#define TX_QUEUE_NUM_ENTRIES            (4)
#endif
/*! Size of the Rx queue, can be previously defined by the application. */
#if !defined(RX_QUEUE_NUM_ENTRIES)
#define RX_QUEUE_NUM_ENTRIES            (4)
#endif

/*! Actual size of the Rx queue. */
#define RX_QUEUE_NUM_ENTRIES_RAW        (RX_QUEUE_NUM_ENTRIES + 1)
/*! Actual Size of the Tx queue. */
#define TX_QUEUE_NUM_ENTRIES_RAW        (TX_QUEUE_NUM_ENTRIES + 1)

/*! Size of the MAC device structure components common to all configurations. */
#define ADI_MAC_DEVICE_SIZE_COMMON      (4 * 4 + 6 * 4 + 8 * ADI_MAC_EVT_MAX + \
                                         (TX_QUEUE_NUM_ENTRIES_RAW + RX_QUEUE_NUM_ENTRIES_RAW) * sizeof(adi_mac_FrameStruct_t) + \
                                         2 * sizeof(adi_mac_Queue_t) + \
                                         sizeof(adi_mac_StatusRegisters_t))

#if defined(ADI_MAC_ENABLE_RX_QUEUE_HI_PRIO)
/*! Size of the MAC device structure components addded when high-priority Rx queue mode is enabled. */
#define ADI_MAC_DEVICE_SIZE_QUEUE_HI    (RX_QUEUE_NUM_ENTRIES_RAW * sizeof(adi_mac_FrameStruct_t) + sizeof(adi_mac_Queue_t))
#else
/*! Size of the MAC device structure components addded when high-priority Rx queue mode is disabled. */
#define ADI_MAC_DEVICE_SIZE_QUEUE_HI    (0)
#endif

#if defined(SPI_OA_EN)
/*! Size of the MAC device structure components addded when OPEN Alliance SPI mode is enabled. */
#define ADI_MAC_DEVICE_SIZE_OA          (2 * 4 + 15 * 4 + sizeof(adi_mac_OaErrorStats_t) + ADI_OA_RX_BACKUP_BUF_SIZE + 2 * ADI_MAC_SPI_CTRL_BUF_SIZE)
#else
/*! Size of the MAC device structure components addded when OPEN Alliance SPI mode is disabled. */
#define ADI_MAC_DEVICE_SIZE_OA          (0)
#endif

/*! Size of the MAC device structure, in bytes. Needs to be a multiple of 4. Note the actual size will depend on packing of non-uint32_t members.*/
#define ADI_MAC_DEVICE_SIZE             (ADI_MAC_DEVICE_SIZE_COMMON + ADI_MAC_DEVICE_SIZE_QUEUE_HI + ADI_MAC_DEVICE_SIZE_OA)

/*! MAC address filter table entries that allow usage of a mask. */
#define ADI_MAC_ADDR_MASK_MAX           (2)

/*! SPI header value indicating a data transaction.     */
#define ADI_MAC_SPI_TRANSACTION_DATA        (0)
/*! SPI header value indicating a control transaction.  */
#define ADI_MAC_SPI_TRANSACTION_CONTROL     (1)

/*! SPI header value indicating a read transaction.     */
#define ADI_MAC_SPI_READ                    (0)
/*! SPI header value indicating a write transaction.    */
#define ADI_MAC_SPI_WRITE                   (1)

/*! SPI register access size in bytes. */
#define ADI_MAC_SPI_ACCESS_SIZE             (4)
/*! Type of SPI register access size. */
#define ADI_MAC_SPI_ACCESS_UNIT_TYPE        uint32_t

/*! Number of MDIO Access registers. */
#define ADI_MAC_MDIO_MAX_COUNT              (8)
/*! Number of MDIO Access registers to use in parallel. */
#define ADI_MAC_MDIO_PARALLEL_COUNT         (2)

/*! Maximum size of all FIFOs combined, in kBytes. */
#define ADI_MAC_FIFO_MAX_SIZE               (28)

/*! Maximum number of attempts to read MDIO RDY bit. */
#define ADI_MAC_MDIO_MAX_RETRIES            (10)

/*! Size of the buffers used in control transactions, in bytes. */
#define ADI_MAC_SPI_CTRL_BUF_SIZE           (256)

/*! Maximum number of iterations to wait for the SPI transaction to finish. */
#define ADI_SPI_TIMEOUT                     (100000)

/* MAC reset keys */
/*! Key 1 for MAC-only reset. */
#define RST_MAC_ONLY_KEY1                   (0x4F1C)
/*! Key 2 for MAC-only reset. */
#define RST_MAC_ONLY_KEY2                   (0xC1F4)

/*! Maximum number of attempts to establish the SPI interface is up and responsive. */
#define ADI_MAC_IF_UP_MAX_RETRIES           (25000)

/*! Maximum number of attempts to establish the read the expected value of PHYID register during the initialization. */
/*  This is based on a wait time of 50ms and a SPI read of 2us.                                                      */
#define ADI_MAC_INIT_MAX_RETRIES            (25000)

/*! Minimum frame size, without FCS. */
#define MIN_FRAME_SIZE                      (60)
/*! Maximum frame size, without FCS. */
#define MAX_FRAME_SIZE                      (1518)
/*! FCS size */
#define FCS_SIZE                            (4)

/*! Receive backup buffer size, in chunks. Note this is not currently used! */
#define ADI_OA_RX_BACKUP_BUF_CHUNK_COUNT    (1)
/*! Receive backup buffer size, in bytes. Note this is not currently used! */
#define ADI_OA_RX_BACKUP_BUF_SIZE           (68 * ADI_OA_RX_BACKUP_BUF_CHUNK_COUNT)

/*! Initialization value for status variables corresponding to PHY IRQ registers. */
/*  This value cannot be achieved in hardware, also different from 0xFFFFFFFF which can indicate an MDIO error. */
#define ADI_MAC_PHY_STATUS_INIT_VAL         (0x7FFF7FFF)

/*! Mask for timestamp timer bits that are disregarded through quantization. */
#define ADI_MAC_TS_QE_MASK              (0xF)
/*! Resolution in nanoseconds of the timestamp timer. */
#define ADI_MAC_TS_MIN_NS               (16)
/*! Number of nanoseconds in a second. */
#define ADI_MAC_TS_ONE_SECOND_IN_NS    (1000000000ULL)
/*! Mask for the nanoseconds portion of a 32b/64b 1588 timestamp value. */
#define ADI_MAC_TS_1588_NS_MASK        (0x3FFFFFFF)
/*! Mask for the seconds portion of a 32b 1588 timestamp value. */
#define ADI_MAC_TS_1588_SEC_MASK       (0xC0000000)
/*! Bit position  for the seconds portion of a 32b 1588 timestamp value. */
#define ADI_MAC_TS_1588_SEC_POS        (28)
/*! Nominal resolution of free-running counter in nanoseconds. */
#define ADI_MAC_TS_FREE_CNT_LSB_IN_NS  (100.0/12.0)

/*! Byte order conversion for 16-bit values. */
#define HTON16(a)                           ((uint16_t)((((a) & 0xFF) << 8) | (((a) >> 8) & 0xFF)))
/*! Byte order conversion for 32-bit values. */
#define HTON32(a)                           __REV((a))

/*! MDIO Device Address extraction from 32-bit PHY register address. */
#define DEVTYPE(a)                          (a >> 16)
/*! MDIO Register Address extraction from 32-bit PHY register address. */
#define REGADDR(a)                          (a & 0xFFFF)

/*!
 * @brief Driver states
 * States can be used in both generic SPI and OPEN Alliance SPI, or for just one protocol.
 */
typedef enum
{
    ADI_MAC_STATE_UNINITIALIZED         = 0,    /*!< Driver is not initialized. This is the initial state.  */
    ADI_MAC_STATE_INITIALIZED,                  /*!< Driver initialized.                                    */
    ADI_MAC_STATE_READY,                        /*!< Driver ready.                                          */
    ADI_MAC_STATE_TX_FRAME,                     /*!< Driver is transmitting a frame to the MAC over SPI.    */
    ADI_MAC_STATE_RX_FRAME,                     /*!< Driver is receiving a frame from the MAC over SPI.     */
    ADI_MAC_STATE_CONTROL_START,                /*!< Driver is starting a control transaction.              */
    ADI_MAC_STATE_CONTROL_END,                  /*!< Driver is completing a control transaction.            */
    ADI_MAC_STATE_DATA_START,                   /*!< Driver is starting a data transaction.                 */
    ADI_MAC_STATE_DATA_END,                     /*!< Driver is completing a data transaction.               */
    ADI_MAC_STATE_IRQ_START,                    /*!< Driver is servicing an interrupt request from the MAC. */
    ADI_MAC_STATE_DATA_READ_STATUS,             /*!< Driver is reading status from MAC registers.           */
    ADI_MAC_STATE_DATA_READ_PHY_REGISTER,
} adi_mac_State_e;

/*!
 * @brief MAC driver configuration.
 */
typedef struct
{
    void            *pDevMem;           /*!< Pointer to memory area used by the driver.                                                         */
    uint32_t        devMemSize;         /*!< Size of the memory used by the driver. Needs to be greater than or equal to #ADI_MAC_DEVICE_SIZE.  */
    bool            fcsCheckEn;         /*!< Configure the driver to check  */
} adi_mac_DriverConfig_t;

/*!
 * @brief SPI interface state.
 */
typedef enum
{
    ADI_MAC_SPI_STATE_READY = 0,        /*!< SPI interface ready. */
    ADI_MAC_SPI_STATE_RX,               /*!< SPI interface receiving (control transaction).     */
    ADI_MAC_SPI_STATE_RX_FRAME,         /*!< SPI interface receiving frame (data transaction).  */
    ADI_MAC_SPI_STATE_TX,               /*!< SPI interface transmitting (control transaction).  */
    ADI_MAC_SPI_STATE_TX_FRAME,         /*!< SPI interface transmitting (data transaction).     */
} adi_mac_SpiState_e;

/*!
 * @brief Egress timestamp capture.
 */
typedef enum
{
    ADI_MAC_EGRESS_CAPTURE_NONE     = 0,    /*!< No action.                         */
    ADI_MAC_EGRESS_CAPTURE_A,               /*!< Capture egress timestamp A.        */
    ADI_MAC_EGRESS_CAPTURE_B,               /*!< Capture egress timestamp B.        */
    ADI_MAC_EGRESS_CAPTURE_C,               /*!< Capture egress timestamp C.        */
} adi_mac_EgressCapture_e;

/*!
 * @brief Egress timestamp captured status.
 */
typedef struct
{
#if defined(ADIN2111)
    bool    p1TimestampReadyA;  /*!< Capture status of Port 1 egress timestamp A */
    bool    p1TimestampReadyB;  /*!< Capture status of Port 1 egress timestamp B */
    bool    p1TimestampReadyC;  /*!< Capture status of Port 1 egress timestamp C */
    bool    p2TimestampReadyA;  /*!< Capture status of Port 2 egress timestamp A */
    bool    p2TimestampReadyB;  /*!< Capture status of Port 2 egress timestamp B */
    bool    p2TimestampReadyC;  /*!< Capture status of Port 2 egress timestamp C */
#else
    bool    timestampReadyA;    /*!< Capture status of egress timestamp A */
    bool    timestampReadyB;    /*!< Capture status of egress timestamp B */
    bool    timestampReadyC;    /*!< Capture status of egress timestamp C */
#endif
} adi_mac_TimestampRdy_t;

/*!
 * @brief MAC address rule definition.
 */
typedef struct {
    union {
        struct {
             unsigned int TO_HOST          : 1; /**< Forward Frames Matching This MAC Address to the Host. */
             unsigned int TO_OTHER_PORT    : 1; /**< Forward Frames Matching This MAC Address to the Other Port. */
             unsigned int RESERVED2        : 1; /**< Reserved */
             unsigned int HOST_PRI         : 1; /**< Host Rx Port Priority */
             unsigned int RESERVED4        : 10; /**< Reserved */
             unsigned int APPLY2PORT1      : 1; /**< Apply to Port 1. */
             unsigned int APPLY2PORT2      : 1; /**< Apply to Port 2. */
        };
        uint16_t VALUE16;
    };
} adi_mac_AddressRule_t;

/*!
 * @brief MAC callback events.
 * Driver supports installing callbacks for the events defined here.
 */
typedef enum
{
    ADI_MAC_EVT_LINK_CHANGE         = 0,    /*!< Link status changed.                                           */
    ADI_MAC_EVT_TX_RDY,                     /*!< TX_RDY asserted.                                               */
    ADI_MAC_EVT_P1_RX_RDY,                  /*!< P1_RX_RDY asserted.                                            */
    ADI_MAC_EVT_STATUS,                     /*!< Nonzero unmasked status.                                       */
#if defined(ADIN2111)
    ADI_MAC_EVT_DYN_TBL_UPDATE,             /*!< Dynamic table can be updated.                                  */
#endif
#if !defined(SPI_OA_EN) || defined(DOXYGEN)
    ADI_MAC_EVT_RX_FRAME_RDY,               /*!< New frame ready to be read from the Rx FIFO (Generic SPI only).*/
#endif
    ADI_MAC_EVT_TIMESTAMP_RDY,              /*!< Egress timestamp has been captured in TTSCA register.          */
    ADI_MAC_EVT_MAX,                        /*!< Enumeration size marker.                                       */
} adi_mac_InterruptEvt_e;

/*!
 * @brief MAC statistics counters structure.
 */
typedef struct
{
    uint32_t RX_FRM_CNT;         /*!< Rx frame count.                     */
    uint32_t RX_BCAST_CNT;       /*!< Rx unicast frame count.             */
    uint32_t RX_MCAST_CNT;       /*!< Rx multicast frame count.           */
    uint32_t RX_UCAST_CNT;       /*!< Rx broadcast frame count.           */
    uint32_t RX_CRC_ERR_CNT;     /*!< Rx CRC errored frame count.         */
    uint32_t RX_ALGN_ERR_CNT;    /*!< Rx alignment error count.           */
    uint32_t RX_LS_ERR_CNT;      /*!< Rx long/Short error count.          */
    uint32_t RX_PHY_ERR_CNT;     /*!< Rx PHY error (MII RX_ER).           */
    uint32_t TX_FRM_CNT;         /*!< Tx frame count.                     */
    uint32_t TX_BCAST_CNT;       /*!< Tx unicast frame count.             */
    uint32_t TX_MCAST_CNT;       /*!< Tx multicast frame count.           */
    uint32_t TX_UCAST_CNT;       /*!< Tx broadcast frame count.           */
    uint32_t RX_DROP_FULL_CNT;   /*!< Rx dropped due to FIFO full count.  */
    uint32_t RX_DROP_FILT_CNT;   /*!< Rx dropped due to filtering count.  */
}  adi_eth_MacStatCounters_t;

/*!
 * @brief ADI frame header structure.
 */
typedef struct
{
    union {
        struct {
            uint16_t PORT               : 1;
            uint16_t RSVD0              : 1;
            uint16_t TIME_STAMP_PRESENT : 1;
            uint16_t TIME_STAMP_PARITY  : 1;
            uint16_t RSVD1              : 2;
            uint16_t EGRESS_CAPTURE     : 2;
            uint16_t RSVD2              : 2;
            uint16_t PRI                : 1;
            uint16_t RSVD3              : 5;
        };
        uint16_t VALUE16;
    };
} adi_mac_FrameHeader_t;

/*!
 * @brief Link status read from the MAC interface  (MAC_STATUS register).
 */
typedef enum
{
    ADI_ETH_LINK_STATUS_DOWN        = (0),         /*!< Link down.  */
    ADI_ETH_LINK_STATUS_UP          = (1),         /*!< Link up.    */
} adi_eth_LinkStatus_e;

/*!
 * @brief Link status read from the MAC interface  (MAC_STATUS register).
 */
typedef enum
{
    ADI_MAC_RX_FIFO_PRIO_LOW        = 0,
    ADI_MAC_RX_FIFO_PRIO_HIGH,
} adi_mac_RxFifoPrio_e;

/*!
 * @brief Buffer descriptor for Tx/Rx.
 */
typedef struct
{
    uint8_t                     *pBuf;              /*!< Pointer to the frame buffer for Tx or Rx.                  */
    uint32_t                    bufSize;            /*!< Buffer size in bytes, used to check for overflows in Rx.   */
    uint32_t                    trxSize;            /*!< Frame size (Tx or Rx), in bytes.                           */
    adi_eth_Callback_t          cbFunc;             /*!< Callback function. Called after the buffer was written to
                                                     *   the Tx FIFO in transmit or read from Rx FIFO on receive.   */
    adi_mac_RxFifoPrio_e        prio;               /*!< Indicates the Rx FIFO priority the frame was read from.    */
#if defined(ADIN2111)
    uint32_t                    port;               /*!< Port  (0/1) on which the frame will transmitted/received.  */
#else
    uint32_t                    rsvd;               /*!< Reserved.                                                  */
#endif
    adi_mac_EgressCapture_e     egressCapt;         /*!< Configure capture of the egress timestamp.                 */
    bool                        timestampValid;     /*!< Indicates if the timestamp field is valid.                 */
    uint32_t                    timestamp;          /*!< Ingress timestamp received from the MAC.                   */
    uint32_t                    timestampExt;       /*!< Extended storage for 64b ingress timestamp.                */
    uint32_t                    refCount;           /*!< Reference counter, indicates if a descriptor needs to
                                                         be sent more than once.                                    */
} adi_eth_BufDesc_t;

/*!
 * @brief Reset definitions.
 */
typedef enum
{
    ADI_ETH_RESET_TYPE_MAC_ONLY     = 0,            /*!< Reset the MAC only.                                        */
    ADI_ETH_RESET_TYPE_MAC_PHY,                     /*!< Reset both MAC and PHY.                                    */
} adi_eth_ResetType_e;


/*!
 * @brief Receive FIFO size definitions.
 */
typedef enum
{
    ADI_MAC_RX_FIFO_SIZE_2K  = (MAC_FIFO_SIZE_P1_RX_HI_SIZE_RXSIZE_2K),
    ADI_MAC_RX_FIFO_SIZE_0K  = (MAC_FIFO_SIZE_P1_RX_HI_SIZE_RXSIZE_0K),
    ADI_MAC_RX_FIFO_SIZE_4K  = (MAC_FIFO_SIZE_P1_RX_HI_SIZE_RXSIZE_4K),
    ADI_MAC_RX_FIFO_SIZE_6K  = (MAC_FIFO_SIZE_P1_RX_HI_SIZE_RXSIZE_6K),
    ADI_MAC_RX_FIFO_SIZE_8K  = (MAC_FIFO_SIZE_P1_RX_HI_SIZE_RXSIZE_8K),
    ADI_MAC_RX_FIFO_SIZE_10K = (MAC_FIFO_SIZE_P1_RX_HI_SIZE_RXSIZE_10K),
    ADI_MAC_RX_FIFO_SIZE_12K = (MAC_FIFO_SIZE_P1_RX_HI_SIZE_RXSIZE_12K),
    ADI_MAC_RX_FIFO_SIZE_14K = (MAC_FIFO_SIZE_P1_RX_HI_SIZE_RXSIZE_14K),
    ADI_MAC_RX_FIFO_SIZE_16K = (MAC_FIFO_SIZE_P1_RX_HI_SIZE_RXSIZE_16K),
} adi_mac_RxFifoSize_e;

/*!
 * @brief Host transmit FIFO size definitions.
 */
typedef enum
{
    ADI_MAC_HTX_FIFO_SIZE_0K  = (MAC_FIFO_SIZE_HTX_SIZE_TXSIZE_0K),
    ADI_MAC_HTX_FIFO_SIZE_2K  = (MAC_FIFO_SIZE_HTX_SIZE_TXSIZE_2K),
    ADI_MAC_HTX_FIFO_SIZE_4K  = (MAC_FIFO_SIZE_HTX_SIZE_TXSIZE_4K),
    ADI_MAC_HTX_FIFO_SIZE_6K  = (MAC_FIFO_SIZE_HTX_SIZE_TXSIZE_6K),
    ADI_MAC_HTX_FIFO_SIZE_8K  = (MAC_FIFO_SIZE_HTX_SIZE_TXSIZE_8K),
    ADI_MAC_HTX_FIFO_SIZE_10K = (MAC_FIFO_SIZE_HTX_SIZE_TXSIZE_10K),
    ADI_MAC_HTX_FIFO_SIZE_12K = (MAC_FIFO_SIZE_HTX_SIZE_TXSIZE_12K),
    ADI_MAC_HTX_FIFO_SIZE_14K = (MAC_FIFO_SIZE_HTX_SIZE_TXSIZE_14K),
    ADI_MAC_HTX_FIFO_SIZE_16K = (MAC_FIFO_SIZE_HTX_SIZE_TXSIZE_16K),
} adi_mac_HtxFifoSize_e;

#if defined(ADIN2111)
/*!
 * @brief Port transmit FIFO size definitions.
 */
typedef enum
{
    ADI_MAC_PTX_FIFO_SIZE_0K  = (MAC_FIFO_SIZE_P1_TX_SIZE_TXSIZE_0K),
    ADI_MAC_PTX_FIFO_SIZE_2K  = (MAC_FIFO_SIZE_P1_TX_SIZE_TXSIZE_2K),
    ADI_MAC_PTX_FIFO_SIZE_4K  = (MAC_FIFO_SIZE_P1_TX_SIZE_TXSIZE_4K),
    ADI_MAC_PTX_FIFO_SIZE_6K  = (MAC_FIFO_SIZE_P1_TX_SIZE_TXSIZE_6K),
    ADI_MAC_PTX_FIFO_SIZE_8K  = (MAC_FIFO_SIZE_P1_TX_SIZE_TXSIZE_8K),
    ADI_MAC_PTX_FIFO_SIZE_10K = (MAC_FIFO_SIZE_P1_TX_SIZE_TXSIZE_10K),
    ADI_MAC_PTX_FIFO_SIZE_12K = (MAC_FIFO_SIZE_P1_TX_SIZE_TXSIZE_12K),
    ADI_MAC_PTX_FIFO_SIZE_14K = (MAC_FIFO_SIZE_P1_TX_SIZE_TXSIZE_14K),
    ADI_MAC_PTX_FIFO_SIZE_16K = (MAC_FIFO_SIZE_P1_TX_SIZE_TXSIZE_16K),
} adi_mac_PtxFifoSize_e;
#endif

/*!
 * @brief FIFO size configuration.
 */
typedef struct
{
#if defined(ADIN2111)
    adi_mac_HtxFifoSize_e   htxSize;        /*!< Size of the host Tx FIFO. */
    adi_mac_RxFifoSize_e    p1RxLoSize;     /*!< Size of the Port 1 low-priority Rx FIFO. */
    adi_mac_RxFifoSize_e    p1RxHiSize;     /*!< Size of the Port 1 high-priority Rx FIFO. */
    adi_mac_RxFifoSize_e    p2RxLoSize;     /*!< Size of the Port 2 low-priority Rx FIFO. */
    adi_mac_RxFifoSize_e    p2RxHiSize;     /*!< Size of the Port 2 high-priority Rx FIFO. */
    adi_mac_PtxFifoSize_e   p1TxSize;       /*!< Size of the Port 2 to Port 1 Tx FIFO. */
    adi_mac_PtxFifoSize_e   p2TxSize;       /*!< Size of the Port 1 to Port 2 Tx FIFO. */
#else
    adi_mac_HtxFifoSize_e   txSize;         /*!< Size of the Tx FIFO. */
    adi_mac_RxFifoSize_e    rxLoSize;       /*!< Size of the low-priority Rx FIFO. */
    adi_mac_RxFifoSize_e    rxHiSize;       /*!< Size of the high-priority Rx FIFO. */
#endif
} adi_mac_FifoSizes_t;

/*!
 * @brief FIFO clear mode.
 */
typedef enum
{
#if defined(ADIN2111)
    ADI_MAC_CLEAR_ALL_FIFOS     = (BITM_MAC_FIFO_CLR_LES_FIFOS_CLR),
    ADI_MAC_CLEAR_P2P_FIFOS     = (BITM_MAC_FIFO_CLR_LES_P2P_FIFOS_CLR),
#endif
    ADI_MAC_CLEAR_RX_FIFO       = (BITM_MAC_FIFO_CLR_MAC_RXF_CLR),
#if !defined(ADIN2111)
    ADI_MAC_CLEAR_TX_FIFO       = (BITM_MAC_FIFO_CLR_MAC_TXF_CLR),
#endif
} adi_mac_FifoClrMode_e;


/*!
 * @brief Configuration options for TS_TIMER waveform.
 */
typedef struct
{
    uint32_t        periodNs;         /*!< Timer waveform period in nanoseconds.                        */
    float           dutyCycle;        /*!< Timer waveform duty cycle. Set to 0.5 for a 50% duty cycle.  */
    bool            idleState;        /*!< Timer waveform idle state. False is logic low.               */
    uint32_t        startTimeNs;      /*!< Timer waveform start time in nanoseconds, must be >=16.      */
} adi_mac_TsTimerConfig_t;

/*!
 * @brief Timestamp formats.
 */
typedef enum
{
    ADI_MAC_TS_FORMAT_NONE      = 0,    /*!< No timestamp                                                                       */
    ADI_MAC_TS_FORMAT_32B_FREE,         /*!< 32b free-running counter: 1 lsb = 1/120MHz = 8.33ns.                               */
    ADI_MAC_TS_FORMAT_32B_1588,         /*!< 32b 1588 timestamp: [31:30] seconds count, [29:0] nanoseconds count.               */
    ADI_MAC_TS_FORMAT_64B_1588,         /*!< 64b 1588 timestamp: [63:32] seconds count, [31:30] zero, [29:0] nanoseconds count. */
} adi_mac_TsFormat_e;

/*!
 * @brief Structure to hold time from parsed timestamps.
 */
typedef struct
{
    uint32_t    sec;    /*!< Number of whole seconds. */
    uint32_t    nsec;   /*!< Number of nanoseconds. */
} adi_mac_TsTimespec_t;


/*!
 * @brief Latest valid flag (EV or SV) read from the footer.
 */
typedef enum
{
    ADI_MAC_OA_VALID_FLAG_NONE      = 0,            /*!< No SV or EV flag received yet. This is the initial value.  */
    ADI_MAC_OA_VALID_FLAG_START,                    /*!< Latest valid flag was SV.                                  */
    ADI_MAC_OA_VALID_FLAG_END,                      /*!< Latest valid flag was EV.                                  */
} adi_mac_OaValidFlag_e;

/*!
 * @brief Error statistics (OPEN Alliance).
 */
typedef struct
{
    uint32_t        fdCount;                        /*!< Number of frames dropped due to FD=1 in the footer.                */
    uint32_t        invalidEvCount;                 /*!< Number of invalid EV=1 detected in the footer (no preceding SV).   */
    uint32_t        invalidSvCount;                 /*!< Number of invalid SV=1 detected in the footer (no preceding EV).   */
    uint32_t        ftrParityErrorCount;            /*!< Number of footer parity errors detected.                           */
    uint32_t        hdrParityErrorCount;            /*!< Number of header parity errors detected.                           */
    uint32_t        syncErrorCount;                 /*!< Number of SYNC errors (SYNC=0 in the footer).                      */
} adi_mac_OaErrorStats_t;

/*!
 * @brief Chunk Payload Selector (OPEN Alliance).
 */
typedef enum
{
    ADI_MAC_OA_CPS_8BYTE  = (MAC_CONFIG0_CPS_CHUNK_8BYTE),     /*!< 8 bytes per chunk  */
    ADI_MAC_OA_CPS_16BYTE = (MAC_CONFIG0_CPS_CHUNK_16BYTE),    /*!< 16 bytes per chunk */
    ADI_MAC_OA_CPS_32BYTE = (MAC_CONFIG0_CPS_CHUNK_32BYTE),    /*!< 32 bytes per chunk */
    ADI_MAC_OA_CPS_64BYTE = (MAC_CONFIG0_CPS_CHUNK_64BYTE),    /*!< 64 bytes per chunk */
} adi_mac_OaCps_e;

/*!
 * @brief Status register values and interrupt events.
 */
typedef struct
{
    uint32_t    status0Masked;          /*!< STATUS0 register value masked to only contain active interrupts via \ref adi_mac_Device_t.irqMask0.    */
    uint32_t    status1Masked;          /*!< STATUS1 register value masked to only contain active interrupts via \ref adi_mac_Device_t.irqMask1.    */
    uint32_t    status0;                /*!< Unmasked STATUS0 register value.                                                                       */
    uint32_t    status1;                /*!< Unmasked STATUS1 register value.                                                                       */
    uint32_t    p1StatusMasked;         /*!< PHY_SUBSYS_IRQ_STATUS and CRSM_IRQ_STATUS register values (the latter in LSBytes)
                                             masked to only contain active interrupts via \ref adi_mac_Device_t.phyIrqMask.                         */
    uint32_t    p1Status;               /*!< Unmasked PHY_SUBSYS_IRQ_STATUS and CRSM_IRQ_STATUS register values (the latter in LSBytes).            */
#if defined(ADIN2111)
    uint32_t    p2StatusMasked;         /*!< Port 2 PHY_SUBSYS_IRQ_STATUS and CRSM_IRQ_STATUS register values (the latter in LSBytes)
                                             masked to only contain active interrupts via \ref adi_mac_Device_t.phyIrqMask.                         */
    uint32_t    p2Status;               /*!< Unmasked Port 2 PHY_SUBSYS_IRQ_STATUS and CRSM_IRQ_STATUS register values (the latter in LSBytes).     */
#endif
} adi_mac_StatusRegisters_t;


/*!
 * @name MAC Callback Status
 * List of MAC callback status bits used ued by the buffer descriptor callback functions.
 */
/** @{ */
/*!
 * Status OK.
 */
#define ADI_MAC_CALLBACK_STATUS_OK          (0)
/*!
 * An FCS error was encountered. Note this is caused by validating the FCS on
 * the host MCU to check the integrity of the SPI transaction. The FCS of the
 * incoming frame is checked by the MAC hardware when the frame is received,
 * and has no effect on this status bit.
 */
#define ADI_MAC_CALLBACK_STATUS_FCS_ERROR   (1 << 0)
/*!
 * Buffer overflow on receive. The buffer submitted by the application is too small
 * to contain the full frame received.
 */
#define ADI_MAC_CALLBACK_STATUS_RX_BUF_OVF  (1 << 1)
/** @} */

/*!
 * @brief Frame structure.
 */
typedef struct
{
    adi_mac_FrameHeader_t   header;                 /*!< Frame header.                                  */
    adi_eth_BufDesc_t       *pBufDesc;              /*!< Pointer to the buffer descriptor.              */
} adi_mac_FrameStruct_t;

/*!
 * @brief Queue to hold frames.
 */
typedef struct
{
    adi_mac_FrameStruct_t       *pEntries;          /*!< Pointer to the frames in the queue.                */
    uint32_t                    numEntries;         /*!< Number of frames that can be held in the queue.    */
    volatile uint32_t           head;               /*!< Queue head index.                                  */
    volatile uint32_t           tail;               /*!< Queue tail index.                                  */
} adi_mac_Queue_t;



/*!
 * @brief Device descriptor structure.
 */
typedef struct
{
    volatile adi_mac_State_e        state;                                      /*!< MAC state.                                             */
    volatile adi_mac_SpiState_e     spiState;                                   /*!< SPI state.                                             */
    volatile uint32_t               spiErr;                                     /*!< Pointer to the high-level device ADINxxxx.             */
    adi_eth_Callback_t              cbFunc[ADI_MAC_EVT_MAX];                    /*!< Callback function pointers.                            */
    void                            *cbParam[ADI_MAC_EVT_MAX];                  /*!< Callback functions parameters.                         */
    void                            *adinDevice;                                /*!< Pointer to the high-level device ADINxxxx.             */
    uint32_t                        phyAddr;                                    /*!< Address of the PHY device.                             */
    uint16_t                        addrFilterActive;                           /*!< Mark address filter entries as active/inactive.        */
    uint32_t                        irqMask0;                                   /*!< Interrupt mask for the STATUS0 register.               */
    uint32_t                        irqMask1;                                   /*!< Interrupt mask for the STATUS1 register.               */
    adi_mac_FrameStruct_t           txQueueFrames[TX_QUEUE_NUM_ENTRIES_RAW];    /*!< Transmit frames held by \ref txQueue.                  */
    adi_mac_Queue_t                 txQueue;                                    /*!< Transmit frames queue.                                 */
    adi_mac_Queue_t                 *pRxQueue;                                  /*!< Pointer to active receive frames queue.                */
    adi_mac_FrameStruct_t           rxQueueLpFrames[RX_QUEUE_NUM_ENTRIES_RAW];  /*!< Receive frames held by \ref rxQueueLp.                 */
    adi_mac_Queue_t                 rxQueueLp;                                  /*!< Receive frames queue.                                  */
#if defined(ADI_MAC_ENABLE_RX_QUEUE_HI_PRIO)
    adi_mac_FrameStruct_t           rxQueueHpFrames[RX_QUEUE_NUM_ENTRIES_RAW];  /*!< High priority receive frames held by \ref rxQueueHp.   */
    adi_mac_Queue_t                 rxQueueHp;                                  /*!< High priority receive frames queue.                    */
#endif

    volatile bool                   pendingCtrl;                                /*!< Pending control transaction request flag.              */
    bool                            configSync;                                 /*!< Configuration has been synchronized.                   */
    bool                            fcsCheckEn;                                 /*!< Enable Frame Check Sequence validation on the host.    */
    adi_mac_TsFormat_e              timestampFormat;                            /*!< Format of ingress/egress timestamp.                    */
    adi_mac_StatusRegisters_t       statusRegisters;                            /*!< Status register values updated on interrupt assertion. */
    uint32_t                        phyIrqMask;                                 /*!< Interrupt mask for PHY status regsiters.               */

#if defined(SPI_OA_EN)
    /* Only used for OA SPI */
    uint32_t                        oaTxc;                                      /*!< TXC value read from the OA footer.                     */
    uint32_t                        oaRca;                                      /*!< RCA value read from the OA footer.                     */
    uint32_t                        oaTxCurBufByteOffset;                       /*!< Current byte offset in the OA Tx buffer.               */
    uint32_t                        oaRxCurBufByteOffset;                       /*!< Current byte offset in the OA Rx buffer.               */
    uint32_t                        oaTxCurBufIdx;                              /*!< Current Tx buffer index in the OA Tx queue.            */
    uint32_t                        oaRxCurBufIdx;                              /*!< Current Rx buffer index in the OA Rx queue.            */
    uint32_t                        oaCps;                                      /*!< OA Chunk Payload Selector currently configured.        */
    uint32_t                        oaMaxChunkCount;                            /*!< Maximum number of chunks in an OA SPI transaction.     */
    uint32_t                        oaTrxSize;                                  /*!< Latest/current OA transaction size, in bytes.          */
    bool                            oaTimestampSplit;                           /*!< Current 64b timestamp is split across OA chunks.       */
    uint8_t                         oaTimestampParity;                          /*!< RTSP value read form the OA footer.                    */
    adi_mac_OaValidFlag_e           oaValidFlag;                                /*!< Latest valid flag (EV or SV) from the OA footer.       */
    adi_mac_OaErrorStats_t          oaErrorStats;                               /*!< OA Error statistics.                                   */
    bool                            oaRxUseBackupBuf;                           /*!< Use OA Rx backup buffer (if Rx queue is empty).
                                                                                 *   Note this is not currently used!                       */
    uint32_t                        oaRxBufChunkStart;                          /*!< Start of the first unprocessed chunk when OA Rx
                                                                                 *   backup buffer engaged.
                                                                                 *   Note this is not currently used!                       */
    uint32_t                        oaRxBufTrxSize;                             /*!< Transaction size when OA Rx backup buffer engaged.
                                                                                 *   Note this is not currently used!                       */
    uint8_t                         oaRxBackupBuf[ADI_OA_RX_BACKUP_BUF_SIZE];   /*!< OA Rx backup buffer size, in bytes.
                                                                                 *   Note this is not currently used!                       */
    uint32_t                        wnr;                                        /*!< Write (1) or read (0) OA control transactions.         */
    uint32_t                        regAddr;                                    /*!< Register address for OA control transactions.          */
    uint32_t                        *pRegData;                                  /*!< Pointer to register data for OA control transactions.  */
    uint32_t                        cnt;                                        /*!< Register count for OA control transactions.            */
    uint8_t                         ctrlTxBuf[ADI_MAC_SPI_CTRL_BUF_SIZE];       /*!< Transmit (write) buffer for OA control transactions.   */
    uint8_t                         ctrlRxBuf[ADI_MAC_SPI_CTRL_BUF_SIZE];       /*!< Receive (read) buffer for OA control transactions.     */
#endif
} adi_mac_Device_t;

/** @}*/

/*! @cond PRIVATE */

typedef struct
{
    adi_eth_Result_e (*Init)                    (adi_mac_Device_t **hDevice, adi_mac_DriverConfig_t *cfg, void *adinDevice);
    adi_eth_Result_e (*UnInit)                  (adi_mac_Device_t *hDevice);
    adi_eth_Result_e (*Reset)                   (adi_mac_Device_t *hDevice, adi_eth_ResetType_e resetType);
    adi_eth_Result_e (*SyncConfig)              (adi_mac_Device_t *hDevice);
    adi_eth_Result_e (*RegisterCallback)        (adi_mac_Device_t *hDevice, adi_eth_Callback_t cbFunc, adi_mac_InterruptEvt_e cbEvent, void *cbParam);
    adi_eth_Result_e (*GetLinkStatus)           (adi_mac_Device_t *hDevice, uint32_t port, adi_eth_LinkStatus_e *linkStatus);
    adi_eth_Result_e (*GetStatCounters)         (adi_mac_Device_t *hDevice, uint32_t port, adi_eth_MacStatCounters_t *stat);
    adi_eth_Result_e (*AddAddressFilter)        (adi_mac_Device_t *hDevice, uint8_t *macAddr, uint8_t *macAddrMask, uint16_t addrRule);
    adi_eth_Result_e (*ClearAddressFilter)      (adi_mac_Device_t *hDevice, uint32_t addrIndex);
    adi_eth_Result_e (*SubmitTxBuffer)          (adi_mac_Device_t *hDevice, adi_mac_FrameHeader_t header, adi_eth_BufDesc_t *pBufDesc);
    adi_eth_Result_e (*SubmitRxBuffer)          (adi_mac_Device_t *hDevice, adi_eth_BufDesc_t *pBufDesc);
#if defined(ADI_MAC_ENABLE_RX_QUEUE_HI_PRIO)
    adi_eth_Result_e (*SubmitRxBufferHp)        (adi_mac_Device_t *hDevice, adi_eth_BufDesc_t *pBufDesc);
#endif
    adi_eth_Result_e (*SetPromiscuousMode)      (adi_mac_Device_t *hDevice, bool bFlag);
    adi_eth_Result_e (*GetPromiscuousMode)      (adi_mac_Device_t *hDevice, bool *pFlag);
#if defined(SPI_OA_EN)
    adi_eth_Result_e (*SetChunkSize)            (adi_mac_Device_t *hDevice, adi_mac_OaCps_e cps);
    adi_eth_Result_e (*GetChunkSize)            (adi_mac_Device_t *hDevice, adi_mac_OaCps_e *pCps);
#endif
    adi_eth_Result_e (*SetCutThroughMode)       (adi_mac_Device_t *hDevice, bool txcte, bool rxcte);
    adi_eth_Result_e (*GetCutThroughMode)       (adi_mac_Device_t *hDevice, bool *pTxcte, bool *pRxcte);
    adi_eth_Result_e (*SetFifoSizes)            (adi_mac_Device_t *hDevice, uint32_t writeVal);
    adi_eth_Result_e (*GetFifoSizes)            (adi_mac_Device_t *hDevice, uint32_t *readVal);
    adi_eth_Result_e (*ClearFifos)              (adi_mac_Device_t *hDevice, adi_mac_FifoClrMode_e clearMode);
    adi_eth_Result_e (*TsEnable)                (adi_mac_Device_t *hDevice, adi_mac_TsFormat_e format);
    adi_eth_Result_e (*TsClear)                 (adi_mac_Device_t *hDevice);
    adi_eth_Result_e (*TsTimerStart)            (adi_mac_Device_t *hDevice, adi_mac_TsTimerConfig_t* pTimerConfig);
    adi_eth_Result_e (*TsTimerStop)             (adi_mac_Device_t *hDevice);
    adi_eth_Result_e (*TsSetTimerAbsolute)      (adi_mac_Device_t *hDevice, uint32_t seconds, uint32_t nanoseconds);
    adi_eth_Result_e (*TsSyncClock)             (adi_mac_Device_t *hDevice, int64_t tError, uint64_t referenceTimeNsDiff, uint64_t localTimeNsDiff);
    adi_eth_Result_e (*TsGetExtCaptTimestamp)   (adi_mac_Device_t *hDevice, adi_mac_TsTimespec_t *pCapturedTimespec);
    adi_eth_Result_e (*TsGetEgressTimestamp)    (adi_mac_Device_t *hDevice, adi_mac_EgressCapture_e egressReg, adi_mac_TsTimespec_t *pCapturedTimespec);
    adi_eth_Result_e (*TsConvert)               (uint32_t timestampLowWord, uint32_t timestampHighWord, adi_mac_TsFormat_e format, adi_mac_TsTimespec_t *pTimespec);
    int64_t          (*TsSubtract)              (adi_mac_TsTimespec_t *pTsA, adi_mac_TsTimespec_t *pTsB);
    adi_eth_Result_e (*ReadRegister)            (adi_mac_Device_t *hDevice, uint16_t regAddr, uint32_t *regData);
    adi_eth_Result_e (*WriteRegister)           (adi_mac_Device_t *hDevice, uint16_t regAddr, uint32_t regData);
    adi_eth_Result_e (*PhyRead)                 (adi_mac_Device_t *hDevice,  uint8_t hwAddr, uint32_t regAddr, uint16_t *data);
    adi_eth_Result_e (*PhyWrite)                (adi_mac_Device_t *hDevice,  uint8_t hwAddr, uint32_t regAddr, uint16_t data);
} adi_mac_DriverEntry_t;

extern adi_mac_DriverEntry_t                    macDriverEntry;
void                                            spiCallback(void *pCBParam, uint32_t Event, void *pArg);

/* Forward declaration of functions used at MAC-level. */
adi_eth_Result_e        MAC_SendFrame               (adi_mac_Device_t *hDevice, adi_mac_FrameStruct_t *pFrame);
#if defined(SPI_OA_EN)
adi_eth_Result_e        oaStateMachine              (adi_mac_Device_t *hDevice);
void                    oaIrqHandler                (adi_mac_Device_t *hDevice);
#else
adi_eth_Result_e        MAC_Read                    (adi_mac_Device_t *hDevice, uint16_t regAddr, void *pBuf, uint32_t nBytes, bool blocking);
adi_eth_Result_e        MAC_Write                   (adi_mac_Device_t *hDevice, uint16_t regAddr, void *pBuf, uint32_t nBytes, bool blocking);
adi_eth_Result_e        MAC_RecvFrame               (adi_mac_Device_t *hDevice, uint32_t port);
#endif

adi_eth_Result_e        MAC_ReadRegister            (adi_mac_Device_t *hDevice, uint16_t regAddr, uint32_t *regData);
adi_eth_Result_e        MAC_WriteRegister           (adi_mac_Device_t *hDevice, uint16_t regAddr, uint32_t regData);

uint8_t                 MAC_CalculateParity         (uint8_t *p, uint32_t nBytes);

uint32_t                queueAvailable              (adi_mac_Queue_t *pQueue);
bool                    queueIsFull                 (adi_mac_Queue_t *pQueue);
bool                    queueIsEmpty                (adi_mac_Queue_t *pQueue);
void                    queueAdd                    (adi_mac_Queue_t *pQueue);
void                    queueRemove                 (adi_mac_Queue_t *pQueue);

/*! @endcond */

#ifdef __cplusplus
}
#endif

#endif /* ADI_MAC_H */
