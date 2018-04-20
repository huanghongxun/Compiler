#pragma once

#include "match.h"
#include "parser.h"

namespace compiler::parser
{
	template<typename _Attribute>
	struct attributed_action_policy
	{
		template<typename _Actor, typename _Iterator>
		static void do_action(_Actor const& actor, _Attribute& val, _Iterator const& first, _Iterator const& last)
		{
			actor(val);
		}
	};

	template<>
	struct attributed_action_policy<nil_t>
	{
		template<typename _Actor, typename _Attribute, typename _Iterator>
		static void do_action(_Actor const& actor, _Attribute& val, _Iterator const& first, _Iterator const& last)
		{
			actor(first, last);
		}
	};

	struct action_policy
	{
		template<typename _Actor, typename _Attribute, typename _Iterator>
		void do_action(_Actor const& actor, _Attribute& val, _Iterator const& first, _Iterator const& last) const
		{
			attributed_action_policy<_Attribute>::do_action(actor, val, first, last);
		}
	};

	struct match_policy
	{
		template<typename T>
		using result = match<T>;

		const match<nil_t> no_match() const
		{
			return match<nil_t>();
		}

		const match<nil_t> empty_match() const
		{
			return match<nil_t>(0, nil_t());
		}

		template<typename _Attribute, typename _Iterator>
		match<_Attribute> create_match(size_t length, _Attribute const& val, _Iterator const& first, _Iterator const& last) const
		{
			return match<_Attribute>(length, val);
		}

		template<typename _Match, typename _Iterator>
		void group_match(_Match& m, parser_id const& id, _Iterator const& first, _Iterator const& last) const
		{}

		template<typename _Match1, typename _Match2>
		void concat_match(_Match1 &l, _Match2 const& r) const
		{
			l.concat(r);
		}
	};

	struct iteration_policy
	{
		template<typename _Scanner>
		void next(_Scanner const& scan) const
		{
			scan.first++;
		}

		template<typename _Scanner>
		bool at_end(_Scanner const& scan) const
		{
			return scan.first == scan.last;
		}

		template<typename T>
		T filter(T ch) const
		{
			return ch;
		}

		template<typename _Scanner>
		typename _Scanner::reference
			get(_Scanner const& scan) const
		{
			return *scan.first;
		}
	};

	template<typename _IterationPolicy, typename _MatchPolicy, typename _ActionPolicy>
	struct scanner_policies : public _IterationPolicy, public _MatchPolicy, public _ActionPolicy
	{
		typedef _IterationPolicy iteration_policy_type;
		typedef _MatchPolicy match_policy_type;
		typedef _ActionPolicy action_policy_type;

		scanner_policies(
			_IterationPolicy const& i = _IterationPolicy(),
			_MatchPolicy const& m = _MatchPolicy(),
			_ActionPolicy const& a = _ActionPolicy()
		) : _IterationPolicy(i), _MatchPolicy(m), _ActionPolicy(a) {}

		template<typename _ScannerPolicies>
		scanner_policies(_ScannerPolicies const& policies)
			: _IterationPolicy(policies), _MatchPolicy(policies), _ActionPolicy(policies) {}
	};

	template<typename _Iterator, typename _Policies>
	struct scanner : public _Policies
	{
		typedef _Iterator iterator;
		typedef typename iterator::value_type value_type;
		typedef typename iterator::reference reference;
		typedef _Policies policies_type;

		scanner(iterator& first, iterator last, _Policies const& p = _Policies())
			: first(first), last(last), _Policies(p)
		{
			at_end();
		}

		scanner(scanner const& other)
			: _Policies(other), first(other.first), last(other.last) {}

		scanner(scanner const& other, iterator first)
			: _Policies(other), first(first), last(other.last) {}

		template<typename _Policies2>
		scanner(scanner<_Iterator, _Policies2> const& other)
			: _Policies(other), first(other.first), last(other.last) {}

		bool at_end() const
		{
			return iteration_policy_type::at_end(*this);
		}

		value_type operator*() const
		{
			return iteration_policy_type::filter(iteration_policy_type::get(*this));
		}

		scanner const& operator++() const
		{
			next();
			return *this;
		}

		iterator& first;
		iterator const last;

		iterator now() const
		{
			return first;
		}

		void next() const
		{
			iteration_policy_type::next(*this);
		}

		void reset(iterator& it) const
		{
			first = it;
		}

		template<typename _Policies2>
		using rebind_policies = scanner<_Iterator, _Policies2>;

		template<typename _Policies2>
		scanner<_Iterator, _Policies2>
			change_policies(_Policies2 const& p = _Policies2()) const
		{
			return scanner<_Iterator, _Policies2>(first, last, p);
		}

		template<typename _IterationPolicy2>
		scanner<_Iterator, scanner_policies<_IterationPolicy2, match_policy_type, action_policy_type>>
			change_iteration_policy() const
		{
			return scanner<_Iterator, scanner_policies<_IterationPolicy2, typename policies_type::match_policy_type, typename policies_type::action_policy_type>>(first, last);
		}

		template<typename _MatchPolicy2>
		scanner<_Iterator, scanner_policies<iteration_policy_type, _MatchPolicy2, action_policy_type>>
			change_match_policy() const
		{
			return scanner<_Iterator, scanner_policies<typename policies_type::iteration_policy_type, _MatchPolicy2, typename policies_type::action_policy_type>>(first, last);
		}

		template<typename _ActionPolicy2>
		scanner<_Iterator, scanner_policies<iteration_policy_type, match_policy_type, _ActionPolicy2>>
			change_action_policy() const
		{
			return scanner<_Iterator, scanner_policies<typename policies_type::iteration_policy_type, typename policies_type::match_policy_type, _ActionPolicy2>>(first, last);
		}

		scanner& operator=(scanner const& other) = delete;
	};

	template<typename _Scanner, typename _Policies>
	struct rebind_scanner_policies
	{
		typedef typename _Scanner::template rebind_policies<_Policies>::type type;
	};
}
