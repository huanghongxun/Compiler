#include "bytecode_init_generator.h"
#include "bytecode_generators.h"
#include "syntax_descriptors.h"
#include "instructions.h"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <any>
#include "variable_layer.h"
#include "unit_assert.h"
#include "string_utils.h"

using namespace std;
using namespace compiler;
using namespace compiler::instructions;
using namespace compiler::syntax;

int compiler::bytecode_init_generator::append(const instruction_ptr& inst)
{
	f->get_instructions().push_back(inst);
	return f->get_instructions().size() - 1;
}

void compiler::bytecode_init_generator::modify(int index, const instruction_ptr& inst)
{
	f->get_instructions()[index] = inst;
}

compiler::bytecode_init_generator::bytecode_init_generator(bytecode_generator *generator)
	: generator(generator), global_vars(0), f(new func)
{
}

void compiler::bytecode_init_generator::build_expression(AST ast)
{
	auto index = type_index(ast->desc.type());

	if (generator->generators.count(index))
	{
		generator->generators[index]->build(*this, ast);
	}
	else
	{
		assert_cond(0);
	}
}

void compiler::bytecode_init_generator::build_statements(AST ast)
{
	for (auto &i : ast->children)
	{
		auto local_desc = any_cast<descriptor_define_local_variables>(i->desc);
		for (auto &j : i->children)
		{
			auto desc = any_cast<descriptor_define_variable>(j->desc);
			assert_cond(desc.type.is_static);

			// create a new variable
			desc.ptr = global_vars.ptr;
			j->desc = desc;

			if (j->children.size() == 1) // initialize the new variable
			{
				append(get_instruction_load(desc.type, desc.ptr));

				build_expression(j->children[0]);
				append(instruction_ptr(new instruction_store()));
				append(instruction_ptr(new instruction_pop()));
			}

			size_t sz = desc.type.base_type->size();
			for (auto &t : desc.dim)
			{
				assert_eq(t->desc.type(), typeid(descriptor_constant));
				sz *= boost::lexical_cast<size_t>(any_cast<descriptor_constant>(t->desc).value);
			}

			global_vars.ptr += sz;

			if (local_desc.is_global)
				global_vars.variables[desc.name] = { desc.ptr, desc.type };
		}
	}
}

size_t compiler::bytecode_init_generator::get_constant_string_pool_ptr()
{
	return generator->get_constant_string_pool_ptr();
}

void compiler::bytecode_init_generator::increase_constant_string_pool_ptr(size_t offset)
{
	generator->increase_constant_string_pool_ptr(offset);
}

local_variable_table & compiler::bytecode_init_generator::get_constant_string_pool()
{
	return generator->get_constant_string_pool();
}

vector<compiler::variables_layer>& compiler::bytecode_init_generator::get_variables()
{
	throw runtime_error("Should not be called");
}

compiler::variables_layer * compiler::bytecode_init_generator::get_global_variables()
{
	return &global_vars;
}

size_t compiler::bytecode_init_generator::get_current_instruction_number()
{
	return f->get_instructions().size();
}

compiler::program_ptr compiler::bytecode_init_generator::get_program()
{
	return generator->get_program();
}

func_ptr compiler::bytecode_init_generator::build(AST statements)
{
	f->fixed_arg_size = 0;
	f->return_type = type_void;

	build_statements(statements);

	append(instruction_ptr(new instruction_return()));

	f->local_size = 0;
	return f;
}
