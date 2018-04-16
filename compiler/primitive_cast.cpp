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
		if (from_type->is_integral())
			return cast_primitive0<void*>(from_type);
		// no need to convert void*
		return instruction_ptr();
	}
	else
		throw std::runtime_error(string_format("primitive cast does not support type %s", to.c_str()));
}

namespace compiler
{
	template<>
	instruction_ptr cast_primitive0<void*>(type_base_ptr from_type)
	{
		string from = from_type->id;
		if (from == type_int->id)
			return instruction_ptr(new instruction_primitive_cast<int, void*>());
		else if (from == type_long->id)
			return instruction_ptr(new instruction_primitive_cast<long, void*>());
		else if (from == type_long_long->id)
			return instruction_ptr(new instruction_primitive_cast<long long, void*>());
		else if (from == type_bool->id)
			return instruction_ptr(new instruction_primitive_cast<bool, void*>());
		else if (from == type_char->id)
			return instruction_ptr(new instruction_primitive_cast<char, void*>());
		else if (from == type_short->id)
			return instruction_ptr(new instruction_primitive_cast<short, void*>());
		else
			throw std::runtime_error(string_format("primitive cast does not support type %s", from.c_str()));
	}
}