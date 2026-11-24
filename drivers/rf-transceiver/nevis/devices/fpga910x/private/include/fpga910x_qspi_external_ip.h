/**
 * \file
 * \brief Standard (single bit) SPI master on the external Xilinx AXI Quad SPI core.
 *
 * FPGA910X API Version: $ADI_FPGA910X_API_VERSION$
 */

/**
* Copyright 2026 Analog Devices Inc.
* Released under the FPGA910X API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#ifndef _FPGA910X_QSPI_EXTERNAL_IP_H_
#define _FPGA910X_QSPI_EXTERNAL_IP_H_

#include <stdint.h>
#include "axi_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/*  The qspi pads can be driven two ways, and these functions are one of them: they hand the pads to
 *  the external AXI Quad SPI core by setting QSPI_IPSEL, then drive that core through the vendored
 *  AMD/Xilinx XSpi driver in third_party/xilinx_qspi. The alternative is the ps2p9102 internal qspi
 *  engine, reached through axi_ps2p9102_qspi_tx_data / _rx_data in the FPGA driver library, which
 *  nothing here affects. Only one owns the pads at a time, selected by QSPI_IPSEL at runtime.
 *
 *  This is not conditional on a build macro, and should not become one. third_party/xilinx_qspi is
 *  committed to the repository, so a clone always has it - no fetch, no network, no setup step, nothing
 *  to guard against. A compile time switch would also not work: the platform layer in
 *  adi_fpga910x_hal_linux_uio.c calls these functions unconditionally and nothing excludes that file
 *  from any build, so disabling them gives implicit declarations - warnings only, under gnu90 - and then
 *  undefined references at link.
 */

// **********************************************************************************
// **********************************************************************************

/**
 * \brief Build parameters of the external AXI Quad SPI core.
 *
 * These are hardware generics fixed when the FPGA image is built, not runtime settings. They must
 * match the Vivado block configuration. Override at compile time if the image changes.
 *
 * \note FPGA910X_QSPI_FIFO_DEPTH is c_fifo_depth of the AXI Quad SPI core, 256 in the current
 *  ZCU102 image. Setting it larger than the real depth hangs the transfer: the driver refills the
 *  transmit fifo this many words at a time without checking TX_FULL, so the surplus writes are dropped,
 *  fewer words come back than the byte counter expects, and the transfer loop spins on an empty receive
 *  fifo. The caller's receive buffer is not at risk - the drain only stores what actually arrived.
 *  Setting it smaller is safe but slower, since the driver then tops the transmit fifo up in smaller
 *  chunks during a frame.
 *
 *  Nothing verifies this against the image. Unlike lsb_first, which the open function reads back, the
 *  fifo depth is a Vivado generic with no readable register, so a mismatch is silent.
 *
 * \note FPGA910X_QSPI_DATA_WIDTH is c_num_transfer_bits, 8 in the current ZCU102 image. It lives here
 *  rather than in fpga910x_qspi_external_ip_cfg_t because it is a build generic, not a runtime choice:
 *  there is no register to write and none to read back, so a caller who could set it per open would
 *  only be able to disagree with the hardware, silently. pg153 Table 6 bears this out - the SPI control
 *  register has no transfer width field.
 *
 *  Two consequences if this is ever overridden above 8, both handled but worth knowing. The caller's
 *  buffers must be aligned to FPGA910X_QSPI_DATA_WIDTH/8 bytes, which fpga910x_qspi_transfer enforces
 *  and rejects with FPGA910X_QSPI_ERR_ALIGNMENT. And the bytes within each word reach the wire in
 *  reverse order on this little endian target, because the vendored driver reads whole words out of a
 *  byte buffer - that one is not enforced, and internal loopback cannot detect it since the reversal
 *  cancels between transmit and receive. Only 8 is exercised by the tests.
 */

#ifndef FPGA910X_QSPI_HAS_FIFOS
#define FPGA910X_QSPI_HAS_FIFOS 1
#endif

#ifndef FPGA910X_QSPI_FIFO_DEPTH
#define FPGA910X_QSPI_FIFO_DEPTH 256
#endif

#ifndef FPGA910X_QSPI_NUM_SS_BITS
#define FPGA910X_QSPI_NUM_SS_BITS 1
#endif

