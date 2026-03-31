#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include <string.h>
#include <errno.h>

#include "capi_i2c_linux_driver.h"

static int linux_i2c_init(struct capi_i2c_controller_handle **handle,
		   const struct capi_i2c_config *config);
static int linux_i2c_read(struct capi_i2c_device *device,
		   struct capi_i2c_transfer *transfer);
static int linux_i2c_write(struct capi_i2c_device *device,
		   struct capi_i2c_transfer *transfer);
static int linux_i2c_remove(struct capi_i2c_controller_handle *handle);

struct capi_i2c_ops linux_i2c_ops = {
    .init = linux_i2c_init,
    .deinit = linux_i2c_remove,
	.transmit = linux_i2c_write,
    .receive = linux_i2c_read,
	.register_callback = NULL,
	.transmit_async = NULL,
	.receive_async = NULL,
};

static int linux_i2c_init(struct capi_i2c_controller_handle **handle,
		   const struct capi_i2c_config *config)
{
    if (handle == NULL || config == NULL) {
		return -EINVAL;
	}

	struct linux_i2c_desc *linux_desc;
	char path[64];

    if (*handle == NULL) {
        *handle = (struct capi_i2c_controller_handle*) malloc(sizeof(
                struct capi_i2c_controller_handle));
        if (!*handle ) {
            return -ENOMEM;
        } else {
            (*handle)->init_allocated = true;
        }
    } else {
        (*handle)->init_allocated = false;
    }

    struct capi_i2c_controller_handle *i2c_h = *handle;

	linux_desc = (struct linux_i2c_desc*) malloc(sizeof(
				struct linux_i2c_desc));
	if (!linux_desc)
		goto free_desc;

    // Are we sure we need the descriptor on the struct, maybe wrapp struct controller in
    // linux_controller struct where where we can put this descriptor.(see stub implemenntation)

	i2c_h->priv = linux_desc;

	linux_desc->messages = NULL;
	linux_desc->len_messages = 0;

	snprintf(path, sizeof(path), "/dev/i2c-%d", (uint32_t)config->identifier);

	linux_desc->fd = open(path, O_RDWR);
	if (linux_desc->fd < 0) {
		printf("%s: Can't open %s\n\r", __func__, path);
		goto free;
	}
	*handle = i2c_h;

	return 0;

free:
	free(linux_desc);
free_desc:
	free(*handle);

	return -1;
}

static int linux_i2c_send_msg(struct capi_i2c_controller_handle *handle)
{
	struct linux_i2c_desc *linux_desc;
	struct i2c_rdwr_ioctl_data packets;
	int32_t ret;

	linux_desc = (struct linux_i2c_desc *)handle->priv;

	packets.msgs = linux_desc->messages;
	packets.nmsgs = linux_desc->len_messages;

	ret = ioctl(linux_desc->fd, I2C_RDWR, &packets); // ,microblaze
	// maillbox
	//
	if (ret <= 0) {
		printf("Error: ioctl ret=%d, errno=%d (%s), nmsgs=%d\n", 
			ret, errno, strerror(errno), packets.nmsgs);
		return -1;
	}
	if (ret <= 0) {  // True if error or no messages sent.
		printf("Error while sending message, error: %d\n", ret);
		return -1;
	}

	free(linux_desc->messages);
	linux_desc->len_messages = 0;
	linux_desc->messages = NULL;

	return 0;
}

/**
 * 
 * @brief Add I2C messages for a combined I2C R/W transaction.
 * @param desc - The I2C descriptor.
 * @param data - Buffer that stores the received/transmission data.
 * @param bytes_number - Number of bytes to read/write.
 * @param read - 0 to write, otherwise read.
 * @return 0 in case of success.
 */
static int linux_i2c_add_msg(struct capi_i2c_controller_handle *handle,
              uint16_t addr,
		      uint8_t *data,
		      uint8_t bytes_number,
		      uint8_t read)
{
	struct linux_i2c_desc *linux_desc;
	struct i2c_msg msg;
	void *ptr;

	linux_desc = (struct linux_i2c_desc *)handle->priv;

	msg.addr = addr;
	msg.len = bytes_number;
	msg.buf = data;

	if (read)
		msg.flags = I2C_M_RD;
	else
		msg.flags = 0;  // Write


	if (linux_desc->messages) {
		ptr = realloc(linux_desc->messages, sizeof(struct i2c_msg) *
			      (linux_desc->len_messages + 1));
		if (!ptr)
			return -ENOMEM;
		linux_desc->messages = (struct i2c_msg *)ptr;
	} else {
		linux_desc->messages = (struct i2c_msg *)malloc(sizeof(struct i2c_msg));
		linux_desc->len_messages = 0;
		if (!linux_desc->messages)
			return -ENOMEM;
	}

	linux_desc->messages[linux_desc->len_messages] = msg;
	linux_desc->len_messages++;

	return 0;
}

static int linux_i2c_read(struct capi_i2c_device *device, struct capi_i2c_transfer *transfer) {
    struct capi_i2c_controller_handle *i2c_h = device->controller;

    int32_t ret;

	if (transfer->sub_address && transfer->sub_address_len > 0) {
		ret = linux_i2c_add_msg(i2c_h, device->address,
			transfer->sub_address, transfer->sub_address_len , 0);
		if (ret)
			goto error_memory;
	}

	if (transfer->buf && transfer->len > 0) {
		ret = linux_i2c_add_msg(i2c_h, device->address, transfer->buf, transfer->len , 1);
		if (ret)
			goto error_memory;
	}

	if (!transfer->no_stop) {
		ret = linux_i2c_send_msg(i2c_h);
		if (ret)
			goto error_io;
	}

	return 0;

error_memory:
	printf("%s: Can't allocate memory\n\r", __func__);
	return -ENOMEM;
error_io:
	printf("%s: Can't read to file\n\r", __func__);
	return -EIO;
}

static int linux_i2c_write(struct capi_i2c_device *device, struct capi_i2c_transfer *transfer) {
    struct capi_i2c_controller_handle *i2c_h = device->controller;

    int32_t ret;

	if (transfer->sub_address && transfer->sub_address_len > 0) {
		ret = linux_i2c_add_msg(i2c_h, device->address,
			transfer->sub_address, transfer->sub_address_len , 0);
		if (ret)
			goto error_memory;
	}

	if (transfer->buf && transfer->len > 0) {
		ret = linux_i2c_add_msg(i2c_h, device->address, transfer->buf, transfer->len , 0);
		if (ret)
			goto error_memory;
	}

	if (!transfer->no_stop) {
		ret = linux_i2c_send_msg(i2c_h);
		if (ret)
			goto error_io;
	}

	return 0;

error_memory:
	printf("%s: Can't allocate memory\n\r", __func__);
	return -ENOMEM;
error_io:
	printf("%s: Can't write to file\n\r", __func__);
	return -EIO;
}

static int linux_i2c_remove(struct capi_i2c_controller_handle *handle)
{
	struct linux_i2c_desc *linux_desc;
	int32_t ret;

	linux_desc = (struct linux_i2c_desc *)handle->priv;

	ret = close(linux_desc->fd);
	if (ret) {
		printf("%s: Can't close device\n\r", __func__);
		return -1;
	}

	free(handle->priv);
	if (handle->init_allocated)
		free(handle);

	return 0;
}