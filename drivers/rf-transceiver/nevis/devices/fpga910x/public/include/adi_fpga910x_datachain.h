/**
 * \file
 * \brief Functions to configure the FPGA910X Data Chains
 *
 * Copyright (c) 2022 - 2026 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 * Released under the ADRV910X API license.
 * For more information, see the "LICENSE.txt" file.
 */

#ifndef _ADI_FPGA910X_DATACHAIN_H_
#define _ADI_FPGA910X_DATACHAIN_H_

#include "adi_fpga910x_datachain_types.h"
#include "adi_adrv910x_common_types.h"
#include "adi_common_types.h"
#include "adi_fpga910x_types.h"
#include "adi_fpga910x_gpio_types.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * The adi_fpga910x_DataChain_Data_Get_XXX and adi_fpga910x_DataChain_Data_Set_XXX functions use local buffers to pack
 * or unpack data in the correct format. 
 * If ADI_FPGA910X_USE_DYNAMIC_MEMORY is defined, these buffers will be allocated dynamically on the heap using calloc
 * If undefined, the buffers will be allocated on the stack. The size of the buffers (and thus maximum capture size)
 * is controlled by ADI_FPGA910X_MAX_CAPTURE_BYTES. The default value is the maximum size allowed by the FPGA
 */
//#define ADI_FPGA910X_USE_DYNAMIC_MEMORY 1
#define ADI_FPGA910X_MAX_CAPTURE_BYTES 0x007FFFFF

