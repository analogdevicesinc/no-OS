#include "adl8113.h"
#include "common_data.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"


int basic_example_main(){
    struct adl8113_dev* device;
    //setting up leds
    /*

    red=External bypass B
    green=internal bypass
    blue=external bypass A
    all high=internal amp

    */
    struct no_os_gpio_desc *led_red, *led_green, *led_blue;

    no_os_gpio_get(&led_red, &led_red_ip);
    no_os_gpio_get(&led_green, &led_green_ip);
    no_os_gpio_get(&led_blue, &led_blue_ip);
    no_os_gpio_direction_output(led_red, NO_OS_GPIO_HIGH);  // OFF initially
    no_os_gpio_direction_output(led_green, NO_OS_GPIO_HIGH);  // OFF initially
    no_os_gpio_direction_output(led_blue, NO_OS_GPIO_HIGH);  // OFF initially
    //no_os_mdelay(3000);  //wait 3 seconds for program to start
    //init button to cycle trough states and set gpio as input
    struct no_os_gpio_desc *button1;
    uint8_t val=1;//not pressed

    no_os_gpio_get(&button1, &button1_ip);
    no_os_gpio_direction_input(button1);

    //device init
    int ret=adl8113_init(&device,&adl8113_param);
    if(ret)
        return ret;

    //cycle troygh all chip modes trough the button
    enum adl8113_mode mode;
    ret=adl8113_get_mode(device, &mode);
    if(ret)
        goto error;
    while(1){
        switch(mode){
            case ADL8113_INTERNAL_AMPLIFIER:
                no_os_gpio_set_value(led_red, NO_OS_GPIO_LOW);
                no_os_gpio_set_value(led_blue, NO_OS_GPIO_LOW);//all high for int amp
                no_os_gpio_set_value(led_green, NO_OS_GPIO_LOW);
                pr_info("Entering internal amplifier mode and waiting for button:\n");

                no_os_gpio_get_value(button1,&val);
                if (val==0){
                    no_os_mdelay(50);  // Wait 50ms for debounce
                    no_os_gpio_get_value(button1,&val);
                    if(val==0){
                        pr_info("Button pressed.\n");
                        mode=ADL8113_INTERNAL_BYPASS;
                        ret=adl8113_set_mode(device,mode);
                        if(ret)
                            goto error;
                        no_os_udelay(1);
                        while(val==0){
                            no_os_gpio_get_value(button1, &val);
                            no_os_mdelay(10);
                        }
                        no_os_mdelay(50);
                    }
                }
                break;

            case ADL8113_INTERNAL_BYPASS:
                no_os_gpio_set_value(led_red, NO_OS_GPIO_HIGH);
                no_os_gpio_set_value(led_blue, NO_OS_GPIO_HIGH);//keep green on high
                no_os_gpio_set_value(led_green, NO_OS_GPIO_LOW);
                pr_info("Entering internal bypass mode and waiting for button:\n");

                no_os_gpio_get_value(button1,&val);
                if (val==0){
                    no_os_mdelay(50);  // Wait 50ms for debounce
                    no_os_gpio_get_value(button1,&val);
                    if(val==0){
                        pr_info("Button pressed.\n");
                        mode=ADL8113_EXTERNAL_BYPASS_A;
                        ret=adl8113_set_mode(device,mode);
                        if(ret)
                            goto error;
                        no_os_udelay(1);
                        while(val==0){
                            no_os_gpio_get_value(button1, &val);
                            no_os_mdelay(10);
                        }
                        no_os_mdelay(50);
                    }
                }
                break;

            case ADL8113_EXTERNAL_BYPASS_A:
                no_os_gpio_set_value(led_red, NO_OS_GPIO_HIGH);
                no_os_gpio_set_value(led_green, NO_OS_GPIO_HIGH);//keep blue on high
                no_os_gpio_set_value(led_blue, NO_OS_GPIO_LOW);

                pr_info("Entering external bypass A mode and waiting for button:\n");

                no_os_gpio_get_value(button1,&val);
                if (val==0){
                    no_os_mdelay(50);  // Wait 50ms for debounce
                    no_os_gpio_get_value(button1,&val);
                    if(val==0){
                        pr_info("Button pressed.\n");
                        mode=ADL8113_EXTERNAL_BYPASS_B;
                        ret=adl8113_set_mode(device,mode);
                        if(ret)
                            goto error;
                        no_os_udelay(1);
                        while(val==0){
                            no_os_gpio_get_value(button1, &val);
                            no_os_mdelay(10);
                        }
                        no_os_mdelay(50);
                    }
                }
                break;

            case ADL8113_EXTERNAL_BYPASS_B:
                no_os_gpio_set_value(led_blue, NO_OS_GPIO_HIGH);
                no_os_gpio_set_value(led_green, NO_OS_GPIO_HIGH);//keep red on high
                no_os_gpio_set_value(led_red, NO_OS_GPIO_LOW);
                pr_info("Entering external bypass B mode and waiting for button:\n");

                no_os_gpio_get_value(button1,&val);
                if (val==0){
                    no_os_mdelay(50);  // Wait 50ms for debounce
                    no_os_gpio_get_value(button1,&val);
                    if(val==0){
                        pr_info("Button pressed.\n");
                        mode=ADL8113_INTERNAL_AMPLIFIER;
                        ret=adl8113_set_mode(device,mode);
                        if(ret)
                            goto error;
                        no_os_udelay(1);
                        while(val==0){
                            no_os_gpio_get_value(button1, &val);
                            no_os_mdelay(10);
                        }
                        no_os_mdelay(50);
                    }
                }
                break;
         }
    }

    //remove device
    return adl8113_remove(device);

    error:
        pr_info("Error!\n");
        return adl8113_remove(device);

}
