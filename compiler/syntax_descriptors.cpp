#include "syntax_descriptors.h"

using namespace compiler;

compiler::syntax::descriptor_binary_operator::descriptor_binary_operator(const string & op)
	: op(op)
{
}

compiler::syntax::descriptor_unary_operator::descriptor_unary_operator(const string & op)
	: op(op)
{
}

compiler::syntax::descriptor_func_call::descriptor_func_call(const string & name)
	: name(name)
{
}

compiler::syntax::descriptor_var::descriptor_var(const string & name)
	: name(name)
{
}

compiler::syntax::descriptor_constant::descriptor_constant(const string & value)
	: value(value)
{
}

compiler::syntax::descriptor_define_variable::descriptor_define_variable(const type_representation &type, const string & name)
	: type(type), name(name)
{
}

compiler::syntax::descriptor_function::descriptor_function(const type_representation &type, const string & name)
	: type(type), name(name)
{
}

void compiler::syntax::descriptor_function::calc_signature()
{
	string sig = name + "(";
	for (auto &i : args)
		sig += i.first.to_string() + ",";
	if (sig.back() == ',')
		sig.erase(sig.size() - 1);
	signature = sig + ")";
}

compiler::syntax::descriptor_primitive_cast::descriptor_primitive_cast(type_representation to_type, type_representation from_type)
	: to_type(to_type), from_type(from_type)
{
}

compiler::syntax::descriptor_assign::descriptor_assign(const string & op)
	: op(op)
{
}
