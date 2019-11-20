/***************************************************************************//**
 *   @file   network.c
 *   @brief  Implementation of network interface.
 *   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
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

#include <errno.h>
#include "lwip/tcp.h"
#include "uthash.h"
#include "lwip_init.h"
#include "network.h"
#include "error.h"
#include "fifo.h"
#define DEBUG_NETWORK
#if defined (__arm__) || defined (__aarch64__)
#include "xil_printf.h"
#endif

enum network_states {
	ES_NONE = 0,
	ES_ACCEPTED,
	ES_RECEIVED,
	ES_CLOSING
};

struct network_instance {
	int32_t instance_id;
	uint32_t read_offset;
	u8_t state;
	u8_t retries;
	struct tcp_pcb *pcb;
	/* pbuf (chain) to recycle */
	struct pbuf *p;
	UT_hash_handle hh;         /* makes this structure hashable */
};

static struct network_instance *instances = NULL;

err_t network_accept(void *arg, struct tcp_pcb *newpcb, err_t err);
err_t network_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
void network_error(void *arg, err_t err);
void network_store(struct tcp_pcb *tpcb, struct network_instance *es);
err_t network_poll(void *arg, struct tcp_pcb *tpcb);

/***************************************************************************//**
 * @brief network_keep_alive
*******************************************************************************/
void network_keep_alive(void)
{
	lwip_keep_alive();
}

/***************************************************************************//**
 * @brief network_init
*******************************************************************************/
ssize_t network_init(void)
{
	return init_lwip();
}

/***************************************************************************//**
 * @brief start_application
*******************************************************************************/
void network_start(void)
{
	struct tcp_pcb *pcb;
	u16_t port = 30431;

	pcb = tcp_new();
	if (pcb != NULL) {
		err_t err;

		err = tcp_bind(pcb, IP_ADDR_ANY, port);
		if (err == ERR_OK) {
			pcb = tcp_listen(pcb);
			tcp_accept(pcb, network_accept);
			xil_printf("tinyiiod server started @ port %d\n\r", port);
		} else {
			/* abort? output diagnostic? */
		}
	} else {
		/* abort? output diagnostic? */
	}
}

/***************************************************************************//**
 * @brief network_accept
*******************************************************************************/
err_t network_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
	struct network_instance *es;
	static int32_t inst_id = 0;
	err_t ret_err;

	inst_id++;

	LWIP_UNUSED_ARG(arg);
	LWIP_UNUSED_ARG(err);

	tcp_setprio(newpcb, TCP_PRIO_MAX);

	es = (struct network_instance *)mem_malloc(sizeof(struct network_instance));
	if (es != NULL) {
		es->state = ES_ACCEPTED;
		es->pcb = newpcb;
		es->retries = 0;
		es->p = NULL;
		es->instance_id = inst_id;
		es->read_offset = 0;
		/* cppcheck-suppress uninitvar ; cppcheck reports this as a false positive */
		HASH_ADD_INT( instances, instance_id, es );
#ifdef DEBUG_NETWORK
		printf("new clientconnected: %"PRIi32"\n", inst_id);
#endif
		/* pass newly allocated es to our callbacks */
		tcp_arg(newpcb, es);
		tcp_recv(newpcb, network_recv);
		tcp_err(newpcb, network_error);
		ret_err = ERR_OK;
	} else {
		ret_err = ERR_MEM;
	}

	return ret_err;
}
//static int32_t current_inst_id;
/***************************************************************************//**
 * @brief network_recv
*******************************************************************************/
err_t network_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
	struct network_instance *es = (struct network_instance *)arg;

	if (es->p == NULL) {
		es->p = p;
	}
	else {
		if (p && (p->len > 0))
			pbuf_chain(es->p, p);
	}

	return ERR_OK;
}

/***************************************************************************//**
 * @brief network_error
*******************************************************************************/
void network_error(void *arg, err_t err)
{
	struct network_instance *es;

	LWIP_UNUSED_ARG(err);

	es = (struct network_instance *)arg;
	if (es != NULL) {
		mem_free(es);
	}
}

#include <xpseudo_asm_gcc.h>
typedef uint32_t sys_prot_t;
#define SYS_ARCH_DECL_PROTECT(lev) sys_prot_t lev

void sys_arch_unprotect(sys_prot_t lev)
{
#if defined (__arm__) || defined (__aarch64__)
	mtcpsr(lev);
#else
	mtmsr(lev);
#endif
}

sys_prot_t sys_arch_protect()
{
	sys_prot_t cur;
#ifdef __MICROBLAZE__
	cur = mfmsr();
	mtmsr(cur & ~0x2);
#elif __arm__
	cur = mfcpsr();
	mtcpsr(cur | 0xC0);
#elif __aarch64__
	cur = mfcpsr();
	mtcpsr(cur | 0xC0);
#endif
	return cur;
}

#if defined (__arm__) || defined (__aarch64__)
#define SYS_ARCH_PROTECT(lev) lev = sys_arch_protect()
#else
#define SYS_ARCH_PROTECT(lev)
#endif

