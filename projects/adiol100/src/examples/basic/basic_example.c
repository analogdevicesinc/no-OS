#include "basic_example.h"
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include "adiol100.h"
#include "common_data.h"
#include "no_os_print_log.h"
#include "no_os_spi.h"

static int iol_send_frame(struct adiol100_dev *dev, enum adiol100_channel ch,
                          uint8_t mc, uint8_t ckt,
                          uint8_t *od, uint8_t od_len,
                          uint8_t rx_bytes, enum adiol100_keep_msg keep)
{
    uint8_t tx[IOL_MAX_FRAME] = {0};

    tx[0] = mc;
    tx[1] = ckt;
    for (int i = 0; i < od_len; i++)
        tx[2 + i] = od[i];

    int ret = adiol100_load_and_send_msg(dev, ch, tx, 2 + od_len,
                                         rx_bytes, keep);
    if (ret)
        return ret;

    no_os_mdelay(20);
    return 0;
}

static int iol_send_frame_and_read(struct adiol100_dev *dev,
                                    enum adiol100_channel ch,
                                    uint8_t mc, uint8_t ckt,
                                    uint8_t *od, uint8_t od_len,
                                    uint8_t rx_bytes, enum adiol100_keep_msg keep,
                                    uint8_t *rx, uint8_t *rx_len)
{
    int ret;

    ret = iol_send_frame(dev, ch, mc, ckt, od, od_len, rx_bytes, keep);
    if (ret)
        return ret;

    return adiol100_read_msg(dev, ch, rx, rx_len);
}

