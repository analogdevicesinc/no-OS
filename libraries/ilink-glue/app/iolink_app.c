/***************************************************************************//**
 *   @file   iolink_app.c
 *   @brief  IO-Link application layer for the i-link stack.
 *   @author Liviu Stan (liviu.stan@analog.com)
********************************************************************************
 * Copyright 2021 rt-labs AB, Sweden.
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-only
*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include "osal.h"
#include "osal_log.h"
#include "iolink.h"
#include "iolink_main.h"
#include "iolink_dl.h"
#include "iolink_app.h"

/*
 * Main event loop bits — shifted by port index so each port gets its own bit.
 * Gaps of 8 leave room for up to 8 ports per category.
 */
#define APP_EVT_PD_0      BIT(0)
#define APP_EVT_PORTE_0   BIT(8)
#define APP_EVT_COMLOST_0 BIT(16)

#define APP_CNF_TIMEOUT_MS  1000
#define APP_ISDU_TIMEOUT_MS 10000
#define APP_RETRY_DELAY_MS  1000

static struct iolink_app_master *app_ctx;

/*
 * Central SMI callback — dispatches all stack confirmations/indications.
 * Registered via iolink_m_cfg_t.cb_smi. Runs in the stack's master thread.
 * Stores response data and sets event bits to wake up waiting callers.
 */
static void smi_cb(void *arg, uint8_t portnumber,
		   iolink_arg_block_id_t ref_arg_block_id,
		   uint16_t arg_block_len, arg_block_t *arg_block)
{
	struct iolink_app_master *m = arg;
	struct iolink_app_port *port = &m->port[portnumber - 1];

	switch (arg_block->id) {
	case IOLINK_ARG_BLOCK_ID_PORT_STATUS_LIST: {
		arg_block_portstatuslist_t *sl =
			(arg_block_portstatuslist_t *)arg_block;
		port->status.port_status_info = sl->port_status_info;
		port->status.port_quality_info = sl->port_quality_info;
		port->status.revision_id = sl->revision_id;
		port->status.transmission_rate = sl->transmission_rate;
		port->status.master_cycle_time = sl->master_cycle_time;
		port->status.vendorid = sl->vendorid;
		port->status.deviceid = sl->deviceid;
		os_event_set(port->event, IOLINK_APP_EVT_PORTSTATUS);
		break;
	}
	case IOLINK_ARG_BLOCK_ID_PORT_EVENT: {
		arg_block_portevent_t *pe = (arg_block_portevent_t *)arg_block;
		switch (pe->event.event_code) {
		case IOLINK_EVENTCODE_PORT_STATUS_CHANGE:
		case IOLINK_EVENTCODE_BAD_DID:
		case IOLINK_EVENTCODE_BAD_VID:
			os_event_set(m->app_event,
				     APP_EVT_PORTE_0 << (portnumber - 1));
			break;
		case IOLINK_EVENTCODE_NO_DEV:
			if (port->state == IOLINK_APP_PORT_RUNNING)
				port->state = IOLINK_APP_PORT_INACTIVE;
			os_event_set(m->app_event,
				     APP_EVT_COMLOST_0 << (portnumber - 1));
			break;
		default:
			break;
		}
		break;
	}
	case IOLINK_ARG_BLOCK_ID_DEV_EVENT:
		break;
	case IOLINK_ARG_BLOCK_ID_OD_RD:
		port->param_read.len = arg_block_len - sizeof(arg_block_od_t);
		os_event_set(port->event, IOLINK_APP_EVT_READ);
		break;
	case IOLINK_ARG_BLOCK_ID_PD_IN: {
		arg_block_pdin_t *pd = (arg_block_pdin_t *)arg_block;
		memcpy(port->pdin.data, pd->data, pd->h.len);
		port->pdin.len = pd->h.len;
		port->pdin.pqi = pd->h.port_qualifier_info;
		os_event_set(port->event, IOLINK_APP_EVT_READ);
		break;
	}
	case IOLINK_ARG_BLOCK_ID_JOB_ERROR: {
		arg_block_joberror_t *je = (arg_block_joberror_t *)arg_block;
		port->error = je->error;
		switch (ref_arg_block_id) {
		case IOLINK_ARG_BLOCK_ID_OD_RD:
			os_event_set(port->event, IOLINK_APP_EVT_READ);
			break;
		case IOLINK_ARG_BLOCK_ID_OD_WR:
			os_event_set(port->event, IOLINK_APP_EVT_WRITE);
			break;
		default:
			os_event_set(port->event, IOLINK_APP_EVT_ERR);
			break;
		}
		break;
	}
	default:
		switch (ref_arg_block_id) {
		case IOLINK_ARG_BLOCK_ID_PORT_CFG_LIST:
			os_event_set(port->event, IOLINK_APP_EVT_PORTCFG);
			break;
		case IOLINK_ARG_BLOCK_ID_OD_WR:
			os_event_set(port->event, IOLINK_APP_EVT_WRITE);
			break;
		default:
			break;
		}
		break;
	}
}

