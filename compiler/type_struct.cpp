#include "type_struct.h"

using namespace compiler;
using namespace std;

compiler::type_struct::type_struct(const string & id)
	: type_base(id, "S" + id)
{
}

const vector<pair<size_t, shared_ptr<type_base>>>& compiler::type_struct::get_fields() const
{
	return fields;
}

void compiler::type_struct::add_field(type_base_ptr type)
{
	fields.emplace_back(sz, type);
	sz += type->size();
}

bool compiler::type_struct::is_assignable_from(type_base_ptr b) const
{
	return id == b->id;
}

bool compiler::type_struct::can_be_cast_to(type_base_ptr to_type) const
{
	return false;
}

bool compiler::type_struct::is_primitive() const
{
	return false;
}

size_t compiler::type_struct::size() const
{
	return sz;
}
