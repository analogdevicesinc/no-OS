/**
 * \file
 * \brief Standard (single bit) SPI master on the external Xilinx AXI Quad SPI core.
 *
 * Drives the qspi pads through the external AXI Quad SPI core rather than the ps2p9102 internal
 * qspi engine, by setting QSPI_IPSEL and then delegating the byte traffic to the vendored AMD/Xilinx
 * XSpi driver in third_party/xilinx_qspi. The two engines are alternatives - only one owns the pads
 * at a time, selected by QSPI_IPSEL.
 *
 * The internal engine stays in the FPGA driver library (axi_ps2p9102_qspi.c) and is untouched by
 * this file.
 *
 * FPGA910X API Version: $ADI_FPGA910X_API_VERSION$
 */

/**
* Copyright 2026 Analog Devices Inc.
* Released under the FPGA910X API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#include <stddef.h>
#include <string.h>

#include "fpga910x_qspi_external_ip.h"
#include "axi_ps2p9102_top.h"
#include "xspi.h"

// **********************************************************************************
// **********************************************************************************
// QSPI_IPSEL value that hands the qspi pins to the external AXI Quad SPI core, rather than to the
// ps2p9102 internal qspi engine.

#define FPGA910X_QSPI_IPSEL_EXTERNAL 0x1

// The dummy transmit byte, the core build generics and the error codes live in the header, since
// callers need them. See fpga910x_qspi_external_ip.h.

// Value Xil_In32 reports for every register read once a bus error has been latched.
//
// The vendored driver polls status registers in loops that have no iteration bound of their own -
// they rely on the hardware eventually setting a bit. Returning 0 on a failed read would therefore
// hang the calling thread rather than surface the error, so this value is chosen to make every one of
// those loops fall through instead:
//
//   TX_FULL  (status register bit 3) set   - ends the transmit fill loops, which otherwise write the
//                                           data register until the fifo reports full
//   TX_EMPTY (interrupt status bit 2) set  - ends the transfer complete waits
//   RX_EMPTY (status register bit 0) clear - lets the receive drain loop run, which is what decrements
//                                           the driver's byte counter and ends the outer transfer loop
//
// That third choice needs care, and a constant alone is not enough. The drain loop has no byte count
// bound of its own: it stores through RecvBufferPtr and decrements the driver's counter for as long as
// the status register says data is waiting. A constant with RX_EMPTY clear therefore never lets it stop,
// so it would write past the caller's receive buffer without limit. Setting RX_EMPTY instead stops the
// drain but also stops the only thing that decrements the byte counter, hanging the outer transfer loop.
//
// So the drain is bounded by iteration count instead, in fpga910x_qspi_fallthrough below: the status
// register reports RX_EMPTY clear for exactly as many polls as the caller's buffer has room for, then
// reports it set. Every other register reports this value unchanged.
//
// The data the drain stores is meaningless, but every entry point checks the latched status and returns
// an error, so the caller discards it.

#define FPGA910X_QSPI_BUS_ERROR_VALUE 0x0000000C

// **********************************************************************************
// **********************************************************************************
//
// State for the vendored AMD/Xilinx XSpi driver in third_party/xilinx_qspi.
//
// That driver reaches registers only through Xil_In32 and Xil_Out32, which are defined at the bottom
// of this file on top of the axi bus master, so that qspi register traffic goes through the FPGA910X
// HAL like every other axi_* driver. Those two functions take no device argument, so the device is
// held here, bound by fpga910x_qspi_external_ip_open. Only one instance can be open at a time, which matches
// the single external AXI Quad SPI core in the image.
//
// qspi_a_device and qspi_bus_status are shared with Xil_In32 / Xil_Out32 at the bottom of this file.

static AXI_DEVICE_T *qspi_a_device = NULL;
static int32_t qspi_bus_status = 0;
static uint8_t qspi_is_open = 0;
static XSpi qspi_instance;

// **********************************************************************************
// **********************************************************************************
//
// The qspi pad routing as it was before the open function took it over.
//
// The internal ps2p9102 qspi engine may own the pads when open is called, so releasing them means
// putting back what was there rather than writing a fixed default, which would be a guess. Recorded
// before the first write, used by the failure path of the open function and by the close function.
//
// Only meaningful while qspi_pads_saved is non zero.

static uint8_t  qspi_pads_saved = 0;
static uint32_t qspi_saved_io_enable = 0;
static uint32_t qspi_saved_ipsel = 0;

// **********************************************************************************
// **********************************************************************************
//
// Bound for the receive drain on the bus error fall through path, armed per transfer by
// fpga910x_qspi_transfer and read by fpga910x_qspi_fallthrough. See FPGA910X_QSPI_BUS_ERROR_VALUE above
// for why a bound is needed at all.
//
// qspi_drain_bytes_per_word is zero outside a transfer, which is also what disables the bound.

static uint32_t qspi_drain_limit_words = 0;
static uint32_t qspi_drain_stats_base  = 0;
static uint32_t qspi_drain_bytes_per_word = 0;

// **********************************************************************************
// **********************************************************************************

/*  defined below the open function, which is its only caller */

