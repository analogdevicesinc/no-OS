/*
 * adi_fmc_fru.c
 * Copyright (C) 2012 Analog Devices
 * Author : Robin Getz <robin.getz@analog.com>
 *
 * This file is maintained as part of:
 *    https://github.com/analogdevicesinc/fru_tools
 * but is released under this license, so you can use it without having 
 * your software fall under the GPL. If you make improvements to this,
 * although you are not required, it would be nice if you sent me a patch.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in
 *   the documentation and/or other materials provided with the
 *   distribution.
 * - Neither the name of Analog Devices, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 * - The use of this software may or may not infringe the patent rights
 *   of one or more patent holders.  This license does not release you
 *   from the requirement that you obtain separate licenses from these
 *   patent holders to use this software.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE
 */

#ifdef __GNUC__
#include <unistd.h>
#include <strings.h>
#else
typedef int ssize_t;
static __inline unsigned int
__bswap_32(unsigned int __bsx)
{
    return ((((__bsx) & 0xff000000) >> 24) | (((__bsx) & 0x00ff0000) >> 8) |
        (((__bsx) & 0x0000ff00) << 8) | (((__bsx) & 0x000000ff) << 24));
}
#define strncasecmp(x,y,z) _strnicmp(x,y,z)
#endif
#include <sys/stat.h>
#include <fcntl.h>

#include "adi_platform.h"
#include "adi_fmc_fru.h"


/*
 * This code is based from:
 * Platform Management FRU Information
 * Storage Definition
 * Document Revision 1.1, September 27, 1999
 * Available at zero cost at:
 * http://download.intel.com/design/servers/ipmi/FRU1011.pdf
 *
 * ANSI/VITA 57.1
 * FPGA Mezzanine Card (FMC) Standard
 * Approved July 2008 (Revised February 2010)
 * http://www.vita.com/fmc.html
 * Available for a small fee
 * https://vita.com/secure/online-store.html
 * used with permission
 * Most info is in section 5.5.1: IPMI Support
 */

#if CHAR_BIT != 8
#error "unsupported char size"
#endif

static const int quiet = 0;
static const int verbose = 0;

/**
* \brief Service for Zero Checksum Calculator of FRU Information
*
* FRU information uses a zero checksum everywhere.
* The modulo 256 sum of the preceding bytes (starting with the first byte
* of the header) plus the checksum byte equals zero.
* Platform Management FRU Information Storage Definition : section 16.2.[56]
*
* \param[in] data - Address of Data to be Checked
* \param[in] len - Size of Data to be checked
*
* \retval unsigned char - 0 if Checksum OK
*/
static unsigned char fru_ZeroCrcCalculate(const unsigned char* const data, const size_t len);

/**
* \brief Service for 6-bit ASCII Packing
*
* Platform Management FRU Information Storage Definition:  Section 13.[23]
*
* \param[out] dest  Pointer to Packed Data
* \param[in] src    Pointer to Data to be Packed
* \param[in] size   Size of Data to be Packed
*
* \retval int Size of Packed Data
*/
static int fru_Ascii2Six(unsigned char** const dest, unsigned char* const src, const size_t size);

/**
* \brief Service for Unpacking 6-bit ASCII
*
* Platform Management FRU Information Storage Definition:  Section 13.[23]
*
* \param[out] buf   Pointer to Packed Data
* \param[in] size   Size of Data to be Packed
*
* \retval unsigned char* Pointer to Unpacked Data (Dynamically Allocated)
*/
static unsigned char* fru_Six2Ascii(const unsigned char* const buf, size_t size);

/**
* \brief Service for Extract Strings from Fields
*
* Section 13 TYPE/LENGTH BYTE FORMAT
* Platform Management FRU Information Storage Definition
*
* \param[in] p
* \param[in] str
* \param[in] field
*
* \retval unsigned int
*/
static unsigned int fru_StringParse(const unsigned char* const p, unsigned char** str, const char* field);

/**
* \brief Service to Insert String into Buffer
*
* Section 13 TYPE/LENGTH BYTE FORMAT
* Platform Management FRU Information Storage Definition
*
* \param[in] buf
* \param[in] str
* \param[in] force
*
* \retval unsigned int - Number of Bytes Copied
*/
static unsigned int fru_StringInsert(unsigned char* const buf, const unsigned char* const str, const bool force);



ADI_API void fru_PrintError(const char* const msg, ...)
{
    va_list ap;
    ADI_LIBRARY_VA_START(ap, msg);
    vfprintf(stderr, msg, ap);
    ADI_LIBRARY_VA_END(ap);
}

ADI_API void fru_PrintWarning(const char* const msg, ...)
{
    va_list ap;

    if (!quiet && verbose)
    {
        ADI_LIBRARY_VA_START(ap, msg);
        ADI_LIBRARY_VPRINTF(msg, ap);
        ADI_LIBRARY_VA_END(ap);
    }
}

ADI_API void fru_PrintInfo(const char* const msg, ...)
{
    va_list ap;

    if (!quiet)
    {
        ADI_LIBRARY_VA_START(ap, msg);
        ADI_LIBRARY_VPRINTF(msg, ap);
        ADI_LIBRARY_VA_END(ap);
    }
}

static unsigned char fru_ZeroCrcCalculate(const unsigned char* const data, const size_t len)
{
    size_t i = 0U;
    unsigned char tmp = 0;

    for (i = 0U; i <= len; ++i)
    {
        tmp += data[i];
    }

    return tmp;
}

/*
 * FRU Board Area Mfg. Date / Time is the
 * number of _minutes_ from 0:00 hrs 01Jan1996
 * Max is 0xFFFFFF (3 bytes worth) or 
 * 16777215 minutes; or 
 *   279620 hours, 15 minutes; or
 *   11650 days, 20 hours, 15 minutes; or
 *   31 years, 328 days, 7 hours, 56 minutes (assuming 525949 minutes in a year); or
 *   up to : Wed Nov 24 07:56 2027
 * See:
 *   section 11, Platform Management FRU Information Storage Definition
 */
