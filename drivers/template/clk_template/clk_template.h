
#include <stdbool.h>
#include <stdint.h>

struct clk_template {
	const char *name;
	bool enable;
	uint64_t rate;
	struct no_os_clk_desc *clk_out;
};

struct clk_template_init {
	const char *name;
};

extern const struct no_os_clk_platform_ops clk_template_ops;

int clk_template_init(struct clk_template **desc,
		const struct clk_template_init *init);
int clk_template_remove(struct clk_template *desc);
