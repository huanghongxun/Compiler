#pragma once

#include "condition.h"
#include "../parser.h"
#include "../as_parser.h"
#include "empty.h"

namespace compiler::parser
{
	template<typename S>
	struct epsilon_selector
	{
		typedef typename as_parser<S>::type subject_type;

		template<bool = is_base_of<parser<subject_type>, subject_type>>
		struct result;

		template<>
		struct result<true>
		{
			typedef empty_match_parser<subject_type> type;
		};

		template<>
		struct result<false>
		{
			typedef condition_parser<subject_type> type;
		};
	};

	struct epsilon_parser : public parser<epsilon_parser>
	{
		typedef epsilon_parser self_type;

		template<typename _Scanner>
		typename parser_result<self_type, _Scanner>::type
			parse(_Scanner const& scan) const
		{
			return scan.empty_match();
		}

		template<typename S>
		auto operator()(S const& subject) const
		{
			typedef typename epsilon_selector<S>::template result<>::type result_t;
			return result_t(subject);
		}
	};

	epsilon_parser const epsilon_p = epsilon_parser();
}