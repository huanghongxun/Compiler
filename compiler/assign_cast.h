#pragma once

#include "primitive_cast.h"

namespace compiler
{
	instruction_ptr assign_cast(type_base_ptr from_type, type_base_ptr to_type);
}
