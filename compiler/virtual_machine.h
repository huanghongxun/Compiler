#pragma once

#include "bytecode.h"
#include "built_in_function.h"
#include "function_executor.h"

namespace compiler
{
	class built_in_function_not_found : public exception
	{
	public:
		const string name;

		built_in_function_not_found(const string &name);
	};

	class virtual_machine : public function_executor
	{
		bytecode code;
		map<string, shared_ptr<built_in_function>> built_in_func;
		local_variable_table global;

		void init_built_in_func();
	public:
		virtual_machine(const bytecode &code);
		virtual_machine(bytecode&& code);

		void init();

		object_t execute_function(const string &func_name, const std::vector<object_t> & args) override;

		void start();

		void register_built_in_func(const string &name, shared_ptr<built_in_function> bif);
	};
}

