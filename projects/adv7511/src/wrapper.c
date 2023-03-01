/**********************************************************************************************
*																						      *
* Copyright (c) 2012 Analog Devices, Inc.  All Rights Reserved.                               *
* This software is proprietary and confidential to Analog Devices, Inc. and its licensors.    *
*                                                                                             *
***********************************************************************************************/
#include "no_os_timer.h"
#include "wrapper.h"
#include "no_os_i2c.h"
#include "tx_lib.h"
#include "tx_isr.h"
#include "no_os_alloc.h"

CONSTANT UINT16 VicInfo[NUM_OF_VICS*4] = {
	/* H     V     I   Hz */
	0,    0,    0, 0,       /* 0 */
	640,  480,  0, 60,
	720,  480,  0, 60,
	720,  480,  0, 60,
	1280, 720,  0, 60,
	1920, 1080, 1, 60,      /* 5 */
	1440, 480,  1, 60,
	1440, 480,  1, 60,
	1440, 240,  0, 60,
	1440, 240,  0, 60,
	2880, 480,  1, 60,      /* 10 */
	2880, 480,  1, 60,
	2880, 240,  0, 60,
	2880, 240,  0, 60,
	1440, 480,  0, 60,
	1440, 480,  0, 60,      /* 15 */
	1920, 1080, 0, 60,
	720,  576,  0, 50,
	720,  576,  0, 50,
	1280, 720,  0, 50,
	1920, 1080, 1, 50,      /* 20 */
	1440, 576,  1, 50,
	1440, 576,  1, 50,
	1440, 288,  0, 50,
	1440, 288,  0, 50,
	2880, 576,  1, 50,      /* 25 */
	2880, 576,  1, 50,
	2880, 288,  0, 50,
	2880, 288,  0, 50,
	1440, 576,  0, 50,
	1440, 576,  0, 50,      /* 30 */
	1920, 1080, 0, 50,
	1920, 1080, 0, 24,
	1920, 1080, 0, 25,
	1920, 1080, 0, 30,
	2880, 480,  0, 60,      /* 35 */
	2880, 480,  0, 60,
	2880, 576,  0, 50,
	2880, 576,  0, 50,
	1920, 1080, 1, 50,
	1920, 1080, 1, 100,     /* 40 */
	1280, 720,  0, 100,
	720,  576,  0, 100,
	720,  576,  0, 100,
	1440, 576,  1, 100,
	1440, 576,  1, 100,     /* 45 */
	1920, 1080, 1, 120,
	1280, 720,  0, 120,
	720,  480,  0, 120,
	720,  480,  0, 120,
	1440, 480,  1, 120,     /* 50 */
	1440, 480,  1, 120,
	720,  576,  0, 200,
	720,  576,  0, 200,
	1440, 576,  1, 200,
	1440, 576,  1, 200,     /* 55 */
	720,  480,  0, 200,
	720,  480,  0, 200,
	1440, 480,  1, 200,
	1440, 480,  1, 200,
	1280, 720,  0, 24,      /* 60 */
	1280, 720,  0, 25,
	1280, 720,  0, 30,
	1920, 1080, 0, 120,
	1920, 1080, 0, 100
	/* 65 */
};

#if TX_USER_INIT
CONSTANT UCHAR UserTxRegInitTable[] = {0};
CONSTANT UCHAR UserTxFieldInitTable[] = {0};
#endif

struct no_os_i2c_desc *i2c_handler;
volatile uint32_t timer_counter_intr = 0;
STATIC UINT32 MsCnt = 0;
STATIC UINT32 Last250usCount = 0;

/*=========================================
 * Local variables
 *========================================*/
STATIC uint8_t CecTxState;
STATIC uint8_t CecCurrOper;                       /* Currently executing op   */
STATIC uint8_t CecRxMessage[CEC_MAX_MSG_SIZE+4];
STATIC uint8_t CecRetryCount;                     /* For arb lost errors      */
STATIC uint8_t OperBuf[32];
STATIC uint8_t BufIdx;
STATIC uint8_t SrcLogAddr;
STATIC struct cec_status cec_status;

extern UINT16 Cec_Notification (UCHAR Event, UINT16 Len, void *Buff);

//void DBG_Printf(const char *data, ...)
//{
//}
//
//void adi_memcpy(void *dst,void* src, UINT32 count)
//{
//	return;
//}
//
//void adi_memset(void *dst,UINT8 data, UINT32 count)
//{
//	return;
//}
/*===========================================================================
 * Get the elapsed time in milliseconds from a defined starting point
 *
 * Entry:   StartCount  = Time in milliseconds at the starting point
 *          CurrMsCount = Pointer to receive the current time in milliseconds
 *                        Can be set to NULL if not needed
 *
 * Return:  Time (in milliseconds) that elapsed since StartCount.
 *          This function can not return elapsed time over 65 seconds
 *
 *==========================================================================*/
UINT32 ATV_GetElapsedMs (UINT32 StartCount, UINT32 *CurrMsCount)
{
	UINT32 Elapsed;
	UINT32 CurrCount;

	CurrCount = HAL_GetCurrentMsCount();
	if (CurrCount >= StartCount)
		Elapsed = CurrCount - StartCount;
	else
		Elapsed = 0xFFFFFFFFL - (StartCount - CurrCount);

	if (CurrMsCount)
		*CurrMsCount = CurrCount;

	return Elapsed;
}

/*===========================================================================
 * Return the current time in milliseconds.
 * If the current time is 0, return 0xFFFFFFFF
 *
 *
 *==========================================================================*/
UINT32 ATV_GetMsCountNZ (void)
{
	UINT32 i;
	i = HAL_GetCurrentMsCount();
	if (i == 0)
		i = 0xFFFFFFFFL;

	return i;
}

/*============================================================================
 * Read up to 8-bit field from a single 8-bit register
 *              ________
 * Example     |___***__|  Mask = 0x1C     BitPos = 2
 *
 *
 * Entry:   DevAddr = Device Address
 *          RegAddr = 8-bit register address
 *          Mask    = Field mask
 *          BitPos  = Field LSBit position in the register (0-7)
 *
 * Return:  Field value in the LSBits of the return value
 *
 *===========================================================================*/
UCHAR ATV_I2CReadField8 (UCHAR DevAddr, UCHAR RegAddr, UCHAR Mask,
			 UCHAR BitPos)
{
	uint8_t read_data;
	int32_t ret;

	ret = HAL_I2CReadBlock(DevAddr, RegAddr, &read_data, 1);
	if(ret != 0)
		return ret;

	return ((read_data & Mask) >> BitPos);
}

