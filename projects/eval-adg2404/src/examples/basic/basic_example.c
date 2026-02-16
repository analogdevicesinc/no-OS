#include "adg2404.h"
#include "common_data.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"

#define safeSwitchingDelay 630 //nanoseconds     it is for 12V,125 degrees,OFF switching

/*
struct no_os_gpio_init_param {
	Port number

	int32_t		port;

	GPIO number

	int32_t		number;

	Pull up/down resistor configuration

	enum no_os_gpio_pull_up pull;

	GPIO platform specific functions

	const struct no_os_gpio_platform_ops *platform_ops;

	GPIO extra parameters (device specific)

	void		*extra;
};
*/

int main(){
    int ret;
    //init structure ,device structure and MUX
    struct adg2404_dev* adg2404Dev=NULL;

    //init mux ,by creating the descriptors
    ret=adg2404_init(&adg2404Dev,&adg2404_ip);
    if(ret)
        return ret;
    no_os_udelay(1);
    //disable mux to start in a known state
    ret=adg2404_enable(adg2404Dev,false);
    if(ret)
        goto error;
    no_os_udelay(1);
    //enable mux
    ret=adg2404_enable(adg2404Dev,true);
    if(ret)
        goto error;
    no_os_udelay(1);

    //cycle trough channels,to cover truth tablew
    enum adg2404_channel channels[]={ADG2404_S1, ADG2404_S2, ADG2404_S3, ADG2404_S4,ADG2404_CH_OFF };
while(1){
    // ret=adg2404_select_channel(adg2404Dev,channels[3]);
    //     if(ret)
    //         goto error;
    for(int i=0;i<5;i++){
        ret=adg2404_select_channel(adg2404Dev,channels[i]);
        if(ret)
            goto error;
        no_os_mdelay(3000);
    }
}
    //edge cases (needs to be studied a bit more)(30 minutes needed)
    //only timing constraints that need to be respected

    //disable mux
    ret=adg2404_enable(adg2404Dev,false);
    no_os_udelay(1);
    //remove mux
    return adg2404_remove(adg2404Dev);
    
    error:
        pr_info("Error!\n");
        return adg2404_remove(adg2404Dev);
}
