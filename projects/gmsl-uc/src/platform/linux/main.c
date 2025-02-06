/***************************************************************************/ /**
*   @file   main.c
*   @brief  Sample GMSL application
*   @author Automotive Software and Systems team, Bangalore, India
********************************************************************************
* Copyright 2024(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*  - Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*  - Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in
*    the documentation and/or other materials provided with the
*    distribution.
*  - Neither the name of Analog Devices, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*  - The use of this software may or may not infringe the patent rights
*    of one or more patent holders.  This license does not release you
*    from the requirement that you obtain separate licenses from these
*    patent holders to use this software.
*  - Use of the software either in source or binary form, must be run
*    on or directly connected to an Analog Devices Inc. component.
*
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "platform_includes.h"
#include "gmsl_app_common_data.h"
#include "gmsl_dbg.h"



#if defined(DIAG_GM_UC_EX_ENABLED)
#include "diag_gm_uc_example.h"
#elif defined(DIAG_MB_UC_EX_ENABLED)
#include "diag_mb_uc_example.h"
#elif defined(CFG_MAX96724_MAX96717_QUAD_EX_ENABLED)
#include "config_example.h"
#elif defined(CFG_DIAG_MAX96792_MAX96793_UC_EX_ENABLED)
#include "max96792_max96793_example.h"
#endif

/**
 * @brief Main function execution for Maxim platform.
 *
 * @return ret - Result of the enabled examples execution.
 */
int main(int argc, char *argv[])
{
	int ret = 0;

	GMSL_LOG_INFO("GMSL UC Main");

#if defined(DIAG_GM_UC_EX_ENABLED)
	ret = diag_gm_uc_example();
#elif defined(DIAG_MB_UC_EX_ENABLED)
	ret = diag_mb_uc_example();
#elif defined(CFG_MAX96724_MAX96717_QUAD_EX_ENABLED)
	ret = gmsl_uc_max96724_max96717_quad_des_ex();
#elif defined(CFG_DIAG_MAX96792_MAX96793_UC_EX_ENABLED)
    ret = gmsl_uc_max96792_max96793_ex();
#endif

	return ret;
}
