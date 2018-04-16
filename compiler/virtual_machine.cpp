#include "virtual_machine.h"
#include "built_in_functions.h"
#include "function_environment.h"
#include "unit_assert.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>

void compiler::virtual_machine::init_built_in_func()
{
	built_in_func["assert"] = shared_ptr<built_in_function>(new built_in_assert());
	built_in_func["putchar"] = shared_ptr<built_in_function>(new built_in_function_1<int, int>(putchar));
	built_in_func["malloc"] = shared_ptr<built_in_function>(new built_in_function_1<void*, size_t>(malloc));
	built_in_func["free"] = shared_ptr<built_in_function>(new built_in_function_1<void, void*>(free));
	built_in_func["memset"] = shared_ptr<built_in_function>(new built_in_function_3<void*, void*, int, size_t>(memset));
	built_in_func["memcpy"] = shared_ptr<built_in_function>(new built_in_function_3<void*, void*, void const*, size_t>(memcpy));
	built_in_func["strcmp"] = shared_ptr<built_in_function>(new built_in_function_2<int, const char*, const char*>(strcmp));
	built_in_func["strlen"] = shared_ptr<built_in_function>(new built_in_function_1<size_t, const char*>(strlen));
	built_in_func["puts"] = shared_ptr<built_in_function>(new built_in_function_1<int, const char*>(puts));
	built_in_func["system"] = shared_ptr<built_in_function>(new built_in_function_1<int, const char*>(system));
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
