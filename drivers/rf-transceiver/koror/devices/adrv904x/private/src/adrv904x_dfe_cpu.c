/**
* Copyright 2015 - 2023 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
 * \file adrv904x_dfe_cpu.c
 * \brief Contains ADRV904X DFE CPU related private function implementations
 *
 * ADRV904X API Version: 2.10.0.4
 */
#include "adi_library_types.h"
#include "adi_adrv904x_cpu_sys_types.h"
#include "adi_adrv904x_dfe_cpu.h"
#include "adi_adrv904x_dfe_vswr.h"
#include "adi_adrv904x_tx.h"
#include "adi_adrv904x_dfe_cpu_pintsw_status_types.h"
#include "../../private/include/adrv904x_cpu.h"
#include "../../private/include/adrv904x_cpu_dfe_init_memory.h"
#include "../../private/include/adrv904x_dfe_cpu.h"
#include "../../private/include/adrv904x_dfe_memory.h"
#include "../../private/include/adrv904x_dfe_svc_bbic_bridge_t.h"
#include "../../private/include/adrv904x_cpu_archive_types.h"
#include "../../private/include/adrv904x_cpu_cmd.h"
#include "../../private/include/adrv904x_dfe_framework_cmd_t.h"
#include "adi_adrv904x_hal.h"
#include "../../private/include/adrv904x_dfe_framework_cmd_sys_status_t.h"
#include "../../private/include/adrv904x_cpu_object_ids_types.h"
#include "../../private/include/adrv904x_reg_addr_macros.h"
#include "../../private/include/adrv904x_tx.h"
#include "../../private/include/adrv904x_dfe_svc_log_trace_t.h"

#include "../../private/bf/adrv904x_bf_tx_dfe_dig_regs.h"

/*======================= D E F I N E S =======================*/
#define ADI_FILE                        ADI_ADRV904X_FILE_PRIVATE_DFE_CPU
#define ADRV904X_DFE_LOG_BUF_SIZE       (1024u)
#define ADRV904X_DFE_MAX_ECC_SECTIONS   (1023u)

/*==================== L O C A L   T Y P E S ==================*/

/*===================== L O C A L   D A T A ===================*/
static const unsigned int _adrv904x_crc32_table[] =
{
    0x00000000, 0x04c11db7, 0x09823b6e, 0x0d4326d9,
    0x130476dc, 0x17c56b6b, 0x1a864db2, 0x1e475005,
    0x2608edb8, 0x22c9f00f, 0x2f8ad6d6, 0x2b4bcb61,
    0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd,
    0x4c11db70, 0x48d0c6c7, 0x4593e01e, 0x4152fda9,
    0x5f15adac, 0x5bd4b01b, 0x569796c2, 0x52568b75,
    0x6a1936c8, 0x6ed82b7f, 0x639b0da6, 0x675a1011,
    0x791d4014, 0x7ddc5da3, 0x709f7b7a, 0x745e66cd,
    0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039,
    0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5,
    0xbe2b5b58, 0xbaea46ef, 0xb7a96036, 0xb3687d81,
    0xad2f2d84, 0xa9ee3033, 0xa4ad16ea, 0xa06c0b5d,
    0xd4326d90, 0xd0f37027, 0xddb056fe, 0xd9714b49,
    0xc7361b4c, 0xc3f706fb, 0xceb42022, 0xca753d95,
    0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1,
    0xe13ef6f4, 0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d,
    0x34867077, 0x30476dc0, 0x3d044b19, 0x39c556ae,
    0x278206ab, 0x23431b1c, 0x2e003dc5, 0x2ac12072,
    0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16,
    0x018aeb13, 0x054bf6a4, 0x0808d07d, 0x0cc9cdca,
    0x7897ab07, 0x7c56b6b0, 0x71159069, 0x75d48dde,
    0x6b93dddb, 0x6f52c06c, 0x6211e6b5, 0x66d0fb02,
    0x5e9f46bf, 0x5a5e5b08, 0x571d7dd1, 0x53dc6066,
    0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba,
    0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e,
    0xbfa1b04b, 0xbb60adfc, 0xb6238b25, 0xb2e29692,
    0x8aad2b2f, 0x8e6c3698, 0x832f1041, 0x87ee0df6,
    0x99a95df3, 0x9d684044, 0x902b669d, 0x94ea7b2a,
    0xe0b41de7, 0xe4750050, 0xe9362689, 0xedf73b3e,
    0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2,
    0xc6bcf05f, 0xc27dede8, 0xcf3ecb31, 0xcbffd686,
    0xd5b88683, 0xd1799b34, 0xdc3abded, 0xd8fba05a,
    0x690ce0ee, 0x6dcdfd59, 0x608edb80, 0x644fc637,
    0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb,
    0x4f040d56, 0x4bc510e1, 0x46863638, 0x42472b8f,
    0x5c007b8a, 0x58c1663d, 0x558240e4, 0x51435d53,
    0x251d3b9e, 0x21dc2629, 0x2c9f00f0, 0x285e1d47,
    0x36194d42, 0x32d850f5, 0x3f9b762c, 0x3b5a6b9b,
    0x0315d626, 0x07d4cb91, 0x0a97ed48, 0x0e56f0ff,
    0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623,
    0xf12f560e, 0xf5ee4bb9, 0xf8ad6d60, 0xfc6c70d7,
    0xe22b20d2, 0xe6ea3d65, 0xeba91bbc, 0xef68060b,
    0xd727bbb6, 0xd3e6a601, 0xdea580d8, 0xda649d6f,
    0xc423cd6a, 0xc0e2d0dd, 0xcda1f604, 0xc960ebb3,
    0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7,
    0xae3afba2, 0xaafbe615, 0xa7b8c0cc, 0xa379dd7b,
    0x9b3660c6, 0x9ff77d71, 0x92b45ba8, 0x9675461f,
    0x8832161a, 0x8cf30bad, 0x81b02d74, 0x857130c3,
    0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640,
    0x4e8ee645, 0x4a4ffbf2, 0x470cdd2b, 0x43cdc09c,
    0x7b827d21, 0x7f436096, 0x7200464f, 0x76c15bf8,
    0x68860bfd, 0x6c47164a, 0x61043093, 0x65c52d24,
    0x119b4be9, 0x155a565e, 0x18197087, 0x1cd86d30,
    0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec,
    0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088,
    0x2497d08d, 0x2056cd3a, 0x2d15ebe3, 0x29d4f654,
    0xc5a92679, 0xc1683bce, 0xcc2b1d17, 0xc8ea00a0,
    0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb, 0xdbee767c,
    0xe3a1cbc1, 0xe760d676, 0xea23f0af, 0xeee2ed18,
    0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4,
    0x89b8fd09, 0x8d79e0be, 0x803ac667, 0x84fbdbd0,
    0x9abc8bd5, 0x9e7d9662, 0x933eb0bb, 0x97ffad0c,
    0xafb010b1, 0xab710d06, 0xa6322bdf, 0xa2f33668,
    0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4
};
static const uint32_t _adrv904x_reflect8[256]=
{
    0x00,0x80,0x40,0xc0,0x20,0xa0,0x60,0xe0,
    0x10,0x90,0x50,0xd0,0x30,0xb0,0x70,0xf0,
    0x08,0x88,0x48,0xc8,0x28,0xa8,0x68,0xe8,
    0x18,0x98,0x58,0xd8,0x38,0xb8,0x78,0xf8,
    0x04,0x84,0x44,0xc4,0x24,0xa4,0x64,0xe4,
    0x14,0x94,0x54,0xd4,0x34,0xb4,0x74,0xf4,
    0x0c,0x8c,0x4c,0xcc,0x2c,0xac,0x6c,0xec,
    0x1c,0x9c,0x5c,0xdc,0x3c,0xbc,0x7c,0xfc,
    0x02,0x82,0x42,0xc2,0x22,0xa2,0x62,0xe2,
    0x12,0x92,0x52,0xd2,0x32,0xb2,0x72,0xf2,
    0x0a,0x8a,0x4a,0xca,0x2a,0xaa,0x6a,0xea,
    0x1a,0x9a,0x5a,0xda,0x3a,0xba,0x7a,0xfa,
    0x06,0x86,0x46,0xc6,0x26,0xa6,0x66,0xe6,
    0x16,0x96,0x56,0xd6,0x36,0xb6,0x76,0xf6,
    0x0e,0x8e,0x4e,0xce,0x2e,0xae,0x6e,0xee,
    0x1e,0x9e,0x5e,0xde,0x3e,0xbe,0x7e,0xfe,
    0x01,0x81,0x41,0xc1,0x21,0xa1,0x61,0xe1,
    0x11,0x91,0x51,0xd1,0x31,0xb1,0x71,0xf1,
    0x09,0x89,0x49,0xc9,0x29,0xa9,0x69,0xe9,
    0x19,0x99,0x59,0xd9,0x39,0xb9,0x79,0xf9,
    0x05,0x85,0x45,0xc5,0x25,0xa5,0x65,0xe5,
    0x15,0x95,0x55,0xd5,0x35,0xb5,0x75,0xf5,
    0x0d,0x8d,0x4d,0xcd,0x2d,0xad,0x6d,0xed,
    0x1d,0x9d,0x5d,0xdd,0x3d,0xbd,0x7d,0xfd,
    0x03,0x83,0x43,0xc3,0x23,0xa3,0x63,0xe3,
    0x13,0x93,0x53,0xd3,0x33,0xb3,0x73,0xf3,
    0x0b,0x8b,0x4b,0xcb,0x2b,0xab,0x6b,0xeb,
    0x1b,0x9b,0x5b,0xdb,0x3b,0xbb,0x7b,0xfb,
    0x07,0x87,0x47,0xc7,0x27,0xa7,0x67,0xe7,
    0x17,0x97,0x57,0xd7,0x37,0xb7,0x77,0xf7,
    0x0f,0x8f,0x4f,0xcf,0x2f,0xaf,0x6f,0xef,
    0x1f,0x9f,0x5f,0xdf,0x3f,0xbf,0x7f,0xff
};

/*==== L O C A L   F U N C T I O N   P R O T O T Y P E S ======*/
static uint32_t adrv904x_reflect32(const uint32_t x);

