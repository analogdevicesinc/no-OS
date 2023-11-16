/**
 * Copyright 2015 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information.
 * see the "LICENSE.pdf" file in this zip file.
 */

/**
* \file adi_fmc_eeprom.c
*
* \brief Definitions for ADI FMC EEPROM
*
* ADRV904X API Version: 2.9.0.4
*/

#include "adi_fmc_eeprom.h"

#ifdef __GNUC__
#include <unistd.h>
#include <getopt.h>
#include <sys/stat.h>
#include <strings.h>
#else
#ifdef _WIN64
#define ssize_t __int64
#else
#define ssize_t long
#endif

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2
#define strncasecmp(x,y,z) _strnicmp(x,y,z)
#endif

#include <fcntl.h>

#include "adi_platform_types.h"
#include "adi_fmc_fru.h"
#include "adi_library.h"


#define DUMP_BOARD     (0x01)
#define DUMP_SUPPLY    (0x02)
#define DUMP_CONNECTOR (0x04)
#define DUMP_I2C       (0x08)

#define EEPROM_DATA_SIZE            1024U
#define EEPROM_DATA_ELEMENT_SIZE    1

static adi_hal_Err_e platform_FileRead(adi_hal_EepromCfg_t* const eepromCfg, uint8_t streamData[EEPROM_DATA_SIZE]);

static adi_hal_Err_e platform_FileRead(adi_hal_EepromCfg_t* const eepromCfg, uint8_t streamData[EEPROM_DATA_SIZE])
{
    adi_hal_Err_e halError = ADI_HAL_ERR_NULL_PTR;
    size_t byteRead = 0U;
    void* streamDataPtr = streamData;

    if (eepromCfg == NULL)
    {
        fru_PrintError("ERROR: Invalid EEPROM Config Pointer for EEPROM Read\n");
        return halError;
    }

    if (streamData == NULL)
    {
        fru_PrintError("ERROR: Invalid File Pointer for EEPROM Read\n");
        return halError;
    }

    halError = ADI_HAL_ERR_PARAM;

    if (0U != eepromCfg->interfaceEnabled)
    {
        eepromCfg->fd = ADI_LIBRARY_FOPEN(eepromCfg->drvName, "rb");
        if (eepromCfg->fd == NULL)
        {
            fru_PrintError("ERROR: Cannot open EEPROM file '%s'\n", eepromCfg->drvName);
            return halError;
        }

        halError = ADI_HAL_ERR_I2C;

        byteRead = ADI_LIBRARY_FREAD(streamDataPtr, EEPROM_DATA_ELEMENT_SIZE, EEPROM_DATA_SIZE, eepromCfg->fd);
        if (byteRead > 0U)
        {
            if (byteRead < 256U)
            {
                (void) ADI_LIBRARY_PRINTF("ERROR: Didnt Read Entire EEPROM File %s\n", streamData);
                goto cleanup;
            }

            if (!feof(eepromCfg->fd))
            {
	            (void) ADI_LIBRARY_PRINTF("ERROR: End of EEPROM File Indicator is Set %s\n", streamData);
                goto cleanup;
            }


            if (byteRead >= 256)
            {
                halError = ADI_HAL_ERR_OK;
            }
        }
    }
cleanup:
    if (0 != ADI_LIBRARY_FCLOSE(eepromCfg->fd))
    {
        halError = ADI_HAL_ERR_MEMORY;
        fru_PrintError("ERROR: Cannot Close EEPROM I2C Driver\n");
        return halError;
    }
    
    return halError;
}