/* PD callback — called by the stack every IO-Link cycle with fresh data. */
static void pd_cb(uint8_t portnumber, void *arg, uint8_t data_len,
		  const uint8_t *data)
{
	struct iolink_app_master *m = arg;
	struct iolink_app_port *port = &m->port[portnumber - 1];

	memcpy(port->pdin.data, data, data_len);
	port->pdin.len = data_len;
	os_event_set(m->app_event, APP_EVT_PD_0 << (portnumber - 1));
}

/* Block until the SMI callback sets the expected event bit, or timeout. */
static int wait_for_cnf(struct iolink_app_port *port, uint32_t mask,
			uint32_t ms)
{
	uint32_t event_value;

	if (os_event_wait(port->event, mask, &event_value, ms)) {
		LOG_WARNING(LOG_STATE_ON,
			    "iolink_app: port %u: timeout waiting for 0x%X\n",
			    port->portnumber, (unsigned int)mask);
		return -1;
	}

	os_event_clr(port->event, event_value);

	if (port->error != IOLINK_SMI_ERRORTYPE_NONE) {
		iolink_smi_errortypes_t err = port->error;
		port->error = IOLINK_SMI_ERRORTYPE_NONE;
		LOG_WARNING(LOG_STATE_ON,
			    "iolink_app: port %u: SMI error 0x%04X\n",
			    port->portnumber, err);
		return -1;
	}

	return 0;
}

/* Configure a port for SDCI auto mode and wait for confirmation. */
static int configure_port(struct iolink_app_port *port)
{
	arg_block_portconfiglist_t cfg;

	memset(&cfg, 0, sizeof(cfg));
	cfg.arg_block.id = IOLINK_ARG_BLOCK_ID_PORT_CFG_LIST;
	cfg.configlist.port_cycle_time = 0;
	cfg.configlist.vendorid = 0;
	cfg.configlist.deviceid = 0;
	cfg.configlist.portmode = IOLINK_PORTMODE_IOL_AUTO;
	cfg.configlist.validation_backup = IOLINK_VALIDATION_CHECK_NO;
	cfg.configlist.iq_behavior = IOLINK_IQ_BEHAVIOR_NO_SUPPORT;

	port->state = IOLINK_APP_PORT_STARTING;

	iolink_error_t err = SMI_PortConfiguration_req(
				     port->portnumber,
				     IOLINK_ARG_BLOCK_ID_VOID_BLOCK,
				     sizeof(arg_block_portconfiglist_t),
				     (arg_block_t *)&cfg);
	if (err != IOLINK_ERROR_NONE)
		return -1;

	return wait_for_cnf(port, IOLINK_APP_EVT_PORTCFG, APP_CNF_TIMEOUT_MS);
}

