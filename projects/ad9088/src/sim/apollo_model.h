/***************************************************************************//**
 *   @file   apollo_model.h
 *   @brief  Software model of the AD9084 (Apollo) SPI slave (simulation).
 *   @author Analog Devices, Inc.
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef APOLLO_MODEL_H_
#define APOLLO_MODEL_H_

#include <stdint.h>

/* Handle one SPI frame in place (read data replaces the dummy bytes). */
void apollo_model_xfer(uint8_t *data, uint16_t len);

/* Device memory, by 32-bit address. */
uint8_t apollo_mem_read8(uint32_t addr);
void apollo_mem_write8(uint32_t addr, uint8_t val);
uint32_t apollo_mem_read32(uint32_t addr);
void apollo_mem_write32(uint32_t addr, uint32_t val);

#endif /* APOLLO_MODEL_H_ */
