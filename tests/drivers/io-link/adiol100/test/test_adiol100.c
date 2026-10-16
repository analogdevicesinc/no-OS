/***************************************************************************//**
 *   @file   test_adiol100.c
 *   @brief  Unit tests for the ADIOL100 IO-Link transceiver driver.
 *   @author Liviu Stan (liviu.stan@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "unity.h"
#include "adiol100.h"
#include "mock_no_os_spi.h"
#include "mock_no_os_gpio.h"
#include "mock_no_os_util.h"
#include "mock_no_os_alloc.h"
#include "mock_no_os_delay.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>

/* SPI register map for the mock chip. */
#define REG_MAP_DIRECT_SIZE	0x20
#define REG_MAP_PAGED_SIZE	0x100

#define CHIP_ADDR		1
#define ADIOL100_REG_FIELD_MSK	0x1F
#define GPIO_NONE		(-1)

#define REG(addr) (*((addr) >= ADIOL100_PAGED_BASE ? \
	&g_paged_regs[(addr) & 0xFF] : &g_direct_regs[(addr)]))

/**
 * @brief Fill every bit outside @p mask with @p fill (0 or 1); bits inside the
 * mask start at 0.
 *
 * Used before calling a function that must only modify @p mask, so that
 * ASSERT_OUTSIDE_UNCHANGED() can prove no other bit was clobbered.
 */
#define PRESET_OUTSIDE(reg, mask, fill) \
	(REG(reg) = (fill) ? (uint16_t)~(uint16_t)(mask) : 0)

/** @brief Assert that every bit outside @p mask still equals @p fill. */
#define ASSERT_OUTSIDE_UNCHANGED(reg, mask, fill) \
	TEST_ASSERT_BITS((uint16_t)~(uint16_t)(mask), \
			 (fill) ? (uint16_t)~(uint16_t)(mask) : 0, REG(reg))

static uint16_t g_direct_regs[REG_MAP_DIRECT_SIZE];
static uint16_t g_paged_regs[REG_MAP_PAGED_SIZE];

/** @brief FIFO TX buffer: captures raw bytes the driver writes to the FIFO. */
static uint8_t g_fifo_tx[ADIOL100_TX_MAX_LEN];
/** @brief Number of bytes captured in g_fifo_tx. */
static uint16_t g_fifo_tx_len;
/** @brief FIFO RX buffer: pre-filled by tests, served back to driver reads. */
static uint8_t g_fifo_rx[ADIOL100_RX_MAX_LEN];
/** @brief Number of valid bytes in g_fifo_rx. */
static uint16_t g_fifo_rx_len;

/** @brief Number of INTERRUPT_A reads so far during estcom polling. */
static int g_estcom_poll_count;
/** @brief Inject g_estcom_irq_val after this many INTERRUPT_A reads. */
static int g_estcom_trigger_at;
/** @brief IRQ flags to inject when g_estcom_poll_count reaches trigger. */
static uint16_t g_estcom_irq_val;

/** @brief Dummy SPI descriptor used by create_test_dev() and handed out by stub_spi_init(). */
static struct no_os_spi_desc g_spi_desc;
/** @brief Device returned by calloc in the init failure tests, so free() can be checked. */
static struct adiol100_dev g_init_dev;

/** @brief Fake GPIO descriptors, indexed by the init param's pin number. */
static struct no_os_gpio_desc g_gpio_descs[3];
/** @brief Pin number whose no_os_gpio_get() fails, or GPIO_NONE. */
static int g_gpio_get_fail = GPIO_NONE;
/** @brief Pin number whose no_os_gpio_direction_input() fails, or GPIO_NONE. */
static int g_gpio_dir_fail = GPIO_NONE;
/** @brief Number of no_os_gpio_remove() calls per descriptor. */
static int g_gpio_removed[3];

/**
 * @brief Allocate zeroed memory, forwarding to libc calloc.
 * @param nmemb - Number of elements.
 * @param size  - Size of each element.
 * @return Pointer to allocated memory or NULL.
 */
static void *stub_calloc(size_t nmemb, size_t size)
{
	return calloc(nmemb, size);
}

/**
 * @brief Free memory, forwarding to libc free.
 * @param ptr - Pointer to free.
 */
static void stub_free(void *ptr)
{
	free(ptr);
}

/**
 * @brief Prepare a value for a register field (shift val into mask position).
 * @param mask - Bitmask of the target field.
 * @param val  - Value to place into the field.
 * @return Value shifted and masked into position.
 */
static uint32_t stub_field_prep(uint32_t mask, uint32_t val)
{
	uint32_t shift = 0;
	if (mask)
		while (!(mask & (1u << shift)))
			shift++;
	return (val << shift) & mask;
}

/**
 * @brief Extract a field value from a register word.
 * @param mask - Bitmask of the target field.
 * @param word - Register value to extract from.
 * @return Field value shifted down to bit 0.
 */
static uint32_t stub_field_get(uint32_t mask, uint32_t word)
{
	uint32_t shift = 0;
	if (mask)
		while (!(mask & (1u << shift)))
			shift++;
	return (word & mask) >> shift;
}

/**
 * @brief Store a 16-bit value in big-endian byte order.
 * @param val - Value to store.
 * @param buf - Destination buffer (at least 2 bytes).
 */
static void stub_put_unaligned_be16(uint16_t val, uint8_t *buf)
{
	buf[0] = (val >> 8) & 0xFF;
	buf[1] = val & 0xFF;
}

/**
 * @brief Load a 16-bit value from big-endian byte order.
 * @param buf - Source buffer (at least 2 bytes).
 * @return Decoded 16-bit value.
 */
static uint16_t stub_get_unaligned_be16(uint8_t *buf)
{
	return ((uint16_t)buf[0] << 8) | buf[1];
}

/**
 * @brief Main SPI stub: decode command byte, route to virtual register map.
 * @param desc - SPI descriptor (unused).
 * @param buf  - SPI transfer buffer (command + data).
 * @param len  - Total transfer length in bytes.
 * @return 0 on success.
 */
static int32_t stub_spi(struct no_os_spi_desc *desc,
			uint8_t *buf, uint16_t len)
{
	uint8_t cmd = buf[0];
	uint8_t reg = (cmd >> ADIOL100_REG_SHIFT) & ADIOL100_REG_FIELD_MSK;
	uint8_t rw = cmd & ADIOL100_READ;

	(void)desc;

	/* FIFO write: capture what the driver sends */
	if ((reg == ADIOL100_REG_TXRXDATA_A ||
	     reg == ADIOL100_REG_TXRXDATA_B) && rw == ADIOL100_WRITE) {
		g_fifo_tx_len = len - 1;
		memcpy(g_fifo_tx, &buf[1], g_fifo_tx_len);
		return 0;
	}

	/* FIFO read: serve pre-filled RX data */
	if ((reg == ADIOL100_REG_TXRXDATA_A ||
	     reg == ADIOL100_REG_TXRXDATA_B) && rw == ADIOL100_READ) {
		memcpy(&buf[1], g_fifo_rx, g_fifo_rx_len);
		return 0;
	}

	/* Burst read: RXFIFOSTAT + INTERRUPT + STATUS + FIFO data */
	if ((reg == ADIOL100_REG_RXFIFOSTAT_A ||
	     reg == ADIOL100_REG_RXFIFOSTAT_B) && rw == ADIOL100_READ && len > 3) {
		uint8_t base = reg;
		stub_put_unaligned_be16(g_direct_regs[base], &buf[1]);
		stub_put_unaligned_be16(g_direct_regs[base + 1], &buf[3]);
		stub_put_unaligned_be16(g_direct_regs[base + 2], &buf[5]);
		memcpy(&buf[7], g_fifo_rx, g_fifo_rx_len);
		return 0;
	}

	if (reg == ADIOL100_REG_EXTRAPAGE) {
		uint8_t page_off = buf[1];
		if (rw == ADIOL100_READ) {
			stub_put_unaligned_be16(g_paged_regs[page_off],
						&buf[2]);
		} else {
			g_paged_regs[page_off] =
				stub_get_unaligned_be16(&buf[2]);
		}
	} else {
		if (rw == ADIOL100_READ) {
			stub_put_unaligned_be16(g_direct_regs[reg], &buf[1]);
		} else {
			g_direct_regs[reg] =
				stub_get_unaligned_be16(&buf[1]);
		}
	}

	return 0;
}

/**
 * @brief SPI stub that always returns -EIO (tests error propagation).
 * @param desc - SPI descriptor (unused).
 * @param buf  - SPI transfer buffer (unused).
 * @param len  - Transfer length (unused).
 * @return -EIO always.
 */
static int32_t stub_spi_fail(struct no_os_spi_desc *desc,
			     uint8_t *buf, uint16_t len)
{
	(void)desc;
	(void)buf;
	(void)len;
	return -EIO;
}

/**
 * @brief SPI stub that serves reads normally and fails every write.
 * @param desc - SPI descriptor (forwarded to stub_spi).
 * @param buf  - SPI transfer buffer.
 * @param len  - Transfer length.
 * @return -EIO for writes, stub_spi result for reads.
 */
static int32_t stub_spi_write_fail(struct no_os_spi_desc *desc,
				   uint8_t *buf, uint16_t len)
{
	if (!(buf[0] & ADIOL100_READ))
		return -EIO;

	return stub_spi(desc, buf, len);
}

/**
 * @brief EstCom SPI stub: wraps stub_spi, injects IRQ after N polls.
 * @param desc - SPI descriptor (forwarded to stub_spi).
 * @param buf  - SPI transfer buffer.
 * @param len  - Transfer length.
 * @return 0 on success.
 */
static int32_t stub_spi_estcom(struct no_os_spi_desc *desc,
			       uint8_t *buf, uint16_t len)
{
	uint8_t cmd = buf[0];
	uint8_t reg = (cmd >> ADIOL100_REG_SHIFT) & ADIOL100_REG_FIELD_MSK;
	uint8_t rw = cmd & ADIOL100_READ;

	if ((reg == ADIOL100_REG_INTERRUPT_A ||
	     reg == ADIOL100_REG_INTERRUPT_B) && rw == ADIOL100_READ) {
		g_estcom_poll_count++;
		if (g_estcom_poll_count >= g_estcom_trigger_at)
			g_direct_regs[reg] = g_estcom_irq_val;
	}

	return stub_spi(desc, buf, len);
}

/** @brief Hand out g_spi_desc so spi_remove() can be checked against it. */
static int32_t stub_spi_init(struct no_os_spi_desc **desc,
			     const struct no_os_spi_init_param *param)
{
	(void)param;
	*desc = &g_spi_desc;
	return 0;
}

/** @brief Hand out the fake descriptor for the requested pin, or fail. */
static int32_t stub_gpio_get(struct no_os_gpio_desc **desc,
			     const struct no_os_gpio_init_param *param)
{
	TEST_ASSERT_LESS_THAN(NO_OS_ARRAY_SIZE(g_gpio_descs), param->number);

	if ((int)param->number == g_gpio_get_fail)
		return -EIO;

	*desc = &g_gpio_descs[param->number];
	return 0;
}

/** @brief Fail for the descriptor of g_gpio_dir_fail, succeed otherwise. */
static int32_t stub_gpio_direction_input(struct no_os_gpio_desc *desc)
{
	if ((int)(desc - g_gpio_descs) == g_gpio_dir_fail)
		return -EIO;

	return 0;
}

/** @brief Count the removal of a descriptor. */
static int32_t stub_gpio_remove(struct no_os_gpio_desc *desc)
{
	g_gpio_removed[desc - g_gpio_descs]++;
	return 0;
}

/** @brief Install real util stubs so the driver computes correct register values. */
static void install_util_stubs(void)
{
	no_os_field_prep_Stub(stub_field_prep);
	no_os_field_get_Stub(stub_field_get);
	no_os_put_unaligned_be16_Stub(stub_put_unaligned_be16);
	no_os_get_unaligned_be16_Stub(stub_get_unaligned_be16);
}

/** @brief Install real alloc stubs so the driver can allocate/free memory. */
static void install_alloc_stubs(void)
{
	no_os_calloc_Stub(stub_calloc);
	no_os_free_Stub(stub_free);
}

/** @brief Zero all registers and FIFO buffers, then load datasheet reset values. */
static void reset_reg_map(void)
{
	memset(g_direct_regs, 0, sizeof(g_direct_regs));
	memset(g_paged_regs, 0, sizeof(g_paged_regs));
	memset(g_fifo_tx, 0, sizeof(g_fifo_tx));
	g_fifo_tx_len = 0;
	memset(g_fifo_rx, 0, sizeof(g_fifo_rx));
	g_fifo_rx_len = 0;

	/* Direct register reset values from datasheet */
	REG(ADIOL100_REG_FRAMCTRL2_A) = 0x0003;
	REG(ADIOL100_REG_FRAMCTRL2_B) = 0x0003;

	/* Paged register reset values from datasheet */
	REG(ADIOL100_REG_CLOCKCFG) = 0x0010;
	REG(ADIOL100_REG_REVISIONID) = 0x0001;
	REG(ADIOL100_REG_DEVICEID) = ADIOL100_DEVICE_ID;
	REG(ADIOL100_REG_SPIFIFO_A_BURST) = 0x6769;
	REG(ADIOL100_REG_SPIFIFO_B_BURST) = 0x6769;
	REG(ADIOL100_REG_CQACURMAX) = 0x0001;
	REG(ADIOL100_REG_LPACURMAX) = 0x0001;
	REG(ADIOL100_REG_CQBCURMAX) = 0x0001;
	REG(ADIOL100_REG_LPBCURMAX) = 0x0001;
}

/**
 * @brief Create a test device without going through adiol100_init().
 * @return Pointer to a heap-allocated device (caller must free).
 */
static struct adiol100_dev *create_test_dev(void)
{
	struct adiol100_dev *dev = calloc(1, sizeof(*dev));
	dev->spi_desc = &g_spi_desc;
	dev->chip_addr = CHIP_ADDR;
	return dev;
}

/** @brief Install every stub adiol100_init() needs except allocation. */
static void install_init_stubs(void)
{
	install_util_stubs();
	no_os_spi_init_Stub(stub_spi_init);
	no_os_spi_write_and_read_Stub(stub_spi);
	no_os_gpio_get_Stub(stub_gpio_get);
	no_os_gpio_direction_input_Stub(stub_gpio_direction_input);
	no_os_gpio_remove_Stub(stub_gpio_remove);
}

/**
 * @brief Expect a failing adiol100_init() to allocate the device, then release
 * the SPI bus and free the device.
 */
static void expect_init_failure_cleanup(void)
{
	no_os_calloc_ExpectAndReturn(1, sizeof(struct adiol100_dev), &g_init_dev);
	no_os_spi_remove_ExpectAndReturn(&g_spi_desc, 0);
	no_os_free_Expect(&g_init_dev);
}

/** @brief Unity setUp: reset virtual register map, GPIO stubs and init device before each test. */
void setUp(void)
{
	reset_reg_map();
	g_gpio_get_fail = GPIO_NONE;
	g_gpio_dir_fail = GPIO_NONE;
	memset(g_gpio_removed, 0, sizeof(g_gpio_removed));
	memset(&g_init_dev, 0, sizeof(g_init_dev));
}

/** @brief Unity tearDown: no-op. */
void tearDown(void)
{
}

/* ---- Init --------------------------------------------------------------- */

