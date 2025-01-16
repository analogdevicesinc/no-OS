/*******************************************************************************
* Copyright (C) 2018 Maxim Integrated Products, Inc. (now owned by Analog Devices, Inc.),
* 2024 Analog Devices, Inc. This software is proprietary and
* confidential to Analog Devices, Inc. and its licensors. , All rights Reserved.
*
* This software is protected by copyright laws of the United States and
* of foreign countries. This material may also be protected by patent laws
* and technology transfer regulations of the United States and of foreign
* countries. This software is furnished under a license agreement and/or a
* nondisclosure agreement and may only be used or reproduced in accordance
* with the terms of those agreements. Dissemination of this information to
* any party or parties not specified in the license agreement and/or
* nondisclosure agreement is expressly prohibited.
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
* OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*
* Except as contained in this notice, the name of Maxim Integrated
* Products, Inc. shall not be used except as stated in the Maxim Integrated
* Products, Inc. Branding Policy.
*
* The mere transfer of this software does not imply any licenses
* of trade secrets, proprietary technology, copyrights, patents,
* trademarks, maskwork rights, or any other form of intellectual
* property whatsoever. Maxim Integrated Products, Inc. retains all
* ownership rights.
******************************************************************************/


/**
 *
 * @internal
 * @file
 * @date Apr 2014
 * @authors Stéphane Di Vito, Maxim Integrated Products Inc.
 * @authors Benjamin Vinot, Maxim Integrated Products Inc.
 *
 * @brief System driver for the MAXQ106x's Host library (PC Version)
 * @details
 * @ingroup drivers
 * @internal
 */

#include "MXQ_Internal.h"

#ifdef __MINGW32__
#    include <windows.h>

#else
//#    define __USE_POSIX199309
#    include <stdlib.h>
#    include <unistd.h>
#    include <time.h>
#    include <errno.h>
#endif

int ibdelay,commspeed;

#if defined(USE_PROTOCOL_SPI) || defined (USE_PROTOCOL_I2C)

#if (PC_INTERFACE==AARDVARK)
#   include "aardvark.h"
	#define AA_PIN_RESET AA_GPIO_SDA
	#define AA_PIN_MODE  AA_GPIO_SCL
    unsigned int mode = AA_PIN_RESET;

#elif (PC_INTERFACE==CHEETAH)
#   ifndef USE_PROTOCOL_SPI
#      error Cheetah supports only SPI
#   endif
#   include "cheetah.h"
#   define SS1 0x00000001
#   define SS2 0x00000002 // aka SCL pin of aardvark -> Reset_N on breakout board (if JP1 set correctly)
#   define SS3 0x00000004 // aka SDA pin of aardvark -> Remote mode on breakout board (if JP2 set correctly)
    unsigned int mode = SS3;

#elif (PC_INTERFACE==FTDI4222)
#include "libft4222.h"

#else
#   error No USB to SPI interface selected, you must define either CHEETAH or AARDVARK to work on a PC.
#endif

#endif //defined(USE_PROTOCOL_SPI) || defined (USE_PROTOCOL_I2C)


#ifdef USE_PROTOCOL_PIPE
#include <fcntl.h>
#include <sched.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <termios.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <unistd.h>

int fd_tosmfifo=0;
int fd_fromsmfifo=0;
char * tosmfifoname = "/tmp/maxq/tosmfifo";
char * fromsmfifoname = "/tmp/maxq/fromsmfifo";

static void wait(void)
{
	usleep(10);
	sched_yield();
}

#endif //USE_PROTOCOL_PIPE

#ifdef USE_PROTOCOL_SERIAL
int fd_serialport=0; /* File descriptor for the port */
#endif


int myinterface=0;



#define TIMEOUT 			120000L
#ifdef MAXQ1065

    #define TIME_WAIT_RESET		500000L
#else

    #define TIME_WAIT_RESET		120000L
#endif




void HOST_wait_usec(mxq_u4 duration_usec)
{
	#ifdef __MINGW32__
	
    __int64 time1 = 0, time2 = 0, freq = 0;

    QueryPerformanceCounter((LARGE_INTEGER  *) &time1);
    QueryPerformanceFrequency((LARGE_INTEGER  *)&freq);
    duration_usec *= (freq /1000000);

    do {
        QueryPerformanceCounter((LARGE_INTEGER  *) &time2);
    } while((time2-time1) < duration_usec);

	#else

    struct timespec tim;
    tim.tv_sec = 0;
    tim.tv_nsec = duration_usec * 1000;

	nanosleep(&tim, NULL);
	#endif
}


void HOST_wait_miliseconds(mxq_u4 duration_ms)
{
	#ifdef __MINGW32__

	LARGE_INTEGER time1, time2, freq, elapsed;

	QueryPerformanceCounter((LARGE_INTEGER *)&time1);
	QueryPerformanceFrequency((LARGE_INTEGER *)&freq);

	do {
		QueryPerformanceCounter((LARGE_INTEGER *)&time2);
		elapsed.QuadPart = time2.QuadPart - time1.QuadPart;
		elapsed.QuadPart *= 1000;              //first scale up counts
		elapsed.QuadPart /= freq.QuadPart;     //then convert to ms
	} while(elapsed.QuadPart < duration_ms);

	#else

	struct timespec tim;

	if (duration_ms >= 1000)
	{
		tim.tv_sec = (int)(duration_ms / 1000);                            /* Must be Non-Negative */
		tim.tv_nsec = (duration_ms - ((long)tim.tv_sec * 1000)) * 1000000; /* Must be in range of 0 to 999999999 */
	}
	else
	{
		tim.tv_sec = 0;                         /* Must be Non-Negative */
		tim.tv_nsec = duration_ms * 1000000;    /* Must be in range of 0 to 999999999 */
	}

	nanosleep(&tim, NULL);
	#endif
}


#ifdef USE_PROTOCOL_PIPE
mxq_err_t HOST_init_pipe(void)
{

	DBG_PRINT("Opening pipe communication...");
	close(fd_fromsmfifo);
	close(fd_tosmfifo);

	/* Open the pipe. Just like you open a file */
	do
	{
		fd_fromsmfifo = open(fromsmfifoname, O_RDONLY);
		if (fd_fromsmfifo<0) {MXQ_PRINT_ERR("error %d\n",errno); wait();}
	}
	while(fd_fromsmfifo<0);

	/* Open the pipe. Just like you open a file */
	do
	{
		fd_tosmfifo = open(tosmfifoname, O_WRONLY);
		if (fd_tosmfifo < 0) {MXQ_PRINT_ERR("error %d\n",errno); wait();}
	}
	while (fd_tosmfifo < 0);

	DBG_PRINT(" Done!\n");

	return MXQ_OK;

}
#endif

#ifdef USE_PROTOCOL_SERIAL

