#include "basic_example.h"
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include "adiol100.h"
#include "common_data.h"
#include "no_os_print_log.h"
#include "no_os_spi.h"


static int mseq_type0_read(struct adiol100_dev *dev, enum adiol100_channel ch,
                           int iol_ch, uint8_t addr, uint8_t *value)
{
    uint8_t tx[2];
    uint8_t rx[16];
    uint8_t rx_len;
    int ret;

    tx[0] = IOL_MC(IOL_READ, iol_ch, addr);
    tx[1] = IOL_CKT_TYPE0;

    ret = adiol100_load_and_send_msg(dev, ch, tx, 2, 2, ADIOL100_DISCARD_MSG);
    if (ret)
        return ret;

    no_os_mdelay(20);

    ret = adiol100_read_msg(dev, ch, rx, &rx_len);
    if (ret)
        return ret;

    if (rx_len >= 1)
        *value = rx[0];
    else
        return -EIO;

    return 0;
}

static int mseq_type0_write(struct adiol100_dev *dev, enum adiol100_channel ch,
                            int iol_ch, uint8_t addr, uint8_t value)
{
    uint8_t tx[3];
    int ret;

    tx[0] = IOL_MC(IOL_WRITE, iol_ch, addr);
    tx[1] = IOL_CKT_TYPE0;
    tx[2] = value;

    ret = adiol100_load_and_send_msg(dev, ch, tx, 3, 1, ADIOL100_DISCARD_MSG);
    if (ret)
        return ret;

    no_os_mdelay(20);
    return 0;
}

static int mseq_type1v_read(struct adiol100_dev *dev, enum adiol100_channel ch,
                            int iol_ch, uint8_t addr, int od_bytes,
                            uint8_t *data, uint8_t *data_len)
{
    int rx_bytes = od_bytes + 1;
    uint8_t tx[2];
    uint8_t rx[36];
    uint8_t rx_len;
    int ret;

    tx[0] = IOL_MC(IOL_READ, iol_ch, addr);
    tx[1] = IOL_CKT_TYPE1;

    ret = adiol100_load_and_send_msg(dev, ch, tx, 2, rx_bytes, ADIOL100_DISCARD_MSG);
    if (ret)
        return ret;

    no_os_mdelay(20);

    ret = adiol100_read_msg(dev, ch, rx, &rx_len);
    if (ret)
        return ret;

    int copy = rx_len - 1;
    if (copy > od_bytes)
        copy = od_bytes;
    if (copy < 0)
        copy = 0;
    for (int i = 0; i < copy; i++)
        data[i] = rx[i];
    *data_len = copy;

    return 0;
}

static int mseq_type1v_write(struct adiol100_dev *dev, enum adiol100_channel ch,
                             int iol_ch, uint8_t addr,
                             uint8_t *data, int data_len, int od_bytes)
{
    int tx_bytes = 2 + od_bytes;
    uint8_t tx[32] = {0};
    int ret;

    tx[0] = IOL_MC(IOL_WRITE, iol_ch, addr);
    tx[1] = IOL_CKT_TYPE1;
    for (int i = 0; i < data_len && i < od_bytes; i++)
        tx[2 + i] = data[i];

    ret = adiol100_load_and_send_msg(dev, ch, tx, tx_bytes, 1, ADIOL100_DISCARD_MSG);
    if (ret)
        return ret;

    no_os_mdelay(20);
    return 0;
}

static int mseq_type2_read(struct adiol100_dev *dev, enum adiol100_channel ch,
                           int iol_ch, uint8_t addr,
                           int od_bytes, int pd_in_bytes)
{
    int rx_bytes = od_bytes + pd_in_bytes;
    uint8_t tx[2];

    tx[0] = IOL_MC(IOL_READ, iol_ch, addr);
    tx[1] = IOL_CKT_TYPE2;

    adiol100_enable_cycle_timer(dev, ch);
    return adiol100_load_and_send_msg(dev, ch, tx, 2, rx_bytes, ADIOL100_KEEP_MSG);
}

static int iol_isdu_send(struct adiol100_dev *dev, enum adiol100_channel ch,
                         int index, int od_bytes)
{
    uint8_t isdu_cmd[3];

    isdu_cmd[0] = 0x93;
    isdu_cmd[1] = (uint8_t)index;
    isdu_cmd[2] = isdu_cmd[0] ^ isdu_cmd[1];

    return mseq_type1v_write(dev, ch, IOL_CH_ISDU, IOL_ISDU_START,
                             isdu_cmd, 3, od_bytes);
}

