# Set Cordio root directory
set(CORDIO_ROOT ${MAXIM_LIBRARIES}/Cordio)

# Create cordio static library
# add_library(cordio STATIC)

# Add include directories
target_include_directories(no-os PUBLIC
    ${CORDIO_ROOT}/wsf/include
    ${CORDIO_ROOT}/ble-host/include
    ${CORDIO_ROOT}/ble-host/sources/stack/cfg
    ${CORDIO_ROOT}/ble-host/sources/stack/hci
    ${CORDIO_ROOT}/ble-host/sources/stack/l2c
    ${CORDIO_ROOT}/ble-host/sources/stack/dm
    ${CORDIO_ROOT}/ble-host/sources/stack/smp
    ${CORDIO_ROOT}/ble-host/sources/hci/exactle
    ${CORDIO_ROOT}/ble-host/sec/common
    ${CORDIO_ROOT}/controller/include/ble
    ${CORDIO_ROOT}/controller/include/common
    ${CORDIO_ROOT}/controller/sources/ble/include
    ${CORDIO_ROOT}/platform/include
    ${CORDIO_ROOT}/ble-profiles/include
    ${CORDIO_ROOT}/ble-profiles/sources/profiles
    ${CORDIO_ROOT}/ble-profiles/sources/profiles/include
    ${CORDIO_ROOT}/ble-profiles/sources/services
    ${CORDIO_ROOT}/ble-profiles/sources/af
    ${CORDIO_ROOT}/thirdparty/uecc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cordio_app
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cordio_pal

    # ${MAXIM_LIBRARIES}/PeriphDrivers/Include/MAX32690
    # ${MAXIM_LIBRARIES}/CMSIS/Device/Maxim/MAX32690/Include
)

