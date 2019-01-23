

#include "drawable.h"

#include <functional>
#include "game.h"
#include "lcd.h"

FLoc_t Triangle_Shape[3];
grid_buffer out_buffer;
grid_net grid;

uint32_t seed = 0;

void fill_a_grid_buffer(grid_buffer &buffer,color_type color)
{
	for(int i=0;i<WX;++i)
		for(int j=0;j<WY;++j)
			buffer[i][j]=color;
}

void trans_grid(int i, int j, grid_buffer& buffer){

//	setXY(i*W, j*W, (i+1)*W-1, (j+1)*W-1);
	setXY(j*WY, i*WX, (j+1)*WY-1, (i+1)*WX-1);

	LCD_Write_DATA_FIFO((color_type*)buffer);

}

//inline void trans_grid_part(const Region_t & region, const grid_buffer& buffer){
//
////	setXY(i*W, j*W, (i+1)*W-1, (j+1)*W-1);
//	setXY(j*WY, i*WX, (j+1)*WY-1, (i+1)*WX-1);
//
//	LCD_Write_DATA_FIFO((color_type*)buffer);
//
//}
void general_display(grid_net & grid)
{
	for(int i=0;i<M;++i)
	{
		for(int j=0;j<N;++j)
		{
			//if(! grid.need_draw(i,j)) continue;

			//1
//			copy_background_to_buffer(i,j,out_buffer,content);
			if (grid.drawer_tail[i][j] > 0) {
				// Should copy from the background
				fill_a_grid_buffer(out_buffer, ColorTable::Green);
			}
			else if (grid.LastTime[i][j]) {
				// Should Copy from background here
				fill_a_grid_buffer(out_buffer, ColorTable::Green);
			}
			else {
				// Use 'Continue' 
				continue;
				//fill_a_grid_buffer(out_buffer, ((i + j) % 2) ? 'o' : ' ');
			}

			setXY(j*WY, i*WX, (j+1)*WY-1, (i+1)*WX-1);
			for(int part =0;part<2;++part)
			{
				grid.rend_to_grid(i,j,out_buffer,part);
				LCD_Write_DATA_FIFO_LESE_128( ((color_type*)out_buffer) +part*128,128);
//				LCD_Write_DATA_FIFO_LESE_128_Type2( ((color_type*)out_buffer) +part*128,128);
			}

			grid.reset_grid(i,j);



		}
	}
}

// No more then limit times, but also no more then one round
void general_display_limit(grid_net & grid, int limit)
{
//	xil_printf("run \n");
	static int i = 0;
	static int j = 0;
	int full_range = M*N;


	while(true)
	{
		for(;i<M;++i)
		{
			for(;j<N;++j)
			{

//				xil_printf("Boom !\n");
				if(full_range == 0 || limit == 0) goto exit;
				--full_range;

				if(! grid.need_draw(i,j)) continue;
				--limit;

				fill_a_grid_buffer(out_buffer, ColorTable::Green);
				setXY(j*WY, i*WX, (j+1)*WY-1, (i+1)*WX-1);
				for(int part =0;part<2;++part)
				{
					grid.rend_to_grid(i,j,out_buffer,part);
					LCD_Write_DATA_FIFO_LESE_128( ((color_type*)out_buffer) +part*128,128);
	//				LCD_Write_DATA_FIFO_LESE_128_Type2( ((color_type*)out_buffer) +part*128,128);
				}

				grid.reset_grid(i,j);

			}
			j=0;
		}
	i = 0;
	}

exit:
 	if(full_range == 0) return;
 	// else : too much to draw in this round
 	int i2 = i;
 	int j2 = j;
 	for(;i2<M;++i2)
 	{
 		for(;j2<N;++j2)
 		{
 			if(full_range == 0) return;
 			-- full_range;
 			grid.leave_grid(i,j);
 		}
 	}
}