/*============================================================================
 * Write up to 8-bit field to a single 8-bit register
 *              ________
 * Example     |___****_|  Mask = 0x1E     BitPos = 1
 *
 * Entry:   DevAddr = Device Address
 *          RegAddr = 8-bit register address
 *          Mask    = Field mask
 *          BitPos  = Field LSBit position in the register (0-7)
 *                    Set to 0 if FieldVal is in correct position of the reg
 *          FieldVal= Value (in the LSBits) of the field to be written
 *                    If FieldVal is already in the correct position (i.e.,
 *                    does not need to be shifted,) set BitPos to 0
 *
 * Return:  None
 *
 *===========================================================================*/
void ATV_I2CWriteField8 (UCHAR DevAddr, UCHAR RegAddr, UCHAR Mask,
			 UCHAR BitPos, UCHAR FieldVal)
{
	int32_t ret;
	uint8_t write_data_byte;

	ret = HAL_I2CReadByte(DevAddr, RegAddr, &write_data_byte);
	if(ret != 0)
		return;

	write_data_byte &= ~Mask;
	write_data_byte |= (FieldVal << BitPos);

	HAL_I2CWriteByte(DevAddr, RegAddr, write_data_byte);
}

/*============================================================================
 * Read up to 32-bit field from two or more consecutive 8-bit registers in
 * Big Endian format
 *
 *                   ________
 * Start Reg Addr-> |     ***|  MSbits          MsbMask = 0x07
 *                  |********|  Middle bits
 *                  |********|  Middle bits
 *                  |********|  Middle bits
 * End Reg Addr---> |**______|  LSbits          LsbMask = 0xC0, LsbPos = 6
 *                                              FldSpan = 5
 *
 * Entry:   DevAddr = Device Address
 *          RegAddr = Starting 8-bit register address
 *          MsbMask = Mask for MSbits
 *          LsbMask = Mask for LSbits
 *          LsbPos  = LSbit position (0-7)
 *          FldSpan = Number of consecutive registers containing field bits
 *                    (Maximum 5 registers)
 *
 * Return:  Field value in the LSBits of the return value
 *
 *===========================================================================*/
UINT32 ATV_I2CReadField32 (UCHAR DevAddr, UCHAR RegAddr, UCHAR MsbMask,
			   UCHAR LsbMask, UCHAR LsbPos, UCHAR FldSpan)
{
	int32_t ret;
	uint8_t *read_data, j = 1;
	uint32_t value = 0;
	int8_t i;

	read_data = no_os_calloc(FldSpan, sizeof *read_data);
	if(!read_data)
		return 0xDEADDEAD;

	ret = HAL_I2CReadBlock(DevAddr, RegAddr, read_data, FldSpan);
	if(ret != 0)
		goto error;

	value |= read_data[(FldSpan - 1)] >> LsbPos;

	for(i = (FldSpan - 2); i >= 0; i--) {
		if(i == 0)
			read_data[i] &= MsbMask;
		value |= (read_data[i] << (j++ * 8 - LsbPos));
	}

	no_os_free(read_data);

	return value;
error:
	no_os_free(read_data);

	return 0xDEADDEAD;
}

/*============================================================================
 * Read up to 32-bit field from two or more consecutive 8-bit registers in
 * Little Endian format
 *
 *                   ________
 * Start Reg Addr-> |***     |  LSbits          LsbMask = 0xE0, LsbPos = 5
 *                  |********|  Middle bits
 *                  |********|  Middle bits
 *                  |********|  Middle bits
 * End Reg Addr---> |______**|  MSbits          MsbMask = 0x03
 *                                              FldSpan = 5
 *
 * Entry:   DevAddr = Device Address
 *          RegAddr = Starting 8-bit register address
 *          MsbMask = Mask for MSbits
 *          LsbMask = Mask for LSbits
 *          LsbPos  = LSbit position (0-7)
 *          FldSpan = Number of consecutive registers containing field bits
 *                    (Maximum 5 registers)
 *
 * Return:  Field value in the LSBits of the return value
 *
 *===========================================================================*/
UINT32 ATV_I2CReadField32LE (UCHAR DevAddr, UCHAR RegAddr, UCHAR MsbMask,
			     UCHAR LsbMask, UCHAR LsbPos, UCHAR FldSpan)
{
	int32_t ret;
	uint8_t *read_data, i;
	uint32_t value = 0;

	read_data = no_os_calloc(FldSpan, sizeof *read_data);
	if(!read_data)
		return 0xDEADDEAD;

	ret = HAL_I2CReadBlock(DevAddr, RegAddr, read_data, FldSpan);
	if(ret != 0)
		goto error;

	value |= (read_data[0] >> LsbPos);

	for(i = 1; i < FldSpan; i++) {
		if(i == (FldSpan - 1))
			read_data[i] &= MsbMask;
		value |= read_data[i] << (i * 8 - LsbPos);
	}

	no_os_free(read_data);

	return value;
error:
	no_os_free(read_data);

	return 0xDEADDEAD;
}

/*============================================================================
 * Write up to 32-bit field to two or more consecutive 8-bit registers in
 * Big Endian format
 *                   ________
 * Start Reg Addr-> |   *****|  MSbits          MsbMask = 0x1F
 *                  |********|  Middle bits
 * End Reg Addr---> |******__|  LSbits          LsbMask = 0xFC, LsbPos = 2
 *                                              FldSpan = 3
 * Entry:   DevAddr = Device Address
 *          RegAddr = Starting 8-bit register address
 *          MsbMask = Mask for MSbits
 *          LsbMask = Mask for LSbits
 *          LsbPos  = LSbit position (0-7)
 *          FldSpan = Number of consecutive registers containing field bits
 *          Val     = Field value (in the LSBits) to be written
 *
 * Return:  None
 *
 *
 *===========================================================================*/
