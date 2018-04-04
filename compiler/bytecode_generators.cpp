#include "bytecode_generators.h"

#include "instructions.h"
#include "syntax_descriptors.h"
#include "unit_assert.h"
#include "type_primitive.h"
#include "primitive_cast.h"
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

using namespace compiler;
using namespace compiler::syntax;
using namespace compiler::instructions;

void discard_if(AST ast, bytecode_appender &appender)
{
	if (ast->discard_result && !ast->type.base_type->is_void())
		appender.append(instruction_ptr(new instruction_pop()));
}

template<typename T>
bool build_assign_common(const string &op, bytecode_appender &appender)
{
	if (op == "*=")
		appender.append(instruction_ptr(new instruction_mul<T>()));
	else if (op == "+=")
		appender.append(instruction_ptr(new instruction_add<T>()));
	else if (op == "-=")
		appender.append(instruction_ptr(new instruction_sub<T>()));
	else if (op == "/=")
		appender.append(instruction_ptr(new instruction_div<T>()));
	else
		return false;
	return true;
}

template<typename T>
bool build_assign_integral(const string &op, bytecode_appender &appender)
{
	if (build_assign_common<T>(op, appender))
		return true;
	else if (op == "&=")
		appender.append(instruction_ptr(new instruction_and<T>()));
	else if (op == "|=")
		appender.append(instruction_ptr(new instruction_or<T>()));
	else if (op == "^=")
		appender.append(instruction_ptr(new instruction_xor<T>()));
	else if (op == "<<=")
		appender.append(instruction_ptr(new instruction_shl<T>()));
	else if (op == ">>=")
		appender.append(instruction_ptr(new instruction_shr<T>()));
	else if (op == "%=")
		appender.append(instruction_ptr(new instruction_rem<T>()));
	else
		return false;
	return true;
}

bool build_assign_bool(const string &op, bytecode_appender &appender)
{
	if (op == "^=")
		appender.append(instruction_ptr(new instruction_xor<bool>()));
	else if (op == "|=")
		appender.append(instruction_ptr(new instruction_or<bool>()));
	else if (op == "&=")
		appender.append(instruction_ptr(new instruction_and<bool>()));
	else
		return false;
	return true;
}

void compiler::bytecode_assign::build(bytecode_appender & appender, AST ast)
{
	auto desc = any_cast<descriptor_assign>(ast->desc);

	assert_eq(ast->children.size(), 2);

	appender.build_expression(ast->children[0]);
	if (desc.op != "=")
		appender.append(instruction_ptr(new instruction_duplicate()));
	appender.build_expression(ast->children[1]);

	auto type_name = ast->children[0]->type.base_type->id;
	assert_eq(ast->children[0]->type.base_type->id, ast->children[1]->type.base_type->id);

	if (desc.op != "=")
	{
		bool flag = false;

		if (type_name == type_int->id)
			flag = build_assign_integral<int>(desc.op, appender);
		else if (type_name == type_short->id)
			flag = build_assign_integral<short>(desc.op, appender);
		else if (type_name == type_long->id)
			flag = build_assign_integral<long>(desc.op, appender);
		else if (type_name == type_long_long->id)
			flag = build_assign_integral<long long>(desc.op, appender);
		else if (type_name == type_float->id)
			flag = build_assign_common<float>(desc.op, appender);
		else if (type_name == type_double->id)
			flag = build_assign_common<double>(desc.op, appender);
		else if (type_name == type_long_double->id)
			flag = build_assign_common<long double>(desc.op, appender);
		else if (type_name == type_bool->id)
			flag = build_assign_bool(desc.op, appender);

		if (!flag)
			throw runtime_error(string_format("Unexpected assign operator %s", desc.op.c_str()));
	}
	appender.append(instruction_ptr(new instruction_store()));
}

