#include "no_os_print_log.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"
#include "adxl371.h"
#include "common_data.h"

int example_main(){
    //adxl372 init and start SPI
    struct adxl37x_dev *dev;
    struct adxl37x_init_param dev_init=adxl371_init_param;
    int32_t ret;
	struct no_os_uart_desc *uart;

	ret = no_os_uart_init(&uart, &adxl371_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart);

    ret=adxl371_init(&dev,dev_init);
    if(ret!=0)
        goto error_init;
    volatile uint8_t status1,status2,pwr_ctl;


        adxl371_read_reg(dev,0x3F,&pwr_ctl);
    pr_info("pwr_ctl register after init of adxl37x: 0x%02X\n\r",pwr_ctl);


    //extract data
    struct adxl37x_xyz_accel_data recorded_data[512];
    int count=0;
    uint16_t numberSamples=127;
    ret=adxl371_set_internal_sync(dev);
    if(ret){
        printf("Internal sync failed!\r\n");
        goto error_init;
    }
    int16_t x,y,z;
    while(1){

        ret=adxl371_get_fifo_xyz_data_workaround(dev,recorded_data,numberSamples);
            if(ret!=0)
             goto error_init;
        
        for(int i=0;i<numberSamples;i++){
            x=(int16_t)(recorded_data[i].x<<4)>>4;//sign extension
            y=(int16_t)(recorded_data[i].y<<4)>>4;
            z=(int16_t)(recorded_data[i].z<<4)>>4;
            pr_info("Datele accel sunt la sample %d: x=%d,y=%d,z=%d\r\n",i,x,y,z);
         }
    }
    //dealloc
    no_os_free(dev);
	return ret;
    //error handling
    error_init:
        pr_err("Error!Free adxl372 struct\n");
	    return ret;
        //free structs


}