/**
 * \brief   Puts the CRC of the DFE image in highest memory location
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
 * \param[in]     imageCrc the crc of the dfe image
  */
static adi_adrv904x_ErrAction_e adrv904x_DfeCrcSend(adi_adrv904x_Device_t * const   device,
                                                    const uint32_t                  imageCRC)

{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t dfeProfileTempAddr = ADRV904X_DFE_CPU_INIT_ADDR_GET(dfeImageCrc);

    recoveryAction = adi_adrv904x_Registers32Write(device,
                                                   NULL,
                                                   &dfeProfileTempAddr,
                                                   &imageCRC,
                                                   NULL,
                                                   1 );
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register Write Issue");
    }
    return recoveryAction;
}

/**
 * \brief   Reverse the bits of a 32 bit number.
 * \details
 *    Formulaic Reverse 32 bits
 *  In:
 *      +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 *      |31:30:29:28:27:26:25:24:23:22:21:20:19:18:17:16:15:14:13:12:11:10: 9: 8: 7: 6: 5: 4: 3: 2: 1: 0|
 *      +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 *  Out:
 *      +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 *      | 0: 1: 2: 3: 4: 5: 6: 7: 8: 9:10:11:12:13:14:15:16:17:18:19:20:21:22:23:24:25:26:27:28:29:30:31|
 *      +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 *
 * \param [in]  x - value to bit-reverse
 * \retval bit reversed value
 */
static uint32_t adrv904x_reflect32(const uint32_t x)
{
    uint32_t bits = 0;
    uint32_t mask = x;

    for( uint32_t i = 0; i < sizeof(x)*8; i++ )
    {
        bits <<= 1;
        if( mask & 1 )
            bits |= 1;
        mask >>= 1;
    }

    return bits;
}

unsigned int adrv904x_DfeCrc32Chunk (const uint8_t buf[],
                                     const uint32_t bufLen,
                                     const uint32_t seedCrc,
                                     const uint8_t finalCrc)
{
    uint32_t i = 0;
    unsigned int crc = seedCrc;
    while (i < bufLen)
    {
        crc = (crc << 8) ^ _adrv904x_crc32_table[((crc >> 24) ^ _adrv904x_reflect8[buf[i]]) & 0xFF];
        i++;
    }
    if ( finalCrc == 1 )
    {
        crc = adrv904x_reflect32( ~crc );
    }
    return crc;
}

#if (ADI_ADRV904X_DFE_LOG_CPU_CMDS == 1u)
static void adrv904x_DfeCpuCmdPayloadValToHex(const char val,
                                              char* const buffer)
{
    uint32_t v[2] = { ((uint32_t)val & 0xF0U) >> 8U, ((uint32_t)val & 0x0FU) >> 0U };
    for( uint32_t i = 0U ; i < 2U; ++i )
    {
        if( v[i] < 10U )
        {
            buffer[i] = (char)'0' + v[i];
        }
        else
        {
            buffer[i] = (char)'A' + (v[i] - 10U);
        }
    }
    buffer[2] = '\0';
}

static void adrv904x_DfeCpuCmdPayloadLog(adi_adrv904x_Device_t* const    device,
                                         void* const                     payload,
                                         const size_t                    payloadSize)
{
    char logBuf[ADRV904X_DFE_LOG_BUF_SIZE];
    char* positionIn = (char*)payload;
    for(uint32_t idx = 0U; idx < payloadSize; idx += ADI_ADRV904X_DFE_LOG_CPU_CMDS_NUM_BYTES_PER_LINE)
    {
        uint32_t i = payloadSize - idx;
        logBuf[0] = '\0';
        char* positionOut = &logBuf[0];
        uint32_t len = ADRV904X_DFE_LOG_BUF_SIZE;
        ADI_LIBRARY_STRNCAT( positionOut,
                             "[DFEMBOX] \0",
                             len);
        uint32_t s = ADI_LIBRARY_STRNLEN( positionOut,
                                          len);
        len -= s;
        positionOut += s;
        if( ADI_ADRV904X_DFE_LOG_CPU_CMDS_NUM_BYTES_PER_LINE < i)
        {
            i = ADI_ADRV904X_DFE_LOG_CPU_CMDS_NUM_BYTES_PER_LINE;
        }
        ADI_LIBRARY_STRNCAT( positionOut,
                             "   ",
                             len);
        s = ADI_LIBRARY_STRNLEN( positionOut,
                                 len);
        len -= s;
        positionOut += s;
        for(uint32_t k = 0U ; k < i; ++k)
        {
            positionOut[0] = ' ';
            adrv904x_DfeCpuCmdPayloadValToHex( *positionIn,
                                               &positionOut[1]);
            s = 3U;
            len -= s;
            positionOut += s;
            ++positionIn;
        }
        *positionOut = '\0';
        ADI_VARIABLE_LOG( &device->common,
                          ADI_HAL_LOG_MSG,
                          "%s",
                          &logBuf[0]);
    }
}

static void adrv904x_DfeCpuCommandLog(adi_adrv904x_Device_t* const    device,
                                      char* const                     pCmdTarget,
                                      const adrv904x_LinkId_e         linkId,
                                      const uint8_t                   cmdId,
                                      void* const                     pCmdPayload,
                                      const size_t                    cmdPayloadSz)
{
    ADI_VARIABLE_LOG( &device->common,
                      ADI_HAL_LOG_MSG,
                      "[DFEMBOX] Sending DFE %s command on link %d: %d, %d bytes: 0x",
                      pCmdTarget,
                      linkId,
                      cmdId,
                      cmdPayloadSz);
    adrv904x_DfeCpuCmdPayloadLog( device,
                                  pCmdPayload,
                                  cmdPayloadSz);
}

static void adrv904x_DfeCpuResponseLog(adi_adrv904x_Device_t* const    device,
                                       char* const                     pCmdTarget,
                                       const uint8_t                   cmdId,
                                       const adrv904x_CpuCmdStatus_e   cmdStatus,
                                       const adrv904x_CpuCmdId_t       rspCmdId,
                                       void* const                     pRespPayload,
                                       const size_t                    respPayloadSz)
{
    ADI_VARIABLE_LOG( &device->common,
                      ADI_HAL_LOG_MSG,
                      "[DFEMBOX] DFE %s command %d, status %d, received response %d, %d bytes: 0x",
                      pCmdTarget,
                      cmdId,
                      cmdStatus,
                      rspCmdId,
                      respPayloadSz);
    adrv904x_DfeCpuCmdPayloadLog( device,
                                  pRespPayload,
                                  respPayloadSz);
}
#endif /* (ADI_ADRV904X_DFE_LOG_CPU_CMDS == 1u) */

