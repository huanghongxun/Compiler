#pragma once

#include "composition.h"
#include "../parser.h"

namespace compiler::parser
{
	//difference stands for expression a - b
	//where a and b are both parsers, returning a new parser that matches a but not b.
	//
	//@param <A> type of left hand side
	//@param <B> type of right hand side
	template<typename A, typename B>
	struct difference
		: binary_operator<A, B, parser<difference<A, B>>>
	{
		typedef difference<A, B> self_type;
		typedef binary_operator<A, B, parser<difference<A, B>>> base_type;

		difference(A const& a, B const& b)
			: base_type(a, b) {}

		template<typename _Scanner>
		typename parser_result<self_type, _Scanner>::type
			parse(_Scanner const& scan) const
		{
			auto save = scan.now();
			if (auto l = left().parse(scan))
			{
				auto now = scan.now();
				scan.reset(save);
				auto r = right().parse(scan);
				if (!bool(r) || (r.length() < l.length()))
				{
					scan.reset(now);
					return l;
				}
			}
			return scan.no_match();
		}
	};

	template<typename A, typename B>
	inline difference<A, B> operator-(parser<A> const& a, parser<B> const& b)
	{
		return difference<A, B>(a.actual(), b.actual());
	}

}