# Collect all source files individually (excluding ble-profiles/sources/af directory)
set(CORDIO_SOURCES
    # ble-profiles/sources (excluding af directory)
    ${CORDIO_ROOT}/ble-profiles/sources/profiles/anpc/anpc_main.c
    ${CORDIO_ROOT}/ble-profiles/sources/profiles/atpc/atpc_main.c
    ${CORDIO_ROOT}/ble-profiles/sources/profiles/atps/atps_main.c
    ${CORDIO_ROOT}/ble-profiles/sources/profiles/bas/bas_main.c
    ${CORDIO_ROOT}/ble-profiles/sources/profiles/blpc/blpc_main.c
    ${CORDIO_ROOT}/ble-profiles/sources/profiles/blps/blps_main.c
    ${CORDIO_ROOT}/ble-profiles/sources/profiles/cpp/cpps_main.c
    ${CORDIO_ROOT}/ble-profiles/sources/profiles/cscp/cscps_main.c
    ${CORDIO_ROOT}/ble-profiles/sources/profiles/dis/dis_main.c
    ${CORDIO_ROOT}/ble-profiles/sources/profiles/fmpl/fmpl_main.c
    ${CORDIO_ROOT}/ble-profiles/sources/profiles/gap/gap_main.c
    ${CORDIO_ROOT}/ble-profiles/sources/profiles/gatt/gatt_main.c
    ${CORDIO_ROOT}/ble-profiles/sources/profiles/glpc/glpc_main.c
    ${CORDIO_ROOT}/ble-profiles/sources/profiles/glps/glps_db.c
    ${CORDIO_ROOT}/ble-profiles/sources/profiles/glps/glps_main.c
    ${CORDIO_ROOT}/ble-profiles/sources/profiles/hid/hid_main.c
    ${CORDIO_ROOT}/ble-profiles/sources/profiles/hrpc/hrpc_main.c
    ${CORDIO_ROOT}/ble-profiles/sources/profiles/hrps/hrps_main.c
    ${CORDIO_ROOT}/ble-profiles/sources/profiles/htpc/htpc_main.c
    ${CORDIO_ROOT}/ble-profiles/sources/profiles/htps/htps_main.c
    ${CORDIO_ROOT}/ble-profiles/sources/profiles/paspc/paspc_main.c
    ${CORDIO_ROOT}/ble-profiles/sources/profiles/plxpc/plxpc_main.c
    ${CORDIO_ROOT}/ble-profiles/sources/profiles/plxps/plxps_db.c
    ${CORDIO_ROOT}/ble-profiles/sources/profiles/plxps/plxps_main.c
    ${CORDIO_ROOT}/ble-profiles/sources/profiles/rscp/rscps_main.c
    ${CORDIO_ROOT}/ble-profiles/sources/profiles/scpps/scpps_main.c
    ${CORDIO_ROOT}/ble-profiles/sources/profiles/sensor/gyro_main.c
    ${CORDIO_ROOT}/ble-profiles/sources/profiles/sensor/temp_main.c
    ${CORDIO_ROOT}/ble-profiles/sources/profiles/tipc/tipc_main.c
    ${CORDIO_ROOT}/ble-profiles/sources/profiles/udsc/udsc_main.c
    ${CORDIO_ROOT}/ble-profiles/sources/profiles/uribeacon/uricfg_main.c
    ${CORDIO_ROOT}/ble-profiles/sources/profiles/wdxc/wdxc_dc.c
    ${CORDIO_ROOT}/ble-profiles/sources/profiles/wdxc/wdxc_ft.c
    ${CORDIO_ROOT}/ble-profiles/sources/profiles/wdxc/wdxc_main.c
    ${CORDIO_ROOT}/ble-profiles/sources/profiles/wdxc/wdxc_stream.c
    ${CORDIO_ROOT}/ble-profiles/sources/profiles/wdxs/wdxs_au.c
    ${CORDIO_ROOT}/ble-profiles/sources/profiles/wdxs/wdxs_dc.c
    ${CORDIO_ROOT}/ble-profiles/sources/profiles/wdxs/wdxs_ft.c
    ${CORDIO_ROOT}/ble-profiles/sources/profiles/wdxs/wdxs_main.c
    ${CORDIO_ROOT}/ble-profiles/sources/profiles/wdxs/wdxs_phy.c
    ${CORDIO_ROOT}/ble-profiles/sources/profiles/wdxs/wdxs_stream.c
    ${CORDIO_ROOT}/ble-profiles/sources/profiles/wpc/wpc_main.c
    ${CORDIO_ROOT}/ble-profiles/sources/profiles/wspc/wspc_main.c
    ${CORDIO_ROOT}/ble-profiles/sources/profiles/wsps/wsps_main.c
    ${CORDIO_ROOT}/ble-profiles/sources/services/svc_alert.c
    ${CORDIO_ROOT}/ble-profiles/sources/services/svc_batt.c
    ${CORDIO_ROOT}/ble-profiles/sources/services/svc_bps.c
    ${CORDIO_ROOT}/ble-profiles/sources/services/svc_core.c
    ${CORDIO_ROOT}/ble-profiles/sources/services/svc_cps.c
    ${CORDIO_ROOT}/ble-profiles/sources/services/svc_cscs.c
    ${CORDIO_ROOT}/ble-profiles/sources/services/svc_cte.c
    ${CORDIO_ROOT}/ble-profiles/sources/services/svc_dis.c
    ${CORDIO_ROOT}/ble-profiles/sources/services/svc_gls.c
    ${CORDIO_ROOT}/ble-profiles/sources/services/svc_gyro.c
    ${CORDIO_ROOT}/ble-profiles/sources/services/svc_hid.c
    ${CORDIO_ROOT}/ble-profiles/sources/services/svc_hrs.c
    ${CORDIO_ROOT}/ble-profiles/sources/services/svc_hts.c
    ${CORDIO_ROOT}/ble-profiles/sources/services/svc_ipss.c
    ${CORDIO_ROOT}/ble-profiles/sources/services/svc_plxs.c
    ${CORDIO_ROOT}/ble-profiles/sources/services/svc_px.c
    ${CORDIO_ROOT}/ble-profiles/sources/services/svc_rscs.c
    ${CORDIO_ROOT}/ble-profiles/sources/services/svc_scpss.c
    ${CORDIO_ROOT}/ble-profiles/sources/services/svc_temp.c
    ${CORDIO_ROOT}/ble-profiles/sources/services/svc_time.c
    ${CORDIO_ROOT}/ble-profiles/sources/services/svc_uricfg.c
    ${CORDIO_ROOT}/ble-profiles/sources/services/svc_wdxs.c
    ${CORDIO_ROOT}/ble-profiles/sources/services/svc_wp.c
    ${CORDIO_ROOT}/ble-profiles/sources/services/svc_wss.c

    # ble-profiles/sources/af
    ${CORDIO_ROOT}/ble-profiles/sources/af/app_disc.c
    ${CORDIO_ROOT}/ble-profiles/sources/af/app_main.c
    ${CORDIO_ROOT}/ble-profiles/sources/af/app_master_ae.c
    ${CORDIO_ROOT}/ble-profiles/sources/af/app_master.c
    ${CORDIO_ROOT}/ble-profiles/sources/af/app_master_leg.c
    ${CORDIO_ROOT}/ble-profiles/sources/af/app_server.c
    ${CORDIO_ROOT}/ble-profiles/sources/af/app_slave_ae.c
    ${CORDIO_ROOT}/ble-profiles/sources/af/app_slave.c
    ${CORDIO_ROOT}/ble-profiles/sources/af/app_slave_leg.c
    ${CORDIO_ROOT}/ble-profiles/sources/af/app_terminal.c
    ${CORDIO_ROOT}/ble-profiles/sources/af/common/app_db.c
    ${CORDIO_ROOT}/ble-profiles/sources/af/common/app_hw.c
    ${CORDIO_ROOT}/ble-profiles/sources/af/common/app_ui.c
    ${CORDIO_ROOT}/ble-profiles/sources/af/common/ui_console.c
    ${CORDIO_ROOT}/ble-profiles/sources/af/common/ui_lcd.c
    ${CORDIO_ROOT}/ble-profiles/sources/af/common/ui_main.c
    ${CORDIO_ROOT}/ble-profiles/sources/af/common/ui_platform.c
    ${CORDIO_ROOT}/ble-profiles/sources/af/common/ui_timer.c

    # ble-host/sources/hci/common
    ${CORDIO_ROOT}/ble-host/sources/hci/common/hci_core.c
    ${CORDIO_ROOT}/ble-host/sources/hci/common/hci_tr.c

    # ble-host/sources/hci/exactle
    ${CORDIO_ROOT}/ble-host/sources/hci/exactle/hci_cmd_ae.c
    ${CORDIO_ROOT}/ble-host/sources/hci/exactle/hci_cmd_bis.c
    ${CORDIO_ROOT}/ble-host/sources/hci/exactle/hci_cmd.c
    ${CORDIO_ROOT}/ble-host/sources/hci/exactle/hci_cmd_cis.c
    ${CORDIO_ROOT}/ble-host/sources/hci/exactle/hci_cmd_cte.c
    ${CORDIO_ROOT}/ble-host/sources/hci/exactle/hci_cmd_enc.c
    ${CORDIO_ROOT}/ble-host/sources/hci/exactle/hci_cmd_iso.c
    ${CORDIO_ROOT}/ble-host/sources/hci/exactle/hci_cmd_master_ae.c
    ${CORDIO_ROOT}/ble-host/sources/hci/exactle/hci_cmd_master.c
    ${CORDIO_ROOT}/ble-host/sources/hci/exactle/hci_cmd_past.c
    ${CORDIO_ROOT}/ble-host/sources/hci/exactle/hci_cmd_phy.c
    ${CORDIO_ROOT}/ble-host/sources/hci/exactle/hci_core_ps.c
    ${CORDIO_ROOT}/ble-host/sources/hci/exactle/hci_evt.c
    ${CORDIO_ROOT}/ble-host/sources/hci/exactle/hci_vs_ae.c
    ${CORDIO_ROOT}/ble-host/sources/hci/exactle/hci_vs.c

    # ble-host/sources/sec/common (excluding sec_ecc_debug.c as it's already removed)
    ${CORDIO_ROOT}/ble-host/sources/sec/common/sec_aes.c
    ${CORDIO_ROOT}/ble-host/sources/sec/common/sec_aes_rev.c
    ${CORDIO_ROOT}/ble-host/sources/sec/common/sec_ccm_hci.c
    ${CORDIO_ROOT}/ble-host/sources/sec/common/sec_cmac_hci.c
    ${CORDIO_ROOT}/ble-host/sources/sec/common/sec_ecc_hci.c
    ${CORDIO_ROOT}/ble-host/sources/sec/common/sec_main.c

    # ble-host/sources/stack/att
    ${CORDIO_ROOT}/ble-host/sources/stack/att/attc_disc.c
    ${CORDIO_ROOT}/ble-host/sources/stack/att/attc_eatt.c
    ${CORDIO_ROOT}/ble-host/sources/stack/att/attc_main.c
    ${CORDIO_ROOT}/ble-host/sources/stack/att/attc_proc.c
    ${CORDIO_ROOT}/ble-host/sources/stack/att/attc_read.c
    ${CORDIO_ROOT}/ble-host/sources/stack/att/attc_sign.c
    ${CORDIO_ROOT}/ble-host/sources/stack/att/attc_write.c
    ${CORDIO_ROOT}/ble-host/sources/stack/att/att_eatt.c
    ${CORDIO_ROOT}/ble-host/sources/stack/att/att_main.c
    ${CORDIO_ROOT}/ble-host/sources/stack/att/atts_ccc.c
    ${CORDIO_ROOT}/ble-host/sources/stack/att/atts_csf.c
    ${CORDIO_ROOT}/ble-host/sources/stack/att/atts_dyn.c
    ${CORDIO_ROOT}/ble-host/sources/stack/att/atts_eatt.c
    ${CORDIO_ROOT}/ble-host/sources/stack/att/atts_ind.c
    ${CORDIO_ROOT}/ble-host/sources/stack/att/atts_main.c
    ${CORDIO_ROOT}/ble-host/sources/stack/att/atts_proc.c
    ${CORDIO_ROOT}/ble-host/sources/stack/att/atts_read.c
    ${CORDIO_ROOT}/ble-host/sources/stack/att/atts_sign.c
    ${CORDIO_ROOT}/ble-host/sources/stack/att/atts_write.c
    ${CORDIO_ROOT}/ble-host/sources/stack/att/att_uuid.c

    # ble-host/sources/stack/cfg
    ${CORDIO_ROOT}/ble-host/sources/stack/cfg/cfg_stack.c

    # ble-host/sources/stack/dm
    ${CORDIO_ROOT}/ble-host/sources/stack/dm/dm_adv_ae.c
    ${CORDIO_ROOT}/ble-host/sources/stack/dm/dm_adv.c
    ${CORDIO_ROOT}/ble-host/sources/stack/dm/dm_adv_leg.c
    ${CORDIO_ROOT}/ble-host/sources/stack/dm/dm_bis_master.c
    ${CORDIO_ROOT}/ble-host/sources/stack/dm/dm_bis_slave.c
    ${CORDIO_ROOT}/ble-host/sources/stack/dm/dm_cis.c
    ${CORDIO_ROOT}/ble-host/sources/stack/dm/dm_cis_master.c
    ${CORDIO_ROOT}/ble-host/sources/stack/dm/dm_cis_slave.c
    ${CORDIO_ROOT}/ble-host/sources/stack/dm/dm_cis_sm.c
    ${CORDIO_ROOT}/ble-host/sources/stack/dm/dm_conn.c
    ${CORDIO_ROOT}/ble-host/sources/stack/dm/dm_conn_cte.c
    ${CORDIO_ROOT}/ble-host/sources/stack/dm/dm_conn_master_ae.c
    ${CORDIO_ROOT}/ble-host/sources/stack/dm/dm_conn_master.c
    ${CORDIO_ROOT}/ble-host/sources/stack/dm/dm_conn_master_leg.c
    ${CORDIO_ROOT}/ble-host/sources/stack/dm/dm_conn_slave_ae.c
    ${CORDIO_ROOT}/ble-host/sources/stack/dm/dm_conn_slave.c
    ${CORDIO_ROOT}/ble-host/sources/stack/dm/dm_conn_slave_leg.c
    ${CORDIO_ROOT}/ble-host/sources/stack/dm/dm_conn_sm.c
    ${CORDIO_ROOT}/ble-host/sources/stack/dm/dm_dev.c
    ${CORDIO_ROOT}/ble-host/sources/stack/dm/dm_dev_priv.c
    ${CORDIO_ROOT}/ble-host/sources/stack/dm/dm_iso.c
    ${CORDIO_ROOT}/ble-host/sources/stack/dm/dm_main.c
    ${CORDIO_ROOT}/ble-host/sources/stack/dm/dm_past.c
    ${CORDIO_ROOT}/ble-host/sources/stack/dm/dm_phy.c
    ${CORDIO_ROOT}/ble-host/sources/stack/dm/dm_priv.c
    ${CORDIO_ROOT}/ble-host/sources/stack/dm/dm_scan_ae.c
    ${CORDIO_ROOT}/ble-host/sources/stack/dm/dm_scan.c
    ${CORDIO_ROOT}/ble-host/sources/stack/dm/dm_scan_leg.c
    ${CORDIO_ROOT}/ble-host/sources/stack/dm/dm_sec.c
    ${CORDIO_ROOT}/ble-host/sources/stack/dm/dm_sec_lesc.c
    ${CORDIO_ROOT}/ble-host/sources/stack/dm/dm_sec_master.c
    ${CORDIO_ROOT}/ble-host/sources/stack/dm/dm_sec_slave.c
    ${CORDIO_ROOT}/ble-host/sources/stack/dm/dm_sync_ae.c

    # ble-host/sources/stack/hci
    ${CORDIO_ROOT}/ble-host/sources/stack/hci/hci_main.c

    # ble-host/sources/stack/l2c
    ${CORDIO_ROOT}/ble-host/sources/stack/l2c/l2c_coc.c
    ${CORDIO_ROOT}/ble-host/sources/stack/l2c/l2c_main.c
    ${CORDIO_ROOT}/ble-host/sources/stack/l2c/l2c_master.c
    ${CORDIO_ROOT}/ble-host/sources/stack/l2c/l2c_slave.c

    # ble-host/sources/stack/smp
    ${CORDIO_ROOT}/ble-host/sources/stack/smp/smp_act.c
    ${CORDIO_ROOT}/ble-host/sources/stack/smp/smp_db.c
    ${CORDIO_ROOT}/ble-host/sources/stack/smp/smpi_act.c
    ${CORDIO_ROOT}/ble-host/sources/stack/smp/smpi_sc_act.c
    ${CORDIO_ROOT}/ble-host/sources/stack/smp/smpi_sc_sm.c
    ${CORDIO_ROOT}/ble-host/sources/stack/smp/smpi_sm.c
    ${CORDIO_ROOT}/ble-host/sources/stack/smp/smp_main.c
    ${CORDIO_ROOT}/ble-host/sources/stack/smp/smp_non.c
    ${CORDIO_ROOT}/ble-host/sources/stack/smp/smpr_act.c
    ${CORDIO_ROOT}/ble-host/sources/stack/smp/smpr_sc_act.c
    ${CORDIO_ROOT}/ble-host/sources/stack/smp/smpr_sc_sm.c
    ${CORDIO_ROOT}/ble-host/sources/stack/smp/smpr_sm.c
    ${CORDIO_ROOT}/ble-host/sources/stack/smp/smp_sc_act.c
    ${CORDIO_ROOT}/ble-host/sources/stack/smp/smp_sc_main.c

    # controller/sources/ble
    ${CORDIO_ROOT}/controller/sources/ble/bb/bb_ble_adv_master_ae.c
    ${CORDIO_ROOT}/controller/sources/ble/bb/bb_ble_adv_master.c
    ${CORDIO_ROOT}/controller/sources/ble/bb/bb_ble_adv_slave_ae.c
    ${CORDIO_ROOT}/controller/sources/ble/bb/bb_ble_adv_slave.c
    ${CORDIO_ROOT}/controller/sources/ble/bb/bb_ble_bis_master.c
    ${CORDIO_ROOT}/controller/sources/ble/bb/bb_ble_bis_slave.c
    ${CORDIO_ROOT}/controller/sources/ble/bb/bb_ble_cis.c
    ${CORDIO_ROOT}/controller/sources/ble/bb/bb_ble_cis_master.c
    ${CORDIO_ROOT}/controller/sources/ble/bb/bb_ble_cis_slave.c
    ${CORDIO_ROOT}/controller/sources/ble/bb/bb_ble_conn.c
    ${CORDIO_ROOT}/controller/sources/ble/bb/bb_ble_conn_master.c
    ${CORDIO_ROOT}/controller/sources/ble/bb/bb_ble_conn_slave.c
    ${CORDIO_ROOT}/controller/sources/ble/bb/bb_ble_dtm.c
    ${CORDIO_ROOT}/controller/sources/ble/bb/bb_ble_main.c
    ${CORDIO_ROOT}/controller/sources/ble/bb/bb_ble_pdufilt.c
    ${CORDIO_ROOT}/controller/sources/ble/bb/bb_ble_periodiclist.c
    ${CORDIO_ROOT}/controller/sources/ble/bb/bb_ble_reslist.c
    ${CORDIO_ROOT}/controller/sources/ble/bb/bb_ble_sniffer.c
    ${CORDIO_ROOT}/controller/sources/ble/bb/bb_ble_whitelist.c
    ${CORDIO_ROOT}/controller/sources/ble/init/init.c
    ${CORDIO_ROOT}/controller/sources/ble/init/init_ctr.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_act_adv_master_ae.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_act_adv_master.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_act_adv_slave_ae.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_act_adv_slave.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_act_bis_master.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_act_bis_slave.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_act_cis.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_act_cis_master.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_act_cis_slave.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_act_conn.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_act_conn_master_ae.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_act_conn_master.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_act_conn_past.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_act_enc.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_act_enc_master.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_act_init_master_ae.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_act_init_master.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_act_pc.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_act_phy.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_isr_adv_master_ae.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_isr_adv_master.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_isr_adv_slave_ae.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_isr_adv_slave.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_isr_bis_master.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_isr_bis_slave.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_isr_cis.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_isr_cis_master.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_isr_cis_slave.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_isr_conn.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_isr_conn_master.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_isr_conn_slave.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_isr_init_master_ae.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_isr_init_master.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_main_adv_master_ae.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_main_adv_master.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_main_adv_slave_ae.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_main_adv_slave.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_main_bis.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_main_bis_master.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_main_bis_slave.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_main.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_main_cis.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_main_cis_master.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_main_cis_slave.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_main_conn.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_main_conn_cs2.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_main_conn_data.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_main_conn_master.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_main_conn_slave.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_main_enc_master.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_main_enc_slave.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_main_init_master_ae.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_main_init_master.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_main_iso.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_main_iso_data.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_main_master_phy.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_main_past.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_main_pc.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_main_priv.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_main_sc.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_main_slave_phy.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_pdu_adv_master_ae.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_pdu_adv_slave_ae.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_pdu_adv_slave.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_pdu_bis.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_pdu_conn.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_pdu_enc.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_pdu_iso.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_sm_adv_master_ae.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_sm_adv_master.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_sm_adv_slave_ae.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_sm_adv_slave.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_sm_bis_master.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_sm_bis_slave.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_sm_cis.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_sm_conn_master.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_sm_conn_slave.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_sm_init_master_ae.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_sm_init_master.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_sm_llcp_cis.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_sm_llcp_cis_master.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_sm_llcp_cis_slave.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_sm_llcp_conn.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_sm_llcp_conn_master.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_sm_llcp_conn_slave.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_sm_llcp_enc_master.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_sm_llcp_enc_slave.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_sm_llcp_master_phy.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_sm_llcp_pc.c
    ${CORDIO_ROOT}/controller/sources/ble/lctr/lctr_sm_llcp_slave_phy.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_cmd_adv_master_ae.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_cmd_adv_master.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_cmd_adv_priv.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_cmd_adv_slave_ae.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_cmd_adv_slave.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_cmd_bis_master.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_cmd_bis_slave.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_cmd.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_cmd_cis_master.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_cmd_cis_slave.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_cmd_conn.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_cmd_conn_master_ae.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_cmd_conn_master.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_cmd_conn_priv.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_cmd_enc_master.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_cmd_enc_slave.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_cmd_iso.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_cmd_past.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_cmd_pc.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_cmd_phy.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_cmd_sc.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_cmd_vs_adv_master_ae.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_cmd_vs_adv_master.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_cmd_vs_adv_slave_ae.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_cmd_vs_adv_slave.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_cmd_vs.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_cmd_vs_conn.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_cmd_vs_conn_master.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_cmd_vs_enc_slave.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_cmd_vs_iso.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_cmd_vs_sc.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_evt_adv_master_ae.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_evt_adv_master.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_evt_adv_slave_ae.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_evt_adv_slave.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_evt_bis_master.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_evt_bis_slave.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_evt.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_evt_cis_master.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_evt_cis_slave.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_evt_conn.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_evt_conn_cs2.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_evt_conn_master.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_evt_conn_priv.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_evt_enc_master.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_evt_enc_slave.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_evt_iso.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_evt_pc.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_evt_phy.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_evt_sc.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_evt_vs.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_init_adv_master_ae.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_init_adv_master.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_init_adv_priv.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_init_adv_slave_ae.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_init_adv_slave.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_init_bis_master.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_init_bis_slave.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_init.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_init_cis_master.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_init_cis_slave.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_init_conn.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_init_conn_cs2.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_init_conn_master_ae.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_init_conn_master.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_init_conn_priv.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_init_enc_master.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_init_enc_slave.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_init_iso.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_init_past.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_init_pc.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_init_phy.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_init_sc.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_main.c
    ${CORDIO_ROOT}/controller/sources/ble/lhci/lhci_main_iso.c
    ${CORDIO_ROOT}/controller/sources/ble/ll/ll_init_adv_master_ae.c
    ${CORDIO_ROOT}/controller/sources/ble/ll/ll_init_adv_master.c
    ${CORDIO_ROOT}/controller/sources/ble/ll/ll_init_adv_slave_ae.c
    ${CORDIO_ROOT}/controller/sources/ble/ll/ll_init_adv_slave.c
    ${CORDIO_ROOT}/controller/sources/ble/ll/ll_init_bis_master.c
    ${CORDIO_ROOT}/controller/sources/ble/ll/ll_init_bis_slave.c
    ${CORDIO_ROOT}/controller/sources/ble/ll/ll_init.c
    ${CORDIO_ROOT}/controller/sources/ble/ll/ll_init_cis_master.c
    ${CORDIO_ROOT}/controller/sources/ble/ll/ll_init_cis_slave.c
    ${CORDIO_ROOT}/controller/sources/ble/ll/ll_init_conn_cs2.c
    ${CORDIO_ROOT}/controller/sources/ble/ll/ll_init_conn_master.c
    ${CORDIO_ROOT}/controller/sources/ble/ll/ll_init_conn_slave.c
    ${CORDIO_ROOT}/controller/sources/ble/ll/ll_init_enc_master.c
    ${CORDIO_ROOT}/controller/sources/ble/ll/ll_init_enc_slave.c
    ${CORDIO_ROOT}/controller/sources/ble/ll/ll_init_init_master_ae.c
    ${CORDIO_ROOT}/controller/sources/ble/ll/ll_init_init_master.c
    ${CORDIO_ROOT}/controller/sources/ble/ll/ll_init_iso.c
    ${CORDIO_ROOT}/controller/sources/ble/ll/ll_init_master_phy.c
    ${CORDIO_ROOT}/controller/sources/ble/ll/ll_init_past.c
    ${CORDIO_ROOT}/controller/sources/ble/ll/ll_init_pc.c
    ${CORDIO_ROOT}/controller/sources/ble/ll/ll_init_priv.c
    ${CORDIO_ROOT}/controller/sources/ble/ll/ll_init_sc.c
    ${CORDIO_ROOT}/controller/sources/ble/ll/ll_init_slave_phy.c
    ${CORDIO_ROOT}/controller/sources/ble/ll/ll_main_adv_master_ae.c
    ${CORDIO_ROOT}/controller/sources/ble/ll/ll_main_adv_master.c
    ${CORDIO_ROOT}/controller/sources/ble/ll/ll_main_adv_slave_ae.c
    ${CORDIO_ROOT}/controller/sources/ble/ll/ll_main_adv_slave.c
    ${CORDIO_ROOT}/controller/sources/ble/ll/ll_main_bis_master.c
    ${CORDIO_ROOT}/controller/sources/ble/ll/ll_main_bis_slave.c
    ${CORDIO_ROOT}/controller/sources/ble/ll/ll_main.c
    ${CORDIO_ROOT}/controller/sources/ble/ll/ll_main_cis_master.c
    ${CORDIO_ROOT}/controller/sources/ble/ll/ll_main_cis_slave.c
    ${CORDIO_ROOT}/controller/sources/ble/ll/ll_main_conn.c
    ${CORDIO_ROOT}/controller/sources/ble/ll/ll_main_conn_master_ae.c
    ${CORDIO_ROOT}/controller/sources/ble/ll/ll_main_conn_master.c
    ${CORDIO_ROOT}/controller/sources/ble/ll/ll_main_conn_slave.c
    ${CORDIO_ROOT}/controller/sources/ble/ll/ll_main_diag.c
    ${CORDIO_ROOT}/controller/sources/ble/ll/ll_main_dtm.c
    ${CORDIO_ROOT}/controller/sources/ble/ll/ll_main_enc_master.c
    ${CORDIO_ROOT}/controller/sources/ble/ll/ll_main_enc_slave.c
    ${CORDIO_ROOT}/controller/sources/ble/ll/ll_main_iso.c
    ${CORDIO_ROOT}/controller/sources/ble/ll/ll_main_past.c
    ${CORDIO_ROOT}/controller/sources/ble/ll/ll_main_pc.c
    ${CORDIO_ROOT}/controller/sources/ble/ll/ll_main_phy.c
    ${CORDIO_ROOT}/controller/sources/ble/ll/ll_main_priv.c
    ${CORDIO_ROOT}/controller/sources/ble/ll/ll_main_sc.c
    ${CORDIO_ROOT}/controller/sources/ble/ll/ll_math.c
    ${CORDIO_ROOT}/controller/sources/ble/lmgr/lmgr_events.c
    ${CORDIO_ROOT}/controller/sources/ble/lmgr/lmgr_main_adv_master_ae.c
    ${CORDIO_ROOT}/controller/sources/ble/lmgr/lmgr_main_adv_slave_ae.c
    ${CORDIO_ROOT}/controller/sources/ble/lmgr/lmgr_main.c
    ${CORDIO_ROOT}/controller/sources/ble/lmgr/lmgr_main_cis_master.c
    ${CORDIO_ROOT}/controller/sources/ble/lmgr/lmgr_main_conn.c
    ${CORDIO_ROOT}/controller/sources/ble/lmgr/lmgr_main_iso.c
    ${CORDIO_ROOT}/controller/sources/ble/lmgr/lmgr_main_master.c
    ${CORDIO_ROOT}/controller/sources/ble/lmgr/lmgr_main_priv.c
    ${CORDIO_ROOT}/controller/sources/ble/lmgr/lmgr_main_sc.c
    ${CORDIO_ROOT}/controller/sources/ble/lmgr/lmgr_main_slave.c
    ${CORDIO_ROOT}/controller/sources/ble/sch/sch_ble.c
    ${CORDIO_ROOT}/controller/sources/ble/sch/sch_rm.c
    ${CORDIO_ROOT}/controller/sources/ble/sch/sch_tm.c

    # controller/sources/common
    ${CORDIO_ROOT}/controller/sources/common/bb/bb_main.c
    ${CORDIO_ROOT}/controller/sources/common/chci/chci_tr.c
    ${CORDIO_ROOT}/controller/sources/common/sch/sch_list.c
    ${CORDIO_ROOT}/controller/sources/common/sch/sch_main.c

    # wsf/sources/util
    ${CORDIO_ROOT}/wsf/sources/util/bda.c
    ${CORDIO_ROOT}/wsf/sources/util/bstream.c
    ${CORDIO_ROOT}/wsf/sources/util/calc128.c
    ${CORDIO_ROOT}/wsf/sources/util/crc32.c
    ${CORDIO_ROOT}/wsf/sources/util/fcs.c
    ${CORDIO_ROOT}/wsf/sources/util/prand.c
    ${CORDIO_ROOT}/wsf/sources/util/print.c
    ${CORDIO_ROOT}/wsf/sources/util/terminal.c
    ${CORDIO_ROOT}/wsf/sources/util/wstr.c

    # thirdparty/uecc
    ${CORDIO_ROOT}/thirdparty/uecc/uECC.c
    ${CORDIO_ROOT}/thirdparty/uecc/uECC_ll.c
)