ADI_API adi_adrv904x_ErrAction_e adrv904x_DfeSdkDataAddrGet(adi_adrv904x_Device_t* const    device,
                                                            uint32_t* const                 dfeSdkDataAddr)
{

    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t addr0 = 0U;
    uint8_t addr1 = 0U;
    uint8_t addr2 = 0U;
    uint8_t addr3 = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    /* Check dfeSdkDataAddr address pointer is not null */
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, dfeSdkDataAddr);

    /* Read the dfe sdk version address from CPU */
    /* Read the dfe sdk version addr0 from CPU */
    recoveryAction = adrv904x_Core_ScratchReg_BfGet(device,
                                                    NULL,
                                                    ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                    (uint16_t) ADRV904X_DFE_SDK_DATA_ADDR_0_SCRATCH_REG_ID,
                                                    &addr0);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE SDK LIB version addr0");
        return recoveryAction;
    }
    /* Read the dfe sdk version addr1 from CPU */
    recoveryAction = adrv904x_Core_ScratchReg_BfGet(device,
                                                    NULL,
                                                    ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                    (uint16_t) ADRV904X_DFE_SDK_DATA_ADDR_1_SCRATCH_REG_ID,
                                                    &addr1);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE SDK LIB version addr1");
        return recoveryAction;
    }
    /* Read the dfe sdk version addr2 from CPU */
    recoveryAction = adrv904x_Core_ScratchReg_BfGet(device,
                                                    NULL,
                                                    ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                    (uint16_t) ADRV904X_DFE_SDK_DATA_ADDR_2_SCRATCH_REG_ID,
                                                    &addr2);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE SDK LIB version addr2");
        return recoveryAction;
    }
    /* Read the dfe sdk version addr3 from CPU */
    recoveryAction = adrv904x_Core_ScratchReg_BfGet(device,
                                                    NULL,
                                                    ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                    (uint16_t) ADRV904X_DFE_SDK_DATA_ADDR_3_SCRATCH_REG_ID,
                                                    &addr3);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE SDK LIB version addr3");
        return recoveryAction;
    }

    /* Get the SDK Lib address */
    *dfeSdkDataAddr = (uint32_t)(((uint32_t)addr0) | ((uint32_t)addr1 << 8) | ((uint32_t)addr2 << 16) | ((uint32_t)addr3 << 24));

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_DfeCaptureBuffersAccessSet(adi_adrv904x_Device_t* const                                    device,
                                                                     const uint32_t                                                  txChannelMask,
                                                                     const uint32_t                                                  objId,
                                                                     const uint16_t                                                  cpuCmd,
                                                                     adi_adrv904x_DfeAppFrameworkTrackingCalCapBufAccessSet_t* const capBufAccessSet)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t lengthResp = 0;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, capBufAccessSet);

    if (((txChannelMask & (~(uint32_t)ADI_ADRV904X_TXALL)) != 0U) || (txChannelMask == (uint32_t)ADI_ADRV904X_TXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            txChannelMask,
            "Invalid Tx channel is selected. Valid values are any combinations of Tx0/1/2/3/4/5/6/7");
        return recoveryAction;
    }

    if (capBufAccessSet->releaseCapBuf > 0x1u)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            capBufAccessSet->releaseCapBuf,
            "Invalid capBufAccessSet->releaseCapBuf is selected. 1 is for enable and 0 is for disable.");
        return recoveryAction;
    }
    
    if (capBufAccessSet->stopAfterCapDone > 0x1u)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            capBufAccessSet->stopAfterCapDone,
            "Invalid capBufAccessSet->stopAfterCapDone is selected. 1 is for enable and 0 is for disable.");
        return recoveryAction;
    }
    
    for (uint8_t i = 0u; i < ADI_ADRV904X_DFE_APP_FRAMEWORK_TRACKING_CAL_MAX_NUM_ERROR_CODES_TO_PAUSE_CAL; ++i)
    {
        capBufAccessSet->errCodes[i] = ADRV904X_HTOCL(capBufAccessSet->errCodes[i]);
    }
        
    for (uint8_t i = 0; i < ADI_ADRV904X_MAX_TXCHANNELS; i++)
    {
        uint32_t txChan = (1 << i);
        if (txChannelMask & txChan)
        {
            recoveryAction = adi_adrv904x_DfeAppControlCmdExec(device,
                objId,
                cpuCmd,
                (adi_adrv904x_Channels_e)txChan,
                (const uint8_t*)capBufAccessSet,
                sizeof(*capBufAccessSet),
                &lengthResp,
                NULL,
                0);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChan, "Failed to send request");
                return recoveryAction;
            }
        }
    }
        return recoveryAction;
}
ADI_API adi_adrv904x_ErrAction_e adrv904x_DfeCaptureBuffersAccessGet(adi_adrv904x_Device_t* const                                    device,
                                                                     const adi_adrv904x_TxChannels_e                                 txChannelSel,
                                                                     const uint32_t                                                  objId,
                                                                     const uint16_t                                                  cpuCmd,
                                                                     adi_adrv904x_DfeAppFrameworkTrackingCalCapBufAccessGet_t* const capBufAccessGet)
{

    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t lengthResp = 0;
    uint32_t i = 0;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, capBufAccessGet);

    /*Check that if requested Tx Channel valid*/
    /* Channel must contain a single channel number (0-7) */
    recoveryAction = adrv904x_DfeVerifyChannel((adi_adrv904x_Channels_e)txChannelSel);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannelSel, "channel parameter is invalid.");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_DfeAppControlCmdExec(device,
        objId,
        cpuCmd,
        (adi_adrv904x_Channels_e)txChannelSel,
        NULL,
        0,
        &lengthResp,
        (uint8_t*)capBufAccessGet,
        sizeof(*capBufAccessGet));
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to send request");
        return recoveryAction;
    }

    for (i = 0; i < capBufAccessGet->num; i++)
    {
        capBufAccessGet->pMem[i] = ADRV904X_CTOHLL(capBufAccessGet->pMem[i]);
    }

        return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_DfeSvcCmdSend(adi_adrv904x_Device_t* const        device,
                                                        const adrv904x_LinkId_e             linkId,
                                                        const uint8_t                       cmdId,
                                                        void* const                         pCmdPayload,
                                                        const size_t                        cmdPayloadSz,
                                                        void* const                         pRespPayload,
                                                        const size_t                        respPayloadSz,
                                                        adrv904x_DfeSvcCmdStatus_t* const   status)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_CpuCmd_t *txCmd = NULL;
    adrv904x_CpuCmdResp_t *rxRsp = NULL;
    adrv904x_CpuCmdId_t rspCmdId;
    adrv904x_CpuCmdStatus_e cmdStatus;
    uint8_t cmdBuf[SVC_BBIC_BRIDGE_MAX_MAILBOX_LINK_SIZE];

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    /* Initialize the caller's status parameter, if applicable. */
    if (status != NULL)
    {
        *status = ADRV904X_CPU_CMD_STATUS_GENERIC;
    }

    /* If the command payload size is nonzero, verify the pointer is not null */
    if (cmdPayloadSz != 0)
    {
        ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, pCmdPayload);
    }
    else if (pCmdPayload != NULL)
    {
        /* If the command payload size is zero, pointer must be null */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, pCmdPayload, "pCmdPayload must be NULL if cmdPayloadSz is 0");
        return recoveryAction;
    }

    /* If the command response payload size is nonzero, verify the pointer is not null */
    if (respPayloadSz != 0)
    {
        ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, pRespPayload);
    }
    else if (pRespPayload != NULL)
    {
        /* If the command response payload size is zero, pointer must be null */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, pRespPayload, "pRespPayload must be NULL if respPayloadSz is 0");
        return recoveryAction;
    }

    /* Verify linkId */
    if ((linkId >= ADRV904X_LINK_ID_MAX) ||
            (linkId <= ADRV904X_LINK_ID_UNKNOWN))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, linkId, "Invalid link ID");
        return recoveryAction;
    }

    /* Verify command payload size is acceptable */
    if (cmdPayloadSz > (sizeof(cmdBuf) - sizeof(adrv904x_CpuCmd_t)))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cmdPayloadSz, "cmdPayloadSz is too large for command buffer.");
        return recoveryAction;
    }

    /* Verify response payload size is acceptable */
    if (respPayloadSz > (sizeof(cmdBuf) - sizeof(adrv904x_CpuCmdResp_t)))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, respPayloadSz, "respPayloadSz is too large for command buffer.");
        return recoveryAction;
    }

    /* Overlay the command header and response header on the buffer */
    txCmd = (adrv904x_CpuCmd_t*)cmdBuf;
    rxRsp = (adrv904x_CpuCmdResp_t*)cmdBuf;

    /* Copy the caller's payload to the correct location. Size is verified above. */
    if (cmdPayloadSz != 0)
    {
        ADI_LIBRARY_MEMCPY((void*)((uint8_t*)txCmd + sizeof(adrv904x_CpuCmd_t)), pCmdPayload, cmdPayloadSz);
    }

    /* If enabled, log the requested DFE CPU command to the API log, including payload */
#if (ADI_ADRV904X_DFE_LOG_CPU_CMDS == 1u)
    adrv904x_DfeCpuCommandLog( device,
                               "service",
                               linkId,
                               cmdId,
                               pCmdPayload,
                               cmdPayloadSz);
#endif /* (ADI_ADRV904X_DFE_LOG_CPU_CMDS == 1u) */

    /* Send the command */
    recoveryAction = adrv904x_CpuCmdWrite(device, ADI_ADRV904X_CPU_TYPE_DFE, linkId, (adrv904x_CpuCmdId_t)cmdId, txCmd, cmdPayloadSz);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to send command to CPU");
        goto debug;
    }

    /* Wait for the response, then receive the payload into rxRsp. rxRsp is verified to be able to hold respPayloadSz payload above. */
    recoveryAction = adrv904x_CpuCmdRespRead(device, ADI_ADRV904X_CPU_TYPE_DFE, linkId, &rspCmdId, rxRsp, (uint32_t)respPayloadSz, &cmdStatus);

    /* If enabled, log the response from the DFE CPU command to the API log, including payload */
#if (ADI_ADRV904X_DFE_LOG_CPU_CMDS == 1u)
    adrv904x_DfeCpuResponseLog( device,
                                "service",
                                cmdId,
                                cmdStatus,
                                rspCmdId,
                                ((uint8_t*)rxRsp + sizeof(adrv904x_CpuCmdResp_t)),
                                respPayloadSz);
#endif /* (ADI_ADRV904X_DFE_LOG_CPU_CMDS == 1u) */

    /* Copy cmd status out for caller, if applicable */
    if (status != NULL)
    {
        *status = cmdStatus;
    }

    /* Process any cmd errors */
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        if (cmdStatus == ADRV904X_CPU_CMD_STATUS_CMD_FAILED)
        {
            /* For command-specific failures, copy the payload to the caller's buffer before aborting. */
            if (respPayloadSz != 0)
            {
                ADI_LIBRARY_MEMCPY(pRespPayload, (void*)((uint8_t*)rxRsp + sizeof(adrv904x_CpuCmdResp_t)), respPayloadSz);
            }
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "CPU Command Response Error");
        }
        else
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "No CPU Command Response Received");
        }

        goto debug;
    }

    /* Copy the payload to the caller's buffer */
    if (respPayloadSz != 0)
    {
        ADI_LIBRARY_MEMCPY(pRespPayload, (void*)((uint8_t*)rxRsp + sizeof(adrv904x_CpuCmdResp_t)), respPayloadSz);
    }

    return recoveryAction;

debug:

    /* Disable Error Clearing for Private API Call Case e.g. CpuPing */
    ++device->common.publicCnt;

    adrv904x_CpuErrorDebugCheck(device);

    adrv904x_DfeCpuErrorDebugCheck(device);

    --device->common.publicCnt;

    return recoveryAction;

}

