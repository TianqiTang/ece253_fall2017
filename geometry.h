/*
 * geometry.h
 *
 *  Created on: 2018Äê1ÔÂ8ÈÕ
 *      Author: hasee
 */

#ifndef SRC_GEOMETRY_H_
#define SRC_GEOMETRY_H_

#include <utility>
#include <algorithm>
#include <math.h>
template <typename T>
class TRange{
public:
	using value_type = T;
	T first,second; //exclusive : i.e. [1,2,3,4], the range is (1,5)
	bool is_empty()const{return first>=second;}
	bool non_empty()const{return first<second;}

	static bool contain(const TRange<T> & range,const T &point)
	{return range.first <= point && point < range.second;}
	bool contain(T point) const {return first<=point && point<second;}
	bool contain(const TRange &op2) const {return first<=op2.first && second >= op2.second;}
	static TRange join(const TRange &op1,const TRange &op2)
	{return {std::max(op1.first,op2.first),std::min(op1.second,op2.second)};}
	TRange join(const TRange &op2) const {return join(*this,op2);}
	static TRange unions(const TRange &op1,const TRange &op2)
	{return {std::min(op1.first,op2.first),std::max(op1.second,op2.second)};}
	TRange unions(const TRange &op2) const {return unions(*this,op2);}
	TRange operator+(const T& op2) const {return {first+op2,second+op2};}
	TRange operator-(const T& op2) const {return {first-op2,second-op2};}

};
template<typename T>
class T2D{
public:
	T x,y;
	T2D operator+ (const T2D &op2)const	{return {x+op2.x,y+op2.y};}
	T2D& operator+= (const T2D &op2) {x+=op2.x;y+=op2.y;return *this;}
	T2D operator- (const T2D &op2)const	{return {x-op2.x,y-op2.y};}
	T2D operator* (const T &op2)const	{return {x*op2,y*op2};}
};
template<typename T>
class T2D<TRange<T>>{
public:
	using RType = T2D<TRange<T>>;
	using PType = T2D<T>;
	TRange<T> x,y;

	bool is_empty()const {return x.is_empty() || y.is_empty();}
	bool contain(const T2D<T> &point)const{return x.contain(point.x) && y.contain(point.y);}
	T2D<TRange<T>> join(const T2D<TRange<T>>& op2)const{
		return {x.join(op2.x),y.join(op2.y)};
	}
	bool contain(const T2D<TRange<T>>& op2)const{return x.contain(op2.x) && y.contain(op2.y);}
	bool overlap(const T2D<TRange<T>>& op2)const{return !(join(op2).is_empty());}
	T2D<TRange<T>> operator+(const T2D<T>& op2)const{return {x+op2.x,y+op2.y};}
	T2D<TRange<T>> operator-(const T2D<T>& op2)const{return {x-op2.x,y-op2.y};}
};
using Range_t = TRange<int>;
using Region_t = T2D<Range_t>;
using Loc_t = T2D<int>;
using Move_t = T2D<int>;
using Speed_t = T2D<float>;
using FLoc_t = T2D<float>;
using FRange_t = TRange<float>;



inline Loc_t move(Loc_t loc, Move_t move)
{
	return {loc.x+move.x,loc.y+move.y};
}

class Geometry_Rect{
public:
	Loc_t location;
	/*const*/ Loc_t size;

	Region_t get_region(){
		return {{location.x,location.x+size.x},{location.y,location.y+size.y}};
	}

};



#endif /* SRC_GEOMETRY_H_ */
