// SPDX-License-Identifier: GPL-2.0
/**
 * \file AD917x.h
 *
 * \brief AD917X API interface header file
 *
 * This file contains all the publicly exposed methods and data structures to
 * interface with the AD917X API.
 *
 * Release 1.1.X
 *
 * Copyright(c) 2017 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 * By using this software you agree to the terms of the associated
 * Analog Devices Software License Agreement.
 */

#ifndef __AD917XAPI_H__
#define __AD917XAPI_H__

#include "api_def.h"
#include "no_os_util.h"

/** \addtogroup API
 *  @{
 */

/** DDS Select */
typedef enum {
	/** Main DDS */
	AD917X_DDSM = 0,
	/** Channel DDS */
	AD917X_DDSC = 1
} ad917x_dds_select_t;

/** DAC Select */
typedef enum {
	/** No DAC */
	AD917X_DAC_NONE = 0,
	/** DAC0 */
	AD917X_DAC0 = 1,
	/** DAC1 */
	AD917X_DAC1 = 2
} ad917x_dac_select_t;

/** Channel select */
typedef enum {
	/** No Channel */
	AD917X_CH_NONE = 0,
	/** Channel 0 */
	AD917X_CH_0 = NO_OS_BIT(0),
	/** Channel 1 */
	AD917X_CH_1 = NO_OS_BIT(1),
	/** Channel 2 */
	AD917X_CH_2 = NO_OS_BIT(2),
	/** Channel 3 */
	AD917X_CH_3 = NO_OS_BIT(3),
	/** Channel 4 */
	AD917X_CH_4 = NO_OS_BIT(4),
	/** Channel 5 */
	AD917X_CH_5 = NO_OS_BIT(5)
} ad917x_channel_select_t;

/** JESD Interface Link Status */
typedef struct {
	/** Bit wise Code Group Sync Status for all JESD Lanes*/
	uint8_t code_grp_sync_stat;
	/** Bit wise Frame Sync Status for all JESD Lanes*/
	uint8_t frame_sync_stat;
	/** Bit wise Good Checksum Status for all JESD Lanes*/
	uint8_t good_checksum_stat;
	/** Bit wise Initial Lane Sync Status for all JESD Lanes*/
	uint8_t init_lane_sync_stat;
} ad917x_jesd_link_stat_t;

/** Enumerates SERDES PLL Status Flags*/
typedef enum {
	AD917X_PLL_LOCK_STAT = 0x1, /**< Serdes PLL lock Status Flag*/
	AD917X_PLL_REG_RDY = 0x2,   /**< Serdes PLL Regulator RDY Status Flag*/
	AD917X_PLL_CAL_STAT = 0x4,  /**< Serdes PLL VCO Calibration Status Flag*/
	AD917X_PLL_LOSSLOCK = 0x8  /**< Serdes PLL Upper Calibration Threshold flag*/
} ad917x_jesd_serdes_pll_flg_t;

/** AD917X API handle */
typedef struct {
	void *user_data;        /**< Void pointer to user defined data for HAL initialization */
	spi_sdo_config_t sdo;      /**< DAC SPI interface configuration*/
	signal_type_t syncoutb;    /**< Desired Signal type for SYNCOUTB signal*/
	signal_coupling_t sysref;  /**< Desired Input coupling for sysref signal*/
	uint64_t dac_freq_hz;   /**< DAC Clock Frequency in Hz. Valid range 2.9GHz to 12GHz*/
	spi_xfer_t dev_xfer;    /**< Function Pointer to HAL SPI access function*/
	delay_us_t delay_us;    /**< Function Pointer to HAL delay function*/
	tx_en_pin_ctrl_t
	tx_en_pin_ctrl; /**< Function Pointer to HAL TX_ENABLE Pin Ctrl function*/
	reset_pin_ctrl_t
	reset_pin_ctrl; /**< Function Point to HAL RESETB Pin Ctrl Function*/
	hw_open_t hw_open;      /**< Function Pointer to HAL initialization function*/
	hw_close_t
	hw_close;    /**< Function Pointer to HAL de-initialization function*/
} ad917x_handle_t;

/**
 * \brief Initialize AD917X Device
 * This API must be called first before any other API calls.
 * It performs internal API initialization of the memory and API states.
 * If AD917X handle member hw_open is not NULL it shall call the function
 * to which it points. This feature may be used to get and initialize the
 * hardware resources required by the API and the AD917X devices.
 * For example GPIO, SPI etc.
 *
 * Its is recommended to call the Reset API after this API to ensure all
 * spi registers are reset to ADI recommended defaults.
 *
 * \param h Pointer to the AD917X device reference handle.
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_INVALID_HANDLE_PTR  Invalid reference handle
 * \retval API_ERROR_INIT_SEQ_FAIL Init sequence failed
 * \retval API_ERROR_SPI_SDO    Invalid SPI configuration
 */
