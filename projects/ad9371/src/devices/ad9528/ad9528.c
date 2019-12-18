/**
 * \file ad9371/src/devices/ad9528/ad9528.c
 * \brief Contains source code for configuring and initializing AD9528 clock device
 */

/**
* Legal Disclaimer
* Copyright 2015-2017 Analog Devices Inc.
* Released under the AD9371 API license, for more information see the "LICENSE.txt" file in this zip file.
*
*/

#include <stdint.h>
#include "common.h"
#include "ad9528.h"

/**
 * \brief Performs a hard reset on the AD9528 DUT
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is structure pointer to the AD9528 data structure containing settings
 * \return Returns enum ADI_ERR, ADIERR_OK=pass, !ADIERR_OK=fail
 */
ADI_ERR AD9528_resetDevice(ad9528Device_t *device)
{
    /* toggle RESETB on device with matching spi chip select index */
    CMB_hardReset(device->spiSettings->chipSelectIndex);
    return ADIERR_OK;
}

/**
 * \brief Sets the AD9528 device SPI settings (3wire/4wire, MSBFirst, etc).
 *
 * This function will use the settings in the device->spiSettings structure
 * to set SPI stream mode, address auto increment direction, MSBFirst/LSBfirst,
 * and 3wire/4wire mode.  The AD9528 device always uses SPI MODE 0 (CPHA=0, CPOL=0).
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
 * \param device is structure pointer to the AD9528 data structure containing settings
 *
 * \return Returns enum ADI_ERR, ADIERR_OK=pass, !ADIERR_OK=fail
 */
ADI_ERR AD9528_setSpiSettings(ad9528Device_t *device)
{
    //device->spiSettings->enSpiStreaming
    uint8_t spiReg = 0;

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

    CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_ADI_SPI_CONFIG_A, spiReg);

    if (device->spiSettings->enSpiStreaming > 0)
    {
        /* Allow SPI streaming mode: SPI message ends when chip select deasserts */
        CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_ADI_SPI_CONFIG_B, 0x00);
    }
    else
    {
        /* Force single instruction mode */
        CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_ADI_SPI_CONFIG_B, 0x80);
    }

    return ADIERR_OK;
}


