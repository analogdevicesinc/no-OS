/**
* \file
* \brief Contains ADI common interface.
*
* ADI common lib Version: $ADI_COMMON_LIB_VERSION$
*/

/**
* Copyright 2015 - 2018 Analog Devices Inc.
* Released under the API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#include "adi_common.h"

int32_t adi_common_port_to_index(adi_common_Port_e port, uint8_t *index)
{
    if (NULL == index)
    {
        return -2;
    }
    
    switch (port)
    {
    case ADI_RX:  
        *index = 0; 
        break;
    case ADI_TX:  
        *index = 1;
        break;
    case ADI_ORX:
        *index = 2;
        break;
    case ADI_ILB: 
        *index = 3;
        break;
    case ADI_ELB: 
        *index = 4;
        break;
    default: return -2;
    }
    
    return 0;
}

int32_t adi_common_channel_to_index(adi_common_ChannelNumber_e channel, uint8_t *index)
{
    if (NULL == index)
    {
        return -2;
    }
    
    switch (channel)
    {
    case ADI_CHANNEL_1: 
        *index = 0;
        break;
    case ADI_CHANNEL_2: 
        *index = 1;
        break;
    default: return -2;
    }
    
    return 0;
}

int32_t adi_common_index_to_port(uint8_t index, adi_common_Port_e *port)
{
    if (NULL == port)
    {
        return -2;
    }
    
    switch (index)
    {
    case 0: 
        *port = ADI_RX;
        break;
    case 1: 
        *port = ADI_TX;
        break;
    case 2: 
        *port = ADI_ORX;
        break;
    case 3: 
        *port = ADI_ILB;
        break;
    case 4: 
        *port = ADI_ELB; 
        break;
    default: return -2;
    }
    
    return 0;
}

int32_t adi_common_index_to_channel(uint8_t index, adi_common_ChannelNumber_e *channel)
{
    if (NULL == channel)
    {
        return -2;
    }
    
    switch (index)
    {
    case 0: 
        *channel = ADI_CHANNEL_1;
        break;
    case 1: 
        *channel = ADI_CHANNEL_2;
        break;
    default: return -2;
    }
    
    return 0;
}
