#pragma once

#include <string>
#include <vector>
#include "object_t.h"

namespace compiler
{
	class function_executor
	{
	public:
		virtual object_t execute_function(const string &func_name, const std::vector<object_t> & args) = 0;
	};


}