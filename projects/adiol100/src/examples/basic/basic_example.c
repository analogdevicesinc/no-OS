#include "basic_example.h"
#include <stdint.h>
#include "adiol100.h"
#include "common_data.h"
#include "no_os_print_log.h"
#include "no_os_spi.h"

int basic_example_main(void)
{
    struct adiol100_dev *dev;
    int ret;

    ret = adiol100_init(&dev, &adiol100_ip);
    if (ret) 
        return ret;

    pr_info("-----hello adiol100-----\r\n");

    uint16_t v;
    ret = adiol100_read(dev, 0x1F02, &v);  pr_info("RevID    = 0x%04X\r\n", v);
    if (ret)
        return ret;

    adiol100_reset_channel_a(dev);

    adiol100_read(dev, 0x1F03, &v);  pr_info("DeviceID = 0x%04X\r\n", v);
    adiol100_read(dev, 0x0003, &v);  pr_info("StatusA  = 0x%04X\r\n", v);

    adiol100_write(dev, 0x1F01, 0x0011);   /* ClockCfg = crystal */
    adiol100_write(dev, 0x000D, 0x8100);   /* LPCfgA = enable L+ */
    adiol100_read(dev, 0x000D, &v);  pr_info("LPCfgA   = 0x%04X\r\n", v);

    no_os_mdelay(3000);   /* 3 seconds for sensor to boot */

    adiol100_write(dev, 0x0009, 0x1060);   /* CQASet: push-pull, drive enable, glitch filter */
    adiol100_read(dev, 0x0009, &v);  pr_info("CQASet   = 0x%04X\r\n", v);

    adiol100_read(dev, 0x0003, &v); pr_info("StatusA  = 0x%04X\r\n", v);

    adiol100_write(dev, 0x0005, 0x1004);   /* FramCtrl1A: InsChks + FramerEn */
    adiol100_read(dev, 0x0005, &v); pr_info("FramCtrl1A  = 0x%04X\r\n", v);
    adiol100_write(dev, 0x1F04, 0x0404);   /* SPIFifoABurst: 4-byte read/write */
    adiol100_read(dev, 0x1F04, &v); pr_info("SPIFifoABurst  = 0x%04X\r\n", v);

    /* Trigger EstCom */
    adiol100_write(dev, 0x0005, 0x1024);
    no_os_mdelay(100);

    /* Check result */
    uint16_t fc1, rxstat;
    adiol100_read(dev, 0x0005, &fc1);
    adiol100_read(dev, 0x0001, &rxstat);
    pr_info("FramCtrl1A = 0x%04X\r\n", fc1);
    pr_info("RxFIFOStat = 0x%04X\r\n", rxstat);

    uint8_t comrt = (fc1 >> 6) & 0x03;
    uint8_t fifo_level = rxstat & 0xFF;
    pr_info("COMRT=%d  fifo_level=%d\r\n", comrt, fifo_level);

    if (comrt != 0 && fifo_level > 0) {
        pr_info("Slave responded at COM%d!\r\n", comrt);

        /* Read FIFO: fifo_level + 2 bytes */
        uint8_t total = fifo_level + 2;
        uint8_t cmd = (0x01 << 6) | (0x04 << 1) | 1;
        uint8_t fifo_buf[16] = {0};
        fifo_buf[0] = cmd;
        no_os_spi_write_and_read(dev->spi_desc, fifo_buf, total + 1);

        pr_info("EstCom FIFO:");
        for (int i = 1; i <= total; i++)
            pr_info(" %02X", fifo_buf[i]);
        pr_info("\r\n");
    } else {
        pr_info("No slave response\r\n");
    }

    adiol100_read(dev, 0x0007, &v);  pr_info("CyclTmrA   = 0x%04X\r\n", v);

    /* ============================================================
     * IDENTIFY SLAVE: read DPP addresses 0x02-0x06
     * All TYPE_0: MC = 0xA0 | address, TxBytes=2, RxBytes=2
     * SPIFifoABurst: write_len = TxBytes+4 = 6, read_len = RxBytes+3 = 5
     * ============================================================ */
    /* write_len = 3+TxBytes = 5, read_len = 2+RxBytes = 4 */
    adiol100_write(dev, 0x1F04, 0x0405);

    static const struct { uint8_t addr; const char *name; } dpp[] = {
        { 0x02, "MinCycleTime" },
        { 0x03, "MSeqCap" },
        { 0x04, "RevisionID" },
        { 0x05, "ProcessDataIn" },
        { 0x06, "ProcessDataOut" },
    };

    uint8_t rx[16];
    uint8_t rx_len;

    for (int i = 0; i < 5; i++) {
        uint8_t tx[5] = {
            i + 1,              /* MsgID: sequential */
            0x02,               /* RxBytes: expect 2 back */
            0x02,               /* TxBytes: sending 2 on wire */
            0xA0 | dpp[i].addr, /* MC: read page address */
            0x00,               /* CKT: chip fills checksum */
        };

        adiol100_fifo_write(dev, tx, 5, 0x10C5);
        no_os_mdelay(20);

        adiol100_fifo_read(dev, rx, &rx_len);
        pr_info("DPP 0x%02X %-16s = 0x%02X  raw(%d):",
                dpp[i].addr, dpp[i].name,
                (rx_len >= 3) ? rx[2] : 0, rx_len);
        for (int j = 0; j < rx_len; j++) pr_info(" %02X", rx[j]);
        pr_info("\r\n");
    }

    /* write_len = 3+3 = 6, read_len = 2+1 = 3 */
    adiol100_write(dev, 0x1F04, 0x0306);

    uint8_t tx[6] = { 0x06, 0x01, 0x03, 0x20, 0x00, 0x95 };
    adiol100_fifo_write(dev, tx, 6, 0x10C5);
    no_os_mdelay(20);

    adiol100_fifo_read(dev, rx, &rx_len);
    pr_info("MasterIdent resp (%d):", rx_len);
    for (int i = 0; i < rx_len; i++) pr_info(" %02X", rx[i]);
    pr_info("\r\n");

    /* Read DPP 0x07-0x0D: identification parameters */
    /* write_len = 3+2 = 5, read_len = 2+2 = 4 */
    adiol100_write(dev, 0x1F04, 0x0405);

    static const struct { uint8_t addr; const char *name; } dpp_id[] = {
        { 0x07, "VendorID MSB" },
        { 0x08, "VendorID LSB" },
        { 0x09, "DeviceID b1" },
        { 0x0A, "DeviceID b2" },
        { 0x0B, "DeviceID b3" },
        { 0x0C, "FunctionID MSB" },
        { 0x0D, "FunctionID LSB" },
    };

    for (int i = 0; i < 7; i++) {
        uint8_t tx2[5] = {
            i + 7,                  /* MsgID: continue sequence from 7 */
            0x02,                   /* RxBytes */
            0x02,                   /* TxBytes */
            0xA0 | dpp_id[i].addr,  /* MC: read page address */
            0x00,                   /* CKT */
        };

        adiol100_fifo_write(dev, tx2, 5, 0x10C5);
        no_os_mdelay(20);

        adiol100_fifo_read(dev, rx, &rx_len);
        pr_info("DPP 0x%02X %-16s = 0x%02X  raw(%d):",
                dpp_id[i].addr, dpp_id[i].name,
                (rx_len >= 3) ? rx[2] : 0, rx_len);
        for (int j = 0; j < rx_len; j++) pr_info(" %02X", rx[j]);
        pr_info("\r\n");
    }

    /* ============================================================
     * PREOPERATE + OPERATE
     * Write commands don't need response validation — if the
     * M-sequence fires without SPI error, the slave accepted.
     * The fifo_lvl=1 residual makes write responses unreliable
     * to read, so we just fire-and-forget for writes.
     * ============================================================ */
    /* write_len = 3+3 = 6, read_len = 2+1 = 3 */
    adiol100_write(dev, 0x1F04, 0x0306);

    /* SetPreOp: write 0x9A to DPP 0x00 */
    uint8_t tx_preop[6] = { 0x0E, 0x01, 0x03, 0x20, 0x00, 0x9A };
    adiol100_fifo_write(dev, tx_preop, 6, 0x10C5);
    no_os_mdelay(20);
    adiol100_read(dev, 0x0001, &v);
    pr_info("SetPreOp: RxFIFOStat=0x%04X\r\n", v);
    adiol100_read(dev, 0x0002, &v);
    pr_info("SetPreOp: InterruptA=0x%04X\r\n", v);

    /* ISDU reads in PREOPERATE.
     * ISDU data rides in the OD bytes of TYPE_1_V M-sequences.
     * Step 1: send ISDU read request (MC=0x70, 8 OD bytes with ISDU cmd)
     * Step 2: poll for response pages (MC=0xF0, then 0xE1, 0xE2...)
     * Step 3: close (MC=0xF1)
     * CKT=0x40 (TYPE_1, InsChks fills checksum). */
    pr_info("ISDU reads...\r\n");
    {
        static const struct { int idx; const char *name; } pages[] = {
            {21, "SerialNr"}, {16, "VendorName"}, {17, "VendorText"},
            {18, "ProdName"}, {19, "ProductID"},  {20, "ProductText"},
        };

        for (int p = 0; p < 6; p++) {
            int index = pages[p].idx;
            uint8_t isdu_cmd[3] = { 0x93, (uint8_t)index, 0 };
            isdu_cmd[2] = isdu_cmd[0] ^ isdu_cmd[1];

            /* Send ISDU command: TxBytes=10, RxBytes=1 */
            adiol100_write(dev, 0x1F04, 0x030D);
            {
                uint8_t cmd_tx[13] = {0};
                cmd_tx[0] = 0x10 + p;
                cmd_tx[1] = 0x01;
                cmd_tx[2] = 0x0A;
                cmd_tx[3] = 0x70;
                cmd_tx[4] = 0x40;
                for (int i = 0; i < 8; i++)
                    cmd_tx[5 + i] = (i < 3) ? isdu_cmd[i] : 0;
                adiol100_fifo_write(dev, cmd_tx, 13, 0x10C5);
                no_os_mdelay(20);
            }

            /* Poll for response: TxBytes=2, RxBytes=9 */
            adiol100_write(dev, 0x1F04, 0x0B05);
            uint8_t result[64];
            int result_len = 0, expected_len = 20;
            int page_num = 0, polls = 0;

            while (result_len < expected_len && polls < 100) {
                uint8_t poll_tx[5] = {
                    18 + polls, 0x09, 0x02,
                    (page_num == 0) ? 0xF0 : (uint8_t)(0xE0 + page_num),
                    0x40
                };
                adiol100_fifo_write(dev, poll_tx, 5, 0x10C5);
                no_os_mdelay(20);
                adiol100_fifo_read(dev, rx, &rx_len);

                if (page_num == 0 && rx_len > 2 && rx[2] == 0x01) {
                    page_num--;
                } else if (page_num == 0 && rx_len > 2 &&
                           (rx[2] & 0xF0) == 0xD0) {
                    if ((rx[2] & 0x0F) == 0x01) {
                        expected_len = (rx_len > 3) ? rx[3] : 0;
                        for (int i = 4; i < 10 && i < rx_len; i++)
                            if (result_len < (int)sizeof(result))
                                result[result_len++] = rx[i];
                    } else {
                        expected_len = rx[2] & 0x0F;
                        for (int i = 3; i < 10 && i < rx_len; i++)
                            if (result_len < (int)sizeof(result))
                                result[result_len++] = rx[i];
                    }
                } else if (rx_len > 2) {
                    for (int i = 1; i < 9 && 1 + i < rx_len; i++)
                        if (result_len < (int)sizeof(result))
                            result[result_len++] = rx[1 + i];
                }
                page_num++;
                polls++;
            }

            if (expected_len > 15) expected_len -= 3;
            else expected_len -= 2;
            if (expected_len < 0) expected_len = 0;

            /* Close ISDU */
            uint8_t close_tx[5] = { 0x01, 0x09, 0x02, 0xF1, 0x40 };
            adiol100_fifo_write(dev, close_tx, 5, 0x10C5);
            no_os_mdelay(20);

            pr_info("  %s (len=%d): ", pages[p].name, expected_len);
            for (int i = 0; i < expected_len && i < result_len; i++) {
                char c = (char)result[i];
                if (c >= 32 && c < 127) pr_info("%c", c);
                else pr_info(".");
            }
            pr_info("\r\n");
        }
    }

    /* SetCycleTime: write 0x08 to DPP 0x01 (TYPE_1_V, TxBytes=10) */
    /* write_len = 3+10 = 13, read_len = 2+1 = 3 */
    adiol100_write(dev, 0x1F04, 0x030D);
    {
        uint8_t tx_cyc[13] = {0};
        tx_cyc[0]  = 0x0F;  /* MsgID */
        tx_cyc[1]  = 0x01;  /* RxBytes */
        tx_cyc[2]  = 0x0A;  /* TxBytes = 2+8 = 10 */
        tx_cyc[3]  = 0x21;  /* MC: write DPP 0x01 */
        tx_cyc[4]  = 0x40;  /* CKT: TYPE_1, InsChks fills checksum */
        tx_cyc[5]  = 0x08;  /* data: MasterCycleTime value */
        /* bytes 6-12 = 0x00 (OD padding, already zeroed) */
        adiol100_fifo_write(dev, tx_cyc, 13, 0x10C5);
    }
    no_os_mdelay(20);
    adiol100_read(dev, 0x0001, &v);
    pr_info("SetCycleTime: RxFIFOStat=0x%04X\r\n", v);

    /* SetOperate: write 0x99 to DPP 0x00 (TYPE_1_V, TxBytes=10) */
    adiol100_write(dev, 0x1F04, 0x030D);
    {
        uint8_t tx_op[13] = {0};
        tx_op[0]  = 0x10;  /* MsgID */
        tx_op[1]  = 0x01;  /* RxBytes */
        tx_op[2]  = 0x0A;  /* TxBytes = 2+8 = 10 */
        tx_op[3]  = 0x20;  /* MC: write DPP 0x00 */
        tx_op[4]  = 0x40;  /* CKT: TYPE_1, InsChks fills checksum */
        tx_op[5]  = 0x99;  /* data: DeviceOperate command */
        /* bytes 6-12 = 0x00 (OD padding, already zeroed) */
        adiol100_fifo_write(dev, tx_op, 13, 0x10C5);
    }
    no_os_mdelay(20);
    adiol100_read(dev, 0x0001, &v);
    pr_info("SetOperate: RxFIFOStat=0x%04X\r\n", v);

    /* Cyclic: TxBytes=2, RxBytes=10 (1+OD2+PDIn7)
     * CKT=0x94 (TYPE_2 in bits 7:6, per GUI)
     * write_len = 2+4 = 6, read_len = 10+3 = 13
     * Trigger = 0x10CF (FramCtrl1A | CQSendA | CycleTmrEn | TxKeepMsg)
     * = 0x10C4 | 0x0B */
    /* write_len = 3+2 = 5, read_len = 2+10 = 12 */
    adiol100_write(dev, 0x1F04, 0x0C05);

    uint8_t tx_cyc2[5] = { 0x11, 0x0A, 0x02, 0xF1, 0x80 };  /* CKT: TYPE_2 */
    adiol100_fifo_write(dev, tx_cyc2, 5, 0x10CF);
    no_os_mdelay(50);

    /* Poll for cyclic PD */
    for (int cyc = 0; cyc < 20; cyc++) {
        adiol100_fifo_read(dev, rx, &rx_len);
        if (rx_len > 4) {
            int16_t dist = (int16_t)((rx[4] << 8) | rx[5]);
            pr_info("Cyclic #%d dist=%dmm raw(%d):", cyc, dist, rx_len);
            for (int i = 0; i < rx_len; i++) pr_info(" %02X", rx[i]);
            pr_info("\r\n");
        } else if (rx_len > 0) {
            pr_info("Cyclic #%d raw(%d):", cyc, rx_len);
            for (int i = 0; i < rx_len; i++) pr_info(" %02X", rx[i]);
            pr_info("\r\n");
        }
        no_os_mdelay(50);
    }

    adiol100_remove(dev);
    return 0;
}
