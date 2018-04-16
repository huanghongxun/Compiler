#pragma once

#include "ref_actor.h"

namespace compiler::parser
{
	struct push_back_action
	{
		template <typename T, typename V>
		void act(T& ref, V const& val) const
		{
			ref.push_back(val);
		}

		template <typename T, typename _Iterator>
		void act(T& ref, _Iterator const& first, _Iterator const& last) const
		{
			typedef typename T::value_type value_type;
			value_type value(first, last);
			ref.push_back(value);
		}
	};

	template <typename T>
	auto push_back_a(T& ref)
	{
		return ref_value_actor<T, push_back_action>(ref);
	}

	template <typename T, typename V>
	auto push_back_a(T& ref, V const& val)
	{
		return ref_const_ref_actor<T, V, push_back_action>(ref, val);
	}

	struct push_front_action
	{
		template <typename T, typename V>
		void act(T& ref, V const& val) const
		{
			ref.push_front(val);
		}

		template <typename T, typename _Iterator>
		void act(T& ref, _Iterator const& first, _Iterator const& last) const
		{
			typedef typename T::value_type value_type;
			value_type value(first, last);
			ref.push_front(value);
		}
	};

	template <typename T>
	auto push_front_a(T& ref)
	{
		return ref_value_actor<T, push_front_action>(ref);
	}

	template <typename T, typename V>
	auto push_front_a(T& ref, V const& val)
	{
		return ref_const_ref_actor<T, V, push_front_action>(ref, val);
	}
}