//
// Created by huang on 2018/3/14 0014.
//

#include "syntax_analyzer.h"
#include "syntax_descriptors.h"
#include "parsers.h"
#include "unit_assert.h"
#include "type_primitive.h"
#include "utility.h"
#include "type_pointer.h"

using namespace std;
using namespace compiler::syntax;

void compiler::syntax_analyzer::expect_code(token actual, string expected)
{
	if (actual.code != expected)
		prog->compilation_error(actual, "Expected %s, but found %s", expected.c_str(), actual.code.c_str());
}

void compiler::syntax_analyzer::assert_next_token(const string &expected)
{
	token actual = next_token();
	expect_code(actual, expected);
}

compiler::AST compiler::syntax_analyzer::analyze()
{
	return parse_root();
}

map<string, compiler::type_representation> compiler::syntax_analyzer::get_types()
{
	return types;
}

compiler::AST compiler::syntax_analyzer::parse_root()
{
	AST ast = make_shared<syntax_tree>(descriptor(), token());

	while (true)
	{
		string leading = peek_token();
		if (leading == "")
			return ast;
		else if (leading == ";")
			assert_next_token(";");
		else if (leading == "using") // ignore using namespace std;
		{
			while (next_token().code != ";");
			continue;
		}
		else if (leading == "typedef")
			ast->add_children(parse_typedef());
		else
			ast->add_children(parse_define_vars(true));
	}
}

compiler::AST compiler::syntax_analyzer::parse_define_vars(bool force_static)
{
	auto type_token = peek_token_with_code_info();
	auto type = parse_type();
	type.is_static |= force_static;

	AST child = parse_define_var(type);
	if (child->desc.type() == typeid(descriptor_function))
		return child;

	AST ast = make_shared<syntax_tree>(descriptor_define_local_variables(type.is_static), type_token);
	ast->add_children(child);

	while (peek_token() != ";") {
		assert_next_token(",");
		ast->add_children(parse_define_var(type));
	}

	assert_next_token(";");

	return ast;
}

compiler::AST compiler::syntax_analyzer::parse_define_var(const type_representation &base_type)
{
	auto type = parse_pointer(base_type);
	auto name_token = peek_token_with_code_info();
	auto name = next_name_token();

	if (peek_token("(")) // is function
	{
		auto desc = descriptor_function(type, name);

		assert_next_token("(");

		if (peek_token(")"))
			assert_next_token(")");
		else
		{
			while (true)
			{
				if (peek_token("..."))
				{
					assert_next_token("...");
					assert_next_token(")");
					desc.is_vararg = true;
					break;
				}
				auto arg_type = parse_pointer(parse_type());
				string arg_name = next_name_token();
				desc.args.emplace_back(arg_type, arg_name);

				if (peek_token(")"))
				{
					assert_next_token(")");
					break;
				}
				assert_next_token(",");
			}
		}
		desc.calc_signature();
		AST func_ast = make_shared<syntax_tree>(desc, name_token);

		if (peek_token("{"))
		{
			assert_next_token("{");
			func_ast->add_children(parse_statements());
			assert_next_token("}");
		}
		else if (peek_token(";"))
			assert_next_token(";");
		else
			prog->compilation_error(peek_token_with_code_info(), "Expect { or ;, but found %s", peek_token().c_str());
		return func_ast;
	}
	else // is variable
	{
		auto desc = descriptor_define_variable(type, name);
		while (peek_token("["))
		{
			assert_next_token("[");
			desc.dim.push_back(parse_expression());
			assert_next_token("]");
		}

		AST ast = make_shared<syntax_tree>(desc, name_token);
		if (peek_token("="))
		{
			assert_next_token("=");
			ast->add_children(parse_unit16());
		}
		return ast;
	}
}

compiler::AST compiler::syntax_analyzer::parse_statement()
{
	if (peek_token("{"))
	{
		assert_next_token("{");
		auto ast = parse_statements();
		assert_next_token("}");
		return ast;
	}

	for (const auto &i : parsers) {
		auto ast = i->parse();
		if (ast != nullptr)
			return ast;
	}

	if (is_name(peek_token(0)) && is_name(peek_token(1)))
		return parse_define_vars();

	auto ast = parse_expression();
	assert_next_token(";");
	return ast;
}

