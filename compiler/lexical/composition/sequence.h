#pragma once

#include "composition.h"
#include "../parser.h"

namespace compiler::parser
{
	//sequence stands for expression a >> b
	//where a and b are both parsers, returning a new parser that matches a and b in sequence.
	//
	//@param <A> type of left hand side
	//@param <B> type of right hand side
	template<typename A, typename B>
	struct sequence
		: binary_operator<A, B, parser<sequence<A, B>>>
	{
		typedef sequence<A, B> self_type;
		typedef binary_operator<A, B, parser<sequence<A, B>>> base_type;

		sequence(A const& a, B const& b)
			: base_type(a, b) {}

		template<typename _Scanner>
		typename parser_result<self_type, _Scanner>::type
			parse(_Scanner const& scan) const
		{
			if (auto l = left().parse(scan))
				if (auto r = right().parse(scan))
				{
					scan.concat_match(l, r);
					return l;
				}
			return scan.no_match();
		}
	};

	template<typename A, typename B>
	inline sequence<A, B> operator>>(parser<A> const& a, parser<B> const& b)
	{
		return sequence<A, B>(a.actual(), b.actual());
	}

}