template<typename T>
bool build_binary_common(const string &op, bytecode_appender &appender)
{
	if (op == "*")
		appender.append(instruction_ptr(new instruction_mul<T>()));
	else if (op == "+")
		appender.append(instruction_ptr(new instruction_add<T>()));
	else if (op == "-")
		appender.append(instruction_ptr(new instruction_sub<T>()));
	else if (op == "/")
		appender.append(instruction_ptr(new instruction_div<T>()));
	else if (op == "==")
		appender.append(instruction_ptr(new instruction_eq<T>()));
	else if (op == "!=")
		appender.append(instruction_ptr(new instruction_neq<T>()));
	else if (op == ">")
		appender.append(instruction_ptr(new instruction_greater<T>()));
	else if (op == "<")
		appender.append(instruction_ptr(new instruction_less<T>()));
	else if (op == "<=")
		appender.append(instruction_ptr(new instruction_leq<T>()));
	else if (op == ">=")
		appender.append(instruction_ptr(new instruction_geq<T>()));
	else
		return false;
	return true;
}

template<typename T>
bool build_binary_integral(const string &op, bytecode_appender &appender)
{
	if (build_binary_common<T>(op, appender))
		return true;
	else if (op == "<<")
		appender.append(instruction_ptr(new instruction_shl<T>()));
	else if (op == ">>")
		appender.append(instruction_ptr(new instruction_shr<T>()));
	else if (op == "&&")
		appender.append(instruction_ptr(new instruction_logical_and<T>()));
	else if (op == "||")
		appender.append(instruction_ptr(new instruction_logical_or<T>()));
	else if (op == "^")
		appender.append(instruction_ptr(new instruction_xor<T>()));
	else if (op == "|")
		appender.append(instruction_ptr(new instruction_or<T>()));
	else if (op == "&")
		appender.append(instruction_ptr(new instruction_and<T>()));
	else if (op == "%")
		appender.append(instruction_ptr(new instruction_rem<T>()));
	else
		return false;
	return true;
}

bool build_binary_bool(const string &op, bytecode_appender &appender)
{
	if (op == "&&")
		appender.append(instruction_ptr(new instruction_logical_and<bool>()));
	else if (op == "||")
		appender.append(instruction_ptr(new instruction_logical_or<bool>()));
	else if (op == "^")
		appender.append(instruction_ptr(new instruction_xor<bool>()));
	else if (op == "|")
		appender.append(instruction_ptr(new instruction_or<bool>()));
	else if (op == "&")
		appender.append(instruction_ptr(new instruction_and<bool>()));
	else
		return false;
	return true;
}

void compiler::bytecode_binary_operator::build(bytecode_appender &appender, AST ast)
{
	auto desc = any_cast<descriptor_binary_operator>(ast->desc);

	assert_eq(ast->children.size(), 2);

	if (desc.op == ",")
	{
		appender.build_expression(ast->children[0]);
		discard_if(ast->children[0], appender);
		appender.build_expression(ast->children[1]);
	}
	else
	{
		appender.build_expression(ast->children[0]);
		appender.build_expression(ast->children[1]);

		auto type_name = ast->children[0]->type.base_type->id;
		if (ast->children[0]->type.base_type->is_pointer() ||
			ast->children[0]->type.base_type->is_array())
		{
			assert_cond(ast->children[1]->type.base_type->is_primitive());
			auto type = dynamic_cast<type_primitive*>(ast->children[1]->type.base_type.get());
			assert_cond(type->is_integral());
			appender.append(instruction_ptr(new instruction_move(desc.op == "+")));
		}
		else
		{
			assert_eq(ast->children[0]->type.base_type->id, ast->children[1]->type.base_type->id);

			bool flag;
			if (type_name == type_int->id)
				flag = build_binary_integral<int>(desc.op, appender);
			else if (type_name == type_short->id)
				flag = build_binary_integral<short>(desc.op, appender);
			else if (type_name == type_long->id)
				flag = build_binary_integral<long>(desc.op, appender);
			else if (type_name == type_long_long->id)
				flag = build_binary_integral<long long>(desc.op, appender);
			else if (type_name == type_float->id)
				flag = build_binary_common<float>(desc.op, appender);
			else if (type_name == type_double->id)
				flag = build_binary_common<double>(desc.op, appender);
			else if (type_name == type_long_double->id)
				flag = build_binary_common<long double>(desc.op, appender);
			else if (type_name == type_bool->id)
				flag = build_binary_bool(desc.op, appender);
			else
				assert_cond(false); // Must be processed in semantic analyzer.

			if (!flag)
				appender.get_program()->compilation_error(ast->t, "Type %s does not accept operator %s", type_name.c_str(), desc.op.c_str());
		}
	}
}