ADI_API time_t fru_Min2Date(unsigned int mins)
{
    struct tm tm;
    time_t tmp;

    /* Set up 01-Jan-1996 , and add the number of minutes to it */
    ADI_LIBRARY_MEMSET(&tm, 0, sizeof(struct tm));
    tm.tm_year = 96;
    tm.tm_mday = 1;
    tm.tm_min += mins;
    tmp = ADI_LIBRARY_MKTIME(&tm);

    return tmp;
}

#ifdef DEBUG
/*
 * Used for debugging
 */
static void fru_DumpStr(unsigned char * p, unsigned int size, unsigned int space)
{
    size_t i, j = 0, k, m, shift;
    unsigned char *t;
    unsigned char mthis = 0;
    unsigned char last = 0;

    t = p;
    k = 8 - space;
    for (i = 0; i < size; i++)
    {
        m = 0;
        (void) ADI_LIBRARY_PRINTF("%02zi: %02x : ", i, (int)*t);
        last = mthis;
        mthis = *t;
        for (shift = 0x80; shift > 0; shift >>= 1)
        {
            (void) ADI_LIBRARY_PRINTF("%s", ((*t & shift) == shift) ? "1" : "0");
            j++, m++;
            if(k == m)
            {
                (void) ADI_LIBRARY_PRINTF(" ");
                k = 8 - space + m;
                if (k >= 8)
                    k -= space;
                m = space + 100;
            }
        }
        if (space == 8)
        {
            if (*t)
            {
                (void) ADI_LIBRARY_PRINTF(" (%c) %02x", *t, *t - 0x20);
            }
            else
            {
                (void) ADI_LIBRARY_PRINTF(" (term) NULL");
            }
        }

        if (space == 6)
        {
            unsigned char x = 0, y = 0;

            if (k == 2)
            {
                y = (mthis >> 2) & 0x3F;
                x = (last >> 4) | ((mthis & 0x3) << 4);
            }
            else if (k == 4)
            {
                x = mthis & 0x3F;
            }
            else if (k == 6)
            {
                x = (last >> 6) | ((mthis & 0xF) << 2);
            }

            if (k == 4 || k == 6)
            {
                (void) ADI_LIBRARY_PRINTF(" (%02x) %02x '%c'", (int)x, x + 0x20, x + 0x20);
            }
            if (k == 2)
            {
                (void) ADI_LIBRARY_PRINTF(" (%02x) %02x '%c' | (%02x) %02x '%c'", (int)x, x + 0x20, x + 0x20, y, y + 0x20, y + 0x20);
            }
        }
        ++t;
        (void) ADI_LIBRARY_PRINTF("\n");
    }
}
#else
static void fru_DumpStr(unsigned char * UNUSED(p), unsigned int UNUSED(size), unsigned int UNUSED(space))
{
    return;
}
#endif

static int fru_Ascii2Six(unsigned char** const dest,  unsigned char* const src, const size_t size)
{
    size_t i = 0;
    ssize_t j;
    unsigned int k, m = 0;
    unsigned char *p, *d, *e;

    if (!src || !size)
        return 0;

    e = d = (unsigned char *)fru_Calloc(1, size);
    if (e == NULL)
    {
        fru_PrintError("ERROR: ADI_LIBRARY_CALLOC could not allocate requested memory\n");
        return -1;
    }
    
    p = src;
    /*
     * 6-bit requires uppercase chars, between 0 and 0x3f
     */
    while (*p != '\0' && i <= size)
    {
        j = (ssize_t)ADI_LIBRARY_TOUPPER(*p) - 0x20;
        if (j < 0 || j >= 0x40)
        {
            fru_PrintWarning("%s : trying to convert a string '%s'\n"
                    "\t\twhich includes char '%c (0x%x)', which can't be converted\n",
                    __func__, src, *p, (int)*p);
            ADI_LIBRARY_FREE(e);
            return -1;
        }
        *d = (unsigned char)j;
        ++p, ++d, i++;
    }
    /* fru_DumpStr(d, size, 4); */

    /* the length of dest, should be 3/4 of size, it's zero padded at the end */
    *dest = (unsigned char *)fru_Calloc(1, size + 1);
    if (*dest == NULL)
    {
        fru_PrintError("ERROR: ADI_LIBRARY_CALLOC could not allocate requested memory\n");
        ADI_LIBRARY_FREE(e);
        return -1;
    }

    p = *dest;

    for (i = 0; i <= size ; i += 4)
    {
        k = e[i];
        m++;
        if ((i + 1) < size)
        {
            k |= e[i + 1] << 6;
            m++;
        }
        if ((i + 2) < size)
        {
            k |= e[i + 2] << 12;
            m++;
        }
        if ((i + 3) < size)
        {
            k |= e[i + 3] << 18;
        }
#ifndef __MINGW32__
#if __BYTE_ORDER == __BIG_ENDIAN
        k = __builtin_bswap32(k);
#endif
#endif
        ADI_LIBRARY_MEMCPY(p, &k, 3);
        p += 3;
    }

    /* fru_DumpStr(*dest, m, 6); */
    ADI_LIBRARY_FREE(e);

    return  m;
}

static unsigned char* fru_Six2Ascii(const unsigned char* const buf, size_t size)
{
    unsigned char *p, *dest;
    size_t i = 0U;

    if (!size)
    {
        return NULL;
    }

    /*fru_DumpStr(buf, size, 6); */
    /* the length of dest, should be 4/3 of size + 1 for null termination char*/
    dest = (unsigned char *)fru_Calloc(1, ((size * 4) / 3) + 2);
    p = dest;

    for (i = 0; i < size; i += 3)
    {
        *dest = (buf[i] & 0x3F) + 0x20;
        /* (void) ADI_LIBRARY_PRINTF("1: %i: 0x%x (%c)\n", i, *dest, *dest); */
        ++dest;
        if ((i + 1) < size)
        {
            *dest = ((buf[i] & 0xC0) >> 6 | (buf[i+1] & 0x0F) << 2) + 0x20;
            /* (void) ADI_LIBRARY_PRINTF("2: %i: 0x%x (%c)\n", i, *dest, *dest); */
            ++dest;
        }
        if ((i + 2) < size)
        {
            *dest = ((buf[i+1] & 0xF0) >> 4 | (buf[i+2] & 0x03) << 4) + 0x20;
            /* (void) ADI_LIBRARY_PRINTF("3: %i: 0x%x (%c)\n", i, *dest, *dest); */
            ++dest;
        }
        if ((i + 3) < size)
        {
            *dest = ((buf[i+2] & 0xFC) >> 2) + 0x20;
            /* (void) ADI_LIBRARY_PRINTF("4: %i: 0x%x (%c)\n", i, *dest, *dest); */
            ++dest;
        }
    }
    /* make sure strings are null terminated */
    *dest = 0;

    /* Drop trailing spaces & null chars */
    --dest;
    while ((*dest == 0 || *dest == ' ') && size)
    {
        *dest = 0;
        --dest;
        --size;
    }

    return p;
}