mxq_err_t HOST_init_serial(void)
{
	fd_serialport = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd_serialport == -1)
	{
		MXQ_PRINT("open_port: Unable to open /dev/ttyUSB0 - ");
		return errno;
	}

	struct termios termAttr;

	tcgetattr(fd_serialport, &termAttr);

	/* Speed */
	cfsetspeed(&termAttr, B57600);

	/* 8N1 no RTS/CTS, no modem mode */
	termAttr.c_cflag |= (CLOCAL | CREAD);
	termAttr.c_cflag &= ~(PARENB | PARODD);
	termAttr.c_cflag &= ~CSTOPB;
	termAttr.c_cflag &= ~CRTSCTS;
	termAttr.c_cflag &= ~CSIZE;
	termAttr.c_cflag |= CS8;

	termAttr.c_iflag &= ~( IXON | IXOFF | IXANY);
	termAttr.c_iflag &= ~(IGNBRK);

	termAttr.c_lflag = 0;
	termAttr.c_oflag = 0;

	/* Blocking */
	termAttr.c_cc[VMIN]  = 0;
	termAttr.c_cc[VMIN]  = 10;            /* 1s timeout */

	tcsetattr(fd_serialport, TCSANOW, &termAttr);

	return MXQ_OK;
}

#endif

#ifdef USE_PROTOCOL_PIPE
mxq_err_t HOST_send_bytes_pipe(const mxq_u1* src, mxq_length len)
{
	mxq_err_t err;
	err = write(fd_tosmfifo, src, len);
	if(err == -1) return errno;

	return err;
}


static mxq_err_t read_timeout(mxq_u1* dest, mxq_length len, int timeout_ms)
{
	  mxq_length rv;
	  if(timeout_ms>0)
	  {
		  fd_set set;
		  struct timeval timeout;

		  FD_ZERO(&set); /* clear the set */
		  FD_SET(fd_fromsmfifo, &set); /* add our file descriptor to the set */

		  timeout.tv_sec = 0;
		  timeout.tv_usec = timeout_ms*1000;

		  rv = select(fd_fromsmfifo + 1, &set, NULL, NULL, &timeout);

		  if (rv<=0) return (mxq_err_t)0; /* timeout or error */
	  }

      rv=read( fd_fromsmfifo, dest, len ); /* there was data to read */
	  if(rv<len) return (mxq_err_t)0;

	  return rv;
}

mxq_err_t HOST_receive_bytes_pipe_first(mxq_u1* dest, mxq_length len)
{
	return read_timeout(dest,len,70000);
}

mxq_err_t HOST_receive_bytes_pipe(mxq_u1* dest, mxq_length len)
{
	return read_timeout(dest,len,0);
}
#endif

#ifdef USE_PROTOCOL_SERIAL
int flushme;

mxq_err_t HOST_send_bytes_serial(const mxq_u1* src, mxq_length len)
{
	if(flushme) 
	{
		tcflush(fd_serialport, TCIOFLUSH);
		flushme=0;
	}

	int err;
	err=write(fd_serialport, src, len);
	if(err<0) return (mxq_err_t)0;

	return len;
}


static int wait_bytes_serial(mxq_u2 timeout_ms)
{
	if(timeout_ms>0)
	{
	  struct timeval timeout;

	  fd_set read_fds, write_fds, except_fds;
	  FD_ZERO(&read_fds);
	  FD_ZERO(&write_fds);
	  FD_ZERO(&except_fds);
	  FD_SET(fd_serialport, &read_fds);

	  timeout.tv_sec = 0;
	  timeout.tv_usec = timeout_ms*1000;

	  return select(fd_serialport +1 , &read_fds, &write_fds, &except_fds, &timeout);

	}

	return 1;
}


mxq_err_t HOST_receive_bytes_serial(mxq_u1* dest, mxq_length len)
{
	flushme=1;
	 int rv;
	 int bytes=0;

     ioctl(fd_serialport, FIONREAD, &bytes);

	 while(bytes<len)
	 {
	  ioctl(fd_serialport, FIONREAD, &bytes);
	  if (wait_bytes_serial(timeout_ms)<=0 )
		  return (mxq_err_t)0;
	 }

	rv=read( fd_serialport, dest, len ); /* there was data to read */
	if(rv<len)
		return (mxq_err_t)0;

	return rv;
}


mxq_err_t HOST_receive_bytes_serial_first(mxq_u1* dest, mxq_length len)
{
	return HOST_receive_bytes_serial(dest,len);
}
#endif


#ifdef USE_PROTOCOL_SPI

#if (PC_INTERFACE==AARDVARK)

int commspeed=1000;
int ibdelay=0;
mxq_err_t HOST_init_spi(void)
{

	mxq_u2 results[2];
	
	DBG_PRINT("SPI (Using the AArdvark adapter)\n");

	if(myinterface!=0) aa_close(myinterface);

	if(aa_find_devices( sizeof(results)/sizeof(mxq_u2), results )==0)
		return MXQ_ERR_TRANS_UNSUPPORTED_INTERFACE;

	myinterface=results[0];

	if((myinterface=aa_open(myinterface))==0)
		return MXQ_ERR_TRANS_UNSUPPORTED_INTERFACE;


	aa_gpio_set(myinterface,       AA_PIN_RESET); // Set SDA -> mode = 0 (TLS) and SCL -> reset_n=1
	aa_gpio_direction(myinterface, AA_PIN_MODE | AA_PIN_RESET);  // Set both as outputs...

	aa_configure(myinterface,AA_CONFIG_SPI_GPIO);

	if(aa_spi_master_ss_polarity(myinterface, AA_SPI_SS_ACTIVE_LOW)!=AA_OK)
		return MXQ_ERR_TRANS_UNSUPPORTED_INTERFACE;

	if(aa_spi_configure(
			myinterface,
			AA_SPI_POL_RISING_FALLING,
			AA_SPI_PHASE_SAMPLE_SETUP,
			AA_SPI_BITORDER_MSB
			)!=AA_OK) return MXQ_ERR_TRANS_UNSUPPORTED_INTERFACE;

	if(aa_spi_bitrate(
			myinterface,
			commspeed
			)!=commspeed) return MXQ_ERR_TRANS_UNSUPPORTED_INTERFACE;

	aa_target_power(myinterface, AA_TARGET_POWER_BOTH);
	aa_sleep_ms(40);

	DBG_PRINT("SPI init done\n");
	return MXQ_OK;

}


void* HOST_get_interface(void) {
	return (void*)myinterface;
}

void HOST_set_interface(void* i) {
	myinterface = i;
}



#elif (PC_INTERFACE==CHEETAH)
int commspeed=1000;
int ibdelay=0;
mxq_err_t HOST_init_spi(void)
{

	mxq_u2 results[2];
	
	DBG_PRINT("SPI (Using the Cheetah adapter)\n");

	if(myinterface!=0) ch_close(myinterface);

	if(ch_find_devices( sizeof(results)/sizeof(mxq_u2), results )<=0)
		return MXQ_ERR_TRANS_UNSUPPORTED_INTERFACE;

	myinterface = results[0];

	if((myinterface=ch_open(myinterface))<=0) return MXQ_ERR_TRANS_UNSUPPORTED_INTERFACE;
	//DBG_PRINT("handle:%d\n",myinterface);

    // Ensure that the SPI subsystem is configured. SS polarities are 0.
    int i=ch_spi_configure(myinterface, CH_SPI_POL_RISING_FALLING, CH_SPI_PHASE_SAMPLE_SETUP, CH_SPI_BITORDER_MSB, 0); // all SSs asserted when low
	//DBG_PRINT("config OK:%x\n",i);

    // Set the bitrate.
    //if(
    i=ch_spi_bitrate(myinterface, commspeed);//!=commspeed) return MXQ_ERR_TRANS_UNSUPPORTED_INTERFACE;
	DBG_PRINT("Bitrate: %d Hz\n",i);

    ch_spi_queue_clear(myinterface);
    ch_spi_queue_ss(myinterface, mode|SS3); 
    ch_spi_queue_oe(myinterface, 1);
    ch_spi_batch_shift(myinterface, 0,NULL);

    // Power the target using the Cheetah adapter's power supply.
    i=ch_target_power(myinterface, CH_TARGET_POWER_ON);
    ch_sleep_ms(40);

	return MXQ_OK;

}


