/***************************************************************************//**
 *   @file   linux/linux_axi_io.c
 *   @brief  Implementation of AXI IO through UIO/devmem.
 *   @author Dragos Bogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

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