static int32_t fpga910x_qspi_configure(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t qspi_peripheral_id, fpga910x_qspi_external_ip_cfg_t *cfg);

/*  latched bus error check, Xil_In32 cannot report a failure through its return value */

static int32_t fpga910x_qspi_bus_status(void) {

    int32_t status;

    status = qspi_bus_status;
    qspi_bus_status = 0;
    return(status);
}

/*  hand the qspi pads back to whatever owned them before the open function took over */

static int32_t fpga910x_qspi_restore_pads(AXI_DEVICE_T *a_device, uint32_t peripheral_id) {

    int32_t status;

    if (qspi_pads_saved == 0) return(0);

    /*  drop the io enable first so the pads are quiet, put the ip select back, then restore the io
     *  enable to what it was. In this order the pads are never enabled with the wrong owner selected.
     *  Field writes throughout, so the sd3 and ss1 gpio muxing from axi_ps2p9102_iosel is preserved.
     */

    status = axi_ps2p9102_top_qspi_io_enable_set(a_device, peripheral_id, 0x0);
    if (status == 0) status = axi_ps2p9102_top_qspi_ipsel_set(a_device, peripheral_id,
        qspi_saved_ipsel);
    if (status == 0) status = axi_ps2p9102_top_qspi_io_enable_set(a_device, peripheral_id,
        qspi_saved_io_enable);

    qspi_pads_saved = 0;
    return(status);
}

/*  release everything the open function acquired, then pass the caller's error through unchanged.
 *
 *  Rollback failures are logged and dropped: the error that brought us here is what the caller needs
 *  to see, and there is nothing useful to do about a bus that has already stopped answering.
 */

static int32_t fpga910x_qspi_open_failed(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    int32_t status) {

    int32_t rollback;

    /*  IsReady is what XSpi_CfgInitialize sets and what XSpi_Reset asserts on, so it is exactly the
     *  test for "we got far enough to initialise the core". Do this before unbinding qspi_a_device,
     *  because XSpi_Reset reaches registers through Xil_Out32.
     */

    if (qspi_instance.IsReady == XIL_COMPONENT_IS_READY) {
        (void) XSpi_Stop(&qspi_instance);
        XSpi_Reset(&qspi_instance);
        memset(&qspi_instance, 0, sizeof(qspi_instance));
    }

    rollback = fpga910x_qspi_restore_pads(a_device, peripheral_id);
    if (rollback != 0) {
        AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_WARNING, "%s: failed to restore the qspi pad "
            "routing after a failed open (%d)! The pads may be left with the external core, so the "
            "ps2p9102 internal qspi engine will not work until an open succeeds.\n",
            __func__, rollback);
    }

    qspi_a_device = NULL;
    qspi_bus_status = 0;
    qspi_is_open = 0;

    return(status);
}