#ifndef FPGA910X_QSPI_DATA_WIDTH
#define FPGA910X_QSPI_DATA_WIDTH 8
#endif

/**
 * \brief Byte clocked out of mosi when the caller supplies no transmit data.
 *
 * spi is full duplex, so something has to be clocked out to clock anything in. 0xaa alternates every
 * bit, which keeps mosi toggling and makes the dummy traffic easy to recognise on a scope, unlike a
 * static level. Override at compile time if a slave on the bus would misread it - a device that
 * decodes mosi while it is driving miso could take a dummy byte for a command, where 0x00 is inert.
 */

#ifndef FPGA910X_QSPI_DUMMY_TX_BYTE
#define FPGA910X_QSPI_DUMMY_TX_BYTE 0xAA
#endif

// **********************************************************************************
// **********************************************************************************

/**
 * \brief Error codes. Bus errors from the axi bus master are passed through unchanged.
 *
 * \note The numbering has three gaps, all retired codes. -111 was ERR_DATA_WIDTH, dropped when the
 *  transfer width stopped being a runtime parameter and became the FPGA910X_QSPI_DATA_WIDTH build
 *  constant - there is no longer a caller supplied value to validate. -115 was ERR_NO_MEMORY, dropped with the
 *  dummy transmit buffer: this driver allocates nothing, so there is no allocation left to fail.
 *  -118 was ERR_SELFTEST, dropped with the self test entry point. The codes keep their original
 *  values rather than closing the gaps, so existing callers and test expectations do not shift.
 */

#define FPGA910X_QSPI_ERR_NULL_PARAM   -110
#define FPGA910X_QSPI_ERR_SLAVE_SELECT -112
#define FPGA910X_QSPI_ERR_COUNT        -113
#define FPGA910X_QSPI_ERR_NOT_OPEN     -114
#define FPGA910X_QSPI_ERR_INIT         -116
#define FPGA910X_QSPI_ERR_TRANSFER     -117
#define FPGA910X_QSPI_ERR_LSB_FIRST    -119
#define FPGA910X_QSPI_ERR_ALIGNMENT    -120

// **********************************************************************************
// **********************************************************************************

/**
 * \brief qspi parameters for standard (single bit) spi mode, applied by the open function.
 *
 * These are the SPI control register fields a caller can meaningfully choose, per pg153 Table 6
 * (SPICR, core base + 0x60), plus the slave select register:
 *
 *   loopback            SPICR bit 0, LOOP
 *   clk_active_low      SPICR bit 3, CPOL
 *   clk_phase_1         SPICR bit 4, CPHA
 *   manual_slave_select SPICR bit 7, Manual Slave Select Assertion Enable
 *   lsb_first           SPICR bit 9, LSB First
 *   slave_select        SPISSR, core base + 0x70 - not a control register field
 *
 * The remaining SPICR bits are deliberately absent, because they are not per-open choices:
 *
 *   bit 1  SPE                        the enable, owned by XSpi_Start and XSpi_Stop
 *   bit 2  Master                     always master here - see the note below
 *   bits 5, 6  TX / RX FIFO Reset     one shot, self clearing, used during reset
 *   bit 8  Master Transaction Inhibit driven by XSpi_Transfer around each frame
 *
 * The transfer width is not here either. It is a build generic rather than a register field, so it
 * lives with the other build parameters as FPGA910X_QSPI_DATA_WIDTH.
 *
 * \note Master, SPICR bit 2, is fixed rather than exposed. Slave mode is not simply the other value of
 *  this bit: the far end would drive sclk and slave select, so the whole transfer path here - which
 *  initiates a frame and then polls for completion - does not apply. pg153 adds that loopback is
 *  functional in master mode only, manual slave select has no effect on slave operation, and standard
 *  slave mode is unsupported at an sck ratio of 2. Exposing the bit without that work would let a caller
 *  select slave and get a driver still behaving as a master.
 */

