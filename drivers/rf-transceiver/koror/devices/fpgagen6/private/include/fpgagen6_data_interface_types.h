/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
 * \file fpgagen6_data_interface_types.h
 * \brief Contains FPGA Data Interface enum and struct definitions
 *
 * FPGA API Version: 2.9.0.4
 */

#ifndef _FPGAGEN6_DATA_INTERFACE_TYPES_H_
#define _FPGAGEN6_DATA_INTERFACE_TYPES_H_

#include "../../private/bf/fpgagen6_bf_axi_adrv904x_phy.h"

/**
*  \brief Sync Out Set Function Pointer Type
*/
typedef adi_fpgagen6_ErrAction_e (*SyncF2pSelectSetFnPtr_t) (   adi_fpgagen6_Device_t* const            device,
                                                                const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e  baseAddr,
                                                                const uint8_t                           bfValue);

/**
*  \brief Sync Out Get Function Pointer Type
*/
typedef adi_fpgagen6_ErrAction_e (*SyncF2pSelectGetFnPtr_t) (   adi_fpgagen6_Device_t* const            device,
                                                                const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e  baseAddr,
                                                                uint8_t* const                          bfValue);

/**
*  \brief Sync P2F Polarity Set Function Pointer Type
*/
typedef adi_fpgagen6_ErrAction_e(*SyncP2fPolaritySetFnPtr_t)(  adi_fpgagen6_Device_t* const            device,
                                                            const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e  baseAddr,
                                                            const uint8_t                           bfValue);


/**
*  \brief Sync P2F Polarity Get Function Pointer Type
*/
typedef adi_fpgagen6_ErrAction_e(*SyncP2fPolarityGetFnPtr_t)(   adi_fpgagen6_Device_t* const            device,
                                                                const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e  baseAddr,
                                                                uint8_t* const                          bfValue);

/**
*  \brief Sync F2P Polarity Set Function Pointer Type
*/
typedef adi_fpgagen6_ErrAction_e(*SyncF2pPolaritySetFnPtr_t)(   adi_fpgagen6_Device_t* const            device,
                                                                const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e  baseAddr,
                                                                const uint8_t                           bfValue);


/**
*  \brief Sync F2P Polarity Get Function Pointer Type
*/
typedef adi_fpgagen6_ErrAction_e(*SyncF2pPolarityGetFnPtr_t)(   adi_fpgagen6_Device_t* const            device,
                                                                const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e  baseAddr,
                                                                uint8_t* const                          bfValue);

#define FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_TX_204B_FRAMER_0     FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0
#define FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_TX_204C_FRAMER_0     FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0
#define FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_TX_204B_FRAMER_1     FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1
#define FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_TX_204C_FRAMER_1     FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1
#define FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_RX_204B_DEFRAMER_0   FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0
#define FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_RX_204C_DEFRAMER_0   FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0
#define FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_RX_204B_DEFRAMER_1   FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1
#define FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_RX_204C_DEFRAMER_1   FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1    
#define FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_RX_204B_DEFRAMER_2   FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2
#define FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_RX_204C_DEFRAMER_2   FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2
#define FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_TX_204B_0            FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_0
#define FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_TX_204C_0            FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_0
#define FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_TX_204B_1            FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_1
#define FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_TX_204C_1            FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_1
#define FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_RX_204B_0            FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_0
#define FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_RX_204C_0            FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_0
#define FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_RX_204B_1            FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_1
#define FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_RX_204C_1            FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_1
#define FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_RX_204B_2            FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_2
#define FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_RX_204C_2            FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_2

#define FPGAGEN6_BF_ADS10V1_TOP_AXI_TXDMA_0                  FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0 
#define FPGAGEN6_BF_ADS10V1_TOP_AXI_TXDMA_1                  FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1
#define FPGAGEN6_BF_ADS10V1_TOP_AXI_TXDMA_2                  FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2 
#define FPGAGEN6_BF_ADS10V1_TOP_AXI_TXDMA_3                  FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3
#define FPGAGEN6_BF_ADS10V1_TOP_AXI_TXDMA_4                  FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4
#define FPGAGEN6_BF_ADS10V1_TOP_AXI_TXDMA_5                  FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5
#define FPGAGEN6_BF_ADS10V1_TOP_AXI_TXDMA_6                  FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6
#define FPGAGEN6_BF_ADS10V1_TOP_AXI_TXDMA_7                  FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7
#define FPGAGEN6_BF_ADS10V1_TOP_AXI_TXDMA_8                  FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8
#define FPGAGEN6_BF_ADS10V1_TOP_AXI_TXDMA_9                  FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9
#define FPGAGEN6_BF_ADS10V1_TOP_AXI_TXDMA_10                 FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10
#define FPGAGEN6_BF_ADS10V1_TOP_AXI_TXDMA_11                 FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11
#define FPGAGEN6_BF_ADS10V1_TOP_AXI_TXDMA_12                 FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12
#define FPGAGEN6_BF_ADS10V1_TOP_AXI_TXDMA_13                 FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13
#define FPGAGEN6_BF_ADS10V1_TOP_AXI_TXDMA_14                 FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14
#define FPGAGEN6_BF_ADS10V1_TOP_AXI_TXDMA_15                 FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15

#define FPGAGEN6_BF_ADS10_TOP_SPI                            FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_KOROR_SPI
#define FPGAGEN6_BF_ADS10_TOP_SPI2                           FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_KOROR_SPI2
#define FPGAGEN6_BF_ADS10_TOP_TDD                            FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_KOROR_TDD

#define FPGAGEN6_BF_ADS10_ADS10V1_TOP_AXI_MEMORY_PAGE        FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_MEMORY_PAGE
#define FPGAGEN6_BF_ADS10_TOP_SYSTEM_TIMING_LINK_CLKWIZ      FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_KOROR_SYSTEM_TIMING_KOROR_LINK_CLKWIZ
#define FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY                  FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_PHY
#define FPGAGEN6_BF_ADS10_TOP_SYSTEM_TIMING_CONTROL          FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_KOROR_SYSTEM_TIMING_KOROR_SYSTEM_TIMING_CONTROL
#define FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_GPIO                 FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_GPIO

#define FPGAGEN6_BF_ADS10_SHIM_TX_DATA_PATH                  FPGAGEN6_BF_ADRV904X_ADS10_KOROR_ADS10_SHIM_TX_DATA_PATH

#endif /* _FPGAGEN6_DATA_INTERFACE_TYPES_H_ */