#pragma once

namespace compiler::parser
{
	template<typename _Derived>
	struct parser
	{
		_Derived& actual()
		{
			return *static_cast<_Derived*>(this);
		}

		_Derived const& actual() const
		{
			return *static_cast<_Derived const*>(this);
		}

		template<typename _Action>
		action<_Derived, _Action> operator[](_Action const& actor) const
		{
			return action<_Derived, _Action>(actual(), actor);
		}
	};
}