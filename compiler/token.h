#pragma once

#include <string>
#include <utility>

using std::string;
using std::move;

namespace compiler
{

    class token
	{
    public:
		token();
		token(string code, int line_number, int line_column);

        string code;
        int line_number;
		int line_column;

        bool eof();
    };

}
