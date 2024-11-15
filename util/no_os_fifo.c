/***************************************************************************//**
 *   @file   no_os_fifo.c
 *   @brief  Implementation of fifo.
 *   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <string.h>
#include <stdlib.h>
#include "no_os_fifo.h"
#include "no_os_error.h"
#include "no_os_alloc.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Create new fifo element
 * @param buff - Data to be saved in fifo.
 * @param len - Length of the data.
 * @return fifo element in case of success, NULL otherwise
 */
static struct no_os_fifo_element * fifo_new_element(char *buff, uint32_t len)
{
	struct no_os_fifo_element *q = no_os_calloc(1,
				       sizeof(struct no_os_fifo_element));
	if (!q)
		return NULL;

	q->len = len;
	q->data = no_os_calloc(1, len);
	if (!(q->data)) {
		no_os_free(q);
		return NULL;
	}
	memcpy(q->data, buff, len);

	return q;
}

/**
 * @brief Get last element in fifo
 * @param p_fifo - pointer to fifo
 * @return fifo last element if exists, NULL otherwise
 */
static struct no_os_fifo_element *no_os_fifo_get_last(struct no_os_fifo_element
		*p_fifo)
{
	if(p_fifo == NULL)
		return NULL;
	while (p_fifo->next) {
		p_fifo = p_fifo->next;
	}

	return p_fifo;
}

/**
 * @brief Insert element to fifo, in the last position.
 * @param p_fifo - Pointer to fifo.
 * @param buff - Data to be saved in fifo.
 * @param len - Length of the data.
 * @return 0 in case of success, -1 otherwise
 */
int32_t no_os_fifo_insert(struct no_os_fifo_element **p_fifo, char *buff,
			  uint32_t len)
{
	struct no_os_fifo_element *p, *q;

	if (len <= 0)
		return -1;

	q = fifo_new_element(buff, len);
	if (!q)
		return -1;

	if (!(*p_fifo)) {
		*p_fifo = q;
	} else {
		p = no_os_fifo_get_last(*p_fifo);
		p->next = q;
	}

	return 0;
}

/**
 * @brief Remove fifo head
 * @param p_fifo - Pointer to fifo.
 * @return next element in fifo if exists, NULL otherwise.
 */
struct no_os_fifo_element * no_os_fifo_remove(struct no_os_fifo_element *p_fifo)
{
	struct no_os_fifo_element *p = p_fifo;

	if (p_fifo != NULL) {
		p_fifo = p_fifo->next;
		no_os_free(p->data);
		no_os_free(p);
	}

	return p_fifo;
}