template<typename T>
bool build_unary_pointer(AST ast, const string &op, bytecode_appender &appender)
{
	if (op == "&")
		appender.append(instruction_ptr(new instruction_reference(ast->children[0]->type)));
	else if (op == "*")
		appender.append(instruction_ptr(new instruction_dereference<T>()));
	else
		return false;
	return true;
}

template<typename T>
bool build_unary_common(AST ast, const string &op, bytecode_appender &appender)
{
	if (op == "&")
		appender.append(instruction_ptr(new instruction_reference(ast->children[0]->type)));
	else if (op == "-")
		appender.append(instruction_ptr(new instruction_neg<T>()));
	else
		return false;
	return true;
}

template<typename T>
bool build_unary_integral(AST ast, const string &op, bytecode_appender &appender)
{
	if (build_unary_common<T>(ast, op, appender))
		return true;
	else  if (op == "!")
		appender.append(instruction_ptr(new instruction_not<T>()));
	else if (op == "~")
		appender.append(instruction_ptr(new instruction_invert<T>()));
	else
		return false;
	return true;
}

bool build_unary_bool(AST ast, const string &op, bytecode_appender &appender)
{
	if (op == "&")
		appender.append(instruction_ptr(new instruction_reference(ast->children[0]->type)));
	else if (op == "!")
		appender.append(instruction_ptr(new instruction_not<bool>()));
	else
		return false;
	return true;
}

void compiler::bytecode_unary_operator::build(bytecode_appender &appender, AST ast)
{
	auto desc = any_cast<descriptor_unary_operator>(ast->desc);

	assert_eq(ast->children.size(), 1);

	appender.build_expression(ast->children[0]);

	auto type_name = ast->children[0]->type.base_type->id;

	bool flag = false;
	if (type_name == type_int->id)
		flag = build_unary_integral<int>(ast, desc.op, appender);
	else if (type_name == type_short->id)
		flag = build_unary_integral<short>(ast, desc.op, appender);
	else if (type_name == type_long->id)
		flag = build_unary_integral<long>(ast, desc.op, appender);
	else if (type_name == type_long_long->id)
		flag = build_unary_integral<long long>(ast, desc.op, appender);
	else if (type_name == type_bool->id)
		flag = build_unary_bool(ast, desc.op, appender);
	else if (type_name == type_float->id)
		flag = build_unary_common<float>(ast, desc.op, appender);
	else if (type_name == type_double->id)
		flag = build_unary_common<double>(ast, desc.op, appender);
	else if (type_name == type_long_double->id)
		flag = build_unary_common<long double>(ast, desc.op, appender);
	else if (ast->children[0]->type.base_type->is_pointer())
	{
		type_pointer *p = dynamic_cast<type_pointer*>(ast->children[0]->type.base_type.get());

		string type_name = p->get_base_type()->id;
		if (type_name == type_int->id)
			flag = build_unary_pointer<int>(ast, desc.op, appender);
		else if (type_name == type_short->id)
			flag = build_unary_pointer<short>(ast, desc.op, appender);
		else if (type_name == type_long->id)
			flag = build_unary_pointer<long>(ast, desc.op, appender);
		else if (type_name == type_long_long->id)
			flag = build_unary_pointer<long long>(ast, desc.op, appender);
		else if (type_name == type_bool->id)
			flag = build_unary_pointer<long long>(ast, desc.op, appender);
		else if (type_name == type_float->id)
			flag = build_unary_pointer<float>(ast, desc.op, appender);
		else if (type_name == type_double->id)
			flag = build_unary_pointer<double>(ast, desc.op, appender);
		else if (type_name == type_long_double->id)
			flag = build_unary_pointer<long double>(ast, desc.op, appender);
		else if (p->get_base_type()->is_pointer() || p->get_base_type()->is_array())
			flag = build_unary_pointer<int>(ast, desc.op, appender);
	}
	else
		assert_cond(false); // Must be processed in semantic analyzer.

	if (!flag)
		throw runtime_error(string_format("Unexpected unary operator %s", desc.op.c_str()));
}