/**
 * \brief Helper function for ADI transceiver eval boards to init the AD9528 data structure
 *
 * This function inits the AD9528 device data structure with defaults that
 * are known to work with the ADI AD9369 and AD9371 evaluation boards.  NOTE: This function will
 * modify the contents of the AD9528 device data structure.
 *
 * <B>Dependencies</B>
 * - spi->chipSelectIndex
 * - spi
 *
 * \param device Pointer to the AD9528 device data structure to initialize
 * \param vcxoFrequency_Hz The VCXO frequency that is connected to the AD9528
 * \param refAFrequency_Hz The reference clock frequency being input into the AD9528 REFCLK A input
 * \param outputDeviceClock_Hz Desired output clock frequency to the FPGA and transceiver device
 *
 * \return Returns enum ADI_ERR, 0=pass, >0 = fail
*/
ADI_ERR AD9528_initDeviceDataStruct(ad9528Device_t *device, uint32_t vcxoFrequency_Hz, uint32_t refAFrequency_Hz, uint32_t outputDeviceClock_Hz)
{
    uint16_t chDiv;
    uint8_t i = 0;

    uint32_t pll1PfdFreq_Hz = 0;
    uint8_t pll1Ndiv = 0;
    uint8_t validPll1DivSetting = 0;
    uint8_t refAdiv = 1;


    uint8_t pll2RfDiv = 3; /* Valid values 3,4,5 */
    uint16_t pll2Ndiv = 1; /* Valid values 1-256 */
    uint32_t pll2InputFreq_Hz = 0;
    uint64_t pll2VcoFreq_Hz = 0;
    uint16_t pll2NdivStart = 0;
    uint8_t validPll2DivSetting = 0;
    uint64_t chDivU64 = 0;

    static const uint64_t PLL2_MIN_VCO_FREQ_HZ = 3450000000ULL;
    static const uint64_t PLL2_MAX_VCO_FREQ_HZ = 4025000000ULL;
    /*******************************************************************
     * PLL1 Settings
     *******************************************************************
     */

    /* find valid PLL1 Ndiv and REFA div */
    for (pll1Ndiv = 1; pll1Ndiv < 20; pll1Ndiv++)
    {
        pll1PfdFreq_Hz = vcxoFrequency_Hz / pll1Ndiv;

        if ((pll1PfdFreq_Hz <= 110000000) && (pll1PfdFreq_Hz >= 1000000) &&
            (pll1PfdFreq_Hz <= refAFrequency_Hz) &&
            (refAFrequency_Hz % pll1PfdFreq_Hz == 0)
           )
        {
            validPll1DivSetting = 1;
            refAdiv = refAFrequency_Hz / pll1PfdFreq_Hz ;
            break;
        }
    }

    if (validPll1DivSetting == 0)
    {
        return ADIERR_INV_PARM;
    }

    device->pll1Settings->nDivider = pll1Ndiv; //This init function always runs PLL1 at 30.72MHz Pfd rate
    device->pll1Settings->refA_bufferCtrl = DIFFERENTIAL;       //DISABLED = 0, SINGLE_ENDED, NEG_SINGLE_ENDED, DIFFERENTIAL
    device->pll1Settings->refA_Divider = refAdiv;
    device->pll1Settings->refA_Frequency_Hz = refAFrequency_Hz;
    device->pll1Settings->refB_bufferCtrl = DISABLED;           //DISABLED = 0, SINGLE_ENDED, NEG_SINGLE_ENDED, DIFFERENTIAL
    device->pll1Settings->refB_Divider = 1;
    device->pll1Settings->refB_Frequency_Hz = 0;
    device->pll1Settings->vcxoBufferCtrl  = NEG_SINGLE_ENDED;   //DISABLED = 0, SINGLE_ENDED, NEG_SINGLE_ENDED, DIFFERENTIAL
    device->pll1Settings->vcxo_Frequency_Hz = vcxoFrequency_Hz;

    /*******************************************************************
     * PLL2 Settings
     *******************************************************************
     */

    /*******************************************************************
     * Output Distribution Settings
     * 9369CE02A uses the following clock outputs
     * OUT 1: FPGA REFCLK
     * OUT 3: FPGA SYSREF
     * OUT 12: DUT SYSREF
     * OUT 13: DUT REFCLK
     *******************************************************************
     */

    /* PLL 2 input clock can be several different things.  Currently hard coded by this init funciton to use
     * PLL 1 output (VCXO Frequency) with no scaling
     */
    pll2InputFreq_Hz = device->pll1Settings->vcxo_Frequency_Hz;
    validPll2DivSetting = 0;

    /* Sweep PLL2 Divider settings to find something that has a valid integer divide (3,4,5)*/
    for (pll2RfDiv = 3; pll2RfDiv <= 5; pll2RfDiv++)
    {

        pll2NdivStart = (PLL2_MIN_VCO_FREQ_HZ / pll2RfDiv / pll2InputFreq_Hz);
        /* find valid PLL2 Ndiv (1-256) */
        for (pll2Ndiv = pll2NdivStart; pll2Ndiv <= 256; pll2Ndiv++)
        {
            pll2VcoFreq_Hz = (pll2InputFreq_Hz * pll2RfDiv * pll2Ndiv);

            if (pll2VcoFreq_Hz < PLL2_MIN_VCO_FREQ_HZ)
            {
                /* Continue to next PLL2 N div */
                continue;
            }
            else if (pll2VcoFreq_Hz > PLL2_MAX_VCO_FREQ_HZ)
            {
                /* Break to next PLL2 RF div */
                break;
            }

            if (((pll2VcoFreq_Hz / pll2RfDiv) % outputDeviceClock_Hz) == 0)
            {

                device->pll2Settings->rfDivider = pll2RfDiv;
                device->pll2Settings->n2Divider = pll2Ndiv;
                device->pll2Settings->totalNdiv = pll2RfDiv * pll2Ndiv;
                chDivU64 = ((pll2VcoFreq_Hz / pll2RfDiv) / outputDeviceClock_Hz);

                if ((chDivU64 < 1) || (chDivU64 > 256))
                {
                    return ADIERR_INV_PARM;
                }
                else
                {
                    chDiv = (uint16_t)(chDivU64);
                }

                validPll2DivSetting = 1;
                break;
            }
        }

        if (validPll2DivSetting == 1)
        {
            break;
        }
    }

    if (validPll2DivSetting == 0)
    {
        return ADIERR_INV_PARM;
    }

    device->outputSettings->outPowerDown = (uint16_t)(~0x300A);     //enable outputs 13,12,3 and 1

    for (i=0; i<14; i++){
        device->outputSettings->outSource[i] = CHANNEL_DIV; //CHANNEL_DIV , PLL1_OUTPUT, SYSREF, INV_PLL1_OUTPUT
        device->outputSettings->outBufferCtrl[i] = LVDS;        //LVDS, LVDS_BOOST, HSTL
        device->outputSettings->outChannelDiv[i] = chDiv;
        device->outputSettings->outFrequency_Hz[i] = outputDeviceClock_Hz;
        device->outputSettings->outAnalogDelay[i] = 0;
        device->outputSettings->outDigitalDelay[i] = 0;
    }


    device->outputSettings->outSource[3] = SYSREF;
    device->outputSettings->outSource[12] = SYSREF;


    device->sysrefSettings->sysrefDivide = 512; //SYSREF frequency = VCXO / 2 / sysrefDivde
    device->sysrefSettings->sysrefNshotMode = ONE_PULSE;        //ONE_PULSE=0, TWO_PULSES=2, FOUR_PULSES=3, SIX_PULSES=4, EIGHT_PULSES=5
    device->sysrefSettings->sysrefPatternMode = NSHOT;          //NSHOT=0, CONTINUOUS=1, PRBS=2, STOP=3
    device->sysrefSettings->sysrefPinBufferMode = DISABLED;     //DISABLED = 0, SINGLE_ENDED, NEG_SINGLE_ENDED, DIFFERENTIAL
    device->sysrefSettings->sysrefPinEdgeMode = LEVEL_ACTIVE_HIGH;  //LEVEL_ACTIVE_HIGH=0, LEVEL_ACTIVE_LOW=1, RISING_EDGE=2, FALLING_EDGE=3
    device->sysrefSettings->sysrefRequestMethod = SPI;          //SPI=0,PIN=1
    device->sysrefSettings->sysrefSource = INTERNAL;            //EXTERNAL=0, EXT_RESAMPLED=1, INTERNAL=2

    return ADIERR_OK;
}

