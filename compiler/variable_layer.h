#pragma once

#include <map>
#include <utility>
#include "type_base.h"

namespace compiler
{
	using namespace std;

	struct variables_layer
	{
		size_t ptr = 0;
		map<string, pair<size_t, type_representation>> variables;

		explicit variables_layer(size_t ptr);
	};
}