#pragma once

#include <string>
#include <typeindex>
#include "func.h"
#include "bytecode.h"
#include "bytecode_appender.h"
#include "bytecode_generator.h"
#include "bytecode_generators.h"

namespace compiler
{
	class bytecode_function_generator : public bytecode_appender
	{
		bytecode_generator *generator;
		vector<variables_layer> variables;
		size_t var_size = 0, max_variable_size = 0;
		func_ptr f;
		bool for_global = false;
		variables_layer *global_vars;

		int append(const instruction_ptr &inst) override;
		void modify(int index, const instruction_ptr &inst) override;
	public:
		bytecode_function_generator(bytecode_generator *generator);
		bytecode_function_generator(bytecode_generator *generator, variables_layer *global_vars);

		void set_global();

		void build_expression(AST ast);
		void build_statements(AST ast);

		func_ptr build(AST ast);

		size_t get_constant_string_pool_ptr() override;
		void increase_constant_string_pool_ptr(size_t offset) override;
		local_variable_table &get_constant_string_pool() override;
		vector<variables_layer> &get_variables() override;
		variables_layer *get_global_variables() override;
		size_t get_current_instruction_number() override;
		program_ptr get_program() override;
	};
}