#include "virtual_machine.h"
#include "built_in_functions.h"
#include "function_environment.h"
#include "unit_assert.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>

void compiler::virtual_machine::init_built_in_func()
{
	// assert.h
	built_in_func["assert"] = shared_ptr<built_in_function>(new built_in_assert());

	// string.h
	built_in_func["memset"] = create_function(memset);
	built_in_func["memcpy"] = create_function(memcpy);
	built_in_func["strcmp"] = create_function(strcmp);
	built_in_func["strlen"] = create_function(strlen);
	built_in_func["strcat"] = create_function(strcat);
	built_in_func["strchr"] = create_function<char*, char*, int>(strchr);
	built_in_func["strrchr"] = create_function<char*, char*, int>(strrchr);
	built_in_func["strcoll"] = create_function(strcoll);
	built_in_func["strcpy"] = create_function(strcpy);
	built_in_func["strdup"] = create_function(strtok);
	built_in_func["strstr"] = create_function<char*, char*, const char*>(strstr);

	// ctype.h
	built_in_func["isalnum"] = create_function(isalnum);
	built_in_func["isalpha"] = create_function(isalpha);
	built_in_func["islower"] = create_function(islower);
	built_in_func["isupper"] = create_function(isupper);
	built_in_func["isdigit"] = create_function(isdigit);
	built_in_func["isxdigit"] = create_function(isxdigit);
	built_in_func["iscntrl"] = create_function(iscntrl);
	built_in_func["isgraph"] = create_function(isgraph);
	built_in_func["isspace"] = create_function(isspace);
	built_in_func["isblank"] = create_function(isblank);
	built_in_func["isprint"] = create_function(isprint);
	built_in_func["ispunct"] = create_function(ispunct);
	built_in_func["tolower"] = create_function(tolower);
	built_in_func["toupper"] = create_function(toupper);

	// stdlib.h
	built_in_func["malloc"] = create_function(malloc);
	built_in_func["free"] = create_function(free);
	built_in_func["system"] = create_function(system);

	// stdio.h
	built_in_func["putchar"] = create_function(putchar);
	built_in_func["puts"] = create_function(puts);
	built_in_func["printf"] = shared_ptr<built_in_function>(new built_in_printf());
	built_in_func["scanf"] = shared_ptr<built_in_function>(new built_in_scanf());
}

compiler::virtual_machine::virtual_machine(const bytecode & code)
	: code(code), global(code.global_pool)
{
	init_built_in_func();
}

compiler::virtual_machine::virtual_machine(bytecode && code)
	: code(forward<bytecode>(code)), global(code.global_pool)
{
	init_built_in_func();
}

void compiler::virtual_machine::init()
{
	global.reset();
	execute_function("__init__", vector<object_t>{});
}

object_t compiler::virtual_machine::execute_function(const string & func_name, const vector<object_t>& args)
{
	auto func = code.get_func(func_name);

	if (func->is_built_in)
	{
		if (built_in_func.count(func_name))
			return built_in_func[func_name]->call(args);
		else
			throw built_in_function_not_found(func_name);
	}

	assert_cond(args.size() >= func->fixed_arg_size);

	size_t arg_size = 0;

	for (auto &o : args)
		arg_size += o.get_raw_memory_size();

	local_variable_table table(func->local_size + arg_size);
	operand_stack stack;

	function_environment env(this, &table, &global, &stack);

	size_t ind = func->local_size;
	for (size_t i = 0; i < args.size(); ++i)
	{
		table.set(ind, args[i].get_raw_memory(), args[i].get_raw_memory_size());
		ind += args[i].get_raw_memory_size();
	}

	auto ins = func->get_instructions();
	for (size_t i = 0; i < ins.size(); )
	{
		ins[i]->operate(env);
		int jmp = env.get_jump_to();
		if (jmp >= 0)
			i = static_cast<size_t>(jmp);
		else
			++i;

		bool ret = env.get_return();
		if (ret)
		{
			auto res = func->return_type.base_type->is_void() ? object_t() : stack.pop().value;
			if (!stack.empty())
				throw runtime_error("Operand stack is not empty when returning");
			return res;
		}
	}

	throw runtime_error("never");
}

void compiler::virtual_machine::start()
{
	execute_function("main", vector<object_t> {});
}

void compiler::virtual_machine::register_built_in_func(const string &name, shared_ptr<built_in_function> bif)
{
	built_in_func[name] = bif;
}

compiler::built_in_function_not_found::built_in_function_not_found(const string & name)
	: name(name)
{
}