void ATV_I2CWriteField32 (UCHAR DevAddr, UCHAR RegAddr, UCHAR MsbMask,
			  UCHAR LsbMask, UCHAR LsbPos, UCHAR FldSpan, UINT32 Val)
{
	int32_t ret;
	uint8_t *read_data, j = 1;
	int8_t i;

	read_data = no_os_calloc(FldSpan, sizeof *read_data);
	if(!read_data)
		return;

	ret = HAL_I2CReadBlock(DevAddr, RegAddr, read_data, FldSpan);
	if(ret != 0)
		goto error;

	read_data[(FldSpan)] &= ~LsbMask;
	read_data[(FldSpan)] |= ((Val << LsbPos) & LsbMask);

	for(i = (FldSpan - 2); i >= 0; i--) {
		if(i == 0) {
			read_data[i] &= ~MsbMask;
			read_data[i] |= ((Val >> ((j * 8) - LsbPos)) & MsbMask);
		} else {
			read_data[i] &= 0;
			read_data[i] |= ((Val >> ((j++ * 8) - LsbPos)) & 0xFF);
		}
	}

	HAL_I2CWriteBlock(DevAddr, RegAddr, read_data, FldSpan);
error:
	no_os_free(read_data);
}

/*===========================================================================
 * Modify multiple registers fields from a user-supllied table
 *
 * Entry:   Table : A pointer to the table of writes that must be performed
 *                  Table[] = {
 *                              DevAddr0, RegOffset0, Mask0, Value0,
 *                              DevAddr1, RegOffset1, Mask1, Value1,
 *                              DevAddr2, RegOffset2, Mask2, Value2,
 *                              ...
 *                              DevAddrN, RegOffsetN, MaskN, ValueN,
 *                              EndVal
 *                             }
 *          EndVal : The last value in the table
 *
 * Return:  None
 *
 *==========================================================================*/
void ATV_I2CWriteFields (UCHAR *Table, UCHAR EndVal)
{
	int32_t ret;
	uint32_t i = 0;
	uint8_t write_word;

	while(1) {
		if(Table[i] == EndVal)
			break;

		ret = HAL_I2CReadByte(Table[i], Table[(i + 1)], &write_word);
		if(ret != 0)
			return;

		write_word &= ~Table[(i + 2)];
		write_word |= (Table[(i + 3)] & Table[(i + 2)]);

		ret = HAL_I2CWriteByte(Table[i], Table[(i + 1)], write_word);
		if(ret != 0)
			return;

		i += 4;
	}
}

/*============================================================================
 * Write up to 32-bit field to two or more consecutive 8-bit registers in
 * Little Endian format
 *                   ________
 * Start Reg Addr-> |*       |  LSbits          LsbMask = 0x80, LsbPos = 7
 *                  |********|  Middle bits
 *                  |********|  Middle bits
 * End Reg Addr---> |********|  MSbits          MsbMask = 0xFF
 *                                              FldSpan = 4
 * Entry:   DevAddr = Device Address
 *          RegAddr = Starting 8-bit register address
 *          MsbMask = Mask for MSbits
 *          LsbMask = Mask for LSbits
 *          LsbPos  = LSbit position (0-7)
 *          FldSpan = Number of consecutive registers containing field bits
 *          Val     = Field value (in the LSBits) to be written
 *
 * Return:  None
 *
 *
 *===========================================================================*/
void ATV_I2CWriteField32LE (UCHAR DevAddr, UCHAR RegAddr, UCHAR MsbMask,
			    UCHAR LsbMask, UCHAR LsbPos, UCHAR FldSpan, UINT32 Val)
{
	int32_t ret;
	uint8_t *read_data, j = 0;
	int8_t i;

	read_data = no_os_calloc(FldSpan, sizeof *read_data);
	if(!read_data)
		return;

	ret = HAL_I2CReadBlock(DevAddr, RegAddr, read_data, FldSpan);
	if(ret != 0)
		goto error;

	read_data[0] &= ~LsbMask;
	read_data[0] |= ((Val << LsbPos) & LsbMask);

	for(i = 1; i < FldSpan; i++) {
		if(i == (FldSpan - 1)) {
			read_data[i] &= MsbMask;
			read_data[i] |= ((Val >> (j * 8 + (8 - LsbPos))) & MsbMask);
		} else {
			read_data[i] &= 0xff;
			read_data[i] |= ((Val >> (j++ * 8 + (8 - LsbPos))) & 0xff);
		}
	}

	HAL_I2CWriteBlock(DevAddr, RegAddr, read_data, FldSpan);
error:
	no_os_free(read_data);
}

/*===========================================================================
 * Perform multiple I2C register writes from a user-supplied Table
 *
 * Entry:   Table : A pointer to the table of writes that must be performed
 *                  Table[] = {
 *                              DevAddr0, RegOffset0, Value0,
 *                              DevAddr1, RegOffset1, Value1,
 *                              DevAddr2, RegOffset2, Value2,
 *                              ...
 *                              DevAddrN, RegOffsetN, ValueN,
 *                              EndVal
 *                             }
 *          EndVal : The last value in the table
 *
 * Return:  None
 *
 *==========================================================================*/
void ATV_I2CWriteTable (UCHAR *Table, UCHAR EndVal)
{
	uint32_t i = 0;
	int32_t ret;

	while(1) {
		if(Table[i] == EndVal)
			break;

		ret = HAL_I2CWriteByte(Table[i], Table[(i + 1)], Table[(i + 2)]);
		if(ret != 0)
			return;
	}
}

/*===========================================================================
 * Look for UCHAR Value in Table with Step increments
 *
 * Return:  Offset of matching value or end value
 *
 *==========================================================================*/
UINT16 ATV_LookupValue8 (UCHAR *Table, UCHAR Value, UCHAR EndVal, UINT16 Step)
{
	UINT16 i=0, found = 0;

	while(1) {
		if(Table[i] == EndVal)
			break;
		if(Table[i] == Value) {
			found = 1;
			break;
		}
		i += Step;
	}

	if(found)
		return i;
	else
		return EndVal;
}

/*===========================================================================
 * Print current system time
 *
 * Entry:   Gran    = 0  for ms
 *                    1  for sec:ms
 *                    2  for min:sec:ms
 *          PostFix = Postfix string
 *
 *==========================================================================*/
void ATV_PrintTime (char *Prefix, UCHAR Gran, char *Postfix)
{
	uint32_t ms_no, s_no, m_no;

	DBG_MSG(Prefix);

	ms_no = ATV_GetMsCountNZ();
	switch(Gran) {
	case 0:
		DBG_MSG("%d", (int)ms_no);
		break;
	case 1:
		s_no = ms_no / 100;
		ms_no %= 100;
		DBG_MSG("%d:%d", (int)s_no, (int)ms_no);
		break;
	case 2:
		s_no = ms_no / 100;
		m_no = s_no / 60;
		s_no %= 60;
		ms_no %= 100;
		DBG_MSG("%d:%d:%d", (int)m_no, (int)s_no, (int)ms_no);
		break;
	default:
		return;
	}

	DBG_MSG(Postfix);
}