int basic_example_main(void)
{
    struct adiol100_dev *dev;
    uint8_t rx[IOL_MAX_FRAME];
    uint8_t rx_len;
    int ret;

    ret = adiol100_init(&dev, &adiol100_ip);
    if (ret)
        return ret;

    pr_info("-----ADIOL100 Basic Example-----\r\n");

    /* ════════════════════════════════════════════════════════════════════════
     *  STARTUP
     * ══════════════════════════════════════════════════════════════════════ */

    /* Configure transceiver */
    adiol100_config_lp(dev, ADIOL100_CH_A, ADIOL100_LP_EN, ADIOL100_LP_REV_EN);
    no_os_mdelay(3000);
    adiol100_config_cq(dev, ADIOL100_CH_A, ADIOL100_CQ_PUSHPULL,
                       ADIOL100_CQ_DRV_EN, ADIOL100_SINKSEL_5MA,
                       ADIOL100_CQSLEW_250NS);
    adiol100_config_framer(dev, ADIOL100_CH_A, ADIOL100_CHKS_EN,
                           ADIOL100_FRAMER_EN);

    /* Send EstablishCommunication command */
    ret = adiol100_estcom(dev, ADIOL100_CH_A);
    if (ret) {
        pr_info("EstCom failed\r\n");
        adiol100_remove(dev);
        return ret;
    }

    uint8_t comrt;
    adiol100_get_comrt(dev, ADIOL100_CH_A, &comrt);
    pr_info("Slave responded at COM%d\r\n", comrt);

    /* Read required DPP parameters */
    uint8_t min_cyc, mseq_cap, pd_in, pd_out;

    iol_send_frame_and_read(dev, ADIOL100_CH_A,
                            IOL_MC(IOL_READ, IOL_CH_PAGE, IOL_DPP_MINCYCLETIME),
                            IOL_CKT_TYPE0, NULL, 0, 2, ADIOL100_DISCARD_MSG,
                            rx, &rx_len);
    min_cyc = rx[0];

    iol_send_frame_and_read(dev, ADIOL100_CH_A,
                            IOL_MC(IOL_READ, IOL_CH_PAGE, IOL_DPP_MSEQCAP),
                            IOL_CKT_TYPE0, NULL, 0, 2, ADIOL100_DISCARD_MSG,
                            rx, &rx_len);
    mseq_cap = rx[0];

    iol_send_frame_and_read(dev, ADIOL100_CH_A,
                            IOL_MC(IOL_READ, IOL_CH_PAGE, IOL_DPP_PDIN),
                            IOL_CKT_TYPE0, NULL, 0, 2, ADIOL100_DISCARD_MSG,
                            rx, &rx_len);
    pd_in = rx[0];

    iol_send_frame_and_read(dev, ADIOL100_CH_A,
                            IOL_MC(IOL_READ, IOL_CH_PAGE, IOL_DPP_PDOUT),
                            IOL_CKT_TYPE0, NULL, 0, 2, ADIOL100_DISCARD_MSG,
                            rx, &rx_len);
    pd_out = rx[0];

    pr_info("MinCycleTime=0x%02X  MSeqCap=0x%02X  PDIn=0x%02X  PDOut=0x%02X\r\n",
            min_cyc, mseq_cap, pd_in, pd_out);

    int preop_od_bytes = iol_decode_preop_od(mseq_cap);
    int op_od_bytes = iol_decode_operate_od(mseq_cap);
    int pd_in_bytes = iol_decode_pd_len(pd_in);
    int pd_out_bytes = iol_decode_pd_len(pd_out);
    pr_info("PreOp OD=%d  Operate OD=%d  PDIn=%d  PDOut=%d\r\n",
            preop_od_bytes, op_od_bytes, pd_in_bytes, pd_out_bytes);

    /* Send MasterIdent command */
    uint8_t cmd = IOL_CMD_MASTERIDENT;
    iol_send_frame(dev, ADIOL100_CH_A,
                   IOL_MC(IOL_WRITE, IOL_CH_PAGE, IOL_DPP_MASTERCMD),
                   IOL_CKT_TYPE0, &cmd, 1, 1, ADIOL100_DISCARD_MSG);
    pr_info("MasterIdent sent\r\n");

    /* Read optional identification DPP 0x07-0x0D */
    static const struct { uint8_t addr; const char *name; } dpp_id[] = {
        { IOL_DPP_VENDORID_MSB,   "VendorID MSB"  },
        { IOL_DPP_VENDORID_LSB,   "VendorID LSB"  },
        { IOL_DPP_DEVICEID_B1,    "DeviceID b1"   },
        { IOL_DPP_DEVICEID_B2,    "DeviceID b2"   },
        { IOL_DPP_DEVICEID_B3,    "DeviceID b3"   },
        { IOL_DPP_FUNCTIONID_MSB, "FunctionID MSB"},
        { IOL_DPP_FUNCTIONID_LSB, "FunctionID LSB"},
    };

    for (int i = 0; i < (int)NO_OS_ARRAY_SIZE(dpp_id); i++) {
        uint8_t val;
        iol_send_frame_and_read(dev, ADIOL100_CH_A,
                                IOL_MC(IOL_READ, IOL_CH_PAGE, dpp_id[i].addr),
                                IOL_CKT_TYPE0, NULL, 0, 2, ADIOL100_DISCARD_MSG,
                                rx, &rx_len);
        val = rx[0];
        pr_info("DPP 0x%02X %-16s = 0x%02X\r\n",
                dpp_id[i].addr, dpp_id[i].name, val);
    }

    /* Send Preoperate command */
    cmd = IOL_CMD_PREOPERATE;
    iol_send_frame(dev, ADIOL100_CH_A,
                   IOL_MC(IOL_WRITE, IOL_CH_PAGE, IOL_DPP_MASTERCMD),
                   IOL_CKT_TYPE0, &cmd, 1, 1, ADIOL100_DISCARD_MSG);
    pr_info("SetPreOp sent\r\n");

    /* ════════════════════════════════════════════════════════════════════════
     *  PREOPERATE
     * ══════════════════════════════════════════════════════════════════════ */

    /* Read ISDU pages (OPTIONAL) */
    pr_info("ISDU reads...\r\n");
    {
        static const int isdu_indices[] = {
            IOL_ISDU_SERIALNUMBER, IOL_ISDU_VENDORNAME, IOL_ISDU_VENDORTEXT,
            IOL_ISDU_PRODUCTNAME,  IOL_ISDU_PRODUCTID,  IOL_ISDU_PRODUCTTEXT,
        };

        for (int p = 0; p < (int)NO_OS_ARRAY_SIZE(isdu_indices); p++) {
            uint8_t od[IOL_MAX_OD];
            uint8_t od_len;
            uint8_t result[IOL_MAX_ISDU_DATA];
            int result_len = 0;
            int data_len;
            int data_start;
            int polls;

            /* Send ISDU read request */
            uint8_t isdu_cmd[] = IOL_ISDU_READ_CMD(isdu_indices[p]);
            iol_send_frame(dev, ADIOL100_CH_A,
                           IOL_MC(IOL_WRITE, IOL_CH_ISDU, IOL_ISDU_START),
                           IOL_CKT_TYPE1, isdu_cmd, preop_od_bytes, 1,
                           ADIOL100_DISCARD_MSG);

            /* Poll until device is ready */
            polls = 0;
            do {
                iol_send_frame_and_read(dev, ADIOL100_CH_A,
                                        IOL_MC(IOL_READ, IOL_CH_ISDU, IOL_ISDU_START),
                                        IOL_CKT_TYPE1, NULL, 0,
                                        preop_od_bytes + 1, ADIOL100_DISCARD_MSG,
                                        od, &od_len);
                /* Strip CKS byte from length */
                if (od_len > 0)
                    od_len--;
                polls++;
            } while (od[0] == IOL_ISDU_BUSY && polls < 100);

            /* Parse response header */
            if ((od[0] & IOL_ISDU_ISERVICE_MSK) == IOL_ISDU_READ_ERROR) {
                /* Close ISDU transaction */
                iol_send_frame(dev, ADIOL100_CH_A,
                               IOL_MC(IOL_READ, IOL_CH_ISDU, IOL_ISDU_IDLE),
                               IOL_CKT_TYPE1, NULL, 0,
                               preop_od_bytes + 1, ADIOL100_DISCARD_MSG);
                adiol100_reset_rx_fifo(dev, ADIOL100_CH_A);
                pr_info("  ISDU %d: error\r\n", isdu_indices[p]);
                continue;
            }

            if ((od[0] & IOL_ISDU_LEN_MSK) == IOL_ISDU_LONG_FORM) {
                data_len = od[1] - IOL_ISDU_LONG_HDR_OVERHEAD;
                data_start = 2;
            } else {
                data_len = (od[0] & IOL_ISDU_LEN_MSK) - IOL_ISDU_SHORT_HDR_OVERHEAD;
                data_start = 1;
            }

            for (int i = data_start; i < od_len; i++)
                result[result_len++] = od[i];

            /* Collect continuation pages */
            int page = 1;
            while (result_len < data_len) {
                iol_send_frame_and_read(dev, ADIOL100_CH_A,
                                        IOL_MC(IOL_READ, IOL_CH_ISDU, page),
                                        IOL_CKT_TYPE1, NULL, 0,
                                        preop_od_bytes + 1, ADIOL100_DISCARD_MSG,
                                        od, &od_len);
                if (od_len > 0)
                    od_len--;
                page++;
                for (int i = 0; i < od_len && result_len < data_len; i++)
                    result[result_len++] = od[i];
            }

            /* Close ISDU transaction */
            iol_send_frame(dev, ADIOL100_CH_A,
                           IOL_MC(IOL_READ, IOL_CH_ISDU, IOL_ISDU_IDLE),
                           IOL_CKT_TYPE1, NULL, 0,
                           preop_od_bytes + 1, ADIOL100_DISCARD_MSG);
            adiol100_reset_rx_fifo(dev, ADIOL100_CH_A);

            pr_info("  ISDU %d (len=%d): ", isdu_indices[p], data_len);
            for (int i = 0; i < data_len; i++)
                pr_info("%c", result[i]);
            pr_info("\r\n");
        }
    }

    /* Set cycle time */
    {
        uint8_t od[IOL_MAX_OD] = {0};
        od[0] = min_cyc;
        iol_send_frame(dev, ADIOL100_CH_A,
                       IOL_MC(IOL_WRITE, IOL_CH_PAGE, IOL_DPP_CYCLETIME),
                       IOL_CKT_TYPE1, od, preop_od_bytes, 1,
                       ADIOL100_DISCARD_MSG);
        pr_info("SetCycleTime=0x%02X\r\n", min_cyc);
    }

    /* Send Operate command */
    {
        uint8_t od[IOL_MAX_OD] = {0};
        od[0] = IOL_CMD_OPERATE;
        iol_send_frame(dev, ADIOL100_CH_A,
                       IOL_MC(IOL_WRITE, IOL_CH_PAGE, IOL_DPP_MASTERCMD),
                       IOL_CKT_TYPE1, od, preop_od_bytes, 1,
                       ADIOL100_DISCARD_MSG);
        pr_info("SetOperate sent\r\n");
    }

    /* ════════════════════════════════════════════════════════════════════════
     *  OPERATE
     * ══════════════════════════════════════════════════════════════════════ */

    /* Request cyclic transfers */
    adiol100_enable_cycle_timer(dev, ADIOL100_CH_A);
    iol_send_frame(dev, ADIOL100_CH_A,
                   IOL_MC(IOL_READ, IOL_CH_ISDU, IOL_ISDU_IDLE),
                   IOL_CKT_TYPE2, NULL, 0, op_od_bytes + pd_in_bytes,
                   ADIOL100_KEEP_MSG);

    for (int cyc = 0; cyc < 20; cyc++) {
        adiol100_read_msg(dev, ADIOL100_CH_A, rx, &rx_len);
        if (rx_len > op_od_bytes + 1) {
            int16_t dist = (int16_t)((rx[op_od_bytes] << 8) | rx[op_od_bytes + 1]);
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