static unsigned int fru_StringParse(const unsigned char* const p, unsigned char** str, const char* field)
{
    unsigned int len, i, j;

    len = p[0] & 0x3F;

    if (!len)
    {
        *str = (unsigned char *)fru_Calloc(1, 3);
        if (*str == NULL)
        {
            fru_PrintError("ERROR: ADI_LIBRARY_CALLOC could not allocate requested memory\n");
            return 0;
        }
        *str[0] = (FRU_STRING_ASCII << 6);
        return 1;
    }

    switch((p[0] >> 6) & 0x3 )
    {
        case FRU_STRING_BINARY:
            /* binary or unspecified */
            *str = (unsigned char *)fru_Calloc(1, len + 2);
            if (*str == NULL)
            {
                fru_PrintError("ERROR: ADI_LIBRARY_CALLOC could not allocate requested memory\n");
                return 0;
            }
            ADI_LIBRARY_MEMCPY(*str, p, len + 1);
            break;

        case FRU_STRING_BCD:
            /* BCD plus */
            fru_PrintError("BCD - sorry\n");
            break;

        case FRU_STRING_SIXBIT:
            /* 6-bit ASCII, packed */
            {
                unsigned char *tmp1, *tmp2;
                size_t tlen;

                tmp1 = fru_Six2Ascii(&p[1], p[0]& 0x3F);
                /* (void) ADI_LIBRARY_PRINTF("str: %s\n", foo); */
                /* fru_DumpStr(foo, ADI_LIBRARY_STRLEN(foo) + 1, 8); */
                tlen = ADI_LIBRARY_STRLEN((char *)tmp1);
                *str = (unsigned char *)fru_Calloc(1, tlen + 2);
                if (*str == NULL)
                {
                    fru_PrintError("ERROR: ADI_LIBRARY_CALLOC could not allocate requested memory\n");
                    if (tmp1 != NULL)
                    {
                        ADI_LIBRARY_FREE(tmp1);
                    }
                    return 0;
                }

                tmp2 = *str;
                tmp2++;
                ADI_LIBRARY_MEMCPY(tmp2, tmp1, tlen + 1);
                if (tlen > 0x3F)
                {
                    tlen = 0x3F;
                }

                *str[0] = (unsigned char)((FRU_STRING_ASCII << 6) | tlen);
                ADI_LIBRARY_FREE(tmp1);
            }
            break;

        case FRU_STRING_ASCII:
            /* 8-bit ASCII */
            *str = (unsigned char *)fru_Calloc(1, len + 2);
            if (*str == NULL)
            {
                fru_PrintError("ERROR: ADI_LIBRARY_CALLOC could not allocate requested memory\n");
                return 0;
            }
            ADI_LIBRARY_MEMCPY(*str, p, len + 1);
            for (i = 1; i < len; ++i)
            {
                if (p[i] < 0x20 || p[i] == 0x7F)
                {
                    fru_PrintWarning("Field '%s' marked as ASCII, but contains non-printable characters:\n", field);
                    fru_PrintWarning("  Length/Type : 0x%x (length:%i; type:ASCII(%i)\n", (int)p[0], p[0] & 0x3F, FRU_STRING_ASCII);
                    fru_PrintWarning("  Contents : ");

                    for (j = 1; j < len + 1; ++j)
                    {
                        fru_PrintWarning("0x%02x ", (int)p[j]);
                    }

                    fru_PrintWarning(" |");
                    for (j = 0; j < len + 1; ++j)
                    {
                        fru_PrintWarning("%c", ((p[j] < 32) || (p[j] >= 127)) ? '.' : p[j]);
                    }

                    fru_PrintWarning("|\n");
                }
            }
            break;
    }

    return len + 1;
}

ADI_API BOARD_INFO_t* fru_BoardAreaParse(const unsigned char* const data)
{
    BOARD_INFO_t* brdInfo = NULL;
    const unsigned char* p = NULL;
    unsigned int len, i, j;

    brdInfo = (BOARD_INFO_t*)fru_Calloc(1, sizeof(BOARD_INFO_t));

    if (data[0] != 0x01)
    {
        fru_PrintError("Board Area Format Version mismatch: 0x%02x should be 0x01\n", (int)data[0]);
        goto err;
    }

    len = (data[1] * 8) - 1;
    if (fru_ZeroCrcCalculate(data, len))
    {
        fru_PrintError("Board Area Checksum failed");
        goto err;
    }

    if (data[2] != 0 && data[2] != 25)
    {
        fru_PrintError("Board Area is non-English - sorry: Lang code = %i\n", (int)data[2]);
        goto err;
    }

    --len;
    while ((data[len] == 0x00) && (len != 0))
    {
        --len;
    }

    if (len == 0 || data[len] != 0xC1)
    {
        fru_PrintError("BOARD INFO not terminated properly, walking backwards len: %u:0x%02x should be 0xC1\n", len, (int)data[len]);
        goto err;
    }

    brdInfo->mfg_date = data[3] | (data[4] << 8) | (data[5] << 16);

    p = &data[6];
    len -= 6;

    i = fru_StringParse(p, &brdInfo->manufacturer, "Manufacture");
    p += i, len -= i;

    i = fru_StringParse(p, &brdInfo->product_name, "Product Name");
    p += i, len -= i;

    i = fru_StringParse(p, &brdInfo->serial_number, "Serial Number");
    p += i, len -= i;

    i = fru_StringParse(p, &brdInfo->part_number, "Part Number");
    p += i, len -= i;

    i = fru_StringParse(p, &brdInfo->FRU_file_ID, "FRU File ID");
    p += i, len -= i;

    j = 0;

    while (len != 0 && j < CUSTOM_FIELDS)
    {
        i = fru_StringParse(p, &brdInfo->custom[j], "Custom Field");
        p += i, len -= i, j++;
    }

    if (*p != 0xC1)
    {
        fru_PrintError("BOARD INFO not terminated properly, "
                "offset %02i(0x%02x) : %02i(0x%02x) should be 0xC1\n",
                p - data, p - data, (int)*p, (int)*p);
        goto err;
    }

    return brdInfo;

err:
    ADI_LIBRARY_FREE(brdInfo->manufacturer);
    ADI_LIBRARY_FREE(brdInfo->product_name);
    ADI_LIBRARY_FREE(brdInfo->serial_number);
    ADI_LIBRARY_FREE(brdInfo->part_number);
    ADI_LIBRARY_FREE(brdInfo->FRU_file_ID);
    for (j = 0; j < CUSTOM_FIELDS; ++j)
    {
        ADI_LIBRARY_FREE(brdInfo->custom[j]);
    }
    ADI_LIBRARY_FREE(brdInfo);

    return NULL;
}

