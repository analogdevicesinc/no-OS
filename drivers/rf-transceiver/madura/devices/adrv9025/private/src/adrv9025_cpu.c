// SPDX-License-Identifier: GPL-2.0
/**
* \file adrv9025_cpu.c
*
* ADRV9025 API Version: 6.4.0.14
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adrv9025_cpu.c
* \brief Contains CPU private features related function implementation defined in
*        adrv9025_cpu.h
*
* ADRV9025 API Version: 6.4.0.14
*/
#ifdef __KERNEL__
#include <linux/kernel.h>
#endif

#include "adi_adrv9025_user.h"
#include "adi_adrv9025_hal.h"
#include "adi_adrv9025_cpu.h"
#include "adi_adrv9025_cpu_types.h"
#include "adi_adrv9025_error.h"
#include "adi_common_error.h"
#include "../../private/include/adrv9025_cpu.h"
#include "../../private/include/adrv9025_cpu_macros.h"
#include "../../private/include/adrv9025_reg_addr_macros.h"
#include "../../private/include/adrv9025_init.h"
#include "../../private/include/adrv9025_cpu_error_mapping.h"
#include "../../private/include/adrv9025_crc32.h"

#ifdef _RELEASE_BUILD_
#line __LINE__ "adrv9025_cpu.c"
#endif

#define ADRV9025_PROFILE_CHUNK_MAX 256u

/*****************************************************************************/
/***** Local data types ******************************************************/
/*****************************************************************************/
enum adrv9025_CpuErrorFlag
{
    ADRV9025_CPU_ERROR_FLAG_INVALID = 0,
    ADRV9025_CPU_ERROR_FLAG_1,
    ADRV9025_CPU_ERROR_FLAG_2,
    ADRV9025_CPU_ERROR_FLAG_3,
    ADRV9025_CPU_ERROR_FLAG_4,
    ADRV9025_CPU_ERROR_FLAG_5,
    ADRV9025_CPU_ERROR_FLAG_6,
    ADRV9025_CPU_ERROR_FLAG_7
};

/*****************************************************************************/
/***** Helper functions' prototypes ******************************************/
/*****************************************************************************/
static uint8_t adrv9025_cpuTypeIsValid(
    adi_adrv9025_CpuType_e type);

static void adrv9025_CpuAddrInitialize(
    adi_adrv9025_CpuAddr_t* cpuAddr,
    adi_adrv9025_CpuType_e  type);

static uint8_t adrv9025_CpuIsAddrInProgRegion(
    adi_adrv9025_Cpu_t* cpu,
    uint32_t            addr);

static int32_t adrv9025_CpuDmaSetup(
    adi_adrv9025_Device_t* device,
    uint32_t               address,
    uint8_t                isWrite,
    uint8_t                isByte,
    uint8_t                autoInc);

static int32_t adrv9025_CpuDmaMemWriteByte(
    adi_adrv9025_Device_t* device,
    uint32_t               address,
    const uint8_t          data[],
    uint32_t               byteCount,
    uint8_t                autoInc);

static adi_adrv9025_CpuType_e adrv9025_CpuWhichTypeAddressBelongs(
    adi_adrv9025_Cpu_t* cpu,
    uint32_t            addr);

static int32_t adrv9025_CpuDmaMemReadByte(
    adi_adrv9025_Device_t* device,
    uint32_t               address,
    uint8_t                returnData[],
    uint32_t               byteCount,
    uint8_t                autoIncrement);

static int32_t adrv9025_CpuMailBoxErrCodeGet(
    adi_adrv9025_Device_t* device,
    adi_adrv9025_CpuType_e cpuType,
    uint16_t*              mailboxErrCode);

static const char* adrv9025_CpuErrMsgGet(
    adi_adrv9025_Device_t* device,
    adi_adrv9025_CpuType_e cpuType,
    uint32_t               errCode,
    uint16_t*              mailboxErrCodeRet);

static enum adrv9025_CpuErrorFlag adrv9025_CpuErrorFlagGet(
    uint32_t err);

static void adrv9025_CpuDataSet(
    uint8_t* cfgData,
    uint8_t  value,
    int      count);

static void adrv9025_CpuLoadAdcBytes(
    uint32_t        start,
    uint8_t         cfgData[],
    const uint16_t* data);

static void adrv9025_CpuLoadTwoBytes(
    uint32_t       start,
    uint8_t        cfgData[],
    const uint16_t data);

static void adrv9025_CpuLoadFourBytes(
    uint32_t       start,
    uint8_t        cfgData[],
    const uint32_t data);

static void adrv9025_CpuLoadRxBytes(
    uint32_t                        start,
    uint8_t                         cfgData[],
    const adi_adrv9025_RxProfile_t* rxProfile);

static void adrv9025_CpuLoadOrxBytes(
    uint32_t                        start,
    uint8_t                         cfgData[],
    const adi_adrv9025_RxProfile_t* rxProfile);

static void adrv9025_CpuLoadTxByte(
    uint32_t                        start,
    uint8_t                         cfgData[],
    const adi_adrv9025_TxProfile_t* txProfile);

static void adrv9025_CpuLoadFramerBytes(
    uint32_t                     start,
    uint8_t                      cfgData[],
    const adi_adrv9025_FrmCfg_t* framer);

static void adrv9025_CpuLoadDeframerBytes(
    uint32_t                      start,
    uint8_t                       cfgData[],
    const adi_adrv9025_DfrmCfg_t* deframer);

static void adrv9025_CpuLoadLinkSharing(
    uint32_t                             start,
    uint8_t                              cfgData[],
    const adi_adrv9025_LinkSharingCfg_t* linkSharing);

static void adrv9025_CpuLoadSerBytes(
    uint32_t                     start,
    uint8_t                      cfgData[],
    const adi_adrv9025_SerCfg_t* serdes);

static void adrv9025_CpuLoadDesBytes(
    uint32_t                     start,
    uint8_t                      cfgData[],
    const adi_adrv9025_DesCfg_t* serdes);

static void adrv9025_CpuLoadNoiseCancelBytes(
    uint32_t start,
    uint8_t  cfgData[]);

static void adrv9025_CpuLoadDataConfig(
    uint32_t                                 start,
    uint8_t                                  cfgData[],
    const adi_adrv9025_JesdDataConfigCtrl_t* dataCfg);

static int32_t adrv9025_SpiCfgSet(
    adi_adrv9025_Device_t*      device,
    adi_adrv9025_SpiSettings_t* spi);

static uint8_t adrv9025_DesCfgHasData(
    adi_adrv9025_Device_t*      device,
    adi_adrv9025_DesCfg_t*      config);

/*****************************************************************************/
/***** Helper functions' definition ******************************************/
/*****************************************************************************/

int32_t adrv9025_SpiCfgSet(adi_adrv9025_Device_t*      device,
                           adi_adrv9025_SpiSettings_t* spi)
{
    uint8_t              spiConfigA                = 0;
    uint8_t              spiConfigB                = 0;
    uint8_t              ioControl                 = 0;
    static const uint8_t SPICFG_MSBFIRST_OFF       = 0;
    static const uint8_t SPICFG_AUTOINC_DOWN       = 0;
    static const uint8_t SPICFG_FOURWIREMODE_OFF   = 0;
    static const uint8_t SPICFG_ENSPISTREAMING_OFF = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        spi);

    ADRV9025_BUGINFO(__FUNCTION__);

    if ((spi->cmosPadDrvStrength != ADI_ADRV9025_CMOSPAD_DRV_WEAK) &&
        (spi->cmosPadDrvStrength != ADI_ADRV9025_CMOSPAD_DRV_STRONG))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         spi->cmosPadDrvStrength,
                         "Invalid Cmos Pad Drive Strength\n");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* core.spi_interface_config_A */
    /* SPI bit is 1 = LSB first */
    if (spi->msbFirst == SPICFG_MSBFIRST_OFF)
    {
        spiConfigA |= ADRV9025_CONFIG_A_SPI_LSB_FIRST;
    }

    if (spi->autoIncAddrUp != SPICFG_AUTOINC_DOWN)
    {
        spiConfigA |= ADRV9025_CONFIG_A_SPI_ADDR_ASCENSION;
    }

    if (spi->fourWireMode != SPICFG_FOURWIREMODE_OFF)
    {
        spiConfigA |= ADRV9025_CONFIG_A_SPI_SDO_ACTIVE;
    }

    ADRV9025_SPIWRITEBYTE("SPI_INTERFACE_CONFIG_A",
                          ADRV9025_ADDR_SPI_INTERFACE_CONFIG_A,
                          spiConfigA);

    if (spi->cmosPadDrvStrength == ADI_ADRV9025_CMOSPAD_DRV_STRONG)
    {
        ioControl |= ADRV9025_IO_CONTROL_SPI_OUTS_DRV_SEL;
    }

    /* Force PAD mode */
    ADRV9025_SPIWRITEBYTE("DIGITAL_IO_CONTROL",
                          ADRV9025_ADDR_DIGITAL_IO_CONTROL,
                          ioControl);

    /* core.spi_interface_config_B */
    if (spi->enSpiStreaming == SPICFG_ENSPISTREAMING_OFF)
    {
        spiConfigB |= ADRV9025_CONFIG_B_SINGLE_INSTRUCTION;
    }

    /* Force single instruction mode */
    ADRV9025_SPIWRITEBYTE("SPI_INTERFACE_CONFIG_B",
                          ADRV9025_ADDR_SPI_INTERFACE_CONFIG_B,
                          spiConfigB);

    return (device->common.error.newAction);
}

/*
NCO Shifter Structure NcoShifter_t
Size : 16 bytes
Element    Type    Range    Description
Band InputBandWidth    uint32        BandWidth in Hz of the Band input signal
Band InputCenterFreq    int32        Center freq in Hz of the Band input signal
Band Nco1Freq    int32        Band NCO1 Frequency shift in Hz
Band Nco2Freq    int32        Band NCO2 Frequency shift in Hz
Table 5 - NCO Shifter Structure

*/

/*
Element    Type    Range    Description
primaryBwHz
uint32    20-300 Mhz    Primary Rx baseband bandwidth in Hz.
Corner3db    uint32    112-300 Mhz    3db corner in Hz used for TIA calibration.
rxOutputRateHz    uint32    30.72-368.64 Mhz
Rx sample rate in Hz.

ADC Corner    uint32    < 225 Mhz    ADC corner freq in Hz
NCO Shifter A    NcoShifter_t [16]        NCO Shifter
NCO Shifter B    NcoShifter_t [16]        NCO Shifter
TIA Power Setting    uint8    0-3    TIA Power Setting
dec5Enable    uint8    1 or 5    Use Dec 5 filter = 5

fir2Enable    uint8    1 or 2    Fir2 enable.
fir1Enable    uint8    1 or 2    Fir1 enable.
hb3Enable    uint8    1 or 2    HB3 enable.
hb2Enable    uint8    1 or 2    HB2 enable.
hb1Enable    uint8    1 or 2    HB1 enable.
pfirNumTaps    uint8    24-96 in incr of 24.
0 = bypassed.    Number of PFIR taps. For bypass, set pfirNumTaps = 0, pfirDecimation=1.
pfirDecimation    uint8    1,2,4    PFIR decimation.
ddcHBMode    uint8    0-7
DDC HB mode:
0: No Half Band Enabled
1: Half Band Filters only
2: Half Band Interpolation by 2
3: Half Band Decimate by 2
4: No Half Band Enabled  (Real I/F MODE)
5: Half Band Filters only (Real I/F MODE)
6: Half Band Interpolation by 2 (Real I/F MODE)
7: Half Band Decimate by 2 (Real I/F MODE)
Bit 7: 1 = Combine dualband AB, 0 disable combine dualband on AB.
RxHb1WidebandEnable uint8	0, 1	1 = Wide band select, 0 = Narrow band select
Reserved[13]	uint8	13 bytes reserved.

*/
void adrv9025_CpuLoadRxBytes(uint32_t                        start,
                             uint8_t                         cfgData[],
                             const adi_adrv9025_RxProfile_t* rxProfile)
{
    adrv9025_CpuLoadFourBytes(start + 0,
                              cfgData,
                              rxProfile->rfBandwidth_kHz * 1000);
    adrv9025_CpuLoadFourBytes(start + 4,
                              cfgData,
                              rxProfile->rxBbf3dBCorner_kHz * 1000);
    adrv9025_CpuLoadFourBytes(start + 8,
                              cfgData,
                              rxProfile->rxOutputRate_kHz * 1000);
    adrv9025_CpuLoadFourBytes(start + 12,
                              cfgData,
                              rxProfile->rxAdcBandWidth_kHz * 1000);
    adrv9025_CpuLoadFourBytes(start + 16,
                              cfgData,
                              rxProfile->rxNcoShifterCfg.bandAInputBandWidth_kHz * 1000);
    adrv9025_CpuLoadFourBytes(start + 20,
                              cfgData,
                              rxProfile->rxNcoShifterCfg.bandAInputCenterFreq_kHz * 1000);
    adrv9025_CpuLoadFourBytes(start + 24,
                              cfgData,
                              rxProfile->rxNcoShifterCfg.bandANco1Freq_kHz * 1000);
    adrv9025_CpuLoadFourBytes(start + 28,
                              cfgData,
                              rxProfile->rxNcoShifterCfg.bandANco2Freq_kHz * 1000);
    adrv9025_CpuLoadFourBytes(start + 32,
                              cfgData,
                              rxProfile->rxNcoShifterCfg.bandBInputBandWidth_kHz * 1000);
    adrv9025_CpuLoadFourBytes(start + 36,
                              cfgData,
                              rxProfile->rxNcoShifterCfg.bandBInputCenterFreq_kHz * 1000);
    adrv9025_CpuLoadFourBytes(start + 40,
                              cfgData,
                              rxProfile->rxNcoShifterCfg.bandBNco1Freq_kHz * 1000);
    adrv9025_CpuLoadFourBytes(start + 44,
                              cfgData,
                              rxProfile->rxNcoShifterCfg.bandBNco2Freq_kHz * 1000);
    cfgData[start + 48] = rxProfile->tiaPowerMode;
    cfgData[start + 49] = rxProfile->rxDec5Decimation;
    cfgData[start + 50] = rxProfile->rxFir2Decimation;
    cfgData[start + 51] = rxProfile->rxFir1Decimation;
    cfgData[start + 52] = rxProfile->rhb3Decimation;
    cfgData[start + 53] = rxProfile->rhb2Decimation;
    cfgData[start + 54] = rxProfile->rhb1Decimation;
    cfgData[start + 55] = rxProfile->rxFir.numFirCoefs;
    cfgData[start + 56] = rxProfile->rxFirDecimation;
    /* For bypass, set pfirNumTaps = 0, pfirDecimation=1. */
    if (rxProfile->rxFir.numFirCoefs == 0)
    {
        cfgData[start + 56] = 1; /* rxProfile->rxFirDecimation; */
    }

    if (rxProfile->rxNcoShifterCfg.bandAbCombinedEnable == 1)
    {
        cfgData[start + 57] = rxProfile->rxDdcMode | 0x80; /* Set left most bit */
    }
    else
    {
        cfgData[start + 57] = rxProfile->rxDdcMode;
    }

    cfgData[start + 58] = rxProfile->rhb1WideBandMode;
    cfgData[start + 59] = 0;

    cfgData[start + 60] = 0;
    cfgData[start + 61] = 0;
    cfgData[start + 62] = 0;
    cfgData[start + 63] = 0;

    cfgData[start + 64] = 0;
    cfgData[start + 65] = 0;
    cfgData[start + 66] = 0;
    cfgData[start + 67] = 0;

    cfgData[start + 68] = 0;
    cfgData[start + 69] = 0;
    cfgData[start + 70] = 0;
    cfgData[start + 71] = 0;
}

