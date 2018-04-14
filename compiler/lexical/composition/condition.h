#pragma once

#include "../parser.h"

namespace compiler::parser
{
	template<typename _Condition, bool test_req = true>
	struct condition_parser : public parser<condition_parser<_Condition, test_req>>
	{
		typedef condition_parser<_Condition, test_req> self_type;

		_Condition cond;

		condition_parser(_Condition const& cond)
			: cond(cond) {}

		template<typename _Scanner>
		typename parser_result<self_type, _Scanner>::type
			parse(_Scanner const& scan) const
		{
			if (test_req == (bool) cond())
				return scan.empty_match();
			else
				return scan.no_match();
		}

		condition_parser<_Condition, !test_req>
			negate() const
		{
			return condition_parser<_Condition, !test_req>(cond);
		}
	};

	template<typename _Condition, bool test_req>
	condition_parser<_Condition, !test_req>
		operator~(condition_parser<_Condition, test_req> const& p)
	{
		return p.negate();
	}
}