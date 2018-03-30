#include "function_environment.h"

#include "unit_assert.h"

compiler::function_environment::function_environment(function_executor * const vm,
	local_variable_table * const local,
	local_variable_table * const global,
	operand_stack * const stack)
	: vm(vm), local(local), global(global), stack(stack)
{
}

void compiler::function_environment::jump(int line_index)
{
	assert_cond(0 <= line_index);
	this->jump_to = line_index;
}

void compiler::function_environment::mark_return()
{
	ret = true;
}

int compiler::function_environment::get_jump_to()
{
	int res = jump_to;
	jump_to = -1;
	return res;
}

bool compiler::function_environment::get_return()
{
	bool res = ret;
	ret = false;
	return res;
}

