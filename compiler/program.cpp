//
// Created by huang on 2018/3/13 0013.
//

#include "program.h"
#include "string_utils.h"
#include "compilation_error.h"
#include <cstdarg>
#include <boost/algorithm/string.hpp>
using namespace std;

compiler::program::program(const string &code) : code(code) {
}

const string compiler::program::get_code() const
{
    return code;
}

compiler::program::program(initializer_list<string> args) : program(boost::join(vector<string>(args), "\n"))
{
}

void compiler::program::compilation_error_impl(int line_number, int line_column, const char *fmt, ...)
{
	fprintf(stderr, "main.c<%d:%d>: ", line_number, line_column);
	va_list vl;
	va_start(vl, fmt);
	vfprintf(stderr, fmt, vl);
	va_end(vl);
	fprintf(stderr, "\n");
}
