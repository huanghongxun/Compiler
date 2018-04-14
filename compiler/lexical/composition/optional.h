#pragma once

#include "composition.h"
#include "../parser.h"

namespace compiler::parser
{
	/**
	* optional stands for expression !s
	* which s is a parser, returning a new parser that matches s or empty.
	*
	* @param <A> type of subject
	*/
	template<typename S>
	struct optional
		: unary_operator<S, parser<optional<S>>>
	{
		typedef optional<S> self_type;
		typedef unary_operator<S, parser<optional<S>>> base_type;

		optional(S const& s)
			: base_type(s) {}

		template<typename _Scanner>
		typename parser_result<self_type, _Scanner>::type
			parse(_Scanner const& scan) const
		{
			auto save = scan.now();
			if (auto s = subject().parse(scan))
			{
				return s;
			}
			else
			{
				scan.reset(save);
				return scan.empty_match();
			}
		}
	};

	template<typename S>
	inline optional<S> operator!(parser<S> const& s)
	{
		return optional<S>(s.actual());
	}

}