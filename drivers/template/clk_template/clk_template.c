
#include "no_os_alloc.h"
#include "no_os_clk.h"
#include "no_os_error.h"

#include "clk_template.h"

int clk_template_init(struct clk_template **desc,
		const struct clk_template_init *init)
{
	struct no_os_clk_init_param clk_out_init;
	struct clk_template *descriptor;
	struct no_os_clk_desc *clk_out;
	int ret;

	descriptor = (struct clk_template *)no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	descriptor->name = init->name;

	clk_out_init.name = init->name;
	clk_out_init.hw_ch_num = 0;
	clk_out_init.dev_desc = &descriptor;
	clk_out_init.platform_ops = &clk_template_ops;

	ret = no_os_clk_init(&clk_out, &clk_out_init);
	if (ret)
		goto error;

	descriptor->clk_out = clk_out;

	*desc = descriptor;

	return 0;

error:
	no_os_free(descriptor);

	return ret;
}

int clk_template_remove(struct clk_template *desc)
{
	no_os_free(desc);

	return 0;
}

int clk_template_enable(struct no_os_clk_desc *desc)
{
	struct clk_template *clk = desc->dev_desc;

	clk->enable = 1;

	return 0;
}

int clk_template_disable(struct no_os_clk_desc *desc)
{
	struct clk_template *clk = desc->dev_desc;

	clk->enable = 0;

	return 0;
}

int clk_template_recalc_rate(struct no_os_clk_desc *desc,
			      uint64_t *rate)
{
	struct clk_template *clk = desc->dev_desc;

	*rate = clk->rate;

	return 0;
}

int clk_template_round_rate(struct no_os_clk_desc *desc,
			     uint64_t rate,
			     uint64_t *rounded_rate)
{
	*rounded_rate = rate;

	return 0;
}

int clk_template_set_rate(struct no_os_clk_desc *desc,
			   uint64_t rate)
{
	struct clk_template *clk = desc->dev_desc;

	clk->rate = rate;

	return 0;
}

const struct no_os_clk_platform_ops clk_template_ops = {
	.clk_enable = &clk_template_enable,
	.clk_disable = &clk_template_disable,
	.clk_recalc_rate =&clk_template_recalc_rate,
	.clk_round_rate = &clk_template_round_rate,
	.clk_set_rate = &clk_template_set_rate,

};