/*  common transfer path for write and read. Slave select is asserted for the whole transfer when the
 *  open call set manual_slave_select; without it the core de-asserts between words and this function
 *  cannot hold a frame together. Nothing here re-checks the bit - it is fixed at open. */

static int32_t fpga910x_qspi_transfer(AXI_DEVICE_T *a_device, uint32_t qspi_peripheral_id,
    const uint8_t txdata[], uint8_t rxdata[], uint32_t count) {

    int xstatus;
    int32_t status;
    uint8_t *sendbuf;
    uint32_t bytes_per_word;

    if (qspi_is_open == 0) {
        AXI_LOG_PRINTF(a_device, qspi_peripheral_id, AXI_LOG_ERROR, "%s: "
            "qspi is not open!\n", __func__);
        return(FPGA910X_QSPI_ERR_NOT_OPEN);
    }

    if (count == 0) {
        AXI_LOG_PRINTF(a_device, qspi_peripheral_id, AXI_LOG_ERROR, "%s: "
            "transfer count can not be 0!\n", __func__);
        return(FPGA910X_QSPI_ERR_COUNT);
    }

    /*  the core moves one word at a time, so a transfer has to be a whole number of words */

    bytes_per_word = qspi_instance.DataWidth / 8;
    if ((count % bytes_per_word) != 0) {
        AXI_LOG_PRINTF(a_device, qspi_peripheral_id, AXI_LOG_ERROR, "%s: transfer count (%d) is not "
            "a multiple of the %d bit data width!\n", __func__, count, qspi_instance.DataWidth);
        return(FPGA910X_QSPI_ERR_COUNT);
    }

    /*  Above an 8 bit width the vendored driver reads and writes the caller's buffers a whole word at
     *  a time, as *(u16 *) or *(u32 *) on what is declared uint8_t *. Dereferencing an unaligned
     *  pointer that way is undefined behaviour, and on armv7 it is the worst kind: a plain LDR or STR
     *  tolerates a misaligned address, so it appears to work until the compiler vectorises the copy
     *  and picks an instruction that faults - which can differ between the Debug and Release builds of
     *  the same source. Refuse it here instead, where the caller gets a diagnostic.
     *
     *  Nothing to check at 8 bits, where every address is aligned.
     */

    if (bytes_per_word > 1) {
        if ((txdata != NULL) && ((((uintptr_t) txdata) % bytes_per_word) != 0)) {
            AXI_LOG_PRINTF(a_device, qspi_peripheral_id, AXI_LOG_ERROR, "%s: transmit buffer is not "
                "aligned to %d bytes, as the %d bit data width requires!\n", __func__,
                bytes_per_word, qspi_instance.DataWidth);
            return(FPGA910X_QSPI_ERR_ALIGNMENT);
        }
        if ((rxdata != NULL) && ((((uintptr_t) rxdata) % bytes_per_word) != 0)) {
            AXI_LOG_PRINTF(a_device, qspi_peripheral_id, AXI_LOG_ERROR, "%s: receive buffer is not "
                "aligned to %d bytes, as the %d bit data width requires!\n", __func__,
                bytes_per_word, qspi_instance.DataWidth);
            return(FPGA910X_QSPI_ERR_ALIGNMENT);
        }
    }

    /*  spi is full duplex, so clocking bytes in means clocking bytes out. Slave select de-asserts
     *  when XSpi_Transfer returns, so this has to be one call - FPGA910X_QSPI_DUMMY_TX_BYTE is
     *  supplied for a caller that only wants to receive, rather than chunking the transfer.
     *
     *  Nothing is allocated for those dummy bytes. The receive buffer is filled with them and handed
     *  to XSpi_Transfer as both the source and the destination, which is the "caller wishes to only
     *  receive" form listed in that function's own header comment. It is safe because the driver
     *  keeps two independent cursors into the buffer and the transmit one always leads: it fills the
     *  fifo from SendBufferPtr before the poll loop drains the matching received bytes back through
     *  RecvBufferPtr, and it refills only after that drain. So every dummy byte is clocked out before
     *  the byte it received overwrites it.
     */

    if (txdata == NULL) {
        if (rxdata == NULL) {
            AXI_LOG_PRINTF(a_device, qspi_peripheral_id, AXI_LOG_ERROR, "%s: a transfer with no "
                "transmit buffer needs a receive buffer to clock the dummy bytes out of, because this "
                "driver allocates nothing and fills rxdata with the pattern to use as the source! "
                "Supply a receive buffer of count bytes, or call fpga910x_qspi_external_ip_write with "
                "real data.\n", __func__);
            return(FPGA910X_QSPI_ERR_NULL_PARAM);
        }
        memset(rxdata, FPGA910X_QSPI_DUMMY_TX_BYTE, count);
        sendbuf = rxdata;
    }
    else {
        /*  XSpi_Transfer only reads from the transmit buffer, so dropping const here is safe */
        sendbuf = (uint8_t *) txdata;
    }

    /*  start from a clean latch so this transfer only reports its own bus errors */

    qspi_bus_status = 0;

    /*  Arm the drain bound before handing control to the vendored driver. If a bus error latches part
     *  way through the transfer, this is what stops the receive drain loop writing past rxdata - see
     *  fpga910x_qspi_fallthrough below. Disarmed again on return, because register reads outside a
     *  transfer are not drain polls.
     */

    qspi_drain_bytes_per_word = bytes_per_word;
    qspi_drain_limit_words    = count / bytes_per_word;
    qspi_drain_stats_base     = qspi_instance.Stats.BytesTransferred;

    xstatus = XSpi_Transfer(&qspi_instance, sendbuf, rxdata, count);

    qspi_drain_bytes_per_word = 0;

    status = fpga910x_qspi_bus_status();
    if (status != 0) return(status);

    if (xstatus != XST_SUCCESS) {
        AXI_LOG_PRINTF(a_device, qspi_peripheral_id, AXI_LOG_ERROR, "%s: transfer of %d bytes "
            "failed (%d)!\n", __func__, count, xstatus);
        return(FPGA910X_QSPI_ERR_TRANSFER);
    }

    return(0);
}

