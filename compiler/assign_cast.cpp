#include "assign_cast.h"

compiler::instruction_ptr compiler::assign_cast(type_base_ptr from_type, type_base_ptr to_type)
{
	if (from_type->is_primitive() && to_type->is_primitive())
		return cast_primitive(from_type, to_type);
	else
		return instruction_ptr();
}