compiler::AST compiler::syntax_analyzer::parse_statements()
{
	AST ast = make_shared<syntax_tree>(descriptor_statements(), peek_token_with_code_info());

	while (peek_token() != "}")
		ast->add_children(parse_statement());
	return ast;
}

compiler::AST compiler::syntax_analyzer::parse_packed_statements()
{
	AST ast = parse_statement();
	if (ast->desc.type() == typeid(descriptor_statements))
		return ast;
	AST fa(new syntax_tree(descriptor_statements(), token()));
	fa->add_children(ast);
	return fa;
}

compiler::AST compiler::syntax_analyzer::parse_typedef()
{
	auto token = peek_token_with_code_info();
	assert_next_token("typedef");
	auto type = parse_pointer(parse_type());
	auto name = next_name_token();
	assert_next_token(";");
	types[name] = type;
	return make_shared<syntax_tree>(descriptor(), token);
}

compiler::AST compiler::syntax_analyzer::parse_expression()
{
	return parse_unit17();
}

compiler::AST compiler::syntax_analyzer::parse_unit0()
{
	token name_token = next_token();
	string name = name_token.code;

	AST ast;

	if (is_constant(name))
	{
		ast = make_shared<syntax_tree>(descriptor_constant(name), name_token);
	}
	else if (is_name(name))
	{
		ast = make_shared<syntax_tree>(descriptor_var(name), name_token);
	}
	else
	{
		expect_code(name_token, "(");
		ast = parse_expression();
		assert_next_token(")");
	}

	while (true)
	{
		if (peek_token("(")) // function call
		{
			assert_next_token("(");
			AST n_ast = make_shared<syntax_tree>(descriptor_func_call(name), peek_token_with_code_info());
			if (!peek_token(")"))
			{
				while (true)
				{
					n_ast->add_children(parse_unit16()); // No comma expression in func call args.
					if (peek_token(","))
						assert_next_token(",");
					else // )
						break;
				}
			}
			assert_next_token(")");
			ast = n_ast;
		}
		else if (peek_token("[")) // array access
		{
			AST n_ast = make_shared<syntax_tree>(descriptor_array_access(), peek_token_with_code_info());
			n_ast->add_children(ast);
			assert_next_token("[");
			n_ast->add_children(parse_expression());
			assert_next_token("]");
			ast = n_ast;
		}
		else
			break;
	}
	return ast;
}

compiler::AST compiler::syntax_analyzer::parse_unit1()
{
	return parse_unit0();
}

compiler::AST compiler::syntax_analyzer::parse_unit2()
{
	token name_token = peek_token_with_code_info();
	string name = name_token.code;

	if (types.count(name)) // functional cast
	{
		auto type = parse_type(); // C++ does not allow functional pointer cast.
		assert_next_token("(");
		AST ast = make_shared<syntax_tree>(descriptor_primitive_cast(type), name_token);
		ast->add_children(parse_expression());
		assert_next_token(")");
		return ast;
	}
	else if (name == "sizeof")
	{
		AST ast;
		assert_next_token("sizeof");
		assert_next_token("(");
		if (is_type_token(peek_token()))
			ast = make_shared<syntax_tree>(descriptor_constant(to_string(parse_pointer(parse_type()).base_type->size())), name_token);
		else
		{
			ast = make_shared<syntax_tree>(descriptor_sizeof(), name_token);
			ast->add_children(parse_expression());
		}
		assert_next_token(")");

		return ast;
	}
	else return parse_unit1();
}

compiler::AST compiler::syntax_analyzer::parse_unit3()
{
	AST ast = parse_unit2();
	if (peek_token("--") || peek_token("++"))
	{
		auto t = next_token();
		int d = t.code == "--" ? -1 : t.code == "++" ? 1 : 0;
		AST n_ast = make_shared<syntax_tree>(descriptor_inc(d, false), t);
		n_ast->add_children(ast);
		return n_ast;
	}
	else return ast;
}