// **********************************************************************************
// **********************************************************************************

int32_t fpga910x_qspi_external_ip_open(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t qspi_peripheral_id, fpga910x_qspi_external_ip_cfg_t *cfg) {

    int32_t status;

    if (cfg == NULL) {
        AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_ERROR, "%s: "
            "qspi parameters can not be NULL!\n", __func__);
        return(FPGA910X_QSPI_ERR_NULL_PARAM);
    }

    if (cfg->slave_select >= FPGA910X_QSPI_NUM_SS_BITS) {
        AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_ERROR, "%s: requested slave select (%d) is "
            "out of range, the core has %d slave select bits!\n", __func__,
            cfg->slave_select, FPGA910X_QSPI_NUM_SS_BITS);
        return(FPGA910X_QSPI_ERR_SLAVE_SELECT);
    }

    /*  re-opening is allowed, stop the core first so it is configured from a known state. That also
     *  hands the pads back, which the save below then reads.
     */

    if (qspi_is_open != 0) {
        status = fpga910x_qspi_external_ip_close(a_device, peripheral_id, qspi_peripheral_id);
        if (status != 0) return(status);
    }

    /*  Record the pad routing before touching it, so a failure below can put it back. Nothing has been
     *  acquired yet, so these two may still return bare.
     */

    status = axi_ps2p9102_top_qspi_io_enable_get(a_device, peripheral_id, &qspi_saved_io_enable);
    if (status != 0) return(status);
    status = axi_ps2p9102_top_qspi_ipsel_get(a_device, peripheral_id, &qspi_saved_ipsel);
    if (status != 0) return(status);
    qspi_pads_saved = 1;

    /*  Hand the qspi pins to the external AXI Quad SPI core. These are field writes rather than a
     *  write of the whole io control register, so that the sd3 and ss1 gpio muxing established by
     *  axi_ps2p9102_iosel is preserved.
     *
     *  From here on the driver owns hardware, so every failure exit goes through
     *  fpga910x_qspi_open_failed to release it. Returning bare would leave the pads with a core that
     *  was never configured, and the close function cannot recover that because it is gated on
     *  qspi_is_open, which a failed open never sets.
     */

    status = axi_ps2p9102_top_qspi_io_enable_set(a_device, peripheral_id, 0x1);
    if (status != 0) return(fpga910x_qspi_open_failed(a_device, peripheral_id, status));
    status = axi_ps2p9102_top_qspi_ipsel_set(a_device, peripheral_id,
        FPGA910X_QSPI_IPSEL_EXTERNAL);
    if (status != 0) return(fpga910x_qspi_open_failed(a_device, peripheral_id, status));

    status = fpga910x_qspi_configure(a_device, peripheral_id, qspi_peripheral_id, cfg);
    if (status != 0) return(fpga910x_qspi_open_failed(a_device, peripheral_id, status));

    qspi_is_open = 1;
    return(0);
}

