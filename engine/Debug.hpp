#pragma once

#include <assert.h>


#define Assert(b, msg) _ASSERT_EXPR(b, L#msg)
#define AssertFail(msg) _ASSERT_EXPR(false, L#msg)