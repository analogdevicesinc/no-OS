/******************************************************************************
* Copyright (c) 2002 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/


#ifndef	PROFILE_H
#define	PROFILE_H	1

#include <stdio.h>
#include "xil_types.h"
#include "profile_config.h"

#ifdef PROC_MICROBLAZE
#include "mblaze_nt_types.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

void _system_init( void ) ;
void _system_clean( void ) ;
void mcount(u32 frompc, u32 selfpc);
void profile_intr_handler( void ) ;
void _profile_init( void );



/****************************************************************************
 * Profiling on hardware - Hash table maintained on hardware and data sent
 * to xmd for gmon.out generation.
 ****************************************************************************/
/*
 * histogram counters are unsigned shorts (according to the kernel).
 */
#define	HISTCOUNTER	u16

struct tostruct {
	u32  selfpc;
	s32	 count;
	s16  link;
	u16	 pad;
};

struct fromstruct {
	u32 frompc ;
	s16 link ;
	u16 pad ;
} ;

/*
 * general rounding functions.
 */
#define ROUNDDOWN(x,y)	(((x)/(y))*(y))
#define ROUNDUP(x,y)	((((x)+(y)-1)/(y))*(y))

/*
 * The profiling data structures are housed in this structure.
 */
struct gmonparam {
	s32		state;

	/* Histogram Information */
	u16		*kcount;	/* No. of bins in histogram */
	u32		kcountsize;	/* Histogram samples */

	/* Call-graph Information */
	struct fromstruct	*froms;
	u32		fromssize;
	struct tostruct		*tos;
	u32		tossize;

	/* Initialization I/Ps */
	u32    	lowpc;
	u32		highpc;
	u32		textsize;
	/* u32 		cg_froms, */
	/* u32 		cg_tos, */
};
extern struct gmonparam *_gmonparam;
extern s32 n_gmon_sections;

/*
 * Possible states of profiling.
 */
#define	GMON_PROF_ON	0
#define	GMON_PROF_BUSY	1
#define	GMON_PROF_ERROR	2
#define	GMON_PROF_OFF	3

/*
 * Sysctl definitions for extracting profiling information from the kernel.
 */
#define	GPROF_STATE	0	/* int: profiling enabling variable */
#define	GPROF_COUNT	1	/* struct: profile tick count buffer */
#define	GPROF_FROMS	2	/* struct: from location hash bucket */
#define	GPROF_TOS	3	/* struct: destination/count structure */
#define	GPROF_GMONPARAM	4	/* struct: profiling parameters (see above) */

#ifdef __cplusplus
}
#endif

#endif 		/* PROFILE_H */