ADI_API adi_adrv904x_ErrAction_e adrv904x_DfeAppCmdSend(adi_adrv904x_Device_t* const        device,
                                                        const adrv904x_LinkId_e             linkId,
                                                        const uint8_t                       cmdId,
                                                        void* const                         pCmdPayload,
                                                        const size_t                        cmdPayloadSz,
                                                        void* const                         pRespPayload,
                                                        const size_t                        respPayloadSz,
                                                        adrv904x_DfeSvcCmdStatus_t* const   status)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_CpuCmd_t *txCmd = NULL;
    adrv904x_CpuCmdResp_t *rxRsp = NULL;
    adrv904x_CpuCmdId_t rspCmdId;
    adrv904x_DfeSvcCmdStatus_t cmdStatus;
    uint8_t cmdBuf[SVC_BBIC_BRIDGE_MAX_MAILBOX_LINK_SIZE];

    adrv904x_DfeAppFrameworkCmdAppCmd_t *appCmd = NULL;  // Application cmd header
    adrv904x_DfeAppFrameworkCmdGenericResp_t *appRsp = NULL; // Application rsp header

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    /* Initialize the caller's status parameter, if applicable. */
    if (status != NULL)
    {
        *status = ADRV904X_CPU_CMD_STATUS_GENERIC;
    }

    /* If the command payload size is nonzero, verify the pointer is not null */
    if (cmdPayloadSz != 0)
    {
        ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, pCmdPayload);
    }
    else if (pCmdPayload != NULL)
    {
        /* If the command payload size is zero, pointer must be null */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, pCmdPayload, "pCmdPayload must be NULL if cmdPayloadSz is 0");
        return recoveryAction;
    }

    /* If the command response payload size is nonzero, verify the pointer is not null */
    if (respPayloadSz != 0)
    {
        ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, pRespPayload);
    }
    else if (pRespPayload != NULL)
    {
        /* If the command response payload size is zero, pointer must be null */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, pRespPayload, "pRespPayload must be NULL if respPayloadSz is 0");
        return recoveryAction;
    }

    /* Verify linkId */
    if ((linkId >= ADRV904X_LINK_ID_MAX) ||
            (linkId <= ADRV904X_LINK_ID_UNKNOWN))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, linkId, "Invalid link ID");
        return recoveryAction;
    }

    /* Verify command payload size is acceptable */
    if (cmdPayloadSz > (sizeof(cmdBuf) - sizeof(adrv904x_CpuCmd_t) - sizeof(adrv904x_DfeAppFrameworkCmdAppCmd_t)))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cmdPayloadSz, "cmdPayloadSz is too large for command buffer.");
        return recoveryAction;
    }

    /* Verify response payload size is acceptable */
    if (respPayloadSz > (sizeof(cmdBuf) - sizeof(adrv904x_CpuCmdResp_t)))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, respPayloadSz, "respPayloadSz is too large for command buffer.");
        return recoveryAction;
    }

    /* Overlay the command header and response header on the buffer */
    txCmd = (adrv904x_CpuCmd_t*)cmdBuf;
    rxRsp = (adrv904x_CpuCmdResp_t*)cmdBuf;

    /* Overlay the application header and application response header on the buffer */
    appCmd = (adrv904x_DfeAppFrameworkCmdAppCmd_t*)(cmdBuf + sizeof(adrv904x_CpuCmd_t));
    appRsp = (adrv904x_DfeAppFrameworkCmdGenericResp_t*)(cmdBuf + sizeof(adrv904x_CpuCmdResp_t));

    /* Set the application command ID */
    appCmd->appCmdOpcode = ADRV904X_HTOCS((uint16_t)cmdId);

    /* Copy the caller's payload to the correct location. Size is verified above. */
    if (cmdPayloadSz != 0)
    {
        ADI_LIBRARY_MEMCPY((void*)((uint8_t*)txCmd + sizeof(adrv904x_CpuCmd_t) + sizeof(adrv904x_DfeAppFrameworkCmdAppCmd_t)), pCmdPayload, cmdPayloadSz) ;
    }

    /* If enabled, log the requested DFE CPU command to the API log, including payload */
#if (ADI_ADRV904X_DFE_LOG_CPU_CMDS == 1u)
    adrv904x_DfeCpuCommandLog( device,
                               "app",
                               linkId,
                               cmdId,
                               pCmdPayload,
                               cmdPayloadSz);
#endif /* (ADI_ADRV904X_DFE_LOG_CPU_CMDS == 1u) */

    /* Send the command */
    recoveryAction = adrv904x_CpuCmdWrite(device, ADI_ADRV904X_CPU_TYPE_DFE, linkId,
                                         (adrv904x_CpuCmdId_t)ADRV904X_DFE_SVC_CPU_CMD_ID_DFE_APPLICATION, txCmd, (cmdPayloadSz + sizeof(adrv904x_DfeAppFrameworkCmdAppCmd_t)));
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to send command to CPU");
        goto debug;
    }

    /* Wait for the response, then receive the payload into rxRsp. rxRsp is verified to be able to hold respPayloadSz payload above. */
    recoveryAction = adrv904x_CpuCmdRespRead(device,
                                             ADI_ADRV904X_CPU_TYPE_DFE,
                                             linkId,
                                             &rspCmdId,
                                             rxRsp,
                                             (uint32_t)respPayloadSz,
                                             (adrv904x_CpuCmdStatus_e*) &cmdStatus);

    /* If enabled, log the response from the DFE CPU command to the API log, including payload */
#if (ADI_ADRV904X_DFE_LOG_CPU_CMDS == 1u)
    adrv904x_DfeCpuResponseLog( device,
                                "app",
                                cmdId,
                                cmdStatus,
                                rspCmdId,
                                ((uint8_t*)rxRsp + sizeof(adrv904x_CpuCmdResp_t)),
                                respPayloadSz);
#endif /* (ADI_ADRV904X_DFE_LOG_CPU_CMDS == 1u) */

    /* Copy cmd status out for caller, if applicable */
    if (status != NULL)
    {
        *status = cmdStatus;
    }

    /* Process any cmd errors */
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        if (cmdStatus == ADRV904X_CPU_CMD_STATUS_CMD_FAILED)
        {
            /* For command-specific failures, copy the payload to the caller's buffer before aborting. */
            if (respPayloadSz != 0)
            {
                ADI_LIBRARY_MEMCPY(pRespPayload, (void*)((uint8_t*)rxRsp + sizeof(adrv904x_CpuCmdResp_t)), respPayloadSz);
            }
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "CPU Command Response Error");
        }
        else
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "No CPU Command Response Received");
        }

        goto debug;
    }

    /* Check the application command response status */
    if (appRsp->status != ADI_ADRV904X_DFE_SVC_ERR_CODE_NO_ERROR)
    {
        /* For command-specific failures, copy the payload to the caller's buffer before aborting. */
        if (respPayloadSz != 0)
        {
            ADI_LIBRARY_MEMCPY(pRespPayload, (void*)((uint8_t*)rxRsp + sizeof(adrv904x_CpuCmdResp_t)), respPayloadSz);
        }
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "DFE Application Command Response Error");
    }


    /* Copy the payload to the caller's buffer */
    if (respPayloadSz != 0)
    {
        ADI_LIBRARY_MEMCPY(pRespPayload, (void*)((uint8_t*)rxRsp + sizeof(adrv904x_CpuCmdResp_t)), respPayloadSz);
    }

    return recoveryAction;

debug:

    /* Disable Error Clearing for Private API Call Case */
    ++device->common.publicCnt;

    adrv904x_CpuErrorDebugCheck(device);

    adrv904x_DfeCpuErrorDebugCheck(device);

    --device->common.publicCnt;

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_DfeCpuAppPing(adi_adrv904x_Device_t* const    device,
                                                        const adi_adrv904x_CpuType_e    cpuType,
                                                        const uint32_t                  writeData,
                                                        uint32_t* const                 readData)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_DfeSvcCmdPing_t pingCmd;
    adrv904x_DfeSvcCmdPingResp_t pingCmdRsp;
    adrv904x_DfeSvcCmdStatus_t cmdStatus = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;
    adi_adrv904x_DfeSvcErrCode_e cpuErrorCode = ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_EVENT_UNEXPECTED;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, readData);

    ADI_LIBRARY_MEMSET(&pingCmd, 0, sizeof(adrv904x_DfeSvcCmdPing_t));
    ADI_LIBRARY_MEMSET(&pingCmdRsp, 0, sizeof(adrv904x_DfeSvcCmdPingResp_t));

    /* Setup ping-specific payload */
    pingCmd.echoData = ADRV904X_HTOCL(writeData);

    if ((cpuType == ADI_ADRV904X_CPU_TYPE_0) ||
        (cpuType == ADI_ADRV904X_CPU_TYPE_1))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuType, "Command not supported by this CPU type");
        return recoveryAction;
    }
/* Send command and receive response */
    else if(cpuType == ADI_ADRV904X_CPU_TYPE_DFE)
    {
        recoveryAction = adrv904x_DfeAppCmdSend(device,
                                                ADRV904X_LINK_ID_0,
                                                ADRV904X_DFE_APP_FRAMEWORK_CMD_PING,
                                                (void*)&pingCmd,
                                                sizeof(pingCmd),
                                                (void*)&pingCmdRsp,
                                                sizeof(pingCmdRsp),
                                                &cmdStatus);

        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_ADRV904X_DFE_APP_CMD_RESP_CHECK_RETURN(pingCmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction);
        }
    }

    /* Extract ping-specific response */
    cpuErrorCode = ADRV904X_CTOHL(pingCmdRsp.status);
    *readData = ADRV904X_CTOHL(pingCmdRsp.echoData);

    /* Handle ping-specific errors */
    if ((cpuErrorCode != ADI_ADRV904X_DFE_SVC_ERR_CODE_NO_ERROR) || (*readData != writeData))
    {
        ADI_ADRV904X_ERROR_INFO_GET_REPORT(ADI_ADRV904X_ERRSRC_CPU,
                                           ADI_ADRV904X_ERRCODE_CPU_PING,
                                           ADI_NO_VARIABLE,
                                           recoveryAction);
                                           return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_PackagingInformationValidate(adi_adrv904x_Device_t* const device,
                                                                       const adi_adrv904x_DfeCpuBinaryInfo_t* const cpuDfeBinaryInfo)
 {
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    FILE*                       cpuImageFilePtr     = NULL;
    adrv904x_CpuArchiveHeader_t header;
    uint32_t                    byteCount           = 0U;

    /* Initialize data for crc */
#ifdef __GNUC__
    cpuImageFilePtr = ADI_LIBRARY_FOPEN((const char *)cpuDfeBinaryInfo->filePath, "rb");
#else
    if (ADI_LIBRARY_FOPEN_S(&cpuImageFilePtr, (const char *)cpuDfeBinaryInfo->filePath, "rb") != 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuDfeBinaryInfo->filePath, "Invalid Binary File or Path Detected");
        goto cleanup;
    }
#endif
    if (NULL == cpuImageFilePtr)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue with opening the DFE Archive");
        goto cleanup;
    }

    /* Move the file pointer to beginning of the file after the file crc */
    if (ADI_LIBRARY_FSEEK(cpuImageFilePtr, sizeof(uint32_t), SEEK_SET) < 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuDfeBinaryInfo->filePath, "Unable to Rewind File Pointer for DFE CRC calculation");
        goto cleanup;
    }

    byteCount = ADI_LIBRARY_FREAD(&header, 1, sizeof(adrv904x_CpuArchiveHeader_t), cpuImageFilePtr);
    if ( byteCount != sizeof(adrv904x_CpuArchiveHeader_t) )
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuDfeBinaryInfo->filePath, "Error reading Header from CPU image file");
        goto cleanup;
    }

    /* Extract archive header contents (stored in CPU byte order) */
    header.magicNum = ADRV904X_CTOHL(header.magicNum);
    header.formatRev = ADRV904X_CTOHL(header.formatRev);
    header.xsum = ADRV904X_CTOHL(header.xsum);

    if ( header.magicNum != ADRV904X_DFE_CPU_ARCHIVE_MAGIC_NUM )
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuDfeBinaryInfo->filePath, "CPU image file - invalid file type");
        goto cleanup;
    }

    if ( header.formatRev != ADRV904X_DFE_CPU_ARCHIVE_REV_1 )
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cpuDfeBinaryInfo->filePath, "CPU image file - incorrect file version");
        goto cleanup;
    }

    /* Send the CRC to DFE */
    recoveryAction = adrv904x_DfeCrcSend( device,  header.xsum) ;
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue with sending the DFE CRC");
        goto cleanup;
    }

