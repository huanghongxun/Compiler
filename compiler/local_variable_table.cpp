#include "local_variable_table.h"


compiler::local_variable_table::local_variable_table(size_t size)
	: size(size), pool(malloc(size), [](void *p) { free(p); })
{
}

compiler::local_variable_table::~local_variable_table()
{
}
