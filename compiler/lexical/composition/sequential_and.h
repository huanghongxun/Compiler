#pragma once

#include "sequence.h"

namespace compiler::parser
{
	template<typename A, typename B>
	inline sequence<A, B> operator&&(parser<A> const& a, parser<B> const& b)
	{
		return sequence<A, B>(a.actual(), b.actual());
	}
}