cleanup :
    if (cpuImageFilePtr != NULL)
    {
        /* Close CPU Image File */
        if (0 != ADI_LIBRARY_FCLOSE(cpuImageFilePtr))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Cannot Close DFE CPU Image File");
        }
    }
    return recoveryAction;
 }

//TODO: Replace as part of ticket TPGSWE-6021
static uint64_t adrv904x_CpuIntFromBytesGet64(const uint8_t* const buf){
    //Only works for little endian
    uint64_t result = 0;
    result = (uint64_t) adrv904x_CpuIntFromBytesGet(&buf[0],4);
    result |= (uint64_t)adrv904x_CpuIntFromBytesGet(&buf[4],4) << 32;
    return result;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_DfeExceptionDataGet(adi_adrv904x_Device_t* const device,
                                                              adrv904x_DfePlatformExceptionData_t* dfeExceptionData)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t address = 0U;
    uint32_t baseAddress = 0U;
    uint8_t exceptionData[8] = {0};

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, dfeExceptionData, cleanup);

    if ((device->devStateInfo.devState & ADI_ADRV904X_STATE_DFECPULOADED) != ADI_ADRV904X_STATE_DFECPULOADED)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "DFE CPU bin must be loaded before getting exception data");
        goto cleanup;
    }

    /* Get dfe exception data from CPU */
    recoveryAction = adrv904x_DfeSdkDataAddrGet(device, &baseAddress);
    baseAddress += ADI_LIBRARY_OFFSETOF(adrv904x_DfeSvcBbicBridgeSdkData_t,dfeExceptionSnapshotData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE SDK exception data");
        goto cleanup;
    }

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionFlag[0]);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 4);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE exception flag, Core:0");
        goto cleanup;
    }
    dfeExceptionData->ExceptionFlag[0] = (adrv904x_DfePlatformExceptionFlag_e)adrv904x_CpuIntFromBytesGet(exceptionData,4);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[0].sp_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Stack Pointer, Core:0");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[0].sp_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[0].spsr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Save Program Status Register, Core:0");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[0].spsr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[0].elr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Exception Link Register, Core:0");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[0].elr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[0].sctlr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, System Control Register, Core:0");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[0].sctlr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[0].actlr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Auxiliary Control Register, Core:0");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[0].actlr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[0].cpacr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Architectural Feature Access Control register,Core:0");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[0].cpacr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[0].csselr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Cache Size Selection register, Core:0");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[0].csselr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[0].esr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Exception Syndrome register, Core:0");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[0].esr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[0].ttbr0_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Translation Table Base Register 0, Core:0");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[0].ttbr0_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[0].ttbr1_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Translation Base Register 1, Core:0");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[0].ttbr1_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[0].mair_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Memory Attribute Indirection register, Core:0");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[0].mair_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[0].amair_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Aux Memory Attribute Indirection register,Core:0");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[0].amair_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[0].tcr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Translation Control register, Core:0");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[0].tcr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[0].tpidr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, SoftwareThread ID register, Core:0");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[0].tpidr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[0].par_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Physical Address register, Core:0");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[0].par_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[0].mpidr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Multiprocessor Affinity register, Core:0");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[0].mpidr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[0].afsr0_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Auxiliary Fault Status register, Core:0");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[0].afsr0_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[0].afsr1_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Auxiliary Fault Status register 1, Core:0");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[0].afsr1_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[0].contextidr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Context ID register, Core:0");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[0].contextidr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[0].vbar_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Vector Base Address register, Core:0");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[0].vbar_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[0].cntkctl_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Counter-timer Kernel Control register, Core:0");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[0].cntkctl_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[0].isr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Interrupt Status register, Core:0");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[0].isr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[0].far_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Fault Address register, Core:0");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[0].far_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[0].icc_sre_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, ICC Register Enable register, Core:0");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[0].icc_sre_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[0].icc_igrpen0_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, ICC Interrupt Group 0 Enable register, Core:0");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[0].icc_igrpen0_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[0].icc_igrpen1_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, ICC Interrupt Group 1 Enable register, Core:0");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[0].icc_igrpen1_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[0].icc_ctlr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, ICC Control Register, Core:0");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[0].icc_ctlr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[0].icc_iar0_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, ICC Interrupt Ack register 0, Core:0");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[0].icc_iar0_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[0].icc_iar1_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, ICC Interrupt Ack register 1, Core:0");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[0].icc_iar1_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[0].icc_pmr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, ICC Interrupt Priority Mask register, Core:0");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[0].icc_pmr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[0].icc_bpr0_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, ICC Virtual Binary Point register 0, Core:0");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[0].icc_bpr0_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[0].icc_bpr1_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, ICC Virtual Binary Point register 1, Core:0");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[0].icc_bpr1_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[0].icc_rpr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, ICC Virtual Running Priority register, Core:0");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[0].icc_rpr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionFlag[1]);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 4);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE exception flag, Core:1");
        goto cleanup;
    }
    dfeExceptionData->ExceptionFlag[1] = (adrv904x_DfePlatformExceptionFlag_e)adrv904x_CpuIntFromBytesGet(exceptionData,4);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[1].sp_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Stack Pointer, Core:1");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[1].sp_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[1].spsr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Save Program Status Register, Core:1");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[1].spsr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[1].elr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Exception Link Register, Core:1");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[1].elr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[1].sctlr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, System Control Register, Core:1");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[1].sctlr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[1].actlr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Auxiliary Control Register, Core:1");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[1].actlr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[1].cpacr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Architectural Feature Access Control register,Core:1");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[1].cpacr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[1].csselr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Cache Size Selection register, Core:1");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[1].csselr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[1].esr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Exception Syndrome register, Core:1");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[1].esr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[1].ttbr0_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Translation Table Base Register 0, Core:1");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[1].ttbr0_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[1].ttbr1_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Translation Base Register 1, Core:1");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[1].ttbr1_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[1].mair_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Memory Attribute Indirection register, Core:1");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[1].mair_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[1].amair_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Aux Memory Attribute Indirection register,Core:1");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[1].amair_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[1].tcr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Translation Control register, Core:1");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[1].tcr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[1].tpidr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, SoftwareThread ID register, Core:1");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[1].tpidr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[1].par_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Physical Address register, Core:1");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[1].par_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[1].mpidr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Multiprocessor Affinity register, Core:1");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[1].mpidr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[1].afsr0_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Auxiliary Fault Status register, Core:1");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[1].afsr0_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[1].afsr1_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Auxiliary Fault Status register 1, Core:1");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[1].afsr1_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[1].contextidr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Context ID register, Core:1");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[1].contextidr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[1].vbar_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Vector Base Address register, Core:1");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[1].vbar_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[1].cntkctl_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Counter-timer Kernel Control register, Core:1");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[1].cntkctl_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[1].isr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Interrupt Status register, Core:1");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[1].isr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[1].far_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Fault Address register, Core:1");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[1].far_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[1].icc_sre_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, ICC Register Enable register, Core:1");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[1].icc_sre_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[1].icc_igrpen0_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, ICC Interrupt Group 0 Enable register, Core:1");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[1].icc_igrpen0_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[1].icc_igrpen1_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, ICC Interrupt Group 1 Enable register, Core:1");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[1].icc_igrpen1_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[1].icc_ctlr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, ICC Control Register, Core:1");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[1].icc_ctlr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[1].icc_iar0_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, ICC Interrupt Ack register 0, Core:1");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[1].icc_iar0_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[1].icc_iar1_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, ICC Interrupt Ack register 1, Core:1");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[1].icc_iar1_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[1].icc_pmr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, ICC Interrupt Priority Mask register, Core:1");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[1].icc_pmr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[1].icc_bpr0_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, ICC Virtual Binary Point register 0, Core:1");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[1].icc_bpr0_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[1].icc_bpr1_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, ICC Virtual Binary Point register 1, Core:1");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[1].icc_bpr1_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[1].icc_rpr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, ICC Virtual Running Priority register, Core:1");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[1].icc_rpr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionFlag[2]);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 4);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE exception flag, Core:2");
        goto cleanup;
    }
    dfeExceptionData->ExceptionFlag[2] = (adrv904x_DfePlatformExceptionFlag_e)adrv904x_CpuIntFromBytesGet(exceptionData,4);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[2].sp_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Stack Pointer, Core:2");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[2].sp_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[2].spsr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Save Program Status Register, Core:2");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[2].spsr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[2].elr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Exception Link Register, Core:2");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[2].elr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[2].sctlr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, System Control Register, Core:2");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[2].sctlr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[2].actlr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Auxiliary Control Register, Core:2");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[2].actlr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[2].cpacr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Architectural Feature Access Control register,Core:2");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[2].cpacr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[2].csselr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Cache Size Selection register, Core:2");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[2].csselr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[2].esr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Exception Syndrome register, Core:2");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[2].esr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[2].ttbr0_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Translation Table Base Register 0, Core:2");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[2].ttbr0_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[2].ttbr1_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Translation Base Register 1, Core:2");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[2].ttbr1_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[2].mair_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Memory Attribute Indirection register, Core:2");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[2].mair_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[2].amair_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Aux Memory Attribute Indirection register,Core:2");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[2].amair_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[2].tcr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Translation Control register, Core:2");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[2].tcr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[2].tpidr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, SoftwareThread ID register, Core:2");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[2].tpidr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[2].par_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Physical Address register, Core:2");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[2].par_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[2].mpidr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Multiprocessor Affinity register, Core:2");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[2].mpidr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[2].afsr0_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Auxiliary Fault Status register, Core:2");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[2].afsr0_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[2].afsr1_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Auxiliary Fault Status register 1, Core:2");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[2].afsr1_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[2].contextidr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Context ID register, Core:2");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[2].contextidr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[2].vbar_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Vector Base Address register, Core:2");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[2].vbar_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[2].cntkctl_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Counter-timer Kernel Control register, Core:2");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[2].cntkctl_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[2].isr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Interrupt Status register, Core:2");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[2].isr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[2].far_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Fault Address register, Core:2");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[2].far_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[2].icc_sre_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, ICC Register Enable register, Core:2");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[2].icc_sre_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[2].icc_igrpen0_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, ICC Interrupt Group 0 Enable register, Core:2");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[2].icc_igrpen0_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[2].icc_igrpen1_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, ICC Interrupt Group 1 Enable register, Core:2");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[2].icc_igrpen1_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[2].icc_ctlr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, ICC Control Register, Core:2");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[2].icc_ctlr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[2].icc_iar0_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, ICC Interrupt Ack register 0, Core:2");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[2].icc_iar0_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[2].icc_iar1_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, ICC Interrupt Ack register 1, Core:2");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[2].icc_iar1_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[2].icc_pmr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, ICC Interrupt Priority Mask register, Core:2");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[2].icc_pmr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[2].icc_bpr0_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, ICC Virtual Binary Point register 0, Core:2");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[2].icc_bpr0_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[2].icc_bpr1_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, ICC Virtual Binary Point register 1, Core:2");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[2].icc_bpr1_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[2].icc_rpr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, ICC Virtual Running Priority register, Core:2");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[2].icc_rpr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionFlag[3]);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 4);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE exception flag, Core:3");
        goto cleanup;
    }
    dfeExceptionData->ExceptionFlag[3] = (adrv904x_DfePlatformExceptionFlag_e)adrv904x_CpuIntFromBytesGet(exceptionData,4);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[3].sp_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Stack Pointer, Core:3");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[3].sp_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[3].spsr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Save Program Status Register, Core:3");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[3].spsr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[3].elr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Exception Link Register, Core:3");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[3].elr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[3].sctlr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, System Control Register, Core:3");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[3].sctlr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[3].actlr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Auxiliary Control Register, Core:3");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[3].actlr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[3].cpacr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Architectural Feature Access Control register,Core:3");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[3].cpacr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[3].csselr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Cache Size Selection register, Core:3");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[3].csselr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[3].esr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Exception Syndrome register, Core:3");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[3].esr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[3].ttbr0_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Translation Table Base Register 0, Core:3");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[3].ttbr0_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[3].ttbr1_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Translation Base Register 1, Core:3");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[3].ttbr1_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[3].mair_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Memory Attribute Indirection register, Core:3");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[3].mair_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[3].amair_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Aux Memory Attribute Indirection register,Core:3");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[3].amair_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[3].tcr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Translation Control register, Core:3");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[3].tcr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[3].tpidr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, SoftwareThread ID register, Core:3");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[3].tpidr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[3].par_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Physical Address register, Core:3");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[3].par_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[3].mpidr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Multiprocessor Affinity register, Core:3");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[3].mpidr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[3].afsr0_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Auxiliary Fault Status register, Core:3");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[3].afsr0_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[3].afsr1_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Auxiliary Fault Status register 1, Core:3");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[3].afsr1_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[3].contextidr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Context ID register, Core:3");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[3].contextidr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[3].vbar_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Vector Base Address register, Core:3");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[3].vbar_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[3].cntkctl_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Counter-timer Kernel Control register, Core:3");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[3].cntkctl_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[3].isr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Interrupt Status register, Core:3");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[3].isr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[3].far_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, Fault Address register, Core:3");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[3].far_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[3].icc_sre_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, ICC Register Enable register, Core:3");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[3].icc_sre_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[3].icc_igrpen0_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, ICC Interrupt Group 0 Enable register, Core:3");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[3].icc_igrpen0_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[3].icc_igrpen1_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, ICC Interrupt Group 1 Enable register, Core:3");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[3].icc_igrpen1_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[3].icc_ctlr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, ICC Control Register, Core:3");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[3].icc_ctlr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[3].icc_iar0_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, ICC Interrupt Ack register 0, Core:3");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[3].icc_iar0_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[3].icc_iar1_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, ICC Interrupt Ack register 1, Core:3");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[3].icc_iar1_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[3].icc_pmr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, ICC Interrupt Priority Mask register, Core:3");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[3].icc_pmr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[3].icc_bpr0_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, ICC Virtual Binary Point register 0, Core:3");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[3].icc_bpr0_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[3].icc_bpr1_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, ICC Virtual Binary Point register 1, Core:3");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[3].icc_bpr1_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

    address = baseAddress + ADI_LIBRARY_OFFSETOF(adrv904x_DfePlatformExceptionData_t,ExceptionRegs[3].icc_rpr_el1);
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, address, exceptionData, NULL, 8);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE Exception Data, ICC Virtual Running Priority register, Core:3");
        goto cleanup;
    }
    dfeExceptionData->ExceptionRegs[3].icc_rpr_el1 = adrv904x_CpuIntFromBytesGet64(exceptionData);

