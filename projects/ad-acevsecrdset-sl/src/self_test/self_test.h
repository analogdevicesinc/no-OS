/***************************************************************************//**
 *   @file   self_test.h
 *   @brief  Self-tests interface file.
 *   @author GMois (george.mois@analog.com), REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright (c) 2023 Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef __SELF_TEST_H__
#define __SELF_TEST_H__

#include "state_machine.h"

/*! Startup test */
int self_test_startup(struct stout *stout,
		      struct rms_adc_values *rms_adc_values);

/*! Relay test */
int self_test_relay_stuck(struct stout *stout);

/*! Startup test for RCD */
int self_test_rcd(struct stout *stout);

/*! Running test for RCD */
int self_test_rcd_running(struct stout *stout, uint16_t step,
			  enum state_machine_events_e *event);

/*! Startup test for power supply */
int self_test_supply(struct stout *stout,
		     struct rms_adc_values *rms_adc_values);

/*! Running test for relay */
int self_test_relay_closed(struct stout *stout, int32_t v2);

/*! Startup test for upstream PE detection */
int self_test_pe_upstream(struct stout *stout);

#endif /* __SELF_TEST_H__ */