/**
 * \brief Reads the DMA configuration for a single DMA module
 *
 * \param[in]  fpga910x  Context variable - Pointer to the FPGA910X device data structure
 * \param[in]  port      The port that the channel refers to
 * \param[in]  channel   The channel of the specified port to configure
 * \param[out] dmaCfg    The DMA configuration that was read
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_DataChain_Dma_Inspect(adi_fpga910x_Device_t *fpga910x,
                                           adi_common_Port_e port,
                                           adi_common_ChannelNumber_e channel,
                                           adi_fpga910x_DmaCfg_t *dmaCfg);

/**
 * \brief Reads the DMA status for a single DMA module
 *
 * \param[in]  fpga910x  Context variable - Pointer to the FPGA910X device data structure
 * \param[in]  port      The port that the channel refers to
 * \param[in]  channel   The channel of the specified port to configure
 * \param[out] status    The DMA status that was read
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_DataChain_DmaStatus_Get(adi_fpga910x_Device_t *fpga910x,
                                             adi_common_Port_e port,
                                             adi_common_ChannelNumber_e channel,
                                             adi_fpga910x_DmaStatus_t *status);

/**
 * \brief Configure the data chains If port is Rx: initiate capture, and wait for capture to complete If port is Tx:
 *        start transmitting in continuous mode
 *
 * \note If port is Rx: This function configures the specified data chains, starts an Rx capture, and blocks until the captures complete or the timeout is reached. Essentially, this function fills the RAM. Use the DataGet_X utility functions to read data from RAM in the appropriate format.
 * \note If port is Tx: This function does NOT set the data to be transmitted. For that, use one of the DataSet_X utility functions to write data to RAM in the appropriate format.
 *
 * \param[in] fpga910x  Context variable - Pointer to the FPGA910X device data structure
 * \param[in] port      The port that the channel refers to
 * \param[in] channel   The channel of the specified port to configure
 * \param[in] dmaCfg    The DMA configuration to configure the data chain
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_DataChain_PerformTransfer(adi_fpga910x_Device_t *fpga910x,
                                               adi_common_Port_e port,
                                               adi_common_ChannelNumber_e channel,
                                               adi_fpga910x_DmaCfg_t *dmaCfg);

/**
 * \brief Configure the data chains If port is Rx: initiate capture and return without waiting for capture to complete
 *        If port is Tx: start transmitting in continuous mode
 *
 * \note If port is Rx: This function configures the specified data chains, starts an Rx capture, and returns without waiting for capture to complete. In order to perform a successful capture, 'adi_fpga910x_DataChain_PerformTransfer_Finish' may be called after this call and will return when transfer is complete. Essentially, this function fills the RAM. Use the DataGet_X utility functions to read data from RAM in the appropriate format.
 * \note If port is Tx: This function does NOT set the data to be transmitted. For that, use one of the DataSet_X utility functions to write data to RAM in the appropriate format.
 *
 * \param[in] fpga910x  Context variable - Pointer to the FPGA910X device data structure
 * \param[in] port      The port that the channel refers to
 * \param[in] channel   The channel of the specified port to configure
 * \param[in] dmaCfg    The DMA configuration to configure the data chain
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_DataChain_PerformTransfer_Start(adi_fpga910x_Device_t *fpga910x,
                                                     adi_common_Port_e port,
                                                     adi_common_ChannelNumber_e channel,
                                                     adi_fpga910x_DmaCfg_t *dmaCfg);

/**
 * \brief Wait for the initiated capture in 'adi_fpga910x_DataChain_PerformTransfer_Start()' function call to complete
 *        If port is Rx: This function must be called after 'adi_fpga910x_DataChain_PerformTransfer_Start()' for capture
 *        to complete If port is Tx: Since transmitting is in continuous mode, this function takes no action
 *
 * \param[in] fpga910x  Context variable - Pointer to the FPGA910X device data structure
 * \param[in] port      The port that the channel refers to
 * \param[in] channel   The channel of the specified port to configure
 * \param[in] dmaCfg    The DMA configuration for which to wait for the capture to complete
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_DataChain_PerformTransfer_Finish(adi_fpga910x_Device_t *fpga910x,
                                                      adi_common_Port_e port,
                                                      adi_common_ChannelNumber_e channel,
                                                      adi_fpga910x_DmaCfg_t *dmaCfg);

/**
 * \brief Configure and start DMA for FH table transfer
 *
 * \param[in] fpga910x  Context variable - Pointer to the FPGA910X device data structure
 * \param[in] dmaCfg    The DMA configuration to configure the SPI
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_DataChain_HopTable_Transfer_Start(adi_fpga910x_Device_t *fpga910x, adi_fpga910x_DmaCfg_t *dmaCfg);

/**
 * \brief Stop DMA for FH table transfer
 *
 * \param[in] fpga910x    Context variable - Pointer to the FPGA910X device data structure
 * \param[in] timeout_ms  Stop DMA or until Timeout reached
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_DataChain_HopTable_Transfer_Stop(adi_fpga910x_Device_t *fpga910x, uint32_t timeout_ms);

/**
 * \brief Configure MSPI in DMA mode for FH table transfer to ARM memory
 *
 * \param[in] fpga910x        Context variable - Pointer to the FPGA910X device data structure
 * \param[in] trigger         The trigger source to use
 * \param[in] bytesPerTable   Number of bytes to be transferred through SPI DMA
 * \param[in] framesPerTable  Number of frames to be transferred through SPI DMA
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_DataChain_Mspi_Dma_Configure(adi_fpga910x_Device_t *fpga910x,
                                                  adi_fpga910x_DmaTrigger_e trigger,
                                                  uint32_t bytesPerTable,
                                                  uint32_t framesPerTable);

/**
 * \brief Configure MSPI in FIFO mode
 *
 * \param[in] fpga910x  Context variable - Pointer to the FPGA910X device data structure
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_DataChain_Mspi_Fifo_Configure(adi_fpga910x_Device_t *fpga910x);

/**
 * \brief Configure the data chains for synchronous transfer between two channels If port is Rx: Synchronously start a
 *        capture on two channels, and wait for both channels capture to complete. If port is Tx: Synchronously starts
 *        transmission on two channels in continuous mode.
 *
 * \note If port is Rx: This function configures the data chains for channel 1 and channel 2, synchronously starts an Rx capture on each channel, and blocks until the captures complete or the timeout is reached. Essentially, this function fills the RAM for channel 1 and channel 2. Use the DataGet_X utility functions to read data from RAM in the appropriate format.
 * \note If port is Tx: This function does NOT set the data to be transmitted. For that, use one of the DataSet_X utility functions to write data to RAM in the appropriate format.
 * \note The trigger in dmaCfg is overwritten with ADI_FPGA910X_DMA_TRIGGER_SYNC.
 *
 * \param[in] fpga910x  Context variable - Pointer to the FPGA910X device data structure
 * \param[in] port      The port to initiate the transfer
 * \param[in] dmaCfg    The DMA configuration to configure the data chain
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_DataChain_PerformSynchronousTransfer(adi_fpga910x_Device_t *fpga910x,
                                                          adi_common_Port_e port,
                                                          adi_fpga910x_DmaCfg_t *dmaCfg);

/**
 * \brief Configure dma without blocking, use status to wait subsequently
 *
 * \param[in] fpga910x  Context variable - Pointer to the FPGA910X device data structure
 * \param[in] port      The port to initiate the transfer
 * \param[in] channel   The channel of the specified port to configure
 * \param[in] dmaCfg    The DMA configuration to configure the data chain
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_DataChain_Dma_Configure(adi_fpga910x_Device_t *fpga910x,
                                             adi_common_Port_e port,
                                             adi_common_ChannelNumber_e channel,
                                             adi_fpga910x_DmaCfg_t *dmaCfg);

/**
 * \brief Set the enabledness of DMA_ENABLE and/or DMA_TRIGGER of Channel1 as master for Channel2 This ensures the
 *        DMA_ENABLE and/or DMA_TRIGGER signals have identical timing on both channels
 *
 * \param[in] fpga910x               Context variable - Pointer to the FPGA910X device data structure
 * \param[in] port                   The port to set the enabledness
 * \param[in] enableMasterToEnable   Set to TRUE to enable Channel1 DMA_ENABLE as master for Channel2
 * \param[in] enableMasterToTrigger  Set to TRUE to enable Channel1 DMA_TRIGGER as master for Channel2
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_DataChain_Dma_Master_Set(adi_fpga910x_Device_t *fpga910x,
                                              adi_common_Port_e port,
                                              bool enableMasterToEnable,
                                              bool enableMasterToTrigger);

/**
 * \brief Get the enabledness of DMA_ENABLE and DMA_TRIGGER of Channel1 as master for Channel2
 *
 * \param[in]  fpga910x               Context variable - Pointer to the FPGA910X device data structure
 * \param[in]  port                   The port to get the enabledness
 * \param[out] enableMasterToEnable   Returns as TRUE if Channel1 DMA_ENABLE is set as master for Channel 2
 * \param[out] enableMasterToTrigger  Returns as TRUE if Channel1 DMA_TRIGGER is set as master for Channel 2
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_DataChain_Dma_Master_Get(adi_fpga910x_Device_t *fpga910x,
                                              adi_common_Port_e port,
                                              bool *enableMasterToEnable,
                                              bool *enableMasterToTrigger);

/**
 * \brief Status of dma with blocking, use status above for non-blocking (?)
 *
 * \param[in] fpga910x  Context variable - Pointer to the FPGA910X device data structure
 * \param[in] port      The port to initiate the transfer
 * \param[in] channel   The channel of the specified port to configure
 * \param[in] dmaCfg    The DMA configuration to configure the data chain
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_DataChain_Dma_Wait(adi_fpga910x_Device_t *fpga910x,
                                        adi_common_Port_e port,
                                        adi_common_ChannelNumber_e channel,
                                        adi_fpga910x_DmaCfg_t *dmaCfg);

/**
 * \brief Read data from FPGA capture memory for 22I22Q mode, gain change, and gain index
 *
 * In 22I22Q, gain change, gain index mode, data is packed into RAM in the format:
 * Address (offset) | Data
 * -------------------------------------------
 * 0x0 | 32'{22I0[21:0], 1'b0, GC0[0], GI0[7:0]}
 * 0x4 | 32'{22Q0[21:0], 1'b0, GC0[0], GI0[7:0]}
 * 0x8 | 32'{22I1[21:0], 1'b0, GC1[0], GI1[7:0]}
 * 0xC | 32'{22Q1[21:0], 1'b0, GC1[0], GI1[7:0]}
 * ... | ...
 *
 * This function unpacks the I and Q data into separate 22-bit arrays for convenient indexing
 *
 * \param[in]  fpga910x        Context variable - Pointer to the FPGA910X device data structure
 * \param[in]  port            The port that the channel refers to
 * \param[in]  channel         The channel of the specified port to configure
 * \param[out] iData           The returned 22-bit I values
 * \param[out] qData           The returned 22-bit Q values
 * \param[out] gainChangeData  The returned 1-bit gain change values
 *                             Gain change data format:
 *                             gainControlData[index] | Value
 *                             -------------------------------
 *                             0 | GC0
 *                             1 | GC1
 *                             2 | GC2
 *                             3 | GC3
 *                             ... | ...
 * \param[out] gainIndexData   The returned 8-bit gain index values
 *                             gainIndexData[index] | Value
 *                             -------------------------------
 *                             0 | GI0
 *                             1 | GI1
 *                             2 | GI2
 *                             3 | GI3
 *                             ... | ...
 * \param[in]  length          The length of the iData, qData, gainChangeData, and gainIndexData arrays; i.e., the number of IQ samples
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_DataChain_Data_Get_22I22Q_GainChange_GainIndex(adi_fpga910x_Device_t *fpga910x,
                                                                    adi_common_Port_e port,
                                                                    adi_common_ChannelNumber_e channel,
                                                                    uint32_t iData[],
                                                                    uint32_t qData[],
                                                                    uint8_t gainChangeData[],
                                                                    uint8_t gainIndexData[],
                                                                    uint32_t length);

/**
 * \brief Read data from FPGA capture memory for 22I22Q mode, gain change, and gain index
 *
 * In 22I22Q, gain change, gain index mode, data is packed into RAM in the format:
 * Address (offset) | Data
 * -------------------------------------------
 * 0x0 | 32'{22I0[21:0], 1'b0, GC0[0], GI0[7:0]}
 * 0x4 | 32'{22Q0[21:0], 1'b0, GC0[0], GI0[7:0]}
 * 0x8 | 32'{22I1[21:0], 1'b0, GC1[0], GI1[7:0]}
 * 0xC | 32'{22Q1[21:0], 1'b0, GC1[0], GI1[7:0]}
 * ... | ...
 *
 * This function unpacks the I and Q data into separate 22-bit arrays for convenient indexing and converts from two's
 * complement to signed
 *
 * \param[in]  fpga910x        Context variable - Pointer to the FPGA910X device data structure
 * \param[in]  port            The port that the channel refers to
 * \param[in]  channel         The channel of the specified port to configure
 * \param[out] iData           The returned 22-bit I values
 * \param[out] qData           The returned 22-bit Q values
 * \param[out] gainChangeData  The returned 1-bit gain change values
 *                             Gain change data format:
 *                             gainControlData[index] | Value
 *                             -------------------------------
 *                             0 | GC0
 *                             1 | GC1
 *                             2 | GC2
 *                             3 | GC3
 *                             ... | ...
 * \param[out] gainIndexData   The returned 8-bit gain index values
 *                             gainIndexData[index] | Value
 *                             -------------------------------
 *                             0 | GI0
 *                             1 | GI1
 *                             2 | GI2
 *                             3 | GI3
 *                             ... | ...
 * \param[in]  length          The length of the iData, qData, gainChangeData, and gainIndexData arrays; i.e., the number of IQ samples
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_DataChain_Data_Get_22I22QSigned_GainChange_GainIndex(adi_fpga910x_Device_t *fpga910x,
                                                                          adi_common_Port_e port,
                                                                          adi_common_ChannelNumber_e channel,
                                                                          int32_t iData[],
                                                                          int32_t qData[],
                                                                          uint8_t gainChangeData[],
                                                                          uint8_t gainIndexData[],
                                                                          uint32_t length);

/**
 * \brief Read data from FPGA capture memory for 22I22Q mode, gain change, and gain index
 *
 * In 22I22Q, gain change, gain index mode, data is packed into RAM in the format:
 * Address (offset) | Data
 * -------------------------------------------
 * 0x0 | 32'{22I0[21:0], 1'b0, GC0[0], GI0[7:0]}
 * 0x4 | 32'{22Q0[21:0], 1'b0, GC0[0], GI0[7:0]}
 * 0x8 | 32'{22I1[21:0], 1'b0, GC1[0], GI1[7:0]}
 * 0xC | 32'{22Q1[21:0], 1'b0, GC1[0], GI1[7:0]}
 * ... | ...
 *
 * This function unpacks the I and Q data into separate 22-bit arrays for convenient indexing
 *
 * \param[in]  fpga910x        Context variable - Pointer to the FPGA910X device data structure
 * \param[in]  port            The port that the channel refers to
 * \param[in]  channel         The channel of the specified port to configure
 *                             Interleaved I and Q data in the format:
 *                             iqData[index] | Value
 *                             ---------------------
 *                             0 | I0
 *                             1 | Q0
 *                             2 | I1
 *                             3 | Q1
 *                             ... | ...
 * \param[out] iqData
 * \param[out] gainChangeData  The returned 1-bit gain change values
 *                             Gain change data format:
 *                             gainControlData[index] | Value
 *                             -------------------------------
 *                             0 | GC0
 *                             1 | GC0
 *                             2 | GC1
 *                             3 | GC1
 *                             ... | ...
 * \param[out] gainIndexData   The returned 8-bit gain index values
 *                             gainIndexData[index] | Value
 *                             -------------------------------
 *                             0 | GI0
 *                             1 | GI0
 *                             2 | GI1
 *                             3 | GI1
 *                             ... | ...
 * \param[in]  length          The length of the iData, qData, gainChangeData, and gainIndexData arrays; i.e., the number of IQ samples
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_DataChain_Data_Get_22I22QInterleaved_GainChange_GainIndex(adi_fpga910x_Device_t *fpga910x,
                                                                               adi_common_Port_e port,
                                                                               adi_common_ChannelNumber_e channel,
                                                                               uint32_t iqData[],
                                                                               uint8_t gainChangeData[],
                                                                               uint8_t gainIndexData[],
                                                                               uint32_t length);

/**
 * \brief Read data from FPGA capture memory for 22I22Q mode, gain change, and gain index
 *
 * In 22I22Q, gain change, gain index mode, data is packed into RAM in the format:
 * Address (offset) | Data
 * -------------------------------------------
 * 0x0 | 32'{22I0[21:0], 1'b0, GC0[0], GI0[7:0]}
 * 0x4 | 32'{22Q0[21:0], 1'b0, GC0[0], GI0[7:0]}
 * 0x8 | 32'{22I1[21:0], 1'b0, GC1[0], GI1[7:0]}
 * 0xC | 32'{22Q1[21:0], 1'b0, GC1[0], GI1[7:0]}
 * ... | ...
 *
 * This function unpacks the I and Q data into separate 22-bit arrays for convenient indexing and converts from two's
 * complement to signed
 *
 * \param[in]  fpga910x        Context variable - Pointer to the FPGA910X device data structure
 * \param[in]  port            The port that the channel refers to
 * \param[in]  channel         The channel of the specified port to configure
 *                             Interleaved I and Q data in the format:
 *                             iqData[index] | Value
 *                             ---------------------
 *                             0 | I0
 *                             1 | Q0
 *                             2 | I1
 *                             3 | Q1
 *                             ... | ...
 * \param[out] iqData
 * \param[out] gainChangeData  The returned 1-bit gain change values
 *                             Gain change data format:
 *                             gainControlData[index] | Value
 *                             -------------------------------
 *                             0 | GC0
 *                             1 | GC0
 *                             2 | GC1
 *                             3 | GC1
 *                             ... | ...
 * \param[out] gainIndexData   The returned 8-bit gain index values
 *                             gainIndexData[index] | Value
 *                             -------------------------------
 *                             0 | GI0
 *                             1 | GI0
 *                             2 | GI1
 *                             3 | GI1
 *                             ... | ...
 * \param[in]  length          The length of the iData, qData, gainChangeData, and gainIndexData arrays; i.e., the number of IQ samples
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_DataChain_Data_Get_22I22QInterleavedSigned_GainChange_GainIndex(adi_fpga910x_Device_t *fpga910x,
                                                                                     adi_common_Port_e port,
                                                                                     adi_common_ChannelNumber_e channel,
                                                                                     int32_t iqData[],
                                                                                     uint8_t gainChangeData[],
                                                                                     uint8_t gainIndexData[],
                                                                                     uint32_t length);

/**
 * \brief Read data from FPGA capture memory for 16I16Q mode
 *
 * In 16I16Q mode, data is packed into RAM in the format:
 * Address (offset) | Data
 * ----------------------------
 * 0x0 | 16I0,16Q0
 * 0x4 | 16I1,16Q1
 * ... | ...
 *
 * This function unpacks the I and Q data into separate 16-bit arrays for convenient indexing
 *
 * \param[in]  fpga910x  Context variable - Pointer to the FPGA910X device data structure
 * \param[in]  port      The port that the channel refers to
 * \param[in]  channel   The channel of the specified port to configure
 * \param[out] iData     The returned 16-bit I values
 * \param[out] qData     The returned 16-bit Q values
 * \param[in]  length    The length of the iData and qData arrays; i.e., the number of IQ samples
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_DataChain_Data_Get_16I16Q(adi_fpga910x_Device_t *fpga910x,
                                               adi_common_Port_e port,
                                               adi_common_ChannelNumber_e channel,
                                               uint16_t iData[],
                                               uint16_t qData[],
                                               uint32_t length);

/**
 * \brief Read data from FPGA capture memory for 16I16Q mode, converting from two's complement to signed
 *
 * In 16I16Q mode, data is packed into RAM in the format:
 * Address (offset) | Data
 * ----------------------------
 * 0x0 | 16I0,16Q0
 * 0x4 | 16I1,16Q1
 * ... | ...
 *
 * This function unpacks the I and Q data into separate 16-bit arrays for convenient indexing and converts from two's
 * complement to signed
 *
 * \param[in]  fpga910x  Context variable - Pointer to the FPGA910X device data structure
 * \param[in]  port      The port that the channel refers to
 * \param[in]  channel   The channel of the specified port to configure
 * \param[out] iData     The returned 16-bit signed I values
 * \param[out] qData     The returned 16-bit signed Q values
 * \param[in]  length    The length of the iData and qData arrays; i.e., the number of IQ samples
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_DataChain_Data_Get_16I16QSigned(adi_fpga910x_Device_t *fpga910x,
                                                     adi_common_Port_e port,
                                                     adi_common_ChannelNumber_e channel,
                                                     int16_t iData[],
                                                     int16_t qData[],
                                                     uint32_t length);

/**
 * \brief Read data from FPGA capture memory in interleaved format for 16I16Q mode
 *
 * In 16I16Q mode, data is packed into RAM in the format:
 * Address (offset) | Data
 * ----------------------------
 * 0x0 | 16I0,16Q0
 * 0x4 | 16I1,16Q1
 * ... | ...
 *
 * This function unpacks the I and Q data into a single interleaved 16-bit array
 *
 * \param[in]  fpga910x  Context variable - Pointer to the FPGA910X device data structure
 * \param[in]  port      The port that the channel refers to
 * \param[in]  channel   The channel of the specified port to configure
 *                       Interleaved I and Q data in the format:
 *                       iqData[index] | Value
 *                       ---------------------
 *                       0 | I0
 *                       1 | Q0
 *                       2 | I1
 *                       3 | Q1
 *                       ... | ...
 * \param[out] iqData
 * \param[in]  length    The length of the iqData array; i.e., twice the number of IQ samples
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_DataChain_Data_Get_16IQInterleaved(adi_fpga910x_Device_t *fpga910x,
                                                        adi_common_Port_e port,
                                                        adi_common_ChannelNumber_e channel,
                                                        uint16_t iqData[],
                                                        uint32_t length);

/**
 * \brief Read data from FPGA capture memory in interleaved format for 16I16Q mode
 *
 * In 16I16Q mode, data is packed into RAM in the format:
 * Address (offset) | Data
 * ----------------------------
 * 0x0 | 16I0,16Q0
 * 0x4 | 16I1,16Q1
 * ... | ...
 *
 * This function unpacks the I and Q data into a single interleaved 16-bit array and converts from two's complement to
 * signed
 *
 * \param[in]  fpga910x  Context variable - Pointer to the FPGA910X device data structure
 * \param[in]  port      The port that the channel refers to
 * \param[in]  channel   The channel of the specified port to configure
 *                       Interleaved I and Q data in the format:
 *                       iqData[index] | Value
 *                       ---------------------
 *                       0 | I0
 *                       1 | Q0
 *                       2 | I1
 *                       3 | Q1
 *                       ... | ...
 * \param[out] iqData
 * \param[in]  length    The length of the iqData array; i.e., twice the number of IQ samples
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_DataChain_Data_Get_16IQInterleavedSigned(adi_fpga910x_Device_t *fpga910x,
                                                              adi_common_Port_e port,
                                                              adi_common_ChannelNumber_e channel,
                                                              int16_t iqData[],
                                                              uint32_t length);

/**
 * \brief Read data from FPGA capture memory in for 16I Only mode
 *
 * In 16I Only mode, data is packed into RAM in the format:
 * Address (offset) | Data
 * ----------------------------
 * 0x0 | 16I1,16I0
 * 0x4 | 16I3,16I2
 * ... | ...
 *
 * This function unpacks the I data into a 16-bit array for convenient indexing
 *
 * \param[in]  fpga910x  Context variable - Pointer to the FPGA910X device data structure
 * \param[in]  port      The port that the channel refers to
 * \param[in]  channel   The channel of the specified port to configure
 *                       I data in the format:
 *                       iData[index] | Value
 *                       ---------------------
 *                       0 | I0
 *                       1 | I1
 *                       2 | I2
 *                       3 | I3
 *                       ... | ...
 * \param[out] iData
 * \param[in]  length    The length of the iData array; i.e., the number of I samples
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_DataChain_Data_Get_16I(adi_fpga910x_Device_t *fpga910x,
                                            adi_common_Port_e port,
                                            adi_common_ChannelNumber_e channel,
                                            uint16_t iData[],
                                            uint32_t length);

/**
 * \brief Read data from FPGA capture memory in for 16I Only mode
 *
 * In 16I Only mode, data is packed into RAM in the format:
 * Address (offset) | Data
 * ----------------------------
 * 0x0 | 16I1,16I0
 * 0x4 | 16I3,16I2
 * ... | ...
 *
 * This function unpacks the I data into a 16-bit array for convenient indexing and converts from two's complement to
 * signed
 *
 * \param[in]  fpga910x  Context variable - Pointer to the FPGA910X device data structure
 * \param[in]  port      The port that the channel refers to
 * \param[in]  channel   The channel of the specified port to configure
 *                       I data in the format:
 *                       iData[index] | Value
 *                       ---------------------
 *                       0 | I0
 *                       1 | I1
 *                       2 | I2
 *                       3 | I3
 *                       ... | ...
 * \param[out] iData
 * \param[in]  length    The length of the iData array; i.e., the number of I samples
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_DataChain_Data_Get_16ISigned(adi_fpga910x_Device_t *fpga910x,
                                                  adi_common_Port_e port,
                                                  adi_common_ChannelNumber_e channel,
                                                  int16_t iData[],
                                                  uint32_t length);

/**
 * \brief Read data from FPGA capture memory for 15I15Q mode and gain change
 *
 * In 15I15Q and gain change mode, data is packed into RAM in the format:
 * Address (offset) | Data
 * -------------------------------------------
 * 0x0 | 32'{15I0[14:0], GC0[0], 15Q0[14:0], GC0[0]}
 * 0x4 | 32'{15I1[14:0], GC1[0], 15Q1[14:0], GC1[0]}
 * ... | ...
 *
 * This function unpacks the I and Q data into separate 16-bit arrays for convenient indexing
 *
 * \param[in]  fpga910x        Context variable - Pointer to the FPGA910X device data structure
 * \param[in]  port            The port that the channel refers to
 * \param[in]  channel         The channel of the specified port to configure
 * \param[out] iData           The returned 15-bit I values
 * \param[out] qData           The returned 15-bit Q values
 * \param[out] gainChangeData  The returned 1-bit gain change values
 *                             Gain change data format:
 *                             gainControlData[index] | Value
 *                             -------------------------------
 *                             0 | GC0
 *                             1 | GC1
 *                             2 | GC2
 *                             3 | GC3
 *                             ... | ...
 * \param[in]  length          The length of the iData, qData, and gainChangeData arrays; i.e., the number of IQ samples
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_DataChain_Data_Get_15I15Q_GainChange(adi_fpga910x_Device_t *fpga910x,
                                                          adi_common_Port_e port,
                                                          adi_common_ChannelNumber_e channel,
                                                          uint16_t iData[],
                                                          uint16_t qData[],
                                                          uint8_t gainChangeData[],
                                                          uint32_t length);

/**
 * \brief Read data from FPGA capture memory for 15I15Q mode and gain change
 *
 * In 15I15Q and gain change mode, data is packed into RAM in the format:
 * Address (offset) | Data
 * -------------------------------------------
 * 0x0 | 32'{15I0[14:0], GC0[0], 15Q0[14:0], GC0[0]}
 * 0x4 | 32'{15I1[14:0], GC1[0], 15Q1[14:0], GC1[0]}
 * ... | ...
 *
 * This function unpacks the I and Q data into separate 16-bit arrays for convenient indexing and converts from two's
 * complement to signed
 *
 * \param[in]  fpga910x        Context variable - Pointer to the FPGA910X device data structure
 * \param[in]  port            The port that the channel refers to
 * \param[in]  channel         The channel of the specified port to configure
 * \param[out] iData           The returned 15-bit I values
 * \param[out] qData           The returned 15-bit Q values
 * \param[out] gainChangeData  The returned 1-bit gain change values
 *                             Gain change data format:
 *                             gainControlData[index] | Value
 *                             -------------------------------
 *                             0 | GC0
 *                             1 | GC1
 *                             2 | GC2
 *                             3 | GC3
 *                             ... | ...
 * \param[in]  length          The length of the iData, qData, and gainChangeData arrays; i.e., the number of IQ samples
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_DataChain_Data_Get_15I15QSigned_GainChange(adi_fpga910x_Device_t *fpga910x,
                                                                adi_common_Port_e port,
                                                                adi_common_ChannelNumber_e channel,
                                                                int16_t iData[],
                                                                int16_t qData[],
                                                                uint8_t gainChangeData[],
                                                                uint32_t length);

/**
 * \brief Read data from FPGA capture memory in interleaved format for 15I15Q mode and gain change
 *
 * In 15I15Q and gain change mode, data is packed into RAM in the format:
 * Address (offset) | Data
 * -------------------------------------------
 * 0x0 | 32'{15I0[14:0], GC0[0], 15Q0[14:0], GC0[0]}
 * 0x4 | 32'{15I1[14:0], GC1[0], 15Q1[14:0], GC1[0]}
 * ... | ...
 *
 * This function unpacks the I and Q data into separate 16-bit arrays for convenient indexing
 *
 * \param[in]  fpga910x        Context variable - Pointer to the FPGA910X device data structure
 * \param[in]  port            The port that the channel refers to
 * \param[in]  channel         The channel of the specified port to configure
 *                             Interleaved I and Q data in the format:
 *                             iqData[index] | Value
 *                             ---------------------
 *                             0 | I0
 *                             1 | Q0
 *                             2 | I1
 *                             3 | Q1
 *                             ... | ...
 * \param[out] iqData
 * \param[out] gainChangeData  The returned 1-bit gain change values
 *                             Gain change data format:
 *                             gainControlData[index] | Value
 *                             -------------------------------
 *                             0 | GC0
 *                             1 | GC0
 *                             2 | GC1
 *                             3 | GC1
 *                             ... | ...
 * \param[in]  length          The length of the iqData array; i.e., twice the number of IQ samples
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_DataChain_Data_Get_15IQInterleaved_GainChange(adi_fpga910x_Device_t *fpga910x,
                                                                   adi_common_Port_e port,
                                                                   adi_common_ChannelNumber_e channel,
                                                                   uint16_t iqData[],
                                                                   uint8_t gainChangeData[],
                                                                   uint32_t length);

/**
 * \brief Read data from FPGA capture memory in interleaved format for 15I15Q mode and gain change
 *
 * In 15I15Q and gain change mode, data is packed into RAM in the format:
 * Address (offset) | Data
 * -------------------------------------------
 * 0x0 | 32'{15I0[14:0], GC0[0], 15Q0[14:0], GC0[0]}
 * 0x4 | 32'{15I1[14:0], GC1[0], 15Q1[14:0], GC1[0]}
 * ... | ...
 *
 * This function unpacks the I and Q data into separate 16-bit arrays for convenient indexing and converts from two's
 * complement to signed
 *
 * \param[in]  fpga910x        Context variable - Pointer to the FPGA910X device data structure
 * \param[in]  port            The port that the channel refers to
 * \param[in]  channel         The channel of the specified port to configure
 *                             Interleaved I and Q data in the format:
 *                             iqData[index] | Value
 *                             ---------------------
 *                             0 | I0
 *                             1 | Q0
 *                             2 | I1
 *                             3 | Q1
 *                             ... | ...
 * \param[out] iqData
 * \param[out] gainChangeData  The returned 1-bit gain change values
 *                             Gain change data format:
 *                             gainControlData[index] | Value
 *                             -------------------------------
 *                             0 | GC0
 *                             1 | GC0
 *                             2 | GC1
 *                             3 | GC1
 *                             ... | ...
 * \param[in]  length          The length of the iqData array; i.e., twice the number of IQ samples
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_DataChain_Data_Get_15IQInterleavedSigned_GainChange(adi_fpga910x_Device_t *fpga910x,
                                                                         adi_common_Port_e port,
                                                                         adi_common_ChannelNumber_e channel,
                                                                         int16_t iqData[],
                                                                         uint8_t gainChangeData[],
                                                                         uint32_t length);

/**
 * \brief Read data from FPGA capture memory in for 8-Bit Symbol mode
 *
 * In 8-Bit Symbol mode, data is packed into RAM in the format:
 * Address (offset) | Data
 * ----------------------------
 * 0x0 | 8S3,8S2,8S1,8S0
 * 0x4 | 8S7,8S6,8S5,8S4
 * ... | ...
 *
 * This function unpacks the data into an 8-bit array for convenient indexing
 *
 * \param[in] fpga910x  Context variable - Pointer to the FPGA910X device data structure
 * \param[in] port      The port that the channel refers to
 * \param[in] channel   The channel of the specified port to configure
 *                      Symbol data in the format:
 *                      symbols[index] | Value
 *                      ---------------------
 *                      0 | S0
 *                      1 | S1
 *                      2 | S2
 *                      3 | S3
 *                      ... | ...
 * \param[in] symbols
 * \param[in] length    The length of the symbols array
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_DataChain_Data_Get_8S(adi_fpga910x_Device_t *fpga910x,
                                           adi_common_Port_e port,
                                           adi_common_ChannelNumber_e channel,
                                           uint8_t symbols[],
                                           uint32_t length);

/**
 * \brief Read gain index from FPGA capture memory in for 8-Bit Symbol mode
 *
 * In 8-Bit Symbol mode, data is packed into RAM in the format:
 * Address (offset) | Data
 * ----------------------------
 * 0x0 | 8S3,8S2,8S1,8S0
 * 0x4 | 8S7,8S6,8S5,8S4
 * ... | ...
 *
 * This function unpacks the data into an 8-bit array for convenient indexing with MSB set to '1' always for gain index and
 * extracts gain index change bits in a separate array
 *
 * \param[in]  fpga910x       Context variable - Pointer to the FPGA910X device data structure
 * \param[in]  channel        The channel of the specified port to configure
 * \param[out] gainChangeBit  Gain index change indicator; Toggles from 0 -> 1 or 1 -> 0 when there is a change in gain index
 * \param[out] symbols        Current gain index read
 *                            Symbol data in the format:
 *                            symbols[index] | Value
 *                            ---------------------
 *                            0 | S0
 *                            1 | S1
 *                            2 | S2
 *                            3 | S3
 *                            ... | ...
 * \param[in]  length         The length of the symbols array
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_DataChain_GainIndex_Get_8S(adi_fpga910x_Device_t *fpga910x,
                                                adi_common_ChannelNumber_e channel,
                                                uint8_t gainChangeBit[],
                                                uint8_t symbols[],
                                                uint32_t length);

/**
 * \brief Read data from FPGA capture memory in for 2-Bit Symbol mode
 *
 * In 2-Bit Symbol mode, data is packed into RAM in the format:
 * Address (offset) | Data
 * ----------------------------
 * 0x0 | 6X,2S3,6X,2S2,6X,2S1,6X,2S0
 * 0x4 | 6X,2S7,6X,2S6,6X,2S5,6X,2S4
 * ... | ...
 *
 * This function unpacks the data into an 8-bit array containing 2 bit symbols for convenient indexing
 *
 * \param[in] fpga910x  Context variable - Pointer to the FPGA910X device data structure
 * \param[in] port      The port that the channel refers to
 * \param[in] channel   The channel of the specified port to configure
 *                      Symbol data in the format:
 *                      symbols[index] | Value
 *                      ---------------------
 *                      0 | S0
 *                      1 | S1
 *                      2 | S2
 *                      3 | S3
 *                      ... | ...
 * \param[in] symbols
 * \param[in] length    The length of the symbols array
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_DataChain_Data_Get_2S(adi_fpga910x_Device_t *fpga910x,
                                           adi_common_Port_e port,
                                           adi_common_ChannelNumber_e channel,
                                           uint8_t symbols[],
                                           uint32_t length);

/**
 * \brief Write data to FPGA transmit memory for 16I16Q mode
 *
 * In 16I16Q mode, data is packed into RAM in the format:
 * Address (offset) | Data
 * ----------------------------
 * 0x0 | 16I0,16Q0
 * 0x4 | 16I1,16Q1
 * ... | ...
 *
 * This function takes separate 16-bit arrays of I and Q data and packs them into RAM appropriately
 *
 * \param[in] fpga910x  Context variable - Pointer to the FPGA910X device data structure
 * \param[in] port      The port that the channel refers to
 * \param[in] channel   The channel of the specified port to configure
 * \param[in] iData     Array of 16-bit I values to be packed into RAM
 * \param[in] qData     Array of 16-bit Q values to be packed into RAM
 * \param[in] length    The length of the iData and qData arrays; i.e., the number of IQ samples
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_DataChain_Data_Set_16I16Q(adi_fpga910x_Device_t *fpga910x,
                                               adi_common_Port_e port,
                                               adi_common_ChannelNumber_e channel,
                                               uint16_t iData[],
                                               uint16_t qData[],
                                               uint32_t length);

/**
 * \brief Write data to FPGA transmit memory for 16I16Q mode, converting from signed to two's complement
 *
 * In 16I16Q mode, data is packed into RAM in the format:
 * Address (offset) | Data
 * ----------------------------
 * 0x0 | 16I0,16Q0
 * 0x4 | 16I1,16Q1
 * ... | ...
 *
 * This function takes separate 16-bit arrays of signed I and Q data, converts them to two's complement, and packs them into RAM appropriately
 *
 * \param[in] fpga910x  Context variable - Pointer to the FPGA910X device data structure
 * \param[in] port      The port that the channel refers to
 * \param[in] channel   The channel of the specified port to configure
 * \param[in] iData     Array of 16-bit I values to be packed into RAM
 * \param[in] qData     Array of 16-bit Q values to be packed into RAM
 * \param[in] length    The length of the iData and qData arrays; i.e., the number of IQ samples
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_DataChain_Data_Set_16I16QSigned(adi_fpga910x_Device_t *fpga910x,
                                                     adi_common_Port_e port,
                                                     adi_common_ChannelNumber_e channel,
                                                     int16_t iData[],
                                                     int16_t qData[],
                                                     uint32_t length);

/**
 * \brief Write data to FPGA transmit memory from interleaved format for 16I16Q mode
 *
 * In 16I16Q mode, data is packed into RAM in the format:
 * Address (offset) | Data
 * ----------------------------
 * 0x0 | 16I0,16Q0
 * 0x4 | 16I1,16Q1
 * ... | ...
 *
 * This function takes a single 16-bit array of interleaved I and Q data and packs it into RAM appropriately
 *
 * \param[in] fpga910x  Context variable - Pointer to the FPGA910X device data structure
 * \param[in] port      The port that the channel refers to
 * \param[in] channel   The channel of the specified port to configure
 *                      Interleaved I and Q data in the format:
 *                      iqData[index] | Value
 *                      ---------------------
 *                      0 | I0
 *                      1 | Q0
 *                      2 | I1
 *                      3 | Q1
 *                      ... | ...
 * \param[in] iqData
 * \param[in] length    The length of the iqData array; i.e., twice the number of IQ samples
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_DataChain_Data_Set_16IQInterleaved(adi_fpga910x_Device_t *fpga910x,
                                                        adi_common_Port_e port,
                                                        adi_common_ChannelNumber_e channel,
                                                        uint16_t iqData[],
                                                        uint32_t length);

/**
 * \brief Write data to FPGA transmit memory from interleaved format for 16I16Q mode, converting from signed to two's
 *        complement
 *
 * In 16I16Q mode, data is packed into RAM in the format:
 * Address (offset) | Data
 * ----------------------------
 * 0x0 | 16I0,16Q0
 * 0x4 | 16I1,16Q1
 * ... | ...
 *
 * This function takes a single 16-bit array of signed, interleaved I and Q data, converts it to two's complement, and packs it into RAM appropriately
 *
 * \param[in] fpga910x  Context variable - Pointer to the FPGA910X device data structure
 * \param[in] port      The port that the channel refers to
 * \param[in] channel   The channel of the specified port to configure
 *                      Interleaved I and Q data in the format:
 *                      iqData[index] | Value
 *                      ---------------------
 *                      0 | I0
 *                      1 | Q0
 *                      2 | I1
 *                      3 | Q1
 *                      ... | ...
 * \param[in] iqData
 * \param[in] length    The length of the iqData array; i.e., twice the number of IQ samples
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_DataChain_Data_Set_16IQInterleavedSigned(adi_fpga910x_Device_t *fpga910x,
                                                              adi_common_Port_e port,
                                                              adi_common_ChannelNumber_e channel,
                                                              int16_t iqData[],
                                                              uint32_t length);

/**
 * \brief Write data to FPGA transmit memory for 16I Only mode
 *
 * In 16I Only mode, data is packed into RAM in the format:
 * Address (offset) | Data
 * ----------------------------
 * 0x0 | 16I1,16I0
 * 0x4 | 16I3,16I2
 * ... | ...
 *
 * This function takes a 16-bit array of I data and packs it into RAM appropriately
 *
 * \param[in] fpga910x  Context variable - Pointer to the FPGA910X device data structure
 * \param[in] port      The port that the channel refers to
 * \param[in] channel   The channel of the specified port to configure
 *                      I data in the format:
 *                      iData[index] | Value
 *                      ---------------------
 *                      0 | I0
 *                      1 | I1
 *                      2 | I2
 *                      3 | I3
 *                      ... | ...
 * \param[in] iData
 * \param[in] length    The length of the iData array; i.e., the number of I samples
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_DataChain_Data_Set_16I(adi_fpga910x_Device_t *fpga910x,
                                            adi_common_Port_e port,
                                            adi_common_ChannelNumber_e channel,
                                            uint16_t iData[],
                                            uint32_t length);

/**
 * \brief Write data to FPGA transmit memory for 16I Only mode, converting from signed to two's complement
 *
 * In 16I Only mode, data is packed into RAM in the format:
 * Address (offset) | Data
 * ----------------------------
 * 0x0 | 16I1,16I0
 * 0x4 | 16I3,16I2
 * ... | ...
 *
 * This function takes a 16-bit array of signed I data, converts it to two's complement, and packs it into RAM appropriately
 *
 * \param[in] fpga910x  Context variable - Pointer to the FPGA910X device data structure
 * \param[in] port      The port that the channel refers to
 * \param[in] channel   The channel of the specified port to configure
 *                      I data in the format:
 *                      iData[index] | Value
 *                      ---------------------
 *                      0 | I0
 *                      1 | I1
 *                      2 | I2
 *                      3 | I3
 *                      ... | ...
 * \param[in] iData
 * \param[in] length    The length of the iData array; i.e., the number of I samples
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_DataChain_Data_Set_16ISigned(adi_fpga910x_Device_t *fpga910x,
                                                  adi_common_Port_e port,
                                                  adi_common_ChannelNumber_e channel,
                                                  int16_t iData[],
                                                  uint32_t length);

/**
 * \brief Write data to FPGA transmit memory for 8-Bit Symbol mode
 *
 * In 8-Bit Symbol mode, data is packed into RAM in the format:
 * Address (offset) | Data
 * ----------------------------
 * 0x0 | 8S3,8S2,8S1,8S0
 * 0x4 | 8S7,8S6,8S5,8S4
 * ... | ...
 *
 * This function takes an 8-bit array of symbols and packs it into RAM appropriately
 *
 * \param[in] fpga910x  Context variable - Pointer to the FPGA910X device data structure
 * \param[in] port      The port that the channel refers to
 * \param[in] channel   The channel of the specified port to configure
 *                      Symbol data in the format:
 *                      symbols[index] | Value
 *                      ---------------------
 *                      0 | S0
 *                      1 | S1
 *                      2 | S2
 *                      3 | S3
 *                      ... | ...
 * \param[in] symbols
 * \param[in] length    The length of the symbols array
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_DataChain_Data_Set_8S(adi_fpga910x_Device_t *fpga910x,
                                           adi_common_Port_e port,
                                           adi_common_ChannelNumber_e channel,
                                           uint8_t symbols[],
                                           uint32_t length);

/**
 * \brief Write data to FPGA transmit memory for 2-Bit Symbol mode
 *
 * In 2-Bit Symbol mode, data is packed into RAM in the format:
 * Address (offset) | Data
 * ----------------------------
 * 0x0 | 6X,2S3,6X,2S2,6X,2S1,6X,2S0
 * 0x4 | 6X,2S7,6X,2S6,6X,2S5,6X,2S4
 * ... | ...
 *
 * This function takes an 8-bit array of 2-bit symbols and packs it into RAM appropriately
 *
 * \param[in] fpga910x  Context variable - Pointer to the FPGA910X device data structure
 * \param[in] port      The port that the channel refers to
 * \param[in] channel   The channel of the specified port to configure
 *                      Symbol data in the format:
 *                      symbols[index] | Value
 *                      ---------------------
 *                      0 | S0
 *                      1 | S1
 *                      2 | S2
 *                      3 | S3
 *                      ... | ...
 * \param[in] symbols
 * \param[in] length    The length of the symbols array
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_DataChain_Data_Set_2S(adi_fpga910x_Device_t *fpga910x,
                                           adi_common_Port_e port,
                                           adi_common_ChannelNumber_e channel,
                                           uint8_t symbols[],
                                           uint32_t length);

/**
 * \brief Write FH table to FPGA transmit memory in 8-bit symbol mode
 *
 * In 8-Bit Symbol mode, data is packed into RAM in the format:
 * Address (offset) | Data
 * ----------------------------
 * 0x0 | 8S3,8S2,8S1,8S0
 * 0x4 | 8S7,8S6,8S5,8S4
 * ... | ...
 *
 * This function takes an 8-bit array of symbols and packs it into RAM appropriately
 *
 * \param[in] fpga910x          Context variable - Pointer to the FPGA910X device data structure
 * \param[in] spiPackedFhTable  SPI packed FH table
 *                              Symbol data in the format:
 *                              symbols[index] | Value
 *                              ---------------------
 *                              0 | S0
 *                              1 | S1
 *                              2 | S2
 *                              3 | S3
 *                              ... | ...
 * \param[in] byteCount         The length of the symbols array
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_DataChain_HopTable_Set_8S(adi_fpga910x_Device_t *fpga910x,
                                               uint8_t spiPackedFhTable[],
                                               uint32_t byteCount);

/**
 * \brief One-time setup for adi_fpga910x_DataChain_SsiSend: configure its trigger pin and
 *        remember the channel/pin/timeout to use on every subsequent SsiSend call.
 *
 * Must be called once before the first adi_fpga910x_DataChain_SsiSend call. Configures
 * triggerPin as a software input (a one-time hardware register write) and stores channel,
 * triggerPin, and timeoutMs so SsiSend itself does not need to re-validate or re-touch the
 * GPIO pin's configuration on every call.
 *
 * \param[in] fpga910x    Context variable - Pointer to the FPGA910X device data structure
 * \param[in] channel     The Tx channel SsiSend will use
 * \param[in] triggerPin  The FPGA910X DGPIO pin SsiSend will wait on for a level change
 * \param[in] timeoutMs   Maximum time SsiSend will wait for the trigger level change before
 *                        returning an error
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_DataChain_SsiSend_Configure(adi_fpga910x_Device_t *fpga910x,
                                                 adi_common_ChannelNumber_e channel,
                                                 adi_fpga910x_GpioPin_e triggerPin,
                                                 uint32_t timeoutMs);

/**
 * \brief Stage iqData for transmission, then block until the configured trigger pin's next
 *        level change, then perform exactly one Tx SSI transfer of that data.
 *
 * adi_fpga910x_DataChain_SsiSend_Configure must be called once before the first call to this
 * function. Unlike channel/triggerPin/timeoutMs (fixed at Configure time), iqData/length are
 * expected to change every call, so they're pushed fresh here via
 * adi_fpga910x_DataChain_Data_Set_16IQInterleaved - BEFORE waiting for the trigger level
 * change, so that write is well clear of it instead of racing the one-shot DMA re-arm. The
 * wait polls triggerPin against the level it was resting at when this call started, and the
 * first sample that differs - in
 * either direction - is the trigger. The DMA's registers are already programmed by then, so
 * detecting the change immediately begins a one-shot Tx DMA transfer (continuous=false,
 * trigger=IMMEDIATE, captureType=DISABLE), and this function blocks until that transfer
 * finishes before returning.
 *
 * \param[in] fpga910x  Context variable - Pointer to the FPGA910X device data structure
 * \param[in] iqData    This call's I/Q samples to transmit, interleaved (I0,Q0,I1,Q1,...)
 * \param[in] length    Number of uint16_t elements in iqData (2x the number of complex samples)
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_DataChain_SsiSend(adi_fpga910x_Device_t *fpga910x,
                                       uint16_t iqData[],
                                       uint32_t length);

/**
 * \brief One-time setup for adi_fpga910x_DataChain_SsiReceive: configure its trigger pin and
 *        remember the channel/pin/timeout to use on every subsequent SsiReceive call.
 *
 * Must be called once before the first adi_fpga910x_DataChain_SsiReceive call. Configures
 * triggerPin as a software input (a one-time hardware register write) and stores channel,
 * triggerPin, and timeoutMs so SsiReceive itself does not need to re-validate or re-touch the
 * GPIO pin's configuration on every call. The capture size is NOT configured here - it comes
 * from SsiReceive's own per-call length instead, so there's one number driving both the DMA
 * capture size and the read-back size rather than two that would need to stay in sync.
 *
 * \param[in] fpga910x    Context variable - Pointer to the FPGA910X device data structure
 * \param[in] channel     The Rx channel SsiReceive will use
 * \param[in] triggerPin  The FPGA910X DGPIO pin SsiReceive will wait on for a level change
 * \param[in] timeoutMs   Maximum time SsiReceive will wait for the trigger level change
 *                        before returning an error
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_DataChain_SsiReceive_Configure(adi_fpga910x_Device_t *fpga910x,
                                                    adi_common_ChannelNumber_e channel,
                                                    adi_fpga910x_GpioPin_e triggerPin,
                                                    uint32_t timeoutMs);

/**
 * \brief Block until the configured trigger pin's next level change, then perform exactly one
 *        Rx SSI capture sized by length and drain this call's captured samples into iqData.
 *
 * adi_fpga910x_DataChain_SsiReceive_Configure must be called once before the first call to
 * this function. Waits for a level change on the configured trigger pin, bounded by the
 * configured timeout: the pin is sampled once at
 * entry to establish its resting level, then polled until a sample differs from it in either
 * direction. The DMA's registers are already programmed by then, so detecting the change
 * immediately begins a one-shot Rx DMA capture (continuous=false, trigger=IMMEDIATE,
 * captureType=RX_DATA, length derived directly from this call's own length parameter), and
 * this function blocks until that transfer finishes. Unlike channel/triggerPin/timeoutMs
 * (fixed at Configure time), iqData/length are decided fresh every call - iqData holds this
 * call's freshly-captured samples, drained via adi_fpga910x_DataChain_Data_Get_16IQInterleaved
 * only AFTER the transfer finishes, since that's the earliest point the data is valid.
 *
 * \param[in]  fpga910x  Context variable - Pointer to the FPGA910X device data structure
 * \param[out] iqData    This call's captured I/Q samples, interleaved (I0,Q0,I1,Q1,...)
 * \param[in]  length    Number of uint16_t elements in iqData (2x the number of complex
 *                       samples/32-bit words to capture this call)
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_DataChain_SsiReceive(adi_fpga910x_Device_t *fpga910x,
                                          uint16_t iqData[],
                                          uint32_t length);

/**
 * \brief Undo adi_fpga910x_DataChain_SsiSend_Configure and mark SsiSend as unconfigured.
 *
 * Should be called before SsiSend_Configure is called again, so the re-configuration is
 * explicit rather than silently reusing the previous channel/triggerPin/timeoutMs.
 *
 * \param[in] fpga910x  Context variable - Pointer to the FPGA910X device data structure
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_DataChain_SsiSend_ConfigDeinit(adi_fpga910x_Device_t *fpga910x);

/**
 * \brief Undo adi_fpga910x_DataChain_SsiReceive_Configure and mark SsiReceive as unconfigured.
 *
 * Should be called before SsiReceive_Configure is called again, so the re-configuration is
 * explicit rather than silently reusing the previous channel/triggerPin/timeoutMs.
 *
 * \param[in] fpga910x  Context variable - Pointer to the FPGA910X device data structure
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_DataChain_SsiReceive_ConfigDeinit(adi_fpga910x_Device_t *fpga910x);

#ifdef __cplusplus
}
#endif

#endif /* _ADI_FPGA910X_DATACHAIN_H_ */