cleanup:
    return recoveryAction;
}

adi_adrv904x_ErrAction_e adrv904x_DfeVerifyChannel(const adi_adrv904x_Channels_e channel)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    /* Channel must not be set to OFF */
    if (channel != ADI_ADRV904X_CHOFF)
    {
        /* Channel must contain a single channel number (0-7) */
        if (channel == ADI_ADRV904X_CH0 ||
            channel == ADI_ADRV904X_CH1 ||
            channel == ADI_ADRV904X_CH2 ||
            channel == ADI_ADRV904X_CH3 ||
            channel == ADI_ADRV904X_CH4 ||
            channel == ADI_ADRV904X_CH5 ||
            channel == ADI_ADRV904X_CH6 ||
            channel == ADI_ADRV904X_CH7)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
        }
    }

    return recoveryAction;
}

adi_adrv904x_ErrAction_e adrv904x_DfeCalCommonStatusGet(adi_adrv904x_Device_t* const device,
                                                        const uint32_t objId,
                                                        const adi_adrv904x_Channels_e channel,
                                                        adi_adrv904x_CalStatus_t* const status)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_DfeSvcCmdStatus_t cmdStatus = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;
    char txBuf[ADRV904X_CPU_CMD_MAX_SIZE_BYTES];
    char rxBuf[ADRV904X_CPU_CMD_RESP_MAX_SIZE_BYTES];
    adrv904x_DfeAppFrameworkCmdAppGetCalStatus_t* calStatusCmd = NULL;
    adrv904x_DfeAppFrameworkCmdAppGetCalStatusResp_t* sendCalStatusCmdRsp = NULL;
    adi_adrv904x_DfeAppErrCode_e cpuErrorCode = ADI_ADRV904X_DFE_APP_ERR_CODE_NO_ERROR;
    adi_adrv904x_CalStatus_t* calStatus = NULL;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_LIBRARY_MEMSET(&txBuf[0U], 0, sizeof(txBuf));
    ADI_LIBRARY_MEMSET(&rxBuf[0U], 0, sizeof(rxBuf));

    /* validate the input parameters  */
    recoveryAction = adrv904x_DfeVerifyChannel(channel);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               channel,
                               "channel parameter is invalid.");
        return recoveryAction;
    }

    /* Build the CPU command */
    calStatusCmd = (adrv904x_DfeAppFrameworkCmdAppGetCalStatus_t*)((uint8_t*)txBuf);
    calStatusCmd->type = ADI_ADRV904X_DFE_APP_FRAMEWORK_TRACKINGCAL_STATUS_COMMON;
    calStatusCmd->calObjId = ADRV904X_HTOCL(objId);
    calStatusCmd->channelNum = ADRV904X_HTOCL(adrv904x_DfeChanMaskToNum(channel));

    sendCalStatusCmdRsp = (adrv904x_DfeAppFrameworkCmdAppGetCalStatusResp_t*)((uint8_t*)rxBuf);

    /* Then send it */
    recoveryAction = adrv904x_DfeAppCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_DFE_APP_FRAMEWORK_CMD_TRACKINGCAL_GETCALSTATUS,
                                            (void*)txBuf,
                                            sizeof(adrv904x_DfeAppFrameworkCmdAppGetCalStatus_t),
                                            (void*)sendCalStatusCmdRsp,
                                            sizeof(adrv904x_DfeAppFrameworkCmdAppGetCalStatusResp_t) + sizeof(adi_adrv904x_CalStatus_t),
                                            &cmdStatus);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_DFE_APP_CMD_RESP_CHECK_RETURN(sendCalStatusCmdRsp->cmdStatus, cmdStatus, cpuErrorCode, recoveryAction);
    }

    calStatus = (adi_adrv904x_CalStatus_t*)(rxBuf + sizeof(adrv904x_DfeAppFrameworkCmdAppGetCalStatusResp_t));

    if (calStatus != NULL)
    {
        /* Translate the response from the CPU */
        status->errorCode = ADRV904X_CTOHL(calStatus->errorCode);
        status->percentComplete = ADRV904X_CTOHL(calStatus->percentComplete);
        status->performanceMetric = ADRV904X_CTOHL(calStatus->performanceMetric);
        status->iterCount = ADRV904X_CTOHL(calStatus->iterCount);
        status->updateCount = ADRV904X_CTOHL(calStatus->updateCount);
    }

        return recoveryAction;
}

uint32_t adrv904x_DfeTrackingCalToObjId(const adi_adrv904x_DfeAppFrameworkTrackingCalibrationMask_e calId)
{
    uint32_t objId = 0U;

    objId  = adrv904x_DfeGetBitPosition((uint32_t)calId);
    objId += ADI_ADRV904X_DFE_SVC_ERR_OBJID_CAL_START;

    return objId;
}

