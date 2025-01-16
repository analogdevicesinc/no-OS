#ifndef _FTDI_LIB_FT4222_H_
#define _FTDI_LIB_FT4222_H_
//------------------------------------------------------------------------------
/**
 * Copyright (c) 2001-2015 Future Technology Devices International Limited
 *
 * THIS SOFTWARE IS PROVIDED BY FUTURE TECHNOLOGY DEVICES INTERNATIONAL LIMITED "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * FUTURE TECHNOLOGY DEVICES INTERNATIONAL LIMITED BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES LOSS OF USE, DATA, OR PROFITS OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * @file LibFT4222.h
 *
 * @author FTDI
 * @date 2015-08-10
 *
 *
 * Revision History:
 * 1.0 - initial version
 * 1.2 - allow use on non-Windows platforms;
 *       add I2CMaster ReadEx/WriteEx/GetStatus
 * 1.3   add FT4222_I2CSlave_SetClockStretch
 *           FT4222_I2CSlave_SetRespWord
 */
 
#ifdef _WIN32
    // Compiling on Windows
    #include <windows.h>

    typedef UINT8  uint8;
    typedef UINT16 uint16;
    typedef UINT32 uint32;
    typedef UINT64 uint64;
    typedef INT8   int8;
    typedef INT16  int16;
    typedef INT32  int32;
    typedef INT64  int64;

    #ifdef LIBFT4222_EXPORTS
        #define LIBFT4222_API __declspec(dllexport)
    #else
        #define LIBFT4222_API __declspec(dllimport)
    #endif

#else // _WIN32
    // Compiling on non-Windows platform.
    #include <stdint.h>

    typedef uint8_t  uint8;
    typedef uint16_t uint16;
    typedef uint32_t uint32;
    typedef uint64_t uint64;
    typedef int8_t   int8;
    typedef int16_t  int16;
    typedef int32_t  int32;
    typedef int64_t  int64;

    // No function-name decorations needed.
    #define LIBFT4222_API

#endif // _WIN32

#ifdef _MSC_VER
    // Prevent MS compiler warning about bad character
    #pragma warning(disable: 4819)
#endif // _MSC_VER

#include "ftd2xx.h"

#define FT4222_EVENT_RXCHAR 8
// spi slave sync word
#define FT4222_SPI_SLAVE_SYNC_WORD   0x5A

// spi slave cmd
#define SPI_MASTER_TRANSFER             0x80
#define SPI_SLAVE_TRANSFER              0x81
#define SPI_SHORT_MASTER_TRANSFER       0x82
#define SPI_SHORT_SLAVE_TRANSFER        0x83
// Backwards compatibility for typo in earlier version
#define SPI_SHART_SLAVE_TRANSFER        0x83
#define SPI_ACK                         0x84

/* I2C Master Controller Status
 *   bit 0 = controller busy: all other status bits invalid
 *   bit 1 = error condition
 *   bit 2 = slave address was not acknowledged during last operation
 *   bit 3 = data not acknowledged during last operation
 *   bit 4 = arbitration lost during last operation
 *   bit 5 = controller idle
 *   bit 6 = bus busy
 */
#define I2CM_CONTROLLER_BUSY(status) (((status) & 0x01) != 0)
#define I2CM_DATA_NACK(status)       (((status) & 0x0A) != 0)
#define I2CM_ADDRESS_NACK(status)    (((status) & 0x06) != 0)
#define I2CM_ARB_LOST(status)        (((status) & 0x12) != 0)
#define I2CM_IDLE(status)            (((status) & 0x20) != 0)
#define I2CM_BUS_BUSY(status)        (((status) & 0x40) != 0)

