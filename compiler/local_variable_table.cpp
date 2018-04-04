#include "local_variable_table.h"
#include <cstring>

compiler::local_variable_table::local_variable_table(size_t size)
	: size(size), pool(malloc(size), [](void *p) { free(p); })
{
}

compiler::local_variable_table::~local_variable_table()
{
}

void compiler::local_variable_table::reset()
{
	memset(pool.get(), 0, size);
}
