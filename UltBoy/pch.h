#pragma once

#include <SDL.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <cassert>

#include <map>
#include <unordered_map>
#include <queue>
#include <vector>
#include <mutex>
#include <thread>
#include <condition_variable>


#define UltBoy_DEBUG 1

#if UltBoy_DEBUG 
#define ASSERT(cond,...) \
{	\
	if (!(cond)) { printf((__VA_ARGS__)); } \
	assert(cond);\
}
#else 
#define ASSERT(cond,...) 
#endif


struct Color
{
    uint8_t a;
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

struct Rect
{
    int x, y, w, h;
};