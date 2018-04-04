#pragma once

#include <string>
#include <vector>
#include <utility>
#include "syntax_tree.h"
#include "type_base.h"
using namespace std;

namespace compiler::syntax
{
	// Children: ...
	class descriptor_statements {};

	// Children: Define variables...
	class descriptor_define_local_variables
	{
	public:
		bool is_static, is_global = false;

		descriptor_define_local_variables(bool is_static);
	};

	// No Children
	class descriptor_define_variable
	{
	public:
		descriptor_define_variable(const type_representation &type, const string &name);
		type_representation type;
		string name;
		vector<AST> dim;
		size_t ptr;
	};

	// Children: Boolean Expression, Statements, Else-statements
	class descriptor_if {};

	// Children: Boolean Expression, Statements
	class descriptor_while {};

	// Children: Expression
	class descriptor_return {};

	// Children: Expression
	class descriptor_sizeof {};

	// Children: Expression, Expression
	class descriptor_assign
	{
	public:
		string op;

		descriptor_assign(const string &op);
	};

	// Children: Expression
	class descriptor_inc
	{
	public:
		int delta;
		bool increase_first; // => result is lvalue

		descriptor_inc(int delta, bool increase_first);
	};

	// Children: Expression, Expression
	class descriptor_binary_operator
	{
	public:
		string op;

		descriptor_binary_operator(const string &op);
	};

	// Children: Expression
	class descriptor_unary_operator
	{
	public:
		string op;

		descriptor_unary_operator(const string &op);
	};

	// Children: Callee, Args...
	class descriptor_func_call
	{
	public:
		string name;

		descriptor_func_call(const string &name);
	};

	// Children: Callee, Arg
	class descriptor_array_access {};

	// No Children
	class descriptor_var
	{
	public:
		string name;

		descriptor_var(const string &name);
	};

	// No Children
	class descriptor_constant
	{
	public:
		string value;

		descriptor_constant(const string &value);
	};

	// Children: Statements
	class descriptor_function
	{
	public:
		type_representation type;
		string name, signature;
		vector<pair<type_representation, string>> args; // arg_type to arg_name
		bool is_vararg;

		descriptor_function(const type_representation &type, const string &name);

		void calc_signature();
	};

	// Children: Expression
	class descriptor_primitive_cast
	{
	public:
		type_representation from_type, to_type;

		descriptor_primitive_cast(type_representation to_type, type_representation from_type = type_representation(type_base_ptr()));
	};
}