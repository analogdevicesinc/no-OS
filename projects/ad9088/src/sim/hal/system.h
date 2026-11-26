/***************************************************************************//**
 *   @file   system.h
 *   @brief  Simulation stand-in for the generated Nios V BSP system.h.
 *
 *           The ad9088 project hard-codes its base addresses in parameters.h;
 *           from the generated system.h it only uses the JTAG-UART IRQ
 *           wiring (IIO example), which the simulation does not need.
 *   @author Analog Devices, Inc.
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef SIM_SYSTEM_H_
#define SIM_SYSTEM_H_

#define SYS_UART_IRQ				0
#define SYS_UART_IRQ_INTERRUPT_CONTROLLER_ID	0

#endif /* SIM_SYSTEM_H_ */