typedef struct fpga910x_qspi_external_ip_cfg
{
    uint8_t slave_select;        /*!< zero based index of the slave select line to assert. SPISSR, not
                                  *  a SPICR field. Must be below FPGA910X_QSPI_NUM_SS_BITS, which is 1
                                  *  in the current image - so 0 is the only legal value today */
    uint8_t clk_active_low;      /*!< SPICR bit 3, cpol. sclk idles low (0), or high (1) */
    uint8_t clk_phase_1;         /*!< SPICR bit 4, cpha. Data valid on the first (0), or second (1) edge */
    uint8_t manual_slave_select; /*!< SPICR bit 7. Slave select held for the whole transfer (1), or
                                  *  toggled between words (0). Standard spi mode only, per pg153 */
    uint8_t loopback;            /*!< SPICR bit 0. Internal loopback enabled (1) or disabled (0). With it set the
                                  *  core ties its transmitter to its own receiver, so the datapath
                                  *  can be exercised with no external device and no data leaves the
                                  *  fpga. The pads are not involved, so this proves the driver and
                                  *  the core only, never the wiring or a slave */
    uint8_t lsb_first;           /*!< SPICR bit 9. Bit order within a byte on the wire. msb first (0) is the spi
                                  *  default and what every version of the core supports. lsb first
                                  *  (1) must match the far end exactly, or every byte arrives bit
                                  *  reversed. Two limits: it is only permitted in standard (single
                                  *  bit) spi mode, which is the only mode the open function
                                  *  configures - dual and quad I/O are msb first only - and it is
                                  *  only available in some builds of the AXI Quad SPI core, so the
                                  *  open function reads the bit back and fails if this build
                                  *  ignored it */
} fpga910x_qspi_external_ip_cfg_t;

// **********************************************************************************
// **********************************************************************************

/*  The register accessors the vendored XSpi driver calls - Xil_In32 and Xil_Out32, renamed to
 *  adi_fpga910x_xil_* by xil_bsp/xil_io.h so nothing named Xil_* is exported from this library - are
 *  deliberately not declared here. They are the bridge to that driver, not part of this driver's
 *  interface: the only callers are the vendored sources, which get the declarations from xil_io.h.
 *  Declaring them here as well would mean keeping two spellings of the same symbol in step, and this
 *  header is included before xil_io.h in the implementation, so the rename would not have applied yet.
 *  See the definitions at the bottom of fpga910x_qspi_external_ip.c.
 */

/**
 * \brief open the qspi bus in standard (single bit) spi mode
 *
 * Enables the qspi io, points QSPI_IPSEL at the external AXI Quad SPI core, then configures and
 * starts that core as a master in polled mode. Only one instance may be open at a time.
 *
 * \note this leaves the ps2p9102 internal qspi engine enable (QSPI_ENABLE) untouched. Only the io
 *  enable and the ip select are written, so the sd3 and ss1 gpio muxing established by
 *  axi_ps2p9102_iosel is preserved.
 *
 * \note cfg->lsb_first is applied by writing SPICR bit 9 directly, because the vendored XSpi driver
 *  has no option for it. With lsb first requested the bit is read back and a mismatch is an error:
 *  lsb first is not available in every build of the AXI Quad SPI core, and a build that ignores it
 *  would otherwise bit reverse every byte at the far end with no indication here.
 *
 * \note lsb first is also mode restricted: the core permits it in standard (single bit) spi mode only,
 *  dual and quad I/O being msb first. This function always sets XSP_STANDARD_MODE, so cfg->lsb_first
 *  is unconditionally legal as things stand. Anyone adding dual or quad mode here must reject
 *  lsb_first in those modes instead of passing it to SPICR, where the core would ignore it and the
 *  read back below would then fail for the wrong reason - reporting an unsupported core build when the
 *  real fault is an illegal mode combination.
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of the ps2p9102 instance, used for the pin mux
 * \param[in] qspi_peripheral_id of the external AXI Quad SPI core instance
 * \param[in] cfg parameters
 * \returns non zero value to indicate bus errors from axi bus master, or a configuration failure
 */

int32_t fpga910x_qspi_external_ip_open(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t qspi_peripheral_id, fpga910x_qspi_external_ip_cfg_t *cfg);

