#pragma once

#include "composition.h"
#include "../parser.h"

namespace compiler::parser
{
	/**
	* sequential_or stands for expression a || b
	* in terms of a | b | a >> b.
	*
	* @param <A> type of left hand side
	* @param <B> type of right hand side
	*/
	template<typename A, typename B>
	struct sequential_or
		: binary_operator<A, B, parser<sequential_or<A, B>>>
	{
		typedef sequential_or<A, B> self_type;
		typedef binary_operator<A, B, parser<sequential_or<A, B>>> base_type;

		sequential_or(A const& a, B const& b)
			: base_type(a, b) {}

		template<typename _Scanner>
		typename parser_result<self_type, _Scanner>::type
			parse(_Scanner const& scan) const
		{
			typedef typename parser_result<self_type, _Scanner>::type result_type;
			typedef typename _Scanner::iterator iterator;

			auto save = scan.now();
			if (auto l = left().parse(scan))
			{
				save = scan.now();
				if (auto r = right().parse(scan))
				{
					scan.concat_match(l, r);
					return l;
				}
				else
				{
					scan.reset(save);
					return l;
				}
			}
			scan.reset(save);
			return right().parse(scan);
		}
	};

	template<typename A, typename B>
	inline sequential_or<A, B> operator||(parser<A> const& a, parser<B> const& b)
	{
		return sequential_or<A, B>(a.actual(), b.actual());
	}

}