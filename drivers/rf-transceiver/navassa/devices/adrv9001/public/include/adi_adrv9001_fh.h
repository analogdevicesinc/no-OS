/**
 * \file
 * \brief Contains ADRV9001 Frequency Hopping function prototypes
 *
 * ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
 */

 /**
 * Copyright 2020 Analog Devices Inc.
 * Released under the ADRV9001 API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#ifndef _ADI_ADRV9001_FH_H_
#define _ADI_ADRV9001_FH_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "adi_adrv9001_fh_types.h"

/**
 * \brief Configure Frequency Hopping settings
 * 
 * \note Hopping is enabled by the fhModeOn parameter in the device profile.
 *       Message type: \ref timing_prioritymailbox "Mailbox command"
 * 
 * \pre Channel state is STANDBY
 * 
 * \param[in] adrv9001  Context variable - Pointer to the ADRV9001 device data structure
 * \param[in] fhConfig  Frequency hopping configuration settings to be applied.
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover   
 */
int32_t adi_adrv9001_fh_Configure(adi_adrv9001_Device_t *adrv9001, adi_adrv9001_FhCfg_t  *fhConfig);

/**
 * \brief Read the parameters in the frequency hopping configuration data struct
 * 
 * \note Message type: \ref timing_prioritymailbox "Mailbox command"
 * 
 * \pre This function can be called by the user anytime after initialization.
 *
 * \param[in]  adrv9001  Context variable - Pointer to the ADRV9001 device data structure
 * \param[out] fhConfig  Frequency hopping configuration settings that are returned.
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_fh_Configuration_Inspect(adi_adrv9001_Device_t *adrv9001, adi_adrv9001_FhCfg_t *fhConfig);

/**
 * \brief Load a frequency hopping table into ARM memory
 * 
 * A frequency hopping table is made up by an array of hop frame information
 * defined by adi_adrv9001_FhHopFrame_t
 * 
 * \note Message type: \ref timing_prioritymailbox "High-priority mailbox command"
 *       Maximum table size is 64
 *
 * \pre This function can be called by the user anytime after initialization.
 *      mode should be set to the same as that set in fhConfig when calling 
 *      adi_adrv9001_fh_Configure
 * 
 * \param[in] adrv9001      Context variable - Pointer to the ADRV9001 device data structure
 * \param[in] mode          Frequency hopping mode
 * \param[in] hopSignal     Hop signal to configure appropriate tableId
 * \param[in] tableId       FH_HOP_TABLE_A or FH_HOP_TABLE_B. Used for ping-pong hop tables.
 * \param[in] hopTable      Array of hop frame information to write as the frequency hopping table
 * \param[in] hopTableSize  Size of hopTable; Number of hop frames to write
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
/* This writes the buffer directly (it's too big for the mailbox), then uses mailbox to message ARM */
int32_t adi_adrv9001_fh_HopTable_Static_Configure(adi_adrv9001_Device_t *adrv9001,
                                                  adi_adrv9001_FhMode_e mode,
                                                  adi_adrv9001_FhHopSignal_e hopSignal,
                                                  adi_adrv9001_FhHopTable_e tableId, 
                                                  adi_adrv9001_FhHopFrame_t hopTable[],
                                                  uint32_t hopTableSize);

/**
 * \brief Inspect frequency hopping table in ARM memory
 *
 * This function reads back a frequency hopping table from ARM memory and loads it into hopTable. 
 * The frequency hopping table to read back is specified by a tableId
 * 
 * \note Message type: \ref timing_prioritymailbox "Mailbox command"
 *
 * \pre This function can be called by the user anytime after initialization.
 *
 * \param[in]  adrv9001        Context variable - Pointer to the ADRV9001 device data structure
 * \param[in]  hopSignal       Hop signal to inspect appropriate tableId
 * \param[in]  tableId         FH_HOP_TABLE_A or FH_HOP_TABLE_B. Used for ping-pong hop tables
 * \param[out] hopTable        Read back array of hopTable which will be updated with the read back values
 * \param[in]  hopTableSize    Size of hopTable; Max number of hop frames to read back
 * \param[out] numEntriesRead  Actual number of hop table entries read back from ARM. Pass null if this info is not needed

 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
/* This writes the buffer directly (it's too big for the mailbox), then uses mailbox to trigger the DMA table generation. */
int32_t adi_adrv9001_fh_HopTable_Inspect(adi_adrv9001_Device_t *adrv9001,
                                         adi_adrv9001_FhHopSignal_e hopSignal,
                                         adi_adrv9001_FhHopTable_e tableId, 
                                         adi_adrv9001_FhHopFrame_t hopTable[],
                                         uint32_t hopTableSize,
                                         uint32_t *numEntriesRead);

