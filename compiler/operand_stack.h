#pragma once

#include <stack>
#include "type_base.h"
#include "object_t.h"
using namespace std;

namespace compiler
{
	class operand_mistyped_error : exception
	{
	public:
		operand_mistyped_error(const type_index &expected, const type_index &found);
	};

	class operand_stack_element
	{
	public:
		operand_stack_element(const object_t &val, void *addr = nullptr);

		const object_t value;
		void *const addr;

		void assert_type(const type_index &index);
	};

	class operand_stack
	{
	private:
		stack<operand_stack_element> s;
	public:
		operand_stack_element pop();

		template<typename T>
		T pop_value()
		{
			auto element = pop();
			element.assert_type(typeid(T));
			return element.value.cast<T>();
		}

		void push(const operand_stack_element &);
		void push(operand_stack_element &&);
		void emplace(const object_t &value);
		bool empty() const;
	};

}