static int iol_isdu_poll(struct adiol100_dev *dev, enum adiol100_channel ch,
                         int page_num, int od_bytes,
                         uint8_t *data, uint8_t *data_len)
{
    uint8_t addr;

    if (page_num == 0)
        addr = IOL_ISDU_START;
    else
        addr = page_num;

    return mseq_type1v_read(dev, ch, IOL_CH_ISDU, addr, od_bytes,
                            data, data_len);
}

static int iol_isdu_close(struct adiol100_dev *dev, enum adiol100_channel ch,
                          int od_bytes)
{
    uint8_t data[32];
    uint8_t data_len;

    return mseq_type1v_read(dev, ch, IOL_CH_ISDU, IOL_ISDU_IDLE, od_bytes,
                            data, &data_len);
}

static int iol_cyclic_start(struct adiol100_dev *dev, enum adiol100_channel ch,
                            int op_od_bytes, int pd_in_bytes)
{
    return mseq_type2_read(dev, ch, IOL_CH_ISDU, IOL_ISDU_IDLE,
                           op_od_bytes, pd_in_bytes);
}

int basic_example_main(void)
{
    struct adiol100_dev *dev;
    int ret;

    ret = adiol100_init(&dev, &adiol100_ip);
    if (ret)
        return ret;

    pr_info("-----hello adiol100-----\r\n");

    adiol100_config_lp(dev, ADIOL100_CH_A, ADIOL100_LP_EN, ADIOL100_LP_REV_EN);
    no_os_mdelay(3000);
    adiol100_config_cq(dev, ADIOL100_CH_A, ADIOL100_CQ_PUSHPULL,
                       ADIOL100_CQ_DRV_EN, ADIOL100_SINKSEL_5MA,
                       ADIOL100_CQSLEW_250NS);
    adiol100_config_framer(dev, ADIOL100_CH_A, ADIOL100_CHKS_EN,
                           ADIOL100_FRAMER_EN);

    ret = adiol100_estcom(dev, ADIOL100_CH_A);
    if (ret) {
        pr_info("EstCom failed\r\n");
        adiol100_remove(dev);
        return ret;
    }

    uint8_t comrt;
    adiol100_get_comrt(dev, ADIOL100_CH_A, &comrt);
    pr_info("Slave responded at COM%d\r\n", comrt);

    /* STARTUP: read required DPP parameters */
    uint8_t min_cyc, mseq_cap, pd_in, pd_out;

    mseq_type0_read(dev, ADIOL100_CH_A, IOL_CH_PAGE, IOL_DPP_MINCYCLETIME, &min_cyc);
    mseq_type0_read(dev, ADIOL100_CH_A, IOL_CH_PAGE, IOL_DPP_MSEQCAP, &mseq_cap);
    mseq_type0_read(dev, ADIOL100_CH_A, IOL_CH_PAGE, IOL_DPP_PDIN, &pd_in);
    mseq_type0_read(dev, ADIOL100_CH_A, IOL_CH_PAGE, IOL_DPP_PDOUT, &pd_out);
    pr_info("MinCycleTime=0x%02X  MSeqCap=0x%02X  PDIn=0x%02X  PDOut=0x%02X\r\n",
            min_cyc, mseq_cap, pd_in, pd_out);

    int preop_od_bytes = iol_decode_preop_od(mseq_cap);
    int op_od_bytes = iol_decode_operate_od(mseq_cap);
    int pd_in_bytes = iol_decode_pd_len(pd_in);
    int pd_out_bytes = iol_decode_pd_len(pd_out);
    pr_info("PreOp OD=%d  Operate OD=%d  PDIn=%d  PDOut=%d\r\n",
            preop_od_bytes, op_od_bytes, pd_in_bytes, pd_out_bytes);

    /* MasterIdent */
    mseq_type0_write(dev, ADIOL100_CH_A, IOL_CH_PAGE, IOL_DPP_MASTERCMD,
                     IOL_CMD_MASTERIDENT);
    pr_info("MasterIdent sent\r\n");

    /* Read optional identification DPP 0x07-0x0D */
    static const struct { uint8_t addr; const char *name; } dpp_id[] = {
        { 0x07, "VendorID MSB" },  { 0x08, "VendorID LSB" },
        { 0x09, "DeviceID b1" },   { 0x0A, "DeviceID b2" },
        { 0x0B, "DeviceID b3" },
        { 0x0C, "FunctionID MSB" },{ 0x0D, "FunctionID LSB" },
    };

    for (int i = 0; i < 7; i++) {
        uint8_t val;
        mseq_type0_read(dev, ADIOL100_CH_A, IOL_CH_PAGE, dpp_id[i].addr, &val);
        pr_info("DPP 0x%02X %-16s = 0x%02X\r\n",
                dpp_id[i].addr, dpp_id[i].name, val);
    }

    /* SetPreOp (still in STARTUP, TYPE_0) */
    mseq_type0_write(dev, ADIOL100_CH_A, IOL_CH_PAGE, IOL_DPP_MASTERCMD,
                     IOL_CMD_PREOPERATE);
    pr_info("SetPreOp sent\r\n");

    /* ISDU reads in PREOPERATE */
    pr_info("ISDU reads...\r\n");
    {
        static const int isdu_indices[] = {
            IOL_ISDU_SERIALNUMBER, IOL_ISDU_VENDORNAME, IOL_ISDU_VENDORTEXT,
            IOL_ISDU_PRODUCTNAME,  IOL_ISDU_PRODUCTID,  IOL_ISDU_PRODUCTTEXT,
        };
        int num_isdu = sizeof(isdu_indices) / sizeof(isdu_indices[0]);

        for (int p = 0; p < num_isdu; p++) {
            uint8_t od[32];
            uint8_t od_len;
            uint8_t result[235];
            int result_len = 0;
            int data_len;
            int data_start;
            int polls;

            iol_isdu_send(dev, ADIOL100_CH_A, isdu_indices[p], preop_od_bytes);

            /* Wait for device to be ready */
            polls = 0;
            do {
                iol_isdu_poll(dev, ADIOL100_CH_A, 0, preop_od_bytes,
                             od, &od_len);
                polls++;
            } while (od[0] == 0x01 && polls < 100);

            /* Parse response header */
            if ((od[0] & 0xF0) == 0xC0) {
                iol_isdu_close(dev, ADIOL100_CH_A, preop_od_bytes);
                pr_info("  ISDU %d: error\r\n", isdu_indices[p]);
                continue;
            }

            if ((od[0] & 0x0F) == 0x01) {
                data_len = od[1] - 3;
                data_start = 2;
            } else {
                data_len = (od[0] & 0x0F) - 2;
                data_start = 1;
            }

            for (int i = data_start; i < od_len; i++)
                result[result_len++] = od[i];

            /* Collect continuation pages */
            int page = 1;
            while (result_len < data_len) {
                iol_isdu_poll(dev, ADIOL100_CH_A, page++, preop_od_bytes,
                             od, &od_len);
                for (int i = 0; i < od_len && result_len < data_len; i++)
                    result[result_len++] = od[i];
            }

            iol_isdu_close(dev, ADIOL100_CH_A, preop_od_bytes);

            pr_info("  ISDU %d (len=%d): ", isdu_indices[p], data_len);
            for (int i = 0; i < data_len; i++)
                pr_info("%c", result[i]);
            pr_info("\r\n");
        }
    }

    /* SetCycleTime (TYPE_1_V in PREOPERATE) */
    mseq_type1v_write(dev, ADIOL100_CH_A, IOL_CH_PAGE, IOL_DPP_CYCLETIME,
                      &min_cyc, 1, preop_od_bytes);
    pr_info("SetCycleTime=0x%02X\r\n", min_cyc);

    /* SetOperate (TYPE_1_V in PREOPERATE) */
    uint8_t op_cmd = IOL_CMD_OPERATE;
    mseq_type1v_write(dev, ADIOL100_CH_A, IOL_CH_PAGE, IOL_DPP_MASTERCMD,
                      &op_cmd, 1, preop_od_bytes);
    pr_info("SetOperate sent\r\n");

    /* OPERATE: cyclic PD exchange (TYPE_2) */
    iol_cyclic_start(dev, ADIOL100_CH_A, op_od_bytes, pd_in_bytes);
    no_os_mdelay(50);

    /* Poll for cyclic PD */
    uint8_t rx[16];
    uint8_t rx_len;
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