/** @brief Test that adiol100_init() returns -EINVAL for NULL dev pointer. */
void test_adiol100_init_null_dev(void)
{
	struct adiol100_init_param ip = { .chip_addr = CHIP_ADDR };
	int ret = adiol100_init(NULL, &ip);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

/** @brief Test that adiol100_init() returns -EINVAL for NULL init params. */
void test_adiol100_init_null_ip(void)
{
	struct adiol100_dev *dev;
	int ret = adiol100_init(&dev, NULL);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

/** @brief Test that adiol100_init() returns -ENOMEM when calloc fails. */
void test_adiol100_init_enomem(void)
{
	struct adiol100_dev *dev;
	struct adiol100_init_param ip = { .chip_addr = CHIP_ADDR };

	no_os_calloc_IgnoreAndReturn(NULL);

	int ret = adiol100_init(&dev, &ip);
	TEST_ASSERT_EQUAL_INT(-ENOMEM, ret);
}

/** @brief Test that adiol100_init() propagates SPI init failure. */
void test_adiol100_init_spi_fail(void)
{
	struct adiol100_dev *dev;
	struct adiol100_init_param ip = { .chip_addr = CHIP_ADDR };

	/* Nothing to release but the device: spi_remove must not be called */
	no_os_calloc_ExpectAndReturn(1, sizeof(struct adiol100_dev), &g_init_dev);
	no_os_spi_init_IgnoreAndReturn(-EIO);
	no_os_free_Expect(&g_init_dev);

	int ret = adiol100_init(&dev, &ip);
	TEST_ASSERT_EQUAL_INT(-EIO, ret);
}

/** @brief Test that adiol100_init() rejects chip_addr > CHIP_ADDR_MAX. */
void test_adiol100_init_bad_chip_addr(void)
{
	struct adiol100_dev *dev;
	struct adiol100_init_param ip = { .chip_addr = 5 };

	install_init_stubs();
	expect_init_failure_cleanup();

	int ret = adiol100_init(&dev, &ip);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

/** @brief Test that adiol100_init() returns -ENODEV when REVID is zero. */
void test_adiol100_init_no_device(void)
{
	struct adiol100_dev *dev;
	struct adiol100_init_param ip = { .chip_addr = CHIP_ADDR };

	/* REVID = 0 means chip not responding */
	REG(ADIOL100_REG_REVISIONID) = 0x0000;

	install_init_stubs();
	expect_init_failure_cleanup();

	int ret = adiol100_init(&dev, &ip);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/** @brief Test that adiol100_init() returns -ENODEV for unrecognized device ID. */
void test_adiol100_init_bad_devid(void)
{
	struct adiol100_dev *dev;
	struct adiol100_init_param ip = { .chip_addr = CHIP_ADDR };

	/* REVID keeps its valid reset value, DEVID is unknown */
	REG(ADIOL100_REG_DEVICEID) = 0xBEEF;

	install_init_stubs();
	expect_init_failure_cleanup();

	int ret = adiol100_init(&dev, &ip);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/** @brief Test that adiol100_init() succeeds with valid params and stores chip_addr. */
void test_adiol100_init_success(void)
{
	struct adiol100_dev *dev;
	struct adiol100_init_param ip = {
		.chip_addr = CHIP_ADDR,
		.clock_src = ADIOL100_CLK_CRYSTAL,
		.clk_div = 0,
	};

	install_alloc_stubs();
	install_init_stubs();

	int ret = adiol100_init(&dev, &ip);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_NOT_NULL(dev);
	TEST_ASSERT_EQUAL_UINT8(CHIP_ADDR, dev->chip_addr);

	no_os_spi_remove_ExpectAndReturn(&g_spi_desc, 0);
	adiol100_remove(dev);
}

/** @brief Test that adiol100_init() accepts the ADIOL101 device ID. */
void test_adiol100_init_success_adiol101(void)
{
	struct adiol100_dev *dev;
	struct adiol100_init_param ip = {
		.chip_addr = CHIP_ADDR,
		.clock_src = ADIOL100_CLK_CRYSTAL,
		.clk_div = 0,
	};

	REG(ADIOL100_REG_DEVICEID) = ADIOL101_DEVICE_ID;

	install_alloc_stubs();
	install_init_stubs();

	int ret = adiol100_init(&dev, &ip);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_NOT_NULL(dev);
	TEST_ASSERT_EQUAL_UINT8(CHIP_ADDR, dev->chip_addr);

	no_os_spi_remove_ExpectAndReturn(&g_spi_desc, 0);
	adiol100_remove(dev);
}

/** @brief Test that adiol100_init() stores each IRQ GPIO in its own field and remove() releases all three. */
void test_adiol100_init_gpio_all(void)
{
	struct adiol100_dev *dev;
	struct no_os_gpio_init_param g = { .number = 0 };
	struct no_os_gpio_init_param a = { .number = 1 };
	struct no_os_gpio_init_param b = { .number = 2 };
	struct adiol100_init_param ip = {
		.chip_addr = CHIP_ADDR,
		.clock_src = ADIOL100_CLK_CRYSTAL,
		.gpio_irqg = &g,
		.gpio_irqa = &a,
		.gpio_irqb = &b,
	};

	install_init_stubs();
	install_alloc_stubs();

	int ret = adiol100_init(&dev, &ip);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_EQUAL_PTR(&g_gpio_descs[0], dev->gpio_irqg);
	TEST_ASSERT_EQUAL_PTR(&g_gpio_descs[1], dev->gpio_irqa);
	TEST_ASSERT_EQUAL_PTR(&g_gpio_descs[2], dev->gpio_irqb);
	TEST_ASSERT_EQUAL_INT(0, g_gpio_removed[0]);	/* irqg */
	TEST_ASSERT_EQUAL_INT(0, g_gpio_removed[1]);	/* irqa */
	TEST_ASSERT_EQUAL_INT(0, g_gpio_removed[2]);	/* irqb */

	no_os_spi_remove_ExpectAndReturn(&g_spi_desc, 0);
	adiol100_remove(dev);
	TEST_ASSERT_EQUAL_INT(1, g_gpio_removed[0]);	/* irqg */
	TEST_ASSERT_EQUAL_INT(1, g_gpio_removed[1]);	/* irqa */
	TEST_ASSERT_EQUAL_INT(1, g_gpio_removed[2]);	/* irqb */
}

/** @brief Test that adiol100_init() releases nothing when the irqg GPIO fails. */
void test_adiol100_init_gpio_irqg_fail(void)
{
	struct adiol100_dev *dev;
	struct no_os_gpio_init_param g = { .number = 0 };
	struct no_os_gpio_init_param a = { .number = 1 };
	struct no_os_gpio_init_param b = { .number = 2 };
	struct adiol100_init_param ip = {
		.chip_addr = CHIP_ADDR,
		.clock_src = ADIOL100_CLK_CRYSTAL,
		.gpio_irqg = &g,
		.gpio_irqa = &a,
		.gpio_irqb = &b,
	};

	install_init_stubs();
	expect_init_failure_cleanup();
	g_gpio_get_fail = 0;

	int ret = adiol100_init(&dev, &ip);
	TEST_ASSERT_EQUAL_INT(-EIO, ret);
	TEST_ASSERT_EQUAL_INT(0, g_gpio_removed[0]);	/* irqg */
	TEST_ASSERT_EQUAL_INT(0, g_gpio_removed[1]);	/* irqa */
	TEST_ASSERT_EQUAL_INT(0, g_gpio_removed[2]);	/* irqb */
}

/** @brief Test that adiol100_init() releases irqg when the irqa GPIO fails. */
void test_adiol100_init_gpio_irqa_fail(void)
{
	struct adiol100_dev *dev;
	struct no_os_gpio_init_param g = { .number = 0 };
	struct no_os_gpio_init_param a = { .number = 1 };
	struct no_os_gpio_init_param b = { .number = 2 };
	struct adiol100_init_param ip = {
		.chip_addr = CHIP_ADDR,
		.clock_src = ADIOL100_CLK_CRYSTAL,
		.gpio_irqg = &g,
		.gpio_irqa = &a,
		.gpio_irqb = &b,
	};

	install_init_stubs();
	expect_init_failure_cleanup();
	g_gpio_get_fail = 1;

	int ret = adiol100_init(&dev, &ip);
	TEST_ASSERT_EQUAL_INT(-EIO, ret);
	TEST_ASSERT_EQUAL_INT(1, g_gpio_removed[0]);	/* irqg */
	TEST_ASSERT_EQUAL_INT(0, g_gpio_removed[1]);	/* irqa */
	TEST_ASSERT_EQUAL_INT(0, g_gpio_removed[2]);	/* irqb */
}

/** @brief Test that adiol100_init() releases irqg and irqa when the irqb GPIO fails. */
void test_adiol100_init_gpio_irqb_fail(void)
{
	struct adiol100_dev *dev;
	struct no_os_gpio_init_param g = { .number = 0 };
	struct no_os_gpio_init_param a = { .number = 1 };
	struct no_os_gpio_init_param b = { .number = 2 };
	struct adiol100_init_param ip = {
		.chip_addr = CHIP_ADDR,
		.clock_src = ADIOL100_CLK_CRYSTAL,
		.gpio_irqg = &g,
		.gpio_irqa = &a,
		.gpio_irqb = &b,
	};

	install_init_stubs();
	expect_init_failure_cleanup();
	g_gpio_get_fail = 2;

	int ret = adiol100_init(&dev, &ip);
	TEST_ASSERT_EQUAL_INT(-EIO, ret);
	TEST_ASSERT_EQUAL_INT(1, g_gpio_removed[0]);	/* irqg */
	TEST_ASSERT_EQUAL_INT(1, g_gpio_removed[1]);	/* irqa */
	TEST_ASSERT_EQUAL_INT(0, g_gpio_removed[2]);	/* irqb */
}

/** @brief Test that adiol100_init() releases the irqg pin exactly once when irqg direction_input fails. */
void test_adiol100_init_gpio_irqg_direction_fail(void)
{
	struct adiol100_dev *dev;
	struct no_os_gpio_init_param g = { .number = 0 };
	struct no_os_gpio_init_param a = { .number = 1 };
	struct no_os_gpio_init_param b = { .number = 2 };
	struct adiol100_init_param ip = {
		.chip_addr = CHIP_ADDR,
		.clock_src = ADIOL100_CLK_CRYSTAL,
		.gpio_irqg = &g,
		.gpio_irqa = &a,
		.gpio_irqb = &b,
	};

	install_init_stubs();
	expect_init_failure_cleanup();
	g_gpio_dir_fail = 0;

	int ret = adiol100_init(&dev, &ip);
	TEST_ASSERT_EQUAL_INT(-EIO, ret);
	TEST_ASSERT_EQUAL_INT(1, g_gpio_removed[0]);	/* irqg */
	TEST_ASSERT_EQUAL_INT(0, g_gpio_removed[1]);	/* irqa */
	TEST_ASSERT_EQUAL_INT(0, g_gpio_removed[2]);	/* irqb */
}

/** @brief Test that adiol100_init() releases the failing irqa pin once and irqg when irqa direction_input fails. */
void test_adiol100_init_gpio_irqa_direction_fail(void)
{
	struct adiol100_dev *dev;
	struct no_os_gpio_init_param g = { .number = 0 };
	struct no_os_gpio_init_param a = { .number = 1 };
	struct no_os_gpio_init_param b = { .number = 2 };
	struct adiol100_init_param ip = {
		.chip_addr = CHIP_ADDR,
		.clock_src = ADIOL100_CLK_CRYSTAL,
		.gpio_irqg = &g,
		.gpio_irqa = &a,
		.gpio_irqb = &b,
	};

	install_init_stubs();
	expect_init_failure_cleanup();
	g_gpio_dir_fail = 1;

	int ret = adiol100_init(&dev, &ip);
	TEST_ASSERT_EQUAL_INT(-EIO, ret);
	TEST_ASSERT_EQUAL_INT(1, g_gpio_removed[0]);	/* irqg */
	TEST_ASSERT_EQUAL_INT(1, g_gpio_removed[1]);	/* irqa */
	TEST_ASSERT_EQUAL_INT(0, g_gpio_removed[2]);	/* irqb */
}

/** @brief Test that adiol100_init() releases the failing irqb pin once, plus irqg and irqa, when irqb direction_input fails. */
void test_adiol100_init_gpio_irqb_direction_fail(void)
{
	struct adiol100_dev *dev;
	struct no_os_gpio_init_param g = { .number = 0 };
	struct no_os_gpio_init_param a = { .number = 1 };
	struct no_os_gpio_init_param b = { .number = 2 };
	struct adiol100_init_param ip = {
		.chip_addr = CHIP_ADDR,
		.clock_src = ADIOL100_CLK_CRYSTAL,
		.gpio_irqg = &g,
		.gpio_irqa = &a,
		.gpio_irqb = &b,
	};

	install_init_stubs();
	expect_init_failure_cleanup();
	g_gpio_dir_fail = 2;

	int ret = adiol100_init(&dev, &ip);
	TEST_ASSERT_EQUAL_INT(-EIO, ret);
	TEST_ASSERT_EQUAL_INT(1, g_gpio_removed[0]);	/* irqg */
	TEST_ASSERT_EQUAL_INT(1, g_gpio_removed[1]);	/* irqa */
	TEST_ASSERT_EQUAL_INT(1, g_gpio_removed[2]);	/* irqb */
}

/* ---- Remove ------------------------------------------------------------- */

/** @brief Test that adiol100_remove() returns -ENODEV for NULL dev pointer. */
void test_adiol100_remove_null_dev(void)
{
	int ret = adiol100_remove(NULL);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/** @brief Test that adiol100_remove() frees SPI and device memory. */
void test_adiol100_remove_success(void)
{
	struct adiol100_dev *dev = create_test_dev();

	no_os_spi_remove_IgnoreAndReturn(0);
	no_os_free_Stub(stub_free);

	int ret = adiol100_remove(dev);
	TEST_ASSERT_EQUAL_INT(0, ret);
}

/* ---- Read / Write / Update ---------------------------------------------- */

/** @brief Test that adiol100_read() returns -ENODEV for NULL dev pointer. */
void test_adiol100_read_null_dev(void)
{
	uint16_t val;
	int ret = adiol100_read(NULL, 0x0000, &val);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/** @brief Test that adiol100_read() returns -EINVAL for NULL value pointer. */
void test_adiol100_read_null_value(void)
{
	struct adiol100_dev *dev = create_test_dev();
	int ret = adiol100_read(dev, 0x0000, NULL);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

/** @brief Test that adiol100_read() rejects invalid register addresses. */
void test_adiol100_read_invalid_reg(void)
{
	struct adiol100_dev *dev = create_test_dev();

	uint16_t val;
	int ret = adiol100_read(dev, ADIOL100_REG_EXTRAPAGE, &val);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);

	free(dev);
}

/** @brief Test that adiol100_read() decodes a direct register via SPI round-trip. */
void test_adiol100_read_direct(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	REG(ADIOL100_REG_STATUS_A) = 0xABCD;

	uint16_t val;
	int ret = adiol100_read(dev, ADIOL100_REG_STATUS_A, &val);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_EQUAL_HEX16(0xABCD, val);

	free(dev);
}

/** @brief Test that adiol100_read() decodes a paged register via EXTRAPAGE. */
void test_adiol100_read_paged(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	REG(ADIOL100_REG_REVISIONID) = 0x0042;

	uint16_t val;
	int ret = adiol100_read(dev, ADIOL100_REG_REVISIONID, &val);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_EQUAL_HEX16(0x0042, val);

	free(dev);
}

/** @brief Test that adiol100_read() propagates SPI transfer failure. */
void test_adiol100_read_spi_fail(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi_fail);

	uint16_t val;
	int ret = adiol100_read(dev, ADIOL100_REG_STATUS_A, &val);
	TEST_ASSERT_EQUAL_INT(-EIO, ret);

	free(dev);
}

/** @brief Test that adiol100_write() returns -ENODEV for NULL dev pointer. */
void test_adiol100_write_null_dev(void)
{
	int ret = adiol100_write(NULL, 0x0000, 0x1234);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/** @brief Test that adiol100_write() rejects invalid register addresses. */
void test_adiol100_write_invalid_reg(void)
{
	struct adiol100_dev *dev = create_test_dev();

	int ret = adiol100_write(dev, ADIOL100_REG_EXTRAPAGE, 0x1234);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);

	free(dev);
}

/** @brief Test that adiol100_write() propagates SPI transfer failure. */
void test_adiol100_write_spi_fail(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi_fail);

	int ret = adiol100_write(dev, ADIOL100_REG_INTEN_A, 0x1234);
	TEST_ASSERT_EQUAL_INT(-EIO, ret);

	free(dev);
}

/** @brief Test that adiol100_update() propagates a read failure without writing. */
void test_adiol100_update_read_fail(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi_fail);

	REG(ADIOL100_REG_INTEN_A) = 0x1234;

	int ret = adiol100_update(dev, ADIOL100_REG_INTEN_A, 0x00FF, 0x00AA);
	TEST_ASSERT_EQUAL_INT(-EIO, ret);
	TEST_ASSERT_EQUAL_HEX16(0x1234, REG(ADIOL100_REG_INTEN_A));

	free(dev);
}

/** @brief Test that adiol100_update() propagates a write failure. */
void test_adiol100_update_write_fail(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi_write_fail);

	REG(ADIOL100_REG_INTEN_A) = 0x1234;

	int ret = adiol100_update(dev, ADIOL100_REG_INTEN_A, 0x00FF, 0x00AA);
	TEST_ASSERT_EQUAL_INT(-EIO, ret);
	TEST_ASSERT_EQUAL_HEX16(0x1234, REG(ADIOL100_REG_INTEN_A));

	free(dev);
}

/** @brief Test that adiol100_write() stores a value in a direct register. */
void test_adiol100_write_direct(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	int ret = adiol100_write(dev, ADIOL100_REG_INTEN_A, 0x1234);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_EQUAL_HEX16(0x1234, REG(ADIOL100_REG_INTEN_A));

	free(dev);
}

/** @brief Test that adiol100_write() stores a value in a paged register via EXTRAPAGE. */
void test_adiol100_write_paged(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	int ret = adiol100_write(dev, ADIOL100_REG_INTERRUPTG_EN, 0xFF00);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_EQUAL_HEX16(0xFF00, REG(ADIOL100_REG_INTERRUPTG_EN));

	free(dev);
}

/** @brief Test that adiol100_update() returns -ENODEV for NULL dev pointer. */
void test_adiol100_update_null_dev(void)
{
	int ret = adiol100_update(NULL, 0x0000, 0xFFFF, 0x1234);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/** @brief Test that adiol100_update() performs correct read-modify-write. */
void test_adiol100_update_rmw(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	/* Set initial value */
	REG(ADIOL100_REG_FRAMCTRL1_A) = 0xFF00;

	/* Update bits [7:6] (COMRT) to 0x03 */
	int ret = adiol100_update(dev, ADIOL100_REG_FRAMCTRL1_A,
				  ADIOL100_COMRT_MSK,
				  stub_field_prep(ADIOL100_COMRT_MSK, 3));
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_EQUAL_HEX16(0xFFC0, REG(ADIOL100_REG_FRAMCTRL1_A));

	free(dev);
}

/** @brief Test that adiol100_update() preserves bits outside the mask, from all ones and all zeros. */
void test_adiol100_update_preserves_other_bits(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	REG(ADIOL100_REG_CQSET_A) = 0xFFFF;

	/* Clear only DRVEN bit */
	int ret = adiol100_update(dev, ADIOL100_REG_CQSET_A,
				  ADIOL100_DRVEN, 0);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_EQUAL_HEX16(0xFFDF, REG(ADIOL100_REG_CQSET_A));

	/* Same from all zeros: set only DRVEN */
	REG(ADIOL100_REG_CQSET_A) = 0x0000;

	ret = adiol100_update(dev, ADIOL100_REG_CQSET_A,
			      ADIOL100_DRVEN, ADIOL100_DRVEN);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_EQUAL_HEX16(ADIOL100_DRVEN, REG(ADIOL100_REG_CQSET_A));

	free(dev);
}

/* ---- RevID / DevID ------------------------------------------------------ */

/** @brief Test that adiol100_read_revid() returns -ENODEV for NULL dev pointer. */
void test_adiol100_read_revid_null_dev(void)
{
	uint16_t rev;
	int ret = adiol100_read_revid(NULL, &rev);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/** @brief Test that adiol100_read_revid() returns -EINVAL for NULL output pointer. */
void test_adiol100_read_revid_null_val(void)
{
	struct adiol100_dev *dev = create_test_dev();
	int ret = adiol100_read_revid(dev, NULL);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

/** @brief Test that adiol100_read_revid() returns the REVISIONID register value. */
void test_adiol100_read_revid(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	REG(ADIOL100_REG_REVISIONID) = 0x0042;

	uint16_t rev;
	int ret = adiol100_read_revid(dev, &rev);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_EQUAL_HEX16(0x0042, rev);

	free(dev);
}

/** @brief Test that adiol100_read_devid() returns -ENODEV for NULL dev pointer. */
void test_adiol100_read_devid_null_dev(void)
{
	uint16_t id;
	int ret = adiol100_read_devid(NULL, &id);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/** @brief Test that adiol100_read_devid() returns -EINVAL for NULL output pointer. */
void test_adiol100_read_devid_null_val(void)
{
	struct adiol100_dev *dev = create_test_dev();
	int ret = adiol100_read_devid(dev, NULL);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

/** @brief Test that adiol100_read_devid() returns the DEVICEID register value. */
void test_adiol100_read_devid(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	REG(ADIOL100_REG_DEVICEID) = 0x1234;

	uint16_t id;
	int ret = adiol100_read_devid(dev, &id);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_EQUAL_HEX16(0x1234, id);

	free(dev);
}

/* ---- Clock -------------------------------------------------------------- */

/** @brief Test that adiol100_setup_clock() returns -ENODEV for NULL dev pointer. */
void test_adiol100_setup_clock_null_dev(void)
{
	int ret = adiol100_setup_clock(NULL, ADIOL100_CLK_CRYSTAL, 0);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/** @brief Test that adiol100_setup_clock() writes CLOCKCFG, CLKOEN, and CLKDIV fields. */
void test_adiol100_setup_clock(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	/* The reset value already has CLKOEN set; start from 0 so it must be set */
	REG(ADIOL100_REG_CLOCKCFG) = 0;

	int ret = adiol100_setup_clock(dev, ADIOL100_CLK_CRYSTAL, 2);
	TEST_ASSERT_EQUAL_INT(0, ret);

	uint16_t clkcfg = REG(ADIOL100_REG_CLOCKCFG);
	TEST_ASSERT_EQUAL_UINT8(ADIOL100_CLK_CRYSTAL,
				stub_field_get(ADIOL100_CLOCKCFG_MSK, clkcfg));
	TEST_ASSERT_BITS(ADIOL100_CLKOEN, ADIOL100_CLKOEN, clkcfg);
	TEST_ASSERT_EQUAL_UINT8(2,
				stub_field_get(ADIOL100_CLKDIV_MSK, clkcfg));

	free(dev);
}

/** @brief Test that adiol100_setup_clock() leaves bits outside CLKOEN/CLKDIV/CLOCKCFG untouched. */
void test_adiol100_setup_clock_preserves_other_bits(void)
{
	struct adiol100_dev *dev = create_test_dev();
	int ret;
	const uint16_t mask = ADIOL100_CLKOEN | ADIOL100_CLKDIV_MSK |
			      ADIOL100_CLOCKCFG_MSK;

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	PRESET_OUTSIDE(ADIOL100_REG_CLOCKCFG, mask, 1);
	ret = adiol100_setup_clock(dev, ADIOL100_CLK_CRYSTAL, 2);
	TEST_ASSERT_EQUAL_INT(0, ret);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_CLOCKCFG, mask, 1);

	PRESET_OUTSIDE(ADIOL100_REG_CLOCKCFG, mask, 0);
	ret = adiol100_setup_clock(dev, ADIOL100_CLK_CRYSTAL, 2);
	TEST_ASSERT_EQUAL_INT(0, ret);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_CLOCKCFG, mask, 0);

	free(dev);
}

/* ---- Reset -------------------------------------------------------------- */

/** @brief Test that adiol100_global_reset() returns -ENODEV for NULL dev pointer. */
void test_adiol100_global_reset_null_dev(void)
{
	int ret = adiol100_global_reset(NULL);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/** @brief Test that adiol100_global_reset() clears INTERRUPTG_EN, TRIGGER, and all LEDs. */
void test_adiol100_global_reset(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	/* Pre-set registers to non-zero so we verify they get cleared */
	REG(ADIOL100_REG_INTERRUPTG_EN) = 0xFFFF;
	REG(ADIOL100_REG_TRIGGER) = 0xFFFF;
	REG(ADIOL100_REG_INTERRUPTG) = 0xFFFF;
	REG(ADIOL100_REG_INTERRUPT_A) = 0xFFFF;
	REG(ADIOL100_REG_INTERRUPT_B) = 0xFFFF;
	REG(ADIOL100_REG_LED1RSEQ) = 0xFFFF;
	REG(ADIOL100_REG_LED1GSEQ) = 0xFFFF;

	int ret = adiol100_global_reset(dev);
	TEST_ASSERT_EQUAL_INT(0, ret);
	/* Verify INTERRUPTG_EN was cleared */
	TEST_ASSERT_EQUAL_HEX16(0x0000, REG(ADIOL100_REG_INTERRUPTG_EN));
	/* Verify TRIGGER was cleared */
	TEST_ASSERT_EQUAL_HEX16(0x0000, REG(ADIOL100_REG_TRIGGER));
	/* Verify reset_leds cleared all LED sequences and set modes to BLINK */
	TEST_ASSERT_EQUAL_HEX16(ADIOL100_LED_SEQ_OFF, REG(ADIOL100_REG_LED1RSEQ));
	TEST_ASSERT_EQUAL_HEX16(ADIOL100_LED_SEQ_OFF, REG(ADIOL100_REG_LED1GSEQ));
	TEST_ASSERT_EQUAL_HEX16(ADIOL100_LED_SEQ_OFF, REG(ADIOL100_REG_LED2RSEQ));
	TEST_ASSERT_EQUAL_HEX16(ADIOL100_LED_SEQ_OFF, REG(ADIOL100_REG_LED2GSEQ));
	TEST_ASSERT_EQUAL_HEX16(ADIOL100_LED_SEQ_OFF, REG(ADIOL100_REG_LED3RSEQ));
	TEST_ASSERT_EQUAL_HEX16(ADIOL100_LED_SEQ_OFF, REG(ADIOL100_REG_LED3GSEQ));
	TEST_ASSERT_EQUAL_HEX16(ADIOL100_LED_SEQ_OFF, REG(ADIOL100_REG_LED4RSEQ));
	TEST_ASSERT_EQUAL_HEX16(ADIOL100_LED_SEQ_OFF, REG(ADIOL100_REG_LED4GSEQ));
	uint16_t ledcfg = REG(ADIOL100_REG_LEDCONFIG);
	TEST_ASSERT_EQUAL_UINT8(ADIOL100_LED_BLINK,
				stub_field_get(ADIOL100_LED1CFG_MSK, ledcfg));
	TEST_ASSERT_EQUAL_UINT8(ADIOL100_LED_BLINK,
				stub_field_get(ADIOL100_LED2CFG_MSK, ledcfg));
	TEST_ASSERT_EQUAL_UINT8(ADIOL100_LED_BLINK,
				stub_field_get(ADIOL100_LED3CFG_MSK, ledcfg));
	TEST_ASSERT_EQUAL_UINT8(ADIOL100_LED_BLINK,
				stub_field_get(ADIOL100_LED4CFG_MSK, ledcfg));

	free(dev);
}

/** @brief Test that adiol100_reset_channel() returns -ENODEV for NULL dev pointer. */
void test_adiol100_reset_channel_null_dev(void)
{
	int ret = adiol100_reset_channel(NULL, ADIOL100_CH_A);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/** @brief Test that adiol100_reset_channel() writes CHANRST to FRAMCTRL2_A. */
void test_adiol100_reset_channel_ch_a(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	int ret = adiol100_reset_channel(dev, ADIOL100_CH_A);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_EQUAL_HEX16(ADIOL100_CHANRST, REG(ADIOL100_REG_FRAMCTRL2_A));

	free(dev);
}

/** @brief Test that adiol100_reset_channel() routes to FRAMCTRL2_B for channel B. */
void test_adiol100_reset_channel_ch_b(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	int ret = adiol100_reset_channel(dev, ADIOL100_CH_B);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_EQUAL_HEX16(ADIOL100_CHANRST, REG(ADIOL100_REG_FRAMCTRL2_B));

	free(dev);
}

/** @brief Test that adiol100_reset_tx_fifo() returns -ENODEV for NULL dev pointer. */
void test_adiol100_reset_tx_fifo_null_dev(void)
{
	int ret = adiol100_reset_tx_fifo(NULL, ADIOL100_CH_A);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/** @brief Test that adiol100_reset_tx_fifo() sets TXFIFORSTN in FRAMCTRL2. */
void test_adiol100_reset_tx_fifo_ch_a(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	int ret = adiol100_reset_tx_fifo(dev, ADIOL100_CH_A);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_BITS(ADIOL100_TXFIFORSTN, ADIOL100_TXFIFORSTN,
			 REG(ADIOL100_REG_FRAMCTRL2_A));

	free(dev);
}

/** @brief Test that adiol100_reset_tx_fifo() routes to FRAMCTRL2_B for channel B. */
void test_adiol100_reset_tx_fifo_ch_b(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	int ret = adiol100_reset_tx_fifo(dev, ADIOL100_CH_B);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_BITS(ADIOL100_TXFIFORSTN, ADIOL100_TXFIFORSTN,
			 REG(ADIOL100_REG_FRAMCTRL2_B));

	free(dev);
}

/** @brief Test that adiol100_reset_tx_fifo() leaves bits outside TXFIFORSTN untouched. */
void test_adiol100_reset_tx_fifo_preserves_other_bits(void)
{
	struct adiol100_dev *dev = create_test_dev();
	int ret;

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	PRESET_OUTSIDE(ADIOL100_REG_FRAMCTRL2_A, ADIOL100_TXFIFORSTN, 1);
	ret = adiol100_reset_tx_fifo(dev, ADIOL100_CH_A);
	TEST_ASSERT_EQUAL_INT(0, ret);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_FRAMCTRL2_A,
				 ADIOL100_TXFIFORSTN, 1);

	PRESET_OUTSIDE(ADIOL100_REG_FRAMCTRL2_A, ADIOL100_TXFIFORSTN, 0);
	ret = adiol100_reset_tx_fifo(dev, ADIOL100_CH_A);
	TEST_ASSERT_EQUAL_INT(0, ret);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_FRAMCTRL2_A,
				 ADIOL100_TXFIFORSTN, 0);

	free(dev);
}

/** @brief Test that adiol100_reset_rx_fifo() returns -ENODEV for NULL dev pointer. */
void test_adiol100_reset_rx_fifo_null_dev(void)
{
	int ret = adiol100_reset_rx_fifo(NULL, ADIOL100_CH_A);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/** @brief Test that adiol100_reset_rx_fifo() sets RXFIFORSTN in FRAMCTRL2. */
void test_adiol100_reset_rx_fifo_ch_a(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	int ret = adiol100_reset_rx_fifo(dev, ADIOL100_CH_A);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_BITS(ADIOL100_RXFIFORSTN, ADIOL100_RXFIFORSTN,
			 REG(ADIOL100_REG_FRAMCTRL2_A));

	free(dev);
}

/** @brief Test that adiol100_reset_rx_fifo() routes to FRAMCTRL2_B for channel B. */
void test_adiol100_reset_rx_fifo_ch_b(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	int ret = adiol100_reset_rx_fifo(dev, ADIOL100_CH_B);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_BITS(ADIOL100_RXFIFORSTN, ADIOL100_RXFIFORSTN,
			 REG(ADIOL100_REG_FRAMCTRL2_B));

	free(dev);
}

/** @brief Test that adiol100_reset_rx_fifo() leaves bits outside RXFIFORSTN untouched. */
void test_adiol100_reset_rx_fifo_preserves_other_bits(void)
{
	struct adiol100_dev *dev = create_test_dev();
	int ret;

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	PRESET_OUTSIDE(ADIOL100_REG_FRAMCTRL2_A, ADIOL100_RXFIFORSTN, 1);
	ret = adiol100_reset_rx_fifo(dev, ADIOL100_CH_A);
	TEST_ASSERT_EQUAL_INT(0, ret);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_FRAMCTRL2_A,
				 ADIOL100_RXFIFORSTN, 1);

	PRESET_OUTSIDE(ADIOL100_REG_FRAMCTRL2_A, ADIOL100_RXFIFORSTN, 0);
	ret = adiol100_reset_rx_fifo(dev, ADIOL100_CH_A);
	TEST_ASSERT_EQUAL_INT(0, ret);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_FRAMCTRL2_A,
				 ADIOL100_RXFIFORSTN, 0);

	free(dev);
}

/* ---- EstCom ------------------------------------------------------------- */

/** @brief Test that adiol100_estcom() returns -ENODEV for NULL dev pointer. */
void test_adiol100_estcom_null_dev(void)
{
	int ret = adiol100_estcom(NULL, ADIOL100_CH_A);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/** @brief Test that adiol100_estcom() clears CQSEND/CYCLETMREN, resets FIFOs, sets ESTCOM. */
void test_adiol100_estcom_success(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_mdelay_Ignore();

	g_estcom_poll_count = 0;
	g_estcom_trigger_at = 3;
	g_estcom_irq_val = ADIOL100_ESTCOMSUCINT;

	REG(ADIOL100_REG_FRAMCTRL1_A) = ADIOL100_CQSEND | ADIOL100_CYCLETMREN;

	no_os_spi_write_and_read_Stub(stub_spi_estcom);

	int ret = adiol100_estcom(dev, ADIOL100_CH_A);
	TEST_ASSERT_EQUAL_INT(0, ret);

	uint16_t fc1 = REG(ADIOL100_REG_FRAMCTRL1_A);
	TEST_ASSERT_BITS(ADIOL100_CQSEND, 0, fc1);
	TEST_ASSERT_BITS(ADIOL100_CYCLETMREN, 0, fc1);
	TEST_ASSERT_BITS(ADIOL100_ESTCOM, ADIOL100_ESTCOM, fc1);

	uint16_t fc2 = REG(ADIOL100_REG_FRAMCTRL2_A);
	TEST_ASSERT_BITS(ADIOL100_TXFIFORSTN, ADIOL100_TXFIFORSTN, fc2);
	TEST_ASSERT_BITS(ADIOL100_RXFIFORSTN, ADIOL100_RXFIFORSTN, fc2);

	TEST_ASSERT_EQUAL_HEX16(0, dev->irq_ch_flags[ADIOL100_CH_A] &
				(ADIOL100_ESTCOMSUCINT | ADIOL100_ESTCOMFAILINT));

	free(dev);
}

/** @brief Test that adiol100_estcom() returns -ENODEV on ESTCOMFAILINT. */
void test_adiol100_estcom_fail(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_mdelay_Ignore();

	g_estcom_poll_count = 0;
	g_estcom_trigger_at = 2;
	g_estcom_irq_val = ADIOL100_ESTCOMFAILINT;

	no_os_spi_write_and_read_Stub(stub_spi_estcom);

	int ret = adiol100_estcom(dev, ADIOL100_CH_A);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);

	free(dev);
}

/** @brief Test that adiol100_estcom() returns -ETIMEDOUT when no IRQ fires. */
void test_adiol100_estcom_timeout(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_mdelay_Ignore();

	g_estcom_poll_count = 0;
	g_estcom_trigger_at = 999; /* Never triggers */
	g_estcom_irq_val = 0;

	no_os_spi_write_and_read_Stub(stub_spi_estcom);

	int ret = adiol100_estcom(dev, ADIOL100_CH_A);
	TEST_ASSERT_EQUAL_INT(-ETIMEDOUT, ret);

	free(dev);
}

/** @brief Test that adiol100_estcom() routes to channel B registers. */
void test_adiol100_estcom_ch_b(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_mdelay_Ignore();

	g_estcom_poll_count = 0;
	g_estcom_trigger_at = 2;
	g_estcom_irq_val = ADIOL100_ESTCOMSUCINT;

	REG(ADIOL100_REG_FRAMCTRL1_B) = ADIOL100_CQSEND | ADIOL100_CYCLETMREN;

	no_os_spi_write_and_read_Stub(stub_spi_estcom);

	int ret = adiol100_estcom(dev, ADIOL100_CH_B);
	TEST_ASSERT_EQUAL_INT(0, ret);

	uint16_t fc1 = REG(ADIOL100_REG_FRAMCTRL1_B);
	TEST_ASSERT_BITS(ADIOL100_CQSEND, 0, fc1);
	TEST_ASSERT_BITS(ADIOL100_CYCLETMREN, 0, fc1);
	TEST_ASSERT_BITS(ADIOL100_ESTCOM, ADIOL100_ESTCOM, fc1);

	uint16_t fc2 = REG(ADIOL100_REG_FRAMCTRL2_B);
	TEST_ASSERT_BITS(ADIOL100_TXFIFORSTN, ADIOL100_TXFIFORSTN, fc2);
	TEST_ASSERT_BITS(ADIOL100_RXFIFORSTN, ADIOL100_RXFIFORSTN, fc2);

	free(dev);
}

/** @brief Test that adiol100_estcom() leaves bits outside its FRAMCTRL1/2 fields untouched. */
void test_adiol100_estcom_preserves_other_bits(void)
{
	struct adiol100_dev *dev = create_test_dev();
	const uint16_t fc1_mask = ADIOL100_CQSEND | ADIOL100_CYCLETMREN |
				  ADIOL100_ESTCOM;
	const uint16_t fc2_mask = ADIOL100_TXFIFORSTN | ADIOL100_RXFIFORSTN;
	int ret;

	install_util_stubs();
	no_os_mdelay_Ignore();
	no_os_spi_write_and_read_Stub(stub_spi_estcom);

	g_estcom_trigger_at = 2;
	g_estcom_irq_val = ADIOL100_ESTCOMSUCINT;

	g_estcom_poll_count = 0;
	PRESET_OUTSIDE(ADIOL100_REG_FRAMCTRL1_A, fc1_mask, 1);
	PRESET_OUTSIDE(ADIOL100_REG_FRAMCTRL2_A, fc2_mask, 1);
	ret = adiol100_estcom(dev, ADIOL100_CH_A);
	TEST_ASSERT_EQUAL_INT(0, ret);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_FRAMCTRL1_A, fc1_mask, 1);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_FRAMCTRL2_A, fc2_mask, 1);

	g_estcom_poll_count = 0;
	PRESET_OUTSIDE(ADIOL100_REG_FRAMCTRL1_A, fc1_mask, 0);
	PRESET_OUTSIDE(ADIOL100_REG_FRAMCTRL2_A, fc2_mask, 0);
	ret = adiol100_estcom(dev, ADIOL100_CH_A);
	TEST_ASSERT_EQUAL_INT(0, ret);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_FRAMCTRL1_A, fc1_mask, 0);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_FRAMCTRL2_A, fc2_mask, 0);

	free(dev);
}

/* ---- Config ------------------------------------------------------------- */

/** @brief Test that adiol100_config_cq() returns -ENODEV for NULL dev pointer. */
void test_adiol100_config_cq_null_dev(void)
{
	int ret = adiol100_config_cq(NULL, ADIOL100_CH_A,
				     ADIOL100_CQ_PUSHPULL,
				     ADIOL100_CQ_DRV_EN,
				     ADIOL100_SINKSEL_5MA,
				     ADIOL100_CQSLEW_250NS,
				     ADIOL100_IEC3TH_DIS,
				     ADIOL100_CQ_SINK,
				     ADIOL100_GLITCH_DIS,
				     ADIOL100_CQ_PNP,
				     ADIOL100_CQEXT_DIS,
				     ADIOL100_INVCQ_DIS,
				     ADIOL100_TXEN_DIS);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/** @brief Test that adiol100_config_cq() writes all 11 CQSET fields correctly. */
void test_adiol100_config_cq_ch_a(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	int ret = adiol100_config_cq(dev, ADIOL100_CH_A,
				     ADIOL100_CQ_PUSHPULL,
				     ADIOL100_CQ_DRV_DIS,
				     ADIOL100_SINKSEL_150UA,
				     ADIOL100_CQSLEW_500NS,
				     ADIOL100_IEC3TH_EN,
				     ADIOL100_CQ_SINK,
				     ADIOL100_GLITCH_2,
				     ADIOL100_CQ_NPN_EN,
				     ADIOL100_CQEXT_DIS,
				     ADIOL100_INVCQ_EN,
				     ADIOL100_TXEN_DIS);
	TEST_ASSERT_EQUAL_INT(0, ret);

	uint16_t cqset = REG(ADIOL100_REG_CQSET_A);
	TEST_ASSERT_BITS(ADIOL100_PUSHPUL, ADIOL100_PUSHPUL, cqset);
	TEST_ASSERT_BITS(ADIOL100_DRVEN, 0, cqset);
	TEST_ASSERT_EQUAL_UINT8(ADIOL100_SINKSEL_150UA,
				stub_field_get(ADIOL100_SINKSEL_MSK, cqset));
	TEST_ASSERT_EQUAL_UINT8(ADIOL100_CQSLEW_500NS,
				stub_field_get(ADIOL100_CQSLEW_MSK, cqset));
	TEST_ASSERT_BITS(ADIOL100_IEC3TH, ADIOL100_IEC3TH, cqset);
	TEST_ASSERT_BITS(ADIOL100_SOURCESINK, 0, cqset);
	TEST_ASSERT_EQUAL_UINT8(ADIOL100_GLITCH_2,
				stub_field_get(ADIOL100_GLITCHFILTER_MSK, cqset));
	TEST_ASSERT_BITS(ADIOL100_NPN, ADIOL100_NPN, cqset);
	TEST_ASSERT_BITS(ADIOL100_CQEXT, 0, cqset);
	TEST_ASSERT_BITS(ADIOL100_INVCQ, ADIOL100_INVCQ, cqset);
	TEST_ASSERT_BITS(ADIOL100_TXEN, 0, cqset);

	free(dev);
}

/** @brief Test that adiol100_config_cq() routes to CQSET_B for channel B. */
void test_adiol100_config_cq_ch_b(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	int ret = adiol100_config_cq(dev, ADIOL100_CH_B,
				     ADIOL100_CQ_NPN,
				     ADIOL100_CQ_DRV_EN,
				     ADIOL100_SINKSEL_2MA,
				     ADIOL100_CQSLEW_1250NS,
				     ADIOL100_IEC3TH_DIS,
				     ADIOL100_CQ_SOURCE,
				     ADIOL100_GLITCH_3,
				     ADIOL100_CQ_PNP,
				     ADIOL100_CQEXT_EN,
				     ADIOL100_INVCQ_DIS,
				     ADIOL100_TXEN_EN);
	TEST_ASSERT_EQUAL_INT(0, ret);

	uint16_t cqset_b = REG(ADIOL100_REG_CQSET_B);
	TEST_ASSERT_BITS(ADIOL100_PUSHPUL, 0, cqset_b);
	TEST_ASSERT_BITS(ADIOL100_DRVEN, ADIOL100_DRVEN, cqset_b);
	TEST_ASSERT_EQUAL_UINT8(ADIOL100_SINKSEL_2MA,
				stub_field_get(ADIOL100_SINKSEL_MSK, cqset_b));
	TEST_ASSERT_EQUAL_UINT8(ADIOL100_CQSLEW_1250NS,
				stub_field_get(ADIOL100_CQSLEW_MSK, cqset_b));
	TEST_ASSERT_BITS(ADIOL100_IEC3TH, 0, cqset_b);
	TEST_ASSERT_BITS(ADIOL100_SOURCESINK, ADIOL100_SOURCESINK, cqset_b);
	TEST_ASSERT_EQUAL_UINT8(ADIOL100_GLITCH_3,
				stub_field_get(ADIOL100_GLITCHFILTER_MSK, cqset_b));
	TEST_ASSERT_BITS(ADIOL100_NPN, 0, cqset_b);
	TEST_ASSERT_BITS(ADIOL100_CQEXT, ADIOL100_CQEXT, cqset_b);
	TEST_ASSERT_BITS(ADIOL100_INVCQ, 0, cqset_b);
	TEST_ASSERT_BITS(ADIOL100_TXEN, ADIOL100_TXEN, cqset_b);

	free(dev);
}

/** @brief Test that adiol100_config_cq() leaves bits outside the CQSET fields untouched. */
void test_adiol100_config_cq_preserves_other_bits(void)
{
	struct adiol100_dev *dev = create_test_dev();
	int ret;
	const uint16_t mask = ADIOL100_IEC3TH | ADIOL100_SOURCESINK |
			      ADIOL100_SINKSEL_MSK | ADIOL100_GLITCHFILTER_MSK |
			      ADIOL100_CQSLEW_MSK | ADIOL100_NPN |
			      ADIOL100_PUSHPUL | ADIOL100_DRVEN |
			      ADIOL100_CQEXT | ADIOL100_INVCQ | ADIOL100_TXEN;

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	PRESET_OUTSIDE(ADIOL100_REG_CQSET_A, mask, 1);
	ret = adiol100_config_cq(dev, ADIOL100_CH_A, ADIOL100_CQ_PUSHPULL,
				 ADIOL100_CQ_DRV_EN, ADIOL100_SINKSEL_5MA,
				 ADIOL100_CQSLEW_250NS, ADIOL100_IEC3TH_DIS,
				 ADIOL100_CQ_SINK, ADIOL100_GLITCH_DIS,
				 ADIOL100_CQ_PNP, ADIOL100_CQEXT_DIS,
				 ADIOL100_INVCQ_DIS, ADIOL100_TXEN_DIS);
	TEST_ASSERT_EQUAL_INT(0, ret);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_CQSET_A, mask, 1);

	PRESET_OUTSIDE(ADIOL100_REG_CQSET_A, mask, 0);
	ret = adiol100_config_cq(dev, ADIOL100_CH_A, ADIOL100_CQ_PUSHPULL,
				 ADIOL100_CQ_DRV_EN, ADIOL100_SINKSEL_5MA,
				 ADIOL100_CQSLEW_250NS, ADIOL100_IEC3TH_DIS,
				 ADIOL100_CQ_SINK, ADIOL100_GLITCH_DIS,
				 ADIOL100_CQ_PNP, ADIOL100_CQEXT_DIS,
				 ADIOL100_INVCQ_DIS, ADIOL100_TXEN_DIS);
	TEST_ASSERT_EQUAL_INT(0, ret);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_CQSET_A, mask, 0);

	free(dev);
}

/** @brief Test that adiol100_config_cq_protection() returns -ENODEV for NULL dev. */
void test_adiol100_config_cq_protection_null_dev(void)
{
	int ret = adiol100_config_cq_protection(NULL, ADIOL100_CH_A,
						ADIOL100_CQCL_300MA,
						ADIOL100_CLBL_1MS,
						ADIOL100_ARTTMO_5MS,
						ADIOL100_AUTORETRY_DIS,
						ADIOL100_VTHR_2V,
						ADIOL100_VTHR_2V,
						ADIOL100_VTHR_2V,
						ADIOL100_VTHR_2V);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/** @brief Test that adiol100_config_cq_protection() writes all 8 CQCURLIM fields. */
void test_adiol100_config_cq_protection_ch_a(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	int ret = adiol100_config_cq_protection(dev, ADIOL100_CH_A,
						ADIOL100_CQCL_300MA,
						ADIOL100_CLBL_1MS,
						ADIOL100_ARTTMO_10MS,
						ADIOL100_AUTORETRY_EN,
						ADIOL100_VTHR_8V,
						ADIOL100_VTHR_4V,
						ADIOL100_VTHR_6V,
						ADIOL100_VTHR_8V);
	TEST_ASSERT_EQUAL_INT(0, ret);

	uint16_t curlim = REG(ADIOL100_REG_CQCURLIM_A);
	TEST_ASSERT_EQUAL_UINT8(ADIOL100_CQCL_300MA,
				stub_field_get(ADIOL100_CQCL_MSK, curlim));
	TEST_ASSERT_EQUAL_UINT8(ADIOL100_CLBL_1MS,
				stub_field_get(ADIOL100_CLBL_MSK, curlim));
	TEST_ASSERT_EQUAL_UINT8(ADIOL100_ARTTMO_10MS,
				stub_field_get(ADIOL100_ARTTMO_MSK, curlim));
	TEST_ASSERT_EQUAL_UINT8(ADIOL100_AUTORETRY_EN,
				stub_field_get(ADIOL100_ARTEN, curlim));
	TEST_ASSERT_EQUAL_UINT8(ADIOL100_VTHR_8V,
				stub_field_get(ADIOL100_CQVTHRH_MSK, curlim));
	TEST_ASSERT_EQUAL_UINT8(ADIOL100_VTHR_4V,
				stub_field_get(ADIOL100_CQVTHRL_MSK, curlim));
	TEST_ASSERT_EQUAL_UINT8(ADIOL100_VTHR_6V,
				stub_field_get(ADIOL100_RXVTHRH_MSK, curlim));
	TEST_ASSERT_EQUAL_UINT8(ADIOL100_VTHR_8V,
				stub_field_get(ADIOL100_RXVTHRL_MSK, curlim));

	free(dev);
}

/** @brief Test that adiol100_config_cq_protection() routes to CQCURLIM_B. */
void test_adiol100_config_cq_protection_ch_b(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	int ret = adiol100_config_cq_protection(dev, ADIOL100_CH_B,
						ADIOL100_CQCL_100MA,
						ADIOL100_CLBL_500US,
						ADIOL100_ARTTMO_50MS,
						ADIOL100_AUTORETRY_DIS,
						ADIOL100_VTHR_2V,
						ADIOL100_VTHR_6V,
						ADIOL100_VTHR_4V,
						ADIOL100_VTHR_2V);
	TEST_ASSERT_EQUAL_INT(0, ret);

	uint16_t curlim_b = REG(ADIOL100_REG_CQCURLIM_B);
	TEST_ASSERT_EQUAL_UINT8(ADIOL100_CQCL_100MA,
				stub_field_get(ADIOL100_CQCL_MSK, curlim_b));
	TEST_ASSERT_EQUAL_UINT8(ADIOL100_CLBL_500US,
				stub_field_get(ADIOL100_CLBL_MSK, curlim_b));
	TEST_ASSERT_EQUAL_UINT8(ADIOL100_ARTTMO_50MS,
				stub_field_get(ADIOL100_ARTTMO_MSK, curlim_b));
	TEST_ASSERT_EQUAL_UINT8(ADIOL100_AUTORETRY_DIS,
				stub_field_get(ADIOL100_ARTEN, curlim_b));
	TEST_ASSERT_EQUAL_UINT8(ADIOL100_VTHR_2V,
				stub_field_get(ADIOL100_CQVTHRH_MSK, curlim_b));
	TEST_ASSERT_EQUAL_UINT8(ADIOL100_VTHR_6V,
				stub_field_get(ADIOL100_CQVTHRL_MSK, curlim_b));
	TEST_ASSERT_EQUAL_UINT8(ADIOL100_VTHR_4V,
				stub_field_get(ADIOL100_RXVTHRH_MSK, curlim_b));
	TEST_ASSERT_EQUAL_UINT8(ADIOL100_VTHR_2V,
				stub_field_get(ADIOL100_RXVTHRL_MSK, curlim_b));

	free(dev);
}

/** @brief Test that adiol100_config_cq_protection_ext() returns -ENODEV for NULL dev. */
void test_adiol100_config_cq_protection_ext_null_dev(void)
{
	int ret = adiol100_config_cq_protection_ext(NULL, ADIOL100_CH_A,
			10, 5, 20, 2,
			50, 30, 8, 4, 10, 3);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/** @brief Test that adiol100_config_cq_protection_ext() writes all 10 fields across 6 regs. */
void test_adiol100_config_cq_protection_ext_ch_a(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	int ret = adiol100_config_cq_protection_ext(dev, ADIOL100_CH_A,
			10, 5, 20, 2,
			50, 30, 8, 4, 12, 3);
	TEST_ASSERT_EQUAL_INT(0, ret);

	TEST_ASSERT_EQUAL_UINT8(10, stub_field_get(ADIOL100_CQEXTCLNOM_MSK,
				REG(ADIOL100_REG_CQEXTCFG_A)));
	TEST_ASSERT_EQUAL_UINT8(5, stub_field_get(ADIOL100_CQFETPWRMAX_MSK,
				REG(ADIOL100_REG_CQFETPWRCFG_A)));
	TEST_ASSERT_EQUAL_UINT8(20, stub_field_get(ADIOL100_CQCURRMAX_MSK,
				REG(ADIOL100_REG_CQCURRENTCFG_A)));
	TEST_ASSERT_EQUAL_UINT8(2, stub_field_get(ADIOL100_CQCURRMIN_MSK,
				REG(ADIOL100_REG_CQCURRENTCFG_A)));
	TEST_ASSERT_EQUAL_UINT8(50, stub_field_get(ADIOL100_CQOLTIMOUT_MSK,
				REG(ADIOL100_REG_CQFETPROTECT_A)));
	TEST_ASSERT_EQUAL_UINT8(30, stub_field_get(ADIOL100_CQCLTIMOUT_MSK,
				REG(ADIOL100_REG_CQFETPROTECT_A)));
	TEST_ASSERT_EQUAL_UINT8(8, stub_field_get(ADIOL100_CQFETSLOPE_MSK,
				REG(ADIOL100_REG_CQSLOPE_A)));
	TEST_ASSERT_EQUAL_UINT8(4, stub_field_get(ADIOL100_CQFETSLOPEBL_MSK,
				REG(ADIOL100_REG_CQSLOPE_A)));
	TEST_ASSERT_EQUAL_UINT8(12, stub_field_get(ADIOL100_CQARTIM_MSK,
				REG(ADIOL100_REG_CQRETRY_A)));
	TEST_ASSERT_EQUAL_UINT8(3, stub_field_get(ADIOL100_CQAR_MSK,
				REG(ADIOL100_REG_CQRETRY_A)));

	free(dev);
}

/** @brief Test that adiol100_config_cq_protection_ext() routes to channel B registers. */
void test_adiol100_config_cq_protection_ext_ch_b(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	int ret = adiol100_config_cq_protection_ext(dev, ADIOL100_CH_B,
			10, 5, 20, 2,
			50, 30, 8, 4, 10, 3);
	TEST_ASSERT_EQUAL_INT(0, ret);

	TEST_ASSERT_EQUAL_UINT8(10, stub_field_get(ADIOL100_CQEXTCLNOM_MSK,
				REG(ADIOL100_REG_CQEXTCFG_B)));
	TEST_ASSERT_EQUAL_UINT8(5, stub_field_get(ADIOL100_CQFETPWRMAX_MSK,
				REG(ADIOL100_REG_CQFETPWRCFG_B)));
	TEST_ASSERT_EQUAL_UINT8(3, stub_field_get(ADIOL100_CQAR_MSK,
				REG(ADIOL100_REG_CQRETRY_B)));

	free(dev);
}

/** @brief Test that adiol100_config_cq_protection_ext() leaves bits outside the CQ protection fields untouched. */
void test_adiol100_config_cq_protection_ext_preserves_other_bits(void)
{
	struct adiol100_dev *dev = create_test_dev();
	int ret;
	const uint16_t cfg_mask = ADIOL100_CQEXTCLNOM_MSK;
	const uint16_t pwr_mask = ADIOL100_CQFETPWRMAX_MSK;
	const uint16_t curr_mask = ADIOL100_CQCURRMAX_MSK |
				   ADIOL100_CQCURRMIN_MSK;
	const uint16_t prot_mask = ADIOL100_CQOLTIMOUT_MSK |
				   ADIOL100_CQCLTIMOUT_MSK;
	const uint16_t slope_mask = ADIOL100_CQFETSLOPE_MSK |
				    ADIOL100_CQFETSLOPEBL_MSK;
	const uint16_t retry_mask = ADIOL100_CQARTIM_MSK | ADIOL100_CQAR_MSK;

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	PRESET_OUTSIDE(ADIOL100_REG_CQEXTCFG_A, cfg_mask, 1);
	PRESET_OUTSIDE(ADIOL100_REG_CQFETPWRCFG_A, pwr_mask, 1);
	PRESET_OUTSIDE(ADIOL100_REG_CQCURRENTCFG_A, curr_mask, 1);
	PRESET_OUTSIDE(ADIOL100_REG_CQFETPROTECT_A, prot_mask, 1);
	PRESET_OUTSIDE(ADIOL100_REG_CQSLOPE_A, slope_mask, 1);
	PRESET_OUTSIDE(ADIOL100_REG_CQRETRY_A, retry_mask, 1);
	ret = adiol100_config_cq_protection_ext(dev, ADIOL100_CH_A, 10, 5, 20,
						2, 50, 30, 8, 4, 10, 3);
	TEST_ASSERT_EQUAL_INT(0, ret);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_CQEXTCFG_A, cfg_mask, 1);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_CQFETPWRCFG_A, pwr_mask, 1);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_CQCURRENTCFG_A, curr_mask, 1);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_CQFETPROTECT_A, prot_mask, 1);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_CQSLOPE_A, slope_mask, 1);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_CQRETRY_A, retry_mask, 1);

	PRESET_OUTSIDE(ADIOL100_REG_CQEXTCFG_A, cfg_mask, 0);
	PRESET_OUTSIDE(ADIOL100_REG_CQFETPWRCFG_A, pwr_mask, 0);
	PRESET_OUTSIDE(ADIOL100_REG_CQCURRENTCFG_A, curr_mask, 0);
	PRESET_OUTSIDE(ADIOL100_REG_CQFETPROTECT_A, prot_mask, 0);
	PRESET_OUTSIDE(ADIOL100_REG_CQSLOPE_A, slope_mask, 0);
	PRESET_OUTSIDE(ADIOL100_REG_CQRETRY_A, retry_mask, 0);
	ret = adiol100_config_cq_protection_ext(dev, ADIOL100_CH_A, 10, 5, 20,
						2, 50, 30, 8, 4, 10, 3);
	TEST_ASSERT_EQUAL_INT(0, ret);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_CQEXTCFG_A, cfg_mask, 0);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_CQFETPWRCFG_A, pwr_mask, 0);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_CQCURRENTCFG_A, curr_mask, 0);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_CQFETPROTECT_A, prot_mask, 0);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_CQSLOPE_A, slope_mask, 0);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_CQRETRY_A, retry_mask, 0);

	free(dev);
}