/*
Orx Baseband Receiver Structure : OrxConfig_t
Size : 56 bytes
Element    Type    Range    Description
RfBw
uint32    112-450 Mhz    Total baseband RF Bandwidth in Hz.
Corner3dB    uint32    112-450 Mhz    3db corner in Hz used for TIA calibration.
orxOutputRateHz    uint32    112.88 -491.52 Mhz
Orx Output Rate in Hz.
ADC Corner    uint32    < 225 Mhz    ADC corner freq in Hz
NCO Shifter    NcoShifter_t [16]        NCO Shifter
TIA Power Setting    uint8    0-3    TIA Power Setting
dec5Enable    uint8    1 or 5    Decimation 4/5.
fir1Enable    uint8    1 or 2    Fir1 enable.
hb3Enable    uint8    1 or 2    HB3 enable.
hb2Enable    uint8    1 or 2    HB2 enable.
hb1Enable    uint8    1 or 2    HB1 enable.
pfirNumTaps    uint8    24-96 in incr of 24.
0 = bypassed.    Number of PFIR taps.  For bypass, set pfirNumTaps = 0, pfirDecimation=1.
pfirDecimation    uint8    1,2,4    PFIR decimation.
ddcHBMode    uint8    0,1,2    DDC HB mode:
0: No Half Band Enabled
1: Half Band Filters only
2: Half Band Interpolation by 2
3: Half Band Decimate by 2
4: No Half Band Enabled  (Real I/F MODE)
5: Half Band Filters only (Real I/F MODE)
6: Half Band Interpolation by 2 (Real I/F MODE)
7: Half Band Decimate by 2 (Real I/F MODE)
Reserved[15]    uint8        15 bytes reserved.
Table 5 - Orx Profile Structure

*/
void adrv9025_CpuLoadOrxBytes(uint32_t                        start,
                              uint8_t                         cfgData[],
                              const adi_adrv9025_RxProfile_t* rxProfile)
{
    adrv9025_CpuLoadFourBytes(start + 0,
                              cfgData,
                              rxProfile->rfBandwidth_kHz * 1000);
    adrv9025_CpuLoadFourBytes(start + 4,
                              cfgData,
                              rxProfile->rxBbf3dBCorner_kHz * 1000);
    adrv9025_CpuLoadFourBytes(start + 8,
                              cfgData,
                              rxProfile->rxOutputRate_kHz * 1000);
    adrv9025_CpuLoadFourBytes(start + 12,
                              cfgData,
                              rxProfile->rxAdcBandWidth_kHz * 1000);
    adrv9025_CpuLoadFourBytes(start + 16,
                              cfgData,
                              rxProfile->rxNcoShifterCfg.bandAInputBandWidth_kHz * 1000);
    adrv9025_CpuLoadFourBytes(start + 20,
                              cfgData,
                              rxProfile->rxNcoShifterCfg.bandAInputCenterFreq_kHz * 1000);
    adrv9025_CpuLoadFourBytes(start + 24,
                              cfgData,
                              rxProfile->rxNcoShifterCfg.bandANco1Freq_kHz * 1000);
    adrv9025_CpuLoadFourBytes(start + 28,
                              cfgData,
                              rxProfile->rxNcoShifterCfg.bandANco2Freq_kHz * 1000);
    cfgData[start + 32] = rxProfile->tiaPowerMode;
    cfgData[start + 33] = rxProfile->rxDec5Decimation;
    cfgData[start + 34] = rxProfile->rxFir1Decimation;
    cfgData[start + 35] = rxProfile->rhb3Decimation;
    cfgData[start + 36] = rxProfile->rhb2Decimation;
    cfgData[start + 37] = rxProfile->rhb1Decimation;
    cfgData[start + 38] = rxProfile->rxFir.numFirCoefs;
    cfgData[start + 39] = rxProfile->rxFirDecimation;
    /* For bypass, set pfirNumTaps = 0, pfirDecimation=1. */
    if (rxProfile->rxFir.numFirCoefs == 0)
    {
        cfgData[start + 39] = 1; /* rxProfile->rxFirDecimation; */
    }

    cfgData[start + 40] = rxProfile->rxDdcMode;
    cfgData[start + 41] = 0;
    cfgData[start + 42] = 0;
    cfgData[start + 43] = 0;

    cfgData[start + 44] = 0;
    cfgData[start + 45] = 0;
    cfgData[start + 46] = 0;
    cfgData[start + 47] = 0;

    cfgData[start + 48] = 0;
    cfgData[start + 49] = 0;
    cfgData[start + 50] = 0;
    cfgData[start + 51] = 0;

    cfgData[start + 52] = 0;
    cfgData[start + 53] = 0;
    cfgData[start + 54] = 0;
    cfgData[start + 55] = 0;
}

/*
Transmitter Structure: TxConfig_t
Size : 40 Bytes
Element    Type    Range    Description
primaryBwHz
uint32    50-200 Mhz    Primary Tx baseband bandwidth in Hz.
DpdBwHz
uint32    112-491 Mhz    Primary DPD Tx bandwidth in Hz.
Butter filter fc    uint32    50-225 Mhz    TxBBF filter tuning value in Hz.
Pre-PD Real pole fc    uint32    112-491 Mhz    ????
TxInputRateHz    uint32    ????    Transmitter input rate in Hz.
TxDpdHb1Enable
uint8    1 or 2    DPD Halfband 1 setting.
TxDpdHb2Enable
uint8    1 or 2    DPD Halfband 2 setting.
TxPfirInter
uint8    0, 1, 2, 4
Transmitter Pfir interpolation.
TxThb1Enable
uint8    1 or 2    Transmitter Halfband 1 setting.
TxThb2Enable
uint8    1 or 2    Transmitter Halfband 2 setting.
TxThb3Enable
uint8    1 or 2    Transmitter Halfband 3 setting.
TxInt5Enable
uint8    1 or 5    Transmitter interpolate setting.
pfirNumTaps
uint8    20-100 24-96 in incr of 20    Transmitter number of Pfir tap.
Reserved[12]    uint8        12 bytes reserved.
Table 4 - Tx Profile Structure

*/
void adrv9025_CpuLoadTxByte(uint32_t                        start,
                            uint8_t                         cfgData[],
                            const adi_adrv9025_TxProfile_t* txProfile)
{
    adrv9025_CpuLoadFourBytes(start + 0,
                              cfgData,
                              txProfile->primarySigBandwidth_kHz * 1000);
    adrv9025_CpuLoadFourBytes(start + 4,
                              cfgData,
                              txProfile->rfBandwidth_kHz * 1000);
    adrv9025_CpuLoadFourBytes(start + 8,
                              cfgData,
                              txProfile->txBbf3dBCorner_kHz * 1000);
    adrv9025_CpuLoadFourBytes(start + 12,
                              cfgData,
                              txProfile->txDac3dBCorner_kHz * 1000);
    adrv9025_CpuLoadFourBytes(start + 16,
                              cfgData,
                              txProfile->txInputRate_kHz * 1000);
    cfgData[start + 20] = txProfile->dpdHb1Interpolation;
    cfgData[start + 21] = txProfile->dpdHb2Interpolation;
    cfgData[start + 22] = txProfile->txFirInterpolation;
    cfgData[start + 23] = txProfile->thb1Interpolation;
    cfgData[start + 24] = txProfile->thb2Interpolation;
    cfgData[start + 25] = txProfile->thb3Interpolation;
    cfgData[start + 26] = txProfile->txInt5Interpolation;
    cfgData[start + 27] = txProfile->txFir.numFirCoefs;
    cfgData[start + 28] = txProfile->txBbfPowerMode;
    cfgData[start + 29] = 0;
    cfgData[start + 30] = 0;
    cfgData[start + 31] = 0;

    cfgData[start + 32] = 0;
    cfgData[start + 33] = 0;
    cfgData[start + 34] = 0;
    cfgData[start + 35] = 0;

    cfgData[start + 36] = 0;
    cfgData[start + 37] = 0;
    cfgData[start + 38] = 0;
    cfgData[start + 39] = 0;
}

/*
JESD Framer Init: JESD_FramerConfig_t
Size : 53 Bytes
*/
void adrv9025_CpuLoadFramerBytes(uint32_t                     start,
                                 uint8_t                      cfgData[],
                                 const adi_adrv9025_FrmCfg_t* framer)
{
    uint8_t newSysref = ((framer->newSysrefOnRelink > 0 ? 1 : 0) |
        ((framer->sysrefForStartup > 0 ? 1 : 0) << 1) |
        ((framer->sysrefNShotCount & 0x0F) << 2) |
        ((framer->sysrefNShotEnable > 0 ? 1 : 0) << 6) |
        ((framer->sysrefIgnoreWhenLinked > 0 ? 1 : 0) << 7));
    uint8_t offset = 0;

    cfgData[start + offset++] = framer->enableJesd204C;
    cfgData[start + offset++] = framer->jesd204E;
    cfgData[start + offset++] = framer->bankId;
    cfgData[start + offset++] = framer->deviceId;
    cfgData[start + offset++] = framer->lane0Id;
    cfgData[start + offset++] = framer->jesd204M;
    cfgData[start + offset++] = framer->jesd204K - 1;
    cfgData[start + offset++] = framer->jesd204F;
    cfgData[start + offset++] = framer->jesd204Np;
    cfgData[start + offset++] = framer->scramble;
    cfgData[start + offset++] = framer->serializerLanesEnabled;
    cfgData[start + offset++] = (framer->lmfcOffset & 0xFF);
    cfgData[start + offset++] = ((framer->lmfcOffset >> 8) & 0xFF);
    cfgData[start + offset++] = newSysref;
    cfgData[start + offset++] = framer->syncbInSelect;
    cfgData[start + offset++] = framer->overSample;
    cfgData[start + offset++] = framer->syncbInLvdsMode;
    cfgData[start + offset++] = framer->syncbInLvdsPnInvert;
    cfgData[start + offset++] = framer->serializerLaneCrossbar.lane0FramerOutSel;
    cfgData[start + offset++] = framer->serializerLaneCrossbar.lane1FramerOutSel;
    cfgData[start + offset++] = framer->serializerLaneCrossbar.lane2FramerOutSel;
    cfgData[start + offset++] = framer->serializerLaneCrossbar.lane3FramerOutSel;
    cfgData[start + offset++] = framer->adcCrossbar.conv0;
    cfgData[start + offset++] = framer->adcCrossbar.conv1;
    cfgData[start + offset++] = framer->adcCrossbar.conv2;
    cfgData[start + offset++] = framer->adcCrossbar.conv3;
    cfgData[start + offset++] = framer->adcCrossbar.conv4;
    cfgData[start + offset++] = framer->adcCrossbar.conv5;
    cfgData[start + offset++] = framer->adcCrossbar.conv6;
    cfgData[start + offset++] = framer->adcCrossbar.conv7;
    cfgData[start + offset++] = framer->adcCrossbar.conv8;
    cfgData[start + offset++] = framer->adcCrossbar.conv9;
    cfgData[start + offset++] = framer->adcCrossbar.conv10;
    cfgData[start + offset++] = framer->adcCrossbar.conv11;
    cfgData[start + offset++] = framer->adcCrossbar.conv12;
    cfgData[start + offset++] = framer->adcCrossbar.conv13;
    cfgData[start + offset++] = framer->adcCrossbar.conv14;
    cfgData[start + offset++] = framer->adcCrossbar.conv15;
    cfgData[start + offset++] = framer->adcCrossbar.conv16;
    cfgData[start + offset++] = framer->adcCrossbar.conv17;
    cfgData[start + offset++] = framer->adcCrossbar.conv18;
    cfgData[start + offset++] = framer->adcCrossbar.conv19;
    cfgData[start + offset++] = framer->adcCrossbar.conv20;
    cfgData[start + offset++] = framer->adcCrossbar.conv21;
    cfgData[start + offset++] = framer->adcCrossbar.conv22;
    cfgData[start + offset++] = framer->adcCrossbar.conv23;
    cfgData[start + offset++] = 0;
    cfgData[start + offset++] = 0;
    cfgData[start + offset++] = 0;
    cfgData[start + offset++] = 0;
    cfgData[start + offset++] = 0;
    cfgData[start + offset++] = 0;
    cfgData[start + offset++] = 0;
}

/*
JESD De-framer Init: Jesd204bDeframerConfig_t
Size : 38 Bytes
*/
void adrv9025_CpuLoadDeframerBytes(uint32_t                      start,
                                   uint8_t                       cfgData[],
                                   const adi_adrv9025_DfrmCfg_t* deframer)
{
    uint8_t newSysref = ((deframer->newSysrefOnRelink > 0 ? 1 : 0) |
        ((deframer->sysrefForStartup > 0 ? 1 : 0) << 1) |
        ((deframer->sysrefNShotCount & 0x0F) << 2) |
        ((deframer->sysrefNShotEnable > 0 ? 1 : 0) << 6) |
        ((deframer->sysrefIgnoreWhenLinked > 0 ? 1 : 0) << 7));
    uint8_t offset = 0;

    cfgData[start + offset++] = deframer->enableJesd204C;
    cfgData[start + offset++] = deframer->jesd204E;
    cfgData[start + offset++] = deframer->bankId;
    cfgData[start + offset++] = deframer->deviceId;
    cfgData[start + offset++] = deframer->lane0Id;
    cfgData[start + offset++] = deframer->jesd204M;
    cfgData[start + offset++] = deframer->jesd204K - 1;
    cfgData[start + offset++] = deframer->jesd204F;
    cfgData[start + offset++] = deframer->jesd204Np;
    cfgData[start + offset++] = deframer->scramble;
    cfgData[start + offset++] = deframer->deserializerLanesEnabled;
    cfgData[start + offset++] = (deframer->lmfcOffset & 0xFF);
    cfgData[start + offset++] = ((deframer->lmfcOffset >> 8) & 0xFF);
    cfgData[start + offset++] = newSysref;
    cfgData[start + offset++] = deframer->syncbOutSelect;
    cfgData[start + offset++] = deframer->syncbOutLvdsMode;
    cfgData[start + offset++] = deframer->syncbOutLvdsPnInvert;
    cfgData[start + offset++] = deframer->syncbOutCmosSlewRate;
    cfgData[start + offset++] = deframer->syncbOutCmosDriveLevel;
    cfgData[start + offset++] = deframer->deserializerLaneCrossbar.deframerInput0LaneSel;
    cfgData[start + offset++] = deframer->deserializerLaneCrossbar.deframerInput1LaneSel;
    cfgData[start + offset++] = deframer->deserializerLaneCrossbar.deframerInput2LaneSel;
    cfgData[start + offset++] = deframer->deserializerLaneCrossbar.deframerInput3LaneSel;
    cfgData[start + offset++] = deframer->dacCrossbar.tx1DacChanI;
    cfgData[start + offset++] = deframer->dacCrossbar.tx1DacChanQ;
    cfgData[start + offset++] = deframer->dacCrossbar.tx2DacChanI;
    cfgData[start + offset++] = deframer->dacCrossbar.tx2DacChanQ;
    cfgData[start + offset++] = deframer->dacCrossbar.tx3DacChanI;
    cfgData[start + offset++] = deframer->dacCrossbar.tx3DacChanQ;
    cfgData[start + offset++] = deframer->dacCrossbar.tx4DacChanI;
    cfgData[start + offset++] = deframer->dacCrossbar.tx4DacChanQ;
    cfgData[start + offset++] = 0;
    cfgData[start + offset++] = 0;
    cfgData[start + offset++] = 0;
    cfgData[start + offset++] = 0;
    cfgData[start + offset++] = 0;
    cfgData[start + offset++] = 0;
    cfgData[start + offset++] = 0;
}

/*
JESD Data Configuration: adi_adrv9025_JesdDataConfigCtrl_t
Size : 3 bytes
*/
void adrv9025_CpuLoadDataConfig(
    uint32_t                                 start,
    uint8_t                                  cfgData[],
    const adi_adrv9025_JesdDataConfigCtrl_t* dataCfg)
{
    cfgData[start + 0] = dataCfg->enable;
    cfgData[start + 1] = dataCfg->configOption1;
    cfgData[start + 2] = dataCfg->configOption2;
}

/*
JESD Link Sharing Init: adi_adrv9025_LinkSharingCfg_t
Size : 20 bytes
*/
void adrv9025_CpuLoadLinkSharing(uint32_t                             start,
                                 uint8_t                              cfgData[],
                                 const adi_adrv9025_LinkSharingCfg_t* linkSharing)
{
    cfgData[start + 0]  = linkSharing->linkSharingEnabled;
    cfgData[start + 1]  = linkSharing->linkSharingM;
    cfgData[start + 2]  = linkSharing->linkSharingS;
    cfgData[start + 3]  = linkSharing->linkSharingNp;
    cfgData[start + 4]  = linkSharing->linkSharingAdcCrossbar.conv0;
    cfgData[start + 5]  = linkSharing->linkSharingAdcCrossbar.conv1;
    cfgData[start + 6]  = linkSharing->linkSharingAdcCrossbar.conv2;
    cfgData[start + 7]  = linkSharing->linkSharingAdcCrossbar.conv3;
    cfgData[start + 8]  = linkSharing->linkSharingAdcCrossbar.conv4;
    cfgData[start + 9]  = linkSharing->linkSharingAdcCrossbar.conv5;
    cfgData[start + 10] = linkSharing->linkSharingAdcCrossbar.conv6;
    cfgData[start + 11] = linkSharing->linkSharingAdcCrossbar.conv7;
    cfgData[start + 12] = linkSharing->linkSharingAdcCrossbar.conv8;
    cfgData[start + 13] = linkSharing->linkSharingAdcCrossbar.conv9;
    cfgData[start + 14] = linkSharing->linkSharingAdcCrossbar.conv10;
    cfgData[start + 15] = linkSharing->linkSharingAdcCrossbar.conv11;
    cfgData[start + 16] = linkSharing->linkSharingAdcCrossbar.conv12;
    cfgData[start + 17] = linkSharing->linkSharingAdcCrossbar.conv13;
    cfgData[start + 18] = linkSharing->linkSharingAdcCrossbar.conv14;
    cfgData[start + 19] = linkSharing->linkSharingAdcCrossbar.conv15;
}

/*
JESD Framer Init: JESD_SerializerLane_t
Size : 8 Bytes
Element    Type    Range    Description
serAmplitude    uint8    0-15    Serializer amplitude setting. Default = 15.
serPreEmphasis    uint8    0-4    Serializer pre-emphasis setting. Default = 1.
serInvertLanePolarity    uint8
Reserved[5]    uint8        5 bytes reserved.
*/
void adrv9025_CpuLoadSerBytes(uint32_t                     start,
                              uint8_t                      cfgData[],
                              const adi_adrv9025_SerCfg_t* serdes)
{
    cfgData[start + 0] = serdes->serAmplitude;
    cfgData[start + 1] = serdes->serPreEmphasis;
    cfgData[start + 2] = serdes->serPostEmphasis;
    cfgData[start + 3] = serdes->serInvertLanePolarity;
    cfgData[start + 4] = 0;
    cfgData[start + 5] = 0;
    cfgData[start + 6] = 0;
    cfgData[start + 7] = 0;
}

