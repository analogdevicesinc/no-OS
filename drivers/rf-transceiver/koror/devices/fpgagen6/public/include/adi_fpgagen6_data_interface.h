/**
 * Copyright 2015 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.pdf" file in this zip file.
 */

/*!
* \file adi_fpgagen6_data_interface.h
*
* \brief
*
* FPGA API Version: 2.9.0.4
*/

#ifndef _ADI_FPGAGEN6_DATA_INTERFACE_H_
#define _ADI_FPGAGEN6_DATA_INTERFACE_H_

#include "adi_fpgagen6_data_interface_types.h"
#include "adi_fpgagen6_error.h"

/**
* \brief    This API function is used to configure a single PHY Serializer channel.
*           This function is used typically during an initialization sequence. In order
*           to determine what Serializer channels to initialize comes from the init
*           settings and is determine based on the use case.
* 
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to FPGAGEN6 device data structure
* \param[in] chSel an index to select the Serializer channel
* \param[in] serializerCfg  contains the serializer configuration setting
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_SerializerCfgSet( adi_fpgagen6_Device_t* const device,
                                                                const uint8_t chSel,
                                                                const adi_fpgagen6_SerializerCfg_t* const serializerCfg);

/**
* \brief    Reads the current configuration for the selected Serializer channel and returns
*           it in the serializerCfg structure.
*           
*           The Serializer channel selects are zero-index based, ie chSel = 0 will return
*           the configuration for Serializer channel 0.
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to FPGAGEN6 device data structure
* \param[in] chSel an index to select the Serializer channel
* \param[in,out] serializerCfg empty structure to hold the current configuration
* 
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_SerializerCfgGet(  adi_fpgagen6_Device_t* const device,
                                                                const uint8_t chSel,
                                                                adi_fpgagen6_SerializerCfg_t* const serializerCfg);

/**
* \brief    This API function will set the desired Serializer channel's reset bit to the
*           value passed in through resetVal. The bit to channel mapping is 1:1, with
*           chSelMask[0] assigned to Serializer channel 0. If a channel's bit is
*           zero its reset bit will remain untouched.
*           
*           This function is typically used during initialization when configuring the
*           Serializer. Setting a channel's reset will kick off the channel reset sequence. After the
*           channel has completed the reset sequence the reset bit will self-clear. This can be
*           verified through adi_fpgagen6_SerializerResetDoneGet.
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to FPGAGEN6 device data structure
* \param[in] chSelMask bitmask representing which channel reset bits to set, 0 = do nothin, 1 = set reset to resetVal
* \param[in] resetVal either 0 or 1. Value to be written to the reset bit (1 to reset; 0 to release reset)
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_SerializerResetSet(   adi_fpgagen6_Device_t* const device,
                                                                    const uint8_t chSelMask,
                                                                    const uint8_t resetVal);

/**
* \brief Returns the tx reset done for all eight Serializer channels in a bit mask fashion,
*        ie bit 0 = Tx channel 0.
*        
*        This function is used as part of PHY initialization after the required channels
*        have been configured and reset.
*        
*        Note: channels must have stable clock input for Reset Done to be set
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to FPGAGEN6 device data structure
* \param[out] txResetDone pointer that will be assigned each lanes tx reset done bit
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_SerializerResetDoneGet(adi_fpgagen6_Device_t* const device,
                                                                     uint8_t* const txResetDone);

/**
* \brief    This API function is used to configure a single PHY Deserializer channel.
*           This function is used typically during an initialization sequence. 
*           
*           In order to determine what Deserializer channels to initialize comes from 
*           the init settings and is determine based on the use case.
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to FPGAGEN6 device data structure
* \param[in] chSel an index to select the Deserializer channel
* \param[in] deserializerCfg  contains the deserializer configuration setting
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_DeserializerCfgSet(   adi_fpgagen6_Device_t* const device,
                                                                    const uint8_t chSel,
                                                                    const adi_fpgagen6_DeserializerCfg_t* const deserializerCfg);

/**
* \brief    Reads the current configuration for the selected Deserializer channel and returns
*           it in the deserializerCfg structure.
*           
*           The Deserializer channel selects are zero-index based, ie chSel = 0 will return
*           the configuration for Deserializer channel 0.
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to FPGAGEN6 device data structure
* \param[in] chSel an index to select the Deserializer channel
* \param[in,out] deserializerCfg empty structure to hold the current configuration
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_DeserializerCfgGet(   adi_fpgagen6_Device_t* const device,
                                                                    const uint8_t chSel,
                                                                    adi_fpgagen6_DeserializerCfg_t* const deserializerCfg);

/**
* \brief    This API function will set the desired Deserializer channel's reset bit to the
*           value passed in through resetVal. The bit to channel mapping is 1:1, with
*           chSelMask[0] assigned to Deserializer channel 0. If a channel's bit is
*           zero its reset bit will remain untouched.
*           
*           This function is typically used during initialization when configuring the
*           Deserializer. Setting a channel's reset will kick off the channel reset sequence. After the
*           channel has completed the reset sequence the reset bit will self-clear. This can be
*           verified through adi_fpgagen6_DeserializerResetDoneGet.
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to FPGAGEN6 device data structure
* \param[in] chSelMask bitmask representing which channel reset bits to set, 0 = do nothing, 1 = set reset to resetVal
* \param[in] resetVal either 0 or 1. Value to be written to the reset bit (1 to reset; 0 to release reset)
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_DeserializerResetSet( adi_fpgagen6_Device_t* const device,
                                                                    const uint8_t chSelMask,
                                                                    const uint8_t resetVal);

/**
* \brief Returns the reset done for all eight Deserializer channels in a bit mask fashion,
*        ie bit 0 = Deserializer channel 0.
*        
*        This function is used as part of PHY initialization after the required channels
*        have been configured and reset.
*        
*        Note: channels must have stable clock input for Reset Done to be set
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to FPGAGEN6 device data structure
* \param[out] rxResetDone pointer that will be assigned each lanes rx reset done bit
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_DeserializerResetDoneGet( adi_fpgagen6_Device_t* const device,
                                                                        uint8_t* const rxResetDone);

/**
* \brief Service to Reset LaneXBars
*
*       Function is passed a data structure containing bit masks for deframer/framer selection 
*       and the associated lanes for reset.
*
* \dep_begin
* \dep{device}
* \dep{device->devStateInfo.interfaceType}
* \dep_end
*
* \param[in,out] device         Context variable - Pointer to FPGAGEN6 device data structure
* \param[in] laneResetMaskInfo  Masks for Resetting Lanes
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_LaneXBarResetSet( adi_fpgagen6_Device_t* const                    device,
                                                                const adi_fpgagen6_LaneXBarResetMask_t* const   laneResetMaskInfo);

/**
* \brief    Service to put all LaneXbar modules into reset for all 204B & 204C link layers.
*           This API function should be used as part of the FPGA init sequence to help put all the FPGA
*           modules into a known state before configuration.
*
* \dep_begin
* \dep{device}
* \dep{device->devStateInfo.interfaceType}
* \dep_end
*
* \param[in,out] device         Context variable - Pointer to FPGAGEN6 device data structure\
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_LaneXBarFullResetSet( adi_fpgagen6_Device_t* const device);

/**
* \brief Service to Get the Reset LaneXBars Information
*
*       Function is passed a data structure containing bit masks for deframer/framer selection 
*       and the associated lanes for reset.
*
* \dep_begin
* \dep{device}
* \dep{device->devStateInfo.interfaceType}
* \dep_end
*
* \param[in,out] device         Context variable - Pointer to FPGAGEN6 device data structure
* \param[in,out] laneResetMaskInfo Masks for Resetting Lanes, depending on Selects Provided
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_LaneXBarResetGet( adi_fpgagen6_Device_t* const            device,
                                                                adi_fpgagen6_LaneXBarResetMask_t* const laneResetMaskInfo);

/**
* \brief    Get a Deframer Configuration
*
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device         Context variable - Pointer to FPGAGEN6 device data structure
* \param[in] deframerSel        Deframer Select
* \param[in,out] deframerCfgInfo   Current Deframer Configuration
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_DeframerCfgGet(   adi_fpgagen6_Device_t* const        device,
                                                                const adi_fpgagen6_DeframerSel_e    deframerSel,
                                                                adi_fpgagen6_DeframerCfg_t* const   deframerCfgInfo);

/**
* \brief    Set a Deframer Configuration
*
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device         Context variable - Pointer to FPGAGEN6 device data structure
* \param[in] deframerSel        Deframer Select
* \param[in] deframerCfgInfo    Deframer Configuration to be Written
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_DeframerCfgSet(   adi_fpgagen6_Device_t* const            device,
                                                                const adi_fpgagen6_DeframerSel_e        deframerSel,
                                                                const adi_fpgagen6_DeframerCfg_t* const deframerCfgInfo);

/**
* \brief    Get a Framer Configuration
*
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device     Context variable - Pointer to FPGAGEN6 device data structure
* \param[in] framerSel      Framer Select
* \param[in,out] framerCfgInfo Current Deframer Configuration
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_FramerCfgGet( adi_fpgagen6_Device_t* const    device,
                                                            const adi_fpgagen6_FramerSel_e  framerSel,
                                                            adi_fpgagen6_FramerCfg_t* const framerCfgInfo);

/**
* \brief    Set a Framer Configuration
*
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device     Context variable - Pointer to FPGAGEN6 device data structure
* \param[in] framerSel      Framer Select
* \param[in] framerCfgInfo  Framer Configuration to be Written
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_FramerCfgSet( adi_fpgagen6_Device_t* const            device,
                                                            const adi_fpgagen6_FramerSel_e          framerSel,
                                                            const adi_fpgagen6_FramerCfg_t* const   framerCfgInfo);

/**
* \brief    Service to Set Reset for a Deframer
*
*           NOTE: Deframer will be held in Reset by this API
*                   DeframerResetDonePoll() is required to release & check reset is done
*                       Reset will not complete if physical layer isnt up
*
*           Typically During Reset, Link Bringup, LaneXbar Reset & Deframer Configuration etc is performed
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to FPGAGEN6 device data structure
* \param[in] deframerSel  Deframer Select
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_DeframerResetSet( adi_fpgagen6_Device_t* const        device,
                                                                const adi_fpgagen6_DeframerSel_e    deframerSel);

/**
* \brief    Service to Release Deframer from Reset and Poll for the Reset Process to Complete
*
*           NOTE: Required to be called after a given DeframerResetSet() for the same Deframer
*                   Releases Reset Pin & Polls for expected response
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to FPGAGEN6 device data structure
* \param[in] deframerSel  Deframer Select
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_DeframerResetDonePoll(adi_fpgagen6_Device_t* const        device,
                                                                    const adi_fpgagen6_DeframerSel_e    deframerSel);

/**
* \brief    Service to Set Reset for a Framer
*
*           NOTE: Framer will be held in Reset by this API
*                   FramerResetDonePoll() is required to release & check reset is done
*                       Reset will not complete if physical layer isnt up
*
*           Typically During Reset, Link Bringup, LaneXbar Reset & Framer Configuration etc is performed
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to FPGAGEN6 device data structure
* \param[in] framerSel  Framer Select
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_FramerResetSet(   adi_fpgagen6_Device_t* const    device,
                                                                const adi_fpgagen6_FramerSel_e  framerSel);

/**
* \brief    Service to Release Framer from Reset and Poll for the Reset Process to Complete
*
*           NOTE: Required to be called after a given DeframerResetSet() for the same Deframer
*                   Releases Reset Pin & Polls for expected response
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to FPGAGEN6 device data structure
* \param[in] framerSel  Framer Select
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_FramerResetDonePoll(  adi_fpgagen6_Device_t* const    device,
                                                                    const adi_fpgagen6_FramerSel_e  framerSel);

/**
* \brief    Service to Configure Reference Clock & SysREF Control Parameters
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to FPGAGEN6 device data structure
* \param[in] sysRefCtrlCfg  SysREF Control Configuration 
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_SysRefControlSet( adi_fpgagen6_Device_t* const                device,
                                                                const adi_fpgagen6_SysRefClkCfg_t* const    sysRefCtrlCfg);

/**
* \brief    Service to Get Reference Clock & SysREF Control Configuration
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to FPGAGEN6 device data structure
* \param[in,out] sysRefCtrlCfg  SysREF Control Configuration 
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_SysRefControlGet( adi_fpgagen6_Device_t* const        device,
                                                                adi_fpgagen6_SysRefClkCfg_t* const  sysRefCtrlCfg);

/**
* \brief    This function is used to control which Framer or Deframers react to Sysref signals. 
* 
*           Typically, this function would be used during link bringup Sysref signals are required
*           for other components but should be ignored by the FPGA link layer modules.
*           
*           The value passed into the enable parameter will be applied to the selected Link Layer modules.
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to FPGAGEN6 device data structure
* \param[in]    framerSelMask a bitmask to select the framers, bit 0 = Framer 0, etc.
* \param[in]    deframerSelMask a bitmask to select the deframers, bit 0 = Deframer 0, etc.
* \param[in]    enable ADI_ENABLE or ADI_DISABLE to enable or disable Sysref to the selected Link Layer modules
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_SysRefEnableSet(  adi_fpgagen6_Device_t* const device,
                                                                const uint8_t framerSelMask,
                                                                const uint8_t deframerSelMask,
                                                                const uint8_t enable);


/**
* \brief    This function is used to enable the internal Sysref generator. The internal sysref generator
*           must be enabled prior to calling adi_fpgagen6_InternalSysrefRequest for the sysref signal
*           to be valid.
* 
*           This is used mainly for debug purposes when an external clock chip is not present to
*           provide a sysref signal to the entire system.
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to FPGAGEN6 device data structure
* \param[in]    enable ADI_ENABLE or ADI_DISABLE to enable or disable internal sysref module
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_InternalSysrefEnableSet(  adi_fpgagen6_Device_t* const device,
                                                                        const uint8_t enable);


/**
* \brief    This function is used to generate a Sysref pulse using the internal generator.
* 
*           Internal sysref generator must be enabled first.
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to FPGAGEN6 device data structure
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_InternalSysrefRequest(adi_fpgagen6_Device_t* const device);

/**
* \brief    This function will return a bitmask indicating JESD status information. The meaning of the bits
*           changes for 204B and 204C (except bit 0 will always indicate JESD link condition):
*           
*           For JESD204B:
*               status[0] = rx_started_8b10b bitfield, i.e. data valid leaving the deframer
*           For JESD204C:
*               status[0] = logical and of mb_lock & sync_header_lock bits to indicate JESD status
*               status[1] = buffer_overflow bit value, 1 = buffer overflow has occured, 0 otherwise
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to FPGAGEN6 device data structure
* \param[in] deframerSel  Deframer Select
* \param[out] status 1 = links are up on this Deframer; 0 otherwise
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_LinkStatusGet(adi_fpgagen6_Device_t* const        device,
                                                            const adi_fpgagen6_DeframerSel_e    deframerSel,
                                                            uint8_t * const status);

/**
* \brief    Service to Configure the SyncXBar
*
*           Control of Multiplexers for Sync Output Signal to the Deframers & Sync Input Signal to the Framers.
*           Multiplexers can connect the Sync Signals for Link Bring Up from the FPGA to the DUT or Loopback the FPGA (Near End) 
*           or the DUT (Far End) Sync Signals
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to FPGAGEN6 device data structure
* \param[in] syncXBarCfgSet  SyncBar Configuration
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_SyncXBarCfgSet(   adi_fpgagen6_Device_t* const        device,
                                                                const adi_fpgagen6_SyncXBar_t* const syncXBarCfgSet);

/**
* \brief    Service to Get Current SyncXBar Configuration
*
*           Current Configuration of Multiplexers for Sync Output Signal to the Deframers & Sync Input Signal to the Framers.
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to FPGAGEN6 device data structure
* \param[in,out] syncXBarCfgGet  SyncBar Configuration
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_SyncXBarCfgGet(   adi_fpgagen6_Device_t* const    device,
                                                                adi_fpgagen6_SyncXBar_t* const  syncXBarCfgGet);


/**
* \brief Enables or disables the Command and Data interface of the Deframer's selected through the deframerSelMask bitmask.
* 
*        The Command and Data interface bits are only valid in 204C, however, the API will not return an error if the current
*        FPGA image is configured for 204B. Rather, it will do nothing and return success. So it is safe to call this function
*        as part of any link bringup sequence without trying to determine what JESD mode.
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out]    device Context variable - Pointer to FPGAGEN6 device data structure
* \param[in]        deframerSel  Deframer Select
* \param[in]        cmdEnable ADI_ENABLE or ADI_DISABLE to enable and disable the command interface 
* \param[in]        dataEnable ADI_ENABLE or ADI_DISABLE to enable and disable the data interface 
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_DeframerCmdDataSet(adi_fpgagen6_Device_t* const device,
                                                                const adi_fpgagen6_DeframerSel_e  deframerSel,
                                                                const uint8_t                cmdEnable,
                                                                const uint8_t                dataEnable);

/**
* \brief Enables or disables the Command and Data interface of the Framer's selected through the framerSelMask bitmask.
* 
*        The Command and Data interface bits are only valid in 204C, however, the API will not return an error if the current
*        FPGA image is configured for 204B. Rather, it will do nothing and return success. So it is safe to call this function
*        as part of any link bringup sequence without trying to determine what JESD mode.
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out]    device Context variable - Pointer to FPGAGEN6 device data structure
* \param[in]        framerSel  Deframer Select
* \param[in]        cmdEnable ADI_ENABLE or ADI_DISABLE to enable and disable the command interface 
* \param[in]        dataEnable ADI_ENABLE or ADI_DISABLE to enable and disable the data interface 
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*
*/
ADI_API adi_fpgagen6_ErrAction_e  adi_fpgagen6_FramerCmdDataSet(adi_fpgagen6_Device_t* const device,
                                                                const adi_fpgagen6_FramerSel_e  framerSel,
                                                                const uint8_t                cmdEnable,
                                                                const uint8_t                dataEnable);

#endif  /* _ADI_FPGAGEN6_DATA_INTERFACE_H_ */
