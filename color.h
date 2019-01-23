/*
 * color.h
 *
 *  Created on: 2018Äê1ÔÂ9ÈÕ
 *      Author: hasee
 */

#ifndef SRC_COLOR_H_
#define SRC_COLOR_H_
#include <stdint.h>

using Color_Type = uint16_t;

constexpr inline Color_Type FromRGB(uint8_t r, uint8_t g, uint8_t b)
	{

	Color_Type rl = (r & 0xF8);
	Color_Type gl = (g & 0xFC);
	Color_Type bl = (b & 0xF8);
		return rl<<8 | gl<<3 | bl>>3;
	}
class ColorTable{
public:
	static const Color_Type Red = FromRGB(255,0,0);
	static const Color_Type Blue = FromRGB(0,0,255);
	static const Color_Type Green = FromRGB(0,255,0);
	static const Color_Type Black = FromRGB(0,0,0);
	static const Color_Type Gray = FromRGB(127,127,127);
	static const Color_Type White = FromRGB(255,255,255);
};



#endif /* SRC_COLOR_H_ */
