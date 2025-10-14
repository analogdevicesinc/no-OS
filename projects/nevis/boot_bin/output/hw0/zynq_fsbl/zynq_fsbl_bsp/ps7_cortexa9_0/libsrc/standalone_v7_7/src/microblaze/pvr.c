/******************************************************************************
* Copyright (c) 2006 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file pvr.c
*
* This header file contains defines for structures used by the microblaze
* PVR routines
*
******************************************************************************/
#include "xparameters.h"
#include "pvr.h"
#include <string.h>

/* Definitions */

/****************************************************************************/
/**
* @brief   Populate the PVR data structure to which pvr points, with the
*          values of the hardware PVR registers.
*
* @param	pvr- address of PVR data structure to be populated
*
* @return	0 - SUCCESS
*          -1 - FAILURE
*
****************************************************************************/
int microblaze_get_pvr (pvr_t *pvr)
{
  if (!pvr)
    return -1;

  bzero ((void*)pvr, sizeof (pvr_t));

#ifdef MICROBLAZE_PVR_NONE
  return -1;
#else
  getpvr (0, pvr->pvr[0]);
#endif  /* MICROBLAZE_PVR_NONE */

#ifdef MICROBLAZE_PVR_FULL
  getpvr (1, pvr->pvr[1]);
  getpvr (2, pvr->pvr[2]);
  getpvr (3, pvr->pvr[3]);

  getpvr (4, pvr->pvr[4]);
  getpvr (5, pvr->pvr[5]);
  getpvr (6, pvr->pvr[6]);
  getpvr (7, pvr->pvr[7]);

  getpvr (8, pvr->pvr[8]);
  getpvr (9, pvr->pvr[9]);
  getpvr (10, pvr->pvr[10]);
  getpvr (11, pvr->pvr[11]);

/*   getpvr (12, pvr->pvr[12]); */
/*   getpvr (13, pvr->pvr[13]); */
/*   getpvr (14, pvr->pvr[14]); */
/*   getpvr (15, pvr->pvr[15]); */

#endif  /* MICROBLAZE_PVR_FULL  */

  return 0;
}