void* HOST_get_interface(void) {
	return (void*)myinterface;
}

void HOST_set_interface(void* i) {
	myinterface = i;
}

#elif (PC_INTERFACE==FTDI4222)

FT_HANDLE            ftHandle = (FT_HANDLE)NULL;
FT_HANDLE            ftHandleGPIO = (FT_HANDLE)NULL;
// SPI Master can assert SS0O in single mode
// SS0O and SS1O in dual mode, and
// SS0O, SS1O, SS2O and SS3O in quad mode.
#define SLAVE_SELECT(x) (1 << (x))

void* HOST_get_interface(void) {
	return (void*)ftHandle;
}

void HOST_set_interface(void* i) {
	ftHandle = i;
}


mxq_err_t HOST_init_spi(void) {

	mxq_err_t r=MXQ_ERR_TRANS_UNSUPPORTED_INTERFACE;
    FT_STATUS status;
    DWORD numOfDevices = 0;
    DWORD ftdiDevNumA = 0, ftdiDevNumB = 1, ftdiDevFound = 0;


	if (ftHandle != (FT_HANDLE)NULL)
	{
		FT4222_UnInitialize(ftHandle);
		FT_Close(ftHandle);
		ftHandle = (FT_HANDLE)NULL;
	}

	if (ftHandleGPIO != (FT_HANDLE)NULL)
	{
		FT4222_UnInitialize(ftHandleGPIO);
		FT_Close(ftHandleGPIO);
		ftHandleGPIO = (FT_HANDLE)NULL;
	}

	status = FT_CreateDeviceInfoList(&numOfDevices);
	for(DWORD iDev=0; iDev<numOfDevices; ++iDev)
	{
		FT_DEVICE_LIST_INFO_NODE devInfo;
		memset(&devInfo, 0, sizeof(devInfo));

		status = FT_GetDeviceInfoDetail(iDev, &devInfo.Flags, &devInfo.Type,
							&devInfo.ID, &devInfo.LocId,
							devInfo.SerialNumber,
							devInfo.Description,
							&devInfo.ftHandle);

		if (FT4222_OK == status)
		{
			if ( !(strcmp(devInfo.Description, "FT4222 A")) ) {
				ftdiDevNumA = iDev;
				ftdiDevFound++;
				DBG_PRINT("FTDI FT4222 A found\n");
			}

			if ( !(strcmp(devInfo.Description, "FT4222 B")) ) {
				ftdiDevNumB = iDev;
				ftdiDevFound++;
				DBG_PRINT("FTDI FT4222 B found\n");
			}
		}
	}

	if (ftdiDevFound < 2)
	{
		MXQ_PRINTF ("No FTDI FT4222 found\n");
		goto exit;
	}

	if(status!=FT4222_OK) {MXQ_PRINTF ("No FTDI FT4222 found\n"); return MXQ_ERR_TRANS_ERROR;}

    FT4222_STATUS        ftStatus;
    FT4222_STATUS        ft4222Status;
    FT4222_Version       ft4222Version;


    ftStatus = FT_Open(ftdiDevNumB, &ftHandleGPIO);
    if (ftStatus != FT4222_OK)
    {
        MXQ_PRINTF("FT_Open failed (error %d)\n", 
               (int)ftStatus);
        goto exit;
    }

#if defined(MAXQ1065)
	GPIO_Dir gpioDir[4];
	gpioDir[0] = GPIO_INPUT;    // RESET_OUT_N
	gpioDir[1] = GPIO_INPUT;    // RDY_N
	gpioDir[2] = GPIO_OUTPUT;   // PDWN_N
	gpioDir[3] = GPIO_INPUT;    // WDI

	if(FT4222_OK!=FT4222_GPIO_Init(ftHandleGPIO, gpioDir)) exit (-1);
	//disable suspend out , enable gpio 2
	if(FT4222_OK!=FT4222_SetSuspendOut(ftHandleGPIO, 0)) exit (-2);
	//disable interrupt , enable gpio 3
	if(FT4222_OK!=FT4222_SetWakeUpInterrupt(ftHandleGPIO, 0)) exit (-3);

	if(FT4222_OK!=FT4222_GPIO_Write(ftHandleGPIO, GPIO_PORT2, 1)) exit (-6); // PDWN_N
	//if(FT4222_OK!=FT4222_GPIO_Write(ftHandleGPIO, GPIO_PORT3, 1)) exit (-7); // WDI

	MXQ_PRINTF("\nWaiting for power-up...\n");
	HOST_wait_miliseconds(250UL);
#elif defined(MAXQ108x)
	GPIO_Dir gpioDir[4];
	gpioDir[0] = GPIO_INPUT;    // I2C_SCL
	gpioDir[1] = GPIO_INPUT;    // I2C_SDA
	gpioDir[2] = GPIO_INPUT;    // SECI/O_2 (DNI)
	gpioDir[3] = GPIO_INPUT;    // RDY#

	if(FT4222_OK!=FT4222_GPIO_Init(ftHandleGPIO, gpioDir)) exit (-1);
	if(FT4222_OK!=FT4222_SetSuspendOut(ftHandleGPIO, 0)) exit (-2);      // disable suspend out, enable gpio 2
	if(FT4222_OK!=FT4222_SetWakeUpInterrupt(ftHandleGPIO, 0)) exit (-3); // disable interrupt, enable gpio 3
#else
	GPIO_Dir gpioDir[4];
	gpioDir[0] = GPIO_OUTPUT;
	gpioDir[1] = GPIO_OUTPUT;
	gpioDir[2] = GPIO_OUTPUT;
	gpioDir[3] = GPIO_OUTPUT;
	if(FT4222_OK!=FT4222_GPIO_Init(ftHandleGPIO, gpioDir)) exit (-1);
//disable suspend out , enable gpio 2
	if(FT4222_OK!=FT4222_SetSuspendOut(ftHandleGPIO, 0)) exit (-2);
//disable interrupt , enable gpio 3
	if(FT4222_OK!=FT4222_SetWakeUpInterrupt(ftHandleGPIO, 0)) exit (-3);
// set gpio0/gpio1/gpio2/gpio3 output level high
//	if(FT4222_OK!=FT4222_GPIO_Write(ftHandleGPIO, GPIO_PORT0, 1)) exit (-4); // REMOTE_MODE
	if(FT4222_OK!=FT4222_GPIO_Write(ftHandleGPIO, GPIO_PORT1, 1)) exit (-5); // WAKEUP_N
	if(FT4222_OK!=FT4222_GPIO_Write(ftHandleGPIO, GPIO_PORT2, 1)) exit (-6); // Reset_N
	if(FT4222_OK!=FT4222_GPIO_Write(ftHandleGPIO, GPIO_PORT3, 0)) exit (-7); // AUX LED
#endif

    ftStatus = FT_Open(ftdiDevNumA, &ftHandle);
    if (ftStatus != FT4222_OK)
    {
        MXQ_PRINTF("FT_Open failed (error %d)\n", 
               (int)ftStatus);
        goto exit;
    }
    
    ft4222Status = FT4222_GetVersion(ftHandle, &ft4222Version);
    if (FT4222_OK != ft4222Status)
    {
        MXQ_PRINTF("FT4222_GetVersion failed (error %d)\n",
               (int)ft4222Status);
        goto exit;
    }
    
    MXQ_PRINTF("FTDI Chip version: %08X, LibFT4222 version: %08X\n",
           (unsigned int)ft4222Version.chipVersion,
           (unsigned int)ft4222Version.dllVersion);

    ft4222Status = FT4222_SetClock(ftHandle, SYS_CLK_60);
    if (FT4222_OK != ft4222Status)
    {
        MXQ_PRINTF("FT4222_SetClock failed (error %d)\n",
               (int)ft4222Status);
        goto exit;
    }

    FT4222_ClockRate ftdi_clk;
    ft4222Status = FT4222_GetClock(ftHandle, &ftdi_clk);
    if (FT4222_OK != ft4222Status)
    {
        MXQ_PRINTF("FT4222_GetClock failed (error %d)\n",
               (int)ft4222Status);
        goto exit;
    }

    // Configure the FT4222 as an SPI Master.
    ft4222Status = FT4222_SPIMaster_Init(
                        ftHandle, 
                        SPI_IO_SINGLE,      // 1 channel
                    #ifdef MAXQ1065
                        CLK_DIV_8,          // 60 MHz / 8 == 7.5 MHz
                    #else
                        CLK_DIV_64,
                    #endif
                        CLK_IDLE_LOW,       // clock idles at logic 0
                        CLK_LEADING,        // data captured on rising edge
                        SLAVE_SELECT(0));   // Use SS0O for slave-select
    if (FT4222_OK != ft4222Status)
    {
        MXQ_PRINTF("FT4222_SPIMaster_Init failed (error %d)\n",
               (int)ft4222Status);
        goto exit;
    }

    ft4222Status = FT4222_SPI_SetDrivingStrength(ftHandle,
                                                 DS_8MA,
                                                 DS_8MA,
                                                 DS_8MA);
    if (FT4222_OK != ft4222Status)
    {
        MXQ_PRINTF("FT4222_SPI_SetDrivingStrength failed (error %d)\n",
               (int)ft4222Status);
        goto exit;
    }

    return MXQ_OK;

    exit:

    if (ftHandle != (FT_HANDLE)NULL)
    {
        (void)FT4222_UnInitialize(ftHandle);
        (void)FT_Close(ftHandle);
    }

    return r;
}


