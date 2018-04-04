#pragma once

#include <map>
#include <string>
#include <typeindex>
#include "func.h"
#include "bytecode.h"
#include "bytecode_appender.h"
#include "bytecode_generators.h"

namespace compiler
{
	using namespace std;

	class bytecode_generator
	{
		program_ptr prog;

		size_t constant_string_pool_ptr = 0;
		local_variable_table constant_string_pool;
		map<string, func_ptr> funcs;

	public:
		map<type_index, shared_ptr<bytecode_generator_base>> generators;

		bytecode_generator(program_ptr prog);

		void build_static_vars(AST ast, AST init);
		bytecode build(AST ast);
		void build_func(AST ast, variables_layer *layer);

		size_t get_constant_string_pool_ptr();
		void increase_constant_string_pool_ptr(size_t offset);
		local_variable_table &get_constant_string_pool();
		program_ptr get_program();
	};
}

