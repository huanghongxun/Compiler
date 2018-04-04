#pragma once

#include <vector>
#include <map>
#include <memory>
#include "local_variable_table.h"
#include "instruction.h"
#include "syntax_tree.h"
#include "program.h"
#include "variable_layer.h"

namespace compiler
{
	using namespace std;

	class bytecode_appender
	{
	public:
		virtual int append(const instruction_ptr &inst) = 0;
		virtual void modify(int index, const instruction_ptr &inst) = 0;

		virtual size_t get_constant_string_pool_ptr() = 0;
		virtual void increase_constant_string_pool_ptr(size_t) = 0;
		virtual local_variable_table &get_constant_string_pool() = 0;
		virtual vector<variables_layer> &get_variables() = 0;
		virtual variables_layer *get_global_variables() = 0;

		virtual void build_expression(AST ast) = 0;
		virtual void build_statements(AST ast) = 0;
		virtual size_t get_current_instruction_number() = 0;

		virtual program_ptr get_program() = 0;
	};
}

