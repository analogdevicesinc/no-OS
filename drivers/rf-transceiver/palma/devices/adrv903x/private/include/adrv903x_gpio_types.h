/**
 * Copyright 2015 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * \file adrv903x_gpio_types.h
 * \brief Contains ADRV903X GPIO related private data prototypes for
 *        adrv903x_gpio.c
 *
 * ADRV903X API Version: 2.12.1.4
 */

#ifndef _ADRV903X_GPIO_TYPES_H_
#define _ADRV903X_GPIO_TYPES_H_

#include "../../public/include/adi_adrv903x_gpio_types.h"
#include "adrv903x_shared_resource_manager_types.h"



#define ADRV903X_GP_INT_TYPE_DEFAULT_LOWER (uint64_t)(0x3FFFFFULL)
#define ADRV903X_GP_INT_TYPE_DEFAULT_UPPER (uint64_t)(0x7FFFFFFFFULL)

#define ADRV903X_GPIO_PINMUX_STAGE3_MAX                         15U
#define ADRV903X_GPIO_PINMUX_STAGE2_RX_MAX                      7U
#define ADRV903X_GPIO_PINMUX_STAGE2_TX_MAX                      7U
#define ADRV903X_GPIO_PINMUX_STAGE2_ORX_MAX                     2U
#define ADRV903X_GPIO_PINMUX_STAGE2_ACTRL_MAX                   511U
#define ADRV903X_GPIO_PINMUX_STAGE1_RX_MAX                      94U
#define ADRV903X_GPIO_PINMUX_STAGE1_TX_MAX                      15U
#define ADRV903X_GPIO_PINMUX_STAGE1_ORX_MAX                     15U
#define ADRV903X_GPIO_ANALOG_PINMUX_MAX                         15U
#define ADRV903X_GPIO_NUM_DESTINATIONS_RX                       7U
#define ADRV903X_GPIO_NUM_DESTINATIONS_TX                       6U


#define ADRV903X_DIGITAL_PIN_GROUP_NUM 8U
#define ADRV903X_DIGITAL_PIN_PER_GROUP_NUM 6U

/**
 *  \brief Enum for ADRV903X GPIO Domain Type: Digital vs Analog
 */
typedef enum adrv903x_GpioDomain
{
    ADRV903X_GPIO_DOMAIN_NONE,
    ADRV903X_GPIO_DOMAIN_DIGITAL,
    ADRV903X_GPIO_DOMAIN_ANALOG
} adrv903x_GpioDomain_e;


/**
 *  \brief Enum for ADRV903X GPIO Route Type
 */
typedef enum adrv903x_GpioRoute
{
    ADRV903X_GPIO_ROUTE_OFF                     = 0U,   /*!< Routing Off. This Route Type is only for feature UNUSED */
    ADRV903X_GPIO_ROUTE_DIG_PINMUX_CORE         = 1U,   /*!< Digital Pinmux Route using Pinmux Stg3 */
    ADRV903X_GPIO_ROUTE_DIG_PINMUX_ACTRL        = 2U,   /*!< Digital Pinmux Route using Pinmux Stg2,3 */
    ADRV903X_GPIO_ROUTE_DIG_PINMUX_RX           = 3U,   /*!< Digital Pinmux Route using Pinmux Stg1,2,3 */
    ADRV903X_GPIO_ROUTE_DIG_PINMUX_TX           = 4U,   /*!< Digital Pinmux Route using Pinmux Stg1,2,3 */
    ADRV903X_GPIO_ROUTE_DIG_PINMUX_ORX          = 5U,   /*!< Digital Pinmux Route using Pinmux Stg1,2,3 */
    ADRV903X_GPIO_ROUTE_DIG_DEST_STREAM_TRIG    = 6U,   /*!< Digital Destination Route to Dig Core for Triggering Streams with GPIOs*/
    ADRV903X_GPIO_ROUTE_DIG_DEST_PPI            = 7U,   /*!< Digital Destination Route to Dig Core for PPI16 Bus */
    ADRV903X_GPIO_ROUTE_DIG_DEST_RX             = 9U,   /*!< Digital Destination Route to Rx using Stg3 selected source 0 and Rx channel gpio_select */
    ADRV903X_GPIO_ROUTE_DIG_DEST_TX             = 10U,  /*!< Digital Destination Route to Tx using Stg3 selected source 0 and Tx channel gpio_select */
    ADRV903X_GPIO_ROUTE_ANA_PINMUX_CORE         = 12U,  /*!< Analog Pinmux Route using simple Analog Pinmux */
    ADRV903X_GPIO_ROUTE_ANA_DEST_CORE           = 13U,  /*!< NOT SUPPORTED: Analog Destination Route to Core using Ana IE Override */
    ADRV903X_GPIO_ROUTE_ANA_DEST_RX             = 14U,  /*!< NOT SUPPORTED: Analog Destination Route to RX using Ana IE Override */
    ADRV903X_GPIO_ROUTE_ANA_DEST_TX             = 15U,  /*!< NOT SUPPORTED: Analog Destination Route to TX using Ana IE Override */
    ADRV903X_GPIO_ROUTE_ANA_DEST_ORX            = 16U,  /*!< NOT SUPPORTED: Analog Destination Route to ORX using Ana IE Override */		
} adrv903x_GpioRoute_e;


/**
 *  \brief Struct for ADRV903X GPIO Signal Information. Used in LUT to obtain
            signal-specific routing details
 */
typedef struct adrv903x_GpioSignalInfo
{
    adi_adrv903x_GpioSignal_e   signal;         /*!< The internal signal to be exposed via GPIO */
    adrv903x_GpioDomain_e       domain;         /*!< Analog or Digital */
    adrv903x_GpioRoute_e        route;          /*!< Indicates the set of pin-muxes involved in routing the signal to a GPIO */
    uint32_t                    pinMask;        /*!< Indicates the GPIO pins to which routing is valid. LSB for GPIO0 etc. */
    uint8_t                     topSelect;
    int16_t                     targetSelect;
} adrv903x_GpioSignalInfo_t;


#endif /* ! _ADRV903X_GPIO_TYPES_H_ */
