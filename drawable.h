/*
 * drawable.h
 *
 *  Created on: 2018Äê1ÔÂ3ÈÕ
 *      Author: hasee
 */

#ifndef SRC_DRAWABLE_H_
#define SRC_DRAWABLE_H_
#include "geometry.h"
#include <stdio.h>
#include <stdint.h>
#include "color.h"
#include "xil_types.h"
#include <math.h>
#define M 20
#define N 15
#define D 32
//#define W 16
#define WX 16
#define WY 16
//const int M = 20;
//const int N = 15;
//const int D = 32;
//const int W = 16;
using color_type = Color_Type;
using grid_buffer = color_type [WX][WY];

extern u8 SmallFont[];

class drawable;
class grid_net{
public:
	using drawer_t = drawable*;

	drawer_t drawer[M][N][D];
	int drawer_tail[M][N];

	bool LastTime[M][N];

	bool push(int m,int n, drawer_t who)
	{
		if(m<0 || m>=M || n<0 || n>=N) return false;
		int &tail = drawer_tail[m][n];
		if(tail<D)
		{
			drawer[m][n][tail]=who;
			++tail;
			return true;
		}
		else
			return false;
	}

	inline void rend_to_grid(int m,int n, grid_buffer& buffer, int piece);

	void reset_grid(int m,int n)
	{
		LastTime[m][n] = drawer_tail[m][n]>0;
		drawer_tail[m][n] = 0;
	}

	void leave_grid(int m,int n)
	{
		drawer_tail[m][n] = 0;
	}

	grid_net() {
		for (int m = 0; m < M; ++m)
			for (int n = 0; n < N; ++n)
				LastTime[m][n] = true;
	}

	bool need_draw(int m,int n)
	{
		return drawer_tail[m][n]>0 || LastTime[m][n];
	}


};

class drawable:public Geometry_Rect{
public:
	color_type my_color;
	virtual void with_grids(grid_net &grid)
	{
		Region_t region = get_region();
		for(int i=(region.x.first/WX);i<=(region.x.second-1)/WX;++i)
		{
			for(int j=(region.y.first/WY);j<=(region.y.second-1)/WY;++j)
			{
				grid.push(i,j,this);
			}
		}
	}

	virtual void draw_in_grid(grid_buffer& buffer, Loc_t grid_loc,const Region_t &one_grid_region)	{
//		Region_t one_grid_region = {{0,WX},{0,WY}};
		Region_t region = (one_grid_region).join(get_region() - grid_loc);

		for(int i = region.x.first;i<region.x.second;++i)
		{
			for(int j=region.y.first;j<region.y.second;++j)
			{
				buffer[i][j] = my_color;
			}
		}

	}
};
inline Region_t grid_region(int x,int y)
{
	int xmin = x * WX;
	int ymin = y * WY;
	return {{xmin,xmin+WX},{ymin,ymin+WY}};
}
class Drawable_Circle:public drawable{
public:
	int r;
	virtual void with_grids(grid_net &grid)
	{
		int r2 = r * r;
		int cx = location.x + size.x /2;
		int cy = location.y + size.y /2;

		for(int i=(location.x/WX);i<=(location.x+size.x-1)/WX;++i)
		{
			int x1 = WX * i - cx;
			int x2 = x1 + WX -1;
			int xx = std::min( x1*x1 , x2*x2 );
			for(int j=(location.y/WY);j<=(location.y+size.y-1)/WY;++j)
			{
				int y1 = WY*j - cy;
				int y2 = y1 + WY -1;
				int yy = std::min( y1*y1 , y2*y2 );

				if( xx+yy <r2 )
					grid.push(i,j,this);

			}
		}
	}

	virtual void draw_in_grid(grid_buffer& buffer, Loc_t grid_loc,const Region_t &one_grid_region)	{
//		Region_t one_grid_region = {{0,WX},{0,WY}};
		Region_t region = (one_grid_region).join(get_region() - grid_loc);
		Loc_t center = location + Loc_t{size.x/2,size.y/2} - grid_loc ;
		int r2 = r*r;

		for(int i = region.x.first;i<region.x.second;++i)
		{
			int dis_x = i - center.x;
			int dis_y2_max = r2 - dis_x * dis_x;
			int dis_y_max = dis_y2_max>0? sqrtf(dis_y2_max):0;

			Range_t range_real = region.y.join({center.y-dis_y_max,center.y+dis_y_max});

			for(int j=range_real.first;j<range_real.second;++j)
			{
				buffer[i][j] = my_color;
			}
		}

	}
};

