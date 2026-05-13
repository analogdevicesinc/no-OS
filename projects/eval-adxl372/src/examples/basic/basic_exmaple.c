#include "no_os_print_log.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"
#include "adxl372.h"
#include "common_data.h"

int example_main(){
    //adxl372 init and start SPI
    struct adxl372_dev *dev;
    struct adxl372_init_param dev_init=adxl372_init_param;
    int32_t ret;
    ret=adxl372_init(&dev,dev_init);
    if(ret!=0)
        goto error_init;

    //extract data
    struct adxl372_xyz_accel_data recorded_data[512];
    int count=0;
    uint16_t numberSamples=0;
    while(1){
        ret=adxl372_service_fifo_ev(dev,recorded_data,&numberSamples);
        if(ret!=0)
            goto error_init;
        adxl372_get_fifo_xyz_data(dev,recorded_data,512);
        if(ret!=0)
            goto error_init;
        for(int i=0;i<numberSamples;i++){
            pr_info("Datele accel sunt la sample %d,la momentul %d: x=%d,y=%d,z=%d\r\n",i,count,recorded_data[i].x,recorded_data[i].y,recorded_data[i].z);
        }
       no_os_mdelay(1000);
       count++;
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