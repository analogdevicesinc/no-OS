/***************************************************************************//**
 *   @file   iolink_app.h
 *   @brief  IO-Link application layer for the i-link stack.
 *   @author Liviu Stan (liviu.stan@analog.com)
********************************************************************************
 * Copyright 2021 rt-labs AB, Sweden.
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-only
*******************************************************************************/

#ifndef IOLINK_APP_H
#define IOLINK_APP_H

#include "iolink.h"
#include "sys/osal_sys.h"

/* Per-port SMI confirmation event bits (used on port->event) */
#define IOLINK_APP_EVT_PD         BIT(0)
#define IOLINK_APP_EVT_PORTE      BIT(1)
#define IOLINK_APP_EVT_COMLOST    BIT(2)
#define IOLINK_APP_EVT_PORTCFG    BIT(3)
#define IOLINK_APP_EVT_PORTSTATUS BIT(4)
#define IOLINK_APP_EVT_READ       BIT(5)
#define IOLINK_APP_EVT_WRITE      BIT(6)
#define IOLINK_APP_EVT_ERR        BIT(7)

/* Port lifecycle states */
enum iolink_app_port_state {
	IOLINK_APP_PORT_INACTIVE,
	IOLINK_APP_PORT_STARTING,
	IOLINK_APP_PORT_RUNNING,
};

/* Cached result of SMI_PortStatus_req — populated by the SMI callback */
struct iolink_app_port_status {
	iolink_port_status_info_t port_status_info;
	iolink_port_quality_info_t port_quality_info;
	uint8_t revision_id;
	iolink_transmission_rate_t transmission_rate;
	uint8_t master_cycle_time;
	uint16_t vendorid;
	uint32_t deviceid;
};

struct iolink_app_port;

/* Called once when a device reaches OPERATE on a port */
typedef void (*iolink_app_run_cb_t)(struct iolink_app_port *port);

/* Per-port context */
struct iolink_app_port {
	struct iolink_app_master *master;  /* Back-pointer to the master */
	uint8_t portnumber;               /* 1-based (stack convention) */
	enum iolink_app_port_state state;
	os_event_t *event;                /* SMI confirmation signaling */
	os_mutex_t *mtx;                  /* Serializes SMI wrapper calls */
	iolink_smi_errortypes_t error;    /* Last error from JOB_ERROR */
	struct iolink_app_port_status status;
	struct {
		iolink_port_qualifier_info_t pqi;
		uint8_t data[IOLINK_PD_MAX_SIZE];
		uint8_t len;
	} pdin;                           /* Process data input buffer */
	struct {
		uint16_t len;
	} param_read;                     /* ISDU read result length */
	iolink_app_run_cb_t run_cb;       /* User callback on OPERATE */
	void *run_cb_arg;
};

/* Master context — one per iolink_m_init() instance */
struct iolink_app_master {
	iolink_m_t *handle;
	struct iolink_app_port port[IOLINK_NUM_PORTS];
	os_event_t *app_event;
};

/* Initialize master, create port threads, configure ports for SDCI auto. */
int iolink_app_init(struct iolink_app_master *ctx, iolink_m_cfg_t *cfg);
/* Main event loop — handles PD, port events, COMLOST. Never returns. */
void iolink_app_run(struct iolink_app_master *ctx);

/* Read process data from a device (explicit SMI_PDIn_req). */
int iolink_app_pdin(struct iolink_app_port *port, uint8_t *buf,
		    uint8_t *len);
/* Write process data to a device (SMI_PDOut_req, output enable = 1). */
int iolink_app_pdout(struct iolink_app_port *port, const uint8_t *buf,
		     uint8_t len);
/* Read a device parameter via ISDU (SMI_DeviceRead_req). */
int iolink_app_read_param(struct iolink_app_port *port, uint16_t index,
			  uint8_t subindex, uint8_t *buf, uint16_t *len);
/* Write a device parameter via ISDU (SMI_DeviceWrite_req). */
int iolink_app_write_param(struct iolink_app_port *port, uint16_t index,
			   uint8_t subindex, const uint8_t *buf, uint16_t len);
/* Query port status (VID, DID, transmission rate, cycle time, etc.). */
int iolink_app_get_status(struct iolink_app_port *port,
			  struct iolink_app_port_status *status);

#endif /* IOLINK_APP_H */
