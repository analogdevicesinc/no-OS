 /**
 * Copyright 2015 - 2023 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.pdf" file in this zip file.
 */

/**
 * \file adi_adrv904x_datainterface.h
 * \brief Contains ADRV904X API public function prototypes related to the data interface
 *
 * ADRV904X API Version: 2.9.0.4
 */

#ifndef _ADI_ADRV904X_DATAINTERFACE_H_
#define _ADI_ADRV904X_DATAINTERFACE_H_

#include "adi_adrv904x_datainterface_types.h"
#include "adi_adrv904x_error.h"
#include "adi_adrv904x_cpu_cmd_run_serdes_eye_sweep.h"


/**
 * \brief Initiates data capture using the Rx/ORx data capture RAM
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable -Pointer to the device settings structure
 * \param[in] channelSelect Channel mask indicating which channel is being captured
 * \param[in] captureLocation The location at which the data capture will occur
 * \param[in,out] captureData An array that stores the captured data
 * \param[in] captureLength The size of the data to capture; i.e. the size of captureData
 * \param[in] trigger Immediate Capture (0); Triggered capture (1)
 * \param[in] timeout_us Timeout value in microseconds. Maximum time to capture data.
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_COMMON_ACT_NO_ACTION if Successful
 */

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxOrxDataCaptureStart( adi_adrv904x_Device_t* const                  device,
                                                                     const adi_adrv904x_RxChannels_e               channelSelect,
                                                                     const adi_adrv904x_RxOrxDataCaptureLocation_e captureLocation,
                                                                     uint32_t                                      captureData[],
                                                                     const uint32_t                                captureLength,
                                                                     const uint8_t                                 trigger,
                                                                     const uint32_t                                timeout_us);

/****************************************************************************
 * Initialization functions
 ****************************************************************************
 */