/** @brief Test that adiol100_config_lp() returns -ENODEV for NULL dev pointer. */
void test_adiol100_config_lp_null_dev(void)
{
	int ret = adiol100_config_lp(NULL, ADIOL100_CH_A,
				     ADIOL100_LP_EN, ADIOL100_LP_REV_EN);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/** @brief Test that adiol100_config_lp() sets LPEN and LPFETREVEN. */
void test_adiol100_config_lp_ch_a(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	int ret = adiol100_config_lp(dev, ADIOL100_CH_A,
				     ADIOL100_LP_DIS, ADIOL100_LP_REV_EN);
	TEST_ASSERT_EQUAL_INT(0, ret);

	uint16_t lpcfg = REG(ADIOL100_REG_LPCFG_A);
	TEST_ASSERT_BITS(ADIOL100_LPEN, 0, lpcfg);
	TEST_ASSERT_BITS(ADIOL100_LPFETREVEN, ADIOL100_LPFETREVEN, lpcfg);

	free(dev);
}

/** @brief Test that adiol100_config_lp() routes to LPCFG_B for channel B. */
void test_adiol100_config_lp_ch_b(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	int ret = adiol100_config_lp(dev, ADIOL100_CH_B,
				     ADIOL100_LP_EN, ADIOL100_LP_REV_DIS);
	TEST_ASSERT_EQUAL_INT(0, ret);
	uint16_t lpcfg_b = REG(ADIOL100_REG_LPCFG_B);
	TEST_ASSERT_BITS(ADIOL100_LPEN, ADIOL100_LPEN, lpcfg_b);
	TEST_ASSERT_BITS(ADIOL100_LPFETREVEN, 0, lpcfg_b);

	free(dev);
}

/** @brief Test that adiol100_config_lp() leaves bits outside LPEN/LPFETREVEN untouched. */
void test_adiol100_config_lp_preserves_other_bits(void)
{
	struct adiol100_dev *dev = create_test_dev();
	int ret;
	const uint16_t mask = ADIOL100_LPEN | ADIOL100_LPFETREVEN;

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	PRESET_OUTSIDE(ADIOL100_REG_LPCFG_A, mask, 1);
	ret = adiol100_config_lp(dev, ADIOL100_CH_A, ADIOL100_LP_EN,
				 ADIOL100_LP_REV_EN);
	TEST_ASSERT_EQUAL_INT(0, ret);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_LPCFG_A, mask, 1);

	PRESET_OUTSIDE(ADIOL100_REG_LPCFG_A, mask, 0);
	ret = adiol100_config_lp(dev, ADIOL100_CH_A, ADIOL100_LP_EN,
				 ADIOL100_LP_REV_EN);
	TEST_ASSERT_EQUAL_INT(0, ret);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_LPCFG_A, mask, 0);

	free(dev);
}