void compiler::bytecode_func_call::build(bytecode_appender & appender, AST ast)
{
	auto desc = any_cast<descriptor_func_call>(ast->desc);

	for (auto &child : ast->children)
	{
		appender.build_expression(child);
	}

	appender.append(instruction_ptr(new instruction_invoke(desc.name, ast->children.size())));
}

void compiler::bytecode_constant::build(bytecode_appender & appender, AST ast)
{
	auto desc = any_cast<descriptor_constant>(ast->desc);

	if (boost::starts_with(desc.value, "\"")) // a string
	{
		auto chars = parse_constant_string(desc.value.substr(1, desc.value.size() - 2));
		appender.get_constant_string_pool().set(appender.get_constant_string_pool_ptr(), chars.data(), chars.size());
		appender.append(instruction_ptr(new instruction_constant<char*>(appender.get_constant_string_pool().get<char>(appender.get_constant_string_pool_ptr()))));
		appender.increase_constant_string_pool_ptr(chars.size());
	}
	else if (boost::starts_with(desc.value, "'")) // a char
	{
		auto chars = parse_constant_string(desc.value.substr(1, desc.value.size() - 2));
		if (chars.size() != 1)
			appender.get_program()->compilation_error(ast->t, "Char length exceeds 1");
		appender.append(instruction_ptr(new instruction_constant<int>(chars[0])));
	}
	else if (desc.value == "true")
	{
		appender.append(instruction_ptr(new instruction_constant<int>(1)));
	}
	else if (desc.value == "false")
	{
		appender.append(instruction_ptr(new instruction_constant<int>(0)));
	}
	else
	{
		try
		{
			boost::to_lower(desc.value);
			if (desc.value.find("e") != string::npos)
			{
				vector<string> strs;
				boost::split(strs, desc.value, boost::is_any_of("e"));
				assert_eq(strs.size(), 2);
				int e = boost::lexical_cast<int>(strs[1]);
				double m = boost::lexical_cast<double>(strs[0]);
				appender.append(instruction_ptr(new instruction_constant<double>(m * pow(10, e))));
			}
			else if (desc.value.find(".") == string::npos)
			{
				if (boost::starts_with(desc.value, "0x"))
				{
					if (boost::ends_with(desc.value, "ll"))
						appender.append(instruction_ptr(new instruction_constant<long long>(boost::lexical_cast<lexical_hex<long long>>(desc.value.substr(0, desc.value.size() - 2)))));
					else if (boost::ends_with(desc.value, "l"))
						appender.append(instruction_ptr(new instruction_constant<long>(boost::lexical_cast<lexical_hex<long>>(desc.value.substr(0, desc.value.size() - 1)))));
					else
						appender.append(instruction_ptr(new instruction_constant<int>(boost::lexical_cast<lexical_hex<int>>(desc.value.substr(0, desc.value.size())))));
				}
				else if (boost::starts_with(desc.value, "0"))
				{
					if (boost::ends_with(desc.value, "ll"))
						appender.append(instruction_ptr(new instruction_constant<long long>(boost::lexical_cast<lexical_oct<long long>>(desc.value.substr(0, desc.value.size() - 2)))));
					else if (boost::ends_with(desc.value, "l"))
						appender.append(instruction_ptr(new instruction_constant<long>(boost::lexical_cast<lexical_oct<long>>(desc.value.substr(0, desc.value.size() - 1)))));
					else
						appender.append(instruction_ptr(new instruction_constant<int>(boost::lexical_cast<lexical_oct<int>>(desc.value.substr(0, desc.value.size())))));
				}
				else
				{
					if (boost::ends_with(desc.value, "ll"))
						appender.append(instruction_ptr(new instruction_constant<long long>(boost::lexical_cast<long long>(desc.value.substr(0, desc.value.size() - 2)))));
					else if (boost::ends_with(desc.value, "l"))
						appender.append(instruction_ptr(new instruction_constant<long>(boost::lexical_cast<long>(desc.value.substr(0, desc.value.size() - 1)))));
					else
						appender.append(instruction_ptr(new instruction_constant<int>(boost::lexical_cast<int>(desc.value.substr(0, desc.value.size())))));
				}
			}
			else
			{
				if (boost::ends_with(desc.value, "l"))
					appender.append(instruction_ptr(new instruction_constant<long double>(boost::lexical_cast<long double>(desc.value.substr(0, desc.value.size() - 1)))));
				else if (boost::ends_with(desc.value, "f"))
					appender.append(instruction_ptr(new instruction_constant<float>(boost::lexical_cast<float>(desc.value.substr(0, desc.value.size() - 1)))));
				else
					appender.append(instruction_ptr(new instruction_constant<double>(boost::lexical_cast<double>(desc.value.substr(0, desc.value.size())))));
			}
		}
		catch (boost::bad_lexical_cast&)
		{
			appender.get_program()->compilation_error(ast->t, "Not a valid constant");
		}
	}
}