ADI_API MULTIRECORD_INFO_t* fru_MultiboardAreaParse(const unsigned char* const data)
{
    int i = 0, tmp, type;
    const unsigned char* p = NULL;
    MULTIRECORD_INFO_t* multi = NULL;

    multi = (MULTIRECORD_INFO_t *)fru_Calloc(1, sizeof(MULTIRECORD_INFO_t));
    if (multi == NULL) 
    {
        fru_PrintError("ERROR: ADI_LIBRARY_CALLOC could not allocate requested memory\n");
        return NULL;
    }
    
    p = data;

    do {
        if (i != 0)
        {
            p += 5 + p[2];
        }

        if (p[0] >= 0x06 && p[0] <= 0xBF)
        {
            fru_PrintError("MultiRecord Area %i: Invalid Record Header\n", i);
            ADI_LIBRARY_FREE(multi);
            return NULL;
        }

        if (fru_ZeroCrcCalculate(p, 4))
        {
            fru_PrintError("MultiRecord Area %i (Record Type 0x%x): Header Checksum failed\n", i, (int)p[0]);
            ADI_LIBRARY_FREE(multi);
            return NULL;
        }

        if (!p[2] || ((fru_ZeroCrcCalculate(p+5, p[2] - 1) + p[3]) & 0xFF))
        {
            fru_PrintError("MultiRecord Area %i (Record Type 0x%x): Record Checksum failed\n", i, (int)p[0]);
            ADI_LIBRARY_FREE(multi);
            return NULL;
        }

        /*
         * Record Type ID
         */
        switch(p[0])
        {
            case MULTIRECORD_DC_OUTPUT:
            case MULTIRECORD_DC_INPUT:
                tmp = p[5] & 0xF;
                if ((tmp) >= NUM_SUPPLIES)
                {
                    fru_PrintError("Too many Supplies defined in Multirecords\n");
                    ADI_LIBRARY_FREE(multi);
                    return NULL;
                }

                multi->supplies[tmp] = (unsigned char *)fru_Calloc(1, p[2] + 6);
                if (multi->supplies[tmp] == NULL)
                {
                    fru_PrintError("ERROR: ADI_LIBRARY_CALLOC could not allocate requested memory\n");
                    ADI_LIBRARY_FREE(multi);
                    return NULL;
                }
                ADI_LIBRARY_MEMCPY (multi->supplies[tmp], p, p[2] + 6);
                multi->supplies[tmp][1] = multi->supplies[tmp][1] & 0x7F;
                break;

            case MULTIRECORD_FMC:
                /*
                 * Use VITA's OUI: 0x0012a2 is specified in the FMC spec - Rule 5.77
                 */
                if ((p[5] | p[6] << 8 | p[7] << 16) != VITA_OUI)
                {
                    fru_PrintError("OUI Doesn't match : is 0x%06X, " "should be 0x%06x\n", p[5] | p[6] << 8 | p[7] << 16, VITA_OUI);
                }
                /* type field is located: Header + Manufacturer ID = 5 + 3 */
                type = p[8] >> 4;

                switch (type)
                {
                    case MULTIRECORD_CONNECTOR:
                        /* see table 7 in FMC spec */
                        multi->connector = (unsigned char *)fru_Calloc(1, p[2] + 6);
                        if (multi->connector == NULL)
                        {
                            fru_PrintError("ERROR: ADI_LIBRARY_CALLOC could not allocate requested memory\n");
                            ADI_LIBRARY_FREE(multi);
                            return NULL;
                        }
                        ADI_LIBRARY_MEMCPY (multi->connector, p, p[2] + 6);
                        /* This isn't the end 'til we re-assemble things */
                        multi->connector[1] = multi->connector[1] & 0x7F;
                        break;

                    case MULTIRECORD_I2C:
                        if (p[2] <= 5)
                        {
                            fru_PrintWarning( "I2C MultiRecord is too short (len:%i)\n"
                                                "   (at least 4 is needed for OUI and subtype)\n", (int)p[2]);
                        }
                        else
                        {
                            /* see table 9 in FMC spec */
                            unsigned char *foo2;
                            foo2 = fru_Six2Ascii(&p[9], p[2] - 4);

                            multi->i2c_devices = (unsigned char *)fru_Calloc(1, ADI_LIBRARY_STRLEN((char *)foo2) + 1);
                            strcpy ((char *)multi->i2c_devices, (char *)foo2);
                            ADI_LIBRARY_FREE(foo2);
                            /* This isn't the end 'til we re-assemble things */
                            multi->i2c_devices[1] = multi->i2c_devices[1] & 0x7F;
                        }
                        break;

                    default:
                        fru_PrintError("Unknown Multirecord type : %i\n", type);
                        break;
                }
                break;

            default:
                fru_PrintError("Unknown MultiRecord Area\n");
                ADI_LIBRARY_FREE(multi);
                return NULL;
        }

        ++i;
    } while (!(p[1] & 0x80));

    return multi;
}

