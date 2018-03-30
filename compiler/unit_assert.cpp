#include "unit_assert.h"

void assert_cond(bool x)
{
#ifndef NDEBUG
	if (!x)
		throw std::runtime_error("Assertion failed");
#endif
}
