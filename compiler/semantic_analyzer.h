#pragma once

#include <vector>
#include <map>
#include <set>
#include "syntax_tree.h"
#include "type_base.h"
#include "program.h"
#include "syntax_descriptors.h"

namespace compiler
{
	using namespace compiler::syntax;
	using namespace std;

	class semantic_analyzer
	{
		program_ptr prog;
		vector<map<string, type_representation>> variables;
		map<string, shared_ptr<descriptor_function>> declared_functions; // name to signature to return type
		set<string> defined_functions;
		vector<set<string>> parameters;

		type_representation deduce_type(AST ast);
		void check_func_definition(AST ast);
		void convert_array_access(AST &ast);
	public:
		semantic_analyzer(program_ptr prog);

		void analyze(AST ast);
	};
}
