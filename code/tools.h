#pragma once 
//These are the tools that editors can use. Should be render agnostic
#include "core.h"
/*
	Flood fill-like algorithm
*/
void FloodFill(Color * colors, int stride, const Rect & region, const Color & newColor, int x, int y );
