#include "semantic_analyzer.h"
#include "unit_assert.h"
#include "syntax_descriptors.h"
#include "string_utils.h"
#include "type_primitive.h"
#include "type_array.h"
#include "type_pointer.h"
#include "syntax_tree_evaluation.h"
using namespace compiler;
using namespace compiler::syntax;

AST cast_node(AST ast, type_representation from_type, type_representation to_type)
{
	AST cast_ast(new syntax_tree(descriptor_primitive_cast(to_type, from_type), ast->t));
	cast_ast->type = to_type;
	cast_ast->add_children(ast);
	return cast_ast;
}

AST assign_cast_node(AST ast, type_representation from_type, type_representation to_type)
{
	AST cast_ast(new syntax_tree(descriptor_primitive_cast(to_type, from_type), ast->t));
	cast_ast->type = to_type;
	cast_ast->add_children(ast);
	return cast_ast;
}

compiler::semantic_analyzer::semantic_analyzer(program_ptr prog)
	: prog(prog)
{
}

compiler::type_representation compiler::semantic_analyzer::deduce_type(AST ast)
{
	if (ast->desc.type() == typeid(descriptor_statements) || ast->desc.type() == typeid(descriptor))
	{
		variables.emplace_back();

		for (auto &i : ast->children)
		{
			deduce_type(i);
			i->discard_result = true;
		}

		variables.pop_back();
		return type_representation();
	}
	else if (ast->desc.type() == typeid(descriptor_function))
	{
		auto desc = any_cast<descriptor_function>(ast->desc);
		if (defined_functions.count(desc.name) && ast->children.size() == 1) // definition
			prog->compilation_error(ast->t, "Function %s has been already defined.", desc.name.c_str());

		if (declared_functions.count(desc.name))
		{
			auto sig = declared_functions[desc.name];
			if (sig->signature != desc.signature)
				prog->compilation_error(ast->t, "Function %s has been redeclared, but signature does not match", desc.name.c_str());
		}
		else
			declared_functions[desc.name] = make_shared<descriptor_function>(desc);

		if (!ast->children.empty()) // definition
		{
			defined_functions.insert(desc.name);
		}

		parameters.emplace_back();
		variables.emplace_back();
		for (auto &i : desc.args)
		{
			parameters.back().insert(i.second);
			variables.back()[i.second] = i.first;
		}
		
		assert_cond(ast->children.size() <= 1);
		if (ast->children.size() == 1)
			deduce_type(ast->children[0]);

		parameters.pop_back();
		variables.pop_back();

		return type_representation();
	}
	else if (ast->desc.type() == typeid(descriptor_define_local_variables))
	{
		for (auto &j : ast->children)
		{
			auto desc = any_cast<descriptor_define_variable>(j->desc);

			if (!desc.dim.empty())
			{
				vector<size_t> dim;
				for (auto &k : desc.dim)
				{
					auto type = deduce_type(k);
					if (!type.base_type->is_integral())
						prog->compilation_error(j->t, "Only integral allowed declaring array.");

					try
					{
						object_t res = evaulate_expression(k);
						dim.push_back((size_t)res.cast_to_integral());
					}
					catch (expression_evaulation_error&)
					{
						prog->compilation_error(j->t, "Not a constant expression");
					}
				}

				auto old_type = desc.type;
				desc.type = type_representation{ create_array_type(old_type.base_type, dim), old_type.is_const, old_type.is_static };

				j->desc = desc;
			}

			auto &layer = variables.back();
			if (layer.count(desc.name))
				prog->compilation_error(j->t, "Variable %s has been already defined.", desc.name.c_str());
			if (!parameters.empty() && parameters.back().count(desc.name))
				prog->compilation_error(j->t, "Variable %s shadows a parameter.", desc.name.c_str());

			layer[desc.name] = desc.type;

			if (j->children.size() == 1)
			{
				auto type = deduce_type(j->children[0]);
				if (!desc.type.base_type->is_assignable_from(type.base_type))
					prog->compilation_error(j->t, "Type %s does not accept value type %s", desc.type.base_type->id.c_str(), type.base_type->id.c_str());

				if (*desc.type.base_type != *type.base_type)
					j->children[0] = assign_cast_node(j->children[0], type, desc.type);
			}

		}
		return type_representation();
	}
	else if (ast->desc.type() == typeid(descriptor_binary_operator))
	{
		auto desc = any_cast<descriptor_binary_operator>(ast->desc);

		auto ltype = deduce_type(ast->children[0]);
		auto rtype = deduce_type(ast->children[1]);

		if (desc.op == ",")
		{
			ast->children[0]->discard_result = true;
			return ast->type = rtype;
		}
		else
		{

			if (ltype.base_type->is_array())
			{
				type_array *arr = dynamic_cast<type_array*>(ltype.base_type.get());
				ltype.base_type = arr->to_pointer();
			}

			if (ltype.base_type->is_pointer())
			{
				if (!rtype.base_type->is_integral())
					prog->compilation_error(ast->t, "Only integral is accepted here.");
				else if (desc.op != "+" && desc.op != "-")
					prog->compilation_error(ast->t, "Pointers can only add or subtract.");
				else
				{
					ast->type = ltype;
					return ast->type;
				}
			}
			else
			{
				auto type = ltype.base_type->implicit_cast_with(ltype.base_type, rtype.base_type);
				auto typer = type_representation(type, ltype.is_const || rtype.is_const, false);
				if (type == nullptr)
					prog->compilation_error(ast->t, "Type %s and type %s are incompatible", ltype.base_type->id.c_str(), rtype.base_type->id.c_str());
				else
				{
					ast->type = type_representation(type);
					if (*ltype.base_type != *type)
						ast->children[0] = cast_node(ast->children[0], ltype, typer);
					if (*rtype.base_type != *type)
						ast->children[1] = cast_node(ast->children[1], rtype, typer);
					return ast->type;
				}
			}
		}
	}
	else if (ast->desc.type() == typeid(descriptor_assign))
	{
		auto ltype = deduce_type(ast->children[0]);
		auto rtype = deduce_type(ast->children[1]);
		if (ltype.is_const)
			prog->compilation_error(ast->t, "Const var is not a lvalue.");
		if (!ltype.base_type->is_assignable_from(rtype.base_type))
			prog->compilation_error(ast->t, "Type %s can not be assigned from type %s", ltype.base_type->id.c_str(), rtype.base_type->id.c_str());

		if (*ltype.base_type != *rtype.base_type)
			ast->children[1] = assign_cast_node(ast->children[1], rtype, ltype);

		return ast->type = ltype;
	}
	else if (ast->desc.type() == typeid(descriptor_unary_operator))
	{
		auto desc = any_cast<descriptor_unary_operator>(ast->desc);
		auto type = deduce_type(ast->children[0]);
		if (desc.op == "~" || desc.op == "!")
		{
			if (!type.base_type->is_integral())
				prog->compilation_error(ast->t, "%s requires an integral as its operand", desc.op.c_str());
			else
				ast->type = type;
		}
		else if (desc.op == "*")
		{
			if (type.base_type->is_array())
			{
				type_array *arr = dynamic_cast<type_array*>(type.base_type.get());
				type.base_type = arr->to_pointer();
			}

			if (type.base_type->is_pointer())
			{
				auto p = dynamic_cast<type_pointer*>(type.base_type.get());
				ast->type = type_representation(p->get_base_type(), p->is_const(), false);
			}
			else
				prog->compilation_error(ast->t, "Only pointers can be dereferenced.");
		}
		else if (desc.op == "&")
			ast->type = wrap_pointer(type);
		assert_cond(ast->type.base_type != nullptr);
		return ast->type;
	}
	else if (ast->desc.type() == typeid(descriptor_var))
	{
		auto desc = any_cast<descriptor_var>(ast->desc);
		for (auto it = variables.rbegin(); it != variables.rend(); ++it)
			if ((*it).count(desc.name))
			{
				for (size_t i = 0; i < ast->children.size(); ++i)
				{
					auto type = deduce_type(ast->children[i]);
					if (!type.base_type->is_integral())
						prog->compilation_error(ast->t, "Only integral allowed accessing array elements");
				}

				return ast->type = (*it)[desc.name];
			}
		prog->compilation_error(ast->t, "Variable %s is undefined", desc.name.c_str());
	}
	else if (ast->desc.type() == typeid(descriptor_sizeof))
	{
		assert_eq(ast->children.size(), 1);

		ast->desc = descriptor_constant(to_string(deduce_type(ast->children[0]).base_type->size()));
		ast->children.clear();

		return ast->type = type_int;
	}
	else if (ast->desc.type() == typeid(descriptor_func_call))
	{
		auto desc = any_cast<descriptor_func_call>(ast->desc);
		if (declared_functions.count(desc.name))
		{
			auto func = declared_functions[desc.name];
			ast->type = func->type;
			if (func->is_vararg)
			{
				if (ast->children.size() < func->args.size())
					prog->compilation_error(ast->t, "Function call args do not match");
			}
			else if (ast->children.size() != func->args.size())
				prog->compilation_error(ast->t, "Function call args do not match");

			for (size_t i = 0; i < ast->children.size(); ++i)
			{
				auto type = deduce_type(ast->children[i]);

				if (i >= func->args.size())
					continue;
				if (!func->args[i].first.base_type->is_assignable_from(type.base_type))
					prog->compilation_error(ast->t, "Function call arg type does not match");

				if (*func->args[i].first.base_type != *type.base_type)
					ast->children[i] = assign_cast_node(ast->children[i], type, func->args[i].first);
			}
		}
		else
			prog->compilation_error(ast->t, "Function %s is undeclared", desc.name.c_str());

		return ast->type;
	}
	else if (ast->desc.type() == typeid(descriptor_inc))
	{
		auto type = deduce_type(ast->children[0]);
		if (!type.base_type->is_pointer() && !type.base_type->is_integral())
			prog->compilation_error(ast->t, "Self-increasing only accepts integral and pointer.");
		return ast->type = type;
	}
	else if (ast->desc.type() == typeid(descriptor_primitive_cast))
	{
		auto desc = any_cast<descriptor_primitive_cast>(ast->desc);
		desc.from_type = deduce_type(ast->children[0]);
		if (!desc.from_type.base_type->can_be_cast_to(desc.to_type.base_type))
			prog->compilation_error(ast->t, "Illegal type cast");
		ast->desc = desc;
		ast->type = desc.to_type;
		return ast->type;
	}
	else if (ast->desc.type() == typeid(descriptor_constant))
	{
		auto desc = any_cast<descriptor_constant>(ast->desc);
		if (boost::starts_with(desc.value, "\"")) // a string
		{
			return ast->type = type_base_ptr(new type_pointer(type_char, true));
		}
		else if (boost::starts_with(desc.value, "'")) // a char
		{
			return ast->type = type_char;
		}
		else if (desc.value == "true")
		{
			return ast->type = type_bool;
		}
		else if (desc.value == "false")
		{
			return ast->type = type_bool;
		}
		else
		{
			boost::to_lower(desc.value);
			if (desc.value.find("e") != string::npos)
			{
				return ast->type = type_double;
			}
			else if (desc.value.find(".") == string::npos)
			{
				if (boost::ends_with(desc.value, "ll"))
					return ast->type = type_long_long;
				else if (boost::ends_with(desc.value, "l"))
					return ast->type = type_long;
				else
					return ast->type = type_int;
			}
			else
			{
				if (boost::ends_with(desc.value, "l"))
					return ast->type = type_long_double;
				else if (boost::ends_with(desc.value, "f"))
					return ast->type = type_float;
				else
					return ast->type = type_double;
			}
		}
	}
	else if (ast->desc.type() == typeid(descriptor_if))
	{
		auto exp = deduce_type(ast->children[0]);
		if (!exp.base_type->is_integral() && !exp.base_type->is_pointer())
			prog->compilation_error(ast->t, "`if` only accept boolean expression");
		deduce_type(ast->children[1]);
		if (ast->children.size() == 3)
			deduce_type(ast->children[2]);
		return type_representation();
	}
	else if (ast->desc.type() == typeid(descriptor_while))
	{
		auto exp = deduce_type(ast->children[0]);
		if (!exp.base_type->is_integral() && !exp.base_type->is_pointer())
			prog->compilation_error(ast->t, "`while / for` only accept boolean expression");
		for (auto &i : ast->children)
			deduce_type(i);
		return type_representation();
	}
	else
	{
		for (auto &i : ast->children)
		{
			deduce_type(i);
		}
		return type_representation();
	}
}

void compiler::semantic_analyzer::check_func_definition(AST ast)
{
	if (ast->desc.type() == typeid(descriptor_func_call))
	{
		auto desc = any_cast<descriptor_func_call>(ast->desc);
		if (!defined_functions.count(desc.name))
			if (!declared_functions.count(desc.name) || !declared_functions[desc.name]->type.is_built_in)
				prog->compilation_error(ast->t, "Function %s does not be defined.", desc.name.c_str());
	}
	for (auto &i : ast->children)
		check_func_definition(i);
}

void compiler::semantic_analyzer::convert_array_access(AST &ast)
{
	if (ast->desc.type() == typeid(descriptor_array_access))
	{
		for (auto &i : ast->children)
			convert_array_access(i);

		ast->desc = descriptor_binary_operator("+");

		AST deref_node = make_shared<syntax_tree>(descriptor_unary_operator("*"), ast->t);
		deref_node->add_children(ast);

		ast = deref_node;
	}
	else
		for (auto &i : ast->children)
			convert_array_access(i);
}

void compiler::semantic_analyzer::analyze(AST ast)
{
	convert_array_access(ast);
	deduce_type(ast);
	check_func_definition(ast);
}
