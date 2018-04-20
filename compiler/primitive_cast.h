#pragma once

#include "unit_assert.h"
#include "instruction.h"
#include "instructions.h"
#include "type_primitive.h"

namespace compiler
{
	using namespace compiler::instructions;

	template<typename T>
	instruction_ptr cast_primitive0(type_base_ptr from_type)
	{
		string from = from_type->id;
		if (from == type_int->id)
			return instruction_ptr(new instruction_primitive_cast<int, T>());
		else if (from == type_float->id)
			return instruction_ptr(new instruction_primitive_cast<float, T>());
		else if (from == type_double->id)
			return instruction_ptr(new instruction_primitive_cast<double, T>());
		else if (from == type_long_double->id)
			return instruction_ptr(new instruction_primitive_cast<long double, T>());
		else if (from == type_long->id)
			return instruction_ptr(new instruction_primitive_cast<long, T>());
		else if (from == type_int64->id)
			return instruction_ptr(new instruction_primitive_cast<long long, T>());
		else if (from == type_bool->id)
			return instruction_ptr(new instruction_primitive_cast<bool, T>());
		else if (from == type_char->id)
			return instruction_ptr(new instruction_primitive_cast<char, T>());
		else if (from == type_short->id)
			return instruction_ptr(new instruction_primitive_cast<short, T>());
		else if (from == type_ushort->id)
			return instruction_ptr(new instruction_primitive_cast<unsigned short, T>());
		else if (from == type_byte->id)
			return instruction_ptr(new instruction_primitive_cast<unsigned char, T>());
		else if (from == type_ulong->id)
			return instruction_ptr(new instruction_primitive_cast<unsigned long, T>());
		else if (from == type_uint64->id)
			return instruction_ptr(new instruction_primitive_cast<unsigned long long, T>());
		else if (from == type_uint->id)
			return instruction_ptr(new instruction_primitive_cast<unsigned int, T>());
		else
			throw std::runtime_error(string_format("primitive cast does not support type %s", from.c_str()));
	}

	template<>
	instruction_ptr cast_primitive0<void*>(type_base_ptr from_type);

	instruction_ptr cast_primitive(type_base_ptr from_type, type_base_ptr to_type);
}