# Add WSF target sources conditionally based on CONFIG_FREERTOS
if(CONFIG_FREERTOS)
    list(APPEND CORDIO_SOURCES
        ${CORDIO_ROOT}/wsf/sources/targets/freertos/wsf_assert.c
        ${CORDIO_ROOT}/wsf/sources/targets/freertos/wsf_buf.c
        ${CORDIO_ROOT}/wsf/sources/targets/freertos/wsf_bufio.c
        ${CORDIO_ROOT}/wsf/sources/targets/freertos/wsf_cs.c
        ${CORDIO_ROOT}/wsf/sources/targets/freertos/wsf_detoken.c
        ${CORDIO_ROOT}/wsf/sources/targets/freertos/wsf_efs.c
        ${CORDIO_ROOT}/wsf/sources/targets/freertos/wsf_heap.c
        ${CORDIO_ROOT}/wsf/sources/targets/freertos/wsf_msg.c
        ${CORDIO_ROOT}/wsf/sources/targets/freertos/wsf_nvm.c
        ${CORDIO_ROOT}/wsf/sources/targets/freertos/wsf_os.c
        ${CORDIO_ROOT}/wsf/sources/targets/freertos/wsf_queue.c
        ${CORDIO_ROOT}/wsf/sources/targets/freertos/wsf_timer.c
        ${CORDIO_ROOT}/wsf/sources/targets/freertos/wsf_trace.c
    )