//
// FT4222 Device status
//
typedef enum FT4222_STATUS
{
    FT4222_OK,
    FT4222_INVALID_HANDLE,
    FT4222_DEVICE_NOT_FOUND,
    FT4222_DEVICE_NOT_OPENED,
    FT4222_IO_ERROR,
    FT4222_INSUFFICIENT_RESOURCES,
    FT4222_INVALID_PARAMETER,
    FT4222_INVALID_BAUD_RATE,
    FT4222_DEVICE_NOT_OPENED_FOR_ERASE,
    FT4222_DEVICE_NOT_OPENED_FOR_WRITE,
    FT4222_FAILED_TO_WRITE_DEVICE,
    FT4222_EEPROM_READ_FAILED,
    FT4222_EEPROM_WRITE_FAILED,
    FT4222_EEPROM_ERASE_FAILED,
    FT4222_EEPROM_NOT_PRESENT,
    FT4222_EEPROM_NOT_PROGRAMMED,
    FT4222_INVALID_ARGS,
    FT4222_NOT_SUPPORTED,
    FT4222_OTHER_ERROR,
    FT4222_DEVICE_LIST_NOT_READY,

    FT4222_DEVICE_NOT_SUPPORTED   = 1000,        // FT_STATUS extending message
    FT4222_CLK_NOT_SUPPORTED,
    FT4222_VENDER_CMD_NOT_SUPPORTED,
    FT4222_IS_NOT_SPI_MODE,
    FT4222_IS_NOT_I2C_MODE,
    FT4222_IS_NOT_SPI_SINGLE_MODE,
    FT4222_IS_NOT_SPI_MULTI_MODE,
    FT4222_WRONG_I2C_ADDR,
    FT4222_INVAILD_FUNCTION,
    FT4222_INVALID_POINTER,
    FT4222_EXCEEDED_MAX_TRANSFER_SIZE,
    FT4222_FAILED_TO_READ_DEVICE,
    FT4222_I2C_NOT_SUPPORTED_IN_THIS_MODE,
    FT4222_GPIO_NOT_SUPPORTED_IN_THIS_MODE,
    FT4222_GPIO_EXCEEDED_MAX_PORTNUM,
    FT4222_GPIO_WRITE_NOT_SUPPORTED,
    FT4222_GPIO_PULLUP_INVALID_IN_INPUTMODE,
    FT4222_GPIO_PULLDOWN_INVALID_IN_INPUTMODE,
    FT4222_GPIO_OPENDRAIN_INVALID_IN_OUTPUTMODE,
    FT4222_INTERRUPT_NOT_SUPPORTED,
    FT4222_GPIO_INPUT_NOT_SUPPORTED,
    FT4222_EVENT_NOT_SUPPORTED,
    FT4222_FUN_NOT_SUPPORT,
}
FT4222_STATUS;


typedef enum FT4222_ClockRate
{
    SYS_CLK_60 = 0,
    SYS_CLK_24,
    SYS_CLK_48,
    SYS_CLK_80,

}
FT4222_ClockRate;

typedef enum FT4222_FUNCTION
{
    FT4222_I2C_MASTER = 1,
    FT4222_I2C_SLAVE,
    FT4222_SPI_MASTER,
    FT4222_SPI_SLAVE,

}
FT4222_FUNCTION;

typedef enum FT4222_SPIMode
{
    SPI_IO_NONE = 0,
    SPI_IO_SINGLE = 1,
    SPI_IO_DUAL = 2,
    SPI_IO_QUAD = 4,

}
FT4222_SPIMode;

typedef enum FT4222_SPIClock
{
    CLK_NONE =  0,
    CLK_DIV_2,      // 1/2   System Clock
    CLK_DIV_4,      // 1/4   System Clock
    CLK_DIV_8,      // 1/8   System Clock
    CLK_DIV_16,     // 1/16  System Clock
    CLK_DIV_32,     // 1/32  System Clock
    CLK_DIV_64,     // 1/64  System Clock
    CLK_DIV_128,    // 1/128 System Clock
    CLK_DIV_256,    // 1/256 System Clock
    CLK_DIV_512,    // 1/512 System Clock

}
FT4222_SPIClock;

typedef enum FT4222_SPICPOL
{
    CLK_IDLE_LOW  =0,
    CLK_IDLE_HIGH =1,
}
FT4222_SPICPOL;

typedef enum FT4222_SPICPHA
{
    CLK_LEADING  =0,
    CLK_TRAILING =1,
}
FT4222_SPICPHA;

typedef enum SPI_DrivingStrength
{
    DS_4MA  =0,
    DS_8MA,
    DS_12MA,
    DS_16MA,
}
SPI_DrivingStrength;

typedef enum GPIO_Port
{
    GPIO_PORT0  =0,
    GPIO_PORT1,
    GPIO_PORT2,
    GPIO_PORT3
}
GPIO_Port;

typedef enum GPIO_Dir
{
    GPIO_OUTPUT =0,
    GPIO_INPUT,
}
GPIO_Dir;

// Backwards compatibility for typo in earlier version
#define GPIO_Tigger GPIO_Trigger
typedef enum GPIO_Trigger
{
    GPIO_TRIGGER_RISING         = 0x01,
    GPIO_TRIGGER_FALLING        = 0x02,
    GPIO_TRIGGER_LEVEL_HIGH     = 0x04,
    GPIO_TRIGGER_LEVEL_LOW      = 0X08
}
GPIO_Trigger;


typedef enum GPIO_Output
{
    GPIO_OUTPUT_LOW,
    GPIO_OUTPUT_HIGH
}
GPIO_Output;


typedef enum I2C_MasterFlag
{
    NONE = 0x80,
    START = 0x02,
    Repeated_START = 0x03,     // Repeated_START will not send master code in HS mode
    STOP  = 0x04,
    START_AND_STOP = 0x06,      // START condition followed by SEND and STOP condition
}
I2C_MasterFlag;


