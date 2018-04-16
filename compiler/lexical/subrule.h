#pragma once

#include "parser.h"

namespace compiler::parser
{
	template<int id, typename _Context>
	struct subrule : public parser<subrule<id, _Context>>
	{
		typedef subrule<id, _Context> self_type;
		typedef typename _Context::context_linker_type context_type;
		typedef typename _Context::attribute_type attribute_type;

		template<typename _Scanner>
		using result =;

		template<typename _Scanner>
		typename parser_result<self_type, _Scanner>::type
			parse_main(_Scanner const& scan) const
		{

		}
	};

	template <typename _Scanner, typename _List>
	struct subrules_scanner_type
	{
		typedef subrules_scanner<_Scanner, _List> type;
	};

	template <typename _Scanner, typename _List>
	struct subrules_scanner_type<subrules_scanner<_Scanner, _List>, _List>
	{
		typedef subrules_scanner<_Scanner, _List> type;
	};

	template <typename _Scanner, typename _List>
	struct subrules_scanner : public _Scanner
	{
		typedef subrules_scanner<_Scanner, _List> self_type;

		subrules_scanner(_Scanner const& scan, _List const& list)
			: _Scanner(scan), list(list) {}

		template <typename _Policies>
		struct rebind_policies
		{
			typedef typename rebind_scanner_policies<_Scanner, _Policies>::type
				rebind_scanner;
			typedef subrules_scanner<rebind_scanner, _List> type;
		};

		template<typename _Policies2>
		subrules_scanner<typename rebind_scanner_policies<_Scanner, _Policies2>::type, _List>
			change_policies() const
		{
			return subrules_scanner<typename rebind_scanner_policies<_Scanner, _Policies2>::type, _List>(scan.change_policies<_Policies2>(), list);
		}

	};

	template <typename _First, typename _Rest>
	struct subrule_list : public parser<subrule_list<_First, _Rest>>
	{
		typedef subrule_list<_First, _Rest> self_type;

		subrule_list(_First const& first, _Rest const& rest)
			: first(first), rest(rest) {}

		template <typename _Scanner>
		using result = typename parser_result<_First, _Scanner>::type;

		template <typename _Scanner>
		typename parser_result<self_type, _Scanner>::type
			parse(_Scanner const& scan) const
		{
			typedef typename subrules_scanner_type<_Scanner, self_type>::type subrules_scanner_type;
			subrules_scanner_type g(scan, *this);
			return first.start.parse(g);
		}

		template <int id, typename _Definition, typename _Context>
		subrule_list<_First, subrule_list<subrule_parser<id, _Definition, _Context>, _Rest>>
			operator,(subrule_parser<id, _Definition, _Context> const& that) const
		{
			return subrule_list<_First, subrule_list<subrule_parser<id, _Definition, _Context>, _Rest>>
				(first, subrule_list<subrule_parser<id, _Definition, _Context>, _Rest>(that, rest));
		}

		_First first;
		_Rest rest;
	};

	template<int id, typename _Definition, typename _Context>
	struct subrule_parser : public parser<subrule_parser<id, _Definition, _Context>>
	{
		typedef subrule_parser<id, _Definition, _Context> self_type;
		typedef subrule<id, _Context> subrule_type;

		template<typename _Scanner>
		using result = 0;

		subrule_parser(subrule_type const& start, _Definition const& that)
			: start(start), that(that) {}

		template<typename _Scanner>
		typename parser_result<self_type, _Scanner>::type
			parse(_Scanner const& scan) const
		{
			typedef subrule_list<self_type, nil_t> list_type;
			typedef subrules_scanner<_Scanner, list_type> scanner_type;

			list_type list(*this, nil_t());
			scanner_type g(scan, list);
			return start.parse(g);
		}

		template <int id2, typename _Definition2, typename _Context2>
		subrule_list<self_type, subrule_list<subrule_parser<id2, _Definition2, _Context2>, nil_t>>
			operator,(subrule_parser<id2, _Definition2, _Context2> const& that) const
		{
			return subrule_list<self_type, subrule_list<subrule_parser<id2, _Definition2, _Context2>, nil_t>>
				(*this, subrule_list<subrule_parser<id2, _Definition2, _Context2>, nil_t>(that, nil_t()));
		}

		_Definition that;
		subrule_type const& start;
	};
}