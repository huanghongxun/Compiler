#pragma once

#include <typeinfo>
#include <string>
#include "instruction.h"
#include "type_primitive.h"
#include "type_pointer.h"
#include "type_array.h"

namespace compiler::instructions
{

	template<typename T>
	struct instruction_add : public instruction
	{
		void operate(function_environment& env) override
		{
			T op2 = env.stack->pop_value<T>();
			T op1 = env.stack->pop_value<T>();
			env.stack->emplace(op1 + op2);
		}
	};

	template<typename T>
	struct instruction_mul : public instruction
	{
		void operate(function_environment& env) override
		{
			T op2 = env.stack->pop_value<T>();
			T op1 = env.stack->pop_value<T>();
			env.stack->emplace(op1 * op2);
		}
	};

	template<typename T>
	struct instruction_sub : public instruction
	{
		void operate(function_environment& env) override
		{
			T op2 = env.stack->pop_value<T>();
			T op1 = env.stack->pop_value<T>();
			env.stack->emplace(op1 - op2);
		}
	};

	template<typename T>
	struct instruction_div : public instruction
	{
		void operate(function_environment& env) override
		{
			T op2 = env.stack->pop_value<T>();
			T op1 = env.stack->pop_value<T>();
			env.stack->emplace(op1 / op2);
		}
	};

	template<typename T>
	struct instruction_or : public instruction
	{
		void operate(function_environment& env) override
		{
			T op2 = env.stack->pop_value<T>();
			T op1 = env.stack->pop_value<T>();
			env.stack->emplace(op1 | op2);
		}
	};

	template<typename T>
	struct instruction_logical_or : public instruction
	{
		void operate(function_environment& env) override
		{
			T op2 = env.stack->pop_value<T>();
			T op1 = env.stack->pop_value<T>();
			env.stack->emplace(op1 || op2);
		}
	};

	template<typename T>
	struct instruction_and : public instruction
	{
		void operate(function_environment& env) override
		{
			T op2 = env.stack->pop_value<T>();
			T op1 = env.stack->pop_value<T>();
			env.stack->emplace(op1 & op2);
		}
	};

	template<typename T>
	struct instruction_logical_and : public instruction
	{
		void operate(function_environment& env) override
		{
			T op2 = env.stack->pop_value<T>();
			T op1 = env.stack->pop_value<T>();
			env.stack->emplace(op1 && op2);
		}
	};

	template<typename T>
	struct instruction_rem : public instruction
	{
		void operate(function_environment& env) override
		{
			T op2 = env.stack->pop_value<T>();
			T op1 = env.stack->pop_value<T>();
			env.stack->emplace(op1 % op2);
		}
	};

	template<typename T>
	struct instruction_eq : public instruction
	{
		void operate(function_environment& env) override
		{
			T op2 = env.stack->pop_value<T>();
			T op1 = env.stack->pop_value<T>();
			env.stack->emplace((int) (op1 == op2));
		}
	};

	template<typename T>
	struct instruction_neq : public instruction
	{
		void operate(function_environment& env) override
		{
			T op2 = env.stack->pop_value<T>();
			T op1 = env.stack->pop_value<T>();
			env.stack->emplace((int) (op1 != op2));
		}
	};

	template<typename T>
	struct instruction_leq : public instruction
	{
		void operate(function_environment& env) override
		{
			T op2 = env.stack->pop_value<T>();
			T op1 = env.stack->pop_value<T>();
			env.stack->emplace((int) (op1 <= op2));
		}
	};

	template<typename T>
	struct instruction_geq : public instruction
	{
		void operate(function_environment& env) override
		{
			T op2 = env.stack->pop_value<T>();
			T op1 = env.stack->pop_value<T>();
			env.stack->emplace((int) (op1 >= op2));
		}
	};

	template<typename T>
	struct instruction_less : public instruction
	{
		void operate(function_environment& env) override
		{
			T op2 = env.stack->pop_value<T>();
			T op1 = env.stack->pop_value<T>();
			env.stack->emplace((int) (op1 < op2));
		}
	};

	template<typename T>
	struct instruction_greater : public instruction
	{
		void operate(function_environment& env) override
		{
			T op2 = env.stack->pop_value<T>();
			T op1 = env.stack->pop_value<T>();
			env.stack->emplace((int) (op1 > op2));
		}
	};

	template<typename T>
	struct instruction_shl : public instruction
	{
		void operate(function_environment& env) override
		{
			T op2 = env.stack->pop_value<T>();
			T op1 = env.stack->pop_value<T>();
			env.stack->emplace(op1 << op2);
		}
	};

	template<typename T>
	struct instruction_shr : public instruction
	{
		void operate(function_environment& env) override
		{
			T op2 = env.stack->pop_value<T>();
			T op1 = env.stack->pop_value<T>();
			env.stack->emplace(op1 >> op2);
		}
	};

