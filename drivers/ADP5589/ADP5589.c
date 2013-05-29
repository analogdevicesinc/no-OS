/***************************************************************************//**
 *   @file   ADP5589.c
 *   @brief  Implementation of ADP5589 Driver.
 *   @author Mihai Bancisor (Mihai.Bancisor@analog.com)
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
********************************************************************************
 *   SVN Revision: $WCREV$
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "ADP5589.h"			// ADP5589 definitions.
#include "Communication.h"		// Communication definitions.

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/***************************************************************************//**
 * @brief Writes data into a register.
 *
 * @param registerAddress - Address of the register.
 * @param registerValue   - Data value to write.
 *
 * @return None.
*******************************************************************************/
void ADP5589_SetRegisterValue(unsigned char registerAddress,
                              unsigned char registerValue)
{
    static unsigned char writeData[2] = {0, 0};

    writeData[0] = registerAddress;
    writeData[1] = registerValue;
    I2C_Write(ADP5589_ADDRESS, writeData, 2, 1);
}

/***************************************************************************//**
 * @brief Reads the value of a register.
 *
 * @param registerAddress - Address of the register.
 *
 * @return registerValue  - Value of the register.
*******************************************************************************/
unsigned char ADP5589_GetRegisterValue(unsigned char registerAddress)
{
    static unsigned char readData[2]   = {0, 0};
    static unsigned char registerValue = 0;
    
    readData[0] = registerAddress;
    I2C_Write(ADP5589_ADDRESS, readData, 1, 0);
    I2C_Read(ADP5589_ADDRESS, readData, 1, 1);
    registerValue = readData[0];
    
    return registerValue;
}

/***************************************************************************//**
 * @brief Initializes the communication peripheral and checks if the ADP5589
 *		  part is present.
 *
 * @return status - Result of the initialization procedure.
 *                  Example: -1 - I2C peripheral was not initialized or
 *                                ADP5589 part is not present.
 *                            0 - I2C peripheral is initialized and ADP5589
 *                                part is present.
*******************************************************************************/
char ADP5589_Init(void)
{
    static unsigned char status = 0;
    
    status = I2C_Init(100000);
    if((ADP5589_GetRegisterValue(ADP5589_ADR_ID) & ADP5589_ID_MAN_ID) != 
    ADP5589_ID)
    {
        status = -1;
    }
    /* Enable internal oscillator and set clock frequency to 500 kHz. */
    ADP5589_SetRegisterValue(ADP5589_ADR_GENERAL_CFG_B, 
                             ADP5589_GENERAL_CFG_B_OSC_EN| 
                             ADP5589_GENERAL_CFG_B_CORE_FREQ(3));
                                                     
    return status;
}

/***************************************************************************//**
 * @brief Initializes the PWM generator in continuous mode.
 *
 * @return None.
*******************************************************************************/
void ADP5589_InitPwm(void)
{
    ADP5589_SetRegisterValue(ADP5589_ADR_PWM_CFG, 
	         	     ADP5589_PWM_CFG_PWM_EN);
    ADP5589_SetRegisterValue(ADP5589_ADR_PIN_CONFIG_D, 
	                     ADP5589_PIN_CONFIG_D_R3_EXTEND(2));
}

/***************************************************************************//**
 * @brief Initializes keyboard decoder.
 * @param pmodPort  - J1 or J2 connector of PmodIOXP
 *			Example: PMOD_IOXP_J1 - J1 connector.
 *			         PMOD_IOXP_J2 - J2 connector.
 * @return None.
*******************************************************************************/
void ADP5589_InitKey(unsigned char pmodPort)
{
    if(!pmodPort)
    {
        /* Config row 0,1,2,3 */
        ADP5589_SetRegisterValue(ADP5589_ADR_PIN_CONFIG_A,0x0F);
        /* Config column 0,1,2,3 */
        ADP5589_SetRegisterValue(ADP5589_ADR_PIN_CONFIG_B,0x0F);
    }
    else
    {
        /* Config row 4,5,6,7 */
        ADP5589_SetRegisterValue(ADP5589_ADR_PIN_CONFIG_A,0xF0);
        /* Config column 4,5,6,7 */
        ADP5589_SetRegisterValue(ADP5589_ADR_PIN_CONFIG_B,0xF0);		
    }
}

