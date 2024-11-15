/***************************************************************************//**
 *   @file   self_test.h
 *   @brief  Self-tests interface file.
 *   @author GMois (george.mois@analog.com), REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright (c) 2023 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __SELF_TEST_H__
#define __SELF_TEST_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "state_machine.h"

/******************************************************************************/
/************************* Functions Declarations******************************/
/******************************************************************************/
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
