/*
 * my_spi.h
 *
 *  Created on: Dec 3, 2017
 *      Author: ttq1008
 */

#ifndef SRC_MY_SPI_H_
#define SRC_MY_SPI_H_
#include "my_base_device.h"
#include "top.h"
#include "xspi.h"


class my_spi:public device_with_intr{
public:
	XSpi_Config *ConfigPtr;
	XSpi SpiInstance; /* The instance of the SPI device */

	my_spi(int _device_id, int _object_intr_ID)
		:device_with_intr( _device_id, _object_intr_ID, XSpi_InterruptHandler,&SpiInstance){

		ConfigPtr = XSpi_LookupConfig(device_id);
		if (!ConfigPtr)	xil_printf("find Config SPI failed\n");
		Status = XSpi_CfgInitialize(&SpiInstance, ConfigPtr,
					  ConfigPtr->BaseAddress);
		test_failed(Status,"find SPI XSpi_CfgInitialize");



//		Status = XSpi_SetOptions(&SpiInstance, XSP_MASTER_OPTION |
//	 					XSP_LOOPBACK_OPTION);

		XSpi_Reset(&SpiInstance);
		int controlReg = XSpi_GetControlReg(&SpiInstance);
		XSpi_SetControlReg(&SpiInstance,
				(controlReg | XSP_CR_ENABLE_MASK | XSP_CR_MASTER_MODE_MASK) &
				(~XSP_CR_TRANS_INHIBIT_MASK));
		XSpi_SetSlaveSelectReg(&SpiInstance, ~0x01);
		XSpi_SetStatusHandler(&SpiInstance, this,
			 		(XSpi_StatusHandler) SpiHandler);
	}

	volatile u8 TransmitWait; // 0 for complete, 1 for not complete : use example: while(TransmitComplete){}

	static void SpiHandler(void *CallBackRef, int ByteCount){
		my_spi &me = *reinterpret_cast<my_spi*>(CallBackRef);
		me.TransmitWait = 0;
	}

	int spi_write(u8* WriteBuffer,u16 ByteCount){
		TransmitWait = 1;
		Status = XSpi_Start(&SpiInstance);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}
		XSpi_Transfer(&SpiInstance, WriteBuffer, NULL, ByteCount);
		while(TransmitWait);
		Status = XSpi_Stop(&SpiInstance);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}
		return XST_SUCCESS;
	}



};





#endif /* SRC_MY_SPI_H_ */