ADI_API adi_hal_Err_e fru_FruParse(const unsigned char* const data, FRU_DATA_t** const fruRecord)
{
    adi_hal_Err_e halError = ADI_HAL_ERR_NULL_PTR;
    FRU_DATA_t *fru = NULL;

    if (data == NULL)
    {
        fru_PrintError("ERROR: Invalid Data Pointer for Parsing FRU\n");
        return halError;
    }

    if (fruRecord == NULL)
    {
        fru_PrintError("ERROR: Invalid FRU Pointer for Parsing FRU\n");
        return halError;
    }

    halError = ADI_HAL_ERR_EEPROM_DATA;

    fru = (FRU_DATA_t*)fru_Calloc(1, sizeof(FRU_DATA_t));
    if (fru == NULL)
    {
        fru_PrintError("Fru allocation is NULL\n");
        halError = ADI_HAL_ERR_MEMORY;
        goto err;
    }

    /* Check FRU version */
    if (data[0] != 0x01)
    {
        fru_PrintError("FRU Version number mismatch 0x%02x should be 0x01\n", (int)data[0]);
        goto err;
    }

    /* Check Padding */
    if (data[6] != 0x00)
    {
        fru_PrintError("FRU byte 6 should be PAD, and be zero -- but it's not\n");
        goto err;
    }

    /* Check header checksum */
    if (fru_ZeroCrcCalculate(data, 7))
    {
        fru_PrintError("Common Header Checksum failed\n");
        goto err;
    }

    /* Parse Internal Use Area */
    if (data[1])
    {
        fru_PrintError("Internal Use Area not yet implemented - sorry\n");
        goto err;
    }

    /* Parse Chassis Info Area */
    if (data[2])
    {
        fru_PrintError("Chassis Info Area not yet implemented - sorry\n");
        goto err;
    }

    /* Parse Board Area */
    if (data[3])
    {
        fru->Board_Area = fru_BoardAreaParse(&data[data[3] * 8]);
        if (!fru->Board_Area)
        {
            fru_PrintError("FRU Board Area is not populated\n");
            goto err;
        }
    }

    /* Parse Chassis Info Area */
    if (data[4])
    {
        fru_PrintError("Chassis Info Area parsing not yet implemented - sorry\n");
        goto err;
    }

    /* Parse MultiRecord Area */
    if (data[5])
    {
        fru->MultiRecord_Area = fru_MultiboardAreaParse(&data[data[5] * 8]);
    }

    *fruRecord = fru;

    return ADI_HAL_ERR_OK;

err:
    if (fru != NULL)
    {
        fru_FruFree(fru);
    }

    return halError;
}

ADI_API void fru_FruWrite(const FRU_DATA_t* const fru, const char* const file_name, const bool packed)
{
    ssize_t tmp;
    size_t len = (size_t)0;
    unsigned char *buf = NULL;
    FILE *fp;

    /* Build as ASCII output */
    buf = fru_FruBlobBuild(fru, &len, packed);

    /* If it's too big, try again, with 6-bit packed */
    if (len >= 255 && !packed)
    {
        ADI_LIBRARY_FREE(buf);
        buf = fru_FruBlobBuild(fru, &len, 1);
        if (len >= 255) {
            ADI_LIBRARY_FREE(buf);
            fru_PrintError("Not able to pack things into 255 char, no output\n");
            return;
        }
    }

    if (!ADI_LIBRARY_STRCMP("-", file_name))
    {
        tmp = write(STDOUT_FILENO, buf, len);
        if ((size_t)tmp != len)
            fru_PrintError("Didn't write entire file\n");
    }
    else
    {
        if ((fp = ADI_LIBRARY_FOPEN(file_name, "wb")) == NULL)
        {
            fru_PrintError("Cannot open file.\n");
            ADI_LIBRARY_FREE(buf);
            return;
        }

        ADI_LIBRARY_FWRITE(buf, 1, len, fp);
        if (ADI_LIBRARY_FCLOSE(fp) != 0)
        {
            fru_PrintError("Cannot close file.\n");
        }
    }
    fru_PrintInfo("wrote %u bytes to %s\n", len, file_name);

    ADI_LIBRARY_FREE(buf);
}

ADI_API void fru_FruFree(FRU_DATA_t* fru)
{
    uint8_t j = 0U;

    if (fru != NULL)
    {
        if (fru->Board_Area != NULL)
        {
            ADI_LIBRARY_FREE(fru->Board_Area->manufacturer);
            ADI_LIBRARY_FREE(fru->Board_Area->product_name);
            ADI_LIBRARY_FREE(fru->Board_Area->serial_number);
            ADI_LIBRARY_FREE(fru->Board_Area->part_number);
            ADI_LIBRARY_FREE(fru->Board_Area->FRU_file_ID);

            for (j = 0U; j < CUSTOM_FIELDS; ++j)
            {
                ADI_LIBRARY_FREE(fru->Board_Area->custom[j]);
            }
            
            ADI_LIBRARY_FREE(fru->Board_Area);
        }

        if (fru->MultiRecord_Area != NULL)
        {
            for (j = 0U; j < NUM_SUPPLIES; ++j)
            {
                ADI_LIBRARY_FREE(fru->MultiRecord_Area->supplies[j]);
            }

            ADI_LIBRARY_FREE(fru->MultiRecord_Area->i2c_devices);

            ADI_LIBRARY_FREE(fru->MultiRecord_Area->connector);

            ADI_LIBRARY_FREE(fru->MultiRecord_Area);
        }

        ADI_LIBRARY_FREE(fru);
    }
}

