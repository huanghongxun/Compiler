#pragma once

#include <stdexcept>

void assert_cond(bool x);

template<typename A, typename B>
void assert_eq(const A &x, const B &y)
{
#ifndef NDEBUG
	if (x == y);
	else throw std::runtime_error("Equivalent Assertion failed");
#endif
}
