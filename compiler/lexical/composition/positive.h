#pragma once

#include "composition.h"
#include "../parser.h"

namespace compiler::parser
{
	/**
	* positive stands for expression +s
	* which s is a parser, returning a new parser that matches s at least once.
	*
	* @param <A> type of subject
	*/
	template<typename S>
	struct positive
		: unary_operator<S, parser<positive<S>>>
	{
		typedef positive<S> self_type;
		typedef unary_operator<S, parser<positive<S>>> base_type;

		positive(S const& s)
			: base_type(s) {}

		template<typename _Scanner>
		typename parser_result<self_type, _Scanner>::type
			parse(_Scanner const& scan) const
		{
			auto accu = subject().parse(scan);
			if (accu)
				while (true)
				{
					auto save = scan.now();
					if (auto next = subject().parse(scan))
						scan.concat_match(accu, next);
					else
					{
						scan.reset(save);
						break;
					}
				}
			return accu;
		}
	};

	template<typename S>
	inline positive<S> operator+(parser<S> const& s)
	{
		return positive<S>(s.actual());
	}

}