/*===========================================================================
 *
 *==========================================================================*/
void HAL_DelayMs (UINT16 Counter)
{
	WaitMilliSec(Counter);
}

/*===========================================================================
 *
 *==========================================================================*/
UINT16 HAL_I2CReadBlock (UCHAR Dev, UCHAR Reg, UCHAR *Data, UINT16 NofBytes)
{
	int32_t ret;
	uint8_t i2c_addr = Dev >> 1; /* I2C non-formatted address */
	uint8_t reg_addr;
	uint16_t i = 0;

	i2c_handler->slave_address = i2c_addr;

	do {
		reg_addr = Reg + i;
		ret = no_os_i2c_write(i2c_handler, &reg_addr, 1, 0);
		if(ret != 0)
			return 0xDEAD;

		ret = no_os_i2c_read(i2c_handler, Data + i, 1, 1);
		if(ret != 0)
			return 0xDEAD;
		i++;
	} while(i != NofBytes);

	return 0;
}

/*===========================================================================
 *
 *==========================================================================*/
UINT16  HAL_I2CWriteBlock (UCHAR Dev, UCHAR Reg, UCHAR *Data,
			   UINT16 NumberBytes)
{
	int32_t ret;
	uint8_t *data_write;
	uint16_t i;
	uint8_t i2c_addr = Dev >> 1; /* I2C non-formatted address */

	i2c_handler->slave_address = i2c_addr;

	data_write = no_os_calloc((NumberBytes + 1), sizeof *data_write);
	if(!data_write)
		return 0xDEAD;

	data_write[0] = Reg;
	for(i = 1; i <= NumberBytes; i++)
		data_write[i] = Data[(i - 1)];

	ret = no_os_i2c_write(i2c_handler, data_write, (NumberBytes + 1), 1);
	if(ret != 0)
		goto error;

	no_os_free(data_write);

	return 0;
error:
	no_os_free(data_write);

	return 0xDEAD;
}

/*===========================================================================
 *
 *==========================================================================*/
UCHAR HAL_I2CReadByte (UCHAR Dev, UCHAR Reg, UCHAR *Data)
{
	int32_t ret;
	uint8_t i2c_addr = Dev >> 1; /* I2C non-formatted address */

	i2c_handler->slave_address = i2c_addr;

	ret = no_os_i2c_write(i2c_handler, &Reg, 1, 0);
	if(ret != 0)
		return 0xDD;

	ret = no_os_i2c_read(i2c_handler, Data, 1, 1);
	if(ret != 0)
		return 0xDD;

	return 0;
}

/*===========================================================================
 *
 *==========================================================================*/
UCHAR HAL_I2CWriteByte (UCHAR Dev, UCHAR Reg, UCHAR Data)
{
	int32_t ret;
	uint8_t data_write[2];
	uint8_t i2c_addr = Dev >> 1; /* I2C non-formatted address */

	i2c_handler->slave_address = i2c_addr;

	data_write[0] = Reg;
	data_write[1] = Data;

	ret = no_os_i2c_write(i2c_handler, data_write, 2, 1);
	if(ret != 0)
		return 0xDD;

	return 0;
}

UCHAR HAL_SetRxChipSelect(UCHAR DevIdx)
{
	return 1;
}

/**
 * Wait for passed number of milli-seconds
 */
void WaitMilliSec(unsigned int msec)
{
	timer_counter_intr = 0;
	while(timer_counter_intr != msec);
}

static inline void cec_reg_soft_reset_set(uint8_t reg_value)
{
	if((reg_value == 0) || (reg_value == 1))
		ATV_I2CWriteField8(ADV7511_CEC_I2C_ADDR, ADV7511_CEC_SOFT_RESET,
				   ADV7511_CEC_SOFT_RESET_MASK, ADV7511_CEC_SOFT_RESET_SHIFT,
				   reg_value);
}

static inline void cec_reg_tx_retry_count_set(uint8_t reg_value)
{
	if((reg_value >= 1) && (reg_value <= 7))
		ATV_I2CWriteField8(ADV7511_CEC_I2C_ADDR, ADV7511_CEC_TX_RETRY_COUNT,
				   ADV7511_CEC_TX_RETRY_COUNT_MASK,
				   ADV7511_CEC_TX_RETRY_COUNT_SHIFT, reg_value);
}

/*============================================================================
 * Reset CEC controller
 *
 * Entry:   None
 *
 * Return:  ATVERR_OK
 *
 *===========================================================================*/
ATV_ERR CEC_Reset (void)
{
	cec_reg_soft_reset_set(1);
	HAL_DelayMs(10);
	cec_reg_soft_reset_set(0);
	CecTxState = CEC_TX_STATE_DONE;
	CecCurrOper = CEC_OP_NONE;
	CecRetryCount = 0;
	cec_reg_tx_retry_count_set(CEC_RETRY_COUNT);
	cec_status.tx_header = 0;
	cec_status.tx_tail = 0;
	cec_status.tx_cnt = 0;

	return ATVERR_OK;
}

static inline void cec_reg_power_mode_set(int8_t reg_value)
{
	if((reg_value >= 0) && (reg_value <= 3))
		ATV_I2CWriteField8(ADV7511_CEC_I2C_ADDR, ADV7511_CEC_POWER_MODE,
				   ADV7511_CEC_POWER_MODE_MASK, ADV7511_CEC_POWER_MODE_SHIFT,
				   reg_value);
}

static inline void cec_reg_rx_enable_set(uint8_t reg_value)
{
	if((reg_value >= 0) && (reg_value <= 1))
		ATV_I2CWriteField8(ADV7511_CEC_I2C_ADDR, ADV7511_CEC_RX_ENABLE,
				   ADV7511_CEC_RX_ENABLE_MASK, ADV7511_CEC_RX_ENABLE_SHIFT,
				   reg_value);
}

static inline void cec_reg_rx_buff_no_set(uint8_t reg_value)
{
	if((reg_value >= 0) && (reg_value <= 1))
		ATV_I2CWriteField8(ADV7511_CEC_I2C_ADDR, ADV7511_CEC_RX_BUFFER_NUMBER,
				   ADV7511_CEC_RX_BUFFER_NUMBER_MASK,
				   ADV7511_CEC_RX_BUFFER_NUMBER_SHIFT, reg_value);
}