/**
 * \brief Debug function to print out summary of AD9528 setup
 *
 * \param device AD9528 Device data structure describing setup of device.
 *
 * \return Returns enum ADI_ERR, 0=pass, >0 = fail
***********************************************************************************/
ADI_ERR AD9528_reportClockRates(ad9528Device_t *device)
{

    return ADIERR_OK;
} //debug function to help verify data structure is setup correctly.

/**
 * \brief Initializes the AD9528 by writing all SPI registers
 *
 * Initializes the AD9528 for use with the AD9369 transceiver.  Designed to work with
 * a reference clock of 122.88MHz.  Reference clock is expected to be 30.72MHz.
 * The output clocks from the AD9528 can run at 122.88M, 153.6M, 184.32M, 245.76M, or 307.2M.
 * Typical setup: REFA frequency = 30.72, VCXO = 122.88MHz
 * SYNC(SYSREF) pulses output at rate of 120kHz
 *
 * <B>Dependencies</B>
 * - All parameters in device structure
 *
 * \param device is structure pointer to AD9528 clock device structure
 *
 * \return Returns enum ADI_ERR, 0=pass, >0 = fail
 */
ADI_ERR AD9528_initialize(ad9528Device_t *device) { //writes all registers to desired values, leaves clock outputs off, SYSREF output disabled.
    uint8_t pll2_Adiv=0;
    uint8_t pll2_Bdiv=0;
    uint8_t reg108=0;
    uint8_t reg109=0;
    uint8_t i = 0;
    uint8_t j = 0;
    uint8_t reg508;
    ADI_ERR retval = ADIERR_OK;

    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, ADIERR_OK, "\n// AD9528_initialize\n");

    //CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_ADI_SPI_CONFIG_A, 0x18); //[4] and [3] =1 enable 4 wire SPI
    //CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_ADI_SPI_CONFIG_B, 0x00);
    retval = AD9528_setSpiSettings(device);
    if (retval != ADIERR_OK)
    {
        return retval;
    }

    /**********************************************************************
     * Configure PLL1
     **********************************************************************
     */

    CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_REF_A_DIVIDER_LSB, device->pll1Settings->refA_Divider & 0xFF);		//REF A div[7:0]
    CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_REF_A_DIVIDER_MSB, (device->pll1Settings->refA_Divider >> 8) & 0x03); //REF A div[9:8]
    CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_REF_B_DIVIDER_LSB, device->pll1Settings->refB_Divider & 0xFF);		//REF B div[7:0]
    CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_REF_B_DIVIDER_MSB, (device->pll1Settings->refB_Divider >> 8) & 0x03); //REF B div[9:8]
    CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_PLL1_N_DIV_LSB, device->pll1Settings->nDivider & 0xFF); //PLL1 N div[7:0]
    CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_PLL1_N_DIV_MSB, (device->pll1Settings->nDivider >> 8) & 0x03); //PLL1 N div[9:8]
    CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_PLL1_CHARGEPUMP, 0x0A); //PLL1 Charge Pump control 1
    CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_PLL1_CP_CTRL2, 0x03); //PLL1 Charge Pump control 2

    switch (device->pll1Settings->refA_bufferCtrl){
        case DISABLED:			reg108 &= ~0x28; reg109 &= ~0x01; break; //clear all REFA enable bits
        case SINGLE_ENDED:		reg108 |=  0x08; reg109 &= ~0x01; break; //Set REFA receiver enable, clear REFA SE negative input enable
        case NEG_SINGLE_ENDED:	reg108 |=  0x08; reg108 &= ~0x10; reg109 |= 0x01; break; //Disable REFA diff receiver, enable REFA receiver and use negative REFA input
        case DIFFERENTIAL:		reg108 |=  0x28; reg109 &= ~0x01; break;
        default:				reg108 |=  0x08; reg109 &= ~0x01; break; //default to single ended case
    }
    switch (device->pll1Settings->refB_bufferCtrl){
        case DISABLED:			reg108 &= ~0x50; reg109 &= ~0x02; break; //clear all REFB enable bits
        case SINGLE_ENDED:		reg108 |=  0x10; reg109 &= ~0x02; break; //Set REFB receiver enable, clear REFB SE negative input enable
        case NEG_SINGLE_ENDED:	reg108 |=  0x10; reg108 &= ~0x40; reg109 |= 0x02; break; //Disable REFB diff receiver, enable REFB receiver and use negative REFB input
        case DIFFERENTIAL:		reg108 |=  0x50; reg109 &= ~0x02; break;
        default:				reg108 |=  0x10; reg109 &= ~0x02; break; //default to single ended case
    }
    switch (device->pll1Settings->vcxoBufferCtrl){
        case DISABLED:			reg108 &= ~0x03; reg108 |=  0x04; break; //power down VCXO input
        case SINGLE_ENDED:		reg108 &= ~0x07; break; //Clear VCXO receiver power down, defaults to Single ended mode
        case NEG_SINGLE_ENDED:	reg108 &= ~0x07; reg108 |=  0x02; break; //Clear VCXO power down, enable Negative single ended mode[1]
        case DIFFERENTIAL:		reg108 &= ~0x07; reg108 |=  0x01; break; //Clear VCXO power down, enable differential input buffer[0]
        default:				reg108 &= ~0x07; break; //default to positive single ended case
    }


    CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_INPUT_RECEIVERS1, reg108); //Input receiver settings (single ended or differential clock inputs)
    CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_INPUT_RECEIVERS2, reg109); //Input receiver settings


    CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_INPUT_RECEIVERS3, 0x02); //Reference selection mode
    CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_PLL1_FASTLOCK, 0x00); //Disable Fast lock

    /**********************************************************************
     * Configure PLL2
     **********************************************************************
     */
    pll2_Bdiv = device->pll2Settings->totalNdiv / 4;
    pll2_Adiv = device->pll2Settings->totalNdiv % 4;

    if (pll2_Adiv > 3) {  //A div valid range (0 to 3)
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, ADIERR_INV_PARM, "\n// AD9528 PLL2 A Divider out of range (0 to 3)\n");
        return ADIERR_INV_PARM;
    }
    if (pll2_Bdiv < 3 || pll2_Bdiv > 63) {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, ADIERR_INV_PARM, "\n// AD9528 PLL2 B Divider out of range (3 to 63)\n");
        return ADIERR_INV_PARM;
    }
    CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_PLL2_CHARGEPUMP, 0xE6); //PLL2 Charge Pump[7:0]
    CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_PLL2_N_DIV, ((pll2_Adiv & 0x3) << 6) | (pll2_Bdiv & 0x3F)); //PLL2 A and B divider
    CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_PLL2_CTRL, 0x03); //PLL2 control
    CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_PLL2_VCO_CTRL, 0x01); //Manual VCO CAL

    if (device->pll2Settings->rfDivider < 3 || device->pll2Settings->rfDivider > 5) {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, ADIERR_INV_PARM, "\n// AD9528 PLL2 RF Divider out of range (3,4, or 5)\n");
        return ADIERR_INV_PARM;
    }

    CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_PLL2_VCO_DIV, device->pll2Settings->rfDivider); //VCO divider
    CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_PLL2_LF_CTRL1, 0x2A); //PLL2 Loop filter [7:0]
    CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_PLL2_LF_CTRL2, 0x00); //PLL2 Loop filter [8]
    CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_PLL2_RDIV, 0x00); //R divider
    CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_PLL2_REPLICA_CHDIV, (device->pll2Settings->totalNdiv / device->pll2Settings->rfDivider) -1 ); //Replica channel divider: Ndivider = rfDivider * R divider
    CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_PLL2_REPLICA_DIV_PHASE, 0x00); //Replica Channel Divider phase

    /**********************************************************************
     * Configure Output Distribution
     **********************************************************************
     */
    for (i=0; i<14; i=i+1){
        CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_CH_OUT0_CTRL1 + j, ((device->outputSettings->outSource[i] & 0x7) << 5) | (device->outputSettings->outAnalogDelay[i] & 0x1F));
        CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_CH_OUT0_CTRL2 + j, ((device->outputSettings->outBufferCtrl[i] & 0x3) << 6) | (device->outputSettings->outDigitalDelay[i] & 0x3F));
        CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_CH_OUT0_CHDIV + j, device->outputSettings->outChannelDiv[i]-1);
        j = j+3;
    }

    CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_OUTPUT_SYNC, 0x00); //Sync outputs
    CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_MASK_SYNC1, 0x00); //Mask SYNC[7:0]
    CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_MASK_SYNC2, 0x00); //Mask SYNC[13:8]
    CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_EN_OUTPUT_PATH_SEL1, 0x00); //Bypass SYSREF resample [6:0], bit0:Enable VCXO to feed output distrubution
    CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_EN_OUTPUT_PATH_SEL2, 0x00); //Bypass SYSREF resample [13:7]

    /**********************************************************************
     * Configure SYSREF operation
     **********************************************************************
     */
    CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_SYSERF_DIV_LSB, device->sysrefSettings->sysrefDivide & 0xFF); //SYSREF divider [7:0]
    CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_SYSERF_DIV_MSB, (device->sysrefSettings->sysrefDivide >> 8) & 0xFF); //SYSREF divider [7:0]

    CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_SYSREF_CTRL3, ((device->sysrefSettings->sysrefRequestMethod & 0x1) << 7) |
    		                                     ((device->sysrefSettings->sysrefPinEdgeMode & 0x3) << 5) );

    CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_SYSREF_CTRL4, ((device->sysrefSettings->sysrefSource & 0x03) << 6) |
    		                                                        ((device->sysrefSettings->sysrefPatternMode & 0x3) << 4 ) |
    		                                                        ((device->sysrefSettings->sysrefNshotMode & 0x7) << 1) );

    if(device->sysrefSettings->sysrefPinBufferMode == DISABLED)
    {
    	CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_SYSREF_CTRL5, 0x04); //power down input buffer for external SYSREF
    }
    else if(device->sysrefSettings->sysrefPinBufferMode == SINGLE_ENDED)
    {
    	CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_SYSREF_CTRL5, 0x00); //enable positive singled ended input
    }
    else if(device->sysrefSettings->sysrefPinBufferMode == NEG_SINGLE_ENDED)
    {
    	CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_SYSREF_CTRL5, 0x02); //enable negative singled ended input
    }
    else if(device->sysrefSettings->sysrefPinBufferMode == DIFFERENTIAL)
    {
    	CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_SYSREF_CTRL5, 0x01); //enable differential input buffer
    }
    else
    {
    	CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_SYSREF_CTRL5, 0x04); //power down input buffer for external SYSREF
    }

    /**********************************************************************
     * Power down control
     **********************************************************************
     */

    if (device->pll1Settings->bypassPll1 > 0)
    {
        CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_POWERDOWN_CTRL, 0x14);// power up PLL2 and output distribution, power down PLL1
    }
    else
    {
        CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_POWERDOWN_CTRL, 0x10);// power up PLL1, PLL2 and output distribution
    }

    CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_CH_POWERDOWN1, device->outputSettings->outPowerDown & 0xFF); //power down out[7:0]
    CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_CH_POWERDOWN2, ((device->outputSettings->outPowerDown >> 8) & 0x3F)); //power down out[13:8]
    CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_LDO_ENABLES1, 0xFF); //LDO enable per output channel [7:0]
    CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_LDO_ENABLES2, 0xFF); //LDO enable per output channel [15:8]
    CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_STATUS0_CTRL, 0x07); //STATUS 0 outputs REFA present
    CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_STATUS1_CTRL, 0x01); //STATUS 1 outputs PLL1 and PLL2 locked
    CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_STATUS_OE, 0x0C);// Make Status0 and Status1 outputs after reset.

    CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_IO_UPDATE, 0x01); //IO update to make all these settings valid in the 9528

    CMB_setTimeout_ms(1000);
     do{
         CMB_SPIReadByte(device->spiSettings, AD9528_ADDR_STATUS_READBACK0, &reg508);
         if(CMB_hasTimeoutExpired() > 0)
         {
             return ADIERR_FAILED;
         }
     }while((reg508 & 0x27) != 0x27);

    return ADIERR_OK;
}

