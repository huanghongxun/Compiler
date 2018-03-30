//
// Created by huang on 2018/3/14 0014.
//

#include "syntax_tree.h"

compiler::syntax_tree::syntax_tree(const any &desc, const token &t)
	: desc(desc), t(t)
{
}

compiler::syntax_tree::syntax_tree(any&& desc, const token &t)
	: desc(move(desc)), t(t)
{
}

void compiler::syntax_tree::add_children(AST tree)
{
	children.push_back(tree);
}
