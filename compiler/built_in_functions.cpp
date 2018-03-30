#include "built_in_functions.h"

void compiler::built_in_assert::assert0(int condition)
{
	if (condition == 0)
	{
		throw runtime_error("Assertion failed");
	}
}

compiler::built_in_assert::built_in_assert()
	: built_in_function_1(assert0)
{
}

compiler::built_in_putchar::built_in_putchar()
	: built_in_function_1(putchar)
{
}

compiler::built_in_free::built_in_free()
	: built_in_function_1(free)
{
}

compiler::built_in_malloc::built_in_malloc()
	: built_in_function_1(malloc)
{
}