static inline void cec_reg_tx_enable_set(uint8_t reg_value)
{
	if((reg_value >= 0) && (reg_value <= 1))
		ATV_I2CWriteField8(ADV7511_CEC_I2C_ADDR, ADV7511_CEC_TX_TRANS_ENABLE,
				   ADV7511_CEC_TX_TRANS_ENABLE_MASK,
				   ADV7511_CEC_TX_TRANS_ENABLE_SHIFT, reg_value);
}

/*============================================================================
 * Enable/Disable CEC controller
 *
 * Entry:   Enable = TRUE to enable CEC controller
 *                   FALSE to disable
 *
 * Return:  ATVERR_OK
 *
 *===========================================================================*/
ATV_ERR CEC_Enable (BOOL Enable)
{
	cec_reg_power_mode_set(Enable ? 0x01 : 0x00);
	CEC_Reset();
	cec_reg_rx_enable_set(1);
//	cec_reg_rx_buff_no_set(1);

#if CEC_CFG01_ENABLE_TWICE              /* Must be defined for 7840/7604 */
	cec_reg_rx_enable_set(1);
	cec_reg_tx_enable_set(1);           /* 2nd EN is done in CEC Send msg */
#endif

#if ( ((TX_INCLUDE_CEC) && ((TX_DEVICE==8002)||(TX_DEVICE==7511))) || \
		((RX_INCLUDE_CEC) && (RX_DEVICE==7844)))
	cec_reg_rx_buff_no_set(1);
#endif

#if CEC_TX_2G
#if ( ((TX_INCLUDE_CEC) && ((TX_DEVICE==8002)||(TX_DEVICE==7511))) || \
		((RX_INCLUDE_CEC) && (RX_DEVICE==7844)))
	cec_reg_rx_buff_no_set(1);
#else
	cec_reg_rx_buff_no_set(0);
#endif
#endif

	return ATVERR_OK;
}

static inline void cec_reg_logaddr0_set(int8_t reg_value)
{
	if((reg_value >= 0) && (reg_value <= 15))
		ATV_I2CWriteField8(ADV7511_CEC_I2C_ADDR, ADV7511_CEC_LOGICADDR0,
				   ADV7511_CEC_LOGICADDR0_MASK, ADV7511_CEC_LOGICADDR0_SHIFT,
				   reg_value);
}

static inline void cec_reg_logaddr1_set(int8_t reg_value)
{
	if((reg_value >= 0) && (reg_value <= 15))
		ATV_I2CWriteField8(ADV7511_CEC_I2C_ADDR, ADV7511_CEC_LOGICADDR1,
				   ADV7511_CEC_LOGICADDR1_MASK, ADV7511_CEC_LOGICADDR1_SHIFT,
				   reg_value);
}

static inline void cec_reg_logaddr2_set(int8_t reg_value)
{
	if((reg_value >= 0) && (reg_value <= 15))
		ATV_I2CWriteField8(ADV7511_CEC_I2C_ADDR, ADV7511_CEC_LOGICADDR2,
				   ADV7511_CEC_LOGICADDR2_MASK, ADV7511_CEC_LOGICADDR2_SHIFT,
				   reg_value);
}

static inline uint8_t cdc_reg_logaddr_mask_get()
{
	return ATV_I2CReadField8(ADV7511_CEC_I2C_ADDR, ADV7511_CEC_LOGICADDR_MASK,
				 ADV7511_CEC_LOGICADDR_MASK_MASK, ADV7511_CEC_LOGICADDR_MASK_SHIFT);
}

static inline void cec_reg_logaddr_mask_set(int8_t reg_value)
{
	if((reg_value >= 0) && (reg_value <= 7))
		ATV_I2CWriteField8(ADV7511_CEC_I2C_ADDR, ADV7511_CEC_LOGICADDR_MASK,
				   ADV7511_CEC_LOGICADDR_MASK_MASK,
				   ADV7511_CEC_LOGICADDR_MASK_SHIFT, reg_value);
}

/*============================================================================
 * Set the logical address for one of 3 logical devices
 *
 * Entry:   LogAddr = Logical address for the device
 *          DevId   = The device to set the logical address to. (0, 1 or 2)
 *          Enable  = TRUE to enable the logical device
 *                    FALSE to disable the logical device
 *
 * Return:  ATVERR_OK
 *          ATVERR_INV_PARM if DevId is larger than 2
 *
 *===========================================================================*/
ATV_ERR CEC_SetLogicalAddr (UCHAR LogAddr, UCHAR DevId, BOOL Enable)
{
	uint8_t reg_mask, dev_bit;

	LogAddr &= 0x0f;
	switch(DevId) {
	case 0:
		cec_reg_logaddr0_set(LogAddr);
		dev_bit = ADV7511_CEC_LOGICADDR_MASK_ADDR0;
		break;
	case 1:
		cec_reg_logaddr1_set(LogAddr);
		dev_bit = ADV7511_CEC_LOGICADDR_MASK_ADDR1;
		break;
	case 2:
		cec_reg_logaddr2_set(LogAddr);
		dev_bit = ADV7511_CEC_LOGICADDR_MASK_ADDR2;
		break;
	default:
		return ATVERR_INV_PARM;
	}

	reg_mask = cdc_reg_logaddr_mask_get();
	reg_mask &= (~dev_bit);
	if(Enable)
		reg_mask |= dev_bit;
	cec_reg_logaddr_mask_set(reg_mask);

	return ATVERR_OK;
}

static inline void cec_reg_tx_framelen_set(int8_t reg_value)
{
	if((reg_value >= 0) && (reg_value <= 31))
		ATV_I2CWriteField8(ADV7511_CEC_I2C_ADDR, ADV7511_CEC_TX_FRAME_LENGTH,
				   ADV7511_CEC_TX_FRAME_LENGTH_MASK,
				   ADV7511_CEC_TX_FRAME_LENGTH_SHIFT, reg_value);
}

/*============================================================================
 * Send message to CEC
 *
 * Entry:   MsgPtr = Pointer to the message to be sent
 *          MsgLen
 *
 * Return:  ATVERR_OK
 *          ATVERR_FAILED if CEC controller is busy
 *          ATVERR_INV_PARM if MsgLen is larger than max message size
 *
 *===========================================================================*/