/** @brief Test that adiol100_config_lp_protection() returns -ENODEV for NULL dev. */
void test_adiol100_config_lp_protection_null_dev(void)
{
	int ret = adiol100_config_lp_protection(NULL, ADIOL100_CH_A,
						10, 5, 20, 2,
						50, 30, 8, 4, 10, 3);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/** @brief Test that adiol100_config_lp_protection() writes all 10 fields across 6 regs. */
void test_adiol100_config_lp_protection_ch_a(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	int ret = adiol100_config_lp_protection(dev, ADIOL100_CH_A,
						10, 5, 20, 2,
						50, 30, 8, 4, 12, 3);
	TEST_ASSERT_EQUAL_INT(0, ret);

	TEST_ASSERT_EQUAL_UINT8(10, stub_field_get(ADIOL100_LPCLNOM_MSK,
				REG(ADIOL100_REG_LPCFG_A)));
	TEST_ASSERT_EQUAL_UINT8(5, stub_field_get(ADIOL100_LPFETPWRMAX_MSK,
				REG(ADIOL100_REG_LPFETPWRCFG_A)));
	TEST_ASSERT_EQUAL_UINT8(20, stub_field_get(ADIOL100_LPCURRMAX_MSK,
				REG(ADIOL100_REG_LPCURRENTCFG_A)));
	TEST_ASSERT_EQUAL_UINT8(2, stub_field_get(ADIOL100_LPCURRMIN_MSK,
				REG(ADIOL100_REG_LPCURRENTCFG_A)));
	TEST_ASSERT_EQUAL_UINT8(50, stub_field_get(ADIOL100_LPOLTIMOUT_MSK,
				REG(ADIOL100_REG_LPFETPROTECT_A)));
	TEST_ASSERT_EQUAL_UINT8(30, stub_field_get(ADIOL100_LPCLTIMOUT_MSK,
				REG(ADIOL100_REG_LPFETPROTECT_A)));
	TEST_ASSERT_EQUAL_UINT8(8, stub_field_get(ADIOL100_LPSLOPE_MSK,
				REG(ADIOL100_REG_LPSLOPE_A)));
	TEST_ASSERT_EQUAL_UINT8(4, stub_field_get(ADIOL100_LPSLOPEBL_MSK,
				REG(ADIOL100_REG_LPSLOPE_A)));
	TEST_ASSERT_EQUAL_UINT8(12, stub_field_get(ADIOL100_LPARTIM_MSK,
				REG(ADIOL100_REG_LPRETRY_A)));
	TEST_ASSERT_EQUAL_UINT8(3, stub_field_get(ADIOL100_LPAR_MSK,
				REG(ADIOL100_REG_LPRETRY_A)));

	free(dev);
}

/** @brief Test that adiol100_config_lp_protection() routes to channel B registers. */
void test_adiol100_config_lp_protection_ch_b(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	int ret = adiol100_config_lp_protection(dev, ADIOL100_CH_B,
						10, 5, 20, 2,
						50, 30, 8, 4, 10, 3);
	TEST_ASSERT_EQUAL_INT(0, ret);

	TEST_ASSERT_EQUAL_UINT8(10, stub_field_get(ADIOL100_LPCLNOM_MSK,
				REG(ADIOL100_REG_LPCFG_B)));
	TEST_ASSERT_EQUAL_UINT8(5, stub_field_get(ADIOL100_LPFETPWRMAX_MSK,
				REG(ADIOL100_REG_LPFETPWRCFG_B)));
	TEST_ASSERT_EQUAL_UINT8(3, stub_field_get(ADIOL100_LPAR_MSK,
				REG(ADIOL100_REG_LPRETRY_B)));

	free(dev);
}

/** @brief Test that adiol100_config_lp_protection() leaves bits outside the LP protection fields untouched. */
void test_adiol100_config_lp_protection_preserves_other_bits(void)
{
	struct adiol100_dev *dev = create_test_dev();
	int ret;
	const uint16_t cfg_mask = ADIOL100_LPCLNOM_MSK;
	const uint16_t pwr_mask = ADIOL100_LPFETPWRMAX_MSK;
	const uint16_t curr_mask = ADIOL100_LPCURRMAX_MSK |
				   ADIOL100_LPCURRMIN_MSK;
	const uint16_t prot_mask = ADIOL100_LPOLTIMOUT_MSK |
				   ADIOL100_LPCLTIMOUT_MSK;
	const uint16_t slope_mask = ADIOL100_LPSLOPE_MSK |
				    ADIOL100_LPSLOPEBL_MSK;
	const uint16_t retry_mask = ADIOL100_LPARTIM_MSK | ADIOL100_LPAR_MSK;

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	PRESET_OUTSIDE(ADIOL100_REG_LPCFG_A, cfg_mask, 1);
	PRESET_OUTSIDE(ADIOL100_REG_LPFETPWRCFG_A, pwr_mask, 1);
	PRESET_OUTSIDE(ADIOL100_REG_LPCURRENTCFG_A, curr_mask, 1);
	PRESET_OUTSIDE(ADIOL100_REG_LPFETPROTECT_A, prot_mask, 1);
	PRESET_OUTSIDE(ADIOL100_REG_LPSLOPE_A, slope_mask, 1);
	PRESET_OUTSIDE(ADIOL100_REG_LPRETRY_A, retry_mask, 1);
	ret = adiol100_config_lp_protection(dev, ADIOL100_CH_A, 10, 5, 20, 2,
					    50, 30, 8, 4, 10, 3);
	TEST_ASSERT_EQUAL_INT(0, ret);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_LPCFG_A, cfg_mask, 1);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_LPFETPWRCFG_A, pwr_mask, 1);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_LPCURRENTCFG_A, curr_mask, 1);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_LPFETPROTECT_A, prot_mask, 1);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_LPSLOPE_A, slope_mask, 1);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_LPRETRY_A, retry_mask, 1);

	PRESET_OUTSIDE(ADIOL100_REG_LPCFG_A, cfg_mask, 0);
	PRESET_OUTSIDE(ADIOL100_REG_LPFETPWRCFG_A, pwr_mask, 0);
	PRESET_OUTSIDE(ADIOL100_REG_LPCURRENTCFG_A, curr_mask, 0);
	PRESET_OUTSIDE(ADIOL100_REG_LPFETPROTECT_A, prot_mask, 0);
	PRESET_OUTSIDE(ADIOL100_REG_LPSLOPE_A, slope_mask, 0);
	PRESET_OUTSIDE(ADIOL100_REG_LPRETRY_A, retry_mask, 0);
	ret = adiol100_config_lp_protection(dev, ADIOL100_CH_A, 10, 5, 20, 2,
					    50, 30, 8, 4, 10, 3);
	TEST_ASSERT_EQUAL_INT(0, ret);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_LPCFG_A, cfg_mask, 0);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_LPFETPWRCFG_A, pwr_mask, 0);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_LPCURRENTCFG_A, curr_mask, 0);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_LPFETPROTECT_A, prot_mask, 0);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_LPSLOPE_A, slope_mask, 0);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_LPRETRY_A, retry_mask, 0);

	free(dev);
}

