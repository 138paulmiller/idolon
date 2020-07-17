#pragma once 
//These are the tools that editors can use. Should be render agnostic
#include "core.hpp"

void LineBresenham( Color* colors, int stride, int x1, int y1, int x2, int y2, const Color& color );

/*
	Flood fill-like algorithm
*/
void FloodFill(Color * colors, int stride, const Rect & region, const Color & newColor, int x, int y );
