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
 * @date Apr 2015
 * @authors Stéphane Di Vito, Maxim Integrated Products Inc.
 * @authors Benjamin Vinot, Maxim Integrated Products Inc.
 *
 * @brief System driver for the MAXQ106x's Host library (Embedded Linux Version)
 * @details
 * @ingroup drivers
 * @internal
 */
#define _HOST_EMBEDDED_LINUX_C

#include "MXQ_Internal.h"

#ifndef __linux
#error Something is wrong
#endif
#define _XOPEN_SOURCE

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <linux/i2c-dev.h>
#include <sys/stat.h>
#include <sys/types.h>

#if defined(MAXQ1065_BENCHMARK) || defined(MAXQ108X_BENCHMARK)
#include <poll.h>
#include <pthread.h>
#include <time.h>
#endif


extern mxq_mode_t g_selectedmode;
int commspeed,ibdelay; // not used (yet)

/* The below may require to type "mknod /dev/spi2.0 c 153 0" on the board console */
/* Also make sure to disable the raspberry pi 1-wire to free the GPIO 4 */

#define IN  0
#define OUT 1
 
#define LOW  0
#define HIGH 1


#if defined(MAXQ1065)
    #define RDY_N		    4
    #define PIN_AUX_LED		17
    #define PIN_RESET_OUT	23
    #define PIN_WDI	        24
    #define PIN_PW_DOWN		25
    #define TIME_WAIT_RESET		500000
#elif defined(MAXQ108x)
    #define RDY_N		    4
	#define SECIO1		    17 // at 1st evkit rev = 23
	#define SECIO2		    27 // at 1st evkit rev = 24
	#define TIME_WAIT_RESET		60000
#else
    #define PIN_RST		25
    #define PIN_MODE	4
    #define PIN_WKP		24
    #define TIME_WAIT_RESET		60000
#endif


#define BUFFER_MAX 3
#define DIRECTION_MAX 35

#define PIN  24 /* P1-18 */
#define POUT 4  /* P1-07 */
 
PRIVATE int GPIOExport(int pin)
{
	char buffer[BUFFER_MAX];
	ssize_t bytes_written;
	int fd;

	fd = open("/sys/class/gpio/export", O_WRONLY);
	    
	if ( fd < 0) {
	    MXQ_PRINT_ERR("Failed to open /sys/class/gpio/export for writing!\n");
	    return(errno);
	}
 
	bytes_written = snprintf(buffer, BUFFER_MAX, "%d", pin);
	write(fd, buffer, bytes_written);
	close(fd);

	// Avoid race condition due to udev rules taking time to be established
	usleep(100000);
	
	return(0);
}

#ifdef UNSED_GPIO_FUNC

PRIVATE int GPIOUnexport(int pin)
{
	char buffer[BUFFER_MAX];
	ssize_t bytes_written;
	int fd;
 
	fd = open("/sys/class/gpio/unexport",O_WRONLY);

	if ( fd < 0 ) {
		MXQ_PRINT_ERR ("Failed to open /sys/class/gpio/unexport for writing!\n");
		return(-1);
	}
 
	bytes_written = snprintf(buffer, BUFFER_MAX, "%d", pin);
	write(fd, buffer, bytes_written);
	close(fd);

	return(0);
}


PRIVATE int GPIORead(int pin)
{

	char path[DIRECTION_MAX];
	char value_str[3];
	int fd;
 
	snprintf(path, DIRECTION_MAX, "/sys/class/gpio/gpio%d/value", pin);
	fd = open(path, O_RDONLY);

	if (-1 == fd) {
		MXQ_PRINT_ERR("Failed to open gpio value for reading!\n");
		return(-1);
	}
 
	if (-1 == read(fd, value_str, 3)) {
		MXQ_PRINT_ERR("Failed to read value!\n");
		return(-1);
	}
 
	close(fd);
 
	return(atoi(value_str));
}
 
 #endif
 
PRIVATE int GPIODirection(int pin, int dir)
{
	static const char s_directions_str[]  = "in\0out";
	char path[DIRECTION_MAX];
	int fd;
 
	snprintf(path, DIRECTION_MAX, "/sys/class/gpio/gpio%d/direction", pin);
	fd = open(path, O_RDWR);

	if (-1 == fd) {
		MXQ_PRINT_ERR ("Failed to open /sys/class/gpio/gpio%d/direction for writing!\n", pin);
		return(-1);
	}

	read(fd,path,1);    

    	if(path[0]!= (IN == dir ? 'i' : 'o')){
		if (-1 == write(fd, &s_directions_str[IN == dir ? 0 : 3], IN == dir ? 2 : 3)) {
			MXQ_PRINT_ERR ("Failed to set direction!\n");
			return(-1);
	    	}
    	}

	close(fd);
	return(0);
}

