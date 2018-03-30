#include "bytecode.h"

compiler::bytecode::bytecode(local_variable_table&& constant_string_pool, std::map<std::string, func_ptr> mp, size_t global_pool)
	: constant_string_pool(forward<local_variable_table>(constant_string_pool)), funcs(mp), global_pool(global_pool)
{
}

compiler::func_ptr compiler::bytecode::get_func(const std::string & name)
{
	return funcs[name];
}
