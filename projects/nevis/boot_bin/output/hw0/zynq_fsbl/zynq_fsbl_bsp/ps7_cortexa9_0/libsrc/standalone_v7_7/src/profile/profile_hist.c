/******************************************************************************
* Copyright (c) 2002 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/


#include "profile.h"
#include "_profile_timer_hw.h"

#ifdef PROC_MICROBLAZE
#include "mblaze_nt_types.h"
#endif

#ifdef PROC_PPC
#include "xpseudo_asm.h"
#define SPR_SRR0 0x01A
#endif

#include "xil_types.h"

extern u32 binsize ;
u32 prof_pc ;

void profile_intr_handler( void )
{

	s32 j;

#ifdef PROC_MICROBLAZE
	asm( "swi r14, r0, prof_pc" ) ;
#elif defined PROC_PPC
	prof_pc = mfspr(SPR_SRR0);
#else
	/* for cortexa9, lr is saved in asm interrupt handler */
#endif
	/* print("PC: "), putnum(prof_pc), print("\r\n"), */
	for(j = 0; j < n_gmon_sections; j++ ){
		if((prof_pc >= ((u32)_gmonparam[j].lowpc)) && (prof_pc < ((u32)_gmonparam[j].highpc))) {
			_gmonparam[j].kcount[(prof_pc-_gmonparam[j].lowpc)/((u32)4 * binsize)]++;
			break;
		}
	}
	/* Ack the Timer Interrupt */
	timer_ack();
}
