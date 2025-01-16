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
 * @internal
 * @file MXQ_Print.c
 * @date Feb 2018
 * @authors Benjamin Vinot, Maxim Integrated Products Inc.
 *
 * @brief Provide display utilities function.
 * @details Provide print function to dump commuynication, and function to display MAXQ106x status.
 * @ingroup APDU
 */

#include <stdarg.h>

#include "MXQ_Internal.h"
#include "MXQ_Config.h"
#include "MXQ_Types.h"
#include "MXQ_Error.h"
#include "MXQ_API.h"
#include "MXQ_CMD.h"
#include "MXQ_Transport.h"
#include "MXQ_Host.h"
#include "MXQ_Print.h"
#include "MXQ_GCU.h"

#define RED   "\033[5m\033[1m\x1B[31m"
#define GRN   "\033[1m\x1B[32m"
#define YEL   "\033[1m\x1B[33m"
#define BLU   "\033[1m\x1B[34m"
#define MAG   "\033[1m\x1B[35m"
#define CYN   "\033[1m\x1B[36m"
#define WHT   "\033[1m\x1B[37m"
#define RESET "\x1B[0m"


#if (MXQ_DUMP_COMM!=-1)

int g_dump_comm        = MXQ_DUMP_COMM;
int g_debugoutputlevel = MXQ_DUMP_DEBUG;

static void printf_color(char * color, const char * format,...) {

	va_list arg; 

	MXQ_PRINTF("%s",color);

	va_start (arg, format);
	MXQ_VPRINTF(format, arg);
	va_end(arg);
	
	MXQ_PRINTF(RESET);
}


void cmd_print_seccmd(const char * identifier, const unsigned char * pdest, mxq_length length){

	if(g_dump_comm) {	
		unsigned int i;
		
		MXQ_PRINTF("%s", identifier);
		for (i = 0; i < length; i++)
			MXQ_PRINTF("%02X %s", pdest[i], (i+1)%16?"":" - ");
		MXQ_PRINTF("\n");
	}	
}

void cmd_print(const char * identifier, const unsigned char * pdest, mxq_length blocklen){
	
	if(g_dump_comm==1) {	
		unsigned int i;
		
		MXQ_PRINTF("%s", identifier);
		
		for (i = 0; i < blocklen; i++)
			MXQ_PRINTF("%02X ", pdest[i]);
		MXQ_PRINTF("\n");
	}
	else if(g_dump_comm==2) {
		unsigned int i;
	
		MXQ_PRINTF("%s", identifier);

		if(blocklen>=7) {
			printf_color(RED,"%02X ", pdest[0]);
			printf_color(GRN,"%02X ", pdest[1]);
			printf_color(GRN,"%02X ", pdest[2]);
			printf_color(MAG,"%02X ", pdest[3]);
			printf_color(MAG,"%02X ", pdest[4]);
			for (i = 5; i < blocklen - 2 ; i++)
				MXQ_PRINTF("%02X ", pdest[i]);
				
			printf_color(BLU,"%02X ", pdest[blocklen - 2 ]);
			printf_color(BLU,"%02X ", pdest[blocklen - 1]);	
		}
		else {
			for (i = 0; i < blocklen; i++) MXQ_PRINTF("%02X ", pdest[i]);
		}

		MXQ_PRINTF("\n");
	}

}

void cmd_print_gcu(const char * identifier, const unsigned char * pdest, mxq_length blocklen){

	unsigned int i;
	
	MXQ_PRINTF("%s", identifier);

	for (i = 0; i < blocklen; i++) MXQ_PRINTF("%02X ", pdest[i]);

	MXQ_PRINTF("\n");
}

void cmd_print_array(const unsigned char * buffer1, const unsigned char * buffer2, unsigned int len)
{
	unsigned int i;

	for(i=0;i<len;i++){
		MXQ_PRINTF("%02x/%02x ",buffer1[i],buffer2[i]);
	}
}

#endif


void __API__ MXQ_DisplayStatus(mxq_status_t* s)
{

	#ifndef MXQ_NO_PRINTF

	char* states[]={"error","error","error","Delivery","Intialized","Operational","Terminated"};

	mxq_u4 i;

	MXQ_PRINTF("Product Name: %s\n",s->product_name);
	MXQ_PRINTF("Serial      : "); for(i=0;i<MXQ_MAXQ106x_USN_LENGTH;i++) MXQ_PRINTF("%02x",s->serial_num[i]); MXQ_PRINTF("\n");
	MXQ_PRINTF("FW          : %d.%d.%d\n",s->v_fw_maj,s->v_fw_min, s->v_fw_rev);
	MXQ_PRINTF("HW          : %02x\n",s->v_hw_cod);
	MXQ_PRINTF("State       : %s (%d)\n",states[s->lifecycle_state], s->lifecycle_state);
	MXQ_PRINTF("FreeMem     : %ld bytes / %ld bytes\n",(long int)s->freemem, (long int)s->totalmem);

	if(s->sysconfig.allowfactoryreset==0x55aa) MXQ_PRINTF("Dev Mode    : Enabled\n"); else MXQ_PRINTF("Dev Mode    : Disabled\n");

	#else
	(void)s;
	#endif
}


