/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h" /* Declarations of disk functions */
#include "ff.h"     /* Obtains integer types */
#include "mxc_errors.h"
#ifdef EXTERNAL_FLASH
#include "mscmem.h"
#endif
/* Definitions of physical drive number for each drive */
#define DEV_SD 0 /* Example: Map MMC/SD card to physical drive 1 */
#define DEV_EXTERNAL_FLASH 1

#ifndef SDHC_CLK_FREQ
/* For non-native SDHC, a SPI is used to emulate the SDHC hardware.  In
that case, sdhc_lib.h is not included and we need to redefine a default speed
here. */
#define SDHC_CLK_FREQ 40000000
#endif
#define SPI_SPEED SDHC_CLK_FREQ

#ifdef NATIVE_SDHC

/* # of times to check for a card, should be > 1 to detect both SD and MMC */
#define INIT_CARD_RETRIES 10

/* Prototypes for module-only functions */
static DRESULT ctrl_sync (void *buff, BYTE pdrv);
static DRESULT get_sector_count (void *buff, BYTE pdrv);
static DRESULT get_block_size (void *buff, BYTE pdrv);
static DRESULT mmc_get_csd (void *buff);

/* Globals */
static unsigned int init_done = 0;

/*local vaiables*/
static uint8_t rtc_en;
/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS
disk_status (BYTE pdrv /* Physical drive nmuber to identify the drive */
	    )
{
	DSTATUS status = 0;
	switch (pdrv) {
	case DEV_SD:

		if (!MXC_SDHC_Card_Inserted ()) {
			init_done = 0;
			status = STA_NOINIT | STA_NODISK;
		}
		break;

#ifdef EXTERNAL_FLASH
	case DEV_EXTERNAL_FLASH:

		if (!mscmem_ID ()) {
			init_done = 0;
			status = STA_NOINIT | STA_NODISK;
		}
		break;

#endif
	}
	return status;
}

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS
disk_initialize (BYTE pdrv /* Physical drive nmuber to identify the drive */
		)
{
	DSTATUS status;
	switch (pdrv) {
	case DEV_SD:

		rtc_en = 0;
#if (FF_FS_NORTC == 0)
		// Initialize RTC
		if (MXC_RTC->cn & MXC_F_RTC_CN_WE) {
			rtc_en = 1;
		} else {
			start_time_sec = (FF_NORTC_YEAR - 1980) * SEC_IN_YEAR_AVG;
			start_time_sec += FF_NORTC_MON * SEC_IN_MONTH_AVG;
			start_time_sec += FF_NORTC_MDAY * SEC_IN_DAY;
			if (RTC_init (MXC_RTC, start_time_sec, 0) == E_NO_ERROR) {
				rtc_en = 1;
			}
		}
#endif
		if (MXC_SDHC_Card_Inserted ()
		    && (MXC_SDHC_Lib_InitCard (INIT_CARD_RETRIES) == E_NO_ERROR)) {
			/* Card initialized and ready for work */
			init_done = 1;
			status = 0;
		} else {
			status = STA_NOINIT;
		}

		return status;
#ifdef EXTERNAL_FLASH
	case DEV_EXTERNAL_FLASH:

		rtc_en = 0;
#if (FF_FS_NORTC == 0)
		// Initialize RTC
		if (MXC_RTC->cn & MXC_F_RTC_CN_WE) {
			rtc_en = 1;
		} else {
			dstart_time_sec = (FF_NORTC_YEAR - 1980) * SEC_IN_YEAR_AVG;
			start_time_sec += FF_NORTC_MON * SEC_IN_MONTH_AVG;
			start_time_sec += FF_NORTC_MDAY * SEC_IN_DAY;
			if (RTC_init (MXC_RTC, start_time_sec, 0) == E_NO_ERROR) {
				rtc_en = 1;
			}
		}
#endif
		mscmem_Init ();
		init_done = 1;
		status = 0;
		return status;

#endif
	}
	return 0;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT
disk_read (BYTE pdrv,    /* Physical drive nmuber to identify the drive */
	   BYTE *buff,   /* Data buffer to store read data */
	   DWORD sector, /* Start sector in LBA */
	   UINT count    /* Number of sectors to read */
	  )
{
	DRESULT status;
	switch (pdrv) {
	case DEV_SD:

		if (MXC_SDHC_Lib_Read (buff, sector, count, MXC_SDHC_LIB_SINGLE_DATA)
		    != E_NO_ERROR) {
			status = RES_ERROR;
		} else {
			status = RES_OK;
		}

		return status;

#ifdef EXTERNAL_FLASH
	case DEV_EXTERNAL_FLASH:
		status = RES_OK;
		for (int index = 0; index < count; index++) {
			mscmem_Read (sector, buff);
			sector++;
			buff += 512;
		}
		return status;

#endif
	}
	return 0;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT
disk_write (BYTE pdrv,        /* Physical drive nmuber to identify the drive */
	    const BYTE *buff, /* Data to be written */
	    DWORD sector,     /* Start sector in LBA */
	    UINT count        /* Number of sectors to write */
	   )
{
	DRESULT status;
	switch (pdrv) {
	case DEV_SD:

		if (MXC_SDHC_Lib_Write (sector, (void *)buff, count,
					MXC_SDHC_LIB_SINGLE_DATA)
		    != E_NO_ERROR) {
			status = RES_ERROR;
		} else {
			status = RES_OK;
		}

		return status;

#ifdef EXTERNAL_FLASH
	case DEV_EXTERNAL_FLASH:

		status = RES_OK;
		for (int index = 0; index < count; index++) {
			/*mscmem_write function does not manipulate buff variable*/
			mscmem_Write (sector, (uint8_t *)buff);
			sector++;
			buff += 512;
		}
		return status;

#endif
	}
	return 0;
}

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT
disk_ioctl (BYTE pdrv, /* Physical drive nmuber (0..) */
	    BYTE cmd,  /* Control code */
	    void *buff /* Buffer to send/receive control data */
	   )
{
	DRESULT status = RES_ERROR;
	switch (cmd) {
	case CTRL_SYNC:
		/* Mandatory */
		status = ctrl_sync (buff, pdrv);
		break;
	case GET_SECTOR_COUNT:
		/* Mandatory */
		status = get_sector_count (buff, pdrv);
		break;
	case GET_BLOCK_SIZE:
		/* Mandatory */
		status = get_block_size (buff, pdrv);
		break;
	case MMC_GET_CSD:
		/* Optional */
		status = mmc_get_csd (buff);
		break;
#ifdef EXTERNAL_FLASH
	case GET_SECTOR_SIZE:
		*(DWORD *)buff = EXT_FLASH_SECTOR_SIZE;
		break;
#endif
	default:
		status = RES_PARERR;
		break;
	}

	return status;
}

DWORD
get_fattime (void)
{
	if (rtc_en) {
		DWORD result;
		uint32_t seconds;
		uint8_t year, month, day, hour, minute, half_seconds;

		// Convert RTC Seconds to time
		seconds = MXC_RTC->sec;
		year = seconds / SEC_IN_YEAR_AVG;    // year from epoch
		seconds = seconds % SEC_IN_YEAR_AVG; // seconds from Jan 1, $year
		month = seconds / SEC_IN_MONTH_AVG;
		seconds = seconds % SEC_IN_MONTH_AVG;
		day = seconds / SEC_IN_DAY; // hours from 12:00am
		seconds = seconds % SEC_IN_DAY;
		hour = seconds / SEC_IN_HOUR;
		seconds = seconds % SEC_IN_HOUR;
		minute = seconds / SEC_IN_MINUTE;
		seconds = seconds % SEC_IN_MINUTE;
		half_seconds = seconds * 2;

		/* Mask bits for inclusion in result */
		year &= 0x7F;
		month &= 0x0F;
		day &= 0x1F;
		hour &= 0x1F;
		minute &= 0x3F;
		half_seconds &= 0x1F;

		/* Add fields into 32bit result */
		result = year << 25;
		result |= month << 21;
		result |= day << 16;
		result |= hour << 11;
		result |= minute << 5;
		result |= half_seconds;
		return result;
	} else {
		return RES_NOTRDY;
	}
}

static DRESULT
ctrl_sync (void *buff, BYTE pdrv)
{
	switch (pdrv) {
	case DEV_SD:
		return RES_OK;
#ifdef EXTERNAL_FLASH
	case DEV_EXTERNAL_FLASH:
		mscmem_write_dirty_sector ();
		return RES_OK;

#endif
	}

	return 0;
}

static DRESULT
get_sector_count (void *buff, BYTE pdrv)
{
	DRESULT status = RES_ERROR;
	mxc_sdhc_csd_regs_t csd;

	switch (pdrv) {
	case DEV_SD:

		if (init_done) {
			int err = MXC_SDHC_Lib_GetCSD (&csd);
			if (err == E_NO_ERROR) {
				*((DWORD *)buff) = (csd.csd.c_size + 1) << 10;
				// The "sector count" for FatFS is the total data area capacity
				// (in bytes) divided by the size of a sector (in bytes).  An
				// SDHC sector is always 512 bytes, and this is guaranteed by the
				// spec.

				// The formula given by the CSD register spec v2.0 (which can be
				// found in the SD physical layer spec) is memory capacity =
				// (C_SIZE + 1) * 512 * 1024 so dividing by the sector size, we
				// have sector count = memory capacity / 512 = (C_SIZE + 1) *
				// 1024 = (C_SIZE + 1) << 10

				// NOTE: We used to multiply by 512 in MXC_SDHC_Lib_GetCSD, then
				// divide again by 512 here. That caused 32-bit integer
				// overflow...  so be wary of overflow!
				status = RES_OK;
			}
		} else {
			status = RES_NOTRDY;
		}
		break;

#ifdef EXTERNAL_FLASH
	case DEV_EXTERNAL_FLASH:
		*(DWORD *)buff = EXT_FLASH_NUM_SECTORS * (EXT_FLASH_SECTOR_SIZE / 512);
		status = RES_OK;
		break;
#endif
	}
	return status;
}

static DRESULT
get_block_size (void *buff, BYTE pdrv)
{
	DRESULT status = RES_ERROR;
	mxc_sdhc_csd_regs_t csd;

	switch (pdrv) {
	case DEV_SD:

		if (init_done) {
			if (MXC_SDHC_Lib_GetCSD (&csd) == E_NO_ERROR) {
				*((DWORD *)buff) = MXC_SDHC_Lib_GetBlockSize (&csd);
				status = RES_OK;
			}
		} else {
			status = RES_NOTRDY;
		}
		break;
#ifdef EXTERNAL_FLASH
	case DEV_EXTERNAL_FLASH:

		*(DWORD *)buff = 1;
		status = RES_OK;
		break;
#endif
	}
	return status;
}

static DRESULT
mmc_get_csd (void *buff)
{
	DRESULT status = RES_ERROR;

	if (init_done) {
		if (MXC_SDHC_Lib_GetCSD (buff) == E_NO_ERROR) {
			status = RES_OK;
		}
	} else {
		status = RES_NOTRDY;
	}

	return status;
}

#else // NATIVE_SDHC

#include <mxc_delay.h>
// #include "board.h"
#include "gpio.h"
#include "spi.h"

#define CS_H()                                                                \
  (mmc_ssPort->out_set = (1 << mmc_ssPin), MXC_GPIO2->out_set = (1 << 6))
#define CS_L()                                                                \
  (mmc_ssPort->out_clr = (1 << mmc_ssPin), MXC_GPIO2->out_clr = (1 << 6))

static int initialized = 0;
static mxc_spi_regs_t *mmc_spi = 0;
static mxc_gpio_regs_t *mmc_ssPort = 0;
static int mmc_ssPin = -1;
static BYTE CardType; /* b0:MMC, b1:SDv1, b2:SDv2, b3:Block addressing */

/*--------------------------------------------------------------------------

   Module Private Functions

---------------------------------------------------------------------------*/

static void
dly_us (UINT n) /* Delay n microseconds (avr-gcc -Os) */
{
	MXC_Delay (n);
}

/* MMC/SD command (SPI mode) */
#define CMD0 (0)           /* GO_IDLE_STATE */
#define CMD1 (1)           /* SEND_OP_COND */
#define ACMD41 (0x80 + 41) /* SEND_OP_COND (SDC) */
#define CMD8 (8)           /* SEND_IF_COND */
#define CMD9 (9)           /* SEND_CSD */
#define CMD10 (10)         /* SEND_CID */
#define CMD12 (12)         /* STOP_TRANSMISSION */
#define CMD13 (13)         /* SEND_STATUS */
#define ACMD13 (0x80 + 13) /* SD_STATUS (SDC) */
#define CMD16 (16)         /* SET_BLOCKLEN */
#define CMD17 (17)         /* READ_SINGLE_BLOCK */
#define CMD18 (18)         /* READ_MULTIPLE_BLOCK */
#define CMD23 (23)         /* SET_BLOCK_COUNT */
#define ACMD23 (0x80 + 23) /* SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24 (24)         /* WRITE_BLOCK */
#define CMD25 (25)         /* WRITE_MULTIPLE_BLOCK */
#define CMD32 (32)         /* ERASE_ER_BLK_START */
#define CMD33 (33)         /* ERASE_ER_BLK_END */
#define CMD38 (38)         /* ERASE */
#define CMD55 (55)         /* APP_CMD */
#define CMD58 (58)         /* READ_OCR */

static DSTATUS Stat = STA_NOINIT; /* Disk status */

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/
static void
init_mmc ()
{
	mxc_spi_pins_t pins;
	mxc_gpio_cfg_t gpio;

	SD_Get_Connections (&mmc_spi, &mmc_ssPort, &mmc_ssPin);

	MXC_ASSERT (mmc_spi);
	MXC_ASSERT (mmc_ssPort);
	MXC_ASSERT (mmc_ssPin >= 0);

	dly_us (10000); /* 10ms */

	gpio.port = MXC_GPIO0;
	gpio.mask = 1 << mmc_ssPin;
	gpio.func = MXC_GPIO_FUNC_OUT;
	gpio.pad = MXC_GPIO_PAD_NONE;
	gpio.vssel = MXC_GPIO_VSSEL_VDDIOH;
	MXC_GPIO_Config (&gpio);

	gpio.port = MXC_GPIO2;
	gpio.mask = 1 << 6;
	gpio.func = MXC_GPIO_FUNC_OUT;
	gpio.pad = MXC_GPIO_PAD_NONE;
	gpio.vssel = MXC_GPIO_VSSEL_VDDIOH;
	MXC_GPIO_Config (&gpio);

	CS_H ();

	pins.clock = true;
	pins.ss0 = false;
	pins.ss1 = false;
	pins.ss2 = false;
	pins.miso = true;
	pins.mosi = true;
	pins.sdio2 = false;
	pins.sdio3 = false;
	pins.vddioh = true;
	MXC_SPI_Init (mmc_spi, 1, 0, 0, 0, SPI_SPEED, pins);
	MXC_SPI_SetDataSize (mmc_spi, 8);
	MXC_SPI_SetDefaultTXData (mmc_spi, 0xFF);

	SD_Power (1);

	initialized = 1;
}

DSTATUS
disk_status (BYTE pdrv /* Physical drive nmuber to identify the drive */
	    )
{
	/* Only a single drive (drive 0) is supported. */
	if (pdrv) {
		return STA_NOINIT;
	}

	return Stat;
}

/*-----------------------------------------------------------------------*/
/* Transmit bytes to the card (bitbanging)                               */
/*-----------------------------------------------------------------------*/

static void
xmit_mmc (const BYTE *buff, /* Data to be sent */
	  UINT bc           /* Number of bytes to send */
	 )
{
	mxc_spi_req_t req;

	if (!initialized) {
		init_mmc ();
	}

	req.spi = mmc_spi;
	req.ssIdx = 0;
	req.ssDeassert = 0;
	req.txData = (uint8_t *)buff;
	req.rxData = 0;
	req.txLen = bc;
	req.rxLen = 0;
	req.txCnt = 0;
	req.rxCnt = 0;
	req.completeCB = 0;

	MXC_SPI_MasterTransaction (&req);
}

/*-----------------------------------------------------------------------*/
/* Receive bytes from the card (bitbanging)                              */
/*-----------------------------------------------------------------------*/

static void
rcvr_mmc (BYTE *buff, /* Pointer to read buffer */
	  UINT bc     /* Number of bytes to receive */
	 )
{
	mxc_spi_req_t req;

	if (!initialized) {
		init_mmc ();
	}

	req.spi = mmc_spi;
	req.ssIdx = 0;
	req.ssDeassert = 0;
	req.txData = 0;
	req.rxData = buff;
	req.txLen = 0;
	req.rxLen = bc;
	req.txCnt = 0;
	req.rxCnt = 0;
	req.completeCB = 0;

	MXC_SPI_MasterTransaction (&req);
}

/*-----------------------------------------------------------------------*/
/* Wait for card ready                                                   */
/*-----------------------------------------------------------------------*/

static int
wait_ready (void) /* 1:OK, 0:Timeout */
{
	BYTE d;
	UINT tmr;

	for (tmr = 5000; tmr; tmr--) {
		/* Wait for ready in timeout of 500ms */
		rcvr_mmc (&d, 1);
		if (d == 0xFF)
			break;
		dly_us (100);
	}

	return tmr ? 1 : 0;
}

/*-----------------------------------------------------------------------*/
/* Deselect the card and release SPI bus                                 */
/*-----------------------------------------------------------------------*/

static void
deselect (void)
{
	BYTE d;

	CS_H ();          /* Set CS# high */
	rcvr_mmc (&d, 1); /* Dummy clock (force DO hi-z for multiple slave SPI) */
}

/*-----------------------------------------------------------------------*/
/* Select the card and wait for ready                                    */
/*-----------------------------------------------------------------------*/

static int
sd_select (void) /* 1:OK, 0:Timeout */
{
	BYTE d;

	CS_L ();          /* Set CS# low */
	rcvr_mmc (&d, 1); /* Dummy clock (force DO enabled) */
	if (wait_ready ())
		return 1; /* Wait for card ready */

	deselect ();
	return 0; /* Failed */
}

/*-----------------------------------------------------------------------*/
/* Receive a data packet from the card                                   */
/*-----------------------------------------------------------------------*/

static int
rcvr_datablock (            /* 1:OK, 0:Failed */
	BYTE *buff, /* Data buffer to store received data */
	UINT btr    /* Byte count */
)
{
	BYTE d[2];
	UINT tmr;

	for (tmr = 1000; tmr; tmr--) {
		/* Wait for data packet in timeout of 100ms */
		rcvr_mmc (d, 1);
		if (d[0] != 0xFF)
			break;
		dly_us (100);
	}
	if (d[0] != 0xFE)
		return 0; /* If not valid data token, return with error */

	rcvr_mmc (buff, btr); /* Receive the data block into buffer */
	rcvr_mmc (d, 2);      /* Discard CRC */

	return 1; /* Return with success */
}

/*-----------------------------------------------------------------------*/
/* Send a data packet to the card                                        */
/*-----------------------------------------------------------------------*/

static int
xmit_datablock (                  /* 1:OK, 0:Failed */
	const BYTE *buff, /* 512 byte data block to be transmitted */
	BYTE token        /* Data/Stop token */
)
{
	BYTE d[2];

	if (!wait_ready ())
		return 0;

	d[0] = token;
	xmit_mmc (d, 1); /* Xmit a token */
	if (token != 0xFD) {
		/* Is it data token? */
		xmit_mmc (buff, 512);      /* Xmit the 512 byte data block to MMC */
		rcvr_mmc (d, 2);           /* Xmit dummy CRC (0xFF,0xFF) */
		rcvr_mmc (d, 1);           /* Receive data response */
		if ((d[0] & 0x1F) != 0x05) /* If not accepted, return with error */
			return 0;
	}

	return 1;
}

/*-----------------------------------------------------------------------*/
/* Send a command packet to the card                                     */
/*-----------------------------------------------------------------------*/

static BYTE
send_cmd (          /* Returns command response (bit7==1:Send failed)*/
	BYTE cmd, /* Command byte */
	DWORD arg /* Argument */
)
{
	BYTE n, d, buf[6];

	if (cmd & 0x80) {
		/* ACMD<n> is the command sequense of CMD55-CMD<n> */
		cmd &= 0x7F;
		n = send_cmd (CMD55, 0);
		if (n > 1)
			return n;
	}

	/* Select the card and wait for ready except to stop multiple block read */
	if (cmd != CMD12) {
		deselect ();
		if (!sd_select ())
			return 0xFF;
	}

	/* Send a command packet */
	buf[0] = 0x40 | cmd;        /* Start + Command index */
	buf[1] = (BYTE)(arg >> 24); /* Argument[31..24] */
	buf[2] = (BYTE)(arg >> 16); /* Argument[23..16] */
	buf[3] = (BYTE)(arg >> 8);  /* Argument[15..8] */
	buf[4] = (BYTE)arg;         /* Argument[7..0] */
	n = 0x01;                   /* Dummy CRC + Stop */
	if (cmd == CMD0)
		n = 0x95; /* (valid CRC for CMD0(0)) */
	if (cmd == CMD8)
		n = 0x87; /* (valid CRC for CMD8(0x1AA)) */
	buf[5] = n;
	xmit_mmc (buf, 6);

	/* Receive command response */
	if (cmd == CMD12)
		rcvr_mmc (&d, 1); /* Skip a stuff byte when stop reading */
	n = 10;             /* Wait for a valid response in timeout of 10 attempts */
	do {
		rcvr_mmc (&d, 1);
	} while ((d & 0x80) && --n);

	return d; /* Return with the response value */
}

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS
disk_initialize (BYTE pdrv /* Physical drive nmuber to identify the drive */
		)
{
	BYTE n, ty, cmd, buf[4];
	UINT tmr;
	DSTATUS s;

	if (pdrv)
		return RES_NOTRDY;

	dly_us (10000); /* 10ms */

	init_mmc ();

	for (n = 10; n; n--)
		rcvr_mmc (buf, 1); /* Apply 80 dummy clocks and the card gets ready to
                          receive command */

	ty = 0;
	int x = send_cmd (CMD0, 0);
	if (x == 1) {
		/* Enter Idle state */
		if (send_cmd (CMD8, 0x1AA) == 1) {
			/* SDv2? */
			rcvr_mmc (buf, 4); /* Get trailing return value of R7 resp */
			if (buf[2] == 0x01 && buf[3] == 0xAA) {
				/* The card can work at vdd range of 2.7-3.6V */
				for (tmr = 1000; tmr; tmr--) {
					/* Wait for leaving idle state (ACMD41 with HCS bit) */
					if (send_cmd (ACMD41, 1UL << 30) == 0)
						break;
					dly_us (1000);
				}
				if (tmr && send_cmd (CMD58, 0) == 0) {
					/* Check CCS bit in the OCR */
					rcvr_mmc (buf, 4);
					ty = (buf[0] & 0x40) ? CT_SDC2 | CT_BLOCK
					     : CT_SDC2; /* SDv2+ */
				}
			}
		} else {
			/* SDv1 or MMCv3 */
			if (send_cmd (ACMD41, 0) <= 1) {
				ty = CT_SDC2;
				cmd = ACMD41; /* SDv1 */
			} else {
				ty = CT_MMC3;
				cmd = CMD1; /* MMCv3 */
			}
			for (tmr = 1000; tmr; tmr--) {
				/* Wait for leaving idle state */
				if (send_cmd (cmd, 0) == 0)
					break;
				dly_us (1000);
			}
			if (!tmr
			    || send_cmd (CMD16, 512) != 0) /* Set R/W block length to 512 */
				ty = 0;
		}
	}
	CardType = ty;
	s = ty ? 0 : STA_NOINIT;
	Stat = s;

	deselect ();

	return s;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT
disk_read (BYTE pdrv,    /* Physical drive nmuber to identify the drive */
	   BYTE *buff,   /* Data buffer to store read data */
	   DWORD sector, /* Start sector in LBA */
	   UINT count    /* Number of sectors to read */
	  )
{
	BYTE cmd;
	DWORD sect = (DWORD)sector;

	if (disk_status (pdrv) & STA_NOINIT)
		return RES_NOTRDY;
	if (!(CardType & CT_BLOCK))
		sect *= 512; /* Convert LBA to byte address if needed */

	cmd = count > 1 ? CMD18
	      : CMD17; /*  READ_MULTIPLE_BLOCK : READ_SINGLE_BLOCK */
	if (send_cmd (cmd, sect) == 0) {
		do {
			if (!rcvr_datablock (buff, 512))
				break;
			buff += 512;
		} while (--count);
		if (cmd == CMD18)
			send_cmd (CMD12, 0); /* STOP_TRANSMISSION */
	}
	deselect ();

	return count ? RES_ERROR : RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT
disk_write (BYTE pdrv,        /* Physical drive nmuber to identify the drive */
	    const BYTE *buff, /* Data to be written */
	    DWORD sector,     /* Start sector in LBA */
	    UINT count        /* Number of sectors to write */
	   )
{
	DWORD sect = (DWORD)sector;

	if (disk_status (pdrv) & STA_NOINIT)
		return RES_NOTRDY;
	if (!(CardType & CT_BLOCK))
		sect *= 512; /* Convert LBA to byte address if needed */

	if (count == 1) {
		/* Single block write */
		if ((send_cmd (CMD24, sect) == 0) /* WRITE_BLOCK */
		    && xmit_datablock (buff, 0xFE))
			count = 0;
	} else {
		/* Multiple block write */
		if (CardType & CT_SDC)
			send_cmd (ACMD23, count);
		if (send_cmd (CMD25, sect) == 0) {
			/* WRITE_MULTIPLE_BLOCK */
			do {
				if (!xmit_datablock (buff, 0xFC))
					break;
				buff += 512;
			} while (--count);
			if (!xmit_datablock (0, 0xFD)) /* STOP_TRAN token */
				count = 1;
		}
	}
	deselect ();

	return count ? RES_ERROR : RES_OK;
}

#endif

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT
disk_ioctl (BYTE pdrv, /* Physical drive nmuber (0..) */
	    BYTE ctrl, /* Control code */
	    void *buff /* Buffer to send/receive control data */
	   )
{
	DRESULT res;
	BYTE n, csd[16];
	DWORD cs;

	if (disk_status (pdrv) & STA_NOINIT)
		return RES_NOTRDY; /* Check if card is in the socket */

	res = RES_ERROR;
	switch (ctrl) {
	case CTRL_SYNC: /* Make sure that no pending write process */
		if (sd_select ())
			res = RES_OK;
		break;

	case GET_SECTOR_COUNT: /* Get number of sectors on the disk (DWORD) */
		if ((send_cmd (CMD9, 0) == 0) && rcvr_datablock (csd, 16)) {
			if ((csd[0] >> 6) == 1) {
				/* SDC ver 2.00 */
				cs = csd[9] + ((WORD)csd[8] << 8) + ((DWORD)(csd[7] & 63) << 16)
				     + 1;
				*(DWORD *)buff = cs << 10;
			} else {
				/* SDC ver 1.XX or MMC */
				n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1)
				    + 2;
				cs = (csd[8] >> 6) + ((WORD)csd[7] << 2)
				     + ((WORD)(csd[6] & 3) << 10) + 1;
				*(DWORD *)buff = cs << (n - 9);
			}
			res = RES_OK;
		}
		break;

	case GET_BLOCK_SIZE: /* Get erase block size in unit of sector (DWORD) */
		*(DWORD *)buff = 128;
		res = RES_OK;
		break;

	default:
		res = RES_PARERR;
	}

	deselect ();

	return res;
}

#endif // NATIVE_SDHC
