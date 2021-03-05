#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parameters.h"
#include "ad7689.h"
#include "spi.h"
#include "gpio.h"
#include "print_log.h"
#include "stm32_gpio.h"
#include "stm32_spi.h"
#include "uart.h"
#include "stm32_uart.h"
#include "stm32_uart_stdio.h"

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
	/* USER CODE END Error_Handler_Debug */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Configure the main internal regulator output voltage
    */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 16;
    RCC_OscInitStruct.PLL.PLLN = 360;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 2;
    RCC_OscInitStruct.PLL.PLLR = 2;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
    /** Activate the Over-Drive mode
    */
    if (HAL_PWREx_EnableOverDrive() != HAL_OK)
    {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB buses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
    {
        Error_Handler();
    }
}

int test1(struct ad7689_dev *dev)
{
	return 0;
}

int test2(struct ad7689_dev *dev)
{
        return 0;
}

int test3(struct ad7689_dev *dev)
{
	return 0;
}

int main(void)
{
	int32_t ret;
	struct ad7689_init_param adcip;
	struct ad7689_dev *adc = NULL;

	struct uart_desc *uart;
	struct stm32_uart_init_param uip_extra = {
		.mode = UART_MODE_TX_RX,
		.hw_flow_ctl = UART_HWCONTROL_NONE,
		.over_sampling = UART_OVERSAMPLING_16,
	};
	struct uart_init_param uip = {
		.device_id = 2,
		.baud_rate = 115200,
		.size = UART_CS_8,
		.parity = UART_PAR_NO,
		.stop = UART_STOP_1,
		.extra = &uip_extra,
	};
	struct stm32_spi_init_param sip_extra = {
		.base = SPI_DEVICE_ID,
		.chip_select_port = SPI_CS_PORT,
		.get_input_clock = HAL_RCC_GetPCLK2Freq,
	};

	HAL_Init();
	SystemClock_Config();

	ret = uart_init(&uart, &uip);
	if (ret < 0)
		return ret;

	stm32_uart_stdio(uart);

	adcip.id = ID_AD7689;
	adcip.spi_init = (spi_init_param) {
		.max_speed_hz = 5000000,
		.mode = SPI_MODE_0,
		.chip_select = SPI_CS,
		.platform_ops = &stm32_platform_ops,
		.extra = &sip_extra,
	};
	adcip.config = (struct ad7689_config) {
		.incc = AD7689_UNIPOLAR_GND,
		.inx = 1,
		.bw = AD7689_BW_FULL,
		.ref = AD7689_REF_EXTERNAL,
		.seq = AD7689_SEQ_SCAN_ALL,
		.rb = false,
	};

	pr_info("Hello!\n");

	while(1) {
		char input[10];
		char number[10];
		pr_info("> ");
		scanf("%s", &input);

		if (!strcmp(input, "quit"))
			break;
		else if(!strcmp(input, "init")) {
			ret = ad7689_init(&adc, &adcip);
			if (ret < 0)
				pr_err("init failed with: %d\n", ret);
		}
		else if(!strcmp(input, "read")) {
			int i;
			pr_info("Number of samples: ");
			scanf("%s", &number);
			int n = atoi(number);
			uint16_t * pbuf = calloc(n, sizeof(uint16_t));
			ret = ad7689_read(adc, pbuf, n);
			if (ret < 0)
				pr_err("read failed with: %d\n", ret);
			else {
				pr_info("data:");
				for(i = 0; i < n; i++)
					pr_info(" %.4x", pbuf[i]);
				pr_info("\n");
			}

			free(pbuf);
		}
		else if(!strcmp(input, "config")) {
			struct ad7689_config c;
			ret = ad7689_read_config(adc, &c);
			if (ret < 0)
				pr_err("read_config failed with: %d\n", ret);
		}
		else if(!strcmp(input, "remove")) {
			ret = ad7689_remove(adc);
			if (ret < 0)
				pr_err("remove failed with: %d\n", ret);
		}
		else if (!strcmp(input, "test1")) {
			ret = test1(adc);
			if (ret < 0)
				pr_err("test1 failed with: %d\n", ret);
		}
		else if (!strcmp(input, "test2")) {
			ret = test2(adc);
			if (ret < 0)
				pr_err("test2 failed with: %d\n", ret);
		}
		else if (!strcmp(input, "test3")) {
			ret = test3(adc);
			if (ret < 0)
				pr_err("test3 failed with: %d\n", ret);
		}
		else
			continue;
	}

	pr_info("Bye!\n");
	return 0;
}