void compiler::bytecode_var::build(bytecode_appender & appender, AST ast)
{
	auto desc = any_cast<descriptor_var>(ast->desc);

	for (auto it = appender.get_variables().rbegin(); it != appender.get_variables().rend(); ++it)
		if ((*it).variables.count(desc.name))
		{
			auto re = (*it).variables[desc.name];
			appender.append(get_instruction_load(re.second, re.first));
			return;
		}

	if (appender.get_global_variables()->variables.count(desc.name))
	{
		auto re = appender.get_global_variables()->variables[desc.name];
		appender.append(get_instruction_load(re.second, re.first));
		return;
	}

	throw runtime_error(string_format("Variable %s not found.", desc.name.c_str()));
}

void compiler::bytecode_if::build(bytecode_appender & appender, AST ast)
{
	assert_cond(2 <= ast->children.size() && ast->children.size() <= 3);

	appender.build_expression(ast->children[0]);
	int if_exp = appender.append(instruction_ptr()), else_exp;
	appender.build_statements(ast->children[1]);
	if (ast->children.size() == 3)
		else_exp = appender.append(instruction_ptr());
	appender.modify(if_exp, instruction_ptr(new instruction_if(appender.get_current_instruction_number())));

	if (ast->children.size() == 3)
	{
		appender.build_statements(ast->children[2]);
		appender.modify(else_exp, instruction_ptr(new instruction_jump(appender.get_current_instruction_number())));
	}
}

void compiler::bytecode_while::build(bytecode_appender & appender, AST ast)
{
	assert_eq(ast->children.size(), 2);

	int start = appender.get_current_instruction_number();
	appender.build_expression(ast->children[0]);

	int if_exp = appender.append(instruction_ptr());
	appender.build_statements(ast->children[1]);
	appender.append(instruction_ptr(new instruction_jump(start)));

	appender.modify(if_exp, instruction_ptr(new instruction_if(appender.get_current_instruction_number())));
}

void compiler::bytecode_return::build(bytecode_appender & appender, AST ast)
{
	assert_cond(ast->children.size() <= 1);

	if (ast->children.size() == 1)
		appender.build_expression(ast->children[0]);

	appender.append(instruction_ptr(new instruction_return()));
}

void compiler::bytecode_primitive_cast::build(bytecode_appender & appender, AST ast)
{
	auto desc = any_cast<descriptor_primitive_cast>(ast->desc);
	assert_eq(ast->children.size(), 1);
	appender.build_expression(ast->children[0]);
	appender.append(cast_primitive(desc.from_type.base_type, desc.to_type.base_type));
}
