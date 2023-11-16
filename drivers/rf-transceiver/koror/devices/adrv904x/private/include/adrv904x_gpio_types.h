/**
 * Copyright 2015 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.pdf" file in this zip file.
 */

/**
 * \file adrv904x_gpio_types.h
 * \brief Contains ADRV904X GPIO related private data prototypes for
 *        adrv904x_gpio.c
 *
 * ADRV904X API Version: 2.9.0.4
 */

#ifndef _ADRV904X_GPIO_TYPES_H_
#define _ADRV904X_GPIO_TYPES_H_

#include "../../public/include/adi_adrv904x_gpio_types.h"
#include "adrv904x_shared_resource_manager_types.h"



#define ADRV904X_GP_INT_TYPE_DEFAULT_LOWER (uint64_t)(0x0000007FFFFFULL)
#define ADRV904X_GP_INT_TYPE_DEFAULT_UPPER (uint64_t)(0x3007FFFFFFFFULL)

#define ADRV904X_GPIO_PINMUX_STAGE3_MAX                         15U
#define ADRV904X_GPIO_PINMUX_STAGE2_RX_MAX                      7U
#define ADRV904X_GPIO_PINMUX_STAGE2_TX_MAX                      7U
#define ADRV904X_GPIO_PINMUX_STAGE2_ORX_MAX                     2U
#define ADRV904X_GPIO_PINMUX_STAGE2_ACTRL_MAX                   511U
#define ADRV904X_GPIO_PINMUX_STAGE1_RX_MAX                      118U
#define ADRV904X_GPIO_PINMUX_STAGE1_TX_MAX                      53U
#define ADRV904X_GPIO_PINMUX_STAGE1_ORX_MAX                     15U
#define ADRV904X_GPIO_ANALOG_PINMUX_MAX                         15U
#define ADRV904X_GPIO_NUM_DESTINATIONS_RX                       7U
#define ADRV904X_GPIO_NUM_DESTINATIONS_TX                       6U

#define ADRV904X_DIGITAL_PIN_GROUP_NUM 8U
#define ADRV904X_DIGITAL_PIN_PER_GROUP_NUM 6U

/**
 *  \brief Enum for ADRV904X GPIO Domain Type: Digital vs Analog
 */
typedef enum adrv904x_GpioDomain
{
    ADRV904X_GPIO_DOMAIN_NONE,
    ADRV904X_GPIO_DOMAIN_DIGITAL,
    ADRV904X_GPIO_DOMAIN_ANALOG
} adrv904x_GpioDomain_e;


/**
 *  \brief Enum for ADRV904X GPIO Route Type
 */
typedef enum adrv904x_GpioRoute
{
    ADRV904X_GPIO_ROUTE_OFF                     = 0U,   /*!< Routing Off. This Route Type is only for feature UNUSED */
    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE         = 1U,   /*!< Digital Pinmux Route using Pinmux Stg3 */
    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL        = 2U,   /*!< Digital Pinmux Route using Pinmux Stg2,3 */
    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX           = 3U,   /*!< Digital Pinmux Route using Pinmux Stg1,2,3 */
    ADRV904X_GPIO_ROUTE_DIG_PINMUX_TX           = 4U,   /*!< Digital Pinmux Route using Pinmux Stg1,2,3 */
    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ORX          = 5U,   /*!< Digital Pinmux Route using Pinmux Stg1,2,3 */
    ADRV904X_GPIO_ROUTE_DIG_DEST_STREAM_TRIG    = 6U,   /*!< Digital Destination Route to Dig Core for Triggering Streams with GPIOs*/
    ADRV904X_GPIO_ROUTE_DIG_DEST_PPI            = 7U,   /*!< Digital Destination Route to Dig Core for PPI16 Bus */
    ADRV904X_GPIO_ROUTE_DIG_DEST_RX             = 9U,   /*!< Digital Destination Route to Rx using Stg3 selected source 0 and Rx channel gpio_select */
    ADRV904X_GPIO_ROUTE_DIG_DEST_TX             = 10U,  /*!< Digital Destination Route to Tx using Stg3 selected source 0 and Tx channel gpio_select */
    ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE         = 12U,  /*!< Analog Pinmux Route using simple Analog Pinmux */
    ADRV904X_GPIO_ROUTE_ANA_DEST_CORE           = 13U,  /*!< NOT SUPPORTED: Analog Destination Route to Core using Ana IE Override */
    ADRV904X_GPIO_ROUTE_ANA_DEST_RX             = 14U,  /*!< NOT SUPPORTED: Analog Destination Route to RX using Ana IE Override */
    ADRV904X_GPIO_ROUTE_ANA_DEST_TX             = 15U,  /*!< NOT SUPPORTED: Analog Destination Route to TX using Ana IE Override */
    ADRV904X_GPIO_ROUTE_ANA_DEST_ORX            = 16U,  /*!< NOT SUPPORTED: Analog Destination Route to ORX using Ana IE Override */		
    ADRV904X_GPIO_ROUTE_DIG_DEST_RADIO_SEQ      = 17U,  /*!< Digital Destination Route to Radio Sequencer */
    ADRV904X_GPIO_ROUTE_DIG_DEST_RX_CDDC_RSSI   = 18U,  /*!< Digital Destination Route to RX CDDC_RSSI */
    ADRV904X_GPIO_ROUTE_DIG_DEST_TX_DTX         = 19U,  /*!< Digital Destination Route to Tx using Stg3 selected source 0 and Tx channel gpio_select, specifically for Koror DTX pin functionality*/
} adrv904x_GpioRoute_e;


/**
 *  \brief Struct for ADRV904X GPIO Signal Information. Used in LUT to obtain
            signal-specific routing details
 */
typedef struct adrv904x_GpioSignalInfo
{
    adi_adrv904x_GpioSignal_e   signal;         /*!< The internal signal to be exposed via GPIO */
    adrv904x_GpioDomain_e       domain;         /*!< Analog or Digital */
    adrv904x_GpioRoute_e        route;          /*!< Indicates the set of pin-muxes involved in routing the signal to a GPIO */
    uint32_t                    pinMask;        /*!< Indicates the GPIO pins to which routing is valid. LSB for GPIO0 etc. */
    uint8_t                     topSelect;
    int16_t                     targetSelect;
} adrv904x_GpioSignalInfo_t;


#endif /* ! _ADRV904X_GPIO_TYPES_H_ */
