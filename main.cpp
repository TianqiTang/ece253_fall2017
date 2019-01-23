#include "xparameters.h"
#include "xintc.h"
#include "xiic.h"
#include "xil_cache.h"
#include "xil_types.h"
#include "xil_assert.h"
#include <sleep.h>
#include <stdio.h>
#include "my_base_device.h"
#include "my_iic.h"
#include "my_gpio.h"
#include "my_tmrctr.h"
#include "top.h"
#include "my_container.h"
#include "drawable.h"
#include "game.h"
#include "my_spi.h"
#include "lcd.h"
int Status;




int main()
{
	Xil_ICacheEnable();
	Xil_DCacheEnable();

	static iic iic_0(XPAR_IIC_0_DEVICE_ID,XPAR_MICROBLAZE_0_AXI_INTC_AXI_IIC_0_IIC2INTC_IRPT_INTR,0x52);
	static iic iic_1(XPAR_IIC_1_DEVICE_ID,XPAR_MICROBLAZE_0_AXI_INTC_AXI_IIC_1_IIC2INTC_IRPT_INTR,0x52);
	static my_xintc intc_obj(XPAR_INTC_0_DEVICE_ID);
	static my_gpio_in btn(XPAR_AXI_GPIO_0_DEVICE_ID,XPAR_MICROBLAZE_0_AXI_INTC_AXI_GPIO_0_IP2INTC_IRPT_INTR);
	static my_gpio_out led(XPAR_AXI_GPIO_1_DEVICE_ID);
	static my_tmrctr clock(XPAR_AXI_TIMER_0_DEVICE_ID,XPAR_MICROBLAZE_0_AXI_INTC_AXI_TIMER_0_INTERRUPT_INTR);
	static my_spi display(XPAR_SPI_DEVICE_ID, XPAR_MICROBLAZE_0_AXI_INTC_SPI_IP2INTC_IRPT_INTR);

	static Game game;
	led.write(0b10101010);



	intc_obj.connect(iic_0);
	intc_obj.connect(iic_1);
	intc_obj.connect(btn);
	intc_obj.connect(clock);
	intc_obj.connect(display);
	intc_obj.run();

//	clock.start();


//	xil_printf("init begin\n");
	iic_0.init_control();
	iic_1.init_control();
//	xil_printf("full\n");

	initLCD();
	xil_printf("\nente2222222r\n\n");
	clrScr();
	xil_printf("\nente33333r\n\n");
	setColor(255, 0, 0);

	fillRect(20, 20, 220, 300);

	xil_printf("\n444444444444\n\n");
	int t=0;
	while(1){
//		usleep(1000);
		++t;
//		xil_printf("Before read data");
		iic_0.read_data();
		iic_1.read_data();
//			xil_printf("joystic: %d\t%d\t%d\t%d\t\n",iic_0.data.stk_x,iic_0.data.stk_y,iic_1.data.stk_x,iic_1.data.stk_y);

		// Set move and fire
		game.input.fire = 	iic_0.data.push1;
		game.input.move = {iic_0.data.stk_x>>4,iic_0.data.stk_y>>4};
		game.run();
		game.draw_grid(grid);

		general_display(grid);
//		general_display_limit(grid,20);

	}

	return 0;
}
