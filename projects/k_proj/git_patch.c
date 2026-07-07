diff --git a/projects/k_proj/src/examples/basic_example/basic_example.c b/projects/k_proj/src/examples/basic_example/basic_example.c
index 762bf257ec..ed2b88a9b5 100644
--- a/projects/k_proj/src/examples/basic_example/basic_example.c
+++ b/projects/k_proj/src/examples/basic_example/basic_example.c
@@ -129,6 +129,7 @@ int example_main()
 
 	printf("\nAD9081K (K) basic example - JESD204 link bring-up.\n");
 
+#if 0 /* Skip HW init — eth connectivity test only */
 	/*
 	 * ----------------------------------------------------------------
 	 * Deassert Si5391 RSTb (GPIO 133) — drive high to release reset.
@@ -471,7 +472,9 @@ int example_main()
 	no_os_mdelay(100);
 	axi_jesd204_rx_status_read(rx_jesd);
 	no_os_mdelay(100);
+#endif /* Skip HW init */
 
+#if 0 /* Skip DMA transfers — eth connectivity test only */
 	printf("Press Enter to start cmd transfer...\n");
 		getchar();
 
@@ -597,24 +600,14 @@ int example_main()
 		       (unsigned long)adc_buffer_dma[i]);
 #endif
 
+#endif /* Skip DMA transfers */
+
 	/*
 	 * ----------------------------------------------------------------
-	 * IIO application setup (UART transport).
+	 * IIO application setup (lwIP ethernet transport, no devices).
 	 * ----------------------------------------------------------------
 	 */
-	struct xil_uart_init_param platform_uart_init_par = {
-#ifdef XPAR_XUARTLITE_NUM_INSTANCES
-		.type = UART_PL,
-#else
-		.type = UART_PS,
-		.irq_id = UART_IRQ_ID
-#endif
-	};
-
 	struct iio_app_desc *app;
-
-	/* --- XEmacPs GEM init params --- */
-
 	static uint8_t mac_addr[6] = {0x00, 0x0A, 0x35, 0x00, 0x01, 0x02};
 
 	struct xemacps_init_param gem_ip = {
@@ -623,94 +616,23 @@ int example_main()
 	memcpy(gem_ip.hwaddr, mac_addr, sizeof(mac_addr));
 
 	struct iio_app_init_param app_init_param = { 0 };
-
-	struct iio_axi_adc_init_param iio_axi_adc_init_par;
-	struct iio_axi_dac_init_param iio_axi_dac_init_par;
-	struct iio_axi_adc_desc *iio_axi_adc_desc;
-	struct iio_axi_dac_desc *iio_axi_dac_desc;
-	struct iio_ad9081k_desc *iio_ad9081k_desc;
-	struct iio_device *adc_dev_desc, *dac_dev_desc, *ad9081k_dev_desc;
-
-	iio_axi_adc_init_par = (struct iio_axi_adc_init_param) {
-		.rx_adc = rx_adc,
-		.rx_dmac = rx_dmac,
-#ifndef PLATFORM_MB
-		.dcache_invalidate_range = (void (*)(uint32_t,
-						     uint32_t))Xil_DCacheInvalidateRange,
-#endif
-	};
-
-	ret = iio_axi_adc_init(&iio_axi_adc_desc, &iio_axi_adc_init_par);
-	if (ret) {
-		pr_err("iio_axi_adc_init() failed: %d\n", ret);
-		goto error_fsm;
-	}
-
-	struct iio_data_buffer read_buff = {
-		.buff = (void *)adc_buffer,
-		.size = sizeof(adc_buffer),
-	};
-	iio_axi_adc_get_dev_descriptor(iio_axi_adc_desc, &adc_dev_desc);
-
-	iio_axi_dac_init_par = (struct iio_axi_dac_init_param) {
-		.tx_dac = tx_dac,
-		.tx_dmac = tx_dmac,
-#ifndef PLATFORM_MB
-		.dcache_flush_range = (void (*)(uint32_t, uint32_t))Xil_DCacheFlushRange,
-#endif
-	};
-
-	ret = iio_axi_dac_init(&iio_axi_dac_desc, &iio_axi_dac_init_par);
-	if (ret) {
-		pr_err("iio_axi_dac_init() failed: %d\n", ret);
-		goto error_iio_adc;
-	}
-
-	struct iio_data_buffer write_buff = {
-		.buff = (void *)dac_buffer,
-		.size = sizeof(dac_buffer),
-	};
-	iio_axi_dac_get_dev_descriptor(iio_axi_dac_desc, &dac_dev_desc);
-
-	ret = iio_ad9081k_init(&iio_ad9081k_desc);
-	if (ret) {
-		pr_err("iio_ad9081k_init() failed: %d\n", ret);
-		goto error_iio_dac;
-	}
-	iio_ad9081k_get_dev_descriptor(iio_ad9081k_desc, &ad9081k_dev_desc);
-
-	struct iio_app_device iio_devices[] = {
-		IIO_APP_DEVICE("cf-ad9081k-lpc", iio_axi_adc_desc,
-			       adc_dev_desc, &read_buff, NULL, NULL),
-		IIO_APP_DEVICE("cf-ad9081k-dds-core-lpc", iio_axi_dac_desc,
-			       dac_dev_desc, NULL, &write_buff, NULL),
-		IIO_APP_DEVICE("ad9081k", iio_ad9081k_desc,
-			       ad9081k_dev_desc, NULL, NULL, NULL),
-	};
-
-	app_init_param.devices = iio_devices;
-	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
+	app_init_param.devices = NULL;
+	app_init_param.nb_devices = 0;
 	app_init_param.uart_init_params = xilinx_lwip_uart_ip;
 	app_init_param.lwip_param.platform_ops = &xemacps_lwip_ops;
 	app_init_param.lwip_param.mac_param    = &gem_ip;
 	memcpy(app_init_param.lwip_param.hwaddr, mac_addr, sizeof(mac_addr));
 
-	no_os_mdelay(100);
-
+	pr_info("Starting IIO app (eth only, no HW)...\n");
 	ret = iio_app_init(&app, app_init_param);
 	if (ret) {
 		pr_err("iio_app_init() failed: %d\n", ret);
-		goto error_iio_ad9081k;
+		return ret;
 	}
 
 	return iio_app_run(app);
 
-error_iio_ad9081k:
-	iio_ad9081k_remove(iio_ad9081k_desc);
-error_iio_dac:
-	iio_axi_dac_remove(iio_axi_dac_desc);
-error_iio_adc:
-	iio_axi_adc_remove(iio_axi_adc_desc);
+#if 0 /* cleanup labels — disabled with HW init */
 error_fsm:
 error_ad9081k:
 	ad9081k_remove(ad9081k_device);
@@ -736,6 +658,7 @@ error_tx_adxcvr:
 	adxcvr_remove(tx_adxcvr);
 error_si5391:
 	si5391_remove(si5391_device);
+#endif /* cleanup labels */
 
 	return ret;
 }
