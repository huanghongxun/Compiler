#pragma once

#include "object_t.h"
#include "syntax_tree.h"

namespace compiler
{

	class expression_evaulation_error : public exception
	{
	};

	object_t evaulate_expression(AST ast);
}