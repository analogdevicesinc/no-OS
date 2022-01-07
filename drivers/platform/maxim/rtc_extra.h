#ifndef RTC_EXTRA_H_
#define RTC_EXTRA_H_

#include <stdint.h>
#include "rtc_regs.h"

struct rtc_init_maxim {
	/** Load value for miliseconds */
	uint32_t ms_load;
};

struct rtc_desc_maxim {
	uint32_t ms;
	mxc_rtc_regs_t *rtc_regs;
};

enum rtc_interrupt_id {
	RTC_TIMEOFDAY_INT,
	RTC_SUBSEC_INT
};

enum timer_freq {
	MAX_1024HZ
};

int32_t rtc_enable_irq();
int32_t rtc_disable_irq();
int32_t rtc_register_callback(struct callback_desc *);
int32_t rtc_unregister_callback();

#endif

