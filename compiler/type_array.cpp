#include "type_array.h"
#include "type_pointer.h"

using namespace std;
using namespace compiler;

compiler::type_array::type_array(type_base_ptr base_type, size_t dim)
	: type_base("", "A" + to_string(dim) + "_" + base_type->id), base_type(base_type), dim(dim)
{
	sz = base_type->size() * dim;
}

bool compiler::type_array::is_assignable_from(type_base_ptr b) const
{
	return false;
}

bool compiler::type_array::can_be_cast_to(type_base_ptr to_type) const
{
	return false;
}

bool compiler::type_array::is_primitive() const
{
	return false;
}

bool compiler::type_array::is_pointer() const
{
	return false;
}

bool compiler::type_array::is_array() const
{
	return true;
}

compiler::type_base_ptr compiler::type_array::get_base_type()
{
	return base_type;
}

compiler::type_base_ptr compiler::type_array::implicit_cast_with(type_base_ptr a, type_base_ptr b) const
{
	return type_base_ptr();
}

size_t compiler::type_array::size() const
{
	return sz;
}

size_t compiler::type_array::get_this_dimension() const
{
	return dim;
}

type_base_ptr compiler::type_array::to_pointer() const
{
	return type_base_ptr(new type_pointer(base_type, false));
}

type_base_ptr compiler::create_array_type(type_base_ptr element_type, const vector<size_t>& dim)
{
	type_base_ptr res = element_type;
	for (auto it = dim.crbegin(); it != dim.crend(); ++it)
		res = type_base_ptr(new type_array(res, *it));
	return res;
}

type_representation compiler::to_pointer(type_representation array_type)
{
	if (!array_type.base_type->is_array())
		throw invalid_argument("Type should be a array type.");

	type_array *arr = dynamic_cast<type_array*>(array_type.base_type.get());
	array_type.base_type = arr->to_pointer();
	array_type.is_const = true;
	array_type.is_lvalue = false;
	return array_type;
}