// **********************************************************************************
// **********************************************************************************

/*  Everything the open function does after it owns the pads: bind the register accessors, then
 *  configure and start the core. Split out so there is exactly one rollback site rather than one per
 *  failure exit - the twelve returns below are unchanged, and the single caller above releases the
 *  hardware for all of them.
 */

static int32_t fpga910x_qspi_configure(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t qspi_peripheral_id, fpga910x_qspi_external_ip_cfg_t *cfg) {

    int xstatus;
    int32_t status;
    uint32_t options;
    u32 control;
    XSpi_Config config;

    /*  bind the register accessors used by the vendored driver, see Xil_In32 below */

    qspi_a_device = a_device;
    qspi_bus_status = 0;

    /*  the generated config table that upstream would supply lives in xspi_g.c, which needs a
     *  board generated xparameters.h and is not part of this tree, so describe the core here. The
     *  base address is an axi offset because Xil_In32 and Xil_Out32 go through the axi bus master.
     */

    memset(&config, 0, sizeof(config));
    config.DeviceId = 0;
    config.BaseAddress = (UINTPTR) qspi_peripheral_id;
    config.HasFifos = FPGA910X_QSPI_HAS_FIFOS;
    config.FifosDepth = FPGA910X_QSPI_FIFO_DEPTH;
    config.SlaveOnly = 0;
    config.NumSlaveBits = FPGA910X_QSPI_NUM_SS_BITS;
    config.DataWidth = FPGA910X_QSPI_DATA_WIDTH;
    config.SpiMode = XSP_STANDARD_MODE;
    config.AxiInterface = 0;
    config.AxiFullBaseAddress = 0;
    config.XipMode = 0;
    config.Use_Startup = 0;

    memset(&qspi_instance, 0, sizeof(qspi_instance));
    xstatus = XSpi_CfgInitialize(&qspi_instance, &config, config.BaseAddress);
    status = fpga910x_qspi_bus_status();
    if (status != 0) return(status);
    if (xstatus != XST_SUCCESS) {
        AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_ERROR, "%s: "
            "failed to initialize the qspi core (%d)!\n", __func__, xstatus);
        return(FPGA910X_QSPI_ERR_INIT);
    }

    options = XSP_MASTER_OPTION;
    if (cfg->manual_slave_select != 0) options = options | XSP_MANUAL_SSELECT_OPTION;
    if (cfg->clk_active_low != 0) options = options | XSP_CLK_ACTIVE_LOW_OPTION;
    if (cfg->clk_phase_1 != 0) options = options | XSP_CLK_PHASE_1_OPTION;
    if (cfg->loopback != 0) options = options | XSP_LOOPBACK_OPTION;

    xstatus = XSpi_SetOptions(&qspi_instance, options);
    status = fpga910x_qspi_bus_status();
    if (status != 0) return(status);
    if (xstatus != XST_SUCCESS) {
        AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_ERROR, "%s: "
            "failed to set qspi options 0x%x (%d)!\n", __func__, options, xstatus);
        return(FPGA910X_QSPI_ERR_INIT);
    }

    /*  Bit order, SPICR bit 9.
     *
     *  This is not reachable through XSpi_SetOptions: the vendored driver's OptionsTable carries only
     *  loopback, cpol, cpha, master and manual slave select, and there is no XSP_LSB_FIRST_OPTION at
     *  all, so it has to be a read modify write here. XSpi_SetOptions preserves it either way - it
     *  reads the register and only touches bits in its own table - but doing this after it keeps the
     *  order obvious. It has to be after XSpi_CfgInitialize, which resets SPICR to its default.
     *
     *  Then read it back, because xspi_l.h is explicit that lsb first "is not available in all
     *  versions of the Xilinx Spi Device". A build without it ignores the write, and the failure mode
     *  of not noticing is silent: every byte on the wire arrives bit reversed at the far end, which
     *  looks like corrupt data rather than a configuration error. msb first needs no read back, being
     *  the reset state and supported by every build.
     */

    control = XSpi_GetControlReg(&qspi_instance);
    if (cfg->lsb_first != 0) control = control | XSP_CR_LSB_MSB_FIRST_MASK;
    else control = control & ~((u32) XSP_CR_LSB_MSB_FIRST_MASK);
    XSpi_SetControlReg(&qspi_instance, control);

    status = fpga910x_qspi_bus_status();
    if (status != 0) return(status);

    if (cfg->lsb_first != 0) {
        control = XSpi_GetControlReg(&qspi_instance);
        status = fpga910x_qspi_bus_status();
        if (status != 0) return(status);

        if ((control & XSP_CR_LSB_MSB_FIRST_MASK) == 0) {
            AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_ERROR, "%s: lsb first was requested but "
                "SPICR bit 9 reads back clear (0x%x), so this build of the AXI Quad SPI core does not "
                "support it. Every byte would be bit reversed at the far end!\n", __func__, control);
            return(FPGA910X_QSPI_ERR_LSB_FIRST);
        }
    }

    /*  XSpi_CfgInitialize resets the core, which deselects every slave, so select after it */

    xstatus = XSpi_SetSlaveSelect(&qspi_instance, (1u << cfg->slave_select));
    status = fpga910x_qspi_bus_status();
    if (status != 0) return(status);
    if (xstatus != XST_SUCCESS) {
        AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_ERROR, "%s: "
            "failed to select qspi slave %d (%d)!\n", __func__, cfg->slave_select, xstatus);
        return(FPGA910X_QSPI_ERR_INIT);
    }

    xstatus = XSpi_Start(&qspi_instance);
    status = fpga910x_qspi_bus_status();
    if (status != 0) return(status);
    if (xstatus != XST_SUCCESS) {
        AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_ERROR, "%s: "
            "failed to start the qspi core (%d)!\n", __func__, xstatus);
        return(FPGA910X_QSPI_ERR_INIT);
    }

    /*  XSpi_Start enables the global interrupt, which the driver documents as the interrupt mode of
     *  operation. Disable it so XSpi_Transfer completes each transfer synchronously before it
     *  returns, instead of waiting for an interrupt handler that is never installed here.
     */

    XSpi_IntrGlobalDisable(&qspi_instance);
    status = fpga910x_qspi_bus_status();
    if (status != 0) return(status);

    return(0);
}

