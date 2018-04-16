#include "built_in_functions.h"
#include "unit_assert.h"

void compiler::built_in_assert::assert0(int condition)
{
	if (condition == 0)
	{
		throw runtime_error("Assertion failed");
	}
}

compiler::built_in_assert::built_in_assert()
	: built_in_function_1(assert0)
{
}

compiler::built_in_putchar::built_in_putchar()
	: built_in_function_1(putchar)
{
}

compiler::built_in_free::built_in_free()
	: built_in_function_1(free)
{
}

compiler::built_in_malloc::built_in_malloc()
	: built_in_function_1(malloc)
{
}

object_t compiler::built_in_scanf::call(const std::vector<object_t>& vec)
{
	size_t len = 0;
	for (size_t i = 1; i < vec.size(); ++i)
		len += vec[i].get_raw_memory_size();
	char *list = (char *)malloc(len), *now = list;
	for (size_t i = 1; i < vec.size(); ++i)
	{
		memcpy(now, vec[i].get_raw_memory(), vec[i].get_raw_memory_size());
		now += vec[i].get_raw_memory_size();
	}

	object_t res = vfscanf(stdin, vec[0].cast<char*>(), list);
	free(list);
	return res;
}

compiler::built_in_puts::built_in_puts()
	: built_in_function_1([](void* v) { return puts(reinterpret_cast<const char*>(v)); })
{
}

compiler::built_in_system::built_in_system()
	: built_in_function_1([](void* v) { return system(reinterpret_cast<const char*>(v)); })
{
}

object_t compiler::built_in_printf::call(const std::vector<object_t>& vec)
{
	size_t len = 0;
	for (size_t i = 1; i < vec.size(); ++i)
		len += vec[i].get_raw_memory_size();
	char *list = (char *)malloc(len), *now = list;
	for (size_t i = 1; i < vec.size(); ++i)
	{
		memcpy(now, vec[i].get_raw_memory(), vec[i].get_raw_memory_size());
		now += vec[i].get_raw_memory_size();
	}

	object_t res = vfprintf(stdout, vec[0].cast<char*>(), list);
	free(list);
	return res;
}
