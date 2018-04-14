#pragma once

#include "../parser.h"
#include "../as_parser.h"

namespace compiler::parser
{

	template<bool nested>
	struct select_confix_parse_refactor;

	template<>
	struct select_confix_parse_refactor<true>
	{
		template<typename _Parser, typename _Scanner, typename _Open, typename _Expr, typename _Close>
		static typename parser_result<_Parser, _Scanner>::type
			parse(_Parser const& p, _Scanner const& scan, _Open const& open, _Expr const& expr, _Close const& close)
		{
			typedef refactor_action_gen<refactor_unary_gen<>> refactor_type;
			const refactor_type refactor_body_d = refactor_type(refactor_unary_d);

			return (open
				>> (p | refactor_body_d[expr - close])
				>> close
				).parse(scan);
		}
	};

	template<>
	struct select_confix_parse_refactor<false>
	{
		template<typename _Parser, typename _Scanner, typename _Open, typename _Expr, typename _Close>
		static typename parser_result<_Parser, _Scanner>::type
			parse(_Parser const&, _Scanner const& scan, _Open const& open, _Expr const& expr, _Close const& close)
		{
			typedef refactor_action_gen<refactor_unary_gen<>> refactor_type;
			const refactor_type refactor_body_d = refactor_type(refactor_unary_d);

			return (open
				>> refactor_body_d[expr - close]
				>> close
				).parse(scan);
		}
	};

	template<bool nested>
	struct select_confix_parse_no_refactor;

	template<>
	struct select_confix_parse_no_refactor<true>
	{
		template<typename _Parser, typename _Scanner, typename _Open, typename _Expr, typename _Close>
		static typename parser_result<_Parser, _Scanner>::type
			parse(_Parser const& p, _Scanner const& scan, _Open const& open, _Expr const& expr, _Close const& close)
		{
			return (open
				>> (p | (expr - close))
				>> close
				).parse(scan);
		}
	};

	template<>
	struct select_confix_parse_no_refactor<false>
	{
		template<typename _Parser, typename _Scanner, typename _Open, typename _Expr, typename _Close>
		static typename parser_result<_Parser, _Scanner>::type
			parse(_Parser const&, _Scanner const& scan, _Open const& open, _Expr const& expr, _Close const& close)
		{
			return (open
				>> (expr - close)
				>> close
				).parse(scan);
		}
	};


	template<typename _Category>
	struct confix_parser_type
	{
		template<bool nested, typename _Parser, typename _Scanner, typename _Open, typename _Expr, typename _Close>
		static typename parser_result<_Parser, _Scanner>::type
			parse(_Parser const& p, _Scanner const& scan, _Open const& open, _Expr const& expr, _Close const& close)
		{
			return select_confix_parse_refactor<nested>::
				parse(p, scan, open, expr, close);
		}
	};

	template<>
	struct confix_parser_type<plain_parser_category>
	{
		template<bool nested, typename _Parser, typename _Scanner, typename _Open, typename _Expr, typename _Close>
		static typename parser_result<_Parser, _Scanner>::type
			parse(_Parser const& p, _Scanner const& scan, _Open const& open, _Expr const& expr, _Close const& close)
		{
			return select_confix_parse_no_refactor<nested>::
				parse(p, scan, open, expr, close);
		}
	};

	template<
		typename _Open, typename _Expr, typename _Close,
		typename _Category, bool is_nested
	> struct confix_parser : public parser<confix_parser<_Open, _Expr, _Close, _Category, is_nested>>
	{
		typedef confix_parser<_Open, _Expr, _Close, _Category, is_nested> self_type;

		_Open open;
		_Expr expr;
		_Close close;

		confix_parser(_Open const& open, _Expr const& expr, _Close const& close)
			: open(open), expr(expr), close(close) {}

		template<typename _Scanner>
		typename parser_result<self_type, _Scanner>::type
			parse(_Scanner const& scan) const
		{
			if (is_nested)
			{
				
			}
			else
			{

			}
		}
	};

	template<bool nested>
	struct confix_parser_gen
	{
		template<typename _Start, typename _Expr, typename _End>
		using result_type = confix_parser<typename as_parser<_Start>::type, typename as_parser<_Expr>::type, typename as_parser<_End>::type, typename as_parser<_Expr>::type::parser_category_type, nested>;

		template<typename _Start, typename _Expr, typename _End>
		result_type<_Start, _Expr, _End> operator()
			(_Start const& start, _Expr const& expr, _End const& end) const
		{
			return result_type<_Start, _Expr, _End>(
				as_parser<_Start>::convert(start),
				as_parser<_Expr>::convert(expr),
				as_parser<_End>::convert(end)
			);
		}
	};

	const confix_parser_gen<false> confix_p = confix_parser_gen<false>();
}