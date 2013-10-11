
#ifndef __LINUX_H__
#define __LINUX_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "stdint.h"
#include "xil_types.h"

#define false	0
#define true	1

#define EAGAIN          11      /* Try again */
#define ENOMEM          12      /* Out of memory */
#define EFAULT          14      /* Bad address */
#define ENODEV          19      /* No such device */
#define EINVAL          22      /* Invalid argument */
#define ETIMEDOUT       110     /* Connection timed out */

typedef uint8_t	bool;

struct work_struct {
};
struct completion {
	unsigned int done;
};

struct clk {
	const char    *name;
	unsigned long           rate;
};

struct clk_hw {
        struct clk *clk;
//        const struct clk_init_data *init;
};

struct clk_init_data {
        const char              *name;
        const struct clk_ops    *ops;
        const char              **parent_names;
        u8                      num_parents;
        unsigned long           flags;
};

struct clk_onecell_data {
        struct clk **clks;
        unsigned int clk_num;
};

#endif
