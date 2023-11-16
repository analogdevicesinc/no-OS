/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/*!
* \file fpgagen6_data_interface.h
*
* \brief
*
* FPGA API Version: 2.9.0.4
*/

#ifndef _FPGAGEN6_DATA_INTERFACE_H_
#define _FPGAGEN6_DATA_INTERFACE_H_


#include "../../private/bf/fpgagen6_bf_axi_adrv904x_rx.h"
#include "../../private/bf/fpgagen6_bf_axi_adrv904x_tx.h"
#include "../../private/bf/fpgagen6_bf_xilinx_jesd.h"
#include "fpgagen6_data_interface_types.h"
#include "adi_fpgagen6_data_interface_types.h"

#define FPGAGEN6_DEFRAMER_IDX_SET_RETURN(deframerSelect, idx, recoveryAction)       \
{                                                                                   \
    switch(deframerSelect)                                                          \
    {                                                                               \
        case ADI_FPGAGEN6_DEFRAMER_0:                                               \
            idx = ADI_FPGAGEN6_DEFRAMER0_IDX;                                       \
            break;                                                                  \
                                                                                    \
        case ADI_FPGAGEN6_DEFRAMER_1:                                               \
            idx = ADI_FPGAGEN6_DEFRAMER1_IDX;                                       \
            break;                                                                  \
                                                                                    \
        case ADI_FPGAGEN6_DEFRAMER_2:                                               \
            idx = ADI_FPGAGEN6_DEFRAMER2_IDX;                                       \
            break;                                                                  \
                                                                                    \
        case ADI_FPGAGEN6_DEFRAMER_NONE:                                            \
        case ADI_FPGAGEN6_DEFRAMER_ALL:                                             \
            /* Fall Through */                                                      \
                                                                                    \
        default:                                                                    \
            recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;                      \
            ADI_PARAM_ERROR_REPORT( &device->common,                                \
                                   recoveryAction,                                  \
                                   deframerSelect,                                  \
                                   "Invalid Deframer Selected");                    \
            return recoveryAction;                                                  \
            break;                                                                  \
    }                                                                               \
}

#define FPGAGEN6_FRAMER_IDX_SET_RETURN(framerSelect, idx, recoveryAction)           \
{                                                                                   \
    switch(framerSelect)                                                            \
    {                                                                               \
        case ADI_FPGAGEN6_FRAMER_0:                                                 \
            idx = ADI_FPGAGEN6_FRAMER0_IDX;                                         \
            break;                                                                  \
                                                                                    \
        case ADI_FPGAGEN6_FRAMER_1:                                                 \
            idx = ADI_FPGAGEN6_FRAMER1_IDX;                                         \
            break;                                                                  \
                                                                                    \
        case ADI_FPGAGEN6_FRAMER_NONE:                                              \
        case ADI_FPGAGEN6_FRAMER_ALL:                                               \
            /* Fall Through */                                                      \
                                                                                    \
        default:                                                                    \
            recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;                      \
            ADI_PARAM_ERROR_REPORT( &device->common,                                \
                                    recoveryAction,                                 \
                                    framerSelect,                                   \
                                    "Invalid Framer Selected");                     \
            return recoveryAction;                                                  \
            break;                                                                  \
    }                                                                               \
}

#define FPGAGEN6_DEFRAMER_IDX_SET_GOTO(deframerSelect, idx, recoveryAction, label)  \
{                                                                                   \
    switch(deframerSelect)                                                          \
    {                                                                               \
        case ADI_FPGAGEN6_DEFRAMER_0:                                               \
            idx = ADI_FPGAGEN6_DEFRAMER0_IDX;                                       \
            break;                                                                  \
                                                                                    \
        case ADI_FPGAGEN6_DEFRAMER_1:                                               \
            idx = ADI_FPGAGEN6_DEFRAMER1_IDX;                                       \
            break;                                                                  \
                                                                                    \
        case ADI_FPGAGEN6_DEFRAMER_2:                                               \
            idx = ADI_FPGAGEN6_DEFRAMER2_IDX;                                       \
            break;                                                                  \
                                                                                    \
        case ADI_FPGAGEN6_DEFRAMER_NONE:                                            \
        case ADI_FPGAGEN6_DEFRAMER_ALL:                                             \
            /* Fall Through */                                                      \
                                                                                    \
        default:                                                                    \
            recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;                      \
            ADI_PARAM_ERROR_REPORT( &device->common,                                \
                                    recoveryAction,                                 \
                                    deframerSelect,                                 \
                                    "Invalid Deframer Selected");                   \
            goto label;                                                             \
            break;                                                                  \
    }                                                                               \
}

