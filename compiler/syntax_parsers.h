#pragma once

#include <utility>
#include "syntax_parser.h"
#include "utility.h"
#include "string_utils.h"

using namespace std;

namespace compiler {

	class parser_if : public syntax_parser
	{
	public:
		explicit parser_if(syntax_analyzer *analyzer);

		AST parse() override;
	};

	class parser_for : public syntax_parser
	{
		AST parse_for1();

	public:
		explicit parser_for(syntax_analyzer *analyzer);

		AST parse() override;
	};

	class parser_while : public syntax_parser
	{
	public:
		explicit parser_while(syntax_analyzer *analyzer);

		AST parse() override;
	};

	class parser_return : public syntax_parser
	{
	public:
		explicit parser_return(syntax_analyzer *analyzer);

		AST parse() override;
	};

	class parser_break : public syntax_parser
	{
	public:
		explicit parser_break(syntax_analyzer *analyzer);

		AST parse() override;
	};

	class parser_continue : public syntax_parser
	{
	public:
		explicit parser_continue(syntax_analyzer *analyzer);

		AST parse() override;
	};
}