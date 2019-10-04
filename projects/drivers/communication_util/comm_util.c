/***************************************************************************//**
 *   @file   comm_util.c
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
#include <string.h>
#include <errno.h>
#include "comm_util.h"

/***************************************************************************//**
 * @brief new_buffer
*******************************************************************************/
static struct fifo * new_buffer(uint32_t len)
{
    struct fifo *buf = malloc(sizeof(struct fifo));

    if(!buf)
        return NULL;
    buf->len = 0;
    buf->index = 0;
    buf->data = malloc(len);
    buf->next = NULL;

    return buf;
}

/***************************************************************************//**
 * @brief get_last
*******************************************************************************/
static struct fifo *get_last(struct fifo *p_fifo)
{
    if(p_fifo == NULL)
        return NULL;
    while (p_fifo->next) {
        p_fifo = p_fifo->next;
    }

    return p_fifo;
}

/***************************************************************************//**
 * @brief fifo_insert_tail
*******************************************************************************/
int32_t fifo_insert_tail(struct fifo **p_fifo, char *buff, int32_t len)
{
    if(len <= 0)
    	return 0;

    struct fifo *p = NULL;
    if(!(*p_fifo)) {
        p = new_buffer(len);
        if(!p)
            return -ENOMEM;
        *p_fifo = p;
    } else {
        p = get_last(*p_fifo);
        p->next = new_buffer(0);
        if(!p->next)
            return -ENOMEM;
        p = p->next;
    }
    memcpy(p->data, buff, len);
    p->len = len;

    return len;
}

/***************************************************************************//**
 * @brief fifo_remove_head
*******************************************************************************/
struct fifo * fifo_remove_head(struct fifo *p_fifo)
{
    struct fifo *p = p_fifo;

    if(p_fifo != NULL) {
        p_fifo = p_fifo->next;
        free(p->data);
        p->len = 0;
        p->next = NULL;
        p->data = NULL;
        free(p);
        p = NULL;
    }
    return p_fifo;
}




