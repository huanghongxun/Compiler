#pragma once

#include "sequence.h"
#include "kleene_star.h"

namespace compiler::parser
{
	template<typename A, typename B>
	inline sequence<A, kleene_star<sequence<B, A>>>
		operator%(parser<A> const& a, parser<B> const& b)
	{
		return a.actual() >> *(b.actual() >> a.actual());
	}
}