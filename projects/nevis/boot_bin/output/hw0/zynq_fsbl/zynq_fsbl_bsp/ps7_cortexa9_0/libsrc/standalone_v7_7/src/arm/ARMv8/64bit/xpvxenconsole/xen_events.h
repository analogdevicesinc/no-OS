/*
 * xen_events.h
 *
 *  Created on: Sep 14, 2016
 *      Author: jarvis
 */
/*
Copyright DornerWorks 2016

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
following conditions are met:
1.	 Redistributions of source code must retain the above copyright notice, this list of conditions and the
following disclaimer.

THIS SOFTWARE IS PROVIDED BY DORNERWORKS FOR USE ON THE CONTRACTED PROJECT, AND ANY EXPRESS OR IMPLIED WARRANTY
IS LIMITED TO THIS USE. FOR ALL OTHER USES THIS SOFTWARE IS PROVIDED ''AS IS'' AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL DORNERWORKS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef _XEN_EVENTS_H_
#define _XEN_EVENTS_H_

#include "xil_types.h"

#define PAGE_SHIFT 12			// not really a good place for this
#define PAGE_SIZE (1<<PAGE_SHIFT)

#define EVENT_IRQ	31

#define XENMAPSPACE_shared_info  0 /* shared info page */
/* ` enum event_channel_op { // EVTCHNOP_* => struct evtchn_* */
#define EVTCHNOP_bind_interdomain 0
#define EVTCHNOP_bind_virq        1
#define EVTCHNOP_bind_pirq        2
#define EVTCHNOP_close            3
#define EVTCHNOP_send             4
#define EVTCHNOP_status           5
#define EVTCHNOP_alloc_unbound    6
#define EVTCHNOP_bind_ipi         7
#define EVTCHNOP_bind_vcpu        8
#define EVTCHNOP_unmask           9
#define EVTCHNOP_reset           10
#define EVTCHNOP_init_control    11
#define EVTCHNOP_expand_array    12
#define EVTCHNOP_set_priority    13


void notify_evtch(u32 evtch_id);
void init_events(void);
int register_event_handler(u32 evtch_id, void (*fptr)(void));
void unmask_evtchn(u32 port);
void handle_event_irq(void* data);

#endif /* _XEN_EVENTS_H_ */