/* Send SMI_PortStatus_req and wait for the result. */
static int query_port_status(struct iolink_app_port *port)
{
	arg_block_void_t vb;

	memset(&vb, 0, sizeof(vb));
	vb.arg_block.id = IOLINK_ARG_BLOCK_ID_VOID_BLOCK;

	iolink_error_t err = SMI_PortStatus_req(
				     port->portnumber,
				     IOLINK_ARG_BLOCK_ID_PORT_STATUS_LIST,
				     sizeof(arg_block_void_t),
				     (arg_block_t *)&vb);
	if (err != IOLINK_ERROR_NONE)
		return -1;

	return wait_for_cnf(port, IOLINK_APP_EVT_PORTSTATUS, APP_CNF_TIMEOUT_MS);
}

/* Called on PORT_STATUS_CHANGE — verify OPERATE and transition to RUNNING. */
static void start_port(struct iolink_app_port *port)
{
	if (query_port_status(port) != 0)
		return;

	if (port->status.port_status_info != IOLINK_PORT_STATUS_INFO_OP) {
		LOG_WARNING(LOG_STATE_ON,
			    "iolink_app: port %u: not operational (status=%u)\n",
			    port->portnumber, port->status.port_status_info);
		return;
	}

	LOG_INFO(LOG_STATE_ON,
		 "iolink_app: port %u: VID=0x%04X DID=0x%06X\n",
		 port->portnumber, port->status.vendorid,
		 (int)port->status.deviceid);

	port->state = IOLINK_APP_PORT_RUNNING;

	if (port->run_cb)
		port->run_cb(port);
}

/* Default PD handler — prints hex bytes to the console, throttled to 1Hz. */
static void handle_pd_event(struct iolink_app_port *port)
{
	static uint32_t last_print_us[IOLINK_NUM_PORTS];
	uint8_t idx = port->portnumber - 1;
	uint8_t pdata[IOLINK_PD_MAX_SIZE];
	uint8_t len;
	uint32_t now;

	if (port->state != IOLINK_APP_PORT_RUNNING)
		return;

	len = port->pdin.len;
	if (len == 0)
		return;

	now = os_get_current_time_us();
	if (now - last_print_us[idx] < 1000000)
		return;

	last_print_us[idx] = now;

	memcpy(pdata, port->pdin.data, len);

	printf("Port %u PD [%d]: ", port->portnumber, len);
	for (int i = 0; i < len; i++)
		printf("%02X ", pdata[i]);
	printf("\n");
}

/**
 * @brief Initialize the IO-Link application layer.
 * @param ctx - Master context (caller-allocated, zeroed internally).
 * @param cfg - i-link stack master configuration. cb_smi, cb_pd and cb_arg
 *              are set internally; the caller only needs to fill port_cfgs,
 *              port_cnt, and thread priorities/stack sizes.
 * @return 0 on success, -1 on failure.
 */
int iolink_app_init(struct iolink_app_master *ctx, iolink_m_cfg_t *cfg)
{
	uint8_t i;

	memset(ctx, 0, sizeof(*ctx));
	app_ctx = ctx;

	ctx->app_event = os_event_create();
	cfg->cb_arg = ctx;
	cfg->cb_smi = smi_cb;
	cfg->cb_pd = pd_cb;

	ctx->handle = iolink_m_init(cfg);
	if (ctx->handle == NULL) {
		LOG_ERROR(LOG_STATE_ON, "iolink_app: iolink_m_init failed\n");
		return -1;
	}

	for (i = 0; i < cfg->port_cnt; i++) {
		if (*cfg->port_cfgs[i].mode == iolink_mode_INACTIVE)
			continue;

		struct iolink_app_port *port = &ctx->port[i];
		port->master = ctx;
		port->portnumber = i + 1;
		port->event = os_event_create();
		port->mtx = os_mutex_create();

		iolink_port_t *pl = iolink_get_port(ctx->handle,
						    port->portnumber);
		iolink_dl_instantiate(pl, cfg->dl_thread_prio,
				      cfg->dl_thread_stack_size);

		if (configure_port(port) != 0)
			LOG_WARNING(LOG_STATE_ON,
				    "iolink_app: failed to configure port %u\n",
				    i + 1);
	}

	return 0;
}

