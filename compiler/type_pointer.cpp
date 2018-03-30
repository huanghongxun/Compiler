#include "type_pointer.h"
#include "type_array.h"

compiler::type_pointer::type_pointer(type_base_ptr base_type, bool is_const)
	: type_base("", string("P") + (is_const ? "K" : "") + base_type->id), base_type(base_type), m_is_const(is_const)
{
}

bool compiler::type_pointer::is_assignable_from(type_base_ptr b) const
{
	if (b->is_array())
	{
		type_array *p = dynamic_cast<type_array*>(b.get());
		return *base_type == *p->get_base_type();
	}
	else if (b->is_pointer())
	{
		type_pointer *p = dynamic_cast<type_pointer*>(b.get());
		if (p->base_type->id == "v") // all pointers can be cast to void* implicitly.
			return true;
		if (*base_type == *p->base_type)
			if (!is_const() && p->is_const()) // char* does not accept const char*
				return false;
			else // const char* accept char*
				return true;
		return false;
	}
	else
		return false;
}

bool compiler::type_pointer::can_be_cast_to(type_base_ptr to_type) const
{
	return to_type->is_pointer();
}

bool compiler::type_pointer::is_primitive() const
{
	return false;
}

bool compiler::type_pointer::is_pointer() const
{
	return true;
}

bool compiler::type_pointer::is_const() const
{
	return m_is_const;
}

compiler::type_base_ptr compiler::type_pointer::get_base_type()
{
	return base_type;
}

compiler::type_base_ptr compiler::type_pointer::implicit_cast_with(type_base_ptr a, type_base_ptr b) const
{
	return type_base_ptr();
}

size_t compiler::type_pointer::size() const
{
	return sizeof(void*); // size of pointers is equal.
}

using namespace compiler;

type_representation compiler::wrap_pointer(type_representation type)
{
	return type_representation(type_base_ptr(new type_pointer(type.base_type, type.is_const)), false, type.is_static);
}

type_representation compiler::unwrap_pointer(type_representation type)
{
	auto p = dynamic_pointer_cast<type_pointer>(type.base_type);
	return type_representation(p->get_base_type(), p->is_const(), type.is_static);
}
