#pragma once

#include <map>
#include <functional>
#include <vector>
#include <memory>
#include "type_base.h"
#include "token.h"
#include "program.h"
#include "syntax_tree.h"
#include "syntax_parser.h"

namespace compiler
{
	using namespace std;

	class syntax_parser;

    class syntax_analyzer
	{
        program_ptr prog;
        vector<token> tokens;
        int ptr = 0;
		vector<unique_ptr<syntax_parser>> parsers;

		map<string, type_representation> types;
		void register_type(type_base_ptr type);
		void alias_type(const string &origin, const string &alias);

		bool is_type_token(const string &type);
		bool is_pointer_token(const string &type);

		void expect_code(token expected, string actual);

		AST parse_left_associativity_operator(function<AST()> inside, function<bool(string)> predicate);
		AST parse_left_unary_operator(function<AST()> inside, function<bool(string)> predicate);
    public:
		syntax_analyzer(program_ptr prog, vector<token> tokens);

		program_ptr get_program();

        bool peek_token(const string &token) const;
		string peek_token(int offset = 0) const;
		token peek_token_with_code_info() const;

		string next_name_token();
        token next_token();
        void assert_next_token(const string &token);

		AST analyze();
		map<string, type_representation> get_types();

		AST parse_root();
		AST parse_define_vars(bool force_static = false);
		AST parse_define_var(const type_representation &type);
		AST parse_statement();
		AST parse_packed_statements();
		AST parse_typedef();
		AST parse_statements();
		AST parse_expression();
		AST parse_unit0();
		AST parse_unit1();
		AST parse_unit2();
		AST parse_unit3();
		AST parse_unit4();
		AST parse_unit5();
		AST parse_unit6();
		AST parse_unit7();
		AST parse_unit8();
		AST parse_unit9();
		AST parse_unit10();
		AST parse_unit11();
		AST parse_unit12();
		AST parse_unit13();
		AST parse_unit14();
		AST parse_unit15();
		AST parse_unit16();
		AST parse_unit17();

		type_representation parse_type();
		type_representation parse_pointer(type_representation base_type);
    };
}