typedef enum SPI_SlaveProtocol
{
    SPI_SLAVE_WITH_PROTOCOL = 0,
    SPI_SLAVE_NO_PROTOCOL,    
    SPI_SLAVE_NO_ACK,
}
SPI_SlaveProtocol;



typedef struct FT4222_Version
{
    DWORD chipVersion;
    DWORD dllVersion;
}
FT4222_Version;

#pragma pack(push, 1)
struct SPI_Slave_Header
{
    uint8           syncWord;
    uint8           cmd;
    uint8           sn;
    uint16          size;
};

#pragma pack(pop)

#ifdef __cplusplus
extern "C" {
#endif



// FT4222 General Functions
LIBFT4222_API FT4222_STATUS FT4222_UnInitialize(FT_HANDLE ftHandle);
LIBFT4222_API FT4222_STATUS FT4222_SetClock(FT_HANDLE ftHandle, FT4222_ClockRate clk);
LIBFT4222_API FT4222_STATUS FT4222_GetClock(FT_HANDLE ftHandle, FT4222_ClockRate* clk);
LIBFT4222_API FT4222_STATUS FT4222_SetWakeUpInterrupt(FT_HANDLE ftHandle, BOOL enable);
LIBFT4222_API FT4222_STATUS FT4222_SetInterruptTrigger(FT_HANDLE ftHandle, GPIO_Trigger trigger);
LIBFT4222_API FT4222_STATUS FT4222_SetSuspendOut(FT_HANDLE ftHandle, BOOL enable);
LIBFT4222_API FT4222_STATUS FT4222_GetMaxTransferSize(FT_HANDLE ftHandle, uint16* pMaxSize);
LIBFT4222_API FT4222_STATUS FT4222_SetEventNotification(FT_HANDLE ftHandle, DWORD mask, PVOID param);
LIBFT4222_API FT4222_STATUS FT4222_GetVersion(FT_HANDLE ftHandle, FT4222_Version* pVersion);
LIBFT4222_API FT4222_STATUS FT4222_ChipReset(FT_HANDLE ftHandle);



// FT4222 SPI Functions
LIBFT4222_API FT4222_STATUS FT4222_SPIMaster_Init(FT_HANDLE ftHandle, FT4222_SPIMode  ioLine, FT4222_SPIClock clock, FT4222_SPICPOL  cpol, FT4222_SPICPHA  cpha, uint8 ssoMap);
LIBFT4222_API FT4222_STATUS FT4222_SPIMaster_SetLines(FT_HANDLE ftHandle, FT4222_SPIMode spiMode);
LIBFT4222_API FT4222_STATUS FT4222_SPIMaster_SingleRead(FT_HANDLE ftHandle, uint8* buffer, uint16 bufferSize, uint16* sizeOfRead, BOOL isEndTransaction);
LIBFT4222_API FT4222_STATUS FT4222_SPIMaster_SingleWrite(FT_HANDLE ftHandle, uint8* buffer, uint16 bufferSize, uint16* sizeTransferred, BOOL isEndTransaction);
LIBFT4222_API FT4222_STATUS FT4222_SPIMaster_SingleReadWrite(FT_HANDLE ftHandle, uint8* const readBuffer, uint8* writeBuffer, uint16 bufferSize, uint16* sizeTransferred, BOOL isEndTransaction);
LIBFT4222_API FT4222_STATUS FT4222_SPIMaster_MultiReadWrite(FT_HANDLE ftHandle, uint8* readBuffer, uint8* writeBuffer, uint8 singleWriteBytes, uint16 multiWriteBytes, uint16 multiReadBytes, uint32* sizeOfRead);

LIBFT4222_API FT4222_STATUS FT4222_SPISlave_Init(FT_HANDLE ftHandle);
LIBFT4222_API FT4222_STATUS FT4222_SPISlave_InitEx(FT_HANDLE ftHandle, SPI_SlaveProtocol protocolOpt);
LIBFT4222_API FT4222_STATUS FT4222_SPISlave_SetMode(FT_HANDLE ftHandle, FT4222_SPICPOL  cpol, FT4222_SPICPHA  cpha);
LIBFT4222_API FT4222_STATUS FT4222_SPISlave_GetRxStatus(FT_HANDLE ftHandle, uint16* pRxSize);
LIBFT4222_API FT4222_STATUS FT4222_SPISlave_Read(FT_HANDLE ftHandle, uint8* buffer, uint16 bufferSize, uint16* sizeOfRead);
LIBFT4222_API FT4222_STATUS FT4222_SPISlave_Write(FT_HANDLE ftHandle, uint8* buffer, uint16 bufferSize, uint16* sizeTransferred);
LIBFT4222_API FT4222_STATUS FT4222_SPISlave_RxQuickResponse(FT_HANDLE ftHandle, BOOL enable);

LIBFT4222_API FT4222_STATUS FT4222_SPI_Reset(FT_HANDLE ftHandle);
LIBFT4222_API FT4222_STATUS FT4222_SPI_ResetTransaction(FT_HANDLE ftHandle, uint8 spiIdx);
LIBFT4222_API FT4222_STATUS FT4222_SPI_SetDrivingStrength(FT_HANDLE ftHandle, SPI_DrivingStrength clkStrength, SPI_DrivingStrength ioStrength, SPI_DrivingStrength ssoStrength);



// FT4222 I2C Functions
LIBFT4222_API FT4222_STATUS FT4222_I2CMaster_Init(FT_HANDLE ftHandle, uint32 kbps);
LIBFT4222_API FT4222_STATUS FT4222_I2CMaster_Read(FT_HANDLE ftHandle, uint16 deviceAddress, uint8* buffer, uint16 bufferSize, uint16* sizeTransferred);
LIBFT4222_API FT4222_STATUS FT4222_I2CMaster_Write(FT_HANDLE ftHandle, uint16 deviceAddress, uint8* buffer, uint16 bufferSize, uint16* sizeTransferred);
LIBFT4222_API FT4222_STATUS FT4222_I2CMaster_ReadEx(FT_HANDLE ftHandle, uint16 deviceAddress, uint8 flag, uint8* buffer, uint16 bufferSize, uint16* sizeTransferred);
LIBFT4222_API FT4222_STATUS FT4222_I2CMaster_WriteEx(FT_HANDLE ftHandle, uint16 deviceAddress, uint8 flag, uint8* buffer, uint16 bufferSize, uint16* sizeTransferred);
LIBFT4222_API FT4222_STATUS FT4222_I2CMaster_Reset(FT_HANDLE ftHandle);
LIBFT4222_API FT4222_STATUS FT4222_I2CMaster_GetStatus(FT_HANDLE ftHandle, uint8 *controllerStatus);


LIBFT4222_API FT4222_STATUS FT4222_I2CSlave_Init(FT_HANDLE ftHandle);
LIBFT4222_API FT4222_STATUS FT4222_I2CSlave_Reset(FT_HANDLE ftHandle);
LIBFT4222_API FT4222_STATUS FT4222_I2CSlave_GetAddress(FT_HANDLE ftHandle, uint8* addr);
LIBFT4222_API FT4222_STATUS FT4222_I2CSlave_SetAddress(FT_HANDLE ftHandle, uint8 addr);
LIBFT4222_API FT4222_STATUS FT4222_I2CSlave_GetRxStatus(FT_HANDLE ftHandle, uint16* pRxSize);
LIBFT4222_API FT4222_STATUS FT4222_I2CSlave_Read(FT_HANDLE ftHandle, uint8* buffer, uint16 bufferSize, uint16* sizeTransferred);
LIBFT4222_API FT4222_STATUS FT4222_I2CSlave_Write(FT_HANDLE ftHandle, uint8* buffer, uint16 bufferSize, uint16* sizeTransferred);
LIBFT4222_API FT4222_STATUS FT4222_I2CSlave_SetClockStretch(FT_HANDLE ftHandle, BOOL enable);
LIBFT4222_API FT4222_STATUS FT4222_I2CSlave_SetRespWord(FT_HANDLE ftHandle, uint8 responseWord);



// FT4222 GPIO Functions
LIBFT4222_API FT4222_STATUS FT4222_GPIO_Init(FT_HANDLE ftHandle, GPIO_Dir gpioDir[4]);
LIBFT4222_API FT4222_STATUS FT4222_GPIO_Read(FT_HANDLE ftHandle, GPIO_Port portNum, BOOL* value);
LIBFT4222_API FT4222_STATUS FT4222_GPIO_Write(FT_HANDLE ftHandle, GPIO_Port portNum, BOOL bValue);
LIBFT4222_API FT4222_STATUS FT4222_GPIO_SetInputTrigger(FT_HANDLE ftHandle, GPIO_Port portNum, GPIO_Trigger trigger);
LIBFT4222_API FT4222_STATUS FT4222_GPIO_GetTriggerStatus(FT_HANDLE ftHandle, GPIO_Port portNum, uint16* queueSize);
LIBFT4222_API FT4222_STATUS FT4222_GPIO_ReadTriggerQueue(FT_HANDLE ftHandle, GPIO_Port portNum, GPIO_Trigger* events, uint16 readSize, uint16* sizeofRead);
LIBFT4222_API FT4222_STATUS FT4222_GPIO_SetWaveFormMode(FT_HANDLE ftHandle, BOOL enable);


 
#ifdef __cplusplus
}
#endif

//------------------------------------------------------------------------------
#endif //_FTDI_LIB_FT4222_H_
