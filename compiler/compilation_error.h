#pragma once

#include <exception>

namespace compiler
{
	using namespace std;

	class compilation_error : public exception
	{
	public:

		compilation_error() noexcept;

		explicit compilation_error(char const* const _Message) noexcept;

	};
}
