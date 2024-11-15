/***************************************************************************//**
 *   @file   linux/linux_axi_io.c
 *   @brief  Implementation of AXI IO through UIO/devmem.
 *   @author Dragos Bogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include "no_os_error.h"
#include "no_os_axi_io.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief AXI IO through UIO read/write function.
 * @param base - UIO index (/dev/uioX).
 * @param offset - Address offset.
 * @param read - Location where read data will be stored.
 * @param write - Data to be written.
 * @return 0 in case of success, -1 otherwise.
 */
static int32_t uio_read_write(uint32_t base, uint32_t offset, uint32_t *read,
			      uint32_t *write)
{
	char buf[32];
	int ret;
	int uio_fd;
	int32_t status = 0;
	void *uio_addr;

	sprintf(buf, "/dev/uio%"PRIu32"", base);

	uio_fd = open(buf, O_RDWR);
	if (uio_fd < 0) {
		printf("%s: Can't open %s\n\r", __func__, buf);
		return -1;
	}

	uio_addr = mmap(NULL,
			offset + sizeof(*read),
			PROT_READ|PROT_WRITE,
			MAP_SHARED,
			uio_fd,
			0);
	if (uio_addr == MAP_FAILED) {
		printf("%s: mmap() failed\n\r", __func__);
		status = -1;
		goto close;
	}

	if (read)
		*read = *(uint32_t *)((uintptr_t)uio_addr + offset);
	if (write)
		*(uint32_t *)((uintptr_t)uio_addr + offset) = *write;

	ret = munmap(uio_addr, offset + sizeof(*read));
	if (ret < 0) {
		printf("%s: munmap() failed\n\r", __func__);
		status = -1;
	}

close:
	ret = close(uio_fd);
	if (ret < 0) {
		printf("%s: Can't close %s\n\r", __func__, buf);
		status = -1;
	}

	return status;
}

#ifdef DEVMEM
/**
 * @brief AXI IO through devmem read/write function.
 * @param base - Base address.
 * @param offset - Address offset.
 * @param read - Location where read data will be stored.
 * @param write - Data to be written.
 * @return 0 in case of success, -1 otherwise.
 */
static int32_t devmem_read_write(uint32_t base, uint32_t offset, uint32_t *read,
				 uint32_t *write)
{
	char command[64];
	char answer[64];
	FILE *stream;
	int32_t ret;

	if (write)
		ret = snprintf(command, sizeof(command),
			       "busybox devmem 0x%x w 0x%x", base + offset, *write);
	else if (read)
		ret = snprintf(command, sizeof(command),
			       "busybox devmem 0x%x", base + offset);
	else
		return -1;

	if (ret < 0 || ret >= (int32_t) sizeof(command))
		return -1;

	stream = popen(command, "r");
	if (stream == NULL)
		return -1;

	if (!(fgets(answer, sizeof(answer), stream))) {
		ret = -1;
		goto close;
	}

	if (read)
		*read = strtol(answer, NULL, 0);

	ret = 0;
close:
	pclose(stream);

	return ret;
}
#endif //DEVMEM
/**
 * @brief AXI IO through UIO/devmem read function.
 * @param base - UIO index (/dev/uioX)/base address.
 * @param offset - Address offset.
 * @param data - Location where read data will be stored.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_axi_io_read(uint32_t base, uint32_t offset, uint32_t *data)
{
#ifdef DEVMEM
	return devmem_read_write(base, offset, data, NULL);
#else
	return uio_read_write(base, offset, data, NULL);
#endif
}

/**
 * @brief AXI IO through UIO specific write function.
 * @param base - UIO index (/dev/uioX).
 * @param offset - Address offset.
 * @param data - Data to be written.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_axi_io_write(uint32_t base, uint32_t offset, uint32_t data)
{
#ifdef DEVMEM
	return devmem_read_write(base, offset, NULL, &data);
#else
	return uio_read_write(base, offset, NULL, &data);
#endif
}
