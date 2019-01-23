/*
 * game.h
 *
 *  Created on: 2018Äê1ÔÂ9ÈÕ
 *      Author: hasee
 */

#ifndef SRC_GAME_H_
#define SRC_GAME_H_

#include "drawable.h"
#include "my_container.h"
#include "color.h"
#include <stdlib.h>
//class Geometry_Flat: public Geometry_Rect{
//	Range_t get_x_range(){
//		return {location.x,location.x+size.x};
//	}
//	virtual Range_t get_y_range(int loc_x);
//
//};
extern FLoc_t Triangle_Shape[3];
extern uint32_t seed;
inline uint32_t my_rand(){
	seed = 1103515245*seed+12345;
	return seed>>15;
}
class Ship:public Drawable_Circle{
public:
//	static Move_t bulletToShip;
//	static const int InjuryPerHurt = 5;
	int score;
	int gunColdTime;
	Ship():score(200),gunColdTime(0) {
		location = { 64,64 };
		size = { 20,20 };
		my_color = ColorTable::Blue ;

		//
		r = 10;
	}
};

class Score:public Drawable_Script{
public:
	int score;
	int player;

	void update(int num){
		score = std::max(0, std::min(999, num));
		sprintf(line, "p%1d:%3d", player, score);
//		xil_printf("p%1d:%3d\n", player, score);

	}
	Score(): score(200), player(0){
		sprintf(line, "p%d:%3d", player%10, score);
		location = {0, 0};
		size = {16, 48};
		my_color = ColorTable::White;
	}
};

//Move_t Ship::bulletToShip = {0,3};

class Bullet:public drawable{
public:
//	static Loc_t bulletSize ; // initialize it !
//	static Move_t bulletSpeed; // initialize it !

	bool active;
	int who_send_me;
	Bullet():active(false) {
		size = {10,5};
		my_color = ColorTable::Red;
	};
};
// Loc_t Bullet::bulletSize = {5,10};
// Move_t Bullet::bulletSpeed = {0,5};
inline FLoc_t norm(FLoc_t dir)
{
	float dis = dir.x*dir.x + dir.y*dir.y;
	dis = 1./sqrtf(dis);
	return {dir.x*dis,dir.y*dis};
}

class Enemy:public Drawable_Rotate_Multi<3>{
public:
	bool active;
	bool time_out;
	int inner_time;
	int score;
	int hurt;
	FLoc_t floc;
	FLoc_t speed;
	float inv_mass;
	void proceed(Loc_t target){
		FLoc_t reletive = { (float)target.x - floc.x,(float)target.y - floc.y};
		reletive = norm(reletive);
		speed += reletive * (0.2*inv_mass);
		point_to = norm(speed);
		floc += speed;

		location.x = (int)floc.x;
		location.y = (int)floc.y;
		rotate();

	}
	Enemy():Drawable_Rotate_Multi<3>(Triangle_Shape),active(false) {
		size = { 5,10 };
		my_color = ColorTable::Red;
		time_out = true;
	};
	void Renew(Loc_t place){
		active = true;
		inner_time = 0;
		point_to = {1,0};
		floc.x = place.x;
		floc.y = place.y;
		speed.x = -1;
		speed.y =0 ;

	}
};

inline void plot_vertex (FLoc_t loc)
{
	xil_printf("<%d,%d>",(int)loc.x,(int)loc.y);
}


class Game{
public:
	Region_t back_ground;
	Ship ships[1];
	Score scores[1];
	my_ring_buffer<32,Bullet> bullets;
	my_ring_buffer<64,Enemy> enemies;
	struct{
		int cold_time=2;
		int x=0;
	}make_enemy;
	struct {
		Move_t move;
		bool fire;
		int time_past;
	}input;



	Game() {
		make_enemy.cold_time = 0;
		ships[0].gunColdTime = 0;
		back_ground = { {0,M*WX},{0,N*WY} };
		input.time_past = 1;


		// shared varaible : edit once is enough
		enemies.data[0].St_vertex[0] = FLoc_t{20,0};
		enemies.data[0].St_vertex[1] = FLoc_t{0,3};
		enemies.data[0].St_vertex[2] = FLoc_t{0,-3};


	};

