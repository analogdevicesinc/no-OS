#include "no_os_gpio.h"
#include "maxim_gpio.h"
#include "max32655.h"

struct max_gpio_init_param led0_extra={
    .vssel=MXC_GPIO_VSSEL_VDDIO,
};

struct max_gpio_init_param led1_extra={
    .vssel=MXC_GPIO_VSSEL_VDDIO,
};

struct max_gpio_init_param led2_extra={
    .vssel=MXC_GPIO_VSSEL_VDDIO,
};

struct no_os_gpio_init_param GPIOled0={
    .port=0,
    .number=18,
    .pull=NO_OS_PULL_NONE,
    .platform_ops=&max_gpio_ops,
    .extra=&led0_extra,
};

struct no_os_gpio_init_param GPIOled1={
    .port=0,
    .number=19,
    .pull=NO_OS_PULL_NONE,
    .platform_ops=&max_gpio_ops,
    .extra=&led1_extra,
};

struct no_os_gpio_init_param GPIOled2={
    .port=0,
    .number=26,
    .pull=NO_OS_PULL_NONE,
    .platform_ops=&max_gpio_ops,
    .extra=&led2_extra,
};

int main(){
    //define descriptors to be set by the get function
    struct no_os_gpio_desc* led0_desc;
    struct no_os_gpio_desc* led1_desc;
    struct no_os_gpio_desc* led2_desc;

    //sets at register level the GPIO pins ,what was set in param struct,and shows what was actually configured in the desc function
    no_os_gpio_get(&led0_desc,&GPIOled0);
    no_os_gpio_get(&led1_desc,&GPIOled1);
    no_os_gpio_get(&led2_desc,&GPIOled2);

    //set the pin on output and the value on the gpio pin
    no_os_gpio_direction_output(led0_desc,NO_OS_GPIO_LOW);
    no_os_gpio_direction_output(led1_desc,NO_OS_GPIO_LOW);
    no_os_gpio_direction_output(led2_desc,NO_OS_GPIO_LOW);

    //set the value on the gpio pin
    no_os_gpio_set_value(led0_desc,NO_OS_GPIO_LOW);
    no_os_gpio_set_value(led1_desc,NO_OS_GPIO_LOW);
    no_os_gpio_set_value(led2_desc,NO_OS_GPIO_LOW);

    // Infinite loop - main() should never return in embedded systems
    while(1) {
        // LEDs are now on, keep them on
    }
}


