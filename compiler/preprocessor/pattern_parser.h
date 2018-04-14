#pragma once

#include "../lexical/primitive/primitives.h"

namespace compiler::preprocessor
{
	using namespace compiler::parser;

	template<typename _Char = char>
	struct pattern_parser : public test_parser<pattern_parser<_Char>>
	{
		unsigned pattern_mask;
		_Char pattern;

		pattern_parser(_Char pattern, unsigned pattern_mask = 0)
			: pattern(pattern), pattern_mask(pattern_mask) {}

		template<typename T>
		bool test(T pattern) const
		{
			return ((unsigned)pattern & pattern_mask) == (unsigned)this->pattern;
		}
	};

	template<typename _Char>
	pattern_parser<_Char> pattern_p(_Char pattern, unsigned pattern_mask = 0)
	{
		return pattern_parser<_Char>(pattern, pattern_mask);
	}
}