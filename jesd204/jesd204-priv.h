/**
 * The JESD204 framework
 *
 * Copyright (c) 2022 Analog Devices Inc.
 */

#ifndef _JESD204_PRIV_H_
#define _JESD204_PRIV_H_

#include "jesd204.h"

#define JESD204_MAX_LINKS	16

/**
 * struct jesd204_dev - JESD204 device
 * @dev_data		ref to data provided by the driver registering with the framework
 * @priv		private data to be returned to the driver
 * @is_top		true if this device is a top device in a topology of
 *			devices that make up a JESD204 link (typically the
 *			device that is the ADC, DAC, or transceiver)
 */
struct jesd204_dev {
	const struct jesd204_dev_data	*dev_data;
	void				*priv;

	bool				is_top;

	/* no-OS specific */
	struct jesd204_topology		*topology;
};

/**
 * struct jesd204_link_opaque - JESD204 link information (opaque part)
 * @link		public link information
 * @jdev_top		JESD204 top level this links belongs to
 * @link_idx		Index in the array of JESD204 links in @jdev_top
 */
struct jesd204_link_opaque {
	struct jesd204_link		link;
	struct jesd204_dev_top		*jdev_top;
	unsigned int			link_idx;
};

/**
 * struct jesd204_dev_top - JESD204 top device (in a JESD204 topology)
 * @jdev		JESD204 device data
 * @jdev_sysref		reference to the object that is the SYSREF provider for this topology
 * @jdev_sysref_sec	reference to the object that is the secondary SYSREF provider
 * 			for this topology, in case the primary jdev_sysref doesn't exist
 * @link_ids		JESD204 link IDs for this top-level device
 *			(connections should match against this)
 * @num_links		number of links
 * @active_links	active JESD204 link settings
 */
struct jesd204_dev_top {
	/* no-OS specific */
	struct jesd204_dev		*jdev;

	struct jesd204_dev		*jdev_sysref;
	struct jesd204_dev		*jdev_sysref_sec;

	unsigned int			link_ids[JESD204_MAX_LINKS];
	unsigned int			num_links;

	struct jesd204_link_opaque	*active_links;
};

struct jesd204_dev_top *jesd204_dev_get_topology_top_dev(
	struct jesd204_dev *jdev);

#endif /* _JESD204_PRIV_H_ */
