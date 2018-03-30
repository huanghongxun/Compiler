//
// Created by huang on 2018/3/14 0014.
//

#include "token.h"

compiler::token::token()
	: token("", 0, 0)
{
}

compiler::token::token(string code, int line_number, int line_column)
	: code(move(code)), line_number(line_number), line_column(line_column)
{
}

bool compiler::token::eof() {
    return code.empty();
}
