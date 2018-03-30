#include "operand_stack.h"

compiler::operand_stack_element compiler::operand_stack::pop()
{
	operand_stack_element top = s.top();
	s.pop();
	return top;
}

void compiler::operand_stack::push(const operand_stack_element &element)
{
	s.push(element);
}

void compiler::operand_stack::push(operand_stack_element &&element)
{
	s.push(element);
}

void compiler::operand_stack::emplace(const object_t & value)
{
	s.emplace(value);
}

bool compiler::operand_stack::empty() const
{
	return s.empty();
}

compiler::operand_mistyped_error::operand_mistyped_error(const type_index &expected, const type_index &found)
	: exception((string("Operand mistyped. Expected ") + expected.name() + string(", but found ") + found.name()).c_str())
{
}

compiler::operand_stack_element::operand_stack_element(const object_t & val, void *const addr)
	: value(val), addr(addr)
{
}

void compiler::operand_stack_element::assert_type(const type_index& index)
{
	if (value.type() != index)
		throw operand_mistyped_error(index, value.type());
}
