#pragma once

#include <string>
#include <memory>
#include "token.h"
#include "stdafx.h"
#include "compilation_error.h"

namespace compiler {

	using namespace std;

    class program {
    public:
        explicit program(const string &code);

        program(initializer_list<string> args);

        const string get_code() const;

		template<typename _Position, typename ... Args>
		NORETURN void compilation_error(_Position const& t, const char *fmt, Args... args)
		{
			compilation_error_impl(t.line, t.column, fmt, args...);
			throw compiler::compilation_error(string_format(fmt, args...).c_str());
		}

		template<typename ... Args>
		NORETURN void compilation_error(int line_number, int line_column, const char *fmt, Args... args)
		{
			compilation_error_impl(line_number, line_column, fmt, args...);
			throw compiler::compilation_error(string_format(fmt, args...).c_str());
		}

    private:
        string code;

		NORETURN void compilation_error_impl(int line_number, int line_column, const char *fmt, ...);
    };

	typedef shared_ptr<program> program_ptr;
}
