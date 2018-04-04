#include "bytecode_function_generator.h"
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

int compiler::bytecode_function_generator::append(const instruction_ptr& inst)
{
	f->get_instructions().push_back(inst);
	return f->get_instructions().size() - 1;
}

void compiler::bytecode_function_generator::modify(int index, const instruction_ptr& inst)
{
	f->get_instructions()[index] = inst;
}

compiler::bytecode_function_generator::bytecode_function_generator(bytecode_generator *generator, variables_layer *global)
	: generator(generator), global_vars(global), f(new func)
{
}

void compiler::bytecode_function_generator::build_expression(AST ast)
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

void compiler::bytecode_function_generator::build_statements(AST ast)
{
	assert_eq(ast->desc.type(), typeid(descriptor_statements));

	variables.emplace_back(variables.back().ptr);

	for (auto &i : ast->children)
	{
		if (i->desc.type() == typeid(descriptor_statements))
			build_statements(i);
		else if (i->desc.type() == typeid(descriptor_define_local_variables))
		{
			auto local_desc = any_cast<descriptor_define_local_variables>(i->desc);
			for (auto &j : i->children)
			{
				auto desc = any_cast<descriptor_define_variable>(j->desc);
				auto &layer = variables.back();

				// create a new variable
				// do not create static variables in function.
				if (!desc.type.is_static)
				{
					desc.ptr = layer.ptr;
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

					layer.ptr += sz;
					max_variable_size = max(max_variable_size, layer.ptr);
				}

				// register variable whatever static or non-static variables.
				layer.variables[desc.name] = { desc.ptr, desc.type };
			}
		}
		else if (i->desc.type() != typeid(descriptor_function))
		{
			build_expression(i);

			// TODO: pop elements in no need
			if (i->type.base_type != nullptr && !i->type.base_type->is_void())
				append(instruction_ptr(new instruction_pop()));
		}
	}

	variables.pop_back();
}

size_t compiler::bytecode_function_generator::get_constant_string_pool_ptr()
{
	return generator->get_constant_string_pool_ptr();
}

void compiler::bytecode_function_generator::increase_constant_string_pool_ptr(size_t offset)
{
	generator->increase_constant_string_pool_ptr(offset);
}

local_variable_table & compiler::bytecode_function_generator::get_constant_string_pool()
{
	return generator->get_constant_string_pool();
}

vector<compiler::variables_layer>& compiler::bytecode_function_generator::get_variables()
{
	return variables;
}

compiler::variables_layer * compiler::bytecode_function_generator::get_global_variables()
{
	return global_vars;
}

size_t compiler::bytecode_function_generator::get_current_instruction_number()
{
	return f->get_instructions().size();
}

compiler::program_ptr compiler::bytecode_function_generator::get_program()
{
	return generator->get_program();
}

func_ptr compiler::bytecode_function_generator::build(AST ast)
{
	auto desc = any_cast<descriptor_function>(ast->desc);

	f->fixed_arg_size = desc.args.size();
	f->return_type = desc.type;

	if (ast->children.empty()) // declaration
		if (desc.type.is_built_in)
		{
			f->is_built_in = true;
			return f;
		}
		else
			return func_ptr();

	assert_eq(ast->children.size(), 1);

	variables.emplace_back(0);

	auto &indexes = f->get_arg_indexes();
	for (auto &i : desc.args)
	{
		auto &layer = variables.back();

		indexes.push_back(layer.ptr);
		layer.variables[i.second] = { layer.ptr, i.first };
		layer.ptr += i.first.base_type->size();
	}

	build_statements(ast->children[0]);

	if (f->return_type.base_type->is_void())
		append(instruction_ptr(new instruction_return()));

	f->local_size = max_variable_size;

	variables.pop_back();
	return f;
}