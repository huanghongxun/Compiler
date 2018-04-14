#pragma once

#include "utility/primitives.h"

namespace compiler::parser
{
	template<typename T>
	struct as_parser
	{
		typedef T type;

		static type const& convert(type const& p)
		{
			return p;
		}
	};

	template<>
	struct as_parser<char>
	{
		typedef char_literal<char> type;

		static type convert(char ch)
		{
			return type(ch);
		}
	};

	template<>
	struct as_parser<wchar_t>
	{
		typedef char_literal<wchar_t> type;

		static type convert(wchar_t ch)
		{
			return type(ch);
		}
	};

	template<>
	struct as_parser<char16_t>
	{
		typedef char_literal<char16_t> type;

		static type convert(char16_t ch)
		{
			return type(ch);
		}
	};

	template<>
	struct as_parser<char32_t>
	{
		typedef char_literal<char32_t> type;

		static type convert(char32_t ch)
		{
			return type(ch);
		}
	};

	template<>
	struct as_parser<char const*>
	{
		typedef string_literal<char const*> type;

		static type convert(char const* ch)
		{
			return type(ch);
		}
	};

	template<>
	struct as_parser<wchar_t const*>
	{
		typedef string_literal<wchar_t const*> type;

		static type convert(wchar_t const* ch)
		{
			return type(ch);
		}
	};

	template<>
	struct as_parser<char16_t const*>
	{
		typedef string_literal<char16_t const*> type;

		static type convert(char16_t const* ch)
		{
			return type(ch);
		}
	};

	template<int N>
	struct as_parser<char[N]> : as_parser<char const*> {};

	template<int N>
	struct as_parser<wchar_t[N]> : as_parser<wchar_t const*> {};

	template<int N>
	struct as_parser<char16_t[N]> : as_parser<char16_t const*> {};

	template<int N>
	struct as_parser<char32_t[N]> : as_parser<char32_t const*> {};

	template<int N>
	struct as_parser<char const[N]> : as_parser<char const*> {};

	template<int N>
	struct as_parser<wchar_t const[N]> : as_parser<wchar_t const*> {};

	template<int N>
	struct as_parser<char16_t const[N]> : as_parser<char16_t const*> {};

	template<int N>
	struct as_parser<char32_t const[N]> : as_parser<char32_t const*> {};
}