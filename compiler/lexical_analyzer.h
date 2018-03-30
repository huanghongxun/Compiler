#pragma once

#include "program.h"
#include "token.h"
#include <vector>

namespace compiler
{

    class lexical_analyzer
	{
    public:
        explicit lexical_analyzer(program_ptr prog);

        std::vector<token> analyze();

    private:
        program_ptr prog;
        string code;
        int cur = 0, line_number = 1, line_column = 0;

        int next_char();

        int peek_char(int offset = 0);

        token next_token();
    };
}