/*
JESD Framer Init: JESD_DeserializerLane_t
Size : 44 Bytes
Element    Type    Range    Description
desInvertLanePolarity    uint8
desEqSetting[2]    uint8
Reserved[5]    uint8        5 bytes reserved.
Table 8 - JESD Desterilizer Lane Structure

*/
void adrv9025_CpuLoadDesBytes(uint32_t                     start,
                              uint8_t                      cfgData[],
                              const adi_adrv9025_DesCfg_t* serdes)
{
    uint8_t offset = 0;

    cfgData[start + offset++] = serdes->highBoost;
    cfgData[start + offset++] = serdes->desInvertLanePolarity;
    cfgData[start + offset++] = 0; // reserved for memory alignment
    cfgData[start + offset++] = 0; // reserved for memory alignment

    adrv9025_CpuLoadFourBytes(start + offset,
                              cfgData,
                              serdes->configOption1);
    offset = offset + 4;
    adrv9025_CpuLoadFourBytes(start + offset,
                              cfgData,
                              serdes->configOption2);
    offset = offset + 4;
    adrv9025_CpuLoadFourBytes(start + offset,
                              cfgData,
                              serdes->configOption3);
    offset = offset + 4;
    adrv9025_CpuLoadFourBytes(start + offset,
                              cfgData,
                              serdes->configOption4);
    offset = offset + 4;
    adrv9025_CpuLoadFourBytes(start + offset,
                              cfgData,
                              serdes->configOption5);
    offset = offset + 4;
    adrv9025_CpuLoadFourBytes(start + offset,
                              cfgData,
                              serdes->configOption6);
    offset = offset + 4;
    adrv9025_CpuLoadFourBytes(start + offset,
                              cfgData,
                              serdes->configOption7);
    offset = offset + 4;
    adrv9025_CpuLoadFourBytes(start + offset,
                              cfgData,
                              serdes->configOption8);
    offset = offset + 4;
    adrv9025_CpuLoadFourBytes(start + offset,
                              cfgData,
                              serdes->configOption9);
    offset = offset + 4;
    adrv9025_CpuLoadFourBytes(start + offset,
                              cfgData,
                              serdes->configOption10);
    offset = offset + 4;
}

/*
Noise Cancel Config Init: noiseCancel_t
Size : 36 Bytes
Element    Type    Range    Description
Start Frequency 1    uint32_t        Start of Noise Cancel band 1
End Frequency 1    uint32_t        End of Noise Cancel band 1
Start Frequency 2    uint32_t        Start of Noise Cancel band 2
End Frequency 2    uint32_t        End of Noise Cancel band 2
Start Frequency 3    uint32_t        Start of Noise Cancel band 3
End Frequency 3    uint32_t        End of Noise Cancel band 3
Flip    uint32_t        Flip Channel 1/2
Reserved[8]    uint8        8 bytes reserved.

Table 6 - Noise Cancellation Config Structure
*/
void adrv9025_CpuLoadNoiseCancelBytes(uint32_t start,
                                      uint8_t  cfgData[])
{
    adrv9025_CpuLoadFourBytes(start + 0,
                              cfgData,
                              0);
    adrv9025_CpuLoadFourBytes(start + 4,
                              cfgData,
                              0);
    adrv9025_CpuLoadFourBytes(start + 8,
                              cfgData,
                              0);
    adrv9025_CpuLoadFourBytes(start + 12,
                              cfgData,
                              0);
    adrv9025_CpuLoadFourBytes(start + 16,
                              cfgData,
                              0);
    adrv9025_CpuLoadFourBytes(start + 20,
                              cfgData,
                              0);
    adrv9025_CpuLoadFourBytes(start + 24,
                              cfgData,
                              0);
    adrv9025_CpuLoadFourBytes(start + 28,
                              cfgData,
                              0);
    adrv9025_CpuLoadFourBytes(start + 32,
                              cfgData,
                              0);
}

static void adrv9025_cfgDataSet(uint8_t cfgData[],
                                uint8_t value,
                                int     count)
{
    int i = 0;
    for (i = 0; i < count; i++)
    {
        cfgData[i] = value;
    }
}

void adrv9025_CpuLoadTwoBytes(uint32_t       start,
                              uint8_t        cfgData[],
                              const uint16_t data)
{
    cfgData[start]     = (uint8_t)(data & 0x00FF);
    cfgData[start + 1] = (uint8_t)((data >> 8) & 0x00FF);
}

void adrv9025_CpuLoadFourBytes(uint32_t       start,
                               uint8_t        cfgData[],
                               const uint32_t data)
{
    cfgData[start]     = (uint8_t)(data & 0x000000FF);
    cfgData[start + 1] = (uint8_t)((data >> 8) & 0x000000FF);
    cfgData[start + 2] = (uint8_t)((data >> 16) & 0x000000FF);
    cfgData[start + 3] = (uint8_t)((data >> 24) & 0x000000FF);
}

void adrv9025_CpuLoadAdcBytes(uint32_t        start,
                              uint8_t         cfgData[],
                              const uint16_t* data)
{
    uint32_t i = 0;

    adrv9025_CpuLoadTwoBytes(start + 0,
                             &cfgData[0],
                             data[0]);
    adrv9025_CpuLoadTwoBytes(start + 2,
                             &cfgData[0],
                             data[1]);
    adrv9025_CpuLoadTwoBytes(start + 4,
                             &cfgData[0],
                             data[2]);
    adrv9025_CpuLoadTwoBytes(start + 6,
                             &cfgData[0],
                             data[3]);

    cfgData[start + 8] = (uint8_t)(data[4] & 0x00FF); //Loop filter R4.
    cfgData[start + 9] = 0;

    adrv9025_CpuLoadTwoBytes(start + 10,
                             &cfgData[0],
                             data[5]);
    adrv9025_CpuLoadTwoBytes(start + 12,
                             &cfgData[0],
                             data[6]);

    cfgData[start + 14] = (uint8_t)(data[7] & 0x00FF); //Loop filter C3.
    cfgData[start + 15] = (uint8_t)(data[8] & 0x00FF); //Feedback ILSB3.

    adrv9025_CpuLoadTwoBytes(start + 16,
                             &cfgData[0],
                             data[9]);

    /* All 1 byte */
    for (i = 0; (i + 18) < ADRV9025_CPU_ADC_PROFILE_SIZE; i++)
    {
        cfgData[start + 18 + i] = (uint8_t)(data[10 + i] & 0x00FF);
    }
}

void adrv9025_CpuDataSet(
    uint8_t* cfgData,
    uint8_t  value,
    int      count)
{
    int i = 0;
    for (i = 0; i < count; i++)
    {
        cfgData[i] = value;
    }
}

enum adrv9025_CpuErrorFlag adrv9025_CpuErrorFlagGet(uint32_t err)
{
    enum adrv9025_CpuErrorFlag result = ADRV9025_CPU_ERROR_FLAG_INVALID;

    switch (err)
    {
        /* CPU Error flag = 1 */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_RUNINIT_OPCODE,
                                  0,
                                  1): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_CPU_EXCEPTION,
                                  1): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_CONFIG,
                                  1): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_CONFIG,
                                  1): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_MCS,
                                  1): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_MCS,
                                  1): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_TRACKING_CAL_ENABLE,
                                  1): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_TRACKING_CAL_ENABLE,
                                  1): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_TX_ORX_MAPPING,
                                  1): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_TX_ORX_MAPPING,
                                  1): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_TXCAL_ENA_DIS,
                                  1): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_TXCAL_ENA_DIS,
                                  1): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_RFPLL_LO_FREQUENCY,
                                  1): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_PLL_LOOP_BW,
                                  1): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_PLL_LOOP_BW,
                                  1): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_TEMP_SENSOR,
                                  1): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_GPIO_CTRL,
                                  1): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_GPIO_CTRL,
                                  1): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_EXT_DPD_MODEL_SELECT,
                                  1): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_EXT_DPD_MODEL_SELECT,
                                  1): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_EXT_DPD_ENABLE_DISABLE,
                                  1): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_EXT_DPD_CAPTURE_TRIG,
                                  1): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_EXT_DPD_CAPTURE_DONE,
                                  1): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_EXT_DPD_CAPTURE_DONE,
                                  1): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_TRACKING_CAL_CTRL,
                                  1): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_SRL_CONFIG,
                                  1): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_SRL_CONFIG,
                                  1): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_SRL_STATISTICS,
                                  1):
        result = ADRV9025_CPU_ERROR_FLAG_1;
        break;

        /* CPU Error flag = 2 */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_RUNINIT_OPCODE,
                                  0,
                                  2): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_CPU_EXCEPTION,
                                  2): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_CONFIG,
                                  2): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_CONFIG,
                                  2): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_MCS,
                                  2): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_MCS,
                                  2): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_TRACKING_CAL_ENABLE,
                                  2): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_TRACKING_CAL_ENABLE,
                                  2): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_TX_ORX_MAPPING,
                                  2): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_TX_ORX_MAPPING,
                                  2): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_TXCAL_ENA_DIS,
                                  2): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_TXCAL_ENA_DIS,
                                  2): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_RFPLL_LO_FREQUENCY,
                                  2): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_RFPLL_LO_FREQUENCY,
                                  2): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_PLL_LOOP_BW,
                                  2): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_PLL_LOOP_BW,
                                  2): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_TEMP_SENSOR,
                                  2): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_GPIO_CTRL,
                                  2): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_GPIO_CTRL,
                                  2): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_EXT_DPD_MODEL_SELECT,
                                  2): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_EXT_DPD_MODEL_SELECT,
                                  2): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_EXT_DPD_ENABLE_DISABLE,
                                  2): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_EXT_DPD_CAPTURE_TRIG,
                                  2): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_EXT_DPD_CAPTURE_DONE,
                                  2): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_EXT_DPD_CAPTURE_DONE,
                                  2): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_TRACKING_CAL_CTRL,
                                  2):
        result = ADRV9025_CPU_ERROR_FLAG_2;
        break;

        /* CPU Error flag = 3 */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_RUNINIT_OPCODE,
                                  0,
                                  3): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_CPU_EXCEPTION,
                                  3): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_CONFIG,
                                  3): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_CONFIG,
                                  3): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_MCS,
                                  3): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_MCS,
                                  3): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_TRACKING_CAL_ENABLE,
                                  3): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_TRACKING_CAL_ENABLE,
                                  3): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_TX_ORX_MAPPING,
                                  3): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_TX_ORX_MAPPING,
                                  3): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_TXCAL_ENA_DIS,
                                  3): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_TXCAL_ENA_DIS,
                                  3): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_RFPLL_LO_FREQUENCY,
                                  3): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_RFPLL_LO_FREQUENCY,
                                  3): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_PLL_LOOP_BW,
                                  3): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_PLL_LOOP_BW,
                                  3): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_TEMP_SENSOR,
                                  3): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_GPIO_CTRL,
                                  3): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_GPIO_CTRL,
                                  3): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_EXT_DPD_MODEL_SELECT,
                                  3): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_EXT_DPD_MODEL_SELECT,
                                  3): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_EXT_DPD_ENABLE_DISABLE,
                                  3): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_EXT_DPD_CAPTURE_TRIG,
                                  3): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_EXT_DPD_CAPTURE_DONE,
                                  3): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_EXT_DPD_CAPTURE_DONE,
                                  3): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_TRACKING_CAL_CTRL,
                                  3):
        result = ADRV9025_CPU_ERROR_FLAG_3;
        break;

        /* CPU Error flag = 4 */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_RUNINIT_OPCODE,
                                  0,
                                  4): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_CPU_EXCEPTION,
                                  4): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_CONFIG,
                                  4): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_CONFIG,
                                  4): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_MCS,
                                  4): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_MCS,
                                  4): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_TRACKING_CAL_ENABLE,
                                  4): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_TRACKING_CAL_ENABLE,
                                  4): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_TX_ORX_MAPPING,
                                  4): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_TX_ORX_MAPPING,
                                  4): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_TXCAL_ENA_DIS,
                                  4): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_TXCAL_ENA_DIS,
                                  4): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_RFPLL_LO_FREQUENCY,
                                  4): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_RFPLL_LO_FREQUENCY,
                                  4): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_PLL_LOOP_BW,
                                  4): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_PLL_LOOP_BW,
                                  4): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_GPIO_CTRL,
                                  4): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_GPIO_CTRL,
                                  4): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_EXT_DPD_MODEL_SELECT,
                                  4): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_EXT_DPD_MODEL_SELECT,
                                  4): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_EXT_DPD_ENABLE_DISABLE,
                                  4): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_EXT_DPD_CAPTURE_TRIG,
                                  4): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_EXT_DPD_CAPTURE_DONE,
                                  4): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_EXT_DPD_CAPTURE_DONE,
                                  4): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_TRACKING_CAL_CTRL,
                                  4):
        result = ADRV9025_CPU_ERROR_FLAG_4;
        break;

        /* CPU Error flag = 5 */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_RUNINIT_OPCODE,
                                  0,
                                  5): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_CPU_EXCEPTION,
                                  5): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_CONFIG,
                                  5): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_CONFIG,
                                  5): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_MCS,
                                  5): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_MCS,
                                  5): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_TRACKING_CAL_ENABLE,
                                  5): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_TRACKING_CAL_ENABLE,
                                  5): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_TX_ORX_MAPPING,
                                  5): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_TX_ORX_MAPPING,
                                  5): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_TXCAL_ENA_DIS,
                                  5): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_TXCAL_ENA_DIS,
                                  5): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_RFPLL_LO_FREQUENCY,
                                  5): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_RFPLL_LO_FREQUENCY,
                                  5): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_PLL_LOOP_BW,
                                  5): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_PLL_LOOP_BW,
                                  5): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_GPIO_CTRL,
                                  5): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_GPIO_CTRL,
                                  5): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_EXT_DPD_MODEL_SELECT,
                                  5): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_EXT_DPD_MODEL_SELECT,
                                  5): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_EXT_DPD_ENABLE_DISABLE,
                                  5): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_EXT_DPD_CAPTURE_TRIG,
                                  5): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_EXT_DPD_CAPTURE_DONE,
                                  5): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_EXT_DPD_CAPTURE_DONE,
                                  5): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_TRACKING_CAL_CTRL,
                                  5):
        result = ADRV9025_CPU_ERROR_FLAG_5;
        break;

        /* CPU Error flag = 6 */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_RUNINIT_OPCODE,
                                  0,
                                  6): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_CPU_EXCEPTION,
                                  6): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_CONFIG,
                                  6): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_CONFIG,
                                  6): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_MCS,
                                  6): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_MCS,
                                  6): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_TRACKING_CAL_ENABLE,
                                  6): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_TRACKING_CAL_ENABLE,
                                  6): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_TX_ORX_MAPPING,
                                  6): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_TX_ORX_MAPPING,
                                  6): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_TXCAL_ENA_DIS,
                                  6): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_TXCAL_ENA_DIS,
                                  6): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_RFPLL_LO_FREQUENCY,
                                  6): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_RFPLL_LO_FREQUENCY,
                                  6): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_PLL_LOOP_BW,
                                  6): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_PLL_LOOP_BW,
                                  6): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_GPIO_CTRL,
                                  6): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_GPIO_CTRL,
                                  6): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_EXT_DPD_MODEL_SELECT,
                                  6): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_EXT_DPD_MODEL_SELECT,
                                  6): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_EXT_DPD_ENABLE_DISABLE,
                                  6): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_EXT_DPD_CAPTURE_TRIG,
                                  6): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_EXT_DPD_CAPTURE_DONE,
                                  6): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_EXT_DPD_CAPTURE_DONE,
                                  6): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_TRACKING_CAL_CTRL,
                                  6):
        result = ADRV9025_CPU_ERROR_FLAG_6;
        break;

        /* CPU Error flag = 7 */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_RUNINIT_OPCODE,
                                  0,
                                  7): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_CONFIG,
                                  7): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_CONFIG,
                                  7): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_RFPLL_LO_FREQUENCY,
                                  7): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_RFPLL_LO_FREQUENCY,
                                  7): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_PLL_LOOP_BW,
                                  7): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_PLL_LOOP_BW,
                                  7): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_GPIO_CTRL,
                                  7): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_GPIO_CTRL,
                                  7): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_CPU_EXCEPTION,
                                  7): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_MCS,
                                  7): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_MCS,
                                  7): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_TRACKING_CAL_ENABLE,
                                  7): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_TRACKING_CAL_ENABLE,
                                  7): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_TX_ORX_MAPPING,
                                  7): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_TX_ORX_MAPPING,
                                  7): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_TXCAL_ENA_DIS,
                                  7): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_TXCAL_ENA_DIS,
                                  7): /* Fall through */
    case ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                  ADRV9025_CPU_OBJECTID_TRACKING_CAL_CTRL,
                                  7):
        result = ADRV9025_CPU_ERROR_FLAG_7;
        break;

    default:
        result = ADRV9025_CPU_ERROR_FLAG_INVALID;
        break;
    }

    return result;
};