/**
 * @brief Run the IO-Link application event loop (never returns).
 * @param ctx - Master context from iolink_app_init().
 */
void iolink_app_run(struct iolink_app_master *ctx)
{
	uint8_t i;
	uint8_t port_cnt = 0;

	for (i = 0; i < IOLINK_NUM_PORTS; i++) {
		if (ctx->port[i].event != NULL)
			port_cnt = i + 1;
	}

	while (1) {
		uint32_t event_value;

		if (os_event_wait(ctx->app_event, 0xFFFFFFFF,
				  &event_value, 1000))
			continue;

		os_event_clr(ctx->app_event, event_value);

		for (i = 0; i < port_cnt; i++) {
			struct iolink_app_port *port = &ctx->port[i];

			if ((APP_EVT_PD_0 << i) & event_value)
				handle_pd_event(port);

			if ((APP_EVT_PORTE_0 << i) & event_value) {
				if (port->state == IOLINK_APP_PORT_STARTING)
					start_port(port);
			}
		}

		for (i = 0; i < port_cnt; i++) {
			if (!((APP_EVT_COMLOST_0 << i) & event_value))
				continue;

			struct iolink_app_port *port = &ctx->port[i];
			if (port->state == IOLINK_APP_PORT_RUNNING)
				continue;
			port->state = IOLINK_APP_PORT_INACTIVE;
			LOG_WARNING(LOG_STATE_ON,
				    "iolink_app: port %u: COMLOST, retrying\n",
				    port->portnumber);
			os_usleep(APP_RETRY_DELAY_MS * 1000);
			iolink_dl_reset(iolink_get_port(ctx->handle,
							port->portnumber));
			configure_port(port);
		}
	}
}

/**
 * @brief Read process data from a device.
 * @param port - Port context.
 * @param buf - Buffer to receive PD bytes.
 * @param len - On return, number of bytes written to buf.
 * @return 0 on success, -1 on failure.
 */
int iolink_app_pdin(struct iolink_app_port *port, uint8_t *buf, uint8_t *len)
{
	arg_block_void_t vb;

	os_mutex_lock(port->mtx);

	memset(&vb, 0, sizeof(vb));
	vb.arg_block.id = IOLINK_ARG_BLOCK_ID_VOID_BLOCK;

	iolink_error_t err = SMI_PDIn_req(
				     port->portnumber,
				     IOLINK_ARG_BLOCK_ID_PD_IN,
				     sizeof(arg_block_void_t),
				     (arg_block_t *)&vb);
	if (err != IOLINK_ERROR_NONE) {
		os_mutex_unlock(port->mtx);
		return -1;
	}

	int ret = wait_for_cnf(port, IOLINK_APP_EVT_READ, APP_CNF_TIMEOUT_MS);
	if (ret == 0) {
		memcpy(buf, port->pdin.data, port->pdin.len);
		*len = port->pdin.len;
	}

	os_mutex_unlock(port->mtx);
	return ret;
}

/**
 * @brief Write process data to a device.
 * @param port - Port context.
 * @param buf - PD bytes to send.
 * @param len - Number of bytes in buf.
 * @return 0 on success, -1 on failure.
 */
int iolink_app_pdout(struct iolink_app_port *port, const uint8_t *buf,
		     uint8_t len)
{
	arg_block_pdout_t pdout;

	os_mutex_lock(port->mtx);

	pdout.h.arg_block.id = IOLINK_ARG_BLOCK_ID_PD_OUT;
	pdout.h.len = len;
	pdout.h.oe = 1;
	memcpy(pdout.data, buf, len);

	iolink_error_t err = SMI_PDOut_req(
				     port->portnumber,
				     IOLINK_ARG_BLOCK_ID_VOID_BLOCK,
				     sizeof(arg_block_pdout_head_t) + len,
				     (arg_block_t *)&pdout);

	os_mutex_unlock(port->mtx);
	return (err == IOLINK_ERROR_NONE) ? 0 : -1;
}

