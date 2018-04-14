#pragma once

#include <memory>

#include "parser.h"
#include "nil_t.h"

namespace compiler::parser
{
	using namespace std;

	template<typename _Derived, typename _Scan, typename _Context, typename _Tag>
	struct rule_base : public parser<_Derived>, public _Context, public _Tag
	{
		typedef _Context context_type;
		typedef typename context_type::context_linker_type linked_context_type;
		typedef typename linked_context_type::attribute_type attribute_type;

		template<typename _Scanner>
		typename parser_result<_Derived, _Scanner>::type
			parse(_Scanner const& scan) const
		{
			typedef parser_scanner_linker<_Scanner> linked_scanner_type;
			typedef typename parser_result<_Derived, _Scanner>::type result_type;
			return parse_context<linked_scanner_type, linked_context_type, result_type>(scan, *this);
		}

		template<typename _Scanner>
		typename parser_result<_Derived, _Scanner>::type
			parse_main(_Scanner const& scan) const
		{
			if (actual().get())
			{
				auto save = scan.now();
				auto res = get()->do_parse_virtual(scan);
				scan.group_match(res, id(), save, scan.now());
				return res;
			}
			else
				return scan.no_match();
		}
	};

	template<typename _Scanner, typename _Attribute>
	struct abstract_parser
	{
		virtual ~abstract_parser() {}

		virtual match<_Attribute> do_parse(_Scanner const& scan) const = 0;

		virtual abstract_parser* clone() const = 0;
	};

	template<typename _Parser, typename _Scanner, typename _Attribute>
	struct concrete_parser : public abstract_parser<_Scanner, _Attribute>
	{
		concrete_parser(_Parser const& p) : p(p) {}

		virtual match<_Attribute> do_parse(_Scanner const& scan) const
		{
			return p.parse(scan);
		}

		virtual abstract_parser<_Scanner, _Attribute>*
			clone() const
		{
			return new concrete_parser(p);
		}

		_Parser p;
	};

	template<typename _Scanner, typename _Context, typename _Tag>
	struct rule : public rule_base<rule<_Scanner, _Context, _Tag>, _Scanner, _Context, _Tag>
	{
		typedef rule<_Scanner, typename _Context, typename _Tag> self_type;
		typedef rule_base<rule<_Scanner, _Context, _Tag>, _Scanner, _Context, _Tag> base_type;
		typedef typename base_type::attribute_type attribute_type;
		typedef abstract_parser<_Scanner, attribute_type> abstract_parser_type;

		rule() {}

		rule(rule const& r)
			: ptr(new concrete_parser<self_type, _Scanner, attribute_type>(r)) {}

		template<typename _Parser>
		rule& operator=(_Parser const& p)
		{
			ptr.reset(new concrete_parser<_Parser, _Scanner, attribute_type>(p));
		}

		rule& operator=(rule const& r)
		{
			ptr.reset(new concrete_parser<rule, _Scanner, attribute_type>(r));
		}

		rule copy() const
		{
			return rule(ptr == nullptr ? nullptr : ptr->clone());
		}
	private:
		unique_ptr<abstract_parser_type> ptr;
	};
}