else()
    list(APPEND CORDIO_SOURCES
        ${CORDIO_ROOT}/wsf/sources/targets/baremetal/wsf_assert.c
        ${CORDIO_ROOT}/wsf/sources/targets/baremetal/wsf_buf.c
        ${CORDIO_ROOT}/wsf/sources/targets/baremetal/wsf_bufio.c
        ${CORDIO_ROOT}/wsf/sources/targets/baremetal/wsf_cs.c
        ${CORDIO_ROOT}/wsf/sources/targets/baremetal/wsf_detoken.c
        ${CORDIO_ROOT}/wsf/sources/targets/baremetal/wsf_efs.c
        ${CORDIO_ROOT}/wsf/sources/targets/baremetal/wsf_heap.c
        ${CORDIO_ROOT}/wsf/sources/targets/baremetal/wsf_msg.c
        ${CORDIO_ROOT}/wsf/sources/targets/baremetal/wsf_nvm.c
        ${CORDIO_ROOT}/wsf/sources/targets/baremetal/wsf_os.c
        ${CORDIO_ROOT}/wsf/sources/targets/baremetal/wsf_queue.c
        ${CORDIO_ROOT}/wsf/sources/targets/baremetal/wsf_timer.c
        ${CORDIO_ROOT}/wsf/sources/targets/baremetal/wsf_trace.c
    )
