#pragma once

#include <locale>
#include <limits>
#include "../parser.h"
#include "../composition/composition.h"

namespace compiler::parser
{
	template<typename _Char, typename _Scanner, typename _Parser>
	void parse_escape_char(_Scanner const& scan, _Parser &parser)
	{
		if (!scan.at_end())
		{
			auto save = scan.now();
			if (auto s = parser.subject().parse(scan))
			{
				_Char unescaped;
				scan.reset(save);
				if (*scan == '\\')
				{
					scan.next();
					switch (*scan)
					{
						case 'b': unescaped = '\b'; scan.next(); break;
						case 't': unescaped = '\t'; scan.next(); break;
						case 'n': unescaped = '\n'; scan.next(); break;
						case 'f': unescaped = '\f'; scan.next(); break;
						case 'r': unescaped = '\r'; scan.next(); break;
						case '"': unescaped = '"'; scan.next(); break;
						case '\'': unescaped = '\''; scan.next(); break;
						case '\\': unescaped = '\\'; scan.next(); break;
						case 'x': case 'X':
						{
							_Char hex = 0;
							auto lim = std::numeric_limits<_Char>::max() >> 4;
							scan.next();
							while (!scan.at_end())
							{
								auto c = *scan;
								if (hex > lim && std::isxdigit(c, std::locale()))
								{
									scan.reset(save);
									return scan.no_match();
								}
								if (std::isdigit(c, std::locale()))
								{
									hex = (hex << 4) + (c - '0');
									scan.next();
								}
								else if (std::isxdigit(c, std::locale()))
								{
									hex = (hex << 4) + ((std::toupper(c, std::locale())) - 'A' + 0xA);
									scan.next();
								}
								else
									break;
							}
							unescaped = hex;
							break;
						}
						case '0': case '1': case '2': case '3':
						case '4': case '5': case '6': case '7':
						{
							_Char oct = 0;
							auto lim = std::numeric_limits<_Char>::max() >> 3;
							while (!scan.at_end())
							{
								auto c = *scan;
								if (oct > lim && ('0' <= c && c <= '7'))
								{
									scan.reset(save);
									return scan.no_match();
								}

								if ('0' <= c && c <= '7')
								{
									oct = (oct << 3) + (c - '0');
									scan.next();
								}
								else
									break;
							}
							unescaped = oct;
							break;
						}
						default:
							scan.reset(save);
							return scan.no_match();
					}
				}
				else
				{
					unescaped = *scan;
					scan.next();
				}

				scan.do_action(parser.predicate(), unescaped, save, scan.now());
				return s;
			}
		}
		return scan.no_match();
	}

	template<typename _Char, typename _Parser, typename _Action>
	struct escape_char_action : public unary_operator<_Parser, parser<escape_char_action<_Char, _Parser, _Action>>>
	{
		typedef escape_char_action<_Char, _Parser, _Action> self_type;

		escape_char_action(_Parser const& p, _Action const& a)
			: base_type(p), actor(a) {}

		template<typename _Scanner>
		typename parser_result<self_type, _Scanner>::type
			parse(_Scanner const& scan) const
		{
			parse_escape_char<_Char>(scan, parser);
		}

		_Action predicate() const { return actor; }
	private:
		_Action actor;
	};

	template<typename _Char>
	struct escape_char_parser : public parser<escape_char_parser<_Char>>
	{
		typedef escape_char_parser<_Char> self_type;

		template<typename _Action>
		escape_char_action<self_type, _Action, _Char>
			operator[](_Action const& actor) const
		{
			return escape_char_action<_Char, self_type, _Action>(*this, actor);
		}

		template<typename _Scanner>
		typename parser_result<self_type, _Scanner>::type
			parse(_Scanner const& scan) const
		{
			return parse_escape_char<_Char>(scan, *this);
		}
	};
}