ATV_ERR CEC_SendMessage (UCHAR *MsgPtr, UCHAR MsgLen)
{
	uint8_t header = 0, tail = 0;
	uint8_t i, j;

	if(MsgLen > CEC_MAX_MSG_SIZE)
		return ATVERR_INV_PARM;

	if(CecTxState == CEC_TX_STATE_DONE) {
		CecTxState = CEC_TX_STATE_BUSY;
		HAL_I2CWriteBlock(ADV7511_CEC_I2C_ADDR, ADV7511_CEC_TX_FRAME_HEADER,
				  MsgPtr, MsgLen);
		cec_reg_tx_framelen_set(MsgLen);
		cec_reg_tx_enable_set(1);
		return ATVERR_OK;
	}

	/*===================================================
	 * when CEC TX is BUSY, save message to buffer
	 * 1) to save message length to the first byte
	 * 2) to find space for message
	 * 3) to save CEC message and update TxTail & TxCnt
	 *===================================================*/
	header = cec_status.tx_header;
	tail = cec_status.tx_tail;
	cec_status.tx_buf[tail] = MsgLen; /* 1. to save message length */
	j = MsgLen;
	if(tail == header) {
		if (cec_status.tx_cnt != 0)    /* not empty buffer */
			return ATVERR_FAILED;
		else
			tail++;
	} else {
		tail++;
		if (tail >= CEC_TX_BUF_LEN)    /* tail at the last place */
			tail = 0;
		if (tail == header)    /* tail reaches header */
			return ATVERR_FAILED;
	}

	if (tail >= header) {
		if((MsgLen + tail) >= CEC_TX_BUF_LEN) {
			j = CEC_TX_BUF_LEN - tail;
			if ((j+header) < MsgLen)
				return ATVERR_FAILED;
		}
	} else {
		if ((header - tail) < MsgLen)
			return ATVERR_FAILED;
	}

	for (i = 0; i < j; i++)
		cec_status.tx_buf[tail++] = *(MsgPtr+i);
	if (j != MsgLen) {
		tail = 0;
		for (i=j; i<MsgLen; i++)
			cec_status.tx_buf[tail++] = *(MsgPtr+i);
	}
	if (tail >= CEC_TX_BUF_LEN)
		tail = 0;
	cec_status.tx_tail = tail;
	cec_status.tx_cnt++;

	return ATVERR_OK;
}

/*============================================================================
 * Send out CEC message in buffer
 *
 * Entry:
 *
 * Return:  ATVERR_OK
 *          ATVERR_FAILED if CEC controller is busy
 *
 * Note:    HDMI spec 1.4
 *          Section CEC 9.2    Message Time Constraints
 *          There are certain time constraints for messages that should be
 *          obeyed at application level. These are a desired maximum response
 *          time of 200ms and a required maximum response time of 1 second.
 *===========================================================================*/
ATV_ERR CEC_SendMessageOut (void)
{
	UCHAR msg_ptr[CEC_TX_BUF_LEN];
	UCHAR msg_len = 0, i = 0;
	UCHAR header;

	header = cec_status.tx_header;
	if (cec_status.tx_cnt != 0) {
		if (CecTxState == CEC_TX_STATE_DONE) {
			/*===================================================
			 * when CEC TX is READY
			 * to dump one CEC message to msg_ptr[]
			 * to update TxHeader & TxCnt for next message
			 *===================================================*/
			msg_len = cec_status.tx_buf[header];
			for (i = 0; i < msg_len; i++) {
				header ++;
				if (header >= CEC_TX_BUF_LEN)
					header = 0;
				msg_ptr[i] = cec_status.tx_buf[header];
			}
			header++;
			if (header >= CEC_TX_BUF_LEN)
				header = 0;
			cec_status.tx_header = header;
			cec_status.tx_cnt--;

			CecTxState = CEC_TX_STATE_BUSY;
			HAL_I2CWriteBlock(ADV7511_CEC_I2C_ADDR, ADV7511_CEC_TX_FRAME_HEADER,
					  msg_ptr, msg_len);
			cec_reg_tx_framelen_set(msg_len);
			cec_reg_tx_enable_set(1);
		} else {
			return ATVERR_FAILED;
		}
	}

	return ATVERR_OK;
}

/*============================================================================
 * Resend last sent message to CEC
 *
 * Entry:   None
 *
 * Return:  ATVERR_OK
 *          ATVERR_FAILED if CEC controller is busy
 *
 *===========================================================================*/
ATV_ERR CEC_ResendLastMessage (void)
{
	if (CecTxState == CEC_TX_STATE_DONE) {
		CecTxState = CEC_TX_STATE_BUSY;
		cec_reg_tx_enable_set(1);
	} else {
		return ATVERR_FAILED;
	}
	return ATVERR_OK;
}

static inline void cec_reg_rx_buf1_rdy_set(int8_t reg_value)
{
	if((reg_value >= 0) && (reg_value <= 1))
		ATV_I2CWriteField8(ADV7511_CEC_I2C_ADDR, ADV7511_CEC_RX_BUFFER1_RDY,
				   ADV7511_CEC_RX_BUFFER1_RDY_MASK,
				   ADV7511_CEC_RX_BUFFER1_RDY_SHIFT, reg_value);
}

static inline void cec_reg_rx_buf2_rdy_set(int8_t reg_value)
{
	if((reg_value >= 0) && (reg_value <= 1))
		ATV_I2CWriteField8(ADV7511_CEC_I2C_ADDR, ADV7511_CEC_RX_BUFFER2_RDY,
				   ADV7511_CEC_RX_BUFFER2_RDY_MASK,
				   ADV7511_CEC_RX_BUFFER2_RDY_SHIFT, reg_value);
}

static inline void cec_reg_rx_buf3_rdy_set(int8_t reg_value)
{
	if((reg_value >= 0) && (reg_value <= 1))
		ATV_I2CWriteField8(ADV7511_CEC_I2C_ADDR, ADV7511_CEC_RX_BUFFER3_RDY,
				   ADV7511_CEC_RX_BUFFER3_RDY_MASK,
				   ADV7511_CEC_RX_BUFFER3_RDY_SHIFT, reg_value);
}

static inline uint8_t cec_reg_tx_nack_cnt_get()
{
	return ATV_I2CReadField8(ADV7511_CEC_I2C_ADDR, ADV7511_CEC_TX_NACK_CNT,
				 ADV7511_CEC_TX_NACK_CNT_MASK, ADV7511_CEC_TX_NACK_CNT_SHIFT);
}

