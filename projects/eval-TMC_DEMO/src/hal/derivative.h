/*******************************************************************************
* Copyright © 2019 TRINAMIC Motion Control GmbH & Co. KG
* (now owned by Analog Devices Inc.),
*
* Copyright © 2023 Analog Devices Inc. All Rights Reserved. This software is
* proprietary & confidential to Analog Devices, Inc. and its licensors.
*******************************************************************************/


#ifndef _DERIVATIVE_H_
	#define  _DERIVATIVE_H_

/* This is purely for the Eclipse parsing.
 * while working on Board-specific code you can
 * select one of the two boards here.
 * The Build process already selects one via makefile & makeFor.h files,
 * this choice will therefore not influence the build process.
 */
#if !defined(Landungsbruecke) && !defined(LandungsbrueckeV3) && !defined(LandungsbrueckeSmall)
#warning "No Board selected by makefile, defining one for debug purposes"
//#define Landungsbruecke
#define LandungsbrueckeV3
//#define LandungsbrueckeSmall
#endif

	#include "tmc/helpers/API_Header.h"

    #if defined(Landungsbruecke)
        #define MODULE_ID "0012"
        #include <MK20D10.h>
        #include "hal/Landungsbruecke/freescale/Cpu.h"
        #include "hal/Landungsbruecke/freescale/nvic.h"
        #define CPU_LITTLE_ENDIAN
        #define __MK_xxx_H__
    #elif defined(LandungsbrueckeV3)
        #define MODULE_ID "0026"
		#include "gd32f4xx.h"
    #elif defined(LandungsbrueckeSmall)
        // The Landungsbruecke (Small) is a normal Landungsbruecke but with a MK20DX256VLL10 µC instead.
        // This other µC has less memory. We assign a different module ID, otherwise the functionality
        // is identical to the normal Landungsbruecke.
        #define MODULE_ID "0016"
        #include <MK20D10.h>
        #include "hal/Landungsbruecke/freescale/Cpu.h"
        #include "hal/Landungsbruecke/freescale/nvic.h"
        #define CPU_LITTLE_ENDIAN
        #define __MK_xxx_H__
	#else
	#error "No Board selected"
	#endif

#endif /* _DERIVATIVE_H_ */