static unsigned int fru_StringInsert(unsigned char* const buf, const unsigned char* const str, const bool force)
{
    int tmp;

    if (TYPE_CODE(str) == FRU_STRING_ASCII)
    {
        tmp = (int)ADI_LIBRARY_STRLEN((const char *)&str[1]);
    }
    else
    {
        tmp = FIELD_LEN(str);
    }

    /* Turn ASCII into 6 bit if possible */
    if ((TYPE_CODE(str) != FRU_STRING_ASCII) || (tmp <= 0x3F && !force))
    {
        /* It fits, so just leave it as ASCII/binary/whatever format it's in */
        buf[0] = (unsigned char)(tmp | (TYPE_CODE(str) << 6));
        ADI_LIBRARY_MEMCPY(&buf[1], &str[1], tmp);
    }
    else
    {
        /* turn it into 6-bit ASCII */
        unsigned char *six = NULL;

        /* (void) ADI_LIBRARY_PRINTF("six in : 0x%x : len: %2d : %s\n", (str[0] >> 6) && 0x3, str[0] & 0x3F , &str[1]); */
        /* fru_DumpStr((unsigned char*)&str[1], tmp + 1 , 8); */
        tmp = fru_Ascii2Six(&six, (unsigned char *)&str[1], tmp);
        if (tmp < 0)
        {
            /* Counldn't encode things */
            fru_PrintWarning("couldn't encode '%s' string\n", &str[1]);
            
            tmp = (int)ADI_LIBRARY_STRLEN((const char *)&str[1]);
            if (tmp > 0x3F)
            {
                fru_PrintWarning("fail : %d\n", tmp);
                fru_PrintError("String too long to fit\n");
            }

            buf[0] = (unsigned char)(tmp | (FRU_STRING_ASCII << 6));
            ADI_LIBRARY_MEMCPY(&buf[1], &str[1], tmp);
            return tmp + 1;
        }

        fru_DumpStr(six, tmp, 6);

        if (tmp > 0x3F)
        {
            fru_PrintWarning("fail : %d\n", tmp);
            fru_PrintError("String too long to fit\n");
        }

        buf[0] = (unsigned char)(tmp | (FRU_STRING_SIXBIT << 6));
        if (six != NULL)
        {
            ADI_LIBRARY_MEMCPY(&buf[1], six, tmp);
            ADI_LIBRARY_FREE(six);
        }
    }

    return (unsigned int)(tmp + 1);
}

ADI_API unsigned char* fru_FruBlobBuild(const FRU_DATA_t* const fru, size_t* const length, const bool packed)
{
    unsigned char *p, *buf;
    unsigned int st, len, tmp, last = 0, i = 0, j;

    buf = (unsigned char *)fru_Calloc(1, 1024);

    buf[0] = 0x01;
    i = 8;
    if (fru->Internal_Area)
    {
        fru_PrintError("Internal Use Area not yet implemented - sorry\n");
        ADI_LIBRARY_FREE(buf);
        return NULL;
    }

    if (fru->Chassis_Info)
    {
        fru_PrintError("Chassis Info not yet implemented - sorry\n");
        ADI_LIBRARY_FREE(buf);
        return NULL;
    }

    if (fru->Board_Area)
    {
        st = i;
        buf[3] = i / 8;
        buf[i] = 0x1;   /* Magic number */
        /* buf[i+1] = length, which needs to be determined later */
        buf[i+2] = 25;  /* English */
        buf[i+3] = (fru->Board_Area->mfg_date) & 0xFF;
        buf[i+4] = (fru->Board_Area->mfg_date >> 8) & 0xFF;
        buf[i+5] = (fru->Board_Area->mfg_date >> 16) & 0xFF;

        i += 6;
        i += fru_StringInsert(&buf[i], fru->Board_Area->manufacturer, packed);
        i += fru_StringInsert(&buf[i], fru->Board_Area->product_name, packed);
        i += fru_StringInsert(&buf[i], fru->Board_Area->serial_number, packed);
        i += fru_StringInsert(&buf[i], fru->Board_Area->part_number, packed);
        i += fru_StringInsert(&buf[i], fru->Board_Area->FRU_file_ID, packed);
        for (j = 0; j < CUSTOM_FIELDS; j++)
        {
            if (fru->Board_Area->custom[j])
            {
                i += fru_StringInsert(&buf[i], fru->Board_Area->custom[j], packed);
            }
        }
        buf[i] = 0xC1;
        i++;
        i = (((i >> 3) + 1) << 3) - 1;
        len = i - st;
        buf[st + 1] = len / 8 + 1;
        buf[i] = 256 - fru_ZeroCrcCalculate(&buf[st], len);
        i++;
    }
    if (fru->Product_Info) 
    {
        /* Do Nothing */
    }
    if (fru->MultiRecord_Area)
    {
        st = i;
        buf[5] = st / 8;
        for (tmp = 0; tmp < NUM_SUPPLIES; tmp++)
        {
            p = fru->MultiRecord_Area->supplies[tmp];
            if (!p)
                continue;
            ADI_LIBRARY_MEMCPY(&buf[i], p, p[2]+ 5);
            last = i + 1;
            i += p[2] + 5;
        }

        p = fru->MultiRecord_Area->connector;
        if (p)
        {
            ADI_LIBRARY_MEMCPY(&buf[i], p, p[2]+ 5);
            last = i + 1;
            i += p[2] + 5;
        }

        p = fru->MultiRecord_Area->i2c_devices;
        if (p)
        {
            unsigned int len, oui = VITA_OUI;
            unsigned char *six = NULL;
            len = fru_Ascii2Six(&six, p, ADI_LIBRARY_STRLEN((char *)p));

            if (six == NULL)
            {
                /* Error decoding ascii */
                ADI_LIBRARY_FREE(buf);
                return NULL;
            }

            /* Type ID, Record Format version, Length, checksum, checksum */
            ADI_LIBRARY_SPRINTF((char *)&buf[i], "%c%c%c%c%c", MULTIRECORD_FMC, 0x02, len + 4, 0, 0);
            /* Store OUI */
#ifndef __MINGW32__
# if __BYTE_ORDER == __BIG_ENDIAN
            oui = __builtin_bswap32(oui);
#endif
#endif
            ADI_LIBRARY_MEMCPY(&buf[i+5], &oui, 3);
            /* Subtype & version  - see Table 9 in FMC spec*/
            ADI_LIBRARY_SPRINTF((char *)&buf[i+8], "%c", (char)((0x1 << 4) | (0)));
            /* finally copy over the string */
            ADI_LIBRARY_MEMCPY(&buf[i+9], six, len);
            /* Record Checksum */
            buf[i+3] = 0x100 - fru_ZeroCrcCalculate(&buf[i+5], len + 3);
            /* Header Checksum */
            buf[i+4] = 0x100 - fru_ZeroCrcCalculate(&buf[i], 4);

            last = i + 1;
            i += len + 9;

            ADI_LIBRARY_FREE(six);
        }
        if (last)
        {
            buf[last] = buf[last] | 0x80;
            /* redo the header checksum, since we just changed the header :( */
            buf[last + 3] = 0;
            buf[last + 3] = 0x100 - fru_ZeroCrcCalculate(&buf[last - 1], 4);
        }
    }
    buf[7] = 256 - fru_ZeroCrcCalculate(buf, 6);

    *length = i;
    return buf;
}

