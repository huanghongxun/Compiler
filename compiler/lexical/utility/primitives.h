#pragma once

#include <locale>
#include <stdexcept>
#include "../parser.h"
#include "../composition/directives.h"

namespace compiler::parser
{
	template<typename _Derived>
	struct test_parser : public parser<_Derived>
	{
		typedef _Derived self_type;

		template <typename _Scanner>
		using result = typename match_result<_Scanner, typename _Scanner::value_type>::type;
		
		template<typename _Scanner>
		typename parser_result<self_type, _Scanner>::type
			parse(_Scanner const& scan) const
		{
			if (!scan.at_end())
			{
				auto ch = *scan;
				if (actual().test(ch))
				{
					auto save = scan.now();
					scan.next();
					return scan.create_match(1, ch, save, scan.now());
				}
			}
			return scan.no_match();
		}
	};

	template<typename _Parser>
	struct negated_test_parser
		: public test_parser<negated_test_parser<_Parser>>
	{
		negated_test_parser(_Parser const& p)
			: positive(p.actual()) {}

		template<typename T>
		bool test(T ch) const
		{
			return !positive.test(ch);
		}

		_Parser const positive;
	};

	template<typename _Char = char>
	struct char_literal : public test_parser<char_literal<_Char>>
	{
		_Char ch;
		char_literal(_Char ch) : ch(ch) {}

		template<typename T>
		bool test(T ch) const
		{
			return this->ch == ch;
		}
	};

	template<typename _Char>
	char_literal<_Char> ch_p(_Char ch) { return char_literal<_Char>(ch); }

	template<typename _Char, size_t N>
	char_literal<_Char> ch_p(_Char const (&str)[N])
	{
		static_assert(N < 3);
		return char_literal<_Char>(str[0]);
	}

	template<typename _Char>
	struct range_parser : public test_parser<range_parser<_Char>>
	{
		_Char first, last;

		range_parser(_Char first, _Char last)
			: first(first), last(last)
		{
			if (!(first < last))
				throw std::invalid_argument("first < last");
		}

		template<typename T>
		bool test(T ch) const
		{
			return !(_Char(ch) < first) && !(last < _Char(ch));
		}
	};

	template<typename _Char>
	inline range_parser<_Char> ch_range(_Char first, _Char last)
	{
		return range_parser<_Char>(first, last);
	}

	template<typename _Iterator>
	_Iterator find_last(_Iterator first)
	{
		while (*first) ++first;
		return first;
	}

	template<typename _Iterator = char const*>
	struct char_sequence : parser<char_sequence<_Iterator>>
	{
		typedef char_sequence<_Iterator> self_type;

		_Iterator first, last;
		
		char_sequence(_Iterator first)
			: char_sequence(first, find_last(first)) {}

		char_sequence(_Iterator first, _Iterator last)
			: first(first), last(last) {}

		template<typename _Scanner>
		typename parser_result<self_type, _Scanner>::type
			parse(_Scanner const& scan) const
		{
			auto save = scan.now();

			auto now = first;
			size_t len = 0;

			while (now != last)
			{
				if (scan.at_end() || *now != *scan)
					return scan.no_match();
				++now; scan.next();

				++len;
			}

			return scan.create_match(len, nil_t(), save, scan.now());
		}
	};

	template<typename _Char>
	char_sequence<_Char const*> chs_p(_Char const* first)
	{
		return char_sequence<_Char const*>(first);
	}

	template<typename _Iterator>
	char_sequence<_Iterator> chs_p(_Iterator first, _Iterator last)
	{
		return char_sequence<_Iterator>(first, last);
	}

	template<typename _Iterator>
	struct string_literal : public parser<string_literal<_Iterator>>
	{
		typedef string_literal<_Iterator> self_type;

		string_literal(_Iterator first)
			: seq(first) {}

		string_literal(_Iterator first, _Iterator last)
			: seq(first, last) {}

		template<typename _Scanner>
		typename parser_result<self_type, _Scanner>::type
			parse(_Scanner const& scan) const
		{
			typedef typename parser_result<self_type, _Scanner>::type result_type;
			return contiguous_parser_parse<result_type>(seq, scan, scan);
		}

	private:
		char_sequence<_Iterator> seq;
	};

	template<typename _Char>
	string_literal<_Char const*> str_p(_Char const* first)
	{
		return string_literal<_Char const*>(first);
	}

	template<typename _Char>
	string_literal<_Char*> str_p(_Char* first)
	{
		return string_literal<_Char const*>(first);
	}

