/*
 * my_gpio.h
 *
 *  Created on: 2017Äê12ÔÂ3ÈÕ
 *      Author: hasee
 */

#ifndef SRC_MY_GPIO_H_
#define SRC_MY_GPIO_H_
#include "my_base_device.h"
#include "top.h"
#include "xgpio.h"
class my_gpio_in:public device_with_intr{
public:
	XGpio GpioInstance;

	my_gpio_in(int _device_id,int _object_intrc_ID,Xil_ExceptionHandler _handler=default_gpio_intr_handler):
		device_with_intr(_device_id,_object_intrc_ID,_handler)
	{
		Status = XGpio_Initialize(&GpioInstance,device_id);
		test_failed(Status,"gpio initial");


		XGpio_SetDataDirection(&GpioInstance,1,0xffffffff/*input*/);
		XGpio_InterruptEnable(&GpioInstance,1);//for channel 1
		XGpio_InterruptGlobalEnable(&GpioInstance);// this to to control ch1 and ch2 at the same time.
	}
	volatile u32 data;
	static void default_gpio_intr_handler(void *callback_ref){
		my_gpio_in &Data = *reinterpret_cast<my_gpio_in*>(callback_ref);
		XGpio_InterruptClear(&Data.GpioInstance, 1);
		int read = XGpio_DiscreteRead(&Data.GpioInstance,1); // push 1-- up 1 : two interupts per push
		if(read) Data.data = read;
	}

};

class my_gpio_out:public device{
public:
	XGpio GpioInstance;

	my_gpio_out(int _device_id):device(_device_id)
	{
		Status = XGpio_Initialize(&GpioInstance,device_id);
		test_failed(Status,"gpio initial");
		XGpio_SetDataDirection(&GpioInstance,1,0x0/*output*/);
	}

	void write(int value){
		XGpio_DiscreteWrite(&GpioInstance,1,value); //test
	}

};



#endif /* SRC_MY_GPIO_H_ */