int32_t ad917x_init(ad917x_handle_t *h);

/**
 * \brief De-initialize the AD917X Device.
 *
 * This API must be called last. No other API should be called after this call.
 * It performs internal API de-initialization of the memory and API states.
 * If AD917X handle member hw_close, is not NULL it shall call the function
 * to which it points. This feature may be used to de-initialize and release
 * any hardware resources required by the API and the AD917X Device.
 * For example GPIO, SPI etc
 *
 * \param h Pointer to the AD917X device reference handle.
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR   Invalid reference handle
 */
int32_t ad917x_deinit(ad917x_handle_t *h);

/**
 * \brief Reset the AD917X
 *
 * Issues a hard reset or soft reset of the device.
 * Performs a full reset of AD917X via the hardware pin (hard) or
 * via the spi register (soft).
 * Resetting all SPI registers to default and triggering the required
 * initialization sequence.
 *
 * \param h         Pointer to the AD917X device reference handle.
 * \param hw_reset  A parameter to indicate if the reset issues is to be via the
 *                  hardware pin or SPI register.
 *                  A value of 1 indicates a hardware reset is required.
 *                  A value of 0 indicates a software reset is required.
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR   Invalid reference handle
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 * \retval API_ERROR_INIT_SEQ_FAIL Init sequence failed
 * \retval API_ERROR_SPI_SDO    Invalid SPI configuration
 */
int32_t ad917x_reset(ad917x_handle_t *h, uint8_t hw_reset);