// **********************************************************************************
// **********************************************************************************

int32_t fpga910x_qspi_external_ip_write(AXI_DEVICE_T *a_device, uint32_t qspi_peripheral_id,
    const uint8_t txdata[], uint32_t count) {

    if (txdata == NULL) {
        AXI_LOG_PRINTF(a_device, qspi_peripheral_id, AXI_LOG_ERROR, "%s: transmit buffer can not be "
            "NULL - this function sends caller supplied bytes, so there is nothing to send! To clock "
            "out the 0x%02x dummy pattern instead, call fpga910x_qspi_external_ip_read with txdata NULL "
            "and a receive buffer.\n", __func__, FPGA910X_QSPI_DUMMY_TX_BYTE);
        return(FPGA910X_QSPI_ERR_NULL_PARAM);
    }

    return(fpga910x_qspi_transfer(a_device, qspi_peripheral_id, txdata, NULL, count));
}

int32_t fpga910x_qspi_external_ip_read(AXI_DEVICE_T *a_device, uint32_t qspi_peripheral_id,
    const uint8_t txdata[], uint8_t rxdata[], uint32_t count) {

    if (rxdata == NULL) {
        AXI_LOG_PRINTF(a_device, qspi_peripheral_id, AXI_LOG_ERROR, "%s: receive buffer can not be "
            "NULL - this function exists to capture what is clocked in, so there is nowhere to put "
            "it! To transmit and discard the received bytes instead, call "
            "fpga910x_qspi_external_ip_write. For a receive with no data to send, keep rxdata and pass "
            "txdata as NULL, which clocks out the 0x%02x dummy pattern.\n",
            __func__, FPGA910X_QSPI_DUMMY_TX_BYTE);
        return(FPGA910X_QSPI_ERR_NULL_PARAM);
    }

    return(fpga910x_qspi_transfer(a_device, qspi_peripheral_id, txdata, rxdata, count));
}