#endif

mxq_err_t HOST_send_bytes_spi(const mxq_u1* src, mxq_length len)
{

#if (PC_INTERFACE==AARDVARK)
	mxq_err_t ret;
	mxq_u1 * dummy = (mxq_u1 *)malloc(len);
	memset(dummy,0,len);
	start_time_all();
	ret =  aa_spi_write(myinterface, len, src, len,dummy);
	start_time_proc();
	free(dummy);
	return ret;

#elif (PC_INTERFACE==CHEETAH)

    ch_spi_queue_clear(myinterface);

    ch_spi_queue_ss(myinterface, mode|SS1);

    unsigned int i;
    for(i=0;i<len;i++) 
    {
    	ch_spi_queue_byte(myinterface, 1, src[i]);
    	if(ibdelay) ch_spi_queue_delay_ns(myinterface, ibdelay*100UL);
    }

    ch_spi_queue_ss(myinterface, mode);

	start_time_all();
	ch_spi_batch_shift(myinterface,0,NULL);
	start_time_proc();

    return MXQ_OK;

#elif (PC_INTERFACE==FTDI4222)

    FT4222_STATUS  ft4222Status;
    int            success = 1;
    uint16         bytesTransceived;
    uint8          response[len];

    start_time_all();

    ft4222Status = FT4222_SPIMaster_SingleReadWrite(
                        ftHandle, 
                        response, 
                        (mxq_u1*)src, 
                        len, 
                        &bytesTransceived,
                        TRUE); // de-assert slave-select afterwards
    if (FT4222_OK != ft4222Status)
    {
        MXQ_PRINTF("FT4222_SPIMaster_SingleReadWrite failed (error %d)!\n",
               ft4222Status);
        success = 0;
        goto exit;
    }

    if (bytesTransceived != len)
    {
        MXQ_PRINTF("FT4222_SPIMaster_SingleReadWrite "
               "transceived %u bytes instead of %u.\n",
               bytesTransceived,
               len);
        success = 0;
        goto exit;
    }

    exit:
        start_time_proc();
        return (success==1)?MXQ_OK:MXQ_ERR_TRANS_ERROR;

#endif

}


extern mxq_mode_t g_selectedmode;


static mxq_err_t HOST_exchange_bytes_spi_internal(const mxq_u1* src, mxq_length len, mxq_u1* dest, mxq_u1 stop_time)
{

#if (PC_INTERFACE==AARDVARK)
	
	len = aa_spi_write(myinterface, len, src, len, dest);
	if (stop_time) {
		stop_time_all();
	}

	//DBG_COMM_PRINT_BUF(src,dest,len);

	return len;

#elif (PC_INTERFACE==CHEETAH)

    ch_spi_queue_clear(myinterface);

    ch_spi_queue_ss(myinterface, mode|SS1);

    unsigned int i;
    for(i=0;i<len;i++) 
    {
    	ch_spi_queue_byte(myinterface, 1, src[i]);
    	if(ibdelay) ch_spi_queue_delay_ns(myinterface, ibdelay*100UL);
    }

    ch_spi_queue_ss(myinterface, mode);

	ch_spi_batch_shift(myinterface,len,dest);

	if (stop_time) {
		stop_time_all();
	}

	//DBG_COMM_PRINT_BUF(src,dest,len);

	return len;

#elif (PC_INTERFACE==FTDI4222)

	FT4222_STATUS  ft4222Status;
 //   int            success = 1;
    uint16         bytesTransceived;

    ft4222Status = FT4222_SPIMaster_SingleReadWrite(
                        ftHandle, 
                        dest, 
                        (mxq_u1*)src, 
                        len, 
                        &bytesTransceived,
                        TRUE); // de-assert slave-select afterwards
    if (FT4222_OK != ft4222Status)
    {
        MXQ_PRINTF("FT4222_SPIMaster_SingleReadWrite failed (error %d)!\n",
               ft4222Status);
   //     success = 0;
        goto exit;
    }

    if (bytesTransceived != len)
    {
        MXQ_PRINTF("FT4222_SPIMaster_SingleReadWrite "
               "transceived %u bytes instead of %u.\n",
               bytesTransceived,
               len);
      //  success = 0;
        goto exit;
    }

	exit:
		if (stop_time) {
			stop_time_all();
		}
		return len;

#endif

}


mxq_err_t HOST_exchange_bytes_spi(const mxq_u1* src, mxq_length len, mxq_u1* dest)
{
	return HOST_exchange_bytes_spi_internal(src, len, dest, 1);
}