/***************************************************************************//**
 * @brief Sets the PWM On and Off times.
 *
 * @param pwmOffTime - The amount of time in uS for which the PWM pulse is LOW.
 * @param pwmONTime  - The amount of time in uS for which the PWM pulse is HIGH.
 *
 * @return None.
*******************************************************************************/
void ADP5589_SetPwm(unsigned short pwmOffTime, unsigned short pwmOnTime)
{
    unsigned char data = 0;
	
    data = (unsigned char)(pwmOffTime >> 8);
    ADP5589_SetRegisterValue(ADP5589_ADR_PWM_OFFT_HIGH, data);
    data = (unsigned char)(pwmOffTime & 0xFF);
    ADP5589_SetRegisterValue(ADP5589_ADR_PWM_OFFT_LOW, data);
    data = (unsigned char)(pwmOnTime & 0xFF);
    ADP5589_SetRegisterValue(ADP5589_ADR_PWM_ONT_LOW, data);
    data = (unsigned char)(pwmOnTime >> 8);
    ADP5589_SetRegisterValue(ADP5589_ADR_PWM_ONT_HIGH, data);
}
/***************************************************************************//**
 * @brief Sets the direction of the pins.
 *
 * @param reg  - The address of the direction register to be written.
 * @param val  - The data to be written to the direction register.
 *
 * @return None.
*******************************************************************************/
void ADP5589_GpioDirection(unsigned char reg, unsigned char val)
{
    ADP5589_SetRegisterValue(reg, val);
}
/***************************************************************************//**
 * @brief Reads the state of the pins.
 *
 * @param reg  - The address of the status register to be read.
 *
 * @return val - The value of the register.
*******************************************************************************/
unsigned char ADP5589_GetPinState(unsigned char reg)
{
    unsigned char val = 0;
	
    val = ADP5589_GetRegisterValue(reg);
	
    return val;
}

/***************************************************************************//**
 * @brief Sets the state of the pins.
 *
 * @param reg    - The address of the status register to be read.
 * @param state  - The value to be written to the pins.
 *
 * @return none
*******************************************************************************/
void ADP5589_SetPinState(unsigned char reg, unsigned char state)
{
    ADP5589_SetRegisterValue(reg,state);
}

/***************************************************************************//**
 * @brief Decodes the key on the Pmod-KYPD.
 *
 * @param reg  	    - The value of the FIFO register
 * @param pmodPort  - J1 or J2 connector of PmodIOXP
 *			Example: PMOD_IOXP_J1 - J1 connector.
 *                               PMOD_IOXP_J2 - J2 connector.
 * @param eventType - Pressed or released.
 *                     Example: ADP5589_EVENT_KEY_RELEASED
 *                              ADP5589_EVENT_KEY_PRESSED
 *
 * @return key      - Actual Key on the Pmod-KYPD.
*******************************************************************************/
unsigned char ADP5589_KeyDecode(unsigned char reg,
                                unsigned char eventType,
                                unsigned char pmodPort)
{
    unsigned char key = 0;
	
    reg -= 0x30 * pmodPort;
    if(eventType == ADP5589_EVENT_KEY_PRESSED)
    {
        reg -= 0x80;
    }
    switch(reg)
    {
        case 0x25:
            key = '1';
            break;
        case 0x24:
            key = '4';
            break;
        case 0x23:
            key = '7';
            break;
        case 0x1A:
            key = '2';
            break;
        case 0x19:
            key = '5';
            break;
        case 0x18:
            key = '8';
            break;
        case 0x0F:
            key = '3';
            break;
        case 0x0E:
            key = '6';
            break;
        case 0x0D:
            key = '9';
            break;
        case 0x04:
            key = 'A';
            break;
        case 0x22:
            key = '0';
            break;
        case 0x17:
            key = 'F';
            break;
        case 0x0C:
            key = 'E';
            break;
        case 0x01:
            key = 'D';
            break;
        case 0x02:
            key = 'C';
            break;
        case 0x03:
            key = 'B';
            break;
        default:
            key = 'x';
            break;
    }

    return key;
}

/***************************************************************************//**
 * @brief Locks the ADP5589 and requests Password for unlock.
 *
 * @param pmodPort  - J1 or J2 connector of PmodIOXP
 *			Example: PMOD_IOXP_J1 - J1 connector.
 *				 PMOD_IOXP_J2 - J2 connector.
 *
 * @return key      - Actual Key on the Pmod-KYPD.
*******************************************************************************/
void ADP5589_KeyLock(unsigned char firstEvent,
                     unsigned char secondEvent,
                     unsigned char pmodPort)
{
    unsigned char data = 0;
    
    ADP5589_SetRegisterValue(ADP5589_ADR_UNLOCK1,
            ADP5589_UNLOCK1_UNLOCK1_STATE | (firstEvent + 0x30 * pmodPort));
    ADP5589_SetRegisterValue(ADP5589_ADR_UNLOCK2,
            ADP5589_UNLOCK2_UNLOCK2_STATE | (secondEvent + 0x30 * pmodPort));
    ADP5589_SetRegisterValue(ADP5589_ADR_LOCK_CFG, ADP5589_LOCK_CFG_LOCK_EN);
    do
    {
        data = ADP5589_GetRegisterValue(ADP5589_ADR_STATUS);
    }
    while((data & ADP5589_STATUS_LOCK_STAT) != 0);
}