/**
 * \brief Set which hop table to use 
 * 
 * Invoke the selection of FH_HOP_TABLE_A or FH_HOP_TABLE_B. 
 * 
 * \note Message type: \ref timing_prioritymailbox "High-priority mailbox command"
 *
 * \param[in] adrv9001  Context variable - Pointer to the ADRV9001 device data structure
 * \param[in] hopSignal Hop signal to set appropriate table Id
 * \param[in] tableId   FH_HOP_TABLE_A or FH_HOP_TABLE_B. Used for ping-pong hop tables.
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_fh_HopTable_Set(adi_adrv9001_Device_t *adrv9001,
                                     adi_adrv9001_FhHopSignal_e hopSignal,
                                     adi_adrv9001_FhHopTable_e tableId);

/**
 * \brief Get the hop table currently in use
 * 
 * \note Message type: \ref timing_prioritymailbox "Mailbox command"
 *
 * \param[in]  adrv9001  Context variable - Pointer to the ADRV9001 device data structure
 * \param[in]  hopSignal Hop signal to get appropriate table Id
 * \param[out] tableId   FH_HOP_TABLE_A or FH_HOP_TABLE_B.
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_fh_HopTable_Get(adi_adrv9001_Device_t *adrv9001,
                                     adi_adrv9001_FhHopSignal_e hopSignal,
                                     adi_adrv9001_FhHopTable_e *tableId);

/**
 * \brief Gets hop frame information for specified index
 *
 * Adrv9001 maintains state for three frequency hopping frames; current frame, upcoming frame (frame at next hop edge), 
 * and next frame (i.e. two hop edges in the future).
 * This command allows to fetch hop frame information, as specified by adi_adrv9001_FhHopFrame_t, from any of these states.
 * 
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \param[in]  adrv9001    Context variable - Pointer to the ADRV9001 device data structure
 * \param[in]  frameIndex  Frame index to get from, as specified by fh_adrv9001_FrameIndexSel_e
 * \param[out] hopFrame    Hop frame info for specified frame index
 * 
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_fh_FrameInfo_Inspect(adi_adrv9001_Device_t *adrv9001, 
                                          adi_adrv9001_FhFrameIndex_e frameIndex,
                                          adi_adrv9001_FhHopFrame_t *hopFrame);

/**
 * \brief Trigger the hop signal by either SPI or mailbox
 *     
 * Thia API will have no effect if it is called before any channel,
 * enabled for frequency hopping, is in PRIMED state.
 * 
 * \pre Channel state is PRIMED RF_ENABLED
 * 
 * \note Message type: \ref timing_direct "Direct register access"
 *       
 * \param[in] adrv9001  Context variable - Pointer to the ADRV9001 device data structure
 * \param[in] hopSignal Hop signal to toggle
 * 
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_fh_Hop(adi_adrv9001_Device_t *adrv9001, adi_adrv9001_FhHopSignal_e hopSignal);

/**
 * \brief Load a frequency hopping table into ARM memory dynamically
 * 
 * \pre This function can be called by the user anytime after initialization.
 * 
 * \param[in]  adrv9001                   Context variable - Pointer to the adrv9001 device data structure
 * \param[in]  mode                       Frequency hopping mode
 * \param[in]  hopSignal                  Hop signal to inspect appropriate tableId
 * \param[in]  hopTable                   Array of hop frame information to write as the frequency hopping table
 * \param[in]  hopTableSize               Size of hopTable; Number of hop frames to write for Tables A and B. 
 *                                        hopTableSize must be 2 * numberHopsPerDynamicLoad; 
 *                                        For example, if numberHopsPerDynamicLoad = 4, then hopTableSize = 8
 * \param[in]  numberHopsPerDynamicLoad   Number of Hops to be loaded in real time
 * \param[out] spiPackedFhTable           Output buffer contains SPI formatted FH table
 * \param[in]  length                     The length of the spiPackedFhTable array

 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_fh_HopTable_Dynamic_Configure(adi_adrv9001_Device_t *adrv9001,
                                                   adi_adrv9001_FhMode_e mode,
                                                   adi_adrv9001_FhHopSignal_e hopSignal,
                                                   adi_adrv9001_FhHopFrame_t hopTable[],
                                                   uint32_t hopTableSize,
                                                   adi_adrv9001_FhPerDynamicLoad_e numberHopsPerDynamicLoad,
                                                   uint8_t spiPackedFhTable[],
                                                   uint32_t length);

/**
 * \brief Get number of SPI packed bytes per dynamic FH table load
 * 
 * \pre This function can be called by the user anytime after initialization.
 * 
 * \param[in]  adrv9001                   Context variable - Pointer to the adrv9001 device data structure
 * \param[in]  numberHopsPerDynamicLoad   Number of Hops to be loaded in real time
 * \param[out] bytesPerTable              The number of SPI packed bytes per dynamic FH table load
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_fh_HopTable_BytesPerTable_Get(adi_adrv9001_Device_t *adrv9001,
                                                   adi_adrv9001_FhPerDynamicLoad_e numberHopsPerDynamicLoad,
                                                   uint32_t *bytesPerTable);

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV9001_FH_H_ */