class Drawable_Script:public drawable{
public:
	char line[6];
	virtual void with_grids(grid_net & grid){
		for(int i=(location.x/WX);i<=(location.x+size.x-1)/WX;++i)
		{
			for(int j=(location.y/WY);j<=(location.y+size.y-1)/WY;++j)
			{
				grid.push(i,j,this);
			}
		}
	}
	virtual void draw_in_grid(grid_buffer& buffer, Loc_t grid_loc,const Region_t &one_grid_region){
		Region_t region = (one_grid_region).join(get_region() - grid_loc);
//		char script1 = line[(grid_loc.x - location.x)/(WX/2)];
//		char script2 = line[(grid_loc.x - location.x)/(WX/2)+1];

		char script1 = line[(grid_loc.y - location.y)/(WX/2)];
		char script2 = line[(grid_loc.y - location.y)/(WX/2)+1];

//		xil_printf("%d\n", (grid_loc.x - location.x)/(WX/2));
//		xil_printf("x%d, %d , %d, %d, %c\n", grid_loc.x, one_grid_region.x.first, location.x, region.x ,script);
//		xil_printf("y%d, %d , %d, %d, %c\n", grid_loc.y, one_grid_region.y.first, location.y, region.y ,script);
//		xil_printf("y%d, %d , %d, %d, %c\n", grid_loc.y, one_grid_region.y.first, location.y, region.y ,script);


//		int pixelIndex = (script - cfont.offset) * (cfont.x_size >> 3) * cfont.y_size + 4;
		int pixelIndex1 = (script1 - SmallFont[2]) * (SmallFont[0] >> 3) * SmallFont[1] + 4;
		int pixelIndex2 = (script2 - SmallFont[2]) * (SmallFont[0] >> 3) * SmallFont[1] + 4;

//		for(int j = 0; j < (cfont.x_size >> 3) * cfont.y_size; j++) {

		for(int j = 0; j < (SmallFont[0] >> 3) * SmallFont[1]; j++) {
			 char ch1 = SmallFont[pixelIndex1];
			 char ch2 = SmallFont[pixelIndex2];

			 for(int i = 0; i < 8; i++) {
				 if ((ch1 & (1 << ( 7-i))) != 0) {
//					buffer[region.x.first+i][region.y.first+j] = my_color;
					buffer[region.y.first+j][region.x.first+i] = my_color;

				 }
				 if ((ch2 & (1 << ( 7-i))) != 0) {
//					buffer[region.x.first+i+8][region.y.first+j] = my_color;
					buffer[region.y.first+j][region.x.first+i+8] = my_color;

				 }

			 }
			 pixelIndex1 ++;
			 pixelIndex2 ++;

		}

	}
};


#define Grid_BreakUp 2
#define WX_BreakUP (WX/Grid_BreakUp)
inline void grid_net::rend_to_grid(int m,int n, grid_buffer& buffer, int piece)
{
	Loc_t grid_loc = {WX*m,WY*n};

	if(piece < Grid_BreakUp && piece >= 0)
	{
		Region_t part = {{piece*WX_BreakUP,(piece+1)*WX_BreakUP},{0,WY}};
			for(int i=0;i<drawer_tail[m][n];++i){
				drawer[m][n][i]->draw_in_grid(buffer,grid_loc,part);
			}
	}

}


template <int PointN>
class Drawable_Rotate_Multi : public drawable{
public:
	FLoc_t point_to;


	//static FLoc_t St_vertex [PointN];
	 FLoc_t Cu_vertex [PointN];

	using Shape_t = FLoc_t [PointN];
	Shape_t &St_vertex ;

	Region_t this_region;

	Drawable_Rotate_Multi (Shape_t &shape):St_vertex(shape){

	}

	void follow(Move_t toward)
	{
		float dis = toward.x*toward.x + toward.y*toward.y;
		dis = 1./sqrtf(dis); // I can optimize it
		point_to.x = toward.x * dis;
		point_to.y = toward.y * dis;
	}

	void rotate()
	{
		float fx1,fx2,fy1,fy2;
		fx1 = fy1 = 9999.;
		fx2 = fy2 = -9999.;

		for(int i=0;i<PointN;++i)
		{
			auto &st = St_vertex[i];
			auto &cu = Cu_vertex[i];
			cu.x = st.x * point_to.x - st.y * point_to.y;
			cu.y = st.x * point_to.y + st.y * point_to.x;

			fx1 = std::min(cu.x,fx1);
			fx2 = std::max(cu.x,fx2);
			fy1 = std::min(cu.y,fy1);
			fy2 = std::max(cu.y,fy2);
		}
		this_region.x.first = fx1 + location.x;
		this_region.x.second = fx2 + location.x;
		this_region.y.first = fy1 + location.y;
		this_region.y.second = fy2 + location.y;

		//dirty hacks
		size.x = this_region.x.second - this_region.x.first;
		size.y = this_region.y.second - this_region.y.first;
	}
	Region_t& get_region(){return this_region;}

	Range_t corss_range(float x){
		FRange_t range = {741,741};
		for(int i=0;i<PointN;++i)
		{
			int next = (i+1)%PointN;
			auto &cu = Cu_vertex[i];
			auto &nx = Cu_vertex[next];
//			if(cu.x == nx.x)
//			{
//				range.first = cu.y;
//				range.second = nx.y;
//				break;
//			}
			if(cu.x<=x && nx.x>x)
			{
				range.first = (cu.y* (nx.x-x) + nx.y*(x-cu.x))/(nx.x-cu.x);
			}
			if(cu.x>x && nx.x<=x)
			{
				range.second = (cu.y*(x-nx.x) + nx.y*(cu.x-x))/(cu.x-nx.x);
			}
		}
		// Can be removed
		return (range.first<range.second)?Range_t{range.first,range.second}:Range_t{range.second,range.first};
	}

	virtual void with_grids(grid_net &grid)
	{
		Region_t region = get_region();
		for(int i=(region.x.first/WX);i<=(region.x.second-1)/WX;++i)
		{
			for(int j=(region.y.first/WY);j<=(region.y.second-1)/WY;++j)
			{
				grid.push(i,j,this);
			}
		}
	}
	virtual void draw_in_grid(grid_buffer& buffer, Loc_t grid_loc,const Region_t &one_grid_region)	{
		Region_t region = (one_grid_region).join(get_region() - grid_loc);

		for(int i = region.x.first;i<region.x.second;++i)
		{
			Range_t y_range_poly = corss_range(i+grid_loc.x-location.x)+(location.y - grid_loc.y);
			Range_t range_real = region.y.join(y_range_poly);

			for(int j=range_real.first;j<range_real.second;++j)
			{
				buffer[i][j] = my_color;
			}
		}

	}


};
//template<>
//FLoc_t Drawable_Rotate_Multi<3,0>::St_vertex [3];


#endif /* SRC_DRAWABLE_H_ */
