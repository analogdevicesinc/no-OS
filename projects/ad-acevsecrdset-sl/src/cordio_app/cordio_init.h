#ifndef __CORDIO_INIT_
#define __CORDIO_INIT_

#include "wsf_types.h"
#include "wsf_trace.h"
#include "wsf_bufio.h"
#include "wsf_msg.h"
#include "wsf_assert.h"
#include "wsf_buf.h"
#include "wsf_heap.h"
#include "wsf_cs.h"
#include "wsf_timer.h"
#include "wsf_os.h"

int cordio_init(void);
void cordio_step();
int mcs_start(void);

#endif // __CORDIO_INIT_
