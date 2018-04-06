#pragma once
#include "program.h"

/*

对于#include，我们递归调用

*/

namespace compiler
{
	class preprocessor
	{
	public:
		explicit preprocessor(program_ptr prog);

		program_ptr prog;

		program_ptr analyze();
	};
}
