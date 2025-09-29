#include <stdlib.h>
#include <stdio.h>
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"
#include "no_os_uart.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"

#include "platform/maxim/maxim_platform.h"

#if defined(NO_OS_USB_UART)
#include "maxim_usb_uart.h"
#endif

#if defined(CONFIG_CORDIO)
#include "cordio_uart.h"
#include "cordio_init.h"
#include "svc_batt.h"
#include "svc_dis.h"
#include "svc_core.h"
#include "svc_wp.h"
#include "att_api.h"
#include "wsf_os.h"
#include "util/bstream.h"
#include "no_os_delay.h"
#include "dm_api.h"
extern int hci_cordio_init(void);
#endif

#define UART_DEVICE_ID  0
#define UART_BAUDRATE   115200
#define UART_IRQ_ID     UART0_IRQn

// BLE mode configuration - controlled by Kconfig (CONFIG_BLE_MODE_HCI_UART_ENABLED)

#if defined(CONFIG_CORDIO)

/* Battery level simulation */
static uint8_t batteryLevel = 85;

/* Sample sensor data */
static char sensorData[50] = "Temp:25.3C,Hum:45%,Pressure:1013hPa";

/* Custom service UUIDs */
#define CUSTOM_SVC_UUID              0x1234
#define CUSTOM_SENSOR_CH_UUID        0x2345
#define CUSTOM_CONTROL_CH_UUID       0x2346

/* Custom service data */
static uint8_t customSensorValue[20] = "Hello BLE World!";
static uint8_t customControlValue = 0;

/* Custom service handles */
#define CUSTOM_SVC_HDL               0x30
#define CUSTOM_SENSOR_CH_HDL         0x31
#define CUSTOM_SENSOR_VAL_HDL        0x32
#define CUSTOM_CONTROL_CH_HDL        0x33
#define CUSTOM_CONTROL_VAL_HDL       0x34

/*************************************************************************************************/
/*!
 *  \brief  Application ATTS write callback for WP service.
 */
/*************************************************************************************************/
static uint8_t ftcWpWriteCback(dmConnId_t connId, uint16_t handle, uint8_t operation,
                              uint16_t offset, uint16_t len, uint8_t *pValue, attsAttr_t *pAttr)
{
  printf("📱 Phone wrote to Wireless Profile: connId=%d, handle=0x%04X, len=%d\n", connId, handle, len);

  /* Echo the data back with timestamp */
  if (len > 0) {
    printf("Received from phone: ");
    for (int i = 0; i < len && i < 20; i++) {
      printf("%c", pValue[i]);
    }
    printf("\n");

    /* Update sensor data with dummy values */
    uint32_t seconds = (uint32_t)(no_os_get_time() / 1000);
    snprintf(sensorData, sizeof(sensorData), "Temp:%.1fC,Hum:%d%%,Time:%lu",
             20.0f + (seconds % 15), 40 + (seconds % 30), seconds);

    /* Send dummy sensor response back to phone */
    AttsSendNtf(connId, handle, strlen(sensorData), (uint8_t*)sensorData);
    printf("📱 Sent dummy sensor data to phone: %s\n", sensorData);
  }

  return ATT_SUCCESS;
}

/*************************************************************************************************/
/*!
 *  \brief  ATTS read callback for battery service.
 */
/*************************************************************************************************/
static uint8_t battReadCback(dmConnId_t connId, uint16_t handle, uint8_t operation,
                            uint16_t offset, attsAttr_t *pAttr)
{
  /* Update battery level with current timestamp for dummy data */
  uint32_t seconds = (uint32_t)(no_os_get_time() / 1000);
  batteryLevel = 50 + (seconds % 50); /* Battery level 50-99% based on time */

  printf("📱 Phone reading battery: %d%% (dummy data)\n", batteryLevel);

  /* Set the attribute value */
  pAttr->pValue = &batteryLevel;
  pAttr->len = 1;

  return ATT_SUCCESS;
}

#endif

#if 1
struct max_usb_uart_init_param usb_uart_extra_ip = {
	.vid = 0x0B6B,
	.pid = 0x003C
};

struct no_os_uart_init_param uart_init_param = {
	.device_id = UART_DEVICE_ID,
	.irq_id = USB_IRQn,
	.asynchronous_rx = true,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.platform_ops = &max_usb_uart_ops,
	.extra = &usb_uart_extra_ip,
};
#else
struct max_uart_init_param uart_extra_ip = {
	.flow = MXC_UART_FLOW_DIS,
};

struct no_os_uart_init_param uart_init_param = {
	.device_id = UART_DEVICE_ID,
	.irq_id = UART_IRQ_ID,
	.asynchronous_rx = true,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.platform_ops = &max_uart_ops,
	.extra = &uart_extra_ip,
};
#endif

/**
 * @brief First thread function
 * @param pvParameters - Thread parameters
 */