#if defined (__arm__) || defined (__aarch64__)
#define SYS_ARCH_UNPROTECT(lev) sys_arch_unprotect(lev)
#else
#define SYS_ARCH_UNPROTECT(lev)
#endif


static int32_t read_byte(char *buf, uint32_t *instance_id)
{
//	SYS_ARCH_DECL_PROTECT(level);
	uint8_t freed;
	struct network_instance *instance, *tmp;

	lwip_keep_alive();

	int32_t ret = -ENOENT;
	if (0xFFFFAAAA == *instance_id)
	{
		HASH_ITER(hh, instances, instance, tmp) {
			if (instance->p)
				break;
		}
	}
	else
	{
		HASH_FIND_INT( instances, instance_id, instance);
		if (instance == NULL)
			return -ENOENT;
	}
	if (instance == NULL)
		return -ENOENT;

//	SYS_ARCH_PROTECT(level);
	if (instance->p) {
		struct pbuf *p = instance->p;
		if (p->len > 0)
		{
			*instance_id = instance->instance_id;
			*buf = ((char*)(p->payload))[instance->read_offset];
			instance->read_offset++;
			ret = SUCCESS;
		}
		if (p->len - instance->read_offset <= 0) {
			instance->read_offset = 0;
			instance->p = p->next;

			tcp_recved(instance->pcb, p->len);

			p->next = NULL;
			do {
				freed = pbuf_free(p);
			} while(freed == 0);
		}
	}
//	SYS_ARCH_UNPROTECT(level);
	return ret;
}

/***************************************************************************//**
 * @brief network_read_line
*******************************************************************************/
ssize_t network_read_line(char *data, size_t bytes_number, uint32_t *instance_id)
{
	ssize_t ret;
	uint32_t i = 0;

	do {
		ret = read_byte(&data[i], instance_id);
		if (ret < 0)
			continue;
		i++;
		if (data[i - 1] == '\n')
			break;
	} while(i < bytes_number);
	data[i - 2] = 0;

	return i - 2;
}

/***************************************************************************//**
 * @brief network_read
*******************************************************************************/
ssize_t network_read(char *data, size_t bytes_number, uint32_t *instance_id)
{
	ssize_t ret;
	uint32_t i = 0;

	do {
		ret = read_byte(&data[i], instance_id);
		if (ret < 0)
			continue;
		i++;
	} while(i < bytes_number);

	return i;
}

/***************************************************************************//**
 * @brief network_write_data
*******************************************************************************/
ssize_t network_write_data(const char *buf, size_t len, uint32_t instance_id)
{
	struct network_instance *instance = NULL;
	u8_t apiflags = TCP_WRITE_FLAG_COPY;
	const char *pbuffer = buf;
	err_t ret;

	/* cppcheck-suppress uninitvar ; cppcheck reports this as a false positive */
	HASH_FIND_INT( instances, &instance_id, instance);
	if(instance == NULL)
		return -ENXIO;
	do {
		do {
			lwip_keep_alive();
		} while(tcp_sndbuf(instance->pcb) == 0);

		int32_t buf_len = tcp_sndbuf(instance->pcb);
		int32_t wr_length = buf_len > len ? len : buf_len;
		apiflags |= buf_len > len ? 0 : TCP_WRITE_FLAG_MORE;
		ret = tcp_write(instance->pcb, pbuffer, wr_length, apiflags);
		if(ret < 0)
			return ret;
		ret = tcp_output(instance->pcb);
		if(ret < 0)
			return ret;
		pbuffer += wr_length;
		len -= wr_length;
	} while(len);
	do {
		lwip_keep_alive();
	} while(tcp_sndbuf(instance->pcb) == 0);

	return 0;
}

/***************************************************************************//**
 * @brief network_close
*******************************************************************************/
void network_close(struct network_instance *es)
{
	struct tcp_pcb *tpcb = es->pcb;
	tcp_arg(tpcb, NULL);
	tcp_sent(tpcb, NULL);
	tcp_recv(tpcb, NULL);
	tcp_err(tpcb, NULL);
	tcp_poll(tpcb, NULL, 0);

	if (es != NULL) {
		mem_free(es);
	}
	tcp_close(tpcb);
}

/***************************************************************************//**
 * @brief network_exit
*******************************************************************************/
ssize_t network_close_instance(uint32_t instance_id)
{
	struct network_instance *instance;
	err_t err = 0;
	uint8_t freed;

//#ifdef DEBUG_NETWORK
//	printf("removing cleint instance: %"PRIi32"\n", instance_id);
//#endif
	/* cppcheck-suppress uninitvar ; cppcheck reports this as a false positive */
	HASH_FIND_INT( instances, &instance_id, instance);
	if(instance == NULL)
		return -ENOENT;
	HASH_DEL(instances, instance);
	if (instance != NULL) {
		if (instance->p)
		{
			do {
				freed = pbuf_free(instance->p);
			} while(freed == 0);
		}
		network_close(instance);
	}

#ifdef DEBUG_NETWORK
	printf("removed client inst %"PRIi32" done\n", instance_id);
#endif

	return err;
}