// **********************************************************************************
// **********************************************************************************

int32_t fpga910x_qspi_external_ip_close(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t qspi_peripheral_id) {

    int xstatus;
    int32_t status;
    int32_t padstatus;

    if (qspi_is_open == 0) return(0);

    xstatus = XSpi_Stop(&qspi_instance);
    if (xstatus != XST_SUCCESS) {
        AXI_LOG_PRINTF(a_device, qspi_peripheral_id, AXI_LOG_WARNING, "%s: "
            "failed to stop the qspi core (%d)!\n", __func__, xstatus);
    }

    XSpi_Reset(&qspi_instance);

    /*  clear the instance so "IsReady set" keeps meaning "this session initialised the core", which is
     *  what the open failure path tests to decide whether it has a core to reset.
     */

    memset(&qspi_instance, 0, sizeof(qspi_instance));

    /*  give the pads back to whatever held them before the open function took over. Must come before
     *  unbinding qspi_a_device only in the sense that the reset above needs it - these are field writes
     *  through a_device, not through the vendored accessors.
     */

    padstatus = fpga910x_qspi_restore_pads(a_device, peripheral_id);
    if (padstatus != 0) {
        AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_WARNING, "%s: failed to restore the qspi pad "
            "routing (%d)! The pads are left with the external core, so the ps2p9102 internal qspi "
            "engine will not work until an open succeeds.\n", __func__, padstatus);
    }

    qspi_is_open = 0;
    qspi_a_device = NULL;

    /*  a latched bus error is the more fundamental failure, so it wins over a pad restore failure */

    status = fpga910x_qspi_bus_status();
    if (status == 0) status = padstatus;
    return(status);
}

