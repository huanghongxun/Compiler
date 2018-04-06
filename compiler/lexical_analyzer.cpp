#include "lexical_analyzer.h"

#include <cctype>
#include "string_utils.h"
#include <iostream>

compiler::lexical_analyzer::lexical_analyzer(program_ptr prog)
        : prog(prog), code(prog->get_code())
{
}

std::vector<compiler::token> compiler::lexical_analyzer::analyze()
{
    std::vector<token> result;
    token t;
    while (t = next_token(), !t.code.empty())
        result.push_back(t);
    return result;
}

int compiler::lexical_analyzer::next_char()
{
    int ch = cur < (int) code.size() ? (line_column++, code[cur++]) : EOF;
	if (ch == '\n')
		line_number++, line_column = 0;
    return ch;
}

int compiler::lexical_analyzer::peek_char(int offset)
{
    return cur + offset < (int) code.size() ? code[cur + offset] : EOF;
}

compiler::token compiler::lexical_analyzer::next_token()
{
    string token;
    int c = next_char();
    while (isspace(c)) c = next_char();

    int num = line_number, col = line_column;

    if (c != EOF)
	{
        token += (char) c;
        if (c == '"' || c == '\'')
		{
            int cur = c;
            bool escape = false;
            while (c = next_char(), true) {
                if (c == EOF)
                    prog->compilation_error(line_number, line_column, "Expected %c, but found EOF", cur);

                token += c;

                if (!escape) {
                    if (c == cur)
                        break;
                    else if (c == '\\')
                        escape = true;
                } else
                    escape = false;
            }


			try
			{
				parse_constant_string(token);
			}
			catch (...)
			{
				prog->compilation_error(line_number, line_column, "Not a valid string constant");
			}
		}
		else if (c == '#' && col == 1)
		{
			while (c != EOF && c != '\n')
				c = next_char();

			return next_token();
		}
		else if (isdigit(c))
		{
			if (peek_char() == 'x' || peek_char() == 'X')
			{
				if (c != '0')
					prog->compilation_error(line_number, line_column, "Unexpected character %c", (char)c);
				token += (char)next_char();
				while (c = peek_char(), is_name_char((char)c))
					token += (char)next_char();
			}
			else
			{
				while (c = peek_char(), isdigit((char)c) || c == '.')
					token += (char)next_char();

				if (c == 'e')
				{
					token += (char)next_char();
					if (c = peek_char(), c == '+' || c == '-')
						token += (char)next_char();
					if (!isdigit(peek_char()))
						prog->compilation_error(line_number, line_column, "Unexpected character %c", (char)c);
					while (c = peek_char(), is_name_char(c))
						token += (char)next_char();
				}
				else
					while (c = peek_char(), is_name_char((char)c))
						token += (char)next_char();
			}
		}
		else if (!is_name_char(c))
		{
            int len = is_symbol(c, peek_char(0), peek_char(1));
            while (--len > 0)
                token += (char) next_char();
        }
		else
		{
            while (c = peek_char(), is_name_char(c))
                token += (char) next_char();
        }
    }

	if (token == "/*")
	{
		while (peek_char(0) != EOF && peek_char(1) != EOF && (peek_char(0) != '*' || peek_char(1) != '/'))
			next_char();
		if (peek_char(0) == '*')
		{
			next_char();
			next_char();
		}

		return next_token();
	}

	if (token == "//")
	{
		while (c != EOF && c != '\n')
			c = next_char();

		return next_token();
	}

    return compiler::token(token, num, col);
}