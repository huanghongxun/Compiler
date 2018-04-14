#pragma once

#include <optional>
#include "composition.h"
#include "../parser.h"

namespace compiler::parser
{
	using namespace std;

	 // optional stands for expression !s
	 // which s is a parser, returning a new parser that matches s or empty.
	 //
	 // @param <S> type of subject
	template<typename S>
	struct kleene_star : unary_operator<S, parser<kleene_star<S>>>
	{
		typedef kleene_star<S> self_type;
		typedef unary_operator<S, parser<kleene_star<S>>> base_type;

		kleene_star(S const& s) : base_type(s) {}

		template<typename _Scanner>
		typename parser_result<self_type, _Scanner>::type
			parse(_Scanner const& scan) const
		{
			auto accu = scan.empty_match();
			while (true)
			{
				auto save = scan.now();
				if (auto next = subject().parse(scan))
					scan.concat_match(accu, next);
				else
				{
					scan.reset(save);
					return accu;
				}
			}
		}
	};

	template<typename S>
	inline kleene_star<S> operator*(parser<S> const& s)
	{
		return kleene_star<S>(s.actual());
	}

}