#pragma once

#include "function_executor.h"
#include "local_variable_table.h"
#include "operand_stack.h"

namespace compiler
{
	class function_environment
	{
		int jump_to = -1;
		bool ret = false;
	public:
		function_environment(function_executor *const vm, local_variable_table *const local, local_variable_table *const global, operand_stack *const stack);

		function_executor* const vm;
		local_variable_table* const local;
		local_variable_table* const global;
		operand_stack* const stack;

		void jump(int line_index);

		void mark_return();

		int get_jump_to();

		bool get_return();
	};
}