#ifndef MAXQ108x // unused function warning
PRIVATE int GPIOWrite(int pin, int value)
{
	static const char s_values_str[] = "01";
 
	char path[DIRECTION_MAX];
	int fd;
 
	snprintf(path, DIRECTION_MAX, "/sys/class/gpio/gpio%d/value", pin);
	fd = open(path, O_RDWR);

	if (-1 == fd) {
		MXQ_PRINT_ERR ("Failed to open /sys/class/gpio/gpio%d/value for writing!\n", pin);
		return(-1);
	}
    
    	read(fd,path,1);

    	if(path[0] != s_values_str[value]) {

	    	DBG_PRINT("GPIOWrite path: %x   strval:  %x \n",path[0],s_values_str[value]);

		if (1 != write(fd, &s_values_str[LOW == value ? 0 : 1], 1)) {
			MXQ_PRINT_ERR( "Failed to write value!\n");
			return(-1);
		}

	}

	close(fd);
	return(0);
}
#endif

#if defined(MAXQ1065_BENCHMARK) || defined(MAXQ108X_BENCHMARK)
PRIVATE int GPIO_SetEdge(int pin, char *edge)
{
	int fd;
	char buf[64];

	snprintf(buf, sizeof(buf), "/sys/class/gpio/gpio%d/edge", pin);

	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		MXQ_PRINT_ERR("Failed to open /sys/class/gpio/gpio%d/edge for GPIO_SetEdge!\n", pin);
		return fd;
	}

	write(fd, edge, strlen(edge) + 1);
	close(fd);
	return 0;
}

PRIVATE int GPIO_Open(int pin)
{
	int fd;
	char buf[64];

	snprintf(buf, sizeof(buf), "/sys/class/gpio/gpio%d/value", pin);

	fd = open(buf, O_RDONLY | O_NONBLOCK );
	if (fd < 0) {
		MXQ_PRINT_ERR("Failed to open /sys/class/gpio/gpio%d/value for GPIO_Open!\n", pin);
	}
	return fd;
}

PRIVATE int GPIO_Close(int fd)
{
	return close(fd);
}
#endif

static void init_pins(void)
{
#if defined(MAXQ1065)
	GPIOExport(RDY_N);
    GPIOExport(PIN_AUX_LED);
    GPIOExport(PIN_RESET_OUT);
    GPIOExport(PIN_WDI);
    GPIOExport(PIN_PW_DOWN);
    GPIODirection(RDY_N,IN);
    GPIODirection(PIN_AUX_LED,OUT);
    GPIODirection(PIN_RESET_OUT,IN);
    GPIODirection(PIN_WDI,IN);
    GPIODirection(PIN_PW_DOWN,OUT);
    GPIOWrite(PIN_AUX_LED,HIGH);
    GPIOWrite(PIN_PW_DOWN,HIGH);
#elif defined(MAXQ108x)
	GPIOExport(RDY_N);
	GPIOExport(SECIO1);
	GPIOExport(SECIO2);
	GPIODirection(RDY_N,IN);
	GPIODirection(SECIO1,IN);
	GPIODirection(SECIO2,IN);
#else
    GPIOExport(PIN_RST);
	GPIOExport(PIN_MODE);
	GPIOExport(PIN_WKP);
	GPIODirection(PIN_RST,OUT);
	GPIODirection(PIN_MODE,OUT);
	GPIODirection(PIN_WKP,OUT);
	GPIOWrite(PIN_RST,HIGH);
    GPIOWrite(PIN_WKP,HIGH);
#endif
    usleep(TIME_WAIT_RESET);
}

#if defined(MAXQ1065_BENCHMARK) || defined(MAXQ108X_BENCHMARK)
#if defined(MAXQ108X_BENCHMARK)
#define POLL_TIMEOUT (-1)
#else
#define POLL_TIMEOUT (10 * 1000) //(-1)
#endif
#define POLL_GPIO (POLLPRI | POLLERR)
#define BENCHMARK_MAX_BUF 64
#define BENCHMARK_RESULT_PATH "benchmark_last_cmd.result"

