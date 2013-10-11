
#ifndef __NO_OS_PORT_H__
#define __NO_OS_PORT_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "stdint.h"
#include "limits.h"
#include "stdio.h"
#include "stdlib.h"
#include "sleep.h"
#include "linux.h"
#include "xil_printf.h"
#include "ad9361.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define SUCCESS							0
#define ARRAY_SIZE(arr)					(sizeof(arr) / sizeof((arr)[0]))
#define min(x, y)						((x < y) ? x : y)
#define max(x, y)						((x > y) ? x : y)
#define clamp(val, min_val, max_val)	(max(min(val, max_val), min_val))
#define min_t(type, x, y)				(type)min((type)x, (type)y)
#define max_t(type, x, y)				(type)max((type)x, (type)y)
#define clamp_t(type, val, min_val, max_val)	\
			(type)clamp((type)val, (type)min_val, (type)max_val)
#define INIT_COMPLETION(x)				((x).done = 0)
#define DIV_ROUND_UP(x, y)				((x + y - 1) / y)
#define DIV_ROUND_CLOSEST(x, divisor)(                  \
{                                                       \
        typeof(x) __x = x;                              \
        typeof(divisor) __d = divisor;                  \
        (((typeof(x))-1) > 0 ||                         \
         ((typeof(divisor))-1) > 0 || (__x) > 0) ?      \
                (((__x) + ((__d) / 2)) / (__d)) :       \
                (((__x) - ((__d) / 2)) / (__d));        \
}                                                       \
)
#define BIT(x)							(1 << (x))
#define CLK_IGNORE_UNUSED       		BIT(3)
#define CLK_GET_RATE_NOCACHE    		BIT(6)

#define dev_err(format, ...)		printf(format, ## __VA_ARGS__)
#define dev_dbg(format, ...)		printf(format, ## __VA_ARGS__)
#define dev_info(format, ...)		printf(format, ## __VA_ARGS__)
#define printk(format, ...)				printf(format, ## __VA_ARGS__)
#define WARN(format, ...)			printf(format, ## __VA_ARGS__)
#define msleep_interruptible(msecs)		usleep(msecs*1000)
#define pr_err							printf
#define pr_warning						printf

static inline void * ERR_PTR(long error)
{
        return (void *) error;
}

static inline unsigned long __ffs(unsigned long word)
{
        int num = 0;

        if ((word & 0xffff) == 0) {
                num += 16;
                word >>= 16;
        }
        if ((word & 0xff) == 0) {
                num += 8;
                word >>= 8;
        }
        if ((word & 0xf) == 0) {
                num += 4;
                word >>= 4;
        }
        if ((word & 0x3) == 0) {
                num += 2;
                word >>= 2;
        }
        if ((word & 0x1) == 0)
                num += 1;
        return num;
}

#define container_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})

#define do_div(n,base) ({							\
		uint64_t __res;								\
		__res = ((uint64_t) (n)) % (uint64_t) (base);	\
		n = ((uint64_t) (n)) / (uint64_t) (base);		\
		__res;										\
})

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
void udelay(unsigned long usecs);
int32_t spi_init(uint32_t device_id,
				 uint8_t  clk_pha,
				 uint8_t  clk_pol);
int32_t spi_read(uint8_t *data,
				 uint8_t bytes_number);
int spi_write_then_read(const unsigned char *txbuf, unsigned n_tx,
                        unsigned char *rxbuf, unsigned n_rx);
void gpio_init(uint32_t device_id);
void gpio_direction(uint8_t pin, uint8_t direction);
void gpio_data(uint8_t pin, uint8_t data);
bool gpio_is_valid(int number);
void gpio_set_value(unsigned gpio, int value);
int clk_prepare_enable(struct clk *clk);
unsigned long clk_get_rate(struct ad9361_rf_phy *phy,
						   struct refclk_scale *clk_priv);
int clk_set_rate(struct ad9361_rf_phy *phy,
				 struct refclk_scale *clk_priv,
				 unsigned long rate);
void clear_bit(int nr, volatile unsigned long *addr);
void set_bit(int nr, volatile unsigned long *addr);
unsigned long int_sqrt(unsigned long x);
int ilog2(int x);
void complete_all(struct completion *x);
bool schedule_work(struct work_struct *work);

#endif
