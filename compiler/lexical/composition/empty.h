#pragma once

#include "composition.h"
#include "../parser.h"
#include "no_action.h"

namespace compiler::parser
{
	template<typename _Subject, bool test_req = true>
	struct empty_match_parser : public unary_operator<_Subject, parser<empty_match_parser<_Subject, test_req>>>
	{
		typedef empty_match_parser<_Subject, test_req> self_type;
		typedef unary_operator<_Subject, parser<empty_match_parser<_Subject, test_req>>> base_type;
		
		explicit empty_match_parser(_Subject const& sub) : base_type(sub) {}

		template<typename _Scanner>
		typename parser_result<self_type, _Scanner>::type
			parse(_Scanner const& scan) const
		{
			auto save = scan.now();
			
			bool matches = subject().parse(scan.change_action_policy<no_action_policy<typename _Scanner::action_policy_type>>());
			if (matches == test_req)
			{
				scan.reset(save);
				return scan.empty_match();
			}
			else
			{
				return scan.no_match();
			}
		}

	};

	template <typename _Subject, bool test_req>
	auto operator~(empty_match_parser<_Subject, test_req> const& p)
	{
		return empty_match_parser<_Subject, !test_req>(p.subject());
	}
}