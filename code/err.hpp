#pragma once

#include <cassert>

//Runtime debugging
#define DEBUG_RT 
//Throw assertions. 
#define DEBUG_ASSERT 

#ifdef DEBUG_ASSERT 
#define ASSERT(cond,...) \
{	\
	if (!(cond)) { printf((__VA_ARGS__)); } \
	assert(cond);\
}
#else 
#define ASSERT(cond,...) 
#endif

namespace Err
{
	void Stacktrace();
}


