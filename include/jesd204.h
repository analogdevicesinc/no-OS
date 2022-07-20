/**
 * The JESD204 framework
 *
 * Copyright (c) 2022 Analog Devices Inc.
 */
#ifndef _JESD204_H_
#define _JESD204_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct jesd204_dev;

enum jesd204_subclass {
	JESD204_SUBCLASS_0,
	JESD204_SUBCLASS_1,
	JESD204_SUBCLASS_2,
};

enum jesd204_version {
	JESD204_VERSION_A,
	JESD204_VERSION_B,
	JESD204_VERSION_C,
};

/* JESD204C Supported encoding scheme */
enum jesd204_encoder {
	JESD204_ENCODER_UNKNOWN,
	JESD204_ENCODER_8B10B,
	JESD204_ENCODER_64B66B,
	JESD204_ENCODER_64B80B,

	JESD204_ENCODER_MAX
};

enum jesd204_sysref_mode {
	JESD204_SYSREF_DISABLED,
	JESD204_SYSREF_CONTINUOUS,
	JESD204_SYSREF_ONESHOT,
};

enum jesd204_state_change_result {
	JESD204_STATE_CHANGE_ERROR = -1,
	JESD204_STATE_CHANGE_DEFER = 0,
	JESD204_STATE_CHANGE_DONE,
};

#define JESD204_LINKS_ALL		((unsigned int)(-1))

#define JESD204_LMFC_OFFSET_UNINITIALIZED	((uint16_t)-1)

/** @struct jesd204_sysref
 * @brief JESD204 parameters for SYSREF
 * @param mode:			SYSREF mode (see jesd204_sysref_mode)
 * @param capture_falling_edge:	true if it should capture falling edge
 * @param valid_falling_edge:		true if falling edge should be valid
 * @param lmfc_offset:		offset for LMFC
 */
struct jesd204_sysref {
	enum jesd204_sysref_mode	mode;
	uint8_t				capture_falling_edge;
	uint8_t				valid_falling_edge;
	uint16_t			lmfc_offset;
};

/**
 * @struct jesd204_link
 * @brief JESD204 link configuration settings
 * @param link_id:			JESD204 link ID provided via DT configuration
 * @param error:			error code for this JESD204 link
 * @param is_transmit:			true if this link is transmit (digital to analog)
 * @param sample_rate:			sample rate for the link
 * @param sample_rate_div:		optional sample rate divider for the link
 * 					final rate = sample_rate / sample_rate_div
 * @param num_lanes:			number of JESD204 lanes (L)
 * @param num_converters:		number of converters per link (M)
 * @param octets_per_frame:		number of octets per frame (F)
 * @param frames_per_multiframe:	number of frames per frame (K)
 * @param num_of_multiblocks_in_emb:	number of multiblocks in extended multiblock (E) (JESD204C)
 * @param bits_per_sample:		number of bits per sample (N')
 * @param converter_resolution:		converter resolution (N)
 * @param jesd_version:			JESD204 version (A, B or C) (JESDV)
 * @param jesd_encoder:			JESD204C encoder (8B10B, 64B66B, 64B80B)
 * @param subclass:			JESD204 subclass (0,1 or 2) (SUBCLASSV)
 * @param device_id:			device ID (DID)
 * @param bank_id:			bank ID (BID)
 * @param scrambling:			true if scrambling enabled (SCR)
 * @param high_density:			true if high-density format is used (HD)
 * @param ctrl_words_per_frame_clk:	number of control words per frame clock
 *					period (CF)
 * @param ctrl_bits_per_sample:		number of control bits per sample (CS)
 * @param samples_per_conv_frame:	number of samples per converter per frame
 *					cycle (S)
 * @param lane_ids:			array of lane IDs (LID); note that this is an
 *					array the size of num_lanes
 * @param sysref:			JESD204 sysref config, see jesd204_sysref
 * @param dac_adj_resolution_steps:	number of adjustment resolution steps to adjust
 *					DAC LMFC (ADJCNT) - Subclass 2 only
 * @param dac_adj_direction:		direction to adjust DAC LMFC (ADJDIR)
 *					Subclass 2 only
 * @param dac_phase_adj:		true to do phase adjustment request to DAC
 *					Subclass 2 only
 */
