
/* ================================================================================
     Project     :   MAC
     File        :   MAC_typedefs.h
     Description :   C typedef structures for bit-fields and enums for enumerations.


     Copyright (c) 2021 Analog Devices, Inc.  All Rights Reserved.
     This software is proprietary and confidential to Analog Devices, Inc. and
     its licensors.

     This file was auto-generated. Do not make local changes to this file.
   ================================================================================ */

#ifndef ADIN1110_MAC_TYPEDEFS_H
#define ADIN1110_MAC_TYPEDEFS_H

/* pickup integer types */
#if defined(_LANGUAGE_C) || (defined(__GNUC__) && !defined(__ASSEMBLER__))
#include <stdint.h>
#endif /* _LANGUAGE_C */

#if defined ( __CC_ARM   )
#pragma push
#pragma anon_unions
#endif


/** @defgroup IDVER Identification Version Register. (IDVER) Register
 *  Identification Version Register. (IDVER) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_IDVER_t
 *  \brief Identification Version Register.
 *  ======================================================================== */
typedef struct _ADI_MAC_IDVER_t {
    union {
        struct {
             uint32_t MINVER           : 4; /**< OA Minor Version. */
             uint32_t MAJVER           : 4; /**< OA Major Version. */
             uint32_t RESERVED8        : 24; /**< Reserved */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_IDVER_t;

/*@}*/

/** @defgroup PHYID PHY Identification Register. (PHYID) Register
 *  PHY Identification Register. (PHYID) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_PHYID_t
 *  \brief PHY Identification Register.
 *  ======================================================================== */
typedef struct _ADI_MAC_PHYID_t {
    union {
        struct {
             uint32_t REVISION         : 4; /**< Manufacturer’s Revision Number. */
             uint32_t MODEL            : 6; /**< Manufacturer’s Model Number. */
             uint32_t OUI              : 22; /**< Organizationally Unique Identifier (Bits 2:23). */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_PHYID_t;

/*@}*/

/** @defgroup CAPABILITY Supported Capabilities Register. (CAPABILITY) Register
 *  Supported Capabilities Register. (CAPABILITY) Register
 *  @{
 */

/*! ========================================================================
 *  \enum ADI_MAC_CAPABILITY_MINCPS
 *  \brief Minimum Supported Chunk Payload Size.
 *  ======================================================================== */
typedef enum
{
     MAC_CAPABILITY_MINCPS_CPS8       = 3,    /**< Minimum Supported Chunk Payload Size is 8 Bytes. */
     MAC_CAPABILITY_MINCPS_CPS16      = 4,    /**< Minimum Supported Chunk Payload Size is 16 Bytes. */
     MAC_CAPABILITY_MINCPS_CPS32      = 5,    /**< Minimum Supported Chunk Payload Size is 32 Bytes. */
     MAC_CAPABILITY_MINCPS_CPS64      = 6     /**< Minimum Supported Chunk Payload Size is 64 Bytes. */
}  ADI_MAC_CAPABILITY_MINCPS;

/*! ========================================================================
 *  \enum ADI_MAC_CAPABILITY_SEQC
 *  \brief TX Data Chunk Sequence and Retry Capability.
 *  ======================================================================== */
typedef enum
{
     MAC_CAPABILITY_SEQC_SEQC_0       = 0,    /**< TX Data Chunk Sequence and Retry is Not Supported. */
     MAC_CAPABILITY_SEQC_SEQC_1       = 1     /**< TX Data Chunk Sequence and Retry is Supported. */
}  ADI_MAC_CAPABILITY_SEQC;

/*! ========================================================================
 *  \enum ADI_MAC_CAPABILITY_FTSC
 *  \brief Frame Timestamp Capability.
 *  ======================================================================== */
typedef enum
{
     MAC_CAPABILITY_FTSC_FTSC_0       = 0,    /**< IEEE 1588 Timestamp Capture on Frame Tx/Rx is Not Supported. */
     MAC_CAPABILITY_FTSC_FTSC_1       = 1     /**< IEEE 1588 Timestamp Capture on Frame Tx/Rx is Supported. */
}  ADI_MAC_CAPABILITY_FTSC;

/*! ========================================================================
 *  \enum ADI_MAC_CAPABILITY_CTC
 *  \brief Cut-Through Capability.
 *  ======================================================================== */
typedef enum
{
     MAC_CAPABILITY_CTC_CTC_0         = 0,    /**< Cut-Through Not Supported. */
     MAC_CAPABILITY_CTC_CTC_1         = 1     /**< Cut-Through Supported. */
}  ADI_MAC_CAPABILITY_CTC;

/*! ========================================================================
 *  \enum ADI_MAC_CAPABILITY_DPRAC
 *  \brief Direct PHY Register Access Capability.
 *  ======================================================================== */
typedef enum
{
     MAC_CAPABILITY_DPRAC_DPRAC_0     = 0,    /**< PHY Registers are Not Directly Accessible. */
     MAC_CAPABILITY_DPRAC_DPRAC_1     = 1     /**< PHY Registers are Directly Accessible. */
}  ADI_MAC_CAPABILITY_DPRAC;

/*! ========================================================================
 *  \enum ADI_MAC_CAPABILITY_IPRAC
 *  \brief Indirect PHY Register Access Capability.
 *  ======================================================================== */
typedef enum
{
     MAC_CAPABILITY_IPRAC_IPRAC_0     = 0,    /**< PHY Registers are Not Indirectly Accessible. */
     MAC_CAPABILITY_IPRAC_IPRAC_1     = 1     /**< PHY Registers are Indirectly Accessible. */
}  ADI_MAC_CAPABILITY_IPRAC;

/*! ========================================================================
 *  \enum ADI_MAC_CAPABILITY_TXFCSVC
 *  \brief Transmit Frame Check Sequence Validation Capability.
 *  ======================================================================== */
typedef enum
{
     MAC_CAPABILITY_TXFCSVC_TXFCSVC_0 = 0,    /**< Transmit FCS Validation is Not Supported. */
     MAC_CAPABILITY_TXFCSVC_TXFCSVC_1 = 1     /**< Transmit FCS Validation is Supported. */
}  ADI_MAC_CAPABILITY_TXFCSVC;

/*! ========================================================================
 *  \struct ADI_MAC_CAPABILITY_t
 *  \brief Supported Capabilities Register.
 *  ======================================================================== */
typedef struct _ADI_MAC_CAPABILITY_t {
    union {
        struct {
             uint32_t MINCPS           : 3; /**< Minimum Supported Chunk Payload Size. */
             uint32_t RESERVED3        : 1; /**< Reserved */
             uint32_t SEQC             : 1; /**< TX Data Chunk Sequence and Retry Capability. */
             uint32_t AIDC             : 1; /**< Address Increment Disable Capability. */
             uint32_t FTSC             : 1; /**< Frame Timestamp Capability. */
             uint32_t CTC              : 1; /**< Cut-Through Capability. */
             uint32_t DPRAC            : 1; /**< Direct PHY Register Access Capability. */
             uint32_t IPRAC            : 1; /**< Indirect PHY Register Access Capability. */
             uint32_t TXFCSVC          : 1; /**< Transmit Frame Check Sequence Validation Capability. */
             uint32_t RESERVED11       : 21; /**< Reserved */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_CAPABILITY_t;

/*@}*/

/** @defgroup RESET Reset Control and Status Register. (RESET) Register
 *  Reset Control and Status Register. (RESET) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_RESET_t
 *  \brief Reset Control and Status Register.
 *  ======================================================================== */
typedef struct _ADI_MAC_RESET_t {
    union {
        struct {
             uint32_t SWRESET          : 1; /**< Software Reset. */
             uint32_t RESERVED1        : 31; /**< Reserved */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_RESET_t;

/*@}*/

/** @defgroup CONFIG0 Configuration Register 0. (CONFIG0) Register
 *  Configuration Register 0. (CONFIG0) Register
 *  @{
 */

/*! ========================================================================
 *  \enum ADI_MAC_CONFIG0_CPS
 *  \brief Chunk Payload Selector (N).
 *  ======================================================================== */
typedef enum
{
     MAC_CONFIG0_CPS_CHUNK_8BYTE      = 3,    /**< No description provided */
     MAC_CONFIG0_CPS_CHUNK_16BYTE     = 4,    /**< No description provided */
     MAC_CONFIG0_CPS_CHUNK_32BYTE     = 5,    /**< No description provided */
     MAC_CONFIG0_CPS_CHUNK_64BYTE     = 6     /**< No description provided */
}  ADI_MAC_CONFIG0_CPS;

/*! ========================================================================
 *  \enum ADI_MAC_CONFIG0_FTSS
 *  \brief Receive Frame Timestamp Select.
 *  ======================================================================== */
typedef enum
{
     MAC_CONFIG0_FTSS_FTSS_0          = 0,    /**< 32-bit Timestamps. */
     MAC_CONFIG0_FTSS_FTSS_1          = 1     /**< 64-bit Timestamps. */
}  ADI_MAC_CONFIG0_FTSS;

/*! ========================================================================
 *  \enum ADI_MAC_CONFIG0_FTSE
 *  \brief Frame Timestamp Enable.
 *  ======================================================================== */
typedef enum
{
     MAC_CONFIG0_FTSE_FTSE_0          = 0,    /**< Frame Receive/Transmit Timestamps are Disabled. */
     MAC_CONFIG0_FTSE_FTSE_1          = 1     /**< Frame Receive/Transmit Timestamps are Enabled. */
}  ADI_MAC_CONFIG0_FTSE;

/*! ========================================================================
 *  \enum ADI_MAC_CONFIG0_TXCTHRESH
 *  \brief Transmit Credit Threshold.
 *  ======================================================================== */
typedef enum
{
     MAC_CONFIG0_TXCTHRESH_CREDIT_1   = 0,    /**< ≥ 1 Credit. */
     MAC_CONFIG0_TXCTHRESH_CREDIT_4   = 1,    /**< ≥ 4 Credits. */
     MAC_CONFIG0_TXCTHRESH_CREDIT_8   = 2,    /**< ≥ 8 Credits. */
     MAC_CONFIG0_TXCTHRESH_CREDIT_16  = 3     /**< ≥ 16 Credits. */
}  ADI_MAC_CONFIG0_TXCTHRESH;

/*! ========================================================================
 *  \enum ADI_MAC_CONFIG0_SYNC
 *  \brief Configuration Synchronization.
 *  ======================================================================== */
typedef enum
{
     MAC_CONFIG0_SYNC_SYNC_0          = 0,    /**< The MACPHY Has Been Reset and is Not Configured. */
     MAC_CONFIG0_SYNC_SYNC_1          = 1     /**< The MACPHY is Configured. */
}  ADI_MAC_CONFIG0_SYNC;

/*! ========================================================================
 *  \struct ADI_MAC_CONFIG0_t
 *  \brief Configuration Register 0.
 *  ======================================================================== */
typedef struct _ADI_MAC_CONFIG0_t {
    union {
        struct {
             uint32_t CPS              : 3; /**< Chunk Payload Selector (N). */
             uint32_t RESERVED3        : 1; /**< Reserved */
             uint32_t SEQE             : 1; /**< Enable TX Data Chunk Sequence and Retry. */
             uint32_t PROTE            : 1; /**< Enable Control Data Read Write Protection. */
             uint32_t FTSS             : 1; /**< Receive Frame Timestamp Select. */
             uint32_t FTSE             : 1; /**< Frame Timestamp Enable. */
             uint32_t RXCTE            : 1; /**< Receive Cut-Through Enable. */
             uint32_t TXCTE            : 1; /**< Transmit Cut-Through Enable. */
             uint32_t TXCTHRESH        : 2; /**< Transmit Credit Threshold. */
             uint32_t ZARFE            : 1; /**< Zero-Align Receive Frame Enable. */
             uint32_t CSARFE           : 1; /**< CSn Align Receive Frame Enable. */
             uint32_t TXFCSVE          : 1; /**< Transmit Frame Check Sequence Validation Enable. */
             uint32_t SYNC             : 1; /**< Configuration Synchronization. */
             uint32_t RESERVED16       : 16; /**< Reserved */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_CONFIG0_t;

/*@}*/

/** @defgroup CONFIG2 Configuration Register 2. (CONFIG2) Register
 *  Configuration Register 2. (CONFIG2) Register
 *  @{
 */

/*! ========================================================================
 *  \enum ADI_MAC_CONFIG2_MSPEED
 *  \brief SPI to MDIO Bridge MDC Clock Speed.
 *  ======================================================================== */
typedef enum
{
     MAC_CONFIG2_MSPEED_MHZ_2P5       = 0,    /**< 2.5 MHz. */
     MAC_CONFIG2_MSPEED_MHZ_4P166     = 1     /**< 4.166 MHz. */
}  ADI_MAC_CONFIG2_MSPEED;

/*! ========================================================================
 *  \enum ADI_MAC_CONFIG2_STATS_CLR_ON_RD
 *  \brief Statistics Clear on Reading.
 *  ======================================================================== */
typedef enum
{
     MAC_CONFIG2_STATS_CLR_ON_RD_HOLD_STAT = 0,    /**< Statistic Counter is Not Cleared on Reading. */
     MAC_CONFIG2_STATS_CLR_ON_RD_CLR_STAT = 1     /**< Clear Statistics Counters on Reading. */
}  ADI_MAC_CONFIG2_STATS_CLR_ON_RD;

/*! ========================================================================
 *  \enum ADI_MAC_CONFIG2_SFD_DETECT_SRC
 *  \brief Determines If the SFD is Detected in the PHY or MAC.
 *  ======================================================================== */
typedef enum
{
     MAC_CONFIG2_SFD_DETECT_SRC_PHY   = 0,    /**< Select the SFD from the PHY. */
     MAC_CONFIG2_SFD_DETECT_SRC_MAC   = 1     /**< Select the SFD from the MAC. */
}  ADI_MAC_CONFIG2_SFD_DETECT_SRC;

/*! ========================================================================
 *  \struct ADI_MAC_CONFIG2_t
 *  \brief Configuration Register 2.
 *  ======================================================================== */
typedef struct _ADI_MAC_CONFIG2_t {
    union {
        struct {
             uint32_t MSPEED           : 2; /**< SPI to MDIO Bridge MDC Clock Speed. */
             uint32_t P1_FWD_UNK2HOST  : 1; /**< Forward Frames Not Matching Any MAC Address to the Host. */
             uint32_t RESERVED3        : 1; /**< Reserved */
             uint32_t P1_RCV_IFG_ERR_FRM : 1; /**< Admit Frames with IFG Errors on P1. */
             uint32_t CRC_APPEND       : 1; /**< Enable CRC Append. */
             uint32_t STATS_CLR_ON_RD  : 1; /**< Statistics Clear on Reading. */
             uint32_t SFD_DETECT_SRC   : 1; /**< Determines If the SFD is Detected in the PHY or MAC. */
             uint32_t TX_RDY_ON_EMPTY  : 1; /**< Assert TX_RDY When the Tx FIFO is Empty. */
             uint32_t RESERVED9        : 23; /**< Reserved */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_CONFIG2_t;

/*@}*/

/** @defgroup STATUS0 Status Register 0. (STATUS0) Register
 *  Status Register 0. (STATUS0) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_STATUS0_t
 *  \brief Status Register 0.
 *  ======================================================================== */
typedef struct _ADI_MAC_STATUS0_t {
    union {
        struct {
             uint32_t TXPE             : 1; /**< Transmit Protocol Error. */
             uint32_t TXBOE            : 1; /**< Host Tx FIFO Overflow. */
             uint32_t TXBUE            : 1; /**< Host Tx FIFO Underrun Error. */
             uint32_t RXBOE            : 1; /**< Receive Buffer Overflow Error. */
             uint32_t LOFE             : 1; /**< Loss of Frame Error. */
             uint32_t HDRE             : 1; /**< Header Error. */
             uint32_t RESETC           : 1; /**< Reset Complete. */
             uint32_t PHYINT           : 1; /**< PHY Interrupt for Port1. */
             uint32_t TTSCAA           : 1; /**< Transmit Timestamp Capture Available A. */
             uint32_t TTSCAB           : 1; /**< Transmit Timestamp Capture Available B. */
             uint32_t TTSCAC           : 1; /**< Transmit Timestamp Capture Available C. */
             uint32_t TXFCSE           : 1; /**< Transmit Frame Check Sequence Error. */
             uint32_t CDPE             : 1; /**< Control Data Protection Error. */
             uint32_t RESERVED13       : 19; /**< Reserved */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_STATUS0_t;

/*@}*/

/** @defgroup STATUS1 Status Register 1. (STATUS1) Register
 *  Status Register 1. (STATUS1) Register
 *  @{
 */

/*! ========================================================================
 *  \enum ADI_MAC_STATUS1_P1_LINK_STATUS
 *  \brief Port 1 Link Status.
 *  ======================================================================== */
typedef enum
{
     MAC_STATUS1_P1_LINK_STATUS_DOWN  = 0,    /**< Link Down. */
     MAC_STATUS1_P1_LINK_STATUS_UP    = 1     /**< Link up. */
}  ADI_MAC_STATUS1_P1_LINK_STATUS;

/*! ========================================================================
 *  \struct ADI_MAC_STATUS1_t
 *  \brief Status Register 1.
 *  ======================================================================== */
typedef struct _ADI_MAC_STATUS1_t {
    union {
        struct {
             uint32_t P1_LINK_STATUS   : 1; /**< Port 1 Link Status. */
             uint32_t LINK_CHANGE      : 1; /**< Link Status Changed. */
             uint32_t RESERVED2        : 1; /**< Reserved */
             uint32_t TX_RDY           : 1; /**< Tx Ready. */
             uint32_t P1_RX_RDY        : 1; /**< Port 1 Rx FIFO Contains Data. */
             uint32_t P1_RX_RDY_HI     : 1; /**< Port1 Rx Ready High Priority. */
             uint32_t RESERVED6        : 2; /**< Reserved */
             uint32_t P1_RX_IFG_ERR    : 1; /**< Rx MAC Inter Frame Gap Error. */
             uint32_t RESERVED9        : 1; /**< Reserved */
             uint32_t SPI_ERR          : 1; /**< Detected an Error on an SPI Transaction. */
             uint32_t RX_ECC_ERR       : 1; /**< ECC Error on Reading the Frame Size from an Rx FIFO. */
             uint32_t TX_ECC_ERR       : 1; /**< ECC Error on Reading the Frame Size from a Tx FIFO. */
             uint32_t RESERVED13       : 19; /**< Reserved */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_STATUS1_t;

/*@}*/

/** @defgroup BUFSTS Buffer Status Register. (BUFSTS) Register
 *  Buffer Status Register. (BUFSTS) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_BUFSTS_t
 *  \brief Buffer Status Register.
 *  ======================================================================== */
typedef struct _ADI_MAC_BUFSTS_t {
    union {
        struct {
             uint32_t RCA              : 8; /**< Receive Chunks Available. */
             uint32_t TXC              : 8; /**< Transmit Credits Available. */
             uint32_t RESERVED16       : 16; /**< Reserved */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_BUFSTS_t;

/*@}*/

/** @defgroup IMASK0 Interrupt Mask Register 0. (IMASK0) Register
 *  Interrupt Mask Register 0. (IMASK0) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_IMASK0_t
 *  \brief Interrupt Mask Register 0.
 *  ======================================================================== */
typedef struct _ADI_MAC_IMASK0_t {
    union {
        struct {
             uint32_t TXPEM            : 1; /**< Transmit Protocol Error Mask. */
             uint32_t TXBOEM           : 1; /**< Transmit Buffer Overflow Error Mask. */
             uint32_t TXBUEM           : 1; /**< Transmit Buffer Underflow Error Mask. */
             uint32_t RXBOEM           : 1; /**< Receive Buffer Overflow Error Mask. */
             uint32_t LOFEM            : 1; /**< Loss of Frame Error Mask. */
             uint32_t HDREM            : 1; /**< Header Error Mask. */
             uint32_t RESETCM          : 1; /**< RESET Complete Mask. */
             uint32_t PHYINTM          : 1; /**< Physical Layer Interrupt Mask. */
             uint32_t TTSCAAM          : 1; /**< Transmit Timestamp Capture Available A Mask. */
             uint32_t TTSCABM          : 1; /**< Transmit Timestamp Capture Available B Mask. */
             uint32_t TTSCACM          : 1; /**< Transmit Timestamp Capture Available C Mask. */
             uint32_t TXFCSEM          : 1; /**< Transmit Frame Check Sequence Error Mask. */
             uint32_t CDPEM            : 1; /**< Control Data Protection Error Mask. */
             uint32_t RESERVED13       : 19; /**< Reserved */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_IMASK0_t;

/*@}*/

/** @defgroup IMASK1 Mask Bits for Driving the Interrupt Pin Register. (IMASK1) Register
 *  Mask Bits for Driving the Interrupt Pin Register. (IMASK1) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_IMASK1_t
 *  \brief Mask Bits for Driving the Interrupt Pin Register.
 *  ======================================================================== */
typedef struct _ADI_MAC_IMASK1_t {
    union {
        struct {
             uint32_t RESERVED0        : 1; /**< Reserved */
             uint32_t LINK_CHANGE_MASK : 1; /**< Mask Bit for LINK_CHANGE. */
             uint32_t RESERVED2        : 1; /**< Reserved */
             uint32_t TX_RDY_MASK      : 1; /**< Mask Bit for TX_FRM_DONE. */
             uint32_t P1_RX_RDY_MASK   : 1; /**< Mask Bit for P1_RX_RDY. */
             uint32_t RESERVED5        : 3; /**< Reserved */
             uint32_t P1_RX_IFG_ERR_MASK : 1; /**< Mask Bit for RX_IFG_ERR. */
             uint32_t RESERVED9        : 1; /**< Reserved */
             uint32_t SPI_ERR_MASK     : 1; /**< Mask Bit for SPI_ERR. */
             uint32_t RX_ECC_ERR_MASK  : 1; /**< Mask Bit for RXF_ECC_ERR. */
             uint32_t TX_ECC_ERR_MASK  : 1; /**< Mask Bit for TXF_ECC_ERR. */
             uint32_t RESERVED13       : 19; /**< Reserved */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_IMASK1_t;

/*@}*/

/** @defgroup TTSCAH Transmit Timestamp Capture Register A (High). (TTSCAH) Register
 *  Transmit Timestamp Capture Register A (High). (TTSCAH) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_TTSCAH_t
 *  \brief Transmit Timestamp Capture Register A (High).
 *  ======================================================================== */
typedef struct _ADI_MAC_TTSCAH_t {
    union {
        struct {
             uint32_t TTSCH_A          : 32; /**< Transmit Timestamp A Bits 63-32. */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_TTSCAH_t;

/*@}*/

/** @defgroup TTSCAL Transmit Timestamp Capture Register A (Low). (TTSCAL) Register
 *  Transmit Timestamp Capture Register A (Low). (TTSCAL) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_TTSCAL_t
 *  \brief Transmit Timestamp Capture Register A (Low).
 *  ======================================================================== */
typedef struct _ADI_MAC_TTSCAL_t {
    union {
        struct {
             uint32_t TTSCL_A          : 32; /**< Transmit Timestamp A Bits 31-0. */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_TTSCAL_t;

/*@}*/

/** @defgroup TTSCBH Transmit Timestamp Capture Register B (High). (TTSCBH) Register
 *  Transmit Timestamp Capture Register B (High). (TTSCBH) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_TTSCBH_t
 *  \brief Transmit Timestamp Capture Register B (High).
 *  ======================================================================== */
typedef struct _ADI_MAC_TTSCBH_t {
    union {
        struct {
             uint32_t TTSCH_B          : 32; /**< Transmit Timestamp B Bits 63-32. */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_TTSCBH_t;

/*@}*/

/** @defgroup TTSCBL Transmit Timestamp Capture Register B (Low). (TTSCBL) Register
 *  Transmit Timestamp Capture Register B (Low). (TTSCBL) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_TTSCBL_t
 *  \brief Transmit Timestamp Capture Register B (Low).
 *  ======================================================================== */
typedef struct _ADI_MAC_TTSCBL_t {
    union {
        struct {
             uint32_t TTSCL_B          : 32; /**< Transmit Timestamp B Bits 31-0. */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_TTSCBL_t;

/*@}*/

/** @defgroup TTSCCH Transmit Timestamp Capture Register C (High). (TTSCCH) Register
 *  Transmit Timestamp Capture Register C (High). (TTSCCH) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_TTSCCH_t
 *  \brief Transmit Timestamp Capture Register C (High).
 *  ======================================================================== */
typedef struct _ADI_MAC_TTSCCH_t {
    union {
        struct {
             uint32_t TTSCH_C          : 32; /**< Transmit Timestamp C Bits 63-32. */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_TTSCCH_t;

/*@}*/

/** @defgroup TTSCCL Transmit Timestamp Capture Register C (Low). (TTSCCL) Register
 *  Transmit Timestamp Capture Register C (Low). (TTSCCL) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_TTSCCL_t
 *  \brief Transmit Timestamp Capture Register C (Low).
 *  ======================================================================== */
typedef struct _ADI_MAC_TTSCCL_t {
    union {
        struct {
             uint32_t TTSCL_C          : 32; /**< Transmit Timestamp C Bits 31-0. */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_TTSCCL_t;

/*@}*/

/** @defgroup MDIOACC_0_ MDIO Access Registers. (MDIOACC_0_) Register
 *  MDIO Access Registers. (MDIOACC_0_) Register
 *  @{
 */

/*! ========================================================================
 *  \enum ADI_MAC_MDIOACC_0__MDIO_OP
 *  \brief Operation Code.
 *  ======================================================================== */
typedef enum
{
     MAC_MDIOACC_0__MDIO_OP_MD_ADDR   = 0,    /**< MD Address Command. */
     MAC_MDIOACC_0__MDIO_OP_MD_WR     = 1,    /**< Write Command. */
     MAC_MDIOACC_0__MDIO_OP_MD_INC_RD = 2,    /**< Incremental Read Command. */
     MAC_MDIOACC_0__MDIO_OP_MD_RD     = 3     /**< Read Command. */
}  ADI_MAC_MDIOACC_0__MDIO_OP;

/*! ========================================================================
 *  \enum ADI_MAC_MDIOACC_0__MDIO_ST
 *  \brief Start of Frame.
 *  ======================================================================== */
typedef enum
{
     MAC_MDIOACC_0__MDIO_ST_CLAUSE45  = 0,    /**< No description provided */
     MAC_MDIOACC_0__MDIO_ST_CLAUSE22  = 1     /**< No description provided */
}  ADI_MAC_MDIOACC_0__MDIO_ST;

/*! ========================================================================
 *  \struct ADI_MAC_MDIOACC_0__t
 *  \brief MDIO Access Registers.
 *  ======================================================================== */
typedef struct _ADI_MAC_MDIOACC_0__t {
    union {
        struct {
             uint32_t MDIO_DATA        : 16; /**< Data/Address Value. */
             uint32_t MDIO_DEVAD       : 5; /**< MDIO Device Address. */
             uint32_t MDIO_PRTAD       : 5; /**< MDIO Port Address. */
             uint32_t MDIO_OP          : 2; /**< Operation Code. */
             uint32_t MDIO_ST          : 2; /**< Start of Frame. */
             uint32_t MDIO_TAERR       : 1; /**< Turnaround Error. */
             uint32_t MDIO_TRDONE      : 1; /**< Transaction Done. */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_MDIOACC_0__t;

/*@}*/

/** @defgroup TX_FSIZE MAC Tx Frame Size Register. (TX_FSIZE) Register
 *  MAC Tx Frame Size Register. (TX_FSIZE) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_TX_FSIZE_t
 *  \brief MAC Tx Frame Size Register.
 *  ======================================================================== */
typedef struct _ADI_MAC_TX_FSIZE_t {
    union {
        struct {
             uint32_t TX_FRM_SIZE      : 11; /**< Transmit Frame Size. */
             uint32_t RESERVED11       : 21; /**< Reserved */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_TX_FSIZE_t;

/*@}*/

/** @defgroup TX MAC Transmit Register. (TX) Register
 *  MAC Transmit Register. (TX) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_TX_t
 *  \brief MAC Transmit Register.
 *  ======================================================================== */
typedef struct _ADI_MAC_TX_t {
    union {
        struct {
             uint32_t TDR              : 32; /**< Transmit Data Register. */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_TX_t;

/*@}*/

/** @defgroup TX_SPACE Tx FIFO Space Register. (TX_SPACE) Register
 *  Tx FIFO Space Register. (TX_SPACE) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_TX_SPACE_t
 *  \brief Tx FIFO Space Register.
 *  ======================================================================== */
typedef struct _ADI_MAC_TX_SPACE_t {
    union {
        struct {
             uint32_t TX_SPACE         : 14; /**< Transmit FIFO Space Available in Half Words (16 Bits). */
             uint32_t RESERVED14       : 18; /**< Reserved */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_TX_SPACE_t;

/*@}*/

/** @defgroup TX_THRESH Transmit Threshold Register. (TX_THRESH) Register
 *  Transmit Threshold Register. (TX_THRESH) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_TX_THRESH_t
 *  \brief Transmit Threshold Register.
 *  ======================================================================== */
typedef struct _ADI_MAC_TX_THRESH_t {
    union {
        struct {
             uint32_t HOST_TX_THRESH   : 6; /**< Host Transmit Start Threshold in Cut Through. */
             uint32_t RESERVED6        : 26; /**< Reserved */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_TX_THRESH_t;

/*@}*/

/** @defgroup FIFO_CLR MAC FIFO Clear Register. (FIFO_CLR) Register
 *  MAC FIFO Clear Register. (FIFO_CLR) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_FIFO_CLR_t
 *  \brief MAC FIFO Clear Register.
 *  ======================================================================== */
typedef struct _ADI_MAC_FIFO_CLR_t {
    union {
        struct {
             uint32_t MAC_RXF_CLR      : 1; /**< Clear the Receive FIFO(s). */
             uint32_t MAC_TXF_CLR      : 1; /**< Clear the Host Transmit FIFO. */
             uint32_t RESERVED2        : 30; /**< Reserved */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_FIFO_CLR_t;

/*@}*/

/** @defgroup SCRATCH_0_ Scratch Registers. (SCRATCH_0_) Register
 *  Scratch Registers. (SCRATCH_0_) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_SCRATCH_0__t
 *  \brief Scratch Registers.
 *  ======================================================================== */
typedef struct _ADI_MAC_SCRATCH_0__t {
    union {
        struct {
             uint32_t SCRATCH_DATA     : 32; /**< Scratch Data. */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_SCRATCH_0__t;

/*@}*/

/** @defgroup MAC_RST_STATUS MAC Reset Status. (MAC_RST_STATUS) Register
 *  MAC Reset Status. (MAC_RST_STATUS) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_MAC_RST_STATUS_t
 *  \brief MAC Reset Status.
 *  ======================================================================== */
typedef struct _ADI_MAC_MAC_RST_STATUS_t {
    union {
        struct {
             uint32_t MAC_OSC_CLK_RDY  : 1; /**< MAC Oscillator Clock Ready. */
             uint32_t MAC_CRYSL_CLK_RDY : 1; /**< MAC Crystal Clock Ready. */
             uint32_t RESERVED2        : 30; /**< Reserved */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_MAC_RST_STATUS_t;

/*@}*/

/** @defgroup SOFT_RST Software Reset Register. (SOFT_RST) Register
 *  Software Reset Register. (SOFT_RST) Register
 *  @{
 */

/*! ========================================================================
 *  \enum ADI_MAC_SOFT_RST_RST_KEY
 *  \brief Software Reset.
 *  ======================================================================== */
typedef enum
{
     MAC_SOFT_RST_RST_KEY_RST_MAC_ONLY_KEY1 = 20252,    /**< Key1 to Reset the MAC Logic Only. */
     MAC_SOFT_RST_RST_KEY_MAC_RST_EXIT_REQ_KEY1 = 28442,    /**< Key 1 to Request Release of Reset to the MAC Core Logic. */
     MAC_SOFT_RST_RST_KEY_MAC_RST_EXIT_REQ_KEY2 = 41462,    /**< Key 2 to Request Release of Reset to the MAC Core Logic. */
     MAC_SOFT_RST_RST_KEY_RST_MAC_ONLY_KEY2 = 49652     /**< Key 2 to Reset the MAC Logic Only. */
}  ADI_MAC_SOFT_RST_RST_KEY;

/*! ========================================================================
 *  \struct ADI_MAC_SOFT_RST_t
 *  \brief Software Reset Register.
 *  ======================================================================== */
typedef struct _ADI_MAC_SOFT_RST_t {
    union {
        struct {
             uint32_t RST_KEY          : 16; /**< Software Reset. */
             uint32_t RESERVED16       : 16; /**< Reserved */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_SOFT_RST_t;

/*@}*/

/** @defgroup SPI_INJ_ERR Inject an Error on MISO from the DUT. (SPI_INJ_ERR) Register
 *  Inject an Error on MISO from the DUT. (SPI_INJ_ERR) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_SPI_INJ_ERR_t
 *  \brief Inject an Error on MISO from the DUT.
 *  ======================================================================== */
typedef struct _ADI_MAC_SPI_INJ_ERR_t {
    union {
        struct {
             uint32_t TEST_SPI_INJ_ERR : 1; /**< Inject an Error on the SPI MISO Path. */
             uint32_t RESERVED1        : 31; /**< Reserved */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_SPI_INJ_ERR_t;

/*@}*/

/** @defgroup FIFO_SIZE FIFO Sizes Register. (FIFO_SIZE) Register
 *  FIFO Sizes Register. (FIFO_SIZE) Register
 *  @{
 */

/*! ========================================================================
 *  \enum ADI_MAC_FIFO_SIZE_HTX_SIZE
 *  \brief Host Transmit FIFO Size.
 *  ======================================================================== */
typedef enum
{
     MAC_FIFO_SIZE_HTX_SIZE_TXSIZE_0K = 0,    /**< 0k Bytes. */
     MAC_FIFO_SIZE_HTX_SIZE_TXSIZE_2K = 1,    /**< 2k Bytes. */
     MAC_FIFO_SIZE_HTX_SIZE_TXSIZE_4K = 2,    /**< 4k Bytes. */
     MAC_FIFO_SIZE_HTX_SIZE_TXSIZE_6K = 3,    /**< 6k Bytes. */
     MAC_FIFO_SIZE_HTX_SIZE_TXSIZE_8K = 4,    /**< 8k Bytes. */
     MAC_FIFO_SIZE_HTX_SIZE_TXSIZE_10K = 5,    /**< 10k Bytes. */
     MAC_FIFO_SIZE_HTX_SIZE_TXSIZE_12K = 6,    /**< 12k Bytes. */
     MAC_FIFO_SIZE_HTX_SIZE_TXSIZE_14K = 7,    /**< 14k Bytes. */
     MAC_FIFO_SIZE_HTX_SIZE_TXSIZE_16K = 8     /**< 16k Byte. */
}  ADI_MAC_FIFO_SIZE_HTX_SIZE;

/*! ========================================================================
 *  \enum ADI_MAC_FIFO_SIZE_P1_RX_LO_SIZE
 *  \brief Port 1 Rx Low Priority FIFO Size.
 *  ======================================================================== */
typedef enum
{
     MAC_FIFO_SIZE_P1_RX_LO_SIZE_RXSIZE_0K = 0,    /**< 0K Bytes. */
     MAC_FIFO_SIZE_P1_RX_LO_SIZE_RXSIZE_2K = 1,    /**< 2k Bytes. */
     MAC_FIFO_SIZE_P1_RX_LO_SIZE_RXSIZE_4K = 2,    /**< 4k Bytes. */
     MAC_FIFO_SIZE_P1_RX_LO_SIZE_RXSIZE_6K = 3,    /**< 6k Bytes. */
     MAC_FIFO_SIZE_P1_RX_LO_SIZE_RXSIZE_8K = 4,    /**< 8k Bytes. */
     MAC_FIFO_SIZE_P1_RX_LO_SIZE_RXSIZE_10K = 5,    /**< 10k Bytes. */
     MAC_FIFO_SIZE_P1_RX_LO_SIZE_RXSIZE_12K = 6,    /**< 12k Bytes. */
     MAC_FIFO_SIZE_P1_RX_LO_SIZE_RXSIZE_14K = 7,    /**< 14k Bytes. */
     MAC_FIFO_SIZE_P1_RX_LO_SIZE_RXSIZE_16K = 8     /**< 16k Bytes. */
}  ADI_MAC_FIFO_SIZE_P1_RX_LO_SIZE;

/*! ========================================================================
 *  \enum ADI_MAC_FIFO_SIZE_P1_RX_HI_SIZE
 *  \brief Port 1 Rx High Priority FIFO Size.
 *  ======================================================================== */
typedef enum
{
     MAC_FIFO_SIZE_P1_RX_HI_SIZE_RXSIZE_0K = 0,    /**< 0K Bytes. */
     MAC_FIFO_SIZE_P1_RX_HI_SIZE_RXSIZE_2K = 1,    /**< 2K Bytes. */
     MAC_FIFO_SIZE_P1_RX_HI_SIZE_RXSIZE_4K = 2,    /**< 4k Bytes. */
     MAC_FIFO_SIZE_P1_RX_HI_SIZE_RXSIZE_6K = 3,    /**< 6k Bytes. */
     MAC_FIFO_SIZE_P1_RX_HI_SIZE_RXSIZE_8K = 4,    /**< 8k Bytes. */
     MAC_FIFO_SIZE_P1_RX_HI_SIZE_RXSIZE_10K = 5,    /**< 10k Bytes. */
     MAC_FIFO_SIZE_P1_RX_HI_SIZE_RXSIZE_12K = 6,    /**< 12k Bytes. */
     MAC_FIFO_SIZE_P1_RX_HI_SIZE_RXSIZE_14K = 7,    /**< 14k Bytes. */
     MAC_FIFO_SIZE_P1_RX_HI_SIZE_RXSIZE_16K = 8     /**< 16k Bytes. */
}  ADI_MAC_FIFO_SIZE_P1_RX_HI_SIZE;

/*! ========================================================================
 *  \struct ADI_MAC_FIFO_SIZE_t
 *  \brief FIFO Sizes Register.
 *  ======================================================================== */
typedef struct _ADI_MAC_FIFO_SIZE_t {
    union {
        struct {
             uint32_t HTX_SIZE         : 4; /**< Host Transmit FIFO Size. */
             uint32_t P1_RX_LO_SIZE    : 4; /**< Port 1 Rx Low Priority FIFO Size. */
             uint32_t P1_RX_HI_SIZE    : 4; /**< Port 1 Rx High Priority FIFO Size. */
             uint32_t RESERVED12       : 20; /**< Reserved */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_FIFO_SIZE_t;

/*@}*/

/** @defgroup TFC Tx FIFO Frame Count Register. (TFC) Register
 *  Tx FIFO Frame Count Register. (TFC) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_TFC_t
 *  \brief Tx FIFO Frame Count Register.
 *  ======================================================================== */
typedef struct _ADI_MAC_TFC_t {
    union {
        struct {
             uint32_t TFC              : 9; /**< Number of Frames in the Tx FIFO. */
             uint32_t RESERVED9        : 23; /**< Reserved */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_TFC_t;

/*@}*/

/** @defgroup TXSIZE Tx FIFO Valid Half Words Register. (TXSIZE) Register
 *  Tx FIFO Valid Half Words Register. (TXSIZE) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_TXSIZE_t
 *  \brief Tx FIFO Valid Half Words Register.
 *  ======================================================================== */
typedef struct _ADI_MAC_TXSIZE_t {
    union {
        struct {
             uint32_t TX_SIZE          : 14; /**< Data in the Tx FIFO. Number of Half Words(16 Bit). */
             uint32_t RESERVED14       : 18; /**< Reserved */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_TXSIZE_t;

/*@}*/

/** @defgroup HTX_OVF_FRM_CNT Host Tx Frames Dropped Due to FIFO Overflow. (HTX_OVF_FRM_CNT) Register
 *  Host Tx Frames Dropped Due to FIFO Overflow. (HTX_OVF_FRM_CNT) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_HTX_OVF_FRM_CNT_t
 *  \brief Host Tx Frames Dropped Due to FIFO Overflow.
 *  ======================================================================== */
typedef struct _ADI_MAC_HTX_OVF_FRM_CNT_t {
    union {
        struct {
             uint32_t HTX_OVF_FRM_CNT  : 32; /**< Counts Host Tx Frames Dropped Due to FIFO Overflow. */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_HTX_OVF_FRM_CNT_t;

/*@}*/

/** @defgroup MECC_ERR_ADDR Address of a Detected ECC Error in Memory. (MECC_ERR_ADDR) Register
 *  Address of a Detected ECC Error in Memory. (MECC_ERR_ADDR) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_MECC_ERR_ADDR_t
 *  \brief Address of a Detected ECC Error in Memory.
 *  ======================================================================== */
typedef struct _ADI_MAC_MECC_ERR_ADDR_t {
    union {
        struct {
             uint32_t MECC_ERR_ADDR    : 14; /**< Address of an Uncorrectable ECC Error in Memory. */
             uint32_t RESERVED14       : 18; /**< Reserved */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_MECC_ERR_ADDR_t;

/*@}*/

/** @defgroup CECC_ERR_0_ Corrected ECC Error Counters. (CECC_ERR_0_) Register
 *  Corrected ECC Error Counters. (CECC_ERR_0_) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_CECC_ERR_0__t
 *  \brief Corrected ECC Error Counters.
 *  ======================================================================== */
typedef struct _ADI_MAC_CECC_ERR_0__t {
    union {
        struct {
             uint32_t CECC_ERR_CNT     : 10; /**< Corrected ECC Error Count. */
             uint32_t RESERVED10       : 22; /**< Reserved */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_CECC_ERR_0__t;

/*@}*/

/** @defgroup ADDR_FILT_UPR_0_ MAC Address Rule and DA Filter Upper 16 Bits Registers. (ADDR_FILT_UPR_0_) Register
 *  MAC Address Rule and DA Filter Upper 16 Bits Registers. (ADDR_FILT_UPR_0_) Register
 *  @{
 */

/*! ========================================================================
 *  \enum ADI_MAC_ADDR_FILT_UPR_0__APPLY2PORT1
 *  \brief Apply to Port 1.
 *  ======================================================================== */
typedef enum
{
     MAC_ADDR_FILT_UPR_0__APPLY2PORT1_NOTAPPLY = 0,    /**< Do Not Apply to Port 1. */
     MAC_ADDR_FILT_UPR_0__APPLY2PORT1_APPLY = 1     /**< Apply to Port 1. */
}  ADI_MAC_ADDR_FILT_UPR_0__APPLY2PORT1;

/*! ========================================================================
 *  \struct ADI_MAC_ADDR_FILT_UPR_0__t
 *  \brief MAC Address Rule and DA Filter Upper 16 Bits Registers.
 *  ======================================================================== */
typedef struct _ADI_MAC_ADDR_FILT_UPR_0__t {
    union {
        struct {
             uint32_t MAC_ADDR         : 16; /**< MAC Address. */
             uint32_t TO_HOST          : 1; /**< Forward Frames Matching This MAC Address to the Host. */
             uint32_t RESERVED17       : 2; /**< Reserved */
             uint32_t HOST_PRI         : 1; /**< Host Rx Port Priority. */
             uint32_t RESERVED20       : 10; /**< Reserved */
             uint32_t APPLY2PORT1      : 1; /**< Apply to Port 1. */
             uint32_t RESERVED31       : 1; /**< Reserved */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_ADDR_FILT_UPR_0__t;

/*@}*/

/** @defgroup ADDR_FILT_LWR_0_ MAC Address DA Filter Lower 32 Bits Registers. (ADDR_FILT_LWR_0_) Register
 *  MAC Address DA Filter Lower 32 Bits Registers. (ADDR_FILT_LWR_0_) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_ADDR_FILT_LWR_0__t
 *  \brief MAC Address DA Filter Lower 32 Bits Registers.
 *  ======================================================================== */
typedef struct _ADI_MAC_ADDR_FILT_LWR_0__t {
    union {
        struct {
             uint32_t MAC_ADDR         : 32; /**< MAC Address. */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_ADDR_FILT_LWR_0__t;

/*@}*/

/** @defgroup ADDR_MSK_UPR_0_ Upper 16 Bits of the MAC Address Mask. (ADDR_MSK_UPR_0_) Register
 *  Upper 16 Bits of the MAC Address Mask. (ADDR_MSK_UPR_0_) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_ADDR_MSK_UPR_0__t
 *  \brief Upper 16 Bits of the MAC Address Mask.
 *  ======================================================================== */
typedef struct _ADI_MAC_ADDR_MSK_UPR_0__t {
    union {
        struct {
             uint32_t MAC_ADDR_MASK    : 16; /**< MAC Address Bit Mask for the Address Table. */
             uint32_t RESERVED16       : 16; /**< Reserved */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_ADDR_MSK_UPR_0__t;

/*@}*/

/** @defgroup ADDR_MSK_LWR_0_ Lower 32 Bits of the MAC Address Mask. (ADDR_MSK_LWR_0_) Register
 *  Lower 32 Bits of the MAC Address Mask. (ADDR_MSK_LWR_0_) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_ADDR_MSK_LWR_0__t
 *  \brief Lower 32 Bits of the MAC Address Mask.
 *  ======================================================================== */
typedef struct _ADI_MAC_ADDR_MSK_LWR_0__t {
    union {
        struct {
             uint32_t MAC_ADDR_MASK    : 32; /**< MAC Address Bit Mask for the Address Table. */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_ADDR_MSK_LWR_0__t;

/*@}*/

/** @defgroup TS_ADDEND Timestamp Accumulator Addend Register. (TS_ADDEND) Register
 *  Timestamp Accumulator Addend Register. (TS_ADDEND) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_TS_ADDEND_t
 *  \brief Timestamp Accumulator Addend Register.
 *  ======================================================================== */
typedef struct _ADI_MAC_TS_ADDEND_t {
    union {
        struct {
             uint32_t TS_ADDEND        : 32; /**< Timestamp Accumulator Addend. */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_TS_ADDEND_t;

/*@}*/

/** @defgroup TS_1SEC_CMP Timer Update Compare Register. (TS_1SEC_CMP) Register
 *  Timer Update Compare Register. (TS_1SEC_CMP) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_TS_1SEC_CMP_t
 *  \brief Timer Update Compare Register.
 *  ======================================================================== */
typedef struct _ADI_MAC_TS_1SEC_CMP_t {
    union {
        struct {
             uint32_t TS_1SEC_CMP      : 32; /**< Timestamp 1 Second Compare Value. */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_TS_1SEC_CMP_t;

/*@}*/

/** @defgroup TS_SEC_CNT Seconds Counter Register. (TS_SEC_CNT) Register
 *  Seconds Counter Register. (TS_SEC_CNT) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_TS_SEC_CNT_t
 *  \brief Seconds Counter Register.
 *  ======================================================================== */
typedef struct _ADI_MAC_TS_SEC_CNT_t {
    union {
        struct {
             uint32_t TS_SEC_CNT       : 32; /**< Write to the Seconds Counter. */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_TS_SEC_CNT_t;

/*@}*/

/** @defgroup TS_NS_CNT Nanoseconds Counter Register. (TS_NS_CNT) Register
 *  Nanoseconds Counter Register. (TS_NS_CNT) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_TS_NS_CNT_t
 *  \brief Nanoseconds Counter Register.
 *  ======================================================================== */
typedef struct _ADI_MAC_TS_NS_CNT_t {
    union {
        struct {
             uint32_t TS_NS_CNT        : 32; /**< Write to the Nanoseconds Counter. */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_TS_NS_CNT_t;

/*@}*/

/** @defgroup TS_CFG Timer Configuration Register. (TS_CFG) Register
 *  Timer Configuration Register. (TS_CFG) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_TS_CFG_t
 *  \brief Timer Configuration Register.
 *  ======================================================================== */
typedef struct _ADI_MAC_TS_CFG_t {
    union {
        struct {
             uint32_t TS_EN            : 1; /**< Enable the 1588 Timestamp Counter. */
             uint32_t TS_CLR           : 1; /**< Clear the 1588 Timestamp Counters. */
             uint32_t TS_TIMER_DEF     : 1; /**< The Default Value for the TS_TIMER Output. */
             uint32_t TS_TIMER_STOP    : 1; /**< Stop Toggling the TS_TIMER Output. */
             uint32_t TS_CAPT_FREE_CNT : 1; /**< Capture the Free Running Counter. */
             uint32_t RESERVED5        : 27; /**< Reserved */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_TS_CFG_t;

/*@}*/

/** @defgroup TS_TIMER_HI High Period for TS_TIMER Register. (TS_TIMER_HI) Register
 *  High Period for TS_TIMER Register. (TS_TIMER_HI) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_TS_TIMER_HI_t
 *  \brief High Period for TS_TIMER Register.
 *  ======================================================================== */
typedef struct _ADI_MAC_TS_TIMER_HI_t {
    union {
        struct {
             uint32_t TS_TIMER_HI      : 32; /**< TS_TIMER High Period. */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_TS_TIMER_HI_t;

/*@}*/

/** @defgroup TS_TIMER_LO Low Period for TS_TIMER Register. (TS_TIMER_LO) Register
 *  Low Period for TS_TIMER Register. (TS_TIMER_LO) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_TS_TIMER_LO_t
 *  \brief Low Period for TS_TIMER Register.
 *  ======================================================================== */
typedef struct _ADI_MAC_TS_TIMER_LO_t {
    union {
        struct {
             uint32_t TS_TIMER_LO      : 32; /**< TS_TIMER Low Period. */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_TS_TIMER_LO_t;

/*@}*/

/** @defgroup TS_TIMER_QE_CORR Quantization Error Correction Register. (TS_TIMER_QE_CORR) Register
 *  Quantization Error Correction Register. (TS_TIMER_QE_CORR) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_TS_TIMER_QE_CORR_t
 *  \brief Quantization Error Correction Register.
 *  ======================================================================== */
typedef struct _ADI_MAC_TS_TIMER_QE_CORR_t {
    union {
        struct {
             uint32_t TS_TIMER_QE_CORR : 8; /**< TS_TIMER Quantization Error Correction Value. */
             uint32_t RESERVED8        : 24; /**< Reserved */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_TS_TIMER_QE_CORR_t;

/*@}*/

/** @defgroup TS_TIMER_START TS_TIMER Counter Start Time Register. (TS_TIMER_START) Register
 *  TS_TIMER Counter Start Time Register. (TS_TIMER_START) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_TS_TIMER_START_t
 *  \brief TS_TIMER Counter Start Time Register.
 *  ======================================================================== */
typedef struct _ADI_MAC_TS_TIMER_START_t {
    union {
        struct {
             uint32_t TS_TSTART        : 32; /**< Point in Time at Which to Start the TS_TIMER Counter. */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_TS_TIMER_START_t;

/*@}*/

/** @defgroup TS_EXT_CAPT0 TS_CAPT Pin 0 Timestamp Register. (TS_EXT_CAPT0) Register
 *  TS_CAPT Pin 0 Timestamp Register. (TS_EXT_CAPT0) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_TS_EXT_CAPT0_t
 *  \brief TS_CAPT Pin 0 Timestamp Register.
 *  ======================================================================== */
typedef struct _ADI_MAC_TS_EXT_CAPT0_t {
    union {
        struct {
             uint32_t TS_EXT_CAPTD     : 32; /**< Timestamp Captured on the Assertion of TS_CAPT Pin. */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_TS_EXT_CAPT0_t;

/*@}*/

/** @defgroup TS_EXT_CAPT1 TS_CAPT Pin 1 Timestamp Register. (TS_EXT_CAPT1) Register
 *  TS_CAPT Pin 1 Timestamp Register. (TS_EXT_CAPT1) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_TS_EXT_CAPT1_t
 *  \brief TS_CAPT Pin 1 Timestamp Register.
 *  ======================================================================== */
typedef struct _ADI_MAC_TS_EXT_CAPT1_t {
    union {
        struct {
             uint32_t TS_EXT_CAPTD     : 32; /**< Timestamp Captured on the Assertion of TS_CAPT Pin. */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_TS_EXT_CAPT1_t;

/*@}*/

/** @defgroup TS_FREECNT_CAPT TS_CAPT Free Running Counter Register. (TS_FREECNT_CAPT) Register
 *  TS_CAPT Free Running Counter Register. (TS_FREECNT_CAPT) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_TS_FREECNT_CAPT_t
 *  \brief TS_CAPT Free Running Counter Register.
 *  ======================================================================== */
typedef struct _ADI_MAC_TS_FREECNT_CAPT_t {
    union {
        struct {
             uint32_t TS_CNT_CAPTD     : 32; /**< Captured Free Running Counter. */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_TS_FREECNT_CAPT_t;

/*@}*/

/** @defgroup P1_RX_FSIZE P1 MAC Rx Frame Size Register. (P1_RX_FSIZE) Register
 *  P1 MAC Rx Frame Size Register. (P1_RX_FSIZE) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_P1_RX_FSIZE_t
 *  \brief P1 MAC Rx Frame Size Register.
 *  ======================================================================== */
typedef struct _ADI_MAC_P1_RX_FSIZE_t {
    union {
        struct {
             uint32_t P1_RX_FRM_SIZE   : 11; /**< Receive Frame Size. */
             uint32_t RESERVED11       : 21; /**< Reserved */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_P1_RX_FSIZE_t;

/*@}*/

/** @defgroup P1_RX P1 MAC Receive Register. (P1_RX) Register
 *  P1 MAC Receive Register. (P1_RX) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_P1_RX_t
 *  \brief P1 MAC Receive Register.
 *  ======================================================================== */
typedef struct _ADI_MAC_P1_RX_t {
    union {
        struct {
             uint32_t P1_RDR           : 32; /**< Receive Data Register. */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_P1_RX_t;

/*@}*/

/** @defgroup P1_RX_FRM_CNT P1 Rx Frame Count Register. (P1_RX_FRM_CNT) Register
 *  P1 Rx Frame Count Register. (P1_RX_FRM_CNT) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_P1_RX_FRM_CNT_t
 *  \brief P1 Rx Frame Count Register.
 *  ======================================================================== */
typedef struct _ADI_MAC_P1_RX_FRM_CNT_t {
    union {
        struct {
             uint32_t P1_RX_FRM_CNT    : 32; /**< Rx Frame Count. */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_P1_RX_FRM_CNT_t;

/*@}*/

/** @defgroup P1_RX_BCAST_CNT P1 Rx Broadcast Frame Count Register. (P1_RX_BCAST_CNT) Register
 *  P1 Rx Broadcast Frame Count Register. (P1_RX_BCAST_CNT) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_P1_RX_BCAST_CNT_t
 *  \brief P1 Rx Broadcast Frame Count Register.
 *  ======================================================================== */
typedef struct _ADI_MAC_P1_RX_BCAST_CNT_t {
    union {
        struct {
             uint32_t P1_RX_BCAST_CNT  : 32; /**< Rx Broadcast Frame Count. */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_P1_RX_BCAST_CNT_t;

/*@}*/

/** @defgroup P1_RX_MCAST_CNT P1 Rx Multicast Frame Count Register. (P1_RX_MCAST_CNT) Register
 *  P1 Rx Multicast Frame Count Register. (P1_RX_MCAST_CNT) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_P1_RX_MCAST_CNT_t
 *  \brief P1 Rx Multicast Frame Count Register.
 *  ======================================================================== */
typedef struct _ADI_MAC_P1_RX_MCAST_CNT_t {
    union {
        struct {
             uint32_t P1_RX_MCAST_CNT  : 32; /**< Rx Multicast Frame Count. */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_P1_RX_MCAST_CNT_t;

/*@}*/

/** @defgroup P1_RX_UCAST_CNT P1 Rx Unicast Frame Count Register. (P1_RX_UCAST_CNT) Register
 *  P1 Rx Unicast Frame Count Register. (P1_RX_UCAST_CNT) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_P1_RX_UCAST_CNT_t
 *  \brief P1 Rx Unicast Frame Count Register.
 *  ======================================================================== */
typedef struct _ADI_MAC_P1_RX_UCAST_CNT_t {
    union {
        struct {
             uint32_t P1_RX_UCAST_CNT  : 32; /**< Rx Unicast Frame Count. */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_P1_RX_UCAST_CNT_t;

/*@}*/

/** @defgroup P1_RX_CRC_ERR_CNT P1 Rx CRC Errored Frame Count Register. (P1_RX_CRC_ERR_CNT) Register
 *  P1 Rx CRC Errored Frame Count Register. (P1_RX_CRC_ERR_CNT) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_P1_RX_CRC_ERR_CNT_t
 *  \brief P1 Rx CRC Errored Frame Count Register.
 *  ======================================================================== */
typedef struct _ADI_MAC_P1_RX_CRC_ERR_CNT_t {
    union {
        struct {
             uint32_t P1_RX_CRC_ERR_CNT : 32; /**< Rx CRC Errored Frame Count. */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_P1_RX_CRC_ERR_CNT_t;

/*@}*/

/** @defgroup P1_RX_ALGN_ERR_CNT P1 Rx Align Error Count Register. (P1_RX_ALGN_ERR_CNT) Register
 *  P1 Rx Align Error Count Register. (P1_RX_ALGN_ERR_CNT) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_P1_RX_ALGN_ERR_CNT_t
 *  \brief P1 Rx Align Error Count Register.
 *  ======================================================================== */
typedef struct _ADI_MAC_P1_RX_ALGN_ERR_CNT_t {
    union {
        struct {
             uint32_t P1_RX_ALGN_ERR_CNT : 32; /**< Rx Align Error Count. */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_P1_RX_ALGN_ERR_CNT_t;

/*@}*/

/** @defgroup P1_RX_LS_ERR_CNT P1 Rx Long/Short Frame Error Count Register. (P1_RX_LS_ERR_CNT) Register
 *  P1 Rx Long/Short Frame Error Count Register. (P1_RX_LS_ERR_CNT) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_P1_RX_LS_ERR_CNT_t
 *  \brief P1 Rx Long/Short Frame Error Count Register.
 *  ======================================================================== */
typedef struct _ADI_MAC_P1_RX_LS_ERR_CNT_t {
    union {
        struct {
             uint32_t P1_RX_LS_ERR_CNT : 32; /**< Rx Long/Short Frame Error Count. */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_P1_RX_LS_ERR_CNT_t;

/*@}*/

/** @defgroup P1_RX_PHY_ERR_CNT P1 Rx PHY Error Count Register. (P1_RX_PHY_ERR_CNT) Register
 *  P1 Rx PHY Error Count Register. (P1_RX_PHY_ERR_CNT) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_P1_RX_PHY_ERR_CNT_t
 *  \brief P1 Rx PHY Error Count Register.
 *  ======================================================================== */
typedef struct _ADI_MAC_P1_RX_PHY_ERR_CNT_t {
    union {
        struct {
             uint32_t P1_RX_PHY_ERR_CNT : 32; /**< Rx PHY Error Count. */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_P1_RX_PHY_ERR_CNT_t;

/*@}*/

/** @defgroup P1_TX_FRM_CNT P1 Tx Frame Count Register. (P1_TX_FRM_CNT) Register
 *  P1 Tx Frame Count Register. (P1_TX_FRM_CNT) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_P1_TX_FRM_CNT_t
 *  \brief P1 Tx Frame Count Register.
 *  ======================================================================== */
typedef struct _ADI_MAC_P1_TX_FRM_CNT_t {
    union {
        struct {
             uint32_t P1_TX_FRM_CNT    : 32; /**< Tx Frame Count. */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_P1_TX_FRM_CNT_t;

/*@}*/

/** @defgroup P1_TX_BCAST_CNT P1 Tx Broadcast Frame Count Register. (P1_TX_BCAST_CNT) Register
 *  P1 Tx Broadcast Frame Count Register. (P1_TX_BCAST_CNT) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_P1_TX_BCAST_CNT_t
 *  \brief P1 Tx Broadcast Frame Count Register.
 *  ======================================================================== */
typedef struct _ADI_MAC_P1_TX_BCAST_CNT_t {
    union {
        struct {
             uint32_t P1_TX_BCAST_CNT  : 32; /**< Tx Broadcast Frame Count. */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_P1_TX_BCAST_CNT_t;

/*@}*/

/** @defgroup P1_TX_MCAST_CNT P1 Tx Multicast Frame Count Register. (P1_TX_MCAST_CNT) Register
 *  P1 Tx Multicast Frame Count Register. (P1_TX_MCAST_CNT) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_P1_TX_MCAST_CNT_t
 *  \brief P1 Tx Multicast Frame Count Register.
 *  ======================================================================== */
typedef struct _ADI_MAC_P1_TX_MCAST_CNT_t {
    union {
        struct {
             uint32_t P1_TX_MCAST_CNT  : 32; /**< Tx Multicast Frame Count. */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_P1_TX_MCAST_CNT_t;

/*@}*/

/** @defgroup P1_TX_UCAST_CNT P1 Tx Unicast Frame Count Register. (P1_TX_UCAST_CNT) Register
 *  P1 Tx Unicast Frame Count Register. (P1_TX_UCAST_CNT) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_P1_TX_UCAST_CNT_t
 *  \brief P1 Tx Unicast Frame Count Register.
 *  ======================================================================== */
typedef struct _ADI_MAC_P1_TX_UCAST_CNT_t {
    union {
        struct {
             uint32_t P1_TX_UCAST_CNT  : 32; /**< Tx Unicast Frame Count. */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_P1_TX_UCAST_CNT_t;

/*@}*/

/** @defgroup P1_RX_DROP_FULL_CNT P1 Rx Frames Dropped Due to FIFO Full Register. (P1_RX_DROP_FULL_CNT) Register
 *  P1 Rx Frames Dropped Due to FIFO Full Register. (P1_RX_DROP_FULL_CNT) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_P1_RX_DROP_FULL_CNT_t
 *  \brief P1 Rx Frames Dropped Due to FIFO Full Register.
 *  ======================================================================== */
typedef struct _ADI_MAC_P1_RX_DROP_FULL_CNT_t {
    union {
        struct {
             uint32_t P1_RX_DROP_FULL_CNT : 32; /**< Rx Frames Dropped Due to FIFO Full. */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_P1_RX_DROP_FULL_CNT_t;

/*@}*/

/** @defgroup P1_RX_DROP_FILT_CNT P1 Rx Frames Dropped Due to Filtering Register. (P1_RX_DROP_FILT_CNT) Register
 *  P1 Rx Frames Dropped Due to Filtering Register. (P1_RX_DROP_FILT_CNT) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_P1_RX_DROP_FILT_CNT_t
 *  \brief P1 Rx Frames Dropped Due to Filtering Register.
 *  ======================================================================== */
typedef struct _ADI_MAC_P1_RX_DROP_FILT_CNT_t {
    union {
        struct {
             uint32_t P1_RX_DROP_FILT_CNT : 32; /**< Rx Frames Dropped Due to Filtering. */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_P1_RX_DROP_FILT_CNT_t;

/*@}*/

/** @defgroup P1_RX_IFG_ERR_CNT Frame Received on Port 1 with IFG Errors. (P1_RX_IFG_ERR_CNT) Register
 *  Frame Received on Port 1 with IFG Errors. (P1_RX_IFG_ERR_CNT) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_P1_RX_IFG_ERR_CNT_t
 *  \brief Frame Received on Port 1 with IFG Errors.
 *  ======================================================================== */
typedef struct _ADI_MAC_P1_RX_IFG_ERR_CNT_t {
    union {
        struct {
             uint32_t P1_RX_IFG_ERR_CNT : 32; /**< IFG Error Counter for Port 1 Received Frames. */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_P1_RX_IFG_ERR_CNT_t;

/*@}*/

/** @defgroup P1_TX_IFG P1 Transmit Inter Frame Gap Register. (P1_TX_IFG) Register
 *  P1 Transmit Inter Frame Gap Register. (P1_TX_IFG) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_P1_TX_IFG_t
 *  \brief P1 Transmit Inter Frame Gap Register.
 *  ======================================================================== */
typedef struct _ADI_MAC_P1_TX_IFG_t {
    union {
        struct {
             uint32_t P1_TX_IFG        : 8; /**< Inter Frame Gap. */
             uint32_t RESERVED8        : 24; /**< Reserved */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_P1_TX_IFG_t;

/*@}*/

/** @defgroup P1_LOOP P1 MAC Loopback Enable Register. (P1_LOOP) Register
 *  P1 MAC Loopback Enable Register. (P1_LOOP) Register
 *  @{
 */

/*! ========================================================================
 *  \enum ADI_MAC_P1_LOOP_P1_LOOPBACK_EN
 *  \brief MAC Loopback.
 *  ======================================================================== */
typedef enum
{
     MAC_P1_LOOP_P1_LOOPBACK_EN_LOOPB_DIS = 0,    /**< Normal Operation - Loopback Disabled. */
     MAC_P1_LOOP_P1_LOOPBACK_EN_LOOPB_EN = 1     /**< Loopback Enabled. */
}  ADI_MAC_P1_LOOP_P1_LOOPBACK_EN;

/*! ========================================================================
 *  \struct ADI_MAC_P1_LOOP_t
 *  \brief P1 MAC Loopback Enable Register.
 *  ======================================================================== */
typedef struct _ADI_MAC_P1_LOOP_t {
    union {
        struct {
             uint32_t P1_LOOPBACK_EN   : 1; /**< MAC Loopback. */
             uint32_t RESERVED1        : 31; /**< Reserved */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_P1_LOOP_t;

/*@}*/

/** @defgroup P1_RX_CRC_EN P1 CRC Check Enable on Receive Register. (P1_RX_CRC_EN) Register
 *  P1 CRC Check Enable on Receive Register. (P1_RX_CRC_EN) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_P1_RX_CRC_EN_t
 *  \brief P1 CRC Check Enable on Receive Register.
 *  ======================================================================== */
typedef struct _ADI_MAC_P1_RX_CRC_EN_t {
    union {
        struct {
             uint32_t P1_CRC_CHK_EN    : 1; /**< CRC Check Enable on Receive. */
             uint32_t RESERVED1        : 31; /**< Reserved */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_P1_RX_CRC_EN_t;

/*@}*/

/** @defgroup P1_RX_IFG P1 Receive Inter Frame Gap Register. (P1_RX_IFG) Register
 *  P1 Receive Inter Frame Gap Register. (P1_RX_IFG) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_P1_RX_IFG_t
 *  \brief P1 Receive Inter Frame Gap Register.
 *  ======================================================================== */
typedef struct _ADI_MAC_P1_RX_IFG_t {
    union {
        struct {
             uint32_t P1_RX_IFG        : 6; /**< Inter Frame Gap. */
             uint32_t RESERVED6        : 26; /**< Reserved */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_P1_RX_IFG_t;

/*@}*/

/** @defgroup P1_RX_MAX_LEN P1 Max Receive Frame Length Register. (P1_RX_MAX_LEN) Register
 *  P1 Max Receive Frame Length Register. (P1_RX_MAX_LEN) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_P1_RX_MAX_LEN_t
 *  \brief P1 Max Receive Frame Length Register.
 *  ======================================================================== */
typedef struct _ADI_MAC_P1_RX_MAX_LEN_t {
    union {
        struct {
             uint32_t P1_MAX_FRM_LEN   : 16; /**< Max Frame Length on Receive. */
             uint32_t RESERVED16       : 16; /**< Reserved */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_P1_RX_MAX_LEN_t;

/*@}*/

/** @defgroup P1_RX_MIN_LEN P1 Min Receive Frame Length Register. (P1_RX_MIN_LEN) Register
 *  P1 Min Receive Frame Length Register. (P1_RX_MIN_LEN) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_P1_RX_MIN_LEN_t
 *  \brief P1 Min Receive Frame Length Register.
 *  ======================================================================== */
typedef struct _ADI_MAC_P1_RX_MIN_LEN_t {
    union {
        struct {
             uint32_t P1_MIN_FRM_LEN   : 16; /**< Min Frame Length on Receive. */
             uint32_t RESERVED16       : 16; /**< Reserved */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_P1_RX_MIN_LEN_t;

/*@}*/

/** @defgroup P1_LO_RFC P1 Rx Low Priority FIFO Frame Count Register. (P1_LO_RFC) Register
 *  P1 Rx Low Priority FIFO Frame Count Register. (P1_LO_RFC) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_P1_LO_RFC_t
 *  \brief P1 Rx Low Priority FIFO Frame Count Register.
 *  ======================================================================== */
typedef struct _ADI_MAC_P1_LO_RFC_t {
    union {
        struct {
             uint32_t P1_LO_RFC        : 9; /**< Receive Frame Count for the Low Priority FIFO. */
             uint32_t RESERVED9        : 23; /**< Reserved */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_P1_LO_RFC_t;

/*@}*/

/** @defgroup P1_HI_RFC P1 Rx High Priority FIFO Frame Count Register. (P1_HI_RFC) Register
 *  P1 Rx High Priority FIFO Frame Count Register. (P1_HI_RFC) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_P1_HI_RFC_t
 *  \brief P1 Rx High Priority FIFO Frame Count Register.
 *  ======================================================================== */
typedef struct _ADI_MAC_P1_HI_RFC_t {
    union {
        struct {
             uint32_t P1_HI_RFC        : 9; /**< Receive Frame Count for the High Priority FIFO. */
             uint32_t RESERVED9        : 23; /**< Reserved */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_P1_HI_RFC_t;

/*@}*/

/** @defgroup P1_LO_RXSIZE P1 Low Priority Rx FIFO Valid Half Words Register. (P1_LO_RXSIZE) Register
 *  P1 Low Priority Rx FIFO Valid Half Words Register. (P1_LO_RXSIZE) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_P1_LO_RXSIZE_t
 *  \brief P1 Low Priority Rx FIFO Valid Half Words Register.
 *  ======================================================================== */
typedef struct _ADI_MAC_P1_LO_RXSIZE_t {
    union {
        struct {
             uint32_t P1_LO_RXSIZE     : 14; /**< Data in the Rx FIFO. Number of Half Words(16 Bit). */
             uint32_t RESERVED14       : 18; /**< Reserved */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_P1_LO_RXSIZE_t;

/*@}*/

/** @defgroup P1_HI_RXSIZE P1 High Priority Rx FIFO Valid Half Words Register. (P1_HI_RXSIZE) Register
 *  P1 High Priority Rx FIFO Valid Half Words Register. (P1_HI_RXSIZE) Register
 *  @{
 */

/*! ========================================================================
 *  \struct ADI_MAC_P1_HI_RXSIZE_t
 *  \brief P1 High Priority Rx FIFO Valid Half Words Register.
 *  ======================================================================== */
typedef struct _ADI_MAC_P1_HI_RXSIZE_t {
    union {
        struct {
             uint32_t P1_HI_RXSIZE     : 14; /**< Data in the Rx FIFO. Number of Half Words(16 Bit). */
             uint32_t RESERVED14       : 18; /**< Reserved */
        };
        uint32_t VALUE32;
    };
} ADI_MAC_P1_HI_RXSIZE_t;

/*@}*/


#if defined (__CC_ARM)
#pragma pop
#endif

#endif /* ADIN1110_MAC_TYPEDEFS_H */

