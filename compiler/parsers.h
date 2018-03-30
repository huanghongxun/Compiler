#pragma once

#include <utility>
#include "parser.h"
#include "utility.h"
#include "string_utils.h"

using namespace std;

namespace compiler {

	class parser_if : public parser
	{
	public:
		explicit parser_if(syntax_analyzer *analyzer);

		AST parse() override;
	};

	class parser_for : public parser
	{
		AST parse_for1();

	public:
		explicit parser_for(syntax_analyzer *analyzer);

		AST parse() override;
	};

	class parser_while : public parser
	{
	public:
		explicit parser_while(syntax_analyzer *analyzer);

		AST parse() override;
	};

	class parser_return : public parser
	{
	public:
		explicit parser_return(syntax_analyzer *analyzer);

		AST parse() override;
	};
}