/*============================================================================
 *
 * Entry:
 *
 * Return:  None
 *
 *
 *===========================================================================*/
static void handle_log_addr_alloc_ints(CEC_INTERRUPTS *CecInts)
{
	uint16_t nack_count;
	uint8_t err_code = 0;
	uint8_t msg;

	/*======================================
	 * Acknowledged, Log address is taken
	 *=====================================*/
	if (CecInts->TxReady) {
		BufIdx++;
		/*==================================
		 * No more logical addresses, exit
		 *=================================*/
		if (OperBuf[BufIdx] == 0xff) {
			CecCurrOper = CEC_OP_NONE;
			Cec_Notification(CEC_EVENT_LOG_ADDR_ALLOC, 0, OperBuf+BufIdx);
		} else {
			/*==================================
			 * Try next logical addresses
			 *=================================*/
			msg = (OperBuf[BufIdx] & 0xf) | ((OperBuf[BufIdx] & 0xf) << 4);
			CEC_SendMessage(&msg, 1);
		}
	} else if (CecInts->Timeout) {
		nack_count = cec_reg_tx_nack_cnt_get();
		/*=============================================
		 * No response, logical addr available, exit
		 *============================================*/
		if (nack_count == (CEC_RETRY_COUNT + 1)) {
			CecCurrOper = CEC_OP_NONE;
			Cec_Notification(CEC_EVENT_LOG_ADDR_ALLOC, 0, OperBuf+BufIdx);
		} else {
			err_code = 0x10;                 /* Try same logical addr again */
		}
	} else if (CecInts->ArbLost) {
		err_code = 0x11;                     /* Try same logical addr again */
	}

	/*====================================
	 * If need to resend
	 *===================================*/
	if (err_code) {
		CecRetryCount++;
		if (CecRetryCount > CEC_RETRY_COUNT) {
			CecRetryCount = 0;
			CecCurrOper = CEC_OP_NONE;
			Cec_Notification(CEC_EVENT_LOG_ADDR_ALLOC, 1, &err_code);
		} else {
			cec_reg_tx_enable_set(1);
		}
	} else {
		CecRetryCount = 0;
	}
}

/*============================================================================
 *
 * Entry:
 *
 * Return:  None
 *
 *
 *===========================================================================*/
static void handle_log_addr_polling_ints(CEC_INTERRUPTS *CecInts)
{
	uint16_t nack_count;
	uint8_t err_code = 0;
	uint8_t msg;

	/*======================================
	 * Acknowledged, Log address is taken
	 *=====================================*/
	if (CecInts->TxReady) {
		OperBuf[BufIdx] = TRUE;
	} else if (CecInts->Timeout) {
		nack_count = cec_reg_tx_nack_cnt_get();
		/*=============================================
		 * No response, logical addr available
		 *============================================*/
		if (nack_count == (CEC_RETRY_COUNT + 1))
			OperBuf[BufIdx] = FALSE;
		else
			err_code = 0x10;             /* Try same logical addr again */
	} else if (CecInts->ArbLost) {
		err_code = 0x11;                 /* Try same logical addr again */
	}

	/*====================================
	 * If need to resend
	 *===================================*/
	if (err_code) {
		CecRetryCount++;
		if (CecRetryCount > CEC_RETRY_COUNT) {
			CecRetryCount = 0;
			CecCurrOper = CEC_OP_NONE;
			Cec_Notification(CEC_EVENT_LOG_ADDR_ALLOC, 1, &err_code);
		} else {
			cec_reg_tx_enable_set(1);
		}
	} else {
		CecRetryCount = 0;
		BufIdx++;
		if (BufIdx == 0x10) {
			CecCurrOper = CEC_OP_NONE;
			Cec_Notification(CEC_EVENT_LOG_ADDR_LIST, 0, OperBuf);
		} else {
			/*==================================
			 * Try next logical addresses
			 *=================================*/
			msg  = BufIdx;
			msg |= (SrcLogAddr==0xff)? (BufIdx << 4): (SrcLogAddr << 4);
			CEC_SendMessage(&msg, 1);
		}
	}
}

/*============================================================================
 *
 * Entry:   CecInts = CEC interrupts
 *
 * Return:  None
 *
 *
 *===========================================================================*/
void CEC_Isr (CEC_INTERRUPTS *CecInts)
{
#if (((TX_INCLUDE_CEC) && ((TX_DEVICE==8002)||(TX_DEVICE==7511))) || \
		((RX_INCLUDE_CEC) && (RX_DEVICE==7844)))
	uint8_t i, j;
#endif

	/*======================================
	 * If TX int, set TX state to done
	 *=====================================*/
	if (CecInts->TxReady || CecInts->Timeout || CecInts->ArbLost)
		CecTxState = CEC_TX_STATE_DONE;

#if (((TX_INCLUDE_CEC) && ((TX_DEVICE==8002) || \
		(TX_DEVICE==7511))) || ((RX_INCLUDE_CEC) && (RX_DEVICE==7844)))
	j = 1; /* First receiving value */
	do {
		for(i = 0; i < CEC_TRIPLE_NUMBER; i++) {
			if (CecInts->RxFrameOrder[i] == j) {
				switch(i) {
				case CEC_RX_BUFFER1:
					HAL_I2CReadBlock(ADV7511_CEC_I2C_ADDR,
							 ADV7511_CEC_RX_BUFF1_HDR, CecRxMessage,
							 (CEC_MAX_MSG_SIZE + 1));
					/*  CEC buffer1 flag need to do toggling  */
					cec_reg_rx_buf1_rdy_set(1);
					cec_reg_rx_buf1_rdy_set(0);
					break;

				case CEC_RX_BUFFER2:
					HAL_I2CReadBlock(ADV7511_CEC_I2C_ADDR,
							 ADV7511_CEC_RX_BUFF2_HDR, CecRxMessage,
							 (CEC_MAX_MSG_SIZE + 1));
					/*  CEC buffer2 flag need to do toggling  */
					cec_reg_rx_buf2_rdy_set(1);
					cec_reg_rx_buf2_rdy_set(0);
					break;

				case CEC_RX_BUFFER3:
					HAL_I2CReadBlock(ADV7511_CEC_I2C_ADDR,
							 ADV7511_CEC_RX_BUFF3_HDR, CecRxMessage,
							 (CEC_MAX_MSG_SIZE + 1));
					/*  CEC buffer3 flag need to do toggling  */
					cec_reg_rx_buf3_rdy_set(1);
					cec_reg_rx_buf3_rdy_set(0);
					break;
				default:
					break;
				}
				Cec_Notification(CEC_EVENT_RX_MSG,
						 CecRxMessage[CEC_MAX_MSG_SIZE],
						 CecRxMessage);
			}
		}
		j++;
	} while(j <= CEC_TRIPLE_NUMBER);