void bt_task(void *pvParameters)
{
#ifdef CONFIG_BLE_MODE_HCI_UART_ENABLED
	printf("Starting BLE in HCI-UART mode\n");
	hci_cordio_init();

	/* HCI service loop in FreeRTOS task */
	while(1) {
		/* Service the HCI transport - this needs to be declared */
		extern bool ChciTrService(void);
		ChciTrService();

		/* Yield to other FreeRTOS tasks */
		vTaskDelay(pdMS_TO_TICKS(1));

		/* Periodic status message */
		static int tick_count = 0;
		if (++tick_count >= 5000) {  // Every ~5 seconds
			printf("HCI-UART service running\n");
			tick_count = 0;
		}
	}
#else
	printf("Starting BLE in Peripheral mode\n");
	cordio_init();
	printf("Cordio initialization completed - GATT services should be available\n");

	printf("GATT services available:\n");
	printf("- Core GATT services (GAP/GATT)\n");
	printf("- Battery Service (0x180F) - read battery level\n");
	printf("- Device Information Service (0x180A)\n");
	printf("- Wireless Profile Service - read/write custom data\n");
	printf("You can connect with your phone and read dummy data!\n");

	/* Main BLE service loop */
	uint32_t tick_counter = 0;
	while(1){
		extern void wsfOsDispatcher(void);
		wsfOsDispatcher();

		/* Periodic updates */
		tick_counter++;
		if (tick_counter % 10000 == 0) {  /* Every ~10 seconds */
			printf("BLE Peripheral running, Tick: %lu\n", tick_counter);
			tick_counter = 0;  /* Reset to prevent overflow */
		}

		/* Small delay to yield to other tasks */
		vTaskDelay(pdMS_TO_TICKS(1));
	}
#endif
}

/**
 * @brief Second thread function
 * @param pvParameters - Thread parameters
 */
void thread2_task(void *pvParameters)
{
	while (1) {
		printf("Tick 2\n");
		no_os_mdelay(1000);
	}
}

/**
 * @brief Create FreeRTOS tasks
 * @return Returns error code
 */
int create_tasks(void)
{
	int ret;
	TaskHandle_t thread1_handle = NULL;
	TaskHandle_t thread2_handle = NULL;

	ret = xTaskCreate(bt_task, "BT task", configMINIMAL_STACK_SIZE,
			  NULL, tskIDLE_PRIORITY + 1, &thread1_handle);
	if (ret != pdPASS) {
		goto error_thread1;
	}

	// ret = xTaskCreate(thread2_task, "Thread2", configMINIMAL_STACK_SIZE,
	// 		  NULL, tskIDLE_PRIORITY + 2, &thread2_handle);
	// if (ret != pdPASS) {
	// 	goto error_thread2;
	// }

	vTaskStartScheduler();

error_thread2:
	if (thread2_handle != NULL)
		vTaskDelete(thread2_handle);
error_thread1:
	if (thread1_handle != NULL)
		vTaskDelete(thread1_handle);

	return -1;
}

int main()
{
	int ret;
	struct no_os_uart_desc *uart;

	ret = no_os_uart_init(&uart, &uart_init_param);
	if (ret)
		return ret;

	no_os_uart_stdio(uart);

	printf("FTC Workshop - FreeRTOS Demo\n");

	ret = create_tasks();

	printf("create_tasks() - %d\n", ret);

	no_os_uart_remove(uart);
	return ret;
}


/*************************************************************************************************/
/*!
 * \brief   How to Add Custom GATT Attributes - Example Template
 *
 * To add custom GATT attributes to your BLE peripheral, you have several options:
 *
 * 1. MODIFY EXISTING SERVICES (Easiest):
 *    - Modify the existing ftcWpWriteCback() and battReadCback() functions above
 *    - The Wireless Profile (WP) service is already available for custom data
 *    - The Battery service can be customized to report your own sensor data
 *
 * 2. ADD CUSTOM SERVICE TO PERIPH_MAIN.C (Recommended):
 *    - Edit /libraries/cordio/src/cordio_app/periph_main.c
 *    - Add your service in the PeriphStart() function after existing services
 *    - Use the same pattern as SvcWpAddGroup() but with your custom service
 *
 * 3. CREATE SEPARATE SERVICE FILE (Advanced):
 *    - Create a new file like svc_custom.c in the cordio library
 *    - Follow the pattern used by existing services (svc_batt.c, svc_dis.c, etc.)
 *    - Add your service to the CMakeLists.txt and call it from PeriphStart()
 *
 * EXAMPLE - Modify Wireless Profile service for custom data:
 *
 * In ftcWpWriteCback() above, you can:
 * - Receive custom commands from BLE clients
 * - Process sensor readings
 * - Control hardware peripherals
 * - Send notifications back to clients using AttsSendNtf()
 *
 * The WP service already provides read/write/notify capabilities!
 */