ADI_API void* fru_Calloc(const size_t nmemb, const size_t size)
{
    void* ptr = NULL;

    if ((ptr = ADI_LIBRARY_CALLOC(nmemb, size)) == NULL)
    {
        fru_PrintError("ERROR: ADI_LIBRARY_CALLOC could not allocate requested memory\n");
    }

    return ptr;
}

ADI_API void fru_FruFieldDump(const char* const  description, const size_t offset, const unsigned char* const field)
{
    (void) description;

    /* does field exist, and have length? */
    if (field)
    {
        /*(void) ADI_LIBRARY_PRINTF("%s\t: ", description);*/
        if (FIELD_LEN(field))
        {
            if (TYPE_CODE(field) == FRU_STRING_ASCII || offset)
            {
                (void) ADI_LIBRARY_PRINTF("%s", &field[offset + 1]); /*first char is a field type byte*/
            }
            else
            {
                (void) ADI_LIBRARY_PRINTF("Non-ASCII\n");
            }
        }
        else
        {
            (void) ADI_LIBRARY_PRINTF("Empty Field\n");
        }
    }
}

ADI_API void fru_BoardDump(const BOARD_INFO_t* const boardInfo)
{
    uint32_t i, j;
    /*time_t tmp = fru_Min2Date(boardInfo->mfg_date);*/

    /*(void) ADI_LIBRARY_PRINTF("Date of Man\t: %s", ADI_LIBRARY_CTIME(&tmp));*/
    fru_FruFieldDump("Manufacture", 0, boardInfo->manufacturer);
    (void) ADI_LIBRARY_PRINTF(":");
    fru_FruFieldDump("Product Name", 0, boardInfo->product_name);
    (void) ADI_LIBRARY_PRINTF(":");
    fru_FruFieldDump("Serial Number", 0, boardInfo->serial_number);
    (void) ADI_LIBRARY_PRINTF(":");
    fru_FruFieldDump("Part Number", 0, boardInfo->part_number);
    /*fru_FruFieldDump("FRU File ID", 0, boardInfo->FRU_file_ID);*/
    (void) ADI_LIBRARY_PRINTF(":");

    if (!strncasecmp((const char *)&boardInfo->manufacturer[1], "Analog Devices", ADI_LIBRARY_STRLEN("Analog Devices")))
    {
        for (i = 0U; i < CUSTOM_FIELDS; ++i)
        {
            /* These are ADI custom fields */
            if (boardInfo->custom[i] && (boardInfo->custom[i][0] & 0x3F))
            {
                switch (boardInfo->custom[i][1])
                {
                    case 0:
                        /*fru_FruFieldDump("PCB Rev ", 1, boardInfo->custom[i]);*/
                        break;

                    case 1:
                        fru_FruFieldDump("PCB ID  ", 1, boardInfo->custom[i]);
                        break;

                    case 2:
                        /*fru_FruFieldDump("BOM Rev ", 1, boardInfo->custom[i]);*/
                        break;

                    case 3:
                        /*fru_FruFieldDump("Uses LVDS", 1, boardInfo->custom[i]);*/
                        break;

                    default:
                        /*fru_FruFieldDump("Unknown ", 1, boardInfo->custom[i]);*/
                        break;
                }
            }
        }
    }
    else
    {
        (void) ADI_LIBRARY_PRINTF("Custom Fields:\n");
        for (i = 0; i < CUSTOM_FIELDS; i++)
        {
            if (boardInfo->custom[i] && (boardInfo->custom[i][0] & 0x3F))
            {
                (void) ADI_LIBRARY_PRINTF("  Field %" PRIu32 " (len=%i):", i, boardInfo->custom[i][0] & 0x3F);
                for (j = 1; j <= (uint32_t) (boardInfo->custom[i][0] & 0x3F); ++j)
                {
                    (void) ADI_LIBRARY_PRINTF(" %02x", boardInfo->custom[i][j] & 0xFF);
                }
                (void) ADI_LIBRARY_PRINTF("  |");
                for (j = 1; j <= (uint32_t) (boardInfo->custom[i][0] & 0x3F); ++j)
                {
                    (void) ADI_LIBRARY_PRINTF("%c", ((boardInfo->custom[i][j] < 32) || (boardInfo->custom[i][j] >= 127)) ? '.' : boardInfo->custom[i][j]);
                }
                (void) ADI_LIBRARY_PRINTF("|\n");
            }
        }
    }
}

/* 
 * DC Load and DC Output Multi-record Definitions
 * Table 8 from the VITA/ANSI 57.1 Spec
 */
const char* const DC_Loads[] = 
{
    "P1 VADJ",          /* Load   :  0 */
    "P1 3P3V",          /* Load   :  1 */
    "P1 12P0V",         /* Load   :  2 */
    "P1 VIO_B_M2C",     /* Output :  3 */
    "P1 VREF_A_M2C",    /* Output :  4 */
    "P1 VREF_B_M2C",    /* Output :  5 */
    "P2 VADJ",          /* Load   :  6 */
    "P2 3P3V",          /* Load   :  7 */
    "P2 12P0V",         /* Load   :  8 */
    "P2 VIO_B_M2C",     /* Load   :  9 */
    "P2 VREF_A_M2C",    /* Load   : 10 */
    "P2 VREF_B_M2C",    /* Load   : 11 */
};