/// Send a SPI message to request a SYSREF pulse or continuous SYSREF from the AD9528.
/*******************************************************************************//**
 * Requests a SYSREF from the AD9528.  It will use whatever settings for SYSREF
 * that are current configured in the AD9528.  This could be a single pulse,
 * multiple pulses, or continuous pulses.
 * <B>Dependencies</B>
 * - device->spiSettings
 * - device->sysrefSettings->sysrefPatternMode
 *
 * \param device is structure pointer to AD9528 clock device structure
 * \param enableSYSREF If NSHOT SYSREF mode, this parameter is ignored.  If PRBS/CONTINUOUS SYSREF mode, 1= enable SYSREF, 0= disable SYSREF.
 *
 * \return Returns enum ADI_ERR, 0=pass, >0 = fail
 ***********************************************************************************/
ADI_ERR AD9528_requestSysref(ad9528Device_t *device, uint8_t enableSYSREF){
    uint8_t _enableSYSREF=0;

    if (device->sysrefSettings->sysrefPatternMode == NSHOT || enableSYSREF > 0)
    {
        _enableSYSREF = 1;
    }
    else
    {
        _enableSYSREF = 0;
    }

    CMB_SPIWriteField(device->spiSettings, AD9528_ADDR_SYSREF_CTRL4, _enableSYSREF, 0x01, 0x00); //Set SPI SYSREF request bit
    CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_IO_UPDATE, 0x01); //IO update to make all these settings valid in the 9528

    return ADIERR_OK;
}