	template<typename T>
	struct instruction_xor : public instruction
	{
		void operate(function_environment& env) override
		{
			T op2 = env.stack->pop_value<T>();
			T op1 = env.stack->pop_value<T>();
			env.stack->emplace(op1 ^ op2);
		}
	};

	template<typename T>
	struct instruction_invert : public instruction
	{
		void operate(function_environment& env) override
		{
			T op1 = env.stack->pop_value<T>();
			env.stack->emplace(~op1);
		}
	};

	template<typename T>
	struct instruction_not : public instruction
	{
		void operate(function_environment& env) override
		{
			T op1 = env.stack->pop_value<T>();
			env.stack->emplace(!op1);
		}
	};

	template<typename T>
	struct instruction_neg : public instruction
	{
		void operate(function_environment& env) override
		{
			T op1 = env.stack->pop_value<T>();
			env.stack->emplace(-op1);
		}
	};

	template<typename T>
	struct instruction_dereference : public instruction
	{
		void operate(function_environment& env) override
		{
			auto op1 = env.stack->pop();
			void *value = op1.value.cast<void*>();

			if (op1.type.base_type->is_pointer())
			{
				type_pointer*p = dynamic_cast<type_pointer*>(op1.type.base_type.get());
				if (p->get_base_type()->is_pointer())
				{
					auto e = operand_stack_element(*reinterpret_cast<void**>(value), value);
					e.type = type_representation{ p->get_base_type(), p->is_const(), false };
					env.stack->push(e);
				}
				else if (p->get_base_type()->is_array())
				{
					auto e = operand_stack_element(value, value);
					type_array *p2 = dynamic_cast<type_array*>(p->get_base_type().get());
					e.type = type_representation{ p2->to_pointer(), false, false };
					env.stack->push(e);
				}
				else
				{
					T* op1 = reinterpret_cast<T*>(value);
					env.stack->push(operand_stack_element(*op1, op1));
				}
			}
			else
				throw runtime_error("Illegal state of operand stack");
		}
	};

	struct instruction_reference : public instruction
	{
		compiler::type_representation type;

		explicit instruction_reference(compiler::type_representation type);

		void operate(function_environment& env) override;
	};

	struct instruction_move : public instruction
	{
		bool move_positive;
		explicit instruction_move(bool move_positive);

		void operate(function_environment& env) override;
	};

	struct instruction_store : public instruction
	{
		void operate(function_environment& env) override;
	};

	template<typename T>
	struct instruction_load : public instruction
	{
		instruction_load(int index, bool is_static)
			: index(index), is_static(is_static)
		{
		}

		int index;
		bool is_static;

		void operate(function_environment& env) override
		{
			if (is_static)
				env.stack->push(operand_stack_element(*env.global->get<T>(index), env.global->get<T>(index)));
			else
				env.stack->push(operand_stack_element(*env.local->get<T>(index), env.local->get<T>(index)));
		}
	};

	struct instruction_load_array : public instruction
	{
		instruction_load_array(type_base_ptr type, int index, bool is_static);

		int index;
		bool is_static;
		type_base_ptr type;

		void operate(function_environment& env) override;
	};

	instruction_ptr get_instruction_load(compiler::type_representation type, int index);

	struct instruction_invoke : public instruction
	{
		explicit instruction_invoke(const std::string &name, int arg_num);

		const std::string name;
		int arg_num;

		void operate(function_environment& env) override;
	};

	template<typename T>
	struct instruction_constant : public instruction
	{
		explicit instruction_constant(const T &value)
			: value(value) {}

		T value;

		void operate(function_environment& env) override
		{
			env.stack->emplace(value);
		}
	};

	struct instruction_pop : public instruction
	{
		void operate(function_environment& env) override;
	};

	struct instruction_duplicate : public instruction
	{
		void operate(function_environment& env) override;
	};

	struct instruction_jump : public instruction
	{
		explicit instruction_jump(int to);

		int to;

		void operate(function_environment& env) override;
	};

	struct instruction_if : public instruction
	{
		explicit instruction_if(int to);

		int to;

		void operate(function_environment& env) override;
	};

	struct instruction_return : public instruction
	{
		void operate(function_environment& env) override;
	};

	template<typename _From, typename _To>
	struct instruction_primitive_cast : public instruction
	{
		int index;

		void operate(function_environment& env) override
		{
			env.stack->emplace((_To) env.stack->pop_value<_From>());
		}
	};

	struct instruction_pointer_cast : public instruction
	{
		type_representation from_type, to_type;

		instruction_pointer_cast(type_representation from_type, type_representation to_type);

		void operate(function_environment& env) override;
	};

}