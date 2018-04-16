#pragma once

#include "composition.h"
#include "../utility/primitives.h"
#include "../parser.h"
#include "../skipper.h"

namespace compiler::parser
{
	template<typename _Result, typename S, typename _Scanner, typename _Base>
	_Result contiguous_parser_parse(S const& s, _Scanner const& scan, no_skipper_iteration_policy<_Base> const&)
	{
		return s.parse(scan);
	}

	template<typename _Result, typename S, typename _Scanner, typename _Base>
	_Result contiguous_parser_parse(S const& s, _Scanner const& scan, skipper_iteration_policy<_Base> const&)
	{
		scan.skip(scan);
		auto hit = s.parse(scan.change_iteration_policy<no_skipper_iteration_policy<typename _Scanner::iteration_policy_type>>());
		return hit;
	}

	template<typename _Result, typename S, typename _Scanner>
	_Result contiguous_parser_parse(S const& s, _Scanner const& scan, iteration_policy const&)
	{
		return s.parse(scan);
	}

	template<typename _Parser>
	struct contiguous
		: public unary_operator<_Parser, parser<contiguous<_Parser>>>
	{
		typedef contiguous<_Parser> self_type;
		typedef unary_operator<_Parser, parser<self_type>> base_type;

		contiguous(_Parser const& p)
			: base_type(p) {}

		template<typename _Scanner>
		typename parser_result<self_type, _Scanner>::type
			parse(_Scanner const& scan) const
		{
			typedef typename parser_result<self_type, _Scanner>::type result_type;

			return contiguous_parser_parse<result_type>(subject(), scan, scan);
		}
	};

	struct lexeme_parser_gen
	{
		template<typename _Parser>
		contiguous<_Parser> operator[](parser<_Parser> const& subject) const
		{
			return contiguous<_Parser>(subject.actual());
		}
	};

	lexeme_parser_gen const lexeme_d = lexeme_parser_gen();

	template<typename _Base>
	struct inhibit_case_iteration_policy : public _Base
	{
		typedef _Base base_type;

		template<typename _Char>
		_Char filter(_Char ch) const
		{
			return std::tolower(ch, std::locale());
		}
	};

	template<typename _Scanner>
	auto change_inhibit_case_iteration_policy(_Scanner const& scan, iteration_policy const&)
	{
		return scan.change_iteration_policy<inhibit_case_iteration_policy<typename _Scanner::iteration_policy_type>>();
	}

	template<typename _Scanner, typename _Base>
	auto change_inhibit_case_iteration_policy(_Scanner const& scan, inhibit_case_iteration_policy<_Base> const&)
	{
		return scan;
	}

	template<typename _Parser>
	struct inhibit_case : public unary_operator<_Parser, parser<inhibit_case<_Parser>>>
	{
		typedef inhibit_case<_Parser> self_type;
		typedef unary_operator<_Parser, parser<inhibit_case<_Parser>>> base_type;

		inhibit_case(_Parser const& p)
			: base_type(p) {}

		template<typename _Scanner>
		typename parser_result<self_type, _Scanner>::type
			parse(_Scanner const& scan) const
		{
			return subject().parse(change_inhibit_case_iteration_policy(scan, scan));
		}
	};

	struct inhibit_case_parser_gen
	{
		inhibit_case<char_literal<char>> operator[](char ch) const
		{
			return inhibit_case<char_literal<char>>(ch);
		}

		inhibit_case<char_literal<wchar_t>> operator[](wchar_t ch) const
		{
			return inhibit_case<char_literal<wchar_t>>(ch);
		}

		inhibit_case<char_literal<char16_t>> operator[](char16_t ch) const
		{
			return inhibit_case<char_literal<char16_t>>(ch);
		}

		inhibit_case<char_literal<char32_t>> operator[](char32_t ch) const
		{
			return inhibit_case<char_literal<char32_t>>(ch);
		}

		template<typename _Iterator>
		inhibit_case<string_literal<_Iterator>> operator[](_Iterator first) const
		{
			return inhibit_case<string_literal<_Iterator>>(first);
		}

		template<typename _Parser>
		inhibit_case<_Parser> operator[](parser<_Parser> const& p) const
		{
			return inhibit_case<_Parser>(p.actual());
		}
	};

	const inhibit_case_parser_gen as_lower_d = inhibit_case_parser_gen();
}