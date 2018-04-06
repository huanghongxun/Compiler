#include "type_primitive.h"

bool compiler::type_primitive::is_assignable_from(type_base_ptr b) const
{
	return !is_void() && !b->is_void() && b->is_primitive();
}

bool compiler::type_primitive::can_be_cast_to(type_base_ptr to_type) const
{
	return to_type->is_primitive();
}

bool compiler::type_primitive::is_primitive() const
{
	return true;
}

bool compiler::type_primitive::is_integral() const
{
	return 0 <= priority && priority < 200; // bool, char, short, int, long, long long
}

bool compiler::type_primitive::is_void() const
{
	return id == "v";
}

compiler::type_base_ptr compiler::type_primitive::implicit_cast_with(type_base_ptr a, type_base_ptr b) const
{
	if (!a->is_primitive() || !b->is_primitive())
		return type_base_ptr();
	auto pa = dynamic_cast<type_primitive *>(a.get());
	auto pb = dynamic_cast<type_primitive *>(b.get());
	if (pa->priority < 100 && pb->priority < 100)
		return type_int;
	else if (pa->priority < pb->priority)
		return b;
	else
		return a;
}

size_t compiler::type_primitive::size() const
{
	return sz;
}

compiler::type_base_ptr compiler::type_int(new type_primitive("int", "i", sizeof(int), 100));
compiler::type_base_ptr compiler::type_float(new type_primitive("float", "f", sizeof(float), 200));
compiler::type_base_ptr compiler::type_double(new type_primitive("double", "d", sizeof(double), 210));
compiler::type_base_ptr compiler::type_long_double(new type_primitive("long double", "ld", sizeof(long double), 220));
compiler::type_base_ptr compiler::type_long(new type_primitive("long int", "l", sizeof(long), 110));
compiler::type_base_ptr compiler::type_long_long(new type_primitive("long long int", "ll", sizeof(long), 120));
compiler::type_base_ptr compiler::type_bool(new type_primitive("bool", "b", sizeof(bool), 0));
compiler::type_base_ptr compiler::type_char(new type_primitive("char", "c", sizeof(char), 10));
compiler::type_base_ptr compiler::type_short(new type_primitive("short", "s", sizeof(short), 20));
compiler::type_base_ptr compiler::type_void(new type_primitive("void", "v", 0, -1));