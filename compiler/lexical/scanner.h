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

	template<typename _Iterator, typename _IterationPolicy = iteration_policy, typename _MatchPolicy = match_policy, typename _ActionPolicy = action_policy>
	struct scanner : public _IterationPolicy, public _MatchPolicy, public _ActionPolicy
	{
		typedef _Iterator iterator;
		typedef typename iterator::value_type value_type;
		typedef typename iterator::reference reference;

		typedef _IterationPolicy iteration_policy_type;
		typedef _MatchPolicy match_policy_type;
		typedef _ActionPolicy action_policy_type;

		scanner(iterator& first, iterator last)
			: first(first), last(last)
		{
		}

		scanner(scanner const& other)
			: first(other.first), last(other.last) {}

		scanner(scanner const& other, iterator first)
			: first(first), last(other.last) {}

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

		template<typename _IterationPolicy2, typename _MatchPolicy2, typename _ActionPolicy2>
		scanner<_Iterator, _IterationPolicy2, _MatchPolicy2, _ActionPolicy2>
			change_policies() const
		{
			return scanner<_Iterator, _IterationPolicy2, _MatchPolicy2, _ActionPolicy2>(first, last);
		}

		template<typename _IterationPolicy2>
		scanner<_Iterator, _IterationPolicy2, _MatchPolicy, _ActionPolicy>
			change_iteration_policy() const
		{
			return scanner<_Iterator, _IterationPolicy2, _MatchPolicy, _ActionPolicy>(first, last);
		}

		template<typename _MatchPolicy2>
		scanner<_Iterator, _IterationPolicy, _MatchPolicy2, _ActionPolicy>
			change_match_policy() const
		{
			return scanner<_Iterator, _IterationPolicy, _MatchPolicy2, _ActionPolicy>(first, last);
		}

		template<typename _ActionPolicy2>
		scanner<_Iterator, _IterationPolicy, _MatchPolicy, _ActionPolicy2>
			change_action_policy() const
		{
			return scanner<_Iterator, _IterationPolicy, _MatchPolicy, _ActionPolicy2>(first, last);
		}

		scanner& operator=(scanner const& other) = delete;
	};
}