static struct timespec tp_start, tp_end;
static float benchmark_ms;

int benchmark_init(void)
{
	struct pollfd fdset;
	int rdy_gpio_fd;
	char *buf[BENCHMARK_MAX_BUF];

	GPIO_SetEdge(RDY_N, "both");
	rdy_gpio_fd = GPIO_Open(RDY_N);

	memset((void*)&fdset, 0, sizeof(fdset));
	fdset.fd = rdy_gpio_fd;
	fdset.events = POLL_GPIO;

	// consume any prior interrupt
	lseek(fdset.fd, 0, SEEK_SET);
	read(fdset.fd, buf, BENCHMARK_MAX_BUF);

	return rdy_gpio_fd;
}

void *benchmark_poll(void *fd_param)
{
	struct pollfd fdset;
	int rc;
	int gpio_fd = (int)fd_param;
	unsigned int interrupt_cnt = 0;
	char *buf[BENCHMARK_MAX_BUF];
	unsigned long long int dur_ns;

	while (1) {
		memset((void*)&fdset, 0, sizeof(fdset));
		fdset.fd = gpio_fd;
		fdset.events = POLL_GPIO;

		rc = poll(&fdset, 1, POLL_TIMEOUT);
		if (rc < 0) {
			MXQ_PRINT_ERR("Failed poll() on GPIO pin...\n");
			goto exit_benchmark_poll;
		}

		if (rc == 0) {
			MXQ_PRINT_ERR("Timeout occurred during poll()...\n");
			goto exit_benchmark_poll;
		}

		if (fdset.revents & POLL_GPIO) {
			lseek(fdset.fd, 0, SEEK_SET);
			read(fdset.fd, buf, BENCHMARK_MAX_BUF);
			interrupt_cnt++;

			if (interrupt_cnt == 1) {
				clock_gettime(CLOCK_MONOTONIC, &tp_start);
			}

			if (interrupt_cnt == 2) {
				clock_gettime(CLOCK_MONOTONIC, &tp_end);
			}
		}

		if (interrupt_cnt >= 2) {
			dur_ns = ((tp_end.tv_sec * 1000000000ULL) + tp_end.tv_nsec) -
					 ((tp_start.tv_sec * 1000000000ULL) + tp_start.tv_nsec);
			benchmark_ms = (float)(dur_ns / 1000000.0);

			MXQ_PRINTF("Duration = %.3fms\n", benchmark_ms);
			break;
		}

		fflush(stdout);
	}

exit_benchmark_poll:
	fflush(stdout);
	GPIO_Close(gpio_fd);
	return NULL;
}

void benchmark_save_result(void)
{
	FILE *f;
	char str[64];
	int write_size;

	memset(str, 0, sizeof(str));
	write_size = snprintf(str, (sizeof(str) - 1), "%.3f", benchmark_ms);
	if (write_size < 0) {
		MXQ_PRINT_ERR("Buffer error for saving benchmark result!\n");
		return;
	}

	// add new line char
	str[write_size] = '\n';
	write_size++;

	if ((f = fopen(BENCHMARK_RESULT_PATH, "ab")) == NULL) {
		MXQ_PRINT_ERR("Failed to open %s for writing!\n", BENCHMARK_RESULT_PATH);
		return;
	}

	// do not include the null char
	if (fwrite(str, 1, write_size, f) != (unsigned int)write_size)
	{
		MXQ_PRINT_ERR("Failed writing to %s with %d bytes!\n", BENCHMARK_RESULT_PATH, write_size);
		fclose(f);
		return;
	}

	fclose(f);
	return;
}

#endif

void* HOST_get_interface(void) {
	return (void*)0;
}

void HOST_set_interface(void* i) {
	(void) i;
}

mxq_err_t HOST_deinit_interface(void)
{
	return MXQ_OK;
}

/******************		SPI		*********************************************/	
#ifdef USE_PROTOCOL_SPI
static const char *spi_device = "/dev/spidev0.0";
static uint8_t mode   = SPI_MODE_0;
static uint8_t bits   = 8;
#if defined(MAXQ1065)
static uint32_t speed = 2000000L;
#else
static uint32_t speed = 1000000L;
#endif
static uint32_t speed_gcu = 20000000L;
#ifdef MAXQ108x
static uint16_t delay = 0;
#else
static uint16_t delay = 1000;
#endif
static uint16_t delay_gcu = 0;
int fd_spi;

