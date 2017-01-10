/**
 *\file mykonos.c
 *
 *\brief Contains Mykonos APIs for transceiver configuration and control
 *
 * Mykonos API version: 1.3.1.3534
 */

/**
 * \mainpage Overview
 *
 * This document is intended for use by software engineering professionals and
 * includes detailed information regarding the data types and function calls
 * which comprise the Mykonos ANSI C API.
 * References to "Mykonos" in the API refer to the Analog Devices development name for the AD9371 family of devices.
 *
 */

/**
 * \page Disclaimer Legal Disclaimer
 * WARRANTY DISCLAIMER: THE SOFTWARE AND ANY RELATED INFORMATION AND/OR ADVICE IS PROVIDED ON AN
 * “AS IS” BASIS, WITHOUT REPRESENTATIONS, GUARANTEES OR WARRANTIES OF ANY KIND, EXPRESS OR IMPLIED,
 * ORAL OR WRITTEN, INCLUDING WITHOUT LIMITATION, WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT.
 */

/**
 * \page Use Suggested Use
 * The purpose of this API library is to add abstraction for the low level
 * SPI control and calculations necessary to control the Mykonos family of transceiver devices
 *
 * Add the included source code as required to your baseband processor software build. Please reference the integration document
 * as well for further instructions.
 */

#include <stdint.h>
#include <stddef.h>
#include "common.h"
#include "mykonos.h"
#include "mykonos_gpio.h"
#include "mykonos_macros.h"
#include "mykonos_user.h"

/* Private helper functions local to this file */
static mykonosErr_t MYKONOS_calculateScaledDeviceClk_kHz(mykonosDevice_t *device, uint32_t *scaledRefClk_kHz, uint8_t *deviceClkDiv);
static mykonosErr_t MYKONOS_calculateDigitalClocks(mykonosDevice_t *device, uint32_t *hsDigClk_kHz, uint32_t *hsDigClkDiv4or5_kHz);
static mykonosErr_t enableDpdTracking(mykonosDevice_t *device, uint8_t tx1Enable, uint8_t tx2Enable);
static mykonosErr_t enableClgcTracking(mykonosDevice_t *device, uint8_t tx1Enable, uint8_t tx2Enable);

/**
 * \brief Verifies the Tx profile members are valid (in range) in the init structure
 *
 * If the Tx profile IQ data rate = 0, it is assumed that the Tx profile is
 * not used.  If Tx IQ data rate > 0, and Tx profile members are out of range,
 *
 * \pre This function is private and is not called directly by the user.
 *
 * <B>Dependencies</B>
 * - device->tx->txProfile
 *
 * \param device Structure pointer to Mykonos device data structure
 * \param txProfile txProfile settings to be verified
 * \param txHsDigClk_kHz Return value of the calculated HS Dig Clock required by the Tx profile
 *
 * \retval MYKONOS_ERR_TXPROFILE_IQRATE Profile IQ rate out of range
 * \retval MYKONOS_ERR_TXPROFILE_RFBW Tx Profile RF bandwidth out of range
 * \retval MYKONOS_ERR_TXPROFILE_FILTER_INTERPOLATION Filter interpolation not valid
 * \retval MYKONOS_ERR_TXPROFILE_FIR_INT FIR filter not valid
 * \retval MYKONOS_ERR_OK All profile members are valid
 */
static mykonosErr_t mykVerifyTxProfile(mykonosDevice_t *device, mykonosTxProfile_t *txProfile, uint32_t *txHsDigClk_kHz)
{
    mykonosErr_t retVal = MYKONOS_ERR_OK;

    *txHsDigClk_kHz = 0;

    /********************************/
    /* Check for a valid Tx profile */
    /********************************/

    if ((txProfile->iqRate_kHz < MIN_TX_IQRATE_KHZ) || (txProfile->iqRate_kHz > MAX_TX_IQRATE_KHZ))
    {
        return MYKONOS_ERR_TXPROFILE_IQRATE;
    }

    if ((txProfile->rfBandwidth_Hz < MIN_TX_RFBW_HZ) || (txProfile->rfBandwidth_Hz > MAX_TX_RFBW_HZ))
    {
        return MYKONOS_ERR_TXPROFILE_RFBW;
    }

    if ((txProfile->thb1Interpolation != 1) && (txProfile->thb1Interpolation != 2))
    {
        return MYKONOS_ERR_TXPROFILE_FILTER_INTERPOLATION;
    }

    if ((txProfile->thb2Interpolation != 1) && (txProfile->thb2Interpolation != 2))
    {
        return MYKONOS_ERR_TXPROFILE_FILTER_INTERPOLATION;
    }

    if ((txProfile->txFirInterpolation != 1) && (txProfile->txFirInterpolation != 2) && (txProfile->txFirInterpolation != 4))
    {
        return MYKONOS_ERR_TXPROFILE_FILTER_INTERPOLATION;
    }

    if ((txProfile->txFir->coefs == NULL) && (txProfile->txFirInterpolation != 1))
    {
        return MYKONOS_ERR_TXPROFILE_FIR_COEFS;
    }

    if ((txProfile->dacDiv != DACDIV_2) && (txProfile->dacDiv != DACDIV_2p5) && (txProfile->dacDiv != DACDIV_4))
    {
        return MYKONOS_ERR_TXPROFILE_DACDIV;
    }

    *txHsDigClk_kHz = (txProfile->iqRate_kHz * txProfile->txFirInterpolation * txProfile->thb1Interpolation * txProfile->thb2Interpolation * txProfile->dacDiv);

    device->profilesValid |= TX_PROFILE_VALID;

    return retVal;
}

/**
 * \brief Verifies the Rx profile members are valid (in range) and calculates HS Dig Clock require for the Rx Profile
 *
 * Private helper function to verify the Rx profile members are valid (in range)
 * and calculates HS Dig Clock require for the Rx Profile
 * If the Rx profile IQ data rate = 0, it is assumed that the Rx profile is
 * not used.  If Rx IQ data rate > 0, and Rx profile members are out of range.
 *
 * \pre This function is private and is not called directly by the user.
 *
 * <B>Dependencies</B>
 * - device->rx->rxProfile
 *
 * \param device Structure pointer to Talise device data structure
 * \param rxChannel receiver channel to be checked
 * \param rxProfile rxProfile settings to be verified
 * \param rxHsDigClk_kHz Return value of the calculated HS Dig Clock required by the Rx profile
 *
 * \retval MYKONOS_ERR_RXPROFILE_RXCHANNEL Rx channel is not valid.
 * \retval MYKONOS_ERR_RXPROFILE_IQRATE out of range IQ rate
 * \retval MYKONOS_ERR_RXPROFILE_RFBW out of range RF bandwidth
 * \retval MYKONOS_ERR_RXPROFILE_FILTER_DECIMATION not valid filter decimation setting
 * \retval MYKONOS_ERR_RXPROFILE_FIR_COEFS FIR filter not valid
 * \retval MYKONOS_ERR_RXPROFILE_ADCDIV not valid ADC divider
 * \retval MYKONOS_ERR_OK all profile members are valid
 */
static mykonosErr_t mykVerifyRxProfile(mykonosDevice_t *device, mykonosRxProfType_t rxChannel, mykonosRxProfile_t *rxProfile, uint32_t *rxHsDigClk_kHz)
{
    mykonosErr_t retVal = MYKONOS_ERR_OK;
    uint32_t minBwHz = 0;
    uint32_t maxBwHz = 0;

    *rxHsDigClk_kHz = 0;

    switch (rxChannel)
    {
        case MYK_RX_PROFILE:
            minBwHz = MIN_RX_RFBW_HZ;
            maxBwHz = MAX_RX_RFBW_HZ;
            break;
        case MYK_OBS_PROFILE:
            minBwHz = MIN_ORX_RFBW_HZ;
            maxBwHz = MAX_ORX_RFBW_HZ;
            break;
        case MYK_SNIFFER_PROFILE:
            minBwHz = MIN_SNIFFER_RFBW_HZ;
            maxBwHz = MAX_SNIFFER_RFBW_HZ;
            break;

        default:
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_RXPROFILE_RXCHANNEL,
                    getMykonosErrorMessage(MYKONOS_ERR_RXPROFILE_RXCHANNEL));
            return MYKONOS_ERR_RXPROFILE_RXCHANNEL;
    }

    /********************************/
    /* Check for a valid Rx profile */
    /********************************/
    if ((rxProfile->iqRate_kHz < MIN_RX_IQRATE_KHZ) || (rxProfile->iqRate_kHz > MAX_RX_IQRATE_KHZ))
    {
        return MYKONOS_ERR_RXPROFILE_IQRATE;
    }

    /* check for Rx/Obs or Sniffer BW */
    if ((rxProfile->rfBandwidth_Hz < minBwHz) || (rxProfile->rfBandwidth_Hz > maxBwHz))
    {
        return MYKONOS_ERR_RXPROFILE_RFBW;
    }

    if ((rxProfile->rhb1Decimation != 1) && (rxProfile->rhb1Decimation != 2))
    {
        return MYKONOS_ERR_RXPROFILE_FILTER_DECIMATION;
    }

    if ((rxProfile->rxDec5Decimation != 4) && (rxProfile->rxDec5Decimation != 5))
    {
        return MYKONOS_ERR_RXPROFILE_FILTER_DECIMATION;
    }

    if ((rxProfile->rxFirDecimation != 1) && (rxProfile->rxFirDecimation != 2) && (rxProfile->rxFirDecimation != 4))
    {
        return MYKONOS_ERR_RXPROFILE_FILTER_DECIMATION;
    }

    if ((rxProfile->rxFir->coefs == NULL) && (rxProfile->rxFirDecimation != 1))
    {
        return MYKONOS_ERR_RXPROFILE_FIR_COEFS;
    }

    if ((rxProfile->adcDiv != 1) && (rxProfile->adcDiv != 2))
    {
        return MYKONOS_ERR_RXPROFILE_ADCDIV;
    }

    *rxHsDigClk_kHz = (rxProfile->iqRate_kHz * rxProfile->rxFirDecimation * rxProfile->rhb1Decimation * rxProfile->rxDec5Decimation * rxProfile->adcDiv);

    switch (rxChannel)
    {
        case MYK_RX_PROFILE:
            device->profilesValid |= RX_PROFILE_VALID;
            break;
        case MYK_OBS_PROFILE:
            device->profilesValid |= ORX_PROFILE_VALID;
            break;
        case MYK_SNIFFER_PROFILE:
            device->profilesValid |= SNIFF_PROFILE_VALID;
            break;
    }

    return retVal;
}

/**
 * \brief Performs a hard reset on the MYKONOS DUT (Toggles RESETB pin on device)
 *
 * Toggles the Mykonos devices RESETB pin.  Only resets the device with
 * the SPI chip select indicated in the device->spiSettings structure.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device Pointer to Mykonos device data structure containing settings
 *
 * \return Returns enum mykonosErr_t, MYKONOS_ERR_OK=pass, !MYKONOS_ERR_OK=fail
 */
mykonosErr_t MYKONOS_resetDevice(mykonosDevice_t *device)
{

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_resetDevice()\n");
#endif

    /* toggle RESETB on device with matching spi chip select index */
    CMB_hardReset(device->spiSettings->chipSelectIndex);
    return MYKONOS_ERR_OK;
}

/**
 * \brief Reads back the silicon revision for the Mykonos Device
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device Pointer to Mykonos device data structure containing settings
 * \param revision Return value of the Mykonos silicon revision
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 */
mykonosErr_t MYKONOS_getDeviceRev(mykonosDevice_t *device, uint8_t *revision)
{
#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_getDeviceRev()\n");
#endif

    CMB_SPIReadField(device->spiSettings, MYKONOS_ADDR_PRODUCT_ID, revision, 0x07, 0);

    return MYKONOS_ERR_OK;
}

/**
 * \brief Reads back the Product ID for the Mykonos Device
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device Pointer to Mykonos device data structure containing settings
 * \param productId Return value of the Mykonos product Id
 *
 * \retval MYKONOS_ERR_GETPRODUCTID__NULL_PARAM recovery action for bad parameter check
 * \retval MYKONOS_ERR_OK Function completed successfully
 */
mykonosErr_t MYKONOS_getProductId(mykonosDevice_t *device, uint8_t *productId)
{
    mykonosErr_t retVal = MYKONOS_ERR_OK;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_getProductId()\n");
#endif

    if (productId == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETPRODUCTID_NULL_PARAM,
                getMykonosErrorMessage(MYKONOS_ERR_GETPRODUCTID_NULL_PARAM));
        return MYKONOS_ERR_GETPRODUCTID_NULL_PARAM;
    }

    CMB_SPIReadField(device->spiSettings, MYKONOS_ADDR_PRODUCT_ID, productId, 0xF8, 3);

    return retVal;
}

/**
 * \brief Get API version number
 *
 * This function reads back the version number of the API
 *
 * \param device Pointer to the Mykonos data structure
 * \param siVer A pointer to the current silicon version number.
 * \param majorVer A pointer to the current major version number.
 * \param minorVer A pointer to the current minor version number.
 * \param buildVer A pointer to the current build version number.
 *
 * \retval MYKONOS_ERR_GET_API_VERSION_NULL_PARAM Null parameter passed to the function.
 * \retval MYKONOS_ERR_OK Function completed successfully
 */
mykonosErr_t MYKONOS_getApiVersion(mykonosDevice_t *device, uint32_t *siVer, uint32_t *majorVer, uint32_t *minorVer, uint32_t *buildVer)
{
    mykonosErr_t retVal = MYKONOS_ERR_OK;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_getApiVersion()\n");
#endif

    if ((siVer == NULL) || (majorVer == NULL) || (minorVer == NULL) || (buildVer == NULL))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GET_API_VERSION_NULL_PARAM,
                getMykonosErrorMessage(MYKONOS_ERR_GET_API_VERSION_NULL_PARAM));
        return MYKONOS_ERR_GET_API_VERSION_NULL_PARAM;
    }

    *siVer = (uint32_t)MYKONOS_CURRENT_SI_VERSION;
    *majorVer = (uint32_t)MYKONOS_CURRENT_MAJOR_VERSION;
    *minorVer = (uint32_t)MYKONOS_CURRENT_MINOR_VERSION;
    *buildVer = (uint32_t)MYKONOS_CURRENT_BUILD_VERSION;
    return retVal;
}

/**
 * \brief Sets the Mykonos device SPI settings (3wire/4wire, MSBFirst, etc).
 *
 * This function will use the settings in the device->spiSettings structure
 * to set SPI stream mode, address auto increment direction, MSBFirst/LSBfirst,
 * and 3wire/4wire mode.  The Mykonos device always uses SPI MODE 0 (CPHA=0, CPOL=0).
 * This function will update your device->spiSettings to set CHPA=0 and CPOL=0 and
 * longInstructionWord =1 to use a 16bit instruction word.
 *
 * <B>Dependencies</B>
 * - writes device->spiSettings->CPHA = 0
 * - writes device->spiSettings->CPOL = 0
 * - writes device->spiSettings->longInstructionWord = 1
 *
 * - device->spiSettings->MSBFirst
 * - device->spiSettings->enSpiStreaming
 * - device->spiSettings->autoIncAddrUp
 * - device->spiSettings->fourWireMode
 *
 * \param device Pointer to Mykonos device data structure containing settings
 *
 * \return Returns enum mykonosErr_t, MYKONOS_ERR_OK=pass, !MYKONOS_ERR_OK=fail
 */
mykonosErr_t MYKONOS_setSpiSettings(mykonosDevice_t *device)
{
    //device->spiSettings->enSpiStreaming
    uint8_t spiReg = 0;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_setSpiSettings()\n");
#endif

    device->spiSettings->CPHA = 0;
    device->spiSettings->CPOL = 0;
    device->spiSettings->longInstructionWord = 1;

    if (device->spiSettings->MSBFirst == 0)
    {
        spiReg |= 0x42; /* SPI bit is 1=LSB first */
    }

    if (device->spiSettings->autoIncAddrUp > 0)
    {
        spiReg |= 0x24;
    }

    if (device->spiSettings->fourWireMode > 0)
    {
        spiReg |= 0x18;
    }

    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_CONFIGURATION_CONTROL_0, spiReg);

    if (device->spiSettings->enSpiStreaming > 0)
    {
        /* Allow SPI streaming mode: SPI message ends when chip select deasserts */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_SPI_CONFIGURATION_CONTROL_1, 0x00);
    }
    else
    {
        /* Force single instruction mode */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_SPI_CONFIGURATION_CONTROL_1, 0x80);
    }

    return MYKONOS_ERR_OK;
}

/**
 * \brief Verifies the integrity of the Mykonos device data structure.
 *
 * The Mykonos device data structure has many pointers to other structures.  The
 * main purpose of this function to verify that the necessary pointers within the
 * device data structure have non zero pointers. The focus is on the Rx/Tx/ObsRx
 * profiles and FIR filters where if a channel is disabled, it may be valid for
 * some pointers to be NULL. This function updates device->profileValid to remember
 * which profile pointers are valid.
 * profileValid[3:0] = {SnifferProfileValid, ObsRxProfileValid, RxProfileValid, TxProfileValid};
 *
 * <B>Dependencies</B>
 * - device (all variables)
 *
 * \param device Pointer to Mykonos device data structure containing settings
 *
 * \return Returns enum mykonosErr_t, MYKONOS_ERR_OK=pass, !MYKONOS_ERR_OK=fail
 */
mykonosErr_t MYKONOS_verifyDeviceDataStructure(mykonosDevice_t *device)
{
    if (device == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, 0, MYKONOS_ERR_CHECKDEVSTRUCT_NULLDEVPOINTER, getMykonosErrorMessage(MYKONOS_ERR_CHECKDEVSTRUCT_NULLDEVPOINTER));
        return MYKONOS_ERR_CHECKDEVSTRUCT_NULLDEVPOINTER;
    }

    device->profilesValid = 0; /* Reset */

    if (device->spiSettings == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, 0, MYKONOS_ERR_CHECKDEVSTRUCT_SPI, getMykonosErrorMessage(MYKONOS_ERR_CHECKDEVSTRUCT_SPI));
        return MYKONOS_ERR_CHECKDEVSTRUCT_SPI;
    }

    /* place this writeToLog after verifying the spiSettings structure is not NULL */
#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_verifyDeviceDataStructure()\n");
#endif

    /**************************************************************************
     * Check Rx stucture pointers
     **************************************************************************
     */
    if (device->rx == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_CHECKDEVSTRUCT_RX, getMykonosErrorMessage(MYKONOS_ERR_CHECKDEVSTRUCT_RX));
        return MYKONOS_ERR_CHECKDEVSTRUCT_RX;
    }
    else
    {
        if (device->rx->rxChannels == RXOFF)
        {/* If no Rx channel enabled, ok to have null pointers for rx gain control, framer, and Rx profile/Rx FIR Filter */
        }
        else
        {
            if ((device->rx->framer == NULL) || (device->rx->rxGainCtrl == NULL) || (device->rx->rxProfile == NULL))
            {
                CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_CHECKDEVSTRUCT_RXSUB,
                        getMykonosErrorMessage(MYKONOS_ERR_CHECKDEVSTRUCT_RXSUB));
                return MYKONOS_ERR_CHECKDEVSTRUCT_RXSUB;
            }

            if ((device->rx->rxProfile->rxFir == NULL) && (device->rx->rxProfile->rxFirDecimation != 1))
            {
                CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_CHECKDEVSTRUCT_RXFIR,
                        getMykonosErrorMessage(MYKONOS_ERR_CHECKDEVSTRUCT_RXFIR));
                return MYKONOS_ERR_CHECKDEVSTRUCT_RXFIR;
            }
        }
    }

    /**************************************************************************
     * Check Tx stucture pointers
     **************************************************************************
     */
    if (device->tx == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_CHECKDEVSTRUCT_TX, getMykonosErrorMessage(MYKONOS_ERR_CHECKDEVSTRUCT_TX));
        return MYKONOS_ERR_CHECKDEVSTRUCT_TX;
    }
    else
    {
        if (device->tx->txChannels == TXOFF)
        {/* If no Tx channel enabled, ok to have null pointers for deframer, and Tx profile/Tx FIR filter */
        }
        else
        {
            if ((device->tx->deframer == NULL) || (device->tx->txProfile == NULL))
            {
                CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_CHECKDEVSTRUCT_TXSUB,
                        getMykonosErrorMessage(MYKONOS_ERR_CHECKDEVSTRUCT_TXSUB));
                return MYKONOS_ERR_CHECKDEVSTRUCT_TXSUB;
            }

            if ((device->tx->txProfile->txFir == NULL) && (device->tx->txProfile->txFirInterpolation != 1))
            {
                CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_CHECKDEVSTRUCT_TXFIR,
                        getMykonosErrorMessage(MYKONOS_ERR_CHECKDEVSTRUCT_TXFIR));
                return MYKONOS_ERR_CHECKDEVSTRUCT_TXFIR;
            }
        }
    }

    /**************************************************************************
     * Check ObsRx stucture pointers
     **************************************************************************
     */
    if (device->obsRx == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_CHECKDEVSTRUCT_OBSRX,
                getMykonosErrorMessage(MYKONOS_ERR_CHECKDEVSTRUCT_OBSRX));
        return MYKONOS_ERR_CHECKDEVSTRUCT_OBSRX;
    }
    else
    {
        if (device->obsRx->obsRxChannelsEnable == MYK_OBS_RXOFF)
        {/* If no ObsRx channel enabled, ok to have null pointers for ObsRx gain control, framer, and ObsRx profile/FIR Filter */
        }
        else
        {
            if ((device->obsRx->framer == NULL))
            {
                CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_CHECKDEVSTRUCT_OBSRXFRAMER,
                        getMykonosErrorMessage(MYKONOS_ERR_CHECKDEVSTRUCT_OBSRXFRAMER));
                return MYKONOS_ERR_CHECKDEVSTRUCT_OBSRXFRAMER;
            }

            if (device->obsRx->snifferProfile != NULL)
            {
                if (device->obsRx->snifferGainCtrl == NULL)
                {
                    CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_CHECKDEVSTRUCT_SNIFFERGAINCTRL,
                            getMykonosErrorMessage(MYKONOS_ERR_CHECKDEVSTRUCT_SNIFFERGAINCTRL));
                    return MYKONOS_ERR_CHECKDEVSTRUCT_SNIFFERGAINCTRL;
                }

                if ((device->obsRx->snifferProfile->rxFir == NULL) && (device->obsRx->snifferProfile->rxFirDecimation != 1))
                {
                    CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_CHECKDEVSTRUCT_SNIFFERFIR,
                            getMykonosErrorMessage(MYKONOS_ERR_CHECKDEVSTRUCT_SNIFFERFIR));
                    return MYKONOS_ERR_CHECKDEVSTRUCT_SNIFFERFIR;
                }
            }

            if (device->obsRx->orxProfile != NULL)
            {
                if (device->obsRx->orxGainCtrl == NULL)
                {
                    CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_CHECKDEVSTRUCT_ORXGAINCTRL,
                            getMykonosErrorMessage(MYKONOS_ERR_CHECKDEVSTRUCT_ORXGAINCTRL));
                    return MYKONOS_ERR_CHECKDEVSTRUCT_ORXGAINCTRL;
                }

                if ((device->obsRx->orxProfile->rxFir == NULL) && (device->obsRx->orxProfile->rxFirDecimation != 1))
                {
                    CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_CHECKDEVSTRUCT_ORXFIR,
                            getMykonosErrorMessage(MYKONOS_ERR_CHECKDEVSTRUCT_ORXFIR));
                    return MYKONOS_ERR_CHECKDEVSTRUCT_ORXFIR;
                }
            }
        }
    }

    return MYKONOS_ERR_OK;
}

/**
 * \brief Initializes the Mykonos device based on the desired device settings.
 *
 * This function initializes the mykonos device, setting up the CLKPLL, digital clocks,
 * JESD204b settings, FIR Filters, digital filtering.  It does not load the ARM
 * or perform any of the ARM init calibrations. It also sets the Rx Manual gain indexes and
 * TxAttenuation settings to the initial values found in the device data structure.  It leaves the
 * Mykonos in a state ready for multichip sync (which can bring up the JESD204 links), the
 * ARM to be loaded, and the init calibartions run.
 *
 * <B>Dependencies</B>
 * - device (all variables)
 *
 * \param device Pointer to Mykonos device data structure containing settings
 *
 * \return Returns enum mykonosErr_t, MYKONOS_ERR_OK=pass, !MYKONOS_ERR_OK=fail
 */
mykonosErr_t MYKONOS_initialize(mykonosDevice_t *device)
{
    uint8_t txChannelSettings = 0;
    uint8_t rxChannelSettings = 0;
    uint8_t orxChannelSettings = 0;
    uint8_t sniffChannelSettings = 0;
    uint8_t adcDacClockRateSelect = 0;
    uint8_t enableDpd = 0;
    uint8_t obsRxRealIf = 0;
    uint8_t enRxHighRejDec5 = 0;
    uint8_t rxRealIfData = 0;
    uint8_t txSyncb = 0x00;
    uint8_t rxSyncb = 0x00;
    uint8_t orxSyncb = 0x00;
    mykonosErr_t retVal = MYKONOS_ERR_OK;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_initialize()\n");
#endif

    retVal = MYKONOS_verifyDeviceDataStructure(device);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    /* Verify Rx/Tx and ObsRx profiles are valid combinations */
    retVal = MYKONOS_verifyProfiles(device);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    /* Set 3 or 4-wire SPI mode, MSBFirst/LSBfirst in device, pushes CPOL=0, CPHA=0, longInstWord=1 into device->spiSettings */
    retVal = MYKONOS_setSpiSettings(device);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    /* Increase SPI_DO drive strength */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_DIGITAL_IO_CONTROL, 0x10);

    /* Enable Reference clock - Set REFCLK pad common mode voltage */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_REF_PAD_CONFIG2, 0x07);

    /* Set Mykonos IO pin settings, GPIO direction, enable relevant LVDS pads */
    /* Enable SYSREF LVDS input pad + 100ohm internal termination */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_SYSREF_PAD_CONFIG, 0x12); //Enable SYSREF input buffer

    if (device->tx > 0 && device->tx->txProfile > 0)
    {
        /* Check for LVDS/CMOS mode */
        if (device->tx->deframer->txSyncbMode > 0)
        {
            txSyncb = 0xF0;
        }
        else
        {
            txSyncb = 0x02;
        }

        /* Enable SYNCOUTB output buffer */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_TX1_SYNC_PAD_CONFIG, txSyncb);
    }

    /* Look at each framer and enable the RXSYNCB used for each framer.
     * It is possible that they use the same RXSYNCB pin */
    if (device->rx > 0 && device->rx->framer > 0)
    {
        /* Check for LVDS/CMOS mode */
        if (device->rx->framer->rxSyncbMode > 0)
        {
            rxSyncb = 0x00;
        }
        else
        {
            rxSyncb = 0x12;
        }

        if (device->rx->framer->obsRxSyncbSelect == 0)
        {
            /* Enable SYNCINB0 LVDS/CMOS input buffer */
            CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_RX1_SYNC_CONFIG, rxSyncb);
        }
        else
        {
            /* Enable SYNCINB1 LVDS/CMOS input buffer */
            CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_RX2_SYNC_CONFIG, rxSyncb);
        }
    }
    if (device->obsRx > 0 && device->obsRx->framer > 0)
    {
        /* Check for LVDS/CMOS mode */
        if (device->obsRx->framer->rxSyncbMode > 0)
        {
            orxSyncb = 0x00;
        }
        else
        {
            orxSyncb = 0x12;
        }

        if (device->obsRx->framer->obsRxSyncbSelect == 0)
        {
            /* Check for rxSyncb and orxSyncb when using the same RXSYNCB */
            if ((orxSyncb != rxSyncb) && (device->rx->framer->obsRxSyncbSelect == 0))
            {
                CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INIT_INV_RXSYNCB_ORXSYNCB_MODE,
                        getMykonosErrorMessage(MYKONOS_ERR_INIT_INV_RXSYNCB_ORXSYNCB_MODE));
                return MYKONOS_ERR_INIT_INV_RXSYNCB_ORXSYNCB_MODE;
            }

            /* Enable SYNCINB0 LVDS input buffer */
            CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_RX1_SYNC_CONFIG, orxSyncb);
        }
        else
        {
            /* Check for rxSyncb and orxSyncb when using the same ORXSYNCB */
            if ((orxSyncb != rxSyncb) && (device->rx->framer->obsRxSyncbSelect == 1))
            {
                CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INIT_INV_RXSYNCB_ORXSYNCB_MODE,
                        getMykonosErrorMessage(MYKONOS_ERR_INIT_INV_RXSYNCB_ORXSYNCB_MODE));
                return MYKONOS_ERR_INIT_INV_RXSYNCB_ORXSYNCB_MODE;
            }

            /* Enable SYNCINB1 LVDS input buffer */
            CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_RX2_SYNC_CONFIG, orxSyncb);
        }
    }

    /* Set number of device clock cycles per microsecond [round(freq/2) - 1] */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_REFERENCE_CLOCK_CYCLES, (uint8_t)((((device->clocks->deviceClock_kHz / 1000) + 1) >> 1) - 1));

    /* Set profile specific digital clock dividers */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_CONFIGURATION_CONTROL_1, 0x04); /* Negate Rx2 so phase matches Rx1 */

    if (device->profilesValid & TX_PROFILE_VALID)
    {
        txChannelSettings = ((device->tx->txChannels & 0x3) << 6);
        switch (device->tx->txProfile->thb2Interpolation)
        {
            case 1:
                break; /* keep bit as 0 in bitfield */
            case 2:
                txChannelSettings |= 0x20;
                break;
            default:
                CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INIT_INV_TXHB2_INTERPOLATION,
                        getMykonosErrorMessage(MYKONOS_ERR_INIT_INV_TXHB2_INTERPOLATION));
                return MYKONOS_ERR_INIT_INV_TXHB2_INTERPOLATION;
        }

        switch (device->tx->txProfile->thb1Interpolation)
        {
            case 1:
                break; /* keep bit as 0 in bitfield */
            case 2:
                txChannelSettings |= 0x10;
                break;
            default:
                CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INIT_INV_TXHB1_INTERPOLATION,
                        getMykonosErrorMessage(MYKONOS_ERR_INIT_INV_TXHB1_INTERPOLATION));
                return MYKONOS_ERR_INIT_INV_TXHB1_INTERPOLATION;
        }

        if (device->tx->txProfile->txFir == 0)
        {
            /* If invalid pointer to FIR filter, Bypass programmable FIR, keep txChannelSettings[1:0] == 0 */
        }
        else
        {
            switch (device->tx->txProfile->txFirInterpolation)
            {
                case 1:
                    txChannelSettings |= 0x01;
                    break;
                case 2:
                    txChannelSettings |= 0x02;
                    break;
                case 4:
                    txChannelSettings |= 0x03;
                    break;
                default:
                    CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INIT_INV_TXFIR_INTERPOLATION,
                            getMykonosErrorMessage(MYKONOS_ERR_INIT_INV_TXFIR_INTERPOLATION));
                    return MYKONOS_ERR_INIT_INV_TXFIR_INTERPOLATION;
            }
        }

        switch (device->tx->txProfile->dacDiv)
        {
            case DACDIV_2:
                break; /* Keep bit [1]=0 */
            case DACDIV_2p5:
                break; /* Keep bit [1]=0, div 2.5 is set when Rx DEC5 filter enabled*/
            case DACDIV_4:
                adcDacClockRateSelect |= 2;
                break;
            default:
                CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INIT_INV_DACDIV, getMykonosErrorMessage(MYKONOS_ERR_INIT_INV_DACDIV));
                return MYKONOS_ERR_INIT_INV_DACDIV;
        }
    }

    if (device->profilesValid & RX_PROFILE_VALID)
    {
        rxChannelSettings = ((device->rx->rxChannels & 0x3) << 6);
        switch (device->rx->rxProfile->rxDec5Decimation)
        {
            case 4:
                break; /* keep bit as 0 in bitfield */
            case 5:
                rxChannelSettings |= 0x04;
                break;
            default:
                CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INIT_INV_RXDEC5_DECIMATION,
                        getMykonosErrorMessage(MYKONOS_ERR_INIT_INV_RXDEC5_DECIMATION));
                return MYKONOS_ERR_INIT_INV_RXDEC5_DECIMATION;
        }

        switch (device->rx->rxProfile->rhb1Decimation)
        {
            case 1:
                break; /* keep bit as 0 in bitfield */
            case 2:
                rxChannelSettings |= 0x10;
                break;
            default:
                CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INIT_INV_RXHB1_DECIMATION,
                        getMykonosErrorMessage(MYKONOS_ERR_INIT_INV_RXHB1_DECIMATION));
                return MYKONOS_ERR_INIT_INV_RXHB1_DECIMATION;
        }

        if (device->rx->rxProfile->rxFir == 0)
        {
            /* If invalid pointer to FIR filter, Bypass programmable FIR, keep rxChannelSettings[1:0] == 0 */
        }
        else
        {
            switch (device->rx->rxProfile->rxFirDecimation)
            {
                case 1:
                    rxChannelSettings |= 0x01;
                    break;
                case 2:
                    rxChannelSettings |= 0x02;
                    break;
                case 4:
                    rxChannelSettings |= 0x03;
                    break;
                default:
                    CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INIT_INV_RXFIR_DECIMATION,
                            getMykonosErrorMessage(MYKONOS_ERR_INIT_INV_RXFIR_DECIMATION));
                    return MYKONOS_ERR_INIT_INV_RXFIR_DECIMATION;
            }
        }

        switch (device->rx->rxProfile->adcDiv)
        {
            case 1:
                adcDacClockRateSelect |= 0;
                break;
            case 2:
                adcDacClockRateSelect |= 1;
                break;
            default:
                CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INIT_INV_ADCDIV, getMykonosErrorMessage(MYKONOS_ERR_INIT_INV_ADCDIV));
                return MYKONOS_ERR_INIT_INV_ADCDIV;
        }

        rxRealIfData = (device->rx->realIfData > 0) ? 1 : 0;
        enRxHighRejDec5 = (device->rx->rxProfile->enHighRejDec5 > 0) ? 1 : 0;
    }
    else if (device->profilesValid & ORX_PROFILE_VALID)
    {
        /* if Rx profile not valid, but ORX profile valid, set RX ADC divider and dec5 to match orx profile setting */
        /* ARM clock is derived from Rx clocking, so Rx ADC div needs to be set */
        switch (device->obsRx->orxProfile->rxDec5Decimation)
        {
            case 4:
                break; /* keep bit as 0 in bitfield */
            case 5:
                rxChannelSettings |= 0x04;
                break;
            default:
                CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INIT_INV_RXDEC5_DECIMATION,
                        getMykonosErrorMessage(MYKONOS_ERR_INIT_INV_RXDEC5_DECIMATION));
                return MYKONOS_ERR_INIT_INV_RXDEC5_DECIMATION;
        }

        switch (device->obsRx->orxProfile->adcDiv)
        {
            case 1:
                adcDacClockRateSelect |= 0;
                break;
            case 2:
                adcDacClockRateSelect |= 1;
                break;
            default:
                CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INIT_INV_ADCDIV, getMykonosErrorMessage(MYKONOS_ERR_INIT_INV_ADCDIV));
                return MYKONOS_ERR_INIT_INV_ADCDIV;
        }
    }
    else if (device->profilesValid & SNIFF_PROFILE_VALID)
    {
        /* if Rx profile not valid, and ORX profile not valid, set RX ADC divider and dec5 to match sniffer profile setting */
        /* ARM clock is derived from Rx clocking, so Rx ADC div needs to be set */
        switch (device->obsRx->snifferProfile->rxDec5Decimation)
        {
            case 4:
                break; /* keep bit as 0 in bitfield */
            case 5:
                rxChannelSettings |= 0x04;
                break;
            default:
                CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INIT_INV_RXDEC5_DECIMATION,
                        getMykonosErrorMessage(MYKONOS_ERR_INIT_INV_RXDEC5_DECIMATION));
                return MYKONOS_ERR_INIT_INV_RXDEC5_DECIMATION;
        }

        switch (device->obsRx->snifferProfile->adcDiv)
        {
            case 1:
                adcDacClockRateSelect |= 0;
                break;
            case 2:
                adcDacClockRateSelect |= 1;
                break;
            default:
                CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INIT_INV_ADCDIV, getMykonosErrorMessage(MYKONOS_ERR_INIT_INV_ADCDIV));
                return MYKONOS_ERR_INIT_INV_ADCDIV;
        }
    }

    /* determine ObsRx ADC Div setting */
    if (device->profilesValid & ORX_PROFILE_VALID)
    {
        switch (device->obsRx->orxProfile->adcDiv)
        {
            case 1:
                break; /* Keep bit[4]=0 */
            case 2:
                adcDacClockRateSelect |= 0x10;
                break; /* Set bit[4]=1 */
            default:
                CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INIT_INV_OBSRX_ADCDIV,
                        getMykonosErrorMessage(MYKONOS_ERR_INIT_INV_OBSRX_ADCDIV));
                return MYKONOS_ERR_INIT_INV_OBSRX_ADCDIV;
        }
    }
    else if (device->profilesValid & SNIFF_PROFILE_VALID)
    {
        switch (device->obsRx->snifferProfile->adcDiv)
        {
            case 1:
                break; /* Keep bit[4]=0 */
            case 2:
                adcDacClockRateSelect |= 0x10;
                break; /* Set bit[4]=1 */
            default:
                CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INIT_INV_OBSRX_ADCDIV,
                        getMykonosErrorMessage(MYKONOS_ERR_INIT_INV_OBSRX_ADCDIV));
                return MYKONOS_ERR_INIT_INV_OBSRX_ADCDIV;
        }
    }
    else if (device->profilesValid & RX_PROFILE_VALID)
    { /* if OBSRX profiles are not valid, set obsRx ADC div to match Rx ADC divider */
        switch (device->rx->rxProfile->adcDiv)
        {
            case 1:
                break; /* Keep bit[4]=0 */
            case 2:
                adcDacClockRateSelect |= 0x10;
                break; /* Set bit[4]=1 */
            default:
                CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INIT_INV_OBSRX_ADCDIV,
                        getMykonosErrorMessage(MYKONOS_ERR_INIT_INV_OBSRX_ADCDIV));
                return MYKONOS_ERR_INIT_INV_OBSRX_ADCDIV;
        }
    }

    /* Determine ORx and Sniffer channel settings */
    if (device->obsRx > 0)
    {
        /* verify pointers in data structure are valid (non-zero) */
        if (device->profilesValid & (ORX_PROFILE_VALID | SNIFF_PROFILE_VALID))
        {
            if (device->obsRx->realIfData > 0)
            {
                obsRxRealIf = 1;
            }
        }

        /* TODO: verify that the decimation matches the given IQ rate in the data structure */

        /* Set Sniffer profile (digital filter enables) if sniffer profile enabled (valid pointer) */
        if (device->profilesValid & SNIFF_PROFILE_VALID)
        {
            sniffChannelSettings = 0;
            switch (device->obsRx->snifferProfile->rhb1Decimation)
            {
                case 1:
                    break;
                case 2:
                    sniffChannelSettings |= 0x10;
                    break;
                default:
                    CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INIT_INV_SNIFFER_RHB1,
                            getMykonosErrorMessage(MYKONOS_ERR_INIT_INV_SNIFFER_RHB1));
                    return MYKONOS_ERR_INIT_INV_SNIFFER_RHB1;
            }

            if (device->obsRx->snifferProfile->rxFir > 0)
            {

                switch (device->obsRx->snifferProfile->rxFirDecimation)
                {
                    case 1:
                        sniffChannelSettings |= 0x01;
                        break;
                    case 2:
                        sniffChannelSettings |= 0x02;
                        break;
                    case 4:
                        sniffChannelSettings |= 0x03;
                        break;
                    default:
                        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INIT_INV_SNIFFER_RFIR_DEC,
                                getMykonosErrorMessage(MYKONOS_ERR_INIT_INV_SNIFFER_RFIR_DEC));
                        return MYKONOS_ERR_INIT_INV_SNIFFER_RFIR_DEC;
                }
            }
        }

        /* Set ORx profile (digital filter enables) if ORx profile enabled (valid pointer) */
        if (device->profilesValid & ORX_PROFILE_VALID)
        {
            orxChannelSettings = 0;
            switch (device->obsRx->orxProfile->rhb1Decimation)
            {
                case 1:
                    break;
                case 2:
                    orxChannelSettings |= 0x10;
                    break;
                default:
                    CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INIT_INV_ORX_RHB1,
                            getMykonosErrorMessage(MYKONOS_ERR_INIT_INV_ORX_RHB1));
                    return MYKONOS_ERR_INIT_INV_ORX_RHB1;
            }

            if (device->obsRx->orxProfile->rxFir > 0)
            {/* if pointer to orx rxFIR is valid */

                switch (device->obsRx->orxProfile->rxFirDecimation)
                {
                    case 1:
                        orxChannelSettings |= 0x01;
                        break;
                    case 2:
                        orxChannelSettings |= 0x02;
                        break;
                    case 4:
                        orxChannelSettings |= 0x03;
                        break;
                    default:
                        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INIT_INV_ORX_RFIR_DEC,
                                getMykonosErrorMessage(MYKONOS_ERR_INIT_INV_ORX_RFIR_DEC));
                        return MYKONOS_ERR_INIT_INV_ORX_RFIR_DEC;
                }
            }
        }
    }

    /************************************************************************
     * Set channel config settings for Rx/Tx/oRx and Sniffer channels
     ***********************************************************************
     */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_CONFIGURATION_CONTROL_2, txChannelSettings);
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_CONFIGURATION_CONTROL_4, rxChannelSettings);

    /* Set option to use new high rejection DEC5 filters in main Rx1/Rx2 path.  */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_CONFIGURATION_CONTROL_5, ((rxRealIfData << 2) | (enRxHighRejDec5 ? 3 : 0)));

    if (rxRealIfData > 0)
    {
        /* Set Rx1 NCO frequency */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_RX_NCO_CH1_FTW_BYTE_3, 0x40);
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_RX_NCO_CH1_FTW_BYTE_2, 0x00);
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_RX_NCO_CH1_FTW_BYTE_1, 0x01);

        /* Set Rx2 NCO frequency */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_RX_NCO_CH2_FTW_BYTE_3, 0x40);
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_RX_NCO_CH2_FTW_BYTE_2, 0x00);
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_RX_NCO_CH2_FTW_BYTE_1, 0x01);

        /* Enable NCO for Rx1 and Rx2 to shift data from zero IF to real IF */
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_RX_NCO_CONTROL, 3, 0x03, 0);
    }

    CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_DPD_SNIFFER_CONFIGURATION_CONTROL_2, obsRxRealIf, 0x40, 6);

    /* if sniffer profile disabled, Sniffer config reg is set to default = 0x80 */
    sniffChannelSettings |= 0x80; /* use PFIR coef set B = bit[7]=1*/
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_SNIFFER_CONFIGURATION_CONTROL, sniffChannelSettings);

    /* if orx profile not valid, SPI reg for orx config is set to default = 0x00 */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_DPD_CONFIGURATION_CONTROL, orxChannelSettings);
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_LOOPBACK_CONFIGURATION_CONTROL, orxChannelSettings);

    /* Set ADC divider, DAC divider, and ObsRx/sniffer dividers */
    CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_CLOCK_CONTROL_1, adcDacClockRateSelect, 0x13, 0);

    /* Disable ORx digital DC offset - bad default SPI setting for ObsRx DC offset */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_DIGITAL_DC_OFFSET_CH3_DPD_M_SHIFT, 0x60);

    /* Necessary write for ARM init cals to work */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_SNIFF_RXLOGEN_BYTE1, 0x0E);

    /* Increase Sniffer LNA gain */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_SNRX_LNA_BIAS_C, 0x0F);

    /* Set the CLKPLL with the frequency from the device data structure */
    retVal = MYKONOS_initDigitalClocks(device);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    /* Wait for CLKPLL CP Cal done and CLKPLL Lock  or throw error message */
    CMB_wait_ms(500);
    retVal = MYKONOS_waitForEvent(device, CLKPLLCP, 1000000);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    retVal = MYKONOS_waitForEvent(device, CLKPLL_LOCK, 1000000);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    /* Enable digital clocks - this is gated by the CLKPLL being locked */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_CLOCK_CONTROL_0, 0x14);

    /* Set the Tx PFIR synchronization clock */
    retVal = MYKONOS_setTxPfirSyncClk(device);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    /* Set the Rx PFIR synchronization clock */
    retVal = MYKONOS_setRxPfirSyncClk(device);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    if (device->profilesValid & TX_PROFILE_VALID)
    {
        /* Set pre TxPFIR Half band filter interpolation */
        if (device->tx->txProfile->txInputHbInterpolation == 1)
        {
            /* disable TX input half band ([3:2]=00) - but allow DPD to still be enabled for tx profiles with wide BW [1] */
            enableDpd = (device->tx->txProfile->enableDpdDataPath > 0) ? 1 : 0;
            CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_TX_FILTER_CONFIGURATION, enableDpd, 0x0E, 1);
        }
        else if (device->tx->txProfile->txInputHbInterpolation == 2)
        {
            if (device->tx->txProfile->iqRate_kHz > 160000)
            {
                CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INIT_INV_TXINPUTHB0_INV_RATE,
                        getMykonosErrorMessage(MYKONOS_ERR_INIT_INV_TXINPUTHB0_INV_RATE));
                return MYKONOS_ERR_INIT_INV_TXINPUTHB0_INV_RATE;
            }

            /* enable TX input half band[2]=1, and Enable DPD[1]=1*/
            CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_TX_FILTER_CONFIGURATION, 3, 0x0E, 1);
        }
        else if (device->tx->txProfile->txInputHbInterpolation == 4)
        {
            if (device->tx->txProfile->iqRate_kHz > 80000)
            {
                CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INIT_INV_TXINPUTHB_INV_RATE,
                        getMykonosErrorMessage(MYKONOS_ERR_INIT_INV_TXINPUTHB_INV_RATE));
                return MYKONOS_ERR_INIT_INV_TXINPUTHB_INV_RATE;
            }

            /* enable TX input HB0[3]=1 and Tx input HB[2]=1, and Enable DPD[1]=1*/
            CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_TX_FILTER_CONFIGURATION, 7, 0x0E, 1);
        }
        else
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INIT_INV_TXINPUTHB_PARM,
                    getMykonosErrorMessage(MYKONOS_ERR_INIT_INV_TXINPUTHB_PARM));
            return MYKONOS_ERR_INIT_INV_TXINPUTHB_PARM;
        }

        /* Enable SPI Tx Atten mode */
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_TX_TPC_CONFIG, 0x01, 0x03, 0);
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_TX_TPC_CONFIG, 0x01, 0x0C, 2);

        /* Set Tx1 Atten step size */
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_TX_INCR_DECR_WORD, device->tx->txAttenStepSize, 0x60, 5);

        /* Set Tx2 Atten step size */
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_TX_TPC_CONFIG, device->tx->txAttenStepSize, 0x60, 5);
    }
    else
    {
        /* Power down DACs */
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_TX_PD_OVERIDE_7_0, 3, 0x0C, 2);
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_TX_PD_OVERRIDE_CONTROL_7_0, 3, 0x0C, 2);
    }

    /* Set RxFE LO Common mode */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_RXFE1_LOCM, 0xF0);
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_RXFE2_LOCM, 0xF0);

    /* Set Rxloopback LO Common mode */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_RXLOOPBACK1_CNTRL_1, 0xFF);
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_RXLOOPBACK2_CNTRL_1, 0xFF);

    /* Setup MGC or AGC Rx gain control */
    retVal = MYKONOS_setupRxAgc(device);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    /* Default Rx to use manual gain control until AGC enabled by user */
    MYKONOS_setRxGainControlMode(device, MGC);

    retVal = MYKONOS_setupObsRxAgc(device);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    /* Default ObsRx to use manual gain control until AGC enabled by user */
    MYKONOS_setObsRxGainControlMode(device, MGC);

    /* Disable GPIO select bits by setting to b11 */
    CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_CONFIGURATION_CONTROL_1, 3, 0x30, 4);

    /* Extra settings for ARM calibrations */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_DIGITAL_DC_OFFSET_SHIFT, 0x11);
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_RX_LOOPBACK1_CNTRL_4, 0x04);
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_RX_LOOPBACK2_CNTRL_4, 0x04);
    CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_AGC_ORX_SNRX_CFG_2, 0x20, 0x20, 0);

    /* Extra settings for ADC initialisation */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_RX_ADC_FLASH_DELAY, 0x28);
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_RX_ADC_FLASH_CTRL, 0x0E);
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_ORX_ADC_FLASH_DELAY, 0x28);
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_ORX_ADC_FLASH_CTRL, 0x0E);

    /* Move to Alert ENSM state */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_ENSM_CONFIG_7_0, 0x05);

    return MYKONOS_ERR_OK;
}

/**
 * \brief Verifies the init structure profiles are valid combinations
 *
 * This function checks that the Rx/Tx/ORx/Sniffer profiles have valid clock rates in
 * order to operate together.  Rx/Tx and ORx/Sniffer share a common high speed digital
 * clock. If an invalid combination of profiles is detected, an error will be
 * returned. If a profile in the init structure is unused, the user should zero
 * out all members of that particular profile structure.  If a Rx/Tx/ORx/Sniffer profile
 * has an IQ rate = 0, it is assumed that the profile is disabled.
 *
 * \pre This function is private and is not called directly by the user.
 *
 * This function uses mykVerifyTxProfile() and mykVerifyRxProfile() as helper functions.
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 * - device->spiSettings->chipSelectIndex
 *
 * \param device Structure pointer to Mykonos device data structure
 *
 * \retval MYKONOS_ERR_PROFILES_HSDIGCLK profiles loaded are not valid
 * \retval MYKONOS_ERR_OK  Function completed successfully
 */
mykonosErr_t MYKONOS_verifyProfiles(mykonosDevice_t *device)
{
    mykonosErr_t retVal = MYKONOS_ERR_OK;

    uint32_t rxHsDigClk_kHz = 0;
    uint32_t orxHsDigClk_kHz = 0;
    uint32_t snifferHsDigClk_kHz = 0;
    uint32_t txHsDigClk_kHz = 0;

    mykonosRxProfile_t *rxProfile = NULL;
    mykonosTxProfile_t *txProfile = NULL;
    mykonosRxProfile_t *orxProfile = NULL;
    mykonosRxProfile_t *snifferProfile = NULL;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_verifyProfiles()\n");
#endif

    device->profilesValid = 0;

    /* Check all loaded profiles */
    if (device->tx->txChannels != TXOFF)
    {
        txProfile = device->tx->txProfile;
        retVal = mykVerifyTxProfile(device, txProfile, &txHsDigClk_kHz);
        if (retVal != MYKONOS_ERR_OK)
        {
            return retVal;
        }
    }

    if (device->rx->rxChannels != RXOFF)
    {
        rxProfile = device->rx->rxProfile;
        retVal = mykVerifyRxProfile(device, MYK_RX_PROFILE, rxProfile, &rxHsDigClk_kHz);
        if (retVal != MYKONOS_ERR_OK)
        {
            return retVal;
        }
    }

    if (device->obsRx->obsRxChannelsEnable != MYK_OBS_RXOFF)
    {
        if (device->obsRx->obsRxChannelsEnable & MYK_ORX1_ORX2)
        {
            orxProfile = device->obsRx->orxProfile;
            retVal = mykVerifyRxProfile(device, MYK_OBS_PROFILE, orxProfile, &orxHsDigClk_kHz);
            if (retVal != MYKONOS_ERR_OK)
            {
                return retVal;
            }
        }

        if (device->obsRx->obsRxChannelsEnable & MYK_SNRXA_B_C)
        {
            snifferProfile = device->obsRx->snifferProfile;
            retVal = mykVerifyRxProfile(device, MYK_SNIFFER_PROFILE, snifferProfile, &snifferHsDigClk_kHz);
            if (retVal != MYKONOS_ERR_OK)
            {
                return retVal;
            }
        }
    }

    return retVal;
}

/**
 * \brief Write indirect registers (Programmable FIRs, Rx gain tables, JESD204B settings).  Must be done after Multi Chip Sync
 *
 * The BBP should never need to call this function.  It is called automatically by the initArm function.
 * This function is a continuation of the MYKONOS_initialize() function.  This part of initialization must be done after
 * the BBP has completed Multi chip Sync.  These registers include FIR filters, Rx gain tables, and JESD204B framer/deframer
 * config registers.
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 * - device->spiSettings->chipSelectIndex
 *
 * \param device Pointer to the Mykonos device data structure containing settings
 *                   an error.
 * \return Returns enum mykonosErr_t, MYKONOS_ERR_OK=pass, !MYKONOS_ERR_OK=fail
 */
mykonosErr_t MYKONOS_initSubRegisterTables(mykonosDevice_t *device)
{
    mykonosErr_t retVal = MYKONOS_ERR_OK;

    /* ------------------------------------------------------------------------------------------- */
    /* Program submap register tables after digital clocks are up and running and Multi Chip Sync */
    /* has been completed by BBP */
    if (device->profilesValid & TX_PROFILE_VALID)
    {
        if (device->tx->txProfile->txFir > 0)
        {
            retVal = MYKONOS_programFir(device, TX1TX2_FIR, device->tx->txProfile->txFir);
            if (retVal != MYKONOS_ERR_OK)
            {
                return retVal;
            }
        }
    }

    if (device->profilesValid & RX_PROFILE_VALID)
    {
        if (device->rx->rxProfile->rxFir > 0)
        {
            retVal = MYKONOS_programFir(device, RX1RX2_FIR, device->rx->rxProfile->rxFir);
            if (retVal != MYKONOS_ERR_OK)
            {
                return retVal;
            }

            /* Load Rx gain table */
            retVal = MYKONOS_programRxGainTable(device, &RxGainTable[0][0], (sizeof(RxGainTable) >> 2), RX1_RX2_GT);
            if (retVal != MYKONOS_ERR_OK)
            {
                return retVal;
            }

            retVal = MYKONOS_setRx1ManualGain(device, device->rx->rxGainCtrl->rx1GainIndex);
            if (retVal != MYKONOS_ERR_OK)
            {
                return retVal;
            }

            retVal = MYKONOS_setRx2ManualGain(device, device->rx->rxGainCtrl->rx2GainIndex);
            if (retVal != MYKONOS_ERR_OK)
            {
                return retVal;
            }

            /* Enable Digital gain for Rx and ObsRx gain tables */
            CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_DIGITAL_GAIN_CONFIG, 0x80);
        }
    }

    if (device->profilesValid & SNIFF_PROFILE_VALID)
    {
        if (device->obsRx->snifferProfile->rxFir > 0)
        {
            retVal = MYKONOS_programFir(device, OBSRX_B_FIR, device->obsRx->snifferProfile->rxFir);
            if (retVal != MYKONOS_ERR_OK)
            {
                return retVal;
            }

            /* Load Sniffer Rx gain table */
            retVal = MYKONOS_programRxGainTable(device, &SnRxGainTable[0][0], (sizeof(SnRxGainTable) >> 2), SNRX_GT);
            if (retVal != MYKONOS_ERR_OK)
            {
                return retVal;
            }

            retVal = MYKONOS_setObsRxManualGain(device, OBS_SNIFFER_A, device->obsRx->snifferGainCtrl->gainIndex);
            if (retVal != MYKONOS_ERR_OK)
            {
                return retVal;
            }
        }
    }

    if (device->profilesValid & ORX_PROFILE_VALID)
    {
        if (device->obsRx->orxProfile->rxFir > 0)
        {/* if pointer to orx rxFIR is valid */
            retVal = MYKONOS_programFir(device, OBSRX_A_FIR, device->obsRx->orxProfile->rxFir);
            if (retVal != MYKONOS_ERR_OK)
            {
                return retVal;
            }

            /* Load ORx gain table */
            retVal = MYKONOS_programRxGainTable(device, &ORxGainTable[0][0], (sizeof(ORxGainTable) >> 2), ORX_GT);
            if (retVal != MYKONOS_ERR_OK)
            {
                return retVal;
            }

            retVal = MYKONOS_setObsRxManualGain(device, OBS_RX1_TXLO, device->obsRx->orxGainCtrl->orx1GainIndex);
            if (retVal != MYKONOS_ERR_OK)
            {
                return retVal;
            }

            retVal = MYKONOS_setObsRxManualGain(device, OBS_RX2_TXLO, device->obsRx->orxGainCtrl->orx2GainIndex);
            if (retVal != MYKONOS_ERR_OK)
            {
                return retVal;
            }
        }
    }

    /* Load Loopback Gain Table for ARM calibrations */
    {
        uint8_t loopBackGainTable[6][4] = {
        /* Order: {FE table, External Ctl, Digital Gain/Atten, Enable Atten} */
        {0, 0, 0, 0}, /* Gain index 255 */
        {7, 0, 0, 0}, /* Gain index 254 */
        {13, 0, 1, 1}, /* Gain index 253 */
        {18, 0, 3, 1}, /* Gain index 252 */
        {23, 0, 3, 1}, /* Gain index 251 */
        {28, 0, 0, 0} /* Gain index 250 */
        };
        retVal = MYKONOS_programRxGainTable(device, &loopBackGainTable[0][0], (sizeof(loopBackGainTable) >> 2), LOOPBACK_GT);
    }

    /* Enable Digital gain for ORx/Sniffer/Loopback gain table */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_DPD_SNIFFER_DIGITAL_GAIN_CONFIG, 0x80);

    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_RCAL_CONTROL, 0x07);/* Set RCAL code to nominal */

    /* If Valid Rx Profile or valid ObsRx profile, setup serializers */
    if ((device->profilesValid & RX_PROFILE_VALID) || (device->profilesValid & (ORX_PROFILE_VALID | SNIFF_PROFILE_VALID)))
    {
        retVal = MYKONOS_setupSerializers(device);
        if (retVal != MYKONOS_ERR_OK)
        {
            return retVal;
        }
    }

    if ((device->rx->rxChannels != RXOFF) && (device->profilesValid & RX_PROFILE_VALID))
    {
        retVal = MYKONOS_setupJesd204bFramer(device);
        if (retVal != MYKONOS_ERR_OK)
        {
            return retVal;
        }
    }

    if ((device->obsRx->obsRxChannelsEnable != MYK_OBS_RXOFF) && (device->profilesValid & (ORX_PROFILE_VALID | SNIFF_PROFILE_VALID)))
    {
        retVal = MYKONOS_setupJesd204bObsRxFramer(device);
        if (retVal != MYKONOS_ERR_OK)
        {
            return retVal;
        }
    }

    if ((device->tx->txChannels != TXOFF) && (device->profilesValid & TX_PROFILE_VALID))
    {
        retVal = MYKONOS_setupDeserializers(device);
        if (retVal != MYKONOS_ERR_OK)
        {
            return retVal;
        }

        retVal = MYKONOS_setupJesd204bDeframer(device);
        if (retVal != MYKONOS_ERR_OK)
        {
            return retVal;
        }

        retVal = MYKONOS_setTx1Attenuation(device, device->tx->tx1Atten_mdB);
        if (retVal != MYKONOS_ERR_OK)
        {
            return retVal;
        }

        retVal = MYKONOS_setTx2Attenuation(device, device->tx->tx2Atten_mdB);
        if (retVal != MYKONOS_ERR_OK)
        {
            return retVal;
        }
    }

    return MYKONOS_ERR_OK;
}

/**
 * \brief Performs a blocking wait for a Mykonos calibration or Pll Lock
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 * - device->spiSettings->chipSelectIndex
 *
 * \param device Pointer to the Mykonos device data structure containing settings
 * \param waitEvent the enum value of the event to wait for
 * \param timeout_us If timeout_us time has passed, function will return with
 *                   an error.
 * \return Returns enum mykonosErr_t, MYKONOS_ERR_OK=pass, !MYKONOS_ERR_OK=fail
 */
mykonosErr_t MYKONOS_waitForEvent(mykonosDevice_t *device, waitEvent_t waitEvent, uint32_t timeout_us)
{
    uint16_t spiAddr = 0;
    uint8_t spiBit = 0;
    uint8_t doneBitLevel = 0;
    uint8_t data = 0;
    mykonosErr_t errCode = MYKONOS_ERR_OK;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_waitForEvent()\n");
#endif

    switch (waitEvent)
    {
        case CALPLL_LOCK:/* wait for x17F[7]=1 */
            spiAddr = MYKONOS_ADDR_CALPLL_SDM_CONTROL;
            spiBit = 7;
            doneBitLevel = 1;
            errCode = MYKONOS_ERR_WAITFOREVENT_TIMEDOUT_CALPLL_LOCK;
            break;
        case CLKPLLCP: /* wait for x154[5]=1 */
            spiAddr = MYKONOS_ADDR_CLK_SYNTH_CAL_STAT;
            spiBit = 5;
            doneBitLevel = 1;
            errCode = MYKONOS_ERR_WAITFOREVENT_TIMEDOUT_CLKPLLCP;
            break;
        case CLKPLL_LOCK: /* wait for x157[0]=1 */
            spiAddr = MYKONOS_ADDR_CLK_SYNTH_VCO_BAND_BYTE1;
            spiBit = 0;
            doneBitLevel = 1;
            errCode = MYKONOS_ERR_WAITFOREVENT_TIMEDOUT_CLKPLL_LOCK;
            break;
        case RF_RXPLLCP:/* wait for x254[5]=1 */
            spiAddr = MYKONOS_ADDR_RXSYNTH_CP_CAL_STAT;
            spiBit = 5;
            doneBitLevel = 1;
            errCode = MYKONOS_ERR_WAITFOREVENT_TIMEDOUT_RXPLLCP;
            break;
        case RF_RXPLL_LOCK: /* wait for x257[0]=1 */
            spiAddr = MYKONOS_ADDR_RXSYNTH_VCO_BAND_BYTE1;
            spiBit = 0;
            doneBitLevel = 1;
            errCode = MYKONOS_ERR_WAITFOREVENT_TIMEDOUT_RXPLL_LOCK;
            break;
        case RF_TXPLLCP: /* wait for x2C4[5]=1 */
            spiAddr = MYKONOS_ADDR_TXSYNTH_CP_CAL_STAT;
            spiBit = 5;
            doneBitLevel = 1;
            errCode = MYKONOS_ERR_WAITFOREVENT_TIMEDOUT_TXPLLCP;
            break;
        case RF_TXPLL_LOCK: /* wait for x2C7[0]=1 */
            spiAddr = MYKONOS_ADDR_TXSYNTH_VCO_BAND_BYTE1;
            spiBit = 0;
            doneBitLevel = 1;
            errCode = MYKONOS_ERR_WAITFOREVENT_TIMEDOUT_TXPLL_LOCK;
            break;
        case RF_SNIFFERPLLCP: /* wait for x354[7]=1 */
            spiAddr = MYKONOS_ADDR_SNIFF_RXSYNTH_CP_CAL_STAT;
            spiBit = 5;
            doneBitLevel = 1;
            errCode = MYKONOS_ERR_WAITFOREVENT_TIMEDOUT_SNIFFPLLCP;
            break;
        case RF_SNIFFERPLL_LOCK: /* wait for x357[0]=1 */
            spiAddr = MYKONOS_ADDR_SNIFF_RXSYNTH_VCO_BAND_BYTE1;
            spiBit = 0;
            doneBitLevel = 1;
            errCode = MYKONOS_ERR_WAITFOREVENT_TIMEDOUT_SNIFFPLL_LOCK;
            break;
        case RXBBF_CALDONE: /* wait for x1B2[5]=0 */
            spiAddr = MYKONOS_ADDR_CALIBRATION_CONTROL;
            spiBit = 5;
            doneBitLevel = 0;
            errCode = MYKONOS_ERR_WAITFOREVENT_TIMEDOUT_RXBBFCALDONE;
            break;
        case TXBBF_CALDONE:/* wait for x1B2[0]=0 */
            spiAddr = MYKONOS_ADDR_CALIBRATION_CONTROL;
            spiBit = 0;
            doneBitLevel = 0;
            errCode = MYKONOS_ERR_WAITFOREVENT_TIMEDOUT_TXBBFCALDONE;
            break;
        case RX_RFDC_CALDONE: /* wait for x1B2[1]=0 */
            spiAddr = MYKONOS_ADDR_CALIBRATION_CONTROL;
            spiBit = 1;
            doneBitLevel = 0;
            errCode = MYKONOS_ERR_WAITFOREVENT_TIMEDOUT_RFDCCALDONE;
            break;
        case RX_ADCTUNER_CALDONE:/* wait for x1B2[7]=0 */
            spiAddr = MYKONOS_ADDR_CALIBRATION_CONTROL;
            spiBit = 7;
            doneBitLevel = 0;
            errCode = MYKONOS_ERR_WAITFOREVENT_TIMEDOUT_ADCTUNECALDONE;
            break;
        case RX1_ADCPROFILE:/* wait for x5DD[5]=0 */
            spiAddr = MYKONOS_ADDR_RX_ADC1_PRFL;
            spiBit = 5;
            doneBitLevel = 0;
            errCode = MYKONOS_ERR_WAITFOREVENT_TIMEDOUT_RX1ADCPROFILE;
            break;
        case RX2_ADCPROFILE:/* wait for x5DE[5]=0 */
            spiAddr = MYKONOS_ADDR_RX_ADC2_PRFL;
            spiBit = 5;
            doneBitLevel = 0;
            errCode = MYKONOS_ERR_WAITFOREVENT_TIMEDOUT_RX2ADCPROFILE;
            break;
        case ORX_ADCPROFILE:/* wait for x5DF[5]=0 */
            spiAddr = MYKONOS_ADDR_ORX_ADC_PRFL;
            spiBit = 5;
            doneBitLevel = 0;
            errCode = MYKONOS_ERR_WAITFOREVENT_TIMEDOUT_ORXADCPROFILE;
            break;
        case RCAL_CALDONE: /* wait for x1B2[6]=0 */
            spiAddr = MYKONOS_ADDR_CALIBRATION_CONTROL;
            spiBit = 6;
            doneBitLevel = 0;
            errCode = MYKONOS_ERR_WAITFOREVENT_TIMEDOUT_RCALDONE;
            break;
        case ARMBUSY:/* wait for xD30[7]=0 */
            spiAddr = MYKONOS_ADDR_ARM_CMD;
            spiBit = 7;
            doneBitLevel = 0;
            errCode = MYKONOS_ERR_WAITFOREVENT_TIMEDOUT_ARMBUSY;
            break;
        case INITARM_DONE:
            spiAddr = MYKONOS_ADDR_ARM_CMD;
            spiBit = 7;
            doneBitLevel = 0;
            errCode = MYKONOS_ERR_WAITFOREVENT_TIMEDOUT_INITARMDONE;
            break;
        default:
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_WAITFOREVENT_INV_PARM,
                    getMykonosErrorMessage(MYKONOS_ERR_WAITFOREVENT_INV_PARM));
            return MYKONOS_ERR_WAITFOREVENT_INV_PARM;
    }

    CMB_setTimeout_us(timeout_us); /* timeout after desired time */

    do
    {
        CMB_SPIReadByte(device->spiSettings, spiAddr, &data);

        /* For SW verification tests, allow API to think all cals are complete*/
#ifdef MYK_CALS_DONE_SWDEBUG
        if (doneBitLevel == 0)
        {
            data = 0x00;
        }
        else
        {
            data = 0xFF;
        }
#endif

        if (CMB_hasTimeoutExpired() > 0)
        {
            CMB_writeToLog(ADIHAL_LOG_WARNING, device->spiSettings->chipSelectIndex, errCode, getMykonosErrorMessage(errCode));
            return errCode;
        }
    } while (((data >> spiBit) & 0x01) != doneBitLevel);

    return MYKONOS_ERR_OK;
}

/**
 * \brief Performs a readback with no wait for a Mykonos calibration or Pll Lock
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 * - device->spiSettings->chipSelectIndex
 *
 * \param device Pointer to the Mykonos device data structure containing settings
 * \param waitEvent the enum value of the event to wait for
 * \param eventDone Return value: 1= calibration event is complete.  0 = Event is still pending
 * \return Returns enum mykonosErr_t, MYKONOS_ERR_OK=pass, !MYKONOS_ERR_OK=fail
 */
mykonosErr_t MYKONOS_readEventStatus(mykonosDevice_t *device, waitEvent_t waitEvent, uint8_t *eventDone)
{
    uint16_t spiAddr = 0;
    uint8_t data = 0;
    uint8_t spiBit = 0;
    uint8_t doneBitLevel = 0;

    switch (waitEvent)
    {
        case CALPLL_LOCK:/* wait for x17F[7]=1 */
            spiAddr = MYKONOS_ADDR_CALPLL_SDM_CONTROL;
            spiBit = 7;
            doneBitLevel = 1;
            break;
        case CLKPLLCP: /* wait for x154[5]=1 */
            spiAddr = MYKONOS_ADDR_CLK_SYNTH_CAL_STAT;
            spiBit = 5;
            doneBitLevel = 1;
            break;
        case CLKPLL_LOCK: /* wait for x157[0]=1 */
            spiAddr = MYKONOS_ADDR_CLK_SYNTH_VCO_BAND_BYTE1;
            spiBit = 0;
            doneBitLevel = 1;
            break;
        case RF_RXPLLCP:/* wait for x254[5]=1 */
            spiAddr = MYKONOS_ADDR_RXSYNTH_CP_CAL_STAT;
            spiBit = 5;
            doneBitLevel = 1;
            break;
        case RF_RXPLL_LOCK: /* wait for x257[0]=1 */
            spiAddr = MYKONOS_ADDR_RXSYNTH_VCO_BAND_BYTE1;
            spiBit = 0;
            doneBitLevel = 1;
            break;
        case RF_TXPLLCP: /* wait for x2C4[5]=1 */
            spiAddr = MYKONOS_ADDR_TXSYNTH_CP_CAL_STAT;
            spiBit = 5;
            doneBitLevel = 1;
            break;
        case RF_TXPLL_LOCK: /* wait for x2C7[0]=1 */
            spiAddr = MYKONOS_ADDR_TXSYNTH_VCO_BAND_BYTE1;
            spiBit = 0;
            doneBitLevel = 1;
            break;
        case RF_SNIFFERPLLCP: /* wait for x354[7]=1 */
            spiAddr = MYKONOS_ADDR_SNIFF_RXSYNTH_CP_CAL_STAT;
            spiBit = 5;
            doneBitLevel = 1;
            break;
        case RF_SNIFFERPLL_LOCK: /* wait for x357[0]=1 */
            spiAddr = MYKONOS_ADDR_SNIFF_RXSYNTH_VCO_BAND_BYTE1;
            spiBit = 0;
            doneBitLevel = 1;
            break;
        case RXBBF_CALDONE: /* wait for x1B2[5]=0 */
            spiAddr = MYKONOS_ADDR_CALIBRATION_CONTROL;
            spiBit = 5;
            doneBitLevel = 0;
            break;
        case TXBBF_CALDONE:/* wait for x1B2[0]=0 */
            spiAddr = MYKONOS_ADDR_CALIBRATION_CONTROL;
            spiBit = 0;
            doneBitLevel = 0;
            break;
        case RX_RFDC_CALDONE: /* wait for x1B2[1]=0 */
            spiAddr = MYKONOS_ADDR_CALIBRATION_CONTROL;
            spiBit = 1;
            doneBitLevel = 0;
            break;
        case RX_ADCTUNER_CALDONE:/* wait for x1B2[7]=0 */
            spiAddr = MYKONOS_ADDR_CALIBRATION_CONTROL;
            spiBit = 7;
            doneBitLevel = 0;
            break;
        case RX1_ADCPROFILE:/* wait for x5DD[5]=0 */
            spiAddr = MYKONOS_ADDR_RX_ADC1_PRFL;
            spiBit = 5;
            doneBitLevel = 0;
            break;
        case RX2_ADCPROFILE:/* wait for x5DE[5]=0 */
            spiAddr = MYKONOS_ADDR_RX_ADC2_PRFL;
            spiBit = 5;
            doneBitLevel = 0;
            break;
        case ORX_ADCPROFILE:/* wait for x5DF[5]=0 */
            spiAddr = MYKONOS_ADDR_ORX_ADC_PRFL;
            spiBit = 5;
            doneBitLevel = 0;
            break;
        case RCAL_CALDONE: /* wait for x1B2[6]=0 */
            spiAddr = MYKONOS_ADDR_CALIBRATION_CONTROL;
            spiBit = 6;
            doneBitLevel = 0;
            break;
        case ARMBUSY:/* wait for xD30[7]=0 */
            spiAddr = MYKONOS_ADDR_ARM_CMD;
            spiBit = 7;
            doneBitLevel = 0;
            break;
        default:
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_WAITFOREVENT_INV_PARM,
                    getMykonosErrorMessage(MYKONOS_ERR_WAITFOREVENT_INV_PARM));
            return MYKONOS_ERR_WAITFOREVENT_INV_PARM;
    }

    CMB_SPIReadByte(device->spiSettings, spiAddr, &data);

    if (doneBitLevel)
    {
        *eventDone = (data >> spiBit) & 0x01;
    }
    else
    {
        *eventDone = ((~(data >> spiBit)) & 0x01);
    }

    return MYKONOS_ERR_OK;
}

/**
 * \brief Sets the CLKPLL output frequency.
 *
 * This code updates the Synth and Loop filter settings based on a VCO
 * frequency LUT. The VCO frequency break points for the Synth LUT can be
 * found in an array called vcoFreqArrayMhz.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 * - device->spiSettings
 * - device->deviceClock_kHz
 * - device->rxSettings->rxProfile->vcoFreq_kHz
 * - device->rxSettings->rxProfile->clkPllHsDiv
 * - device->rxSettings->rxProfile->clkPllVcoDiv
 *
 * \param device is structure pointer to the MYKONOS data structure containing settings
 *
 * \return Returns enum MYKONOS_ERR, MYKONOS_ERR_OK=pass, !MYKONOS_ERR_OK=fail
 */
mykonosErr_t MYKONOS_initDigitalClocks(mykonosDevice_t *device)
{
    /* RF Synth variables */
    /* pull Synth and Loop filter values from a Look Up Table */
    uint8_t vcoOutLvl;
    uint8_t vcoVaractor; /* [3:0] */
    uint8_t vcoBiasRef; /* [2:0] */
    uint8_t vcoBiasTcf; /* [1:0] */
    uint8_t vcoCalOffset; /* [3:0] */
    uint8_t vcoVaractorRef; /* [3:0] */
    uint8_t loopFilterIcp; /* Icp[5:0] */
    uint8_t loopFilterC2C1; /* C2[3:0],C1[3:0] */
    uint8_t loopFilterR1C3; /* R1[3:0],C3[3:0] */
    uint8_t loopFilterR3; /* R3[3:0] */
    uint8_t vcoIndex;
    uint8_t i;

    /* RF PLL variables */
    uint16_t integerWord;
    uint32_t fractionalWord;
    uint32_t fractionalRemainder;
    uint32_t scaledRefClk_Hz;

    /* common */
    uint64_t hsDigClk_Hz = 0;
    uint32_t scaledRefClk_kHz = 0;
    uint8_t deviceClkDiv = 0;
    mykonosErr_t retVal = MYKONOS_ERR_OK;

    uint8_t clockControl2 = 0;
    uint8_t sdmSettings = 0;
    uint8_t hsDiv = 4;
    uint8_t vcoDiv = 0;
    uint8_t vcoDivTimes10 = 10;

    /******************RF Synth Section************************/
    static const uint8_t icp_46p08[53] = {15, 16, 17, 18, 19, 20, 21, 23, 24, 25, 26, 27, 28, 29, 30, 32, 20, 21, 21, 22, 23, 23, 24, 25, 25, 26, 27, 27, 28, 29, 29, 30,
            31, 32, 32, 33, 34, 34, 35, 36, 36, 37, 38, 39, 21, 21, 21, 22, 22, 22, 23, 23, 23};

    static const uint8_t icp_61p44[53] = {11, 12, 13, 13, 14, 15, 16, 17, 18, 18, 19, 20, 21, 22, 23, 24, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22,
            23, 24, 24, 25, 25, 26, 26, 27, 27, 28, 28, 29, 15, 16, 16, 16, 16, 17, 17, 17, 17};

    static const uint8_t icp_76p8[53] = {9, 9, 10, 11, 11, 12, 13, 13, 14, 15, 15, 16, 17, 17, 18, 19, 12, 12, 13, 13, 13, 14, 14, 15, 15, 15, 16, 16, 17, 17, 17, 18, 18,
            19, 19, 20, 20, 20, 21, 21, 22, 22, 23, 23, 12, 12, 13, 13, 13, 13, 13, 14, 14};

    static const uint32_t vcoFreqArray_kHz[53] = {12605000UL, 12245000UL, 11906000UL, 11588000UL, 11288000UL, 11007000UL, 10742000UL, 10492000UL, 10258000UL, 10036000UL,
            9827800UL, 9631100UL, 9445300UL, 9269800UL, 9103600UL, 8946300UL, 8797000UL, 8655300UL, 8520600UL, 8392300UL, 8269900UL, 8153100UL, 8041400UL, 7934400UL,
            7831800UL, 7733200UL, 7638400UL, 7547100UL, 7459000UL, 7374000UL, 7291900UL, 7212400UL, 7135500UL, 7061000UL, 6988700UL, 6918600UL, 6850600UL, 6784600UL,
            6720500UL, 6658200UL, 6597800UL, 6539200UL, 6482300UL, 6427000UL, 6373400UL, 6321400UL, 6270900UL, 6222000UL, 6174500UL, 6128400UL, 6083600UL, 6040100UL,
            5997700UL};

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_initDigitalClocks()\n");
#endif

    retVal = MYKONOS_calculateScaledDeviceClk_kHz(device, &scaledRefClk_kHz, &deviceClkDiv);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    hsDiv = device->clocks->clkPllHsDiv;
    vcoDiv = device->clocks->clkPllVcoDiv;

    switch (hsDiv)
    {
        case 4:
            hsDiv = 4; /* clockControl2[3:2] = 00 */
            break;
        case 5:
            hsDiv = 5;
            clockControl2 |= 0x04; /* Set bit[2]=1 */
            break;
        default:
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_CLKPLL_INV_HSDIV, getMykonosErrorMessage(MYKONOS_ERR_CLKPLL_INV_HSDIV));
            return MYKONOS_ERR_CLKPLL_INV_HSDIV;
    }

    switch (vcoDiv)
    {
        case VCODIV_1:
            vcoDivTimes10 = 10; /* clockControl2[1:0] = 00 */
            break;
        case VCODIV_1p5:
            vcoDivTimes10 = 15;
            clockControl2 |= 0x01;
            break;
        case VCODIV_2:
            vcoDivTimes10 = 20;
            clockControl2 |= 0x02;
            break;
        case VCODIV_3:
            vcoDivTimes10 = 30;
            clockControl2 |= 0x03;
            break;
        default:
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_CLKPLL_INV_VCODIV, getMykonosErrorMessage(MYKONOS_ERR_CLKPLL_INV_VCODIV));
            return MYKONOS_ERR_CLKPLL_INV_VCODIV;
    }

    clockControl2 |= ((deviceClkDiv & 3) << 4);

    /* find vco table index based on vco frequency */
    for (i = 0; device->clocks->clkPllVcoFreq_kHz < vcoFreqArray_kHz[i]; i++)
    {
        /* Intentionally blank, for loop exits when condition met, index i used below */
    }
    vcoIndex = i + 1;

    if (vcoIndex < 1 || vcoIndex > 53)
    {
        /* vco index out of range */
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SETCLKPLL_INV_VCOINDEX,
                getMykonosErrorMessage(MYKONOS_ERR_SETCLKPLL_INV_VCOINDEX));
        return MYKONOS_ERR_SETCLKPLL_INV_VCOINDEX;
    }

    /* valid for all refclk frequencies */
    vcoOutLvl = (vcoIndex <= 16) ? 13 : (vcoIndex > 16 && vcoIndex <= 44) ? 10 : 7;
    vcoVaractor = (vcoIndex <= 16) ? 0 : (vcoIndex > 16 && vcoIndex <= 44) ? 1 : 3;
    vcoBiasRef = (vcoIndex <= 7) ? 4 : (vcoIndex > 7 && vcoIndex <= 29) ? 6 : 7;
    vcoBiasTcf = (vcoIndex <= 29) ? 1 : (vcoIndex > 29 && vcoIndex <= 44) ? 2 : 3;

    vcoVaractorRef = (vcoIndex <= 16) ? 10 : (vcoIndex > 16 && vcoIndex <= 29) ? 11 : (vcoIndex > 29 && vcoIndex <= 32) ? 12 :
                     (vcoIndex > 32 && vcoIndex <= 44) ? 14 : 12;

    if ((scaledRefClk_kHz >= 40000) && (scaledRefClk_kHz < 53760))
    { /* Settings designed for 46.08 MHz PLL REFCLK */
        vcoCalOffset = (vcoIndex <= 2) ? 14 : (vcoIndex > 2 && vcoIndex <= 5) ? 13 : (vcoIndex > 5 && vcoIndex <= 13) ? 12 : (vcoIndex > 13 && vcoIndex <= 16) ? 11 : 15;

        loopFilterIcp = icp_46p08[vcoIndex - 1];
        loopFilterC2C1 = 0xF6; /* C2=0xF0 */
        loopFilterR1C3 = 0xD5;
        loopFilterR3 = 0x0E;
    }
    else if ((scaledRefClk_kHz >= 53760) && (scaledRefClk_kHz < 69120))
    { /* Settings designed for 61.44 MHz PLL REFCLK */
        vcoCalOffset = (vcoIndex <= 2) ? 14 : (vcoIndex > 2 && vcoIndex <= 5) ? 13 : (vcoIndex > 5 && vcoIndex <= 11) ? 12 : (vcoIndex > 11 && vcoIndex <= 16) ? 11 : 15;

        loopFilterIcp = icp_61p44[vcoIndex - 1];
        loopFilterC2C1 = 0xF6; /* C2=0xF0 */
        loopFilterR1C3 = 0xD5;
        loopFilterR3 = 0x0E;
    }
    else if ((scaledRefClk_kHz >= 69120) && (scaledRefClk_kHz <= 80000))
    { /* Settings designed for 76.8 MHz PLL REFCLK */
        vcoCalOffset = (vcoIndex <= 2) ? 14 : (vcoIndex > 2 && vcoIndex <= 5) ? 13 : (vcoIndex > 5 && vcoIndex <= 10) ? 12 : (vcoIndex > 10 && vcoIndex <= 16) ? 11 : 15;

        loopFilterIcp = icp_76p8[vcoIndex - 1];
        loopFilterC2C1 = (vcoIndex == 43) ? 0xF7 : 0xF6; /*C2=0xF0, C1=6 or 7 */
        loopFilterR1C3 = 0xD5;
        loopFilterR3 = 0x0E;
    }
    else
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SETRFPLL_INV_REFCLK, getMykonosErrorMessage(MYKONOS_ERR_SETRFPLL_INV_REFCLK));
        return MYKONOS_ERR_SETRFPLL_INV_REFCLK; /* invalid ref clk */
    }

    /* Hold CLKPLL digital logic in reset */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_CLOCK_CONTROL_4, 0x00);

    /* Set reference clock scaler + HSdiv + VCOdiv */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_CLOCK_CONTROL_2, clockControl2);

    /*Set VCO cal time */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_CLK_SYNTH_CAL_CONTROL, 0x02);

    /* Write Synth Setting regs */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_CLK_SYNTH_F_VCOTN_BYTE1, ((vcoCalOffset & 0x0F) << 3)); /* Set VCO Cal offset[3:0] */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_CLK_SYNTH_BYTE1, (0xC0 | (vcoVaractor & 0x0F))); /* Init ALC[3:0], VCO varactor[3:0] */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_CLK_SYNTH_BYTE2, (0x40 | (vcoOutLvl & 0x0F))); /* VCO output level */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_CLK_SYNTH_BYTE9, (((vcoBiasTcf & 0x03) << 3) | (vcoBiasRef & 0x07))); /* Set VCO Bias Tcf[1:0] and VCO Bias Ref[2:0] */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_CLK_SYNTH_VCO_CAL_REF, 0x30); /* Set VCO cal time (compare length) + Set VCO Cal Ref Tcf[2:0]=0 */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_CLK_SYNTH_VCO_VAR_CTL1, 0x70); /* Set VCO Varactor Ref Tcf[2:0] and VCO Varactor Offset[3:0] */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_CLK_SYNTH_VCO_VAR_CTL2, (vcoVaractorRef & 0x0F)); /* Set VCO Varactor Reference[3:0] */

    /* Write Loop filter */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_CLK_SYNTH_BYTE3, (0x80 | (loopFilterIcp & 0x03F))); /* Set Loop Filter Icp[5:0] */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_CLK_SYNTH_BYTE6, loopFilterC2C1); /* Set Loop Filter C2[3:0] and C1[3:0] */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_CLK_SYNTH_BYTE7, loopFilterR1C3); /* Set Loop Filter R1[3:0] and C3[3:0] */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_CLK_SYNTH_LF_R3, (loopFilterR3 & 0x0F)); /*Set Loop Filter R3[3:0] */

    /* Calculate PLL integer and fractional words with integer math */
    scaledRefClk_Hz = scaledRefClk_kHz * 1000;
    hsDigClk_Hz = (((uint64_t)(device->clocks->clkPllVcoFreq_kHz) * 10000) / vcoDivTimes10) / hsDiv;
    integerWord = (uint16_t)(hsDigClk_Hz / scaledRefClk_Hz);
    fractionalRemainder = hsDigClk_Hz % scaledRefClk_Hz;

    /* +1 >> 1 is rounding (add .5) */
    fractionalWord = ((uint32_t)((((uint64_t)fractionalRemainder * 4177920) / (uint64_t)scaledRefClk_Hz) + 1) >> 1);

    /* if fractionalWord rounded up and == PLL modulus, fix it */
    if (fractionalWord == 2088960)
    {
        fractionalWord = 0;
        integerWord = integerWord + 1;
    }

    if (fractionalWord > 0)
    { /* in normal case, the fractional word should be zero and SDM bypassed */
        CMB_writeToLog(ADIHAL_LOG_WARNING, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SETCLKPLL_INV_FRACWORD,
                getMykonosErrorMessage(MYKONOS_ERR_SETCLKPLL_INV_FRACWORD));
        /* down graded to warning, do not return error code */
        //return MYKONOS_ERR_SETCLKPLL_INV_FRACWORD;
        sdmSettings = 0x20;
    }
    else
    {
        /* Bypass SDM */
        sdmSettings = 0xE0;
    }

    /* Set PLL fractional word[22:0] */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_CLK_SYNTH_DIVIDER_FRAC_BYTE0, (fractionalWord & 0xFF));
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_CLK_SYNTH_DIVIDER_FRAC_BYTE1, ((fractionalWord >> 8) & 0xFF));
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_CLK_SYNTH_DIVIDER_FRAC_BYTE2, ((fractionalWord >> 16) & 0x7F));

    /* Write PLL integer word [7:0] */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_CLK_SYNTH_DIVIDER_INT_BYTE1, (sdmSettings | ((integerWord >> 8) & 0x7)));
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_CLK_SYNTH_DIVIDER_INT_BYTE0, (integerWord & 0xFF));

    /* Release PLL from reset, and set start VCO cal bit to 0 */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_CLOCK_CONTROL_4, 0x80);

    /* Power up CLKPLL */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_CLOCK_CONTROL_0, 0x00);
    CMB_wait_ms(200); /* Allow PLL time to power up */

    /* Enable Charge pump cal after Charge Pump current set (Icp[5:0]) */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_CLK_SYNTH_BYTE5, 0x84);

    /* Start VCO Cal */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_CLOCK_CONTROL_4, 0x81);

    return MYKONOS_ERR_OK;
}

/**
 * \brief Sets the RF PLL local oscillator frequency (RF carrier frequency).
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 * - device->spiSettings
 *
 * \param device is structure pointer to the MYKONOS data structure containing settings
 * \param pllName Name of the PLL to configure
 * \param rfPllLoFrequency_Hz Desired RF LO frequency
 *
 * \return MYKONOS_ERR_OK Function completed successfully
 * \return MYKONOS_ERR_SETRFPLL_ARMERROR ARM Command to set RF PLL frequency failed
 */
mykonosErr_t MYKONOS_setRfPllFrequency(mykonosDevice_t *device, mykonosRfPllName_t pllName, uint64_t rfPllLoFrequency_Hz)
{
    const uint8_t SETCMD_OPCODE = 0x0A;
    const uint8_t SET_PLL_FREQUENCY = 0x63;

    mykonosErr_t retVal = MYKONOS_ERR_OK;
    uint8_t armData[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    uint8_t extData[2] = {0, 0};
    uint32_t timeoutMs = 0;
    uint8_t cmdStatusByte = 0;

    armData[0] = (uint8_t)(rfPllLoFrequency_Hz & 0xFF);
    armData[1] = (uint8_t)((rfPllLoFrequency_Hz >> 8) & 0xFF);
    armData[2] = (uint8_t)((rfPllLoFrequency_Hz >> 16) & 0xFF);
    armData[3] = (uint8_t)((rfPllLoFrequency_Hz >> 24) & 0xFF);
    armData[4] = (uint8_t)((rfPllLoFrequency_Hz >> 32) & 0xFF);
    armData[5] = (uint8_t)((rfPllLoFrequency_Hz >> 40) & 0xFF);
    armData[6] = (uint8_t)((rfPllLoFrequency_Hz >> 48) & 0xFF);
    armData[7] = (uint8_t)((rfPllLoFrequency_Hz >> 56) & 0xFF);

    /* write 64-bit frequency to ARM memory */
    retVal = MYKONOS_writeArmMem(device, MYKONOS_ADDR_ARM_START_DATA_ADDR, &armData[0], 8);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    extData[0] = SET_PLL_FREQUENCY;

    switch (pllName)
    {
        case RX_PLL:
            extData[1] = 0x00;
            break;
        case TX_PLL:
            extData[1] = 0x01;
            break;
        case SNIFFER_PLL:
            extData[1] = 0x02;
            break;
        default:
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SETRFPLL_INV_PLLNAME,
                    getMykonosErrorMessage(MYKONOS_ERR_SETRFPLL_INV_PLLNAME));
            return MYKONOS_ERR_SETRFPLL_INV_PLLNAME;
        }
    }

    retVal = MYKONOS_sendArmCommand(device, SETCMD_OPCODE, &extData[0], sizeof(extData));
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    timeoutMs = 1000;
    retVal = MYKONOS_waitArmCmdStatus(device, SETCMD_OPCODE, timeoutMs, &cmdStatusByte);
    if (retVal != MYKONOS_ERR_OK)
    {
        if (cmdStatusByte > 0)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SETRFPLL_ARMERROR, getMykonosErrorMessage(MYKONOS_ERR_SETRFPLL_ARMERROR));
            return MYKONOS_ERR_SETRFPLL_ARMERROR;
        }

        return retVal;
    }

    if (cmdStatusByte > 0)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SETRFPLL_ARMERROR, getMykonosErrorMessage(MYKONOS_ERR_SETRFPLL_ARMERROR));
        return MYKONOS_ERR_SETRFPLL_ARMERROR;
    }

    return MYKONOS_ERR_OK;
}

/**
 * \brief Gets the RF PLL local oscillator frequency (RF carrier frequency).
 *
 * This function is used to get the RF PLL's frequency.  It can get the RX PLL, TX PLL
 * Sniffer PLL, and CLKPLL.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 * - device->spiSettings
 * - device->clocks->deviceClock_kHz
 *
 * \param device is structure pointer to the MYKONOS data structure containing settings
 * \param pllName Name of the PLL for which to read the frequency
 * \param rfPllLoFrequency_Hz RF LO frequency currently set for the PLL specified
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_GET_PLLFREQ_INV_REFCLKDIV Invalid CLKPLL reference clock divider read from Mykonos device
 * \retval MYKONOS_ERR_GET_PLLFREQ_INV_HSDIV Invalid CLKPLL high speed clock divider read from Mykonos device
 * \retval MYKONOS_ERR_GETRFPLL_INV_PLLNAME Invalid PLL name, can not get PLL frequency.  Use PLL name ENUM.
 * \retval MYKONOS_ERR_GETRFPLL_ARMERROR ARM Command to get RF PLL frequency failed
 * \retval MYKONOS_ERR_GETRFPLL_NULLPARAM rfPllLoFrequency_Hz function parameter pointer is NULL
 */
mykonosErr_t MYKONOS_getRfPllFrequency(mykonosDevice_t *device, mykonosRfPllName_t pllName, uint64_t *rfPllLoFrequency_Hz)
{
    const uint8_t RFPLL_LO_FREQUENCY = 0x63;

    mykonosErr_t retVal = MYKONOS_ERR_OK;
    uint8_t armData[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    uint8_t extData[2] = {0, 0};
    uint32_t timeoutMs = 0;
    uint8_t cmdStatusByte = 0;
    uint8_t getClkPllFrequency = 0;

    uint32_t clkPllIntWord = 0;
    uint32_t clkPllFracWord = 0;
    uint8_t clkPllRefClkDiv = 0;
    uint8_t hsDiv = 0;
    uint8_t vcoDivTimes10 = 0;
    uint8_t hsDivReg = 0;
    uint8_t vcoDivReg = 0;
    uint8_t clkPllIntWord7_0 = 0;
    uint8_t clkPllIntWord10_8 = 0;
    uint8_t clkPllFracWord7_0 = 0;
    uint8_t clkPllFracWord15_8 = 0;
    uint8_t clkPllFracWord22_16 = 0;
    uint64_t refclk_Hz = 0;

    extData[0] = RFPLL_LO_FREQUENCY;

    switch (pllName)
    {
        case CLK_PLL:
            getClkPllFrequency = 1;
            break;
        case RX_PLL:
            extData[1] = 0x00;
            break;
        case TX_PLL:
            extData[1] = 0x01;
            break;
        case SNIFFER_PLL:
            extData[1] = 0x02;
            break;
        default:
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETRFPLL_INV_PLLNAME,
                    getMykonosErrorMessage(MYKONOS_ERR_GETRFPLL_INV_PLLNAME));
            return MYKONOS_ERR_GETRFPLL_INV_PLLNAME;
        }
    }

    if (getClkPllFrequency > 0)
    {
        CMB_SPIReadField(device->spiSettings, MYKONOS_ADDR_CLOCK_CONTROL_2, &hsDivReg, 0x0C, 2);
        CMB_SPIReadField(device->spiSettings, MYKONOS_ADDR_CLOCK_CONTROL_2, &vcoDivReg, 0x03, 0);
        CMB_SPIReadField(device->spiSettings, MYKONOS_ADDR_CLOCK_CONTROL_2, &clkPllRefClkDiv, 0x70, 4);

        CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_CLK_SYNTH_DIVIDER_INT_BYTE0, &clkPllIntWord7_0);
        CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_CLK_SYNTH_DIVIDER_INT_BYTE1, &clkPllIntWord10_8);
        CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_CLK_SYNTH_DIVIDER_FRAC_BYTE0, &clkPllFracWord7_0);
        CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_CLK_SYNTH_DIVIDER_FRAC_BYTE1, &clkPllFracWord15_8);
        CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_CLK_SYNTH_DIVIDER_FRAC_BYTE2, &clkPllFracWord22_16);

        clkPllIntWord = (((uint32_t)(clkPllIntWord10_8 & 0x7) << 8) | clkPllIntWord7_0);
        clkPllFracWord = (((uint32_t)(clkPllFracWord22_16 & 0x7F) << 16) | ((uint32_t)(clkPllFracWord15_8) << 8) | clkPllFracWord7_0);

        switch (clkPllRefClkDiv)
        {
            case 0:
                refclk_Hz = (uint64_t)(device->clocks->deviceClock_kHz * 1000);
                break;
            case 1:
                refclk_Hz = (uint64_t)((device->clocks->deviceClock_kHz * 1000) >> 1);
                break; /* div 2 */
            case 2:
                refclk_Hz = (uint64_t)((device->clocks->deviceClock_kHz * 1000) >> 2);
                break; /* div 4 */
            case 3:
                refclk_Hz = (uint64_t)((device->clocks->deviceClock_kHz * 1000) << 1);
                break; /* times 2 */
            default:
            {
                CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GET_PLLFREQ_INV_REFCLKDIV,
                        getMykonosErrorMessage(MYKONOS_ERR_GET_PLLFREQ_INV_REFCLKDIV));
                return MYKONOS_ERR_GET_PLLFREQ_INV_REFCLKDIV;
            }
        }

        switch (vcoDivReg)
        {
            case 0:
                vcoDivTimes10 = 10;
                break;
            case 1:
                vcoDivTimes10 = 15;
                break;
            case 2:
                vcoDivTimes10 = 20;
                break;
            case 3:
                vcoDivTimes10 = 30;
                break;
        }

        switch (hsDivReg)
        {
            case 0:
                hsDiv = 4;
                break;
            case 1:
                hsDiv = 5;
                break;
            default:
            {
                CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GET_PLLFREQ_INV_HSDIV,
                        getMykonosErrorMessage(MYKONOS_ERR_GET_PLLFREQ_INV_HSDIV));
                return MYKONOS_ERR_GET_PLLFREQ_INV_HSDIV;
            }
        }

        /* round to nearest Hz for fractional word (fractional modulus = 2088960) */
        *rfPllLoFrequency_Hz = (uint64_t)(((refclk_Hz * clkPllIntWord) + (((refclk_Hz * clkPllFracWord / 1044480) + 1) >> 1)) * hsDiv * vcoDivTimes10 / 10);
    }
    else
    {
        retVal = MYKONOS_sendArmCommand(device, MYKONOS_ARM_GET_OPCODE, &extData[0], sizeof(extData));
        if (retVal != MYKONOS_ERR_OK)
        {
            return retVal;
        }

        timeoutMs = 1000;
        retVal = MYKONOS_waitArmCmdStatus(device, MYKONOS_ARM_GET_OPCODE, timeoutMs, &cmdStatusByte);
        if (retVal != MYKONOS_ERR_OK)
        {
            if (cmdStatusByte > 0)
            {
                CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETRFPLL_ARMERROR,
                        getMykonosErrorMessage(MYKONOS_ERR_GETRFPLL_ARMERROR));
                return MYKONOS_ERR_GETRFPLL_ARMERROR;
            }

            return retVal;
        }

        /* read 64-bit frequency from ARM memory */
        retVal = MYKONOS_readArmMem(device, MYKONOS_ADDR_ARM_START_DATA_ADDR, &armData[0], 8, 1);
        if (retVal != MYKONOS_ERR_OK)
        {
            return retVal;
        }

        if (cmdStatusByte > 0)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETRFPLL_ARMERROR, getMykonosErrorMessage(MYKONOS_ERR_GETRFPLL_ARMERROR));
            return MYKONOS_ERR_GETRFPLL_ARMERROR;
        }

        if (rfPllLoFrequency_Hz == NULL)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETRFPLL_NULLPARAM,
                    getMykonosErrorMessage(MYKONOS_ERR_GETRFPLL_NULLPARAM));
            return MYKONOS_ERR_GETRFPLL_NULLPARAM;
        }

        *rfPllLoFrequency_Hz = (uint64_t)((uint64_t)(armData[0])) | ((uint64_t)(armData[1]) << 8) | ((uint64_t)(armData[2]) << 16) | ((uint64_t)(armData[3]) << 24)
                | ((uint64_t)(armData[4]) << 32) | ((uint64_t)(armData[5]) << 40) | ((uint64_t)(armData[6]) << 48) | ((uint64_t)(armData[7]) << 56);
    }
    return MYKONOS_ERR_OK;
}

/**
 * \brief Checks if the PLLs are locked 
 * 
 * This function updates the pllLockStatus pointer with a lock status it per 
 * PLL.  
 * pllLockStatus[0] = CLKPLL Locked
 * pllLockStatus[1] = RX_PLL Locked
 * pllLockStatus[2] = TX_PLL Locked
 * pllLockStatus[3] = SNIFFER_PLL Locked
 * pllLockStatus[4] = CAL_PLL Locked
 *
 * \param device the Mykonos device data structure
 * \param pllLockStatus Lock status bit per PLL
 *
 * \return Returns enum MYKONOS_ERR, MYKONOS_ERR_OK=pass, !MYKONOS_ERR_OK=fail
 */
mykonosErr_t MYKONOS_checkPllsLockStatus(mykonosDevice_t *device, uint8_t *pllLockStatus)
{
    uint8_t readData = 0;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_checkPllsLockStatus()\n");
#endif

    if (pllLockStatus == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_CHECK_PLL_LOCK_NULL_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_CHECK_PLL_LOCK_NULL_PARM));
        return MYKONOS_ERR_CHECK_PLL_LOCK_NULL_PARM;
    }

    *pllLockStatus = 0;

    CMB_SPIReadField(device->spiSettings, MYKONOS_ADDR_CLK_SYNTH_VCO_BAND_BYTE1, &readData, 0x01, 0);
    *pllLockStatus = readData;

    CMB_SPIReadField(device->spiSettings, MYKONOS_ADDR_RXSYNTH_VCO_BAND_BYTE1, &readData, 0x01, 0);
    *pllLockStatus = *pllLockStatus | (readData << 1);

    CMB_SPIReadField(device->spiSettings, MYKONOS_ADDR_TXSYNTH_VCO_BAND_BYTE1, &readData, 0x01, 0);
    *pllLockStatus = *pllLockStatus | (readData << 2);

    CMB_SPIReadField(device->spiSettings, MYKONOS_ADDR_SNIFF_RXSYNTH_VCO_BAND_BYTE1, &readData, 0x01, 0);
    *pllLockStatus = *pllLockStatus | (readData << 3);

    CMB_SPIReadField(device->spiSettings, MYKONOS_ADDR_CALPLL_SDM_CONTROL, &readData, 0x80, 7);
    *pllLockStatus = *pllLockStatus | (readData << 4);

    return MYKONOS_ERR_OK;
}

/*
 *****************************************************************************
 * Shared Data path functions
 *****************************************************************************
 */

/**
 * \brief Sets the digital Tx PFIR SYNC clock divider.
 *
 * This function is a helper function.  It is called automatically in
 * MYKONOS_initialize() and should not need to be called by the BBIC.
 *
 * This function sets the digital clock divider that is used to synchronize the
 * Tx PFIR each time the Tx channel power up.  The Sync clock must
 * be set equal to or slower than the FIR processing clock.
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 * - device->spiSettings->chipSelectIndex
 * - device->profilesValid
 * - device->tx->txProfile->txFir
 * - device->tx->txProfile->txFir->numFirCoefs
 * - device->tx->txProfile->txFirInterpolation
 * - device->tx->txProfile->iqRate_kHz
 *
 * \param device Pointer to the Mykonos data structure
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_TXFIR_INV_NUMTAPS_PARM: Invalid number of Tx FIR coefficients
 * \retval MYKONOS_ERR_TXFIR_INV_NUMROWS Invalid number of PFIR coefficient rows
 * \retval MYKONOS_ERR_TXFIR_TAPSEXCEEDED Too many Tx PFIR taps for the IQ sample rate.  FIR processing clock can not run fast enough to handle the number of taps
 */
mykonosErr_t MYKONOS_setTxPfirSyncClk(mykonosDevice_t *device)
{
    const uint8_t numTapMultiple = 16;
    const uint8_t maxNumTaps = 96;
    uint8_t effectiveRows = 0;
    uint8_t numRows = 0;
    uint32_t dpClk_kHz = 0;
    uint32_t syncClk_kHz = 0;
    uint32_t hsDigClkDiv4or5_kHz = 0;
    uint8_t syncDiv = 0;

    if (((device->profilesValid & TX_PROFILE_VALID) > 0) && (device->tx->txProfile->txFir != NULL))
    {
        /* Calculate number of FIR rows for number of Taps */
        if ((device->tx->txProfile->txFir->numFirCoefs % numTapMultiple == 0) && (device->tx->txProfile->txFir->numFirCoefs > 0)
                && (device->tx->txProfile->txFir->numFirCoefs <= maxNumTaps))
        {
            numRows = (device->tx->txProfile->txFir->numFirCoefs / numTapMultiple);
        }
        else
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_TXFIR_INV_NUMTAPS_PARM,
                    getMykonosErrorMessage(MYKONOS_ERR_TXFIR_INV_NUMTAPS_PARM));
            return MYKONOS_ERR_TXFIR_INV_NUMTAPS_PARM;
        }

        effectiveRows = ((numRows + (device->tx->txProfile->txFirInterpolation - 1)) / device->tx->txProfile->txFirInterpolation);

        /* round up to next power of 2 */
        switch (effectiveRows)
        {
            case 1:
                effectiveRows = 1;
                break;
            case 2:
                effectiveRows = 2;
                break;
            case 3: /* fall through */
            case 4:
                effectiveRows = 4;
                break;
            case 5: /* fall through */
            case 6: /* fall through */
            case 7: /* fall through */
            case 8:
                effectiveRows = 8;
                break;
            default:
            {
                /* invalid number of FIR taps */
                CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_TXFIR_INV_NUMROWS,
                        getMykonosErrorMessage(MYKONOS_ERR_TXFIR_INV_NUMROWS));
                return MYKONOS_ERR_TXFIR_INV_NUMROWS;
            }
        }

        dpClk_kHz = (device->tx->txProfile->iqRate_kHz * device->tx->txProfile->txInputHbInterpolation * device->tx->txProfile->txFirInterpolation * effectiveRows);

        /* FIR DPCLK can only run at max of 500MHz */
        if (dpClk_kHz > 500000)
        {
            /* Max number of Tx PFIR taps exceeded for Tx Profile */
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_TXFIR_TAPSEXCEEDED,
                    getMykonosErrorMessage(MYKONOS_ERR_TXFIR_TAPSEXCEEDED));
            return MYKONOS_ERR_TXFIR_TAPSEXCEEDED;
        }

        /* SYNC CLOCK is the PFIR output rate / 2 */
        syncClk_kHz = device->tx->txProfile->iqRate_kHz * device->tx->txProfile->txFirInterpolation / 2;
        MYKONOS_calculateDigitalClocks(device, NULL, &hsDigClkDiv4or5_kHz);

        /* Select correct divider setting for SYNCCLK - must be == syncClk_kHz or slower */
        for (syncDiv = 0; syncDiv < 5; syncDiv++)
        {
            if ((hsDigClkDiv4or5_kHz / (4 << syncDiv)) <= syncClk_kHz)
            {
                break;
            }
        }

        /* Write Tx PFIR SYNC Clock divider */
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_CLOCK_CONTROL_5, syncDiv, 0x70, 4);
    }

    return MYKONOS_ERR_OK;
}

/**
 * \brief Sets the digital Rx PFIR SYNC clock divider.
 *
 * This function is a helper function.  It is called automatically in
 * MYKONOS_initialize() and should not need to be called by the BBIC.
 *
 * This function sets the digital clock divider that is used to synchronize the
 * Rx/ORx/Sniffer PFIRs each time the channels power up.  The Sync clock must
 * be set equal to or slower than the slowest Rx/ORx/Sniffer PFIR processing
 * clock.
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 * - device->spiSettings->chipSelectIndex
 * - device->profilesValid
 * - device->rx->rxProfile->rxFir
 * - device->rx->rxProfile->rxFir->numFirCoefs
 * - device->rx->rxProfile->iqRate_kHz
 * - device->obsRx->orxProfile->rxFir
 * - device->obsRx->orxProfile->rxFir->numFirCoefs
 * - device->obsRx->orxProfile->iqRate_kHz
 * - device->obsRx->snifferProfile->rxFir
 * - device->obsRx->snifferProfile->rxFir->numFirCoefs
 * - device->obsRx->snifferProfile->iqRate_kHz
 *
 * \param device Pointer to the Mykonos data structure
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_RXFIR_TAPSEXCEEDED ERROR: the number of Rx FIR taps exceeds the number of taps accommodated by the FIR processing clock
 * \retval MYKONOS_ERR_ORXFIR_TAPSEXCEEDED ERROR: the number of ORx FIR Taps exceeds the number of taps accommodated by the FIR processing clock
 * \retval MYKONOS_ERR_SNRXFIR_TAPSEXCEEDED ERROR: the number of sniffer FIR taps exceeds the number of taps accommodated by the FIR processing clock
 */
mykonosErr_t MYKONOS_setRxPfirSyncClk(mykonosDevice_t *device)
{
    uint32_t rxDpClk_kHz = 0;
    uint32_t orxDpClk_kHz = 0;
    uint32_t snRxDpClk_kHz = 0;
    uint32_t slowestIqRate_kHz = 0;
    uint32_t syncClk_kHz = 0;
    uint8_t effectiveRxNumRows = 0;
    uint8_t effectiveOrxNumRows = 0;
    uint8_t effectiveSnrxNumRows = 0;

    uint32_t hsDigClkDiv4or5_kHz = 0;
    uint8_t syncDiv = 0;

    /**
     * Calculate Rx SYNC Clock divider for Rx PFIR.  Same Rx SYNC Clock is used
     * for Rx, ORx, and Sniffer PFIRs, and must be slow enough to handle the slowest
     * PFIR rate.
     **/

    if (((device->profilesValid & RX_PROFILE_VALID) > 0) && (device->rx->rxProfile->rxFir != NULL))
    {
        switch (device->rx->rxProfile->rxFir->numFirCoefs)
        {
            case 24:
                effectiveRxNumRows = 1;
                break;
            case 48:
                effectiveRxNumRows = 2;
                break;
            case 72:
                effectiveRxNumRows = 4;
                break;
        }

        rxDpClk_kHz = device->rx->rxProfile->iqRate_kHz * effectiveRxNumRows;

        if (rxDpClk_kHz > 500000)
        {
            /* Max number of Rx PFIR taps exceeded for Profile */
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_RXFIR_TAPSEXCEEDED,
                    getMykonosErrorMessage(MYKONOS_ERR_RXFIR_TAPSEXCEEDED));
            return MYKONOS_ERR_RXFIR_TAPSEXCEEDED;
        }

        slowestIqRate_kHz = device->rx->rxProfile->iqRate_kHz;
    }

    if (((device->profilesValid & ORX_PROFILE_VALID) > 0) && (device->obsRx->orxProfile->rxFir != NULL))
    {
        switch (device->obsRx->orxProfile->rxFir->numFirCoefs)
        {
            case 24:
                effectiveOrxNumRows = 1;
                break;
            case 48:
                effectiveOrxNumRows = 2;
                break;
            case 72:
                effectiveOrxNumRows = 4;
                break;
        }

        orxDpClk_kHz = device->obsRx->orxProfile->iqRate_kHz * effectiveOrxNumRows;

        if (orxDpClk_kHz > 500000)
        {
            /* Max number of ORx PFIR taps exceeded for Profile */
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_ORXFIR_TAPSEXCEEDED,
                    getMykonosErrorMessage(MYKONOS_ERR_ORXFIR_TAPSEXCEEDED));
            return MYKONOS_ERR_ORXFIR_TAPSEXCEEDED;
        }

        if ((slowestIqRate_kHz == 0) || (slowestIqRate_kHz > device->obsRx->orxProfile->iqRate_kHz))
        {
            slowestIqRate_kHz = device->obsRx->orxProfile->iqRate_kHz;

        }
    }

    if (((device->profilesValid & SNIFF_PROFILE_VALID) > 0) && (device->obsRx->snifferProfile->rxFir != NULL))
    {
        switch (device->obsRx->snifferProfile->rxFir->numFirCoefs)
        {
            case 24:
                effectiveSnrxNumRows = 1;
                break;
            case 48:
                effectiveSnrxNumRows = 2;
                break;
            case 72:
                effectiveSnrxNumRows = 4;
                break;
        }

        snRxDpClk_kHz = device->obsRx->snifferProfile->iqRate_kHz * effectiveSnrxNumRows;

        if (snRxDpClk_kHz > 500000)
        {
            /* Max number of ORx PFIR taps exceeded for Profile */
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SNRXFIR_TAPSEXCEEDED,
                    getMykonosErrorMessage(MYKONOS_ERR_SNRXFIR_TAPSEXCEEDED));
            return MYKONOS_ERR_SNRXFIR_TAPSEXCEEDED;
        }

        if ((slowestIqRate_kHz == 0) || (slowestIqRate_kHz > device->obsRx->snifferProfile->iqRate_kHz))
        {
            slowestIqRate_kHz = device->obsRx->snifferProfile->iqRate_kHz;
        }
    }

    /* SYNC CLOCK should be FIR output rate / 2 */
    syncClk_kHz = (slowestIqRate_kHz / 2);
    MYKONOS_calculateDigitalClocks(device, NULL, &hsDigClkDiv4or5_kHz);

    /* Select correct divider setting for SYNCCLK - must be == syncClk_kHz or slower */
    for (syncDiv = 0; syncDiv < 5; syncDiv++)
    {
        if ((hsDigClkDiv4or5_kHz / (4 << syncDiv)) <= syncClk_kHz)
        {
            break;
        }
    }

    /* Write Rx PFIR SYNC Clock divider */
    CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_CLOCK_CONTROL_5, syncDiv, 0x07, 0);

    return MYKONOS_ERR_OK;
}

/**
 * \brief Configures one or more FIR filters in the device
 *
 * The device stores up to 6 FIR filters (2Rx, 2Obs Rx/Sniffer, and 2Tx).
 * Rx filters can have 24, 48, or 72 taps.  Tx filters can have 16, 32,
 * 48, 64, 80, or 96 taps.
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 * - device->spiSettings->chipSelectIndex
 *
 * \param device Pointer to the Mykonos data structure
 * \param filterToProgram Name of the desired filter to program
 * \param firFilter Pointer to the filter to write into the device
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_PROGRAMFIR_NULL_PARM ERROR: firFilter parameter is a NULL pointer
 * \retval MYKONOS_ERR_PROGRAMFIR_COEFS_NULL ERROR: firFilter->coefs is a NULL pointer
 * \retval MYKONOS_ERR_PROGRAMFIR_INV_FIRNAME_PARM ERROR: Invalid FIR filter name in filterToProgram parameter
 * \retval MYKONOS_ERR_PROGRAMFIR_INV_NUMTAPS_PARM ERROR: Invalid number of taps for the filter
 * \retval MYKONOS_ERR_RXFIR_INV_GAIN_PARM ERROR: Rx FIR filter has invalid gain setting
 * \retval MYKONOS_ERR_OBSRXFIR_INV_GAIN_PARM ERROR: OBSRX_A (ORX) FIR filter has invalid gain setting
 * \retval MYKONOS_ERR_SRXFIR_INV_GAIN_PARM ERROR: OBSRX_B (Sniffer) FIR filter has invalid gain setting
 * \retval MYKONOS_ERR_TXFIR_INV_GAIN_PARM ERROR: Tx FIR filter has invalid gain setting
 */
mykonosErr_t MYKONOS_programFir(mykonosDevice_t *device, mykonosfirName_t filterToProgram, mykonosFir_t *firFilter)
{
    uint8_t filterSelect = 0;
    uint8_t i = 0;
    uint8_t numTapsReg = 0;
    uint8_t numTapMultiple = 24; /* Rx=24, Tx=16 */
    uint8_t maxNumTaps = 72; /* Rx=72, Tx=96 */
    uint8_t filterGain = 0;

#if MYK_ENABLE_SPIWRITEARRAY == 1
    uint32_t addrIndex = 0;
    uint32_t dataIndex = 0;
    uint32_t spiBufferSize = ((MYK_SPIWRITEARRAY_BUFFERSIZE / 6) * 6); /* Make buffer size a multiple of 6 */
    uint16_t addrArray[MYK_SPIWRITEARRAY_BUFFERSIZE] = {0};
    uint8_t dataArray[MYK_SPIWRITEARRAY_BUFFERSIZE] = {0};
#endif

    const uint8_t COEF_WRITE_EN = 0x40;
    const uint8_t PROGRAM_CLK_EN = 0x80;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_programFir()\n");
#endif

    if (firFilter == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_PROGRAMFIR_NULL_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_PROGRAMFIR_NULL_PARM));
        return MYKONOS_ERR_PROGRAMFIR_NULL_PARM;
    }

    if (firFilter->coefs == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_PROGRAMFIR_COEFS_NULL,
                getMykonosErrorMessage(MYKONOS_ERR_PROGRAMFIR_COEFS_NULL));
        return MYKONOS_ERR_PROGRAMFIR_COEFS_NULL;
    }

    switch (filterToProgram)
    {
        case TX1_FIR:
            filterSelect = 0x01;
            numTapMultiple = 16;
            maxNumTaps = 96;
            break;
        case TX2_FIR:
            filterSelect = 0x02;
            numTapMultiple = 16;
            maxNumTaps = 96;
            break;
        case TX1TX2_FIR:
            filterSelect = 0x03;
            numTapMultiple = 16;
            maxNumTaps = 96;
            break;
        case RX1_FIR:
            filterSelect = 0x04;
            numTapMultiple = 24;
            maxNumTaps = 72;
            break;
        case RX2_FIR:
            filterSelect = 0x08;
            numTapMultiple = 24;
            maxNumTaps = 72;
            break;
        case RX1RX2_FIR:
            filterSelect = 0x0C;
            numTapMultiple = 24;
            maxNumTaps = 72;
            break;
        case OBSRX_A_FIR:
            filterSelect = 0x10;
            numTapMultiple = 24;
            maxNumTaps = 72;
            break;
        case OBSRX_B_FIR:
            filterSelect = 0x20;
            numTapMultiple = 24;
            maxNumTaps = 72;
            break;
        default:
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_PROGRAMFIR_INV_FIRNAME_PARM,
                    getMykonosErrorMessage(MYKONOS_ERR_PROGRAMFIR_INV_FIRNAME_PARM));
            return MYKONOS_ERR_PROGRAMFIR_INV_FIRNAME_PARM;
    }

    /* Calculate register value for number of Taps */
    if ((firFilter->numFirCoefs % numTapMultiple == 0) && (firFilter->numFirCoefs > 0) && (firFilter->numFirCoefs <= maxNumTaps))
    {
        numTapsReg = (firFilter->numFirCoefs / numTapMultiple) - 1;
    }
    else
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_PROGRAMFIR_INV_NUMTAPS_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_PROGRAMFIR_INV_NUMTAPS_PARM));
        return MYKONOS_ERR_PROGRAMFIR_INV_NUMTAPS_PARM;
    }

    /* Select which FIR filter to program coeffs for */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_PFIR_COEFF_CTL, (PROGRAM_CLK_EN | filterSelect)); //select filter and enable the programming clock

#if (MYK_ENABLE_SPIWRITEARRAY == 0)

    /* write filter coefficients */
    for (i = 0; i < firFilter->numFirCoefs; i++)
    {
        /* Write Low byte of 16bit coefficient */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_PFIR_COEFF_ADDR, (i * 2));
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_PFIR_COEFF_DATA, (firFilter->coefs[i] & 0xFF));
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_PFIR_COEFF_CTL, (PROGRAM_CLK_EN | COEF_WRITE_EN | filterSelect)); /* write enable (self clearing) */

        /* Write High Byte of 16bit coefficient */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_PFIR_COEFF_ADDR, ((i * 2) + 1));
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_PFIR_COEFF_DATA, ((firFilter->coefs[i] >> 8) & 0xFF));
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_PFIR_COEFF_CTL, (PROGRAM_CLK_EN | COEF_WRITE_EN | filterSelect));/* write enable (self clearing) */
    }

#elif (MYK_ENABLE_SPIWRITEARRAY == 1)

    addrIndex = 0;
    dataIndex = 0;
    for (i = 0; i < firFilter->numFirCoefs; i++)
    {
        addrArray[addrIndex++] = MYKONOS_ADDR_PFIR_COEFF_ADDR;
        addrArray[addrIndex++] = MYKONOS_ADDR_PFIR_COEFF_DATA;
        addrArray[addrIndex++] = MYKONOS_ADDR_PFIR_COEFF_CTL;
        addrArray[addrIndex++] = MYKONOS_ADDR_PFIR_COEFF_ADDR;
        addrArray[addrIndex++] = MYKONOS_ADDR_PFIR_COEFF_DATA;
        addrArray[addrIndex++] = MYKONOS_ADDR_PFIR_COEFF_CTL;

        dataArray[dataIndex++] = (i * 2);
        dataArray[dataIndex++] = (firFilter->coefs[i] & 0xFF);
        dataArray[dataIndex++] = (PROGRAM_CLK_EN | COEF_WRITE_EN | filterSelect);
        dataArray[dataIndex++] = ((i * 2) + 1);
        dataArray[dataIndex++] = ((firFilter->coefs[i] >> 8) & 0xFF);
        dataArray[dataIndex++] = (PROGRAM_CLK_EN | COEF_WRITE_EN | filterSelect);

        /* Send full buffer size when possible */
        /* spiBufferSize set to multiple of 6 at top of function */
        if (addrIndex >= spiBufferSize)
        {
            CMB_SPIWriteBytes(device->spiSettings, &addrArray[0], &dataArray[0], addrIndex);
            dataIndex = 0;
            addrIndex = 0;
        }
    }

    if (addrIndex > 0)
    {
        CMB_SPIWriteBytes(device->spiSettings, &addrArray[0], &dataArray[0], addrIndex);
    }

#endif

    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_PFIR_COEFF_CTL, filterSelect); /* clear Program clock enable */
    /* write filter gain and #taps */
    if (filterToProgram == RX1_FIR || filterToProgram == RX2_FIR || filterToProgram == RX1RX2_FIR)
    {
        /* Write Rx FIR #taps */
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_RX_FILTER_CONFIGURATION, numTapsReg, 0x60, 5);

        /* Set Rx filter gain */
        switch (firFilter->gain_dB)
        {
            case -12:
                filterGain = 0x00;
                break;
            case -6:
                filterGain = 0x01;
                break;
            case 0:
                filterGain = 0x02;
                break;
            case 6:
                filterGain = 0x03;
                break;
            default:
                CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_RXFIR_INV_GAIN_PARM,
                        getMykonosErrorMessage(MYKONOS_ERR_RXFIR_INV_GAIN_PARM));
                return MYKONOS_ERR_RXFIR_INV_GAIN_PARM;
        }

        /* Write Rx filter gain */
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_RX_FILTER_GAIN, filterGain, 0x03, 0);
    }
    else if (filterToProgram == OBSRX_A_FIR)
    {
        /* Write Obs Rx FIR A #taps */
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_RX_FILTER_CONFIGURATION, numTapsReg, 0x06, 1);

        /* Set Rx filter gain */
        switch (firFilter->gain_dB)
        {
            case -12:
                filterGain = 0x00;
                break;
            case -6:
                filterGain = 0x01;
                break;
            case 0:
                filterGain = 0x02;
                break;
            case 6:
                filterGain = 0x03;
                break;
            default:
                CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OBSRXFIR_INV_GAIN_PARM,
                        getMykonosErrorMessage(MYKONOS_ERR_OBSRXFIR_INV_GAIN_PARM));
                return MYKONOS_ERR_OBSRXFIR_INV_GAIN_PARM;
        }

        /* Write Obs Rx filter gain */
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_DPD_SNIFFER_RX_FILTER_GAIN, filterGain, 0x03, 0);
    }
    else if (filterToProgram == OBSRX_B_FIR)
    {
        /* Write Obs Rx FIR B #taps */
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_RX_FILTER_CONFIGURATION, numTapsReg, 0x18, 3);

        /* Set Rx filter gain */
        switch (firFilter->gain_dB)
        {
            case -12:
                filterGain = 0x00;
                break;
            case -6:
                filterGain = 0x01;
                break;
            case 0:
                filterGain = 0x02;
                break;
            case 6:
                filterGain = 0x03;
                break;
            default:
                CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SRXFIR_INV_GAIN_PARM,
                        getMykonosErrorMessage(MYKONOS_ERR_SRXFIR_INV_GAIN_PARM));
                return MYKONOS_ERR_SRXFIR_INV_GAIN_PARM;
        }

        /* Write Obs Rx filter gain */
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_DPD_SNIFFER_RX_FILTER_GAIN, filterGain, 0x60, 5);
    }
    else if (filterToProgram == TX1_FIR || filterToProgram == TX2_FIR || filterToProgram == TX1TX2_FIR)
    {
        /* Write number of Taps */
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_TX_FILTER_CONFIGURATION, numTapsReg, 0xE0, 5);

        switch (firFilter->gain_dB)
        {
            case 0:
                filterGain = 0;
                break;
            case 6:
                filterGain = 1;
                break;
            default:
                CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_TXFIR_INV_GAIN_PARM,
                        getMykonosErrorMessage(MYKONOS_ERR_TXFIR_INV_GAIN_PARM));
                return MYKONOS_ERR_TXFIR_INV_GAIN_PARM;
        }

        /* Write Tx Filter gain */
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_TX_FILTER_CONFIGURATION, filterGain, 0x01, 0);

    }

    return MYKONOS_ERR_OK;
}

/**
 * \brief Reads the FIR filter programmed into the device
 *
 * The device stores up to 6 FIR filters (2Rx, 2Obs Rx/Sniffer, and 2Tx).
 * Rx filters can have 24, 48, or 72 taps.  Tx filters can have 16, 32,
 * 48, 64, 80, or 96 taps.
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 * - device->spiSettings->chipSelectIndex
 *
 * \param device Pointer to the Mykonos data structure
 * \param filterToRead Name of the desired filter to be read
 * \param firFilter Pointer to the filter to be read from the device
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_READFIR_NULL_PARM ERROR: firFilter parameter is a NULL pointer
 * \retval MYKONOS_ERR_READFIR_COEFS_NULL ERROR: firFilter->coefs is a NULL pointer
 * \retval MYKONOS_ERR_READFIR_INV_FIRNAME_PARM ERROR: Invalid FIR filter name in filterToRead parameter
 * \retval MYKONOS_ERR_READFIR_INV_NUMTAPS_PARM ERROR: Invalid number of taps for the filter
 * \retval MYKONOS_ERR_RXFIR_INV_GAIN_PARM ERROR: Rx FIR filter has invalid gain setting
 * \retval MYKONOS_ERR_OBSRXFIR_INV_GAIN_PARM ERROR: OBSRX_A (ORX) FIR filter has invalid gain setting
 * \retval MYKONOS_ERR_SRXFIR_INV_GAIN_PARM ERROR: OBSRX_B (Sniffer) FIR filter has invalid gain setting
 * \retval MYKONOS_ERR_TXFIR_INV_GAIN_PARM ERROR: Tx FIR filter has invalid gain setting
 */
mykonosErr_t MYKONOS_readFir(mykonosDevice_t *device, mykonosfirName_t filterToRead, mykonosFir_t *firFilter)
{
    uint8_t filterSelect = 0;
    uint8_t i = 0;
    uint8_t numTapsReg = 0;
    uint8_t numTapMultiple = 24;
    uint8_t maxNumTaps = 72;
    uint8_t filterGain = 0;
    uint8_t msbRead = 0;
    uint8_t lsbRead = 0;

    const uint8_t PROGRAM_CLK_EN = 0x80;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_readFir()\n");
#endif

    if (firFilter == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_READFIR_NULL_PARM, getMykonosErrorMessage(MYKONOS_ERR_READFIR_NULL_PARM));
        return MYKONOS_ERR_READFIR_NULL_PARM;
    }

    if (firFilter->coefs == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_READFIR_COEFS_NULL, getMykonosErrorMessage(MYKONOS_ERR_READFIR_COEFS_NULL));
        return MYKONOS_ERR_READFIR_COEFS_NULL;
    }

    switch (filterToRead)
    {
        case TX1_FIR:
            filterSelect = 0x01;
            numTapMultiple = 16;
            maxNumTaps = 96;
            break;
        case TX2_FIR:
            filterSelect = 0x02;
            numTapMultiple = 16;
            maxNumTaps = 96;
            break;
        case TX1TX2_FIR:
            filterSelect = 0x03;
            numTapMultiple = 16;
            maxNumTaps = 96;
            break;
        case RX1_FIR:
            filterSelect = 0x04;
            numTapMultiple = 24;
            maxNumTaps = 72;
            break;
        case RX2_FIR:
            filterSelect = 0x08;
            numTapMultiple = 24;
            maxNumTaps = 72;
            break;
        case RX1RX2_FIR:
            filterSelect = 0x0C;
            numTapMultiple = 24;
            maxNumTaps = 72;
            break;
        case OBSRX_A_FIR:
            filterSelect = 0x10;
            numTapMultiple = 24;
            maxNumTaps = 72;
            break;
        case OBSRX_B_FIR:
            filterSelect = 0x20;
            numTapMultiple = 24;
            maxNumTaps = 72;
            break;
        default:
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_READFIR_INV_FIRNAME_PARM,
                    getMykonosErrorMessage(MYKONOS_ERR_READFIR_INV_FIRNAME_PARM));
            return MYKONOS_ERR_READFIR_INV_FIRNAME_PARM;
    }

    /* clear Program clock enable */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_PFIR_COEFF_CTL, filterSelect);

    /* Read filter gain and #taps */
    if (filterToRead == RX1_FIR || filterToRead == RX2_FIR || filterToRead == RX1RX2_FIR)
    {
        /* Read Rx FIR #taps */
        CMB_SPIReadField(device->spiSettings, MYKONOS_ADDR_RX_FILTER_CONFIGURATION, &numTapsReg, 0x60, 5);

        firFilter->numFirCoefs = (numTapsReg + 1) * numTapMultiple;

        /* Read Rx filter gain */
        CMB_SPIReadField(device->spiSettings, MYKONOS_ADDR_RX_FILTER_GAIN, &filterGain, 0x03, 0);

        switch (filterGain)
        {
            case 0x00:
                firFilter->gain_dB = -12;
                break;
            case 0x01:
                firFilter->gain_dB = -6;
                break;
            case 0x02:
                firFilter->gain_dB = 0;
                break;
            case 0x03:
                firFilter->gain_dB = 6;
                break;
            default:
                CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_RXFIR_INV_GAIN_PARM,
                        getMykonosErrorMessage(MYKONOS_ERR_RXFIR_INV_GAIN_PARM));
                return MYKONOS_ERR_RXFIR_INV_GAIN_PARM;
        }
    }
    else if (filterToRead == OBSRX_A_FIR)
    {
        /* Read Obs Rx FIR A #taps */
        CMB_SPIReadField(device->spiSettings, MYKONOS_ADDR_RX_FILTER_CONFIGURATION, &numTapsReg, 0x60, 1);

        firFilter->numFirCoefs = (numTapsReg + 1) * numTapMultiple;

        /* Read Obs Rx FIR A gain */
        CMB_SPIReadField(device->spiSettings, MYKONOS_ADDR_DPD_SNIFFER_RX_FILTER_GAIN, &filterGain, 0x03, 0);

        switch (filterGain)
        {
            case 0x00:
                firFilter->gain_dB = -12;
                break;
            case 0x01:
                firFilter->gain_dB = -6;
                break;
            case 0x02:
                firFilter->gain_dB = 0;
                break;
            case 0x03:
                firFilter->gain_dB = 6;
                break;
            default:
                CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OBSRXFIR_INV_GAIN_PARM,
                        getMykonosErrorMessage(MYKONOS_ERR_OBSRXFIR_INV_GAIN_PARM));
                return MYKONOS_ERR_OBSRXFIR_INV_GAIN_PARM;
        }
    }
    else if (filterToRead == OBSRX_B_FIR)
    {
        /* Read Obs Rx FIR B #taps */
        CMB_SPIReadField(device->spiSettings, MYKONOS_ADDR_RX_FILTER_CONFIGURATION, &numTapsReg, 0x18, 3);

        firFilter->numFirCoefs = (numTapsReg + 1) * numTapMultiple;

        /* Read Obs Rx FIR B gain */
        CMB_SPIReadField(device->spiSettings, MYKONOS_ADDR_DPD_SNIFFER_RX_FILTER_GAIN, &filterGain, 0x60, 5);

        switch (filterGain)
        {
            case 0x00:
                firFilter->gain_dB = -12;
                break;
            case 0x01:
                firFilter->gain_dB = -6;
                break;
            case 0x02:
                firFilter->gain_dB = 0;
                break;
            case 0x03:
                firFilter->gain_dB = 6;
                break;
            default:
                CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SRXFIR_INV_GAIN_PARM,
                        getMykonosErrorMessage(MYKONOS_ERR_SRXFIR_INV_GAIN_PARM));
                return MYKONOS_ERR_SRXFIR_INV_GAIN_PARM;
        }
    }
    else if (filterToRead == TX1_FIR || filterToRead == TX2_FIR || filterToRead == TX1TX2_FIR)
    {
        /* Read number of Taps */
        CMB_SPIReadField(device->spiSettings, MYKONOS_ADDR_TX_FILTER_CONFIGURATION, &numTapsReg, 0xE0, 5);

        firFilter->numFirCoefs = (numTapsReg + 1) * numTapMultiple;

        /* Read Tx Filter gain */
        CMB_SPIReadField(device->spiSettings, MYKONOS_ADDR_TX_FILTER_CONFIGURATION, &filterGain, 0x01, 0);

        switch (filterGain)
        {
            case 0:
                firFilter->gain_dB = 0;
                break;
            case 1:
                firFilter->gain_dB = 6;
                break;
            default:
                CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_TXFIR_INV_GAIN_PARM,
                        getMykonosErrorMessage(MYKONOS_ERR_TXFIR_INV_GAIN_PARM));
                return MYKONOS_ERR_TXFIR_INV_GAIN_PARM;
        }
    }

    /* Select which FIR filter to read the coeffs for */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_PFIR_COEFF_CTL, (PROGRAM_CLK_EN | filterSelect)); //select filter and enable the programming clock

    if (firFilter->numFirCoefs > maxNumTaps)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_READFIR_INV_NUMTAPS_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_READFIR_INV_NUMTAPS_PARM));
        return MYKONOS_ERR_READFIR_INV_NUMTAPS_PARM;
    }

    /* write filter coefficients */
    for (i = 0; i < firFilter->numFirCoefs; i++)
    {
        /* Write Low byte of 16bit coefficient */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_PFIR_COEFF_ADDR, (i * 2));
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_PFIR_COEFF_CTL, (PROGRAM_CLK_EN | filterSelect)); /* write enable (self clearing) */
        CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_PFIR_COEFF_DATA, &lsbRead);

        /* Write High Byte of 16bit coefficient */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_PFIR_COEFF_ADDR, ((i * 2) + 1));
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_PFIR_COEFF_CTL, (PROGRAM_CLK_EN | filterSelect));/* write enable (self clearing) */
        CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_PFIR_COEFF_DATA, &msbRead);

        firFilter->coefs[i] = (int16_t)((lsbRead & 0xFF) | ((msbRead << 8) & 0xFF00));
    }

    return MYKONOS_ERR_OK;
}

/*
 *****************************************************************************
 * Rx Data path functions
 *****************************************************************************
 */
/*!
 * \brief Programs the gain table settings for either Rx1, Rx2, Rx1 + Rx2, ORx, or SnRx receiver types
 *
 * The gain table for a receiver type is set with the parameters passed by uint8_t gainTablePtr array.
 * gainTablePtr is a 4 x n array, where there are four (4) elements per index, and the array
 * length (n) is dependent upon receiver type. The (n) value is conveyed by numGainIndexesInTable.
 * All gain tables have a maximum index of 255 when used with this function. The minimum gain index is
 * application dependent.
 *
 * Where for Rx1, Rx2, and ObsRx:
 * [A, B, C, D]: A = Front End Gain, B = External Control, C = Digital Attenuation/Gain, D = Attenuation/Gain select
 *
 * Where for SnRx:
 * [A, B, C, D]: A = Front End Gain, B = LNA Bypass, C = Digital Attenuation/Gain, D = Attenuation/Gain select
 *
 * The gain table starting address changes with each receiver type. This function accounts for this change as well as the
 * difference between byte [B] for {Rx1, Rx2, ObsRx} and SnRx receiver array values and programs the correct registers.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device Pointer to the Mykonos data structure
 * \param gainTablePtr Pointer to 4 x n array containing gain table program values
 * \param numGainIndexesInTable The number of 'n' indices in 4 x n array.  A range check is performed to ensure the maximum is not exceeded.
 * \param rxChannel mykonosGainTable_t enum type to select either Rx1, Rx2, Rx1 + Rx2, ORx, or SnRx gain table for programming.  A
 * channel check is performed to ensure a valid selection.
 *
 * \return Returns enum MYKONOS_ERR, MYKONOS_ERR_OK if successful, MYKONOS_ERR_RXGAINTABLE_INV_CHANNEL if invalid channel is selected, MYKONOS_ERR_RXGAINTABLE_INV_GAIN_INDEX_RANGE
 * if numGainIndexesInTable exceeds range for selected receiver gain table
 */
mykonosErr_t MYKONOS_programRxGainTable(mykonosDevice_t *device, uint8_t *gainTablePtr, uint8_t numGainIndexesInTable, mykonosGainTable_t rxChannel)
{
    uint8_t ctlReg = 0;
    uint8_t ch3CtlReg = 0;
    uint16_t rxFEGainAddr = 0;
    uint16_t rxExtCtlLnaAddr = 0;
    uint16_t rxDigGainAttenAddr = 0;
    uint16_t rx2FEGainAddr = 0;
    uint16_t rx2ExtCtlAddr = 0;
    uint16_t rx2DigGainAttenAddr = 0;
    uint8_t minGainIndex = 0;
    uint8_t startIndex = 0;
    int16_t i = 0;
    uint16_t tableRowIndex = 0;
    uint8_t retFlag = 0;

#if MYK_ENABLE_SPIWRITEARRAY
    uint32_t addrIndex = 0;
    uint32_t dataIndex = 0;
    uint32_t spiBufferSize = (((MYK_SPIWRITEARRAY_BUFFERSIZE / 9) - 1) * 9);
    uint16_t addrArray[MYK_SPIWRITEARRAY_BUFFERSIZE] = {0};
    uint8_t dataArray[MYK_SPIWRITEARRAY_BUFFERSIZE] = {0};
#endif

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_programRxGainTable()\n");
#endif

    if (gainTablePtr == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_PROGRAM_RXGAIN_TABLE_NULL_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_PROGRAM_RXGAIN_TABLE_NULL_PARM));
        return MYKONOS_ERR_PROGRAM_RXGAIN_TABLE_NULL_PARM;
    }

    /* checking range of numGainIndexesInTable against maximums and for valid rxChannel selection */
    switch (rxChannel)
    {
        case RX1_GT:
            /* Rx1 max gain index = 255, check is out of range of uint8_t */
            break;
        case RX2_GT:
            /* Rx1 max gain index = 255, check is out of range of uint8_t */
            break;
        case RX1_RX2_GT:
            /* Rx1 max gain index = 255, check is out of range of uint8_t */
            break;
        case ORX_GT:
            if (numGainIndexesInTable > MAX_ORX_GAIN_TABLE_NUMINDEXES)
            {
                retFlag = 1;
            }
            break;
        case SNRX_GT:
            if (numGainIndexesInTable > MAX_SNRX_GAIN_TABLE_NUMINDEXES)
            {
                retFlag = 1;
            }
            break;
        case LOOPBACK_GT:
            if (numGainIndexesInTable > MAX_LOOPBACK_GAIN_TABLE_NUMINDEXES)
            {
                retFlag = 1;
            }
            break;
        default:
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_RXGAINTABLE_INV_CHANNEL,
                    getMykonosErrorMessage(MYKONOS_ERR_RXGAINTABLE_INV_CHANNEL));
            return MYKONOS_ERR_RXGAINTABLE_INV_CHANNEL;
    }

    if (retFlag)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_RXGAINTABLE_INV_GAIN_INDEX_RANGE,
                getMykonosErrorMessage(MYKONOS_ERR_RXGAINTABLE_INV_GAIN_INDEX_RANGE));
        return MYKONOS_ERR_RXGAINTABLE_INV_GAIN_INDEX_RANGE;
    }
    else
    {
        /* calculating minimum gain index value */
        minGainIndex = MAX_GAIN_TABLE_INDEX - numGainIndexesInTable + 1;
    }

    /* forming control register words based on channel select, assigning starting index, and register addressing, also updating max and min gain table indices in structure */
    switch (rxChannel)
    {
        case RX1_GT:
            ctlReg = (rxChannel << 3) | 0x05;
            startIndex = START_RX_GAIN_INDEX;
            rxFEGainAddr = MYKONOS_ADDR_GAIN_TABLE_RX1_FE_GAIN;
            rxExtCtlLnaAddr = MYKONOS_ADDR_GAIN_TABLE_RX1_EXT_CTL;
            rxDigGainAttenAddr = MYKONOS_ADDR_GAIN_TABLE_RX1_DIG_GAIN;
            device->rx->rxGainCtrl->rx1MaxGainIndex = MAX_GAIN_TABLE_INDEX;
            device->rx->rxGainCtrl->rx1MinGainIndex = minGainIndex;
            break;
        case RX2_GT:
            ctlReg = (rxChannel << 3) | 0x05;
            startIndex = START_RX_GAIN_INDEX;
            rxFEGainAddr = MYKONOS_ADDR_GAIN_TABLE_RX2_FE_GAIN;
            rxExtCtlLnaAddr = MYKONOS_ADDR_GAIN_TABLE_RX2_EXT_CTL;
            rxDigGainAttenAddr = MYKONOS_ADDR_GAIN_TABLE_RX2_DIG_GAIN;
            device->rx->rxGainCtrl->rx2MaxGainIndex = MAX_GAIN_TABLE_INDEX;
            device->rx->rxGainCtrl->rx2MinGainIndex = minGainIndex;
            break;
        case RX1_RX2_GT:
            ctlReg = (rxChannel << 3) | 0x05;
            startIndex = START_RX_GAIN_INDEX;
            rxFEGainAddr = MYKONOS_ADDR_GAIN_TABLE_RX1_FE_GAIN;
            rx2FEGainAddr = MYKONOS_ADDR_GAIN_TABLE_RX2_FE_GAIN;
            rxExtCtlLnaAddr = MYKONOS_ADDR_GAIN_TABLE_RX1_EXT_CTL;
            rx2ExtCtlAddr = MYKONOS_ADDR_GAIN_TABLE_RX2_EXT_CTL;
            rxDigGainAttenAddr = MYKONOS_ADDR_GAIN_TABLE_RX1_DIG_GAIN;
            rx2DigGainAttenAddr = MYKONOS_ADDR_GAIN_TABLE_RX2_DIG_GAIN;
            device->rx->rxGainCtrl->rx1MaxGainIndex = MAX_GAIN_TABLE_INDEX;
            device->rx->rxGainCtrl->rx1MinGainIndex = minGainIndex;
            device->rx->rxGainCtrl->rx2MaxGainIndex = MAX_GAIN_TABLE_INDEX;
            device->rx->rxGainCtrl->rx2MinGainIndex = minGainIndex;
            break;
        case ORX_GT:
            ctlReg = 0x05;
            ch3CtlReg = 0x08;
            startIndex = START_ORX_GAIN_INDEX;
            rxFEGainAddr = MYKONOS_ADDR_GAIN_TABLE_RX3_FE_GAIN;
            rxExtCtlLnaAddr = MYKONOS_ADDR_GAIN_TABLE_RX3_LNA_ENAB;
            rxDigGainAttenAddr = MYKONOS_ADDR_GAIN_TABLE_RX3_DIG_GAIN;
            device->obsRx->orxGainCtrl->maxGainIndex = MAX_GAIN_TABLE_INDEX;
            device->obsRx->orxGainCtrl->minGainIndex = minGainIndex;
            break;
        case SNRX_GT:
            ctlReg = 0x05;
            ch3CtlReg = 0x00;
            startIndex = START_SNRX_GAIN_INDEX;
            rxFEGainAddr = MYKONOS_ADDR_GAIN_TABLE_RX3_FE_GAIN;
            rxExtCtlLnaAddr = MYKONOS_ADDR_GAIN_TABLE_RX3_LNA_ENAB;
            rxDigGainAttenAddr = MYKONOS_ADDR_GAIN_TABLE_RX3_DIG_GAIN;
            device->obsRx->snifferGainCtrl->maxGainIndex = MAX_GAIN_TABLE_INDEX;
            device->obsRx->snifferGainCtrl->minGainIndex = minGainIndex;
            break;
        case LOOPBACK_GT: /* Loopback is only for ARM calibrations */
            ctlReg = 0x05;
            ch3CtlReg = 0x10;
            startIndex = START_LOOPBACK_GAIN_INDEX;
            rxFEGainAddr = MYKONOS_ADDR_GAIN_TABLE_RX3_FE_GAIN;
            rxExtCtlLnaAddr = MYKONOS_ADDR_GAIN_TABLE_RX3_LNA_ENAB;
            rxDigGainAttenAddr = MYKONOS_ADDR_GAIN_TABLE_RX3_DIG_GAIN;
            break;
    }

    /* starting the gain table clock and read from gain table address bits */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_GAIN_TABLE_CONFIGURATION, ctlReg);

    /* if ORx or Sniffer are selected also writing channel 3 readback bits for ObsRx or Sniffer selection */
    if (rxChannel > RX1_RX2_GT)
    {
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_CH3_GAIN_TABLE_CONFIGURATION, ch3CtlReg);
    }

    /* programming a table selected by rxChannel enum type */

#if MYK_ENABLE_SPIWRITEARRAY == 0

    for(i = startIndex; i >= ((startIndex + 1) - numGainIndexesInTable); i--)
    {
        tableRowIndex = (uint16_t)(startIndex - i) << 2;

        /* set current gain table index (address) */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_GAIN_TABLE_ADDR, i);

        /* Set Rx Front End gain[5:0] */
        CMB_SPIWriteByte(device->spiSettings, rxFEGainAddr, gainTablePtr[tableRowIndex]);

        /* Set external control [5:0] OR LNA bypass if rxChannel == SNRX_GT */
        if (rxChannel == SNRX_GT)
        {
            CMB_SPIWriteByte(device->spiSettings, rxExtCtlLnaAddr, gainTablePtr[tableRowIndex + 1] << 4);
        }
        else
        {
            CMB_SPIWriteByte(device->spiSettings, rxExtCtlLnaAddr, gainTablePtr[tableRowIndex + 1]);
        }

        /* Set digital attenuation/gain[6:0] and set/clear attenuation bit */
        CMB_SPIWriteByte(device->spiSettings, rxDigGainAttenAddr, (gainTablePtr[tableRowIndex + 3] << 7) | gainTablePtr[tableRowIndex + 2]);

        /* repeating gain table settings if Rx1 and Rx2 are selected for Rx2 configuration */
        if (rxChannel == RX1_RX2_GT)
        {
            /* Set Rx Front End gain[5:0] */
            CMB_SPIWriteByte(device->spiSettings, rx2FEGainAddr, gainTablePtr[tableRowIndex]);

            /* Set external control [5:0] */
            CMB_SPIWriteByte(device->spiSettings, rx2ExtCtlAddr, gainTablePtr[tableRowIndex + 1]);

            /* Set digital attenuation/gain[6:0] */
            CMB_SPIWriteByte(device->spiSettings, rx2DigGainAttenAddr, (gainTablePtr[tableRowIndex + 3] << 7) | gainTablePtr[tableRowIndex + 2]);
        }

        /* setting the write enable depending on rxChannel choice */
        if (rxChannel == ORX_GT)
        {
            CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_CH3_GAIN_TABLE_CONFIGURATION, ch3CtlReg | 0x02);
            CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_GAIN_TABLE_CONFIGURATION, ctlReg | 0x02);
        }
        else if (rxChannel == SNRX_GT)
        {
            CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_CH3_GAIN_TABLE_CONFIGURATION, ch3CtlReg | 0x01);
            CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_GAIN_TABLE_CONFIGURATION, ctlReg | 0x02);
        }
        else if (rxChannel == LOOPBACK_GT)
        {
            CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_CH3_GAIN_TABLE_CONFIGURATION, ch3CtlReg | 0x04);
            CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_GAIN_TABLE_CONFIGURATION, ctlReg | 0x02);
        }
        else
        {
            CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_GAIN_TABLE_CONFIGURATION, ctlReg | 0x02);
        }
    }

#elif MYK_ENABLE_SPIWRITEARRAY == 1
//#elif 0

    addrIndex = 0;
    dataIndex = 0;
    for (i = startIndex; i >= ((startIndex + 1) - numGainIndexesInTable); i--)
    {
        tableRowIndex = (uint16_t)(startIndex - (uint8_t)i) << 2;

        /* set current gain table index (address) */
        addrArray[addrIndex++] = MYKONOS_ADDR_GAIN_TABLE_ADDR;
        dataArray[dataIndex++] = (uint8_t)i;

        /* Set Rx Front End gain[5:0] */
        addrArray[addrIndex++] = rxFEGainAddr;
        dataArray[dataIndex++] = gainTablePtr[tableRowIndex];

        /* Set external control [5:0] OR LNA bypass if rxChannel == SNRX_GT */
        addrArray[addrIndex++] = rxExtCtlLnaAddr;
        dataArray[dataIndex++] = (rxChannel == SNRX_GT) ? (gainTablePtr[tableRowIndex + 1] << 4) : (gainTablePtr[tableRowIndex + 1]);

        /* Set digital attenuation/gain[6:0] and set/clear attenuation bit */
        addrArray[addrIndex++] = rxDigGainAttenAddr;
        dataArray[dataIndex++] = ((gainTablePtr[tableRowIndex + 3] << 7) | gainTablePtr[tableRowIndex + 2]);

        /* repeating gain table settings if Rx1 and Rx2 are selected for Rx2 configuration */
        if (rxChannel == RX1_RX2_GT)
        {
            /* Set Rx Front End gain[5:0] */
            addrArray[addrIndex++] = rx2FEGainAddr;
            dataArray[dataIndex++] = gainTablePtr[tableRowIndex];

            /* Set external control [5:0] */
            addrArray[addrIndex++] = rx2ExtCtlAddr;
            dataArray[dataIndex++] = gainTablePtr[tableRowIndex + 1];

            /* Set digital attenuation/gain[6:0] */
            addrArray[addrIndex++] = rx2DigGainAttenAddr;
            dataArray[dataIndex++] = ((gainTablePtr[tableRowIndex + 3] << 7) | gainTablePtr[tableRowIndex + 2]);
        }

        /* setting the write enable depending on rxChannel choice */
        if (rxChannel == ORX_GT)
        {
            addrArray[addrIndex++] = MYKONOS_ADDR_CH3_GAIN_TABLE_CONFIGURATION;
            dataArray[dataIndex++] = (ch3CtlReg | 0x02);
            addrArray[addrIndex++] = MYKONOS_ADDR_GAIN_TABLE_CONFIGURATION;
            dataArray[dataIndex++] = (ctlReg | 0x02);
        }
        else if (rxChannel == SNRX_GT)
        {
            addrArray[addrIndex++] = MYKONOS_ADDR_CH3_GAIN_TABLE_CONFIGURATION;
            dataArray[dataIndex++] = (ch3CtlReg | 0x01);
            addrArray[addrIndex++] = MYKONOS_ADDR_GAIN_TABLE_CONFIGURATION;
            dataArray[dataIndex++] = (ctlReg | 0x02);
        }
        else if (rxChannel == LOOPBACK_GT)
        {
            addrArray[addrIndex++] = MYKONOS_ADDR_CH3_GAIN_TABLE_CONFIGURATION;
            dataArray[dataIndex++] = (ch3CtlReg | 0x04);
            addrArray[addrIndex++] = MYKONOS_ADDR_GAIN_TABLE_CONFIGURATION;
            dataArray[dataIndex++] = (ctlReg | 0x02);
        }
        else
        {
            addrArray[addrIndex++] = MYKONOS_ADDR_GAIN_TABLE_CONFIGURATION;
            dataArray[dataIndex++] = (ctlReg | 0x02);
        }

        if (addrIndex >= spiBufferSize)
        {
            CMB_SPIWriteBytes(device->spiSettings, &addrArray[0], &dataArray[0], addrIndex);
            dataIndex = 0;
            addrIndex = 0;
        }
    }

    if (addrIndex > 0)
    {
        CMB_SPIWriteBytes(device->spiSettings, &addrArray[0], &dataArray[0], addrIndex);
    }

#endif

    /* clearing the channel 3 gain table configuration register if selected and stopping the gain table clock */
    if (rxChannel > RX1_RX2_GT)
    {
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_CH3_GAIN_TABLE_CONFIGURATION, 0x00);
    }
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_GAIN_TABLE_CONFIGURATION, 0x08);

    return MYKONOS_ERR_OK;
}

/**
 * \brief Sets the Rx1 Manual Gain Index
 *
 * If the value passed in the gainIndex parameter is within range of the gain table minimum and
 * maximum indexes, the Rx1 gain index will be updated in the device data structure
 * and written to the transceiver. Else, an error will be returned. The maximum index is 255
 * and the minimum index is application specific.
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 * - device->rxSettings->rxGainControl->rx1GainIndex
 * - device->rxSettings->rxGainControl->rx1MaxGainIndex
 * - device->rxSettings->rxGainControl->rx1MinGainIndex
 *
 * \param device Pointer to the Mykonos data structure
 * \param gainIndex Desired Rx1 gain index
 * 
 * \return Returns enum MYKONOS_ERR, MYKONOS_ERR_OK=pass, !MYKONOS_ERR_OK=fail
 */
mykonosErr_t MYKONOS_setRx1ManualGain(mykonosDevice_t *device, uint8_t gainIndex)
{

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_setRx1ManualGain()\n");
#endif

    if ((gainIndex < device->rx->rxGainCtrl->rx1MinGainIndex) || (gainIndex > device->rx->rxGainCtrl->rx1MaxGainIndex))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SETRX1GAIN_INV_GAIN_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_SETRX1GAIN_INV_GAIN_PARM));
        return MYKONOS_ERR_SETRX1GAIN_INV_GAIN_PARM;

    }
    else
    {
        /* If the desired gain index is in range of the gain table, update the
         * value in the device data structure, and write to device.
         */
        device->rx->rxGainCtrl->rx1GainIndex = gainIndex;

        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_MANUAL_GAIN_INDEX_CH_1, gainIndex);

        return MYKONOS_ERR_OK;
    }
}

/**
 * \brief Sets the Rx2 Manual Gain index
 *
 * If the value passed in the gainIndex parameter is within range of the gain table minimum and
 * maximum indexes, the Rx2 gain index will be updated in the device data structure
 * and written to the transceiver. Else, an error will be returned. The maximum index is 255
 * and the minimum index is application specific.
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 * - device->rxTxSettings->rxGainControl->rx2GainIndex
 * - device->rxTxSettings->rxGainControl->rx2MaxGainIndex
 * - device->rxTxSettings->rxGainControl->rx2MinGainIndex
 *
 * \param device Pointer to the Mykonos data structure
 * \param gainIndex Desired Rx2 gain index
 * 
 * \return Returns enum MYKONOS_ERR, MYKONOS_ERR_OK=pass, !MYKONOS_ERR_OK=fail
 */
mykonosErr_t MYKONOS_setRx2ManualGain(mykonosDevice_t *device, uint8_t gainIndex)
{

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_setRx2ManualGain()\n");
#endif

    if ((gainIndex < device->rx->rxGainCtrl->rx2MinGainIndex) || (gainIndex > device->rx->rxGainCtrl->rx2MaxGainIndex))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SETRX2GAIN_INV_GAIN_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_SETRX2GAIN_INV_GAIN_PARM));
        return MYKONOS_ERR_SETRX2GAIN_INV_GAIN_PARM;

    }
    else
    {
        /* If the desired gain index is in range of the gain table, update the
         * value in the device data structure, and write to device.
         */
        device->rx->rxGainCtrl->rx2GainIndex = gainIndex;

        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_MANUAL_GAIN_INDEX_CH_2, gainIndex);

        return MYKONOS_ERR_OK;
    }
}

/**
 * \brief Reads the Rx1 Gain Index for Manual or AGC gain control mode
 *
 * This function reads the Rx1 gain index for manual or AGC modes. If the 
 * *rx1GainIndex pointer is nonzero, the read back gain index will
 * be returned in the parameter.  If the *rx1GainIndex pointer
 * is NULL, the device data structure will be updated with the new read back value
 * 
 * <B>Dependencies</B>
 * - device->spiSettings
 * - device->rxTxSettings->rxGainControl->rx1GainIndex
 *
 * \param device Pointer to the Mykonos data structure
 * \param rx1GainIndex uint8_t Pointer to the Rx1 gain index value
 * 
 * \return Returns enum MYKONOS_ERR, MYKONOS_ERR_OK=pass, !MYKONOS_ERR_OK=fail
 */
mykonosErr_t MYKONOS_getRx1Gain(mykonosDevice_t *device, uint8_t *rx1GainIndex)
{
    uint8_t readData;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_getRx1Gain()\n");
#endif

    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_GAIN_CTL_CHANNEL_1, &readData);

    /*
     * If rx1GainIndex address is not NULL, return the Rx1 gain index in the
     * rx1GainIndex function parameter
     */
    if (rx1GainIndex != NULL)
    {
        *rx1GainIndex = readData;
    }

    /*
     * set the current Rx1 gain in the device data structure
     */
    if (device->profilesValid & RX_PROFILE_VALID)
    {
        device->rx->rxGainCtrl->rx1GainIndex = readData;
    }

    return MYKONOS_ERR_OK;
}

/**
 * \brief Reads the Rx2 Gain Index for Manual or AGC gain control mode
 *
 * This function reads the Rx2 gain index for manual or AGC modes. If the 
 * *rx1GainIndex pointer is nonzero, the read back gain index will
 * be returned in the parameter.  If the *rx1GainIndex pointer
 * is NULL, the device data structure will be updated with the new read back value
 * 
 * <B>Dependencies</B>
 * - device->spiSettings
 * - device->rxTxSettings->rxGainControl->rx2GainIndex
 *
 * \param device Pointer to the Mykonos data structure
 * \param rx2GainIndex Desired Rx2 gain index
 * 
 * \return Returns enum MYKONOS_ERR, MYKONOS_ERR_OK=pass, !MYKONOS_ERR_OK=fail
 */
mykonosErr_t MYKONOS_getRx2Gain(mykonosDevice_t *device, uint8_t *rx2GainIndex)
{
    uint8_t readData;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_getRx2Gain()\n");
#endif

    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_GAIN_CTL_CHANNEL_2, &readData);

    /*
     * If rx1GainIndex address is not NULL, return the Rx1 gain index in the
     * rx1GainIndex function parameter
     */
    if (rx2GainIndex != NULL)
    {
        *rx2GainIndex = readData;
    }

    /*
     * set the current Rx2 gain in the device data structure
     */
    if (device->profilesValid & RX_PROFILE_VALID)
    {
        device->rx->rxGainCtrl->rx2GainIndex = readData;
    }

    return MYKONOS_ERR_OK;
}

/**
 * \brief Sets up the device Rx Automatic Gain Control (AGC) registers.
 *
 * Three data structures (mykonosAgcCfg_t, mykonosPeakDetAgcCfg_t, mykonosPowerMeasAgcCfg_t)
 * must be instantiated prior to calling this function. Valid ranges for data structure members
 * must also be provided.
 *
 *
 * <B>Dependencies:</B>
 * - device->spiSettings
 * - device->spiSettings->chipSelectIndex
 * - device->rx->rxAgcCtrl->agcRx1MaxGainIndex
 * - device->rx->rxAgcCtrl->agcRx1MinGainIndex
 * - device->rx->rxAgcCtrl->agcRx2MaxGainIndex
 * - device->rx->rxAgcCtrl->agcRx2MinGainIndex
 * - device->rx->rxAgcCtrl->agcObsRxMaxGainIndex
 * - device->rx->rxAgcCtrl->agcObsRxMinGainIndex
 * - device->rx->rxAgcCtrl->agcObsRxSelect
 * - device->rx->rxAgcCtrl->agcPeakThresholdMode
 * - device->rx->rxAgcCtrl->agcLowThsPreventGainIncrease
 * - device->rx->rxAgcCtrl->agcGainUpdateCounter
 * - device->rx->rxAgcCtrl->agcSlowLoopSettlingDelay
 * - device->rx->rxAgcCtrl->agcPeakWaitTime
 * - device->rx->rxAgcCtrl->agcResetOnRxEnable
 * - device->rx->rxAgcCtrl->agcEnableSyncPulseForGainCounter
 * - device->rx->rxAgcCtrl->peakAgc->apdHighThresh
 * - device->rx->rxAgcCtrl->peakAgc->apdLowThresh
 * - device->rx->rxAgcCtrl->peakAgc->hb2HighThresh
 * - device->rx->rxAgcCtrl->peakAgc->hb2LowThresh
 * - device->rx->rxAgcCtrl->peakAgc->hb2VeryLowThresh
 * - device->rx->rxAgcCtrl->peakAgc->apdHighThreshExceededCnt
 * - device->rx->rxAgcCtrl->peakAgc->apdLowThreshExceededCnt
 * - device->rx->rxAgcCtrl->peakAgc->hb2HighThreshExceededCnt
 * - device->rx->rxAgcCtrl->peakAgc->hb2LowThreshExceededCnt
 * - device->rx->rxAgcCtrl->peakAgc->hb2VeryLowThreshExceededCnt
 * - device->rx->rxAgcCtrl->peakAgc->apdHighGainStepAttack
 * - device->rx->rxAgcCtrl->peakAgc->apdLowGainStepRecovery
 * - device->rx->rxAgcCtrl->peakAgc->hb2HighGainStepAttack
 * - device->rx->rxAgcCtrl->peakAgc->hb2LowGainStepRecovery
 * - device->rx->rxAgcCtrl->peakAgc->hb2VeryLowGainStepRecovery
 * - device->rx->rxAgcCtrl->peakAgc->apdFastAttack
 * - device->rx->rxAgcCtrl->peakAgc->hb2FastAttack
 * - device->rx->rxAgcCtrl->peakAgc->hb2OverloadDetectEnable
 * - device->rx->rxAgcCtrl->peakAgc->hb2OverloadDurationCnt
 * - device->rx->rxAgcCtrl->peakAgc->hb2OverloadThreshCnt
 * - device->rx->rxAgcCtrl->powerAgc->pmdUpperHighThresh
 * - device->rx->rxAgcCtrl->powerAgc->pmdUpperLowThresh
 * - device->rx->rxAgcCtrl->powerAgc->pmdLowerHighThresh
 * - device->rx->rxAgcCtrl->powerAgc->pmdLowerLowThresh
 * - device->rx->rxAgcCtrl->powerAgc->pmdUpperHighGainStepAttack
 * - device->rx->rxAgcCtrl->powerAgc->pmdUpperLowGainStepAttack
 * - device->rx->rxAgcCtrl->powerAgc->pmdLowerHighGainStepRecovery
 * - device->rx->rxAgcCtrl->powerAgc->pmdLowerLowGainStepRecovery
 * - device->rx->rxAgcCtrl->powerAgc->pmdMeasDuration
 * - device->rx->rxAgcCtrl->powerAgc->pmdMeasConfig
 *
 * \param device is structure pointer to the Mykonos data structure containing settings
 * The pointer to the Mykonos AGC data structure containing settings is checked for a null pointer
 * to ensure it has been initialized. If not an error is thrown.
 *
 * \retval Returns MYKONOS_ERR=pass, !MYKONOS_ERR=fail
 * \retval MYKONOS_ERR_INV_AGC_RX_STRUCT_INIT
 * \retval MYKONOS_ERR_INV_AGC_RX_PEAK_STRUCT_INIT
 * \retval MYKONOS_ERR_INV_AGC_RX_PWR_STRUCT_INIT
 * \retval MYKONOS_ERR_INV_AGC_RX1_MAX_GAIN_INDEX
 * \retval MYKONOS_ERR_INV_AGC_RX1_MIN_GAIN_INDEX
 * \retval MYKONOS_ERR_INV_AGC_RX2_MAX_GAIN_INDEX
 * \retval MYKONOS_ERR_INV_AGC_RX2_MIN_GAIN_INDEX
 * \retval MYKONOS_ERR_INV_AGC_RX_GAIN_UPDATE_TIME_PARM
 * \retval MYKONOS_ERR_INV_AGC_RX_PEAK_WAIT_TIME_PARM
 * \retval MYKONOS_ERR_INV_AGC_RX_SLOW_LOOP_SETTLING_DELAY
 * \retval MYKONOS_ERR_INV_AGC_PMD_MEAS_DURATION
 * \retval MYKONOS_ERR_INV_AGC_PMD_MEAS_CONFIG
 * \retval MYKONOS_ERR_INV_AGC_RX_LOW_THS_PREV_GAIN_INC
 * \retval MYKONOS_ERR_INV_AGC_RX_PEAK_THRESH_MODE
 * \retval MYKONOS_ERR_INV_AGC_RX_RESET_ON_RX_ENABLE
 * \retval MYKONOS_ERR_INV_AGC_RX_ENABLE_SYNC_PULSE_GAIN_COUNTER
 * \retval MYKONOS_ERR_INV_AGC_RX_PMD_LOWER_HIGH_THRESH
 * \retval MYKONOS_ERR_INV_AGC_RX_PMD_UPPER_LOW_THRESH
 * \retval MYKONOS_ERR_INV_AGC_RX_PMD_LOWER_LOW_THRESH
 * \retval MYKONOS_ERR_INV_AGC_RX_PMD_UPPER_HIGH_THRESH
 * \retval MYKONOS_ERR_INV_AGC_RX_PMD_UPPER_HIGH_GAIN_STEP
 * \retval MYKONOS_ERR_INV_AGC_RX_PMD_LOWER_LOW_GAIN_STEP
 * \retval MYKONOS_ERR_INV_AGC_RX_PMD_UPPER_LOW_GAIN_STEP
 * \retval MYKONOS_ERR_INV_AGC_RX_PMD_UPPER_LOW_GAIN_STEP
 * \retval MYKONOS_ERR_INV_AGC_RX_PKDET_FAST_ATTACK_VALUE
 * \retval MYKONOS_ERR_INV_AGC_RX_APD_HIGH_THRESH_PARM
 * \retval MYKONOS_ERR_INV_AGC_RX_APD_LOW_THRESH_PARM
 * \retval MYKONOS_ERR_INV_AGC_RX_HB2_HIGH_THRESH_PARM
 * \retval MYKONOS_ERR_INV_AGC_RX_HB2_LOW_THRESH_PARM
 * \retval MYKONOS_ERR_INV_AGC_RX_HB2_VERY_LOW_THRESH_PARM
 * \retval MYKONOS_ERR_INV_AGC_RX_APD_HIGH_GAIN_STEP_PARM
 * \retval MYKONOS_ERR_INV_AGC_RX_APD_LOW_GAIN_STEP_PARM
 * \retval MYKONOS_ERR_INV_AGC_RX_HB2_HIGH_GAIN_STEP_PARM
 * \retval MYKONOS_ERR_INV_AGC_RX_HB2_LOW_GAIN_STEP_PARM
 * \retval MYKONOS_ERR_INV_AGC_RX_HB2_VERY_LOW_GAIN_STEP_PARM
 * \retval MYKONOS_ERR_INV_AGC_RX_HB2_OVLD_ENABLE
 * \retval MYKONOS_ERR_INV_AGC_RX_HB2_OVLD_DUR_CNT
 * \retval MYKONOS_ERR_INV_AGC_RX_HB2_OVLD_THRESH_CNT
 */
mykonosErr_t MYKONOS_setupRxAgc(mykonosDevice_t *device)
{
    uint8_t decPowerConfig = 0;
    uint8_t lower1ThreshGainStepRegValue = 0;
    uint8_t powerThresholdsRegValue = 0;
    uint8_t hb2OvldCfgRegValue = 0;
    uint8_t agcGainUpdateCtr[3] = {0};

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_setRxAgc()\n");
#endif

    /* Check mykonosAgcCfg_t device->rx->rxAgcCtrl pointer for initialization */
    if (&device->rx->rxAgcCtrl == 0)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_RX_STRUCT_INIT,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_RX_STRUCT_INIT));
        return MYKONOS_ERR_INV_AGC_RX_STRUCT_INIT;
    }

    /* Check mykonosPeakDetAgcCfg_t device->rx->rxAgcCtrl->peakAgc pointer for initialization */
    if (&device->rx->rxAgcCtrl->peakAgc == 0)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_RX_PEAK_STRUCT_INIT,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_RX_PEAK_STRUCT_INIT));
        return MYKONOS_ERR_INV_AGC_RX_PEAK_STRUCT_INIT;
    }

    /* Check mykonosPowerMeasAgcCfg_t device->rx->rxAgcCtrl->powerAgc pointer for initialization */
    if (&device->rx->rxAgcCtrl->powerAgc == 0)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_RX_PWR_STRUCT_INIT,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_RX_PWR_STRUCT_INIT));
        return MYKONOS_ERR_INV_AGC_RX_PWR_STRUCT_INIT;
    }

    /* Range check agcRx1MaxGainIndex versus gain table limits */
    if ((device->rx->rxAgcCtrl->agcRx1MaxGainIndex > device->rx->rxGainCtrl->rx1MaxGainIndex)
            || (device->rx->rxAgcCtrl->agcRx1MaxGainIndex < device->rx->rxAgcCtrl->agcRx1MinGainIndex))

    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_RX1_MAX_GAIN_INDEX,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_RX1_MAX_GAIN_INDEX));
        return MYKONOS_ERR_INV_AGC_RX1_MAX_GAIN_INDEX;
    }
    else
    {
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_RX1_MAX_GAIN_INDEX, device->rx->rxAgcCtrl->agcRx1MaxGainIndex);
    }

    /* Range check agcRx1MinGainIndex versus gain table limits */
    if ((device->rx->rxAgcCtrl->agcRx1MinGainIndex < device->rx->rxGainCtrl->rx1MinGainIndex)
            || (device->rx->rxAgcCtrl->agcRx1MaxGainIndex < device->rx->rxAgcCtrl->agcRx1MinGainIndex))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_RX1_MIN_GAIN_INDEX,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_RX1_MIN_GAIN_INDEX));
        return MYKONOS_ERR_INV_AGC_RX1_MIN_GAIN_INDEX;
    }
    else
    {
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_RX1_MIN_GAIN_INDEX, device->rx->rxAgcCtrl->agcRx1MinGainIndex);
    }

    /* Range check agcRx2MaxGainIndex versus gain table limits */
    if ((device->rx->rxAgcCtrl->agcRx2MaxGainIndex > device->rx->rxGainCtrl->rx2MaxGainIndex)
            || (device->rx->rxAgcCtrl->agcRx2MaxGainIndex < device->rx->rxAgcCtrl->agcRx2MinGainIndex))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_RX2_MAX_GAIN_INDEX,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_RX2_MAX_GAIN_INDEX));
        return MYKONOS_ERR_INV_AGC_RX2_MAX_GAIN_INDEX;
    }
    else
    {
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_RX2_MAX_GAIN_INDEX, device->rx->rxAgcCtrl->agcRx2MaxGainIndex);
    }

    /* Range check agcRx2MinGainIndex versus gain table limits */
    if ((device->rx->rxAgcCtrl->agcRx2MinGainIndex < device->rx->rxGainCtrl->rx2MinGainIndex)
            || (device->rx->rxAgcCtrl->agcRx2MaxGainIndex < device->rx->rxAgcCtrl->agcRx2MinGainIndex))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_RX2_MIN_GAIN_INDEX,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_RX2_MIN_GAIN_INDEX));
        return MYKONOS_ERR_INV_AGC_RX2_MIN_GAIN_INDEX;
    }
    else
    {
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_RX2_MIN_GAIN_INDEX, device->rx->rxAgcCtrl->agcRx2MinGainIndex);
    }

    /* Range check for agcGainUpdateCounter (22-bit) */
    if ((device->rx->rxAgcCtrl->agcGainUpdateCounter > 0x3FFFFF) || (device->rx->rxAgcCtrl->agcGainUpdateCounter < 1))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_RX_GAIN_UPDATE_TIME_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_RX_GAIN_UPDATE_TIME_PARM));
        return MYKONOS_ERR_INV_AGC_RX_GAIN_UPDATE_TIME_PARM;
    }
    else
    {
        /* Split agcGainUpdateCounter into three values */
        agcGainUpdateCtr[0] = (uint8_t)(device->rx->rxAgcCtrl->agcGainUpdateCounter);
        agcGainUpdateCtr[1] = (uint8_t)(device->rx->rxAgcCtrl->agcGainUpdateCounter >> 8);
        agcGainUpdateCtr[2] = (uint8_t)(device->rx->rxAgcCtrl->agcGainUpdateCounter >> 16);

        /* Write two bytes directly due. Third word has its upper two bits masked off.  */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_SLOW_GAIN_UPDATE_CNT_1, agcGainUpdateCtr[0]);
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_SLOW_GAIN_UPDATE_CNT_2, agcGainUpdateCtr[1]);
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_AGC_SLOW_GAIN_UPDATE_CNT_3, agcGainUpdateCtr[2], 0x3F, 0);
    }

    /* Range check on agcPeakWaitTime (5-bit) */
    if (device->rx->rxAgcCtrl->agcPeakWaitTime > 0x1F || device->rx->rxAgcCtrl->agcPeakWaitTime < 0x02)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_RX_PEAK_WAIT_TIME_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_RX_PEAK_WAIT_TIME_PARM));
        return MYKONOS_ERR_INV_AGC_RX_PEAK_WAIT_TIME_PARM;
    }
    else
    {
        /* Write agcPeakWaitTime */
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_AGC_CFG_2, device->rx->rxAgcCtrl->agcPeakWaitTime, 0x1F, 0);
    }

    /* Set MYKONOS_ADDR_AGC_CFG_2 bits [6:5] = b11 to enable AGC counters for MGC mode - needed for ARM cals to work correctly */
    CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_AGC_CFG_2, 3, 0x60, 5);

    /* Range check for agcSlowLoopSettlingDelay (7-bit) */
    if (device->rx->rxAgcCtrl->agcSlowLoopSettlingDelay > 0x7F)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_RX_SLOW_LOOP_SETTLING_DELAY,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_RX_SLOW_LOOP_SETTLING_DELAY));
        return MYKONOS_ERR_INV_AGC_RX_SLOW_LOOP_SETTLING_DELAY;
    }
    else
    {
        /* Write agcSlowLoopSettlingDelay */
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_AGC_SLOW_LOOP_CFG, device->rx->rxAgcCtrl->agcSlowLoopSettlingDelay, 0x7F, 0);
    }

    /* Range check for pmdMeasDuration */
    if ((1 << (3 + device->rx->rxAgcCtrl->powerAgc->pmdMeasDuration)) >= (device->rx->rxAgcCtrl->agcGainUpdateCounter))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_PMD_MEAS_DURATION,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_PMD_MEAS_DURATION));
        return MYKONOS_ERR_INV_AGC_PMD_MEAS_DURATION;
    }
    else
    {
        /* Write pmdMeasDuration */
        CMB_SPIWriteField(device->spiSettings, MYKONOS_DEC_POWER_CONFIG_2, device->rx->rxAgcCtrl->powerAgc->pmdMeasDuration, 0x0F, 0);
    }

    /* Range check for pmdMeasConfig */
    if (device->rx->rxAgcCtrl->powerAgc->pmdMeasConfig > 0x3)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_PMD_MEAS_CONFIG,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_PMD_MEAS_CONFIG));
        return MYKONOS_ERR_INV_AGC_PMD_MEAS_CONFIG;
    }
    else
    {
        if (device->rx->rxAgcCtrl->powerAgc->pmdMeasConfig == 0)
        {
            decPowerConfig = 0x0; /* Dec Pwr measurement disable */
        }
        else if (device->rx->rxAgcCtrl->powerAgc->pmdMeasConfig == 1)
        {
            decPowerConfig = 0x3; /* Dec Pwr measurement enable,  HB2 for decPwr measurement */
        }
        else if (device->rx->rxAgcCtrl->powerAgc->pmdMeasConfig == 2)
        {
            decPowerConfig = 0x5; /* Dec Pwr measurement enable, RFIR for decPwr measurement */
        }
        else if (device->rx->rxAgcCtrl->powerAgc->pmdMeasConfig == 3)
        {
            decPowerConfig = 0x11; /* Dec Pwr measurement enable, BBDC2 for decPwr measurement */
        }
        /* Write pmdMeasConfig */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_DEC_POWER_CONFIG_1, decPowerConfig);
    }

    /* Range check agcLowThsPreventGainIncrease (1-bit) */
    if (device->rx->rxAgcCtrl->agcLowThsPreventGainIncrease > 1)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_RX_LOW_THS_PREV_GAIN_INC,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_RX_LOW_THS_PREV_GAIN_INC));
        return MYKONOS_ERR_INV_AGC_RX_LOW_THS_PREV_GAIN_INC;
    }
    else
    {
        /* Write agcLowThsPreventGainIncrease */
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_AGC_SLOW_LOCK_LEV_THRSH, device->rx->rxAgcCtrl->agcLowThsPreventGainIncrease, 0x80, 7);
    }

    /* Range check agcPeakThresholdMode (1-bit),  */
    if (device->rx->rxAgcCtrl->agcPeakThresholdMode > 1)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_RX_PEAK_THRESH_MODE,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_RX_PEAK_THRESH_MODE));
        return MYKONOS_ERR_INV_AGC_RX_PEAK_THRESH_MODE;
    }
    else
    {
        /* Save to lower1ThreshGainStepRegValue register variable */
        lower1ThreshGainStepRegValue |= (device->rx->rxAgcCtrl->agcPeakThresholdMode << 5);
    }

    /* Range check agcResetOnRxEnable (1-bit) */
    if (device->rx->rxAgcCtrl->agcResetOnRxEnable > 1)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_RX_LOW_THS_PREV_GAIN_INC,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_RX_LOW_THS_PREV_GAIN_INC));
        return MYKONOS_ERR_INV_AGC_RX_RESET_ON_RX_ENABLE;
    }
    else
    {
        /* Write agcResetOnRxEnable */
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_AGC_SLOW_GAIN_UPDATE_CNT_3, (device->rx->rxAgcCtrl->agcResetOnRxEnable << 7), 0x80, 0);
    }

    /* Range check agcEnableSyncPulseForGainCounter (1-bit) */
    if (device->rx->rxAgcCtrl->agcEnableSyncPulseForGainCounter > 1)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_RX_ENABLE_SYNC_PULSE_GAIN_COUNTER,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_RX_ENABLE_SYNC_PULSE_GAIN_COUNTER));
        return MYKONOS_ERR_INV_AGC_RX_ENABLE_SYNC_PULSE_GAIN_COUNTER;
    }
    else
    {
        /* Write agcEnableSyncPulseForGainCounter */
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_AGC_SLOW_LOOP_CFG, (device->rx->rxAgcCtrl->agcEnableSyncPulseForGainCounter << 7), 0x80, 0);
    }

    /* WRITE REGISTERS FOR THE AGC POWER MEASUREMENT DETECTOR (PMD) STRUCTURE */

    /* Range check pmdLowerHighThresh (7-bit) vs 0x7F and pmdUpperLowThresh */
    if ((device->rx->rxAgcCtrl->powerAgc->pmdLowerHighThresh <= device->rx->rxAgcCtrl->powerAgc->pmdUpperLowThresh)
            || device->rx->rxAgcCtrl->powerAgc->pmdLowerHighThresh > 0x7F)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_RX_PMD_LOWER_HIGH_THRESH,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_RX_PMD_LOWER_HIGH_THRESH));
        return MYKONOS_ERR_INV_AGC_RX_PMD_LOWER_HIGH_THRESH;
    }
    else
    {
        /* Write pmdLowerHighThresh */
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_AGC_SLOW_LOCK_LEV_THRSH, device->rx->rxAgcCtrl->powerAgc->pmdLowerHighThresh, 0x7F, 0);
    }

    /* Range check pmdUpperLowThresh (7-bit): Comparison to pmdLowerHigh done earlier */
    if (device->rx->rxAgcCtrl->powerAgc->pmdUpperLowThresh > 0x7F)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_RX_PMD_UPPER_LOW_THRESH,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_RX_PMD_UPPER_LOW_THRESH));
        return MYKONOS_ERR_INV_AGC_RX_PMD_UPPER_LOW_THRESH;
    }
    else
    {
        /* Write pmdUpperLowThresh */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_LOCK_LEVEL, device->rx->rxAgcCtrl->powerAgc->pmdUpperLowThresh);
    }

    /* Range check pmdLowerLowThresh (4-bit)  */
    if (device->rx->rxAgcCtrl->powerAgc->pmdLowerLowThresh > 0xF)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_RX_PMD_LOWER_LOW_THRESH,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_RX_PMD_LOWER_LOW_THRESH));
        return MYKONOS_ERR_INV_AGC_RX_PMD_LOWER_LOW_THRESH;
    }
    else
    {
        /* Write pmdUpperLowThresh to temp variable */
        powerThresholdsRegValue |= device->rx->rxAgcCtrl->powerAgc->pmdLowerLowThresh;
    }

    /* Range check pmdUpperHighThresh (4-bit)  */
    if (device->rx->rxAgcCtrl->powerAgc->pmdUpperHighThresh > 0xF)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_RX_PMD_UPPER_HIGH_THRESH,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_RX_PMD_UPPER_HIGH_THRESH));
        return MYKONOS_ERR_INV_AGC_RX_PMD_UPPER_HIGH_THRESH;
    }
    else
    {
        /* Write pmdUpperHighThresh to temp var, then to register */
        powerThresholdsRegValue |= (device->rx->rxAgcCtrl->powerAgc->pmdUpperHighThresh << 4);
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_SLOW_POWER_THRSH, powerThresholdsRegValue);
    }

    /* Range check pmdUpperHighGainStepAttack (5-bit)  */
    if (device->rx->rxAgcCtrl->powerAgc->pmdUpperHighGainStepAttack > 0x1F)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_RX_PMD_UPPER_HIGH_GAIN_STEP,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_RX_PMD_UPPER_HIGH_GAIN_STEP));
        return MYKONOS_ERR_INV_AGC_RX_PMD_UPPER_HIGH_GAIN_STEP;
    }
    else
    {
        /* Write pmdUpperHighGainStepAttack */
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_AGC_SLOW_UPPER1_THRSH_GAIN_STEP, device->rx->rxAgcCtrl->powerAgc->pmdUpperHighGainStepAttack, 0x1F, 0);
    }

    /* Range check pmdLowerLowGainStepRecovery (5-bit)  */
    if (device->rx->rxAgcCtrl->powerAgc->pmdLowerLowGainStepRecovery > 0x1F)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_RX_PMD_LOWER_LOW_GAIN_STEP,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_RX_PMD_LOWER_LOW_GAIN_STEP));
        return MYKONOS_ERR_INV_AGC_RX_PMD_LOWER_LOW_GAIN_STEP;
    }
    else
    {
        /* Write pmdLowerLowGainStepRecovery to temp var, then to register */
        lower1ThreshGainStepRegValue |= (device->rx->rxAgcCtrl->powerAgc->pmdLowerLowGainStepRecovery);
    }

    /* Range check pmdUpperLowGainStepRecovery (5-bit)  */
    if (device->rx->rxAgcCtrl->powerAgc->pmdUpperLowGainStepAttack > 0x1F)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_RX_PMD_UPPER_LOW_GAIN_STEP,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_RX_PMD_UPPER_LOW_GAIN_STEP));
        return MYKONOS_ERR_INV_AGC_RX_PMD_UPPER_LOW_GAIN_STEP;
    }
    else
    {
        /* Write pmdLowerLowGainStepRecovery to temp var, then to register */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_SLOW_UPPER0_THRSH_GAIN_STEP, device->rx->rxAgcCtrl->powerAgc->pmdUpperLowGainStepAttack);
    }

    /* Range check pmdLowerHighGainStepRecovery (5-bit)  */
    if (device->rx->rxAgcCtrl->powerAgc->pmdLowerHighGainStepRecovery > 0x1F)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_RX_PMD_LOWER_HIGH_GAIN_STEP,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_RX_PMD_LOWER_HIGH_GAIN_STEP));
        return MYKONOS_ERR_INV_AGC_RX_PMD_LOWER_HIGH_GAIN_STEP;
    }
    else
    {
        /* Write pmdLowerLowGainStepRecovery to temp var, then to register */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_SLOW_LOWER0_THRSH_GAIN_STEP, device->rx->rxAgcCtrl->powerAgc->pmdLowerHighGainStepRecovery);
    }

    /* WRITE REGISTERS FOR THE AGC PEAK DETECTOR (APD/HB2) STRUCTURE */

    /* Range check apdFastAttack and hb2FastAttack (1-bit)  */
    if (device->rx->rxAgcCtrl->peakAgc->apdFastAttack > 0x1 || device->rx->rxAgcCtrl->peakAgc->hb2FastAttack > 0x1)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_RX_PKDET_FAST_ATTACK_VALUE,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_RX_PKDET_FAST_ATTACK_VALUE));
        return MYKONOS_ERR_INV_AGC_RX_PKDET_FAST_ATTACK_VALUE;
    }
    else
    {
        /* Write pmdLowerLowGainStepRecovery to temp var, then to register */
        lower1ThreshGainStepRegValue |= (device->rx->rxAgcCtrl->peakAgc->apdFastAttack << 7);
        lower1ThreshGainStepRegValue |= (device->rx->rxAgcCtrl->peakAgc->hb2FastAttack << 6);
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_SLOW_LOWER1_THRSH_GAIN_STEP, lower1ThreshGainStepRegValue);
    }

    /* Range check apdHighThresh */
    if ((device->rx->rxAgcCtrl->peakAgc->apdHighThresh > 0x3F) || (device->rx->rxAgcCtrl->peakAgc->apdHighThresh <= device->rx->rxAgcCtrl->peakAgc->apdLowThresh))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_RX_APD_HIGH_THRESH_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_RX_APD_HIGH_THRESH_PARM));
        return MYKONOS_ERR_INV_AGC_RX_APD_HIGH_THRESH_PARM;
    }
    else
    {
        /* Write apdHighThresh */
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_AGC_ULB_THRSH, device->rx->rxAgcCtrl->peakAgc->apdHighThresh, 0X3F, 0);
    }

    /* Range check apdLowThresh */
    if ((device->rx->rxAgcCtrl->peakAgc->apdLowThresh > 0x3F) || (device->rx->rxAgcCtrl->peakAgc->apdHighThresh < device->rx->rxAgcCtrl->peakAgc->apdLowThresh))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_RX_APD_LOW_THRESH_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_RX_APD_LOW_THRESH_PARM));
        return MYKONOS_ERR_INV_AGC_RX_APD_LOW_THRESH_PARM;
    }
    else
    {
        /* write apdLowThresh */
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_AGC_LLB_THRSH, device->rx->rxAgcCtrl->peakAgc->apdLowThresh, 0x3F, 0);
    }

    /* Range check hb2HighThresh */
    if ((device->rx->rxAgcCtrl->peakAgc->hb2HighThresh > 0xFF) || (device->rx->rxAgcCtrl->peakAgc->hb2HighThresh < device->rx->rxAgcCtrl->peakAgc->hb2LowThresh))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_RX_HB2_HIGH_THRESH_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_RX_HB2_HIGH_THRESH_PARM));
        return MYKONOS_ERR_INV_AGC_RX_HB2_HIGH_THRESH_PARM;
    }
    else
    {
        /* write hb2HighThresh */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_OVRLD_PD_DEC_OVRLD_UPPER_THRSH, device->rx->rxAgcCtrl->peakAgc->hb2HighThresh);
    }

    /* Range check hb2LowThresh */
    if ((device->rx->rxAgcCtrl->peakAgc->hb2LowThresh > 0xFF) || (device->rx->rxAgcCtrl->peakAgc->hb2LowThresh > device->rx->rxAgcCtrl->peakAgc->hb2HighThresh))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_RX_HB2_LOW_THRESH_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_RX_HB2_LOW_THRESH_PARM));
        return MYKONOS_ERR_INV_AGC_RX_HB2_LOW_THRESH_PARM;
    }
    else
    {
        /* write hb2LowThresh */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_OVRLD_PD_DEC_OVRLD_LOWER_THRSH, device->rx->rxAgcCtrl->peakAgc->hb2LowThresh);
    }

    /* Range check hb2VeryLowThresh */
    if ((device->rx->rxAgcCtrl->peakAgc->hb2VeryLowThresh > 0xFF) || (device->rx->rxAgcCtrl->peakAgc->hb2VeryLowThresh > device->rx->rxAgcCtrl->peakAgc->hb2LowThresh))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_RX_HB2_VERY_LOW_THRESH_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_RX_HB2_VERY_LOW_THRESH_PARM));
        return MYKONOS_ERR_INV_AGC_RX_HB2_VERY_LOW_THRESH_PARM;
    }
    else
    {
        /* write hb2VeryLowThresh */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_OVRLD_PD_DEC_VERYLOW_THRSH, device->rx->rxAgcCtrl->peakAgc->hb2VeryLowThresh);
    }

    /* Write threshold counter values for apdHigh/apdLow/hb2High/hb2Low/hb2VeryLow */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_SLOW_ULB_CNT_THRSH, device->rx->rxAgcCtrl->peakAgc->apdHighThreshExceededCnt);
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_SLOW_LLB_CNT_THRSH, device->rx->rxAgcCtrl->peakAgc->apdLowThreshExceededCnt);
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_SLOW_HIGH_OVRG_CNT_THRSH, device->rx->rxAgcCtrl->peakAgc->hb2HighThreshExceededCnt);
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_SLOW_LOW_OVRG_CNT_THRSH, device->rx->rxAgcCtrl->peakAgc->hb2LowThreshExceededCnt);
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_SLOW_VERYLOW_OVRG_CNT_THRSH, device->rx->rxAgcCtrl->peakAgc->hb2VeryLowThreshExceededCnt);

    /* Range check on apdHighGainStepAttack (5-bit) */
    if (device->rx->rxAgcCtrl->peakAgc->apdHighGainStepAttack > 0x1F)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_RX_APD_HIGH_GAIN_STEP_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_RX_APD_HIGH_GAIN_STEP_PARM));
        return MYKONOS_ERR_INV_AGC_RX_APD_HIGH_GAIN_STEP_PARM;
    }
    else
    {
        /* Write apdHighGainStepAttack */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_OVRG_GAIN_STEP_1, device->rx->rxAgcCtrl->peakAgc->apdHighGainStepAttack);
    }

    /* Range check on apdLowGainStepRecovery (5-bit) */
    if (device->rx->rxAgcCtrl->peakAgc->apdLowGainStepRecovery > 0x1F)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_RX_APD_LOW_GAIN_STEP_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_RX_APD_LOW_GAIN_STEP_PARM));
        return MYKONOS_ERR_INV_AGC_RX_APD_LOW_GAIN_STEP_PARM;
    }
    else
    {
        /* Write apdLowGainStepRecovery */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_OVRG_GAIN_STEP_4, device->rx->rxAgcCtrl->peakAgc->apdLowGainStepRecovery);
    }

    /* Range check on hb2HighGainStepAttack (5-bit) */
    if (device->rx->rxAgcCtrl->peakAgc->hb2HighGainStepAttack > 0x1F)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_RX_HB2_HIGH_GAIN_STEP_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_RX_HB2_HIGH_GAIN_STEP_PARM));
        return MYKONOS_ERR_INV_AGC_RX_HB2_HIGH_GAIN_STEP_PARM;
    }
    else
    {
        /* Write hb2HighGainStepAttack */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_OVRG_GAIN_STEP_2, device->rx->rxAgcCtrl->peakAgc->hb2HighGainStepAttack);
    }

    /* Range check on hb2LowGainStepRecovery (5-bit) */
    if (device->rx->rxAgcCtrl->peakAgc->hb2LowGainStepRecovery > 0x1F)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_RX_HB2_LOW_GAIN_STEP_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_RX_HB2_LOW_GAIN_STEP_PARM));
        return MYKONOS_ERR_INV_AGC_RX_HB2_LOW_GAIN_STEP_PARM;
    }
    else
    {
        /* Write hb2LowGainStepRecovery */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_OVRG_GAIN_STEP_5, device->rx->rxAgcCtrl->peakAgc->hb2LowGainStepRecovery);
    }

    /* Range check on hb2VeryLowGainStepRecovery (5-bit) */
    if (device->rx->rxAgcCtrl->peakAgc->hb2VeryLowGainStepRecovery > 0x1F)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_RX_HB2_VERY_LOW_GAIN_STEP_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_RX_HB2_VERY_LOW_GAIN_STEP_PARM));
        return MYKONOS_ERR_INV_AGC_RX_HB2_VERY_LOW_GAIN_STEP_PARM;
    }
    else
    {
        /* Write hb2VeryLowGainStepRecovery */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_OVRG_GAIN_STEP_6, device->rx->rxAgcCtrl->peakAgc->hb2VeryLowGainStepRecovery);
    }

    /* Range Check on hb2OverloadDetectEnable */
    if (device->rx->rxAgcCtrl->peakAgc->hb2OverloadDetectEnable > 0x1)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_RX_HB2_OVLD_ENABLE,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_RX_HB2_OVLD_ENABLE));
        return MYKONOS_ERR_INV_AGC_RX_HB2_OVLD_ENABLE;
    }

    /* Range Check on hb2OverloadDetectEnable */
    if (device->rx->rxAgcCtrl->peakAgc->hb2OverloadDurationCnt > 0x7)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_RX_HB2_OVLD_DUR_CNT,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_RX_HB2_OVLD_DUR_CNT));
        return MYKONOS_ERR_INV_AGC_RX_HB2_OVLD_DUR_CNT;
    }

    /* Range Check on hb2OverloadDetectEnable */
    if (device->rx->rxAgcCtrl->peakAgc->hb2OverloadThreshCnt > 0xF)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_RX_HB2_OVLD_THRESH_CNT,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_RX_HB2_OVLD_THRESH_CNT));
        return MYKONOS_ERR_INV_AGC_RX_HB2_OVLD_THRESH_CNT;
    }
    else
    {
        /* Write the hb2OvldCfgRegValue, the combination of hb2OverloadThreshCnt, hb2OverloadDurationCnt, and hb2OverloadDetectEnable */
        hb2OvldCfgRegValue = (device->rx->rxAgcCtrl->peakAgc->hb2OverloadThreshCnt) | (device->rx->rxAgcCtrl->peakAgc->hb2OverloadDurationCnt << 4)
                | (device->rx->rxAgcCtrl->peakAgc->hb2OverloadDetectEnable << 7);
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_OVRLD_PD_DEC_OVRLD_CFG, hb2OvldCfgRegValue);
    }

    /* Hard-coded value for the ADC overload configuration. Sets the HB2 offset to -6dB.*/
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_OVRLD_ADC_OVRLD_CFG, 0x18);
    /* Hard-coded value for APD decay setting. Setting allows for the quickest settling time of peak detector */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_RX_BLOCK_DET_DECAY, 0x0);

    /* performing a soft reset */
    MYKONOS_resetRxAgc(device);

    return MYKONOS_ERR_OK;
}

/**
 * \brief This function resets the AGC state machine
 *
 * Calling this function resets all state machines within the gain control and maximum gain.
 *
 * <B>Dependencies:</B>
 * - device->spiSettings
 *
 * \param device is structure pointer to the Mykonos data structure containing the device SPI settings
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 */
mykonosErr_t MYKONOS_resetRxAgc(mykonosDevice_t *device)
{
    const uint8_t AGC_RESET = 0x80;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_resetRxAgc()\n");
#endif

    CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_AGC_CFG_2, 1, AGC_RESET, 7);
    CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_AGC_CFG_2, 0, AGC_RESET, 7);

    return MYKONOS_ERR_OK;

}

/**
 * \brief This function sets the min/max gain indexes for AGC in the main RX channel
 *
 * Allows to change min/max gain index on runtime.
 * If RX1_RX2 selected, then the maxGainIndex/minGainIndex value will be applied to both channels.
 * If only Rx1 selected, then only Rx1 min/max gain indices will be updated, along with their device data structure values.
 * If only Rx2 selected, then only Rx2 min/max gain indices will be updated, along with their device data structure values.
 *
 * <B>Dependencies:</B>
 * - device->spiSettings
 * - device->rx->rxAgcCtrl
 *
 * \param device is structure pointer to the Mykonos data structure containing the device SPI settings
 * \param rxChannelSelect RX channel for setting the max and min gain index settings
 * \param maxGainIndex Max gain index setting
 * \param minGainIndex Min gain index setting
 *
 * \retval MYKONOS_ERR_SET_RX_MAX_GAIN_INDEX Max gain index bigger than max gain index loaded table.
 * \retval MYKONOS_ERR_SET_RX_MIN_GAIN_INDEX Min gain index lower than min gain index loaded table.
 * \retval MYKONOS_ERR_AGC_MIN_MAX_CHANNEL Wrong RX channel selected
 * \retval MYKONOS_ERR_OK Function completed successfully
 */
mykonosErr_t MYKONOS_setRxAgcMinMaxGainIndex(mykonosDevice_t *device, mykonosRxChannels_t rxChannelSelect, uint8_t maxGainIndex, uint8_t minGainIndex)
{
    const uint8_t MIN_GAIN_INDEX = 1 + MAX_GAIN_TABLE_INDEX - (sizeof(RxGainTable) / sizeof(RxGainTable[0]));

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_setRxAgcMinMaxGainIndex()\n");
#endif

    /* Check for maxGainIndex */
    if (maxGainIndex > MAX_GAIN_TABLE_INDEX)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SET_RX_MAX_GAIN_INDEX,
                getMykonosErrorMessage(MYKONOS_ERR_SET_RX_MAX_GAIN_INDEX));
        return MYKONOS_ERR_SET_RX_MAX_GAIN_INDEX;
    }

    /* Check for minGainIndex */
    if (minGainIndex < MIN_GAIN_INDEX)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SET_RX_MIN_GAIN_INDEX,
                getMykonosErrorMessage(MYKONOS_ERR_SET_RX_MIN_GAIN_INDEX));
        return MYKONOS_ERR_SET_RX_MIN_GAIN_INDEX;
    }

    if (rxChannelSelect & RX1)
    {
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_RX1_MAX_GAIN_INDEX, maxGainIndex);
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_RX1_MIN_GAIN_INDEX, minGainIndex);
        device->rx->rxAgcCtrl->agcRx1MaxGainIndex = maxGainIndex;
        device->rx->rxAgcCtrl->agcRx1MinGainIndex = minGainIndex;
    }

    if (rxChannelSelect & RX2)
    {
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_RX2_MAX_GAIN_INDEX, maxGainIndex);
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_RX2_MIN_GAIN_INDEX, minGainIndex);
        device->rx->rxAgcCtrl->agcRx2MaxGainIndex = maxGainIndex;
        device->rx->rxAgcCtrl->agcRx2MinGainIndex = minGainIndex;
    }

    /* Check for invalid Rx channel */
    if (!(rxChannelSelect & RX1_RX2))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_AGC_MIN_MAX_RX_CHANNEL,
                getMykonosErrorMessage(MYKONOS_ERR_AGC_MIN_MAX_RX_CHANNEL));
        return MYKONOS_ERR_AGC_MIN_MAX_RX_CHANNEL;
    }

    return MYKONOS_ERR_OK;
}

/**
 * \brief This function sets the min/max gain indexes for AGC in the observation channel
 *
 * Allows to change min/max gain index on runtime.
 *
 * <B>Dependencies:</B>
 * - device->spiSettings
 * - device->rx->rxAgcCtrl
 *
 * \param device is structure pointer to the Mykonos data structure containing the device SPI settings
 * \param obsRxChannelSelect Observation channel for setting the max and min gain index settings
 * \param maxGainIndex Max gain index setting
 * \param minGainIndex Min gain index setting
 *
 * \retval MYKONOS_ERR_SET_ORX_MAX_GAIN_INDEX_CHANNEL Wrong read back channel.
 * \retval MYKONOS_ERR_SET_ORX_MAX_GAIN_INDEX Max gain index bigger than max gain index loaded table.
 * \retval MYKONOS_ERR_SET_ORX_MIN_GAIN_INDEX Min gain index lower than min gain index loaded table.
 * \retval MYKONOS_ERR_AGC_MIN_MAX_ORX_CHANNEL Wrong observation channel selected
 * \retval MYKONOS_ERR_OK Function completed successfully
 */
mykonosErr_t MYKONOS_setObsRxAgcMinMaxGainIndex(mykonosDevice_t *device, mykonosObsRxChannels_t obsRxChannelSelect, uint8_t maxGainIndex, uint8_t minGainIndex)
{
    uint8_t rdObsActive = 0x00;
    uint8_t obsActive = 0x00;
    uint8_t obsCheck = 0x00;
    uint8_t minGainIndexCheck = 0x00;
    uint8_t maxGainIndexCheck = 0x00;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_setObsRxAgcMinMaxGainIndex()\n");
#endif

    /* Read active channel */
    CMB_SPIReadField(device->spiSettings, MYKONOS_ADDR_AGC_ORX_SNRX_ACTIVE, &rdObsActive, 0x03, 0);

    /* Check active channel and set min/max checks */
    switch (rdObsActive)
    {
        case 0x00:
        case 0x01:
            /*sniffer*/
            maxGainIndexCheck = MAX_GAIN_TABLE_INDEX;
            minGainIndexCheck = MAX_GAIN_TABLE_INDEX - (sizeof(SnRxGainTable) / sizeof(SnRxGainTable[0]));
            obsActive = OBS_SNIFFER | OBS_SNIFFER_A | OBS_SNIFFER_B | OBS_SNIFFER_C;
            break;
        case 0x02:
        case 0x03:
            /*observation 1*/
            maxGainIndexCheck = MAX_GAIN_TABLE_INDEX;
            minGainIndexCheck = MAX_GAIN_TABLE_INDEX - (sizeof(ORxGainTable) / sizeof(ORxGainTable[0]));
            obsActive = OBS_RX1_TXLO | OBS_RX1_SNIFFERLO | OBS_RX2_TXLO | OBS_RX2_SNIFFERLO;
            break;

        default:
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SET_ORX_MAX_GAIN_INDEX_CHANNEL,
                    getMykonosErrorMessage(MYKONOS_ERR_SET_ORX_MAX_GAIN_INDEX_CHANNEL));
            return MYKONOS_ERR_SET_ORX_MAX_GAIN_INDEX_CHANNEL;
    }

    /* Check for maxGainIndex */
    if ((maxGainIndex > maxGainIndexCheck) || (maxGainIndex < minGainIndexCheck))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SET_ORX_MAX_GAIN_INDEX,
                getMykonosErrorMessage(MYKONOS_ERR_SET_ORX_MAX_GAIN_INDEX));
        return MYKONOS_ERR_SET_ORX_MAX_GAIN_INDEX;
    }

    /* Check for minGainIndex */
    if ((minGainIndex < minGainIndexCheck) || (minGainIndex > maxGainIndexCheck))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SET_ORX_MIN_GAIN_INDEX,
                getMykonosErrorMessage(MYKONOS_ERR_SET_ORX_MIN_GAIN_INDEX));
        return MYKONOS_ERR_SET_ORX_MIN_GAIN_INDEX;
    }

    device->obsRx->orxAgcCtrl->agcObsRxMaxGainIndex = maxGainIndex;
    device->obsRx->orxAgcCtrl->agcObsRxMinGainIndex = minGainIndex;

    switch (obsRxChannelSelect)
    {
        case OBS_SNIFFER:
        case OBS_SNIFFER_A:
        case OBS_SNIFFER_B:
        case OBS_SNIFFER_C:
            obsCheck = OBS_SNIFFER | OBS_SNIFFER_A | OBS_SNIFFER_B | OBS_SNIFFER_C;
            maxGainIndex = maxGainIndex - MAX_SNRX_GAIN_TABLE_NUMINDEXES;
            minGainIndex = minGainIndex - MAX_SNRX_GAIN_TABLE_NUMINDEXES;
            break;

        case OBS_RX1_TXLO:
        case OBS_RX2_TXLO:
        case OBS_RX1_SNIFFERLO:
        case OBS_RX2_SNIFFERLO:
            obsCheck = OBS_RX1_TXLO | OBS_RX1_SNIFFERLO | OBS_RX2_TXLO | OBS_RX2_SNIFFERLO;
            maxGainIndex = maxGainIndex - MAX_ORX_GAIN_TABLE_NUMINDEXES;
            minGainIndex = minGainIndex - MAX_ORX_GAIN_TABLE_NUMINDEXES;
            break;

        default:
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_AGC_MIN_MAX_ORX_CHANNEL,
                    getMykonosErrorMessage(MYKONOS_ERR_AGC_MIN_MAX_ORX_CHANNEL));
            return MYKONOS_ERR_AGC_MIN_MAX_ORX_CHANNEL;
    }

    if (obsCheck & obsActive)
    {
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_ORX_SNRX_MAX_GAIN_INDEX, maxGainIndex);
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_ORX_SNRX_MIN_GAIN_INDEX, minGainIndex);
    }
    else
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_AGC_MIN_MAX_ORX_CHANNEL,
                getMykonosErrorMessage(MYKONOS_ERR_AGC_MIN_MAX_ORX_CHANNEL));
        return MYKONOS_ERR_AGC_MIN_MAX_ORX_CHANNEL;
    }

    return MYKONOS_ERR_OK;
}

/**
 * \brief This function sets the Temperature gain compensation for Rx1 channel
 *
 * The temperature gain compensation control allow for a +3000mdB to -3000mdB digital gain
 * in the data path.
 * The resolution of the temperature compensation gain is 250mdB.
 *
 * <B>Dependencies:</B>
 * - device->spiSettings
 *
 * \param device is structure pointer to the Mykonos data structure containing the device SPI settings
 * \param rx1TempCompGain_mdB gain compensation for Rx1, the range is from -3000mdB to +3000mdB in steps of 250mdB
 *
 * \retval MYKONOS_ERR_RX1_TEMP_GAIN_COMP_RANGE The temp gain compensation is outside range (-3000mdB to 3000mdB)
 * \retval MYKONOS_ERR_RX1_TEMP_GAIN_COMP_STEP Not valid temp gain compensation, step size is 250mdB
 * \retval MYKONOS_ERR_OK Function completed successfully
 */
mykonosErr_t MYKONOS_setRx1TempGainComp(mykonosDevice_t *device, int16_t rx1TempCompGain_mdB)
{
    int8_t tempGainComp = 0x00;

    const int16_t LOW_LIMIT_GAIN_COMP = -3000;
    const int16_t HIGH_LIMIT_GAIN_COMP = 3000;
    const int16_t GAIN_COMP_STEP = 250;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_setRx1TempGainComp()\n");
#endif

    /* Check for Gain compensation range */
    if ((rx1TempCompGain_mdB > HIGH_LIMIT_GAIN_COMP) || (rx1TempCompGain_mdB < LOW_LIMIT_GAIN_COMP))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_RX1_TEMP_GAIN_COMP_RANGE,
                getMykonosErrorMessage(MYKONOS_ERR_RX1_TEMP_GAIN_COMP_RANGE));
        return MYKONOS_ERR_RX1_TEMP_GAIN_COMP_RANGE;
    }

    /* Check for Gain compensation steps */
    if (rx1TempCompGain_mdB % GAIN_COMP_STEP)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_RX1_TEMP_GAIN_COMP_STEP,
                getMykonosErrorMessage(MYKONOS_ERR_RX1_TEMP_GAIN_COMP_STEP));
        return MYKONOS_ERR_RX1_TEMP_GAIN_COMP_STEP;
    }

    /* Prepare register write */
    tempGainComp = (rx1TempCompGain_mdB / GAIN_COMP_STEP) & 0x1F;
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_RX1_TEMP_GAIN_COMP, (uint8_t)tempGainComp);

    return MYKONOS_ERR_OK;
}

/**
 * \brief This function gets the Temperature gain compensation for Rx1
 *
 * <B>Dependencies:</B>
 * - device->spiSettings
 *
 * \param device is structure pointer to the Mykonos data structure containing the device SPI settings
 * \param rx1TempCompGain_mdB Which will be populated with the Temperature Gain Compensation in mdB value for Rx1 channel.
 *
 * \retval MYKONOS_ERR_RX1_TEMP_GAIN_COMP_NULL rx1TempCompGain_mdB pointer is null.
 * \retval MYKONOS_ERR_OK Function completed successfully
 */
mykonosErr_t MYKONOS_getRx1TempGainComp(mykonosDevice_t *device, int16_t *rx1TempCompGain_mdB)
{
    uint8_t tempGainComp = 0x00;
    int8_t tempGainCompDB = 0x00;

    const int16_t GAIN_COMP_STEP = 250;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_getRx1TempGainComp()\n");
#endif

    /* Check for null passed parameter */
    if (rx1TempCompGain_mdB == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_RX1_TEMP_GAIN_COMP_NULL,
                getMykonosErrorMessage(MYKONOS_ERR_RX1_TEMP_GAIN_COMP_NULL));
        return MYKONOS_ERR_RX1_TEMP_GAIN_COMP_NULL;
    }

    /* Read Temperature Gain Compensation */
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_RX1_TEMP_GAIN_COMP, &tempGainComp);

    /* Convert to dB and prepare the signed return */
    tempGainCompDB = tempGainComp & 0x1F;
    if (tempGainCompDB & 0x10)
    {
        tempGainCompDB = (((~tempGainCompDB) & 0x0F) + 1) * -1;
    }

    /* Assign value to the passed pointer */
    *rx1TempCompGain_mdB = tempGainCompDB * GAIN_COMP_STEP;

    return MYKONOS_ERR_OK;
}

/**
 * \brief This function sets the Temperature gain compensation for Rx2 channel
 *
 * The temperature gain compensation control allow for a +3000mdB to -3000mdB digital gain
 * in the data path.
 * The resolution of the temperature compensation gain is 250mdB.
 *
 * <B>Dependencies:</B>
 * - device->spiSettings
 *
 * \param device is structure pointer to the Mykonos data structure containing the device SPI settings
 * \param rx2TempCompGain_mdB gain compensation for Rx1,
 * the range is from -3000mdB to +3000mdB in steps of 250mdB
 *
 * \retval MYKONOS_ERR_RX2_TEMP_GAIN_COMP_RANGE The temp gain compensation is outside range (-3000mdB to 3000mdB)
 * \retval MYKONOS_ERR_RX2_TEMP_GAIN_COMP_STEP Not valid temp gain compensation, step size is 250mdB
 * \retval MYKONOS_ERR_OK Function completed successfully
 */
mykonosErr_t MYKONOS_setRx2TempGainComp(mykonosDevice_t *device, int16_t rx2TempCompGain_mdB)
{
    int8_t tempGainComp = 0x00;

    const int16_t LOW_LIMIT_GAIN_COMP = -3000;
    const int16_t HIGH_LIMIT_GAIN_COMP = 3000;
    const int16_t GAIN_COMP_STEP = 250;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_setRx2TempGainComp()\n");
#endif

    /* Check for Gain compensation range */
    if ((rx2TempCompGain_mdB > HIGH_LIMIT_GAIN_COMP) || (rx2TempCompGain_mdB < LOW_LIMIT_GAIN_COMP))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_RX2_TEMP_GAIN_COMP_RANGE,
                getMykonosErrorMessage(MYKONOS_ERR_RX2_TEMP_GAIN_COMP_RANGE));
        return MYKONOS_ERR_RX2_TEMP_GAIN_COMP_RANGE;
    }

    /* Check for Gain compensation range */
    if (rx2TempCompGain_mdB % GAIN_COMP_STEP)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_RX2_TEMP_GAIN_COMP_STEP,
                getMykonosErrorMessage(MYKONOS_ERR_RX2_TEMP_GAIN_COMP_STEP));
        return MYKONOS_ERR_RX2_TEMP_GAIN_COMP_STEP;
    }

    /* Prepare register write */
    tempGainComp = (rx2TempCompGain_mdB / GAIN_COMP_STEP) & 0x1F;
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_RX2_TEMP_GAIN_COMP, (uint8_t)tempGainComp);

    return MYKONOS_ERR_OK;
}

/**
 * \brief This function gets the Temperature gain compensation for Rx2
 *
 * <B>Dependencies:</B>
 * - device->spiSettings
 *
 * \param device is structure pointer to the Mykonos data structure containing the device SPI settings
 * \param rx2TempCompGain_mdB Which will be populated with the Temperature Gain Compensation in mdB value for Rx2 channel.
 *
 * \retval MYKONOS_ERR_RX2_TEMP_GAIN_COMP_NULL rx2TempCompGain_mdB pointer is null.
 * \retval MYKONOS_ERR_OK Function completed successfully
 */
mykonosErr_t MYKONOS_getRx2TempGainComp(mykonosDevice_t *device, int16_t *rx2TempCompGain_mdB)
{
    uint8_t tempGainComp = 0x00;
    int8_t tempGainCompDB = 0x00;

    const int16_t GAIN_COMP_STEP = 250;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_getRx2TempGainComp()\n");
#endif

    /* Check for null passed parameter */
    if (rx2TempCompGain_mdB == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_RX2_TEMP_GAIN_COMP_NULL,
                getMykonosErrorMessage(MYKONOS_ERR_RX2_TEMP_GAIN_COMP_NULL));
        return MYKONOS_ERR_RX2_TEMP_GAIN_COMP_NULL;
    }

    /* Read programmed Temperature Gain Compensation */
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_RX2_TEMP_GAIN_COMP, &tempGainComp);

    /* Convert to dB and prepare the signed return */
    tempGainCompDB = tempGainComp & 0x1F;
    if (tempGainCompDB & 0x10)
    {
        tempGainCompDB = (((~tempGainCompDB) & 0x0F) + 1) * -1;
    }

    /* Assign value to the passed pointer */
    *rx2TempCompGain_mdB = tempGainCompDB * GAIN_COMP_STEP;

    return MYKONOS_ERR_OK;
}

/**
 * \brief This function sets the Temperature gain compensation for the observation channel
 *
 * The temperature gain compensation control allow for a +3000mdB to -3000mdB digital gain
 * in the data path.
 * The resolution of the temperature compensation gain is 250mdB.
 *
 * <B>Dependencies:</B>
 * - device->spiSettings
 *
 * \param device is structure pointer to the Mykonos data structure containing the device SPI settings
 * \param obsRxTempCompGain_mdB gain compensation for observation channel,
 * the range is from -3000mdB to +3000mdB in steps of 250mdB
 *
 * \retval MYKONOS_ERR_OBS_RX_TEMP_GAIN_COMP_RANGE The temp gain compensation is outside range (-3000mdB to 3000mdB)
 * \retval MYKONOS_ERR_OBS_RX_TEMP_GAIN_COMP_STEP Not valid temp gain compensation, step size is 250mdB
 * \retval MYKONOS_ERR_OK Function completed successfully
 */
mykonosErr_t MYKONOS_setObsRxTempGainComp(mykonosDevice_t *device, int16_t obsRxTempCompGain_mdB)
{
    int8_t tempGainComp = 0x00;

    const int16_t LOW_LIMIT_GAIN_COMP = -3000;
    const int16_t HIGH_LIMIT_GAIN_COMP = 3000;
    const int16_t GAIN_COMP_STEP = 250;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_setObsRxTempGainComp()\n");
#endif

    /* Check for Gain compensation range */
    if ((obsRxTempCompGain_mdB > HIGH_LIMIT_GAIN_COMP) || (obsRxTempCompGain_mdB < LOW_LIMIT_GAIN_COMP))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OBS_RX_TEMP_GAIN_COMP_RANGE,
                getMykonosErrorMessage(MYKONOS_ERR_OBS_RX_TEMP_GAIN_COMP_RANGE));
        return MYKONOS_ERR_OBS_RX_TEMP_GAIN_COMP_RANGE;
    }

    /* Check for Gain compensation range */
    if (obsRxTempCompGain_mdB % GAIN_COMP_STEP)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OBS_RX_TEMP_GAIN_COMP_STEP,
                getMykonosErrorMessage(MYKONOS_ERR_OBS_RX_TEMP_GAIN_COMP_STEP));
        return MYKONOS_ERR_OBS_RX_TEMP_GAIN_COMP_STEP;
    }

    /* Prepare register write */
    tempGainComp = (obsRxTempCompGain_mdB / GAIN_COMP_STEP) & 0x1F;
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_OBS_TEMP_GAIN_COMP, (uint8_t)tempGainComp);

    return MYKONOS_ERR_OK;
}

/**
 * \brief This function gets the Temperature gain compensation for the observation channel
 *
 *
 * <B>Dependencies:</B>
 * - device->spiSettings
 *
 * \param device is structure pointer to the Mykonos data structure containing the device SPI settings
 * \param obsRxTempCompGain_mdB Which will be populated with the Temperature Gain Compensation in mdB value for the observation channel.
 *
 * \retval MYKONOS_ERR_OBS_RX_TEMP_GAIN_COMP_NULL obsRxTempCompGain_mdB pointer is null.
 * \retval MYKONOS_ERR_OK Function completed successfully
 */
mykonosErr_t MYKONOS_getObsRxTempGainComp(mykonosDevice_t *device, int16_t *obsRxTempCompGain_mdB)
{
    uint8_t tempGainComp = 0x00;
    int8_t tempGainCompDB = 0x00;

    const int16_t GAIN_COMP_STEP = 250;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_getObsRxTempGainComp()\n");
#endif

    /* Check for null passed parameter */
    if (obsRxTempCompGain_mdB == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OBS_RX_TEMP_GAIN_COMP_NULL,
                getMykonosErrorMessage(MYKONOS_ERR_OBS_RX_TEMP_GAIN_COMP_NULL));
        return MYKONOS_ERR_OBS_RX_TEMP_GAIN_COMP_NULL;
    }

    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_OBS_TEMP_GAIN_COMP, &tempGainComp);

    /* Convert to dB and prepare the signed return */
    tempGainCompDB = tempGainComp & 0x1F;
    if (tempGainCompDB & 0x10)
    {
        tempGainCompDB = (((~tempGainCompDB) & 0x0F) + 1) * -1;
    }

    /* Assign value to the passed pointer */
    *obsRxTempCompGain_mdB = tempGainCompDB * GAIN_COMP_STEP;

    return MYKONOS_ERR_OK;
}

/**
 * \brief Enables or disables the Rx slow loop gain counter to use the sync pulse
 *
 * <B>Dependencies:</B>
 * - device->spiSettings
 *
 * \param device is structure pointer to the Mykonos data structure containing the device SPI settings
 * \param enable is a uint8_t data type where '0' = disable Rx gain counter from using sync pulse, '1' = enable Rx gain. All other values will throw an error.
 * counter to use the sync pulse. This value is written back to the device data structure for storage.
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 */
mykonosErr_t MYKONOS_enableRxGainCtrSyncPulse(mykonosDevice_t *device, uint8_t enable)
{
    const uint8_t syncPulseBitMask = 0x80;
    uint8_t enableBit = 0;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_setRxGainCtrSyncPulse()\n");
#endif

    /* read, modify, write Rx gain counter sync pulse enable bit */
    enableBit = (enable > 0) ? 1 : 0;
    CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_AGC_SLOW_LOOP_CFG, enableBit, syncPulseBitMask, 7);

    return MYKONOS_ERR_OK;
}

/**
 * \brief Configures the Rx gain control mode
 *
 * <B>Dependencies:</B>
 * - device->spiSettings
 * - device->spiSettings->chipSelectIndex
 * - device->rx->rxGainCtrl->gainMode
 *
 * \param device is structure pointer to the Mykonos data structure containing settings
 * \param mode is a mykonosGainMode_t enumerated gain control mode type where:
 * Manual Gain = MGC, Automatic Gain Control = AGC, and hybrid mode = HYBRID
 *
 * When the mode enumerated type is passed, the Rx1 and Rx2 gain mode is set to this value and the
 * Rx gainMode structure member is updated with the new value
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_INV_RX_GAIN_MODE_PARM Invalid AGC mode pass in function mode paramter
 */
mykonosErr_t MYKONOS_setRxGainControlMode(mykonosDevice_t *device, mykonosGainMode_t mode)
{
    uint8_t rxGainCfgReg = 0;

    const uint8_t RX_GAIN_TYPE_MASK = 0x1F;
    const uint8_t RX_GAIN_HYBRID_MASK = 0x10;
    const uint8_t RX_GAIN_TYPE_SHIFT = 0x02;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_setRxGainControlMode()\n");
#endif

    /* read AGC type for Rx1 and Rx2 for modify, write */
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_AGC_CFG_1, &rxGainCfgReg);
    rxGainCfgReg &= ~RX_GAIN_TYPE_MASK;

    /* performing AGC type check */
    switch (mode)
    {
        case MGC:
            rxGainCfgReg &= ~RX_GAIN_HYBRID_MASK;
            break;
        case AGC:
            rxGainCfgReg &= ~RX_GAIN_HYBRID_MASK;
            break;
        case HYBRID:
            rxGainCfgReg |= RX_GAIN_HYBRID_MASK;
            break;
        default:
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_RX_GAIN_MODE_PARM,
                    getMykonosErrorMessage(MYKONOS_ERR_INV_RX_GAIN_MODE_PARM));
            return MYKONOS_ERR_INV_RX_GAIN_MODE_PARM;
    }

    /* modify, write AGC type for Rx1 and Rx2 */
    rxGainCfgReg |= mode;
    rxGainCfgReg |= mode << RX_GAIN_TYPE_SHIFT;
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_CFG_1, rxGainCfgReg);

    /* writing mode setting to data structure */
    if ((device->profilesValid & RX_PROFILE_VALID) > 0)
    {
        device->rx->rxGainCtrl->gainMode = mode;
    }

    return MYKONOS_ERR_OK;
}

/**
 * \brief Performs a power measurement in the Rx1 digital data path.
 *
 * Due to interdependencies between the AGC and power measurement the power measurement duration and
 * where the measurement is taken is variable.
 * The location of the power measurement is given by device->rx->rxAgcCtrl->rxPwrAgc->pmdMeasConfig
 * The number of samples the power measurement uses is given by 8*2^(device->rx->rxAgcCtrl->rxPwrAgc->pmdMeasDuration) at the IQ rate,
 * if measured at RFIR output. This number of samples must be less than the agcGainUpdateCounter.
 * If the receiver is disabled during the power measurement, this function returns a 0 value for rx1DecPower_mdBFS
 *
 * The resolution of this function is 0.25dB.
 * The dynamic range of this function is 40dB. Signals lower than 40dBFS may not be measured accurately.
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 * - device->rx->rxAgcCtrl->rxPwrAgc->pmdMeasConfig
 * - device->rx->rxAgcCtrl->rxPwrAgc->pmdMeasDuration
 *
 * \param device Pointer to the Mykonos data structure
 * \param rx1DecPower_mdBFS Pointer to store the Rx1 decimated power return.  Value returned in mdBFS
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 */
mykonosErr_t MYKONOS_getRx1DecPower(mykonosDevice_t *device, uint16_t *rx1DecPower_mdBFS)
{
    uint8_t rx1DecPower_dBFS = 0;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_getRx1DecPower()\n");
#endif

    /* read Rx1 Dec Power Measurement */
    if (rx1DecPower_mdBFS != NULL)
    {
        CMB_SPIReadByte(device->spiSettings, MYKONOS_CH1_DECIMATED_PWR, &rx1DecPower_dBFS);
        *rx1DecPower_mdBFS = (uint16_t)(rx1DecPower_dBFS * 250); /* 250 = 1000 * 0.25dB */
    }
    else
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GET_RX1_DEC_POWER_NULL_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_GET_RX1_DEC_POWER_NULL_PARM));
        return MYKONOS_ERR_GET_RX1_DEC_POWER_NULL_PARM;
    }

    return MYKONOS_ERR_OK;
}

/**
 * \brief Performs a power measurement in the Rx2 digital data path.
 *
 * Due to interdependencies between the AGC and power measurement the power measurement duration and
 * where the measurement is taken is variable.
 * The location of the power measurement is given by device->rx->rxAgcCtrl->rxPwrAgc->pmdMeasConfig
 * The number of samples the power measurement uses is given by 8*2^(device->rx->rxAgcCtrl->rxPwrAgc->pmdMeasDuration) at the IQ rate,
 * if measured at RFIR output. This number of samples must be less than the agcGainUpdateCounter.
 * If the receiver is disabled during the power measurement, this function returns a 0 value for rx2DecPower_mdBFS
 *
 * The resolution of this function is 0.25dB.
 * The dynamic range of this function is 40dB. Signals lower than 40dBFS may not be measured accurately.
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 *
 * \param device Pointer to the Mykonos data structure
 * \param rx2DecPower_mdBFS Pointer to store the Rx1 decimated power return.  Value returned in mdBFS
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 */
mykonosErr_t MYKONOS_getRx2DecPower(mykonosDevice_t *device, uint16_t *rx2DecPower_mdBFS)
{
    uint8_t rx2DecPower_dBFS = 0;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_getRx2DecPower()\n");
#endif

    /* read Rx2 Dec Power Measurement */
    if (rx2DecPower_mdBFS != NULL)
    {
        CMB_SPIReadByte(device->spiSettings, MYKONOS_CH2_DECIMATED_PWR, &rx2DecPower_dBFS);
        *rx2DecPower_mdBFS = (uint16_t)(rx2DecPower_dBFS * 250); /* 250 = 1000 * 0.25dB */
    }
    else
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GET_RX2_DEC_POWER_NULL_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_GET_RX2_DEC_POWER_NULL_PARM));
        return MYKONOS_ERR_GET_RX2_DEC_POWER_NULL_PARM;
    }

    return MYKONOS_ERR_OK;
}

/*
 *****************************************************************************
 * Observation Rx Data path functions
 *****************************************************************************
 */

/**
 * \brief Sets the default Obs Rx channel to enter when device moves from
 *        radioOff to radioOn
 *
 * By default, the observation receiver remains powered down when the device
 * moves into the radioOn state.  If the BBIC prefers a particular ObsRx
 * channel to be enabled, this function can be called in radioOff to set
 * the default channel to power up when the device moves to radioOn.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 * - device->obsRx->defaultObsRxChannel
 *
 * \param device is structure pointer to the Mykonos data structure containing settings
 * \param defaultObsRxCh is mykonosObsRxChannels_t enum type which selects the desired observation receive path to power up
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_SETDEFOBSRXPATH_NULL_OBSRX_STRUCT Observation profile not valid, device->obsRx structure is NULL
 * \retval MYKONOS_ERR_SETDEFOBSRXPATH_NULL_DEF_OBSRX_STRUCT Invalid defaultObsRxCh function parameter
 */
mykonosErr_t MYKONOS_setDefaultObsRxPath(mykonosDevice_t *device, mykonosObsRxChannels_t defaultObsRxCh)
{
    uint8_t orxEntryMode[1] = {0};
    uint16_t byteOffset = 0;
    mykonosErr_t retVal = MYKONOS_ERR_OK;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_setDefaultObsRxPath()\n");
#endif

    if (device->obsRx == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SETDEFOBSRXPATH_NULL_DEF_OBSRX_STRUCT,
                getMykonosErrorMessage(MYKONOS_ERR_SETDEFOBSRXPATH_NULL_DEF_OBSRX_STRUCT));
        return MYKONOS_ERR_SETDEFOBSRXPATH_NULL_DEF_OBSRX_STRUCT;
    }

    if (((defaultObsRxCh > OBS_RX2_SNIFFERLO) && (defaultObsRxCh != OBS_SNIFFER_A) && (defaultObsRxCh != OBS_SNIFFER_B) && (defaultObsRxCh != OBS_SNIFFER_C)))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_PU_OBSRXPATH_INV_PARAM,
                getMykonosErrorMessage(MYKONOS_ERR_PU_OBSRXPATH_INV_PARAM));
        return MYKONOS_ERR_PU_OBSRXPATH_INV_PARAM;
    }

    device->obsRx->defaultObsRxChannel = defaultObsRxCh;
    orxEntryMode[0] = (device->obsRx->defaultObsRxChannel & 0xFF);

    byteOffset = 6;
    retVal = MYKONOS_writeArmConfig(device, MYKONOS_ARM_OBJECTID_RADIO_CONTROL, byteOffset, &orxEntryMode[0], 1);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    return MYKONOS_ERR_OK;
}

/**
 * \brief Powers up or down the Observation Rx signal chain in the radioOn state
 *
 * When the ARM radio control is in ARM command mode, this
 * function allows the user to selectively power up or down the desired ObsRx
 * data path. If this function is called when the ARM is expecting GPIO pin
 * control of the ObsRx path source, an error will be returned.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is structure pointer to the Mykonos data structure containing settings
 * \param obsRxCh is mykonosObsRxChannels_t enum type which selects the desired observation receive path to power up
 *         
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_PU_OBSRXPATH_INV_PARAM Invalid obsRxCh function parameter
 * \retval MYKONOS_ERR_PU_OBSRXPATH_ARMERROR ARM returned an error while trying to set the ObsRx Path source
 */
mykonosErr_t MYKONOS_setObsRxPathSource(mykonosDevice_t *device, mykonosObsRxChannels_t obsRxCh)
{
    uint8_t cmdStatByte = 0;
    uint32_t timeoutMs = 0;
    uint8_t payload[2] = {0, 0};
    mykonosErr_t retVal = MYKONOS_ERR_OK;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_setObsRxPathSource()\n");
#endif

    if (((obsRxCh > OBS_RX2_SNIFFERLO) && (obsRxCh != OBS_SNIFFER_A) && (obsRxCh != OBS_SNIFFER_B) && (obsRxCh != OBS_SNIFFER_C)))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_PU_OBSRXPATH_INV_PARAM,
                getMykonosErrorMessage(MYKONOS_ERR_PU_OBSRXPATH_INV_PARAM));
        return MYKONOS_ERR_PU_OBSRXPATH_INV_PARAM;
    }

    payload[0] = MYKONOS_ARM_OBJECTID_ORX_MODE;
    payload[1] = (uint8_t)(obsRxCh);

    retVal = MYKONOS_sendArmCommand(device, MYKONOS_ARM_SET_OPCODE, &payload[0], sizeof(payload));
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    timeoutMs = 1000; /* wait max of 1sec for command to complete */
    retVal = MYKONOS_waitArmCmdStatus(device, MYKONOS_ARM_SET_OPCODE, timeoutMs, &cmdStatByte);
    if (retVal != MYKONOS_ERR_OK)
    {
        if (cmdStatByte > 0)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_PU_OBSRXPATH_ARMERROR,
                    getMykonosErrorMessage(MYKONOS_ERR_PU_OBSRXPATH_ARMERROR));
            return MYKONOS_ERR_PU_OBSRXPATH_ARMERROR;
        }

        return retVal;
    }

    /* Verify ARM command did not return an error */
    if (cmdStatByte > 0)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_PU_OBSRXPATH_ARMERROR,
                getMykonosErrorMessage(MYKONOS_ERR_PU_OBSRXPATH_ARMERROR));
        return MYKONOS_ERR_PU_OBSRXPATH_ARMERROR;
    }

    return MYKONOS_ERR_OK;
}

/**
 * \brief Reads back the currently enabled Observation Rx channel in the
 *        radioOn state
 *
 * In pin mode, the pin could change asynchronous to reading back the
 * current enable ObsRx path.  Calling this function while in radioOff will
 * return OBS_RXOFF since all radio channels are powered down.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is structure pointer to the Mykonos data structure containing settings
 * \param obsRxCh Parameter to return enum of the current observation receive
 *                path powered up
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_PU_GETOBSRXPATH_ARMERROR ARM returned an error while trying to get the ObsRx Path source
 */
mykonosErr_t MYKONOS_getObsRxPathSource(mykonosDevice_t *device, mykonosObsRxChannels_t *obsRxCh)
{
    uint8_t extData[1] = {MYKONOS_ARM_OBJECTID_ORX_MODE};
    uint8_t cmdStatusByte = 0;
    uint8_t armData[1] = {0};
    uint32_t timeoutMs = 0;
    mykonosErr_t retVal = MYKONOS_ERR_OK;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_getObsRxPathSource()\n");
#endif

    retVal = MYKONOS_sendArmCommand(device, MYKONOS_ARM_GET_OPCODE, &extData[0], sizeof(extData));
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    timeoutMs = 1000;
    retVal = MYKONOS_waitArmCmdStatus(device, MYKONOS_ARM_GET_OPCODE, timeoutMs, &cmdStatusByte);
    if (retVal != MYKONOS_ERR_OK)
    {
        if (cmdStatusByte > 0)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_PU_GETOBSRXPATH_ARMERROR,
                    getMykonosErrorMessage(MYKONOS_ERR_PU_GETOBSRXPATH_ARMERROR));
            return MYKONOS_ERR_PU_GETOBSRXPATH_ARMERROR;
        }

        return retVal;
    }

    /* read 64-bit frequency from ARM memory */
    retVal = MYKONOS_readArmMem(device, MYKONOS_ADDR_ARM_START_DATA_ADDR, &armData[0], 1, 1);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    if (cmdStatusByte > 0)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_PU_GETOBSRXPATH_ARMERROR,
                getMykonosErrorMessage(MYKONOS_ERR_PU_GETOBSRXPATH_ARMERROR));
        return MYKONOS_ERR_PU_GETOBSRXPATH_ARMERROR;
    }

    *obsRxCh = (mykonosObsRxChannels_t)(armData[0]);

    return MYKONOS_ERR_OK;
}

/**
 * \brief Sets the Rx gain of the ObsRx channel
 *
 * The ObsRx channel can have different RF inputs (ORx1/ORx2/SnRx A,B,C)
 * This function sets the ObsRx gain index independently for ORx1/ORx2, or SnRx.
 * SnRx A, B, and C share the same gain index.  Please note that ORx1/ORx2 share a gain
 * table, as does SnRx A, B, and C. The maximum index is 255
 * and the minimum index is application specific.
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 * - device->obsRx->orxGainCtrl->maxGainIndex
 * - device->obsRx->orxGainCtrl->minGainIndex
 * - device->obsRx->orxGainCtrl->orx1GainIndex
 * - device->obsRx->orxGainCtrl->orx2GainIndex
 * - device->obsRx->snifferGainCtrl->maxGainIndex
 * - device->obsRx->snifferGainCtrl->minGainIndex
 * - device->obsRx->snifferGainCtrl->gainIndex
 *
 * \param device Pointer to the Mykonos device data structure
 * \param obsRxCh is an enum type mykonosObsRxChannels_t to identify the desired RF input for gain change
 * \param gainIndex Desired manual gain table index to set
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_SETORXGAIN_INV_ORX1GAIN Invalid gain requested for ORx1: outside gain table min/max index
 * \retval MYKONOS_ERR_SETORXGAIN_INV_ORX2GAIN Invalid gain requested for ORx2: outside gain table min/max index
 * \retval MYKONOS_ERR_SETORXGAIN_INV_SNRXGAIN Invalid gain requested for Sniffer: outside gain table min/max index
 * \retval MYKONOS_ERR_SETORXGAIN_INV_CHANNEL Function parameter obsRxCh has an invalid enum value
 */
mykonosErr_t MYKONOS_setObsRxManualGain(mykonosDevice_t *device, mykonosObsRxChannels_t obsRxCh, uint8_t gainIndex)
{

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_setObsRxManualGain()\n");
#endif

    if ((obsRxCh == OBS_RX1_TXLO) || (obsRxCh == OBS_RX1_SNIFFERLO))
    {
        if (gainIndex <= device->obsRx->orxGainCtrl->maxGainIndex && gainIndex >= device->obsRx->orxGainCtrl->minGainIndex)
        {
            CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_ORX1_MANUAL_GAIN_INDEX, (gainIndex - MIN_ORX_GAIN_TABLE_INDEX));
            device->obsRx->orxGainCtrl->orx1GainIndex = gainIndex;
        }
        else
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SETORXGAIN_INV_ORX1GAIN,
                    getMykonosErrorMessage(MYKONOS_ERR_SETORXGAIN_INV_ORX1GAIN));
            return MYKONOS_ERR_SETORXGAIN_INV_ORX1GAIN;
        }
    }
    else if ((obsRxCh == OBS_RX2_TXLO) || (obsRxCh == OBS_RX2_SNIFFERLO))
    {
        if (gainIndex <= device->obsRx->orxGainCtrl->maxGainIndex && gainIndex >= device->obsRx->orxGainCtrl->minGainIndex)
        {
            CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_ORX2_MANUAL_GAIN_INDEX, (gainIndex - MIN_ORX_GAIN_TABLE_INDEX));
            device->obsRx->orxGainCtrl->orx2GainIndex = gainIndex;
        }
        else
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SETORXGAIN_INV_ORX2GAIN,
                    getMykonosErrorMessage(MYKONOS_ERR_SETORXGAIN_INV_ORX2GAIN));
            return MYKONOS_ERR_SETORXGAIN_INV_ORX2GAIN;
        }

    }
    else if (obsRxCh == OBS_SNIFFER_A || obsRxCh == OBS_SNIFFER_B || obsRxCh == OBS_SNIFFER_C || obsRxCh == OBS_SNIFFER)
    {
        if (gainIndex <= device->obsRx->snifferGainCtrl->maxGainIndex && gainIndex >= device->obsRx->snifferGainCtrl->minGainIndex)
        {
            CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_SNRX_MANUAL_GAIN_INDEX, (gainIndex - MIN_SNRX_GAIN_TABLE_INDEX));
            device->obsRx->snifferGainCtrl->gainIndex = gainIndex;
        }
        else
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SETORXGAIN_INV_SNRXGAIN,
                    getMykonosErrorMessage(MYKONOS_ERR_SETORXGAIN_INV_SNRXGAIN));
            return MYKONOS_ERR_SETORXGAIN_INV_SNRXGAIN;
        }
    }
    else
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SETORXGAIN_INV_CHANNEL,
                getMykonosErrorMessage(MYKONOS_ERR_SETORXGAIN_INV_CHANNEL));
        return MYKONOS_ERR_SETORXGAIN_INV_CHANNEL;
    }

    return MYKONOS_ERR_OK;
}

/**
 * \brief Gets the gain index of the currently enabled ObsRx channel
 *
 * The ObsRx data path can have multiple RF sources.  This function will
 * read back the gain index of the currently enabled RF source. If the ObsRx
 * data path is disabled, an error is returned.  If the functions uint8_t *gainIndex
 * parameter is a valid pointer, the gain index is returned at the pointers
 * address.  Else, if the uint8_t *gainIndex pointer is NULL, the gainIndex read back
 * is stored in the device data structure.
 *
 * NOTE: if the observation source is chosen by ORX_MODE pins, it is possible that
 * the readback value will be incorrect if the obsRx channel changes while this function
 * reads the gain.
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 * - device->profilesValid
 * - device->obsRx->orxGainCtrl->orx1GainIndex
 * - device->obsRx->orxGainCtrl->orx2GainIndex
 * - device->obsRx->snifferGainCtrl->gainIndex
 *
 * \param device Pointer to the Mykonos device data structure
 * \param gainIndex Return value of the current gain index for the currently enabled ObsRx channel.
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_GETORX1GAIN_INV_POINTER The ObsRx profile is not valid in the device data structure
 * \retval MYKONOS_ERR_GETORX2GAIN_INV_POINTER The ObsRx profile is not valid in the device data structure
 * \retval MYKONOS_ERR_GETSNIFFGAIN_INV_POINTER The sniffer profile is not valid in the device data structure
 * \retval MYKONOS_ERR_GETOBSRXGAIN_CH_DISABLED The observation receiver is currently disabled, can not read back gain index.
 */
mykonosErr_t MYKONOS_getObsRxGain(mykonosDevice_t *device, uint8_t *gainIndex)
{
    /* Potential issue, if ARM is using loopback path when this function is called, it will likely readback loopback gain */
    uint8_t readObsRxPathEnabled = 0;
    uint8_t readObsRxGain = 0;

    const uint8_t ORX1_EN = 0x01;
    const uint8_t ORX2_EN = 0x02;
    const uint8_t SNRX_EN = 0x04;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_getObsRxGain()\n");
#endif

    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_DPD_SNIFFER_CONFIGURATION_CONTROL_1, &readObsRxPathEnabled);

    /* If obsRx channel enabled, then read gain index */
    if (readObsRxPathEnabled > 0)
    {
        CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_GAIN_CTL_ORX_SNRX_GAIN, &readObsRxGain);

        switch (readObsRxPathEnabled)
        {
            case 0x01:
                readObsRxGain += MIN_ORX_GAIN_TABLE_INDEX;
                break;
            case 0x02:
                readObsRxGain += MIN_ORX_GAIN_TABLE_INDEX;
                break;
            case 0x04:
                readObsRxGain += MIN_SNRX_GAIN_TABLE_INDEX;
                break;
            default:
                break;
        }

        if (gainIndex != NULL)
        {
            *gainIndex = readObsRxGain;
        }

        /* Store the gain index in the device's gain control data structure. */

        if (readObsRxPathEnabled == ORX1_EN)
        {
            /* verify valid data structure pointers */
            if (device->profilesValid & ORX_PROFILE_VALID)
            {
                device->obsRx->orxGainCtrl->orx1GainIndex = readObsRxGain;
            }
            else
            {
                CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETORX1GAIN_INV_POINTER,
                        getMykonosErrorMessage(MYKONOS_ERR_GETORX1GAIN_INV_POINTER));
                return MYKONOS_ERR_GETORX1GAIN_INV_POINTER;
            }

        }
        else if (readObsRxPathEnabled == ORX2_EN)
        {
            /* verify valid pointers */
            if (device->profilesValid & ORX_PROFILE_VALID)
            {
                device->obsRx->orxGainCtrl->orx2GainIndex = readObsRxGain;
            }
            else
            {
                CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETORX2GAIN_INV_POINTER,
                        getMykonosErrorMessage(MYKONOS_ERR_GETORX2GAIN_INV_POINTER));
                return MYKONOS_ERR_GETORX2GAIN_INV_POINTER;
            }

        }
        else if (readObsRxPathEnabled == SNRX_EN)
        {
            /* verify valid pointers */
            if (device->profilesValid & SNIFF_PROFILE_VALID)
            {
                device->obsRx->snifferGainCtrl->gainIndex = readObsRxGain;
            }
            else
            {
                CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETSNIFFGAIN_INV_POINTER,
                        getMykonosErrorMessage(MYKONOS_ERR_GETSNIFFGAIN_INV_POINTER));
                return MYKONOS_ERR_GETSNIFFGAIN_INV_POINTER;
            }
        }
        else
        {
            /* ignore gain index for Tx loopback input into obsRx channel */
        }
    }
    else
    {
        CMB_writeToLog(ADIHAL_LOG_WARNING, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETOBSRXGAIN_CH_DISABLED,
                getMykonosErrorMessage(MYKONOS_ERR_GETOBSRXGAIN_CH_DISABLED));
        return MYKONOS_ERR_GETOBSRXGAIN_CH_DISABLED;
    }

    return MYKONOS_ERR_OK;
}

/**
 * \brief Sets up the device ObsRx Automatic Gain Control (AGC) registers.
 *
 * Three data structures (of types mykonosAgcCfg_t, mykonosPeakDetAgcCfg_t, mykonosPowerMeasAgcCfg_t)
 * must be instantiated prior to calling this function. Valid ranges for data structure members
 * must also be provided.
 *
 *
 * <B>Dependencies:</B>
 * - device->spiSettings
 * - device->spiSettings->chipSelectIndex
 * - device->obsRx->orxAgcCtrl->agcRx1MaxGainIndex
 * - device->obsRx->orxAgcCtrl->agcRx1MinGainIndex
 * - device->obsRx->orxAgcCtrl->agcRx2MaxGainIndex
 * - device->obsRx->orxAgcCtrl->agcRx2MinGainIndex
 * - device->obsRx->orxAgcCtrl->agcObsRxMaxGainIndex
 * - device->obsRx->orxAgcCtrl->agcObsRxMinGainIndex
 * - device->obsRx->orxAgcCtrl->agcObsRxSelect
 * - device->obsRx->orxAgcCtrl->agcPeakThresholdMode
 * - device->obsRx->orxAgcCtrl->agcLowThsPreventGainIncrease
 * - device->obsRx->orxAgcCtrl->agcGainUpdateCounter
 * - device->obsRx->orxAgcCtrl->agcSlowLoopSettlingDelay
 * - device->obsRx->orxAgcCtrl->agcPeakWaitTime
 * - device->obsRx->orxAgcCtrl->agcResetOnRxEnable
 * - device->obsRx->orxAgcCtrl->agcEnableSyncPulseForGainCounter
 * - device->obsRx->orxAgcCtrl->peakAgc->apdHighThresh
 * - device->obsRx->orxAgcCtrl->peakAgc->apdLowThresh
 * - device->obsRx->orxAgcCtrl->peakAgc->hb2HighThresh
 * - device->obsRx->orxAgcCtrl->peakAgc->hb2LowThresh
 * - device->obsRx->orxAgcCtrl->peakAgc->hb2VeryLowThresh
 * - device->obsRx->orxAgcCtrl->peakAgc->apdHighThreshExceededCnt
 * - device->obsRx->orxAgcCtrl->peakAgc->apdLowThreshExceededCnt
 * - device->obsRx->orxAgcCtrl->peakAgc->hb2HighThreshExceededCnt
 * - device->obsRx->orxAgcCtrl->peakAgc->hb2LowThreshExceededCnt
 * - device->obsRx->orxAgcCtrl->peakAgc->hb2VeryLowThreshExceededCnt
 * - device->obsRx->orxAgcCtrl->peakAgc->apdHighGainStepAttack
 * - device->obsRx->orxAgcCtrl->peakAgc->apdLowGainStepRecovery
 * - device->obsRx->orxAgcCtrl->peakAgc->hb2HighGainStepAttack
 * - device->obsRx->orxAgcCtrl->peakAgc->hb2LowGainStepRecovery
 * - device->obsRx->orxAgcCtrl->peakAgc->hb2VeryLowGainStepRecovery
 * - device->obsRx->orxAgcCtrl->peakAgc->apdFastAttack
 * - device->obsRx->orxAgcCtrl->peakAgc->hb2FastAttack
 * - device->obsRx->orxAgcCtrl->peakAgc->hb2OverloadDetectEnable
 * - device->obsRx->orxAgcCtrl->peakAgc->hb2OverloadDurationCnt
 * - device->obsRx->orxAgcCtrl->peakAgc->hb2OverloadThreshCnt
 * - device->obsRx->orxAgcCtrl->powerAgc->pmdUpperHighThresh
 * - device->obsRx->orxAgcCtrl->powerAgc->pmdUpperLowThresh
 * - device->obsRx->orxAgcCtrl->powerAgc->pmdLowerHighThresh
 * - device->obsRx->orxAgcCtrl->powerAgc->pmdLowerLowThresh
 * - device->obsRx->orxAgcCtrl->powerAgc->pmdUpperHighGainStepAttack
 * - device->obsRx->orxAgcCtrl->powerAgc->pmdUpperLowGainStepAttack
 * - device->obsRx->orxAgcCtrl->powerAgc->pmdLowerHighGainStepRecovery
 * - device->obsRx->orxAgcCtrl->powerAgc->pmdLowerLowGainStepRecovery
 * - device->obsRx->orxAgcCtrl->powerAgc->pmdMeasDuration
 * - device->obsRx->orxAgcCtrl->powerAgc->pmdMeasConfig
 *
 * \param device is structure pointer to the Mykonos data structure containing settings
 * The pointer to the Mykonos AGC data structure containing settings is checked for a null pointer
 * to ensure it has been initialized. If not an error is thrown.
 *
 * \retval Returns MYKONOS_ERR=pass, !MYKONOS_ERR=fail
 * \retval MYKONOS_ERR_INV_AGC_OBSRX_STRUCT_INIT
 * \retval MYKONOS_ERR_INV_AGC_OBSRX_PEAK_STRUCT_INIT
 * \retval MYKONOS_ERR_INV_AGC_OBSRX_PWR_STRUCT_INIT
 * \retval MYKONOS_ERR_INV_AGC_OBSRX_MAX_GAIN_INDEX
 * \retval MYKONOS_ERR_INV_AGC_OBSRX_MIN_GAIN_INDEX
 * \retval MYKONOS_ERR_INV_AGC_OBSRX_SELECT
 * \retval MYKONOS_ERR_INV_AGC_OBSRX_GAIN_UPDATE_TIME_PARM
 * \retval MYKONOS_ERR_INV_AGC_OBSRX_PEAK_WAIT_TIME_PARM
 * \retval MYKONOS_ERR_INV_AGC_OBSRX_SLOW_LOOP_SETTLING_DELAY
 * \retval MYKONOS_ERR_INV_AGC_OBSRX_PMD_MEAS_DURATION
 * \retval MYKONOS_ERR_INV_AGC_OBSRX_PMD_MEAS_CONFIG
 * \retval MYKONOS_ERR_INV_AGC_OBSRX_LOW_THS_PREV_GAIN_INC
 * \retval MYKONOS_ERR_INV_AGC_OBSRX_PEAK_THRESH_MODE
 * \retval MYKONOS_ERR_INV_AGC_OBSRX_RESET_ON_RX_ENABLE
 * \retval MYKONOS_ERR_INV_AGC_OBSRX_ENABLE_SYNC_PULSE_GAIN_COUNTER
 * \retval MYKONOS_ERR_INV_AGC_OBSRX_PMD_LOWER_HIGH_THRESH
 * \retval MYKONOS_ERR_INV_AGC_OBSRX_PMD_UPPER_LOW_THRESH
 * \retval MYKONOS_ERR_INV_AGC_OBSRX_PMD_LOWER_LOW_THRESH
 * \retval MYKONOS_ERR_INV_AGC_OBSRX_PMD_UPPER_HIGH_THRESH
 * \retval MYKONOS_ERR_INV_AGC_OBSRX_PMD_UPPER_HIGH_GAIN_STEP
 * \retval MYKONOS_ERR_INV_AGC_OBSRX_PMD_LOWER_LOW_GAIN_STEP
 * \retval MYKONOS_ERR_INV_AGC_OBSRX_PMD_UPPER_LOW_GAIN_STEP
 * \retval MYKONOS_ERR_INV_AGC_OBSRX_PMD_LOWER_HIGH_GAIN_STEP
 * \retval MYKONOS_ERR_INV_AGC_OBSRX_PKDET_FAST_ATTACK_VALUE
 * \retval MYKONOS_ERR_INV_AGC_OBSRX_APD_HIGH_THRESH_PARM
 * \retval MYKONOS_ERR_INV_AGC_OBSRX_APD_LOW_THRESH_PARM
 * \retval MYKONOS_ERR_INV_AGC_OBSRX_HB2_HIGH_THRESH_PARM
 * \retval MYKONOS_ERR_INV_AGC_OBSRX_HB2_LOW_THRESH_PARM
 * \retval MYKONOS_ERR_INV_AGC_OBSRX_HB2_VERY_LOW_THRESH_PARM
 * \retval MYKONOS_ERR_INV_AGC_OBSRX_APD_HIGH_GAIN_STEP_PARM
 * \retval MYKONOS_ERR_INV_AGC_OBSRX_APD_LOW_GAIN_STEP_PARM
 * \retval MYKONOS_ERR_INV_AGC_OBSRX_HB2_HIGH_GAIN_STEP_PARM
 * \retval MYKONOS_ERR_INV_AGC_OBSRX_HB2_LOW_GAIN_STEP_PARM
 * \retval MYKONOS_ERR_INV_AGC_OBSRX_HB2_VERY_LOW_GAIN_STEP_PARM
 * \retval MYKONOS_ERR_INV_AGC_OBSRX_HB2_OVLD_ENABLE
 * \retval MYKONOS_ERR_INV_AGC_OBSRX_HB2_OVLD_DUR_CNT
 * \retval MYKONOS_ERR_INV_AGC_OBSRX_HB2_OVLD_THRESH_CNT
 */
mykonosErr_t MYKONOS_setupObsRxAgc(mykonosDevice_t *device)
{
    /* Current configuration does not support AGC on ORx channel */
    uint8_t decPowerConfig = 0;
    uint8_t lower1ThreshGainStepRegValue = 0;
    uint8_t powerThresholdsRegValue = 0;
    uint8_t hb2OvldCfgRegValue = 0;
    uint8_t agcGainUpdateCtr[3] = {0};

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_setupObsRxAgc()\n");
#endif

    /* Check mykonosAgcCfg_t device->obsRx->orxAgcCtrl pointer for initialization */
    if (&device->obsRx->orxAgcCtrl == 0)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_OBSRX_STRUCT_INIT,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_OBSRX_STRUCT_INIT));
        return MYKONOS_ERR_INV_AGC_OBSRX_STRUCT_INIT;
    }

    /* Check mykonosPeakDetAgcCfg_t device->orx->orxAgcCtrl->peakAgc pointer for initialization */
    if (&device->obsRx->orxAgcCtrl->peakAgc == 0)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_OBSRX_PEAK_STRUCT_INIT,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_OBSRX_PEAK_STRUCT_INIT));
        return MYKONOS_ERR_INV_AGC_OBSRX_PEAK_STRUCT_INIT;
    }

    /* Check mykonosPowerMeasAgcCfg_t device->obsRx->orxAgcCtrl->powerAgc pointer for initialization */
    if (&device->obsRx->orxAgcCtrl->powerAgc == 0)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_OBSRX_PWR_STRUCT_INIT,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_OBSRX_PWR_STRUCT_INIT));
        return MYKONOS_ERR_INV_AGC_OBSRX_PWR_STRUCT_INIT;
    }

    /* Range check agcObsRxMaxGainIndex versus gain table limits and agcObsRxMinGainIndex */
    if ((device->obsRx->orxAgcCtrl->agcObsRxMaxGainIndex > device->obsRx->snifferGainCtrl->maxGainIndex) ||
        (device->obsRx->orxAgcCtrl->agcObsRxMaxGainIndex < device->obsRx->orxAgcCtrl->agcObsRxMinGainIndex))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_OBSRX_MAX_GAIN_INDEX,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_OBSRX_MAX_GAIN_INDEX));
        return MYKONOS_ERR_INV_AGC_OBSRX_MAX_GAIN_INDEX;
    }
    else
    {
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_ORX_SNRX_MAX_GAIN_INDEX,
                (device->obsRx->orxAgcCtrl->agcObsRxMaxGainIndex - MAX_SNRX_GAIN_TABLE_NUMINDEXES));
    }

    /* Range check agcObsRxMinGainIndex versus gain table limits and agcObsRxMaxGainIndex */
    if ((device->obsRx->orxAgcCtrl->agcObsRxMinGainIndex < device->obsRx->snifferGainCtrl->minGainIndex) ||
        (device->obsRx->orxAgcCtrl->agcObsRxMaxGainIndex < device->obsRx->orxAgcCtrl->agcObsRxMinGainIndex))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_OBSRX_MIN_GAIN_INDEX,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_OBSRX_MIN_GAIN_INDEX));
        return MYKONOS_ERR_INV_AGC_OBSRX_MIN_GAIN_INDEX;
    }
    else
    {
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_ORX_SNRX_MIN_GAIN_INDEX,
                (device->obsRx->orxAgcCtrl->agcObsRxMinGainIndex - MAX_SNRX_GAIN_TABLE_NUMINDEXES));
    }

    /* Range check agcObsRxSelect. Sniffer only support. Sniffer is selected if value is 0 or 1. 2 or 3 select ORx1/ORx2 respectively */
    if (device->obsRx->orxAgcCtrl->agcObsRxSelect > 1)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_OBSRX_SELECT,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_OBSRX_SELECT));
        return MYKONOS_ERR_INV_AGC_OBSRX_SELECT;
    }
    else
    {
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_ORX_SNRX_ACTIVE, device->obsRx->orxAgcCtrl->agcObsRxSelect);
    }

    /* Range check for agcGainUpdateCounter (22-bit) */
    if ((device->obsRx->orxAgcCtrl->agcGainUpdateCounter > 0x3FFFFF) ||
        (device->obsRx->orxAgcCtrl->agcGainUpdateCounter < 1))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_OBSRX_GAIN_UPDATE_TIME_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_OBSRX_GAIN_UPDATE_TIME_PARM));
        return MYKONOS_ERR_INV_AGC_OBSRX_GAIN_UPDATE_TIME_PARM;
    }
    else
    {
        /* Split agcGainUpdateCounter into three values */
        agcGainUpdateCtr[0] = (uint8_t)(device->obsRx->orxAgcCtrl->agcGainUpdateCounter);
        agcGainUpdateCtr[1] = (uint8_t)(device->obsRx->orxAgcCtrl->agcGainUpdateCounter >> 8);
        agcGainUpdateCtr[2] = (uint8_t)(device->obsRx->orxAgcCtrl->agcGainUpdateCounter >> 16);

        /* Write two bytes directly due. Third word has its upper two bits masked off.  */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_SLOW_ORX_SNRX_GAIN_UPDATE_CTR_1, agcGainUpdateCtr[0]);
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_SLOW_ORX_SNRX_GAIN_UPDATE_CTR_2, agcGainUpdateCtr[1]);
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_AGC_SLOW_ORX_SNRX_GAIN_UPDATE_CTR_3, agcGainUpdateCtr[2], 0x3F, 0);
    }

    /* Range check on agcPeakWaitTime (5-bit) */
    if ((device->obsRx->orxAgcCtrl->agcPeakWaitTime > 0x1F) ||
        (device->obsRx->orxAgcCtrl->agcPeakWaitTime < 0x02))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_OBSRX_PEAK_WAIT_TIME_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_OBSRX_PEAK_WAIT_TIME_PARM));
        return MYKONOS_ERR_INV_AGC_OBSRX_PEAK_WAIT_TIME_PARM;
    }
    else
    {
        /* Write agcPeakWaitTime */
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_AGC_ORX_SNRX_CFG_2, device->obsRx->orxAgcCtrl->agcPeakWaitTime, 0x1F, 0);
    }

    /* Range check for agcSlowLoopSettlingDelay (7-bit) */
    if (device->obsRx->orxAgcCtrl->agcSlowLoopSettlingDelay > 0x7F)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_OBSRX_SLOW_LOOP_SETTLING_DELAY,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_OBSRX_SLOW_LOOP_SETTLING_DELAY));
        return MYKONOS_ERR_INV_AGC_OBSRX_SLOW_LOOP_SETTLING_DELAY;
    }
    else
    {
        /* Write agcSlowLoopSettlingDelay */
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_AGC_SLOW_ORX_SNRX_LOOP_CFG, device->obsRx->orxAgcCtrl->agcSlowLoopSettlingDelay, 0x7F, 0);
    }

    /* Range check for pmdMeasDuration */
    if ((1 << (3 + device->obsRx->orxAgcCtrl->powerAgc->pmdMeasDuration)) >= (device->obsRx->orxAgcCtrl->agcGainUpdateCounter))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_OBSRX_PMD_MEAS_DURATION,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_OBSRX_PMD_MEAS_DURATION));
        return MYKONOS_ERR_INV_AGC_OBSRX_PMD_MEAS_DURATION;
    }
    else
    {
        /* Write pmdMeasDuration */
        CMB_SPIWriteField(device->spiSettings, MYKONOS_SNIFFER_DEC_POWER_CONFIG_2, device->obsRx->orxAgcCtrl->powerAgc->pmdMeasDuration, 0x0F, 0);
    }

    /* Range check for pmdMeasConfig */
    if (device->obsRx->orxAgcCtrl->powerAgc->pmdMeasConfig > 0x3)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_OBSRX_PMD_MEAS_CONFIG,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_OBSRX_PMD_MEAS_CONFIG));
        return MYKONOS_ERR_INV_AGC_OBSRX_PMD_MEAS_CONFIG;
    }
    else
    {
        if (device->obsRx->orxAgcCtrl->powerAgc->pmdMeasConfig == 0)
        {
            decPowerConfig = 0x0; /* Dec Pwr measurement disable */
        }
        else if (device->obsRx->orxAgcCtrl->powerAgc->pmdMeasConfig == 1)
        {
            decPowerConfig = 0x3; /* Dec Pwr measurement enable,  HB2 for decPwr measurement */
        }
        else if (device->obsRx->orxAgcCtrl->powerAgc->pmdMeasConfig == 2)
        {
            decPowerConfig = 0x5; /* Dec Pwr measurement enable, RFIR for decPwr measurement */
        }
        else if (device->obsRx->orxAgcCtrl->powerAgc->pmdMeasConfig == 3)
        {
            decPowerConfig = 0x11; /* Dec Pwr measurement enable, BBDC2 for decPwr measurement */
        }
        /* Write pmdMeasConfig */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_SNIFFER_DEC_POWER_CONFIG_1, decPowerConfig);
    }

    /* Range check agcLowThsPreventGainIncrease (1-bit) */
    if (device->obsRx->orxAgcCtrl->agcLowThsPreventGainIncrease > 1)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_OBSRX_LOW_THS_PREV_GAIN_INC,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_OBSRX_LOW_THS_PREV_GAIN_INC));
        return MYKONOS_ERR_INV_AGC_OBSRX_LOW_THS_PREV_GAIN_INC;
    }
    else
    {
        /* Write agcLowThsPreventGainIncrease */
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_AGC_SLOW_ORX_SNRX_LOCK_LEV_THRSH, device->obsRx->orxAgcCtrl->agcLowThsPreventGainIncrease, 0x80, 7);
    }

    /* Range check agcPeakThresholdMode (1-bit),  */
    if (device->obsRx->orxAgcCtrl->agcPeakThresholdMode > 1)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_OBSRX_PEAK_THRESH_MODE,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_OBSRX_PEAK_THRESH_MODE));
        return MYKONOS_ERR_INV_AGC_OBSRX_PEAK_THRESH_MODE;
    }
    else
    {
        /* Save to lower1ThreshGainStepRegValue register variable */
        lower1ThreshGainStepRegValue |= (device->obsRx->orxAgcCtrl->agcPeakThresholdMode << 5);
    }

    /* Range check agcResetOnRxEnable (1-bit) */
    if (device->obsRx->orxAgcCtrl->agcResetOnRxEnable > 1)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_OBSRX_RESET_ON_RX_ENABLE,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_OBSRX_RESET_ON_RX_ENABLE));
        return MYKONOS_ERR_INV_AGC_OBSRX_RESET_ON_RX_ENABLE;
    }
    else
    {
        /* Write agcResetOnRxEnable */
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_AGC_SLOW_ORX_SNRX_GAIN_UPDATE_CTR_3, (device->obsRx->orxAgcCtrl->agcResetOnRxEnable << 7), 0x80, 0);
    }

    /* Range check agcEnableSyncPulseForGainCounter (1-bit) */
    if (device->obsRx->orxAgcCtrl->agcEnableSyncPulseForGainCounter > 1)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_OBSRX_ENABLE_SYNC_PULSE_GAIN_COUNTER,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_OBSRX_ENABLE_SYNC_PULSE_GAIN_COUNTER));
        return MYKONOS_ERR_INV_AGC_OBSRX_ENABLE_SYNC_PULSE_GAIN_COUNTER;
    }
    else
    {
        /* Write agcEnableSyncPulseForGainCounter */
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_AGC_SLOW_ORX_SNRX_LOOP_CFG, (device->obsRx->orxAgcCtrl->agcEnableSyncPulseForGainCounter << 7), 0x80, 0);
    }

    /* WRITE REGISTERS FOR THE AGC POWER MEASUREMENT DETECTOR (PMD) STRUCTURE */

    /* Range check pmdLowerHighThresh (7-bit) vs 0x7F and pmdUpperLowThresh */
    if ((device->obsRx->orxAgcCtrl->powerAgc->pmdLowerHighThresh <= device->obsRx->orxAgcCtrl->powerAgc->pmdUpperLowThresh) ||
        (device->obsRx->orxAgcCtrl->powerAgc->pmdLowerHighThresh > 0x7F))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_OBSRX_PMD_LOWER_HIGH_THRESH,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_OBSRX_PMD_LOWER_HIGH_THRESH));
        return MYKONOS_ERR_INV_AGC_OBSRX_PMD_LOWER_HIGH_THRESH;
    }
    else
    {
        /* Write pmdLowerHighThresh */
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_AGC_SLOW_ORX_SNRX_LOCK_LEV_THRSH, device->obsRx->orxAgcCtrl->powerAgc->pmdLowerHighThresh, 0x7F, 0);
    }

    /* Range check pmdUpperLowThresh (7-bit): Comparison to pmdLowerHigh done earlier */
    if (device->obsRx->orxAgcCtrl->powerAgc->pmdUpperLowThresh > 0x7F)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_OBSRX_PMD_UPPER_LOW_THRESH,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_OBSRX_PMD_UPPER_LOW_THRESH));
        return MYKONOS_ERR_INV_AGC_OBSRX_PMD_UPPER_LOW_THRESH;
    }
    else
    {
        /* Write pmdUpperLowThresh */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_ORX_SNRX_LOCK_LEVEL, device->obsRx->orxAgcCtrl->powerAgc->pmdUpperLowThresh);
    }

    /* Range check pmdLowerLowThresh (4-bit)  */
    if (device->obsRx->orxAgcCtrl->powerAgc->pmdLowerLowThresh > 0xF)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_OBSRX_PMD_LOWER_LOW_THRESH,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_OBSRX_PMD_LOWER_LOW_THRESH));
        return MYKONOS_ERR_INV_AGC_OBSRX_PMD_LOWER_LOW_THRESH;
    }
    else
    {
        /* Write pmdUpperLowThresh to temp variable */
        powerThresholdsRegValue |= device->obsRx->orxAgcCtrl->powerAgc->pmdLowerLowThresh;
    }

    /* Range check pmdUpperHighThresh (4-bit)  */
    if (device->obsRx->orxAgcCtrl->powerAgc->pmdUpperHighThresh > 0xF)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_OBSRX_PMD_UPPER_HIGH_THRESH,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_OBSRX_PMD_UPPER_HIGH_THRESH));
        return MYKONOS_ERR_INV_AGC_OBSRX_PMD_UPPER_HIGH_THRESH;
    }
    else
    {
        /* Write pmdUpperHighThresh to temp var, then to register */
        powerThresholdsRegValue |= (device->obsRx->orxAgcCtrl->powerAgc->pmdUpperHighThresh << 4);
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_SLOW_ORX_SNRX_POWER_THRSH, powerThresholdsRegValue);
    }

    /* Range check pmdUpperHighGainStepAttack (5-bit)  */
    if (device->obsRx->orxAgcCtrl->powerAgc->pmdUpperHighGainStepAttack > 0x1F)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_OBSRX_PMD_UPPER_HIGH_GAIN_STEP,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_OBSRX_PMD_UPPER_HIGH_GAIN_STEP));
        return MYKONOS_ERR_INV_AGC_OBSRX_PMD_UPPER_HIGH_GAIN_STEP;
    }
    else
    {
        /* Write pmdUpperHighGainStepAttack */
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_AGC_SLOW_ORX_SNRX_UPPER1_THRSH_GAIN_STEP, device->obsRx->orxAgcCtrl->powerAgc->pmdUpperHighGainStepAttack,
                0x1F, 0);
    }

    /* Range check pmdLowerLowGainStepRecovery (5-bit)  */
    if (device->obsRx->orxAgcCtrl->powerAgc->pmdLowerLowGainStepRecovery > 0x1F)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_OBSRX_PMD_LOWER_LOW_GAIN_STEP,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_OBSRX_PMD_LOWER_LOW_GAIN_STEP));
        return MYKONOS_ERR_INV_AGC_OBSRX_PMD_LOWER_LOW_GAIN_STEP;
    }
    else
    {
        /* Write pmdLowerLowGainStepRecovery to temp var, then to register */
        lower1ThreshGainStepRegValue |= (device->obsRx->orxAgcCtrl->powerAgc->pmdLowerLowGainStepRecovery);
    }

    /* Range check pmdUpperLowGainStepRecovery (5-bit)  */
    if (device->obsRx->orxAgcCtrl->powerAgc->pmdUpperLowGainStepAttack > 0x1F)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_OBSRX_PMD_UPPER_LOW_GAIN_STEP,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_OBSRX_PMD_UPPER_LOW_GAIN_STEP));
        return MYKONOS_ERR_INV_AGC_OBSRX_PMD_UPPER_LOW_GAIN_STEP;
    }
    else
    {
        /* Write pmdUpperLowGainStepRecovery to register */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_SLOW_ORX_SNRX_UPPER0_THRSH_GAIN_STEP, device->obsRx->orxAgcCtrl->powerAgc->pmdUpperLowGainStepAttack);
    }

    /* Range check pmdLowerHighGainStepRecovery (5-bit)  */
    if (device->obsRx->orxAgcCtrl->powerAgc->pmdLowerHighGainStepRecovery > 0x1F)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_OBSRX_PMD_LOWER_HIGH_GAIN_STEP,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_OBSRX_PMD_LOWER_HIGH_GAIN_STEP));
        return MYKONOS_ERR_INV_AGC_OBSRX_PMD_LOWER_HIGH_GAIN_STEP;
    }
    else
    {
        /* Write pmdLowerLowGainStepRecovery to temp var, then to register */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_SLOW_ORX_SNRX_LOWER0_THRSH_GAIN_STEP, device->obsRx->orxAgcCtrl->powerAgc->pmdLowerHighGainStepRecovery);
    }

    /* WRITE REGISTERS FOR THE AGC PEAK DETECTOR (APD/HB2) STRUCTURE */

    /* Range check apdFastAttack and hb2FastAttack (1-bit)  */
    if ((device->obsRx->orxAgcCtrl->peakAgc->apdFastAttack > 0x1) ||
        (device->obsRx->orxAgcCtrl->peakAgc->hb2FastAttack > 0x1))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_OBSRX_PKDET_FAST_ATTACK_VALUE,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_OBSRX_PKDET_FAST_ATTACK_VALUE));
        return MYKONOS_ERR_INV_AGC_OBSRX_PKDET_FAST_ATTACK_VALUE;
    }
    else
    {
        /* Write pmdLowerLowGainStepRecovery to temp var, then to register */
        lower1ThreshGainStepRegValue |= (device->obsRx->orxAgcCtrl->peakAgc->apdFastAttack << 7);
        lower1ThreshGainStepRegValue |= (device->obsRx->orxAgcCtrl->peakAgc->hb2FastAttack << 6);
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_SLOW_ORX_SNRX_LOWER1_THRSH_GAIN_STEP, lower1ThreshGainStepRegValue);
    }

    /* Range check apdHighThresh */
    if ((device->obsRx->orxAgcCtrl->peakAgc->apdHighThresh > 0x3F) ||
        (device->obsRx->orxAgcCtrl->peakAgc->apdHighThresh <= device->obsRx->orxAgcCtrl->peakAgc->apdLowThresh))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_OBSRX_APD_HIGH_THRESH_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_OBSRX_APD_HIGH_THRESH_PARM));
        return MYKONOS_ERR_INV_AGC_OBSRX_APD_HIGH_THRESH_PARM;
    }
    else
    {
        /* Write apdHighThresh */
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_AGC_ORX_SNRX_ULB_THRSH, device->obsRx->orxAgcCtrl->peakAgc->apdHighThresh, 0X3F, 0);
    }

    /* Range check apdLowThresh */
    if ((device->obsRx->orxAgcCtrl->peakAgc->apdLowThresh > 0x3F) ||
        (device->obsRx->orxAgcCtrl->peakAgc->apdHighThresh < device->obsRx->orxAgcCtrl->peakAgc->apdLowThresh))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_OBSRX_APD_LOW_THRESH_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_OBSRX_APD_LOW_THRESH_PARM));
        return MYKONOS_ERR_INV_AGC_OBSRX_APD_LOW_THRESH_PARM;
    }
    else
    {
        /* write apdLowThresh */
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_AGC_ORX_SNRX_LLB_THRSH, device->obsRx->orxAgcCtrl->peakAgc->apdLowThresh, 0x3F, 0);
    }

    /* Range check hb2HighThresh */
    if (device->obsRx->orxAgcCtrl->peakAgc->hb2HighThresh < device->obsRx->orxAgcCtrl->peakAgc->hb2LowThresh)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_OBSRX_HB2_HIGH_THRESH_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_OBSRX_HB2_HIGH_THRESH_PARM));
        return MYKONOS_ERR_INV_AGC_OBSRX_HB2_HIGH_THRESH_PARM;
    }
    else
    {
        /* write hb2HighThresh */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_ORX_SNRX_OVRLD_PD_DEC_OVRLD_UPPER_THRSH, device->obsRx->orxAgcCtrl->peakAgc->hb2HighThresh);
    }

    /* Range check hb2LowThresh */
    if (device->obsRx->orxAgcCtrl->peakAgc->hb2LowThresh > device->obsRx->orxAgcCtrl->peakAgc->hb2HighThresh)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_OBSRX_HB2_LOW_THRESH_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_OBSRX_HB2_LOW_THRESH_PARM));
        return MYKONOS_ERR_INV_AGC_OBSRX_HB2_LOW_THRESH_PARM;
    }
    else
    {
        /* write hb2LowThresh */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_ORX_SNRX_OVRLD_PD_DEC_OVRLD_LOWER_THRSH, device->obsRx->orxAgcCtrl->peakAgc->hb2LowThresh);
    }

    /* Range check hb2VeryLowThresh */
    if (device->obsRx->orxAgcCtrl->peakAgc->hb2VeryLowThresh > device->obsRx->orxAgcCtrl->peakAgc->hb2LowThresh)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_OBSRX_HB2_VERY_LOW_THRESH_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_OBSRX_HB2_VERY_LOW_THRESH_PARM));
        return MYKONOS_ERR_INV_AGC_OBSRX_HB2_VERY_LOW_THRESH_PARM;
    }
    else
    {
        /* write hb2VeryLowThresh */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_ORX_SNRX_OVRLD_PD_DEC_OVRLD_VERYLOW_THRSH, device->obsRx->orxAgcCtrl->peakAgc->hb2VeryLowThresh);
    }

    /* Write threshold counter values for apdHigh/apdLow/hb2High/hb2Low/hb2VeryLow */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_SLOW_ORX_SNRX_ULB_CNT_THRSH, device->obsRx->orxAgcCtrl->peakAgc->apdHighThreshExceededCnt);
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_SLOW_ORX_SNRX_LLB_CNT_THRSH, device->obsRx->orxAgcCtrl->peakAgc->apdLowThreshExceededCnt);
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_SLOW_ORX_SNRX_ADC_HIGH_OVRG_CNT_THRSH, device->obsRx->orxAgcCtrl->peakAgc->hb2HighThreshExceededCnt);
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_SLOW_ORX_SNRX_ADC_LOW_OVRG_CNT_THRSH, device->obsRx->orxAgcCtrl->peakAgc->hb2LowThreshExceededCnt);
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_SLOW_ORX_SNRX_ADC_VERYLOW_OVRG_CNT_THRSH, device->obsRx->orxAgcCtrl->peakAgc->hb2VeryLowThreshExceededCnt);

    /* Range check on apdHighGainStepAttack (5-bit) */
    if (device->obsRx->orxAgcCtrl->peakAgc->apdHighGainStepAttack > 0x1F)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_OBSRX_APD_HIGH_GAIN_STEP_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_OBSRX_APD_HIGH_GAIN_STEP_PARM));
        return MYKONOS_ERR_INV_AGC_OBSRX_APD_HIGH_GAIN_STEP_PARM;
    }
    else
    {
        /* Write apdHighGainStepAttack */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_ORX_SNRX_OVRG_GAIN_STEP_1, device->obsRx->orxAgcCtrl->peakAgc->apdHighGainStepAttack);
    }

    /* Range check on apdLowGainStepRecovery (5-bit) */
    if (device->obsRx->orxAgcCtrl->peakAgc->apdLowGainStepRecovery > 0x1F)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_OBSRX_APD_LOW_GAIN_STEP_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_OBSRX_APD_LOW_GAIN_STEP_PARM));
        return MYKONOS_ERR_INV_AGC_OBSRX_APD_LOW_GAIN_STEP_PARM;
    }
    else
    {
        /* Write apdLowGainStepRecovery */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_ORX_SNRX_OVRG_GAIN_STEP_4, device->obsRx->orxAgcCtrl->peakAgc->apdLowGainStepRecovery);
    }

    /* Range check on hb2HighGainStepAttack (5-bit) */
    if (device->obsRx->orxAgcCtrl->peakAgc->hb2HighGainStepAttack > 0x1F)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_OBSRX_HB2_HIGH_GAIN_STEP_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_OBSRX_HB2_HIGH_GAIN_STEP_PARM));
        return MYKONOS_ERR_INV_AGC_OBSRX_HB2_HIGH_GAIN_STEP_PARM;
    }
    else
    {
        /* Write hb2HighGainStepAttack */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_ORX_SNRX_OVRG_GAIN_STEP_2, device->obsRx->orxAgcCtrl->peakAgc->hb2HighGainStepAttack);
    }

    /* Range check on hb2LowGainStepRecovery (5-bit) */
    if (device->obsRx->orxAgcCtrl->peakAgc->hb2LowGainStepRecovery > 0x1F)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_OBSRX_HB2_LOW_GAIN_STEP_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_OBSRX_HB2_LOW_GAIN_STEP_PARM));
        return MYKONOS_ERR_INV_AGC_OBSRX_HB2_LOW_GAIN_STEP_PARM;
    }
    else
    {
        /* Write hb2LowGainStepRecovery */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_ORX_SNRX_OVRG_GAIN_STEP_5, device->obsRx->orxAgcCtrl->peakAgc->hb2LowGainStepRecovery);
    }

    /* Range check on hb2VeryLowGainStepRecovery (5-bit) */
    if (device->obsRx->orxAgcCtrl->peakAgc->hb2VeryLowGainStepRecovery > 0x1F)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_OBSRX_HB2_VERY_LOW_GAIN_STEP_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_OBSRX_HB2_VERY_LOW_GAIN_STEP_PARM));
        return MYKONOS_ERR_INV_AGC_OBSRX_HB2_VERY_LOW_GAIN_STEP_PARM;
    }
    else
    {
        /* Write hb2VeryLowGainStepRecovery */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_ORX_SNRX_OVRG_GAIN_STEP_6, device->obsRx->orxAgcCtrl->peakAgc->hb2VeryLowGainStepRecovery);
    }

    /* Range Check on hb2OverloadDetectEnable */
    if (device->obsRx->orxAgcCtrl->peakAgc->hb2OverloadDetectEnable > 0x1)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_OBSRX_HB2_OVLD_ENABLE,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_OBSRX_HB2_OVLD_ENABLE));
        return MYKONOS_ERR_INV_AGC_OBSRX_HB2_OVLD_ENABLE;
    }

    /* Range Check on hb2OverloadDetectEnable */
    if (device->obsRx->orxAgcCtrl->peakAgc->hb2OverloadDurationCnt > 0x7)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_OBSRX_HB2_OVLD_DUR_CNT,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_OBSRX_HB2_OVLD_DUR_CNT));
        return MYKONOS_ERR_INV_AGC_OBSRX_HB2_OVLD_DUR_CNT;
    }

    /* Range Check on hb2OverloadDetectEnable */
    if (device->obsRx->orxAgcCtrl->peakAgc->hb2OverloadThreshCnt > 0xF)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AGC_OBSRX_HB2_OVLD_THRESH_CNT,
                getMykonosErrorMessage(MYKONOS_ERR_INV_AGC_OBSRX_HB2_OVLD_THRESH_CNT));
        return MYKONOS_ERR_INV_AGC_OBSRX_HB2_OVLD_THRESH_CNT;
    }
    else
    {
        /* Write the hb2OvldCfgRegValue, the combination of hb2OverloadThreshCnt, hb2OverloadDurationCnt, and hb2OverloadDetectEnable */
        hb2OvldCfgRegValue = (device->obsRx->orxAgcCtrl->peakAgc->hb2OverloadThreshCnt) |
                             (device->obsRx->orxAgcCtrl->peakAgc->hb2OverloadDurationCnt << 4) |
                             (device->obsRx->orxAgcCtrl->peakAgc->hb2OverloadDetectEnable << 7);
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_ORX_SNRX_OVRLD_PD_DEC_OVRLD_CFG, hb2OvldCfgRegValue);
    }

    /* Hard-coded value for the ADC overload configuration. Sets the HB2 offset to -6dB.*/
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_ORX_SNRX_OVRLD_ADC_OVRLD_CFG, 0x18);
    /* Hard-coded value for APD decay setting. Setting allows for the quickest settling time of peak detector */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AGC_ORX_SNRX_BLOCK_DET_DECAY, 0x0);

    /* performing a soft reset */
    MYKONOS_resetRxAgc(device);

    return MYKONOS_ERR_OK;
}

/**
 * \brief Enables or disables the ObsRx slow loop gain counter to use the sync pulse
 *
 * <B>Dependencies:</B>
 * - device->spiSettings
 *
 * \param device is structure pointer to the Mykonos data structure containing the device SPI settings
 * \param enable is a uint8_t data type where '0' = disable ObsRx gain counter from using sync pulse, '1' = enable ObsRx gain. All other values will throw an error.
 *               counter to use the sync pulse. This value is written back to the device data structure for storage.
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 */
mykonosErr_t MYKONOS_enableObsRxGainCtrSyncPulse(mykonosDevice_t *device, uint8_t enable)
{
    const uint8_t syncPulseBitMask = 0x80;
    uint8_t enableBit = 0;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_enableObsRxGainCtrSyncPulse()\n");
#endif

    enableBit = (enable > 0) ? 1 : 0;

    /* read, modify, write ObsRx gain counter sync pulse enable bit */
    CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_AGC_SLOW_ORX_SNRX_LOOP_CFG, enableBit, syncPulseBitMask, 7);

    return MYKONOS_ERR_OK;
}

/**
 * \brief Configures the ObsRx gain control mode
 *
 * <B>Dependencies:</B>
 * - device->spiSettings
 * - device->obsRx->orxGainCtrl->gainMode
 *
 * \param device is structure pointer to the Mykonos data structure containing settings
 * \param mode is a mykonosGainMode_t enumerated gain control mode type
 *
 * When the mode enumerated type is passed, the ObsRx gain mode is set to this value and the
 * ObsRX agcType mykonosAgcCfg_t structure member is updated with the new value
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_INV_ORX_GAIN_MODE_PARM Invalid Observation Rx Gain control mode selected (use mykonosGainMode_t ENUM values)
 */
mykonosErr_t MYKONOS_setObsRxGainControlMode(mykonosDevice_t *device, mykonosGainMode_t mode)
{
    const uint8_t ORX_GAIN_TYPE_MASK = 0x3;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_setObsRxGainControlMode()\n");
#endif

    /* performing gain type check */
    switch (mode)
    {
        case MGC:
            break;
        case AGC:
            break;
        case HYBRID:
            break;
        default:
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_ORX_GAIN_MODE_PARM,
                    getMykonosErrorMessage(MYKONOS_ERR_INV_ORX_GAIN_MODE_PARM));
            return MYKONOS_ERR_INV_ORX_GAIN_MODE_PARM;
    }

    CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_AGC_ORX_SNRX_CFG_1, mode, ORX_GAIN_TYPE_MASK, 0);

    /* writing mode setting to data structure */
    if ((device->profilesValid & ORX_PROFILE_VALID) > 0)
    {
        device->obsRx->orxGainCtrl->gainMode = mode;
    }

    return MYKONOS_ERR_OK;
}

/**
 * \brief Performs a power measurement in the ObsRx digital data path.
 *
 * Due to interdependencies between the AGC and power measurement the power measurement duration and
 * where the measurement is taken is variable.
 * The location of the power measurement is given by device->obsRx->obsRxAgcCtrl->obsRxPwrAgc->pmdMeasConfig
 * The number of samples the power measurement uses is given by 8*2^(device->obsRx->obsRxAgcCtrl->obsRxPwrAgc->pmdMeasConfig) at the IQ rate,
 * if measured at RFIR output. This number of samples must be less than the agcGainUpdateCounter.
 * If the receiver is disabled during the power measurement, this function returns a 0 value for rx2DecPower_mdBFS
 *
 * The resolution of this function is 0.25dB.
 * The dynamic range of this function is 40dB. Signals lower than 40dBFS may not be measured accurately.
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 * - device->obsRx->obsRxAgcCtrl->obsRxPwrAgc->pmdMeasConfig
 * - device->obsRx->obsRxAgcCtrl->obsRxPwrAgc->pmdMeasConfig
 *
 * \param device Pointer to the Mykonos data structure
 * \param obsRxDecPower_mdBFS Pointer to store the Rx1 decimated power return.  Value returned in mdBFS
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 */
mykonosErr_t MYKONOS_getObsRxDecPower(mykonosDevice_t *device, uint16_t *obsRxDecPower_mdBFS)
{
    uint8_t obsRxDecPower_dBFS = 0;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_getObsRxDecPower()\n");
#endif

    /* read ObsRx Dec Power Measurement */
    if (obsRxDecPower_mdBFS != NULL)
    {
        CMB_SPIReadByte(device->spiSettings, MYKONOS_SNIFFER_DECIMATED_PWR, &obsRxDecPower_dBFS);
        *obsRxDecPower_mdBFS = (uint16_t)(obsRxDecPower_dBFS * 250); /* 250 = 1000 * 0.25dB */
    }
    else
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GET_OBSRX_DEC_POWER_NULL_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_GET_OBSRX_DEC_POWER_NULL_PARM));
        return MYKONOS_ERR_GET_OBSRX_DEC_POWER_NULL_PARM;
    }

    return MYKONOS_ERR_OK;
}

/*
 *****************************************************************************
 * Tx Data path functions
 *****************************************************************************
 */

/**
 * \brief Sets the Tx1 RF output Attenuation
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 * - device->spiSettings->chipSelectIndex
 * - device->tx->txAttenStepSize
 *
 * \param device Pointer to the Mykonos device's data structure
 * \param tx1Attenuation_mdB The desired TxAttenuation in milli-dB (Range: 0 to 41950 mdB)
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_SETTX1ATTEN_INV_PARM tx1Attenuation_mdB parameter is out of range (0 - 41950)
 * \retval MYKONOS_ERR_SETTX1ATTEN_INV_STEPSIZE_PARM device->tx->txAttenStepSize is not a valid enum value
 */
mykonosErr_t MYKONOS_setTx1Attenuation(mykonosDevice_t *device, uint16_t tx1Attenuation_mdB)
{
    uint16_t data = 0;
    uint16_t attenStepSizeDiv = 50;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_setTx1Attenuation()\n");
#endif

    /* check input parameter is in valid range */
    if (tx1Attenuation_mdB > 41950)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SETTX1ATTEN_INV_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_SETTX1ATTEN_INV_PARM));
        return MYKONOS_ERR_SETTX1ATTEN_INV_PARM;
    }

    switch (device->tx->txAttenStepSize)
    {
        case TXATTEN_0P05_DB:
            attenStepSizeDiv = 50;
            break;
        case TXATTEN_0P1_DB:
            attenStepSizeDiv = 100;
            break;
        case TXATTEN_0P2_DB:
            attenStepSizeDiv = 200;
            break;
        case TXATTEN_0P4_DB:
            attenStepSizeDiv = 400;
            break;
        default:
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SETTX1ATTEN_INV_STEPSIZE_PARM,
                    getMykonosErrorMessage(MYKONOS_ERR_SETTX1ATTEN_INV_STEPSIZE_PARM));
            return MYKONOS_ERR_SETTX1ATTEN_INV_STEPSIZE_PARM;

    }

    data = (tx1Attenuation_mdB / attenStepSizeDiv);

    /* Write MSB bits followed by LSB bits, TxAtten updates when [7:0] written */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_TX1_ATTENUATION_1, ((data >> 8) & 0x03));
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_TX1_ATTENUATION_0, (data & 0xFF));

    return MYKONOS_ERR_OK;
}

/**
 * \brief Sets the Tx2 RF output Attenuation (Step size is 0.05dB.)
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 * - device->spiSettings->chipSelectIndex
 * - device->tx->txAttenStepSize
 *
 * \param device Pointer to the Mykonos device's data structure
 * \param tx2Attenuation_mdB The desired TxAttenuation in milli-dB
 *                           (Range: 0 to 41950 mdB)
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_SETTX2ATTEN_INV_PARM tx2Attenuation_mdB parameter is out of range (0 - 41950)
 * \retval MYKONOS_ERR_SETTX2ATTEN_INV_STEPSIZE_PARM device->tx->txAttenStepSize is not a valid enum value
 */
mykonosErr_t MYKONOS_setTx2Attenuation(mykonosDevice_t *device, uint16_t tx2Attenuation_mdB)
{
    uint16_t attenStepSizeDiv = 50;
    uint16_t data = 0;
    //const uint8_t SPI_TXATTEN_MODE = 0x01;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_setTx2Attenuation()\n");
#endif

    /* check input parameter is in valid range */
    if (tx2Attenuation_mdB > 41950)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SETTX2ATTEN_INV_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_SETTX2ATTEN_INV_PARM));
        return MYKONOS_ERR_SETTX2ATTEN_INV_PARM;
    }

    switch (device->tx->txAttenStepSize)
    {
        case TXATTEN_0P05_DB:
            attenStepSizeDiv = 50;
            break;
        case TXATTEN_0P1_DB:
            attenStepSizeDiv = 100;
            break;
        case TXATTEN_0P2_DB:
            attenStepSizeDiv = 200;
            break;
        case TXATTEN_0P4_DB:
            attenStepSizeDiv = 400;
            break;
        default:
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SETTX2ATTEN_INV_STEPSIZE_PARM,
                    getMykonosErrorMessage(MYKONOS_ERR_SETTX2ATTEN_INV_STEPSIZE_PARM));
            return MYKONOS_ERR_SETTX2ATTEN_INV_STEPSIZE_PARM;
    }

    data = (tx2Attenuation_mdB / attenStepSizeDiv);

    /* Write MSB bits followed by LSB bits, TxAtten updates when [7:0] written */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_TX2_ATTENUATION_1, ((data >> 8) & 0x03));
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_TX2_ATTENUATION_0, (data & 0xFF));

    return MYKONOS_ERR_OK;
}

/**
 * \brief Reads back the Tx1 RF output Attenuation
 *
 *  This function reads back the TxAttenuation setting currently applied to the transmit chain.
 *  The function will work with SPI mode or pin controlled TxAtten mode using the increment/decrement GPIO
 *  pins.  For the readback value to be valid the Tx data path must be powered up.  If the Tx data path
 *  is powered down or radioOff state, the last TxAtten setting while the Tx was powered up will be
 *  read back.
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 * - device->spiSettings->chipSelectIndex
 *
 * \param device Pointer to the Mykonos device's data structure
 * \param tx1Attenuation_mdB The readback value of the Tx1 Attenuation in milli-dB (Range: 0 to 41950 mdB)
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_GETTX1ATTEN_NULL_PARM tx1Attenuation_mdB return parameter has NULL pointer
 */
mykonosErr_t MYKONOS_getTx1Attenuation(mykonosDevice_t *device, uint16_t *tx1Attenuation_mdB)
{
    uint8_t txAttenLsb = 0;
    uint8_t txAttenMsb = 0;
    uint16_t attenStepSizeDiv = 50;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_getTx1Attenuation()\n");
#endif

    /* check return parameter pointer is not NULL pointer */
    if (tx1Attenuation_mdB == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETTX1ATTEN_NULL_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_GETTX1ATTEN_NULL_PARM));
        return MYKONOS_ERR_GETTX1ATTEN_NULL_PARM;
    }

    /* Write TxAtten read back reg to update readback values, then read Tx1Atten */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_TX1_ATTENUATION_1_READBACK, 0x00);
    CMB_SPIReadField(device->spiSettings, MYKONOS_ADDR_TX1_ATTENUATION_0_READBACK, &txAttenLsb, 0xFF, 0);
    CMB_SPIReadField(device->spiSettings, MYKONOS_ADDR_TX1_ATTENUATION_1_READBACK, &txAttenMsb, 0x03, 0);

    /* Readback word always reads back with 0.05dB resolution */
    *tx1Attenuation_mdB = (((uint16_t)(txAttenLsb) | ((uint16_t)(txAttenMsb) << 8)) * attenStepSizeDiv);

    return MYKONOS_ERR_OK;
}

/**
 * \brief Reads back the Tx2 RF output Attenuation
 *
 *  This function reads back the TxAttenuation setting currently applied to the transmit chain.
 *  The function will work with SPI mode or pin controlled TxAtten mode using the increment/decrement GPIO
 *  pins.  For the readback value to be valid the Tx data path must be powered up.  If the Tx data path
 *  is powered down or radioOff state, the last TxAtten setting while the Tx was powered up will be
 *  read back.
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 * - device->spiSettings->chipSelectIndex
 *
 * \param device Pointer to the Mykonos device's data structure
 * \param tx2Attenuation_mdB The readback value of the Tx2 Attenuation in milli-dB (Range: 0 to 41950 mdB)
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_GETTX2ATTEN_NULL_PARM tx2Attenuation_mdB return parameter pointer is NULL
 */
mykonosErr_t MYKONOS_getTx2Attenuation(mykonosDevice_t *device, uint16_t *tx2Attenuation_mdB)
{
    uint8_t txAttenLsb = 0;
    uint8_t txAttenMsb = 0;
    uint16_t attenStepSizeDiv = 50;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_getTx2Attenuation()\n");
#endif

    /* check return parameter pointer is not NULL pointer */
    if (tx2Attenuation_mdB == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETTX2ATTEN_NULL_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_GETTX2ATTEN_NULL_PARM));
        return MYKONOS_ERR_GETTX2ATTEN_NULL_PARM;
    }

    /* Write TxAtten read back reg to update readback values, then read Tx1Atten */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_TX2_ATTENUATION_1_READBACK, 0x00);
    CMB_SPIReadField(device->spiSettings, MYKONOS_ADDR_TX2_ATTENUATION_0_READBACK, &txAttenLsb, 0xFF, 0);
    CMB_SPIReadField(device->spiSettings, MYKONOS_ADDR_TX2_ATTENUATION_1_READBACK, &txAttenMsb, 0x03, 0);

    /* Readback word always reads back with 0.05dB resolution */
    *tx2Attenuation_mdB = (((uint16_t)(txAttenLsb) | ((uint16_t)(txAttenMsb) << 8)) * attenStepSizeDiv);

    return MYKONOS_ERR_OK;
}

/**
 * \brief Checks the Tx Filter over-range bit assignments for digital clipping in the Tx data path
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 *
 * txFilterStatus bit-field assignments are:
 * txFilterStatus[0] = TFIR Ch1 Overflow
 * txFilterStatus[1] = HB1 Ch1 Overflow
 * txFilterStatus[2] = HB2 Ch1 Overflow
 * txFilterStatus[3] = QEC Ch1 Overflow
 * txFilterStatus[4] = TFIR Ch2 Overflow
 * txFilterStatus[5] = HB1 Ch2 Overflow
 * txFilterStatus[6] = HB2 Ch2 Overflow
 * txFilterStatus[7] = QEC Ch2 Overflow
 *
 * \param device Pointer to the Mykonos device's data structure
 * \param txFilterStatus is an 8-bit Tx filter over-range status bit-field
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_GET_TXFILTEROVRG_NULL_PARM Function txFilterStatus parameter pointer is NULL
 */
mykonosErr_t MYKONOS_getTxFilterOverRangeStatus(mykonosDevice_t *device, uint8_t *txFilterStatus)
{

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_getTxFilterOverRangeStatus()\n");
#endif

    if (txFilterStatus == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GET_TXFILTEROVRG_NULL_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_GET_TXFILTEROVRG_NULL_PARM));
        return MYKONOS_ERR_GET_TXFILTEROVRG_NULL_PARM;
    }

    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_TX_FILTER_OVERFLOW, txFilterStatus);
    return MYKONOS_ERR_OK;
}

/**
 * \brief Enables/Disables the Tx NCO test tone
 *
 *  This function enables/disables a digital numerically controlled oscillator
 *  in the Mykonos Digital to create a test CW tone on Tx1 and Tx2 RF outputs.
 *
 *  The TxAttenuation is forced in this function to max analog output power, but
 *  the digital attenuation is backed off 6dB to make sure the digital filter
 *  does not clip and cause spurs in the tx spectrum.
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 * - device->profilesValid
 * - device->tx->txProfile->iqRate_kHz
 *
 * \param device Pointer to the Mykonos device's data structure
 * \param enable 0 = Disable Tx NCO, 1 = Enable Tx NCO on both transmitters
 * \param tx1ToneFreq_kHz Signed frequency in kHz of the desired Tx1 tone
 * \param tx2ToneFreq_kHz Signed frequency in kHz of the desired Tx2 tone
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_ENTXNCO_TXPROFILE_INVALID Can not enable Tx NCO when Tx Profile is not enabled/valid
 * \retval MYKONOS_ERR_ENTXNCO_TX1_FREQ_INVALID Tx1 NCO Tone frequency out of range (-IQrate/2 to IQrate/2)
 * \retval MYKONOS_ERR_ENTXNCO_TX2_FREQ_INVALID Tx2 NCO Tone frequency out of range (-IQrate/2 to IQrate/2)
 */
mykonosErr_t MYKONOS_enableTxNco(mykonosDevice_t *device, uint8_t enable, int32_t tx1ToneFreq_kHz, int32_t tx2ToneFreq_kHz)
{
    int16_t tx1NcoTuneWord = 0;
    int16_t tx2NcoTuneWord = 0;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_enableTxNco()\n");
#endif

    if (enable > 0)
    {
        if ((device->profilesValid & TX_PROFILE_VALID) == 0)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_ENTXNCO_TXPROFILE_INVALID,
                    getMykonosErrorMessage(MYKONOS_ERR_ENTXNCO_TXPROFILE_INVALID));
            return MYKONOS_ERR_ENTXNCO_TXPROFILE_INVALID;
        }

        if ((tx1ToneFreq_kHz > ((int32_t)(device->tx->txProfile->iqRate_kHz) / 2)) || (tx1ToneFreq_kHz < ((int32_t)(device->tx->txProfile->iqRate_kHz) * -1 / 2)))
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_ENTXNCO_TX1_FREQ_INVALID,
                    getMykonosErrorMessage(MYKONOS_ERR_ENTXNCO_TX1_FREQ_INVALID));
            return MYKONOS_ERR_ENTXNCO_TX1_FREQ_INVALID;
        }

        if ((tx2ToneFreq_kHz > ((int32_t)(device->tx->txProfile->iqRate_kHz) / 2)) || (tx2ToneFreq_kHz < ((int32_t)(device->tx->txProfile->iqRate_kHz) * -1 / 2)))
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_ENTXNCO_TX2_FREQ_INVALID,
                    getMykonosErrorMessage(MYKONOS_ERR_ENTXNCO_TX2_FREQ_INVALID));
            return MYKONOS_ERR_ENTXNCO_TX2_FREQ_INVALID;
        }

        /* Force Tx output power to max analog output power, but 6dB digital */
        /* backoff to prevent the NCO from clipping the Tx PFIR filter */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_TX1_GAIN_0, 0x78);
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_TX1_GAIN_1, 0x00, 0x3F, 0);
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_TX1_GAIN_2, 0x0);

        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_TX2_GAIN_0, 0x78);
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_TX2_GAIN_1, 0x00, 0x3F, 0);
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_TX2_GAIN_2, 0x0);

        /* Enable manual Tx output power mode */
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_TX_TPC_CONFIG, 0x0A, 0x0F, 0);

        /* Set Tx NCO tuning words */
        tx1NcoTuneWord = (int16_t)(((int64_t)(tx1ToneFreq_kHz) << 16) / device->tx->txProfile->iqRate_kHz * -1);
        tx2NcoTuneWord = (int16_t)(((int64_t)(tx2ToneFreq_kHz) << 16) / device->tx->txProfile->iqRate_kHz * -1);

        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_TX_ABBF_FREQ_CAL_NCO_I_MSB, ((tx1NcoTuneWord >> 8) & 0xFF));
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_TX_ABBF_FREQ_CAL_NCO_I_LSB, (tx1NcoTuneWord & 0xFF));

        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_TX_ABBF_FREQ_CAL_NCO_Q_MSB, ((tx2NcoTuneWord >> 8) & 0xFF));
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_TX_ABBF_FREQ_CAL_NCO_Q_LSB, (tx2NcoTuneWord & 0xFF));

        /* Enable Tx NCO - set [7] = 1 */
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_DIGITAL_TEST_BYTE_0, 1, 0x80, 7);
    }
    else
    {
        /* Disable Tx NCO - set [7] = 0 */
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_DIGITAL_TEST_BYTE_0, 0, 0x80, 7);

        /* Enable normal Tx Atten table mode */
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_TX_TPC_CONFIG, 0x05, 0x0F, 0);
    }

    return MYKONOS_ERR_OK;
}

/**
 * \brief Writes Mykonos device registers with settings for the PA Protection block.
 *
 * This function writes register settings for the Mykonos PA Protection Block.
 * This function does not enable the PA Protection functionality.
 * Note that independent control of PA protection for both Tx channels is not possible.
 * The PA Protection block allows for error flags to go high if the accumulated TX power in the data path
 * exceeds a programmable threshold level based on samples taken in a programmable duration.
 *
 * \post After calling this function the user will need to call MYKONOS_enablePaProtection(...)
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 * - device->spiSettings->chipSelectIndex
 *
 * \param device Pointer to the Mykonos device's data structure
 * \param powerThreshold PA Protection Threshold sets the power level that, if detected in the TX data path, raises the PA error flags. The range is 0 to 4096.
 * To calculate the required setting for the power threshold:
 * powerThreshold = MAX_DAC_CODE * (10^((txPowerThresh_dBFS/10)))
 * For example: If the required dBFS for the threshold is -10 dBFS, then the powerThreshold passed should be 409.
 *
 * \param attenStepSize Attenuation Step Size sets the size of the attenuation step when Tx Atten Control is Enabled. The range is 0 to 128 with a resolution is 0.2dB/LSB.
 * \param avgDuration Averaging Duration for the TX data path power measurement. The range is 0 to 14 specified in number of cycles of the TX IQ Rate.
 * \param stickyFlagEnable "1" Enables the PA Error Flags to stay high after being triggered, even if the power decreases below threshold. "0" disables this functionality.
 * \param txAttenControlEnable "1" Enables autonomous attenuation changes in response to PA error flags. "0" disables this functionality.
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_SETUP_PA_PROT_INV_AVG_DURATION avgDuration parameter invalid (valid 0-15)
 * \retval MYKONOS_ERR_SETUP_PA_PROT_INV_ATTEN_STEP attenStepSize function parameter invalid (valid 0-127)
 */
mykonosErr_t MYKONOS_setupPaProtection(mykonosDevice_t *device, uint16_t powerThreshold, uint8_t attenStepSize, uint8_t avgDuration, uint8_t stickyFlagEnable, uint8_t txAttenControlEnable)
{
    uint8_t paProtectConfig = 0;
    uint8_t paProtectAttenCntl = 0;

    const uint8_t PROTECT_AVG_DUR_MASK = 0x1E;
    const uint8_t STICKY_FLAG_EN_MASK = 0x40;
    const uint8_t ATT_STEP_MASK = 0xFE;
    const uint8_t PROTECT_ATT_EN_MASK = 0x01;
    const uint8_t AVG_DUR_MASK = 0x0F;
    const uint8_t ATT_STEP_SIZE = 0x7F;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_setupPaProtection()\n");
#endif

    /* Overwrite PA Protection Config register data with averaging duration and sticky flag. Also performing range check on stickyFlagEnable and avgDuration */
    if (avgDuration > AVG_DUR_MASK)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SETUP_PA_PROT_INV_AVG_DURATION,
                getMykonosErrorMessage(MYKONOS_ERR_SETUP_PA_PROT_INV_AVG_DURATION));
        return MYKONOS_ERR_SETUP_PA_PROT_INV_AVG_DURATION;
    }

    if (attenStepSize > ATT_STEP_SIZE)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SETUP_PA_PROT_INV_ATTEN_STEP,
                getMykonosErrorMessage(MYKONOS_ERR_SETUP_PA_PROT_INV_ATTEN_STEP));
        return MYKONOS_ERR_SETUP_PA_PROT_INV_ATTEN_STEP;
    }

    paProtectConfig = (PROTECT_AVG_DUR_MASK & (avgDuration << 1));

    if (stickyFlagEnable > 0)
    {
        paProtectConfig |= (STICKY_FLAG_EN_MASK);
    }

    paProtectAttenCntl = ATT_STEP_MASK & (attenStepSize << 1);

    if (txAttenControlEnable > 0)
    {
        paProtectAttenCntl |= (PROTECT_ATT_EN_MASK);
    }

    /* Clear and write all PA Protection setup registers */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_PA_PROTECTION_ATTEN_CONTROL, 0x00);
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_PA_PROTECTION_CONFIGURATION, 0x00);
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_PA_PROTECTION_THRESHOLD_LSB, 0x00);
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_PA_PROTECTION_THRESHOLD_MSB, 0x00);

    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_PA_PROTECTION_ATTEN_CONTROL, paProtectAttenCntl);
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_PA_PROTECTION_CONFIGURATION, paProtectConfig);
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_PA_PROTECTION_THRESHOLD_LSB, (powerThreshold & 0xFF));
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_PA_PROTECTION_THRESHOLD_MSB, ((powerThreshold >> 8) & 0x0F));

    return MYKONOS_ERR_OK;
}

/**
 * \brief Enables the Mykonos PA Protection block according to the parameters passed in MYKONOS_setupPaProtection(...).
 *
 * This function enables the PA Protection block according to the parameters passed in MYKONOS_setupPaProtection(...)
 * The paProtectEnable signal enables the PA Protection block, allowing usage of the PA protection functions.
 *
 * \pre Before calling this function the user needs to setup the PA protection by calling MYKONOS_setupPaProtection(...)
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 * - device->spiSettings->chipSelectIndex
 *
 * \param device Pointer to the Mykonos device's data structure
 * \param paProtectEnable "1" Enables the PA Protection block. "0" Disables the PA Protection block
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 */
mykonosErr_t MYKONOS_enablePaProtection(mykonosDevice_t *device, uint8_t paProtectEnable)
{
    uint8_t paProtectEnableReg = 0;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_enablePaProtection()\n");
#endif

    paProtectEnableReg = (paProtectEnable > 0) ? 1 : 0;
    CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_PA_PROTECTION_CONFIGURATION, paProtectEnableReg, 0x01, 0);

    return MYKONOS_ERR_OK;
}

/**
 * \brief Obtains an estimate of a TX channel's accumulated power over the sample duration provided in MYKONOS_setupPaProtection(...)
 *
 * This function uses the 'avgDuration' parameter provided in MYKONOS_setupPaProtection to set the number of samples to accumulate
 * to obtain an estimate for a TX channel specified by the 'channel' parameter.
 * A 12-bit field estimating the channel power is returned in the '*channelPower' pointer.
 * To obtain the dBFS value of the reading:
 * dBFS value = 10*log10(channelPower/MAX_DAC_CODE)
 * where MAX_DAC_CODE = 2^12 =  4096
 * For example: If channelPower is reading 409 then the channel power in dBFS is -10dBFS.
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 * - device->spiSettings->chipSelectIndex
 *
 * \param device Point to the Mykonos device's data structure
 * \param channel Select the channel of interest. Only use TX1 (1) or TX2 (2) of mykonosTxChannels_t
 * \param *channelPower A pointer that stores the selected channels power. Read back is provided as a 12 bit value.
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_GET_DAC_PWR_INV_POINTER Function channelPower parameter is a NULL pointer
 * \retval MYKONOS_ERR_SETUP_PA_PROT_INV_TX_CHANNEL Invalid Tx Channel passed in function channel parameter (TX1 or TX2)
 */
mykonosErr_t MYKONOS_getDacPower(mykonosDevice_t *device, mykonosTxChannels_t channel, uint16_t *channelPower)
{
    uint8_t channelPowerLsb = 0;
    uint8_t channelPowerMsb = 0;

    const uint8_t CHAN_POWER_MSB_MASK = 0xF;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_getDacPower()\n");
#endif

    if (channelPower == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GET_DAC_PWR_INV_POINTER,
                getMykonosErrorMessage(MYKONOS_ERR_GET_DAC_PWR_INV_POINTER));
        return MYKONOS_ERR_GET_DAC_PWR_INV_POINTER;
    }

    /* Set bit D5 to read the appropriate channel power */
    switch (channel)
    {
        case TX1:
            CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_PA_PROTECTION_CONFIGURATION, 0, 0x20, 5);
            break;
        case TX2:
            CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_PA_PROTECTION_CONFIGURATION, 1, 0x20, 5);
            break;
        default:
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SETUP_PA_PROT_INV_TX_CHANNEL,
                    getMykonosErrorMessage(MYKONOS_ERR_SETUP_PA_PROT_INV_TX_CHANNEL));
            return MYKONOS_ERR_SETUP_PA_PROT_INV_TX_CHANNEL;
    }

    /* SPI Write to register PA Protection power readback registers (write strobe) */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_PA_PROTECTION_POWER_READBACK_LSB, 0x00);

    /* SPI Read of the PA Protection power readback registers */
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_PA_PROTECTION_POWER_READBACK_LSB, &channelPowerLsb);
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_PA_PROTECTION_POWER_READBACK_MSB, &channelPowerMsb);

    *channelPower = (channelPowerLsb | (((uint16_t)(channelPowerMsb & CHAN_POWER_MSB_MASK)) << 8));

    return MYKONOS_ERR_OK;
}

/**
 * \brief Returns PA Protection Error Flag Status
 *
 * This function provides a readback of the PA protection Error Flag Status through the '*errorFlagStatus' pointer
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 * - device->spiSettings->chipSelectIndex
 *
 * \param device Point to the Mykonos device's data structure
 * \param errorFlagStatus Pointer to store the error flag status.
 * errorFlagStatus  |  error
 * -----------------|------------
 *         0        |   indicates no PA Error Flags are high
 *         1        |   indicates TX1 Error Flag
 *         2        |   indicates TX2 Error Flag
 *         3        |   indicates TX1 and TX2 Error Flags are high
 *
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_GET_PA_FLAG_STATUS_INV_POINTER Function errorFlagStatus parameter has NULL pointer
 */
mykonosErr_t MYKONOS_getPaProtectErrorFlagStatus(mykonosDevice_t *device, uint8_t *errorFlagStatus)
{

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_getPaProtectErrorFlagStatus()\n");
#endif

    if (errorFlagStatus == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GET_PA_FLAG_STATUS_INV_POINTER,
                getMykonosErrorMessage(MYKONOS_ERR_GET_PA_FLAG_STATUS_INV_POINTER));
        return MYKONOS_ERR_GET_PA_FLAG_STATUS_INV_POINTER;
    }

    /* SPI Write to register PA Protection power readback registers (write strobe) */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_PA_PROTECTION_POWER_READBACK_LSB, 0x00);

    /* SPI Read of the PA Protection power readback registers */
    CMB_SPIReadField(device->spiSettings, MYKONOS_ADDR_PA_PROTECTION_POWER_READBACK_MSB, errorFlagStatus, 0x60, 5);

    return MYKONOS_ERR_OK;
}

/**
 * \brief Manually clears the PA Protection Error Flags.
 *
 * This function manually clears the PA Error Flags. The user must setup the PA Protection block to enable
 * sticky error flags. Sticky error flags require the user to clear the bit manually even if the accumulated power is
 * below the power threshold for the PA protection block.
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 * - device->spiSettings->chipSelectIndex
 *
 * \param device Pointer to the Mykonos device's data structure
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 */
mykonosErr_t MYKONOS_clearPaErrorFlag(mykonosDevice_t *device)
{
    uint8_t paProtectConfig;

    const uint8_t PA_ERROR_CLEAR_MASK = 0x80;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_clearPaErrorFlag()\n");
#endif

    /* SPI Read of the PA Protection power readback registers */
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_PA_PROTECTION_CONFIGURATION, &paProtectConfig);

    /* SPI Write to self clear the PA Error Flags */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_PA_PROTECTION_CONFIGURATION, (paProtectConfig | PA_ERROR_CLEAR_MASK));

    return MYKONOS_ERR_OK;
}

/**
 * \brief Helper function for return of character string based on 32-bit mykonosErr_t enum value
 *
 * To save codespace, these error strings are ifdef'd out unless the user
 * adds a define MYKONOS_VERBOSE to their workspace.  This function can be
 * useful for debug.  Each function also returns unique error codes to 
 * make it easier to determine where the code broke.
 *
 * \param errorCode is enumerated error code value
 *
 * \return Returns character string based on enumerated value
 */
const char* getMykonosErrorMessage(mykonosErr_t errorCode)
{
#ifndef MYKONOS_VERBOSE
    return "";

#else

    switch (errorCode)
    {
        case MYKONOS_ERR_OK:
            return "";
        case MYKONOS_ERR_INV_PARM:
            return "Mykonos: Invalid parameter\n";
        case MYKONOS_ERR_FAILED:
            return "Mykonos: General Failure\n";
        case MYKONOS_ERR_WAITFOREVENT_INV_PARM:
            return "waitForEvent had invalid parameter.\n";
        case MYKONOS_ERR_WAITFOREVENT_TIMEDOUT:
            return "waitForEvent timed out.\n";
        case MYKONOS_ERR_SETENSM_INVALID_NEWSTATE_WAIT:
            return "Requested new ENSM state is invalid from the WAIT state.\n";
        case MYKONOS_ERR_SETENSM_INVALID_NEWSTATE_ALERT:
            return "Requested new ENSM state is invalid from the ALERT state.\n";
        case MYKONOS_ERR_SETENSM_INVALID_NEWSTATE_TXRX:
            return "Requested new ENSM state is invalid from the TXRX state.\n";
        case MYKONOS_ERR_SETENSM_INVALIDSTATE:
            return "Current ENSM state is an invalid state or calibration state.\n";
        case MYKONOS_ERR_PU_RXPATH_INV_PARAM:
            return "Invalid Rx channel was requested to be powered up.\n";
        case MYKONOS_ERR_PU_TXPATH_INV_PARAM:
            return "Invalid Tx channel was requested to be powered up.\n";
        case MYKONOS_ERR_PU_OBSRXPATH_INV_PARAM:
            return "Invalid ObsRx channel was requested to be powered up.\n";
        case MYKONOS_ERR_SETDEFOBSRXPATH_NULL_DEF_OBSRX_STRUCT:
            return "Invalid default ObsRx channel was requested to be powered up.\n";
        case MYKONOS_ERR_INIT_INV_ORXCHAN:
            return "Invalid ObsRx channel requested during initialize().\n";
        case MYKONOS_ERR_INIT_INV_RXSYNCB_ORXSYNCB_MODE:
            return "Invalid combination for rxsyncb and orxsyncb, if shared syncb they should have the CMOS/LVDS mode\n";
        case MYKONOS_ERR_INIT_INV_TXFIR_INTERPOLATION:
            return "Invalid TxFIR interpolation value.(Valid: 1,2,4)\n";
        case MYKONOS_ERR_INIT_INV_TXHB2_INTERPOLATION:
            return "Invalid TXHB2 interpolation value.(Valid: 1 or 2)\n";
        case MYKONOS_ERR_INIT_INV_TXHB1_INTERPOLATION:
            return "Invalid TXHB1 interpolation value.(Valid: 1 or 2)\n";
        case MYKONOS_ERR_INIT_INV_RXFIR_DECIMATION:
            return "Invalid RxFIR decimation value.(Valid: 1,2,4)\n";
        case MYKONOS_ERR_INIT_INV_RXDEC5_DECIMATION:
            return "Invalid Rx DEC5 decimation value.(Valid: 4 or 5)\n";
        case MYKONOS_ERR_INIT_INV_RXHB1_DECIMATION:
            return "Invalid RxHB1 decimation value.(Valid: 1 or 2)\n";
        case MYKONOS_ERR_INIT_INV_SNIFFER_RHB1:
            return "Invalid Sniffer HB1 decimation value (Valid 1 or 2)\n";
        case MYKONOS_ERR_INIT_INV_SNIFFER_RFIR_DEC:
            return "Invalid Sniffer RFIR decimation value (Valid 1,2,4)\n";
        case MYKONOS_ERR_INIT_INV_ORX_RHB1:
            return "Invalid ORx HB1 decimation value (Valid 1 or 2)\n";
        case MYKONOS_ERR_INIT_INV_ORX_RFIR_DEC:
            return "Invalid ORx RFIR decimation value. (Valid 1,2,4)\n";
        case MYKONOS_ERR_INIT_INV_ADCDIV:
            return "Invalid Rx ADC divider. (Valid 1 or 2)\n";
        case MYKONOS_ERR_INIT_INV_DACDIV:
            return "Invalid Tx DAC divider. Use enum DACDIV_2, DACDIV_2p5 or DACDIV_4.\n";
        case MYKONOS_ERR_INIT_INV_OBSRX_ADCDIV:
            return "Invalid ObsRx ADC div. (Valid 1 or 2)\n";
        case MYKONOS_ERR_CLKPLL_INV_HSDIV:
            return "Invalid CLKPLL HSDIV. (Valid 4 or 5)\n";
        case MYKONOS_ERR_CLKPLL_INV_VCODIV:
            return "Invalid CLKPLL VCODIV. Use enum VCODIV_1, VCODIV_1p5, VCODIV_2, VCODIV_3\n";
        case MYKONOS_ERR_CLKPLL_INV_RXTXPROFILES:
            return "No Rx or Tx profile is specified.\n";
        case MYKONOS_ERR_SETCLKPLL_INV_VCOINDEX:
            return "CLKPLL VCO frequency out of range.\n";
        case MYKONOS_ERR_SETCLKPLL_INV_FRACWORD:
            return "CLKPLL fractional word is non zero.\n";
        case MYKONOS_ERR_SETRFPLL_INV_PLLNAME:
            return "Invalid pllName requested in setRfPllFreuquency()\n";
        case MYKONOS_ERR_SETRFPLL_INV_LO_PARM:
            return "RF PLL frequency out of range\n";
        case MYKONOS_ERR_GETRFPLL_INV_PLLNAME:
            return "Invalid pllName requested in getRfPllFrequency()\n";
        case MYKONOS_ERR_GETRFPLL_ARMERROR:
            return "ARM Command Error in MYKONOS_getRfPllFrequency()\n";
        case MYKONOS_ERR_GETRFPLL_NULLPARAM:
            return "NULL pointer in function parameter for MYKONOS_setRfPllFrequency()\n";
        case MYKONOS_ERR_INV_SCALEDDEVCLK_PARAM:
            return "Could not scale device clock into range 40MHz to 80Mhz.\n";
        case MYKONOS_ERR_SETRFPLL_INV_VCOINDEX:
            return "RFPLL VCO frequency out of range.\n";
        case MYKONOS_ERR_SETRFPLL_INV_REFCLK:
            return "Unsupported PLL reference clock or refclk out of range.\n";
        case MYKONOS_ERR_SETORXGAIN_INV_CHANNEL:
            return "Invalid ObsRx channel in setObsRxManualGain().\n";
        case MYKONOS_ERR_SETORXGAIN_INV_ORX1GAIN:
            return "Invalid gain requested in setObsRxManualGain() for ORX1\n";
        case MYKONOS_ERR_SETORXGAIN_INV_ORX2GAIN:
            return "Invalid gain requested in setObsRxManualGain() for ORX2\n";
        case MYKONOS_ERR_SETORXGAIN_INV_SNRXGAIN:
            return "Invalid gain requested in setObsRxManualGain() for Sniffer\n";
        case MYKONOS_ERR_GETORX1GAIN_INV_POINTER:
            return "Cannot return ORx1 gain to gain control data structure (invalid pointer).\n";
        case MYKONOS_ERR_GETORX2GAIN_INV_POINTER:
            return "Cannot return ORx2 gain to gain control data structure (invalid pointer).\n";
        case MYKONOS_ERR_GETSNIFFGAIN_INV_POINTER:
            return "Cannot return Sniffer gain to gain control data structure (invalid pointer).\n";
        case MYKONOS_ERR_GETOBSRXGAIN_CH_DISABLED:
            return "Cannot read ObsRx gain index. ObsRx Channel is disabled.\n";
        case MYKONOS_ERR_SETTX1ATTEN_INV_PARM:
            return "Tx1 attenuation is out of range (0 - 41950 mdB).\n";
        case MYKONOS_ERR_SETTX1ATTEN_INV_STEPSIZE_PARM:
            return "Invalid Tx1Atten stepsize. Use enum mykonosTxAttenStepSize_t\n";
        case MYKONOS_ERR_SETTX2ATTEN_INV_PARM:
            return "Tx2 attenuation is out of range (0 - 41950 mdB).\n";
        case MYKONOS_ERR_SETTX2ATTEN_INV_STEPSIZE_PARM:
            return "Invalid Tx2Atten stepsize. Use enum mykonosTxAttenStepSize_t\n";
        case MYKONOS_ERR_PROGRAMFIR_INV_NUMTAPS_PARM:
            return "Invalid number of FIR taps\n";
        case MYKONOS_ERR_READFIR_INV_NUMTAPS_PARM:
            return "Invalid number of FIR taps read for the selected Filter\n";
        case MYKONOS_ERR_PROGRAMFIR_INV_FIRNAME_PARM:
            return "Invalid FIR filter name requested. use enum mykonosfirName_t\n";
        case MYKONOS_ERR_RXFIR_INV_GAIN_PARM:
            return "Rx FIR filter has invalid gain setting\n";
        case MYKONOS_ERR_OBSRXFIR_INV_GAIN_PARM:
            return "ObsRx FIR filter A (ORx) has invalid gain setting\n";
        case MYKONOS_ERR_SRXFIR_INV_GAIN_PARM:
            return "ObsRx FIR filter B (Sniffer) has invalid gain setting\n";
        case MYKONOS_ERR_TXFIR_INV_GAIN_PARM:
            return "Tx FIR filter has invalid gain setting\n";
        case MYKONOS_ERR_READFIR_NULL_PARM:
            return "MYKONOS_readFir() has a null *firFilter parameter\n";
        case MYKONOS_ERR_READFIR_COEFS_NULL:
            return "MYKONOS_readFir() has a null coef array in *firFilter structure\n";
        case MYKONOS_ERR_READFIR_INV_FIRNAME_PARM:
            return "Invalid FIR filter name requested. use enum mykonosfirName_t\n";
        case MYKONOS_ERR_SETRX1GAIN_INV_GAIN_PARM:
            return "Rx1 manual gain index out of range of gain table\n";
        case MYKONOS_ERR_SETRX2GAIN_INV_GAIN_PARM:
            return "Rx2 manual gain index out of range of gain table\n";
        case MYKONOS_ERR_INITSER_INV_VCODIV_PARM:
            return "Found invalid VCO divider value during setupSerializers()\n";
        case MYKONOS_ERR_INITDES_INV_VCODIV_PARM:
            return "Found invalid VCO divider value during setupDeserializers()\n";
        case MYKONOS_ERR_SER_INV_M_PARM:
            return "Invalid JESD Framer M parameter during setupSerializers()\n";
        case MYKONOS_ERR_SER_INV_L_PARM:
            return "Invalid JESD Framer L parameter during setupSerializers()\n";
        case MYKONOS_ERR_SER_INV_HSCLK_PARM:
            return "Invalid HSCLK frequency in setupSerializers()\n";
        case MYKONOS_ERR_SER_INV_LANERATE_PARM:
            return "Invalid Lanerate frequency in setupSerializer()\n";
        case MYKONOS_ERR_SER_INV_LANEEN_PARM:
            return "Invalid number of JESD204 lanes enabled.\n";
        case MYKONOS_ERR_SER_INV_AMP_PARM:
            return "Invalid JESD204 serializer amplitude\n";
        case MYKONOS_ERR_SER_INV_PREEMP_PARM:
            return "Invalid JESD204 serializer preemphasis setting\n";
        case MYKONOS_ERR_SER_INV_LANEPN_PARM:
            return "Invald JESD204 serializer PN invert setting\n";
        case MYKONOS_ERR_SER_LANE_CONFLICT_PARM:
            return "Rx framer and ObsRx framer are attempting to use the same physical lanes\n";
        case MYKONOS_ERR_SER_INV_TXSER_DIV_PARM:
            return "Invalid serializer lane clock divider\n";
        case MYKONOS_ERR_SER_LANE_RATE_CONFLICT_PARM:
            return "Rx lane and ObsRx lane rate must match\n";
        case MYKONOS_ERR_SER_INV_REAL_IF_DATA_PARM:
            return "Framer M can only =1 if Real IF data option is enabled\n";
        case MYKONOS_ERR_HS_AND_LANE_RATE_NOT_INTEGER_MULT:
            return "Serializer HSCLK and lane clock are not integer multiples\n";
        case MYKONOS_ERR_DES_HS_AND_LANE_RATE_NOT_INTEGER_MULT:
            return "Deserializer HSCLK and lane clock are not integer multiples\n";
        case MYKONOS_ERR_DESER_INV_M_PARM:
            return "Invalid deserializer M value. (Valid 2 or 4)\n";
        case MYKONOS_ERR_DESER_INV_L_PARM:
            return "Invalid deserializer L value. (Valid 1,2,4)\n";
        case MYKONOS_ERR_DESER_INV_HSCLK_PARM:
            return "Invalid HSCLK in setupDeserializer()\n";
        case MYKONOS_ERR_DESER_INV_LANERATE_PARM:
            return "Invalid deserializer lane rate\n";
        case MYKONOS_ERR_DESER_INV_LANEEN_PARM:
            return "Invalid number of deserializer lanes enabled\n";
        case MYKONOS_ERR_DESER_INV_EQ_PARM:
            return "Invalid deserializer EQ setting";
        case MYKONOS_ERR_DESER_INV_LANEPN_PARM:
            return "Invalid deserializer invert Lane PN setting\n";
        case MYKONOS_ERR_FRAMER_INV_M_PARM:
            return "Invalid Rx framer M setting\n";
        case MYKONOS_ERR_FRAMER_INV_BANKID_PARM:
            return "Invalid Rx framer Bank ID\n";
        case MYKONOS_ERR_FRAMER_INV_LANEID_PARM:
            return "Invalid Rx framer Lane ID\n";
        case MYKONOS_ERR_FRAMER_INV_K_OFFSET_PARAM:
            return "Invalid Rx framer LMFC offset\n";
        case MYKONOS_ERR_FRAMER_INV_REAL_IF_DATA_PARM:
            return "Invalid Rx framer Real IF setting\n";
        case MYKONOS_ERR_OBSRX_FRAMER_INV_M_PARM:
            return "Invalid ObsRx Framer M value\n";
        case MYKONOS_ERR_OBSRX_FRAMER_INV_BANKID_PARM:
            return "Invalid ObsRx Framer Bank ID\n";
        case MYKONOS_ERR_OBSRX_FRAMER_INV_LANEID_PARM:
            return "Invalid ObsRx framer Lane ID\n";
        case MYKONOS_ERR_OBSRX_FRAMER_INV_K_OFFSET_PARAM:
            return "Invalid ObsRx framer LMFC offset\n";
        case MYKONOS_ERR_OBSRX_FRAMER_INV_REAL_IF_DATA_PARM:
            return "Invalid ObsRx framer Real IF setting, M must =1\n";
        case MYKONOS_ERR_DEFRAMER_INV_M_PARM:
            return "Invalid Deframer M setting\n";
        case MYKONOS_ERR_DEFRAMER_INV_BANKID_PARM:
            return "Invalid Deframer Bank ID\n";
        case MYKONOS_ERR_ERR_DEFRAMER_INV_LANEID_PARM:
            return "Invalid Deframer Lane ID\n";
        case MYKONOS_ERR_DEFRAMER_INV_K_OFFSET_PARAM:
            return "Invalid Deframer LMFC offset";
        case MYKONOS_ERR_DEFRAMER_INV_K_PARAM:
            return "Invalid Deframer K setting\n";
        case MYKONOS_ERR_DEFRAMER_INV_FK_PARAM:
            return "Invalid Deframer F*K value\n";
        case MYKONOS_ERR_RX_FRAMER_INV_PRBS_POLYORDER_PARAM:
            return "Invalid polyOrder parameter in enableRxFramerPrbs()\n";
        case MYKONOS_ERR_OBSRX_FRAMER_INV_PRBS_POLYORDER_PARAM:
            return "Invalid polyOrder parameter in enableObsRxFramerPrbs()\n";
        case MYKONOS_ERR_DEFRAMER_INV_PRBS_ENABLE_PARAM:
            return "Invalid enable parameter value in enableDeframerPrbsChecker()\n";
        case MYKONOS_ERR_DEFRAMER_INV_PRBS_POLYORDER_PARAM:
            return "Invalid polyOrder parameter in enableDeframerPrbsChecker()\n";
        case MYKONOS_ERR_DEFRAMER_INV_PRBS_CNTR_SEL_PARAM:
            return "Invalid lane counter select in readDeframerPrbsCounters()\n";
        case MYKONOS_ERR_INITARM_INV_DATARATE_PARM:
            return 0;
        case MYKONOS_ERR_INITARM_INV_VCODIV:
            return "Invalid ARM VCO divider\n";
        case MYKONOS_ERR_INITARM_INV_REGCLK:
            return "Invalid ARM SPI register clock rate\n";
        case MYKONOS_ERR_INITARM_INV_ARMCLK_PARAM:
            return "Invalid ARM clock rate\n";
        case MYKONOS_ERR_LOADHEX_INV_CHARCOUNT:
            return "LoadHex() char count = 0\n";
        case MYKONOS_ERR_LOADHEX_INVALID_FIRSTCHAR:
            return "LoadHex() First char is not :\n";
        case MYKONOS_ERR_LOADHEX_INVALID_CHKSUM:
            return "LoadHex() line checksum is invalid\n";
        case MYKONOS_ERR_LOADBIN_INVALID_BYTECOUNT:
            return 0;
        case MYKONOS_ERR_ARM_INVALID_BUILDCHKSUM:
            return "Verify ARM checksum failed\n";
        case MYKONOS_ERR_READARMMEM_INV_ADDR_PARM:
            return "ReadArmMem() was given an invalid memory address\n";
        case MYKONOS_ERR_WRITEARMMEM_INV_ADDR_PARM:
            return "WriteArmMem() was given an invalid memory address\n";
        case MYKONOS_ERR_ARMCMD_INV_OPCODE_PARM:
            return "Invalid ARM opcode given to sendArmCommand()\n";
        case MYKONOS_ERR_ARMCMD_INV_NUMBYTES_PARM:
            return "Invalid number of extended data in sendArmCommand()\n";
        case MYKONOS_ERR_ARMCMDSTATUS_INV_OPCODE_PARM:
            return "Invalid opcode given to waitArmCmdStatus()\n";
        case MYKONOS_ERR_CHECKDEVSTRUCT_NULLDEVPOINTER:
            return "Pointer to Mykonos device data structure is NULL\n";
        case MYKONOS_ERR_CHECKDEVSTRUCT_SPI:
            return "Invalid spiSettings pointer in device data structure\n";
        case MYKONOS_ERR_CHECKDEVSTRUCT_RX:
            return "Invalid device->rx pointer in device data structure\n";
        case MYKONOS_ERR_CHECKDEVSTRUCT_RXSUB:
            return "Invalid pointer within device->rx data structure\n";
        case MYKONOS_ERR_CHECKDEVSTRUCT_RXFIR:
            return "Invalid RXFIR pointer in device data structure\n";
        case MYKONOS_ERR_CHECKDEVSTRUCT_TX:
            return "Invalid device->tx pointer in device data structure\n";
        case MYKONOS_ERR_CHECKDEVSTRUCT_TXSUB:
            return "Invalid pointer within device->tx data structure\n";
        case MYKONOS_ERR_CHECKDEVSTRUCT_TXFIR:
            return "Invalid TXFIR pointer in device data structure\n";
        case MYKONOS_ERR_CHECKDEVSTRUCT_OBSRX:
            return "Invalid device->obsRx pointer in device data structure\n";
        case MYKONOS_ERR_CHECKDEVSTRUCT_OBSRXSUB:
            return "Invalid pointer within device->obsRx data structure\n";
        case MYKONOS_ERR_CHECKDEVSTRUCT_SNIFFERFIR:
            return "Invalid Sniffer FIR pointer in device data structure\n";
        case MYKONOS_ERR_CHECKDEVSTRUCT_ORXFIR:
            return "Invalid ORX FIR pointer in device data structure\n";
        case MYKONOS_ERR_CHECKDEVSTRUCT_ORXGAINCTRL:
            return "Invalid Orx gain control pointer in device data structure\n";
        case MYKONOS_ERR_CHECKDEVSTRUCT_SNIFFERGAINCTRL:
            return "Invalid Sniffer gain control pointer in device data structure\n";
        case MYKONOS_ERR_CHECKDEVSTRUCT_OBSRXFRAMER:
            return "Invalid obsRx framer pointer in device data structure\n";
        case MYKONOS_ERR_INITSER_INV_PROFILE:
            return "Invalid RX profile within device data structure detected in MYKONOS_setupSerializers()\n";
        case MYKONOS_ERR_INITDES_INV_TXPROFILE:
            return "Invalid TX profile within device data structure detected in MYKONOS_setupDeserializers()\n";
        case MYKONOS_ERR_JESD204B_ILAS_MISMATCH:
            return "Mismatch detected in MYKONOS_jesd204bIlasCheck()\n";
        case MYKONOS_ERR_RXGAINTABLE_INV_CHANNEL:
            return "Invalid channel specified in MYKONOS_programRxGainTable()\n";
        case MYKONOS_ERR_RXGAINTABLE_INV_GAIN_INDEX_RANGE:
            return "Invalid numGainIndicesinTable greater than possible number of gain indices for channel. \n";
        case MYKONOS_ERR_WRITE_CFG_MEMORY_FAILED:
            return "Failed write to ARM memory in MYKONOS_writeArmProfile()\n";
        case MYKONOS_ERR_INV_RXFRAMER_PCLKDIV_PARM:
            return "Invalid PCLKDIV parameter detected in MYKONOS_setupSerializers()\n";
        case MYKONOS_ERR_RXFRAMER_INV_FK_PARAM:
            return "Invalid FK parameter detected in MYKONOS_setupJesd204bFramer()\n";
        case MYKONOS_ERR_OBSRXFRAMER_INV_FK_PARAM:
            return "Invalid FK paramter detected in MYKONOS_setupJesd204bObsRxFramer()\n";
        case MYKONOS_ERR_INV_OBSRXFRAMER_PCLKDIV_PARM:
            return "Invalid PCLKDIV paramter detected in MYKONOS_setupSerializers()\n";
        case MYKONOS_ERR_PU_OBSRXPATH_INV_LOSOURCE_PARAM:
            return "Invalid LO Source for OBSRX data path \n";
        case MYKONOS_ERR_ARM_RADIOON_FAILED:
            return "ARM command to move to radioOn state failed. \n";
        case MYKONOS_ERR_ARM_RADIOOFF_FAILED:
            return "ARM command to move to radioOff state failed. \n";
        case MYKONOS_ERR_INV_RX_GAIN_MODE_PARM:
            return "Invalid gain control mode detected in MYKONOS_setRxGainControlMode()\n";
        case MYKONOS_ERR_INV_ORX_GAIN_MODE_PARM:
            return "Invalid gain control mode detected in MYKONOS_setObsRxGainControlMode()\n";
        case MYKONOS_ERR_INV_AGC_RX_STRUCT_INIT:
            return "Invalid RX AGC structure detected at &device->rx->rxAgcCtrl in MYKONOS_setupRxAgc()\n";
        case MYKONOS_ERR_INV_AGC_RX_PEAK_WAIT_TIME_PARM:
            return "device->rx->rxAgcCtrl->agcPeakWaitTime out of range in MYKONOS_setupRxAgc()\n";
        case MYKONOS_ERR_INV_AGC_RX_GAIN_UPDATE_TIME_PARM:
            return "device->rx->rxAgcCtrl->agcGainUpdateTime_us out of range in MYKONOS_setupRxAgc()\n";
        case MYKONOS_ERR_INV_AGC_RX_APD_HIGH_THRESH_PARM:
            return "device->rx->rxAgcCtrl->apdHighThresh out of range in MYKONOS_setupRxAgc()\n";
        case MYKONOS_ERR_INV_AGC_RX_APD_LOW_THRESH_PARM:
            return "device->rx->rxAgcCtrl->apdLowThresh out of range in MYKONOS_setupRxAgc()\n";
        case MYKONOS_ERR_INV_AGC_RX_BLOCK_DET_DECAY_PARM:
            return "device->rx->rxAgcCtrl->apdDecay out of range in MYKONOS_setupRxAgc()\n";
        case MYKONOS_ERR_INV_AGC_RX_PEAK_STRUCT_INIT:
            return "Data structure for peakAgc not initialized when used in MYKONOS_setupRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_RX_PWR_STRUCT_INIT:
            return "Data structure for powerAgc not initialized when used in MYKONOS_setupRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_RX1_MAX_GAIN_INDEX:
            return "device->rx->rxAgcCtrl->agcRx1MaxGainIndex out of range in MYKONOS_setupRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_RX1_MIN_GAIN_INDEX:
            return "device->rx->rxAgcCtrl->agcRx1MinGainIndex out of range in MYKONOS_setupRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_RX2_MAX_GAIN_INDEX:
            return "device->rx->rxAgcCtrl->agcRx2MaxGainIndex out of range in MYKONOS_setupRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_RX2_MIN_GAIN_INDEX:
            return "device->rx->rxAgcCtrl->agcRx2MinGainIndex out of range in MYKONOS_setupRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_RX_SLOW_LOOP_SETTLING_DELAY:
            return "device->rx->rxAgcCtrl->agcSlowLoopSettlingDelay out of range in MYKONOS_setupRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_PMD_MEAS_DURATION:
            return "device->rx->rxAgcCtrl->powerAgc->pmdMeasDuration out of range in MYKONOS_setupRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_PMD_MEAS_CONFIG:
            return "device->rx->rxAgcCtrl->powerAgc->pmdMeasConfig out of range in MYKONOS_setupRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_RX_LOW_THS_PREV_GAIN_INC:
            return "device->rx->rxAgcCtrl->agcLowThsPreventGainIncrease out of range in MYKONOS_setupRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_RX_PEAK_THRESH_MODE:
            return "device->rx->rxAgcCtrl->agcPeakThresholdMode out of range in MYKONOS_setupRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_RX_RESET_ON_RX_ENABLE:
            return "device->rx->rxAgcCtrl->agcResetOnRxEnable out of range in MYKONOS_setupRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_RX_ENABLE_SYNC_PULSE_GAIN_COUNTER:
            return "device->rx->rxAgcCtrl->agcEnableSyncPulseForGainCounter out of range in MYKONOS_setupRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_RX_PMD_LOWER_HIGH_THRESH:
            return "device->rx->rxAgcCtrl->powerAgc->pmdLowerHighThresh out of range in MYKONOS_setupRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_RX_PMD_UPPER_LOW_THRESH:
            return "device->rx->rxAgcCtrl->powerAgc->pmdUpperLowThresh out of range in MYKONOS_setupRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_RX_PMD_LOWER_LOW_THRESH:
            return "device->rx->rxAgcCtrl->powerAgc->pmdLowerLowThresh out of range in MYKONOS_setupRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_RX_PMD_UPPER_HIGH_THRESH:
            return "device->rx->rxAgcCtrl->powerAgc->pmdUpperHighThresh out of range in MYKONOS_setupRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_RX_PMD_UPPER_HIGH_GAIN_STEP:
            return "device->rx->rxAgcCtrl->powerAgc->pmdUpperHighGainStepAttack out of range in MYKONOS_setupRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_RX_PMD_LOWER_LOW_GAIN_STEP:
            return "device->rx->rxAgcCtrl->powerAgc->pmdLowerLowGainStepRecovery out of range in MYKONOS_setupRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_RX_PMD_LOWER_HIGH_GAIN_STEP:
            return "device->rx->rxAgcCtrl->powerAgc->pmdLowerHighGainStepRecovery out of range in MYKONOS_setupRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_RX_PMD_UPPER_LOW_GAIN_STEP:
            return "device->rx->rxAgcCtrl->powerAgc->pmdUpperLowGainStepAttack out of range in MYKONOS_setupRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_RX_PKDET_FAST_ATTACK_VALUE:
            return "device->rx->rxAgcCtrl->peakAgc->apdFastAttack or hb2FastAttack out of range in MYKONOS_setupRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_RX_HB2_HIGH_THRESH_PARM:
            return "device->rx->rxAgcCtrl->peakAgc->hb2HighThresh out of range in MYKONOS_setupRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_RX_HB2_LOW_THRESH_PARM:
            return "device->rx->rxAgcCtrl->peakAgc->hb2LowThresh out of range in MYKONOS_setupRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_RX_HB2_VERY_LOW_THRESH_PARM:
            return "device->rx->rxAgcCtrl->peakAgc->hb2VeryLowThresh out of range in MYKONOS_setupRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_RX_APD_HIGH_GAIN_STEP_PARM:
            return "device->rx->rxAgcCtrl->peakAgc->apdHighGainStepAttack out of range in MYKONOS_setupRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_RX_APD_LOW_GAIN_STEP_PARM:
            return "device->rx->rxAgcCtrl->peakAgc->apdLowGainStepRecovery out of range in MYKONOS_setupRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_RX_HB2_HIGH_GAIN_STEP_PARM:
            return "device->rx->rxAgcCtrl->peakAgc->hb2HighGainStepAttack out of range in MYKONOS_setupRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_RX_HB2_LOW_GAIN_STEP_PARM:
            return "device->rx->rxAgcCtrl->peakAgc->hb2LowGainStepRecovery out of range in MYKONOS_setupRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_RX_HB2_VERY_LOW_GAIN_STEP_PARM:
            return "device->rx->rxAgcCtrl->peakAgc->hb2VeryLowGainStepRecovery out of range in MYKONOS_setupRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_RX_HB2_OVLD_ENABLE:
            return "device->rx->rxAgcCtrl->peakAgc->hb2OverloadEnable out of range in MYKONOS_setupRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_RX_HB2_OVLD_THRESH_CNT:
            return "device->rx->rxAgcCtrl->peakAgc->hb2OverloadThreshCnt out of range in MYKONOS_setupRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_RX_HB2_OVLD_DUR_CNT:
            return "device->rx->rxAgcCtrl->peakAgc->hb2OverloadDurationCnt out of range in MYKONOS_setupRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_OBSRX_STRUCT_INIT:
            return "Invalid OBSRX AGC structure detected at &device->obsRx->orxAgcCtrl in MYKONOS_setupObsRxAgc()\n";
        case MYKONOS_ERR_INV_AGC_OBSRX_PEAK_STRUCT_INIT:
            return "Data structure for obsRx->orxAgcCtrl->peakAgc not initialized when used in MYKONOS_setupObsRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_OBSRX_PWR_STRUCT_INIT:
            return "Data structure for obsRx->orxAgcCtrl->powerAgc not initialized when used in MYKONOS_setupObsRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_OBSRX_MAX_GAIN_INDEX:
            return "device->obsRx->orxAgcCtrl->agcObsRxMaxGainIndex out of range in  MYKONOS_setupObsRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_OBSRX_MIN_GAIN_INDEX:
            return "device->obsRx->orxAgcCtrl->agcObsRxMinGainIndex out of range in MYKONOS_setupObsRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_OBSRX_SELECT:
            return "device->obsRx->orxAgcCtrl->agcObsRxSelect out of range in MYKONOS_setupObsRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_OBSRX_GAIN_UPDATE_TIME_PARM:
            return "device->obsRx->orxAgcCtrl->agcGainUpdateTime_us out of range in MYKONOS_setupObsRxAgc()\n";
        case MYKONOS_ERR_INV_AGC_OBSRX_PEAK_WAIT_TIME_PARM:
            return "device->obsRx->orxAgcCtrl->agcPeakWaitTime out of range in MYKONOS_setupObsRxAgc()\n";
        case MYKONOS_ERR_INV_AGC_OBSRX_SLOW_LOOP_SETTLING_DELAY:
            return "device->obsRx->orxAgcCtrl->agcSlowLoopSettlingDelay out of range in MYKONOS_setupObsRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_OBSRX_PMD_MEAS_DURATION:
            return "device->obsRx->orxAgcCtrl->powerAgc->pmdMeasDuration out of range in MYKONOS_setupObsRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_OBSRX_PMD_MEAS_CONFIG:
            return "device->obsRx->orxAgcCtrl->powerAgc->pmdMeasConfig out of range in MYKONOS_setupObsRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_OBSRX_LOW_THS_PREV_GAIN_INC:
            return "device->obsRx->orxAgcCtrl->agcLowThsPreventGainIncrease out of range in MYKONOS_setupObsRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_OBSRX_PEAK_THRESH_MODE:
            return "device->obsRx->orxAgcCtrl->agcPeakThresholdMode out of range in MYKONOS_setupObsRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_OBSRX_RESET_ON_RX_ENABLE:
            return "device->obsRx->orxAgcCtrl->agcResetOnRxEnable out of range in MYKONOS_setupObsRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_OBSRX_ENABLE_SYNC_PULSE_GAIN_COUNTER:
            return "device->obsRx->orxAgcCtrl->agcEnableSyncPulseForGainCounter out of range in MYKONOS_setupObsRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_OBSRX_PMD_LOWER_HIGH_THRESH:
            return "device->obsRx->orxAgcCtrl->powerAgc->pmdLowerHighThresh out of range in MYKONOS_setupObsRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_OBSRX_PMD_UPPER_LOW_THRESH:
            return "device->obsRx->orxAgcCtrl->powerAgc->pmdUpperLowThresh out of range in MYKONOS_setupObsRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_OBSRX_PMD_LOWER_LOW_THRESH:
            return "device->obsRx->orxAgcCtrl->powerAgc->pmdLowerLowThresh out of range in MYKONOS_setupObsRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_OBSRX_PMD_UPPER_HIGH_THRESH:
            return "device->obsRx->orxAgcCtrl->powerAgc->pmdUpperHighThresh out of range in MYKONOS_setupObsRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_OBSRX_PMD_UPPER_HIGH_GAIN_STEP:
            return "device->obsRx->orxAgcCtrl->powerAgc->pmdUpperHighGainStepAttack out of range in MYKONOS_setupObsRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_OBSRX_PMD_LOWER_LOW_GAIN_STEP:
            return "device->obsRx->orxAgcCtrl->powerAgc->pmdLowerLowGainStepRecovery out of range in MYKONOS_setupObsRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_OBSRX_PMD_UPPER_LOW_GAIN_STEP:
            return "device->obsRx->orxAgcCtrl->powerAgc->pmdUpperLowGainStepAttack out of range in MYKONOS_setupObsRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_OBSRX_PKDET_FAST_ATTACK_VALUE:
            return "device->obsRx->orxAgcCtrl->peakAgc->apdFastAttack or hb2FastAttack out of range in MYKONOS_setupObsRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_OBSRX_APD_HIGH_THRESH_PARM:
            return "device->obsRx->orxAgcCtrl->apdHighThresh out of range in MYKONOS_setupObsRxAgc()\n";
        case MYKONOS_ERR_INV_AGC_OBSRX_APD_LOW_THRESH_PARM:
            return "device->obsRx->orxAgcCtrl->apdLowThresh out of range in MYKONOS_setupObsRxAgc()\n";
        case MYKONOS_ERR_INV_AGC_OBSRX_HB2_HIGH_THRESH_PARM:
            return "device->obsRx->orxAgcCtrl->peakAgc->hb2HighThresh out of range in MYKONOS_setupObsRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_OBSRX_HB2_LOW_THRESH_PARM:
            return "device->obsRx->orxAgcCtrl->peakAgc->hb2LowThresh out of range in MYKONOS_setupObsRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_OBSRX_HB2_VERY_LOW_THRESH_PARM:
            return "device->obsRx->orxAgcCtrl->peakAgc->hb2VeryLowThresh out of range in MYKONOS_setupObsRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_OBSRX_APD_HIGH_GAIN_STEP_PARM:
            return "device->obsRx->orxAgcCtrl->peakAgc->apdHighGainStepAttack out of range in MYKONOS_setupObsRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_OBSRX_APD_LOW_GAIN_STEP_PARM:
            return "device->obsRx->orxAgcCtrl->peakAgc->apdLowGainStepRecovery out of range in MYKONOS_setupObsRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_OBSRX_HB2_HIGH_GAIN_STEP_PARM:
            return "device->obsRx->orxAgcCtrl->peakAgc->hb2HighGainStepAttack out of range in MYKONOS_setupObsRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_OBSRX_HB2_LOW_GAIN_STEP_PARM:
            return "device->obsRx->orxAgcCtrl->peakAgc->hb2LowGainStepRecovery out of range in MYKONOS_setupObsRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_OBSRX_HB2_VERY_LOW_GAIN_STEP_PARM:
            return "device->obsRx->orxAgcCtrl->peakAgc->hb2VeryLowGainStepRecovery out of range in MYKONOS_setupObsRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_OBSRX_HB2_OVLD_ENABLE:
            return "device->obsRx->orxAgcCtrl->peakAgc->hb2OverloadEnable out of range in MYKONOS_setupObsRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_OBSRX_HB2_OVLD_DUR_CNT:
            return "device->obsRx->orxAgcCtrl->peakAgc->hb2OverloadDurationCnt out of range in MYKONOS_setupObsRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_OBSRX_HB2_OVLD_THRESH_CNT:
            return "device->obsRx->orxAgcCtrl->peakAgc->hb2OverloadThresholdCnt out of range in MYKONOS_setupObsRxAgc() \n";
        case MYKONOS_ERR_INV_AGC_OBSRX_PMD_LOWER_HIGH_GAIN_STEP:
            return "device->obsRx->orxAgcCtrl->powerAgc->pmdLowerHighGainStepRecovery out of range in MYKONOS_setupObsRxAgc() \n";
        case MYKONOS_ERR_WAITFOREVENT_TIMEDOUT_CALPLL_LOCK:
            return "CAL PLL Lock event timed out in MYKONOS_waitForEvent()\n";
        case MYKONOS_ERR_WAITFOREVENT_TIMEDOUT_CLKPLLCP:
            return "Clock PLL Charge Pump Cal event timed out in MYKONOS_waitForEvent()\n";
        case MYKONOS_ERR_WAITFOREVENT_TIMEDOUT_CLKPLL_LOCK:
            return "Clock PLL Lock event timed out in MYKONOS_waitForEvent()\n";
        case MYKONOS_ERR_WAITFOREVENT_TIMEDOUT_RXPLLCP:
            return "RX PLL Charge Pump Cal event timed out in MYKONOS_waitForEvent()\n";
        case MYKONOS_ERR_WAITFOREVENT_TIMEDOUT_RXPLL_LOCK:
            return "RX PLL Lock event timed out in MYKONOS_waitForEvent()\n";
        case MYKONOS_ERR_WAITFOREVENT_TIMEDOUT_TXPLLCP:
            return "TX PLL Charge Pump Cal event timed out in MYKONOS_waitForEvent()\n";
        case MYKONOS_ERR_WAITFOREVENT_TIMEDOUT_TXPLL_LOCK:
            return "TX PLL Lock event timed out in MYKONOS_waitForEvent()\n";
        case MYKONOS_ERR_WAITFOREVENT_TIMEDOUT_SNIFFPLLCP:
            return "Sniffer PLL Charge Pump Cal event timed out in MYKONOS_waitForEvent()\n";
        case MYKONOS_ERR_WAITFOREVENT_TIMEDOUT_SNIFFPLL_LOCK:
            return "Sniffer PLL Lock event timed out in MYKONOS_waitForEvent()\n";
        case MYKONOS_ERR_WAITFOREVENT_TIMEDOUT_RXBBFCALDONE:
            return "RX Baseband Filter Cal event timed out in MYKONOS_waitForEvent()\n";
        case MYKONOS_ERR_WAITFOREVENT_TIMEDOUT_TXBBFCALDONE:
            return "TX Baseband Filter Cal timed out in MYKONOS_waitForEvent()\n";
        case MYKONOS_ERR_WAITFOREVENT_TIMEDOUT_RFDCCALDONE:
            return "RF DC Offset Cal event timed out in MYKONOS_waitForEvent()\n";
        case MYKONOS_ERR_WAITFOREVENT_TIMEDOUT_ADCTUNECALDONE:
            return "ADC Tuner Cal event timed out in MYKONOS_waitForEvent()\n";
        case MYKONOS_ERR_WAITFOREVENT_TIMEDOUT_RX1ADCPROFILE:
            return "Rx1 ADC Profile Loading event timed out in MYKONOS_waitForEvent()\n";
        case MYKONOS_ERR_WAITFOREVENT_TIMEDOUT_RX2ADCPROFILE:
            return "Rx2 ADC Profile Loading event timed out in MYKONOS_waitForEvent()\n";
        case MYKONOS_ERR_WAITFOREVENT_TIMEDOUT_ORXADCPROFILE:
            return "ObsRx ADC Profile Loading event timed out in MYKONOS_waitForEvent()\n";
        case MYKONOS_ERR_WAITFOREVENT_TIMEDOUT_RCALDONE:
            return "Resistor Cal event timed out in MYKONOS_waitForEvent()\n";
        case MYKONOS_ERR_WAITFOREVENT_TIMEDOUT_ARMBUSY:
            return "ARM Busy event timed out in MYKONOS_waitForEvent()\n";
        case MYKONOS_ERR_WAITFOREVENT_TIMEDOUT_INITARMDONE:
            return "Initialize ARM event timed out in MYKONOS_waitForEvent()\n";
        case MYKONOS_ERR_TIMEDOUT_ARMMAILBOXBUSY:
            return "ARM Mailbox Busy. Command not executed in MYKONOS_sendArmCommand()\n";
        case MYKONOS_ERR_PU_OBSRXPATH_ARMERROR:
            return "ARM Command Error in MYKONOS_setObsRxPathSource()\n";
        case MYKONOS_ERR_EN_TRACKING_CALS_ARMSTATE_ERROR:
            return "Device not in radioOff/IDLE state. Error in MYKONOS_enableTrackingCals()\n";
        case MYKONOS_ERR_EN_TRACKING_CALS_ARMERROR:
            return "ARM Command Error in MYKONOS_enableTrackingCals()\n";
        case MYKONOS_ERR_SETRFPLL_ARMERROR:
            return "ARM Command Error in MYKONOS_setRfPllFrequency()\n";
        case MYKONOS_ERR_INIT_INV_TXINPUTHB_PARM:
            return "device->tx->txProfile->txInputHbInterpolation out of range in MYKONOS_initialize(): valid = 1,2 or 4\n";
        case MYKONOS_ERR_LOAD_ADCPROFILE_INV_VCODIV:
            return "device->clocks->clkPllVcoDiv value not supported in MYKONOS_loadAdcProfiles\n";
        case MYKONOS_ERR_LOAD_ADCPROFILE_CUSTOM_RXREQUIRED:
            return "Custom RX ADC Profile required\n";
        case MYKONOS_ERR_LOAD_ADCPROFILE_CUSTOM_ORXREQUIRED:
            return "Custom ORX ADC Profile required\n";
        case MYKONOS_ERR_LOAD_ADCPROFILE_CUSTOM_SNRXREQUIRED:
            return "Custom SNRX ADC Profile required\n";
        case MYKONOS_ERR_LOAD_ADCPROFILE_CUSTOM_LBREQUIRED:
            return "Custom Loopback ADC profile required\n";
        case MYKONOS_ERR_LOAD_LBADCPROFILE_ARMMEM_FAILED:
            return "WriteArmMemory call failed while writing the Loopback ADC profile into ARM memory\n";
        case MYKONOS_ERR_LOAD_SNRXADCPROFILE_ARMMEM_FAILED:
            return "WriteArmMemory call failed while writing the Sniffer ADC profile into ARM memory\n";
        case MYKONOS_ERR_LOAD_ORXADCPROFILE_ARMMEM_FAILED:
            return "WriteArmMemory call failed while writing the ObsRx ADC profile into ARM memory\n";
        case MYKONOS_ERR_LOAD_RXADCPROFILE_ARMMEM_FAILED:
            return "WriteArmMemory call failed while writing the RX ADC profile into ARM memory\n";
        case MYKONOS_ERR_LOAD_ADCPROFILE_SNRX_ADCDIV_ZERO:
            return "SnRx profile has invalid ADC divider of 0, causing a divide by zero\n";
        case MYKONOS_ERR_LOAD_ADCPROFILE_ORXADCDIV_ZERO:
            return "ORx profile has invalid ADC divider of 0, causing a divide by zero\n";
        case MYKONOS_ERR_LOAD_ADCPROFILE_RXADCDIV_ZERO:
            return "Rx profile has invalid ADC divider of 0, causing a divide by zero\n";
        case MYKONOS_ERR_LOAD_ADCPROFILE_MISSING_ORX_PROFILE:
            return "If Tx Profile is valid, a matching ORx profile must be provided to set ADC divider and digital filtering for loopback calibrations\n";
        case MYKONOS_ERR_SETUP_PA_PROT_INV_AVG_DURATION:
            return "Invalid setting for avgDuration in MYKONOS_setupPaProtection(...)\n";
        case MYKONOS_ERR_SETUP_PA_PROT_INV_ATTEN_STEP:
            return "Invalid setting for attenStepSize in MYKONOS_setupPaProtection(...)\n";
        case MYKONOS_ERR_SETUP_PA_PROT_INV_ATTEN_ENABLE:
            return "Invalid setting for txAttenControlEnable in MYKONOS_setupPaProtection(...)\n";
        case MYKONOS_ERR_SETUP_PA_PROT_INV_POWER_THRESH:
            return "Invalid setting for powerThreshold in MYKONOS_setupPaProtection(...)\n";
        case MYKONOS_ERR_SETUP_PA_PROT_INV_TX_CHANNEL:
            return "Invalid TX channel selected for MYKONOS_getDacPower(...)\n";
        case MYKONOS_ERR_GET_DAC_PWR_INV_POINTER:
            return "Cannot return DAC Power information (invalid pointer)\n";
        case MYKONOS_ERR_GET_PA_FLAG_STATUS_INV_POINTER:
            return "Cannot return PA Protection Flag Status information (invalid pointer)\n";
        case MYKONOS_ERR_GET_OBSRX_OVERLOADS_NULL_PARM:
            return "MYKONOS_getObsRxPathOverloads() has null *obsRxOverloads parameter\n";
        case MYKONOS_ERR_GET_RX1_OVERLOADS_NULL_PARM:
            return "MYKONOS_getRxPathOverloads() has null *rx1Overloads parameter\n";
        case MYKONOS_ERR_GET_RX2_OVERLOADS_NULL_PARM:
            return "MYKONOS_getRxPathOverloads() has null *rx2Overloads parameter\n";
        case MYKONOS_ERR_GETRADIOSTATE_NULL_PARAM:
            return "MYKONOS_getRadioState() has null *radioStatus parameter\n";
        case MYKONOS_ERR_ABORT_INITCALS_NULL_PARAM:
            return "MYKONOS_abortInitCals() has null *calsCompleted parameter\n";
        case MYKONOS_ERR_WAIT_INITCALS_ARMERROR:
            return "MYKONOS_waitInitCals() returned an ARM error\n";
        case MYKONOS_ERR_WAIT_INITCALS_NULL_PARAM:
            return "MYKONOS_waitInitCals() has one or more null parameters\n";
        case MYKONOS_ERR_CHECK_PLL_LOCK_NULL_PARM:
            return "MYKONOS_checkPllsLockStatus() has a null *pllLockStatus parameter\n";
        case MYKONOS_ERR_GET_TXFILTEROVRG_NULL_PARM:
            return "MYKONOS_getTxFilterOverRangeStatus() has a null *txFilterStatus parameter\n";
        case MYKONOS_ERR_PROGRAM_RXGAIN_TABLE_NULL_PARM:
            return "MYKONOS_programRxGainTable() has a null *gainTablePtr parameter\n";
        case MYKONOS_ERR_PROGRAMFIR_NULL_PARM:
            return "MYKONOS_programFir() has a null *firFilter parameter\n";
        case MYKONOS_ERR_PROGRAMFIR_COEFS_NULL:
            return "MYKONOS_programFir() has a null coef array in *firFilter structure\n";
        case MYKONOS_ERR_READ_DEFRAMERSTATUS_NULL_PARAM:
            return "MYKONOS_readDeframerStatus() has a null *deframerStatus parameter\n";
        case MYKONOS_ERR_READ_DEFRAMERPRBS_NULL_PARAM:
            return "MYKONOS_readDeframerPrbscounters() has a null *prbsErrorCount parameter\n";
        case MYKONOS_ERR_READ_ORXFRAMERSTATUS_NULL_PARAM:
            return "MYKONOS_readOrxFramerStatus() has a null *obsFramerStatus parameter\n";
        case MYKONOS_ERR_READ_RXFRAMERSTATUS_NULL_PARAM:
            return "MYKONOS_readRxFramerStatus() has a null *framerStatus parameter\n";
        case MYKONOS_ERR_ARMCMDSTATUS_NULL_PARM:
            return "MYKONOS_waitArmCmdStatus() has a null *cmdStatByte parameter\n";
        case MYKONOS_ERR_READARMCMDSTATUS_NULL_PARM:
            return "MYKONOS_readArmCmdStatus() has one or more null pointer parameters\n";
        case MYKONOS_ERR_READARMCMDSTATUS_INV_OPCODE_PARM:
            return "MYKONOS_readArmCmdStatusByte() has an invalid opcode parameter\n";
        case MYKONOS_ERR_READARMCMDSTATUSBYTE_NULL_PARM:
            return "MYKONOS_readArmCmdStatusByte() has a null *cmdStatByte parameter\n";
        case MYKONOS_ERR_ARMCMD_NULL_PARM:
            return "MYKONOS_sendArmCommand() has a null *extendedData parameter\n";
        case MYKONOS_ERR_WRITEARMMEM_NULL_PARM:
            return "MYKONOS_writeArmMem() has a null *data parameter\n";
        case MYKONOS_ERR_LOADBIN_NULL_PARAM:
            return "MYKONOS_loadArmFromBinary() has a null *binary parameter\n";
        case MYKONOS_ERR_GETTX1ATTEN_NULL_PARM:
            return "MYKONOS_getTx1Attenuation() has NULL tx1Attenuation_mdB parameter\n";
        case MYKONOS_ERR_GETTX2ATTEN_NULL_PARM:
            return "MYKONOS_getTx2Attenuation() has NULL tx2Attenuation_mdB parameter\n";
        case MYKONOS_ERR_ENFRAMERLINK_INV_LANESEN_PARM:
            return "Invalid serializer Lanes Enable parameter in Rx framer structure\n";
        case MYKONOS_ERR_ENOBSFRAMERLINK_INV_LANESEN_PARM:
            return "Invalid serializer Lanes Enable parameter in ObsRx framer structure\n";
        case MYKONOS_ERR_ENTXNCO_TXPROFILE_INVALID:
            return "Can not enable Tx NCO when Tx Profile is invalid\n";
        case MYKONOS_ERR_ENTXNCO_TX1_FREQ_INVALID:
            return "Invalid Tx1 NCO frequency\n";
        case MYKONOS_ERR_ENTXNCO_TX2_FREQ_INVALID:
            return "Invalid Tx2 NCO frequency\n";
        case MYKONOS_ERR_JESD204B_ILAS_MISMATCH_NULLPARAM:
            return "Function parameter has NULL pointer in MYKONOS_jesd204bIlasCheck()\n";
        case MYKONOS_ERR_GET_PLLFREQ_INV_REFCLKDIV:
            return "CLKPLL refclk divider was read back as an invalid setting in MYKONOS_getRfPllFrequency()\n";
        case MYKONOS_ERR_GET_PLLFREQ_INV_HSDIV:
            return "CLKPLL HSDIV divider was read back as an invalid setting in MYKONOS_getRfPllFrequency()\n";
        case MYKONOS_ERR_GET_RX1_DEC_POWER_NULL_PARM:
            return "MYKONOS_getRx1DecPower() has a null *rx1DecPower_mdBFS parameter\n";
        case MYKONOS_ERR_GET_RX1_DEC_POWER_NUM_SAMPLES:
            return "MYKONOS_getRx1DecPower() numSamples greater than agcGainUpdateCounter\n";
        case MYKONOS_ERR_GET_RX2_DEC_POWER_NULL_PARM:
            return "MYKONOS_getRx2DecPower() has a null *rx2DecPower_mdBFS parameter\n";
        case MYKONOS_ERR_GET_RX2_DEC_POWER_NUM_SAMPLES:
            return "MYKONOS_getRx2DecPower() numSamples greater than agcGainUpdateCounter\n";
        case MYKONOS_ERR_GET_OBSRX_DEC_POWER_NULL_PARM:
            return "MYKONOS_getObsRxDecPower() has a null *obsRxDecPower_mdBFS parameter\n";
        case MYKONOS_ERR_GET_OBSRX_DEC_POWER_NUM_SAMPLES:
            return "MYKONOS_getObsRxDecPower() numSamples greater than agcGainUpdateCounter\n";
        case MYKONOS_ERR_GETARMVER_NULL_PARM:
            return "MYKONOS_getArmVersion() has a NULL pointer in one of the function parameters\n";
        case MYKONOS_ERR_EN_DPDTRACKING_ARMSTATE_ERROR:
            return "ARM is not in the RadioOff state before MYKONOS_enableDpdTracking(), call MYKONOS_radioOff()\n";
        case MYKONOS_ERR_RESTDPDMOD_WRONGBUFFERSIZE:
            return "MYKONOS_restoreDpdModel() supplied DPD Model Data Buffer is incorrect size\n";
        case MYKONOS_ERR_RESTDPDMOD_ARMSTATE:
            return "ARM is not in the RadioOff state before MYKONOS_restoreDpdModel(), call MYKONOS_radioOff()\n";
        case MYKONOS_ERR_RESTDPDMOD_INVALID_TXCHANNEL:
            return "MYKONOS_restoreDpdModel() invalid txChannel specified\n";
        case MYKONOS_ERR_SAVDPDMOD_WRONGBUFFERSIZE:
            return "MYKONOS_saveDpdModelL() supplied DPD Model Data Buffer is incorrect size\n";
        case MYKONOS_ERR_SAVDPDMOD_ARMSTATE:
            return "ARM is not in the RadioOff state before MYKONOS_saveDpdModelL(), call MYKONOS_radioOff()\n";
        case MYKONOS_ERR_SAVDPDMOD_INVALID_TXCHANNEL:
            return "MYKONOS_saveDpdModelL() invalid txChannel specified\n";
        case MYKONOS_ERR_EN_CLGCTRACKING_ARMSTATE_ERROR:
            return "ARM is not in the RadioOff state before MYKONOS_enableClgcTracking(), call MYKONOS_radioOff()\n";
        case MYKONOS_ERR_CFGDPD_ARMSTATE_ERROR:
            return "MYKONOS_configDpd() was called while ARM was not in radioOff state\n";
        case MYKONOS_ERR_CFGDPD_TXORX_PROFILE_INV:
            return "Tx or ORx profile is not valid, both are necessary for DPD features in MYKONOS_configDpd()\n";
        case MYKONOS_ERR_CFGDPD_NULL_DPDCFGSTRUCT:
            return "device->tx->dpdConfig structure has NULL pointer in MYKONOS_configDpd()\n";
        case MYKONOS_ERR_WRITEARMCFG_ARMERRFLAG:
            return "The ARM reported a error while writing to an ARM config structure in MYKONOS_writeArmCfgStruct()\n";
        case MYKONOS_ERR_CFGDPD_INV_DPDDAMPING:
            return "ERROR: device->tx->dpdConfig->damping parameter is out of range (0-15)\n";
        case MYKONOS_ERR_CFGDPD_INV_DPDSAMPLES:
            return "ERROR: device->tx->dpdConfig->samples parameter is out of range (64 - 32768)\n";
        case MYKONOS_ERR_CFGDPD_INV_NUMWEIGHTS:
            return "ERROR: device->tx->dpdConfig->numWeights parameter is out of range (0-3)\n";
        case MYKONOS_ERR_CFGDPD_INV_DPDOUTLIERTHRESH:
            return "ERROR: device->tx->dpdConfig->outlierThreshold parameter is out of range\n";
        case MYKONOS_ERR_CFGDPD_INV_DPDPRIORWEIGHT:
            return "ERROR: device->tx->dpdConfig->modelPriorWeight parameter is out of range (Valid 0-32)\n";
        case MYKONOS_ERR_CFGCLGC_INV_DESIREDGAIN:
            return "ERROR: device->tx->clgcConfig->tx1DesiredGain or tx2DesiredGain parameter is out of range\n";
        case MYKONOS_ERR_CFGCLGC_INV_TXATTENLIMIT:
            return "ERROR: device->tx->clgcConfig->tx1AttenLimit or tx2AttenLimit parameter is out of range\n";
        case MYKONOS_ERR_CFGCLGC_INV_CLGC_CTRLRATIO:
            return "ERROR: device->tx->clgcConfig->tx1ControlRatio or tx2ControlRatio parameter is out of range\n";
        case MYKONOS_ERR_CFGDPD_INV_DPD_ADDDELAY:
            return "ERROR: device->tx->dpdConfig->additionalDelayOffset parameter is out of range\n";
        case MYKONOS_ERR_CFGDPD_INV_PNSEQLEVEL:
            return "ERROR: device->tx->dpdConfig->pathDelayPnSeqLevel parameter is out of range\n";
        case MYKONOS_ERR_CFGDPD_INV_MODELVERSION:
            return "ERROR: device->tx->dpdConfig->modelVersion parameter is out of range\n";
        case MYKONOS_ERR_READARMCFG_ARMERRFLAG:
            return "ERROR: MYKONOS_readArmConfig failed due to an ARM error\n";
        case MYKONOS_ERR_GETPENDTRKCALS_NULL_PARAM:
            return "MYKONOS_getPendingTrackingCals() has NULL function parameter\n";
        case MYKONOS_ERR_ARMCMDSTATUS_ARMERROR:
            return "MYKONOS_waitArmCmdStatus() exited due to ARM error for the desired ARM opcode\n";
        case MYKONOS_ERR_WAITARMCMDSTATUS_TIMEOUT:
            return "MYKONOS_waitArmCmdStatus() timed out waiting for the ARM to complete the requested command\n";
        case MYKONOS_ERR_PU_GETOBSRXPATH_ARMERROR:
            return "ARM returned an error while trying to get the ObsRx Path source in MYKONOS_getObsRxPathSource()\n";
        case MYKONOS_ERR_GETDPDCFG_NULL_DPDCFGSTRUCT:
            return "MYKONOS_getDpdConfig() could not complete due to a NULL pointer to device->tx->dpdConfig structure\n";
        case MYKONOS_ERR_GETDPDCFG_TXORX_PROFILE_INV:
            return "The Tx and ORx profiles must be valid for DPD related functions\n";
        case MYKONOS_ERR_GETDPDSTATUS_ARMERRFLAG:
            return "MYKONOS_getDpdStatus() reported an ARM error with the ARM Get command\n";
        case MYKONOS_ERR_GETDPDSTATUS_NULLPARAM:
            return "MYKONOS_getDpdStatus() has NULL pointer in the function parameter\n";
        case MYKONOS_ERR_SETDEFOBSRXPATH_NULL_OBSRX_STRUCT:
            return "Observation profile not valid, device->obsRx structure is NULL in MYKONOS_setDefaultObsRxPath()\n";
        case MYKONOS_ERR_GETCLGCSTATUS_NULLPARAM:
            return "MYKONOS_getClgcStatus() has NULL clgcStatus parameter\n";
        case MYKONOS_ERR_GETCLGCSTATUS_ARMERRFLAG:
            return "ARM reported an error with the ARM GET command during MYKONOS_getClgcStatus()\n";
        case MYKONOS_ERR_READ_DEFFIFODEPTH_NULL_PARAM:
            return "MYKONOS_getDeframerFifoDepth() function parameter fifoDepth is a NULL pointer\n";
        case MYKONOS_ERR_READ_DEFFIFODEPTH_LMFCCOUNT_NULL_PARAM:
            return "MYKONOS_getDeframerFifoDepth() function parameter readEnLmfcCount is a NULL pointer\n";
        case MYKONOS_ERR_GETDPDSTATUS_INV_CH:
            return "MYKONOS_getDpdStatus() txChannel parameter is not valid (TX1 and TX2 are the only valid options)\n";
        case MYKONOS_ERR_GETCLGCSTATUS_INV_CH:
            return "MYKONOS_getClgcStatus() txChannel parameter is not valid (TX1 and TX2 are the only valid options)\n";
        case MYKONOS_ERR_INIT_INV_TXINPUTHB_INV_RATE:
            return "Invalid Tx profile: When using Tx Input halfband, IQ rate must not exceed 160MSPS\n";
        case MYKONOS_ERR_INIT_INV_TXINPUTHB0_INV_RATE:
            return "Invalid Tx profile: When using Tx Input halfband 0, IQ rate must not exceed 80MSPS\n";
        case MYKONOS_ERR_RXFIR_TAPSEXCEEDED:
            return "Rx PFIR can not be clocked fast enough to handle the number of FIR coefficents provided\n";
        case MYKONOS_ERR_ORXFIR_TAPSEXCEEDED:
            return "ORx PFIR can not be clocked fast enough to handle the number of FIR coefficents provided\n";
        case MYKONOS_ERR_SNRXFIR_TAPSEXCEEDED:
            return "Sniffer Rx PFIR can not be clocked fast enough to handle the number of FIR coefficents provided\n";
        case MYKONOS_ERR_TXFIR_INV_NUMTAPS_PARM:
            return "Invalid number of Tx FIR coefficients\n";
        case MYKONOS_ERR_TXFIR_INV_NUMROWS:
            return "Invalid number of PFIR coefficient rows\n";
        case MYKONOS_ERR_TXFIR_TAPSEXCEEDED:
            return "Too many Tx PFIR taps for the IQ sample rate.  FIR processing clock can not run fast enough to handle the number of taps\n";
        case MYKONOS_ERR_GETINITCALSTATUS_NULL_PARAM:
            return "MYKONOS_getInitCalStatus() has a null pointer in parameter list\n";
        case MYKONOS_ERR_GETINITCALSTATUS_ARMERROR:
            return "MYKONOS_getInitCalStatus() returned an ARM error while getting the init cal status information";
        case MYKONOS_ERR_CFGCLGC_TXORX_PROFILE_INV:
            return "Tx and ObsRx profiles must be valid to use the CLGC feature in MYKONOS_configClgc()\n";
        case MYKONOS_ERR_CFGCLGC_NULL_CLGCCFGSTRUCT:
            return "CLGC config structure is NULL in device->tx->clgcConfig in MYKONOS_configClgc()\n";
        case MYKONOS_ERR_CFGCLGC_ARMSTATE_ERROR:
            return "CLGC config could not be set because ARM is not in radioOff or ready state in MYKONOS_configClgc()\n";
        case MYKONOS_ERR_GETCLGCCFG_TXORX_PROFILE_INV:
            return "Could not read back CLGC status because Tx and ORx profiles are not valid\n";
        case MYKONOS_ERR_GETCLGCCFG_NULL_CFGSTRUCT:
            return "Could not read back CLGC status because return structure device->tx->clgcConfig pointer is NULL\n";
        case MYKONOS_ERR_CALCDIGCLK_NULLDEV_PARAM:
            return "device structure pointer is NULL in MYKONOS_calculateDigitalClocks()\n";
        case MYKONOS_ERR_CALCDIGCLK_NULL_CLKSTRUCT:
            return "device->clocks structure pointer is NULL in MYKONOS_calculateDigitalClocks()\n";
        case MYKONOS_ERR_NULL_DEVICE_PARAM:
            return "The device pointer in the calling function's parameter list is a NULL pointer\n";
        case MYKONOS_ERR_CALCDEVCLK_NULLPARAM:
            return "MYKONOS_calculateScaledDeviceClk_kHz() has a NULL pointer in one of the function parameters\n";
        case MYKONOS_ERR_CFGCLGC_INV_CLGC_ADDDELAY:
            return "CLGC Additional Delay parameter is out of range in MYKONOS_configClgc\n";
        case MYKONOS_ERR_CFGCLGC_INV_PNSEQLEVEL:
            return "CLGC PN Sequence level parameter is out of range in MYKONOS_configClgc\n";
        case MYKONOS_ERR_CFGVSWR_TXORX_PROFILE_INV:
            return "The Tx and ORx profiles must be valid to use the VSWR feature in MYKONOS_configVswr()\n";
        case MYKONOS_ERR_CFGVSWR_ARMSTATE_ERROR:
            return "ARM must be in radioOff state to configure the VSWR config parameters in MYKONOS_configVswr()\n";
        case MYKONOS_ERR_CFGVSWR_INV_3P3GPIOPIN:
            return "VSWR 3.3v GPIO pin selection is out of range (0-11) in MYKONOS_configVswr()\n";
        case MYKONOS_ERR_CFGVSWR_INV_PNSEQLEVEL:
            return "VSWR init PN sequence level is out of range in MYKONOS_configVswr()\n";
        case MYKONOS_ERR_CFGVSWR_INV_VSWR_ADDDELAY:
            return "VSWR additionalDelay member is out of range in MYKONOS_configVswr()\n";
        case MYKONOS_ERR_CFGVSWR_NULL_VSWRCFGSTRUCT:
            return "device->tx->vswrConfig pointer is null in MYKONOS_confgiVswr()\n";
        case MYKONOS_ERR_GETVSWRCFG_NULL_CFGSTRUCT:
            return "device->tx->vswrConfig pointer is null in MYKONOS_getVswrConfig()\n";
        case MYKONOS_ERR_GETVSWRCFG_TXORX_PROFILE_INV:
            return "Tx and ORx profiles must be valid in MYKONOS_getVswrConfig()\n";
        case MYKONOS_ERR_GETVSWRSTATUS_ARMERRFLAG:
            return "ARM returned an error while attempting to read the VSWR status structure\n";
        case MYKONOS_ERR_GETVSWRSTATUS_INV_CH:
            return "Invalid Tx channel parameter passed to MYKONOS_getVswrStatus()\n";
        case MYKONOS_ERR_GETVSWRSTATUS_NULLPARAM:
            return "vswrStatus function parameter is null in MYKONOS_getVswrStatus\n";
        case MYKONOS_ERR_SET_RX_MAX_GAIN_INDEX:
            return "Max gain index bigger than max gain index loaded table in MYKONOS_setRxAgcMinMaxGainIndex().\n";
        case MYKONOS_ERR_SET_RX_MIN_GAIN_INDEX:
            return "Min gain index lower than min gain index loaded table in MYKONOS_setRxAgcMinMaxGainIndex().\n";
        case MYKONOS_ERR_AGC_MIN_MAX_RX_CHANNEL:
            return "Wrong RX channel selected in MYKONOS_setRxAgcMinMaxGainIndex().\n";
        case MYKONOS_ERR_SET_ORX_MAX_GAIN_INDEX_CHANNEL:
            return "Wrong read back channel in MYKONOS_setObsRxAgcMinMaxGainIndex().\n";
        case MYKONOS_ERR_SET_ORX_MAX_GAIN_INDEX:
            return "Max gain index bigger than max gain index loaded table in MYKONOS_setObsRxAgcMinMaxGainIndex().\n";
        case MYKONOS_ERR_SET_ORX_MIN_GAIN_INDEX:
            return "Min gain index lower than min gain index loaded table in MYKONOS_setObsRxAgcMinMaxGainIndex().\n";
        case MYKONOS_ERR_AGC_MIN_MAX_ORX_CHANNEL:
            return "Wrong observation channel selected in MYKONOS_setObsRxAgcMinMaxGainIndex().\n";
        case MYKONOS_ERR_RX1_TEMP_GAIN_COMP_RANGE:
            return "The temp gain compensation is outside range (-3000mdB to 3000mdB) in MYKONOS_setRx1TempGainComp().\n";
        case MYKONOS_ERR_RX1_TEMP_GAIN_COMP_STEP:
            return "Not valid temp gain compensation, step size is 250mdB in MYKONOS_setRx1TempGainComp().\n";
        case MYKONOS_ERR_RX2_TEMP_GAIN_COMP_RANGE:
            return "The temp gain compensation is outside range (-3000mdB to 3000mdB) in MYKONOS_setRx2TempGainComp().\n";
        case MYKONOS_ERR_RX2_TEMP_GAIN_COMP_STEP:
            return "Not valid temp gain compensation, step size is 250mdB in MYKONOS_setRx2TempGainComp().\n";
        case MYKONOS_ERR_OBS_RX_TEMP_GAIN_COMP_RANGE:
            return "The temp gain compensation is outside range (-3000mdB to 3000mdB) in MYKONOS_setObsRxTempGainComp().\n";
        case MYKONOS_ERR_OBS_RX_TEMP_GAIN_COMP_STEP:
            return "Not valid temp gain compensation, step size is 250mdB in MYKONOS_setObsRxTempGainComp().\n";
        case MYKONOS_ERR_RX1_TEMP_GAIN_COMP_NULL:
            return "rx1TempCompGain_mdB pointer is null MYKONOS_getRx1TempGainComp().\n";
        case MYKONOS_ERR_RX2_TEMP_GAIN_COMP_NULL:
            return "rx2TempCompGain_mdB pointer is null MYKONOS_getRx2TempGainComp().\n";
        case MYKONOS_ERR_OBS_RX_TEMP_GAIN_COMP_NULL:
            return "obsRxTempCompGain_mdB pointer is null MYKONOS_getObsRxTempGainComp().\n";
        case MYKONOS_ERR_GETORXQECSTATUS_NULLPARAM:
            return "Function parameter mykonosOrxQecStatus_t is a NULL pointer in MYKONOS_getOrxQecStatus().\n";
        case MYKONOS_ERR_GETORXQECSTATUS_INV_CH:
            return "Channel selection not valid in MYKONOS_getOrxQecStatus().\n";
        case MYKONOS_ERR_GETORXQECSTATUS_ARMERRFLAG:
            return "ARM command error in MYKONOS_getOrxQecStatus().\n";
        case MYKONOS_ERR_GETRXQECSTATUS_NULLPARAM:
            return "Function parameter mykonosRxQecStatus_t is a NULL pointer in MYKONOS_getRxQecStatus().\n";
        case MYKONOS_ERR_GETRXQECSTATUS_INV_CH:
            return "Channel selection not valid in MYKONOS_getRxQecStatus().\n";
        case MYKONOS_ERR_GETRXQECSTATUS_ARMERRFLAG:
            return "ARM command error in MYKONOS_getRxQecStatus().\n";
        case MYKONOS_ERR_GETTXQECSTATUS_NULLPARAM:
            return "Function parameter mykonosTxQecStatus_t is a NULL pointer in MYKONOS_getRxQecStatus().\n";
        case MYKONOS_ERR_GETTXQECSTATUS_INV_CH:
            return "Channel selection not valid in MYKONOS_getTxQecStatus().\n";
        case MYKONOS_ERR_GETTXQECSTATUS_ARMERRFLAG:
            return "ARM command error in MYKONOS_getTxQecStatus().\n";
        case MYKONOS_ERR_GETTXLOLSTATUS_NULLPARAM:
            return "Function parameter mykonosTxLolStatus_t is a NULL pointer in MYKONOS_getTLolStatus().\n";
        case MYKONOS_ERR_GETTXLOLSTATUS_INV_CH:
            return "Channel selection not valid in MYKONOS_getTLolStatus().\n";
        case MYKONOS_ERR_GETTXLOLSTATUS_ARMERRFLAG:
            return "ARM command error in MYKONOS_getTLolStatus().\n";
        case MYKONOS_ERR_RESCHEDULE_TRACK_CAL_INV:
            return "Not valid calibration passed to MYKONOS_rescheduleTrackingCal().\n";
        case MYKONOS_ERR_RESCHEDULE_TRACK_ARMERRFLAG:
            return "ARM error in MYKONOS_rescheduleTrackingCal().\n";
        case MYKONOS_ERR_ARMSTATE_EXCEPTION:
            return "ARM system problem has been detected.\n";
        case MYKONOS_ERR_ARMSTATE_CAL_ERROR:
            return "ARM has detected an error in the tracking calibrations.\n";
        case MYKONOS_ERR_ARMSTATE_PROFILE_ERROR:
            return "ARM has detected an illegal profile.\n";
        case MYKONOS_ERR_WAITARMCSTATE_TIMEOUT:
            return "Timeout occurred in MYKONOS_checkArmState().\n";
        case MYKONOS_ERR_GET_API_VERSION_NULL_PARAM:
            return "Null parameter passed to the function MYKONOS_getApiVersion().\n";
        case MYKONOS_ERR_GETPRODUCTID_NULL_PARAM:
            return "Null parameter passed to the function MYKONOS_getProductId().\n";
        case MYKONOS_ERR_TXPROFILE_IQRATE:
            return "Tx Profile IQ rate out of range.\n";
        case MYKONOS_ERR_TXPROFILE_RFBW:
            return "Tx Profile RF bandwidth out of range.\n";
        case MYKONOS_ERR_TXPROFILE_FILTER_INTERPOLATION:
            return "Tx Filter interpolation not valid.\n";
        case MYKONOS_ERR_TXPROFILE_FIR_COEFS:
            return "Tx FIR filter not valid.\n";
        case MYKONOS_ERR_RXPROFILE_RXCHANNEL:
            return " Rx channel is not valid.\n";
        case MYKONOS_ERR_RXPROFILE_IQRATE:
            return "Receiver profile out of range IQ rate.\n";
        case MYKONOS_ERR_RXPROFILE_RFBW:
            return "Receiver profile out of range RF bandwidth.\n";
        case MYKONOS_ERR_RXPROFILE_FILTER_DECIMATION:
            return "Receiver profile not valid filter decimation setting.\n";
        case MYKONOS_ERR_RXPROFILE_FIR_COEFS:
            return "Receiver profile FIR filter not valid.\n";
        case MYKONOS_ERR_RXPROFILE_ADCDIV:
            return "Receiver profile not valid ADC divider.\n";
        case MYKONOS_ERR_PROFILES_HSDIGCLK:
            return "Profile combinations loaded are not valid.\n";
        case MYKONOS_ERR_RESET_TXLOL_INV_PARAM:
            return "Selected channel is not valid.\n";
        case MYKONOS_ERR_RESET_TXLOL_ARMERROR:
            return "ARM command error in MYKONOS_resetExtTxLolChannel().\n";
        case MYKONOS_ERR_SETSTATEALL_TRACK_CAL_INV:
            return "Not valid calibration mask passed for trackCals.\n";
        case MYKONOS_ERR_SETSTATEALL_TRACK_ARMERRFLAG:
            return "ARM error flag set.\n";
        case MYKONOS_ERR_GETSTATEALL_TRACK_ARMERRFLAG:
            return "ARM error flag set.\n";
        case MYKONOS_ERR_GETSTATEALL_TRACK_ARMERROR:
            return "ARM command error.\n";
        case MYKONOS_ERR_GETSTATEALL_TRACK_NULL_PARAM:
            return "Null parameter passed for trackCals.\n";
        case MYKONOS_ERR_SETSTATE_TRACK_CAL_INV:
            return "Not valid calibration passed.\n";
        case MYKONOS_ERR_SETSTATE_TRACK_ARMERRFLAG:
            return "ARM command error.\n";
        case MYKONOS_ERR_GETSTATE_TRACK_NULL_PARAM:
            return "Null parameter passed to trackCalState.\n";
        case MYKONOS_ERR_GETSTATE_TRACK_ARMERRFLAG:
            return "ARM command error flag set.\n";
        case MYKONOS_ERR_GETSTATE_TRACK_ARMERROR:
            return "ARM command error.\n";
        case MYKONOS_ERR_SETCLGCGAIN_INV_TXCHANNEL:
            return "Tx channel is not valid (Valid ENUM values: TX1 or TX2 only).\n";
        case MYKONOS_ERR_SETCLGCGAIN_TRACK_ARMERRFLAG:
            return "ARM command flag error set.\n";
        case MYKONOS_ERR_SETCLGCGAIN_INV_DESIREDGAIN:
            return "CLGC gain parameter is out of range, valid range is from -10000 to 10000.\n";
        case MYKONOS_ERR_SETDPDACT_INV_TXCHANNEL:
            return "Tx channel is not valid (Valid ENUM values: TX1, TX2 or TX1_TX2).\n";
        case MYKONOS_ERR_SETDPDACT_INV_STATE:
            return "Invalid Actuator state, valid states are 0-disable and 1-enable.\n";
        case MYKONOS_ERR_SETDPDACT_ARMERRFLAG:
            return "ARM command flag error set.\n";
        default:
            return "";
    }

#endif
}

/**
 * \brief Calculates the scaled device clock frequency at the input of the
 *        CLKPLL or RFPLL
 *
 * Use this helper function any time the scaled device clock frequency is needed.
 *
 * <B>Dependencies</B>
 * - device->clocks->deviceClock_kHz
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is structure pointer to the Mykonos data structure containing settings
 * \param scaledRefClk_kHz Output: The returned scaled reference clock for the device's PLLs
 * \param deviceClkDiv Output: The device clock divider setting that gets set in the devices SPI register.
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_NULL_DEVICE_PARAM Function parameter device pointer is NULL
 * \retval MYKONOS_ERR_INV_SCALEDDEVCLK_PARAM The scaled PLL refclk frequency is out of range (40MHz to 80MHz)
 * \retval MYKONOS_ERR_CALCDEVCLK_NULLPARAM Function parameter scaledRefClk_kHz or deviceClkDiv has a NULL pointer
 */
static mykonosErr_t MYKONOS_calculateScaledDeviceClk_kHz(mykonosDevice_t *device, uint32_t *scaledRefClk_kHz, uint8_t *deviceClkDiv)
{
    uint32_t deviceClock_kHz = 0;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_calculateScaledDeviceClk_kHz()\n");
#endif

    if ((device == NULL) || (device->clocks == NULL))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, 0, MYKONOS_ERR_NULL_DEVICE_PARAM, getMykonosErrorMessage(MYKONOS_ERR_NULL_DEVICE_PARAM));
        return MYKONOS_ERR_NULL_DEVICE_PARAM;
    }

    deviceClock_kHz = device->clocks->deviceClock_kHz;

    if ((scaledRefClk_kHz == NULL) || (deviceClkDiv == NULL))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_CALCDEVCLK_NULLPARAM,
                getMykonosErrorMessage(MYKONOS_ERR_CALCDEVCLK_NULLPARAM));
        return MYKONOS_ERR_CALCDEVCLK_NULLPARAM;
    }

    /* scaled Ref Clock at input to CLKPLL must be in range 40-80MHz. */
    if (deviceClock_kHz > 20000 && deviceClock_kHz <= 40000)
    {
        *scaledRefClk_kHz = deviceClock_kHz << 1;
        *deviceClkDiv = 3;
    } /* x2 */
    else if (deviceClock_kHz > 40000 && deviceClock_kHz <= 80000)
    {
        *scaledRefClk_kHz = deviceClock_kHz;
        *deviceClkDiv = 0;
    } /* x1 */
    else if (deviceClock_kHz > 80000 && deviceClock_kHz <= 160000)
    {
        *scaledRefClk_kHz = deviceClock_kHz >> 1;
        *deviceClkDiv = 1;
    } /* div2 */
    else if (deviceClock_kHz > 160000 && deviceClock_kHz <= 320000)
    {
        *scaledRefClk_kHz = deviceClock_kHz >> 2;
        *deviceClkDiv = 2;
    } /* div4 */
    else
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_SCALEDDEVCLK_PARAM,
                getMykonosErrorMessage(MYKONOS_ERR_INV_SCALEDDEVCLK_PARAM));
        return MYKONOS_ERR_INV_SCALEDDEVCLK_PARAM;
    }
    return MYKONOS_ERR_OK;
}

/**
 * \brief Resets the JESD204B Deframer
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 *
 * \param device is a pointer to the device settings structure
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 */
mykonosErr_t MYKONOS_resetDeframer(mykonosDevice_t *device)
{

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_resetDeframer()\n");
#endif

    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_DEFRAMER_RESET, 0x03);
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_DEFRAMER_RESET, 0x00);

    return MYKONOS_ERR_OK;
}

/**
 * \brief Sets up the JESD204B Serializers
 *
 * This function uses the Rx framer and ObsRx framer structures to setup the 4
 * serializer lanes that are shared between the two framers.  If the Rx profile is valid
 * the serializer amplitude and preEmphasis are used from the Rx framer.  If only the ObsRx
 * profile is valid, the obsRx framer settings are used.
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 * - device->spiSettings->chipSelectIndex
 * - device->rx->framer->M
 * - device->rx->framer->serializerAmplitude
 * - device->rx->framer->preEmphasis
 * - device->rx->framer->serializerLanesEnabled
 * - device->rx->framer->invertLanePolarity
 * - device->obsRx->framer->serializerLanesEnabled
 * - device->obsRx->framer->invertLanePolarity
 * - device->obsRx->framer->M
 * - device->obsRx->framer->serializerLanesEnabled
 * - device->obsRx->framer->serializerAmplitude
 * - device->obsRx->framer->preEmphasis
 *
 * \param device Pointer to the device settings structure
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_INITSER_INV_VCODIV_PARM Mykonos CLKPLL has invalid VCO divider, verify CLKPLL config
 * \retval MYKONOS_ERR_SER_LANE_CONFLICT_PARM When both Rx and ObsRx framers are enabled, framers can not share the same physical lane.
 * \retval MYKONOS_ERR_SER_INV_REAL_IF_DATA_PARM Rx Framer M can only = 1 when Real IF mode is enabled
 * \retval MYKONOS_ERR_SER_INV_M_PARM Invalid Rx Framer M (valid 1,2,4)
 * \retval MYKONOS_ERR_SER_INV_LANEEN_PARM Invalid Rx framer serializerLanesEnabled (valid 0-15)
 * \retval MYKONOS_ERR_SER_INV_AMP_PARM Invalid Rx serializer amplitude (valid 0-31)
 * \retval MYKONOS_ERR_SER_INV_PREEMP_PARM Invalid Rx serializer pre-emphesis (valid 0-7)
 * \retval MYKONOS_ERR_SER_INV_LANEPN_PARM Invalid Rx serializer PN invert setting (valid 0-15)
 * \retval MYKONOS_ERR_SER_INV_L_PARM Invalid Rx serializer lanes enabled (must use 1, 2 or 4 lanes)
 * \retval MYKONOS_ERR_SER_INV_LANERATE_PARM Invalid Rx serializer lane rate (valid 614.4Mbps - 6144Mbps)
 *
 * \retval MYKONOS_ERR_SER_INV_REAL_IF_DATA_PARM
 * \retval MYKONOS_ERR_SER_INV_LANEEN_PARM
 * \retval MYKONOS_ERR_SER_INV_AMP_PARM
 * \retval MYKONOS_ERR_SER_INV_PREEMP_PARM
 * \retval MYKONOS_ERR_SER_INV_LANEPN_PARM
 * \retval MYKONOS_ERR_SER_INV_L_PARM
 * \retval MYKONOS_ERR_SER_INV_LANERATE_PARM
 *
 * \retval MYKONOS_ERR_SER_LANE_RATE_CONFLICT_PARM Necessary lane rates for Rx and ObsRx framer can not be obtained with possible divider settings
 * \retval MYKONOS_ERR_SER_INV_HSCLK_PARM Invalid HSCLK frequency (check CLKPLL config, HSCLK must be <= 6144GHz)
 * \retval MYKONOS_ERR_HS_AND_LANE_RATE_NOT_INTEGER_MULT HSCLK is not an integer multiple of the lane clock rate
 * \retval MYKONOS_ERR_SER_INV_TXSER_DIV_PARM No valid Tx serializer divider to obtain desired lane rates
 * \retval MYKONOS_ERR_INITSER_INV_PROFILE Rx/ObsRx and sniffer profiles are not valid - can not config serializers
 * \retval MYKONOS_ERR_INV_RXFRAMER_PCLKDIV_PARM Invalid Rx framer PCLK divider
 * \retval MYKONOS_ERR_INV_OBSRXFRAMER_PCLKDIV_PARM Invalid ORx/Sniffer framer PCLK divider
 *
 */
mykonosErr_t MYKONOS_setupSerializers(mykonosDevice_t *device)
{
    uint8_t div2 = 0;
    uint8_t txser_div = 0;
    uint8_t txser_div_reg = 0;
    uint32_t clkPllVcoFrequency_kHz = 0;
    uint32_t hsclkRate_kHz = 0;
    uint32_t rxLaneRate_kHz = 0;
    uint32_t obsRxLaneRate_kHz = 0;
    uint32_t fasterLaneRate_kHz = 0;
    uint32_t slowerLaneRate_kHz = 0;
    uint8_t modHsLaneRate = 0;
    uint8_t rxL = 0;
    uint8_t obsRxL = 0;
    uint8_t i = 0;
    uint8_t amplitudeEmphasis = 0;
    uint8_t lanePowerDown = 0x00;
    mykonosVcoDiv_t vcoDiv = VCODIV_1;
    uint8_t rxLanePn = 0;
    uint8_t obsRxLanePn = 0;

    uint32_t hsDigClkdiv4_5_kHz = 0;
    uint32_t rxPclkDiv = 0;
    uint32_t obsRxPclkDiv = 0;
    uint32_t tempPclkDiv = 0;
    uint32_t rxFramerPclk_kHz = 0;
    uint32_t obsRxFramerPclk_kHz = 0;
    uint8_t rxSyncbSelect = 0;
    uint8_t obsRxSyncbSelect = 0;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_setupSerializers()\n");
#endif

    vcoDiv = device->clocks->clkPllVcoDiv;
    clkPllVcoFrequency_kHz = device->clocks->clkPllVcoFreq_kHz;

    switch (vcoDiv)
    {
        case VCODIV_1:
            hsclkRate_kHz = clkPllVcoFrequency_kHz;
            break;
        case VCODIV_1p5:
            hsclkRate_kHz = (clkPllVcoFrequency_kHz / 15) * 10;
            break;
        case VCODIV_2:
            hsclkRate_kHz = clkPllVcoFrequency_kHz >> 1;
            break;
        case VCODIV_3:
            hsclkRate_kHz = (clkPllVcoFrequency_kHz / 30) * 10;
            break;
        default:
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INITSER_INV_VCODIV_PARM,
                    getMykonosErrorMessage(MYKONOS_ERR_INITSER_INV_VCODIV_PARM));
            return MYKONOS_ERR_INITSER_INV_VCODIV_PARM;
        }
    }

    if ((device->profilesValid & RX_PROFILE_VALID) && (device->profilesValid & (ORX_PROFILE_VALID | SNIFF_PROFILE_VALID)))
    {
        if (device->rx->framer->serializerLanesEnabled & device->obsRx->framer->serializerLanesEnabled)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SER_LANE_CONFLICT_PARM,
                    getMykonosErrorMessage(MYKONOS_ERR_SER_LANE_CONFLICT_PARM));
            return MYKONOS_ERR_SER_LANE_CONFLICT_PARM;
        }
    }

    lanePowerDown = 0x9F;
    if (device->profilesValid & RX_PROFILE_VALID)
    {
        if (device->rx->framer->M == 1 && !(device->rx->realIfData))
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SER_INV_REAL_IF_DATA_PARM,
                    getMykonosErrorMessage(MYKONOS_ERR_SER_INV_REAL_IF_DATA_PARM));
            return MYKONOS_ERR_SER_INV_REAL_IF_DATA_PARM;
        }

        if (device->rx->framer->M != 1 && device->rx->framer->M != 2 && device->rx->framer->M != 4)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SER_INV_M_PARM, getMykonosErrorMessage(MYKONOS_ERR_SER_INV_M_PARM));
            return MYKONOS_ERR_SER_INV_M_PARM;
        }

        for (i = 0; i < 4; i++)
        {
            rxL += ((device->rx->framer->serializerLanesEnabled >> i) & 0x01);
        }

        if (device->rx->framer->serializerLanesEnabled > 15)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SER_INV_LANEEN_PARM,
                    getMykonosErrorMessage(MYKONOS_ERR_SER_INV_LANEEN_PARM));
            return MYKONOS_ERR_SER_INV_LANEEN_PARM;
        }

        if (device->rx->framer->serializerAmplitude > 31)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SER_INV_AMP_PARM, getMykonosErrorMessage(MYKONOS_ERR_SER_INV_AMP_PARM));
            return MYKONOS_ERR_SER_INV_AMP_PARM;
        }

        if (device->rx->framer->preEmphasis > 7)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SER_INV_PREEMP_PARM,
                    getMykonosErrorMessage(MYKONOS_ERR_SER_INV_PREEMP_PARM));
            return MYKONOS_ERR_SER_INV_PREEMP_PARM;
        }

        if (device->rx->framer->invertLanePolarity > 15)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SER_INV_LANEPN_PARM,
                    getMykonosErrorMessage(MYKONOS_ERR_SER_INV_LANEPN_PARM));
            return MYKONOS_ERR_SER_INV_LANEPN_PARM;
        }
        if ((rxL != 1) && (rxL != 2) && (rxL != 4))
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SER_INV_L_PARM, getMykonosErrorMessage(MYKONOS_ERR_SER_INV_L_PARM));
            return MYKONOS_ERR_SER_INV_L_PARM;
        }

        rxLaneRate_kHz = device->rx->rxProfile->iqRate_kHz * 20 * device->rx->framer->M / rxL;
        lanePowerDown &= (~(device->rx->framer->serializerLanesEnabled));

        if ((rxLaneRate_kHz < 614400) || (rxLaneRate_kHz > 6144000))
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SER_INV_LANERATE_PARM,
                    getMykonosErrorMessage(MYKONOS_ERR_SER_INV_LANERATE_PARM));
            return MYKONOS_ERR_SER_INV_LANERATE_PARM;
        }
    }

    if (device->obsRx->framer > 0 && (device->profilesValid & (ORX_PROFILE_VALID | SNIFF_PROFILE_VALID)))
    {
        if (device->obsRx->framer->M == 1 && !(device->obsRx->realIfData))
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SER_INV_REAL_IF_DATA_PARM,
                    getMykonosErrorMessage(MYKONOS_ERR_SER_INV_REAL_IF_DATA_PARM));
            return MYKONOS_ERR_SER_INV_REAL_IF_DATA_PARM;
        }

        for (i = 0; i < 4; i++)
        {
            obsRxL += ((device->obsRx->framer->serializerLanesEnabled >> i) & 0x01);
        }

        if (device->obsRx->framer->serializerLanesEnabled > 15)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SER_INV_LANEEN_PARM,
                    getMykonosErrorMessage(MYKONOS_ERR_SER_INV_LANEEN_PARM));
            return MYKONOS_ERR_SER_INV_LANEEN_PARM;
        }

        if (device->obsRx->framer->serializerAmplitude > 31)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SER_INV_AMP_PARM, getMykonosErrorMessage(MYKONOS_ERR_SER_INV_AMP_PARM));
            return MYKONOS_ERR_SER_INV_AMP_PARM;
        }

        if (device->obsRx->framer->preEmphasis > 7)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SER_INV_PREEMP_PARM,
                    getMykonosErrorMessage(MYKONOS_ERR_SER_INV_PREEMP_PARM));
            return MYKONOS_ERR_SER_INV_PREEMP_PARM;
        }

        if (device->obsRx->framer->invertLanePolarity > 15)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SER_INV_LANEPN_PARM,
                    getMykonosErrorMessage(MYKONOS_ERR_SER_INV_LANEPN_PARM));
            return MYKONOS_ERR_SER_INV_LANEPN_PARM;
        }

        if ((obsRxL != 0) && (obsRxL != 1) && (obsRxL != 2) && (obsRxL != 4))
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SER_INV_L_PARM, getMykonosErrorMessage(MYKONOS_ERR_SER_INV_L_PARM));
            return MYKONOS_ERR_SER_INV_L_PARM;
        }

        /* check for valid pointer */
        if (obsRxL == 0)
        {
            /* Allow ORx receiver to work but disable Obs Rx framer link - valid on 9373 */
            obsRxLaneRate_kHz = 0;
        }
        else if (device->profilesValid & ORX_PROFILE_VALID)
        {
            obsRxLaneRate_kHz = device->obsRx->orxProfile->iqRate_kHz * 20 * device->obsRx->framer->M / obsRxL;
        }
        else if (device->profilesValid & SNIFF_PROFILE_VALID)
        {
            obsRxLaneRate_kHz = device->obsRx->snifferProfile->iqRate_kHz * 20 * device->obsRx->framer->M / obsRxL;
        }

        lanePowerDown &= (~(device->obsRx->framer->serializerLanesEnabled));

        if ((obsRxLaneRate_kHz != 0) && ((obsRxLaneRate_kHz < 614400) || (obsRxLaneRate_kHz > 6144000)))
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SER_INV_LANERATE_PARM,
                    getMykonosErrorMessage(MYKONOS_ERR_SER_INV_LANERATE_PARM));
            return MYKONOS_ERR_SER_INV_LANERATE_PARM;
        }
    }

    /* checking for RX and OBSRX effective lane rates match */
    /* Need to account for oversampling, verify that the 2 lane rate are integer related */

    if ((rxLaneRate_kHz > 0) && (obsRxLaneRate_kHz > 0))
    {
        fasterLaneRate_kHz = (rxLaneRate_kHz >= obsRxLaneRate_kHz) ? (rxLaneRate_kHz) : (obsRxLaneRate_kHz);
        slowerLaneRate_kHz = (rxLaneRate_kHz >= obsRxLaneRate_kHz) ? (obsRxLaneRate_kHz) : (rxLaneRate_kHz);

        /* Verify that lane rates are integer multiples of each other */
        if ((fasterLaneRate_kHz % slowerLaneRate_kHz) != 0)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SER_LANE_RATE_CONFLICT_PARM,
                    getMykonosErrorMessage(MYKONOS_ERR_SER_LANE_RATE_CONFLICT_PARM));
            return MYKONOS_ERR_SER_LANE_RATE_CONFLICT_PARM;
        }
    }
    else
    {
        fasterLaneRate_kHz = (rxLaneRate_kHz >= obsRxLaneRate_kHz) ? (rxLaneRate_kHz) : (obsRxLaneRate_kHz);
    }

    if (hsclkRate_kHz > 6144000)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SER_INV_HSCLK_PARM, getMykonosErrorMessage(MYKONOS_ERR_SER_INV_HSCLK_PARM));
        return MYKONOS_ERR_SER_INV_HSCLK_PARM;
    }

    /* performing integer multiple check on HS clock and lane rate clock */
    if (fasterLaneRate_kHz == 0)
    {
        /* All serializer lanes are disabled */
        txser_div = 1;
        div2 = 0;
    }
    else
    {
        modHsLaneRate = hsclkRate_kHz % fasterLaneRate_kHz;
        if (modHsLaneRate)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_HS_AND_LANE_RATE_NOT_INTEGER_MULT,
                    getMykonosErrorMessage(MYKONOS_ERR_HS_AND_LANE_RATE_NOT_INTEGER_MULT));
            return MYKONOS_ERR_HS_AND_LANE_RATE_NOT_INTEGER_MULT;
        }

        /* if lane rate = HSCLK /1, /2 or /4, the division is set in reg x11A */
        /* if lane rate = HSCLK /8, x11A is set for /4 and 0xB3 is set for /2 */

        txser_div = (uint8_t)(hsclkRate_kHz / fasterLaneRate_kHz);
        if (txser_div == 8)
        {
            div2 = 1;
        }
        else
        {
            div2 = 0;
        }
    }

    switch (txser_div)
    {
        case 1:
            txser_div_reg = 0;
            break;
        case 2:
            txser_div_reg = 1;
            break;
        case 4:
            txser_div_reg = 2;
            break;
        case 8:
            txser_div_reg = 2;
            break;
        default:
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SER_INV_TXSER_DIV_PARM,
                    getMykonosErrorMessage(MYKONOS_ERR_SER_INV_TXSER_DIV_PARM));
            return MYKONOS_ERR_SER_INV_TXSER_DIV_PARM;
            break;
    }

    if (device->profilesValid & RX_PROFILE_VALID)
    {
        amplitudeEmphasis = ((device->rx->framer->serializerAmplitude & 0x1f) << 3) | (device->rx->framer->preEmphasis & 0x07);
    }
    else if (device->profilesValid & (ORX_PROFILE_VALID | SNIFF_PROFILE_VALID))
    {
        amplitudeEmphasis = ((device->obsRx->framer->serializerAmplitude & 0x1f) << 3) | (device->obsRx->framer->preEmphasis & 0x07);
    }
    else
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INITSER_INV_PROFILE, getMykonosErrorMessage(MYKONOS_ERR_INITSER_INV_PROFILE));
        return MYKONOS_ERR_INITSER_INV_PROFILE;
    }

    if (device->profilesValid & RX_PROFILE_VALID)
    {
        rxSyncbSelect = (device->rx->framer->obsRxSyncbSelect > 0) ? 1 : 0;
        rxLanePn = (rxSyncbSelect << 7) | (device->rx->framer->invertLanePolarity & 0xF);
    }

    if (device->profilesValid & (ORX_PROFILE_VALID | SNIFF_PROFILE_VALID))
    {
        obsRxSyncbSelect = (device->obsRx->framer->obsRxSyncbSelect > 0) ? 1 : 0;
        obsRxLanePn = (obsRxSyncbSelect << 7) | (device->obsRx->framer->invertLanePolarity & 0xF);
    }

    /* Serializer lane clock frequency */
    /* serLaneClock_kHz = fasterLaneRate_kHz / txser_div / 20; */
    if (device->profilesValid & RX_PROFILE_VALID)
    {
        /* calculate manual PCLK frequency for Rx framer  = Rx IQrate * F, where F = (2*M/L)  or PCLK = lanerate /10 */
        if (device->rx->framer->overSample)
        {
            rxFramerPclk_kHz = fasterLaneRate_kHz / 10;
        }
        else
        {
            rxFramerPclk_kHz = rxLaneRate_kHz / 10;
        }

        hsDigClkdiv4_5_kHz = hsclkRate_kHz / device->clocks->clkPllHsDiv / device->rx->rxProfile->rxDec5Decimation;

        /* PCLKDiv: 0=4x, 1=2x, 2=1x, 3=/2, 4=/4, 5=/8, 6=/16 */
        if (hsDigClkdiv4_5_kHz == rxFramerPclk_kHz)
        {
            rxPclkDiv = 2;
        }
        else if (hsDigClkdiv4_5_kHz > rxFramerPclk_kHz)
        {
            tempPclkDiv = hsDigClkdiv4_5_kHz / rxFramerPclk_kHz;
            switch (tempPclkDiv)
            {
                case 2:
                    rxPclkDiv = 3;
                    break;
                case 4:
                    rxPclkDiv = 4;
                    break;
                case 8:
                    rxPclkDiv = 5;
                    break;
                case 16:
                    rxPclkDiv = 6;
                    break;
                default:
                {
                    CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_RXFRAMER_PCLKDIV_PARM,
                            getMykonosErrorMessage(MYKONOS_ERR_INV_RXFRAMER_PCLKDIV_PARM));
                    return MYKONOS_ERR_INV_RXFRAMER_PCLKDIV_PARM;
                }
            }
        }
        else if (hsDigClkdiv4_5_kHz < rxFramerPclk_kHz)
        {
            tempPclkDiv = rxFramerPclk_kHz / hsDigClkdiv4_5_kHz;
            switch (tempPclkDiv)
            {
                case 2:
                    rxPclkDiv = 1;
                    break;
                case 4:
                    rxPclkDiv = 0;
                    break;
                default:
                {
                    CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_RXFRAMER_PCLKDIV_PARM,
                            getMykonosErrorMessage(MYKONOS_ERR_INV_RXFRAMER_PCLKDIV_PARM));
                    return MYKONOS_ERR_INV_RXFRAMER_PCLKDIV_PARM;
                }
            }
        }
    }

    if (obsRxL == 0)
    {
        /* Make sure PCLK is set as low as possible because it interacts with Rx Framer logic, */
        /* even though ObsRx Framer disabled */
        obsRxPclkDiv = 6;
    }
    else if (device->profilesValid & (ORX_PROFILE_VALID | SNIFF_PROFILE_VALID))
    {
        /* calculate manual PCLK frequency for obsRx framer  = obsRx IQrate * F, where F = (2*M/L) */
        if (device->profilesValid & ORX_PROFILE_VALID)
        {
            if (device->obsRx->framer->overSample)
            {
                obsRxFramerPclk_kHz = fasterLaneRate_kHz / 10;
            }
            else
            {
                obsRxFramerPclk_kHz = obsRxLaneRate_kHz / 10;
            }

            hsDigClkdiv4_5_kHz = hsclkRate_kHz / device->clocks->clkPllHsDiv / device->obsRx->orxProfile->rxDec5Decimation;
        }
        else if (device->profilesValid & SNIFF_PROFILE_VALID)
        {
            if (device->obsRx->framer->overSample)
            {
                obsRxFramerPclk_kHz = fasterLaneRate_kHz / 10;
            }
            else
            {
                obsRxFramerPclk_kHz = obsRxLaneRate_kHz / 10;
            }

            hsDigClkdiv4_5_kHz = hsclkRate_kHz / device->clocks->clkPllHsDiv / device->obsRx->snifferProfile->rxDec5Decimation;
        }

        /* PCLKDiv: 0=4x, 1=2x, 2=1x, 3=/2, 4=/4, 5=/8, 6=/16 */
        if (hsDigClkdiv4_5_kHz == obsRxFramerPclk_kHz)
        {
            obsRxPclkDiv = 2;
        }
        else if ((hsDigClkdiv4_5_kHz > obsRxFramerPclk_kHz) && (obsRxFramerPclk_kHz != 0))
        {
            tempPclkDiv = hsDigClkdiv4_5_kHz / obsRxFramerPclk_kHz;
            switch (tempPclkDiv)
            {
                case 2:
                    obsRxPclkDiv = 3;
                    break;
                case 4:
                    obsRxPclkDiv = 4;
                    break;
                case 8:
                    obsRxPclkDiv = 5;
                    break;
                case 16:
                    obsRxPclkDiv = 6;
                    break;
                default:
                {
                    CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_OBSRXFRAMER_PCLKDIV_PARM,
                            getMykonosErrorMessage(MYKONOS_ERR_INV_OBSRXFRAMER_PCLKDIV_PARM));
                    return MYKONOS_ERR_INV_OBSRXFRAMER_PCLKDIV_PARM;
                }
            }
        }
        else if (hsDigClkdiv4_5_kHz < obsRxFramerPclk_kHz)
        {
            tempPclkDiv = obsRxFramerPclk_kHz / hsDigClkdiv4_5_kHz;
            switch (tempPclkDiv)
            {
                case 2:
                    obsRxPclkDiv = 1;
                    break;
                case 4:
                    obsRxPclkDiv = 0;
                    break;
                default:
                {
                    CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_OBSRXFRAMER_PCLKDIV_PARM,
                            getMykonosErrorMessage(MYKONOS_ERR_INV_OBSRXFRAMER_PCLKDIV_PARM));
                    return MYKONOS_ERR_INV_OBSRXFRAMER_PCLKDIV_PARM;
                }
            }
        }
    }

    /* set lane rate divide setting */
    CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_CLOCK_CONTROL_3, txser_div_reg, 0x03, 0);

    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_SERIALIZER_CTL_2, amplitudeEmphasis);

    /* Serializer: Release Reset */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_SERIALIZER_SPECIAL, 0x03);

    /* Serializer: txser clk enable */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_SERIALIZER_HS_DIV_TXSER_CLK_EN, 0x01);

    /* Serializer: Enable 2 to 1 serializer */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_SERIALIZER_CTL_3, 0x20 | div2);

    /* power up desired serializer lanes */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_SERIALIZER_CTL_1, lanePowerDown);

    /* apply desired lane PN inversions to the TX and OBSRX framers */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_FRAMER_LANE_DATA_CTL, rxLanePn);
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_OBS_FRAMER_LANE_DATA_CTL, obsRxLanePn);

    /* set Rx framer manual PCLK frequency based on lane rate of the serializers */
    CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_FRAMER_CLK_EN, rxPclkDiv, 0x70, 4);

    /* enable bit repeat mode in Rx framer - since manual PCLK, oversample will still work */
    CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_FRAMER_CLK_EN, 0, 0x80, 7);

    /* enable manual PCLK mode in Rx framer */
    CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_FRAMER_CLK_EN, 1, 0x04, 2);

    /* set obsRx framer manual PCLK frequency based on lane rate of the serializers */
    CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_OBS_FRAMER_CLK_EN, obsRxPclkDiv, 0x70, 4);

    /* enable bit repeat mode in ObsRx framer - since manual PCLK, oversample will still work */
    CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_OBS_FRAMER_CLK_EN, 0, 0x80, 7);

    /* enable manual PCLK mode in ObsRx framer */
    CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_OBS_FRAMER_CLK_EN, 1, 0x04, 2);

    return MYKONOS_ERR_OK;
}

/**
 * \brief Sets up the JESD204B Deserializers
 *
 * This function enables the necessary deserializer lanes, sets the deserializer clocks
 * PN inversion settings, and EQ settings based on the info found in the device data structure.
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 * - device->spiSettings->chipSelectIndex
 * - device->tx->txProfile->clkPllVcoDiv
 * - device->tx->txProfile->vcoFreq_kHz
 * - device->tx->txProfile->txIqRate_kHz
 * - device->tx->deframer->M
 * - device->tx->deframer->deserializerLanesEnabled
 * - device->tx->deframer->invertLanePolarity
 * - device->tx->deframer->EQSetting
 *
 * \param device Pointer to the device settings structure

 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_INITDES_INV_TXPROFILE Tx Profile is not valid in data structure - can not setup deserializer
 * \retval MYKONOS_ERR_INITDES_INV_VCODIV_PARM Mykonos CLKPLL VCO divider is invalid
 * \retval MYKONOS_ERR_DESER_INV_M_PARM Invalid M (valid 2 or 4)
 * \retval MYKONOS_ERR_DESER_INV_L_PARM Invalid L (valid 1,2,4)
 * \retval MYKONOS_ERR_DESER_INV_HSCLK_PARM Invalid HSCLK, must be 6.144G or less after CLKPLL VCO divider - verify CLKPLL config
 * \retval MYKONOS_ERR_DESER_INV_LANERATE_PARM Invalid lanerate, must be between 614.4 Mbps to 6144 Mbps
 * \retval MYKONOS_ERR_DESER_INV_LANEEN_PARM Invalid deserializerLanesEnabled (valid 0-15 in 1,2,4 lane combinations)
 * \retval MYKONOS_ERR_DESER_INV_EQ_PARM Invalid EQ parameter (valid 0-4)
 * \retval MYKONOS_ERR_DESER_INV_LANEPN_PARM Invalid PN invert setting, (valid 0-15, invert bit per lane)
 * \retval MYKONOS_ERR_DES_HS_AND_LANE_RATE_NOT_INTEGER_MULT Invalid clock settings, HSCLK is not an integer multiple of lane rate
 *
 */
mykonosErr_t MYKONOS_setupDeserializers(mykonosDevice_t *device)
{
    uint8_t lanePowerDown = 0xF;
    uint8_t des_div = 0;
    uint8_t des_div_bitfield = 0;
    uint8_t rxcdr_div = 0;
    uint8_t rxcdr_div_bitfield = 0;
    uint32_t hsclkRate_kHz = 0;
    uint8_t i = 0;
    uint8_t L = 0;
    uint32_t laneRate_kHz = 0;
    uint8_t invertLanePolarity = 0;
    uint8_t modHsLaneRate = 0;

    const uint32_t MAX_HSCLKRATE_KHZ = 12288000;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_setupDeserializers()\n");
#endif

    if ((device->profilesValid & TX_PROFILE_VALID) == 0)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INITDES_INV_TXPROFILE,
                getMykonosErrorMessage(MYKONOS_ERR_INITDES_INV_TXPROFILE));
        return MYKONOS_ERR_INITDES_INV_TXPROFILE;
    }

    if (device->tx->deframer->M != 2 && device->tx->deframer->M != 4)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DESER_INV_M_PARM, getMykonosErrorMessage(MYKONOS_ERR_DESER_INV_M_PARM));
        return MYKONOS_ERR_DESER_INV_M_PARM;
    }

    for (i = 0; i < 4; i++)
    {
        L += ((device->tx->deframer->deserializerLanesEnabled >> i) & 0x01);
    }

    if ((L == 0) || (L == 3) || (L > 4))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DESER_INV_L_PARM, getMykonosErrorMessage(MYKONOS_ERR_DESER_INV_L_PARM));
        return MYKONOS_ERR_DESER_INV_L_PARM;
    }

    //laneRate_kHz calculation and checks
    laneRate_kHz = device->tx->txProfile->iqRate_kHz * 20 * device->tx->deframer->M / L;

    if ((laneRate_kHz < 614400) || (laneRate_kHz > 6144000))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DESER_INV_LANERATE_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_DESER_INV_LANERATE_PARM));
        return MYKONOS_ERR_DESER_INV_LANERATE_PARM;
    }

    if (device->tx->deframer->deserializerLanesEnabled > 15)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DESER_INV_LANEEN_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_DESER_INV_LANEEN_PARM));
        return MYKONOS_ERR_DESER_INV_LANEEN_PARM;
    }

    if ((device->tx->deframer->deserializerLanesEnabled == 0x7) || (device->tx->deframer->deserializerLanesEnabled == 0xB)
            || (device->tx->deframer->deserializerLanesEnabled == 0xD) || (device->tx->deframer->deserializerLanesEnabled == 0xE))
    {
        /* 3 lanes not valid, only 1,2, or 4 lanes */
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DESER_INV_LANEEN_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_DESER_INV_LANEEN_PARM));
        return MYKONOS_ERR_DESER_INV_LANEEN_PARM;
    }

    if (device->tx->deframer->EQSetting > 4)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DESER_INV_EQ_PARM, getMykonosErrorMessage(MYKONOS_ERR_DESER_INV_EQ_PARM));
        return MYKONOS_ERR_DESER_INV_EQ_PARM;
    }

    if (device->tx->deframer->invertLanePolarity > 15)
    {
        /* only lower 4 bits of parameter are valid */
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DESER_INV_LANEPN_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_DESER_INV_LANEPN_PARM));
        return MYKONOS_ERR_DESER_INV_LANEPN_PARM;
    }

    //hsclkRate_kHz calculations
    switch (device->clocks->clkPllVcoDiv)
    {
        case VCODIV_1:
            hsclkRate_kHz = device->clocks->clkPllVcoFreq_kHz;
            break;
        case VCODIV_1p5:
            hsclkRate_kHz = (device->clocks->clkPllVcoFreq_kHz / 15) * 10;
            break;
        case VCODIV_2:
            hsclkRate_kHz = device->clocks->clkPllVcoFreq_kHz >> 1;
            break;
        case VCODIV_3:
            hsclkRate_kHz = (device->clocks->clkPllVcoFreq_kHz / 30) * 10;
            break;
        default:
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INITDES_INV_VCODIV_PARM,
                    getMykonosErrorMessage(MYKONOS_ERR_INITDES_INV_VCODIV_PARM));
            return MYKONOS_ERR_INITDES_INV_VCODIV_PARM;
    }

    if (hsclkRate_kHz > MAX_HSCLKRATE_KHZ)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DESER_INV_HSCLK_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_DESER_INV_HSCLK_PARM));
        return MYKONOS_ERR_DESER_INV_HSCLK_PARM;
    }

    /* The deserializer clock needs to be in the range 3.8GHz to 6.144 GHz in reg x107.  The rest of the division is in xDD */
    if (hsclkRate_kHz <= 6144000)
    {
        rxcdr_div = 1;
    }
    else if (hsclkRate_kHz > 6144000 && hsclkRate_kHz <= MAX_HSCLKRATE_KHZ)
    {
        rxcdr_div = 2;
    }
    else if (hsclkRate_kHz > MAX_HSCLKRATE_KHZ)
    {
        rxcdr_div = 4;
    }

    switch (rxcdr_div)
    {
        case 1:
            rxcdr_div_bitfield = 0;
            break;
        case 2:
            rxcdr_div_bitfield = 1;
            break;
        case 4:
            rxcdr_div_bitfield = 2;
            break;
        default:
            rxcdr_div_bitfield = 0;
            break;
    }

    /* performing integer multiple check on HS clock and lane rate clock */
    modHsLaneRate = hsclkRate_kHz % laneRate_kHz;
    if (modHsLaneRate)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DES_HS_AND_LANE_RATE_NOT_INTEGER_MULT,
                getMykonosErrorMessage(MYKONOS_ERR_DES_HS_AND_LANE_RATE_NOT_INTEGER_MULT));
        return MYKONOS_ERR_DES_HS_AND_LANE_RATE_NOT_INTEGER_MULT;
    }

    des_div = (uint8_t)(hsclkRate_kHz / laneRate_kHz / rxcdr_div);

    switch (des_div)
    {
        case 1:
            des_div_bitfield = 0;
            break;
        case 2:
            des_div_bitfield = 1;
            break;
        case 4:
            des_div_bitfield = 2;
            break;
        case 8:
            des_div_bitfield = 3;
            break;
        default:
            des_div_bitfield = 0;
            break;
    }

    CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_CLOCK_CONTROL_3, rxcdr_div_bitfield, 0x0C, 0x02);

    invertLanePolarity = (device->tx->deframer->invertLanePolarity & 0xF);

    lanePowerDown = (~(device->tx->deframer->deserializerLanesEnabled)) & 0xF;

    /* Deserializer: PLL clock enable */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_DESERIALIZER_HS_DIV_RXCDR_CLK_EN, 0x01);

    /* Deserializer: Set Pdet control */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_DESERIALIZER_PDET_CTL, 0x09);

    /* Deserializer: Set Power down control */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_DESERIALIZER_CTL_0, (0x80U | lanePowerDown | (des_div_bitfield << 4)));

    /* Deserializer: Set Sine Shape */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_DESERIALIZER_SIN_SHAPE_0, 0x00);

    /* Deserializer: Set ss gain and vga */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_DESERIALIZER_SIN_SHAPE_1, 0x00);

    /* Deserializer: Enable EQ, AC coupled JESD204B lanes */
    /* TODO: add ability to set this for AC coupling or DC coupling */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_DESERIALIZER_EQ_CTL_1, 0x07);

    /* Deserializer: Clear Data present at negedge, disable peak adjust */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_DESERIALIZER_MISC_CTL, 0x04);

    /* Deserializer: Set Equalizer for lanes 1 and 0 */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_DESERIALIZER_EQ_CTL_1_TO_0, ((device->tx->deframer->EQSetting << 3) | device->tx->deframer->EQSetting));

    /* Deserializer: Set Equalizer for lanes 2 and 3 */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_DESERIALIZER_EQ_CTL_3_TO_2, ((device->tx->deframer->EQSetting << 3) | device->tx->deframer->EQSetting));

    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_DEFRAMER_LANE_DATA_CTL, invertLanePolarity);

    /* Reset Deserializers - toggle resetb bit in [0] */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_DESERIALIZER_SPECIAL, 0x00);
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_DESERIALIZER_SPECIAL, 0x01);

    /* Start ALC cal */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_DESERIALIZER_CDR_CAL_CTL, 0x02);

    /* Deframer: Enable clocks and lane clocks */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_DEFRAMER_CLK_EN, 0x03);

    return MYKONOS_ERR_OK;
}

/**
 * \brief Sets up the JESD204B Framer
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 * - device->spiSettings->chipSelectIndex
 * - device->rx->framer->M
 * - device->rx->realIfData
 * - device->rx->framer->bankId
 * - device->rx->framer->lane0Id
 * - device->rx->framer->serializerLanesEnabled
 * - device->rx->framer->obsRxSyncbSelect
 * - device->rx->framer->K
 * - device->rx->framer->externalSysref
 * - device->rx->rxChannels
 * - device->rx->framer->newSysrefOnRelink
 * - device->rx->framer->enableAutoChanXbar
 * - device->rx->framer->lmfcOffset
 * - device->rx->framer->scramble
 *
 * \param device Pointer to the device settings structure
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_FRAMER_INV_REAL_IF_DATA_PARM Invalid framer M, M can only = 1 in real IF mode
 * \retval MYKONOS_ERR_FRAMER_INV_M_PARM Invalid framer M (valid 1,2,4)
 * \retval MYKONOS_ERR_FRAMER_INV_BANKID_PARM Invalid BankId (valid 0-15)
 * \retval MYKONOS_ERR_FRAMER_INV_LANEID_PARM Invalid Lane0Id (valid 0-31)
 * \retval MYKONOS_ERR_RXFRAMER_INV_FK_PARAM
 * \retval MYKONOS_ERR_FRAMER_INV_K_OFFSET_PARAM
 */
mykonosErr_t MYKONOS_setupJesd204bFramer(mykonosDevice_t *device)
{
    uint8_t i = 0;
    uint8_t fifoLaneEnable = 0;
    uint8_t L = 0;
    uint8_t ML = 0;
    uint8_t framerConfigF = 0;

    uint8_t subaddr[29] = {0}; /* holds address to framer sub register map */
    uint8_t subdata[29] = {0}; /* holds data for framer sub register map */

    /* Setup submap registers */
    uint8_t SUBCLASSV = 1; /* JESD subclass 1 */
    uint8_t JESDV = 1; /* Version: JESD204B */
    uint8_t DID = 0;
    uint8_t BID = 0;
    uint8_t LID0 = 0;
    uint8_t LID1 = 1;
    uint8_t LID2 = 2;
    uint8_t LID3 = 3;
    uint8_t CS = 0;
    uint8_t N = 0x0F;
    uint8_t Np = 0x0F;
    uint8_t S = 0;
    uint8_t CF = 0;
    uint8_t K = 31;
    uint8_t HD = 0;
    uint8_t FramerL = 1;
    uint8_t FramerM = 2;
    uint8_t FramerF = 1;
    uint8_t framerADC_XBar = 0xE4;
    uint16_t FK = 0;

    uint8_t regE0 = 0;
    uint8_t regE2 = 0;
    uint8_t framerLaneXbar = 0xE4;
    uint16_t CheckSum = 0;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_setupJesd204bFramer()\n");
#endif

    if (device->rx->framer->M == 1 && !(device->rx->realIfData))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_FRAMER_INV_REAL_IF_DATA_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_FRAMER_INV_REAL_IF_DATA_PARM));
        return MYKONOS_ERR_FRAMER_INV_REAL_IF_DATA_PARM;
    }

    if (device->rx->framer->M != 1 && device->rx->framer->M != 2 && device->rx->framer->M != 4)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_FRAMER_INV_M_PARM, getMykonosErrorMessage(MYKONOS_ERR_FRAMER_INV_M_PARM));
        return MYKONOS_ERR_FRAMER_INV_M_PARM;
    }

    if (device->rx->framer->bankId > 15)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_FRAMER_INV_BANKID_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_FRAMER_INV_BANKID_PARM));
        return MYKONOS_ERR_FRAMER_INV_BANKID_PARM;
    }

    /* no need to check deviceId, its range is full uint8_t range */
    if (device->rx->framer->lane0Id > 31)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_FRAMER_INV_LANEID_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_FRAMER_INV_LANEID_PARM));
        return MYKONOS_ERR_FRAMER_INV_LANEID_PARM;
    }

    //count number of lanes
    L = 0;
    ML = 0;

    for (i = 0; i < 4; i++)
    {
        L += ((device->rx->framer->serializerLanesEnabled >> i) & 0x01);
    }
    ML = device->rx->framer->M * 10 + L;

    FK = (uint16_t)device->rx->framer->K * 2 * device->rx->framer->M / L;

    if (FK < 20 || FK > 256 || FK % 4 != 0)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_RXFRAMER_INV_FK_PARAM,
                getMykonosErrorMessage(MYKONOS_ERR_RXFRAMER_INV_FK_PARAM));
        return MYKONOS_ERR_RXFRAMER_INV_FK_PARAM;
    }

    if (device->rx->framer->externalSysref == 0)
    {
        /* Framer: Generate SYSREF internally */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_FRAMER_TEST_CNTR_CTL, 0x20);
    }

    framerADC_XBar = 0xB1;
    if (ML == 24)
    {
        if (device->rx->rxChannels == RX1)
        {
            /* adc 0 and 2 used */
            framerADC_XBar = ((framerADC_XBar & 0x0C) << 2) | (framerADC_XBar & 0x03);
        }
        else if (device->rx->rxChannels == RX2)
        {
            /* swap ADC xbar for Rx1 and Rx2 */
            framerADC_XBar = ((framerADC_XBar & 0xF) << 4) | ((framerADC_XBar & 0xF0) >> 4);
            /* adc 0 and 2 used */
            framerADC_XBar = ((framerADC_XBar & 0x0C) << 2) | (framerADC_XBar & 0x03);
        }

        /* Framer: Set ADC Crossbar */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_FRAMER_ADC_XBAR_SEL, framerADC_XBar);
    }
    else
    {
        /* Framer: Set ADC Crossbar */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_FRAMER_ADC_XBAR_SEL, framerADC_XBar);
    }

    if (ML == 42)
    {
        /* uses framer outputs 0 and 2 instead of 0 and 1...fix framer lane XBar */
        /* only 2 lane cases here */
        switch ((device->rx->framer->serializerLanesEnabled & 0x0F))
        {
            case 3:
                framerLaneXbar = 0x08;
                break;
            case 5:
                framerLaneXbar = 0x20;
                break;
            case 6:
                framerLaneXbar = 0x20;
                break;
            case 9:
                framerLaneXbar = 0x80;
                break;
            case 10:
                framerLaneXbar = 0x80;
                break;
            case 12:
                framerLaneXbar = 0x80;
                break;
            default:
                /* default to valid setting for Lane 0 */
                framerLaneXbar = 0x08;
                break;
        }
    }
    else
    {
        switch ((device->rx->framer->serializerLanesEnabled & 0x0F))
        {
            /* all 4 lanes get framer 0 output */
            case 1:
                framerLaneXbar = 0x00;
                break;
            case 2:
                framerLaneXbar = 0x00;
                break;
            case 3:
                framerLaneXbar = 0x04;
                break;
            case 4:
                framerLaneXbar = 0x00;
                break;
            case 5:
                framerLaneXbar = 0x10;
                break;
            case 6:
                framerLaneXbar = 0x10;
                break;
            case 8:
                framerLaneXbar = 0x00;
                break;
            case 9:
                framerLaneXbar = 0x40;
                break;
            case 10:
                framerLaneXbar = 0x40;
                break;
            case 12:
                framerLaneXbar = 0x40;
                break;
            case 15:
                framerLaneXbar = 0xE4;
                break;
            default:
                /* default to valid setting for all Lanes */
                framerLaneXbar = 0xE4;
                break;
        }
    }
    /* Framer: Set Lane Crossbar */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_FRAMER_LANE_XBAR_SEL, framerLaneXbar);

    if (ML == 24)
    {
        /* Framer: Clear ADC Xbar channel auto switch */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_FRAMER_CONFIG_LOOPBACK_XBAR_REV, 0x00);
    }
    else
    {
        /* Framer: Set ADC Xbar channel auto switch */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_FRAMER_CONFIG_LOOPBACK_XBAR_REV, 0x04);
    }

    /* enabling the SYSREF for relink if newSysrefOnRelink is set */
    if (device->rx->framer->newSysrefOnRelink)
    {
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_FRAMER_SYSREF_FIFO_EN, 0x01, 0x40, 6);
    }

    /* enabling auto channel if the enableAutoChanXbar structure member is set */
    if (device->rx->framer->enableAutoChanXbar)
    {
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_FRAMER_CONFIG_LOOPBACK_XBAR_REV, 0x01, 0x04, 2);
    }

    /* determining F octets per frame based on 2*M/L */
    framerConfigF = 2 * (device->rx->framer->M / L);

    /* Framer: Soft Reset */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_FRAMER_RESET, 0x03);

    /* Framer: Clear Soft Reset */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_FRAMER_RESET, 0x00);

    /* Framer: Set F (Octets in Frame) */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_FRAMER_CONFIG_F, framerConfigF);

    /* Framer: Enable framer clock and lane clocks */
    CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_FRAMER_CLK_EN, 0x03, 0x03, 0);

    /* Set Framer lane FIFO enable for each lane enabled */
    fifoLaneEnable = (((device->rx->framer->serializerLanesEnabled & 0x0F) << 4) | 0x01);

    /* Framer: Enable Lane FIFOs */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_FRAMER_LANE_CTL, fifoLaneEnable);

    /* Setup submap registers */
    SUBCLASSV = 1; /* JESD subclass 1 */
    JESDV = 1; /* Version: JESD204B */
    DID = device->rx->framer->deviceId;
    BID = device->rx->framer->bankId;
    LID0 = device->rx->framer->lane0Id;
    LID1 = device->rx->framer->lane0Id + 1;
    LID2 = device->rx->framer->lane0Id + 2;
    LID3 = device->rx->framer->lane0Id + 3;

    CS = 0;
    N = 0x0F;
    Np = 0x0F;
    S = 0;
    CF = 0;
    HD = 0;
    K = device->rx->framer->K - 1;
    FramerL = L - 1;
    FramerM = device->rx->framer->M - 1;
    FramerF = (2 * device->rx->framer->M / L) - 1;

    if (ML == 11)
    {
        regE0 = 0x01;
        regE2 = 0x01;
    }
    else if (ML == 12)
    {
        regE0 = 0x01;
        regE2 = 0x03;
        HD = 0x01;
    }
    else if (ML == 21)
    {
        regE0 = 0x03;
        regE2 = 0x01;
    }
    else if (ML == 22)
    {
        regE0 = 0x03;
        regE2 = 0x03;
    }
    else if (ML == 24)
    {
        regE0 = 0x05;
        regE2 = 0x0F;
        HD = 0x01;
    }
    else if (ML == 41)
    {
        regE0 = 0x0F;
        regE2 = 0x01;
    }
    else if (ML == 42)
    {
        regE0 = 0x0F;
        regE2 = 0x05;
    }
    else if (ML == 44)
    {
        regE0 = 0x0F;
        regE2 = 0x0F;
    }
    else
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_FRAMER_INV_M_PARM, getMykonosErrorMessage(MYKONOS_ERR_FRAMER_INV_M_PARM));
        return MYKONOS_ERR_FRAMER_INV_M_PARM;
    }

    /* setting K offset for framer */
    if (device->rx->framer->lmfcOffset < device->rx->framer->K)
    {
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_FRAMER_LMFC_K_OFFSET, device->rx->framer->lmfcOffset);
    }
    else
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_FRAMER_INV_K_OFFSET_PARAM,
                getMykonosErrorMessage(MYKONOS_ERR_FRAMER_INV_K_OFFSET_PARAM));
        return MYKONOS_ERR_FRAMER_INV_K_OFFSET_PARAM;
    }

    subaddr[0] = 0x00;
    subdata[0] = (DID & 0xFF); /* Device ID */
    subaddr[1] = 0x01;
    subdata[1] = (BID & 0x0F); /* Bank ID */
    subaddr[2] = 0x02;
    subdata[2] = (LID0 & 0x1F); /* Lane 0 ID */
    subaddr[3] = 0x03;
    subdata[3] = (device->rx->framer->scramble << 7) | (FramerL & 0x1F); /* [7] Scramble, #Lanes[4:0]-1 */
    subaddr[4] = 0x04;
    subdata[4] = (FramerF & 0xFF); /* F[7:0]-1 */
    subaddr[5] = 0x05;
    subdata[5] = (K & 0x1F); /* K[4:0]-1 */
    subaddr[6] = 0x06;
    subdata[6] = (FramerM & 0xFF); /* M[7:0]-1 */
    subaddr[7] = 0x07;
    subdata[7] = ((CS & 0x3) << 6) | (N & 0x1F); /* [7:6] = CS[1:0], N[4:0]-1 */
    subaddr[8] = 0x08;
    subdata[8] = ((SUBCLASSV & 7) << 5) | (Np & 0x1F); /* NP[4:0] -1 */
    subaddr[9] = 0x09;
    subdata[9] = ((JESDV & 7) << 5) | (S & 0x1F); /* S[4:0]-1 */
    subaddr[10] = 0x0A;
    subdata[10] = ((HD & 1) << 7) | (CF & 0x1F); /* [7] = HD, CF[4:0] */
    subaddr[11] = 0x0B;
    subdata[11] = 0x00; /* reserved */
    subaddr[12] = 0x0C;
    subdata[12] = 0x00; /* reserved */

    CheckSum = (DID & 0xFF) + (BID & 0xF) + (LID0 & 0x1F) + (device->rx->framer->scramble) + (FramerL & 0x1F) + (FramerF & 0xFF) + (K & 0x1F) + (FramerM & 0xFF)
            + (CS & 0x3) + (N & 0x1F) + (Np & 0x1F) + (S & 0x1F) + (HD & 1) + (CF & 0x1F) + (SUBCLASSV & 7) + (JESDV & 7);

    /* Checksum Lane 0 */
    subaddr[13] = 0x0D;
    subdata[13] = CheckSum & 0xFF;

    /* in JESD204B ML=42 case, framer ip lanes 0 and 2 are used, write lane 1 id and checksum to ip lane 2 regs */
    if (ML == 42)
    {
        /* Lane 1 ID */
        subaddr[14] = 0x1A;
        subdata[14] = LID1;

        CheckSum = (DID & 0xFF) + (BID & 0xF) + (LID1 & 0x1F) + (device->rx->framer->scramble) + (FramerL & 0x1F) + (FramerF & 0xFF) + (K & 0x1F) + (FramerM & 0xFF)
                + (CS & 0x3) + (N & 0x1F) + (Np & 0x1F) + (S & 0x1F) + (HD & 1) + (CF & 0x1F) + (SUBCLASSV & 7) + (JESDV & 7);

        /* Checksum Lane 1 */
        subaddr[15] = 0x1D;
        subdata[15] = CheckSum & 0xFF;
    }
    else
    {
        /* Lane 1 ID */
        subaddr[14] = 0x12;
        subdata[14] = LID1;

        CheckSum = (DID & 0xFF) + (BID & 0xF) + (LID1 & 0x1F) + (device->rx->framer->scramble) + (FramerL & 0x1F) + (FramerF & 0xFF) + (K & 0x1F) + (FramerM & 0xFF)
                + (CS & 0x3) + (N & 0x1F) + (Np & 0x1F) + (S & 0x1F) + (HD & 1) + (CF & 0x1F) + (SUBCLASSV & 7) + (JESDV & 7);

        /* Checksum Lane 1 */
        subaddr[15] = 0x15;
        subdata[15] = CheckSum & 0xFF;

        /* Lane 2 ID */
        subaddr[16] = 0x1A;
        subdata[16] = LID2;

        CheckSum = (DID & 0xFF) + (BID & 0xF) + (LID2 & 0x1F) + (device->rx->framer->scramble) + (FramerL & 0x1F) + (FramerF & 0xFF) + (K & 0x1F) + (FramerM & 0xFF)
                + (CS & 0x3) + (N & 0x1F) + (Np & 0x1F) + (S & 0x1F) + (HD & 1) + (CF & 0x1F) + (SUBCLASSV & 7) + (JESDV & 7);

        /* Checksum Lane 2 */
        subaddr[17] = 0x1D;
        subdata[17] = CheckSum & 0xFF;

        /* Lane 3 ID */
        subaddr[18] = 0x22;
        subdata[18] = LID3;

        CheckSum = (DID & 0xFF) + (BID & 0xF) + (LID3 & 0x1F) + (device->rx->framer->scramble) + (FramerL & 0x1F) + (FramerF & 0xFF) + (K & 0x1F) + (FramerM & 0xFF)
                + (CS & 0x3) + (N & 0x1F) + (Np & 0x1F) + (S & 0x1F) + (HD & 1) + (CF & 0x1F) + (SUBCLASSV & 7) + (JESDV & 7);

        subaddr[19] = 0x25;
        subdata[19] = CheckSum & 0xFF;
    }

    subaddr[20] = 0xE0;
    subdata[20] = regE0; /* Enable converter n logic */
    subaddr[21] = 0xE2;
    subdata[21] = regE2; /* Enable Lane n logic */
    subaddr[22] = 0xE4;
    subdata[22] = 0x00;
    subaddr[23] = 0xE6;
    subdata[23] = 0x00;
    subaddr[24] = 0xF0;
    subdata[24] = 0x00; /* #multiframes in ILAS */
    subaddr[25] = 0xF2;
    subdata[25] = 0x00;
    subaddr[26] = 0xF3;
    subdata[26] = 0x00;
    subaddr[27] = 0xF4;
    subdata[27] = 0x00;
    subaddr[28] = 0xC0;
    subdata[28] = 0x03; /* Framer enable, both sides perform lane sync */

    for (i = 0; i <= 28; i++)
    {
        /* Set framer sub register map address */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_FRAMER_ADDR, subaddr[i]);

        /* Set framer sub register map data word */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_FRAMER_DATA, subdata[i]);

        /* Write enable */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_FRAMER_WRITE_EN, 0x01);
    }

    if (device->rx->framer->externalSysref > 0)
    {
        /* Framer: Enable lane FIFO sync (Enable CGS) */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_FRAMER_SYSREF_FIFO_EN, 0x30);
    }
    else
    {
        /* Framer: Enable lane FIFO sync (Enable CGS) */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_FRAMER_SYSREF_FIFO_EN, 0x10);
    }

    return MYKONOS_ERR_OK;
}

/**
 * \brief Sets up the JESD204B OBSRX Framer
 *
 * <B>Dependencies</B>
 * - device->rxChannels
 * - device->spiSettings->chipSelectIndex
 * - device->obsRx->framer->bankId
 * - device->obsRx->framer->M
 * - device->obsRx->framer->serializerLanesEnabled
 * - device->obsRx->framer->externalSysref
 * - device->spiSettings
 * - device->obsRx->framer->deviceId
 * - device->obsRx->framer->lane0Id
 * - device->obsRx->framer->K
 * - device->obsRx->framer->lmfcOffset
 * - device->obsRx->framer->scramble
 * - device->obsRx->framer->obsRxSyncbSelect
 *
 * \param device Pointer to the device settings structure
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_OBSRX_FRAMER_INV_REAL_IF_DATA_PARM M parameter can only be 1 when real IF data mode is enabled
 * \retval MYKONOS_ERR_OBSRX_FRAMER_INV_M_PARM ObsRx Framer M parameter can only be 1 or 2
 * \retval MYKONOS_ERR_OBSRX_FRAMER_INV_BANKID_PARM Invalid BankId (0-15)
 * \retval MYKONOS_ERR_OBSRX_FRAMER_INV_LANEID_PARM Invalid lane0Id (0-31)
 * \retval MYKONOS_ERR_OBSRXFRAMER_INV_FK_PARAM Invalid F*K value (F * K must be > 20 and divisible by 4)
 * \retval MYKONOS_ERR_OBSRX_FRAMER_INV_K_OFFSET_PARAM Invalid K offset, must be less than K
 */
mykonosErr_t MYKONOS_setupJesd204bObsRxFramer(mykonosDevice_t *device)
{
    uint8_t i = 0;
    uint8_t laneFifoEnable = 0;
    uint8_t L = 1;
    uint8_t ML = 2;
    uint8_t framerConfigF = 0;

    uint8_t subaddr[29] = {0}; /* holds address to framer sub register map */
    uint8_t subdata[29] = {0}; /* holds data for framer sub register map */

    /* Setup submap registers */
    uint8_t SUBCLASSV = 1; /* JESD subclass 1 */
    uint8_t JESDV = 1; /* Version: JESD204B */
    uint8_t DID = 0;
    uint8_t BID = 0;
    uint8_t LID0 = 0;
    uint8_t LID1 = 1;
    uint8_t LID2 = 2;
    uint8_t LID3 = 3;
    uint8_t CS = 0;
    uint8_t N = 0x0F;
    uint8_t Np = 0x0F;
    uint8_t S = 0;
    uint8_t CF = 0;
    uint8_t K = 31;
    uint8_t HD = 0;
    uint8_t FramerL = 1;
    uint8_t FramerM = 2;
    uint8_t FramerF = 1;
    uint8_t framerADC_XBar = 0xE4;
    uint8_t regE0 = 0;
    uint8_t regE2 = 0;
    uint8_t framerLaneXbar = 0xE4;
    uint16_t CheckSum = 0;
    uint16_t FK = 0;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_setupJesd204bObsRxFramer()\n");
#endif

    if (device->obsRx->framer->M == 1 && !(device->obsRx->realIfData))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OBSRX_FRAMER_INV_REAL_IF_DATA_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_OBSRX_FRAMER_INV_REAL_IF_DATA_PARM));
        return MYKONOS_ERR_OBSRX_FRAMER_INV_REAL_IF_DATA_PARM;
    }

    if (device->obsRx->framer->M != 1 && device->obsRx->framer->M != 2)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OBSRX_FRAMER_INV_M_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_OBSRX_FRAMER_INV_M_PARM));
        return MYKONOS_ERR_OBSRX_FRAMER_INV_M_PARM;
    }

    if (device->obsRx->framer->bankId > 15)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OBSRX_FRAMER_INV_BANKID_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_OBSRX_FRAMER_INV_BANKID_PARM));
        return MYKONOS_ERR_OBSRX_FRAMER_INV_BANKID_PARM;
    }

    /* no need to check deviceId, its range is full uint8_t range */
    if (device->obsRx->framer->lane0Id > 31)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OBSRX_FRAMER_INV_LANEID_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_OBSRX_FRAMER_INV_LANEID_PARM));
        return MYKONOS_ERR_OBSRX_FRAMER_INV_LANEID_PARM;
    }

    //count number of lanes
    L = 0;
    ML = 0;

    for (i = 0; i < 4; i++)
    {
        L += ((device->obsRx->framer->serializerLanesEnabled >> i) & 0x01);
    }

    ML = device->obsRx->framer->M * 10 + L;

    if (L == 0)
    {
        /* Disable framer and return successfully */
        /* Disable framer clock and lane clocks */
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_OBS_FRAMER_CLK_EN, 0, 0x03, 0);

        /* Disable lane FIFO enables */
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_OBS_FRAMER_LANE_CTL, 0, 0xF0, 4);
        return MYKONOS_ERR_OK;
    }

    FK = (uint16_t)device->obsRx->framer->K * 2 * device->obsRx->framer->M / L;
    if (FK < 20 || FK > 256 || FK % 4 != 0)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OBSRXFRAMER_INV_FK_PARAM,
                getMykonosErrorMessage(MYKONOS_ERR_OBSRXFRAMER_INV_FK_PARAM));
        return MYKONOS_ERR_OBSRXFRAMER_INV_FK_PARAM;
    }

    if (device->obsRx->framer->externalSysref == 0)
    {
        /* Framer: Generate SYSREF internally */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_OBS_FRAMER_TEST_CNTR_CTL, 0x20);
    }

    /* Framer: Set ADC Crossbar */
    framerADC_XBar = 0xB1;
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_OBS_FRAMER_ADC_XBAR_SEL, framerADC_XBar);

    if (ML == 42)
    {
        /* uses framer outputs 0 and 2 instead of 0 and 1...fix framer lane XBar */
        /* only 2 lane cases here */
        switch ((device->obsRx->framer->serializerLanesEnabled & 0x0F))
        {
            case 3:
                framerLaneXbar = 0x08;
                break;
            case 5:
                framerLaneXbar = 0x20;
                break;
            case 6:
                framerLaneXbar = 0x20;
                break;
            case 9:
                framerLaneXbar = 0x80;
                break;
            case 10:
                framerLaneXbar = 0x80;
                break;
            case 12:
                framerLaneXbar = 0x80;
                break;
            default:
                /* default to valid setting for Lane 0 */
                framerLaneXbar = 0x08;
                break;
        }
    }
    else
    {
        switch ((device->obsRx->framer->serializerLanesEnabled & 0x0F))
        {
            /* all 4 lanes get framer 0 output */
            case 1:
                framerLaneXbar = 0x00;
                break;
            case 2:
                framerLaneXbar = 0x00;
                break;
            case 3:
                framerLaneXbar = 0x04;
                break;
            case 4:
                framerLaneXbar = 0x00;
                break;
            case 5:
                framerLaneXbar = 0x10;
                break;
            case 6:
                framerLaneXbar = 0x10;
                break;
            case 8:
                framerLaneXbar = 0x00;
                break;
            case 9:
                framerLaneXbar = 0x40;
                break;
            case 10:
                framerLaneXbar = 0x40;
                break;
            case 12:
                framerLaneXbar = 0x40;
                break;
            case 15:
                framerLaneXbar = 0xE4;
                break;
            default:
                /* default to valid setting for all Lanes */
                framerLaneXbar = 0xE4;
                break;
        }
    }

    /* Framer: set Lane Crossbar */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_OBS_FRAMER_LANE_XBAR_SEL, framerLaneXbar);

    /* Framer: determine the framerConfigF value (2*M/L) */
    framerConfigF = 2 * (device->obsRx->framer->M / L);

    /* Framer: Soft Reset */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_OBS_FRAMER_RESET, 0x03);

    /* Framer: Clear Soft Reset */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_OBS_FRAMER_RESET, 0x00);

    /* Framer: Set F (Octets in Frame) */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_OBS_FRAMER_CONFIG_F, framerConfigF);

    /* Framer: Enable framer clock and lane clocks */
    CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_OBS_FRAMER_CLK_EN, 0x03, 0x03, 0);

    /* Set Framer lane FIFO enable for each lane enabled */
    laneFifoEnable = (((device->obsRx->framer->serializerLanesEnabled & 0x0F) << 4) | 0x01);

    /* Framer: Enable Lane FIFOs */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_OBS_FRAMER_LANE_CTL, laneFifoEnable);

    /* enabling the SYSREF for relink if newSysrefOnRelink is set */
    if (device->obsRx->framer->newSysrefOnRelink)
    {
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_OBS_FRAMER_SYSREF_FIFO_EN, 0x01, 0x40, 6);
    }

    /* enabling auto channel if the enableAutoChanXbar structure member is set */
    if (device->obsRx->framer->enableAutoChanXbar)
    {
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_OBS_FRAMER_CONFIG_LOOPBACK_XBAR_REV, 0x01, 0x04, 2);
    }

    /* Setup submap registers */
    SUBCLASSV = 1; /* JESD subclass 1 */
    JESDV = 1; /* Version: JESD204B */
    DID = device->obsRx->framer->deviceId;
    BID = device->obsRx->framer->bankId;
    LID0 = device->obsRx->framer->lane0Id;
    LID1 = device->obsRx->framer->lane0Id + 1;
    LID2 = device->obsRx->framer->lane0Id + 2;
    LID3 = device->obsRx->framer->lane0Id + 3;

    CS = 0;
    N = 0x0F;
    Np = 0x0F;
    S = 0;
    CF = 0;
    K = device->obsRx->framer->K - 1;
    HD = 0;
    FramerL = L - 1;
    FramerM = device->obsRx->framer->M - 1;
    FramerF = (2 * device->obsRx->framer->M / L) - 1;

    if (ML == 11)
    {
        regE0 = 0x1;
        regE2 = 0x1;
    }
    else if (ML == 12)
    {
        regE0 = 0x1;
        regE2 = 0x3;
        HD = 1;
    }
    else if (ML == 21)
    {
        regE0 = 0x3;
        regE2 = 0x1;
    }
    else if (ML == 22)
    {
        regE0 = 0x3;
        regE2 = 0x3;
    }
    else
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OBSRX_FRAMER_INV_M_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_OBSRX_FRAMER_INV_M_PARM));
        return MYKONOS_ERR_OBSRX_FRAMER_INV_M_PARM;
    }

    /* setting K offset for framer */
    if (device->obsRx->framer->lmfcOffset < device->rx->framer->K)
    {
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_OBS_FRAMER_LMFC_K_OFFSET, device->obsRx->framer->lmfcOffset);
    }
    else
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OBSRX_FRAMER_INV_K_OFFSET_PARAM,
                getMykonosErrorMessage(MYKONOS_ERR_OBSRX_FRAMER_INV_K_OFFSET_PARAM));
        return MYKONOS_ERR_OBSRX_FRAMER_INV_K_OFFSET_PARAM;
    }

    subaddr[0] = 0x00;
    subdata[0] = (DID & 0xFF); /* Device ID */
    subaddr[1] = 0x01;
    subdata[1] = (BID & 0x0F); /* Bank ID */
    subaddr[2] = 0x02;
    subdata[2] = (LID0 & 0x1F); /* Lane 0 ID */
    subaddr[3] = 0x03;
    subdata[3] = (device->obsRx->framer->scramble << 7) | (FramerL & 0x1F); /* [7] Scramble, #Lanes[4:0]-1 */
    subaddr[4] = 0x04;
    subdata[4] = (FramerF & 0xFF); /* F[7:0]-1 */
    subaddr[5] = 0x05;
    subdata[5] = (K & 0x1F); /* K[4:0]-1 */
    subaddr[6] = 0x06;
    subdata[6] = (FramerM & 0xFF); /* M[7:0]-1 */
    subaddr[7] = 0x07;
    subdata[7] = ((CS & 0x3) << 6) | (N & 0x1F); /* [7:6] = CS[1:0], N[4:0]-1 */
    subaddr[8] = 0x08;
    subdata[8] = ((SUBCLASSV & 7) << 5) | (Np & 0x1F); /* NP[4:0] -1 */
    subaddr[9] = 0x09;
    subdata[9] = ((JESDV & 7) << 5) | (S & 0x1F); /* S[4:0]-1 */
    subaddr[10] = 0x0a;
    subdata[10] = ((HD & 1) << 7) | (CF & 0x1F); /* [7] = HD, CF[4:0] */
    subaddr[11] = 0x0b;
    subdata[11] = 0x00; /* reserved */
    subaddr[12] = 0x0c;
    subdata[12] = 0x00; /* reserved */

    CheckSum = (DID & 0xFF) + (BID & 0xF) + (LID0 & 0x1F) + (device->obsRx->framer->scramble) + (FramerL & 0x1F) + (FramerF & 0xFF) + (K & 0x1F) + (FramerM & 0xFF)
            + (CS & 0x3) + (N & 0x1F) + (Np & 0x1F) + (S & 0x1F) + (HD & 1) + (CF & 0x1F) + (SUBCLASSV & 7) + (JESDV & 7);

    /* Checksum Lane 0 */
    subaddr[13] = 0x0d;
    subdata[13] = CheckSum & 0xFF;

    /* Lane 1 ID */
    subaddr[14] = 0x12;
    subdata[14] = LID1;

    CheckSum = (DID & 0xFF) + (BID & 0xF) + (LID1 & 0x1F) + (device->obsRx->framer->scramble) + (FramerL & 0x1F) + (FramerF & 0xFF) + (K & 0x1F) + (FramerM & 0xFF)
            + (CS & 0x3) + (N & 0x1F) + (Np & 0x1F) + (S & 0x1F) + (HD & 1) + (CF & 0x1F) + (SUBCLASSV & 7) + (JESDV & 7);

    /* Checksum Lane 1 */
    subaddr[15] = 0x15;
    subdata[15] = CheckSum & 0xFF;

    /* Lane 2 ID */
    subaddr[16] = 0x1a;
    subdata[16] = LID2;

    CheckSum = (DID & 0xFF) + (BID & 0xF) + (LID2 & 0x1F) + (device->obsRx->framer->scramble) + (FramerL & 0x1F) + (FramerF & 0xFF) + (K & 0x1F) + (FramerM & 0xFF)
            + (CS & 0x3) + (N & 0x1F) + (Np & 0x1F) + (S & 0x1F) + (HD & 1) + (CF & 0x1F) + (SUBCLASSV & 7) + (JESDV & 7);

    /* Checksum Lane 2 */
    subaddr[17] = 0x1d;
    subdata[17] = CheckSum & 0xFF;

    /* Lane 3 ID */
    subaddr[18] = 0x22;
    subdata[18] = LID3;

    CheckSum = (DID & 0xFF) + (BID & 0xF) + (LID3 & 0x1F) + (device->obsRx->framer->scramble) + (FramerL & 0x1F) + (FramerF & 0xFF) + (K & 0x1F) + (FramerM & 0xFF)
            + (CS & 0x3) + (N & 0x1F) + (Np & 0x1F) + (S & 0x1F) + (HD & 1) + (CF & 0x1F) + (SUBCLASSV & 7) + (JESDV & 7);

    subaddr[19] = 0x25;
    subdata[19] = CheckSum & 0xFF;

    subaddr[20] = 0xe0;
    subdata[20] = regE0; /* Enable converter n logic */
    subaddr[21] = 0xe2;
    subdata[21] = regE2; /* Enable Lane n logic */
    subaddr[22] = 0xe4;
    subdata[22] = 0x00;
    subaddr[23] = 0xe6;
    subdata[23] = 0x00;
    subaddr[24] = 0xf0;
    subdata[24] = 0x00; /* #multiframes in ILAS */
    subaddr[25] = 0xf2;
    subdata[25] = 0x00;
    subaddr[26] = 0xf3;
    subdata[26] = 0x00;
    subaddr[27] = 0xf4;
    subdata[27] = 0x00;
    subaddr[28] = 0xc0;
    subdata[28] = 0x03; /* Framer enable, both sides perform lane sync */

    for (i = 0; i <= 28; i++)
    {
        /* Set framer sub register map address */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_OBS_FRAMER_ADDR, subaddr[i]);

        /* Set framer sub register map data word */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_OBS_FRAMER_DATA, subdata[i]);

        /* Write enable */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_OBS_FRAMER_WRITE_EN, 0x01);
    }

    if (device->obsRx->framer->externalSysref > 0)
    {
        /* Framer: Enable lane FIFO sync (Enable CGS) */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_OBS_FRAMER_SYSREF_FIFO_EN, 0x30);
    }
    else
    {
        /* Framer: Enable lane FIFO sync (Enable CGS) */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_OBS_FRAMER_SYSREF_FIFO_EN, 0x10);
    }

    return MYKONOS_ERR_OK;
}

/**
 * \brief Enables/Disables the JESD204B Rx Framer
 *
 * This function is normally not necessary.  In the event that the link needs to be reset, this
 * function allows the Rx framer to be disabled and re-enabled.
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 * - device->rx->framer->serializerLanesEnabled
 *
 * \param device is a pointer to the device settings structure
 * \param enable 0 = Disable the selected framer, 1 = enable the selected framer link
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_ENFRAMERLINK_INV_LANESEN_PARM Invalid serializerLanesEnabled parameter in device data structure
 */
mykonosErr_t MYKONOS_enableRxFramerLink(mykonosDevice_t *device, uint8_t enable)
{
    uint8_t enableLink = 0;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_enableFramerLink()\n");
#endif

    enableLink = (enable > 0) ? 1 : 0;

    if (enableLink)
    {
        /* Power Up serializer lanes */
        if (device->rx->framer->serializerLanesEnabled > 15)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_ENFRAMERLINK_INV_LANESEN_PARM,
                    getMykonosErrorMessage(MYKONOS_ERR_ENFRAMERLINK_INV_LANESEN_PARM));
            return MYKONOS_ERR_ENFRAMERLINK_INV_LANESEN_PARM;
        }

        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_SERIALIZER_CTL_1, ((~device->rx->framer->serializerLanesEnabled) & 0x0F),
                device->rx->framer->serializerLanesEnabled, 0);

        /* Enable Clocks to Framer */
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_FRAMER_CLK_EN, 3, 0x03, 0);

        /* Release reset to framer and lane FIFOs */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_FRAMER_RESET, 0x00);
    }
    else
    {
        /* Disable Link */
        /* Hold Rx framer in reset */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_FRAMER_RESET, 0x03);

        /* Disable Clocks to Framer */
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_FRAMER_CLK_EN, 0, 0x03, 0);

        /* Power down serializer lanes */
        if (device->rx->framer->serializerLanesEnabled > 15)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_ENFRAMERLINK_INV_LANESEN_PARM,
                    getMykonosErrorMessage(MYKONOS_ERR_ENFRAMERLINK_INV_LANESEN_PARM));
            return MYKONOS_ERR_ENFRAMERLINK_INV_LANESEN_PARM;
        }

        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_SERIALIZER_CTL_1, device->rx->framer->serializerLanesEnabled, device->rx->framer->serializerLanesEnabled, 0);
    }

    return MYKONOS_ERR_OK;
}

/**
 * \brief Enables/Disables the JESD204B ObsRx Framer
 *
 * This function is normally not necessary.  In the event that the link needs to be reset, this
 * function allows the ObsRx framer to be disabled and re-enabled.
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 * - device->obsRx->framer->serializerLanesEnabled
 *
 * \param device is a pointer to the device settings structure
 * \param enable 0 = Disable the selected framer, 1 = enable the selected framer link
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_ENFRAMERLINK_INV_LANESEN_PARM Invalid serializerLanesEnabled parameter in device data structure (Valid 0-15)
 */
mykonosErr_t MYKONOS_enableObsRxFramerLink(mykonosDevice_t *device, uint8_t enable)
{
    uint8_t enableLink = 0;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_enableObsRxFramerLink()\n");
#endif

    enableLink = (enable > 0) ? 1 : 0;

    if (enableLink)
    {
        /* Power Up serializer lanes */
        if (device->rx->framer->serializerLanesEnabled > 15)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_ENOBSFRAMERLINK_INV_LANESEN_PARM,
                    getMykonosErrorMessage(MYKONOS_ERR_ENOBSFRAMERLINK_INV_LANESEN_PARM));
            return MYKONOS_ERR_ENOBSFRAMERLINK_INV_LANESEN_PARM;
        }

        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_SERIALIZER_CTL_1, ((~device->obsRx->framer->serializerLanesEnabled) & 0x0F),
                device->obsRx->framer->serializerLanesEnabled, 0);

        /* Enable Clocks to Framer */
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_OBS_FRAMER_CLK_EN, 3, 0x03, 0);

        /* Release reset to framer and lane FIFOs */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_OBS_FRAMER_RESET, 0x00);
    }
    else
    {
        /* Disable Link */
        /* Hold Rx framer in reset */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_OBS_FRAMER_RESET, 0x03);

        /* Disable Clocks to Framer */
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_OBS_FRAMER_CLK_EN, 0, 0x03, 0);

        /* Power down serializer lanes */
        if (device->obsRx->framer->serializerLanesEnabled > 15)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_ENOBSFRAMERLINK_INV_LANESEN_PARM,
                    getMykonosErrorMessage(MYKONOS_ERR_ENOBSFRAMERLINK_INV_LANESEN_PARM));
            return MYKONOS_ERR_ENOBSFRAMERLINK_INV_LANESEN_PARM;
        }

        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_SERIALIZER_CTL_1, device->obsRx->framer->serializerLanesEnabled,
                device->obsRx->framer->serializerLanesEnabled, 0);
    }

    return MYKONOS_ERR_OK;
}

/**
 * \brief Sets up the JESD204B Deframer
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 * - device->tx->deframer->M
 * - device->tx->deframer->bankId
 * - device->tx->deframer->lane0Id
 * - device->tx->deframer->K
 * - device->tx->deframer->deserializerLanesEnabled
 * - device->tx->deframer->externalSysref
 * - device->tx->deframer->newSysrefOnRelink
 * - device->tx->deframer->enableAutoChanXbar
 * - device->tx->deframer->lmfcOffset
 * - device->tx->deframer->scramble
 *
 * \param device Pointer to device settings structure
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_DEFRAMER_INV_M_PARM Invalid M parameter in deframer structure
 * \retval MYKONOS_ERR_DEFRAMER_INV_BANKID_PARM Invalid BankId parameter in deframer structure (Valid 0-15)
 * \retval MYKONOS_ERR_ERR_DEFRAMER_INV_LANEID_PARM Invalid Lane0Id parameter in deframer structure (Valid 0-31)
 * \retval MYKONOS_ERR_DEFRAMER_INV_K_PARAM Invalid K parameter in deframer structure (valid 1-32 with other constraints)
 * \retval MYKONOS_ERR_DEFRAMER_INV_FK_PARAM Invalid F*K parameter (Valid F*K > 20, F*K must be divisible by 4), K must be <= 32
 * \retval MYKONOS_ERR_DEFRAMER_INV_K_OFFSET_PARAM Invalid K offset parameter in deframer structure (must be less than K)
 */
mykonosErr_t MYKONOS_setupJesd204bDeframer(mykonosDevice_t *device)
{
    uint8_t i = 0;
    uint8_t temp = 0;
    uint8_t reg080 = 0;
    uint8_t laneXbar = 0;
    uint8_t L = 0;
    uint8_t ML = 0;

    /* Setup submap registers */
    uint8_t SUBCLASSV = 1;
    uint8_t JESDV = 1;
    uint8_t DID = 0;
    uint8_t BID = 0;
    uint8_t LID0 = 0;
    uint8_t CS = 2; /* 2 control bits */
    uint8_t N = 0x0D; /* 14 bits */
    uint8_t Np = 0x0F; /* 16 bits */
    uint8_t S = 0;
    uint8_t CF = 0;
    uint8_t K = 0x1F;
    uint16_t FK = 0;
    uint8_t HD = 0; /* only one case has HD == 1 */
    uint8_t CTRLREG0 = 0;
    uint8_t DeframerL = 1;
    uint8_t DeframerM = 2;
    uint8_t DeframerF = 0;
    uint8_t CTRLREG1 = 0;
    uint8_t CTRLREG2 = 0;
    uint8_t DeframerLaneEnable = 0x00;
    uint16_t CheckSum = 0;
    uint8_t subaddr[25] = {0};
    uint8_t subdata[25] = {0};

    uint8_t deframerInput = 0;
    uint8_t lane = 0;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_setupJesd204bDeframer()\n");
#endif

    if (device->tx->deframer->M != 0 && device->tx->deframer->M != 2 && device->tx->deframer->M != 4)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DEFRAMER_INV_M_PARM, getMykonosErrorMessage(MYKONOS_ERR_DEFRAMER_INV_M_PARM));
        return MYKONOS_ERR_DEFRAMER_INV_M_PARM;
    }

    if (device->tx->deframer->bankId > 15)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DEFRAMER_INV_BANKID_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_DEFRAMER_INV_BANKID_PARM));
        return MYKONOS_ERR_DEFRAMER_INV_BANKID_PARM;
    }

    if (device->tx->deframer->lane0Id > 31)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_ERR_DEFRAMER_INV_LANEID_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_ERR_DEFRAMER_INV_LANEID_PARM));
        return MYKONOS_ERR_ERR_DEFRAMER_INV_LANEID_PARM;
    }

    //count number of lanes
    L = 0;
    ML = 0;

    for (i = 0; i < 4; i++)
    {
        L += ((device->tx->deframer->deserializerLanesEnabled >> i) & 0x01);
    }

    ML = device->tx->deframer->M * 10 + L;

    if (device->tx->deframer->K > 32)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DEFRAMER_INV_K_PARAM,
                getMykonosErrorMessage(MYKONOS_ERR_DEFRAMER_INV_K_PARAM));
        return MYKONOS_ERR_DEFRAMER_INV_K_PARAM;
    }

    FK = (uint16_t)device->tx->deframer->K * 2 * device->tx->deframer->M / L;

    if (FK < 20 || FK > 256 || FK % 4 != 0)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DEFRAMER_INV_FK_PARAM,
                getMykonosErrorMessage(MYKONOS_ERR_DEFRAMER_INV_FK_PARAM));
        return MYKONOS_ERR_DEFRAMER_INV_FK_PARAM;
    }

    CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_DEFRAMER_SYNC_REQ_RETIME, (device->tx->deframer->K - 1), 0x1F, 0x00);

    if (!device->tx->deframer->externalSysref)
    {
        /* Deframer: Generate SYSREF internally */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_DEFRAMER_TEST, 0x02);
    }

    /* enabling the SYSREF for relink if newSysrefOnRelink is set */
    if (device->tx->deframer->newSysrefOnRelink)
    {
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_DEFRAMER_SYSREF_FIFO_EN, 0x01, 0x40, 6);
    }

    /* enabling auto channel if the enableAutoChanXbar structure member is set */
    if (device->tx->deframer->enableAutoChanXbar)
    {
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_DEFRAMER_DET_FIFO_WR_STRT_DAC_XBAR_REV, 0x01, 0x04, 2);
    }

    /* Fix bad default bit to allow deterministic latency on deframer (Clear bit 4)*/
    CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_DEFRAMER_DET_FIFO_WR_STRT_DAC_XBAR_REV, 0x00, 0x10, 4);

    /* Deframer: Set DAC Crossbar */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_DEFRAMER_DAC_XBAR_SEL, 0xB1);

    /* Lane crossbar  - Allow user to reorder lanes in deframer->deserializerLaneCrossbar, but this code still */
    /* maps used lanes to deframer inputs */
    deframerInput = 0;
    for (lane = 0; lane < 4; lane++)
    {
        if ((device->tx->deframer->deserializerLanesEnabled >> lane) & 1)
        {
            laneXbar |= (((device->tx->deframer->deserializerLaneCrossbar >> (lane << 1)) & 3) << (deframerInput << 1));
            deframerInput += 1;
        }
    }

    if (ML == 42)
    {
        /* M4L2 uses internal deframer ports 0 and 2 */
        /* swap bits 3:2 and 5:4, keep 1:0 and 7:6 in place */
        laneXbar = (laneXbar & 0xC3U) | ((laneXbar & 0x30) >> 2) | ((laneXbar & 0x0C) << 2);
    }

    /* Deframer: Set Lane Crossbar */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_DEFRAMER_LANE_XBAR_SEL, laneXbar);

    //Find value for Framer F depending on M and L
    switch (ML)
    {
        case 21:
            reg080 = 0x04;
            break;
        case 22:
            reg080 = 0x22;
            break;
        case 24:
            reg080 = 0x41;
            break;
        case 41:
            reg080 = 0x18;
            break;
        case 42:
            reg080 = 0x34;
            break;
        case 44:
            reg080 = 0x52;
            break;
        default:
            reg080 = 0x04;
            break;
    }

    /* Deframer: Set F (Octets in Frame) */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_DEFRAMER_CONFIG_F, reg080);

    /* Deframer: Enable clocks and lane clocks */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_DEFRAMER_CLK_EN, 0x03);

    /* Set deframer lane FIFO enable for each lane enabled */
    temp = (device->tx->deframer->deserializerLanesEnabled & 0x0F);

    /* Deframer: Enable Lane FIFOs */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_DEFRAMER_LANE_FIFO_CTL, temp);

    /* Setup submap registers */
    SUBCLASSV = 1; /* JESD subclass 1 */
    JESDV = 1; /* Version: JESD204B */
    DID = device->tx->deframer->deviceId;
    BID = device->tx->deframer->bankId;
    LID0 = device->tx->deframer->lane0Id;
    CS = 2; /* 2 control bits */
    N = 0x0D; /* 14 bits */
    Np = 0x0F;
    S = 0;
    CF = 0;
    K = device->tx->deframer->K - 1;
    HD = 0; /* only one case has HD = 1 */
    CTRLREG0 = 1;
    DeframerL = L - 1;
    DeframerM = device->tx->deframer->M - 1;
    DeframerF = (2 * device->tx->deframer->M / L) - 1;
    CheckSum = 0;

    if (ML == 21)
    {
        CTRLREG1 = 4;
        CTRLREG2 = 0x14;
        DeframerLaneEnable = 1;
    }
    else if (ML == 22)
    {
        CTRLREG1 = 2;
        CTRLREG2 = 4;
        DeframerLaneEnable = 3;
    }
    else if (ML == 24)
    {
        HD = 1;
        CTRLREG1 = 1;
        CTRLREG2 = 0;
        DeframerLaneEnable = 0x0F;
    }
    else if (ML == 41)
    {
        CTRLREG1 = 8;
        CTRLREG2 = 0;
        DeframerLaneEnable = 1;
    }
    else if (ML == 42)
    {
        CTRLREG1 = 4;
        CTRLREG2 = 0;
        DeframerLaneEnable = 5;
    }
    else if (ML == 44)
    {
        CTRLREG1 = 2;
        CTRLREG2 = 0;
        DeframerLaneEnable = 0x0F;
    }
    else
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DEFRAMER_INV_M_PARM, getMykonosErrorMessage(MYKONOS_ERR_DEFRAMER_INV_M_PARM));
        return MYKONOS_ERR_DEFRAMER_INV_M_PARM;
    }

    /* LMFC offset limit check */
    if (device->tx->deframer->lmfcOffset < device->tx->deframer->K)
    {
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_DEFRAMER_LMFC_K_OFFSET, device->tx->deframer->lmfcOffset);
    }
    else
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DEFRAMER_INV_K_OFFSET_PARAM,
                getMykonosErrorMessage(MYKONOS_ERR_DEFRAMER_INV_K_OFFSET_PARAM));
        return MYKONOS_ERR_DEFRAMER_INV_K_OFFSET_PARAM;
    }

    subaddr[0] = 0x50;
    subdata[0] = DID & 0xFFU; /* Device ID */
    subaddr[1] = 0x51;
    subdata[1] = BID & 0x0F; /* Bank ID */
    subaddr[2] = 0x52;
    subdata[2] = LID0 & 0x1F; /* Lane 0 ID */
    subaddr[3] = 0x53;
    subdata[3] = (device->tx->deframer->scramble << 7) | (DeframerL & 0x1F); /* [7] = Scramble Enable, #Lanes[4:0] */
    subaddr[4] = 0x54;
    subdata[4] = DeframerF & 0xFFU; /* F[7:0] */
    subaddr[5] = 0x55;
    subdata[5] = K & 0x1F; /* K[4:0] */
    subaddr[6] = 0x56;
    subdata[6] = DeframerM & 0xFFU; /* M[7:0] */
    subaddr[7] = 0x57;
    subdata[7] = ((CS & 0x3) << 6) | (N & 0x1F); /* [7:6] = CS[1:0], N[4:0] */
    subaddr[8] = 0x58;
    subdata[8] = ((SUBCLASSV & 7) << 5) | (Np & 0x1F); /* Np[4:0] */
    subaddr[9] = 0x59;
    subdata[9] = ((JESDV & 7) << 5) | (S & 0x1F); /* S[4:0] */
    subaddr[10] = 0x5A;
    subdata[10] = ((HD & 1) << 7) | (CF & 0x1F); /* [7]=HD, CF[4:0] */
    subaddr[11] = 0x5B;
    subdata[11] = 0x00; /* reserved */
    subaddr[12] = 0x5C;
    subdata[12] = 0x00; /* reserved */
    CheckSum = (DID & 0xFF) + (BID & 0xF) + (LID0 & 0x1F) + (device->tx->deframer->scramble) + (DeframerL & 0x1F) + (DeframerF & 0xFF) + (K & 0x1F) + (DeframerM & 0xFF)
            + (CS & 0x3) + (N & 0x1F) + (Np & 0x1F) + (S & 0x1F) + (HD & 1) + (CF & 0x1F) + (SUBCLASSV & 7) + (JESDV & 7);
    subaddr[13] = 0x5D;
    subdata[13] = CheckSum & 0xFFU; /* Checksum Lane 0 */
    subaddr[14] = 0x6D;
    subdata[14] = 0xA0U; /* Bad Disparity setup */
    subaddr[15] = 0x6E;
    subdata[15] = 0xA0U; /* Not in Table setup */
    subaddr[16] = 0x6F;
    subdata[16] = 0xA0U; /* UnExpected K character setup */
    subaddr[17] = 0x75;
    subdata[17] = CTRLREG0; /* CTRLREG 0 */
    subaddr[18] = 0x76;
    subdata[18] = CTRLREG1; /* CTRLREG 1 (Bytes per frame) */
    subaddr[19] = 0x77;
    subdata[19] = CTRLREG2; /* CTRLREG 2 */
    subaddr[20] = 0x78;
    subdata[20] = 0x01; /* # 4*K multiframes during ILAS */
    subaddr[21] = 0x7A;
    subdata[21] = 0xE0U; /* Setup JESD interrupt sources */
    subaddr[22] = 0x7B;
    subdata[22] = 0x08U; /* Sync assertion setup */
    subaddr[23] = 0x7C;
    subdata[23] = 0xFFU; /* Error count threshold */
    subaddr[24] = 0x7D;
    subdata[24] = DeframerLaneEnable & 0xFFU; /* Lane enable */

    for (i = 0; i <= 24; i++)
    {
        /* Set deframer sub-register map address */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_DEFRAMER_ADDR, subaddr[i]);

        /* Set deframer sub-register map data word */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_DEFRAMER_DATA, subdata[i]);

        /* Set write enable to latch data into deframer sub register map */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_DEFRAMER_WR_EN, 0x01);
    }

    /* Deframer: Enable lane FIFO sync */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_DEFRAMER_SYSREF_FIFO_EN, 0x10);

    return MYKONOS_ERR_OK;
}

/**
 * \brief Sets up the chip for multichip sync, and cleans up after MCS.
 *
 *  When working with multiple transceivers or even only one transceiver that requires deterministic
 *  latency between the Tx and observation and or main Rx JESD204B data path, Multichip sync is
 *  necessary.  This function should be run after all transceivers are initialized.
 *
 *  After the SYSREF pulses have been sent, call the MYKONOS_enableMultichipSync() function again with the
 *  enableMcs parameter set to 0.  When enableMcs = 0, the MCS status will be returned in the mcsStatus
 *  parameter.
 *
 *  Typical sequence:
 *  1) Initialize all Mykonos devices in system using MYKONOS_initialize()
 *  2) Run MYKONOS_enableMultichipSync with enableMcs = 1
 *  3) Send at least 3 SYSREF pulses
 *  4) Run MYKONOS_enableMultichipSync with enableMcs = 0
 *  5) Load ARM, run ARM cals and continue to active Transmit/Receive
 *
 *  mcsStatus | bit Description
 * -----------|--------------------------------------------------------
 *       [0]  | MCS JESD SYSREF Status (1 = sync occurred)
 *       [1]  | MCS Digital Clocks Sync Status (1 = sync occurred)
 *       [2]  | MCS CLKPLL SDM Sync Status (1 = sync occurred)
 *       [3]  | MCS Device Clock divider Sync Status (1 = sync occurred)
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 *
 * \param device is a pointer to the device settings structure
 * \param enableMcs =1 will enable the MCS state machine, =0 will allow reading back MCS status
 * \param mcsStatus optional parameter, if pointer is not null the function Which will be populated with the mcsStatus word described in the table above.
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 */
mykonosErr_t MYKONOS_enableMultichipSync(mykonosDevice_t *device, uint8_t enableMcs, uint8_t *mcsStatus)
{
    uint8_t clkPllSdmBypass = 0;
    uint8_t mcsEnable = 0x9B; /* [7] Keeps RF LO divider enabled, Enable MCS[4] and reset Device Clock divider[3], Digital clocks[1], and JESD204 SYSREF[0] */

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_enableMultichipSync()\n");
#endif

    if (enableMcs)
    {
        /* If CLKPLL SDM not bypassed, reset CLKPLL SDM as well. */
        CMB_SPIReadField(device->spiSettings, MYKONOS_ADDR_CLK_SYNTH_DIVIDER_INT_BYTE1, &clkPllSdmBypass, 0x40, 6);

        if (clkPllSdmBypass == 0)
        {
            mcsEnable |= 0x04; /* enable MCS for CLKPLL SDM */
        }

        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_MCS_CONTROL, mcsEnable);
    }

    /* if mcsStatus is a valid pointer, return the MCS status */
    if (mcsStatus != NULL)
    {
        CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_MCS_STATUS, mcsStatus);
    }

    return MYKONOS_ERR_OK;
}

/**
 * \brief Enables or disables SYSREF to the transceiver's RX framer
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 *
 * \param device is a pointer to the device settings structure
 * \param enable = '1' enables SYSREF to RX framer, '0' disables SYSREF to framer
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 */
mykonosErr_t MYKONOS_enableSysrefToRxFramer(mykonosDevice_t *device, uint8_t enable)
{

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_enableSysrefToRxFramer()\n");
#endif

    if (enable)
    {
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_FRAMER_SYSREF_FIFO_EN, 0x01, 0x01, 0);
    }
    else
    {
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_FRAMER_SYSREF_FIFO_EN, 0x00, 0x01, 0);
    }

    return MYKONOS_ERR_OK;
}

/**
 * \brief Enables or disables SYSREF to the transceiver's Observation RX framer
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 *
 * \param device is a pointer to the device settings structure
 * \param enable = '1' enables SYSREF to OBSRX framer, '0' disables SYSREF to framer
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 */
mykonosErr_t MYKONOS_enableSysrefToObsRxFramer(mykonosDevice_t *device, uint8_t enable)
{

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_enableSysrefToObsRxFramer()\n");
#endif

    if (enable)
    {
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_OBS_FRAMER_SYSREF_FIFO_EN, 0x01, 0x01, 0);
    }
    else
    {
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_OBS_FRAMER_SYSREF_FIFO_EN, 0x00, 0x01, 0);
    }

    return MYKONOS_ERR_OK;
}

/**
 * \brief Enables or disables SYSREF to the transceiver's deframer
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 *
 * \param device is a pointer to the device settings structure
 * \param enable = '1' enables SYSREF to deframer, '0' disables SYSREF to deframer
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 */
mykonosErr_t MYKONOS_enableSysrefToDeframer(mykonosDevice_t *device, uint8_t enable)
{

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_enableSysrefToDeframer()\n");
#endif

    if (enable)
    {
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_DEFRAMER_SYSREF_FIFO_EN, 0x01, 0x01, 0);
    }
    else
    {
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_DEFRAMER_SYSREF_FIFO_EN, 0x00, 0x01, 0);
    }

    return MYKONOS_ERR_OK;
}

/**
 * \brief Reads the transceiver's RX framer status
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 *
 * framerStatus |  Description
 * -------------|-----------------------------------------------------------------------------
 *         [7]  | SYSREF phase error – a new SYSREF had different timing than the first that set the LMFC timing.
 *         [6]  | Framer lane FIFO read/write pointer delta has changed.  Can help debug issues with deterministic latency.
 *         [5]  | Framer has received the SYSREF and has retimed its LMFC
 *       [4:2]  | Framer ILAS state: 0=CGS, 1= 1st Multframe, 2= 2nd Multiframe, 3= 3rd Multiframe, 4= 4th multiframe, 5= Last multiframe, 6=invalid, 7= ILAS complete
 *       [1:0]  | Framer Tx state: 0=CGS, 1= ILAS, 2 = ADC Data
 *
 * \param device is a pointer to the device settings structure
 * \param framerStatus is the RX framer status byte read
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_READ_RXFRAMERSTATUS_NULL_PARAM Function parameter framerStatus has NULL pointer
 */
mykonosErr_t MYKONOS_readRxFramerStatus(mykonosDevice_t *device, uint8_t *framerStatus)
{

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_readRxFramerStatus()\n");
#endif

    if (framerStatus == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_READ_RXFRAMERSTATUS_NULL_PARAM,
                getMykonosErrorMessage(MYKONOS_ERR_READ_RXFRAMERSTATUS_NULL_PARAM));
        return MYKONOS_ERR_READ_RXFRAMERSTATUS_NULL_PARAM;
    }

    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_FRAMER_STATUS_STRB, 0x01);
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_FRAMER_STATUS_STRB, 0x00);
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_FRAMER_STATUS, framerStatus);

    return MYKONOS_ERR_OK;
}

/**
 * \brief Reads the transceiver's Observation RX framer status
 *
 * <B>Dependencies</B>
 * * - device->spiSettings
 *
 * obsFramerStatus |  Description
 * ----------------|-----------------------------------------------------------------------------
 *            [7]  | SYSREF phase error – a new SYSREF had different timing than the first that set the LMFC timing.
 *            [6]  | Framer lane FIFO read/write pointer delta has changed.  Can help debug issues with deterministic latency.
 *            [5]  | Framer has received the SYSREF and has retimed its LMFC
 *          [4:2]  | Framer ILAS state: 0=CGS, 1= 1st Multframe, 2= 2nd Multiframe, 3= 3rd Multiframe, 4= 4th multiframe, 5= Last multiframe, 6=invalid, 7= ILAS complete
 *          [1:0]  | Framer Tx state: 0=CGS, 1= ILAS, 2 = ADC Data
 *
 * \param device is a pointer to the device settings structure
 * \param obsFramerStatus is the OBSRX framer status byte read
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_READ_ORXFRAMERSTATUS_NULL_PARAM Function parameter obsFramerStatus has NULL pointer
 */
mykonosErr_t MYKONOS_readOrxFramerStatus(mykonosDevice_t *device, uint8_t *obsFramerStatus)
{

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_readOrxFramerStatus()\n");
#endif

    if (obsFramerStatus == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_READ_ORXFRAMERSTATUS_NULL_PARAM,
                getMykonosErrorMessage(MYKONOS_ERR_READ_ORXFRAMERSTATUS_NULL_PARAM));
        return MYKONOS_ERR_READ_ORXFRAMERSTATUS_NULL_PARAM;
    }

    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_OBS_FRAMER_STATUS_STRB, 0x01);
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_OBS_FRAMER_STATUS_STRB, 0x00);
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_OBS_FRAMER_STATUS, obsFramerStatus);

    return MYKONOS_ERR_OK;
}

/**
 * \brief Reads the transceiver's deframer status
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 *
 *   deframerStatus  |  Bit Name                |  Description
 *   ----------------|--------------------------|---------------------------------------------
 *              [7]  | Unused                   | Unused
 *              [6]  | Deframer IRQ             | This bit indicates that the IRQ interrupt was asserted.
 *              [5]  | Deframer SYSREF Received | When this bit is set, it indicates that the SYSREF pulse was received by the deframer IP
 *              [4]  | Deframer Receiver Error  | This bit is set when PRBS has received an error.
 *              [3]  | Valid Checksum           | This bit is set when the received ILAS checksum is valid.
 *              [2]  | EOF Event                | This bit captures the internal status of the framer End of Frame event. Value =1 if framing error during ILAS
 *              [1]  | EOMF Event               | This bit captures the internal status of the framer End of Multi-Frame event. Value =1 if framing error during ILAS
 *              [0]  | FS Lost                  | This bit captures the internal status of the framer Frame Symbol event. Value =1 if framing error during ILAS or user data (invalid replacement characters)
 *
 *
 * \param device is a pointer to the device settings structure
 * \param deframerStatus is the deframer status byte read
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_READ_DEFRAMERSTATUS_NULL_PARAM Function parameter deframerStatus has NULL pointer
 */
mykonosErr_t MYKONOS_readDeframerStatus(mykonosDevice_t *device, uint8_t *deframerStatus)
{

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_readDeframerStatus()\n");
#endif

    if (deframerStatus == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_READ_DEFRAMERSTATUS_NULL_PARAM,
                getMykonosErrorMessage(MYKONOS_ERR_READ_DEFRAMERSTATUS_NULL_PARAM));
        return MYKONOS_ERR_READ_DEFRAMERSTATUS_NULL_PARAM;
    }

    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_DEFRAMER_STAT_STRB, 0x01);
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_DEFRAMER_STAT_STRB, 0x00);
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_DEFRAMER_STAT, deframerStatus);

    return MYKONOS_ERR_OK;
}

/**
 * \brief Reads the Mykonos JESD204b Deframer determinstic FIFO depth
 *
 * To verify that the deterministic latency FIFO is not close to a underflow or
 * overflow condition, it is recommended to check the FIFO depth. If the FIFO
 * is close to an overflow or underflow condition, it is possible that from
 * power up to powerup, deterministic latency may not be met.  If a underflow
 * or overflow occurred, the data would still be correct, but would possibly
 * slip by 1 multiframe (losing deterministic latency).  To correct for an
 * overflow/underflow, the BBIC would need to add delay from SYSREF until
 * the first symbol in a multiframe
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 *
 * \param device is a pointer to the device settings structure
 * \param fifoDepth Return value that describes the depth of the Mykonos
 *                  deterministic latency deframer FIFO.
 * \param readEnLmfcCount Returns the LMFC count value when the deterministic FIFO read enable was asserted.
 *                        Counts at the Mykonos internal deframer PCLK frequency.
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_READ_DEFFIFODEPTH_NULL_PARAM Error: function parameter fifoDepth is a NULL pointer
 * \retval MYKONOS_ERR_READ_DEFFIFODEPTH_LMFCCOUNT_NULL_PARAM Error: function parameter readEnLmfcCount is a NULL pointer
 *
 */
mykonosErr_t MYKONOS_getDeframerFifoDepth(mykonosDevice_t *device, uint8_t *fifoDepth, uint8_t *readEnLmfcCount)
{
    uint8_t fifoReadPtr = 0;
    uint8_t fifoWritePtr = 0;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_getDeframerFifoDepth()\n");
#endif

    if (fifoDepth == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_READ_DEFFIFODEPTH_NULL_PARAM,
                getMykonosErrorMessage(MYKONOS_ERR_READ_DEFFIFODEPTH_NULL_PARAM));
        return MYKONOS_ERR_READ_DEFFIFODEPTH_NULL_PARAM;
    }

    if (readEnLmfcCount == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_READ_DEFFIFODEPTH_LMFCCOUNT_NULL_PARAM,
                getMykonosErrorMessage(MYKONOS_ERR_READ_DEFFIFODEPTH_LMFCCOUNT_NULL_PARAM));
        return MYKONOS_ERR_READ_DEFFIFODEPTH_LMFCCOUNT_NULL_PARAM;
    }

    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_DEFRAMER_STAT_STRB, 0x01);
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_DEFRAMER_STAT_STRB, 0x00);

    /* read/write pointers are 7 bits, (0 - 127) */
    CMB_SPIReadField(device->spiSettings, MYKONOS_ADDR_DEFRAMER_DET_FIFO_RD_ADDR, &fifoReadPtr, 0x7F, 0);
    CMB_SPIReadField(device->spiSettings, MYKONOS_ADDR_DEFRAMER_DET_FIFO_WR_ADDR, &fifoWritePtr, 0x7F, 0);

    /* Adding 128 and modulus 128 handle the wrap cases where the read pointer
     * is less than write pointer
     */
    *fifoDepth = (((fifoReadPtr + 128) - fifoWritePtr) % 128);

    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_DEFRAMER_DET_FIFO_PHASE, readEnLmfcCount);

    return MYKONOS_ERR_OK;
}

/**
 * \brief Selects the PRBS type and enables or disables RX Framer PRBS20 generation
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 *
 * \param device is a pointer to the device settings structure
 * \param polyOrder selects the PRBS type based on a two-bit value range from 0-3
 * \param enable '1' = enables PRBS RX framer PRBS generator, '0' = disables PRBS generator
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_RX_FRAMER_INV_PRBS_POLYORDER_PARAM Invalid polyOrder parameter, use ENUM
 *
 */
mykonosErr_t MYKONOS_enableRxFramerPrbs(mykonosDevice_t *device, mykonosPrbsOrder_t polyOrder, uint8_t enable)
{
    uint8_t wrmask = 0;
    uint8_t enableBit = 0;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_enableRxFramerPrbs()\n");
#endif

    enableBit = (enable > 0) ? 1 : 0;

    if ((polyOrder == MYK_PRBS7) || (polyOrder == MYK_PRBS15) || (polyOrder == MYK_PRBS31))
    {
        wrmask = (polyOrder << 1) | enableBit;
    }
    else
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_RX_FRAMER_INV_PRBS_POLYORDER_PARAM,
                getMykonosErrorMessage(MYKONOS_ERR_RX_FRAMER_INV_PRBS_POLYORDER_PARAM));
        return MYKONOS_ERR_RX_FRAMER_INV_PRBS_POLYORDER_PARAM;
    }

    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_FRAMER_PRBS20_CTL, wrmask);

    return MYKONOS_ERR_OK;
}

/**
 * \brief Selects the PRBS type and enables or disables OBSRX Framer PRBS20 generation
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 *
 * \param device is a pointer to the device settings structure
 * \param polyOrder selects the PRBS type based on a two-bit value range from 0-3
 * \param enable '1' = enables PRBS OBSRX framer PRBS generator, '0' = disables PRBS generator
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_OBSRX_FRAMER_INV_PRBS_POLYORDER_PARAM polyOrder parameter is not a valid value - use ENUM
 */
mykonosErr_t MYKONOS_enableObsRxFramerPrbs(mykonosDevice_t *device, mykonosPrbsOrder_t polyOrder, uint8_t enable)
{
    uint8_t wrmask = 0;
    uint8_t enableBit = 0;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_enableObsRxFramerPrbs()\n");
#endif

    enableBit = (enable > 0) ? 1 : 0;

    if ((polyOrder == MYK_PRBS7) || (polyOrder == MYK_PRBS15) || (polyOrder == MYK_PRBS31))
    {
        wrmask = (polyOrder << 1) | enableBit;
    }
    else
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OBSRX_FRAMER_INV_PRBS_POLYORDER_PARAM,
                getMykonosErrorMessage(MYKONOS_ERR_OBSRX_FRAMER_INV_PRBS_POLYORDER_PARAM));
        return MYKONOS_ERR_OBSRX_FRAMER_INV_PRBS_POLYORDER_PARAM;
    }

    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_OBS_FRAMER_PRBS20_CTL, wrmask);

    return MYKONOS_ERR_OK;
}

/**
 * \brief Injects a PRBS error into the RX data path
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 *
 * \param device is a pointer to the device settings structure
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 */
mykonosErr_t MYKONOS_rxInjectPrbsError(mykonosDevice_t *device)
{
    uint8_t prbsControl = 0x00;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_rxInjectPrbsError()\n");
#endif

    /* reading current PRBS20 control register contents */
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_FRAMER_PRBS20_CTL, &prbsControl);

    /* setting bit 4 in framer PRBS20 control register and then clearing it for error injection */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_FRAMER_PRBS20_CTL, prbsControl |= 0x10);
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_FRAMER_PRBS20_CTL, prbsControl &= ~0x10);

    return MYKONOS_ERR_OK;
}

/**
 * \brief Initiates a PRBS error injection into the Observation RX data path
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 *
 * \param device is a pointer to the device settings structure
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 */
mykonosErr_t MYKONOS_obsRxInjectPrbsError(mykonosDevice_t *device)
{
    uint8_t prbsControl = 0x00;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_obsRxInjectPrbsError()\n");
#endif

    /* reading current PRBS20 control register contents */
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_OBS_FRAMER_PRBS20_CTL, &prbsControl);

    /* setting bit 4 in framer PRBS20 control register and then clearing it for error injection */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_OBS_FRAMER_PRBS20_CTL, prbsControl |= 0x10);
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_OBS_FRAMER_PRBS20_CTL, prbsControl &= ~0x10);

    return MYKONOS_ERR_OK;
}

/**
 * \brief Configures and enables or disables the transceiver's deframer PRBS checker
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 *
 * \param device is a pointer to the device settings structure
 * \param lanes selects the lane for PRBS checking based on a 4-bit mask, where each weighted bit
 * corresponds with a different lane selection as such: '1' = lane 0, '2' = lane 1, '4' = lane 2, '8' = lane 3
 * \param polyOrder selects the PRBS type based on enum values (MYK_PRBS7, MYK_PRBS15, MYK_PRBS31)
 * \param enable '1' = enables checking, '0' = disables checking
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_DEFRAMER_INV_PRBS_POLYORDER_PARAM Invalid polyOrder parameter - use ENUM
 * \retval MYKONOS_ERR_DEFRAMER_INV_PRBS_ENABLE_PARAM Invalid enable (valid 0-1) or lanes (valid 0-15) parameter
 */
mykonosErr_t MYKONOS_enableDeframerPrbsChecker(mykonosDevice_t *device, uint8_t lanes, mykonosPrbsOrder_t polyOrder, uint8_t enable)
{
    uint8_t wrmask = 0x00;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_enableDeframerPrbsChecker()\n");
#endif

    if ((enable <= 0x01) && (lanes <= 0x0F))
    {
        if ((polyOrder == MYK_PRBS7) || (polyOrder == MYK_PRBS15) || (polyOrder == MYK_PRBS31))
        {
            wrmask = (lanes << 4) | (polyOrder << 1) | enable;
        }
        else
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DEFRAMER_INV_PRBS_POLYORDER_PARAM,
                    getMykonosErrorMessage(MYKONOS_ERR_DEFRAMER_INV_PRBS_POLYORDER_PARAM));
            return MYKONOS_ERR_DEFRAMER_INV_PRBS_POLYORDER_PARAM;
        }
    }
    else
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DEFRAMER_INV_PRBS_ENABLE_PARAM,
                getMykonosErrorMessage(MYKONOS_ERR_DEFRAMER_INV_PRBS_ENABLE_PARAM));
        return MYKONOS_ERR_DEFRAMER_INV_PRBS_ENABLE_PARAM;
    }

    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_DEFRAMER_PRBS20_CTL, wrmask);

    return MYKONOS_ERR_OK;
}

/**
 * \brief Reads the deframer PRBS counters
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 *
 * \param device is a pointer to the device settings structure
 * \param counterSelect selects the PRBS error counter to be read based on values between 0-3
 *                      If counterSelect exceeds this value an error is thrown.
 *
 * \param prbsErrorCount is return value after reading the PRBS error count
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_READ_DEFRAMERPRBS_NULL_PARAM Function parameter prbsErrorCount has NULL pointer
 * \retval MYKONOS_ERR_DEFRAMER_INV_PRBS_CNTR_SEL_PARAM if counterSelect is out of bounds
 */
mykonosErr_t MYKONOS_readDeframerPrbsCounters(mykonosDevice_t *device, uint8_t counterSelect, uint32_t *prbsErrorCount)
{
    uint8_t wrmask = 0x00;
    uint8_t errorCnt[3];
    const uint8_t COUNTER_SATURATE = 0x40;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_readDeframerPrbsCounters()\n");
#endif

    if (prbsErrorCount == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_READ_DEFRAMERPRBS_NULL_PARAM,
                getMykonosErrorMessage(MYKONOS_ERR_READ_DEFRAMERPRBS_NULL_PARAM));
        return MYKONOS_ERR_READ_DEFRAMERPRBS_NULL_PARAM;
    }

    if (counterSelect & ~0x03)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DEFRAMER_INV_PRBS_CNTR_SEL_PARAM,
                getMykonosErrorMessage(MYKONOS_ERR_DEFRAMER_INV_PRBS_CNTR_SEL_PARAM));
        return MYKONOS_ERR_DEFRAMER_INV_PRBS_CNTR_SEL_PARAM;
    }
    else
    {
        wrmask = COUNTER_SATURATE | (counterSelect << 4);
        *prbsErrorCount = 0x00000000;
    }

    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_DEFRAMER_PRBS20_STRB_CHKSUM_TYPE, wrmask);
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_DEFRAMER_PRBS20_STRB_CHKSUM_TYPE, wrmask |= 0x01);
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_DEFRAMER_PRBS20_STRB_CHKSUM_TYPE, wrmask &= ~0x01);

    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_DEFRAMER_PRBS20_ERR_CNTR_7_TO_0, &errorCnt[0]);
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_DEFRAMER_PRBS20_ERR_CNTR_15_TO_8, &errorCnt[1]);
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_DEFRAMER_PRBS20_ERR_CNTR_23_TO_16, &errorCnt[2]);

    *prbsErrorCount = (uint32_t)(errorCnt[2] << 16) | (uint32_t)(errorCnt[1] << 8) | (uint32_t)(errorCnt[0]);

    return MYKONOS_ERR_OK;
}

/**
 * \brief Clears the deframer/deserializer PRBS counters
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 *
 * \param device is a pointer to the device settings structure
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 */
mykonosErr_t MYKONOS_clearDeframerPrbsCounters(mykonosDevice_t *device)
{
    uint8_t spiReg = 0;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_clearDeframerPrbsCounters()\n");
#endif

    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_DEFRAMER_PRBS20_CTL, &spiReg);
    spiReg &= ~0x08; //make sure PRBS clear bit is 0

    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_DEFRAMER_PRBS20_CTL, (spiReg | 0xF8)); //clear counters for all lanes
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_DEFRAMER_PRBS20_CTL, spiReg); //set reg back to previous value

    return MYKONOS_ERR_OK;
}

/**
 * \brief Reads the lane 0 JESD204B deframer configuration and compares it against the ILAS received values
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is a pointer to the device settings structure
 * \param mismatch [15-0] is a bit-encoded word which for mismatch[15] = '0' = no mismatch, '1' = mismatch for one or more parameters
 * mismatch bits [14-0] are aligned with the mykonosJesd204bLane0Config_t structure members starting at '0' = DID.  The aligned bit is set to
 * '1' if the configuration parameter does not agree with its corresponding received ILAS value, otherwise '0' if they agree.
 *
 * The bit assignments for the 16-bit word are:
 *   mismatch | bit description
 * -----------|--------------------------------------------------------
 *      [0]   | JESD204B DID: device ID
 *      [1]   | JESD204B BID: bank ID
 *      [2]   | JESD204B LID0: lane ID
 *      [3]   | JESD204B L: lanes per data converter
 *      [4]   | JESD204B SCR: scramble setting
 *      [5]   | JESD204B F: octets per frame
 *      [6]   | JESD204B K: frames per multiframe
 *      [7]   | JESD204B M: number of data converters
 *      [8]   | JESD204B N: data converter sample resolution
 *      [9]   | JESD204B CS: number of control bits transferred per sample per frame
 *      [10]  | JESD204B NP: JESD204B word size based on the highest data converter resolution
 *      [11]  | JESD204B S: number of samples per converter per frame
 *      [12]  | JESD204B CF: '0' = control bits appended to each sample, '1' = control bits appended to end of frame
 *      [13]  | JESD204B HD: high density bit, where '0' = samples are contained with single lane, '1' = samples are divided over more than one lane
 *      [14]  | JESD204B FCHK0: configuration checksum OK bit. where '1' = fail, '0' = pass
 *      [15]  | MISMATCH DETECTED BIT: bits 0-14 are ored together to set this bit
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_JESD204B_ILAS_MISMATCH_NULLPARAM Function parameter mismatch has NULL pointer
 */
mykonosErr_t MYKONOS_jesd204bIlasCheck(mykonosDevice_t *device, uint16_t *mismatch)
{
    uint8_t i = 0;
    uint8_t ilasdata[15] = {0};
    uint8_t cfgdata[15] = {0};
    mykonosJesd204bLane0Config_t lane0ILAS;
    mykonosJesd204bLane0Config_t lane0Cfg;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_jesd204bIlasCheck()\n");
#endif

    if (mismatch == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_JESD204B_ILAS_MISMATCH_NULLPARAM,
                getMykonosErrorMessage(MYKONOS_ERR_JESD204B_ILAS_MISMATCH_NULLPARAM));
        return MYKONOS_ERR_JESD204B_ILAS_MISMATCH_NULLPARAM;
    }

    *mismatch = 0;

    /* setting deframer read received ILAS data */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_DEFRAMER_WR_EN, 0x00);

    /* reading deframer received ILAS register contents into array */
    for (i = 0; i < 15; i++)
    {
        /* setting the deframer sub-address for received ilas data */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_DEFRAMER_ADDR, MYKONOS_SUBADDR_DEFRAMER_LANE0_ILAS_RECVD + i);

        /* reading the received ILAS data into byte array */
        CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_DEFRAMER_DATA, &ilasdata[i]);
    }

    /* reading deframer config register contents into array */
    for (i = 0; i < 15; i++)
    {
        /* setting the deframer sub-address for received ilas data */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_DEFRAMER_ADDR, MYKONOS_SUBADDR_DEFRAMER_LANE0_ILAS_CFG + i);

        /* reading the received ILAS data into byte array */
        CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_DEFRAMER_DATA, &cfgdata[i]);
    }

    /* loading the structures with the read values for easier reading when doing compares */
    lane0ILAS.DID = ilasdata[0];
    lane0ILAS.BID = ilasdata[1] & 0x0F;
    lane0ILAS.LID0 = ilasdata[2] & 0x1F;
    lane0ILAS.L = ilasdata[3] & 0x1F;
    lane0ILAS.SCR = ilasdata[3] >> 7;
    lane0ILAS.F = ilasdata[4];
    lane0ILAS.K = ilasdata[5] & 0x1F;
    lane0ILAS.M = ilasdata[6];
    lane0ILAS.N = ilasdata[7] & 0x1F;
    lane0ILAS.CS = ilasdata[7] >> 6;
    lane0ILAS.NP = ilasdata[8] & 0x1F;
    lane0ILAS.S = ilasdata[9] & 0x1F;
    lane0ILAS.CF = ilasdata[10] & 0x1F;
    lane0ILAS.HD = ilasdata[10] >> 7;
    lane0ILAS.FCHK0 = ilasdata[13];

    lane0Cfg.DID = cfgdata[0];
    lane0Cfg.BID = cfgdata[1] & 0x0F;
    lane0Cfg.LID0 = cfgdata[2] & 0x0F;
    lane0Cfg.L = cfgdata[3] & 0x1F;
    lane0Cfg.SCR = cfgdata[3] >> 7;
    lane0Cfg.F = cfgdata[4];
    lane0Cfg.K = cfgdata[5] & 0x1F;
    lane0Cfg.M = cfgdata[6];
    lane0Cfg.N = cfgdata[7] & 0x1F;
    lane0Cfg.CS = cfgdata[7] >> 6;
    lane0Cfg.NP = cfgdata[8] & 0x1F;
    lane0Cfg.S = cfgdata[9] & 0x1F;
    lane0Cfg.CF = cfgdata[10] & 0x1F;
    lane0Cfg.HD = cfgdata[10] >> 7;
    lane0Cfg.FCHK0 = cfgdata[13];

    /* performing ILAS mismatch check */
    if (lane0ILAS.DID != lane0Cfg.DID)
    {
        *mismatch |= 0x0001;
    }

    if (lane0ILAS.BID != lane0Cfg.BID)
    {
        *mismatch |= 0x0002;
    }

    if (lane0ILAS.LID0 != lane0Cfg.LID0)
    {
        *mismatch |= 0x0004;
    }

    if (lane0ILAS.L != lane0Cfg.L)
    {
        *mismatch |= 0x0008;
    }

    if (lane0ILAS.SCR != lane0Cfg.SCR)
    {
        *mismatch |= 0x0010;
    }

    if (lane0ILAS.F != lane0Cfg.F)
    {
        *mismatch |= 0x0020;
    }

    if (lane0ILAS.K != lane0Cfg.K)
    {
        *mismatch |= 0x0040;
    }

    if (lane0ILAS.M != lane0Cfg.M)
    {
        *mismatch |= 0x0080;
    }

    if (lane0ILAS.N != lane0Cfg.N)
    {
        *mismatch |= 0x0100;
    }

    if (lane0ILAS.CS != lane0Cfg.CS)
    {
        *mismatch |= 0x0200;
    }

    if (lane0ILAS.NP != lane0Cfg.NP)
    {
        *mismatch |= 0x0400;
    }

    if (lane0ILAS.S != lane0Cfg.S)
    {
        *mismatch |= 0x0800;
    }

    if (lane0ILAS.CF != lane0Cfg.CF)
    {
        *mismatch |= 0x1000;
    }

    if (lane0ILAS.HD != lane0Cfg.HD)
    {
        *mismatch |= 0x2000;
    }

    if (lane0ILAS.FCHK0 != lane0Cfg.FCHK0)
    {
        *mismatch |= 0x4000;
    }

    if (*mismatch)
    {
        *mismatch |= 0x8000;
        CMB_writeToLog(ADIHAL_LOG_WARNING, device->spiSettings->chipSelectIndex, MYKONOS_ERR_JESD204B_ILAS_MISMATCH,
                getMykonosErrorMessage(MYKONOS_ERR_JESD204B_ILAS_MISMATCH));
    }

    return MYKONOS_ERR_OK;
}

/**
 * \brief Select data to inject into the Rx framer input (ADC data or Loopback data from deframer output)
 *
 * This function allows inputting deframed data (IQ samples) from the Tx data path into the Rx framer.
 * For this to work correctly, the IQ data rate of the Tx data path must match the Rx IQ data rate.
 * Framer/Deframer JESD204 config parameters can vary as long as the Rx and Tx IQ rates match.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device A pointer to the device settings structure
 * \param dataSource 0 = ADC data at Rx Framer input, 1 = Deframed Tx JESD204 IQ samples input into Rx framer
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 */
mykonosErr_t MYKONOS_setRxFramerDataSource(mykonosDevice_t *device, uint8_t dataSource)
{
    uint8_t enableLoopBack = 0;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_setRxFramerDataSource()\n");
#endif

    enableLoopBack = (dataSource > 0) ? 1 : 0;

    CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_FRAMER_CONFIG_LOOPBACK_XBAR_REV, enableLoopBack, 0x10, 4);

    return MYKONOS_ERR_OK;
}

/**
 * \brief Select data to inject into the ObsRx framer input (ADC data or Loopback data from deframer output)
 *
 * This function allows inputting deframed data (IQ samples) from the Tx data path into the Obs Rx framer.
 * For this to work correctly, the IQ data rate of the Tx data path must match the ORx IQ data rate.
 * Framer/Deframer JESD204 config parameters can vary as long as the Obs Rx and Tx IQ rates match.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device A pointer to the device settings structure
 * \param dataSource 0 = ADC data at ObsRx Framer input, 1 = Deframed Tx JESD204 IQ samples input into Obs Rx framer
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 */
mykonosErr_t MYKONOS_setObsRxFramerDataSource(mykonosDevice_t *device, uint8_t dataSource)
{
    uint8_t enableLoopBack = 0;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_setObsRxFramerDataSource()\n");
#endif

    enableLoopBack = (dataSource > 0) ? 1 : 0;

    CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_OBS_FRAMER_CONFIG_LOOPBACK_XBAR_REV, enableLoopBack, 0x10, 4);

    return MYKONOS_ERR_OK;
}

/**
 * \brief Runs the Mykonos initialization calibrations
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * ENUM mykonosInitCalibrations_t can be used to OR together to generate the calMask parameter.
 *
 *  calMask Bit | Calibration
 *  ------------|----------------------
 *       0      | Tx BB Filter
 *       1      | ADC Tuner
 *       2      | TIA 3dB Corner
 *       3      | DC Offset
 *       4      | Tx Attenuation Delay
 *       5      | Rx Gain Delay
 *       6      | Flash Cal
 *       7      | Path Delay
 *       8      | Tx LO Leakage Internal
 *       9      | Tx LO Leakage External
 *       10     | Tx QEC Init
 *       11     | LoopBack Rx LO Delay
 *       12     | LoopBack Rx Rx QEC Init
 *       13     | Rx LO Delay
 *       14     | Rx QEC Init
 *       15     | DPD Init
 *       16     | Tx CLGC (Closed Loop Gain Control)
 *       17     | Tx VSWR Init
 *    [31-18]   | Ignored - Future space for new calibrations
 *
 * \param device A pointer to the device settings structure
 * \param calMask A bitmask that informs the Mykonos ARM processor which calibrations to run
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 */
mykonosErr_t MYKONOS_runInitCals(mykonosDevice_t *device, uint32_t calMask)
{
    mykonosErr_t retVal = MYKONOS_ERR_OK;
    const uint8_t RUNINITCALS_OPCODE = 0x02;
    uint8_t payload[4] = {0, 0, 0, 0};

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_runInitCals()\n");
#endif

    payload[0] = (uint8_t)(calMask & 0xFF);
    payload[1] = (uint8_t)((calMask >> 8) & 0xFF);
    payload[2] = (uint8_t)((calMask >> 16) & 0xFF);
    payload[3] = (uint8_t)((calMask >> 24) & 0xFF);

    retVal = MYKONOS_sendArmCommand(device, RUNINITCALS_OPCODE, &payload[0], sizeof(payload));
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    return MYKONOS_ERR_OK;
}

/**
 * \brief Blocking waits for the Mykonos initialization calibrations to complete
 *
 * The *errorFlag and *errorCode parameters are optional.  If the pointers are
 * set to null, no values will be returned.  If the function returns an error
 * that the init calibration failed, use the MYKONOS_getInitCalStatus() function
 * to get more detailed information about why the init cal failed.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device A pointer to the device settings structure
 * \param timeoutMs A timeout value in Ms to wait for the calibrations to complete
 * \param errorFlag A 3bit error flag that helps identify what went wrong in the ARM.  0=No Error
 * \param errorCode The value represents the init calibration object ID that caused a failure.
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_WAIT_INITCALS_ARMERROR ARM returned error unrelated to init cals
 */
mykonosErr_t MYKONOS_waitInitCals(mykonosDevice_t *device, uint32_t timeoutMs, uint8_t *errorFlag, uint8_t *errorCode)
{
    mykonosErr_t retVal = MYKONOS_ERR_OK;
    mykonosErr_t retValCalStatus = MYKONOS_ERR_OK;
    uint8_t cmdStatusByte = 0;
    uint8_t _errorFlag = 0; /* Local version of parameter */
    mykonosInitCalStatus_t initCalStatus = {0};

    const uint8_t INITCALS_CAL_ERROR = 0x07;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_waitInitCals()\n");
#endif

    /* Clear before making any calls that can throw errors */
    if (errorFlag != NULL)
    {
        *errorFlag = 0;
    }

    /* Clear before making any calls that can throw errors */
    if (errorCode != NULL)
    {
        *errorCode = 0;
    }

    retVal = MYKONOS_waitArmCmdStatus(device, MYKONOS_ARM_RUNINIT_OPCODE, timeoutMs, &cmdStatusByte);
    _errorFlag = cmdStatusByte >> 1; /* remove pending bit in [0], error flag is in bits [3:1] */

    if (errorFlag != NULL)
    {
        *errorFlag = _errorFlag;
    }

    if (retVal != MYKONOS_ERR_OK)
    {
        if (_errorFlag == INITCALS_CAL_ERROR)
        {
            /* return Error code if a calibration had an error */
            retValCalStatus = MYKONOS_getInitCalStatus(device, &initCalStatus);
            if (retValCalStatus == MYKONOS_ERR_OK)
            {
                if (errorCode != NULL)
                {
                    *errorCode = initCalStatus.initErrCal;
                }
            }

            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_WAIT_INITCALS_CALFAILED,
                    getMykonosErrorMessage(MYKONOS_ERR_WAIT_INITCALS_CALFAILED));
            return MYKONOS_ERR_WAIT_INITCALS_CALFAILED;
        }
        else if (cmdStatusByte > 0)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_WAIT_INITCALS_ARMERROR,
                    getMykonosErrorMessage(MYKONOS_ERR_WAIT_INITCALS_ARMERROR));
            return MYKONOS_ERR_WAIT_INITCALS_ARMERROR;
        }
    }

    /* Same logic if MYKONOS_waitArmCmdStatus() did not return an error, but cmdStatusByte shows an ARM error */
    if (_errorFlag == INITCALS_CAL_ERROR)
    {
        /* return Error code if a calibration had an error */
        retValCalStatus = MYKONOS_getInitCalStatus(device, &initCalStatus);
        if (retValCalStatus == MYKONOS_ERR_OK)
        {
            if (errorCode != NULL)
            {
                *errorCode = initCalStatus.initErrCal;
            }
        }

        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_WAIT_INITCALS_CALFAILED,
                getMykonosErrorMessage(MYKONOS_ERR_WAIT_INITCALS_CALFAILED));
        return MYKONOS_ERR_WAIT_INITCALS_CALFAILED;
    }
    else if (cmdStatusByte > 0)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_WAIT_INITCALS_ARMERROR,
                getMykonosErrorMessage(MYKONOS_ERR_WAIT_INITCALS_ARMERROR));
        return MYKONOS_ERR_WAIT_INITCALS_ARMERROR;
    }

    return MYKONOS_ERR_OK;
}

/**
 * \brief Aborts from an on going ARM init calibration operation.
 *
 *  The ARM init calibrations can take several seconds.  If for any reason the Baseband processor
 *  needs to stop the running ARM calibration sequence, call this function.  The *calsCompleted
 *  parameter is an option parameter that will return which cals completed before the abort
 *  command was received.  If *calsCompleted is a null pointer, no value will be returned.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device A pointer to the device settings structure
 * \param calsCompleted A bitmask is returned which describes which cals completed during the previous
 *                      MYKONOS_runInitCals() call.
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_ABORT_INITCALS_NULL_PARAM Function parameter calsCompleted has NULL pointer
 */
mykonosErr_t MYKONOS_abortInitCals(mykonosDevice_t *device, uint32_t *calsCompleted)
{
    mykonosErr_t retVal = MYKONOS_ERR_OK;
    uint32_t timeoutMs = 1000; //1 second timeout
    uint8_t cmdStatusByte = 0;
    uint8_t payload = 0x43; /* object ID to get INIT_CAL_DONE status */
    uint8_t calCompleteBitField[4] = {0, 0, 0, 0};

    retVal = MYKONOS_sendArmCommand(device, MYKONOS_ARM_ABORT_OPCODE, 0, 0);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal; /* Will return if mailbox busy bit is busy for more than 2 seconds */
    }

    retVal = MYKONOS_waitArmCmdStatus(device, MYKONOS_ARM_ABORT_OPCODE, timeoutMs, &cmdStatusByte);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal; /* Will return if timeout occurred */
    }

    /* Read back Calibration Completion status */
    retVal = MYKONOS_sendArmCommand(device, MYKONOS_ARM_GET_OPCODE, &payload, sizeof(payload));
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal; /* Will return if mailbox busy bit is busy for more than 2 seconds */
    }

    retVal = MYKONOS_waitArmCmdStatus(device, MYKONOS_ARM_GET_OPCODE, timeoutMs, &cmdStatusByte);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal; /* Will return if timeout occurred */
    }

    retVal = MYKONOS_readArmMem(device, (MYKONOS_ADDR_ARM_START_DATA_ADDR + 4), &calCompleteBitField[0], 4, 1);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    if (calsCompleted != NULL)
    {
        *calsCompleted = ((uint32_t)(calCompleteBitField[3]) << 24) | ((uint32_t)(calCompleteBitField[2]) << 16) | ((uint32_t)(calCompleteBitField[1]) << 8)
                | ((uint32_t)(calCompleteBitField[0]));
    }
    else
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_ABORT_INITCALS_NULL_PARAM,
                getMykonosErrorMessage(MYKONOS_ERR_ABORT_INITCALS_NULL_PARAM));
        return MYKONOS_ERR_ABORT_INITCALS_NULL_PARAM;
    }

    return MYKONOS_ERR_OK;
}

/**
 * \brief Gets the device initialization calibration status
 *
 * This function requests the init cal status information from the Mykonos ARM
 * processor.  The ARM returns information including a bitmask that describes
 * which calibrations have completed during the last cal of runInitCals, as
 * well as the init cals that have run successfully since loading the ARM.  If
 * an ARM error does occur during one of the init calibrations, the initErrCal
 * member returns the object ID of the failing calibration.  The initErrCode
 * returns the specific ARM error code that caused that calibration to fail.
 * The calsMinimum structure member describes the minimum set of init calibrations
 * required to complete by the ARM before it will allow the device to move to
 * the radioOn state.
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 * - device->spiSettings->chipSelectIndex
 *
 * \param device A pointer to the device settings structure
 * \param initCalStatus Pointer to a structure that returns cal status information such as cals completed since last run, and init error codes
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_GETINITCALSTATUS_NULL_PARAM if initCalStatus parameter is a NULL pointer
 * \retval MYKONOS_ERR_GETINITCALSTATUS_ARMERROR if ARM returned an error while requesting the init cal status information
 */
mykonosErr_t MYKONOS_getInitCalStatus(mykonosDevice_t *device, mykonosInitCalStatus_t *initCalStatus)
{
    mykonosErr_t retVal = MYKONOS_ERR_OK;
    uint8_t cmdStatusByte = 0;
    uint8_t calStatusArray[14] = {0};
    uint8_t payload[1] = {MYKONOS_ARM_OBJECTID_INIT_CAL_DONE};

    uint32_t timeoutMs = 1000;

    /* Verify function parameter pointer is not NULL */
    if (initCalStatus == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETINITCALSTATUS_NULL_PARAM,
                getMykonosErrorMessage(MYKONOS_ERR_GETINITCALSTATUS_NULL_PARAM));
        return MYKONOS_ERR_GETINITCALSTATUS_NULL_PARAM;
    }

    retVal = MYKONOS_sendArmCommand(device, MYKONOS_ARM_GET_OPCODE, &payload[0], sizeof(payload));
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal; /* Will return if mailbox busy bit is busy for more than 2 seconds */
    }

    retVal = MYKONOS_waitArmCmdStatus(device, MYKONOS_ARM_GET_OPCODE, timeoutMs, &cmdStatusByte);
    if (retVal != MYKONOS_ERR_OK)
    {
        if (cmdStatusByte > 0)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETINITCALSTATUS_ARMERROR,
                    getMykonosErrorMessage(MYKONOS_ERR_GETINITCALSTATUS_ARMERROR));
            return MYKONOS_ERR_GETINITCALSTATUS_ARMERROR;
        }

        return retVal; /* Will return if timeout occurred */
    }

    if (cmdStatusByte > 0)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETINITCALSTATUS_ARMERROR,
                getMykonosErrorMessage(MYKONOS_ERR_GETINITCALSTATUS_ARMERROR));
        return MYKONOS_ERR_GETINITCALSTATUS_ARMERROR;
    }

    retVal = MYKONOS_readArmMem(device, MYKONOS_ADDR_ARM_START_DATA_ADDR, &calStatusArray[0], sizeof(calStatusArray), 1);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    initCalStatus->calsDoneLifetime = ((uint32_t)(calStatusArray[3]) << 24) | ((uint32_t)(calStatusArray[2]) << 16) | ((uint32_t)(calStatusArray[1]) << 8)
            | ((uint32_t)(calStatusArray[0]));
    initCalStatus->calsDoneLastRun = ((uint32_t)(calStatusArray[7]) << 24) | ((uint32_t)(calStatusArray[6]) << 16) | ((uint32_t)(calStatusArray[5]) << 8)
            | ((uint32_t)(calStatusArray[4]));
    initCalStatus->calsMinimum = ((uint32_t)(calStatusArray[11]) << 24) | ((uint32_t)(calStatusArray[10]) << 16) | ((uint32_t)(calStatusArray[9]) << 8)
            | ((uint32_t)(calStatusArray[8]));
    initCalStatus->initErrCal = calStatusArray[12];
    initCalStatus->initErrCode = calStatusArray[13];

    return MYKONOS_ERR_OK;
}

/**
 * \brief Instructs the ARM processor to move the radio state to the Radio ON state
 *
 * When the ARM to the Radio On state, the enabled Rx and Tx signal chains will power up,
 * and the ARM tracking calibrations will begin.  To exit this state back to a low power,
 * offline state, call the MYKONOS_radioOff() function.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device Pointer to the device settings structure
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_ARM_RADIOON_FAILED ARM returned error running this command
 */
mykonosErr_t MYKONOS_radioOn(mykonosDevice_t *device)
{
    mykonosErr_t retVal = MYKONOS_ERR_OK;
    uint32_t timeoutMs = 1000; //1 second timeout
    uint8_t cmdStatusByte = 0;

    retVal = MYKONOS_sendArmCommand(device, MYKONOS_ARM_RADIOON_OPCODE, 0, 0);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal; /* Will return if mailbox busy bit is busy for more than 2 seconds */
    }

    retVal = MYKONOS_waitArmCmdStatus(device, MYKONOS_ARM_RADIOON_OPCODE, timeoutMs, &cmdStatusByte);
    if (retVal != MYKONOS_ERR_OK)
    {
        if (cmdStatusByte > 0)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_ARM_RADIOON_FAILED,
                    getMykonosErrorMessage(MYKONOS_ERR_ARM_RADIOON_FAILED));
            return MYKONOS_ERR_ARM_RADIOON_FAILED;
        }

        return retVal; /* Will return if timeout occurred */
    }

    /* If ARM command error flag is 2, the command was not accepted, RUN_INIT must complete first */
    if (cmdStatusByte > 0)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_ARM_RADIOON_FAILED, getMykonosErrorMessage(MYKONOS_ERR_ARM_RADIOON_FAILED));
        return MYKONOS_ERR_ARM_RADIOON_FAILED;
    }

    return MYKONOS_ERR_OK;
}

/**
 * \brief Instructs the ARM processor to move the radio state to the off state
 *
 * When the ARM moves from the Radio On state to Radio Off (Idle) the ARM tracking calibrations
 * are stopped and the TxEnable/RxEnable, etc GPIO control pins will be ignored.  This will also
 * keep the receive and transmit chains powered down until the MYKONOS_radioOn() function
 * is called again.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device Pointer to the device settings structure
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_ARM_RADIOOFF_FAILED ARM returned error running this command
 */
mykonosErr_t MYKONOS_radioOff(mykonosDevice_t *device)
{
    mykonosErr_t retVal = MYKONOS_ERR_OK;
    const uint8_t ABORTCAL_OPCODE = 0x00;
    uint32_t timeoutMs = 1000; //1 second timeout
    uint8_t cmdStatusByte = 0;

    retVal = MYKONOS_sendArmCommand(device, ABORTCAL_OPCODE, 0, 0);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal; /* Will return if mailbox busy bit is busy for more than 2 seconds */
    }

    retVal = MYKONOS_waitArmCmdStatus(device, ABORTCAL_OPCODE, timeoutMs, &cmdStatusByte);
    if (retVal != MYKONOS_ERR_OK)
    {
        if (cmdStatusByte > 0)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_ARM_RADIOOFF_FAILED,
                    getMykonosErrorMessage(MYKONOS_ERR_ARM_RADIOOFF_FAILED));
            return MYKONOS_ERR_ARM_RADIOOFF_FAILED;
        }

        return retVal; /* Will return if timeout occurred */
    }

    if (cmdStatusByte > 0)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_ARM_RADIOOFF_FAILED, getMykonosErrorMessage(MYKONOS_ERR_ARM_RADIOOFF_FAILED));
        return MYKONOS_ERR_ARM_RADIOOFF_FAILED;
    }

    return MYKONOS_ERR_OK;
}

/**
 * \brief Reads the current ARM radio state
 *
 * Currently, *radioStatus only returns data in the lower 8 bits, but
 * is defined as a 32bit status to allow for more information to be returned
 * in the future.
 *
 * radioStatus  |  Bitfield
 * -------------|------------------
 *        [1:0] | State[1:0], 0=POWERUP, 1=READY, 2=INIT, 3=RADIO ON
 *        [3:2] | unused
 *        [4]   | TDD_nFDD , 1= TDD, 0=FDD
 *        [7:5] | unused
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device Pointer to the device settings structure
 * \param radioStatus The current ARM radio state is returned in this parameter
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_GETRADIOSTATE_NULL_PARAM Function parameter radioStatus has a NULL pointer
 */
mykonosErr_t MYKONOS_getRadioState(mykonosDevice_t *device, uint32_t *radioStatus)
{
    uint8_t status = 0;

    /* if radioStatus is not null */
    if (radioStatus != NULL)
    {
        CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_ARM_OPCODE_STATE_0, &status);
        *radioStatus = (uint32_t)status;
    }
    else
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETRADIOSTATE_NULL_PARAM,
                getMykonosErrorMessage(MYKONOS_ERR_GETRADIOSTATE_NULL_PARAM));
        return MYKONOS_ERR_GETRADIOSTATE_NULL_PARAM;
    }

    return MYKONOS_ERR_OK;
}

/**
 * \brief Sets which ARM tracking cals are enabled during the radioOn state.
 *
 * This command must be called during radioOff state.  If called during radioOn,
 * an error will be returned.  The enum mykonosTrackingCalibrations_t can be used to
 * OR together to form the enableMask parameter.
 *
 * enableMask  |  Bit description
 * ------------|------------
 *        [0]  | TRACK_RX1_QEC
 *        [1]  | TRACK_RX2_QEC
 *        [2]  | TRACK_ORX1_QEC
 *        [3]  | TRACK_ORX2_QEC
 *        [4]  | TRACK_TX1_LOL
 *        [5]  | TRACK_TX2_LOL
 *        [6]  | TRACK_TX1_QEC
 *        [7]  | TRACK_TX2_QEC
 *        [8]  | TRACK_TX1_DPD
 *        [9]  | TRACK_TX2_DPD
 *       [10]  | TRACK_TX1_CLGC
 *       [11]  | TRACK_TX2_CLGC
 *       [12]  | TRACK_TX1_VSWR
 *       [13]  | TRACK_TX2_VSWR
 *       [16]  | TRACK_ORX1_QEC_SNLO
 *       [17]  | TRACK_ORX2_QEC_SNLO
 *       [18]  | TRACK_SRX_QEC
 *
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device Pointer to the device settings structure
 * \param enableMask A bitmask that selects which cals to run during radioOn state.
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_EN_TRACKING_CALS_ARMSTATE_ERROR Error: Tracking cals can only be enabled in the radioOff state.
 * \retval MYKONOS_ERR_EN_TRACKING_CALS_ARMERROR ARM returned error when executing this command
 */
mykonosErr_t MYKONOS_enableTrackingCals(mykonosDevice_t *device, uint32_t enableMask)
{
    mykonosErr_t retVal = MYKONOS_ERR_OK;
    uint8_t armData[4] = {0, 0, 0, 0};
    uint8_t extData[4] = {MYKONOS_ARM_OBJECTID_CALSCHEDULER, 0x00, 0x00, 0x04}; //Target ARM Object ID, Offset LSB, Offset MSB, Length
    uint32_t timeoutMs = 0;
    uint8_t cmdStatusByte = 0;
    uint32_t radioStatus = 0;
    uint8_t allowTx1AttenUpdates = 0;
    uint8_t allowTx2AttenUpdates = 0;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_enableTrackingCals()\n");
#endif

    /* read radio state to make sure ARM is in radioOff /IDLE */
    retVal = MYKONOS_getRadioState(device, &radioStatus);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    /* throw error if not in radioOff/IDLE state */
    if ((radioStatus & 0x03) != MYKONOS_ARM_SYSTEMSTATE_IDLE)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_EN_TRACKING_CALS_ARMSTATE_ERROR,
                getMykonosErrorMessage(MYKONOS_ERR_EN_TRACKING_CALS_ARMSTATE_ERROR));
        return MYKONOS_ERR_EN_TRACKING_CALS_ARMSTATE_ERROR;
    }

    /* In the ARM, DPD tracking and CLGC tracking share the same cal.  Must
     * set extra enable bits in ARM Memory to tell which cal to run. */
    retVal = enableDpdTracking(device, ((enableMask & TRACK_TX1_DPD) ? 1 : 0), ((enableMask & TRACK_TX2_DPD) ? 1 : 0));
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    if (((device->profilesValid & TX_PROFILE_VALID) > 0) && (device->tx->clgcConfig != NULL))
    {
        allowTx1AttenUpdates = device->tx->clgcConfig->allowTx1AttenUpdates;
        allowTx2AttenUpdates = device->tx->clgcConfig->allowTx2AttenUpdates;
    }
    else
    {
        allowTx1AttenUpdates = 0;
        allowTx2AttenUpdates = 0;
    }

    retVal = enableClgcTracking(device, ((allowTx1AttenUpdates > 0) ? 1 : 0), ((allowTx2AttenUpdates > 0) ? 1 : 0));
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    armData[0] = (uint8_t)(enableMask & 0xFF);
    armData[1] = (uint8_t)((enableMask >> 8) & 0xFF);
    armData[2] = (uint8_t)((enableMask >> 16) & 0xFF);
    armData[3] = (uint8_t)((enableMask >> 24) & 0xFF);
    retVal = MYKONOS_writeArmMem(device, MYKONOS_ADDR_ARM_START_DATA_ADDR, &armData[0], sizeof(armData));
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    retVal = MYKONOS_sendArmCommand(device, MYKONOS_ARM_WRITECFG_OPCODE, &extData[0], sizeof(extData));
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    timeoutMs = 1000;
    retVal = MYKONOS_waitArmCmdStatus(device, MYKONOS_ARM_WRITECFG_OPCODE, timeoutMs, &cmdStatusByte);
    if (retVal != MYKONOS_ERR_OK)
    {
        if (cmdStatusByte > 0)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_EN_TRACKING_CALS_ARMERROR,
                    getMykonosErrorMessage(MYKONOS_ERR_EN_TRACKING_CALS_ARMERROR));
            return MYKONOS_ERR_EN_TRACKING_CALS_ARMERROR;
        }

        return retVal;
    }

    if (cmdStatusByte > 0)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_EN_TRACKING_CALS_ARMERROR,
                getMykonosErrorMessage(MYKONOS_ERR_EN_TRACKING_CALS_ARMERROR));
        return MYKONOS_ERR_EN_TRACKING_CALS_ARMERROR;
    }

    return MYKONOS_ERR_OK;
}

/**
 * \brief Reschedules a tracking calibration to run. Can be used to
 * override the tracking calibration timer and force a tracking calibration to run.
 * Can be used to reschedule a tracking calibration after a tracking calibration
 * error has been detected.  Only one tracking calibration object can be scheduled
 * per channel per function call.
 *
 * \pre Command can be called in either Radio On or Radio Off state.  ARM must be
 * initialized.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device Pointer to the device settings structure
 * \param trackingCal Selects the tracking calibration to schedule.
 *
 * \retval MYKONOS_ERR_RESCHEDULE_TRACK_CAL_INV Not valid calibration passed
 * \retval MYKONOS_ERR_RESCHEDULE_TRACK_ARMERRFLAG ARM error
 * \retval MYKONOS_ERR_OK Function completed successfully
 */
mykonosErr_t MYKONOS_rescheduleTrackingCal(mykonosDevice_t *device, mykonosTrackingCalibrations_t trackingCal)
{
    uint32_t retVal = MYKONOS_ERR_OK;
    uint8_t extData[3] = {0};
    uint8_t cmdStatusByte = 0;
    uint32_t timeoutMs = 1000;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_rescheduleTrackingCal()\n");
#endif

    switch (trackingCal)
    {
        case TRACK_RX1_QEC:
            extData[1] = MYKONOS_ARM_OBJECTID_RXQEC_TRACKING;
            extData[2] = 0;
            break;

        case TRACK_RX2_QEC:
            extData[1] = MYKONOS_ARM_OBJECTID_RXQEC_TRACKING;
            extData[2] = 1;
            break;

        case TRACK_ORX1_QEC:
            extData[1] = MYKONOS_ARM_OBJECTID_ORXQEC_TRACKING;
            extData[2] = 0;
            break;

        case TRACK_ORX2_QEC:
            extData[1] = MYKONOS_ARM_OBJECTID_ORXQEC_TRACKING;
            extData[2] = 1;
            break;

        case TRACK_TX1_LOL:
            extData[1] = MYKONOS_ARM_OBJECTID_TXLOL_TRACKING;
            extData[2] = 0;
            break;

        case TRACK_TX2_LOL:
            extData[1] = MYKONOS_ARM_OBJECTID_TXLOL_TRACKING;
            extData[2] = 1;
            break;

        case TRACK_TX1_QEC:
            extData[1] = MYKONOS_ARM_OBJECTID_TXQEC_TRACKING;
            extData[2] = 0;
            break;

        case TRACK_TX2_QEC:
            extData[1] = MYKONOS_ARM_OBJECTID_TXQEC_TRACKING;
            extData[2] = 1;
            break;

        case TRACK_TX1_DPD:
            extData[1] = MYKONOS_ARM_OBJECTID_DPDCONFIG;
            extData[2] = 0;
            break;

        case TRACK_TX2_DPD:
            extData[1] = MYKONOS_ARM_OBJECTID_DPDCONFIG;
            extData[2] = 1;
            break;

        case TRACK_TX1_CLGC:
            extData[1] = MYKONOS_ARM_OBJECTID_CLGCCONFIG;
            extData[2] = 0;
            break;

        case TRACK_TX2_CLGC:
            extData[1] = MYKONOS_ARM_OBJECTID_CLGCCONFIG;
            extData[2] = 1;
            break;

        case TRACK_TX1_VSWR:
            extData[1] = MYKONOS_ARM_OBJECTID_VSWRCONFIG;
            extData[2] = 0;
            break;

        case TRACK_TX2_VSWR:
            extData[1] = MYKONOS_ARM_OBJECTID_VSWRCONFIG;
            extData[2] = 1;
            break;

        default:
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_RESCHEDULE_TRACK_CAL_INV,
                    getMykonosErrorMessage(MYKONOS_ERR_RESCHEDULE_TRACK_CAL_INV));
            return MYKONOS_ERR_RESCHEDULE_TRACK_CAL_INV;
    }

    extData[0] = MYKONOS_ARM_OBJECTID_TRACKING_CAL_PENDING;

    /* sending ARM command */
    retVal = MYKONOS_sendArmCommand(device, MYKONOS_ARM_SET_OPCODE, &extData[0], sizeof(extData));
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    /* check for completion */
    retVal = MYKONOS_waitArmCmdStatus(device, MYKONOS_ARM_SET_OPCODE, timeoutMs, &cmdStatusByte);
    if (retVal != MYKONOS_ERR_OK)
    {
        if (cmdStatusByte > 0)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_RESCHEDULE_TRACK_ARMERRFLAG,
                    getMykonosErrorMessage(MYKONOS_ERR_RESCHEDULE_TRACK_ARMERRFLAG));
            return MYKONOS_ERR_RESCHEDULE_TRACK_ARMERRFLAG;
        }

        return retVal;
    }

    if (cmdStatusByte > 0)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_RESCHEDULE_TRACK_ARMERRFLAG,
                getMykonosErrorMessage(MYKONOS_ERR_RESCHEDULE_TRACK_ARMERRFLAG));
        return MYKONOS_ERR_RESCHEDULE_TRACK_ARMERRFLAG;
    }

    return retVal;
}

/**
 * \brief Suspend or resume tracking calibrations in RADIO_ON.
 *
 * This function is used to suspend or resume active tracking calibrations based on the passed mask trackingCals.
 *
 * \pre Command can be called in Radio On.
 *
 * trackCals[bit]  |  Bit description
 * ----------------|------------
 *       [0]       | TRACK_RX1_QEC
 *       [1]       | TRACK_RX2_QEC
 *       [2]       | TRACK_ORX1_QEC
 *       [3]       | TRACK_ORX2_QEC
 *       [4]       | TRACK_TX1_LOL
 *       [5]       | TRACK_TX2_LOL
 *       [6]       | TRACK_TX1_QEC
 *       [7]       | TRACK_TX2_QEC
 *       [8]       | TRACK_TX1_DPD
 *       [9]       | TRACK_TX2_DPD
 *      [10]       | TRACK_TX1_CLGC
 *      [11]       | TRACK_TX2_CLGC
 *      [12]       | TRACK_TX1_VSWR
 *      [13]       | TRACK_TX2_VSWR
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device Pointer to the device settings structure
 * \param trackCals Selects the tracking calibrations to suspend or resume during the radio ON state.
 * mykonosTrackingCalibrations_t enumerated types are or'd together to form the tracking calibration
 * mask word. If the bit is high the calibration will resume, if the bit is low the calibration will be suspended.
 *
 * \retval MYKONOS_ERR_SETSTATEALL_TRACK_CAL_INV Not valid calibration mask passed for trackCals
 * \retval MYKONOS_ERR_SETSTATEALL_TRACK_ARMERRFLAG ARM error
 * \retval MYKONOS_ERR_OK Function completed successfully
 */
mykonosErr_t MYKONOS_setAllTrackCalState(mykonosDevice_t *device, uint32_t trackCals)
{
    uint32_t retVal = MYKONOS_ERR_OK;
    uint8_t cfgData[4] = {0};
    uint8_t extData[3] = {MYKONOS_ARM_OBJECTID_TRACKING_CAL_SUSPEND_RESUME, 0x0F, 0};
    uint8_t cmdStatusByte = 0;
    uint32_t enTrackCal = 0x00;
    uint32_t timeoutMs = 1000;

    const uint32_t TRACKING_MASK = 0x3FFF;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_setAllTrackCalState()\n");
#endif

    /* reading enabled tracking calibrations */
    retVal = MYKONOS_getEnabledTrackingCals(device, &enTrackCal);

    /* trackingCalMask check */
    if (((trackCals | enTrackCal) > enTrackCal) || (trackCals > TRACKING_MASK))
    {
        /* invalid cal mask error return, tracking cal not enable so we can not resume it */
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SETSTATEALL_TRACK_CAL_INV,
                getMykonosErrorMessage(MYKONOS_ERR_SETSTATEALL_TRACK_CAL_INV));
        return MYKONOS_ERR_SETSTATEALL_TRACK_CAL_INV;
    }

    /* convert tracking mask to array of uint8_t type */
    cfgData[0] = (uint8_t)(trackCals & 0xFF);
    cfgData[1] = (uint8_t)((trackCals >> 8) & 0xFF);
    cfgData[2] = (uint8_t)((trackCals >> 16) & 0x0FF);
    cfgData[3] = (uint8_t)((trackCals >> 24) & 0xFF);

    retVal = MYKONOS_writeArmMem(device, MYKONOS_ADDR_ARM_START_DATA_ADDR, &cfgData[0], sizeof(cfgData));

    retVal = MYKONOS_sendArmCommand(device, MYKONOS_ARM_SET_OPCODE, &extData[0], sizeof(extData));

    /* check for completion */
    retVal = MYKONOS_waitArmCmdStatus(device, MYKONOS_ARM_SET_OPCODE, timeoutMs, &cmdStatusByte);
    if (retVal != MYKONOS_ERR_OK)
    {
        if (cmdStatusByte > 0)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SETSTATEALL_TRACK_ARMERRFLAG,
                    getMykonosErrorMessage(MYKONOS_ERR_SETSTATEALL_TRACK_ARMERRFLAG));
            return MYKONOS_ERR_SETSTATEALL_TRACK_ARMERRFLAG;
        }

        return retVal;
    }

    if (cmdStatusByte > 0)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SETSTATEALL_TRACK_ARMERRFLAG,
                getMykonosErrorMessage(MYKONOS_ERR_SETSTATEALL_TRACK_ARMERRFLAG));
        return MYKONOS_ERR_SETSTATEALL_TRACK_ARMERRFLAG;
    }

    return retVal;
}

/**
 * \brief Get the Suspended or Resumed state for tracking calibrations
 *
 * This function is used to get the suspend or resume state of all active tracking calibrations and the state is stored in trackCals.
 *
 * \pre Command can be called in Radio On.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device Pointer to the device settings structure
 * \param trackCals pointer to store the tracking calibration state.
 * If the bit is set then the tracking calibration is resumed and if not set then the tracking cal is suspended, the bit field follows:
 *
 * trackCals[bit] |  Bit description
 * ---------------|------------
 *       [0]      | TRACK_RX1_QEC
 *       [1]      | TRACK_RX2_QEC
 *       [2]      | TRACK_ORX1_QEC
 *       [3]      | TRACK_ORX2_QEC
 *       [4]      | TRACK_TX1_LOL
 *       [5]      | TRACK_TX2_LOL
 *       [6]      | TRACK_TX1_QEC
 *       [7]      | TRACK_TX2_QEC
 *       [8]      | TRACK_TX1_DPD
 *       [9]      | TRACK_TX2_DPD
 *      [10]      | TRACK_TX1_CLGC
 *      [11]      | TRACK_TX2_CLGC
 *      [12]      | TRACK_TX1_VSWR
 *      [13]      | TRACK_TX2_VSWR
 *
 * \retval MYKONOS_ERR_GETSTATEALL_TRACK_NULL_PARAM Null parameter passed for trackCals
 * \retval MYKONOS_ERR_GETSTATEALL_TRACK_ARMERRFLAG ARM error flag set.
 * \retval MYKONOS_ERR_GETSTATEALL_TRACK_ARMERROR ARM command error.
 * \retval MYKONOS_ERR_OK Function completed successfully
 */
mykonosErr_t MYKONOS_getAllTrackCalState(mykonosDevice_t *device, uint32_t *trackCals)
{
    uint32_t retVal = MYKONOS_ERR_OK;
    uint8_t extData = MYKONOS_ARM_OBJECTID_TRACKING_CAL_SUSPEND_RESUME;
    uint8_t armData[4] = {0};
    uint8_t cmdStatusByte = 0;
    uint32_t timeoutMs = 1000;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_getAllTrackCalState()\n");
#endif

    /* Check for passed parameter */
    if (trackCals == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETSTATEALL_TRACK_NULL_PARAM,
                getMykonosErrorMessage(MYKONOS_ERR_GETSTATEALL_TRACK_NULL_PARAM));
        return MYKONOS_ERR_GETSTATEALL_TRACK_NULL_PARAM;
    }

    /* sending ARM command */
    retVal = MYKONOS_sendArmCommand(device, MYKONOS_ARM_GET_OPCODE, &extData, sizeof(extData));
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    /* check for completion */
    retVal = MYKONOS_waitArmCmdStatus(device, MYKONOS_ARM_GET_OPCODE, timeoutMs, &cmdStatusByte);
    if (retVal != MYKONOS_ERR_OK)
    {
        if (cmdStatusByte > 0)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETSTATEALL_TRACK_ARMERRFLAG,
                    getMykonosErrorMessage(MYKONOS_ERR_GETSTATEALL_TRACK_ARMERRFLAG));
            return MYKONOS_ERR_GETSTATEALL_TRACK_ARMERRFLAG;
        }

        return retVal;
    }

    if (cmdStatusByte > 0)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETSTATEALL_TRACK_ARMERRFLAG,
                getMykonosErrorMessage(MYKONOS_ERR_GETSTATEALL_TRACK_ARMERRFLAG));
        return MYKONOS_ERR_GETSTATEALL_TRACK_ARMERRFLAG;
    }

    /* read 32-bit tracking state from ARM memory */
    retVal = MYKONOS_readArmMem(device, MYKONOS_ADDR_ARM_START_DATA_ADDR, &armData[0], sizeof(armData), 1);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    if (cmdStatusByte > 0)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETSTATEALL_TRACK_ARMERROR,
                getMykonosErrorMessage(MYKONOS_ERR_GETSTATEALL_TRACK_ARMERROR));
        return MYKONOS_ERR_GETSTATEALL_TRACK_ARMERROR;
    }

    *trackCals = (uint32_t)armData[0] | ((uint32_t)armData[1] << 8) | ((uint32_t)armData[2] << 16) | ((uint32_t)armData[3] << 24);

    return retVal;
}

/**
 * \brief Suspend or resume individual tracking calibration
 *
 * \pre The tracking calibration must have been enabled with MYKONOS_enableTrackingCals(), this command can be called in Radio On.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device Pointer to the device settings structure
 * \param trackingCal Selects the tracking calibration to resume or suspend.
 * \param trackCalState if set then the selected tracking calibration will be resumed and if not set then the tracking cal will be suspended
 *
 * \retval MYKONOS_ERR_SETSTATE_TRACK_CAL_INV Not valid calibration passed
 * \retval MYKONOS_ERR_SETSTATE_TRACK_ARMERRFLAG ARM error
 * \retval MYKONOS_ERR_OK Function completed successfully
 */
mykonosErr_t MYKONOS_setTrackingCalState(mykonosDevice_t *device, mykonosTrackingCalibrations_t trackingCal, uint8_t trackCalState)
{
    uint32_t retVal = MYKONOS_ERR_OK;
    uint8_t extData[2] = {0};
    uint8_t cmdStatusByte = 0;
    uint8_t suspendTrack = 0x0F;
    uint32_t timeoutMs = 1000;
    uint32_t enTrackCal = 0x00;

    const uint8_t CHANNEL_1 = 0x00;
    const uint8_t CHANNEL_2 = 0x10;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_setTrackingCalState()\n");
#endif

    if (trackCalState > 0)
    {
        suspendTrack = 0x2F;
    }

    /* reading enabled tracking calibrations */
    retVal = MYKONOS_getEnabledTrackingCals(device, &enTrackCal);

    /* trackingCalMask check */
    if ((trackingCal & enTrackCal) != trackingCal)
    {
        /* invalid cal mask error return, tracking cal not enable so we can not resume or suspend it */
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SETSTATE_TRACK_CAL_INV,
                getMykonosErrorMessage(MYKONOS_ERR_SETSTATE_TRACK_CAL_INV));
        return MYKONOS_ERR_SETSTATE_TRACK_CAL_INV;
    }

    switch (trackingCal)
    {
        case TRACK_RX1_QEC:
            extData[1] = MYKONOS_ARM_OBJECTID_RXQEC_TRACKING & suspendTrack;
            extData[1] |= CHANNEL_1;
            break;

        case TRACK_RX2_QEC:
            extData[1] = MYKONOS_ARM_OBJECTID_RXQEC_TRACKING & suspendTrack;
            extData[1] |= CHANNEL_2;
            break;

        case TRACK_ORX1_QEC:
            extData[1] = MYKONOS_ARM_OBJECTID_ORXQEC_TRACKING & suspendTrack;
            extData[1] |= CHANNEL_1;
            break;

        case TRACK_ORX2_QEC:
            extData[1] = MYKONOS_ARM_OBJECTID_ORXQEC_TRACKING & suspendTrack;
            extData[1] |= CHANNEL_2;
            break;

        case TRACK_TX1_LOL:
            extData[1] = MYKONOS_ARM_OBJECTID_TXLOL_TRACKING & suspendTrack;
            extData[1] |= CHANNEL_1;
            break;

        case TRACK_TX2_LOL:
            extData[1] = MYKONOS_ARM_OBJECTID_TXLOL_TRACKING & suspendTrack;
            extData[1] |= CHANNEL_2;
            break;

        case TRACK_TX1_QEC:
            extData[1] = MYKONOS_ARM_OBJECTID_TXQEC_TRACKING & suspendTrack;
            extData[1] |= CHANNEL_1;
            break;

        case TRACK_TX2_QEC:
            extData[1] = MYKONOS_ARM_OBJECTID_TXQEC_TRACKING & suspendTrack;
            extData[1] |= CHANNEL_2;
            break;

        case TRACK_TX1_DPD:
            extData[1] = MYKONOS_ARM_OBJECTID_DPDCONFIG & suspendTrack;
            extData[1] |= CHANNEL_1;
            break;

        case TRACK_TX2_DPD:
            extData[1] = MYKONOS_ARM_OBJECTID_DPDCONFIG & suspendTrack;
            extData[1] |= CHANNEL_2;
            break;

        case TRACK_TX1_CLGC:
            extData[1] = MYKONOS_ARM_OBJECTID_CLGCCONFIG & suspendTrack;
            extData[1] |= CHANNEL_1;
            break;

        case TRACK_TX2_CLGC:
            extData[1] = MYKONOS_ARM_OBJECTID_CLGCCONFIG & suspendTrack;
            extData[1] |= CHANNEL_2;
            break;

        case TRACK_TX1_VSWR:
            extData[1] = MYKONOS_ARM_OBJECTID_VSWRCONFIG & suspendTrack;
            extData[1] |= CHANNEL_1;
            break;

        case TRACK_TX2_VSWR:
            extData[1] = MYKONOS_ARM_OBJECTID_VSWRCONFIG & suspendTrack;
            extData[1] |= CHANNEL_2;
            break;

        default:
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SETSTATE_TRACK_CAL_INV,
                    getMykonosErrorMessage(MYKONOS_ERR_SETSTATE_TRACK_CAL_INV));
            return MYKONOS_ERR_SETSTATE_TRACK_CAL_INV;
    }

    extData[0] = MYKONOS_ARM_OBJECTID_TRACKING_CAL_SUSPEND_RESUME;

    /* sending ARM command */
    retVal = MYKONOS_sendArmCommand(device, MYKONOS_ARM_SET_OPCODE, &extData[0], sizeof(extData));
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    /* check for completion */
    retVal = MYKONOS_waitArmCmdStatus(device, MYKONOS_ARM_SET_OPCODE, timeoutMs, &cmdStatusByte);
    if (retVal != MYKONOS_ERR_OK)
    {
        if (cmdStatusByte > 0)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SETSTATE_TRACK_ARMERRFLAG,
                    getMykonosErrorMessage(MYKONOS_ERR_SETSTATE_TRACK_ARMERRFLAG));
            return MYKONOS_ERR_SETSTATE_TRACK_ARMERRFLAG;
        }

        return retVal;
    }

    if (cmdStatusByte > 0)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SETSTATE_TRACK_ARMERRFLAG,
                getMykonosErrorMessage(MYKONOS_ERR_SETSTATE_TRACK_ARMERRFLAG));
        return MYKONOS_ERR_SETSTATE_TRACK_ARMERRFLAG;
    }

    return retVal;
}

/**
 * \brief Get the Suspended or Resumed state for individual tracking calibration
 *
 * \pre Command can be called in Radio On.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device Pointer to the device settings structure
 * \param trackingCal Selects the tracking calibration to get the resumed or suspended state.
 * \param trackCalState pointer to store the tracking calibration state,
 * if set then the selected tracking calibration is resumed and if not set then the tracking cal is suspended
 *
 * \retval MYKONOS_ERR_GETSTATE_TRACK_NULL_PARAM Null parameter passed to trackCalState
 * \retval MYKONOS_ERR_GETSTATE_TRACK_ARMERRFLAG ARM command error flag set.
 * \retval MYKONOS_ERR_GETSTATE_TRACK_ARMERROR ARM command error.
 * \retval MYKONOS_ERR_OK Function completed successfully
 */
mykonosErr_t MYKONOS_getTrackingCalState(mykonosDevice_t *device, mykonosTrackingCalibrations_t trackingCal, uint8_t *trackCalState)
{
    uint32_t retVal = MYKONOS_ERR_OK;
    uint8_t extData = MYKONOS_ARM_OBJECTID_TRACKING_CAL_SUSPEND_RESUME;
    uint8_t armData[4] = {0};
    uint8_t cmdStatusByte = 0;
    uint32_t timeoutMs = 1000;
    uint32_t trackMask = 0x00;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_getTrackingCalState()\n");
#endif

    /* Check for passed parameter */
    if (trackCalState == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETSTATE_TRACK_NULL_PARAM,
                getMykonosErrorMessage(MYKONOS_ERR_GETSTATE_TRACK_NULL_PARAM));
        return MYKONOS_ERR_GETSTATE_TRACK_NULL_PARAM;
    }

    /* sending ARM command */
    retVal = MYKONOS_sendArmCommand(device, MYKONOS_ARM_GET_OPCODE, &extData, sizeof(extData));
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    /* check for completion */
    retVal = MYKONOS_waitArmCmdStatus(device, MYKONOS_ARM_GET_OPCODE, timeoutMs, &cmdStatusByte);
    if (retVal != MYKONOS_ERR_OK)
    {
        if (cmdStatusByte > 0)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETSTATE_TRACK_ARMERRFLAG,
                    getMykonosErrorMessage(MYKONOS_ERR_GETSTATE_TRACK_ARMERRFLAG));
            return MYKONOS_ERR_GETSTATE_TRACK_ARMERRFLAG;
        }

        return retVal;
    }

    if (cmdStatusByte > 0)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETSTATE_TRACK_ARMERRFLAG,
                getMykonosErrorMessage(MYKONOS_ERR_GETSTATE_TRACK_ARMERRFLAG));
        return MYKONOS_ERR_GETSTATE_TRACK_ARMERRFLAG;
    }

    /* read 32-bit tracking state from ARM memory */
    retVal = MYKONOS_readArmMem(device, MYKONOS_ADDR_ARM_START_DATA_ADDR, &armData[0], sizeof(armData), 1);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    if (cmdStatusByte > 0)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETSTATE_TRACK_ARMERROR,
                getMykonosErrorMessage(MYKONOS_ERR_GETSTATE_TRACK_ARMERROR));
        return MYKONOS_ERR_GETSTATE_TRACK_ARMERROR;
    }

    trackMask = (uint32_t)armData[0] | ((uint32_t)armData[1] << 8) | ((uint32_t)armData[2] << 16) | ((uint32_t)armData[3] << 24);

    if (trackMask & trackingCal)
    {
        *trackCalState = 1;
    }
    else
    {
        *trackCalState = 0;
    }

    return retVal;
}

/**
 * \brief Reads back which ARM tracking cals are enabled
 *
 * enableMask  |  Bit description
 * ------------|----------------------
 *        [0]  | TRACK_RX1_QEC
 *        [1]  | TRACK_RX2_QEC
 *        [2]  | TRACK_ORX1_QEC
 *        [3]  | TRACK_ORX2_QEC
 *        [4]  | TRACK_TX1_LOL
 *        [5]  | TRACK_TX2_LOL
 *        [6]  | TRACK_TX1_QEC
 *        [7]  | TRACK_TX2_QEC
 *        [8]  | TRACK_TX1_DPD
 *        [9]  | TRACK_TX2_DPD
 *       [10]  | TRACK_TX1_CLGC
 *       [11]  | TRACK_TX2_CLGC
 *       [12]  | TRACK_TX1_VSWR
 *       [13]  | TRACK_TX2_VSWR
 *       [16]  | TRACK_ORX1_QEC_SNLO
 *       [17]  | TRACK_ORX1_QEC_SNLO
 *       [18]  | TRACK_SRX_QEC
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device Pointer to the device settings structure
 * \param enableMask Returned bitmask that shows which tracking cals are enabled
 *                   to run during radioOn state. See mykonosTrackingCalibrations_t enum of tracking cals.
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 */
mykonosErr_t MYKONOS_getEnabledTrackingCals(mykonosDevice_t *device, uint32_t *enableMask)
{
    mykonosErr_t retVal = MYKONOS_ERR_OK;
    uint8_t armData[4] = {0};

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_getEnabledTrackingCals()\n");
#endif

    /* Ask ARM to read tracking cal enable bits and place in mailbox buffer memory */
    retVal = MYKONOS_readArmConfig(device, MYKONOS_ARM_OBJECTID_CALSCHEDULER, 0, &armData[0], 4);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    *enableMask = ((uint32_t)(armData[0]) | ((uint32_t)(armData[1]) << 8) | ((uint32_t)(armData[2]) << 16) | ((uint32_t)(armData[3]) << 24));

    return MYKONOS_ERR_OK;
}

/**
 * \brief Returns the tracking calibration pending and error status.
 *
 * When in radioOn state, the enabled tracking calibrations will set the pending
 * flag when the particular calibration is ready to be run, but has not completed
 * yet. For Tx tracking cals to complete, the BBIC must set the ObsRx path to
 * the INTERNAL CALS mode.  If a tracking calibration had an error, the corresponding
 * error flag will be asserted.
 *
 *  pendingCalMask | bit Description
 * ----------------|--------------------------------------------------------
 *            [0]  | Rx1 QEC tracking pending
 *            [1]  | Rx1 QEC tracking error
 *            [2]  | Rx2 QEC tracking pending
 *            [3]  | Rx2 QEC tracking error
 *            [4]  | ORx1 QEC tracking pending
 *            [5]  | ORx1 QEC tracking error
 *            [6]  | ORx2 QEC tracking pending
 *            [7]  | ORx2 QEC tracking error
 *            [8]  | Tx1 LOL tracking pending
 *            [9]  | Tx1 LOL tracking error
 *           [10]  | Tx2 LOL tracking pending
 *           [11]  | Tx2 LOL tracking error
 *           [12]  | Tx1 QEC tracking pending
 *           [13]  | Tx1 QEC tracking error
 *           [14]  | Tx2 QEC tracking pending
 *           [15]  | Tx2 QEC tracking error
 *           [16]  | Tx1 DPD tracking pending
 *           [17]  | Tx1 DPD tracking error
 *           [18]  | Tx2 DPD tracking pending
 *           [19]  | Tx2 DPD tracking error
 *           [20]  | Tx1 CLGC tracking pending
 *           [21]  | Tx1 CLGC tracking error
 *           [22]  | Tx2 CLGC tracking pending
 *           [23]  | Tx2 CLGC tracking error
 *           [24]  | Tx1 VSWR tracking pending
 *           [25]  | Tx1 VSWR tracking error
 *           [26]  | Tx2 VSWR tracking pending
 *           [27]  | Tx2 VSWR tracking error
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device Pointer to the device settings structure
 * \param pendingCalMask Bit mask that describes which tracking cals are pending
 *                       or had errors
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_GETPENDTRKCALS_NULL_PARAM Function parameter pendingCalMask is a NULL pointer
 */
mykonosErr_t MYKONOS_getPendingTrackingCals(mykonosDevice_t *device, uint32_t *pendingCalMask)
{
    uint8_t readData[4] = {0};

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_getPendingTrackingCals()\n");
#endif

    if (pendingCalMask == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETPENDTRKCALS_NULL_PARAM,
                getMykonosErrorMessage(MYKONOS_ERR_GETPENDTRKCALS_NULL_PARAM));
        return MYKONOS_ERR_GETPENDTRKCALS_NULL_PARAM;
    }

    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_ARM_OPCODE_STATE_3, &readData[0]);
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_ARM_OPCODE_STATE_4, &readData[1]);
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_ARM_OPCODE_STATE_5, &readData[2]);
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_ARM_OPCODE_STATE_6, &readData[3]);
    *pendingCalMask = ((uint32_t)readData[3] << 24) | ((uint32_t)readData[2] << 16) | ((uint32_t)readData[1] << 8) | ((uint32_t)readData[0]);

    return MYKONOS_ERR_OK;
}

/**
 * \brief Returns the status of the TxLOL external tracking calibration
 *
 * The Tx LOL external tracking calibration is run during the radioOn state.
 * The function can be called to read back the status of the TxLOL external
 * calibration including metrics like error codes, percentage of data
 * collected for current cal, the performance of the cal and the number of
 * times the cal has run and updated the hardware.
 *
 * \pre Before the function is called, the device must be initialized, the ARM
 * loaded, and init cals run.  These functions can be called in radioOff or
 * radioOn state.
 *
 * \param device Pointer to the device settings structure
 * \param txChannel The channel (Tx1/Tx2) whose status is to be read back
 * \param txLolStatus Status of the TxLOL external calibration, as a structure
 * of type mykonosTxLolStatus_t is returned to this pointer address
 *
 * \retval MYKONOS_ERR_GETTXLOLSTATUS_NULLPARAM Function parameter mykonosTxLolStatus_t is a NULL pointer
 * \retval MYKONOS_ERR_GETTXLOLSTATUS_INV_CH Channel selection not valid
 * \retval MYKONOS_ERR_GETTXLOLSTATUS_ARMERRFLAG ARM command error
 * \retval MYKONOS_ERR_OK  Function completed successfully
 */
mykonosErr_t MYKONOS_getTxLolStatus(mykonosDevice_t *device, mykonosTxChannels_t txChannel, mykonosTxLolStatus_t *txLolStatus)
{
    uint8_t extData[3] = {MYKONOS_ARM_OBJECTID_CAL_STATUS, MYKONOS_ARM_OBJECTID_TXLOL_TRACKING, 0};
    uint8_t cmdStatusByte = 0;
    uint32_t offset = 0;
    uint32_t timeoutMs = 0;
    uint8_t armReadBack[20] = {0};
    mykonosErr_t retVal = MYKONOS_ERR_OK;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_getTxLolStatus()\n");
#endif

    if (txLolStatus == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETTXLOLSTATUS_NULLPARAM,
                getMykonosErrorMessage(MYKONOS_ERR_GETTXLOLSTATUS_NULLPARAM));
        return MYKONOS_ERR_GETTXLOLSTATUS_NULLPARAM;
    }

    switch (txChannel)
    {
        case TX1:
            extData[2] = 0;
            break;
        case TX2:
            extData[2] = 1;
            break;
        default:
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETTXLOLSTATUS_INV_CH,
                    getMykonosErrorMessage(MYKONOS_ERR_GETTXLOLSTATUS_INV_CH));
            return MYKONOS_ERR_GETTXLOLSTATUS_INV_CH;
        }
    }

    retVal = MYKONOS_sendArmCommand(device, MYKONOS_ARM_GET_OPCODE, &extData[0], sizeof(extData));
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    timeoutMs = 1000;
    retVal = MYKONOS_waitArmCmdStatus(device, MYKONOS_ARM_GET_OPCODE, timeoutMs, &cmdStatusByte);
    if (retVal != MYKONOS_ERR_OK)
    {
        if (cmdStatusByte > 0)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETTXLOLSTATUS_ARMERRFLAG,
                    getMykonosErrorMessage(MYKONOS_ERR_GETTXLOLSTATUS_ARMERRFLAG));
            return MYKONOS_ERR_GETTXLOLSTATUS_ARMERRFLAG;
        }

        return retVal;
    }

    if (cmdStatusByte > 0)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETTXLOLSTATUS_ARMERRFLAG,
                getMykonosErrorMessage(MYKONOS_ERR_GETTXLOLSTATUS_ARMERRFLAG));
        return MYKONOS_ERR_GETTXLOLSTATUS_ARMERRFLAG;
    }

    /* read status from ARM memory */
    offset = 0;
    retVal = MYKONOS_readArmMem(device, (MYKONOS_ADDR_ARM_START_DATA_ADDR + offset), &armReadBack[0], sizeof(armReadBack), 1);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    /* Assign to data structure */
    txLolStatus->errorCode = (((uint32_t)armReadBack[3]) << 24) | (((uint32_t)armReadBack[2]) << 16) | (((uint32_t)armReadBack[1]) << 8) | ((uint32_t)armReadBack[0]);
    txLolStatus->percentComplete = (((uint32_t)armReadBack[7]) << 24) | (((uint32_t)armReadBack[6]) << 16) | (((uint32_t)armReadBack[5]) << 8)
            | ((uint32_t)armReadBack[4]);
    txLolStatus->performanceMetric = (((uint32_t)armReadBack[11]) << 24) | (((uint32_t)armReadBack[10]) << 16) | (((uint32_t)armReadBack[9]) << 8)
            | ((uint32_t)armReadBack[8]);
    txLolStatus->iterCount = (((uint32_t)armReadBack[15]) << 24) | (((uint32_t)armReadBack[14]) << 16) | (((uint32_t)armReadBack[13]) << 8) | ((uint32_t)armReadBack[12]);
    txLolStatus->updateCount = (((uint32_t)armReadBack[19]) << 24) | (((uint32_t)armReadBack[18]) << 16) | (((uint32_t)armReadBack[17]) << 8)
            | ((uint32_t)armReadBack[16]);

    return MYKONOS_ERR_OK;
}

/**
 * \brief Returns the status of the TxQEC tracking calibration
 *
 * The Tx QEC tracking calibration is run during the radioOn state.
 * The function can be called to read back the status of the TxQEC
 * calibration including metrics like error codes, percentage of data
 * collected for current cal, the performance of the cal and the number of
 * times the cal has run and updated the hardware.
 *
 * \pre Before the function is called, the device must be initialized, the ARM
 * loaded, and init cals run.  These functions can be called in radioOff or
 * radioOn state.
 *
 * \param device Pointer to the device settings structure
 * \param txChannel The channel (Tx1/Tx2) whose status is to be read back
 * \param txQecStatus Status of the TxQEC external calibration, as a structure
 * of type mykonosTxQecStatus_t is returned to this pointer address
 *
 * \retval MYKONOS_ERR_GETTXQECSTATUS_NULLPARAM Function parameter txQecStatus is a NULL pointer
 * \retval MYKONOS_ERR_GETTXQECSTATUS_INV_CH Channel selection not valid
 * \retval MYKONOS_ERR_GETTXQECSTATUS_ARMERRFLAG ARM command error
 * \retval MYKONOS_ERR_OK  Function completed successfully
 */
mykonosErr_t MYKONOS_getTxQecStatus(mykonosDevice_t *device, mykonosTxChannels_t txChannel, mykonosTxQecStatus_t *txQecStatus)
{
    uint8_t extData[3] = {MYKONOS_ARM_OBJECTID_CAL_STATUS, MYKONOS_ARM_OBJECTID_TXQEC_TRACKING, 0};
    uint8_t cmdStatusByte = 0;
    uint32_t offset = 0;
    uint32_t timeoutMs = 0;
    uint8_t armReadBack[20] = {0};
    mykonosErr_t retVal = MYKONOS_ERR_OK;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_getTxQecStatus()\n");
#endif

    if (txQecStatus == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETTXQECSTATUS_NULLPARAM,
                getMykonosErrorMessage(MYKONOS_ERR_GETTXQECSTATUS_NULLPARAM));
        return MYKONOS_ERR_GETTXQECSTATUS_NULLPARAM;
    }

    switch (txChannel)
    {
        case TX1:
            extData[2] = 0;
            break;
        case TX2:
            extData[2] = 1;
            break;
        default:
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETTXQECSTATUS_INV_CH,
                    getMykonosErrorMessage(MYKONOS_ERR_GETTXQECSTATUS_INV_CH));
            return MYKONOS_ERR_GETTXQECSTATUS_INV_CH;
        }
    }

    retVal = MYKONOS_sendArmCommand(device, MYKONOS_ARM_GET_OPCODE, &extData[0], sizeof(extData));
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    timeoutMs = 1000;
    retVal = MYKONOS_waitArmCmdStatus(device, MYKONOS_ARM_GET_OPCODE, timeoutMs, &cmdStatusByte);
    if (retVal != MYKONOS_ERR_OK)
    {
        if (cmdStatusByte > 0)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETTXQECSTATUS_ARMERRFLAG,
                    getMykonosErrorMessage(MYKONOS_ERR_GETTXQECSTATUS_ARMERRFLAG));
            return MYKONOS_ERR_GETTXQECSTATUS_ARMERRFLAG;
        }

        return retVal;
    }

    if (cmdStatusByte > 0)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETTXQECSTATUS_ARMERRFLAG,
                getMykonosErrorMessage(MYKONOS_ERR_GETTXQECSTATUS_ARMERRFLAG));
        return MYKONOS_ERR_GETTXQECSTATUS_ARMERRFLAG;
    }

    /* read status from ARM memory */
    offset = 0;
    retVal = MYKONOS_readArmMem(device, (MYKONOS_ADDR_ARM_START_DATA_ADDR + offset), &armReadBack[0], sizeof(armReadBack), 1);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    /* Assign to data structure */
    txQecStatus->errorCode = (((uint32_t)armReadBack[3]) << 24) | (((uint32_t)armReadBack[2]) << 16) | (((uint32_t)armReadBack[1]) << 8) | ((uint32_t)armReadBack[0]);
    txQecStatus->percentComplete = (((uint32_t)armReadBack[7]) << 24) | (((uint32_t)armReadBack[6]) << 16) | (((uint32_t)armReadBack[5]) << 8)
            | ((uint32_t)armReadBack[4]);
    txQecStatus->performanceMetric = (((uint32_t)armReadBack[11]) << 24) | (((uint32_t)armReadBack[10]) << 16) | (((uint32_t)armReadBack[9]) << 8)
            | ((uint32_t)armReadBack[8]);
    txQecStatus->iterCount = (((uint32_t)armReadBack[15]) << 24) | (((uint32_t)armReadBack[14]) << 16) | (((uint32_t)armReadBack[13]) << 8) | ((uint32_t)armReadBack[12]);
    txQecStatus->updateCount = (((uint32_t)armReadBack[19]) << 24) | (((uint32_t)armReadBack[18]) << 16) | (((uint32_t)armReadBack[17]) << 8)
            | ((uint32_t)armReadBack[16]);

    return MYKONOS_ERR_OK;
}

/**
 * \brief Returns the status of the RxQEC tracking calibration
 *
 * The Rx QEC tracking calibration is run during the radioOn state.
 * The function can be called to read back the status of the RxQEC external
 * calibration including metrics like error codes, percentage of data
 * collected for current cal, the performance of the cal and the number of
 * times the cal has run and updated the hardware.
 *
 * \pre Before the function is called, the device must be initialized, the ARM
 * loaded, and init cals run.  These functions can be called in radioOff or
 * radioOn state.
 *
 * \param device Pointer to the device settings structure
 * \param rxChannel The channel (Rx1/Rx2) whose status is to be read back
 * \param rxQecStatus Status of the RxQEC calibration, as a structure
 * of type mykonosRxQecStatus_t is returned to this pointer address
 *
 * \retval MYKONOS_ERR_GETRXQECSTATUS_NULLPARAM Function parameter rxQecStatus is a NULL pointer
 * \retval MYKONOS_ERR_GETRXQECSTATUS_INV_CH Channel selection not valid
 * \retval MYKONOS_ERR_GETRXQECSTATUS_ARMERRFLAG ARM command error
 * \retval MYKONOS_ERR_OK  Function completed successfully
 */
mykonosErr_t MYKONOS_getRxQecStatus(mykonosDevice_t *device, mykonosRxChannels_t rxChannel, mykonosRxQecStatus_t *rxQecStatus)
{
    uint8_t extData[3] = {MYKONOS_ARM_OBJECTID_CAL_STATUS, MYKONOS_ARM_OBJECTID_RXQEC_TRACKING, 0};
    uint8_t cmdStatusByte = 0;
    uint32_t offset = 0;
    uint32_t timeoutMs = 0;
    uint8_t armReadBack[20] = {0};
    mykonosErr_t retVal = MYKONOS_ERR_OK;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_getRxQecStatus()\n");
#endif

    if (rxQecStatus == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETRXQECSTATUS_NULLPARAM,
                getMykonosErrorMessage(MYKONOS_ERR_GETRXQECSTATUS_NULLPARAM));
        return MYKONOS_ERR_GETRXQECSTATUS_NULLPARAM;
    }

    switch (rxChannel)
    {
        case RX1:
            extData[2] = 0;
            break;
        case RX2:
            extData[2] = 1;
            break;
        default:
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETRXQECSTATUS_INV_CH,
                    getMykonosErrorMessage(MYKONOS_ERR_GETRXQECSTATUS_INV_CH));
            return MYKONOS_ERR_GETRXQECSTATUS_INV_CH;
        }
    }

    retVal = MYKONOS_sendArmCommand(device, MYKONOS_ARM_GET_OPCODE, &extData[0], sizeof(extData));
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    timeoutMs = 1000;
    retVal = MYKONOS_waitArmCmdStatus(device, MYKONOS_ARM_GET_OPCODE, timeoutMs, &cmdStatusByte);
    if (retVal != MYKONOS_ERR_OK)
    {
        if (cmdStatusByte > 0)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETRXQECSTATUS_ARMERRFLAG,
                    getMykonosErrorMessage(MYKONOS_ERR_GETRXQECSTATUS_ARMERRFLAG));
            return MYKONOS_ERR_GETRXQECSTATUS_ARMERRFLAG;
        }

        return retVal;
    }

    if (cmdStatusByte > 0)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETRXQECSTATUS_ARMERRFLAG,
                getMykonosErrorMessage(MYKONOS_ERR_GETRXQECSTATUS_ARMERRFLAG));
        return MYKONOS_ERR_GETRXQECSTATUS_ARMERRFLAG;
    }

    /* read status from ARM memory */
    offset = 0;
    retVal = MYKONOS_readArmMem(device, (MYKONOS_ADDR_ARM_START_DATA_ADDR + offset), &armReadBack[0], sizeof(armReadBack), 1);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    /* Assign to data structure */
    rxQecStatus->errorCode = (((uint32_t)armReadBack[3]) << 24) | (((uint32_t)armReadBack[2]) << 16) | (((uint32_t)armReadBack[1]) << 8) | ((uint32_t)armReadBack[0]);
    rxQecStatus->percentComplete = (((uint32_t)armReadBack[7]) << 24) | (((uint32_t)armReadBack[6]) << 16) | (((uint32_t)armReadBack[5]) << 8)
            | ((uint32_t)armReadBack[4]);
    rxQecStatus->selfcheckIrrDb = (((uint32_t)armReadBack[11]) << 24) | (((uint32_t)armReadBack[10]) << 16) | (((uint32_t)armReadBack[9]) << 8)
            | ((uint32_t)armReadBack[8]);
    rxQecStatus->iterCount = (((uint32_t)armReadBack[15]) << 24) | (((uint32_t)armReadBack[14]) << 16) | (((uint32_t)armReadBack[13]) << 8) | ((uint32_t)armReadBack[12]);
    rxQecStatus->updateCount = (((uint32_t)armReadBack[19]) << 24) | (((uint32_t)armReadBack[18]) << 16) | (((uint32_t)armReadBack[17]) << 8)
            | ((uint32_t)armReadBack[16]);

    return MYKONOS_ERR_OK;
}

/**
 * \brief Returns the status of the ORxQEC tracking calibration
 *
 * The ORx QEC tracking calibration is run during the radioOn state.
 * The function can be called to read back the status of the ORxQEC external
 * calibration including metrics like error codes, percentage of data
 * collected for current cal, the performance of the cal and the number of
 * times the cal has run and updated the hardware.
 *
 * \pre Before the function is called, the device must be initialized, the ARM
 * loaded, and init cals run.  These functions can be called in radioOff or
 * radioOn state.
 *
 * \param device Pointer to the device settings structure
 * \param orxChannel The channel whose status is to be read back
 * \param orxQecStatus Status of the ORxQEC external calibration, as a structure
 * of type mykonosOrxQecStatus_t is returned to this pointer address
 *
 * \retval MYKONOS_ERR_GETORXQECSTATUS_NULLPARAM Function parameter orxQecStatus is a NULL pointer
 * \retval MYKONOS_ERR_GETORXQECSTATUS_INV_CH Channel selection not valid
 * \retval MYKONOS_ERR_GETORXQECSTATUS_ARMERRFLAG ARM command error
 * \retval MYKONOS_ERR_OK  Function completed successfully
 */
mykonosErr_t MYKONOS_getOrxQecStatus(mykonosDevice_t *device, mykonosObsRxChannels_t orxChannel, mykonosOrxQecStatus_t *orxQecStatus)
{
    uint8_t extData[3] = {MYKONOS_ARM_OBJECTID_CAL_STATUS, MYKONOS_ARM_OBJECTID_ORXQEC_TRACKING, 0};
    uint8_t cmdStatusByte = 0;
    uint32_t offset = 0;
    uint32_t timeoutMs = 0;
    uint8_t armReadBack[20] = {0};
    mykonosErr_t retVal = MYKONOS_ERR_OK;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_getOrxQecStatus()\n");
#endif

    if (orxQecStatus == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETORXQECSTATUS_NULLPARAM,
                getMykonosErrorMessage(MYKONOS_ERR_GETORXQECSTATUS_NULLPARAM));
        return MYKONOS_ERR_GETORXQECSTATUS_NULLPARAM;
    }

    switch (orxChannel)
    {
        case OBS_RX1_TXLO:
        case OBS_RX1_SNIFFERLO:
            extData[2] = 0;
            break;
        case OBS_RX2_TXLO:
        case OBS_RX2_SNIFFERLO:
            extData[2] = 1;
            break;
        default:
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETORXQECSTATUS_INV_CH,
                    getMykonosErrorMessage(MYKONOS_ERR_GETORXQECSTATUS_INV_CH));
            return MYKONOS_ERR_GETORXQECSTATUS_INV_CH;
        }
    }

    retVal = MYKONOS_sendArmCommand(device, MYKONOS_ARM_GET_OPCODE, &extData[0], sizeof(extData));
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    timeoutMs = 1000;
    retVal = MYKONOS_waitArmCmdStatus(device, MYKONOS_ARM_GET_OPCODE, timeoutMs, &cmdStatusByte);
    if (retVal != MYKONOS_ERR_OK)
    {
        if (cmdStatusByte > 0)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETORXQECSTATUS_ARMERRFLAG,
                    getMykonosErrorMessage(MYKONOS_ERR_GETORXQECSTATUS_ARMERRFLAG));
            return MYKONOS_ERR_GETORXQECSTATUS_ARMERRFLAG;
        }

        return retVal;
    }

    if (cmdStatusByte > 0)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETORXQECSTATUS_ARMERRFLAG,
                getMykonosErrorMessage(MYKONOS_ERR_GETORXQECSTATUS_ARMERRFLAG));
        return MYKONOS_ERR_GETORXQECSTATUS_ARMERRFLAG;
    }

    /* read status from ARM memory */
    offset = 0;
    retVal = MYKONOS_readArmMem(device, (MYKONOS_ADDR_ARM_START_DATA_ADDR + offset), &armReadBack[0], sizeof(armReadBack), 1);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    /* Assign to data structure */
    orxQecStatus->errorCode = (((uint32_t)armReadBack[3]) << 24) | (((uint32_t)armReadBack[2]) << 16) | (((uint32_t)armReadBack[1]) << 8) | ((uint32_t)armReadBack[0]);
    orxQecStatus->percentComplete = (((uint32_t)armReadBack[7]) << 24) | (((uint32_t)armReadBack[6]) << 16) | (((uint32_t)armReadBack[5]) << 8)
            | ((uint32_t)armReadBack[4]);
    orxQecStatus->selfcheckIrrDb = (((uint32_t)armReadBack[11]) << 24) | (((uint32_t)armReadBack[10]) << 16) | (((uint32_t)armReadBack[9]) << 8)
            | ((uint32_t)armReadBack[8]);
    orxQecStatus->iterCount = (((uint32_t)armReadBack[15]) << 24) | (((uint32_t)armReadBack[14]) << 16) | (((uint32_t)armReadBack[13]) << 8)
            | ((uint32_t)armReadBack[12]);
    orxQecStatus->updateCount = (((uint32_t)armReadBack[19]) << 24) | (((uint32_t)armReadBack[18]) << 16) | (((uint32_t)armReadBack[17]) << 8)
            | ((uint32_t)armReadBack[16]);

    return MYKONOS_ERR_OK;
}

/**
 * \brief Selects the Sniffer RF input to use for the observation receiver when in ObsRx pin mode and ORX_MODE = SNIFFER(4)
 *
 * This function is only valid when using the ObsRx Pin mode.  In pin mode, 3 GPIO pins select an Observation Rx source.
 * See mykonosObsRxChannels_t enum values less than 7.  When the ORX_MODE GPIO pins are set to 4 for Sniffer, Sniffer inputs
 * A, B, and C can be chosen by calling this function.  This function can be called any time after the ARM is loaded and running.
 * It can be called in radioOn or radioOff state.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device Pointer to the device settings structure
 * \param snifferChannel Desired channel to set.  This channel will be enabled when ORX_MODE = SNIFFER.
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_SET_ARMGPIO_PINS_ARMERROR ARM returned an error
 */
mykonosErr_t MYKONOS_setSnifferChannel(mykonosDevice_t *device, mykonosSnifferChannel_t snifferChannel)
{
    uint8_t armData[2] = {0};
    uint32_t timeoutMs = 0;
    uint8_t cmdStatusByte = 0;
    mykonosErr_t retVal = MYKONOS_ERR_OK;

    armData[0] = 0x6A; /* SET SRX_SOURCE ARM object ID */
    armData[1] = (uint8_t)snifferChannel;

    retVal = MYKONOS_sendArmCommand(device, MYKONOS_ARM_SET_OPCODE, &armData[0], sizeof(armData));
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    timeoutMs = 1000;
    retVal = MYKONOS_waitArmCmdStatus(device, MYKONOS_ARM_SET_OPCODE, timeoutMs, &cmdStatusByte);
    if (retVal != MYKONOS_ERR_OK)
    {
        if (cmdStatusByte > 0)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SET_ARMGPIO_PINS_ARMERROR,
                    getMykonosErrorMessage(MYKONOS_ERR_SET_ARMGPIO_PINS_ARMERROR));
            return MYKONOS_ERR_SET_ARMGPIO_PINS_ARMERROR;
        }

        return retVal;
    }

    if (cmdStatusByte > 0)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SET_ARMGPIO_PINS_ARMERROR,
                getMykonosErrorMessage(MYKONOS_ERR_SET_ARMGPIO_PINS_ARMERROR));
        return MYKONOS_ERR_SET_ARMGPIO_PINS_ARMERROR;
    }

    return MYKONOS_ERR_OK;
}

/**
 * \brief Resets the ARM processor and performs initialization
 *
 * Sets ARM Run = 0, Disables parity checks, sets ARM and SPI reg clock selects,
 * resets ARM, and enables ARM SPI register access
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 * - device->rx->rxProfile->iqRate_kHz
 * - device->deviceClock_kHz
 * - device->rx->rxProfile->rfBandwidth_Hz
 * - device->tx->txProfile->rfBandwidth_Hz
 * - device->rx->rxProfile->rxFirDecimation
 * - device->rx->rxProfile->rhb1Decimation
 * - device->spiSettings
 *
 * \param device Pointer to the device settings structure
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_INITARM_INV_VCODIV
 * \retval MYKONOS_ERR_INITARM_INV_REGCLK Could not calculate a valid ARM Register clock divider
 * \retval MYKONOS_ERR_INITARM_INV_ARMCLK_PARAM Could not calculate a valid ARM clock divider
 */
mykonosErr_t MYKONOS_initArm(mykonosDevice_t *device)
{
    uint8_t regClkSel = 0;
    uint8_t armClkSel = 0;
    uint32_t hsClkRateDiv4or5_Khz = 0;
    uint32_t hb1Clk = 0;
    uint32_t vcoDivTimes10 = 1;
    uint8_t adcDiv = 0;
    mykonosErr_t retVal = MYKONOS_ERR_OK;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_initArm()\n");
#endif

    /* Finish init - this is part of init that must run after Multi Chip Sync */
    retVal = MYKONOS_initSubRegisterTables(device);
    if (retVal != MYKONOS_ERR_OK)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, retVal, getMykonosErrorMessage(retVal));
        return retVal;
    }

    switch (device->clocks->clkPllVcoDiv)
    {
        case VCODIV_1:
            vcoDivTimes10 = 10;
            break;
        case VCODIV_1p5:
            vcoDivTimes10 = 15;
            break;
        case VCODIV_2:
            vcoDivTimes10 = 20;
            break;
        case VCODIV_3:
            vcoDivTimes10 = 30;
            break;
        default:
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INITARM_INV_VCODIV,
                    getMykonosErrorMessage(MYKONOS_ERR_INITARM_INV_VCODIV));
            return MYKONOS_ERR_INITARM_INV_VCODIV;
        }
    }

    hsClkRateDiv4or5_Khz = device->clocks->clkPllVcoFreq_kHz * 10 / vcoDivTimes10 / 20;

    /* If ADC divider is set, divide hsClkRateDiv4or5_kHz by 2 (ADC divider) */
    CMB_SPIReadField(device->spiSettings, MYKONOS_ADDR_CLOCK_CONTROL_1, &adcDiv, 0x01, 0);
    hb1Clk = (adcDiv == 1) ? (hsClkRateDiv4or5_Khz >> 1) : (hsClkRateDiv4or5_Khz);

    /* the ARM clock should not exceed 250 Mhz */
    if (hb1Clk <= 250000)
    {
        armClkSel = 0x00;
    }
    else if (hb1Clk > 250000 && hb1Clk <= 500000)
    {
        armClkSel = 0x02;
    }
    else if (hb1Clk > 500000 && hb1Clk <= 1000000)
    {
        armClkSel = 0x04;
    }
    else
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INITARM_INV_ARMCLK_PARAM,
                getMykonosErrorMessage(MYKONOS_ERR_INITARM_INV_ARMCLK_PARAM));
        return MYKONOS_ERR_INITARM_INV_ARMCLK_PARAM;
    }

    /* the SPI read reg clock must be equal or less than 100MHz and the SPI write reg clock must be less equal or less than 200 Mhz*/
    if (hb1Clk <= 100000)
    {
        regClkSel = 0x00;
    }
    else if (hb1Clk > 100000 && hb1Clk <= 200000)
    {
        regClkSel = 0x04;
    }
    else if (hb1Clk > 200000 && hb1Clk <= 400000)
    {
        regClkSel = 0x09;
    }
    else if (hb1Clk > 400000 && hb1Clk <= 800000)
    {
        regClkSel = 0x0E;
    }
    else
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INITARM_INV_REGCLK, getMykonosErrorMessage(MYKONOS_ERR_INITARM_INV_REGCLK));
        return MYKONOS_ERR_INITARM_INV_REGCLK;
    }

    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_ARM_CTL_1, 0x8C); /* arm_debug_enable[7]=1, mem_hresp_mask[3]=1, auto_incr[2]=1, arm_m3_run[0]=0 */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_ARM_CLK_CTL, armClkSel |= 0x41); /* setting the ARM clock rate, resetting the PC, and enabling the ARM clock */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_ARM_CLK_CTL, armClkSel &= ~0x40); /* maintaining the ARM clock rate, disabling the PC reset, and maintaining ARM clock enable */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_ARM_BRIDGE_CLK_CTL, regClkSel); /* setting the SPI read and write clock rates */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AHB_SPI_BRIDGE, 0x13); /* blockout_window_size[4:1] = 9, ahb_spi_bridge_enable[0] = 1 */

    retVal = MYKONOS_writeArmProfile(device);
    if (retVal != MYKONOS_ERR_OK)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, retVal, getMykonosErrorMessage(retVal));
        return retVal;
    }

    retVal = MYKONOS_loadAdcProfiles(device);
    if (retVal != MYKONOS_ERR_OK)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, retVal, getMykonosErrorMessage(retVal));
        return retVal;
    }

    return MYKONOS_ERR_OK;
}

/**
 * \brief Loads binary byte array into ARM program memory
 *
 * This function assumes the ARM auto increment bit is set in 0x0D00[2]. Valid memory
 * addresses are: Program Memory (0x01000000 - 0x01017FFF)
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 * - device->spiSettings
 *
 * \param device is structure pointer to the Mykonos data structure containing settings
 * \param binary is a byte array containing ARM program memory data bytes (directly from .bin file)
 * \param count is the number of bytes in the byte array
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_LOADBIN_NULL_PARAM Function parameter binary has a NULL pointer
 * \retval MYKONOS_ERR_LOADBIN_INVALID_BYTECOUNT Count parameter must be 98304 bytes
 */
mykonosErr_t MYKONOS_loadArmFromBinary(mykonosDevice_t *device, uint8_t *binary, uint32_t count)
{
    mykonosErr_t retVal = MYKONOS_ERR_OK;
    uint8_t stackPtr[4] = {0};
    uint8_t bootAddr[4] = {0};

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_loadArmFromBinary()\n");
#endif

    if (binary == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_LOADBIN_NULL_PARAM, getMykonosErrorMessage(MYKONOS_ERR_LOADBIN_NULL_PARAM));
        return MYKONOS_ERR_LOADBIN_NULL_PARAM;
    }

    if (count != 98304)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_LOADBIN_INVALID_BYTECOUNT,
                getMykonosErrorMessage(MYKONOS_ERR_LOADBIN_INVALID_BYTECOUNT));
        return MYKONOS_ERR_LOADBIN_INVALID_BYTECOUNT;
    }
    else
    {
        /* extraction of stack pointer and boot address from top of array */
        stackPtr[0] = binary[0];
        stackPtr[1] = binary[1];
        stackPtr[2] = binary[2];
        stackPtr[3] = binary[3];

        bootAddr[0] = binary[4];
        bootAddr[1] = binary[5];
        bootAddr[2] = binary[6];
        bootAddr[3] = binary[7];

        /* writing binary data to ARM memory */
        retVal = MYKONOS_writeArmMem(device, MYKONOS_ADDR_ARM_START_PROG_ADDR, &binary[0], count);
    }

    if (retVal != MYKONOS_ERR_OK)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, retVal, getMykonosErrorMessage(retVal));
        return retVal;
    }
    else
    {
        /* writing the stack pointer address */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_ARM_STACK_PTR_BYTE_0, stackPtr[0]); /* stack pointer [7:0]     */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_ARM_STACK_PTR_BYTE_1, stackPtr[1]); /* stack pointer [15:8]  */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_ARM_STACK_PTR_BYTE_2, stackPtr[2]); /* stack pointer [23:16] */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_ARM_STACK_PTR_BYTE_3, stackPtr[3]); /* stack pointer [31:24] */

        /* writing the boot address */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_ARM_BOOT_ADDR_BYTE_0, bootAddr[0]); /* boot address [7:0]     */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_ARM_BOOT_ADDR_BYTE_1, bootAddr[1]); /* boot address [15:8]     */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_ARM_BOOT_ADDR_BYTE_2, bootAddr[2]); /* boot address [23:16]     */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_ARM_BOOT_ADDR_BYTE_3, bootAddr[3]); /* boot address [31:24]     */

        /* setting the ARM run bit */
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_ARM_CTL_1, 0x8D); /* arm_debug_enable[7]=1, mem_hresp_mask[3]=1, auto_incr[2]=1, arm_m3_run[0]=1 */
    }

    /* verifying ARM checksum */
    retVal = MYKONOS_verifyArmChecksum(device);
    if (retVal != MYKONOS_ERR_OK)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, retVal, getMykonosErrorMessage(retVal));
        return retVal;
    }

    /* verifying ARM state is in MYKONOS_ARM_READY state, otherwise return error */
    retVal = MYKONOS_checkArmState(device, MYK_ARM_READY);
    if (retVal != MYKONOS_ERR_OK)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, retVal, getMykonosErrorMessage(retVal));
        return retVal;
    }

    /* Setup ARM GPIO pins and program ARMs radio control structure */
    retVal = MYKONOS_setArmGpioPins(device);
    if (retVal != MYKONOS_ERR_OK)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, retVal, getMykonosErrorMessage(retVal));
        return retVal;
    }

    retVal = MYKONOS_setRadioControlPinMode(device);
    if (retVal != MYKONOS_ERR_OK)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, retVal, getMykonosErrorMessage(retVal));
        return retVal;
    }

    /* Set the default ObsRx Path source for when the device moves to RadioOn */
    retVal = MYKONOS_setDefaultObsRxPath(device, device->obsRx->defaultObsRxChannel);
    if (retVal != MYKONOS_ERR_OK)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, retVal, getMykonosErrorMessage(retVal));
        return retVal;
    }

    return MYKONOS_ERR_OK;
}

/**
 * \brief Verifies the ARM checksum value
 *
 * The checksum which is written into the .hex file is verified with the calculated checksum
 * in the Mykonos ARM after the hex file has been loaded
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is structure pointer to the Mykonos data structure containing settings
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 */
mykonosErr_t MYKONOS_verifyArmChecksum(mykonosDevice_t *device)
{
    uint32_t buildTimeChecksum = 0x00000000;
    uint32_t calculatedChecksum = 0x00000000;
    uint8_t buildData[4];
    uint8_t calcData[4];
    const uint8_t CHECKSUM_BYTES = 0x4;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_verifyArmChecksum()\n");
#endif

    /* disabling auto increment and reading four (4) bytes at ARM checksum memory location */
    MYKONOS_readArmMem(device, MYKONOS_ADDR_ARM_BUILD_CHKSUM_ADDR, buildData, CHECKSUM_BYTES, 0);

    /* determining build time checksum */
    buildTimeChecksum = (((uint32_t)buildData[3] << 24) | ((uint32_t)buildData[2] << 16) | ((uint32_t)buildData[1] << 8) | (uint32_t)buildData[0]);

    /* using 200 msec timeout for exit out of while loop [maximum checksum calculation time = 5 ms] */
    CMB_setTimeout_ms(200);

    /* determining calculated checksum */
    do
    {
        MYKONOS_readArmMem(device, MYKONOS_ADDR_ARM_CALC_CHKSUM_ADDR, calcData, CHECKSUM_BYTES, 0);
        calculatedChecksum = (((uint32_t)calcData[3] << 24) | ((uint32_t)calcData[2] << 16) | ((uint32_t)calcData[1] << 8) | (uint32_t)calcData[0]);
    } while ((!calculatedChecksum) && (!CMB_hasTimeoutExpired()));

    /* performing consistency check */
    if (buildTimeChecksum == calculatedChecksum)
    {
        return MYKONOS_ERR_OK;
    }
    else
    {
        return MYKONOS_ERR_ARM_INVALID_BUILDCHKSUM;
    }
}

/**
 * \brief Verifies the ARM status once it is start running
 *
 * Wait for ARM to go into radio state, if takes longer there is and ARM error
 * get error in Application layer calling MYKONOS_getRadioState()
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is structure pointer to the Mykonos data structure containing settings
 * \param armStateCheck if the ARM is not in this state it will return an error,
 *
 * \retval MYKONOS_ERR_ARMSTATE_EXCEPTION ARM system problem has been detected.
 * \retval MYKONOS_ERR_ARMSTATE_CAL_ERROR ARM has detected an error in the tracking calibrations.
 * \retval MYKONOS_ERR_ARMSTATE_EXCEPTION ARM has detected an illegal profile.
 * \retval MYKONOS_ERR_WAITARMCSTATE_TIMEOUT Timeout occurred in check ARM state.
 * \retval MYKONOS_ERR_OK Function completed successfully
 */
mykonosErr_t MYKONOS_checkArmState(mykonosDevice_t *device, mykonosArmState_t armStateCheck)
{
    mykonosErr_t retVal = MYKONOS_ERR_OK;
    uint32_t armStatus = 0x00;
    uint32_t timeoutMs = 500; /* 500ms timeOut */
    uint8_t endCheck = 0x00;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_checkArmState()\n");
#endif

    CMB_setTimeout_ms(timeoutMs);

    do
    {
        retVal = MYKONOS_getRadioState(device, &armStatus);
        if (retVal != MYKONOS_ERR_OK)
        {
            return retVal;
        }

        if (armStateCheck == armStatus)
        {
            retVal = MYKONOS_ERR_OK;
            break;
        }

        if (CMB_hasTimeoutExpired())
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_WAITARMCSTATE_TIMEOUT,
                    getMykonosErrorMessage(MYKONOS_ERR_WAITARMCSTATE_TIMEOUT));
            retVal = MYKONOS_ERR_WAITARMCSTATE_TIMEOUT;
            break;
        }

        switch (armStatus)
        {
            /* this cases are not ARM errors */
            case MYK_ARM_POWERUP:
            case MYK_ARM_READY:
            case MYK_ARM_IDLE:
            case MYK_ARM_RADIO_ON:
                break;

            case MYK_ARM_PROFILE_ERROR:
                /* return error directly */
                retVal = MYKONOS_ERR_ARMSTATE_PROFILE_ERROR;
                endCheck = 1;
                break;

            case MYK_ARM_CAL_ERROR:
                /* return error directly */
                retVal = MYKONOS_ERR_ARMSTATE_CAL_ERROR;
                endCheck = 1;
                break;

            case MYK_ARM_EXCEPTION:
                /* return error directly */
                retVal = MYKONOS_ERR_ARMSTATE_EXCEPTION;
                endCheck = 1;
                break;

            case MYK_ARM_EXCEPTION | MYK_ARM_PROFILE_ERROR:
                /* return error directly */
                retVal = MYKONOS_ERR_ARMSTATE_EXCEPTION;
                endCheck = 1;
                break;

            default:
                endCheck = 0;
                break;
        }

    } while (!endCheck);

    return retVal;
}

/**
 * \brief Reads back the version of the ARM binary loaded into the Mykonos ARM memory
 *
 * This function reads the ARM memory to read back the major.minor.releaseCandidate
 * version for the ARM binary loaded into ARM memory.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is structure pointer to the Mykonos data structure containing settings
 * \param majorVer The Major version is returned in this parameter
 * \param minorVer The Minor version is returned in this parameter
 * \param rcVer The release candidate version (build number) is returned in this parameter
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_GETARMVER_NULL_PARM One of the function parameters has a NULL pointer
 */
mykonosErr_t MYKONOS_getArmVersion(mykonosDevice_t *device, uint8_t *majorVer, uint8_t *minorVer, uint8_t *rcVer)
{
    mykonosErr_t retVal = MYKONOS_ERR_OK;
    uint8_t ver[4] = {0};
    uint32_t fullVersion = 0;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_getArmVersion()\n");
#endif

    if ((majorVer == NULL) || (minorVer == NULL) || (rcVer == NULL))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETARMVER_NULL_PARM, getMykonosErrorMessage(MYKONOS_ERR_GETARMVER_NULL_PARM));
        return MYKONOS_ERR_GETARMVER_NULL_PARM;
    }

    retVal = MYKONOS_readArmMem(device, MYKONOS_ADDR_ARM_VERSION, &ver[0], sizeof(ver), 1);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    fullVersion = ((uint32_t)(ver[0]) | ((uint32_t)(ver[1]) << 8) | ((uint32_t)(ver[2]) << 16) | ((uint32_t)(ver[3]) << 24));
    *rcVer = (fullVersion % 100);
    *minorVer = ((fullVersion / 100) % 100);
    *majorVer = (fullVersion / 10000);

    return MYKONOS_ERR_OK;
}

/**
 * \brief This function, when called during RadioOff, will configure DPD settings
 *
 *  A AD9373 device is required for DPD to be enabled.  The DPD has several user
 *  adjustable settings that can be configured before running the runInitCals
 *  with the calMask set for the DPD init cal. Call this function with desired
 *  settings set before running the DPD init cal or enabling DPD tracking.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 * - device->profilesValid
 * - device->tx->dpdConfig (All members)
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_CFGDPD_TXORX_PROFILE_INV ERROR: to use these features, a valid Tx and ORx profile must exist in the device data structure
 * \retval MYKONOS_ERR_CFGDPD_NULL_DPDCFGSTRUCT ERROR: device->tx->dpdConfig is a NULL pointer
 * \retval MYKONOS_ERR_CFGDPD_ARMSTATE_ERROR ERROR: To set these settings, the ARM must be in the radioOff (IDLE) state.
 * \retval MYKONOS_ERR_CFGDPD_INV_DPDDAMPING ERROR: damping parameter is out of range
 * \retval MYKONOS_ERR_CFGDPD_INV_DPDSAMPLES ERROR: samples parameter is out of range
 * \retval MYKONOS_ERR_CFGDPD_INV_NUMWEIGHTS ERROR: numWeights parameter is out of range (0-3)
 * \retval MYKONOS_ERR_CFGDPD_INV_MODELVERSION ERROR: modelVersion parameter is out of range (0-3)
 * \retval MYKONOS_ERR_CFGDPD_INV_DPDOUTLIERTHRESH ERROR: outlierThreshold parameter is out of range
 * \retval MYKONOS_ERR_CFGDPD_INV_DPD_ADDDELAY ERROR: additionalDelayOffset parameter is out of range
 * \retval MYKONOS_ERR_CFGDPD_INV_PNSEQLEVEL ERROR: pathDelayPnSeqLevel parameter is out of range
 *
 */
mykonosErr_t MYKONOS_configDpd(mykonosDevice_t *device)
{
    uint32_t radioStatus = 0;
    mykonosErr_t retVal = MYKONOS_ERR_OK;
    uint8_t armFieldValue[6] = {0};
    uint8_t byteOffset = 0;
    uint16_t negPnLevel = 0;
    uint8_t highPowerModelUpdate = 0;
    uint8_t robustModeling = 0;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_configDpd()\n");
#endif

    /* DPD requires Tx and ORx enabled, throw error if both are not enabled */
    if ((device->profilesValid & (TX_PROFILE_VALID | ORX_PROFILE_VALID)) != (TX_PROFILE_VALID | ORX_PROFILE_VALID))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_CFGDPD_TXORX_PROFILE_INV,
                getMykonosErrorMessage(MYKONOS_ERR_CFGDPD_TXORX_PROFILE_INV));
        return MYKONOS_ERR_CFGDPD_TXORX_PROFILE_INV;
    }

    if (device->tx->dpdConfig == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_CFGDPD_NULL_DPDCFGSTRUCT,
                getMykonosErrorMessage(MYKONOS_ERR_CFGDPD_NULL_DPDCFGSTRUCT));
        return MYKONOS_ERR_CFGDPD_NULL_DPDCFGSTRUCT;
    }

    /* read radio state to make sure ARM is in radioOff /IDLE */
    retVal = MYKONOS_getRadioState(device, &radioStatus);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    /* throw error if not in radioOff/IDLE state */
    if (((radioStatus & 0x03) != MYKONOS_ARM_SYSTEMSTATE_IDLE) && ((radioStatus & 0x03) != MYKONOS_ARM_SYSTEMSTATE_READY))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_CFGDPD_ARMSTATE_ERROR,
                getMykonosErrorMessage(MYKONOS_ERR_CFGDPD_ARMSTATE_ERROR));
        return MYKONOS_ERR_CFGDPD_ARMSTATE_ERROR;
    }

    /* check DPD damping and samples parameters */
    if (device->tx->dpdConfig->damping > 15)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_CFGDPD_INV_DPDDAMPING,
                getMykonosErrorMessage(MYKONOS_ERR_CFGDPD_INV_DPDDAMPING));
        return MYKONOS_ERR_CFGDPD_INV_DPDDAMPING;
    }

    if ((device->tx->dpdConfig->samples < 64) || (device->tx->dpdConfig->samples > 32768))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_CFGDPD_INV_DPDSAMPLES,
                getMykonosErrorMessage(MYKONOS_ERR_CFGDPD_INV_DPDSAMPLES));
        return MYKONOS_ERR_CFGDPD_INV_DPDSAMPLES;
    }

    if (device->tx->dpdConfig->numWeights > 3)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_CFGDPD_INV_NUMWEIGHTS,
                getMykonosErrorMessage(MYKONOS_ERR_CFGDPD_INV_NUMWEIGHTS));
        return MYKONOS_ERR_CFGDPD_INV_NUMWEIGHTS;
    }

    if (device->tx->dpdConfig->modelVersion > 3)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_CFGDPD_INV_MODELVERSION,
                getMykonosErrorMessage(MYKONOS_ERR_CFGDPD_INV_MODELVERSION));
        return MYKONOS_ERR_CFGDPD_INV_MODELVERSION;
    }

    highPowerModelUpdate = (device->tx->dpdConfig->highPowerModelUpdate > 0) ? 1 : 0;
    robustModeling = (device->tx->dpdConfig->robustModeling > 0) ? 1 : 0;

    armFieldValue[0] = (((device->tx->dpdConfig->numWeights & 3) << 4) | (device->tx->dpdConfig->damping & 0xF));
    armFieldValue[1] = ((robustModeling << 3) | (highPowerModelUpdate << 2) | (device->tx->dpdConfig->modelVersion & 3));
    armFieldValue[2] = (device->tx->dpdConfig->samples & 0xFF);
    armFieldValue[3] = ((device->tx->dpdConfig->samples >> 8) & 0xFF);
    byteOffset = 0;
    retVal = MYKONOS_writeArmConfig(device, MYKONOS_ARM_OBJECTID_DPDCONFIG, byteOffset, &armFieldValue[0], 4);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    /* set DPD outlier threshold parameter */
    if ((device->tx->dpdConfig->outlierThreshold < 1) || (device->tx->dpdConfig->outlierThreshold > 8192))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_CFGDPD_INV_DPDOUTLIERTHRESH,
                getMykonosErrorMessage(MYKONOS_ERR_CFGDPD_INV_DPDOUTLIERTHRESH));
        return MYKONOS_ERR_CFGDPD_INV_DPDOUTLIERTHRESH;
    }

    armFieldValue[0] = (device->tx->dpdConfig->outlierThreshold & 0xFF);
    armFieldValue[1] = ((device->tx->dpdConfig->outlierThreshold >> 8) & 0xFF);
    byteOffset = 10;
    retVal = MYKONOS_writeArmConfig(device, MYKONOS_ARM_OBJECTID_DPDCONFIG, byteOffset, &armFieldValue[0], 2);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    /* set DPD Model Prior Weight parameter */
    if (device->tx->dpdConfig->modelPriorWeight > 32)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_CFGDPD_INV_DPDPRIORWEIGHT,
                getMykonosErrorMessage(MYKONOS_ERR_CFGDPD_INV_DPDPRIORWEIGHT));
        return MYKONOS_ERR_CFGDPD_INV_DPDPRIORWEIGHT;
    }
    armFieldValue[0] = (device->tx->dpdConfig->modelPriorWeight & 0xFF);
    armFieldValue[1] = 0;
    byteOffset = 12;
    retVal = MYKONOS_writeArmConfig(device, MYKONOS_ARM_OBJECTID_DPDCONFIG, byteOffset, &armFieldValue[0], 2);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    /* no need to verify weights min/max as all int8 values are valid */
    armFieldValue[0] = (device->tx->dpdConfig->weights[0].real);
    armFieldValue[1] = (device->tx->dpdConfig->weights[0].imag);
    armFieldValue[2] = (device->tx->dpdConfig->weights[1].real);
    armFieldValue[3] = (device->tx->dpdConfig->weights[1].imag);
    armFieldValue[4] = (device->tx->dpdConfig->weights[2].real);
    armFieldValue[5] = (device->tx->dpdConfig->weights[2].imag);

    byteOffset = 14;
    retVal = MYKONOS_writeArmConfig(device, MYKONOS_ARM_OBJECTID_DPDCONFIG, byteOffset, &armFieldValue[0], 6);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    /* set DPD additional delay offset parameter */
    if ((device->tx->dpdConfig->additionalDelayOffset < -64) || (device->tx->dpdConfig->additionalDelayOffset > 64))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_CFGDPD_INV_DPD_ADDDELAY,
                getMykonosErrorMessage(MYKONOS_ERR_CFGDPD_INV_DPD_ADDDELAY));
        return MYKONOS_ERR_CFGDPD_INV_DPD_ADDDELAY;
    }

    armFieldValue[0] = (device->tx->dpdConfig->additionalDelayOffset & 0xFF);
    armFieldValue[1] = ((device->tx->dpdConfig->additionalDelayOffset >> 8) & 0xFF);
    byteOffset = 2;
    retVal = MYKONOS_writeArmConfig(device, MYKONOS_ARM_OBJECTID_DPDINIT_CONFIG, byteOffset, &armFieldValue[0], 2);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    if ((device->tx->dpdConfig->pathDelayPnSeqLevel < 1) || (device->tx->dpdConfig->pathDelayPnSeqLevel > 8191))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_CFGDPD_INV_PNSEQLEVEL,
                getMykonosErrorMessage(MYKONOS_ERR_CFGDPD_INV_PNSEQLEVEL));
        return MYKONOS_ERR_CFGDPD_INV_PNSEQLEVEL;
    }

    /* Set Path Delay PN sequence amplitude - positive and negative */
    armFieldValue[0] = (device->tx->dpdConfig->pathDelayPnSeqLevel & 0xFF);
    armFieldValue[1] = ((device->tx->dpdConfig->pathDelayPnSeqLevel >> 8) & 0xFF);

    negPnLevel = ((~device->tx->dpdConfig->pathDelayPnSeqLevel) + 1); /* times -1 */
    armFieldValue[2] = (negPnLevel & 0xFF);
    armFieldValue[3] = (((negPnLevel) >> 8) & 0xFF);
    byteOffset = 10;
    retVal = MYKONOS_writeArmConfig(device, MYKONOS_ARM_OBJECTID_DPDINIT_CONFIG, byteOffset, &armFieldValue[0], 4);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    return MYKONOS_ERR_OK;
}

/* \brief Reads the DPD config structure from ARM memory
 *
 *  This function reads the DPD structure
 *  from ARM memory and returns in the device->tx->dpdConfig structure.
 *
 *  A AD9373 device is required for DPD to be enabled.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 * - device->profilesValid
 * - device->tx->dpdConfig (All members)
 *
 * \param device is structure pointer to the Mykonos data structure containing settings
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_GETDPDCFG_TXORX_PROFILE_INV Error: Tx and ORx profiles must be valid for DPD functions
 * \retval MYKONOS_ERR_GETDPDCFG_NULL_DPDCFGSTRUCT Error: NULL pointer to device->tx->dpdConfig structure
 */
mykonosErr_t MYKONOS_getDpdConfig(mykonosDevice_t *device)
{
    uint16_t byteOffset = 0;
    uint8_t armMem[20] = {0};
    mykonosErr_t retVal = MYKONOS_ERR_OK;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_getDpdConfig()\n");
#endif

    /* DPD requires Tx and ORx enabled, throw error if both are not enabled */
    if ((device->profilesValid & (TX_PROFILE_VALID | ORX_PROFILE_VALID)) != (TX_PROFILE_VALID | ORX_PROFILE_VALID))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETDPDCFG_TXORX_PROFILE_INV,
                getMykonosErrorMessage(MYKONOS_ERR_GETDPDCFG_TXORX_PROFILE_INV));
        return MYKONOS_ERR_GETDPDCFG_TXORX_PROFILE_INV;
    }

    if (device->tx->dpdConfig == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETDPDCFG_NULL_DPDCFGSTRUCT,
                getMykonosErrorMessage(MYKONOS_ERR_GETDPDCFG_NULL_DPDCFGSTRUCT));
        return MYKONOS_ERR_GETDPDCFG_NULL_DPDCFGSTRUCT;
    }

    byteOffset = 0;
    retVal = MYKONOS_readArmConfig(device, MYKONOS_ARM_OBJECTID_DPDCONFIG, byteOffset, &armMem[0], 20);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    device->tx->dpdConfig->damping = (armMem[0] & 0x0F);
    device->tx->dpdConfig->numWeights = ((armMem[0] >> 4) & 0x03);

    device->tx->dpdConfig->modelVersion = (armMem[1] & 0x03);
    device->tx->dpdConfig->highPowerModelUpdate = ((armMem[1] >> 2) & 0x01);
    device->tx->dpdConfig->robustModeling = ((armMem[1] >> 3) & 0x01);
    device->tx->dpdConfig->samples = ((uint16_t)(armMem[3]) << 8) | (uint16_t)(armMem[2]);

    device->tx->dpdConfig->outlierThreshold = ((uint16_t)(armMem[11]) << 8) | (uint16_t)(armMem[10]);
    device->tx->dpdConfig->modelPriorWeight = armMem[12];
    device->tx->dpdConfig->weights[0].real = (int8_t)(armMem[14]);
    device->tx->dpdConfig->weights[0].imag = (int8_t)(armMem[15]);
    device->tx->dpdConfig->weights[1].real = (int8_t)(armMem[16]);
    device->tx->dpdConfig->weights[1].imag = (int8_t)(armMem[17]);
    device->tx->dpdConfig->weights[2].real = (int8_t)(armMem[18]);
    device->tx->dpdConfig->weights[2].imag = (int8_t)(armMem[19]);

    byteOffset = 2;
    retVal = MYKONOS_readArmConfig(device, MYKONOS_ARM_OBJECTID_DPDINIT_CONFIG, byteOffset, &armMem[0], 2);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    device->tx->dpdConfig->additionalDelayOffset = (int16_t)(((uint16_t)(armMem[1]) << 8) | (uint16_t)(armMem[0]));

    byteOffset = 10;
    retVal = MYKONOS_readArmConfig(device, MYKONOS_ARM_OBJECTID_DPDINIT_CONFIG, byteOffset, &armMem[0], 2);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    device->tx->dpdConfig->pathDelayPnSeqLevel = (((uint16_t)(armMem[1]) << 8) | (uint16_t)(armMem[0]));

    return MYKONOS_ERR_OK;
}

/**
 * \brief This function reads the DPD calibration status from the Mykonos ARM processor.
 *
 * The dpdStatus is read back from the ARM processor and returned in the function
 * parameter dpdStatus.
 *
 * A AD9373 device is required for this feature to be enabled.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is structure pointer to the Mykonos data structure containing settings
 * \param txChannel Desired Transmit channel to read back DPD status for (Valid ENUM values: TX1 or TX2 only)
 * \param dpdStatus Pointer to a structure to return the status information to
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_GETDPDSTATUS_NULLPARAM dpdStatus function parameter is a NULL pointer
 * \retval MYKONOS_ERR_GETDPDSTATUS_INV_CH txChannel parameter is a non-supported value.
 * \retval MYKONOS_ERR_GETDPDSTATUS_ARMERRFLAG ARM reported an error while processing the GET ARM command
 */
mykonosErr_t MYKONOS_getDpdStatus(mykonosDevice_t *device, mykonosTxChannels_t txChannel, mykonosDpdStatus_t *dpdStatus)
{
    uint8_t extData[3] = {MYKONOS_ARM_OBJECTID_CAL_STATUS, MYKONOS_ARM_OBJECTID_DPDCONFIG, 0};
    uint8_t armData[28] = {0};
    uint32_t timeoutMs = 0;
    uint8_t cmdStatusByte = 0;
    uint8_t channelSelect = 0;
    mykonosErr_t retVal = MYKONOS_ERR_OK;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_getDpdStatus()\n");
#endif

    if (dpdStatus == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETDPDSTATUS_NULLPARAM,
                getMykonosErrorMessage(MYKONOS_ERR_GETDPDSTATUS_NULLPARAM));
        return MYKONOS_ERR_GETDPDSTATUS_NULLPARAM;
    }

    switch (txChannel)
    {
        case TX1:
            channelSelect = 0;
            break;
        case TX2:
            channelSelect = 1;
            break;
        default:
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETDPDSTATUS_INV_CH,
                    getMykonosErrorMessage(MYKONOS_ERR_GETDPDSTATUS_INV_CH));
            return MYKONOS_ERR_GETDPDSTATUS_INV_CH;
        }
    }

    extData[2] = channelSelect;

    retVal = MYKONOS_sendArmCommand(device, MYKONOS_ARM_GET_OPCODE, &extData[0], sizeof(extData));
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    timeoutMs = 1000;
    retVal = MYKONOS_waitArmCmdStatus(device, MYKONOS_ARM_GET_OPCODE, timeoutMs, &cmdStatusByte);
    if (retVal != MYKONOS_ERR_OK)
    {
        if (cmdStatusByte > 0)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETDPDSTATUS_ARMERRFLAG,
                    getMykonosErrorMessage(MYKONOS_ERR_GETDPDSTATUS_ARMERRFLAG));
            return MYKONOS_ERR_GETDPDSTATUS_ARMERRFLAG;
        }

        return retVal;
    }

    if (cmdStatusByte > 0)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETDPDSTATUS_ARMERRFLAG,
                getMykonosErrorMessage(MYKONOS_ERR_GETDPDSTATUS_ARMERRFLAG));
        return MYKONOS_ERR_GETDPDSTATUS_ARMERRFLAG;
    }

    /* read status from ARM memory */
    retVal = MYKONOS_readArmMem(device, MYKONOS_ADDR_ARM_START_DATA_ADDR, &armData[0], 28, 1);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    dpdStatus->dpdErrorStatus = ((uint32_t)(armData[1]) << 8) | (uint32_t)(armData[0]); /* Only lower 16 bits */
    dpdStatus->dpdTrackCount = ((uint32_t)(armData[11]) << 24) | ((uint32_t)(armData[10]) << 16) | ((uint32_t)(armData[9]) << 8) | (uint32_t)(armData[8]);
    dpdStatus->dpdModelErrorPercent = ((uint32_t)(armData[15]) << 24) | ((uint32_t)(armData[14]) << 16) | ((uint32_t)(armData[13]) << 8) | (uint32_t)(armData[12]);
    dpdStatus->dpdExtPathDelay = ((uint32_t)(armData[26]) * 16) + (uint32_t)(armData[24]);

    return MYKONOS_ERR_OK;
}

/**
 * \brief This private helper function called during RadioOff, will allow DPD tracking to be scheduled
 *        by the ARM when back in the RadioOn state.
 *
 *  This function sets a flag in the ARM memory that if the enableDpd tracking cal mask
 *  and this setting are both set, then the ARM will schedule DPD tracking to occur
 *  in the radioOn ARM state.
 *
 *  A AD9373 device is required for DPD to be enabled.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 * - device->tx->dpdConfig
 *
 * \param device is structure pointer to the Mykonos data structure containing settings
 * \param tx1Enable 0=disable DPD Tx1 tracking, 1= enable DPD Tx1 tracking
 * \param tx2Enable 0=disable DPD Tx2 tracking, 1= enable DPD Tx2 tracking
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_EN_DPDTRACKING_ARMSTATE_ERROR ARM is not in the RadioOff state, call MYKONOS_radioOff()
 */
static mykonosErr_t enableDpdTracking(mykonosDevice_t *device, uint8_t tx1Enable, uint8_t tx2Enable)
{
    uint32_t radioStatus = 0;
    mykonosErr_t retVal = MYKONOS_ERR_OK;
    uint8_t enableBit[4] = {0};
    uint8_t byteOffset = 20;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_enableDpdTracking()\n");
#endif

    /* read radio state to make sure ARM is in radioOff /IDLE */
    retVal = MYKONOS_getRadioState(device, &radioStatus);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    /* throw error if not in radioOff/IDLE state */
    if (((radioStatus & 0x03) != MYKONOS_ARM_SYSTEMSTATE_IDLE) && ((radioStatus & 0x03) != MYKONOS_ARM_SYSTEMSTATE_READY))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_EN_DPDTRACKING_ARMSTATE_ERROR,
                getMykonosErrorMessage(MYKONOS_ERR_EN_DPDTRACKING_ARMSTATE_ERROR));
        return MYKONOS_ERR_EN_DPDTRACKING_ARMSTATE_ERROR;
    }

    enableBit[0] = (tx1Enable > 0) ? 1 : 0;
    enableBit[1] = 0;
    enableBit[2] = (tx2Enable > 0) ? 1 : 0;
    enableBit[3] = 0;

    retVal = MYKONOS_writeArmConfig(device, MYKONOS_ARM_OBJECTID_DPDCONFIG, byteOffset, &enableBit[0], 4);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    return MYKONOS_ERR_OK;
}

/**
 * \brief This function called during RadioOff, will allow loading of the DPD model file.
 *
 *  This function writes a copy of the user's DPD model to ARM memory and instructs the ARM to install that DPD model
 *  into hardware.  Note that initializing the device will over write DPD model data.  Note that the DPD model being
 *  restored must match the PA for which it is configured.  Restoring a DPD model to a different PA than for which it
 *  is configured will not yield the desired performance.  The user is responsible to insure the DPD model matches the
 *  PA configuration.
 *
 * \param device Structure pointer to the Mykonos data structure containing settings
 * \param txChannel Desired transmit channel to which to write the DPD model file (Valid ENUM type mykonosTxChannels_t: TX1 or TX2 or TX1_TX2)
 * \param modelDataBuffer Pointer to the user buffer containing the history/model data to be loaded to a txChannel
 *        Valid sizes: 172 bytes for a single model/rms load to either TX1 or TX2.
 *                     344 bytes for a dual model/rms load to both TX1_TX2, where the TX1 model/rms data will occupy the first 172 bytes
 *                     and TX2 model/rms data will occupy the second 172 bytes.
 * \param modelNumberBytes Total buffer size of the user history/model data buffer. Allowed sizes are 172 bytes for TX1 or TX2 and
 *        344 bytes for TX1_TX2.
 *
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_RESTDPDMOD_ARMSTATE ARM is not in the RadioOff state, call MYKONOS_radioOff()
 * \retval MYKONOS_ERR_RESTDPDMOD_WRONGBUFFERSIZE User suppled modelNumberBytes size is incorrect. TX1 or TX2 = 172,  TX1_TX2 = 344
 * \retval MYKONOS_ERR_RESTDPDMOD_INVALID_TXCHANNEL User supplied txChannel does not match TX1 or TX2 or TX1_TX2
 * \retval MYKONOS_ERR_RESTDPDMOD_ARMERRFLAG ARM returned error for Set ARM Command
 */
mykonosErr_t MYKONOS_restoreDpdModel(mykonosDevice_t *device, mykonosTxChannels_t txChannel, uint8_t *modelDataBuffer, uint32_t modelNumberBytes)
{
    uint32_t radioStatus = 0;
    mykonosErr_t retVal = MYKONOS_ERR_OK;
    uint16_t modelSize = 0;
    uint16_t rmsSize = 0;
    uint8_t armData[4] = {0};
    uint32_t armBufferAddr = 0;
    uint8_t *bufferPtr = 0;
    uint8_t extendedData[4] = {0};
    uint8_t cmdStatusByte = 0;

    const uint32_t timeoutMs = 1000;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_restoreDpdModel()\n");
#endif

    /* read radio state to make sure ARM is in radioOff /IDLE */
    retVal = MYKONOS_getRadioState(device, &radioStatus);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    /* throw error if not in radioOff/IDLE state */
    if (((radioStatus & 0x03) != MYKONOS_ARM_SYSTEMSTATE_IDLE) && ((radioStatus & 0x03) != MYKONOS_ARM_SYSTEMSTATE_READY))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_RESTDPDMOD_ARMSTATE, getMykonosErrorMessage(MYKONOS_ERR_RESTDPDMOD_ARMSTATE)); /* radio not off or initialized */
        return MYKONOS_ERR_RESTDPDMOD_ARMSTATE;
    }

    /* retrieve the model buffer byte count */
    retVal = MYKONOS_readArmMem(device, (MYKONOS_ADDR_ARM_START_PROG_ADDR + MYKONOS_ADDR_DPD_MODEL_BUF_SIZE), &armData[0], 2, 1);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }
    modelSize = (((uint32_t)armData[0]) | (((uint32_t)armData[1]) << 8));

    /* retrieve the rms buffer byte count */
    retVal = MYKONOS_readArmMem(device, (MYKONOS_ADDR_ARM_START_PROG_ADDR + MYKONOS_ADDR_DPD_RMS_BUF_SIZE), &armData[0], 2, 1);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }
    rmsSize = (((uint32_t)armData[0]) | (((uint32_t)armData[1]) << 8));

    if (txChannel == TX1)
    {
        /* check for single valid provided buffer size */
        if (modelNumberBytes != (modelSize + rmsSize))
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_RESTDPDMOD_WRONGBUFFERSIZE,
                    getMykonosErrorMessage(MYKONOS_ERR_RESTDPDMOD_WRONGBUFFERSIZE));
            return MYKONOS_ERR_RESTDPDMOD_WRONGBUFFERSIZE;
        }

        /* retrieve the arm TX1 model buffer address */
        retVal = MYKONOS_readArmMem(device, (MYKONOS_ADDR_ARM_START_PROG_ADDR + MYKONOS_ADDR_TX1_DPD_MODEL_INDIRECT_PTR), &armData[0], 4, 1);
        if (retVal != MYKONOS_ERR_OK)
        {
            return retVal;
        }
        armBufferAddr = (((uint32_t)armData[0]) | (((uint32_t)armData[1]) << 8) | (((uint32_t)armData[2]) << 16) | (((uint32_t)armData[3]) << 24));

        /* write the TX1 model data */
        bufferPtr = modelDataBuffer;
        retVal = MYKONOS_writeArmMem(device, armBufferAddr, bufferPtr, modelSize); /* write TX1 model */
        if (retVal != MYKONOS_ERR_OK)
        {
            return retVal;
        }
        bufferPtr += modelSize;

        /* retrieve the arm TX1 rms buffer address */
        retVal = MYKONOS_readArmMem(device, (MYKONOS_ADDR_ARM_START_PROG_ADDR + MYKONOS_ADDR_TX1_DPD_RMS_INDIRECT_PTR), &armData[0], 4, 1);
        if (retVal != MYKONOS_ERR_OK)
        {
            return retVal;
        }
        armBufferAddr = (((uint32_t)armData[0]) | (((uint32_t)armData[1]) << 8) | (((uint32_t)armData[2]) << 16) | (((uint32_t)armData[3]) << 24));

        /* write the TX1 rms */
        retVal = MYKONOS_writeArmMem(device, armBufferAddr, bufferPtr, rmsSize);
        if (retVal != MYKONOS_ERR_OK)
        {
            return retVal;
        }
    }/*end TX1 */

    else if (txChannel == TX2)
    {
        /* check for single valid provided buffer size */
        if (modelNumberBytes != (modelSize + rmsSize))
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_RESTDPDMOD_WRONGBUFFERSIZE,
                    getMykonosErrorMessage(MYKONOS_ERR_RESTDPDMOD_WRONGBUFFERSIZE));
            return MYKONOS_ERR_RESTDPDMOD_WRONGBUFFERSIZE;
        }

        /* retrieve the arm TX2 model buffer address */
        retVal = MYKONOS_readArmMem(device, (MYKONOS_ADDR_ARM_START_PROG_ADDR + MYKONOS_ADDR_TX2_DPD_MODEL_INDIRECT_PTR), &armData[0], 4, 1);
        if (retVal != MYKONOS_ERR_OK)
        {
            return retVal;
        }
        armBufferAddr = (((uint32_t)armData[0]) | (((uint32_t)armData[1]) << 8) | (((uint32_t)armData[2]) << 16) | (((uint32_t)armData[3]) << 24));

        /* write the TX2 model data */
        bufferPtr = modelDataBuffer;
        retVal = MYKONOS_writeArmMem(device, armBufferAddr, bufferPtr, modelSize); /* write TX2 model */
        if (retVal != MYKONOS_ERR_OK)
        {
            return retVal;
        }
        bufferPtr += modelSize;

        /* retrieve the arm TX2 rms buffer address */
        retVal = MYKONOS_readArmMem(device, (MYKONOS_ADDR_ARM_START_PROG_ADDR + MYKONOS_ADDR_TX2_DPD_RMS_INDIRECT_PTR), &armData[0], 4, 1);
        if (retVal != MYKONOS_ERR_OK)
        {
            return retVal;
        }
        armBufferAddr = (((uint32_t)armData[0]) | (((uint32_t)armData[1]) << 8) | (((uint32_t)armData[2]) << 16) | (((uint32_t)armData[3]) << 24));

        /* write the TX2 rms */
        retVal = MYKONOS_writeArmMem(device, armBufferAddr, bufferPtr, rmsSize);
        if (retVal != MYKONOS_ERR_OK)
        {
            return retVal;
        }
    }/* end TX2 */

    /* check for dual txChannel type */
    else if (txChannel == TX1_TX2)
    {
        /* check for single valid provided buffer size */
        if (modelNumberBytes != (2 * (modelSize + rmsSize)))
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_RESTDPDMOD_WRONGBUFFERSIZE,
                    getMykonosErrorMessage(MYKONOS_ERR_RESTDPDMOD_WRONGBUFFERSIZE));
            return MYKONOS_ERR_RESTDPDMOD_WRONGBUFFERSIZE;
        }

        /* retrieve the arm TX1 model buffer address */
        retVal = MYKONOS_readArmMem(device, (MYKONOS_ADDR_ARM_START_PROG_ADDR + MYKONOS_ADDR_TX1_DPD_MODEL_INDIRECT_PTR), &armData[0], 4, 1);
        if (retVal != MYKONOS_ERR_OK)
        {
            return retVal;
        }
        armBufferAddr = (((uint32_t)armData[0]) | (((uint32_t)armData[1]) << 8) | (((uint32_t)armData[2]) << 16) | (((uint32_t)armData[3]) << 24));

        /* write the TX1 model data */
        bufferPtr = modelDataBuffer;
        retVal = MYKONOS_writeArmMem(device, armBufferAddr, bufferPtr, modelSize); /* write TX1 model */
        if (retVal != MYKONOS_ERR_OK)
        {
            return retVal;
        }
        bufferPtr += modelSize;

        /* retrieve the arm TX1 rms buffer address */
        retVal = MYKONOS_readArmMem(device, (MYKONOS_ADDR_ARM_START_PROG_ADDR + MYKONOS_ADDR_TX1_DPD_RMS_INDIRECT_PTR), &armData[0], 4, 1);
        if (retVal != MYKONOS_ERR_OK)
        {
            return retVal;
        }
        armBufferAddr = (((uint32_t)armData[0]) | (((uint32_t)armData[1]) << 8) | (((uint32_t)armData[2]) << 16) | (((uint32_t)armData[3]) << 24));

        /* write the TX1 rms */
        retVal = MYKONOS_writeArmMem(device, armBufferAddr, bufferPtr, rmsSize);
        if (retVal != MYKONOS_ERR_OK)
        {
            return retVal;
        }
        bufferPtr += rmsSize;

        /* retrieve the arm TX2 model buffer address */
        retVal = MYKONOS_readArmMem(device, (MYKONOS_ADDR_ARM_START_PROG_ADDR + MYKONOS_ADDR_TX2_DPD_MODEL_INDIRECT_PTR), &armData[0], 4, 1);
        if (retVal != MYKONOS_ERR_OK)
        {
            return retVal;
        }
        armBufferAddr = (((uint32_t)armData[0]) | (((uint32_t)armData[1]) << 8) | (((uint32_t)armData[2]) << 16) | (((uint32_t)armData[3]) << 24));

        /* write the TX2 model data */
        retVal = MYKONOS_writeArmMem(device, armBufferAddr, bufferPtr, modelSize); /* write TX2 model */
        if (retVal != MYKONOS_ERR_OK)
        {
            return retVal;
        }
        bufferPtr += modelSize;

        /* retrieve the arm TX2 rms buffer address */
        retVal = MYKONOS_readArmMem(device, (MYKONOS_ADDR_ARM_START_PROG_ADDR + MYKONOS_ADDR_TX2_DPD_RMS_INDIRECT_PTR), &armData[0], 4, 1);
        if (retVal != MYKONOS_ERR_OK)
        {
            return retVal;
        }
        armBufferAddr = (((uint32_t)armData[0]) | (((uint32_t)armData[1]) << 8) | (((uint32_t)armData[2]) << 16) | (((uint32_t)armData[3]) << 24));

        /* write the TX2 rms */
        retVal = MYKONOS_writeArmMem(device, armBufferAddr, bufferPtr, rmsSize);
        if (retVal != MYKONOS_ERR_OK)
        {
            return retVal;
        }
    }/*end two txChannel model data load */

    /* invalid txChannel for this api command */
    else
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_RESTDPDMOD_INVALID_TXCHANNEL,
                getMykonosErrorMessage(MYKONOS_ERR_RESTDPDMOD_INVALID_TXCHANNEL));
        return MYKONOS_ERR_RESTDPDMOD_INVALID_TXCHANNEL;
    }/* end invalid txChannel check */

    /* instruct ARM to load DPD Model */
    extendedData[0] = MYKONOS_ARM_OBJECTID_GS_TRACKCALS;
    extendedData[1] = MYKONOS_ARM_DPD_INIT_MODEL;
    extendedData[2] = txChannel;

    retVal = MYKONOS_sendArmCommand(device, MYKONOS_ARM_SET_OPCODE, &extendedData[0], sizeof(extendedData));
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    /* wait max of 1sec for command to complete */
    retVal = MYKONOS_waitArmCmdStatus(device, MYKONOS_ARM_SET_OPCODE, timeoutMs, &cmdStatusByte);
    if (retVal != MYKONOS_ERR_OK)
    {
        if (cmdStatusByte > 0)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_RESTDPDMOD_ARMERRFLAG,
                    getMykonosErrorMessage(MYKONOS_ERR_RESTDPDMOD_ARMERRFLAG));
            return MYKONOS_ERR_RESTDPDMOD_ARMERRFLAG;
        }
        return retVal;
    }

    return MYKONOS_ERR_OK;
}

/**
 * \brief This function called during RadioOff, will allow retrevial of the DPD model file.
 *
 *  This function reads a copy of the DPD model from ARM memory to user memory specified by the modelDataBuffer pointer.
 *  The user must provide the correct buffer size with the modelNumberBytes argument.
 *
 * \param device is structure pointer to the Mykonos data structure containing settings
 * \param txChannel Desired Transmit channel to read back DPD status for (Valid ENUM type mykonosTxChannels_t: TX1 or TX2 or TX1_TX2)
 * \param modelDataBuffer a pointer to the user buffer where the model/rms data is to be written.
 *        Valid sizes: 172 bytes for a single model load to either TX1 or TX2.
 *                     344 bytes for a dual model load to both TX1_TX2, where the TX1 model/rms data will occupy the first 172 bytes
 *                          and TX2 model/rms data will occupy the second 172 bytes.
 * \param modelNumberBytes is the total buffer size of the user rms/model data buffer. Allowed sizes are 172 bytes for TX1 or TX2 and
 *        344 bytes for TX1_TX2.
 *
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_SAVEDPDMODEL_ARMSTATE_ERROR ARM is not in the RadioOff state, call MYKONOS_radioOff()
 * \retval MYKONOS_ERR_SAVEDPDMODEL_BUFFERSIZE_ERROR suppled modelNumberBytes size is incorrect. TX1 or TX2 = 172,  TX1_TX2 = 344
 * \retval MYKONOS_ERR_SAVEDPDMODEL_INVALID_TXCHANNEL_ERROR supplied txChannel does not match TX1 or TX2 or TX1_TX2
 */
mykonosErr_t MYKONOS_saveDpdModel(mykonosDevice_t *device, mykonosTxChannels_t txChannel, uint8_t *modelDataBuffer, uint32_t modelNumberBytes)
{
    uint32_t radioStatus = 0;
    mykonosErr_t retVal = MYKONOS_ERR_OK;
    uint16_t modelSize = 0;
    uint16_t rmsSize = 0;
    uint8_t armData[4] = {0};
    uint32_t armBufferAddr = 0;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_saveDpdModel()\n");
#endif

    /* read radio state to make sure ARM is in radioOff /IDLE */
    retVal = MYKONOS_getRadioState(device, &radioStatus);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    /* throw error if not in radioOff/IDLE state */
    if (((radioStatus & 0x03) != MYKONOS_ARM_SYSTEMSTATE_IDLE) && ((radioStatus & 0x03) != MYKONOS_ARM_SYSTEMSTATE_READY))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SAVDPDMOD_ARMSTATE, getMykonosErrorMessage(MYKONOS_ERR_SAVDPDMOD_ARMSTATE)); /* radio not off or initialized */
        return MYKONOS_ERR_SAVDPDMOD_ARMSTATE;
    }

    /* retrieve the model buffer byte count */
    retVal = MYKONOS_readArmMem(device, (MYKONOS_ADDR_ARM_START_PROG_ADDR + MYKONOS_ADDR_DPD_MODEL_BUF_SIZE), &armData[0], 2, 1);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }
    modelSize = (((uint32_t)armData[0]) | (((uint32_t)armData[1]) << 8));

    /* retrieve the rms buffer byte count */
    retVal = MYKONOS_readArmMem(device, (MYKONOS_ADDR_ARM_START_PROG_ADDR + MYKONOS_ADDR_DPD_RMS_BUF_SIZE), &armData[0], 2, 1);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }
    rmsSize = (((uint32_t)armData[0]) | (((uint32_t)armData[1]) << 8));

    if (txChannel == TX1)
    {
        /* check for single valid provided buffer size */
        if (modelNumberBytes != (modelSize + rmsSize))
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_RESTDPDMOD_WRONGBUFFERSIZE,
                    getMykonosErrorMessage(MYKONOS_ERR_RESTDPDMOD_WRONGBUFFERSIZE));
            return MYKONOS_ERR_RESTDPDMOD_WRONGBUFFERSIZE; /* wrong buffer size */
        }

        /* retrieve the arm TX1 model buffer address */
        retVal = MYKONOS_readArmMem(device, MYKONOS_ADDR_ARM_START_PROG_ADDR + MYKONOS_ADDR_TX1_DPD_MODEL_INDIRECT_PTR, &armData[0], 4, 1);
        if (retVal != MYKONOS_ERR_OK)
        {
            return retVal;
        }
        armBufferAddr = (((uint32_t)armData[0]) | (((uint32_t)armData[1]) << 8) | (((uint32_t)armData[2]) << 16) | (((uint32_t)armData[3]) << 24));

        /* read the TX1 model */
        retVal = MYKONOS_readArmMem(device, armBufferAddr, modelDataBuffer, modelSize, 1);
        if (retVal != MYKONOS_ERR_OK)
        {
            return retVal;
        }
        modelDataBuffer += modelSize;

        /* retrieve the arm TX1 rms buffer address */
        retVal = MYKONOS_readArmMem(device, MYKONOS_ADDR_ARM_START_PROG_ADDR + MYKONOS_ADDR_TX1_DPD_RMS_INDIRECT_PTR, &armData[0], 4, 1);
        if (retVal != MYKONOS_ERR_OK)
        {
            return retVal;
        }
        armBufferAddr = (((uint32_t)armData[0]) | (((uint32_t)armData[1]) << 8) | (((uint32_t)armData[2]) << 16) | (((uint32_t)armData[3]) << 24));

        /* read the TX1 rms */
        retVal = MYKONOS_readArmMem(device, armBufferAddr, modelDataBuffer, rmsSize, 1);
        if (retVal != MYKONOS_ERR_OK)
        {
            return retVal;
        }
    }/*end TX1 */

    else if (txChannel == TX2)
    {
        /* check for single valid provided buffer size */
        if (modelNumberBytes != (modelSize + rmsSize))
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_RESTDPDMOD_WRONGBUFFERSIZE,
                    getMykonosErrorMessage(MYKONOS_ERR_RESTDPDMOD_WRONGBUFFERSIZE));
            return MYKONOS_ERR_RESTDPDMOD_WRONGBUFFERSIZE; /* wrong buffer size */
        }

        /* retrieve the arm TX2 model buffer address */
        retVal = MYKONOS_readArmMem(device, MYKONOS_ADDR_ARM_START_PROG_ADDR + MYKONOS_ADDR_TX2_DPD_MODEL_INDIRECT_PTR, &armData[0], 4, 1);
        if (retVal != MYKONOS_ERR_OK)
        {
            return retVal;
        }
        armBufferAddr = (((uint32_t)armData[0]) | (((uint32_t)armData[1]) << 8) | (((uint32_t)armData[2]) << 16) | (((uint32_t)armData[3]) << 24));

        /* read the TX2 model */
        retVal = MYKONOS_readArmMem(device, armBufferAddr, modelDataBuffer, modelSize, 1);
        if (retVal != MYKONOS_ERR_OK)
        {
            return retVal;
        }
        modelDataBuffer += modelSize;

        /* retrieve the arm TX2 rms buffer address */
        retVal = MYKONOS_readArmMem(device, MYKONOS_ADDR_ARM_START_PROG_ADDR + MYKONOS_ADDR_TX2_DPD_RMS_INDIRECT_PTR, &armData[0], 4, 1);
        if (retVal != MYKONOS_ERR_OK)
        {
            return retVal;
        }
        armBufferAddr = (((uint32_t)armData[0]) | (((uint32_t)armData[1]) << 8) | (((uint32_t)armData[2]) << 16) | (((uint32_t)armData[3]) << 24));

        /* read the TX2 rms */
        retVal = MYKONOS_readArmMem(device, armBufferAddr, modelDataBuffer, rmsSize, 1);
        if (retVal != MYKONOS_ERR_OK)
        {
            return retVal;
        }
    }/*end TX2 */

    /* check for dual txChannel type */
    else if (txChannel == TX1_TX2)
    {
        /* check for single valid provided buffer size */
        if (modelNumberBytes != (2 * (modelSize + rmsSize)))
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_RESTDPDMOD_WRONGBUFFERSIZE,
                    getMykonosErrorMessage(MYKONOS_ERR_RESTDPDMOD_WRONGBUFFERSIZE));
            return MYKONOS_ERR_RESTDPDMOD_WRONGBUFFERSIZE; /* wrong buffer size */
        }

        /* retrieve the arm TX1 model buffer address */
        retVal = MYKONOS_readArmMem(device, MYKONOS_ADDR_ARM_START_PROG_ADDR + MYKONOS_ADDR_TX1_DPD_MODEL_INDIRECT_PTR, &armData[0], 4, 1);
        if (retVal != MYKONOS_ERR_OK)
        {
            return retVal;
        }
        armBufferAddr = (((uint32_t)armData[0]) | (((uint32_t)armData[1]) << 8) | (((uint32_t)armData[2]) << 16) | (((uint32_t)armData[3]) << 24));

        /* read the TX1 model */
        retVal = MYKONOS_readArmMem(device, armBufferAddr, modelDataBuffer, modelSize, 1);
        if (retVal != MYKONOS_ERR_OK)
        {
            return retVal;
        }
        modelDataBuffer += modelSize;

        /* retrieve the arm TX1 rms buffer address */
        retVal = MYKONOS_readArmMem(device, MYKONOS_ADDR_ARM_START_PROG_ADDR + MYKONOS_ADDR_TX1_DPD_RMS_INDIRECT_PTR, &armData[0], 4, 1);
        if (retVal != MYKONOS_ERR_OK)
        {
            return retVal;
        }
        armBufferAddr = (((uint32_t)armData[0]) | (((uint32_t)armData[1]) << 8) | (((uint32_t)armData[2]) << 16) | (((uint32_t)armData[3]) << 24));

        /* read the TX1 rms */
        retVal = MYKONOS_readArmMem(device, armBufferAddr, modelDataBuffer, rmsSize, 1);
        if (retVal != MYKONOS_ERR_OK)
        {
            return retVal;
        }
        modelDataBuffer += rmsSize;

        /* retrieve the arm TX2 model buffer address */
        retVal = MYKONOS_readArmMem(device, MYKONOS_ADDR_ARM_START_PROG_ADDR + MYKONOS_ADDR_TX2_DPD_MODEL_INDIRECT_PTR, &armData[0], 4, 1);
        if (retVal != MYKONOS_ERR_OK)
        {
            return retVal;
        }
        armBufferAddr = (((uint32_t)armData[0]) | (((uint32_t)armData[1]) << 8) | (((uint32_t)armData[2]) << 16) | (((uint32_t)armData[3]) << 24));

        /* read the TX2 model */
        retVal = MYKONOS_readArmMem(device, armBufferAddr, modelDataBuffer, modelSize, 1);
        if (retVal != MYKONOS_ERR_OK)
        {
            return retVal;
        }
        modelDataBuffer += modelSize;

        /* retrieve the arm TX2 rms buffer address */
        retVal = MYKONOS_readArmMem(device, MYKONOS_ADDR_ARM_START_PROG_ADDR + MYKONOS_ADDR_TX2_DPD_RMS_INDIRECT_PTR, &armData[0], 4, 1);
        if (retVal != MYKONOS_ERR_OK)
        {
            return retVal;
        }
        armBufferAddr = (((uint32_t)armData[0]) | (((uint32_t)armData[1]) << 8) | (((uint32_t)armData[2]) << 16) | (((uint32_t)armData[3]) << 24));

        /* read the TX2 rms */
        retVal = MYKONOS_readArmMem(device, armBufferAddr, modelDataBuffer, rmsSize, 1);
        if (retVal != MYKONOS_ERR_OK)
        {
            return retVal;
        }
    }/* end two txChannel */

    /* invalid txChannel for this api command */
    else
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SAVDPDMOD_INVALID_TXCHANNEL,
                getMykonosErrorMessage(MYKONOS_ERR_SAVDPDMOD_INVALID_TXCHANNEL));
        return MYKONOS_ERR_SAVDPDMOD_INVALID_TXCHANNEL; /* invalid txChannel */
    }/* end invalid txChannel check */

    return MYKONOS_ERR_OK;
}

/**
 * \brief This function sets the state of the DPD actuator.
 *
 *  This function can be called in either Radio On or Off state.
 *
 * \pre A AD9373 device is required for DPD to be enabled. DPD init cal has been run and DPD tracking enable.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is structure pointer to the Mykonos data structure containing settings
 * \param txChannel Desired Transmit channel to set the Actuator State (Valid ENUM values: TX1, TX2 or TX1_TX2)
 * \param actState Desired actuator state for the DPD, valid states are 0-disable and 1-enable
 *
 * \retval MYKONOS_ERR_SETDPDACT_INV_TXCHANNEL ERROR: Tx channel is not valid (Valid ENUM values: TX1, TX2 or TX1_TX2)
 * \retval MYKONOS_ERR_SETDPDACT_INV_STATE ERROR: Invalid Actuator state, valid states are 0-disable and 1-enable.
 * \retval MYKONOS_ERR_SETDPDACT_ARMERRFLAG ERROR: ARM command flag error set
 * \retval MYKONOS_ERR_OK Function completed successfully
 */
mykonosErr_t MYKONOS_setDpdActState(mykonosDevice_t *device, mykonosTxChannels_t txChannel, uint8_t actState)
{
    mykonosErr_t retVal = MYKONOS_ERR_OK;
    uint32_t timeoutMs = 1000;
    uint8_t extData[3] = {0};
    uint8_t actuatorSet = 0x00;
    uint8_t cmdStatusByte = 0;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_setDpdActuator()\n");
#endif

    switch (txChannel)
    {
        case TX1:
            break;
        case TX2:
            break;
        case TX1_TX2:
            break;

        default:
            /* this function allow single channel gain only */
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SETDPDACT_INV_TXCHANNEL,
                    getMykonosErrorMessage(MYKONOS_ERR_SETDPDACT_INV_TXCHANNEL));
            return MYKONOS_ERR_SETDPDACT_INV_TXCHANNEL;
    }
    switch (actState)
    {
        case 0:
            actuatorSet = DISABLE_DPD_ACTUATOR;
            break;
        case 1:
            actuatorSet = ENABLE_DPD_ACTUATOR;
            break;

        default:
            /* this function allow single channel gain only */
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SETDPDACT_INV_STATE,
                    getMykonosErrorMessage(MYKONOS_ERR_SETDPDACT_INV_STATE));
            return MYKONOS_ERR_SETDPDACT_INV_STATE;
    }

    extData[0] = MYKONOS_ARM_OBJECTID_TRACKING_CAL_CONTROL;
    extData[1] = actuatorSet;
    extData[2] = (uint8_t)txChannel;

    retVal = MYKONOS_sendArmCommand(device, MYKONOS_ARM_SET_OPCODE, &extData[0], sizeof(extData));
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    /* check for completion */
    retVal = MYKONOS_waitArmCmdStatus(device, MYKONOS_ARM_SET_OPCODE, timeoutMs, &cmdStatusByte);
    if (retVal != MYKONOS_ERR_OK)
    {
        if (cmdStatusByte > 0)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SETDPDACT_ARMERRFLAG,
                    getMykonosErrorMessage(MYKONOS_ERR_SETDPDACT_ARMERRFLAG));
            return MYKONOS_ERR_SETDPDACT_ARMERRFLAG;
        }

        return retVal;
    }

    if (cmdStatusByte > 0)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SETDPDACT_ARMERRFLAG,
                getMykonosErrorMessage(MYKONOS_ERR_SETDPDACT_ARMERRFLAG));
        return MYKONOS_ERR_SETDPDACT_ARMERRFLAG;
    }

    return MYKONOS_ERR_OK;
}

/**
 * \brief This function, when called during RadioOff, will configure CLGC settings
 *
 *  A AD9373 device is required for CLGC to be enabled.  The CLGC has several user
 *  adjustable settings that can be configured before running the runInitCals
 *  with the calMask set for the CLGC init cal. Call this function with desired
 *  settings set before running the CLGC init cal or enabling CLGC tracking.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 * - device->profilesValid
 * - device->tx->clgcConfig (All members)
 *
 * \retval MYKONOS_ERR_CFGCLGC_TXORX_PROFILE_INV ERROR: Tx and ObsRx profiles must be valid to use the CLGC feature
 * \retval MYKONOS_ERR_CFGCLGC_NULL_CLGCCFGSTRUCT ERROR: CLGC config structure pointer is null in device->tx->clgcConfig
 * \retval MYKONOS_ERR_CFGCLGC_ARMSTATE_ERROR ERROR: ARM is not in the IDLE(radioOff) or Ready state.
 * \retval MYKONOS_ERR_CFGCLGC_INV_DESIREDGAIN ERROR: CLGC tx1DesiredGain or tx2DesiredGain parameter is out of range
 * \retval MYKONOS_ERR_CFGCLGC_INV_TXATTENLIMIT ERROR: CLGC tx1AttenLimit or tx2AttenLimit parameter is out of range
 * \retval MYKONOS_ERR_CFGCLGC_INV_CLGC_CTRLRATIO ERROR: clgcControlRatio parameter is out of range
 */
mykonosErr_t MYKONOS_configClgc(mykonosDevice_t *device)
{
    uint32_t radioStatus = 0;
    mykonosErr_t retVal = MYKONOS_ERR_OK;
    uint8_t armFieldValue[4] = {0};
    uint8_t byteOffset = 0;
    uint16_t negPnLevel = 0;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_configClgc()\n");
#endif

    /* CLGC requires Tx and ORx enabled, throw error if both are not enabled */
    if ((device->profilesValid & (TX_PROFILE_VALID | ORX_PROFILE_VALID)) != (TX_PROFILE_VALID | ORX_PROFILE_VALID))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_CFGCLGC_TXORX_PROFILE_INV,
                getMykonosErrorMessage(MYKONOS_ERR_CFGCLGC_TXORX_PROFILE_INV));
        return MYKONOS_ERR_CFGCLGC_TXORX_PROFILE_INV;
    }

    if (device->tx->clgcConfig == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_CFGCLGC_NULL_CLGCCFGSTRUCT,
                getMykonosErrorMessage(MYKONOS_ERR_CFGCLGC_NULL_CLGCCFGSTRUCT));
        return MYKONOS_ERR_CFGCLGC_NULL_CLGCCFGSTRUCT;
    }

    /* read radio state to make sure ARM is in radioOff /IDLE */
    retVal = MYKONOS_getRadioState(device, &radioStatus);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    /* throw error if not in radioOff/IDLE state */
    if (((radioStatus & 0x03) != MYKONOS_ARM_SYSTEMSTATE_IDLE) && ((radioStatus & 0x03) != MYKONOS_ARM_SYSTEMSTATE_READY))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_CFGCLGC_ARMSTATE_ERROR,
                getMykonosErrorMessage(MYKONOS_ERR_CFGCLGC_ARMSTATE_ERROR));
        return MYKONOS_ERR_CFGCLGC_ARMSTATE_ERROR;
    }

    /* set CLGC desired gain parameter */
    if ((device->tx->clgcConfig->tx1DesiredGain < -10000) || (device->tx->clgcConfig->tx1DesiredGain > 10000) || (device->tx->clgcConfig->tx2DesiredGain < -10000)
            || (device->tx->clgcConfig->tx2DesiredGain > 10000))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_CFGCLGC_INV_DESIREDGAIN,
                getMykonosErrorMessage(MYKONOS_ERR_CFGCLGC_INV_DESIREDGAIN));
        return MYKONOS_ERR_CFGCLGC_INV_DESIREDGAIN;
    }

    armFieldValue[0] = (device->tx->clgcConfig->tx1DesiredGain & 0xFF);
    armFieldValue[1] = ((device->tx->clgcConfig->tx1DesiredGain >> 8) & 0xFF);
    armFieldValue[2] = (device->tx->clgcConfig->tx2DesiredGain & 0xFF);
    armFieldValue[3] = ((device->tx->clgcConfig->tx2DesiredGain >> 8) & 0xFF);

    byteOffset = 0;
    retVal = MYKONOS_writeArmConfig(device, MYKONOS_ARM_OBJECTID_CLGCCONFIG, byteOffset, &armFieldValue[0], 4);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    /* set CLGC Tx Atten limit parameter */
    if ((device->tx->clgcConfig->tx1AttenLimit > 40) || (device->tx->clgcConfig->tx2AttenLimit > 40))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_CFGCLGC_INV_TXATTENLIMIT,
                getMykonosErrorMessage(MYKONOS_ERR_CFGCLGC_INV_TXATTENLIMIT));
        return MYKONOS_ERR_CFGCLGC_INV_TXATTENLIMIT;
    }

    armFieldValue[0] = (device->tx->clgcConfig->tx1AttenLimit & 0xFF);
    armFieldValue[1] = ((device->tx->clgcConfig->tx1AttenLimit >> 8) & 0xFF);
    armFieldValue[2] = (device->tx->clgcConfig->tx2AttenLimit & 0xFF);
    armFieldValue[3] = ((device->tx->clgcConfig->tx2AttenLimit >> 8) & 0xFF);

    byteOffset = 8;
    retVal = MYKONOS_writeArmConfig(device, MYKONOS_ARM_OBJECTID_CLGCCONFIG, byteOffset, &armFieldValue[0], 4);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    /* set CLGC Control Ratio parameter */
    if ((device->tx->clgcConfig->tx1ControlRatio < 1) || (device->tx->clgcConfig->tx1ControlRatio > 100) || (device->tx->clgcConfig->tx2ControlRatio < 1)
            || (device->tx->clgcConfig->tx2ControlRatio > 100))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_CFGCLGC_INV_CLGC_CTRLRATIO,
                getMykonosErrorMessage(MYKONOS_ERR_CFGCLGC_INV_CLGC_CTRLRATIO));
        return MYKONOS_ERR_CFGCLGC_INV_CLGC_CTRLRATIO;
    }

    armFieldValue[0] = (device->tx->clgcConfig->tx1ControlRatio & 0xFF);
    armFieldValue[1] = ((device->tx->clgcConfig->tx1ControlRatio >> 8) & 0xFF);
    armFieldValue[2] = (device->tx->clgcConfig->tx2ControlRatio & 0xFF);
    armFieldValue[3] = ((device->tx->clgcConfig->tx2ControlRatio >> 8) & 0xFF);

    byteOffset = 12;
    retVal = MYKONOS_writeArmConfig(device, MYKONOS_ARM_OBJECTID_CLGCCONFIG, byteOffset, &armFieldValue[0], 4);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    /* set CLGC additional delay offset parameter (init cal)*/
    if ((device->tx->clgcConfig->additionalDelayOffset < -64) || (device->tx->clgcConfig->additionalDelayOffset > 64))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_CFGCLGC_INV_CLGC_ADDDELAY,
                getMykonosErrorMessage(MYKONOS_ERR_CFGCLGC_INV_CLGC_ADDDELAY));
        return MYKONOS_ERR_CFGCLGC_INV_CLGC_ADDDELAY;
    }

    armFieldValue[0] = (device->tx->clgcConfig->additionalDelayOffset & 0xFF);
    armFieldValue[1] = ((device->tx->clgcConfig->additionalDelayOffset >> 8) & 0xFF);
    byteOffset = 2;
    retVal = MYKONOS_writeArmConfig(device, MYKONOS_ARM_OBJECTID_CLGCINIT_CONFIG, byteOffset, &armFieldValue[0], 2);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    if ((device->tx->clgcConfig->pathDelayPnSeqLevel < 1) || (device->tx->clgcConfig->pathDelayPnSeqLevel > 8191))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_CFGCLGC_INV_PNSEQLEVEL,
                getMykonosErrorMessage(MYKONOS_ERR_CFGCLGC_INV_PNSEQLEVEL));
        return MYKONOS_ERR_CFGCLGC_INV_PNSEQLEVEL;
    }

    /* Set Path Delay PN sequence amplitude - positive and negative (init cal)*/
    armFieldValue[0] = (device->tx->clgcConfig->pathDelayPnSeqLevel & 0xFF);
    armFieldValue[1] = ((device->tx->clgcConfig->pathDelayPnSeqLevel >> 8) & 0xFF);

    negPnLevel = ((~device->tx->clgcConfig->pathDelayPnSeqLevel) + 1); /* times -1 */
    armFieldValue[2] = (negPnLevel & 0xFF);
    armFieldValue[3] = (((negPnLevel) >> 8) & 0xFF);
    byteOffset = 10;
    retVal = MYKONOS_writeArmConfig(device, MYKONOS_ARM_OBJECTID_CLGCINIT_CONFIG, byteOffset, &armFieldValue[0], 4);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    retVal = enableClgcTracking(device, ((device->tx->clgcConfig->allowTx1AttenUpdates > 0) ? 1 : 0), ((device->tx->clgcConfig->allowTx2AttenUpdates > 0) ? 1 : 0));
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    return MYKONOS_ERR_OK;
}

/* \brief Reads the CLGC config structure from ARM memory
 *
 *  This function reads the Closed Loop Gain Control structure
 *  from ARM memory and returns in the device->tx->clgcConfig structure.
 *
 *  A AD9373 device is required for CLGC to be enabled.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 * - device->profilesValid
 * - device->tx->clgcConfig (All members)
 *
 * \param device is structure pointer to the Mykonos data structure containing settings
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_GETCLGCCFG_TXORX_PROFILE_INV Error: Tx and ORx profiles must be valid for CLGC functions
 * \retval MYKONOS_ERR_GETCLGCCFG_NULL_CFGSTRUCT Error: NULL pointer to device->tx->clgcConfig structure
 */
mykonosErr_t MYKONOS_getClgcConfig(mykonosDevice_t *device)
{
    uint16_t byteOffset = 0;
    uint8_t armMem[16] = {0};
    mykonosErr_t retVal = MYKONOS_ERR_OK;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_getClgcConfig()\n");
#endif

    /* CLGC requires Tx and ORx enabled, throw error if both are not enabled */
    if ((device->profilesValid & (TX_PROFILE_VALID | ORX_PROFILE_VALID)) != (TX_PROFILE_VALID | ORX_PROFILE_VALID))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETCLGCCFG_TXORX_PROFILE_INV,
                getMykonosErrorMessage(MYKONOS_ERR_GETCLGCCFG_TXORX_PROFILE_INV));
        return MYKONOS_ERR_GETCLGCCFG_TXORX_PROFILE_INV;
    }

    if (device->tx->clgcConfig == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETCLGCCFG_NULL_CFGSTRUCT,
                getMykonosErrorMessage(MYKONOS_ERR_GETCLGCCFG_NULL_CFGSTRUCT));
        return MYKONOS_ERR_GETCLGCCFG_NULL_CFGSTRUCT;
    }

    byteOffset = 0;
    retVal = MYKONOS_readArmConfig(device, MYKONOS_ARM_OBJECTID_CLGCCONFIG, byteOffset, &armMem[0], sizeof(armMem));
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    device->tx->clgcConfig->tx1DesiredGain = (int16_t)(((uint16_t)(armMem[1]) << 8) | (uint16_t)(armMem[0]));
    device->tx->clgcConfig->tx2DesiredGain = (int16_t)(((uint16_t)(armMem[3]) << 8) | (uint16_t)(armMem[2]));
    device->tx->clgcConfig->tx1AttenLimit = (((uint16_t)(armMem[9]) << 8) | (uint16_t)(armMem[8]));
    device->tx->clgcConfig->tx2AttenLimit = (((uint16_t)(armMem[11]) << 8) | (uint16_t)(armMem[10]));
    device->tx->clgcConfig->tx1ControlRatio = (((uint16_t)(armMem[13]) << 8) | (uint16_t)(armMem[12]));
    device->tx->clgcConfig->tx2ControlRatio = (((uint16_t)(armMem[15]) << 8) | (uint16_t)(armMem[14]));

    byteOffset = 2;
    retVal = MYKONOS_readArmConfig(device, MYKONOS_ARM_OBJECTID_CLGCINIT_CONFIG, byteOffset, &armMem[0], 2);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    device->tx->clgcConfig->additionalDelayOffset = (int16_t)(((uint16_t)(armMem[1]) << 8) | (uint16_t)(armMem[0]));

    byteOffset = 10;
    retVal = MYKONOS_readArmConfig(device, MYKONOS_ARM_OBJECTID_CLGCINIT_CONFIG, byteOffset, &armMem[0], 2);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    device->tx->clgcConfig->pathDelayPnSeqLevel = (((uint16_t)(armMem[1]) << 8) | (uint16_t)(armMem[0]));

    return MYKONOS_ERR_OK;
}

/**
 * \brief This function reads the CLGC calibration status from the Mykonos ARM processor.
 *
 * The Closed Loop Gain Control Status is read back from the ARM processor and
 * returned in the function parameter clgcStatus.
 *
 * A AD9373 device is required for this feature to be enabled.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is structure pointer to the Mykonos data structure containing settings
 * \param txChannel Desired Transmit channel to read back CLGC status for (Valid ENUM values: TX1 or TX2 only)
 * \param clgcStatus Pointer to a structure to return the status information to
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_GETCLGCSTATUS_NULLPARAM clgcStatus function parameter is a NULL pointer
 * \retval MYKONOS_ERR_GETCLGCSTATUS_INV_CH txChannel parameter is a non supported value.
 * \retval MYKONOS_ERR_GETCLGCSTATUS_ARMERRFLAG ARM reported an error while processing the GET ARM command
 */
mykonosErr_t MYKONOS_getClgcStatus(mykonosDevice_t *device, mykonosTxChannels_t txChannel, mykonosClgcStatus_t *clgcStatus)
{
    uint8_t extData[3] = {MYKONOS_ARM_OBJECTID_CAL_STATUS, MYKONOS_ARM_OBJECTID_CLGCCONFIG, 0};
    uint8_t armData[52] = {0};
    uint32_t timeoutMs = 0;
    uint8_t cmdStatusByte = 0;
    uint32_t offset = 0;
    mykonosErr_t retVal = MYKONOS_ERR_OK;
    uint8_t channelSelect = 0;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_getClgcStatus()\n");
#endif

    if (clgcStatus == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETCLGCSTATUS_NULLPARAM,
                getMykonosErrorMessage(MYKONOS_ERR_GETCLGCSTATUS_NULLPARAM));
        return MYKONOS_ERR_GETCLGCSTATUS_NULLPARAM;
    }

    switch (txChannel)
    {
        case TX1:
            channelSelect = 0;
            break;
        case TX2:
            channelSelect = 1;
            break;
        default:
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETCLGCSTATUS_INV_CH,
                    getMykonosErrorMessage(MYKONOS_ERR_GETCLGCSTATUS_INV_CH));
            return MYKONOS_ERR_GETCLGCSTATUS_INV_CH;
        }
    }

    extData[2] = channelSelect;

    retVal = MYKONOS_sendArmCommand(device, MYKONOS_ARM_GET_OPCODE, &extData[0], sizeof(extData));
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    timeoutMs = 1000;
    retVal = MYKONOS_waitArmCmdStatus(device, MYKONOS_ARM_GET_OPCODE, timeoutMs, &cmdStatusByte);
    if (retVal != MYKONOS_ERR_OK)
    {
        if (cmdStatusByte > 0)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETCLGCSTATUS_ARMERRFLAG,
                    getMykonosErrorMessage(MYKONOS_ERR_GETCLGCSTATUS_ARMERRFLAG));
            return MYKONOS_ERR_GETCLGCSTATUS_ARMERRFLAG;
        }

        return retVal;
    }

    if (cmdStatusByte > 0)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETCLGCSTATUS_ARMERRFLAG,
                getMykonosErrorMessage(MYKONOS_ERR_GETCLGCSTATUS_ARMERRFLAG));
        return MYKONOS_ERR_GETCLGCSTATUS_ARMERRFLAG;
    }

    /* read status from ARM memory */
    offset = 0;
    retVal = MYKONOS_readArmMem(device, (MYKONOS_ADDR_ARM_START_DATA_ADDR + offset), &armData[0], sizeof(armData), 1);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    clgcStatus->errorStatus = ((uint32_t)(armData[3]) << 24) | ((uint32_t)(armData[2]) << 16) | ((uint32_t)(armData[1]) << 8) | (uint32_t)(armData[0]);
    clgcStatus->desiredGain = (int32_t)(((uint32_t)(armData[23]) << 24) | ((uint32_t)(armData[22]) << 16) | ((uint32_t)(armData[21]) << 8) | (uint32_t)(armData[20]));
    clgcStatus->currentGain = (int32_t)(((uint32_t)(armData[27]) << 24) | ((uint32_t)(armData[26]) << 16) | ((uint32_t)(armData[25]) << 8) | (uint32_t)(armData[24]));
    clgcStatus->txGain = ((uint32_t)(armData[39]) << 24) | ((uint32_t)(armData[38]) << 16) | ((uint32_t)(armData[37]) << 8) | (uint32_t)(armData[36]);
    clgcStatus->txRms = (int32_t)(((uint32_t)(armData[43]) << 24) | ((uint32_t)(armData[42]) << 16) | ((uint32_t)(armData[41]) << 8) | (uint32_t)(armData[40]));
    clgcStatus->orxRms = (int32_t)(((uint32_t)(armData[47]) << 24) | ((uint32_t)(armData[46]) << 16) | ((uint32_t)(armData[45]) << 8) | (uint32_t)(armData[44]));
    clgcStatus->trackCount = ((uint32_t)(armData[51]) << 24) | ((uint32_t)(armData[50]) << 16) | ((uint32_t)(armData[49]) << 8) | (uint32_t)(armData[48]);

    return MYKONOS_ERR_OK;
}

/**
 * \brief This private helper function called during RadioOff, will allow CLGC tracking to be scheduled
 *        by the ARM when back in the RadioOn state.
 *
 *  This function sets a flag in the ARM memory that if the enableDpd tracking cal mask
 *  and this setting are both set, then the ARM will schedule closed loop gain control
 *  tracking to occur in the radioOn ARM state.
 *
 *  A AD9373 device is required for this feature to be enabled.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 * - device->tx->dpdConfig
 *
 * \param device is structure pointer to the Mykonos data structure containing settings
 * \param tx1Enable 0=disable CLGC Tx1 tracking, 1= enable CLGC Tx1 tracking
 * \param tx2Enable 0=disable CLGC Tx2 tracking, 1= enable CLGC Tx2 tracking
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_EN_CLGCTRACKING_ARMSTATE_ERROR ARM is not in the RadioOff state, call MYKONOS_radioOff()
 */
static mykonosErr_t enableClgcTracking(mykonosDevice_t *device, uint8_t tx1Enable, uint8_t tx2Enable)
{
    uint32_t radioStatus = 0;
    mykonosErr_t retVal = MYKONOS_ERR_OK;
    uint8_t enableBit[4] = {0};
    uint8_t byteOffset = 4;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_enableClgcTracking()\n");
#endif

    /* read radio state to make sure ARM is in radioOff /IDLE */
    retVal = MYKONOS_getRadioState(device, &radioStatus);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    /* throw error if not in radioOff/IDLE state */
    if (((radioStatus & 0x03) != MYKONOS_ARM_SYSTEMSTATE_IDLE) && ((radioStatus & 0x03) != MYKONOS_ARM_SYSTEMSTATE_READY))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_EN_CLGCTRACKING_ARMSTATE_ERROR,
                getMykonosErrorMessage(MYKONOS_ERR_EN_CLGCTRACKING_ARMSTATE_ERROR));
        return MYKONOS_ERR_EN_CLGCTRACKING_ARMSTATE_ERROR;
    }

    enableBit[0] = (tx1Enable > 0) ? 1 : 0;
    enableBit[1] = 0;
    enableBit[2] = (tx2Enable > 0) ? 1 : 0;
    enableBit[3] = 0;

    retVal = MYKONOS_writeArmConfig(device, MYKONOS_ARM_OBJECTID_CLGCCONFIG, byteOffset, &enableBit[0], 4);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    return MYKONOS_ERR_OK;
}

/**
 * \brief This function updates the CLGC desired gain parameter.
 *
 *  This function can be called in either Radio On or Off state.
 *
 * \pre A AD9373 device is required for CLGC to be enabled. CLGC init cal has been run and CLGC tracking enable.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is structure pointer to the Mykonos data structure containing settings
 *  \param txChannel Desired Transmit channel to set the (Valid ENUM values: TX1 or TX2 only)
 * \param gain Total gain and attenuation (dB * 100) for the selected channel txChannel
 *
 * \retval MYKONOS_ERR_SETCLGCGAIN_INV_TXCHANNEL ERROR: Tx channel is not valid (Valid ENUM values: TX1 or TX2 only)
 * \retval MYKONOS_ERR_SETCLGCGAIN_TRACK_ARMERRFLAG ERROR: ARM command flag error set
 * \retval MYKONOS_ERR_SETCLGCGAIN_INV_DESIREDGAIN ERROR: CLGC gain parameter is out of range, valid range is from -10000 to 10000.
 * \retval MYKONOS_ERR_OK Function completed successfully
 */
mykonosErr_t MYKONOS_setClgcGain(mykonosDevice_t *device, mykonosTxChannels_t txChannel, int16_t gain)
{
    mykonosErr_t retVal = MYKONOS_ERR_OK;
    uint32_t timeoutMs = 1000;
    uint8_t extData[4] = {0};
    uint8_t clcgChannel = 0x00;
    uint8_t cmdStatusByte = 0;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_setClgcGain()\n");
#endif

    /* Range check for desired gain parameter */
    if ((gain < -10000) || (gain > 10000))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SETCLGCGAIN_INV_DESIREDGAIN,
                getMykonosErrorMessage(MYKONOS_ERR_SETCLGCGAIN_INV_DESIREDGAIN));
        return MYKONOS_ERR_SETCLGCGAIN_INV_DESIREDGAIN;
    }

    switch (txChannel)
    {
        case TX1:
            clcgChannel = SET_CLGC_DESIRED_GAIN_1;
            break;
        case TX2:
            clcgChannel = SET_CLGC_DESIRED_GAIN_2;
            break;

        default:
            /* this function allow single channel gain only */
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SETCLGCGAIN_INV_TXCHANNEL,
                    getMykonosErrorMessage(MYKONOS_ERR_SETCLGCGAIN_INV_TXCHANNEL));
            return MYKONOS_ERR_SETCLGCGAIN_INV_TXCHANNEL;
    }

    extData[0] = MYKONOS_ARM_OBJECTID_TRACKING_CAL_CONTROL;
    extData[1] = clcgChannel;
    extData[2] = (gain & 0xFF);
    extData[3] = ((gain >> 8) & 0xFF);

    retVal = MYKONOS_sendArmCommand(device, MYKONOS_ARM_SET_OPCODE, &extData[0], sizeof(extData));
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    /* check for completion */
    retVal = MYKONOS_waitArmCmdStatus(device, MYKONOS_ARM_SET_OPCODE, timeoutMs, &cmdStatusByte);
    if (retVal != MYKONOS_ERR_OK)
    {
        if (cmdStatusByte > 0)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SETCLGCGAIN_TRACK_ARMERRFLAG,
                    getMykonosErrorMessage(MYKONOS_ERR_SETCLGCGAIN_TRACK_ARMERRFLAG));
            return MYKONOS_ERR_SETCLGCGAIN_TRACK_ARMERRFLAG;
        }

        return retVal;
    }

    if (cmdStatusByte > 0)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SETCLGCGAIN_TRACK_ARMERRFLAG,
                getMykonosErrorMessage(MYKONOS_ERR_SETCLGCGAIN_TRACK_ARMERRFLAG));
        return MYKONOS_ERR_SETCLGCGAIN_TRACK_ARMERRFLAG;
    }

    return MYKONOS_ERR_OK;
}

/**
 * \brief This function, when called during RadioOff, will configure VSWR settings
 *
 *  A AD9373 device is required for VSWR to be enabled.  The VSWR has several user
 *  adjustable settings that can be configured before running the runInitCals
 *  with the calMask set for the VSWR init cal. Call this function with desired
 *  settings set before running the VSWR init cal or enabling VSWR tracking.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 * - device->profilesValid
 * - device->tx->vswrConfig (All members)
 *
 * \retval MYKONOS_ERR_CFGVSWR_TXORX_PROFILE_INV ERROR: Tx and ObsRx profiles must be valid to use the VSWR feature
 * \retval MYKONOS_ERR_CFGVSWR_NULL_VSWRCFGSTRUCT ERROR: CLGC config structure pointer is null in device->tx->clgcConfig
 * \retval MYKONOS_ERR_CFGVSWR_ARMSTATE_ERROR ERROR: ARM is not in the IDLE(radioOff) or Ready state.
 */
mykonosErr_t MYKONOS_configVswr(mykonosDevice_t *device)
{
    uint32_t radioStatus = 0;
    mykonosErr_t retVal = MYKONOS_ERR_OK;
    uint8_t armFieldValue[16] = {0};
    uint8_t byteOffset = 0;
    uint16_t negPnLevel = 0;
    const uint8_t ENABLE_VSWR = 1;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_configVswr()\n");
#endif

    /* VSWR requires Tx and ORx enabled, throw error if both are not enabled */
    if ((device->profilesValid & (TX_PROFILE_VALID | ORX_PROFILE_VALID)) != (TX_PROFILE_VALID | ORX_PROFILE_VALID))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_CFGVSWR_TXORX_PROFILE_INV,
                getMykonosErrorMessage(MYKONOS_ERR_CFGVSWR_TXORX_PROFILE_INV));
        return MYKONOS_ERR_CFGVSWR_TXORX_PROFILE_INV;
    }

    if (device->tx->vswrConfig == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_CFGVSWR_NULL_VSWRCFGSTRUCT,
                getMykonosErrorMessage(MYKONOS_ERR_CFGVSWR_NULL_VSWRCFGSTRUCT));
        return MYKONOS_ERR_CFGVSWR_NULL_VSWRCFGSTRUCT;
    }

    /* read radio state to make sure ARM is in radioOff /IDLE */
    retVal = MYKONOS_getRadioState(device, &radioStatus);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    /* throw error if not in radioOff/IDLE state */
    if (((radioStatus & 0x03) != MYKONOS_ARM_SYSTEMSTATE_IDLE) && ((radioStatus & 0x03) != MYKONOS_ARM_SYSTEMSTATE_READY))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_CFGVSWR_ARMSTATE_ERROR,
                getMykonosErrorMessage(MYKONOS_ERR_CFGVSWR_ARMSTATE_ERROR));
        return MYKONOS_ERR_CFGVSWR_ARMSTATE_ERROR;
    }

    /* range check valid 3p3 GPIO pin */
    if ((device->tx->vswrConfig->tx1VswrSwitchGpio3p3Pin > 11) || (device->tx->vswrConfig->tx2VswrSwitchGpio3p3Pin > 11))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_CFGVSWR_INV_3P3GPIOPIN,
                getMykonosErrorMessage(MYKONOS_ERR_CFGVSWR_INV_3P3GPIOPIN));
        return MYKONOS_ERR_CFGVSWR_INV_3P3GPIOPIN;
    }

    armFieldValue[0] = ENABLE_VSWR;
    armFieldValue[1] = 0;
    armFieldValue[2] = ENABLE_VSWR;
    armFieldValue[3] = 0;
    armFieldValue[4] = (device->tx->vswrConfig->tx1VswrSwitchGpio3p3Pin & 0xFF);
    armFieldValue[5] = 0;
    armFieldValue[6] = (device->tx->vswrConfig->tx2VswrSwitchGpio3p3Pin & 0xFF);
    armFieldValue[7] = 0;
    armFieldValue[8] = (device->tx->vswrConfig->tx1VswrSwitchPolarity > 0) ? 1 : 0;
    armFieldValue[9] = 0;
    armFieldValue[10] = (device->tx->vswrConfig->tx2VswrSwitchPolarity > 0) ? 1 : 0;
    armFieldValue[11] = 0;
    armFieldValue[12] = device->tx->vswrConfig->tx1VswrSwitchDelay_ms; /* support full 0 to 255 ms range */
    armFieldValue[13] = 0;
    armFieldValue[14] = device->tx->vswrConfig->tx2VswrSwitchDelay_ms; /* support full 0 to 255 ms range */
    armFieldValue[15] = 0;
    byteOffset = 0;
    retVal = MYKONOS_writeArmConfig(device, MYKONOS_ARM_OBJECTID_VSWRCONFIG, byteOffset, &armFieldValue[0], 16);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    /* set VSWR additional delay offset parameter (init cal)*/
    if ((device->tx->vswrConfig->additionalDelayOffset < -64) || (device->tx->vswrConfig->additionalDelayOffset > 64))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_CFGVSWR_INV_VSWR_ADDDELAY,
                getMykonosErrorMessage(MYKONOS_ERR_CFGVSWR_INV_VSWR_ADDDELAY));
        return MYKONOS_ERR_CFGVSWR_INV_VSWR_ADDDELAY;
    }

    armFieldValue[0] = (device->tx->vswrConfig->additionalDelayOffset & 0xFF);
    armFieldValue[1] = ((device->tx->vswrConfig->additionalDelayOffset >> 8) & 0xFF);
    byteOffset = 2;
    retVal = MYKONOS_writeArmConfig(device, MYKONOS_ARM_OBJECTID_VSWRINIT_CONFIG, byteOffset, &armFieldValue[0], 2);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    if ((device->tx->vswrConfig->pathDelayPnSeqLevel < 1) || (device->tx->vswrConfig->pathDelayPnSeqLevel > 8191))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_CFGVSWR_INV_PNSEQLEVEL,
                getMykonosErrorMessage(MYKONOS_ERR_CFGVSWR_INV_PNSEQLEVEL));
        return MYKONOS_ERR_CFGVSWR_INV_PNSEQLEVEL;
    }

    /* Set Path Delay PN sequence amplitude - positive and negative (init cal)*/
    armFieldValue[0] = (device->tx->vswrConfig->pathDelayPnSeqLevel & 0xFF);
    armFieldValue[1] = ((device->tx->vswrConfig->pathDelayPnSeqLevel >> 8) & 0xFF);

    negPnLevel = ((~device->tx->vswrConfig->pathDelayPnSeqLevel) + 1); /* times -1 */
    armFieldValue[2] = (negPnLevel & 0xFF);
    armFieldValue[3] = (((negPnLevel) >> 8) & 0xFF);
    byteOffset = 10;
    /* Set for Tx1 channel */
    retVal = MYKONOS_writeArmConfig(device, MYKONOS_ARM_OBJECTID_VSWRINIT_CONFIG, byteOffset, &armFieldValue[0], 4);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    /* Set for Tx2 channel */
    byteOffset = 30;
    retVal = MYKONOS_writeArmConfig(device, MYKONOS_ARM_OBJECTID_VSWRINIT_CONFIG, byteOffset, &armFieldValue[0], 4);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    return MYKONOS_ERR_OK;
}

/* \brief Reads the VSWR config structure from ARM memory
 *
 *  This function reads the VSWR structure
 *  from ARM memory and returns in the device->tx->vswrConfig structure.
 *
 *  A AD9373 device is required for VSWR to be enabled.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 * - device->profilesValid
 * - device->tx->vswrConfig (All members)
 *
 * \param device is structure pointer to the Mykonos data structure containing settings
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_GETVSWRCFG_TXORX_PROFILE_INV Error: Tx and ORx profiles must be valid for VSWR functions
 * \retval MYKONOS_ERR_GETVSWRCFG_NULL_CFGSTRUCT Error: NULL pointer to device->tx->vswrConfig structure
 */
mykonosErr_t MYKONOS_getVswrConfig(mykonosDevice_t *device)
{
    uint16_t byteOffset = 0;
    uint8_t armMem[12] = {0};
    mykonosErr_t retVal = MYKONOS_ERR_OK;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_getVswrConfig()\n");
#endif

    /* VSWR requires Tx and ORx enabled, throw error if both are not enabled */
    if ((device->profilesValid & (TX_PROFILE_VALID | ORX_PROFILE_VALID)) != (TX_PROFILE_VALID | ORX_PROFILE_VALID))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETVSWRCFG_TXORX_PROFILE_INV,
                getMykonosErrorMessage(MYKONOS_ERR_GETVSWRCFG_TXORX_PROFILE_INV));
        return MYKONOS_ERR_GETVSWRCFG_TXORX_PROFILE_INV;
    }

    if (device->tx->vswrConfig == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETVSWRCFG_NULL_CFGSTRUCT,
                getMykonosErrorMessage(MYKONOS_ERR_GETVSWRCFG_NULL_CFGSTRUCT));
        return MYKONOS_ERR_GETVSWRCFG_NULL_CFGSTRUCT;
    }

    byteOffset = 4;
    retVal = MYKONOS_readArmConfig(device, MYKONOS_ARM_OBJECTID_VSWRCONFIG, byteOffset, &armMem[0], sizeof(armMem));
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    device->tx->vswrConfig->tx1VswrSwitchGpio3p3Pin = armMem[0];
    device->tx->vswrConfig->tx2VswrSwitchGpio3p3Pin = armMem[2];
    device->tx->vswrConfig->tx1VswrSwitchPolarity = armMem[4];
    device->tx->vswrConfig->tx2VswrSwitchPolarity = armMem[6];
    device->tx->vswrConfig->tx1VswrSwitchDelay_ms = armMem[8];
    device->tx->vswrConfig->tx2VswrSwitchDelay_ms = armMem[10];

    byteOffset = 2;
    retVal = MYKONOS_readArmConfig(device, MYKONOS_ARM_OBJECTID_VSWRINIT_CONFIG, byteOffset, &armMem[0], 2);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    device->tx->vswrConfig->additionalDelayOffset = (int16_t)(((uint16_t)(armMem[1]) << 8) | (uint16_t)(armMem[0]));

    byteOffset = 10;
    retVal = MYKONOS_readArmConfig(device, MYKONOS_ARM_OBJECTID_VSWRINIT_CONFIG, byteOffset, &armMem[0], 2);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    device->tx->vswrConfig->pathDelayPnSeqLevel = (((uint16_t)(armMem[1]) << 8) | (uint16_t)(armMem[0]));

    return MYKONOS_ERR_OK;
}

/**
 * \brief This function reads the VSWR calibration status from the Mykonos ARM processor.
 *
 * The VSWR Status is read back from the ARM processor and
 * returned in the function parameter vswrStatus.
 *
 * A AD9373 device is required for this feature to be enabled.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is structure pointer to the Mykonos data structure containing settings
 * \param txChannel Desired Transmit channel to read back VSWR status for (Valid ENUM values: TX1 or TX2 only)
 * \param vswrStatus Pointer to a structure to return the status information to
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_GETVSWRSTATUS_NULLPARAM vswrStatus function parameter is a NULL pointer
 * \retval MYKONOS_ERR_GETVSWRSTATUS_INV_CH txChannel parameter is a non supported value.
 * \retval MYKONOS_ERR_GETVSWRSTATUS_ARMERRFLAG ARM reported an error while processing the GET ARM command
 */
mykonosErr_t MYKONOS_getVswrStatus(mykonosDevice_t *device, mykonosTxChannels_t txChannel, mykonosVswrStatus_t *vswrStatus)
{
    uint8_t extData[3] = {MYKONOS_ARM_OBJECTID_CAL_STATUS, MYKONOS_ARM_OBJECTID_VSWRCONFIG, 0};
    uint8_t armData[64] = {0};
    uint32_t timeoutMs = 0;
    uint8_t cmdStatusByte = 0;
    uint32_t offset = 0;
    mykonosErr_t retVal = MYKONOS_ERR_OK;
    uint8_t channelSelect = 0;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_getVswrStatus()\n");
#endif

    if (vswrStatus == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETVSWRSTATUS_NULLPARAM,
                getMykonosErrorMessage(MYKONOS_ERR_GETVSWRSTATUS_NULLPARAM));
        return MYKONOS_ERR_GETVSWRSTATUS_NULLPARAM;
    }

    switch (txChannel)
    {
        case TX1:
            channelSelect = 0;
            break;
        case TX2:
            channelSelect = 1;
            break;
        default:
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETVSWRSTATUS_INV_CH,
                    getMykonosErrorMessage(MYKONOS_ERR_GETVSWRSTATUS_INV_CH));
            return MYKONOS_ERR_GETVSWRSTATUS_INV_CH;
        }
    }

    extData[2] = channelSelect;

    retVal = MYKONOS_sendArmCommand(device, MYKONOS_ARM_GET_OPCODE, &extData[0], sizeof(extData));
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    timeoutMs = 1000;
    retVal = MYKONOS_waitArmCmdStatus(device, MYKONOS_ARM_GET_OPCODE, timeoutMs, &cmdStatusByte);
    if (retVal != MYKONOS_ERR_OK)
    {
        /* throw more specific error message instead of returning error code from waitArmCmdStatus */
        if (cmdStatusByte > 0)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETVSWRSTATUS_ARMERRFLAG,
                    getMykonosErrorMessage(MYKONOS_ERR_GETVSWRSTATUS_ARMERRFLAG));
            return MYKONOS_ERR_GETVSWRSTATUS_ARMERRFLAG;
        }

        return retVal;
    }

    if (cmdStatusByte > 0)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETVSWRSTATUS_ARMERRFLAG,
                getMykonosErrorMessage(MYKONOS_ERR_GETVSWRSTATUS_ARMERRFLAG));
        return MYKONOS_ERR_GETVSWRSTATUS_ARMERRFLAG;
    }

    /* read status from ARM memory */
    offset = 0;
    retVal = MYKONOS_readArmMem(device, (MYKONOS_ADDR_ARM_START_DATA_ADDR + offset), &armData[0], sizeof(armData), 1);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    vswrStatus->errorStatus = ((uint32_t)(armData[3]) << 24) | ((uint32_t)(armData[2]) << 16) | ((uint32_t)(armData[1]) << 8) | (uint32_t)(armData[0]);
    vswrStatus->forwardGainRms_dB =
            (int32_t)(((uint32_t)(armData[23]) << 24) | ((uint32_t)(armData[22]) << 16) | ((uint32_t)(armData[21]) << 8) | (uint32_t)(armData[20]));
    vswrStatus->forwardGainReal = (int32_t)(((uint32_t)(armData[27]) << 24) | ((uint32_t)(armData[26]) << 16) | ((uint32_t)(armData[25]) << 8) | (uint32_t)(armData[24]));
    vswrStatus->forwardGainImag = (int32_t)(((uint32_t)(armData[31]) << 24) | ((uint32_t)(armData[30]) << 16) | ((uint32_t)(armData[29]) << 8) | (uint32_t)(armData[28]));
    vswrStatus->reflectedGainRms_dB = (int32_t)(((uint32_t)(armData[35]) << 24) | ((uint32_t)(armData[34]) << 16) | ((uint32_t)(armData[33]) << 8)
            | (uint32_t)(armData[32]));
    vswrStatus->reflectedGainReal =
            (int32_t)(((uint32_t)(armData[39]) << 24) | ((uint32_t)(armData[38]) << 16) | ((uint32_t)(armData[37]) << 8) | (uint32_t)(armData[36]));
    vswrStatus->reflectedGainImag =
            (int32_t)(((uint32_t)(armData[43]) << 24) | ((uint32_t)(armData[42]) << 16) | ((uint32_t)(armData[41]) << 8) | (uint32_t)(armData[40]));
    vswrStatus->trackCount = ((uint32_t)(armData[63]) << 24) | ((uint32_t)(armData[62]) << 16) | ((uint32_t)(armData[61]) << 8) | (uint32_t)(armData[60]);
    vswrStatus->vswr_forward_tx_rms = ((uint32_t)(armData[47]) << 24) | ((uint32_t)(armData[46]) << 16) | ((uint32_t)(armData[45]) << 8) | (uint32_t)(armData[44]);
    vswrStatus->vswr_forward_orx_rms = ((uint32_t)(armData[51]) << 24) | ((uint32_t)(armData[50]) << 16) | ((uint32_t)(armData[49]) << 8) | (uint32_t)(armData[48]);
    vswrStatus->vswr_reflection_tx_rms = ((uint32_t)(armData[55]) << 24) | ((uint32_t)(armData[54]) << 16) | ((uint32_t)(armData[53]) << 8) | (uint32_t)(armData[52]);
    vswrStatus->vswr_reflection_orx_rms = ((uint32_t)(armData[59]) << 24) | ((uint32_t)(armData[58]) << 16) | ((uint32_t)(armData[57]) << 8) | (uint32_t)(armData[56]);

    return MYKONOS_ERR_OK;
}

/**
 * \brief Read from the Mykonos ARM program or data memory
 *
 * Valid memory addresses are: Program Memory (0x01000000 - 0x01017FFF),
 * Data Memory (0x20000000 - 0x2000FFFF)
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 * - device->spiSettings
 *
 * \param device is structure pointer to the Mykonos data structure containing settings
 * \param address The 32bit ARM address to read from.
 * \param returnData Byte(uint8_t) array containing the data read from the ARM memory.
 * \param bytesToRead Number of bytes in the returnData array.
 * \param autoIncrement is boolean flag to enable or disable autoincrement of ARM register address
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_READARMMEM_INV_ADDR_PARM ARM memory address is out of range
 *
 */
mykonosErr_t MYKONOS_readArmMem(mykonosDevice_t *device, uint32_t address, uint8_t *returnData, uint32_t bytesToRead, uint8_t autoIncrement)
{
    uint8_t dataMem;
    uint32_t i;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_readArmMem()\n");
#endif

    /* check that start and stop address are in valid range */
    if ((!(address >= MYKONOS_ADDR_ARM_START_PROG_ADDR && address <= MYKONOS_ADDR_ARM_END_PROG_ADDR))
            && !(address >= MYKONOS_ADDR_ARM_START_DATA_ADDR && address <= MYKONOS_ADDR_ARM_END_DATA_ADDR))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_READARMMEM_INV_ADDR_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_READARMMEM_INV_ADDR_PARM));
        return MYKONOS_ERR_READARMMEM_INV_ADDR_PARM;
    }

    if ((!(((address + bytesToRead - 1) >= MYKONOS_ADDR_ARM_START_PROG_ADDR) && (address + bytesToRead - 1) <= MYKONOS_ADDR_ARM_END_PROG_ADDR))
            && (!(((address + bytesToRead - 1) >= MYKONOS_ADDR_ARM_START_DATA_ADDR) && (address + bytesToRead - 1) <= MYKONOS_ADDR_ARM_END_DATA_ADDR)))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_READARMMEM_INV_ADDR_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_READARMMEM_INV_ADDR_PARM));
        return MYKONOS_ERR_READARMMEM_INV_ADDR_PARM;
    }

    if (address >= MYKONOS_ADDR_ARM_START_DATA_ADDR && address <= MYKONOS_ADDR_ARM_END_DATA_ADDR)
    {
        dataMem = 1;
    }
    else
    {
        dataMem = 0;
    }

    /* NOT assuming auto increment bit is already set from MYKONOS_initARM function call */
    /* setting auto increment address bit if autoIncrement evaluates true */
    if (autoIncrement)
    {
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_ARM_CTL_1, 0x01, 0x04, 2);
    }
    else
    {
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_ARM_CTL_1, 0x00, 0x04, 2);
    }

    /* setting up for ARM read */
    CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_ARM_CTL_1, 0x01, 0x20, 5);
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_ARM_ADDR_BYTE_0, (uint8_t)((address) >> 2)); /* write address[9:2] */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_ARM_ADDR_BYTE_1, (uint8_t)(address >> 10) | (dataMem << 7)); /* write address[15:10] */

    /* start read-back at correct byte offset */
    /* read data is located at SPI address 0xD04=data[7:0], 0xD05=data[15:8], 0xD06=data[23:16], 0xD07=data[31:24]. */
    /* with address auto increment set, after xD07 is read, the address will automatically increment */
    /* without address auto increment set, 0x4 must be added to the address for correct indexing */
    if (autoIncrement)
    {
        for (i = 0; i < bytesToRead; i++)
        {
            CMB_SPIReadByte(device->spiSettings, (MYKONOS_ADDR_ARM_DATA_BYTE_0 | (((address & 0x3) + i) % 4)), &returnData[i]);
        }
    }
    else
    {
        for (i = 0; i < bytesToRead; i++)
        {
            CMB_SPIReadByte(device->spiSettings, (MYKONOS_ADDR_ARM_DATA_BYTE_0 | (((address & 0x3) + i) % 4)), &returnData[i]);

            if ((MYKONOS_ADDR_ARM_DATA_BYTE_0 | (((address & 0x3) + i) % 4)) == MYKONOS_ADDR_ARM_DATA_BYTE_3)
            {
                CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_ARM_ADDR_BYTE_0, (uint8_t)(((address + 0x4) & 0x3FF) >> 2));
                CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_ARM_ADDR_BYTE_1, ((address + 0x4) & 0x1FC00U) >> 10 | (dataMem << 7));
            }
        }
    }

    return MYKONOS_ERR_OK;
}

/**
 * \brief Write to the Mykonos ARM program or data memory
 *
 * Valid memory addresses are: Program Memory (0x01000000 - 0x01017FFF),
 * Data Memory (0x20000000 - 0x2000FFFF)
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 * - device->spiSettings
 *
 * \param device is structure pointer to the Mykonos data structure containing settings
 * \param address The 32bit ARM address to write to.
 * \param data Byte(uint8_t) array containing the data to write to the ARM memory.
 * \param byteCount Number of bytes in the data array.
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_WRITEARMMEM_NULL_PARM Function parameter data has NULL pointer when byteCount is >0
 * \retval MYKONOS_ERR_WRITEARMMEM_INV_ADDR_PARM ARM memory address is out of range of valid ARM memory
 */
mykonosErr_t MYKONOS_writeArmMem(mykonosDevice_t *device, uint32_t address, uint8_t *data, uint32_t byteCount)
{
    /* write address, then data with auto increment enabled. */
    uint8_t dataMem;
    uint32_t i;

#if MYK_ENABLE_SPIWRITEARRAY == 1
    uint32_t addrIndex = 0;
    uint32_t dataIndex = 0;
    uint32_t spiBufferSize = MYK_SPIWRITEARRAY_BUFFERSIZE;
    uint16_t addrArray[MYK_SPIWRITEARRAY_BUFFERSIZE] = {0};
#endif

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_writeArmMem()\n");
#endif

    if ((data == NULL) && (byteCount > 0))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_WRITEARMMEM_NULL_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_WRITEARMMEM_NULL_PARM));
        return MYKONOS_ERR_WRITEARMMEM_NULL_PARM;
    }

    if ((!(address >= MYKONOS_ADDR_ARM_START_PROG_ADDR && address <= MYKONOS_ADDR_ARM_END_PROG_ADDR))
            && !(address >= MYKONOS_ADDR_ARM_START_DATA_ADDR && address <= MYKONOS_ADDR_ARM_END_DATA_ADDR))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_WRITEARMMEM_INV_ADDR_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_WRITEARMMEM_INV_ADDR_PARM));
        return MYKONOS_ERR_WRITEARMMEM_INV_ADDR_PARM;
    }

    if ((!(((address + byteCount - 1) >= MYKONOS_ADDR_ARM_START_PROG_ADDR) && ((address + byteCount - 1) <= MYKONOS_ADDR_ARM_END_PROG_ADDR)))
            && (!(((address + byteCount - 1) >= MYKONOS_ADDR_ARM_START_DATA_ADDR) && ((address + byteCount - 1) <= MYKONOS_ADDR_ARM_END_DATA_ADDR))))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_WRITEARMMEM_INV_ADDR_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_WRITEARMMEM_INV_ADDR_PARM));
        return MYKONOS_ERR_WRITEARMMEM_INV_ADDR_PARM;
    }

    if (address >= MYKONOS_ADDR_ARM_START_DATA_ADDR && address <= MYKONOS_ADDR_ARM_END_DATA_ADDR)
    {
        dataMem = 1;
    }
    else
    {
        dataMem = 0;
    }

    /* set auto increment address bit */
    CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_ARM_CTL_1, 0x01, 0x04, 2);

    /* writing the address */
    CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_ARM_CTL_1, 0x00, 0x20, 5);
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_ARM_ADDR_BYTE_0, (uint8_t)((address) >> 2));
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_ARM_ADDR_BYTE_1, (uint8_t)(address >> 10) | (dataMem << 7));

    /* start write at correct byte offset */
    /* write data is located at SPI address 0xD04=data[7:0], 0xD05=data[15:8], 0xD06=data[23:16], 0xD07=data[31:24] */
    /* with address auto increment set, after x407 is written, the address will automatically increment */

#if (MYK_ENABLE_SPIWRITEARRAY == 0)

    for (i = 0; i < byteCount; i++)
    {
        CMB_SPIWriteByte(device->spiSettings, (MYKONOS_ADDR_ARM_DATA_BYTE_0 | (((address & 0x3) + i) % 4)), data[i]);
    }

#elif (MYK_ENABLE_SPIWRITEARRAY == 1)

    addrIndex = 0;
    dataIndex = 0;
    for (i = 0; i < byteCount; i++)
    {
        addrArray[addrIndex++] = (MYKONOS_ADDR_ARM_DATA_BYTE_0 | (((address & 0x3) + i) % 4));

        if (addrIndex == spiBufferSize)
        {
            CMB_SPIWriteBytes(device->spiSettings, &addrArray[0], &data[dataIndex], addrIndex);
            dataIndex = dataIndex + addrIndex;
            addrIndex = 0;
        }
    }

    if (addrIndex > 0)
    {
        CMB_SPIWriteBytes(device->spiSettings, &addrArray[0], &data[dataIndex], addrIndex);
    }

#endif

    return MYKONOS_ERR_OK;
}

/**
 * \brief Low level helper function used by Mykonos API to write the ARM memory config structures
 *
 * Normally this function should not be required to be used directly by the BBIC.  This is a helper
 * function used by other Mykonos API commands to write settings into the ARM memory.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 * - device->spiSettings
 *
 * \param device is structure pointer to the Mykonos data structure containing settings
 * \param objectId ARM id of a particular structure or setting in ARM memory
 * \param offset Byte offset from the start of the objectId's memory location in ARM memory
 * \param data A byte array containing data to write to the ARM memory buffer.
 * \param byteCount Number of bytes in the data array (Valid size = 1-255 bytes)
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_WRITEARMCFG_ARMERRFLAG ARM write config command failed with a nonzero error code
 */
mykonosErr_t MYKONOS_writeArmConfig(mykonosDevice_t *device, uint8_t objectId, uint16_t offset, uint8_t *data, uint8_t byteCount)
{
    mykonosErr_t retVal = MYKONOS_ERR_OK;
    uint8_t extendedData[4] = {0}; /* ARM Object id, byte offset LSB, offset MSB = 0, copy 2 bytes */
    uint32_t timeoutMs = 1000;
    uint8_t cmdStatusByte = 0;

    retVal = MYKONOS_writeArmMem(device, MYKONOS_ADDR_ARM_START_DATA_ADDR, &data[0], byteCount);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    extendedData[0] = objectId;
    extendedData[1] = (offset & 0xFF);
    extendedData[2] = ((offset >> 8) & 0xFF);
    extendedData[3] = byteCount;
    retVal = MYKONOS_sendArmCommand(device, MYKONOS_ARM_WRITECFG_OPCODE, &extendedData[0], sizeof(extendedData));
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    retVal = MYKONOS_waitArmCmdStatus(device, MYKONOS_ARM_WRITECFG_OPCODE, timeoutMs, &cmdStatusByte);
    if (retVal != MYKONOS_ERR_OK)
    {
        if (cmdStatusByte > 0)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_WRITEARMCFG_ARMERRFLAG,
                    getMykonosErrorMessage(MYKONOS_ERR_WRITEARMCFG_ARMERRFLAG));
            return MYKONOS_ERR_WRITEARMCFG_ARMERRFLAG;
        }

        return retVal;
    }

    if (cmdStatusByte > 0)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_WRITEARMCFG_ARMERRFLAG,
                getMykonosErrorMessage(MYKONOS_ERR_WRITEARMCFG_ARMERRFLAG));
        return MYKONOS_ERR_WRITEARMCFG_ARMERRFLAG;
    }

    return MYKONOS_ERR_OK;
}

/**
 * \brief Low level helper function used by Mykonos API to read the ARM memory config structures
 *
 * Normally this function should not be required to be used directly by the BBIC.  This is a helper
 * function used by other Mykonos API commands to read settings from the ARM memory.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 * - device->spiSettings
 *
 * \param device is structure pointer to the Mykonos data structure containing settings
 * \param objectId ARM id of a particular structure or setting in ARM memory
 * \param offset Byte offset from the start of the objectId's memory location in ARM memory
 * \param data A byte array containing data to write to the ARM memory buffer.
 * \param byteCount Number of bytes in the data array (Valid size = 1-255 bytes)
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_READARMCFG_ARMERRFLAG ARM read config command failed with a nonzero error code
 */
mykonosErr_t MYKONOS_readArmConfig(mykonosDevice_t *device, uint8_t objectId, uint16_t offset, uint8_t *data, uint8_t byteCount)
{
    mykonosErr_t retVal = MYKONOS_ERR_OK;
    uint8_t extendedData[4] = {0}; /* ARM Object id, byte offset LSB, offset MSB = 0, copy 2 bytes */
    uint32_t timeoutMs = 1000;
    uint8_t cmdStatusByte = 0;
    const uint8_t AUTO_INCREMENT = 1;
    extendedData[0] = objectId;
    extendedData[1] = (offset & 0xFF);
    extendedData[2] = ((offset >> 8) & 0xFF);
    extendedData[3] = byteCount;

    retVal = MYKONOS_sendArmCommand(device, MYKONOS_ARM_READCFG_OPCODE, &extendedData[0], sizeof(extendedData));
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    retVal = MYKONOS_waitArmCmdStatus(device, MYKONOS_ARM_READCFG_OPCODE, timeoutMs, &cmdStatusByte);
    if (retVal != MYKONOS_ERR_OK)
    {
        if (cmdStatusByte > 0)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_READARMCFG_ARMERRFLAG,
                    getMykonosErrorMessage(MYKONOS_ERR_READARMCFG_ARMERRFLAG));
            return MYKONOS_ERR_READARMCFG_ARMERRFLAG;
        }

        return retVal;
    }

    if (cmdStatusByte > 0)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_READARMCFG_ARMERRFLAG,
                getMykonosErrorMessage(MYKONOS_ERR_READARMCFG_ARMERRFLAG));
        return MYKONOS_ERR_READARMCFG_ARMERRFLAG;
    }

    retVal = MYKONOS_readArmMem(device, MYKONOS_ADDR_ARM_START_DATA_ADDR, &data[0], byteCount, AUTO_INCREMENT);
    if (retVal != MYKONOS_ERR_OK)
    {
        return retVal;
    }

    return MYKONOS_ERR_OK;
}

/**
 * \brief Sends a command to the Mykonos ARM processor
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 * - device->spiSettings
 *
 * \param device is structure pointer to the Mykonos data structure containing settings
 * \param opCode Value (0-30, even only) indicating the desired function to run in the ARM.
 * \param extendedData A byte array containing extended data to write to the ARM command interface.
 * \param extendedDataNumBytes Number of bytes in the extendedData array (Valid size = 0-4 bytes)
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_ARMCMD_NULL_PARM Function parameter extendedData is NULL and extendedDataNumBytes is positive
 * \retval MYKONOS_ERR_ARMCMD_INV_OPCODE_PARM ARM opcode is out of range (valid 0-30, even only)
 * \retval MYKONOS_ERR_ARMCMD_INV_NUMBYTES_PARM Number of extended bytes parameter is out of range (valid 0-4)
 * \retval MYKONOS_ERR_TIMEDOUT_ARMMAILBOXBUSY ARM control interface is busy, command could not be executed by ARM
 */
mykonosErr_t MYKONOS_sendArmCommand(mykonosDevice_t *device, uint8_t opCode, uint8_t *extendedData, uint8_t extendedDataNumBytes)
{
    uint8_t armCommandBusy = 0;
    uint8_t i = 0;
    uint16_t extCmdByteStartAddr = MYKONOS_ADDR_ARM_EXT_CMD_BYTE_1;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_sendArmCommand()\n");
#endif

    if ((extendedData == NULL) && (extendedDataNumBytes > 0))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_ARMCMD_NULL_PARM, getMykonosErrorMessage(MYKONOS_ERR_ARMCMD_NULL_PARM));
        return MYKONOS_ERR_ARMCMD_NULL_PARM;
    }

    /* check for even-numbered opCodes only including opcode 0, but not must be greater than opCode 30 */
    if ((opCode != 0) && ((opCode % 2) || (opCode > 30)))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_ARMCMD_INV_OPCODE_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_ARMCMD_INV_OPCODE_PARM));
        return MYKONOS_ERR_ARMCMD_INV_OPCODE_PARM;
    }

    /* the number of valid extended data bytes is from 0-4 */
    if (extendedDataNumBytes > 4)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_ARMCMD_INV_NUMBYTES_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_ARMCMD_INV_NUMBYTES_PARM));
        return MYKONOS_ERR_ARMCMD_INV_NUMBYTES_PARM;
    }

    /* setting a 2 sec timeout for mailbox busy bit to be clear (can't send an arm mailbox command until mailbox is ready) */
    CMB_setTimeout_ms(2000);

    do
    {
        CMB_SPIReadField(device->spiSettings, MYKONOS_ADDR_ARM_CMD, &armCommandBusy, 0x80, 7);

        if (CMB_hasTimeoutExpired())
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_TIMEDOUT_ARMMAILBOXBUSY,
                    getMykonosErrorMessage(MYKONOS_ERR_TIMEDOUT_ARMMAILBOXBUSY));
            return MYKONOS_ERR_TIMEDOUT_ARMMAILBOXBUSY;
        }
    } while (armCommandBusy);

    if (extendedDataNumBytes)
    {
        for (i = 0; i < extendedDataNumBytes; i++)
        {
            CMB_SPIWriteByte(device->spiSettings, extCmdByteStartAddr + i, extendedData[i]);
        }
    }

    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_ARM_CMD, opCode);

    return MYKONOS_ERR_OK;
}

/**
 * \brief Reads the Mykonos ARM 64-bit command status register
 *
 * A 64-bit status register consisting of a pending bit and three-bit error type is read one byte at
 * a time for opcodes 0-30. The function parses the pending bits and error bits into
 * two (2) separate 16-bit words containing pending bits, and error bits if the error type > 0
 * The words are weighted according to each even-numbered opcode from 0-30,
 * where, 0x0001 = opcode '0', 0x0002 = opcode '2', 0x0004 = opcode '4', 0x0008 = opcode '6' and so on.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is a pointer to the device settings structure
 * \param errorWord 16-bit error word comprised of weighted bits according to each opcode number
 * The weighted bit = '1' if error type > 0, '0' if OK
 * \param statusWord 16-bit pending bits word comprised of weighted bits according to each opcode number
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_READARMCMDSTATUS_NULL_PARM Function parameters errorWord or statusWord have a NULL pointer
 */
mykonosErr_t MYKONOS_readArmCmdStatus(mykonosDevice_t *device, uint16_t *errorWord, uint16_t *statusWord)
{
    uint8_t i = 0;
    uint8_t bytes[8] = {0};

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_readArmCmdStatus()\n");
#endif

    if (errorWord == NULL || statusWord == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_READARMCMDSTATUS_NULL_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_READARMCMDSTATUS_NULL_PARM));
        return MYKONOS_ERR_READARMCMDSTATUS_NULL_PARM;
    }

    /* making sure the errorWord and statusWord are clear */
    *errorWord = 0;
    *statusWord = 0;

    /* read in the entire 64-bit status register into a byte array for parsing */
    for (i = 0; i < 8; i++)
    {
        CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_ARM_CMD_STATUS_0 + i, &bytes[i]);
    }

    /* parse the byte array for pending bits and error types and generate statusWord and errorWord bits */
    for (i = 0; i < 8; i++)
    {
        *statusWord |= (uint16_t)(((bytes[i] & 0x10) >> 3) | (bytes[i] & 0x01)) << (i * 2);

        if (bytes[i] & 0x0E)
        {
            *errorWord |= (0x0001 << (i * 2));
        }
        else if (bytes[i] & 0xE0)
        {
            *errorWord |= (0x0002 << (i * 2));
        }
    }

    return MYKONOS_ERR_OK;
}

/**
 * \brief Isolated byte read of the Mykonos ARM 64-bit command status register based on the opcode
 *
 * A single byte read is performed on the 64-bit command status register according to
 * the opcode of interest. The pending bit and the error type are extracted from the status
 * register and returned as a single byte in the lower nibble.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is a pointer to the device settings structure
 * \param opCode Valid values are even increments from 0-30. The ARM opCode is used to determine which status register byte to read
 * \param cmdStatByte returns cmdStatByte[3:1] = error type, cmdStatByte[0] = pending flag for selected opCode
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_READARMCMDSTATUSBYTE_NULL_PARM Function parameter cmdStatByte has NULL pointer
 * \retval MYKONOS_ERR_READARMCMDSTATUS_INV_OPCODE_PARM ARM opcode is out of range (valid 0-30, even only)
 */
mykonosErr_t MYKONOS_readArmCmdStatusByte(mykonosDevice_t *device, uint8_t opCode, uint8_t *cmdStatByte)
{
    uint8_t cmdByteIndex = 0;
    uint8_t cmdByte = 0;

#if 0
#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_readArmCmdStatByte()\n");
#endif
#endif

    if (cmdStatByte == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_READARMCMDSTATUSBYTE_NULL_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_READARMCMDSTATUSBYTE_NULL_PARM));
        return MYKONOS_ERR_READARMCMDSTATUSBYTE_NULL_PARM;
    }

    /* check for even-numbered opCodes only including opcode 0, but not must be greater than opCode 30 */
    if ((opCode != 0) && ((opCode % 2) || (opCode > 30)))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_READARMCMDSTATUS_INV_OPCODE_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_READARMCMDSTATUS_INV_OPCODE_PARM));
        return MYKONOS_ERR_READARMCMDSTATUS_INV_OPCODE_PARM;
    }
    else
    {
        cmdByteIndex = (opCode / 4);

        CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_ARM_CMD_STATUS_0 + cmdByteIndex, &cmdByte);

        if ((opCode / 2) % 2)
        {
            *cmdStatByte = ((cmdByte >> 4) & 0x0F);
        }
        else
        {
            *cmdStatByte = (cmdByte & 0x0F);
        }

        return MYKONOS_ERR_OK;
    }
}

/**
 * \brief Mykonos ARM command status wait function polls command status until opcode completes
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is a pointer to the device settings structure
 * \param opCode Valid values are even increments from 0-30. The ARM opCode is used to determine which pending bit to wait for
 * \param timeoutMs conveys the time-out period in milliseconds
 * \param cmdStatByte returns cmdStatByte[3:1] = error type, cmdStatByte[0] = pending flag for selected opCode
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_ARMCMDSTATUS_NULL_PARM Function parameter cmdStatByte has NULL pointer
 * \retval MYKONOS_ERR_ARMCMDSTATUS_INV_OPCODE_PARM Invalid ARM opcode (valid 0-30, even numbers only)
 * \retval MYKONOS_ERR_ARMCMDSTATUS_ARMERROR ARM Mailbox error flag for requested opcode returned a nonzero error flag
 * \retval MYKONOS_ERR_WAITARMCMDSTATUS_TIMEOUT Timeout occurred before ARM command completed
 */
mykonosErr_t MYKONOS_waitArmCmdStatus(mykonosDevice_t *device, uint8_t opCode, uint32_t timeoutMs, uint8_t *cmdStatByte)
{

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_waitArmCmdStatus()\n");
#endif

    if (cmdStatByte == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_ARMCMDSTATUS_NULL_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_ARMCMDSTATUS_NULL_PARM));
        return MYKONOS_ERR_ARMCMDSTATUS_NULL_PARM;
    }

    /* check for even-numbered opCodes only including opcode 0, but not must be greater than opCode 30 */
    if ((opCode != 0) && ((opCode % 2) || (opCode > 30)))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_ARMCMDSTATUS_INV_OPCODE_PARM,
                getMykonosErrorMessage(MYKONOS_ERR_ARMCMDSTATUS_INV_OPCODE_PARM));
        return MYKONOS_ERR_ARMCMDSTATUS_INV_OPCODE_PARM;
    }

    /* start wait */
    CMB_setTimeout_ms(timeoutMs);

    do
    {
        MYKONOS_readArmCmdStatusByte(device, opCode, cmdStatByte);

        /* If error flag is non zero in [3:1], - return error */
        if ((*cmdStatByte & 0x0E) > 0)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_ARMCMDSTATUS_ARMERROR,
                    getMykonosErrorMessage(MYKONOS_ERR_ARMCMDSTATUS_ARMERROR));
            return MYKONOS_ERR_ARMCMDSTATUS_ARMERROR;
        }

        if (CMB_hasTimeoutExpired())
        {
            return MYKONOS_ERR_WAITARMCMDSTATUS_TIMEOUT;
        }
    } while (*cmdStatByte & 0x01);

    return MYKONOS_ERR_OK;
}

/**
 * \brief Mykonos ARM configuration write
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is a pointer to the device settings structure
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 */
mykonosErr_t MYKONOS_writeArmProfile(mykonosDevice_t *device)
{
    const uint8_t length = 100;

    int32_t i = 0;
    uint8_t vcoDiv = 0;
    uint8_t hsDiv = 0;
    uint16_t channelsEnabled = 0;
    uint8_t cfgData[100] = {0};

    mykonosErr_t retVal = MYKONOS_ERR_OK;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_writeArmConfig()\n");
#endif

    /* reading in required mykonosDevice_t structure data into array - not pretty but it works */
    for (i = 0; i < 4; i++)
    {
        cfgData[i] = (uint8_t)(((device->clocks->deviceClock_kHz * 1000) >> (i * 8)) & 0x000000FF);
    }

    for (i = 4; i < 8; i++)
    {
        cfgData[i] = (uint8_t)(((device->clocks->clkPllVcoFreq_kHz) >> ((i - 4) * 8)) & 0x000000FF);
    }

    vcoDiv = (uint8_t)(device->clocks->clkPllVcoDiv);
    hsDiv = device->clocks->clkPllHsDiv;

    cfgData[8] = vcoDiv; /* uint8_t vco_div */
    cfgData[9] = hsDiv; /* uint8_t hs_div */

    channelsEnabled |= device->tx->txChannels & 0x03;
    channelsEnabled |= ((device->rx->rxChannels & 0x03) << 2);
    if (device->profilesValid & ORX_PROFILE_VALID)
    {
        channelsEnabled |= ((device->obsRx->obsRxChannelsEnable & 3) << 4);
    }

    if (device->profilesValid & SNIFF_PROFILE_VALID)
    {
        channelsEnabled |= (((device->obsRx->obsRxChannelsEnable >> 2) & 7) << 6);
    }

    for (i = 10; i < 12; i++)
    {
        cfgData[i] = (uint8_t)((channelsEnabled >> ((i - 10) * 8)) & 0xFF);
    }

    cfgData[12] = ((device->rx->rxPllUseExternalLo & 0x01) << 1) | (device->tx->txPllUseExternalLo & 0x01);
    cfgData[13] = 0x00; /* Not used...padding */
    cfgData[14] = 0x00; /* Not used...padding */
    cfgData[15] = 0x00; /* Not used...padding */

    if (device->profilesValid & TX_PROFILE_VALID)
    {
        /* start of Tx profile data */
        cfgData[16] = device->tx->txProfile->dacDiv;
        cfgData[17] = device->tx->txProfile->txFirInterpolation;
        cfgData[18] = device->tx->txProfile->thb1Interpolation;
        cfgData[19] = device->tx->txProfile->thb2Interpolation;

        for (i = 20; i < 24; i++)
        {
            cfgData[i] = (uint8_t)(((device->tx->txProfile->iqRate_kHz * 1000) >> ((i - 20) * 8)) & 0x000000FF);
        }

        for (i = 24; i < 28; i++)
        {
            cfgData[i] = (uint8_t)(((device->tx->txProfile->primarySigBandwidth_Hz) >> ((i - 24) * 8)) & 0x000000FF);
        }

        for (i = 28; i < 32; i++)
        {
            cfgData[i] = (uint8_t)(((device->tx->txProfile->rfBandwidth_Hz) >> ((i - 28) * 8)) & 0x000000FF);
        }

        for (i = 32; i < 36; i++)
        {
            cfgData[i] = (uint8_t)(((device->tx->txProfile->txDac3dBCorner_kHz) >> ((i - 32) * 8)) & 0x000000FF);
        }

        for (i = 36; i < 40; i++)
        {
            cfgData[i] = (uint8_t)(((device->tx->txProfile->txBbf3dBCorner_kHz * 1000) >> ((i - 36) * 8)) & 0x000000FF);
        }
    }
    else
    {
        cfgData[16] = 0;
        cfgData[17] = 0;
        cfgData[18] = 0;
        cfgData[19] = 0;
        cfgData[20] = 0;
        cfgData[21] = 0;
        cfgData[22] = 0;
        cfgData[23] = 0;
        cfgData[24] = 0;
        cfgData[25] = 0;
        cfgData[26] = 0;
        cfgData[27] = 0;
        cfgData[28] = 0;
        cfgData[29] = 0;
        cfgData[30] = 0;
        cfgData[31] = 0;
        cfgData[32] = 0;
        cfgData[33] = 0;
        cfgData[34] = 0;
        cfgData[35] = 0;
        cfgData[36] = 0;
        cfgData[37] = 0;
        cfgData[38] = 0;
        cfgData[39] = 0;
    }

    if (device->profilesValid & RX_PROFILE_VALID)
    {
        /* start of Rx profile data */
        cfgData[40] = device->rx->rxProfile->adcDiv;
        cfgData[41] = device->rx->rxProfile->rxFirDecimation;
        cfgData[42] = device->rx->rxProfile->rxDec5Decimation;
        cfgData[43] = device->rx->rxProfile->rhb1Decimation;

        for (i = 44; i < 48; i++)
        {
            cfgData[i] = (uint8_t)(((device->rx->rxProfile->iqRate_kHz * 1000) >> ((i - 44) * 8)) & 0x000000FF);
        }

        for (i = 48; i < 52; i++)
        {
            /* sig bw placeholder */
            cfgData[i] = (uint8_t)(((device->rx->rxProfile->rfBandwidth_Hz) >> ((i - 48) * 8)) & 0x000000FF);
        }

        for (i = 52; i < 56; i++)
        {
            cfgData[i] = (uint8_t)(((device->rx->rxProfile->rfBandwidth_Hz) >> ((i - 52) * 8)) & 0x000000FF);
        }

        for (i = 56; i < 60; i++)
        {
            cfgData[i] = (uint8_t)(((device->rx->rxProfile->rxBbf3dBCorner_kHz * 1000) >> ((i - 56) * 8)) & 0x000000FF);
        }
    }
    else
    {
        cfgData[40] = 0;
        cfgData[41] = 0;
        cfgData[42] = 0;
        cfgData[43] = 0;
        cfgData[44] = 0;
        cfgData[45] = 0;
        cfgData[46] = 0;
        cfgData[47] = 0;
        cfgData[48] = 0;
        cfgData[49] = 0;
        cfgData[50] = 0;
        cfgData[51] = 0;
        cfgData[52] = 0;
        cfgData[53] = 0;
        cfgData[54] = 0;
        cfgData[55] = 0;
        cfgData[56] = 0;
        cfgData[57] = 0;
        cfgData[58] = 0;
        cfgData[59] = 0;
    }

    if (device->profilesValid & ORX_PROFILE_VALID)
    {
        /* start of ObsRx profile data */
        cfgData[60] = device->obsRx->orxProfile->adcDiv;
        cfgData[61] = device->obsRx->orxProfile->rxFirDecimation;
        cfgData[62] = device->obsRx->orxProfile->rxDec5Decimation;
        cfgData[63] = device->obsRx->orxProfile->rhb1Decimation;

        for (i = 64; i < 68; i++)
        {
            cfgData[i] = (uint8_t)(((device->obsRx->orxProfile->iqRate_kHz * 1000) >> ((i - 64) * 8)) & 0x000000FF);
        }

        for (i = 68; i < 72; i++)
        {
            /* sig bw placeholder */
            cfgData[i] = (uint8_t)(((device->obsRx->orxProfile->rfBandwidth_Hz) >> ((i - 68) * 8)) & 0x000000FF);
        }

        for (i = 72; i < 76; i++)
        {
            cfgData[i] = (uint8_t)(((device->obsRx->orxProfile->rfBandwidth_Hz) >> ((i - 72) * 8)) & 0x000000FF);
        }

        for (i = 76; i < 80; i++)
        {
            cfgData[i] = (uint8_t)(((device->obsRx->orxProfile->rxBbf3dBCorner_kHz * 1000) >> ((i - 76) * 8)) & 0x000000FF);
        }
    }
    else
    {
        cfgData[60] = 0;
        cfgData[61] = 0;
        cfgData[62] = 0;
        cfgData[63] = 0;
        cfgData[64] = 0;
        cfgData[65] = 0;
        cfgData[66] = 0;
        cfgData[67] = 0;
        cfgData[68] = 0;
        cfgData[69] = 0;
        cfgData[70] = 0;
        cfgData[71] = 0;
        cfgData[72] = 0;
        cfgData[73] = 0;
        cfgData[74] = 0;
        cfgData[75] = 0;
        cfgData[76] = 0;
        cfgData[77] = 0;
        cfgData[78] = 0;
        cfgData[79] = 0;
    }

    if (device->profilesValid & SNIFF_PROFILE_VALID)
    {
        /* start of SnRx profile data */
        cfgData[80] = device->obsRx->snifferProfile->adcDiv;
        cfgData[81] = device->obsRx->snifferProfile->rxFirDecimation;
        cfgData[82] = device->obsRx->snifferProfile->rxDec5Decimation;
        cfgData[83] = device->obsRx->snifferProfile->rhb1Decimation;

        for (i = 84; i < 88; i++)
        {
            cfgData[i] = (uint8_t)(((device->obsRx->snifferProfile->iqRate_kHz * 1000) >> ((i - 84) * 8)) & 0x000000FF);
        }

        for (i = 88; i < 92; i++)
        {
            /* sig bw placeholder */
            cfgData[i] = (uint8_t)(((device->obsRx->snifferProfile->rfBandwidth_Hz) >> ((i - 88) * 8)) & 0x000000FF);
        }

        for (i = 92; i < 96; i++)
        {
            cfgData[i] = (uint8_t)(((device->obsRx->snifferProfile->rfBandwidth_Hz) >> ((i - 92) * 8)) & 0x000000FF);
        }

        for (i = 96; i < 100; i++)
        {
            cfgData[i] = (uint8_t)(((device->obsRx->snifferProfile->rxBbf3dBCorner_kHz * 1000) >> ((i - 96) * 8)) & 0x000000FF);
        }
    }
    else
    {
        cfgData[80] = 0;
        cfgData[81] = 0;
        cfgData[82] = 0;
        cfgData[83] = 0;
        cfgData[84] = 0;
        cfgData[85] = 0;
        cfgData[86] = 0;
        cfgData[87] = 0;
        cfgData[88] = 0;
        cfgData[89] = 0;
        cfgData[90] = 0;
        cfgData[91] = 0;
        cfgData[92] = 0;
        cfgData[93] = 0;
        cfgData[94] = 0;
        cfgData[95] = 0;
        cfgData[96] = 0;
        cfgData[97] = 0;
        cfgData[98] = 0;
        cfgData[99] = 0;
    }

    /* writing to the ARM memory with the array data */
    retVal = MYKONOS_writeArmMem(device, MYKONOS_ADDR_ARM_START_DATA_ADDR, &cfgData[0], length);
    if (retVal)
    {
        return retVal;
    }

    return MYKONOS_ERR_OK;
}

/**
 * \brief Function called automatically that loads the ADC profiles into the ARM
 *
 * User should not need to call this function normally.  It is called automatically
 * during MYKONOS_initArm() to load the ADC profiles (tunes the ADC performance).
 *
 * Rx ADC profile is only loaded if the Rx Profile is valid, ORx ADC profile is
 * only loaded if the ORx Profile is valid. Sniffer ADC profile is only loaded
 * if the Sniffer profile is valid.  The Loopback ADC profile is always
 * loaded.  If Tx profile is valid, the loopback ADC profile is chosen based
 * on the Tx Primary Signal bandwidth.  If no valid Tx Profile, the Rx profile
 * is used to set the Loopback ADC profile.  Else, ORx, then sniffer profiles
 * are used to set the loopback ADC profile.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device Pointer to the device settings structure
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_LOAD_ADCPROFILE_INV_VCODIV Mykonos CLKPLL has invalid VCO divider in the clocks structure
 * \retval MYKONOS_ERR_LOAD_ADCPROFILE_MISSING_ORX_PROFILE When Tx Profile used, a matching ORx Profile must be provided with a valid ADC divider.
 *                                                         The ORx digital filters and clock dividers are used with the Loopback Rx path for Tx calibrations.
 * \retval MYKONOS_ERR_LOAD_ADCPROFILE_CUSTOM_RXREQUIRED ADC Profile Lookup table does not have a match
 *                                                       for current Rx Profile settings.  Custom ADC profile required
 * \retval MYKONOS_ERR_LOAD_ADCPROFILE_CUSTOM_ORXREQUIRED ADC Profile Lookup table does not have a match
 *                                                        for current ORx Profile settings.  Custom ADC profile required
 * \retval MYKONOS_ERR_LOAD_ADCPROFILE_CUSTOM_SNRXREQUIRED ADC Profile Lookup table does not have a match
 *                                                         for current Sniffer Rx Profile settings.  Custom ADC profile required
 * \retval MYKONOS_ERR_LOAD_ADCPROFILE_CUSTOM_LBREQUIRED ADC Profile Lookup table does not have a match
 *                                                       for Loopback passband BW and ADC Clock frequency settings.  Custom ADC profile required
 * \retval MYKONOS_ERR_LOAD_ADCPROFILE_RXADCDIV_ZERO Rx Profile has invalid ADC divider = 0, causing a divide by zero error
 * \retval MYKONOS_ERR_LOAD_ADCPROFILE_ORXADCDIV_ZERO ORx profile has invalid ADC divider = 0, causing a divide by zero error
 * \retval MYKONOS_ERR_LOAD_ADCPROFILE_SNRX_ADCDIV_ZERO Sniffer profile has invalid ADC divider = 0, causing a divide by zero error
 * \retval MYKONOS_ERR_LOAD_RXADCPROFILE_ARMMEM_FAILED Error returned while trying to write Rx ADC profile to ARM memory
 * \retval MYKONOS_ERR_LOAD_ORXADCPROFILE_ARMMEM_FAILED Error returned while trying to write ORx ADC profile to ARM memory
 * \retval MYKONOS_ERR_LOAD_SNRXADCPROFILE_ARMMEM_FAILED Error returned while trying to write Sniffer ADC profile into ARM memory
 * \retval MYKONOS_ERR_LOAD_LBADCPROFILE_ARMMEM_FAILED Error returned while trying to write Loopback ADC profile into ARM memory
 */
mykonosErr_t MYKONOS_loadAdcProfiles(mykonosDevice_t *device)
{
    uint8_t adcProfile[32] = {0};
    uint8_t i = 0;
    mykonosErr_t retVal = MYKONOS_ERR_OK;
    uint32_t hsDigClk_MHz = 0;
    uint32_t adcClk_MHz = 0;
    uint32_t vcoDiv = 0;
    uint32_t vcoDivTimes10 = 10;
    uint8_t profileIndex = 0;
    const uint8_t ARM_CONFIG_OFFSET = 100; /* number of bytes written in MYKONOS_writeArmProfile() to ARM memory */

    const uint8_t NUM_ADCPROFILE_COEFS = 16;
    const uint8_t NUM_ADC_PROFILES = 20;
    static const uint16_t adcProfileLut[20][18] = {
        /* Max RFBW, ADCCLK_MHz, adcProfile[16] */
        { 20,  491, 1494, 564, 201,  98, 1280, 134,  945,  40,  529,  10,  326, 39, 30, 16,  9, 201},
        { 60,  983,  712, 462, 201,  98, 1280, 291, 1541, 149, 1054,  46,  645, 34, 48, 32, 18, 193},
        { 75,  983,  680, 477, 201,  98, 1280, 438, 1577, 242, 1046,  73,  636, 30, 48, 31, 18, 192},
        {100,  983,  655, 446, 201,  98, 1280, 336, 1631, 334, 1152, 207,  733, 33, 48, 32, 21, 212},
        { 75, 1228,  569, 369, 201,  98, 1280, 291, 1541, 149, 1320,  58,  807, 34, 48, 40, 23, 189},
        {100, 1228,  534, 386, 201,  98, 1280, 491, 1591, 279, 1306, 104,  792, 28, 48, 39, 23, 187},
        {160, 1228,  491, 375, 201,  98, 1280, 514, 1728, 570, 1455, 443,  882, 27, 48, 39, 25, 205},
        {200, 1228,  450, 349, 201,  98, 1280, 730, 1626, 818, 1476, 732,  834, 20, 41, 36, 24, 200},
        { 80, 1250,  555, 365, 201,  98, 1280, 317, 1547, 165, 1341,  65,  819, 33, 48, 40, 24, 188},
        {102, 1250,  524, 379, 201,  98, 1280, 494, 1592, 281, 1328, 107,  805, 28, 48, 40, 23, 187},
        { 80, 1333,  526, 339, 201,  98, 1280, 281, 1539, 143, 1433,  60,  877, 35, 48, 43, 25, 188},
        {217, 1333,  414, 321, 201,  98, 1280, 730, 1626, 818, 1603, 794,  905, 20, 41, 40, 26, 199},
        { 75, 1474,  486, 302, 199,  98, 1280, 206, 1523, 101, 1578,  47,  977, 37, 48, 48, 28, 186},
        {100, 1474,  465, 311, 201,  98, 1280, 353, 1556, 187, 1581,  86,  964, 32, 48, 47, 28, 185},
        {150, 1474,  436, 296, 190,  98, 1280, 336, 1631, 334, 1638, 293, 1102, 33, 48, 46, 31, 205},
        { 40, 1536,  479, 285, 190,  98, 1280, 112, 1505,  53, 1574,  25, 1026, 40, 48, 48, 29, 186},
        {100, 1536,  450, 297, 193,  98, 1280, 328, 1550, 171, 1586,  79, 1006, 33, 48, 48, 29, 184},
        {150, 1536,  421, 283, 182,  98, 1280, 313, 1620, 306, 1638, 269, 1153, 34, 48, 46, 33, 204},
        {200, 1536,  392, 299, 180,  98, 1280, 514, 1728, 570, 1638, 498, 1104, 27, 48, 44, 32, 200},
        {240, 1536,  366, 301, 178,  98, 1280, 686, 1755, 818, 1638, 742, 1056, 22, 45, 41, 31, 196}};

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_loadAdcProfiles\n");
#endif

    vcoDiv = device->clocks->clkPllVcoDiv;
    switch (vcoDiv)
    {
        case VCODIV_1:
            vcoDivTimes10 = 10;
            break;
        case VCODIV_1p5:
            vcoDivTimes10 = 15;
            break;
        case VCODIV_2:
            vcoDivTimes10 = 20;
            break;
        case VCODIV_3:
            vcoDivTimes10 = 30;
            break;
        default:
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_LOAD_ADCPROFILE_INV_VCODIV,
                    getMykonosErrorMessage(MYKONOS_ERR_LOAD_ADCPROFILE_INV_VCODIV));
            return MYKONOS_ERR_LOAD_ADCPROFILE_INV_VCODIV;
        }
    }

    hsDigClk_MHz = device->clocks->clkPllVcoFreq_kHz / vcoDivTimes10 / 100 / device->clocks->clkPllHsDiv;

    /* If Tx Profile is valid, set loopback ADC profile based on Tx primary signal BW */
    if ((device->profilesValid & TX_PROFILE_VALID) > 0)
    {
        /* If custom profile, load it */
        if (device->obsRx->customLoopbackAdcProfile != NULL)
        {
            for (i = 0; i < NUM_ADCPROFILE_COEFS; i++)
            {
                adcProfile[i * 2] = device->obsRx->customLoopbackAdcProfile[i] & 0xFF;
                adcProfile[i * 2 + 1] = (device->obsRx->customLoopbackAdcProfile[i] >> 8) & 0xFF;
            }
        }
        else
        {
            /* Tx requires ORx profile is configured */
            if (((device->profilesValid & TX_PROFILE_VALID) > 0) && ((device->profilesValid & ORX_PROFILE_VALID) == 0))
            {
                CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_LOAD_ADCPROFILE_MISSING_ORX_PROFILE,
                        getMykonosErrorMessage(MYKONOS_ERR_LOAD_ADCPROFILE_MISSING_ORX_PROFILE));
                return MYKONOS_ERR_LOAD_ADCPROFILE_MISSING_ORX_PROFILE;
            }

            if (device->obsRx->orxProfile->adcDiv == 0)
            {
                CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_LOAD_ADCPROFILE_ORXADCDIV_ZERO,
                        getMykonosErrorMessage(MYKONOS_ERR_LOAD_ADCPROFILE_ORXADCDIV_ZERO));
                return MYKONOS_ERR_LOAD_ADCPROFILE_ORXADCDIV_ZERO;
            }

            adcClk_MHz = hsDigClk_MHz / device->obsRx->orxProfile->adcDiv;

            /* find correct ADC profile in the LUT */
            profileIndex = NUM_ADC_PROFILES;
            for (i = 0; i < NUM_ADC_PROFILES; i++)
            {
                /* Find a row in the LUT that matches the ADC clock frequency */
                if ((adcProfileLut[i][1] == adcClk_MHz) && (adcProfileLut[i][0] >= (device->tx->txProfile->primarySigBandwidth_Hz / 1000000)))
                {
                    profileIndex = i;
                    break;
                }
            }

            /* Verify that a profile was found in the LUT, if not return error */
            /* In this case a custom profile would need to be passed in */
            if (profileIndex >= NUM_ADC_PROFILES)
            {
                CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_LOAD_ADCPROFILE_CUSTOM_LBREQUIRED,
                        getMykonosErrorMessage(MYKONOS_ERR_LOAD_ADCPROFILE_CUSTOM_LBREQUIRED));
                return MYKONOS_ERR_LOAD_ADCPROFILE_CUSTOM_LBREQUIRED;
            }

            for (i = 0; i < NUM_ADCPROFILE_COEFS; i++)
            {
                adcProfile[i * 2] = adcProfileLut[profileIndex][i + 2] & 0xFF;
                adcProfile[i * 2 + 1] = (adcProfileLut[profileIndex][i + 2] >> 8) & 0xFF;
            }
        }

        /* writing to the ARM memory: Set Loopback ADC Profile based on Tx Primary signal Bandwidth */
        retVal = MYKONOS_writeArmMem(device, MYKONOS_ADDR_ARM_START_DATA_ADDR + ARM_CONFIG_OFFSET + 96, &adcProfile[0], sizeof(adcProfile));
        if (retVal)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_LOAD_LBADCPROFILE_ARMMEM_FAILED,
                    getMykonosErrorMessage(MYKONOS_ERR_LOAD_LBADCPROFILE_ARMMEM_FAILED));
            return MYKONOS_ERR_LOAD_LBADCPROFILE_ARMMEM_FAILED;
        }
    }

    if ((device->profilesValid & RX_PROFILE_VALID) > 0)
    {
        /* If custom profile, load it */
        if (device->rx->rxProfile->customAdcProfile != NULL)
        {
            for (i = 0; i < NUM_ADCPROFILE_COEFS; i++)
            {
                adcProfile[i * 2] = device->rx->rxProfile->customAdcProfile[i] & 0xFF;
                adcProfile[i * 2 + 1] = (device->rx->rxProfile->customAdcProfile[i] >> 8) & 0xFF;
            }
        }
        else
        {
            if (device->rx->rxProfile->adcDiv == 0)
            {
                CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_LOAD_ADCPROFILE_RXADCDIV_ZERO,
                        getMykonosErrorMessage(MYKONOS_ERR_LOAD_ADCPROFILE_RXADCDIV_ZERO));
                return MYKONOS_ERR_LOAD_ADCPROFILE_RXADCDIV_ZERO;
            }

            adcClk_MHz = hsDigClk_MHz / device->rx->rxProfile->adcDiv;

            /* find correct ADC profile in the LUT */
            profileIndex = NUM_ADC_PROFILES;
            for (i = 0; i < NUM_ADC_PROFILES; i++)
            {
                /* Find a row in the LUT that matches the ADC clock frequency */
                if ((adcProfileLut[i][1] == adcClk_MHz) && (adcProfileLut[i][0] >= (device->rx->rxProfile->rfBandwidth_Hz / 1000000)))
                {
                    profileIndex = i;
                    break;
                }
            }

            /* Verify that a profile was found in the LUT, if not return error */
            /* In this case a custom profile would need to be passed in */
            if (profileIndex >= NUM_ADC_PROFILES)
            {
                CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_LOAD_ADCPROFILE_CUSTOM_RXREQUIRED,
                        getMykonosErrorMessage(MYKONOS_ERR_LOAD_ADCPROFILE_CUSTOM_RXREQUIRED));
                return MYKONOS_ERR_LOAD_ADCPROFILE_CUSTOM_RXREQUIRED;
            }

            for (i = 0; i < NUM_ADCPROFILE_COEFS; i++)
            {
                adcProfile[i * 2] = adcProfileLut[profileIndex][i + 2] & 0xFF;
                adcProfile[i * 2 + 1] = (adcProfileLut[profileIndex][i + 2] >> 8) & 0xFF;
            }
        }

        /* writing to the ARM memory with ADC Profile for Rx path */
        retVal = MYKONOS_writeArmMem(device, MYKONOS_ADDR_ARM_START_DATA_ADDR + ARM_CONFIG_OFFSET, &adcProfile[0], sizeof(adcProfile));
        if (retVal)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_LOAD_RXADCPROFILE_ARMMEM_FAILED,
                    getMykonosErrorMessage(MYKONOS_ERR_LOAD_RXADCPROFILE_ARMMEM_FAILED));
            return MYKONOS_ERR_LOAD_RXADCPROFILE_ARMMEM_FAILED;
        }

        /* writing to the ARM memory: Set Loopback ADC Profile = Rx ADC Profile ONLY if Tx profile is not valid */
        if ((device->profilesValid & TX_PROFILE_VALID) == 0)
        {
            retVal = MYKONOS_writeArmMem(device, MYKONOS_ADDR_ARM_START_DATA_ADDR + ARM_CONFIG_OFFSET + 96, &adcProfile[0], sizeof(adcProfile));
            if (retVal)
            {
                CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_LOAD_LBADCPROFILE_ARMMEM_FAILED,
                        getMykonosErrorMessage(MYKONOS_ERR_LOAD_LBADCPROFILE_ARMMEM_FAILED));
                return MYKONOS_ERR_LOAD_LBADCPROFILE_ARMMEM_FAILED;
            }
        }
    }

    if ((device->profilesValid & ORX_PROFILE_VALID) > 0)
    {
        /* If custom profile, load it */
        if (device->obsRx->orxProfile->customAdcProfile != NULL)
        {
            for (i = 0; i < NUM_ADCPROFILE_COEFS; i++)
            {
                adcProfile[i * 2] = device->obsRx->orxProfile->customAdcProfile[i] & 0xFF;
                adcProfile[i * 2 + 1] = (device->obsRx->orxProfile->customAdcProfile[i] >> 8) & 0xFF;
            }
        }
        else
        {
            if (device->obsRx->orxProfile->adcDiv == 0)
            {
                CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_LOAD_ADCPROFILE_ORXADCDIV_ZERO,
                        getMykonosErrorMessage(MYKONOS_ERR_LOAD_ADCPROFILE_ORXADCDIV_ZERO));
                return MYKONOS_ERR_LOAD_ADCPROFILE_ORXADCDIV_ZERO;
            }

            adcClk_MHz = hsDigClk_MHz / device->obsRx->orxProfile->adcDiv;

            /* find correct ADC profile in the LUT */
            profileIndex = NUM_ADC_PROFILES;
            for (i = 0; i < NUM_ADC_PROFILES; i++)
            {
                /* Find a row in the LUT that matches the ADC clock frequency */
                if ((adcProfileLut[i][1] == adcClk_MHz) && (adcProfileLut[i][0] >= (device->obsRx->orxProfile->rfBandwidth_Hz / 1000000)))
                {
                    profileIndex = i;
                    break;
                }
            }

            /* Verify that a profile was found in the LUT, if not return error */
            /* In this case a custom profile would need to be passed in */
            if (profileIndex >= NUM_ADC_PROFILES)
            {
                CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_LOAD_ADCPROFILE_CUSTOM_ORXREQUIRED,
                        getMykonosErrorMessage(MYKONOS_ERR_LOAD_ADCPROFILE_CUSTOM_ORXREQUIRED));
                return MYKONOS_ERR_LOAD_ADCPROFILE_CUSTOM_ORXREQUIRED;
            }

            for (i = 0; i < NUM_ADCPROFILE_COEFS; i++)
            {
                adcProfile[i * 2] = adcProfileLut[profileIndex][i + 2] & 0xFF;
                adcProfile[i * 2 + 1] = (adcProfileLut[profileIndex][i + 2] >> 8) & 0xFF;
            }
        }

        /* writing to the ARM memory with ADC Profile for ORx path */
        retVal = MYKONOS_writeArmMem(device, MYKONOS_ADDR_ARM_START_DATA_ADDR + ARM_CONFIG_OFFSET + 32, &adcProfile[0], sizeof(adcProfile));
        if (retVal)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_LOAD_ORXADCPROFILE_ARMMEM_FAILED,
                    getMykonosErrorMessage(MYKONOS_ERR_LOAD_ORXADCPROFILE_ARMMEM_FAILED));
            return MYKONOS_ERR_LOAD_ORXADCPROFILE_ARMMEM_FAILED;
        }

        /* If Rx and Tx Profiles are not valid, set Loopback ADC profile to ORx ADC Profile */
        if (((device->profilesValid & TX_PROFILE_VALID) == 0) && ((device->profilesValid & RX_PROFILE_VALID) == 0))
        {
            retVal = MYKONOS_writeArmMem(device, MYKONOS_ADDR_ARM_START_DATA_ADDR + ARM_CONFIG_OFFSET + 96, &adcProfile[0], sizeof(adcProfile));
            if (retVal)
            {
                CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_LOAD_LBADCPROFILE_ARMMEM_FAILED,
                        getMykonosErrorMessage(MYKONOS_ERR_LOAD_LBADCPROFILE_ARMMEM_FAILED));
                return MYKONOS_ERR_LOAD_LBADCPROFILE_ARMMEM_FAILED;
            }
        }
    }

    if ((device->profilesValid & SNIFF_PROFILE_VALID) > 0)
    {
        /* If custom profile, load it */
        if (device->obsRx->snifferProfile->customAdcProfile != NULL)
        {
            for (i = 0; i < NUM_ADCPROFILE_COEFS; i++)
            {
                adcProfile[i * 2] = device->obsRx->orxProfile->customAdcProfile[i] & 0xFF;
                adcProfile[i * 2 + 1] = (device->obsRx->orxProfile->customAdcProfile[i] >> 8) & 0xFF;
            }
        }
        else
        {

            if (device->obsRx->snifferProfile->adcDiv == 0)
            {
                CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_LOAD_ADCPROFILE_SNRX_ADCDIV_ZERO,
                        getMykonosErrorMessage(MYKONOS_ERR_LOAD_ADCPROFILE_SNRX_ADCDIV_ZERO));
                return MYKONOS_ERR_LOAD_ADCPROFILE_SNRX_ADCDIV_ZERO;
            }

            adcClk_MHz = hsDigClk_MHz / device->obsRx->snifferProfile->adcDiv;

            /* find correct ADC profile in the LUT */
            profileIndex = NUM_ADC_PROFILES;
            for (i = 0; i < NUM_ADC_PROFILES; i++)
            {
                /* Find a row in the LUT that matches the ADC clock frequency */
                if ((adcProfileLut[i][1] == adcClk_MHz) && (adcProfileLut[i][0] >= (device->obsRx->snifferProfile->rfBandwidth_Hz / 1000000)))
                {
                    profileIndex = i;
                    break;
                }
            }

            /* Verify that a profile was found in the LUT, if not return error */
            /* In this case a custom profile would need to be passed in */
            if (profileIndex >= NUM_ADC_PROFILES)
            {
                CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_LOAD_ADCPROFILE_CUSTOM_SNRXREQUIRED,
                        getMykonosErrorMessage(MYKONOS_ERR_LOAD_ADCPROFILE_CUSTOM_SNRXREQUIRED));
                return MYKONOS_ERR_LOAD_ADCPROFILE_CUSTOM_SNRXREQUIRED;
            }

            for (i = 0; i < NUM_ADCPROFILE_COEFS; i++)
            {
                adcProfile[i * 2] = adcProfileLut[profileIndex][i + 2] & 0xFF;
                adcProfile[i * 2 + 1] = (adcProfileLut[profileIndex][i + 2] >> 8) & 0xFF;
            }
        }

        /* writing to the ARM memory with ADC Profile for sniffer Rx path */
        retVal = MYKONOS_writeArmMem(device, MYKONOS_ADDR_ARM_START_DATA_ADDR + ARM_CONFIG_OFFSET + 64, &adcProfile[0], sizeof(adcProfile));
        if (retVal)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_LOAD_SNRXADCPROFILE_ARMMEM_FAILED,
                    getMykonosErrorMessage(MYKONOS_ERR_LOAD_SNRXADCPROFILE_ARMMEM_FAILED));
            return MYKONOS_ERR_LOAD_SNRXADCPROFILE_ARMMEM_FAILED;
        }

        /* If Tx, Rx, and ORx Profiles are not valid, set Loopback ADC profile to Sniffer Rx ADC Profile */
        if (((device->profilesValid & TX_PROFILE_VALID) == 0) && ((device->profilesValid & RX_PROFILE_VALID) == 0) && ((device->profilesValid & ORX_PROFILE_VALID) == 0))
        {
            retVal = MYKONOS_writeArmMem(device, MYKONOS_ADDR_ARM_START_DATA_ADDR + ARM_CONFIG_OFFSET + 96, &adcProfile[0], sizeof(adcProfile));
            if (retVal)
            {
                CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_LOAD_LBADCPROFILE_ARMMEM_FAILED,
                        getMykonosErrorMessage(MYKONOS_ERR_LOAD_LBADCPROFILE_ARMMEM_FAILED));
                return MYKONOS_ERR_LOAD_LBADCPROFILE_ARMMEM_FAILED;
            }
        }
    }

    return MYKONOS_ERR_OK;
}

/**
 * \brief Helper function to calculate commonly used digital clock frequencies in the device
 *
 * User should not need to call this function normally.  It is called automatically
 * within other API functions to calculate the main digital clock.  If the pointers
 * to the return parameters are NULL, that calculation will be skipped and not
 * returned.
 *
 *
 * <B>Dependencies</B>
 * -device->clocks->clkPllVcoFreq_kHz;
 * -device->clocks->clkPllVcoDiv;
 * -device->clocks->clkPllHsDiv;
 *
 * \param device Pointer to the device settings structure
 * \param hsDigClk_kHz Return value for the calculated Mykonos high speed Digital clock at the output of the CLKPLL in kHz
 * \param hsDigClkDiv4or5_kHz Return value for the Mykonos high speed digital clock divided by 4 or 5 in kHz
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_CALCDIGCLK_NULLDEV_PARAM device parameter is a NULL pointer
 * \retval MYKONOS_ERR_CALCDIGCLK_NULL_CLKSTRUCT device->clocks structure is a NULL pointer
 * \retval MYKONOS_ERR_CLKPLL_INV_VCODIV Invalid CLKPLL VCO divider in device->clocks->clkPllVcoDiv
 * \retval MYKONOS_ERR_CLKPLL_INV_HSDIV Invalid CLKPLL High speed divider in device->clocks->clkPllHsDiv
 */
static mykonosErr_t MYKONOS_calculateDigitalClocks(mykonosDevice_t *device, uint32_t *hsDigClk_kHz, uint32_t *hsDigClkDiv4or5_kHz)
{
    uint32_t hsclkRate_kHz = 0;
    uint32_t clkPllVcoFrequency_kHz = 0;
    mykonosVcoDiv_t vcoDiv = VCODIV_1;
    uint8_t hsDiv = 4;

    if (device == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, 0, MYKONOS_ERR_CALCDIGCLK_NULLDEV_PARAM, getMykonosErrorMessage(MYKONOS_ERR_CALCDIGCLK_NULLDEV_PARAM));
        return MYKONOS_ERR_CALCDIGCLK_NULLDEV_PARAM;
    }

    if ((device->clocks == NULL))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_CALCDIGCLK_NULL_CLKSTRUCT,
                getMykonosErrorMessage(MYKONOS_ERR_CALCDIGCLK_NULL_CLKSTRUCT));
        return MYKONOS_ERR_CALCDIGCLK_NULL_CLKSTRUCT;
    }

    clkPllVcoFrequency_kHz = device->clocks->clkPllVcoFreq_kHz;
    vcoDiv = device->clocks->clkPllVcoDiv;
    hsDiv = device->clocks->clkPllHsDiv;

    switch (vcoDiv)
    {
        case VCODIV_1:
            hsclkRate_kHz = clkPllVcoFrequency_kHz;
            break;
        case VCODIV_1p5:
            hsclkRate_kHz = (clkPllVcoFrequency_kHz / 15) * 10;
            break;
        case VCODIV_2:
            hsclkRate_kHz = clkPllVcoFrequency_kHz >> 1;
            break;
        case VCODIV_3:
            hsclkRate_kHz = (clkPllVcoFrequency_kHz / 30) * 10;
            break;
        default:
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_CLKPLL_INV_VCODIV, getMykonosErrorMessage(MYKONOS_ERR_CLKPLL_INV_VCODIV));
            return MYKONOS_ERR_CLKPLL_INV_VCODIV;
        }
    }

    switch (hsDiv)
    {
        case 4:
            hsDiv = 4;
            break;
        case 5:
            hsDiv = 5;
            break;
        default:
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_CLKPLL_INV_HSDIV, getMykonosErrorMessage(MYKONOS_ERR_CLKPLL_INV_HSDIV));
            return MYKONOS_ERR_CLKPLL_INV_HSDIV;
        }
    }

    if (hsDigClk_kHz != NULL)
    {
        *hsDigClk_kHz = hsclkRate_kHz / hsDiv;
    }

    if (hsDigClkDiv4or5_kHz != NULL)
    {
        /* This digital div 4 or 5 is always mutually exclusive with the HS Divider (4 or 5) to always
         * create a /20 from the hsclk.
         */
        *hsDigClkDiv4or5_kHz = hsclkRate_kHz / 20;
    }

    return MYKONOS_ERR_OK;
}

/**
 * \brief Performs reset to the External Tx LO Leakage tracking calibration channel estimate
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device Pointer to the Mykonos device data structure containing settings
 * \param channelSel Enum selects the channel to reset
 *
 * \retval MYKONOS_ERR_RESET_TXLOL_INV_PARAM Selected channel is not valid
 * \retval MYKONOS_ERR_RESET_TXLOL_ARMERROR ARM error
 * \retval MYKONOS_ERR_OK Function completed successfully
 */
mykonosErr_t MYKONOS_resetExtTxLolChannel(mykonosDevice_t *device, mykonosTxChannels_t channelSel)
{
    const uint8_t TXLOL_RESET_CHANNEL_ESTIMATE = 0x01;
    const uint32_t GETTXLOLSTATUS_TIMEOUT_MS = 1000;

    mykonosErr_t retVal = MYKONOS_ERR_OK;
    uint8_t extData[3] = {MYKONOS_ARM_OBJECTID_GS_TRACKCALS, TXLOL_RESET_CHANNEL_ESTIMATE, 0};
    uint8_t cmdStatusByte = 0;
    uint8_t armErrorFlag = 0;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_resetExtTxLolChannel()\n");
#endif

    /* Check Channel */
    switch (channelSel)
    {
        case TX1:
            extData[2] = 0x01;
            break;
        case TX2:
            extData[2] = 0x02;
            break;
        case TX1_TX2:
            extData[2] = 0x03;
            break;
        default:
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_RESET_TXLOL_INV_PARAM,
                    getMykonosErrorMessage(MYKONOS_ERR_RESET_TXLOL_INV_PARAM));
            return MYKONOS_ERR_RESET_TXLOL_INV_PARAM;
    }

    /* throw error if not in radioOff/IDLE state */
    retVal = MYKONOS_checkArmState(device, (MYK_ARM_IDLE | MYK_ARM_RADIO_ON));
    if (retVal)
    {
        return retVal;
    }

    retVal = MYKONOS_sendArmCommand(device, MYKONOS_ARM_SET_OPCODE, &extData[0], sizeof(extData));
    if (retVal)
    {
        return retVal;
    }
    retVal = MYKONOS_waitArmCmdStatus(device, MYKONOS_ARM_SET_OPCODE, GETTXLOLSTATUS_TIMEOUT_MS, &cmdStatusByte);

    /* Error check WaitArmCmdStatus return */
    armErrorFlag = (cmdStatusByte >> 1);
    if (armErrorFlag > 0)
    {
        return MYKONOS_ERR_RESET_TXLOL_ARMERROR;
    }

    return retVal;
}