compiler::AST compiler::syntax_analyzer::parse_unit4()
{
	if (is_any_of<string>("++", "--")(peek_token()))
	{
		auto t = next_token();
		int d = t.code == "--" ? -1 : t.code == "++" ? 1 : 0;
		AST ast = make_shared<syntax_tree>(descriptor_inc(d, true), t);
		ast->add_children(parse_unit4());
		return ast;
	}
	else if (is_any_of<string>("+", "-", "!", "~", "*", "&")(peek_token()))
	{
		auto t = next_token();
		AST ast = make_shared<syntax_tree>(descriptor_unary_operator(t.code), t);
		ast->add_children(parse_unit4());
		return ast;
	}
	else if (peek_token(0) == "(" && is_type_token(peek_token(1))) // C-style cast
	{
		auto t = peek_token_with_code_info();
		assert_next_token("(");
		auto type = parse_pointer(parse_type());
		assert_next_token(")");
		AST ast = make_shared<syntax_tree>(descriptor_primitive_cast(type), t);
		ast->add_children(parse_unit4());
		return ast;
	}
	else return parse_unit3();
}

compiler::AST compiler::syntax_analyzer::parse_unit5()
{
	return parse_left_associativity_operator([this]() { return parse_unit4(); }, is_any_of<string>("*", "/", "%"));
}

compiler::AST compiler::syntax_analyzer::parse_unit6()
{
	return parse_left_associativity_operator([this]() { return parse_unit5(); }, is_any_of<string>("+", "-"));
}

compiler::AST compiler::syntax_analyzer::parse_unit7()
{
	return parse_left_associativity_operator([this]() { return parse_unit6(); }, is_any_of<string>("<<", ">>"));
}

compiler::AST compiler::syntax_analyzer::parse_unit8()
{
	return parse_unit7();
}

compiler::AST compiler::syntax_analyzer::parse_unit9()
{
	return parse_left_associativity_operator([this]() { return parse_unit8(); }, is_any_of<string>("<", "<=", ">", ">="));
}

compiler::AST compiler::syntax_analyzer::parse_unit10()
{
	return parse_left_associativity_operator([this]() { return parse_unit9(); }, is_any_of<string>("==", "!="));
}

compiler::AST compiler::syntax_analyzer::parse_unit11()
{
	return parse_left_associativity_operator([this]() { return parse_unit10(); }, is_any_of<string>("&"));
}

compiler::AST compiler::syntax_analyzer::parse_unit12()
{
	return parse_left_associativity_operator([this]() { return parse_unit11(); }, is_any_of<string>("^"));
}

compiler::AST compiler::syntax_analyzer::parse_unit13()
{
	return parse_left_associativity_operator([this]() { return parse_unit12(); }, is_any_of<string>("|"));
}

compiler::AST compiler::syntax_analyzer::parse_unit14()
{
	return parse_left_associativity_operator([this]() { return parse_unit13(); }, is_any_of<string>("&&"));
}

compiler::AST compiler::syntax_analyzer::parse_unit15()
{
	return parse_left_associativity_operator([this]() { return parse_unit14(); }, is_any_of<string>("||"));
}

compiler::AST compiler::syntax_analyzer::parse_unit16()
{
	AST left = parse_unit15();

	if (peek_token("=") || peek_token("+=") || peek_token("-=") || peek_token("/=") || peek_token("%=") || peek_token("*=") || peek_token("|=") || peek_token("&=") || peek_token("^=") || peek_token("<<=") || peek_token(">>="))
	{
		auto op = next_token();
		AST ast = make_shared<syntax_tree>(descriptor_assign(op.code), op);
		ast->add_children(left);
		ast->add_children(parse_unit16());
		return ast;
	}
	else
	{
		return left;
	}
}

compiler::AST compiler::syntax_analyzer::parse_unit17()
{
	return parse_left_associativity_operator([this]() { return parse_unit16(); }, is_any_of<string>(","));
}