uint32_t adrv904x_DfeGetBitPosition(const uint32_t mask)
{
    uint32_t bitPos = 0U;

    if (mask != 0U)
    {
        for (bitPos = 0U; ((1U << bitPos) & mask) == 0U; bitPos++)
        {
        }
    }

    return bitPos;
}

uint32_t adrv904x_DfeChanMaskToNum(const adi_adrv904x_Channels_e mask)
{
    return adrv904x_DfeGetBitPosition((uint32_t)mask);
}

uint32_t adrv904x_DfeChannelToChannelId(const adi_adrv904x_Channels_e channel)
{
    uint32_t div = 0U;

    for (div = 0U; div < 8U; div++)
    {
        if ((uint32_t)channel == (1U << div))
        {
            break;
        }
    }

    return div;
}

adi_adrv904x_ErrAction_e adrv904x_DfeSendSysStatusCmd(adi_adrv904x_Device_t* const device,
                                                      const adrv904x_DfeFrameworkCmdSysStatusType_e type,
                                                      const adrv904x_CpuObjectId_e sysObjId,
                                                      const adi_adrv904x_Channels_e channel)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    char txBuf[ADRV904X_CPU_CMD_MAX_SIZE_BYTES];
    adrv904x_DfeAppFrameworkCmdGetSysStatus_t* SysStatusCmd = NULL;
    adrv904x_DfeAppFrameworkCmdAppCmd_t* appInfo = NULL;
    adrv904x_CpuCmd_t* Cmd = NULL;
    adi_adrv904x_CpuType_e cpuType = ADI_ADRV904X_CPU_TYPE_DFE;
    (void)type; /* (void) used here because the type (public/private) is hardcoded to public for the public function (adrv904x_DfeSysStatusGet) */
                /* and private for the private function (adi_adrv904x_DfeSysPvtStatusGet).  This is consistent with Palau.  */

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_LIBRARY_MEMSET(&txBuf, 0, sizeof(txBuf));

    /* validate the input parameters  */
    recoveryAction = adrv904x_DfeVerifyChannel(channel);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               channel,
                               "channel parameter is invalid.");
        return recoveryAction;
    }

    if (((sysObjId  < (uint32_t) ADI_ADRV904X_DFE_APP_FRAMEWORK_OBJID_CAL_VSWR) ||
        (sysObjId  > (uint32_t) ADI_ADRV904X_DFE_APP_FRAMEWORK_OBJID_CAL_DPD)) &&
        (sysObjId != (uint32_t) ADRV904X_DFE_APP_FRAMEWORK_CMD_OBJID_DRV_DUMMY))          /*  DUMMY sysObjId used for testing */
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, sysObjId, "objId parameter is invalid.");
        return recoveryAction;
    }

    /* Build the CPU command */
    Cmd = (adrv904x_CpuCmd_t*)txBuf;
    appInfo = (adrv904x_DfeAppFrameworkCmdAppCmd_t*)((uint8_t*)Cmd + sizeof(adrv904x_CpuCmd_t));
    appInfo->appCmdOpcode = ADRV904X_DFE_APP_FRAMEWORK_CMD_SYSTEM_STATUS;
    SysStatusCmd = (adrv904x_DfeAppFrameworkCmdGetSysStatus_t*)((uint8_t*)Cmd + sizeof(adrv904x_CpuCmd_t) + sizeof(adrv904x_DfeAppFrameworkCmdAppCmd_t)) ;
    SysStatusCmd->statusType = (adrv904x_CpuCmd_SysStatusType_t)type;
    SysStatusCmd->sysObjId = ADRV904X_HTOCL(sysObjId);
    SysStatusCmd->channelNum = ADRV904X_HTOCL(adrv904x_DfeChannelToChannelId(channel));

    /* Then send it */
    recoveryAction = adrv904x_CpuCmdWrite(device,
                                          cpuType,
                                          ADRV904X_LINK_ID_0,
                                          ADRV904X_DFE_SVC_CPU_CMD_ID_DFE_APPLICATION,
                                          Cmd,
                                          sizeof(adrv904x_DfeAppFrameworkCmdGetSysStatus_t) + sizeof(adrv904x_CpuCmd_t) + sizeof(adrv904x_DfeAppFrameworkCmdAppCmd_t));

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
                             recoveryAction,
                             "Error sending GET_SYS_STATUS command.");
        return recoveryAction;
    }

    return recoveryAction;
}

adi_adrv904x_ErrAction_e adrv904x_DfeGetSysStatusCmdResp(adi_adrv904x_Device_t* const device,
                                                         const adi_adrv904x_Channels_e channel,
                                                         const size_t sysStatusSize,
                                                         void* const RxBuf,
                                                         const void** const SysStatus)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_CpuCmdId_t cmdId = 0U;
    adrv904x_CpuCmdResp_t* Cmd = (adrv904x_CpuCmdResp_t*)RxBuf;
    adrv904x_DfeAppFrameworkCmdGetSysStatusResp_t* CmdResp = (adrv904x_DfeAppFrameworkCmdGetSysStatusResp_t*)((uint8_t*)Cmd + sizeof(adrv904x_CpuCmdResp_t));
    adi_adrv904x_CpuType_e cpuType = ADI_ADRV904X_CPU_TYPE_DFE;
    adrv904x_CpuCmdStatus_e cmdStatus = ADRV904X_CPU_CMD_STATUS_GENERIC;
    adrv904x_CpuErrorCode_e cmdErrorCode = ADRV904X_CPU_NO_ERROR;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, RxBuf);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, SysStatus);

    /* validate the input parameters  */
    recoveryAction = adrv904x_DfeVerifyChannel(channel);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               channel,
                               "channel parameter is invalid.");
        return recoveryAction;
    }

    if (sysStatusSize == 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               sysStatusSize,
                               "Length is zero.");
        return recoveryAction;
    }

    /* Read the response from the CPU */
    recoveryAction = adrv904x_CpuCmdRespRead(device,
                                             cpuType,
                                             ADRV904X_LINK_ID_0,
                                             &cmdId,
                                             Cmd,
                                             sizeof(adrv904x_CpuCmdResp_t) + sizeof(adrv904x_DfeAppFrameworkCmdGetSysStatusResp_t) + sysStatusSize,
                                             &cmdStatus);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        if (cmdStatus == ADRV904X_CPU_CMD_STATUS_CMD_FAILED)
        {
            /* If the command failed for a command-specific reason, extract the command status code and log the error. */
            cmdErrorCode = (adrv904x_CpuErrorCode_e)ADRV904X_CTOHS(CmdResp->status);
            ADI_ERROR_REPORT(&device->common,
                             ADI_ADRV904X_ERRSRC_API,
                             ADI_COMMON_ERRCODE_API,
                             recoveryAction,
                             cmdErrorCode,
                             "GET_SYS_STATUS command failed.");
            return recoveryAction;
        }
        else
        {
            /* Otherwise log a generic command failed error */
            ADI_API_ERROR_REPORT(&device->common,
                                 recoveryAction,
                                 "Error getting response for GET_SYS_STATUS command.");
            return recoveryAction;
        }
    }

    /* Find the sys status in the response payload, and set the caller's pointer to it. */
    *SysStatus = (void*)((uint8_t*)CmdResp + sizeof(adrv904x_DfeAppFrameworkCmdGetSysStatusResp_t));

    return recoveryAction;
}

adi_adrv904x_ErrAction_e adrv904x_DfeSysStatusGet(adi_adrv904x_Device_t* const device,
                                                  const adrv904x_CpuObjectId_e objId,
                                                  const adi_adrv904x_Channels_e channel,
                                                  adi_adrv904x_CpuSysStatus_t* const status)
{
    adi_adrv904x_CpuSysStatus_t* SysStatus = NULL;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    char rxBuf[ADRV904X_CPU_CMD_RESP_MAX_SIZE_BYTES];

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, status);

    /* validate the input parameters  */
    recoveryAction = adrv904x_DfeVerifyChannel(channel);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               channel,
                               "channel parameter is invalid.");
        return recoveryAction;
    }

    /* Send the sys status get command to the CPU */
    recoveryAction = adrv904x_DfeSendSysStatusCmd(device,
                                                  ADRV904X_DFE_FRAMEWORK_CMD_SYS_STATUS_PUBLIC,
                                                  objId,
                                                  channel);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
                             recoveryAction,
                             "SendSysStatusCmd failed");
        return recoveryAction;
    }

    ADI_LIBRARY_MEMSET(&rxBuf, 0, sizeof(rxBuf));

    /* Get the response from the CPU */
    recoveryAction = adrv904x_DfeGetSysStatusCmdResp(device,
                                                     channel,
                                                     sizeof(adi_adrv904x_CpuSysStatus_t),
                                                     (void*)&rxBuf[0],
                                                     (const void** const)&SysStatus);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
                             recoveryAction,
                             "GetSysStatusCmdResp failed");
        return recoveryAction;
    }

    if (SysStatus != NULL)
    {
        /* Translate the response from the CPU */
        status->errorCode = ADRV904X_CTOHL(SysStatus->errorCode);
        status->placeHolder = ADRV904X_CTOHL(SysStatus->placeHolder);
    }

    return recoveryAction;
}