/**
 * \brief  Sets the ADC sample crossbar for the specified ADRV904X framer
 *
 *  For ADRV904X:
 *    - It only supports framer-0 and framer-1. For framer-2 must use adi_adrv904x_OrxAdcSampleXbarSet
 *    - framer-0 has 64 entries while framer-1 only has 32 entries.
 *
 * \pre This function is called after JESD204B/C initialization
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable -Pointer to the device settings structure
 * \param[in] framerSel selected of framer defined in adi_adrv904x_FramerSel_e
 * \param[in] adcXbar Pointer to the JESD structure adi_adrv904x_AdcSampleXbarCfg_t
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_AdcSampleXbarSet(adi_adrv904x_Device_t* const device,
                                                               const adi_adrv904x_FramerSel_e  framerSel,
                                                               const adi_adrv904x_AdcSampleXbarCfg_t* const adcXbar);
/**
 * \brief Gets the ADC sample crossbar converter configuration map for
 * the chosen JESD204B/C framer converter
 *  For ADRV904X:
 *    - It only supports framer-0 and framer-1. For framer-2 must use adi_adrv904x_OrxAdcSampleXbarGet
 *    - framer-0 has 64 entries while framer-1 only has 32 entries.
 *
 * \pre This function is called after JESD204B/C initialization
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable -Pointer to the device settings structure
 * \param[in] framerSel selected of framer defined in adi_adrv904x_FramerSel_e
 * \param[out] adcXbar Pointer to the JESD structure adi_adrv904x_AdcSampleXbarCfg_t
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_AdcSampleXbarGet(adi_adrv904x_Device_t* const device,
                                                               const adi_adrv904x_FramerSel_e  framerSel,
                                                               adi_adrv904x_AdcSampleXbarCfg_t* const adcXbar);

/**
 * \brief Sets the DAC sample crossbar for the specified ADRV904X deframer
 *
 * \pre This function is called after JESD204B/C initialization
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable -Pointer to the device settings structure
 * \param[in] deframerSel selected of deframer defined in adi_adrv904x_DeframerSel_e
 * \param[in] dacXbar Pointer to the JESD structure adi_adrv904x_DacSampleXbarCfg_t
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DacSampleXbarSet(adi_adrv904x_Device_t* const device,
                                                               const adi_adrv904x_DeframerSel_e  deframerSel,
                                                               const adi_adrv904x_DacSampleXbarCfg_t* const dacXbar);
/**
 * \brief Gets the DAC sample crossbar for the specified ADRV904X deframer
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable -Pointer to the device settings structure
 * \param[in] deframerSel selected of framer defined in adi_adrv904x_DeframerSel_e
 * \param[out] dacXbar Pointer to the JESD structure adi_adrv904x_DacSampleXbarCfg_t
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DacSampleXbarGet(adi_adrv904x_Device_t* const           device,
                                                               const adi_adrv904x_DeframerSel_e       deframerSel,
                                                               adi_adrv904x_DacSampleXbarCfg_t* const dacXbar);

/**
* \brief Gets the JESD204B/C Framer's configuration
*
* This function reads the JESD204B/C framer settings.
*
* \pre This function may be called any time after device initialization
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the device settings structure
* \param[in] framerSel selected of framer defined in adi_adrv904x_FramerSel_e
* \param[out] framerCfg Pointer to the JESD Framer configuration read back
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_FramerCfgGet(adi_adrv904x_Device_t*    const  device,
                                                           const adi_adrv904x_FramerSel_e   framerSel,
                                                           adi_adrv904x_FramerCfg_t* const  framerCfg);


/**
* \brief Gets the JESD204B/C Framer's configuration
*
* This function reads the JESD204B/C framer settings.
*
* \pre This function may be called any time after device initialization
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the device settings structure
* \param[in] framerSel selected of framer defined in adi_adrv904x_FramerSel_e
* \param[in] chanSel selected Rx channel defined in adi_adrv904x_RxChannels_e
* \param[out] framerCfg Pointer to the JESD Framer configuration read back
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_FramerCfgGet_v2(adi_adrv904x_Device_t*    const  device,
                                                              const adi_adrv904x_FramerSel_e   framerSel,
                                                              const adi_adrv904x_RxChannels_e  chanSel,
                                                              adi_adrv904x_FramerCfg_t* const  framerCfg);

/**
* \brief Gets the JESD204B/C Deframer's configuration
*
* This function reads the JESD204B/C deframer settings.
*
* \pre This function may be called any time after device initialization
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the device settings structure
* \param[in] deframerSel selected of framer defined in adi_adrv904x_DeframerSel_e
* \param[out] deframerCfg Pointer to the JESD Deframer configuration read back
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DeframerCfgGet(adi_adrv904x_Device_t* const        device,
                                                             adi_adrv904x_DeframerSel_e          deframerSel,
                                                             adi_adrv904x_DeframerCfg_t* const   deframerCfg);

/**
* \brief Gets the JESD204B/C Deframer's configuration
*
* This function reads the JESD204B/C deframer settings.
*
* \pre This function may be called any time after device initialization
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the device settings structure
* \param[in] deframerSel selected of framer defined in adi_adrv904x_DeframerSel_e
* \param[in] chanSel selected Tx channel defined in adi_adrv904x_TxChannels_e
* \param[out] deframerCfg Pointer to the JESD Deframer configuration read back
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DeframerCfgGet_v2(adi_adrv904x_Device_t* const        device,
                                                                const adi_adrv904x_DeframerSel_e    deframerSel,
                                                                const adi_adrv904x_TxChannels_e     chanSel,
                                                                adi_adrv904x_DeframerCfg_t* const   deframerCfg);
/**
 * \brief Gets the link states for all the framers.
 *
 * \pre This function is called after JESD204B/C initialization
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable -Pointer to the device settings structure
 * \param[out] framerLinkState Pointer to the framer link states
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_FramerLinkStateGet(adi_adrv904x_Device_t*  const device,
                                                                 uint8_t* const   framerLinkState);

/**
 * \brief Sets the framer link states.
 *
 * \pre This function is called after JESD204B/C initialization
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable -Pointer to the device settings structure
 * \param [in] framerSelMask indicate which framers are selected.
 * \param[in]  enable indicate to enable(1) or disable(0) the selected framers in framerSelMask
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_FramerLinkStateSet(adi_adrv904x_Device_t*  device,
                                                                 const uint8_t  framerSelMask,
                                                                 uint8_t const  enable);

/**
 * \brief Reset Deframer PRBS Count error.
 *
 * \pre This function is called after JESD204B/C initialization
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable -Pointer to the device settings structure
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfrmPrbsCountReset(adi_adrv904x_Device_t*  const device);

/**
 * \brief Gets the link states for all the deframers.
 *
 * \pre This function is called after JESD204B/C initialization
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable -Pointer to the device settings structure
 * \param[out] deframerLinkState Pointer to the deframer link states
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DeframerLinkStateGet(adi_adrv904x_Device_t*  device,
                                                                   uint8_t* const   deframerLinkState);

/**
 * \brief Sets the deframer link states.
 *
 * \pre This function is called after JESD204B/C initialization
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable -Pointer to the device settings structure
 * \param [in] deframerSelMask indicate which deframers are selected.
 * \param[in]  enable indicate to enable(1) or disable(0) the selected framers in framerSelMask
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DeframerLinkStateSet(adi_adrv904x_Device_t*  device,
                                                                   const uint8_t  deframerSelMask,
                                                                   uint8_t const  enable);

/**
 * \brief Configures and enables or disables the transceiver's lane/sample PRBS
 *        checker.
 * This is a debug function to be used for debug of the Tx JESD204B/C lanes.
 * The Tx link(s) need to be configured and on to use this function. If the
 * checkerLocation is ADI_ADRV904X_PRBSCHECK_LANEDATA, the PRBS is checked at the
 * output of the deserializer. If the checkLocation is ADI_ADRV904X_PRBSCHECK_SAMPLEDATA
 * the PRBS data is expected to be framed JESD204B/C data and the PRBS is checked
 * after the JESD204B/C data is deframed.  For the sample data, there is only
 * a PRBS checker on deframer output 0.  The lane PRBS has a checker on each
 * deserializer lane.
 *
 * \pre This function is called after JESD204B/C initialization
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable -Pointer to the device settings structure
 * \param [in] dfrmPrbsCfg Pointer to adi_adrv904x_DfrmPrbsCfg_t data structure.
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfrmPrbsCheckerStateSet(adi_adrv904x_Device_t*  const device,
                                                                      const adi_adrv904x_DfrmPrbsCfg_t * const dfrmPrbsCfg);

/**
 * \brief Read back transceiver's lane/sample PRBS checker configuration
 *
 * \pre This function is called after JESD204B/C initialization
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param [in,out] device Context variable -Pointer to the device settings structure
 * \param [in,out] dfrmPrbsCfg Pointer to adi_adrv904x_DfrmPrbsCfg_t data structure.
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfrmPrbsCheckerStateGet(adi_adrv904x_Device_t*  const device,
                                                                      adi_adrv904x_DfrmPrbsCfg_t * const dfrmPrbsCfg);

/**
 * \brief Enables or disables the external SYSREF JESD204B/C signal to the transceiver's framers
 *
 * For the framer to retime its LMFC (local multi frame clock), a SYSREF rising edge is required.
 * The external SYSREF signal at the pin can be gated off internally so the framer does not see
 * a potential invalid SYSREF pulse before it is configured correctly.
 *
 * By default the SYSREF signal ungated, however, the Multichip Sync state machine
 * still does not allow the external SYSREF to reach the framer until the other stages of multichip
 * sync have completed.  As long as the external SYSREF is correctly configured before performing MCS,
 * this function may not be needed by the BBIC, since the MCS state machine gates the SYSREF to the
 * framer.
 *
 * \pre This function is called after the device has been initialized and the JESD204B/C framer is enabled
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in] device Context variable -Pointer to the device settings structure
 * \param [in] framerSelMask indicate which framers are selected.
 * \param [in] enable indicate to enable(1) or disable(0) Sysref for the selected framers in framerSelMask
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_FramerSysrefCtrlSet(adi_adrv904x_Device_t*  const device,
                                                                  const uint8_t  framerSelMask,
                                                                  uint8_t const  enable);

/**
 * \brief Reads the bit indicating external SYSREF JESD204b signal of the transceiver's framers. Only status for
 * \ one channel at a time can be read.
 *
 * \pre This function is called after the device has been initialized and the JESD204B/C framer is enabled
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in] device Context variable -Pointer to the device settings structure
 * \param [in] framerSel indicate which framers are selected.
 * \param [out] enable Pointer to contain value to indicate enable(1) or disable(0) the selected framer in framerSelMask
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_FramerSysrefCtrlGet(adi_adrv904x_Device_t*  const device,
                                                                  const adi_adrv904x_FramerSel_e  framerSel,
                                                                  uint8_t * const enable);

/**
 * \brief Enables or disables the external SYSREF JESD204B/C signal to the transceiver's deframers
 *
 * \pre This function is called after the device has been initialized and the JESD204B/C deframer is enabled
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in] device Context variable -Pointer to the device settings structure
 * \param [in] deframerSelMask indicate which deframers are selected.
 * \param [in] enable indicate to enable(1) or disable(0) Sysref for the selected deframers in deframerSelMask
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DeframerSysrefCtrlSet(adi_adrv904x_Device_t*  const device,
                                                                    const uint8_t  deframerSelMask,
                                                                    uint8_t const  enable);

/**
 * \brief Reads the bit indicating external SYSREF JESD204B/C signal of the transceiver's deframers. Only status for one channel
 * \ at a time can be read.
 *
 *
 * \pre This function is called after the device has been initialized and the JESD204B/C deframer is enabled
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in] device Context variable -Pointer to the device settings structure
 * \param [in] deframerSel indicate which deframers are selected.
 * \param [out] enable Pointer to contain value to indicate enable(1) or disable(0) the selected deframer in deframerSelMask
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DeframerSysrefCtrlGet(adi_adrv904x_Device_t*  const device,
                                                                    const adi_adrv904x_DeframerSel_e  deframerSel,
                                                                    uint8_t * const enable);

/**
* \brief Selects the PRBS type and enables or disables RX Framer PRBS generation
 *
 * This is a debug function to be used for debug of the Rx JESD204B/C lanes.
 * Rx data transmission on the JESD204B/C link(s) is not possible
 * when the framer test data is activated.  To disable PRBS call this function
 * again with the framer data source set to ADI_ADRV904X_FTD_ADC_DATA.
 *
 * \pre This function may be called any time after device initialization
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param [in] device Context variable -Pointer to the device settings structure
 * \param [in,out] frmTestDataCfg Pointer to adi_adrv904x_FrmTestDataCfg_t for PRBS configuration setting.
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_FramerTestDataSet(adi_adrv904x_Device_t*  const device,
                                                                adi_adrv904x_FrmTestDataCfg_t * const frmTestDataCfg);

/**
 * \brief Gets  the PRBS Framer Test Mode and Inject Points
 *
 * This is a debug function to be used for debug of the Rx JESD204B/C lanes.
 * Rx data transmission on the JESD204B/C link(s) is not possible
 * when the framer test data is activated.
 *
 * \pre This function may be called any time after device initialization
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param [in] device Context variable -Pointer to the device settings structure
 * \param [in] framerSel contains the framer of interest
 * \param [in,out] frmTestDataCfg Pointer to adi_adrv904x_FrmTestDataCfg_t for PRBS configuration setting.
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_FramerTestDataGet(adi_adrv904x_Device_t*  const device,
                                                                const adi_adrv904x_FramerSel_e  framerSel,
                                                                adi_adrv904x_FrmTestDataCfg_t * const frmTestDataCfg);

/**
 * \brief Get the deserializer lane and deframer sample PRBS error counters
 *
 * In the case that the PRBS checker is set to check at the deframer output
 * sample, there is only a checker on the Deframer Sample 0 output.  In this
 * case the lane function parameter is ignored and the sample 0 PRBS counter
 * is returned.
 *
 * errorStatus:  Index 0 contains error status if in sample mode, otherwise status are maintained per index.
 *               Sample Mode: 0b: Error Count, 1b: Error Flag,  2b: Clears Error.
 *               Data Mode:  0b: Lane inverted, 1b: invalid data flag, b2: sample/lane error flag.
 * \pre If reading PRBS samples framed in the JESD204 data, the JESD204 deframer must be configured.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param [in] device Context variable -Pointer to the device settings structure
 * \param [in,out] counters Pointer to PRBS Error counter structure to be returned
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfrmPrbsErrCountGet(adi_adrv904x_Device_t*  const device,
                                                                  adi_adrv904x_DfrmPrbsErrCounters_t * const counters);

/**
 * \brief Reset the serializer
 *
 * \pre This function can be called after JESD204B/C initialization
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param [in, out] device Context variable -Pointer to the device settings structure
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_SerializerReset(adi_adrv904x_Device_t*  const device);


/**
 * \brief Reset the serializer v2
 *
 * \pre This function can be called after JESD204B/C initialization and after adi_adrv904x_SerializerReset
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param [in, out] device Context variable -Pointer to the device settings structure
 * \param[in]     pSerResetParms - Pointer to the Serializer Reset Parameter structure adrv904x_CpuCmd_SerReset_t
 * \param[out]    pSerResetResp  - Pointer to the Serializer Reset Result Parameter structure adrv904x_CpuCmd_SerResetResp_t
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_SerializerReset_v2(adi_adrv904x_Device_t* const device,
                                                                 adi_adrv904x_CpuCmd_SerReset_t* const pSerResetParms,
                                                                 adi_adrv904x_CpuCmd_SerResetResp_t* const pSerResetResp);

/**
 * \brief Sets phase adjustment value(i.e. Lmfc offset) for selected framer.
 *
 * \pre This function can be called after JESD204B/C initialization
 *
 * Maximum value for lmfcOffset variable is (JESD K param* JESD S param) - 1
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param [in, out] device Context variable -Pointer to the device settings structure
 * \param[in] framerSelect Framer selection to configure lmfc offset
 * \param[in] lmfcOffset Phase adjustment value for selected framer
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_FramerLmfcOffsetSet(adi_adrv904x_Device_t*  const device,
                                                                  const adi_adrv904x_FramerSel_e  framerSelect,
                                                                  const uint16_t lmfcOffset);

/**
 * \brief Reads phase adjustment value(i.e. Lmfc offset) of selected framer
 *
 * \pre This function can be called after JESD204B/C initialization
 *
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param [in, out] device Context variable -Pointer to the device settings structure
 * \param[in] framerSelect Deframer selection to readback lmfc offset
 * \param[out] lmfcOffset Pointer to readback value for phase adjustment
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_FramerLmfcOffsetGet(adi_adrv904x_Device_t*  const device,
                                                                  const adi_adrv904x_FramerSel_e  framerSelect,
                                                                  uint16_t * const lmfcOffset);

/**
 * \brief Sets phase adjustment value(i.e. Lmfc offset) for selected value
 *
 * \pre This function can be called after JESD204B/C initialization
 *
 * Maximum value for lmfcOffset variable is (JESD K param* JESD S param) - 1
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param [in, out] device Context variable -Pointer to the device settings structure
 * \param[in] deframerSelect Deframer selection to configure lmfc offset
 * \param[in] lmfcOffset Phase adjustment value for selected deframer
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfrmLmfcOffsetSet(adi_adrv904x_Device_t*  const device,
                                                                const adi_adrv904x_DeframerSel_e  deframerSelect,
                                                                const uint16_t lmfcOffset);

/**
 * \brief Reads phase adjustment value(i.e. Lmfc offset) of selected deframer
 *
 * \pre This function can be called after JESD204B/C initialization
 *
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param [in, out] device Context variable -Pointer to the device settings structure
 * \param[in] deframerSelect Deframer selection to readback lmfc offset
 * \param[out] lmfcOffset Pointer to readback value for phase adjustment
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfrmLmfcOffsetGet(adi_adrv904x_Device_t*  const device,
                                                                const adi_adrv904x_DeframerSel_e  deframerSelect,
                                                                uint16_t * const lmfcOffset);

/**
 * \brief Reads phase diff value for selected deframer
 *
 * \pre This function can be called after JESD204B/C initialization
 *
 * This function calculates the phase diff value after the adjustment which is
 * configured with adi_adrv904x_DfrmLmfcOffsetSet(). This phase difference can
 * be used to determine how to set phase adjustment
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param [in, out] device Context variable -Pointer to the device settings structure
 * \param[in] deframerSelect Deframer selection to readback lmfc offset
 * \param[out] phaseDiff Pointer to readback value for phase diff
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfrmPhaseDiffGet(adi_adrv904x_Device_t*  const device,
                                                               const adi_adrv904x_DeframerSel_e  deframerSelect,
                                                               uint16_t * const phaseDiff);
/**
* \brief Get framer status for the selected framer. Support for both JESD 204B/C.
*
* \pre This function can be called after JESD204B/C initialization
* 204B and 204C: framerStatus->status bit pattern meaning:
*        b0: Input cfg not supported
*        b1: SYSREF phase established by framer
*        b2: SYSREF phase error - a new SYSREF had different
*            timing than the first that set the LMFC timing.
*        b3: pclk slow error
*        b4: pclk fast error
*        b5: reserved
*        b6: reserved
*        b7: Link type: 0: 204B, 1: 204C
*
* 204B: framerStatus->qbfStateStatus numeric value:
*        0x0     CGS
*        0x1     ILA_M0R
*        0x2     ILA_M0
*        0x3     ILA_M1R
*        0x4     ILA_M1C1
*        0x5     ILA_M1C2
*        0x6     ILA_M1C3
*        0x7     ILA_M1
*        0x8     ILA_M2R
*        0x9     ILA_M2
*        0xA     ILA_M3R
*        0xB     ILA_M3
*        0xC     ILA_BP
*        0xD     UDATA
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in]  device Context variable - Pointer to the ADRV904X data structure
* \param[in]  framerSel Framer selection
* \param[out] framerStatus Pointer to structure contains status for both 204B/C framer.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_FramerStatusGet(adi_adrv904x_Device_t  *  const device,
                                                              const adi_adrv904x_FramerSel_e framerSel,
                                                              adi_adrv904x_FramerStatus_t * const framerStatus);

/**
* \brief Get deframer status for the selected deframer. Support for both JESD 204B/C.
* This function is deprecated and replaced by adi_adrv904x_DeframerStatusGet_v2()
*
* \pre This function can be called after JESD204B/C initialization
*
*   Status for JESD204B: in structure adi_adrv904x_DeframerStatus_t
*   deframerStatus  |  Bit Name       |  Description
*   ----------------|-----------------|--------------------------------------------------
*              [7]  | Reserved        | This bit is deprecated. For checksum status use adi_adrv904x_DfrmIlasMismatchGet().
*              [6]  | EOF Event       | This bit captures the internal status of the framer End of Frame event. Value =1 if framing error during ILAS
*              [5]  | EOMF Event      | This bit captures the internal status of the framer End of Multi-Frame event. Value =1 if framing error during ILAS
*              [4]  | FS Lost         | This bit captures the internal status of the framer Frame Symbol event. Value =1 if framing error during ILAS or user data (invalid replacement characters)
*              [3]  | Reserved        | Reserved
*              [2]  | User Data Valid | =1 when in user data (deframer link is up and sending valid DAC data)
*              [1]  | SYSREF Received | Deframer has received the external SYSREF signal
*              [0]  | Syncb level     | Current level of Syncb signal internal to deframer (=1 means link is up)
*
*   Status for JESD204C: in structure adi_adrv904x_DeframerStatus_t
*   deframerStatus  |  Bit Name                |  Description
*   ----------------|--------------------------|--------------------------------------------------
*            [2:0]  |  Current lock state      | These 3 bits indicate state of JESD204C Lock
*
*   An explanation of what each the value of the 3 bits relates to is below:
*            value  |  Description
*   ----------------|-----------------------------------------------------------------------------
*              0    |  Reset
*              1    |  Unlocked
*              2    |  Block(Blocks aligned)
*              3    |  M_Block(Lanes aligned)
*              4    |  E_M_Block(Multiblock aligned)
*              5    |  FEC_BUF
*              6    |  FEC_READY (Good State!)
*              7    |  Forced
*
*   reserved: in structure adi_adrv904x_DeframerStatus_t
*   Bit position    |  Bit Name                |  Description
*   ----------------|--------------------------|--------------------------------------------------
*            [0]    |  JESD204 type            | 0: 204B, 1: 204C
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in]  device Context variable - Pointer to the ADRV904X data structure
* \param[in]  deframerSel Deframer selection
* \param[out] deframerStatus Pointer to structure contains status for both 204B/C deframer.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DeframerStatusGet(adi_adrv904x_Device_t *  const device,
                                                                const adi_adrv904x_DeframerSel_e deframerSel,
                                                                adi_adrv904x_DeframerStatus_t * const deframerStatus);

/**
* \brief Get deframer status for all lanes in the selected deframer. Support for both JESD 204B/C.
*
* \pre This function can be called after JESD204B/C initialization
*
*   Structure adi_adrv904x_DeframerStatus_v2_t
*
*   phyLaneMask: The lane bit masks of lanes used by this selected deframer.
*
*   linkState       |  Bit Name                |  Description
*   ----------------|--------------------------|--------------------------------------------------
*            [0]    |  Link type               | 1: link type 204C. 0: link type 204B
*            [1]    |  Link state              | 1: link is up. 0: link is down
*
*   laneStatus: an array to contain status of all lanes.
*
*   Status for JESD204B: laneStatus
*   laneStatus[X]   |  Bit Name       |  Description
*   ----------------|-----------------|--------------------------------------------------
*              [7]  | Reserved        | This bit is deprecated. For checksum status use adi_adrv904x_DfrmIlasMismatchGet().
*              [6]  | EOF Event       | This bit captures the internal status of the framer End of Frame event. Value =1 if framing error during ILAS
*              [5]  | EOMF Event      | This bit captures the internal status of the framer End of Multi-Frame event. Value =1 if framing error during ILAS
*              [4]  | FS Lost         | This bit captures the internal status of the framer Frame Symbol event. Value =1 if framing error during ILAS or user data (invalid replacement characters)
*              [3]  | LaneValid       | This lane is used by the selected deframer.
*              [2]  | User Data Valid | =1 when in user data (deframer link is up and sending valid DAC data)
*              [1]  | SYSREF Received | Deframer has received the external SYSREF signal
*              [0]  | Syncb level     | Current level of Syncb signal internal to deframer (=1 means link is up)
*
*   Status for JESD204C: laneStatus
*   laneStatus[X]   |  Bit Name                |  Description
*   ----------------|--------------------------|--------------------------------------------------
*            [2:0]  |  Current lock state      | These 3 bits indicate state of JESD204C Lock
*            [3]    |  LaneValid               | This lane is used by the selected deframer.
*
*   An explanation of what each the value of the 3 bits Current lock state [2:0] is below:
*            value  |  Description
*   ----------------|-----------------------------------------------------------------------------
*              0    |  Reset
*              1    |  Unlocked
*              2    |  Block(Blocks aligned)
*              3    |  M_Block(Lanes aligned)
*              4    |  E_M_Block(Multiblock aligned)
*              5    |  FEC_BUF
*              6    |  FEC_READY (Good State!)
*              7    |  Forced
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in]  device Context variable - Pointer to the ADRV904X data structure
* \param[in]  deframerSel Deframer selection
* \param[out] deframerStatus Pointer to structure contains status for all lanes for the selected deframer.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DeframerStatusGet_v2(adi_adrv904x_Device_t*  const device,
                                                                   const adi_adrv904x_DeframerSel_e deframerSel,
                                                                   adi_adrv904x_DeframerStatus_v2_t * const deframerStatus);

/**
* \brief Get deframer status for the selected deframer. Support for only JESD204B.
*
* \pre This function can be called after JESD204B initialization
*
*   Lane Status of structure adi_adrv904x_DfrmErrCounterStatus_t
*   Bit   |                     Description
*---------|-------------------------------------------------------------
*   [7]   | UEK Error, 0 = No Error, 1 = Error
*   [6]   | NIT Error, 0 = No Error, 1 = Error
*   [5]   | ILS Status, 0 = Not Completed, 1 = Completed Successfully
*   [4]   | ILD Status, 0 = Not Completed, 1 = Completed Successfully
*   [3]   | FS Status, 0 = Not Completed, 1 = Completed Successfully
*   [2]   | CCS Status, 0 = Not Completed, 1 = Completed Successfully
*   [1]   | CGS Status, 0 = Not Completed, 1 = Completed Successfully
*   [0]   | BD Error, 0 = No Error, 1 = Error
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in]  device Context variable - Pointer to the ADRV904X data structure
* \param[in]  deframerSel Deframer selection
* \param[in]  laneNumber selects the lane to read (values from 0 - 7)
* \param[out] errCounterStatus Pointer to structure for returning the lane status and error count values.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfrmErrCounterStatusGet(adi_adrv904x_Device_t *  const device,
                                                                      const adi_adrv904x_DeframerSel_e deframerSel,
                                                                      const uint8_t laneNumber,
                                                                      adi_adrv904x_DfrmErrCounterStatus_t * const errCounterStatus);

/**
* \brief Clear the error counters selected deframer. Support for only JESD204B.
*
* \pre This function can be called after JESD204B initialization
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in]  device Context variable - Pointer to the ADRV904X data structure
* \param[in]  deframerSel Deframer selection
* \param[in]  laneNumber selects the lane to read (values from 0 - 7)
* \param[in] errCounterMask bitmasks to indicate which error type counters to be cleared.
*                 b0: BD clear, b1: INT clear, b2: UEK clear.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfrmErrCounterReset(adi_adrv904x_Device_t*  const device,
                                                                  const adi_adrv904x_DeframerSel_e deframerSel,
                                                                  const uint8_t laneNumber,
                                                                  uint32_t const errCounterMask);

/**
* \brief Sets a threshold for CRC errors, which when exceeded, triggers the corresponding Deframer IRQ on any lane.
*        Lanes error counts are considered independently not the cumulative error count across all lanes.
*        A threshold value of 0, which is the default, will trigger the IRQ for any CRC error.
*
* \pre This function can be called after JESD204B/C initialization
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in, out] device Context variable - Pointer to the ADRV904X data structure
* \param[in]      threshold numbers of CRC errors to trigger the corresponding Deframer IRQ (default value: 0)
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfrmErrCounterThresholdSet(adi_adrv904x_Device_t* const device,
                                                                         const uint8_t                threshold);

/**
* \brief Get deframer status for the selected deframer. Support for only JESD204C.
*
* \pre This function can be called after JESD204C initialization
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in]  device Context variable - Pointer to the ADRV904X data structure
* \param[in]  deframerSel Deframer selection
* \param[in]  laneNumber selects the lane to read (values from 0 - 7)
* \param[out] errCounterStatus Pointer to structure for returning the lane status and error count values.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_Dfrm204cErrCounterStatusGet(adi_adrv904x_Device_t *  const device,
                                                                          const adi_adrv904x_DeframerSel_e deframerSel,
                                                                          const uint8_t laneNumber,
                                                                          adi_adrv904x_Dfrm204cErrCounterStatus_t * const errCounterStatus);

/**
* \brief Clear the error counters selected deframer. Support for only JESD204C.
*
* \pre This function can be called after JESD204C initialization
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in]  device Context variable - Pointer to the ADRV904X data structure
* \param[in]  deframerSel Deframer selection
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_Dfrm204cErrCounterReset(adi_adrv904x_Device_t  *  const device,
                                                                      const adi_adrv904x_DeframerSel_e deframerSel);

/**
* \brief to get a deframer link condition up or down. Support for both JESD204 B/C
*
* \pre This function can be called after JESD204B/C initialization
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in]  device Context variable - Pointer to the ADRV904X data structure
* \param[in]  deframerSel Deframer selection
* \param[out] dfrmLinkCondition pointer to a 8 bit value: 0: down, 1: up
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfrmLinkConditionGet(adi_adrv904x_Device_t *  const device,
                                                                   const adi_adrv904x_DeframerSel_e deframerSel,
                                                                   uint8_t * const dfrmLinkCondition);

/**
* \brief This function will return the elastic FIFO depth for the selected deframer.
*
* \pre This function can be called after JESD204B/C initialization
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in]  device Context variable - Pointer to the ADRV904X data structure
* \param[in]  deframerSel Deframer selection
* \param[in]  laneNumber selects the lane to read (values from 0 - 7)
* \param[out] fifoDepth Pointer that will hold the elastic FIFO depth.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfrmFifoDepthGet(adi_adrv904x_Device_t*  const device,
                                                               const adi_adrv904x_DeframerSel_e deframerSel,
                                                               const uint8_t laneNumber,
                                                               uint8_t * const fifoDepth);

/**
* \brief This function will return the core buffer depth for the selected deframer.
*
* \pre This function can be called after JESD204B/C initialization
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in]  device Context variable - Pointer to the ADRV904X data structure
* \param[in]  deframerSel Deframer selection
* \param[out] coreBufDepth Pointer that will hold the core buffer depth.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfrmCoreBufDepthGet(adi_adrv904x_Device_t*  const device,
                                                                  const adi_adrv904x_DeframerSel_e deframerSel,
                                                                  uint8_t * const coreBufDepth);

/**
 * \brief This API is deprecated and replaced by adi_adrv904x_DfrmIlasMismatchGet_v2().
 *        Compares received Lane0 ILAS configuration to ADRV904X deframer
 *        configuration and returns 32-bit mask indicating values that
 *        mismatched.  Actual lane0 ILAS configuration and deframer
 *        configuration values can be obtained by passing a pointer to a
 *        structure of type adi_adrv904x_DfrmCompareData_t.
 *
 * \pre The Rx JESD204B link(s) needs to be configured and running to use
 *      this function
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * mismatch Mask|  Description
 * -------------|------------------------------------------
 *         [14] | Lane0 Checksum, 0 = match, 1 = mismatch
 *         [13] | HD,   0 = match, 1 = mismatch
 *         [12] | CF,   0 = match, 1 = mismatch
 *         [11] | S,    0 = match, 1 = mismatch
 *         [10] | NP,   0 = match, 1 = mismatch
 *         [9]  | CS,   0 = match, 1 = mismatch
 *         [8]  | N,    0 = match, 1 = mismatch
 *         [7]  | M,    0 = match, 1 = mismatch
 *         [6]  | K,    0 = match, 1 = mismatch
 *         [5]  | F,    0 = match, 1 = mismatch
 *         [4]  | SCR,  0 = match, 1 = mismatch
 *         [3]  | L,    0 = match, 1 = mismatch
 *         [2]  | LID0, 0 = match, 1 = mismatch
 *         [1]  | BID,  0 = match, 1 = mismatch
 *         [0]  | DID,  0 = match, 1 = mismatch
 *
 * \param[in]  device Context variable - Pointer to the ADRV904X data structure
 * \param[in]  deframerSel Deframer selection
 * \param[out] dfrmData Pointer to a adi_adrv904x_DfrmCompareData_t structure that
 *                      returns the deframer ILAS and configuration settings as well as the
 *                      mismatch flag and zero data flag.  If the zero flag is set to zero,
 *                      then no valid ILAS data found indicating link is not enabled.
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfrmIlasMismatchGet(adi_adrv904x_Device_t*  const device,
                                                                  const adi_adrv904x_DeframerSel_e deframerSel,
                                                                  adi_adrv904x_DfrmCompareData_t* const dfrmData);

/**
 * \brief Compares received all Lane ILAS configurations to ADRV904X deframer
 *        configuration and returns 32-bit mask indicating values that
 *        mismatched for each lane.  Actual all lane ILAS configurations and deframer
 *        configuration values can be obtained by passing a pointer to a
 *        structure of type adi_adrv904x_DfrmCompareData_v2_t.
 *
 * \pre The Rx JESD204B link(s) needs to be configured and running to use
 *      this function
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * mismatch Mask|  Description (adi_adrv904x_IlasMismatch_e)
 * -------------|------------------------------------------
 *         [14] | Lane Checksum, 0 = match, 1 = mismatch
 *         [13] | HD,   0 = match, 1 = mismatch
 *         [12] | CF,   0 = match, 1 = mismatch
 *         [11] | S,    0 = match, 1 = mismatch
 *         [10] | NP,   0 = match, 1 = mismatch
 *         [9]  | CS,   0 = match, 1 = mismatch
 *         [8]  | N,    0 = match, 1 = mismatch
 *         [7]  | M,    0 = match, 1 = mismatch
 *         [6]  | K,    0 = match, 1 = mismatch
 *         [5]  | F,    0 = match, 1 = mismatch
 *         [4]  | SCR,  0 = match, 1 = mismatch
 *         [3]  | L,    0 = match, 1 = mismatch
 *         [2]  | LID0, 0 = match, 1 = mismatch
 *         [1]  | BID,  0 = match, 1 = mismatch
 *         [0]  | DID,  0 = match, 1 = mismatch
 *
 * \param[in]  device Context variable - Pointer to the ADRV904X data structure
 * \param[in]  deframerSel Deframer selection
 * \param[out] dfrmData Pointer to a adi_adrv904x_DfrmCompareData_v2_t structure that
 *                      returns the deframer ILAS and all lane configurations settings as well as the
 *                      mismatch flag and zero data flag.  If the zero flag is set to zero,
 *                      then no valid ILAS data found indicating link is not enabled.
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfrmIlasMismatchGet_v2(adi_adrv904x_Device_t*  const device,
                                                                     const adi_adrv904x_DeframerSel_e deframerSel,
                                                                     adi_adrv904x_DfrmCompareData_v2_t* const dfrmData);
/**
* \brief This function will enable JESD framer loopback for the selected framer.
*        The ADX Crossbar setting is changed to loopback setting.
*
* \pre This function can be called after JESD204B/C initialization
*      - At least one Rx, one TX, and one deframer must be enabled.
*      - IQ Sample clock for deframer and framer must match to avoid over/under-sampling.
*      - Samples are sent to TX and received at RX.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in]  device Context variable - Pointer to the ADRV904X data structure
* \param[in]  framerSel framer selection to enable loopback. Only one framer is allowed.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_FramerLoopbackSet(adi_adrv904x_Device_t*  const device,
                                                                const adi_adrv904x_FramerSel_e framerSel);

/**
* \brief This function will disable JESD framer loopback. adi_adrv904x_FramerLoopbackSet changes the adcCrossbar values,
*        so user wants to disable the FramerLoopback and restore the previous adcCrossbar value, they must save it and call 
*        it as in argument of this function. 
*
* \pre This function can be called after adi_adrv904x_FramerLoopbackSet
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in]  device Context variable - Pointer to the ADRV904X data structure
* \param[in]  framerSel framer selection to disable loopback. Only one framer is allowed.
* \param[in]  adcXbar pointer to intended adcXbar configuration for after FramerLoopback is disabled.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_FramerLoopbackDisable(adi_adrv904x_Device_t* const          device,
                                                                    const adi_adrv904x_FramerSel_e        framerSel,
                                                                    const adi_adrv904x_AdcSampleXbarCfg_t* const adcXbar);


/**
* \brief This function will enable JESD deframer loopback.
*
* \pre This function can be called after JESD204B/C initialization
*      - Framer-0 and deframer-0 must be enabled.
*      - At least one Rx, one TX must be enabled.
*      - IQ Sample clock for deframer and framer must match to avoid over/under-sampling.
*      - Samples are sent to RX and received at TX.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in]  device Context variable - Pointer to the ADRV904X data structure
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DeframerLoopbackSet(adi_adrv904x_Device_t*  const device);

/**
* \brief This function will disable JESD deframer loopback.
*
* \pre This function can be called after adi_adrv904x_FramerLoopbackSet
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in]  device Context variable - Pointer to the ADRV904X data structure
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DeframerLoopbackDisable(adi_adrv904x_Device_t*  const device);


/**
* \brief This function will disable JESD deframer loopback. adi_adrv904x_DeframerLoopbackSet changes the adcCrossbar values,
*        so user wants to disable the DeframerLoopback and restore the previous adcCrossbar value, they must save it and call 
*        it as in argument of this function. 
*
* \pre This function can be called after adi_adrv904x_DeframerLoopbackSet
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in]  device Context variable - Pointer to the ADRV904X data structure
* \param[in]  framerSel framer selection to disable loopback. Only one framer is allowed.
* \param[in]  adcXbar pointer to intended adcXbar configuration for after FramerLoopback is disabled.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DeframerLoopbackDisable_v2(adi_adrv904x_Device_t* const                 device,
                                                                         const adi_adrv904x_FramerSel_e               framerSel,
                                                                         const adi_adrv904x_AdcSampleXbarCfg_t* const adcXbar);

/**
* \brief This function will enable JESD deframer lane loopback.
*
* \pre This function can be called after JESD204B/C initialization
*      - At least one Rx, one TX, one framer, and one deframer must be enabled.
*      - IQ Sample clock for deframer and framer must match to avoid over/under-sampling.
*      - Samples are sent to RX and received at TX.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in]  device Context variable - Pointer to the ADRV904X data structure
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DeframerLaneLoopbackSet(adi_adrv904x_Device_t*  const device);

/**
* \brief This function will disable JESD deframer lane loopback.
*
* \pre This function can be called after adi_adrv904x_DeframerLaneLoopbackSet.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in]  device Context variable - Pointer to the ADRV904X data structure
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DeframerLaneLoopbackDisable(adi_adrv904x_Device_t*  const device);

/**
 * \brief Set the framer JESD204B syncb signal mode to SPI or PIN mode
 *
 * \pre This function is called after the device has been initialized
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in]  device Context variable - Pointer to the ADRV904X data structure
 * \param[in]  framerSelMask Select framer to set syncb mode
 * \param[in]  syncbMode Value to set syncb mode, '0' PIN mode, '1' SPI mode
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_FramerSyncbModeSet(adi_adrv904x_Device_t* const  device,
                                                                 const uint8_t                 framerSelMask,
                                                                 const uint8_t                 syncbMode);

/**
 * \brief Get the framer JESD204B syncb signal mode
 *
 * \pre This function is called after the device has been initialized
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in]  device Context variable - Pointer to the ADRV904X data structure
 * \param[in]  framerSelMask Select framer to read syncb status, only one framer could be selected
 * \param[out]  syncbMode Value to readback syncb mode, '0' PIN mode, '1' SPI mode
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_FramerSyncbModeGet(adi_adrv904x_Device_t* const  device,
                                                                 const uint8_t                 framerSelMask,
                                                                 uint8_t* const                syncbMode);

/**
 * \brief Set the framer JESD204B syncb signal status
 *
 * \pre This function is called after the device has been initialized and run in the syncb SPI mode only
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in]  device Context variable - Pointer to the ADRV904X data structure
 * \param[in]  framerSelMask Select framer to set syncb status
 * \param[in]  syncbStatus Value to set syncb status, '1' the syncb is valid, '0' the syncb is invalid
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_FramerSyncbStatusSet(adi_adrv904x_Device_t* const  device,
                                                                   const uint8_t                 framerSelMask,
                                                                   const uint8_t                 syncbStatus);

/**
 * \brief Get the framer JESD204B syncb signal status
 *
 * \pre This function is called after the device has been initialized
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in]  device Context variable - Pointer to the ADRV904X data structure
 * \param[in]  framerSelMask Select framer to read syncb status, only one framer could be selected
 * \param[out]  syncbStatus Value to set syncb status, '1' the syncb is valid, '0' the syncb is invalid
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_FramerSyncbStatusGet(adi_adrv904x_Device_t* const  device,
                                                                   const uint8_t                 framerSelMask,
                                                                   uint8_t* const                syncbStatus);

/**
 * \brief Get the framer JESD204B syncb signal error counter
 *
 * \pre This function is called after the device has been initialized
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in]  device Context variable - Pointer to the ADRV904X data structure
 * \param[in]  framerSelMask Select framer to read syncb error counter, only one framer could be selected
 * \param[out]  syncbErrCnt value to readback syncb error counter
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_FramerSyncbErrCntGet(adi_adrv904x_Device_t* const  device,
                                                                   const uint8_t                 framerSelMask,
                                                                   uint8_t* const                syncbErrCnt);

/**
 * \brief Reset the framer JESD204B syncb signal error counter
 *
 * \pre This function is called after the device has been initialized
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in]  device Context variable - Pointer to the ADRV904X data structure
 * \param[in]  framerSelMask Select framer to reset syncb error counter
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_FramerSyncbErrCntReset(adi_adrv904x_Device_t* const  device,
                                                                     const uint8_t                 framerSelMask);

/**
 * \brief Get the deframer JESD204B syncb signal error counter
 *
 * \pre This function is called after the device has been initialized
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in]  device Context variable - Pointer to the ADRV904X data structure
 * \param[in]  deframerSelMask Select deframer to read syncb error counter, only one deframer could be selected
 * \param[out]  syncbErrCnt value to readback syncb error counter
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DeframerSyncbErrCntGet(adi_adrv904x_Device_t* const  device,
                                                                     const uint8_t                 deframerSelMask,
                                                                     uint8_t* const                syncbErrCnt);

/**
 * \brief Reset the deframer JESD204B syncb signal error counter
 *
 * \pre This function is called after the device has been initialized
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in]  device Context variable - Pointer to the ADRV904X data structure
 * \param[in]  deframerSelMask Select deframer to reset syncb error counter
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DeframerSyncbErrCntReset(adi_adrv904x_Device_t* const  device,
                                                                       const uint8_t                 deframerSelMask);

/**
 * \brief Clears, Enables or disables the Framer errors:
 *         - PCLK related errors, i.e., whether it is faster/slower than the sample/convertor clock
 *         - SYSREF Phase Error
 *
 * \pre This function is called after the device has been initialized
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in]  device Context variable - Pointer to the ADRV904X data structure
 * \param[in]  framerSel - selected framer defined in adi_adrv904x_FramerSel_e
 * \param[in]  action - selected error action defined in adi_adrv904x_SerdesErrAction_e
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_FramerErrorCtrl(adi_adrv904x_Device_t* const          device,
                                                              const adi_adrv904x_FramerSel_e        framerSel,
                                                              const adi_adrv904x_SerdesErrAction_e  action);



/**
 * \brief Clears, Enables or disables the Deframer errors:
 *         - PCLK related errors, i.e., whether it is faster/slower than the sample/convertor clock
 *         - SYSREF Phase Error
 *
 * \pre This function is called after the device has been initialized
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in]  device Context variable - Pointer to the ADRV904X data structure
 * \param[in]  deframerSel - selected deframer defined in adi_adrv904x_DeframerSel_e
 * \param[in]  action - selected error action defined in adi_adrv904x_SerdesErrAction_e
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DeframerErrorCtrl(adi_adrv904x_Device_t* const          device,
                                                                const adi_adrv904x_DeframerSel_e      deframerSel,
                                                                const adi_adrv904x_SerdesErrAction_e  action);

/**
 * \brief Reads the JESD 204B IRQ interrupt clear register. There is one common register for all deframers.
 *
 * This function reads the contents of the deframer 204B IRQ clear register.  Use this function whenever a general purpose (GP)
 * deframer IRQ asserts to find the maskable deframer IRQ sources.  Note: Deframer IRQ sources Elastic Buffer Error Flag,
 * Sysref Buffer Error, and the four Lane FIFO Async Error IRQ sources are always enabled and can not be masked in the
 * interrupt clear register.
 *
 * \pre This function may be called any time after JESD204B link initialization
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
 * \param[out] irqMask Pointer to the bit mask value containing the contents of the deframer IRQ Clear Register
 *
 *                 Bit   |  Description
 *          -------------|-----------------------------------------------------------------------------
 *                  [15] | UnUsed
 *                  [14] | UnUsed
 *                  [13] | UnUsed
 *                  [12] | UnUsed
 *                  [11] | UnUsed
 *                  [10] | UnUsed
 *                  [9]  | UnUsed
 *                  [8]  | CMM - Configuration mismatch for lane0  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [7]  | BD - Bad Disparity error counter  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [6]  | Not-In-Table error counter  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [5]  | Unexpected K error counter  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [4]  | ILD - Inter-lane De-skew  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [3]  | ILS - Initial lane sync  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [2]  | GCS - Good Check Sum  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [1]  | FS - Frame Sync 0 = No Interrupt, 1 = Interrupt Asserted
 *                  [0]  | CSG - Code Group Sync  0 = No Interrupt, 1 = Interrupt Asserted
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfrmIrqMaskGet(   adi_adrv904x_Device_t*  const   device,
                                                                uint16_t* const                 irqMask);

/**
 * \brief Writes the JESD 204B IRQ interrupt clear register. There is one common register for all deframers.
 *
 * This function writes the specified IRQ mask value to the deframer 204B IRQ clear register.  Use this function whenever a general purpose (GP)
 * deframer IRQ asserts to clear the pending maskable deframer IRQ or to enable/disable deframer interrupt sources. Note: Deframer IRQ sources
 * Elastic Buffer Error Flag, Sysref Buffer Error, and the four Lane FIFO Async Error IRQ sources are always enabled and can not be masked in the
 * interrupt clear register.  This function does not read-modify-write the interrupt clear register. To manually clear the interrupt, write
 * a one (set) to disable or mask the bit of the interrupt followed by writing a zero (clear) to enable the bit of the interrupt.
 * However, if the interrupt condition still exists after setting the mask bit, the corresponding IRQ vector bit will re-assert.
 *
 * \pre This function may be called any time after Jesd204B link initialization
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
 * \param[in]  irqMask Pointer to the bit mask value to be written to the deframer IRQ Clear Register (this is not a read-modify-write)
 *
 *                 Bit   |  Description
 *          -------------|-----------------------------------------------------------------------------
 *                  [15] | UnUsed
 *                  [14] | UnUsed
 *                  [13] | UnUsed
 *                  [12] | UnUsed
 *                  [11] | UnUsed
 *                  [10] | UnUsed
 *                  [9]  | UnUsed
 *                  [8]  | CMM - Configuration mismatch for lane0  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [7]  | BD - Bad Disparity error counter  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [6]  | Not-In-Table error counter  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [5]  | Unexpected K error counter  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [4]  | ILD - Inter-lane De-skew  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [3]  | ILS - Initial lane sync  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [2]  | GCS - Good Check Sum  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [1]  | FS - Frame Sync 0 = No Interrupt, 1 = Interrupt Asserted
 *                  [0]  | CSG - Code Group Sync  0 = No Interrupt, 1 = Interrupt Asserted
 *
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfrmIrqMaskSet(   adi_adrv904x_Device_t*  const   device,
                                                                const uint16_t                  irqMask);

/**
 * \brief Reset the JESD 204B IRQ interrupt clear register. There is one common register for all deframers.
 *
 * This function clears all deframer 204B IRQ sources.  Use this function whenever a general purpose (GP)
 * deframer IRQ asserts to clear the pending deframer IRQ.
 *
 * \pre This function may be called any time after JESD204b link initialization
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfrmIrqSourceReset(   adi_adrv904x_Device_t*  const   device);

/**
 * \brief Read the JESD 204B IRQ interrupt source registers for the specified deframer.
 *
 * This function fetches the contents of the deframer 204B IRQ Vector register and other IRQ sources.  Use this function whenever
 * a general purpose (GP) deframer IRQ asserts to determine the source of the deframer IRQ.  Common IRQ sources are Bad Disparity (BD),
 * Not-In-Table (NIT), and Unexpected K-char (UEK) counters greater than the specified error threshold count value.
 *
 * \pre This function may be called any time after JESD204B link initialization
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
 * \param[in]  deframerSelect Selects the deframer to interrogate.
 * \param[out] irqSourceVector Pointer to a bit mask containing the status of the IRQ Vector source register at read time.
 *
 *                 Bit   |  Description
 *          -------------|-----------------------------------------------------------------------------
 *                  [15] | UnUsed
 *                  [14] | UnUsed
 *                  [13] | UnUsed
 *                  [12] | UnUsed
 *                  [11] | UnUsed
 *                  [10] | UnUsed
 *                  [9]  | UnUsed
 *                  [8]  | CMM - Configuration mismatch for lane0  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [7]  | BD - Bad Disparity error counter  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [6]  | Not-In-Table error counter  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [5]  | Unexpected K error counter  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [4]  | ILD - Inter-lane De-skew  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [3]  | ILS - Initial lane sync  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [2]  | GCS - Good Check Sum  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [1]  | FS - Frame Sync 0 = No Interrupt, 1 = Interrupt Asserted
 *                  [0]  | CSG - Code Group Sync  0 = No Interrupt, 1 = Interrupt Asserted
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfrmIrqSourceGet( adi_adrv904x_Device_t*  const               device,
                                                                const adi_adrv904x_DeframerSel_e            deframerSelect,
                                                                adi_adrv904x_DeframerIrqVector_t* const     irqSourceVector);

/**
* \brief Used to configure and reset the deframer JESD 204B Error counters Bad Disparity Error (BD),
*        Not-In-Table (NIT), Unexpected K-char (UEK).
*
* This function allows individual counter enable/disable, reset, and configuration control
* for all lanes. IRQ Mask updates will affect all deframers identically.
* This function will set the error counter interrupt bits at deframer interrupt request
* mask according to interruptEnable input
*
* \pre This function may be called any time after Jesd link initialization
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
* \param[in]  interruptEnable argument to enable/disable GPINT in case error counters overflow 255
* \param[in]  laneNumber Lane number for which to set config.
* \param[in]  errCounterControl 8-bit mask for enabling/disabling and resetting individual counters.
*
*     bit  |  Lane
*     -----|------------------
*       0  |  Bad Disparity Error 0 = disable, 1 = enable
*       1  |  Not-In-Table (NIT) 0 = disable, 1 = enable
*       2  |  Unexpected K-char (UEK) 0 = disable, 1 = enable
*       3  |  Reserved
*       4  |  Bad Disparity Error 0 = Do not reset counter, 1 = Reset counter to zero
*       5  |  Not-In-Table (NIT) 0 = Do not reset counter, 1 = Reset counter to zero
*       6  |  Unexpected K-char (UEK) 0 = Do not reset counter, 1 = Reset counter to zero
*       7  |  Reserved
*
* \param[in]  errCounterHoldCfg 8-bit mask for configuring the deframer error counters to stop counting
*               on overflow from 0xFF or to reset and continue counting.
*
*     bit  |  Lane
*     -----|------------------
*       0  |  Bad Disparity Error 0 = Reset and continue counting, 1 = Stop counting at 0xFF overflow
*       1  |  Not-In-Table (NIT) 0 = Reset and continue counting, 1 = Stop counting at 0xFF overflow
*       2  |  Unexpected K-char (UEK) 0 = Reset and continue counting, 1 = Stop counting at 0xFF overflow
*       3  |  Reserved
*       4  |  Reserved
*       5  |  Reserved
*       6  |  Reserved
*       7  |  Reserved
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfrmErrCntrCntrlSet(  adi_adrv904x_Device_t*  const               device,
                                                                    const adi_adrv904x_DfrmErrCounterIrqSel_e   interruptEnable,
                                                                    const uint8_t                               laneNumber,
                                                                    const uint8_t                               errCounterControl,
                                                                    const uint8_t                               errCounterHoldCfg);

/**
* \brief Used to read the configuration for the deframer JESD 204B error counters Bad Disparity Error (BD),
*        Not-In-Table (NIT), Unexpected K-char (UEK). Counter controls are common to all deframers.
*
* \pre This function may be called any time after Jesd link initialization
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
* \param[in]  laneNumber Lane number for which to read config.
* \param[out] errCounterControl Pointer to 8-bit variable containing individual counters enable bits.
*
*     bit  |  Lane
*     -----|------------------
*       0  |  Bad Disparity Error 0 = disable, 1 = enable
*       1  |  Not-In-Table (NIT) 0 = disable, 1 = enable
*       2  |  Unexpected K-char (UEK) 0 = disable, 1 = enable
*       3  |  Reserved
*       4  |  Reserved
*       5  |  Reserved
*       6  |  Reserved
*       7  |  Reserved
*
* \param[out] errCounterHoldCfg Pointer to 8-bit variable containing configuration for deframer error counters to stop counting
*        on overflow from 0xFF or to reset and continue counting.
*
*     bit  |  Lane
*     -----|------------------
*       0  |  Bad Disparity Error 0 = Reset and continue counting, 1 = Stop counting at 0xFF overflow
*       1  |  Not-In-Table (NIT) 0 = Reset and continue counting, 1 = Stop counting at 0xFF overflow
*       2  |  Unexpected K-char (UEK) 0 = Reset and continue counting, 1 = Stop counting at 0xFF overflow
*       3  |  Reserved
*       4  |  Reserved
*       5  |  Reserved
*       6  |  Reserved
*       7  |  Reserved
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfrmErrCntrCntrlGet(  adi_adrv904x_Device_t*  const       device,
                                                                    const uint8_t                       laneNumber,
                                                                    uint8_t* const                      errCounterControl,
                                                                    uint8_t* const                      errCounterHoldCfg);

/**
* \brief This function will run SERDES horizontal eye sweep.
*
* \pre This function can be called after JESD204B/C initialization
*      - At least one Rx, one TX, one framer, and one deframer must be enabled.
*      - IQ Sample clock for deframer and framer must match to avoid over/under-sampling.
*      - PRBS pattern matching the one selected in this API must be enabled on the sender side before running the test
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in]  device Context variable - Pointer to the ADRV904X data structure
* \param[in]  runEyeSweep Eye Sweep configuration
* \param[out] runEyeSweepResp Eye Sweep Response structure
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RunEyeSweep(adi_adrv904x_Device_t* const device,
                                                          const adi_adrv904x_CpuCmd_RunEyeSweep_t* const runEyeSweep,
                                                          adi_adrv904x_CpuCmd_RunEyeSweepResp_t* const runEyeSweepResp);

/**
* \brief This function will run SERDES horizontal eye sweep. For larger eyes and longer dwell times
*        the capture time can sometimes exceed the command server timeout especially when tracking
*        calibrations are in progress. To address this issue, a new Serdes Control Command is used.
*
* \pre This function can be called after JESD204B/C initialization
*      - At least one Rx, one TX, one framer, and one deframer must be enabled.
*      - IQ Sample clock for deframer and framer must match to avoid over/under-sampling.
*      - PRBS pattern matching the one selected in this API must be enabled on the sender side before running the test
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in]  device Context variable - Pointer to the ADRV904X data structure
* \param[in]  runEyeSweep Eye Sweep configuration
* \param[out] runEyeSweepResp Eye Sweep Response structure
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RunEyeSweep_v2(adi_adrv904x_Device_t* const device,
                                                             const adi_adrv904x_CpuCmd_RunEyeSweep_t* const runEyeSweep,
                                                             adi_adrv904x_CpuCmd_RunEyeSweepResp_t* const runEyeSweepResp);

/**
* \brief This function will run SERDES vertical eye sweep.
*
* \pre This function can be called after JESD204B/C initialization
*      - At least one Rx, one TX, one framer, and one deframer must be enabled.
*      - IQ Sample clock for deframer and framer must match to avoid over/under-sampling.
*      - PRBS pattern matching the one selected in this API must be enabled on the sender side before running the test
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in]  device Context variable - Pointer to the ADRV904X data structure
* \param[in]  runVerticalEyeSweep  Vertical Eye Sweep configuration
* \param[out] runVerticalEyeSweepResp Vertical Eye Sweep Response structure
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RunVerticalEyeSweep(adi_adrv904x_Device_t* const device,
                                                                  const adi_adrv904x_CpuCmd_RunVertEyeSweep_t* const runVerticalEyeSweep,
                                                                  adi_adrv904x_CpuCmd_RunVertEyeSweepResp_t* const runVerticalEyeSweepResp);

/**
* \brief This function will run SERDES vertical eye sweep. For larger eyes and longer dwell times
*        the capture time can sometimes exceed the command server timeout especially when tracking
*        calibrations are in progress. To address this issue, a new Serdes Control Command is used.
*
* \pre This function can be called after JESD204B/C initialization
*      - At least one Rx, one TX, one framer, and one deframer must be enabled.
*      - IQ Sample clock for deframer and framer must match to avoid over/under-sampling.
*      - PRBS pattern matching the one selected in this API must be enabled on the sender side before running the test
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in]  device Context variable - Pointer to the ADRV904X data structure
* \param[in]  runVerticalEyeSweep  Vertical Eye Sweep configuration
* \param[out] runVerticalEyeSweepResp Vertical Eye Sweep Response structure
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RunVerticalEyeSweep_v2(adi_adrv904x_Device_t* const device,
                                                                     const adi_adrv904x_CpuCmd_RunVertEyeSweep_t* const runVerticalEyeSweep,
                                                                     adi_adrv904x_CpuCmd_RunVertEyeSweepResp_t* const runVerticalEyeSweepResp);

/**
 * \brief Injects an error into the Framer test data by inverting the data
 *
 * This is a debug function to be used for debug of the Rx JESD204B/C lanes.
 * Rx data transmission on the JESD204B/C link(s) is not possible
 * when the framer test data is activated.
 *
 * \pre This function is called after the framer test data is enabled.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
 * \param[in] framerSelect Select the desired framer ADI_ADRV904X_FRAMER_0, ADI_ADRV904X_FRAMER_1, ADI_ADRV904X_FRAMER_2
 * \param[in] laneMask is an eight bit mask allowing selection of lanes 0-7 (at package balls) for the selected framer
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_FramerTestDataInjectError(adi_adrv904x_Device_t*  const  device,
                                                                        const adi_adrv904x_FramerSel_e framerSelect,
                                                                        const uint8_t  laneMask);

/**
 * \brief  Sets the serializer lane configuration parameters for
 * the chosen lane
 * \pre This function is called after the device has been initialized
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device     - Context variable, pointer to the device settings structure
 * \param[in]     laneNumber - Selects the serializer lane (values from 0 - 7)
 * \param[in]     serLaneCfg - Pointer to the Serializer Lane settings structure adi_adrv904x_SerLaneCfg_t
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_SerLaneCfgSet(adi_adrv904x_Device_t* const device,
                                                            const uint8_t laneNumber,
                                                            const adi_adrv904x_SerLaneCfg_t* const serLaneCfg);
/**
 * \brief Gets the serializer lane configuration parameters for
 * the chosen lane
 *
 * \pre This function is called after the device has been initialized
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device     - Context variable, pointer to the device settings structure
 * \param[in]     laneNumber - Selects the serializer lane (values from 0 - 7)
 * \param[out]    serLaneCfg - Pointer to the Serializer Lane settings structure adi_adrv904x_SerLaneCfg_t
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_SerLaneCfgGet(adi_adrv904x_Device_t* const device,
                                                            const uint8_t laneNumber,
                                                            adi_adrv904x_SerLaneCfg_t* const serLaneCfg);

/**
 * \brief Retrieve initial calibration status for SERDIN lane.
 * 
 * If a filename is supplied the data is also written as a single line containing a comma separated list of
 * human-readable values. If the file does not exist it is created. If it does exist it is appended to. The file is
 * flushed and closed before the function returns.
 *
 * In the case where the fuction creates the file it will first write four lines in this order:
 *   - a line indicating the file contains serdes calibration information
 *   - a line indicating the API version number
 *   - a header line indicating the meaning of the fields written by InitCalStatusGet()
 *   - a header line indicating the meaning of the fields written by TrackingCalStatusGet()
 * before then writing the initial cal data line itself. The line's first field will have the value 'ic'.
 *
 * Other than in the header line the exact format of the line written is not defined but can be analyzed by a
 * stand-alone ADI-supplied application which will recommend serdes configuration changes based on the contents.
 * 
 * It is intended that both InitCalStatusGet() and TrackingCalStatusGet() are passed the same value for filePath.
 *
 * As the initial calibration only occurs once, subsequent calls to this function will return the same data as the
 * initial call.
 * 
 * \param[in,out] device Context variable - Context variable - Structure pointer to the ADRV904X device data structure
 * \param[in] filePath May be NULL. If set is the filename to which to write the data
 * \param[in] laneNumber Selects the SERDIN lane
 * \param[in] msg May be NULL. If set this text will be included in the data written to file
 * \param[out] calStatus The serdes init cal status is written out here
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_SerdesInitCalStatusGet(adi_adrv904x_Device_t* const               device,
                                                                     const adi_adrv904x_GenericStrBuf_t* const  filePath,
                                                                     const uint8_t                              laneNumber,
                                                                     const adi_adrv904x_GenericStrBuf_t* const  msg,
                                                                     adi_adrv904x_SerdesInitCalStatus_t* const  calStatus);
                                                                  
/**
 * \brief Retrieve tracking calibration status for SERDIN lane.
 * 
 * Unless noted below the behaviour of this function is the same as that of SerdesInitCalStatusGet except this function
 * deals with serdes tracking cal status data.
 * 
 * In the case where the fuction creates the file it will write initial lines to the file in an identical manner as
 * SerdesInitCalStatusGet()before then writing the tracking cal data line itself. The line's first field will have the
 * value 'tc'.
 *
 * Unlike SerdesInitCalStatusGet tracking calibration status changes regularly so subsequent calls to this function
 * will return different data.
 * 
 * \param[in,out] device Context variable - Context variable - Structure pointer to the ADRV904X device data structure
 * \param[in] filePath May be NULL. If set is the filename to which to write the data
 * \param[in] laneNumber Selects the SERDIN lane
 * \param[in] msg May be NULL. If set this text will be included in the data written to file
 * \param[out] calStatus The serdes tracking cal status is written out here
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
 
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_SerdesTrackingCalStatusGet(adi_adrv904x_Device_t* const               device,
                                                                         const adi_adrv904x_GenericStrBuf_t* const  filePath,
                                                                         const uint8_t                              laneNumber,
                                                                         const adi_adrv904x_GenericStrBuf_t* const  msg,
                                                                         adi_adrv904x_SerdesTrackingCalStatus_t*    calStatus);


/**
 * \brief Set the CDDC Mode for the selected framer. Only framer 0 and 1 are supported.
 *
 * \pre This function is called after the device has been initialized
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device    - Context variable, pointer to the device settings structure
 * \param[in]     framerSel - selected of framer defined in adi_adrv904x_FramerSel_e
 * \param[in]     cddcMode  - cddc mode value as defined in adi_adrv904x_CddcMode_e
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CddcModeSet(adi_adrv904x_Device_t* const     device,
                                                          const adi_adrv904x_FramerSel_e   framerSel,
                                                          const adi_adrv904x_CddcMode_e    cddcMode);

/**
 * \brief Get the CDDC Mode value for the selected framer.
 *
 * \pre This function is called after the device has been initialized
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device    - Context variable, pointer to the device settings structure
 * \param[in]     framerSel - selected of framer defined in adi_adrv904x_FramerSel_e
 * \param[out]    cddcMode  - pointer to hold cddc mode value.
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CddcModeGet(adi_adrv904x_Device_t* const     device,
                                                          const adi_adrv904x_FramerSel_e   framerSel,
                                                          adi_adrv904x_CddcMode_e * const  cddcMode);

/**
 * \brief Set the CDUC Mode for the selected deframer.
 *
 * \pre This function is called after the device has been initialized
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device      - Context variable, pointer to the device settings structure
 * \param[in]     deframerSel - selected of deframer defined in adi_adrv904x_DeframerSel_e
 * \param[in]     cducMode    - cduc mode value as defined in adi_adrv904x_CducMode_e
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CducModeSet(adi_adrv904x_Device_t* const       device,
                                                          const adi_adrv904x_DeframerSel_e   deframerSel,
                                                          const adi_adrv904x_CducMode_e      cducMode);

/**
 * \brief Get the CDUC Mode value for the selected deframer.
 *
 * \pre This function is called after the device has been initialized
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device      - Context variable, pointer to the device settings structure
 * \param[in]     deframerSel - selected of deframer defined in adi_adrv904x_DeframerSel_e
 * \param[out]    cducMode    - pointer to hold cduc mode value.
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CducModeGet(adi_adrv904x_Device_t* const       device,
                                                          const adi_adrv904x_DeframerSel_e   deframerSel,
                                                          adi_adrv904x_CducMode_e * const    cducMode);

/**
 * \brief Set the CDDC Sample Crossbar for the selected framer and RX channel. Only framer 0 and 1 are supported.
 *
 * \pre This function is called after the device has been initialized
 *  For the entries in the structure adi_adrv904x_CddcSampleXbarCfg_t, the valid value is from 0..31.
 *  Valid value will enable the entry and value 0x80 (128) will mark the entry unused and disable the entry.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device    - Context variable, pointer to the device settings structure
 * \param[in]     framerSel - selected of framer defined in adi_adrv904x_FramerSel_e
 * \param[in]     rxChanSel  - selected RX channel defined in adi_adrv904x_RxChannels_e
 * \param[in]     cddcSampleXbarCfg - pointer to a structure contains CDDC Sample XBar values
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CddcSampleXBarSet(adi_adrv904x_Device_t* const                   device,
                                                                const adi_adrv904x_FramerSel_e                 framerSel,
                                                                const adi_adrv904x_RxChannels_e                rxChanSel,
                                                                const adi_adrv904x_CddcSampleXbarCfg_t * const cddcSampleXbarCfg);

/**
 * \brief Get the CDDC Sample Crossbar for the selected framer and RX channel. Only framer 0 and 1 are supported.
 *  For the entries in the structure adi_adrv904x_CddcSampleXbarCfg_t, the valid value is from 0..31.
 *  Value 0x80 (128) indicates the entry unused and disabled.
 *
 * \pre This function is called after the device has been initialized
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device    - Context variable, pointer to the device settings structure
 * \param[in]     framerSel - selected of framer defined in adi_adrv904x_FramerSel_e
 * \param[in]     rxChanSel  - selected RX channel defined in adi_adrv904x_RxChannels_e
 * \param[out]    cddcSampleXbarCfg - pointer to a structure to contain the return of CDDC Sample XBar values
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CddcSampleXBarGet(adi_adrv904x_Device_t* const             device,
                                                                const adi_adrv904x_FramerSel_e           framerSel,
                                                                const adi_adrv904x_RxChannels_e          rxChanSel,
                                                                adi_adrv904x_CddcSampleXbarCfg_t * const cddcSampleXbarCfg);

/**
 * \brief Set the CDUC Sample Crossbar for the selected deframer and TX channel.
 *
 * \pre This function is called after the device has been initialized
 *  For the entries in the structure adi_adrv904x_CducSampleXbarCfg_t, the valid value is from 0..31.
 *  Valid value will enable the entry and value 0x80 (128) will mark the entry unused and disable the entry.
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device    - Context variable, pointer to the device settings structure
 * \param[in]     deframerSel - selected of deframer defined in adi_adrv904x_DeframerSel_e
 * \param[in]     txChanSel  - selected TX channel defined in adi_adrv904x_TxChannels_e
 * \param[in]     cducSampleXbarCfg - pointer to a structure contains CDUC Sample XBar values.
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CducSampleXBarSet(adi_adrv904x_Device_t* const                   device,
                                                                const adi_adrv904x_DeframerSel_e               deframerSel,
                                                                const adi_adrv904x_TxChannels_e                txChanSel,
                                                                const adi_adrv904x_CducSampleXbarCfg_t * const cducSampleXbarCfg);

/**
 * \brief Get the CDUC Sample Crossbar for the selected deframer and TX channel.
 *  For the entries in the structure adi_adrv904x_CducSampleXbarCfg_t, the valid value is from 0..31.
 *  Value 0x80 (128) indicates the entry unused and disabled.
 *
 * \pre This function is called after the device has been initialized
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device    - Context variable, pointer to the device settings structure
 * \param[in]     deframerSel - selected of deframer defined in adi_adrv904x_DeframerSel_e
 * \param[in]     txChanSel  - selected RX channel defined in adi_adrv904x_TxChannels_e
 * \param[out]    cducSampleXbarCfg - pointer to a structure to contain the return of CDUC Sample XBar values
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CducSampleXBarGet(adi_adrv904x_Device_t* const             device,
                                                                const adi_adrv904x_DeframerSel_e         deframerSel,
                                                                const adi_adrv904x_TxChannels_e          txChanSel,
                                                                adi_adrv904x_CducSampleXbarCfg_t * const cducSampleXbarCfg);

/**
 * \brief Set the CDDC RX Channel Crossbar for the selected framer. Only framer 0 and 1 are supported.
 *  For the entries in the structure adi_adrv904x_CddcChannelXbarCfg_t, the valid value is from 0..7.
 *  Valid value will enable the entry and value 0x80 (128) will mark the entry unused and disable the entry.
 *
 * \pre This function is called after the device has been initialized
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device    - Context variable, pointer to the device settings structure
 * \param[in]     framerSel - selected of framer defined in adi_adrv904x_FramerSel_e
 * \param[in]     cddcChannelXbarCfg - pointer to a structure contains RX Channel XBar values
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CddcRxChanXBarSet(adi_adrv904x_Device_t* const                    device,
                                                                const adi_adrv904x_FramerSel_e                  framerSel,
                                                                const adi_adrv904x_CddcChannelXbarCfg_t * const cddcChannelXbarCfg);

/**
 * \brief Get the CDDC RX Channel Crossbar values for the selected framer. Only framer 0 and 1 are supported.
 *  For the entries in the structure adi_adrv904x_CddcChannelXbarCfg_t, the valid value is from 0..7.
 *  Value 0x80 (128) indicates the entry unused and disabled.
 *
 * \pre This function is called after the device has been initialized
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device    - Context variable, pointer to the device settings structure
 * \param[in]     framerSel - selected of framer defined in adi_adrv904x_FramerSel_e
 * \param[out]    cddcChannelXbarCfg - pointer to a structure to contain the return of CDDC RX Channel XBar values
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CddcRxChanXBarGet(adi_adrv904x_Device_t* const              device,
                                                                const adi_adrv904x_FramerSel_e            framerSel,
                                                                adi_adrv904x_CddcChannelXbarCfg_t * const cddcChannelXbarCfg);

/**
 * \brief Set the CDUC TX Channel Crossbar for the selected deframer.
 *
 * \pre This function is called after the device has been initialized
 *  For the entries in the structure adi_adrv904x_CducChannelXbarCfg_t, the valid value is from 0..7.
 *  Valid value will enable the entry and value 0x80 (128) will mark the entry unused and disable the entry.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device    - Context variable, pointer to the device settings structure
 * \param[in]     deframerSel - selected of deframer defined in adi_adrv904x_DeframerSel_e
 * \param[in]     cducChannelXbarCfg - pointer to a structure contains CDUC TX Channel XBar values
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CducTxChanXBarSet(adi_adrv904x_Device_t* const                     device,
                                                                const adi_adrv904x_DeframerSel_e                 deframerSel,
                                                                const adi_adrv904x_CducChannelXbarCfg_t  * const cducChannelXbarCfg);

/**
 * \brief Get the CDUC TX Channel Crossbar  values for the selected deframer.
 *
 * \pre This function is called after the device has been initialized
 *  For the entries in the structure adi_adrv904x_CducChannelXbarCfg_t, the valid value is from 0..7.
 *  Value 0x80 (128) indicates the entry unused and disabled.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device    - Context variable, pointer to the device settings structure
 * \param[in]     deframerSel - selected of deframer defined in adi_adrv904x_DeframerSel_e
 * \param[out]    cducChannelXbarCfg - pointer to a structure to contain the return of CDUC TX Channel XBar values
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CducTxChanXBarGet(adi_adrv904x_Device_t* const               device,
                                                                const adi_adrv904x_DeframerSel_e           deframerSel,
                                                                adi_adrv904x_CducChannelXbarCfg_t  * const cducChannelXbarCfg);

/**
 * \brief  Sets the ORX ADC sample crossbar for the specified framer-2
 *  It does not support framer-0 and framer-1.  For framer-1 and Framer-2 must use adi_adrv904x_AdcSampleXbarSet
 *
 * \pre This function is called after JESD204B/C initialization
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable -Pointer to the device settings structure
 * \param[in] framerSel selected of framer defined in adi_adrv904x_FramerSel_e
 * \param[in] orxAdcXbar Pointer to the JESD structure adi_adrv904x_OrxAdcSampleXbarCfg_t
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_OrxAdcSampleXbarSet(adi_adrv904x_Device_t* const device,
                                                                  const adi_adrv904x_FramerSel_e  framerSel,
                                                                  const adi_adrv904x_OrxAdcSampleXbarCfg_t* const orxAdcXbar);

/**
 * \brief Gets the ORX ADC sample crossbar converter configuration map for framer-2
 *  It does not support framer-0 and framer-1. For framer-1 and Framer-2 must use adi_adrv904x_AdcSampleXbarGet
 *
 * \pre This function is called after JESD204B/C initialization
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable -Pointer to the device settings structure
 * \param[in] framerSel selected of framer defined in adi_adrv904x_FramerSel_e
 * \param[out] orxAdcXbar Pointer to the JESD structure adi_adrv904x_OrxAdcSampleXbarCfg_t
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_OrxAdcSampleXbarGet(adi_adrv904x_Device_t* const device,
                                                                  const adi_adrv904x_FramerSel_e  framerSel,
                                                                  adi_adrv904x_OrxAdcSampleXbarCfg_t* const orxAdcXbar);
#endif  /* _ADI_ADRV904X_DATAINTERFACE_H_ */
