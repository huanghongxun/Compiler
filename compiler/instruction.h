#pragma once

#include <memory>
#include "function_environment.h"

namespace compiler
{
	using namespace std;

	class instruction
	{
	public:
		virtual void operate(function_environment& env) = 0;
	};

	typedef shared_ptr<instruction> instruction_ptr;
}
