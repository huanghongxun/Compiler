#pragma once

#include <vector>
#include <memory>
#include "instruction.h"
#include "type_base.h"

namespace compiler
{
	using namespace std;

	class func
	{
		vector<instruction_ptr> instructions;
		vector<size_t> arg_index;

	public:
		size_t local_size;
		size_t fixed_arg_size;
		type_representation return_type;
		bool is_built_in;

		vector<instruction_ptr> &get_instructions();

		const vector<instruction_ptr> &get_instructions() const;

		size_t arg_num() const;

		vector<size_t> &get_arg_indexes();

		const vector<size_t> &get_arg_indexes() const;
	};

	typedef shared_ptr<func> func_ptr;
}