/** @brief Test that adiol100_config_framer() returns -ENODEV for NULL dev pointer. */
void test_adiol100_config_framer_null_dev(void)
{
	int ret = adiol100_config_framer(NULL, ADIOL100_CH_A,
					 ADIOL100_CHKS_EN,
					 ADIOL100_FRAMER_EN,
					 ADIOL100_CKTERR_SEND,
					 ADIOL100_MSGREP_DIS,
					 ADIOL100_BDELAY_0TBIT,
					 3,
					 ADIOL100_TSIZEERR_SEND,
					 ADIOL100_UARTPAR_DIS,
					 ADIOL100_UART_230400,
					 ADIOL100_UART_DIS,
					 ADIOL100_SAFEPULS_DIS);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/** @brief Test that adiol100_config_framer() writes all 11 fields across FRAMCTRL1/2. */
void test_adiol100_config_framer_ch_a(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	int ret = adiol100_config_framer(dev, ADIOL100_CH_A,
					 ADIOL100_CHKS_EN,
					 ADIOL100_FRAMER_DIS,
					 ADIOL100_CKTERR_DISCARD,
					 ADIOL100_MSGREP_DIS,
					 ADIOL100_BDELAY_2TBIT,
					 5, /* ddelay, reset value is 3 */
					 ADIOL100_TSIZEERR_DISCARD,
					 ADIOL100_UARTPAR_DIS,
					 ADIOL100_UART_38400,
					 ADIOL100_UART_EN,
					 ADIOL100_SAFEPULS_DIS);
	TEST_ASSERT_EQUAL_INT(0, ret);

	uint16_t fc1 = REG(ADIOL100_REG_FRAMCTRL1_A);
	TEST_ASSERT_BITS(ADIOL100_INSCHKS, ADIOL100_INSCHKS, fc1);
	TEST_ASSERT_BITS(ADIOL100_FRAMEREN, 0, fc1);
	TEST_ASSERT_BITS(ADIOL100_UARTPAREN, 0, fc1);
	TEST_ASSERT_BITS(ADIOL100_SAFEPULSDET, 0, fc1);
	TEST_ASSERT_EQUAL_UINT8(ADIOL100_UART_38400,
				stub_field_get(ADIOL100_UARTBAUD_MSK, fc1));
	TEST_ASSERT_BITS(ADIOL100_UARTEN, ADIOL100_UARTEN, fc1);

	uint16_t fc2 = REG(ADIOL100_REG_FRAMCTRL2_A);
	TEST_ASSERT_EQUAL_UINT8(5, stub_field_get(ADIOL100_DDELAY_MSK, fc2));
	TEST_ASSERT_BITS(ADIOL100_TSIZEERRINH, ADIOL100_TSIZEERRINH, fc2);
	TEST_ASSERT_BITS(ADIOL100_CKTERRINH, ADIOL100_CKTERRINH, fc2);
	TEST_ASSERT_BITS(ADIOL100_MSGREPEAT, 0, fc2);
	TEST_ASSERT_EQUAL_UINT8(ADIOL100_BDELAY_2TBIT,
				stub_field_get(ADIOL100_BDELAY_MSK, fc2));

	free(dev);
}

/** @brief Test that adiol100_config_framer() routes to channel B FRAMCTRL registers. */
void test_adiol100_config_framer_ch_b(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	int ret = adiol100_config_framer(dev, ADIOL100_CH_B,
					 ADIOL100_CHKS_DIS,
					 ADIOL100_FRAMER_EN,
					 ADIOL100_CKTERR_SEND,
					 ADIOL100_MSGREP_EN,
					 ADIOL100_BDELAY_3TBIT,
					 6, /* ddelay, reset value is 3 */
					 ADIOL100_TSIZEERR_SEND,
					 ADIOL100_UARTPAR_EN,
					 ADIOL100_UART_115200,
					 ADIOL100_UART_DIS,
					 ADIOL100_SAFEPULS_EN);
	TEST_ASSERT_EQUAL_INT(0, ret);

	uint16_t fc1 = REG(ADIOL100_REG_FRAMCTRL1_B);
	TEST_ASSERT_BITS(ADIOL100_INSCHKS, 0, fc1);
	TEST_ASSERT_BITS(ADIOL100_FRAMEREN, ADIOL100_FRAMEREN, fc1);
	TEST_ASSERT_BITS(ADIOL100_UARTPAREN, ADIOL100_UARTPAREN, fc1);
	TEST_ASSERT_BITS(ADIOL100_SAFEPULSDET, ADIOL100_SAFEPULSDET, fc1);
	TEST_ASSERT_EQUAL_UINT8(ADIOL100_UART_115200,
				stub_field_get(ADIOL100_UARTBAUD_MSK, fc1));
	TEST_ASSERT_BITS(ADIOL100_UARTEN, 0, fc1);

	uint16_t fc2 = REG(ADIOL100_REG_FRAMCTRL2_B);
	TEST_ASSERT_EQUAL_UINT8(6, stub_field_get(ADIOL100_DDELAY_MSK, fc2));
	TEST_ASSERT_BITS(ADIOL100_TSIZEERRINH, 0, fc2);
	TEST_ASSERT_BITS(ADIOL100_CKTERRINH, 0, fc2);
	TEST_ASSERT_BITS(ADIOL100_MSGREPEAT, ADIOL100_MSGREPEAT, fc2);
	TEST_ASSERT_EQUAL_UINT8(ADIOL100_BDELAY_3TBIT,
				stub_field_get(ADIOL100_BDELAY_MSK, fc2));

	free(dev);
}

/** @brief Test that adiol100_config_framer() leaves bits outside the FRAMCTRL1/2 fields untouched. */
void test_adiol100_config_framer_preserves_other_bits(void)
{
	struct adiol100_dev *dev = create_test_dev();
	int ret;
	const uint16_t fc1_mask = ADIOL100_UARTPAREN | ADIOL100_SAFEPULSDET |
				  ADIOL100_INSCHKS | ADIOL100_UARTBAUD_MSK |
				  ADIOL100_UARTEN | ADIOL100_FRAMEREN;
	const uint16_t fc2_mask = ADIOL100_TSIZEERRINH | ADIOL100_CKTERRINH |
				  ADIOL100_MSGREPEAT | ADIOL100_BDELAY_MSK |
				  ADIOL100_DDELAY_MSK;

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	PRESET_OUTSIDE(ADIOL100_REG_FRAMCTRL1_A, fc1_mask, 1);
	PRESET_OUTSIDE(ADIOL100_REG_FRAMCTRL2_A, fc2_mask, 1);
	ret = adiol100_config_framer(dev, ADIOL100_CH_A, ADIOL100_CHKS_EN,
				     ADIOL100_FRAMER_EN, ADIOL100_CKTERR_SEND,
				     ADIOL100_MSGREP_DIS, ADIOL100_BDELAY_0TBIT,
				     3, ADIOL100_TSIZEERR_SEND,
				     ADIOL100_UARTPAR_DIS, ADIOL100_UART_230400,
				     ADIOL100_UART_DIS, ADIOL100_SAFEPULS_DIS);
	TEST_ASSERT_EQUAL_INT(0, ret);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_FRAMCTRL1_A, fc1_mask, 1);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_FRAMCTRL2_A, fc2_mask, 1);

	PRESET_OUTSIDE(ADIOL100_REG_FRAMCTRL1_A, fc1_mask, 0);
	PRESET_OUTSIDE(ADIOL100_REG_FRAMCTRL2_A, fc2_mask, 0);
	ret = adiol100_config_framer(dev, ADIOL100_CH_A, ADIOL100_CHKS_EN,
				     ADIOL100_FRAMER_EN, ADIOL100_CKTERR_SEND,
				     ADIOL100_MSGREP_DIS, ADIOL100_BDELAY_0TBIT,
				     3, ADIOL100_TSIZEERR_SEND,
				     ADIOL100_UARTPAR_DIS, ADIOL100_UART_230400,
				     ADIOL100_UART_DIS, ADIOL100_SAFEPULS_DIS);
	TEST_ASSERT_EQUAL_INT(0, ret);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_FRAMCTRL1_A, fc1_mask, 0);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_FRAMCTRL2_A, fc2_mask, 0);

	free(dev);
}

/** @brief Test that adiol100_config_di() returns -ENODEV for NULL dev pointer. */
void test_adiol100_config_di_null_dev(void)
{
	int ret = adiol100_config_di(NULL, ADIOL100_CH_A,
				     ADIOL100_IEC3TH_DIS,
				     ADIOL100_CQ_SINK,
				     ADIOL100_SINKSEL_5MA,
				     ADIOL100_GLITCH_DIS,
				     ADIOL100_INVDI_DIS);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/** @brief Test that adiol100_config_di() writes all 5 DICFG fields. */
void test_adiol100_config_di_ch_a(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	int ret = adiol100_config_di(dev, ADIOL100_CH_A,
				     ADIOL100_IEC3TH_EN,
				     ADIOL100_CQ_SINK,
				     ADIOL100_SINKSEL_2MA,
				     ADIOL100_GLITCH_3,
				     ADIOL100_INVDI_EN);
	TEST_ASSERT_EQUAL_INT(0, ret);

	uint16_t dicfg = REG(ADIOL100_REG_DICFG_A);
	TEST_ASSERT_BITS(ADIOL100_IEC3THDI, ADIOL100_IEC3THDI, dicfg);
	TEST_ASSERT_BITS(ADIOL100_SOURCESINKDI, 0, dicfg);
	TEST_ASSERT_EQUAL_UINT8(ADIOL100_SINKSEL_2MA,
				stub_field_get(ADIOL100_SINKSELDI_MSK, dicfg));
	TEST_ASSERT_EQUAL_UINT8(ADIOL100_GLITCH_3,
				stub_field_get(ADIOL100_GLITCHFILTERDI_MSK, dicfg));
	TEST_ASSERT_BITS(ADIOL100_INVDI, ADIOL100_INVDI, dicfg);

	free(dev);
}

/** @brief Test that adiol100_config_di() routes to DICFG_B for channel B. */
void test_adiol100_config_di_ch_b(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	int ret = adiol100_config_di(dev, ADIOL100_CH_B,
				     ADIOL100_IEC3TH_DIS,
				     ADIOL100_CQ_SOURCE,
				     ADIOL100_SINKSEL_5MA,
				     ADIOL100_GLITCH_1,
				     ADIOL100_INVDI_DIS);
	TEST_ASSERT_EQUAL_INT(0, ret);

	uint16_t dicfg = REG(ADIOL100_REG_DICFG_B);
	TEST_ASSERT_BITS(ADIOL100_IEC3THDI, 0, dicfg);
	TEST_ASSERT_BITS(ADIOL100_SOURCESINKDI, ADIOL100_SOURCESINKDI, dicfg);
	TEST_ASSERT_EQUAL_UINT8(ADIOL100_SINKSEL_5MA,
				stub_field_get(ADIOL100_SINKSELDI_MSK, dicfg));
	TEST_ASSERT_EQUAL_UINT8(ADIOL100_GLITCH_1,
				stub_field_get(ADIOL100_GLITCHFILTERDI_MSK, dicfg));
	TEST_ASSERT_BITS(ADIOL100_INVDI, 0, dicfg);

	free(dev);
}

/** @brief Test that adiol100_config_di() leaves bits outside the DICFG fields untouched. */
void test_adiol100_config_di_preserves_other_bits(void)
{
	struct adiol100_dev *dev = create_test_dev();
	int ret;
	const uint16_t mask = ADIOL100_IEC3THDI | ADIOL100_SOURCESINKDI |
			      ADIOL100_SINKSELDI_MSK |
			      ADIOL100_GLITCHFILTERDI_MSK | ADIOL100_INVDI;

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	PRESET_OUTSIDE(ADIOL100_REG_DICFG_A, mask, 1);
	ret = adiol100_config_di(dev, ADIOL100_CH_A, ADIOL100_IEC3TH_EN,
				 ADIOL100_CQ_SOURCE, ADIOL100_SINKSEL_5MA,
				 ADIOL100_GLITCH_1, ADIOL100_INVDI_DIS);
	TEST_ASSERT_EQUAL_INT(0, ret);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_DICFG_A, mask, 1);

	PRESET_OUTSIDE(ADIOL100_REG_DICFG_A, mask, 0);
	ret = adiol100_config_di(dev, ADIOL100_CH_A, ADIOL100_IEC3TH_EN,
				 ADIOL100_CQ_SOURCE, ADIOL100_SINKSEL_5MA,
				 ADIOL100_GLITCH_1, ADIOL100_INVDI_DIS);
	TEST_ASSERT_EQUAL_INT(0, ret);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_DICFG_A, mask, 0);

	free(dev);
}

/** @brief Test that adiol100_config_watchdog() returns -ENODEV for NULL dev pointer. */
void test_adiol100_config_watchdog_null_dev(void)
{
	int ret = adiol100_config_watchdog(NULL, ADIOL100_WDG_500US, 10,
					   ADIOL100_WDG_MODE_SPI,
					   ADIOL100_WDG_EN,
					   ADIOL100_WDG_UNLOCKED);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/** @brief Test that adiol100_config_watchdog() writes all 5 WATCHDOG fields. */
void test_adiol100_config_watchdog(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	int ret = adiol100_config_watchdog(dev, ADIOL100_WDG_2MS, 10,
					   ADIOL100_WDG_MODE_CLEAR,
					   ADIOL100_WDG_DIS,
					   ADIOL100_WDG_UNLOCKED);
	TEST_ASSERT_EQUAL_INT(0, ret);

	uint16_t wdg = REG(ADIOL100_REG_WATCHDOG);
	TEST_ASSERT_BITS(ADIOL100_WDGENABLE, 0, wdg);
	TEST_ASSERT_EQUAL_UINT8(ADIOL100_WDG_2MS,
				stub_field_get(ADIOL100_WDGTIMEBASE_MSK, wdg));
	TEST_ASSERT_EQUAL_UINT8(10,
				stub_field_get(ADIOL100_WDGTIME_MSK, wdg));
	TEST_ASSERT_BITS(ADIOL100_WDGMODE, ADIOL100_WDGMODE, wdg);
	TEST_ASSERT_BITS(ADIOL100_WDGLOCK, 0, wdg);

	free(dev);
}

/** @brief Test that adiol100_config_watchdog() leaves bits outside the WATCHDOG fields untouched. */
void test_adiol100_config_watchdog_preserves_other_bits(void)
{
	struct adiol100_dev *dev = create_test_dev();
	int ret;
	const uint16_t mask = ADIOL100_WDGTIMEBASE_MSK | ADIOL100_WDGTIME_MSK |
			      ADIOL100_WDGLOCK | ADIOL100_WDGMODE |
			      ADIOL100_WDGENABLE;

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	PRESET_OUTSIDE(ADIOL100_REG_WATCHDOG, mask, 1);
	ret = adiol100_config_watchdog(dev, ADIOL100_WDG_500US, 10,
				       ADIOL100_WDG_MODE_SPI, ADIOL100_WDG_EN,
				       ADIOL100_WDG_UNLOCKED);
	TEST_ASSERT_EQUAL_INT(0, ret);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_WATCHDOG, mask, 1);

	PRESET_OUTSIDE(ADIOL100_REG_WATCHDOG, mask, 0);
	ret = adiol100_config_watchdog(dev, ADIOL100_WDG_500US, 10,
				       ADIOL100_WDG_MODE_SPI, ADIOL100_WDG_EN,
				       ADIOL100_WDG_UNLOCKED);
	TEST_ASSERT_EQUAL_INT(0, ret);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_WATCHDOG, mask, 0);

	free(dev);
}

/* ---- IRQ ---------------------------------------------------------------- */

