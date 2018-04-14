#pragma once

#include "confix.h"
#include "primitives.h"
#include "../as_parser.h"
#include "../composition/operators.h"

namespace compiler::parser
{
	template<bool is_nested>
	struct comment_parser_gen
	{
		template<typename _Start>
		using result_s_type = confix_parser<
			typename as_parser<_Start>::type,
			kleene_star<any_parser>,
			alternative<eol_parser, end_parser>,
			unary_parser_category,
			is_nested
		>;

		template<typename _Start>
		typename result_s_type<_Start> operator()(_Start const& start) const
		{
			return result_s_type<_Start>(
				as_parser<_Start>::convert(start),
				*any_p,
				eol_p | end_p
				);
		}

		template<typename _Start, typename _End>
		using result_se_type = confix_parser<
			typename as_parser<_Start>::type,
			kleene_star<any_parser>,
			typename as_parser<_End>::type,
			unary_parser_category,
			is_nested
		>;

		template<typename _Start, typename _End>
		typename result_se_type<_Start, _End> operator()(_Start const& start, _End const& end) const
		{
			return result_se_type<_Start, _End>(
				as_parser<_Start>::convert(start),
				*any_p,
				as_parser<_End>::convert(end)
				);
		}
	};

	comment_parser_gen<false> const comment_p = comment_parser_gen<false>();
}