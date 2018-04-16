#pragma once

#include "program.h"
#include "token.h"
#include <vector>
#include <cctype>
#include "string_utils.h"
#include "tokens.h"
#include "lexical/lexical.h"
#include <iostream>

namespace compiler
{
	template<typename _String, typename _Position, typename _Iterator = typename _String::iterator>
	struct lexical_iterator
	{
		lexical_iterator(_String file, _String str, size_t tab)
			: now(str.begin()), end(str.end()), tab(tab), pos{ file, 1, 1 }
		{}

		lexical_iterator(_String file, _Iterator now, _Iterator end, size_t tab)
			: now(now), end(end), tab(tab), pos{ file, 1, 1 }
		{}

		lexical_iterator(_Iterator now, _Iterator end, _Position pos, size_t tab)
			: now(now), end(end), pos(pos), tab(tab)
		{}

		_Iterator get_current_line_begin() const
		{
			return line_begin;
		}

		_Iterator get_current_line_end() const
		{
			auto line_end = line_begin;
			while (line_end != last && *line_end != '\r' && *line_end != '\n')
				++line_end;
			return line_end;
		}

		_String get_current_line() const
		{
			return _String(get_current_line_begin(), get_current_line_end());
		}

		_Position const& get_position() const
		{
			return pos;
		}

		auto operator*() const
		{
			return *now;
		}

		lexical_iterator& operator++()
		{
			next();
			return *this;
		}

		lexical_iterator operator++(int)
		{
			lexical_iterator it = *this;
			next();
			return it;
		}

		lexical_iterator operator+(size_t offset) const
		{
			lexical_iterator it = *this;
			for (size_t i = 0; i < offset; ++i)
				++it;
			return it;
		}

		bool operator==(lexical_iterator const& that) const
		{
			return now == that.now;
		}

		bool operator!=(lexical_iterator const& that) const
		{
			return now != that.now;
		}

	private:

		void next()
		{
			if (now == end) return;
			auto cur = *now;
			if (cur == '\n')
			{
				++now;
				new_line();
			}
			else if (cur == '\r')
			{
				++now;
				if (now == end || *now != '\n')
				{
					new_line();
				}
			}
			else if (cur == '\t')
			{
				pos.column += tab - (pos.column - 1) % tab;
				++now;
			}
			else
			{
				++pos.column;
				++now;
			}
		}

		void new_line()
		{
			++pos.line;
			pos.column = 1;

			line_begin = now;
		}

		_Iterator now, end, line_begin;
		_Position pos;
		size_t tab;
	};

	struct simple_position
	{
		string file;
		int line, column;
	};

	using namespace compiler::parser;

	enum class lexical_grammar_id
	{
		root,
		space
	};

    struct lexical_analyzer : public grammar<lexical_analyzer>
	{

		template<typename _Scanner>
		struct definition
		{
			typedef rule<_Scanner, parser_context<>, dynamic_parser_tag> rule_type;

			rule_type lexical_grammar, qualified_name;
			rule_type int_lit;
			rule_type ppsp;

			definition(lexical_analyzer const& self)
			{
				lexical_grammar.set_id(lexical_grammar_id::root);
				ppsp.set_id(lexical_grammar_id::space);

				qualified_name =
					ch_p(token_id::colon_colon) ||
					(no_node_d[*ppsp] >> list_p((ch_p(token_id::identifier) | pattern_p(token_category::keyword, token_category::token_type_mask)), no_node_d[*ppsp] >> ch_p(token_id::colon_colon) >> no_node_d[*ppsp]));

				lexical_grammar = (
					(ch_p('"') >> *(escaped_ch_p - ch_p('"')) >> '"') | // string constant
					(ch_p('\'') >> escaped_ch_p - ch_p('\'') >> '\'') | // char constant
					(int_p >> !(as_lower_d[str_p("l")] | as_lower_d[str_p("ll")])) | // int constant
					(real_p >> !(as_lower_d[str_p("f")] | as_lower_d[str_p("l")])) | // float constant
					ch_p('{') |
					ch_p('}') |
					ch_p('[') |
					ch_p(']') |
					qualified_name
					);

				ppsp = ch_p(token_id::space) | ch_p(token_id::block_comment);
			}
		};

        explicit lexical_analyzer(program_ptr prog)
			: prog(prog), code(prog->get_code()), begin(prog->get_file(), code.begin(), code.end(), 4), end(prog->get_file(), code.end(), code.end(), 4)
		{
		}

        std::vector<token> analyze()
		{
			std::vector<token> result;
			token t;
			while (t = next_token(), !t.code.empty())
				result.push_back(t);
			return result;
		}

    private:
        program_ptr prog;
        string code;

		lexical_iterator<string, simple_position, string::iterator> begin, end;

        token next_token()
		{
			string token;
			while (begin != end && isspace(*begin)) begin++;

			if (begin != end)
			{
				auto pos = begin.get_position();
				char c = *begin++;
				token += c;
				if (c == '"' || c == '\'')
				{
					bool escape = false;
					while (true) {
						char c2 = *begin++;
						if (begin == end)
							prog->compilation_error(pos, "Expected %c, but found EOF", c);

						token += c2;

						if (!escape) {
							if (c2 == c)
								break;
							else if (c2 == '\\')
								escape = true;
						}
						else
							escape = false;
					}


					try
					{
						parse_constant_string(token);
					}
					catch (...)
					{
						prog->compilation_error(pos, "Not a valid string constant");
					}
				}
				else if (c == '#')
				{
					while (begin != end && *begin != '\n') ++begin;

					return next_token();
				}
				else if (isdigit(c))
				{
					if (*begin == 'x' || *begin == 'X')
					{
						token += *begin++;
						if (c != '0')
							prog->compilation_error(pos, "Unexpected character %c", c);
						if (begin == end)
							prog->compilation_error(pos, "Unexpected EOF", c);
						token += *begin++;
						while (begin != end && is_name_char(*begin))
							token += *begin++;
					}
					else
					{
						while (begin != end && (isdigit(*begin) || *begin == '.'))
							token += *begin++;

						if (begin != end && *begin == 'e')
						{
							token += *begin++;
							if (begin != end && (*begin == '+' || *begin == '-'))
								token += *begin++;
							if (begin != end && !isdigit(*begin))
								prog->compilation_error(begin.get_position(), "Unexpected character %c", *begin);
							while (begin != end && is_name_char(*begin))
								token += *begin++;
						}
						else
							while (begin != end && is_name_char(*begin))
								token += *begin++;
					}
				}
				else if (!is_name_char(c))
				{
					int len = is_symbol(c, begin == end ? 0 : *begin, begin + 1 == end ? 0 : *(begin + 1));
					while (--len > 0)
						token += *begin++;
				}
				else
				{
					while (begin != end && is_name_char(*begin))
						token += *begin++;
				}
			}

			if (token == "/*")
			{
				while (begin != end && (begin + 1) != end && (*begin != '*' || *(begin + 1) != '/'))
					++begin;
				if (*begin == '*')
				{
					begin++;
					begin++;
				}

				return next_token();
			}

			if (token == "//")
			{
				while (begin != end && *begin != '\n')
					begin++;

				return next_token();
			}

			return compiler::token(token, begin.get_position().line, begin.get_position().column);
		}
    };
}