/// Allow changing the 9528 SYSREF frequency and pattern mode(CONTINUOUS, PRBS vs NSHOT mode)
/*******************************************************************************//**
 * NOTE: this function will update the device structure with the parameter values passed
 * into this function. The device->sysrefSettings->sysrefPatternMode, device->sysrefSettings->sysrefNshotMode,
 * and device->sysrefSettings->sysrefDivide values will be updated in the device structure.
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 *
 * \param device is structure pointer to AD9528 clock device structure
 * \param divideFromPll1Out Sets the SYSREF pulse frequency. SYSREF frequency = PLL1_VCXO frequency/divideFromPll1Out
 * \param sysrefPatternMode Valid settings: NSHOT, CONTINUOUS, PRBS, STOP
 * \param nShotPulses Valid settings: ONE_PULSE=0, TWO_PULSES=2, FOUR_PULSES=3, SIX_PULSES=4, EIGHT_PULSES=5
 *
 * \return Returns enum ADI_ERR, 0=pass, >0 = fail
 ***********************************************************************************/
ADI_ERR AD9528_setupSYSREF(ad9528Device_t *device, uint16_t divideFromPll1Out,  sysrefPatternMode_t sysrefPatternMode, sysrefNshotMode_t nShotPulses)
{
    uint8_t reg403=0;

    device->sysrefSettings->sysrefPatternMode = sysrefPatternMode;
    device->sysrefSettings->sysrefNshotMode = nShotPulses;
    device->sysrefSettings->sysrefDivide = divideFromPll1Out >> 1;  //SYSREF frequency is (PLL1)VCXO frequency / 2 / device->sysrefSettings->sysrefDivide

    CMB_SPIReadByte(device->spiSettings, AD9528_ADDR_SYSREF_CTRL4, &reg403);

    reg403 = reg403 & ~0x01; //clear bit 0 to stop a continuous sysref if it is on.
    CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_SYSREF_CTRL4, reg403);

    reg403 = reg403 & ~0x3E; //clear bits for pattern mode and Nshot mode bitfields
    reg403 = reg403 | ((sysrefPatternMode & 0x3) << 4) | ((nShotPulses & 0x7) << 1);
    CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_SYSREF_CTRL4, reg403); //update internal sysref pattern mode and Nshot mode (number of pulses)

    //SYSREF request bit x403[0] is left 0.  The next call to AD9528_requestSysref will start the SYSREF
    CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_IO_UPDATE, 0x01); //IO update to make all these settings valid in the 9528


    return ADIERR_OK;
}

