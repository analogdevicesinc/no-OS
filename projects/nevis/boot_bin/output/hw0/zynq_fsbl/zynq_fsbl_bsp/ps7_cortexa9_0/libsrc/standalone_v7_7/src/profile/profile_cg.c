/******************************************************************************
* Copyright (c) 2002 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/


#include "profile.h"
#include "_profile_timer_hw.h"
#ifdef PROC_MICROBLAZE
#include "mblaze_nt_types.h"
#endif

/*
 * The mcount function is excluded from the library, if the user defines
 * PROFILE_NO_GRAPH.
 */
#ifndef PROFILE_NO_GRAPH

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef PROFILE_NO_FUNCPTR
s32 searchpc(const struct fromto_struct *cgtable, s32 cgtable_size, u32 frompc );
#else
s32 searchpc(const struct fromstruct *froms, s32 fromssize, u32 frompc );
#endif

/*extern struct gmonparam *_gmonparam, */

#ifdef PROFILE_NO_FUNCPTR
s32 searchpc(const struct fromto_struct *cgtable, s32 cgtable_size, u32 frompc )
{
	s32 index = 0 ;

	while( (index < cgtable_size) && (cgtable[index].frompc != frompc) ){
		index++ ;
	}
	if( index == cgtable_size ) {
		return -1 ;
	} else {
		return index ;
	}
}
#else
s32 searchpc(const struct fromstruct *froms, s32 fromssize, u32 frompc )
{
	s32 index = 0 ;
	s32 Status;

	while( (index < fromssize) && (froms[index].frompc != frompc) ){
		index++ ;
	}
	if( index == fromssize ) {
		Status = -1 ;
	} else {
		Status = index ;
	}
	return Status;
}
#endif		/* PROFILE_NO_FUNCPTR */


void mcount( u32 frompc, u32 selfpc )
{
	register struct gmonparam *p = NULL;
	register s32 toindex, fromindex;
	s32 j;

	disable_timer();

	/*print("CG: "), putnum(frompc), print("->"), putnum(selfpc), print("\r\n") ,
	 * check that frompcindex is a reasonable pc value.
	 * for example:	signal catchers get called from the stack,
	 *		not from text space.  too bad.
	*/
	for(j = 0; j < n_gmon_sections; j++ ){
		if((frompc >= _gmonparam[j].lowpc) && (frompc < _gmonparam[j].highpc)) {
			p = &_gmonparam[j];
			break;
		}
	}
	if( j == n_gmon_sections ) {
		goto done;
	}

#ifdef PROFILE_NO_FUNCPTR
	fromindex = searchpc( p->cgtable, p->cgtable_size, frompc ) ;
	if( fromindex == -1 ) {
		fromindex = p->cgtable_size ;
		p->cgtable_size++ ;
		p->cgtable[fromindex].frompc = frompc ;
		p->cgtable[fromindex].selfpc = selfpc ;
		p->cgtable[fromindex].count = 1 ;
		goto done ;
	}
	p->cgtable[fromindex].count++ ;
#else
	fromindex = (s32)searchpc( p->froms, ((s32)p->fromssize), frompc ) ;
	if( fromindex == -1 ) {
		fromindex = (s32)p->fromssize ;
		p->fromssize++ ;
		/*if( fromindex >= N_FROMS ) {
		* print("Error : From PC table overflow\r\n")
		* goto overflow
		*}*/
		p->froms[fromindex].frompc = frompc ;
		p->froms[fromindex].link = -1 ;
	}else {
		toindex = ((s32)(p->froms[fromindex].link));
		while(toindex != -1) {
			toindex = (((s32)p->tossize) - toindex)-1 ;
			if( p->tos[toindex].selfpc == selfpc ) {
				p->tos[toindex].count++ ;
				goto done ;
			}
			toindex = ((s32)(p->tos[toindex].link)) ;
		}
	}

	/*if( toindex == -1 ) { */
	p->tos-- ;
	p->tossize++ ;
	/* if( toindex >= N_TOS ) {
	* print("Error : To PC table overflow\r\n")
	* goto overflow
	*} */
	p->tos[0].selfpc = selfpc ;
	p->tos[0].count = 1 ;
	p->tos[0].link = p->froms[fromindex].link ;
	p->froms[fromindex].link = ((s32)(p->tossize))-((s32)1);
#endif

 done:
	p->state = GMON_PROF_ON;
	goto enable_timer_label ;
 /* overflow: */
	/*p->state = GMON_PROF_ERROR */
 enable_timer_label:
	enable_timer();
	return ;
}


#endif		/* PROFILE_NO_GRAPH */
