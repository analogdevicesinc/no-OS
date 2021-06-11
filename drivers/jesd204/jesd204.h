#include <stdint.h>
#include <stdbool.h>

#define ARRAY_SIZE(x) \
	(sizeof(x) / sizeof((x)[0]))

enum jesd204_state_op_reason {
	JESD204_STATE_OP_REASON_INIT,
	JESD204_STATE_OP_REASON_UNINIT,
};

enum jesd204_dev_op {
	JESD204_OP_DEVICE_INIT,
	JESD204_OP_LINK_INIT,
	JESD204_OP_LINK_SUPPORTED,
	JESD204_OP_LINK_PRE_SETUP,
	JESD204_OP_CLK_SYNC_STAGE1,
	JESD204_OP_CLK_SYNC_STAGE2,
	JESD204_OP_CLK_SYNC_STAGE3,
	JESD204_OP_LINK_SETUP,
	JESD204_OP_OPT_SETUP_STAGE1,
	JESD204_OP_OPT_SETUP_STAGE2,
	JESD204_OP_OPT_SETUP_STAGE3,
	JESD204_OP_OPT_SETUP_STAGE4,
	JESD204_OP_OPT_SETUP_STAGE5,
	JESD204_OP_CLOCKS_ENABLE,
	JESD204_OP_LINK_ENABLE,
	JESD204_OP_LINK_RUNNING,
	JESD204_OP_OPT_POST_RUNNING_STAGE,

	__JESD204_MAX_OPS,
};

enum jesd204_state_op_mode {
	JESD204_STATE_OP_MODE_PER_LINK,
	JESD204_STATE_OP_MODE_PER_DEVICE,
};

enum jesd204_state_change_result {
	JESD204_STATE_CHANGE_ERROR = -1,
	JESD204_STATE_CHANGE_DEFER = 0,
	JESD204_STATE_CHANGE_DONE,
};

struct jesd204_dev_info {
	bool sysref_provider;
};

enum jesd204_sysref_mode {
	JESD204_SYSREF_DISABLED,
	JESD204_SYSREF_CONTINUOUS,
	JESD204_SYSREF_ONESHOT,
};

struct jesd204_sysref {
	enum jesd204_sysref_mode	mode;
	uint8_t				capture_falling_edge;
	uint8_t				valid_falling_edge;
	uint16_t			lmfc_offset;
};

struct jesd204_link_param {
	uint64_t sample_rate;
	uint32_t sample_rate_div;

	bool is_transmit;

	uint8_t num_lanes;
	uint8_t num_converters;
	uint8_t octets_per_frame;
	uint16_t frames_per_multiframe;
	uint8_t num_of_multiblocks_in_emb; /* E */

	uint8_t bits_per_sample;

	uint8_t converter_resolution;
	uint8_t jesd_version;
	uint8_t jesd_encoder;
	uint8_t subclass;

	uint8_t device_id;
	uint8_t bank_id;

	uint8_t scrambling;
	uint8_t high_density;

	uint8_t ctrl_words_per_frame_clk;
	uint8_t ctrl_bits_per_sample;
	uint8_t samples_per_conv_frame;

	uint8_t *lane_ids;

	struct jesd204_sysref sysref;

	/* Subclass 2 only */
	uint8_t dac_adj_resolution_steps;
	uint8_t dac_adj_direction;
	uint8_t dac_phase_adj;
};

struct jesd204_link {
	uint16_t link_id;
	const char *name;
	struct jesd204_link_param param;
};

struct jesd204_dev {
	uint32_t id;
	const char *name;
	struct jesd204_dev **inputs;
	uint16_t inputs_count;
	struct jesd204_dev **outputs;
	uint16_t outputs_count;
	struct jesd204_link **links;
	uint16_t links_count;
	struct jesd204_dev_info *info;
};


typedef int (*jesd204_dev_cb)(struct jesd204_dev *jdev,
			      enum jesd204_state_op_reason reason);

typedef int (*jesd204_link_cb)(struct jesd204_dev *jdev,
			       enum jesd204_state_op_reason,
			       struct jesd204_link *lnk);

struct jesd204_state_op {
	enum jesd204_state_op_mode	mode;
	jesd204_dev_cb			per_device;
	jesd204_link_cb			per_link;
	bool				post_state_sysref;
};

struct jesd204_dev_data {
	size_t					sizeof_priv;
	unsigned int				max_num_links;
	unsigned int				num_retries;
	struct jesd204_state_op			state_ops[__JESD204_MAX_OPS];
};

struct jesd204_init_arg {
	const struct jesd204_dev_data *data;
	enum jesd204_dev_op op;
	enum jesd204_state_op_reason reason;
	bool *per_device_ran;
	struct jesd204_link *link;
};

typedef int (*jesd204_iter_cb)(struct jesd204_dev *dev, void *arg);

int jesd204_link(const char *name, struct jesd204_link **link, struct jesd204_link_param param);
struct jesd204_dev * jesd204_device(const char *name, struct jesd204_dev *output,
		     struct jesd204_link *link, struct jesd204_dev_info *info);
int jesd204_connect(struct jesd204_dev *out, struct jesd204_dev *in, struct jesd204_link *link);
void jesd204_delete(struct jesd204_dev *dev);
int jesd204_init(struct jesd204_dev *jdev, const struct jesd204_dev_data *init_data);