endif()

# Add specific platform sources
if(EXISTS ${CORDIO_ROOT}/platform/targets/maxim/${TARGET}/sources/pal_crypto.c)
    list(APPEND CORDIO_SOURCES ${CORDIO_ROOT}/platform/targets/maxim/${TARGET}/sources/pal_crypto.c)
endif()
if(EXISTS ${CORDIO_ROOT}/platform/targets/maxim/${TARGET}/sources/pal_timer.c)
    list(APPEND CORDIO_SOURCES ${CORDIO_ROOT}/platform/targets/maxim/${TARGET}/sources/pal_timer.c)
endif()
if(EXISTS ${CORDIO_ROOT}/platform/targets/maxim/${TARGET}/sources/pal_rtc.c)
    list(APPEND CORDIO_SOURCES ${CORDIO_ROOT}/platform/targets/maxim/${TARGET}/sources/pal_rtc.c)
endif()
if(EXISTS ${CORDIO_ROOT}/platform/targets/maxim/${TARGET}/sources/pal_cfg.c)
    list(APPEND CORDIO_SOURCES ${CORDIO_ROOT}/platform/targets/maxim/${TARGET}/sources/pal_cfg.c)
endif()

# Remove ignored files
list(REMOVE_ITEM CORDIO_SOURCES ${CORDIO_ROOT}/ble-host/sources/sec/common/sec_ecc_debug.c)

