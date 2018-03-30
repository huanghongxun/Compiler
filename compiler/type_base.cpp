#include "type_base.h"

compiler::type_base::type_base(const std::string &name, const std::string & id)
	: id(id), name(name)
{
}

bool compiler::type_base::is_primitive() const
{
	return false;
}

bool compiler::type_base::is_integral() const
{
	return false;
}

bool compiler::type_base::is_void() const
{
	return false;
}

bool compiler::type_base::is_pointer() const
{
	return false;
}

bool compiler::type_base::is_array() const
{
	return false;
}

bool compiler::type_base::operator==(const type_base & b) const
{
	return id == b.id;
}

bool compiler::type_base::operator!=(const type_base & b) const
{
	return id != b.id;
}

compiler::type_representation::type_representation(type_base_ptr base_type, bool is_const, bool is_static)
	: base_type(base_type), is_const(is_const), is_static(is_static)
{
}

compiler::type_representation::type_representation(type_base_ptr base_type)
	: type_representation(base_type, false, false)
{
}

compiler::type_representation::type_representation()
	: base_type(type_base_ptr())
{
}

std::string compiler::type_representation::to_string()
{
	if (base_type == nullptr)
		return "";
	else
		return (is_const ? "P" : "") + base_type->id;
}
