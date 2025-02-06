/*******************************************************************************
 *   @file   gmsl_app_common_data.c
 *   @brief  Common data definition to be used by gmsl example applications.
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

#include "gmsl_app_common_data.h"
#include "parameters.h"

/* I2C Initialization Parameters for bus id 7 */
const struct no_os_i2c_init_param gmsl_i2c7_ip = {
	.max_speed_hz = 100000U,
	.platform_ops = &linux_i2c_ops,
	.extra = &linux_i2c7_extra_ip,
};

/* I2C Initialization Parameters for bus id 2 */
const struct no_os_i2c_init_param gmsl_i2c2_ip = {
	.max_speed_hz = 100000U,
	.platform_ops = &linux_i2c_ops,
	.extra = &linux_i2c2_extra_ip,
	.slave_address = 0xFF,
};

int ovx3a_cam_init(struct no_os_i2c_desc *desc)
{
	//////////////////////////////////////////////////////////
	// Image Sensor Instructions
	//////////////////////////////////////////////////////////
	gmsl_reg_write(desc, 0x0103u, 0x01u);
	no_os_mdelay(40);
	gmsl_reg_write(desc, 0x4d07u, 0x21u);
	gmsl_reg_write(desc, 0x4d0eu, 0x80u);
	gmsl_reg_write(desc, 0x4d11u, 0x7du);
	gmsl_reg_write(desc, 0x0303u, 0x02u);
	gmsl_reg_write(desc, 0x0305u, 0x30u);
	gmsl_reg_write(desc, 0x0306u, 0x03u);
	gmsl_reg_write(desc, 0x0307u, 0x01u);
	gmsl_reg_write(desc, 0x0316u, 0x00u);
	gmsl_reg_write(desc, 0x0317u, 0x42u);
	gmsl_reg_write(desc, 0x0323u, 0x02u);
	gmsl_reg_write(desc, 0x0325u, 0x68u);
	gmsl_reg_write(desc, 0x0326u, 0x00u);
	gmsl_reg_write(desc, 0x032bu, 0x00u);
	gmsl_reg_write(desc, 0x0400u, 0xe7u);
	gmsl_reg_write(desc, 0x0401u, 0xffu);
	gmsl_reg_write(desc, 0x0404u, 0x2bu);
	gmsl_reg_write(desc, 0x0405u, 0x32u);
	gmsl_reg_write(desc, 0x0406u, 0x33u);
	gmsl_reg_write(desc, 0x0407u, 0x8fu);
	gmsl_reg_write(desc, 0x0408u, 0x0cu);
	gmsl_reg_write(desc, 0x0410u, 0xe7u);
	gmsl_reg_write(desc, 0x0411u, 0xffu);
	gmsl_reg_write(desc, 0x0414u, 0x2bu);
	gmsl_reg_write(desc, 0x0415u, 0x32u);
	gmsl_reg_write(desc, 0x0416u, 0x33u);
	gmsl_reg_write(desc, 0x0417u, 0x8fu);
	gmsl_reg_write(desc, 0x0418u, 0x0cu);
	gmsl_reg_write(desc, 0x3002u, 0x03u);
	gmsl_reg_write(desc, 0x3012u, 0x41u);
	gmsl_reg_write(desc, 0x301eu, 0xb0u);
	gmsl_reg_write(desc, 0x3706u, 0x39u);
	gmsl_reg_write(desc, 0x370au, 0x00u);
	gmsl_reg_write(desc, 0x370bu, 0xd1u);
	gmsl_reg_write(desc, 0x370fu, 0x40u);
	gmsl_reg_write(desc, 0x3711u, 0x22u);
	gmsl_reg_write(desc, 0x3712u, 0x12u);
	gmsl_reg_write(desc, 0x3713u, 0x00u);
	gmsl_reg_write(desc, 0x3718u, 0x04u);
	gmsl_reg_write(desc, 0x3719u, 0x3cu);
	gmsl_reg_write(desc, 0x371au, 0x06u);
	gmsl_reg_write(desc, 0x371bu, 0x14u);
	gmsl_reg_write(desc, 0x372cu, 0x17u);
	gmsl_reg_write(desc, 0x3733u, 0x41u);
	gmsl_reg_write(desc, 0x3741u, 0x44u);
	gmsl_reg_write(desc, 0x3742u, 0x34u);
	gmsl_reg_write(desc, 0x3746u, 0x03u);
	gmsl_reg_write(desc, 0x374bu, 0x03u);
	gmsl_reg_write(desc, 0x3755u, 0x09u);
	gmsl_reg_write(desc, 0x376cu, 0x04u);
	gmsl_reg_write(desc, 0x376du, 0x08u);
	gmsl_reg_write(desc, 0x376fu, 0x08u);
	gmsl_reg_write(desc, 0x3770u, 0x91u);
	gmsl_reg_write(desc, 0x3771u, 0x08u);
	gmsl_reg_write(desc, 0x3774u, 0x8au);
	gmsl_reg_write(desc, 0x3777u, 0x99u);
	gmsl_reg_write(desc, 0x3779u, 0x22u);
	gmsl_reg_write(desc, 0x377au, 0x00u);
	gmsl_reg_write(desc, 0x377bu, 0x00u);
	gmsl_reg_write(desc, 0x377cu, 0x48u);
	gmsl_reg_write(desc, 0x3785u, 0x08u);
	gmsl_reg_write(desc, 0x3790u, 0x10u);
	gmsl_reg_write(desc, 0x3793u, 0x00u);
	gmsl_reg_write(desc, 0x379cu, 0x01u);
	gmsl_reg_write(desc, 0x37a1u, 0x80u);
	gmsl_reg_write(desc, 0x37b3u, 0x0au);
	gmsl_reg_write(desc, 0x37beu, 0x01u);
	gmsl_reg_write(desc, 0x37bfu, 0x01u);
	gmsl_reg_write(desc, 0x37c6u, 0x48u);
	gmsl_reg_write(desc, 0x37c7u, 0x38u);
	gmsl_reg_write(desc, 0x37c9u, 0x00u);
	gmsl_reg_write(desc, 0x37cau, 0x39u);
	gmsl_reg_write(desc, 0x37cbu, 0x00u);
	gmsl_reg_write(desc, 0x37ccu, 0xa3u);
	gmsl_reg_write(desc, 0x37d1u, 0x39u);
	gmsl_reg_write(desc, 0x37d2u, 0x00u);
	gmsl_reg_write(desc, 0x37d3u, 0xa3u);
	gmsl_reg_write(desc, 0x37d5u, 0x39u);
	gmsl_reg_write(desc, 0x37d6u, 0x00u);
	gmsl_reg_write(desc, 0x37d7u, 0xa3u);
	gmsl_reg_write(desc, 0x3c06u, 0x29u);
	gmsl_reg_write(desc, 0x3c0bu, 0xa8u);
	gmsl_reg_write(desc, 0x3c53u, 0x68u);
	gmsl_reg_write(desc, 0x3192u, 0x00u);
	gmsl_reg_write(desc, 0x3193u, 0x00u);
	gmsl_reg_write(desc, 0x3206u, 0x80u);
	gmsl_reg_write(desc, 0x3216u, 0x01u);
	gmsl_reg_write(desc, 0x3400u, 0x08u);
	gmsl_reg_write(desc, 0x3409u, 0x02u);
	gmsl_reg_write(desc, 0x3501u, 0x00u);
	gmsl_reg_write(desc, 0x3502u, 0x40u);
	gmsl_reg_write(desc, 0x3508u, 0x01u);
	gmsl_reg_write(desc, 0x3509u, 0x00u);
	gmsl_reg_write(desc, 0x350au, 0x01u);
	gmsl_reg_write(desc, 0x350bu, 0x00u);
	gmsl_reg_write(desc, 0x350cu, 0x00u);
	gmsl_reg_write(desc, 0x3548u, 0x01u);
	gmsl_reg_write(desc, 0x3549u, 0x90u);
	gmsl_reg_write(desc, 0x354au, 0x01u);
	gmsl_reg_write(desc, 0x354bu, 0x16u);
	gmsl_reg_write(desc, 0x354cu, 0x00u);
	gmsl_reg_write(desc, 0x3581u, 0x00u);
	gmsl_reg_write(desc, 0x3582u, 0x40u);
	gmsl_reg_write(desc, 0x3588u, 0x01u);
	gmsl_reg_write(desc, 0x3589u, 0x90u);
	gmsl_reg_write(desc, 0x358au, 0x01u);
	gmsl_reg_write(desc, 0x358bu, 0x16u);
	gmsl_reg_write(desc, 0x358cu, 0x00u);
	gmsl_reg_write(desc, 0x3600u, 0x00u);
	gmsl_reg_write(desc, 0x3602u, 0x42u);
	gmsl_reg_write(desc, 0x3603u, 0xf3u);
	gmsl_reg_write(desc, 0x3604u, 0x93u);
	gmsl_reg_write(desc, 0x3605u, 0xffu);
	gmsl_reg_write(desc, 0x3606u, 0xc0u);
	gmsl_reg_write(desc, 0x3607u, 0x4au);
	gmsl_reg_write(desc, 0x360au, 0xd0u);
	gmsl_reg_write(desc, 0x360bu, 0x0bu);
	gmsl_reg_write(desc, 0x360eu, 0x88u);
	gmsl_reg_write(desc, 0x3611u, 0x4bu);
	gmsl_reg_write(desc, 0x3612u, 0x4eu);
	gmsl_reg_write(desc, 0x3614u, 0x8au);
	gmsl_reg_write(desc, 0x3615u, 0x98u);
	gmsl_reg_write(desc, 0x3619u, 0x00u);
	gmsl_reg_write(desc, 0x3620u, 0x02u);
	gmsl_reg_write(desc, 0x3626u, 0x0eu);
	gmsl_reg_write(desc, 0x362cu, 0x0eu);
	gmsl_reg_write(desc, 0x362du, 0x12u);
	gmsl_reg_write(desc, 0x362eu, 0x0bu);
	gmsl_reg_write(desc, 0x362fu, 0x18u);
	gmsl_reg_write(desc, 0x3630u, 0x30u);
	gmsl_reg_write(desc, 0x3631u, 0x57u);
	gmsl_reg_write(desc, 0x3632u, 0x99u);
	gmsl_reg_write(desc, 0x3633u, 0x99u);
	gmsl_reg_write(desc, 0x3643u, 0x0cu);
	gmsl_reg_write(desc, 0x3644u, 0x00u);
	gmsl_reg_write(desc, 0x3645u, 0x0eu);
	gmsl_reg_write(desc, 0x3646u, 0x0fu);
	gmsl_reg_write(desc, 0x3647u, 0x0eu);
	gmsl_reg_write(desc, 0x3648u, 0x00u);
	gmsl_reg_write(desc, 0x3649u, 0x11u);
	gmsl_reg_write(desc, 0x364au, 0x12u);
	gmsl_reg_write(desc, 0x364cu, 0x0eu);
	gmsl_reg_write(desc, 0x364du, 0x0eu);
	gmsl_reg_write(desc, 0x364eu, 0x12u);
	gmsl_reg_write(desc, 0x364fu, 0x0eu);
	gmsl_reg_write(desc, 0x3652u, 0xc5u);
	gmsl_reg_write(desc, 0x3657u, 0x88u);
	gmsl_reg_write(desc, 0x3658u, 0x08u);
	gmsl_reg_write(desc, 0x365au, 0x57u);
	gmsl_reg_write(desc, 0x365bu, 0x30u);
	gmsl_reg_write(desc, 0x365cu, 0x18u);
	gmsl_reg_write(desc, 0x365du, 0x0bu);
	gmsl_reg_write(desc, 0x3660u, 0x01u);
	gmsl_reg_write(desc, 0x3661u, 0x07u);
	gmsl_reg_write(desc, 0x3662u, 0x00u);
	gmsl_reg_write(desc, 0x3665u, 0x92u);
	gmsl_reg_write(desc, 0x3666u, 0x13u);
	gmsl_reg_write(desc, 0x3667u, 0x2cu);
	gmsl_reg_write(desc, 0x3668u, 0x95u);
	gmsl_reg_write(desc, 0x3669u, 0x2cu);
	gmsl_reg_write(desc, 0x366fu, 0xc4u);
	gmsl_reg_write(desc, 0x3671u, 0x0bu);
	gmsl_reg_write(desc, 0x3673u, 0x6au);
	gmsl_reg_write(desc, 0x3678u, 0x88u);
	gmsl_reg_write(desc, 0x3800u, 0x00u);
	gmsl_reg_write(desc, 0x3801u, 0x00u);
	gmsl_reg_write(desc, 0x3802u, 0x00u);
	gmsl_reg_write(desc, 0x3803u, 0x04u);
	gmsl_reg_write(desc, 0x3804u, 0x07u);
	gmsl_reg_write(desc, 0x3805u, 0x8fu);
	gmsl_reg_write(desc, 0x3806u, 0x05u);
	gmsl_reg_write(desc, 0x3807u, 0x0bu);
	gmsl_reg_write(desc, 0x3808u, 0x07u);
	gmsl_reg_write(desc, 0x3809u, 0x80u);
	gmsl_reg_write(desc, 0x380au, 0x05u);
	gmsl_reg_write(desc, 0x380bu, 0x00u);
	gmsl_reg_write(desc, 0x380cu, 0x0au);
	gmsl_reg_write(desc, 0x380du, 0x90u);
	gmsl_reg_write(desc, 0x380eu, 0x05u);
	gmsl_reg_write(desc, 0x380fu, 0x37u);
	gmsl_reg_write(desc, 0x3810u, 0x00u);
	gmsl_reg_write(desc, 0x3811u, 0x08u);
	gmsl_reg_write(desc, 0x3813u, 0x04u);
	gmsl_reg_write(desc, 0x381cu, 0x00u);
	gmsl_reg_write(desc, 0x3820u, 0x44u);
	gmsl_reg_write(desc, 0x3821u, 0x00u);
	gmsl_reg_write(desc, 0x3822u, 0x14u);
	gmsl_reg_write(desc, 0x3832u, 0x10u);
	gmsl_reg_write(desc, 0x3833u, 0x01u);
	gmsl_reg_write(desc, 0x3834u, 0xf0u);
	gmsl_reg_write(desc, 0x383du, 0x20u);
	gmsl_reg_write(desc, 0x384cu, 0x02u);
	gmsl_reg_write(desc, 0x384du, 0x14u);
	gmsl_reg_write(desc, 0x384eu, 0x00u);
	gmsl_reg_write(desc, 0x384fu, 0x40u);
	gmsl_reg_write(desc, 0x3850u, 0x00u);
	gmsl_reg_write(desc, 0x3851u, 0x42u);
	gmsl_reg_write(desc, 0x3852u, 0x00u);
	gmsl_reg_write(desc, 0x3853u, 0x40u);
	gmsl_reg_write(desc, 0x3854u, 0x00u);
	gmsl_reg_write(desc, 0x3855u, 0x05u);
	gmsl_reg_write(desc, 0x3856u, 0x05u);
	gmsl_reg_write(desc, 0x3857u, 0x33u);
	gmsl_reg_write(desc, 0x3858u, 0x3cu);
	gmsl_reg_write(desc, 0x3859u, 0x00u);
	gmsl_reg_write(desc, 0x385au, 0x03u);
	gmsl_reg_write(desc, 0x385bu, 0x05u);
	gmsl_reg_write(desc, 0x385cu, 0x32u);
	gmsl_reg_write(desc, 0x385fu, 0x00u);
	gmsl_reg_write(desc, 0x3860u, 0x10u);
	gmsl_reg_write(desc, 0x3861u, 0x00u);
	gmsl_reg_write(desc, 0x3862u, 0x40u);
	gmsl_reg_write(desc, 0x3863u, 0x00u);
	gmsl_reg_write(desc, 0x3864u, 0x40u);
	gmsl_reg_write(desc, 0x3865u, 0x00u);
	gmsl_reg_write(desc, 0x3866u, 0x40u);
	gmsl_reg_write(desc, 0x3b40u, 0x3eu);
	gmsl_reg_write(desc, 0x3b41u, 0x00u);
	gmsl_reg_write(desc, 0x3b42u, 0x02u);
	gmsl_reg_write(desc, 0x3b43u, 0x00u);
	gmsl_reg_write(desc, 0x3b44u, 0x00u);
	gmsl_reg_write(desc, 0x3b45u, 0x20u);
	gmsl_reg_write(desc, 0x3b46u, 0x00u);
	gmsl_reg_write(desc, 0x3b47u, 0x20u);
	gmsl_reg_write(desc, 0x3b84u, 0x05u);
	gmsl_reg_write(desc, 0x3b85u, 0x00u);
	gmsl_reg_write(desc, 0x3b86u, 0x00u);
	gmsl_reg_write(desc, 0x3b87u, 0x10u);
	gmsl_reg_write(desc, 0x3b88u, 0x00u);
	gmsl_reg_write(desc, 0x3b89u, 0x10u);
	gmsl_reg_write(desc, 0x3b8au, 0x00u);
	gmsl_reg_write(desc, 0x3b8bu, 0x08u);
	gmsl_reg_write(desc, 0x3b8eu, 0x03u);
	gmsl_reg_write(desc, 0x3b8fu, 0xe8u);
	gmsl_reg_write(desc, 0x3d85u, 0x0bu);
	gmsl_reg_write(desc, 0x3d8cu, 0x70u);
	gmsl_reg_write(desc, 0x3d8du, 0x26u);
	gmsl_reg_write(desc, 0x3d97u, 0x70u);
	gmsl_reg_write(desc, 0x3d98u, 0x24u);
	gmsl_reg_write(desc, 0x3d99u, 0x70u);
	gmsl_reg_write(desc, 0x3d9au, 0x6du);
	gmsl_reg_write(desc, 0x3d9bu, 0x70u);
	gmsl_reg_write(desc, 0x3d9cu, 0x6eu);
	gmsl_reg_write(desc, 0x3d9du, 0x73u);
	gmsl_reg_write(desc, 0x3d9eu, 0xffu);
	gmsl_reg_write(desc, 0x3f00u, 0x04u);
	gmsl_reg_write(desc, 0x4001u, 0x2bu);
	gmsl_reg_write(desc, 0x4004u, 0x00u);
	gmsl_reg_write(desc, 0x4005u, 0x80u);
	gmsl_reg_write(desc, 0x4008u, 0x02u);
	gmsl_reg_write(desc, 0x4009u, 0x0du);
	gmsl_reg_write(desc, 0x400au, 0x10u);
	gmsl_reg_write(desc, 0x400bu, 0x80u);
	gmsl_reg_write(desc, 0x400fu, 0x80u);
	gmsl_reg_write(desc, 0x4010u, 0x10u);
	gmsl_reg_write(desc, 0x4011u, 0xbbu);
	gmsl_reg_write(desc, 0x4016u, 0x00u);
	gmsl_reg_write(desc, 0x4017u, 0x10u);
	gmsl_reg_write(desc, 0x402eu, 0x00u);
	gmsl_reg_write(desc, 0x402fu, 0x80u);
	gmsl_reg_write(desc, 0x4030u, 0x00u);
	gmsl_reg_write(desc, 0x4031u, 0x80u);
	gmsl_reg_write(desc, 0x4032u, 0x9fu);
	gmsl_reg_write(desc, 0x4033u, 0x00u);
	gmsl_reg_write(desc, 0x4308u, 0x00u);
	gmsl_reg_write(desc, 0x4502u, 0x00u);
	gmsl_reg_write(desc, 0x4507u, 0x16u);
	gmsl_reg_write(desc, 0x4580u, 0xf8u);
	gmsl_reg_write(desc, 0x4602u, 0x02u);
	gmsl_reg_write(desc, 0x4603u, 0x00u);
	gmsl_reg_write(desc, 0x460au, 0x50u);
	gmsl_reg_write(desc, 0x460cu, 0x60u);
	gmsl_reg_write(desc, 0x4800u, 0x04u);
	gmsl_reg_write(desc, 0x480eu, 0x04u);
	gmsl_reg_write(desc, 0x4813u, 0x24u);
	gmsl_reg_write(desc, 0x4815u, 0x2bu);
	gmsl_reg_write(desc, 0x4837u, 0x35u);
	gmsl_reg_write(desc, 0x484bu, 0x27u);
	gmsl_reg_write(desc, 0x484cu, 0x00u);
	gmsl_reg_write(desc, 0x4886u, 0x00u);
	gmsl_reg_write(desc, 0x4903u, 0x80u);
	gmsl_reg_write(desc, 0x4f00u, 0xffu);
	gmsl_reg_write(desc, 0x4f01u, 0xffu);
	gmsl_reg_write(desc, 0x4f05u, 0x01u);
	gmsl_reg_write(desc, 0x5180u, 0x04u);
	gmsl_reg_write(desc, 0x5181u, 0x00u);
	gmsl_reg_write(desc, 0x5182u, 0x04u);
	gmsl_reg_write(desc, 0x5183u, 0x00u);
	gmsl_reg_write(desc, 0x5184u, 0x04u);
	gmsl_reg_write(desc, 0x5185u, 0x00u);
	gmsl_reg_write(desc, 0x5186u, 0x04u);
	gmsl_reg_write(desc, 0x5187u, 0x00u);
	gmsl_reg_write(desc, 0x51a0u, 0x04u);
	gmsl_reg_write(desc, 0x51a1u, 0x00u);
	gmsl_reg_write(desc, 0x51a2u, 0x04u);
	gmsl_reg_write(desc, 0x51a3u, 0x00u);
	gmsl_reg_write(desc, 0x51a4u, 0x04u);
	gmsl_reg_write(desc, 0x51a5u, 0x00u);
	gmsl_reg_write(desc, 0x51a6u, 0x04u);
	gmsl_reg_write(desc, 0x51a7u, 0x00u);
	gmsl_reg_write(desc, 0x51c0u, 0x04u);
	gmsl_reg_write(desc, 0x51c1u, 0x00u);
	gmsl_reg_write(desc, 0x51c2u, 0x04u);
	gmsl_reg_write(desc, 0x51c3u, 0x00u);
	gmsl_reg_write(desc, 0x51c4u, 0x04u);
	gmsl_reg_write(desc, 0x51c5u, 0x00u);
	gmsl_reg_write(desc, 0x51c6u, 0x04u);
	gmsl_reg_write(desc, 0x51c7u, 0x00u);
	gmsl_reg_write(desc, 0x5380u, 0x19u);
	gmsl_reg_write(desc, 0x5382u, 0x2eu);
	gmsl_reg_write(desc, 0x53a0u, 0x41u);
	gmsl_reg_write(desc, 0x53a2u, 0x04u);
	gmsl_reg_write(desc, 0x53a3u, 0x00u);
	gmsl_reg_write(desc, 0x53a4u, 0x04u);
	gmsl_reg_write(desc, 0x53a5u, 0x00u);
	gmsl_reg_write(desc, 0x53a7u, 0x00u);
	gmsl_reg_write(desc, 0x5400u, 0x19u);
	gmsl_reg_write(desc, 0x5402u, 0x2eu);
	gmsl_reg_write(desc, 0x5420u, 0x41u);
	gmsl_reg_write(desc, 0x5422u, 0x04u);
	gmsl_reg_write(desc, 0x5423u, 0x00u);
	gmsl_reg_write(desc, 0x5424u, 0x04u);
	gmsl_reg_write(desc, 0x5425u, 0x00u);
	gmsl_reg_write(desc, 0x5427u, 0x00u);
	gmsl_reg_write(desc, 0x5480u, 0x19u);
	gmsl_reg_write(desc, 0x5482u, 0x2eu);
	gmsl_reg_write(desc, 0x54a0u, 0x41u);
	gmsl_reg_write(desc, 0x54a2u, 0x04u);
	gmsl_reg_write(desc, 0x54a3u, 0x00u);
	gmsl_reg_write(desc, 0x54a4u, 0x04u);
	gmsl_reg_write(desc, 0x54a5u, 0x00u);
	gmsl_reg_write(desc, 0x54a7u, 0x00u);
	gmsl_reg_write(desc, 0x5800u, 0x38u);
	gmsl_reg_write(desc, 0x5801u, 0x03u);
	gmsl_reg_write(desc, 0x5802u, 0xc0u);
	gmsl_reg_write(desc, 0x5804u, 0x00u);
	gmsl_reg_write(desc, 0x5805u, 0x80u);
	gmsl_reg_write(desc, 0x5806u, 0x01u);
	gmsl_reg_write(desc, 0x5807u, 0x00u);
	gmsl_reg_write(desc, 0x580eu, 0x10u);
	gmsl_reg_write(desc, 0x5812u, 0x34u);
	gmsl_reg_write(desc, 0x5000u, 0x89u);
	gmsl_reg_write(desc, 0x5001u, 0x42u);
	gmsl_reg_write(desc, 0x5002u, 0x19u);
	gmsl_reg_write(desc, 0x5003u, 0x16u);
	gmsl_reg_write(desc, 0x5004u, 0x00u);
	gmsl_reg_write(desc, 0x5005u, 0x40u);
	gmsl_reg_write(desc, 0x5006u, 0x00u);
	gmsl_reg_write(desc, 0x5007u, 0x40u);
	gmsl_reg_write(desc, 0x503eu, 0x00u);
	gmsl_reg_write(desc, 0x503fu, 0x00u);
	gmsl_reg_write(desc, 0x5602u, 0x02u);
	gmsl_reg_write(desc, 0x5603u, 0x58u);
	gmsl_reg_write(desc, 0x5604u, 0x03u);
	gmsl_reg_write(desc, 0x5605u, 0x20u);
	gmsl_reg_write(desc, 0x5606u, 0x02u);
	gmsl_reg_write(desc, 0x5607u, 0x58u);
	gmsl_reg_write(desc, 0x5608u, 0x03u);
	gmsl_reg_write(desc, 0x5609u, 0x20u);
	gmsl_reg_write(desc, 0x560au, 0x02u);
	gmsl_reg_write(desc, 0x560bu, 0x58u);
	gmsl_reg_write(desc, 0x560cu, 0x03u);
	gmsl_reg_write(desc, 0x560du, 0x20u);
	gmsl_reg_write(desc, 0x560eu, 0x02u);
	gmsl_reg_write(desc, 0x560fu, 0x58u);
	gmsl_reg_write(desc, 0x5610u, 0x03u);
	gmsl_reg_write(desc, 0x5611u, 0x20u);
	gmsl_reg_write(desc, 0x5612u, 0x02u);
	gmsl_reg_write(desc, 0x5613u, 0x58u);
	gmsl_reg_write(desc, 0x5614u, 0x03u);
	gmsl_reg_write(desc, 0x5615u, 0x20u);
	gmsl_reg_write(desc, 0x5616u, 0x02u);
	gmsl_reg_write(desc, 0x5617u, 0x58u);
	gmsl_reg_write(desc, 0x5618u, 0x03u);
	gmsl_reg_write(desc, 0x5619u, 0x20u);
	gmsl_reg_write(desc, 0x5642u, 0x02u);
	gmsl_reg_write(desc, 0x5643u, 0x58u);
	gmsl_reg_write(desc, 0x5644u, 0x03u);
	gmsl_reg_write(desc, 0x5645u, 0x20u);
	gmsl_reg_write(desc, 0x5646u, 0x02u);
	gmsl_reg_write(desc, 0x5647u, 0x58u);
	gmsl_reg_write(desc, 0x5648u, 0x03u);
	gmsl_reg_write(desc, 0x5649u, 0x20u);
	gmsl_reg_write(desc, 0x564au, 0x02u);
	gmsl_reg_write(desc, 0x564bu, 0x58u);
	gmsl_reg_write(desc, 0x564cu, 0x03u);
	gmsl_reg_write(desc, 0x564du, 0x20u);
	gmsl_reg_write(desc, 0x564eu, 0x02u);
	gmsl_reg_write(desc, 0x564fu, 0x58u);
	gmsl_reg_write(desc, 0x5650u, 0x03u);
	gmsl_reg_write(desc, 0x5651u, 0x20u);
	gmsl_reg_write(desc, 0x5652u, 0x02u);
	gmsl_reg_write(desc, 0x5653u, 0x58u);
	gmsl_reg_write(desc, 0x5654u, 0x03u);
	gmsl_reg_write(desc, 0x5655u, 0x20u);
	gmsl_reg_write(desc, 0x5656u, 0x02u);
	gmsl_reg_write(desc, 0x5657u, 0x58u);
	gmsl_reg_write(desc, 0x5658u, 0x03u);
	gmsl_reg_write(desc, 0x5659u, 0x20u);
	gmsl_reg_write(desc, 0x5682u, 0x02u);
	gmsl_reg_write(desc, 0x5683u, 0x58u);
	gmsl_reg_write(desc, 0x5684u, 0x03u);
	gmsl_reg_write(desc, 0x5685u, 0x20u);
	gmsl_reg_write(desc, 0x5686u, 0x02u);
	gmsl_reg_write(desc, 0x5687u, 0x58u);
	gmsl_reg_write(desc, 0x5688u, 0x03u);
	gmsl_reg_write(desc, 0x5689u, 0x20u);
	gmsl_reg_write(desc, 0x568au, 0x02u);
	gmsl_reg_write(desc, 0x568bu, 0x58u);
	gmsl_reg_write(desc, 0x568cu, 0x03u);
	gmsl_reg_write(desc, 0x568du, 0x20u);
	gmsl_reg_write(desc, 0x568eu, 0x02u);
	gmsl_reg_write(desc, 0x568fu, 0x58u);
	gmsl_reg_write(desc, 0x5690u, 0x03u);
	gmsl_reg_write(desc, 0x5691u, 0x20u);
	gmsl_reg_write(desc, 0x5692u, 0x02u);
	gmsl_reg_write(desc, 0x5693u, 0x58u);
	gmsl_reg_write(desc, 0x5694u, 0x03u);
	gmsl_reg_write(desc, 0x5695u, 0x20u);
	gmsl_reg_write(desc, 0x5696u, 0x02u);
	gmsl_reg_write(desc, 0x5697u, 0x58u);
	gmsl_reg_write(desc, 0x5698u, 0x03u);
	gmsl_reg_write(desc, 0x5699u, 0x20u);
	gmsl_reg_write(desc, 0x5709u, 0x0fu);
	gmsl_reg_write(desc, 0x5749u, 0x0fu);
	gmsl_reg_write(desc, 0x5789u, 0x0fu);
	gmsl_reg_write(desc, 0x5200u, 0x70u);
	gmsl_reg_write(desc, 0x5201u, 0x70u);
	gmsl_reg_write(desc, 0x5202u, 0x73u);
	gmsl_reg_write(desc, 0x5203u, 0xffu);
	gmsl_reg_write(desc, 0x5205u, 0x6fu);
	gmsl_reg_write(desc, 0x5209u, 0x18u);
	gmsl_reg_write(desc, 0x520bu, 0x04u);
	gmsl_reg_write(desc, 0x5285u, 0x6fu);
	gmsl_reg_write(desc, 0x5289u, 0x18u);
	gmsl_reg_write(desc, 0x528bu, 0x04u);
	gmsl_reg_write(desc, 0x5305u, 0x6fu);
	gmsl_reg_write(desc, 0x5309u, 0x18u);
	gmsl_reg_write(desc, 0x530bu, 0x04u);
	gmsl_reg_write(desc, 0x3501u, 0x01u);
	gmsl_reg_write(desc, 0x3502u, 0x90u);
	gmsl_reg_write(desc, 0x0100u, 0x01u);
	return 0;
}