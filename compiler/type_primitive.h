#pragma once

#include <string>
#include "type_base.h"

namespace compiler
{
	using namespace std;

	class type_primitive : public type_base
	{
		size_t sz;
		int priority;
	public:
		type_primitive(const string &name, const string &id, size_t sz, int priority)
			: type_base(name, id), sz(sz), priority(priority)
		{
		}

		bool is_assignable_from(type_base_ptr b) const override;
		bool can_be_cast_to(type_base_ptr to_type) const override;
		bool is_primitive() const override;
		bool is_integral() const override;
		bool is_void() const override;
		type_base_ptr implicit_cast_with(type_base_ptr a, type_base_ptr b) const override;
		size_t size() const override;
	};

	extern type_base_ptr type_int;
	extern type_base_ptr type_float;
	extern type_base_ptr type_double;
	extern type_base_ptr type_long_double;
	extern type_base_ptr type_long;
	extern type_base_ptr type_long_long;
	extern type_base_ptr type_bool;
	extern type_base_ptr type_char;
	extern type_base_ptr type_short;
	extern type_base_ptr type_void;
}