#define FPGAGEN6_FRAMER_IDX_SET_GOTO(framerSelect, idx, recoveryAction, label)      \
{                                                                                   \
    switch(framerSelect)                                                            \
    {                                                                               \
        case ADI_FPGAGEN6_FRAMER_0:                                                 \
            idx = ADI_FPGAGEN6_FRAMER0_IDX;                                         \
            break;                                                                  \
                                                                                    \
        case ADI_FPGAGEN6_FRAMER_1:                                                 \
            idx = ADI_FPGAGEN6_FRAMER1_IDX;                                         \
            break;                                                                  \
                                                                                    \
        case ADI_FPGAGEN6_FRAMER_NONE:                                              \
        case ADI_FPGAGEN6_FRAMER_ALL:                                               \
            /* Fall Through */                                                      \
                                                                                    \
        default:                                                                    \
            recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;                      \
            ADI_PARAM_ERROR_REPORT( &device->common,                                \
                                   recoveryAction,                                  \
                                   framerSelect,                                    \
                                   "Invalid Framer Selected");                      \
            goto label;                                                             \
            break;                                                                  \
    }                                                                               \
}

#define FPGAGEN6_INTERFACE_IDX_SET_RETURN(idx, recoveryAction)                      \
{                                                                                   \
    switch (device->devStateInfo.interfaceType)                                     \
    {                                                                               \
        case ADI_FPGAGEN6_INTERFACE_JESD204B:                                       \
            interfaceIdx = ADI_FPGAGEN6_JESD204B_IDX;                               \
            break;                                                                  \
                                                                                    \
        case ADI_FPGAGEN6_INTERFACE_JESD204C:                                       \
            interfaceIdx = ADI_FPGAGEN6_JESD204C_IDX;                               \
            break;                                                                  \
                                                                                    \
        case ADI_FPGAGEN6_INTERFACE_NOT_SET:                                        \
        case ADI_FPGAGEN6_INTERFACE_UNKNOWN:                                        \
            /* Fall Through */                                                      \
                                                                                    \
        default:                                                                    \
            recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;                      \
            ADI_PARAM_ERROR_REPORT( &device->common,                                \
                                   recoveryAction,                                  \
                                   device->devStateInfo.interfaceType,              \
                                   "Invalid Data Interface Configured");            \
            return recoveryAction;                                                  \
            break;                                                                  \
    }                                                                               \
}

#define FPGAGEN6_INTERFACE_IDX_SET_GOTO(idx, recoveryAction, label)                 \
{                                                                                   \
    switch (device->devStateInfo.interfaceType)                                     \
    {                                                                               \
        case ADI_FPGAGEN6_INTERFACE_JESD204B:                                       \
            interfaceIdx = ADI_FPGAGEN6_JESD204B_IDX;                               \
            break;                                                                  \
                                                                                    \
        case ADI_FPGAGEN6_INTERFACE_JESD204C:                                       \
            interfaceIdx = ADI_FPGAGEN6_JESD204C_IDX;                               \
            break;                                                                  \
                                                                                    \
        case ADI_FPGAGEN6_INTERFACE_NOT_SET:                                        \
        case ADI_FPGAGEN6_INTERFACE_UNKNOWN:                                        \
            /* Fall Through */                                                      \
                                                                                    \
        default:                                                                    \
            recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;                      \
            ADI_PARAM_ERROR_REPORT( &device->common,                                \
                                   recoveryAction,                                  \
                                   device->devStateInfo.interfaceType,              \
                                   "Invalid Data Interface Configured");            \
            goto label;                                                             \
            break;                                                                  \
    }                                                                               \
}

extern const fpgagen6_BfAxiAdrvgen6RxChanAddr_e axiAdrvgen6RxChannelAddr[ADI_FPGAGEN6_MAX_DEFRAMERS][ADI_FPGAGEN6_INTERFACE_TYPES_NUM];
extern const fpgagen6_BfAxiAdrvgen6TxChanAddr_e axiAdrvgen6TxChannelAddr[ADI_FPGAGEN6_MAX_FRAMERS][ADI_FPGAGEN6_INTERFACE_TYPES_NUM];
extern const fpgagen6_BfXilinxJesdChanAddr_e xilinxJesdDeframerAddr[ADI_FPGAGEN6_MAX_DEFRAMERS][ADI_FPGAGEN6_INTERFACE_TYPES_NUM];
extern const fpgagen6_BfXilinxJesdChanAddr_e xilinxJesdFramerAddr[ADI_FPGAGEN6_MAX_FRAMERS][ADI_FPGAGEN6_INTERFACE_TYPES_NUM];

