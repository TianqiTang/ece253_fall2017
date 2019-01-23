/*
 * my_tmrctr.h
 *
 *  Created on: 2017Äê12ÔÂ3ÈÕ
 *      Author: hasee
 */

#ifndef SRC_MY_TMRCTR_H_
#define SRC_MY_TMRCTR_H_
#include "my_base_device.h"
#include "top.h"
#include "xtmrctr.h"

class my_tmrctr:public device_with_intr{
public:
	XTmrCtr tmrctr;
	int timeLength = 0x07FFFFFF;
	int tmr_options=XTC_INT_MODE_OPTION | XTC_AUTO_RELOAD_OPTION;
	XTmrCtr_Handler Timmer_Handler=default_timmer_handler;

	my_tmrctr(int _device_id,int _object_intrc_ID)
		:device_with_intr( _device_id, _object_intrc_ID, (Xil_ExceptionHandler)XTmrCtr_InterruptHandler,&tmrctr){
		Status = XTmrCtr_Initialize(&tmrctr,_device_id);
		test_failed(Status,"initlal timmer\n");
		reset_configs();
	}


	void reset_configs()
	{
		XTmrCtr_SetHandler(&tmrctr, Timmer_Handler, this);
		XTmrCtr_SetOptions(&tmrctr, 0,tmr_options);
		XTmrCtr_SetResetValue(&tmrctr, 0, 0xFFFFFFFF - timeLength);
	}

	void start(){
		XTmrCtr_Start(&tmrctr, 0);
	}
	void stop(){
		XTmrCtr_Stop(&tmrctr, 0);
	}
	int count = 0;
	static void default_timmer_handler (void *CallBackRef, u8 TmrCtrNumber){
		my_tmrctr& me = *reinterpret_cast<my_tmrctr*>(CallBackRef);
		// No thing to be done
		me.count++;
	}
};


#endif /* SRC_MY_TMRCTR_H_ */
