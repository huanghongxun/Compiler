#pragma once

#include "syntax_analyzer.h"
#include "syntax_tree.h"

namespace compiler {
	class syntax_analyzer;

    class syntax_parser {
	protected:
        syntax_analyzer *m_syntax_analyzer;

    public:
        explicit syntax_parser(syntax_analyzer *analyzer);

        virtual AST parse() = 0;
    };
}
