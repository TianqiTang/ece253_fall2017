/*
 * my_iic.h
 *
 *  Created on: 2017Äê12ÔÂ2ÈÕ
 *      Author: hasee
 */

#ifndef SRC_MY_IIC_H_
#define SRC_MY_IIC_H_
#include "my_base_device.h"
#include "top.h"
#include "xiic.h"

class wii_data_filed{
public:
	int stk_x;
	int stk_y;
//	u8 acc_x;
//	u8 acc_y;
//	u8 acc_z;
	u8 push1;
	u8 push2;
};

class iic:public device_with_intr{
public:
	XIic_Config *ConfigPtr;
	XIic IicInstance;
	int SLAVE_ADDRESS;

	iic(int _device_id,int _object_intrc_ID,int _SLAVE_ADDRESS)
	:device_with_intr( _device_id, _object_intrc_ID, XIic_InterruptHandler,&IicInstance)
	{
		SLAVE_ADDRESS = _SLAVE_ADDRESS;
		ConfigPtr = XIic_LookupConfig(device_id);
		if(!ConfigPtr) xil_printf("find Config IIC failed\n"); // 0 = failed , so can't use testfailed.
		Status = XIic_CfgInitialize(&IicInstance, ConfigPtr,
						ConfigPtr->BaseAddress);
		test_failed(Status,"find IIC XIic_CfgInitialize");
		Status = XIic_SetAddress(&IicInstance, XII_ADDR_TO_SEND_TYPE,
					 SLAVE_ADDRESS);

		XIic_SetSendHandler(&IicInstance, this,
					(XIic_Handler) SendHandler);
		XIic_SetRecvHandler(&IicInstance, this,
					(XIic_Handler) ReceiveHandler);
		XIic_SetStatusHandler(&IicInstance, this,
					  (XIic_StatusHandler) StatusHandler);
	}
	volatile u8 TransmitWait; // 0 for complete, 1 for not complete : use example: while(TransmitComplete){}
	volatile u8 ReceiveWait;
		static void SendHandler (void *CallBackRef, int ByteCount){
			iic &me = *reinterpret_cast<iic*>(CallBackRef);
			me.TransmitWait = 0;
		}
		static void ReceiveHandler (void *CallBackRef, int ByteCount){
			iic &me = *reinterpret_cast<iic*>(CallBackRef);
			me.ReceiveWait = 0;
		}
		static void StatusHandler (void *CallBackRef, int StatusEvent){

		}


		int WriteData(u8* WriteBuffer,u16 ByteCount)
		{
			/*
			 * Set the defaults.
			 */
			TransmitWait = 1;
			/*
			 * Start the IIC device.
			 */
			Status = XIic_Start(&IicInstance);
			if (Status != XST_SUCCESS) {
				return XST_FAILURE;
			}
			IicInstance.Options = 0x0;
			/*
			 * Send the Data.
			 */
			Status = XIic_MasterSend(&IicInstance, WriteBuffer, ByteCount);
			if (Status != XST_SUCCESS) {
				return XST_FAILURE;
			}
			while ((TransmitWait) || (XIic_IsIicBusy(&IicInstance) == TRUE)) {

			}
			Status = XIic_Stop(&IicInstance);
			if (Status != XST_SUCCESS) {
				return XST_FAILURE;
			}
			return XST_SUCCESS;
		}

		int ReadData(u8 *ReadBuffer, u16 ByteCount)
		{

			/*
			 * Set the defaults.
			 */
			ReceiveWait = 1;

			/*
			 * Start the IIC device.
			 */
			Status = XIic_Start(&IicInstance);
			if (Status != XST_SUCCESS) {
				return XST_FAILURE;
			}


			IicInstance.Options = 0x0;

			/*
			 * Receive the Data.
			 */
			Status = XIic_MasterRecv(&IicInstance, ReadBuffer, ByteCount);
			if (Status != XST_SUCCESS) {
				return XST_FAILURE;
			}

			/*
			 * Wait till all the data is received.
			 */
			while ((ReceiveWait) || (XIic_IsIicBusy(&IicInstance) == TRUE)) {

			}

			/*
			 * Stop the IIC device.
			 */
			Status = XIic_Stop(&IicInstance);
			if (Status != XST_SUCCESS) {
				return XST_FAILURE;
			}

			return XST_SUCCESS;
		}

		wii_data_filed data;
		u8 simple_buffer[6];
		void init_control(){
			 static u8 init_str[] = {0xF0, 0x55};// use two string to setup that without encryption
			 static u8 init_str2[] = {0xFB,  0x00};
			WriteData(init_str,2);
			WriteData(init_str2,2);
		}
		void read_data(){
			 static u8 data_location[] = {0x00};
			WriteData(data_location,1);
			ReadData(simple_buffer,6);
			data.stk_x = simple_buffer[0]-128;
			data.stk_y = simple_buffer[1]-128;
			data.push1 = 1^(simple_buffer[5]&1);
			data.push2 = 1^((simple_buffer[5]>>1)&1);
		}
};




#endif /* SRC_MY_IIC_H_ */
