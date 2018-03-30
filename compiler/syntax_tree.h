#pragma once

#include <vector>
#include <string>
#include <any>
#include <memory>
#include "descriptor.h"
#include "type_base.h"
#include "token.h"
using namespace std;

namespace compiler
{
	class syntax_tree;

	typedef shared_ptr<syntax_tree> AST;

    class syntax_tree
	{
    public:
		syntax_tree(const any &desc, const token &t);
		syntax_tree(any&& desc, const token &t);

        any desc;
        vector<AST> children;
		type_representation type;
		token t;

		void add_children(AST tree);
    };
}