const char* adrv9025_CpuErrMsgGet(
    adi_adrv9025_Device_t* device,
    adi_adrv9025_CpuType_e cpuType,
    uint32_t               errCode,
    uint16_t*              mailboxErrCodeRet)
{
    int32_t              recoveryAction   = ADI_COMMON_ACT_NO_ACTION;
    static const uint8_t ERR_FLAG_MASK    = 0x0E;
    static const uint8_t ERR_FLAG_SHIFT   = 1;
    static const uint8_t ERR_FLAG_MAILBOX = 7;
    uint32_t             cpuErrorFlag     = ADRV9025_BF_DECODE(errCode,
                                                               ERR_FLAG_MASK,
                                                               ERR_FLAG_SHIFT);
    uint16_t mailboxErrCode = 0;

    if (cpuErrorFlag == ERR_FLAG_MAILBOX)
    {
        /*Clear the error so that CPU Mailbox functions execute*/
        recoveryAction = adrv9025_ErrorClear(&device->common);
        if (recoveryAction != ADI_COMMON_ACT_NO_ACTION)
        {
            return "Error clearing MailBox Error Code.\n";
        }

        recoveryAction = adrv9025_CpuMailBoxErrCodeGet(device,
                                                       cpuType,
                                                       &mailboxErrCode);
        if (recoveryAction != ADI_COMMON_ACT_NO_ACTION)
        {
            return "Error getting MailBox Error Code.\n";
        }

        *mailboxErrCodeRet = mailboxErrCode;
    }

    errCode = ((errCode & (ADRV9025_CPU_OPCODE_MASK | ADRV9025_CPU_OBJ_ID_MASK)) | cpuErrorFlag);

    if (cpuErrorFlag > 0)
    {
        switch (adrv9025_CpuErrorFlagGet(errCode))
        {
        case ADRV9025_CPU_ERROR_FLAG_1:
            return "Detected nested ADRV9025 CPU commands, second command ignored\n";
        case ADRV9025_CPU_ERROR_FLAG_2:
            return "ADRV9025 CPU Command is not supported\n";
        case ADRV9025_CPU_ERROR_FLAG_3:
            return "ADRV9025 CPU Invalid State\n";
        case ADRV9025_CPU_ERROR_FLAG_4:
            return "ADRV9025 CPU Reserved 4\n";
        case ADRV9025_CPU_ERROR_FLAG_5:
            return "ADRV9025 CPU Reserved 5\n";
        case ADRV9025_CPU_ERROR_FLAG_6:
            return "ADRV9025 CPU Reserved 6\n";
        case ADRV9025_CPU_ERROR_FLAG_7:
            return "ADRV9025 CPU Command Error\n";
        default:
            return "Invalid CPU error passed, not in error list\n";
        }
    }
    return "Wrong error handler - not a ADRV9025 CPU error\n";
}

int32_t adrv9025_CpuMailBoxErrCodeGet(
    adi_adrv9025_Device_t* device,
    adi_adrv9025_CpuType_e cpuType,
    uint16_t*              mailboxErrCode)
{
    /* reading the command status register for given opcode */
    uint8_t                 errCode = 0;
    uint8_t                 objId   = 0;
    adi_adrv9025_CpuAddr_t* cpuAddr = NULL;

    cpuAddr = adrv9025_CpuAddrGet(&device->devStateInfo.cpu,
                                  cpuType);
    ADI_NULL_PTR_RETURN(&device->common,
                        cpuAddr);

    ADRV9025_SPIREADBYTE("CPU_CMD_STATUS_10",
                         cpuAddr->cmdStatusAddr + 10u,
                         &errCode);
    ADRV9025_SPIREADBYTE("CPU_CMD_STATUS_11",
                         cpuAddr->cmdStatusAddr + 11u,
                         &objId);

    *mailboxErrCode = ((uint16_t)objId << 8) | errCode;
    return (device->common.error.newAction);
}

int32_t adrv9025_CpuSystemErrCodeGet(adi_adrv9025_Device_t* device,
                                     adi_adrv9025_CpuType_e cpuType,
                                     uint16_t*              systemErrCode)
{
    /* Reading the ARM system error code and ObjID */
    uint8_t                 errCode = 0;
    uint8_t                 objId   = 0;
    adi_adrv9025_CpuAddr_t* cpuAddr = NULL;

    cpuAddr = adrv9025_CpuAddrGet(&device->devStateInfo.cpu,
                                  cpuType);
    ADI_NULL_PTR_RETURN(&device->common,
                        cpuAddr);

    ADRV9025_SPIREADBYTE("CPU_CMD_STATUS_10",
                         cpuAddr->cmdStatusAddr + 12u,
                         &errCode);
    ADRV9025_SPIREADBYTE("CPU_CMD_STATUS_11",
                         cpuAddr->cmdStatusAddr + 13u,
                         &objId);

    *systemErrCode = ((uint16_t)objId << 8) | errCode;
    return (device->common.error.newAction);
}

int32_t adrv9025_CpuDmaMemReadByte(
    adi_adrv9025_Device_t* device,
    uint32_t               address,
    uint8_t                returnData[],
    uint32_t               byteCount,
    uint8_t                autoIncrement)
{
    uint32_t i              = 0;
    uint8_t  dataRead0      = 0;
    int32_t  recoveryAction = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        returnData);

    ADRV9025_BUGINFO(__FUNCTION__);

    ADRV9025_DMAINFO("CPU_MEM_READ",
                     address,
                     byteCount);

    if (byteCount == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_NULL_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "byteCount is zero");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* starting write at zero address offset */
    for (i = 0; i < byteCount; i++)
    {
        /* Always read byte0 even if target byte is not located on full word boundary */
        ADRV9025_SPIREADBYTEDMA("CPU_DMA_DATA_0+",
                                ADRV9025_CPU_ADDR_DMA_DATA0,
                                &dataRead0);
        returnData[i] = dataRead0;

        if (autoIncrement == ADI_FALSE)
        {
            address++;
            ADRV9025_SPIWRITEWORDDMA("CPU_DMA_ADDR",
                                     ADRV9025_CPU_ADDR_DMA_ADDR3,
                                     address);
        }
    }

    return (device->common.error.newAction);
}

int32_t adrv9025_CpuDmaMemWriteByte(
    adi_adrv9025_Device_t* device,
    uint32_t               address,
    const uint8_t          data[],
    uint32_t               byteCount,
    uint8_t                autoInc)
{
    uint32_t i              = 0;
    int32_t  recoveryAction = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        data);

    ADRV9025_BUGINFO(__FUNCTION__);

    ADRV9025_DMAINFO("CPU_MEM_WRITE",
                     address,
                     byteCount);

    if (byteCount == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_NULL_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         byteCount,
                         "byteCount is zero");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    for (i = 0; i < byteCount; i++)
    {
        /* Always write to byte0 even if target byte is not at full word boundary */
        ADRV9025_SPIWRITEBYTEDMA("CPU_DMA_DATA_0+",
                                 ADRV9025_CPU_ADDR_DMA_DATA0,
                                 data[i]);

        if (autoInc == ADI_FALSE)
        {
            address++;
            /* core_bf.bus_addr.write(bf_status, 32'hxxxxxxxx); */
            ADRV9025_SPIWRITEWORDDMA("CPU_DMA_ADDR",
                                     ADRV9025_CPU_ADDR_DMA_ADDR3,
                                     address);
        }
    }

    return (device->common.error.newAction);
}

int32_t adrv9025_CpuDmaSetup(adi_adrv9025_Device_t* device,
                             uint32_t               address,
                             uint8_t                isWrite,
                             uint8_t                isByte,
                             uint8_t                autoInc)
{
    uint8_t reg            = 0u;
    int32_t recoveryAction = 0;
    /*
    Use SPI DMA for Write
    */
    /* Write */
    /* 1 = read, 0 = write */
    /* core_bf.rd_wrb.write(bf_status, 1'b1); */
    if (isWrite == ADI_FALSE)
    {
        reg |= ADRV9025_CPU_MASK_DMA_CTL_RD_WRB;
    }
    else
    {
        reg &= ~ADRV9025_CPU_MASK_DMA_CTL_RD_WRB;
    }

    /* system AHB bus slave or code AHB bus slave */
    /* 1'b0=code bus; 1'b1=system bus */
    /* core_bf.sys_codeb.write(bf_status, 1'b1); */
    /* determining the data space and setting dataMem bit accordingly */
    if (adrv9025_CpuIsAddrInProgRegion(&device->devStateInfo.cpu,
                                       address) == ADI_FALSE)
    {
        reg |= ADRV9025_CPU_MASK_DMA_CTL_SYS_CODEB;
    }

    /* bus size, 2'b00=byte; 2'b01=half-word; 2'b10=full-word; 2'b11=invalid */
    /* core_bf.bus_size.write(bf_status, 2'b10); */
    reg |= ADRV9025_BF_ENCODE((isByte == ADI_FALSE)? 2:0,
                              ADRV9025_CPU_MASK_DMA_CTL_BUS_SIZE,
                              ADRV9025_CPU_SHIFT_DMA_CTL_BUS_SIZE);

    /* address auto incremental, 1'b0=no; 1'b1=yes */
    /* core_bf.auto_incr.write(bf_status, 1'b0); */
    if (autoInc != ADI_FALSE)
    {
        reg |= ADRV9025_CPU_MASK_DMA_CTL_AUTO_INCR;
    }

    /* spi_dma_ahb_bus_select */
    /* 1 = CPU D, 0 = CPU. */
    /* When 1, SPI DMA can access resources connected to CPU D's AHB subsystem. */
    if (adrv9025_CpuWhichTypeAddressBelongs(&device->devStateInfo.cpu,
                                            address) == ADI_ADRV9025_CPU_TYPE_D)
    {
        reg |= ADRV9025_CPU_MASK_DMA_CTL_BUS_SELECT;
    }

    /* setting up the DMA control register for a write */
    ADRV9025_SPIWRITEBYTEDMA("CPU_DMA_CTL",
                             ADRV9025_CPU_ADDR_DMA_CTL,
                             reg);

    /* core_bf.bus_addr.write(bf_status, 32'hxxxxxxxx); */
    ADRV9025_SPIWRITEWORDDMA("CPU_DMA_ADDR",
                             ADRV9025_CPU_ADDR_DMA_ADDR3,
                             address);

    return (device->common.error.newAction);
}

adi_adrv9025_CpuType_e adrv9025_CpuWhichTypeAddressBelongs(
    adi_adrv9025_Cpu_t* cpu,
    uint32_t            addr)
{
    adi_adrv9025_CpuType_e  result  = ADI_ADRV9025_CPU_TYPE_UNKNOWN;
    adi_adrv9025_CpuAddr_t* cpuAddr = NULL;
    uint8_t                 i       = 0u;

    for (i = 0u; i < ADRV9025_MAX_CPUS && result == ADI_ADRV9025_CPU_TYPE_UNKNOWN; i++)
    {
        cpuAddr = &cpu->cpuAddr[i];
        if ((addr >= cpuAddr->progStartAddr && addr <= cpuAddr->progEndAddr) ||
            (addr >= cpuAddr->dataStartAddr && addr <= cpuAddr->dataEndAddr))
        {
            result = (adi_adrv9025_CpuType_e)i;
        }
    }
    return result;
}

uint8_t adrv9025_CpuIsAddrInProgRegion(
    adi_adrv9025_Cpu_t* cpu,
    uint32_t            addr)
{
    adi_adrv9025_CpuAddr_t* cpuAddr = NULL;
    uint8_t                 result  = ADI_FALSE;
    uint8_t                 i       = 0u;

    for (i = 0; i < ADRV9025_MAX_CPUS && result == ADI_FALSE; i++)
    {
        cpuAddr = &cpu->cpuAddr[i];
        if (addr >= cpuAddr->progStartAddr && addr <= cpuAddr->progEndAddr)
        {
            result = ADI_TRUE;
        }
    }

    return result;
}

void adrv9025_CpuAddrInitialize(
    adi_adrv9025_CpuAddr_t* cpuAddr,
    adi_adrv9025_CpuType_e  type)
{
    if (adrv9025_cpuTypeIsValid(type) != 0)
    {
        switch (type)
        {
        case ADI_ADRV9025_CPU_TYPE_C:
            cpuAddr->enabled = ADI_TRUE;
            cpuAddr->ctlAddr            = ADRV9025_CPU_C_ADDR_CTL_1;
            cpuAddr->bootAddr           = ADRV9025_CPU_C_ADDR_BOOT_ADDR_BYTE0;
            cpuAddr->stackPtrAddr       = ADRV9025_CPU_C_ADDR_STACK_PTR_BYTE0;
            cpuAddr->cmdAddr            = ADRV9025_CPU_C_ADDR_COMMAND;
            cpuAddr->extCmdAddr         = ADRV9025_CPU_C_ADDR_EXT_CMD_BYTE_1;
            cpuAddr->cmdStatusAddr      = ADRV9025_CPU_C_ADDR_CMD_STATUS_0;
            cpuAddr->progStartAddr      = ADRV9025_CPU_C_ADDR_PROG_START;
            cpuAddr->progEndAddr        = ADRV9025_CPU_C_ADDR_PROG_END;
            cpuAddr->versionAddr        = ADRV9025_CPU_C_ADDR_VERSION;
            cpuAddr->structChecksumAddr = ADRV9025_CPU_C_ADDR_CALC_CHKSUM_PTR;
            cpuAddr->debugPointersAddr  = ADRV9025_CPU_C_ADDR_DEBUG_PTR;
            cpuAddr->buildChecksumAddr  = ADRV9025_CPU_C_ADDR_BUILD_CHKSUM;
            cpuAddr->dataStartAddr      = ADRV9025_CPU_C_ADDR_DATA_START;
            cpuAddr->dataEndAddr        = ADRV9025_CPU_C_ADDR_DATA_END;
            cpuAddr->cfrPulseAddr       = ADRV9025_CPU_C_ADDR_CFR_PULSE;
            cpuAddr->mailboxSetAddr     = ADRV9025_CPU_C_ADDR_MAILBOX_SET;
            cpuAddr->mailboxGetAddr     = ADRV9025_CPU_C_ADDR_MAILBOX_GET;
            cpuAddr->mailboxRunInitAddr = ADRV9025_CPU_C_ADDR_MAILBOX_RUN_INIT;
            cpuAddr->mailboxFhmAddr     = ADRV9025_CPU_C_ADDR_MAILBOX_FHM;
            break;
        case ADI_ADRV9025_CPU_TYPE_D:
            cpuAddr->enabled = ADI_FALSE;
            cpuAddr->ctlAddr            = ADRV9025_CPU_D_ADDR_CTL_1;
            cpuAddr->bootAddr           = ADRV9025_CPU_D_ADDR_BOOT_ADDR_BYTE0;
            cpuAddr->stackPtrAddr       = ADRV9025_CPU_D_ADDR_STACK_PTR_BYTE0;
            cpuAddr->cmdAddr            = ADRV9025_CPU_D_ADDR_COMMAND;
            cpuAddr->extCmdAddr         = ADRV9025_CPU_D_ADDR_EXT_CMD_BYTE_1;
            cpuAddr->cmdStatusAddr      = ADRV9025_CPU_D_ADDR_CMD_STATUS_0;
            cpuAddr->progStartAddr      = ADRV9025_CPU_D_ADDR_PROG_START;
            cpuAddr->progEndAddr        = ADRV9025_CPU_D_ADDR_PROG_END;
            cpuAddr->versionAddr        = ADRV9025_CPU_D_ADDR_VERSION;
            cpuAddr->structChecksumAddr = ADRV9025_CPU_D_ADDR_CALC_CHKSUM_PTR;
            cpuAddr->debugPointersAddr  = ADRV9025_CPU_D_ADDR_DEBUG_PTR;
            cpuAddr->buildChecksumAddr  = ADRV9025_CPU_D_ADDR_BUILD_CHKSUM;
            cpuAddr->dataStartAddr      = ADRV9025_CPU_D_ADDR_DATA_START;
            cpuAddr->dataEndAddr        = ADRV9025_CPU_D_ADDR_DATA_END;
            cpuAddr->cfrPulseAddr       = ADRV9025_CPU_D_ADDR_CFR_PULSE;
            cpuAddr->mailboxSetAddr     = ADRV9025_CPU_D_ADDR_MAILBOX_SET;
            cpuAddr->mailboxGetAddr     = ADRV9025_CPU_D_ADDR_MAILBOX_GET;
            cpuAddr->mailboxRunInitAddr = ADRV9025_CPU_D_ADDR_MAILBOX_RUN_INIT;
            cpuAddr->mailboxFhmAddr     = ADRV9025_CPU_D_ADDR_MAILBOX_FHM;
            break;
        default:
            break;
        }
    }
}

uint8_t adrv9025_cpuTypeIsValid(adi_adrv9025_CpuType_e type)
{
    uint8_t result = ADI_FALSE;

    if (type > ADI_ADRV9025_CPU_TYPE_UNKNOWN &&
        type < ADI_ADRV9025_CPU_TYPE_MAX)
    {
        result = ADI_TRUE;
    }

    return result;
}

/*****************************************************************************/
/***** Public functions' definition ******************************************/
/*****************************************************************************/
adi_adrv9025_CpuAddr_t* adrv9025_CpuAddrGet(
    adi_adrv9025_Cpu_t*    cpu,
    adi_adrv9025_CpuType_e cpuType)
{
    return &cpu->cpuAddr[cpuType];
}

void adrv9025_CpuInitialize(
    adi_adrv9025_Device_t* device)
{
    adi_adrv9025_Cpu_t* cpu = &device->devStateInfo.cpu;
    uint8_t             i   = 0;

    cpu->devProfileAddr = ADRV9025_CPU_C_ADDR_DEV_PROFILE;
    cpu->adcProfileAddr = ADRV9025_CPU_C_ADDR_ADC_PROFILE;
    for (i = 0; i < ADRV9025_MAX_CPUS; i++)
    {
        adrv9025_CpuAddrInitialize(&cpu->cpuAddr[i],
                                   (adi_adrv9025_CpuType_e)i);
    }
}

uint8_t adrv9025_CpuValidateMemoryAddr(
    adi_adrv9025_Cpu_t* cpu,
    uint32_t            addr,
    uint32_t            size)
{
    adi_adrv9025_CpuAddr_t* cpuAddr = NULL;
    uint8_t                 isValid = ADI_FALSE;
    uint8_t                 i       = 0u;
    uint32_t                endAddr = addr + size - 1u;

    for (i = 0; i < ADRV9025_MAX_CPUS && isValid == ADI_FALSE; i++)
    {
        cpuAddr = &cpu->cpuAddr[i];
        if ((addr >= cpuAddr->progStartAddr && endAddr <= cpuAddr->progEndAddr) ||
            (addr >= cpuAddr->dataStartAddr && endAddr <= cpuAddr->dataEndAddr))
        {
            isValid = ADI_TRUE;
        }
    }

    return isValid;
}