/**
 * @brief Read a device parameter via ISDU.
 * @param port - Port context.
 * @param index - ISDU index.
 * @param subindex - ISDU subindex.
 * @param buf - Buffer to receive parameter data.
 * @param len - In: buffer size. Out: actual bytes received.
 * @return 0 on success, -1 on failure.
 */
int iolink_app_read_param(struct iolink_app_port *port, uint16_t index,
			  uint8_t subindex, uint8_t *buf, uint16_t *len)
{
	uint16_t req_len = *len;
	uint16_t arg_block_len = sizeof(arg_block_od_t) + req_len;
	uint8_t buffer[arg_block_len];
	arg_block_od_t *od = (arg_block_od_t *)buffer;

	os_mutex_lock(port->mtx);

	memset(buffer, 0, arg_block_len);
	od->arg_block.id = IOLINK_ARG_BLOCK_ID_OD_RD;
	od->index = index;
	od->subindex = subindex;

	iolink_error_t err = SMI_DeviceRead_req(
				     port->portnumber,
				     IOLINK_ARG_BLOCK_ID_OD_RD,
				     arg_block_len,
				     (arg_block_t *)od);
	if (err != IOLINK_ERROR_NONE) {
		os_mutex_unlock(port->mtx);
		return -1;
	}

	int ret = wait_for_cnf(port, IOLINK_APP_EVT_READ,
			       APP_ISDU_TIMEOUT_MS);
	if (ret == 0 && port->param_read.len > 0) {
		uint16_t copy_len = port->param_read.len;
		if (copy_len > req_len)
			copy_len = req_len;
		memcpy(buf, od->data, copy_len);
		*len = copy_len;
		port->param_read.len = 0;
	}

	os_mutex_unlock(port->mtx);
	return ret;
}

/**
 * @brief Write a device parameter via ISDU.
 * @param port - Port context.
 * @param index - ISDU index.
 * @param subindex - ISDU subindex.
 * @param buf - Parameter data to write.
 * @param len - Number of bytes in buf.
 * @return 0 on success, -1 on failure.
 */
int iolink_app_write_param(struct iolink_app_port *port, uint16_t index,
			   uint8_t subindex, const uint8_t *buf, uint16_t len)
{
	uint16_t arg_block_len = sizeof(arg_block_od_t) + len;
	uint8_t buffer[arg_block_len];
	arg_block_od_t *od = (arg_block_od_t *)buffer;

	os_mutex_lock(port->mtx);

	memset(buffer, 0, arg_block_len);
	od->arg_block.id = IOLINK_ARG_BLOCK_ID_OD_WR;
	od->index = index;
	od->subindex = subindex;
	memcpy(od->data, buf, len);

	iolink_error_t err = SMI_DeviceWrite_req(
				     port->portnumber,
				     IOLINK_ARG_BLOCK_ID_VOID_BLOCK,
				     arg_block_len,
				     (arg_block_t *)od);
	if (err != IOLINK_ERROR_NONE) {
		os_mutex_unlock(port->mtx);
		return -1;
	}

	int ret = wait_for_cnf(port, IOLINK_APP_EVT_WRITE,
			       APP_ISDU_TIMEOUT_MS);

	os_mutex_unlock(port->mtx);
	return ret;
}

/**
 * @brief Query the current status of a port.
 * @param port - Port context.
 * @param status - Output: port status (VID, DID, transmission rate, etc.).
 * @return 0 on success, -1 on failure.
 */
int iolink_app_get_status(struct iolink_app_port *port,
			  struct iolink_app_port_status *status)
{
	os_mutex_lock(port->mtx);
	int ret = query_port_status(port);
	if (ret == 0)
		*status = port->status;
	os_mutex_unlock(port->mtx);
	return ret;
}