	template<typename _Iterator>
	string_literal<_Iterator> str_p(_Iterator first, _Iterator last)
	{
		return string_literal<_Iterator>(first, last);
	}

	template<typename _String>
	auto str_p(_String const& str)
	{
		return str_p(str.begin(), str.end());
	}

	struct nothing_parser : parser<nothing_parser>
	{
		typedef nothing_parser self_type;

		template<typename _Scanner>
		typename parser_result<self_type, _Scanner>::type
			parse(_Scanner const& scan) const
		{
			return scan.no_match();
		}
	};

	struct any_parser : public test_parser<any_parser>
	{
		typedef any_parser self_type;

		template<typename _Char>
		bool test(_Char) const
		{
			return true;
		}
	};

	any_parser const any_p;

	nothing_parser operator~(any_parser)
	{
		return nothing_parser();
	}

	struct alnum_parser : public test_parser<alnum_parser>
	{
		typedef alnum_parser self_type;

		template<typename _Char>
		bool test(_Char ch) const
		{
			return std::isalnum<_Char>(ch, std::locale());
		}
	};

	struct alpha_parser : public test_parser<alpha_parser>
	{
		typedef alpha_parser self_type;

		template<typename _Char>
		bool test(_Char ch) const
		{
			return std::isalpha<_Char>(ch, std::locale());
		}
	};

	struct cntrl_parser : public test_parser<cntrl_parser>
	{
		typedef cntrl_parser self_type;

		template<typename _Char>
		bool test(_Char ch) const
		{
			return std::iscntrl<_Char>(ch, std::locale());
		}
	};

	struct digit_parser : public test_parser<digit_parser>
	{
		typedef digit_parser self_type;

		template<typename _Char>
		bool test(_Char ch) const
		{
			return std::isdigit<_Char>(ch, std::locale());
		}
	};

	struct graph_parser : public test_parser<graph_parser>
	{
		typedef graph_parser self_type;

		template<typename _Char>
		bool test(_Char ch) const
		{
			return std::isgraph<_Char>(ch, std::locale());
		}
	};

	struct lower_parser : public test_parser<lower_parser>
	{
		typedef lower_parser self_type;

		template<typename _Char>
		bool test(_Char ch) const
		{
			return std::islower<_Char>(ch, std::locale());
		}
	};

	struct print_parser : public test_parser<print_parser>
	{
		typedef print_parser self_type;

		template<typename _Char>
		bool test(_Char ch) const
		{
			return std::isprint<_Char>(ch, std::locale());
		}
	};

	struct punct_parser : public test_parser<punct_parser>
	{
		typedef punct_parser self_type;

		template<typename _Char>
		bool test(_Char ch) const
		{
			return std::ispunct<_Char>(ch, std::locale());
		}
	};

	struct blank_parser : public test_parser<blank_parser>
	{
		typedef blank_parser self_type;

		template<typename _Char>
		bool test(_Char ch) const
		{
			return std::isblank<_Char>(ch, std::locale());
		}
	};

	struct space_parser : public test_parser<space_parser>
	{
		typedef space_parser self_type;

		template<typename _Char>
		bool test(_Char ch) const
		{
			return std::isspace<_Char>(ch, std::locale());
		}
	};

	struct upper_parser : public test_parser<upper_parser>
	{
		typedef upper_parser self_type;

		template<typename _Char>
		bool test(_Char ch) const
		{
			return std::isupper<_Char>(ch, std::locale());
		}
	};

	struct xdigit_parser : public test_parser<xdigit_parser>
	{
		typedef xdigit_parser self_type;

		template<typename _Char>
		bool test(_Char ch) const
		{
			return std::isxdigit<_Char>(ch, std::locale());
		}
	};

	struct eol_parser : public test_parser<eol_parser>
	{
		template<typename _Scanner>
		typename parser_result<self_type, _Scanner>::type
			parse(_Scanner const& ch) const
		{
			auto save = scan.now();
			size_t len = 0;

			if (!scan.at_end() && *scan == '\r')
			{
				scan.next();
				++len;
			}

			if (!scan.at_end() && *scan = '\n')
			{
				scan.next();
				++len;
			}

			if (len)
				return scan.create_match(len, nil_t(), save, scan.now());
			else
				return scan.no_match();
		}
	};

	struct end_parser : public parser<end_parser>
	{
		typedef end_parser self_type;

		template<typename _Scanner>
		typename parser_result<self_type, _Scanner>::type
			parse(_Scanner const& scan) const
		{
			if (scan.at_end())
				return scan.empty_match();
			else
				return scan.no_match();
		}
	};
}