int32_t adrv9025_CpuDmaMemWrite(
    adi_adrv9025_Device_t* device,
    uint32_t               address,
    const uint8_t          data[],
    uint32_t               byteCount,
    uint8_t                autoIncrement)
{
    static const uint32_t ADDR_CPU_DMA_DATA[4] =
    {
        ADRV9025_CPU_ADDR_DMA_DATA3,
        ADRV9025_CPU_ADDR_DMA_DATA2,
        ADRV9025_CPU_ADDR_DMA_DATA1,
        ADRV9025_CPU_ADDR_DMA_DATA0
    };
    static const uint32_t WORD_LENGTH = 4;
    static const uint8_t  cacheEnable = ADI_ADRV9025_CPU_MEMWRITE_CACHE_ENABLE;

    int32_t  recoveryAction                               = 0;
    uint32_t i                                            = 0;
    uint32_t addrIndex                                    = 0;
    uint32_t dataIndex                                    = 0;
    uint16_t addrArray[(ADI_COMMON_HAL_SPIARRAYSIZE / 3)] = {0};
    uint8_t  dataArray[(ADI_COMMON_HAL_SPIARRAYSIZE / 3)] = {0};
    uint32_t spiBufferSize                                = (uint32_t)(sizeof(addrArray) / sizeof(addrArray[0]));
    uint32_t index                                        = 0;
    uint8_t  isByteWrite                                  = ADI_FALSE;
    uint8_t  addrArrayStream[4]                           = {0};

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        data);

    ADRV9025_DMAINFO("CPU_MEM_WRITE",
                     address,
                     byteCount);

    if (byteCount == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_NULL_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         byteCount,
                         "byteCount is zero");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* If Address is not on word boundary, Or ByteCount is not on Word boundary, write byte by byte */
    if (((address & 0x00000003) > 0) || ((byteCount & 0x00000003) > 0))
    {
        isByteWrite = ADI_TRUE;
    }

    /* setup DMA control register for write operation */
    adrv9025_CpuDmaSetup(device,
                         address,
                         ADI_TRUE,
                         isByteWrite,
                         autoIncrement);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* If Address is not on word boundary, Or ByteCount is not on Word boundary */
    if (isByteWrite != ADI_FALSE)
    {
        adrv9025_CpuDmaMemWriteByte(device,
                                    address,
                                    data,
                                    byteCount,
                                    autoIncrement);
    }
    else
    {
#if ADI_ADRV9025_SPI_WRITE_STREAMING_ENABLE == 1
        /* Enable Spi streaming */
        ADRV9025_SPI_STREAM_ENABLE(device);
#endif

        if ((device->spiSettings.enSpiStreaming == 1) && (autoIncrement == ADI_TRUE))
        {
            /* 1) STREAMING WITH AUTO INCREMENT */
            /* Send sequential words without address */

#ifdef ADI_ADRV9025_USER_DEFINED_SPI_WRITE_STREAM
            /* Streaming with custom hal layer function(adi_hal_CustomSpiStreamWrite) */
            ADRV9025_SPIWRITEWORDDMASTREAMWITHCUSTOMHALFUNCTION("DMA_MEM_WRITE_STREAM", ADRV9025_CPU_ADDR_DMA_DATA3, data, byteCount);
#else
            /* Streaming with standard hal layer function(adrv9025_hal_SpiWrite) */
            ADRV9025_SPIWRITEWORDDMASTREAM("DMA_MEM_WRITE_STREAM",
                                           ADRV9025_CPU_ADDR_DMA_DATA3,
                                           data,
                                           byteCount);
#endif
        }
        else if ((device->spiSettings.enSpiStreaming == 1) && (autoIncrement == ADI_FALSE))
        {
            /* 2) STREAMING WITHOUT AUTO INCREMENT */
            /* Send each word with its address */
            for (i = 0; i < byteCount; i += WORD_LENGTH)
            {
                addrArrayStream[0] = (uint8_t)((address & 0x000000FF) >> 0);
                addrArrayStream[1] = (uint8_t)((address & 0x0000FF00) >> 8);
                addrArrayStream[2] = (uint8_t)((address & 0x00FF0000) >> 16);
                addrArrayStream[3] = (uint8_t)((address & 0xFF000000) >> 24);

                ADRV9025_SPIWRITEWORDDMASTREAM("DMA_MEM_WRITE_STREAM",
                                               ADRV9025_CPU_ADDR_DMA_ADDR3,
                                               addrArrayStream,
                                               WORD_LENGTH);

                address += WORD_LENGTH;

                ADRV9025_SPIWRITEWORDDMASTREAM("DMA_MEM_WRITE_STREAM",
                                               ADRV9025_CPU_ADDR_DMA_DATA3,
                                               &data[i],
                                               WORD_LENGTH);
            }
        }
        else if ((cacheEnable == ADI_TRUE) && (autoIncrement == ADI_TRUE))
        {
            /* 3) NON-STREAMING WITH CACHE AND AUTO INCREMENT */
            /* Cache Enable and Auto Inc */
            for (i = 0; i < byteCount; i++)
            {
                /* Write mem_write_data bitfield (mem_write_data[7:0] has to be the last register written) */
                /* core_bf.mem_write_data.write(bf_status, 32'hxxxxxxxx); */
                index = address % 4;

                /* Writing byte order: 3,2,1,0 */
                if (index == 0)
                {
                    dataIndex = i + 3;
                }
                else
                {
                    dataIndex--;
                }

                dataArray[addrIndex] = data[dataIndex];
                addrArray[addrIndex] = ADDR_CPU_DMA_DATA[index];

                addrIndex++;
                address++;

                if (addrIndex >= spiBufferSize)
                {
                    ADRV9025_SPIWRITEBYTESDMA("DMA_MEM_WRITE_CACHE",
                                              (address - addrIndex),
                                              &addrArray[0],
                                              &dataArray[0],
                                              addrIndex);
                    addrIndex = 0;
                }
            }

            if (addrIndex > 0)
            {
                ADRV9025_SPIWRITEBYTESDMA("DMA_MEM_WRITE_CACHE_LAST",
                                          (address - addrIndex),
                                          &addrArray[0],
                                          &dataArray[0],
                                          addrIndex);
            }
        }
        else
        {
            /* 4) NON-STREAMING WITHOUT CACHE */
            /* starting write at zero address offset */
            for (i = 0; i < byteCount; i += 4)
            {
                /* Write mem_write_data bitfield (mem_write_data[7:0] has to be the last register written) */
                /* core_bf.bus_addr.write(bf_status, 32'hxxxxxxxx); */
                ADRV9025_SPIWRITEWORDDMA("CPU_DMA_ADDR",
                                         ADRV9025_CPU_ADDR_DMA_DATA3,
                                         (uint32_t)(data[i + 3] << 24) | (uint32_t)(data[i + 2] << 16) |
                                         (uint32_t)(data[i + 1] << 8) | (uint32_t)(data[i + 0] << 0));
                if (autoIncrement == ADI_FALSE)
                {
                    /* AHB address for write */
                    /* core_bf.bus_addr.write(bf_status, 32'hxxxxxxxx); */
                    address += 4;
                    ADRV9025_SPIWRITEWORDDMA("CPU_DMA_ADDR",
                                             ADRV9025_CPU_ADDR_DMA_ADDR3,
                                             address);
                }
            }
        }
#if ADI_ADRV9025_SPI_WRITE_STREAMING_ENABLE == 1
        /* Disable Spi streaming */
        ADRV9025_SPI_STREAM_DISABLE(device);
#endif
    }

    /* setting up the DMA control register for a read to avoid accidental writing */
    adrv9025_CpuDmaSetup(device,
                         address,
                         ADI_FALSE,
                         isByteWrite,
                         autoIncrement);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return recoveryAction;
}

int32_t adrv9025_CpuDmaMemRead(
    adi_adrv9025_Device_t* device,
    uint32_t               address,
    uint8_t                returnData[],
    uint32_t               byteCount,
    uint8_t                autoIncrement)
{
    static const uint32_t ADDR_ARM_DMA_DATA[4] = {
        ADRV9025_CPU_ADDR_DMA_DATA3,
        ADRV9025_CPU_ADDR_DMA_DATA2,
        ADRV9025_CPU_ADDR_DMA_DATA1,
        ADRV9025_CPU_ADDR_DMA_DATA0
    };
    static const uint8_t  cacheEnable            = ADI_ADRV9025_CPU_MEMREAD_CACHE_ENABLE;
    static const uint32_t SHIFT_FOR_NUM_OF_WORDS = 2;
    static const uint32_t WORD_LENGTH            = 4;

    int32_t  recoveryAction                               = 0;
    uint32_t dataRead                                     = 0;
    uint8_t  isByteRead                                   = ADI_FALSE;
    uint32_t dataCounter                                  = 0;
    uint32_t wordCount                                    = 0;
    uint8_t  tempByte                                     = 0;
    uint32_t i                                            = 0;
    uint32_t index                                        = 0;
    uint32_t addrIndex                                    = 0;
    uint16_t addrArray[(ADI_COMMON_HAL_SPIARRAYSIZE / 3)] = {0};
    uint32_t spiBufferSize                                = (uint32_t)(sizeof(addrArray) / sizeof(addrArray[0]));
    uint8_t  addrArrayStream[4]                           = {0};

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        returnData);

    ADRV9025_BUGINFO(__FUNCTION__);

    ADRV9025_DMAINFO("CPU_MEM_READ",
                     address,
                     byteCount);

    if (byteCount == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_NULL_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         byteCount,
                         "byteCount is zero");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* If Address is not on word boundary, Or ByteCount is not on Word boundary, read byte by byte */
    if (((address & 0x00000003) > 0) || ((byteCount & 0x00000003) > 0))
    {
        isByteRead = ADI_TRUE;
    }

    /* setup DMA control register for write operation */
    adrv9025_CpuDmaSetup(device,
                         address,
                         ADI_FALSE,
                         isByteRead,
                         autoIncrement);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* If Address is not on word boundary, Or ByteCount is not on Word boundary */
    if (isByteRead != ADI_FALSE)
    {
        return adrv9025_CpuDmaMemReadByte(device,
                                          address,
                                          returnData,
                                          byteCount,
                                          autoIncrement);
    }

#if ADI_ADRV9025_SPI_READ_STREAMING_ENABLE == 1
    /* Enable Spi streaming */
    ADRV9025_SPI_STREAM_ENABLE(device);
#endif

    wordCount = byteCount >> SHIFT_FOR_NUM_OF_WORDS;

    if ((device->spiSettings.enSpiStreaming == 1) && (autoIncrement == ADI_TRUE))
    {
        /* 1) STREAMING WITH AUTO INCREMENT */
        /* Send sequential words without address */

#ifdef ADI_ADRV9025_USER_DEFINED_SPI_READ_STREAM
        /* Streaming with custom hal layer function(adi_hal_CustomSpiStreamRead) */
        ADRV9025_SPIREADWORDDMASTREAMWITHCUSTOMHALFUNCTION("DMA_MEM_READ_STREAM", ADRV9025_CPU_ADDR_DMA_DATA3, returnData, byteCount);
#else
        /* Streaming with standard hal layer function(adrv9025_hal_SpiRead) */
        ADRV9025_SPIREADWORDDMASTREAM("DMA_MEM_READ_STREAM",
                                      ADRV9025_CPU_ADDR_DMA_DATA3,
                                      returnData,
                                      byteCount);
#endif
    }
    else if ((device->spiSettings.enSpiStreaming == 1) && (autoIncrement == ADI_FALSE))
    {
        /* 2) STREAMING WITHOUT AUTO INCREMENT */
        /* Send each word with its address */
        for (i = 0; i < byteCount; i += WORD_LENGTH)
        {
            addrArrayStream[0] = (uint8_t)((address & 0x000000FF) >> 0);
            addrArrayStream[1] = (uint8_t)((address & 0x0000FF00) >> 8);
            addrArrayStream[2] = (uint8_t)((address & 0x00FF0000) >> 16);
            addrArrayStream[3] = (uint8_t)((address & 0xFF000000) >> 24);

            ADRV9025_SPIWRITEWORDDMASTREAM("DMA_MEM_WRITE_STREAM",
                                           ADRV9025_CPU_ADDR_DMA_ADDR3,
                                           addrArrayStream,
                                           WORD_LENGTH);

            address += WORD_LENGTH;

            ADRV9025_SPIREADWORDDMASTREAM("DMA_MEM_READ_STREAM",
                                          ADRV9025_CPU_ADDR_DMA_DATA3,
                                          &returnData[i],
                                          WORD_LENGTH);
        }
    }
    else if ((cacheEnable == ADI_TRUE) && (autoIncrement == ADI_TRUE))
    {
        /* 3) NON-STREAMING WITH CACHE AND AUTO INCREMENT */
        /* Cache Enable and Auto Inc */
        for (i = 0; i < byteCount; i++)
        {
            /* Read mem_write_data bitfield (mem_write_data[7:0] has to be the last register read) */
            /* core_bf.mem_write_data.write(bf_status, 32'hxxxxxxxx); */
            index = address % WORD_LENGTH;

            addrArray[addrIndex] = ADDR_ARM_DMA_DATA[index];

            addrIndex++;
            address++;

            if (addrIndex >= spiBufferSize)
            {
                ADRV9025_SPIREADBYTESDMA("DMA_MEM_READ_CACHE",
                                         (address - addrIndex),
                                         &addrArray[0],
                                         &returnData[dataCounter],
                                         addrIndex);
                dataCounter += addrIndex;
                addrIndex = 0;
            }
        }

        if (addrIndex > 0)
        {
            ADRV9025_SPIREADBYTESDMA("DMA_MEM_READ_CACHE_LAST",
                                     (address - addrIndex),
                                     &addrArray[0],
                                     &returnData[dataCounter],
                                     addrIndex);
        }

        for (i = 0; i < wordCount; i++)
        {
            /* Reorder bytes from Data3-2-1-0 to Data0-1-2-3. This is required because API reads data starting from Data3(lower address) to hit Data0(higher address) last */
            tempByte                          = returnData[i * WORD_LENGTH];
            returnData[i * WORD_LENGTH]       = returnData[(i * WORD_LENGTH) + 3];
            returnData[(i * WORD_LENGTH) + 3] = tempByte;

            tempByte                          = returnData[(i * WORD_LENGTH) + 1];
            returnData[(i * WORD_LENGTH) + 1] = returnData[(i * WORD_LENGTH) + 2];
            returnData[(i * WORD_LENGTH) + 2] = tempByte;
        }
    }
    else
    {
        /* 4) NON-STREAMING WITHOUT CACHE */
        /* starting write at zero address offset */
        for (i = 0; i < byteCount; i += 4)
        {
            /* Write mem_write_data bitfield (mem_write_data[7:0] has to be the last register written) */
            /* core_bf.mem_write_data.write(bf_status, 32'hxxxxxxxx); */
            ADRV9025_SPIREADWORDDMA("ARM_DMA_ADDR",
                                    ADRV9025_CPU_ADDR_DMA_DATA3,
                                    &dataRead)

            returnData[i + 3] = (uint8_t)(dataRead >> 24);
            returnData[i + 2] = (uint8_t)(dataRead >> 16);
            returnData[i + 1] = (uint8_t)(dataRead >> 8);
            returnData[i + 0] = (uint8_t)(dataRead >> 0);

            if (autoIncrement == ADI_FALSE)
            {
                /* AHB address for read */
                /* core_bf.bus_addr.write(bf_status, 32'hxxxxxxxx); */
                address += 4;
                ADRV9025_SPIWRITEWORDDMA("ARM_DMA_ADDR",
                                         ADRV9025_CPU_ADDR_DMA_ADDR3,
                                         address);
            }
        }
    }

#if ADI_ADRV9025_SPI_READ_STREAMING_ENABLE == 1
    /* Disable Spi streaming */
    ADRV9025_SPI_STREAM_DISABLE(device);
#endif

    return (device->common.error.newAction);
}

const char* adrv9025_CpuErrCodeToString(
    adi_adrv9025_Device_t* device,
    uint32_t               errCode)
{
    UNUSED_PARA(device);
#if ADI_ADRV9025_CPU_VERBOSE == 0
    return "";
#else
    const char* result = NULL;
    uint32_t    min    = 0u;
    uint32_t    max    = (sizeof(adrv9025_error_table_ErrorCode) / sizeof(adrv9025_arm_api_error_t)) - 1u;
    uint32_t    curr   = (min + max) / 2;

    /* do a binary search */
    while (min <= max && result == NULL)
    {
        if (errCode == adrv9025_error_table_ErrorCode[curr].errcode)
        {
            result = adrv9025_error_table_ErrorCode[curr].errstr;
        }
        else if (errCode < adrv9025_error_table_ErrorCode[curr].errcode)
        {
            max = curr - 1u;
        }
        else
        {
            min = curr + 1u;
        }

        curr = (min + max) / 2;
    }

    return result;
#endif
}

