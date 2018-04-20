#pragma once

#include "built_in_function.h"

namespace compiler
{
	class built_in_assert : public built_in_function_1<void, int>
	{
		static void assert0(int condition);
	public:
		built_in_assert();
	};

	class built_in_scanf : public built_in_function
	{
	public:
		object_t call(const std::vector<object_t> &vec) override;
	};

	class built_in_printf : public built_in_function
	{
	public:
		object_t call(const std::vector<object_t> &vec) override;
	};
}