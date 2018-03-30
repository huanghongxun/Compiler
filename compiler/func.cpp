#include "func.h"

using namespace std;

vector<shared_ptr<compiler::instruction>>& compiler::func::get_instructions()
{
	return instructions;
}

const vector<shared_ptr<compiler::instruction>>& compiler::func::get_instructions() const
{
	return instructions;
}

size_t compiler::func::arg_num() const
{
	return arg_index.size();
}

vector<size_t>& compiler::func::get_arg_indexes()
{
	return arg_index;
}

const vector<size_t>& compiler::func::get_arg_indexes() const
{
	return arg_index;
}
