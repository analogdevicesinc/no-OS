// SPDX-License-Identifier: GPL-2.0
/**
 * \file ad9208_api/api_def.h
 *
 * \brief API definitions header file.
 *
 * This file contains all common API definitions.
 *
 * Copyright(c) 2016 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 * By using this software you agree to the terms of the associated
 * Analog Devices Software License Agreement.
 */

#ifndef __API_DEF_H__
#define __API_DEF_H__
#include <stdint.h>
#include "no_os_util.h"

#define ADI_POW2_48 ((uint64_t)1u << 48)
#define ADI_MAXUINT48 (ADI_POW2_48 - 1)

#define ADI_POW2_32 ((uint64_t)1u << 32)
#define ADI_MAXUINT32 (ADI_POW2_32 - 1)
#define ADI_MAXUINT24 (0xFFFFFF)
#define ADI_GET_BYTE(w, p) (uint8_t)(((w) >> (p)) & 0xFF)
#define ALL -1

#define DIV_U64_REM(x, y, r) no_os_div64_u64_rem(x, y, r)
#ifndef NO_OS_DIV_U64
#define NO_OS_DIV_U64(x, y) no_os_div_u64(x, y)
#endif
#define DIV_S64(x, y) no_os_div_s64(x, y)

/**
 * \brief Platform dependent SPI access functions.
 *
 *
 * \param indata  Pointer to array with the data to be sent on the SPI
 * \param outdata Pointer to array where the data to which the SPI will be written
 * \param size_bytes The size in bytes allocated for each of the indata and outdata arrays.
 *
 * \retval 0 for success
 * \retval Any non-zero value indicates an error
 *
 * \note indata and outdata arrays are of same size.
 */
typedef int (*spi_xfer_t) (void *user_data, uint8_t *indata, uint8_t *outdata,
			   int size_bytes);

/**
 * \brief Delay for specified number of microseconds. Platform Dependant.
 *
 * Performs a blocking or sleep delay for the specified time in microseconds
 * The implementation of this function is platform dependant and
 * is required for correct operation of the API.
 *
 * \retval 0 for success
 * \retval Any non-zero value indicates an error
 *
 * \param us - time to delay/sleep in microseconds.
 */
typedef int (*delay_us_t) (void *user_data, unsigned int us);

/**
 * \brief Platform hardware initialisation for the AD9164 Device
 *
 * This function shall initialize all external hardware resources required by
 * the ADI Device and API for correct functionatlity as per the
 * target platform.
 * For example initialisation of SPI, GPIO resources, clocks etc.
 *
 *
 * \param *user_data - A void pointer to a client defined structure containing any
 *             parameters/settings that may be required by the function
 *             to initialise the hardware for the ADI Device.
 * \retval 0 for success
 * \retval Any non-zero value indicates an error
 *
 */
typedef int (*hw_open_t) (void *user_data);

/**
 * \brief Closes any platform hardware resources for the AD9164 Device
 *
 * This function shall close or shutdown all external hardware resources
 * required by the AD9164 Device and API for correct functionatlity
 * as per the target platform.
 * For example initialisation of SPI, GPIO resources, clocks etc.
 * It should close and free any resources assigned in the hw_open_t function.
 *
 * \param *user_data - A void pointer to a client defined structure containing any
 *				parameters/settings that may be required by the function
 *				to close/shutdown the hardware for the ADI Device.
 * \retval 0 for success
 * \retval Any non-zero value indicates an error
 *
 */
typedef int (*hw_close_t) (void *user_data);
/**
 * \brief Client  Event Handler
 *
 *
 * \param event	A uint16_t value representing the event that occurred.
 * \param ref   A uint8_t value indicating the reference for that event if any.
 *				For example 0 if even occured on lane 0.
 * \param data  A void pointer to any user data that may pertain to that event.
 *
 * \retval 0 for success
 * \retval Any non-zero value indicates an error
 *
 * \note
 */
typedef int (*event_handler_t) (uint16_t event, uint8_t ref, void *data);

/**
 * \brief TX_ENABLE PIN CONTROL FUNCTION
 *
 *
 * \param *user_data A void pointer to a client defined structure containing
 *					any parameters/settings that may be required by the function
 *					to control the hardware for the ADI Device TX_ENABLE PIN.
 * \param enable A uint8_t value indicating the desired enable/disable
 *					setting for the tx_enable pin.
 *					A value of 1 indicates TX_ENABLE pin is set HIGH
 *					A value of 0 indicates TX_ENABLE pin is set LOW
 *
 *
 * \retval 0 for success
 * \retval Any non-zero value indicates an error
 *
 * \note
 */
typedef int (*tx_en_pin_ctrl_t) (void *user_data, uint8_t enable);

/**
 * \brief RESETB PIN CONTROL FUNCTION
 *
 *
 * \param *user_data A void pointer to a client defined structure containing
 *					any parameters/settings that may be required by the function
 *					to control the hardware for the ADI Device RESETB PIN.
 * \param enable A uint8_t value indicating the desired enable/disable
 *					reset via the ADI device RESETB pin.
 *					A value of 1 indicates RESETB pin is set LOW
 *					A value of 0 indicates RESETB pin is set HIGH
 *
 * \retval 0 for success
 * \retval Any non-zero value indicates an error
 *
 * \note
 */