mxq_err_t HOST_receive_bytes_spi_first(mxq_u1* dest, mxq_length len)
{
	mxq_err_t ret;
	mxq_u4 timeout_ms = TIMEOUT;
	mxq_u1 *dummy;
	mxq_u1 pollbyte[]={0xCC};
	memset(dest,0,len);

	dummy = (mxq_u1 *)malloc(len);
	if (dummy == NULL)
	{
		return 0;
	}
	memset(dummy, 0xCC, len);

#if (PC_INTERFACE==AARDVARK)

	/* Send CC until we receive 55 */
	while((dest[0]!=0x55) && (timeout_ms))
	{
		aa_spi_write(myinterface, 1, pollbyte, 1, dest);
		HOST_wait_usec(1000); /* Wait 1 msec */
		timeout_ms--;
	}

#elif (PC_INTERFACE==CHEETAH)

    ch_spi_queue_clear(myinterface);

    ch_spi_queue_ss(myinterface, mode|SS1);

    ch_spi_queue_array(myinterface, 1, pollbyte);

    ch_spi_queue_ss(myinterface, mode);

	/* Send CC until we receive 55 */
	while((dest[0]!=0x55) && (timeout_ms))
	{
		ch_spi_batch_shift(myinterface,1,dest);

		HOST_wait_usec(1000); /* Wait 1 msec */
		timeout_ms--;
	}

#elif (PC_INTERFACE==FTDI4222)

	FT4222_STATUS  ft4222Status;
    uint16         bytesTransceived;
	HOST_wait_miliseconds(1); /* Wait 1 msec */

	while((dest[0]!=0x55) 
		&& (timeout_ms))
	{
	    ft4222Status = FT4222_SPIMaster_SingleReadWrite(
	                        ftHandle, 
	                        dest, 
	                        pollbyte, 
	                        sizeof(pollbyte), 
	                        &bytesTransceived,
	                        TRUE); // de-assert slave-select afterwards
	    if (FT4222_OK != ft4222Status)
	    {
	        MXQ_PRINTF("FT4222_SPIMaster_SingleReadWrite failed (error %d)!\n",
	               ft4222Status);
	        goto exit;
	    }

	    if (bytesTransceived != sizeof(pollbyte))
	    {
	        MXQ_PRINTF("FT4222_SPIMaster_SingleReadWrite "
	               "transceived %d bytes instead of %d.\n",
	               (int)bytesTransceived,
	               (int)sizeof(pollbyte));
	        goto exit;
	    }
		HOST_wait_miliseconds(1); /* Wait 1 msec */
		timeout_ms--;
	}

	goto noexit;

    exit:
        stop_time_proc();
        stop_time_all();
        return MXQ_ERR_TRANS_ERROR;
    noexit:

#endif

	stop_time_proc();

	if(timeout_ms==0){
		stop_time_all();
		ret = MXQ_ERR_TRANS_TIMEOUT;
		goto error;
	}

	dest++;
	len--;

	ret = HOST_exchange_bytes_spi_internal(dummy, len, dest, 0) + 1;
	free(dummy);

error:
	return ret;

}


mxq_err_t HOST_receive_bytes_spi(mxq_u1* dest, mxq_length len)
{
    mxq_err_t length;
	mxq_u1 * dummy = (mxq_u1 *)malloc(len);	
    if( dummy == NULL)
    {
        return MXQ_OK;
    }
	memset(dummy,0xCC,len);
	length = HOST_exchange_bytes_spi_internal(dummy, len, dest, 1);
	free(dummy);
    return length;
}


mxq_err_t HOST_activate_tls(void)
{
#if defined(MAXQ106x)
	// Warning: This is for breakout board wiring
#if (PC_INTERFACE==AARDVARK)
    mode &= ~AA_PIN_MODE;  // SDA -> MAXQ106x mode = 0
	
#elif (PC_INTERFACE==CHEETAH)
	mode |= SS3; // SS3 = 0 -> MAXQ106x Mode =0
#elif (PC_INTERFACE==FTDI4222)
	if(FT4222_OK!=FT4222_GPIO_Write(ftHandleGPIO, GPIO_PORT0, 0)) exit (-4); // REMOTE_MODE
#endif

	HOST_reset_maxq106x();
#endif
	return MXQ_OK;

}

mxq_err_t HOST_follow_tls(void)
{
#if defined(MAXQ106x)
	// Warning: This is for breakout board wiring
#if (PC_INTERFACE==AARDVARK)
    mode &= ~AA_PIN_MODE;  // SDA -> MAXQ106x mode = 0
	
#elif (PC_INTERFACE==CHEETAH)
	mode |= SS3; // SS3 = 0 -> MAXQ106x Mode =0
#elif (PC_INTERFACE==FTDI4222)
	if(FT4222_OK!=FT4222_GPIO_Write(ftHandleGPIO, GPIO_PORT0, 0)) exit (-4); // REMOTE_MODE
#endif
#endif
	return MXQ_OK;
}


mxq_err_t HOST_activate_gcu(void)
{
#if defined(MAXQ106x)
	// Warning: This is for breakout board wiring
#if (PC_INTERFACE==AARDVARK)
    mode |= AA_PIN_MODE; 
	 
#elif (PC_INTERFACE==CHEETAH)
	mode &= ~SS3; // SS3 = 1 -> MAXQ106x Mode =1
#elif (PC_INTERFACE==FTDI4222)
	if(FT4222_OK!=FT4222_GPIO_Write(ftHandleGPIO, GPIO_PORT0, 1)) exit (-4); // REMOTE_MODE
#endif

	HOST_reset_maxq106x();

#if (PC_INTERFACE==CHEETAH)
	ch_spi_bitrate(myinterface, 20000); ibdelay=0;
#endif
#endif	
	return MXQ_OK;
}

mxq_err_t HOST_follow_gcu(void)
{
#if defined(MAXQ106x)
	// Warning: This is for breakout board wiring
#if (PC_INTERFACE==AARDVARK)
    mode |= AA_PIN_MODE; 
	 
#elif (PC_INTERFACE==CHEETAH)
	mode &= ~SS3; // SS3 = 1 -> MAXQ106x Mode =1
#elif (PC_INTERFACE==FTDI4222)
	if(FT4222_OK!=FT4222_GPIO_Write(ftHandleGPIO, GPIO_PORT0, 1)) exit (-4); // REMOTE_MODE
#endif

	HOST_wait_usec(TIME_WAIT_RESET);

#if (PC_INTERFACE==CHEETAH)
	ch_spi_bitrate(myinterface, 20000); ibdelay=0;
#endif
#endif
	return MXQ_OK;
}

#endif // USE_PROTOCOL_SPI


#ifdef USE_PROTOCOL_I2C

#define SLAVE_ADDR 0x60

