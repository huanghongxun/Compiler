#pragma once

#include "built_in_function.h"

namespace compiler
{
	class built_in_putchar : public built_in_function_1<int, int>
	{
	public:
		built_in_putchar();
	};

	class built_in_assert : public built_in_function_1<void, int>
	{
		static void assert0(int condition);
	public:
		built_in_assert();
	};

	class built_in_malloc : public built_in_function_1<void*, size_t>
	{
	public:
		built_in_malloc();
	};

	class built_in_free : public built_in_function_1<void, void*>
	{
	public:
		built_in_free();
	};
}