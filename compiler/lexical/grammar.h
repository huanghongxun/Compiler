#pragma once

#include "parser.h"

namespace compiler::parser
{
	template<typename _Derived, typename _Context = parser_context<>>
	struct grammar : public parser<_Derived>
	{
		typedef grammar<_Derived, _Context> self_type;
		typedef typename _Context::context_linker_type context_type;

		template<typename _Scanner>
		auto parse_main(_Scanner const& scan) const
		{
			return grammar_parser_parse<0>(this, scan);
		}

		template<typename _Scanner>
		auto parse(_Scanner const& scan) const
		{
			typedef typename parser_result<self_type, _Scanner>::type result_type;
			typedef parser_scanner_linker<_Scanner> scanner_type;
			return parse_context<scanner_type, context_type, result_type>(scan, *this);
		}

	private:
		template<int N, typename _Scanner>
		typename parser_result<self_type, _Scanner>::type
			parse0(_Scanner const& scan) const
		{
			typename parser_result<self_type, _Scanner>::type result;

		}
	};

	template<int N>
	struct call_helper
	{
		template<typename R, typename _Definition, typename _Scanner>
		static void work(R &res, _Definition &def, _Scanner const& scan)
		{
			res = def.template get_start_parser<N>()->parse(scan);
		}
	};

	template<>
	struct call_helper<0>
	{
		template<typename R, typename _Definition, typename _Scanner>
		static void work(R &res, _Definition &def, _Scanner const &scan)
		{
			res = def.start().parse(scan);
		}
	};
}