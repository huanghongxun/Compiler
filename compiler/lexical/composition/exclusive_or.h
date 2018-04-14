#pragma once

#include "composition.h"
#include "../parser.h"

namespace compiler::parser
{
	/**
	 * exlusive_or stands for expression a ^ b
	 * where a and b are both parsers, returning a new parser that matches a or b but not both.
	 *
	 * @param <A> type of left hand side
	 * @param <B> type of right hand side
	 */
	template<typename A, typename B>
	struct exclusive_or
		: binary_operator<A, B, parser<exclusive_or<A, B>>>
	{
		typedef exclusive_or<A, B> self_type;
		typedef binary_operator<A, B, parser<exclusive_or<A, B>>> base_type;

		exclusive_or(A const& a, B const& b)
			: base_type(a, b) {}

		template<typename _Scanner>
		typename parser_result<self_type, _Scanner>::type
			parse(_Scanner const& scan) const
		{
			auto save = scan.now();
			auto l = left().parse(scan);
			auto save_l = scan.now();
			scan.reset(save);
			auto r = right().parse(scan);
			auto save_r = scan.now();
			scan.reset(save);
			if (bool(l) ^ bool(r))
			{
				if (l)
				{
					scan.reset(save_l);
					return l;
				}
				else
				{
					scan.reset(save_r);
					return r;
				}
			}
			else
				return scan.no_match();
		}
	};

	template<typename A, typename B>
	inline exclusive_or<A, B> operator^(parser<A> const& a, parser<B> const& b)
	{
		return exclusive_or<A, B>(a.actual(), b.actual());
	}

}