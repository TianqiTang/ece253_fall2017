/*
 * my_base_device.h
 *
 *  Created on: 2017Äê12ÔÂ2ÈÕ
 *      Author: hasee
 */

#ifndef SRC_MY_BASE_DEVICE_H_
#define SRC_MY_BASE_DEVICE_H_
#include "xintc.h"
#include "top.h"

class device{
public:
	int device_id;
	device(int _device_id):device_id(_device_id){}
};
class device_with_intr:public device{
public:
	int object_intrc_ID;
	Xil_ExceptionHandler handler;
	void *callback_ref;
	device_with_intr(int _device_id,int _object_intrc_ID,Xil_ExceptionHandler _handler,void *_callback_ref=0):
		device(_device_id),object_intrc_ID(_object_intrc_ID),handler(_handler),callback_ref(_callback_ref){
		if(callback_ref == 0) callback_ref = this;
	}
};

class my_xintc{
public:
	XIntc intc;
	int intc_ID;
	my_xintc(int _intc_ID){
		intc_ID = _intc_ID;
		Status= XIntc_Initialize(&intc,intc_ID);
		test_failed(Status,"inital intc");
	}

	void connect(device_with_intr& dev)
	{
		Status = XIntc_Connect(&intc,dev.object_intrc_ID,dev.handler,dev.callback_ref);
		test_failed(Status,"Intc connect");
		XIntc_Enable(&intc,dev.object_intrc_ID);
	}

	void run(){
		Xil_ExceptionInit(); // did no thing
		Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
				 (Xil_ExceptionHandler)XIntc_InterruptHandler, &intc);
		/* Enable non-critical exceptions */
		Xil_ExceptionEnable();
		Status = XIntc_Start(&intc, XIN_REAL_MODE);
		test_failed(Status,"start intc");
	}
};


#endif /* SRC_MY_BASE_DEVICE_H_ */