/**
 * \brief write bytes, discarding anything clocked in
 *
 * With manual_slave_select set at open, slave select is asserted for the whole call and de-asserted
 * on return, so a transaction that must hold slave select across a command and its data has to be a
 * single call - use fpga910x_qspi_external_ip_read for that. With it clear the core de-asserts slave
 * select between words (SPICR bit 7, pg153), framing the call one word at a time, and no single call
 * can hold slave select across a command and its data.
 *
 * \note with FPGA910X_QSPI_DATA_WIDTH above 8, txdata must be aligned to that width in bytes, and the
 *  bytes within each word reach the wire in reverse order. See the build parameters at the top of this
 *  header - an unaligned buffer is rejected, the byte order is not.
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] qspi_peripheral_id of the external AXI Quad SPI core instance
 * \param[in] txdata bytes to send, must not be NULL - there would be nothing to send. To clock out
 *  the dummy pattern instead, use fpga910x_qspi_external_ip_read with a NULL txdata
 * \param[in] count number of bytes to send
 * \returns non zero value to indicate bus errors from axi bus master, a misaligned buffer, or a
 *  transfer failure
 */

int32_t fpga910x_qspi_external_ip_write(AXI_DEVICE_T *a_device, uint32_t qspi_peripheral_id,
    const uint8_t txdata[], uint32_t count);

/**
 * \brief full duplex transfer, capturing what is clocked in
 *
 * spi is full duplex, so reading count bytes means clocking count bytes out. Pass the bytes to send
 * in txdata and the captured bytes come back in rxdata. Pass NULL for txdata to clock out a dummy
 * pattern instead, which covers a plain receive. That pattern is FPGA910X_QSPI_DUMMY_TX_BYTE, 0xaa
 * by default, compile time overridable.
 *
 * With manual_slave_select set at open, slave select is asserted for the whole call and de-asserted
 * on return, so a command and the data it returns must be requested together in one call rather than
 * as a write followed by a read. With it clear the core de-asserts slave select between words (SPICR
 * bit 7, pg153), framing the call one word at a time, so a command and its response cannot be issued
 * as one frame at all - pass manual_slave_select unless the far end wants per-word framing.
 *
 * \note this driver allocates no memory. With a NULL txdata the dummy bytes are clocked out of
 *  rxdata itself, which is filled with them first, so rxdata must be writable for the whole of count
 *  and must not alias anything the caller still needs.
 *
 * \note with FPGA910X_QSPI_DATA_WIDTH above 8, both buffers must be aligned to that width in bytes, and
 *  the bytes within each word reach the wire in reverse order. See the build parameters at the top of
 *  this header - an unaligned buffer is rejected, the byte order is not.
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] qspi_peripheral_id of the external AXI Quad SPI core instance
 * \param[in] txdata bytes to send, or NULL to send FPGA910X_QSPI_DUMMY_TX_BYTE
 * \param[out] rxdata bytes received, must have space for count bytes and must not be NULL. The
 *  vendored XSpi_Transfer would accept a NULL receive pointer, and this driver's write path relies
 *  on that, but a read with nowhere to store the data would discard everything it captured and
 *  still return success - so it is refused here. To transmit and discard, use
 *  fpga910x_qspi_external_ip_write
 * \param[in] count number of bytes to transfer
 * \returns non zero value to indicate bus errors from axi bus master, a misaligned buffer, or a
 *  transfer failure
 */

int32_t fpga910x_qspi_external_ip_read(AXI_DEVICE_T *a_device, uint32_t qspi_peripheral_id,
    const uint8_t txdata[], uint8_t rxdata[], uint32_t count);

/**
 * \brief stop and reset the external AXI Quad SPI core, releasing the instance and the qspi pads
 *
 * Undoes what the open function did, in both halves: the core is stopped and reset, and the qspi pads
 * are handed back to whatever owned them before the open function took them over - which is usually
 * the ps2p9102 internal qspi engine. So an open / close pair leaves the pad routing as it found it,
 * and the internal engine is usable again afterwards without the caller touching QSPI_IPSEL itself.
 *
 * \note this takes the ps2p9102 peripheral_id as well as the core's, because the pad routing lives in
 *  the ps2p9102 io control register rather than in the AXI Quad SPI core. Pass the same two ids the
 *  open function was given.
 *
 * \note calling this without a successful open is harmless and does nothing, so a failed open needs no
 *  close - it releases the pads itself.
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of the ps2p9102 instance, used for the pin mux
 * \param[in] qspi_peripheral_id of the external AXI Quad SPI core instance
 * \returns non zero value to indicate bus errors from axi bus master, or a failure to restore the pads
 */

int32_t fpga910x_qspi_external_ip_close(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t qspi_peripheral_id);

#ifdef __cplusplus
}
#endif

#endif /* _FPGA910X_QSPI_EXTERNAL_IP_H_ */