extern const SyncF2pSelectSetFnPtr_t syncF2pSelect_BfSetFnPtrs[ADI_FPGAGEN6_MAX_DEFRAMERS];
extern const SyncF2pSelectGetFnPtr_t syncF2pSelect_BfGetFnPtrs[ADI_FPGAGEN6_MAX_DEFRAMERS];
extern const SyncF2pPolaritySetFnPtr_t syncF2pPolarity_BfSetFnPtrs[ADI_FPGAGEN6_MAX_DEFRAMERS];
extern const SyncF2pPolarityGetFnPtr_t syncF2pPolarity_BfGetFnPtrs[ADI_FPGAGEN6_MAX_DEFRAMERS];
extern const SyncP2fPolaritySetFnPtr_t syncP2fPolarity_BfSetFnPtrs[ADI_FPGAGEN6_MAX_FRAMERS];
extern const SyncP2fPolarityGetFnPtr_t syncP2fPolarity_BfGetFnPtrs[ADI_FPGAGEN6_MAX_FRAMERS];

/**
* \brief    Get the current Lane Assignment Configuration for the Deframer LaneXBar
*
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device     Context variable - Pointer to FPGAGEN6 device data structure
* \param[in] deframerSel    Deframer Select
* \param[out] laneXBar      Current Lane Assignment Configuration
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_RxLaneXBarCfgGet( adi_fpgagen6_Device_t* const        device,
                                                            const adi_fpgagen6_DeframerSel_e    deframerSel,
                                                            uint8_t                             laneXBar[]);

/**
* \brief    Set the Lane Assignment Configuration for the Deframer LaneXBar
*
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device     Context variable - Pointer to FPGAGEN6 device data structure
* \param[in] deframerSel    Deframer Select
* \param[in] laneXBar       Lane Assignment Configuration to be Written
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_RxLaneXBarCfgSet( adi_fpgagen6_Device_t* const        device,
                                                            const adi_fpgagen6_DeframerSel_e    deframerSel,
                                                            const uint8_t                       laneXBar[]);

/**
* \brief    Get the Lane Assignment Configuration for the Framer LaneXBar
*
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to FPGAGEN6 device data structure
* \param[in] framerSel  Framer Select
* \param[out] laneXBar  Current Lane Assignment Configuration
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_TxLaneXBarCfgGet( adi_fpgagen6_Device_t* const    device,
                                                            const adi_fpgagen6_FramerSel_e  framerSel,
                                                            uint8_t                             laneXBar[]);

/**
* \brief    Set the Lane Assignment Configuration for the Framer LaneXBar
*
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to FPGAGEN6 device data structure
* \param[in] framerSel  Framer Select
* \param[in] laneXBar   Lane Assignment Configuration to be Written
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_TxLaneXBarCfgSet( adi_fpgagen6_Device_t* const            device,
                                                            const adi_fpgagen6_FramerSel_e          framerSel,
                                                            const uint8_t                       laneXBar[]);

/**
* \brief Gets the current Deframer Configuration Data for a given Deframer Select
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device         Context variable - Pointer to FPGAGEN6 device data structure
* \param[in] deframerSel        Deframer Select
* \param[out] deframerCfgInfo   Deframer Configuration Structure
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_DeframerCfgGet(   adi_fpgagen6_Device_t* const        device,
                                                            const adi_fpgagen6_DeframerSel_e    deframerSel,
                                                            adi_fpgagen6_DeframerCfg_t* const   deframerCfgInfo);

/**
* \brief Writes a given Deframer Configuration to a given Deframer Select
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device         Context variable - Pointer to FPGAGEN6 device data structure
* \param[in] deframerSel        Deframer Select
* \param[in] deframerCfgInfo   Deframer Configuration Structure
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_DeframerCfgSet(   adi_fpgagen6_Device_t* const            device,
                                                            const adi_fpgagen6_DeframerSel_e        deframerSel,
                                                            const adi_fpgagen6_DeframerCfg_t* const deframerCfgInfo);

/**
* \brief Gets the current Framer Configuration Data for a given Framer Select
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device     Context variable - Pointer to FPGAGEN6 device data structure
* \param[in] framerSel      Framer Select
* \param[out] framerCfgInfo Framer Configuration Structure
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_FramerCfgGet( adi_fpgagen6_Device_t* const    device,
                                                        const adi_fpgagen6_FramerSel_e  framerSel,
                                                        adi_fpgagen6_FramerCfg_t* const framerCfgInfo);

/**
* \brief Writes a given Framer Configuration to a given Framer Select
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device     Context variable - Pointer to FPGAGEN6 device data structure
* \param[in] framerSel      Framer Select
* \param[in] framerCfgInfo  Framer Configuration Structure
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_FramerCfgSet( adi_fpgagen6_Device_t* const            device,
                                                        const adi_fpgagen6_FramerSel_e          framerSel,
                                                        const adi_fpgagen6_FramerCfg_t* const   framerCfgInfo);


/**
* \brief Private Helper Function for L Calculation
*
* \param[in] lanesEnabled   Lanes Enabled
*
* \retval uint8_t - Lanes Enabled Count
*/
ADI_API uint8_t fpgagen6_LCalculate(const uint8_t lanesEnabled);


#endif  /* _FPGAGEN6_DATA_INTERFACE_H_ */
