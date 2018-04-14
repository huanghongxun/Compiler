#pragma once

namespace compiler::parser
{
	template<typename S, typename _Base>
	struct unary_operator : public _Base
	{
		typedef _Base base_type;
		typedef S subject_type;
		typedef typename S::actual_type subject_actual_type;

		unary_operator(S const& sub)
			: _Base(), sub(sub) {}

		unary_operator(_Base const& base, S const& sub)
			: _Base(base), sub(sub) {}

		S const& subject() const
		{
			return sub;
		}
	private:
		subject_actual_type sub;
	};

	template<typename A, typename B, typename _Base>
	struct binary_operator : public _Base
	{
		typedef _Base base_type;
		typedef A left_type;
		typedef B right_type;
		typedef typename A::actual_type left_actual_type;
		typedef typename B::actual_type right_actual_type;

		binary_operator(left_type const& a, right_type const& b)
			: _Base(), a(a), b(b) {}

		left_type const& left() const
		{
			return a;
		}

		right_type const& right() const
		{
			return b;
		}
	private:
		left_type a;
		right_type b;
	};
}