ADI_API adi_hal_Err_e adi_platform_FmcEepromRead(void* const devHalCfg, adi_hal_BoardInfo_t* const boardInfo)
{
    adi_hal_Err_e halError = ADI_HAL_ERR_NULL_PTR;
    adi_hal_Cfg_t* halCfg = NULL;
    uint8_t streamData[EEPROM_DATA_SIZE] = { 0 };
    FRU_DATA_t* fmcFru = NULL;
    uint32_t i = 0U;
    int32_t bytesFormatted = 0U;

    if (NULL == devHalCfg)
    {
        fru_PrintError("ERROR: Invalid Device HAL Pointer for FMC EEPROM Read\n");
        return halError;
    }

    if (boardInfo == NULL)
    {
        fru_PrintError("ERROR: Invalid HAL Board Information Pointer for FMC EEPROM Read\n");
        return halError;
    }
    
    ADI_LIBRARY_MEMSET(boardInfo, 0, sizeof(adi_hal_BoardInfo_t));

    halError = ADI_HAL_ERR_PARAM;

    /* Assume ADI HAL Structure */
    halCfg = (adi_hal_Cfg_t*) devHalCfg;

    if (0U != halCfg->eepromCfg.interfaceEnabled)
    {

        halError = platform_FileRead(&halCfg->eepromCfg, streamData);
        if (halError != ADI_HAL_ERR_OK)
        {
            fru_PrintError("ERROR: Platform File Read Issue for FMC EEPROM Read\n");
            return halError;
        }

        halError = fru_FruParse(streamData, &fmcFru);
        if (halError != ADI_HAL_ERR_OK)
        {
            fru_PrintError("ERROR: Parsing FMC FRU Issue for FMC EEPROM Read\n");
            return halError;
        }

        if (fmcFru == NULL)
        {
            /* Double Check that Fru Structure Has Been Provided */
            fru_PrintError("ERROR: Invalid FRU Structure for FMC EEPROM Read\n");
            halError = ADI_HAL_ERR_EEPROM_DATA;
        }
        else
        {
            halError = ADI_HAL_ERR_MEMORY;

            /* Board Area Info */

            /* Manufacturer */
            bytesFormatted = ADI_LIBRARY_SNPRINTF((char*)boardInfo->manufacturer, ADI_HAL_STRING_LENGTH, "%s", (fmcFru->Board_Area->manufacturer + 1));
            if ((bytesFormatted < 1) ||
                (bytesFormatted >= (int32_t) ADI_HAL_STRING_LENGTH))
            {
                return halError;
            }

            /* Product Name */
            bytesFormatted = ADI_LIBRARY_SNPRINTF((char*)boardInfo->productName, ADI_HAL_STRING_LENGTH, "%s", (fmcFru->Board_Area->product_name + 1));
            if ((bytesFormatted < 1) ||
                (bytesFormatted >= (int32_t) ADI_HAL_STRING_LENGTH))
            {
                return halError;
            }

            /* Serial Number */
            bytesFormatted = ADI_LIBRARY_SNPRINTF((char*)boardInfo->serialNumber, ADI_HAL_STRING_LENGTH, "%s", (fmcFru->Board_Area->serial_number + 1));
            if ((bytesFormatted < 1) ||
                (bytesFormatted >= (int32_t) ADI_HAL_STRING_LENGTH))
            {
                return halError;
            }

            /* Part Number */
            bytesFormatted = ADI_LIBRARY_SNPRINTF((char*)boardInfo->partNumber, ADI_HAL_STRING_LENGTH, "%s", (fmcFru->Board_Area->part_number + 1));
            if ((bytesFormatted < 1) ||
                (bytesFormatted >= (int32_t) ADI_HAL_STRING_LENGTH))
            {
                return halError;
            }

            if (!strncasecmp((const char*)&fmcFru->Board_Area->manufacturer[1], "Analog Devices", ADI_LIBRARY_STRLEN("Analog Devices")))
            {
                /* Custom Manufacturing Information Fields; Each Field is preceded by a type/length Byte */

                /* PCB Name Details */
                bytesFormatted = ADI_LIBRARY_SNPRINTF((char*)boardInfo->pcbName, ADI_HAL_STRING_LENGTH, "%s", (fmcFru->Board_Area->custom[1] + 2));
                if ((bytesFormatted < 1) ||
                (bytesFormatted >= (int32_t) ADI_HAL_STRING_LENGTH))
                {
                    return halError;
                }

                for (i = 0U; i < CUSTOM_FIELDS; ++i)
                {
                    /* These are ADI custom fields */
                    if (fmcFru->Board_Area->custom[i] && (fmcFru->Board_Area->custom[i][0] & 0x3FU))
                    {
                        switch (fmcFru->Board_Area->custom[i][1])
                        {
                            case 0:
                                /* fru_FruFieldDump("PCB Rev ", 1, fru->custom[i]); */
                                break;

                            case 1:
                                bytesFormatted = ADI_LIBRARY_SNPRINTF((char*)boardInfo->pcbId, ADI_HAL_STRING_LENGTH, "%s", (fmcFru->Board_Area->custom[i]));
                                if ((bytesFormatted < 1) ||
                                    (bytesFormatted >= (int32_t) ADI_HAL_STRING_LENGTH))
                                {
                                    return halError;
                                }
                                break;

                            case 2:
                                bytesFormatted = ADI_LIBRARY_SNPRINTF((char*)boardInfo->bomRev, ADI_HAL_STRING_LENGTH, "%s", (fmcFru->Board_Area->custom[i]));
                                if ((bytesFormatted < 1) ||
                                    (bytesFormatted >= (int32_t) ADI_HAL_STRING_LENGTH))
                                {
                                    return halError;
                                }
                                else
                                {
                                    /* Set OK when confirmed an Analog Devices Part & all FRU Information, including Custom Fields has been extracted */
                                    halError = ADI_HAL_ERR_OK;
                                }
                                break;

                            case 3:
                                /* fru_FruFieldDump("Uses LVDS", 1, fru->custom[i]); */
                                break;

                            default:
                                /* fru_FruFieldDump("Unknown ", 1, fru->custom[i]); */
                                break;
                        }
                    }
                }
            }
        }
    }

    fru_FruFree(fmcFru);

    return halError;
}