#else
	/*======================================
	 * RX int
	 *=====================================*/
	if (CecInts->RxReady) {
		HAL_I2CReadBlock(ADV7511_CEC_I2C_ADDR, ADV7511_CEC_RX_BUFF1_HDR,
				 CecRxMessage, (CEC_MAX_MSG_SIZE + 1));

#if CEC_TX_2G
		cec_reg_rx_buf1_rdy_set(1);
		cec_reg_rx_buf1_rdy_set(0);
#endif

		cec_reg_rx_enable_set(1);
		Cec_Notification(CEC_EVENT_RX_MSG, CecRxMessage[CEC_MAX_MSG_SIZE],
				 CecRxMessage);
	}
#endif

	/*======================================
	 * Check current operation
	 *=====================================*/
	switch(CecCurrOper) {
	case CEC_OP_LOG_ADDR_ALLOC:
		handle_log_addr_alloc_ints(CecInts);
		break;
	case CEC_OP_GET_LOG_ADDR_LIST:
		handle_log_addr_polling_ints(CecInts);
		break;
	default:
		if (CecInts->TxReady) {
			CecRetryCount = 0;
			Cec_Notification(CEC_EVENT_TX_DONE, 0, NULL);
		}
		if (CecInts->Timeout) {
			CecRetryCount = 0;
			Cec_Notification(CEC_EVENT_TX_TIMEOUT, 0, NULL);
		}
		if (CecInts->ArbLost) {
			CecRetryCount++;
			if (CecRetryCount > CEC_RETRY_COUNT) {
				CecRetryCount = 0;
				Cec_Notification(CEC_EVENT_TX_ARB_LOST, 0, NULL);
			} else {
				cec_reg_tx_enable_set(1);
			}
		}
		break;
	}
}

/*============================================================================
 * Perform logical address allocation
 *
 * Entry:   LogAddrList = Pointer to a prioritized list of logical addresses
 *                        that the device will try to obtain, terminated by
 *                        0xff.
 *
 * Return:  ATVERR_OK
 *          ATVERR_FAILED if CEC is busy
 *          ATVERR_INV_PARM if LogAddrList is too long or contains no data
 *
 * This function returns immediately. If a logical address slot is found,
 * the caller will be notified by the event ADI_EVENT_CEC_LOG_ADDR_ALLOC
 *
 *===========================================================================*/
ATV_ERR CEC_AllocateLogAddr (UCHAR *LogAddrList)
{
	UCHAR i, msg;
	ATV_ERR RetVal;

	if ((CecCurrOper != CEC_OP_NONE) || (CecTxState != CEC_TX_STATE_DONE))
		return (ATVERR_FAILED);

	/*=========================================
	 * Copy message parameters to local vars
	 *========================================*/
	BufIdx = 0;
	for (i = 0; i < (CEC_MAX_MSG_SIZE + 1); i++) {
		OperBuf[i] = LogAddrList[i];
		if (OperBuf[i] == 0xff)
			break;
	}

	/*=========================================
	 * If any error in message parameters
	 *========================================*/
	if ((i == 0) || (i >= (CEC_MAX_MSG_SIZE + 1)))
		return (ATVERR_INV_PARM);

	/*=========================================
	 * Kick-off transmission
	 *========================================*/
	msg = (*LogAddrList & 0xf) | ((*LogAddrList & 0xf) << 4);
	RetVal = CEC_SendMessage(&msg, 1);
	if (RetVal == ATVERR_OK)
		CecCurrOper = CEC_OP_LOG_ADDR_ALLOC;

	return RetVal;
}

/**
 * Return switch state.
 *
 * @param void
 *
 * @return bool value corresponding to the switch state.
 */
BOOL HAL_GetMBSwitchState()
{
	BOOL ret = TRUE;
#if (ATV_XCVR_EVAL  || ATV_761X_EVAL || ATV_785X_EVAL)
	ret = (!GPIO_get_switch_status());
#endif
	return ret;
}

/**
 * Get the current microseconds count from the timer variable.
 *
 * @param void
 *
 * @return number of microseconds
 */
uint32_t HAL_GetCurrentMsCount()
{
	UINT32 i;

	i = timer_counter_intr;
	if (i >= Last250usCount)
		i -= Last250usCount;
	else
		i = 0xFFFFFFFFL - (Last250usCount - i);
	i >>= 2;
	if(i) {
		Last250usCount = (timer_counter_intr & 0xFFFFFFFCL);
		MsCnt += i;
	}

	return MsCnt;
}

/**
 * Wrapper for the UART ISR.
 *
 * Uses the PS UART driver if it exists, else uses the PL UART one.
 *
 * @param [in] instance - Pointer to the UART driver handler passed by the
 *                        application.
 *
 * @return void
 */
void uart_int_handler(void *instance)
{
#if defined(XUARTPS_H)
	XUartPs_InterruptHandler(instance);
#elif defined(XUARTLITE_H)
	XUartLite_InterruptHandler(instance);
#endif
}

/**
 * Returns the status of the interrupts asserted in the ADV7511 IC.
 *
 * @param void
 *
 * @return status of the ADV7511 interrupts
 */
bool HAL_TxIntPending()
{
	TX_INTERRUPTS interrupts;

	TXHAL_GetInterrupts(&interrupts);

	return (interrupts.AudFifoFull | interrupts.BksvReady |
		interrupts.EdidReady | interrupts.EmbSyncErr | interrupts.HdcpAuth |
		interrupts.HdcpError | interrupts.Hpd | interrupts.MonSen |
		interrupts.Vsync | interrupts.Cec.ArbLost | interrupts.Cec.RxReady |
		interrupts.Cec.RxReady1 | interrupts.Cec.RxReady2 |
		interrupts.Cec.RxReady3 | interrupts.Cec.Timeout |
		interrupts.Cec.TxReady | interrupts.Cec.RxFrameOrder[0] |
		interrupts.Cec.RxFrameOrder[1] | interrupts.Cec.RxFrameOrder[2]) ?
	       true : false;
}