/**
 * \brief Get Chip Identification Data
 *
 * read-back Product type, identification and revision data
 *
 * \param h          Pointer to the AD917X device reference handle.
 * \param chip_id    Pointer to a variable of type  ad917x_chip_id_t to
 *                   which the product data read-back from chip shall be stored.
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR Invalid reference handle.
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int32_t ad917x_get_chip_id(ad917x_handle_t *h, adi_chip_id_t *chip_id);


/**
 * \brief Perform SPI register write access to AD917X Device
 *
 * \param h        Pointer to the AD917X device reference handle.
 * \param address  AD917X Device SPI address to which the value of data
 *                 parameter shall be written
 * \param data     8-bit value to be written to SPI register defined
 *                 by the address parameter.
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int32_t ad917x_register_write(ad917x_handle_t *h,
			      const uint16_t address, const uint8_t data);
/**
 * \brief Perform SPI register read access to AD917X Device.
 *
 *
 * \param h        Pointer to the AD917X device reference handle.
 * \param address  AD917X Device SPI address from which the value of data
 *                 parameter shall be read,
 * \param data     Pointer to an 8-bit variable to which the value of the
 *                 SPI register at the address defined by address parameter
 *                 shall be stored.
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int32_t ad917x_register_read(ad917x_handle_t *h,
			     const uint16_t address, uint8_t *data);
/**
 * \brief Get API Revision Data
 *
 * \param h             Pointer to the AD917X device reference handle.
 * \param rev_major     Pointer to variable to store the Major Revision Number
 * \param rev_minor     Pointer to variable to store the Minor Revision Number
 * \param rev_rc        Pointer to variable to store the RC Revision Number
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int32_t ad917x_get_revision(ad917x_handle_t *h, uint8_t *rev_major,
			    uint8_t *rev_minor, uint8_t *rev_rc);


/**
 * \brief Configure the On Chip DAC PLL
 *
 * The AD917X may be configured to use a clock directly applied to the device
 * as the DAC clock or may generate a DAC Clock using the clock applied
 * by the system as a reference.
 * This API allows direct configuration of the On Chip PLL parameters.
 *
 * \param h            Pointer to the AD917X device reference handle.
 * \param dac_pll_en   Enable for internal DAC Clock generation.
 *                     If set, ref_clk_freq_khz must be set with
 *                     value of reference clock applied by the system.
 *                     0 - Do not generate DAC CLK internally.
 *                     1 - Generate DAC CLK internally
 * \param m_div        Reference Clock Pre-divider. Where
 *                     M_DIVIDER = Ceiling (Fref_clk_mhz/500 MHz)
 *                     Valid Range 1 to 4
 * \param n_div        VCO Feedback Divider Ratio. Where
 *                     N_DIVIDER = Fvco * M_DIVIDER/(8 * Fref_clk)
 *                     Valid Range 2 -50
 * \param vco_div      Required VCO Divider for the Desired DAC CLK, where
 *                     Fdac = Fvco/VCO_DIVIDER
 *                     Valid range 1-3
 *
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int32_t ad917x_set_dac_pll_config(ad917x_handle_t *h, uint8_t dac_pll_en,
				  uint8_t m_div, uint8_t n_div, uint8_t vco_div);
/**
 * \brief Set the DAC CLK Frequency
 *
 * The AD917X may be configured to use a clock directly applied to the device
 * as the DAC clock or may generate a DAC Clock using the clock applied
 * by the system as a reference.
 * This API set the desired DAC Clock Frequency, irrespective of how the
 * DAC CLK is generated.
 * DAC Clock Frequency range is 2.9 GHz to 12GHz
 * for AD9172 and AD9173. DAC CLK Frequency range is 2.9 GHz to 6GHz for AD9171.
 *
 * This function shall be used in conjunction with the following API
 * ad917x_set_dac_pll_cfg
 *
 * \param h                Pointer to the AD917X device reference handle.
 * \param dac_clk_freq_hz  Desired DAC CLK Frequency value in Hz
 *
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int32_t ad917x_set_dac_clk_freq(ad917x_handle_t *h,
				uint64_t dac_clk_freq_hz);

/**
 * \brief Get the DAC CLK Frequency
 *
 * The AD917X may be configured to use a clock directly applied to the device
 * as the DAC clock or may generate a DAC Clock using the clock applied
 * by the system as a reference.
 * This API get the desired DAC Clock Frequency, irrespective of how the
 * DAC CLK is generated.
 * DAC Clock Frequency range is 2.9 GHz to 12GHz
 * for AD9172 and AD9173. DAC CLK Frequency range is 2.9 GHz to 6GHz for AD9171.
 *
 * \param h                Pointer to the AD917X device reference handle.
 * \param dac_clk_freq_hz  Pointer to the uint64_t variable in which the dac
 *                         clk frequency value in Hz shall be stored.
 *                         See description for valid range
 *
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int32_t ad917x_get_dac_clk_freq(ad917x_handle_t *h,
				uint64_t *dac_clk_freq_hz);
/**
 * \brief Get DAC CLK Status
 *
 *
 * \param h              Pointer to the AD917X device reference handle.
 * \param pll_lock_stat  Pointer to which DAC PLL Lock Status shall be stored.
 * 			 Set to NULL if status data not required.
 *                       0 - DAC PLL Not Locked.
 *                       1-  DAC PLL Locked.
 * \param dll_lock_stat  Pointer to which DAC DLL Lock Status Shall be stored.
 * 			 Set to NULL if status data not required.
 *                       0 - DAC PLL Not Locked.
 *                       1-  DAC PLL Locked.
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int32_t ad917x_get_dac_clk_status(ad917x_handle_t *h,
				  uint8_t *pll_lock_stat, uint8_t *dll_lock_stat);

/**
 * \brief Set CLKOUT configuration
 *
 *
 * \param h      Pointer to the AD917X device reference handle.
 * \param l_div  Output CLK divider setting. Valid range 1 to 4
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int32_t ad917x_set_clkout_config(ad917x_handle_t *h, uint8_t l_div);

/**
 * \brief Configure the DAC Clock Input path based on a the desired dac clock
 * frequency, the applied reference clock and the onchip PLL
 *
 * The AD917X may be configured to use a clock directly applied to the device
 * as the DAC clock or may generate a DAC Clock using the clock applied
 * by the system as a reference.
 * This function shall calculate and apply the required onchip PLL configuration
 * based on the desired dac clock frequency and the applied referenc clock
 * frequency.
 *
 * This function may be used instead of the following two APIs
 * ad917x_set_dac_pll_cfg
 * DAC Clock Frequency range is 2.9 GHz to 12GHz
 * for AD9172 and AD9173. DAC CLK Frequency range is 2.9 GHz to 6GHz for AD9171.
 *
 * \param h              Pointer to the AD917X device reference handle.
 * \param dac_clk_freq_hz   Desired DAC Clk Frequency in Hz. See description for range.
 * \param dac_pll_en    Enable for internal DAC Clock generation.
 *                       If set, ref_clk_freq_khz must be set with
 *                       value of reference clock applied by the system.
 *                       0 - Do not generate DAC CLK internally.
 *                       Clock applied to DAC is the dac clock.
 *                       1 - Generate DAC CLK internally from a reference clock
 *                       applied to the DAC
 * \param ref_clk_freq_hz   Value of reference clk frequency applied to AD917X
 *                          Set to 0 if DAC CLK is applied to the pin.
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int32_t ad917x_set_dac_clk(ad917x_handle_t *h,
			   uint64_t dac_clk_freq_hz, uint8_t dac_pll_en, uint64_t ref_clk_freq_hz);

/**
 * \brief Configure the JESD Datapath for AD917X
 *
 * Configure JESD interface parameters and digital datapath interpolation mode.
 * An error shall be returned if the input parameters define an
 * unsupported configuration. Refer to AD917x Device Datasheet for full details.
 *
 *
 * \param h           Pointer to the AD917X device reference handle.
 * \param dual_en     Dual Link enable setting
 *                    0 - Single Link Mode
 *                    1 - Dual Link Mode
 * \param jesd_mode   The desired value of the pre-definded JESD link modes
 *                    supported by the AD917X. Valid range 0 to 21.
 *                    Based on this value the AD917X JESD interface
 *                    shall be configured as per one of the supported JESD
 *                    parameter configurations.
 *                    Refer to the user guide for full details on the modes
 *                    and the corresponding JESD settings.
 * \param ch_intpl    The desired channel interpolation. Valid range 1 to 12
 * \param dp_intpl  The desired main dac datapath interpolation. Valid range 1 to 10
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR Invalid reference handle.
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int32_t ad917x_jesd_config_datapath(ad917x_handle_t *h, uint8_t dual_en,
				    uint8_t jesd_mode, uint8_t ch_intpl, uint8_t dp_intpl);

/**
 * \brief Get JESD Configuration Status
 *
 * Returns JESD Configuration Valid Mode Status
 *
 * \param h              Pointer to the AD917X device reference handle.
 * \param cfg_valid      Pointer to a variable in which the Valid JESD
 *                       Configuration status shall be stored
 *                       0 - Invalid JESD and Interpolation Mode Configured
 *                       1 - Valid JESD and Interpolation Mode Configured.
 *
 *
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR Invalid reference handle.
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int32_t ad917x_jesd_get_cfg_status(ad917x_handle_t *h, uint8_t *cfg_valid);

/**
 * \brief Read back all current JESD parameter settings.
 *
 * Read-back all the currently configured JESD Interface parameters.
 *
 * \param h           Pointer to the AD917X device reference handle.
 * \param jesd_param  Pointer to a variable that will be set will the
 *                    current values of the JESD interface parameters.
 *
 *
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR Invalid reference handle.
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int32_t ad917x_jesd_get_cfg_param(ad917x_handle_t *h,
				  jesd_param_t *jesd_param);

/**
 * \brief Enable SysRef Input
 *
 * Enable AD917X SYSREF +- Pin Input Interface for the target system SYSREF signal
 *
 * \param h            Pointer to the AD917X device reference handle.
 * \param en           Enable SYSREF Input Interface
 *                     1 - Power Up SYSREF Input
 *                     0 - Power Down SYSREF Input
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR Invalid reference handle.
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int32_t ad917x_jesd_set_sysref_enable(ad917x_handle_t *h, uint8_t en);
/**
 * \brief Get the current SYSREF Input
 *
 * Configure AD917X SYSREF +- Pin Input Interface for the target
 * system SYSREF signal
 *
 * \param h            Pointer to the AD917X device reference handle.
 * \param *en          Pointer to variable to which SYSREF Input Interface
 *                     Enable status shall be stored
 *                     1 - Power Up SYSREF Input
 *                     0 - Power Down SYSREF Input
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR Invalid reference handle.
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int32_t ad917x_jesd_get_sysref_enable(ad917x_handle_t *h, uint8_t *en);


/**
 * \brief Set the LMFC Delay and Variance for the JESD Links
 *
 * API to configure the LMFC Delay and Variance for JESD Link Setup.
 *
 * \param h     Pointer to the AD917X device reference handle.
 * \param link  Target JESD Link on which to configure LMFC Parameters
 * \param delay Dynamic Link Latency for LMFC Rx in PCLK cycles. Range 0 to 63
 * \param var   Variable Delay Buffer in PCLK cycles. Range 0 to 12
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR Invalid reference handle.
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int32_t ad917x_jesd_set_lmfc_delay(ad917x_handle_t *h, jesd_link_t link,
				   uint8_t delay, uint8_t var);
/**
 * \brief Enable the SYNCOUTB Output Signal
 *
 * Configure and enable/disable the SYNCOUT_B Output Signal
 *
 * \param h            Pointer to the AD917X device reference handle.
 * \param syncoutb     Target SYNCOUTB Signal.
 *                     Valid values defined by ad917x_syncoutb_t
 *                     SYNCOUTB_0
 *                     SYNCOUTB_1
 *                     SYCNOUTB_ALL
 * \param en           Enable/Disable SYNCOUTB
 *                     for target SYNCOUTB signal. Range 0 to 1
 *                     0 - Disable
 *                     1 - Enable
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR Invalid reference handle.
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int32_t ad917x_jesd_set_syncoutb_enable(ad917x_handle_t *h,
					jesd_syncoutb_t syncoutb, uint8_t en);

/**
 * \brief Enable the de-scrambler for the JESD Interface
 *
 * Enable or Disable the descrambler for the JESD Interface
 *
 * \param h   Pointer to the ad917x device reference handle.
 * \param en  Enable control for JESD Scrambler.
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR Invalid reference handle.
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int32_t ad917x_jesd_set_scrambler_enable(ad917x_handle_t *h, uint8_t en);

/**
 * \brief Configure the Lane Cross Bar in the JESD datalink layer
 *
 * Configure AD917X Lane Cross Bar to route the physical JESD lanes
 * to the desired logical lanes.
 *
 * \param h              Pointer to the AD917X device reference handle.
 * \param logical_lane   uint8_t value indicating the corresponding logical
 *                       lane for the physical lane listed
 *                       in parameter physical_lane.
 * \param physical_lane  uint8_t value indicating the Physical Lane
 *                       to be routed to the serdes logical indicated
 *                       by the logical_lane parameter
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR Invalid reference handle.
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int32_t ad917x_jesd_set_lane_xbar(ad917x_handle_t *h,
				  uint8_t logical_lane, uint8_t physical_lane);

/**
 * \brief Get current Lane Cross Bar configuration for the JESD datalink layer
 *
 * Return the physical to logical lane mapping set by the configured by the
 * current Lane Cross Bar configuration.
 *
 * \param h             Pointer to the AD917X device reference handle.
 * \param phy_log_map   Pointer a 8 deep uint8_t array.Each element of the array
 *                      represents the physical lane 0 - 7 and
 *                      the value represents the logical lane assigned to
 *                      that physical lane.
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR Invalid reference handle.
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int32_t ad917x_jesd_get_lane_xbar(ad917x_handle_t *h, uint8_t *phy_log_map);

/**
 * \brief Invert or un-invert logical lanes
 *
 * Each logical lane can be inverted which can be used to ease
 * routing of SERDIN signals.
 *
 * \param h             Pointer to the AD917X device reference handle.
 * \param logical_lane  Logical lane ID to be inverted. 0 to 7
 * \param invert        Desired invert status for the logical lane
 *                      represented in logical_lane parameter.
 *                      Set to 1 to invert.
 *                      Set to 0 to de-invert.
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR Invalid reference handle.
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int32_t ad917x_jesd_invert_lane(ad917x_handle_t *h,
				uint8_t logical_lane, uint8_t invert);
/**
 * \brief Enable the JESD Interface
 *
 * Configure power up and enable the ad917x the JESD Interface
 *
 * \param h         Pointer to the AD917X device reference handle.
 * \param lanes_msk Lanes to be enabled on JESD Interface.
 *                  8-bit mask where bit 0 represents lane 0,
 *                  bit 1 represents lane 1 etc.
 *                  Set to 1 to enable JESD lane, set to 0 to disable JESD Lane.
 * \param run_cal   Run JESD interface equalisation routine
 * \param en        Enable control for the JESD Interface
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR Invalid reference handle.
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int32_t ad917x_jesd_enable_datapath(ad917x_handle_t *h,
				    uint8_t lanes_msk, uint8_t run_cal, uint8_t en);
/**
 * \brief  Get SERDES PLL Status
 *
 * Read Serdes PLL status and return the status via the
 * pll_status parameter.
 *
 *
 * \param h            Pointer to the AD917X device reference handle.
 * \param *pll_status  Pointer to the variable that will be set with
 *                     the PLL status.
 *                     bit[0] => SERDES PLL Lock Status
 *                     bit[1] => Regulator Status
 *                     bit[2] => Calibration Status
 *                     bit[3] => LOSS_LOCK Status
 *
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR Invalid reference handle.
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int32_t ad917x_jesd_get_pll_status(ad917x_handle_t *h, uint8_t *pll_status);

/**
 * \brief  Enable JESD Link
 *
 * Enable SERDES Link to start the bring up JESD Link procedure
 *
 * JESD Transmitter Link shall be enabled and ready to begin link bring
 * prior to calling this function. SERDES PLL shall be locked.
 *
 * \param h     Pointer to the AD917X device reference handle.
 * \param link  Target Link on which to start JESD Link Bring up Procedure
 * \param en    Enable control for the JESD Link
 *              0 - Enable Link
 *              1 - Disable Link
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR Invalid reference handle.
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int32_t ad917x_jesd_enable_link(ad917x_handle_t *h,
				jesd_link_t link, uint8_t en);

/**
 * \brief  Get JESD Link Status
 *
 * Read-back JESD Status for all lanes. JESD status include
 * Code Group Sync Status, Frame Sync Status, Checksum Status
 * Initial Lane Sync Status for the active JESD link.
 *
 *
 * \param h             Pointer to the AD917x device reference handle.
 * \param link          Desired link of which to retrieve status.
 * \param *link_status  Pointer to the variable of type jesd_link_status
 *                      that will be set with current jesd link read-back data.
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR Invalid reference handle.
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int32_t ad917x_jesd_get_link_status(ad917x_handle_t *h,
				    jesd_link_t link, ad917x_jesd_link_stat_t *link_status);

/**
 * \brief  Select Page
 *
 * \param h             Pointer to the AD916x device reference handle.
 * \param dac           DAC number. Valid values are:
 *						AD9172_DAC_NONE - No DAC selected
 *						AD9172_DAC0 - DAC0 selected
 *						AD9172_DAC1 - DAC1 selected
 *						AD9172_DAC0 | AD9172_DAC1 - Both DACs selected
 * \param channel       Channel number.Valid values are:
 *						AD9172_CH_NONE - No channel selected
 *						AD9172_CH_0 - Channel 0 selected
 *						...
 *						AD9172_CH_5 - Channel 5 selected
 *						More than one channel can be selected at once by ORing them.
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR Invalid reference handle.
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int32_t ad917x_set_page_idx(ad917x_handle_t *h, const uint32_t dac,
			    const uint32_t channel);

/**
 * \brief  Get select page index
 *
 * \param h             Pointer to the AD917x device reference handle.
 * \param *dac          Pointer to the selected DAC number.
 * \param *channel      Pointer to the selected Channel number.
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR Invalid reference handle.
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int32_t ad917x_get_page_idx(ad917x_handle_t *h, int32_t *dac, int32_t *channel);

/**
 * \brief  Set Channel gain
 *
 * Sets the scalar channel gain value. It is paged by CHANNEL_PAGE in Reg08
 *
 * \param h     Pointer to the AD917x device reference handle.
 * \param gain  Gain value
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR Invalid reference handle.
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int32_t ad917x_set_channel_gain(ad917x_handle_t *h, const uint16_t gain);

/**
 * \brief  Get Channel gain
 *
 * Gets the scalar channel gain value. It is paged by CHANNEL_PAGE in Reg08
 *
 * \param h     Pointer to the AD917x device reference handle.
* \param gain  Pointer to the gain value
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR Invalid reference handle.
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int32_t ad917x_get_channel_gain(ad917x_handle_t *h, uint16_t *gain);


/**
 * \brief  Set DC Calibration tone
 *
 * Sets the DC tone amplitude. This amplitude goes to both I and Q paths.
 * It is paged by CHANNEL_PAGE in Reg08
 *
 * \param h     Pointer to the AD917x device reference handle.
* \param amp    Calibration tone amplitude
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR Invalid reference handle.
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int32_t ad917x_set_dc_cal_tone_amp(ad917x_handle_t *h, const uint16_t amp);

/**
 * \brief  Get NCO phase offset
 *
 * Gets main datapath and/or channel datapath NCO phase offset.
 *
 * \param h     Pointer to the AD917x device reference handle.
 * \param dacs  DAC number
 *              AD917X_DAC0 - DAC0 NCO
 *              AD917X_DAC1 - DAC1 NCO
 *              AD917X_DAC0 | AD917X_DAC1 - DAC0 NCO and DAC1 NCO
 *
 * \param channels  Channel number
 *              AD917X_CH_0 - Channel 0 NCO
 *              AD917X_CH_1 - Channel 1 NCO
 *              AD917X_CH_2 - Channel 2 NCO
 *              AD917X_CH_3 - Channel 3 NCO
 *              AD917X_CH_4 - Channel 4 NCO
 *              AD917X_CH_5 - Channel 5 NCO
 * \param dacs_po  Phase offset for the selected DAC NCO(s)
 * \param ch_po  Phase offset for the selected channel NCO(s)
 *
* \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR Invalid reference handle.
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int32_t ad917x_nco_get_phase_offset(ad917x_handle_t *h,
				    const ad917x_dac_select_t dacs, uint16_t *dacs_po,
				    const ad917x_channel_select_t channels, uint16_t *ch_po);

/**
 * \brief  Set NCO phase offset
 *
 * Sets main datapath and/or channel datapath NCO phase offset.
 *
 * \param h     Pointer to the AD917x device reference handle.
 * \param dacs  DAC number
 *              AD917X_DAC0 - DAC0 NCO
 *              AD917X_DAC1 - DAC1 NCO
 *
 * \param channels  Channel number
 *              AD917X_CH_0 - Channel 0 NCO
 *              AD917X_CH_1 - Channel 1 NCO
 *              AD917X_CH_2 - Channel 2 NCO
 *              AD917X_CH_3 - Channel 3 NCO
 *              AD917X_CH_4 - Channel 4 NCO
 *              AD917X_CH_5 - Channel 5 NCO
 * \param dacs_po  Pointer to the phase offset for the selected DAC NCO(s)
 * \param ch_po  Pointer to the phase offset for the selected channel NCO(s)
 *
* \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR Invalid reference handle.
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int32_t ad917x_nco_set_phase_offset(ad917x_handle_t *h,
				    const ad917x_dac_select_t dacs, const uint16_t dacs_po,
				    const ad917x_channel_select_t channels, const uint16_t ch_po);

/**
 * \brief  Set FTW, ACC and MOD values
 *
 * Set FTW, ACC and MOD values for the paged NCO. The page has to be selected in advance.
 *
 * \param h     Pointer to the AD917x device reference handle.
 * \param nco   Channel or Main data path select
 *              AD917X_DDSM - Main data path select
 *              AD917X_DDSC - Channel data path select
 * \param ftw  FTW value
 * \param acc_modulus  Modulus value
 * \param acc_delta  Delta value
 *
* \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR Invalid reference handle.
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int32_t ad917x_nco_set_ftw(ad917x_handle_t *h,
			   const ad917x_dds_select_t nco,
			   const uint64_t ftw,
			   const uint64_t acc_modulus,
			   const uint64_t acc_delta);

/**
 * \brief  Get FTW, ACC and MOD values
 *
 * Get FTW, ACC and MOD values for the paged NCO. The page has to be selected in advance.
 *
 * \param h     Pointer to the AD917x device reference handle.
 * \param nco   Channel or Main data path select
 *              AD917X_DDSM - Main data path select
 *              AD917X_DDSC - Channel data path select
 * \param ftw  Pointer to the FTW value, Set to NULL if data not required
 * \param acc_modulus  Pointer to the Modulus value, Set to NULL if data not required
 * \param acc_delta  Pointer to the Delta value, Set to NULL if data not required
 *
* \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR Invalid reference handle.
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int32_t ad917x_nco_get_ftw(ad917x_handle_t *h,
			   const ad917x_dds_select_t nco,
			   uint64_t *ftw,
			   uint64_t *acc_modulus,
			   uint64_t *acc_delta);

/**
 * \brief  Set NCO
 *
 * Set NCO to produce a desired frequency with a desired amplitude
 *
 * \param h     Pointer to the AD917x device reference handle.
 * \param dacs   Main data path DAC NCO select
 *              AD917X_DAC0 - DAC0 NCO select
 *              AD917X_DAC1 - DAC1 NCO select
 * \param channels  Channel number
 *              AD917X_CH_0 - Channel 0 NCO
 *              AD917X_CH_1 - Channel 1 NCO
 *              AD917X_CH_2 - Channel 2 NCO
 *              AD917X_CH_3 - Channel 3 NCO
 *              AD917X_CH_4 - Channel 4 NCO
 *              AD917X_CH_5 - Channel 5 NCO
 * \param carrier_freq_hz  Desired carrier frequency in Hz
 * \param amplitude  Desired amplitude value
 * \param dc_test_tone_en - enable test tone
 * \param ddsm_cal_dc_input_en - enable main datapath test tone
 *
* \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR Invalid reference handle.
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int32_t ad917x_nco_set(ad917x_handle_t *h,
		       const ad917x_dac_select_t dacs,
		       const ad917x_channel_select_t channels,
		       const int64_t carrier_freq_hz,
		       const uint16_t amplitude,
		       int32_t dc_test_tone_en,
		       int32_t ddsm_cal_dc_input_en);

/**
 * \brief  NCO Enable
 *
 * Enable/Disable NCOs. Enables only the DACs and Channel NCOs provided as
 * parameters. All other DACs and Channel NCOs are disabled.
 *
 * \param h     Pointer to the AD917x device reference handle.
 * \param dacs   Main data path DAC NCO select
 *              AD917X_DAC0 - DAC0 NCO select
 *              AD917X_DAC1 - DAC1 NCO select
 * \param channels  Channel number
 *              AD917X_CH_0 - Channel 0 NCO
 *              AD917X_CH_1 - Channel 1 NCO
 *              AD917X_CH_2 - Channel 2 NCO
 *              AD917X_CH_3 - Channel 3 NCO
 *              AD917X_CH_4 - Channel 4 NCO
 *              AD917X_CH_5 - Channel 5 NCO
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR Invalid reference handle.
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int32_t ad917x_nco_enable(ad917x_handle_t *h,
			  const ad917x_dac_select_t dacs,
			  const ad917x_channel_select_t channels);

/**
 * \brief  Set Main DAC Cal DC Input
 *
 * Set Main DAC Cal DC Input
 *
 * \param h     Pointer to the AD917x device reference handle.
 * \param ddsm_cal_dc_input_en  Enable flag:
 *				0 - Disabled
 *				1 - Enabled

 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR Invalid reference handle.
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int32_t ad917x_ddsm_cal_dc_input_set(ad917x_handle_t *h,
				     int32_t ddsm_cal_dc_input_en);

/**
 * \brief  Get Main DAC Cal DC Input
 *
 * Get Main DAC Cal DC Input
 *
 * \param h     Pointer to the AD917x device reference handle.
 * \param ddsm_cal_dc_input_en  Pointer to integer, where the result will be storred
 *              0 - Disabled
 *              1 - Enabled

 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR Invalid reference handle.
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int32_t ad917x_ddsm_cal_dc_input_get(ad917x_handle_t *h,
				     int32_t *ddsm_cal_dc_input_en);

/**
 * \brief  Set DC Test Tone enable status
 *
 * Set DC Test Tone enable status.
 *
 * \param h     Pointer to the AD917x device reference handle.
 * \param dc_test_tone_en  Enable flag:
 *				0 - Disabled
 *				1 - Enabled
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR Invalid reference handle.
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int32_t ad917x_dc_test_tone_set(ad917x_handle_t *h, int32_t dc_test_tone_en);

/**
 * \brief  Get DC Test Tone enable status
 *
 * Get DC Test Tone enable status.
 *
 * \param h     Pointer to the AD917x device reference handle.
 * \param dc_test_tone_en  Pointer to integer, where the result will be storred
 *              0 - Disabled
 *              1 - Enabled
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR Invalid reference handle.
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int32_t ad917x_dc_test_tone_get(ad917x_handle_t *h, int32_t *dc_test_tone_en);

/**
 * \brief  Get Channel NCO frequency.
 *
 * Get a Channel NCO frequency in Hz.
 *
 * \param h     Pointer to the AD917x device reference handle.
 * \param channel  Channel number. Can be only one of:
 *              AD917X_CH_0 - Channel 0 NCO
 *              AD917X_CH_1 - Channel 1 NCO
 *              AD917X_CH_2 - Channel 2 NCO
 *              AD917X_CH_3 - Channel 3 NCO
 *              AD917X_CH_4 - Channel 4 NCO
 *              AD917X_CH_5 - Channel 5 NCO
 *
 * \param carrier_freq_hz - pointer to 64 bit integer, where the
 * result frequency will be storred
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR Invalid reference handle.
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int32_t ad917x_nco_channel_freq_get(ad917x_handle_t *h,
				    ad917x_channel_select_t channel,
				    int64_t *carrier_freq_hz);

/**
 * \brief  Get Main DAC NCO frequency.
 *
 * Get a Main DAC NCO frequency in Hz.
 *
 * \param h     Pointer to the AD917x device reference handle.
 * \param dac   Main data path DAC NCO select. Can be only one of:
 *              AD917X_DAC0 - DAC0 NCO select
 *              AD917X_DAC1 - DAC1 NCO select
 * \param carrier_freq_hz  pointer to 64 bit integer, where the
 * result frequency will be storred
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR Invalid reference handle.
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int32_t ad917x_nco_main_freq_get(ad917x_handle_t *h,
				 ad917x_dac_select_t dac,
				 int64_t *carrier_freq_hz);

/** @} */

#endif /* !__AD917XAPI_H__ */
