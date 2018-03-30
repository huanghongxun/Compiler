#include "compilation_error.h"

compiler::compilation_error::compilation_error() noexcept
	: exception()
{
}

compiler::compilation_error::compilation_error(char const * const _Message) noexcept
	: exception(_Message)
{

}