// **********************************************************************************
// **********************************************************************************
//
// Register access for the vendored AMD/Xilinx XSpi driver, declared in
// third_party/xil_bsp/xil_io.h. These are the accessors that driver reaches every register through, so
// qspi register traffic goes through the FPGA910X HAL like every other axi_* driver rather than
// dereferencing a memory mapped pointer.
//
// THE NAMES BELOW ARE MACROS. xil_io.h defines Xil_In32 as adi_fpga910x_xil_in32 and Xil_Out32 as
// adi_fpga910x_xil_out32, so these definitions - and the vendored driver's calls to them - resolve to
// the adi_ names and nothing called Xil_* is exported from ADRV910XApi.a. That matters because the
// upstream names belong to the Xilinx standalone BSP: a consumer who links one too would otherwise get
// a duplicate definition, and a consumer with their own Xil_In32 for a different peripheral could have
// it silently captured by this one - which would be wrong, not merely different, because of the
// address space below. See xil_io.h for the reasoning.
//
// Addr is an axi bus offset, not a virtual address - fpga910x_qspi_external_ip_open sets XSpi.BaseAddr
// to the axi offset of the core, and the vendored driver adds its register offsets to that.
//
// Spelled with standard types rather than the u32/UINTPTR aliases. Those are the same types, but the
// standard spelling keeps fpga910x_qspi_external_ip.h free of any xil_* dependency, so a caller of this
// driver needs that one header and nothing else.
//
// Xil_In32 cannot report a failure - every uint32_t is a legitimate register value - so a bus error is
// latched in qspi_bus_status instead and checked after each vendored driver call.

/*  One register read's worth of fall through value, once an error is latched.
 *
 *  Every register reports FPGA910X_QSPI_BUS_ERROR_VALUE, which unblocks the vendored driver's unbounded
 *  poll loops - except the status register, where RX_EMPTY has to change state part way through.
 *
 *  Stats.BytesTransferred is incremented once per receive drain iteration, which is the same event that
 *  decrements the driver's byte counter. So reporting RX_EMPTY clear for exactly count/bytes_per_word
 *  drains and set thereafter lands the byte counter on zero and the receive pointer on the end of the
 *  caller's buffer - whether the error latched at the start of the transfer or part way through it, and
 *  on the write path too, where there is no receive buffer to bound against but the drain still has to
 *  run to decrement the counter.
 *
 *  Outside a transfer qspi_drain_bytes_per_word is zero and this is a plain constant.
 */

static uint32_t fpga910x_qspi_fallthrough(uintptr_t Addr) {

    uint32_t drained;

    if ((qspi_drain_bytes_per_word != 0) &&
        (Addr == (uintptr_t) (qspi_instance.BaseAddr + XSP_SR_OFFSET))) {

        drained = (qspi_instance.Stats.BytesTransferred - qspi_drain_stats_base) /
                  qspi_drain_bytes_per_word;

        if (drained >= qspi_drain_limit_words) {
            return(FPGA910X_QSPI_BUS_ERROR_VALUE | XSP_SR_RX_EMPTY_MASK);
        }
    }

    return(FPGA910X_QSPI_BUS_ERROR_VALUE);
}

uint32_t Xil_In32(uintptr_t Addr) {

    int32_t status;
    uint32_t data = 0;

    if (qspi_a_device == NULL) {
        qspi_bus_status = FPGA910X_QSPI_ERR_NOT_OPEN;
        return(fpga910x_qspi_fallthrough(Addr));
    }

    /*  once an error is latched, keep reporting the fall through value for the rest of the call, so a
     *  failure part way through a polling loop still unwinds rather than spinning
     */

    if (qspi_bus_status != 0) return(fpga910x_qspi_fallthrough(Addr));

    status = AXI_REG_READ(qspi_a_device, (uint32_t) Addr, 0, &data);
    if (status != 0) {
        qspi_bus_status = status;
        return(fpga910x_qspi_fallthrough(Addr));
    }

    return((uint32_t) data);
}

void Xil_Out32(uintptr_t Addr, uint32_t Value) {

    int32_t status;

    if (qspi_a_device == NULL) {
        qspi_bus_status = FPGA910X_QSPI_ERR_NOT_OPEN;
        return;
    }

    status = AXI_REG_WRITE(qspi_a_device, (uint32_t) Addr, 0, (uint32_t) Value);
    if (status != 0) qspi_bus_status = status;
}

// **********************************************************************************
// **********************************************************************************
