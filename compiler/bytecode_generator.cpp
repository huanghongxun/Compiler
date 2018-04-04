#include "bytecode_generator.h"
#include "bytecode_generators.h"
#include "syntax_descriptors.h"
#include "instructions.h"
#include "bytecode_function_generator.h"
#include "bytecode_init_generator.h"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <any>
#include "unit_assert.h"
#include "variable_layer.h"
#include "string_utils.h"
#include "type_primitive.h"

using namespace std;
using namespace compiler;
using namespace compiler::instructions;
using namespace compiler::syntax;

compiler::bytecode_generator::bytecode_generator(program_ptr prog)
	: constant_string_pool(8 * 1024 * 1024), prog(prog)
{
	generators[type_index(typeid(descriptor_binary_operator))] = make_shared<bytecode_binary_operator>();
	generators[type_index(typeid(descriptor_assign))] = make_shared<bytecode_assign>();
	generators[type_index(typeid(descriptor_unary_operator))] = make_shared<bytecode_unary_operator>();
	generators[type_index(typeid(descriptor_func_call))] = make_shared<bytecode_func_call>();
	generators[type_index(typeid(descriptor_constant))] = make_shared<bytecode_constant>();
	generators[type_index(typeid(descriptor_var))] = make_shared<bytecode_var>();
	generators[type_index(typeid(descriptor_inc))] = make_shared<bytecode_inc>();

	generators[type_index(typeid(descriptor_primitive_cast))] = make_shared<bytecode_primitive_cast>();

	generators[type_index(typeid(descriptor_if))] = make_shared<bytecode_if>();
	generators[type_index(typeid(descriptor_while))] = make_shared<bytecode_while>();
	generators[type_index(typeid(descriptor_return))] = make_shared<bytecode_return>();
}

size_t compiler::bytecode_generator::get_constant_string_pool_ptr()
{
	return constant_string_pool_ptr;
}

void compiler::bytecode_generator::increase_constant_string_pool_ptr(size_t offset)
{
	constant_string_pool_ptr += offset;
}

local_variable_table & compiler::bytecode_generator::get_constant_string_pool()
{
	return constant_string_pool;
}

program_ptr compiler::bytecode_generator::get_program()
{
	return prog;
}

void compiler::bytecode_generator::build_static_vars(AST ast, AST init)
{
	if (ast->desc.type() == typeid(descriptor_define_local_variables))
	{
		auto desc = any_cast<descriptor_define_local_variables>(ast->desc);
		if (desc.is_static)
			init->add_children(ast);
	}
	else
		for (auto &i : ast->children)
			build_static_vars(i, init);
}

void mark_global_variable_definition(AST ast)
{
	for (auto &i : ast->children)
	{
		if (i->desc.type() == typeid(descriptor_define_local_variables))
		{
			auto desc = any_cast<descriptor_define_local_variables>(i->desc);
			desc.is_global = true;
			i->desc = desc;
		}
	}
}

bytecode compiler::bytecode_generator::build(AST ast)
{
	AST initializer = make_shared<syntax_tree>(descriptor(), token());
	mark_global_variable_definition(ast);
	build_static_vars(ast, initializer);
	bytecode_init_generator init_generator(this);
	funcs["__init__"] = init_generator.build(initializer);

	for (auto &i : ast->children)
	{
		if (i->desc.type() == typeid(descriptor_function))
			build_func(i, init_generator.get_global_variables());
		else if (i->desc.type() != typeid(descriptor_define_local_variables))
			throw runtime_error("unrecognized descriptor");
	}

	return bytecode(move(constant_string_pool), funcs, init_generator.get_global_variables()->ptr);
}

void compiler::bytecode_generator::build_func(AST ast, variables_layer *layer)
{
	auto desc = any_cast<descriptor_function>(ast->desc);

	auto func = bytecode_function_generator(this, layer).build(ast);
	if (func != nullptr)
		funcs[desc.name] = func;
}