int32_t adrv9025_CpuCmdErrorHandler(
    adi_adrv9025_Device_t* device,
    adi_adrv9025_CpuType_e cpuType,
    int32_t                errHdl,
    uint32_t               detErr,
    int32_t                recoveryAction)
{
    UNUSED_PARA(errHdl);
    int32_t              recAction      = recoveryAction;
    uint8_t              cpuOpCode      = 0;
    uint8_t              cpuObjId       = 0;
    uint8_t              cmdStatusByte  = 0;
    uint32_t             cpuErrorFlag   = 0;
    uint32_t             cpuErrorCode   = 0;
    uint16_t             mailboxErrCode = 0;
    const char*          errorString    = NULL;
    static const uint8_t ERR_FLAG_MASK  = 0x0E;
    static const uint8_t ERR_FLAG_SHIFT = 1;

    cpuOpCode     = (uint8_t)(detErr >> ADRV9025_CPU_OPCODE_SHIFT);
    cpuObjId      = (uint8_t)(detErr >> ADRV9025_CPU_OBJ_ID_SHIFT);
    cmdStatusByte = (uint8_t)(detErr);

    /* perform object id overrides for opcodes Abort, RunInit, RadioOn */
    if ((cpuOpCode == ADRV9025_CPU_ABORT_OPCODE)
        || (cpuOpCode == ADRV9025_CPU_RUNINIT_OPCODE)
        || (cpuOpCode == ADRV9025_CPU_RADIOON_OPCODE))
    {
        cpuObjId = 0x00;
    }

    cpuErrorFlag = ADRV9025_BF_DECODE(cmdStatusByte,
                                      ERR_FLAG_MASK,
                                      ERR_FLAG_SHIFT);

    if (cpuErrorFlag > 0)
    {
        cpuErrorCode = ADRV9025_CPU_CMD_ERRCODE(cpuOpCode,
                                                cpuObjId,
                                                cpuErrorFlag);
        if (cpuErrorCode == ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_RUNINIT_OPCODE,
                                                     0,
                                                     7))
        {
            recAction = ADI_ADRV9025_ACT_ERR_RERUN_INIT_CALS;
        }
        else
        {
            switch (adrv9025_CpuErrorFlagGet(cpuErrorCode))
            {
            case ADRV9025_CPU_ERROR_FLAG_1:
            case ADRV9025_CPU_ERROR_FLAG_2:
            case ADRV9025_CPU_ERROR_FLAG_3:
            case ADRV9025_CPU_ERROR_FLAG_4:
            case ADRV9025_CPU_ERROR_FLAG_5:
            case ADRV9025_CPU_ERROR_FLAG_6:
            case ADRV9025_CPU_ERROR_FLAG_7:
                recAction = ADI_COMMON_ACT_ERR_CHECK_PARAM;
                break;
            default:
                recAction = ADI_COMMON_ACT_NO_ACTION;
            }
        }

        if (recAction != ADI_COMMON_ACT_NO_ACTION)
        {
            errorString = adrv9025_CpuErrMsgGet(device,
                                                cpuType,
                                                detErr,
                                                &mailboxErrCode);
            ADI_ERROR_REPORT(&device->common,
                             ADI_ADRV9025_SRC_CPUCMD,
                             detErr,
                             recAction,
                             detErr,
                             errorString);

            /* Report Cpu mailboxErrCode*/
            if ((cpuErrorFlag == 7) && (mailboxErrCode > 0))
            {
                errorString = adrv9025_CpuErrCodeToString(device,
                                                          mailboxErrCode);
                ADI_ERROR_REPORT(&device->common,
                                 ADI_ADRV9025_SRC_CPUCMD,
                                 mailboxErrCode,
                                 recAction,
                                 mailboxErrCode,
                                 errorString);
            }

            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return recAction;
}

int32_t adrv9025_CpuAdcProfilesWrite(adi_adrv9025_Device_t*            device,
                                     const adi_adrv9025_AdcProfiles_t* adcProfiles)
{
    int32_t  recoveryAction                           = ADI_COMMON_ACT_NO_ACTION;
    uint8_t  cfgData[ADRV9025_PROFILE_CHUNK_MAX + 10] = {0};
    uint32_t profileAddr                              = 0;
    uint32_t checksum                                 = 0;
    uint32_t start                                    = 0;

    static const uint32_t ADRV9025_ADDR_DEVICE_PROFILE = 0x20000000;
    static const uint32_t ADRV9025_ADC_PROFILE_MAX     = 684;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        adcProfiles);

    ADRV9025_BUGINFO(__FUNCTION__);

    if (device->devStateInfo.cpu.adcProfileAddr < ADRV9025_ADDR_DEVICE_PROFILE)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "Invalid ADC profile Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    adrv9025_CpuDataSet(cfgData,
                        0,
                        ADRV9025_PROFILE_CHUNK_MAX);
    start = 0;
    /* RX CONFIG 96 bytes: 20 - 115 */
    adrv9025_CpuLoadAdcBytes(start,
                             &cfgData[0],
                             &adcProfiles->RxChannel1[0]);
    start += ADRV9025_CPU_ADC_PROFILE_SIZE;
    adrv9025_CpuLoadAdcBytes(start,
                             &cfgData[0],
                             &adcProfiles->RxChannel2[0]);
    start += ADRV9025_CPU_ADC_PROFILE_SIZE;
    adrv9025_CpuLoadAdcBytes(start,
                             &cfgData[0],
                             &adcProfiles->RxChannel3[0]);
    start += ADRV9025_CPU_ADC_PROFILE_SIZE;
    adrv9025_CpuLoadAdcBytes(start,
                             &cfgData[0],
                             &adcProfiles->RxChannel4[0]);
    start += ADRV9025_CPU_ADC_PROFILE_SIZE;

    profileAddr = device->devStateInfo.cpu.adcProfileAddr;
    checksum    = adrv9025_Crc32ForChunk(&cfgData[0],
                                         start,
                                         checksum,
                                         0);
    recoveryAction = adi_adrv9025_CpuMemWrite(device,
                                              profileAddr,
                                              &cfgData[0],
                                              start,
                                              ADI_ADRV9025_CPU_MEM_AUTO_INCR);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Error from adi_adrv9025_CpuMemWrite()");
    ADI_ERROR_RETURN(device->common.error.newAction);

    profileAddr += start;
    start = 0;
    adrv9025_CpuDataSet(&cfgData[0],
                        0,
                        ADRV9025_PROFILE_CHUNK_MAX);
    adrv9025_CpuLoadAdcBytes(start,
                             &cfgData[0],
                             &adcProfiles->OrxChannel1Profile0[0]);
    start += ADRV9025_CPU_ADC_PROFILE_SIZE;
    adrv9025_CpuLoadAdcBytes(start,
                             &cfgData[0],
                             &adcProfiles->OrxChannel1Profile1[0]);
    start += ADRV9025_CPU_ADC_PROFILE_SIZE;
    adrv9025_CpuLoadAdcBytes(start,
                             &cfgData[0],
                             &adcProfiles->OrxChannel1Profile2[0]);
    start += ADRV9025_CPU_ADC_PROFILE_SIZE;
    adrv9025_CpuLoadAdcBytes(start,
                             &cfgData[0],
                             &adcProfiles->OrxChannel1Profile3[0]);
    start += ADRV9025_CPU_ADC_PROFILE_SIZE;

    checksum = adrv9025_Crc32ForChunk(&cfgData[0],
                                      start,
                                      checksum,
                                      0);
    recoveryAction = adi_adrv9025_CpuMemWrite(device,
                                              profileAddr,
                                              &cfgData[0],
                                              start,
                                              ADI_ADRV9025_CPU_MEM_AUTO_INCR);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Error from adi_adrv9025_CpuMemWrite()");
    ADI_ERROR_RETURN(device->common.error.newAction);

    profileAddr += start;
    start = 0;
    adrv9025_CpuDataSet(&cfgData[0],
                        0,
                        ADRV9025_PROFILE_CHUNK_MAX);

    adrv9025_CpuLoadAdcBytes(start,
                             &cfgData[0],
                             &adcProfiles->OrxChannel2Profile0[0]);
    start += ADRV9025_CPU_ADC_PROFILE_SIZE;
    adrv9025_CpuLoadAdcBytes(start,
                             &cfgData[0],
                             &adcProfiles->OrxChannel2Profile1[0]);
    start += ADRV9025_CPU_ADC_PROFILE_SIZE;
    adrv9025_CpuLoadAdcBytes(start,
                             &cfgData[0],
                             &adcProfiles->OrxChannel2Profile2[0]);
    start += ADRV9025_CPU_ADC_PROFILE_SIZE;
    adrv9025_CpuLoadAdcBytes(start,
                             &cfgData[0],
                             &adcProfiles->OrxChannel2Profile3[0]);
    start += ADRV9025_CPU_ADC_PROFILE_SIZE;

    cfgData[start++] = adcProfiles->OrxChannel1Index;

    cfgData[start++] = adcProfiles->OrxChannel2Index;

    /* Reserved 6 bytes */
    start += 6;

    /* Profile Checksum 4 bytes: 608 - 611 */
    checksum = adrv9025_Crc32ForChunk(&cfgData[0],
                                      start,
                                      checksum,
                                      1); /* last checksum */
    adrv9025_CpuLoadFourBytes(start,
                              &cfgData[0],
                              checksum);
    start += 4;

    recoveryAction = adi_adrv9025_CpuMemWrite(device,
                                              profileAddr,
                                              &cfgData[0],
                                              start,
                                              ADI_ADRV9025_CPU_MEM_AUTO_INCR);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Error from adi_adrv9025_CpuMemWrite()");
    ADI_ERROR_RETURN(device->common.error.newAction);

    profileAddr += start;
    start = 0;

    if (profileAddr != (device->devStateInfo.cpu.adcProfileAddr + ADRV9025_ADC_PROFILE_MAX))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         profileAddr,
                         "Exceed Max ADC Profile bytes");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return (device->common.error.newAction);
}

/*
API to CPU Clock Profile: Structure DeviceProfile_t
Size: 1256 bytes
Element	Size	Type	Range	Description
VcoFreqKhz	4	uint32	4915.2- 12288 Mhz	VCO frequency of the system as set by API, in Khz.
hsDigHz_api	4	uint32	490-1250 Mhz	System HS Dig clock setting calculated by the API in Hz.
deviceClockHz	4	uint32	61.44-983.04 Mhz
    System device clock setting, in multiples of 61.44 in Hz.
clkGenSel	1	uint8	0 or 1	PLL select (Serdes or ClkPLL). Typically ClkPll will be selected but for debug it will be possible to use Serdes.
LoPhaseSync	1	uint8	0-2	0 = Don't Sync
1 = Run Init and sync
2 = Run init and Continuous tracking
LoSelect	1	uint8	0-0x3F	D0 -  Rx12 Sel      0 = LO1 to Rx12, 1 = LO2 to Rx12
D1 -  Rx34 Sel      0 = LO1 to Rx34, 1 = LO2 to Rx34
D2 -  Tx12 Sel      0 = LO1 to Tx12, 1 = LO2 to Tx12
D3 -  Tx34 Sel      0 = LO1 to Tx34, 1 = LO2 to Tx34
D4 -  Orx12 Sel    0 = TxLO to Orx12, 1 = AuxLO to Orx12
D5 -  Orx34 Sel    0 = TxLO to Orx34, 1 = AuxLO to Orx34
NOTE: The TxLO is the LO that is routed to the Tx.
NOTE: LO1 = RxLO, LO2 = TxLO
extLoSel	1	uint8	0-0xF	D0-2 -  LO1  Sel:
0 = Inter LO1,  0x1 = Ext out,  0x2 = Ext in x2, 0x3 = Extin x4, 0x4 = Ext in x8

D3-5 -  LO2  Sel:
0 = Inter LO2,  0x1 = Ext out,  0x2 = Ext in x2, 0x3 = Extin x4, 0x4 = Ext in x8

D6 - Swap cntrl for Ext LO1 : Def = 0 no-swap,1=swap
D7 - Swap cntrl for Ext LO2: Def = 0 no-swap,1=swap
Ext LO Freq 1	4	uint32	25Mhz to 12 Ghz	External LO 1 freq in Khz
Ext LO Freq 2	4	uint32	25Mhz to 12 Ghz	External LO 1 freq in Khz

chanConfig	2	uint16	0-0xFFF	D0 -  Tx1           0 = Disabled, 1 = Enabled
D1 -  Tx2           0 = Disabled, 1 = Enabled
D2 -  Tx3           0 = Disabled, 1 = Enabled
D3 -  Tx4           0 = Disabled, 1 = Enabled
D4 -  Rx1           0 = Disabled, 1 = Enabled
D5 -  Rx2           0 = Disabled, 1 = Enabled
D6 -  Rx3           0 = Disabled, 1 = Enabled
D7 -  Rx4           0 = Disabled, 1 = Enabled
D8 -  Orx1         0 = Disabled, 1 = Enabled
D9 -  Orx2         0 = Disabled, 1 = Enabled
D10 -  Orx3       0 = Disabled, 1 = Enabled
D11 -  Orx4       0 = Disabled, 1 = Enabled
D12 -  Lbrx1/2  0 = Disabled, 1 = Enabled
D13 -  Lbrx3/4  0 = Disabled, 1 = Enabled
Ext LO1 Divider	1	ExtLo1Div		This divider is independent from internal LO1 gen divider
(valid 1, 2, 4, 8, 16, 32, 64, 128)
Ext LO2 Divider	1	ExtLo2Div		This divider is independent from internal LO1 gen divider
(valid 1, 2, 4, 8, 16, 32, 64, 128)
Padding[8]	8	uint8
RxConfig[4]	288	RxConfig_t		Receiver profile for channels 1 to 4, refer to Table 3.

TxConfig[4]	160	TxConfig_t		Transmit profile for channels 1 to 4, refer to Table 4.

OrxConfig[4]	224	OrxConfig_t		Orx profile for channels 1 to 4, refer to Table 5.

LbConfig[2]	112	OrxConfig_t		Loopback profile for channels 1 to 2, refer to Table 5.

JESD Profile	380	JesdSettings_t		JSED profile, refer to Table 7.

Noise Cancellation	36	noiseCancel_t		Noise cancellation configuration, refer to
Reserved[16]	16	uint8		16 bytes reserved memory.
Profile checksum	4	uint32		Checksum of the entire profile excluding ADC profiles, Using the CRC32 algorithm.


 **/

int32_t adrv9025_CpuProfileWrite(adi_adrv9025_Device_t*     device,
                                 const adi_adrv9025_Init_t* init)
{
    int32_t  recoveryAction                           = ADI_COMMON_ACT_NO_ACTION;
    uint32_t profileAddr                              = 0;
    uint32_t cpuChannels                              = 0;
    uint32_t checksum                                 = 0;
    uint32_t start                                    = 0;
    uint32_t i                                        = 0;
    uint8_t  cfgData[ADRV9025_PROFILE_CHUNK_MAX + 10] = {0};

    static const uint32_t ADRV9025_ADDR_DEVICE_PROFILE = 0x20000000;
    static const uint32_t ADRV9025_PROFILE_MAX         = 1352;

    static const uint32_t ADRV9025_PROFILE_TX  = 40;
    static const uint32_t ADRV9025_PROFILE_RX  = 72;
    static const uint32_t ADRV9025_PROFILE_ORX = 56;

    static const uint32_t ADRV9025_PROFILE_JESD_FRAMER      = 53;
    static const uint32_t ADRV9025_PROFILE_JESD_DEFRAMER    = 38;
    static const uint32_t ADRV9025_PROFILE_JESD_SERCFG      = 8;
    static const uint32_t ADRV9025_PROFILE_JESD_DESCFG      = 44;
    static const uint32_t ADRV9025_PROFILE_JESD_LINKSHARING = 20;
    static const uint32_t ADRV9025_PROFILE_JESD_DATACONFIG  = 3;
    static const uint32_t ADRV9025_PROFILE_JESD_END         = 8;
    static const uint32_t ADRV9025_PROFILE_NOISE_CANCEL     = 36;

    static const uint32_t ADRV9025_TX1  = 0x001;  /*!< Rx1 channel enabled */
    static const uint32_t ADRV9025_TX2  = 0x002;  /*!< Rx2 channel enabled */
    static const uint32_t ADRV9025_TX3  = 0x004;  /*!< Rx3 channel enabled */
    static const uint32_t ADRV9025_TX4  = 0x008;  /*!< Rx4 channel enabled */
    static const uint32_t ADRV9025_RX1  = 0x010;  /*!< Rx1 channel enabled */
    static const uint32_t ADRV9025_RX2  = 0x020;  /*!< Rx2 channel enabled */
    static const uint32_t ADRV9025_RX3  = 0x040;  /*!< Rx3 channel enabled */
    static const uint32_t ADRV9025_RX4  = 0x080;  /*!< Rx4 channel enabled */
    static const uint32_t ADRV9025_ORX1 = 0x100;  /*!< ORx1 channel enabled */
    static const uint32_t ADRV9025_ORX2 = 0x200;  /*!< ORx2 channel enabled */
    static const uint32_t ADRV9025_ORX3 = 0x400;  /*!< ORx3 channel enabled */
    static const uint32_t ADRV9025_ORX4 = 0x800;  /*!< ORx4 channel enabled */
    static const uint32_t ADRV9025_LB12 = 0x1000; /*!< LBx12 channel enabled */
    static const uint32_t ADRV9025_LB34 = 0x2000; /*!< LBx34 channel enabled */

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        init);

    ADRV9025_BUGINFO(__FUNCTION__);

    if (device->devStateInfo.cpu.devProfileAddr < ADRV9025_ADDR_DEVICE_PROFILE)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "Invalid profile Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    adrv9025_CpuDataSet(&cfgData[0],
                        0,
                        ADRV9025_PROFILE_CHUNK_MAX);

    adrv9025_CpuLoadFourBytes(0,
                              &cfgData[0],
                              (init->clocks.clkPllVcoFreq_kHz));
    adrv9025_CpuLoadFourBytes(4,
                              &cfgData[0],
                              (device->devStateInfo.hsDigClk_kHz * 1000));
    adrv9025_CpuLoadFourBytes(8,
                              &cfgData[0],
                              (init->clocks.deviceClock_kHz * 1000));

    cfgData[12] = 1; /* clkGenSel User PLL */
