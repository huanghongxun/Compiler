#pragma once

#include "../composition/composition.h"
#include "../as_parser.h"

namespace compiler::parser
{
	template<typename _Subject, typename _Cond>
	struct if_parser : public unary_operator<typename as_parser<_Subject>::type, parser<if_parser<_Subject, _Cond>>>
	{
		typedef if_parser<_Subject, _Cond> self_type;

	};

	template<typename _Cond>
	struct if_parser_gen
	{
		if_parser_gen(_Cond const& cond) : cond(cond) {}

		template<typename _Subject>
		if_parser<_Subject, _Cond> operator[](_Subject const& subject) const
		{
			return if_parser<_Subject, _Cond>(subject, cond);
		}

		_Cond const& cond;
	};
}