/// Update the AD9528 clock outputs that are enabled.
/*******************************************************************************//**
 * NOTE: this function will update the device structure with the parameter values passed
 * into this function. The device->sysrefSettings->sysrefPatternMode, device->sysrefSettings->sysrefNshotMode,
 * and device->sysrefSettings->sysrefDivide values will be updated in the device structure.
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 *
 * \param device is structure pointer to AD9528 clock device structure
 * \param clkEnable Sets which clock outputs are enabled. Bit per clock output. bit0 = out0, etc.
 *
 * \return Returns enum ADI_ERR, 0=pass, >0 = fail
 ***********************************************************************************/
ADI_ERR AD9528_enableClockOutputs(ad9528Device_t *device, uint16_t clkEnable)
{ //clkEnable: bit per clock output

    device->outputSettings->outPowerDown = ~clkEnable;
    CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_CH_POWERDOWN1, device->outputSettings->outPowerDown & 0xFF); //power down out[7:0]
    CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_CH_POWERDOWN2, ((device->outputSettings->outPowerDown >> 8) & 0x3F)); //power down out[13:8]

    CMB_SPIWriteByte(device->spiSettings, AD9528_ADDR_IO_UPDATE, 0x01); //IO update to make all these settings valid in the 9528
    return ADIERR_OK;
}

