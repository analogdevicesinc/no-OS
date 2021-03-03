/***************************************************************************//**
 *   @file   fifo.c
 *   @brief  Implementation of fifo.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <string.h>
#include <stdlib.h>
#include "fifo.h"
#include "error.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Create new fifo element
 * @param buff - Data to be saved in fifo.
 * @param len - Length of the data.
 * @return fifo element in case of success, NULL otherwise
 */
static struct fifo_element * fifo_new_element(char *buff, uint32_t len)
{
	struct fifo_element *q = calloc(1, sizeof(struct fifo_element));
	if (!q)
		return NULL;

	q->len = len;
	q->data = calloc(1, len);
	if (!(q->data)) {
		free(q);
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
static struct fifo_element *fifo_get_last(struct fifo_element *p_fifo)
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
 * @return SUCCESS in case of success, FAILURE otherwise
 */
int32_t fifo_insert(struct fifo_element **p_fifo, char *buff, uint32_t len)
{
	struct fifo_element *p, *q;

	if (len <= 0)
		return FAILURE;

	q = fifo_new_element(buff, len);
	if (!q)
		return FAILURE;

	if (!(*p_fifo)) {
		*p_fifo = q;
	} else {
		p = fifo_get_last(*p_fifo);
		p->next = q;
	}

	return SUCCESS;
}

/**
 * @brief Remove fifo head
 * @param p_fifo - Pointer to fifo.
 * @return next element in fifo if exists, NULL otherwise.
 */
struct fifo_element * fifo_remove(struct fifo_element *p_fifo)
{
	struct fifo_element *p = p_fifo;

	if (p_fifo != NULL) {
		p_fifo = p_fifo->next;
		free(p->data);
		free(p);
	}

	return p_fifo;
}