static mxq_err_t spi_init(void)
{
	int ret = 0;

	fd_spi = open(spi_device, O_RDWR);
	if (fd_spi < 0) return errno;

	ret = ioctl(fd_spi, SPI_IOC_WR_MODE, &mode);
	if (ret == -1) return errno;

	ret = ioctl(fd_spi, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1) return errno;

	ret = ioctl(fd_spi, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1) return errno;

	return ret;
}

mxq_err_t HOST_init_spi(void)
{
    init_pins();
    return spi_init();
}

mxq_err_t HOST_send_bytes_spi(const mxq_u1* src, mxq_length len)
{
	mxq_err_t ret;
	mxq_u1 rx[len];

#if defined(MAXQ1065_BENCHMARK) || defined(MAXQ108X_BENCHMARK)
	pthread_t bench_thread;
	int rdy_gpio_fd;

	rdy_gpio_fd = benchmark_init();
	pthread_create(&bench_thread, NULL, benchmark_poll, (void *)rdy_gpio_fd);
#endif

	start_time_all();
	ret = HOST_exchange_bytes_spi(src, len, rx);
	start_time_proc();

#if defined(MAXQ1065_BENCHMARK) || defined(MAXQ108X_BENCHMARK)
	pthread_join(bench_thread, NULL);
	benchmark_save_result();
#endif

	return ret;
}

mxq_err_t HOST_exchange_bytes_spi(const mxq_u1* src, mxq_length len, mxq_u1* dest)
{
	int ret;
	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)src,
		.rx_buf = (unsigned long)dest,
		.len = len,
		.delay_usecs = delay,
		.speed_hz = speed,
		.bits_per_word = bits,
		.cs_change = 0, //Previous: 1
	};

   if(g_selectedmode==1) {
	   tr.speed_hz=speed_gcu;
	   tr.delay_usecs=delay_gcu;
   }
   

	ret = ioctl(fd_spi, SPI_IOC_MESSAGE(1), &tr);
    	if (ret == -1) return errno;

//  	DBG_COMM_PRINT_BUF(src,dest,len);
   
	return MXQ_OK;
}




mxq_err_t HOST_receive_bytes_spi_first(mxq_u1* dest, mxq_length len)
{
	int ret;

	dest[0]=0;

	mxq_u1 dummy[1]={0xcc};

	uint32_t timeout=1800000L;

	/* Send bytes until we receive 55 */
#if defined(MAXQ108X_BENCHMARK)
	while(dest[0]!=0x55)
#else
	while((dest[0]!=0x55) && (timeout))
#endif
	{
		struct spi_ioc_transfer tr = {
			.tx_buf = (unsigned long)dummy,
			.rx_buf = (unsigned long)dest,
			.len = 1,
			.delay_usecs = 0,
			.speed_hz = speed,
			.bits_per_word = bits,
			.cs_change = 0,
		};

   if(g_selectedmode==1) {
	   tr.speed_hz=speed_gcu;
	   tr.delay_usecs=delay_gcu;
   }

		ret = ioctl(fd_spi, SPI_IOC_MESSAGE(1), &tr);
		if (ret == -1) return errno;
		timeout--;
	}


	stop_time_proc();

	if(timeout==0) {
		/* spi_deselect_device(SPI_MASTER_BASE,&mq); */
		return MXQ_ERR_TRANS_TIMEOUT;
	}

	dest++;
	len--;

	len = HOST_receive_bytes_spi(dest,len);
	if(len<=0) return len;

	return len+1;
}

mxq_err_t HOST_receive_bytes_spi(mxq_u1* dest, mxq_length len)
{
	int ret;
	mxq_u1 dummy[len];

	memset(dummy,0,sizeof(dummy));

	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)dummy,
		.rx_buf = (unsigned long)dest,
		.len = len,
		.delay_usecs = delay,
		.speed_hz = speed,
		.bits_per_word = bits,
		.cs_change = 0, //Previous: 1
	};


   if(g_selectedmode==1) {
	   tr.speed_hz=speed_gcu;
	   tr.delay_usecs=delay_gcu;
   }


	ret = ioctl(fd_spi, SPI_IOC_MESSAGE(1), &tr);
	stop_time_all();
	if (ret == -1) return errno;

	return len;
}

#endif



/******************		I2C		*********************************************/		
#ifdef USE_PROTOCOL_I2C