typedef int (*reset_pin_ctrl_t) (void *user_data, uint8_t enable);

/** API register access structure */
struct adi_reg_data {
	/** Register address */
	uint16_t reg;
	/** Register value */
	uint8_t val;
};

/** ADI Device Identification Data */
typedef struct {
	/** Chip Type Code */
	uint8_t chip_type;
	/** Product ID Code */
	uint16_t prod_id;
	/** Product Grade Code*/
	uint8_t prod_grade;
	/** Device Revision */
	uint8_t dev_revision;
} adi_chip_id_t;

/** SPI Interface Parameters. */
typedef enum {
	/* Keep this invalid value as 0, so the API can test for wrong setting. */
	SPI_NONE = 0,
	/*
	 * Set SPI SDO (Serial Data Output) pin as active.
	 * This is in case the 4-wire SPI is needed.
	 */
	SPI_SDO = 1,
	/*
	 * Set SPI SDIO (Serial Data Input-Output) pin as active.
	 * This is in case the 3-wire SPI is needed.
	 */
	SPI_SDIO = 2,
	/* Keep it last. */
	SPI_CONFIG_MAX = 3
} spi_sdo_config_t;

/** Enumerate Signal Types */
typedef enum {
	SIGNAL_CMOS = 0, /**CMOS SIGNAL TYPE */
	SIGNAL_LVDS,	 /**LVDS SIGNAL TYPE */
	SIGNAL_UNKNOWN	 /**UNKNOW/UNDEFINED SIGNAL TYPE */
} signal_type_t;

/** Enumerate coupling mode */
typedef enum {
	COUPLING_AC = 0,	 /**AC COUPLED SIGNAL */
	COUPLING_DC,		 /**DC SIGNAL  */
	COUPLING_UNKNOWN	 /**UNKNOWN/UNDEFINED COUPLING */
} signal_coupling_t;

/** Enumerates JESD LINK Signals */
typedef enum {
	JESD_LINK_0 = 0x0,     /**< JESD LINK 0 */
	JESD_LINK_1 = 0x1,     /**< JESD LINK 1 */
	JESD_LINK_ALL = 0xFF   /**< ALL JESD LINKS */
} jesd_link_t;

/** Enumerates SYNCOUTB Output Signals */
typedef enum {
	SYNCOUTB_0 = 0x0,     /**< SYNCOUTB 0 */
	SYNCOUTB_1 = 0x1,     /**< SYNCOUTB 1 */
	SYNCOUTB_ALL = 0xFF   /**< ALL SYNCOUTB SIGNALS */
} jesd_syncoutb_t;

/** Enumerates SYSREF Synchronization Mode */
typedef enum {
	SYSREF_NONE = 0x0,     /**< No SYSREF Support */
	SYSREF_ONESHOT,	 /**< ONE-SHOT SYSREF */
	SYSREF_CONT,	 /**< Continuous Sysref Synchronisation */
	SYSREF_MON,	 /**< SYSREF monitor Mode */
	SYSREF_MODE_INVLD
} jesd_sysref_mode_t;

/** JESD Interface Parameters. */
typedef struct {
	uint8_t jesd_L;	    /**< JESD Lane Param L. */
	uint8_t jesd_F;	    /**< JESD Octet Param F. */
	uint8_t jesd_M;	    /**< JESD Converter Param M. */
	uint8_t jesd_S;	    /**< JESD No of Sample Param S. */

	uint8_t jesd_HD;    /** JESD High Density Param HD.*/
	uint8_t jesd_K;	    /**< JESD multiframe Param K.  */
	uint8_t jesd_N;	    /**< JESD Converter Resolution Param N.  */
	uint8_t jesd_NP;    /**< JESD Bit Packing Sample NP. */
	uint8_t jesd_CF;    /**< JESD Param CF.  */
	uint8_t jesd_CS;    /**< JESD Param CS.  */

	uint8_t jesd_DID;   /**< JESD Device ID Param DID. */
	uint8_t jesd_BID;   /**< JESD Bank ID. Param BID */
	uint8_t jesd_LID0;  /**< JESD Lane ID for Lane 0 Param LIDO*/
	uint8_t jesd_JESDV; /**< JESD Version */
} jesd_param_t;

/** Enumerates all available Jesd Subclass Modes */
typedef enum {
	JESD_SUBCLASS_0,     /**< JESD SUBCLASS 0*/
	JESD_SUBCLASS_1,     /**< JESD SUBCLASS 1 */
	JESD_SUBCLASS_INVALID
} jesd_subclass_t;

/** Enumerates all available PRBS patterns */
typedef enum {
	PRBS_NONE, /**< PRBS OFF */
	PRBS7,	   /**< PRBS7 pattern */
	PRBS15,	   /**< PRBS15 pattern */
	PRBS31,	   /**< PRBS31 pattern */
	PRBS_MAX   /**< Number of member in this enum */
} jesd_prbs_pattern_t;

#endif /* !__API_DEF_H__ */
