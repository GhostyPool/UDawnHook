#pragma once
#include <stddef.h>

#define VALIDATE_SIZE(struc, size) static_assert(sizeof(struc) == size, "Invalid structure size of " #struc)
#define VALIDATE_OFFSET(struc, member, offset) \
	static_assert(offsetof(struc, member) == offset, "The offset of " #member " in " #struc " is not " #offset "...")

#ifdef _DEBUG
	#define DEBUG_LOG(func, msg, ...) eLog::Message(func, msg __VA_OPT__(,) __VA_ARGS__)
#else
	#define DEBUG_LOG(func, msg, ...) ((void)0)

#endif

inline int g_MHStatus = -1;