ADI_API void fru_MultiRecordDump( const MULTIRECORD_INFO_t* const multirecordInfo)
{
    unsigned char *p, *n, *z;
    int i;

    z = (unsigned char *)fru_Calloc(1, 12);

    for (i = 0; i <= NUM_SUPPLIES - 1; ++i)
    {
        if (!multirecordInfo->supplies[i])
            continue;
        p = multirecordInfo->supplies[i];
        n = p + 5;
        switch (p[0])
        {
            case 1:
                (void) ADI_LIBRARY_PRINTF("DC Output\n");
                (void) ADI_LIBRARY_PRINTF("  Output Number: %d (%s)\n", n[0] & 0xF, DC_Loads[n[0] & 0xF]);
                if (ADI_LIBRARY_MEMCMP(&n[1], z, 11))
                {
                    (void) ADI_LIBRARY_PRINTF("  Nominal volts:              %d (mV)\n", (n[1] | (n[2] << 8)) * 10);
                    (void) ADI_LIBRARY_PRINTF("  Maximum negative deviation: %d (mV)\n", (n[3] | (n[4] << 8)) * 10);
                    (void) ADI_LIBRARY_PRINTF("  Maximum positive deviation: %d (mV)\n", (n[5] | (n[6] << 8)) * 10);
                    (void) ADI_LIBRARY_PRINTF("  Ripple and Noise pk-pk:     %d (mV)\n", n[7] | (n[8] << 8));
                    (void) ADI_LIBRARY_PRINTF("  Minimum current draw:       %d (mA)\n", n[9] | (n[10] << 8));
                    (void) ADI_LIBRARY_PRINTF("  Maximum current draw:       %d (mA)\n", n[11] | (n[12] << 8));
                }
                else
                    (void) ADI_LIBRARY_PRINTF("  All Zeros\n");
                break;

            case 2:
                (void) ADI_LIBRARY_PRINTF("DC Load\n");
                (void) ADI_LIBRARY_PRINTF("  Output number: %d (%s)\n", n[0] & 0xF, DC_Loads[n[0] & 0xF]);
                (void) ADI_LIBRARY_PRINTF("  Nominal volts:          %04d (mV)\n", (n[1] | (n[2] << 8)) * 10);
                (void) ADI_LIBRARY_PRINTF("  Minimum voltage:        %04d (mV)\n", (n[3] | (n[4] << 8)) * 10);
                (void) ADI_LIBRARY_PRINTF("  Maximum voltage:        %04d (mV)\n", (n[5] | (n[6] << 8)) * 10);
                (void) ADI_LIBRARY_PRINTF("  Ripple and Noise pk-pk: %04d (mV)\n", n[7] | (n[8] << 8));
                (void) ADI_LIBRARY_PRINTF("  Minimum current load:   %04d (mA)\n", n[9] | (n[10] << 8));
                (void) ADI_LIBRARY_PRINTF("  Maximum current load:   %04d (mA)\n", n[11] | (n[12] << 8));
                break;

            default:
                break;
        }
    }
    ADI_LIBRARY_FREE(z);
}

ADI_API void fru_I2cDump(const MULTIRECORD_INFO_t* const multirecordInfo)
{
    unsigned char *p, *n;
    unsigned int shift;

    if (!multirecordInfo->i2c_devices)
    {
        (void) ADI_LIBRARY_PRINTF("No I2C information\n");
        return;
    }

    p = multirecordInfo->i2c_devices;
    while (*p)
    {
        n = p;
        /* skip address for now */
        while (*p < '0') p++;
        /* print name */
        while (*p >= '0')
        {
            (void) ADI_LIBRARY_PRINTF("%c", *p);
            p++;
        }
        /* now print the address */
        (void) ADI_LIBRARY_PRINTF("\t");
        while ((*n - 0x20) <= 0x0F)
        {
            (void) ADI_LIBRARY_PRINTF("0x%02x|0x%02x (0b", (*n - 0x20) << 4, (*n - 0x20) << 3);
            for (shift = 0x08; shift > 0; shift >>= 1)
                (void) ADI_LIBRARY_PRINTF("%s", (((*n - 0x20) & shift) == shift) ? "1" : "0");
            (void) ADI_LIBRARY_PRINTF("nnn[RW]);  ");
            ++n;
        }
        (void) ADI_LIBRARY_PRINTF("\n");
    }
}

ADI_API void fru_FmcDump(const MULTIRECORD_INFO_t* const multirecordInfo)
{
    unsigned char *p, *n;

    if (!multirecordInfo->connector)
    {
        (void) ADI_LIBRARY_PRINTF("No Connector information\n");
        return;
    }

    p = multirecordInfo->connector;
    n = p + 5;

    n += 3;
    switch (n[1] >> 6)
    {
        case 0:
            (void) ADI_LIBRARY_PRINTF("Single Width Card\n");
            break;

        case 1:
            (void) ADI_LIBRARY_PRINTF("Double Width Card\n");
            break;

        default:
            (void) ADI_LIBRARY_PRINTF("error - not the right size\n");
            break;
    }

    switch ((n[1] >> 4) & 0x3)
    {
        case 0:
            (void) ADI_LIBRARY_PRINTF("P1 is LPC\n");
            break;

        case 1:
            (void) ADI_LIBRARY_PRINTF("P1 is HPC\n");
            break;

        default:
            (void) ADI_LIBRARY_PRINTF("P1 not legal size\n");
            break;
    }

    switch ((n[1] >> 2) & 0x3)
    {
        case 0:
            (void) ADI_LIBRARY_PRINTF("P2 is LPC\n");
            break;

        case 1:
            (void) ADI_LIBRARY_PRINTF("P2 is HPC\n");
            break;

        case 3:
            if (n[1] >> 6 != 0)
                (void) ADI_LIBRARY_PRINTF("P2 is not populated\n");
            break;

        default:
            (void) ADI_LIBRARY_PRINTF("P2 not legal size\n");
            break;
    }

    (void) ADI_LIBRARY_PRINTF("P1 Bank A Signals needed %d\n", n[2]);
    (void) ADI_LIBRARY_PRINTF("P1 Bank B Signals needed %d\n", n[3]);
    (void) ADI_LIBRARY_PRINTF("P1 GBT Transceivers needed %d\n", n[6] >> 4);

    if (((n[1] >> 2) & 0x3) != 3)
    {
        (void) ADI_LIBRARY_PRINTF("P2 Bank A Signals needed %d\n", n[4]);
        (void) ADI_LIBRARY_PRINTF("P2 Bank B Signals needed %d\n", n[5]);
        (void) ADI_LIBRARY_PRINTF("P2 GBT Transceivers needed %d\n", n[6] & 0xF);
    }

    (void) ADI_LIBRARY_PRINTF("Max JTAG Clock %d\n", n[7]);
}
