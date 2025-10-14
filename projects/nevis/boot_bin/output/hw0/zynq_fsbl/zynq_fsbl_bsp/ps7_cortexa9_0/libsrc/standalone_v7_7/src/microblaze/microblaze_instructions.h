/******************************************************************************
* Copyright (c) 2018 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file microblaze_instructions.h
*
* It provides wrapper macros to call 32/64 bit variant of specific
* arithmetic/logical instructions, based on the processor in execution.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who      Date     Changes
* ----- -------- -------- -----------------------------------------------
* 6.8 	mus  	 10/09/18 First release
*
*
* </pre>
*
******************************************************************************/

/**
 *@cond nocomments
 */

#ifndef MICROBLAZE_INSTRUCTIONS_H /* prevent circular inclusions */
#define MICROBLAZE_INSTRUCTIONS_H /* by using protection macros */

#ifdef __cplusplus
extern "C" {
#endif

#if defined (__arch64__)
#define ADD     addl
#define ADDC    addlc
#define ADDI    addli
#define ADDIC   addlic
#define ADDIK   addlik
#define ADDIKC  addlikc
#define ADDK    addlk
#define ADDKC   addlkc
#define AND     andl
#define ANDI    andli
#define ANDN    andnl
#define ANDNI   andnli
#define BEQI    beaeqi
#define BEQID   beaeqid
#define BGEI    beagei
#define BGEID   beageid
#define BGTI    beagti
#define BGTID   beagtid
#define BLEI    bealei
#define BLEID   bealeid
#define BLTI    bealti
#define BLTID   bealtid
#define BNEI    beanei
#define BNEID   beaneid
#define BR      brea
#define BRALID  brealid
#define BRD     bread
#define BRI     breai
#define BRID    breaid
#define BRLD    breald
#define BRLID   brealid
#define CMP     cmpl
#define CMPU    cmplu
#define LI      lli
#define LOAD    ll
#define OR      orl
#define ORI     orli
#define RSUB    rsubl
#define RSUBC   rsublc
#define RSUBI   rsubli
#define RSUBIC  rsublic
#define RSUBIK  rsublik
#define RSUBIKC rsublikc
#define RSUBK   rsublk
#define RSUBKC  rsublkc
#define SI      sli
#define STORE   sl
#define SUBK    sublk
#define XOR     xorl
#define XORI    xorli
#else
#define ADD     add
#define ADDC    addc
#define ADDI    addi
#define ADDIC   addic
#define ADDIK   addik
#define ADDIKC  addikc
#define ADDK    addk
#define ADDKC   addkc
#define AND     and
#define ANDI    andi
#define ANDN    andn
#define ANDNI   andni
#define BEQI    beqi
#define BEQID   beqid
#define BGEI    bgei
#define BGEID   bgeid
#define BGTI    bgti
#define BGTID   bgtid
#define BLEI    blei
#define BLEID   bleid
#define BLTI    blti
#define BLTID   bltid
#define BNEI    bnei
#define BNEID   bneid
#define BR      br
#define BRALID  bralid
#define BRD     brd
#define BRI     bri
#define BRID    brid
#define BRLD    brld
#define BRLID   brlid
#define CMP     cmp
#define CMPU    cmpu
#define LI      lwi
#define LOAD    lw
#define OR      or
#define ORI     ori
#define RSUB    rsub
#define RSUBC   rsubc
#define RSUBI   rsubi
#define RSUBIC  rsubic
#define RSUBIK  rsubik
#define RSUBIKC rsubikc
#define RSUBK   rsubk
#define RSUBKC  rsubkc
#define SI      swi
#define STORE   sw
#define SUBK    subk
#define XOR     xor
#define XORI    xori
#endif

#ifdef __cplusplus
}
#endif

#endif /* MICROBLAZE_INSTRUCTIONS_H */

/**
 *@endcond
 */