#if (PC_INTERFACE==AARDVARK)
mxq_err_t HOST_init_i2c(void)
{
	mxq_u2 results[2];

	DBG_PRINT("I2C (Using the AArdvark adapter)\n");

	if(myinterface!=0) aa_close(myinterface);

	if(aa_find_devices( sizeof(results)/sizeof(mxq_u2), results )<=0)
		return MXQ_ERR_TRANS_UNSUPPORTED_INTERFACE;

	myinterface=results[0];

	if((myinterface=aa_open(myinterface))<=0)
		return MXQ_ERR_TRANS_UNSUPPORTED_INTERFACE;

	/* Select I2C mode */
	aa_configure(myinterface,AA_CONFIG_GPIO_I2C);

	/* Select master mode */
	aa_i2c_slave_disable(myinterface);

	/* Set bitrate 400kHz */
	aa_i2c_bitrate(myinterface, 400);

	aa_i2c_free_bus(myinterface);

	/* Activate i2C pullups */
	aa_i2c_pullup(myinterface, AA_I2C_PULLUP_BOTH);

	return MXQ_OK;
}

void* HOST_get_interface(void) {
	return (void*)myinterface;
}

void HOST_set_interface(void* i) {
	myinterface = i;
}


#elif (PC_INTERFACE==FTDI4222)
FT_HANDLE            ftHandle = (FT_HANDLE)NULL;
FT_HANDLE            ftHandleGPIO = (FT_HANDLE)NULL;

void* HOST_get_interface(void) {
	return (void*)ftHandle;
}

void HOST_set_interface(void* i) {
	ftHandle = i;
}

mxq_err_t HOST_init_i2c(void)
{
	DBG_PRINT("I2C (Using the FTDI4222 adapter)\n");

	/*** ListFtUsbDevices ***/
	FT_STATUS ftStatus = 0;
	DWORD numOfDevices = 0;
	DWORD ftdiDevNumA = 0, ftdiDevNumB = 1, ftdiDevFound = 0;

	if (ftHandle != (FT_HANDLE)NULL)
	{
		FT4222_UnInitialize(ftHandle);
		FT_Close(ftHandle);
		ftHandle = (FT_HANDLE)NULL;
	}

	if (ftHandleGPIO != (FT_HANDLE)NULL)
	{
		FT4222_UnInitialize(ftHandleGPIO);
		FT_Close(ftHandleGPIO);
		ftHandleGPIO = (FT_HANDLE)NULL;
	}

	ftStatus = FT_CreateDeviceInfoList(&numOfDevices);
	for(DWORD iDev=0; iDev<numOfDevices; ++iDev)
	{
		FT_DEVICE_LIST_INFO_NODE devInfo;
		memset(&devInfo, 0, sizeof(devInfo));

		ftStatus = FT_GetDeviceInfoDetail(iDev, &devInfo.Flags, &devInfo.Type,
							&devInfo.ID, &devInfo.LocId,
							devInfo.SerialNumber,
							devInfo.Description,
							&devInfo.ftHandle);

		if (FT4222_OK == ftStatus)
		{
			if ( !(strcmp(devInfo.Description, "FT4222 A")) ) {
				ftdiDevNumA = iDev;
				ftdiDevFound++;
				DBG_PRINT("FTDI FT4222 A found\n");
			}

			if ( !(strcmp(devInfo.Description, "FT4222 B")) ) {
				ftdiDevNumB = iDev;
				ftdiDevFound++;
				DBG_PRINT("FTDI FT4222 B found\n");
			}
		}
	}

	if (ftdiDevFound < 2)
	{
		MXQ_PRINTF ("No FTDI FT4222 found\n");
		goto i2c_ftdi_exit;
	}

	/*** Open GPIO ***/
	FT4222_STATUS		ft4222Status;
	FT4222_Version		ft4222Version;

	ftStatus = FT_Open(ftdiDevNumB, &ftHandleGPIO);
	if (FT4222_OK != ftStatus)
	{
		MXQ_PRINTF("FT_Open failed (error %d)\n", (int)ftStatus);
		goto i2c_ftdi_exit;
	}

#if defined(MAXQ1065)
	GPIO_Dir gpioDir[4];
	gpioDir[0] = GPIO_INPUT;   // SCL
	gpioDir[1] = GPIO_INPUT;   // SDA
	gpioDir[2] = GPIO_OUTPUT;  // PDWN_N
	gpioDir[3] = GPIO_INPUT;   // RDY_N

	if(FT4222_OK!=FT4222_GPIO_Init(ftHandleGPIO, gpioDir)) {
		MXQ_PRINTF("FT4222_GPIO_Init failed (error)\n");
		exit (-1);
	}
	//disable suspend out , enable gpio 2
	if(FT4222_OK!=FT4222_SetSuspendOut(ftHandleGPIO, 0)) {
		MXQ_PRINTF("FT4222_SetSuspendOut failed (error)\n");
		exit (-2);
	}
	//disable interrupt , enable gpio 3
	if(FT4222_OK!=FT4222_SetWakeUpInterrupt(ftHandleGPIO, 0)) {
		MXQ_PRINTF("FT4222_SetWakeUpInterrupt failed (error)\n");
		exit (-3);
	}

	ftStatus = FT4222_GPIO_Write(ftHandleGPIO, GPIO_PORT2, 1);
	if(FT4222_OK!=ftStatus) { // PDWN_N
		MXQ_PRINTF("FT4222_GPIO_Write (GPIO_PORT2, 1) failed (error %d)\n", (unsigned int)ftStatus);
		exit (-6);
	}

	MXQ_PRINTF("\nWaiting for power-up...\n");
	HOST_wait_miliseconds(250UL);
#elif defined(MAXQ108x)
	GPIO_Dir gpioDir[4];
	gpioDir[0] = GPIO_INPUT;    // I2C_SCL
	gpioDir[1] = GPIO_INPUT;    // I2C_SDA
	gpioDir[2] = GPIO_INPUT;    // SECI/O_2 (DNI)
	gpioDir[3] = GPIO_INPUT;    // RDY#

	if(FT4222_OK!=FT4222_GPIO_Init(ftHandleGPIO, gpioDir)) {
		MXQ_PRINTF("FT4222_GPIO_Init failed (error)\n");
		exit (-1);
	}
	//disable suspend out , enable gpio 2
	if(FT4222_OK!=FT4222_SetSuspendOut(ftHandleGPIO, 0)) {
		MXQ_PRINTF("FT4222_SetSuspendOut failed (error)\n");
		exit (-2);
	}
	//disable interrupt , enable gpio 3
	if(FT4222_OK!=FT4222_SetWakeUpInterrupt(ftHandleGPIO, 0)) {
		MXQ_PRINTF("FT4222_SetWakeUpInterrupt failed (error)\n");
		exit (-3);
	}
#else
	GPIO_Dir gpioDir[4];
	gpioDir[0] = GPIO_OUTPUT;
	gpioDir[1] = GPIO_OUTPUT;
	gpioDir[2] = GPIO_OUTPUT;
	gpioDir[3] = GPIO_OUTPUT;

	if(FT4222_OK!=FT4222_GPIO_Init(ftHandleGPIO, gpioDir)) exit (-1);
	//disable suspend out , enable gpio 2
	if(FT4222_OK!=FT4222_SetSuspendOut(ftHandleGPIO, 0)) exit (-2);
	//disable interrupt , enable gpio 3
	if(FT4222_OK!=FT4222_SetWakeUpInterrupt(ftHandleGPIO, 0)) exit (-3);

	// set gpio0/gpio1/gpio2/gpio3 output level high
//	if(FT4222_OK!=FT4222_GPIO_Write(ftHandleGPIO, GPIO_PORT0, 1)) exit (-4); // REMOTE_MODE
	if(FT4222_OK!=FT4222_GPIO_Write(ftHandleGPIO, GPIO_PORT1, 1)) exit (-5); // WAKEUP_N
	if(FT4222_OK!=FT4222_GPIO_Write(ftHandleGPIO, GPIO_PORT2, 1)) exit (-6); // Reset_N
	if(FT4222_OK!=FT4222_GPIO_Write(ftHandleGPIO, GPIO_PORT3, 0)) exit (-7); // AUX LED
#endif

	/*** Open I2C ***/
	ftStatus = FT_Open(ftdiDevNumA, &ftHandle);
	if (FT4222_OK != ftStatus)
	{
		MXQ_PRINTF("Open a FT4222 device failed!\n");
		goto i2c_ftdi_exit;
	}

	ft4222Status = FT4222_GetVersion(ftHandle, &ft4222Version);
	if (FT4222_OK != ft4222Status)
	{
		MXQ_PRINTF("FT4222_GetVersion failed (error %d)\n", (int)ft4222Status);
		goto i2c_ftdi_exit;
	}

	MXQ_PRINTF("FTDI Chip version: %08X, LibFT4222 version: %08X\n",
				(unsigned int)ft4222Version.chipVersion,
				(unsigned int)ft4222Version.dllVersion);

	/*** Init ***/
	// Set bitrate 400kHz
	ftStatus = FT4222_I2CMaster_Init(ftHandle, 400);
	if (FT4222_OK != ftStatus)
	{
		MXQ_PRINTF("Init FT4222 as I2C master device failed!\n");
		goto i2c_ftdi_exit;
	}
	return MXQ_OK;

i2c_ftdi_exit:
	if (ftHandle != (FT_HANDLE)NULL)
	{
		(void)FT4222_UnInitialize(ftHandle);
		(void)FT_Close(ftHandle);
	}
	return MXQ_ERR_TRANS_UNSUPPORTED_INTERFACE;
}
#endif

