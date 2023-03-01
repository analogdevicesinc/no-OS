#include "no_os_error.h"
#include "stm32_hal.h"
int stm32_get_exti_irq_id_from_pin(uint8_t pin_nb, IRQn_Type *irq_id)
{
	switch(pin_nb) {
	case 4:
		*irq_id = EXTI4_IRQn;
		break;
	case 3:
		*irq_id = EXTI3_IRQn;
		break;
	case 1:
		*irq_id = EXTI1_IRQn;
		break;
	case 2:
		*irq_id = EXTI2_IRQn;
		break;
	case 0:
		*irq_id = EXTI0_IRQn;
		break;
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
		*irq_id = EXTI9_5_IRQn;
		break;
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
		*irq_id = EXTI15_10_IRQn;
		break;
	default:
		return -ENOSYS;
	}
	return 0;
}