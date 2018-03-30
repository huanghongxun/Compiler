#pragma once

#include <vector>
#include <map>
#include "func.h"

namespace compiler
{
	using namespace std;

	class bytecode
	{
		local_variable_table constant_string_pool;
		map<string, func_ptr> funcs;
	public:
		bytecode(local_variable_table&& constant_string_pool, map<string, func_ptr> funcs, size_t global_pool);
		
		const size_t global_pool;

		func_ptr get_func(const std::string &name);
	};
}