mxq_err_t HOST_send_bytes_i2c(const mxq_u1* src, mxq_u2 len)
{
#if (PC_INTERFACE==AARDVARK)
	int errorcode;
	mxq_u2 numwritten=0;

	start_time_all();

	if ((errorcode=aa_i2c_write_ext(myinterface, SLAVE_ADDR, AA_I2C_NO_FLAGS, len, src, &numwritten)) != AA_I2C_STATUS_OK)
	{
		stop_time_all();
		DBG_PRINT("I2C error %x \n",errorcode);
		return MXQ_ERR_TRANS_ERROR;
	}

	numwritten&=0x0000FFFF;

	if(numwritten!=len)
	{
		stop_time_all();
		DBG_PRINT("I2C error: only %d/%d bytes written.\n",numwritten,len);
		aa_i2c_free_bus(myinterface);
		return MXQ_ERR_TRANS_ERROR;
	}

	start_time_proc();
	return len;

#elif (PC_INTERFACE==FTDI4222)
	FT_STATUS ftStatus;
	mxq_u2 numwritten = 0;
	mxq_u1 controllerStatus = 0;
	mxq_u4 bus_timeout_ms;

	start_time_all();

send_bytes_i2c_start:
	ftStatus = FT4222_I2CMaster_Write(ftHandle, SLAVE_ADDR, (unsigned char *)src, len, &numwritten);
	if (FT4222_OK != ftStatus)
	{
		DBG_PRINT("# [HOST_I2C] send_bytes (fail): error 0x%X\n", (unsigned int)ftStatus);
		goto send_bytes_i2c_error;
	}

	if (numwritten != len)
	{
		DBG_PRINT("# [HOST_I2C] send_bytes (fail): only %d/%d bytes written\n", numwritten, len);
		goto send_bytes_i2c_error;
	}

	// wait for successful transmission
	bus_timeout_ms = TIMEOUT;
	while (bus_timeout_ms)
	{
		ftStatus = FT4222_I2CMaster_GetStatus(ftHandle, &controllerStatus);
		if (FT4222_OK != ftStatus) {
			DBG_PRINT("# [HOST_I2C] send_bytes (fail): ftStatus 0x%X controllerStatus 0x%X\n", (unsigned int)ftStatus, controllerStatus);
			break; // live with the result
		}

		// check if error condition exists
		if ( (controllerStatus & 0x02) != 0 )
		{
			DBG_PRINT("# [HOST_I2C] send_bytes (continue) (info): ftStatus 0x%X controllerStatus 0x%X\n", (unsigned int)ftStatus, controllerStatus);
			FT4222_I2CMaster_Reset(ftHandle);
			HOST_wait_usec(10000UL); //HOST_wait_msec(10);
			goto send_bytes_i2c_start;
		}

		// check if the controller is idle
		if ( controllerStatus == 0x20 )
		{
			break;
		}

		bus_timeout_ms--;
		HOST_wait_usec(1000UL); //HOST_wait_msec(1);
	}

	if (bus_timeout_ms == 0)
	{
		DBG_PRINT("# [HOST_I2C] send_bytes (fail) (wait tx timeout): ftStatus 0x%X controllerStatus 0x%X\n", (unsigned int)ftStatus, controllerStatus);
		goto send_bytes_i2c_error;
	}

	start_time_proc();
	return len;

send_bytes_i2c_error:
	stop_time_all();
	return MXQ_ERR_TRANS_ERROR;
#endif
}


mxq_err_t HOST_receive_bytes_i2c_first(mxq_u1* dest, mxq_u2 len)
{
#if (PC_INTERFACE==AARDVARK)
	int errorcode;
	mxq_u2 numread=0;
	mxq_u4 timeout_ms = TIMEOUT;
	dest[0]=0;

	/* Poll bus until we receive something */
	while((dest[0]!=0x55) && (timeout_ms))
	{
		timeout_ms--;
		if ((errorcode=aa_i2c_read_ext(myinterface, SLAVE_ADDR, AA_I2C_NO_FLAGS, len, dest, &numread)) != AA_I2C_STATUS_OK)
		{
			/* if(errorcode==AA_I2C_STATUS_BUS_LOCKED) continue;
			 * aa_i2c_free_bus(myinterface);
			 *DBG_PRINT("system_receive_bytes_i2c_first: error %d\n",errorcode);
			 * return MXQ_ERR_TRANS_ERROR;
			 */
			aa_sleep_ms(10);
			continue;
		}

	}

	stop_time_proc();

	if(timeout_ms==0) {
		stop_time_all();
		return MXQ_ERR_TRANS_ERROR;
	}

	return numread;

#elif (PC_INTERFACE==FTDI4222)
	FT_STATUS ftStatus;
	mxq_u2 numread = 0;
	mxq_u4 timeout_ms = TIMEOUT;
	dest[0] = 0;

	// Poll bus until we receive something
	while((dest[0]!=0x55) && (timeout_ms))
	{
		HOST_wait_miliseconds(1);
		ftStatus = FT4222_I2CMaster_Read(ftHandle, SLAVE_ADDR, dest, len, &numread);
		if (FT4222_OK != ftStatus)
		{
			DBG_PRINT("# [HOST_I2C] receive_bytes_first (fail): ftStatus 0x%X\n", (unsigned int)ftStatus);
			timeout_ms = 0;
			break;
		}
		timeout_ms--;
	}

	stop_time_proc();

	if(timeout_ms==0) {
		stop_time_all();
		return MXQ_ERR_TRANS_ERROR;
	}

	return numread;
#endif
}


