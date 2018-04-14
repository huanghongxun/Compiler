#pragma once

#include "../scanner.h"

namespace compiler::parser
{
	template<typename _Base = action_policy>
	struct no_action_policy : public _Base
	{
		template<typename _Actor, typename _Attribute, typename _Iterator>
		void do_action(_Actor const&, _Attribute&, _Iterator const&, _Iterator const&)
		{
		}
	};
}