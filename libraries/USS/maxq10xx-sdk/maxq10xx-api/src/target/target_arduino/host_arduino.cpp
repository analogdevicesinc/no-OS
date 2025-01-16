/*******************************************************************************
* Copyright (C) 2018 Maxim Integrated Products, Inc., All rights Reserved.
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

#define MXQ_ON_ARDUINO
#include "MXQ_Internal.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <SPI.h>

extern mxq_mode_t g_selectedmode;
int    commspeed,ibdelay; // not used (yet)

#define TIME_WAIT_RESET		60000


/******************		SPI		*********************************************/	
#ifdef USE_PROTOCOL_SPI
static uint32_t speed = 1000000L;
static uint32_t speed_gcu = 20000000L;

void ard_printf(char * format,...) {

	va_list arg; 

	char mystring[2048]; // Risk of buffer overflow here

	va_start (arg, format);
	vsprintf(mystring,format,arg);
	va_end(arg);

	MYSERIAL.print(mystring);
}


void ard_vprintf(char * format, va_list arg) {

	char mystring[2048]; // Risk of buffer overflow here
	sprintf(mystring,format,arg);

	MYSERIAL.print(mystring);
}

mxq_err_t HOST_deinit_interface()
{
	return MXQ_OK;
}

mxq_err_t HOST_init_spi(void)
{
	// start the SPI library:
	digitalWrite(MYSS_PIN, HIGH);    // SS is pin MYSS_PIN
	pinMode(MYSS_PIN,OUTPUT);
	MYSPI.begin();
	MYSPI.beginTransaction (SPISettings (speed, MSBFIRST, SPI_MODE0));   	
	MXQ_PRINTF("SPI bus for maxq1061 initialized\n");
	return MXQ_OK;
}

mxq_err_t HOST_send_bytes_spi(const mxq_u1* src, mxq_length len)
{
	mxq_u1 rx[len];

	return HOST_exchange_bytes_spi(src,len,rx);
}

mxq_err_t HOST_exchange_bytes_spi(const mxq_u1* src, mxq_length len, mxq_u1* dest)
{
   
  	digitalWrite(MYSS_PIN, LOW);    // SS is pin MYSS_PIN
    for (int i=0 ; i<len; i++) {
			dest[i]=MYSPI.transfer (src[i]);
			//char s[16];
			//sprintf(s,"%02x/%02x ",src[i],dest[i]);
    		//MYSERIAL.print(s);
		}
  	digitalWrite(MYSS_PIN, HIGH);    // SS is pin MYSS_PIN

	//MYSERIAL.print('\n');
	return (mxq_err_t)len;
}

mxq_err_t HOST_receive_bytes_spi_first(mxq_u1* dest, mxq_length len)
{
	mxq_err_t ret;

	dest[0]=0;

	mxq_u1 dummy[1]={0xcc};

	uint32_t timeout=90000L;

	/* Send bytes until we receive 55 */
	while((dest[0]!=0x55) && (timeout))
	{

		HOST_exchange_bytes_spi(dummy,1,dest);
		timeout--;
	}

	if(timeout==0) {
		/* spi_deselect_device(SPI_MASTER_BASE,&mq); */
		return MXQ_ERR_TRANS_TIMEOUT;
	}

	dest++;
	len--;

	len = HOST_receive_bytes_spi(dest,len);
	if(len<=0) return (mxq_err_t)len;

	return (mxq_err_t)(len+1);
}

mxq_err_t HOST_receive_bytes_spi(mxq_u1* dest, mxq_length len)
{
	mxq_err_t ret;
	mxq_u1 dummy[len];

	memset(dummy,0xCC,sizeof(dummy));

	return HOST_exchange_bytes_spi(dummy,len,dest);
}

#endif

/******************		Mode		*******************************************/



mxq_err_t HOST_follow_tls(void)
{
	HOST_wait_usec(TIME_WAIT_RESET);

	return MXQ_OK;
}
	
mxq_err_t HOST_activate_tls(void)
{
	pinMode(2,OUTPUT);
	digitalWrite(2,LOW);

	return HOST_reset_maxq106x();
}

mxq_err_t HOST_activate_gcu(void)
{
	pinMode(2,OUTPUT);
	digitalWrite(2,HIGH);
	
	return HOST_reset_maxq106x();
}


mxq_err_t HOST_follow_gcu(void)
{
	HOST_wait_usec(TIME_WAIT_RESET);

	return MXQ_OK;
}


mxq_err_t HOST_reset_maxq106x(void) 
{

	pinMode(4,OUTPUT);
	digitalWrite(4,LOW);
	HOST_wait_usec(100);
	digitalWrite(4,HIGH);

	HOST_wait_usec(TIME_WAIT_RESET);
	
	return MXQ_OK;
}

mxq_err_t HOST_wakeup_maxq106x(void) 
{

	pinMode(3,OUTPUT);
	digitalWrite(3,LOW);
	HOST_wait_usec(100);
	digitalWrite(3,HIGH);
	
	return MXQ_OK;

}

void HOST_wait_usec(mxq_u4 usec) {
	delayMicroseconds(usec);
}
