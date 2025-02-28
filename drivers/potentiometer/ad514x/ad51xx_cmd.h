/***************************************************************************//**
 *   @file   ad51xx_cmd.h
 *   @brief  Header file for the AD51xx digital potentiometer drivers commands.
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
*******************************************************************************/
#ifndef AD51XX_CMD_H_
#define AD51XX_CMD_H_
/*
 *   Command format of the Digipot family AD51xx
 *   C[3-1] ==> Command Bits
 *   A[3-1] ==> address Bits
 *   D[7-1] ==> data Bits
 *  +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 *  |  C3 | C2  |  C1 | C0  |  A3 | A2  |  A1 | A0  |  D7 | D6  |  D5 | D4  |  D3 | D2  |  D1 | D0  |
 *  +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 */
/* Bit mask and position for the command bits */
#define CMD_BITS_POSITION 12
#define CMD_BITS_MASK     0xF000

#define ADS_BITS_POSITION  8
#define ADS_BITS_MASK      0x0F00


#define DATA_BITS_POSITION 0
#define DATA_BITS_MASK     0x00FF

#define OPERATING_MODE_BIT_MASK 0XFB
#define OPERATING_MODE_BIT_POSITION 0x02


#define EXTRACT_CMD_BITS(x) (x&CMD_BITS_MASK)>>CMD_BITS_POSITION
#define EXTRACT_ADS_BITS(x) (x&ADS_BITS_MASK)>>ADS_BITS_POSITION
#define EXTRACT_DATA_BITS(x) (x&DATA_BITS_MASK)>>DATA_BITS_POSITION

#define AD51XX_CMD_SW_RESET                 (0x0B<<CMD_BITS_POSITION|0x00)

#define AD51XX_CMD_WRITE_RDAC_REG           (0x01<<CMD_BITS_POSITION|0x00)
#define AD51XX_CMD_WRITE_INP_REG            (0x02<<CMD_BITS_POSITION|0x00)

#define AD51XX_CMD_READ_INP_REG             (0x03<<CMD_BITS_POSITION|0x00)
#define AD51XX_CMD_READ_EEPROM              (0x03<<CMD_BITS_POSITION|0x01)
#define AD51XX_CMD_READ_CTL_REG             (0x03<<CMD_BITS_POSITION|0x02)
#define AD51XX_CMD_READ_RDAC_REG            (0x03<<CMD_BITS_POSITION|0x03)

#define AD51XX_CMD_LRDAC_INC                (0x04<<CMD_BITS_POSITION|0x01)
#define AD51XX_CMD_LRDAC_DEC                (0x04<<CMD_BITS_POSITION|0x00)
#define AD51XX_CMD_LRDAC_6DB_INC            (0x05<<CMD_BITS_POSITION|0x01)
#define AD51XX_CMD_LRDAC_6DB_DEC            (0x05<<CMD_BITS_POSITION|0x00)
#define AD51XX_CMD_COPY_INP_REG_TO_RDAC     (0x06<<CMD_BITS_POSITION|0x00)
#define AD51XX_CMD_COPY_RDAC_TO_EEPROM      (0x07<<CMD_BITS_POSITION|0x01)
#define AD51XX_CMD_COPY_EEPROM_TO_RDAC      (0x07<<CMD_BITS_POSITION|0x00)

#define AD51XX_CMD_TOP_SCALE_ENTER          (0x09<<CMD_BITS_POSITION|0x81)
#define AD51XX_CMD_TOP_SCALE_EXIT           (0x09<<CMD_BITS_POSITION|0x80)

#define AD51XX_CMD_BOTTOM_SCALE_ENTER       (0x09<<CMD_BITS_POSITION|0x01)
#define AD51XX_CMD_BOTTOM_SCALE_EXIT        (0x09<<CMD_BITS_POSITION|0x00)


#define AD51XX_CMD_ENTER_SHUTDOWN           (0x0C<<CMD_BITS_POSITION|0x01)
#define AD51XX_CMD_ENTER_NORMAL_MODE        (0x0C<<CMD_BITS_POSITION|0x00)

#define AD51XX_CMD_WRITE_TO_EEPROM          (0x08<<CMD_BITS_POSITION|0x00)
#define AD51XX_CMD_WRITE_SERIAL_REG_TO_CTL  (0x0D<<CMD_BITS_POSITION|0x00)

#endif	// AD51XX_CMD_H_
