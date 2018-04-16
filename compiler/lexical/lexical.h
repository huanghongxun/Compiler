#pragma once

#include "nil_t.h"
#include "match.h"
#include "parser.h"
#include "grammar.h"
#include "rule.h"
#include "scanner.h"
#include "skipper.h"

#include "actor/actors.h"

#include "composition/composition.h"
#include "composition/action.h"
#include "composition/directives.h"
#include "composition/epsilon.h"
#include "composition/empty.h"
#include "composition/condition.h"
#include "composition/operators.h"

#include "utility/confix.h"
#include "utility/escape_char.h"
#include "utility/numerics.h"
#include "utility/primitives.h"

namespace compiler::parser
{
	template<typename _Iterator, typename _Derived>
	parse_info<_Iterator> parse(_Iterator first, _Iterator const& last, parser<_Derived> const& p)
	{
		scanner<_Iterator> scan(first, last);
		match<nil_t> hit = p.actual().parse(scan);
		return parse_info<_Iterator>(first, hit, hit && scan.at_end(), hit.length());
	}

	template<typename _Char, typename _Derived>
	parse_info<_Char const*> parse(_Char const* str, parser<_Derived> const& p)
	{
		return parse(str, find_last(str), p);
	}
}