/// Waits for PLL1 and PLL2 to lock and the REFA and VCXO clocks to be present
/*******************************************************************************//**
 * Waits until Reference A clock and VCXO clock input to be detected (present),
 * and PLL1 and PLL2 to lock.  Will timeout after 1 second and return ADIERR_FAILED.
 * If clock are detected and PLLs locked, returns ADIERR_OK.
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 *
 * \param device is structure pointer to AD9528 clock device structure
 *
 * \return Returns enum ADI_ERR, 0=pass, >0 = fail
 ***********************************************************************************/
ADI_ERR AD9528_waitForPllLock(ad9528Device_t *device)
{
    uint8_t reg508;

    CMB_setTimeout_ms(1000);
    do{
        CMB_SPIReadByte(device->spiSettings, AD9528_ADDR_STATUS_READBACK0, &reg508);
        if(CMB_hasTimeoutExpired() > 0)
        {
            return ADIERR_FAILED;
        }
    }while((reg508 & 0x27) != 0x27);

    return ADIERR_OK;
}

/// Reads the Lock Status of the PLLs and present detect of the reference clocks
/*******************************************************************************//**
 * NOTE: This is a non blocking function that reads a register to see if PLL1
 * and PLL2 are locked and Reference clock A and the VCXO input clock are
 * detected (present). If the *status parameter is not NULL, the status will
 * be returned.  If *status is NULL, the error return value will denote if
 * the PllStatus is locked or not with ADIERR_OK or ADIERR_FAILED.
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 *
 * \param device is structure pointer to AD9528 clock device structure
 * \param status PLL status 
 *
 * \return Returns enum ADI_ERR, 0=pass, >0 = fail
 ***********************************************************************************/
ADI_ERR AD9528_readPllStatus(ad9528Device_t *device, uint8_t *status)
{
    uint8_t reg508;

    //bit5 is VCXO clock detected
    //bit2 is REFCLK A detected
    //bit1 is PLL2 lock detect
    //bit0 is PLL1 lock detect

    /* read twice to refresh status bits */
    CMB_SPIReadByte(device->spiSettings, AD9528_ADDR_STATUS_READBACK0, &reg508);
    CMB_SPIReadByte(device->spiSettings, AD9528_ADDR_STATUS_READBACK0, &reg508);
    /* if status is a valid pointer, return the PLL status */
    if (status > 0)
    {
        *status = reg508;
    }

    /* PLLs 1 and 2 are locked and VCXO and REFA detected if reg508 has bits 5,2,1 and 0 set */
    if((reg508 & 0x27) == 0x27)
        return ADIERR_OK;
    else
        return ADIERR_FAILED;
}