mxq_err_t HOST_receive_bytes_i2c(mxq_u1* dest, mxq_u2 len)
{
#if (PC_INTERFACE==AARDVARK)
	int errorcode;
	mxq_u2 numread=0;

	if ((errorcode=aa_i2c_read_ext(myinterface, SLAVE_ADDR, AA_I2C_NO_FLAGS, len, dest, &numread)) != AA_I2C_STATUS_OK)
	{
		aa_i2c_free_bus(myinterface);
		DBG_PRINT("HOST_receive_bytes_i2c_first: error %d\n",errorcode);
		return MXQ_ERR_TRANS_ERROR;
	}

	stop_time_all();
	return numread;

#elif (PC_INTERFACE==FTDI4222)
	FT_STATUS ftStatus;
	mxq_u1 controllerStatus = 0;
	mxq_u2 numread = 0;
	mxq_u4 timeout_ms = TIMEOUT;

	while(timeout_ms)
	{
		timeout_ms--;
		ftStatus = FT4222_I2CMaster_Read(ftHandle, SLAVE_ADDR, dest, len, &numread);
		if (FT4222_OK != ftStatus)
		{
			DBG_PRINT("# [HOST_I2C] receive_bytes (fail): ftStatus 0x%X\n", (unsigned int)ftStatus);
			numread = 0;
			goto receive_bytes_i2c_error;
		}

		ftStatus = FT4222_I2CMaster_GetStatus(ftHandle, &controllerStatus);
		if (FT4222_OK != ftStatus) {
			DBG_PRINT("# [HOST_I2C] receive_bytes (fail): ftStatus 0x%X controllerStatus 0x%X\n", (unsigned int)ftStatus, controllerStatus);
			break; // live with the result
		}

		// check if error condition exists
		if ( (controllerStatus & 0x02) != 0 )
		{
			DBG_PRINT("# [HOST_I2C] receive_bytes (continue) (info): ftStatus 0x%X controllerStatus 0x%X\n", (unsigned int)ftStatus, controllerStatus);
			HOST_wait_usec(1000UL);	//HOST_wait_msec(1);
			continue;
		}

		// done
		break;
	}

receive_bytes_i2c_error:
	stop_time_all();
	return numread;
#endif
}

mxq_err_t HOST_activate_tls(void)
{
	return MXQ_OK;
}

mxq_err_t HOST_follow_tls(void)
{
	return MXQ_OK;
}

mxq_err_t HOST_activate_gcu(void)
{
	return MXQ_OK;
}

mxq_err_t HOST_follow_gcu(void)
{
	return MXQ_OK;
}

#endif // USE_PROTOCOL_I2C

mxq_err_t HOST_deinit_interface()
{
#if (PC_INTERFACE==FTDI4222)
	if (ftHandle != (FT_HANDLE)NULL)
	{
		(void)FT4222_UnInitialize(ftHandle);
		(void)FT_Close(ftHandle);
	}

	if (ftHandleGPIO != (FT_HANDLE)NULL)
	{
		(void)FT4222_UnInitialize(ftHandleGPIO);
		(void)FT_Close(ftHandleGPIO);
	}
#endif
	return MXQ_OK;
}


#if (PC_INTERFACE==PIPE)

mxq_err_t HOST_activate_gcu(void) {
	MXQ_PRINT_ERR("Cannot do HOST_activate_gcu\n");
	exit(-1);
}

mxq_err_t HOST_follow_gcu(void) {
	MXQ_PRINT_ERR("Warning: Cannot do HOST_follow_gcu\n");
	//exit(-1);
	return MXQ_OK;
}

mxq_err_t HOST_follow_tls(void) {
	return MXQ_OK;
}

mxq_err_t HOST_activate_tls(void) {
	return MXQ_OK;
}
#endif


mxq_err_t HOST_reset_maxq106x(void) 
{

	// Warning: This is for breakout board wiring
	// Low pulse on SCL -> RESET_N
#if (PC_INTERFACE==AARDVARK)
	aa_gpio_set(myinterface, mode&(~AA_PIN_RESET)); // assert SCL -> reset_n
	HOST_wait_usec(1000UL);
	aa_gpio_set(myinterface, mode);

#elif (PC_INTERFACE==CHEETAH)
    ch_spi_queue_clear(myinterface);
    ch_spi_queue_ss(myinterface, mode|SS2); // assert ss2 -> reset_n 
    ch_spi_queue_delay_ns(myinterface,10000000UL);
    ch_spi_queue_ss(myinterface, mode); // assert ss depending on current mode 
	ch_spi_batch_shift(myinterface,0,NULL);

#elif (PC_INTERFACE==PIPE)
	//TODO: send reset signal through pipe

#elif (PC_INTERFACE==FTDI4222)
	#if defined(MAXQ1065)
		if(FT4222_OK!=FT4222_GPIO_Write(ftHandleGPIO, GPIO_PORT2, 0)) exit (-4);
		HOST_wait_usec(1000UL);
		if(FT4222_OK!=FT4222_GPIO_Write(ftHandleGPIO, GPIO_PORT2, 1)) exit (-4);
	#elif defined(MAXQ106x)
		if(FT4222_OK!=FT4222_GPIO_Write(ftHandleGPIO, GPIO_PORT1, 0)) exit (-4);
		HOST_wait_usec(1000UL);
		if(FT4222_OK!=FT4222_GPIO_Write(ftHandleGPIO, GPIO_PORT1, 1)) exit (-4);
	#endif

#endif

	HOST_wait_usec(TIME_WAIT_RESET);
	
	return MXQ_OK;
}

mxq_err_t HOST_wakeup_maxq106x(void) 
{

#if defined(MAXQ1065)
	#if (PC_INTERFACE==FTDI4222)
		if(FT4222_OK!=FT4222_GPIO_Write(ftHandleGPIO, GPIO_PORT2, 1)) exit (-4);
		return MXQ_OK;
	#endif
#elif defined(MAXQ106x)
	#if (PC_INTERFACE==FTDI4222)
		if(FT4222_OK!=FT4222_GPIO_Write(ftHandleGPIO, GPIO_PORT1, 1)) exit (-4);
		return MXQ_OK;
	#endif
#elif defined(MAXQ108x)
	return MXQ_OK;
#endif

	return MXQ_ERR_INTERNAL_ERROR;

}

mxq_err_t HOST_sleep_maxq10xx(void) {

#if defined(MAXQ1065)
	#if (PC_INTERFACE==FTDI4222)
		if(FT4222_OK!=FT4222_GPIO_Write(ftHandleGPIO, GPIO_PORT2, 0)) exit (-4);
		return MXQ_OK;
	#endif
#elif defined(MAXQ106x)
	#if (PC_INTERFACE==FTDI4222)
		if(FT4222_OK!=FT4222_GPIO_Write(ftHandleGPIO, GPIO_PORT1, 0)) exit (-4);
		return MXQ_OK;
	#endif
#elif defined(MAXQ108x)
	return MXQ_OK;
#endif

	return MXQ_ERR_INTERNAL_ERROR;
}

