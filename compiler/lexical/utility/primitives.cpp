#include "primitives.h"

namespace compiler::parser
{
	nothing_parser operator~(any_parser)
	{
		return nothing_parser();
	}
}