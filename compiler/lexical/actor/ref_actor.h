#pragma once

namespace compiler::parser
{
	// A semantic action policy holder.
	// act(ref, val): ref with parse result.
	template <typename T, typename _Action>
	struct ref_value_actor : public _Action
	{
		explicit ref_value_actor(T& ref)
			: ref(ref) {}

		template <typename T2>
		void operator()(T2 const& val) const
		{
			act(ref, val);
		}

		template <typename _Iterator>
		void operator()(_Iterator const& first, _Iterator const& last) const
		{
			act(ref, first, last);
		}
	private:
		T & ref;
	};

	// A semantic action policy holder.
	// act(ref, val): ref with parse result.
	template <typename T, typename V, typename _Action>
	struct ref_const_ref_actor : public _Action
	{
		explicit ref_const_ref_actor(T& ref, V const& val)
			: ref(ref), val(val) {}

		template <typename T2>
		void operator()(T2 const&) const
		{
			act(ref, val);
		}

		template <typename _Iterator>
		void operator()(_Iterator const&, _Iterator const&) const
		{
			act(ref, val);
		}
	private:
		T & ref;
		V const& val;
	};

	// A semantic action policy holder.
	// act(ref, val): ref without parse result.
	template <typename T, typename _Action>
	struct ref_actor : public _Action
	{
		explicit ref_actor(T& ref)
			: ref(ref) {}

		template <typename T2>
		void operator()(T2 const&) const
		{
			act(ref);
		}

		template <typename _Iterator>
		void operator()(_Iterator const&, _Iterator const&) const
		{
			act(ref);
		}
	private:
		T & ref;
	};
}