compiler::type_representation compiler::syntax_analyzer::parse_type()
{
	bool is_static = false, is_const = false, is_type = false, is_built_in = false;
	string type_name;
	while (is_type_token(peek_token()))
	{
		auto token = next_token();
		if (token.code == "static")
			if (is_static)
				prog->compilation_error(token, "Duplicate `static` qualifier");
			else
				is_static = true;
		else if (token.code == "const")
			if (is_const)
				prog->compilation_error(token, "Duplicate `const` qualifier");
			else
				is_const = true;
		else if (token.code == "__built_in")
			if (is_built_in)
				prog->compilation_error(token, "Duplicate `__built_in` qualifier");
			else
				is_built_in = true;
		else
			type_name = token.code, is_type = true;
	}
	if (!is_type)
		if (is_name(peek_token()))
			prog->compilation_error(peek_token_with_code_info(), "Type %s is not defined", peek_token().c_str());
		else
			prog->compilation_error(peek_token_with_code_info(), "Expect a type name, but found %s", peek_token().c_str());
	auto r = type_representation{ types[type_name].base_type, is_const, is_static };
	r.is_built_in = is_built_in;
	return r;
}

compiler::type_representation compiler::syntax_analyzer::parse_pointer(type_representation base_type)
{
	if (peek_token("*"))
	{
		assert_next_token("*");

		bool is_const = false;
		type_representation this_type = base_type;

		if (peek_token("const"))
		{
			assert_next_token("const");

			is_const = true;
		}
		return type_representation{ type_base_ptr(new type_pointer(this_type.base_type, this_type.is_const)), is_const, base_type.is_static };
	}
	else
		return base_type;
}

void compiler::syntax_analyzer::register_type(type_base_ptr type)
{
	types[type->name] = type_representation{ type };
}

void compiler::syntax_analyzer::alias_type(const string & origin, const string & alias)
{
	types[alias] = types[origin];
}

bool compiler::syntax_analyzer::is_type_token(const string & type)
{
	return type == "static" || type == "const" || type == "__built_in" || types.count(type);
}

bool compiler::syntax_analyzer::is_pointer_token(const string & type)
{
	return type == "const" || type == "*";
}

inline compiler::AST compiler::syntax_analyzer::parse_left_associativity_operator(function<AST()> inside, function<bool(std::string)> predicate)
{
	AST ast = inside();
	while (predicate(peek_token()))
	{
		auto t = next_token();
		AST n_ast = make_shared<syntax_tree>(descriptor_binary_operator(t.code), t);
		n_ast->add_children(ast);
		n_ast->add_children(inside());
		ast = n_ast;
	}
	return ast;
}

compiler::AST compiler::syntax_analyzer::parse_left_unary_operator(function<AST()> inside, function<bool(string)> predicate)
{
	if (predicate(peek_token()))
	{
		auto t = next_token();
		AST ast = make_shared<syntax_tree>(descriptor_unary_operator(t.code), t);
		ast->add_children(parse_left_unary_operator(inside, predicate));
		return ast;
	}
	else
		return inside();
}

compiler::syntax_analyzer::syntax_analyzer(program_ptr prog, vector<token> tokens)
	: prog(prog), tokens(tokens)
{
	parsers.emplace_back(new parser_if(this));
	parsers.emplace_back(new parser_for(this));
	parsers.emplace_back(new parser_while(this));
	parsers.emplace_back(new parser_return(this));

	register_type(type_int);
	register_type(type_long);
	register_type(type_short);
	register_type(type_char);
	register_type(type_float);
	register_type(type_double);
	register_type(type_bool);
	register_type(type_void);
}

compiler::program_ptr compiler::syntax_analyzer::get_program()
{
	return prog;
}

bool compiler::syntax_analyzer::peek_token(const string &token) const
{
	return peek_token() == token;
}

string compiler::syntax_analyzer::peek_token(int offset) const
{
	return ptr + offset >= (int)tokens.size() ? "" : tokens[ptr + offset].code;
}

compiler::token compiler::syntax_analyzer::peek_token_with_code_info() const
{
	return ptr >= (int)tokens.size() ? token() : tokens[ptr];
}

string compiler::syntax_analyzer::next_name_token()
{
	token t = next_token();
	if (!is_name(t.code))
		prog->compilation_error(t, "Invalid symbol %s", t.code.c_str());
	return t.code;
}

compiler::token compiler::syntax_analyzer::next_token()
{
	return ptr >= (int)tokens.size() ? token() : tokens[ptr++];
}
