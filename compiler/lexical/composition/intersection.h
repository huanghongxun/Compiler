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
	struct intersection
		: binary_operator<A, B, parser<intersection<A, B>>>
	{
		typedef intersection<A, B> self_type;
		typedef binary_operator<A, B, parser<intersection<A, B>>> base_type;

		intersection(A const& a, B const& b)
			: base_type(a, b) {}

		template<typename _Scanner>
		typename parser_result<self_type, _Scanner>::type
			parse(_Scanner const& scan) const
		{
			auto save = scan.now();
			if (auto l = left().parse(scan))
			{
				_Scanner bscan(scan.now(), scan.now(), scan);
				scan.reset(save);
				auto r = right().parse(bscan);
				if (l.length() == r.length())
					return l;
			}
			return scan.no_match();
		}
	};

	template<typename A, typename B>
	inline intersection<A, B> operator&(parser<A> const& a, parser<B> const& b)
	{
		return intersection<A, B>(a.actual(), b.actual());
	}

}