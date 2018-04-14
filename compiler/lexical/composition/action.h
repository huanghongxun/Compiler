#pragma once

#include "composition.h"
#include "../parser.h"
#include "../nil_t.h"

namespace compiler::parser
{
	/*
	
		actor class: p[f]
			where p is a parser and f is a function
			when the parser is successful, the actor calls the scanner::do_action(actor, attribute, first, last)

			where actor is action's function
			attribute is the match(returned by the parser) object's attribute
			first is the iterator pointing to the start of the matching portion of the input
			last is the iterator pointing to the end of the matching portion of the input
	*/
	template<typename _Parser, typename _Action>
	struct action : public unary_operator<_Parser, parser<action<_Parser, _Action>>>
	{
		typedef action<_Parser, _Action> self_type;
		typedef _Action predicate_type;
		typedef unary_operator<_Parser, parser<action<_Parser, _Action>>> base_type;

		action(_Parser const& p, _Action const& a)
			: base_type(p), actor(a) {}

		template<typename _Scanner>
		typename parser_result<self_type, _Scanner>::type
			parse(_Scanner const& scan) const
		{
			scan.at_end();
			auto save = scan.now();
			auto s = subject().parse(scan);
			if (s)
			{
				auto val = s.value();
				scan.do_action(actor, val, save, scan.now());
			}
			return s;
		}

		predicate_type const& predicate() const
		{
			return actor;
		}
	private:
		predicate_type actor;
	};
}