void __API__ MXQ_DisplayConfig(mxq_config_t* s)
{

	#ifndef MXQ_NO_PRINTF

	MXQ_PRINTF("I2C Slave Address:         0x%02x\n",s->i2cslaveaddr);
	MXQ_PRINTF("I2C Char Timeout(ms):      %d\n",s->i2cchartimeout);
	MXQ_PRINTF("SPI Char Timeout(ms):      %d\n",s->spichartimeout);
	MXQ_PRINTF("Block Timeout(ms):         %d\n",s->blocktimeout);
	MXQ_PRINTF("Force HOST auth:           %s\n",s->secpol_needhost==0xaa55?"No":"Yes");
	MXQ_PRINTF("Tamper Action:             0x%04x\n",s->tamper_action);
#if defined(MAXQ1065)
	MXQ_PRINTF("Sleep/Restore allowed:     %s\n",s->sleep_restore_enable==0xaa55?"No":"Yes");
#endif
	MXQ_PRINTF("Terminated allowed:        %s\n",s->terminated_enable==0xaa55?"No":"Yes");
	MXQ_PRINTF("Initial WD period(ms):     %d\n",s->wd_period_initial);
	MXQ_PRINTF("Normal WD period(ms):      %d\n",s->wd_period_normal);
	MXQ_PRINTF("Reset Pulse Width(ms):     %d\n",s->reset_pulse_width);
	MXQ_PRINTF("Reset Pulse Polarity:      0x%04x\n",s->reset_pulse_pol);
	MXQ_PRINTF("Reset Events:              0x%04x\n",s->reset_events);
	MXQ_PRINTF("TLS MS Output:             %s\n",s->tlsmastersecretallowoutput==0xaa55?"No":"Yes");

	#else
	(void)s;
	#endif

}

#ifdef BENCHMARK

#include <time.h>

struct timespec tp;
		//  clk_id = CLOCK_REALTIME;
clockid_t 	  clk_id = CLOCK_MONOTONIC;
	//  clk_id = CLOCK_BOOTTIME;
	//  clk_id = CLOCK_PROCESS_CPUTIME_ID;
unsigned long long last_ts,last_ts_all;
float lastcmdduration;
float lastcmdduration_all;
unsigned long long last_ts_1;
float lastduration_1;

void start_time(void)
{
	  clock_gettime(clk_id, &tp);
	  unsigned long long int curtime = tp.tv_sec*1000000000ULL + tp.tv_nsec;

	  MXQ_PRINTF(">GLOB>now: %llu\n", curtime);
	  last_ts_1 = curtime;

}

void stop_time(void)
{
	  clock_gettime(clk_id, &tp);
	  unsigned long long int curtime = tp.tv_sec*1000000000ULL + tp.tv_nsec;
	  lastduration_1 = (float)(curtime-last_ts_1)/1000000.0;
	  MXQ_PRINTF("<GLOB<now: %llu (dur= %.3fms)\n", curtime, lastduration_1);
	  last_ts_1 = curtime;
}


#ifdef BENCHMARK_CMD
void start_time_all(void)
{
	  clock_gettime(clk_id, &tp);
	  unsigned long long int curtime = tp.tv_sec*1000000000ULL + tp.tv_nsec;

	  MXQ_PRINTF(">ALL>now: %llu\n", curtime);
	  last_ts_all = curtime;

}

void stop_time_all(void)
{
	  clock_gettime(clk_id, &tp);
	  unsigned long long int curtime = tp.tv_sec*1000000000ULL + tp.tv_nsec;
	  lastcmdduration_all = (float)(curtime-last_ts_all)/1000000.0;
	  MXQ_PRINTF("<ENDALL<now: %llu (dur= %.3fms)\n", curtime, lastcmdduration_all);

}

void start_time_proc(void)
{

	  clock_gettime(clk_id, &tp);
	  unsigned long long int curtime = tp.tv_sec*1000000000ULL + tp.tv_nsec;

	  MXQ_PRINTF(">PROC>now: %llu\n", curtime);
	  last_ts = curtime;

}

void stop_time_proc(void)
{
	  clock_gettime(clk_id, &tp);
	  unsigned long long int curtime = tp.tv_sec*1000000000ULL + tp.tv_nsec;
	  lastcmdduration = (float)(curtime-last_ts)/1000000.0;
	  MXQ_PRINTF("<ENDPROC<now: %llu (dur= %.3fms)\n", curtime, lastcmdduration);

}
#else
void start_time_all(void) {}
void stop_time_all(void) {}
void start_time_proc(void) {}
void stop_time_proc(void) {}
#endif // BENCHMARK_CMD


#else
void start_time_all(void) {}
void stop_time_all(void) {}
void start_time_proc(void) {}
void stop_time_proc(void) {}
#endif //BENCHMARK