# Add platform-dependent sources from libraries/cordio/src
set(CORDIO_PLATFORM_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cordio_app/cordio_init.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cordio_app/periph_main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cordio_app/stack_periph.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cordio_app/stack_dats.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cordio_app/dats_main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cordio_app/fit_main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cordio_app/stack_fit.c
    # ${CMAKE_CURRENT_SOURCE_DIR}/src/cordio_pal/pal_rtc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cordio_pal/pal_sys.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cordio_pal/pal_codec.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cordio_pal/pal_uart.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cordio_pal/pal_flash.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cordio_pal/pal_btn.c
)
list(APPEND CORDIO_SOURCES ${CORDIO_PLATFORM_SOURCES})

# Add sources to cordio library
if(CORDIO_SOURCES)
    target_sources(no-os PRIVATE ${CORDIO_SOURCES})
endif()

# Add compile definitions
target_compile_definitions(no-os PUBLIC
    BT_ENABLED=1
    CHCI_TR_UART=1
    HCI_TR_EXACTLE=1
    BT_VER=11
    uECC_ASM=2
    PAL_TMR_IDX=4
    PAL_SLEEP_TMR_IDX=5
    INIT_PERIPHERAL=1
    INIT_CENTRAL=0
    INIT_BROADCASTER=0
    INIT_OBSERVER=0
    WSF_TRACE_ENABLED=TRUE
    WSF_TRACE_ENABLED_VERBOSE=0
)

# Add compiler flags
target_compile_options(no-os PRIVATE -fomit-frame-pointer -fno-common)

target_link_libraries(no-os ${MAXIM_LIBRARIES}/BlePhy/MAX${TARGET_NUM}/libphy.a)
target_link_libraries(no-os m)

# Link cordio library to no-os target
# target_link_libraries(no-os cordio)