	void run(){

		//ship
		for (int ship_id : {0})
		{
			auto &one = ships[ship_id];
			//Ship may move
			Move_t should_move = input.move; // edit it
			one.location += should_move;
			Region_t region1 = one.get_region();
			if(!back_ground.contain(region1)) // run out of back ground
			{
				Region_t region2 = region1.join(back_ground);
				Move_t fix_move = {region2.x.first+region2.x.second - region1.x.first - region1.x.second,
						region2.y.first+region2.y.second - region1.y.first - region1.y.second};
				one.location += fix_move;

			}

			//Ship may fire
			bool commandFire = input.fire; // edit it
			if(commandFire && one.gunColdTime <=0 && !bullets.full())
			{
				one.gunColdTime = 0; // edit it
				Bullet& new_bullet = bullets.push_fast();
				new_bullet.active = true;
 				new_bullet.location = one.location + Move_t{15,0};//Ship::bulletToShip;
			}
			one.gunColdTime = std::max(0, one.gunColdTime - input.time_past);
			//Ship may hurt
			Region_t region = one.get_region();
			for(auto &oneEnemy:enemies)
			{
				if(oneEnemy.active == false) continue;
				if(region.overlap(oneEnemy.get_region()))
				{
					oneEnemy.active = false;
					one.score -= oneEnemy.hurt;
					if(one.score < 0) one.score = 0;
				}
			}
			scores[ship_id].update(one.score);
		}

		//bullet
		for(auto &one:bullets)
		{
			if(one.active == false) continue;
			one.location += {10,0};//Bullet::bulletSpeed;
			if(back_ground.overlap(one.get_region()) == false)
			{
				one.active = false;

			}
		}
		clear_inactive(bullets);
//
//		//enemy = create + fly + fly-out-of-box
		if(make_enemy.cold_time<=0 && !enemies.full())
		{

			make_enemy.cold_time = 10;
			//make one more enemy
			Enemy& new_one = enemies.push_fast();
			int x_choice = my_rand()%2;
			int x_loc_to_be = x_choice?(back_ground.x.second -5):5;
			new_one.Renew({ x_loc_to_be ,my_rand()%(back_ground.y.second) });
			new_one.speed = new_one.speed * ((float)(my_rand()%10) /(x_choice?2.:-2.));

			new_one.inv_mass = (float)(my_rand()%10+5)/7.;
			bool is_friend = my_rand()%5 == 0;
			if(is_friend)
			{
				new_one.score = -2;
				new_one.hurt =  - 15;
				new_one.my_color = ColorTable::Red;
			} else
			{
				new_one.score = 2;
				new_one.hurt = 5;
				new_one.my_color = ColorTable::Black;
			}



		}
		make_enemy.cold_time -= input.time_past;

		for(auto&one:enemies)
		{
			//enemy may fly
			if(one.active == false) continue;
			one.proceed(ships[0].location + Move_t{ships[0].size.x/2,ships[0].size.y/2});
			Region_t region = one.get_region();
			if(one.time_out && !back_ground.overlap(region))
			{
				one.active = false;
				continue;
			}
			//enemy may hurt : go through all
			for(auto &one_bullet:bullets)
			{
				if(one_bullet.active == false) continue;
				if(region.overlap(one_bullet.get_region()))
				{
					one_bullet.active = false;
					one.active = false;
					ships[one_bullet.who_send_me].score += one.score;
					break;
				}
			}

		}
		clear_inactive(enemies);
	}

	void draw_grid(grid_net &grid) {
		for (auto &one : ships)
			one.with_grids(grid);
		for (auto &one : bullets)
			if(one.active)
				one.with_grids(grid);
		for (auto &one : enemies)
			if(one.active)
				one.with_grids(grid);
		for (auto &one: scores)
			one.with_grids(grid);
	}
};


extern grid_buffer out_buffer;
extern grid_net grid;
void fill_a_grid_buffer(grid_buffer &buffer,color_type color);
void trans_grid(int i, int j, grid_buffer& buffer);
void general_display(grid_net & grid);
void general_display_limit(grid_net & grid, int limit);
#endif /* SRC_GAME_H_ */
