#include "primitive_cast.h"
#include "string_utils.h"
compiler::instruction_ptr compiler::cast_primitive(type_base_ptr from_type, type_base_ptr to_type)
{
	string to = to_type->id;
	if (to == type_int->id)
		return cast_primitive0<int>(from_type);
	else if (to == type_float->id)
		return cast_primitive0<float>(from_type);
	else if (to == type_double->id)
		return cast_primitive0<double>(from_type);
	else if (to == type_long_double->id)
		return cast_primitive0<long double>(from_type);
	else if (to == type_long->id)
		return cast_primitive0<long>(from_type);
	else if (to == type_long_long->id)
		return cast_primitive0<long long>(from_type);
	else if (to == type_bool->id)
		return cast_primitive0<bool>(from_type);
	else if (to == type_char->id)
		return cast_primitive0<char>(from_type);
	else if (to == type_short->id)
		return cast_primitive0<short>(from_type);
	else if (to_type->is_pointer())
	{
		assert_cond(from_type->is_pointer());
		return instruction_ptr(new instruction_pointer_cast(from_type, to_type));
	}
	else
		throw std::runtime_error(string_format("primitive cast does not support type %s", to.c_str()));
}
