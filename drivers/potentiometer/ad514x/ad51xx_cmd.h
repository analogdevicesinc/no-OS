/*******************************************************************************
 *   @file   ad51xx_cmd.h
 *   @brief  Header file for the AD51xx digital potentiometer drivers commands.
********************************************************************************
Copyright 2025(c) Analog Devices, Inc.

SPDX-License-Identifier: BSD-3-Clause
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