#ifdef API_ADRV9025_USE_SERDES
    cfgData[12] = 0; /* clkGenSel User SERDES */
#endif

    if ((init->clocks.rx12LoSelect != ADI_ADRV9025_LOSEL_LO1) &&
        (init->clocks.rx12LoSelect != ADI_ADRV9025_LOSEL_LO2))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         init->clocks.rx12LoSelect,
                         "Invalid LO Select for Rx1/Rx2. Valid values are LO1(1) and LO2(2)");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    if ((init->clocks.rx34LoSelect != ADI_ADRV9025_LOSEL_LO1) &&
        (init->clocks.rx34LoSelect != ADI_ADRV9025_LOSEL_LO2))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         init->clocks.rx34LoSelect,
                         "Invalid LO Select for Rx3/Rx4. Valid values are LO1(1) and LO2(2)");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    if ((init->clocks.tx12LoSelect != ADI_ADRV9025_LOSEL_LO1) &&
        (init->clocks.tx12LoSelect != ADI_ADRV9025_LOSEL_LO2))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         init->clocks.tx12LoSelect,
                         "Invalid LO Select for Tx1/Tx2. Valid values are LO1(1) and LO2(2)");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    if ((init->clocks.tx34LoSelect != ADI_ADRV9025_LOSEL_LO1) &&
        (init->clocks.tx34LoSelect != ADI_ADRV9025_LOSEL_LO2))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         init->clocks.tx34LoSelect,
                         "Invalid LO Select for Tx3/Tx4. Valid values are LO1(1) and LO2(2)");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    if ((init->clocks.orx12LoSelect != ADI_ADRV9025_ORXLOSEL_TXLO) &&
        (init->clocks.orx12LoSelect != ADI_ADRV9025_ORXLOSEL_AUXLO))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         init->clocks.orx12LoSelect,
                         "Invalid LO Select for Orx1/Orx2. Valid values are TXLO(1) and AuxLO(2)");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    if ((init->clocks.orx34LoSelect != ADI_ADRV9025_ORXLOSEL_TXLO) &&
        (init->clocks.orx34LoSelect != ADI_ADRV9025_ORXLOSEL_AUXLO))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         init->clocks.orx34LoSelect,
                         "Invalid LO Select for Orx3/Orx4. Valid values are TXLO(1) and AuxLO(2)");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    cfgData[13] = init->clocks.rfPllPhaseSyncMode; /* lophaseSync ?*/
    cfgData[14] = ((uint8_t)init->clocks.rx12LoSelect - 1) & 0x01;
    cfgData[14] |= ((((uint8_t)init->clocks.rx34LoSelect - 1) & 0x01) << 1);
    cfgData[14] |= ((((uint8_t)init->clocks.tx12LoSelect - 1) & 0x01) << 2);
    cfgData[14] |= ((((uint8_t)init->clocks.tx34LoSelect - 1) & 0x01) << 3);
    cfgData[14] |= ((((uint8_t)init->clocks.orx12LoSelect - 1) & 0x01) << 4);
    cfgData[14] |= ((((uint8_t)init->clocks.orx34LoSelect - 1) & 0x01) << 5);

    if ((init->clocks.rfPll1LoMode != ADI_ADRV9025_INTLO_NOOUTPUT) &&
        (init->clocks.rfPll1LoMode != ADI_ADRV9025_INTLO_OUTPUT) &&
        (init->clocks.rfPll1LoMode != ADI_ADRV9025_EXTLO_2X_INPUT) &&
        (init->clocks.rfPll1LoMode != ADI_ADRV9025_EXTLO_4X_INPUT) &&
        (init->clocks.rfPll1LoMode != ADI_ADRV9025_EXTLO_8X_INPUT))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         init->clocks.rfPll1LoMode,
                         "Invalid Rf Pll1 LO Mode");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((init->clocks.rfPll2LoMode != ADI_ADRV9025_INTLO_NOOUTPUT) &&
        (init->clocks.rfPll2LoMode != ADI_ADRV9025_INTLO_OUTPUT) &&
        (init->clocks.rfPll2LoMode != ADI_ADRV9025_EXTLO_2X_INPUT) &&
        (init->clocks.rfPll2LoMode != ADI_ADRV9025_EXTLO_4X_INPUT) &&
        (init->clocks.rfPll2LoMode != ADI_ADRV9025_EXTLO_8X_INPUT))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         init->clocks.rfPll2LoMode,
                         "Invalid Rf Pll2 LO Mode");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    cfgData[15] = (uint8_t)(init->clocks.rfPll1LoMode & 0x07);
    cfgData[15] |= (uint8_t)((init->clocks.rfPll2LoMode & 0x07) << 3);

    adrv9025_CpuLoadFourBytes(16,
                              &cfgData[0],
                              (init->clocks.extLoFreq1_kHz)); /* External LO freq 1 in Khz */
    adrv9025_CpuLoadFourBytes(20,
                              &cfgData[0],
                              (init->clocks.extLoFreq2_kHz)); /* External LO freq 2 in Khz */

    if (ADRV9025_BF_EQUAL(device->devStateInfo.profilesValid,
                          ADI_ADRV9025_RX_PROFILE_VALID))
    {
        if (ADRV9025_BF_EQUAL(init->rx.rxInitChannelMask,
                              ADI_ADRV9025_RX1))
        {
            cpuChannels |= ADRV9025_RX1;
        }
        if (ADRV9025_BF_EQUAL(init->rx.rxInitChannelMask,
                              ADI_ADRV9025_RX2))
        {
            cpuChannels |= ADRV9025_RX2;
        }
        if (ADRV9025_BF_EQUAL(init->rx.rxInitChannelMask,
                              ADI_ADRV9025_RX3))
        {
            cpuChannels |= ADRV9025_RX3;
        }
        if (ADRV9025_BF_EQUAL(init->rx.rxInitChannelMask,
                              ADI_ADRV9025_RX4))
        {
            cpuChannels |= ADRV9025_RX4;
        }
    }

    if (ADRV9025_BF_EQUAL(device->devStateInfo.profilesValid,
                          ADI_ADRV9025_TX_PROFILE_VALID))
    {
        if (ADRV9025_BF_EQUAL(init->tx.txInitChannelMask,
                              ADI_ADRV9025_TX1))
        {
            cpuChannels |= ADRV9025_TX1;
        }
        if (ADRV9025_BF_EQUAL(init->tx.txInitChannelMask,
                              ADI_ADRV9025_TX2))
        {
            cpuChannels |= ADRV9025_TX2;
        }
        if (ADRV9025_BF_EQUAL(init->tx.txInitChannelMask,
                              ADI_ADRV9025_TX3))
        {
            cpuChannels |= ADRV9025_TX3;
        }
        if (ADRV9025_BF_EQUAL(init->tx.txInitChannelMask,
                              ADI_ADRV9025_TX4))
        {
            cpuChannels |= ADRV9025_TX4;
        }
    }

    if (ADRV9025_BF_EQUAL(device->devStateInfo.profilesValid,
                          ADI_ADRV9025_ORX_PROFILE_VALID))
    {
        if (ADRV9025_BF_EQUAL(init->rx.rxInitChannelMask,
                              ADI_ADRV9025_ORX1))
        {
            cpuChannels |= ADRV9025_ORX1;
        }
        if (ADRV9025_BF_EQUAL(init->rx.rxInitChannelMask,
                              ADI_ADRV9025_ORX2))
        {
            cpuChannels |= ADRV9025_ORX2;
        }
        if (ADRV9025_BF_EQUAL(init->rx.rxInitChannelMask,
                              ADI_ADRV9025_ORX3))
        {
            cpuChannels |= ADRV9025_ORX3;
        }
        if (ADRV9025_BF_EQUAL(init->rx.rxInitChannelMask,
                              ADI_ADRV9025_ORX4))
        {
            cpuChannels |= ADRV9025_ORX4;
        }
    }

    if (ADRV9025_BF_EQUAL(device->devStateInfo.profilesValid,
                          ADI_ADRV9025_LB_PROFILE_VALID))
    {
        if (ADRV9025_BF_EQUAL(init->rx.rxInitChannelMask,
                              ADI_ADRV9025_LB12))
        {
            cpuChannels |= ADRV9025_LB12;
        }
        if (ADRV9025_BF_EQUAL(init->rx.rxInitChannelMask,
                              ADI_ADRV9025_LB34))
        {
            cpuChannels |= ADRV9025_LB34;
        }
    }

    cfgData[24] = (uint8_t)((cpuChannels) & 0x000000FF);
    cfgData[25] = (uint8_t)((cpuChannels >> 8) & 0x000000FF);

    if ((init->clocks.rfPll1LoOutDivider != 0) &&
        (init->clocks.rfPll1LoOutDivider != 1) &&
        (init->clocks.rfPll1LoOutDivider != 2) &&
        (init->clocks.rfPll1LoOutDivider != 4) &&
        (init->clocks.rfPll1LoOutDivider != 8) &&
        (init->clocks.rfPll1LoOutDivider != 16) &&
        (init->clocks.rfPll1LoOutDivider != 32) &&
        (init->clocks.rfPll1LoOutDivider != 64) &&
        (init->clocks.rfPll1LoOutDivider != 128))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         init->clocks.rfPll1LoOutDivider,
                         "Invalid clock rfPll1LoOutDivider. Valid values are 0, 1, 2, 4, 8, 16, 32, 64, 128");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    cfgData[26] = (uint8_t)(init->clocks.rfPll1LoOutDivider);

    if ((init->clocks.rfPll2LoOutDivider != 0) &&
        (init->clocks.rfPll2LoOutDivider != 1) &&
        (init->clocks.rfPll2LoOutDivider != 2) &&
        (init->clocks.rfPll2LoOutDivider != 4) &&
        (init->clocks.rfPll2LoOutDivider != 8) &&
        (init->clocks.rfPll2LoOutDivider != 16) &&
        (init->clocks.rfPll2LoOutDivider != 32) &&
        (init->clocks.rfPll2LoOutDivider != 64) &&
        (init->clocks.rfPll2LoOutDivider != 128))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         init->clocks.rfPll2LoOutDivider,
                         "Invalid clock rfPll2LoOutDivider. Valid values are 0, 1, 2, 4, 8, 16, 32, 64, 128");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    cfgData[27] = (uint8_t)(init->clocks.rfPll2LoOutDivider);

    /* Pass LDO Select to CPU */
    cfgData[28] = init->clocks.ldoSelect;
    cfgData[29] = 0;
    cfgData[30] = 0;
    cfgData[31] = 0;
    cfgData[32] = 0;
    cfgData[33] = 0;
    cfgData[34] = 0;
    cfgData[35] = 0;

    start = 36;

    profileAddr = device->devStateInfo.cpu.devProfileAddr;

    checksum = adrv9025_Crc32ForChunk(&cfgData[0],
                                      start,
                                      checksum,
                                      0);
    recoveryAction = adi_adrv9025_CpuMemWrite(device,
                                              profileAddr,
                                              &cfgData[0],
                                              start,
                                              ADI_ADRV9025_CPU_MEM_AUTO_INCR);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Error from adi_adrv9025_CpuMemWrite()");
    ADI_ERROR_RETURN(device->common.error.newAction);

    profileAddr += start; // offset = 36
    start = 0;
    adrv9025_cfgDataSet(&cfgData[0],
                        0,
                        ADRV9025_PROFILE_CHUNK_MAX);

    /* RX CONFIG 144 bytes */
    for (i = 0; i < 2; i++)
    {
        adrv9025_CpuLoadRxBytes(start,
                                &cfgData[0],
                                &init->rx.rxChannelCfg[i].profile);
        start += ADRV9025_PROFILE_RX;
    }

    checksum = adrv9025_Crc32ForChunk(&cfgData[0],
                                      start,
                                      checksum,
                                      0);
    recoveryAction = adi_adrv9025_CpuMemWrite(device,
                                              profileAddr,
                                              &cfgData[0],
                                              start,
                                              ADI_ADRV9025_CPU_MEM_AUTO_INCR);
    ADI_ERROR_RETURN(device->common.error.newAction);

    profileAddr += start; //offset = 180
    start = 0;
    adrv9025_cfgDataSet(&cfgData[0],
                        0,
                        ADRV9025_PROFILE_CHUNK_MAX);

    /* ORx config 144 bytes */
    for (i = 2; i < 4; i++)
    {
        adrv9025_CpuLoadRxBytes(start,
                                &cfgData[0],
                                &init->rx.rxChannelCfg[i].profile);
        start += ADRV9025_PROFILE_RX;
    }

    checksum = adrv9025_Crc32ForChunk(&cfgData[0],
                                      start,
                                      checksum,
                                      0);
    recoveryAction = adi_adrv9025_CpuMemWrite(device,
                                              profileAddr,
                                              &cfgData[0],
                                              start,
                                              ADI_ADRV9025_CPU_MEM_AUTO_INCR);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Error from adi_adrv9025_CpuMemWrite()");
    ADI_ERROR_RETURN(device->common.error.newAction);

    profileAddr += start; // offset = 324
    start = 0;
    adrv9025_cfgDataSet(&cfgData[0],
                        0,
                        ADRV9025_PROFILE_CHUNK_MAX);

    /* TX CONFIG 160 bytes */
    for (i = 0; i < 4; i++)
    {
        adrv9025_CpuLoadTxByte(start,
                               &cfgData[0],
                               &init->tx.txChannelCfg[i].profile);
        start += ADRV9025_PROFILE_TX;
    }

    checksum = adrv9025_Crc32ForChunk(&cfgData[0],
                                      start,
                                      checksum,
                                      0);
    recoveryAction = adi_adrv9025_CpuMemWrite(device,
                                              profileAddr,
                                              &cfgData[0],
                                              start,
                                              ADI_ADRV9025_CPU_MEM_AUTO_INCR);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Error from adi_adrv9025_CpuMemWrite()");
    ADI_ERROR_RETURN(device->common.error.newAction);

    profileAddr += start; // offset = 484
    start = 0;

    /* ORX CONFIG 224 bytes */
    for (i = 4; i < 8; i++)
    {
        adrv9025_CpuLoadOrxBytes(start,
                                 &cfgData[0],
                                 &init->rx.rxChannelCfg[i].profile);
        start += ADRV9025_PROFILE_ORX;
    }

    checksum = adrv9025_Crc32ForChunk(&cfgData[0],
                                      start,
                                      checksum,
                                      0);
    recoveryAction = adi_adrv9025_CpuMemWrite(device,
                                              profileAddr,
                                              &cfgData[0],
                                              start,
                                              ADI_ADRV9025_CPU_MEM_AUTO_INCR);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Error from adi_adrv9025_CpuMemWrite()");
    ADI_ERROR_RETURN(device->common.error.newAction);

    profileAddr += start; // offset = 708
    start = 0;
    adrv9025_cfgDataSet(&cfgData[0],
                        0,
                        ADRV9025_PROFILE_CHUNK_MAX);

    /* LB ORX CONFIG 112 bytes */
    for (i = 8; i < 10; i++)
    {
        adrv9025_CpuLoadOrxBytes(start,
                                 &cfgData[0],
                                 &init->rx.rxChannelCfg[i].profile);
        start += ADRV9025_PROFILE_ORX;
    }

    checksum = adrv9025_Crc32ForChunk(&cfgData[0],
                                      start,
                                      checksum,
                                      0);
    recoveryAction = adi_adrv9025_CpuMemWrite(device,
                                              profileAddr,
                                              &cfgData[0],
                                              start,
                                              ADI_ADRV9025_CPU_MEM_AUTO_INCR);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Error from adi_adrv9025_CpuMemWrite()");
    ADI_ERROR_RETURN(device->common.error.newAction);

    profileAddr += start; // offset = 820
    start = 0;
    adrv9025_cfgDataSet(&cfgData[0],
                        0,
                        ADRV9025_PROFILE_CHUNK_MAX);

    /* JESD CONFIG 342 bytes */
    /* Frammer 3 * 53byte = 159 bytes */
    for (i = 0; i < 3; i++)
    {
        adrv9025_CpuLoadFramerBytes(start,
                                    &cfgData[0],
                                    &init->dataInterface.framer[i]);
        start += ADRV9025_PROFILE_JESD_FRAMER;
    }

    checksum = adrv9025_Crc32ForChunk(&cfgData[0],
                                      start,
                                      checksum,
                                      0);
    recoveryAction = adi_adrv9025_CpuMemWrite(device,
                                              profileAddr,
                                              &cfgData[0],
                                              start,
                                              ADI_ADRV9025_CPU_MEM_AUTO_INCR);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Error from adi_adrv9025_CpuMemWrite()");
    ADI_ERROR_RETURN(device->common.error.newAction);

    profileAddr += start; // offset = 979
    start = 0;
    adrv9025_cfgDataSet(&cfgData[0],
                        0,
                        ADRV9025_PROFILE_CHUNK_MAX);

    /* Deframer: 2 * 38 bytes = 76 bytes */
    for (i = 0; i < 2; i++)
    {
        adrv9025_CpuLoadDeframerBytes(start,
                                      &cfgData[0],
                                      &init->dataInterface.deframer[i]);
        start += ADRV9025_PROFILE_JESD_DEFRAMER;
    }

    checksum = adrv9025_Crc32ForChunk(&cfgData[0],
                                      start,
                                      checksum,
                                      0);
    recoveryAction = adi_adrv9025_CpuMemWrite(device,
                                              profileAddr,
                                              &cfgData[0],
                                              start,
                                              ADI_ADRV9025_CPU_MEM_AUTO_INCR);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Error from adi_adrv9025_CpuMemWrite()");
    ADI_ERROR_RETURN(device->common.error.newAction);

    profileAddr += start; // offset = 1055
    start = 0;
    adrv9025_cfgDataSet(&cfgData[0],
                        0,
                        ADRV9025_PROFILE_CHUNK_MAX);

    /* Serializer ADI_ADRV9025_NUM_LANES * 8 bytes = 32 bytes */
    for (i = 0; i < ADI_ADRV9025_NUM_LANES; i++)
    {
        adrv9025_CpuLoadSerBytes(start,
                                 &cfgData[0],
                                 &init->dataInterface.serCfg[i]);
        start += ADRV9025_PROFILE_JESD_SERCFG;
    }

    cfgData[start++] = 0;

    /* Deserializer ADI_ADRV9025_NUM_LANES * 44 bytes = 176 bytes */
    for (i = 0; i < ADI_ADRV9025_NUM_LANES; i++)
    {
        adrv9025_CpuLoadDesBytes(start,
                                 &cfgData[0],
                                 &init->dataInterface.desCfg[i]);
        start += ADRV9025_PROFILE_JESD_DESCFG;
    }

    /* Link sharing 20 bytes */
    adrv9025_CpuLoadLinkSharing(start,
                                &cfgData[0],
                                &init->dataInterface.linkSharingCfg);
    start += ADRV9025_PROFILE_JESD_LINKSHARING;

    /* Data configuration 3 bytes */
    adrv9025_CpuLoadDataConfig(start,
                               &cfgData[0],
                               &init->dataInterface.dataCfg);
    start += ADRV9025_PROFILE_JESD_DATACONFIG;

    checksum = adrv9025_Crc32ForChunk(&cfgData[0],
                                      start,
                                      checksum,
                                      0);
    recoveryAction = adi_adrv9025_CpuMemWrite(device,
                                              profileAddr,
                                              &cfgData[0],
                                              start,
                                              ADI_ADRV9025_CPU_MEM_AUTO_INCR);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Error from adi_adrv9025_CpuMemWrite()");
    ADI_ERROR_RETURN(device->common.error.newAction);

    profileAddr += start; // offset = 1287 (extra padded byte before deserializer structs)
    start = 0;
    adrv9025_cfgDataSet(&cfgData[0],
                        0,
                        ADRV9025_PROFILE_CHUNK_MAX);

    /* Jesd 8 bytes */
    cfgData[start + 0] = init->dataInterface.channelSelect;
    cfgData[start + 1] = init->dataInterface.channelMode;
    cfgData[start + 2] = 0;
    cfgData[start + 3] = 0;
    cfgData[start + 4] = 0;
    cfgData[start + 5] = 0;
    cfgData[start + 6] = 0;
    cfgData[start + 7] = 0;

    start += ADRV9025_PROFILE_JESD_END;

    cfgData[start++] = 0;

    /* Noise Cancellation 36 bytes */
    adrv9025_CpuLoadNoiseCancelBytes(start,
                                     &cfgData[0]);
    start += ADRV9025_PROFILE_NOISE_CANCEL;

    /* Reserved 16 bytes */
    adrv9025_CpuLoadFourBytes(start + 0,
                              &cfgData[0],
                              0);
    adrv9025_CpuLoadFourBytes(start + 4,
                              &cfgData[0],
                              0);
    adrv9025_CpuLoadFourBytes(start + 8,
                              &cfgData[0],
                              0);
    adrv9025_CpuLoadFourBytes(start + 12,
                              &cfgData[0],
                              0);

    start += 16;
    /* Profile Checksum 4 bytes */
    checksum = adrv9025_Crc32ForChunk(&cfgData[0],
                                      start,
                                      checksum,
                                      1); /* last checksum */
    adrv9025_CpuLoadFourBytes(start,
                              &cfgData[0],
                              checksum);
    start += 4;

    /* Final bytes */
    recoveryAction = adi_adrv9025_CpuMemWrite(device,
                                              profileAddr,
                                              &cfgData[0],
                                              start,
                                              ADI_ADRV9025_CPU_MEM_AUTO_INCR);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Error from adi_adrv9025_CpuMemWrite()");
    ADI_ERROR_RETURN(device->common.error.newAction);

    profileAddr += start; // offset = 1352 (extra padded byte before noise cancel struct)
    start = 0;

    if (profileAddr != (device->devStateInfo.cpu.devProfileAddr + ADRV9025_PROFILE_MAX))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         profileAddr,
                         "Exceed Max Profile bytes");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return (device->common.error.newAction);
}
int32_t adrv9025_CpuSerdesTestModeEnter(adi_adrv9025_Device_t* device)
{
    int32_t  recoverAction  = ADI_COMMON_ACT_NO_ACTION;
    uint8_t  extData[4]     = { 0 };
    uint8_t  cmdStatusByte  = 0;

    static const uint8_t SERDES_QR_MODE   = 1;
    static const uint8_t ARM_ERR_CODE     = 0x0E;
    static const uint8_t SERDES_TEST_MODE = 0x08;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API_PRIV);

    /* Arm Test Mode Configuration */
    extData[0] = ADRV9025_CPU_OBJECTID_GS_TEST_MODE;
    extData[1] = SERDES_TEST_MODE;                    /* test_mode_serdes */
    extData[2] = SERDES_QR_MODE;                      /* serdes qr mode */

    /* Write the extData - arm configuration to ADI_ADRV9025_CPU_TYPE_C */
    recoverAction = adi_adrv9025_CpuCmdWrite(device,
                                             ADI_ADRV9025_CPU_TYPE_C,
                                             (uint8_t)ADRV9025_CPU_SET_OPCODE,
                                             &extData[0],
                                             4);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoverAction,
                     NULL,
                     "Failed to write ARM Command");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Wait up to a maximum of ADI_ADRV9025_TEST_MODE_TIMEOUT_US checking for set opcode status to return */
    recoverAction = adi_adrv9025_CpuCmdStatusWait(device,
                                                  ADI_ADRV9025_CPU_TYPE_C,
                                                  (uint8_t)ADRV9025_CPU_SET_OPCODE,
                                                  &cmdStatusByte,
                                                  (uint32_t)ADI_ADRV9025_TEST_MODE_TIMEOUT_US,
                                                  (uint32_t)ADI_ADRV9025_TEST_MODE_INTERVAL_US);

    /* If cmdStatusByte is non-zero then flag an ARM error */
    if ((cmdStatusByte & ARM_ERR_CODE) > 0)
    {
        recoverAction = adrv9025_CpuCmdErrorHandler(device,
                                                    ADI_ADRV9025_CPU_TYPE_C,
                                                    ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                                                             extData[0],
                                                                             cmdStatusByte),
                                                    recoverAction,
                                                    ADI_ADRV9025_ACT_ERR_RESET_CPU);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return recoverAction;
}

