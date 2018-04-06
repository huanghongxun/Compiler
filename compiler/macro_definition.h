#pragma once

#include <vector>
#include "tokens.h"

namespace compiler
{
	using namespace std;

	template<typename TokenT, typename ContainerT>
	class marco_definition
	{
	public:
		marco_definition(const TokenT &name, bool has_parameters, bool is_predefined)
			: name(name), has_parameters(has_parameters), is_predefined(is_predefined)
			
		{
		}

		TokenT name;
		bool has_parameters;
		bool is_predefined;
		bool replaced_parameters;
		vector<TokenT> parameters;
		ContainerT definition;

		void replace_parameters()
		{
		}
	};

} // namespace preprocessor