#pragma once

#include "composition.h"
#include "../parser.h"

namespace compiler::parser
{
	/**
	* alternative stands for expression a | b
	* where a and b are both parsers, returning a new parser that matches a or b.
	* 
	* This expression is short circuit evaluated. b will not be parsed if a returns a successful match.
	*
	* @param <A> type of left hand side
	* @param <B> type of right hand side
	*/
	template<typename A, typename B>
	struct alternative
		: binary_operator<A, B, parser<alternative<A, B>>>
	{
		typedef alternative<A, B> self_type;
		typedef binary_operator<A, B, parser<alternative<A, B>>> base_type;

		alternative(A const& a, B const& b)
			: base_type(a, b) {}

		template<typename _Scanner>
		typename parser_result<self_type, _Scanner>::type
			parse(_Scanner const& scan) const
		{
			auto save = scan.now();
			if (auto l = left().parse(scan))
				return l;
			scan.reset(save);
			return right().parse(scan);
		}
	};

	template<typename A, typename B>
	inline alternative<A, B> operator|(parser<A> const& a, parser<B> const& b)
	{
		return alternative<A, B>(a.actual(), b.actual());
	}

}