/** @brief Test that adiol100_get_channel_irq() returns -ENODEV for NULL dev pointer. */
void test_adiol100_get_channel_irq_null_dev(void)
{
	uint16_t flags;
	int ret = adiol100_get_channel_irq(NULL, ADIOL100_CH_A, &flags);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/** @brief Test that adiol100_get_channel_irq() returns -EINVAL for NULL flags pointer. */
void test_adiol100_get_channel_irq_null_flags(void)
{
	struct adiol100_dev *dev = create_test_dev();
	int ret = adiol100_get_channel_irq(dev, ADIOL100_CH_A, NULL);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

/** @brief Test that adiol100_get_channel_irq() ORs flags across reads and clear works. */
void test_adiol100_get_channel_irq_accumulates(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	uint16_t flags;

	/* First read: RXDARDYINT */
	REG(ADIOL100_REG_INTERRUPT_A) = ADIOL100_RXDARDYINT;
	adiol100_get_channel_irq(dev, ADIOL100_CH_A, &flags);
	TEST_ASSERT_EQUAL_HEX16(ADIOL100_RXDARDYINT, flags);

	/* Second read: ESTCOMSUCINT, should OR with previous */
	REG(ADIOL100_REG_INTERRUPT_A) = ADIOL100_ESTCOMSUCINT;
	adiol100_get_channel_irq(dev, ADIOL100_CH_A, &flags);
	TEST_ASSERT_EQUAL_HEX16(ADIOL100_RXDARDYINT | ADIOL100_ESTCOMSUCINT,
				flags);

	/* Clear RXDARDYINT, only ESTCOMSUCINT should remain */
	adiol100_clear_channel_irq(dev, ADIOL100_CH_A, ADIOL100_RXDARDYINT);
	TEST_ASSERT_EQUAL_HEX16(ADIOL100_ESTCOMSUCINT,
				dev->irq_ch_flags[ADIOL100_CH_A]);

	free(dev);
}

/** @brief Test that adiol100_get_channel_irq() routes to INTERRUPT_B for channel B. */
void test_adiol100_get_channel_irq_ch_b(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	uint16_t flags;

	REG(ADIOL100_REG_INTERRUPT_B) = ADIOL100_CQLEVELINT;
	adiol100_get_channel_irq(dev, ADIOL100_CH_B, &flags);
	TEST_ASSERT_EQUAL_HEX16(ADIOL100_CQLEVELINT, flags);
	TEST_ASSERT_EQUAL_HEX16(ADIOL100_CQLEVELINT,
				dev->irq_ch_flags[ADIOL100_CH_B]);

	free(dev);
}

/** @brief Test that adiol100_get_global_irq() returns -ENODEV for NULL dev pointer. */
void test_adiol100_get_global_irq_null_dev(void)
{
	uint16_t flags;
	int ret = adiol100_get_global_irq(NULL, &flags);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/** @brief Test that adiol100_get_global_irq() returns -EINVAL for NULL flags pointer. */
void test_adiol100_get_global_irq_null_flags(void)
{
	struct adiol100_dev *dev = create_test_dev();
	int ret = adiol100_get_global_irq(dev, NULL);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

/** @brief Test that adiol100_get_global_irq() ORs flags across reads and clear works. */
void test_adiol100_get_global_irq_accumulates(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	uint16_t flags;

	REG(ADIOL100_REG_INTERRUPTG) = ADIOL100_THWARNINT;
	adiol100_get_global_irq(dev, &flags);
	TEST_ASSERT_EQUAL_HEX16(ADIOL100_THWARNINT, flags);

	REG(ADIOL100_REG_INTERRUPTG) = ADIOL100_V24ERRINT;
	adiol100_get_global_irq(dev, &flags);
	TEST_ASSERT_EQUAL_HEX16(ADIOL100_THWARNINT | ADIOL100_V24ERRINT, flags);

	adiol100_clear_global_irq(dev, ADIOL100_THWARNINT);
	TEST_ASSERT_EQUAL_HEX16(ADIOL100_V24ERRINT, dev->irq_global_flags);

	free(dev);
}

/** @brief Test that adiol100_enable_global_irq() returns -ENODEV for NULL dev pointer. */
void test_adiol100_enable_global_irq_null_dev(void)
{
	int ret = adiol100_enable_global_irq(NULL, ADIOL100_THWARNINT);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/** @brief Test that adiol100_enable_global_irq() writes all named IRQ macros. */
void test_adiol100_enable_global_irq(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	uint16_t mask = ADIOL100_EXTCLKOKINT | ADIOL100_WDGINT |
			ADIOL100_SPICRCERRINT | ADIOL100_EXTCLKMISINT |
			ADIOL100_THSHDNINT | ADIOL100_THWARNINT |
			ADIOL100_V24ERRINT | ADIOL100_V24WARNINT;
	int ret = adiol100_enable_global_irq(dev, mask);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_EQUAL_HEX16(mask, REG(ADIOL100_REG_INTERRUPTG_EN));

	free(dev);
}

/** @brief Test that adiol100_enable_channel_irq() returns -ENODEV for NULL dev pointer. */
void test_adiol100_enable_channel_irq_null_dev(void)
{
	int ret = adiol100_enable_channel_irq(NULL, ADIOL100_CH_A,
					      ADIOL100_ESTCOMSUCINT);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/** @brief Test that adiol100_enable_channel_irq() writes mask to INTEN_A. */
void test_adiol100_enable_channel_irq_ch_a(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	uint16_t mask = ADIOL100_ESTCOMSUCINT | ADIOL100_ESTCOMFAILINT;
	int ret = adiol100_enable_channel_irq(dev, ADIOL100_CH_A, mask);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_EQUAL_HEX16(mask, REG(ADIOL100_REG_INTEN_A));

	free(dev);
}

/** @brief Test that adiol100_enable_channel_irq() routes to INTEN_B for channel B. */
void test_adiol100_enable_channel_irq_ch_b(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	int ret = adiol100_enable_channel_irq(dev, ADIOL100_CH_B,
					      ADIOL100_RXDARDYINT);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_EQUAL_HEX16(ADIOL100_RXDARDYINT, REG(ADIOL100_REG_INTEN_B));

	free(dev);
}

/* ---- Timer / Trigger ---------------------------------------------------- */

/** @brief Test that adiol100_set_cycle_tmr() returns -ENODEV for NULL dev pointer. */
void test_adiol100_set_cycle_tmr_null_dev(void)
{
	int ret = adiol100_set_cycle_tmr(NULL, ADIOL100_CH_A, 0xA5);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/** @brief Test that adiol100_set_cycle_tmr() writes CYCLTMR field. */
void test_adiol100_set_cycle_tmr_ch_a(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	int ret = adiol100_set_cycle_tmr(dev, ADIOL100_CH_A, 0xA5);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_BITS(ADIOL100_CYCLTMR_MSK, 0xA5,
			 REG(ADIOL100_REG_CYCLTMR_A));

	free(dev);
}

/** @brief Test that adiol100_set_cycle_tmr() routes to CYCLTMR_B for channel B. */
void test_adiol100_set_cycle_tmr_ch_b(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	int ret = adiol100_set_cycle_tmr(dev, ADIOL100_CH_B, 0xA5);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_BITS(ADIOL100_CYCLTMR_MSK, 0xA5,
			 REG(ADIOL100_REG_CYCLTMR_B));

	free(dev);
}

/** @brief Test that adiol100_set_cycle_tmr() leaves bits outside CYCLTMR_MSK untouched. */
void test_adiol100_set_cycle_tmr_preserves_other_bits(void)
{
	struct adiol100_dev *dev = create_test_dev();
	int ret;

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	PRESET_OUTSIDE(ADIOL100_REG_CYCLTMR_A, ADIOL100_CYCLTMR_MSK, 1);
	ret = adiol100_set_cycle_tmr(dev, ADIOL100_CH_A, 0xA5);
	TEST_ASSERT_EQUAL_INT(0, ret);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_CYCLTMR_A,
				 ADIOL100_CYCLTMR_MSK, 1);

	PRESET_OUTSIDE(ADIOL100_REG_CYCLTMR_A, ADIOL100_CYCLTMR_MSK, 0);
	ret = adiol100_set_cycle_tmr(dev, ADIOL100_CH_A, 0xA5);
	TEST_ASSERT_EQUAL_INT(0, ret);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_CYCLTMR_A,
				 ADIOL100_CYCLTMR_MSK, 0);

	free(dev);
}

/** @brief Test that adiol100_get_cycle_tmr() returns -ENODEV for NULL dev pointer. */
void test_adiol100_get_cycle_tmr_null_dev(void)
{
	uint16_t tmr;
	int ret = adiol100_get_cycle_tmr(NULL, ADIOL100_CH_A, &tmr);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/** @brief Test that adiol100_get_cycle_tmr() returns -EINVAL for NULL output pointer. */
void test_adiol100_get_cycle_tmr_null_val(void)
{
	struct adiol100_dev *dev = create_test_dev();
	int ret = adiol100_get_cycle_tmr(dev, ADIOL100_CH_A, NULL);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

/** @brief Test that adiol100_get_cycle_tmr() reads back the cycle timer value. */
void test_adiol100_get_cycle_tmr_ch_a(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	REG(ADIOL100_REG_CYCLTMR_A) = 0xA5;

	uint16_t tmr;
	int ret = adiol100_get_cycle_tmr(dev, ADIOL100_CH_A, &tmr);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_BITS(ADIOL100_CYCLTMR_MSK, 0xA5, tmr);

	free(dev);
}

/** @brief Test that adiol100_get_cycle_tmr() reads from CYCLTMR_B for channel B. */
void test_adiol100_get_cycle_tmr_ch_b(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	REG(ADIOL100_REG_CYCLTMR_B) = 0x2A;

	uint16_t tmr;
	int ret = adiol100_get_cycle_tmr(dev, ADIOL100_CH_B, &tmr);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_BITS(ADIOL100_CYCLTMR_MSK, 0x2A, tmr);

	free(dev);
}

/** @brief Test that adiol100_enable_cycle_timer() returns -ENODEV for NULL dev pointer. */
void test_adiol100_enable_cycle_timer_null_dev(void)
{
	int ret = adiol100_enable_cycle_timer(NULL, ADIOL100_CH_A);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/** @brief Test that adiol100_enable_cycle_timer() sets CYCLETMREN in FRAMCTRL1. */
void test_adiol100_enable_cycle_timer_ch_a(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	int ret = adiol100_enable_cycle_timer(dev, ADIOL100_CH_A);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_BITS(ADIOL100_CYCLETMREN, ADIOL100_CYCLETMREN,
			 REG(ADIOL100_REG_FRAMCTRL1_A));

	free(dev);
}

/** @brief Test that adiol100_enable_cycle_timer() routes to FRAMCTRL1_B for channel B. */
void test_adiol100_enable_cycle_timer_ch_b(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	int ret = adiol100_enable_cycle_timer(dev, ADIOL100_CH_B);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_BITS(ADIOL100_CYCLETMREN, ADIOL100_CYCLETMREN,
			 REG(ADIOL100_REG_FRAMCTRL1_B));

	free(dev);
}

/** @brief Test that adiol100_enable_cycle_timer() leaves bits outside CYCLETMREN untouched. */
void test_adiol100_enable_cycle_timer_preserves_other_bits(void)
{
	struct adiol100_dev *dev = create_test_dev();
	int ret;

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	PRESET_OUTSIDE(ADIOL100_REG_FRAMCTRL1_A, ADIOL100_CYCLETMREN, 1);
	ret = adiol100_enable_cycle_timer(dev, ADIOL100_CH_A);
	TEST_ASSERT_EQUAL_INT(0, ret);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_FRAMCTRL1_A,
				 ADIOL100_CYCLETMREN, 1);

	PRESET_OUTSIDE(ADIOL100_REG_FRAMCTRL1_A, ADIOL100_CYCLETMREN, 0);
	ret = adiol100_enable_cycle_timer(dev, ADIOL100_CH_A);
	TEST_ASSERT_EQUAL_INT(0, ret);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_FRAMCTRL1_A,
				 ADIOL100_CYCLETMREN, 0);

	free(dev);
}

/** @brief Test that adiol100_disable_cycle_timer() returns -ENODEV for NULL dev. */
void test_adiol100_disable_cycle_timer_null_dev(void)
{
	int ret = adiol100_disable_cycle_timer(NULL, ADIOL100_CH_A);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/** @brief Test that adiol100_disable_cycle_timer() clears CYCLETMREN in FRAMCTRL1. */
void test_adiol100_disable_cycle_timer_ch_a(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	REG(ADIOL100_REG_FRAMCTRL1_A) = ADIOL100_CYCLETMREN;

	int ret = adiol100_disable_cycle_timer(dev, ADIOL100_CH_A);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_BITS(ADIOL100_CYCLETMREN, 0, REG(ADIOL100_REG_FRAMCTRL1_A));

	free(dev);
}

/** @brief Test that adiol100_disable_cycle_timer() routes to FRAMCTRL1_B for channel B. */
void test_adiol100_disable_cycle_timer_ch_b(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	REG(ADIOL100_REG_FRAMCTRL1_B) = ADIOL100_CYCLETMREN;

	int ret = adiol100_disable_cycle_timer(dev, ADIOL100_CH_B);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_BITS(ADIOL100_CYCLETMREN, 0, REG(ADIOL100_REG_FRAMCTRL1_B));

	free(dev);
}

/** @brief Test that adiol100_disable_cycle_timer() leaves bits outside CYCLETMREN untouched. */
void test_adiol100_disable_cycle_timer_preserves_other_bits(void)
{
	struct adiol100_dev *dev = create_test_dev();
	int ret;

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	PRESET_OUTSIDE(ADIOL100_REG_FRAMCTRL1_A, ADIOL100_CYCLETMREN, 1);
	ret = adiol100_disable_cycle_timer(dev, ADIOL100_CH_A);
	TEST_ASSERT_EQUAL_INT(0, ret);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_FRAMCTRL1_A,
				 ADIOL100_CYCLETMREN, 1);

	PRESET_OUTSIDE(ADIOL100_REG_FRAMCTRL1_A, ADIOL100_CYCLETMREN, 0);
	ret = adiol100_disable_cycle_timer(dev, ADIOL100_CH_A);
	TEST_ASSERT_EQUAL_INT(0, ret);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_FRAMCTRL1_A,
				 ADIOL100_CYCLETMREN, 0);

	free(dev);
}

/** @brief Test that adiol100_config_trigger() returns -ENODEV for NULL dev pointer. */
void test_adiol100_config_trigger_null_dev(void)
{
	int ret = adiol100_config_trigger(NULL, ADIOL100_CH_A,
					  ADIOL100_AUTOCYCLER_DIS,
					  ADIOL100_TRIG_5,
					  ADIOL100_TRIG_EN);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/** @brief Test that adiol100_config_trigger() writes all 3 TRIGASSIGN fields. */
void test_adiol100_config_trigger_ch_a(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	int ret = adiol100_config_trigger(dev, ADIOL100_CH_A,
					  ADIOL100_AUTOCYCLER_KEEP,
					  ADIOL100_TRIG_5,
					  ADIOL100_TRIG_EN);
	TEST_ASSERT_EQUAL_INT(0, ret);

	uint16_t trig = REG(ADIOL100_REG_TRIGASSIGN_A);
	TEST_ASSERT_BITS(ADIOL100_TRIGEN, ADIOL100_TRIGEN, trig);
	TEST_ASSERT_BITS(ADIOL100_AUTOCYCLERDIS, 0, trig);
	TEST_ASSERT_EQUAL_UINT8(ADIOL100_TRIG_5,
				stub_field_get(ADIOL100_TRIGA_MSK, trig));

	free(dev);
}

/** @brief Test that adiol100_config_trigger() routes to TRIGASSIGN_B for channel B. */
void test_adiol100_config_trigger_ch_b(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	int ret = adiol100_config_trigger(dev, ADIOL100_CH_B,
					  ADIOL100_AUTOCYCLER_DIS,
					  ADIOL100_TRIG_5,
					  ADIOL100_TRIG_DIS);
	TEST_ASSERT_EQUAL_INT(0, ret);
	uint16_t trig_b = REG(ADIOL100_REG_TRIGASSIGN_B);
	TEST_ASSERT_BITS(ADIOL100_TRIGEN, 0, trig_b);
	TEST_ASSERT_BITS(ADIOL100_AUTOCYCLERDIS, ADIOL100_AUTOCYCLERDIS, trig_b);
	TEST_ASSERT_EQUAL_UINT8(ADIOL100_TRIG_5,
				stub_field_get(ADIOL100_TRIGA_MSK, trig_b));

	free(dev);
}

/** @brief Test that adiol100_config_trigger() leaves bits outside the TRIGASSIGN fields untouched. */
void test_adiol100_config_trigger_preserves_other_bits(void)
{
	struct adiol100_dev *dev = create_test_dev();
	int ret;
	const uint16_t mask = ADIOL100_AUTOCYCLERDIS | ADIOL100_TRIGA_MSK |
			      ADIOL100_TRIGEN;

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	PRESET_OUTSIDE(ADIOL100_REG_TRIGASSIGN_A, mask, 1);
	ret = adiol100_config_trigger(dev, ADIOL100_CH_A,
				      ADIOL100_AUTOCYCLER_DIS, ADIOL100_TRIG_5,
				      ADIOL100_TRIG_EN);
	TEST_ASSERT_EQUAL_INT(0, ret);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_TRIGASSIGN_A, mask, 1);

	PRESET_OUTSIDE(ADIOL100_REG_TRIGASSIGN_A, mask, 0);
	ret = adiol100_config_trigger(dev, ADIOL100_CH_A,
				      ADIOL100_AUTOCYCLER_DIS, ADIOL100_TRIG_5,
				      ADIOL100_TRIG_EN);
	TEST_ASSERT_EQUAL_INT(0, ret);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_TRIGASSIGN_A, mask, 0);

	free(dev);
}

/** @brief Test that adiol100_fire_trigger() returns -ENODEV for NULL dev pointer. */
void test_adiol100_fire_trigger_null_dev(void)
{
	int ret = adiol100_fire_trigger(NULL, ADIOL100_TRIG_3);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/** @brief Test that adiol100_fire_trigger() writes trigger value to TRIGGER register. */
void test_adiol100_fire_trigger(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	int ret = adiol100_fire_trigger(dev, ADIOL100_TRIG_3);
	TEST_ASSERT_EQUAL_INT(0, ret);

	uint8_t trig_val = stub_field_get(ADIOL100_TRIGGER_MSK,
					  REG(ADIOL100_REG_TRIGGER));
	TEST_ASSERT_EQUAL_UINT8(3, trig_val);

	free(dev);
}

/** @brief Test that adiol100_fire_trigger() leaves bits outside TRIGGER_MSK untouched. */
void test_adiol100_fire_trigger_preserves_other_bits(void)
{
	struct adiol100_dev *dev = create_test_dev();
	int ret;

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	PRESET_OUTSIDE(ADIOL100_REG_TRIGGER, ADIOL100_TRIGGER_MSK, 1);
	ret = adiol100_fire_trigger(dev, ADIOL100_TRIG_3);
	TEST_ASSERT_EQUAL_INT(0, ret);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_TRIGGER, ADIOL100_TRIGGER_MSK, 1);

	PRESET_OUTSIDE(ADIOL100_REG_TRIGGER, ADIOL100_TRIGGER_MSK, 0);
	ret = adiol100_fire_trigger(dev, ADIOL100_TRIG_3);
	TEST_ASSERT_EQUAL_INT(0, ret);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_TRIGGER, ADIOL100_TRIGGER_MSK, 0);

	free(dev);
}

/* ---- Burst / ComRT ------------------------------------------------------ */

/** @brief Test that adiol100_set_burst_len() returns -ENODEV for NULL dev pointer. */
void test_adiol100_set_burst_len_null_dev(void)
{
	int ret = adiol100_set_burst_len(NULL, ADIOL100_CH_A, 10, 20);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/** @brief Test that adiol100_set_burst_len() writes both RDLEN and WRLEN. */
void test_adiol100_set_burst_len_ch_a(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	int ret = adiol100_set_burst_len(dev, ADIOL100_CH_A, 10, 20);
	TEST_ASSERT_EQUAL_INT(0, ret);

	uint16_t burst = REG(ADIOL100_REG_SPIFIFO_A_BURST);
	uint8_t rd = stub_field_get(ADIOL100_BURST_RDLEN_MSK, burst);
	uint8_t wr = stub_field_get(ADIOL100_BURST_WRLEN_MSK, burst);
	TEST_ASSERT_EQUAL_UINT8(20, rd);
	TEST_ASSERT_EQUAL_UINT8(10, wr);

	free(dev);
}

/** @brief Test that adiol100_set_burst_len() routes to SPIFIFO_B_BURST for channel B. */
void test_adiol100_set_burst_len_ch_b(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	int ret = adiol100_set_burst_len(dev, ADIOL100_CH_B, 10, 20);
	TEST_ASSERT_EQUAL_INT(0, ret);

	uint16_t burst_b = REG(ADIOL100_REG_SPIFIFO_B_BURST);
	TEST_ASSERT_EQUAL_UINT8(10,
				stub_field_get(ADIOL100_BURST_WRLEN_MSK, burst_b));
	TEST_ASSERT_EQUAL_UINT8(20,
				stub_field_get(ADIOL100_BURST_RDLEN_MSK, burst_b));

	free(dev);
}

/** @brief Test that adiol100_get_comrt() returns -ENODEV for NULL dev pointer. */
void test_adiol100_get_comrt_null_dev(void)
{
	uint8_t comrt;
	int ret = adiol100_get_comrt(NULL, ADIOL100_CH_A, &comrt);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/** @brief Test that adiol100_get_comrt() returns -EINVAL for NULL output pointer. */
void test_adiol100_get_comrt_null_val(void)
{
	struct adiol100_dev *dev = create_test_dev();
	int ret = adiol100_get_comrt(dev, ADIOL100_CH_A, NULL);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

/** @brief Test that adiol100_get_comrt() extracts COMRT field from FRAMCTRL1. */
void test_adiol100_get_comrt_ch_a(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	/* Set COMRT = 3 (COM3) in FRAMCTRL1_A bits [7:6] */
	REG(ADIOL100_REG_FRAMCTRL1_A) = stub_field_prep(ADIOL100_COMRT_MSK, 3);

	uint8_t comrt;
	int ret = adiol100_get_comrt(dev, ADIOL100_CH_A, &comrt);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_EQUAL_UINT8(3, comrt);

	free(dev);
}

/** @brief Test that adiol100_get_comrt() reads from FRAMCTRL1_B for channel B. */
void test_adiol100_get_comrt_ch_b(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	REG(ADIOL100_REG_FRAMCTRL1_B) = stub_field_prep(ADIOL100_COMRT_MSK, 2);

	uint8_t comrt;
	int ret = adiol100_get_comrt(dev, ADIOL100_CH_B, &comrt);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_EQUAL_UINT8(2, comrt);

	free(dev);
}

/* ---- Messaging / FIFO --------------------------------------------------- */

/** @brief Test that adiol100_load_msg() returns -ENODEV for NULL dev pointer. */
void test_adiol100_load_msg_null_dev(void)
{
	uint8_t buf[4];
	int ret = adiol100_load_msg(NULL, ADIOL100_CH_A, buf, 3, 2,
				    ADIOL100_KEEP_MSG);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/** @brief Test that adiol100_load_msg() returns -EINVAL for NULL data pointer. */
void test_adiol100_load_msg_null_data(void)
{
	struct adiol100_dev *dev = create_test_dev();
	int ret = adiol100_load_msg(dev, ADIOL100_CH_A, NULL, 3, 2,
				    ADIOL100_KEEP_MSG);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

/** @brief Test that adiol100_load_msg() returns -EINVAL for invalid channel. */
void test_adiol100_load_msg_bad_channel(void)
{
	struct adiol100_dev *dev = create_test_dev();
	uint8_t buf[4];
	int ret = adiol100_load_msg(dev, 99, buf, 3, 2, ADIOL100_KEEP_MSG);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

/** @brief Test that adiol100_load_msg() rejects txbytes > TX_MSG_MAX_LEN. */
void test_adiol100_load_msg_tx_too_long(void)
{
	struct adiol100_dev *dev = create_test_dev();
	uint8_t buf[128];
	int ret = adiol100_load_msg(dev, ADIOL100_CH_A, buf,
				    ADIOL100_TX_MSG_MAX_LEN + 1, 2,
				    ADIOL100_KEEP_MSG);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

/** @brief Test that adiol100_load_msg() rejects rxbytes > RX_MSG_MAX_LEN. */
void test_adiol100_load_msg_rx_too_long(void)
{
	struct adiol100_dev *dev = create_test_dev();
	uint8_t buf[4];
	int ret = adiol100_load_msg(dev, ADIOL100_CH_A, buf, 3,
				    ADIOL100_RX_MSG_MAX_LEN + 1,
				    ADIOL100_KEEP_MSG);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

/** @brief Test that adiol100_load_msg() writes MsgID/RxBytes/TxBytes/payload and sets TXKEEPMSG. */
void test_adiol100_load_msg_ch_a(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	g_fifo_tx_len = 0;
	dev->msg_id[ADIOL100_CH_A] = 5;
	uint8_t payload[] = { 0xA2, 0x80, 0x55 };
	int ret = adiol100_load_msg(dev, ADIOL100_CH_A, payload, 3, 2,
				    ADIOL100_KEEP_MSG);
	TEST_ASSERT_EQUAL_INT(0, ret);

	/* Verify FIFO contents: MsgID, RxBytes, TxBytes, payload */
	TEST_ASSERT_EQUAL_UINT8(5, g_fifo_tx[0]);
	TEST_ASSERT_EQUAL_UINT8(2, g_fifo_tx[1]);
	TEST_ASSERT_EQUAL_UINT8(3, g_fifo_tx[2]);
	TEST_ASSERT_EQUAL_UINT8(0xA2, g_fifo_tx[3]);
	TEST_ASSERT_EQUAL_UINT8(0x80, g_fifo_tx[4]);
	TEST_ASSERT_EQUAL_UINT8(0x55, g_fifo_tx[5]);

	/* Verify TXKEEPMSG was set */
	TEST_ASSERT_BITS(ADIOL100_TXKEEPMSG, ADIOL100_TXKEEPMSG,
			 REG(ADIOL100_REG_FRAMCTRL1_A));

	TEST_ASSERT_EQUAL_UINT8(6, dev->msg_id[ADIOL100_CH_A]);

	free(dev);
}

/** @brief Test that adiol100_load_msg() increments msg_id on each call. */
void test_adiol100_load_msg_increments_id(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	uint8_t payload[] = { 0xA2 };

	adiol100_load_msg(dev, ADIOL100_CH_A, payload, 1, 1,
			  ADIOL100_KEEP_MSG);
	TEST_ASSERT_EQUAL_UINT8(1, dev->msg_id[ADIOL100_CH_A]);

	adiol100_load_msg(dev, ADIOL100_CH_A, payload, 1, 1,
			  ADIOL100_KEEP_MSG);
	TEST_ASSERT_EQUAL_UINT8(2, dev->msg_id[ADIOL100_CH_A]);

	free(dev);
}

/** @brief Test that adiol100_load_msg() routes to TXRXDATA_B for channel B. */
void test_adiol100_load_msg_ch_b(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	g_fifo_tx_len = 0;
	uint8_t payload[] = { 0xBB };
	int ret = adiol100_load_msg(dev, ADIOL100_CH_B, payload, 1, 1,
				    ADIOL100_KEEP_MSG);
	TEST_ASSERT_EQUAL_INT(0, ret);

	TEST_ASSERT_EQUAL_UINT8(0xBB, g_fifo_tx[3]);
	TEST_ASSERT_BITS(ADIOL100_TXKEEPMSG, ADIOL100_TXKEEPMSG,
			 REG(ADIOL100_REG_FRAMCTRL1_B));
	TEST_ASSERT_EQUAL_UINT8(1, dev->msg_id[ADIOL100_CH_B]);

	free(dev);
}

/** @brief Test that adiol100_load_msg() leaves bits outside TXKEEPMSG untouched. */
void test_adiol100_load_msg_preserves_other_bits(void)
{
	struct adiol100_dev *dev = create_test_dev();
	int ret;
	uint8_t payload[] = { 0xA2 };

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	PRESET_OUTSIDE(ADIOL100_REG_FRAMCTRL1_A, ADIOL100_TXKEEPMSG, 1);
	ret = adiol100_load_msg(dev, ADIOL100_CH_A, payload, 1, 1,
				ADIOL100_KEEP_MSG);
	TEST_ASSERT_EQUAL_INT(0, ret);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_FRAMCTRL1_A,
				 ADIOL100_TXKEEPMSG, 1);

	PRESET_OUTSIDE(ADIOL100_REG_FRAMCTRL1_A, ADIOL100_TXKEEPMSG, 0);
	ret = adiol100_load_msg(dev, ADIOL100_CH_A, payload, 1, 1,
				ADIOL100_KEEP_MSG);
	TEST_ASSERT_EQUAL_INT(0, ret);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_FRAMCTRL1_A,
				 ADIOL100_TXKEEPMSG, 0);

	free(dev);
}

/** @brief Test that adiol100_send_msg() returns -ENODEV for NULL dev pointer. */
void test_adiol100_send_msg_null_dev(void)
{
	int ret = adiol100_send_msg(NULL, ADIOL100_CH_A);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/** @brief Test that adiol100_send_msg() sets CQSEND in FRAMCTRL1_A. */
void test_adiol100_send_msg_ch_a(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	int ret = adiol100_send_msg(dev, ADIOL100_CH_A);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_BITS(ADIOL100_CQSEND, ADIOL100_CQSEND,
			 REG(ADIOL100_REG_FRAMCTRL1_A));

	free(dev);
}

/** @brief Test that adiol100_send_msg() routes to FRAMCTRL1_B for channel B. */
void test_adiol100_send_msg_ch_b(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	int ret = adiol100_send_msg(dev, ADIOL100_CH_B);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_BITS(ADIOL100_CQSEND, ADIOL100_CQSEND,
			 REG(ADIOL100_REG_FRAMCTRL1_B));

	free(dev);
}

/** @brief Test that adiol100_send_msg() leaves bits outside CQSEND untouched. */
void test_adiol100_send_msg_preserves_other_bits(void)
{
	struct adiol100_dev *dev = create_test_dev();
	int ret;

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	PRESET_OUTSIDE(ADIOL100_REG_FRAMCTRL1_A, ADIOL100_CQSEND, 1);
	ret = adiol100_send_msg(dev, ADIOL100_CH_A);
	TEST_ASSERT_EQUAL_INT(0, ret);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_FRAMCTRL1_A, ADIOL100_CQSEND, 1);

	PRESET_OUTSIDE(ADIOL100_REG_FRAMCTRL1_A, ADIOL100_CQSEND, 0);
	ret = adiol100_send_msg(dev, ADIOL100_CH_A);
	TEST_ASSERT_EQUAL_INT(0, ret);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_FRAMCTRL1_A, ADIOL100_CQSEND, 0);

	free(dev);
}

/** @brief Test that adiol100_load_and_send_msg() returns -ENODEV for NULL dev. */
void test_adiol100_load_and_send_msg_null_dev(void)
{
	uint8_t buf[4];
	int ret = adiol100_load_and_send_msg(NULL, ADIOL100_CH_A,
					     buf, 3, 2,
					     ADIOL100_KEEP_MSG, 0);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/** @brief Test that adiol100_load_and_send_msg() returns -EINVAL for NULL data. */
void test_adiol100_load_and_send_msg_null_data(void)
{
	struct adiol100_dev *dev = create_test_dev();
	int ret = adiol100_load_and_send_msg(dev, ADIOL100_CH_A,
					     NULL, 3, 2,
					     ADIOL100_KEEP_MSG, 0);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

/** @brief Test that adiol100_load_and_send_msg() rejects invalid channel. */
void test_adiol100_load_and_send_msg_bad_ch(void)
{
	struct adiol100_dev *dev = create_test_dev();
	uint8_t buf[4];
	int ret = adiol100_load_and_send_msg(dev, 99,
					     buf, 3, 2,
					     ADIOL100_KEEP_MSG, 0);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

/** @brief Test that adiol100_load_and_send_msg() rejects txbytes > TX_MSG_MAX_LEN. */
void test_adiol100_load_and_send_msg_tx_too_long(void)
{
	struct adiol100_dev *dev = create_test_dev();
	uint8_t buf[128];
	int ret = adiol100_load_and_send_msg(dev, ADIOL100_CH_A,
					     buf,
					     ADIOL100_TX_MSG_MAX_LEN + 1, 2,
					     ADIOL100_KEEP_MSG, 0);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

/** @brief Test that adiol100_load_and_send_msg() rejects rxbytes > RX_MSG_MAX_LEN. */
void test_adiol100_load_and_send_msg_rx_too_long(void)
{
	struct adiol100_dev *dev = create_test_dev();
	uint8_t buf[4];
	int ret = adiol100_load_and_send_msg(dev, ADIOL100_CH_A,
					     buf, 3,
					     ADIOL100_RX_MSG_MAX_LEN + 1,
					     ADIOL100_KEEP_MSG, 0);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

/** @brief Test that adiol100_load_and_send_msg() writes FIFO + appends the caller's FramCtrl1 with CQSEND and TXKEEPMSG. */
void test_adiol100_load_and_send_msg(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	g_fifo_tx_len = 0;
	dev->msg_id[ADIOL100_CH_A] = 5;
	uint8_t payload[] = { 0xA2, 0x80 };
	int ret = adiol100_load_and_send_msg(dev, ADIOL100_CH_A,
					     payload, 2, 2,
					     ADIOL100_KEEP_MSG,
					     ADIOL100_FRAMEREN);
	TEST_ASSERT_EQUAL_INT(0, ret);

	/* Verify FIFO contents: MsgID, RxBytes, TxBytes, payload */
	TEST_ASSERT_EQUAL_UINT8(5, g_fifo_tx[0]);
	TEST_ASSERT_EQUAL_UINT8(2, g_fifo_tx[1]);
	TEST_ASSERT_EQUAL_UINT8(2, g_fifo_tx[2]);
	TEST_ASSERT_EQUAL_UINT8(0xA2, g_fifo_tx[3]);
	TEST_ASSERT_EQUAL_UINT8(0x80, g_fifo_tx[4]);

	/* Verify FramCtrl1 appended after FIFO data is the caller's value plus
	 * CQSEND and TXKEEPMSG, and nothing else */
	uint16_t fc1 = stub_get_unaligned_be16(&g_fifo_tx[5]);
	TEST_ASSERT_EQUAL_HEX16(ADIOL100_FRAMEREN | ADIOL100_CQSEND |
				ADIOL100_TXKEEPMSG, fc1);

	TEST_ASSERT_EQUAL_UINT8(6, dev->msg_id[ADIOL100_CH_A]);

	free(dev);
}

/** @brief Test that adiol100_read_msg() returns -ENODEV for NULL dev pointer. */
void test_adiol100_read_msg_null_dev(void)
{
	uint8_t buf[4];
	int ret = adiol100_read_msg(NULL, ADIOL100_CH_A, buf, 4);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/** @brief Test that adiol100_read_msg() returns -EINVAL for NULL data pointer. */
void test_adiol100_read_msg_null_data(void)
{
	struct adiol100_dev *dev = create_test_dev();
	int ret = adiol100_read_msg(dev, ADIOL100_CH_A, NULL, 4);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

/** @brief Test that adiol100_read_msg() rejects data_len > RX_MSG_MAX_LEN. */
void test_adiol100_read_msg_too_long(void)
{
	struct adiol100_dev *dev = create_test_dev();
	uint8_t buf[128];
	int ret = adiol100_read_msg(dev, ADIOL100_CH_A, buf,
				    ADIOL100_RX_MSG_MAX_LEN + 1);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

/** @brief Test that adiol100_read_msg() extracts payload from pre-filled RX FIFO. */
void test_adiol100_read_msg_ch_a(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	/* Pre-fill RX FIFO: MsgID, RxBytesAct, then payload */
	g_fifo_rx[0] = 0x07;
	g_fifo_rx[1] = 0x02;
	g_fifo_rx[2] = 0xDE;
	g_fifo_rx[3] = 0xAD;
	g_fifo_rx_len = 4;

	uint8_t data[2];
	int ret = adiol100_read_msg(dev, ADIOL100_CH_A, data, 2);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_EQUAL_UINT8(0xDE, data[0]);
	TEST_ASSERT_EQUAL_UINT8(0xAD, data[1]);

	free(dev);
}

/** @brief Test that adiol100_read_msg() routes to TXRXDATA_B for channel B. */
void test_adiol100_read_msg_ch_b(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	g_fifo_rx[0] = 0x01;
	g_fifo_rx[1] = 0x01;
	g_fifo_rx[2] = 0xCC;
	g_fifo_rx_len = 3;

	uint8_t data[1];
	int ret = adiol100_read_msg(dev, ADIOL100_CH_B, data, 1);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_EQUAL_UINT8(0xCC, data[0]);

	free(dev);
}

/** @brief Test that adiol100_read_channel_burst() returns -ENODEV for NULL dev. */
void test_adiol100_read_channel_burst_null_dev(void)
{
	uint16_t rxstat, irq, status;
	uint8_t data[4];
	int ret = adiol100_read_channel_burst(NULL, ADIOL100_CH_A,
					      &rxstat, &irq, &status,
					      data, 4);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/** @brief Test that adiol100_read_channel_burst() rejects NULL output pointers. */
void test_adiol100_read_channel_burst_null_ptrs(void)
{
	struct adiol100_dev *dev = create_test_dev();
	uint16_t rxstat, irq, status;
	uint8_t data[4];

	int ret = adiol100_read_channel_burst(dev, ADIOL100_CH_A,
					      NULL, &irq, &status, data, 4);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);

	ret = adiol100_read_channel_burst(dev, ADIOL100_CH_A,
					  &rxstat, NULL, &status, data, 4);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);

	ret = adiol100_read_channel_burst(dev, ADIOL100_CH_A,
					  &rxstat, &irq, NULL, data, 4);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);

	ret = adiol100_read_channel_burst(dev, ADIOL100_CH_A,
					  &rxstat, &irq, &status, NULL, 4);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);

	free(dev);
}

/** @brief Test that adiol100_read_channel_burst() rejects data_len > RX_MSG_MAX_LEN. */
void test_adiol100_read_channel_burst_too_long(void)
{
	struct adiol100_dev *dev = create_test_dev();
	uint16_t rxstat, irq, status;
	uint8_t data[128];
	int ret = adiol100_read_channel_burst(dev, ADIOL100_CH_A,
					      &rxstat, &irq, &status,
					      data,
					      ADIOL100_RX_MSG_MAX_LEN + 1);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

/** @brief Test that adiol100_read_channel_burst() returns 3 regs + FIFO payload. */
void test_adiol100_read_channel_burst(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	/* Pre-fill the 3 registers the burst reads */
	REG(ADIOL100_REG_RXFIFOSTAT_A) = 0x1234;
	REG(ADIOL100_REG_INTERRUPT_A) = 0x5678;
	REG(ADIOL100_REG_STATUS_A) = 0x9ABC;

	/* Pre-fill RX FIFO: MsgID, RxBytesAct, then payload */
	g_fifo_rx[0] = 0x03;
	g_fifo_rx[1] = 0x02;
	g_fifo_rx[2] = 0xCA;
	g_fifo_rx[3] = 0xFE;
	g_fifo_rx_len = 4;

	uint16_t rxstat, irq, status;
	uint8_t data[2];
	int ret = adiol100_read_channel_burst(dev, ADIOL100_CH_A,
					      &rxstat, &irq, &status,
					      data, 2);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_EQUAL_HEX16(0x1234, rxstat);
	TEST_ASSERT_EQUAL_HEX16(0x5678, irq);
	TEST_ASSERT_EQUAL_HEX16(0x9ABC, status);
	TEST_ASSERT_EQUAL_UINT8(0xCA, data[0]);
	TEST_ASSERT_EQUAL_UINT8(0xFE, data[1]);

	free(dev);
}

/* ---- ADC ---------------------------------------------------------------- */

/** @brief Test that adiol100_read_adc_global() returns -ENODEV for NULL dev pointer. */
void test_adiol100_read_adc_global_null_dev(void)
{
	uint16_t val;
	int ret = adiol100_read_adc_global(NULL, ADIOL100_ADC_V24_VOLT, &val);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/** @brief Test that adiol100_read_adc_global() returns -EINVAL for NULL output pointer. */
void test_adiol100_read_adc_global_null_val(void)
{
	struct adiol100_dev *dev = create_test_dev();
	int ret = adiol100_read_adc_global(dev, ADIOL100_ADC_V24_VOLT, NULL);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

/** @brief Test that adiol100_read_adc_global() masks to 13 bits, stripping upper bits. */
void test_adiol100_read_adc_global(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	REG(ADIOL100_REG_V24VOLT) = 0xEABC;

	uint16_t val;
	int ret = adiol100_read_adc_global(dev, ADIOL100_ADC_V24_VOLT, &val);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_EQUAL_HEX16(0x0ABC, val);

	free(dev);
}

/** @brief Test that adiol100_read_adc_chan() returns -ENODEV for NULL dev pointer. */
void test_adiol100_read_adc_chan_null_dev(void)
{
	uint16_t val;
	int ret = adiol100_read_adc_chan(NULL, ADIOL100_ADC_CQ_VOLT,
					 ADIOL100_CH_A, &val);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/** @brief Test that adiol100_read_adc_chan() returns -EINVAL for NULL output pointer. */
void test_adiol100_read_adc_chan_null_val(void)
{
	struct adiol100_dev *dev = create_test_dev();
	int ret = adiol100_read_adc_chan(dev, ADIOL100_ADC_CQ_VOLT,
					 ADIOL100_CH_A, NULL);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

/** @brief Test that adiol100_read_adc_chan() masks to 13 bits and applies channel B offset. */
void test_adiol100_read_adc_chan(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	/* Channel A: CQ voltage, upper bits should be masked off */
	REG(ADIOL100_REG_CQAVOLT) = 0xF123;

	uint16_t val;
	int ret = adiol100_read_adc_chan(dev, ADIOL100_ADC_CQ_VOLT,
					 ADIOL100_CH_A, &val);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_EQUAL_HEX16(0x1123, val);

	/* Channel B: CQ voltage is at offset +0x10 */
	REG(ADIOL100_REG_CQBVOLT) = 0x0456;
	ret = adiol100_read_adc_chan(dev, ADIOL100_ADC_CQ_VOLT,
				     ADIOL100_CH_B, &val);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_EQUAL_HEX16(0x0456, val);

	free(dev);
}

/** @brief Test that adiol100_reset_adc_stats() returns -ENODEV for NULL dev pointer. */
void test_adiol100_reset_adc_stats_null_dev(void)
{
	int ret = adiol100_reset_adc_stats(NULL);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/** @brief Test that adiol100_reset_adc_stats() sets all 6 reset bits in CQMEASCONF. */
void test_adiol100_reset_adc_stats(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	int ret = adiol100_reset_adc_stats(dev);
	TEST_ASSERT_EQUAL_INT(0, ret);

	uint16_t conf = REG(ADIOL100_REG_CQMEASCONF);
	TEST_ASSERT_BITS(ADIOL100_TEMPRESSTAT, ADIOL100_TEMPRESSTAT, conf);
	TEST_ASSERT_BITS(ADIOL100_RESV24STAT, ADIOL100_RESV24STAT, conf);
	TEST_ASSERT_BITS(ADIOL100_RESCQSTATA, ADIOL100_RESCQSTATA, conf);
	TEST_ASSERT_BITS(ADIOL100_RESCQSTATB, ADIOL100_RESCQSTATB, conf);
	TEST_ASSERT_BITS(ADIOL100_RESLPSTATA, ADIOL100_RESLPSTATA, conf);
	TEST_ASSERT_BITS(ADIOL100_RESLPSTATB, ADIOL100_RESLPSTATB, conf);

	free(dev);
}

/** @brief Test that adiol100_reset_adc_stats() leaves bits outside the six reset bits untouched. */
void test_adiol100_reset_adc_stats_preserves_other_bits(void)
{
	struct adiol100_dev *dev = create_test_dev();
	int ret;
	const uint16_t mask = ADIOL100_TEMPRESSTAT | ADIOL100_RESV24STAT |
			      ADIOL100_RESLPSTATB | ADIOL100_RESLPSTATA |
			      ADIOL100_RESCQSTATB | ADIOL100_RESCQSTATA;

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	PRESET_OUTSIDE(ADIOL100_REG_CQMEASCONF, mask, 1);
	ret = adiol100_reset_adc_stats(dev);
	TEST_ASSERT_EQUAL_INT(0, ret);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_CQMEASCONF, mask, 1);

	PRESET_OUTSIDE(ADIOL100_REG_CQMEASCONF, mask, 0);
	ret = adiol100_reset_adc_stats(dev);
	TEST_ASSERT_EQUAL_INT(0, ret);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_CQMEASCONF, mask, 0);

	free(dev);
}

/* ---- LED ---------------------------------------------------------------- */

/** @brief Test that adiol100_set_led_brightness() returns -ENODEV for NULL dev. */
void test_adiol100_set_led_brightness_null_dev(void)
{
	int ret = adiol100_set_led_brightness(NULL, ADIOL100_LED1,
					      ADIOL100_LED_BGT_50,
					      ADIOL100_LED_BGT_50);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/** @brief Test that adiol100_set_led_brightness() rejects invalid LED index. */
void test_adiol100_set_led_brightness_bad_led(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	int ret = adiol100_set_led_brightness(dev, 99,
					      ADIOL100_LED_BGT_50,
					      ADIOL100_LED_BGT_50);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);

	free(dev);
}

/** @brief Test that adiol100_set_led_brightness() writes red/green fields of each LED in LED12BGT/LED34BGT. */
void test_adiol100_set_led_brightness(void)
{
	struct adiol100_dev *dev = create_test_dev();
	const enum adiol100_led leds[] = {
		ADIOL100_LED1, ADIOL100_LED2, ADIOL100_LED3, ADIOL100_LED4,
	};
	const uint16_t regs[] = {
		ADIOL100_REG_LED12BGT, ADIOL100_REG_LED12BGT,
		ADIOL100_REG_LED34BGT, ADIOL100_REG_LED34BGT,
	};
	const uint16_t red_masks[] = {
		ADIOL100_LED_LO_RED_MSK, ADIOL100_LED_HI_RED_MSK,
		ADIOL100_LED_LO_RED_MSK, ADIOL100_LED_HI_RED_MSK,
	};
	const uint16_t green_masks[] = {
		ADIOL100_LED_LO_GREEN_MSK, ADIOL100_LED_HI_GREEN_MSK,
		ADIOL100_LED_LO_GREEN_MSK, ADIOL100_LED_HI_GREEN_MSK,
	};
	int ret;

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	for (int i = 0; i < 4; i++) {
		REG(regs[i]) = 0;

		ret = adiol100_set_led_brightness(dev, leds[i],
						  ADIOL100_LED_BGT_100,
						  ADIOL100_LED_BGT_50);
		TEST_ASSERT_EQUAL_INT(0, ret);
		TEST_ASSERT_EQUAL_UINT8(ADIOL100_LED_BGT_100,
					stub_field_get(red_masks[i], REG(regs[i])));
		TEST_ASSERT_EQUAL_UINT8(ADIOL100_LED_BGT_50,
					stub_field_get(green_masks[i], REG(regs[i])));
	}

	free(dev);
}

/** @brief Test that adiol100_set_led_brightness() only changes the selected LED's fields. */
void test_adiol100_set_led_brightness_preserves_other_bits(void)
{
	struct adiol100_dev *dev = create_test_dev();
	const enum adiol100_led leds[] = {
		ADIOL100_LED1, ADIOL100_LED2, ADIOL100_LED3, ADIOL100_LED4,
	};
	const uint16_t regs[] = {
		ADIOL100_REG_LED12BGT, ADIOL100_REG_LED12BGT,
		ADIOL100_REG_LED34BGT, ADIOL100_REG_LED34BGT,
	};
	const uint16_t masks[] = {
		ADIOL100_LED_LO_GREEN_MSK | ADIOL100_LED_LO_RED_MSK,
		ADIOL100_LED_HI_GREEN_MSK | ADIOL100_LED_HI_RED_MSK,
		ADIOL100_LED_LO_GREEN_MSK | ADIOL100_LED_LO_RED_MSK,
		ADIOL100_LED_HI_GREEN_MSK | ADIOL100_LED_HI_RED_MSK,
	};
	int ret;

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	for (int i = 0; i < 4; i++) {
		for (int fill = 0; fill <= 1; fill++) {
			PRESET_OUTSIDE(regs[i], masks[i], fill);
			ret = adiol100_set_led_brightness(dev, leds[i],
							  ADIOL100_LED_BGT_100,
							  ADIOL100_LED_BGT_50);
			TEST_ASSERT_EQUAL_INT(0, ret);
			ASSERT_OUTSIDE_UNCHANGED(regs[i], masks[i], fill);
		}
	}

	free(dev);
}

/** @brief Test that adiol100_set_led_sequence() returns -ENODEV for NULL dev. */
void test_adiol100_set_led_sequence_null_dev(void)
{
	int ret = adiol100_set_led_sequence(NULL, ADIOL100_LED1,
					    ADIOL100_LED_SEQ_OFF,
					    ADIOL100_LED_SEQ_OFF);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/** @brief Test that adiol100_set_led_sequence() rejects invalid LED index. */
void test_adiol100_set_led_sequence_bad_led(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	int ret = adiol100_set_led_sequence(dev, 99,
					    ADIOL100_LED_SEQ_OFF,
					    ADIOL100_LED_SEQ_OFF);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);

	free(dev);
}

/** @brief Test that adiol100_set_led_sequence() writes red and green sequence registers. */
void test_adiol100_set_led_sequence(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	int ret = adiol100_set_led_sequence(dev, ADIOL100_LED1,
					    ADIOL100_LED_SEQ_ON,
					    ADIOL100_LED_SEQ_BLINK_SLOW);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_EQUAL_HEX16(ADIOL100_LED_SEQ_ON,
				REG(ADIOL100_REG_LED1RSEQ));
	TEST_ASSERT_EQUAL_HEX16(ADIOL100_LED_SEQ_BLINK_SLOW,
				REG(ADIOL100_REG_LED1GSEQ));

	free(dev);
}

/** @brief Test that adiol100_set_led_mode() returns -ENODEV for NULL dev pointer. */
void test_adiol100_set_led_mode_null_dev(void)
{
	int ret = adiol100_set_led_mode(NULL, ADIOL100_LED1,
					ADIOL100_LED_BLINK);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/** @brief Test that adiol100_set_led_mode() rejects invalid LED index. */
void test_adiol100_set_led_mode_bad_led(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	int ret = adiol100_set_led_mode(dev, 99, ADIOL100_LED_BLINK);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);

	free(dev);
}

/** @brief Test that adiol100_set_led_mode() writes the LEDnCFG field of each LED in LEDCONFIG. */
void test_adiol100_set_led_mode(void)
{
	struct adiol100_dev *dev = create_test_dev();
	const enum adiol100_led leds[] = {
		ADIOL100_LED1, ADIOL100_LED2, ADIOL100_LED3, ADIOL100_LED4,
	};
	const uint16_t masks[] = {
		ADIOL100_LED1CFG_MSK, ADIOL100_LED2CFG_MSK,
		ADIOL100_LED3CFG_MSK, ADIOL100_LED4CFG_MSK,
	};
	int ret;

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	for (int i = 0; i < 4; i++) {
		REG(ADIOL100_REG_LEDCONFIG) = 0;

		ret = adiol100_set_led_mode(dev, leds[i], ADIOL100_LED_BLINK_FAST);
		TEST_ASSERT_EQUAL_INT(0, ret);
		TEST_ASSERT_EQUAL_UINT8(ADIOL100_LED_BLINK_FAST,
					stub_field_get(masks[i],
						       REG(ADIOL100_REG_LEDCONFIG)));
	}

	free(dev);
}

/** @brief Test that adiol100_set_led_mode() only changes the selected LED's CFG field. */
void test_adiol100_set_led_mode_preserves_other_bits(void)
{
	struct adiol100_dev *dev = create_test_dev();
	const enum adiol100_led leds[] = {
		ADIOL100_LED1, ADIOL100_LED2, ADIOL100_LED3, ADIOL100_LED4,
	};
	const uint16_t masks[] = {
		ADIOL100_LED1CFG_MSK, ADIOL100_LED2CFG_MSK,
		ADIOL100_LED3CFG_MSK, ADIOL100_LED4CFG_MSK,
	};
	int ret;

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	for (int i = 0; i < 4; i++) {
		for (int fill = 0; fill <= 1; fill++) {
			PRESET_OUTSIDE(ADIOL100_REG_LEDCONFIG, masks[i], fill);
			ret = adiol100_set_led_mode(dev, leds[i],
						    ADIOL100_LED_BLINK);
			TEST_ASSERT_EQUAL_INT(0, ret);
			ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_LEDCONFIG,
						 masks[i], fill);
		}
	}

	free(dev);
}

/** @brief Test that adiol100_reset_leds() returns -ENODEV for NULL dev pointer. */
void test_adiol100_reset_leds_null_dev(void)
{
	int ret = adiol100_reset_leds(NULL);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/** @brief Test that adiol100_reset_leds() clears all 8 sequences and sets modes to BLINK. */
void test_adiol100_reset_leds(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	/* SEQ_OFF and LED_BLINK are both 0, so start non-zero to check if it resets */
	REG(ADIOL100_REG_LEDCONFIG) = 0xFFFF;
	REG(ADIOL100_REG_LED1RSEQ) = 0xFFFF;
	REG(ADIOL100_REG_LED1GSEQ) = 0xFFFF;
	REG(ADIOL100_REG_LED2RSEQ) = 0xFFFF;
	REG(ADIOL100_REG_LED2GSEQ) = 0xFFFF;
	REG(ADIOL100_REG_LED3RSEQ) = 0xFFFF;
	REG(ADIOL100_REG_LED3GSEQ) = 0xFFFF;
	REG(ADIOL100_REG_LED4RSEQ) = 0xFFFF;
	REG(ADIOL100_REG_LED4GSEQ) = 0xFFFF;

	int ret = adiol100_reset_leds(dev);
	TEST_ASSERT_EQUAL_INT(0, ret);

	TEST_ASSERT_EQUAL_HEX16(ADIOL100_LED_SEQ_OFF, REG(ADIOL100_REG_LED1RSEQ));
	TEST_ASSERT_EQUAL_HEX16(ADIOL100_LED_SEQ_OFF, REG(ADIOL100_REG_LED1GSEQ));
	TEST_ASSERT_EQUAL_HEX16(ADIOL100_LED_SEQ_OFF, REG(ADIOL100_REG_LED2RSEQ));
	TEST_ASSERT_EQUAL_HEX16(ADIOL100_LED_SEQ_OFF, REG(ADIOL100_REG_LED2GSEQ));
	TEST_ASSERT_EQUAL_HEX16(ADIOL100_LED_SEQ_OFF, REG(ADIOL100_REG_LED3RSEQ));
	TEST_ASSERT_EQUAL_HEX16(ADIOL100_LED_SEQ_OFF, REG(ADIOL100_REG_LED3GSEQ));
	TEST_ASSERT_EQUAL_HEX16(ADIOL100_LED_SEQ_OFF, REG(ADIOL100_REG_LED4RSEQ));
	TEST_ASSERT_EQUAL_HEX16(ADIOL100_LED_SEQ_OFF, REG(ADIOL100_REG_LED4GSEQ));

	uint16_t ledcfg = REG(ADIOL100_REG_LEDCONFIG);
	TEST_ASSERT_EQUAL_UINT8(ADIOL100_LED_BLINK,
				stub_field_get(ADIOL100_LED1CFG_MSK, ledcfg));
	TEST_ASSERT_EQUAL_UINT8(ADIOL100_LED_BLINK,
				stub_field_get(ADIOL100_LED2CFG_MSK, ledcfg));
	TEST_ASSERT_EQUAL_UINT8(ADIOL100_LED_BLINK,
				stub_field_get(ADIOL100_LED3CFG_MSK, ledcfg));
	TEST_ASSERT_EQUAL_UINT8(ADIOL100_LED_BLINK,
				stub_field_get(ADIOL100_LED4CFG_MSK, ledcfg));

	free(dev);
}

/** @brief Test that adiol100_led_sync() returns -ENODEV for NULL dev pointer. */
void test_adiol100_led_sync_null_dev(void)
{
	int ret = adiol100_led_sync(NULL);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/** @brief Test that adiol100_led_sync() sets LEDSYNC in TRIGGER register. */
void test_adiol100_led_sync(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	int ret = adiol100_led_sync(dev);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_BITS(ADIOL100_LEDSYNC, ADIOL100_LEDSYNC,
			 REG(ADIOL100_REG_TRIGGER));

	free(dev);
}

/** @brief Test that adiol100_led_sync() leaves bits outside LEDSYNC untouched. */
void test_adiol100_led_sync_preserves_other_bits(void)
{
	struct adiol100_dev *dev = create_test_dev();
	int ret;

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	PRESET_OUTSIDE(ADIOL100_REG_TRIGGER, ADIOL100_LEDSYNC, 1);
	ret = adiol100_led_sync(dev);
	TEST_ASSERT_EQUAL_INT(0, ret);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_TRIGGER, ADIOL100_LEDSYNC, 1);

	PRESET_OUTSIDE(ADIOL100_REG_TRIGGER, ADIOL100_LEDSYNC, 0);
	ret = adiol100_led_sync(dev);
	TEST_ASSERT_EQUAL_INT(0, ret);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_TRIGGER, ADIOL100_LEDSYNC, 0);

	free(dev);
}

/* ---- Status ------------------------------------------------------------- */

/** @brief Test that adiol100_get_status() returns -ENODEV for NULL dev pointer. */
void test_adiol100_get_status_null_dev(void)
{
	uint16_t flags;
	int ret = adiol100_get_status(NULL, ADIOL100_CH_A, &flags);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/** @brief Test that adiol100_get_status() returns -EINVAL for NULL flags pointer. */
void test_adiol100_get_status_null_flags(void)
{
	struct adiol100_dev *dev = create_test_dev();
	int ret = adiol100_get_status(dev, ADIOL100_CH_A, NULL);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

/** @brief Test that adiol100_get_status() reads STATUS_A register value. */
void test_adiol100_get_status_ch_a(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	REG(ADIOL100_REG_STATUS_A) = ADIOL100_COMLOST | ADIOL100_CQLEVEL;

	uint16_t flags;
	int ret = adiol100_get_status(dev, ADIOL100_CH_A, &flags);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_EQUAL_HEX16(ADIOL100_COMLOST | ADIOL100_CQLEVEL, flags);

	free(dev);
}

/** @brief Test that adiol100_get_status() routes to STATUS_B for channel B. */
void test_adiol100_get_status_ch_b(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	REG(ADIOL100_REG_STATUS_B) = ADIOL100_COMLOST;

	uint16_t flags;
	int ret = adiol100_get_status(dev, ADIOL100_CH_B, &flags);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_EQUAL_HEX16(ADIOL100_COMLOST, flags);

	free(dev);
}

/** @brief Test that adiol100_get_fifo_status() returns -ENODEV for NULL dev pointer. */
void test_adiol100_get_fifo_status_null_dev(void)
{
	uint16_t flags;
	int ret = adiol100_get_fifo_status(NULL, ADIOL100_CH_A, &flags);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/** @brief Test that adiol100_get_fifo_status() returns -EINVAL for NULL flags pointer. */
void test_adiol100_get_fifo_status_null_flags(void)
{
	struct adiol100_dev *dev = create_test_dev();
	int ret = adiol100_get_fifo_status(dev, ADIOL100_CH_A, NULL);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

/** @brief Test that adiol100_get_fifo_status() reads RXFIFOSTAT_A register value. */
void test_adiol100_get_fifo_status_ch_a(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	REG(ADIOL100_REG_RXFIFOSTAT_A) = 0x0005;

	uint16_t flags;
	int ret = adiol100_get_fifo_status(dev, ADIOL100_CH_A, &flags);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_EQUAL_HEX16(0x0005, flags);

	free(dev);
}

/** @brief Test that adiol100_get_fifo_status() routes to RXFIFOSTAT_B for channel B. */
void test_adiol100_get_fifo_status_ch_b(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	REG(ADIOL100_REG_RXFIFOSTAT_B) = 0x0003;

	uint16_t flags;
	int ret = adiol100_get_fifo_status(dev, ADIOL100_CH_B, &flags);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_EQUAL_HEX16(0x0003, flags);

	free(dev);
}

/* ---- Misc --------------------------------------------------------------- */

/** @brief Test that adiol100_wakeup_request() returns -ENODEV for NULL dev pointer. */
void test_adiol100_wakeup_request_null_dev(void)
{
	int ret = adiol100_wakeup_request(NULL, ADIOL100_CH_A);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/** @brief Test that adiol100_wakeup_request() sets WUPULS in FRAMCTRL1_A. */
void test_adiol100_wakeup_request_ch_a(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	int ret = adiol100_wakeup_request(dev, ADIOL100_CH_A);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_BITS(ADIOL100_WUPULS, ADIOL100_WUPULS,
			 REG(ADIOL100_REG_FRAMCTRL1_A));

	free(dev);
}

/** @brief Test that adiol100_wakeup_request() routes to FRAMCTRL1_B for channel B. */
void test_adiol100_wakeup_request_ch_b(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	int ret = adiol100_wakeup_request(dev, ADIOL100_CH_B);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_BITS(ADIOL100_WUPULS, ADIOL100_WUPULS,
			 REG(ADIOL100_REG_FRAMCTRL1_B));

	free(dev);
}

/** @brief Test that adiol100_wakeup_request() leaves bits outside WUPULS untouched. */
void test_adiol100_wakeup_request_preserves_other_bits(void)
{
	struct adiol100_dev *dev = create_test_dev();
	int ret;

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	PRESET_OUTSIDE(ADIOL100_REG_FRAMCTRL1_A, ADIOL100_WUPULS, 1);
	ret = adiol100_wakeup_request(dev, ADIOL100_CH_A);
	TEST_ASSERT_EQUAL_INT(0, ret);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_FRAMCTRL1_A, ADIOL100_WUPULS, 1);

	PRESET_OUTSIDE(ADIOL100_REG_FRAMCTRL1_A, ADIOL100_WUPULS, 0);
	ret = adiol100_wakeup_request(dev, ADIOL100_CH_A);
	TEST_ASSERT_EQUAL_INT(0, ret);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_FRAMCTRL1_A, ADIOL100_WUPULS, 0);

	free(dev);
}

/** @brief Test that adiol100_clear_estcom_fails() returns -ENODEV for NULL dev. */
void test_adiol100_clear_estcom_fails_null_dev(void)
{
	int ret = adiol100_clear_estcom_fails(NULL, ADIOL100_CH_A);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/** @brief Test that adiol100_clear_estcom_fails() sets ESTCOMFACLR in FRAMCTRL1_A. */
void test_adiol100_clear_estcom_fails_ch_a(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	int ret = adiol100_clear_estcom_fails(dev, ADIOL100_CH_A);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_BITS(ADIOL100_ESTCOMFACLR, ADIOL100_ESTCOMFACLR,
			 REG(ADIOL100_REG_FRAMCTRL1_A));

	free(dev);
}

/** @brief Test that adiol100_clear_estcom_fails() routes to FRAMCTRL1_B for channel B. */
void test_adiol100_clear_estcom_fails_ch_b(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	int ret = adiol100_clear_estcom_fails(dev, ADIOL100_CH_B);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_BITS(ADIOL100_ESTCOMFACLR, ADIOL100_ESTCOMFACLR,
			 REG(ADIOL100_REG_FRAMCTRL1_B));

	free(dev);
}

/** @brief Test that adiol100_clear_estcom_fails() leaves bits outside ESTCOMFACLR untouched. */
void test_adiol100_clear_estcom_fails_preserves_other_bits(void)
{
	struct adiol100_dev *dev = create_test_dev();
	int ret;

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	PRESET_OUTSIDE(ADIOL100_REG_FRAMCTRL1_A, ADIOL100_ESTCOMFACLR, 1);
	ret = adiol100_clear_estcom_fails(dev, ADIOL100_CH_A);
	TEST_ASSERT_EQUAL_INT(0, ret);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_FRAMCTRL1_A,
				 ADIOL100_ESTCOMFACLR, 1);

	PRESET_OUTSIDE(ADIOL100_REG_FRAMCTRL1_A, ADIOL100_ESTCOMFACLR, 0);
	ret = adiol100_clear_estcom_fails(dev, ADIOL100_CH_A);
	TEST_ASSERT_EQUAL_INT(0, ret);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_FRAMCTRL1_A,
				 ADIOL100_ESTCOMFACLR, 0);

	free(dev);
}

/** @brief Test that adiol100_clear_watchdog() returns -ENODEV for NULL dev pointer. */
void test_adiol100_clear_watchdog_null_dev(void)
{
	int ret = adiol100_clear_watchdog(NULL);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/** @brief Test that adiol100_clear_watchdog() sets WDGCLEAR in WATCHDOG register. */
void test_adiol100_clear_watchdog(void)
{
	struct adiol100_dev *dev = create_test_dev();

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	int ret = adiol100_clear_watchdog(dev);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_BITS(ADIOL100_WDGCLEAR, ADIOL100_WDGCLEAR,
			 REG(ADIOL100_REG_WATCHDOG));

	free(dev);
}

/** @brief Test that adiol100_clear_watchdog() leaves bits outside WDGCLEAR untouched. */
void test_adiol100_clear_watchdog_preserves_other_bits(void)
{
	struct adiol100_dev *dev = create_test_dev();
	int ret;

	install_util_stubs();
	no_os_spi_write_and_read_Stub(stub_spi);

	PRESET_OUTSIDE(ADIOL100_REG_WATCHDOG, ADIOL100_WDGCLEAR, 1);
	ret = adiol100_clear_watchdog(dev);
	TEST_ASSERT_EQUAL_INT(0, ret);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_WATCHDOG, ADIOL100_WDGCLEAR, 1);

	PRESET_OUTSIDE(ADIOL100_REG_WATCHDOG, ADIOL100_WDGCLEAR, 0);
	ret = adiol100_clear_watchdog(dev);
	TEST_ASSERT_EQUAL_INT(0, ret);
	ASSERT_OUTSIDE_UNCHANGED(ADIOL100_REG_WATCHDOG, ADIOL100_WDGCLEAR, 0);

	free(dev);
}