int32_t adrv9025_CpuSerdesTestModeExit(adi_adrv9025_Device_t* device)
{
    int32_t               recoverAction  = ADI_COMMON_ACT_NO_ACTION;
    uint8_t               extData[4]     = { 0 };
    uint8_t               cmdStatusByte  = 0;

    static const uint8_t  ARM_ERR_CODE   = 0x0E;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API_PRIV);

    /* Arm Test Mode Exit config */
    extData[0] = ADRV9025_CPU_OBJECTID_TM_EXIT;
    extData[1] = 0;

    /* Exit Serdes Test Mode */
    recoverAction = adi_adrv9025_CpuCmdWrite(device,
                                             ADI_ADRV9025_CPU_TYPE_C,
                                             (uint8_t)ADRV9025_CPU_TEST_OPCODE,
                                             &extData[0],
                                             4);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoverAction,
                     NULL,
                     "Failed to write ARM Command");
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoverAction = adi_adrv9025_CpuCmdStatusWait(device,
                                                  ADI_ADRV9025_CPU_TYPE_C,
                                                  (uint8_t)ADRV9025_CPU_TEST_OPCODE,
                                                  &cmdStatusByte,
                                                  (uint32_t)ADI_ADRV9025_TEST_MODE_TIMEOUT_US,
                                                  (uint32_t)ADI_ADRV9025_TEST_MODE_INTERVAL_US);
    /* If cmdStatusByte is non-zero then flag an ARM error */
    if ((cmdStatusByte & ARM_ERR_CODE) > 0)
    {
        recoverAction = adrv9025_CpuCmdErrorHandler(device,
                                                    ADI_ADRV9025_CPU_TYPE_C,
                                                    ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_TEST_OPCODE,
                                                                                extData[0],
                                                                                cmdStatusByte),
                                                    recoverAction,
                                                    ADI_ADRV9025_ACT_ERR_RESET_CPU);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    return recoverAction;
}


static uint8_t adrv9025_DesCfgHasData(adi_adrv9025_Device_t*      device,
                                    adi_adrv9025_DesCfg_t*      config)
{
    static const uint32_t ADRV9025_JESD_DESCFG_LANES = 4;
    uint8_t   i        = 0;
    uint8_t  retVal    = 0;

    for (i = 0; i < ADRV9025_JESD_DESCFG_LANES; i++)
    {
        if ((config[i].configOption1 != 0) ||
            (config[i].configOption2 != 0) ||
            (config[i].configOption3 != 0) ||
            (config[i].configOption4 != 0) ||
            (config[i].configOption5 != 0) ||
            (config[i].configOption6 != 0) ||
            (config[i].configOption7 != 0) ||
            (config[i].configOption8 != 0) ||
            (config[i].configOption9 != 0) ||
            (config[i].configOption10 != 0))
        {
            retVal = 1;
            break;
        }
    }

    return retVal;
}

int32_t adrv9025_JesdDesCfgSet(adi_adrv9025_Device_t *device,
                               adi_adrv9025_Init_t   *init)
{
    static const uint32_t ADRV9025_PROFILE_JESD_DESCFG_OFFSET       = 1088;
    static const uint32_t ADRV9025_PROFILE_JESD_DESCFG              = 44;
    static const uint32_t ADRV9025_PROFILE_JESD_DESCFG_LENGTH_BYTES = 4 * 44;

    int32_t recoverAction  = ADI_COMMON_ACT_NO_ACTION;
    uint8_t cfgData[4*44]  = { 0 };
    uint8_t i       = 0;
    uint8_t hasData = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_NULL_DEVICE_PTR_RETURN(init);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API_PRIV);

    hasData = adrv9025_DesCfgHasData(device, init->dataInterface.desCfg);

    if ((init->dataInterface.deframer[0].enableJesd204C == 1) && (hasData == 0))
    {
        /* Check Silicon Rev */
        if ((device->devStateInfo.deviceSiRev & 0xB0) == 0xB0)
        {
            init->dataInterface.desCfg[0].configOption1  = 806223887U;
            init->dataInterface.desCfg[0].configOption2  = 51123472U;
            init->dataInterface.desCfg[0].configOption3  = 1129575U;
            init->dataInterface.desCfg[0].configOption4  = 2690707553U;
            init->dataInterface.desCfg[0].configOption5  = 1349654899U;
            init->dataInterface.desCfg[0].configOption6  = 386304022U;
            init->dataInterface.desCfg[0].configOption7  = 1578626072U;
            init->dataInterface.desCfg[0].configOption8  = 4279845916U;
            init->dataInterface.desCfg[0].configOption9  = 1209860126U;
            init->dataInterface.desCfg[0].configOption10 = 2031636U;
            init->dataInterface.desCfg[0].highBoost      = 162U;

            init->dataInterface.desCfg[1].configOption1  = 714081178U;
            init->dataInterface.desCfg[1].configOption2  = 27525806U;
            init->dataInterface.desCfg[1].configOption3  = 817365442U;
            init->dataInterface.desCfg[1].configOption4  = 1088291812U;
            init->dataInterface.desCfg[1].configOption5  = 82119142U;
            init->dataInterface.desCfg[1].configOption6  = 4254142363U;
            init->dataInterface.desCfg[1].configOption7  = 421U;
            init->dataInterface.desCfg[1].configOption8  = 0U;
            init->dataInterface.desCfg[1].configOption9  = 0U;
            init->dataInterface.desCfg[1].configOption10 = 0U;
            init->dataInterface.desCfg[1].highBoost      = 106U;

            init->dataInterface.desCfg[2].configOption1  = 1007919116U;
            init->dataInterface.desCfg[2].configOption2  = 4027383838U;
            init->dataInterface.desCfg[2].configOption3  = 52404284U;
            init->dataInterface.desCfg[2].configOption4  = 4030529862U;
            init->dataInterface.desCfg[2].configOption5  = 90966637U;
            init->dataInterface.desCfg[2].configOption6  = 7277168U;
            init->dataInterface.desCfg[2].configOption7  = 0U;
            init->dataInterface.desCfg[2].configOption8  = 0U;
            init->dataInterface.desCfg[2].configOption9  = 0U;
            init->dataInterface.desCfg[2].configOption10 = 0U;
            init->dataInterface.desCfg[2].highBoost      = 100U;

            init->dataInterface.desCfg[3].configOption1  = 0U;
            init->dataInterface.desCfg[3].configOption2  = 0U;
            init->dataInterface.desCfg[3].configOption3  = 0U;
            init->dataInterface.desCfg[3].configOption4  = 0U;
            init->dataInterface.desCfg[3].configOption5  = 0U;
            init->dataInterface.desCfg[3].configOption6  = 0U;
            init->dataInterface.desCfg[3].configOption7  = 0U;
            init->dataInterface.desCfg[3].configOption8  = 0U;
            init->dataInterface.desCfg[3].configOption9  = 0U;
            init->dataInterface.desCfg[3].configOption10 = 0U;
            init->dataInterface.desCfg[3].highBoost      = 0U;
        }
        else
        {
            init->dataInterface.desCfg[0].configOption1  = 925455U;
            init->dataInterface.desCfg[0].configOption2  = 17563664U;
            init->dataInterface.desCfg[0].configOption3  = 34684007U;
            init->dataInterface.desCfg[0].configOption4  = 2690707553U;
            init->dataInterface.desCfg[0].configOption5  = 1349661043U;
            init->dataInterface.desCfg[0].configOption6  = 386322966U;
            init->dataInterface.desCfg[0].configOption7  = 1696065816U;
            init->dataInterface.desCfg[0].configOption8  = 4279868700U;
            init->dataInterface.desCfg[0].configOption9  = 354286110U;
            init->dataInterface.desCfg[0].configOption10 = 4280260130U;
            init->dataInterface.desCfg[0].highBoost      = 162U;

            init->dataInterface.desCfg[1].configOption1  = 1008952340U;
            init->dataInterface.desCfg[1].configOption2  = 85324176U;
            init->dataInterface.desCfg[1].configOption3  = 1620705444U;
            init->dataInterface.desCfg[1].configOption4  = 714149019U;
            init->dataInterface.desCfg[1].configOption5  = 10875282U;
            init->dataInterface.desCfg[1].configOption6  = 530317990U;
            init->dataInterface.desCfg[1].configOption7  = 1672413112U;
            init->dataInterface.desCfg[1].configOption8  = 12729298U;
            init->dataInterface.desCfg[1].configOption9  = 601034708U;
            init->dataInterface.desCfg[1].configOption10 = 417206232U;
            init->dataInterface.desCfg[1].highBoost      = 162U;

            init->dataInterface.desCfg[2].configOption1  = 98107862U;
            init->dataInterface.desCfg[2].configOption2  = 4259447780U;
            init->dataInterface.desCfg[2].configOption3  = 0U;
            init->dataInterface.desCfg[2].configOption4  = 0U;
            init->dataInterface.desCfg[2].configOption5  = 0U;
            init->dataInterface.desCfg[2].configOption6  = 0U;
            init->dataInterface.desCfg[2].configOption7  = 0U;
            init->dataInterface.desCfg[2].configOption8  = 0U;
            init->dataInterface.desCfg[2].configOption9  = 0U;
            init->dataInterface.desCfg[2].configOption10 = 0U;
            init->dataInterface.desCfg[2].highBoost      = 34U;

            init->dataInterface.desCfg[3].configOption1  = 1007885845U;
            init->dataInterface.desCfg[3].configOption2  = 1344118796U;
            init->dataInterface.desCfg[3].configOption3  = 4027383838U;
            init->dataInterface.desCfg[3].configOption4  = 52404284U;
            init->dataInterface.desCfg[3].configOption5  = 4030529862U;
            init->dataInterface.desCfg[3].configOption6  = 90966637U;
            init->dataInterface.desCfg[3].configOption7  = 7277168U;
            init->dataInterface.desCfg[3].configOption8  = 0U;
            init->dataInterface.desCfg[3].configOption9  = 0U;
            init->dataInterface.desCfg[3].configOption10 = 0U;
            init->dataInterface.desCfg[3].highBoost      = 116U;
        }

        /* Deserialize desCfg data to cfgData to be transferred to Memory */
        for (i = 0; i < ADI_ADRV9025_NUM_LANES; i++)
        {
            adrv9025_CpuLoadDesBytes(i * ADRV9025_PROFILE_JESD_DESCFG,
                                     &cfgData[0],
                                     &init->dataInterface.desCfg[i]);
        }

        recoverAction = adi_adrv9025_CpuConfigWrite(device,
                                                    ADI_ADRV9025_CPU_TYPE_C,
                                                    ADRV9025_CPU_OBJECTID_DEVICE_PROFILE,
                                                    ADRV9025_PROFILE_JESD_DESCFG_OFFSET,
                                                    &cfgData[0],
                                                    ADRV9025_PROFILE_JESD_DESCFG_LENGTH_BYTES);
    }

    return recoverAction;
}