/* The below may require to type "mknod /dev/i2c.1 c 153 0" on the board console */
static const char *i2c_device = "/dev/i2c-1";
int fd_i2c;
 

static mxq_err_t i2c_init(void)
{
	int ret = MXQ_ERR_TRANS_ERROR;
	int addr = 0x60;
    
    fd_i2c = open(i2c_device, O_RDWR);
    if (fd_i2c < 0) return errno;
	
    ret = ioctl(fd_i2c, I2C_SLAVE, addr);
    if (ret == -1) return errno;

    return ret;
}

mxq_err_t HOST_send_bytes_i2c(const mxq_u1* src, mxq_u2 len)
{
	mxq_err_t ret = MXQ_ERR_TRANS_ERROR;

	ret = write(fd_i2c,src,len);
	if (ret == -1) return errno;

	return ret;
}


mxq_err_t HOST_receive_bytes_i2c_first(mxq_u1* dest, mxq_u2 len)
{
	uint32_t timeout=1800000L;
	mxq_err_t ret = MXQ_ERR_TRANS_ERROR;

	dest[0]=0;
	
	/* Send bytes until we receive 55 */
	while((dest[0]!=0x55) && (timeout))
	{
		read(fd_i2c, dest,1);
		timeout--;
	}

	if(timeout==0)
	{
		return MXQ_ERR_TRANS_TIMEOUT;
	}

	dest++;
	len--;

	ret = read(fd_i2c,dest,len);
    if (ret == -1) return errno;

    return ret + 1;
}


mxq_err_t HOST_receive_bytes_i2c(mxq_u1* dest, mxq_u2 len)
{
	mxq_err_t ret = MXQ_ERR_TRANS_ERROR;

	ret = read(fd_i2c, dest, len);
	if (ret == -1) return errno;

	return ret;
}


mxq_err_t HOST_init_i2c(void)
{
	init_pins();
	return i2c_init();
}
#endif //#ifdef USE_PROTOCOL_I2C


void HOST_wait_usec(mxq_u4 duration_usec)
{
		usleep(duration_usec);
}


mxq_err_t HOST_reset_maxq106x(void) {
#if defined(MAXQ1065)
    GPIOWrite(PIN_PW_DOWN,LOW);
    usleep(5000);
    GPIOWrite(PIN_PW_DOWN,HIGH);
#elif defined(MAXQ106x)
	GPIOWrite(PIN_RST,LOW);
    usleep(5);
	GPIOWrite(PIN_RST,HIGH);
#endif
	usleep(TIME_WAIT_RESET);
	
	return MXQ_OK;
}

mxq_err_t HOST_wakeup_maxq106x(void) {
#if defined(MAXQ1065)
	GPIOWrite(PIN_PW_DOWN,HIGH);
	usleep(TIME_WAIT_RESET);
#elif defined(MAXQ106x)
	GPIOWrite(PIN_WKP,LOW);
    usleep(TIME_WAIT_RESET);
	GPIOWrite(PIN_WKP,HIGH);
#endif

	return MXQ_OK;
}

mxq_err_t HOST_sleep_maxq10xx(void) {
#if defined(MAXQ1065)
	GPIOWrite(PIN_PW_DOWN,LOW);
#endif
	return MXQ_OK;
}

/******************		Mode		*******************************************/

mxq_err_t HOST_follow_tls(void)
{
#if defined(MAXQ106x)
	GPIOWrite(PIN_MODE,LOW);
	usleep(TIME_WAIT_RESET);
#endif
	return MXQ_OK;
}
	
mxq_err_t HOST_activate_tls(void)
{
#if defined(MAXQ106x)
	GPIOWrite(PIN_MODE,LOW);
	usleep(TIME_WAIT_RESET);

	HOST_reset_maxq106x();
#endif
	return MXQ_OK;
}

mxq_err_t HOST_activate_gcu(void)
{
#if defined(MAXQ106x)
	GPIOWrite(PIN_MODE,HIGH);
	usleep(TIME_WAIT_RESET);

	HOST_reset_maxq106x();
#endif
	return MXQ_OK;
}


mxq_err_t HOST_follow_gcu(void)
{
#if defined(MAXQ106x)
	GPIOWrite(PIN_MODE,HIGH);
	usleep(TIME_WAIT_RESET);
#endif
	return MXQ_OK;
}

#undef _HOST_EMBEDDED_LINUX_C