struct jesd204_link {
	uint32_t link_id;
	int error;

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

enum jesd204_state_op_reason {
	JESD204_STATE_OP_REASON_INIT,
	JESD204_STATE_OP_REASON_UNINIT,
};

static inline const char *jesd204_state_op_reason_str(enum
		jesd204_state_op_reason reason)
{
	switch (reason) {
	case JESD204_STATE_OP_REASON_INIT:
		return "initialization";
	case JESD204_STATE_OP_REASON_UNINIT:
		return "uninitialization";
	default:
		return "unknown";
	}
}

typedef int (*jesd204_sysref_cb)(struct jesd204_dev *jdev);

typedef int (*jesd204_dev_cb)(struct jesd204_dev *jdev,
			      enum jesd204_state_op_reason reason);

typedef int (*jesd204_link_cb)(struct jesd204_dev *jdev,
			       enum jesd204_state_op_reason,
			       struct jesd204_link *lnk);

enum jesd204_state_op_mode {
	JESD204_STATE_OP_MODE_PER_LINK,
	JESD204_STATE_OP_MODE_PER_DEVICE,
};

/**
 * @struct jesd204_state_op
 * @brief JESD204 device per-state op
 * @param mode:		mode for this state op, depending on this per_device or per_link is called
 * @param per_device:		op called for each JESD204 **device** during a transition
 * @param per_link		op called for each JESD204 **link** individually during a transition
 * // FIXME: maybe pass 'struct jesd204_sysref' for post_state_sysref, to make this configurable? we'll see later
 * // FIXME: for now, the device should also be a top-level device, in case of multi-chip setups
 * @param post_state_sysref:	true if a SYSREF should be issued after the state change
 */
struct jesd204_state_op {
	enum jesd204_state_op_mode	mode;
	jesd204_dev_cb			per_device;
	jesd204_link_cb			per_link;
	bool				post_state_sysref;
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

/**
 * @struct jesd204_dev_data
 * @brief JESD204 device initialization data
 * @param sysref_cb:		SYSREF callback, if this device/driver supports it
 * @param sizeof_priv:		amount of data to allocate for private information
 * @param max_num_links:	maximum number of JESD204 links this device can support
 * @param num_retries:		number of retries in case of error (only for top-level device)
 * @param state_ops:		ops for each state transition of type @struct jesd204_state_op
 */
struct jesd204_dev_data {
	jesd204_sysref_cb			sysref_cb;
	size_t					sizeof_priv;
	unsigned int				max_num_links;
	unsigned int				num_retries;
	struct jesd204_state_op			state_ops[__JESD204_MAX_OPS];
};

/* no-OS specific */
#define JESD204_MAX_TOPOLOGY_LINKS	16

/* no-OS specific */
struct jesd204_topology_dev {
	struct jesd204_dev	*jdev;
	bool			is_top_device;
	bool			is_sysref_provider;
	unsigned int		link_ids[JESD204_MAX_TOPOLOGY_LINKS];
	unsigned int		links_number;
};

/* no-OS specific */
struct jesd204_topology {
	struct jesd204_dev_top		*dev_top;
	struct jesd204_topology_dev	*devs;
	unsigned int			devs_number;
};

/* no-OS specific */
int jesd204_dev_register(struct jesd204_dev **jdev,
			 const struct jesd204_dev_data *dev_data);

/* no-OS specific */
int jesd204_dev_unregister(struct jesd204_dev *jdev);

/* no-OS specific */
int jesd204_topology_init(struct jesd204_topology **topology,
			  struct jesd204_topology_dev *devs,
			  unsigned int devs_number);

/* no-OS specific */
int jesd204_topology_remove(struct jesd204_topology *topology);

/* no-OS specific */
int jesd204_fsm_start(struct jesd204_topology *topology, unsigned int link_idx);

/* no-OS specific */
int jesd204_fsm_stop(struct jesd204_topology *topology, unsigned int link_idx);

void *jesd204_dev_priv(struct jesd204_dev *jdev);

int jesd204_link_get_lmfc_lemc_rate(struct jesd204_link *lnk,
				    unsigned long *rate_hz);

int jesd204_link_get_rate_khz(struct jesd204_link *lnk,
			      unsigned long *lane_rate_khz);

int jesd204_link_get_device_clock(struct jesd204_link *lnk,
				  unsigned long *device_clock);

int jesd204_sysref_async(struct jesd204_dev *jdev);

int jesd204_sysref_async_force(struct jesd204_dev *jdev);

bool jesd204_dev_is_top(struct jesd204_dev *jdev);

void jesd204_copy_link_params(struct jesd204_link *dst,
			      const struct jesd204_link *src);

#endif