adi_adrv904x_ErrAction_e adrv904x_DfeEccEnableSet(adi_adrv904x_Device_t* const device,
                                                  const uint32_t eccEnableMemRegion)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_FEATURE;
    adrv904x_DfeSvcCmdStatus_t cmdStatus = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;
    adi_adrv904x_DfeSvcErrCode_e cmdErrorCode = ADI_ADRV904X_DFE_SVC_ERR_CODE_NO_ERROR;

    adrv904x_DfeSvcCmdEccEnableSet_t eccEnableSet;
    adrv904x_DfeSvcCmdEccEnableSetResp_t eccEnableSetRsp;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_LIBRARY_MEMSET(&eccEnableSet, 0, sizeof(adrv904x_DfeSvcCmdEccEnableSet_t));
    ADI_LIBRARY_MEMSET(&eccEnableSetRsp, 0, sizeof(adrv904x_DfeSvcCmdEccEnableSetResp_t));

    eccEnableSet.eccEnableMemRegion = ADRV904X_HTOCL(eccEnableMemRegion);

    /* validate input parameters */
    if ((eccEnableSet.eccEnableMemRegion == 0u) || 
        (eccEnableSet.eccEnableMemRegion > ADRV904X_DFE_MAX_ECC_SECTIONS))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, 
                               recoveryAction, 
                               eccEnableMemRegion, 
                               "Invalid eccEnableMemRegion provided.");
        return recoveryAction;
    }
    else
    {
        /* Send command and receive response */
        recoveryAction = adrv904x_DfeSvcCmdSend(device,
                                                ADRV904X_LINK_ID_0,
                                                ADRV904X_DFE_SVC_CMD_ID_ECC_ENABLE_SET,
                                                (void *)&eccEnableSet,
                                                sizeof(adrv904x_DfeSvcCmdEccEnableSet_t),
                                                (void *)&eccEnableSetRsp,
                                                sizeof(adrv904x_DfeSvcCmdEccEnableSetResp_t),
                                                &cmdStatus);
    }

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        /* If the command failed for a command-specific reason, extract the command status code and log the error. */
        cmdErrorCode = (adi_adrv904x_DfeSvcErrCode_e)ADRV904X_CTOHL(eccEnableSetRsp.status);
        ADI_ERROR_REPORT(&device->common,
                         ADI_ADRV904X_ERRSRC_API,
                         ADI_COMMON_ERRCODE_API,
                         recoveryAction,
                         cmdErrorCode,
                         "ECC Enable Set command failed.");
    }

    return recoveryAction;
}

adi_adrv904x_ErrAction_e adrv904x_DfeEccEnableGet(adi_adrv904x_Device_t* const device,
                                                  uint32_t* const  eccEnableMemRegion)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_FEATURE;
    adrv904x_DfeSvcCmdStatus_t cmdStatus = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;
    adi_adrv904x_DfeSvcErrCode_e cmdErrorCode = ADI_ADRV904X_DFE_SVC_ERR_CODE_NO_ERROR;

    adrv904x_DfeSvcCmdEccEnableGetResp_t eccEnableGetRsp;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_LIBRARY_MEMSET(&eccEnableGetRsp, 0, sizeof(adrv904x_DfeSvcCmdEccEnableGetResp_t));

    /* Send command and receive response */
    recoveryAction = adrv904x_DfeSvcCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_DFE_SVC_CMD_ID_ECC_ENABLE_GET,
                                            0U,
                                            0U,
                                            (void *)&eccEnableGetRsp,
                                            sizeof(adrv904x_DfeSvcCmdEccEnableGetResp_t),
                                            &cmdStatus);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        /* If the command failed for a command-specific reason, extract the command status code and log the error. */
        cmdErrorCode = (adi_adrv904x_DfeSvcErrCode_e)ADRV904X_CTOHL(eccEnableGetRsp.status);
        ADI_ERROR_REPORT(&device->common,
                         ADI_ADRV904X_ERRSRC_API,
                         ADI_COMMON_ERRCODE_API,
                         recoveryAction,
                         cmdErrorCode,
                         "ECC Enable Get command failed.");
        return recoveryAction;
    }

    *eccEnableMemRegion = ADRV904X_CTOHL(eccEnableGetRsp.eccEnabledMemRegion);

    return recoveryAction;
}

adi_adrv904x_ErrAction_e adrv904x_DfeLastErrorCodeGet(adi_adrv904x_Device_t* const                  device,
                                                      adi_adrv904x_DfeSvcLogTraceErrLog_t* const    lastErrorCode)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    const uint32_t MAXNUM_ERROBJ = sizeof(adi_adrv904x_DfeSvcLogTraceErrLog_t) / sizeof(adi_adrv904x_DfeSvcLogTraceErrLogData_t);
	const uint32_t MSW_64BIT_CONVERSION = 32U;
    uint32_t loop = 0U;
    uint32_t address = 0u;
    uint32_t errLogCode = 0U;
    uint32_t errLogLevel = 0U;
    uint32_t tempLogTimeStamp = 0U;
    uint64_t errLogTimeStamp = 0U;

    adi_adrv904x_DfeSvcLogTraceErrLogData_t* pLastErrorData = (adi_adrv904x_DfeSvcLogTraceErrLogData_t*)lastErrorCode;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, lastErrorCode);

    /* Get DFE errorCode addresses from CPU */
    recoveryAction = adrv904x_DfeSdkDataAddrGet(device, &address);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE last errorCode address");
        return recoveryAction;
    }
    address += ADI_LIBRARY_OFFSETOF(adrv904x_DfeSvcBbicBridgeSdkData_t, errLogData);

    for (loop = 0U; loop < MAXNUM_ERROBJ; loop++)
    {
        /* Read and Store the DFE errorCodes */
        recoveryAction = adi_adrv904x_Register32Read(device, NULL, address, (uint32_t*)&errLogCode, 0xFFFFFFFF);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE last error Code");
            return recoveryAction;
        }

        address += sizeof(uint32_t);
        recoveryAction = adi_adrv904x_Register32Read(device, NULL, address, (uint32_t*)&errLogLevel, 0xFFFFFFFF);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE last error Level");
            return recoveryAction;
        }

        address += sizeof(uint32_t);
        recoveryAction = adi_adrv904x_Register32Read(device, NULL, address, (uint32_t*)&tempLogTimeStamp, 0xFFFFFFFF);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE last error TimeStamp");
            return recoveryAction;
        }
        errLogTimeStamp = tempLogTimeStamp;

        address += sizeof(uint32_t);
        recoveryAction = adi_adrv904x_Register32Read(device, NULL, address, (uint32_t*)&tempLogTimeStamp, 0xFFFFFFFF);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE last error TimeStamp");
            return recoveryAction;
        }
        errLogTimeStamp |= ((uint64_t)(tempLogTimeStamp) << MSW_64BIT_CONVERSION);

        address += sizeof(uint32_t);
        pLastErrorData->errLogCode = ADRV904X_CTOHL(errLogCode);
        pLastErrorData->errLogLevel = ADRV904X_CTOHL(errLogLevel);
        pLastErrorData->errLogTimeStamp = ADRV904X_CTOHLL(errLogTimeStamp);
        pLastErrorData++;
    }

    return recoveryAction;
}

ADI_API void adrv904x_DfeCpuErrorDebugCheck(adi_adrv904x_Device_t* const device)
{
    adi_adrv904x_ErrAction_e                    recoveryAction                  = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adi_adrv904x_ErrSource_e                    errSrc                          = ADI_ADRV904X_ERRSRC_NONE;
    adi_adrv904x_ErrorInfo_t                    error                           = { 0, NULL, NULL, ADI_ADRV904X_ERR_ACT_NONE, NULL };
    uint32_t                                    dfeException                    = 0U;
    const uint32_t                              DFE_SVC_LOGTRACE_NUM_OBJECTS    = sizeof(adi_adrv904x_DfeSvcLogTraceErrLog_t) / sizeof(adi_adrv904x_DfeSvcLogTraceErrLogData_t);
    adi_adrv904x_DfeSvcLogTraceErrLog_t         lastError;
    adi_adrv904x_DfeSvcLogTraceErrLogData_t*    lastErrorLogData                = (adi_adrv904x_DfeSvcLogTraceErrLogData_t*)&lastError;
    uint32_t                                    i                               = 0U;
    adi_adrv904x_DfeSvcLogTraceErrLogData_t     highestSeverity;

    if (device == NULL)
    {
        return;
    }

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    if ((device->devStateInfo.devState & ADI_ADRV904X_STATE_DFECPULOADED) == ADI_ADRV904X_STATE_DFECPULOADED)
    {
        /* DFE Loaded; Not Booted */
        recoveryAction = adi_adrv904x_DfeCpuCheckException(device, &dfeException);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "DFE CPU Exception Check Issue");
            /* Continue Debug */
        }

        if (dfeException != 0U)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_RESET_DEVICE;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, dfeException, "DFE CPU Exception Detected");
            /* Continue Debug */
        }
    }

    (void) ADI_LIBRARY_MEMSET(&lastError, 0, sizeof(adi_adrv904x_DfeSvcLogTraceErrLog_t));

    (void) ADI_LIBRARY_MEMSET(&highestSeverity, 0, sizeof(adi_adrv904x_DfeSvcLogTraceErrLogData_t));

    recoveryAction = adrv904x_DfeLastErrorCodeGet(device, &lastError);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, ADI_NO_ERROR_MESSAGE);
        return;
    }

    /* Report Highest Severity Action */

    for (i = 0U; i < DFE_SVC_LOGTRACE_NUM_OBJECTS; ++i)
    {
        if (lastErrorLogData->errLogCode != 0U)
        {
            if ((lastErrorLogData->errLogLevel > highestSeverity.errLogLevel) &&
                (lastErrorLogData->errLogTimeStamp > highestSeverity.errLogTimeStamp))
            {
                highestSeverity.errLogCode = lastErrorLogData->errLogCode;
                highestSeverity.errLogLevel = lastErrorLogData->errLogLevel;
                highestSeverity.errLogTimeStamp = lastErrorLogData->errLogTimeStamp;
            }
        }

        ++lastErrorLogData;
    }

    if (highestSeverity.errLogCode != 0U)
    {
        /* APP & SVC Error Codes are routed through this API & Structure */
        if (highestSeverity.errLogCode > 0xE000U)
        {
            errSrc = ADI_ADRV904X_ERRSRC_DFE_APP_RUNTIME;
        }
        else
        {
            errSrc = ADI_ADRV904X_ERRSRC_DFE_SVC_RUNTIME;
        }

        /* ErrCause is captured once; Not Reporting Error Cause at this point */

        ADI_ADRV904X_ERROR_INFO_GET(ADI_ADRV904X_ERRSRC_DFE_APP_RUNTIME,
                                    highestSeverity.errLogCode,
                                    highestSeverity.errLogLevel,
                                    recoveryAction,
                                    error);

        ADI_ERROR_REPORT(   &device->common,
                            errSrc,
                            highestSeverity.errLogCode,
                            error.actionCode,
                            highestSeverity.errLogLevel,
                            error.errMsg);
    }

        (void) recoveryAction;
}