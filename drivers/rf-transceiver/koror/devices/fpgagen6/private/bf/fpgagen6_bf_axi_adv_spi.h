/** 
 * \file fpgagen6_bf_axi_adv_spi.h Automatically generated file with generator ver 0.0.16.0.
 * 
 * \brief Contains BitField functions to support FPGAGEN6 transceiver device.
 * 
 * FPGAGEN6 BITFIELD VERSION: 0.0.0.11
 * 
 * Disclaimer Legal Disclaimer
 * 
 * Copyright 2015 - 2023 Analog Devices Inc.
 * 
 * Released under the FPGAGEN6 API license, for more information see the "LICENSE.PDF" file in this zip file.
 */

#ifndef _FPGAGEN6_BF_AXI_ADV_SPI_H_
#define _FPGAGEN6_BF_AXI_ADV_SPI_H_

#include "adi_fpgagen6_types.h"
#include "adi_fpgagen6_error.h"
#include "../../private/bf/fpgagen6_bf_axi_adv_spi_types.h"

#ifndef ADI_API
  #ifdef __cplusplus
    #define ADI_API extern "C"
  #else
    #define ADI_API
  #endif
#endif



/**
 * \brief When the advance feature set for this slave is enabled, this field indicates which bit in the transfer begins the tristate while in three wire mode.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_AdvFeat3wireTriPoint_BfSet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               const uint8_t bfValue);


/**
 * \brief When the advance feature set for this slave is enabled, this field indicates which bit in the transfer begins the tristate while in three wire mode.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_AdvFeat3wireTriPoint_BfGet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               uint8_t* const bfValue);



/**
 * \brief When the advanced feature set is enabled for this particular slave select, sets the clock divider when this slave select is enabled.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_AdvFeatClockDiv_BfSet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                          uint8_t channelId, 
                                                                          const uint16_t bfValue);


/**
 * \brief When the advanced feature set is enabled for this particular slave select, sets the clock divider when this slave select is enabled.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_AdvFeatClockDiv_BfGet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                          uint8_t channelId, 
                                                                          uint16_t* const bfValue);



/**
 * \brief When the advanced feature set is enabled for this particular slave select, sets the clock phase when this slave select is enabled.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_AdvFeatCpha_BfSet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                      uint8_t channelId, 
                                                                      const uint8_t bfValue);


/**
 * \brief When the advanced feature set is enabled for this particular slave select, sets the clock phase when this slave select is enabled.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_AdvFeatCpha_BfGet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                      uint8_t channelId, 
                                                                      uint8_t* const bfValue);



/**
 * \brief When the advanced feature set is enabled for this particular slave select, sets the clock polarity when this slave select is enabled.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_AdvFeatCpol_BfSet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                      uint8_t channelId, 
                                                                      const uint8_t bfValue);


/**
 * \brief When the advanced feature set is enabled for this particular slave select, sets the clock polarity when this slave select is enabled.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_AdvFeatCpol_BfGet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                      uint8_t channelId, 
                                                                      uint8_t* const bfValue);



/**
 * \brief When set, this bit enables the advanced feature set.  When clear, the default configuration is used for this particular slave select.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_AdvFeatEnable_BfSet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                        uint8_t channelId, 
                                                                        const uint8_t bfValue);


/**
 * \brief When set, this bit enables the advanced feature set.  When clear, the default configuration is used for this particular slave select.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_AdvFeatEnable_BfGet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                        uint8_t channelId, 
                                                                        uint8_t* const bfValue);



/**
 * \brief When the advanced feature set is enabled for this particular slave select, sets the four wire mode when this slave select is enabled.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_AdvFeatFourWire_BfSet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                          uint8_t channelId, 
                                                                          const uint8_t bfValue);


/**
 * \brief When the advanced feature set is enabled for this particular slave select, sets the four wire mode when this slave select is enabled.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_AdvFeatFourWire_BfGet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                          uint8_t channelId, 
                                                                          uint8_t* const bfValue);



/**
 * \brief When the advanced feature set is enabled for this particular slave select, sets the LSB First Mode when this slave select is enabled.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_AdvFeatLsbFirst_BfSet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                          uint8_t channelId, 
                                                                          const uint8_t bfValue);


/**
 * \brief When the advanced feature set is enabled for this particular slave select, sets the LSB First Mode when this slave select is enabled.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_AdvFeatLsbFirst_BfGet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                          uint8_t channelId, 
                                                                          uint8_t* const bfValue);



/**
 * \brief When the advanced feature set is enabled for this particular slave select, setting this bit to 1 will move the sample point of the MISO data to the next clock edge (half period).
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_AdvFeatMisoSampleSlip_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                                uint8_t channelId, 
                                                                                const uint8_t bfValue);


/**
 * \brief When the advanced feature set is enabled for this particular slave select, setting this bit to 1 will move the sample point of the MISO data to the next clock edge (half period).
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_AdvFeatMisoSampleSlip_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                                uint8_t channelId, 
                                                                                uint8_t* const bfValue);



/**
 * \brief When this bit is set to 0, the SPI_CLK is based off of a 100MHz free-running clock. When this bit is set to 1, the SPI_CLK is based off of the device clock. The device clock must be set up via the SPI prior to setting this bit to 1.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_AdvFeatSpiBaseClkSel_BfSet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               const uint8_t bfValue);


/**
 * \brief When this bit is set to 0, the SPI_CLK is based off of a 100MHz free-running clock. When this bit is set to 1, the SPI_CLK is based off of the device clock. The device clock must be set up via the SPI prior to setting this bit to 1.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_AdvFeatSpiBaseClkSel_BfGet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               uint8_t* const bfValue);



/**
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_BlockId_BfGet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                  uint8_t* const bfValue);



/**
 * \brief Selects the edge of the base clock to align the rising edge of SPI_CLK. This value must be less than or equal to the clock divider x 2. The clock divider bit field specifies the number of base clock cycles in half of a SPI Clock period. The value written should be value - 1.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_ClockDivPhase_BfSet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                        const uint32_t bfValue);


/**
 * \brief Selects the edge of the base clock to align the rising edge of SPI_CLK. This value must be less than or equal to the clock divider x 2. The clock divider bit field specifies the number of base clock cycles in half of a SPI Clock period. The value written should be value - 1.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_ClockDivPhase_BfGet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                        uint32_t* const bfValue);



/**
 * \brief Setting this bit defines the default clock phase.  The default is used when the desired chip select doesn't have an advanced configuration.
 *        When set to:
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_Cpha_BfSet(adi_fpgagen6_Device_t* const device,
                                                               const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                               const uint8_t bfValue);


/**
 * \brief Setting this bit defines the default clock phase.  The default is used when the desired chip select doesn't have an advanced configuration.
 *        When set to:
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_Cpha_BfGet(adi_fpgagen6_Device_t* const device,
                                                               const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                               uint8_t* const bfValue);



/**
 * \brief Setting this bit defines the default clock polarity.  The default is used when the desired chip select doesn't have an advanced configuration.
 *        When set to:
 *        0 = Active-High clock; SCK idles Low.
 *        1 = Active-Low clock; SCK idles High.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_Cpol_BfSet(adi_fpgagen6_Device_t* const device,
                                                               const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                               const uint8_t bfValue);


/**
 * \brief Setting this bit defines the default clock polarity.  The default is used when the desired chip select doesn't have an advanced configuration.
 *        When set to:
 *        0 = Active-High clock; SCK idles Low.
 *        1 = Active-Low clock; SCK idles High.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_Cpol_BfGet(adi_fpgagen6_Device_t* const device,
                                                               const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                               uint8_t* const bfValue);



/**
 * \brief IPISR Bit[4] is the data receive register full interrupt.
 *        Without FIFOs, this bit is set at the end of a SPI element
 *        transfer by a one-clock period strobe to the interrupt
 *        register (An element can be a byte, half-word, or word
 *        depending on the value of Transfer Width).
 *        With FIFOs, this bit is set at the end of the SPI element
 *        transfer, when the receive FIFO has been completely filled
 *        by a one-clock period strobe to the interrupt register.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_DrrFull_BfSet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                  const uint8_t bfValue);


/**
 * \brief IPISR Bit[4] is the data receive register full interrupt.
 *        Without FIFOs, this bit is set at the end of a SPI element
 *        transfer by a one-clock period strobe to the interrupt
 *        register (An element can be a byte, half-word, or word
 *        depending on the value of Transfer Width).
 *        With FIFOs, this bit is set at the end of the SPI element
 *        transfer, when the receive FIFO has been completely filled
 *        by a one-clock period strobe to the interrupt register.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_DrrFull_BfGet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                  uint8_t* const bfValue);



/**
 * \brief 0 = Disabled.
 *        1 = Enabled.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_DrrFullIen_BfSet(adi_fpgagen6_Device_t* const device,
                                                                     const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                     const uint8_t bfValue);


/**
 * \brief 0 = Disabled.
 *        1 = Enabled.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_DrrFullIen_BfGet(adi_fpgagen6_Device_t* const device,
                                                                     const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                     uint8_t* const bfValue);



/**
 * \brief IPISR Bit[5] is the data receive FIFO overrun interrupt. This
 *        bit is set by a one-clock period strobe to the interrupt
 *        register when an attempt to write data to a full receive
 *        register or FIFO is made by the SPI core logic to complete a
 *        SPI transfer.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_DrrOverrun_BfSet(adi_fpgagen6_Device_t* const device,
                                                                     const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                     const uint8_t bfValue);


/**
 * \brief IPISR Bit[5] is the data receive FIFO overrun interrupt. This
 *        bit is set by a one-clock period strobe to the interrupt
 *        register when an attempt to write data to a full receive
 *        register or FIFO is made by the SPI core logic to complete a
 *        SPI transfer.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_DrrOverrun_BfGet(adi_fpgagen6_Device_t* const device,
                                                                     const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                     uint8_t* const bfValue);



/**
 * \brief 0 = Disabled.
 *        1 = Enabled.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_DrrOverrunIen_BfSet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                        const uint8_t bfValue);


/**
 * \brief 0 = Disabled.
 *        1 = Enabled.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_DrrOverrunIen_BfGet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                        uint8_t* const bfValue);



/**
 * \brief IPISR Bit[2] is the data transmit register/FIFO empty
 *        interrupt. It is set when the last byte of data has been
 *        transferred out to the external flash memory.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_DtrEmpty_BfSet(adi_fpgagen6_Device_t* const device,
                                                                   const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                   const uint8_t bfValue);


/**
 * \brief IPISR Bit[2] is the data transmit register/FIFO empty
 *        interrupt. It is set when the last byte of data has been
 *        transferred out to the external flash memory.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_DtrEmpty_BfGet(adi_fpgagen6_Device_t* const device,
                                                                   const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                   uint8_t* const bfValue);



/**
 * \brief 0 = Disabled.
 *        1 = Enabled.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_DtrEmptyIen_BfSet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                      const uint8_t bfValue);


/**
 * \brief 0 = Disabled.
 *        1 = Enabled.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_DtrEmptyIen_BfGet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                      uint8_t* const bfValue);



/**
 * \brief Allows passing all individually enabled interrupts to the
 *        interrupt controller.
 *        When set to:
 *        0 = Disabled.
 *        1 = Enabled.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_Gie_BfSet(adi_fpgagen6_Device_t* const device,
                                                              const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                              const uint8_t bfValue);


/**
 * \brief Allows passing all individually enabled interrupts to the
 *        interrupt controller.
 *        When set to:
 *        0 = Disabled.
 *        1 = Enabled.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_Gie_BfGet(adi_fpgagen6_Device_t* const device,
                                                              const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                              uint8_t* const bfValue);



/**
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_InstanceId_BfGet(adi_fpgagen6_Device_t* const device,
                                                                     const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                     uint8_t* const bfValue);



/**
 * \brief When this bit is 1 the state machine is busy. When the bit is 0, the state machine is idle.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_IocheckBusy_BfGet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                      uint8_t* const bfValue);



/**
 * \brief Current cycle count. This is the value of the down-counting cycle counter. It is the number of cycles remaining in the test.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_IocheckCurrentCycle_BfGet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                              uint32_t* const bfValue);



/**
 * \brief This bit field enables the state machine. It switches the appropriate controls to be driven by the state machine. The state machine is idle until triggered.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_IocheckEnable_BfSet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                        const uint8_t bfValue);


/**
 * \brief This bit field enables the state machine. It switches the appropriate controls to be driven by the state machine. The state machine is idle until triggered.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_IocheckEnable_BfGet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                        uint8_t* const bfValue);



/**
 * \brief The number of comparison errors detected.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_IocheckErrorCount_BfGet(adi_fpgagen6_Device_t* const device,
                                                                            const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                            uint32_t* const bfValue);



/**
 * \brief Current state of the state machine. 4'h0 = idle; 4'h1 = loading write 0 into FIFO; 4'h2 = waiting for write 0 to complete; 4'h3 = unloading write 0 from FIFO; 4'h4 = loading read 0 into FIFO; 4'h5 = waiting for read 0 to complete; 4'h6 = unloading read 0 from FIFO; 4'h7 = loading write 1 into FIFO; 4'h8 = waiting for write 1 to complete; 4'h9 = unloading write 1 from FIFO; 4'hA = loading read 1 into FIFO; 4'hB = waiting for read 1 to complete; 4'hC = unloading read 1 from FIFO;
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_IocheckFsmState_BfGet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                          uint16_t* const bfValue);



/**
 * \brief The number of write/read/write/read cycles. This bitfield is cycles - 1.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_IocheckRdwrCycles_BfSet(adi_fpgagen6_Device_t* const device,
                                                                            const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                            const uint32_t bfValue);


/**
 * \brief The number of write/read/write/read cycles. This bitfield is cycles - 1.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_IocheckRdwrCycles_BfGet(adi_fpgagen6_Device_t* const device,
                                                                            const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                            uint32_t* const bfValue);



/**
 * \brief Enter the value of the slave select. 4'h0 = DUT; 4'h1 = AD9528; 4'h2 = SPI2; others unused
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_IocheckSlaveSelect_BfSet(adi_fpgagen6_Device_t* const device,
                                                                             const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                             const uint8_t bfValue);


/**
 * \brief Enter the value of the slave select. 4'h0 = DUT; 4'h1 = AD9528; 4'h2 = SPI2; others unused
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_IocheckSlaveSelect_BfGet(adi_fpgagen6_Device_t* const device,
                                                                             const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                             uint8_t* const bfValue);



/**
 * \brief The SPI address to use in the test.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_IocheckTestAddress_BfSet(adi_fpgagen6_Device_t* const device,
                                                                             const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                             const uint16_t bfValue);


/**
 * \brief The SPI address to use in the test.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_IocheckTestAddress_BfGet(adi_fpgagen6_Device_t* const device,
                                                                             const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                             uint16_t* const bfValue);



/**
 * \brief The first value written to the SPI register. The state machine will alternated between test_data_0 and test data_1.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_IocheckTestData0_BfSet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);


/**
 * \brief The first value written to the SPI register. The state machine will alternated between test_data_0 and test data_1.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_IocheckTestData0_BfGet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                           uint8_t* const bfValue);



/**
 * \brief The second value written to the SPI register. The state machine will alternated between test_data_0 and test data_1.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_IocheckTestData1_BfSet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);


/**
 * \brief The second value written to the SPI register. The state machine will alternated between test_data_0 and test data_1.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_IocheckTestData1_BfGet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                           uint8_t* const bfValue);



/**
 * \brief Write a 1 to start the state machine. this bit is self clearing.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_IocheckTrigger_BfSet(adi_fpgagen6_Device_t* const device,
                                                                         const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                         const uint8_t bfValue);


/**
 * \brief Write a 1 to start the state machine. this bit is self clearing.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_IocheckTrigger_BfGet(adi_fpgagen6_Device_t* const device,
                                                                         const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                         uint8_t* const bfValue);



/**
 * \brief The number of base clock cycles to wait after the reads and writes. This is base clock cycles not PSI clock cycles. The value is cycles - 1.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_IocheckWaitCycles_BfSet(adi_fpgagen6_Device_t* const device,
                                                                            const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                            const uint32_t bfValue);


/**
 * \brief The number of base clock cycles to wait after the reads and writes. This is base clock cycles not PSI clock cycles. The value is cycles - 1.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_IocheckWaitCycles_BfGet(adi_fpgagen6_Device_t* const device,
                                                                            const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                            uint32_t* const bfValue);



/**
 * \brief When set to a 1, the SPI_CLK will be locked to the same phase of the base clock. When cleared to 0, SPI_CLK maybe be at random phases of base clock.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_LockClockPhase_BfSet(adi_fpgagen6_Device_t* const device,
                                                                         const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                         const uint8_t bfValue);


/**
 * \brief When set to a 1, the SPI_CLK will be locked to the same phase of the base clock. When cleared to 0, SPI_CLK maybe be at random phases of base clock.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_LockClockPhase_BfGet(adi_fpgagen6_Device_t* const device,
                                                                         const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                         uint8_t* const bfValue);



/**
 * \brief Enables local loopback operation and is functional only in
 *        standard SPI master mode.
 *        When set to:
 *        • 0 = Normal operation.
 *        • 1 = Loopback mode. The transmitter output is internally
 *        connected to the receiver input. The receiver and
 *        transmitter operate normally, except that received data
 *        (from remote slave) is ignored
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_Loop_BfSet(adi_fpgagen6_Device_t* const device,
                                                               const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                               const uint8_t bfValue);


/**
 * \brief Enables local loopback operation and is functional only in
 *        standard SPI master mode.
 *        When set to:
 *        • 0 = Normal operation.
 *        • 1 = Loopback mode. The transmitter output is internally
 *        connected to the receiver input. The receiver and
 *        transmitter operate normally, except that received data
 *        (from remote slave) is ignored
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_Loop_BfGet(adi_fpgagen6_Device_t* const device,
                                                               const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                               uint8_t* const bfValue);



/**
 * \brief This bit selects LSB first data transfer format.
 *        The default transfer format is MSB first.
 *        When set to:
 *        0 = MSB first transfer format.
 *        1 = LSB first transfer format.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_LsbFirst_BfSet(adi_fpgagen6_Device_t* const device,
                                                                   const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                   const uint8_t bfValue);


/**
 * \brief This bit selects LSB first data transfer format.
 *        The default transfer format is MSB first.
 *        When set to:
 *        0 = MSB first transfer format.
 *        1 = LSB first transfer format.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_LsbFirst_BfGet(adi_fpgagen6_Device_t* const device,
                                                                   const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                   uint8_t* const bfValue);



/**
 * \brief Manual slave select assertion enable:
 *        This bit forces the data in the slave select register to be
 *        asserted on the slave select output anytime the device is
 *        configured as a master and the device is enabled (SPE
 *        asserted).
 *        This bit has no effect on slave operation.
 *        When set to:
 *        0 = Slave select output asserted by master core logic.
 *        1 = Slave select output follows data in slave select register.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_ManSsEn_BfSet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                  const uint8_t bfValue);


/**
 * \brief Manual slave select assertion enable:
 *        This bit forces the data in the slave select register to be
 *        asserted on the slave select output anytime the device is
 *        configured as a master and the device is enabled (SPE
 *        asserted).
 *        This bit has no effect on slave operation.
 *        When set to:
 *        0 = Slave select output asserted by master core logic.
 *        1 = Slave select output follows data in slave select register.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_ManSsEn_BfGet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                  uint8_t* const bfValue);



/**
 * \brief This module is Master only.  Writing this bit has no effect and it will always read 1.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_Master_BfSet(adi_fpgagen6_Device_t* const device,
                                                                 const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                 const uint8_t bfValue);


/**
 * \brief This module is Master only.  Writing this bit has no effect and it will always read 1.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_Master_BfGet(adi_fpgagen6_Device_t* const device,
                                                                 const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                 uint8_t* const bfValue);



/**
 * \brief IPISR Bit[0] is the mode-fault error flag.
 *        This interrupt is generated if the SS signal goes active while
 *        the SPI device is configured as a master. This bit is set
 *        immediately on SS going active.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_Modf_BfSet(adi_fpgagen6_Device_t* const device,
                                                               const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                               const uint8_t bfValue);


/**
 * \brief IPISR Bit[0] is the mode-fault error flag.
 *        This interrupt is generated if the SS signal goes active while
 *        the SPI device is configured as a master. This bit is set
 *        immediately on SS going active.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_Modf_BfGet(adi_fpgagen6_Device_t* const device,
                                                               const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                               uint8_t* const bfValue);



/**
 * \brief This flag is set if the SS signal goes active while the SPI
 *        device is configured as a master. MODF is automatically
 *        cleared by reading the SPISR. A Low-to-High MODF
 *        transition generates a single-cycle strobe interrupt.
 *        0 = No error.
 *        1 = Error condition detected.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_ModfFlag_BfGet(adi_fpgagen6_Device_t* const device,
                                                                   const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                   uint8_t* const bfValue);



/**
 * \brief 0 = Disabled.
 *        1 = Enabled.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_ModfIen_BfSet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                  const uint8_t bfValue);


/**
 * \brief 0 = Disabled.
 *        1 = Enabled.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_ModfIen_BfGet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                  uint8_t* const bfValue);



/**
 * \brief This bit inhibits master transactions.
 *        This bit has no effect on slave operation.
 *        When set to:
 *        0 = Master transactions enabled.
 *        1 = Master transactions disabled.
 *        Note: This bit immediately inhibits the transaction. Setting this
 *        bit while transfer is in progress would result in unpredictable
 *        outcome.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_MstrInhibit_BfSet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                      const uint8_t bfValue);


/**
 * \brief This bit inhibits master transactions.
 *        This bit has no effect on slave operation.
 *        When set to:
 *        0 = Master transactions enabled.
 *        1 = Master transactions disabled.
 *        Note: This bit immediately inhibits the transaction. Setting this
 *        bit while transfer is in progress would result in unpredictable
 *        outcome.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_MstrInhibit_BfGet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                      uint8_t* const bfValue);



/**
 * \brief The number of slaves configured in the SPI Master.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_NumSlaves_BfGet(adi_fpgagen6_Device_t* const device,
                                                                    const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                    uint32_t* const bfValue);



/**
 * \brief Soft reset for the core engine.  To issue a reset, write 0x0000000A.  Any other value will result in an error.  Automatically clears.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_Reset_BfSet(adi_fpgagen6_Device_t* const device,
                                                                const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                const uint32_t bfValue);



/**
 * \brief Data read from the RX Data FIFO.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_RxData_BfGet(adi_fpgagen6_Device_t* const device,
                                                                 const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                 uint8_t* const bfValue);



/**
 * \brief This bit is set high when the receive FIFO is empty.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_RxEmpty_BfGet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                  uint8_t* const bfValue);



/**
 * \brief This field indicates the number of elements that are currently in the receive FIFO.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_RxFifoOccupancy_BfGet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                          uint16_t* const bfValue);



/**
 * \brief When written to 1, this bit forces a reset of the receive FIFO
 *        to the empty condition. One AXI clock cycle after reset, this
 *        bit is again set to 0.
 *        When set to:
 *        0 = Receive FIFO normal operation.
 *        1 = Reset receive FIFO pointer.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_RxFifoReset_BfSet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                      const uint8_t bfValue);



/**
 * \brief This bit is set high when the receive FIFO is full.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_RxFull_BfGet(adi_fpgagen6_Device_t* const device,
                                                                 const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                 uint8_t* const bfValue);



/**
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_Scratch_BfSet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                  const uint32_t bfValue);


/**
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_Scratch_BfGet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                  uint32_t* const bfValue);



/**
 * \brief Active-Low, one-hot encoded slave select vector.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_SlaveSelect_BfSet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                      const uint32_t bfValue);


/**
 * \brief Active-Low, one-hot encoded slave select vector.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_SlaveSelect_BfGet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                      uint32_t* const bfValue);



/**
 * \brief Setting this bit to 1 enables the SPI devices as noted here.
 *        When set to:
 *        • 0 = SPI system disabled. Master outputs are in 3-state and inputs are ignored.
 *        • 1 = SPI system enabled. Master outputs active (for example, IO0 (MOSI) and SCK in idle state). The master starts transferring when transmit data is available.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_Spe_BfSet(adi_fpgagen6_Device_t* const device,
                                                              const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                              const uint8_t bfValue);


/**
 * \brief Setting this bit to 1 enables the SPI devices as noted here.
 *        When set to:
 *        • 0 = SPI system disabled. Master outputs are in 3-state and inputs are ignored.
 *        • 1 = SPI system enabled. Master outputs active (for example, IO0 (MOSI) and SCK in idle state). The master starts transferring when transmit data is available.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_Spe_BfGet(adi_fpgagen6_Device_t* const device,
                                                              const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                              uint8_t* const bfValue);



/**
 * \brief When this status is 1, the transaction is currently active.  the status will clear when the TX FIFO is empty and the last byte is shifted out.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_TransActive_BfGet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                      uint8_t* const bfValue);



/**
 * \brief Data to be pushed into the TX Data FIFO.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_TxData_BfSet(adi_fpgagen6_Device_t* const device,
                                                                 const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                 const uint8_t bfValue);



/**
 * \brief This bit is set high when the transmit FIFO is empty.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_TxEmpty_BfGet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                  uint8_t* const bfValue);



/**
 * \brief In standard SPI configuration, IPISR Bit[6] is the transmit
 *        FIFO half-empty interrupt. For example, when FIFO depth =
 *        256, this bit is set by a one-clock period strobe to the
 *        interrupt register when the occupancy value is
 *        decremented from 10000000 to 01111111. Note that 01111111 means there are 128 elements in the FIFO to be transmitted. In this
 *        mode, the FIFO depth is fixed to 256 only.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_TxFifoHalfEmpty_BfSet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                          const uint8_t bfValue);


/**
 * \brief In standard SPI configuration, IPISR Bit[6] is the transmit
 *        FIFO half-empty interrupt. For example, when FIFO depth =
 *        256, this bit is set by a one-clock period strobe to the
 *        interrupt register when the occupancy value is
 *        decremented from 10000000 to 01111111. Note that 01111111 means there are 128 elements in the FIFO to be transmitted. In this
 *        mode, the FIFO depth is fixed to 256 only.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_TxFifoHalfEmpty_BfGet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                          uint8_t* const bfValue);



/**
 * \brief 0 = Disabled.
 *        1 = Enabled.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_TxFifoHalfEmptyIen_BfSet(adi_fpgagen6_Device_t* const device,
                                                                             const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                             const uint8_t bfValue);


/**
 * \brief 0 = Disabled.
 *        1 = Enabled.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_TxFifoHalfEmptyIen_BfGet(adi_fpgagen6_Device_t* const device,
                                                                             const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                             uint8_t* const bfValue);



/**
 * \brief This field indicates the number of elements that are currently in the transmit FIFO.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_TxFifoOccupancy_BfGet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                          uint16_t* const bfValue);



/**
 * \brief When written to 1, this bit forces a reset of the transmit
 *        FIFO to the empty condition. One AXI clock cycle after
 *        reset, this bit is again set to 0.
 *        When set to:
 *        0 = Transmit FIFO normal operation.
 *        1 = Reset transmit FIFO pointer.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_TxFifoReset_BfSet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                      const uint8_t bfValue);



/**
 * \brief This bit is set high when the transmit FIFO is full.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_TxFull_BfGet(adi_fpgagen6_Device_t* const device,
                                                                 const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                 uint8_t* const bfValue);



/**
 * \brief This is a local parameter in HDL and follows a MAJOR.MINOR.PATCH format, integers only.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdvSpiChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdvSpi_Version_BfGet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfAxiAdvSpiChanAddr_e baseAddr,
                                                                  uint32_t* const bfValue);
#endif // _FPGAGEN6_BF_AXI_ADV_SPI_H_

